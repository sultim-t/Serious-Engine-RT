/* Copyright (c) 2021 Sultim Tsyrendashiev
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

#include "Engine/Raytracing/SSRT.h"

static void ssrt_SetOrtho(const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop,
                          const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar,
                          const BOOL bSubPixelAdjust/*=FALSE*/)
{
  // check API and matrix type
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_RT);

  // cached?
  if (GFX_fLastL == fLeft && GFX_fLastT == fTop && GFX_fLastN == fNear
      && GFX_fLastR == fRight && GFX_fLastB == fBottom && GFX_fLastF == fFar && gap_bOptimizeStateChanges) return;
  GFX_fLastL = fLeft;   GFX_fLastT = fTop;     GFX_fLastN = fNear;
  GFX_fLastR = fRight;  GFX_fLastB = fBottom;  GFX_fLastF = fFar;

  float proj[16];
  Svk_MatOrtho(proj, fLeft, fRight, fBottom, fTop, fNear, fFar);

  _pGfx->gl_SSRT->SetProjectionMatrix(proj);
}

static void ssrt_SetFrustum(const FLOAT fLeft, const FLOAT fRight,
                            const FLOAT fTop, const FLOAT fBottom,
                            const FLOAT fNear, const FLOAT fFar)
{
  // check API
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_RT);

  // cached?
  if (GFX_fLastL == -fLeft && GFX_fLastT == -fTop && GFX_fLastN == -fNear
      && GFX_fLastR == -fRight && GFX_fLastB == -fBottom && GFX_fLastF == -fFar && gap_bOptimizeStateChanges) return;
  GFX_fLastL = -fLeft;   GFX_fLastT = -fTop;     GFX_fLastN = -fNear;
  GFX_fLastR = -fRight;  GFX_fLastB = -fBottom;  GFX_fLastF = -fFar;

  float proj[16];
  Svk_MatFrustum(proj, fLeft, fRight, fBottom, fTop, fNear, fFar);

  _pGfx->gl_SSRT->SetProjectionMatrix(proj);
}

static void ssrt_SetViewMatrix(const FLOAT *pfMatrix/*=NULL*/)
{
  // check API
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_RT);

  // cached? (only identity matrix)
  if (pfMatrix == NULL && GFX_bViewMatrix == NONE && gap_bOptimizeStateChanges) return;
  GFX_bViewMatrix = (pfMatrix != NULL);

  if (pfMatrix != NULL)
  {
    _pGfx->gl_SSRT->SetViewMatrix(pfMatrix);
  }
  else
  {
    float identity[] =
    {
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      0,0,0,1
    };

    _pGfx->gl_SSRT->SetViewMatrix(identity);
  }
}


// DUMMY FUNCTIONS FOR RAY TRACING API
static void ssrt_void(void){ NOTHING; }
static void ssrt_BlendFunc( GfxBlend eSrc, GfxBlend eDst) { NOTHING; }
static void ssrt_DepthFunc( GfxComp eFunc) { NOTHING; }
static void ssrt_DepthRange( FLOAT fMin, FLOAT fMax) { NOTHING; }
static void ssrt_CullFace( GfxFace eFace) { NOTHING; }
static void ssrt_ClipPlane( const DOUBLE *pdViewPlane) { NOTHING; }
static void ssrt_SetTextureMatrix( const FLOAT *pfMatrix) { NOTHING; }
static void ssrt_PolygonMode( GfxPolyMode ePolyMode) { NOTHING; }
static void ssrt_SetTextureWrapping( enum GfxWrap eWrapU, enum GfxWrap eWrapV) { NOTHING; }
static void ssrt_SetTextureModulation( INDEX iScale) { NOTHING; }
static void ssrt_GenDelTexture( ULONG &ulTexObject) { NOTHING; }
static void ssrt_SetVertexArray( GFXVertex4 *pvtx, INDEX ctVtx) { NOTHING; }
static void ssrt_SetNormalArray( GFXNormal *pnor) { NOTHING; }
static void ssrt_SetTexCoordArray( GFXTexCoord *ptex, BOOL b4) { NOTHING; }
static void ssrt_SetColorArray( GFXColor *pcol) { NOTHING; }
static void ssrt_DrawElements( INDEX ctElem, INDEX *pidx) { NOTHING; }
static void ssrt_SetConstantColor( COLOR col) { NOTHING; }
static void ssrt_SetColorMask( ULONG ulColorMask) { NOTHING; }
