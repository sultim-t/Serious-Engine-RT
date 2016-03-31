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

#include "StdH.h"
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include "MHighScore.h"


void CHighScoreMenu::Initialize_t(void)
{
  gm_mgHScore.mg_boxOnScreen = FLOATaabbox2D(FLOAT2D(0, 0), FLOAT2D(1, 0.5));
  gm_lhGadgets.AddTail(gm_mgHScore.mg_lnNode);

  gm_mgTitle.mg_strText = TRANS("HIGH SCORE TABLE");
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);
}

void CHighScoreMenu::StartMenu(void)
{
  gm_pgmParentMenu = pgmCurrentMenu;
  CGameMenu::StartMenu();
}