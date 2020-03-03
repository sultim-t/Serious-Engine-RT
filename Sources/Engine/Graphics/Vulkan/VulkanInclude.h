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

#ifndef SE_INCL_VULKANINCLUDE_H
#define SE_INCL_VULKANINCLUDE_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#ifdef SE1_VULKAN

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <Engine/Graphics/Vulkan/SvkVertex.h>
#include <Engine/Graphics/Vulkan/SvkPipelineStates.h>
#include <Engine/Graphics/Vulkan/SvkSamplerStates.h>
#include <Engine/Templates/StaticArray.h>

#define gl_VkMaxCmdBufferCount                  2

#define SVK_PREFERRED_SWAPCHAIN_SIZE            3

#define SVK_DESCRIPTOR_MAX_SET_COUNT            8192
#define SVK_DESCRIPTOR_MAX_SAMPLER_COUNT        8192

#define SVK_VERT_START_COUNT                    4096
#define SVK_VERT_ALLOC_STEP                     4096

#define SVK_DYNAMIC_VERTEX_BUFFER_START_SIZE	  (8 * 1024 * 1024)
#define SVK_DYNAMIC_INDEX_BUFFER_START_SIZE	    (2 * 1024 * 1024)
#define SVK_DYNAMIC_UNIFORM_BUFFER_START_SIZE   (256 * 1024)
#define SVK_DYNAMIC_UNIFORM_MAX_ALLOC_SIZE      1024

#define SVK_RENDERPASS_COLOR_ATTACHMENT_INDEX   0
#define SVK_RENDERPASS_DEPTH_ATTACHMENT_INDEX   1

#define SVK_SAMPLER_LOD_BIAS                    (0.0f)

struct SvkTextureObject
{
public:
  uint32_t          sto_Width;
  uint32_t          sto_Height;
  VkFormat          sto_Format;

  VkImage           sto_Image;
  VkImageView       sto_ImageView;
  VkImageLayout     sto_Layout;
  VkDeviceMemory    sto_Memory;
  uint32_t          sto_MemoryHandle;

  SvkSamplerFlags   sto_SamplerFlags;

  // for destruction
  VkDevice          sto_VkDevice;

public:
  SvkTextureObject()
  {
    Reset();
  }

  void Reset()
  {
    sto_Width = sto_Height = 0;
    sto_Format = VK_FORMAT_UNDEFINED;
    sto_Image = VK_NULL_HANDLE;
    sto_ImageView = VK_NULL_HANDLE;
    sto_MemoryHandle = 0;
    sto_Memory = VK_NULL_HANDLE;
    sto_VkDevice = VK_NULL_HANDLE;
    sto_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
    sto_SamplerFlags = 0;
  }
};

struct SvkDynamicBuffer
{
  VkBuffer        sdb_Buffer;
  VkDeviceSize    sdb_CurrentOffset;
  void*           sdb_Data;
};

struct SvkDynamicUniform : public SvkDynamicBuffer
{
  VkDescriptorSet sdu_DescriptorSet;
};

// Dynamic buffer to delete
struct SvkDBufferToDelete
{
  VkBuffer        sdd_Memory;
  VkBuffer        sdd_Buffers[gl_VkMaxCmdBufferCount];
  // optional
  VkDescriptorSet sdd_DescriptorSets[gl_VkMaxCmdBufferCount];
};

struct SvkDynamicBufferGlobal
{
  uint32_t        sdg_CurrentDynamicBufferSize;
  VkDeviceMemory  sdg_DynamicBufferMemory;
};

struct SvkPipelineState
{
  SvkPipelineStateFlags   sps_Flags;
  VkPipeline              sps_Pipeline;
};

struct SvkSamplerObject
{
  VkDevice    sso_Device;
  VkSampler   sso_Sampler;
};

struct SvkTextureDescSet
{
  VkImageView       sds_ImageView;
  VkImageLayout     sds_Layout;
  SvkSamplerFlags   sds_SamplerFlags;
  VkDescriptorSet   sds_DescSet;
};

struct SvkVertexLayout
{
  CStaticArray<VkVertexInputBindingDescription>    svl_Bindings;
  CStaticArray<VkVertexInputAttributeDescription>  svl_Attributes;
};

void Svk_MatCopy(float *dest, const float *src);
void Svk_MatSetIdentity(float *result);
void Svk_MatMultiply(float *result, const float *a, const float *b);
void Svk_MatFrustum(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);
void Svk_MatOrtho(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);

#endif
#endif