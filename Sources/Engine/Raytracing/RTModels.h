#pragma once

#include "stdh.h"

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>
#include <Engine/Models/ModelData.h>

// this will INTERRUPT original renderer
CListHead RT_lhActiveBrushes;     // list of active brushes
CListHead RT_lhActiveSectors;     // list of active sectors


void RT_AddLight(CLightSource *plsLight)
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
    return;
  }

  const CPlacement3D &placement = plsLight->ls_penEntity->GetPlacement();
  const FLOAT3D &position = placement.pl_PositionVector;

  UBYTE r, g, b;
  plsLight->GetLightColor(r, g, b);

  // directional light
  if (plsLight->ls_ulFlags & LSF_DIRECTIONAL)
  {
    FLOAT3D direction;
    AnglesToDirectionVector(placement.pl_OrientationAngle, direction);



  }
  else
  {
    // assume that hotspot is a radius of sphere light
    float radius = plsLight->ls_rHotSpot;
    float falloff = plsLight->ls_rFallOff;


  }
}

void RT_AddActiveSector(CBrushSector &bscSector)
{

  // TODO: Use absolute or relative verts
  // maybe export it to .obj file
  bscSector.bsc_abvxVertices[ 0 ].bvx_vAbsolute;

  bscSector.bsc_abpoPolygons[ 0 ].bpo_apbvxTriangleVertices;
  bscSector.bsc_abpoPolygons[ 0 ].bpo_aiTriangleElements;
  bscSector.bsc_abpoPolygons[ 0 ].bpo_colColor;
  bscSector.bsc_abpoPolygons[ 0 ].bpo_abptTextures;

  // RT: scan for all lights in sector
  CEntity *penSectorEntity = bscSector.bsc_pbmBrushMip->bm_pbrBrush->br_penEntity;
  // if it has the entity (it is not the background brush)
  if (penSectorEntity != NULL && bscSector.bsc_bspBSPTree.bt_pbnRoot != NULL)
  {
    FOREACHDSTOFSRC(bscSector.bsc_rsEntities, CEntity, en_rdSectors, pen)
      if (pen->en_RenderType == CEntity::RT_MODEL /*|| pen->en_RenderType == CEntity::RT_EDITORMODEL*/)
      {
        CLightSource *pls = pen->GetLightSource();

        if (pls != NULL)
        {
          RT_AddLight(pls);
        }
      }
    ENDFOR
  }



  // dont need it, original function just transformed vertices
  return;

  // if already active
  if (bscSector.bsc_lnInActiveSectors.IsLinked())
  {
    return;
  }
  RT_lhActiveSectors.AddTail(bscSector.bsc_lnInActiveSectors);

  // DONT, adding all entities is already being done
  //// get the entity the sector is in
  //CEntity *penSectorEntity = bscSector.bsc_pbmBrushMip->bm_pbrBrush->br_penEntity;
  //// if it has the entity (it is not the background brush)
  //if (penSectorEntity != NULL)
  //{
  //  // add all other entities near the sector
  //  AddEntitiesInSector(&bscSector);
  //}
}


// does nothing, actually
void RT_PrepareBrush(CEntity *penBrush)
{
  ASSERT(penBrush != NULL);
  // get its brush
  CBrush3D &brBrush = *penBrush->en_pbrBrush;

  // if the brush is already active in rendering
  if (brBrush.br_lnInActiveBrushes.IsLinked())
  {
    return;
  }
  RT_lhActiveBrushes.AddTail(brBrush.br_lnInActiveBrushes);

  CPlacement3D placement = 
    penBrush->en_ulPhysicsFlags & EPF_MOVABLE ?
    penBrush->GetLerpedPlacement() :
    penBrush->en_plPlacement;

  FLOAT3D position = placement.pl_PositionVector;
  FLOATmatrix3D rotation;
  MakeRotationMatrix(rotation, placement.pl_OrientationAngle);
}


