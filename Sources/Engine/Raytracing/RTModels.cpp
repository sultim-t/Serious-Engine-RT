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

#include <Engine/Entities/Entity.h>
#include <Engine/Models/RenderModel.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/Models/ModelData.h>
#include <Engine/Light/LightSource.h>
#include <Engine/World/World.h>
#include <Engine/World/WorldRayCasting.h>

#include <Engine/Base/ListIterator.inl>

#include "SSRT.h"
#include "RTProcessing.h"
#include "SSRTGlobals.h"



extern SSRT::SSRTGlobals _srtGlobals;

extern INDEX gfx_bRenderModels;
extern INDEX gfx_bRenderPredicted;



static uint32_t RT_ModelPartIndex = 0;

static bool RT_IsInvisibleEnemy = false;



struct RT_VertexData
{
  INDEX       vertexCount;
  GFXVertex   *pPositons;
  GFXNormal   *pNormals;
  GFXTexCoord *pTexCoords;
  GFXColor    *pColors;
};


static bool RT_ShouldBeAlphaTested(CTextureObject *to, SurfaceTranslucencyType stt, bool forceTranslucency, SSRT::Scene *pScene)
{ 
  // force translucency is for the cases when model changes its alpha value dynamically (e.g. fading out);
  // it works only on opaque or alpha-tested models
  bool changesDynamically = forceTranslucency && ((stt == STT_OPAQUE) || (stt == STT_TRANSPARENT));


  // if model changes (like disappering gibs, body parts) 
  return _srtGlobals.srt_bModelChangeableTranslucentToAlphaTested && changesDynamically;
}


static bool RT_ShouldBeRasterized(SurfaceTranslucencyType stt, bool forceAlphaTest)
{
  if (stt == STT_TRANSLUCENT)
  {
    return !forceAlphaTest;
  }
  else if (stt == STT_ADD)
  {
    return true;
  }
  else if (stt == STT_MULTIPLY)
  {
    return true;
  }
  else
  {
    return false;
  }
}


static void RT_SetBlend(bool &blendEnable, RgBlendFactor &blendSrc, RgBlendFactor &blendDst, SurfaceTranslucencyType stt, bool forceTranslucency = false)
{
  if (stt == STT_TRANSLUCENT || (forceTranslucency && ((stt == STT_OPAQUE) || (stt == STT_TRANSPARENT))))
  {
    blendEnable = true;
    blendSrc = RG_BLEND_FACTOR_SRC_ALPHA;
    blendDst = RG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  }
  else if (stt == STT_ADD)
  {
    blendEnable = true;
    blendSrc = RG_BLEND_FACTOR_SRC_ALPHA;
    blendDst = RG_BLEND_FACTOR_ONE;
  }
  else if (stt == STT_MULTIPLY)
  {
    blendEnable = true;
    blendSrc = RG_BLEND_FACTOR_ZERO;
    blendDst = RG_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
  }
  else
  {
    blendEnable = false;
  }
}


// RT: SetRenderingParameters(..) in RenderModel_View.cpp
static RgGeometryPassThroughType RT_GetPassThroughType(SurfaceTranslucencyType stt, bool forceAlphaTest)
{
  if (stt == STT_TRANSPARENT || forceAlphaTest)
  {
    return RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
  }
  else
  {
    return RG_GEOMETRY_PASS_THROUGH_TYPE_OPAQUE;
  }
}


// RT: same as PrepareView(..) from RenderModel.cpp
//     but view transfomation is not applied
static void RT_PrepareRotation(const CRenderModel &rm, const FLOAT3D &viewerPos, const FLOATmatrix3D &viewerRotation, FLOATmatrix3D &m)
{
  ULONG flags = rm.rm_pmdModelData->md_Flags;

  // RT: do nothing if normal
  if ((flags & MF_FACE_FORWARD) == 0 &&
      (flags & MF_HALF_FACE_FORWARD) == 0)
  {
    m = rm.rm_mObjectRotation;
  }

  // if half face forward
  if (flags & MF_HALF_FACE_FORWARD)
  {
    // get the y-axis vector of object rotation
    FLOAT3D vY(rm.rm_mObjectRotation(1, 2), rm.rm_mObjectRotation(2, 2), rm.rm_mObjectRotation(3, 2));

    // RT: 
    FLOAT3D vViewerZ = (rm.rm_vObjectPosition - viewerPos).SafeNormalize();

    // calculate x and z axis vectors to make object head towards viewer
    FLOAT3D vX = (-vViewerZ) * vY;
    vX.Normalize();
    FLOAT3D vZ = vY * vX;

    // compose the rotation matrix back from those angles
    m(1, 1) = vX(1);  m(1, 2) = vY(1);  m(1, 3) = vZ(1);
    m(2, 1) = vX(2);  m(2, 2) = vY(2);  m(2, 3) = vZ(2);
    m(3, 1) = vX(3);  m(3, 2) = vY(3);  m(3, 3) = vZ(3);
  }
  // if full face forward
  else if (flags & MF_FACE_FORWARD)
  {
    // use just object banking for rotation
    FLOAT fSinP = -rm.rm_mObjectRotation(2, 3);
    FLOAT fCosP = Sqrt(1 - fSinP * fSinP);
    FLOAT fSinB, fCosB;

    if (fCosP > 0.001f)
    {
      const FLOAT f1oCosP = 1.0f / fCosP;
      fSinB = rm.rm_mObjectRotation(2, 1) * f1oCosP;
      fCosB = rm.rm_mObjectRotation(2, 2) * f1oCosP;
    }
    else
    {
      fSinB = 0.0f;
      fCosB = 1.0f;
    }

    m(1, 1) = +fCosB;  m(1, 2) = -fSinB;  m(1, 3) = 0;
    m(2, 1) = +fSinB;  m(2, 2) = +fCosB;  m(2, 3) = 0;
    m(3, 1) = 0;  m(3, 2) = 0;  m(3, 3) = 1;

    m = viewerRotation * m;
  }
}


