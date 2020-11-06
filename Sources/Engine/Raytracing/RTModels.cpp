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
#include <Engine/Models/RenderModel.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/Models/ModelData.h>
#include <Engine/Light/LightSource.h>

#include <Engine/Base/ListIterator.inl>

#include <Engine/Raytracing/SSRT.h>


extern INDEX gfx_bRenderModels;
extern INDEX gfx_bRenderPredicted;


struct RT_VertexData
{
  GFXVertex   *vertices;
  INDEX       vertexCount;
  GFXNormal   *normals;
  GFXTexCoord *texCoords;
};


static void RT_AddLight(const CLightSource *plsLight, SSRT::SSRTMain *ssrt)
{
  ASSERT(plsLight != NULL);

  // ignore lights that subtract light
  if (plsLight->ls_ulFlags & LSF_DARKLIGHT)
  {
    return;
  }

  // TODO: RT: test if ignoring these lights is ok
  // ignore not important lights
  if (!(plsLight->ls_ulFlags & LSF_CASTSHADOWS))
  {
    //return;
  }

  const CPlacement3D &placement = plsLight->ls_penEntity->GetPlacement();
  const FLOAT3D &position = placement.pl_PositionVector;

  COLOR color = plsLight->GetLightColor();

  // directional light
  if (plsLight->ls_ulFlags & LSF_DIRECTIONAL)
  {
    FLOAT3D direction;
    AnglesToDirectionVector(placement.pl_OrientationAngle, direction);

    SSRT::CDirectionalLight light = {};
    light.direction = direction;
    light.color = color;
    // for a reference: sun is 0.5 degrees
    light.angularSize = 5.0f;

    ssrt->AddLight(light);
  }
  else
  {
    // assume that hotspot is a radius of sphere light
    float radius = plsLight->ls_rHotSpot;
    float falloff = plsLight->ls_rFallOff;

    SSRT::CSphereLight light = {};
    light.entityID = plsLight->ls_penEntity->en_ulID;
    light.isEnabled = true;
    light.absPosition = position;
    light.color = color;
    light.intensity = 1.0f;
    light.sphereRadius = radius;

    ssrt->AddLight(light);
  }
}


// Create render model structure for rendering an attached model
//BOOL CModelObject::CreateAttachment( CRenderModel &rmMain, CAttachmentModelObject &amo)
static BOOL RT_CreateAttachment(const CModelObject &moParent, const CRenderModel &rmMain, const CAttachmentModelObject &amo)
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
static void RT_SetupModelRendering(const CModelObject &mo, CRenderModel &rm, SSRT::SSRTMain *ssrt)
{
  // get model's data and lerp info, GetData()
  rm.rm_pmdModelData = (CModelData *)mo.ao_AnimData;
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
  //if (bXInverted != bYInverted != bZInverted != _aprProjection->pr_bInverted) 
  //{
  //  rm.rm_ulFlags |= RMF_INVERTED;
  //}

  // prepare projections
  //PrepareView(rm);

  // TODO: RT: detect if models must be aligned with a viewer
  // rm.rm_pmdModelData->md_Flags&MF_HALF_FACE_FORWARD -- only X,Z face viewer
  // rm.rm_pmdModelData->md_Flags&MF_FACE_FORWARD      -- face viewer

  //// get mip factor from projection (if needed)
  //if ((INDEX &) rm.rm_fDistanceFactor == 12345678)
  //{
  //  FLOAT3D vObjectAbs;
  //  _aprProjection->PreClip(rm.rm_vObjectPosition, vObjectAbs);
  //  rm.rm_fDistanceFactor = _aprProjection->MipFactor(Min(vObjectAbs(3), 0.0f));
  //}
  //// adjust mip factor in case of dynamic stretch factor
  //if (mo_Stretch != FLOAT3D(1, 1, 1))
  //{
  //  rm.rm_fMipFactor = rm.rm_fDistanceFactor - Log2(Max(mo_Stretch(1), Max(mo_Stretch(2), mo_Stretch(3))));
  //}
  //else
  //{
  //  rm.rm_fMipFactor = rm.rm_fDistanceFactor;
  //}
  //// adjust mip factor by custom settings
  //rm.rm_fMipFactor = rm.rm_fMipFactor * mdl_fLODMul + mdl_fLODAdd;

  // get current mip model using mip factor
  rm.rm_iMipLevel = 0;//mo.GetMipModel(rm.rm_fMipFactor);
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
    RT_SetupModelRendering(pamo->amo_moModelObject, *pamo->amo_prm, ssrt);
  }
}


