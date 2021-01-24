/* Copyright (c) 2020-2021 Sultim Tsyrendashiev
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

namespace SSRT
{
class Scene;
class TextureUploader;
}

class CRenderModel;

void RT_AddNonZoningBrush(CEntity *penBrush, SSRT::Scene *scene);

void RT_AddModelEntity(const CEntity *penModel, const FLOAT3D *viewerPos, SSRT::Scene *ssrt);
void RT_AddFirstPersonModel(CModelObject *mo, CRenderModel *rm, ULONG entityId, SSRT::Scene *scene);

unsigned RT_SetTextureAsCurrent(CTextureData *textureData, SSRT::TextureUploader *uploader);