static void FlushModelInfo(ULONG entityID,
                           CRenderModel &rm,
                           CModelObject &mo,
                           const INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH],
                           INDEX *pIndices, INDEX indexCount,
                           const RT_VertexData &vd,
                           SurfaceTranslucencyType stt,
                           GFXColor modelColor,
                           bool forceTranslucency,
                           bool isBackground,
                           bool isFullBright,
                           SSRT::Scene *pScene)
{
  // RT: flush all
  ASSERT(indexCount > 0);

  bool isFirstPersonWeapon = entityID >= SSRT_FIRSTPERSON_ENTITY_START_ID;

  // RT: update rotation if MF_FACE_FORWARD or MF_HALF_FACE_FORWARD
  FLOATmatrix3D rotation;
  RT_PrepareRotation(rm, pScene->GetCameraPosition(), pScene->GetCameraRotation(), rotation);


  static std::vector<RgVertex> tempVerts;
  {
    tempVerts.clear();
    tempVerts.reserve(vd.vertexCount);

    for (uint32_t i = 0; i < vd.vertexCount; i++)
    {
      RgVertex v = {};
      memcpy(v.position, &vd.pPositons[i], sizeof(float) * 3);
      memcpy(v.texCoord, &vd.pTexCoords[i], sizeof(float) * 2);
      v.packedColor = UINT32_MAX;

      if (vd.pNormals != nullptr)
      {
        memcpy(v.normal, &vd.pNormals[i], sizeof(float) * 3);
      }

      tempVerts.emplace_back(v);
    }
  }


  SSRT::CModelGeometry modelInfo = {};
  modelInfo.entityID = entityID;
  modelInfo.modelPartIndex = RT_ModelPartIndex;

  modelInfo.absPosition = rm.rm_vObjectPosition;
  modelInfo.absRotation = rotation;

  modelInfo.vertexCount = vd.vertexCount;
  modelInfo.vertices = tempVerts.data();

  modelInfo.indexCount = indexCount;
  modelInfo.indices = pIndices;


  // assuming that sky type is always RG_SKY_TYPE_RASTERIZED_GEOMETRY
  const bool isRasterizedSky = isBackground;


  CTextureObject *to = &mo.mo_toTexture;
  CTextureData *td = to != nullptr ? (CTextureData *)to->GetData() : nullptr;
  CTextureObject *reflectionTo = &mo.mo_toReflection;

  bool forceAlphaTestOriginally = RT_ShouldBeAlphaTested(to, stt, forceTranslucency, pScene);
  modelInfo.passThroughType = RT_GetPassThroughType(stt, forceAlphaTestOriginally);
  modelInfo.isRasterized = RT_ShouldBeRasterized(stt, forceAlphaTestOriginally) || isRasterizedSky;

  // if alpha test forced by the texture
  if (modelInfo.passThroughType != RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED || modelInfo.isRasterized)
  {
    if (pScene->GetCustomInfo()->IsAlphaTestForced(to))
    {
      modelInfo.passThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_ALPHA_TESTED;
      modelInfo.isRasterized = false;
    }
  }

  if (((stt == STT_TRANSLUCENT || stt == STT_ADD || stt == STT_TRANSPARENT || stt == STT_MULTIPLY) && pScene->GetCustomInfo()->IsGlass(to)) || RT_IsInvisibleEnemy)
  {
    modelInfo.passThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_GLASS_REFLECT_REFRACT;
    modelInfo.isRasterized = false;
  }

  if (pScene->GetCustomInfo()->IsMirror(reflectionTo))
  {
    modelInfo.passThroughType = RG_GEOMETRY_PASS_THROUGH_TYPE_MIRROR;
    modelInfo.isRasterized = false;

    td = nullptr;
  }

  modelInfo.visibilityType = pScene->GetCustomInfo()->GetModelMaskBit(modelInfo.absPosition);

  if (isFirstPersonWeapon)
  {
    modelInfo.visibilityType = RG_GEOMETRY_VISIBILITY_TYPE_FIRST_PERSON;
  }
  else if (entityID == pScene->GetViewerEntityID())
  {
    modelInfo.visibilityType = RG_GEOMETRY_VISIBILITY_TYPE_FIRST_PERSON_VIEWER;
  }
  else if (isBackground)
  {
    modelInfo.visibilityType = RG_GEOMETRY_VISIBILITY_TYPE_WORLD_0;
    modelInfo.isSky = true;

    // convert rotation from background viewer space to world space,
    // as RTGL1 accepts viewer position
    FLOATmatrix3D bv;
    MakeInverseRotationMatrix(bv, pScene->GetBackgroundViewerOrientationAngle());

    rotation = bv * rotation;
  }

 
  modelInfo.color = Vector<FLOAT, 4>(modelColor.r, modelColor.g, modelColor.b, modelColor.a);
  modelInfo.color /= 255.0f;
 
  
  modelInfo.isReflective = rm.rm_pmmiMip->mmpi_ulLayerFlags & SRF_REFLECTIONS || pScene->GetCustomInfo()->IsFullMetallicForced(to);
  modelInfo.isSpecular = rm.rm_pmmiMip->mmpi_ulLayerFlags & SRF_SPECULAR;



  if (td != nullptr)
  {
    // must be repeat for all models, because of previous call 
    // of gfxSetTextureWrapping(GFX_REPEAT, GFX_REPEAT)
    td->td_tpLocal.tp_eWrapU = GFX_REPEAT;
    td->td_tpLocal.tp_eWrapV = GFX_REPEAT;

    modelInfo.textures[0] = td;
    modelInfo.textureFrames[0] = to->GetFrame();
  }

  for (INDEX i = 0; i < SSRT_MAX_ATTACHMENT_DEPTH; i++)
  {
    modelInfo.attchPath[i] = attchPath[i];
  }

  RT_SetBlend(modelInfo.blendEnable, modelInfo.blendSrc, modelInfo.blendDst, stt, forceTranslucency);


  modelInfo.generateNormals = vd.pNormals == nullptr;
  modelInfo.invertedNormals = rm.rm_ulFlags & RMF_INVERTED;

  if ((rm.rm_ulFlags & RMF_WEAPON) && !_srtGlobals.srt_bWeaponUseOriginalNormals)
  {
    // kludge: reverse IsCalcNormalsForced for RMF_WEAPON
    if (!pScene->GetCustomInfo()->IsCalcNormalsForced(&mo.mo_toTexture))
    {
      modelInfo.generateNormals = true;
    }
  }
  else
  {
    if (vd.pNormals != nullptr && pScene->GetCustomInfo()->IsCalcNormalsForced(&mo.mo_toTexture))
    {
      modelInfo.generateNormals = true;
    }
  }


  modelInfo.isEmissive = isFullBright; // rm.rm_rtRenderType & RT_SHADING_NONE;

  if (!modelInfo.isRasterized && pScene->GetCustomInfo()->IsEmissionForced(to))
  {
    modelInfo.textures[0] = nullptr;
    modelInfo.isEmissive = true;

    // for ring models IsEmissionForced is true
    float fPlateActive;
    if (pScene->GetCustomInfo()->GetActivatePlateState(modelInfo.absPosition, &fPlateActive))
    {
      // set emission colors
      float hue = Lerp(0.02f, 0.33f, fPlateActive);

      UBYTE r, g, b;
      ColorToRGB(
        HSVToColor((UBYTE)(Clamp<int>(hue * 255, 0, 255)), 255, 255),
        r, g, b);

      modelInfo.color = Vector<FLOAT, 4>(r, g, b , 255);
      modelInfo.color /= 255.0f;
    }
  }


  // ignore rasterized parts of first person viewer
  bool isRasterizedFirstPerson = modelInfo.visibilityType == RG_GEOMETRY_VISIBILITY_TYPE_FIRST_PERSON_VIEWER && modelInfo.isRasterized;

  if (!isRasterizedFirstPerson)
  {
    pScene->AddModel(modelInfo);
  }

  RT_ModelPartIndex++;
}


