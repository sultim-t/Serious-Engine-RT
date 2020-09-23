226
%{
#include "Entities/StdH/StdH.h"
extern INDEX ent_bReportBrokenChains;
%}

class CMessageHolder : CRationalEntity {
name      "MessageHolder";
thumbnail "Thumbnails\\MessageHolder.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N' = "MessageHolder",
  3 CTString m_strDescription = "",
  2 CTFileName m_fnmMessage  "Message" 'M' = CTString(""),
  4 FLOAT m_fDistance  "Distance" 'D' = 2.0f,
  5 BOOL m_bActive "Active" 'A' = TRUE,
  6 CEntityPointer m_penNext "Next" 'X',

components:
  1 model   MODEL_MARKER     "Models\\Editor\\MessageHolder.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\MessageHolder.tex"

functions:
  const CTString &GetDescription(void) const {
    ((CTString&)m_strDescription).PrintF("%s", (const char*)m_fnmMessage.FileName());
    return m_strDescription;
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);
    wait() {
      on (ETrigger eTrigger): {
        if (!m_bActive) {
          resume;
        }
        CEntity *penCaused = FixupCausedToPlayer(this, eTrigger.penCaused);
        EComputerMessage eMsg;
        eMsg.fnmMessage = m_fnmMessage;
        penCaused->SendEvent(eMsg);
        resume;
      }
      on (EActivate): {
        m_bActive = TRUE;
        resume;
      }
      on (EDeactivate): {
        m_bActive = FALSE;
        resume;
      }
    }
    return;
  }
};

