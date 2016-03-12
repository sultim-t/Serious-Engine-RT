/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "stdh.h"

#include <Engine/Base/Statistics_internal.h>
#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/RenderPoly.h>
#include <Engine/Graphics/Color.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/GfxProfile.h>

// asm shortcuts
#define O offset
#define Q qword ptr
#define D dword ptr
#define W  word ptr
#define B  byte ptr

extern INDEX tex_bProgressiveFilter; // filter mipmaps in creation time (not afterwards)


// returns number of mip-maps to skip from original texture
INDEX ClampTextureSize( PIX pixClampSize, PIX pixClampDimension, PIX pixSizeU, PIX pixSizeV)
{
  __int64 pixMaxSize  = (__int64)pixSizeU * (__int64)pixSizeV;
  PIX pixMaxDimension = Max( pixSizeU, pixSizeV);
  INDEX ctSkipMips    = 0;
  while( (pixMaxSize>pixClampSize || pixMaxDimension>pixClampDimension) && pixMaxDimension>1) {
    ctSkipMips++;
    pixMaxDimension >>=1;
    pixMaxSize >>=2;
  }
  return ctSkipMips;
}


// retrives memory offset of a specified mip-map or a size of all mip-maps (IN PIXELS!)
// (zero offset means first, i.e. largest mip-map)
PIX GetMipmapOffset( INDEX iMipLevel, PIX pixWidth, PIX pixHeight)
{
  PIX pixTexSize = 0;
  PIX pixMipSize = pixWidth*pixHeight;
  INDEX iMips = GetNoOfMipmaps( pixWidth, pixHeight);
  iMips = Min( iMips, iMipLevel);
  while( iMips>0) {
    pixTexSize +=pixMipSize;
    pixMipSize>>=2;
    iMips--;
  }
  return pixTexSize;
}


// return offset, pointer and dimensions of mipmap of specified size inside texture or shadowmap mipmaps
INDEX GetMipmapOfSize( PIX pixWantedSize, ULONG *&pulFrame, PIX &pixWidth, PIX &pixHeight)
{
  INDEX iMipOffset = 0;
  while( pixWidth>1 && pixHeight>1) {
    const PIX pixCurrentSize = pixWidth*pixHeight;
    if( pixCurrentSize <= pixWantedSize) break; // found
    pulFrame += pixCurrentSize;
    pixWidth >>=1;
    pixHeight>>=1;
    iMipOffset++;
  } // done
  return iMipOffset;
}


// adds 8-bit opaque alpha channel to 24-bit bitmap (in place supported)
void AddAlphaChannel( UBYTE *pubSrcBitmap, ULONG *pulDstBitmap, PIX pixSize, UBYTE *pubAlphaBitmap)
{
  UBYTE ubR,ubG,ubB, ubA=255;
  // loop backwards thru all bitmap pixels
  for( INDEX iPix=(pixSize-1); iPix>=0; iPix--) {
    ubR = pubSrcBitmap[iPix*3 +0];
    ubG = pubSrcBitmap[iPix*3 +1];
    ubB = pubSrcBitmap[iPix*3 +2];
    if( pubAlphaBitmap!=NULL) ubA = pubAlphaBitmap[iPix];
    else ubA = 255; // for the sake of forced RGBA internal formats!
    pulDstBitmap[iPix] = ByteSwap( RGBAToColor( ubR,ubG,ubB, ubA));
  }
}

// removes 8-bit alpha channel from 32-bit bitmap (in place supported)
void RemoveAlphaChannel( ULONG *pulSrcBitmap, UBYTE *pubDstBitmap, PIX pixSize)
{
  UBYTE ubR,ubG,ubB;
  // loop thru all bitmap pixels
  for( INDEX iPix=0; iPix<pixSize; iPix++) {
    ColorToRGB( ByteSwap( pulSrcBitmap[iPix]), ubR,ubG,ubB);
    pubDstBitmap[iPix*3 +0] = ubR;
    pubDstBitmap[iPix*3 +1] = ubG;
    pubDstBitmap[iPix*3 +2] = ubB;
  }
}



// flips 24 or 32-bit bitmap (iType: 1-horizontal, 2-vertical, 3-diagonal) - in place supported
void FlipBitmap( UBYTE *pubSrc, UBYTE *pubDst, PIX pixWidth, PIX pixHeight, INDEX iFlipType, BOOL bAlphaChannel)
{
  // safety
  ASSERT( iFlipType>=0 && iFlipType<4);
  // no flipping ?
  PIX pixSize = pixWidth*pixHeight;
  if( iFlipType==0) {
    // copy bitmap only if needed
    INDEX ctBPP = (bAlphaChannel ? 4 : 3);
    if( pubSrc!=pubDst) memcpy( pubDst, pubSrc, pixSize*ctBPP);
    return;
  }

  // prepare images without alpha channels
  ULONG *pulNew = NULL;
  ULONG *pulNewSrc = (ULONG*)pubSrc;
  ULONG *pulNewDst = (ULONG*)pubDst;
  if( !bAlphaChannel) {
    pulNew = (ULONG*)AllocMemory( pixSize *BYTES_PER_TEXEL);
    AddAlphaChannel( pubSrc, pulNew, pixSize);
    pulNewSrc = pulNew;
    pulNewDst = pulNew;
  }

  // prepare half-width and half-height rounded
  const PIX pixHalfWidth  = (pixWidth+1) /2;
  const PIX pixHalfHeight = (pixHeight+1)/2;

  // flip horizontal
  if( iFlipType==2 || iFlipType==3)
  { // for each row
    for( INDEX iRow=0; iRow<pixHeight; iRow++)
    { // find row pointer
      PIX pixRowOffset = iRow*pixWidth;
      // for each pixel in row
      for( INDEX iPix=0; iPix<pixHalfWidth; iPix++)
      { // transfer pixels
        PIX pixBeg = pulNewSrc[pixRowOffset+iPix];
        PIX pixEnd = pulNewSrc[pixRowOffset+(pixWidth-1-iPix)];
        pulNewDst[pixRowOffset+iPix]              = pixEnd;
        pulNewDst[pixRowOffset+(pixWidth-1-iPix)] = pixBeg;
      }
    }
  }

  // prepare new pointers
  if( iFlipType==3) pulNewSrc = pulNewDst;

  // flip vertical/diagonal
  if( iFlipType==1 || iFlipType==3)
  { // for each row
    for( INDEX iRow=0; iRow<pixHalfHeight; iRow++)
    { // find row pointers
      PIX pixBegOffset = iRow*pixWidth;
      PIX pixEndOffset = (pixHeight-1-iRow)*pixWidth;
      // for each pixel in row
      for( INDEX iPix=0; iPix<pixWidth; iPix++)
      { // transfer pixels
        PIX pixBeg = pulNewSrc[pixBegOffset+iPix];
        PIX pixEnd = pulNewSrc[pixEndOffset+iPix];
        pulNewDst[pixBegOffset+iPix] = pixEnd;
        pulNewDst[pixEndOffset+iPix] = pixBeg;
      }
    }
  }

  // postpare images without alpha channels
  if( !bAlphaChannel) {
    RemoveAlphaChannel( pulNewDst, pubDst, pixSize);
    if( pulNew!=NULL) FreeMemory(pulNew);
  }
}



