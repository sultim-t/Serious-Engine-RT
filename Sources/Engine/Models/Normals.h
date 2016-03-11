/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_NORMALS_H
#define SE_INCL_NORMALS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#define MAX_GOURAUDNORMALS 256
extern FLOAT3D  avGouraudNormals[MAX_GOURAUDNORMALS];

/* Find nearest Gouraud normal for a vector. */
INDEX GouraudNormal(    const FLOAT3D &vNormal);
void CompressNormal_HQ( const FLOAT3D &vNormal, UBYTE &ubH, UBYTE &ubP);
void DecompressNormal_HQ(     FLOAT3D &vNormal, UBYTE  ubH, UBYTE  ubP);


#endif  /* include-once check. */

