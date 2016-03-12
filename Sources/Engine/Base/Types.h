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

#ifndef SE_INCL_TYPES_H
#define SE_INCL_TYPES_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Base.h>
#include <Engine/Graphics/gl_types.h>

typedef signed long  int    SLONG;
typedef signed short int    SWORD;
typedef signed char	        SBYTE;
typedef signed int          SINT;

typedef unsigned long  int  ULONG;
typedef unsigned short int  UWORD;
typedef unsigned char       UBYTE;
typedef unsigned int        UINT;


#ifdef PLATFORM_UNIX  /* rcg10042001 */
    #define __forceinline inline

    #if (!defined MAX_PATH)
      #define MAX_PATH 256
    #endif

    typedef long long  __int64;
    typedef unsigned long  int  DWORD;
    typedef signed long  int    LONG;

    typedef void *HWND;  /* !!! FIXME this sucks. */
    typedef void *HINSTANCE;  /* !!! FIXME this sucks. */
    typedef void *HGLRC;  /* !!! FIXME this sucks. */
    typedef ULONG COLORREF;  /* !!! FIXME this sucks. */

    typedef struct
    {
        LONG x;
        LONG y;
    } POINT;

    typedef struct
    {
        LONG left;
        LONG top;
        LONG right;
        LONG bottom;
    } RECT;
#endif


#define MAX_SLONG ((SLONG)0x7FFFFFFFL)
#define MAX_SWORD ((UWORD)0x7FFF)
#define MAX_SBYTE ((SBYTE)0x7F)

#define MIN_SLONG ((SLONG)0x80000000L)
#define MIN_SWORD ((SWORD)0x8000)
#define MIN_SBYTE ((SBYTE)0x80)

#define MIN_ULONG ((ULONG)0x00000000L)
#define MIN_UWORD ((UWORD)0x0000)
#define MIN_UBYTE ((UBYTE)0x00)

#define MAX_ULONG ((ULONG)0xFFFFFFFFL)
#define MAX_UWORD ((UWORD)0xFFFF)
#define MAX_UBYTE ((UBYTE)0xFF)

typedef int BOOL;		        // this is for TRUE/FALSE
typedef long int RESULT;		// for error codes
typedef long int INDEX;     // for indexed values and quantities

#define FALSE 0
#define TRUE  1

#define NONE 0
#define NOTHING ((void) 0)
#define FOREVER for(;;)

#define DECLARE_NOCOPYING(classname)        \
  classname(const classname &c);            \
  classname &operator=(const classname &c); 
#define IMPLEMENT_NOCOPYING(classname)      \
  classname::classname(const classname &c) { ASSERT(FALSE); };            \
  classname &classname::operator=(const classname &c){ ASSERT(FALSE); return *this; }; 

// standard angles
#define ANGLE_0    (  0.0f)
#define ANGLE_90   ( 90.0f)
#define ANGLE_180  (180.0f)
#define ANGLE_270  (270.0f)
#define ANGLE_360  (360.0f)

// you need <stddef.h> for this!
#define structptr(structure, member, ptr) \
( (struct structure *) ( ((UBYTE *)(ptr)) - \
 offsetof(struct structure, member)) )

// standard types

// simple types
typedef SLONG   PIX;    // pixel coordinates
typedef SLONG   TEX;    // texel coordinates
typedef SLONG   MEX;    // texels in mip-level 0
typedef float   FLOAT;
typedef double  DOUBLE;
typedef float   ANGLE;
typedef float   TIME;
typedef FLOAT   RANGE;
typedef ULONG   COLOR;  // color is always in 32 bit true-color format

// macros for windows/croteam true color conversion
#define CLRF_CLR(clr) ( ((clr & 0xff000000) >> 24) | \
                        ((clr & 0x00ff0000) >>  8) | \
                        ((clr & 0x0000ff00) <<  8))
#define CLR_CLRF(clrref) ( ((clrref & 0x000000ff) << 24) | \
                           ((clrref & 0x0000ff00) <<  8) | \
                           ((clrref & 0x00ff0000) >>  8))

// z-buffer depth constants
#define ZBUF_FRONT  (0.0f)
#define ZBUF_BACK   (1.0f)

// alpha factor constants
#define CT_OPAQUE      MAX_UBYTE
#define CT_TRANSPARENT MIN_UBYTE

// line types (masks)
#define _FULL_	   0xFFFFFFFF
#define _SYMMET16_ 0xFF88FF88
#define _SYMMET32_ 0xFFFF0180
#define _POINT_    0xAAAAAAAA

#define _DOT2_	   0xCCCCCCCC
#define _DOT4_	   0xF0F0F0F0
#define _DOT8_	   0xFF00FF00
#define _DOT16_    0xFFFF0000

#define _TY31_	   0xEEEEEEEE
#define _TY62_	   0xFCFCFCFC
#define _TY124_    0xFFF0FFF0
#define _TY13_	   0x88888888
#define _TY26_	   0xC0C0C0C0
#define _TY412_    0xF000F000

