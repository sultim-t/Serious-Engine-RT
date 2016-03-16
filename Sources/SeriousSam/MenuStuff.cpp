/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

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