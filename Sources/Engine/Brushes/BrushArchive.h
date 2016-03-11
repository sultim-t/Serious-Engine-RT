/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_BRUSHARCHIVE_H
#define SE_INCL_BRUSHARCHIVE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>
#include <Engine/Templates/StaticArray.h>
#include <Engine/Templates/DynamicArray.h>

/*
 * Brush archive class -- a collection of brushes used by a level.
 */
class ENGINE_API CBrushArchive : public CSerial {
public:
  CDynamicArray<CBrush3D> ba_abrBrushes;    // all the brushes in archive
  // lists of all shadow maps that need calculation
  CListHead ba_lhUncalculatedShadowMaps;
  CWorld *ba_pwoWorld;  // the world
  // pointers to all polygons and sectors
  CStaticArray<CBrushPolygon *> ba_apbpo;
  CStaticArray<CBrushSector *> ba_apbsc;

  // overrides from CSerial
  /* Read/write to/from stream. */
  void Read_t( CTStream *istrFile);  // throw char *
  void Write_t( CTStream *ostrFile); // throw char *
  void ReadPortalSectorLinks_t( CTStream &strm);  // throw char *
  void WritePortalSectorLinks_t( CTStream &strm); // throw char *
  void ReadEntitySectorLinks_t( CTStream &strm);  // throw char *
  void WriteEntitySectorLinks_t( CTStream &strm); // throw char *

  /* Calculate bounding boxes in all brushes. */
  void CalculateBoundingBoxes(void);
  /* Create links between portals and sectors on their other side. */
  void LinkPortalsAndSectors(void);
  /* Make indices for all brush elements. */
  void MakeIndices(void);
  // remove shadow layers without valid light source in all brushes
  void RemoveDummyLayers(void);
  // cache all shadowmaps (upon loading of world)
  void CacheAllShadowmaps(void);
};


#endif  /* include-once check. */

