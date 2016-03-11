/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ALLOCATIONARRAY_H
#define SE_INCL_ALLOCATIONARRAY_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Templates/StaticStackArray.h>

/*
 * Template class for stack-like array with static allocation of objects.
 */
template<class Type>
class CAllocationArray : public CStaticArray<Type> {
public:
  CStaticStackArray<INDEX> aa_aiFreeElements; // array of indices of free elements
  INDEX aa_ctAllocationStep;  // how many elements to allocate when pool overflows
public:
  /* Default constructor. */
  inline CAllocationArray(void);
  /* Destructor. */
  inline ~CAllocationArray(void);

  /* Set how many elements to allocate when pool overflows. */
  inline void SetAllocationStep(INDEX ctStep);
  /* Create a given number of objects - do not use. */
  inline void New(INDEX iCount);
  /* Destroy all objects - do not use. */
  inline void Delete(void);
  /* Destroy all objects, and reset the array to initial (empty) state. */
  inline void Clear(void);

  /* Alocate a new object. */
  inline INDEX Allocate(void);
  /* Free object with given index. */
  inline void Free(INDEX iToFree);
  /* Free all objects, but keep pool space. */
  inline void FreeAll(void);

  // check if an index is allocated (slow!)
  inline BOOL IsAllocated(INDEX i);

  /* Random access operator. */
  inline Type &operator[](INDEX iObject);
  inline const Type &operator[](INDEX iObject) const;
  /* Get number of allocated objects in array. */
  INDEX Count(void) const;
  /* Get index of a object from it's pointer. */
  INDEX Index(Type *ptObject);

  /* Assignment operator. */
  CAllocationArray<Type> &operator=(const CAllocationArray<Type> &aaOriginal);
};


#endif  /* include-once check. */

