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

#include <unordered_map>

#include <Engine/Base/Lists.h>
#include <Engine/Base/Relations.h>
#include <Engine/Brushes/Brush.h>
#include <Engine/Entities/Entity.h>
#include <Engine/Light/LightSource.h>
#include <Engine/Graphics/Fog.h>
#include <Engine/Models/ModelData.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/World/World.h>
#include <Engine/Templates/BSP.h>
#include <Engine/Raytracing/Scene.h>

#include <Engine/Base/ListIterator.inl>
#include <Engine/Templates/StaticStackArray.h>

#include "RTProcessing.h"
#include "SSRTGlobals.h"


extern SSRT::SSRTGlobals _srtGlobals;


// list of active sectors
static CListHead RT_lhActiveSectors;
// RT: currently added models, used for clearing ENF_INRENDERING flag at the end of scanning
static CStaticStackArray<CEntity *> RT_saAddedModels;

static std::unordered_map<CBrushSector *, INDEX> RT_umBrushSectorDepth;


// RT: modified version of RenderModel.cpp::CalculateBoundingBox
static void CalculateBoundingSphere(CEntity *pEn, FLOAT3D *vOutCenter, float *fOutRadius)
{
  CModelObject *pmo = pEn->GetModelObject();

  // get model's data and lerp info
  CModelData *pmdModelData = (CModelData *)pmo->GetData();

  INDEX iFrame0, iFrame1;
  FLOAT fRatio;
  pmo->GetFrame(iFrame0, iFrame1, fRatio);

  // calculate projection model bounding box in object space
  const FLOAT3D &vMin0 = pmdModelData->md_FrameInfos[iFrame0].mfi_Box.Min();
  const FLOAT3D &vMax0 = pmdModelData->md_FrameInfos[iFrame0].mfi_Box.Max();
  const FLOAT3D &vMin1 = pmdModelData->md_FrameInfos[iFrame1].mfi_Box.Min();
  const FLOAT3D &vMax1 = pmdModelData->md_FrameInfos[iFrame1].mfi_Box.Max();

  FLOAT3D vObjectMinBB = Lerp(vMin0, vMin1, fRatio);
  FLOAT3D vObjectMaxBB = Lerp(vMax0, vMax1, fRatio);
  vObjectMinBB(1) *= pmo->mo_Stretch(1);  vObjectMaxBB(1) *= pmo->mo_Stretch(1);
  vObjectMinBB(2) *= pmo->mo_Stretch(2);  vObjectMaxBB(2) *= pmo->mo_Stretch(2);
  vObjectMinBB(3) *= pmo->mo_Stretch(3);  vObjectMaxBB(3) *= pmo->mo_Stretch(3);

  FLOATaabbox3D aabb(vObjectMaxBB, vObjectMinBB);

  FLOATaabbox3D enAabb;
  pEn->GetBoundingBox(enAabb);

  *vOutCenter = pEn->GetPlacement().pl_PositionVector + aabb.Center();
  *fOutRadius = Max(aabb.Size().Length(), enAabb.Size().Length());
}


static bool RT_TestModel(CEntity *pEn, SSRT::Scene *pScene)
{
  if (pEn->en_ulFlags & ENF_INRENDERING)
  {
    return false;
  }

  // always add light sources
  if (pEn->GetLightSource() != nullptr)
  {
    // dir lights are added separately, without culling
    return !(pEn->GetLightSource()->ls_ulFlags & LSF_DIRECTIONAL);
  }

  // add, if culling is disabled for this model
  if (pScene->GetCustomInfo()->IsAngularSizeCullingDisabled(pEn))
  {
    return true;
  }

  // if it's not a light source,
  // check anglular size of entity's sphere
  FLOAT3D vCenter;
  float fRadius;
  CalculateBoundingSphere(pEn, &vCenter, &fRadius);

  float fDistance = (vCenter - pScene->GetCameraPosition()).Length();

  if (fDistance > fRadius)
  {
    float fAngularSize = ATan(fRadius / fDistance) * 2.0f;
    
    // adjust for the current resolution
    float fRenderArea = _srtGlobals.srt_fRenderSize(1) * _srtGlobals.srt_fRenderSize(2);
    fAngularSize *= Max(1.0f, fRenderArea / (1920.0f * 1080.0f));

    // if model's angular size is too small, ignore it
    if (fAngularSize < _srtGlobals.srt_fCullingMinAngularSize)
    {
      return false;
    }
  }

  return true;
}

