214
%{
#include "Entities/StdH/StdH.h"
%}

uses "Entities/Marker";

// world link
enum WorldLinkType {
  1 WLT_FIXED     "Fixed",      // fixed link
  2 WLT_RELATIVE  "Relative",   // relative link
};

class CWorldLink : CMarker {
name      "World link";
thumbnail "Thumbnails\\WorldLink.tbn";
features "IsImportant";

properties:
  1 CTString m_strGroup           "Group" 'G' = "",
  2 CTFileNameNoDep m_strWorld    "World" 'W' = "",
  3 BOOL m_bStoreWorld            "Store world" 'S' = FALSE,
  4 enum WorldLinkType m_EwltType "Type" 'Y' = WLT_RELATIVE,

components:
  1 model   MODEL_WORLDLINK     "Models\\Editor\\WorldLink.mdl",
  2 texture TEXTURE_WORLDLINK   "Models\\Editor\\WorldLink.tex"


functions:
/************************************************************
 *                      START EVENT                         *
 ************************************************************/
  BOOL HandleEvent(const CEntityEvent &ee) {
    if (ee.ee_slEvent == EVENTCODE_ETrigger) {
      ETrigger &eTrigger = (ETrigger &)ee;
      _SwcWorldChange.strGroup = m_strGroup;      // group name
      _SwcWorldChange.plLink = GetPlacement();    // link placement
      _SwcWorldChange.iType = (INDEX)m_EwltType;  // type
      _pNetwork->ChangeLevel(m_strWorld, m_bStoreWorld, 0);
      return TRUE;
    }
    return FALSE;
  };

procedures:
/************************************************************
 *                       M  A  I  N                         *
 ************************************************************/
  Main(EVoid) {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_WORLDLINK);
    SetModelMainTexture(TEXTURE_WORLDLINK);

    // set name
    m_strName.PrintF("World link - %s", (const char*)m_strGroup);

    return;
  }
};
