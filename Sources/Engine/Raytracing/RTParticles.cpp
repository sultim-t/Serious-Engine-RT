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

#include <Engine/Entities/Entity.h>
#include <Engine/Graphics/DrawPort.h>
#include <Engine/Math/Projection.h>
#include <Engine/Raytracing/SSRT.h>
#include <Engine/Raytracing/RTProcessing.h>
#include <Engine/Raytracing/SSRTGlobals.h>
#include <Engine/World/World.h>

#include <Engine/Templates/DynamicContainer.cpp>


extern INDEX gfx_bRenderParticles;

extern SSRT::SSRTGlobals _srtGlobals;

extern CEntity *_Particle_penCurrentViewer;
extern FLOAT _Particle_fCurrentMip;
extern BOOL  _Particle_bHasFog;
extern BOOL  _Particle_bHasHaze;

extern const FLOAT *pfSinTable;
extern const FLOAT *pfCosTable;


// variables used for rendering particles
static CStaticStackArray<RgRasterizedGeometryVertexStruct> RT_Vertices;
static FLOAT          RT_fTextureCorrectionU, RT_fTextureCorrectionV;
static GFXTexCoord    RT_atex[4];
static CTextureData   *RT_ptd = NULL;
static INDEX          RT_iFrame = 0;

static bool           RT_bAlphaTest;
static bool           RT_bBlendEnable;
static RgBlendFactor  RT_eBlendSrc;
static RgBlendFactor  RT_eBlendDst;

static CAnyProjection3D RT_pEmptyProjection = {};

static SSRT::Scene *RT_sCurrentScene;


void RT_AddParticlesForEntity(CEntity *pTargetEntity, SSRT::Scene *pScene)
{
  CEntity *pViewerEntity = pScene->GetViewerEntity();

  if (!gfx_bRenderParticles || pViewerEntity == nullptr)
  {
    return;
  }

  ASSERT(!RT_pEmptyProjection.IsSimple() && !RT_pEmptyProjection.IsIsometric() && 
         !RT_pEmptyProjection.IsParallel() && !RT_pEmptyProjection.IsPerspective());


  Particle_PrepareSystem(nullptr, RT_pEmptyProjection);

  RT_sCurrentScene = pScene;

  Particle_PrepareEntity(4.0f, 0, 0, pViewerEntity);
  ASSERT(_Particle_penCurrentViewer == pViewerEntity);

  // RT: this will call function in DrawPort_Particles.cpp
  pTargetEntity->RenderParticles();

  RT_sCurrentScene = nullptr;
  _Particle_penCurrentViewer = NULL;

  Particle_EndSystem(FALSE);
}


static void RT_GenerateQuadIndices(SSRT::CParticlesGeometry *preparedInfo)
{
  const INDEX ctElements = preparedInfo->vertexCount * 6 / 4;
  if (ctElements <= 0)
  {
    return;
  }

  // make sure that enough quad elements has been initialized,
  // indices are the same so use one array without changing it
  const INDEX ctQuads = _aiCommonQuads.Count();
  if (ctElements > ctQuads)
  {
    const INDEX toAdd = ctElements - ctQuads;

    const INDEX iStart = _aiCommonQuads.Count() / 6 * 4;
    INDEX *piQuads = _aiCommonQuads.Push(toAdd * 6);
    for (INDEX i = 0; i < toAdd; i++)
    {
      piQuads[i * 6 + 0] = iStart + i * 4 + 0;
      piQuads[i * 6 + 1] = iStart + i * 4 + 1;
      piQuads[i * 6 + 2] = iStart + i * 4 + 2;
      piQuads[i * 6 + 3] = iStart + i * 4 + 2;
      piQuads[i * 6 + 4] = iStart + i * 4 + 3;
      piQuads[i * 6 + 5] = iStart + i * 4 + 0;
    }
  }

  preparedInfo->pIndexData = &_aiCommonQuads[0];
  preparedInfo->indexCount = ctElements;
}