/* Add to rendering all entities that are inside an zoning brush sector. */
static void RT_AddEntitiesInSector(CBrushSector *pbscSectorInside, SSRT::Scene *pScene)
{
  // if we don't have a relevant sector to test with 
  if (pbscSectorInside == NULL || pbscSectorInside->bsc_bspBSPTree.bt_pbnRoot == NULL)
  {
    // do nothing
    return;
  }

  // RT: get entity of the sector and update it
  {
    ASSERT(pbscSectorInside->bsc_pbmBrushMip && pbscSectorInside->bsc_pbmBrushMip->bm_pbrBrush);
    CEntity *penSectorEntity = pbscSectorInside->bsc_pbmBrushMip->bm_pbrBrush->br_penEntity;

    pScene->UpdateBrush(penSectorEntity);
  }

  // for all entities in the sector
  {
    FOREACHDSTOFSRC(pbscSectorInside->bsc_rsEntities, CEntity, en_rdSectors, pen)
      if (pen->en_RenderType == CEntity::RT_BRUSH)
      {
        pScene->UpdateBrush(pen);
      }
      else if (pen->en_RenderType == CEntity::RT_MODEL || pen->en_RenderType == CEntity::RT_EDITORMODEL)
      {
        if (!RT_TestModel(pen, pScene))
        {
          continue;
        }

        pen->en_ulFlags |= ENF_INRENDERING;

        // add it as a model and scan for light sources
        RT_AddModelEntity(pen, pScene);
        
        // also, add its particles
        RT_AddParticlesForEntity(pen, pScene);
        
        RT_saAddedModels.Push() = pen;
      }
    ENDFOR
  }
}


static void RT_AddActiveSector(CBrushSector &bscSector, SSRT::Scene *pScene)
{
  // if already active
  if (bscSector.bsc_lnInActiveSectors.IsLinked())
  {
    // do nothing;
    return;
  }

  // add it to active sectors list
  RT_lhActiveSectors.AddTail(bscSector.bsc_lnInActiveSectors);

  CBrush3D &br = *bscSector.bsc_pbmBrushMip->bm_pbrBrush;

  // if should render field brush sector
  if (br.br_penEntity->en_RenderType == CEntity::RT_FIELDBRUSH) // && !_wrpWorldRenderPrefs.IsFieldBrushesOn())
  {
    // skip it
    //bscSector.bsc_ulFlags |= BSCF_INVISIBLE;
    return;
  }

  // mark that sector is visible
  //bscSector.bsc_ulFlags &= ~BSCF_INVISIBLE;

  // remember current sector
  /*re_pbscCurrent = &bscSector;
  re_pbrCurrent = &br;*/

  // get the entity the sector is in
  CEntity *penSectorEntity = bscSector.bsc_pbmBrushMip->bm_pbrBrush->br_penEntity;
  // if it has the entity (it is not the background brush)
  if (penSectorEntity != NULL)
  {
    // add all other entities near the sector
    RT_AddEntitiesInSector(&bscSector, pScene);
  }
}


/* Add to rendering one particular zoning brush sector. */
static void RT_AddGivenZoningSector(CBrushSector *pbsc, INDEX iSectorDepth, SSRT::Scene *pScene)
{
  // RT: if should be culled
  if (iSectorDepth >= _srtGlobals.srt_iCullingMaxSectorDepth)
  {
    return;
  }

  // get the sector's brush mip, brush and entity
  CBrushMip *pbmBrushMip = pbsc->bsc_pbmBrushMip;
  CBrush3D *pbrBrush = pbmBrushMip->bm_pbrBrush;
  ASSERT(pbrBrush != NULL);
  CEntity *penBrush = pbrBrush->br_penEntity;
  ASSERT(penBrush != NULL);

  // if the brush is field brush
  if (penBrush->en_RenderType == CEntity::RT_FIELDBRUSH)
  {
    // skip it
    return;
  }

  // RT: always add
  /*
  // here, get only the first brush mip
  CBrushMip *pbmRelevant = pbrBrush->GetFirstMip();

  // if it is the one of that sector
  if (pbmRelevant == pbmBrushMip)
  */
  {
    // add that sector to active sectors
    RT_AddActiveSector(*pbsc, pScene);
  }
}


// RT: from RenCache.cpp::void CRenderer::SetupFogAndHaze(void)
static void RT_CheckViewerInHaze(CBrushSector &bsc, SSRT::Scene *pScene)
{
  if (pScene->IsCameraInHaze())
  {
    return;
  }

  if (pScene->GetCustomInfo()->IsBrushSectorHazeIgnored(&bsc))
  {
    return;
  }

  const auto &vCameraPos = FLOATtoDOUBLE(pScene->GetCameraPosition());

  const auto &mRot = pScene->GetCameraRotation();
  auto vCameraDir = -FLOAT3D(mRot(1, 3), mRot(2, 3), mRot(3, 3));

  CHazeParameters hpDummy;
  CFogParameters fpDummy;
  if (pScene->GetCustomInfo()->IsBrushSectorHazeForced(&bsc) ||
      bsc.bsc_pbmBrushMip->bm_pbrBrush->br_penEntity->GetHaze(bsc.GetHazeType(), hpDummy, vCameraDir) || 
      bsc.bsc_pbmBrushMip->bm_pbrBrush->br_penEntity->GetFog(bsc.GetFogType(), fpDummy))
  {
    // if viewer is in this sector
    if (bsc.bsc_bspBSPTree.TestSphere(vCameraPos, 0.01) >= 0)
    {
      // mark that viewer is in haze
      pScene->SetCameraIsInHaze();

      return;
    }
  }
}


