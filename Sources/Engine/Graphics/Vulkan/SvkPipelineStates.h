#ifndef SE_INCL_SVKPIPELINESTATES_H
#define SE_INCL_SVKPIPELINESTATES_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

typedef uint32_t SvkPipelineStateFlags;

static const SvkPipelineStateFlags SVK_PLS_POLYGON_MODE_FILL                        = 0 << 0;
static const SvkPipelineStateFlags SVK_PLS_POLYGON_MODE_LINE                        = 1 << 0;
static const SvkPipelineStateFlags SVK_PLS_POLYGON_MODE_POINT                       = 2 << 0;
static const SvkPipelineStateFlags SVK_PLS_POLYGON_MODE_BITS                        = 3 << 0;

static const SvkPipelineStateFlags SVK_PLS_CULL_MODE_BACK                           = 0 << 2;
static const SvkPipelineStateFlags SVK_PLS_CULL_MODE_FRONT                          = 1 << 2;
static const SvkPipelineStateFlags SVK_PLS_CULL_MODE_NONE                           = 2 << 2;
static const SvkPipelineStateFlags SVK_PLS_CULL_MODE_BITS                           = 3 << 2;

static const SvkPipelineStateFlags SVK_PLS_FRONT_FACE_COUNTER_CLOCKWISE             = 0 << 4;
static const SvkPipelineStateFlags SVK_PLS_FRONT_FACE_CLOCKWISE                     = 1 << 4;
static const SvkPipelineStateFlags SVK_PLS_FRONT_FACE_BITS                          = 1 << 4;

static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_NEVER                   = 0 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_LESS                    = 1 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_EQUAL                   = 2 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_LESS_OR_EQUAL           = 3 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_GREATER                 = 4 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_NOT_EQUAL               = 5 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_GREATER_OR_EQUAL        = 6 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_ALWAYS                  = 7 << 5;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_COMPARE_OP_BITS                    = 7 << 5;

static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_ZERO                    = 0 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_ONE                     = 1 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_SRC_COLOR               = 2 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_SRC_COLOR     = 3 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_DST_COLOR               = 4 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_DST_COLOR     = 5 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_SRC_ALPHA               = 6 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA     = 7 << 8;
static const SvkPipelineStateFlags SVK_PLS_SRC_BLEND_FACTOR_BITS                    = 7 << 8;

static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_ZERO                    = 0 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_ONE                     = 1 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_SRC_COLOR               = 2 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_SRC_COLOR     = 3 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_DST_COLOR               = 4 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_DST_COLOR     = 5 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_SRC_ALPHA               = 6 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA     = 7 << 11;
static const SvkPipelineStateFlags SVK_PLS_DST_BLEND_FACTOR_BITS                    = 7 << 11;

static const SvkPipelineStateFlags SVK_PLS_BLEND_OP_ADD                             = 0 << 14;
static const SvkPipelineStateFlags SVK_PLS_BLEND_OP_SUBTRACT                        = 1 << 14;
static const SvkPipelineStateFlags SVK_PLS_BLEND_OP_REVERSE_SUBTRACT                = 2 << 14;
static const SvkPipelineStateFlags SVK_PLS_BLEND_OP_MIN                             = 3 << 14;
static const SvkPipelineStateFlags SVK_PLS_BLEND_OP_MAX                             = 4 << 14;
static const SvkPipelineStateFlags SVK_PLS_BLEND_OP_BITS                            = 7 << 14;

static const SvkPipelineStateFlags SVK_PLS_COLOR_WRITE_MASK_R_BIT                   = 1 << 17;
static const SvkPipelineStateFlags SVK_PLS_COLOR_WRITE_MASK_G_BIT                   = 1 << 18;
static const SvkPipelineStateFlags SVK_PLS_COLOR_WRITE_MASK_B_BIT                   = 1 << 19;
static const SvkPipelineStateFlags SVK_PLS_COLOR_WRITE_MASK_A_BIT                   = 1 << 20;

// bools
static const SvkPipelineStateFlags SVK_PLS_DEPTH_BIAS_BOOL                          = 1 << 21;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_TEST_BOOL                          = 1 << 22;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_WRITE_BOOL                         = 1 << 23;
static const SvkPipelineStateFlags SVK_PLS_DEPTH_BOUNDS_BOOL                        = 1 << 24;
static const SvkPipelineStateFlags SVK_PLS_BLEND_ENABLE_BOOL                        = 1 << 25;
static const SvkPipelineStateFlags SVK_PLS_ALPHA_ENABLE_BOOL                        = 1 << 26;

// additional
static const SvkPipelineStateFlags SVK_PLS_COLOR_WRITE_MASK_RGB =
  SVK_PLS_COLOR_WRITE_MASK_R_BIT | SVK_PLS_COLOR_WRITE_MASK_G_BIT | SVK_PLS_COLOR_WRITE_MASK_B_BIT;
static const SvkPipelineStateFlags SVK_PLS_COLOR_WRITE_MASK_RGBA = 
  SVK_PLS_COLOR_WRITE_MASK_RGB | SVK_PLS_COLOR_WRITE_MASK_A_BIT;

static const SvkPipelineStateFlags SVK_PLS_DEFAULT_FLAGS = 
  SVK_PLS_COLOR_WRITE_MASK_RGBA | SVK_PLS_DEPTH_COMPARE_OP_LESS |
  SVK_PLS_DEPTH_TEST_BOOL | SVK_PLS_DEPTH_WRITE_BOOL;

#endif