// makes one level lower mipmap (bilinear or nearest-neighbour with border preservance)
static __int64 mmRounder = 0x0002000200020002;
static void MakeOneMipmap( ULONG *pulSrcMipmap, ULONG *pulDstMipmap, PIX pixWidth, PIX pixHeight, BOOL bBilinear)
{
  // some safety checks
  ASSERT( pixWidth>1 && pixHeight>1);
  ASSERT( pixWidth  == 1L<<FastLog2(pixWidth));
  ASSERT( pixHeight == 1L<<FastLog2(pixHeight));
  pixWidth >>=1;
  pixHeight>>=1;

  if( bBilinear) // type of filtering?
  { // BILINEAR
    __asm {
      pxor    mm0,mm0
      mov     ebx,D [pixWidth]
      mov     esi,D [pulSrcMipmap]
      mov     edi,D [pulDstMipmap]
      mov     edx,D [pixHeight]
rowLoop:
      mov     ecx,D [pixWidth]
pixLoopN:           
      movd    mm1,D [esi+ 0]        // up-left
      movd    mm2,D [esi+ 4]        // up-right
      movd    mm3,D [esi+ ebx*8 +0] // down-left
      movd    mm4,D [esi+ ebx*8 +4] // down-right
      punpcklbw mm1,mm0
      punpcklbw mm2,mm0
      punpcklbw mm3,mm0
      punpcklbw mm4,mm0
      paddw   mm1,mm2
      paddw   mm1,mm3
      paddw   mm1,mm4
      paddw   mm1,Q [mmRounder]
      psrlw   mm1,2
      packuswb mm1,mm0
      movd    D [edi],mm1
      // advance to next pixel
      add     esi,4*2
      add     edi,4
      dec     ecx
      jnz     pixLoopN
      // advance to next row
      lea     esi,[esi+ ebx*8] // skip one row in source mip-map
      dec     edx
      jnz     rowLoop
      emms
    }
  }
  else
  { // NEAREST-NEIGHBOUR but with border preserving
    ULONG ulRowModulo = pixWidth*2 *BYTES_PER_TEXEL;
    __asm {   
      xor     ebx,ebx
      mov     esi,D [pulSrcMipmap]
      mov     edi,D [pulDstMipmap]
      // setup upper half
      mov     edx,D [pixHeight]
      shr     edx,1
halfLoop:
      mov     ecx,D [pixWidth]
      shr     ecx,1
leftLoop:
      mov     eax,D [esi+ ebx*8+ 0] // upper-left (or lower-left)
      mov     D [edi],eax
      // advance to next pixel
      add     esi,4*2
      add     edi,4
      sub     ecx,1
      jg      leftLoop
      // do right row half
      mov     ecx,D [pixWidth]
      shr     ecx,1
      jz      halfEnd
rightLoop:
      mov     eax,D [esi+ ebx*8+ 4] // upper-right (or lower-right)
      mov     D [edi],eax
      // advance to next pixel
      add     esi,4*2
      add     edi,4
      sub     ecx,1
      jg      rightLoop
halfEnd:
      // advance to next row
      add     esi,D [ulRowModulo]  // skip one row in source mip-map
      sub     edx,1
      jg      halfLoop
      // do eventual lower half loop (if not yet done)
      mov     edx,D [pixHeight]
      shr     edx,1
      jz      fullEnd
      cmp     ebx,D [pixWidth]
      mov     ebx,D [pixWidth]
      jne     halfLoop
fullEnd:
    }
  }
}


// makes ALL lower mipmaps (to size of 1x1!) of a specified 32-bit bitmap
// and returns pointer to newely created and mipmaped image
// (only first ctFineMips number of mip-maps will be filtered with bilinear subsampling, while
//  all others will be downsampled with nearest-neighbour method)
void MakeMipmaps( INDEX ctFineMips, ULONG *pulMipmaps, PIX pixWidth, PIX pixHeight, INDEX iFilter/*=NONE*/)
{
  ASSERT( pixWidth>0 && pixHeight>0);
  _pfGfxProfile.StartTimer( CGfxProfile::PTI_MAKEMIPMAPS);

  // prepare some variables
  INDEX ctMipmaps = 1;
  PIX pixTexSize  = 0;
  PIX pixCurrWidth  = pixWidth;
  PIX pixCurrHeight = pixHeight;
  ULONG *pulSrcMipmap, *pulDstMipmap;

  // determine filtering mode (-1=prefiltering, 0=none, 1=postfiltering)
  INDEX iFilterMode = 0;
  if( iFilter!=0) {
    iFilterMode = -1;
    if( !tex_bProgressiveFilter) iFilterMode = +1;
  }

  // loop thru mip-map levels
  while( pixCurrWidth>1 && pixCurrHeight>1)
  { // determine mip size
    PIX pixMipSize = pixCurrWidth*pixCurrHeight;
    pulSrcMipmap   = pulMipmaps   + pixTexSize;
    pulDstMipmap   = pulSrcMipmap + pixMipSize;
    // do pre filter is required
    if( iFilterMode<0) FilterBitmap( iFilter, pulSrcMipmap, pulSrcMipmap, pixCurrWidth, pixCurrHeight);
    // create one mipmap
    MakeOneMipmap( pulSrcMipmap, pulDstMipmap, pixCurrWidth, pixCurrHeight, ctMipmaps<ctFineMips);
    // do post filter if required
    if( iFilterMode>0) FilterBitmap( iFilter, pulSrcMipmap, pulSrcMipmap, pixCurrWidth, pixCurrHeight);
    // advance to next mipmap
    pixTexSize += pixMipSize;
    pixCurrWidth  >>=1;
    pixCurrHeight >>=1;
    ctMipmaps++;
  }
  // all done
  _pfGfxProfile.StopTimer( CGfxProfile::PTI_MAKEMIPMAPS);
}


// mipmap colorization table (from 1024 to 1)
static COLOR _acolMips[10] = { C_RED, C_GREEN, C_BLUE, C_CYAN, C_MAGENTA, C_YELLOW, C_RED, C_GREEN, C_BLUE, C_WHITE };

// colorize mipmaps
void ColorizeMipmaps( INDEX i1stMipmapToColorize, ULONG *pulMipmaps, PIX pixWidth, PIX pixHeight)
{
  // prepare ...
  ULONG *pulSrcMipmap = pulMipmaps + GetMipmapOffset( i1stMipmapToColorize, pixWidth, pixHeight);
  ULONG *pulDstMipmap;
  PIX pixCurrWidth  = pixWidth >>i1stMipmapToColorize;
  PIX pixCurrHeight = pixHeight>>i1stMipmapToColorize;
  PIX pixMipSize;
  // skip too large textures
  const PIX pixMaxDim = Max( pixCurrWidth, pixCurrHeight);
  if( pixMaxDim>1024) return;
  INDEX iTableOfs = 10-FastLog2(pixMaxDim);

  // loop thru mip-map levels
  while( pixCurrWidth>1 && pixCurrHeight>1)
  { // prepare current mip-level
    pixMipSize   = pixCurrWidth*pixCurrHeight;
    pulDstMipmap = pulSrcMipmap + pixMipSize;
    // mask mipmap
    const ULONG ulColorMask = ByteSwap( _acolMips[iTableOfs] | 0x3F3F3FFF);
    for( INDEX iPix=0; iPix<pixMipSize; iPix++) pulSrcMipmap[iPix] &= ulColorMask;
    // advance to next mipmap
    pulSrcMipmap += pixMipSize;
    pixCurrWidth  >>=1;
    pixCurrHeight >>=1;
    iTableOfs++;
  }
}



// calculates standard deviation of a bitmap
DOUBLE CalcBitmapDeviation( ULONG *pulBitmap, PIX pixSize)
{
  UBYTE ubR,ubG,ubB;
  ULONG ulSumR =0, ulSumG =0, ulSumB =0;
__int64 mmSumR2=0, mmSumG2=0, mmSumB2=0;

  // calculate sum and sum^2
  for( INDEX iPix=0; iPix<pixSize; iPix++) {
    ColorToRGB( ByteSwap(pulBitmap[iPix]), ubR,ubG,ubB);
    ulSumR  += ubR;      ulSumG  += ubG;      ulSumB  += ubB;
    mmSumR2 += ubR*ubR;  mmSumG2 += ubG*ubG;  mmSumB2 += ubB*ubB;
  }

  // calculate deviation of each channel
  DOUBLE d1oSize   = 1.0 / (DOUBLE) pixSize;
  DOUBLE d1oSizeM1 = 1.0 / (DOUBLE)(pixSize-1);
  DOUBLE dAvgR = (DOUBLE)ulSumR *d1oSize;
  DOUBLE dAvgG = (DOUBLE)ulSumG *d1oSize;
  DOUBLE dAvgB = (DOUBLE)ulSumB *d1oSize;
  DOUBLE dDevR = Sqrt( ((DOUBLE)mmSumR2 - 2*ulSumR*dAvgR + pixSize*dAvgR*dAvgR) *d1oSizeM1);
  DOUBLE dDevG = Sqrt( ((DOUBLE)mmSumG2 - 2*ulSumG*dAvgG + pixSize*dAvgG*dAvgG) *d1oSizeM1);
  DOUBLE dDevB = Sqrt( ((DOUBLE)mmSumB2 - 2*ulSumB*dAvgB + pixSize*dAvgB*dAvgB) *d1oSizeM1);

  // return maximum deviation
  return Max( Max( dDevR, dDevG), dDevB);
}