// some mexels constants
#define MAX_MEX_LOG2 10
#define MIN_MEX_LOG2  0
#define MAX_MEX     (1L<<MAX_MEX_LOG2)
#define MIN_MEX     (1L<<MIN_MEX_LOG2)

// macro for converting mexels to meters
#define METERS_MEX(mex)    ((FLOAT)(((FLOAT)mex)/MAX_MEX))
#define MEX_METERS(meters) ((MEX)(meters*MAX_MEX))

#define ARRAYCOUNT(array) (sizeof(array)/sizeof((array)[0]))

// sound volume constants
#define SL_VOLUME_MIN (0.0f)
#define SL_VOLUME_MAX (4.0f)

inline DOUBLE FLOATtoDOUBLE(const FLOAT f) { return DOUBLE(f); }
inline FLOAT DOUBLEtoFLOAT(const DOUBLE d) { return FLOAT(d);  }

inline float UpperLimit(float x) { return +3E38f; }
inline float LowerLimit(float x) { return -3E38f; }
inline double UpperLimit(double x) { return +1E308; }
inline double LowerLimit(double x) { return -1E308; }
inline SLONG UpperLimit(SLONG x) { return MAX_SLONG; }
inline SLONG LowerLimit(SLONG x) { return MIN_SLONG; }
inline SWORD UpperLimit(SWORD x) { return MAX_SWORD; }
inline SWORD LowerLimit(SWORD x) { return MIN_SWORD; }

// class predeclarations
class CAnimData;
class CAnimObject;
class CAnimSet;
class CAnyProjection3D;
class CBrush3D;
class CBrushArchive;
class CBrushEdge;
class CBrushEdge;
class CBrushMip;
class CBrushMip;
class CBrushPlane;
class CBrushPolygon;
class CBrushPolygonEdge;
class CBrushSector;
class CBrushShadowLayer;
class CBrushShadowMap;
class CBrushTexture;
class CBrushTexture;
class CBrushVertex;
class CBrushVertex;
class CTerrain;
class CTerrainTile;
class CTerrainLayer;
class CTerrainArchive;
class CCastRay;
class CChangeable;
class CChangeableRT;
class CClipMove;
class CClipTest;
class CCollisionInfo;
class CCompressor;
class CConsole;
class CContentType;
class CDisplayMode;
class CDLLEntityClass;
class CDrawPort;
class CEntity;
class CEntityPointer;
class CEntityPropertyEnumType;
class CEntityClass;
class CFieldSettings;
class CFontData;
class CFontCharData;
class CGfxLibrary;
class CImageInfo;
class CIsometricProjection3D;
class CLastPositions;
class CLayerMaker;
class CLayerMixer;
class CLightSource;
class CListHead;
class CListNode;
class CLiveEntity;
class CMesh;
class CMessageDispatcher;
class CModelInstance;
class CModelObject;
class CModelData;
class CMovableBrushEntity;
class CMovableEntity;
class CMovableModelEntity;
class CNetworkMessage;
class CNetworkNode;
class CNetworkStream;
class CNetworkStreamBlock;
class CPlacement3D;
class CPlanarGradients;
class CProjection3D;
class CPlayerAction;
class CPlayerBuffer;
class CPlayerCharacter;
class CPlayerEntity;
class CPlayerSource;
class CPlayerTarget;
class CRationalEntity;
class CParallelProjection3D;
class CPerspectiveProjection3D;
class CRaster;
class CRelationDst;
class CRelationLnk;
class CRelationSrc;
class CRenderer;
class CRenderer;
class CSCapeLibrary;
class CSCapeTimerHandler;
class CScreenEdge;
class CScreenEdge;
class CScreenPolygon;
class CScreenPolygon;
class CSerial;
class CServer;
class CSessionState;
class CShadingInfo;
class CShadowMap;
class CSimpleProjection3D;
class CSimpleProjection3D_DOUBLE;
class CShadowMapMaker;
class CShell;
class CShellSymbol;
class CSkeleton;
class CStaticLightSource;
class CSoundData;
class CSoundLibrary;
class CSoundObject;
class CSoundListener;
class CSoundParameters;
class CSoundParameters3D;
class CSurfaceType;
class CTCriticalSection;
class CTextureData;
class CTextureObject;
class CTFileName;
class CTFileStream;
class CTMemoryStream;
class CTSingleLock;
class CTStream;
class CTString;
class CTimer;
class CTimerHandler;
class CTimerValue;
class CObject3D;
class CObjectSector;
class CObjectPolygon;
class CObjectVertex;
class CObjectPlane;
class CObjectMaterial;
class CUpdateable;
class CUpdateableRT;
class CViewPort;
class CWorkingVertex;
class CWorkingPlane;
class CWorkingEdge;
class CWorld;

