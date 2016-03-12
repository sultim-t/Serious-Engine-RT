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
#include <Engine/Base/IFeel.h>
#include <Engine/Base/FileName.h>
#include <Engine/Base/Stream.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/Shell.h>

//Imm_GetProductName
HINSTANCE _hLib = NULL;
BOOL (*immCreateDevice)(HINSTANCE &hInstance, HWND &hWnd) = NULL;
void (*immDeleteDevice)(void) = NULL;
BOOL (*immProductName)(char *strProduct,int iMaxCount) = NULL;
BOOL (*immLoadFile)(const char *fnFile) = NULL;
void (*immUnloadFile)(void) = NULL;
void (*immPlayEffect)(const char *pstrEffectName) = NULL;
void (*immStopEffect)(const char *pstrEffectName) = NULL;
void (*immChangeGain)(const float fGain) = NULL;

FLOAT ifeel_fGain = 1.0f;
INDEX ifeel_bEnabled = FALSE;

#ifndef NDEBUG
  #define IFEEL_DLL_NAME "Bin/Debug/ImmWrapper.dll"
#else
  #define IFEEL_DLL_NAME "Bin/ImmWrapper.dll"
#endif

void ifeel_GainChange(void *ptr)
{
  IFeel_ChangeGain(ifeel_fGain);
}

CTString IFeel_GetProductName()
{
  char strProduct[MAX_PATH];
  if(immProductName != NULL)
  {
    if(immProductName(strProduct,MAX_PATH))
    {
      return strProduct;
    }
  }
  return "";
}

CTString IFeel_GetProjectFileName()
{
  CTString strIFeelTable;
  CTFileName fnIFeelTable = (CTString)"Data\\IFeel.txt";
  CTString strDefaultProjectFile = "Data\\Default.ifr";
  // get product name
  CTString strProduct = IFeel_GetProductName();
  try
  {
    strIFeelTable.Load_t(fnIFeelTable);
  }
  catch(char *strErr)
  {
    CPrintF("%s\n",strErr);
    return "";
  }

  CTString strLine;
  // read up to 1000 devices
  for(INDEX idev=0;idev<1000;idev++)
  {
    char strDeviceName[256];
    char strProjectFile[256];
    strLine = strIFeelTable;
    // read first line
    strLine.OnlyFirstLine();
    if(strLine==strIFeelTable)
    {
      break;
    }
    // remove that line 
    strIFeelTable.RemovePrefix(strLine);
    strIFeelTable.DeleteChar(0);
    // read device name and project file name
    strIFeelTable.ScanF("\"%256[^\"]\" \"%256[^\"]\"",&strDeviceName,&strProjectFile);
    // check if this is default 
    if(strcmp(strDeviceName,"Default")==0) strDefaultProjectFile = strProjectFile;
    // check if this is current device 
    if(strProduct == strDeviceName) return strProjectFile;
  }
  // device was not found, return default project file
  CPrintF("No project file specified for device '%s'.\nUsing default project file\n",strProduct);
  return strDefaultProjectFile;
}

// inits imm ifeel device
BOOL IFeel_InitDevice(HINSTANCE &hInstance, HWND &hWnd)
{
  _pShell->DeclareSymbol("void inp_IFeelGainChange(INDEX);", &ifeel_GainChange);
  _pShell->DeclareSymbol("persistent user FLOAT inp_fIFeelGain post:inp_IFeelGainChange;", &ifeel_fGain);
  _pShell->DeclareSymbol("const user INDEX sys_bIFeelEnabled;", &ifeel_bEnabled);
  IFeel_ChangeGain(ifeel_fGain);

  // load iFeel lib 
  CTFileName fnmExpanded;
  ExpandFilePath(EFP_READ | EFP_NOZIPS,(CTString)IFEEL_DLL_NAME,fnmExpanded);
  if(_hLib!=NULL) return FALSE;

  UINT iOldErrorMode = SetErrorMode( SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
  _hLib = LoadLibraryA(fnmExpanded);
  SetErrorMode(iOldErrorMode);
  if(_hLib==NULL)
  {
    CPrintF("Error loading ImmWraper.dll.\n\tIFeel disabled\n");
    return FALSE;
  }

  // take func pointers
  immCreateDevice = (BOOL(*)(HINSTANCE &hInstance, HWND &hWnd)) GetProcAddress(_hLib,"Imm_CreateDevice");
  immDeleteDevice = (void(*)(void)) GetProcAddress(_hLib,"Imm_DeleteDevice");
  immProductName = (BOOL(*)(char *strProduct,int iMaxCount)) GetProcAddress(_hLib,"Imm_GetProductName");
  immLoadFile = (BOOL(*)(const char *fnFile))GetProcAddress(_hLib,"Imm_LoadFile");
  immUnloadFile = (void(*)(void))GetProcAddress(_hLib,"immUnloadFile");
  immPlayEffect = (void(*)(const char *pstrEffectName))GetProcAddress(_hLib,"Imm_PlayEffect");
  immStopEffect = (void(*)(const char *pstrEffectName))GetProcAddress(_hLib,"Imm_StopEffect");
  immChangeGain = (void(*)(const float fGain))GetProcAddress(_hLib,"Imm_ChangeGain");

  // create device
  if(immCreateDevice == NULL)
  {
    return FALSE;
  }
  BOOL hr = immCreateDevice(hInstance,hWnd);
  if(!hr)
  {
    CPrintF("IFeel mouse not found.\n");
    IFeel_DeleteDevice();
    return FALSE;
  }
  CPrintF("IFeel mouse '%s' initialized\n",(const char*)IFeel_GetProductName());
  ifeel_bEnabled = TRUE;
  return TRUE;
}
// delete imm ifeel device
void IFeel_DeleteDevice()
{
  if(immDeleteDevice != NULL) immDeleteDevice();
  immCreateDevice = NULL;
  immDeleteDevice = NULL;
  immProductName = NULL;
  immLoadFile = NULL;
  immUnloadFile = NULL;
  immPlayEffect = NULL;
  immStopEffect = NULL;
  immChangeGain = NULL;

  if(_hLib != NULL) FreeLibrary(_hLib);
  _hLib = NULL;
}
// loads project file
BOOL IFeel_LoadFile(CTFileName fnFile)
{
  CTFileName fnmExpanded;
  ExpandFilePath(EFP_READ | EFP_NOZIPS,fnFile,fnmExpanded);

  if(immLoadFile!=NULL)
  {
    BOOL hr = immLoadFile((const char*)fnmExpanded);
    if(hr)
    {
      CPrintF("IFeel project file '%s' loaded\n", fnFile);
      return TRUE;
    }
    else
    {
      CPrintF("Error loading IFeel project file '%s'\n", fnFile);
      return FALSE;
    }
  }
  return FALSE;
}
// unloads project file
void IFeel_UnloadFile()
{
  if(immUnloadFile!=NULL) immUnloadFile();
}
// plays effect from ifr file
void IFeel_PlayEffect(char *pstrEffectName)
{
  IFeel_ChangeGain(ifeel_fGain);
  if(immPlayEffect!=NULL) immPlayEffect(pstrEffectName);
}
// stops effect from ifr file
void IFeel_StopEffect(char *pstrEffectName)
{
  if(immStopEffect!=NULL) immStopEffect(pstrEffectName);
}
// change gain
void IFeel_ChangeGain(FLOAT fGain)
{
  if(immChangeGain!=NULL)
  {
    immChangeGain(fGain);
    //CPrintF("Changing IFeel gain to '%g'\n",fGain);
  }
}
