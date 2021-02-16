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

#include "StdH.h"
#include "RTProcessing.h"

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>
#include <Engine/Brushes/BrushTransformed.h>
#include <Engine/Graphics/RenderScene.h>
#include <Engine/Raytracing/SSRT.h>
#include <Engine/World/World.h>
#include <Engine/World/WorldSettings.h>

#include <Engine/Templates/DynamicArray.h>
#include <Engine/Templates/DynamicArray.cpp>



#define MAX_BRUSH_TEXTURE_COUNT 3



static CStaticStackArray<GFXVertex> RT_AllSectorVertices;
static CStaticStackArray<GFXNormal> RT_AllSectorNormals;
static CStaticStackArray<INDEX> RT_AllSectorIndices;
static CStaticStackArray<GFXTexCoord> RT_AllSectorTexCoords[MAX_BRUSH_TEXTURE_COUNT];


struct RT_WorldBaseIgnore
{
  const char *worldName;
  FLOAT3D position;
};


// Positions of unnecessary brushes (that also have "World Base" names)
static const FLOAT3D RT_WorldBaseForceInvisibleEpsilon = { 0.5f, 0.5f, 0.5f };
static const RT_WorldBaseIgnore RT_WorldBaseForceInvisible[] =
{
  { "01_Hatshepsut",        { 0, 1152, 64 }       },
  { "02_SandCanyon",        { 224, 128, -312}     },
  { "02_SandCanyon",        { 224, 128, -120}     },
  { "02_SandCanyon",        { -98.5f, 113.5f, 18} },
  { "04_ValleyOfTheKings",  { 0, 96, 0 }          },
  { "05_MoonMountains",     { -64, 80, -64}       },
  { "06_Oasis",             { 128, -16, 0}        },
};


struct RT_TextureLayerBlending
{
  UBYTE layerBlendingType[MAX_BRUSH_TEXTURE_COUNT];
  COLOR layerColor[MAX_BRUSH_TEXTURE_COUNT];
};


static RgGeometryPassThroughType RT_GetPassThroughType(uint32_t flags)
{
  switch (flags)
  {
    case BPOF_TRANSPARENT:  return RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
    case BPOF_TRANSLUCENT:  return RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
    default:                return RG_GEOMETRY_PASS_THROUGH_TYPE_OPAQUE;
  }
}


static RgGeometryMaterialBlendType RT_GetMaterialBlendType(UBYTE layerBlendingType)
{
  switch (layerBlendingType & STXF_BLEND_MASK)
  {
    case STXF_BLEND_OPAQUE: return RG_GEOMETRY_MATERIAL_BLEND_TYPE_OPAQUE;
    case STXF_BLEND_ALPHA:  return RG_GEOMETRY_MATERIAL_BLEND_TYPE_ALPHA;
    case STXF_BLEND_ADD:    return RG_GEOMETRY_MATERIAL_BLEND_TYPE_ADD;
    case STXF_BLEND_SHADE:  return RG_GEOMETRY_MATERIAL_BLEND_TYPE_SHADE;
    default:                return RG_GEOMETRY_MATERIAL_BLEND_TYPE_OPAQUE;
  }
}