void RT_AddNonZoningBrush(CEntity *penBrush, CBrushSector *pbscThatAdds)
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

  // if the brush is already added
  if (brBrush.br_lnInActiveBrushes.IsLinked())
  {
    // skip it
    return;
  }


  // skip whole non-zoning brush if all polygons in all sectors are invisible for rendering 
  if (!(penBrush->en_ulFlags & ENF_ZONING))
  { 
    // test every brush polygon for it's visibility flag
    // for every sector in brush
    FOREACHINDYNAMICARRAY(brBrush.GetFirstMip()->bm_abscSectors, CBrushSector, itbsc)
    {
      // for all polygons in sector
      FOREACHINSTATICARRAY(itbsc->bsc_abpoPolygons, CBrushPolygon, itpo)
      {
        // advance to next polygon if invisible
        CBrushPolygon &bpo = *itpo;
        if (!(bpo.bpo_ulFlags & BPOF_INVISIBLE)) goto addBrush;
      }
    }
    // skip this brush
    return;
  }

addBrush:
  // prepare the brush entity for rendering if not yet prepared
  RT_PrepareBrush(brBrush.br_penEntity);

  // get highest mip
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
        RT_AddActiveSector(itbsc.Current());
      }
    }
  }
}


// ----------------------------

// from RenderModel.cpp
static void CalculateBoundingBox(CModelObject *pmo, CRenderModel &rm)
{
  if (rm.rm_ulFlags & RMF_BBOXSET) return;
  // get model's data and lerp info
  rm.rm_pmdModelData = (CModelData *) pmo->GetData();
  pmo->GetFrame(rm.rm_iFrame0, rm.rm_iFrame1, rm.rm_fRatio);
  // calculate projection model bounding box in object space
  const FLOAT3D &vMin0 = rm.rm_pmdModelData->md_FrameInfos[ rm.rm_iFrame0 ].mfi_Box.Min();
  const FLOAT3D &vMax0 = rm.rm_pmdModelData->md_FrameInfos[ rm.rm_iFrame0 ].mfi_Box.Max();
  const FLOAT3D &vMin1 = rm.rm_pmdModelData->md_FrameInfos[ rm.rm_iFrame1 ].mfi_Box.Min();
  const FLOAT3D &vMax1 = rm.rm_pmdModelData->md_FrameInfos[ rm.rm_iFrame1 ].mfi_Box.Max();
  rm.rm_vObjectMinBB = Lerp(vMin0, vMin1, rm.rm_fRatio);
  rm.rm_vObjectMaxBB = Lerp(vMax0, vMax1, rm.rm_fRatio);
  rm.rm_vObjectMinBB(1) *= pmo->mo_Stretch(1);  rm.rm_vObjectMaxBB(1) *= pmo->mo_Stretch(1);
  rm.rm_vObjectMinBB(2) *= pmo->mo_Stretch(2);  rm.rm_vObjectMaxBB(2) *= pmo->mo_Stretch(2);
  rm.rm_vObjectMinBB(3) *= pmo->mo_Stretch(3);  rm.rm_vObjectMaxBB(3) *= pmo->mo_Stretch(3);
  rm.rm_ulFlags |= RMF_BBOXSET;
}

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
void RT_SetupModelRendering(CModelObject &mo, CRenderModel &rm)
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
    const BOOL bVisible = RT_CreateAttachment(mo, rm, *pamo);
    if (!bVisible)
    { // skip if not visible
      pamo->amo_prm = NULL;
      _armRenderModels.Pop();
      continue;
    } 
    
    // prepare if visible
    RT_SetupModelRendering(pamo->amo_moModelObject, *pamo->amo_prm);
  }
}


