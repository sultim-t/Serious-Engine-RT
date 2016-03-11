/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Network/NetworkProfile.h>

// profile form for profiling world editing
CNetworkProfile npNetworkProfile;
CProfileForm &_pfNetworkProfile = npNetworkProfile;

CNetworkProfile::CNetworkProfile(void)
 : CProfileForm("Network", "ticks",
    CNetworkProfile::PCI_COUNT, CNetworkProfile::PTI_COUNT)
{
  // initialize network profile form
  SETTIMERNAME(CNetworkProfile::PTI_MAINLOOP,                 "MainLoop()", "");
  SETTIMERNAME(CNetworkProfile::PTI_TIMERLOOP,                "TimerLoop()", "");
  SETTIMERNAME(CNetworkProfile::PTI_SERVER_LOOP,              "ServerLoop()", "");
  SETTIMERNAME(CNetworkProfile::PTI_SESSIONSTATE_LOOP,        "SessionStateLoop()", "");
  SETTIMERNAME(CNetworkProfile::PTI_SESSIONSTATE_PROCESSGAMESTREAM, "CSessionState::ProcessGameStream()", "");
  SETTIMERNAME(CNetworkProfile::PTI_SENDMESSAGE,              "Send()", "");
  SETTIMERNAME(CNetworkProfile::PTI_RECEIVEMESSAGE,           "Receive()", "");

  SETCOUNTERNAME(CNetworkProfile::PCI_GAMESTREAMRESENDS, "game stream resends");

  SETCOUNTERNAME(CNetworkProfile::PCI_GAMESTREAM_BYTES_SENT,     "gamestream bytes sent");
  SETCOUNTERNAME(CNetworkProfile::PCI_GAMESTREAM_BYTES_RECEIVED, "gamestream bytes received");
  SETCOUNTERNAME(CNetworkProfile::PCI_ACTION_BYTES_SENT,         "action bytes sent");
  SETCOUNTERNAME(CNetworkProfile::PCI_ACTION_BYTES_RECEIVED,     "action bytes received");

  SETCOUNTERNAME(CNetworkProfile::PCI_MESSAGESSENT,      "messages sent");
  SETCOUNTERNAME(CNetworkProfile::PCI_MESSAGESRECEIVED,  "messages received");
  SETCOUNTERNAME(CNetworkProfile::PCI_BYTESSENT,         "bytes sent");
  SETCOUNTERNAME(CNetworkProfile::PCI_BYTESRECEIVED,     "bytes received");
}
