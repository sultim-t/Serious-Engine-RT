#include "stdh.h"
#include <Engine/Graphics/Vulkan/VulkanInclude.h>

#ifdef SE1_VULKAN

void Svk_MatCopy(float *dest, const float *src)
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      dest[i * 4 + j] = src[i * 4 + j];
    }
  }
}

void Svk_MatSetIdentity(float *result)
{
  result[0] = 1.0f;
  result[1] = 0.0f;
  result[2] = 0.0f;
  result[3] = 0.0f;
  result[4] = 0.0f;
  result[5] = 1.0f;
  result[6] = 0.0f;
  result[7] = 0.0f;
  result[8] = 0.0f;
  result[9] = 0.0f;
  result[10] = 1.0f;
  result[11] = 0.0f;
  result[12] = 0.0f;
  result[13] = 0.0f;
  result[14] = 0.0f;
  result[15] = 1.0f;
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

#endif // SE1_VULKAN
