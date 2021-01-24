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
#include "SSRTObjects.h"

namespace SSRT
{

bool CBrushGeometry::operator==(const CBrushGeometry &other) const
{
  if (this->entityID != other.entityID || isMovable != other.isMovable)
  {
    return false;
  }

  return true;
}

bool CModelGeometry::operator==(const CModelGeometry &other) const
{
  if (this->entityID != other.entityID)
  {
    return false;
  }

  for (INDEX i = 0; i < SSRT_MAX_ATTACHMENT_DEPTH; i++)
  {
    if (this->attchPath[i] == -1 && other.attchPath[i] == -1)
    {
      break;
    }

    if (this->attchPath[i] != other.attchPath[i])
    {
      return false;
    }
  }

  return true;
}

}