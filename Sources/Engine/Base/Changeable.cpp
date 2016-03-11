/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Base/Changeable.h>
#include <Engine/Base/ChangeableRT.h>
#include <Engine/Base/Updateable.h>
#include <Engine/Base/UpdateableRT.h>
#include <Engine/Base/Timer.h>

/*
 * Constructor.
 */
CChangeable::CChangeable(void)
{
  ch_LastChangeTime = TIME(-1);
}

/*
 * Mark that something has changed in this object.
 */
void CChangeable::MarkChanged(void)
{
  ch_LastChangeTime = _pTimer->CurrentTick();
}

/*
 * Test if some updateable object is up to date with this changeable.
 */
BOOL CChangeable::IsUpToDate(const CUpdateable &ud) const
{
  return ch_LastChangeTime < ud.LastUpdateTime();
}

/*
 * Constructor.
 */
CChangeableRT::CChangeableRT(void)
{
  ch_LastChangeTime = TIME(-1);
}

/*
 * Mark that something has changed in this object.
 */
void CChangeableRT::MarkChanged(void)
{
  ch_LastChangeTime = _pTimer->GetRealTimeTick();
}

/*
 * Test if some updateable object is up to date with this changeable.
 */
BOOL CChangeableRT::IsUpToDate(const CUpdateableRT &ud) const
{
  return ch_LastChangeTime < ud.LastUpdateTime();
}

