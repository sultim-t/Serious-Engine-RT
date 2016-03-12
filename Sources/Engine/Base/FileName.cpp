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

#include "stdh.h"

#include <Engine/Base/FileName.h>

#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Stream.h>
#include <Engine/Templates/NameTable_CTFileName.h>
#include <Engine/Templates/DynamicStackArray.cpp>

template CDynamicArray<CTFileName>;
template CDynamicStackArray<CTFileName>;
#include <Engine/Templates/StaticStackArray.cpp>
template CStaticStackArray<long>;

/*
 * Get directory part of a filename.
 */
CTFileName CTFileName::FileDir() const
{
  ASSERT(IsValid());

  // make a temporary copy of string
  CTFileName strPath(*this);
  // find last backlash in it
  char *pPathBackSlash = strrchr( strPath.str_String, '\\');
  // if there is no backslash
  if( pPathBackSlash == NULL) {
    // return emptystring as directory
    return( CTFileName(""));
  }
  // set end of string after where the backslash was
  pPathBackSlash[1] = 0;
  // return a copy of temporary string
  return( CTFileName( strPath));
}

CTFileName &CTFileName::operator=(const char *strCharString)
{
  ASSERTALWAYS( "Use CTFILENAME for conversion from char *!");
  return *this;
}

/*
 * Get name part of a filename.
 */
CTFileName CTFileName::FileName() const
{
  ASSERT(IsValid());

  // make a temporary copy of string
  CTFileName strPath(*this);
  // find last dot in it
  char *pDot = strrchr( strPath.str_String, '.');
  // if there is a dot
  if( pDot != NULL) {
    // set end of string there
    pDot[0] = 0;
  }

  // find last backlash in what's left
  char *pBackSlash = strrchr( strPath.str_String, '\\');
  // if there is no backslash
  if( pBackSlash == NULL) {
    // return it all as filename
    return( CTFileName(strPath));
  }
  // return a copy of temporary string, starting after the backslash
  return( CTFileName( pBackSlash+1));
}

/*
 * Get extension part of a filename.
 */
CTFileName CTFileName::FileExt() const
{
  ASSERT(IsValid());

  // find last dot in the string
  char *pExtension = strrchr( str_String, '.');
  // if there is no dot
  if( pExtension == NULL) {
    // return no extension
    return( CTFileName(""));
  }
  // return a copy of the extension part, together with the dot
  return( CTFileName( pExtension));
}

CTFileName CTFileName::NoExt() const
{
  return FileDir()+FileName();
}

static INDEX GetSlashPosition(const CHAR* pszString)
{
  for (INDEX iPos = 0; '\0' != *pszString; ++iPos, ++pszString) {
    if (('\\' == *pszString) || ('/' == *pszString)) {
      return iPos;
    }
  }
  return -1;
}

/*
 * Set path to the absolute path, taking \.. and /.. into account.
 */
void CTFileName::SetAbsolutePath(void)
{
  // Collect path parts
  CTString strRemaining(*this);
  CStaticStackArray<CTString> astrParts;
  INDEX iSlashPos = GetSlashPosition(strRemaining);
  if (0 > iSlashPos) {
    return; // Invalid path
  }
  for (;;) {
    CTString &strBeforeSlash = astrParts.Push();
    CTString strAfterSlash;
    strRemaining.Split(iSlashPos, strBeforeSlash, strAfterSlash);
    strAfterSlash.TrimLeft(strAfterSlash.Length() - 1);
    strRemaining = strAfterSlash;
    iSlashPos = GetSlashPosition(strRemaining);
    if (0 > iSlashPos) {
      astrParts.Push() = strRemaining;
      break;
    }
  }
  // Remove certain path parts
  for (INDEX iPart = 0; iPart < astrParts.Count(); ++iPart) {
    if (CTString("..") != astrParts[iPart]) {
      continue;
    }
    if (0 == iPart) {
      return; // Invalid path
    }
    // Remove ordered
    CStaticStackArray<CTString> astrShrinked;
    astrShrinked.Push(astrParts.Count() - 2);
    astrShrinked.PopAll();
    for (INDEX iCopiedPart = 0; iCopiedPart < astrParts.Count(); ++iCopiedPart) {
      if ((iCopiedPart != iPart - 1) && (iCopiedPart != iPart)) {
        astrShrinked.Push() = astrParts[iCopiedPart];
      }
    }
    astrParts.MoveArray(astrShrinked);
    iPart -= 2;
  }
  // Set new content
  strRemaining.Clear();
  for (INDEX iPart = 0; iPart < astrParts.Count(); ++iPart) {
    strRemaining += astrParts[iPart];
    if (iPart < astrParts.Count() - 1) {
#ifdef PLATFORM_WIN32
      strRemaining += CTString("\\");
#else
      strRemaining += CTString("/");
#endif
    }
  }
  (*this) = strRemaining;
}

