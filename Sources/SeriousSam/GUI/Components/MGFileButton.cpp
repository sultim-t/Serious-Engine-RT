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
#include "MGFileButton.h"
#include "GUI/Menus/MenuManager.h"

extern CSoundData *_psdPress;


CMGFileButton::CMGFileButton(void)
{
  mg_iState = FBS_NORMAL;
}

// refresh current text from description
void CMGFileButton::RefreshText(void)
{
  mg_strText = mg_strDes;
  mg_strText.OnlyFirstLine();
  mg_strInfo = mg_strDes;
  mg_strInfo.RemovePrefix(mg_strText);
  mg_strInfo.DeleteChar(0);
}

void CMGFileButton::SaveDescription(void)
{
  CTFileName fnFileNameDescription = mg_fnm.NoExt() + ".des";
  try {
    mg_strDes.Save_t(fnFileNameDescription);
  } catch (char *strError) {
    CPrintF("%s\n", strError);
  }
}

CMGFileButton *_pmgFileToSave = NULL;
void OnFileSaveOK(void)
{
  if (_pmgFileToSave != NULL) {
    _pmgFileToSave->SaveYes();
  }
}

void CMGFileButton::DoSave(void)
{
  if (FileExistsForWriting(mg_fnm)) {
    _pmgFileToSave = this;
    extern void SaveConfirm(void);
    SaveConfirm();
  } else {
    SaveYes();
  }
}

void CMGFileButton::SaveYes(void)
{
  ASSERT(_pGUIM->gmLoadSaveMenu.gm_bSave);
  // call saving function
  BOOL bSucceeded = _pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen(mg_fnm);
  // if saved
  if (bSucceeded) {
    // save the description too
    SaveDescription();
  }
}

void CMGFileButton::DoLoad(void)
{
  ASSERT(!_pGUIM->gmLoadSaveMenu.gm_bSave);
  // if no file
  if (!FileExists(mg_fnm)) {
    // do nothing
    return;
  }
  if (_pGUIM->gmLoadSaveMenu.gm_pgmNextMenu != NULL) {
    _pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = _pGUIM->gmLoadSaveMenu.gm_pgmNextMenu;
  }
  // call loading function
  BOOL bSucceeded = _pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen(mg_fnm);
  ASSERT(bSucceeded);
}

static CTString _strTmpDescription;
static CTString _strOrgDescription;

void CMGFileButton::StartEdit(void)
{
  CMGEdit::OnActivate();
}

void CMGFileButton::OnActivate(void)
{
  if (mg_fnm == "") {
    return;
  }

  PlayMenuSound(_psdPress);
  IFeel_PlayEffect("Menu_press");

  // if loading
  if (!_pGUIM->gmLoadSaveMenu.gm_bSave) {
    // load now
    DoLoad();
    // if saving
  } else {
    // switch to editing mode
    BOOL bWasEmpty = mg_strText == EMPTYSLOTSTRING;
    mg_strDes = _pGUIM->gmLoadSaveMenu.gm_strSaveDes;
    RefreshText();
    _strOrgDescription = _strTmpDescription = mg_strText;

    if (bWasEmpty) {
      _strOrgDescription = EMPTYSLOTSTRING;
    }

    mg_pstrToChange = &_strTmpDescription;
    StartEdit();
    mg_iState = FBS_SAVENAME;
  }
}

BOOL CMGFileButton::OnKeyDown(int iVKey)
{
  if (mg_iState == FBS_NORMAL) {
    if (_pGUIM->gmLoadSaveMenu.gm_bSave || _pGUIM->gmLoadSaveMenu.gm_bManage) {
      if (iVKey == VK_F2) {
        if (FileExistsForWriting(mg_fnm)) {
          // switch to renaming mode
          _strOrgDescription = mg_strText;
          _strTmpDescription = mg_strText;
          mg_pstrToChange = &_strTmpDescription;
          StartEdit();
          mg_iState = FBS_RENAME;
        }
        return TRUE;

      } else if (iVKey == VK_DELETE) {
        if (FileExistsForWriting(mg_fnm)) {
          // delete the file, its description and thumbnail
          RemoveFile(mg_fnm);
          RemoveFile(mg_fnm.NoExt() + ".des");
          RemoveFile(mg_fnm.NoExt() + "Tbn.tex");
          // refresh menu
          _pGUIM->gmLoadSaveMenu.EndMenu();
          _pGUIM->gmLoadSaveMenu.StartMenu();
          OnSetFocus();
        }
        return TRUE;
      }
    }
    return CMenuGadget::OnKeyDown(iVKey);
  } else {
    // go out of editing mode
    if (mg_bEditing) {
      if (iVKey == VK_UP || iVKey == VK_DOWN) {
        CMGEdit::OnKeyDown(VK_ESCAPE);
      }
    }
    return CMGEdit::OnKeyDown(iVKey);
  }
}

void CMGFileButton::OnSetFocus(void)
{
  mg_iState = FBS_NORMAL;

  if (_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails && mg_bEnabled) {
    SetThumbnail(mg_fnm);
  } else {
    ClearThumbnail();
  }

  pgmCurrentMenu->KillAllFocuses();
  CMGButton::OnSetFocus();
}

void CMGFileButton::OnKillFocus(void)
{
  // go out of editing mode
  if (mg_bEditing) {
    OnKeyDown(VK_ESCAPE);
  }

  CMGEdit::OnKillFocus();
}

// override from edit gadget
void CMGFileButton::OnStringChanged(void)
{
  // if saving
  if (mg_iState == FBS_SAVENAME) {
    // do the save
    mg_strDes = _strTmpDescription + "\n" + mg_strInfo;
    DoSave();
  // if renaming
  } else if (mg_iState == FBS_RENAME) {
    // do the rename
    mg_strDes = _strTmpDescription + "\n" + mg_strInfo;
    SaveDescription();
    // refresh menu
    _pGUIM->gmLoadSaveMenu.EndMenu();
    _pGUIM->gmLoadSaveMenu.StartMenu();
    OnSetFocus();
  }
}
void CMGFileButton::OnStringCanceled(void)
{
  mg_strText = _strOrgDescription;
}

void CMGFileButton::Render(CDrawPort *pdp)
{
  // render original gadget first
  CMGEdit::Render(pdp);

  // if currently selected
  if (mg_bFocused && mg_bEnabled) {
    // add info at the bottom if screen
    SetFontMedium(pdp);

    PIXaabbox2D box = FloatBoxToPixBox(pdp, BoxSaveLoad(15.0));
    PIX pixI = box.Min()(1);
    PIX pixJ = box.Min()(2);

    COLOR col = LCDGetColor(C_mlGREEN | 255, "file info");
    pdp->PutText(mg_strInfo, pixI, pixJ, col);
  }
}