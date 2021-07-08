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
#include "CustomInfo.h"

#include <Engine/Graphics/Texture.h>
#include <Engine/Entities/Entity.h>
#include <Engine/World/World.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/Models/ModelData.h>
#include <Engine/Light/LightSource.h>
#include <Engine/Graphics/TextureEffects.h>
#include <Engine/Raytracing/SSRTGlobals.h>
#include <Engine/Raytracing/Scene.h>


#include <Engine/Base/ListIterator.inl>


extern SSRT::SSRTGlobals _srtGlobals;


constexpr float WORLD_BASE_FORCE_INVISIBLE_EPSILON = 0.5f;


static struct 
{
  const char  *pWorldName;
  INDEX       iCullingMaxSectorDepth;
} 
RT_WorldCullingMaxSectorDepth[] =
{
  { "01_Hatshepsut",        4 },
  { "02_SandCanyon",        8 },
  { "03_TombOfRamses",      7 },
  { "04_ValleyOfTheKings",  5 },
  { "05_MoonMountains",     4 },
  { "06_Oasis",             4 },
  { "07_Dunes",             8 },
  { "08_Suburbs",           2 },
  { "09_Sewers",            4 },
  { "10_Metropolis",        5 },
  { "11_AlleyOfSphinxes",   3 },
  { "12_Karnak",            5 },
  { "13_Luxor",             4 },
  { "14_SacredYards",       4 },
  { "15_TheGreatPyramid",   5 },
};


SSRT::CustomInfo::CustomInfo(CWorld *pWorld)
{
  ASSERT(pWorld != nullptr);

  waterTextureNames = 
  {
    "WaterBase",
    "WaterFX",
    "WaterFall01",
    "WaterFall02",
  };

  fireTextureNames =
  {
    "Fire",
  };

  // Positions of unnecessary brushes (that also have "World Base" names)
  worldBaseToIgnore =
  {
    { "01_Hatshepsut",        { 0, 1152, 64 }       },
    { "02_SandCanyon",        { 224, 128, -312}     },
    { "02_SandCanyon",        { 224, 128, -120}     },
    { "02_SandCanyon",        { -98.5f, 113.5f, 18} },
    { "04_ValleyOfTheKings",  { 0, 96, 0 }          },
    { "05_MoonMountains",     { -64, 80, -64}       },
    { "06_Oasis",             { 128, -16, 0}        },
  };

  // Positions of unnecessary brushes (that also have "World Base" names)
  dirLightsToIgnore =
  {
    { "06_Oasis", "Temple Roof" },
  };

  dynTexCoordsWldToIgnore =
  {
    "12_Karnak"
  };

  const char *texToIgnore = "LightBeam.tex";

  const auto &wldName = pWorld->wo_fnmFileName.FileName();
  bool found = false;

  for (const auto &s : RT_WorldCullingMaxSectorDepth)
  {
    if (wldName == s.pWorldName)
    {
      _srtGlobals.srt_iCullingMaxSectorDepth = s.iCullingMaxSectorDepth;

      found = true;
      break;
    }
  }

  if (!found)
  {
    _srtGlobals.srt_iCullingMaxSectorDepth = 8;
  }
}

SSRT::CustomInfo::~CustomInfo()
{}

bool SSRT::CustomInfo::IsWaterTexture(CTextureData *ptd) const
{
  if (ptd == nullptr)
  {
    return false;
  }

  // if it's water effect texture, then it's true
  if (ptd->td_ptegEffect != nullptr && ptd->td_ptegEffect->IsWater())
  {
    return true;
  }

  return IsTextureNameIn(ptd, waterTextureNames);
}

bool SSRT::CustomInfo::IsFireTexture(CTextureData *ptd) const
{
  if (ptd == nullptr)
  {
    return false;
  }

  // assume that fire texture is always an effect texture
  if (ptd->td_ptegEffect == nullptr)
  {
    return false;
  }

  // must not be water
  if (ptd->td_ptegEffect->IsWater())
  {
    return false;
  }

  return IsTextureNameIn(ptd, fireTextureNames);
}

