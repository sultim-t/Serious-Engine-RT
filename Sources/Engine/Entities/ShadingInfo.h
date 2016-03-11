/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_SHADINGINFO_H
#define SE_INCL_SHADINGINFO_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>
#include <Engine/Math/Vector.h>

// Used for caching shading info for models if they don't move
class ENGINE_API CShadingInfo {
public:
  CListNode si_lnInPolygon;       // for linking in the relevant polygon
  CBrushPolygon *si_pbpoPolygon;  // the polygon that entity is above
  CTerrain      *si_ptrTerrain;   // terrain that entity is above
  FLOAT3D si_vNearPoint;          // the relevant point in absolute space
  PIX si_pixShadowU, si_pixShadowV; // the relevant point in the polygon shadow map
  FLOAT si_fUDRatio, si_fLRRatio;   // fraction between pixels
  CEntity *si_penEntity;          // the entity which uses this shading info
};


#endif  /* include-once check. */

