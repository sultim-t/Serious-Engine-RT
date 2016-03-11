/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"

static const INDEX ctTextures = 0;
static const INDEX ctUvmaps   = 0;
static const INDEX ctColors   = 1;
static const INDEX ctFloats   = 0;
static const INDEX ctFlags    = 2;

SHADER_MAIN(Color)
{
  shaSetTexture(-1);
  shaSetColor(0);

  shaDisableBlend();
  shaDisableAlphaTest();
  shaEnableDepthWrite();
  shaEnableDepthTest();
  shaDepthFunc(GFX_LESS_EQUAL);

  BOOL bDoubleSided = shaGetFlags()&BASE_DOUBLE_SIDED;
  BOOL bFullBright  = shaGetFlags()&BASE_FULL_BRIGHT;

  if(bDoubleSided) {
    shaCullFace(GFX_NONE);
  } else {
    shaCullFace(GFX_BACK);
  }

  shaCalculateLight();
  shaRender();
}

SHADER_DESC(Color,ShaderDesc &shDesc)
{
  shDesc.sd_astrTextureNames.New(ctTextures);
  shDesc.sd_astrTexCoordNames.New(ctUvmaps);
  shDesc.sd_astrColorNames.New(ctColors);
  shDesc.sd_astrFloatNames.New(ctFloats);
  shDesc.sd_astrFlagNames.New(ctFlags);

  shDesc.sd_astrColorNames[0] = "Surface color";
  shDesc.sd_astrFlagNames[0] = "Double sided";
  shDesc.sd_astrFlagNames[1] = "Full bright";
  shDesc.sd_strShaderInfo = "Color shader";
}