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

#ifndef SE_INCL_FUNCTIONS_H
#define SE_INCL_FUNCTIONS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// asm shortcuts
#define O offset
#define Q qword ptr
#define D dword ptr
#define W  word ptr
#define B  byte ptr

/* 
 *  template implementations
 */

template<class Type>
inline Type Abs( const Type x)
{
  return ( x>=Type(0) ? x : -x );
}

template<class Type>
inline Type Max( const Type a, const Type b)
{
  return ( a<b ? b : a );
}

template<class Type>
inline Type Min( const Type a, const Type b)
{
  return ( a>b ? b : a );
}

// linear interpolation
template<class Type>
inline Type Lerp( const Type x0, const Type x1, const FLOAT fRatio)
{
       if( fRatio==0) return x0;
  else if( fRatio==1) return x1;
  else return x0+(x1-x0)*fRatio;
}

template<class Type>
inline Type Sgn( const Type x)
{
  return (x)>Type(0) ? Type(1):( x<0 ? Type(-1):Type(0) );
}

template<class Type>
inline Type SgnNZ( const Type x)
{
  return (x)>=Type(0) ? Type(1):Type(-1);
}

template<class Type>
inline void Swap( Type &a, Type &b)
{
  Type t=a; a=b;  b=t;
} 

template<class Type>
inline Type ClampUp( const Type x, const Type uplimit)
{
  return ( x<=uplimit ? x : uplimit );
}

template<class Type>
inline Type ClampDn( const Type x, const Type dnlimit)
{
  return ( x>=dnlimit ? x : dnlimit );
}

template<class Type>
inline Type Clamp( const Type x, const Type dnlimit, const Type uplimit)
{
  return ( x>=dnlimit ? (x<=uplimit ? x : uplimit): dnlimit );
}

/* 
 *  fast implementations
 */


inline DOUBLE Abs( const DOUBLE f) { return fabs(f); }
inline FLOAT  Abs( const FLOAT f)  { return (FLOAT)fabs(f); }
inline SLONG  Abs( const SLONG sl) { return labs(sl); }


