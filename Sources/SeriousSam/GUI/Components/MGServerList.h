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

#ifndef SE_INCL_MENU_GADGET_SERVERLIST_H
#define SE_INCL_MENU_GADGET_SERVERLIST_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"


class CMGServerList : public CMGButton {
public:
  INDEX mg_iSelected;
  INDEX mg_iFirstOnScreen;
  INDEX mg_ctOnScreen;
  // server list dimensions
  PIX mg_pixMinI;
  PIX mg_pixMaxI;
  PIX mg_pixListMinJ;
  PIX mg_pixListStepJ;
  // header dimensions
  PIX mg_pixHeaderMinJ;
  PIX mg_pixHeaderMidJ;
  PIX mg_pixHeaderMaxJ;
  PIX mg_pixHeaderI[8];
  // scrollbar dimensions
  PIX mg_pixSBMinI;
  PIX mg_pixSBMaxI;
  PIX mg_pixSBMinJ;
  PIX mg_pixSBMaxJ;
  // scrollbar dragging params
  PIX mg_pixDragJ;
  PIX mg_iDragLine;
  PIX mg_pixMouseDrag;
  // current mouse pos
  PIX mg_pixMouseI;
  PIX mg_pixMouseJ;

  INDEX mg_iSort;     // column to sort by
  BOOL mg_bSortDown;  // sort in reverse order

  CMGServerList();
  BOOL OnKeyDown(int iVKey);
  PIXaabbox2D GetScrollBarFullBox(void);
  PIXaabbox2D GetScrollBarHandleBox(void);
  void OnSetFocus(void);
  void OnKillFocus(void);
  void Render(CDrawPort *pdp);
  void AdjustFirstOnScreen(void);
  void OnMouseOver(PIX pixI, PIX pixJ);
};

#endif  /* include-once check. */