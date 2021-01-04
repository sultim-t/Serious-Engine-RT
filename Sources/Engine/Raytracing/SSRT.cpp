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


#include "StdH.h"
#include "SSRT.h"

#include <stdio.h>

#include <Engine/Base/Shell.h>
#include <Engine/World/World.h>
#include <Engine/Graphics/ViewPort.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include <Engine/Raytracing/SSRTObjects.h>
#include <Engine/Raytracing/RTProcessing.h>


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#define RG_CHECKERROR(x) ASSERT(x == RG_SUCCESS)


extern CShell *_pShell;


// dump brushes and models to .obj files;
// use it with care, all calls must be done for one frame
// (offsets are local static variables)
#define DUMP_GEOMETRY_TO_OBJ 0
#if DUMP_GEOMETRY_TO_OBJ
static void ExportGeometry(const CAbstractGeometry &geom, INDEX offset, const char *path)
#endif


namespace SSRT
{
void SSRTMain::Init()
{
  RgInstanceCreateInfo info = {};
  info.name = "RTGL1 Test";
  info.physicalDeviceIndex = 0;
  info.enableValidationLayer = RG_TRUE;

  info.vertexPositionStride = sizeof(GFXVertex);
  info.vertexNormalStride = sizeof(GFXNormal);
  info.vertexTexCoordStride = sizeof(GFXTexCoord);
  info.vertexColorStride = sizeof(uint32_t);
  info.rasterizedDataBufferSize = 32 * 1024 * 1024;

  const char *pWindowExtensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
  };

  info.ppWindowExtensions = pWindowExtensions;
  info.windowExtensionCount = sizeof(pWindowExtensions) / sizeof(const char *);

  info.pfnCreateSurface = [] (uint64_t vkInstance, uint64_t *pResultVkSurfaceKHR)
  {
    VkInstance instance = reinterpret_cast<VkInstance>(vkInstance);

    VkSurfaceKHR surface;

    HINSTANCE hInstance = GetModuleHandle(NULL);
    extern HWND _hwndMain;

    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.hinstance = hInstance;
    createInfo.hwnd = _hwndMain;
    VkResult r = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
    ASSERT(r == VK_SUCCESS);

    *pResultVkSurfaceKHR = reinterpret_cast<uint64_t>(surface);
  };

  info.pfnDebugPrint = [] (const char *msg)
  {
    CPrintF(msg);
  };

  RgResult r = rgCreateInstance(&info, &instance);
  RG_CHECKERROR(r);
}

void SSRTMain::Destroy()
{
  if (instance != 0)
  {
    rgDestroyInstance(instance);
  }
}

SSRTMain::~SSRTMain()
{
  Destroy();
}

void SSRTMain::CopyTransform(RgTransform &dst, const CAbstractGeometry &src)
{
  memcpy(&dst.matrix, src.absRotation.matrix, sizeof(src.absRotation.matrix));
  dst.matrix[0][3] = src.absPosition(1);
  dst.matrix[1][3] = src.absPosition(2);
  dst.matrix[2][3] = src.absPosition(3);
}

void SSRTMain::AddModel(const CModelGeometry &model)
{
  if (model.vertices == nullptr || model.vertexCount == 0 
      || model.indices == nullptr || model.indexCount == 0)
  {
    return;
  }

#if DUMP_GEOMETRY_TO_OBJ
  static INDEX offset = 0;
  ExportGeometry(model, offset, "MODELS.obj");
  offset += model.vertexCount;
#endif

  AddRTObject(model, models, entityToModel);

  // send info to RTGL1
  RgGeometryUploadInfo dnInfo = {};
  dnInfo.geomType = RG_GEOMETRY_TYPE_DYNAMIC;
  dnInfo.vertexCount = model.vertexCount;
  dnInfo.vertexData = (float *) model.vertices;
  dnInfo.normalData = (float *) model.normals;
  dnInfo.texCoordData = (float *) model.texCoords;
  dnInfo.colorData = nullptr;
  dnInfo.indexCount = model.indexCount;
  dnInfo.indexData = (uint32_t *) model.indices;
  dnInfo.geomMaterial = {
    RG_NO_TEXTURE,
    RG_NO_TEXTURE,
    RG_NO_TEXTURE
  };
  CopyTransform(dnInfo.transform, model);

  RgGeometry geomId;
  RgResult r = rgUploadGeometry(instance, &dnInfo, &geomId);
  RG_CHECKERROR(r);
}