static float RT_GetMipFactor(const FLOAT3D &position, const CModelObject &mo, SSRT::Scene *scene)
{
  FLOAT distance = (scene->GetCameraPosition() - position).Length();

  if (mo.mo_Stretch != FLOAT3D(1, 1, 1))
  {
    distance /= Max(mo.mo_Stretch(1), Max(mo.mo_Stretch(2), mo.mo_Stretch(3)));
  }

  return Log2(distance) * (_srtGlobals.srt_fModelLODMul) + _srtGlobals.srt_fModelLODAdd;
}


// Create render model structure for rendering an attached model
//BOOL CModelObject::CreateAttachment( CRenderModel &rmMain, CAttachmentModelObject &amo)
static BOOL RT_CreateAttachment(const CModelObject &moParent, 
                                const CRenderModel &rmMain, 
                                const CAttachmentModelObject &amo)
{
  CRenderModel &rmAttached = *amo.amo_prm;
  rmAttached.rm_ulFlags = rmMain.rm_ulFlags & (RMF_FOG | RMF_HAZE | RMF_WEAPON) | RMF_ATTACHMENT;

  // get the position
  const CAttachedModelPosition &amp = rmMain.rm_pmdModelData->md_aampAttachedPosition[ amo.amo_iAttachedPosition ];

  // copy common values
  rmAttached.rm_vLightDirection = rmMain.rm_vLightDirection;
  rmAttached.rm_fDistanceFactor = rmMain.rm_fDistanceFactor;
  rmAttached.rm_colLight = rmMain.rm_colLight;
  rmAttached.rm_colAmbient = rmMain.rm_colAmbient;
  rmAttached.rm_colBlend = rmMain.rm_colBlend;

  // unpack the reference vertices
  FLOAT3D vCenter, vFront, vUp;
  const INDEX iCenter = amp.amp_iCenterVertex;
  const INDEX iFront = amp.amp_iFrontVertex;
  const INDEX iUp = amp.amp_iUpVertex;

  auto unpackVertex = [ &moParent ] (const CRenderModel &rm, INDEX iVertex, FLOAT3D &vVertex)
  {
    // GetData()
    const CModelData *data = (const CModelData*)moParent.ao_AnimData;

    if (data->md_Flags & MF_COMPRESSED_16BIT)
    {
      // get 16 bit packed vertices
      const SWPOINT3D &vsw0 = rm.rm_pFrame16_0[iVertex].mfv_SWPoint;
      const SWPOINT3D &vsw1 = rm.rm_pFrame16_1[iVertex].mfv_SWPoint;
      // convert them to float and lerp between them
      vVertex(1) = (Lerp((FLOAT) vsw0(1), (FLOAT) vsw1(1), rm.rm_fRatio) - rm.rm_vOffset(1)) * rm.rm_vStretch(1);
      vVertex(2) = (Lerp((FLOAT) vsw0(2), (FLOAT) vsw1(2), rm.rm_fRatio) - rm.rm_vOffset(2)) * rm.rm_vStretch(2);
      vVertex(3) = (Lerp((FLOAT) vsw0(3), (FLOAT) vsw1(3), rm.rm_fRatio) - rm.rm_vOffset(3)) * rm.rm_vStretch(3);
    }
    else
    {
      // get 8 bit packed vertices
      const SBPOINT3D &vsb0 = rm.rm_pFrame8_0[iVertex].mfv_SBPoint;
      const SBPOINT3D &vsb1 = rm.rm_pFrame8_1[iVertex].mfv_SBPoint;
      // convert them to float and lerp between them
      vVertex(1) = (Lerp((FLOAT) vsb0(1), (FLOAT) vsb1(1), rm.rm_fRatio) - rm.rm_vOffset(1)) * rm.rm_vStretch(1);
      vVertex(2) = (Lerp((FLOAT) vsb0(2), (FLOAT) vsb1(2), rm.rm_fRatio) - rm.rm_vOffset(2)) * rm.rm_vStretch(2);
      vVertex(3) = (Lerp((FLOAT) vsb0(3), (FLOAT) vsb1(3), rm.rm_fRatio) - rm.rm_vOffset(3)) * rm.rm_vStretch(3);
    }
  };

  unpackVertex(rmMain, iCenter, vCenter);
  unpackVertex(rmMain, iFront, vFront);
  unpackVertex(rmMain, iUp, vUp);

  // create front and up direction vectors
  FLOAT3D vY = vUp - vCenter;
  FLOAT3D vZ = vCenter - vFront;
  // project center and directions from object to absolute space
  const FLOATmatrix3D &mO2A = rmMain.rm_mObjectRotation;
  const FLOAT3D &vO2A = rmMain.rm_vObjectPosition;
  vCenter = vCenter * mO2A + vO2A;
  vY = vY * mO2A;
  vZ = vZ * mO2A;

  // make a rotation matrix from the direction vectors
  FLOAT3D vX = vY * vZ;
  vY = vZ * vX;
  vX.Normalize();
  vY.Normalize();
  vZ.Normalize();
  FLOATmatrix3D mOrientation;
  mOrientation(1, 1) = vX(1);  mOrientation(1, 2) = vY(1);  mOrientation(1, 3) = vZ(1);
  mOrientation(2, 1) = vX(2);  mOrientation(2, 2) = vY(2);  mOrientation(2, 3) = vZ(2);
  mOrientation(3, 1) = vX(3);  mOrientation(3, 2) = vY(3);  mOrientation(3, 3) = vZ(3);

  // adjust for relative placement of the attachment
  FLOAT3D vOffset;
  FLOATmatrix3D mRelative;
  MakeRotationMatrix(mRelative, amo.amo_plRelative.pl_OrientationAngle);
  vOffset(1) = amo.amo_plRelative.pl_PositionVector(1) * moParent.mo_Stretch(1);
  vOffset(2) = amo.amo_plRelative.pl_PositionVector(2) * moParent.mo_Stretch(2);
  vOffset(3) = amo.amo_plRelative.pl_PositionVector(3) * moParent.mo_Stretch(3);
  FLOAT3D vO = vCenter + vOffset * mOrientation;
  mOrientation *= mRelative; // convert absolute to relative orientation
  rmAttached.SetObjectPlacement(vO, mOrientation);

  return TRUE;
  // done here if clipping optimizations are not allowed
  /*extern INDEX gap_iOptimizeClipping;
  if (gap_iOptimizeClipping < 1)
  {
    gap_iOptimizeClipping = 0;
    return TRUE;
  }

  CalculateBoundingBox(&amo.amo_moModelObject, rmAttached);
  return TRUE;*/
}