static void RT_FlushBrushInfo(CEntity *penBrush, 
                              CBrushPolygonTexture *textures[MAX_BRUSH_TEXTURE_COUNT], 
                              uint32_t polygonFlags,
                              const RT_TextureLayerBlending &blending,
                              SSRT::Scene *scene)
{
  if (RT_AllSectorVertices.Count() == 0 || RT_AllSectorIndices.Count() == 0)
  {
    return;
  }

  bool isMovable = penBrush->en_ulPhysicsFlags & EPF_MOVABLE;

  const CPlacement3D &placement = isMovable ?
    penBrush->GetLerpedPlacement() :
    penBrush->en_plPlacement;

  FLOAT3D position = placement.pl_PositionVector;
  FLOATmatrix3D rotation;
  MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

  SSRT::CBrushGeometry brushInfo = {};
  brushInfo.entityID = penBrush->en_ulID;
  brushInfo.isMovable = isMovable;

  brushInfo.absPosition = position;
  brushInfo.absRotation = rotation;

  brushInfo.vertexCount = RT_AllSectorVertices.Count();
  brushInfo.vertices = &RT_AllSectorVertices[0];
  brushInfo.texCoordLayers[0] = &RT_AllSectorTexCoords[0][0];
  brushInfo.texCoordLayers[1] = &RT_AllSectorTexCoords[1][0];
  brushInfo.texCoordLayers[2] = &RT_AllSectorTexCoords[2][0];
  brushInfo.normals = &RT_AllSectorNormals[0];

  brushInfo.indexCount = RT_AllSectorIndices.Count();
  brushInfo.indices = &RT_AllSectorIndices[0];

  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    if (textures[i] == nullptr)
    {
      continue;
    }

    CTextureObject &to = textures[i]->bpt_toTexture;
    CTextureData *td = (CTextureData *)to.GetData();

    brushInfo.textureObjects[i] = &to;

    if (td != nullptr)
    {
      brushInfo.textures[i] = td;
      brushInfo.textureFrames[i] = to.GetFrame();
      
      brushInfo.layerBlendings[i] = RT_GetMaterialBlendType(blending.layerBlendingType[i]);

      GFXColor gcolor = GFXColor(blending.layerColor[i]);
      Vector<FLOAT, 4> fcolor = { (float)gcolor.r, (float)gcolor.g , (float)gcolor.b, (float)gcolor.a };
      fcolor /= 255.0f;
      
      brushInfo.layerColors[i] = fcolor;

      // texture modulation for second and third textures (RSSetInitialTextureParametersMT)
      if (i > 0)
      {
        // alpha-blended texture are oversaturated, ignore them
        if (brushInfo.layerBlendings[i] == RG_GEOMETRY_MATERIAL_BLEND_TYPE_SHADE)
        {
          brushInfo.layerColors[i] *= 2;
        }
      }
    }
  }

  // get polygon's blending type
  brushInfo.passThroughType = RT_GetPassThroughType(polygonFlags);

  scene->AddBrush(brushInfo);

  RT_BrushClear();
}


static bool RT_AreTexturesSame(CBrushPolygonTexture *pPrevTextures[MAX_BRUSH_TEXTURE_COUNT],
                               CBrushPolygonTexture curTextures[MAX_BRUSH_TEXTURE_COUNT])
{
  CTextureData *cur[MAX_BRUSH_TEXTURE_COUNT];
  CTextureData *last[MAX_BRUSH_TEXTURE_COUNT];

  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    last[i] = pPrevTextures[i] == nullptr? nullptr : (CTextureData *)pPrevTextures[i]->bpt_toTexture.GetData();
    cur[i] = (CTextureData *)curTextures[i].bpt_toTexture.GetData();
  }

  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    if ((cur[i] == nullptr && last[i] != nullptr) || 
        (cur[i] != nullptr && last[i] == nullptr))
    {
      return false;
    }

    if (cur[i] != nullptr && last[i] != nullptr)
    {
      if (cur[i]->td_ulObject != last[i]->td_ulObject)
      {
        return false;
      }
    }
  }

  return true;
}


static bool RT_AreBlendingsSame(const RT_TextureLayerBlending &a, const RT_TextureLayerBlending &b)
{
  for (INDEX i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    if (a.layerBlendingType[i] != b.layerBlendingType[i] ||
        a.layerColor[i] != b.layerColor[i])
    {
      return false;
    }
  }

  return true;
}


