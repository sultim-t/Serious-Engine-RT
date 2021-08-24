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

#include <Engine/Raytracing/SSRTObjects.h>
#include <Engine/Raytracing/TextureUploader.h>
#include <Engine/Raytracing/SSRTGlobals.h>
#include <Engine/Raytracing/CustomInfo.h>


extern SSRT::SSRTGlobals _srtGlobals;


extern INDEX tex_iNormalQuality;
extern INDEX tex_iAnimationQuality;
extern INDEX tex_iNormalSize;
extern INDEX tex_iAnimationSize;
extern INDEX tex_iEffectSize;

extern INDEX tex_iDithering;
extern INDEX tex_iFiltering;

extern INDEX gap_bAllowSingleMipmap;
extern FLOAT gfx_tmProbeDecay;


static SSRT::CustomInfo *_pCustomInfo = nullptr;

void RT_SetCustomInfoForTextures(SSRT::CustomInfo *pCustomInfo)
{
  _pCustomInfo = pCustomInfo;
}

static void TryFixWrapping(SSRT::CPreparedTextureInfo &info, CTextureData *ptd)
{
  if (_pCustomInfo == nullptr)
  {
    return;
  }

  if (!info.isDynamic && _pCustomInfo->IsClampWrapForced(ptd))
  {
    info.wrapU = RG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.wrapV = RG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  }
}

static bool IsOverrideDisabled(CTextureData *ptd)
{
  if (_pCustomInfo == nullptr)
  {
    return false;
  }

  return _pCustomInfo->IsOverrideDisabled(ptd);
}


static bool IsEffectTexture(const CTextureData *ptd)
{
  if (ptd->td_ptegEffect == nullptr)
  {
    return false;
  }

  if (_pCustomInfo != nullptr && _pCustomInfo->IsNoEffectOnTexture(ptd))
  {
    return false;
  }

  return true;
}


static void UnpackTexParams(const CTexParams &tpLocal, RgSamplerFilter *filter, RgSamplerAddressMode *wrapU, RgSamplerAddressMode *wrapV)
{
  switch (tpLocal.tp_iFilter)
  {
    case 110:  case 10:
    case 111:  case 11:
    case 112:  case 12:   *filter = RG_SAMPLER_FILTER_NEAREST; break;

    default:              *filter = RG_SAMPLER_FILTER_LINEAR; break;
  }

  *wrapU = tpLocal.tp_eWrapU == GFX_REPEAT ? RG_SAMPLER_ADDRESS_MODE_REPEAT : RG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  *wrapV = tpLocal.tp_eWrapV == GFX_REPEAT ? RG_SAMPLER_ADDRESS_MODE_REPEAT : RG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
}


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
  // effect textures are treated differently
  if (IsEffectTexture(&td))
  {
    return true;
  }
  else
  {
    return (td.td_ctFineMipLevels < 2);
  }
}


static void ProcessEffectTexture(CTextureData &td, bool *pBNoDiscard, bool *pBNeedUpload, PIX *pPixWidth, PIX *pPixHeight)
{
  ASSERT(IsEffectTexture(&td));

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

  bool bFirstUpdate = td.td_ptegEffect->teg_updTexture.LastUpdateTime() == -1;
  bool bDisableUpdates = _srtGlobals.srt_bIgnoreWaterEffectTextureUpdates && td.td_ptegEffect->IsWater();

  // if not calculated for this tick (must be != to test for time rewinding)
  bool bOutdated = td.td_ptegEffect->teg_updTexture.LastUpdateTime() != _pTimer->CurrentTick();

  if (bFirstUpdate || (bOutdated && !bDisableUpdates))
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

  if (bNeedUpload)
  {
    extern INDEX tex_iEffectFiltering;

    const INDEX iTexFilter = td.td_ptegEffect->IsWater() ? NONE : tex_iEffectFiltering;  // don't filter water textures
    if (td.td_tpLocal.tp_bSingleMipmap)
    {
      // no mipmaps?
      if (iTexFilter != NONE)
      {
        FilterBitmap(iTexFilter, td.td_pulFrames, td.td_pulFrames, pixWidth, pixHeight);
      }
    }
  }
}

