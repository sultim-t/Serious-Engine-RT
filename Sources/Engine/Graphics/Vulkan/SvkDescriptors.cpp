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
  // TODO: textures
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
