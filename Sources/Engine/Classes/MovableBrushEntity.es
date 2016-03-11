/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

/*
 * Brush entity that can move.
 */

3
%{
#include "StdH.h"
#include <Engine/Entities/InternalClasses.h>
%}

class export CMovableBrushEntity : CMovableEntity {
name      "MovableBrushEntity";
thumbnail "";
properties:
components:
functions:
  /*
   * Calculate physics for moving.
   */
  export void DoMoving(void)  // override from CMovableEntity
  {
    CMovableEntity::DoMoving();
    // recalculate all bounding boxes relative to new position
//    en_pbrBrush->CalculateBoundingBoxes(); // !!! why here (its done in SetPlacement()?)
  }

  /* Copy entity from another entity of same class. */
  /*CMovableBrushEntity &operator=(CMovableBrushEntity &enOther)
  {
    CMovableEntity::operator=(enOther);
    return *this;
  }*/
  /* Read from stream. */
  export void Read_t( CTStream *istr) // throw char *
  {
    CMovableEntity::Read_t(istr);
  }
  /* Write to stream. */
  export void Write_t( CTStream *ostr) // throw char *
  {
    CMovableEntity::Write_t(ostr);
  }

procedures:
  // must have at least one procedure per class
  Dummy() {};
};
