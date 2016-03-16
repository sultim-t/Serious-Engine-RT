/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_MODEL_H
#define SE_INCL_MENU_GADGET_MODEL_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"


class CMGModel : public CMGButton {
public:
	CModelObject mg_moModel;
	CModelObject mg_moFloor;
	CPlacement3D mg_plModel;
	BOOL mg_fFloorY;

	CMGModel(void);
	void Render(CDrawPort *pdp);
};

#endif  /* include-once check. */