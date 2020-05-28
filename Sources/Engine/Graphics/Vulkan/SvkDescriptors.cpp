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
    gl_VkDescSetLayoutTexture,
    gl_VkDescSetLayoutTexture,
    gl_VkDescSetLayoutTexture,
    gl_VkDescSetLayoutTexture
  };

  const uint32_t pushConstCount = 2;
  VkPushConstantRange pushConsts[pushConstCount] = {};

  VkPushConstantRange &mvpPushConst = pushConsts[0];
  mvpPushConst.offset = 0;
  mvpPushConst.size = 16 * sizeof(float);
  mvpPushConst.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkPushConstantRange &colorScalePushConst = pushConsts[1];
  colorScalePushConst.offset = 16 * sizeof(float);
  colorScalePushConst.size = sizeof(float);
  colorScalePushConst.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  // create default layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 4;
  pipelineLayoutInfo.pSetLayouts = setLayouts;
  pipelineLayoutInfo.pushConstantRangeCount = pushConstCount;
  pipelineLayoutInfo.pPushConstantRanges = pushConsts;

  r = vkCreatePipelineLayout(gl_VkDevice, &pipelineLayoutInfo, nullptr, &gl_VkPipelineLayout);
  VK_CHECKERROR(r);

  // create layout for occlusion quiries
  VkPipelineLayoutCreateInfo pipelineLayoutOcclInfo = {};
  pipelineLayoutOcclInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  r = vkCreatePipelineLayout(gl_VkDevice, &pipelineLayoutOcclInfo, nullptr, &gl_VkPipelineLayoutOcclusion);
  VK_CHECKERROR(r);
}

void SvkMain::DestroyDescriptorSetLayouts()
{
  ASSERT(gl_VkDescSetLayoutTexture != VK_NULL_HANDLE);
  ASSERT(gl_VkPipelineLayout != VK_NULL_HANDLE);

  vkDestroyDescriptorSetLayout(gl_VkDevice, gl_VkDescSetLayoutTexture, nullptr);
  vkDestroyPipelineLayout(gl_VkDevice, gl_VkPipelineLayout, nullptr);
  vkDestroyPipelineLayout(gl_VkDevice, gl_VkPipelineLayoutOcclusion, nullptr);
}

void SvkMain::CreateDescriptorPools()
{
#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkTextureDescPools[i] == VK_NULL_HANDLE);
  }
#endif // !NDEBUG

  VkResult r;

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
  }
}

void SvkMain::DestroyDescriptorPools()
{
#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkTextureDescPools[i] != VK_NULL_HANDLE);
  }
#endif // !NDEBUG

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    vkDestroyDescriptorPool(gl_VkDevice, gl_VkTextureDescPools[i], nullptr);
    gl_VkTextureDescPools[i] = VK_NULL_HANDLE;
  }
}

void SvkMain::PrepareDescriptorSets(uint32_t cmdBufferIndex)
{
    VkResult r = vkResetDescriptorPool(gl_VkDevice, gl_VkTextureDescPools[cmdBufferIndex], 0);
    VK_CHECKERROR(r);
}

#endif