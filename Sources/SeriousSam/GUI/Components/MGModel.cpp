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
#include <Engine/Base/KeyNames.h>
#include <Engine/CurrentVersion.h>
#include <GameMP/LCDDrawing.h>
#include "MGModel.h"

extern INDEX sam_bWideScreen;


CMGModel::CMGModel(void)
{
  mg_fFloorY = 0;
}

void CMGModel::Render(CDrawPort *pdp)
{
  // if no model
  if (mg_moModel.GetData() == NULL) {
    // just render text
    mg_strText = TRANS("No model");
    CMGButton::Render(pdp);
    return;
  }

  // get position on screen
  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  CDrawPort dpModel(pdp, box);
  dpModel.Lock();
  dpModel.FillZBuffer(1.0f);

  LCDSetDrawport(&dpModel);
  // clear menu here
  dpModel.Fill(C_BLACK | 255);
  LCDRenderClouds1();
  LCDRenderClouds2();

  // prepare projection
  CRenderModel rmRenderModel;
  CPerspectiveProjection3D pr;
  pr.FOVL() = sam_bWideScreen ? AngleDeg(45.0f) : AngleDeg(30.0f);
  pr.ScreenBBoxL() = FLOATaabbox2D(
    FLOAT2D(0.0f, 0.0f),
    FLOAT2D((float)dpModel.GetWidth(), (float)dpModel.GetHeight())
    );
  pr.AspectRatioL() = 1.0f;
  pr.FrontClipDistanceL() = 0.3f;
  pr.ViewerPlacementL() = CPlacement3D(FLOAT3D(0, 0, 0), ANGLE3D(0, 0, 0));

  // initialize remdering
  CAnyProjection3D apr;
  apr = pr;
  BeginModelRenderingView(apr, &dpModel);
  rmRenderModel.rm_vLightDirection = FLOAT3D(0.2f, -0.2f, -0.2f);

  // if model needs floor
  if (mg_moFloor.GetData() != NULL) {
    // set floor's position
    CPlacement3D pl = mg_plModel;
    pl.pl_OrientationAngle = ANGLE3D(0, 0, 0);
    pl.pl_PositionVector = mg_plModel.pl_PositionVector;
    pl.pl_PositionVector(2) += mg_fFloorY;
    rmRenderModel.SetObjectPlacement(pl);

    // render the floor
    rmRenderModel.rm_colLight = C_WHITE;
    rmRenderModel.rm_colAmbient = C_WHITE;
    mg_moFloor.SetupModelRendering(rmRenderModel);
    mg_moFloor.RenderModel(rmRenderModel);
  }

  // set model's position
  CPlacement3D pl;
  pl.pl_OrientationAngle = mg_plModel.pl_OrientationAngle;
  pl.pl_PositionVector = mg_plModel.pl_PositionVector;
  extern FLOAT sam_fPlayerOffset;
  pl.pl_PositionVector(3) += sam_fPlayerOffset;
  rmRenderModel.SetObjectPlacement(pl);

  // render the model
  rmRenderModel.rm_colLight = LerpColor(C_BLACK, C_WHITE, 0.4f) | CT_OPAQUE;
  rmRenderModel.rm_colAmbient = LerpColor(C_BLACK, C_WHITE, 0.2f) | CT_OPAQUE;
  mg_moModel.SetupModelRendering(rmRenderModel);
  FLOATplane3D plFloorPlane = FLOATplane3D(FLOAT3D(0.0f, 1.0f, 0.0f),
    mg_plModel.pl_PositionVector(2) + mg_fFloorY);
  FLOAT3D vShadowLightDir = FLOAT3D(-0.2f, -0.4f, -0.6f);
  CPlacement3D plLightPlacement = CPlacement3D(
    mg_plModel.pl_PositionVector +
    vShadowLightDir*mg_plModel.pl_PositionVector(3) * 5,
    ANGLE3D(0, 0, 0));
  mg_moModel.RenderShadow(rmRenderModel, plLightPlacement, 200.0f, 200.0f, 1.0f, plFloorPlane);
  mg_moModel.RenderModel(rmRenderModel);
  EndModelRenderingView();

  LCDScreenBox(LCDGetColor(C_GREEN, "model box") | GetCurrentColor());

  dpModel.Unlock();

  pdp->Unlock();
  pdp->Lock();
  LCDSetDrawport(pdp);

  // print the model name
  {
    PIXaabbox2D box = FloatBoxToPixBox(pdp, BoxPlayerModelName());
    COLOR col = GetCurrentColor();

    PIX pixI = box.Min()(1);
    PIX pixJ = box.Max()(2);
    pdp->PutText(mg_strText, pixI, pixJ, col);
  }
}