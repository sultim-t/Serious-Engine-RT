/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_RASTER_H
#define SE_INCL_RASTER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>
#include <Engine/Graphics/DrawPort.h>

/*
 *  Raster
 */

class CRaster {
public:
  CViewPort *ra_pvpViewPort;        // viewport if existing
  CDrawPort ra_MainDrawPort;		    // initial drawport for entire raster
  CListHead ra_DrawPortList;	      // list of drawports

  PIX    ra_Width;								  // number of pixels in one row
  PIX    ra_Height;							    // number of pixels in one column
  SLONG  ra_LockCount;							// counter for memory locking
  ULONG  ra_Flags;									// special flags

  /* Recalculate dimensions for all drawports. */
	void RecalculateDrawPortsDimensions(void);

  /* Constructor for given size. */
  CRaster( PIX pixWidth, PIX pixHeight, ULONG ulFlags);
	/* Destructor. */
  virtual ~CRaster(void);
  /* Change size of this raster and all it's drawports. */
  void Resize(PIX pixNewWidth, PIX pixNewHeight);

public:
  /* Lock for drawing. */
  virtual BOOL Lock();
  /* Unlock after drawing. */
  virtual void Unlock();
};


#endif  /* include-once check. */

