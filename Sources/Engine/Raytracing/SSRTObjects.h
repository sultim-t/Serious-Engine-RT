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


struct CAbstractGeometry
{
  ULONG               entityID;

  // absolute position and rotation
  FLOAT3D             absPosition;
  FLOATmatrix3D       absRotation;

  // Data that should be set to the renderer,
  // vertices are already animated.
  // This data will become invalid after adding CModelGeometry
  // as SE will reset global arrays
  INDEX               vertexCount;
  const RgVertex      *vertices;

  // each 3 indices make a triangle
  INDEX               indexCount;
  const INDEX         *indices;

  CTextureData        *textures[3];
  INDEX               textureFrames[3];

  RgGeometryPassThroughType passThroughType;

  bool                blendEnable;
  RgBlendFactor       blendSrc;
  RgBlendFactor       blendDst;

  bool                isSky;
  bool                isEmissive;
  bool                generateNormals;

  RgGeometryPrimaryVisibilityType visibilityType;

public:
  virtual ~CAbstractGeometry() = default;
  virtual uint64_t    GetUniqueID() const = 0;
};


// Dynamic geometry that changes every frame
struct CModelGeometry : public CAbstractGeometry
{
  Vector<FLOAT, 4>    color;

  bool                invertedNormals;
  bool                generateNormals;

  bool                isSpecular;
  bool                isReflective;

  bool                isRasterized;

  // path to attachment using attachment position ID,
  // -1 means the end of the list
  INDEX               attchPath[SSRT_MAX_ATTACHMENT_DEPTH] = {};
  uint32_t            modelPartIndex;

public:
  uint64_t GetUniqueID() const override;
};


// Static geometry, vertex data doesn't change. 
// If it's movable, then transformation can be changed
struct CBrushGeometry : public CAbstractGeometry
{
  // one color per material
  Vector<FLOAT, 4>              layerColors[3];
  RgGeometryMaterialBlendType   layerBlendings[3];

  bool                isMovable = false;
  CTextureObject      *textureObjects[3];

  uint32_t            brushPartIndex;
  bool                hasScrollingTextures;

  bool                isRasterized;

  bool                noMediaChangeOnRefraction;

public:
  static uint64_t GetBrushUniqueID(ULONG entityID, uint32_t brushPartIndex);
  uint64_t GetUniqueID() const override;
};


struct CUpdateTexCoordsInfo
{
  ULONG               brushEntityID;
  uint32_t            brushPartIndex;
  INDEX               vertexCount;
  GFXTexCoord         *texCoordLayers[3];
};


struct CParticlesGeometry
{
  // absolute position and rotation
  FLOAT3D             absPosition;
  FLOATmatrix3D       absRotation;

  INDEX               vertexCount;
  const RgVertex      *pVertexData;

  // each 3 indices make a triangle
  INDEX               indexCount;
  const INDEX         *pIndexData;

  CTextureData        *pTexture;
  INDEX               textureFrame;

  bool                blendEnable;
  RgBlendFactor       blendSrc;
  RgBlendFactor       blendDst;

  bool                isSky;
};


struct CDirectionalLight
{
  ULONG           entityID;

  FLOAT3D         direction;
  FLOAT3D         color;
};


struct CSphereLight
{ 
  const CLightSource *plsLight;
  ULONG           entityID;

  FLOAT3D         absPosition;
  FLOAT3D         color;
  float           radius;
  float           falloffDistance;
};


struct CSpotLight
{
  FLOAT3D         absPosition;
  FLOAT3D         direction;
  FLOAT3D         upVector;
  bool            isFirstPerson;
};


struct CWorldRenderingInfo
{
  // top left point
  uint32_t        screenX;
  uint32_t        screenY;
  uint32_t        screenWidth;
  uint32_t        screenHeight;

  CEntity         *pViewerEntity;
  CWorld          *pWorld;
  CDrawPort       *pDrawport;
  CAnyProjection3D prProjection;
  CAnyProjection3D prProjectionBackground;

  FLOAT3D         cameraPosition;
  FLOATmatrix3D   cameraRotation;

  // horizontal FOV in degrees
  float           fovH;
  float           cameraFar;
  
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
  // bool           alphaTest;
  // always TRUE
  // bool           blendEnable;
  // always GFX_SRC_ALPHA
  //GfxBlend        blendFuncSrc;
  // always GFX_INV_SRC_ALPHA
  //GfxBlend        blendFuncDst;

  GfxWrap           textureWrapU;
  GfxWrap           textureWrapV;

  // use CTextureData, assuming that there is only 1 frame
  CTextureData      *textureData;

  const GFXVertex4  *pPositions;
  const GFXTexCoord *pTexCoords;
  const GFXColor    *pColors;
  INDEX             vertexCount;

  const INDEX       *pIndices; 
  INDEX             indexCount;
};


struct CPreparedTextureInfo
{
  uint32_t              textureID;
  const CTFileName      *path;
  bool                  isDynamic;
  void                  *imageData;
  uint32_t              width;
  uint32_t              height;
  bool                  generateMipmaps;
  RgSamplerFilter       filter;
  RgSamplerAddressMode  wrapU;
  RgSamplerAddressMode  wrapV;
  bool                  disableOverride;
};


struct CPreparedAnimatedTextureInfo
{
  uint32_t              textureID;
  const CTFileName      *path;
  void                  *frameData;
  uint32_t              frameCount;
  uint64_t              frameStride;
  uint32_t              width;
  uint32_t              height;
  bool                  generateMipmaps;
  RgSamplerFilter       filter;
  RgSamplerAddressMode  wrapU;
  RgSamplerAddressMode  wrapV;
};

}