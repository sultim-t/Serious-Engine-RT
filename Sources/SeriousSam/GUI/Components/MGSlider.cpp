/* /* Copyright (c) 2002-2012 Croteam Ltd. 
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
#include "MGSlider.h"

extern PIX  _pixCursorPosI;
extern PIX  _pixCursorPosJ;
extern INDEX sam_bWideScreen;


CMGSlider::CMGSlider()
{
  mg_iMinPos = 0;
  mg_iMaxPos = 16;
  mg_iCurPos = 8;
  mg_pOnSliderChange = NULL;
  mg_fFactor = 1.0f;
}

void CMGSlider::ApplyCurrentPosition(void)
{
  mg_iCurPos = Clamp(mg_iCurPos, mg_iMinPos, mg_iMaxPos);
  FLOAT fStretch = FLOAT(mg_iCurPos) / (mg_iMaxPos - mg_iMinPos);
  mg_fFactor = fStretch;

  if (mg_pOnSliderChange != NULL) {
    mg_pOnSliderChange(mg_iCurPos);
  }
}

void CMGSlider::ApplyGivenPosition(INDEX iMin, INDEX iMax, INDEX iCur)
{
  mg_iMinPos = iMin;
  mg_iMaxPos = iMax;
  mg_iCurPos = iCur;
  ApplyCurrentPosition();
}


BOOL CMGSlider::OnKeyDown(int iVKey)
{
  // if scrolling left
  if ((iVKey == VK_BACK || iVKey == VK_LEFT) && mg_iCurPos>mg_iMinPos) {
    mg_iCurPos--;
    ApplyCurrentPosition();
    return TRUE;
    // if scrolling right
  } else if ((iVKey == VK_RETURN || iVKey == VK_RIGHT) && mg_iCurPos<mg_iMaxPos) {
    mg_iCurPos++;
    ApplyCurrentPosition();
    return TRUE;
    // if lmb pressed
  } else if (iVKey == VK_LBUTTON) {
    // get position of slider box on screen
    PIXaabbox2D boxSlider = GetSliderBox();
    // if mouse is within
    if (boxSlider >= PIX2D(_pixCursorPosI, _pixCursorPosJ)) {
      // set new position exactly where mouse pointer is
      FLOAT fRatio = FLOAT(_pixCursorPosI - boxSlider.Min()(1)) / boxSlider.Size()(1);
      fRatio = (fRatio - 0.01f) / (0.99f - 0.01f);
      fRatio = Clamp(fRatio, 0.0f, 1.0f);
      mg_iCurPos = fRatio*(mg_iMaxPos - mg_iMinPos) + mg_iMinPos;
      ApplyCurrentPosition();
      return TRUE;
    }
  }
  return CMenuGadget::OnKeyDown(iVKey);
}

PIXaabbox2D CMGSlider::GetSliderBox(void)
{
  extern CDrawPort *pdp;
  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIR = box.Min()(1) + box.Size()(1)*0.55f;
  PIX pixJ = box.Min()(2);
  PIX pixJSize = box.Size()(2)*0.95f;
  PIX pixISizeR = box.Size()(1)*0.45f;
  if (sam_bWideScreen) pixJSize++;
  return PIXaabbox2D(PIX2D(pixIR + 1, pixJ + 1), PIX2D(pixIR + pixISizeR - 2, pixJ + pixJSize - 2));
}

void CMGSlider::Render(CDrawPort *pdp)
{
  SetFontMedium(pdp);

  // get geometry
  COLOR col = GetCurrentColor();
  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIL = box.Min()(1) + box.Size()(1)*0.45f;
  PIX pixIR = box.Min()(1) + box.Size()(1)*0.55f;
  PIX pixJ = box.Min()(2);
  PIX pixJSize = box.Size()(2)*0.95f;
  PIX pixISizeR = box.Size()(1)*0.45f;
  if (sam_bWideScreen) pixJSize++;

  // print text left of slider
  pdp->PutTextR(mg_strText, pixIL, pixJ, col);

  // draw box around slider
  LCDDrawBox(0, -1, PIXaabbox2D(PIX2D(pixIR + 1, pixJ), PIX2D(pixIR + pixISizeR - 2, pixJ + pixJSize - 2)),
    LCDGetColor(C_GREEN | 255, "slider box"));

  // draw filled part of slider
  pdp->Fill(pixIR + 2, pixJ + 1, (pixISizeR - 5)*mg_fFactor, (pixJSize - 4), col);

  // print percentage text
  CTString strPercentage;
  strPercentage.PrintF("%d%%", (int)floor(mg_fFactor * 100 + 0.5f));
  pdp->PutTextC(strPercentage, pixIR + pixISizeR / 2, pixJ + 1, col);
}