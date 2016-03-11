/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Base/KeyNames.h>
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
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
	}
	else if (mg_adDirection == AD_UP) {
		str = TRANS("Page Up");
	}
	else if (mg_adDirection == AD_DOWN) {
		str = TRANS("Page Down");
	}
	else {
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