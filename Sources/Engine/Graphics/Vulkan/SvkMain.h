#pragma once

#ifndef SE_INCL_SVKMAIN_H
#define SE_INCL_SVKMAIN_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#ifdef SE1_VULKAN

#include <Engine/Base/Timer.h>
#include <Engine/Base/CTString.h>
#include <Engine/Base/Lists.h>
#include <Engine/Math/Functions.h>
#include <Engine/Graphics/Adapter.h>

#include <Engine/Graphics/Color.h>
#include <Engine/Graphics/Vertex.h>
#include <Engine/Templates/StaticStackArray.cpp>

#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/Vulkan/VulkanInclude.h>
#include <Engine/Graphics/Vulkan/SvkStaticHashTable.h>
#include <Engine/Graphics/Vulkan/SvkMemoryPool.h>

#ifndef NDEBUG
extern void Vk_CheckError(VkResult r);
#define VK_CHECKERROR(r)   Vk_CheckError(r);
#else
#define VK_CHECKERROR(r)   (void)(0);
#endif

class SvkMain
{
public:
  VkInstance                      gl_VkInstance;
  VkDevice                        gl_VkDevice;
  VkSurfaceKHR                    gl_VkSurface;

  VkSwapchainKHR                  gl_VkSwapchain;
  VkExtent2D                      gl_VkSwapChainExtent;
  uint32_t                        gl_VkCurrentImageIndex;
  VkFormat                        gl_VkSurfColorFormat;
  VkColorSpaceKHR                 gl_VkSurfColorSpace;
  VkFormat                        gl_VkSurfDepthFormat;
  VkPresentModeKHR                gl_VkSurfPresentMode;
  CStaticArray<VkImage>           gl_VkSwapchainImages;
  CStaticArray<VkImageView>       gl_VkSwapchainImageViews;
  CStaticArray<VkImage>           gl_VkSwapchainDepthImages;
  CStaticArray<VkDeviceMemory>    gl_VkSwapchainDepthMemory;
  CStaticArray<VkImageView>       gl_VkSwapchainDepthImageViews;
  CStaticArray<VkFramebuffer>     gl_VkFramebuffers;

  VkSemaphore                     gl_VkImageAvailableSemaphores[gl_VkMaxCmdBufferCount];
  VkSemaphore                     gl_VkRenderFinishedSemaphores[gl_VkMaxCmdBufferCount];
  VkFence                         gl_VkCmdFences[gl_VkMaxCmdBufferCount];

  VkRenderPass                    gl_VkRenderPass;

  VkDescriptorSetLayout           gl_VkDescriptorSetLayout;
  VkDescriptorSetLayout           gl_VkDescSetLayoutTexture;
  VkPipelineLayout                gl_VkPipelineLayout;

  VkShaderModule                  gl_VkShaderModuleVert;
  VkShaderModule                  gl_VkShaderModuleFrag;
  VkShaderModule                  gl_VkShaderModuleFragAlpha;

  VkRect2D                        gl_VkCurrentScissor;
  VkViewport                      gl_VkCurrentViewport;

  uint32_t                        gl_VkCmdBufferCurrent;
  VkCommandPool                   gl_VkCmdPool;
  VkCommandBuffer                 gl_VkCmdBuffers[gl_VkMaxCmdBufferCount];
  bool                            gl_VkCmdIsRecording;

  VkDescriptorPool                        gl_VkUniformDescPool;

  SvkDynamicBufferGlobal                  gl_VkDynamicVBGlobal;
  SvkDynamicBuffer                        gl_VkDynamicVB[gl_VkMaxCmdBufferCount];

  SvkDynamicBufferGlobal                  gl_VkDynamicIBGlobal;
  SvkDynamicBuffer                        gl_VkDynamicIB[gl_VkMaxCmdBufferCount];

  SvkDynamicBufferGlobal                  gl_VkDynamicUBGlobal;
  SvkDynamicUniform                       gl_VkDynamicUB[gl_VkMaxCmdBufferCount];

  // dynamic buffers to delete
  CStaticStackArray<SvkDBufferToDelete>   gl_VkDynamicToDelete[gl_VkMaxCmdBufferCount];

