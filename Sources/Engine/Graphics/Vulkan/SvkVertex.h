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

#ifndef SE_INCL_SVKVERTEX_H
#define SE_INCL_SVKVERTEX_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Graphics/Color.h>

#ifdef SE1_VULKAN

// Vertex structure that will be used in shaders
struct SvkVertex
{
  FLOAT Position[4];
  FLOAT Color[4];
  FLOAT Normal[4];
  FLOAT TexCoord[2];

  // default constructor
  SvkVertex()
  {
    Position[0] = Position[1] = Position[2] = 0.0f;
    Position[3] = 1.0f;
    Color[0] = Color[1] = Color[2] = Color[3] = 1.0f;
    Normal[0] = Normal[2] = Normal[3] = 0.0f;
    Normal[1] = 1.0f;
    TexCoord[0] = TexCoord[1] = 0.0f;
  }

  inline void SetPosition(FLOAT px, FLOAT py, FLOAT pz)
  {
    Position[0] = px;
    Position[1] = py;
    Position[2] = pz;
  }

  inline void SetNormal(FLOAT nx, FLOAT ny, FLOAT nz)
  {
    Normal[0] = nx;
    Normal[1] = ny;
    Normal[2] = nz;
  }

  inline void SetColorRGB(FLOAT r, FLOAT g, FLOAT b)
  {
    Color[0] = r;
    Color[1] = g;
    Color[2] = b;
  }

  inline void SetColorRGB(COLOR col)
  {
    BYTE colBytes[3];
    ColorToRGB(col, colBytes[0], colBytes[1], colBytes[2]);

    Color[0] = colBytes[0] / 255.0f;
    Color[1] = colBytes[1] / 255.0f;
    Color[2] = colBytes[2] / 255.0f;
  }

  inline void SetColorRGBA(FLOAT r, FLOAT g, FLOAT b, FLOAT a = 1.0f)
  {
    Color[0] = r;
    Color[1] = g;
    Color[2] = b;
    Color[3] = a;
  }

  inline void SetColorRGBA(COLOR col)
  {
    BYTE colBytes[4];
    ColorToRGBA(col, colBytes[0], colBytes[1], colBytes[2], colBytes[3]);

    Color[0] = colBytes[0] / 255.0f;
    Color[1] = colBytes[1] / 255.0f;
    Color[2] = colBytes[2] / 255.0f;
    Color[3] = colBytes[3] / 255.0f;
  }

  inline void SetTexCoord(FLOAT tx, FLOAT ty)
  {
    TexCoord[0] = tx;
    TexCoord[1] = ty;
  }
};

#define SVK_VERT_SIZE (sizeof(SvkVertex))

#define SVK_VERT_POS_SIZE (4*sizeof(FLOAT))
#define SVK_VERT_COL_SIZE (4*sizeof(FLOAT))
#define SVK_VERT_NOR_SIZE (4*sizeof(FLOAT))
#define SVK_VERT_TEX_SIZE (2*sizeof(FLOAT))

#define SVK_VERT_POS_FORMAT VK_FORMAT_R32G32B32A32_SFLOAT
#define SVK_VERT_COL_FORMAT VK_FORMAT_R32G32B32A32_SFLOAT
#define SVK_VERT_NOR_FORMAT VK_FORMAT_R32G32B32A32_SFLOAT
#define SVK_VERT_TEX_FORMAT VK_FORMAT_R32G32_SFLOAT

#define SVK_VERT_POS_OFFSET (offsetof(SvkVertex, Position))
#define SVK_VERT_COL_OFFSET (offsetof(SvkVertex, Color))
#define SVK_VERT_NOR_OFFSET (offsetof(SvkVertex, Normal))
#define SVK_VERT_TEX_OFFSET (offsetof(SvkVertex, TexCoord))

#define SVK_VERT_POS_LOC (0)
#define SVK_VERT_COL_LOC (1)
#define SVK_VERT_NOR_LOC (2)
#define SVK_VERT_TEX_LOC (3)


#endif
#endif