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
#include <Engine/Brushes/Brush.h>
#include <Engine/Brushes/BrushTransformed.h>
#include <Engine/Graphics/RenderScene.h>
#include <Engine/Raytracing/SSRT.h>
#include <Engine/World/World.h>
#include <Engine/World/WorldSettings.h>

#include <Engine/Templates/DynamicArray.h>
#include <Engine/Templates/DynamicArray.cpp>



// TODO: change to brush texture count to 3 after fixing
#define MAX_BRUSH_TEXTURE_COUNT_IS_ONE



static CStaticStackArray<GFXVertex> RT_AllSectorVertices;
static CStaticStackArray<INDEX> RT_AllSectorIndices;
static CStaticStackArray<GFXTexCoord> RT_AllSectorTexCoords;
static CTextureObject *RT_LastSectorTextures[3];


// RT: ScenePolygon class for RT processing
struct RT_ScenePolygon
{
  INDEX  spo_iVtx0;           // first vertex in arrays
  INDEX  spo_ctVtx;           // number of vertices in arrays
  INDEX *spo_piElements;      // array of triangle elements
  INDEX  spo_ctElements;      // element count

  // RT: only 3, shadow info is not included 
  CMappingVectors spo_amvMapping[3];  // texture colors and alpha
  COLOR spo_acolColors[3];            // texture flags
  UBYTE spo_aubTextureFlags[3];       // current mip factors for each texture

  CTextureObject *spo_aptoTextures[3];

  // internal for rendering
  INDEX spo_iVtx0Pass;     // index of first coordinate in per-pass arrays
  COLOR spo_cColor;        // polygon color (for flat or shadow modes)
  ULONG spo_ulFlags;       // polygon flags (selected or not? ...)
};


static void FlushBrushInfo(CEntity *penBrush, SSRT::Scene *scene);