//void CModelObject::SetupModelRendering( CRenderModel &rm)
static void RT_SetupModelRendering(CModelObject &mo, 
                                   CRenderModel &rm, 
                                   SSRT::Scene *scene)
{
  // get model's data and lerp info, GetData()
  rm.rm_pmdModelData = mo.GetData();
  mo.GetFrame(rm.rm_iFrame0, rm.rm_iFrame1, rm.rm_fRatio);

  const INDEX ctVertices = rm.rm_pmdModelData->md_VerticesCt;

  if (rm.rm_pmdModelData->md_Flags & MF_COMPRESSED_16BIT)
  {
    // set pFrame to point to last and next frames' vertices
    rm.rm_pFrame16_0 = &rm.rm_pmdModelData->md_FrameVertices16[ rm.rm_iFrame0 * ctVertices ];
    rm.rm_pFrame16_1 = &rm.rm_pmdModelData->md_FrameVertices16[ rm.rm_iFrame1 * ctVertices ];
  }
  else
  {
    // set pFrame to point to last and next frames' vertices
    rm.rm_pFrame8_0 = &rm.rm_pmdModelData->md_FrameVertices8[ rm.rm_iFrame0 * ctVertices ];
    rm.rm_pFrame8_1 = &rm.rm_pmdModelData->md_FrameVertices8[ rm.rm_iFrame1 * ctVertices ];
  }

  // obtain current rendering preferences
  rm.rm_rtRenderType = _mrpModelRenderPrefs.GetRenderType();
  // remember blending color
  rm.rm_colBlend = MulColors(rm.rm_colBlend, mo.mo_colBlendColor);

  // get decompression/stretch factors
  FLOAT3D &vDataStretch = rm.rm_pmdModelData->md_Stretch;
  rm.rm_vStretch(1) = vDataStretch(1) * mo.mo_Stretch(1);
  rm.rm_vStretch(2) = vDataStretch(2) * mo.mo_Stretch(2);
  rm.rm_vStretch(3) = vDataStretch(3) * mo.mo_Stretch(3);
  rm.rm_vOffset = rm.rm_pmdModelData->md_vCompressedCenter;
  // check if object is inverted (in mirror)
  BOOL bXInverted = rm.rm_vStretch(1) < 0;
  BOOL bYInverted = rm.rm_vStretch(2) < 0;
  BOOL bZInverted = rm.rm_vStretch(3) < 0;
  rm.rm_ulFlags &= ~RMF_INVERTED;
  // RT: i.e. the sum (bXInverted+bYInverted+bZInverted) is odd
  if (bXInverted != bYInverted != bZInverted != 0) 
  {
    rm.rm_ulFlags |= RMF_INVERTED;
  }

  rm.rm_fMipFactor = RT_GetMipFactor(rm.rm_vObjectPosition, mo, scene);

  // get current mip model using mip factor
  rm.rm_iMipLevel = mo.GetMipModel(rm.rm_fMipFactor);
  //mo.mo_iLastRenderMipLevel = rm.rm_iMipLevel;
  // get current vertices mask
  rm.rm_pmmiMip = &rm.rm_pmdModelData->md_MipInfos[ rm.rm_iMipLevel ];

  // don't allow any shading, if shading is turned off
  if (rm.rm_rtRenderType & RT_SHADING_NONE)
  {
    rm.rm_colAmbient = C_WHITE | CT_OPAQUE;
    rm.rm_colLight = C_BLACK;
  }

  // precalculate rendering data if needed
  extern void PrepareModelForRendering(CModelData & md);
  PrepareModelForRendering(*rm.rm_pmdModelData);

  // done with setup if viewing from this model
  if (rm.rm_ulFlags & RMF_SPECTATOR)
  {
    return;
  }

  // for each attachment on this model object
  FOREACHINLIST(CAttachmentModelObject, amo_lnInMain, mo.mo_lhAttachments, itamo)
  {
    CAttachmentModelObject *pamo = itamo;
    // create new render model structure
    
    extern CStaticStackArray<CRenderModel> _armRenderModels;

    pamo->amo_prm = &_armRenderModels.Push();
    RT_CreateAttachment(mo, rm, *pamo);
    
    // prepare if visible
    RT_SetupModelRendering(pamo->amo_moModelObject, *pamo->amo_prm, scene);
  }
}


