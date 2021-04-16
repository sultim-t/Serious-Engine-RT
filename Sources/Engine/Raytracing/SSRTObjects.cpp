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

uint64_t CModelGeometry::GetUniqueID() const
{
  return static_cast<uint64_t>(modelPartIndex) << 32 | entityID;
}

uint64_t CBrushGeometry::GetBrushUniqueID(ULONG _entityID, uint32_t _brushPartIndex)
{
  return static_cast<uint64_t>(_brushPartIndex) << 32 | _entityID;
}

uint64_t CBrushGeometry::GetUniqueID() const
{
  return GetBrushUniqueID(entityID, brushPartIndex);
}

}