static bool RT_IsZoningSectorThin(const CBrushSector &bsc)
{
  FLOAT3D vSize = bsc.bsc_boxRelative.Size();

  bool isThin[3] =
  {
    vSize(1) <= _srtGlobals.srt_fCullingThinSectorSize,
    vSize(2) <= _srtGlobals.srt_fCullingThinSectorSize,
    vSize(3) <= _srtGlobals.srt_fCullingThinSectorSize,
  };

  return isThin[0] || isThin[1] || isThin[2];
}


static void RT_AddZoningSectorsAroundEntity(CEntity *pen, SSRT::Scene *pScene)
{
  // works only for non-zoning entities
  ASSERT(!(pen->en_ulFlags & ENF_ZONING));

  CListHead lhToAdd;
  // for all sectors this entity is in
  {
    FOREACHSRCOFDST(pen->en_rdSectors, CBrushSector, bsc_rsEntities, pbsc)
      // if the sector is not active
      if (!pbsc->bsc_lnInActiveSectors.IsLinked())
      {
        // add to list of sectors to add
        lhToAdd.AddTail(pbsc->bsc_lnInActiveSectors);

        // RT: zero depth for viewer sectors
        RT_umBrushSectorDepth[pbsc] = 0;


        // RT: while iterating for sectors viewer is in, check if it's in haze
        RT_CheckViewerInHaze(*pbsc, pScene);
      }
    ENDFOR
  }

  const DOUBLE3D re_vdViewSphere = FLOATtoDOUBLE(pScene->GetCameraPosition());
  const double re_dViewSphereR = 1.0f;

  // for each active sector
  while (!lhToAdd.IsEmpty())
  {
    CBrushSector *pbsc = LIST_HEAD(lhToAdd, CBrushSector, bsc_lnInActiveSectors);
    // remove it from list of sectors to add
    pbsc->bsc_lnInActiveSectors.Remove();

    ASSERT(RT_umBrushSectorDepth.find(pbsc) != RT_umBrushSectorDepth.end());
    INDEX iDepth = RT_umBrushSectorDepth[pbsc];

    bool isThin = RT_IsZoningSectorThin(*pbsc);

    // add it to final list
    RT_AddGivenZoningSector(pbsc, iDepth, pScene);
    // if isn't really added (wrong mip)
    if (!pbsc->bsc_lnInActiveSectors.IsLinked())
    {
      // skip it
      continue;
    }

    // for each portal in the sector
    FOREACHINSTATICARRAY(pbsc->bsc_abpoPolygons, CBrushPolygon, itbpo)
    {
      CBrushPolygon *pbpo = itbpo;
      if (!(pbpo->bpo_ulFlags & BPOF_PORTAL))
      {
        continue;
      }
      // for each sector related to the portal
      {
        FOREACHDSTOFSRC(pbpo->bpo_rsOtherSideSectors, CBrushSector, bsc_rdOtherSidePortals, pbscRelated)
          // if the sector is not active
          if (!pbscRelated->bsc_lnInActiveSectors.IsLinked())
          {
            // if the view sphere is in the sector
            // if (pbscRelated->bsc_bspBSPTree.TestSphere(re_vdViewSphere, re_dViewSphereR) >= 0)
            {
              // add it to list to add
              lhToAdd.AddTail(pbscRelated->bsc_lnInActiveSectors);

              // RT: increase depth for the related sector, if it's not too thin
              ASSERT(RT_umBrushSectorDepth.find(pbscRelated) == RT_umBrushSectorDepth.end());
              RT_umBrushSectorDepth[pbscRelated] = isThin ? iDepth : iDepth + 1;
            }
          }
        ENDFOR
      }
    }
  }
}


static void RT_CleanupScanning()
{
  {
    FORDELETELIST(CBrushSector, bsc_lnInActiveSectors, RT_lhActiveSectors, itbsc)
    {
      itbsc->bsc_lnInActiveSectors.Remove();
    }
  }

  for (int i = 0; i < RT_saAddedModels.Count(); i++)
  {
    RT_saAddedModels[i]->en_ulFlags &= ~ENF_INRENDERING;
  }

  RT_saAddedModels.PopAll();
  RT_umBrushSectorDepth.clear();
}


void RT_AddModelEntitiesAroundViewer(SSRT::Scene *pScene)
{
  if (pScene == nullptr || pScene->GetViewerEntity() == nullptr)
  {
    return;
  }

  ASSERT(RT_lhActiveSectors.IsEmpty());
  ASSERT(RT_saAddedModels.Count() == 0);
  ASSERT(RT_umBrushSectorDepth.empty());

  RT_AddZoningSectorsAroundEntity(pScene->GetViewerEntity(), pScene);

  RT_CleanupScanning();
  ASSERT(RT_lhActiveSectors.IsEmpty());
  ASSERT(RT_saAddedModels.Count() == 0);
  ASSERT(RT_umBrushSectorDepth.empty());
}