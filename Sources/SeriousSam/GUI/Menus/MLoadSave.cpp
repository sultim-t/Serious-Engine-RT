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
#include "MenuStuff.h"
#include "MLoadSave.h"


void CLoadSaveMenu::Initialize_t(void)
{
  gm_pgmNextMenu = NULL;

  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  gm_mgNotes.mg_boxOnScreen = BoxMediumRow(10.0);
  gm_mgNotes.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgNotes.mg_iCenterI = -1;
  gm_mgNotes.mg_bEnabled = FALSE;
  gm_mgNotes.mg_bLabel = TRUE;
  gm_lhGadgets.AddTail(gm_mgNotes.mg_lnNode);

  for (INDEX iLabel = 0; iLabel<SAVELOAD_BUTTONS_CT; iLabel++)
  {
    INDEX iPrev = (SAVELOAD_BUTTONS_CT + iLabel - 1) % SAVELOAD_BUTTONS_CT;
    INDEX iNext = (iLabel + 1) % SAVELOAD_BUTTONS_CT;
    // initialize label gadgets
    gm_amgButton[iLabel].mg_pmgUp = &gm_amgButton[iPrev];
    gm_amgButton[iLabel].mg_pmgDown = &gm_amgButton[iNext];
    gm_amgButton[iLabel].mg_boxOnScreen = BoxSaveLoad(iLabel);
    gm_amgButton[iLabel].mg_pActivatedFunction = NULL; // never called!
    gm_amgButton[iLabel].mg_iCenterI = -1;
    gm_lhGadgets.AddTail(gm_amgButton[iLabel].mg_lnNode);
  }

  gm_lhGadgets.AddTail(gm_mgArrowUp.mg_lnNode);
  gm_lhGadgets.AddTail(gm_mgArrowDn.mg_lnNode);
  gm_mgArrowUp.mg_adDirection = AD_UP;
  gm_mgArrowDn.mg_adDirection = AD_DOWN;
  gm_mgArrowUp.mg_boxOnScreen = BoxArrow(AD_UP);
  gm_mgArrowDn.mg_boxOnScreen = BoxArrow(AD_DOWN);
  gm_mgArrowUp.mg_pmgRight = gm_mgArrowUp.mg_pmgDown = &gm_amgButton[0];
  gm_mgArrowDn.mg_pmgRight = gm_mgArrowDn.mg_pmgUp = &gm_amgButton[SAVELOAD_BUTTONS_CT - 1];

  gm_ctListVisible = SAVELOAD_BUTTONS_CT;
  gm_pmgArrowUp = &gm_mgArrowUp;
  gm_pmgArrowDn = &gm_mgArrowDn;
  gm_pmgListTop = &gm_amgButton[0];
  gm_pmgListBottom = &gm_amgButton[SAVELOAD_BUTTONS_CT - 1];
}

void CLoadSaveMenu::StartMenu(void)
{
  gm_bNoEscape = FALSE;

  // delete all file infos
  FORDELETELIST(CFileInfo, fi_lnNode, gm_lhFileInfos, itfi) {
    delete &itfi.Current();
  }

  // list the directory
  CDynamicStackArray<CTFileName> afnmDir;
  MakeDirList(afnmDir, gm_fnmDirectory, "", 0);
  gm_iLastFile = -1;

  // for each file in the directory
  for (INDEX i = 0; i<afnmDir.Count(); i++) {
    CTFileName fnm = afnmDir[i];

    // if it can be parsed
    CTString strName;
    if (ParseFile(fnm, strName)) {
      // create new info for that file
      CFileInfo *pfi = new CFileInfo;
      pfi->fi_fnFile = fnm;
      pfi->fi_strName = strName;
      // add it to list
      gm_lhFileInfos.AddTail(pfi->fi_lnNode);
    }
  }

  // sort if needed
  switch (gm_iSortType) {
  default: ASSERT(FALSE);
  case LSSORT_NONE: break;
  case LSSORT_NAMEUP:
    gm_lhFileInfos.Sort(qsort_CompareFileInfos_NameUp, offsetof(CFileInfo, fi_lnNode));
    break;
  case LSSORT_NAMEDN:
    gm_lhFileInfos.Sort(qsort_CompareFileInfos_NameDn, offsetof(CFileInfo, fi_lnNode));
    break;
  case LSSORT_FILEUP:
    gm_lhFileInfos.Sort(qsort_CompareFileInfos_FileUp, offsetof(CFileInfo, fi_lnNode));
    break;
  case LSSORT_FILEDN:
    gm_lhFileInfos.Sort(qsort_CompareFileInfos_FileDn, offsetof(CFileInfo, fi_lnNode));
    break;
  }

  // if saving
  if (gm_bSave) {
    // add one info as empty slot
    CFileInfo *pfi = new CFileInfo;
    CTString strNumber;
    strNumber.PrintF("%04d", gm_iLastFile + 1);
    pfi->fi_fnFile = gm_fnmDirectory + gm_fnmBaseName + strNumber + gm_fnmExt;
    pfi->fi_strName = EMPTYSLOTSTRING;
    // add it to beginning
    gm_lhFileInfos.AddHead(pfi->fi_lnNode);
  }

  // set default parameters for the list
  gm_iListOffset = 0;
  gm_ctListTotal = gm_lhFileInfos.Count();

  // find which one should be selected
  gm_iListWantedItem = 0;
  if (gm_fnmSelected != "") {
    INDEX i = 0;
    FOREACHINLIST(CFileInfo, fi_lnNode, gm_lhFileInfos, itfi) {
      CFileInfo &fi = *itfi;
      if (fi.fi_fnFile == gm_fnmSelected) {
        gm_iListWantedItem = i;
        break;
      }
      i++;
    }
  }

  CGameMenu::StartMenu();
}