void RT_Particle_PrepareTexture(CTextureObject *pto, ParticleBlendType pbt)
{
  RT_bAlphaTest = false;
  RT_bBlendEnable = false;
  RT_eBlendSrc = RG_BLEND_FACTOR_ZERO;
  RT_eBlendDst = RG_BLEND_FACTOR_ONE;

  // determine blend type
  switch (pbt)
  {
    case PBT_BLEND:
      //  gfxBlendFunc(GFX_SRC_ALPHA, GFX_INV_SRC_ALPHA);
      RT_bBlendEnable = true;
      RT_eBlendSrc = RG_BLEND_FACTOR_SRC_ALPHA;
      RT_eBlendDst = RG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      break;
    case PBT_ADD:
      // gfxBlendFunc(GFX_ONE, GFX_ONE);
      RT_bBlendEnable = true;
      RT_eBlendSrc = RG_BLEND_FACTOR_ONE;
      RT_eBlendDst = RG_BLEND_FACTOR_ONE;
      break;
    case PBT_MULTIPLY:
      // gfxBlendFunc(GFX_ZERO, GFX_INV_SRC_COLOR);
      RT_bBlendEnable = true;
      RT_eBlendSrc = RG_BLEND_FACTOR_ZERO;
      RT_eBlendDst = RG_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
      break;
    case PBT_ADDALPHA:
      // gfxBlendFunc(GFX_SRC_ALPHA, GFX_ONE);
      RT_bBlendEnable = true;
      RT_eBlendSrc = RG_BLEND_FACTOR_SRC_ALPHA;
      RT_eBlendDst = RG_BLEND_FACTOR_ONE;
      break;
    case PBT_FLEX:
      // gfxBlendFunc(GFX_ONE, GFX_INV_SRC_ALPHA);
      RT_bBlendEnable = true;
      RT_eBlendSrc = RG_BLEND_FACTOR_ONE;
      RT_eBlendDst = RG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      break;
    case PBT_TRANSPARENT:
      // = RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
      // TODO: upload as ray-traced?
      RT_bAlphaTest = true;
      break;
  }

  // get texture parameters for current frame and needed mip factor
  RT_ptd = (CTextureData *)pto->GetData();
  RT_iFrame = pto->GetFrame();

  // obtain curently used texture's width and height in mexes
  MEX mexTextureWidth = RT_ptd->GetWidth();
  MEX mexTextureHeight = RT_ptd->GetHeight();

  // calculate correction factor (relative to greater texture dimension)
  RT_fTextureCorrectionU = 1.0f / mexTextureWidth;
  RT_fTextureCorrectionV = 1.0f / mexTextureHeight;
}


void RT_Particle_SetTexturePart(MEX mexWidth, MEX mexHeight, INDEX iCol, INDEX iRow)
{
  // prepare full texture for displaying
  MEXaabbox2D boxTextureClipped(MEX2D(mexWidth * (iCol + 0), mexHeight * (iRow + 0)),
                                MEX2D(mexWidth * (iCol + 1), mexHeight * (iRow + 1)));

  // prepare coordinates of the rectangle
  RT_atex[0].s = boxTextureClipped.Min()(1) * RT_fTextureCorrectionU;
  RT_atex[0].t = boxTextureClipped.Min()(2) * RT_fTextureCorrectionV;
  RT_atex[1].s = boxTextureClipped.Min()(1) * RT_fTextureCorrectionU;
  RT_atex[1].t = boxTextureClipped.Max()(2) * RT_fTextureCorrectionV;
  RT_atex[2].s = boxTextureClipped.Max()(1) * RT_fTextureCorrectionU;
  RT_atex[2].t = boxTextureClipped.Max()(2) * RT_fTextureCorrectionV;
  RT_atex[3].s = boxTextureClipped.Max()(1) * RT_fTextureCorrectionU;
  RT_atex[3].t = boxTextureClipped.Min()(2) * RT_fTextureCorrectionV;
}


