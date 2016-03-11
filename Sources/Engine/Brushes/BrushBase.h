/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_BRUSH_BASE_H
#define SE_INCL_BRUSH_BASE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// this is base class for brushes and terrains 
class ENGINE_API CBrushBase {
public:
  virtual INDEX GetBrushType() {
    ASSERT(FALSE);
    return BT_NONE;
  };

  enum BrushType {
    BT_NONE        = 0,     // none 
    BT_BRUSH3D     = 1,     // this is Brush3D
    BT_TERRAIN     = 2,     // this is Terrain
  };
};

#endif