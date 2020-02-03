
#ifdef SE1_VULKAN

// ENABLE/DISABLE FUNCTIONS

extern FLOAT VkViewMatrix[16];
extern FLOAT VkProjectionMatrix[16];

static void svk_EnableTexture(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_abTexture[GFX_iActiveTexUnit] = TRUE;
}



static void svk_DisableTexture(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_abTexture[GFX_iActiveTexUnit] = FALSE;
}



static void svk_EnableDepthTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() |= SVK_PLS_DEPTH_TEST_BOOL;
  GFX_bDepthTest = TRUE;
}



static void svk_DisableDepthTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() &= ~SVK_PLS_DEPTH_TEST_BOOL;
  GFX_bDepthTest = FALSE;
}



static void svk_EnableDepthBias(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);  
  // must be in recording state
  ASSERT(_pGfx->gl_VkCmdIsRecording);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  _pGfx->GetPipelineState() |= SVK_PLS_DEPTH_BIAS_BOOL;
  vkCmdSetDepthBias(_pGfx->GetCurrentCmdBuffer(), -2.0f, 0.0f, -1.0f);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_DisableDepthBias(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _pGfx->GetPipelineState() &= ~SVK_PLS_DEPTH_BIAS_BOOL;
}



static void svk_EnableDepthWrite(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() |= SVK_PLS_DEPTH_WRITE_BOOL;
  GFX_bDepthWrite = TRUE;
}



static void svk_DisableDepthWrite(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() &= ~SVK_PLS_DEPTH_WRITE_BOOL;
  GFX_bDepthWrite = FALSE;
}



static void svk_EnableAlphaTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() |= SVK_PLS_ALPHA_ENABLE_BOOL;
  GFX_bAlphaTest = TRUE;
}



static void svk_DisableAlphaTest(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() &= ~SVK_PLS_ALPHA_ENABLE_BOOL;
  GFX_bAlphaTest = FALSE;
}



static void svk_EnableBlend(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() |= SVK_PLS_BLEND_ENABLE_BOOL;
  GFX_bBlending = TRUE;
}



static void svk_DisableBlend(void)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() &= ~SVK_PLS_BLEND_ENABLE_BOOL;
  GFX_bBlending = FALSE;
}



// helper for blending operation function
__forceinline SvkPipelineStateFlags BlendToSvkSrcFlag(GfxBlend eFunc) {
  switch (eFunc) {
  case GFX_ZERO:          return SVK_PLS_SRC_BLEND_FACTOR_ZERO;
  case GFX_ONE:           return SVK_PLS_SRC_BLEND_FACTOR_ONE;
  case GFX_SRC_COLOR:     return SVK_PLS_SRC_BLEND_FACTOR_SRC_COLOR;
  case GFX_INV_SRC_COLOR: return SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
  case GFX_DST_COLOR:     return SVK_PLS_SRC_BLEND_FACTOR_DST_COLOR;
  case GFX_INV_DST_COLOR: return SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
  case GFX_SRC_ALPHA:     return SVK_PLS_SRC_BLEND_FACTOR_SRC_ALPHA;
  case GFX_INV_SRC_ALPHA: return SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  default: ASSERTALWAYS("Invalid GFX blending function!");
  } return 0;
}

__forceinline SvkPipelineStateFlags BlendToSvkDstFlag(GfxBlend eFunc) {
  switch (eFunc) {
  case GFX_ZERO:          return SVK_PLS_DST_BLEND_FACTOR_ZERO;
  case GFX_ONE:           return SVK_PLS_DST_BLEND_FACTOR_ONE;
  case GFX_SRC_COLOR:     return SVK_PLS_DST_BLEND_FACTOR_SRC_COLOR;
  case GFX_INV_SRC_COLOR: return SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
  case GFX_DST_COLOR:     return SVK_PLS_DST_BLEND_FACTOR_DST_COLOR;
  case GFX_INV_DST_COLOR: return SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
  case GFX_SRC_ALPHA:     return SVK_PLS_DST_BLEND_FACTOR_SRC_ALPHA;
  case GFX_INV_SRC_ALPHA: return SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  default: ASSERTALWAYS("Invalid GFX blending function!");
  } return 0;
}


