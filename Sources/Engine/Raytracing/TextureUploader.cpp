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

#define OVERRIDEN_TEXTURE_FILE_EXTENSION ".png"

SSRT::TextureUploader::TextureUploader(RgInstance _instance) : instance(_instance)
{
}

SSRT::TextureUploader::~TextureUploader()
{
}

uint32_t SSRT::TextureUploader::GenerateIndex()
{
  return 1;
  // TODO: generate index
}

void SSRT::TextureUploader::UploadTexture(const CPreparedTextureInfo &info)
{
  RgMaterial material;

  const CTString &overridenPath = info.path->FileDir() + info.path->FileName() + OVERRIDEN_TEXTURE_FILE_EXTENSION;

  if (!info.isDynamic)
  {
    RgStaticMaterialCreateInfo stInfo = {};
    stInfo.size.width = info.width;
    stInfo.size.height = info.height;
    stInfo.data = (uint32_t*)info.imageData;
    stInfo.useMipmaps = info.generateMipmaps;
    stInfo.filter = info.filter;
    stInfo.addressModeU = info.wrapU;
    stInfo.addressModeV = info.wrapV;
    stInfo.disableOverride = true;
    stInfo.relativePath = overridenPath;

    RgResult r = rgCreateStaticMaterial(instance, &stInfo, &material);
    RG_CHECKERROR(r);
  }
  else
  {
    RgDynamicMaterialCreateInfo dninfo = {};
    dninfo.size.width = info.width;
    dninfo.size.height = info.height;
    dninfo.data = (uint32_t*)info.imageData;
    dninfo.useMipmaps = info.generateMipmaps;
    dninfo.filter = info.filter;
    dninfo.addressModeU = info.wrapU;
    dninfo.addressModeV = info.wrapV;

    RgResult r = rgCreateDynamicMaterial(instance, &dninfo, &material);
    RG_CHECKERROR(r);
  }

  // TODO: register material by texture index
}

void SSRT::TextureUploader::UploadTexture(const CPreparedAnimatedTextureInfo &animInfo)
{
  RgMaterial material;
  std::vector<RgStaticMaterialCreateInfo> frames(animInfo.frameCount);

  const CTString &overridenPath = animInfo.path->FileDir() + animInfo.path->FileName() + OVERRIDEN_TEXTURE_FILE_EXTENSION;

  for (uint32_t i = 0; i < animInfo.frameCount; i++)
  {
    auto &f = frames[i];

    void *curFrameData = (uint8_t*)animInfo.frameData + i * animInfo.frameStride;

    f.size.width = animInfo.width;
    f.size.height = animInfo.height;
    f.data = (uint32_t*)curFrameData;
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

  // TODO: register material by texture index
}

RgMaterial SSRT::TextureUploader::Get(CTextureObject *pTexture)
{
  if (pTexture == nullptr)
  {
    return RG_NO_MATERIAL;
  }

  RT_SetTextureAsCurrent(pTexture, this);


}
