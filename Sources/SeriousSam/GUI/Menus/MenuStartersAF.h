/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_STARTERSAF_H
#define SE_INCL_MENU_STARTERSAF_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif


BOOL LSLoadSinglePlayer(const CTFileName &fnm);
BOOL LSLoadNetwork(const CTFileName &fnm);
BOOL LSLoadSplitScreen(const CTFileName &fnm);
BOOL LSLoadDemo(const CTFileName &fnm);
BOOL LSLoadPlayerModel(const CTFileName &fnm);
BOOL LSLoadControls(const CTFileName &fnm);
BOOL LSLoadAddon(const CTFileName &fnm);
BOOL LSLoadMod(const CTFileName &fnm);
BOOL LSLoadCustom(const CTFileName &fnm);
BOOL LSLoadNetSettings(const CTFileName &fnm);
BOOL LSSaveAnyGame(const CTFileName &fnm);
BOOL LSSaveDemo(const CTFileName &fnm);
void StartDemoPlay(void);
void StartNetworkLoadGame(void);
void StartSplitScreenGameLoad(void);

#endif  /* include-once check. */