// Copied, CRenderer::SetOneTextureParameters from RenCache.cpp
/*static void RT_SetOneTextureParameters(CBrushPolygon &bpo, RT_ScenePolygon &spo, INDEX iLayer, CWorld *pWorld)
{
  spo.spo_aptoTextures[iLayer] = NULL;
  CTextureData *ptd = (CTextureData *)bpo.bpo_abptTextures[iLayer].bpt_toTexture.GetData();

  // if there is no texture or it should not be shown
  // RT: _wrpWorldRenderPrefs.wrp_abTextureLayers is ignored: all layers must be shown
  if (ptd == NULL)
  {
    // do nothing
    return;
  }

  // RT: working plane is null
  // CWorkingPlane &wpl = *bpo.bpo_pbplPlane->bpl_pwplWorking;

  // set texture and its parameters
  spo.spo_aptoTextures[iLayer] = &bpo.bpo_abptTextures[iLayer].bpt_toTexture;

  // get texture blending type
  CTextureBlending &tb = pWorld->wo_atbTextureBlendings[bpo.bpo_abptTextures[iLayer].s.bpt_ubBlend];

  // set texture blending flags
  ASSERT(BPTF_CLAMPU == STXF_CLAMPU && BPTF_CLAMPV == STXF_CLAMPV && BPTF_AFTERSHADOW == STXF_AFTERSHADOW);

  spo.spo_aubTextureFlags[iLayer] =
    (bpo.bpo_abptTextures[iLayer].s.bpt_ubFlags & (BPTF_CLAMPU | BPTF_CLAMPV | BPTF_AFTERSHADOW))
    | (tb.tb_ubBlendingType);

  if (bpo.bpo_abptTextures[iLayer].s.bpt_ubFlags & BPTF_REFLECTION)
  {
    spo.spo_aubTextureFlags[iLayer] |= STXF_REFLECTION;
  }

  // set texture blending color
  spo.spo_acolColors[iLayer] = MulColors(bpo.bpo_abptTextures[iLayer].s.bpt_colColor, tb.tb_colMultiply);

  // if texture should be not transformed
  INDEX iTransformation = bpo.bpo_abptTextures[iLayer].s.bpt_ubScroll;
  if (iTransformation == 0)
  {
    // if texture is wrapped on both axes
    if ((bpo.bpo_abptTextures[iLayer].s.bpt_ubFlags & (BPTF_CLAMPU | BPTF_CLAMPV)) == 0)
    {
      // make a mapping adjusted for texture wrapping
      const MEX mexMaskU = ptd->GetWidth() - 1;
      const MEX mexMaskV = ptd->GetHeight() - 1;

      CMappingDefinition mdTmp = bpo.bpo_abptTextures[iLayer].bpt_mdMapping;
      mdTmp.md_fUOffset = (FloatToInt(mdTmp.md_fUOffset * 1024.0f) & mexMaskU) / 1024.0f;
      mdTmp.md_fVOffset = (FloatToInt(mdTmp.md_fVOffset * 1024.0f) & mexMaskV) / 1024.0f;

      const FLOAT3D vOffset = wpl.wpl_plView.ReferencePoint() - wpl.wpl_mvView.mv_vO;

      const FLOAT fS = vOffset % wpl.wpl_mvView.mv_vU;
      const FLOAT fT = vOffset % wpl.wpl_mvView.mv_vV;
      const FLOAT fU = fS * mdTmp.md_fUoS + fT * mdTmp.md_fUoT + mdTmp.md_fUOffset;
      const FLOAT fV = fS * mdTmp.md_fVoS + fT * mdTmp.md_fVoT + mdTmp.md_fVOffset;

      mdTmp.md_fUOffset += (FloatToInt(fU * 1024.0f) & ~mexMaskU) / 1024.0f;
      mdTmp.md_fVOffset += (FloatToInt(fV * 1024.0f) & ~mexMaskV) / 1024.0f;

      // make texture mapping vectors from default vectors of the plane
      mdTmp.MakeMappingVectors(wpl.wpl_mvView, spo.spo_amvMapping[iLayer]);
    }
    // if texture is clamped
    else
    {
      // just make texture mapping vectors from default vectors of the plane
      bpo.bpo_abptTextures[iLayer].bpt_mdMapping.MakeMappingVectors(wpl.wpl_mvView, spo.spo_amvMapping[iLayer]);
    }
  }
  // if texture should be transformed
  else
  {
    // make mapping vectors as normal and then transform them
    CMappingDefinition &mdBase = bpo.bpo_abptTextures[iLayer].bpt_mdMapping;
    CMappingDefinition &mdScroll = pWorld->wo_attTextureTransformations[iTransformation].tt_mdTransformation;

    CMappingVectors mvTmp;
    mdBase.MakeMappingVectors(wpl.wpl_mvView, mvTmp);

    mdScroll.TransformMappingVectors(mvTmp, spo.spo_amvMapping[iLayer]);
  }
}*/


static bool AreLastTexturesSame(CBrushPolygonTexture textures[3])
{
  CTextureData *cur[] = 
  {
    (CTextureData *)textures[0].bpt_toTexture.GetData(),
  #ifdef MAX_BRUSH_TEXTURE_COUNT_IS_ONE
    nullptr,
    nullptr,
  #else
    (CTextureData *)textures[1].bpt_toTexture.GetData(),
    (CTextureData *)textures[2].bpt_toTexture.GetData(),
  #endif
  };

  CTextureData *last[] =
  {
    RT_LastSectorTextures[0] != nullptr ? (CTextureData *)RT_LastSectorTextures[0]->GetData() : nullptr,
  #ifdef MAX_BRUSH_TEXTURE_COUNT_IS_ONE
    nullptr,
    nullptr,
  #else
    RT_LastSectorTextures[1] != nullptr ? (CTextureData *)RT_LastSectorTextures[1]->GetData() : nullptr,
    RT_LastSectorTextures[2] != nullptr ? (CTextureData *)RT_LastSectorTextures[2]->GetData() : nullptr,
  #endif
  };

  for (uint32_t i = 0; i < 3; i++)
  {
    if (cur[i] == nullptr && last[i] != nullptr)
    {
      return false;
    }

    if (cur[i] != nullptr && last[i] == nullptr)
    {
      return false;
    }

    if (cur[i] != nullptr && last[i] != nullptr)
    {
      if (cur[i]->td_ulObject != last[i]->td_ulObject)
      {
        return false;
      }
    }
  }

  return true;
}