// RT: add vertices to corresponding CModelObject
//void RenderOneSide( CRenderModel &rm, BOOL bBackSide, ULONG ulLayerFlags)
static void RT_RenderOneSide(ULONG entityID, CRenderModel &rm,
                      const RT_VertexData &vd, BOOL bBackSide, 
                      const INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH], INDEX attchCount,
                      SSRT::SSRTMain *ssrt)
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

      // RT: let's assume that every surface is opaque for now
#if 0
      // skip surface if ... 
      if (!(ulFlags & SRF_DIFFUSE)  // not in this layer,
          || (bBackSide && !(ulFlags & SRF_DOUBLESIDED)) // rendering back side and surface is not double sided,
          /*|| !(_ulColorMask & ms.ms_ulOnColor)  // not on or off.
          || (_ulColorMask & ms.ms_ulOffColor)*/)
      {
        if (ctElements > 0)
        {
          FlushElements(ctElements, &mmi.mmpi_aiElements[ iStartElem ]);
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
          FlushElements(ctElements, &mmi.mmpi_aiElements[ iStartElem ]);
        }

        SetRenderingParameters(stt, slBump);
        sttLast = stt;
        iStartElem += ctElements;
        ctElements = 0;
      }
#endif

      // batch the surface polygons for rendering
      ctElements += ms.ms_ctSrfEl;
    }
  }

  // RT: flush all
  if (ctElements > 0)
  {
    //FlushElements(ctElements, &mmi.mmpi_aiElements[ iStartElem ]);

    INDEX *indices = &mmi.mmpi_aiElements[iStartElem];
    INDEX indexCount = ctElements;

    // TODO: RT: default, MF_HALF_FACE_FORWARD, MF_FACE_FORWARD

    SSRT::CModelGeometry modelInfo = {};
    modelInfo.entityID = entityID;
    modelInfo.isEnabled = true;
    modelInfo.absPosition = rm.rm_vObjectPosition;
    modelInfo.absRotation = rm.rm_mObjectRotation;
    modelInfo.color = RGBAToColor(255, 255, 255, 255);
    modelInfo.vertexCount = vd.vertexCount;
    modelInfo.vertices = vd.vertices;
    modelInfo.normals = vd.normals;
    modelInfo.texCoords = vd.texCoords;
    modelInfo.indexCount = indexCount;
    modelInfo.indices = indices;

    for (INDEX i = 0; i < SSRT_MAX_ATTACHMENT_DEPTH; i++)
    {
      modelInfo.attchPath[i] = attchPath[i];
    }

    ssrt->AddModel(modelInfo);
  }
}



static void RT_RenderModel_View(ULONG entityID, const CModelObject &mo, CRenderModel &rm,
                         const INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH], INDEX attchCount, SSRT::SSRTMain *ssrt)
