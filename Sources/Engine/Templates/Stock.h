/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */


#if !defined(TYPE) || !defined(CStock_TYPE) || !defined(CNameTable_TYPE)
#error
#endif

#include <Engine/Templates/DynamicContainer.h>

/*
 * Template for stock of some kind of objects that can be saved and loaded.
 */
class CStock_TYPE {
public:
  CDynamicContainer<TYPE> st_ctObjects;   // objects on stock
  CNameTable_TYPE st_ntObjects;  // name table for fast lookup

public:
  /* Default constructor. */
  CStock_TYPE(void);
  /* Destructor. */
  ~CStock_TYPE(void);

  /* Obtain an object from stock - loads if not loaded. */
  ENGINE_API TYPE *Obtain_t(const CTFileName &fnmFileName); // throw char *
  /* Release an object when not needed any more. */
  ENGINE_API void Release(TYPE *ptObject);
  // free all unused elements of the stock
  ENGINE_API void FreeUnused(void);
  // calculate amount of memory used by all objects in the stock
  SLONG CalculateUsedMemory(void);
  // dump memory usage report to a file
  void DumpMemoryUsage_t(CTStream &strm); // throw char *
  // get number of total elements in stock
  INDEX GetTotalCount(void);
  // get number of used elements in stock
  INDEX GetUsedCount(void);
};
