/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENUMANAGER_H
#define SE_INCL_MENUMANAGER_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "MAudioOptions.h"
#include "MConfirm.h"
#include "MControls.h"
#include "MCustomizeAxis.h"
#include "MCustomizeKeyboard.h"
#include "MCredits.h"
#include "MDisabled.h"
#include "MHighScore.h"
#include "MInGame.h"
#include "MLevels.h"
#include "MMain.h"
#include "MNetwork.h"
#include "MNetworkJoin.h"
#include "MNetworkOpen.h"
#include "MNetworkStart.h"
#include "MOptions.h"
#include "MRenderingOptions.h"
#include "MServers.h"
#include "MSinglePlayer.h"
#include "MSinglePlayerNew.h"
#include "MSplitScreen.h"
#include "MSplitStart.h"
#include "MVar.h"
#include "MVideoOptions.h"


class CMenuManager {
public:
	CConfirmMenu gmConfirmMenu;
	CMainMenu gmMainMenu;
	CInGameMenu gmInGameMenu;
	CSinglePlayerMenu gmSinglePlayerMenu;
	CSinglePlayerNewMenu gmSinglePlayerNewMenu;
	CDisabledMenu gmDisabledFunction;
	CLevelsMenu gmLevelsMenu;
	CVarMenu gmVarMenu;
	CPlayerProfileMenu gmPlayerProfile;
	CControlsMenu gmControls;
	CLoadSaveMenu gmLoadSaveMenu;
	CHighScoreMenu gmHighScoreMenu;
	CCustomizeKeyboardMenu gmCustomizeKeyboardMenu;
	CServersMenu gmServersMenu;
	CCustomizeAxisMenu gmCustomizeAxisMenu;
	COptionsMenu gmOptionsMenu;
	CVideoOptionsMenu gmVideoOptionsMenu;
	CAudioOptionsMenu gmAudioOptionsMenu;
	CNetworkMenu gmNetworkMenu;
	CNetworkJoinMenu gmNetworkJoinMenu;
	CNetworkStartMenu gmNetworkStartMenu;
	CNetworkOpenMenu gmNetworkOpenMenu;
	CSplitScreenMenu gmSplitScreenMenu;
	CSplitStartMenu gmSplitStartMenu;
	CSelectPlayersMenu gmSelectPlayersMenu;
};

extern CMenuManager *_pGUIM; // TODO: Make singleton!


#endif  /* include-once check. */