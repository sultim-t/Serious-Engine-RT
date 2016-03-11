/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_RENDERPOLY_H
#define SE_INCL_RENDERPOLY_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// general structure
struct PolyVertex2D
{
  FLOAT pv2_fI, pv2_fJ;     // DrawPort 2D coords
  FLOAT pv2_f1oK;           // Viewpoint (camera) distance (z-buffer value)
  FLOAT pv2_fUoK, pv2_fVoK; // texture coordinates (for perspective mapping)
};

// functions for rendering triangles
extern void SetTriangleTexture( ULONG *pulCurrentMipmap, PIX pixMipWidth, PIX pixMipHeight);

extern void DrawTriangle_Mask( UBYTE *pubMaskPlane, SLONG slMaskWidth, SLONG slMaskHeight,
                               struct PolyVertex2D *ppv2Vtx1, struct PolyVertex2D *ppv2Vtx2,
                               struct PolyVertex2D *ppv2Vtx3, BOOL bTransparency);

// has model cast some cluster shadow (shadow renderer needs this)
extern BOOL _bSomeDarkExists;



#endif  /* include-once check. */

