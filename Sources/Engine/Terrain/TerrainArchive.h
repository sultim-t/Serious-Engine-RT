/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_TERRAIN_ARCHIVE_H
#define SE_INCL_TERRAIN_ARCHIVE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Serial.h>
#include <Engine/Base/Lists.h>
#include <Engine/Templates/StaticArray.h>
#include <Engine/Templates/DynamicArray.h>
#include <Engine/Templates/DynamicArray.cpp>

/*
 * Terrain archive class -- a collection of terrains used by a level.
 */
class ENGINE_API CTerrainArchive : public CSerial {
public:
  CDynamicArray<CTerrain> ta_atrTerrains; // all the terrains in archive
  CWorld *ta_pwoWorld;  // the world

  // overrides from CSerial
  /* Read/write to/from stream. */
  void Read_t( CTStream *istrFile);  // throw char *
  void Write_t( CTStream *ostrFile); // throw char *
};


#endif  /* include-once check. */