// RT: add vertices to corresponding CModelObject
//void RenderOneSide( CRenderModel &rm, BOOL bBackSide, ULONG ulLayerFlags)
static void RT_RenderOneSide(ULONG entityID,
                             CRenderModel &rm,
                             CModelObject &mo,
                             const RT_VertexData &vd,
                             BOOL bBackSide, 
                             bool isBackground,
                             const INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH],
                             bool forceTranslucency,
                             SSRT::Scene *scene)
{
  // set face culling
  if (bBackSide)
  {
    //if (!(_ulMipLayerFlags & SRF_DOUBLESIDED))
    {
      return;
    }
    //else g fxCullFace(GFX_FRONT);
  }
  //else g fxCullFace(GFX_BACK);

  // start with invalid rendering parameters
  SurfaceTranslucencyType sttLast = STT_INVALID;

  // for each surface in current mip model
  INDEX iStartElem = 0;
  INDEX ctElements = 0;
  ModelMipInfo &mmi = *rm.rm_pmmiMip;

  bool isFullbright = false;

  auto flushModel = [entityID, &rm, &mo, attchPath, &mmi, &vd, scene, &sttLast, forceTranslucency, isBackground, &isFullbright] (INDEX firstIndex, INDEX indexCount)
  {
    ASSERT(firstIndex >= 0);
    if (indexCount <= 0)
    {
      return;
    }

    // RT: use surface color, it doesn't differ over mapping surface because of the lack of vertex lighting
    GFXColor modelColor = vd.pColors[mmi.mmpi_aiElements[firstIndex]];

    FlushModelInfo(entityID, rm, mo, attchPath,
                   &mmi.mmpi_aiElements[firstIndex], indexCount,
                   vd, sttLast, modelColor, forceTranslucency, isBackground, isFullbright, scene);
  };

  {
    FOREACHINSTATICARRAY(mmi.mmpi_MappingSurfaces, MappingSurface, itms)
    {
      const MappingSurface &ms = *itms;
      const ULONG ulFlags = ms.ms_ulRenderingFlags;

      // end rendering if surface is invisible or empty - these are the last surfaces in surface list
      if ((ulFlags & SRF_INVISIBLE) || ms.ms_ctSrfVx == 0)
      {
        break;
      }

      isFullbright = ms.ms_sstShadingType == SST_FULLBRIGHT;

      // skip surface if ... 
      if (!(ulFlags & SRF_DIFFUSE)  // not in this layer,
          || (bBackSide && !(ulFlags & SRF_DOUBLESIDED)) // rendering back side and surface is not double sided,
          /*|| !(_ulColorMask & ms.ms_ulOnColor)  // not on or off.
          || (_ulColorMask & ms.ms_ulOffColor)*/)
      {
        if (ctElements > 0)
        {
          flushModel(iStartElem, ctElements);
        }

        iStartElem += ctElements + ms.ms_ctSrfEl;
        ctElements = 0;
        continue;
      }

      // get rendering parameters
      SurfaceTranslucencyType stt = ms.ms_sttTranslucencyType;

      // if surface uses rendering parameters different than last one
      if (sttLast != stt)
      {
        // set up new API states
        if (ctElements > 0)
        {
          flushModel(iStartElem, ctElements);
        }

        // SetRenderingParameters(stt, slBump);
        sttLast = stt;
        iStartElem += ctElements;
        ctElements = 0;
      }

      // batch the surface polygons for rendering
      ctElements += ms.ms_ctSrfEl;
    }
  }

  flushModel(iStartElem, ctElements);
}



static void RT_RenderModel_View(ULONG entityID, CModelObject &mo, CRenderModel &rm, bool isBackground,
                         const INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH], INDEX attchCount, SSRT::Scene *scene)
