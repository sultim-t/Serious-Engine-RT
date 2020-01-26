
#ifdef SE1_VULKAN

// ENABLE/DISABLE FUNCTIONS


static void svk_EnableTexture(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_TEXTURE_2D);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_abTexture[GFX_iActiveTexUnit]);
#endif

  // cached?
  if (GFX_abTexture[GFX_iActiveTexUnit] && gap_bOptimizeStateChanges) return;
  GFX_abTexture[GFX_iActiveTexUnit] = TRUE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglEnable(GL_TEXTURE_2D);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_DisableTexture(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_TEXTURE_2D);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_abTexture[GFX_iActiveTexUnit]);
#endif

  // cached?
  if (!GFX_abTexture[GFX_iActiveTexUnit] && gap_bOptimizeStateChanges) return;
  GFX_abTexture[GFX_iActiveTexUnit] = FALSE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDisable(GL_TEXTURE_2D);
  //pglDisableClientState(GL_TEXTURE_COORD_ARRAY);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_EnableDepthTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_DEPTH_TEST);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bDepthTest);
#endif
  // cached?
  if (GFX_bDepthTest && gap_bOptimizeStateChanges) return;
  GFX_bDepthTest = TRUE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglEnable(GL_DEPTH_TEST);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}


static void svk_DisableDepthTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_DEPTH_TEST);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bDepthTest);
#endif

  // cached?
  if (!GFX_bDepthTest && gap_bOptimizeStateChanges) return;
  GFX_bDepthTest = FALSE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDisable(GL_DEPTH_TEST);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}


static void svk_EnableDepthBias(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  vkCmdSetDepthBias(_pGfx->GetCurrentCmdBuffer(),-1.0f, 0.0f, -2.0f);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}


static void svk_DisableDepthBias(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  vkCmdSetDepthBias(_pGfx->GetCurrentCmdBuffer(), 0.0f, 0.0f, 0.0f);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_EnableDepthWrite(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //pglGetIntegerv(GL_DEPTH_WRITEMASK, (GLint*)&bRes);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bDepthWrite);
#endif

  // cached?
  if (GFX_bDepthWrite && gap_bOptimizeStateChanges) return;
  GFX_bDepthWrite = TRUE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDepthMask(GL_TRUE);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_DisableDepthWrite(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //pglGetIntegerv(GL_DEPTH_WRITEMASK, (GLint*)&bRes);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bDepthWrite);
#endif

  // cached?
  if (!GFX_bDepthWrite && gap_bOptimizeStateChanges) return;
  GFX_bDepthWrite = FALSE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDepthMask(GL_FALSE);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_EnableAlphaTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_ALPHA_TEST);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bAlphaTest);
#endif

  // cached?
  if (GFX_bAlphaTest && gap_bOptimizeStateChanges) return;
  GFX_bAlphaTest = TRUE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglEnable(GL_ALPHA_TEST);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_DisableAlphaTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_ALPHA_TEST);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bAlphaTest);
#endif

  // cached?
  if (!GFX_bAlphaTest && gap_bOptimizeStateChanges) return;
  GFX_bAlphaTest = FALSE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDisable(GL_ALPHA_TEST);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_EnableBlend(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_BLEND);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bBlending);
#endif
  // cached?
  if (GFX_bBlending && gap_bOptimizeStateChanges) return;
  GFX_bBlending = TRUE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglEnable(GL_BLEND);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);

  // adjust dithering
  /*if (gap_iDithering == 2) svk_EnableDither();
  else svk_DisableDither();*/
}



static void svk_DisableBlend(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_BLEND);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bBlending);
#endif

  // cached?
  if (!GFX_bBlending && gap_bOptimizeStateChanges) return;
  GFX_bBlending = FALSE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDisable(GL_BLEND);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);

  // adjust dithering
  /*if (gap_iDithering == 0) svk_DisableDither();
  else svk_EnableDither();*/
}



// enable usage of color array for subsequent rendering
static void svk_EnableColorArray(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_COLOR_ARRAY);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bColorArray);
#endif

  // cached?
  if (GFX_bColorArray && gap_bOptimizeStateChanges) return;
  GFX_bColorArray = TRUE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglEnableClientState(GL_COLOR_ARRAY);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// enable usage of constant color for subsequent rendering
