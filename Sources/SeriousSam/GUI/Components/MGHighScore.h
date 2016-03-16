/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_HIGHSCORE_H
#define SE_INCL_MENU_GADGET_HIGHSCORE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MenuGadget.h"


class CMGHighScore : public CMenuGadget {
public:
	void Render(CDrawPort *pdp);
};

#endif  /* include-once check. */