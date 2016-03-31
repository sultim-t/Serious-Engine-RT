/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_GAME_MENU_SELECTPLAYERS_H
#define SE_INCL_GAME_MENU_SELECTPLAYERS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGChangePlayer.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGTrigger.h"


class CSelectPlayersMenu : public CGameMenu {
public:
  BOOL gm_bAllowDedicated;
  BOOL gm_bAllowObserving;

  CMGTitle gm_mgTitle;

  CMGTrigger gm_mgDedicated;
  CMGTrigger gm_mgObserver;
  CMGTrigger gm_mgSplitScreenCfg;

  CMGChangePlayer gm_mgPlayer0Change;
  CMGChangePlayer gm_mgPlayer1Change;
  CMGChangePlayer gm_mgPlayer2Change;
  CMGChangePlayer gm_mgPlayer3Change;

  CMGButton gm_mgNotes;

  CMGButton gm_mgStart;

  void Initialize_t(void);
  void StartMenu(void);
  void EndMenu(void);
};

#endif  /* include-once check. */