static FLOAT3D RT_GetViewDirection()
{
  if (RT_sCurrentScene != nullptr)
  {
    const FLOATmatrix3D &m = RT_sCurrentScene->GetCameraRotation();

    return FLOAT3D(m(1, 3), m(2, 3), m(3, 3));
  }
  else
  {
    return FLOAT3D(0, 0, 1);
  }
}


static void RT_GetViewVectors(FLOAT3D &x, FLOAT3D &y, ANGLE additionalBanking = 0)
{
  if (_Particle_penCurrentViewer != nullptr)
  {
    const FLOATmatrix3D &m = RT_sCurrentScene->GetCameraRotation();

    if (additionalBanking == 0)
    {
      x = FLOAT3D(m(1, 1), m(2, 1), m(3, 1));
      y = FLOAT3D(m(1, 2), m(2, 2), m(3, 2));
    }
    else
    {
      FLOATmatrix3D a;
      MakeRotationMatrix(a, { 0, 0, additionalBanking });

      a = m * a;
      x = FLOAT3D(a(1, 1), a(2, 1), a(3, 1));
      y = FLOAT3D(a(1, 2), a(2, 2), a(3, 2));
    }
  }
  else
  {
    x = FLOAT3D(0, 0, 1);
    y = FLOAT3D(0, 1, 0);
  }
}


void RT_Particle_RenderSquare(const FLOAT3D &vPos, FLOAT fSize, ANGLE aRotation, COLOR col, FLOAT fYRatio)
{  
  // trivial rejection
  if (fSize < 0.0001f || ((col & CT_AMASK) >> CT_ASHIFT) < 2)
  {
    return;
  }

  const FLOAT fRX = fSize;
  const FLOAT fRY = fSize * fYRatio;

  FLOAT3D dx, dy;

  // rotate dx and dy
  RT_GetViewVectors(dx, dy, aRotation);

  dx *= fRX;
  dy *= fRY;

  FLOAT3D v0 = vPos - dx + dy;
  FLOAT3D v1 = vPos - dx - dy;
  FLOAT3D v2 = vPos + dx - dy;
  FLOAT3D v3 = vPos + dx + dy;

  // add to vertex arrays
  RgRasterizedGeometryVertexStruct *pVerts = RT_Vertices.Push(4);

  memcpy(pVerts[0].position, v0.vector, sizeof(float) * 3);
  memcpy(pVerts[1].position, v1.vector, sizeof(float) * 3);
  memcpy(pVerts[2].position, v2.vector, sizeof(float) * 3);
  memcpy(pVerts[3].position, v3.vector, sizeof(float) * 3);

  memcpy(pVerts[0].texCoord, &RT_atex[0], sizeof(float) * 2);
  memcpy(pVerts[1].texCoord, &RT_atex[1], sizeof(float) * 2);
  memcpy(pVerts[2].texCoord, &RT_atex[2], sizeof(float) * 2);
  memcpy(pVerts[3].texCoord, &RT_atex[3], sizeof(float) * 2);

  const GFXColor glcol(col);
  pVerts[0].packedColor = glcol.abgr;
  pVerts[1].packedColor = glcol.abgr;
  pVerts[2].packedColor = glcol.abgr;
  pVerts[3].packedColor = glcol.abgr;
}


