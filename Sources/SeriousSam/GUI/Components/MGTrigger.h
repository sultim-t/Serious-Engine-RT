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

#ifndef SE_INCL_MENU_GADGET_TRIGGER_H
#define SE_INCL_MENU_GADGET_TRIGGER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MenuGadget.h"


class CMGTrigger : public CMenuGadget {
public:
  CTString mg_strLabel;
  CTString mg_strValue;
  CTString *mg_astrTexts;
  INDEX mg_ctTexts;
  INDEX mg_iSelected;
  INDEX mg_iCenterI;
  BOOL mg_bVisual;

  CMGTrigger(void);

  void ApplyCurrentSelection(void);
  void OnSetNextInList(int iVKey);
  void(*mg_pPreTriggerChange)(INDEX iCurrentlySelected);
  void(*mg_pOnTriggerChange)(INDEX iCurrentlySelected);

  // return TRUE if handled
  BOOL OnKeyDown(int iVKey);
  void Render(CDrawPort *pdp);
};

#endif  /* include-once check. */