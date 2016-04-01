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
#include <Engine/Build.h>

#include "MenuStuff.h"

#define RADIOTRANS(str) ("ETRS" str)

extern CTString astrNoYes[] = {
  RADIOTRANS("No"),
  RADIOTRANS("Yes"),
};

extern CTString astrComputerInvoke[] = {
  RADIOTRANS("Use"),
  RADIOTRANS("Double-click use"),
};

extern CTString astrWeapon[] = {
  RADIOTRANS("Only if new"),
  RADIOTRANS("Never"),
  RADIOTRANS("Always"),
  RADIOTRANS("Only if stronger"),
};

extern PIX apixWidths[][2] = {
  320, 240,
  400, 300,
  512, 384,
  640, 240,
  640, 480,
  720, 540,
  800, 300,
  800, 600,
  960, 720,
  1024, 384,
  1024, 768,
  1152, 864,
  1280, 480,
  1280, 960,
  1600, 600,
  1600, 1200,
  1920, 720,
  1920, 1440,
  2048, 786,
  2048, 1536,
};

extern CTString astrCrosshair[] = {
  "",
  "Textures\\Interface\\Crosshairs\\Crosshair1.tex",
  "Textures\\Interface\\Crosshairs\\Crosshair2.tex",
  "Textures\\Interface\\Crosshairs\\Crosshair3.tex",
  "Textures\\Interface\\Crosshairs\\Crosshair4.tex",
  "Textures\\Interface\\Crosshairs\\Crosshair5.tex",
  "Textures\\Interface\\Crosshairs\\Crosshair6.tex",
  "Textures\\Interface\\Crosshairs\\Crosshair7.tex",
};

extern CTString astrMaxPlayersRadioTexts[] = {
  RADIOTRANS("2"),
  RADIOTRANS("3"),
  RADIOTRANS("4"),
  RADIOTRANS("5"),
  RADIOTRANS("6"),
  RADIOTRANS("7"),
  RADIOTRANS("8"),
  RADIOTRANS("9"),
  RADIOTRANS("10"),
  RADIOTRANS("11"),
  RADIOTRANS("12"),
  RADIOTRANS("13"),
  RADIOTRANS("14"),
  RADIOTRANS("15"),
  RADIOTRANS("16"),
};
// here, we just reserve space for up to 16 different game types
// actual names are added later
extern CTString astrGameTypeRadioTexts[] = {
  "", "", "", "", "",
  "", "", "", "", "",
  "", "", "", "", "",
  "", "", "", "", "",
};

extern INDEX ctGameTypeRadioTexts = 1;

extern CTString astrDifficultyRadioTexts[] = {
  RADIOTRANS("Tourist"),
  RADIOTRANS("Easy"),
  RADIOTRANS("Normal"),
  RADIOTRANS("Hard"),
  RADIOTRANS("Serious"),
  RADIOTRANS("Mental"),
};

extern CTString astrSplitScreenRadioTexts[] = {
  RADIOTRANS("1"),
  RADIOTRANS("2 - split screen"),
  RADIOTRANS("3 - split screen"),
  RADIOTRANS("4 - split screen"),
};

extern CTString astrDisplayPrefsRadioTexts[] = {
  RADIOTRANS("Speed"),
  RADIOTRANS("Normal"),
  RADIOTRANS("Quality"),
  RADIOTRANS("Custom"),
};

extern CTString astrDisplayAPIRadioTexts[] = {
  RADIOTRANS("OpenGL"),
  RADIOTRANS("Direct3D"),
};

extern CTString astrBitsPerPixelRadioTexts[] = {
  RADIOTRANS("Desktop"),
  RADIOTRANS("16 BPP"),
  RADIOTRANS("32 BPP"),
};

extern CTString astrFrequencyRadioTexts[] = {
  RADIOTRANS("No sound"),
  RADIOTRANS("11kHz"),
  RADIOTRANS("22kHz"),
  RADIOTRANS("44kHz"),
};

extern CTString astrSoundAPIRadioTexts[] = {
  RADIOTRANS("WaveOut"),
  RADIOTRANS("DirectSound"),
  RADIOTRANS("EAX"),
};

ULONG GetSpawnFlagsForGameType(INDEX iGameType)
{
  if (iGameType == -1) return SPF_SINGLEPLAYER;

  // get function that will provide us the flags
  CShellSymbol *pss = _pShell->GetSymbol("GetSpawnFlagsForGameTypeSS", /*bDeclaredOnly=*/ TRUE);
  // if none
  if (pss == NULL) {
    // error
    ASSERT(FALSE);
    return 0;
  }

  ULONG(*pFunc)(INDEX) = (ULONG(*)(INDEX))pss->ss_pvValue;
  return pFunc(iGameType);
}

