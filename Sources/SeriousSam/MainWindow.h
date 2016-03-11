/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

extern BOOL _bWindowChanging;    // ignores window messages while this is set
extern HWND _hwndMain;

// init/end main window management
void MainWindow_Init(void);
void MainWindow_End(void);
// close the main application window
void CloseMainWindow(void);
// open the main application window for windowed mode
void OpenMainWindowNormal(PIX pixSizeI, PIX pixSizeJ);
void ResetMainWindowNormal(void);
// open the main application window for fullscreen mode
void OpenMainWindowFullScreen(PIX pixSizeI, PIX pixSizeJ);
// open the main application window invisible
void OpenMainWindowInvisible(void);