/*
inline FLOAT Min( const FLOAT fA, const FLOAT fB)
{
  FLOAT fRet;
  __asm {
    fld     D [fA]
    fld     D [fB]
    fucomi  st(0),st(1)
    fcmovnb st(0),st(1)
    ffree   st(1)
    fstp    D [fRet]
  }
  return fRet;
}

inline FLOAT Max( const FLOAT fA, const FLOAT fB)
{
  FLOAT fRet;
  __asm {
    fld     D [fA]
    fld     D [fB]
    fucomi  st(0),st(1)
    fcmovb  st(0),st(1)
    ffree   st(1)
    fstp    D [fRet]
  }
  return fRet;
}


inline SLONG Min( const SLONG slA, const SLONG slB)
{
  SLONG slRet;
  __asm {
    mov     eax,D [slA]
    cmp     eax,D [slB]
    cmovg   eax,D [slB]
    mov     D [slRet],eax
  }
  return slRet;
}

inline ULONG Min( const ULONG slA, const ULONG slB)
{
  ULONG ulRet;
  __asm {
    mov     eax,D [slA]
    cmp     eax,D [slB]
    cmova   eax,D [slB]
    mov     D [ulRet],eax
  }
  return ulRet;
}

inline SLONG Max( const SLONG slA, const SLONG slB)
{
  SLONG slRet;
  __asm {
    mov     eax,D [slA]
    cmp     eax,D [slB]
    cmovl   eax,D [slB]
    mov     D [slRet],eax
  }
  return slRet;
}

inline ULONG Max( const ULONG slA, const ULONG slB)
{
  ULONG ulRet;
  __asm {
    mov     eax,D [slA]
    cmp     eax,D [slB]
    cmovb   eax,D [slB]
    mov     D [ulRet],eax
  }
  return ulRet;
}



inline FLOAT ClampUp( const FLOAT f, const FLOAT fuplimit)
{
  FLOAT fRet;
  __asm {
    fld     D [fuplimit]
    fld     D [f]
    fucomi  st(0),st(1)
    fcmovnb st(0),st(1)
    fstp    D [fRet]
    fstp    st(0)
  }
  return fRet;
}

inline FLOAT ClampDn( const FLOAT f, const FLOAT fdnlimit)
{
  FLOAT fRet;
  __asm {
    fld     D [fdnlimit]
    fld     D [f]
    fucomi  st(0),st(1)
    fcmovb  st(0),st(1)
    fstp    D [fRet]
    fstp    st(0)
  }
  return fRet;
}

inline FLOAT Clamp( const FLOAT f, const FLOAT fdnlimit, const FLOAT fuplimit)
{
  FLOAT fRet;
  __asm {
    fld     D [fdnlimit]
    fld     D [fuplimit]
    fld     D [f]
    fucomi  st(0),st(2)
    fcmovb  st(0),st(2)
    fucomi  st(0),st(1)
    fcmovnb st(0),st(1)
    fstp    D [fRet]
    fcompp
  }
  return fRet;
}


inline SLONG ClampDn( const SLONG sl, const SLONG sldnlimit)
{
  SLONG slRet;
  __asm {
    mov     eax,D [sl]
    cmp     eax,D [sldnlimit]
    cmovl   eax,D [sldnlimit]
    mov     D [slRet],eax
  }
  return slRet;
}

inline SLONG ClampUp( const SLONG sl, const SLONG sluplimit)
{
  SLONG slRet;
  __asm {
    mov     eax,D [sl]
    cmp     eax,D [sluplimit]
    cmovg   eax,D [sluplimit]
    mov     D [slRet],eax
  }
  return slRet;
}

inline SLONG Clamp( const SLONG sl, const SLONG sldnlimit, const SLONG sluplimit)
{
  SLONG slRet;
  __asm {
    mov     eax,D [sl]
    cmp     eax,D [sldnlimit]
    cmovl   eax,D [sldnlimit]
    cmp     eax,D [sluplimit]
    cmovg   eax,D [sluplimit]
    mov     D [slRet],eax
  }
  return slRet;
}

*/

/* 
 *  fast functions
 */

#define FP_ONE_BITS  0x3F800000

// fast reciprocal value
inline FLOAT FastRcp( const FLOAT f)
{
  INDEX i = 2*FP_ONE_BITS - *(INDEX*)&(f); 
  FLOAT r = *(FLOAT*)&i;
  return( r * (2.0f - f*r));
}

// convert float from 0.0f to 1.0f -> ulong form 0 to 255
inline ULONG NormFloatToByte( const FLOAT f)
{
    /* rcg10042001 !!! FIXME: Move this elsewhere. */
#ifdef _MSC_VER
  const FLOAT f255 = 255.0f;
  ULONG ulRet;
  __asm {
    fld   D [f]
    fmul  D [f255]
    fistp D [ulRet]
  }
  return ulRet;
#else
  assert((f >= 0.0) && (f <= 1.0));
  return( (ULONG) (f * 255.0) );
#endif
}

// convert ulong from 0 to 255 -> float form 0.0f to 255.0f
inline FLOAT NormByteToFloat( const ULONG ul)
{
  return (FLOAT)ul * (1.0f/255.0f);
}


// fast float to int conversion
inline SLONG FloatToInt( FLOAT f)
{
#if (defined USE_PORTABLE_C)
  return((SLONG) f);  /* best of luck to you. */

#elif (defined _MSC_VER)
  SLONG slRet;
  __asm {
    fld    D [f]
    fistp  D [slRet]
  }
  return slRet;

#elif (defined __GNUC__)
  SLONG slRet;
  __asm__ __volatile__ (
    "flds     (%%ebx)   \n\t"
    "fistpl   (%%esi)   \n\t"
        :
        : "b" (&f), "S" (&slRet)
        : "memory"
  );
  return(slRet);
#else
  #error Fill this in for your platform.
#endif
}

