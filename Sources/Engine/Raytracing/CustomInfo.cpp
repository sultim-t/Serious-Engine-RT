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
#include <Engine/Models/RenderModel.h>
#include <Engine/Light/LightSource.h>
#include <Engine/Graphics/TextureEffects.h>
#include <Engine/Raytracing/SSRTGlobals.h>
#include <Engine/Raytracing/Scene.h>
#include <Engine/World/WorldSettings.h>

#include <Engine/Templates/Stock_CtextureData.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include <Engine/Base/ListIterator.inl>


extern SSRT::SSRTGlobals _srtGlobals;


typedef SSRT::CustomInfo::EWorld EWorld;


const struct
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


constexpr FLOAT GREAT_PYRAMID_SKY_COLOR_START = 1.0f;
constexpr FLOAT GREAT_PYRAMID_SKY_COLOR_END = 0.25f;
constexpr FLOAT GREAT_PYRAMID_SKY_COLOR_CHANGE_LENGTH = 30.0f;
constexpr FLOAT GREAT_PYRAMID_SUN_INTENSITY_DEFAULT = 2.0f;
constexpr FLOAT GREAT_PYRAMID_SUN_INTENSITY_BOSS_FIGHT = 0.0f;
constexpr FLOAT GREAT_PYRAMID_SKY_COLOR_BOSS_FIGHT = 5.0f;
constexpr FLOAT GREAT_PYRAMID_FIGHT_TONEMAPPING_MIN_LOG = -2;

const struct
{
  EWorld  eWorld;
  FLOAT                     fSunIntensity;
  FLOAT                             fSunSaturation;
  FLOAT                                   fSkyColorMultiplier;
  FLOAT                                         fSkyColorSaturation;
  FLOAT                                                 fTonemappingMinLogLuminance;
  FLOAT                                                         fTonemappingMaxLogLuminance;
  FLOAT                                                               fPotentialLightSphFalloffDefault;
  FLOAT                                                                     fPotentialLightSphFalloffMultiplier;
  FLOAT                                                                         fOriginalLightSphFalloffMultiplier;
  FLOAT                                                                             fPotentialLightSphFalloffMin;
}
RT_WorldIlluminationParams[] =
{
  // defaults:                4,    0.5f, 1,    0.75f,  -2,     0,    1,    1,  1,  5 
  { EWorld::SandCanyon,       3,    0.5f, 1,    0.75f,  -2,     0,    8,    1,  1,  5 },
  { EWorld::ValleyOfTheKings, 3,    0.5f, 1,    0.75f,  -2,     0,    8,    1,  2,  5 },
  { EWorld::Suburbs,          4,    0.5f, 0.5f, 1,      -2,     0,    1,    1,  1,  5 },
  { EWorld::Sewers,           4,    0.5f, 1,    0.75f,  -2,     0,    1,    1,  2,  5 },
  { EWorld::AlleyOfSphinxes,  4,    0.5f, 0.5f, 0.75f,  -2,     0,    1,    1,  1,  5 },
  { EWorld::Metropolis,       4,    0.5f, 0.5f, 0.75f,  -2,     0,    1,    1,  1,  5 },
  { EWorld::Luxor,            4,    0.5f, 1,    0.75f,  -2,     0,    4,    1,  1,  5 },
  { EWorld::Karnak,           4,    0.9f, 0.5f, 1,      -2,     0,    1, 0.5f,  1,  7 },
  { EWorld::SacredYards,      4,    0.5f, 0.5f, 0.75f,  -2,     0,    1,    1,  1,  5  },
  { EWorld::TheGreatPyramid,  GREAT_PYRAMID_SUN_INTENSITY_DEFAULT, 
                                       1, 0.5f, 1,       0,     2,    1,    1,  1,  5 },
};


const struct
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
  { EWorld::Karnak,            5,  true  },
  { EWorld::Luxor,             4, false },
  { EWorld::SacredYards,       4, false },
  { EWorld::TheGreatPyramid,   5, false },
};


constexpr float WORLD_BASE_FORCE_INVISIBLE_EPSILON = 0.5f;

const struct
{
  EWorld eWorld;
  FLOAT3D vPosition;
  bool canBeZoning;
}
RT_WorldBaseToIgnore[] =
{
  { EWorld::Hatshepsut,       {      0,   1152,   64 }, false },
  { EWorld::SandCanyon,       {    224,    128, -312 }, false },
  { EWorld::SandCanyon,       {    224,    128, -120 }, false },
  { EWorld::SandCanyon,       { -98.5f, 113.5f,   18 }, false },
  { EWorld::ValleyOfTheKings, {      0,     96,    0 }, false },
  { EWorld::ValleyOfTheKings, {    190, -69.5f, -420 }, true },
  { EWorld::MoonMountains,    {    -64,     80,  -64 }, false },
  { EWorld::Oasis,            {    128,    -16,    0 }, false },
};


const struct
{
  EWorld      eWorld;
  const char  *pEntityName;
}
RT_DirectionalLightsToIgnore[] =
{
  { EWorld::Oasis, "Temple Roof" },
};


const char * const RT_TexturePaths_Water[] =
{
  "Textures\\Effects\\Water\\Water07\\WaterBase.tex",
  "Textures\\Effects\\Water\\Water07\\WaterFX.tex",
  "Textures\\Effects\\Water\\Water08\\WaterBase.tex",
  "Textures\\Effects\\Water\\Water08\\WaterFX.tex",
  "Textures\\Effects\\WaterFall\\WaterFall01.tex",
  "Textures\\Effects\\WaterFall\\WaterFall02.tex",
};


const char * const RT_TexturePaths_Fire[] =
{
  "Models\\Effects\\Fire03\\Fire.tex",
};


const char * const RT_TexturePaths_DisabledCulling[] =
{
  // bullet holes have small angular size, but they're important
  "Models\\Effects\\BulletOnTheWall\\Bullet.tex",
  "Models\\Effects\\BulletOnTheWall\\BulletSand.tex",
};


const char * const RT_TexturePaths_FullyMetallic[] =
{
  "Models\\Effects\\BloodOnTheWall01\\BloodSpill02.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodSpill05.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodSpill06.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain01.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain02.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain03.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain04.tex",
  "Models\\Effects\\BloodPatches01\\Blood01.tex",
};


const char * const RT_TexturePaths_AlphaTest[] =
{
  "Models\\Effects\\BloodOnTheWall01\\BloodSpill02.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodSpill05.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodSpill06.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain01.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain02.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain03.tex",
  "Models\\Effects\\BloodOnTheWall01\\BloodStain04.tex",
  "Models\\Effects\\BloodPatches01\\Blood01.tex",
  "Models\\Effects\\BulletOnTheWall\\Bullet.tex",
  "Models\\Effects\\BulletOnTheWall\\BulletSand.tex",
  "Models\\Plants\\Garden02\\Garden06.tex",
  "Models\\Ages\\Egypt\\Gods\\TothMonkey\\TothMonkey.tex",
  "Models\\Items\\Ammo\\Electricity\\Effect.tex",
  "Models\\CutSequences\\Screens\\Screen1.tex",
  "Models\\CutSequences\\Screens\\Screen2.tex",
  "Models\\CutSequences\\Screens\\Screen3 .tex",
};