void SSRTMain::AddBrush(const CBrushGeometry &brush, bool isMovable)
{
  if (brush.vertices == nullptr || brush.vertexCount == 0
      || brush.indices == nullptr || brush.indexCount == 0)
  {
    return;
  }

  auto &brushArray = isMovable ? movableBrushes : staticBrushes;
  auto &entityToBrush = isMovable ? entityToMovableBrush : entityToStaticBrush;

  auto &found = entityToBrush.find(brush.entityID);

  // if it's already added
  if (found != entityToBrush.end())
  {
    // entity must have only 1 brush
    // TODO: RT: delete "entityToBrush" maps?
    ASSERT(found->second.size() == 1);
    ASSERT(brush.entityID == brushArray[found->second[0]].entityID);

    auto &targetBrush = brushArray[found->second[0]];
    
    targetBrush.isEnabled = brush.isEnabled;
    targetBrush.absPosition = brush.absPosition;
    targetBrush.absRotation = brush.absRotation;
    targetBrush.color = brush.color;

    RgUpdateTransformInfo updateInfo = {};
    updateInfo.movableStaticGeom = targetBrush.rgGeomId;
    CopyTransform(updateInfo.transform, targetBrush);

    RgGeometry geomId;
    RgResult r = rgUpdateGeometryTransform(instance, &updateInfo);
    RG_CHECKERROR(r);

    return;
  }

  if (brush.vertices == nullptr || brush.vertexCount == 0 ||
      brush.indices == nullptr || brush.indexCount == 0)
  {
    return;
  }

#if DUMP_GEOMETRY_TO_OBJ
  static INDEX offset = 0;
  ExportGeometry(brush, offset, "BRUSHES.obj");
  offset += brush.vertexCount;
#endif

  AddRTObject(brush, brushArray, entityToBrush);

  // send info to RTGL1
  RgGeometryUploadInfo stInfo = {};
  stInfo.geomType = isMovable ? RG_GEOMETRY_TYPE_STATIC_MOVABLE : RG_GEOMETRY_TYPE_STATIC;
  stInfo.vertexCount = brush.vertexCount;
  stInfo.vertexData = (float*)brush.vertices;
  stInfo.normalData = (float*)brush.normals;
  stInfo.texCoordData = (float*)brush.texCoords;
  stInfo.colorData = nullptr;
  stInfo.indexCount = brush.indexCount;
  stInfo.indexData = (uint32_t*)brush.indices;
  stInfo.geomMaterial = {
    RG_NO_TEXTURE,
    RG_NO_TEXTURE,
    RG_NO_TEXTURE
  };

  CopyTransform(stInfo.transform, brush);

  RgGeometry geomId;
  RgResult r = rgUploadGeometry(instance, &stInfo, &geomId);
  RG_CHECKERROR(r);
}

void SSRTMain::AddLight(const CSphereLight &sphLt)
{
  sphLights.push_back(sphLt);
}

void SSRTMain::AddLight(const CDirectionalLight &dirLt)
{
  dirLights.push_back(dirLt);
}

CWorld *SSRTMain::GetCurrentWorld()
{
#ifdef _WIN64
  CWorld *pwo = (CWorld *) _pShell->GetUINT64("pwoCurrentWorld64_0", "pwoCurrentWorld64_1");
#else
  CWorld *pwo = (CWorld *) _pShell->GetINDEX("pwoCurrentWorld");
#endif

  return pwo;
}

void SSRTMain::StartFrame(CViewPort *pvp)
{
  extern INDEX srt_bEnableRayTracing;
  srt_bEnableRayTracing = Clamp(srt_bEnableRayTracing, 0L, 1L);

  currentFirstPersonModelCount = 0;

  CWorld *pwo = GetCurrentWorld();
  if (pwo == nullptr)
  {
    StopWorld();
  }

  RECT rectWindow;
  GetClientRect(pvp->vp_hWnd, &rectWindow);

  curWindowWidth = rectWindow.right - rectWindow.left;
  curWindowHeight = rectWindow.bottom - rectWindow.top;

  RgResult r = rgStartFrame(instance, curWindowWidth, curWindowHeight);
  RG_CHECKERROR(r);
}

void SSRTMain::ProcessWorld(const CWorldRenderingInfo &info)
{
  CWorld *world = info.world;

  CWorld *pwo = GetCurrentWorld();
  ASSERT(world == pwo);

  if (world == nullptr)
  {
    return;
  }

  // check if need to reload new world
  if (world->GetName() != currentWorldName)
  {
    SetWorld(world);
  }

  worldRenderInfo = info;
  isRenderingWorld = true;

  // check all movable brushes, models and light sources
  FOREACHINDYNAMICCONTAINER(world->wo_cenEntities, CEntity, iten)
  {
    if (iten->en_ulID == info.viewerEntityID)
    {
      continue;
    }

    if (iten->en_RenderType == CEntity::RT_MODEL)
    {
      // add it as a model and scan for light sources
      RT_AddModelEntity(&iten.Current(), this);
    }
    else if (iten->en_RenderType == CEntity::RT_BRUSH && (iten->en_ulPhysicsFlags & EPF_MOVABLE))
    {
      if ((iten->en_ulFlags & ENF_HIDDEN) || (iten->en_ulFlags & ENF_ZONING))
      {
        return;
      }

      // if it's a movable brush
      const CPlacement3D &placement = iten->GetLerpedPlacement();

      FLOAT3D position = placement.pl_PositionVector;
      FLOATmatrix3D rotation;
      MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

      // update its transform
      CBrushGeometry brushInfo = {};
      brushInfo.entityID = iten->en_ulID;
      brushInfo.isEnabled = true;
      brushInfo.absPosition = position;
      brushInfo.absRotation = rotation;

      // leave vertex data empty, it won't be updated
      AddBrush(brushInfo, true);
    }
  }

#if DUMP_GEOMETRY_TO_OBJ
  // stop program to prevent dump file grow
  ASSERTALWAYS("Geometry was dumped. App will be terminated.");
#endif
}

