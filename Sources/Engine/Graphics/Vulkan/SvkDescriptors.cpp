#include "stdh.h"
#include <Engine/Graphics/GfxLibrary.h>

void CGfxLibrary::CreateDescriptorSetLayout()
{
  VkResult r;

  VkDescriptorSetLayoutBinding uniformBinding = {};
  uniformBinding.binding = 0;
  uniformBinding.descriptorCount = 1;
  uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
}

void CGfxLibrary::CreateDescriptorPools()
{
#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkDescriptorPools[i] == VK_NULL_HANDLE);
    ASSERT(gl_VkDescriptorSets[i].Count() == 0);
  }
#endif // !NDEBUG

  VkResult r;

  const uint32_t poolSizeCount = 2;
  VkDescriptorPoolSize poolSizes[poolSizeCount];
  // ------------
  // ------------
  // TODO: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
  // ------------
  // ------------
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = gl_VkMaxDescUniformCount;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = gl_VkMaxDescSamplerCount;

  VkDescriptorPoolCreateInfo descPoolInfo = {};
  descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descPoolInfo.poolSizeCount = poolSizeCount;
  descPoolInfo.pPoolSizes = poolSizes;
  descPoolInfo.maxSets = gl_VkMaxDescSetCount;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    r = vkCreateDescriptorPool(gl_VkDevice, &descPoolInfo, nullptr, &gl_VkDescriptorPools[i]);
    VK_CHECKERROR(r);

    gl_VkDescriptorSets[i].New(gl_VkMaxDescSetCount);
  }
}

void CGfxLibrary::DestroyDescriptorPools()
{
#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkDescriptorPools[i] != VK_NULL_HANDLE);
  }
#endif // !NDEBUG

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    vkDestroyDescriptorPool(gl_VkDevice, gl_VkDescriptorPools[i], nullptr);
    gl_VkDescriptorPools[i] = VK_NULL_HANDLE;

    gl_VkDescriptorSets[i].Clear();
  }
}

VkDescriptorSet CGfxLibrary::CreateDescriptorSet()
{
#ifndef NDEBUG
  ASSERT(gl_VkDescriptorPools[gl_VkCmdBufferCurrent] != VK_NULL_HANDLE);

  auto &s = gl_VkDescriptorSets[gl_VkCmdBufferCurrent];
  //ASSERT(s.Count() < gl_VkMaxDescSetCount - 1);
  if (s.Count() > gl_VkMaxDescSetCount / 2)
  {
    uint32_t c = s.Count();
  }
#endif // !NDEBUG

  VkResult r;
  VkDescriptorSet descSet = gl_VkDescriptorSets[gl_VkCmdBufferCurrent].Push();

  VkDescriptorSetAllocateInfo setAllocInfo = {};
  setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  setAllocInfo.descriptorPool = gl_VkDescriptorPools[gl_VkCmdBufferCurrent];
  setAllocInfo.descriptorSetCount = 1;
  setAllocInfo.pSetLayouts = &gl_VkDescriptorSetLayout;
  r = vkAllocateDescriptorSets(gl_VkDevice, &setAllocInfo, &descSet);
  VK_CHECKERROR(r);


  return descSet;
}
