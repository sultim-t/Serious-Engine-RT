/* Copyright (c) 2020 Sultim Tsyrendashiev
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"
#include "RTProcessing.h"

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>
#include <Engine/Templates/DynamicArray.h>

#include <Engine/Templates/DynamicArray.cpp>

#include <Engine/Raytracing/SSRT.h>

static CStaticStackArray<GFXVertex> RT_AllSectorVertices;
static CStaticStackArray<INDEX> RT_AllSectorIndices;

static void RT_AddActiveSector(CBrushSector &bscSector, SSRT::SSRTMain *ssrt)
{
  RT_AllSectorVertices.PopAll();
  RT_AllSectorIndices.PopAll();

  // maybe export it to .obj file

  CBrush3D *brush = bscSector.bsc_pbmBrushMip->bm_pbrBrush;

  FOREACHINSTATICARRAY(bscSector.bsc_abpoPolygons, CBrushPolygon, itpo)
  {
    CBrushPolygon &polygon = *itpo;

    // for texture cordinates and transparency/translucency processing
  #pragma region MakeScreenPolygon
      /*// all portals will be rendered as portals,
      // original renderer also replaced them with pretenders if they're far away
      polygon.bpo_ulFlags &= ~BPOF_RENDERASPORTAL;
      if (polygon.bpo_ulFlags & BPOF_PORTAL)
      {
        polygon.bpo_ulFlags |= BPOF_RENDERASPORTAL;
      }*/


      // TODO: RT: texture coordinates for brushes

      // CRenderer::SetOneTextureParameters(CBrushPolygon &polygon, ScenePolygon &spo, INDEX iLayer)
      //SetOneTextureParameters(polygon, sppo, 0);
      //SetOneTextureParameters(polygon, sppo, 1);
      //SetOneTextureParameters(polygon, sppo, 2);

    if (polygon.bpo_ulFlags & BPOF_TRANSPARENT)
    {
      // TODO: RT: if needs alpha keying
    }

    bool needTranslucency = FALSE;
    const float fOpacity = brush->br_penEntity->GetOpacity();
    if (fOpacity < 1)
    {
      //needTranslucency = TRUE;
    }

    // if a translucent brush or a translucent portal
    if (needTranslucency || ((polygon.bpo_ulFlags & BPOF_PORTAL) && (polygon.bpo_ulFlags & BPOF_TRANSLUCENT)))
    {
      polygon.bpo_ulFlags |= BPOF_RENDERTRANSLUCENT;
      // TODO: RT: translucent brushes
    }
    else
    {

    }
  #pragma endregion

    // add brush polygon
  #pragma region AddPolygonToScene

    INDEX firstVertexId = RT_AllSectorVertices.Count();
    INDEX *origIndices = &polygon.bpo_aiTriangleElements[0];

    INDEX vertCount = polygon.bpo_apbvxTriangleVertices.Count();
    GFXVertex *vertices = RT_AllSectorVertices.Push(vertCount);

    for (INDEX i = 0; i < vertCount; i++)
    {
      CBrushVertex *brushVert = polygon.bpo_apbvxTriangleVertices[i];
      vertices[i].x = brushVert->bvx_vRelative(1);
      vertices[i].y = brushVert->bvx_vRelative(2);
      vertices[i].z = brushVert->bvx_vRelative(3);
      vertices[i].shade = 0;
    }

    INDEX indexCount = polygon.bpo_aiTriangleElements.Count();
    INDEX *indices = RT_AllSectorIndices.Push(indexCount);

    // set new indices relative to the shift in RT_AllSectorVertices
    for (INDEX i = 0; i < indexCount; i++)
    {
      indices[i] = origIndices[i] + firstVertexId;
    }

  #pragma endregion

    // TODO: RT: brush textures
    //COLOR color = polygon.bpo_colColor;
    //CBrushPolygonTexture *textures = polygon.bpo_abptTextures;
  }
#pragma region RenderSceneZOnly
#pragma endregion

  CEntity *brushEntity = brush->br_penEntity;

  const CPlacement3D &placement = brushEntity->en_ulPhysicsFlags & EPF_MOVABLE ?
    brushEntity->GetLerpedPlacement() :
    brushEntity->en_plPlacement;

  FLOAT3D position = placement.pl_PositionVector;
  FLOATmatrix3D rotation;
  MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

  SSRT::CBrushGeometry brushInfo = {};
  brushInfo.pOriginalEntity = brush->br_penEntity;
  brushInfo.isEnabled = true;
  brushInfo.color = RGBAToColor(255, 255, 255, 255);
  brushInfo.absPosition = position;
  brushInfo.absRotation = rotation;
  brushInfo.vertexCount = RT_AllSectorVertices.Count();
  brushInfo.vertices = &RT_AllSectorVertices[0];
  brushInfo.texCoords = nullptr;
  brushInfo.normals = nullptr;
  brushInfo.indexCount = RT_AllSectorIndices.Count();
  brushInfo.indices = &RT_AllSectorIndices[0];

  ssrt->AddBrush(brushInfo);
}


void RT_AddNonZoningBrush(CEntity *penBrush, CBrushSector *pbscThatAdds, SSRT::SSRTMain *ssrt)
{
  ASSERT(penBrush != NULL);
  // get its brush
  CBrush3D &brBrush = *penBrush->en_pbrBrush;

  // if hidden
  if (penBrush->en_ulFlags & ENF_HIDDEN)
  {
    // skip it
    return;
  }

  // RT: this must not happen, as iteration in AddAllEntities is done over all brushes
  //// if the brush is already added
  //if (brBrush.br_lnInActiveBrushes.IsLinked())
  //{
  //  // skip it
  //  return;
  //}

  if (penBrush->en_ulFlags & ENF_ZONING)
  {
    return;
  }

  // skip whole non-zoning brush if all polygons in all sectors are invisible for rendering 
  bool isVisible = false;

  // test every brush polygon for it's visibility flag
  // for every sector in brush
  FOREACHINDYNAMICARRAY(brBrush.GetFirstMip()->bm_abscSectors, CBrushSector, itbsc)
  {
    // for all polygons in sector
    FOREACHINSTATICARRAY(itbsc->bsc_abpoPolygons, CBrushPolygon, itpo)
    {
      // advance to next polygon if invisible
      CBrushPolygon &bpo = *itpo;
      if (!(bpo.bpo_ulFlags & BPOF_INVISIBLE))
      {
        isVisible = true;
        break;
      }
    }

    if (isVisible)
    {
      break;
    }
  }

  if (!isVisible)
  {
    return;
  }

  // RT: get highest mip
  CBrushMip *pbm = brBrush.GetBrushMipByDistance(0);

  // if brush mip exists for that mip factor
  if (pbm != NULL)
  {
    // for each sector
    FOREACHINDYNAMICARRAY(pbm->bm_abscSectors, CBrushSector, itbsc)
    {
      // if the sector is not hidden
      if (!(itbsc->bsc_ulFlags & BSCF_HIDDEN))
      {
        // add that sector to active sectors
        RT_AddActiveSector(itbsc.Current(), ssrt);
      }
    }
  }
}
