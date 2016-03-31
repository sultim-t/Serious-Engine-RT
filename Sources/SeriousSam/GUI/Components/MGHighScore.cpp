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
#include "MGHighScore.h"

#define HSCOLUMNS 6
CTString strHighScores[HIGHSCORE_COUNT + 1][HSCOLUMNS];
FLOAT afI[HSCOLUMNS] = {
  0.12f, 0.15f, 0.6f, 0.7f, 0.78f, 0.9f
};


void CMGHighScore::Render(CDrawPort *pdp)
{
  SetFontMedium(pdp);

  COLOR colHeader = LCDGetColor(C_GREEN | 255, "hiscore header");
  COLOR colData = LCDGetColor(C_mdGREEN | 255, "hiscore data");
  COLOR colLastSet = LCDGetColor(C_mlGREEN | 255, "hiscore last set");
  INDEX iLastSet = _pGame->gm_iLastSetHighScore;

  CTString strText;

  strHighScores[0][0] = TRANS("No.");
  strHighScores[0][1] = TRANS("Player Name");
  strHighScores[0][2] = TRANS("Difficulty");
  strHighScores[0][3] = TRANS("Time");
  strHighScores[0][4] = TRANS("Kills");
  strHighScores[0][5] = TRANS("Score");

  {for (INDEX i = 0; i<HIGHSCORE_COUNT; i++) {
    switch (_pGame->gm_ahseHighScores[i].hse_gdDifficulty) {
    default:
      ASSERT(FALSE);
    case (CSessionProperties::GameDifficulty) - 100:
      strHighScores[i + 1][1] = "---";
      continue;
      break;
    case CSessionProperties::GD_TOURIST:
      strHighScores[i + 1][2] = TRANS("Tourist");
      break;
    case CSessionProperties::GD_EASY:
      strHighScores[i + 1][2] = TRANS("Easy");
      break;
    case CSessionProperties::GD_NORMAL:
      strHighScores[i + 1][2] = TRANS("Normal");
      break;
    case CSessionProperties::GD_HARD:
      strHighScores[i + 1][2] = TRANS("Hard");
      break;
    case CSessionProperties::GD_EXTREME:
      strHighScores[i + 1][2] = TRANS("Serious");
      break;
    case CSessionProperties::GD_EXTREME + 1:
      strHighScores[i + 1][2] = TRANS("Mental");
      break;
    }
    strHighScores[i + 1][0].PrintF("%d", i + 1);
    strHighScores[i + 1][1] = _pGame->gm_ahseHighScores[i].hse_strPlayer;
    strHighScores[i + 1][3] = TimeToString(_pGame->gm_ahseHighScores[i].hse_tmTime);
    strHighScores[i + 1][4].PrintF("%03d", _pGame->gm_ahseHighScores[i].hse_ctKills);
    strHighScores[i + 1][5].PrintF("%9d", _pGame->gm_ahseHighScores[i].hse_ctScore);
  }}

  PIX pixJ = pdp->GetHeight()*0.25f;
  {for (INDEX iRow = 0; iRow<HIGHSCORE_COUNT + 1; iRow++) {
    COLOR col = (iRow == 0) ? colHeader : colData;
    if (iLastSet != -1 && iRow - 1 == iLastSet) {
      col = colLastSet;
    }
    {for (INDEX iColumn = 0; iColumn<HSCOLUMNS; iColumn++) {
      PIX pixI = pdp->GetWidth()*afI[iColumn];
      if (iColumn == 1) {
        pdp->PutText(strHighScores[iRow][iColumn], pixI, pixJ, col);
      }
      else {
        pdp->PutTextR(strHighScores[iRow][iColumn], pixI, pixJ, col);
      }
    }}
    if (iRow == 0) {
      pixJ += pdp->GetHeight()*0.06f;
    } else {
      pixJ += pdp->GetHeight()*0.04f;
    }
  }}
}