/* Copyright (c) 2002-2012 Croteam Ltd. 
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

#ifndef SE_INCL_MESH_H
#define SE_INCL_MESH_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/CTString.h>
#include <Engine/Base/Serial.h>
#include <Engine/Math/Vector.h>
#include <Engine/Math/Placement.h>
#include <Engine/Templates/DynamicArray.h>
#include <Engine/Templates/StaticArray.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/Shader.h>

#define ML_HALF_FACE_FORWARD (1UL<<0)  // half face forward
#define ML_FULL_FACE_FORWARD (1UL<<1)  // full face forward

struct ENGINE_API MeshLOD
{
  MeshLOD() {
    mlod_fMaxDistance = -1;
    mlod_ulFlags      =  0;
  };
  ~MeshLOD() {}
  FLOAT mlod_fMaxDistance;
  ULONG mlod_ulFlags;
  CStaticArray<struct MeshVertex>    mlod_aVertices;   // vertices
  CStaticArray<struct MeshNormal>    mlod_aNormals;	   // normals
  CStaticArray<struct MeshUVMap>     mlod_aUVMaps;     // UV maps
  CStaticArray<struct MeshSurface>   mlod_aSurfaces;   // surfaces
  CStaticArray<struct MeshWeightMap> mlod_aWeightMaps; // weight maps
  CStaticArray<struct MeshMorphMap>  mlod_aMorphMaps;  // morph maps
  CTString mlod_fnSourceFile;// file name of ascii am file, used in Ska studio
};

struct ENGINE_API MeshVertex
{
  FLOAT x, y, z;
  ULONG dummy; // 16 byte alingment
};

struct ENGINE_API MeshNormal
{
  FLOAT nx, ny, nz;
  ULONG dummy; // 16 byte alingment
};

struct ENGINE_API MeshUVMap
{
  ULONG muv_iID;
  CStaticArray<struct MeshTexCoord>  muv_aTexCoords; // texture coordinates
};

struct ENGINE_API MeshTexCoord
{
  FLOAT u, v;
};

struct ENGINE_API MeshSurface
{
  INDEX msrf_iFirstVertex;
  INDEX msrf_ctVertices;
  INDEX msrf_iSurfaceID;
  CShader *msrf_pShader;
  ShaderParams msrf_ShadingParams;
  CStaticArray<struct MeshTriangle> msrf_aTriangles;		// list of triangles
};

struct ENGINE_API MeshTriangle
{
  INDEX iVertex[3];  
};

struct ENGINE_API MeshWeightMap
{
  int mwm_iID;
  CStaticArray<struct MeshVertexWeight> mwm_aVertexWeight; // weight maps
};

struct ENGINE_API MeshVertexWeight
{
  INDEX mww_iVertex;      // absolute index of the vertex this weight refers to
  FLOAT mww_fWeight;      // weight for this bone [0.0 - 1.0]
};

struct ENGINE_API MeshMorphMap
{
  INDEX mmp_iID;
  BOOL  mmp_bRelative;
  CStaticArray<struct MeshVertexMorph> mmp_aMorphMap; // Morph maps
};

struct ENGINE_API MeshVertexMorph
{
  INDEX mwm_iVxIndex;      // absolute index of the vertex this weight refers to
  FLOAT mwm_x;
  FLOAT mwm_y;
  FLOAT mwm_z;
  FLOAT mwm_nx;
  FLOAT mwm_ny;
  FLOAT mwm_nz;
  ULONG dummy;        // 32 byte padding
};

class ENGINE_API CMesh : public CSerial
{
public:
  CMesh();
  ~CMesh();
  void Optimize(void);
  void OptimizeLod(MeshLOD &mLod);
  void NormalizeWeights(void);
  void NormalizeWeightsInLod(MeshLOD &mlod);

  void AddMeshLod(MeshLOD &mlod);
  void RemoveMeshLod(MeshLOD *pmlodRemove);
  void Read_t( CTStream *istrFile);  // throw char *
  void Write_t( CTStream *ostrFile); // throw char *
  void Clear(void);
  SLONG GetUsedMemory(void);
  CStaticArray<struct MeshLOD> msh_aMeshLODs;
}; 
ENGINE_API void ChangeSurfaceShader_t(MeshSurface &msrf,CTString fnNewShader);


#endif  /* include-once check. */

