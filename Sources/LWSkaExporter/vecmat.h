/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

/*
======================================================================
vecmat.h

Basic vector and matrix functions.
====================================================================== */

#include <lwtypes.h>

#define vecangle(a,b) (float)acos(dot(a,b))    /* a and b must be unit vectors */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


float dot( LWFVector a, LWFVector b );
void cross( LWFVector a, LWFVector b, LWFVector c );
void normalize( LWFVector v );

#ifdef __cplusplus
}
#endif //__cplusplus
