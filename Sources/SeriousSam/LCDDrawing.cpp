/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "Stdh.h"
#include "LCDDrawing.h"

extern void LCDInit(void)
{
  _pGame->LCDInit();
}

extern void LCDEnd(void)
{
  _pGame->LCDEnd();
}

extern void LCDPrepare(FLOAT fFade)
{
  _pGame->LCDPrepare(fFade);
}

extern void LCDSetDrawport(CDrawPort *pdp)
{
  _pGame->LCDSetDrawport(pdp);
}

extern void LCDDrawBox(PIX pixUL, PIX pixDR, PIXaabbox2D &box, COLOR col)
{
  _pGame->LCDDrawBox(pixUL, pixDR, box, col);
}

extern void LCDScreenBoxOpenLeft(COLOR col)
{
  _pGame->LCDScreenBoxOpenLeft(col);
}

extern void LCDScreenBoxOpenRight(COLOR col)
{
  _pGame->LCDScreenBoxOpenRight(col);
}

extern void LCDScreenBox(COLOR col)
{
  _pGame->LCDScreenBox(col);
}

extern void LCDRenderClouds1(void)
{
  _pGame->LCDRenderClouds1();
}

extern void LCDRenderClouds2(void)
{
  _pGame->LCDRenderClouds2();
}

extern void LCDRenderGrid(void)
{
  _pGame->LCDRenderGrid();
}

extern COLOR LCDGetColor(COLOR colDefault, const char *strName)
{
  return _pGame->LCDGetColor(colDefault, strName);
}
extern COLOR LCDFadedColor(COLOR col)
{
  return _pGame->LCDFadedColor(col);
}

extern COLOR LCDBlinkingColor(COLOR col0, COLOR col1)
{
  return _pGame->LCDBlinkingColor(col0, col1);
}

extern void LCDDrawPointer(PIX pixI, PIX pixJ)
{
  _pGame->LCDDrawPointer(pixI, pixJ);
}
