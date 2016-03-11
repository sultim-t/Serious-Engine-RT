/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAMESHELL_H
#define SE_INCL_GAMESHELL_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

struct GameGUI_interface {
  // initialize game and load settings
  void (*Initialize)(const CTFileName &fnGameSettings);
  // save settings and cleanup
  void (*End)(void);
  // run a quicktest game from within editor
  void (*QuickTest)(const CTFileName &fnMapName, CDrawPort *pdpDrawport, CViewPort *pvpViewport);
  // show console window
  void (*OnInvokeConsole)(void);
  // adjust players and controls
  void (*OnPlayerSettings)(void);
  // adjust audio settings
  void (*OnAudioQuality)(void);
  // adjust video settings
  void (*OnVideoQuality)(void);
  // select current active player and controls
  void (*OnSelectPlayerAndControls)(void);
};


#endif  /* include-once check. */

