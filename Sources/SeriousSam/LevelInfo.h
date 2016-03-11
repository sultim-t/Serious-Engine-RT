/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */


extern CListHead _lhAutoDemos;
extern CListHead _lhAllLevels;
extern CListHead _lhFilteredLevels;

class CLevelInfo {
public:
  CListNode li_lnNode;
  CTFileName li_fnLevel;
  CTString li_strName;
  ULONG li_ulSpawnFlags;

  CLevelInfo(void);
  CLevelInfo(const CLevelInfo &li);
  void operator=(const CLevelInfo &li);
};

// find all levels that match given flags
void FilterLevels(ULONG ulSpawnFlags);

// init level-info subsystem
void LoadLevelsList(void);
// cleanup level-info subsystem
void ClearLevelsList(void);
// get level info for its filename
CLevelInfo FindLevelByFileName(const CTFileName &fnm);
// if level doesn't support given flags, find one that does
void ValidateLevelForFlags(CTString &fnm, ULONG ulSpawnFlags);

// init list of autoplay demos
void LoadDemosList(void);
// clear list of autoplay demos
void ClearDemosList(void);
