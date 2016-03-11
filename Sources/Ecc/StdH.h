/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <math.h>

#ifdef PLATFORM_UNIX
#include <errno.h>
#include <sys/param.h>
#include <unistd.h>
#define _fullpath(x, y, z) realpath(y, x)
#endif

