/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include <stdio.h>
#include <lwserver.h>
#include <lwgeneric.h>
#include <lwmotion.h>
#include <lwshader.h>
#include <lwsurf.h>
#include <lwmaster.h>
#include "base.h"

extern XCALL_(int) Activation_ExportMesh( long version, GlobalFunc *global, LWLayoutGeneric *local, void *serverData);
extern XCALL_(int) Activation_ExportAnim( long version, GlobalFunc *global, LWLayoutGeneric *local, void *serverData);
extern XCALL_(int) Animation_Handler( long version, GlobalFunc *global, LWItemMotionHandler *local, void *serverData);
extern XCALL_(int) SectionAnimation_Handler( long version, GlobalFunc *_global, LWItemMotionHandler *local, void *serverData);
extern XCALL_(int) Interface_SurfaceParameters( long version, GlobalFunc *global, LWInterface *local, void *serverData );
extern XCALL_(int) Handler_SurfaceParameters( long version, GlobalFunc *global, LWShaderHandler *local, void *serverData);
extern XCALL_(int) Activate_Master(long version, GlobalFunc *g, LWMasterHandler *local, void *data);
extern XCALL_(int) Interface_Master(long version, GlobalFunc *global, LWInterface *local, void *serverData);
extern XCALL_(int) Activate_ModelerMeshExporter( long version, GlobalFunc *global, MeshEditBegin *local, void *serverData );
extern XCALL_(int) Activate_ModelerSurfaceToWeights( long version, GlobalFunc *global, MeshEditBegin *local, void *serverData );
extern XCALL_(int) Activate_CopyWeightMaps( long version, GlobalFunc *global, MeshEditBegin *local, void *serverData );

extern "C" ServerRecord ServerDesc[] = {
   { LWITEMMOTION_HCLASS,   DEBUGEXT "internal_SEAnimExport", (ActivateFunc*)&Animation_Handler, NULL },
	 { LWITEMMOTION_HCLASS,   DEBUGEXT "internal_SESectionAnimExport", (ActivateFunc*)&SectionAnimation_Handler, NULL },	
   { LWSHADER_HCLASS, DEBUGEXT "SE_Shaders", (ActivateFunc*)&Handler_SurfaceParameters, NULL },
   { LWSHADER_ICLASS, DEBUGEXT "SE_Shaders", (ActivateFunc*)&Interface_SurfaceParameters, NULL },
   { LWMASTER_HCLASS, DEBUGEXT "SE_Exporter", (ActivateFunc*)&Activate_Master, NULL },
   { LWMASTER_ICLASS, DEBUGEXT "SE_Exporter", (ActivateFunc*)&Interface_Master, NULL },
	 { LWMESHEDIT_CLASS, DEBUGEXT "SE_Modeler_Exporter", (ActivateFunc*)&Activate_ModelerMeshExporter, NULL },
	 { LWMESHEDIT_CLASS, DEBUGEXT "SE_SurfaceToWeights", (ActivateFunc*)&Activate_ModelerSurfaceToWeights, NULL },
	 { LWMESHEDIT_CLASS, DEBUGEXT "SE_CopyWeightMaps", (ActivateFunc*)&Activate_CopyWeightMaps, NULL },
   { NULL }
};