// template class predeclarations
template<class Type, int iOffset> class CListIter;
template<class Type> class CDynamicArray;
template<class Type> class CDynamicStackArray;
template<class Type> class CDynamicArrayIterator;
template<class Type> class CStaticArray;
template<class Type> class CStaticStackArray;
template<class Type> class CStaticArrayIterator;
template<class Type> class CLinearAllocator;
template<class Type> class CDynamicContainer;
template<class Type, int iRows, int iColumns> class Matrix;
template<class Type, int iDimensions> class AABBox;
template<class Type, int iDimensions> class Vector;
template<class Type, int iDimensions> class Plane;
template<class Type> class OBBox;
template<class Type> class Quaternion;
template<int iInt, int iFrac> class FixInt;

template<class Type, int iDimensions> class BSPVertex;
template<class Type, int iDimensions> class BSPVertexContainer;
template<class Type, int iDimensions> class BSPEdge;
template<class Type, int iDimensions> class BSPNode;
template<class Type, int iDimensions> class BSPPolygon;
template<class Type, int iDimensions> class BSPTree;
template<class Type, int iDimensions> class BSPCutter;

typedef FixInt<16,16>           FIX16_16;

// vectors
typedef Vector<PIX, 2>          PIX2D;
typedef Vector<MEX, 2>          MEX2D;
typedef Vector<PIX, 3>          PIX3D;
typedef Vector<ANGLE, 3>        ANGLE3D;
typedef Vector<FLOAT, 2>        FLOAT2D;
typedef Vector<FLOAT, 3>        FLOAT3D;
typedef Vector<DOUBLE, 2>       DOUBLE2D;
typedef Vector<DOUBLE, 3>       DOUBLE3D;

// planes
typedef Plane<FLOAT, 3>         FLOATplane3D;
typedef Plane<DOUBLE, 3>        DOUBLEplane3D;

// axis-aligned bounding boxes
typedef AABBox<MEX, 2>          MEXaabbox2D;
typedef AABBox<PIX, 2>          PIXaabbox2D;
typedef AABBox<FLOAT, 2>        FLOATaabbox2D;
typedef AABBox<FLOAT, 3>        FLOATaabbox3D;
typedef AABBox<DOUBLE, 2>       DOUBLEaabbox2D;
typedef AABBox<DOUBLE, 3>       DOUBLEaabbox3D;

// oriented bounding boxes
typedef OBBox<FLOAT>            FLOATobbox3D;
typedef OBBox<DOUBLE>           DOUBLEobbox3D;

// matrices
typedef Matrix<PIX, 2, 2>       PIXmatrix2D;
typedef Matrix<PIX, 3, 3>       PIXmatrix3D;
typedef Matrix<ANGLE, 3, 3>     ANGLEmatrix3D;
typedef Matrix<FLOAT, 2, 2>     FLOATmatrix2D;
typedef Matrix<FLOAT, 3, 3>     FLOATmatrix3D;
typedef Matrix<DOUBLE, 3, 3>    DOUBLEmatrix3D;
typedef FLOAT Matrix12[12];

// quaternions
typedef Quaternion<FLOAT>       FLOATquat3D;
typedef Quaternion<DOUBLE>      DOUBLEquat3D;

// BSP types
typedef BSPVertex<DOUBLE, 3>          DOUBLEbspvertex3D;
typedef BSPVertexContainer<DOUBLE, 3> DOUBLEbspvertexcontainer3D;
typedef BSPEdge<DOUBLE, 3>            DOUBLEbspedge3D;
typedef BSPNode<DOUBLE, 3>            DOUBLEbspnode3D;
typedef BSPPolygon<DOUBLE, 3>         DOUBLEbsppolygon3D;
typedef BSPTree<DOUBLE, 3>            DOUBLEbsptree3D;
typedef BSPCutter<DOUBLE, 3>          DOUBLEbspcutter3D;

typedef BSPVertex<FLOAT, 3>          FLOATbspvertex3D;
typedef BSPVertexContainer<FLOAT, 3> FLOATbspvertexcontainer3D;
typedef BSPEdge<FLOAT, 3>            FLOATbspedge3D;
typedef BSPNode<FLOAT, 3>            FLOATbspnode3D;
typedef BSPPolygon<FLOAT, 3>         FLOATbsppolygon3D;
typedef BSPTree<FLOAT, 3>            FLOATbsptree3D;
typedef BSPCutter<FLOAT, 3>          FLOATbspcutter3D;

// general clearing functions
template<class cType>
inline void Clear(cType &t) { t.cType::Clear(); };

// specific clearing functions for built-in types
inline void Clear(signed long int sli) {};
inline void Clear(unsigned long int uli) {};
inline void Clear(int i) {};
inline void Clear(float i) {};
inline void Clear(double i) {};
inline void Clear(void *pv) {};

#define SYMBOLLOCATOR(symbol)

#endif  /* include-once check. */

