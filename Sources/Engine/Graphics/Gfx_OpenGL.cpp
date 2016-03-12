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
#include <Engine/Base/Translation.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Memory.h>
#include <Engine/Base/Console.h>

#include <Engine/Graphics/ViewPort.h>
#include <Engine/Graphics/MultiMonitor.h>

#include <Engine/Templates/DynamicContainer.cpp>
#include <Engine/Templates/Stock_CTextureData.h>

#include <Engine/Base/ListIterator.inl>


extern INDEX ogl_iTBufferEffect;
extern INDEX ogl_iTBufferSamples;


// fog/haze textures
extern ULONG _fog_ulTexture;
extern ULONG _haze_ulTexture;

// change control
extern INDEX GFX_ctVertices;


// attributes for t-buffer
int aiAttribList[] = {
  WGL_DRAW_TO_WINDOW_EXT, TRUE,
  WGL_SUPPORT_OPENGL_EXT, TRUE,
  WGL_DOUBLE_BUFFER_EXT, TRUE,
	WGL_PIXEL_TYPE_EXT, WGL_TYPE_RGBA_EXT,
  WGL_COLOR_BITS_EXT, 16,
  WGL_DEPTH_BITS_EXT, 16,
	WGL_SAMPLE_BUFFERS_3DFX, 1,
  WGL_SAMPLES_3DFX, 4,
	0, 0
};
int *piAttribList = aiAttribList;


// engine's internal opengl state variables
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
extern BOOL  glbUsingVARs = FALSE;   // vertex_array_range


// define gl function pointers
#define DLLFUNCTION(dll, output, name, inputs, params, required) \
  output (__stdcall *p##name) inputs = NULL;
#include "gl_functions.h"
#undef DLLFUNCTION

// extensions
void (__stdcall *pglLockArraysEXT)(GLint first, GLsizei count) = NULL;
void (__stdcall *pglUnlockArraysEXT)(void) = NULL;

GLboolean (__stdcall *pwglSwapIntervalEXT)(GLint interval) = NULL;
GLint     (__stdcall *pwglGetSwapIntervalEXT)(void) = NULL;

void (__stdcall *pglActiveTextureARB)(GLenum texunit) = NULL;
void (__stdcall *pglClientActiveTextureARB)(GLenum texunit) = NULL;

// t-buffer support
char *(__stdcall *pwglGetExtensionsStringARB)(HDC hdc);
BOOL  (__stdcall *pwglChoosePixelFormatARB)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
BOOL  (__stdcall *pwglGetPixelFormatAttribivARB)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int *piAttributes, int *piValues);
void  (__stdcall *pglTBufferMask3DFX)(GLuint mask);

// NV occlusion query
void (__stdcall *pglGenOcclusionQueriesNV)( GLsizei n, GLuint *ids);
void (__stdcall *pglDeleteOcclusionQueriesNV)( GLsizei n, const GLuint *ids);
void (__stdcall *pglBeginOcclusionQueryNV)( GLuint id);
void (__stdcall *pglEndOcclusionQueryNV)(void);
void (__stdcall *pglGetOcclusionQueryivNV)( GLuint id, GLenum pname, GLint *params);
void (__stdcall *pglGetOcclusionQueryuivNV)( GLuint id, GLenum pname, GLuint *params);
GLboolean (__stdcall *pglIsOcclusionQueryNV)( GLuint id);

// ATI GL_ATI_pn_triangles
void (__stdcall *pglPNTrianglesiATI)( GLenum pname, GLint param);
void (__stdcall *pglPNTrianglesfATI)( GLenum pname, GLfloat param);


void WIN_CheckError(BOOL bRes, const char *strDescription)
{
  if( bRes) return;
  DWORD dwWindowsErrorCode = GetLastError();
  if( dwWindowsErrorCode==ERROR_SUCCESS) return; // ignore stupid 'successful' error 
  WarningMessage("%s: %s", strDescription, GetWindowsError(dwWindowsErrorCode));
}


static void FailFunction_t(const char *strName) {
  ThrowF_t(TRANS("Required function %s not found."), strName);
}