static void RT_AddActiveSector(CBrushSector &bscSector, CEntity *penBrush, SSRT::Scene *scene)
{
  CBrush3D *brush = bscSector.bsc_pbmBrushMip->bm_pbrBrush;
  if (brush->br_pfsFieldSettings != NULL)
  {
    return;
  }

  // RT: simulate ScenePolygon
  RT_ScenePolygon sppo;

  FOREACHINSTATICARRAY(bscSector.bsc_abpoPolygons, CBrushPolygon, itpo)
  {
    CBrushPolygon &polygon = *itpo;

    // for texture cordinates and transparency/translucency processing
  #pragma region MakeScreenPolygon

    if ((polygon.bpo_ulFlags & BPOF_PORTAL)/* || (polygon.bpo_ulFlags & BPOF_INVISIBLE)*/)
    {
      continue;
    }


    // TODO: RT: texture coordinates for brushes
    //RT_SetOneTextureParameters(polygon, sppo, 0, scene->GetWorld());
    //RT_SetOneTextureParameters(polygon, sppo, 1, scene->GetWorld());
    //RT_SetOneTextureParameters(polygon, sppo, 2, scene->GetWorld());

    if (polygon.bpo_ulFlags & BPOF_TRANSPARENT)
    {
      // TODO: RT: if needs alpha keying
    }

    bool needTranslucency = FALSE;
    const float fOpacity = brush->br_penEntity->GetOpacity();
    if (fOpacity < 1)
    {
      needTranslucency = TRUE;
    }

    // if a translucent brush or a translucent portal
    if (needTranslucency || ((polygon.bpo_ulFlags & BPOF_PORTAL) && (polygon.bpo_ulFlags & BPOF_TRANSLUCENT)))
    {
      polygon.bpo_ulFlags |= BPOF_RENDERTRANSLUCENT;
      // TODO: RT: translucent brushes
    }
    else
    {

    }
  #pragma endregion

    INDEX firstVertexId = RT_AllSectorVertices.Count();
    INDEX vertCount = polygon.bpo_apbvxTriangleVertices.Count();

    // position data
    GFXVertex *vertices = RT_AllSectorVertices.Push(vertCount);

    for (INDEX i = 0; i < vertCount; i++)
    {
      CBrushVertex *brushVert = polygon.bpo_apbvxTriangleVertices[i];
      vertices[i].x = brushVert->bvx_vRelative(1);
      vertices[i].y = brushVert->bvx_vRelative(2);
      vertices[i].z = brushVert->bvx_vRelative(3);
      vertices[i].shade = 0;
    }

    // tex coord data: from RSSetTextureCoords(..)
    GFXTexCoord *texCoords = RT_AllSectorTexCoords.Push(vertCount);

    // TODO: RT: more than 1 texture coords for brushes
  #ifdef MAX_BRUSH_TEXTURE_COUNT_IS_ONE
    uint32_t iLayer = 0;
  #else
    for (uint32_t iLayer =0;)
  #endif
    CTextureData *ptd = (CTextureData *)polygon.bpo_abptTextures[iLayer].bpt_toTexture.GetData();
    if (ptd != nullptr)
    {
      const FLOAT mulU = 1024.0f / (float)ptd->GetWidth();
      const FLOAT mulV = 1024.0f / (float)ptd->GetHeight();

      FLOATplane3D &plane = polygon.bpo_pbplPlane->bpl_plRelative;

      CMappingVectors mvBrushSpace;
      mvBrushSpace.FromPlane(plane);

      CMappingVectors amvMapping;

      if ((polygon.bpo_abptTextures[iLayer].s.bpt_ubFlags & (BPTF_CLAMPU | BPTF_CLAMPV)) == 0)
      {
        // make a mapping adjusted for texture wrapping
        const MEX mexMaskU = ptd->GetWidth() - 1;
        const MEX mexMaskV = ptd->GetHeight() - 1;

        CMappingDefinition mdTmp = polygon.bpo_abptTextures[iLayer].bpt_mdMapping;
        mdTmp.md_fUOffset = (FloatToInt(mdTmp.md_fUOffset * 1024.0f) & mexMaskU) / 1024.0f;
        mdTmp.md_fVOffset = (FloatToInt(mdTmp.md_fVOffset * 1024.0f) & mexMaskV) / 1024.0f;

        const FLOAT3D vOffset = plane.ReferencePoint() - mvBrushSpace.mv_vO;

        const FLOAT fS = vOffset % mvBrushSpace.mv_vU;
        const FLOAT fT = vOffset % mvBrushSpace.mv_vV;
        const FLOAT fU = fS * mdTmp.md_fUoS + fT * mdTmp.md_fUoT + mdTmp.md_fUOffset;
        const FLOAT fV = fS * mdTmp.md_fVoS + fT * mdTmp.md_fVoT + mdTmp.md_fVOffset;

        mdTmp.md_fUOffset += (FloatToInt(fU * 1024.0f) & ~mexMaskU) / 1024.0f;
        mdTmp.md_fVOffset += (FloatToInt(fV * 1024.0f) & ~mexMaskV) / 1024.0f;

        // make texture mapping vectors from default vectors of the plane
        mdTmp.MakeMappingVectors(mvBrushSpace, amvMapping);
      }
      else
      {
        polygon.bpo_abptTextures[iLayer].bpt_mdMapping.MakeMappingVectors(mvBrushSpace, amvMapping);
      }

      // adjust MEX
      amvMapping.mv_vU *= mulU;
      amvMapping.mv_vV *= mulV;

      const FLOAT3D &vO = amvMapping.mv_vO;
      const FLOAT3D &vU = amvMapping.mv_vU;
      const FLOAT3D &vV = amvMapping.mv_vV;

      for (INDEX i = 0; i < vertCount; i++)
      {
          const FLOAT fDX = vertices[i].x - vO(1);
          const FLOAT fDY = vertices[i].y - vO(2);
          const FLOAT fDZ = vertices[i].z - vO(3);

          texCoords[i].s = vU(1) * fDX + vU(2) * fDY + vU(3) * fDZ;
          texCoords[i].t = vV(1) * fDX + vV(2) * fDY + vV(3) * fDZ;
      }
    }


    // index data
    INDEX indexCount = polygon.bpo_aiTriangleElements.Count();
    INDEX *origIndices = &polygon.bpo_aiTriangleElements[0];

    INDEX *indices = RT_AllSectorIndices.Push(indexCount);

    // set new indices relative to the shift in RT_AllSectorVertices
    for (INDEX i = 0; i < indexCount; i++)
    {
      indices[i] = origIndices[i] + firstVertexId;
    }



    // TODO: RT: brush textures
    //COLOR color = polygon.bpo_colColor;

    if (!AreLastTexturesSame(polygon.bpo_abptTextures))
    {
      RT_LastSectorTextures[0] = &polygon.bpo_abptTextures[0].bpt_toTexture;
    #ifdef MAX_BRUSH_TEXTURE_COUNT_IS_ONE
      RT_LastSectorTextures[1] = nullptr;
      RT_LastSectorTextures[2] = nullptr;
    #else
      RT_LastSectorTextures[1] = &polygon.bpo_abptTextures[1].bpt_toTexture;
      RT_LastSectorTextures[2] = &polygon.bpo_abptTextures[2].bpt_toTexture;
    #endif

      FlushBrushInfo(penBrush, scene);
    }
  }
}