//void CModelObject::RenderModel_View(CRenderModel &rm)
{
  // TODO: RT: use this if some additional transformation is required for weapons
  //(rm.rm_ulFlags & RMF_WEAPON)

  // setup drawing direction (in case of mirror)
  if (rm.rm_ulFlags & RMF_INVERTED)
  {
    //g fxFrontFace(GFX_CW);
  }
  else
  {
    //g fxFrontFace(GFX_CCW);
  }

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


  // set forced translucency and color mask
  //_bForceTranslucency = ((rm.rm_colBlend & CT_AMASK) >> CT_ASHIFT) != CT_OPAQUE;
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
  const BOOL bNeedNormals = TRUE;
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
        pnorSrfBase[ iSrfVx ].nx = pnorMipBase[ iMipVx ].nx;
        pnorSrfBase[ iSrfVx ].ny = pnorMipBase[ iMipVx ].ny;
        pnorSrfBase[ iSrfVx ].nz = pnorMipBase[ iMipVx ].nz;
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


  // RENDER DIFFUSE LAYER -------------------------------------------------------------------


  // get diffuse texture corrections
  const CTextureData *ptdDiff = (CTextureData *) mo.mo_toTexture.ao_AnimData;
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
      const COLOR colD = ms.ms_colDiffuse; // AdjustColor(ms.ms_colDiffuse, _slTexHueShift, _slTexSaturation);
      colSrfDiff.MultiplyRGBA(colD, colMdlDiff);

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

  // TODO: RT: texture loading

  // must render diffuse if there is no texture (white mode)
  if ((ulMipLayerFlags & SRF_DIFFUSE) || ptdDiff == NULL)
  {
    //gfxSetTexCoordArray(&_atexSrfBase[ 0 ], FALSE);

    RT_VertexData vd = {};
    vd.vertices = &_avtxSrfBase[0];
    vd.vertexCount = _ctAllSrfVx;
    vd.normals = &_anorSrfBase[0];
    vd.texCoords = &_atexSrfBase[0];

    // RT: everything is set, copy model data to SSRT
    //RT_RenderOneSide(rm, TRUE);
    RT_RenderOneSide(entityID, rm, vd, FALSE, attchPath, attchCount, ssrt);
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
static void RT_RenderModel(ULONG entityID, const CModelObject &mo, CRenderModel &rm, INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH], INDEX attchCount, SSRT::SSRTMain *ssrt)
{
  // skip invisible models
  if (mo.mo_Stretch == FLOAT3D(0, 0, 0)) return;

  // RT: _iRenderingType is 1 for not shadows

  // if we should draw polygons and model 
  if (!(rm.rm_ulFlags & RMF_SPECTATOR) && (!(rm.rm_rtRenderType & RT_NO_POLYGON_FILL)
                                           || (rm.rm_rtRenderType & RT_WIRE_ON) || (rm.rm_rtRenderType & RT_HIDDEN_LINES)))
  {
    //// eventually calculate projection model bounding box in object space (needed for fog/haze trivial rejection)
    //if (rm.rm_ulFlags & (RMF_FOG | RMF_HAZE)) CalculateBoundingBox(this, rm);
    // render complete model
    //rm.SetModelView();
    RT_RenderModel_View(entityID, mo, rm, attchPath, attchCount, ssrt);
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
    
    RT_RenderModel(entityID, pamo->amo_moModelObject, *pamo->amo_prm, attchPath, attchCount + 1, ssrt);
  }
}


//void CRenderer::RenderOneModel(CEntity &en, CModelObject &moModel, const CPlacement3D &plModel,
//                               const FLOAT fDistanceFactor, BOOL bRenderShadow, ULONG ulDMFlags)
static void RT_RenderOneModel(const CEntity &en, const CModelObject &moModel, const CPlacement3D &plModel,
                       const FLOAT fDistanceFactor, BOOL bRenderShadow, ULONG ulDMFlags, 
                       SSRT::SSRTMain *ssrt)
{
  // skip invisible models
  if (moModel.mo_Stretch == FLOAT3D(0, 0, 0)) return;

  // prepare render model structure
  CRenderModel rm;
  rm.rm_fDistanceFactor = fDistanceFactor;

  // RT: for calculating rotaion matrix and setting position vector
  rm.SetObjectPlacement(plModel);

  // TODO: RT: alpha
  // moModel->HasAlpha()

  //if (ulDMFlags & DMF_FOG)      rm.rm_ulFlags |= RMF_FOG;
  //if (ulDMFlags & DMF_HAZE)     rm.rm_ulFlags |= RMF_HAZE;

  ULONG viewerEntityID = -1; // ssrt->GetViewerEntityID();

  // mark that we don't actualy need entire model
  if (viewerEntityID == en.en_ulID)
  {
    rm.rm_ulFlags |= RMF_SPECTATOR;
  }

  /*if (IsOfClass(&en, "Player Weapons"))
  {
    rm.rm_ulFlags |= RMF_WEAPON;
  }*/

  // prepare CRenderModel structure for rendering of one model
  RT_SetupModelRendering(moModel, rm, ssrt);

  // if the entity is not the viewer, or this is not primary renderer
  if (viewerEntityID != en.en_ulID)
  {  
    // attachment path for SSRT::CModelGeometry,
    INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH];
    for (INDEX i = 0; i < SSRT_MAX_ATTACHMENT_DEPTH; i++)
    {
      // init all as -1
      attchPath[i] = -1;
    }

    // RT: process a model with all its attachmnets
    RT_RenderModel(en.en_ulID, moModel, rm, attchPath, 0, ssrt);
  }
  else
  {
    //// just remember the shading info (needed for first-person-weapon rendering)
    //_vViewerLightDirection = rm.rm_vLightDirection;
    //_colViewerLight = rm.rm_colLight;
    //_colViewerAmbient = rm.rm_colAmbient;
  }

  extern CStaticStackArray<CRenderModel> _armRenderModels;
  _armRenderModels.PopAll();
}


