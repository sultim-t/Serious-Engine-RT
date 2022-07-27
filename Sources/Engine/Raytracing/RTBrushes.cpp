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
#include <Engine/Brushes/Brush.h>
#include <Engine/Brushes/BrushTransformed.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/Graphics/RenderScene.h>
#include <Engine/Raytracing/SSRT.h>
#include <Engine/World/World.h>
#include <Engine/World/WorldSettings.h>
#include <Engine/World/WorldRayCasting.h>

#include <Engine/Templates/DynamicArray.h>
#include <Engine/Templates/DynamicArray.cpp>

#include <Engine/Base/ListIterator.inl>


#include "RTProcessing.h"
#include "SSRTGlobals.h"

extern SSRT::SSRTGlobals _srtGlobals;



constexpr uint32_t MAX_BRUSH_TEXTURE_COUNT = 3;



static uint32_t RT_BrushPartIndex = 0;
static CStaticStackArray<RgVertex> RT_AllSectorVertices;
static CStaticStackArray<INDEX> RT_AllSectorIndices;



static void RT_BrushClear()
{
  RT_AllSectorVertices.PopAll();
  RT_AllSectorIndices.PopAll();
}

void RT_BrushProcessingClear()
{
  RT_BrushClear();
}


struct RT_TextureLayerBlending
{
  UBYTE layerBlendingType[MAX_BRUSH_TEXTURE_COUNT];
  COLOR layerColor[MAX_BRUSH_TEXTURE_COUNT];
};


static bool RT_ShouldBeRasterized(uint32_t bpofFlags)
{
  // check comments in RT_GetPassThroughType
  return false; // bpofFlags & BPOF_TRANSLUCENT;
}


static RgGeometryPassThroughType RT_GetPassThroughType(uint32_t bpofFlags, bool isMirror, bool isWarpPortal, bool isWater, SSRT::Scene *pScene)
{
  if (isWarpPortal)
  {
    return RG_GEOMETRY_PASS_THROUGH_TYPE_PORTAL;
  }
  else if (isMirror)
  {
    return RG_GEOMETRY_PASS_THROUGH_TYPE_MIRROR;
  }
  else if (isWater)
  {
    bool isReflectiveAndRefractive = bpofFlags & BPOF_TRANSLUCENT;

    if (!isReflectiveAndRefractive && !pScene->GetCustomInfo()->IsOnlyReflectWaterAllowed())
    {
      isReflectiveAndRefractive = true;
    }

    return isReflectiveAndRefractive ?
      RG_GEOMETRY_PASS_THROUGH_TYPE_WATER_REFLECT_REFRACT :
      RG_GEOMETRY_PASS_THROUGH_TYPE_WATER_ONLY_REFLECT;
  }
  else if  (bpofFlags & BPOF_TRANSPARENT)
  {
    return RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
  }
  // it seems that tranclucent brushes should not be rasterized,
  // as (GFX_SRC_ALPHA, GFX_INV_SRC_ALPHA) blending is used,
  // example: sand canyon, lattice inside
  else if (bpofFlags & BPOF_TRANSLUCENT)
  {
    return RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
  }
  else
  {
    return RG_GEOMETRY_PASS_THROUGH_TYPE_OPAQUE;
  }
}


static RgGeometryMaterialBlendType RT_GetMaterialBlendType(UBYTE layerBlendingType)
{
  switch (layerBlendingType & STXF_BLEND_MASK)
  {
    case STXF_BLEND_OPAQUE: return RG_GEOMETRY_MATERIAL_BLEND_TYPE_OPAQUE;
    case STXF_BLEND_ALPHA:  return RG_GEOMETRY_MATERIAL_BLEND_TYPE_ALPHA;
    case STXF_BLEND_ADD:    return RG_GEOMETRY_MATERIAL_BLEND_TYPE_ADD;
    case STXF_BLEND_SHADE:  return RG_GEOMETRY_MATERIAL_BLEND_TYPE_SHADE;
    default:                return RG_GEOMETRY_MATERIAL_BLEND_TYPE_OPAQUE;
  }
}


#define IGNORE_BRUSH_COLOR 1


struct BrushFlushInfo
{
  CBrushPolygonTexture *textures[MAX_BRUSH_TEXTURE_COUNT];
  uint32_t polygonFlags;
  RT_TextureLayerBlending blending;
  bool hasScrollingTextures;
  RgGeometryPrimaryVisibilityType maskBit;
  bool isWater;
  bool isMirror;
  bool isWarpPortal;
  bool noMediaChangeOnRefraction;
  COLOR lastColor;

