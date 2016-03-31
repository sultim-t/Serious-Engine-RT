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

#ifndef SE_INCL_MENU_GADGET_ARROW_H
#define SE_INCL_MENU_GADGET_ARROW_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "ArrowDir.h"
#include "MGButton.h"


class CMGArrow : public CMGButton {
public:
  enum ArrowDir mg_adDirection;
  void Render(CDrawPort *pdp);
  void OnActivate(void);
};

#endif  /* include-once check. */