// set blending operations
static void svk_BlendFunc(GfxBlend eSrc, GfxBlend eDst)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  // disable all bits and set one
  _pGfx->GetPipelineState() &= ~SVK_PLS_SRC_BLEND_FACTOR_BITS;
  _pGfx->GetPipelineState() &= ~SVK_PLS_DST_BLEND_FACTOR_BITS;

  _pGfx->GetPipelineState() |= BlendToSvkSrcFlag(eSrc);
  _pGfx->GetPipelineState() |= BlendToSvkDstFlag(eDst);

  GFX_eBlendSrc = eSrc;
  GFX_eBlendDst = eDst;
}



// color buffer writing enable
static void svk_SetColorMask(ULONG ulColorMask)
{
  ASSERT( _pGfx->gl_eCurrentAPI==GAT_VK);
  _ulCurrentColorMask = ulColorMask; // keep for Get...()

  const BOOL bR = (ulColorMask&CT_RMASK) == CT_RMASK;
  const BOOL bG = (ulColorMask&CT_GMASK) == CT_GMASK;
  const BOOL bB = (ulColorMask&CT_BMASK) == CT_BMASK;
  const BOOL bA = (ulColorMask&CT_AMASK) == CT_AMASK;

  // disable all and set specific
  _pGfx->GetPipelineState() &= ~SVK_PLS_COLOR_WRITE_MASK_RGBA;

  if (bR) { _pGfx->GetPipelineState() |= SVK_PLS_COLOR_WRITE_MASK_R_BIT; }
  if (bG) { _pGfx->GetPipelineState() |= SVK_PLS_COLOR_WRITE_MASK_G_BIT; }
  if (bB) { _pGfx->GetPipelineState() |= SVK_PLS_COLOR_WRITE_MASK_B_BIT; }
  if (bA) { _pGfx->GetPipelineState() |= SVK_PLS_COLOR_WRITE_MASK_A_BIT; }
}



// helper for depth compare function
__forceinline SvkPipelineStateFlags CompToSvkSrcFlag(GfxComp eFunc) {
  switch (eFunc) {
  case GFX_NEVER:         return SVK_PLS_DEPTH_COMPARE_OP_NEVER;
  case GFX_LESS:          return SVK_PLS_DEPTH_COMPARE_OP_LESS;
  case GFX_LESS_EQUAL:    return SVK_PLS_DEPTH_COMPARE_OP_LESS_OR_EQUAL;
  case GFX_EQUAL:         return SVK_PLS_DEPTH_COMPARE_OP_EQUAL;
  case GFX_NOT_EQUAL:     return SVK_PLS_DEPTH_COMPARE_OP_NOT_EQUAL;
  case GFX_GREATER_EQUAL: return SVK_PLS_DEPTH_COMPARE_OP_GREATER_OR_EQUAL;
  case GFX_GREATER:       return SVK_PLS_DEPTH_COMPARE_OP_GREATER;
  case GFX_ALWAYS:        return SVK_PLS_DEPTH_COMPARE_OP_ALWAYS;
  default: ASSERTALWAYS("Invalid GFX compare function!");
  } return 0;
}



// set depth buffer compare mode
static void svk_DepthFunc(GfxComp eFunc)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  // disable all bits and set one
  _pGfx->GetPipelineState() &= ~SVK_PLS_DEPTH_COMPARE_OP_BITS;

  _pGfx->GetPipelineState() |= CompToSvkSrcFlag(eFunc);
  GFX_eDepthFunc = eFunc;
}



// set depth buffer range
static void svk_DepthRange(FLOAT fMin, FLOAT fMax)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  // must be in recording state
  ASSERT(_pGfx->gl_VkCmdIsRecording);

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // TODO: Vulkan: depth bounds
  //_pGfx->GetPipelineState() |= SVK_PLS_DEPTH_BOUNDS_BOOL;
  //vkCmdSetDepthBounds(_pGfx->GetCurrentCmdBuffer(), fMin, fMax);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);

  GFX_fMinDepthRange = fMin;
  GFX_fMaxDepthRange = fMax;
}



