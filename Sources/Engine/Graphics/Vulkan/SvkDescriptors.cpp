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

void CGfxLibrary::CreateDescriptorSetLayouts()
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

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 5;
  pipelineLayoutInfo.pSetLayouts = setLayouts;

  r = vkCreatePipelineLayout(gl_VkDevice, &pipelineLayoutInfo, nullptr, &gl_VkPipelineLayout);
  VK_CHECKERROR(r);
}

void CGfxLibrary::DestroyDescriptorSetLayouts()
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
  poolSizes[1].descriptorCount = SVK_DESCRIPTOR_MAX_SAMPLER_COUNT;

  VkDescriptorPoolCreateInfo descPoolInfo = {};
  descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  // to deallocate unused desc sets in dynamic buffers
  descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descPoolInfo.poolSizeCount = poolSizeCount;
  descPoolInfo.pPoolSizes = poolSizes;
  descPoolInfo.maxSets = SVK_DESCRIPTOR_MAX_SET_COUNT;

  r = vkCreateDescriptorPool(gl_VkDevice, &descPoolInfo, nullptr, &gl_VkDescriptorPool);
  VK_CHECKERROR(r);
}

void CGfxLibrary::DestroyDescriptorPools()
{
  ASSERT(gl_VkDescriptorPool != VK_NULL_HANDLE);

  vkDestroyDescriptorPool(gl_VkDevice, gl_VkDescriptorPool, nullptr);
  gl_VkDescriptorPool = VK_NULL_HANDLE;
}

#endif