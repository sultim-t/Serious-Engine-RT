/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Base/CTString.h>
#include <Engine/Base/ProgressHook.h>
#include <Engine/Network/Network.h>
#include <Engine/Network/CommunicationInterface.h>

static void (*_pLoadingHook_t)(CProgressHookInfo *pgli) = NULL;  // hook for loading/connecting
static CProgressHookInfo _phiLoadingInfo; // info passed to the hook
BOOL _bRunNetUpdates = FALSE;
static CTimerValue tvLastUpdate;
static BOOL  bTimeInitialized = FALSE;
extern FLOAT net_fSendRetryWait;

// set hook for loading/connecting
void SetProgressHook(void (*pHook)(CProgressHookInfo *pgli))
{
  _pLoadingHook_t = pHook;
}
// call loading/connecting hook
void SetProgressDescription(const CTString &strDescription)
{
  _phiLoadingInfo.phi_strDescription = strDescription;
}

void CallProgressHook_t(FLOAT fCompleted)
{
  if (_pLoadingHook_t!=NULL) {
    _phiLoadingInfo.phi_fCompleted = fCompleted;
    _pLoadingHook_t(&_phiLoadingInfo);

 
    if (!bTimeInitialized) {
      tvLastUpdate = _pTimer->GetHighPrecisionTimer();
      bTimeInitialized = TRUE;
    }
    CTimerValue tvNow = _pTimer->GetHighPrecisionTimer();
    if ((tvNow-tvLastUpdate) > CTimerValue(net_fSendRetryWait*1.1)) {
		  if (_pNetwork->ga_IsServer) {
        // handle server messages
        _cmiComm.Server_Update();
		  } else {
			  // handle client messages
			  _cmiComm.Client_Update();
		  }
      tvLastUpdate = _pTimer->GetHighPrecisionTimer();
    }    

  }
}

