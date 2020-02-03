#include "stdh.h"

#include <Engine/Graphics/GfxLibrary.h>

#include <Engine/Base/Statistics_internal.h>
#include <Engine/Math/Functions.h>
#include <Engine/Graphics/Color.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/GfxProfile.h>

#include <Engine/Base/ListIterator.inl>

static CTexParams *_tpCurrent;
extern INDEX GFX_iActiveTexUnit;

static SvkSamplerFlags UnpackFilter_Vulkan(INDEX iFilter)
{
  SvkSamplerFlags flags = 0;

  switch (iFilter)
  {
  case 110:  case 10:
  case 111:  case 11:  flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_FILTER_MIPMAP_NEAREST; break;
  case 112:  case 12:  flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_FILTER_MIPMAP_LINEAR; break;

  case 220:  case 20:
  case 221:  case 21:  flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_FILTER_MIPMAP_NEAREST; break;
  case 222:  case 22:  flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_FILTER_MIPMAP_LINEAR; break;

  case 120:
  case 121:            flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_FILTER_MIPMAP_NEAREST; break;
  case 122:            flags |= SVK_TSS_FILTER_MAG_NEAREST | SVK_TSS_FILTER_MIN_LINEAR | SVK_TSS_FILTER_MIPMAP_LINEAR; break;

  case 210:
  case 211:            flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_FILTER_MIPMAP_NEAREST; break;
  case 212:            flags |= SVK_TSS_FILTER_MAG_LINEAR | SVK_TSS_FILTER_MIN_NEAREST | SVK_TSS_FILTER_MIPMAP_LINEAR; break;
  default: ASSERTALWAYS("Illegal Vulkan texture filtering mode."); break;
  }

  return flags;
}

/*static void UnpackFilter_Vulkan(INDEX iFilter, VkFilter &eMagFilter, VkFilter &eMinFilter, VkSamplerMipmapMode &eMipMapMode)
{
  switch (iFilter) 
  {
  case 110:  case 10:
  case 111:  case 11:  eMagFilter = VK_FILTER_NEAREST;  eMinFilter = VK_FILTER_NEAREST; eMipMapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; break;
  case 112:  case 12:  eMagFilter = VK_FILTER_NEAREST;  eMinFilter = VK_FILTER_NEAREST; eMipMapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;  break;

  case 220:  case 20:
  case 221:  case 21:  eMagFilter = VK_FILTER_LINEAR;   eMinFilter = VK_FILTER_LINEAR;  eMipMapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; break;
  case 222:  case 22:  eMagFilter = VK_FILTER_LINEAR;   eMinFilter = VK_FILTER_LINEAR;  eMipMapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;  break;

  case 120:
  case 121:            eMagFilter = VK_FILTER_NEAREST;  eMinFilter = VK_FILTER_LINEAR;  eMipMapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; break;
  case 122:            eMagFilter = VK_FILTER_NEAREST;  eMinFilter = VK_FILTER_LINEAR;  eMipMapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;  break;

  case 210:
  case 211:            eMagFilter = VK_FILTER_LINEAR;   eMinFilter = VK_FILTER_NEAREST; eMipMapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; break;
  case 212:            eMagFilter = VK_FILTER_LINEAR;   eMinFilter = VK_FILTER_NEAREST; eMipMapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;  break;
  default: ASSERTALWAYS("Illegal Vulkan texture filtering mode."); break;
  }
}*/

SvkSamplerFlags MimicTexParams_Vulkan(CTexParams &tpLocal)
{
  ASSERT(&tpLocal != NULL);

  SvkSamplerFlags flags = 0; // _pGfx->gl_VkGlobalSamplerState;

  if (tpLocal.tp_iFilter != _tpGlobal[0].tp_iFilter)
  {
    tpLocal.tp_iFilter = _tpGlobal[0].tp_iFilter;

    //flags &= ~SVK_TSS_FILTER_MAG_BITS;
    //flags &= ~SVK_TSS_FILTER_MIN_BITS;
    //flags &= ~SVK_TSS_MIPMAP_BITS;

    flags |= UnpackFilter_Vulkan(tpLocal.tp_iFilter);
  };

  if (tpLocal.tp_iAnisotropy != _tpGlobal[0].tp_iAnisotropy)
  {
    tpLocal.tp_iAnisotropy = _tpGlobal[0].tp_iAnisotropy;

    //flags &= ~SVK_TSS_ANISOTROPY_BITS;

    if (tpLocal.tp_iAnisotropy > 8)       { flags |= SVK_TSS_ANISOTROPY_16; }
    else if (tpLocal.tp_iAnisotropy > 4)  { flags |= SVK_TSS_ANISOTROPY_8; }
    else if (tpLocal.tp_iAnisotropy > 2)  { flags |= SVK_TSS_ANISOTROPY_4; }
    else if (tpLocal.tp_iAnisotropy > 1)  { flags |= SVK_TSS_ANISOTROPY_2; }
    else                                  { flags |= SVK_TSS_ANISOTROPY_0; }
  }

  if (tpLocal.tp_eWrapU != _tpGlobal[GFX_iActiveTexUnit].tp_eWrapU || tpLocal.tp_eWrapV != _tpGlobal[GFX_iActiveTexUnit].tp_eWrapV)
  {
    tpLocal.tp_eWrapU = _tpGlobal[GFX_iActiveTexUnit].tp_eWrapU;
    tpLocal.tp_eWrapV = _tpGlobal[GFX_iActiveTexUnit].tp_eWrapV;

    //flags &= ~SVK_TSS_WRAP_U_BITS;
    //flags &= ~SVK_TSS_WRAP_V_BITS;

    if (tpLocal.tp_eWrapU == GFX_REPEAT) { flags |= SVK_TSS_WRAP_U_REPEAT; }
    else { flags |= SVK_TSS_WRAP_U_CLAMP; }

    if (tpLocal.tp_eWrapV == GFX_REPEAT) { flags |= SVK_TSS_WRAP_V_REPEAT; }
    else { flags |= SVK_TSS_WRAP_V_CLAMP; }
  }

  _tpCurrent = &tpLocal;

  return flags;
}

void UploadTexture_Vulkan(SvkTextureObject *pTexture, ULONG *pulTexture, PIX pixSizeU, PIX pixSizeV, VkFormat eInternalFormat, BOOL bDiscard)
{
}