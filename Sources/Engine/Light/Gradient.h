/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GRADIENT_H
#define SE_INCL_GRADIENT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

class CGradientParameters {
public:
  FLOAT3D gp_vGradientDir;   // gradient direction in absolute space
  FLOAT gp_fH0; // position of color 0
  FLOAT gp_fH1; // position of color 1
  BOOL gp_bDark; // if dark light

  COLOR gp_col0;      // color 0
  COLOR gp_col1;      // color 1
};


#endif  /* include-once check. */

