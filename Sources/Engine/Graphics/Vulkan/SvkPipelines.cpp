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

SvkPipelineStateFlags &SvkMain::GetPipelineState()
{
  return gl_VkGlobalState;
}

SvkPipelineState &SvkMain::GetPipeline(SvkPipelineStateFlags flags)
{
  for (INDEX i = 0; i < gl_VkPipelines.Count(); i++)
  {
    if (gl_VkPipelines[i].sps_Flags == flags)
    {
      return gl_VkPipelines[i];
    }
  }

  VkShaderModule vert = gl_VkShaderModuleVert;
  VkShaderModule frag = (flags & SVK_PLS_ALPHA_ENABLE_BOOL) ? gl_VkShaderModuleFragAlpha : gl_VkShaderModuleFrag;

  // if not found, create new pipeline with specified flags
  return CreatePipeline(flags, *gl_VkDefaultVertexLayout, vert, frag);
}

void SvkMain::DestroyPipelines()
{
  for (INDEX i = 0; i < gl_VkPipelines.Count(); i++)
  {
    vkDestroyPipeline(gl_VkDevice, gl_VkPipelines[i].sps_Pipeline, nullptr);
  }

  gl_VkPipelines.Clear();
}

SvkPipelineState &SvkMain::CreatePipeline(
  SvkPipelineStateFlags flags, const SvkVertexLayout &vertLayout,
  VkShaderModule vertShaderModule, VkShaderModule fragShaderModule)
{
  SvkPipelineState &newState = gl_VkPipelines.Push();
  newState.sps_Flags = flags;

  // if dynamic depth bounds required, dynamicStatesCount will be incremented
  uint32_t dynamicStatesCount = 2;
  VkDynamicState dynamicStates[3] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_DEPTH_BOUNDS
  };

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = vertLayout.svl_Bindings.Count();
  vertexInputInfo.pVertexBindingDescriptions = &vertLayout.svl_Bindings[0];
  vertexInputInfo.vertexAttributeDescriptionCount = vertLayout.svl_Attributes.Count();
  vertexInputInfo.pVertexAttributeDescriptions = &vertLayout.svl_Attributes[0];

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = nullptr; // will be set dynamically
  viewportState.scissorCount = 1;
  viewportState.pScissors = nullptr; // will be set dynamically

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.lineWidth = 1.0f;
  rasterizer.depthBiasEnable = (flags & SVK_PLS_DEPTH_BIAS_BOOL) ? VK_TRUE : VK_FALSE;
 
  if (rasterizer.depthBiasEnable)
  {
    rasterizer.depthBiasConstantFactor = -2.0f;
    rasterizer.depthBiasSlopeFactor = -1.0f;
  }

  switch (flags & SVK_PLS_POLYGON_MODE_BITS)
  {
  case SVK_PLS_POLYGON_MODE_FILL:   rasterizer.polygonMode = VK_POLYGON_MODE_FILL;  break;
  case SVK_PLS_POLYGON_MODE_LINE:   rasterizer.polygonMode = VK_POLYGON_MODE_LINE;  break;
  case SVK_PLS_POLYGON_MODE_POINT:  rasterizer.polygonMode = VK_POLYGON_MODE_POINT; break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag"); break;
  }

  switch (flags & SVK_PLS_CULL_MODE_BITS)
  {
  case SVK_PLS_CULL_MODE_BACK:      rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;  break;
  case SVK_PLS_CULL_MODE_FRONT:     rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT; break;
  case SVK_PLS_CULL_MODE_NONE:      rasterizer.cullMode = VK_CULL_MODE_NONE;      break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag"); break;
  }

  switch (flags & SVK_PLS_FRONT_FACE_BITS)
  {
  case SVK_PLS_FRONT_FACE_COUNTER_CLOCKWISE:  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; break;
  case SVK_PLS_FRONT_FACE_CLOCKWISE:          rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;         break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag"); break;
  }

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = (flags & SVK_PLS_DEPTH_TEST_BOOL) ? VK_TRUE : VK_FALSE;
  depthStencil.depthWriteEnable = (flags & SVK_PLS_DEPTH_WRITE_BOOL) ? VK_TRUE : VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  if (depthStencil.depthWriteEnable && !depthStencil.depthTestEnable)
  {
    // depth writes are always disabled when depthTestEnable is VK_FALSE,
    // so depthTestEnable must be VK_TRUE
    depthStencil.depthTestEnable = VK_TRUE;
  }

  if (false) // (flags & SVK_PLS_DEPTH_BOUNDS_BOOL)
  {
    depthStencil.depthBoundsTestEnable = VK_TRUE;
    dynamicStatesCount++;
  }
  else
  {
    depthStencil.depthBoundsTestEnable = VK_FALSE;
  }

  switch (flags & SVK_PLS_DEPTH_COMPARE_OP_BITS)
  {
  case SVK_PLS_DEPTH_COMPARE_OP_NEVER:            depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;            break;
  case SVK_PLS_DEPTH_COMPARE_OP_LESS:             depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;             break;
  case SVK_PLS_DEPTH_COMPARE_OP_EQUAL:            depthStencil.depthCompareOp = VK_COMPARE_OP_EQUAL;            break;
  case SVK_PLS_DEPTH_COMPARE_OP_LESS_OR_EQUAL:    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;    break;
  case SVK_PLS_DEPTH_COMPARE_OP_GREATER:          depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER;          break;
  case SVK_PLS_DEPTH_COMPARE_OP_NOT_EQUAL:        depthStencil.depthCompareOp = VK_COMPARE_OP_NOT_EQUAL;        break;
  case SVK_PLS_DEPTH_COMPARE_OP_GREATER_OR_EQUAL: depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; break;
  case SVK_PLS_DEPTH_COMPARE_OP_ALWAYS:           depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;           break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag"); break;
  }

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.blendEnable = (flags & SVK_PLS_BLEND_ENABLE_BOOL) ? VK_TRUE : VK_FALSE;
  
  colorBlendAttachment.colorWriteMask = 0;
  if (flags & SVK_PLS_COLOR_WRITE_MASK_R_BIT) { colorBlendAttachment.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT; }
  if (flags & SVK_PLS_COLOR_WRITE_MASK_G_BIT) { colorBlendAttachment.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT; }
  if (flags & SVK_PLS_COLOR_WRITE_MASK_B_BIT) { colorBlendAttachment.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT; }
  if (flags & SVK_PLS_COLOR_WRITE_MASK_A_BIT) { colorBlendAttachment.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT; }

  VkBlendFactor srcFactor, dstFactor;
  VkBlendOp blendOp;

  switch (flags & SVK_PLS_SRC_BLEND_FACTOR_BITS)
  {
  case SVK_PLS_SRC_BLEND_FACTOR_ZERO:                 srcFactor = VK_BLEND_FACTOR_ZERO;                 break;
  case SVK_PLS_SRC_BLEND_FACTOR_ONE:                  srcFactor = VK_BLEND_FACTOR_ONE;                  break;
  case SVK_PLS_SRC_BLEND_FACTOR_SRC_COLOR:            srcFactor = VK_BLEND_FACTOR_SRC_COLOR;            break;
  case SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:  srcFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;  break;
  case SVK_PLS_SRC_BLEND_FACTOR_DST_COLOR:            srcFactor = VK_BLEND_FACTOR_DST_COLOR;            break;
  case SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_DST_COLOR:  srcFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;  break;
  case SVK_PLS_SRC_BLEND_FACTOR_SRC_ALPHA:            srcFactor = VK_BLEND_FACTOR_SRC_ALPHA;            break;
  case SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:  srcFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;  break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag"); break;
  }

  switch (flags & SVK_PLS_DST_BLEND_FACTOR_BITS)
  {
  case SVK_PLS_DST_BLEND_FACTOR_ZERO:                 dstFactor = VK_BLEND_FACTOR_ZERO;                 break;
  case SVK_PLS_DST_BLEND_FACTOR_ONE:                  dstFactor = VK_BLEND_FACTOR_ONE;                  break;
  case SVK_PLS_DST_BLEND_FACTOR_SRC_COLOR:            dstFactor = VK_BLEND_FACTOR_SRC_COLOR;            break;
  case SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:  dstFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;  break;
  case SVK_PLS_DST_BLEND_FACTOR_DST_COLOR:            dstFactor = VK_BLEND_FACTOR_DST_COLOR;            break;
  case SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_DST_COLOR:  dstFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;  break;
  case SVK_PLS_DST_BLEND_FACTOR_SRC_ALPHA:            dstFactor = VK_BLEND_FACTOR_SRC_ALPHA;            break;
  case SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:  dstFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;  break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag"); break;
  }

  switch (flags & SVK_PLS_BLEND_OP_BITS)
  {
  case SVK_PLS_BLEND_OP_ADD:                          blendOp = VK_BLEND_OP_ADD;              break;
  case SVK_PLS_BLEND_OP_SUBTRACT:                     blendOp = VK_BLEND_OP_SUBTRACT;         break;
  case SVK_PLS_BLEND_OP_REVERSE_SUBTRACT:             blendOp = VK_BLEND_OP_REVERSE_SUBTRACT; break;
  case SVK_PLS_BLEND_OP_MIN:                          blendOp = VK_BLEND_OP_MIN;              break;
  case SVK_PLS_BLEND_OP_MAX:                          blendOp = VK_BLEND_OP_MAX;              break;
  default: ASSERTALWAYS("Vulkan: incorrect pipeline state flag"); break;
  }

  colorBlendAttachment.srcColorBlendFactor  = colorBlendAttachment.srcAlphaBlendFactor  = srcFactor;
  colorBlendAttachment.dstColorBlendFactor  = colorBlendAttachment.dstAlphaBlendFactor  = dstFactor;
  colorBlendAttachment.colorBlendOp         = colorBlendAttachment.alphaBlendOp         = blendOp;

  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;

  VkPipelineDynamicStateCreateInfo dynamicInfo = {};
  dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicInfo.dynamicStateCount = dynamicStatesCount;
  dynamicInfo.pDynamicStates = dynamicStates;


  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicInfo;
  pipelineInfo.layout = gl_VkPipelineLayout;
  pipelineInfo.renderPass = gl_VkRenderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  VkResult r = vkCreateGraphicsPipelines(gl_VkDevice, gl_VkPipelineCache, 1, &pipelineInfo, nullptr, &newState.sps_Pipeline);
  VK_CHECKERROR(r);

  return newState;
}

void SvkMain::CreatePipelineCache()
{
  ASSERT(gl_VkPipelineCache == VK_NULL_HANDLE);

  VkPipelineCacheCreateInfo cacheInfo = {};
  cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

  VkResult r = vkCreatePipelineCache(gl_VkDevice, &cacheInfo, nullptr, &gl_VkPipelineCache);
}

#endif