const char *const RT_TexturePaths_NoEffectOnTexture[] =
{
  "Models\\Items\\Ammo\\Electricity\\Effect.tex",
};


const char *const RT_TexturePaths_AlphaTest_Particles[] =
{
  "Textures\\Effects\\Particles\\Lightning.tex",
  "Models\\Weapons\\GhostBuster\\Projectile\\Ray.tex",
};


const char *const RT_TexturePaths_Emission[] =
{
  "Textures\\Levels\\GreatPyramid\\LiftHolders03.tex",
  "Models\\CutSequences\\Pyramid\\RingBaseFX.tex",
  "Models\\CutSequences\\Pyramid\\RingBase.tex",
  "Models\\CutSequences\\Pyramid\\RingLightningFX.tex",
};


const char *const RT_TexturePaths_Glass[] =
{
  "Models\\Items\\Health\\Small\\Small.tex",
  "Models\\Items\\Health\\Medium\\Medium.tex",
  "Models\\Items\\Keys\\Elements\\Air.tex",
  "Models\\Items\\Keys\\Elements\\Fire.tex",
  "Models\\Items\\Keys\\Elements\\Texture.tex",
  "Models\\Items\\Keys\\Elements\\Water.tex",
  "Models\\Items\\Ammo\\Electricity\\Electricity.tex",
};


/*
const char *const RT_TexturePaths_Portal[] =
{
  // "Textures\\Effects\\Effect01\\Effect.tex",
  "Textures\\Ages\\Egypt\\Patterns\\Glyphs02.tex",
};
*/


const char *const RT_TexturePaths_Mirror[] =
{
  "Models\\IHVTest\\ReflectionMap\\teapot_env.tex",
  "Models\\IHVTest\\KarnakDemoReflectionMap01\\ItemRoomGravitySecret.tex"
};


const char *const RT_TexturePaths_CalcNormals[] =
{
  "Models\\Items\\Health\\Pill\\Pill.tex",
  "Models\\Items\\Ammo\\Bullets\\Bullets.tex",
  "Models\\Items\\Ammo\\Shells\\Shells.tex",
  "Models\\CutSequences\\Portal\\Base.tex",
  "Models\\CutSequences\\Portal\\Portal.tex",
  "Models\\Weapons\\Hand.tex",
};


// TODO: ignore Amon model with/without reflection texture
const char *const RT_TexturePaths_IgnoreModel[] =
{
  "Models\\Ages\\Egypt\\Gods\\Amon\\Amon.tex",
};


const char *const RT_TexturePaths_ClampWrap[] =
{
  "Textures\\Levels\\Hatshepsut\\ColumnArtwork01.tex",
  "Textures\\Levels\\ChambersOfHorus\\EyeOfRa02.tex",
  "Textures\\Levels\\AmonComplex\\EyeOfRa.tex",
};


const char *const RT_TexturePaths_DisabledOverride[] =
{
  "Textures\\Levels\\GreatPyramid\\FloorPyramod01.tex",
};


const char *const RT_TexturePaths_LightOffsetFix[] =
{
  "Models\\Effects\\ExplosionGrenade\\Texture.tex",
  "Models\\Effects\\ExplosionParticles\\Texture.tex",
  "Models\\Effects\\ExplosionRocket\\Texture.tex",
  "Models\\Weapons\\GrenadeLauncher\\Grenade\\Grenade.tex",
  "Models\\Effects\\ShockWaveGreen\\ShockWaveGreen.tex",
};


const char *const RT_TexturePaths_LightForceDynamic[] =
{
  // for the rocketman with light in Sand Canyon
  "Models\\Enemies\\Headman\\Rocketman.tex",
};


const char *const RT_TexturePaths_InvisibleEnemy[] =
{
  "Models\\Enemies\\Eyeman\\Eyeman4.tex",
  "Models\\Enemies\\Eyeman\\Eyeman5.tex",
};


// Ignore models with such reflection texture. It's ONLY for Great Pyramid, chrome Amon models
const char *const RT_TexturePaths_IgnoredModels[] =
{
  "Models\\Ages\\Egypt\\Gods\\Amon\\Amon.tex"
};


static bool vector_Contains(const std::vector<CTextureData*> &aVec, const CTextureData *pTd)
{
  return std::find(aVec.begin(), aVec.end(), pTd) != aVec.end();
}


