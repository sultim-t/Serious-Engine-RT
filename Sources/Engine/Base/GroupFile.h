/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GROUPFILE_H
#define SE_INCL_GROUPFILE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/FileName.h>

/*
 * Class containing info about files inside group file
 */
class ENGINE_API CGroupFileFileInfo {
public:
  CTFileName gffi_fnFileName; // full name of the file (relative to application path)
  ULONG gffi_ulFileDataOffset; // file data block offset from beginning of the file
  ULONG gffi_ulFileSize; // size of sub group file
  /* Read one file info from stream. */
  void Read_t(CTStream *istr); // throw char *
  /* Write one file info into stream. */
  void Write_t(CTStream *ostr); // throw char *
};


#endif  /* include-once check. */

