/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_TERRAIN_RAY_CASTING_H
#define SE_INCL_TERRAIN_RAY_CASTING_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

FLOAT TestRayCastHit(CTerrain *ptrTerrain, const FLOATmatrix3D &mRotation, const FLOAT3D &vPosition, 
                     const FLOAT3D &vOrigin, const FLOAT3D &vTarget,const FLOAT fOldDistance, 
                     const BOOL bHitInvisibleTris);

FLOAT TestRayCastHit(CTerrain *ptrTerrain, const FLOATmatrix3D &mRotation, const FLOAT3D &vPosition, 
                     const FLOAT3D &vOrigin, const FLOAT3D &vTarget,const FLOAT fOldDistance, 
                     const BOOL bHitInvisibleTris, FLOATplane3D &plHitPlane, FLOAT3D &vHitPoint);
#endif