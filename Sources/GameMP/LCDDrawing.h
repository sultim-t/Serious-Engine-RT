/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_LCDDRAWING_H
#define SE_INCL_LCDDRAWING_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern void LCDInit(void);
extern void LCDEnd(void);
extern void LCDPrepare(FLOAT fFade);
extern void LCDSetDrawport(CDrawPort *pdp);
extern void LCDDrawBox(PIX pixUL, PIX pixDR, PIXaabbox2D &box, COLOR col);
extern void LCDScreenBox(COLOR col);
extern void LCDScreenBoxOpenLeft(COLOR col);
extern void LCDScreenBoxOpenRight(COLOR col);
extern void LCDRenderClouds1(void);
extern void LCDRenderClouds2(void);
extern void LCDRenderClouds2Light(void);
extern void LCDRenderGrid(void);
extern void LCDDrawPointer(PIX pixI, PIX pixJ);
extern COLOR LCDGetColor(COLOR colDefault, const char *strName);
extern COLOR LCDFadedColor(COLOR col);
extern COLOR LCDBlinkingColor(COLOR col0, COLOR col1);


#endif  /* include-once check. */

