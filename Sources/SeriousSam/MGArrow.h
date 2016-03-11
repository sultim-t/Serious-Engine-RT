/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_ARROW_H
#define SE_INCL_MENU_GADGET_ARROW_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "ArrowDir.h"
#include "MGButton.h"


class CMGArrow : public CMGButton {
public:
	enum ArrowDir mg_adDirection;
	void Render(CDrawPort *pdp);
	void OnActivate(void);
};

#endif  /* include-once check. */