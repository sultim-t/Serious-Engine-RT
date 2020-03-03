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
#include <Engine/Graphics/Vulkan/SvkMain.h>

#ifdef SE1_VULKAN

void SvkMain::CreateDescriptorSetLayouts()
{
  VkResult r;

  VkDescriptorSetLayoutBinding uniformBinding = {};
  uniformBinding.binding = 0;
  uniformBinding.descriptorCount = 1;
  uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  uniformBinding.pImmutableSamplers = nullptr;
  uniformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo uniformLayoutInfo = {};
  uniformLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  uniformLayoutInfo.bindingCount = 1;
  uniformLayoutInfo.pBindings = &uniformBinding;

  r = vkCreateDescriptorSetLayout(gl_VkDevice, &uniformLayoutInfo, nullptr, &gl_VkDescriptorSetLayout);
  VK_CHECKERROR(r);

  VkDescriptorSetLayoutBinding samplerBinding = {};
  samplerBinding.binding = 0;
  samplerBinding.descriptorCount = 1;
  samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.pImmutableSamplers = nullptr;
  samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo samplerLayoutInfo = {};
  samplerLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  samplerLayoutInfo.bindingCount = 1;
  samplerLayoutInfo.pBindings = &samplerBinding;

  r = vkCreateDescriptorSetLayout(gl_VkDevice, &samplerLayoutInfo, nullptr, &gl_VkDescSetLayoutTexture);
  VK_CHECKERROR(r);

  VkDescriptorSetLayout setLayouts[] = 
  { 
    gl_VkDescriptorSetLayout, 
    gl_VkDescSetLayoutTexture,
    gl_VkDescSetLayoutTexture,
    gl_VkDescSetLayoutTexture,
    gl_VkDescSetLayoutTexture
  };

  VkPushConstantRange colorScalePushConstant = {};
  colorScalePushConstant.offset = 0;
  colorScalePushConstant.size = sizeof(float);
  colorScalePushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 5;
  pipelineLayoutInfo.pSetLayouts = setLayouts;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &colorScalePushConstant;

  r = vkCreatePipelineLayout(gl_VkDevice, &pipelineLayoutInfo, nullptr, &gl_VkPipelineLayout);
  VK_CHECKERROR(r);
}

void SvkMain::DestroyDescriptorSetLayouts()
{
  ASSERT(gl_VkDescriptorSetLayout != VK_NULL_HANDLE);
  ASSERT(gl_VkDescSetLayoutTexture != VK_NULL_HANDLE);
  ASSERT(gl_VkPipelineLayout != VK_NULL_HANDLE);

  vkDestroyDescriptorSetLayout(gl_VkDevice, gl_VkDescriptorSetLayout, nullptr);
  vkDestroyDescriptorSetLayout(gl_VkDevice, gl_VkDescSetLayoutTexture, nullptr);
  vkDestroyPipelineLayout(gl_VkDevice, gl_VkPipelineLayout, nullptr);

  gl_VkDescriptorSetLayout = VK_NULL_HANDLE;
  gl_VkDescSetLayoutTexture = VK_NULL_HANDLE;
  gl_VkPipelineLayout = VK_NULL_HANDLE;
}

void SvkMain::CreateDescriptorPools()
{
#ifndef NDEBUG
  ASSERT(gl_VkUniformDescPool == VK_NULL_HANDLE);
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkTextureDescPools[i] == VK_NULL_HANDLE);
  }
#endif // !NDEBUG

  VkResult r;


  // one dynamic buffer per command buffer
  VkDescriptorPoolSize unPoolSize;
  unPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  unPoolSize.descriptorCount = gl_VkMaxCmdBufferCount;

  VkDescriptorPoolCreateInfo unDescPoolInfo = {};
  unDescPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  // to deallocate unused desc sets in dynamic buffers
  unDescPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  unDescPoolInfo.poolSizeCount = 1;
  unDescPoolInfo.pPoolSizes = &unPoolSize;
  unDescPoolInfo.maxSets = SVK_DESCRIPTOR_MAX_SET_COUNT;

  r = vkCreateDescriptorPool(gl_VkDevice, &unDescPoolInfo, nullptr, &gl_VkUniformDescPool);
  VK_CHECKERROR(r);


  // create one pool per command buffer to prevent descriptor set update collisions
  VkDescriptorPoolSize smPoolSize;
  smPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  smPoolSize.descriptorCount = SVK_DESCRIPTOR_MAX_SAMPLER_COUNT;

  VkDescriptorPoolCreateInfo smDescPoolInfo = {};
  smDescPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  // no flags are set to make set allocation faster
  smDescPoolInfo.flags = 0;
  smDescPoolInfo.poolSizeCount = 1;
  smDescPoolInfo.pPoolSizes = &smPoolSize;
  smDescPoolInfo.maxSets = SVK_DESCRIPTOR_MAX_SET_COUNT;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    r = vkCreateDescriptorPool(gl_VkDevice, &smDescPoolInfo, nullptr, &gl_VkTextureDescPools[i]);
    VK_CHECKERROR(r);

    // and allocate desc set hash tables
    gl_VkTextureDescSets[i] = new SvkStaticHashTable<SvkTextureDescSet>();
    gl_VkTextureDescSets[i]->New(16, 256);
  }
}

void SvkMain::DestroyDescriptorPools()
{
#ifndef NDEBUG
  ASSERT(gl_VkUniformDescPool != VK_NULL_HANDLE);
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkTextureDescPools[i] != VK_NULL_HANDLE);
  }
#endif // !NDEBUG

  vkDestroyDescriptorPool(gl_VkDevice, gl_VkUniformDescPool, nullptr);
  gl_VkUniformDescPool = VK_NULL_HANDLE;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    vkDestroyDescriptorPool(gl_VkDevice, gl_VkTextureDescPools[i], nullptr);
    gl_VkTextureDescPools[i] = VK_NULL_HANDLE;

    delete gl_VkTextureDescSets[i];
    gl_VkTextureDescSets[i] = VK_NULL_HANDLE;
  }
}

void SvkMain::PrepareDescriptorSets(uint32_t cmdBufferIndex)
{
  //if (gl_VkTextureDescSets[cmdBufferIndex]->Count() > SVK_DESCRIPTOR_MAX_SET_COUNT * 2.0f / 3.0f)
  //{
  //  gl_VkTextureDescSets[cmdBufferIndex]->Clear();

  //  VkResult r = vkResetDescriptorPool(gl_VkDevice, gl_VkTextureDescPools[cmdBufferIndex], 0);
  //  VK_CHECKERROR(r);
  //}
}

#endif