// set face culling
static void svk_CullFace(GfxFace eFace)
{
  // check consistency and face
  ASSERT(eFace == GFX_FRONT || eFace == GFX_BACK || eFace == GFX_NONE);
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() &= ~SVK_PLS_CULL_MODE_BITS;

  if (eFace == GFX_FRONT)
  {
    _pGfx->GetPipelineState() |= SVK_PLS_CULL_MODE_FRONT;
  }
  else if (eFace == GFX_BACK) 
  {
    _pGfx->GetPipelineState() |= SVK_PLS_CULL_MODE_BACK;
  }
  else
  {
    _pGfx->GetPipelineState() |= SVK_PLS_CULL_MODE_NONE;
  }

  GFX_eCullFace = eFace;
}



// set front face
static void svk_FrontFace(GfxFace eFace)
{
  // check consistency and face
  ASSERT(eFace == GFX_CW || eFace == GFX_CCW);
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() &= ~SVK_PLS_FRONT_FACE_BITS;

  if (eFace == GFX_CCW)
  {
    _pGfx->GetPipelineState() |= SVK_PLS_FRONT_FACE_COUNTER_CLOCKWISE;
  }
  else
  {
    _pGfx->GetPipelineState() |= SVK_PLS_FRONT_FACE_CLOCKWISE;
  }

  GFX_bFrontFace = eFace == GFX_CCW;
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

  if (pfMatrix != NULL) 
  {
    Svk_MatCopy(VkViewMatrix, pfMatrix);
  }
  else 
  {
    Svk_MatSetIdentity(VkViewMatrix);
  }

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

  Svk_MatOrtho(VkProjectionMatrix, fLeft, fRight, fBottom, fTop, fNear, fFar);

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

  Svk_MatFrustum(VkProjectionMatrix, fLeft, fRight, fBottom, fTop, fNear, fFar);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// set polygon mode (point, line or fill)
static void svk_PolygonMode(GfxPolyMode ePolyMode)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  _pGfx->GetPipelineState() &= ~SVK_PLS_POLYGON_MODE_BITS;

  switch (ePolyMode) 
  {
  case GFX_POINT:  _pGfx->GetPipelineState() |= SVK_PLS_POLYGON_MODE_POINT; break;
  case GFX_LINE:   _pGfx->GetPipelineState() |= SVK_PLS_POLYGON_MODE_LINE;   break;
  case GFX_FILL:   _pGfx->GetPipelineState() |= SVK_PLS_POLYGON_MODE_FILL;   break;
  default:  ASSERTALWAYS("Wrong polygon mode!");
  }
}




// TEXTURE MANAGEMENT


// set texture custom wrapping mode, that can differ from original texture's wrapping
static void svk_SetTextureWrapping(enum GfxWrap eWrapU, enum GfxWrap eWrapV)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  // just set, it will be processed when texture will be set
  _tpGlobal[GFX_iActiveTexUnit].tp_eWrapU = eWrapU;
  _tpGlobal[GFX_iActiveTexUnit].tp_eWrapV = eWrapV;
}



