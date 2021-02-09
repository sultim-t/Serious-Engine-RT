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

#include <Engine/Entities/Entity.h>
#include <Engine/Graphics/DrawPort.h>
#include <Engine/Math/Projection.h>
#include <Engine/Raytracing/SSRT.h>
#include <Engine/World/World.h>

#include <Engine/Templates/DynamicContainer.cpp>


extern CEntity *_Particle_penCurrentViewer;
extern FLOAT _Particle_fCurrentMip;
extern BOOL  _Particle_bHasFog;
extern BOOL  _Particle_bHasHaze;

extern const FLOAT *pfSinTable;
extern const FLOAT *pfCosTable;


// variables used for rendering particles
static FLOAT          RT_fTextureCorrectionU, RT_fTextureCorrectionV;
static GFXTexCoord    RT_atex[4];
static CTextureData   *RT_ptd = NULL;
static INDEX          RT_iFrame = 0;
static RgGeometryPassThroughType RT_ePassThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;

static SSRT::Scene *RT_sCurrentScene;


void RT_AddAllParticles(CWorld *pWorld, CEntity *penViewer, SSRT::Scene *scene)
{
  const bool bBackground = false;
  CAnyProjection3D emptyProjection = {};

  // if (bBackground) {} else {}
  Particle_PrepareSystem(nullptr, emptyProjection);

  FOREACHINDYNAMICCONTAINER(pWorld->wo_cenEntities, CEntity, iten)
  {
    /*if (iten->en_RenderType != CEntity::RT_MODEL)
    {
      continue;
    }

    BOOL bIsBackground = iten->en_ulFlags & ENF_BACKGROUND;

    if ((bBackground && !bIsBackground) || (!bBackground && bIsBackground))
    {
      continue;
    }*/

    RT_sCurrentScene = scene;

    Particle_PrepareEntity(4.0f, 0, 0, penViewer);
    ASSERT(_Particle_penCurrentViewer == penViewer);

    // RT: this will call function in DrawPort_Particles.cpp
    iten->RenderParticles();

    RT_sCurrentScene = nullptr;
    _Particle_penCurrentViewer = NULL;
  }

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

  preparedInfo->indices = &_aiCommonQuads[0];
  preparedInfo->indexCount = ctElements;
}


