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

#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/ViewPort.h>

#include <Engine/Graphics/GfxProfile.h>
#include <Engine/Base/Statistics_internal.h>

//#include <d3dx8math.h>
//#pragma comment(lib, "d3dx8.lib")


//#include <d3dx8tex.h>
//#pragma comment(lib, "d3dx8.lib")
//extern "C" HRESULT WINAPI D3DXGetErrorStringA( HRESULT hr, LPSTR pBuffer, UINT BufferLen);
//char acErrorString[256];
//D3DXGetErrorString( hr, acErrorString, 255);
//ASSERTALWAYS( acErrorString);

extern INDEX gap_bOptimizeStateChanges;
extern INDEX gap_iOptimizeClipping;
extern INDEX gap_iDithering;
                
            
// cached states
extern BOOL GFX_bDepthTest  = FALSE;
extern BOOL GFX_bDepthWrite = FALSE;
extern BOOL GFX_bAlphaTest  = FALSE;
extern BOOL GFX_bDithering  = TRUE;
extern BOOL GFX_bBlending   = TRUE;
extern BOOL GFX_bClipping   = TRUE;
extern BOOL GFX_bClipPlane  = FALSE;
extern BOOL GFX_bColorArray = FALSE;
extern BOOL GFX_bTruform    = FALSE;
extern BOOL GFX_bFrontFace  = TRUE;
extern BOOL GFX_bViewMatrix = TRUE;
extern INDEX GFX_iActiveTexUnit = 0;
extern FLOAT GFX_fMinDepthRange = 0.0f;
extern FLOAT GFX_fMaxDepthRange = 0.0f;

extern GfxBlend GFX_eBlendSrc  = GFX_ONE;
extern GfxBlend GFX_eBlendDst  = GFX_ZERO;
extern GfxComp  GFX_eDepthFunc = GFX_LESS_EQUAL;
extern GfxFace  GFX_eCullFace  = GFX_NONE;
extern BOOL       GFX_abTexture[GFX_MAXTEXUNITS] = { FALSE, FALSE, FALSE, FALSE };
extern INDEX GFX_iTexModulation[GFX_MAXTEXUNITS] = { 0, 0, 0, 0 };

// last ortho/frustum values (frustum has negative sign, because of orgho-frustum switching!)
extern FLOAT GFX_fLastL = 0;
extern FLOAT GFX_fLastR = 0;
extern FLOAT GFX_fLastT = 0;
extern FLOAT GFX_fLastB = 0;
extern FLOAT GFX_fLastN = 0;
extern FLOAT GFX_fLastF = 0;

// number of vertices currently in buffer
extern INDEX GFX_ctVertices = 0;

// for D3D: mark need for clipping (when wants to be disable but cannot be because of user clip plane)
static BOOL _bWantsClipping = TRUE;
// current color mask (for Get... function)
static ULONG _ulCurrentColorMask = (CT_RMASK|CT_GMASK|CT_BMASK|CT_AMASK);
// locking state for OGL
static BOOL _bCVAReallyLocked = FALSE;

// clip plane and last view matrix for D3D
extern FLOAT D3D_afClipPlane[4]    = {0,0,0,0};
extern FLOAT D3D_afViewMatrix[16]  = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
static FLOAT _afActiveClipPlane[4] = {0,0,0,0};

// Truform/N-Patches
extern INDEX truform_iLevel  = -1;
extern BOOL  truform_bLinear = FALSE;


