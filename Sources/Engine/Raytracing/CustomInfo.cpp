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

#include <Engine/Base/Shell.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Entities/Entity.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/Models/ModelData.h>
#include <Engine/Light/LightSource.h>
#include <Engine/Graphics/TextureEffects.h>
#include <Engine/Raytracing/SSRTGlobals.h>
#include <Engine/Raytracing/Scene.h>

#include <Engine/Templates/Stock_CtextureData.h>


#include <Engine/Base/ListIterator.inl>


extern SSRT::SSRTGlobals _srtGlobals;


enum class EWorld
{
  Other,
  Hatshepsut,
  SandCanyon,       
  TombOfRamses,    
  ValleyOfTheKings,
  MoonMountains,  
  Oasis,           
  Dunes,            
  Suburbs,        
  Sewers,        
  Metropolis,      
  AlleyOfSphinxes,
  Karnak,            
  Luxor,            
  SacredYards,      
  TheGreatPyramid,  
};



static struct
{
  const char  *pName;
  EWorld      eWorld;
}
RT_WorldNameToEnum[] =
{
  { "01_Hatshepsut",        EWorld::Hatshepsut        },
  { "02_SandCanyon",        EWorld::SandCanyon        },
  { "03_TombOfRamses",      EWorld::TombOfRamses      },
  { "04_ValleyOfTheKings",  EWorld::ValleyOfTheKings  },
  { "05_MoonMountains",     EWorld::MoonMountains     },
  { "06_Oasis",             EWorld::Oasis             },
  { "07_Dunes",             EWorld::Dunes             },
  { "08_Suburbs",           EWorld::Suburbs           },
  { "09_Sewers",            EWorld::Sewers            },
  { "10_Metropolis",        EWorld::Metropolis        },
  { "11_AlleyOfSphinxes",   EWorld::AlleyOfSphinxes   },
  { "12_Karnak",            EWorld::Karnak            },
  { "13_Luxor",             EWorld::Luxor             },
  { "14_SacredYards",       EWorld::SacredYards       },
  { "15_TheGreatPyramid",   EWorld::TheGreatPyramid   },
};


static EWorld GetWorldEnum(CWorld *pWorld)
{
  const auto &wldName = pWorld->wo_fnmFileName.FileName();

  for (const auto &n : RT_WorldNameToEnum)
  {
    if (n.pName == wldName)
    {
      return n.eWorld;
    }
  }
  
  return EWorld::Other;
}


static struct 
{
  EWorld  eWorld;
  INDEX   iCullingMaxSectorDepth;
  bool    bIgnoreDynamicTexCoords;
} 
RT_WorldParams[] =
{
  { EWorld::Hatshepsut,        4, false },
  { EWorld::SandCanyon,        8, false },
  { EWorld::TombOfRamses,      7, false },
  { EWorld::ValleyOfTheKings,  5, false },
  { EWorld::MoonMountains,     4, false },
  { EWorld::Oasis,             4, false },
  { EWorld::Dunes,             8, false },
  { EWorld::Suburbs,           2, false },
  { EWorld::Sewers,            4, false },
  { EWorld::Metropolis,        5, false },
  { EWorld::AlleyOfSphinxes,   3, false },
  { EWorld::Karnak,            5, true  },
  { EWorld::Luxor,             4, false },
  { EWorld::SacredYards,       4, false },
  { EWorld::TheGreatPyramid,   5, false },
};


constexpr float WORLD_BASE_FORCE_INVISIBLE_EPSILON = 0.5f;

static struct
{
  EWorld eWorld;
  FLOAT3D vPosition;
}
RT_WorldBaseToIgnore[] =
{
  { EWorld::Hatshepsut,       { 0,        1152,   64 } },
  { EWorld::SandCanyon,       { 224,       128, -312 } },
  { EWorld::SandCanyon,       { 224,       128, -120 } },
  { EWorld::SandCanyon,       { -98.5f, 113.5f,   18 } },
  { EWorld::ValleyOfTheKings, { 0,          96,    0 } },
  { EWorld::MoonMountains,    { -64,        80,  -64 } },
  { EWorld::Oasis,            { 128,       -16,    0 } },
};


