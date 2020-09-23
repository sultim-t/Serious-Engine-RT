509
%{
#include "Entities/StdH/StdH.h"
%}

// input parameter for acid
event EAcid {
  CEntityPointer penOwner,        // entity which owns it
  CEntityPointer penTarget,       // target entity which receive damage
};


class CAcid : CMovableModelEntity {
name      "Acid";
thumbnail "";

properties:
  1 CEntityPointer m_penOwner,    // entity which owns it
  2 CEntityPointer m_penTarget,   // target entity which receive damage
  5 BOOL m_bLoop = FALSE,         // internal for loops

components:
functions:
/************************************************************
 *                   P R O C E D U R E S                    *
 ************************************************************/
procedures:
  // --->>> MAIN
  Main(EAcid ea) {
    // attach to parent (another entity)
    ASSERT(ea.penOwner!=NULL);
    ASSERT(ea.penTarget!=NULL);
    m_penOwner = ea.penOwner;
    m_penTarget = ea.penTarget;

    // initialization
    InitAsVoid();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // acid damage
    SpawnReminder(this, 10.0f, 0);
    m_bLoop = TRUE;
    while(m_bLoop) {
      wait(0.25f) {
        // damage to parent
        on (EBegin) : {
          // inflict damage to parent
          if (m_penTarget!=NULL && !(m_penTarget->GetFlags()&ENF_DELETED)) {
            m_penTarget->InflictDirectDamage(m_penTarget, m_penOwner, DMT_ACID, 0.25f, FLOAT3D(0, 0, 0), FLOAT3D(0, 0, 0));
          // stop existing
          } else {
            m_bLoop = FALSE;
            stop;
          }
          resume;
        }
        on (ETimer) : { stop; }
        on (EReminder) : {
          m_bLoop = FALSE;
          stop;
        }
      }
    }

    // cease to exist
    Destroy();
    return;
  }
};