// functions' pointers
extern void (*gfxEnableDepthWrite)(void) = NULL;
extern void (*gfxEnableDepthBias)(void) = NULL;
extern void (*gfxEnableDepthTest)(void) = NULL;
extern void (*gfxEnableAlphaTest)(void) = NULL;
extern void (*gfxEnableBlend)(void) = NULL;
extern void (*gfxEnableDither)(void) = NULL;
extern void (*gfxEnableTexture)(void) = NULL;
extern void (*gfxEnableClipping)(void) = NULL;
extern void (*gfxEnableClipPlane)(void) = NULL;
extern void (*gfxDisableDepthWrite)(void) = NULL;
extern void (*gfxDisableDepthBias)(void) = NULL;
extern void (*gfxDisableDepthTest)(void) = NULL;
extern void (*gfxDisableAlphaTest)(void) = NULL;
extern void (*gfxDisableBlend)(void) = NULL;
extern void (*gfxDisableDither)(void) = NULL;
extern void (*gfxDisableTexture)(void) = NULL;
extern void (*gfxDisableClipping)(void) = NULL;
extern void (*gfxDisableClipPlane)(void) = NULL;
extern void (*gfxBlendFunc)( GfxBlend eSrc, GfxBlend eDst) = NULL;
extern void (*gfxDepthFunc)( GfxComp eFunc) = NULL;
extern void (*gfxDepthRange)( FLOAT fMin, FLOAT fMax) = NULL;
extern void (*gfxCullFace)(  GfxFace eFace) = NULL;
extern void (*gfxFrontFace)( GfxFace eFace) = NULL;
extern void (*gfxClipPlane)( const DOUBLE *pdPlane) = NULL;
extern void (*gfxSetOrtho)( const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop,  const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar, const BOOL bSubPixelAdjust) = NULL;
extern void (*gfxSetFrustum)( const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop,  const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar) = NULL;
extern void (*gfxSetTextureMatrix)( const FLOAT *pfMatrix) = NULL;
extern void (*gfxSetViewMatrix)( const FLOAT *pfMatrix) = NULL;
extern void (*gfxPolygonMode)( GfxPolyMode ePolyMode) = NULL;
extern void (*gfxSetTextureWrapping)( enum GfxWrap eWrapU, enum GfxWrap eWrapV) = NULL;
extern void (*gfxSetTextureModulation)( INDEX iScale) = NULL;
extern void (*gfxGenerateTexture)( ULONG &ulTexObject) = NULL;
extern void (*gfxDeleteTexture)( ULONG &ulTexObject) = NULL;
extern void (*gfxSetVertexArray)( GFXVertex4 *pvtx, INDEX ctVtx) = NULL;
extern void (*gfxSetNormalArray)( GFXNormal *pnor) = NULL;
extern void (*gfxSetTexCoordArray)( GFXTexCoord *ptex, BOOL b4) = NULL;
extern void (*gfxSetColorArray)( GFXColor *pcol) = NULL;
extern void (*gfxDrawElements)( INDEX ctElem, INDEX *pidx) = NULL;
extern void (*gfxSetConstantColor)(COLOR col) = NULL;
extern void (*gfxEnableColorArray)(void) = NULL;
extern void (*gfxDisableColorArray)(void) = NULL;
extern void (*gfxFinish)(void) = NULL;
extern void (*gfxLockArrays)(void) = NULL;
extern void (*gfxEnableTruform)( void) = NULL;
extern void (*gfxDisableTruform)(void) = NULL;
extern void (*gfxSetColorMask)( ULONG ulColorMask) = NULL; 



// dummy function (one size fits all:)
static void none_void(void)
{
  ASSERT( _pGfx->gl_eCurrentAPI==GAT_NONE);
}


// error checkers (this is for debug version only)

extern void OGL_CheckError(void)
{
#ifndef NDEBUG
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
  if( eAPI==GAT_OGL) ASSERT( pglGetError()==GL_NO_ERROR);
  else ASSERT( eAPI==GAT_NONE);
#endif
}

#ifdef SE1_D3D
extern void D3D_CheckError(HRESULT hr)
{
#ifndef NDEBUG
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
  if( eAPI==GAT_D3D) ASSERT( hr==D3D_OK);
  else ASSERT( eAPI==GAT_NONE);
#endif
}
#endif // SE1_D3D


// TEXTURE MANAGEMENT
#ifdef SE1_D3D
static LPDIRECT3DTEXTURE8 *_ppd3dCurrentTexture;
#endif // SE1_D3D

extern INDEX GetTexturePixRatio_OGL( GLuint uiBindNo);
extern INDEX GetFormatPixRatio_OGL( GLenum eFormat);
extern void  MimicTexParams_OGL( CTexParams &tpLocal);
extern void  UploadTexture_OGL( ULONG *pulTexture, PIX pixSizeU, PIX pixSizeV,
                                GLenum eInternalFormat, BOOL bUseSubImage);

#ifdef SE1_D3D
extern INDEX GetTexturePixRatio_D3D( LPDIRECT3DTEXTURE8 pd3dTexture);
extern INDEX GetFormatPixRatio_D3D( D3DFORMAT d3dFormat);
extern void  MimicTexParams_D3D( CTexParams &tpLocal);
extern void  UploadTexture_D3D( LPDIRECT3DTEXTURE8 *ppd3dTexture, ULONG *pulTexture,
                                PIX pixSizeU, PIX pixSizeV, D3DFORMAT eInternalFormat, BOOL bDiscard);
#endif // SE1_D3D

