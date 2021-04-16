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
#include "Utils.h"

#include <Engine/Base/Shell.h>

CWorld *Utils::GetCurrentWorld()
{
#ifdef _WIN64
  CWorld *pwo = (CWorld *)_pShell->GetUINT64("pwoCurrentWorld64_0", "pwoCurrentWorld64_1");
#else
  CWorld *pwo = (CWorld *)_pShell->GetINDEX("pwoCurrentWorld");
#endif

  return pwo;
}

void Utils::CopyTransform(RgTransform &dst, const SSRT::CAbstractGeometry &src)
{
  for (uint32_t i = 0; i < 3; i++)
  {
    for (uint32_t j = 0; j < 3; j++)
    {
      dst.matrix[i][j] = src.absRotation.matrix[i][j];
    }
  }

  dst.matrix[0][3] = src.absPosition(1);
  dst.matrix[1][3] = src.absPosition(2);
  dst.matrix[2][3] = src.absPosition(3);
}

void Utils::CopyTransform(RgTransform &dst, const CPlacement3D &placement)
{
  const FLOAT3D &position = placement.pl_PositionVector;
  FLOATmatrix3D rotation;
  MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

  CopyTransform(dst, position, rotation);
}

void Utils::CopyTransform(RgTransform &dst, const FLOAT3D &position, const FLOATmatrix3D &rotation)
{
  for (uint32_t i = 0; i < 3; i++)
  {
    for (uint32_t j = 0; j < 3; j++)
    {
      dst.matrix[i][j] = rotation.matrix[i][j];
    }
  }

  dst.matrix[0][3] = position(1);
  dst.matrix[1][3] = position(2);
  dst.matrix[2][3] = position(3);
}
