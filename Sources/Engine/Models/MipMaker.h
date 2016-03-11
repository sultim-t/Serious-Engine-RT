/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MIPMAKER_H
#define SE_INCL_MIPMAKER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/CTString.h>
#include <Engine/Math/Object3D.h>
#include <Engine/Math/AABBox.h>
#include <Engine/Templates/DynamicArray.h>

class CMipPolygonVertex {
public:
  CMipPolygonVertex *mpv_pmpvNextInPolygon;
  class CMipPolygon *mpv_pmpPolygon;
  class CMipVertex *mpv_pmvVertex;
  inline void Clear(void) {};
};

class CMipVertex : public FLOAT3D {
public:
  FLOAT3D mv_vRestFrameCoordinate;
  INDEX mv_iSurface;
  BOOL mv_bUsed;
  CMipVertex *mv_pmvxRemap;
  CMipVertex();
  ~CMipVertex();
  void Clear(void);
};

class CMipPolygon {
public:
  class CMipPolygonVertex *mp_pmpvFirstPolygonVertex;
  INDEX mp_iSurface;
  CMipPolygon(void);
  ~CMipPolygon(void);
  void Clear(void);
};

class CMipSurface {
public:
  CTString ms_strName;
  COLOR ms_colColor;
  inline void Clear(void) {};
};

class CMipModel {
public:
  CDynamicArray< CMipSurface> mm_amsSurfaces;
  CDynamicArray< CMipPolygon> mm_ampPolygons;
  CDynamicArray< CMipVertex> mm_amvVertices;
  FLOATaabbox3D mm_boxBoundingBox;
  void FromObject3D_t( CObject3D &objRestFrame, CObject3D &objMipSourceFrame);
  void ToObject3D( CObject3D &objDestination);
  
  ~CMipModel();
  BOOL CreateMipModel_t(INDEX iVetexRemoveRate, INDEX iSurfacePreservingFactor);
  INDEX FindSurfacesForVertices(void);
  void FindBestVertexPair( CMipVertex *&pmvBestSource, CMipVertex *&pmvBestTarget);
  void JoinVertexPair( CMipVertex *pmvBestSource, CMipVertex *pmvBestTarget);
  void RemoveUnusedVertices(void);
  void CheckObjectValidity(void);
  FLOAT GetGoodness(CMipVertex *pmvSource, CMipVertex *pmvTarget);
};


#endif  /* include-once check. */