// update texture LOD bias
extern FLOAT _fCurrentLODBias = 0;  // LOD bias adjuster
extern void UpdateLODBias( const FLOAT fLODBias)
{ 
  // check API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT( eAPI==GAT_OGL || eAPI==GAT_D3D || eAPI==GAT_NONE);
#else // SE1_D3D
  ASSERT( eAPI==GAT_OGL || eAPI==GAT_NONE);
#endif // SE1_D3D
  // only if supported and needed
  if( _fCurrentLODBias==fLODBias && _pGfx->gl_fMaxTextureLODBias==0) return;
  _fCurrentLODBias = fLODBias;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // OpenGL
  if( eAPI==GAT_OGL) 
  { // if no multitexturing
    if( _pGfx->gl_ctTextureUnits<2) { 
      pglTexEnvf( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, fLODBias);
      OGL_CHECKERROR;
    } 
    // if multitexturing is active
    else {
      for( INDEX iUnit=0; iUnit<_pGfx->gl_ctTextureUnits; iUnit++) { // loop thru units
        pglActiveTexture(iUnit);  // select the unit
        pglTexEnvf( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, fLODBias);
        OGL_CHECKERROR;
      } // reselect the original unit
      pglActiveTexture(GFX_iActiveTexUnit);
      OGL_CHECKERROR;
    }
  }
  // Direct3D
#ifdef SE1_D3D
  else if( eAPI==GAT_D3D)
  { // just set it
    HRESULT hr;
    for( INDEX iUnit=0; iUnit<_pGfx->gl_ctTextureUnits; iUnit++) { // loop thru tex units
      hr = _pGfx->gl_pd3dDevice->SetTextureStageState( iUnit, D3DTSS_MIPMAPLODBIAS, *((DWORD*)&fLODBias));
      D3D_CHECKERROR(hr);
    }
  }
#endif // SE1_D3D
  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// get current texture filtering mode
extern void gfxGetTextureFiltering( INDEX &iFilterType, INDEX &iAnisotropyDegree)
{
  iFilterType = _tpGlobal[0].tp_iFilter;
  iAnisotropyDegree = _tpGlobal[0].tp_iAnisotropy;
}


// set texture filtering mode
extern void gfxSetTextureFiltering( INDEX &iFilterType, INDEX &iAnisotropyDegree)
{              
  // clamp vars
  INDEX iMagTex = iFilterType /100;     iMagTex = Clamp( iMagTex, 0L, 2L);  // 0=same as iMinTex, 1=nearest, 2=linear
  INDEX iMinTex = iFilterType /10 %10;  iMinTex = Clamp( iMinTex, 1L, 2L);  // 1=nearest, 2=linear
  INDEX iMinMip = iFilterType %10;      iMinMip = Clamp( iMinMip, 0L, 2L);  // 0=no mipmapping, 1=nearest, 2=linear
  iFilterType   = iMagTex*100 + iMinTex*10 + iMinMip;
  iAnisotropyDegree = Clamp( iAnisotropyDegree, 1L, _pGfx->gl_iMaxTextureAnisotropy);

  // skip if not changed
  if( _tpGlobal[0].tp_iFilter==iFilterType && _tpGlobal[0].tp_iAnisotropy==iAnisotropyDegree) return;
  _tpGlobal[0].tp_iFilter = iFilterType;
  _tpGlobal[0].tp_iAnisotropy = iAnisotropyDegree;

  // for OpenGL, that's about it
#ifdef SE1_D3D
  if( _pGfx->gl_eCurrentAPI!=GAT_D3D) return;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // for D3D, it's a stage state (not texture state), so change it!
  HRESULT hr;
 _D3DTEXTUREFILTERTYPE eMagFilter, eMinFilter, eMipFilter;
  const LPDIRECT3DDEVICE8 pd3dDev = _pGfx->gl_pd3dDevice; 
  extern void UnpackFilter_D3D( INDEX iFilter, _D3DTEXTUREFILTERTYPE &eMagFilter,
                               _D3DTEXTUREFILTERTYPE &eMinFilter, _D3DTEXTUREFILTERTYPE &eMipFilter);
  UnpackFilter_D3D( iFilterType, eMagFilter, eMinFilter, eMipFilter);
  if( iAnisotropyDegree>1) { // adjust filter for anisotropy
    eMagFilter = D3DTEXF_ANISOTROPIC;
    eMinFilter = D3DTEXF_ANISOTROPIC;
  }
  // set filtering and anisotropy degree
  for( INDEX iUnit=0; iUnit<_pGfx->gl_ctTextureUnits; iUnit++) { // must loop thru all usable texture units
    hr = pd3dDev->SetTextureStageState( iUnit, D3DTSS_MAXANISOTROPY, iAnisotropyDegree);  D3D_CHECKERROR(hr);
    hr = pd3dDev->SetTextureStageState( iUnit, D3DTSS_MAGFILTER, eMagFilter);  D3D_CHECKERROR(hr);
    hr = pd3dDev->SetTextureStageState( iUnit, D3DTSS_MINFILTER, eMinFilter);  D3D_CHECKERROR(hr);
    hr = pd3dDev->SetTextureStageState( iUnit, D3DTSS_MIPFILTER, eMipFilter);  D3D_CHECKERROR(hr);
  }
  // done
  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
#endif // SE1_D3D
}


// set new texture LOD biasing
extern void gfxSetTextureBiasing( FLOAT &fLODBias)
{
  // adjust LOD biasing if needed
  fLODBias = Clamp( fLODBias, -_pGfx->gl_fMaxTextureLODBias, +_pGfx->gl_fMaxTextureLODBias); 
  if( _pGfx->gl_fTextureLODBias != fLODBias) {
    _pGfx->gl_fTextureLODBias = fLODBias;
    UpdateLODBias( fLODBias);
  }
}



// set texture unit as active
extern void gfxSetTextureUnit( INDEX iUnit)
{
  // check API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT( eAPI==GAT_OGL || eAPI==GAT_D3D || eAPI==GAT_NONE);
#else // SE1_D3D
  ASSERT( eAPI==GAT_OGL || eAPI==GAT_NONE);
#endif // SE1_D3D
  ASSERT( iUnit>=0 && iUnit<4); // supports 4 layers (for now)

  // check consistency
#ifndef NDEBUG
  if( eAPI==GAT_OGL) {
    GLint gliRet;
    pglGetIntegerv( GL_ACTIVE_TEXTURE_ARB, &gliRet);
    ASSERT( GFX_iActiveTexUnit==(gliRet-GL_TEXTURE0_ARB));
    pglGetIntegerv( GL_CLIENT_ACTIVE_TEXTURE_ARB, &gliRet);
    ASSERT( GFX_iActiveTexUnit==(gliRet-GL_TEXTURE0_ARB));
  }
#endif

  // cached?
  if( GFX_iActiveTexUnit==iUnit) return;
  GFX_iActiveTexUnit = iUnit;

  // really set only for OpenGL
  if( eAPI!=GAT_OGL) return;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);
  pglActiveTexture(iUnit);
  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set texture as current
extern void gfxSetTexture( ULONG &ulTexObject, CTexParams &tpLocal)
{
  // clamp texture filtering if needed
  static INDEX _iLastTextureFiltering = 0;
  if( _iLastTextureFiltering != _tpGlobal[0].tp_iFilter) {
    INDEX iMagTex = _tpGlobal[0].tp_iFilter /100;     iMagTex = Clamp( iMagTex, 0L, 2L);  // 0=same as iMinTex, 1=nearest, 2=linear
    INDEX iMinTex = _tpGlobal[0].tp_iFilter /10 %10;  iMinTex = Clamp( iMinTex, 1L, 2L);  // 1=nearest, 2=linear
    INDEX iMinMip = _tpGlobal[0].tp_iFilter %10;      iMinMip = Clamp( iMinMip, 0L, 2L);  // 0=no mipmapping, 1=nearest, 2=linear
    _tpGlobal[0].tp_iFilter = iMagTex*100 + iMinTex*10 + iMinMip;
    _iLastTextureFiltering  = _tpGlobal[0].tp_iFilter;
  }

  // determine API and enable texturing
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_D3D || eAPI == GAT_NONE);
#else // SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_NONE);
#endif // SE1_D3D
  gfxEnableTexture();

  _sfStats.StartTimer(CStatForm::STI_BINDTEXTURE);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);
  _pfGfxProfile.StartTimer(CGfxProfile::PTI_SETCURRENTTEXTURE);
  _pfGfxProfile.IncrementTimerAveragingCounter(CGfxProfile::PTI_SETCURRENTTEXTURE);

  if( eAPI==GAT_OGL) { // OpenGL
    pglBindTexture( GL_TEXTURE_2D, ulTexObject);
    MimicTexParams_OGL(tpLocal);
  } 