// DITHERING ROUTINES

// dither tables
static ULONG ulDither4[4][4] = {
  { 0x0F0F0F0F, 0x07070707, 0x0D0D0D0D, 0x05050505 }, 
  { 0x03030303, 0x0B0B0B0B, 0x01010101, 0x09090909 },
  { 0x0C0C0C0C, 0x04040404, 0x0E0E0E0E, 0x06060606 },
  { 0x00000000, 0x08080808, 0x02020202, 0x0A0A0A0A }
};
static ULONG ulDither3[4][4] = {
  { 0x06060606, 0x02020202, 0x06060606, 0x02020202 }, 
  { 0x00000000, 0x04040404, 0x00000000, 0x04040404 },
  { 0x06060606, 0x02020202, 0x06060606, 0x02020202 }, 
  { 0x00000000, 0x04040404, 0x00000000, 0x04040404 },
};
static ULONG ulDither2[4][4] = {
  { 0x02020202, 0x06060606, 0x02020202, 0x06060606 },
  { 0x06060606, 0x02020202, 0x06060606, 0x02020202 },
  { 0x02020202, 0x06060606, 0x02020202, 0x06060606 },
  { 0x06060606, 0x02020202, 0x06060606, 0x02020202 },
};


static __int64 mmErrDiffMask=0;
static __int64 mmW3 = 0x0003000300030003;
static __int64 mmW5 = 0x0005000500050005;
static __int64 mmW7 = 0x0007000700070007;
static __int64 mmShift = 0;
static __int64 mmMask  = 0;
static ULONG *pulDitherTable;

// performs dithering of a 32-bit bipmap (can be in-place)
void DitherBitmap( INDEX iDitherType, ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight,
                   PIX pixCanvasWidth, PIX pixCanvasHeight)
{
  _pfGfxProfile.StartTimer( CGfxProfile::PTI_DITHERBITMAP);

  // determine row modulo
  if( pixCanvasWidth ==0) pixCanvasWidth  = pixWidth;
  if( pixCanvasHeight==0) pixCanvasHeight = pixHeight;
  ASSERT( pixCanvasWidth>=pixWidth && pixCanvasHeight>=pixHeight);
  SLONG slModulo      = (pixCanvasWidth-pixWidth) *BYTES_PER_TEXEL;
  SLONG slWidthModulo = pixWidth*BYTES_PER_TEXEL +slModulo;

  // if bitmap is smaller than 4x2 pixels
  if( pixWidth<4 || pixHeight<2)
  { // don't dither it at all, rather copy only (if needed)
    if( pulDst!=pulSrc) memcpy( pulDst, pulSrc, pixCanvasWidth*pixCanvasHeight *BYTES_PER_TEXEL);
    goto theEnd;
  }

  // determine proper dither type
  switch( iDitherType)
  { // low dithers
  case 1:
    pulDitherTable = &ulDither2[0][0];
    mmShift = 2;
    mmMask  = 0x3F3F3F3F3F3F3F3F;
    goto ditherOrder;
  case 2:
    pulDitherTable = &ulDither2[0][0];
    mmShift = 1;
    mmMask  = 0x7F7F7F7F7F7F7F7F;
    goto ditherOrder;
  case 3:
    mmErrDiffMask = 0x0003000300030003;
    goto ditherError;
  // medium dithers
  case 4:
    pulDitherTable = &ulDither2[0][0];
    mmShift = 0;
    mmMask  = 0xFFFFFFFFFFFFFFFF;
    goto ditherOrder;
  case 5:
    pulDitherTable = &ulDither3[0][0];
    mmShift = 1;
    mmMask  = 0x7F7F7F7F7F7F7F7F;
    goto ditherOrder;
  case 6:
    pulDitherTable = &ulDither4[0][0];
    mmShift = 1;
    mmMask  = 0x7F7F7F7F7F7F7F7F;
    goto ditherOrder;
  case 7:
    mmErrDiffMask = 0x0007000700070007;
    goto ditherError;
  // high dithers
  case 8:
    pulDitherTable = &ulDither3[0][0];
    mmShift = 0;
    mmMask  = 0xFFFFFFFFFFFFFFFF;
    goto ditherOrder;
  case 9:
    pulDitherTable = &ulDither4[0][0];
    mmShift = 0;
    mmMask  = 0xFFFFFFFFFFFFFFFF;
    goto ditherOrder;
  case 10:
    mmErrDiffMask = 0x000F000F000F000F;
    goto ditherError;
  default:
    // improper dither type
    ASSERTALWAYS( "Improper dithering type.");
    // if bitmap copying is needed
    if( pulDst!=pulSrc) memcpy( pulDst, pulSrc, pixCanvasWidth*pixCanvasHeight *BYTES_PER_TEXEL);
    goto theEnd;
  }

// ------------------------------- ordered matrix dithering routine

ditherOrder:
  __asm {
    mov     esi,D [pulSrc]
    mov     edi,D [pulDst]
    mov     ebx,D [pulDitherTable]
    // reset dither line offset
    xor     eax,eax
    mov     edx,D [pixHeight]
rowLoopO:
    // get horizontal dither patterns
    movq    mm4,Q [ebx+ eax*4 +0]
    movq    mm5,Q [ebx+ eax*4 +8]
    psrlw   mm4,Q [mmShift]
    psrlw   mm5,Q [mmShift]
    pand    mm4,Q [mmMask]
    pand    mm5,Q [mmMask]
    // process row
    mov     ecx,D [pixWidth]
pixLoopO:
    movq    mm1,Q [esi +0]
    movq    mm2,Q [esi +8]
    paddusb mm1,mm4
    paddusb mm2,mm5
    movq    Q [edi +0],mm1
    movq    Q [edi +8],mm2
    // advance to next pixel
    add     esi,4*4
    add     edi,4*4
    sub     ecx,4
    jg      pixLoopO  // !!!! possible memory leak?
    je      nextRowO
    // backup couple of pixels
    lea     esi,[esi+ ecx*4]
    lea     edi,[edi+ ecx*4]
nextRowO:
    // get next dither line patterns
    add     esi,D [slModulo]
    add     edi,D [slModulo]
    add     eax,1*4
    and     eax,4*4-1
    // advance to next row
    dec     edx
    jnz     rowLoopO
    emms;
  }
  goto theEnd;

// ------------------------------- error diffusion dithering routine

ditherError:
  // since error diffusion algorithm requires in-place dithering, original bitmap must be copied if needed
  if( pulDst!=pulSrc) memcpy( pulDst, pulSrc, pixCanvasWidth*pixCanvasHeight *BYTES_PER_TEXEL);
  // slModulo+=4;
  // now, dither destination
  __asm {
    pxor    mm0,mm0
    mov     esi,D [pulDst]
    mov     ebx,D [pixCanvasWidth]
    mov     edx,D [pixHeight]
    dec     edx // need not to dither last row
rowLoopE:
    // left to right
    mov     ecx,D [pixWidth]
    dec     ecx
pixLoopEL:
    movd    mm1,D [esi]
    punpcklbw mm1,mm0
    pand    mm1,Q [mmErrDiffMask] 
    // determine errors
    movq    mm3,mm1
    movq    mm5,mm1
    movq    mm7,mm1
    pmullw  mm3,Q [mmW3]
    pmullw  mm5,Q [mmW5]
    pmullw  mm7,Q [mmW7]
    psrlw   mm3,4     // *3/16
    psrlw   mm5,4     // *5/16
    psrlw   mm7,4     // *7/16
    psubw   mm1,mm3
    psubw   mm1,mm5
    psubw   mm1,mm7   // *rest/16
    packuswb mm1,mm0
    packuswb mm3,mm0
    packuswb mm5,mm0
    packuswb mm7,mm0
    // spread errors
    paddusb mm7,Q [esi+       +4]
    paddusb mm3,Q [esi+ ebx*4 -4]
    paddusb mm5,Q [esi+ ebx*4 +0]
    paddusb mm1,Q [esi+ ebx*4 +4]  // !!!! possible memory leak?
    movd    D [esi+       +4],mm7
    movd    D [esi+ ebx*4 -4],mm3
    movd    D [esi+ ebx*4 +0],mm5
    movd    D [esi+ ebx*4 +4],mm1
    // advance to next pixel
    add     esi,4
    dec     ecx
    jnz     pixLoopEL
    // advance to next row
    add     esi,D [slWidthModulo]
    dec     edx
    jz      allDoneE

    // right to left
    mov     ecx,D [pixWidth]
    dec     ecx
pixLoopER:
    movd    mm1,D [esi]
    punpcklbw mm1,mm0
    pand    mm1,Q [mmErrDiffMask] 
    // determine errors
    movq    mm3,mm1
    movq    mm5,mm1
    movq    mm7,mm1
    pmullw  mm3,Q [mmW3]
    pmullw  mm5,Q [mmW5]
    pmullw  mm7,Q [mmW7]
    psrlw   mm3,4     // *3/16
    psrlw   mm5,4     // *5/16
    psrlw   mm7,4     // *7/16
    psubw   mm1,mm3
    psubw   mm1,mm5
    psubw   mm1,mm7   // *rest/16
    packuswb mm1,mm0
    packuswb mm3,mm0
    packuswb mm5,mm0
    packuswb mm7,mm0
    // spread errors
    paddusb mm7,Q [esi+       -4]
    paddusb mm1,Q [esi+ ebx*4 -4]
    paddusb mm5,Q [esi+ ebx*4 +0]
    paddusb mm3,Q [esi+ ebx*4 +4]   // !!!! possible memory leak?
    movd    D [esi+       -4],mm7
    movd    D [esi+ ebx*4 -4],mm1
    movd    D [esi+ ebx*4 +0],mm5
    movd    D [esi+ ebx*4 +4],mm3
    // revert to previous pixel
    sub     esi,4
    dec     ecx
    jnz     pixLoopER
    // advance to next row
    lea     esi,[esi+ ebx*4]
    dec     edx
    jnz     rowLoopE
allDoneE:
    emms;
  }
  goto theEnd;

  // all done
theEnd:
  _pfGfxProfile.StopTimer( CGfxProfile::PTI_DITHERBITMAP);
}



