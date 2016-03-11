/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ERRORTABLE_H
#define SE_INCL_ERRORTABLE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

struct ErrorCode {
  SLONG ec_Code;        // error code value
  char *ec_Name;        // error code constant name (in .h files)
  char *ec_Description; // error description (in help files)
};

struct ErrorTable {
  INDEX et_Count;               // number of errors
  struct ErrorCode *et_Errors;  // array of error codes
};

// macro for defining error codes
#define ERRORCODE(code, description) {code, #code, description}
// macro for defining error table
#define ERRORTABLE(errorcodes) {sizeof(errorcodes)/sizeof(struct ErrorCode), errorcodes}
/* Get the name string for error code. */
ENGINE_API extern const char *ErrorName(const struct ErrorTable *pet, SLONG slErrCode);
/* Get the description string for error code. */
ENGINE_API extern const char *ErrorDescription(const struct ErrorTable *pet, SLONG slErrCode);


#endif  /* include-once check. */