static void RT_AddActiveSector(CBrushSector &bscSector, CEntity *penBrush, SSRT::Scene *scene)
{
  ASSERT(RT_AllSectorVertices.Count() == 0 || RT_AllSectorIndices.Count() == 0);

  CBrush3D *brush = bscSector.bsc_pbmBrushMip->bm_pbrBrush;
  if (brush->br_pfsFieldSettings != NULL)
  {
    return;
  }

  // keep last rendered info, to batch geometry
  CBrushPolygonTexture *pLastTextures[MAX_BRUSH_TEXTURE_COUNT] = {};
  COLOR lastColor = C_WHITE | CT_OPAQUE;
  uint32_t lastFlags = UINT32_MAX;
  RT_TextureLayerBlending lastBlending = {};
  for (INDEX i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    lastBlending.layerColor[i] = C_WHITE | CT_OPAQUE;
    lastBlending.layerBlendingType[i] = STXF_BLEND_OPAQUE;
  }

  FOREACHINSTATICARRAY(bscSector.bsc_abpoPolygons, CBrushPolygon, itpo)
  {
    CBrushPolygon &polygon = *itpo;

    // only portals that are translucent should be rendered
    bool isPortal = polygon.bpo_ulFlags & BPOF_PORTAL;
    bool isTranslucent = polygon.bpo_ulFlags & BPOF_TRANSLUCENT;

    // if shouldn't be rendered in game
    if ((isPortal && !isTranslucent) ||
        (polygon.bpo_ulFlags & BPOF_INVISIBLE) ||
        (polygon.bpo_ulFlags & BPOF_OCCLUDER))
    {
      continue;
    }

    // for texture cordinates and transparency/translucency processing
  #pragma region MakeScreenPolygon

    uint32_t polygonFlags = 0;

    if (polygon.bpo_ulFlags & BPOF_TRANSPARENT)
    {
      polygonFlags |= BPOF_TRANSPARENT;
    }

    if (isPortal && isTranslucent)
    {
      polygonFlags |= BPOF_TRANSLUCENT;
    }

    RT_TextureLayerBlending curBlending = {};

    // if GF_FLAT mode, i.e. pspo->spo_aptoTextures[0]==NULL || !bTexture0
    if (polygon.bpo_abptTextures[0].bpt_toTexture.GetData() == nullptr)
    {
      curBlending.layerBlendingType[0] = STXF_BLEND_OPAQUE;
      curBlending.layerColor[0] = polygon.bpo_colColor;

      for (INDEX i = 1; i < MAX_BRUSH_TEXTURE_COUNT; i++)
      {
        curBlending.layerBlendingType[i] = STXF_BLEND_OPAQUE;
        curBlending.layerColor[i] = C_WHITE | CT_OPAQUE;
      }
    }
    else
    {
      for (INDEX i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
      {
        CBrushPolygonTexture &layerTexture = polygon.bpo_abptTextures[i];
        CTextureBlending &tb = scene->GetWorld()->wo_atbTextureBlendings[layerTexture.s.bpt_ubBlend];
        
        COLOR colorLayer = layerTexture.s.bpt_colColor;

        const float fOpacity = brush->br_penEntity->GetOpacity();

        // process transparency color
        if (fOpacity < 1)
        {
          const SLONG slOpacity = NormFloatToByte(fOpacity);
          UBYTE &layerBlending = tb.tb_ubBlendingType;

          // if texture is opaque 
          if ((layerBlending & STXF_BLEND_MASK) == 0)
          {
            // set it to blend with opaque alpha
            layerBlending |= STXF_BLEND_ALPHA;
            colorLayer |= CT_AMASK;
          }

          // if texture is blended
          if (layerBlending & STXF_BLEND_ALPHA)
          {
            // adjust it's alpha factor
            SLONG slAlpha = (colorLayer & CT_AMASK) >> CT_ASHIFT;
            slAlpha = (slAlpha * slOpacity) >> 8;

            colorLayer &= ~CT_AMASK;
            colorLayer |= slAlpha;
          }
        }

        colorLayer = MulColors(colorLayer, tb.tb_colMultiply);

        curBlending.layerBlendingType[i] = tb.tb_ubBlendingType;
        curBlending.layerColor[i] = colorLayer;
      }
    }

  #pragma endregion

  #pragma region Flush if different data
    // if settings are different
    if (!RT_AreTexturesSame(pLastTextures, polygon.bpo_abptTextures) ||
        !RT_AreBlendingsSame(lastBlending, curBlending) ||
        lastFlags != polygonFlags)
    {
      // flush last saved info, and start new recordings
      RT_FlushBrushInfo(penBrush, pLastTextures, lastFlags, lastBlending, scene);
    }
  #pragma endregion

  #pragma region Positions
    INDEX firstVertexId = RT_AllSectorVertices.Count();
    INDEX vertCount = polygon.bpo_apbvxTriangleVertices.Count();


    // position data
    GFXVertex *vertices = RT_AllSectorVertices.Push(vertCount);

    for (INDEX i = 0; i < vertCount; i++)
    {
      CBrushVertex *brushVert = polygon.bpo_apbvxTriangleVertices[i];
      vertices[i].x = brushVert->bvx_vRelative(1);
      vertices[i].y = brushVert->bvx_vRelative(2);
      vertices[i].z = brushVert->bvx_vRelative(3);
      vertices[i].shade = 0;
    }
  #pragma endregion

  #pragma region Normals
    GFXNormal *normals = RT_AllSectorNormals.Push(vertCount);

    for (INDEX i = 0; i < vertCount; i++)
    {
      float *planeNormal = polygon.bpo_pbplPlane->bpl_plRelative.vector;
      normals[i].nx = planeNormal[0];
      normals[i].ny = planeNormal[1];
      normals[i].nz = planeNormal[2];
      normals[i].ul = 0;
    }

  #pragma endregion

  #pragma region Texture coordinates
    for (uint32_t iLayer = 0; iLayer < MAX_BRUSH_TEXTURE_COUNT; iLayer++)
    {  
      // tex coord data: from RSSetTextureCoords(..)
      GFXTexCoord *texCoords = RT_AllSectorTexCoords[iLayer].Push(vertCount);


      CBrushPolygonTexture &layerTexture = polygon.bpo_abptTextures[iLayer];

      CTextureData *ptd = (CTextureData *)layerTexture.bpt_toTexture.GetData();
      if (ptd != nullptr)
      {
        const FLOAT mulU = 1024.0f / (float)ptd->GetWidth();
        const FLOAT mulV = 1024.0f / (float)ptd->GetHeight();

        FLOATplane3D &plane = polygon.bpo_pbplPlane->bpl_plRelative;

        CMappingVectors mvBrushSpace;
        mvBrushSpace.FromPlane(plane);

        CMappingVectors amvMapping;

        // if texture should be not transformed
        INDEX iTransformation = layerTexture.s.bpt_ubScroll;
        if (iTransformation == 0)
        {
          if ((polygon.bpo_abptTextures[iLayer].s.bpt_ubFlags & (BPTF_CLAMPU | BPTF_CLAMPV)) == 0)
          {
            // make a mapping adjusted for texture wrapping
            const MEX mexMaskU = ptd->GetWidth() - 1;
            const MEX mexMaskV = ptd->GetHeight() - 1;

            CMappingDefinition mdTmp = polygon.bpo_abptTextures[iLayer].bpt_mdMapping;
            mdTmp.md_fUOffset = (FloatToInt(mdTmp.md_fUOffset * 1024.0f) & mexMaskU) / 1024.0f;
            mdTmp.md_fVOffset = (FloatToInt(mdTmp.md_fVOffset * 1024.0f) & mexMaskV) / 1024.0f;

            const FLOAT3D vOffset = plane.ReferencePoint() - mvBrushSpace.mv_vO;

            const FLOAT fS = vOffset % mvBrushSpace.mv_vU;
            const FLOAT fT = vOffset % mvBrushSpace.mv_vV;
            const FLOAT fU = fS * mdTmp.md_fUoS + fT * mdTmp.md_fUoT + mdTmp.md_fUOffset;
            const FLOAT fV = fS * mdTmp.md_fVoS + fT * mdTmp.md_fVoT + mdTmp.md_fVOffset;

            mdTmp.md_fUOffset += (FloatToInt(fU * 1024.0f) & ~mexMaskU) / 1024.0f;
            mdTmp.md_fVOffset += (FloatToInt(fV * 1024.0f) & ~mexMaskV) / 1024.0f;

            // make texture mapping vectors from default vectors of the plane
            mdTmp.MakeMappingVectors(mvBrushSpace, amvMapping);
          }
          else
          {
            polygon.bpo_abptTextures[iLayer].bpt_mdMapping.MakeMappingVectors(mvBrushSpace, amvMapping);
          }
        }
        // if texture should be transformed
        else
        {
          // make mapping vectors as normal and then transform them
          CMappingDefinition &mdBase = polygon.bpo_abptTextures[iLayer].bpt_mdMapping;
          CMappingDefinition &mdScroll = scene->GetWorld()->wo_attTextureTransformations[iTransformation].tt_mdTransformation;

          CMappingVectors mvTmp;
          mdBase.MakeMappingVectors(mvBrushSpace, mvTmp);

          mdScroll.TransformMappingVectors(mvTmp, amvMapping);
        }

        // adjust MEX
        amvMapping.mv_vU *= mulU;
        amvMapping.mv_vV *= mulV;

        const FLOAT3D &vO = amvMapping.mv_vO;
        const FLOAT3D &vU = amvMapping.mv_vU;
        const FLOAT3D &vV = amvMapping.mv_vV;

        for (INDEX i = 0; i < vertCount; i++)
        {
          const FLOAT fDX = vertices[i].x - vO(1);
          const FLOAT fDY = vertices[i].y - vO(2);
          const FLOAT fDZ = vertices[i].z - vO(3);

          texCoords[i].s = vU(1) * fDX + vU(2) * fDY + vU(3) * fDZ;
          texCoords[i].t = vV(1) * fDX + vV(2) * fDY + vV(3) * fDZ;
        }
      }
    }
  #pragma endregion
  
  #pragma region Indices
    // index data
    INDEX indexCount = polygon.bpo_aiTriangleElements.Count();
    INDEX *origIndices = &polygon.bpo_aiTriangleElements[0];

    INDEX *indices = RT_AllSectorIndices.Push(indexCount);

    // set new indices relative to the shift in RT_AllSectorVertices
    for (INDEX i = 0; i < indexCount; i++)
    {
      indices[i] = origIndices[i] + firstVertexId;
    }
  #pragma endregion

    // rewrite last info
    lastFlags = polygonFlags;
    lastBlending = curBlending;
    for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
    {
      pLastTextures[i] = &polygon.bpo_abptTextures[i];

      auto *td = (CTextureData *)pLastTextures[i]->bpt_toTexture.GetData();

      if (td != nullptr)
      {
        // set new local params for the texture
        td->td_tpLocal.tp_eWrapU = pLastTextures[i]->s.bpt_ubFlags & BPTF_CLAMPU ? GFX_CLAMP : GFX_REPEAT;
        td->td_tpLocal.tp_eWrapV = pLastTextures[i]->s.bpt_ubFlags & BPTF_CLAMPV ? GFX_CLAMP : GFX_REPEAT;
      }
    }
  }

  // flush what's left
  RT_FlushBrushInfo(penBrush, pLastTextures, lastFlags, lastBlending, scene);
}


bool RT_IsBrushIgnored(CEntity *penBrush)
{
  // ugly way to remove terrain planes above the world
  if (!(penBrush->en_ulFlags & ENF_ZONING) && penBrush->GetName() == "World Base")
  {
    // check if it's exactly those unnecessary polygons
    for (auto &ignore : RT_WorldBaseForceInvisible)
    {
      // if not for the current world
      if (penBrush->GetWorld() == nullptr || penBrush->GetWorld()->wo_fnmFileName.FileName() != ignore.worldName)
      {
        continue;
      }

      FLOAT3D lowerBound = penBrush->GetPlacement().pl_PositionVector - RT_WorldBaseForceInvisibleEpsilon;
      FLOAT3D upperBound = penBrush->GetPlacement().pl_PositionVector + RT_WorldBaseForceInvisibleEpsilon;

      bool isInside = true;

      for (uint32_t i = 1; i <= 3; i++)
      {
        // if not inside bounds
        if (ignore.position(i) > upperBound(i) || ignore.position(i) < lowerBound(i))
        {
          isInside = false;
          break;
        }
      }

      if (isInside)
      {
        return true;
      }
    }
  }

  return false;
}


void RT_AddBrushEntity(CEntity *penBrush, SSRT::Scene *scene)
{
  ASSERT(penBrush != NULL);
  // get its brush
  CBrush3D &brBrush = *penBrush->en_pbrBrush;

  // if hidden
  if (penBrush->en_ulFlags & ENF_HIDDEN || penBrush->en_ulFlags & ENF_INVISIBLE)
  {
    // skip it
    return;
  }

  if (RT_IsBrushIgnored(penBrush))
  {
    return;
  }


  // RT: get highest mip
  CBrushMip *pbm = brBrush.GetFirstMip();

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
        RT_AddActiveSector(itbsc.Current(), penBrush, scene);
      }
    }
  }
}

void RT_BrushClear()
{
  RT_AllSectorNormals.Clear();
  RT_AllSectorVertices.Clear();
  RT_AllSectorIndices.Clear();

  for (uint32_t i = 0; i < MAX_BRUSH_TEXTURE_COUNT; i++)
  {
    RT_AllSectorTexCoords[i].Clear();
  }
}
