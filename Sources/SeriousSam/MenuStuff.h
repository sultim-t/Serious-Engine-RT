/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_STUFF_H
#define SE_INCL_MENU_STUFF_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern INDEX ctGameTypeRadioTexts;

extern CTString astrNoYes[2];
extern CTString astrWeapon[4];
extern CTString astrComputerInvoke[2];
extern CTString astrCrosshair[8];
extern CTString astrMaxPlayersRadioTexts[15];
extern CTString astrGameTypeRadioTexts[20];
extern CTString astrDifficultyRadioTexts[6];
extern CTString astrSplitScreenRadioTexts[4];
extern CTString astrDisplayPrefsRadioTexts[4];
extern CTString astrDisplayAPIRadioTexts[2];
extern CTString astrBitsPerPixelRadioTexts[3];
extern CTString astrFrequencyRadioTexts[4];
extern CTString astrSoundAPIRadioTexts[3];

ULONG GetSpawnFlagsForGameType(INDEX iGameType);
BOOL IsMenuEnabled(const CTString &strMenuName);

#endif  /* include-once check. */