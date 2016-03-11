/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"

#include <Engine/Entities/LastPositions.h>
#include <Engine/Math/Functions.h>
#include <Engine/Base/Timer.h>
#include <Engine/Templates/StaticArray.cpp>

CLastPositions::CLastPositions(const CLastPositions &lpOrg)
{
  lp_avPositions = lpOrg.lp_avPositions ;
  lp_iLast       = lpOrg.lp_iLast       ;
  lp_ctUsed      = lpOrg.lp_ctUsed      ;
  lp_tmLastAdded = lpOrg.lp_tmLastAdded ;
}

// add a new position
void CLastPositions::AddPosition(const FLOAT3D &vPos)
{
  lp_iLast++;
  if (lp_iLast>=lp_avPositions.Count()) {
    lp_iLast=0;
  }
  lp_ctUsed = Min(INDEX(lp_ctUsed+1), lp_avPositions.Count());
  lp_avPositions[lp_iLast] = vPos;
  lp_tmLastAdded = _pTimer->CurrentTick();
}

// get a position
const FLOAT3D &CLastPositions::GetPosition(INDEX iPre)
{
  INDEX iPos = lp_iLast-iPre;
  if (iPos<0) {
    iPos+=lp_avPositions.Count();
  }
  return lp_avPositions[iPos];
}
