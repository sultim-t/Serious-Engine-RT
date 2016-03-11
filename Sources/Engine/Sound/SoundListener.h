/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_SOUNDLISTENER_H
#define SE_INCL_SOUNDLISTENER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>
#include <Engine/Math/Vector.h>
#include <Engine/Math/Matrix.h>

class CSoundListener {
public:
  CListNode sli_lnInActiveListeners;  // for linking for current frame of listening

  FLOAT3D sli_vPosition;          // listener position
  FLOATmatrix3D sli_mRotation;    // listener rotation matrix
  FLOAT3D sli_vSpeed;             // speed of the listener
  FLOAT sli_fVolume;              // listener volume (i.e. deaf factor)
  FLOAT sli_fFilter;              // global filter for all sounds on this listener
  CEntity *sli_penEntity;         // listener entity (for listener local sounds)
  INDEX sli_iEnvironmentType;     // EAX environment predefine
  FLOAT sli_fEnvironmentSize;     // EAX environment size
};


#endif  /* include-once check. */

