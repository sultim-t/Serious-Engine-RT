/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_VIEWPORT_H
#define SE_INCL_VIEWPORT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Graphics/Raster.h>
#ifdef SE1_D3D
#include <d3d8.h>
#endif // SE1_D3D

/*
 *  ViewPort
 */

/* rcg !!! FIXME: This will need to go away. */
#ifdef PLATFORM_WIN32
class CTempDC {
public:
  HDC hdc;
  HWND hwnd;
  CTempDC(HWND hWnd);
  ~CTempDC(void);
};
#endif

// base abstract class for viewport
class ENGINE_API CViewPort {
public:
// implementation
  HWND vp_hWnd;                 // canvas (child) window
  HWND vp_hWndParent;           // window of the viewport
  CRaster vp_Raster;            // the used Raster
#ifdef SE1_D3D
	LPDIRECT3DSWAPCHAIN8 vp_pSwapChain;  // swap chain for D3D
	LPDIRECT3DSURFACE8   vp_pSurfDepth;  // z-buffer for D3D
#endif // SE1_D3D
  INDEX vp_ctDisplayChanges;    // number of display driver

  // open/close canvas window
  void OpenCanvas(void);
  void CloseCanvas(BOOL bRelease=FALSE);

// interface
  /* Constructor for given window. */
  CViewPort(PIX pixWidth, PIX pixHeight, HWND hWnd);
	/* Destructor. */
  ~CViewPort(void);

	/* Display the back buffer on screen. */
  void SwapBuffers(void);
  // change size of this viewport, it's raster and all it's drawports to fit it window
  void Resize(void);
};


#endif  /* include-once check. */

