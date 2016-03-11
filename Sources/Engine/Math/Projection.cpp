/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Math/Projection.h>

/////////////////////////////////////////////////////////////////////
//  CProjection3D
/////////////////////////////////////////////////////////////////////
// Construction / destruction

/*
 * Default constructor.
 */
CProjection3D::CProjection3D(void) {
  pr_Prepared = FALSE;
  pr_ObjectStretch = FLOAT3D(1.0f, 1.0f, 1.0f);
  pr_bFaceForward = FALSE;
  pr_bHalfFaceForward = FALSE;
  pr_vObjectHandle = FLOAT3D(0.0f, 0.0f, 0.0f);
  pr_fDepthBufferNear = 0.0f;
  pr_fDepthBufferFar  = 1.0f;
  pr_NearClipDistance = 0.25f;
  pr_FarClipDistance = -9999.0f;  // never used by default
  pr_bMirror = FALSE;
  pr_bWarp = FALSE;
  pr_fViewStretch = 1.0f;
}

CPlacement3D _plOrigin(FLOAT3D(0,0,0), ANGLE3D(0,0,0));
