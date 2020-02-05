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
#include <Engine/Graphics/GfxLibrary.h>

#ifdef SE1_VULKAN

VkSampler CGfxLibrary::GetSampler(SvkSamplerFlags flags)
{
  // TODO: Vulkan: more optimal structure for sampler list
  for (INDEX i = 0; i < gl_VkSamplers.Count(); i++)
  {
    if (gl_VkSamplers[i].sv_Flags == flags)
    {
      return gl_VkSamplers[i].sv_Sampler;
    }
  }

  // create sampler and add it to list
  return CreateSampler(flags);
}

VkSampler CGfxLibrary::CreateSampler(SvkSamplerFlags flags)
{
  SvkSampler &ss = gl_VkSamplers.Push();
  ss.sv_Flags = flags;

  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.mipLodBias = SVK_SAMPLER_LOD_BIAS;
  samplerInfo.minLod = 0;
  samplerInfo.maxLod = FLT_MAX;

  switch (flags & SVK_TSS_FILTER_MIN_BITS)
  {
  case SVK_TSS_FILTER_MIN_LINEAR: samplerInfo.minFilter = VK_FILTER_LINEAR; break;
  case SVK_TSS_FILTER_MIN_NEAREST: samplerInfo.minFilter = VK_FILTER_NEAREST; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag");
  }

  switch (flags & SVK_TSS_FILTER_MAG_BITS)
  {
  case SVK_TSS_FILTER_MAG_LINEAR: samplerInfo.magFilter = VK_FILTER_LINEAR; break;
  case SVK_TSS_FILTER_MAG_NEAREST: samplerInfo.magFilter = VK_FILTER_NEAREST; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag");
  }

  switch (flags & SVK_TSS_MIPMAP_BITS)
  {
  case SVK_TSS_MIPMAP_LINEAR: samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; break;
  case SVK_TSS_MIPMAP_NEAREST: samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag");
  }

  switch (flags & SVK_TSS_WRAP_U_BITS)
  {
  case SVK_TSS_WRAP_U_REPEAT: samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; break;
  case SVK_TSS_WRAP_U_CLAMP: samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; break;
  case SVK_TSS_WRAP_U_CLAMP_EDGE: samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag");
  }

  switch (flags & SVK_TSS_WRAP_V_BITS)
  {
  case SVK_TSS_WRAP_V_REPEAT: samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; break;
  case SVK_TSS_WRAP_V_CLAMP: samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; break;
  case SVK_TSS_WRAP_V_CLAMP_EDGE: samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag");
  }

  samplerInfo.anisotropyEnable = (flags & SVK_TSS_ANISOTROPY_BITS) != 0;

  switch (flags & SVK_TSS_ANISOTROPY_BITS)
  {
  case SVK_TSS_ANISOTROPY_16: samplerInfo.maxAnisotropy = 16; break;
  case SVK_TSS_ANISOTROPY_8: samplerInfo.maxAnisotropy = 8; break;
  case SVK_TSS_ANISOTROPY_4: samplerInfo.maxAnisotropy = 4; break;
  case SVK_TSS_ANISOTROPY_2: samplerInfo.maxAnisotropy = 2; break;
  case SVK_TSS_ANISOTROPY_0: samplerInfo.maxAnisotropy = 0; break;
  }

  if (samplerInfo.maxAnisotropy > gl_VkPhProperties.limits.maxSamplerAnisotropy)
  {
    samplerInfo.maxAnisotropy = gl_VkPhProperties.limits.maxSamplerAnisotropy;
  }

  VkResult r = vkCreateSampler(gl_VkDevice, &samplerInfo, nullptr, &ss.sv_Sampler);
  VK_CHECKERROR(r);

  return ss.sv_Sampler;
}

void CGfxLibrary::DestroySamplers()
{
  for (INDEX i = 0; i < gl_VkSamplers.Count(); i++)
  {
    vkDestroySampler(gl_VkDevice, gl_VkSamplers[i].sv_Sampler, nullptr);
  }

  gl_VkSamplers.Clear();
}

#endif