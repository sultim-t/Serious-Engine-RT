/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include <GameMP/Game.h>

// This class is exported from the GameGUI.dll
class CGameGUI {
public:
  // functions called from World Editor
  __declspec(dllexport) static void OnInvokeConsole(void);
  __declspec(dllexport) static void OnPlayerSettings(void);
  __declspec(dllexport) static void OnAudioQuality(void);
  __declspec(dllexport) static void OnVideoQuality(void);
  __declspec(dllexport) static void OnSelectPlayerAndControls(void);
};

// global game gui object
extern CGameGUI _GameGUI;