  explicit BrushFlushInfo()
  {
    blending = {};
    for (INDEX i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
    {
      textures[i] = nullptr;
      blending.layerColor[i] = C_WHITE | CT_OPAQUE;
      blending.layerBlendingType[i] = STXF_BLEND_OPAQUE;
    }
    polygonFlags = UINT32_MAX;
    hasScrollingTextures = false;
    maskBit = RgGeometryPrimaryVisibilityType::RG_GEOMETRY_VISIBILITY_TYPE_WORLD_0;
    isWater = false;
    isMirror = false;
    isWarpPortal = false;
    noMediaChangeOnRefraction = false;
    lastColor = C_WHITE | CT_OPAQUE;
  }
};


static void RT_FlushBrushInfo(CEntity *penBrush, uint32_t brushPartIndex, const BrushFlushInfo &f, bool onlyRasterized, SSRT::Scene *pScene)
{
  if (RT_AllSectorVertices.Count() == 0 || RT_AllSectorIndices.Count() == 0)
  {
    return;
  }


  const bool isMovable = pScene->IsBrushMovable(penBrush);
  const bool isSky = penBrush->en_ulFlags & ENF_BACKGROUND;

  // assuming that sky type is always RG_SKY_TYPE_RASTERIZED_GEOMETRY
  bool isRasterizedSky = isSky;

  bool isRasterized =
    (!f.isWater && RT_ShouldBeRasterized(f.polygonFlags)) ||
    isRasterizedSky;

  if (f.isMirror)
  {
    isRasterized = false;
  }

  if (onlyRasterized && !isRasterized)
  {
    return;
  }

  const CPlacement3D &placement = isMovable ?
    penBrush->GetLerpedPlacement() :
    penBrush->en_plPlacement;

  FLOAT3D position = placement.pl_PositionVector;
  FLOATmatrix3D rotation;
  MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

  SSRT::CBrushGeometry brushInfo = {};
  brushInfo.entityID = penBrush->en_ulID;
  brushInfo.isMovable = isMovable;
  brushInfo.passThroughType = RT_GetPassThroughType(f.polygonFlags, f.isMirror, f.isWarpPortal, f.isWater, pScene);
  brushInfo.isSky = isSky;
  
  brushInfo.isRasterized = isRasterized;

  if (isSky)
  {
    // convert rotation from background viewer space to world space,
    // as RTGL1 accepts viewer position
    FLOATmatrix3D bv;
    MakeInverseRotationMatrix(bv, pScene->GetBackgroundViewerOrientationAngle());

    rotation = bv * rotation;
  }

  brushInfo.absPosition = position;
  brushInfo.absRotation = rotation;

  brushInfo.vertexCount = RT_AllSectorVertices.Count();
  brushInfo.vertices = &RT_AllSectorVertices[0];

  brushInfo.indexCount = RT_AllSectorIndices.Count();
  brushInfo.indices = &RT_AllSectorIndices[0];

  brushInfo.brushPartIndex = brushPartIndex;
  brushInfo.hasScrollingTextures = f.hasScrollingTextures;

  brushInfo.isEmissive = false;

  uint32_t iDstIndex = 0;

  for (uint32_t iSrcIndex = 0; iSrcIndex < MAX_BRUSH_TEXTURE_COUNT; iSrcIndex++)
  {
    if (f.textures[iSrcIndex] == nullptr)
    {
      continue;
    }

    CTextureObject &to = f.textures[iSrcIndex]->bpt_toTexture;
    CTextureData *td = (CTextureData *)to.GetData();

    if (td == nullptr)
    {
      continue;
    }

    brushInfo.textureObjects[iDstIndex] = &to;

    // layer texture data
    brushInfo.textures[iDstIndex] = td;
    brushInfo.textureFrames[iDstIndex] = to.GetFrame();
      
    // layer blending
    auto blendType = RT_GetMaterialBlendType(f.blending.layerBlendingType[iSrcIndex]);

    // layer color
    GFXColor gcolor = GFXColor(f.blending.layerColor[iSrcIndex]);
    Vector<FLOAT, 4> fcolor = { (float)gcolor.r, (float)gcolor.g , (float)gcolor.b, (float)gcolor.a };
    fcolor /= 255.0f;

  #ifdef IGNORE_BRUSH_COLOR
    fcolor(1) = 1.0f;
    fcolor(2) = 1.0f;
    fcolor(3) = 1.0f;
  #endif // IGNORE_BRUSH_COLOR

    // texture modulation for second and third textures (RSSetInitialTextureParametersMT)
    if (iSrcIndex > 0)
    {
      // alpha-blended texture are oversaturated, ignore them
      if (blendType != RG_GEOMETRY_MATERIAL_BLEND_TYPE_ALPHA)
      {
        fcolor(1) *= 2;
        fcolor(2) *= 2;
        fcolor(3) *= 2;
      }
    }

    brushInfo.layerBlendings[iDstIndex] = blendType;
    brushInfo.layerColors[iDstIndex] = fcolor;

    brushInfo.isEmissive |= pScene->GetCustomInfo()->IsEmissionForced(&to);

    iDstIndex++;
  }

  // will be overriden
  brushInfo.blendEnable = false;
  brushInfo.blendSrc = brushInfo.blendDst = RG_BLEND_FACTOR_ONE;

  brushInfo.visibilityType = f.maskBit;

  brushInfo.noMediaChangeOnRefraction = f.noMediaChangeOnRefraction;

  pScene->AddBrush(brushInfo);
}


static void RT_UpdateBrushTexCoords(CEntity *penBrush, uint32_t brushPartIndex, SSRT::Scene *pScene)
{
  if (RT_AllSectorVertices.Count() == 0)
  {
    return;
  }

  SSRT::CUpdateTexCoordsInfo info = {};
  info.brushEntityID = penBrush->en_ulID;
  info.brushPartIndex = brushPartIndex;
  info.vertexCount = RT_AllSectorVertices.Count();

  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    // info.texCoordLayers[i] = &RT_AllSectorTexCoords[i][0];
  }

