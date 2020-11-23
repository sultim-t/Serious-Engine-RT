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

#include <Engine/Raytracing/SSRT.h>


void RT_AddHudQuads(SSRT::CHudElementInfo *preparedInfo, SSRT::SSRTMain *ssrt)
{
  const INDEX ctElements = preparedInfo->vertexCount * 6 / 4;
  if (ctElements <= 0)
  {
    return;
  }

  // make sure that enough quad elements has been initialized,
  // indices are the same so use one array without changing it
  const INDEX ctQuads = _aiCommonQuads.Count();
  if (ctElements > ctQuads)
  {
    const INDEX toAdd = ctElements - ctQuads;

    const INDEX iStart = _aiCommonQuads.Count() / 6 * 4;
    INDEX *piQuads = _aiCommonQuads.Push(toAdd * 6);
    for (INDEX i = 0; i < toAdd; i++)
    {
      piQuads[i * 6 + 0] = iStart + i * 4 + 0;
      piQuads[i * 6 + 1] = iStart + i * 4 + 1;
      piQuads[i * 6 + 2] = iStart + i * 4 + 2;
      piQuads[i * 6 + 3] = iStart + i * 4 + 2;
      piQuads[i * 6 + 4] = iStart + i * 4 + 3;
      piQuads[i * 6 + 5] = iStart + i * 4 + 0;
    }
  }

  preparedInfo->pIndices = &_aiCommonQuads[0];
  preparedInfo->indexCount = ctElements;

  ssrt->AddHudElement(*preparedInfo);
}


void RT_AddHudElements(SSRT::CHudElementInfo *preparedInfo, SSRT::SSRTMain *ssrt)
{
  // everything is ready
  ssrt->AddHudElement(*preparedInfo);
}
