/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Graphics/DisplayMode.h>
#include <Engine/Base/Translation.h>

/*
 * Default constructor.
 */
CDisplayMode::CDisplayMode(void)
{
  // clear all members
  dm_pixSizeI = 0;
  dm_pixSizeJ = 0;
  dm_ddDepth  = DD_DEFAULT;
}


// get depth string
CTString CDisplayMode::DepthString(void) const
{
  switch (dm_ddDepth) {
  case DD_NODEPTH: return "none"; break;
  case DD_16BIT:   return "16"; break;
  case DD_32BIT:   return "32"; break;
  case DD_DEFAULT: return TRANS("desktop"); break;
  default: ASSERT(FALSE); return TRANS("unknown"); break;
  }
}


// check if mode is dualhead
BOOL CDisplayMode::IsDualHead(void)
{
  // if size is 8:3 it is dual head
  return dm_pixSizeI*3==dm_pixSizeJ*8;
}


// check if mode is widescreen
BOOL CDisplayMode::IsWideScreen(void)
{
  // only this one supported as widescreen
  return dm_pixSizeI==856 && dm_pixSizeJ==480;
}
// check if mode is fullscreen
BOOL CDisplayMode::IsFullScreen(void)
{
  return dm_pixSizeI!=0 && dm_pixSizeJ!=0;
}
