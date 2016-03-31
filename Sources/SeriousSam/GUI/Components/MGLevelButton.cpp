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
#include <Engine/Base/KeyNames.h>
#include <Engine/CurrentVersion.h>
#include <GameMP/LCDDrawing.h>
#include "MGLevelButton.h"

extern CSoundData *_psdPress;


void CMGLevelButton::OnActivate(void)
{
  PlayMenuSound(_psdPress);
  IFeel_PlayEffect("Menu_press");
  _pGame->gam_strCustomLevel = mg_fnmLevel;
  extern void(*_pAfterLevelChosen)(void);
  _pAfterLevelChosen();
}

void CMGLevelButton::OnSetFocus(void)
{
  SetThumbnail(mg_fnmLevel);
  CMGButton::OnSetFocus();
}