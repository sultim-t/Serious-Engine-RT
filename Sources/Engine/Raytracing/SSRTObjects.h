/* Copyright (c) 2020-2021 Sultim Tsyrendashiev
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

#pragma once

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>
#include <RTGL1/RTGL1.h>

#define RG_CHECKERROR(x) ASSERT(x == RG_SUCCESS)

class CRenderModel;

namespace SSRT
{ 

// attachments can have attachments, SSRT_MAX_ATTCH_DEPTH defines the max depth
#define SSRT_MAX_ATTACHMENT_DEPTH 8
// fake entity IDs for first person models
#define SSRT_FIRSTPERSON_ENTITY_START_ID 2000000000
#define SSRT_MATERIAL_LAYER_COUNT 3


struct RTObject
{
  ULONG           entityID;
  bool            isEnabled;

public:
  //ULONG           GetEnitityID() const;
  bool            operator==(const RTObject &other) const;

  virtual         ~RTObject() = 0;
};


struct CTexture
{
  const char      *name;

};


struct CMaterial
{
  // ID of this material
  uint32_t        materialId;
  // base texture data
  CTextureData    *textureData;
};


// Each triangle has 3 materials (the 4th one is shadowlayer)
struct CTriangleMaterial
{
  uint32_t        layerMaterialId[SSRT_MATERIAL_LAYER_COUNT];
};


struct CAbstractGeometry : RTObject
{
  // absolute position and rotation
  FLOAT3D             absPosition;
  FLOATmatrix3D       absRotation;

  COLOR               color;
  // if geometry is using only one material, then 
  // this value will be used, otherwise look at materialIds
  uint32_t            globalMaterialId;

  // Data that should be set to the renderer,
  // vertices are already animated.
  // This data will become invalid after adding CModelGeometry
  // as SE will reset global arrays
  INDEX               vertexCount;
  GFXVertex           *vertices;
  GFXNormal           *normals;
  GFXTexCoord         *texCoords;
  // each 3 indices make a triangle
  INDEX               indexCount;
  INDEX               *indices;
  // material ID for each triangle (i.e. for each 3 indices)
  CTriangleMaterial   materials;
  RgGeometry          rgGeomId;
};


// Dynamic geometry that changes every frame
struct CModelGeometry : public CAbstractGeometry
{
  // path to attachment using attachment position ID,
  // -1 means the end of the list
  INDEX           attchPath[SSRT_MAX_ATTACHMENT_DEPTH];
  // for first person weapons
  bool            isFirstPerson;

public:
  //CModelObject    *GetModelObject();
  bool            operator==(const CModelGeometry &other) const;
};


// Static geometry, vertex data doesn't change. 
// If it's movable, then transformation can be changed
struct CBrushGeometry : public CAbstractGeometry
{
public:
  //bool            IsMovable() const;
  //CBrush3D        *GetOriginalBrush();
  bool            operator==(const CBrushGeometry &other) const;
};


struct CDirectionalLight : public RTObject
{
  FLOAT3D         direction;
  COLOR           color;
  // directional light angular size on a sky in degrees
  float           angularSize;
};


struct CSphereLight : public RTObject
{
  FLOAT3D         absPosition;
  COLOR           color;
  float           intensity;
  float           sphereRadius;
};


struct CWorldRenderingInfo
{
  // top left point
  uint32_t        screenX;
  uint32_t        screenY;
  uint32_t        screenWidth;
  uint32_t        screenHeight;

  ULONG           viewerEntityID;
  CWorld          *world;

  FLOAT3D         viewerPosition;
  FLOATmatrix3D   viewerRotation;

  // horizontal FOV in degrees
  float           fovH;

  // optional matrices
  // perspective projection matrix
  float           projectionMatrix[16];
  // view matrix
  float           viewMatrix[16];
};


struct CFirstPersonModelInfo
{
  CModelObject    *modelObject;
  CRenderModel    *renderModel;
  // overriden FOV for first person model
  float           fovH;
};


// Note: depth test/write is disabled for HUD
struct CHudElementInfo
{
  // always FALSE
  // bool            alphaTest;
  // always TRUE
  // bool            blendEnable;
  // always GFX_SRC_ALPHA
  //GfxBlend        blendFuncSrc;
  // always GFX_INV_SRC_ALPHA
  //GfxBlend        blendFuncDst;
  GfxWrap         textureWrapU;
  GfxWrap         textureWrapV;
  CTextureData    *textureData;

  GFXVertex4      *pPositions; 
  GFXTexCoord     *pTexCoords;
  GFXColor        *pColors; 
  INDEX           vertexCount;
  INDEX           *pIndices; 
  INDEX           indexCount;
};

}