  // TODO: does nothing at the moment
  // pScene->UpdateBrushTexCoords(info);
}


static bool RT_AreTexturesSame(CBrushPolygonTexture *pPrevTextures[MAX_BRUSH_TEXTURE_COUNT],
                               CBrushPolygonTexture curTextures[MAX_BRUSH_TEXTURE_COUNT])
{
  CTextureData *cur[MAX_BRUSH_TEXTURE_COUNT];
  CTextureData *last[MAX_BRUSH_TEXTURE_COUNT];

  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    last[i] = pPrevTextures[i] == nullptr? nullptr : (CTextureData *)pPrevTextures[i]->bpt_toTexture.GetData();
    cur[i] = (CTextureData *)curTextures[i].bpt_toTexture.GetData();
  }

  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    if ((cur[i] == nullptr && last[i] != nullptr) ||
        (cur[i] != nullptr && last[i] == nullptr))
    {
      return false;
    }

    if (cur[i] != nullptr && last[i] != nullptr)
    {
      if (cur[i]->td_ulObject != last[i]->td_ulObject)
      {
        return false;
      }

      // if same material handles, and but still they're not registered
      if (cur[i]->td_ulObject == 0)
      {
        // check pointers
        if (cur[i] != last[i])
        {
          return false;
        }
      }
    }
  }

  return true;
}


static bool RT_AreBlendingsSame(const RT_TextureLayerBlending &a, const RT_TextureLayerBlending &b)
{
  for (INDEX i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    if (a.layerBlendingType[i] != b.layerBlendingType[i] ||
        a.layerColor[i] != b.layerColor[i])
    {
      return false;
    }
  }

  return true;
}


static void RT_ProcessPositions(const CBrushPolygon &polygon, RgVertex *vertices, INDEX vertCount)
{
  for (INDEX i = 0; i < vertCount; i++)
  {
    const CBrushVertex *brushVert = polygon.bpo_apbvxTriangleVertices[i];
    vertices[i].position[0] = brushVert->bvx_vRelative(1);
    vertices[i].position[1] = brushVert->bvx_vRelative(2);
    vertices[i].position[2] = brushVert->bvx_vRelative(3);
    //vertices[i].shade = 0;

    vertices[i].packedColor = UINT32_MAX;
  }
}


static void RT_ProcessNormals(const CBrushPolygon &polygon, RgVertex *vertices, uint32_t vertCount)
{
  float *planeNormal = polygon.bpo_pbplPlane->bpl_plRelative.vector;

  for (INDEX i = 0; i < vertCount; i++)
  {
    vertices[i].normal[0] = planeNormal[0];
    vertices[i].normal[1] = planeNormal[1];
    vertices[i].normal[2] = planeNormal[2];
  }
}


static bool RT_HasWaterTextures(CBrushPolygon &polygon, bool isWaterTexture[MAX_BRUSH_TEXTURE_COUNT], SSRT::Scene *pScene)
{
  bool has = false;

  for (uint32_t iLayer = 0; iLayer < MAX_BRUSH_TEXTURE_COUNT; iLayer++)
  {
    CBrushPolygonTexture &layerTexture = polygon.bpo_abptTextures[iLayer];
    CTextureData *ptd = (CTextureData *)layerTexture.bpt_toTexture.GetData();

    if (pScene->GetCustomInfo()->IsWaterTexture(ptd))
    {
      isWaterTexture[iLayer] = true;
      has = true;
      break;
    }
    else
    {
      isWaterTexture[iLayer] = false;
    }
  }

  return has;
}


static bool RT_HasScrollingTextures(CBrushPolygon &polygon)
{
  for (uint32_t iLayer = 0; iLayer < MAX_BRUSH_TEXTURE_COUNT; iLayer++)
  {
    CBrushPolygonTexture &layerTexture = polygon.bpo_abptTextures[iLayer];

    CTextureData *ptd = (CTextureData *)layerTexture.bpt_toTexture.GetData();
    if (ptd != nullptr)
    {
      INDEX iTransformation = layerTexture.s.bpt_ubScroll;

      if (iTransformation != 0)
      {
        return true;
      }
    }
  }

  return false;
}


