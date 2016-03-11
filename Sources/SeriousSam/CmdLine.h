/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_CMDLINE_H
#define SE_INCL_CMDLINE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern CTString cmd_strWorld;       // world to load
extern INDEX cmd_iGoToMarker;       // marker to go to
extern CTString cmd_strScript;      // script to execute
extern CTString cmd_strServer;      // server to connect to
extern INDEX cmd_iPort;             // port to connect to
extern CTString cmd_strPassword;    // network password
extern CTString cmd_strOutput;      // output from parsing command line
extern BOOL cmd_bServer;            // set to run as server
extern BOOL cmd_bQuickJoin;         // do not ask for players and network settings

void ParseCommandLine(CTString strCmd);


#endif  /* include-once check. */

