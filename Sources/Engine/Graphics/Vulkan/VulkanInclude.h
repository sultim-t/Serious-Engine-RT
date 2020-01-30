#ifndef SE_INCL_VULKANINCLUDE_H
#define SE_INCL_VULKANINCLUDE_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <Engine/Graphics/Vulkan/SvkVertex.h>

#define gl_VkMaxCmdBufferCount 2

#define gl_VkMaxDescSetCount 8192
#define gl_VkMaxDescUniformCount 8192
#define gl_VkMaxDescSamplerCount 8192

#define gl_VkMaxVertexCount 65536
#define gl_VkMaxIndexCount 196608

#define gl_VkVerts_StartCount 1024
#define gl_VkVerts_AllocationStep 1024

struct SvkTextureObject
{
  VkImage         sto_Image;
  VkImageView     sto_ImageView;
  VkSampler       sto_Sampler;
};

struct SvkBufferObject
{
  VkBuffer        sbo_Buffer;
  VkDeviceMemory  sbo_Memory;
};

struct SvkDescriptorObject
{
  VkDescriptorSet sdo_DescSet;
  uint32_t        sdo_Offset;
};

void Svk_MatCopy(float *dest, const float *src);
void Svk_MatSetIdentity(float *result);
void Svk_MatMultiply(float *result, const float *a, const float *b);
void Svk_MatFrustum(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);
void Svk_MatOrtho(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);

#endif