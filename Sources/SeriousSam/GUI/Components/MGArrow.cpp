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
#include "MGArrow.h"


void CMGArrow::Render(CDrawPort *pdp)
{
  SetFontMedium(pdp);

  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  COLOR col = GetCurrentColor();

  CTString str;
  if (mg_adDirection == AD_NONE) {
    str = "???";
  } else if (mg_adDirection == AD_UP) {
    str = TRANS("Page Up");
  } else if (mg_adDirection == AD_DOWN) {
    str = TRANS("Page Down");
  } else {
    ASSERT(FALSE);
  }

  PIX pixI = box.Min()(1);
  PIX pixJ = box.Min()(2);
  pdp->PutText(str, pixI, pixJ, col);
}

void CMGArrow::OnActivate(void)
{
  if (mg_adDirection == AD_UP) {
    pgmCurrentMenu->ScrollList(-3);
  }
  else if (mg_adDirection == AD_DOWN) {
    pgmCurrentMenu->ScrollList(+3);
  }
}