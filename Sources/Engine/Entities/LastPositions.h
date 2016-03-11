/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_LASTPOSITIONS_H
#define SE_INCL_LASTPOSITIONS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Templates/StaticArray.h>
#include <Engine/Math/Vector.h>

// last positions for particle systems
class ENGINE_API CLastPositions {
public:
  CStaticArray<FLOAT3D> lp_avPositions;
  INDEX lp_iLast;   // where entity was last placed
  INDEX lp_ctUsed;  // how many positions are actually used
  TIME lp_tmLastAdded;  // time when last updated
  
  CLastPositions() {};
  CLastPositions(const CLastPositions &lpOrg);

  // add a new position
  void AddPosition(const FLOAT3D &vPos);
  // get a position
  const FLOAT3D &GetPosition(INDEX iPre);
};


#endif  /* include-once check. */

