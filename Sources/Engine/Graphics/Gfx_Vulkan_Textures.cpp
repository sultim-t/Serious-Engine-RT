/* Copyright (c) 2020 Sultim Tsyrendashiev
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

#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/Vulkan/SvkMain.h>

#include <Engine/Base/Statistics_internal.h>
#include <Engine/Math/Functions.h>
#include <Engine/Graphics/Color.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/GfxProfile.h>

#include <Engine/Base/ListIterator.inl>

static CTexParams *_tpCurrent;
extern INDEX GFX_iActiveTexUnit;

static SvkSamplerFlags UnpackFilter_Vulkan(INDEX iFilter)
{
  SvkSamplerFlags flags = 0;

  switch (iFilter)
  {
  case 110:  case 10:
  case 111:  case 11:  flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_MIPMAP_NEAREST; break;
  case 112:  case 12:  flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_MIPMAP_LINEAR; break;

  case 220:  case 20:
  case 221:  case 21:  flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_MIPMAP_NEAREST; break;
  case 222:  case 22:  flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_MIPMAP_LINEAR; break;

  case 120:
  case 121:            flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_MIPMAP_NEAREST; break;
  case 122:            flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_MIPMAP_LINEAR; break;

  case 210:
  case 211:            flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_MIPMAP_NEAREST; break;
  case 212:            flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_MIPMAP_LINEAR; break;
  default: ASSERTALWAYS("Illegal Vulkan texture filtering mode."); break;
  }

  return flags;
}

SvkSamplerFlags MimicTexParams_Vulkan(CTexParams &tpLocal)
{
  ASSERT(&tpLocal != NULL);

  SvkSamplerFlags flags = 0; // _pGfx->gl_VkGlobalSamplerState;

  if (tpLocal.tp_iFilter != _tpGlobal[0].tp_iFilter)
  {
    tpLocal.tp_iFilter = _tpGlobal[0].tp_iFilter;
  };

  if (tpLocal.tp_iAnisotropy != _tpGlobal[0].tp_iAnisotropy)
  {
    tpLocal.tp_iAnisotropy = _tpGlobal[0].tp_iAnisotropy;
  }

  if (tpLocal.tp_eWrapU != _tpGlobal[GFX_iActiveTexUnit].tp_eWrapU || tpLocal.tp_eWrapV != _tpGlobal[GFX_iActiveTexUnit].tp_eWrapV)
  {
    tpLocal.tp_eWrapU = _tpGlobal[GFX_iActiveTexUnit].tp_eWrapU;
    tpLocal.tp_eWrapV = _tpGlobal[GFX_iActiveTexUnit].tp_eWrapV;
  }

  //flags &= ~SVK_TSS_FILTER_MAG_BITS;
  //flags &= ~SVK_TSS_FILTER_MIN_BITS;
  //flags &= ~SVK_TSS_MIPMAP_BITS;

  flags |= UnpackFilter_Vulkan(tpLocal.tp_iFilter);
  
  //flags &= ~SVK_TSS_ANISOTROPY_BITS;

  if (tpLocal.tp_iAnisotropy > 8)       { flags |= SVK_TSS_ANISOTROPY_16; }
  else if (tpLocal.tp_iAnisotropy > 4)  { flags |= SVK_TSS_ANISOTROPY_8;  }
  else if (tpLocal.tp_iAnisotropy > 2)  { flags |= SVK_TSS_ANISOTROPY_4;  }
  else if (tpLocal.tp_iAnisotropy > 1)  { flags |= SVK_TSS_ANISOTROPY_2;  }
  else                                  { flags |= SVK_TSS_ANISOTROPY_0;  }

  //flags &= ~SVK_TSS_WRAP_U_BITS;
  //flags &= ~SVK_TSS_WRAP_V_BITS;

  if (tpLocal.tp_eWrapU == GFX_REPEAT)
  {
    flags |= SVK_TSS_WRAP_U_REPEAT; 
  }
  else if (_pGfx->gl_ulFlags & GLF_EXT_EDGECLAMP)
  {
    flags |= SVK_TSS_WRAP_U_CLAMP_EDGE;
  }
  else
  {
    flags |= SVK_TSS_WRAP_U_CLAMP; 
  }

  if (tpLocal.tp_eWrapV == GFX_REPEAT)
  { 
    flags |= SVK_TSS_WRAP_V_REPEAT; 
  }
  else if (_pGfx->gl_ulFlags & GLF_EXT_EDGECLAMP)
  {
    flags |= SVK_TSS_WRAP_V_CLAMP_EDGE;
  }
  else
  { 
    flags |= SVK_TSS_WRAP_V_CLAMP; 
  }

  _tpCurrent = &tpLocal;

  return flags;
}

void UploadTexture_Vulkan(uint32_t *iTexture, ULONG *pulTexture, PIX pixSizeU, PIX pixSizeV, VkFormat eInternalFormat, BOOL bUseSubImage)
{  // safeties
  ASSERT(pulTexture != NULL);
  ASSERT(pixSizeU > 0 && pixSizeV > 0);
  _sfStats.StartTimer(CStatForm::STI_BINDTEXTURE);
  _pfGfxProfile.StartTimer(CGfxProfile::PTI_TEXTUREUPLOADING);

  uint32_t mipmapCount = 0;
  VkExtent2D mipmapSizes[32];
  
  // upload each original mip-map
  PIX pixOffset = 0;
  while (pixSizeU > 0 && pixSizeV > 0)
  {
    // check that memory is readable
    ASSERT(pulTexture[pixOffset + pixSizeU * pixSizeV - 1] != 0xDEADBEEF);
    // upload mipmap as fast as possible
    if (bUseSubImage) {
      //pglTexSubImage2D(GL_TEXTURE_2D, iMip, 0, 0, pixSizeU, pixSizeV,
      //  GL_RGBA, GL_UNSIGNED_BYTE, pulTexture + pixOffset);
    }
    else {
      //pglTexImage2D(GL_TEXTURE_2D, iMip, eInternalFormat, pixSizeU, pixSizeV, 0,
      //  GL_RGBA, GL_UNSIGNED_BYTE, pulTexture + pixOffset);
    };

    if (pixSizeU > 0 && pixSizeV > 0)
    {
      mipmapSizes[mipmapCount].width = pixSizeU;
      mipmapSizes[mipmapCount].height = pixSizeV;
      mipmapCount++;
    }

    // advance to next mip-map
    pixOffset += pixSizeU * pixSizeV;
    pixSizeU >>= 1;
    pixSizeV >>= 1;

    // end here if there is only one mip-map to upload
    if (_tpCurrent->tp_bSingleMipmap) 
    {
      break;
    }
  }

  // see if we need to generate and upload additional mipmaps (those under 1*N or N*1)
  /*if (!_tpCurrent->tp_bSingleMipmap && pixSizeU != pixSizeV)
  { // prepare variables
    PIX pixSize = Max(pixSizeU, pixSizeV);
    ASSERT(pixSize <= 2048);
    ULONG *pulSrc = pulTexture + pixOffset - pixSize * 2;
    ULONG *pulDst = _aulLastMipmaps;
    // loop thru mipmaps
    while (pixSizeU > 0 || pixSizeV > 0)
    { // make next mipmap
      if (pixSizeU == 0) pixSizeU = 1;
      if (pixSizeV == 0) pixSizeV = 1;
      pixSize = pixSizeU * pixSizeV;
      __asm {
        pxor    mm0, mm0
        mov     esi, D[pulSrc]
        mov     edi, D[pulDst]
        mov     ecx, D[pixSize]
        pixLoop:
        movd    mm1, D[esi + 0]
          movd    mm2, D[esi + 4]
          punpcklbw mm1, mm0
          punpcklbw mm2, mm0
          paddw   mm1, mm2
          psrlw   mm1, 1
          packuswb mm1, mm0
          movd    D[edi], mm1
          add     esi, 4 * 2
          add     edi, 4
          dec     ecx
          jnz     pixLoop
          emms
      }
      // upload mipmap
      if (bUseSubImage) {
        pglTexSubImage2D(GL_TEXTURE_2D, iMip, 0, 0, pixSizeU, pixSizeV,
          GL_RGBA, GL_UNSIGNED_BYTE, pulDst);
      }
      else {
        pglTexImage2D(GL_TEXTURE_2D, iMip, eInternalFormat, pixSizeU, pixSizeV, 0,
          GL_RGBA, GL_UNSIGNED_BYTE, pulDst);
      } OGL_CHECKERROR;
      // advance to next mip-map
      pulSrc = pulDst;
      pulDst += pixSize;
      pixOffset += pixSize;
      pixSizeU >>= 1;
      pixSizeV >>= 1;
      iMip++;
    }
  }*/

  _pGfx->gl_SvkMain->InitTexture32Bit(*iTexture, eInternalFormat, pulTexture, mipmapSizes, mipmapCount, bUseSubImage == TRUE);

  // all done
  _pfGfxProfile.IncrementCounter(CGfxProfile::PCI_TEXTUREUPLOADS, 1);
  _pfGfxProfile.IncrementCounter(CGfxProfile::PCI_TEXTUREUPLOADBYTES, pixOffset * 4);
  _sfStats.IncrementCounter(CStatForm::SCI_TEXTUREUPLOADS, 1);
  _sfStats.IncrementCounter(CStatForm::SCI_TEXTUREUPLOADBYTES, pixOffset * 4);
  _pfGfxProfile.StopTimer(CGfxProfile::PTI_TEXTUREUPLOADING);
  _sfStats.StopTimer(CStatForm::STI_BINDTEXTURE);
}