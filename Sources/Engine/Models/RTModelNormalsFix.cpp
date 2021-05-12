/* Copyright (c) 2021 Sultim Tsyrendashiev
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

#include <vector>

#include "ModelData.h"
#include "Model_internal.h"
#include "Normals.h"

#include <Engine/Templates/StaticStackArray.cpp>

static void RT_AddTriangleNormal(CModelData *pModelData, std::vector<FLOAT3D> &pNormals, INDEX iFr, 
                                 INDEX i0, INDEX i1, INDEX i2)
{
  // triangle vertex indices for md_FrameVertices
  INDEX iFVtx[] =
  {
    iFr * pModelData->md_VerticesCt + i0,
    iFr * pModelData->md_VerticesCt + i1,
    iFr * pModelData->md_VerticesCt + i2,
  };

  FLOAT3D pTriVerts[3];

  if (pModelData->md_Flags & MF_COMPRESSED_16BIT)
  {
    for (INDEX j = 0; j < 3; j++)
    {
      const auto &swVert = pModelData->md_FrameVertices16[iFVtx[j]].mfv_SWPoint;
      pTriVerts[j](1) = swVert(1);
      pTriVerts[j](2) = swVert(2);
      pTriVerts[j](3) = swVert(3);
    }
  }
  else
  {
    for (INDEX j = 0; j < 3; j++)
    {
      const auto &sbVert = pModelData->md_FrameVertices8[iFVtx[j]].mfv_SBPoint;
      pTriVerts[j](1) = sbVert(1);
      pTriVerts[j](2) = sbVert(2);
      pTriVerts[j](3) = sbVert(3);
    }
  }

  FLOAT3D v1 = pTriVerts[1] - pTriVerts[0];
  FLOAT3D v2 = pTriVerts[2] - pTriVerts[0];
  v1.SafeNormalize();
  v2.SafeNormalize();

  FLOAT3D vTriangleNormal = v1 * v2;

  pNormals[i0] += vTriangleNormal;
  pNormals[i1] += vTriangleNormal;
  pNormals[i2] += vTriangleNormal;
}

void RT_FixModelNormals(CModelData *pModelData)
{
  return;

  // assume that main mip contains all triangles
  const auto &pMainMip = pModelData->md_MipInfos[0];
  const auto &pMipIndices = pMainMip.mmpi_aiElements;
  const UWORD *puwMipToMdl = &pMainMip.mmpi_auwSrfToMip[0];

  // init all normals as 0
  std::vector<FLOAT3D> pNormals(pModelData->md_VerticesCt);

  // for each frame which contain vertex data
  for (INDEX iFr = 0; iFr < pModelData->md_FramesCt; iFr++)
  {
    for (auto &n : pNormals)
    {
      n = FLOAT3D(0, 0, 0);
    }

    if (pMipIndices.Count() > 0)
    {
      for (INDEX i = 0; i < pMipIndices.Count(); i += 3)
      {
        // need to convert mip indices to model indices
        const INDEX iMdlVx[] =
        {
          puwMipToMdl[pMipIndices[i + 0]],
          puwMipToMdl[pMipIndices[i + 1]],
          puwMipToMdl[pMipIndices[i + 2]],
        };


        RT_AddTriangleNormal(pModelData, pNormals, iFr, 
                             iMdlVx[0], iMdlVx[1], iMdlVx[2]);
      }
    }
    else
    {
      // no indices, triangle is a sequence of 3 vertices
      for (INDEX v = 0; v < pModelData->md_VerticesCt; v += 3)
      {
        RT_AddTriangleNormal(pModelData, pNormals, iFr, 
                             v + 0, v + 1, v + 2);
      }
    }


    // normalize normal sum in each vertex for current frame and store it
    for (INDEX iVtx = 0; iVtx < pModelData->md_VerticesCt; iVtx++)
    {
      // vertex index for md_FrameVertices
      INDEX iFVtx = iFr * pModelData->md_VerticesCt + iVtx;

      FLOAT3D vNormal = pNormals[iVtx];
      vNormal.SafeNormalize();

      if (pModelData->md_Flags & MF_COMPRESSED_16BIT)
      {
        CompressNormal_HQ(vNormal, pModelData->md_FrameVertices16[iFVtx].mfv_ubNormH,
                          pModelData->md_FrameVertices16[iFVtx].mfv_ubNormP);
      }
      else
      {
        pModelData->md_FrameVertices8[iFVtx].mfv_NormIndex = (UBYTE)GouraudNormal(vNormal);
      }
    }
  }
}