#ifdef SE1_D3D
  else if( eAPI==GAT_D3D) { // Direct3D
    _ppd3dCurrentTexture = (LPDIRECT3DTEXTURE8*)&ulTexObject;
    HRESULT hr = _pGfx->gl_pd3dDevice->SetTexture( GFX_iActiveTexUnit, *_ppd3dCurrentTexture);
    D3D_CHECKERROR(hr);
    MimicTexParams_D3D(tpLocal);
  }
#endif // SE1_D3D
  // done
  _pfGfxProfile.StopTimer(CGfxProfile::PTI_SETCURRENTTEXTURE);
  _sfStats.StopTimer(CStatForm::STI_BINDTEXTURE);
  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// upload texture
extern void gfxUploadTexture( ULONG *pulTexture, PIX pixWidth, PIX pixHeight, ULONG ulFormat, BOOL bNoDiscard)
{
  // determine API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_D3D || eAPI == GAT_NONE);
#else // SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_NONE);
#endif // SE1_D3D

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  if( eAPI==GAT_OGL) { // OpenGL
    UploadTexture_OGL( pulTexture, pixWidth, pixHeight, (GLenum)ulFormat, bNoDiscard);
  }
#ifdef SE1_D3D
  else if( eAPI==GAT_D3D) { // Direct3D
    const LPDIRECT3DTEXTURE8 _pd3dLastTexture = *_ppd3dCurrentTexture;
    UploadTexture_D3D( _ppd3dCurrentTexture, pulTexture, pixWidth, pixHeight, (D3DFORMAT)ulFormat, !bNoDiscard);
    // in case that texture has been changed, must re-set it as current
    if( _pd3dLastTexture != *_ppd3dCurrentTexture) {
      HRESULT hr = _pGfx->gl_pd3dDevice->SetTexture( GFX_iActiveTexUnit, *_ppd3dCurrentTexture);
      D3D_CHECKERROR(hr);
    }
  } 
#endif // SE1_D3D
  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}




