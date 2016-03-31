/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"

#include "MenuPrinting.h"
#include "ArrowDir.h"

static const FLOAT _fBigStartJ = 0.25f;
static const FLOAT _fBigSizeJ = 0.066f;
static const FLOAT _fMediumSizeJ = 0.04f;

static const FLOAT _fNoStartI = 0.25f;
static const FLOAT _fNoSizeI = 0.04f;
static const FLOAT _fNoSpaceI = 0.01f;
static const FLOAT _fNoUpStartJ = 0.24f;
static const FLOAT _fNoDownStartJ = 0.44f;
static const FLOAT _fNoSizeJ = 0.04f;

FLOATaabbox2D BoxTitle(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0, _fBigSizeJ),
    FLOAT2D(1, _fBigSizeJ));
}

FLOATaabbox2D BoxNoUp(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(_fNoStartI+fRow*(_fNoSizeI+_fNoSpaceI), _fNoUpStartJ),
    FLOAT2D(_fNoStartI+fRow*(_fNoSizeI+_fNoSpaceI)+_fNoSizeI, _fNoUpStartJ+_fNoSizeJ));
}

FLOATaabbox2D BoxNoDown(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(_fNoStartI+fRow*(_fNoSizeI+_fNoSpaceI), _fNoDownStartJ),
    FLOAT2D(_fNoStartI+fRow*(_fNoSizeI+_fNoSpaceI)+_fNoSizeI, _fNoDownStartJ+_fNoSizeJ));
}

FLOATaabbox2D BoxBigRow(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.1f, _fBigStartJ+fRow*_fBigSizeJ),
    FLOAT2D(0.9f, _fBigStartJ+(fRow+1)*_fBigSizeJ));
}
FLOATaabbox2D BoxBigLeft(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.1f, _fBigStartJ+fRow*_fBigSizeJ),
    FLOAT2D(0.45f, _fBigStartJ+(fRow+1)*_fBigSizeJ));
}

FLOATaabbox2D BoxBigRight(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.55f, _fBigStartJ+fRow*_fBigSizeJ),
    FLOAT2D(0.9f, _fBigStartJ+(fRow+1)*_fBigSizeJ));
}

FLOATaabbox2D BoxSaveLoad(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.20f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.95f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxVersion(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.05f, _fBigStartJ+-5.5f*_fMediumSizeJ),
    FLOAT2D(0.97f, _fBigStartJ+(-5.5f+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxMediumRow(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.05f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.95f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxKeyRow(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.15f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.85f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxMediumLeft(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.05f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.45f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxPlayerSwitch(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.05f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.65f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxMediumMiddle(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(_fNoStartI, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.95f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxPlayerEdit(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(_fNoStartI, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.65f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxMediumRight(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.55f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.95f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxPopup(void)
{
  return FLOATaabbox2D(FLOAT2D(0.2f, 0.4f), FLOAT2D(0.8f, 0.6f));
}

FLOATaabbox2D BoxPopupLabel(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.22f, 0.43f),
    FLOAT2D(0.78f, 0.49f));
}

FLOATaabbox2D BoxPopupYesLarge(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.30f, 0.51f),
    FLOAT2D(0.48f, 0.57f));
}

FLOATaabbox2D BoxPopupNoLarge(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.52f, 0.51f),
    FLOAT2D(0.70f, 0.57f));
}

FLOATaabbox2D BoxPopupYesSmall(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.30f, 0.54f),
    FLOAT2D(0.48f, 0.59f));
}

FLOATaabbox2D BoxPopupNoSmall(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.52f, 0.54f),
    FLOAT2D(0.70f, 0.59f));
}

FLOATaabbox2D BoxChangePlayer(INDEX iTable, INDEX iButton)
{
  return FLOATaabbox2D(
    FLOAT2D(0.5f+0.15f*(iButton-1), _fBigStartJ+_fMediumSizeJ*2.0f*iTable),
    FLOAT2D(0.5f+0.15f*(iButton+0), _fBigStartJ+_fMediumSizeJ*2.0f*(iTable+1)));
}

FLOATaabbox2D BoxInfoTable(INDEX iTable)
{
  switch(iTable) {
  case 0:
  case 1:
  case 2:
  case 3:
    return FLOATaabbox2D(
      FLOAT2D(0.1f, _fBigStartJ+_fMediumSizeJ*2.0f*iTable),
      FLOAT2D(0.5f, _fBigStartJ+_fMediumSizeJ*2.0f*(iTable+1)));
  default:
    ASSERT(FALSE);
  case -1:  // single player table
  return FLOATaabbox2D(
    FLOAT2D(0.1f, 1-0.2f-_fMediumSizeJ*2.0f),
    FLOAT2D(0.5f, 1-0.2f));
  }
}

FLOATaabbox2D BoxArrow(enum ArrowDir ad)
{
  switch(ad) {
  default:
    ASSERT(FALSE);
  case AD_UP:
    return FLOATaabbox2D(
      FLOAT2D(0.02f, _fBigStartJ+0*_fMediumSizeJ),
      FLOAT2D(0.15f, _fBigStartJ+(0+1)*_fMediumSizeJ));
  case AD_DOWN:
    return FLOATaabbox2D(
      FLOAT2D(0.02f, _fBigStartJ+15*_fMediumSizeJ),
      FLOAT2D(0.15f, _fBigStartJ+(15+1)*_fMediumSizeJ));
  }
}

FLOATaabbox2D BoxBack(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.02f, 0.95f),
    FLOAT2D(0.15f, 1.0f));
}

