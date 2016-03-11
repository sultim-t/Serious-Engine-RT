/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

236
%{
#include "StdH.h"
#include "TacticsHolder.h"
%}

class CTacticsChanger : CRationalEntity {
name      "TacticsChanger";
thumbnail "Thumbnails\\TacticsChanger.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N' = "TacticsChanger",
  2 CTString m_strDescription = "",

  10 enum   TacticType m_tctType "Type" 'Y' = TCT_NONE, // tactic type
  11 FLOAT  m_fParam1  "Parameter 1" 'D' = 0.0f,   // parameter 1
  12 FLOAT  m_fParam2  "Parameter 2" 'F' = 0.0f,   // parameter 2
  13 FLOAT  m_fParam3  "Parameter 3" 'G' = 0.0f,   // parameter 3
  14 FLOAT  m_fParam4  "Parameter 4" 'H' = 0.0f,   // parameter 4
  15 FLOAT  m_fParam5  "Parameter 5" 'J' = 0.0f,   // parameter 5
/*16 BOOL   m_bRetryOnBlock "Retry tactic when blocked" = FALSE,
  17 INDEX  m_bRetryCount   "Number of retries" = 1,  */
  18 CEntityPointer m_penTacticsHolder "Tactics holder" 'H',
  

components:
  1 model   MODEL_MANAGER     "ModelsMP\\Editor\\TacticsChanger.mdl",
  2 texture TEXTURE_MANAGER   "ModelsMP\\Editor\\TacticsChanger.tex"

functions:
  const CTString &GetDescription(void) const {
    return m_strDescription;
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MANAGER);
    SetModelMainTexture(TEXTURE_MANAGER);
 
    autowait(0.1f);
    
    while(TRUE) {
      wait()
      {
        on (ETrigger) : {
          if (m_penTacticsHolder!=NULL) {
            CTacticsHolder *penTactics = &(CTacticsHolder &)*m_penTacticsHolder;
            penTactics->m_tctType = m_tctType;
            penTactics->m_fParam1 = m_fParam1;
            penTactics->m_fParam2 = m_fParam2;
            penTactics->m_fParam3 = m_fParam3;
            penTactics->m_fParam4 = m_fParam4;
            penTactics->m_fParam5 = m_fParam5;
            penTactics->m_tmLastActivation = _pTimer->CurrentTick();
          }
          stop;
        }
        otherwise() : {
          resume;
        }
      };  
     
      // wait a bit to recover
      autowait(0.1f);
    }
  }
};