void RT_RenderModel_View(CModelObject &mo, CRenderModel &rm)
//void CModelObject::RenderModel_View(CRenderModel &rm)
{
  // TODO: RT: use this if some additional transformation is required for weapons
  //(rm.rm_ulFlags & RMF_WEAPON)

  // setup drawing direction (in case of mirror)
  if (rm.rm_ulFlags & RMF_INVERTED)
  {
    //gfxFrontFace(GFX_CW);
  }
  else
  {
    //gfxFrontFace(GFX_CCW);
  }

  // declare pointers for general usage
  INDEX iSrfVx0, ctSrfVx;
  GFXTexCoord *ptexSrfBase;
  GFXTexCoord *ptexSrfBump;
  GFXVertex *pvtxSrfBase;
  GFXColor *pcolSrfBase;
  FLOAT2D *pvTexCoord;
  ModelMipInfo &mmi = *rm.rm_pmmiMip;
  const ModelMipInfo &mmi0 = rm.rm_pmdModelData->md_MipInfos[ 0 ];

  // allocate vertex arrays
  INDEX ctAllMipVx = mmi.mmpi_ctMipVx;
  INDEX ctAllSrfVx = mmi.mmpi_ctSrfVx;
  ASSERT(ctAllMipVx > 0 && ctAllSrfVx > 0);

  extern CStaticStackArray<GFXVertex3>  _avtxMipBase;
  extern CStaticStackArray<GFXTexCoord> _atexMipBase;
  extern CStaticStackArray<GFXNormal3>  _anorMipBase;
  extern CStaticStackArray<GFXColor>    _acolMipBase;

  extern CStaticStackArray<GFXVertex4>  _avtxSrfBase;
  extern CStaticStackArray<GFXNormal4>  _anorSrfBase;
  extern CStaticStackArray<GFXTexCoord> _atexSrfBase;
  extern CStaticStackArray<GFXColor>    _acolSrfBase;

  ASSERT(_avtxMipBase.Count() == 0);  _avtxMipBase.Push(ctAllMipVx);
  ASSERT(_atexMipBase.Count() == 0);  _atexMipBase.Push(ctAllMipVx);
  ASSERT(_acolMipBase.Count() == 0);  _acolMipBase.Push(ctAllMipVx);
  ASSERT(_anorMipBase.Count() == 0);  _anorMipBase.Push(ctAllMipVx);

  ASSERT(_avtxSrfBase.Count() == 0);  _avtxSrfBase.Push(ctAllSrfVx);
  ASSERT(_atexSrfBase.Count() == 0);  _atexSrfBase.Push(ctAllSrfVx);
  ASSERT(_acolSrfBase.Count() == 0);  _acolSrfBase.Push(ctAllSrfVx);
  ASSERT(_anorSrfBase.Count() == 0);  _anorSrfBase.Push(ctAllSrfVx);


  // determine multitexturing capability for overbrighting purposes
  //extern INDEX mdl_bAllowOverbright;
  //const BOOL bOverbright = mdl_bAllowOverbright && _pGfx->gl_ctTextureUnits > 1;

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
  extern GFXColor *pcolMipBase;
  extern GFXVertex3 *pvtxMipBase;
  extern GFXNormal3 *pnorMipBase;

  pvtxMipBase = &_avtxMipBase[ 0 ];
  pcolMipBase = &_acolMipBase[ 0 ];
  pnorMipBase = &_anorMipBase[ 0 ];

  // RT: always get normals
  const BOOL bNeedNormals = TRUE;
  extern void UnpackFrame(CRenderModel & rm, BOOL bKeepNormals);
  UnpackFrame(rm, bNeedNormals);

  // PREPARE SURFACE VERTICES ------------------------------------------------------------------------


  // for each surface in current mip model
  BOOL bEmpty = TRUE;
  {
    FOREACHINSTATICARRAY(mmi.mmpi_MappingSurfaces, MappingSurface, itms)
    {
      const MappingSurface &ms = *itms;
      iSrfVx0 = ms.ms_iSrfVx0;
      ctSrfVx = ms.ms_ctSrfVx;
      // skip to next in case of invisible or empty surface
      if ((ms.ms_ulRenderingFlags & SRF_INVISIBLE) || ctSrfVx == 0) break;
      bEmpty = FALSE;
      UWORD *puwSrfToMip = &mmi.mmpi_auwSrfToMip[ iSrfVx0 ];
      pvtxSrfBase = &_avtxSrfBase[ iSrfVx0 ];
      INDEX iSrfVx;

      // setup vetrex array
      for (iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++)
      {
        const INDEX iMipVx = puwSrfToMip[ iSrfVx ];
        pvtxSrfBase[ iSrfVx ].x = pvtxMipBase[ iMipVx ].x;
        pvtxSrfBase[ iSrfVx ].y = pvtxMipBase[ iMipVx ].y;
        pvtxSrfBase[ iSrfVx ].z = pvtxMipBase[ iMipVx ].z;
      }

      // setup normal array for truform (if enabled)
      GFXNormal *pnorSrfBase = &_anorSrfBase[ iSrfVx0 ];
      for (iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++)
      {
        const INDEX iMipVx = puwSrfToMip[ iSrfVx ];
        pnorSrfBase[ iSrfVx ].nx = pnorMipBase[ iMipVx ].nx;
        pnorSrfBase[ iSrfVx ].ny = pnorMipBase[ iMipVx ].ny;
        pnorSrfBase[ iSrfVx ].nz = pnorMipBase[ iMipVx ].nz;
      }
    }
  }
  // prepare (and lock) vertex array
  //gfxEnableDepthTest();
  gfxSetVertexArray(&_avtxSrfBase[ 0 ], ctAllSrfVx);
  gfxSetNormalArray(&_anorSrfBase[ 0 ]);

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
  const COLOR colD = AdjustColor(rm.rm_pmdModelData->md_colDiffuse, _slTexHueShift, _slTexSaturation);
  const COLOR colB = AdjustColor(rm.rm_colBlend, _slTexHueShift, _slTexSaturation);
  colMdlDiff.MultiplyRGBA(colD, colB);

  // for each surface in current mip model
  {
    FOREACHINSTATICARRAY(mmi.mmpi_MappingSurfaces, MappingSurface, itms)
    {
      const MappingSurface &ms = *itms;
      iSrfVx0 = ms.ms_iSrfVx0;
      ctSrfVx = ms.ms_ctSrfVx;
      if ((ms.ms_ulRenderingFlags & SRF_INVISIBLE) || ctSrfVx == 0) break;  // done if found invisible or empty surface
      // cache surface pointers
      UWORD *puwSrfToMip = &mmi.mmpi_auwSrfToMip[ iSrfVx0 ];
      pvTexCoord = &mmi.mmpi_avmexTexCoord[ iSrfVx0 ];
      ptexSrfBase = &_atexSrfBase[ iSrfVx0 ];
      pcolSrfBase = &_acolSrfBase[ iSrfVx0 ];

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
      for (INDEX iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++) pcolSrfBase[ iSrfVx ] = colSrfDiff;

      /*if (ms.ms_sstShadingType == SST_FULLBRIGHT)
      {
        // eventually adjust reflection color for overbrighting
        GFXColor colSrfDiffAdj = colSrfDiff;
        //if (bOverbright)
        //{
        //  colSrfDiffAdj.r >>= 1;
        //  colSrfDiffAdj.g >>= 1;
        //  colSrfDiffAdj.b >>= 1;
        //} // just copy diffuse color
        for (INDEX iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++) pcolSrfBase[ iSrfVx ] = colSrfDiffAdj;
      }
      else
      {
        // setup diffuse color array
        for (INDEX iSrfVx = 0; iSrfVx < ctSrfVx; iSrfVx++)
        {
          const INDEX iMipVx = puwSrfToMip[ iSrfVx ];
          pcolSrfBase[ iSrfVx ].MultiplyRGBCopyA1(colSrfDiff, pcolMipBase[ iMipVx ]);
        }
      }*/
    }
  }

  // must render diffuse if there is no texture (white mode)
  if ((ulMipLayerFlags & SRF_DIFFUSE) || ptdDiff == NULL)
  {
    // prepare overbrighting if supported
    //if (bOverbright) gfxSetTextureModulation(2);

    //// set texture/color arrays 
    //INDEX iFrame = 0;
    //if (ptdDiff != NULL) iFrame = mo.mo_toTexture.GetFrame();

    //// !!! REMOVE THIS, we need only to get info
    //extern void SetCurrentTexture(CTextureData *ptd, INDEX iFrame);
    //SetCurrentTexture(ptdDiff, iFrame);

    gfxSetTexCoordArray(&_atexSrfBase[ 0 ], FALSE);
    //gfxSetColorArray(&_acolSrfBase[ 0 ]);

    // !!! REMOVE THIS, we need only to get info
    extern void RenderOneSide(CRenderModel & rm, BOOL bBackSide, ULONG ulLayerFlags);
    RenderOneSide(rm, TRUE, SRF_DIFFUSE);
    RenderOneSide(rm, FALSE, SRF_DIFFUSE);

    // revert to normal brightness if overbrighting was on
    //if (bOverbright) gfxSetTextureModulation(1);
  }

  // adjust z-buffer and blending functions
  if (ulMipLayerFlags & MMI_OPAQUE)
  {
    //gfxDepthFunc(GFX_EQUAL);
  }
  else
  {
    //gfxDepthFunc(GFX_LESS_EQUAL);
  }
  //gfxDisableDepthWrite();
  //gfxDisableAlphaTest(); // disable alpha testing if enabled after some surface
  //gfxEnableBlend();

  // almost done
  //gfxDepthFunc(GFX_LESS_EQUAL);
  //gfxUnlockArrays();

  // reset model vertex buffers and rendering face
  extern void ResetVertexArrays(void);
  ResetVertexArrays();
}


