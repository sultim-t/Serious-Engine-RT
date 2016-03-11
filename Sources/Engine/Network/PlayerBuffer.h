/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_PLAYERBUFFER_H
#define SE_INCL_PLAYERBUFFER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Network/NetworkMessage.h>
#include <Engine/Entities/PlayerCharacter.h>
#include <Engine/Network/ActionBuffer.h>

/*
 * Player buffer, located on server; buffers action messages
 */
class ENGINE_API CPlayerBuffer {
public:
  BOOL plb_Active;                      // set if this player exists
  INDEX plb_Index;
  INDEX plb_iClient;                    // client that controls this player
  INDEX plb_iPing;  // ping in milliseconds
  CPlayerAction plb_paLastAction;       // last action sent (used for delta-packing)
  CActionBuffer plb_abReceived;         // buffer of actions that were received, but not sent yet
  CPlayerCharacter plb_pcCharacter;     // this player's character data
public:

  /* Default constructor. */
  CPlayerBuffer(void);
  /* Destructor. */
  ~CPlayerBuffer(void);

  /* Activate player buffer for a new player. */
  void Activate(INDEX iClient);
  /* Deactivate player data for removed player. */
  void Deactivate(void);
  /* Check if this player is active. */
  BOOL IsActive(void) { return plb_Active; };

  /* Receive action packet from player source. */
  void ReceiveActionPacket(CNetworkMessage *pnm, INDEX iMaxBuffer);
  /* Create action packet for player target from oldest buffered action. */
  // (prepares lag info for given client number)
  void CreateActionPacket(CNetworkMessage *pnm, INDEX iClient);
  /* Advance action buffer by one tick by removing oldest action. */
  void AdvanceActionBuffer(void);
};


#endif  /* include-once check. */

