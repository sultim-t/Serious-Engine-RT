/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_CRCTABLE_H
#define SE_INCL_CRCTABLE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern BOOL CRCT_bGatherCRCs;  // set while gathering CRCs of all loaded files

// init CRC table
void CRCT_Init(void);
// add one file to active list
void CRCT_AddFile_t(const CTFileName &fnm, ULONG ulCRC=0);// throw char *
// check if a file is added
BOOL CRCT_IsFileAdded(const CTFileName &fnm);
// reset all files to not active
void CRCT_ResetActiveList(void);
// free all memory used by the crc cache
void CRCT_Clear(void);
// dump list of all active files to the stream
void CRCT_MakeFileList_t(CTStream &strmFiles);  // throw char *
// dump checksums for all files from the list
ULONG CRCT_MakeCRCForFiles_t(CTStream &strmFiles);  // throw char *


#endif  /* include-once check. */