bool SSRT::CustomInfo::HasModelFireTexture(CEntity *penModel) const
{
  if (penModel->en_RenderType != CEntity::RT_MODEL)
  {
    return false;
  }

  // when editor light sources are enabled, lights from fire textures 
  // will be added, as they present in a world
  if (!_srtGlobals.srt_bLightSphericalIgnoreEditorModels)
  {
    return false;
  }

  CModelObject *pmoModelObject = penModel->GetModelForRendering();

  if (pmoModelObject == nullptr)
  {
    return false;
  }

  auto *td = (CTextureData *)pmoModelObject->mo_toTexture.GetData();
  return IsFireTexture(td);
}

bool SSRT::CustomInfo::IsBrushIgnored(CEntity *penBrush) const
{ 
  // ugly way to remove terrain planes above the world
  if (!(penBrush->en_ulFlags & ENF_ZONING) && penBrush->GetName() == "World Base")
  {
    // check if it's exactly those unnecessary polygons
    for (const auto &ignore : worldBaseToIgnore)
    {
      // if not for the current world
      if (penBrush->GetWorld() == nullptr || penBrush->GetWorld()->wo_fnmFileName.FileName() != ignore.worldName.c_str())
      {
        continue;
      }

      const FLOAT3D &p = penBrush->GetPlacement().pl_PositionVector;

      bool isInside = (p - ignore.position).ManhattanNorm() < WORLD_BASE_FORCE_INVISIBLE_EPSILON;

      if (isInside)
      {
        return true;
      }
    }
  }

  return false;
}

bool SSRT::CustomInfo::IsDirectionalLightIgnored(const CLightSource *plsLight) const
{
  CEntity *pen = plsLight->ls_penEntity;

  if (pen == nullptr || pen->GetWorld() == nullptr)
  {
    return false;
  }

  const auto &wldName = pen->GetWorld()->wo_fnmFileName.FileName();
  const auto &ltName = plsLight->ls_penEntity->GetName();

  for (const auto &ignore : dirLightsToIgnore)
  {
    if (wldName == ignore.worldName.c_str() && ltName == ignore.entityName.c_str())
    {
      return true;
    }
  }

  return false;
}

bool SSRT::CustomInfo::IsSphericalLightIgnored(const CLightSource *plsLight) const
{
  //UBYTE h, s, v;
  //ColorToHSV(plsLight->GetLightColor(), h, s, v);

  //// ignore dim lights
  //if (h < srt_iLightSphericalHSVThresholdHLower ||
  //    h > srt_iLightSphericalHSVThresholdHUpper ||
  //    v < srt_iLightSphericalHSVThresholdVLower ||
  //    v > srt_iLightSphericalHSVThresholdVUpper)
  //{
  //  return true;
  //}

  CEntity *pen = plsLight->ls_penEntity;

  if (HasLightEntityVertices(pen))
  {
    return false;
  }

  if (HasLightEntityVertices(pen->GetParent()))
  {
    return false;
  }

  {
    FOREACHINLIST(CEntity, en_lnInParent, pen->en_lhChildren, itenChild)
    {
      if (HasLightEntityVertices(itenChild))
      {
        return false;
      }
    }
  }

  return true;
}

bool SSRT::CustomInfo::AreDynamicTexCoordsIgnored(CEntity *penBrush) const
{
  if (_srtGlobals.srt_bIgnoreDynamicTexCoords || penBrush->GetWorld() == nullptr)
  {
    return true;
  }

  const auto &wldName = penBrush->GetWorld()->wo_fnmFileName.FileName();

  for (const auto &ignore : dynTexCoordsWldToIgnore)
  {
    if (wldName == ignore.c_str())
    {
      return true;
    }
  }

  return false;
}

bool SSRT::CustomInfo::IsTextureNameIn(CTextureData *ptd, const std::vector<std::string> &collection)
{
  for (const auto &name : collection)
  {
    if (ptd->GetName().FileName() == name.c_str())
    {
      return true;
    }
  }

  return false;
}

bool SSRT::CustomInfo::HasLightEntityVertices(CEntity *pen) const
{
  if (pen == nullptr)
  {
    return false;
  }

  if (pen->GetRenderType() != CEntity::RT_MODEL)
  {
    if (pen->GetRenderType() != CEntity::RT_EDITORMODEL)
    {
      return false;
    }

    if (_srtGlobals.srt_bLightSphericalIgnoreEditorModels)
    {
      return false;
    }
  }

  CModelData *md = pen->GetModelObject()->GetData();

  return md != nullptr && md->md_VerticesCt > 0;
}
