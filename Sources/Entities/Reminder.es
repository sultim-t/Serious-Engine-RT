703
%{
#include "Entities/StdH/StdH.h"
%}

// input parameter for timer
event EReminderInit {
  CEntityPointer penOwner,    // who owns it
  FLOAT fWaitTime,            // wait time
  INDEX iValue,               // reminder event value
};

class export CReminder : CRationalEntity {
name      "Reminder";
thumbnail "";

properties:
  1 CEntityPointer m_penOwner,    // entity which owns it
  2 FLOAT m_fWaitTime = 0.0f,     // wait time
  3 INDEX m_iValue = 0,           // reminder event value

components:
functions:
procedures:
  Main(EReminderInit eri) {
    // remember the initial parameters
    ASSERT(eri.penOwner!=NULL);
    m_penOwner = eri.penOwner;
    m_fWaitTime = eri.fWaitTime;
    m_iValue = eri.iValue;
    
    // init as nothing
    InitAsVoid();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // wait
    if (m_fWaitTime > 0.0f) {
      autowait(m_fWaitTime);
      EReminder er;
      er.iValue = m_iValue;
      if (m_penOwner!=NULL) {
        m_penOwner->SendEvent(er);
      }
    }

    // cease to exist
    Destroy();

    return;
  };
};