/* Copyright (c) 2002-2012 Croteam Ltd. 
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

#ifndef SE_INCL_VERTEX_H
#define SE_INCL_VERTEX_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif


struct GFXVertex3
{
  FLOAT x,y,z;
};


struct GFXNormal3
{
  FLOAT nx,ny,nz;
};


struct GFXTexCoord
{
  union {
    struct { FLOAT u,v; };
    struct { FLOAT s,t; };
  };
};


struct GFXTexCoord4
{
  FLOAT s,t,r,q;
};


struct GFXColor
{
  union {
    struct { UBYTE r,g,b,a; };
    struct { ULONG abgr;    };  // reverse order - use ByteSwap()!
  };

  GFXColor() {};

  GFXColor( COLOR col) {
    _asm mov   ecx,dword ptr [this]
    _asm mov   eax,dword ptr [col]
    _asm bswap eax
    _asm mov   dword ptr [ecx],eax
  }

  __forceinline void Set( COLOR col) {
    _asm mov   ecx,dword ptr [this]
    _asm mov   eax,dword ptr [col]
    _asm bswap eax
    _asm mov   dword ptr [ecx],eax
  }

  void MultiplyRGBA( const GFXColor &col1, const GFXColor &col2) {
    r = (ULONG(col1.r)*col2.r)>>8;
    g = (ULONG(col1.g)*col2.g)>>8;
    b = (ULONG(col1.b)*col2.b)>>8;
    a = (ULONG(col1.a)*col2.a)>>8;
  }

  void MultiplyRGB( const GFXColor &col1, const GFXColor &col2) {
    r = (ULONG(col1.r)*col2.r)>>8;
    g = (ULONG(col1.g)*col2.g)>>8;
    b = (ULONG(col1.b)*col2.b)>>8;
  }

  void MultiplyRGBCopyA1( const GFXColor &col1, const GFXColor &col2) {
    r = (ULONG(col1.r)*col2.r)>>8;
    g = (ULONG(col1.g)*col2.g)>>8;
    b = (ULONG(col1.b)*col2.b)>>8;
    a = col1.a;
  }

  void AttenuateRGB( ULONG ulA) {
    r = (ULONG(r)*ulA)>>8;
    g = (ULONG(g)*ulA)>>8;
    b = (ULONG(b)*ulA)>>8;
  }

  void AttenuateA( ULONG ulA) {
    a = (ULONG(a)*ulA)>>8;
  }
};


#define GFXVertex GFXVertex4
struct GFXVertex4
{
  GFXVertex4()
  {
  }
  FLOAT x,y,z;
  union {
    struct { struct GFXColor col; };
    struct { SLONG shade; };
  };
};


#define GFXNormal GFXNormal4
struct GFXNormal4
{
  FLOAT nx,ny,nz;
  ULONG ul;
};



#endif  /* include-once check. */