void RT_AddNonZoningBrush(CEntity *penBrush, SSRT::Scene *scene)
{
  RT_AllSectorVertices.PopAll();
  RT_AllSectorIndices.PopAll();
  RT_AllSectorTexCoords.PopAll();
  RT_LastSectorTextures[0] = nullptr;
  RT_LastSectorTextures[1] = nullptr;
  RT_LastSectorTextures[2] = nullptr;

  ASSERT(penBrush != NULL);
  // get its brush
  CBrush3D &brBrush = *penBrush->en_pbrBrush;

  // if hidden
  if (penBrush->en_ulFlags & ENF_HIDDEN)
  {
    // skip it
    return;
  }

  // RT: this must not happen, as iteration in AddAllEntities is done over all brushes
  //// if the brush is already added
  //if (brBrush.br_lnInActiveBrushes.IsLinked())
  //  return;

  //if (penBrush->en_ulFlags & ENF_ZONING)
  //  return;

  // skip whole non-zoning brush if all polygons in all sectors are invisible for rendering 
  bool isVisible = false;

  // test every brush polygon for it's visibility flag
  // for every sector in brush
  FOREACHINDYNAMICARRAY(brBrush.GetFirstMip()->bm_abscSectors, CBrushSector, itbsc)
  {
    // for all polygons in sector
    FOREACHINSTATICARRAY(itbsc->bsc_abpoPolygons, CBrushPolygon, itpo)
    {
      // advance to next polygon if invisible
      CBrushPolygon &bpo = *itpo;
      if (!(bpo.bpo_ulFlags & BPOF_INVISIBLE))
      {
        isVisible = true;
        break;
      }
    }
  }

  if (!isVisible)
  {
    return;
  }

  // RT: get highest mip
  CBrushMip *pbm = brBrush.GetBrushMipByDistance(0);

  // if brush mip exists for that mip factor
  if (pbm != NULL)
  {
    ASSERT(RT_AllSectorVertices.Count() == 0 && RT_AllSectorIndices.Count() == 0);

    // for each sector
    FOREACHINDYNAMICARRAY(pbm->bm_abscSectors, CBrushSector, itbsc)
    {
      // if the sector is not hidden
      if (!(itbsc->bsc_ulFlags & BSCF_HIDDEN))
      {
        // add that sector to active sectors
        RT_AddActiveSector(itbsc.Current(), penBrush, scene);
      }
    }

    FlushBrushInfo(penBrush, scene);
  }
}