//void CModelObject::RenderModel_View(CRenderModel &rm)
{
  BOOL bXInverted = rm.rm_vStretch(1) < 0;
  BOOL bYInverted = rm.rm_vStretch(2) < 0;
  BOOL bZInverted = rm.rm_vStretch(3) < 0;

  // declare pointers for general usage
  INDEX iSrfVx0, ctSrfVx;
  ModelMipInfo &mmi = *rm.rm_pmmiMip;

  // allocate vertex arrays
  extern INDEX _ctAllMipVx;
  extern INDEX _ctAllSrfVx;
  _ctAllMipVx = mmi.mmpi_ctMipVx;
  _ctAllSrfVx = mmi.mmpi_ctSrfVx;
  ASSERT(_ctAllMipVx > 0 && _ctAllSrfVx > 0);

  extern CStaticStackArray<GFXVertex3>  _avtxMipBase;
  extern CStaticStackArray<GFXTexCoord> _atexMipBase;
  extern CStaticStackArray<GFXNormal3>  _anorMipBase;
  extern CStaticStackArray<GFXColor>    _acolMipBase;

  extern CStaticStackArray<GFXVertex4>  _avtxSrfBase;
  extern CStaticStackArray<GFXNormal4>  _anorSrfBase;
  extern CStaticStackArray<GFXTexCoord> _atexSrfBase;
  extern CStaticStackArray<GFXColor>    _acolSrfBase;

  ASSERT(_avtxMipBase.Count() == 0);  _avtxMipBase.Push(_ctAllMipVx);
  ASSERT(_atexMipBase.Count() == 0);  _atexMipBase.Push(_ctAllMipVx);
  ASSERT(_acolMipBase.Count() == 0);  _acolMipBase.Push(_ctAllMipVx);
  ASSERT(_anorMipBase.Count() == 0);  _anorMipBase.Push(_ctAllMipVx);

  ASSERT(_avtxSrfBase.Count() == 0);  _avtxSrfBase.Push(_ctAllSrfVx);
  ASSERT(_atexSrfBase.Count() == 0);  _atexSrfBase.Push(_ctAllSrfVx);
  ASSERT(_acolSrfBase.Count() == 0);  _acolSrfBase.Push(_ctAllSrfVx);
  ASSERT(_anorSrfBase.Count() == 0);  _anorSrfBase.Push(_ctAllSrfVx);


  bool forceTranslucency = ((rm.rm_colBlend & CT_AMASK) >> CT_ASHIFT) != CT_OPAQUE;
  ULONG ulColorMask = mo.mo_ColorMask;

  // adjust all surfaces' params for eventual forced-translucency case
  ULONG ulMipLayerFlags = mmi.mmpi_ulLayerFlags;
  //if (_bForceTranslucency)
  //{
  //  _ulMipLayerFlags &= ~MMI_OPAQUE;
  //  _ulMipLayerFlags |= MMI_TRANSLUCENT;
  //}


  // unpack one model frame vertices and eventually normals (lerped or not lerped, as required)
  // RT: these global variables are required for UnpackFrame()
  extern GFXVertex3 *pvtxMipBase;
  extern GFXNormal3 *pnorMipBase;
  // this will array be ignored as it's used in per-vertex lighting,
  // so _slAR, _slAG, _slAB, _slLR, _slLG, _slLB are not set
  extern GFXColor *pcolMipBase;

  pvtxMipBase = &_avtxMipBase[ 0 ];
  pcolMipBase = &_acolMipBase[ 0 ];
  pnorMipBase = &_anorMipBase[ 0 ];

  // RT: always get normals
  const BOOL bNeedNormals = true;

  extern void UnpackFrame(CRenderModel & rm, BOOL bKeepNormals);
  UnpackFrame(rm, bNeedNormals);

  // PREPARE SURFACE VERTICES ------------------------------------------------------------------------


  // for each surface in current mip model
  {
    FOREACHINSTATICARRAY(mmi.mmpi_MappingSurfaces, MappingSurface, itms)
    {
      const MappingSurface &ms = *itms;
      iSrfVx0 = ms.ms_iSrfVx0;
      ctSrfVx = ms.ms_ctSrfVx;

      // skip to next in case of invisible or empty surface
      if ((ms.ms_ulRenderingFlags & SRF_INVISIBLE) || ctSrfVx == 0)
      {
        break;
      }
      
      const UWORD *puwSrfToMip = &mmi.mmpi_auwSrfToMip[ iSrfVx0 ];

      // setup vertex array
      GFXVertex *pvtxSrfBase = &_avtxSrfBase[iSrfVx0];
      for (INDEX iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++)
      {
        const INDEX iMipVx = puwSrfToMip[ iSrfVx ];
        pvtxSrfBase[ iSrfVx ].x = pvtxMipBase[ iMipVx ].x;
        pvtxSrfBase[ iSrfVx ].y = pvtxMipBase[ iMipVx ].y;
        pvtxSrfBase[ iSrfVx ].z = pvtxMipBase[ iMipVx ].z;
      }

      GFXNormal *pnorSrfBase = &_anorSrfBase[ iSrfVx0 ];
      for (INDEX iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++)
      {
        const INDEX iMipVx = puwSrfToMip[ iSrfVx ];

        pnorSrfBase[iSrfVx].nx = pnorMipBase[iMipVx].nx;
        pnorSrfBase[iSrfVx].ny = pnorMipBase[iMipVx].ny;
        pnorSrfBase[iSrfVx].nz = pnorMipBase[iMipVx].nz;

        if (rm.rm_ulFlags & RMF_INVERTED)
        {
          pnorSrfBase[iSrfVx].nx *= bXInverted ? -1 : 1;
          pnorSrfBase[iSrfVx].ny *= bYInverted ? -1 : 1;
          pnorSrfBase[iSrfVx].nz *= bZInverted ? -1 : 1;
        }
      }
    }
  }

  // RT: this data will be set in RT_VertexData
  // prepare (and lock) vertex array
  //g fxEnableDepthTest();
  //gfxSetVertexArray(&_avtxSrfBase[ 0 ], _ctAllSrfVx);
  //gfxSetNormalArray(&_anorSrfBase[ 0 ]);

  // texture mapping correction factors (mex -> norm float)
  FLOAT fTexCorrU, fTexCorrV;
  //gfxSetTextureWrapping(GFX_REPEAT, GFX_REPEAT);

  // color and fill mode setup
  bool bFlatFill = (rm.rm_rtRenderType & RT_WHITE_TEXTURE) || mo.mo_toTexture.ao_AnimData == NULL;
  const BOOL bTexMode = rm.rm_rtRenderType & (RT_TEXTURE | RT_WHITE_TEXTURE);
  const BOOL bAllLayers = bTexMode && !bFlatFill;  // disallow rendering of every layer except diffuse


  // RENDER DIFFUSE LAYER -------------------------------------------------------------------


  // get diffuse texture corrections
  const CTextureData *ptdDiff = (const CTextureData *) mo.mo_toTexture.ao_AnimData;
  if (ptdDiff != NULL)
  {
    fTexCorrU = 1.0f / ptdDiff->GetWidth();
    fTexCorrV = 1.0f / ptdDiff->GetHeight();
  }
  else
  {
    fTexCorrU = 1.0f;
    fTexCorrV = 1.0f;
  }

  // get model diffuse color
  GFXColor colMdlDiff;
  const COLOR colD = rm.rm_pmdModelData->md_colDiffuse; // AdjustColor(rm.rm_pmdModelData->md_colDiffuse, _slTexHueShift, _slTexSaturation);
  const COLOR colB = rm.rm_colBlend; // AdjustColor(rm.rm_colBlend, _slTexHueShift, _slTexSaturation);
  colMdlDiff.MultiplyRGBA(colD, colB);

  // for each surface in current mip model
  {
    FOREACHINSTATICARRAY(mmi.mmpi_MappingSurfaces, MappingSurface, itms)
    {
      const MappingSurface &ms = *itms;
      iSrfVx0 = ms.ms_iSrfVx0;
      ctSrfVx = ms.ms_ctSrfVx;

      if ((ms.ms_ulRenderingFlags & SRF_INVISIBLE) || ctSrfVx == 0)
      {
        break;  // done if found invisible or empty surface
      }

      // cache surface pointers
      const UWORD *puwSrfToMip = &mmi.mmpi_auwSrfToMip[ iSrfVx0 ];
      const FLOAT2D *pvTexCoord = &mmi.mmpi_avmexTexCoord[ iSrfVx0 ];

      GFXTexCoord *ptexSrfBase = &_atexSrfBase[ iSrfVx0 ];
      GFXColor *pcolSrfBase = &_acolSrfBase[ iSrfVx0 ];

      // get surface diffuse color and combine with model color
      GFXColor colSrfDiff;
      colSrfDiff.MultiplyRGBA(ms.ms_colDiffuse, colMdlDiff);

      // setup texcoord array
      for (INDEX iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++)
      {
        ptexSrfBase[ iSrfVx ].s = pvTexCoord[ iSrfVx ](1) * fTexCorrU;
        ptexSrfBase[ iSrfVx ].t = pvTexCoord[ iSrfVx ](2) * fTexCorrV;
      }

      // setup color array
      // RT: pcolMipBase is an array that holds light color,
      // we don't need it, so just set overall diffuse color for whole surface 
      for (INDEX iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++)
      {
        pcolSrfBase[ iSrfVx ] = colSrfDiff;
      }
    }
  }

  // must render diffuse if there is no texture (white mode)
  if ((ulMipLayerFlags & SRF_DIFFUSE) || ptdDiff == NULL)
  {
    //gfxSetTexCoordArray(&_atexSrfBase[ 0 ], FALSE);

    RT_VertexData vd = {};
    vd.vertexCount = _ctAllSrfVx;
    vd.pPositons = &_avtxSrfBase[0];
    vd.pNormals = &_anorSrfBase[0];
    vd.pTexCoords = &_atexSrfBase[0];
    vd.pColors = &_acolSrfBase[0];

    // RT: everything is set, copy model data to SSRT
    //RT_RenderOneSide(rm, TRUE);
    RT_RenderOneSide(entityID, rm, mo, vd, FALSE, isBackground, attchPath, forceTranslucency, scene);
  }

  // adjust z-buffer and blending functions
  if (ulMipLayerFlags & MMI_OPAQUE)
  {
    //g fxDepthFunc(GFX_EQUAL);
  }
  else
  {
    //g fxDepthFunc(GFX_LESS_EQUAL);
  }
  //g fxDisableDepthWrite();
  //g fxDisableAlphaTest(); // disable alpha testing if enabled after some surface
  //g fxEnableBlend();

  // almost done
  //g fxDepthFunc(GFX_LESS_EQUAL);
  //g fxUnlockArrays();

  // reset model vertex buffers and rendering face
  extern void ResetVertexArrays(void);
  ResetVertexArrays();
}


