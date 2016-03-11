/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ENTITYHASHING_H
#define SE_INCL_ENTITYHASHING_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine\Math\Types.h>
#include <Engine\Math\Placement.h>
#include <Engine\Templates\CommunicationInterface.h>
#include <Engine\Classes\MovableEntity.h>

struct CClientEntry {  
//implementation
  TIME  ce_tmLastUpdated;
  CPlacement3D ce_plLastSentPlacement;
  CPlacement3D ce_plLastSentSpeed;

  CClientEntry() {
    ce_tmLastUpdated = -1.0f;
    ce_plLastSentPlacement.pl_PositionVector = FLOAT3D(0,0,0);
    ce_plLastSentPlacement.pl_OrientationAngle = ANGLE3D(0,0,0);
    ce_plLastSentSpeed.pl_PositionVector = FLOAT3D(0,0,0);
    ce_plLastSentSpeed.pl_OrientationAngle = ANGLE3D(0,0,0);
  }

}

class CEntityHashItem {
// implementation
public:
  ULONG ehi_ulEntityID;
  CEntityPointer ehi_epEntityPointer;
  CClientEntry ehi_ceClientEntries[SERVER_CLIENTS];

  CEntityHashItem() {ehi_ulEntityID = -1;} // entity pointer will initialize itself to NULL

  ~CEntityItem() {}; // entity poiner will destroy itself and remove the reference
     
  void WritePackedPlacement(CClientEntry &ceEntry,CNetworkMessage &nmMessage);

// interface
public:
  BOOL ClientNeedsUpdate(INDEX iClient,CNetworkMessage &nmMessage);
};


#define VALUE_TYPE ULONG
#define TYPE CEntityHashItem
#define CHashTableSlot_TYPE CEntityHashTableSlot
#define CHashTable_TYPE     CEntityHashTable
#include <Engine\Templates\HashTable.h>



class ENGINE_API CEntityHash {
// implementation
public:
  CEntityHashTable eh_ehtHashTable;

  CEntityHash();
  ~CEntityHash();

  ULONG GetItemKey(ULONG ulEntityID) {return ulEntityID;}
  ULONG GetItemValue(CEntityHashItem* ehiItem) {return ehiItem->ehi_ulUntityID;}


// interface
public:
  BOOL ClientNeedsUpdate(INDEX iClient,ULONG ulEntityID,CNetworkMessage &nmMessage);

  void AddEntity(CEntityPointer* penEntity);
  void RemoveEntity(CEntityPointer* penEntity);

}


#endif // include