// performs dithering of a 32-bit mipmaps (can be in-place)
void DitherMipmaps( INDEX iDitherType, ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight)
{
  // safety check
  ASSERT( pixWidth>0 && pixHeight>0);
  // loop thru mipmaps
  PIX pixMipSize;
  while( pixWidth>0 && pixHeight>0)
  { // dither one mipmap
    DitherBitmap( iDitherType, pulSrc, pulDst, pixWidth, pixHeight);
    // advance to next mipmap
    pixMipSize = pixWidth*pixHeight;
    pulSrc += pixMipSize;
    pulDst += pixMipSize;
    pixWidth >>=1;
    pixHeight>>=1;
  }
}



// blur/sharpen filters
static INDEX aiFilters[6][3] = {
  {  0,  1, 16 },  // minimum
  {  0,  2,  8 },  // low
  {  1,  2,  7 },  // medium
  {  1,  2,  3 },  // high
  {  3,  4,  5 },  // maximum
  {  1,  1,  1 }}; // 

// temp for middle pixels, vertical/horizontal edges, and corners
static __int64 mmMc,  mmMe,  mmMm;  // corner, edge, middle
static __int64 mmEch, mmEm;  // corner-high, middle
#define mmEcl mmMc  // corner-low
#define mmEe  mmMe  // edge
static __int64 mmCm;  // middle
#define mmCc mmMc  // corner
#define mmCe mmEch // edge
static __int64 mmInvDiv;
static __int64 mmAdd = 0x0007000700070007;

// temp rows for in-place filtering support
static ULONG aulRows[2048];