static void SetCurrentAndUpload(CTextureData &td, PIX pixWidth, PIX pixHeight, SSRT::TextureUploader *uploader)
{
  // check whether frames are present
  ASSERT(td.td_pulFrames != NULL && td.td_pulFrames[0] != 0xDEADBEEF);

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

    SSRT::CPreparedAnimatedTextureInfo info = {};
    info.textureID = td.td_ulObject;
    info.width = pixWidth;
    info.height = pixHeight;
    info.frameData = td.td_pulFrames;
    info.frameCount = td.td_ctFrames;
    info.frameStride = td.td_slFrameSize;
    info.generateMipmaps = !td.td_tpLocal.tp_bSingleMipmap;
    info.path = &td.GetName();

    UnpackTexParams(td.td_tpLocal, &info.filter, &info.wrapU, &info.wrapV);

    // instead of gfxUploadTexture
    uploader->UploadTexture(info);
  }
  else
  {
    // single-frame textures

    SSRT::CPreparedTextureInfo info = {};
    info.textureID = td.td_ulObject;
    info.width = pixWidth;
    info.height = pixHeight;
    info.imageData = td.td_pulFrames;
    info.isDynamic = IsEffectTexture(&td);
    info.generateMipmaps = info.isDynamic ? false : !td.td_tpLocal.tp_bSingleMipmap;
    info.path = &td.GetName();
    info.disableOverride = IsOverrideDisabled(&td);

    UnpackTexParams(td.td_tpLocal, &info.filter, &info.wrapU, &info.wrapV);

    TryFixWrapping(info, &td);

    // instead of gfxUploadTexture
    uploader->UploadTexture(info);
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

  // RT: td_pulObjects is empty (commented previously) 

  // RT: texture was already uploaded
  //gfxSetTexture(td.td_ulObject, td.td_tpLocal);
  // RT: don't mark, as texture probing is not used, 
  //     but GetHighPrecisionTimer call is expensive
  //td.MarkDrawn();
}


// RT: This function is a copy of CTextureData::SetAsCurrent
// but modified for 
unsigned RT_SetTextureAsCurrent(CTextureData *textureData, SSRT::TextureUploader *uploader)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_RT);
  ASSERT(textureData != nullptr);

  CTextureData &td = *textureData;

  bool bNeedUpload  = false; // forceUpload;
  bool bNoDiscard   = true;

  PIX  pixWidth   = td.GetPixWidth();
  PIX  pixHeight  = td.GetPixHeight();

  // RT: no probes
  td.td_ulProbeObject = NONE;

  // determine whether texture has single mipmap
  td.td_tpLocal.tp_bSingleMipmap = IsSingleMipmap(td);

  // if we have an effect texture
  if (IsEffectTexture(&td))
  {
    // effect texture must have only one frame
    // ASSERT(frameIndex == 0);
    ProcessEffectTexture(td, &bNoDiscard, &bNeedUpload, &pixWidth, &pixHeight);
  }

  // RT: no filtering/mipmaps for effect textures
  // RT: even with td_ctFrames > 1, td_pulObjects are not used,
  //     as animated textures have only one material index too

  // if not already generated, generate bind number(s) and force upload
  if (td.td_ulObject == NONE)
  {
    // check whether frames are present
    ASSERT(td.td_pulFrames != NULL && td.td_pulFrames[0] != 0xDEADBEEF);

    // RT: generate index for this texture
    td.td_ulObject = uploader->GenerateTextureID();

    // must do initial uploading
    bNeedUpload = true;
    bNoDiscard = false;
  }

  // if needs to be uploaded
  if (bNeedUpload)
  {
    SetCurrentAndUpload(td, pixWidth, pixHeight, uploader);
  }
  else
  {
    SetCurrent(td);
  }

  // debug check
  ASSERT(td.td_ulObject != NONE);

  // RT: this field is used for texture index
  return td.td_ulObject;
}
