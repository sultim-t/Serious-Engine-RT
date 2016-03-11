/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

202
%{
#include "StdH.h"
%}

class CAreaMarker: CEntity {
name      "AreaMarker";
thumbnail "Thumbnails\\AreaMarker.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N' = "AreaMarker",
  2 CTString m_strDescription = "",
  3 FLOATaabbox3D m_boxArea "Area box" 'B' = FLOATaabbox3D(FLOAT3D(0,0,0), FLOAT3D(10,10,10)),

components:
  1 model   MODEL_AREAMARKER     "Models\\Editor\\Axis.mdl",
  2 texture TEXTURE_AREAMARKER   "Models\\Editor\\Vector.tex"

functions:
  
  void GetAreaBox(FLOATaabbox3D &box) {
    box = m_boxArea;
    box +=GetPlacement().pl_PositionVector;
    return;
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_AREAMARKER);
    SetModelMainTexture(TEXTURE_AREAMARKER);

    return;
  }
};