static void svk_DisableColorArray(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //BOOL bRes;
  //bRes = pglIsEnabled(GL_COLOR_ARRAY);
  //VK_CHECKERROR1;
  //ASSERT(!bRes == !GFX_bColorArray);
#endif

  // cached?
  if (!GFX_bColorArray && gap_bOptimizeStateChanges) return;
  GFX_bColorArray = FALSE;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDisableClientState(GL_COLOR_ARRAY);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// helper for blending operation function
__forceinline VkBlendFactor BlendToVK(GfxBlend eFunc) {
  switch (eFunc) {
  case GFX_ZERO:          return VK_BLEND_FACTOR_ZERO;
  case GFX_ONE:           return VK_BLEND_FACTOR_ONE;
  case GFX_SRC_COLOR:     return VK_BLEND_FACTOR_SRC_COLOR;
  case GFX_INV_SRC_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
  case GFX_DST_COLOR:     return VK_BLEND_FACTOR_DST_COLOR;
  case GFX_INV_DST_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
  case GFX_SRC_ALPHA:     return VK_BLEND_FACTOR_SRC_ALPHA;
  case GFX_INV_SRC_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  default: ASSERTALWAYS("Invalid GFX blending function!");
  } return VK_BLEND_FACTOR_ONE;
}

__forceinline GfxBlend BlendFromVK(VkBlendFactor gFunc) {
  switch (gFunc) {
  case VK_BLEND_FACTOR_ZERO:                return GFX_ZERO;
  case VK_BLEND_FACTOR_ONE:                 return GFX_ONE;
  case VK_BLEND_FACTOR_SRC_COLOR:           return GFX_SRC_COLOR;
  case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return GFX_INV_SRC_COLOR;
  case VK_BLEND_FACTOR_DST_COLOR:           return GFX_DST_COLOR;
  case VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR: return GFX_INV_DST_COLOR;
  case VK_BLEND_FACTOR_SRC_ALPHA:           return GFX_SRC_ALPHA;
  case VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA: return GFX_INV_SRC_ALPHA;
  default: ASSERTALWAYS("Unsupported Vulkan blending function!");
  } return GFX_ONE;
}


// set blending operations
static void svk_BlendFunc(GfxBlend eSrc, GfxBlend eDst)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  VkBlendFactor gleSrc, gleDst;
#ifndef NDEBUG
  //GfxBlend gfxSrc, gfxDst;
  //pglGetIntegerv(GL_BLEND_SRC, (GLint*)&gleSrc);
  //pglGetIntegerv(GL_BLEND_DST, (GLint*)&gleDst);
  //VK_CHECKERROR1;
  //gfxSrc = BlendFromVK(gleSrc);
  //gfxDst = BlendFromVK(gleDst);
  //ASSERT(gfxSrc == GFX_eBlendSrc && gfxDst == GFX_eBlendDst);
#endif
  // cached?
  if (eSrc == GFX_eBlendSrc && eDst == GFX_eBlendDst && gap_bOptimizeStateChanges) return;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  gleSrc = BlendToVK(eSrc);
  gleDst = BlendToVK(eDst);

  //pglBlendFunc(gleSrc, gleDst);
  // update BOTH alphaBlendFactor and colorBlendFactor

  VK_CHECKERROR1;
  // done
  GFX_eBlendSrc = eSrc;
  GFX_eBlendDst = eDst;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// helper for depth compare function
__forceinline VkCompareOp CompToVK(GfxComp eFunc) {
  switch (eFunc) {
  case GFX_NEVER:         return VK_COMPARE_OP_NEVER;
  case GFX_LESS:          return VK_COMPARE_OP_LESS;
  case GFX_LESS_EQUAL:    return VK_COMPARE_OP_LESS_OR_EQUAL;
  case GFX_EQUAL:         return VK_COMPARE_OP_EQUAL;
  case GFX_NOT_EQUAL:     return VK_COMPARE_OP_NOT_EQUAL;
  case GFX_GREATER_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
  case GFX_GREATER:       return VK_COMPARE_OP_GREATER;
  case GFX_ALWAYS:        return VK_COMPARE_OP_ALWAYS;
  default: ASSERTALWAYS("Invalid GFX compare function!");
  } return VK_COMPARE_OP_ALWAYS;
}

__forceinline GfxComp CompFromVK(VkCompareOp gFunc) {
  switch (gFunc) {
  case VK_COMPARE_OP_NEVER:             return GFX_NEVER;
  case VK_COMPARE_OP_LESS:              return GFX_LESS;
  case VK_COMPARE_OP_LESS_OR_EQUAL:     return GFX_LESS_EQUAL;
  case VK_COMPARE_OP_EQUAL:             return GFX_EQUAL;
  case VK_COMPARE_OP_NOT_EQUAL:         return GFX_NOT_EQUAL;
  case VK_COMPARE_OP_GREATER_OR_EQUAL:  return GFX_GREATER_EQUAL;
  case VK_COMPARE_OP_GREATER:           return GFX_GREATER;
  case VK_COMPARE_OP_ALWAYS:            return GFX_ALWAYS;
  default: ASSERTALWAYS("Invalid Vulkan compare function!");
  } return GFX_ALWAYS;
}



// set depth buffer compare mode
static void svk_DepthFunc(GfxComp eFunc)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  VkCompareOp gleFunc;
#ifndef NDEBUG
  //GfxComp gfxFunc;
  //pglGetIntegerv(GL_DEPTH_FUNC, (GLint*)&gleFunc);
  //VK_CHECKERROR1;
  //gfxFunc = CompFromVK(gleFunc);
  //ASSERT(gfxFunc == GFX_eDepthFunc);
#endif
  // cached?
  if (eFunc == GFX_eDepthFunc && gap_bOptimizeStateChanges) return;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  gleFunc = CompToVK(eFunc);

  //pglDepthFunc(gleFunc);

  VK_CHECKERROR1;
  GFX_eDepthFunc = eFunc;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set depth buffer range
static void svk_DepthRange(FLOAT fMin, FLOAT fMax)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //FLOAT fDepths[2];
  //pglGetFloatv(GL_DEPTH_RANGE, (GLfloat*)&fDepths);
  //VK_CHECKERROR1;
  //ASSERT(fDepths[0] == GFX_fMinDepthRange && fDepths[1] == GFX_fMaxDepthRange);
#endif

  // cached?
  if (GFX_fMinDepthRange == fMin && GFX_fMaxDepthRange == fMax && gap_bOptimizeStateChanges) return;
  GFX_fMinDepthRange = fMin;
  GFX_fMaxDepthRange = fMax;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  vkCmdSetDepthBounds(_pGfx->GetCurrentCmdBuffer(), fMin, fMax);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set face culling
static void svk_CullFace(GfxFace eFace)
{
  // check consistency and face
  ASSERT(eFace == GFX_FRONT || eFace == GFX_BACK || eFace == GFX_NONE);
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //GLenum gleCull;
  //BOOL bRes = pglIsEnabled(GL_CULL_FACE);
  //pglGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&gleCull);
  //VK_CHECKERROR1;
  //ASSERT((bRes == GL_FALSE && GFX_eCullFace == GFX_NONE)
  //  || (bRes == GL_TRUE && gleCull == GL_FRONT && GFX_eCullFace == GFX_FRONT)
  //  || (bRes == GL_TRUE && gleCull == GL_BACK && GFX_eCullFace == GFX_BACK));
#endif
  // cached?
  if (GFX_eCullFace == eFace && gap_bOptimizeStateChanges) return;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //if (eFace == GFX_FRONT) {
  //  if (GFX_eCullFace == GFX_NONE) pglEnable(GL_CULL_FACE);
  //  pglCullFace(GL_FRONT);
  //}
  //else if (eFace == GFX_BACK) {
  //  if (GFX_eCullFace == GFX_NONE) pglEnable(GL_CULL_FACE);
  //  pglCullFace(GL_BACK);
  //}
  //else {
  //  pglDisable(GL_CULL_FACE);
  //}
  VK_CHECKERROR1;
  GFX_eCullFace = eFace;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set front face
static void svk_FrontFace(GfxFace eFace)
{
  // check consistency and face
  ASSERT(eFace == GFX_CW || eFace == GFX_CCW);
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //GLenum gleFace;
  //pglGetIntegerv(GL_FRONT_FACE, (GLint*)&gleFace);
  //VK_CHECKERROR1;
  //ASSERT((gleFace == GL_CCW && GFX_bFrontFace)
  //  || (gleFace == GL_CW && !GFX_bFrontFace));
#endif
  // cached?
  BOOL bFrontFace = (eFace == GFX_CCW);
  if (!bFrontFace == !GFX_bFrontFace && gap_bOptimizeStateChanges) return;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //if (eFace == GFX_CCW) {
  //  pglFrontFace(GL_CCW);
  //}
  //else {
  //  pglFrontFace(GL_CW);
  //}
  VK_CHECKERROR1;
  GFX_bFrontFace = bFrontFace;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set view matrix 
static void svk_SetViewMatrix(const FLOAT* pfMatrix/*=NULL*/)
{
  // check API
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  // cached? (only identity matrix)
  if (pfMatrix == NULL && GFX_bViewMatrix == NONE && gap_bOptimizeStateChanges) return;
  GFX_bViewMatrix = (pfMatrix != NULL);

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // set matrix
  //pglMatrixMode(GL_MODELVIEW);
  //if (pfMatrix != NULL) pglLoadMatrixf(pfMatrix);
  //else pglLoadIdentity();
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set orthographic matrix
static void svk_SetOrtho(const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop,
  const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar,
  const BOOL bSubPixelAdjust/*=FALSE*/)
{
  // check API and matrix type
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  // cached?
  if (GFX_fLastL == fLeft && GFX_fLastT == fTop && GFX_fLastN == fNear
    && GFX_fLastR == fRight && GFX_fLastB == fBottom && GFX_fLastF == fFar && gap_bOptimizeStateChanges) return;
  GFX_fLastL = fLeft;   GFX_fLastT = fTop;     GFX_fLastN = fNear;
  GFX_fLastR = fRight;  GFX_fLastB = fBottom;  GFX_fLastF = fFar;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // set matrix
  //pglMatrixMode(GL_PROJECTION);
  //pglLoadIdentity();
  //pglOrtho(fLeft, fRight, fBottom, fTop, fNear, fFar);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set frustrum matrix
static void svk_SetFrustum(const FLOAT fLeft, const FLOAT fRight,
  const FLOAT fTop, const FLOAT fBottom,
  const FLOAT fNear, const FLOAT fFar)
{
  // check API
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  // cached?
  if (GFX_fLastL == -fLeft && GFX_fLastT == -fTop && GFX_fLastN == -fNear
    && GFX_fLastR == -fRight && GFX_fLastB == -fBottom && GFX_fLastF == -fFar && gap_bOptimizeStateChanges) return;
  GFX_fLastL = -fLeft;   GFX_fLastT = -fTop;     GFX_fLastN = -fNear;
  GFX_fLastR = -fRight;  GFX_fLastB = -fBottom;  GFX_fLastF = -fFar;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //// set matrix
  //pglMatrixMode(GL_PROJECTION);
  //pglLoadIdentity();
  //pglFrustum(fLeft, fRight, fBottom, fTop, fNear, fFar);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set polygon mode (point, line or fill)
static void svk_PolygonMode(GfxPolyMode ePolyMode)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //switch (ePolyMode) {
  //case GFX_POINT:  pglPolygonMode(GL_FRONT_AND_BACK, GL_POINT);  break;
  //case GFX_LINE:   pglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   break;
  //case GFX_FILL:   pglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   break;
  //default:  ASSERTALWAYS("Wrong polygon mode!");
  //} // check
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}




// TEXTURE MANAGEMENT


// set texture wrapping mode
static void svk_SetTextureWrapping(enum GfxWrap eWrapU, enum GfxWrap eWrapV)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  //// check texture unit consistency
  //GLint gliRet;
  //pglGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &gliRet);
  //ASSERT(GFX_iActiveTexUnit == (gliRet - GL_TEXTURE0_ARB));
  //pglGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE_ARB, &gliRet);
  //ASSERT(GFX_iActiveTexUnit == (gliRet - GL_TEXTURE0_ARB));
#endif

  _tpGlobal[GFX_iActiveTexUnit].tp_eWrapU = eWrapU;
  _tpGlobal[GFX_iActiveTexUnit].tp_eWrapV = eWrapV;
}



// set texture modulation mode
static void svk_SetTextureModulation(INDEX iScale)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG                 
  //// check current modulation
  //GLint iRet;
  //pglGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &iRet);
  //ASSERT((GFX_iTexModulation[GFX_iActiveTexUnit] == 1 && iRet == GL_MODULATE)
  //  || (GFX_iTexModulation[GFX_iActiveTexUnit] == 2 && iRet == GL_COMBINE_EXT));
  //VK_CHECKERROR1;
#endif

  // cached?
  ASSERT(iScale == 1 || iScale == 2);
  if (GFX_iTexModulation[GFX_iActiveTexUnit] == iScale) return;
  GFX_iTexModulation[GFX_iActiveTexUnit] = iScale;

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //if (iScale == 2) {
  //  pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  //  pglTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
  //  pglTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 2.0f);
  //}
  //else {
  //  pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //} VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// generate texture for API
static void svk_GenerateTexture(ULONG& ulTexObject)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _sfStats.StartTimer(CStatForm::STI_BINDTEXTURE);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // generate one dummy texture that'll be entirely replaced upon 1st upload
  //pglGenTextures(1, (GLuint*)&ulTexObject);
  ulTexObject = 1;
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_BINDTEXTURE);
  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// unbind texture from API
static void svk_DeleteTexture(ULONG& ulTexObject)
{
  // skip if already unbound
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  if (ulTexObject == NONE) return;

  _sfStats.StartTimer(CStatForm::STI_BINDTEXTURE);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDeleteTextures(1, (GLuint*)&ulTexObject);
  ulTexObject = NONE;

  _sfStats.StopTimer(CStatForm::STI_BINDTEXTURE);
  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// VERTEX ARRAYS


// prepare vertex array for API
static void svk_SetVertexArray(GFXVertex4* pvtx, INDEX ctVtx)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  ASSERT(ctVtx > 0 && pvtx != NULL && GFX_iActiveTexUnit == 0);
  GFX_ctVertices = ctVtx;
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglDisableClientState(GL_TEXTURE_COORD_ARRAY);
  //pglDisableClientState(GL_COLOR_ARRAY);
  //pglDisableClientState(GL_NORMAL_ARRAY);
  //ASSERT(!pglIsEnabled(GL_TEXTURE_COORD_ARRAY));
  //ASSERT(!pglIsEnabled(GL_COLOR_ARRAY));
  //ASSERT(!pglIsEnabled(GL_NORMAL_ARRAY));
  //ASSERT(pglIsEnabled(GL_VERTEX_ARRAY));
  //pglVertexPointer(3, GL_FLOAT, 16, pvtx);
  VK_CHECKERROR1;
  GFX_bColorArray = FALSE; // mark that color array has been disabled (because of potential LockArrays)

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// prepare normal array for API
static void svk_SetNormalArray(GFXNormal* pnor)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  ASSERT(pnor != NULL);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  pglEnableClientState(GL_NORMAL_ARRAY);/*
  ASSERT(pglIsEnabled(GL_NORMAL_ARRAY));
  pglNormalPointer(GL_FLOAT, 16, pnor);*/
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// prepare color array for API (and force rendering with color array!)
static void svk_SetColorArray(GFXColor* pcol)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  ASSERT(pcol != NULL);
  svk_EnableColorArray();
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglColorPointer(4, GL_UNSIGNED_BYTE, 0, pcol);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// prepare texture coordinates array for API
static void svk_SetTexCoordArray(GFXTexCoord* ptex, BOOL b4/*=FALSE*/)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  ASSERT(ptex != NULL);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //pglEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //ASSERT(pglIsEnabled(GL_TEXTURE_COORD_ARRAY));
  //pglTexCoordPointer(b4 ? 4 : 2, GL_FLOAT, 0, ptex);
  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set constant color (and force rendering w/o color array!)
static void svk_SetConstantColor(COLOR col)
{
  //ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  //svk_DisableColorArray();
  //_sfStats.StartTimer(CStatForm::STI_GFXAPI);

  //glCOLOR(col);
  //VK_CHECKERROR1;

  //_sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// draw prepared arrays
static void svk_DrawElements(INDEX ctElem, INDEX* pidx)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  // check if all indices are inside lock count (or smaller than 65536)
  if (pidx != NULL) for (INDEX i = 0; i < ctElem; i++) ASSERT(pidx[i] < GFX_ctVertices);
#endif

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);
  _pGfx->gl_ctTotalTriangles += ctElem / 3;  // for profiling

  // arrays or elements
  if (pidx == NULL)
  {
    ASSERTALWAYS("Vulkan error: QUADS are not implemented.\n");
    // pglDrawArrays(GL_QUADS, 0, ctElem);
  }
  else
  {
    //pglDrawElements(GL_TRIANGLES, ctElem, GL_UNSIGNED_INT, pidx);

    //vkCmdBindIndexBuffer(_pGfx->GetCurrentCmdBuffer(), , 0, VK_INDEX_TYPE_UINT32);
    //vkCmdDrawIndexed(_pGfx->GetCurrentCmdBuffer(), ctElem, 1, 0, 0, 0);
  }

  VK_CHECKERROR1;

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// force finish of API rendering queue
static void svk_Finish(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // vkDeviceWaitIdle(_pGfx->gl_VkDevice);
  VkResult r = vkQueueWaitIdle(_pGfx->gl_VkQueueGraphics);
  VK_CHECKERROR(r);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}

static void svk_SetColorMask(ULONG ulColorMask)
{ }

static void svk_LockArrays(void)
{
  // only for OpenGL
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  return;
}

static void svk_EnableDither(void)
{}

static void svk_DisableDither(void)
{}

static void svk_EnableClipping(void)
{}

static void svk_DisableClipping(void)
{}

static void svk_EnableClipPlane(void)
{}

static void svk_DisableClipPlane(void)
{}

static void svk_EnableTruform(void)
{}

static void svk_DisableTruform(void)
{}

static void svk_ClipPlane(const DOUBLE* pdViewPlane)
{}

static void svk_SetTextureMatrix(const FLOAT* pfMatrix/*=NULL*/)
{}

#endif // SE1_VULKAN