/*
 * Remove application path from a file name and returns TRUE if it's a relative path.
 */
BOOL CTFileName::RemoveApplicationPath_t(void) // throws char *
{
  CTFileName fnmApp = _fnmApplicationPath;
  fnmApp.SetAbsolutePath();
  // remove the path string from beginning of the string
  BOOL bIsRelative = RemovePrefix(fnmApp);
  if (_fnmMod!="") {
    RemovePrefix(_fnmApplicationPath+_fnmMod);
  }
  return bIsRelative;
}

/*
 * Read from stream.
 */
 CTStream &operator>>(CTStream &strmStream, CTFileName &fnmFileName)
{
  // if dictionary is enabled
  if (strmStream.strm_dmDictionaryMode == CTStream::DM_ENABLED) {
    // read the index in dictionary
    INDEX iFileName;
    strmStream>>iFileName;
    // get that file from the dictionary
    fnmFileName = strmStream.strm_afnmDictionary[iFileName];

  // if dictionary is processing or not active
  } else {
    char strTag[] = "_FNM"; strTag[0] = 'D';  // must create tag at run-time!
    // skip dependency catcher header
    strmStream.ExpectID_t(strTag);    // data filename
    // read the string
    strmStream>>(CTString &)fnmFileName;
    fnmFileName.fnm_pserPreloaded = NULL;
  }

  return strmStream;
}

/*
 * Write to stream.
 */
 CTStream &operator<<(CTStream &strmStream, const CTFileName &fnmFileName)
{
  // if dictionary is enabled
  if (strmStream.strm_dmDictionaryMode == CTStream::DM_ENABLED) {
    // try to find the filename in dictionary
    CTFileName *pfnmExisting = strmStream.strm_ntDictionary.Find(fnmFileName);
    // if not existing
    if (pfnmExisting==NULL) {
      // add it
      pfnmExisting = &strmStream.strm_afnmDictionary.Push();
      *pfnmExisting = fnmFileName;
      strmStream.strm_ntDictionary.Add(pfnmExisting);
    }
    // write its index
    strmStream<<strmStream.strm_afnmDictionary.Index(pfnmExisting);

  // if dictionary is processing or not active
  } else {
    char strTag[] = "_FNM"; strTag[0] = 'D';  // must create tag at run-time!
    // write dependency catcher header
    strmStream.WriteID_t(strTag);     // data filename
    // write the string
    strmStream<<(CTString &)fnmFileName;
  }

  return strmStream;
}

void CTFileName::ReadFromText_t(CTStream &strmStream,
                                const CTString &strKeyword) // throw char *
{
  ASSERT(IsValid());

  char strTag[] = "_FNM "; strTag[0] = 'T';  // must create tag at run-time!
  // keyword must be present
  strmStream.ExpectKeyword_t(strKeyword);
  // after the user keyword, dependency keyword must be present
  strmStream.ExpectKeyword_t(strTag);

  // read the string from the file
  char str[1024];
  strmStream.GetLine_t(str, sizeof(str));
  fnm_pserPreloaded = NULL;

  // copy it here
  (*this) = CTString( (const char *)str);
}