//void CModelObject::RenderModel(CRenderModel &rm)
static void RT_RenderModel(ULONG entityID, CModelObject &mo, CRenderModel &rm, bool isBackground, INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH], INDEX attchCount, SSRT::Scene *scene)
{
  // skip invisible models
  if (mo.mo_Stretch == FLOAT3D(0, 0, 0))
  {
    return;
  }

  if (scene->GetCustomInfo()->IsModelWithTextureIgnored(&rm, &mo.mo_toTexture))
  {
    return;
  }

  // RT: _iRenderingType is 1 for not shadows

  // if we should draw polygons and model 
  if (!(rm.rm_ulFlags & RMF_SPECTATOR) && (!(rm.rm_rtRenderType & RT_NO_POLYGON_FILL)
                                           || (rm.rm_rtRenderType & RT_WIRE_ON) || (rm.rm_rtRenderType & RT_HIDDEN_LINES)))
  {
    //// eventually calculate projection model bounding box in object space (needed for fog/haze trivial rejection)
    //if (rm.rm_ulFlags & (RMF_FOG | RMF_HAZE)) CalculateBoundingBox(this, rm);
    // render complete model
    //rm.SetModelView();
    RT_RenderModel_View(entityID, mo, rm, isBackground, attchPath, attchCount, scene);
  }

  // if too many attachments on attachments
  if (attchCount >= SSRT_MAX_ATTACHMENT_DEPTH)
  {
    return;
  }

  // render each attachment on this model object
  FOREACHINLIST(CAttachmentModelObject, amo_lnInMain, mo.mo_lhAttachments, itamo)
  {
    // calculate bounding box of an attachment    
    CAttachmentModelObject *pamo = itamo;
    if (pamo->amo_prm == NULL)
    {
      continue;
    }

    // set the last as current attachment position,
    // we're assuming that each attachment uses its unique amo_iAttachedPosition
    attchPath[attchCount] = pamo->amo_iAttachedPosition;
    
    RT_RenderModel(entityID, pamo->amo_moModelObject, *pamo->amo_prm, isBackground, attchPath, attchCount + 1, scene);
  }
}


//void CRenderer::RenderOneModel(CEntity &en, CModelObject &moModel, const CPlacement3D &plModel,
//                               const FLOAT fDistanceFactor, BOOL bRenderShadow, ULONG ulDMFlags)
static void RT_RenderOneModel(CEntity &en, 
                              CModelObject &moModel, 
                              const CPlacement3D &plModel,
                              const FLOAT fDistanceFactor, 
                              BOOL bRenderShadow, 
                              ULONG ulDMFlags,
                              SSRT::Scene *scene)
{
  // skip invisible models
  if (moModel.mo_Stretch == FLOAT3D(0, 0, 0))
  {
    return;
  }

  // RT: just any params; AdjustShadingParameters is used for very different logic, like model fading by time
  COLOR colLight   = C_GRAY;
  COLOR colAmbient = C_dGRAY;
  FLOAT3D vTotalLightDirection( 1.0f, -1.0f, 1.0f);
  bool hasShadows = en.AdjustShadingParameters( vTotalLightDirection, colLight, colAmbient);

  RT_IsInvisibleEnemy = scene->GetCustomInfo()->IsInvisibleEnemy(&en, colLight, colAmbient, hasShadows);

  // prepare render model structure
  CRenderModel rm;
  rm.rm_fDistanceFactor = fDistanceFactor;

  // RT: for calculating rotaion matrix and setting position vector
  rm.SetObjectPlacement(plModel);

  // TODO: RT: alpha
  // moModel->HasAlpha()

  //if (ulDMFlags & DMF_FOG)      rm.rm_ulFlags |= RMF_FOG;
  //if (ulDMFlags & DMF_HAZE)     rm.rm_ulFlags |= RMF_HAZE;

  // RT: it's guaranteed that this function won't be called for viewer entity
  /*if (viewerEntityID == en.en_ulID)
  {
    rm.rm_ulFlags |= RMF_SPECTATOR;
  }*/

  /*if (IsOfClass(&en, "Player Weapons"))
  {
    rm.rm_ulFlags |= RMF_WEAPON;
  }*/

  // prepare CRenderModel structure for rendering of one model
  RT_SetupModelRendering(moModel, rm, scene);

  // attachment path for SSRT::CModelGeometry,
  INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH];
  for (INDEX i = 0; i < SSRT_MAX_ATTACHMENT_DEPTH; i++)
  {
    // init all as -1
    attchPath[i] = -1;
  }

  // RT: process a model with all its attachmnets
  RT_RenderModel(en.en_ulID, moModel, rm, en.en_ulFlags & ENF_BACKGROUND, attchPath, 0, scene);

  extern CStaticStackArray<CRenderModel> _armRenderModels;
  _armRenderModels.PopAll();
}


