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

#include "StdH.h"

#include <Engine/Base/Translation.h>
#include <Engine/Base/Memory.h>
#include <Engine/Base/Console.h>

#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/Vulkan/SvkMain.h>
#include <Engine/Graphics/ViewPort.h>

#include <Engine/Raytracing/SSRT.h>

// fog/haze textures
extern ULONG _fog_ulTexture;
extern ULONG _haze_ulTexture;

extern BOOL GFX_abTexture[GFX_MAXTEXUNITS];

extern BOOL GFX_bDepthTest;
extern BOOL GFX_bDepthWrite;
extern BOOL GFX_bAlphaTest;
extern BOOL GFX_bBlending;
extern BOOL GFX_bDithering;
extern BOOL GFX_bClipping;
extern BOOL GFX_bClipPlane;
extern BOOL GFX_bColorArray;
extern BOOL GFX_bFrontFace;
extern BOOL GFX_bTruform;
extern INDEX GFX_iActiveTexUnit;
extern FLOAT GFX_fMinDepthRange;
extern FLOAT GFX_fMaxDepthRange;
extern GfxBlend GFX_eBlendSrc;
extern GfxBlend GFX_eBlendDst;
extern GfxComp  GFX_eDepthFunc;
extern GfxFace  GFX_eCullFace;
extern INDEX GFX_iTexModulation[GFX_MAXTEXUNITS];
extern INDEX GFX_ctVertices;
extern BOOL  GFX_bViewMatrix;


BOOL CGfxLibrary::InitDriver_RayTracing()
{
  ASSERT(gl_SSRT == nullptr);

  gl_SSRT = new SSRT::SSRTMain();
  gl_SSRT->Init();

  return TRUE;
}

// Init global values
void CGfxLibrary::InitContext_RayTracing()
{
  // must have context
  ASSERT(_pGfx->gl_pvpActive != NULL);
  // reset engine's internal state variables
  for (INDEX iUnit = 0; iUnit < GFX_MAXTEXUNITS; iUnit++)
  {
    GFX_abTexture[iUnit] = FALSE;
    GFX_iTexModulation[iUnit] = 1;
  }

  // set default texture unit and modulation mode
  GFX_iActiveTexUnit = 0;
  _pGfx->gl_ctMaxStreams = 16;
  extern FLOAT GFX_fLastL, GFX_fLastR, GFX_fLastT, GFX_fLastB, GFX_fLastN, GFX_fLastF;
  GFX_fLastL = GFX_fLastR = GFX_fLastT = GFX_fLastB = GFX_fLastN = GFX_fLastF = 0;
  GFX_bViewMatrix = TRUE;
  GFX_bTruform = FALSE;
  GFX_bClipping = TRUE;

  // reset global state for pipeline
  GFX_abTexture[0] = TRUE;
  GFX_bDithering = TRUE;
  GFX_bBlending = FALSE;
  GFX_bDepthTest = FALSE;
  GFX_bAlphaTest = FALSE;
  GFX_bClipPlane = FALSE;
  GFX_eCullFace = GFX_NONE;
  GFX_bFrontFace = TRUE;
  GFX_bDepthWrite = FALSE;
  GFX_eDepthFunc = GFX_LESS_EQUAL;
  GFX_eBlendSrc = GFX_eBlendDst = GFX_ONE;
  GFX_fMinDepthRange = 0.0f;
  GFX_fMaxDepthRange = 1.0f;
  GFX_bColorArray = TRUE;
  GFX_ctVertices = 0;

  // report header
  CPrintF(TRANS("\n* Vulkan ray tracing context created: *------------------------------------\n"));
  CDisplayAdapter &da = _pGfx->gl_gaAPI[GAT_RT].ga_adaAdapter[_pGfx->gl_iCurrentAdapter];
  CPrintF("  (%s, %s, %s)\n\n", da.da_strVendor, da.da_strRenderer, da.da_strVersion);

  _pGfx->gl_ctTextureUnits = 4;
  _pGfx->gl_ctRealTextureUnits = 4;

  _pGfx->gl_fMaxTextureLODBias = 0;
  _pGfx->gl_iMaxTextureAnisotropy = 16;

  _pGfx->gl_iTessellationLevel = 0;
  _pGfx->gl_iMaxTessellationLevel = 0;

  _pGfx->gl_ulFlags |= GLF_HASACCELERATION;
  _pGfx->gl_ulFlags |= GLF_32BITTEXTURES;
  _pGfx->gl_ulFlags |= GLF_VSYNC;
  _pGfx->gl_ulFlags &= ~GLF_TEXTURECOMPRESSION;
  _pGfx->gl_ulFlags |= GLF_EXT_EDGECLAMP;

  // setup fog and haze textures
  extern PIX _fog_pixSizeH;
  extern PIX _fog_pixSizeL;
  extern PIX _haze_pixSize;
  _fog_pixSizeH = 0;
  _fog_pixSizeL = 0;
  _haze_pixSize = 0;

  // prepare pattern texture
  extern CTexParams _tpPattern;
  extern ULONG _ulPatternTexture;
  extern ULONG _ulLastUploadedPattern;
  _ulPatternTexture = 0;
  _ulLastUploadedPattern = 0;
  _tpPattern.Clear();

  // reset texture filtering and array locking
  _tpGlobal[0].Clear();
  _tpGlobal[1].Clear();
  _tpGlobal[2].Clear();
  _tpGlobal[3].Clear();
  _pGfx->gl_dwVertexShader = NONE;

  extern INDEX gap_iTextureFiltering;
  extern INDEX gap_iTextureAnisotropy;
  gfxSetTextureFiltering(gap_iTextureFiltering, gap_iTextureAnisotropy);

  extern FLOAT gap_fTextureLODBias;
  gap_fTextureLODBias = 0;

  // mark pretouching and probing
  extern BOOL _bNeedPretouch;
  _bNeedPretouch = TRUE;
  _pGfx->gl_bAllowProbing = FALSE;

  // update console system vars
  extern void UpdateGfxSysCVars(void);
  UpdateGfxSysCVars();
}

