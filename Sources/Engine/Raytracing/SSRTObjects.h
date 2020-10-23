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

namespace SSRT
{ 

// attachments can have attachments, SSRT_MAX_ATTCH_DEPTH defines the max depth
#define SSRT_MAX_ATTACHMENT_DEPTH 8

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

}