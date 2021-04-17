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
#include <Engine/World/World.h>
#include <Engine/Templates/BSP.h>
#include <Engine/Raytracing/Scene.h>

#include <Engine/Base/ListIterator.inl>



// list of active sectors
static CListHead re_lhActiveSectors;     


/* Add to rendering all entities that are inside an zoning brush sector. */
static void RT_AddEntitiesInSector(CBrushSector *pbscSectorInside, SSRT::Scene *pScene)
{
  // if we don't have a relevant sector to test with 
  if (pbscSectorInside == NULL || pbscSectorInside->bsc_bspBSPTree.bt_pbnRoot == NULL)
  {
    // do nothing
    return;
  }

  // for all entities in the sector
  {
    FOREACHDSTOFSRC(pbscSectorInside->bsc_rsEntities, CEntity, en_rdSectors, pen)
      if (pen->en_RenderType == CEntity::RT_MODEL || pen->en_RenderType == CEntity::RT_EDITORMODEL)
      {
        extern INDEX srt_bEnableViewerShadows;

        // if it's a viewer and there should be no shadows from it
        if (!srt_bEnableViewerShadows && pScene->GetViewerEntityID() == pen->en_ulID)
        {
          continue;
        }

        // add it as a model and scan for light sources
        RT_AddModelEntity(pen, pScene);

        // also, add its particles
        RT_AddParticlesForEntity(pen, pScene);
      }
    ENDFOR
  }
};


static void RT_AddActiveSector(CBrushSector &bscSector, SSRT::Scene *pScene)
{
  // if already active
  if (bscSector.bsc_lnInActiveSectors.IsLinked())
  {
    // do nothing;
    return;
  }

  // add it to active sectors list
  re_lhActiveSectors.AddTail(bscSector.bsc_lnInActiveSectors);

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

    // RT: TODO: portals for culling models?
  }
}


static void RT_CleanupScanning()
{
  {
    FORDELETELIST(CBrushSector, bsc_lnInActiveSectors, re_lhActiveSectors, itbsc)
    {
      itbsc->bsc_lnInActiveSectors.Remove();
    }
  }
  ASSERT(re_lhActiveSectors.IsEmpty());
}


void RT_AddModelEntitiesAroundViewer(SSRT::Scene *pScene)
{
  if (pScene == nullptr || pScene->GetViewerEntity() == nullptr)
  {
    return;
  }

  ASSERT(re_lhActiveSectors.IsEmpty());

  RT_AddZoningSectorsAroundEntity(pScene->GetViewerEntity(), pScene);

  RT_CleanupScanning();
  ASSERT(re_lhActiveSectors.IsEmpty());
}