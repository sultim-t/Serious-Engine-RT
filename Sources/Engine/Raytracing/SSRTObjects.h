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

#pragma once

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>

class CRenderModel;

namespace SSRT
{ 

// attachments can have attachments, SSRT_MAX_ATTCH_DEPTH defines the max depth
#define SSRT_MAX_ATTACHMENT_DEPTH 8

#define SSRT_FIRSTPERSON_ENTITY_START_ID 2000000000

struct RTObject
{
  ULONG           entityID;
  bool            isEnabled;

public:
  //ULONG           GetEnitityID() const;
  bool            operator==(const RTObject &other) const;

  virtual         ~RTObject() = 0;
};

struct CAbstractGeometry : RTObject
{
  // absolute position and rotation
  FLOAT3D         absPosition;
  FLOATmatrix3D   absRotation;

  COLOR           color;

  // Data that should be set to the renderer,
  // vertices are already animated.
  // This data will become invalid after adding CModelGeometry
  // as SE will reset global arrays
  INDEX           vertexCount;
  GFXVertex       *vertices;
  GFXNormal       *normals;
  GFXTexCoord     *texCoords;
  INDEX           indexCount;
  INDEX           *indices;
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
  ULONG           screenX;
  ULONG           screenY;
  ULONG           screenWidth;
  ULONG           screenHeight;
  ULONG           viewerEntityID;
  //CWorld          *world;

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


struct CHudElementInfo
{
  bool            depthTest;
  bool            depthWrite;
  bool            alphaTest;
  bool            blendEnable;
  GfxBlend        blendFuncSrc;
  GfxBlend        blendFuncDst;
  bool            textureEnable;
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