  SvkSamplerFlags                         gl_VkGlobalSamplerState;
  SvkStaticHashTable<SvkSamplerObject>    gl_VkSamplers;

  // all loaded textures
  SvkStaticHashTable<SvkTextureObject>    gl_VkTextures;
  uint32_t                                gl_VkLastTextureId;
  SvkMemoryPool *gl_VkImageMemPool;

  VkDescriptorPool                        gl_VkTextureDescPools[gl_VkMaxCmdBufferCount];
  SvkStaticHashTable<SvkTextureDescSet> *gl_VkTextureDescSets[gl_VkMaxCmdBufferCount];


  CStaticStackArray<uint32_t>             gl_VkTexturesToDelete[gl_VkMaxCmdBufferCount];

  // pointers to currently active textures
  uint32_t                                gl_VkActiveTextures[GFX_MAXTEXUNITS];

  SvkPipelineStateFlags                   gl_VkGlobalState;
  SvkPipelineState *gl_VkPreviousPipeline;
  CStaticStackArray<SvkPipelineState>     gl_VkPipelines;
  VkPipelineCache                         gl_VkPipelineCache;
  SvkVertexLayout *gl_VkDefaultVertexLayout;

  VkPhysicalDevice                        gl_VkPhysDevice;
  VkPhysicalDeviceMemoryProperties        gl_VkPhMemoryProperties;
  VkPhysicalDeviceProperties              gl_VkPhProperties;
  VkPhysicalDeviceFeatures                gl_VkPhFeatures;
  VkSurfaceCapabilitiesKHR                gl_VkPhSurfCapabilities;
  CStaticArray<VkSurfaceFormatKHR>        gl_VkPhSurfFormats;
  CStaticArray<VkPresentModeKHR>          gl_VkPhSurfPresentModes;

  CStaticArray<const char *>              gl_VkPhysDeviceExtensions;
  CStaticArray<const char *>              gl_VkLayers;

  uint32_t                        gl_VkQueueFamGraphics;
  uint32_t                        gl_VkQueueFamTransfer;
  uint32_t                        gl_VkQueueFamPresent;
  VkQueue                         gl_VkQueueGraphics;
  VkQueue                         gl_VkQueueTransfer;
  VkQueue                         gl_VkQueuePresent;

  VkDebugUtilsMessengerEXT        gl_VkDebugMessenger;

  // current mesh
  CStaticStackArray<SvkVertex>    gl_VkVerts;

public:
  SvkMain();

  // Vulkan specific
  BOOL InitDriver_Vulkan();
  void EndDriver_Vulkan();
  void Reset_Vulkan();
  //BOOL InitDisplay_Vulkan(INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ, enum DisplayDepth eColorDepth);
  void InitContext_Vulkan();
  BOOL SetCurrentViewport_Vulkan(CViewPort *pvp);
  void SwapBuffers_Vulkan();
  void SetViewport_Vulkan(float leftUpperX, float leftUpperY, float width, float height, float minDepth, float maxDepth);

  BOOL PickPhysicalDevice();
  BOOL InitSurface_Win32(HINSTANCE hinstance, HWND hwnd);
  BOOL CreateDevice();
  void CreateRenderPass();

  void CreateSyncPrimitives();
  void DestroySyncPrimitives();

  void CreateVertexLayouts();
  void DestroyVertexLayouts();

  // create desc set layout and its pipeline layout
  void CreateDescriptorSetLayouts();
  void DestroyDescriptorSetLayouts();

  void CreateShaderModules();
  void DestroyShaderModules();

  void CreateDescriptorPools();
  void DestroyDescriptorPools();
  void PrepareDescriptorSets(uint32_t cmdBufferIndex);

  SvkPipelineState &GetPipeline(SvkPipelineStateFlags flags);
  // create new pipeline and add it to list
  SvkPipelineState &CreatePipeline(SvkPipelineStateFlags flags, const SvkVertexLayout &vertLayout,
    VkShaderModule vertShader, VkShaderModule fragShader);
  void CreatePipelineCache();
  void DestroyPipelines();

