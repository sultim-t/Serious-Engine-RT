/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"

#undef TEXTURE_COUNT
#undef UVMAPS_COUNT
#undef COLOR_COUNT
#undef FLOAT_COUNT
#undef FLAGS_COUNT

#define TEXTURE_COUNT 0
#define UVMAPS_COUNT  0
#define COLOR_COUNT   0
#define FLOAT_COUNT   0

SHADER_MAIN(Invisible)
{
}

SHADER_DESC(Invisible,ShaderDesc &shDesc)
{
  shDesc.sd_astrTextureNames.New(TEXTURE_COUNT);
  shDesc.sd_astrTexCoordNames.New(UVMAPS_COUNT);
  shDesc.sd_astrColorNames.New(COLOR_COUNT);
  shDesc.sd_astrFloatNames.New(FLOAT_COUNT);
}