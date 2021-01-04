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

#include "stdh.h"

#ifdef SE1_VULKAN

void Svk_MatCopy(float *dest, const float *src)
{
  memcpy(dest, src, 16 * sizeof(float));
}

void Svk_MatSetIdentity(float *result)
{
  memset(result, 0, 16 * sizeof(float));

  result[0] = result[5] =
    result[10] = result[15] = 1.0f;
}

void Svk_MatMultiply(float *result, const float *a, const float *b)
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      result[i * 4 + j] =
        a[i * 4 + 0] * b[0 * 4 + j] +
        a[i * 4 + 1] * b[1 * 4 + j] +
        a[i * 4 + 2] * b[2 * 4 + j] +
        a[i * 4 + 3] * b[3 * 4 + j];
    }
  }
}

void Svk_MatFrustum(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
{
  const float fRpL = fRight + fLeft;  const float fRmL = fRight - fLeft;  const float fFpN = fFar + fNear;
  const float fTpB = fTop + fBottom;  const float fTmB = fTop - fBottom;  const float fFmN = fFar - fNear;
  const float f2Fm2N = 2.0f * fFar - 2.0f * fNear;

  result[0 * 4 + 0] = 2.0f * fNear / fRmL;
  result[0 * 4 + 1] = 0.0f;
  result[0 * 4 + 2] = 0.0f;
  result[0 * 4 + 3] = 0.0f;

  result[1 * 4 + 0] = 0.0f;
  result[1 * 4 + 1] = -2.0f * fNear / fTmB; // 2.0f * fNear / fTmB;
  result[1 * 4 + 2] = 0.0f;
  result[1 * 4 + 3] = 0.0f;

  result[2 * 4 + 0] = fRpL / fRmL;
  result[2 * 4 + 1] = -fTpB / fTmB; // fTpB / fTmB;
  result[2 * 4 + 2] = -(2 * fFar - fNear) / f2Fm2N; // -fFar / fFmN;
  result[2 * 4 + 3] = -1.0f;

  result[3 * 4 + 0] = 0.0f;
  result[3 * 4 + 1] = 0.0f;
  result[3 * 4 + 2] = -fFar * fNear / f2Fm2N; // -fFar * fNear / fFmN;
  result[3 * 4 + 3] = 0.0f;
}

void Svk_MatOrtho(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
{
  const float fRpL = fRight + fLeft;  const float fRmL = fRight - fLeft;  const float fFpN = fFar + fNear;
  const float fTpB = fTop + fBottom;  const float fTmB = fTop - fBottom;  const float fFmN = fFar - fNear;
  const float f2Fm2N = 2 * fFar - 2 * fNear;

  result[0 * 4 + 0] = 2.0f / fRmL;
  result[0 * 4 + 1] = 0.0f;
  result[0 * 4 + 2] = 0.0f;
  result[0 * 4 + 3] = 0.0f;

  result[1 * 4 + 0] = 0.0f;
  result[1 * 4 + 1] = -2.0f / fTmB; // 2.0f / fTmB;
  result[1 * 4 + 2] = 0.0f;
  result[1 * 4 + 3] = 0.0f;

  result[2 * 4 + 0] = 0.0f;
  result[2 * 4 + 1] = 0.0f;
  result[2 * 4 + 2] = -1.0f / f2Fm2N; // -1.0f / fFmN;
  result[2 * 4 + 3] = 0.0f;

  result[3 * 4 + 0] = -fRpL / fRmL;
  result[3 * 4 + 1] = fTpB / fTmB; // -fTpB / fTmB;
  result[3 * 4 + 2] = (fFar - 2.0f * fNear) / f2Fm2N;// -fNear / fFmN; 
  result[3 * 4 + 3] = 1.0f;
}

void Svk_MatInverse(float *pResult, const float *pM)
{
  // cast to multidim array
  const float(&m)[4][4] = *reinterpret_cast<const float(*)[4][4]>(pM);
  float(&result)[4][4] = *reinterpret_cast<float(*)[4][4]>(pResult);

  result[0][0] = 
    m[1][1] * m[2][2] * m[3][3] -
    m[1][1] * m[2][3] * m[3][2] -
    m[2][1] * m[1][2] * m[3][3] +
    m[2][1] * m[1][3] * m[3][2] +
    m[3][1] * m[1][2] * m[2][3] -
    m[3][1] * m[1][3] * m[2][2];

  result[1][0] = 
   -m[1][0] * m[2][2] * m[3][3] +
    m[1][0] * m[2][3] * m[3][2] +
    m[2][0] * m[1][2] * m[3][3] -
    m[2][0] * m[1][3] * m[3][2] -
    m[3][0] * m[1][2] * m[2][3] +
    m[3][0] * m[1][3] * m[2][2];

  result[2][0] = 
    m[1][0] * m[2][1] * m[3][3] -
    m[1][0] * m[2][3] * m[3][1] -
    m[2][0] * m[1][1] * m[3][3] +
    m[2][0] * m[1][3] * m[3][1] +
    m[3][0] * m[1][1] * m[2][3] -
    m[3][0] * m[1][3] * m[2][1];

  result[3][0] = 
   -m[1][0] * m[2][1] * m[3][2] +
    m[1][0] * m[2][2] * m[3][1] +
    m[2][0] * m[1][1] * m[3][2] -
    m[2][0] * m[1][2] * m[3][1] -
    m[3][0] * m[1][1] * m[2][2] +
    m[3][0] * m[1][2] * m[2][1];

  result[0][1] =
   -m[0][1] * m[2][2] * m[3][3] +
    m[0][1] * m[2][3] * m[3][2] +
    m[2][1] * m[0][2] * m[3][3] -
    m[2][1] * m[0][3] * m[3][2] -
    m[3][1] * m[0][2] * m[2][3] +
    m[3][1] * m[0][3] * m[2][2];

  result[1][1] = 
    m[0][0] * m[2][2] * m[3][3] -
    m[0][0] * m[2][3] * m[3][2] -
    m[2][0] * m[0][2] * m[3][3] +
    m[2][0] * m[0][3] * m[3][2] +
    m[3][0] * m[0][2] * m[2][3] -
    m[3][0] * m[0][3] * m[2][2];

  result[2][1] = 
   -m[0][0] * m[2][1] * m[3][3] +
    m[0][0] * m[2][3] * m[3][1] +
    m[2][0] * m[0][1] * m[3][3] -
    m[2][0] * m[0][3] * m[3][1] -
    m[3][0] * m[0][1] * m[2][3] +
    m[3][0] * m[0][3] * m[2][1];

  result[3][1] = 
    m[0][0] * m[2][1] * m[3][2] -
    m[0][0] * m[2][2] * m[3][1] -
    m[2][0] * m[0][1] * m[3][2] +
    m[2][0] * m[0][2] * m[3][1] +
    m[3][0] * m[0][1] * m[2][2] -
    m[3][0] * m[0][2] * m[2][1];

  result[0][2] = 
    m[0][1] * m[1][2] * m[3][3] -
    m[0][1] * m[1][3] * m[3][2] -
    m[1][1] * m[0][2] * m[3][3] +
    m[1][1] * m[0][3] * m[3][2] +
    m[3][1] * m[0][2] * m[1][3] -
    m[3][1] * m[0][3] * m[1][2];

  result[1][2] = 
   -m[0][0] * m[1][2] * m[3][3] +
    m[0][0] * m[1][3] * m[3][2] +
    m[1][0] * m[0][2] * m[3][3] -
    m[1][0] * m[0][3] * m[3][2] -
    m[3][0] * m[0][2] * m[1][3] +
    m[3][0] * m[0][3] * m[1][2];

  result[2][2] = 
    m[0][0] * m[1][1] * m[3][3] -
    m[0][0] * m[1][3] * m[3][1] -
    m[1][0] * m[0][1] * m[3][3] +
    m[1][0] * m[0][3] * m[3][1] +
    m[3][0] * m[0][1] * m[1][3] -
    m[3][0] * m[0][3] * m[1][1];

  result[3][2] = 
   -m[0][0] * m[1][1] * m[3][2] +
    m[0][0] * m[1][2] * m[3][1] +
    m[1][0] * m[0][1] * m[3][2] -
    m[1][0] * m[0][2] * m[3][1] -
    m[3][0] * m[0][1] * m[1][2] +
    m[3][0] * m[0][2] * m[1][1];

  result[0][3] = 
   -m[0][1] * m[1][2] * m[2][3] +
    m[0][1] * m[1][3] * m[2][2] +
    m[1][1] * m[0][2] * m[2][3] -
    m[1][1] * m[0][3] * m[2][2] -
    m[2][1] * m[0][2] * m[1][3] +
    m[2][1] * m[0][3] * m[1][2];

  result[1][3] = 
    m[0][0] * m[1][2] * m[2][3] -
    m[0][0] * m[1][3] * m[2][2] -
    m[1][0] * m[0][2] * m[2][3] +
    m[1][0] * m[0][3] * m[2][2] +
    m[2][0] * m[0][2] * m[1][3] -
    m[2][0] * m[0][3] * m[1][2];

  result[2][3] = 
   -m[0][0] * m[1][1] * m[2][3] +
    m[0][0] * m[1][3] * m[2][1] +
    m[1][0] * m[0][1] * m[2][3] -
    m[1][0] * m[0][3] * m[2][1] -
    m[2][0] * m[0][1] * m[1][3] +
    m[2][0] * m[0][3] * m[1][1];

  result[3][3] = 
    m[0][0] * m[1][1] * m[2][2] -
    m[0][0] * m[1][2] * m[2][1] -
    m[1][0] * m[0][1] * m[2][2] +
    m[1][0] * m[0][2] * m[2][1] +
    m[2][0] * m[0][1] * m[1][2] -
    m[2][0] * m[0][2] * m[1][1];

  float det = m[0][0] * result[0][0] + m[0][1] * result[1][0] + m[0][2] * result[2][0] + m[0][3] * result[3][0];

  det = 1.0f / det;

  for (int i = 0; i < 16; i++)
  {
    pResult[i] = pResult[i] * det;
  }
}

#endif // SE1_VULKAN