static struct
{
  EWorld      eWorld;
  const char  *pEntityName;
}
RT_DirectionalLightsToIgnore[] =
{
  { EWorld::Oasis, "Temple Roof" },
};


const char * const RT_WaterTextureNames[] =
{
  "WaterBase",
  "WaterFX",
  "WaterFall01",
  "WaterFall02",
};


const char * const RT_FireTextureNames[] =
{
  "Fire",
};


// bullet holes has small angular size, but they're important
const char * const RT_BulletHoleTexturePaths[] =
{
  "Models\\Effects\\BulletOnTheWall\\Bullet.tex",
  "Models\\Effects\\BulletOnTheWall\\BulletSand.tex",
};


const char * const RT_TextureForcedReflectiveDirectories[] =
{
  "Models\\Effects\\BloodOnTheWall01\\",
  "Models\\Effects\\BloodPatches01\\",
};


const char * const RT_TextureForcedAlphaTestDirectories[] =
{
  "Models\\Effects\\BloodOnTheWall01\\",
  "Models\\Effects\\BloodPatches01\\",
  "Models\\Effects\\BulletOnTheWall\\",
  "Models\\Plants\\Garden02\\",
};


SSRT::CustomInfo::CustomInfo(CWorld *pWorld)
{
  ASSERT(pWorld != nullptr);

  EWorld eCurrentWorld = GetWorldEnum(pWorld);
  TIME tmWorldCreation = 0;


  // Positions of unnecessary brushes (that also have "World Base" names)
  for (const auto &b : RT_WorldBaseToIgnore)
  {
    if (b.eWorld == eCurrentWorld)
    {
      worldBasePositionsToIgnore.push_back(b.vPosition);
    }
  }

  // Positions of unnecessary brushes (that also have "World Base" names)
  for (const auto &d : RT_DirectionalLightsToIgnore)
  {
    if (d.eWorld == eCurrentWorld)
    {
      dirLightsNamesToIgnore.push_back(d.pEntityName);
    }
  }

  _srtGlobals.srt_iCullingMaxSectorDepth = 8;
  _srtGlobals.srt_bIgnoreDynamicTexCoords = false;

  for (const auto &s : RT_WorldParams)
  {
    if (s.eWorld == eCurrentWorld)
    {
      _srtGlobals.srt_iCullingMaxSectorDepth = s.iCullingMaxSectorDepth;
      _srtGlobals.srt_bIgnoreDynamicTexCoords = s.bIgnoreDynamicTexCoords;
    }
  }

  for (const char *pTdPath : RT_BulletHoleTexturePaths)
  {
     ptdCachedBulletHoleTextures.push_back(_pTextureStock->st_ntObjects.Find(pTdPath));
  }


  // always disable flashlight on level start
  _srtGlobals.srt_bFlashlightEnable = false;

  isFlashlightHintEnabled = true;
  DisableFlashlightHint();

  // enable hint onlt for particular levels
  switch (eCurrentWorld)
  {
    case EWorld::SandCanyon:
    case EWorld::TombOfRamses:
    case EWorld::ValleyOfTheKings:
    case EWorld::Sewers:
      tmFlashlightHintStart = tmWorldCreation + 1.0f;
      tmFlashlightHintEnd = tmFlashlightHintStart + 5.0f;
      break;
    default:
      tmFlashlightHintStart = -1.0f;
      tmFlashlightHintEnd = -1.0f;
      break;
  }


  tmAnimatedSunOrigin = tmWorldCreation;

  switch (eCurrentWorld)
  {
    case EWorld::Hatshepsut:
      _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 6 * 60;
      _srtGlobals.srt_fAnimatedSunTimeLength = 2 * 60;
      _srtGlobals.srt_vAnimatedSunTargetEuler = { 35, -45, 0 };
      break;
    case EWorld::Oasis:
      _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 60;
      _srtGlobals.srt_fAnimatedSunTimeLength = 4 * 60;
      _srtGlobals.srt_vAnimatedSunTargetEuler = { -27, -70, 0 };
      break;
    default:
      _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 0;
      // disable animation
      _srtGlobals.srt_fAnimatedSunTimeLength = -1;
      _srtGlobals.srt_vAnimatedSunTargetEuler = { 45, -45, 0 };
      break;
  }


  // Hatshepsut level has clouds that should obscure the sun
  bSunIntensityDependsOnSkyClouds = eCurrentWorld == EWorld::Hatshepsut;
  fSkyCloudsOpacity = 0.0f;
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

  return IsTextureNameIn(ptd, RT_WaterTextureNames, ARRAYCOUNT(RT_WaterTextureNames));
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

  return IsTextureNameIn(ptd, RT_FireTextureNames, ARRAYCOUNT(RT_FireTextureNames));
}

