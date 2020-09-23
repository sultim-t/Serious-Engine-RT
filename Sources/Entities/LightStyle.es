201
%{
#include "Entities/StdH/StdH.h"
%}

class CLightStyle : CEntity {
name      "Light";
thumbnail "Models\\Editor\\LightStyle.tbn";
features "HasName", "IsTargetable";

properties:
    2 CTString m_strName               "Name" 'N' = "<light style>",
//    1 LIGHTANIMATION m_iLightAnimation "Light animation" 'A'

components:
    1 model   MODEL_LIGHTSTYLE    "Models\\Editor\\LightSource.mdl",
    2 texture TEXTURE_LIGHTSTYLE  "Models\\Editor\\LightStyle.tex"

functions:
procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_LIGHTSTYLE);
    SetModelMainTexture(TEXTURE_LIGHTSTYLE);

    return;
  }
};
