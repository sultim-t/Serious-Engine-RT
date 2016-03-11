/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAMEAGENT_H
#define SE_INCL_GAMEAGENT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern CTString ga_strServer;

/// Initialize GameAgent.
extern void GameAgent_ServerInit(void);
/// Let GameAgent know that the server has stopped.
extern void GameAgent_ServerEnd(void);
/// GameAgent server update call which responds to enumeration pings and sends pings to masterserver.
extern void GameAgent_ServerUpdate(void);
/// Notify GameAgent that the server state has changed.
extern void GameAgent_ServerStateChanged(void);

/// Request serverlist enumeration.
extern void GameAgent_EnumTrigger(BOOL bInternet);
/// GameAgent client update for enumeration.
extern void GameAgent_EnumUpdate(void);
/// Cancel the GameAgent serverlist enumeration.
extern void GameAgent_EnumCancel(void);

/// Server request structure. Primarily used for getting server pings.
class CServerRequest {
public:
  ULONG sr_ulAddress;
  USHORT sr_iPort;
  long long sr_tmRequestTime;

public:
  CServerRequest(void);
  ~CServerRequest(void);

  /* Destroy all objects, and reset the array to initial (empty) state. */
  void Clear(void);
};

#endif // include once check
