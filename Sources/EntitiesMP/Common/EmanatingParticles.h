/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// type of emiters
enum CEmiterType {
  ET_AIR_ELEMENTAL = 1,
  ET_SUMMONER_STAFF,
  ET_FIREWORKS01,
};

class DECL_DLL CEmittedParticle {
public:
  FLOAT3D ep_vLastPos;
  FLOAT3D ep_vPos;
  FLOAT ep_fLastRot;
  FLOAT ep_fRot;
  FLOAT ep_fRotSpeed;
  FLOAT3D ep_vSpeed; // speed added in each tick
  COLOR ep_colLastColor;
  COLOR ep_colColor;
  FLOAT ep_tmEmitted;
  FLOAT ep_tmLife;
  FLOAT ep_fStretch;

  /* Default constructor. */
  CEmittedParticle(void);
  void Read_t( CTStream &strm);
  void Write_t( CTStream &strm);
};

class DECL_DLL CEmiter {
public:
  enum CEmiterType em_etType;
  BOOL em_bInitialized;
  FLOAT em_tmStart;
  FLOAT em_tmLife;
  FLOAT3D em_vG;
  COLOR em_colGlobal;
  INDEX em_iGlobal;
  CStaticStackArray<CEmittedParticle> em_aepParticles;

  /* Default constructor. */
  CEmiter(void);
  void Initialize(CEntity *pen);
  FLOAT3D GetGravity(CEntity *pen);
  void RenderParticles(void);
  void AnimateParticles(void);
  void AddParticle(FLOAT3D vPos, FLOAT3D vSpeed, FLOAT fRot, FLOAT fRotSpeed, 
    FLOAT tmBirth, FLOAT tmLife, FLOAT fStretch, COLOR colColor);
  void Read_t( CTStream &strm);
  void Write_t( CTStream &strm);
};

