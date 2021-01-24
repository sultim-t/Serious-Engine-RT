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

#include <Engine/Graphics/Texture.h>

SSRT::TextureUploader::TextureUploader(RgInstance _instance) : instance(_instance)
{
}

SSRT::TextureUploader::~TextureUploader()
{
}

RgMaterial SSRT::TextureUploader::Get(CTextureData *pTexture)
{
  return RG_NO_MATERIAL;
  //pTexture->
}
