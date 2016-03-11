/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Base/Updateable.h>
#include <Engine/Base/UpdateableRT.h>
#include <Engine/Base/Timer.h>

/*
 * Constructor.
 */
CUpdateable::CUpdateable(void)
{
  ud_LastUpdateTime = TIME(-1);
}

/*
 * Mark that the object has been updated.
 */
void CUpdateable::MarkUpdated(void)
{
  ud_LastUpdateTime = _pTimer->CurrentTick();
}

/*
 * Get time when last updated.
 */
TIME CUpdateable::LastUpdateTime(void) const
{
  return ud_LastUpdateTime;
}

/* Mark that the object has become invalid in spite of its time stamp. */
void CUpdateable::Invalidate(void)
{
  ud_LastUpdateTime = TIME(-1);
}

/*
 * Constructor.
 */
CUpdateableRT::CUpdateableRT(void)
{
  ud_LastUpdateTime = TIME(-1);
}

/*
 * Mark that the object has been updated.
 */
void CUpdateableRT::MarkUpdated(void)
{
  ud_LastUpdateTime = _pTimer->GetRealTimeTick();
}

/*
 * Get time when last updated.
 */
TIME CUpdateableRT::LastUpdateTime(void) const
{
  return ud_LastUpdateTime;
}

/* Mark that the object has become invalid in spite of its time stamp. */
void CUpdateableRT::Invalidate(void)
{
  ud_LastUpdateTime = TIME(-1);
}