// returns size of uploaded texture
extern SLONG gfxGetTextureSize( ULONG ulTexObject, BOOL bHasMipmaps/*=TRUE*/)
{
  // nothing used if nothing uploaded
  if( ulTexObject==NULL) return 0;

  // determine API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_D3D || eAPI == GAT_NONE);
#else // SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_NONE);
#endif // SE1_D3D
  SLONG slMipSize;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // OpenGL
  if( eAPI==GAT_OGL)
  {
    // was texture compressed?
    pglBindTexture( GL_TEXTURE_2D, ulTexObject); 
    BOOL bCompressed = FALSE;
    if( _pGfx->gl_ulFlags & GLF_EXTC_ARB) {
      pglGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_ARB, (BOOL*)&bCompressed);
      OGL_CHECKERROR;
    }
    // for compressed textures, determine size directly
    if( bCompressed) {
      pglGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, (GLint*)&slMipSize);
      OGL_CHECKERROR;
    }
    // non-compressed textures goes thru determination of internal format
    else {
      PIX pixWidth, pixHeight;
      pglGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  (GLint*)&pixWidth);
      pglGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&pixHeight);
      OGL_CHECKERROR;
      slMipSize = pixWidth*pixHeight * gfxGetTexturePixRatio(ulTexObject);
    }
  }
  // Direct3D
#ifdef SE1_D3D
  else if( eAPI==GAT_D3D)
  {
    // we can determine exact size from texture surface (i.e. mipmap)
    D3DSURFACE_DESC d3dSurfDesc;
    HRESULT hr = ((LPDIRECT3DTEXTURE8)ulTexObject)->GetLevelDesc( 0, &d3dSurfDesc);
    D3D_CHECKERROR(hr);
    slMipSize = d3dSurfDesc.Size;
  }
#endif // SE1_D3D

  // eventually count in all the mipmaps (takes extra 33% of texture size)
  extern INDEX gap_bAllowSingleMipmap;
  const SLONG slUploadSize = (bHasMipmaps || !gap_bAllowSingleMipmap) ? slMipSize*4/3 : slMipSize;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
  return slUploadSize;
}



// returns bytes/pixels ratio for uploaded texture
extern INDEX gfxGetTexturePixRatio( ULONG ulTextureObject)
{
  // determine API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_D3D || eAPI == GAT_NONE);
#else // SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_NONE);
#endif // SE1_D3D
  if( eAPI==GAT_OGL) {
    return GetTexturePixRatio_OGL( (GLuint)ulTextureObject);
  } 
#ifdef SE1_D3D
  else if( eAPI==GAT_D3D) return GetTexturePixRatio_D3D( (LPDIRECT3DTEXTURE8)ulTextureObject);
#endif // SE1_D3D
  else return 0;
}


// returns bytes/pixels ratio for uploaded texture
extern INDEX gfxGetFormatPixRatio( ULONG ulTextureFormat)
{
  // determine API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_D3D || eAPI == GAT_NONE);
#else // SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_NONE);
#endif // SE1_D3D
  if( eAPI==GAT_OGL) {
    return GetFormatPixRatio_OGL( (GLenum)ulTextureFormat);
  } 
#ifdef SE1_D3D
  else if( eAPI==GAT_D3D) return GetFormatPixRatio_D3D( (D3DFORMAT)ulTextureFormat);
#endif // SE1_D3D
  else return 0;
}



// PATTERN TEXTURE FOR LINES

CTexParams _tpPattern;
extern ULONG _ulPatternTexture = NONE;
extern ULONG _ulLastUploadedPattern = 0;

// upload pattern to accelerator memory
extern void gfxSetPattern( ULONG ulPattern)
{
  // set pattern to be current texture
  _tpPattern.tp_bSingleMipmap = TRUE;
  gfxSetTextureWrapping( GFX_REPEAT, GFX_REPEAT);
  gfxSetTexture( _ulPatternTexture, _tpPattern);

  // if this pattern is currently uploaded, do nothing
  if( _ulLastUploadedPattern==ulPattern) return;

  // convert bits to ULONGs
  ULONG aulPattern[32];
  for( INDEX iBit=0; iBit<32; iBit++) {
    if( (0x80000000>>iBit) & ulPattern) aulPattern[iBit] = 0xFFFFFFFF;
    else aulPattern[iBit] = 0x00000000;
  }
  // remember new pattern and upload
  _ulLastUploadedPattern = ulPattern;
  gfxUploadTexture( &aulPattern[0], 32, 1, TS.ts_tfRGBA8, FALSE);
}



// VERTEX ARRAYS


// for D3D - (type 0=vtx, 1=nor, 2=col, 3=tex)
extern void SetVertexArray_D3D( INDEX iType, ULONG *pulVtx);


extern void gfxUnlockArrays(void)
{
  // only if locked (OpenGL can lock 'em)
  if( !_bCVAReallyLocked) return;
  ASSERT( _pGfx->gl_eCurrentAPI==GAT_OGL);
#ifndef NDEBUG
  INDEX glctRet;
  pglGetIntegerv( GL_ARRAY_ELEMENT_LOCK_COUNT_EXT, (GLint*)&glctRet);
  ASSERT( glctRet==GFX_ctVertices);
#endif
  pglUnlockArraysEXT();
  OGL_CHECKERROR;
 _bCVAReallyLocked = FALSE;
}



// OpenGL workarounds


