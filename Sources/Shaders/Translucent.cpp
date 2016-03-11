/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"

#define TEXTURE_COUNT 1
#define UVMAPS_COUNT  1
#define COLOR_COUNT   1
#define FLOAT_COUNT   0
#define FLAGS_COUNT   2

#define BASE_TEXTURE 0
#define BASE_UVMAP   0
#define BASE_COLOR   0
#define BASE_FLOAT   0

SHADER_MAIN(Translucent)
{
  shaSetTexture(BASE_TEXTURE);
  shaSetTextureWrapping( GFX_REPEAT, GFX_REPEAT);
  shaSetUVMap(BASE_UVMAP);
  shaSetColor(BASE_COLOR);
  shaDepthFunc(GFX_LESS_EQUAL);

  shaCalculateLight();

  shaBlendFunc(GFX_SRC_ALPHA, GFX_INV_SRC_ALPHA);
  shaEnableDepthTest();
  shaEnableBlend();
  shaDisableDepthWrite();

  shaModifyColorForFog();
  if(shaOverBrightningEnabled()) shaSetTextureModulation(2);

  BOOL bDoubleSided = shaGetFlags()&BASE_DOUBLE_SIDED;
  if(bDoubleSided) {
    shaCullFace(GFX_FRONT);
    shaRender();
  }

  shaCullFace(GFX_BACK);
  shaRender();
  if(shaOverBrightningEnabled()) shaSetTextureModulation(1);
}

SHADER_DESC(Translucent,ShaderDesc &shDesc)
{
  shDesc.sd_astrTextureNames.New(TEXTURE_COUNT);
  shDesc.sd_astrTexCoordNames.New(UVMAPS_COUNT);
  shDesc.sd_astrColorNames.New(COLOR_COUNT);
  shDesc.sd_astrFloatNames.New(FLOAT_COUNT);
  shDesc.sd_astrFlagNames.New(FLAGS_COUNT);

  shDesc.sd_astrTextureNames[0] = "Translucent texture";
  shDesc.sd_astrTexCoordNames[0] = "Translucent uvmap";
  shDesc.sd_astrColorNames[0] = "Translucent color";
  shDesc.sd_astrFlagNames[0] = "Double sided";
  shDesc.sd_astrFlagNames[1] = "Full bright";
  shDesc.sd_strShaderInfo = "Translucent shader";
}