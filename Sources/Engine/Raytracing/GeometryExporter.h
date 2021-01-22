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

#include "SSRTObjects.h"

namespace SSRT
{
// If DUMP_GEOMETRY_TO_OBJ set to 1, geometry is exported to files.
// Otherwise empty implementation is used.
class GeometryExporter
{
public:
  static void ExportGeometry(const SSRT::CBrushGeometry &geom);
  static void ExportGeometry(const SSRT::CModelGeometry &geom);
  static void Stop();

private:
  static void ExportGeometry(const SSRT::CAbstractGeometry &geom, INDEX offset, const char *path);
};
}



#if DUMP_GEOMETRY_TO_OBJ
void SSRT::GeometryExporter::ExportGeometry(const SSRT::CAbstractGeometry &geom, INDEX offset, const char *path)
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

void SSRT::GeometryExporter::ExportGeometry(const SSRT::CBrushGeometry &geom)
{
  static INDEX brushOffset = 0;
  GeometryExporter::ExportGeometry(geom, brushOffset, "BRUSHES.obj");
  brushOffset += geom.vertexCount;
}

void SSRT::GeometryExporter::ExportGeometry(const SSRT::CModelGeometry &geom)
{
  static INDEX modelOffset = 0;

  ExportGeometry(geom, modelOffset, "MODELS.obj");
  modelOffset += geom.vertexCount;
}

void SSRT::GeometryExporter::Stop()
{
  // stop program to prevent dump file grow
  ASSERTALWAYS("Geometry was dumped. App will be terminated.");
}

#else

inline void SSRT::GeometryExporter::ExportGeometry(const SSRT::CBrushGeometry &geom)
{}
inline void SSRT::GeometryExporter::ExportGeometry(const SSRT::CModelGeometry &geom)
{}
inline void SSRT::GeometryExporter::ExportGeometry(const SSRT::CAbstractGeometry &geom, INDEX offset, const char *path)
{}
inline void SSRT::GeometryExporter::Stop()
{}

#endif