// FilterBitmap() INTERNAL: generates convolution filter matrix if needed
static INDEX iLastFilter;
static void GenerateConvolutionMatrix( INDEX iFilter)
{
  // same as last?
  if( iLastFilter==iFilter) return;
  // update filter
  iLastFilter = iFilter;
  INDEX iFilterAbs = Abs(iFilter) -1;
  // convert convolution values to MMX format
  INDEX iMc = aiFilters[iFilterAbs][0];  // corner
  INDEX iMe = aiFilters[iFilterAbs][1];  // edge
  INDEX iMm = aiFilters[iFilterAbs][2];  // middle
  // negate values for sharpen filter case
  if( iFilter<0) {
    iMm += (iMe+iMc) *8;  // (4*Edge + 4*Corner) *2
    iMe  = -iMe;
    iMc  = -iMc;
  }
  // find values for edge and corner cases
  INDEX iEch = iMc  + iMe;
  INDEX iEm  = iMm  + iMe;
  INDEX iCm  = iEch + iEm;
  // prepare divider
  __int64 mm = ((__int64)ceil(65536.0f/(iMc*4+iMe*4+iMm))) & 0xFFFF;
  mmInvDiv   = (mm<<48) | (mm<<32) | (mm<<16) | mm;
  // prepare filter values
  mm = iMc  & 0xFFFF;  mmMc = (mm<<48) | (mm<<32) | (mm<<16) | mm;
  mm = iMe  & 0xFFFF;  mmMe = (mm<<48) | (mm<<32) | (mm<<16) | mm;
  mm = iMm  & 0xFFFF;  mmMm = (mm<<48) | (mm<<32) | (mm<<16) | mm;
  mm = iEch & 0xFFFF;  mmEch= (mm<<48) | (mm<<32) | (mm<<16) | mm;
  mm = iEm  & 0xFFFF;  mmEm = (mm<<48) | (mm<<32) | (mm<<16) | mm;
  mm = iCm  & 0xFFFF;  mmCm = (mm<<48) | (mm<<32) | (mm<<16) | mm;
}

 
// applies filter to bitmap
void FilterBitmap( INDEX iFilter, ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight,
                   PIX pixCanvasWidth, PIX pixCanvasHeight)
{
  _pfGfxProfile.StartTimer( CGfxProfile::PTI_FILTERBITMAP);
  ASSERT( iFilter>=-6 && iFilter<=+6);

  // adjust canvas size
  if( pixCanvasWidth ==0) pixCanvasWidth  = pixWidth;
  if( pixCanvasHeight==0) pixCanvasHeight = pixHeight;
  ASSERT( pixCanvasWidth>=pixWidth && pixCanvasHeight>=pixHeight);

  // if bitmap is smaller than 4x4
  if( pixWidth<4 || pixHeight<4)
  { // don't blur it at all, but eventually only copy
    if( pulDst!=pulSrc) memcpy( pulDst, pulSrc, pixCanvasWidth*pixCanvasHeight *BYTES_PER_TEXEL);
    _pfGfxProfile.StopTimer( CGfxProfile::PTI_FILTERBITMAP);
    return;
  }

  // prepare convolution matrix and row modulo
  iFilter = Clamp( iFilter, -6L, +6L);
  GenerateConvolutionMatrix( iFilter);
  SLONG slModulo1 = (pixCanvasWidth-pixWidth+1) *BYTES_PER_TEXEL;
  SLONG slCanvasWidth = pixCanvasWidth *BYTES_PER_TEXEL;

  // lets roll ...
  __asm {
    cld
    mov     eax,D [pixCanvasWidth] // EAX = positive row offset
    mov     edx,eax
    neg     edx                    // EDX = negative row offset
    pxor    mm0,mm0
    mov     esi,D [pulSrc]
    mov     edi,D [pulDst]
    xor     ebx,ebx

// ----------------------- process upper left corner

    movd    mm1,D [esi+       +0]
    movd    mm2,D [esi+       +4]
    movd    mm3,D [esi+ eax*4 +0]
    movd    mm4,D [esi+ eax*4 +4]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    paddw   mm2,mm3
    pmullw  mm1,Q [mmCm]
    pmullw  mm2,Q [mmCe]
    pmullw  mm4,Q [mmCc]
    paddw   mm1,mm2
    paddw   mm1,mm4
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    D [ebx+ aulRows],mm1
    add     esi,4
    add     ebx,4

// ----------------------- process upper edge pixels

    mov     ecx,D [pixWidth]
    sub     ecx,2
    // for each pixel
upperLoop:      
    movd    mm1,D [esi+       -4]
    movd    mm2,D [esi+       +0]
    movd    mm3,D [esi+       +4]
    movd    mm4,D [esi+ eax*4 -4]
    movd    mm5,D [esi+ eax*4 +0]
    movd    mm6,D [esi+ eax*4 +4]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    punpcklbw mm5,mm0
    punpcklbw mm6,mm0
    paddw   mm1,mm3
    paddw   mm4,mm6
    pmullw  mm1,Q [mmEch]
    pmullw  mm2,Q [mmEm]
    pmullw  mm4,Q [mmEcl]
    pmullw  mm5,Q [mmEe]
    paddw   mm1,mm2
    paddw   mm1,mm4
    paddw   mm1,mm5
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    D [ebx+ aulRows],mm1
    // advance to next pixel
    add     esi,4
    add     ebx,4
    dec     ecx
    jnz     upperLoop

// ----------------------- process upper right corner

    movd    mm1,D [esi+       -4]
    movd    mm2,D [esi+       +0]
    movd    mm3,D [esi+ eax*4 -4]
    movd    mm4,D [esi+ eax*4 +0]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    paddw   mm1,mm4
    pmullw  mm1,Q [mmCe]
    pmullw  mm2,Q [mmCm]
    pmullw  mm3,Q [mmCc]
    paddw   mm1,mm2
    paddw   mm1,mm3
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    D [ebx+ aulRows],mm1

// ----------------------- process bitmap middle pixels

    add     esi,D [slModulo1]
    add     edi,D [slCanvasWidth]
    mov     ebx,D [pixHeight]
    sub     ebx,2
    // for each row
rowLoop:      
    push    ebx
    xor     ebx,ebx
    // process left edge pixel
    movd    mm1,D [esi+ edx*4 +0]
    movd    mm2,D [esi+ edx*4 +4]
    movd    mm3,D [esi+       +0]
    movd    mm4,D [esi+       +4]
    movd    mm5,D [esi+ eax*4 +0]
    movd    mm6,D [esi+ eax*4 +4]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    punpcklbw mm5,mm0
    punpcklbw mm6,mm0
    paddw   mm1,mm5
    paddw   mm2,mm6
    pmullw  mm1,Q [mmEch]
    pmullw  mm2,Q [mmEcl]
    pmullw  mm3,Q [mmEm]
    pmullw  mm4,Q [mmEe]
    paddw   mm1,mm2
    paddw   mm1,mm3
    paddw   mm1,mm4
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    mm2,D [ebx+ aulRows]
    movd    D [ebx+ aulRows],mm1
    movd    D [edi+ edx*4],mm2
    add     esi,4
    add     edi,4
    add     ebx,4

    // for each pixel in current row
    mov     ecx,D [pixWidth]
    sub     ecx,2
pixLoop:
    // prepare upper convolution row
    movd    mm1,D [esi+ edx*4 -4]
    movd    mm2,D [esi+ edx*4 +0]
    movd    mm3,D [esi+ edx*4 +4]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    // prepare middle convolution row
    movd    mm4,D [esi+ -4]
    movd    mm5,D [esi+ +0]
    movd    mm6,D [esi+ +4]
    punpcklbw mm4,mm0
    punpcklbw mm5,mm0
    punpcklbw mm6,mm0
    // free some registers
    paddw   mm1,mm3
    paddw   mm2,mm4
    pmullw  mm5,Q [mmMm]
    // prepare lower convolution row
    movd    mm3,D [esi+ eax*4 -4]
    movd    mm4,D [esi+ eax*4 +0]
    movd    mm7,D [esi+ eax*4 +4]
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    punpcklbw mm7,mm0
    // calc weightened value
    paddw   mm2,mm6
    paddw   mm1,mm3
    paddw   mm2,mm4
    paddw   mm1,mm7
    pmullw  mm2,Q [mmMe]
    pmullw  mm1,Q [mmMc]
    paddw   mm2,mm5
    paddw   mm1,mm2
    // calc and store wightened value
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    mm2,D [ebx+ aulRows]
    movd    D [ebx+ aulRows],mm1
    movd    D [edi+ edx*4],mm2
    // advance to next pixel
    add     esi,4
    add     edi,4
    add     ebx,4
    dec     ecx
    jnz     pixLoop

    // process right edge pixel
    movd    mm1,D [esi+ edx*4 -4]
    movd    mm2,D [esi+ edx*4 +0]
    movd    mm3,D [esi+       -4]
    movd    mm4,D [esi+       +0]
    movd    mm5,D [esi+ eax*4 -4]
    movd    mm6,D [esi+ eax*4 +0]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    punpcklbw mm5,mm0
    punpcklbw mm6,mm0
    paddw   mm1,mm5
    paddw   mm2,mm6
    pmullw  mm1,Q [mmEcl]
    pmullw  mm2,Q [mmEch]
    pmullw  mm3,Q [mmEe]
    pmullw  mm4,Q [mmEm]
    paddw   mm1,mm2
    paddw   mm1,mm3
    paddw   mm1,mm4
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    mm2,D [ebx+ aulRows]
    movd    D [ebx+ aulRows],mm1
    movd    D [edi+ edx*4],mm2
    // advance to next row
    add     esi,D [slModulo1]
    add     edi,D [slModulo1]
    pop     ebx
    dec     ebx
    jnz     rowLoop

// ----------------------- process lower left corner

    xor     ebx,ebx
    movd    mm1,D [esi+ edx*4 +0]
    movd    mm2,D [esi+ edx*4 +4]
    movd    mm3,D [esi+       +0]
    movd    mm4,D [esi+       +4]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    paddw   mm1,mm4
    pmullw  mm1,Q [mmCe]
    pmullw  mm2,Q [mmCc]
    pmullw  mm3,Q [mmCm]
    paddw   mm1,mm2
    paddw   mm1,mm3
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    mm2,D [ebx+ aulRows]
    movd    D [edi],mm1
    movd    D [edi+ edx*4],mm2
    add     esi,4
    add     edi,4
    add     ebx,4

// ----------------------- process lower edge pixels

    mov     ecx,D [pixWidth]
    sub     ecx,2
    // for each pixel
lowerLoop:      
    movd    mm1,D [esi+ edx*4 -4]
    movd    mm2,D [esi+ edx*4 +0]
    movd    mm3,D [esi+ edx*4 +4]
    movd    mm4,D [esi+       -4]
    movd    mm5,D [esi+       +0]
    movd    mm6,D [esi+       +4]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    punpcklbw mm5,mm0
    punpcklbw mm6,mm0
    paddw   mm1,mm3
    paddw   mm4,mm6
    pmullw  mm1,Q [mmEcl]
    pmullw  mm2,Q [mmEe]
    pmullw  mm4,Q [mmEch]
    pmullw  mm5,Q [mmEm]
    paddw   mm1,mm2
    paddw   mm1,mm4
    paddw   mm1,mm5
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    mm2,D [ebx+ aulRows]
    movd    D [edi],mm1
    movd    D [edi+ edx*4],mm2
    // advance to next pixel
    add     esi,4
    add     edi,4
    add     ebx,4
    dec     ecx
    jnz     lowerLoop

// ----------------------- lower right corners

    movd    mm1,D [esi+ edx*4 -4]
    movd    mm2,D [esi+ edx*4 +0]
    movd    mm3,D [esi+       -4]
    movd    mm4,D [esi+       +0]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    punpcklbw mm3,mm0
    punpcklbw mm4,mm0
    paddw   mm2,mm3
    pmullw  mm1,Q [mmCc]
    pmullw  mm2,Q [mmCe]
    pmullw  mm4,Q [mmCm]
    paddw   mm1,mm2
    paddw   mm1,mm4
    paddsw  mm1,Q [mmAdd]
    pmulhw  mm1,Q [mmInvDiv]
    packuswb mm1,mm0
    movd    mm2,D [ebx+ aulRows]
    movd    D [edi],mm1
    movd    D [edi+ edx*4],mm2
    emms
  }

  // all done (finally)
  _pfGfxProfile.StopTimer( CGfxProfile::PTI_FILTERBITMAP);
}
 


// saturate color of bitmap
void AdjustBitmapColor( ULONG *pulSrc, ULONG *pulDst, PIX pixWidth, PIX pixHeight, 
                        SLONG const slHueShift, SLONG const slSaturation)
{
  for( INDEX i=0; i<(pixWidth*pixHeight); i++) {
    pulDst[i] = ByteSwap( AdjustColor( ByteSwap(pulSrc[i]), slHueShift, slSaturation));
  }
}