// initialization of commond quad elements array
extern void AddQuadElements( const INDEX ctQuads)
{
  const INDEX iStart = _aiCommonQuads.Count() /6*4;
  INDEX *piQuads = _aiCommonQuads.Push(ctQuads*6); 
  for( INDEX i=0; i<ctQuads; i++) {
    piQuads[i*6 +0] = iStart+ i*4 +0;
    piQuads[i*6 +1] = iStart+ i*4 +1;
    piQuads[i*6 +2] = iStart+ i*4 +2;
    piQuads[i*6 +3] = iStart+ i*4 +2;
    piQuads[i*6 +4] = iStart+ i*4 +3;
    piQuads[i*6 +5] = iStart+ i*4 +0;
  }
}


// helper function for flushers
static void FlushArrays( INDEX *piElements, INDEX ctElements)
{
  // check
  const INDEX ctVertices = _avtxCommon.Count();
  ASSERT( _atexCommon.Count()==ctVertices);
  ASSERT( _acolCommon.Count()==ctVertices);
  extern BOOL CVA_b2D;
  gfxSetVertexArray( &_avtxCommon[0], ctVertices);
  if(CVA_b2D) gfxLockArrays();
  gfxSetTexCoordArray( &_atexCommon[0], FALSE);
  gfxSetColorArray( &_acolCommon[0]);
  gfxDrawElements( ctElements, piElements);
  gfxUnlockArrays();
}


// render quad elements to screen buffer
extern void gfxFlushQuads(void)
{
  // if there is something to draw
  const INDEX ctElements = _avtxCommon.Count()*6/4;
  if( ctElements<=0) return;
  // draw thru arrays (for OGL only) or elements?
  extern INDEX ogl_bAllowQuadArrays;
  if( _pGfx->gl_eCurrentAPI==GAT_OGL && ogl_bAllowQuadArrays) FlushArrays( NULL, _avtxCommon.Count());
  else {
    // make sure that enough quad elements has been initialized
    const INDEX ctQuads = _aiCommonQuads.Count();
    if( ctElements>ctQuads) AddQuadElements( ctElements-ctQuads); // yes, 4 times more!
    FlushArrays( &_aiCommonQuads[0], ctElements);
  }
}
 

// render elements to screen buffer
extern void gfxFlushElements(void)
{
  const INDEX ctElements = _aiCommonElements.Count();
  if( ctElements>0) FlushArrays( &_aiCommonElements[0], ctElements);
}