void RT_Particle_PrepareTexture(CTextureObject *pto, ParticleBlendType pbt)
{
  // determine blend type
  switch (pbt)
  {
    case PBT_BLEND:
      //  gfxBlendFunc(GFX_SRC_ALPHA, GFX_INV_SRC_ALPHA);
      RT_ePassThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_BLEND_UNDER;
      break;
    case PBT_ADD:
      // gfxBlendFunc(GFX_ONE, GFX_ONE);
      RT_ePassThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_BLEND_ADDITIVE;
      break;
    case PBT_MULTIPLY:
      // gfxBlendFunc(GFX_ZERO, GFX_INV_SRC_COLOR);
      RT_ePassThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_BLEND_UNDER;
      break;
    case PBT_ADDALPHA:
      // gfxBlendFunc(GFX_SRC_ALPHA, GFX_ONE);
      RT_ePassThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
      break;
    case PBT_FLEX:
      // gfxBlendFunc(GFX_ONE, GFX_INV_SRC_ALPHA);
      RT_ePassThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_BLEND_UNDER;
      break;
    case PBT_TRANSPARENT:
      RT_ePassThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
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
    const FLOATmatrix3D &m = RT_sCurrentScene->GetViewerRotation();

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
    const FLOATmatrix3D &m = RT_sCurrentScene->GetViewerRotation();

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

  // add to vertex arrays
  GFXVertex4 *pvtx = _avtxCommon.Push(4);
  GFXTexCoord *ptex = _atexCommon.Push(4);
  GFXColor *pcol = _acolCommon.Push(4);

  FLOAT3D v0 = vPos - dx + dy;
  FLOAT3D v1 = vPos - dx - dy;
  FLOAT3D v2 = vPos + dx - dy;
  FLOAT3D v3 = vPos + dx + dy;

  pvtx[0].x = v0(1);  pvtx[0].y = v0(2);  pvtx[0].z = v0(3);
  pvtx[1].x = v1(1);  pvtx[1].y = v1(2);  pvtx[1].z = v1(3);
  pvtx[2].x = v2(1);  pvtx[2].y = v2(2);  pvtx[2].z = v2(3);
  pvtx[3].x = v3(1);  pvtx[3].y = v3(2);  pvtx[3].z = v3(3);

  // prepare texture coords 
  ptex[0] = RT_atex[1];
  ptex[1] = RT_atex[0];
  ptex[2] = RT_atex[3];
  ptex[3] = RT_atex[2];

  // prepare colors
  const GFXColor glcol(col);
  pcol[0] = glcol;
  pcol[1] = glcol;
  pcol[2] = glcol;
  pcol[3] = glcol;
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
  GFXVertex *pvtx = _avtxCommon.Push(4);
  GFXTexCoord *ptex = _atexCommon.Push(4);
  GFXColor *pcol = _acolCommon.Push(4);

  // prepare vertices
  pvtx[0].x = a0(1);  pvtx[0].y = a0(2);  pvtx[0].z = a0(3);
  pvtx[1].x = b0(1);  pvtx[1].y = b0(2);  pvtx[1].z = b0(3);
  pvtx[2].x = b1(1);  pvtx[2].y = b1(2);  pvtx[2].z = b1(3);
  pvtx[3].x = a1(1);  pvtx[3].y = a1(2);  pvtx[3].z = a1(3);

  // prepare texture coords 
  ptex[0] = RT_atex[0];
  ptex[1] = RT_atex[1];
  ptex[2] = RT_atex[2];
  ptex[3] = RT_atex[3];

  // prepare colors
  const GFXColor glcol0(col0);
  const GFXColor glcol1(col1);
  pcol[0] = glcol0;
  pcol[1] = glcol1;
  pcol[2] = glcol1;
  pcol[3] = glcol0;
}


void RT_Particle_RenderQuad3D(const FLOAT3D &vPos0, const FLOAT3D &vPos1, const FLOAT3D &vPos2, const FLOAT3D &vPos3, COLOR col)
{ 
  // trivial rejection
  if (((col & CT_AMASK) >> CT_ASHIFT) < 2) return;

  // separate colors (for the sake of fog/haze)
  COLOR col0, col1, col2, col3;
  col0 = col1 = col2 = col3 = col;

  // add to vertex arrays
  GFXVertex *pvtx = _avtxCommon.Push(4);
  GFXTexCoord *ptex = _atexCommon.Push(4);
  GFXColor *pcol = _acolCommon.Push(4);

  // RT: use global space, so don't project
  pvtx[0].x = vPos0(1);  pvtx[0].y = vPos0(2);  pvtx[0].z = vPos0(3);
  pvtx[1].x = vPos1(1);  pvtx[1].y = vPos1(2);  pvtx[1].z = vPos1(3);
  pvtx[2].x = vPos2(1);  pvtx[2].y = vPos2(2);  pvtx[2].z = vPos2(3);
  pvtx[3].x = vPos3(1);  pvtx[3].y = vPos3(2);  pvtx[3].z = vPos3(3);

  // prepare texture coords 
  ptex[0] = RT_atex[0];
  ptex[1] = RT_atex[1];
  ptex[2] = RT_atex[2];
  ptex[3] = RT_atex[3];

  // prepare colors
  const GFXColor glcol0(col0);
  const GFXColor glcol1(col1);
  const GFXColor glcol2(col2);
  const GFXColor glcol3(col3);
  pcol[0] = glcol0;
  pcol[1] = glcol1;
  pcol[2] = glcol2;
  pcol[3] = glcol3;
}


void RT_Particle_Flush()
{
  if (RT_sCurrentScene != nullptr && _avtxCommon.Count() > 0)
  {
    FLOATmatrix3D identity;
    identity(1, 1) = 1; identity(1, 2) = 0; identity(1, 3) = 0;
    identity(2, 1) = 0; identity(2, 2) = 1; identity(2, 3) = 0;
    identity(3, 1) = 0; identity(3, 2) = 0; identity(3, 3) = 1;

    SSRT::CParticlesGeometry info = {};
    info.entityID = UINT32_MAX;
    info.absPosition = { 0,0,0 };
    info.absRotation = identity;
    info.passThroughType = RT_ePassThroughType;
    info.vertexCount = _avtxCommon.Count();
    info.vertices = &_avtxCommon[0];
    info.normals = nullptr;
    info.texCoords = &_atexCommon[0];

    // do we need multiple colors for particle batch?
    info.color = { _acolCommon[0].r / 256.0f, _acolCommon[0].g / 256.0f, _acolCommon[0].b / 256.0f, _acolCommon[0].a / 256.0f, };

    info.textures[0] = RT_ptd;
    info.textureFrames[0] = RT_iFrame;

    RT_GenerateQuadIndices(&info);

    RT_sCurrentScene->AddParticles(info);
  }

  // all done
  gfxResetArrays();
}

