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
#include "RTProcessing.h"

#include <Engine/Base/Lists.h>
#include <Engine/Base/Relations.h>
#include <Engine/Brushes/Brush.h>
#include <Engine/Entities/Entity.h>
#include <Engine/Light/LightSource.h>
#include <Engine/World/World.h>
#include <Engine/Templates/BSP.h>
#include <Engine/Raytracing/Scene.h>

#include <Engine/Base/ListIterator.inl>
#include <Engine/Templates/StaticStackArray.h>



// list of active sectors
static CListHead RT_lhActiveSectors;
// RT: currently added models, used for clearing ENF_INRENDERING flag at the end of scanning
static CStaticStackArray<CEntity*> RT_saAddedModels;


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
        extern INDEX srt_bEnableViewerShadows;

        // if it's a viewer and there should be no shadows from it
        if (!srt_bEnableViewerShadows && pScene->GetViewerEntityID() == pen->en_ulID)
        {
          continue;
        }

        if (pen->en_ulFlags & ENF_INRENDERING)
        {
          continue;
        }
        pen->en_ulFlags |= ENF_INRENDERING;

        bool isDirLight = pen->GetLightSource() != nullptr && (pen->GetLightSource()->ls_ulFlags & LSF_DIRECTIONAL);

        // add it as a model and scan for light sources
        if (!isDirLight)
        {
          RT_AddModelEntity(pen, pScene);
        }

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
static void RT_AddGivenZoningSector(CBrushSector *pbsc, SSRT::Scene *pScene)
{
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
      }
    ENDFOR
  }

  const DOUBLE3D re_vdViewSphere = FLOATtoDOUBLE(pScene->GetCameraPosition());
  const double re_dViewSphereR = 30.0;

  // for each active sector
  while (!lhToAdd.IsEmpty())
  {
    CBrushSector *pbsc = LIST_HEAD(lhToAdd, CBrushSector, bsc_lnInActiveSectors);
    // remove it from list of sectors to add
    pbsc->bsc_lnInActiveSectors.Remove();

    // add it to final list
    RT_AddGivenZoningSector(pbsc, pScene);
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
            if (pbscRelated->bsc_bspBSPTree.TestSphere(re_vdViewSphere, re_dViewSphereR) >= 0)
            {
              // add it to list to add
              lhToAdd.AddTail(pbscRelated->bsc_lnInActiveSectors);
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
}


void RT_AddModelEntitiesAroundViewer(SSRT::Scene *pScene)
{
  if (pScene == nullptr || pScene->GetViewerEntity() == nullptr)
  {
    return;
  }

  ASSERT(RT_lhActiveSectors.IsEmpty());
  ASSERT(RT_saAddedModels.Count() == 0);

  RT_AddZoningSectorsAroundEntity(pScene->GetViewerEntity(), pScene);

  RT_CleanupScanning();
  ASSERT(RT_lhActiveSectors.IsEmpty());
  ASSERT(RT_saAddedModels.Count() == 0);
}