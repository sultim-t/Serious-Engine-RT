/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_FILEINFO_H
#define SE_INCL_FILEINFO_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif


class CFileInfo {
public:
	CListNode fi_lnNode;
	CTFileName fi_fnFile;
	CTString fi_strName;
};


#endif  /* include-once check. */