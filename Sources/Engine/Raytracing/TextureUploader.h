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

#pragma once

#include "RTGL1/RTGL1.h"
#include "Engine/Raytracing/SSRTObjects.h"

namespace SSRT
{

class TextureUploader
{
public:
  TextureUploader(RgInstance instance);
  ~TextureUploader();

  TextureUploader(const TextureUploader &other) = delete;
  TextureUploader(TextureUploader &&other) noexcept = delete;
  TextureUploader &operator=(const TextureUploader &other) = delete;
  TextureUploader &operator=(TextureUploader &&other) noexcept = delete;

  uint32_t GenerateIndex();
  void UploadTexture(const CPreparedTextureInfo &info);
  void UploadTexture(const CPreparedAnimatedTextureInfo &animInfo);

  RgMaterial Get(CTextureData *pTexture);

private:
  RgInstance instance;

};

}