// log base 2 of any float numero
inline FLOAT Log2( FLOAT f) {
#if (defined USE_PORTABLE_C)
  return (FLOAT)(log10(x)*3.321928094887);  // log10(x)/log10(2)

#elif (defined _MSC_VER)
  FLOAT fRet;
  _asm {
    fld1
    fld     D [f]
    fyl2x
    fstp    D [fRet]
  }
  return fRet;

#elif (defined __GNUC__)
  FLOAT fRet;
  __asm__ __volatile__ (
    "fld1               \n\t"
    "flds     (%%ebx)   \n\t"
    "fyl2x              \n\t"
    "fstps    (%%esi)   \n\t"
        :
        : "b" (&f), "S" (&fRet)
        : "memory"
  );
  return(fRet);
#else
  #error Fill this in for your platform.
#endif
}


// returns accurate values only for integers that are power of 2
inline SLONG FastLog2( SLONG x)
{
#if (defined USE_PORTABLE_C)
  #error write me.

#elif (defined _MSC_VER)
  SLONG slRet;
  __asm {
    bsr   eax,D [x]
    mov   D [slRet],eax
  }
  return slRet;

#elif (defined __GNUC__)
  SLONG slRet;
  __asm__ __volatile__ (
    "bsrl  (%%ebx), %%eax     \n\t"
    "movl   %%eax, (%%esi)    \n\t"
        :
        : "b" (&x), "S" (&slRet)
        : "memory"
  );
  return(slRet);
#else
  #error Fill this in for your platform.
#endif
}

// returns log2 of first larger value that is a power of 2
inline SLONG FastMaxLog2( SLONG x)
{ 
#if (defined USE_PORTABLE_C)
  #error write me.

#elif (defined _MSC_VER)
  SLONG slRet;
  __asm {
    bsr   eax,D [x]
    bsf   edx,D [x]
    cmp   edx,eax
    adc   eax,0
    mov   D [slRet],eax
  }
  return slRet;

#elif (defined __GNUC__)
  SLONG slRet;
  __asm__ __volatile__ (
    "bsrl  (%%ebx), %%eax     \n\t"
    "bsfl  (%%ebx), %%edx     \n\t"
    "cmpl  %%eax, %%edx       \n\t"
    "adcl  $0, %%eax          \n\t"
    "movl  %%eax, (%%esi)     \n\t"
        :
        : "b" (&x), "S" (&slRet)
        : "memory"
  );
  return(slRet);
#else
  #error Fill this in for your platform.
#endif
}



// square root (works with negative numbers)
inline FLOAT Sqrt( FLOAT x) { return (FLOAT)sqrt( ClampDn( x, 0.0f)); }



/*
 * Trigonometrical functions
 */

//#define ANGLE_MASK 0x3fff
#define ANGLE_SNAP (0.25f)   //0x0010
// Wrap angle to be between 0 and 360 degrees
inline ANGLE WrapAngle(ANGLE a) {
  return (ANGLE) fmod( fmod(a,360.0f) + 360.0f, 360.0f);  // 0..360
}

// Normalize angle to be between -180 and +180 degrees
inline ANGLE NormalizeAngle(ANGLE a) {
  return WrapAngle(a+ANGLE_180)-ANGLE_180;
}

// math constants
static const FLOAT PI = FLOAT(3.14159265359);

// convert degrees into angle
inline ANGLE AngleDeg(FLOAT fDegrees) {
  //return ANGLE (fDegrees*ANGLE_180/FLOAT(180.0));
  return fDegrees;
}
// convert radians into angle
inline ANGLE AngleRad(FLOAT fRadians) {
  return ANGLE (fRadians*ANGLE_180/PI);
}
// convert radians into angle
inline ANGLE AngleRad(DOUBLE dRadians) {
  return ANGLE (dRadians*ANGLE_180/PI);
}
// convert angle into degrees
inline FLOAT DegAngle(ANGLE aAngle) {
  //return FLOAT (WrapAngle(aAngle)*FLOAT(180.0)/ANGLE_180);
  return WrapAngle(aAngle);
}
// convert angle into radians
inline FLOAT RadAngle(ANGLE aAngle) {
  return FLOAT (WrapAngle(aAngle)*PI/ANGLE_180);
}

