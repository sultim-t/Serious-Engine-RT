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

#ifndef SE_INCL_GAME_MENU_H
#define SE_INCL_GAME_MENU_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif


class CGameMenu {
public:
  CListHead gm_lhGadgets;
  CGameMenu *gm_pgmParentMenu;
  BOOL gm_bPopup;
  const char *gm_strName;   // menu name (for mod interface only)
  class CMenuGadget *gm_pmgSelectedByDefault;
  class CMenuGadget *gm_pmgArrowUp;
  class CMenuGadget *gm_pmgArrowDn;
  class CMenuGadget *gm_pmgListTop;
  class CMenuGadget *gm_pmgListBottom;
  INDEX gm_iListOffset;
  INDEX gm_iListWantedItem;   // item you want to focus initially
  INDEX gm_ctListVisible;
  INDEX gm_ctListTotal;
  CGameMenu(void);
  void ScrollList(INDEX iDir);
  void KillAllFocuses(void);
  virtual void Initialize_t(void);
  virtual void Destroy(void);
  virtual void StartMenu(void);
  virtual void FillListItems(void);
  virtual void EndMenu(void);
  // return TRUE if handled
  virtual BOOL OnKeyDown(int iVKey);
  virtual BOOL OnChar(MSG msg);
  virtual void Think(void);
};

#endif  /* include-once check. */