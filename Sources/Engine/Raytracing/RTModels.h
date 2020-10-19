#pragma once

#include "stdh.h"

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>
#include <Engine/Models/ModelData.h>

#include <Engine/Raytracing/SSRT.h>

// this will INTERRUPT original renderer
//CListHead RT_lhActiveSectors;     // list of active sectors


void RT_AddLight(CLightSource *plsLight, SSRT::SSRTMain *ssrt)
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
    light.pOriginalEntity = plsLight->ls_penEntity;
    light.isEnabled = true;
    light.absPosition = position;
    light.color = color;
    light.intensity = 1.0f;
    light.sphereRadius = radius;

    ssrt->AddLight(light);
  }
}

static CStaticStackArray<GFXVertex4> RT_AllBrushVertices;
static CStaticStackArray<INDEX> RT_AllBrushIndices;

void RT_AddActiveSector(CBrushSector &bscSector, SSRT::SSRTMain *ssrt)
{
  RT_AllBrushVertices.PopAll();
  RT_AllBrushIndices.PopAll();

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

    INDEX firstVertexId = RT_AllBrushVertices.Count();
    INDEX *origIndices = &polygon.bpo_aiTriangleElements[0];

    INDEX vertCount = polygon.bpo_apbvxTriangleVertices.Count();
    GFXVertex4 *vertices = RT_AllBrushVertices.Push(vertCount);

    for (INDEX i = 0; i < vertCount; i++)
    {
      CBrushVertex *brushVert = polygon.bpo_apbvxTriangleVertices[i];
      vertices[i].x = brushVert->bvx_vRelative(1);
      vertices[i].y = brushVert->bvx_vRelative(2);
      vertices[i].z = brushVert->bvx_vRelative(3);
      vertices[i].shade = 0;
    }

    INDEX indexCount = polygon.bpo_aiTriangleElements.Count();
    INDEX *indices = RT_AllBrushIndices.Push(indexCount);

    // set new indices relative to the shift in RT_AllBrushVertices
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
  brushInfo.vertexCount = RT_AllBrushVertices.Count();
  brushInfo.vertices = &RT_AllBrushVertices[0];
  brushInfo.texCoords = nullptr;
  brushInfo.normals = nullptr;
  brushInfo.indexCount = RT_AllBrushIndices.Count();
  brushInfo.indices = &RT_AllBrushIndices[0];

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


// ----------------------------

struct RT_VertexData
{
  GFXVertex   *vertices;
  INDEX       vertexCount;
  GFXNormal   *normals;
  GFXTexCoord *texCoords;
};

// Create render model structure for rendering an attached model
//BOOL CModelObject::CreateAttachment( CRenderModel &rmMain, CAttachmentModelObject &amo)
BOOL RT_CreateAttachment(CModelObject &moParent, CRenderModel &rmMain, CAttachmentModelObject &amo)
{
  CRenderModel &rmAttached = *amo.amo_prm;
  rmAttached.rm_ulFlags = rmMain.rm_ulFlags & (RMF_FOG | RMF_HAZE | RMF_WEAPON) | RMF_ATTACHMENT;

  // get the position
  rmMain.rm_pmdModelData->md_aampAttachedPosition.Lock();
  const CAttachedModelPosition &amp = rmMain.rm_pmdModelData->md_aampAttachedPosition[ amo.amo_iAttachedPosition ];
  rmMain.rm_pmdModelData->md_aampAttachedPosition.Unlock();

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
  moParent.UnpackVertex(rmMain, iCenter, vCenter);
  moParent.UnpackVertex(rmMain, iFront, vFront);
  moParent.UnpackVertex(rmMain, iUp, vUp);

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
  MakeRotationMatrixFast(mRelative, amo.amo_plRelative.pl_OrientationAngle);
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
void RT_SetupModelRendering(CModelObject &mo, CRenderModel &rm, SSRT::SSRTMain *ssrt)
{
  // get model's data and lerp info
  rm.rm_pmdModelData = (CModelData *) mo.GetData();
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
  mo.mo_iLastRenderMipLevel = rm.rm_iMipLevel;
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
void RT_RenderOneSide(CEntity *pen, CRenderModel &rm, CModelObject &mo, const RT_VertexData &vd, BOOL bBackSide, SSRT::SSRTMain *ssrt)
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
    modelInfo.pOriginalEntity = pen;
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

    ssrt->AddModel(modelInfo);
  }
}



void RT_RenderModel_View(CEntity *pen, CModelObject &mo, CRenderModel &rm, SSRT::SSRTMain *ssrt)
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
  const ModelMipInfo &mmi0 = rm.rm_pmdModelData->md_MipInfos[ 0 ];

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
      
      UWORD *puwSrfToMip = &mmi.mmpi_auwSrfToMip[ iSrfVx0 ];

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
  CTextureData *ptdDiff = (CTextureData *) mo.mo_toTexture.GetData();
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
      UWORD *puwSrfToMip = &mmi.mmpi_auwSrfToMip[ iSrfVx0 ];
      FLOAT2D *pvTexCoord = &mmi.mmpi_avmexTexCoord[ iSrfVx0 ];

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
    RT_RenderOneSide(pen, rm, mo, vd, FALSE, ssrt);
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
void RT_RenderModel(CEntity *pen, CModelObject &mo, CRenderModel &rm, SSRT::SSRTMain *ssrt)
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
    RT_RenderModel_View(pen, mo, rm, ssrt);
  }

  // render each attachment on this model object
  FOREACHINLIST(CAttachmentModelObject, amo_lnInMain, mo.mo_lhAttachments, itamo)
  {
    // calculate bounding box of an attachment    
    CAttachmentModelObject *pamo = itamo;
    if (pamo->amo_prm == NULL) continue; 
    
    RT_RenderModel(pen, pamo->amo_moModelObject, *pamo->amo_prm, ssrt);
  }
}