// create mip-map table for texture or shadow of given dimensions
void MakeMipmapTable( PIX pixU, PIX pixV, MipmapTable &mmt)
{
  mmt.mmt_pixU = pixU;
  mmt.mmt_pixV = pixV;
  // start at first mip map
  PIX pixCurrentU = mmt.mmt_pixU;
  PIX pixCurrentV = mmt.mmt_pixV;
  INDEX iMipmapCurrent = 0;
  SLONG slOffsetCurrent = 0;
  // while the mip-map is not zero-sized
  while (pixCurrentU>0 && pixCurrentV>0) {
    // remember its offset
    mmt.mmt_aslOffsets[iMipmapCurrent] = slOffsetCurrent;
    // go to next mip map
    slOffsetCurrent+=pixCurrentU*pixCurrentV;
    iMipmapCurrent++;
    pixCurrentU>>=1;
    pixCurrentV>>=1;
  }
  // remember number of mip maps and total size
  mmt.mmt_ctMipmaps   = iMipmapCurrent;
  mmt.mmt_slTotalSize = slOffsetCurrent;
}



// TRIANGLE MASK RENDERING (FOR MODEL CLUSTER SHADOWS) ROUTINES

static ULONG *_pulTexture;
static PIX    _pixTexWidth, _pixTexHeight;
extern BOOL   _bSomeDarkExists = FALSE;


// set texture that will be used for all subsequent triangles
void SetTriangleTexture( ULONG *pulCurrentMipmap, PIX pixMipWidth, PIX pixMipHeight)
{
  _pulTexture   = pulCurrentMipmap;
  _pixTexWidth  = pixMipWidth;
  _pixTexHeight = pixMipHeight;
}

// render one triangle to mask plane for shadow casting purposes
void DrawTriangle_Mask( UBYTE *pubMaskPlane, SLONG slMaskWidth, SLONG slMaskHeight,
                        struct PolyVertex2D *ppv2Vtx1, struct PolyVertex2D *ppv2Vtx2,
                        struct PolyVertex2D *ppv2Vtx3, BOOL bTransparency)
{
  struct PolyVertex2D *pUpper  = ppv2Vtx1;
  struct PolyVertex2D *pMiddle = ppv2Vtx2;
  struct PolyVertex2D *pLower  = ppv2Vtx3;
  struct PolyVertex2D *pTmp;

  // sort vertices by J position
  if( pUpper->pv2_fJ > pMiddle->pv2_fJ) {
    pTmp = pUpper; pUpper = pMiddle; pMiddle = pTmp;
  }
  if( pUpper->pv2_fJ > pLower->pv2_fJ) {
    pTmp = pUpper; pUpper = pLower; pLower = pTmp;
  }
  if( pMiddle->pv2_fJ > pLower->pv2_fJ) {
    pTmp = pMiddle; pMiddle = pLower; pLower = pTmp;
  }

  // determine vertical deltas
  FLOAT fDJShort1 = pMiddle->pv2_fJ - pUpper->pv2_fJ;
  FLOAT fDJShort2 = pLower->pv2_fJ  - pMiddle->pv2_fJ;
  FLOAT fDJLong   = pLower->pv2_fJ  - pUpper->pv2_fJ;
  if( fDJLong == 0) return;

  // determine horizontal deltas
  FLOAT fDIShort1 = pMiddle->pv2_fI - pUpper->pv2_fI;
  FLOAT fDIShort2 = pLower->pv2_fI  - pMiddle->pv2_fI;
  FLOAT fDILong   = pLower->pv2_fI  - pUpper->pv2_fI;

  // determine U/K, V/K and 1/K deltas
  FLOAT fD1oKShort1 = pMiddle->pv2_f1oK - pUpper->pv2_f1oK;
  FLOAT fD1oKShort2 = pLower->pv2_f1oK  - pMiddle->pv2_f1oK;
  FLOAT fD1oKLong   = pLower->pv2_f1oK  - pUpper->pv2_f1oK;
  FLOAT fDUoKShort1 = pMiddle->pv2_fUoK - pUpper->pv2_fUoK;
  FLOAT fDUoKShort2 = pLower->pv2_fUoK  - pMiddle->pv2_fUoK;
  FLOAT fDUoKLong   = pLower->pv2_fUoK  - pUpper->pv2_fUoK;
  FLOAT fDVoKShort1 = pMiddle->pv2_fVoK - pUpper->pv2_fVoK;
  FLOAT fDVoKShort2 = pLower->pv2_fVoK  - pMiddle->pv2_fVoK;
  FLOAT fDVoKLong   = pLower->pv2_fVoK  - pUpper->pv2_fVoK;

  // determine stepping factors;
  FLOAT f1oDJShort1, f1oDJShort2, f1oDJLong;
  if( fDJShort1 != 0) f1oDJShort1 = 1 / fDJShort1;  else f1oDJShort1 = 0;
  if( fDJShort2 != 0) f1oDJShort2 = 1 / fDJShort2;  else f1oDJShort2 = 0;
  if( fDJLong   != 0) f1oDJLong   = 1 / fDJLong;    else f1oDJLong   = 0;

  FLOAT fDIoDJShort1 = fDIShort1 * f1oDJShort1;
  FLOAT fDIoDJShort2 = fDIShort2 * f1oDJShort2;
  FLOAT fDIoDJLong   = fDILong   * f1oDJLong;
  FLOAT fMaxWidth    = fDIoDJLong*fDJShort1 + pUpper->pv2_fI - pMiddle->pv2_fI;

  // determine drawing direction and factors by direction
  SLONG direction = +1;
  if( fMaxWidth > 0) direction = -1;

  // find start and end values for J
  PIX pixUpJ = FloatToInt(pUpper->pv2_fJ  +0.5f);
  PIX pixMdJ = FloatToInt(pMiddle->pv2_fJ +0.5f);
  PIX pixDnJ = FloatToInt(pLower->pv2_fJ  +0.5f);

  // clip vertically
  if( pixDnJ<0 || pixUpJ>=slMaskHeight) return;
  if( pixUpJ<0) pixUpJ=0;
  if( pixDnJ>slMaskHeight) pixDnJ=slMaskHeight;
  if( pixMdJ<0) pixMdJ=0;
  if( pixMdJ>slMaskHeight) pixMdJ=slMaskHeight;
  SLONG fixWidth = slMaskWidth<<11;

  // find prestepped I
  FLOAT fPrestepUp = (FLOAT)pixUpJ - pUpper->pv2_fJ;
  FLOAT fPrestepMd = (FLOAT)pixMdJ - pMiddle->pv2_fJ;
  SLONG fixILong   = FloatToInt((pUpper->pv2_fI  + fPrestepUp * fDIoDJLong  )*2048.0f) +fixWidth*pixUpJ;
  SLONG fixIShort1 = FloatToInt((pUpper->pv2_fI  + fPrestepUp * fDIoDJShort1)*2048.0f) +fixWidth*pixUpJ;
  SLONG fixIShort2 = FloatToInt((pMiddle->pv2_fI + fPrestepMd * fDIoDJShort2)*2048.0f) +fixWidth*pixMdJ;

  // convert steps from floats to fixints (21:11)
  SLONG fixDIoDJLong   = FloatToInt(fDIoDJLong  *2048.0f) +fixWidth;
  SLONG fixDIoDJShort1 = FloatToInt(fDIoDJShort1*2048.0f) +fixWidth;
  SLONG fixDIoDJShort2 = FloatToInt(fDIoDJShort2*2048.0f) +fixWidth;

  // find row counter and max delta J
  SLONG ctJShort1 = pixMdJ - pixUpJ;
  SLONG ctJShort2 = pixDnJ - pixMdJ;
  SLONG ctJLong   = pixDnJ - pixUpJ;

  FLOAT currK, curr1oK, currUoK, currVoK;
  PIX   pixJ = pixUpJ;

