/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GEOMETRY_H
#define SE_INCL_GEOMETRY_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Math/Vector.h>
#include <Engine/Math/Plane.h>
#include <Engine/Math/Matrix.h>

/*
 * General geometry functions
 */

/* Calculate rotation matrix from angles in 3D. */
//ENGINE_API extern void operator^=(FLOATmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
ENGINE_API extern void MakeRotationMatrix(FLOATmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
ENGINE_API extern void MakeRotationMatrixFast(FLOATmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
/* Calculate inverse rotation matrix from angles in 3D. */
//ENGINE_API extern void operator!=(FLOATmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
ENGINE_API extern void MakeInverseRotationMatrix(FLOATmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
ENGINE_API extern void MakeInverseRotationMatrixFast(FLOATmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
/* Decompose rotation matrix into angles in 3D. */
//ENGINE_API extern void operator^=(ANGLE3D &a3dAngles, const FLOATmatrix3D &t3dRotation);
ENGINE_API extern void DecomposeRotationMatrix(ANGLE3D &a3dAngles, const FLOATmatrix3D &t3dRotation);
ENGINE_API extern void DecomposeRotationMatrixNoSnap(ANGLE3D &a3dAngles, const FLOATmatrix3D &t3dRotation);

/* Create direction vector from angles in 3D (ignoring banking). */
ENGINE_API extern void AnglesToDirectionVector(const ANGLE3D &a3dAngles,
                                             FLOAT3D &vDirection);
/* Create angles in 3D from direction vector(ignoring banking).
   (direction must be normalized!)*/
ENGINE_API extern void DirectionVectorToAngles(const FLOAT3D &vDirection,
                                             ANGLE3D &a3dAngles);
ENGINE_API extern void DirectionVectorToAnglesNoSnap(const FLOAT3D &vDirection,
                                             ANGLE3D &a3dAngles);
/* Create angles in 3D from up vector (ignoring objects relative heading).
   (up vector must be normalized!)*/
ENGINE_API extern void UpVectorToAngles(const FLOAT3D &vUp,
                                             ANGLE3D &a3dAngles);


/* Calculate rotation matrix from angles in 3D. */
ENGINE_API extern void operator^=(DOUBLEmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
/* Calculate inverse rotation matrix from angles in 3D. */
ENGINE_API extern void operator!=(DOUBLEmatrix3D &t3dRotation, const ANGLE3D &a3dAngles);
/* Decompose rotation matrix into angles in 3D. */
ENGINE_API extern void operator^=(ANGLE3D &a3dAngles, const DOUBLEmatrix3D &t3dRotation);




#endif  /* include-once check. */

