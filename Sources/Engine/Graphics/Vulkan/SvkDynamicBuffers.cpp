#include "StdH.h"

#include <Engine/Graphics/GfxLibrary.h>

SvkBufferObject CGfxLibrary::GetVertexBuffer(const void *data, uint32_t dataSize)
{
  SvkBufferObject &result = gl_VkVertexBuffers[gl_VkCmdBufferCurrent].Push();

  CreateBuffer(dataSize,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    result.sbo_Buffer, result.sbo_Memory);

  CopyToDeviceMemory(result.sbo_Memory, data, dataSize);

  return result;
}

SvkBufferObject CGfxLibrary::GetIndexBuffer(const void *data, uint32_t dataSize)
{
  SvkBufferObject &result = gl_VkIndexBuffers[gl_VkCmdBufferCurrent].Push();

  CreateBuffer(dataSize,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    result.sbo_Buffer, result.sbo_Memory);

  CopyToDeviceMemory(result.sbo_Memory, data, dataSize);

  return result;
}

const SvkDescriptorObject &CGfxLibrary::GetUniformBuffer(const void *data, uint32_t dataSize)
{
  // create uniform buffer
  SvkBufferObject &uniformBuffer = gl_VkUniformBuffers[gl_VkCmdBufferCurrent].Push();

  CreateBuffer(dataSize,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    uniformBuffer.sbo_Buffer, uniformBuffer.sbo_Memory);

  CopyToDeviceMemory(uniformBuffer.sbo_Memory, data, dataSize);

  // TODO: sampler
  // create descriptor set
  SvkDescriptorObject &result = gl_VkDescriptors[gl_VkCmdBufferCurrent].Push();
  result.sdo_DescSet = CreateDescriptorSet();
  result.sdo_Offset = 0;

  VkDescriptorBufferInfo bufferInfo = {};
  bufferInfo.buffer = uniformBuffer.sbo_Buffer;
  bufferInfo.offset = 0;
  bufferInfo.range = dataSize;

  // TODO: textures
  //VkDescriptorImageInfo imageInfo = {};
  //imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  //imageInfo.imageView = textureImageView;
  //imageInfo.sampler = textureSampler;

  // update descriptor set
  VkWriteDescriptorSet descriptorWrites[2];

  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].pNext = nullptr;
  descriptorWrites[0].dstSet = result.sdo_DescSet;
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].dstArrayElement = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[0].descriptorCount = 1;
  descriptorWrites[0].pBufferInfo = &bufferInfo;
  descriptorWrites[0].pImageInfo = nullptr;
  descriptorWrites[0].pTexelBufferView = nullptr;

  //descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  //descriptorWrites[1].pNext = nullptr;
  //descriptorWrites[1].dstSet = gl_VkDescriptorSets[i];
  //descriptorWrites[1].dstBinding = 1;
  //descriptorWrites[1].dstArrayElement = 0;
  //descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  //descriptorWrites[1].descriptorCount = 1;
  //descriptorWrites[1].pImageInfo = &imageInfo;
  //descriptorWrites[1].pBufferInfo = nullptr;
  //descriptorWrites[1].pTexelBufferView = nullptr;

  // TODO: textures - change to 2
  vkUpdateDescriptorSets(gl_VkDevice, 1, descriptorWrites, 0, nullptr);

  return result;
}

void CGfxLibrary::InitDynamicBuffers()
{
#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkVertexBuffers[i].Count() == 0);
    ASSERT(gl_VkIndexBuffers[i].Count() == 0);
    ASSERT(gl_VkUniformBuffers[i].Count() == 0);
    ASSERT(gl_VkDescriptors[i].Count() == 0);
  }
#endif // !NDEBUG

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    // clear arrays by setting empty one
    gl_VkVertexBuffers[i] = {};
    gl_VkIndexBuffers[i] = {};
    gl_VkUniformBuffers[i] = {};
    gl_VkDescriptors[i] = {};
  }
}

void CGfxLibrary::FreeDynamicBuffers(uint32_t cmdBufferIndex)
{
  auto &verts = gl_VkVertexBuffers[cmdBufferIndex];
  auto &indices = gl_VkIndexBuffers[cmdBufferIndex];
  auto &uniforms = gl_VkUniformBuffers[cmdBufferIndex];

  for (uint32_t i = 0; i < verts.Count(); i++)
  {
    vkDestroyBuffer(gl_VkDevice, verts[i].sbo_Buffer, nullptr);
    vkFreeMemory(gl_VkDevice, verts[i].sbo_Memory, nullptr);
  }  

  for (uint32_t i = 0; i < indices.Count(); i++)
  {
    vkDestroyBuffer(gl_VkDevice, indices[i].sbo_Buffer, nullptr);
    vkFreeMemory(gl_VkDevice, indices[i].sbo_Memory, nullptr);
  }

  for (uint32_t i = 0; i < uniforms.Count(); i++)
  {
    vkDestroyBuffer(gl_VkDevice, uniforms[i].sbo_Buffer, nullptr);
    vkFreeMemory(gl_VkDevice, uniforms[i].sbo_Memory, nullptr);
  }

  verts.PopAll();
  indices.PopAll();
  uniforms.PopAll();

  if (gl_VkDescriptorPools[cmdBufferIndex] != VK_NULL_HANDLE)
  {
    // reset descriptor pool to begin new list
    vkResetDescriptorPool(gl_VkDevice, gl_VkDescriptorPools[cmdBufferIndex], 0);
  }

  gl_VkDescriptors[cmdBufferIndex].PopAll();

  // -------
  // -------
  // TODO: delete this -- VkDescriptorSets must be dynamic
  gl_VkDescriptorSets[cmdBufferIndex].Clear();
  // -------
  // -------
}