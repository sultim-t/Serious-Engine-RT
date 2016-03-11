/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Templates/StaticStackArray.h>
#include <Engine/Base/CTString.h>
#include <Engine/Ska/StringTable.h>
#include <Engine/Templates/StaticStackArray.cpp>

struct stTable
{
  INDEX st_iID;
  CTString strName;
};
CStaticStackArray<struct stTable> _arStringTable;

// add index in table
INDEX AddIndexToTable(CTString strName)
{
  _arStringTable.Push();

  INDEX ctStrings = _arStringTable.Count();
  _arStringTable[ctStrings-1].strName = strName;
  _arStringTable[ctStrings-1].st_iID = ctStrings;
  return ctStrings-1;
}
// find string in table and return his index, if not found add new and return his index
INDEX ska_GetIDFromStringTable(CTString strName)
{
  if(strName == "") return -1;

  INDEX ctStrings = _arStringTable.Count();
  for(INDEX i=0;i<ctStrings;i++)
  {
    if(_arStringTable[i].strName == strName)
    {
      return i;
    }
  }
  return AddIndexToTable(strName);  
}
// find string in table and return his index, if not found return -1
INDEX ska_FindStringInTable(CTString strName)
{
  if(strName == "") return -1;

  INDEX ctStrings = _arStringTable.Count();
  for(INDEX i=0;i<ctStrings;i++)
  {
    if(_arStringTable[i].strName == strName)
    {
      return i;
    }
  }
  return -1;
}
// return name for index iIndex
CTString ska_GetStringFromTable(INDEX iIndex)
{
  INDEX ctStrings = _arStringTable.Count();
  if((iIndex >= 0) && (iIndex <= ctStrings-1))
  {
    return _arStringTable[iIndex].strName;
  }
  return "";
}