static void OGL_SetFunctionPointers_t(HINSTANCE hiOGL)
{
  const char *strName;
  // get gl function pointers
  #define DLLFUNCTION(dll, output, name, inputs, params, required) \
    strName = #name;  \
    p##name = (output (__stdcall*) inputs) GetProcAddress( hi##dll, strName); \
    if( required && p##name == NULL) FailFunction_t(strName);
  #include "gl_functions.h"
  #undef DLLFUNCTION
}


static void OGL_ClearFunctionPointers(void)
{
  // clear gl function pointers
  #define DLLFUNCTION(dll, output, name, inputs, params, required) p##name = NULL;
  #include "gl_functions.h"
  #undef DLLFUNCTION
}




#define BACKOFF pwglMakeCurrent( NULL, NULL); \
	              pwglDeleteContext( hglrc); \
	              ReleaseDC( dummyhwnd, hdc); \
	              DestroyWindow( dummyhwnd); \
            	  UnregisterClassA( classname, hInstance);



// helper for choosing t-buffer's pixel format
static BOOL _TBCapability = FALSE;
static INDEX ChoosePixelFormatTB( HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd,
                                  PIX pixResWidth, PIX pixResHeight)
{
  _TBCapability = FALSE;
	char *extensions = NULL;
	char *wglextensions = NULL;
	HGLRC hglrc; 
	HWND dummyhwnd;
	WNDCLASSA cls;
  HINSTANCE hInstance = GetModuleHandle(NULL);
	LPCSTR classname = "dummyOGLwin";
	cls.style = CS_OWNDC;
	cls.lpfnWndProc = DefWindowProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = hInstance;
	cls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor(NULL, IDC_WAIT);
	cls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = classname;
  // didn't manage to register class?
	if( !RegisterClassA(&cls))	return 0;

	// create window fullscreen 
  //CPrintF( "  Dummy window: %d x %d\n", pixResWidth, pixResHeight);
	dummyhwnd = CreateWindowExA( WS_EX_TOPMOST, classname, "Dummy OGL window",
                              WS_POPUP|WS_VISIBLE, 0, 0, pixResWidth, pixResHeight,
                              NULL, NULL, hInstance, NULL);
  // didn't make it?
  if( dummyhwnd == NULL) {
	  UnregisterClassA( classname, hInstance);
    return 0;
  }
  //CPrintF( "  Dummy passed...\n");
	hdc = GetDC(dummyhwnd);
  // try to choose pixel format
	int iPixelFormat = pwglChoosePixelFormat( hdc, ppfd);
	if( !iPixelFormat) {
    ReleaseDC( dummyhwnd, hdc);
    DestroyWindow(dummyhwnd);
	  UnregisterClassA( classname, hInstance);
	  return 0;
	}
  //CPrintF( "  Choose pixel format passed...\n");
  // try to set pixel format
	if( !pwglSetPixelFormat( hdc, iPixelFormat, ppfd)) {
    ReleaseDC( dummyhwnd, hdc);
    DestroyWindow(dummyhwnd);
	  UnregisterClassA( classname, hInstance);
	  return 0;
	}
  //CPrintF( "  Set pixel format passed...\n");

	// create context using the default accelerated pixelformat that was passed
	hglrc = pwglCreateContext(hdc);
	pwglMakeCurrent( hdc, hglrc);
	// update the value list with information passed from the ppfd.
	aiAttribList[ 9] =  ppfd->cColorBits;
	aiAttribList[11] =  ppfd->cDepthBits;
	aiAttribList[15] = _pGfx->go_ctSampleBuffers;

	// get the extension list.
	extensions = (char*)pglGetString(GL_EXTENSIONS);
	// get the wgl extension list.
	if( strstr((const char*)extensions, "WGL_EXT_extensions_string ") != NULL)
  { // windows extension string supported
    pwglGetExtensionsStringARB = (char* (__stdcall*)(HDC))pwglGetProcAddress( "wglGetExtensionsStringARB");
    if( pwglGetExtensionsStringARB == NULL) {
      BACKOFF
      return 0;
    }
    //CPrintF( "  WGL extension string passed...\n");
		// get WGL extension string
		wglextensions = (char*)pwglGetExtensionsStringARB(hdc);
 	}
  else {
    BACKOFF
    return 0;
	}

 	// check for the pixel format and multisample extension strings
 	if( (strstr((const char*)wglextensions, "WGL_ARB_pixel_format ") != NULL) && 
      (strstr((const char*)extensions,    "GL_3DFX_multisample ")  != NULL)) {
    // 3dfx extensions present
    _TBCapability = TRUE;
    pwglChoosePixelFormatARB      = (BOOL (__stdcall*)(HDC,const int*,const FLOAT*,UINT,int*,UINT*))pwglGetProcAddress( "wglChoosePixelFormatARB");
    pwglGetPixelFormatAttribivARB = (BOOL (__stdcall*)(HDC,int,int,UINT,int*,int*)                 )pwglGetProcAddress( "wglGetPixelFormatAttribivARB");
		pglTBufferMask3DFX = (void (__stdcall*)(GLuint))pwglGetProcAddress("glTBufferMask3DFX");
    if( pwglChoosePixelFormatARB==NULL && pglTBufferMask3DFX==NULL) {
      BACKOFF
      return 0;
		}
    //CPrintF( "  WGL choose pixel format present...\n");
    int iAttribListNum = {WGL_NUMBER_PIXEL_FORMATS_EXT};
  	int iMaxFormats    = 1; // default number to return
		if( pwglGetPixelFormatAttribivARB!=NULL) {
			// get total number of formats supported.
			pwglGetPixelFormatAttribivARB( hdc, NULL, NULL, 1, &iAttribListNum, &iMaxFormats);
      //CPrintF( "Max formats: %d\n", iMaxFormats);
		}
    UINT uiNumFormats;
	  int *piFormats = (int*)AllocMemory( sizeof(UINT) *iMaxFormats);
    // try to get all formats that fit the pixel format criteria
    if( !pwglChoosePixelFormatARB( hdc, piAttribList, NULL, iMaxFormats, piFormats, &uiNumFormats)) {
      FreeMemory(piFormats);
      BACKOFF
      return 0;
    }
    //CPrintF( "  WGL choose pixel format passed...\n");
		// return the first match for now
    iPixelFormat = 0;
    if( uiNumFormats>0) {
      iPixelFormat = piFormats[0];
      //CPrintF( "Num formats: %d\n", uiNumFormats);
      //CPrintF( "First format: %d\n", iPixelFormat);
    }
    FreeMemory(piFormats);
  }
	else
  {	// wglChoosePixelFormatARB extension does not exist :(
		iPixelFormat = 0;
	}
  BACKOFF
  return iPixelFormat;
}


// prepares pixel format for OpenGL context
BOOL CGfxLibrary::SetupPixelFormat_OGL( HDC hdc, BOOL bReport/*=FALSE*/)
{
  int iPixelFormat = 0;
  const PIX pixResWidth  = gl_dmCurrentDisplayMode.dm_pixSizeI;
  const PIX pixResHeight = gl_dmCurrentDisplayMode.dm_pixSizeJ;
  const DisplayDepth dd  = gl_dmCurrentDisplayMode.dm_ddDepth;

  PIXELFORMATDESCRIPTOR pfd;
  memset( &pfd, 0, sizeof(pfd));
  pfd.nSize      = sizeof(pfd);
  pfd.nVersion   = 1;
  pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;

  // clamp depth/stencil values
  extern INDEX gap_iDepthBits;
  extern INDEX gap_iStencilBits;
       if( gap_iDepthBits <12) gap_iDepthBits   = 0;
  else if( gap_iDepthBits <22) gap_iDepthBits   = 16;
  else if( gap_iDepthBits <28) gap_iDepthBits   = 24;
  else                         gap_iDepthBits   = 32;
       if( gap_iStencilBits<3) gap_iStencilBits = 0;
  else if( gap_iStencilBits<7) gap_iStencilBits = 4;
  else                         gap_iStencilBits = 8;

  // set color/depth buffer values
  pfd.cColorBits   = (dd!=DD_16BIT) ? 32 : 16;
  pfd.cDepthBits   = gap_iDepthBits;
  pfd.cStencilBits = gap_iStencilBits;

  // must be required and works only in full screen via GDI functions
  ogl_iTBufferEffect = Clamp( ogl_iTBufferEffect, 0L, 2L);
  if( ogl_iTBufferEffect>0 && pixResWidth>0 && pixResHeight>0)
  { // lets T-buffer ... :)
    //CPrintF( "TBuffer init...\n");
    ogl_iTBufferSamples = (1L) << FastLog2(ogl_iTBufferSamples);
    if( ogl_iTBufferSamples<2) ogl_iTBufferSamples = 4;
    go_ctSampleBuffers = ogl_iTBufferSamples;
    go_iCurrentWriteBuffer = 0;
    iPixelFormat = ChoosePixelFormatTB( hdc, &pfd, pixResWidth, pixResHeight);
	  // need to reset the desktop resolution because CPFTB() resets it
    BOOL bSuccess = CDS_SetMode( pixResWidth, pixResHeight, dd);
    if( !bSuccess) iPixelFormat = 0;
    // check T-buffer support
    if( _TBCapability) pglGetIntegerv( GL_SAMPLES_3DFX, (GLint*)&go_ctSampleBuffers);
    if( !iPixelFormat) { ogl_iTBufferEffect=0; CPrintF( TRANS("TBuffer initialization failed.\n")); }
    else CPrintF( TRANS("TBuffer initialization passed (%d buffers in use).\n"), go_ctSampleBuffers);
  }

  // if T-buffer didn't make it, let's try thru regular path
  if( !iPixelFormat) {
    go_ctSampleBuffers = 0;
    go_iCurrentWriteBuffer = 0;
    iPixelFormat = pwglChoosePixelFormat( hdc, &pfd);
  }

  if( !iPixelFormat) {
    WIN_CHECKERROR( 0, "ChoosePixelFormat");
    return FALSE;
  }
  if( !pwglSetPixelFormat( hdc, iPixelFormat, &pfd)) {
    WIN_CHECKERROR( 0, "SetPixelFormat");
    return FALSE;
  }

  // test acceleration
  memset( &pfd, 0, sizeof(pfd));
  if( !pwglDescribePixelFormat( hdc, iPixelFormat, sizeof(pfd), &pfd)) return FALSE;
  BOOL bGenericFormat      = pfd.dwFlags & PFD_GENERIC_FORMAT;
  BOOL bGenericAccelerated = pfd.dwFlags & PFD_GENERIC_ACCELERATED;
  BOOL bHasAcceleration    = (bGenericFormat &&  bGenericAccelerated) ||  // MCD
                            (!bGenericFormat && !bGenericAccelerated);    // ICD
  if( bHasAcceleration) gl_ulFlags |=  GLF_HASACCELERATION;
  else                  gl_ulFlags &= ~GLF_HASACCELERATION;

  // done if report pixel format info isn't required
  if( !bReport) return TRUE;

  // prepare pixel type description
  CTString strPixelType;
  if( pfd.iPixelType==PFD_TYPE_RGBA) strPixelType = "TYPE_RGBA"; 
  else if( pfd.iPixelType&PFD_TYPE_COLORINDEX) strPixelType = "TYPE_COLORINDEX";
  else strPixelType = "unknown";
  // prepare flags description
  CTString strFlags = "";
  if( pfd.dwFlags&PFD_DRAW_TO_WINDOW)        strFlags += "DRAW_TO_WINDOW "; 
  if( pfd.dwFlags&PFD_DRAW_TO_BITMAP)        strFlags += "DRAW_TO_BITMAP "; 
  if( pfd.dwFlags&PFD_SUPPORT_GDI)           strFlags += "SUPPORT_GDI "; 
  if( pfd.dwFlags&PFD_SUPPORT_OPENGL)        strFlags += "SUPPORT_OPENGL "; 
  if( pfd.dwFlags&PFD_GENERIC_ACCELERATED)   strFlags += "GENERIC_ACCELERATED "; 
  if( pfd.dwFlags&PFD_GENERIC_FORMAT)        strFlags += "GENERIC_FORMAT "; 
  if( pfd.dwFlags&PFD_NEED_PALETTE)          strFlags += "NEED_PALETTE "; 
  if( pfd.dwFlags&PFD_NEED_SYSTEM_PALETTE)   strFlags += "NEED_SYSTEM_PALETTE "; 
  if( pfd.dwFlags&PFD_DOUBLEBUFFER)          strFlags += "DOUBLEBUFFER "; 
  if( pfd.dwFlags&PFD_STEREO)                strFlags += "STEREO "; 
  if( pfd.dwFlags&PFD_SWAP_LAYER_BUFFERS)    strFlags += "SWAP_LAYER_BUFFERS "; 
  if( pfd.dwFlags&PFD_DEPTH_DONTCARE)        strFlags += "DEPTH_DONTCARE "; 
  if( pfd.dwFlags&PFD_DOUBLEBUFFER_DONTCARE) strFlags += "DOUBLEBUFFER_DONTCARE "; 
  if( pfd.dwFlags&PFD_STEREO_DONTCARE)       strFlags += "STEREO_DONTCARE "; 
  if( pfd.dwFlags&PFD_SWAP_COPY)             strFlags += "SWAP_COPY "; 
  if( pfd.dwFlags&PFD_SWAP_EXCHANGE)         strFlags += "SWAP_EXCHANGE "; 
  if( strFlags=="") strFlags = "none";
                              
  // output pixel format description to console (for debugging purposes)
  CPrintF( TRANS("\nPixel Format Description:\n"));
  CPrintF( TRANS("  Number:     %d (%s)\n"), iPixelFormat, strPixelType);
  CPrintF( TRANS("  Flags:      %s\n"), strFlags);
  CPrintF( TRANS("  Color bits: %d (%d:%d:%d:%d)\n"), pfd.cColorBits, 
           pfd.cRedBits, pfd.cGreenBits, pfd.cBlueBits, pfd.cAlphaBits);
  CPrintF( TRANS("  Depth bits: %d (%d for stencil)\n"), pfd.cDepthBits, pfd.cStencilBits);
  gl_iCurrentDepth = pfd.cDepthBits; // keep depth bits
  
  // all done
  CPrintF( "\n");
  return TRUE;
}


// test if an extension exists
static BOOL HasExtension( const char *strAllExtensions, const char *strExtension)
{
  // find substring
  const char *strFound = strstr( strAllExtensions, strExtension);
  //  no extension if not found
  if( strFound==NULL) return FALSE;
  INDEX iExtensionLen = strlen(strExtension);
  // if found substring is substring of some other extension
  if( strFound[iExtensionLen]!=' ' && strFound[iExtensionLen]!=0) {
    // continue searching after that char
    return HasExtension( strFound+iExtensionLen, strExtension);
  }
  // extension found
  return TRUE; 
}


// add OpenGL extensions to engine
void CGfxLibrary::AddExtension_OGL( ULONG ulFlag, const char *strName)
{
  gl_ulFlags = (gl_ulFlags & ~ulFlag) | ulFlag;
  go_strSupportedExtensions += strName;
  go_strSupportedExtensions += " ";
}


// determine OpenGL extensions that engine supports
void CGfxLibrary::TestExtension_OGL( ULONG ulFlag, const char *strName)
{
  if( HasExtension( go_strExtensions, strName)) AddExtension_OGL( ulFlag, strName);
}


// creates OpenGL drawing context
BOOL CGfxLibrary::CreateContext_OGL(HDC hdc)
{
  if( !SetupPixelFormat_OGL( hdc, TRUE)) return FALSE;
  go_hglRC = pwglCreateContext(hdc);
  if( go_hglRC==NULL) {
    WIN_CHECKERROR(0, "CreateContext");
    return FALSE;
  }
  if( !pwglMakeCurrent(hdc, go_hglRC)) {
    // NOTE: This error is sometimes reported without a reason on 3dfx hardware
    // so we just have to ignore it.
    //WIN_CHECKERROR(0, "MakeCurrent after CreateContext");
    return FALSE;
  }
  return TRUE;
}



// prepares OpenGL drawing context
void CGfxLibrary::InitContext_OGL(void)
{
  // must have context
  ASSERT( gl_pvpActive!=NULL);

  // reset engine's internal OpenGL state variables
  extern BOOL GFX_abTexture[GFX_MAXTEXUNITS];
  for( INDEX iUnit=0; iUnit<GFX_MAXTEXUNITS; iUnit++) {
    GFX_abTexture[iUnit] = FALSE;
    GFX_iTexModulation[iUnit] = 1;
  }
  // set default texture unit and modulation mode
  GFX_iActiveTexUnit = 0;
  gl_ctMaxStreams = 16; // GL always has enough "streams" for multi-texturing
  // reset frustum/ortho stuff
  extern BOOL  GFX_bViewMatrix;
  extern FLOAT GFX_fLastL, GFX_fLastR, GFX_fLastT, GFX_fLastB, GFX_fLastN, GFX_fLastF;
  GFX_fLastL = GFX_fLastR = GFX_fLastT = GFX_fLastB = GFX_fLastN = GFX_fLastF = 0;
  GFX_bViewMatrix = TRUE;

  glbUsingVARs  = FALSE;
  GFX_bTruform  = FALSE;
  GFX_bClipping = TRUE;
  pglEnable(  GL_TEXTURE_2D);     GFX_abTexture[0] = TRUE;
  pglEnable(  GL_DITHER);         GFX_bDithering   = TRUE;
  pglDisable( GL_BLEND);          GFX_bBlending    = FALSE;
  pglDisable( GL_DEPTH_TEST);     GFX_bDepthTest   = FALSE;
  pglDisable( GL_ALPHA_TEST);     GFX_bAlphaTest   = FALSE;
  pglDisable( GL_CLIP_PLANE0);    GFX_bClipPlane   = FALSE;
  pglDisable( GL_CULL_FACE);      GFX_eCullFace    = GFX_NONE;
  pglFrontFace( GL_CCW);          GFX_bFrontFace   = TRUE;
  pglDepthMask( GL_FALSE);        GFX_bDepthWrite  = FALSE;
  pglDepthFunc( GL_LEQUAL);       GFX_eDepthFunc   = GFX_LESS_EQUAL;
  pglBlendFunc( GL_ONE, GL_ONE);  GFX_eBlendSrc = GFX_eBlendDst = GFX_ONE;
  pglDepthRange( 0.0f, 1.0f);     GFX_fMinDepthRange = 0.0f;
                                  GFX_fMaxDepthRange = 1.0f;
  // (re)set some OpenGL defaults
  gfxPolygonMode( GFX_FILL);
  pglShadeModel( GL_SMOOTH);
  pglEnable( GL_SCISSOR_TEST);
  pglDrawBuffer( GL_BACK);
  pglAlphaFunc( GL_GEQUAL, 0.5f);
  pglColor4f( 1.0f, 1.0f, 1.0f, 1.0f);
  pglMatrixMode( GL_MODELVIEW);
  pglLoadIdentity();
  pglMatrixMode( GL_TEXTURE);
  pglLoadIdentity();
  // enable rendering only thru vertex arrays by default
  pglEnableClientState(  GL_VERTEX_ARRAY);
  pglDisableClientState( GL_NORMAL_ARRAY);
  pglDisableClientState( GL_TEXTURE_COORD_ARRAY);
  pglDisableClientState( GL_COLOR_ARRAY);
  GFX_bColorArray = FALSE;

  // set single byte pixel alignment
  pglPixelStorei( GL_PACK_ALIGNMENT,   1);
  pglPixelStorei( GL_UNPACK_ALIGNMENT, 1);

  // TEST EXTENSIONS
  CDisplayAdapter &da = gl_gaAPI[GAT_OGL].ga_adaAdapter[gl_iCurrentAdapter];
  da.da_strVendor   = (const char*)pglGetString(GL_VENDOR);
  da.da_strRenderer = (const char*)pglGetString(GL_RENDERER);
  da.da_strVersion  = (const char*)pglGetString(GL_VERSION);
  go_strExtensions  = (const char*)pglGetString(GL_EXTENSIONS);

  // report
  CPrintF( TRANS("\n* OpenGL context created: *----------------------------------\n"));
  CPrintF( "  (%s, %s, %s)\n\n", da.da_strVendor, da.da_strRenderer, da.da_strVersion);

  // test for used extensions
  GLint   gliRet;
  GLfloat glfRet;
  go_strSupportedExtensions = "";

  // check for WGL extensions, too
  go_strWinExtensions = "";
  pwglGetExtensionsStringARB = (char* (__stdcall*)(HDC))pwglGetProcAddress("wglGetExtensionsStringARB");
  if( pwglGetExtensionsStringARB != NULL) {
    AddExtension_OGL( NONE, "WGL_ARB_extensions_string"); // register
    CTempDC tdc(gl_pvpActive->vp_hWnd);
    go_strWinExtensions = (char*)pwglGetExtensionsStringARB(tdc.hdc);
  }

  // multitexture is supported only thru GL_EXT_texture_env_combine extension
  gl_ctTextureUnits = 1;
  gl_ctRealTextureUnits = 1;
  pglActiveTextureARB       = NULL;
  pglClientActiveTextureARB = NULL;
  if( HasExtension( go_strExtensions, "GL_ARB_multitexture")) {
    pglGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, (int*)&gl_ctRealTextureUnits); // get number of texture units
    if( gl_ctRealTextureUnits>1 && HasExtension( go_strExtensions, "GL_EXT_texture_env_combine")) {
      AddExtension_OGL( NONE, "GL_ARB_multitexture");
      AddExtension_OGL( NONE, "GL_EXT_texture_env_combine");
      pglActiveTextureARB       = (void (__stdcall*)(GLenum))pwglGetProcAddress( "glActiveTextureARB");
      pglClientActiveTextureARB = (void (__stdcall*)(GLenum))pwglGetProcAddress( "glClientActiveTextureARB");
      ASSERT( pglActiveTextureARB!=NULL && pglClientActiveTextureARB!=NULL);
      gl_ctTextureUnits = Min( GFX_MAXTEXUNITS, gl_ctRealTextureUnits);
    } else {
      CPrintF( TRANS("  GL_TEXTURE_ENV_COMBINE extension missing - multi-texturing cannot be used.\n"));
    }
  }

  // find all supported texture compression extensions
  TestExtension_OGL( GLF_EXTC_ARB,    "GL_ARB_texture_compression");
  TestExtension_OGL( GLF_EXTC_S3TC,   "GL_EXT_texture_compression_s3tc");
  TestExtension_OGL( GLF_EXTC_FXT1,   "GL_3DFX_texture_compression_FXT1");
  TestExtension_OGL( GLF_EXTC_LEGACY, "GL_S3_s3tc");
  // mark if there is at least one extension present
  gl_ulFlags &= ~GLF_TEXTURECOMPRESSION;
  if( (gl_ulFlags&GLF_EXTC_ARB)  || (gl_ulFlags&GLF_EXTC_FXT1)
   || (gl_ulFlags&GLF_EXTC_S3TC) || (gl_ulFlags&GLF_EXTC_LEGACY)) {
    gl_ulFlags |= GLF_TEXTURECOMPRESSION;
  }

  // determine max supported dimension of texture
  pglGetIntegerv( GL_MAX_TEXTURE_SIZE, (GLint*)&gl_pixMaxTextureDimension);
  OGL_CHECKERROR;

  // determine support for texture LOD biasing
  gl_fMaxTextureLODBias = 0.0f;
  if( HasExtension( go_strExtensions, "GL_EXT_texture_lod_bias")) {
    AddExtension_OGL( NONE, "GL_EXT_texture_lod_bias"); // register
    // check max possible lod bias (absolute)
    pglGetFloatv( GL_MAX_TEXTURE_LOD_BIAS_EXT, &glfRet);
    GLenum gleError = pglGetError();  // just because of invalid extension implementations (S3)
    if( gleError || glfRet<0.1f || glfRet>4.0f) glfRet = 4.0f;
    gl_fMaxTextureLODBias = glfRet;
    OGL_CHECKERROR;
  }

  // determine support for anisotropic filtering
  gl_iMaxTextureAnisotropy = 1;
  if( HasExtension( go_strExtensions, "GL_EXT_texture_filter_anisotropic")) {
    AddExtension_OGL( NONE, "GL_EXT_texture_filter_anisotropic"); // register
    // keep max allowed anisotropy degree
    pglGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gliRet);
    gl_iMaxTextureAnisotropy = gliRet;
    OGL_CHECKERROR;
  }

  // check support for compiled vertex arrays
  pglLockArraysEXT   = NULL;
  pglUnlockArraysEXT = NULL;
  if( HasExtension( go_strExtensions, "GL_EXT_compiled_vertex_array")) {
    AddExtension_OGL( GLF_EXT_COMPILEDVERTEXARRAY, "GL_EXT_compiled_vertex_array");
    pglLockArraysEXT   = (void (__stdcall*)(GLint,GLsizei))pwglGetProcAddress( "glLockArraysEXT");
    pglUnlockArraysEXT = (void (__stdcall*)(void)         )pwglGetProcAddress( "glUnlockArraysEXT");
    ASSERT( pglLockArraysEXT!=NULL && pglUnlockArraysEXT!=NULL);
  }

  // check support for swap interval
  pwglSwapIntervalEXT    = NULL;
  pwglGetSwapIntervalEXT = NULL;
  if( HasExtension( go_strExtensions, "WGL_EXT_swap_control")) {
    AddExtension_OGL( GLF_VSYNC, "WGL_EXT_swap_control");
    pwglSwapIntervalEXT    = (GLboolean (__stdcall*)(GLint))pwglGetProcAddress( "wglSwapIntervalEXT");
    pwglGetSwapIntervalEXT = (GLint     (__stdcall*)(void) )pwglGetProcAddress( "wglGetSwapIntervalEXT");
    ASSERT( pwglSwapIntervalEXT!=NULL && pwglGetSwapIntervalEXT!=NULL);
  }

  // determine support for ATI Truform technology
  extern INDEX truform_iLevel;
  extern BOOL  truform_bLinear;
  truform_iLevel  = -1;
  truform_bLinear = FALSE;
  pglPNTrianglesiATI = NULL;
  pglPNTrianglesfATI = NULL;
  gl_iTessellationLevel    = 0;
  gl_iMaxTessellationLevel = 0;
  if( HasExtension( go_strExtensions, "GL_ATI_pn_triangles")) {
    AddExtension_OGL( NONE, "GL_ATI_pn_triangles");
    pglPNTrianglesiATI = (void (__stdcall*)(GLenum,GLint  ))pwglGetProcAddress( "glPNTrianglesiATI");
    pglPNTrianglesfATI = (void (__stdcall*)(GLenum,GLfloat))pwglGetProcAddress( "glPNTrianglesfATI");
    ASSERT( pglPNTrianglesiATI!=NULL && pglPNTrianglesfATI!=NULL);
    // check max possible tessellation
    pglGetIntegerv( GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI, &gliRet);
    gl_iMaxTessellationLevel = gliRet;
    OGL_CHECKERROR;
  } 

  // if T-buffer is supported
  if( _TBCapability) {
    // add extension and disable t-buffer usage by default
    AddExtension_OGL( GLF_EXT_TBUFFER, "GL_3DFX_multisample");
    pglDisable( GL_MULTISAMPLE_3DFX);
    OGL_CHECKERROR;
  }

  // test for clamp to edge
  TestExtension_OGL( GLF_EXT_EDGECLAMP, "GL_EXT_texture_edge_clamp");

  // test for clip volume hint
  TestExtension_OGL( GLF_EXT_CLIPHINT, "GL_EXT_clip_volume_hint");
  /*
  // test for occlusion culling
  TestExtension_OGL( GLF_EXT_OCCLUSIONTEST, "GL_HP_occlusion_test");

  pglGenOcclusionQueriesNV = NULL;  pglDeleteOcclusionQueriesNV = NULL;
  pglBeginOcclusionQueryNV = NULL;  pglEndOcclusionQueryNV      = NULL;
  pglGetOcclusionQueryivNV = NULL;  pglGetOcclusionQueryuivNV   = NULL;
  pglIsOcclusionQueryNV    = NULL;

  if( HasExtension( go_strExtensions, "GL_NV_occlusion_query"))
  { // prepare extension's functions
    AddExtension_OGL( GLF_EXT_OCCLUSIONQUERY, "GL_NV_occlusion_query");
    pglGenOcclusionQueriesNV    = (void (__stdcall*)(GLsizei, GLuint*))pwglGetProcAddress( "glGenOcclusionQueriesNV");
    pglDeleteOcclusionQueriesNV = (void (__stdcall*)(GLsizei, const GLuint*))pwglGetProcAddress( "glDeleteOcclusionQueriesNV");
    pglBeginOcclusionQueryNV    = (void (__stdcall*)(GLuint))pwglGetProcAddress( "glBeginOcclusionQueryNV");
    pglEndOcclusionQueryNV      = (void (__stdcall*)(void))pwglGetProcAddress( "glEndOcclusionQueryNV");
    pglGetOcclusionQueryivNV    = (void (__stdcall*)(GLuint, GLenum, GLint*))pwglGetProcAddress( "glGetOcclusionQueryivNV");
    pglGetOcclusionQueryuivNV   = (void (__stdcall*)(GLuint, GLenum, GLuint*))pwglGetProcAddress( "glGetOcclusionQueryuivNV");
    pglIsOcclusionQueryNV  = (GLboolean (__stdcall*)(GLuint))pwglGetProcAddress( "glIsOcclusionQueryNV");
    ASSERT( pglGenOcclusionQueriesNV!=NULL && pglDeleteOcclusionQueriesNV!=NULL
         && pglBeginOcclusionQueryNV!=NULL && pglEndOcclusionQueryNV!=NULL
         && pglGetOcclusionQueryivNV!=NULL && pglGetOcclusionQueryuivNV!=NULL
         && pglIsOcclusionQueryNV!=NULL);
  }
  */
  // done with seeking for supported extensions
  if( go_strSupportedExtensions=="") go_strSupportedExtensions = "none";

  // allocate vertex buffers
  // eglAdjustVertexBuffers(ogl_iVertexBufferSize*1024);
  // OGL_CHECKERROR;

  // check if 32-bit textures are supported
  GLuint uiTmpTex;
  const ULONG ulTmpTex = 0xFFFFFFFF;
  pglGenTextures( 1, &uiTmpTex);
  pglBindTexture( GL_TEXTURE_2D, uiTmpTex);
  pglTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 1,1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &ulTmpTex);
  OGL_CHECKERROR;
  gl_ulFlags &= ~GLF_32BITTEXTURES;
  pglGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &gliRet);
  if( gliRet==8) gl_ulFlags |= GLF_32BITTEXTURES;
  pglDeleteTextures( 1, &uiTmpTex);
  OGL_CHECKERROR;
  
  // setup fog and haze textures
  extern PIX _fog_pixSizeH;
  extern PIX _fog_pixSizeL;
  extern PIX _haze_pixSize;
  pglGenTextures( 1, (GLuint*)&_fog_ulTexture);
  pglGenTextures( 1, (GLuint*)&_haze_ulTexture);
  _fog_pixSizeH = 0;
  _fog_pixSizeL = 0;
  _haze_pixSize = 0;
  OGL_CHECKERROR;

  // prepare pattern texture
  extern CTexParams _tpPattern;
  extern ULONG _ulPatternTexture;
  extern ULONG _ulLastUploadedPattern;
  pglGenTextures( 1, (GLuint*)&_ulPatternTexture);
  _ulLastUploadedPattern = 0;
  _tpPattern.Clear();

  // reset texture filtering and array locking
  _tpGlobal[0].Clear();
  _tpGlobal[1].Clear();
  _tpGlobal[2].Clear();
  _tpGlobal[3].Clear();
  GFX_ctVertices = 0;
  gl_dwVertexShader = NONE;

  // set default texture filtering/biasing
  extern INDEX gap_iTextureFiltering;
  extern INDEX gap_iTextureAnisotropy;
  extern FLOAT gap_fTextureLODBias;
  gfxSetTextureFiltering( gap_iTextureFiltering, gap_iTextureAnisotropy);
  gfxSetTextureBiasing( gap_fTextureLODBias);

  // mark pretouching and probing
  extern BOOL _bNeedPretouch;
  _bNeedPretouch = TRUE;
  gl_bAllowProbing = FALSE;

  // update console system vars
  extern void UpdateGfxSysCVars(void);
  UpdateGfxSysCVars();

  // reload all loaded textures and eventually shadows
  extern INDEX shd_bCacheAll;
  extern void ReloadTextures(void);
  extern void CacheShadows(void);
  ReloadTextures();
  if( shd_bCacheAll) CacheShadows();
}


