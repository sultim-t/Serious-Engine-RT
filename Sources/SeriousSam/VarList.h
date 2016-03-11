/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_VARLIST_H
#define SE_INCL_VARLIST_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

class CVarSetting {
public:
  CListNode vs_lnNode;
  BOOL  vs_bSeparator;
  BOOL  vs_bCanChangeInGame;
  INDEX vs_iSlider;
  CTString vs_strName;
  CTString vs_strTip;
  CTString vs_strVar;
  CTString vs_strFilter;
  CTFileName vs_strSchedule;
  INDEX vs_iValue;
  INDEX vs_ctValues;
  INDEX vs_iOrgValue;
  BOOL  vs_bCustom;
  CStaticStackArray<CTString> vs_astrTexts;
  CStaticStackArray<CTString> vs_astrValues;
  CVarSetting();
  void Clear(void);
  BOOL Validate(void);
};


extern CListHead _lhVarSettings;

void LoadVarSettings(const CTFileName &fnmCfg);
void FlushVarSettings(BOOL bApply);


#endif  /* include-once check. */

