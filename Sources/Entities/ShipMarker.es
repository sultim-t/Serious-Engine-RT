104
%{
#include "Entities/StdH/StdH.h"
%}

uses "Entities/Marker";

class CShipMarker: CMarker {
name      "Ship Marker";
thumbnail "Thumbnails\\ShipMarker.tbn";

properties:
  1 BOOL m_bHarbor                    "Harbor" 'H' = FALSE,
  2 FLOAT m_fSpeed                    "Speed [m/s]" 'S' = -1.0f,
  3 FLOAT m_fRotation                 "Rotation [deg/s]" 'R' = -1.0f,
  4 FLOAT m_fAcceleration             "Acceleration" 'C' = 10.0f,
  5 FLOAT m_fRockingV                 "Rocking V" 'V' = -1.0f,
  6 FLOAT m_fRockingA                 "Rocking A" 'A' = -1.0f,
  7 FLOAT m_tmRockingChange           "Rocking Change Time" = 3.0f,

components:
  1 model   MODEL_MARKER     "Models\\Editor\\ShipMarker.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\ShipMarker.tex"

functions:
  /* Check if entity can drop marker for making linked route. */
  BOOL DropsMarker(CTFileName &fnmMarkerClass, CTString &strTargetProperty) const {
    fnmMarkerClass = CTFILENAME("Classes\\ShipMarker.ecl");
    strTargetProperty = "Target";
    return TRUE;
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

    return;
  }
};