// initialize OpenGL driver
BOOL CGfxLibrary::InitDriver_OGL( BOOL b3Dfx/*=FALSE*/)
{
  ASSERT( gl_hiDriver==NONE);
  UINT iOldErrorMode = SetErrorMode( SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
  CTString strDriverFileName = b3Dfx ? "3DFXVGL.DLL" : "OPENGL32.DLL";

  try
  { // if driver doesn't exists on disk
    char strBuffer[_MAX_PATH+1];
    char *strDummy;
    int iRes = SearchPathA( NULL, strDriverFileName, NULL, _MAX_PATH, strBuffer, &strDummy);
    if( iRes==0) ThrowF_t(TRANS("OpenGL driver '%s' not present"), strDriverFileName);

    // load opengl library
    gl_hiDriver = ::LoadLibraryA( strDriverFileName);
    // if it cannot be loaded (although it is present on disk)
    if( gl_hiDriver==NONE) {
      // if it is 3dfx stand-alone driver
      if( b3Dfx) {
        // do a fatal error and inform user to deinstall it,
        // since this loading attempt probably messed up the entire system
        FatalError(TRANS( "3Dfx OpenGL driver '%s' is installed, but cannot be loaded!\n"
                          "If you previously had a 3Dfx card and it was removed,\n"
                          "please deinstall the driver and restart windows before\n"
                          "continuing.\n"), strDriverFileName);
      } // fail!
      ThrowF_t(TRANS("Cannot load OpenGL driver '%s'"), strDriverFileName);
    }
    // prepare functions
    OGL_SetFunctionPointers_t(gl_hiDriver);
  }
  catch( char *strError)
  { // didn't make it :(
    if( gl_hiDriver!=NONE) FreeLibrary(gl_hiDriver);
    gl_hiDriver = NONE;
    CPrintF( TRANS("Error starting OpenGL: %s\n"), strError);
    SetErrorMode(iOldErrorMode);
    return FALSE;
  }

  // revert to old error mode
  SetErrorMode(iOldErrorMode);

  // if default driver
  if( !b3Dfx) {
    // use GDI functions
    pwglSwapBuffers       = ::SwapBuffers;
    pwglSetPixelFormat    = ::SetPixelFormat;
    pwglChoosePixelFormat = ::ChoosePixelFormat;
    // NOTE:
    // some ICD implementations are not infact in OPENGL32.DLL, but in some
    // other installed DLL, which is loaded when original OPENGL32.DLL from MS is
    // loaded. For those, we in fact load OPENGL32.DLL from MS, so we must _not_
    // call these functions directly, because they are in MS dll. We must call
    // functions from GDI, which in turn call either OPENGL32.DLL, _or_ the client driver,
    // as appropriate.
  }
  // done
  return TRUE;
} 


// shutdown OpenGL driver
void CGfxLibrary::EndDriver_OGL(void)
{
  // unbind all textures
  if( _pTextureStock!=NULL) {
    {FOREACHINDYNAMICCONTAINER( _pTextureStock->st_ctObjects, CTextureData, ittd) {
      CTextureData &td = *ittd;
      td.td_tpLocal.Clear();
      td.Unbind();
    }}
  }
  // unbind fog, haze and flat texture
  gfxDeleteTexture( _fog_ulTexture); 
  gfxDeleteTexture( _haze_ulTexture);
  ASSERT( _ptdFlat!=NULL);
  _ptdFlat->td_tpLocal.Clear();
  _ptdFlat->Unbind();

  // shut the driver down
  if( go_hglRC!=NULL) {
    if( pwglMakeCurrent!=NULL) {
      BOOL bRes = pwglMakeCurrent(NULL, NULL);
      WIN_CHECKERROR( bRes, "MakeCurrent(NULL, NULL)");
    }
    ASSERT( pwglDeleteContext!=NULL);
    BOOL bRes = pwglDeleteContext(go_hglRC);
    WIN_CHECKERROR( bRes, "DeleteContext");
    go_hglRC = NULL;
  }
  OGL_ClearFunctionPointers();
}



// prepare current viewport for rendering thru OpenGL
BOOL CGfxLibrary::SetCurrentViewport_OGL(CViewPort *pvp)
{
  // if must init entire opengl
  if( gl_ulFlags & GLF_INITONNEXTWINDOW)
  {
    gl_ulFlags &= ~GLF_INITONNEXTWINDOW;
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();
    // init now
    CTempDC tdc(pvp->vp_hWnd);
    if( !CreateContext_OGL(tdc.hdc)) return FALSE;
    gl_pvpActive = pvp; // remember as current viewport (must do that BEFORE InitContext)
    InitContext_OGL();
    pvp->vp_ctDisplayChanges = gl_ctDriverChanges;
    return TRUE;
  }

  // if window was not set for this driver
  if( pvp->vp_ctDisplayChanges<gl_ctDriverChanges)
  {
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();
    // set it
    CTempDC tdc(pvp->vp_hWnd);
    if( !SetupPixelFormat_OGL(tdc.hdc)) return FALSE;
    pvp->vp_ctDisplayChanges = gl_ctDriverChanges;
  }

  if( gl_pvpActive!=NULL) {
    // fail, if only one window is allowed (3dfx driver), already initialized and trying to set non-primary viewport
    const BOOL bOneWindow = (gl_gaAPI[GAT_OGL].ga_adaAdapter[gl_iCurrentAdapter].da_ulFlags & DAF_ONEWINDOW);
    if( bOneWindow && gl_pvpActive->vp_hWnd!=NULL && gl_pvpActive->vp_hWnd!=pvp->vp_hWnd) return FALSE;
    // no need to set context if it is the same window as last time
    if( gl_pvpActive->vp_hWnd==pvp->vp_hWnd) return TRUE;
  }

  // try to set context to this window
  pwglMakeCurrent( NULL, NULL);
  CTempDC tdc(pvp->vp_hWnd);
  // fail, if cannot set context to this window
  if( !pwglMakeCurrent( tdc.hdc, go_hglRC)) return FALSE;

  // remember as current window
  gl_pvpActive = pvp;
  return TRUE;
}



/*
 * 3dfx t-buffer control
 */


extern void SetTBufferEffect( BOOL bEnable)
{
  // adjust console vars
  ogl_iTBufferEffect  = Clamp( ogl_iTBufferEffect, 0L, 2L);
  ogl_iTBufferSamples = (1L) << FastLog2(ogl_iTBufferSamples);
  if( ogl_iTBufferSamples<2) ogl_iTBufferSamples = 4;
  // if supported
  if( _pGfx->gl_ulFlags&GLF_EXT_TBUFFER)
  { // disable multisampling if not required
    ASSERT( pglTBufferMask3DFX!=NULL);
    if( ogl_iTBufferEffect==0 || _pGfx->go_ctSampleBuffers<2 || !bEnable) pglDisable( GL_MULTISAMPLE_3DFX);
    else {
      pglEnable( GL_MULTISAMPLE_3DFX);
      UINT uiMask = 0xFFFFFFFF;
      // set one buffer in case of motion-blur
      if( ogl_iTBufferEffect==2) uiMask = (1UL) << _pGfx->go_iCurrentWriteBuffer;
      //pglTBufferMask3DFX(uiMask);
    }
  }
}