void CLoadSaveMenu::EndMenu(void)
{
  // delete all file infos
  FORDELETELIST(CFileInfo, fi_lnNode, gm_lhFileInfos, itfi) {
    delete &itfi.Current();
  }
  gm_pgmNextMenu = NULL;
  CGameMenu::EndMenu();
}

void CLoadSaveMenu::FillListItems(void)
{
  // disable all items first
  for (INDEX i = 0; i<SAVELOAD_BUTTONS_CT; i++) {
    gm_amgButton[i].mg_bEnabled = FALSE;
    gm_amgButton[i].mg_strText = TRANS("<empty>");
    gm_amgButton[i].mg_strTip = "";
    gm_amgButton[i].mg_iInList = -2;
  }

  BOOL bHasFirst = FALSE;
  BOOL bHasLast = FALSE;
  INDEX ctLabels = gm_lhFileInfos.Count();
  INDEX iLabel = 0;
  FOREACHINLIST(CFileInfo, fi_lnNode, gm_lhFileInfos, itfi) {
    CFileInfo &fi = *itfi;
    INDEX iInMenu = iLabel - gm_iListOffset;
    if ((iLabel >= gm_iListOffset) &&
      (iLabel<(gm_iListOffset + SAVELOAD_BUTTONS_CT)))
    {
      bHasFirst |= (iLabel == 0);
      bHasLast |= (iLabel == ctLabels - 1);
      gm_amgButton[iInMenu].mg_iInList = iLabel;
      gm_amgButton[iInMenu].mg_strDes = fi.fi_strName;
      gm_amgButton[iInMenu].mg_fnm = fi.fi_fnFile;
      gm_amgButton[iInMenu].mg_bEnabled = TRUE;
      gm_amgButton[iInMenu].RefreshText();
      if (gm_bSave) {
        if (!FileExistsForWriting(gm_amgButton[iInMenu].mg_fnm)) {
          gm_amgButton[iInMenu].mg_strTip = TRANS("Enter - save in new slot");
        }
        else {
          gm_amgButton[iInMenu].mg_strTip = TRANS("Enter - save here, F2 - rename, Del - delete");
        }
      }
      else if (gm_bManage) {
        gm_amgButton[iInMenu].mg_strTip = TRANS("Enter - load this, F2 - rename, Del - delete");
      }
      else {
        gm_amgButton[iInMenu].mg_strTip = TRANS("Enter - load this");
      }
    }
    iLabel++;
  }

  // enable/disable up/down arrows
  gm_mgArrowUp.mg_bEnabled = !bHasFirst && ctLabels>0;
  gm_mgArrowDn.mg_bEnabled = !bHasLast  && ctLabels>0;
}

// called to get info of a file from directory, or to skip it
BOOL CLoadSaveMenu::ParseFile(const CTFileName &fnm, CTString &strName)
{
  if (fnm.FileExt() != gm_fnmExt) {
    return FALSE;
  }

  CTFileName fnSaveGameDescription = fnm.NoExt() + ".des";
  try {
    strName.Load_t(fnSaveGameDescription);
  } catch (char *strError) {
    (void)strError;
    strName = fnm.FileName();

    if (fnm.FileExt() == ".ctl") {
      INDEX iCtl = -1;
      strName.ScanF("Controls%d", &iCtl);
      if (iCtl >= 0 && iCtl <= 7) {
        strName.PrintF(TRANS("From player: %s"), _pGame->gm_apcPlayers[iCtl].GetNameForPrinting());
      }
    }
  }

  INDEX iFile = -1;
  fnm.FileName().ScanF((const char*)(gm_fnmBaseName + "%d"), &iFile);

  gm_iLastFile = Max(gm_iLastFile, iFile);

  return TRUE;
}