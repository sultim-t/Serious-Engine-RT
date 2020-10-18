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
#include "SSRT.h"

namespace SSRT
{
  ULONG RTObject::GetEnitityID() const 
  {
    ASSERT(pOriginalEntity != nullptr);
    return pOriginalEntity->en_ulID;
  }


  CAbstractGeometry::~CAbstractGeometry()
  {}


  inline CBrush3D *CBrushGeometry::GetOriginalBrush()
  {
    ASSERT(pOriginalEntity != nullptr);
    return pOriginalEntity->en_pbrBrush;
  }


  inline CModelObject *CModelGeometry::GetModelObject()
  {
    ASSERT(pOriginalEntity != nullptr);
    return pOriginalEntity->en_pmoModelObject;
  }


  inline bool CBrushGeometry::IsMovable() const
  {
    ASSERT(pOriginalEntity != nullptr);
    return pOriginalEntity->en_ulPhysicsFlags & EPF_MOVABLE;
  }


  void SSRTMain::AddModel(const CModelGeometry &model)
  {
    AddRTObject(model, models, entityToModel);
  }


  void SSRTMain::AddBrush(const CBrushGeometry &brush)
  {
    if (brush.IsMovable())
    {
      AddRTObject(brush, staticBrushes, entityToStaticBrush);
    }
    else
    {
      AddRTObject(brush, movableBrushes, entityToMovableBrush);
    }
  }


  void SSRTMain::AddLight(const CSphereLight &sphLt)
  {
    AddRTObject(sphLt, sphLights, entityToSphLight);
  }


  void SSRTMain::AddLight(const CDirectionalLight &dirLt)
  {
    AddRTObject(dirLt, dirLights, entityToDirLight);
  }
}