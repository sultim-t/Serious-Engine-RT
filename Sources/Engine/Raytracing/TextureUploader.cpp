/* Copyright (c) 2021 Sultim Tsyrendashiev
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

#include "StdH.h"
#include "TextureUploader.h"

#include <vector>

#include <Engine/Graphics/Texture.h>
#include <Engine/Raytracing/RTProcessing.h>

#define TEXTURE_INDEX_OFFSET              1
#define TEXTURE_COUNT_ALLOC_STEP          512
#define OVERRIDEN_TEXTURE_FILE_EXTENSION  ".png"

uint32_t SSRT::TextureUploader::IndexToID(uint32_t index)
{
  return index + TEXTURE_INDEX_OFFSET;
}

uint32_t SSRT::TextureUploader::IDToIndex(uint32_t id)
{
  ASSERT(id != 0);
  return id - TEXTURE_INDEX_OFFSET;
}

SSRT::TextureUploader::TextureUploader(RgInstance _instance) : instance(_instance)
{
  materials.resize(TEXTURE_COUNT_ALLOC_STEP);
  materialExist.resize(TEXTURE_COUNT_ALLOC_STEP);
}

SSRT::TextureUploader::~TextureUploader()
{
  RgResult r;

  for (uint32_t i = 0; i < materials.size(); i++)
  {
    if (materialExist[i])
    {
      r = rgDestroyMaterial(instance, materials[i]);
      RG_CHECKERROR(r);
    }
  }
}

uint32_t SSRT::TextureUploader::GenerateTextureID()
{
  // find empty in materials
  auto it = 
    std::find(materialExist.begin(), materialExist.end(), false);

  uint32_t textureIndex;

  if (it != materialExist.end())
  {
    textureIndex = (uint32_t)std::distance(materialExist.begin(), it);
  }
  else
  {
    // realloc all arrays if not enough indices
    uint32_t oldSize = (uint32_t)materialExist.size();
    uint32_t newSize = oldSize + TEXTURE_COUNT_ALLOC_STEP;

    materialExist.resize(newSize);
    materials.resize(newSize);

    textureIndex = oldSize;
  }

  return IndexToID(textureIndex);
}

void SSRT::TextureUploader::DeleteTexture(uint32_t textureID)
{
  DestroyMaterial(IDToIndex(textureID));
}

void SSRT::TextureUploader::UploadTexture(const CPreparedTextureInfo &info)
{
  RgMaterial material;
  uint32_t textureIndex = IDToIndex(info.textureID);

  const CTString &overridenPath = info.path->FileDir() + info.path->FileName() + OVERRIDEN_TEXTURE_FILE_EXTENSION;

  if (!info.isDynamic)
  {
    // static textures must not be uploaded yet 
    ASSERT(!materialExist[textureIndex]);

    RgStaticMaterialCreateInfo stInfo = {};
    stInfo.size.width = info.width;
    stInfo.size.height = info.height;
    stInfo.textureData.albedoAlphaData = (uint32_t*)info.imageData;
    stInfo.useMipmaps = info.generateMipmaps;
    stInfo.filter = info.filter;
    stInfo.addressModeU = info.wrapU;
    stInfo.addressModeV = info.wrapV;
    stInfo.disableOverride = false;
    stInfo.relativePath = overridenPath;

    RgResult r = rgCreateStaticMaterial(instance, &stInfo, &material);
    RG_CHECKERROR(r);
  }
  else
  {
    // if material already exist, then must just update it, without creating
    bool onlyUpdate = materialExist[textureIndex];

    if (!onlyUpdate)
    {
      RgDynamicMaterialCreateInfo dninfo = {};
      dninfo.size.width = info.width;
      dninfo.size.height = info.height;
      dninfo.textureData.albedoAlphaData = (uint32_t *)info.imageData;
      dninfo.useMipmaps = info.generateMipmaps;
      dninfo.filter = info.filter;
      dninfo.addressModeU = info.wrapU;
      dninfo.addressModeV = info.wrapV;

      RgResult r = rgCreateDynamicMaterial(instance, &dninfo, &material);
      RG_CHECKERROR(r);
    }
    else
    {
      material = materials[textureIndex];

      RgDynamicMaterialUpdateInfo updateInfo = {};
      updateInfo.dynamicMaterial = material;
      updateInfo.textureData.albedoAlphaData = (uint32_t *)info.imageData;

      RgResult r = rgUpdateDynamicMaterial(instance, &updateInfo);
      RG_CHECKERROR(r);

      // don't add
      return;
    }
  }

  AddMaterial(textureIndex, material);
}

void SSRT::TextureUploader::UploadTexture(const CPreparedAnimatedTextureInfo &animInfo)
{
  uint32_t textureIndex = IDToIndex(animInfo.textureID);

  // must not be uploaded yet
  ASSERT(!materialExist[textureIndex]);

  RgMaterial material;
  std::vector<RgStaticMaterialCreateInfo> frames(animInfo.frameCount);

  const CTString &overridenPath = animInfo.path->FileDir() + animInfo.path->FileName() + OVERRIDEN_TEXTURE_FILE_EXTENSION;

  for (uint32_t i = 0; i < animInfo.frameCount; i++)
  {
    auto &f = frames[i];

    void *curFrameData = (uint8_t*)animInfo.frameData + i * animInfo.frameStride;

    f.size.width = animInfo.width;
    f.size.height = animInfo.height;
    f.textureData.albedoAlphaData = (uint32_t*)curFrameData;
    f.useMipmaps = animInfo.generateMipmaps;
    f.filter = animInfo.filter;
    f.addressModeU = animInfo.wrapU;
    f.addressModeV = animInfo.wrapV;
    f.disableOverride = true;
    f.relativePath = overridenPath;
  }

  RgAnimatedMaterialCreateInfo info = {};
  info.frameCount = animInfo.frameCount;
  info.frames = frames.data();

  RgResult r = rgCreateAnimatedMaterial(instance, &info, &material);
  RG_CHECKERROR(r);

  AddMaterial(textureIndex, material);
}

RgMaterial SSRT::TextureUploader::GetMaterial(CTextureData *pTexture, uint32_t textureFrameIndex)
{
  if (pTexture == nullptr)
  {
    return RG_NO_MATERIAL;
  }

  // update texture, and upload if needed
  uint32_t textureID = RT_SetTextureAsCurrent(pTexture, this);
  uint32_t textureIndex = IDToIndex(textureID);

  ASSERT(materialExist[textureIndex]);

  RgMaterial mat = materials[textureIndex];

  // if an animated material
  if (pTexture->td_ctFrames > 1)
  {
    // update frame index
    RgResult r = rgChangeAnimatedMaterialFrame(instance, mat, textureFrameIndex);
    RG_CHECKERROR(r);
  }

  return mat;
}

void SSRT::TextureUploader::AddMaterial(uint32_t textureIndex, RgMaterial material)
{
  // set default values
  materialExist[textureIndex] = true;
  materials[textureIndex] = material;
}

void SSRT::TextureUploader::DestroyMaterial(uint32_t textureIndex)
{
  ASSERT(materialExist[textureIndex]);

  RgResult r = rgDestroyMaterial(instance, materials[textureIndex]);
  RG_CHECKERROR(r);

  materialExist[textureIndex] = false;
  materials[textureIndex] = RG_NO_MATERIAL;
}

