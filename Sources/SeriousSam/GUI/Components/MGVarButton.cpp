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
#include "VarList.h"
#include "MGVarButton.h"

extern PIX  _pixCursorPosI;
extern PIX  _pixCursorPosJ;


BOOL CMGVarButton::IsSeparator(void)
{
  if (mg_pvsVar == NULL) return FALSE;
  return mg_pvsVar->vs_bSeparator;
}

BOOL CMGVarButton::IsEnabled(void)
{
  return(_gmRunningGameMode == GM_NONE
    || mg_pvsVar == NULL
    || mg_pvsVar->vs_bCanChangeInGame);
}

// return slider position on scren
PIXaabbox2D CMGVarButton::GetSliderBox(void)
{
  extern CDrawPort *pdp;
  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIR = box.Min()(1) + box.Size()(1)*0.55f;
  PIX pixJ = box.Min()(2);
  PIX pixISize = box.Size()(1)*0.13f;
  PIX pixJSize = box.Size()(2);
  return PIXaabbox2D(PIX2D(pixIR, pixJ + 1), PIX2D(pixIR + pixISize - 4, pixJ + pixJSize - 6));
}

extern BOOL _bVarChanged;
BOOL CMGVarButton::OnKeyDown(int iVKey)
{
  if (mg_pvsVar == NULL || mg_pvsVar->vs_bSeparator || !mg_pvsVar->Validate() || !mg_bEnabled) {
    return CMenuGadget::OnKeyDown(iVKey);
  }

  // handle slider
  if (mg_pvsVar->vs_iSlider && !mg_pvsVar->vs_bCustom) {
    // ignore RMB
    if (iVKey == VK_RBUTTON) return TRUE;
    // handle LMB
    if (iVKey == VK_LBUTTON) {
      // get position of slider box on screen
      PIXaabbox2D boxSlider = GetSliderBox();
      // if mouse is within
      if (boxSlider >= PIX2D(_pixCursorPosI, _pixCursorPosJ)) {
        // set new position exactly where mouse pointer is
        mg_pvsVar->vs_iValue = (FLOAT)(_pixCursorPosI - boxSlider.Min()(1)) / boxSlider.Size()(1) * (mg_pvsVar->vs_ctValues);
        _bVarChanged = TRUE;
      }
      // handled
      return TRUE;
    }
  }

  if (iVKey == VK_RETURN) {
    FlushVarSettings(TRUE);
    void MenuGoToParent(void);
    MenuGoToParent();
    return TRUE;
  }

  if (iVKey == VK_LBUTTON || iVKey == VK_RIGHT) {
    if (mg_pvsVar != NULL) {
      INDEX iOldValue = mg_pvsVar->vs_iValue;
      mg_pvsVar->vs_iValue++;
      if (mg_pvsVar->vs_iValue >= mg_pvsVar->vs_ctValues) {
        // wrap non-sliders, clamp sliders
        if (mg_pvsVar->vs_iSlider) mg_pvsVar->vs_iValue = mg_pvsVar->vs_ctValues - 1L;
        else mg_pvsVar->vs_iValue = 0;
      }
      if (iOldValue != mg_pvsVar->vs_iValue) {
        _bVarChanged = TRUE;
        mg_pvsVar->vs_bCustom = FALSE;
        mg_pvsVar->Validate();
      }
    }
    return TRUE;
  }

  if (iVKey == VK_LEFT || iVKey == VK_RBUTTON) {
    if (mg_pvsVar != NULL) {
      INDEX iOldValue = mg_pvsVar->vs_iValue;
      mg_pvsVar->vs_iValue--;
      if (mg_pvsVar->vs_iValue<0) {
        // wrap non-sliders, clamp sliders
        if (mg_pvsVar->vs_iSlider) mg_pvsVar->vs_iValue = 0;
        else mg_pvsVar->vs_iValue = mg_pvsVar->vs_ctValues - 1L;
      }
      if (iOldValue != mg_pvsVar->vs_iValue) {
        _bVarChanged = TRUE;
        mg_pvsVar->vs_bCustom = FALSE;
        mg_pvsVar->Validate();
      }
    }
    return TRUE;
  }

  // not handled
  return CMenuGadget::OnKeyDown(iVKey);
}

void CMGVarButton::Render(CDrawPort *pdp)
{
  if (mg_pvsVar == NULL) {
    return;
  }

  SetFontMedium(pdp);

  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIL = box.Min()(1) + box.Size()(1)*0.45f;
  PIX pixIR = box.Min()(1) + box.Size()(1)*0.55f;
  PIX pixIC = box.Center()(1);
  PIX pixJ = box.Min()(2);

  if (mg_pvsVar->vs_bSeparator)
  {
    mg_bEnabled = FALSE;
    COLOR col = LCDGetColor(C_WHITE | 255, "separator");
    CTString strText = mg_pvsVar->vs_strName;
    pdp->PutTextC(strText, pixIC, pixJ, col);
  } else if (mg_pvsVar->Validate()) {
    // check whether the variable is disabled
    if (mg_pvsVar->vs_strFilter != "") mg_bEnabled = _pShell->GetINDEX(mg_pvsVar->vs_strFilter);
    COLOR col = GetCurrentColor();
    pdp->PutTextR(mg_pvsVar->vs_strName, pixIL, pixJ, col);
    // custom is by default
    CTString strText = TRANS("Custom");
    if (!mg_pvsVar->vs_bCustom)
    { // not custom!
      strText = mg_pvsVar->vs_astrTexts[mg_pvsVar->vs_iValue];
      // need slider?
      if (mg_pvsVar->vs_iSlider>0) {
        // draw box around slider
        PIX pixISize = box.Size()(1)*0.13f;
        PIX pixJSize = box.Size()(2);
        LCDDrawBox(0, -1, PIXaabbox2D(PIX2D(pixIR, pixJ + 1), PIX2D(pixIR + pixISize - 4, pixJ + pixJSize - 6)),
          LCDGetColor(C_GREEN | 255, "slider box"));

        // draw filled part of slider
        if (mg_pvsVar->vs_iSlider == 1) {
          // fill slider
          FLOAT fFactor = (FLOAT)(mg_pvsVar->vs_iValue + 1) / mg_pvsVar->vs_ctValues;
          pdp->Fill(pixIR + 1, pixJ + 2, (pixISize - 6)*fFactor, pixJSize - 9, col);
        } else {
          // ratio slider
          ASSERT(mg_pvsVar->vs_iSlider == 2);
          FLOAT fUnitWidth = (FLOAT)(pixISize - 5) / mg_pvsVar->vs_ctValues;
          pdp->Fill(pixIR + 1 + (mg_pvsVar->vs_iValue*fUnitWidth), pixJ + 2, fUnitWidth, pixJSize - 9, col);
        }
        // move text printout to the right of slider
        pixIR += box.Size()(1)*0.15f;
      }
    }
    // write right text
    pdp->PutText(strText, pixIR, pixJ, col);
  }
}