// CRenderer::RenderModels(BOOL bBackground)
static void RT_Post_RenderModels(const CEntity &en, const CModelObject &mo, SSRT::SSRTMain *ssrt)
{
  if (!gfx_bRenderModels)
  {
    return;
  }

  BOOL bIsBackground =/* re_bBackgroundEnabled &&*/ (en.en_ulFlags & ENF_BACKGROUND);

  // TODO: RT: ignore background for now
  if (bIsBackground)
  {
    return;
  }

  RT_RenderOneModel(en, mo, en.GetLerpedPlacement(), 0, TRUE, 0, ssrt);
}


void RT_AddModelEntity(const CEntity *penModel, SSRT::SSRTMain *ssrt)
{
  // if the entity is currently active or hidden, don't add it again
  if (penModel->en_ulFlags & (ENF_INRENDERING | ENF_HIDDEN))
  {
    return;
  }

  // skip the entity if predicted, and predicted entities should not be rendered
  if (penModel->IsPredicted() && !gfx_bRenderPredicted)
  {
    return;
  }
  
  // add light source there is one
  const CLightSource *pls = ((CEntity *) penModel)->GetLightSource();
  if (pls != NULL)
  {
    RT_AddLight(pls, ssrt);
  }

  // get its model object
  const CModelObject *pmoModelObject;
  if (penModel->en_RenderType != CEntity::RT_BRUSH &&
      penModel->en_RenderType != CEntity::RT_FIELDBRUSH)
  {
    pmoModelObject = ((CEntity *) penModel)->GetModelForRendering();
  }
  else
  {
    // empty brushes are also rendered as models
    //pmoModelObject = _wrpWorldRenderPrefs.wrp_pmoEmptyBrush;
    
    return;
  }

  // mark the entity as active in rendering
  //penModel->en_ulFlags |= ENF_INRENDERING;

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
  RT_Post_RenderModels(*penModel, *pmoModelObject, ssrt);
}


void RT_AddFirstPersonModel(const CModelObject *mo, CRenderModel *rm, ULONG entityId, SSRT::SSRTMain *ssrt)
{
  RT_SetupModelRendering(*mo, *rm, ssrt);

  // attachment path for SSRT::CModelGeometry,
  INDEX attchPath[SSRT_MAX_ATTACHMENT_DEPTH];
  for (INDEX i = 0; i < SSRT_MAX_ATTACHMENT_DEPTH; i++)
  {
    // init all as -1
    attchPath[i] = -1;
  }

  // RT: process a model with all its attachmnets
  RT_RenderModel(entityId, *mo, *rm, attchPath, 0, ssrt);

  extern CStaticStackArray<CRenderModel> _armRenderModels;
  _armRenderModels.PopAll();
}
