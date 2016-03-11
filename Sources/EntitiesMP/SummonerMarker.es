/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

350
%{
#include "StdH.h"
%}

uses "EntitiesMP/Marker";

class CSummonerMarker: CMarker {
name      "SummonerMarker";
thumbnail "Thumbnails\\EnemyMarker.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N' = "SummonerMarker",
  2 CTString m_strDescription = "SummonerMarker",
  3 RANGE m_fMarkerRange "Marker Range" 'M' = 0.0f,  // range around marker (markers don't have to be hit directly)

components:
  1 model   MODEL_SUMMONERMARKER     "Models\\Editor\\EnemyMarker.mdl",
  2 texture TEXTURE_SUMMONERMARKER   "Models\\Editor\\BoundingBox.tex"

functions:

  /* Check if entity is moved on a route set up by its targets. */
  BOOL MovesByTargetedRoute(CTString &strTargetProperty) const {
    strTargetProperty = "Target";
    return TRUE;
  };

  /* Check if entity can drop marker for making linked route. */
  BOOL DropsMarker(CTFileName &fnmMarkerClass, CTString &strTargetProperty) const {
    fnmMarkerClass = CTFILENAME("Classes\\SummonerMarker.ecl");
    strTargetProperty = "Target";
    return TRUE;
  }

  BOOL IsTargetValid(SLONG slPropertyOffset, CEntity *penTarget)
  {
    if( slPropertyOffset == offsetof(CSummonerMarker, m_penTarget))
    {
      if (IsOfClass(penTarget, "SummonerMarker")) { return TRUE; }
      else { return FALSE; }
    }   
    return CEntity::IsTargetValid(slPropertyOffset, penTarget);
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_SUMMONERMARKER);
    SetModelMainTexture(TEXTURE_SUMMONERMARKER);

    return;
  }
};

