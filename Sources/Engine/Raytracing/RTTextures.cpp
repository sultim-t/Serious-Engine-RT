/* Copyright (c) 2020-2021 Sultim Tsyrendashiev
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

#include "StdH.h"
#include "RTProcessing.h"

#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/TextureEffects.h>


extern INDEX tex_iNormalQuality;
extern INDEX tex_iAnimationQuality;
extern INDEX tex_iNormalSize;
extern INDEX tex_iAnimationSize;
extern INDEX tex_iEffectSize;

extern INDEX tex_iDithering;
extern INDEX tex_iFiltering;

extern INDEX gap_bAllowSingleMipmap;
extern FLOAT gfx_tmProbeDecay;


static INDEX GetWantedMipmapLevel(const CTextureData &td, PIX *pPixWidth, PIX *pPixHeight)
{
  PIX &pixWidth = *pPixWidth;
  PIX &pixHeight = *pPixHeight;

  // get max allowed effect texture dimension
  PIX pixClampAreaSize = 1L << 16L;
  tex_iEffectSize = Clamp(tex_iEffectSize, 4L, 8L);

  if (!(td.td_ulFlags & TEX_CONSTANT))
  {
    pixClampAreaSize = 1L << (tex_iEffectSize * 2);
  }

  INDEX iWantedMipLevel = td.td_iFirstMipLevel
    + ClampTextureSize(pixClampAreaSize, _pGfx->gl_pixMaxTextureDimension, pixWidth, pixHeight);

  // check whether wanted mip level is beyond last mip-level
  iWantedMipLevel = td.ClampMipLevel((FLOAT)iWantedMipLevel);

  // default adjustment for mapping
  pixWidth >>= iWantedMipLevel - td.td_iFirstMipLevel;
  pixHeight >>= iWantedMipLevel - td.td_iFirstMipLevel;
  ASSERT(pixWidth > 0 && pixHeight > 0);

  // eventually adjust water effect texture size (if larger than base)
  if (td.td_ptegEffect->IsWater())
  {
    INDEX iMipDiff = Min(FastLog2(td.td_ptdBaseTexture->GetPixWidth()) - FastLog2(pixWidth),
                         FastLog2(td.td_ptdBaseTexture->GetPixHeight()) - FastLog2(pixHeight));
    iWantedMipLevel = iMipDiff;

    if (iMipDiff < 0)
    {
      pixWidth >>= (-iMipDiff);
      pixHeight >>= (-iMipDiff);

      iWantedMipLevel = 0;

      ASSERT(pixWidth > 0 && pixHeight > 0);
    }
  }

  return iWantedMipLevel;
}


static bool IsSingleMipmap(const CTextureData &td)
{
  // prepare effect cvars
  extern INDEX tex_bDynamicMipmaps;

  if (tex_bDynamicMipmaps)
  {
    tex_bDynamicMipmaps = 1;
  }

  // determine whether texture has single mipmap
  if (gap_bAllowSingleMipmap)
  {
    // effect textures are treated differently
    if (td.td_ptegEffect != NULL)
    {
      return !tex_bDynamicMipmaps;
    }
    else
    {
      return (td.td_ctFineMipLevels < 2);
    }
  }
  else
  {
    // single mipmap is not allowed
    return false;
  }
}


static void ProcessEffectTexture(CTextureData &td, bool *pBNoDiscard, bool *pBNeedUpload, PIX *pPixWidth, PIX *pPixHeight)
{
  ASSERT(td.td_ptegEffect != NULL);

  bool &bNoDiscard = *pBNoDiscard;
  bool &bNeedUpload = *pBNeedUpload;
  PIX &pixWidth = *pPixWidth;
  PIX &pixHeight = *pPixHeight;

  INDEX iWantedMipLevel = GetWantedMipmapLevel(td, &pixWidth, &pixHeight);

  // if current frame size differs from the previous one
  SLONG slFrameSize = GetMipmapOffset(15, pixWidth, pixHeight) * BYTES_PER_TEXEL;

  if (td.td_pulFrames == NULL || td.td_slFrameSize != slFrameSize)
  {
    // (re)allocate the frame buffer
    if (td.td_pulFrames != NULL)
    {
      FreeMemory(td.td_pulFrames);
    }

    td.td_pulFrames = (ULONG *)AllocMemory(slFrameSize);
    td.td_slFrameSize = slFrameSize;

    bNoDiscard = false;
  }

  // if not calculated for this tick (must be != to test for time rewinding)
  if (td.td_ptegEffect->teg_updTexture.LastUpdateTime() != _pTimer->CurrentTick())
  {
    // discard eventual cached frame and calculate new frame
    td.MarkChanged();
    td.td_ptegEffect->Animate();
    bNeedUpload = true;

    // make sure that effect and base textures are static
    td.Force(TEX_STATIC);
    td.td_ptdBaseTexture->Force(TEX_STATIC);

    // copy some flags from base texture to effect texture
    td.td_ulFlags |= td.td_ptdBaseTexture->td_ulFlags & (TEX_ALPHACHANNEL | TEX_TRANSPARENT | TEX_GRAY);

    // render effect texture
    td.td_ptegEffect->Render(iWantedMipLevel, pixWidth, pixHeight);

    // RT: always RGBA8
    ULONG ulNewFormat = TS.ts_tfRGBA8;

    // internal format changed? - must discard!
    if (td.td_ulInternalFormat != ulNewFormat)
    {
      td.td_ulInternalFormat = ulNewFormat;
      bNoDiscard = false;
    }
  }
}

static void SetCurrentAndUpload(CTextureData &td, PIX pixWidth, PIX pixHeight, bool bNoDiscard)
{
  // check whether frames are present
  ASSERT(td.td_pulFrames != NULL && td.td_pulFrames[0] != 0xDEADBEEF);

  // must discard uploaded texture if single mipmap flag has been changed
  const BOOL bLastSingleMipmap = td.td_ulFlags & TEX_SINGLEMIPMAP;
  bNoDiscard = (bNoDiscard && bLastSingleMipmap == td.td_tpLocal.tp_bSingleMipmap);

  // update flag
  if (td.td_tpLocal.tp_bSingleMipmap)
  {
    td.td_ulFlags |= TEX_SINGLEMIPMAP;
  }
  else
  {
    td.td_ulFlags &= ~TEX_SINGLEMIPMAP;
  }

  // upload all texture frames
  if (td.td_ctFrames > 1)
  {
    // animation textures
    for (INDEX iFr = 0; iFr < td.td_ctFrames; iFr++)
    {
      // determine frame offset and upload texture frame
      ULONG *pulCurrentFrame = td.td_pulFrames + (iFr * td.td_slFrameSize / BYTES_PER_TEXEL);

      gfxSetTexture(td.td_pulObjects[iFr], td.td_tpLocal);
      gfxUploadTexture(pulCurrentFrame, pixWidth, pixHeight, td.td_ulInternalFormat, bNoDiscard);
    }
  }
  else
  {
    // single-frame textures
    gfxSetTexture(td.td_ulObject, td.td_tpLocal);
    gfxUploadTexture(td.td_pulFrames, pixWidth, pixHeight, td.td_ulInternalFormat, bNoDiscard);
  }

  // clear local texture parameters because we need to correct later texture setting
  td.td_tpLocal.Clear();

  // free frames' memory if allowed
  if (!(td.td_ulFlags & TEX_STATIC))
  {
    FreeMemory(td.td_pulFrames);
    td.td_pulFrames = NULL;
  }
}


static void SetCurrent(CTextureData &td)
{
  // set corresponding probe or texture frame as current
  ULONG ulTexObject = td.td_ulObject;

  // RT: td_pulObjects is empty (commented previously) 

  // set real texture and mark that this texture has been drawn
  gfxSetTexture(ulTexObject, td.td_tpLocal);
  td.MarkDrawn();
}


// RT: This function is a copy of CTextureData::SetAsCurrent
// but modified for 
void RT_SetTextureAsCurrent(CTextureData *textureData, INDEX frameIndex/*=0*/, BOOL forceUpload/*=false*/)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_RT);

  if (textureData == nullptr)
  {
    return;
  }

  CTextureData &td = *textureData;
  ASSERT(frameIndex < td.td_ctFrames);

  bool bNeedUpload  = forceUpload;
  bool bNoDiscard   = true;

  PIX  pixWidth   = td.GetPixWidth();
  PIX  pixHeight  = td.GetPixHeight();

  // RT: no probes
  td.td_ulProbeObject = NONE;

  // if we have an effect texture
  if (td.td_ptegEffect != NULL)
  {
    // effect texture must have only one frame
    ASSERT(frameIndex == 0);

    ProcessEffectTexture(td, &bNoDiscard, &bNeedUpload, &pixWidth, &pixHeight);
  }

  // determine whether texture has single mipmap
  td.td_tpLocal.tp_bSingleMipmap = IsSingleMipmap(td);

  // RT: no filtering/mipmaps for effect textures
  // RT: even with td_ctFrames > 1, td_pulObjects are not used,
  //     as animated textures have only one material index too

  // if not already generated, generate bind number(s) and force upload
  if ((td.td_ctFrames > 1 && td.td_pulObjects == NULL) || (td.td_ctFrames <= 1 && td.td_ulObject == NONE))
  {
    // check whether frames are present
    ASSERT(td.td_pulFrames != NULL && td.td_pulFrames[0] != 0xDEADBEEF);

    // must do initial uploading
    bNeedUpload = true;
    bNoDiscard = false;
  }

  // if needs to be uploaded
  if (bNeedUpload)
  {
    SetCurrentAndUpload(td, pixWidth, pixHeight, bNoDiscard);
  }
  else
  {
    SetCurrent(td);
  }

  // debug check
  ASSERT((td.td_ctFrames > 1 && td.td_pulObjects != NULL) || (td.td_ctFrames <= 1 && td.td_ulObject != NONE));
}
