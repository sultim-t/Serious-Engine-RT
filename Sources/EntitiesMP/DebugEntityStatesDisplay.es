/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

233
%{
#include "StdH.h"
void HUD_SetEntityForStackDisplay(CRationalEntity *pren);
%}


class CEntityStateDisplay : CRationalEntity {
name      "EntityStateDisplay";
thumbnail "Thumbnails\\EntityStateDisplay.tbn";
features  "HasTarget", "HasName";

properties:
  1 CTString m_strName         "Name" 'N' = "EntityStateDisplay",  
  2 CEntityPointer m_penTarget "Target" 'T' COLOR(C_dGREEN|0xFF), // entity which it points to
  

components:
  1 model   MODEL_MARKER     "ModelsMP\\Editor\\Debug_EntityStack.mdl",
  2 texture TEXTURE_MARKER   "ModelsMP\\Editor\\Debug_EntityStack.tex"

functions:
  void ~CEntityStateDisplay()
  {
    HUD_SetEntityForStackDisplay(NULL);
  }
  
procedures:
  Main() {
    
     // init as nothing
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    GetModelObject()->StretchModel(FLOAT3D(0.4f, 0.4f, 0.4f));
    
    // setup target for stack display every 1/10th of the second so
    // that after a reload or restart everything will work allright
    while(TRUE) {
      wait(0.1f)
      {
        on (EBegin) : {
          if (m_penTarget!=NULL) {
            HUD_SetEntityForStackDisplay((CRationalEntity *)&*m_penTarget);
          } else {
            HUD_SetEntityForStackDisplay(NULL);
          }
          resume;
        }
        on (ETimer) : {
          stop;  
        }
      }
    }
    return;
  };
};