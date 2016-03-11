/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_CHANGEPLAYER_H
#define SE_INCL_MENU_GADGET_CHANGEPLAYER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"


class CMGChangePlayer : public CMGButton {
public:
	INDEX mg_iLocalPlayer;

	void SetPlayerText(void);
	void OnActivate(void);
};

#endif  /* include-once check. */