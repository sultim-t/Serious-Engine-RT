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

#ifndef SE_INCL_MENU_GADGET_EDIT_H
#define SE_INCL_MENU_GADGET_EDIT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"


class CMGEdit : public CMGButton {
public:
  INDEX mg_ctMaxStringLen;
  CTString *mg_pstrToChange;

  CMGEdit(void);

  // return TRUE if handled
  BOOL OnKeyDown(int iVKey);
  BOOL OnChar(MSG msg);
  void Clear(void);
  void OnActivate(void);
  void OnKillFocus(void);
  void Render(CDrawPort *pdp);
  virtual void OnStringChanged(void);
  virtual void OnStringCanceled(void);
};

#endif  /* include-once check. */