SSRT::CustomInfo::CustomInfo(CWorld *pWorld)
{
  ASSERT(pWorld != nullptr);

  this->pWorld = pWorld;
  eCurrentWorld = GetWorldEnum(pWorld);
  
  // It's not the actual world start time, it's 
  // the time SSRT scene was initialized. So 
  // loading a saved game after quitting a world, 
  // will result in wrong sun animation/flashlight hint
  TIME tmWorldCreation = _pTimer->GetLerpedCurrentTick();



  // Positions of unnecessary brushes (that also have "World Base" names)
  bCheckOnlyZoningForBrushIgnored = true;
  for (const auto &b : RT_WorldBaseToIgnore)
  {
    if (b.eWorld == eCurrentWorld)
    {
      worldBasePositionsToIgnore.push_back(b.vPosition);

      bCheckOnlyZoningForBrushIgnored &= b.canBeZoning;
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



  bool isMetropolisSerious = false;

  if (eCurrentWorld == EWorld::Metropolis && pWorld->GetBackgroundViewer() != nullptr)
  {
    const FLOAT3D &vBackgroundViewer = pWorld->GetBackgroundViewer()->GetLerpedPlacement().pl_PositionVector;
    const FLOAT3D vSerious = { 3280, -96, -3280 };

    isMetropolisSerious = (vBackgroundViewer - vSerious).ManhattanNorm() < 0.5f;
  }



  {
    _srtGlobals.srt_fSunIntensity = 4.0f;
    _srtGlobals.srt_fSunSaturation = 0.5f;
    _srtGlobals.srt_fSkyColorMultiplier = 1.0f;
    _srtGlobals.srt_fSkyColorSaturation = 0.75f;
    _srtGlobals.srt_fTonemappingMinLogLuminance = -2.0f;
    _srtGlobals.srt_fTonemappingMaxLogLuminance = 0.0f;
    _srtGlobals.srt_fPotentialLightSphFalloffDefault = 1.0f;
    _srtGlobals.srt_fPotentialLightSphFalloffMultiplier = 1.0f;
    _srtGlobals.srt_fOriginalLightSphFalloffMultiplier = 1.0f;
    _srtGlobals.srt_fPotentialLightSphFalloffMin = 5.0f;
  }
  for (const auto &s : RT_WorldIlluminationParams)
  {
    if (s.eWorld == eCurrentWorld)
    {
      _srtGlobals.srt_fSunIntensity = s.fSunIntensity;
      _srtGlobals.srt_fSunSaturation = s.fSunSaturation;
      _srtGlobals.srt_fSkyColorMultiplier = s.fSkyColorMultiplier;
      _srtGlobals.srt_fSkyColorSaturation = s.fSkyColorSaturation;
      _srtGlobals.srt_fTonemappingMinLogLuminance = s.fTonemappingMinLogLuminance;
      _srtGlobals.srt_fTonemappingMaxLogLuminance = s.fTonemappingMaxLogLuminance;
      _srtGlobals.srt_fPotentialLightSphFalloffDefault = s.fPotentialLightSphFalloffDefault;
      _srtGlobals.srt_fPotentialLightSphFalloffMultiplier = s.fPotentialLightSphFalloffMultiplier;
      _srtGlobals.srt_fOriginalLightSphFalloffMultiplier = s.fOriginalLightSphFalloffMultiplier;
      _srtGlobals.srt_fPotentialLightSphFalloffMin = s.fPotentialLightSphFalloffMin;
      break;
    }
  }
  // special case: Serious difficulty in Metropolis
  if (isMetropolisSerious)
  {    
    _srtGlobals.srt_fSunIntensity = 0.75f;
    _srtGlobals.srt_fSunSaturation = 0.5f;
    _srtGlobals.srt_fSkyColorMultiplier = 0.25f;
    _srtGlobals.srt_fSkyColorSaturation = 0.75f;
  }
  // special case: Great Pyramid animated sky intensity
  {
    tmAnimatedSkyIntensityOrigin = -1.0f;

    if (eCurrentWorld == EWorld::TheGreatPyramid)
    {
      _srtGlobals.srt_fSkyColorMultiplier = GREAT_PYRAMID_SKY_COLOR_START;
    }
  }



  const struct
  {
    const char * const *pArray;
    int iArrayCount;
    std::vector<CTextureData*> *pDst;
  } 
  tdsToFind[] =
  {
    { RT_TexturePaths_DisabledCulling,            ARRAYCOUNT(RT_TexturePaths_DisabledCulling),            &ptdCachedTextures.aDisabledCulling },
    { RT_TexturePaths_AlphaTest,                  ARRAYCOUNT(RT_TexturePaths_AlphaTest),                  &ptdCachedTextures.aAlphaTest },
    { RT_TexturePaths_AlphaTest_Particles,        ARRAYCOUNT(RT_TexturePaths_AlphaTest_Particles),        &ptdCachedTextures.aAlphaTestParticles },
    { RT_TexturePaths_FullyMetallic,              ARRAYCOUNT(RT_TexturePaths_FullyMetallic),              &ptdCachedTextures.aFullyMetallic },
    { RT_TexturePaths_Water,                      ARRAYCOUNT(RT_TexturePaths_Water),                      &ptdCachedTextures.aWater },
    { RT_TexturePaths_Fire,                       ARRAYCOUNT(RT_TexturePaths_Fire),                       &ptdCachedTextures.aFire },
    { RT_TexturePaths_Emission,                   ARRAYCOUNT(RT_TexturePaths_Emission),                   &ptdCachedTextures.aEmission },
    { RT_TexturePaths_Glass,                      ARRAYCOUNT(RT_TexturePaths_Glass),                      &ptdCachedTextures.aGlass },
    { RT_TexturePaths_Mirror,                     ARRAYCOUNT(RT_TexturePaths_Mirror),                     &ptdCachedTextures.aMirror },
    { RT_TexturePaths_CalcNormals,                ARRAYCOUNT(RT_TexturePaths_CalcNormals),                &ptdCachedTextures.aCalcNormals },
    { RT_TexturePaths_ClampWrap,                  ARRAYCOUNT(RT_TexturePaths_ClampWrap),                  &ptdCachedTextures.aClampWrap },
    { RT_TexturePaths_DisabledOverride,           ARRAYCOUNT(RT_TexturePaths_DisabledOverride),           &ptdCachedTextures.aDisabledOverride },
    { RT_TexturePaths_LightOffsetFix,             ARRAYCOUNT(RT_TexturePaths_LightOffsetFix),             &ptdCachedTextures.aLightOffsetFix },
    { RT_TexturePaths_LightForceDynamic,          ARRAYCOUNT(RT_TexturePaths_LightForceDynamic),          &ptdCachedTextures.aLightForceDynamic },
    { RT_TexturePaths_InvisibleEnemy,             ARRAYCOUNT(RT_TexturePaths_InvisibleEnemy),             &ptdCachedTextures.aInvisibleEnemy },
    { RT_TexturePaths_NoEffectOnTexture,          ARRAYCOUNT(RT_TexturePaths_NoEffectOnTexture),          &ptdCachedTextures.aNoEffectOnTexture },
    { RT_TexturePaths_IgnoredModels,              ARRAYCOUNT(RT_TexturePaths_IgnoredModels),              &ptdCachedTextures.aIgnoreModel },
  };

  for (const auto &s : tdsToFind)
  {
    for (int i = 0; i < s.iArrayCount; i++)
    {
      const char *pTdPath = s.pArray[i];

      s.pDst->push_back(_pTextureStock->st_ntObjects.Find(pTdPath));
    }
  }



  // always disable flashlight on level start
  _srtGlobals.srt_bFlashlightEnable = false;
  _srtGlobals.srt_fFlashlightAngleOuter = 15;
  _srtGlobals.srt_fFlashlightFalloffDistance = 50;

  isFlashlightHintEnabled = true;
  DisableFlashlightHint();

  // enable hint only for particular levels
  switch (eCurrentWorld)
  {
    case EWorld::SandCanyon:
    case EWorld::TombOfRamses:
    case EWorld::ValleyOfTheKings:
      tmFlashlightHintStart = tmWorldCreation + 1.0f;
      tmFlashlightHintEnd = tmFlashlightHintStart + 5.0f;
      break;
    case EWorld::Sewers:
      tmFlashlightHintStart = tmWorldCreation + 1.0f;
      // a bit longer, because of cutscene
      tmFlashlightHintEnd = tmFlashlightHintStart + 10.0f;
      break;
    default:
      tmFlashlightHintStart = -1.0f;
      tmFlashlightHintEnd = -1.0f;
      break;
  }
  fFlashlightIntensityForLevel = 1.0f;

  if (eCurrentWorld == EWorld::Sewers)
  {
    // a lot wider angle, because the level is too dark
    _srtGlobals.srt_fFlashlightAngleOuter = 35;
  }
  else if (eCurrentWorld == EWorld::TheGreatPyramid)
  {
    // too dark during spaceship's shadow
    _srtGlobals.srt_fFlashlightAngleOuter = 30;
    _srtGlobals.srt_fFlashlightFalloffDistance = 150;
  }



  tmAnimatedSunOrigin = tmWorldCreation;

  {
    _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 0;
    // disable animation (length=-1)
    _srtGlobals.srt_fAnimatedSunTimeLength = -1;
    _srtGlobals.srt_vAnimatedSunTargetEuler = { 45, -45, 0 };
  }
  switch (eCurrentWorld)
  {
    case EWorld::Hatshepsut:
      _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 3 * 60;
      _srtGlobals.srt_fAnimatedSunTimeLength = 2 * 60;
      _srtGlobals.srt_vAnimatedSunTargetEuler = { 35, -45, 0 };
      break;
    case EWorld::AlleyOfSphinxes:
      _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 0;
      _srtGlobals.srt_fAnimatedSunTimeLength = 0;
      _srtGlobals.srt_vAnimatedSunTargetEuler = { 45, -15, 0 };
      break;
    case EWorld::Oasis:
      _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 60;
      _srtGlobals.srt_fAnimatedSunTimeLength = 4 * 60;
      _srtGlobals.srt_vAnimatedSunTargetEuler = { -27, -70, 0 };
      break;
    case EWorld::TheGreatPyramid:
      _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 0;
      _srtGlobals.srt_fAnimatedSunTimeLength = 0;
      _srtGlobals.srt_vAnimatedSunTargetEuler = { -135, -45, 0 };
      break;
    case EWorld::Metropolis:
      if (isMetropolisSerious)
      {
        _srtGlobals.srt_fAnimatedSunTimeOffsetStart = 0;
        _srtGlobals.srt_fAnimatedSunTimeLength = 0;
        _srtGlobals.srt_vAnimatedSunTargetEuler = { -125, -55, 0 };
      }
      break;
  }


  // Hatshepsut level has clouds that should obscure the sun
  bSunIntensityDependsOnSkyClouds = eCurrentWorld == EWorld::Hatshepsut;
  fSkyCloudsOpacity = 0.0f;
  // lower sky intensity while it's cloudy
  bSkyboxIntensityDependsOnSkyClouds = eCurrentWorld == EWorld::Hatshepsut;
  fSkyboxCloudyIntensity = 0.05f;


  switch (eCurrentWorld)
  {
    case EWorld::Hatshepsut:
      brushPolygonsToIgnore =
      {
        // water
        { 572 },
      };
      brushSectorsToIgnore = 
      {
        // black box outside
        { 3 },
      };
      break;

    case EWorld::ValleyOfTheKings:
      brushPolygonsToIgnore =
      {
        // water
        { 991 },
        { 2764 },
        { 2712 },
        // bars
        { 2072 },
        { 2255 },
        { 6439 },
      };

      brushPolygonRangesToMask =
      {
        { 817, 832, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 781, 798, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 744, 753, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 712, 719, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
      };

      brushSectorsToMask =
      {
        // hide terrain part when inside (in the area with pool and tower)
        { 34, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_2 }
      };

      brushSectorsToIgnore =
      {
        { 566 }
      };

      break;

    case EWorld::MoonMountains:
      brushPolygonsToIgnore =
      {       
        // water
        { 970 }
      };

      brushPolygonRangesToIgnore =
      {
        { 1471, 1494 },
        // water
        { 1316, 1328 }
      };

      brushSectorsToMask =
      {
        // start terrain
        { 0, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        // cave in the middle
        { 6, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 14, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 15, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 16, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        // black pyramid
        { 23, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_2 },
      };
      break;

    case EWorld::Oasis:
      brushSectorsToIgnore =
      {
        // black boxes outside
        { 67 },
        { 68 },
      };

      brushPolygonsToIgnore =
      {
        // water
        { 1577 },
        { 1580 },
      };
      brushPolygonRangesToIgnore =
      {
        // water
        { 652, 663 },
        { 672, 676 },
        { 1241, 1242 },
        { 1254, 1267 },
        { 1269, 1294 },
        { 1500, 1504 },
        { 1578, 1579 },
        { 1561, 1572 },
      };

      // not visible from the inside of the building
      brushPolygonsToMask =
      {
        { 1139, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 1140, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 1142, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 590, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 591, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 592, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 563, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 }
      };
      brushSectorsToMask = 
      {
        { 90, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 }
      };
      break;

    case EWorld::Suburbs:
      brushPolygonsToIgnore =
      {
        { 976 },
        { 969 },
        { 968 },
      };
      break;

    case EWorld::Sewers:
      brushPolygonsToIgnore =
      {
        // water
        { 483 },
        { 415 },
        { 58 },
        { 120 },
        { 129 },
        { 285 },
        { 307 },
        { 313 },
        { 319 },
        { 328 },
        { 338 },
        { 366 },
        { 463 },
        { 667 },
        { 675 },
      };
      break;

    case EWorld::Metropolis:
      brushPolygonsToIgnore =
      {
        { 131 },
        { 132 },
        { 1206 },
        { 973 },
      };
      break;

    case EWorld::Karnak:
      brushPolygonsToIgnore =
      {
        // start room roof
        { 3014 },

        // windows all right
        { 1210 },
        { 1211 },
        { 1186 },
        { 1187 },
        { 1188 },
        { 1189 },

        // windows all left
        { 1191 },
        { 1190 },
        { 1214 },

        // top of the building with pool and ammo
        { 1884 },
        { 1885 },
        { 1886 },
        { 1887 },

        // water
        { 1030 },
        { 3361 },
        { 3384 },
        { 2307 },
        { 2308 },
        { 2309 },
        { 2310 },
        { 23 },
        { 25 },
        { 3175 },
        { 3176 },
        { 806 },
        { 817 },
      };

      brushPolygonsToMask =
      {
        // start room left windows
        { 3015, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        // start room right windows
        { 3018, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
      };

      break;

    case EWorld::Luxor:
      brushPolygonsToIgnore =
      {
        // left area secret
        { 903 },
        { 1906 },
        // right area secret
        { 854 },
        { 878 },
        { 856 },
        { 857 },
        // secret ammo
        { 1374 },
        // middle section roof, but sky must be visible
        { 613 },
        { 614 }
      };

      brushPolygonsToMask =
      {
        { 599, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 }
      };
      brushPolygonRangesToMask =
      {
        { 606, 610, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 1849, 1851, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
        { 1865, 1870, RG_GEOMETRY_VISIBILITY_TYPE_WORLD_1 },
      };
      break;

    case EWorld::SacredYards:
      brushPolygonsToIgnore =
      {
        { 5 },
        { 741 },
        { 317 },
        { 4580 },
        // roof
        { 25 },
        { 1183 },
        { 1190 },
      };
      break;
  }


  switch (eCurrentWorld)
  {
    case EWorld::SandCanyon:
      cutsceneLightPositions =
      {
        { 45, -22.75f, 90 },
        { -18.875f, -15.25f, 92.5f },
      };
      break;

    case EWorld::TombOfRamses:
      cutsceneLightPositions =
      {
        { 146, 85.25f, -537 },
        //{ 146.75f, 65.75f, -327.75f },
        { 145.5f, 65.25f, -344.25f }
      };
      break;

    case EWorld::ValleyOfTheKings:
      cutsceneLightPositions =
      {
        { -292.75f, -101, -586.25f },
        { -259.25f, -98.25f, -589 },
        { -267.75f, -106.75f, -576.75f },
        // before the elemental
        { -175.5f, -114.5f, -575 },
      };
      break;

    case EWorld::MoonMountains:
      cutsceneLightPositions =
      {
        { 24.375f, -133.188f, -174.188f },
        { 27, -119.5f, -410.5f }
      };
      break;

    case EWorld::Oasis:
      cutsceneLightPositions =
      {
        { 52, 13.5f, -434 }
      };
      break;

    case EWorld::Suburbs:
      cutsceneLightPositions =
      {
        { -2, -28.75f, 636 }
      };
      break;

    case EWorld::Sewers:
      cutsceneLightPositions =
      {
        { -32.5f, -30, -288 },
        { 71.5f, 25.5f, -288.5f },
        { 4, -43.75f, -87.7813f },
        { 0, -7.75f, -4 },
        { -32.25f, -6.375f, -69.25f }
      };
      break;

    case EWorld::Metropolis:
      cutsceneLightPositions =
      {
        { 1336, -58.75f, -32 },
        { 1300.25f, -62.75f, -32.25f }
      };
      break;

    case EWorld::Karnak:
      cutsceneLightPositions =
      {
        // final cutscene
        { -167, 5.25f, 2048 },
        // scorpion room
        { 480.75f, -0.75f, 2048 }
      };
      break;

    case EWorld::Luxor:
      cutsceneLightPositions =
      {
        { -3.13968f, 22.5902f, 11.6911f },
        { -2.96487f, 22.0902f, -12.2622f },
      };
      break;
  }

  
  float fPotentialLightThreshold = 2.0f;
  switch (eCurrentWorld)
  {
    case EWorld::TombOfRamses:
    case EWorld::Metropolis:
    case EWorld::Karnak:
    case EWorld::Luxor:
      fPotentialLightThreshold = 4.0f;
      break;
    case EWorld::TheGreatPyramid:
      fPotentialLightThreshold = 4.0f; // 8.0f; -- if large fire is needed on towers' top
      break;
  }
  _srtGlobals.srt_fPotentialLightSphDistanceThreshold = fPotentialLightThreshold;



  _srtGlobals.srt_fWaterDensityMultiplier = 5.0f;
  switch (eCurrentWorld)
  {
    case EWorld::Sewers:
      _srtGlobals.srt_vWaterExtinction = { 0.014f, 0.010f, 0.080f };
      break;
    default:
      _srtGlobals.srt_vWaterExtinction = { 0.025f, 0.016f, 0.011f };
      break;
  }



  _srtGlobals.srt_bReflRefrShadows = false;
  _srtGlobals.srt_bReflRefrToIndirect = false;
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

  return vector_Contains(ptdCachedTextures.aWater, ptd);
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

  return vector_Contains(ptdCachedTextures.aFire, ptd);;
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
  return vector_Contains(ptdCachedTextures.aDisabledCulling, ptd);;
}

RgGeometryPrimaryVisibilityType SSRT::CustomInfo::GetModelMaskBit(const FLOAT3D &vPosition) const
{
  // a tree that must not be visible from the area with a pool and tower
  if (eCurrentWorld == EWorld::ValleyOfTheKings && (vPosition - FLOAT3D(34, -41, -210.5f)).ManhattanNorm() < 10.0f)
  {
    return RgGeometryPrimaryVisibilityType::RG_GEOMETRY_VISIBILITY_TYPE_WORLD_2;
  }

  return RgGeometryPrimaryVisibilityType::RG_GEOMETRY_VISIBILITY_TYPE_WORLD_0;
}

RgGeometryPrimaryVisibilityType SSRT::CustomInfo::GetBrushMaskBit(const CBrushPolygon *pPolygon) const
{
  ASSERT(pPolygon != nullptr && pPolygon->bpo_pbscSector != nullptr);
  
  for (const MaskedBrushSector &s : brushSectorsToMask)
  {
    if (pPolygon->bpo_pbscSector->bsc_iInWorld == s.iBrushSectorIndex)
    {
      return s.eMaskBit;
    }
  }

  for (const MaskedBrushPoly &p : brushPolygonsToMask)
  {
    if (pPolygon->bpo_iInWorld == p.iBrushPolygonIndex)
    {
      return p.eMaskBit;
    }
  }

  for (const MaskedBrushPolyRange &r : brushPolygonRangesToMask)
  {
    if (pPolygon->bpo_iInWorld >= r.iBrushPolygonIndexStart && 
        pPolygon->bpo_iInWorld <= r.iBrushPolygonIndexEnd)
    {
      return r.eMaskBit;
    }
  }

  return RgGeometryPrimaryVisibilityType::RG_GEOMETRY_VISIBILITY_TYPE_WORLD_0;
}

uint32_t SSRT::CustomInfo::GetCullMask(const FLOAT3D &vCameraPosition) const
{
  switch (eCurrentWorld)
  {
    case EWorld::Hatshepsut:
      if (vCameraPosition(1) > 3000.0f)
      {
        return 0b000;
      }
      break;

    case EWorld::ValleyOfTheKings:
      // after terrain
      if (vCameraPosition(3) < -265)
      {
        return 0b011;
      }
      // before terrain
      else if (vCameraPosition(3) > 242.0f)
      {
        return 0b101;
      }
      // on terrain
      else
      {
        return 0b111;
      }
      break;

    case EWorld::MoonMountains:
      if (vCameraPosition(3) > -193.0f)
      {
        // start terrain and middle cave are visible
        return 0b011;
      }
      else if (vCameraPosition(3) < -345.0f)
      {
        // black pyramid is visible
        return 0b101;
      }
      else
      {
        return 0b001;
      }
      break;

    case EWorld::Oasis:
      if ((vCameraPosition(1) > 26.0f && vCameraPosition(3) < -82.0f) || vCameraPosition(3) < -127.0f)
      {
        return 0b001;
      }
      break;

    case EWorld::Karnak:
      // unhide start room window polygons
      if (vCameraPosition(1) > 659.0f && vCameraPosition(3) > 2002.0f)
      {
        return 0b001;
      }
      break;

    case EWorld::Luxor:
      if (-172.0f < vCameraPosition(1) && vCameraPosition(1) < 40.0f)
      {
        // inside the building with a transmitter, ignore outside walls
        return 0b001;
      }
      break;
  }

  return 0b111;
}

float SSRT::CustomInfo::GetRayLength(const FLOAT3D &vCameraPosition) const
{
  switch (eCurrentWorld)
  {
    case EWorld::Luxor:
      if (vCameraPosition(1) > 3000.0f)
      {
        // draw only starship
        return 500.0f;
      }
      break;
  }

  return 10000.0f;
}

bool SSRT::CustomInfo::IsNoWaterRefractionForced(const FLOAT3D &vCameraPosition) const
{
  // if in the first area of moon mountains, the waterfall should look like fully reflective
  if (eCurrentWorld == EWorld::MoonMountains && vCameraPosition(3) > -154)
  {
    return true;
  }

  return false;
}

bool SSRT::CustomInfo::IsNoBackfaceReflForNoMediaChange(const FLOAT3D &vCameraPosition) const
{
  switch (eCurrentWorld)
  {
    case EWorld::MoonMountains:
      // TODO
      return false;

    case EWorld::Oasis:
      return true;
  }

  return false;
}

int SSRT::CustomInfo::GetCullingMaxSectorDepth() const
{
  switch (_srtGlobals.srt_iCullingMaxSectorDepthQualityLevel)
  {
    // less
    case 1:
      return Max<INDEX>(roundf(_srtGlobals.srt_iCullingMaxSectorDepth * 0.75f), Min<INDEX>(_srtGlobals.srt_iCullingMaxSectorDepth, 3));
    // reduced
    case 2:
      return Max<INDEX>(roundf(_srtGlobals.srt_iCullingMaxSectorDepth * 0.4f), 2);
    // min
    case 3:
      return 2;
  }

  return _srtGlobals.srt_iCullingMaxSectorDepth;
}

bool SSRT::CustomInfo::IsBrushIgnored(CEntity *penBrush) const
{ 
  bool isZoning = penBrush->en_ulFlags & ENF_ZONING; 

  if (bCheckOnlyZoningForBrushIgnored && !isZoning)
  {
    return false;
  }

  // ugly way to remove terrain planes and other unnecessary entities above the world
  if (penBrush->GetName() == "World Base")
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

bool SSRT::CustomInfo::IsBrushSectorIgnored(const CBrushSector *pSector) const
{
  ASSERT(pSector != nullptr);

  for (const IgnoredBrushSector &s : brushSectorsToIgnore)
  {
    if (pSector->bsc_iInWorld == s.iBrushSectorIndex)
    {
      return true;
    }
  }

  return false;
}

bool SSRT::CustomInfo::IsBrushPolygonIgnored(const CBrushPolygon *pPolygon) const
{
  ASSERT(pPolygon != nullptr);
  ASSERT(pPolygon->bpo_pbscSector != nullptr);

  for (const IgnoredBrushSector &s : brushSectorsToIgnore)
  {
    if (pPolygon->bpo_pbscSector->bsc_iInWorld == s.iBrushSectorIndex)
    {
      return true;
    }
  }

  for (const IgnoredBrushPoly &p : brushPolygonsToIgnore)
  {
    if (pPolygon->bpo_iInWorld == p.iBrushPolygonIndex)
    {
      return true;
    }
  }

  for (const IgnoredBrushPolyRange &r : brushPolygonRangesToIgnore)
  {
    if (pPolygon->bpo_iInWorld >= r.iBrushPolygonIndexStart &&
        pPolygon->bpo_iInWorld <= r.iBrushPolygonIndexEnd)
    {
      return true;
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

bool SSRT::CustomInfo::IsSphericalLightIgnored(const CLightSource *plsLight, float fPotentialLightThreshold) const
{
  CEntity *pen = plsLight->ls_penEntity;

  if (HasLightEntityVertices(pen))
  {
    return false;
  }

  const FLOAT3D &vPosition = pen->GetPlacement().pl_PositionVector;

  if (HasLightEntityVertices(pen->GetParent()) 
      && (pen->GetParent()->GetPlacement().pl_PositionVector - vPosition).ManhattanNorm() < fPotentialLightThreshold)
  {
    return false;
  }

  // check if it's a cutscene light
  for (const FLOAT3D &v : cutsceneLightPositions)
  {
    if ((v - vPosition).ManhattanNorm() < 0.5f)
    {
      return false;
    }
  }

  /*{
    FOREACHINLIST(CEntity, en_lnInParent, pen->en_lhChildren, itenChild)
    {
      if (HasLightEntityVertices(itenChild))
      {
        return false;
      }
    }
  }*/

  return true;
}

bool SSRT::CustomInfo::IsDynamicSphericalLightIgnored(const CLightSource *plsLight) const
{
  // check for very specific light in karnak in secret area
  if (eCurrentWorld == EWorld::Luxor)
  {
    if (plsLight->ls_paoLightAnimation == nullptr)
    {
      return false;
    }

    if (plsLight->ls_paoLightAnimation->ao_AnimData == nullptr)
    {
      return false;
    }

    if (plsLight->ls_penEntity != nullptr &&
       (plsLight->ls_penEntity->GetLerpedPlacement().pl_PositionVector - FLOAT3D(160, 5, -103.25f)).ManhattanNorm() < 1.0f)
    {
      return true;
    }
  }

  return false;
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

  // -1 for original direction
  if (_srtGlobals.srt_fAnimatedSunTimeLength < -0.5f)
  {
    FLOAT3D vDirection;
    AnglesToDirectionVector(vOriginalEuler, vDirection);
  
    return vDirection;
  }

  // 0 for overriden direction without lerping
  if (_srtGlobals.srt_fAnimatedSunTimeLength < 0.05f)
  {
    FLOAT3D vDirection;
    AnglesToDirectionVector(_srtGlobals.srt_vAnimatedSunTargetEuler, vDirection);

    return vDirection;
  }

  // >0 for animated

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

FLOAT SSRT::CustomInfo::GetSkyIntensity() const
{
  return 
    _srtGlobals.srt_fSkyColorMultiplier *
    (bSkyboxIntensityDependsOnSkyClouds ? Lerp(1.0f, fSkyboxCloudyIntensity, fSkyCloudsOpacity) : 1.0f);
}

FLOAT SSRT::CustomInfo::GetSkySaturation() const
{
  return _srtGlobals.srt_fSkyColorSaturation;
}

RgSkyType SSRT::CustomInfo::GetLevelSkyType() const
{
  switch (eCurrentWorld)
  {
    case EWorld::TombOfRamses:
    case EWorld::Sewers:
      return RG_SKY_TYPE_COLOR;
    default:
      return RG_SKY_TYPE_RASTERIZED_GEOMETRY;
  }
}

bool SSRT::CustomInfo::AreDynamicTexCoordsIgnored(CEntity *penBrush) const
{
  return _srtGlobals.srt_bIgnoreDynamicTexCoords;
}

template<std::size_t S>
static bool IsIndexInArray(const INDEX(&refArr)[S], INDEX iValue)
{
  for (INDEX a : refArr)
  {
    if (a == iValue)
    {
      return true;
    }
  }

  return false;
}

bool SSRT::CustomInfo::DoesPolygonPreserveTheSameMedia(const CBrushPolygon *pPolygon) const
{
  const INDEX iPoly = pPolygon->bpo_iInWorld;

  switch (eCurrentWorld)
  {
    case EWorld::MoonMountains:
      return 
        // flat waterfall in the first area
        iPoly == 25 ||
        // part of the second waterfall which is visible from cave area
        (iPoly >= 2113 && iPoly <= 2125);

    case EWorld::Oasis:
      return 
        // waterfall
        iPoly >= 664 && iPoly <= 671;
  }

  return false;
}

static bool ptdCachedTextures_Check(const CTextureData *pTd, const std::vector<CTextureData *> &aVec)
{
  if (pTd != nullptr)
  {
    return vector_Contains(aVec, pTd);
  }

  return false;
}

static bool ptdCachedTextures_Check(const CTextureObject *pTo, const std::vector<CTextureData *> &aVec)
{
  if (pTo != nullptr && pTo->ao_AnimData != nullptr)
  {
    return vector_Contains(aVec, (CTextureData *)pTo->ao_AnimData);
  }

  return false;
}

bool SSRT::CustomInfo::IsFullMetallicForced(CTextureObject *pTo) const
{
  return ptdCachedTextures_Check(pTo, ptdCachedTextures.aFullyMetallic);
}

bool SSRT::CustomInfo::IsAlphaTestForced(CTextureObject *pTo) const
{
  return ptdCachedTextures_Check(pTo, ptdCachedTextures.aAlphaTest);
}

bool SSRT::CustomInfo::IsAlphaTestForcedForParticles(CTextureData *pTd) const
{
  return ptdCachedTextures_Check(pTd, ptdCachedTextures.aAlphaTestParticles);
}

bool SSRT::CustomInfo::IsEmissionForced(CTextureObject *pTo) const
{
  return ptdCachedTextures_Check(pTo, ptdCachedTextures.aEmission);
}

bool SSRT::CustomInfo::IsGlass(CTextureObject *pTo) const
{
  return ptdCachedTextures_Check(pTo, ptdCachedTextures.aGlass);
}

bool SSRT::CustomInfo::IsMirror(CTextureObject *ptoReflection) const
{
  return ptdCachedTextures_Check(ptoReflection, ptdCachedTextures.aMirror);
}

bool SSRT::CustomInfo::IsCalcNormalsForced(CTextureObject *pTo) const
{
  return ptdCachedTextures_Check(pTo, ptdCachedTextures.aCalcNormals);
}

bool SSRT::CustomInfo::IsClampWrapForced(CTextureData *pTd) const
{
  return ptdCachedTextures_Check(pTd, ptdCachedTextures.aClampWrap);
}

bool SSRT::CustomInfo::IsOverrideDisabled(CTextureData *pTd) const
{
  return eCurrentWorld != EWorld::TheGreatPyramid && ptdCachedTextures_Check(pTd, ptdCachedTextures.aDisabledOverride);
}

bool SSRT::CustomInfo::IsModelWithTextureIgnored(CRenderModel *prm, CTextureObject *pTo) const
{
  return 
    eCurrentWorld == EWorld::TheGreatPyramid && 
    prm != nullptr && (prm->rm_pmmiMip->mmpi_ulLayerFlags & SRF_REFLECTIONS) &&
    ptdCachedTextures_Check(pTo, ptdCachedTextures.aIgnoreModel);
}

bool SSRT::CustomInfo::IsNoEffectOnTexture(const CTextureData *pTd) const
{
  return ptdCachedTextures_Check(pTd, ptdCachedTextures.aNoEffectOnTexture);
}

bool SSRT::CustomInfo::IsLightOffsetFixEnabled(const CLightSource *plsLight) const
{
  if (plsLight == nullptr ||
      plsLight->ls_penEntity == nullptr ||
      plsLight->ls_penEntity->GetRenderType() != CEntity::RenderType::RT_MODEL)
  {
    return false;
  }

  return ptdCachedTextures_Check(&plsLight->ls_penEntity->GetModelObject()->mo_toTexture, ptdCachedTextures.aLightOffsetFix);
}

bool SSRT::CustomInfo::IsLightForceDynamic(const CLightSource *plsLight) const
{
  // we need to check only very specific case: rocketeer in Sand canyon

  if (eCurrentWorld != EWorld::SandCanyon)
  {
    return false;
  }

  if (plsLight == nullptr ||
      plsLight->ls_penEntity == nullptr ||
      plsLight->ls_penEntity->en_penParent == nullptr ||
      plsLight->ls_penEntity->en_penParent->GetRenderType() != CEntity::RenderType::RT_MODEL)
  {
    return false;
  }

  // check only parent: the light source has that rocketeer as a parent
  CEntity *penParent = plsLight->ls_penEntity->en_penParent;

  return ptdCachedTextures_Check(&penParent->GetModelObject()->mo_toTexture, ptdCachedTextures.aLightForceDynamic);
}

bool SSRT::CustomInfo::CanHazeBeApplied() const
{
  switch (eCurrentWorld)
  {
    case EWorld::Dunes:
    case EWorld::AlleyOfSphinxes:
    case EWorld::SandCanyon:
    case EWorld::TombOfRamses:
    case EWorld::TheGreatPyramid:
    case EWorld::Suburbs:
    case EWorld::Metropolis:
      return false;
    default:
      return true;
  }
}

bool SSRT::CustomInfo::IsOnlyReflectWaterAllowed() const
{
  switch (eCurrentWorld)
  {
    // waterfall in oasis
    case EWorld::Oasis:
      return false;
    default:
      return true;
  }
}

bool SSRT::CustomInfo::IsBrushSectorHazeIgnored(const CBrushSector *pSector) const
{
  // haze inside building
  const INDEX aValleyIgnoredSectors[] = { 92, 96, 98, 114, 225, 187, 188, 292, 110, 291, 89, 543 };
  // waterfall
  const INDEX aOasisIgnoredSectors[] = { 62, 64 };
  const INDEX aSewersIgnoredSectors[] = { 144, 36, 42, 43 };
  // room with a heart
  const INDEX aKarnakIgnoredSectors[] = { 44, 129, 225, 194, 68, 298, 195, 189, 196, 352 };

  switch (eCurrentWorld)
  {
    case EWorld::ValleyOfTheKings:
      return IsIndexInArray(aValleyIgnoredSectors, pSector->bsc_iInWorld);

    case EWorld::Oasis:
      return IsIndexInArray(aOasisIgnoredSectors, pSector->bsc_iInWorld);

    case EWorld::Sewers:
      return IsIndexInArray(aSewersIgnoredSectors, pSector->bsc_iInWorld);

    case EWorld::Karnak:
      return IsIndexInArray(aKarnakIgnoredSectors, pSector->bsc_iInWorld);
  }
  
  return false;
}

bool SSRT::CustomInfo::IsBrushSectorHazeForced(const CBrushSector *pSector) const
{
  return false; // eCurrentWorld == EWorld::Oasis && pSector->bsc_iInWorld == 64; // inside cave
}

bool SSRT::CustomInfo::GetActivatePlateState(const FLOAT3D &vPosition, float *pOutState) const
{
  if (eCurrentWorld != EWorld::TheGreatPyramid)
  {
    return false;
  }

  const struct
  {
    FLOAT3D vRingModelPosition;
    // look WorldBase::SetPyramidPlateActivateAlpha
    INDEX iTextureBlendingID;
  }
  aActivationPlates[] =
  {
    // ring 1
    { FLOAT3D(0, 216, -1264),   11 },
    // ring 2
    { FLOAT3D(144, 216, -1408), 12 },
    // ring 3
    { FLOAT3D(0, 216, -1552),   13 },
    // ring 4
    { FLOAT3D(-144, 216, -1408), 14 },
  };

  for (const auto &p : aActivationPlates)
  {
    if ((vPosition - p.vRingModelPosition).ManhattanNorm() < 1.0f)
    {
      CTextureBlending &tb = pWorld->wo_atbTextureBlendings[p.iTextureBlendingID];
      
      ASSERT(
        tb.tb_strName == "Activated plate 1" ||
        tb.tb_strName == "Activated plate 2" ||
        tb.tb_strName == "Activated plate 3" ||
        tb.tb_strName == "Activated plate 4"
      );

      UBYTE r, g, b;
      ColorToRGB(tb.tb_colMultiply, r, g, b);
      
      *pOutState = r / 255.0f;
      return true;
    }
  }

  *pOutState = 0.0f;
  return true;
}

bool SSRT::CustomInfo::IsInvisibleEnemy(CEntity *pEntity, COLOR colLight, COLOR colAmbient, bool hasShadows) const
{
  if (pEntity->GetRenderType() != CEntity::RenderType::RT_MODEL)
  {
    return false;
  }

  auto *mo = pEntity->GetModelObject();
  bool isTextureForInvisible = ptdCachedTextures_Check(&mo->mo_toTexture, ptdCachedTextures.aInvisibleEnemy);

  return !hasShadows && isTextureForInvisible && colAmbient == C_WHITE && mo->mo_colBlendColor;
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

void SSRT::CustomInfo::Update(const FLOAT3D &vCameraPosition)
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


  if (eCurrentWorld == EWorld::TheGreatPyramid)
  {
    // if on boss fight arena
    if (vCameraPosition(2) > 175.0f )
    {
      _srtGlobals.srt_fSunIntensity = GREAT_PYRAMID_SUN_INTENSITY_BOSS_FIGHT;
      _srtGlobals.srt_fSkyColorMultiplier = GREAT_PYRAMID_SKY_COLOR_BOSS_FIGHT;

      _srtGlobals.srt_fTonemappingMinLogLuminance = GREAT_PYRAMID_FIGHT_TONEMAPPING_MIN_LOG;
    }
    else
    {
      // start animation
      if (vCameraPosition(3) < -234.0f && tmAnimatedSkyIntensityOrigin < 0)
      {
        tmAnimatedSkyIntensityOrigin = tmCurrent;
      }

      if (tmAnimatedSkyIntensityOrigin > 0.0f && (tmCurrent - tmAnimatedSkyIntensityOrigin < GREAT_PYRAMID_SKY_COLOR_CHANGE_LENGTH))
      {
        _srtGlobals.srt_fSkyColorMultiplier = Lerp(
          GREAT_PYRAMID_SKY_COLOR_START,
          GREAT_PYRAMID_SKY_COLOR_END,
          Clamp((tmCurrent - tmAnimatedSkyIntensityOrigin) / GREAT_PYRAMID_SKY_COLOR_CHANGE_LENGTH, 0.0f, 1.0f)
        );
      }
    }
  }

  if (eCurrentWorld == EWorld::MoonMountains)
  {
    _srtGlobals.srt_bReflRefrShadows = true;
    _srtGlobals.srt_bReflRefrToIndirect = true;

    // first section or end cutscene
    if (vCameraPosition(3) > -195)
    {
      _srtGlobals.srt_bReflRefrShadows = false;
    }
    else if (vCameraPosition(2) < -65)
    {
      if (vCameraPosition(1) > 0 && vCameraPosition(3) < -350)
      {
        // inside cave with water
        _srtGlobals.srt_bReflRefrShadows = false;
        _srtGlobals.srt_bReflRefrToIndirect = false;
      }
      else if (vCameraPosition(2) < -105)
      {
        // behind the second waterfall, last section
        _srtGlobals.srt_bReflRefrShadows = true;
        _srtGlobals.srt_bReflRefrToIndirect = false;
      }
    }
  }

  fFlashlightIntensityForLevel = 1.0f;
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
    fSkyCloudsOpacity = pow(fSkyCloudsOpacity, 0.5f);
  }
}

void SSRT::CustomInfo::OnSphereLightAdd(const CSphereLight &sphLt)
{
  // if a rocketeer with light is in this range, disable flashlight to show his light source
  if (eCurrentWorld == EWorld::SandCanyon && sphLt.plsLight != nullptr)
  {
    const float x = sphLt.absPosition(1);
    const float z = sphLt.absPosition(3);

    const float zFlickerEnd = -165.0f;
    const float zFlickerStart = -162.0f;

    bool activateFlashlightFlicker =
      -180.0f < z && z < zFlickerStart &&
      55.0f < x && x < 77.0f;

    if (!activateFlashlightFlicker)
    {
      return;
    }

    // if certainly that exact rocketeer
    if (!IsLightForceDynamic(sphLt.plsLight))
    {
      return;
    }

    if (z < zFlickerEnd)
    {
      fFlashlightIntensityForLevel = 0.0f;
      return;
    }

    // to [0..1]
    const float zRelative = Abs(z - zFlickerStart) / Abs(zFlickerEnd - zFlickerStart);
    const float firstAttenuation = 0.3f;

    if (zRelative < firstAttenuation)
    {
      fFlashlightIntensityForLevel = Lerp(1.0f, 0.25f, zRelative / firstAttenuation);
    }
    else
    {
      fFlashlightIntensityForLevel = Lerp(0.8f, 0.0f, zRelative - firstAttenuation / 0.7f);
    }
  }
}

float SSRT::CustomInfo::GetFlashlightIntensityForLevel(const FLOAT3D &vCameraPosition) const
{
  if (eCurrentWorld == EWorld::SandCanyon &&
      // if player in some specific place
      vCameraPosition(1) > 70 && vCameraPosition(1) < 90)
  {
    return fFlashlightIntensityForLevel;
  }

  return 1.0f;
}

bool SSRT::CustomInfo::IsPotentialForcedAdd(const FLOAT3D &vPos) const
{
  // very specific light in sand canyon
  return eCurrentWorld == EWorld::SandCanyon && (vPos - FLOAT3D(8.5f, -22.125f, -193.5f)).ManhattanNorm() < 1.0f;
}
