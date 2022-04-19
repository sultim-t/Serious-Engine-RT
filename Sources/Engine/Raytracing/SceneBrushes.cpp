/* Copyright (c) 2021 Sultim Tsyrendashiev
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
#include "SceneBrushes.h"

#include <Engine/World/World.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include "Scene.h"
#include "TextureUploader.h"
#include "RTProcessing.h"
#include "Utils.h"
#include "SSRTGlobals.h"


extern SSRT::SSRTGlobals _srtGlobals;


SSRT::SceneBrushes::SceneBrushes(RgInstance _instance, CWorld *_pWorld, TextureUploader *_pTextureUploader)
  : instance(_instance), pTextureUploader(_pTextureUploader), bRefreshMovableStates(false)
{
}

SSRT::SceneBrushes::~SceneBrushes()
{

}

static void GetBlendingFromMaterialBlending(
  RgGeometryMaterialBlendType materialBlend, 
  RgBool32 *blendEnable, RgBlendFactor *blendSrc, RgBlendFactor *blendDst)
{
  switch (materialBlend)
  {
  case RG_GEOMETRY_MATERIAL_BLEND_TYPE_ALPHA: *blendEnable = RG_TRUE; *blendSrc = RG_BLEND_FACTOR_SRC_ALPHA; *blendDst = RG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; break;
  case RG_GEOMETRY_MATERIAL_BLEND_TYPE_ADD:   *blendEnable = RG_TRUE; *blendSrc = RG_BLEND_FACTOR_ONE;       *blendDst = RG_BLEND_FACTOR_ONE; break;
  case RG_GEOMETRY_MATERIAL_BLEND_TYPE_SHADE: *blendEnable = RG_TRUE; *blendSrc = RG_BLEND_FACTOR_DST_COLOR; *blendDst = RG_BLEND_FACTOR_SRC_COLOR; break;
  default: *blendEnable = RG_FALSE;
  }
}

void SSRT::SceneBrushes::RegisterBrush(const CBrushGeometry &brush)
{
  if (brush.vertices == nullptr || brush.vertexCount == 0 || brush.indices == nullptr || brush.indexCount == 0)
  {
    return;
  }

  if (!brush.isRasterized)
  {
    ASSERT(!brush.isSky);

    RgGeometryUploadInfo stInfo = {};
    stInfo.uniqueID = brush.GetUniqueID();
    stInfo.geomType = brush.isMovable ?
      RG_GEOMETRY_TYPE_STATIC_MOVABLE :
      RG_GEOMETRY_TYPE_STATIC;
    stInfo.passThroughType = brush.passThroughType;
    stInfo.visibilityType = brush.visibilityType;
    stInfo.vertexCount = brush.vertexCount;
    stInfo.pVertexData = brush.vertices;
    stInfo.pNormalData = brush.normals;
    stInfo.defaultRoughness = _srtGlobals.srt_fBrushRoughnessDefault;
    stInfo.defaultMetallicity = _srtGlobals.srt_fBrushMetallicDefault;
    stInfo.defaultEmission = brush.isEmissive ? _srtGlobals.srt_fEmissionForFullbright : 0.0f;
    stInfo.indexCount = brush.indexCount;
    stInfo.pIndexData = brush.indices;

    for (uint32_t i = 0; i < 3; i++)
    {
      stInfo.pTexCoordLayerData[i] = brush.texCoordLayers[i];
      stInfo.layerBlendingTypes[i] = brush.layerBlendings[i];
      stInfo.layerColors[i] = { brush.layerColors[i](1), brush.layerColors[i](2), brush.layerColors[i](3), brush.layerColors[i](4) };
    }

    stInfo.geomMaterial =
    {
      pTextureUploader->GetMaterial(brush.textures[0], brush.textureFrames[0]),
      pTextureUploader->GetMaterial(brush.textures[1], brush.textureFrames[1]),
      pTextureUploader->GetMaterial(brush.textures[2], brush.textureFrames[2]),
    };

    stInfo.flags = brush.noMediaChangeOnRefraction ? RG_GEOMETRY_UPLOAD_NO_MEDIA_CHANGE_ON_REFRACT_BIT : 0;

    if (stInfo.passThroughType == RG_GEOMETRY_PASS_THROUGH_TYPE_MIRROR || 
        stInfo.passThroughType == RG_GEOMETRY_PASS_THROUGH_TYPE_PORTAL)
    {
      stInfo.flags |= RG_GEOMETRY_UPLOAD_REFL_REFR_ALBEDO_MULTIPLY_BIT;
    }

    Utils::CopyTransform(stInfo.transform, brush);

    RgResult r = rgUploadGeometry(instance, &stInfo);
    RG_CHECKERROR(r);
  }
  else
  {
    // for now, no other brushes except background should be drawn with rasterization;
    // SceneBrushes::Update uses this assumption
    ASSERT(brush.isSky);

    RgRasterizedGeometryVertexArrays vertInfo = {};
    vertInfo.pVertexData = brush.vertices;
    vertInfo.pColorData = nullptr;
    vertInfo.vertexStride = sizeof(GFXVertex);
    vertInfo.texCoordStride = sizeof(GFXTexCoord);
    vertInfo.colorStride = sizeof(GFXColor);

    RgRasterizedGeometryUploadInfo info = {};
    info.renderType = brush.isSky ? RG_RASTERIZED_GEOMETRY_RENDER_TYPE_SKY : RG_RASTERIZED_GEOMETRY_RENDER_TYPE_DEFAULT;
    info.vertexCount = brush.vertexCount;
    info.pArrays = &vertInfo;
    info.indexCount = brush.indexCount;
    info.pIndexData = brush.indices;

    Utils::CopyTransform(info.transform, brush);

    // draw each material layer separately (like in the original renderer)
    for (uint32_t i = 0; i < sizeof(brush.textures) / sizeof(brush.textures[0]); i++)
    {
      if (brush.textures[i] == nullptr)
      {
        continue;
      }
    
      vertInfo.pTexCoordData = brush.texCoordLayers[i];

      info.color = { brush.layerColors[i](1), brush.layerColors[i](2), brush.layerColors[i](3), brush.layerColors[i](4) };
      info.material = pTextureUploader->GetMaterial(brush.textures[i], brush.textureFrames[i]);

      info.pipelineState = RG_RASTERIZED_GEOMETRY_STATE_DEPTH_TEST | RG_RASTERIZED_GEOMETRY_STATE_DEPTH_WRITE;

      RgBool32 blendEnable = RG_FALSE;
      GetBlendingFromMaterialBlending(brush.layerBlendings[i], &blendEnable, &info.blendFuncSrc , &info.blendFuncDst);
      if (blendEnable)
      {
        info.pipelineState |= RG_RASTERIZED_GEOMETRY_STATE_BLEND_ENABLE;
      }

      RgResult r = rgUploadRasterizedGeometry(instance, &info, nullptr, nullptr);
      RG_CHECKERROR(r);
    }
  }

  if (!brush.isRasterized)
  {
    // save ray-traced movable brush info for updating
    if (brush.isMovable)
    {
      // create vector if doesn't exist
      if (entityToMovableBrush.find(brush.entityID) == entityToMovableBrush.end())
      {
        entityToMovableBrush[brush.entityID] = {};
      }

      entityToMovableBrush[brush.entityID].push_back(brush.GetUniqueID());
    }

    RegisterBrushTextures(brush);
  }
  else
  { 
    entityIsRasterizedBrush.insert(brush.entityID);
  }
}

void SSRT::SceneBrushes::RegisterBrushTextures(const CBrushGeometry &brush)
{
  // save effect and animated textures to update them each frame
  for (uint32_t i = 0; i < 3; i++)
  {
    CTextureObject *to = brush.textureObjects[i];
    CTextureData *td = brush.textures[i];

    // if effect or animated
    bool hasNonStaticTexture = to && td && (td->td_ptegEffect != nullptr || td->td_ctFrames > 1);

    if (hasNonStaticTexture)
    {
      entityHasNonStaticTexture.insert(brush.entityID);
      break;
    }
  }

  // save brush part geom index for updating dynamic texture coordinates
  if (brush.hasScrollingTextures)
  {
    // create vector if doesn't exist
    if (entitiesWithDynamicTexCoords.find(brush.entityID) == entitiesWithDynamicTexCoords.end())
    {
      entitiesWithDynamicTexCoords[brush.entityID] = {};
    }

    BrushPartGeometryIndex t = {};
    t.brushPartIndex = brush.brushPartIndex;
    t.vertexCount = brush.vertexCount;

    entitiesWithDynamicTexCoords[brush.entityID].push_back(t);
  }
}

void SSRT::SceneBrushes::UpdateBrushTexCoords(const CUpdateTexCoordsInfo &info)
{
  auto it = entitiesWithDynamicTexCoords.find(info.brushEntityID);
  ASSERT(it != entitiesWithDynamicTexCoords.end());

  for (const auto &part : it->second)
  {
    if (part.brushPartIndex == info.brushPartIndex)
    {
      if (part.vertexCount != info.vertexCount)
      {
        ASSERT(false);
        continue;
      }

      // if found geomIndex for that part
      RgUpdateTexCoordsInfo texCoordsInfo = {};
      texCoordsInfo.staticUniqueID = CBrushGeometry::GetBrushUniqueID(info.brushEntityID, info.brushPartIndex);
      texCoordsInfo.pTexCoordLayerData[0] = info.texCoordLayers[0];
      texCoordsInfo.pTexCoordLayerData[1] = info.texCoordLayers[1];
      texCoordsInfo.pTexCoordLayerData[2] = info.texCoordLayers[2];
      texCoordsInfo.vertexOffset = 0;
      texCoordsInfo.vertexCount = info.vertexCount;

      RgResult r = rgUpdateGeometryTexCoords(instance, &texCoordsInfo);
      RG_CHECKERROR(r);

      return;
    }
  }
}

void SSRT::SceneBrushes::SetMovableNewTransform(ULONG entityId, const CPlacement3D &placement)
{
  const auto it = entityToMovableBrush.find(entityId);

  if (it == entityToMovableBrush.end())
  {
    return;
  }

  RgResult r;

  RgUpdateTransformInfo updateInfo = {};
  Utils::CopyTransform(updateInfo.transform, placement);

  // for each part of this entity
  for (uint64_t uniqueId : it->second)
  {
    updateInfo.movableStaticUniqueID = uniqueId;

    r = rgUpdateGeometryTransform(instance, &updateInfo);
    RG_CHECKERROR(r);
  }
}

void SSRT::SceneBrushes::SetMovableAsHidden(ULONG entityId)
{
  const auto it = entityToMovableBrush.find(entityId);

  // ignore if it wasn't registered
  if (it == entityToMovableBrush.end())
  {
    return;
  }

  RgResult r;

  // to hide brush, let the trasform be null
  RgUpdateTransformInfo updateInfo = {};
  updateInfo.transform = {};

  for (uint64_t uniqueId : it->second)
  {
    updateInfo.movableStaticUniqueID = uniqueId;

    r = rgUpdateGeometryTransform(instance, &updateInfo);
    RG_CHECKERROR(r);
  }
}

bool SSRT::SceneBrushes::ArePlacementsSame(const CPlacement3D &a, const CPlacement3D &b)
{
  float posDiff = 0.0f;
  float angDiff = 0.0f;

  for (int i = 0; i < 3; i++)
  {
    posDiff += Abs(a.pl_PositionVector.vector[i] - b.pl_PositionVector.vector[i]);
    angDiff += Abs(a.pl_OrientationAngle.vector[i] - b.pl_OrientationAngle.vector[i]);
  }
  
  const float posEps = 0.005f;
  const float angEps = 0.005f;

  return posDiff < posEps && angDiff < angEps;
}

void SSRT::SceneBrushes::UpdateMovableBrush(CEntity *pBrushEntity)
{
  ULONG entityId = pBrushEntity->en_ulID;

  const bool isHidden = pBrushEntity->en_ulFlags & ENF_HIDDEN;
  const auto &pl = pBrushEntity->GetLerpedPlacement();

  const auto f = entityToMovableBrushPlacement.find(entityId);
  MovableState *movableState = nullptr;

  if (!bRefreshMovableStates)
  {
    if (f != entityToMovableBrushPlacement.end())
    {
      movableState = &f->second;

      // if should be and was hidden
      if (isHidden && movableState->isHidden)
      {
        return;
      }

      // if shouldn't be and wasn't hidden, check the placements
      if (!isHidden && !movableState->isHidden)
      {
        if (ArePlacementsSame(movableState->placement, pl))
        {
          // if it wasn't moving in prev frame, skip it
          if (!movableState->wasMoving)
          {
            return;
          }

          // if it was, then mark it
          movableState->wasMoving = false;
        }
        else
        {
          movableState->wasMoving = true;
        }
      }
    }
    else
    {
      // create and get pointer
      movableState = &entityToMovableBrushPlacement[entityId];
    }
  }

  if (!isHidden)
  {
    // if it's a visible movable brush, update transform
    SetMovableNewTransform(entityId, pl);

    movableState->isHidden = false;
    movableState->placement = pl;
  }
  else
  {
    SetMovableAsHidden(entityId);

    movableState->isHidden = true;
  }
}

void SSRT::SceneBrushes::Update(CEntity *pBrushEntity, Scene *pScene)
{
  ASSERT(pBrushEntity->GetRenderType() == CEntity::RT_BRUSH);

  ULONG entityId = pBrushEntity->en_ulID;


  {
    // if it's rasterized
    if (entityIsRasterizedBrush.find(entityId) != entityIsRasterizedBrush.end())
    {
      // assume that only sky brushes are rasterized
      if (!(pBrushEntity->GetFlags() & ENF_BACKGROUND))
      {
        return;
      }

      RT_AddRasterizedBrushEntity(pBrushEntity, pScene);

      // further processing is for ray traced geometry, so ignore them
      return;
    }
  }


  {
    // if it has non static texture
    if (entityHasNonStaticTexture.find(entityId) != entityHasNonStaticTexture.end())
    {
      RT_UpdateBrushNonStaticTexture(pBrushEntity, pScene);
    }
  }


  // if brush has dynamic texture coords and they're not ignored
  if (!pScene->GetCustomInfo()->AreDynamicTexCoordsIgnored(pBrushEntity) && entitiesWithDynamicTexCoords.find(entityId) != entitiesWithDynamicTexCoords.end())
  {
    // this will call UpdateBrushTexCoords(..)
    RT_UpdateBrushTexCoords(pBrushEntity, pScene);
  }


  // process movable
  if (pScene->IsBrushMovable(pBrushEntity))
  {
    UpdateMovableBrush(pBrushEntity);
  }


  RT_CheckWarpPortalsAndMirrors(pBrushEntity, pScene);


  bRefreshMovableStates = false;
}

void SSRT::SceneBrushes::ResetOnGameStop()
{
  bRefreshMovableStates = true;
}
