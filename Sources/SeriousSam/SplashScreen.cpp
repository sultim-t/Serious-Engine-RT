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

#include "StdH.h"
#include "resource.h"

#define NAME "Splash"

static HBITMAP _hbmSplash = NULL;
static BITMAP _bmSplash;
static HBITMAP _hbmSplashMask = NULL;
static BITMAP _bmSplashMask;
static HWND hwnd = NULL;

static long FAR PASCAL SplashWindowProc( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{
  switch( message ) {
  case WM_PAINT: {
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps); 

    HDC hdcMem = CreateCompatibleDC(ps.hdc); 
    SelectObject(hdcMem, _hbmSplashMask); 
    BitBlt(ps.hdc, 0, 0, _bmSplash.bmWidth, _bmSplash.bmHeight, hdcMem, 0, 0, 
      SRCAND); 
    SelectObject(hdcMem, _hbmSplash); 
    BitBlt(ps.hdc, 0, 0, _bmSplash.bmWidth, _bmSplash.bmHeight, hdcMem, 0, 0, 
      SRCPAINT); 

    DeleteDC(hdcMem); 
    EndPaint(hWnd, &ps); 
   
    return 0;
                 } break;
  case WM_ERASEBKGND: return 1; break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

void ShowSplashScreen(HINSTANCE hInstance)
{

  _hbmSplash = LoadBitmapA(hInstance, (char*)IDB_SPLASH);
  if (_hbmSplash==NULL) {
    return;
  }
  _hbmSplashMask = LoadBitmapA(hInstance, (char*)IDB_SPLASHMASK);
  if (_hbmSplashMask==NULL) {
    return;
  }

  GetObject(_hbmSplash, sizeof(BITMAP), (LPSTR) &_bmSplash); 
  GetObject(_hbmSplashMask, sizeof(BITMAP), (LPSTR) &_bmSplashMask);
  if (_bmSplashMask.bmWidth  != _bmSplash.bmWidth
    ||_bmSplashMask.bmHeight != _bmSplash.bmHeight) {
    return;
  }

	int iScreenX = ::GetSystemMetrics(SM_CXSCREEN);	// screen size
	int iScreenY = ::GetSystemMetrics(SM_CYSCREEN);

  WNDCLASSA wc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = SplashWindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon( hInstance, (LPCTSTR)IDR_MAINFRAME );
  wc.hCursor = LoadCursor( NULL, IDC_ARROW );
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NAME;
  wc.lpszClassName = NAME;
  RegisterClassA(&wc);

  /*
   * create a window
   */
  hwnd = CreateWindowExA(
	  WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW,
	  NAME,
	  "SeriousSam loading...",   // title
    WS_POPUP,
	  iScreenX/2-_bmSplash.bmWidth/2,
	  iScreenY/2-_bmSplash.bmHeight/2,
	  _bmSplash.bmWidth,_bmSplash.bmHeight,  // window size
	  NULL,
	  NULL,
	  hInstance,
	  NULL);

  if(!hwnd) {
	  return;
  }
 
  ShowWindow( hwnd, SW_SHOW);
  RECT rect;
  GetClientRect(hwnd, &rect); 
  InvalidateRect(hwnd, &rect, TRUE); 
  UpdateWindow(hwnd); 
}

void HideSplashScreen(void)
{
  if (hwnd==NULL) {
    return;
  }
  DestroyWindow(hwnd);
  DeleteObject(_hbmSplash);
  DeleteObject(_hbmSplashMask);
}