  // if model has texture and texture has alpha channel, do complex mapping thru texture's alpha channel
  if( _pulTexture!=NULL && bTransparency) 
  { 
    // calculate some texture variables
    FLOAT fD1oKoDJShort1 = fD1oKShort1 * f1oDJShort1;
    FLOAT fD1oKoDJShort2 = fD1oKShort2 * f1oDJShort2;
    FLOAT fD1oKoDJLong   = fD1oKLong   * f1oDJLong;
    FLOAT fDUoKoDJShort1 = fDUoKShort1 * f1oDJShort1;
    FLOAT fDUoKoDJShort2 = fDUoKShort2 * f1oDJShort2;
    FLOAT fDUoKoDJLong   = fDUoKLong   * f1oDJLong;
    FLOAT fDVoKoDJShort1 = fDVoKShort1 * f1oDJShort1;
    FLOAT fDVoKoDJShort2 = fDVoKShort2 * f1oDJShort2;
    FLOAT fDVoKoDJLong   = fDVoKLong   * f1oDJLong;
    ;// FactOverDI = (DFoDJ * (J2-J1) + fact1 - fact2) * 1/width
    FLOAT f1oMaxWidth = 1 / fMaxWidth;
    FLOAT fD1oKoDI = (fD1oKoDJLong * fDJShort1 + pUpper->pv2_f1oK - pMiddle->pv2_f1oK) * f1oMaxWidth;
    FLOAT fDUoKoDI = (fDUoKoDJLong * fDJShort1 + pUpper->pv2_fUoK - pMiddle->pv2_fUoK) * f1oMaxWidth;
    FLOAT fDVoKoDI = (fDVoKoDJLong * fDJShort1 + pUpper->pv2_fVoK - pMiddle->pv2_fVoK) * f1oMaxWidth;
    if( direction == -1) {
      fD1oKoDI = -fD1oKoDI;
      fDUoKoDI = -fDUoKoDI;
      fDVoKoDI = -fDVoKoDI;
    }
    // find prestepped U/K, V/K, 1/K
    FLOAT f1oKLong   = pUpper->pv2_f1oK  + fPrestepUp * fD1oKoDJLong;
    FLOAT f1oKShort1 = pUpper->pv2_f1oK  + fPrestepUp * fD1oKoDJShort1;
    FLOAT f1oKShort2 = pMiddle->pv2_f1oK + fPrestepMd * fD1oKoDJShort2;
    FLOAT fUoKLong   = pUpper->pv2_fUoK  + fPrestepUp * fDUoKoDJLong;
    FLOAT fUoKShort1 = pUpper->pv2_fUoK  + fPrestepUp * fDUoKoDJShort1;
    FLOAT fUoKShort2 = pMiddle->pv2_fUoK + fPrestepMd * fDUoKoDJShort2;
    FLOAT fVoKLong   = pUpper->pv2_fVoK  + fPrestepUp * fDVoKoDJLong;
    FLOAT fVoKShort1 = pUpper->pv2_fVoK  + fPrestepUp * fDVoKoDJShort1;
    FLOAT fVoKShort2 = pMiddle->pv2_fVoK + fPrestepMd * fDVoKoDJShort2;
    
    // render upper triangle part
    PIX pixTexU, pixTexV;
    while( ctJShort1>0) {
      SLONG currI = fixILong>>11;
      SLONG countI = abs( currI - (fixIShort1>>11));
      if( countI==0) goto nextLine1;
      curr1oK = f1oKLong;
      currUoK = fUoKLong;
      currVoK = fVoKLong;
      if( direction == -1) currI--;
      if( countI>0) _bSomeDarkExists = TRUE;
      while( countI>0) {
        currK = 1.0f/curr1oK;
        pixTexU = (FloatToInt(currUoK*currK)) & (_pixTexWidth -1);
        pixTexV = (FloatToInt(currVoK*currK)) & (_pixTexHeight-1);
        if( _pulTexture[pixTexV*_pixTexWidth+pixTexU] & ((CT_rAMASK<<7)&CT_rAMASK)) pubMaskPlane[currI] = 0;
        curr1oK += fD1oKoDI;
        currUoK += fDUoKoDI;
        currVoK += fDVoKoDI;
        currI   += direction;
        countI--;
      }
  nextLine1:
      pixJ++;
      f1oKLong   += fD1oKoDJLong;
      f1oKShort1 += fD1oKoDJShort1;
      fUoKLong   += fDUoKoDJLong;
      fUoKShort1 += fDUoKoDJShort1;
      fVoKLong   += fDVoKoDJLong;
      fVoKShort1 += fDVoKoDJShort1;
      fixILong   += fixDIoDJLong;
      fixIShort1 += fixDIoDJShort1;
      ctJShort1--;
    }

    // render lower triangle part
    while( ctJShort2>0) {
      SLONG currI = fixILong>>11;
      SLONG countI = abs( currI - (fixIShort2>>11));
      if( countI==0) goto nextLine2;
      curr1oK = f1oKLong;
      currUoK = fUoKLong;
      currVoK = fVoKLong;
      if( direction == -1) currI--;
      if( countI>0) _bSomeDarkExists = TRUE;
      while( countI>0) {
        currK = 1.0f/curr1oK;
        pixTexU = (FloatToInt(currUoK*currK)) & (_pixTexWidth -1);
        pixTexV = (FloatToInt(currVoK*currK)) & (_pixTexHeight-1);
        if( _pulTexture[pixTexV*_pixTexWidth+pixTexU] & CT_rAMASK) pubMaskPlane[currI] = 0;
        curr1oK += fD1oKoDI;
        currUoK += fDUoKoDI;
        currVoK += fDVoKoDI;
        currI   += direction;
        countI--;
      }
  nextLine2:
      pixJ++;
      f1oKLong   += fD1oKoDJLong;
      f1oKShort2 += fD1oKoDJShort2;
      fUoKLong   += fDUoKoDJLong;
      fUoKShort2 += fDUoKoDJShort2;
      fVoKLong   += fDVoKoDJLong;
      fVoKShort2 += fDVoKoDJShort2;
      fixILong   += fixDIoDJLong;
      fixIShort2 += fixDIoDJShort2;
      ctJShort2--;
    }
  }
  // simple flat mapping (no texture at all)
  else 
  { 
    // render upper triangle part
    while( ctJShort1>0) {
      SLONG currI = fixILong>>11;
      SLONG countI = abs( currI - (fixIShort1>>11));
      if( direction == -1) currI--;
      if( countI>0) _bSomeDarkExists = TRUE;
      while( countI>0) {
        pubMaskPlane[currI] = 0;
        currI += direction;
        countI--;
      }
      pixJ++;
      fixILong   += fixDIoDJLong;
      fixIShort1 += fixDIoDJShort1;
      ctJShort1--;
    }
    // render lower triangle part
    while( ctJShort2>0) {
      SLONG currI = fixILong>>11;
      SLONG countI = abs( currI - (fixIShort2>>11));
      if( countI>0) _bSomeDarkExists = TRUE;
      if( direction == -1) currI--;
      while( countI>0) {
        pubMaskPlane[currI] = 0;
        currI += direction;
        countI--;
      }
      pixJ++;
      fixILong   += fixDIoDJLong;
      fixIShort2 += fixDIoDJShort2;
      ctJShort2--;
    }
  }
}








// ---------------------------------------------------------------------------------------------