bool SSRT::CustomInfo::HasModelFireTexture(CEntity *penModel) const
{
  if (penModel->en_RenderType != CEntity::RT_MODEL)
  {
    return false;
  }

  // when editor light sources are enabled, lights from fire textures 
  // will be added, as they present in a world
  if (!_srtGlobals.srt_bLightSphIgnoreEditorModels)
  {
    return false;
  }

  CModelObject *pmoModelObject = penModel->GetModelForRendering();

  if (pmoModelObject == nullptr)
  {
    return false;
  }

  auto *ptd = (CTextureData *)pmoModelObject->mo_toTexture.GetData();
  return IsFireTexture(ptd);
}

bool SSRT::CustomInfo::IsAngularSizeCullingDisabled(CEntity *penModel) const
{
  if (penModel->en_RenderType != CEntity::RT_MODEL)
  {
    return false;
  }

  CModelObject *pmoModelObject = penModel->GetModelForRendering();

  if (pmoModelObject == nullptr)
  {
    return false;
  }

  auto *ptd = (CTextureData *)pmoModelObject->mo_toTexture.GetData();

  if (ptd == nullptr || ptd->td_ctFrames != 1 || ptd->td_ptegEffect != nullptr)
  {
    return false;
  }

  // if found in cache
  return std::find(ptdCachedBulletHoleTextures.begin(), ptdCachedBulletHoleTextures.end(), ptd) != ptdCachedBulletHoleTextures.end();
}