void CGfxLibrary::EndDriver_RayTracing()
{
  ASSERT(gl_SSRT != nullptr);
  delete gl_SSRT;

  gl_SSRT = nullptr;
}

BOOL CGfxLibrary::SetCurrentViewport_RayTracing(CViewPort* pvp)
{
  ASSERT(gl_SSRT != nullptr);

  // determine full screen mode
  CDisplayMode dm;
  RECT rectWindow;
  _pGfx->GetCurrentDisplayMode(dm);
  ASSERT((dm.dm_pixSizeI == 0 && dm.dm_pixSizeJ == 0) || (dm.dm_pixSizeI != 0 && dm.dm_pixSizeJ != 0));
  GetClientRect(pvp->vp_hWnd, &rectWindow);
  const PIX pixWinSizeI = rectWindow.right - rectWindow.left;
  const PIX pixWinSizeJ = rectWindow.bottom - rectWindow.top;

  // full screen allows only one window (main one, which has already been initialized)
  if (dm.dm_pixSizeI == pixWinSizeI && dm.dm_pixSizeJ == pixWinSizeJ)
  {
    // remember as current viewport (must do that BEFORE InitContext)
    _pGfx->gl_pvpActive = pvp;

    if (_pGfx->gl_ulFlags & GLF_INITONNEXTWINDOW)
    {
      InitContext_RayTracing();
    }

    _pGfx->gl_ulFlags &= ~GLF_INITONNEXTWINDOW;
    return TRUE;
  }

  // if must init entire Vulkan
  if (_pGfx->gl_ulFlags & GLF_INITONNEXTWINDOW)
  {
    _pGfx->gl_ulFlags &= ~GLF_INITONNEXTWINDOW;

    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();

    _pGfx->gl_pvpActive = pvp;
    InitContext_RayTracing();

    pvp->vp_ctDisplayChanges = _pGfx->gl_ctDriverChanges;
    return TRUE;
  }

  // if window was not set for this driver
  if (pvp->vp_ctDisplayChanges < _pGfx->gl_ctDriverChanges)
  {
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();

    pvp->vp_ctDisplayChanges = _pGfx->gl_ctDriverChanges;
    _pGfx->gl_pvpActive = pvp;

    return TRUE;
  }

  // no need to set context if it is the same window as last time
  if (_pGfx->gl_pvpActive != NULL && _pGfx->gl_pvpActive->vp_hWnd == pvp->vp_hWnd)
  {
    return TRUE;
  }

  // remember as current window
  _pGfx->gl_pvpActive = pvp;
  return TRUE;
}

void CGfxLibrary::StartFrame_RayTracing(CRaster* pRaster)
{
  ASSERT(gl_SSRT != nullptr);

  gl_SSRT->StartFrame(pRaster->ra_pvpViewPort);
}

void CGfxLibrary::EndFrame_RayTracing()
{
  ASSERT(gl_SSRT != nullptr);

  gl_SSRT->EndFrame();
}