#if 0

  // bilinear filtering of lower mipmap
  
  // row loop
  UBYTE r,g,b,a;
  for( PIX v=0; v<pixHeight; v++)
  { // column loop
    for( PIX u=0; u<pixWidth; u++)
    { // read four neighbour pixels
      COLOR colUL = pulSrcMipmap[((v*2+0)*pixCurrWidth*2+u*2) +0];
      COLOR colUR = pulSrcMipmap[((v*2+0)*pixCurrWidth*2+u*2) +1];
      COLOR colDL = pulSrcMipmap[((v*2+1)*pixCurrWidth*2+u*2) +0];
      COLOR colDR = pulSrcMipmap[((v*2+1)*pixCurrWidth*2+u*2) +1];
      // separate and add channels
      ULONG rRes=0, gRes=0, bRes=0, aRes=0;
      ColorToRGBA( colUL, r,g,b,a); rRes += r; gRes += g; bRes += b; aRes += a;
      ColorToRGBA( colUR, r,g,b,a); rRes += r; gRes += g; bRes += b; aRes += a;
      ColorToRGBA( colDL, r,g,b,a); rRes += r; gRes += g; bRes += b; aRes += a;
      ColorToRGBA( colDR, r,g,b,a); rRes += r; gRes += g; bRes += b; aRes += a;
      // round, average and store
      rRes += 2; gRes += 2; bRes += 2; aRes += 2;
      rRes >>=2; gRes >>=2; bRes >>=2; aRes >>=2;
      pulDstMipmap[v*pixCurrWidth+u] = RGBAToColor( rRes,gRes,bRes,aRes);
    }
  }



  // nearest-neighbouring of lower mipmap (with border preservance)

  // row loop
  PIX u,v;
  for( v=0; v<pixCurrHeight/2; v++) { 
    for( u=0; u<pixCurrWidth/2; u++) { // mipmap upper left pixel
      pulDstMipmap[v*pixCurrWidth+u] = pulSrcMipmap[((v*2+0)*pixCurrWidth*2+u*2) +0];
    }
    for( u=pixCurrWidth/2; u<pixCurrWidth; u++) { // mipmap upper right pixel
      pulDstMipmap[v*pixCurrWidth+u] = pulSrcMipmap[((v*2+0)*pixCurrWidth*2+u*2) +1];
    }
  }
  for( v=pixCurrHeight/2; v<pixCurrHeight; v++) { 
    for( u=0; u<pixCurrWidth/2; u++) { // mipmap upper left pixel
      pulDstMipmap[v*pixCurrWidth+u] = pulSrcMipmap[((v*2+1)*pixCurrWidth*2+u*2) +0];
    }
    for( u=pixCurrWidth/2; u<pixCurrWidth; u++) { // mipmap upper right pixel
      pulDstMipmap[v*pixCurrWidth+u] = pulSrcMipmap[((v*2+1)*pixCurrWidth*2+u*2) +1];
    }
  }



  // left to right error diffusion dithering

  __asm {
    pxor    mm0,mm0
    mov     esi,D [pulDst]
    mov     ebx,D [pixCanvasWidth]
    mov     edx,D [pixHeight]
    dec     edx // need not to dither last row
rowLoopE:
    mov     ecx,D [pixWidth]
    dec     ecx
pixLoopE:
    movd    mm1,D [esi]
    punpcklbw mm1,mm0
    pand    mm1,Q [mmErrDiffMask] 
    // determine errors
    movq    mm3,mm1
    paddw   mm3,mm3 // *2
    movq    mm5,mm3
    paddw   mm5,mm5 // *4
    movq    mm7,mm5
    paddw   mm7,mm7 // *8
    paddw   mm3,mm1 // *3
    paddw   mm5,mm1 // *5
    psubw   mm7,mm1 // *7
    psrlw   mm1,4
    psrlw   mm3,4
    psrlw   mm5,4
    psrlw   mm7,4
    packuswb mm1,mm0
    packuswb mm3,mm0
    packuswb mm5,mm0
    packuswb mm7,mm0
    // spread errors
    movd    mm2,D [esi+ ebx*4 +4]
    paddusb mm1,mm2
    paddusb mm3,Q [esi+ ebx*4 -4]
    paddusb mm5,Q [esi+ ebx*4 +0]
    paddusb mm7,Q [esi+       +4]
    movd    D [esi+ ebx*4 +4],mm1
    movd    D [esi+ ebx*4 -4],mm3
    movd    D [esi+ ebx*4 +0],mm5
    movd    D [esi+       +4],mm7
    // advance to next pixel
    add     esi,4
    dec     ecx
    jnz     pixLoopE
    // advance to next row
    add     esi,D [slModulo]
    dec     edx
    jnz     rowLoopE
    emms
  }


  // left to right and right to left error diffusion dithering

  __asm {
    pxor    mm0,mm0
    mov     esi,D [pulDst]
    mov     ebx,D [pixCanvasWidth]
    mov     edx,D [pixHeight]
    dec     edx // need not to dither last row
rowLoopE:
    // left to right
    mov     ecx,D [pixWidth]
    dec     ecx
pixLoopEL:
    movd    mm1,D [esi]
    punpcklbw mm1,mm0
    pand    mm1,Q [mmErrDiffMask] 
    // determine errors
    movq    mm3,mm1
    paddw   mm3,mm3 // *2
    movq    mm5,mm3
    paddw   mm5,mm5 // *4
    movq    mm7,mm5
    paddw   mm7,mm7 // *8
    paddw   mm3,mm1 // *3
    paddw   mm5,mm1 // *5
    psubw   mm7,mm1 // *7
    psrlw   mm1,4
    psrlw   mm3,4
    psrlw   mm5,4
    psrlw   mm7,4
    packuswb mm1,mm0
    packuswb mm3,mm0
    packuswb mm5,mm0
    packuswb mm7,mm0
    // spread errors
    movd    mm2,D [esi+ ebx*4 +4]
    paddusb mm1,mm2
    paddusb mm3,Q [esi+ ebx*4 -4]
    paddusb mm5,Q [esi+ ebx*4 +0]
    paddusb mm7,Q [esi+       +4]
    movd    D [esi+ ebx*4 +4],mm1
    movd    D [esi+ ebx*4 -4],mm3
    movd    D [esi+ ebx*4 +0],mm5
    movd    D [esi+       +4],mm7
    // advance to next pixel
    add     esi,4
    dec     ecx
    jnz     pixLoopEL
    // advance to next row
    add     esi,D [slWidthModulo]
    dec     edx
    jz      allDoneE

    // right to left
    mov     ecx,D [pixWidth]
    dec     ecx
pixLoopER:
    movd    mm1,D [esi]
    punpcklbw mm1,mm0
    pand    mm1,Q [mmErrDiffMask] 
    // determine errors
    movq    mm3,mm1
    paddw   mm3,mm3 // *2
    movq    mm5,mm3
    paddw   mm5,mm5 // *4
    movq    mm7,mm5
    paddw   mm7,mm7 // *8
    paddw   mm3,mm1 // *3
    paddw   mm5,mm1 // *5
    psubw   mm7,mm1 // *7
    psrlw   mm1,4
    psrlw   mm3,4
    psrlw   mm5,4
    psrlw   mm7,4
    packuswb mm1,mm0
    packuswb mm3,mm0
    packuswb mm5,mm0
    packuswb mm7,mm0
    // spread errors
    paddusb mm1,Q [esi+ ebx*4 -4]
    paddusb mm3,Q [esi+ ebx*4 +4]
    paddusb mm5,Q [esi+ ebx*4 +0]
    paddusb mm7,Q [esi+       -4]
    movd    D [esi+ ebx*4 -4],mm1
    movd    D [esi+ ebx*4 +4],mm3
    movd    D [esi+ ebx*4 +0],mm5
    movd    D [esi+       -4],mm7
    // revert to previous pixel
    sub     esi,4
    dec     ecx
    jnz     pixLoopER
    // advance to next row
    add     esi,D [slCanvasWidth]
    dec     edx
    jnz     rowLoopE
allDoneE:
    emms
  }



// bicubic

  static INDEX aiWeights[4][4] = {
{ -1,  9,  9, -1, },
{  9, 47, 47,  9, },
{  9, 47, 47,  9, },
{ -1,  9,  9, -1  }
};


      const SLONG slMaskU=pixWidth *2 -1;
    const SLONG slMaskV=pixHeight*2 -1;

    // bicubic?
    if( pixWidth>4 && pixHeight>4 /*&& tex_bBicubicMipmaps*/)
    {
      for( INDEX j=0; j<pixHeight; j++) {
        for( INDEX i=0; i<pixWidth; i++) {
          COLOR col;
          UBYTE ubR, ubG, ubB, ubA;
          SLONG slR=0, slG=0, slB=0, slA=0;
          for( INDEX v=0; v<4; v++) {
            const INDEX iRowSrc = ((v-1)+j*2) & slMaskV;
            for( INDEX u=0; u<4; u++) {
              const INDEX iColSrc = ((u-1)+i*2) & slMaskU;
              const INDEX iWeight = aiWeights[u][v];
              col = ByteSwap( pulSrcMipmap[iRowSrc*(slMaskU+1)+iColSrc]);
              ColorToRGBA( col, ubR,ubG,ubB,ubA);
              slR += ubR*iWeight;
              slG += ubG*iWeight;
              slB += ubB*iWeight;
              slA += ubA*iWeight;
            }
          }
          col = RGBAToColor( slR>>8, slG>>8, slB>>8, slA>>8);
          pulDstMipmap[j*pixWidth+i] = ByteSwap(col);
        }
      }
    }
    // bilinear!
    else
    {


    }

#endif
