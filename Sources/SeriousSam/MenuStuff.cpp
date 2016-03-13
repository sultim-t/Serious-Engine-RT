/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Build.h>

#include "MenuStuff.h"

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