BOOL IsMenuEnabled(const CTString &strMenuName)
{
  // get function that will provide us the flags
  CShellSymbol *pss = _pShell->GetSymbol("IsMenuEnabledSS", /*bDeclaredOnly=*/ TRUE);
  // if none
  if (pss == NULL) {
    // error
    ASSERT(FALSE);
    return TRUE;
  }

  BOOL(*pFunc)(const CTString &) = (BOOL(*)(const CTString &))pss->ss_pvValue;
  return pFunc(strMenuName);
}

// initialize game type strings table
void InitGameTypes(void)
{
  // get function that will provide us the info about gametype
  CShellSymbol *pss = _pShell->GetSymbol("GetGameTypeNameSS", /*bDeclaredOnly=*/ TRUE);
  // if none
  if (pss == NULL) {
    // error
    astrGameTypeRadioTexts[0] = "<???>";
    ctGameTypeRadioTexts = 1;
    return;
  }

  // for each mode
  for (ctGameTypeRadioTexts = 0; ctGameTypeRadioTexts<ARRAYCOUNT(astrGameTypeRadioTexts); ctGameTypeRadioTexts++) {
    // get the text
    CTString(*pFunc)(INDEX) = (CTString(*)(INDEX))pss->ss_pvValue;
    CTString strMode = pFunc(ctGameTypeRadioTexts);
    // if no mode modes
    if (strMode == "") {
      // stop
      break;
    }
    // add that mode
    astrGameTypeRadioTexts[ctGameTypeRadioTexts] = strMode;
  }
}

int qsort_CompareFileInfos_NameUp(const void *elem1, const void *elem2)
{
  const CFileInfo &fi1 = **(CFileInfo **)elem1;
  const CFileInfo &fi2 = **(CFileInfo **)elem2;
  return strcmp(fi1.fi_strName, fi2.fi_strName);
}

int qsort_CompareFileInfos_NameDn(const void *elem1, const void *elem2)
{
  const CFileInfo &fi1 = **(CFileInfo **)elem1;
  const CFileInfo &fi2 = **(CFileInfo **)elem2;
  return -strcmp(fi1.fi_strName, fi2.fi_strName);
}

int qsort_CompareFileInfos_FileUp(const void *elem1, const void *elem2)
{
  const CFileInfo &fi1 = **(CFileInfo **)elem1;
  const CFileInfo &fi2 = **(CFileInfo **)elem2;
  return strcmp(fi1.fi_fnFile, fi2.fi_fnFile);
}

int qsort_CompareFileInfos_FileDn(const void *elem1, const void *elem2)
{
  const CFileInfo &fi1 = **(CFileInfo **)elem1;
  const CFileInfo &fi2 = **(CFileInfo **)elem2;
  return -strcmp(fi1.fi_fnFile, fi2.fi_fnFile);
}

INDEX APIToSwitch(enum GfxAPIType gat)
{
  switch (gat) {
  case GAT_OGL: return 0;
#ifdef SE1_D3D
  case GAT_D3D: return 1;
#endif // SE1_D3D
  default: ASSERT(FALSE); return 0;
  }
}

enum GfxAPIType SwitchToAPI(INDEX i)
{
  switch (i) {
  case 0: return GAT_OGL;
#ifdef SE1_D3D
  case 1: return GAT_D3D;
#endif // SE1_D3D
  default: ASSERT(FALSE); return GAT_OGL;
  }
}

INDEX DepthToSwitch(enum DisplayDepth dd)
{
  switch (dd) {
  case DD_DEFAULT: return 0;
  case DD_16BIT: return 1;
  case DD_32BIT: return 2;
  default: ASSERT(FALSE); return 0;
  }
}

enum DisplayDepth SwitchToDepth(INDEX i)
{
  switch (i) {
  case 0: return DD_DEFAULT;
  case 1: return DD_16BIT;
  case 2: return DD_32BIT;
  default: ASSERT(FALSE); return DD_DEFAULT;
  }
}

// controls that are currently customized
CTFileName _fnmControlsToCustomize = CTString("");

void ControlsMenuOn()
{
  _pGame->SavePlayersAndControls();
  try {
    _pGame->gm_ctrlControlsExtra.Load_t(_fnmControlsToCustomize);
  }
  catch (char *strError) {
    WarningMessage(strError);
  }
}

void ControlsMenuOff()
{
  try {
    if (_pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions.Count()>0) {
      _pGame->gm_ctrlControlsExtra.Save_t(_fnmControlsToCustomize);
    }
  }
  catch (char *strError) {
    FatalError(strError);
  }
  FORDELETELIST(CButtonAction, ba_lnNode, _pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions, itAct) {
    delete &itAct.Current();
  }
  _pGame->LoadPlayersAndControls();
}