/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Math/Float.h>

/* Get current precision setting of FPU. */
enum FPUPrecisionType GetFPUPrecision(void)
{
  // get control flags from FPU
  ULONG fpcw = _control87( 0, 0);

  // extract the precision from the flags
  switch(fpcw&_MCW_PC) {
  case _PC_24:
    return FPT_24BIT;
    break;
  case _PC_53:
    return FPT_53BIT;
    break;
  case _PC_64:
    return FPT_64BIT;
    break;
  default:
    ASSERT(FALSE);
    return FPT_24BIT;
  };
}

/* Set current precision setting of FPU. */
void SetFPUPrecision(enum FPUPrecisionType fptNew)
{
  ULONG fpcw;
  // create FPU flags from the precision
  switch(fptNew) {
  case FPT_24BIT:
    fpcw=_PC_24;
    break;
  case FPT_53BIT:
    fpcw=_PC_53;
    break;
  case FPT_64BIT:
    fpcw=_PC_64;
    break;
  default:
    ASSERT(FALSE);
    fpcw=_PC_24;
  };
  // set the FPU precission
  _control87( fpcw, MCW_PC);
}

/////////////////////////////////////////////////////////////////////
// CSetFPUPrecision
/*
 * Constructor with automatic setting of FPU precision.
 */
CSetFPUPrecision::CSetFPUPrecision(enum FPUPrecisionType fptNew)
{
  // remember old precision
  sfp_fptOldPrecision = GetFPUPrecision();
  // set new precision if needed
  sfp_fptNewPrecision = fptNew;
  if (sfp_fptNewPrecision!=sfp_fptOldPrecision) {
    SetFPUPrecision(fptNew);
  }
}

/*
 * Destructor with automatic restoring of FPU precision.
 */
CSetFPUPrecision::~CSetFPUPrecision(void)
{
  // check consistency
  ASSERT(GetFPUPrecision()==sfp_fptNewPrecision);
  // restore old precision if needed
  if (sfp_fptNewPrecision!=sfp_fptOldPrecision) {
    SetFPUPrecision(sfp_fptOldPrecision);
  }
}

BOOL IsValidFloat(float f)
{
  return _finite(f) && (*(ULONG*)&f)!=0xcdcdcdcdUL;
/*  int iClass = _fpclass(f);
  return
    iClass==_FPCLASS_NN ||
    iClass==_FPCLASS_ND ||
    iClass==_FPCLASS_NZ ||
    iClass==_FPCLASS_PZ ||
    iClass==_FPCLASS_PD ||
    iClass==_FPCLASS_PN;
    */
}

BOOL IsValidDouble(double f)
{
  return _finite(f) && (*(unsigned __int64*)&f)!=0xcdcdcdcdcdcdcdcdI64;
/*  int iClass = _fpclass(f);
  return
    iClass==_FPCLASS_NN ||
    iClass==_FPCLASS_ND ||
    iClass==_FPCLASS_NZ ||
    iClass==_FPCLASS_PZ ||
    iClass==_FPCLASS_PD ||
    iClass==_FPCLASS_PN;
    */
}