// set truform parameters
extern void gfxSetTruform( INDEX iLevel, BOOL bLinearNormals)
{
  // skip if Truform isn't supported
  if( _pGfx->gl_iMaxTessellationLevel<1) {
    truform_iLevel  = 0;
    truform_bLinear = FALSE;
    return;
  }
  // skip if same as last time
  iLevel = Clamp( iLevel, 0L, _pGfx->gl_iMaxTessellationLevel);
  if( truform_iLevel==iLevel && !truform_bLinear==!bLinearNormals) return;

  // determine API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_D3D || eAPI == GAT_NONE);
#else // SE1_D3D
  ASSERT(eAPI == GAT_OGL || eAPI == GAT_NONE);
#endif // SE1_D3D

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // OpenGL needs truform set here
  if( eAPI==GAT_OGL) {
    GLenum eTriMode = bLinearNormals ? GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI : GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI;
    pglPNTrianglesiATI( GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI, iLevel);
    pglPNTrianglesiATI( GL_PN_TRIANGLES_NORMAL_MODE_ATI, eTriMode);
    OGL_CHECKERROR;
  }
  // if disabled, Direct3D will set tessellation level at "enable" call
#ifdef SE1_D3D
  else if( eAPI==GAT_D3D && GFX_bTruform) { 
    FLOAT fSegments = iLevel+1;
    HRESULT hr = _pGfx->gl_pd3dDevice->SetRenderState( D3DRS_PATCHSEGMENTS, *((DWORD*)&fSegments));
    D3D_CHECKERROR(hr);
  }
#endif // SE1_D3D

  // keep current truform params
  truform_iLevel  = iLevel;
  truform_bLinear = bLinearNormals;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// readout current colormask
extern ULONG gfxGetColorMask(void)
{
  return _ulCurrentColorMask;
}



#include "GFX_wrapper_OpenGL.cpp"
#include "GFX_wrapper_Direct3D.cpp"



// DUMMY FUNCTIONS FOR NONE API
static void none_BlendFunc( GfxBlend eSrc, GfxBlend eDst) { NOTHING; }
static void none_DepthFunc( GfxComp eFunc) { NOTHING; };
static void none_DepthRange( FLOAT fMin, FLOAT fMax) { NOTHING; };
static void none_CullFace( GfxFace eFace) { NOTHING; };
static void none_ClipPlane( const DOUBLE *pdViewPlane) { NOTHING; };
static void none_SetOrtho(   const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop, const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar, const BOOL bSubPixelAdjust) { NOTHING; };
static void none_SetFrustum( const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop, const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar) { NOTHING; };
static void none_SetMatrix( const FLOAT *pfMatrix) { NOTHING; };
static void none_PolygonMode( GfxPolyMode ePolyMode) { NOTHING; };
static void none_SetTextureWrapping( enum GfxWrap eWrapU, enum GfxWrap eWrapV) { NOTHING; };
static void none_SetTextureModulation( INDEX iScale) { NOTHING; };
static void none_GenDelTexture( ULONG &ulTexObject) { NOTHING; };
static void none_SetVertexArray( GFXVertex4 *pvtx, INDEX ctVtx) { NOTHING; };
static void none_SetNormalArray( GFXNormal *pnor) { NOTHING; };
static void none_SetTexCoordArray( GFXTexCoord *ptex, BOOL b4) { NOTHING; };
static void none_SetColorArray( GFXColor *pcol) { NOTHING; };
static void none_DrawElements( INDEX ctElem, INDEX *pidx) { NOTHING; };
static void none_SetConstantColor( COLOR col) { NOTHING; };
static void none_SetColorMask( ULONG ulColorMask) { NOTHING; };



// functions initialization for OGL, D3D or NONE (dummy)
extern void GFX_SetFunctionPointers( INDEX iAPI)
{
  // OpenGL?
  if( iAPI==(INDEX)GAT_OGL)
  {
    gfxEnableDepthWrite     = &ogl_EnableDepthWrite;    
    gfxEnableDepthBias      = &ogl_EnableDepthBias;     
    gfxEnableDepthTest      = &ogl_EnableDepthTest;     
    gfxEnableAlphaTest      = &ogl_EnableAlphaTest;     
    gfxEnableBlend          = &ogl_EnableBlend;         
    gfxEnableDither         = &ogl_EnableDither;        
    gfxEnableTexture        = &ogl_EnableTexture;       
    gfxEnableClipping       = &ogl_EnableClipping;      
    gfxEnableClipPlane      = &ogl_EnableClipPlane;     
    gfxEnableTruform        = &ogl_EnableTruform;       
    gfxDisableDepthWrite    = &ogl_DisableDepthWrite;   
    gfxDisableDepthBias     = &ogl_DisableDepthBias;    
    gfxDisableDepthTest     = &ogl_DisableDepthTest;    
    gfxDisableAlphaTest     = &ogl_DisableAlphaTest;    
    gfxDisableBlend         = &ogl_DisableBlend;        
    gfxDisableDither        = &ogl_DisableDither;       
    gfxDisableTexture       = &ogl_DisableTexture;      
    gfxDisableClipping      = &ogl_DisableClipping;     
    gfxDisableClipPlane     = &ogl_DisableClipPlane;    
    gfxDisableTruform       = &ogl_DisableTruform;      
    gfxBlendFunc            = &ogl_BlendFunc;           
    gfxDepthFunc            = &ogl_DepthFunc;           
    gfxDepthRange           = &ogl_DepthRange;          
    gfxCullFace             = &ogl_CullFace;            
    gfxFrontFace            = &ogl_FrontFace;            
    gfxClipPlane            = &ogl_ClipPlane;           
    gfxSetOrtho             = &ogl_SetOrtho;            
    gfxSetFrustum           = &ogl_SetFrustum;          
    gfxSetTextureMatrix     = &ogl_SetTextureMatrix;       
    gfxSetViewMatrix        = &ogl_SetViewMatrix;       
    gfxPolygonMode          = &ogl_PolygonMode;         
    gfxSetTextureWrapping   = &ogl_SetTextureWrapping;  
    gfxSetTextureModulation = &ogl_SetTextureModulation;
    gfxGenerateTexture      = &ogl_GenerateTexture;     
    gfxDeleteTexture        = &ogl_DeleteTexture;       
    gfxSetVertexArray       = &ogl_SetVertexArray;      
    gfxSetNormalArray       = &ogl_SetNormalArray;      
    gfxSetTexCoordArray     = &ogl_SetTexCoordArray;    
    gfxSetColorArray        = &ogl_SetColorArray;       
    gfxDrawElements         = &ogl_DrawElements;        
    gfxSetConstantColor     = &ogl_SetConstantColor;    
    gfxEnableColorArray     = &ogl_EnableColorArray;    
    gfxDisableColorArray    = &ogl_DisableColorArray;   
    gfxFinish               = &ogl_Finish;              
    gfxLockArrays           = &ogl_LockArrays;          
    gfxSetColorMask         = &ogl_SetColorMask;
  }
  // Direct3D?
#ifdef SE1_D3D
  else if( iAPI==(INDEX)GAT_D3D)
  {
    gfxEnableDepthWrite     = &d3d_EnableDepthWrite;
    gfxEnableDepthBias      = &d3d_EnableDepthBias;
    gfxEnableDepthTest      = &d3d_EnableDepthTest;
    gfxEnableAlphaTest      = &d3d_EnableAlphaTest;
    gfxEnableBlend          = &d3d_EnableBlend;
    gfxEnableDither         = &d3d_EnableDither;
    gfxEnableTexture        = &d3d_EnableTexture;
    gfxEnableClipping       = &d3d_EnableClipping;
    gfxEnableClipPlane      = &d3d_EnableClipPlane;
    gfxEnableTruform        = &d3d_EnableTruform;
    gfxDisableDepthWrite    = &d3d_DisableDepthWrite;
    gfxDisableDepthBias     = &d3d_DisableDepthBias;
    gfxDisableDepthTest     = &d3d_DisableDepthTest;
    gfxDisableAlphaTest     = &d3d_DisableAlphaTest;
    gfxDisableBlend         = &d3d_DisableBlend;
    gfxDisableDither        = &d3d_DisableDither;
    gfxDisableTexture       = &d3d_DisableTexture;
    gfxDisableClipping      = &d3d_DisableClipping;
    gfxDisableClipPlane     = &d3d_DisableClipPlane;
    gfxDisableTruform       = &d3d_DisableTruform;
    gfxBlendFunc            = &d3d_BlendFunc;
    gfxDepthFunc            = &d3d_DepthFunc;
    gfxDepthRange           = &d3d_DepthRange;
    gfxCullFace             = &d3d_CullFace;
    gfxFrontFace            = &d3d_FrontFace;            
    gfxClipPlane            = &d3d_ClipPlane;
    gfxSetOrtho             = &d3d_SetOrtho;
    gfxSetFrustum           = &d3d_SetFrustum;
    gfxSetTextureMatrix     = &d3d_SetTextureMatrix;       
    gfxSetViewMatrix        = &d3d_SetViewMatrix;
    gfxPolygonMode          = &d3d_PolygonMode;
    gfxSetTextureWrapping   = &d3d_SetTextureWrapping;
    gfxSetTextureModulation = &d3d_SetTextureModulation;
    gfxGenerateTexture      = &d3d_GenerateTexture;
    gfxDeleteTexture        = &d3d_DeleteTexture;   
    gfxSetVertexArray       = &d3d_SetVertexArray;  
    gfxSetNormalArray       = &d3d_SetNormalArray;  
    gfxSetTexCoordArray     = &d3d_SetTexCoordArray;
    gfxSetColorArray        = &d3d_SetColorArray;   
    gfxDrawElements         = &d3d_DrawElements;    
    gfxSetConstantColor     = &d3d_SetConstantColor;
    gfxEnableColorArray     = &d3d_EnableColorArray;
    gfxDisableColorArray    = &d3d_DisableColorArray;
    gfxFinish               = &d3d_Finish;
    gfxLockArrays           = &d3d_LockArrays;
    gfxSetColorMask         = &d3d_SetColorMask;
  }
#endif // SE1_D3D
  // NONE!
  else
  {
    gfxEnableDepthWrite     = &none_void;
    gfxEnableDepthBias      = &none_void;
    gfxEnableDepthTest      = &none_void;
    gfxEnableAlphaTest      = &none_void;
    gfxEnableBlend          = &none_void;
    gfxEnableDither         = &none_void;
    gfxEnableTexture        = &none_void;
    gfxEnableClipping       = &none_void;
    gfxEnableClipPlane      = &none_void;
    gfxEnableTruform        = &none_void;
    gfxDisableDepthWrite    = &none_void;
    gfxDisableDepthBias     = &none_void;
    gfxDisableDepthTest     = &none_void;
    gfxDisableAlphaTest     = &none_void;
    gfxDisableBlend         = &none_void;
    gfxDisableDither        = &none_void;
    gfxDisableTexture       = &none_void;
    gfxDisableClipping      = &none_void;
    gfxDisableClipPlane     = &none_void;
    gfxDisableTruform       = &none_void;
    gfxBlendFunc            = &none_BlendFunc;
    gfxDepthFunc            = &none_DepthFunc;
    gfxDepthRange           = &none_DepthRange;
    gfxCullFace             = &none_CullFace;
    gfxFrontFace            = &none_CullFace;
    gfxClipPlane            = &none_ClipPlane;
    gfxSetOrtho             = &none_SetOrtho;
    gfxSetFrustum           = &none_SetFrustum;
    gfxSetTextureMatrix     = &none_SetMatrix;
    gfxSetViewMatrix        = &none_SetMatrix;
    gfxPolygonMode          = &none_PolygonMode;
    gfxSetTextureWrapping   = &none_SetTextureWrapping;
    gfxSetTextureModulation = &none_SetTextureModulation;
    gfxGenerateTexture      = &none_GenDelTexture;
    gfxDeleteTexture        = &none_GenDelTexture;   
    gfxSetVertexArray       = &none_SetVertexArray;  
    gfxSetNormalArray       = &none_SetNormalArray;  
    gfxSetTexCoordArray     = &none_SetTexCoordArray;
    gfxSetColorArray        = &none_SetColorArray;   
    gfxDrawElements         = &none_DrawElements;    
    gfxSetConstantColor     = &none_SetConstantColor;
    gfxEnableColorArray     = &none_void;
    gfxDisableColorArray    = &none_void;
    gfxFinish               = &none_void;
    gfxLockArrays           = &none_void;
    gfxSetColorMask         = &none_SetColorMask;
  }
}
