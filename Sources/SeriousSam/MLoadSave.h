/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_LOADSAVE_H
#define SE_INCL_GAME_MENU_LOADSAVE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CLoadSaveMenu : public CGameMenu {
public:
	// settings adjusted before starting the menu
	CGameMenu *gm_pgmNextMenu;  // menu to go to after selecting a file (if null, use parent menu)
	CTFileName gm_fnmSelected;  // file that is selected initially
	CTFileName gm_fnmDirectory; // directory that should be read
	CTFileName gm_fnmBaseName;  // base file name for saving (numbers are auto-added)
	CTFileName gm_fnmExt;       // accepted file extension
	BOOL gm_bSave;              // set when chosing file for saving
	BOOL gm_bManage;            // set if managing (rename/delet is enabled)
	CTString gm_strSaveDes;     // default description (if saving)
	BOOL gm_bAllowThumbnails;   // set when chosing file for saving
	BOOL gm_bNoEscape;          // forbid exiting with escape/rmb
#define LSSORT_NONE     0
#define LSSORT_NAMEUP   1
#define LSSORT_NAMEDN   2
#define LSSORT_FILEUP   3
#define LSSORT_FILEDN   4
	INDEX gm_iSortType;    // sort type

	// function to activate when file is chosen
	// return true if saving succeeded - description is saved automatically
	// always return true for loading
	BOOL(*gm_pAfterFileChosen)(const CTFileName &fnm);

	// internal properties
	CListHead gm_lhFileInfos;   // all file infos to list
	INDEX gm_iLastFile;         // index of last saved file in numbered format

	// called to get info of a file from directory, or to skip it
	BOOL ParseFile(const CTFileName &fnm, CTString &strName);

	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
	void FillListItems(void);
};

#endif  /* include-once check. */