// CRenderer::RenderModels(BOOL bBackground)
static void RT_Post_RenderModels(CEntity &en, 
                                 CModelObject &mo, 
                                 SSRT::Scene *scene)
{
  if (!gfx_bRenderModels)
  {
    return;
  }

  RT_RenderOneModel(en, mo, en.GetLerpedPlacement(), 0, TRUE, 0, scene);
}


void RT_AddModelEntity(CEntity *penModel, SSRT::Scene *pScene)
{        
  RT_ModelPartIndex = 0;
  RT_IsInvisibleEnemy = false;

  // if the entity is currently active or hidden, don't add it again
  if (penModel->en_ulFlags & ENF_HIDDEN)
  {
    return;
  }

  // skip the entity if predicted, and predicted entities should not be rendered
  if (penModel->IsPredicted() && !gfx_bRenderPredicted)
  {
    return;
  }


  // add light source there is one
  RT_ProcessModelLights(penModel, pScene);


  // if it's a viewer and there should be no shadows from it
  if (!_srtGlobals.srt_bEnableViewerShadows && pScene->GetViewerEntityID() == penModel->en_ulID)
  {
    return;
  }


  // RT: ignore editor model's geometry, but not its light source
  if (penModel->en_RenderType == CEntity::RT_EDITORMODEL)
  {
    return;
  }

  // get its model object
  CModelObject *pmoModelObject;
  if (penModel->en_RenderType != CEntity::RT_BRUSH &&
      penModel->en_RenderType != CEntity::RT_FIELDBRUSH)
  {
    pmoModelObject = penModel->GetModelForRendering();
  }
  else
  {
    // empty brushes are also rendered as models
    //pmoModelObject = _wrpWorldRenderPrefs.wrp_pmoEmptyBrush;
    
    return;
  }

  // mark the entity as active in rendering
  //penModel->en_ulFlags |= ENF_INRENDERING;

  // RT: set enormous negative value, so we won't have fading on ModelHolders
  float mipFactor = -10000000.0f;
  penModel->AdjustMipFactor(mipFactor);

  // add it to a container for delayed rendering
  //CDelayedModel dm = {}; // re_admDelayedModels.Push();
  //dm.dm_pmoModel = pmoModelObject;
  //dm.dm_ulFlags = NONE; // invisible until proved otherwise
  //dm.dm_fDistance = 0;// fDistance;
  //dm.dm_fMipFactor = 0;// fMipFactor;

  // if it has a light source with a lens flare and we're not rendering shadows
  //if (!re_bRenderingShadows && pls != NULL && pls->ls_plftLensFlare != NULL)
  //{
  //  AddLensFlare(penModel, pls, pprProjection, re_iIndex);
  //}

  // RT: flags for fog and haze?
  // adjust model flags
  //if (pmoModelObject->HasAlpha())
  //{
  //  dm.dm_ulFlags |= DMF_HASALPHA;
  //}

  // safety check
  if (pmoModelObject == NULL)
  {
    return;
  }

  // allow its rendering
  //dm.dm_ulFlags |= DMF_VISIBLE;

  // RT: call it here to bypass re_admDelayedModels list
  RT_Post_RenderModels(*penModel, *pmoModelObject, pScene);
}


void RT_AddFirstPersonModel(CModelObject *mo, CRenderModel *rm, ULONG entityId, SSRT::Scene *scene)
{
  RT_ModelPartIndex = 0;
  RT_IsInvisibleEnemy = false;

  auto &v = rm->rm_vObjectPosition;
  auto &m = rm->rm_mObjectRotation;

  float localToCamera[] =
  {
    m(1, 1), m(1, 2), m(1, 3), v(1),
    m(2, 1), m(2, 2), m(2, 3), v(2),
    m(3, 1), m(3, 2), m(3, 3), v(3),
    0,       0,       0,       1
  };

  const auto &cv = scene->GetCameraPosition();
  const auto &cm = scene->GetCameraRotation();

  float cameraToGlobal[] =
  {
    cm(1, 1), cm(1, 2), cm(1, 3), cv(1),
    cm(2, 1), cm(2, 2), cm(2, 3), cv(2),
    cm(3, 1), cm(3, 2), cm(3, 3), cv(3),
    0,        0,        0,        1
  };

  // local to global
  float g[4][4];

  extern void Svk_MatMultiply(float *result, const float *a, const float *b);
  Svk_MatMultiply((float *)g, cameraToGlobal, localToCamera);

  v =
  {
    g[0][3],
    g[1][3],
    g[2][3]
  };

  for (uint32_t i = 0; i < 3; i++)
  {
    for (uint32_t j = 0; j < 3; j++)
    {
      m.matrix[i][j] = g[i][j];
    }
  }

  RT_SetupModelRendering(*mo, *rm, scene);

  // attachment path for SSRT::CModelGeometry,
  INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH];
  for (INDEX i = 0; i < SSRT_MAX_ATTACHMENT_DEPTH; i++)
  {
    // init all as -1
    attchPath[i] = -1;
  }

  // RT: process a model with all its attachmnets
  RT_RenderModel(entityId, *mo, *rm, false, attchPath, 0, scene);

  extern CStaticStackArray<CRenderModel> _armRenderModels;
  _armRenderModels.PopAll();
}