  BOOL CreateSwapchainDepth(uint32_t width, uint32_t height, uint32_t imageIndex);

  void CreateCmdBuffers();
  void DestroyCmdBuffers();

  void InitDynamicBuffers();
  void InitDynamicVertexBuffers(uint32_t newSize);
  void InitDynamicIndexBuffers(uint32_t newSize);
  void InitDynamicUniformBuffers(uint32_t newSize);
  void InitDynamicBuffer(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicBuffer *buffers, VkBufferUsageFlags usage);

  void ClearCurrentDynamicOffsets(uint32_t cmdBufferIndex);
  void GetVertexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer);
  void GetIndexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer);
  void GetUniformBuffer(uint32_t size, SvkDynamicUniform &outDynUniform);
  void FlushDynamicBuffersMemory();

  void AddDynamicBufferToDeletion(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicBuffer *buffers);
  void AddDynamicUniformToDeletion(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicUniform *buffers);
  // free frame data: vertex, index, uniform buffers, descriptor sets
  void FreeUnusedDynamicBuffers(uint32_t cmdBufferIndex);
  // destroy all dynamic buffers data, including unused
  void DestroyDynamicBuffers();

  VkSampler GetSampler(SvkSamplerFlags flags);
  VkSampler CreateSampler(SvkSamplerFlags flags);

  void InitSamplers();
  void DestroySamplers();

  void CreateTexturesDataStructure();
  void DestroyTexturesDataStructure();

  SvkTextureObject &GetTextureObject(uint32_t textureId);
  VkDescriptorSet GetTextureDescriptor(uint32_t textureId);
  void FreeDeletedTextures(uint32_t cmdBufferIndex);
  static void DestroyTextureObject(SvkTextureObject &sto);

  void AcquireNextImage();
  void StartFrame();
  void EndFrame();

  VkShaderModule CreateShaderModule(const uint32_t *spvCode, uint32_t codeSize);

  // utils
  BOOL GetQueues(VkPhysicalDevice physDevice,
    uint32_t &graphicsQueueFamily, uint32_t &transferQueueFamily, uint32_t &presentQueueFamily);
  BOOL CheckDeviceExtensions(VkPhysicalDevice physDevice, const CStaticArray<const char *> &requiredExtensions);
  VkExtent2D GetSwapchainExtent(uint32_t width, uint32_t height);
  uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask);
  uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask, VkFlags preferredMask);
  VkFormat FindSupportedFormat(const VkFormat *formats, uint32_t formatCount, VkImageTiling tiling, VkFormatFeatureFlags features);
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
  void CopyToDeviceMemory(VkDeviceMemory deviceMemory, const void *data, VkDeviceSize size);

public:
  void CreateSwapchain(uint32_t width, uint32_t height);
  void RecreateSwapchain(uint32_t newWidth, uint32_t newHeight);
  void DestroySwapchain();


  // get current global pipeline state
  SvkPipelineStateFlags &GetPipelineState();

  void UpdateViewportDepth(float minDepth, float maxDepth);

  // Get current started cmd buffer to write in
  VkCommandBuffer GetCurrentCmdBuffer();
  void DrawTriangles(uint32_t indexCount, const uint32_t *indices);


  void SetTexture(uint32_t textureUnit, uint32_t textureId, SvkSamplerFlags samplerFlags);
  // create texture handler, texture IDs starts with 1, not 0
  uint32_t CreateTexture();
  // init texture; if onlyUpdate is true, texture will not be allocated
  void InitTexture32Bit(
    uint32_t &textureId, VkFormat format, void *textureData,
    VkExtent2D *mipLevels, uint32_t mipLevelsCount, bool onlyUpdate);
  // delete texture
  void AddTextureToDeletion(uint32_t textureId);
  // for statistics
  uint32_t GetTexturePixCount(uint32_t textureId);


  void ClearColor(int32_t x, int32_t y, uint32_t width, uint32_t height, float *rgba);
  void ClearDepth(int32_t x, int32_t y, uint32_t width, uint32_t height, float depth);
  void ClearColor(float *rgba);
  void ClearDepth(float depth);
};

#endif
#endif