static void RT_ProcessTexCoords(CBrushPolygon &polygon, RgVertex *vertices, uint32_t vertCount, SSRT::Scene *pScene)
{
  FLOAT3D polygonReferencePoint = polygon.bpo_pbplPlane->bpl_plRelative.ReferencePoint();
  const CMappingVectors &mvBrushSpace = polygon.bpo_pbplPlane->bpl_pwplWorking->wpl_mvRelative;
  for (INDEX i = 0; i < vertCount; i++)
  {
    memset(vertices[i].texCoord, 0, sizeof(vertices[i].texCoord));
    memset(vertices[i].texCoordLayer1, 0, sizeof(vertices[i].texCoordLayer1));
    memset(vertices[i].texCoordLayer2, 0, sizeof(vertices[i].texCoordLayer2));
  }

  for (uint32_t iLayer = 0; iLayer < MAX_BRUSH_TEXTURE_COUNT; iLayer++)
  {
    // tex coord data: from RSSetTextureCoords(..)
    // always push tex coords of vertCount amount, for 1 to 1 mapping of vertex atttributes

    CBrushPolygonTexture &layerTexture = polygon.bpo_abptTextures[iLayer];

    CTextureData *ptd = (CTextureData *)layerTexture.bpt_toTexture.GetData();
    if (ptd != nullptr)
    {
      const FLOAT mulU = 1024.0f / (float)ptd->GetWidth();
      const FLOAT mulV = 1024.0f / (float)ptd->GetHeight();

      CMappingVectors amvMapping;

      // if texture should be not transformed
      INDEX iTransformation = layerTexture.s.bpt_ubScroll;
      if (iTransformation == 0)
      {
        if ((polygon.bpo_abptTextures[iLayer].s.bpt_ubFlags & (BPTF_CLAMPU | BPTF_CLAMPV)) == 0)
        {
          // make a mapping adjusted for texture wrapping
          const MEX mexMaskU = ptd->GetWidth() - 1;
          const MEX mexMaskV = ptd->GetHeight() - 1;

          CMappingDefinition mdTmp = polygon.bpo_abptTextures[iLayer].bpt_mdMapping;
          mdTmp.md_fUOffset = (FloatToInt(mdTmp.md_fUOffset * 1024.0f) & mexMaskU) / 1024.0f;
          mdTmp.md_fVOffset = (FloatToInt(mdTmp.md_fVOffset * 1024.0f) & mexMaskV) / 1024.0f;

          const FLOAT3D vOffset = polygonReferencePoint - mvBrushSpace.mv_vO;

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
      }
      // if texture should be transformed
      else
      {
        // make mapping vectors as normal and then transform them
        CMappingDefinition &mdBase = polygon.bpo_abptTextures[iLayer].bpt_mdMapping;
        CMappingDefinition &mdScroll = pScene->GetWorld()->wo_attTextureTransformations[iTransformation].tt_mdTransformation;

        CMappingVectors mvTmp;
        mdBase.MakeMappingVectors(mvBrushSpace, mvTmp);

        mdScroll.TransformMappingVectors(mvTmp, amvMapping);
      }

      // adjust MEX
      amvMapping.mv_vU *= mulU;
      amvMapping.mv_vV *= mulV;

      const FLOAT3D &vO = amvMapping.mv_vO;
      const FLOAT3D &vU = amvMapping.mv_vU;
      const FLOAT3D &vV = amvMapping.mv_vV;

      for (INDEX i = 0; i < vertCount; i++)
      {
        const FLOAT fDX = vertices[i].position[0] - vO(1);
        const FLOAT fDY = vertices[i].position[1] - vO(2);
        const FLOAT fDZ = vertices[i].position[2] - vO(3);

        float s = vU(1) * fDX + vU(2) * fDY + vU(3) * fDZ;
        float t = vV(1) * fDX + vV(2) * fDY + vV(3) * fDZ;

        switch (iLayer)
        {
        case 0: 
          vertices[i].texCoord[0] = s;
          vertices[i].texCoord[1] = t;
          break;

        case 1:
          vertices[i].texCoordLayer1[0] = s;
          vertices[i].texCoordLayer1[1] = t;
          break;

        case 2:
          vertices[i].texCoordLayer2[0] = s;
          vertices[i].texCoordLayer2[1] = t;
          break;

        default:
          break;
        }
      }
    }
  }
}


static void RT_ProcessIndices(CBrushPolygon &polygon, uint32_t firstVertexId)
{
  // index data
  INDEX indexCount = polygon.bpo_aiTriangleElements.Count();
  INDEX *origIndices = &polygon.bpo_aiTriangleElements[0];

  INDEX *indices = RT_AllSectorIndices.Push(indexCount);

  // set new indices relative to the shift in RT_AllSectorVertices
  for (INDEX i = 0; i < indexCount; i++)
  {
    indices[i] = origIndices[i] + firstVertexId;
  }
}


static void RT_AddActiveSector(CBrushSector &bscSector, CEntity *penBrush, bool onlyTexCoords, bool onlyRasterized, SSRT::Scene *pScene)
{
  ASSERT(RT_AllSectorVertices.Count() == 0 || RT_AllSectorIndices.Count() == 0);


  CBrush3D *brush = bscSector.bsc_pbmBrushMip->bm_pbrBrush;
  if (brush->br_pfsFieldSettings != NULL)
  {
    return;
  }


  // keep last rendered info, to batch geometry
  BrushFlushInfo lastInfo = BrushFlushInfo();


  auto flushLastSavedInfo = [&] ()
  {
    if (!onlyTexCoords)
    {
      RT_FlushBrushInfo(penBrush, RT_BrushPartIndex, lastInfo, onlyRasterized, pScene);
    }
    // update tex coords only for scrolling textures
    else if (lastInfo.hasScrollingTextures)
    {
      RT_UpdateBrushTexCoords(penBrush, RT_BrushPartIndex, pScene);
    }

    RT_BrushClear();

    // TODO: use polygon ID as a base index and flush ID as additional
    RT_BrushPartIndex++;
  };


  FOREACHINSTATICARRAY(bscSector.bsc_abpoPolygons, CBrushPolygon, itpo)
  {
    CBrushPolygon &polygon = *itpo;

    // only portals that are translucent should be rendered
    bool isPortal = polygon.bpo_ulFlags & BPOF_PORTAL;
    bool isTranslucent = polygon.bpo_ulFlags & BPOF_TRANSLUCENT;

    bool ignorePolygon = (polygon.bpo_ulFlags & BPOF_INVISIBLE) != 0;
    ignorePolygon     |= (polygon.bpo_ulFlags & BPOF_OCCLUDER) != 0;
    ignorePolygon     |= isPortal && !isTranslucent;
    ignorePolygon     |= pScene->GetCustomInfo()->IsBrushPolygonIgnored(&polygon);

    // if shouldn't be rendered in game
    if (ignorePolygon)
    {
      continue;
    }


  #pragma region Warp/mirror polygon
    bool isMirror = false;
    bool isWarpPortal = false;
    INDEX iMirrorType = polygon.bpo_bppProperties.bpp_ubMirrorType;
    // if mirror
    if (iMirrorType != 0)
    {
      CMirrorParameters mirrorParams;
      BOOL bSuccess = penBrush->GetMirror(iMirrorType, mirrorParams);

      if (bSuccess)
      {
        // warp portal
        if (mirrorParams.mp_ulFlags & MPF_WARP)
        {
          isWarpPortal = true;
        }
        // mirror
        else
        {
          isMirror = true;
        }
      }
    }
  #pragma endregion


    // for texture cordinates and transparency/translucency processing
  #pragma region MakeScreenPolygon

    uint32_t polygonFlags = 0;

    if (polygon.bpo_ulFlags & BPOF_TRANSPARENT)
    {
      polygonFlags |= BPOF_TRANSPARENT;
    }

    if (isPortal && isTranslucent)
    {
      polygonFlags |= BPOF_TRANSLUCENT;
    }

    RT_TextureLayerBlending curBlending = {};

    // if GF_FLAT mode, i.e. pspo->spo_aptoTextures[0]==NULL || !bTexture0
    if (polygon.bpo_abptTextures[0].bpt_toTexture.GetData() == nullptr)
    {
      curBlending.layerBlendingType[0] = STXF_BLEND_OPAQUE;
      curBlending.layerColor[0] = polygon.bpo_colColor;

      for (INDEX i = 1; i < MAX_BRUSH_TEXTURE_COUNT; i++)
      {
        curBlending.layerBlendingType[i] = STXF_BLEND_OPAQUE;
        curBlending.layerColor[i] = C_WHITE | CT_OPAQUE;
      }
    }
    else
    {
      for (INDEX i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
      {
        CBrushPolygonTexture &layerTexture = polygon.bpo_abptTextures[i];
        CTextureBlending &tb = pScene->GetWorld()->wo_atbTextureBlendings[layerTexture.s.bpt_ubBlend];
        
        COLOR colorLayer = layerTexture.s.bpt_colColor;

        const float fOpacity = brush->br_penEntity->GetOpacity();

        // process transparency color
        if (fOpacity < 1)
        {
          const SLONG slOpacity = NormFloatToByte(fOpacity);
          UBYTE &layerBlending = tb.tb_ubBlendingType;

          // if texture is opaque 
          if ((layerBlending & STXF_BLEND_MASK) == 0)
          {
            // set it to blend with opaque alpha
            layerBlending |= STXF_BLEND_ALPHA;
            colorLayer |= CT_AMASK;
          }

          // if texture is blended
          if (layerBlending & STXF_BLEND_ALPHA)
          {
            // adjust it's alpha factor
            SLONG slAlpha = (colorLayer & CT_AMASK) >> CT_ASHIFT;
            slAlpha = (slAlpha * slOpacity) >> 8;

            colorLayer &= ~CT_AMASK;
            colorLayer |= slAlpha;
          }
        }

        colorLayer = MulColors(colorLayer, tb.tb_colMultiply);

        curBlending.layerBlendingType[i] = tb.tb_ubBlendingType;
        curBlending.layerColor[i] = colorLayer;
      }
    }

    bool hasScrollingTextures = RT_HasScrollingTextures(polygon);

    // commented to have brushPartIndex consistent with the previous uploads
    /*if (onlyTexCoords && !hasScrollingTextures)
    {
      continue;
    }*/

    bool isWaterPolygon = false;
    bool isWaterTexture[MAX_BRUSH_TEXTURE_COUNT] = {};

    if (!onlyTexCoords && !onlyRasterized)
    {
      isWaterPolygon = RT_HasWaterTextures(polygon, isWaterTexture, pScene);
    }

    RgGeometryPrimaryVisibilityType maskBit = pScene->GetCustomInfo()->GetBrushMaskBit(&polygon);
  #pragma endregion


    bool noMediaChange = pScene->GetCustomInfo()->DoesPolygonPreserveTheSameMedia(&polygon);


    // if settings are different
    bool mustBeFlushed =
      !RT_AreTexturesSame(lastInfo.textures, polygon.bpo_abptTextures) ||
      !RT_AreBlendingsSame(lastInfo.blending, curBlending) ||
      lastInfo.polygonFlags != polygonFlags ||
      lastInfo.hasScrollingTextures != hasScrollingTextures ||
      lastInfo.maskBit != maskBit ||
      lastInfo.isMirror != isMirror ||
      lastInfo.isWarpPortal != isWarpPortal ||
      lastInfo.noMediaChangeOnRefraction != noMediaChange;

    if (mustBeFlushed)
    {
      // flush last saved info, and start new recordings
      flushLastSavedInfo();
    }


    INDEX firstVertexId = RT_AllSectorVertices.Count();
    INDEX vertCount = polygon.bpo_apbvxTriangleVertices.Count();

    // position data
    RgVertex *vertices = RT_AllSectorVertices.Push(vertCount);

    if (!onlyTexCoords || hasScrollingTextures)
    {
      RT_ProcessPositions(polygon, vertices, vertCount);
      RT_ProcessTexCoords(polygon, vertices, vertCount, pScene);
    }

    if (!onlyTexCoords)
    {
      RT_ProcessNormals(polygon, vertices, vertCount);
      RT_ProcessIndices(polygon, firstVertexId);
    }


    // rewrite last info
    lastInfo.polygonFlags = polygonFlags;
    lastInfo.blending = curBlending;
    lastInfo.hasScrollingTextures = hasScrollingTextures;
    lastInfo.isWater = isWaterPolygon;
    lastInfo.maskBit = maskBit;
    lastInfo.isMirror = isMirror;
    lastInfo.isWarpPortal = isWarpPortal;
    lastInfo.noMediaChangeOnRefraction = noMediaChange;
    for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
    {
      lastInfo.textures[i] = &polygon.bpo_abptTextures[i];

      auto *td = (CTextureData *)lastInfo.textures[i]->bpt_toTexture.GetData();

      if (td != nullptr)
      {
        // set new local params for the texture
        td->td_tpLocal.tp_eWrapU = lastInfo.textures[i]->s.bpt_ubFlags & BPTF_CLAMPU ? GFX_CLAMP : GFX_REPEAT;
        td->td_tpLocal.tp_eWrapV = lastInfo.textures[i]->s.bpt_ubFlags & BPTF_CLAMPV ? GFX_CLAMP : GFX_REPEAT;
      }
    }
  }

  // flush what's left
  flushLastSavedInfo();
}


static void RT_ProcessBrushEntity(CEntity *penBrush, bool onlyTexCoords, bool onlyRasterized, SSRT::Scene *pScene)
{
  RT_BrushPartIndex = 0;

  ASSERT(penBrush != NULL);
  // get its brush
  CBrush3D &brBrush = *penBrush->en_pbrBrush;

  // if hidden
  if (penBrush->en_ulFlags & ENF_HIDDEN || penBrush->en_ulFlags & ENF_INVISIBLE)
  {
    // skip it
    return;
  }

  // RT: if onlyTexCoords=true, then penBrush was already ignored on 
  // previous RT_ProcessBrushEntity call, when onlyTexCoords was false.
  // Same with onlyRasterized
  if (!onlyTexCoords && !onlyRasterized)
  {
    if (pScene->GetCustomInfo()->IsBrushIgnored(penBrush))
    {
      return;
    }
  }


  // RT: get highest mip
  CBrushMip *pbm = brBrush.GetFirstMip();

  // if brush mip exists for that mip factor
  if (pbm != NULL)
  {
    // for each sector
    FOREACHINDYNAMICARRAY(pbm->bm_abscSectors, CBrushSector, itbsc)
    {
      // if the sector is not hidden
      if (!(itbsc->bsc_ulFlags & BSCF_HIDDEN) &&
          !pScene->GetCustomInfo()->IsBrushSectorIgnored(itbsc))
      {
        // add that sector to active sectors
        RT_AddActiveSector(itbsc.Current(), penBrush, onlyTexCoords, onlyRasterized, pScene);
      }
    }
  }
}


void RT_AddBrushEntity(CEntity *penBrush, SSRT::Scene *pScene)
{
  RT_ProcessBrushEntity(penBrush, false, false, pScene);
}


void RT_UpdateBrushTexCoords(CEntity *penBrush, SSRT::Scene *pScene)
{
  RT_ProcessBrushEntity(penBrush, true, false, pScene);
}


void RT_AddRasterizedBrushEntity(CEntity *penBrush, SSRT::Scene *pScene)
{
  RT_ProcessBrushEntity(penBrush, false, true, pScene);
}


void RT_UpdateBrushNonStaticTexture(CEntity *penBrush, SSRT::Scene *pScene)
{
  ASSERT(penBrush != NULL);
  // get its brush
  CBrush3D &brBrush = *penBrush->en_pbrBrush;

  // if hidden
  if (penBrush->en_ulFlags & ENF_HIDDEN || penBrush->en_ulFlags & ENF_INVISIBLE)
  {
    // skip it
    return;
  }

  // RT: ignored brushes are not passed to this function

  // RT: get highest mip
  CBrushMip *pbm = brBrush.GetFirstMip();

  // if brush mip exists for that mip factor
  if (pbm != NULL)
  {
    FOREACHINDYNAMICARRAY(pbm->bm_abscSectors, CBrushSector, itbsc)
    {
      // if the sector is not hidden
      if (!(itbsc->bsc_ulFlags & BSCF_HIDDEN))
      {
        CBrushSector &bscSector = *itbsc;

        CBrush3D *brush = bscSector.bsc_pbmBrushMip->bm_pbrBrush;
        if (brush->br_pfsFieldSettings != NULL)
        {
          continue;
        }

        for (uint32_t iPoly = 0; iPoly < bscSector.bsc_abpoPolygons.Count(); iPoly++)
        {
          CBrushPolygon &poly = bscSector.bsc_abpoPolygons[iPoly];

          for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
          {
            auto &to = poly.bpo_abptTextures[i].bpt_toTexture;

            auto *td = (CTextureData *)to.GetData();

            // if td exist and it's effect or animated texture
            if (td != nullptr && (td->td_ptegEffect != nullptr || td->td_ctFrames > 1))
            {
              uint32_t tdFrame = to.GetFrame();
              pScene->UpdateBrushNonStaticTexture(td, tdFrame);
            }
          }
        }
      }
    }
  }
}


void RT_CheckWarpPortalsAndMirrors(CEntity *penBrush, SSRT::Scene *scene)
{
  ASSERT(penBrush != NULL);
  // get its brush
  CBrush3D &brBrush = *penBrush->en_pbrBrush;

  // if hidden
  if (penBrush->en_ulFlags & ENF_HIDDEN || penBrush->en_ulFlags & ENF_INVISIBLE)
  {
    // skip it
    return;
  }

  // RT: ignored brushes are not passed to this function

  // RT: get highest mip
  CBrushMip *pbm = brBrush.GetFirstMip();

  // if brush mip exists for that mip factor
  if (pbm != NULL)
  {
    FOREACHINDYNAMICARRAY(pbm->bm_abscSectors, CBrushSector, itbsc)
    {
      // if the sector is not hidden
      if (!(itbsc->bsc_ulFlags & BSCF_HIDDEN))
      {
        CBrushSector &bscSector = *itbsc;

        CBrush3D *brush = bscSector.bsc_pbmBrushMip->bm_pbrBrush;
        if (brush->br_pfsFieldSettings != NULL)
        {
          continue;
        }

        for (uint32_t iPoly = 0; iPoly < bscSector.bsc_abpoPolygons.Count(); iPoly++)
        {
          CBrushPolygon &poly = bscSector.bsc_abpoPolygons[iPoly];

          INDEX iMirrorType = poly.bpo_bppProperties.bpp_ubMirrorType;
          // if mirror
          if (iMirrorType != 0)
          {
            CMirrorParameters mirrorParams;
            BOOL bSuccess = penBrush->GetMirror(iMirrorType, mirrorParams);

            // warp portal
            if (bSuccess && (mirrorParams.mp_ulFlags & MPF_WARP))
            {
              scene->AddWarpPortal(penBrush, iMirrorType);
            }
          }
        }
      }
    }
  }
}


void RT_PrintBrushPolygonInfo(SSRT::Scene *pScene)
{
  if (!_srtGlobals.srt_bPrintBrushPolygonInfo)
  {
    return;
  }

  bool bWithModels = _srtGlobals.srt_bPrintBrushPolygonInfo / 10 % 10;
  bool bWithTranslucent = _srtGlobals.srt_bPrintBrushPolygonInfo / 100 % 10;
  bool bIsPhysical = _srtGlobals.srt_bPrintBrushPolygonInfo / 1000 % 10;
  bool bWithTerrainInvisibleTris = _srtGlobals.srt_bPrintBrushPolygonInfo / 10000 % 10;
  
  bool isWater = false;

  if (_srtGlobals.srt_bPrintBrushPolygonInfo == 999999)
  {
    isWater = true;
    bWithModels = false;
    bIsPhysical = false;
    bWithTranslucent = true;
  }

  _srtGlobals.srt_bPrintBrushPolygonInfo = 0;

  if (pScene == nullptr || pScene->GetWorld() == nullptr)
  {
    return;
  }

  const auto &mRotation = pScene->GetCameraRotation();
  const auto vForward = FLOAT3D(mRotation(1, 3), mRotation(2, 3), mRotation(3, 3)) * -1;

  CCastRay crRay(pScene->GetViewerEntity(), pScene->GetCameraPosition(), pScene->GetCameraPosition() + vForward * 500.0f);
  crRay.cr_ttHitModels = bWithModels ? CCastRay::TT_FULL: CCastRay::TT_NONE;
  crRay.cr_bHitPortals = bWithTranslucent;
  crRay.cr_bHitTranslucentPortals = bWithTranslucent;
  crRay.cr_bPhysical = bIsPhysical;
  crRay.cr_bHitTerrainInvisibleTris = bWithTerrainInvisibleTris;
  pScene->GetWorld()->CastRay(crRay);

  CPrintF("\n");

  if (crRay.cr_penHit != nullptr)
  {
    if (crRay.cr_penHit->GetRenderType() == CEntity::RenderType::RT_MODEL)
    {
      CPrintF("Model:\n");
      CPrintF("%s\n", crRay.cr_penHit->GetName());
      CPrintF("en_ulID: %i\n", crRay.cr_penHit->en_ulID);
      CPrintF("cr_fHitDistance: %fm\n", crRay.cr_fHitDistance);

      auto *pmo = crRay.cr_penHit->en_pmoModelObject;

      if (pmo->mo_toTexture.ao_AnimData != nullptr)
      {
        CPrintF("mo_toTexture: %s\n", pmo->mo_toTexture.ao_AnimData->GetName());
      }

      FOREACHINLIST(CAttachmentModelObject, amo_lnInMain, pmo->mo_lhAttachments, itamo)
      {
        CAttachmentModelObject &amo = *itamo;

        CPrintF("Attachment %i: %s\n    Subattachments count: %i", amo.amo_iAttachedPosition, amo.amo_moModelObject.mo_lhAttachments.Count());

        if (amo.amo_moModelObject.mo_toTexture.ao_AnimData != nullptr)
        {
          CPrintF("    mo_toTexture: %s", amo.amo_moModelObject.mo_toTexture.ao_AnimData->GetName());
        }
      }

      if (pmo->mo_toReflection.ao_AnimData != nullptr)
      {
        CPrintF("mo_toReflection: %s\n", pmo->mo_toReflection.ao_AnimData->GetName());
      }

      return;
    }

    if (crRay.cr_penHit->GetRenderType() != CEntity::RenderType::RT_BRUSH)
    {
      return;
    }

    bool ok = !isWater;
    for (uint32_t i = 0; i < 3; i++)
    {
      CTextureData *ptd = (CTextureData *)crRay.cr_pbpoBrushPolygon->bpo_abptTextures[i].bpt_toTexture.ao_AnimData;

      if (ptd != nullptr)
      {
        ok |= pScene->GetCustomInfo()->IsWaterTexture(ptd);
      }
    }

    if (!ok)
    {
      return;
    }


    CPrintF("%s\n", crRay.cr_penHit->GetName());
    CPrintF("en_ulID: %i\n", crRay.cr_penHit->en_ulID);
    CPrintF("cr_fHitDistance: %fm\n", crRay.cr_fHitDistance);

    if (crRay.cr_pbscBrushSector != nullptr)
    {
      CPrintF("bsc_strName: %s\n", crRay.cr_pbscBrushSector->bsc_strName);
      CPrintF("bsc_iInWorld: %i\n", crRay.cr_pbscBrushSector->bsc_iInWorld);
    }

    if (crRay.cr_pbpoBrushPolygon != nullptr)
    {
      CPrintF("bpo_iInWorld: %i\n", crRay.cr_pbpoBrushPolygon->bpo_iInWorld);

      for (uint32_t i = 0; i < 3; i++)
      {
        CTextureData *ptd = (CTextureData *)crRay.cr_pbpoBrushPolygon->bpo_abptTextures[i].bpt_toTexture.ao_AnimData;

        if (ptd != nullptr)
        {
          CPrintF("%s\n", ptd->GetName().str_String);
        }
      }
    }
  }
}
