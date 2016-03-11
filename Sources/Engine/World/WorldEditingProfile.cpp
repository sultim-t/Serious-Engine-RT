/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/World/WorldEditingProfile.h>

// profile form for profiling world editing
CWorldEditingProfile wepWorldEditingProfile;
CProfileForm &_pfWorldEditingProfile = wepWorldEditingProfile;

/////////////////////////////////////////////////////////////////////
// CWorldEditingProfile

CWorldEditingProfile::CWorldEditingProfile(void)
 : CProfileForm ("World editing", "operations",
    CWorldEditingProfile::PCI_COUNT, CWorldEditingProfile::PTI_COUNT)
{
  SETTIMERNAME(PTI_CSGTOTAL,       "CSG operations total", "");
  SETTIMERNAME(PTI_OBJECTOPTIMIZE, "CObject3D::Optimize()", "");
  SETTIMERNAME(PTI_OBJECTCSG,      "CObject3D::CSGxxxx()", "");
  SETTIMERNAME(PTI_OBJECTCSG,      "CObject3D::CSGxxxx()", "");

  SETTIMERNAME(PTI_ADDOBJECTTOBRUSH,       "AddObjectToBrush()", "");
  SETTIMERNAME(PTI_FINDSHADOWLAYERS,       "FindShadowLayers()", "");
  SETTIMERNAME(PTI_CALCULATEBOUNDINGBOXES, "CalculateBoundingBoxes()", "");
  SETTIMERNAME(PTI_ADDFROMOBJECT3D,        "AddFromObject3D()", "");
  SETTIMERNAME(PTI_TRIANGULATE,            "Triangulate()", "");
  SETTIMERNAME(PTI_TRISTRIPMODELS,         "tristrip models", "");

  SETTIMERNAME(PTI_LINKENTITIESTOSECTORS, "LinkEntitiesToSectors()", "");
  SETTIMERNAME(PTI_LINKPORTALSANDSECTORS, "LinkPortalsAndSectors()", "");
  SETTIMERNAME(PTI_READBRUSHES,           "ReadBrushes()", "");
  SETTIMERNAME(PTI_READBSP,               "ReadBSP()", "");
  SETTIMERNAME(PTI_READPORTALSECTORLINKS, "ReadPortalSectorLinks()", "");
  SETTIMERNAME(PTI_READSTATE,             "ReadState()", "");
  SETTIMERNAME(PTI_REINITIALIZEENTITIES,  "ReinitializeEntities()", "");

  SETTIMERNAME(PTI_MAKESHADOWMAP, "MakeShadowMap()", "");
  SETTIMERNAME(PTI_RENDERSHADOWS, "RenderShadows()", "");

  SETTIMERNAME(PTI_MIXLAYERS,              "CLayerMixer::MixLayers()", "");
  SETTIMERNAME(PTI_CALCULATEDATA,          "CLayerMixer::CalculateData()", "");
  SETTIMERNAME(PTI_AMBIENTFILL,            "CLayerMixer::FillWithAmbientLight()", "");
  SETTIMERNAME(PTI_ADDONELAYERPOINT,       "CLayerMixer::AddOneLayerPoint()", "");
  SETTIMERNAME(PTI_ADDONELAYERDIRECTIONAL, "CLayerMixer::AddOneLayerDirectional()", "");

  SETCOUNTERNAME(PCI_SECTORSOPTIMIZED, "sectors optimized");
  SETCOUNTERNAME(PCI_SHADOWIMAGES,     "shadow images generated");
  SETCOUNTERNAME(PCI_SHADOWCLUSTERS,   "total shadow clusters generated in all images");
  SETCOUNTERNAME(PCI_POLYGONSHADOWS,   "total polygon shadows cast");
}