FLOATaabbox2D BoxNext(void)
{
  return FLOATaabbox2D(
    FLOAT2D(0.85f, 0.95f),
    FLOAT2D(0.98f, 1.0f));
}

FLOATaabbox2D BoxLeftColumn(FLOAT fRow)
{
  return FLOATaabbox2D(
    FLOAT2D(0.02f, _fBigStartJ+fRow*_fMediumSizeJ),
    FLOAT2D(0.15f, _fBigStartJ+(fRow+1)*_fMediumSizeJ));
}

FLOATaabbox2D BoxPlayerModel(void)
{
  extern INDEX sam_bWideScreen;
  if (!sam_bWideScreen) {
    return FLOATaabbox2D(FLOAT2D(0.68f, 0.235f), FLOAT2D(0.965f, 0.78f));
  } else {
    return FLOATaabbox2D(FLOAT2D(0.68f, 0.235f), FLOAT2D(0.68f+(0.965f-0.68f)*9.0f/12.0f, 0.78f));
  }
}

FLOATaabbox2D BoxPlayerModelName(void)
{
  return FLOATaabbox2D(FLOAT2D(0.68f, 0.78f), FLOAT2D(0.965f, 0.82f));
}

PIXaabbox2D FloatBoxToPixBox(const CDrawPort *pdp, const FLOATaabbox2D &boxF)
{
  PIX pixW = pdp->GetWidth();
  PIX pixH = pdp->GetHeight();
  return PIXaabbox2D(
    PIX2D(boxF.Min()(1)*pixW, boxF.Min()(2)*pixH),
    PIX2D(boxF.Max()(1)*pixW, boxF.Max()(2)*pixH));
}

FLOATaabbox2D PixBoxToFloatBox(const CDrawPort *pdp, const PIXaabbox2D &boxP)
{
  FLOAT fpixW = pdp->GetWidth();
  FLOAT fpixH = pdp->GetHeight();
  return FLOATaabbox2D(
    FLOAT2D(boxP.Min()(1)/fpixW, boxP.Min()(2)/fpixH),
    FLOAT2D(boxP.Max()(1)/fpixW, boxP.Max()(2)/fpixH));
}

extern CFontData _fdTitle;
void SetFontTitle(CDrawPort *pdp)
{
  pdp->SetFont( &_fdTitle);
  pdp->SetTextScaling( 1.25f * pdp->GetWidth() /640 *pdp->dp_fWideAdjustment);
  pdp->SetTextAspect(1.0f);
}

extern CFontData _fdBig;
void SetFontBig(CDrawPort *pdp)
{
  pdp->SetFont( &_fdBig);
  pdp->SetTextScaling( 1.0f * pdp->GetWidth() /640 *pdp->dp_fWideAdjustment);
  pdp->SetTextAspect(1.0f);
}

extern CFontData _fdMedium;
void SetFontMedium(CDrawPort *pdp)
{
  pdp->SetFont( &_fdMedium);
  pdp->SetTextScaling( 1.0f * pdp->GetWidth() /640 *pdp->dp_fWideAdjustment);
  pdp->SetTextAspect(0.75f);
}

void SetFontSmall(CDrawPort *pdp)
{
  pdp->SetFont( _pfdConsoleFont);
  pdp->SetTextScaling( 1.0f);
  pdp->SetTextAspect(1.0f);
}
