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
#include "MGTrigger.h"


INDEX GetNewLoopValue(int iVKey, INDEX iCurrent, INDEX ctMembers)
{
  INDEX iPrev = (iCurrent + ctMembers - 1) % ctMembers;
  INDEX iNext = (iCurrent + 1) % ctMembers;
  // return and right arrow set new text
  if (iVKey == VK_RETURN || iVKey == VK_LBUTTON || iVKey == VK_RIGHT)
  {
    return iNext;
  // left arrow and backspace sets prev text
  } else if ((iVKey == VK_BACK || iVKey == VK_RBUTTON) || (iVKey == VK_LEFT)) {
    return iPrev;
  }
  return iCurrent;
}

CMGTrigger::CMGTrigger(void)
{
  mg_pPreTriggerChange = NULL;
  mg_pOnTriggerChange = NULL;
  mg_iCenterI = 0;
  mg_bVisual = FALSE;
}

void CMGTrigger::ApplyCurrentSelection(void)
{
  mg_iSelected = Clamp(mg_iSelected, 0L, mg_ctTexts - 1L);
  mg_strValue = mg_astrTexts[mg_iSelected];
}

void CMGTrigger::OnSetNextInList(int iVKey)
{
  if (mg_pPreTriggerChange != NULL) {
    mg_pPreTriggerChange(mg_iSelected);
  }

  mg_iSelected = GetNewLoopValue(iVKey, mg_iSelected, mg_ctTexts);
  mg_strValue = mg_astrTexts[mg_iSelected];

  if (mg_pOnTriggerChange != NULL) {
    (*mg_pOnTriggerChange)(mg_iSelected);
  }
}

BOOL CMGTrigger::OnKeyDown(int iVKey)
{
  if ((iVKey == VK_RETURN || iVKey == VK_LBUTTON) ||
    (iVKey == VK_LEFT) ||
    (iVKey == VK_BACK || iVKey == VK_RBUTTON) ||
    (iVKey == VK_RIGHT))
  {
    // key is handled
    if (mg_bEnabled) OnSetNextInList(iVKey);
    return TRUE;
  }
  // key is not handled
  return FALSE;
}

void CMGTrigger::Render(CDrawPort *pdp)
{
  SetFontMedium(pdp);

  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIL = box.Min()(1) + box.Size()(1)*0.45f;
  PIX pixIR = box.Min()(1) + box.Size()(1)*0.55f;
  PIX pixJ = box.Min()(2);

  COLOR col = GetCurrentColor();
  if (!mg_bVisual || mg_strValue == "") {
    CTString strValue = mg_strValue;
    if (mg_bVisual) {
      strValue = TRANS("none");
    }

    if (mg_iCenterI == -1) {
      pdp->PutText(mg_strLabel, box.Min()(1), pixJ, col);
      pdp->PutTextR(strValue, box.Max()(1), pixJ, col);
    } else {
      pdp->PutTextR(mg_strLabel, pixIL, pixJ, col);
      pdp->PutText(strValue, pixIR, pixJ, col);
    }
  } else {
    CTString strLabel = mg_strLabel + ": ";
    pdp->PutText(strLabel, box.Min()(1), pixJ, col);
    CTextureObject to;
    try {
      to.SetData_t(mg_strValue);
      CTextureData *ptd = (CTextureData *)to.GetData();
      PIX pixSize = box.Size()(2);
      PIX pixCX = box.Max()(1) - pixSize / 2;
      PIX pixCY = box.Center()(2);
      pdp->PutTexture(&to, PIXaabbox2D(
        PIX2D(pixCX - pixSize / 2, pixCY - pixSize / 2),
        PIX2D(pixCX - pixSize / 2 + pixSize, pixCY - pixSize / 2 + pixSize)), C_WHITE | 255);
    } catch (char *strError) {
      CPrintF("%s\n", strError);
    }
    to.SetData(NULL);
  }
}