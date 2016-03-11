/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_PLAYERSOURCE_H
#define SE_INCL_PLAYERSOURCE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Synchronization.h>
#include <Engine/Network/NetworkMessage.h>
#include <Engine/Entities/PlayerCharacter.h>

/*
 * Player source, located on client computer; creating actions and receiving updates
 */
class ENGINE_API CPlayerSource {
public:
  INDEX pls_Index;
  BOOL pls_Active;                        // set if this player exists
  CPlayerCharacter pls_pcCharacter;       // this player's character data

  CTCriticalSection pls_csAction;  // access to player action
  CPlayerAction pls_paAction;      // action that this player is currently doing
#define PLS_MAXLASTACTIONS 3
  CPlayerAction pls_apaLastActions[PLS_MAXLASTACTIONS]; // old actions remembered for resending
public:
  /* Activate a new player. */
  void Start_t(CPlayerCharacter &pcCharacter);
  /* Deactivate removed player. */
  void Stop(void);
  /* Check if this player is active. */
  BOOL IsActive(void) { return pls_Active; };
  // request character change for a player
  // NOTE: the request is asynchronious and possible failure cannot be detected
  void ChangeCharacter(CPlayerCharacter &pcNew);

  /* Create action packet from current player commands and for sending to server. */
  void WriteActionPacket(CNetworkMessage &nm);
public:

  CPlayerSource();
  ~CPlayerSource();

  /* Set current player action. */
  void SetAction(const CPlayerAction &paAction);
  // get mask of this player for chat messages
  ULONG GetChatMask(void);
};


#endif  /* include-once check. */

