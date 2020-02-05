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

#ifndef SE_INCL_SVKSAMPLERSTATES_H
#define SE_INCL_SVKSAMPLERSTATES_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#ifdef SE1_VULKAN

typedef uint32_t SvkSamplerFlags;

static const SvkSamplerFlags SVK_TSS_FILTER_MIN_LINEAR      = 0 << 0;
static const SvkSamplerFlags SVK_TSS_FILTER_MIN_NEAREST     = 1 << 0;
static const SvkSamplerFlags SVK_TSS_FILTER_MIN_BITS        = 1 << 0;

static const SvkSamplerFlags SVK_TSS_FILTER_MAG_LINEAR      = 0 << 1;
static const SvkSamplerFlags SVK_TSS_FILTER_MAG_NEAREST     = 1 << 1;
static const SvkSamplerFlags SVK_TSS_FILTER_MAG_BITS        = 1 << 1;

static const SvkSamplerFlags SVK_TSS_WRAP_U_REPEAT          = 0 << 2;
static const SvkSamplerFlags SVK_TSS_WRAP_U_CLAMP           = 1 << 2;
static const SvkSamplerFlags SVK_TSS_WRAP_U_BITS            = 1 << 2;

static const SvkSamplerFlags SVK_TSS_WRAP_V_REPEAT          = 0 << 3;
static const SvkSamplerFlags SVK_TSS_WRAP_V_CLAMP           = 1 << 3;
static const SvkSamplerFlags SVK_TSS_WRAP_V_BITS            = 1 << 3;

static const SvkSamplerFlags SVK_TSS_MIPMAP_LINEAR          = 0 << 4;
static const SvkSamplerFlags SVK_TSS_MIPMAP_NEAREST         = 1 << 4;
static const SvkSamplerFlags SVK_TSS_MIPMAP_BITS            = 1 << 4;

static const SvkSamplerFlags SVK_TSS_ANISOTROPY_0           = 0 << 5;
static const SvkSamplerFlags SVK_TSS_ANISOTROPY_2           = 1 << 5;
static const SvkSamplerFlags SVK_TSS_ANISOTROPY_4           = 2 << 5;
static const SvkSamplerFlags SVK_TSS_ANISOTROPY_8           = 3 << 5;
static const SvkSamplerFlags SVK_TSS_ANISOTROPY_16          = 4 << 5;
static const SvkSamplerFlags SVK_TSS_ANISOTROPY_BITS        = 7 << 5;

#endif
#endif