// generate texture for API
static void svk_GenerateTexture(ULONG& ulTexObject)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _sfStats.StartTimer(CStatForm::STI_BINDTEXTURE);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // TODO: vulkan texture handlers
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

  // TODO: vulkan texture handlers
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

  CStaticStackArray<SvkVertex> &verts = _pGfx->gl_VkVerts;

  verts.PopAll();
  SvkVertex *pushed = verts.Push(ctVtx);

  for (INDEX i = 0; i < ctVtx; i++)
  {
    pushed[i].SetPosition(pvtx[i].x, pvtx[i].y, pvtx[i].z);
  }

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// prepare normal array for API
static void svk_SetNormalArray(GFXNormal* pnor)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  ASSERT(pnor != NULL);
  ASSERT(GFX_ctVertices > 0);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  CStaticStackArray<SvkVertex> &verts = _pGfx->gl_VkVerts;
  INDEX ctVtx = verts.Count();
  ASSERT(ctVtx > 0);
  ASSERT(ctVtx == GFX_ctVertices);

  for (INDEX i = 0; i < ctVtx; i++)
  {
    verts[i].SetNormal(pnor[i].nx, pnor[i].ny, pnor[i].nz);
  }

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// prepare color array for API (and force rendering with color array!)
static void svk_SetColorArray(GFXColor* pcol)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  ASSERT(pcol != NULL);

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  CStaticStackArray<SvkVertex> &verts = _pGfx->gl_VkVerts;
  INDEX ctVtx = verts.Count();
  ASSERT(ctVtx > 0);
  ASSERT(ctVtx == GFX_ctVertices);

  for (INDEX i = 0; i < ctVtx; i++)
  {
    verts[i].SetColorRGBA(pcol[i].r / 255.0f, pcol[i].g / 255.0f, pcol[i].b / 255.0f, pcol[i].a / 255.0f);
  }

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// prepare texture coordinates array for API
static void svk_SetTexCoordArray(GFXTexCoord* ptex, BOOL b4/*=FALSE*/)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  ASSERT(ptex != NULL);
  
  if (!b4)
  {
    // ignore projective tex coords for now
    return;
  }

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  CStaticStackArray<SvkVertex> &verts = _pGfx->gl_VkVerts;
  INDEX ctVtx = verts.Count();
  ASSERT(ctVtx > 0);

  for (INDEX i = 0; i < ctVtx; i++)
  {
    verts[i].SetTexCoord(ptex[i].u, ptex[i].v);
  }

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



// draw prepared arrays
static void svk_DrawElements(INDEX ctElem, INDEX *pidx)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
#ifndef NDEBUG
  // check if all indices are inside lock count (or smaller than 65536)
  if (pidx != NULL) for (INDEX i = 0; i < ctElem; i++) ASSERT(pidx[i] < GFX_ctVertices);
#endif

  _sfStats.StartTimer(CStatForm::STI_GFXAPI);
  _pGfx->gl_ctTotalTriangles += ctElem / 3;  // for profiling

  ASSERT(pidx != NULL); // draw quads only for OpenGL
  ASSERT(_pGfx->gl_VkVerts.Count() > 0);
  ASSERT(_pGfx->gl_VkVerts.Count() == GFX_ctVertices);

  _pGfx->DrawTriangles(ctElem, (uint32_t *)pidx);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



static void svk_Finish(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  _sfStats.StartTimer(CStatForm::STI_GFXAPI);

  // force finish of API rendering queue
  VkResult r = vkQueueWaitIdle(_pGfx->gl_VkQueueGraphics);
  VK_CHECKERROR(r);

  _sfStats.StopTimer(CStatForm::STI_GFXAPI);
}



#pragma region empty

static void svk_EnableColorArray(void)
{
  // always enabled
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bColorArray = TRUE;
}

static void svk_DisableColorArray(void)
{
  // always enabled
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bColorArray = TRUE;
}

static void svk_LockArrays(void)
{
  // only for OpenGL
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
}

static void svk_EnableDither(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bDithering = FALSE;
}

static void svk_DisableDither(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bDithering = FALSE;
}

static void svk_EnableClipping(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bClipping = FALSE;
}

static void svk_DisableClipping(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bClipping = FALSE;
}

// this is used for mirrors
static void svk_EnableClipPlane(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bClipPlane = FALSE;
}

static void svk_DisableClipPlane(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bClipPlane = FALSE;
}

// this is used for mirrors
static void svk_ClipPlane(const DOUBLE *pdViewPlane)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
}

// used only on some old gpus
static void svk_EnableTruform(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bTruform = FALSE;
}

// used only on some old gpus
static void svk_DisableTruform(void)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
  GFX_bTruform = FALSE;
}

static void svk_SetTextureMatrix(const FLOAT* pfMatrix/*=NULL*/)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
}

static void svk_SetConstantColor(COLOR col)
{
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);
}

static void svk_SetTextureModulation(INDEX iScale)
{
  // check consistency
  ASSERT(_pGfx->gl_eCurrentAPI == GAT_VK);

  ASSERT(iScale == 1 || iScale == 2);
  GFX_iTexModulation[GFX_iActiveTexUnit] = iScale;
}
#pragma endregion

#endif // SE1_VULKAN