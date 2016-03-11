/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_TERRAIN_RENDER_H
#define SE_INCL_TERRAIN_RENDER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// Prepare scene for terrain render
void PrepareScene(CAnyProjection3D &apr, CDrawPort *pdp, CTerrain *ptrTerrain);
// Render one terrain
void RenderTerrain(void);
// Render one terrain in wireframe mode
void RenderTerrainWire(COLOR &colEdges);
// Regenerate terrain tile
void ReGenerateTile(INDEX itt);
// Draw terrain quad tree
void DrawQuadTree(void);
// Draw box in wireframe
void gfxDrawWireBox(FLOATaabbox3D &bbox, COLOR col);
// Draw selected vertices
void DrawSelectedVertices(GFXVertex *pavVertices, GFXColor *pacolColors, INDEX ctVertices);

SLONG GetUsedMemoryForTileBatching(void);

#endif