void SSRTMain::ProcessFirstPersonModel(const CFirstPersonModelInfo &info)
{
  // world must be currently rendering
  ASSERT(isRenderingWorld);

  RT_AddFirstPersonModel(info.modelObject, info.renderModel, SSRT_FIRSTPERSON_ENTITY_START_ID + currentFirstPersonModelCount, this);
  currentFirstPersonModelCount++;
}

void SSRTMain::ProcessHudElement(const CHudElementInfo &hud)
{
  // RT: TODO: send hud info to RTGL1
}

void SSRTMain::EndFrame()
{
  RgDrawFrameInfo frameInfo = {};
  frameInfo.renderWidth = curWindowWidth;
  frameInfo.renderHeight = curWindowHeight;

  memcpy(frameInfo.view, worldRenderInfo.viewMatrix, 16 * sizeof(float));
  memcpy(frameInfo.viewInversed, worldRenderInfo.viewMatrixInversed, 16 * sizeof(float));
  memcpy(frameInfo.projection, worldRenderInfo.projectionMatrix, 16 * sizeof(float));
  memcpy(frameInfo.projectionInversed, worldRenderInfo.projectionMatrixInversed, 16 * sizeof(float));

  RgResult r = rgDrawFrame(instance, &frameInfo);
  RG_CHECKERROR(r);

  // models will be rescanned in the beginning of the frame
  // it's done because of dynamic vertex data
  entityToModel.clear();
  models.clear();

  // lights will be readded too
  sphLights.clear();
  dirLights.clear();
}

void SSRTMain::SetWorld(CWorld *pwld)
{
  currentWorldName = pwld->GetName();

  RgResult r = rgStartNewScene(instance);
  RG_CHECKERROR(r);

  // find all brushes, their geomtry won't change,
  // but movable brushes have dynamic transformation 
  // and they'll be updated on a frame start

  FOREACHINDYNAMICCONTAINER(pwld->wo_cenEntities, CEntity, iten)
  {
    // if it is any brush
    if (iten->en_RenderType == CEntity::RT_BRUSH)
    {
      // add all of its sectors
      RT_AddNonZoningBrush(&iten.Current(), NULL, this);
    }
  }
}

void SSRTMain::StopWorld()
{
  currentWorldName = "";

  models.clear();
  staticBrushes.clear();
  movableBrushes.clear();
  sphLights.clear();
  dirLights.clear();

  entityToModel.clear();
  entityToStaticBrush.clear();
  entityToMovableBrush.clear();
}

}


#if DUMP_GEOMETRY_TO_OBJ
static void ExportGeometry(const CAbstractGeometry &geom, INDEX offset, const char *path)
{
  FILE *file = fopen(path, "a");
  if (file == nullptr)
  {
    return;
  }

  for (INDEX i = 0; i < geom.vertexCount; i++)
  {
    FLOAT3D p = FLOAT3D(geom.vertices[i].x, geom.vertices[i].y, geom.vertices[i].z);
    FLOAT3D n = geom.normals ? FLOAT3D(geom.normals[i].nx, geom.normals[i].ny, geom.normals[i].nz) : FLOAT3D(0, 1, 0);
    FLOAT2D t = geom.texCoords ? FLOAT2D(geom.texCoords[i].s, geom.texCoords[i].t) : FLOAT2D();

    p = p * geom.absRotation + geom.absPosition;
    n = n * geom.absRotation;

    fprintf(file, "v %.3f %.3f %.3f\n", p(1), p(2), p(3));
    fprintf(file, "vn %.3f %.3f %.3f\n", n(1), n(2), n(3));
    fprintf(file, "vt %.3f %.3f\n", t(1), t(2));
  }

  fprintf(file, "g %d\n", geom.entityID);

  INDEX triangleCount = geom.indexCount / 3;

  for (INDEX i = 0; i < triangleCount; i++)
  {
    // obj indices start with 1
    INDEX a = geom.indices[i * 3 + 0] + offset + 1;
    INDEX b = geom.indices[i * 3 + 1] + offset + 1;
    INDEX c = geom.indices[i * 3 + 2] + offset + 1;

    fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", a, a, a, b, b, b, c, c, c);
  }

  fclose(file);
}
#endif