ENGINE_API FLOAT Sin(ANGLE a);
ENGINE_API FLOAT Cos(ANGLE a);
ENGINE_API FLOAT Tan(ANGLE a);

inline ENGINE_API FLOAT SinFast(ANGLE a) { return (FLOAT)sin(a*(PI/ANGLE_180)); };
inline ENGINE_API FLOAT CosFast(ANGLE a) { return (FLOAT)cos(a*(PI/ANGLE_180)); };
inline ENGINE_API FLOAT TanFast(ANGLE a) { return (FLOAT)tan(a*(PI/ANGLE_180)); };

inline ANGLE ASin(FLOAT y) {
  return AngleRad (asin(Clamp(y, -1.0f, 1.0f)));
}
inline ANGLE ASin(DOUBLE y) {
  return AngleRad (asin(Clamp(y, -1.0, 1.0)));
}
inline ANGLE ACos(FLOAT x) {
  return AngleRad (acos(Clamp(x, -1.0f, 1.0f)));
}
inline ANGLE ACos(DOUBLE x) {
  return AngleRad (acos(Clamp(x, -1.0, 1.0)));
}
inline ANGLE ATan(FLOAT z) {
  return AngleRad (atan(z));
}
inline ANGLE ATan(DOUBLE z) {
  return AngleRad (atan(z));
}
inline ANGLE ATan2(FLOAT y, FLOAT x) {
  return AngleRad (atan2(y, x));
}
inline ANGLE ATan2(DOUBLE y, DOUBLE x) {
  return AngleRad (atan2(y, x));
}

// does "snap to grid" for given coordinate
ENGINE_API void Snap( FLOAT &fDest, FLOAT fStep);
ENGINE_API void Snap( DOUBLE &fDest, DOUBLE fStep);
// does "snap to grid" for given angle
//ENGINE_API void Snap( ANGLE &angDest, ANGLE angStep);


/* 
 *  linear interpolation, special functions for floats and angles
 */

inline FLOAT LerpFLOAT(FLOAT f0, FLOAT f1, FLOAT fFactor)
{
  return f0+(f1-f0)*fFactor;
}

inline ANGLE LerpANGLE(ANGLE a0, ANGLE a1, FLOAT fFactor)
{
  // calculate delta
  ANGLE aDelta = WrapAngle(a1)-WrapAngle(a0);
  // adjust delta not to wrap around 360
  if (aDelta>ANGLE_180) {
    aDelta-=ANGLE(ANGLE_360);
  } else if (aDelta<-ANGLE_180) {
    aDelta+=ANGLE(ANGLE_360);
  }
  // interpolate the delta
  return a0+ANGLE(fFactor*aDelta);
}

// Calculates ratio function /~~\ where 0<x<1, taking in consideration fade in and fade out percentages
// (ie. 0.2f means 20% fade in, 0.1f stands for 10% fade out)
inline FLOAT CalculateRatio(FLOAT fCurr, FLOAT fMin, FLOAT fMax, FLOAT fFadeInRatio, FLOAT fFadeOutRatio)
{
  if(fCurr<=fMin || fCurr>=fMax)
  {
    return 0.0f;
  }
  FLOAT fDelta = fMax-fMin;
  FLOAT fRatio=(fCurr-fMin)/fDelta;
  if(fRatio<fFadeInRatio) {
    fRatio = Clamp( fRatio/fFadeInRatio, 0.0f, 1.0f);
  } else if(fRatio>(1-fFadeOutRatio)) {
    fRatio = Clamp( (1.0f-fRatio)/fFadeOutRatio, 0.0f, 1.0f);
  } else {
    fRatio = 1.0f;
  }
  return fRatio;
}


#undef O
#undef Q
#undef D
#undef W
#undef B


#endif  /* include-once check. */

