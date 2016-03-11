/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Base/Relations.h>

#include <Engine/Base/ListIterator.inl>


/////////////////////////////////////////////////////////////////////
// CRelationSrc

// Construction/destruction.
CRelationSrc::CRelationSrc(void)
{
}

CRelationSrc::~CRelationSrc(void)
{
  Clear();
}

void CRelationSrc::Clear(void)
{
  // just delete all links, they will unlink on destruction
  FORDELETELIST(CRelationLnk, rl_lnSrc, *this, itlnk) {
    delete &*itlnk;
  }
}

/////////////////////////////////////////////////////////////////////
// CRelationDst

// Construction/destruction.
CRelationDst::CRelationDst(void)
{
}

CRelationDst::~CRelationDst(void)
{
  Clear();
}

void CRelationDst::Clear(void)
{
  // just delete all links, they will unlink on destruction
  FORDELETELIST(CRelationLnk, rl_lnDst, *this, itlnk) {
    delete &*itlnk;
  }
}

/////////////////////////////////////////////////////////////////////
// CRelationLnk

// Construction/destruction.
CRelationLnk::CRelationLnk(void)
{
}

CRelationLnk::~CRelationLnk(void)
{
  // unlink from both domain and codomain members
  rl_lnSrc.Remove();
  rl_lnDst.Remove();
}

// Get the domain member of this pair.
CRelationSrc &CRelationLnk::GetSrc(void)
{
  return *rl_prsSrc;
}

// Get the codomain member of this pair.
CRelationDst &CRelationLnk::GetDst(void)
{
  return *rl_prdDst;
}

// Global functions for creating relations.
void AddRelationPair(CRelationSrc &rsSrc, CRelationDst &rdDst)
{
  // create a new link
  CRelationLnk &lnk = *new CRelationLnk;
  lnk.rl_prsSrc = &rsSrc;
  lnk.rl_prdDst = &rdDst;
  // add the link to the domain and codomain members
  rsSrc.AddTail(lnk.rl_lnSrc);
  rdDst.AddTail(lnk.rl_lnDst);
}
void AddRelationPairTailTail(CRelationSrc &rsSrc, CRelationDst &rdDst)
{
  // create a new link
  CRelationLnk &lnk = *new CRelationLnk;
  lnk.rl_prsSrc = &rsSrc;
  lnk.rl_prdDst = &rdDst;
  // add the link to the domain and codomain members
  rsSrc.AddTail(lnk.rl_lnSrc);
  rdDst.AddTail(lnk.rl_lnDst);
}
void  AddRelationPairHeadHead(CRelationSrc &rsSrc, CRelationDst &rdDst)
{
  // create a new link
  CRelationLnk &lnk = *new CRelationLnk;
  lnk.rl_prsSrc = &rsSrc;
  lnk.rl_prdDst = &rdDst;
  // add the link to the domain and codomain members
  rsSrc.AddHead(lnk.rl_lnSrc);
  rdDst.AddHead(lnk.rl_lnDst);
}
