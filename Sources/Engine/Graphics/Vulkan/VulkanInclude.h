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

#define SVK_DYNAMIC_VERTEX_BUFFER_START_SIZE	  (8192 * 1024)
#define SVK_DYNAMIC_INDEX_BUFFER_START_SIZE	    (1024 * 1024)
#define SVK_DYNAMIC_UNIFORM_BUFFER_START_SIZE   (256 * 1024)
#define SVK_DYNAMIC_UNIFORM_MAX_ALLOC_SIZE      1024

#define SVK_RENDERPASS_COLOR_ATTACHMENT_INDEX   0
#define SVK_RENDERPASS_DEPTH_ATTACHMENT_INDEX   1

#define SVK_SAMPLER_LOD_BIAS                    (0.0f)

struct SvkTextureObject
{
public:
  uint32_t        sto_Width;
  uint32_t        sto_Height;
  VkFormat        sto_Format;
  VkImage         sto_Image;
  VkImageView     sto_ImageView;
  SvkSamplerFlags sto_SamplerFlags;
  VkSampler       sto_Sampler;
  VkImageLayout   sto_Layout;
  VkDeviceMemory  sto_Memory;
  VkDescriptorSet sto_DescSet;

private:
  // sampler flags when desc set was created;
  // required to determine if desc set must be recreated,
  // as other texture object params will not be changed
  SvkSamplerFlags sto_DescSetSamplerFlags;

public:
  void Destroy(VkDevice device, VkDescriptorPool pool)
  {
    // destroy everything except sampler, as texture object doesn't own it
    if (sto_DescSet != VK_NULL_HANDLE)
    {
      vkFreeDescriptorSets(device, pool, 1, &sto_DescSet);
    }

    vkDestroyImage(device, sto_Image, nullptr);
    vkDestroyImageView(device, sto_ImageView, nullptr);
    vkFreeMemory(device, sto_Memory, nullptr);

    Reset();
  }

  void Reset()
  {
    sto_Width = sto_Height = 0;
    sto_Format = VK_FORMAT_UNDEFINED;
    sto_Image = VK_NULL_HANDLE;
    sto_ImageView = VK_NULL_HANDLE;
    sto_Sampler = VK_NULL_HANDLE;
    sto_Memory = VK_NULL_HANDLE;
    sto_DescSet = VK_NULL_HANDLE;
    sto_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
    // different values so for first time set will be marked as outdated
    sto_SamplerFlags = 0;
    sto_DescSetSamplerFlags = UINT32_MAX;
  }

  bool IsDescSetOutdated()
  {
    return sto_DescSetSamplerFlags != sto_SamplerFlags;
  }

  void SetDescriptorSet(const VkDescriptorSet &newDescSet)
  {
    sto_DescSet = newDescSet;
    sto_DescSetSamplerFlags = sto_SamplerFlags;
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

struct SvkVertexLayout
{
  CStaticArray<VkVertexInputBindingDescription>    svl_Bindings;
  CStaticArray<VkVertexInputAttributeDescription>  svl_Attributes;
};

struct SvkSampler
{
  SvkSamplerFlags   sv_Flags;
  VkSampler         sv_Sampler;
};

struct SvkActivatableTexture
{
  uint32_t  sat_TextureID;
  bool      sat_IsActivated;
};

void Svk_MatCopy(float *dest, const float *src);
void Svk_MatSetIdentity(float *result);
void Svk_MatMultiply(float *result, const float *a, const float *b);
void Svk_MatFrustum(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);
void Svk_MatOrtho(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);

#endif
#endif