bool SSRT::CustomInfo::IsBrushIgnored(CEntity *penBrush) const
{ 
  // ugly way to remove terrain planes above the world
  if (!(penBrush->en_ulFlags & ENF_ZONING) && penBrush->GetName() == "World Base")
  {
    // check if it's exactly those unnecessary polygons
    for (const auto &vIgnorePos : worldBasePositionsToIgnore)
    {
      const FLOAT3D &p = penBrush->GetPlacement().pl_PositionVector;

      bool isInside = (p - vIgnorePos).ManhattanNorm() < WORLD_BASE_FORCE_INVISIBLE_EPSILON;

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

  const auto &ltName = plsLight->ls_penEntity->GetName();

  for (const auto &strEntityName : dirLightsNamesToIgnore)
  {
    if (ltName == strEntityName.c_str())
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

template<class Type>
inline Type SmoothStep(const Type edge0, const Type edge1, const Type x)
{
  Type t = Clamp((x - edge0) / (edge1 - edge0), (Type)0.0, (Type)1.0);
  return t * t * (3.0 - 2.0 * t);
}

FLOAT3D SSRT::CustomInfo::GetAnimatedSunDirection(const FLOAT3D &vOriginalEuler) const
{
  TIME tmCurrent = _pTimer->GetLerpedCurrentTick();

  if (_srtGlobals.srt_fAnimatedSunTimeLength <= 0.0f)
  {
    FLOAT3D vDirection;
    AnglesToDirectionVector(vOriginalEuler, vDirection);
  
    return vDirection;
  }

  FLOATquat3D qOrigin;
  qOrigin.FromEuler(vOriginalEuler);

  FLOATquat3D qTarget;
  qTarget.FromEuler(_srtGlobals.srt_vAnimatedSunTargetEuler);

  FLOAT t = SmoothStep(tmAnimatedSunOrigin + _srtGlobals.srt_fAnimatedSunTimeOffsetStart, 
                       tmAnimatedSunOrigin + _srtGlobals.srt_fAnimatedSunTimeOffsetStart + _srtGlobals.srt_fAnimatedSunTimeLength,
                       tmCurrent);

  FLOATquat3D qLerped = Slerp(t, qOrigin, qTarget);

  FLOATmatrix3D mLerped;
  qLerped.ToMatrix(mLerped);

  return FLOAT3D(0, 0, -1) * mLerped;
}

FLOAT SSRT::CustomInfo::GetCloudsOpacity() const
{
  return bSunIntensityDependsOnSkyClouds ? fSkyCloudsOpacity : 0.0f;
}

bool SSRT::CustomInfo::AreDynamicTexCoordsIgnored(CEntity *penBrush) const
{
  return _srtGlobals.srt_bIgnoreDynamicTexCoords;
}

bool SSRT::CustomInfo::IsReflectiveForced(CTextureObject *pTo) const
{
  if (pTo != nullptr)
  {
    CTFileName dir = pTo->GetName().FileDir();

    for (const char *pNm : RT_TextureForcedReflectiveDirectories)
    {
      if (dir == pNm)
      {
        return true;
      }
    }
  }

  return false;
}

bool SSRT::CustomInfo::IsAlphaTestForced(CTextureObject *pTo, bool isTranslucent) const
{
  if (isTranslucent && pTo != nullptr)
  {
    CTFileName dir = pTo->GetName().FileDir();

    for (const char *pNm : RT_TextureForcedAlphaTestDirectories)
    {
      if (dir == pNm)
      {
        return true;
      }
    }
  }

  return false;
}

bool SSRT::CustomInfo::IsTextureNameIn(CTextureData *ptd, const char * const pCollection[], uint32_t iCount)
{
  if (ptd == nullptr)
  {
    return false;
  }

  for (uint32_t i = 0; i < iCount; i++)
  {
    if (ptd->GetName().FileName() == pCollection[i])
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

    if (_srtGlobals.srt_bLightSphIgnoreEditorModels)
    {
      return false;
    }
  }

  CModelData *md = pen->GetModelObject()->GetData();

  return md != nullptr && md->md_VerticesCt > 0;
}

void SSRT::CustomInfo::Update()
{
  TIME tmCurrent = _pTimer->GetLerpedCurrentTick();


  // if flashlight was enabled, don't show the hint anymore
  if (_srtGlobals.srt_bFlashlightEnable)
  {
    tmFlashlightHintStart = -1.0;
    tmFlashlightHintEnd = -1.0;
  }

  if (tmCurrent >= tmFlashlightHintStart && tmCurrent <= tmFlashlightHintEnd)
  {
    EnableFlashlightHint();
  }
  else
  {
    DisableFlashlightHint();
  }


  if (_srtGlobals.srt_bAnimatedSunRestart)
  {
    tmAnimatedSunOrigin = tmCurrent;
    _srtGlobals.srt_bAnimatedSunRestart = 0;
  }
}

void SSRT::CustomInfo::EnableFlashlightHint()
{
  if (!isFlashlightHintEnabled)
  {
    _pShell->SetINDEX("hud_bShowFlashlightHint", 1);
    isFlashlightHintEnabled = true;
  }
}

void SSRT::CustomInfo::DisableFlashlightHint()
{
  if (isFlashlightHintEnabled)
  {
    _pShell->SetINDEX("hud_bShowFlashlightHint", 0);
    isFlashlightHintEnabled = false;
  }
}

void SSRT::CustomInfo::OnSkyBrushAdd(const SSRT::CBrushGeometry &brush)
{
  ASSERT(brush.isSky);

  if (!bSunIntensityDependsOnSkyClouds)
  {
    return;
  }

  const uint32_t iCloudsLayer = 1;

  // hardcoded way to determine cloud opacity
  if (brush.textures[iCloudsLayer] != nullptr && brush.textures[iCloudsLayer]->GetName().FileName() == "Storm5")
  {
    // get alpha value
    fSkyCloudsOpacity = Clamp(brush.layerColors[iCloudsLayer](4), 0.0f, 1.0f);
  }
}