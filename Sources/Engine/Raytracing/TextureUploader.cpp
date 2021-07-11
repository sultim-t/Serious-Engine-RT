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

#include "SSRTGlobals.h"
#include <string>


extern SSRT::SSRTGlobals _srtGlobals;


#define TEXTURE_INDEX_OFFSET              1
#define TEXTURE_COUNT_ALLOC_STEP          512
#define OVERRIDEN_TEXTURE_FILE_EXTENSION  ".ktx2"

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
  animatedTextureFrameIndex.resize(TEXTURE_COUNT_ALLOC_STEP, UINT32_MAX);
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
    animatedTextureFrameIndex.resize(newSize, UINT32_MAX);

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
    stInfo.textures.albedoAlpha.pData = info.imageData;
    stInfo.textures.albedoAlpha.isSRGB = _srtGlobals.srt_bTexturesOriginalSRGB;
    stInfo.useMipmaps = info.generateMipmaps;
    stInfo.filter = info.filter;
    stInfo.addressModeU = info.wrapU;
    stInfo.addressModeV = info.wrapV;
    stInfo.disableOverride = false;
    stInfo.pRelativePath = overridenPath;

    RgResult r = rgCreateStaticMaterial(instance, &stInfo, &material);
    RG_CHECKERROR(r);

    AddMaterial(textureIndex, material);
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
      dninfo.textures.albedoAlpha.pData = info.imageData;
      dninfo.textures.albedoAlpha.isSRGB = _srtGlobals.srt_bTexturesOriginalSRGB;
      dninfo.useMipmaps = info.generateMipmaps;
      dninfo.filter = info.filter;
      dninfo.addressModeU = info.wrapU;
      dninfo.addressModeV = info.wrapV;

      RgResult r = rgCreateDynamicMaterial(instance, &dninfo, &material);
      RG_CHECKERROR(r);

      
      AddMaterial(textureIndex, material);
    }
    else
    {
      material = materials[textureIndex];

      RgDynamicMaterialUpdateInfo updateInfo = {};
      updateInfo.dynamicMaterial = material;
      updateInfo.textures.albedoAlpha.pData = info.imageData;
      updateInfo.textures.albedoAlpha.isSRGB = _srtGlobals.srt_bTexturesOriginalSRGB;

      RgResult r = rgUpdateDynamicMaterial(instance, &updateInfo);
      RG_CHECKERROR(r);

      // don't add
    }
  }
}

void SSRT::TextureUploader::UploadTexture(const CPreparedAnimatedTextureInfo &animInfo)
{
  uint32_t textureIndex = IDToIndex(animInfo.textureID);

  // must not be uploaded yet
  ASSERT(!materialExist[textureIndex]);

  // must be 3-digit number for overriding
  if (animInfo.frameCount > 999)
  {
    ASSERTALWAYS("Aniamated texture frame count must be less or equal to 999");
    return;
  }

  // allocate for frame infos
  RgMaterial material;
  std::vector<RgStaticMaterialCreateInfo> frames(animInfo.frameCount);


  // overriden path
  std::string overridenPathBase = animInfo.path->FileDir();
  overridenPathBase += animInfo.path->FileName();

  const uint32_t counterOffset = (uint32_t)overridenPathBase.length();
  overridenPathBase += "000";

  overridenPathBase += OVERRIDEN_TEXTURE_FILE_EXTENSION;

  std::vector<char> allChars(animInfo.frameCount * (overridenPathBase.length() + 1));


  for (uint32_t i = 0; i < animInfo.frameCount; i++)
  {
    // copy base path
    char *curFrameOvrd = &allChars[i * (overridenPathBase.length() + 1)];
    memcpy(curFrameOvrd, overridenPathBase.data(), overridenPathBase.length());
    curFrameOvrd[overridenPathBase.length()] = '\0';
    
    curFrameOvrd[counterOffset + 0] = '0' + (i / 100) % 10;
    curFrameOvrd[counterOffset + 1] = '0' + (i / 10) % 10;
    curFrameOvrd[counterOffset + 2] = '0' + i % 10;


    void *curFrameData = (uint8_t*)animInfo.frameData + i * animInfo.frameStride;

    auto &f = frames[i];
    f.size.width = animInfo.width;
    f.size.height = animInfo.height;
    f.textures.albedoAlpha.pData = curFrameData;
    f.textures.albedoAlpha.isSRGB = _srtGlobals.srt_bTexturesOriginalSRGB;
    f.useMipmaps = animInfo.generateMipmaps;
    f.filter = animInfo.filter;
    f.addressModeU = animInfo.wrapU;
    f.addressModeV = animInfo.wrapV;
    f.disableOverride = false;
    f.pRelativePath = curFrameOvrd;
  }

  RgAnimatedMaterialCreateInfo info = {};
  info.frameCount = animInfo.frameCount;
  info.pFrames = frames.data();

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
    // and it doesn't have relevant texture frame index
    if (animatedTextureFrameIndex[textureIndex] != textureFrameIndex)
    {
      // update frame index
      RgResult r = rgChangeAnimatedMaterialFrame(instance, mat, textureFrameIndex);
      RG_CHECKERROR(r);

      animatedTextureFrameIndex[textureIndex] = textureFrameIndex;
    }
  }

  return mat;
}

void SSRT::TextureUploader::AddMaterial(uint32_t textureIndex, RgMaterial material)
{
  // set default values
  materialExist[textureIndex] = true;
  materials[textureIndex] = material;
  animatedTextureFrameIndex[textureIndex] = UINT32_MAX;
}

void SSRT::TextureUploader::DestroyMaterial(uint32_t textureIndex)
{
  ASSERT(materialExist[textureIndex]);

  RgResult r = rgDestroyMaterial(instance, materials[textureIndex]);
  RG_CHECKERROR(r);

  materialExist[textureIndex] = false;
  materials[textureIndex] = RG_NO_MATERIAL;
  animatedTextureFrameIndex[textureIndex] = UINT32_MAX;
}

