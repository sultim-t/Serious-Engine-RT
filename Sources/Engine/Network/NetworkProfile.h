/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef __ENGINE_BASE_PROFILING_H__
#include <Engine/Base/Profiling.h>
#endif

/* Class for holding profiling information for network. */
class CNetworkProfile : public CProfileForm {
public:
  // indices for profiling counters and timers
  enum ProfileTimerIndex {
    PTI_MAINLOOP,                 // time spent in main game loop
    PTI_TIMERLOOP,                // time spent in timer game loop

    PTI_SERVER_LOOP,              // time server spent processing messages
    PTI_SESSIONSTATE_LOOP,        // time session state spent processing messages
    PTI_SESSIONSTATE_PROCESSGAMESTREAM, // time session state spent processing gamestream (includes physics)

    PTI_SENDMESSAGE,              // time spend sending message
    PTI_RECEIVEMESSAGE,           // time spend receiving message
    PTI_COUNT
  };
  enum ProfileCounterIndex {
    PCI_GAMESTREAMRESENDS,  // how many times gamestream block was resent from server

    PCI_GAMESTREAM_BYTES_SENT,      // bytes sent in gamestream messages
    PCI_GAMESTREAM_BYTES_RECEIVED,  // bytes received in gamestream messages
    PCI_ACTION_BYTES_SENT,          // bytes sent in action messages
    PCI_ACTION_BYTES_RECEIVED,      // bytes received in action messages

    PCI_MESSAGESSENT,       // total number of messages sent
    PCI_MESSAGESRECEIVED,   // total number of messages received
    PCI_BYTESSENT,          // total number of bytes sent
    PCI_BYTESRECEIVED,      // total number of bytes received
    PCI_COUNT
  };
  // constructor
  CNetworkProfile(void);
};
