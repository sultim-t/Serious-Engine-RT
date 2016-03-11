/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_FLOAT_H
#define SE_INCL_FLOAT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif


/*
 * FPU control functions and classes.
 */

// FPU precision setting values
enum FPUPrecisionType {
  FPT_24BIT,
  FPT_53BIT,
  FPT_64BIT,
};
/* Get current precision setting of FPU. */
ENGINE_API enum FPUPrecisionType GetFPUPrecision(void);
/* Set current precision setting of FPU. */
ENGINE_API void SetFPUPrecision(enum FPUPrecisionType fptNew);

/*
 * Class that provides automatic saving/setting/restoring of FPU precision setting.
 */
class ENGINE_API CSetFPUPrecision {
private:
  enum FPUPrecisionType sfp_fptOldPrecision;
  enum FPUPrecisionType sfp_fptNewPrecision;
public:
  /* Constructor with automatic setting of FPU precision. */
  CSetFPUPrecision(enum FPUPrecisionType fptNew);
  /* Destructor with automatic restoring of FPU precision. */
  ~CSetFPUPrecision(void);
};


ENGINE_API BOOL IsValidFloat(float f);
ENGINE_API BOOL IsValidDouble(double f);


#endif  /* include-once check. */

