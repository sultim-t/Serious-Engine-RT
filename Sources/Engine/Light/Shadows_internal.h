/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

class CLightRectangle {
public:
  // the rectangle coordinates
  PIX lr_pixMinU;
  PIX lr_pixMinV;
  PIX lr_pixSizeU;
  PIX lr_pixSizeV;
  // point on the plane closest to the light
  FLOAT lr_fpixHotU;
  FLOAT lr_fpixHotV;
  FLOAT lr_fLightPlaneDistance;
};