//void CModelObject::RenderModel(CRenderModel &rm)
void RT_RenderModel(CModelObject &mo, CRenderModel &rm)
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
    RT_RenderModel_View(rm);
  }

  // render each attachment on this model object
  FOREACHINLIST(CAttachmentModelObject, amo_lnInMain, mo.mo_lhAttachments, itamo)
  {
    // calculate bounding box of an attachment    
    CAttachmentModelObject *pamo = itamo;
    if (pamo->amo_prm == NULL) continue; // skip view-rejected attachments
    RT_RenderModel(pamo->amo_moModelObject, *pamo->amo_prm);
  }
}


//void CRenderer::RenderOneModel(CEntity &en, CModelObject &moModel, const CPlacement3D &plModel,
//                               const FLOAT fDistanceFactor, BOOL bRenderShadow, ULONG ulDMFlags)
void RT_RenderOneModel(CEntity &en, CModelObject &moModel, const CPlacement3D &plModel,
                       const FLOAT fDistanceFactor, BOOL bRenderShadow, ULONG ulDMFlags)
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
  //if (ulDMFlags & DMF_INSIDE)   rm.rm_ulFlags |= RMF_INSIDE;
  //if (ulDMFlags & DMF_INMIRROR) rm.rm_ulFlags |= RMF_INMIRROR;

  // RT: TODO: save current viewer entity
  CEntity *re_penViewer = NULL;

  // mark that we don't actualy need entire model
  if (re_penViewer == &en)
  {
    rm.rm_ulFlags |= RMF_SPECTATOR;
  }

  // TEMP: disable Truform usage on weapon models
  if (IsOfClass(&en, "Player Weapons")) rm.rm_ulFlags |= RMF_WEAPON;

  // prepare CRenderModel structure for rendering of one model
  RT_SetupModelRendering(moModel, rm);

  // RT: no plane shadows

  // if the entity is not the viewer, or this is not primary renderer
  if (re_penViewer != &en)
  {
    // render model
    RT_RenderModel(moModel, rm);
    // if the entity is viewer
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
void RT_Post_RenderModels(CDelayedModel &dm)
{
  //if (_bMultiPlayer) gfx_bRenderModels = 1; // must render in multiplayer mode!
  //if (!gfx_bRenderModels && !re_bRenderingShadows) return;

  //// sort all the delayed models by distance
  //qsort(re_admDelayedModels.GetArrayOfPointers(), re_admDelayedModels.Count(),
  //      sizeof(CDelayedModel *), qsort_CompareDelayedModels);

  //CAnyProjection3D *papr;
  //if (bBackground)
  //{
  //  papr = &re_prBackgroundProjection;
  //}
  //else
  //{
  //  papr = &re_prProjection;
  //}
  //// begin model rendering
  //if (!re_bRenderingShadows)
  //{
  //  BeginModelRenderingView(*papr, re_pdpDrawPort);
  //  RM_BeginRenderingView(*papr, re_pdpDrawPort);
  //}
  //else
  //{
  //  BeginModelRenderingMask(*papr, re_pubShadow, re_slShadowWidth, re_slShadowHeight);
  //  RM_BeginModelRenderingMask(*papr, re_pubShadow, re_slShadowWidth, re_slShadowHeight);
  //}


  // for each of models that were kept for delayed rendering
  //for (INDEX iModel = 0; iModel < re_admDelayedModels.Count(); iModel++)
  {
    //CDelayedModel &dm = re_admDelayedModels[ iModel ];

    CEntity &en = *dm.dm_penModel;
    BOOL bIsBackground =/* re_bBackgroundEnabled &&*/ (en.en_ulFlags & ENF_BACKGROUND);

    //// skip if not rendered in this pass or not visible
    //if (/*(bBackground && !bIsBackground)
    //    || (!bBackground && bIsBackground)
    //    ||*/ !(dm.dm_ulFlags & DMF_VISIBLE)) continue;

    if (en.en_RenderType == CEntity::RT_SKAMODEL || en.en_RenderType == CEntity::RT_SKAEDITORMODEL)
    {
      // ASSUME THAT THIS NEVER HAPPENS
    }
    else
    {
      // render the model with its shadow
      CModelObject &moModelObject = *dm.dm_pmoModel;
      RT_RenderOneModel(en, moModelObject, en.GetLerpedPlacement(), dm.dm_fMipFactor, TRUE, dm.dm_ulFlags);
    }

  }
  //// end model rendering
  //EndModelRenderingView(FALSE); // don't restore ortho projection for now
  //RM_EndRenderingView(FALSE);
}


void RT_AddModelEntity(CEntity *penModel)
{
  // if the entity is currently active or hidden, don't add it again
  if (penModel->en_ulFlags & (ENF_INRENDERING | ENF_HIDDEN)) return;
  // skip the entity if predicted, and predicted entities should not be rendered
  if (penModel->IsPredicted() && !gfx_bRenderPredicted) return;

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

  // add it to container of all drawn entities
  //re_cenDrawn.Add(penModel);
  // add it to a container for delayed rendering
  CDelayedModel dm = {}; // re_admDelayedModels.Push();
  dm.dm_penModel = penModel;
  dm.dm_pmoModel = pmoModelObject;
  dm.dm_ulFlags = NONE; // invisible until proved otherwise
  //// get proper projection for the entity
  //CProjection3D *pprProjection;
  //if (re_bBackgroundEnabled && (penModel->en_ulFlags & ENF_BACKGROUND))
  //{
  //  pprProjection = re_prBackgroundProjection;
  //}
  //else
  //{
  //  pprProjection = re_prProjection;
  //}
  //// transform its handle to view space
  //FLOAT3D vHandle;
  //pprProjection->PreClip(penModel->en_plPlacement.pl_PositionVector, vHandle);
  //// setup mip factor
  //FLOAT fDistance = vHandle(3) + penModel->GetDepthSortOffset();
  //FLOAT fMipFactor = pprProjection->MipFactor(fDistance);
  //penModel->AdjustMipFactor(fMipFactor);
  dm.dm_fDistance = 0;// fDistance;
  dm.dm_fMipFactor = 0;// fMipFactor;

//  FLOAT fR = penModel->en_fSpatialClassificationRadius;
//  if (penModel->en_RenderType == CEntity::RT_BRUSH
//      || penModel->en_RenderType == CEntity::RT_FIELDBRUSH)
//  {
//    fR = 1.0f;
//  }
//  ASSERT(fR > 0.0f);
//
//  // test object sphere to frustum
//  FLOATobbox3D boxEntity;
//  BOOL  bModelHasBox = FALSE;
//  INDEX iFrustumTest = pprProjection->TestSphereToFrustum(vHandle, fR);
//  // if test is indeterminate
//  if (iFrustumTest == 0)
//  {
//    // create oriented box and test it to frustum
//    CreateModelOBBox(penModel, vHandle, pprProjection->pr_ViewerRotationMatrix, boxEntity);
//    bModelHasBox = TRUE; // mark that box has been created
//    iFrustumTest = pprProjection->TestBoxToFrustum(boxEntity);
//  }
//  // if not inside 
//  if (iFrustumTest < 0)
//  {
//    return;
//  }
//
//  // do additional check for eventual mirror plane (if allowed)
//  INDEX iMirrorPlaneTest = -1;
//  extern INDEX gap_iOptimizeClipping;
//  if (gap_iOptimizeClipping > 0 && (pprProjection->pr_bMirror || pprProjection->pr_bWarp))
//  {
//    // test sphere against plane
//    const FLOAT fPlaneDistance = pprProjection->pr_plMirrorView.PointDistance(vHandle);
//    if (fPlaneDistance < -fR) iMirrorPlaneTest = -1;
//    else if (fPlaneDistance > +fR) iMirrorPlaneTest = +1;
//    else
//    { // if test is indeterminate
//// create box entity if needed
//      if (!bModelHasBox)
//      {
//        CreateModelOBBox(penModel, vHandle, pprProjection->pr_ViewerRotationMatrix, boxEntity);
//        bModelHasBox = TRUE;
//      } // test it to mirror/warp plane
//      iMirrorPlaneTest = boxEntity.TestAgainstPlane(pprProjection->pr_plMirrorView);
//    }
//    // if not in mirror
//    if (iMirrorPlaneTest < 0)
//    {
//      return;
//    }
//  }

  // if it has a light source with a lens flare and we're not rendering shadows
  CLightSource *pls = penModel->GetLightSource();
  //if (!re_bRenderingShadows && pls != NULL && pls->ls_plftLensFlare != NULL)
  //{
  //  // add the lens flare to rendering
  //  AddLensFlare(penModel, pls, pprProjection, re_iIndex);
  //}

  // RT: flags for fog and haze?
  // adjust model flags
  if (pmoModelObject->HasAlpha())
  {
    dm.dm_ulFlags |= DMF_HASALPHA;
  }
  //if (re_bCurrentSectorHasFog)
  {
    //dm.dm_ulFlags |= DMF_FOG;
  }
  //if (re_bCurrentSectorHasHaze)
  {
    //dm.dm_ulFlags |= DMF_HAZE;
  }

  //// if this is an editor model and editor models are disabled
  //if (penModel->en_RenderType == CEntity::RT_EDITORMODEL
  //    && !_wrpWorldRenderPrefs.IsEditorModelsOn())
  //{
  //  return;
  //}

  // safety check
  if (pmoModelObject == NULL)
  {
    return;
  }

  //// if the object is not visible at its distance
  //if ((!re_bRenderingShadows && !pmoModelObject->IsModelVisible(fMipFactor)))
  //{
  //  return;
  //}

  //// if entity selecting by laso requested, test for selecting
  //if (_pselenSelectOnRender != NULL) SelectEntityOnRender(*pprProjection, *penModel);

  // allow its rendering
  //dm.dm_ulFlags |= DMF_VISIBLE;
  ASSERT(pmoModelObject != NULL);

  // if rendering shadows use only first mip level
  //if (re_bRenderingShadows) dm.dm_fMipFactor = 0;

  // RT: call it here to bypass re_admDelayedModels list
  RT_Post_RenderModels(dm);
}



void RT_CleanUpLists()
{
  // for all active sectors
  {
    FORDELETELIST(CBrushSector, bsc_lnInActiveSectors, RT_lhActiveSectors, itbsc)
    {
      // remove it from list
      itbsc->bsc_lnInActiveSectors.Remove();

      // for all polygons in sector
      FOREACHINSTATICARRAY(itbsc->bsc_abpoPolygons, CBrushPolygon, itpo)
      {
        CBrushPolygon &bpo = *itpo;
        // clear screen polygon pointers
        bpo.bpo_pspoScreenPolygon = NULL;
      }
    }
  }
  ASSERT(RT_lhActiveSectors.IsEmpty());

  // for all active brushes
  {
    FORDELETELIST(CBrush3D, br_lnInActiveBrushes, RT_lhActiveBrushes, itbr)
    {
      // remove it from list
      itbr->br_lnInActiveBrushes.Remove();
    }
  }
  ASSERT(RT_lhActiveBrushes.IsEmpty());
}


void RT_AddAllEntities(CWorld *pwld)
{
  RT_CleanUpLists();

  // for all entities in world
  FOREACHINDYNAMICCONTAINER(pwld->wo_cenEntities, CEntity, iten)
  {
    // if it is brush
    if (iten->en_RenderType == CEntity::RT_BRUSH
        || (iten->en_RenderType == CEntity::RT_FIELDBRUSH
            && _wrpWorldRenderPrefs.IsFieldBrushesOn()))
    {
      // add all of its sectors
      RT_AddNonZoningBrush(&iten.Current(), NULL);

      // if it is model, or editor model that should be drawn
    }
    else if (iten->en_RenderType == CEntity::RT_MODEL
             /*|| (iten->en_RenderType == CEntity::RT_EDITORMODEL
                 && _wrpWorldRenderPrefs.IsEditorModelsOn())*/)
    {
      // add it as a model
      RT_AddModelEntity(&iten.Current());
    }
  }
}