/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_DISPLAYMODE_H
#define SE_INCL_DISPLAYMODE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>
#include <Engine/Base/CTString.h>

// display mode bit-depth
enum DisplayDepth
{
  DD_NODEPTH = -1,
  DD_DEFAULT =  0,
  DD_16BIT   =  1,
  DD_32BIT   =  2,
  DD_24BIT   =  3, // for z-buffer
};

/*
 *  Structure that holds display mode description
 */

class ENGINE_API CDisplayMode {
public:
  PIX dm_pixSizeI; // size of screen in pixels
  PIX dm_pixSizeJ;
  enum DisplayDepth dm_ddDepth;  // bits per pixel for color

  /* Default constructor. */
  CDisplayMode(void);

  // get depth string
  CTString DepthString(void) const;
  // check if mode is dualhead
  BOOL IsDualHead(void);
  // check if mode is widescreen
  BOOL IsWideScreen(void);
  // check if mode is fullscreen
  BOOL IsFullScreen(void);
};


#endif  /* include-once check. */

