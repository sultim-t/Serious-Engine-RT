/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_FIELDSETTINGS_H
#define SE_INCL_FIELDSETTINGS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Graphics/Texture.h>

/*
 * Structure describing properties of a field entity.
 */

class ENGINE_API CFieldSettings {
public:
  CTextureObject fs_toTexture;    // texture for rendering field in WEd
  COLOR fs_colColor;              // color for rendering field in WEd
};


#endif  /* include-once check. */

