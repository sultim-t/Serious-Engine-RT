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

#ifndef SE_INCL_GAME_MENU_INGAME_H
#define SE_INCL_GAME_MENU_INGAME_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGTitle.h"


class CInGameMenu : public CGameMenu {
public:
  CMGTitle gm_mgTitle;
  CMGButton gm_mgLabel1;
  CMGButton gm_mgLabel2;
  CMGButton gm_mgQuickLoad;
  CMGButton gm_mgQuickSave;
  CMGButton gm_mgLoad;
  CMGButton gm_mgSave;
  CMGButton gm_mgDemoRec;
  CMGButton gm_mgHighScore;
  CMGButton gm_mgOptions;
  CMGButton gm_mgStop;
  CMGButton gm_mgQuit;

  void Initialize_t(void);
  void StartMenu(void);
};

#endif  /* include-once check. */