static void FlushBrushInfo(CEntity *penBrush, SSRT::Scene *scene)
{
  if (RT_AllSectorVertices.Count() == 0 || RT_AllSectorIndices.Count() == 0)
  {
    return;
  }

  bool isMovable = penBrush->en_ulPhysicsFlags & EPF_MOVABLE;

  const CPlacement3D &placement = isMovable ?
    penBrush->GetLerpedPlacement() :
    penBrush->en_plPlacement;

  FLOAT3D position = placement.pl_PositionVector;
  FLOATmatrix3D rotation;
  MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

  SSRT::CBrushGeometry brushInfo = {};
  brushInfo.entityID = penBrush->en_ulID;
  brushInfo.isEnabled = true;
  brushInfo.isMovable = isMovable;
  brushInfo.color = RGBAToColor(255, 255, 255, 255);

  brushInfo.absPosition = position;
  brushInfo.absRotation = rotation;

  brushInfo.vertexCount = RT_AllSectorVertices.Count();
  brushInfo.vertices = &RT_AllSectorVertices[0];
  brushInfo.texCoords = &RT_AllSectorTexCoords[0];
  brushInfo.normals = nullptr;

  brushInfo.indexCount = RT_AllSectorIndices.Count();
  brushInfo.indices = &RT_AllSectorIndices[0];

#ifdef MAX_BRUSH_TEXTURE_COUNT_IS_ONE
  uint32_t i = 0;
#else
  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
#endif
  {
    CTextureObject *to = RT_LastSectorTextures[i];
    CTextureData *td = to != nullptr ? (CTextureData *)to->GetData() : nullptr;

    brushInfo.textures[i] = td;
    brushInfo.textureFrames[i] = to != nullptr && td != nullptr ? to->GetFrame() : 0;
  }

  scene->AddBrush(brushInfo);

  RT_AllSectorVertices.PopAll();
  RT_AllSectorIndices.PopAll();
  RT_AllSectorTexCoords.PopAll();
  RT_LastSectorTextures[0] = nullptr;
  RT_LastSectorTextures[1] = nullptr;
  RT_LastSectorTextures[2] = nullptr;
}