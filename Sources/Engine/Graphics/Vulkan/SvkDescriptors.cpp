#include "stdh.h"
#include <Engine/Graphics/GfxLibrary.h>

void CGfxLibrary::CreateDescriptorSetLayouts()
{
  VkResult r;

  VkDescriptorSetLayoutBinding uniformBinding = {};
  uniformBinding.binding = 0;
  uniformBinding.descriptorCount = 1;
  uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  uniformBinding.pImmutableSamplers = nullptr;
  uniformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerBinding = {};
  samplerBinding.binding = 1;
  samplerBinding.descriptorCount = 1;
  samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.pImmutableSamplers = nullptr;
  samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding bindings[2] = { uniformBinding, samplerBinding };
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  // TODO: Vulkan: textures
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = bindings;

  r = vkCreateDescriptorSetLayout(gl_VkDevice, &layoutInfo, nullptr, &gl_VkDescriptorSetLayout);
  VK_CHECKERROR(r);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &gl_VkDescriptorSetLayout;

  r = vkCreatePipelineLayout(gl_VkDevice, &pipelineLayoutInfo, nullptr, &gl_VkPipelineLayout);
  VK_CHECKERROR(r);
}

void CGfxLibrary::DestroyDescriptorSetLayouts()
{
  ASSERT(gl_VkDescriptorSetLayout != VK_NULL_HANDLE);
  ASSERT(gl_VkPipelineLayout != VK_NULL_HANDLE);

  vkDestroyDescriptorSetLayout(gl_VkDevice, gl_VkDescriptorSetLayout, nullptr);
  vkDestroyPipelineLayout(gl_VkDevice, gl_VkPipelineLayout, nullptr);

  gl_VkDescriptorSetLayout = VK_NULL_HANDLE;
  gl_VkPipelineLayout = VK_NULL_HANDLE;
}

void CGfxLibrary::CreateDescriptorPools()
{
  ASSERT(gl_VkDescriptorPool == VK_NULL_HANDLE);

  VkResult r;

  const uint32_t poolSizeCount = 2;
  VkDescriptorPoolSize poolSizes[poolSizeCount];

  // dynamic buffers count same as cmd buffers
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  poolSizes[0].descriptorCount = gl_VkMaxCmdBufferCount;

  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = SVK_DESC_MAX_SAMPLER_COUNT;

  VkDescriptorPoolCreateInfo descPoolInfo = {};
  descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  // to deallocate unused desc sets in dynamic buffers
  descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descPoolInfo.poolSizeCount = poolSizeCount;
  descPoolInfo.pPoolSizes = poolSizes;
  descPoolInfo.maxSets = SVK_DESC_MAX_SET_COUNT;

  r = vkCreateDescriptorPool(gl_VkDevice, &descPoolInfo, nullptr, &gl_VkDescriptorPool);
  VK_CHECKERROR(r);
}

void CGfxLibrary::DestroyDescriptorPools()
{
  ASSERT(gl_VkDescriptorPool != VK_NULL_HANDLE);

  vkDestroyDescriptorPool(gl_VkDevice, gl_VkDescriptorPool, nullptr);
  gl_VkDescriptorPool = VK_NULL_HANDLE;
}

VkSampler CGfxLibrary::GetSampler(SvkSamplerFlags flags)
{
  for (INDEX i = 0; i < gl_VkSamplers.Count(); i++)
  {
    if (gl_VkSamplers[i].sv_Flags == flags)
    {
      return gl_VkSamplers[i].sv_Sampler;
    }
  }

  return CreateSampler(flags);
}

VkSampler CGfxLibrary::CreateSampler(SvkSamplerFlags flags)
{
  SvkSampler &ss = gl_VkSamplers.Push();
  ss.sv_Flags = flags;

  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.mipLodBias = SVK_SAMPLER_LOD_BIAS;

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
  case SVK_TSS_WRAP_U_CLAMP: samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag");
  }

  switch (flags & SVK_TSS_WRAP_V_BITS)
  {
  case SVK_TSS_WRAP_V_REPEAT: samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; break;
  case SVK_TSS_WRAP_V_CLAMP: samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag");
  }

  samplerInfo.anisotropyEnable = (flags & SVK_TSS_ANISOTROPY_BITS) != 0;

  switch (flags & SVK_TSS_ANISOTROPY_BITS)
  {
  case SVK_TSS_ANISOTROPY_0: samplerInfo.maxAnisotropy = 0; break;
  case SVK_TSS_ANISOTROPY_2: samplerInfo.maxAnisotropy = 2; break;
  case SVK_TSS_ANISOTROPY_4: samplerInfo.maxAnisotropy = 4; break;
  case SVK_TSS_ANISOTROPY_8: samplerInfo.maxAnisotropy = 8; break;
  case SVK_TSS_ANISOTROPY_16: samplerInfo.maxAnisotropy = 16; break;
  }

  if (samplerInfo.maxAnisotropy > gl_VkPhProperties.limits.maxSamplerAnisotropy)
  {
    samplerInfo.maxAnisotropy = gl_VkPhProperties.limits.maxSamplerAnisotropy;
  }

  VkResult r = vkCreateSampler(gl_VkDevice, &samplerInfo, nullptr, &ss.sv_Sampler);
  VK_CHECKERROR(r);
}

void CGfxLibrary::DestroySamplers()
{
  for (INDEX i = 0; i < gl_VkSamplers.Count; i++)
  {
    vkDestroySampler(gl_VkDevice, gl_VkSamplers[i].sv_Sampler, nullptr);
  }

  gl_VkSamplers.Clear();
}

void CGfxLibrary::SetTexture(uint32_t textureUnit, uint32_t textureId, SvkSamplerFlags samplerFlags)
{
  ASSERT(textureUnit >= 0 && textureUnit < GFX_MAXTEXUNITS);

  // set new sampler
  gl_VkTextures[textureId].sto_Sampler = GetSampler(samplerFlags);
  
  // WHEN DRAWING, SET TEXTURES WITH 'ISACTIVATED'

  // TODO: Vlkan: init this values with false in REservulkan()
  gl_VkActiveTextures[textureUnit].sat_IsActivated = true;
  gl_VkActiveTextures[textureUnit].sat_TextureID = textureId;
}
