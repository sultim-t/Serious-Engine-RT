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

#include <Base/Statistics_internal.h>
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


// RT: This function is a copy of CTextureData::SetAsCurrent
// but modified for 
void RT_SetAsCurrent(CTextureData *textureData, INDEX iFrameNo/*=0*/, BOOL bForceUpload/*=FALSE*/)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_RT);

  if (textureData == nullptr)
  {
    return;
  }

  CTextureData &td = *textureData;

  ASSERT(iFrameNo < td.td_ctFrames);
  BOOL bNeedUpload = bForceUpload;
  BOOL bNoDiscard = TRUE;
  PIX  pixWidth = td.GetPixWidth();
  PIX  pixHeight = td.GetPixHeight();

  // eventually re-adjust LOD bias
  extern FLOAT _fCurrentLODBias;
  const FLOAT fWantedLODBias = _pGfx->gl_fTextureLODBias;
  extern void UpdateLODBias(const FLOAT fLODBias);
  if (td.td_ulFlags & TEX_CONSTANT)
  {
    // non-adjustable textures don't tolerate positive LOD bias
    if (_fCurrentLODBias > 0) UpdateLODBias(0);
    else if (_fCurrentLODBias > fWantedLODBias) UpdateLODBias(fWantedLODBias);
  }
  else if (td.td_ulFlags & TEX_EQUALIZED)
  {
    // equilized textures don't tolerate negative LOD bias
    if (_fCurrentLODBias < 0) UpdateLODBias(0);
    else if (_fCurrentLODBias < fWantedLODBias) UpdateLODBias(fWantedLODBias);
  }
  else if (_fCurrentLODBias != fWantedLODBias)
  {
    // all other textures must take LOD bias into account
    UpdateLODBias(fWantedLODBias);
  }

  // determine probing
  extern BOOL ProbeMode(CTimerValue tvLast);
  BOOL bUseProbe = ProbeMode(td.td_tvLastDrawn);

  // if we have an effect texture
  if (td.td_ptegEffect != NULL)
  {
    ASSERT(iFrameNo == 0); // effect texture must have only one frame
    // get max allowed effect texture dimension
    PIX pixClampAreaSize = 1L << 16L;
    tex_iEffectSize = Clamp(tex_iEffectSize, 4L, 8L);
    if (!(td.td_ulFlags & TEX_CONSTANT)) pixClampAreaSize = 1L << (tex_iEffectSize * 2);
    INDEX iWantedMipLevel = td.td_iFirstMipLevel
      + ClampTextureSize(pixClampAreaSize, _pGfx->gl_pixMaxTextureDimension, pixWidth, pixHeight);
    // check whether wanted mip level is beyond last mip-level
    iWantedMipLevel = td.ClampMipLevel(iWantedMipLevel);

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
    // if current frame size differs from the previous one
    SLONG slFrameSize = GetMipmapOffset(15, pixWidth, pixHeight) * BYTES_PER_TEXEL;
    if (td.td_pulFrames == NULL || td.td_slFrameSize != slFrameSize)
    {
      // (re)allocate the frame buffer
      if (td.td_pulFrames != NULL) FreeMemory(td.td_pulFrames);
      td.td_pulFrames = (ULONG *)AllocMemory(slFrameSize);
      td.td_slFrameSize = slFrameSize;
      bNoDiscard = FALSE;
    }

    // if not calculated for this tick (must be != to test for time rewinding)
    if (td.td_ptegEffect->teg_updTexture.LastUpdateTime() != _pTimer->CurrentTick())
    {
      // discard eventual cached frame and calculate new frame
      td.MarkChanged();
      td.td_ptegEffect->Animate();
      bNeedUpload = TRUE;
      // make sure that effect and base textures are static
      td.Force(TEX_STATIC);
      td.td_ptdBaseTexture->Force(TEX_STATIC);
      // copy some flags from base texture to effect texture
      td.td_ulFlags |= td.td_ptdBaseTexture->td_ulFlags & (TEX_ALPHACHANNEL | TEX_TRANSPARENT | TEX_GRAY);
      // render effect texture
      td.td_ptegEffect->Render(iWantedMipLevel, pixWidth, pixHeight);
      // determine internal format
      ULONG ulNewFormat;
      if (td.td_ulFlags & TEX_GRAY)
      {
        if (td.td_ulFlags & TEX_ALPHACHANNEL) ulNewFormat = TS.ts_tfLA8;
        else ulNewFormat = TS.ts_tfL8;
      }
      else
      {
        if (td.td_ulFlags & TEX_TRANSPARENT) ulNewFormat = TS.ts_tfRGB5A1;
        else if (td.td_ulFlags & TEX_ALPHACHANNEL) ulNewFormat = TS.ts_tfRGBA4;
        else ulNewFormat = TS.ts_tfRGB5;
      }
      // effect texture can be in 32-bit quality only if base texture hasn't been dithered
      extern INDEX tex_bFineEffect;
      if (tex_bFineEffect && (td.td_ptdBaseTexture->td_ulFlags & TEX_DITHERED))
      {
        ulNewFormat = PromoteTo32bitFormat(ulNewFormat);
      }
      // internal format changed? - must discard!
      if (td.td_ulInternalFormat != ulNewFormat)
      {
        td.td_ulInternalFormat = ulNewFormat;
        bNoDiscard = FALSE;
      }
    } // effect texture cannot have probing
    bUseProbe = FALSE;
  }

  // prepare effect cvars
  extern INDEX tex_bDynamicMipmaps;
  extern INDEX tex_iEffectFiltering;
  if (tex_bDynamicMipmaps) tex_bDynamicMipmaps = 1;
  tex_iEffectFiltering = Clamp(tex_iEffectFiltering, -6L, +6L);

  // determine whether texture has single mipmap
  if (gap_bAllowSingleMipmap)
  {
    // effect textures are treated differently
    if (td.td_ptegEffect != NULL) td.td_tpLocal.tp_bSingleMipmap = !tex_bDynamicMipmaps;
    else td.td_tpLocal.tp_bSingleMipmap = (td.td_ctFineMipLevels < 2);
  }
  else
  {
    // single mipmap is not allowed
    td.td_tpLocal.tp_bSingleMipmap = FALSE;
  }

  // effect texture might need dynamic mipmaps creation
  if (bNeedUpload && td.td_ptegEffect != NULL)
  {
    _sfStats.StartTimer(CStatForm::STI_EFFECTRENDER);
    const INDEX iTexFilter = td.td_ptegEffect->IsWater() ? NONE : tex_iEffectFiltering;  // don't filter water textures
    if (td.td_tpLocal.tp_bSingleMipmap)
    { // no mipmaps?
      if (iTexFilter != NONE) FilterBitmap(iTexFilter, td.td_pulFrames, td.td_pulFrames, pixWidth, pixHeight);
    }
    else
    { // mipmaps!
      const INDEX ctFine = tex_bDynamicMipmaps ? 15 : 0; // whether they're fine or coarse still depends on cvar
      MakeMipmaps(ctFine, td.td_pulFrames, pixWidth, pixHeight, iTexFilter);
    } // done with effect
    _sfStats.StopTimer(CStatForm::STI_EFFECTRENDER);
  }

  // if not already generated, generate bind number(s) and force upload
  const PIX pixTextureSize = pixWidth * pixHeight;
  if ((td.td_ctFrames > 1 && td.td_pulObjects == NULL) || (td.td_ctFrames <= 1 && td.td_ulObject == NONE))
  {
    // check whether frames are present
    ASSERT(td.td_pulFrames != NULL && td.td_pulFrames[0] != 0xDEADBEEF);

    if (td.td_ctFrames > 1)
    {
      // animation textures
      td.td_pulObjects = (ULONG *)AllocMemory(td.td_ctFrames * sizeof(td.td_ulProbeObject));
      for (INDEX i = 0; i < td.td_ctFrames; i++) gfxGenerateTexture(td.td_pulObjects[i]);
    }
    else
    {
      // single-frame textures
      gfxGenerateTexture(td.td_ulObject);
    }
    // generate probe texture (if needed)
    ASSERT(td.td_ulProbeObject == NONE);
    if (td.td_ptegEffect == NULL && pixTextureSize > 16 * 16) gfxGenerateTexture(td.td_ulProbeObject);
    // must do initial uploading
    bNeedUpload = TRUE;
    bNoDiscard = FALSE;
  }

  // constant textures cannot be probed either
  if (td.td_ulFlags & TEX_CONSTANT) gfxDeleteTexture(td.td_ulProbeObject);
  if (td.td_ulProbeObject == NONE)  bUseProbe = FALSE;

  // update statistics if not updated already for this frame
  if (td.td_iRenderFrame != _pGfx->gl_iFrameNumber)
  {
    td.td_iRenderFrame = _pGfx->gl_iFrameNumber;
    // determine size and update
    SLONG slBytes = pixWidth * pixHeight * gfxGetFormatPixRatio(td.td_ulInternalFormat);
    if (!td.td_tpLocal.tp_bSingleMipmap) slBytes = slBytes * 4 / 3;
    _sfStats.IncrementCounter(CStatForm::SCI_TEXTUREBINDS, 1);
    _sfStats.IncrementCounter(CStatForm::SCI_TEXTUREBINDBYTES, slBytes);
  }

  // if needs to be uploaded
  if (bNeedUpload)
  {
    // check whether frames are present
    ASSERT(td.td_pulFrames != NULL && td.td_pulFrames[0] != 0xDEADBEEF);

    // must discard uploaded texture if single mipmap flag has been changed
    const BOOL bLastSingleMipmap = td.td_ulFlags & TEX_SINGLEMIPMAP;
    bNoDiscard = (bNoDiscard && bLastSingleMipmap == td.td_tpLocal.tp_bSingleMipmap);
    // update flag
    if (td.td_tpLocal.tp_bSingleMipmap) td.td_ulFlags |= TEX_SINGLEMIPMAP;
    else td.td_ulFlags &= ~TEX_SINGLEMIPMAP;

    // upload all texture frames
    ASSERT(td.td_ulInternalFormat != TEXFMT_NONE);
    if (td.td_ctFrames > 1)
    {
      // animation textures
      for (INDEX iFr = 0; iFr < td.td_ctFrames; iFr++)
      { // determine frame offset and upload texture frame
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
    // upload probe texture if exist
    if (td.td_ulProbeObject != NONE)
    {
      PIX pixProbeWidth = pixWidth;
      PIX pixProbeHeight = pixHeight;
      ULONG *pulProbeFrame = td.td_pulFrames;
      GetMipmapOfSize(16 * 16, pulProbeFrame, pixProbeWidth, pixProbeHeight);
      gfxSetTexture(td.td_ulProbeObject, td.td_tpLocal);
      gfxUploadTexture(pulProbeFrame, pixProbeWidth, pixProbeHeight, TS.ts_tfRGBA4, FALSE);
    }
    // clear local texture parameters because we need to correct later texture setting
    td.td_tpLocal.Clear();
    // free frames' memory if allowed
    if (!(td.td_ulFlags & TEX_STATIC))
    {
      FreeMemory(td.td_pulFrames);
      td.td_pulFrames = NULL;
    }
    // done uploading
    ASSERT((td.td_ctFrames > 1 && td.td_pulObjects != NULL) || (td.td_ctFrames == 1 && td.td_ulObject != NONE));
    return;
  }

  // do special case for animated textures when parameters re-initialization is required
  if (td.td_ctFrames > 1 && !td.td_tpLocal.IsEqual(_tpGlobal[0]))
  {
    // must reset local texture parameters for each frame of animated texture
    for (INDEX iFr = 0; iFr < td.td_ctFrames; iFr++)
    {
      td.td_tpLocal.Clear();
      gfxSetTexture(td.td_pulObjects[iFr], td.td_tpLocal);
    }
  }
  // set corresponding probe or texture frame as current
  ULONG ulTexObject = td.td_ulObject; // single-frame
  if (td.td_ctFrames > 1) ulTexObject = td.td_pulObjects[iFrameNo]; // animation
  if (bUseProbe)
  {
    // set probe if burst value doesn't allow real texture
    if (_pGfx->gl_slAllowedUploadBurst < 0)
    {
      CTexParams tpTmp = td.td_tpLocal;
      ASSERT(td.td_ulProbeObject != NONE);
      gfxSetTexture(td.td_ulProbeObject, tpTmp);
      //extern INDEX _ctProbeTexs;
      //_ctProbeTexs++;
      //CPrintF( "Probed!\n");
      return;
    }
    // reduce allowed burst value
    _pGfx->gl_slAllowedUploadBurst -= pixWidth * pixHeight * 4; // assume 32-bit textures (don't ask driver!)
  }
  // set real texture and mark that this texture has been drawn
  gfxSetTexture(ulTexObject, td.td_tpLocal);
  MarkDrawn();

  // debug check
  ASSERT((td.td_ctFrames > 1 && td.td_pulObjects != NULL) || (td.td_ctFrames <= 1 && td.td_ulObject != NONE));
}