//void CRenderer::RenderOneModel(CEntity &en, CModelObject &moModel, const CPlacement3D &plModel,
//                               const FLOAT fDistanceFactor, BOOL bRenderShadow, ULONG ulDMFlags)
void RT_RenderOneModel(CEntity &en, CModelObject &moModel, const CPlacement3D &plModel,
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

  //if (ulDMFlags & DMF_FOG)      rm.rm_ulFlags |= RMF_FOG;
  //if (ulDMFlags & DMF_HAZE)     rm.rm_ulFlags |= RMF_HAZE;

  // RT: TODO: save current viewer entity
  CEntity *re_penViewer = NULL;

  // mark that we don't actualy need entire model
  if (re_penViewer == &en)
  {
    rm.rm_ulFlags |= RMF_SPECTATOR;
  }

  if (IsOfClass(&en, "Player Weapons"))
  {
    rm.rm_ulFlags |= RMF_WEAPON;
  }

  // prepare CRenderModel structure for rendering of one model
  RT_SetupModelRendering(moModel, rm, ssrt);

  // if the entity is not the viewer, or this is not primary renderer
  if (re_penViewer != &en)
  {  
    // RT: set all vertices (with attachments) in one array
    RT_RenderModel(&en, moModel, rm, ssrt);
  }
  else
  {
    //// just remember the shading info (needed for first-person-weapon rendering)
    //_vViewerLightDirection = rm.rm_vLightDirection;
    //_colViewerLight = rm.rm_colLight;
    //_colViewerAmbient = rm.rm_colAmbient;
  }
}


// CRenderer::RenderModels(BOOL bBackground)
void RT_Post_RenderModels(CDelayedModel &dm, SSRT::SSRTMain *ssrt)
{
  if (!gfx_bRenderModels)
  {
    return;
  }

  CEntity &en = *dm.dm_penModel;
  BOOL bIsBackground =/* re_bBackgroundEnabled &&*/ (en.en_ulFlags & ENF_BACKGROUND);

  // TODO: RT: ignore background for now
  if (bIsBackground)
  {
    return;
  }

  // render the model with its shadow
  CModelObject &moModelObject = *dm.dm_pmoModel;
  RT_RenderOneModel(en, moModelObject, en.GetLerpedPlacement(), dm.dm_fMipFactor, TRUE, dm.dm_ulFlags, ssrt);
}


void RT_AddModelEntity(CEntity *penModel, SSRT::SSRTMain *ssrt)
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
  CLightSource *pls = penModel->GetLightSource();
  if (pls != NULL)
  {
    RT_AddLight(pls, ssrt);
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
  penModel->en_ulFlags |= ENF_INRENDERING;

  // add it to a container for delayed rendering
  CDelayedModel dm = {}; // re_admDelayedModels.Push();
  dm.dm_penModel = penModel;
  dm.dm_pmoModel = pmoModelObject;
  dm.dm_ulFlags = NONE; // invisible until proved otherwise
  dm.dm_fDistance = 0;// fDistance;
  dm.dm_fMipFactor = 0;// fMipFactor;

  // if it has a light source with a lens flare and we're not rendering shadows
  //if (!re_bRenderingShadows && pls != NULL && pls->ls_plftLensFlare != NULL)
  //{
  //  AddLensFlare(penModel, pls, pprProjection, re_iIndex);
  //}

  // RT: flags for fog and haze?
  // adjust model flags
  if (pmoModelObject->HasAlpha())
  {
    dm.dm_ulFlags |= DMF_HASALPHA;
  }

  // safety check
  if (pmoModelObject == NULL)
  {
    return;
  }

  // allow its rendering
  //dm.dm_ulFlags |= DMF_VISIBLE;
  ASSERT(pmoModelObject != NULL);

  // RT: call it here to bypass re_admDelayedModels list
  RT_Post_RenderModels(dm, ssrt);
}



void RT_CleanUpLists()
{
  //// for all active sectors
  //{
  //  FORDELETELIST(CBrushSector, bsc_lnInActiveSectors, RT_lhActiveSectors, itbsc)
  //  {
  //    // remove it from list
  //    itbsc->bsc_lnInActiveSectors.Remove();

  //    // for all polygons in sector
  //    FOREACHINSTATICARRAY(itbsc->bsc_abpoPolygons, CBrushPolygon, itpo)
  //    {
  //      CBrushPolygon &polygon = *itpo;
  //      // clear screen polygon pointers
  //      polygon.bpo_pspoScreenPolygon = NULL;
  //    }
  //  }
  //}
  //ASSERT(RT_lhActiveSectors.IsEmpty());

  //// for all active brushes
  //{
  //  FORDELETELIST(CBrush3D, br_lnInActiveBrushes, RT_lhActiveBrushes, itbr)
  //  {
  //    // remove it from list
  //    itbr->br_lnInActiveBrushes.Remove();
  //  }
  //}
  //ASSERT(RT_lhActiveBrushes.IsEmpty());
}


void RT_AddAllEntities(CWorld *pwld, SSRT::SSRTMain *ssrt)
{
  RT_CleanUpLists();

  // for all entities in world
  FOREACHINDYNAMICCONTAINER(pwld->wo_cenEntities, CEntity, iten)
  {
    // if it is brush
    if (iten->en_RenderType == CEntity::RT_BRUSH)
    {
      // add all of its sectors
      RT_AddNonZoningBrush(&iten.Current(), NULL, ssrt);

    }
    else if (iten->en_RenderType == CEntity::RT_MODEL)
    {
      // add it as a model
      RT_AddModelEntity(&iten.Current(), ssrt);
    }
  }
}