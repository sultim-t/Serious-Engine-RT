/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_H
#define SE_INCL_MENU_H
#ifdef PRAGMA_ONCE
#pragma once
#endif



// set new thumbnail
void SetThumbnail(CTFileName fn);
// remove thumbnail
void ClearThumbnail(void);

void InitializeMenus( void);
void DestroyMenus( void);
void MenuOnKeyDown( int iVKey);
void MenuOnChar(MSG msg);
void MenuOnMouseMove(PIX pixI, PIX pixJ);
void MenuOnLMBDown(void);
BOOL DoMenu( CDrawPort *pdp); // returns TRUE if still active, FALSE if should quit
void StartMenus( char *str="");
void StopMenus(BOOL bGoToRoot =TRUE);
BOOL IsMenusInRoot(void);
void ChangeToMenu( class CGameMenu *pgmNew);
extern void PlayMenuSound(CSoundData *psd);

#define KEYS_ON_SCREEN 14
#define LEVELS_ON_SCREEN 16
#define SERVERS_ON_SCREEN 15
#define VARS_ON_SCREEN 14

extern CListHead _lhServers;

extern INDEX _iLocalPlayer;

enum GameMode {
  GM_NONE = 0,
  GM_SINGLE_PLAYER,
  GM_NETWORK,
  GM_SPLIT_SCREEN,
  GM_DEMO,
  GM_INTRO,
};
extern GameMode _gmMenuGameMode;
extern GameMode _gmRunningGameMode;

extern CGameMenu *pgmCurrentMenu;

#include "GameMenu.h"

#include "MLoadSave.h"
#include "MPlayerProfile.h"
#include "MSelectPlayers.h"


#endif  /* include-once check. */