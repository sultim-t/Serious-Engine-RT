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

constexpr bool DISABLE_OCCLUSION_QUERIES = true;

void SvkMain::InitOcclusionQuerying()
{
  if (DISABLE_OCCLUSION_QUERIES)
  {
    return;
  }

  // create query pool
  VkQueryPoolCreateInfo queryPoolInfo = {};
  queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
  queryPoolInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
  queryPoolInfo.queryCount = SVK_OCCLUSION_QUERIES_MAX;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    VkResult r = vkCreateQueryPool(gl_VkDevice, &queryPoolInfo, nullptr, &gl_VkOcclusionQueryPools[i]);
    VK_CHECKERROR(r);
  }

  CreateOcclusionPipeline();
}

void SvkMain::DestroyOcclusionQuerying()
{
  if (DISABLE_OCCLUSION_QUERIES)
  {
    return;
  }

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    vkDestroyQueryPool(gl_VkDevice, gl_VkOcclusionQueryPools[i], nullptr);
  }

  vkDestroyPipeline(gl_VkDevice, gl_VkPipelineOcclusion, nullptr);
}

void SvkMain::ResetOcclusionQueries(VkCommandBuffer cmd, uint32_t cmdIndex)
{
  if (DISABLE_OCCLUSION_QUERIES)
  {
    return;
  }

  ASSERT(gl_VkCmdIsRecording);

  gl_VkOcclusionQueryLast[cmdIndex] = 0;
  vkCmdResetQueryPool(cmd, gl_VkOcclusionQueryPools[cmdIndex], 0, SVK_OCCLUSION_QUERIES_MAX);
}

uint32_t SvkMain::CreateOcclusionQuery(float fromx, float fromy, float tox, float toy, float z)
{
  if (DISABLE_OCCLUSION_QUERIES)
  {
    return 0;
  }

  ASSERT(gl_VkCmdIsRecording);
  ASSERT(fromx <= tox && fromy <= toy);

  uint32_t queryId = gl_VkOcclusionQueryLast[gl_VkCmdBufferCurrent];
  gl_VkOcclusionQueryLast[gl_VkCmdBufferCurrent]++;

  // create data
  float verts[] = {
    fromx, fromy, z, 1,
    fromx, toy, z, 1,
    tox, toy, z, 1,
    tox, fromy, z, 1
  };

  uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0
  };

  // get buffers to draw
  SvkDynamicBuffer vertexBuffer, indexBuffer;
  GetVertexBuffer(sizeof(verts), vertexBuffer);
  GetIndexBuffer(sizeof(indices), indexBuffer);

  memcpy(vertexBuffer.sdb_Data, verts, sizeof(verts));
  memcpy(indexBuffer.sdb_Data, indices, sizeof(indices));

  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  // begin new query
  vkCmdBeginQuery(cmd, gl_VkOcclusionQueryPools[gl_VkCmdBufferCurrent], queryId, 0);
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gl_VkPipelineOcclusion);

  // reset to prevent usage of this pipeline for a next object
  gl_VkPreviousPipeline = nullptr;

  vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer.sdb_Buffer, &vertexBuffer.sdb_CurrentOffset);
  vkCmdBindIndexBuffer(cmd, indexBuffer.sdb_Buffer, indexBuffer.sdb_CurrentOffset, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);

  vkCmdEndQuery(cmd, gl_VkOcclusionQueryPools[gl_VkCmdBufferCurrent], queryId);

  return queryId;
}

void SvkMain::GetOcclusionResults(uint32_t firstQuery, uint32_t queryCount, uint32_t *results)
{
  if (DISABLE_OCCLUSION_QUERIES)
  {
    return;
  }

  VkResult r = vkGetQueryPoolResults(
    gl_VkDevice, gl_VkOcclusionQueryPools[gl_VkCmdBufferCurrent],
    firstQuery, queryCount,
    queryCount * sizeof(uint32_t), results, sizeof(uint32_t),
    VK_QUERY_RESULT_PARTIAL_BIT);

  if (r != VK_NOT_READY)
  {
    VK_CHECKERROR(r);
  }
}


#endif