void RT_Particle_RenderLine(const FLOAT3D &a, const FLOAT3D &b, FLOAT width, COLOR col)
{  
  // trivial rejection
  if (width < 0 || ((col & CT_AMASK) >> CT_ASHIFT) < 2)
  {
    return;
  }

  COLOR col0, col1;
  col0 = col1 = col;

  FLOAT3D lineDir = (b - a).Normalize();
  FLOAT3D viewDir = RT_GetViewDirection();

  // perpendicular
  FLOAT3D cross = lineDir * viewDir;
  cross *= width;

  // shift by width to opposite direction to make a quad
  FLOAT3D a0 = a + cross;
  FLOAT3D a1 = a - cross;

  FLOAT3D b0 = b + cross;
  FLOAT3D b1 = b - cross;

  // add to vertex arrays
  RgRasterizedGeometryVertexStruct *pVerts = RT_Vertices.Push(4);

  memcpy(pVerts[0].position, a0.vector, sizeof(float) * 3);
  memcpy(pVerts[1].position, b0.vector, sizeof(float) * 3);
  memcpy(pVerts[2].position, b1.vector, sizeof(float) * 3);
  memcpy(pVerts[3].position, a1.vector, sizeof(float) * 3);

  memcpy(pVerts[0].texCoord, &RT_atex[0], sizeof(float) * 2);
  memcpy(pVerts[1].texCoord, &RT_atex[1], sizeof(float) * 2);
  memcpy(pVerts[2].texCoord, &RT_atex[2], sizeof(float) * 2);
  memcpy(pVerts[3].texCoord, &RT_atex[3], sizeof(float) * 2);

  const GFXColor glcol0(col0);
  const GFXColor glcol1(col1);
  pVerts[0].packedColor = glcol0.abgr;
  pVerts[1].packedColor = glcol1.abgr;
  pVerts[2].packedColor = glcol1.abgr;
  pVerts[3].packedColor = glcol0.abgr;
}


void RT_Particle_RenderQuad3D(const FLOAT3D &vPos0, const FLOAT3D &vPos1, const FLOAT3D &vPos2, const FLOAT3D &vPos3, COLOR col)
{ 
  // trivial rejection
  if (((col & CT_AMASK) >> CT_ASHIFT) < 2)
  {
    return;
  }

  // separate colors (for the sake of fog/haze)
  COLOR col0, col1, col2, col3;
  col0 = col1 = col2 = col3 = col;

  // add to vertex arrays
  RgRasterizedGeometryVertexStruct *pVerts = RT_Vertices.Push(4);

  // RT: use global space, so don't project
  memcpy(pVerts[0].position, vPos0.vector, sizeof(float) * 3);
  memcpy(pVerts[1].position, vPos1.vector, sizeof(float) * 3);
  memcpy(pVerts[2].position, vPos2.vector, sizeof(float) * 3);
  memcpy(pVerts[3].position, vPos3.vector, sizeof(float) * 3);

  memcpy(pVerts[0].texCoord, &RT_atex[0], sizeof(float) * 2);
  memcpy(pVerts[1].texCoord, &RT_atex[1], sizeof(float) * 2);
  memcpy(pVerts[2].texCoord, &RT_atex[2], sizeof(float) * 2);
  memcpy(pVerts[3].texCoord, &RT_atex[3], sizeof(float) * 2);

  const GFXColor glcol0(col0);
  const GFXColor glcol1(col1);
  const GFXColor glcol2(col2);
  const GFXColor glcol3(col3);
  pVerts[0].packedColor = glcol0.abgr;
  pVerts[1].packedColor = glcol1.abgr;
  pVerts[2].packedColor = glcol2.abgr;
  pVerts[3].packedColor = glcol3.abgr;
}


void RT_Particle_Flush()
{
  if (RT_sCurrentScene != nullptr && RT_Vertices.Count() > 0)
  {
    FLOATmatrix3D identity;
    identity(1, 1) = 1; identity(1, 2) = 0; identity(1, 3) = 0;
    identity(2, 1) = 0; identity(2, 2) = 1; identity(2, 3) = 0;
    identity(3, 1) = 0; identity(3, 2) = 0; identity(3, 3) = 1;

    SSRT::CParticlesGeometry info = {};
    info.absPosition = { 0,0,0 };
    info.absRotation = identity;
    info.vertexCount = RT_Vertices.Count();
    info.pVertexData = &RT_Vertices[0];

    info.pTexture = RT_ptd;
    info.textureFrame = RT_iFrame;

    info.blendEnable = true;
    info.blendSrc = RT_eBlendSrc;
    info.blendDst = RT_eBlendDst;

    RT_GenerateQuadIndices(&info);

    RT_sCurrentScene->AddParticles(info);
  }

  RT_ptd = nullptr;
  RT_iFrame = 0;

  RT_Vertices.Clear();

  // all done
  gfxResetArrays();
}

