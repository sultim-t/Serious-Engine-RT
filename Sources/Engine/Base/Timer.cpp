/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "stdh.h"

#include <Engine/Base/Timer.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/Translation.h>

#include <Engine/Base/Registry.h>
#include <Engine/Base/Profiling.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Statistics_Internal.h>

#include <Engine/Base/ListIterator.inl>
#include <Engine/Base/Priority.inl>

// Read the Pentium TimeStampCounter
static inline __int64 ReadTSC(void)
{
  __int64 mmRet;
  __asm {
    rdtsc
    mov   dword ptr [mmRet+0],eax
    mov   dword ptr [mmRet+4],edx
  }
  return mmRet;
}


// link with Win-MultiMedia
#pragma comment(lib, "winmm.lib")

// current game time always valid for the currently active task
static _declspec(thread) TIME _CurrentTickTimer = 0.0f;

// CTimer implementation

// pointer to global timer object
CTimer *_pTimer = NULL;

const TIME CTimer::TickQuantum = TIME(1/20.0);    // 20 ticks per second

/*
 * Timer interrupt callback function.
 */
/*
  NOTE:
  This function is a bit more complicated than it could be, because
  it has to deal with a feature in the windows multimedia timer that
  is undesired here.
  That is the fact that, if the timer function is stalled for a while,
  because some other thread or itself took too much time, the timer function
  is called more times to catch up with the hardware clock.
  This can cause complete lockout if timer handlers constantly consume more
  time than is available between two calls of timer function.
  As a workaround, this function measures hardware time and refuses to call
  the handlers if it is not on time.

  In effect, if some timer handler starts spending too much time, the
  handlers are called at lower frequency until the application (hopefully)
  stabilizes.

  When such a catch-up situation occurs, 'real time' timer still keeps
  more or less up to date with the hardware time, but the timer handlers
  skip some ticks. E.g. if timer handlers start spending twice more time
  than is tick quantum, they get called approx. every two ticks.

  EXTRA NOTE:
  Had to disable that, because it didn't work well (caused jerking) on 
  Win95 osr2 with no patches installed!
*/
void CTimer_TimerFunc_internal(void)
{
// Access to stream operations might be invoked in timer handlers, but
// this is disabled for now. Should also synchronize access to list of
// streams and to group file before enabling that!
//  CTSTREAM_BEGIN {

    // increment the 'real time' timer
    _pTimer->tm_RealTimeTimer += _pTimer->TickQuantum;

    // get the current time for real and in ticks
    CTimerValue tvTimeNow = _pTimer->GetHighPrecisionTimer();
    TIME        tmTickNow = _pTimer->tm_RealTimeTimer;
    // calculate how long has passed since we have last been on time
    TIME tmTimeDelay = (TIME)(tvTimeNow - _pTimer->tm_tvLastTimeOnTime).GetSeconds();
    TIME tmTickDelay =       (tmTickNow - _pTimer->tm_tmLastTickOnTime);

    _sfStats.StartTimer(CStatForm::STI_TIMER);
    // if we are keeping up to time (more or less)
//    if (tmTimeDelay>=_pTimer->TickQuantum*0.9f) {

      // for all hooked handlers
      FOREACHINLIST(CTimerHandler, th_Node, _pTimer->tm_lhHooks, itth) {
        // handle
        itth->HandleTimer();
      }
//    }
    _sfStats.StopTimer(CStatForm::STI_TIMER);

    // remember that we have been on time now
    _pTimer->tm_tvLastTimeOnTime = tvTimeNow;
    _pTimer->tm_tmLastTickOnTime = tmTickNow;

//  } CTSTREAM_END;
}
void __stdcall CTimer_TimerFunc(UINT uID, UINT uMsg, ULONG dwUser, ULONG dw1, ULONG dw2)
{
  // access to the list of handlers must be locked
  CTSingleLock slHooks(&_pTimer->tm_csHooks, TRUE);
  // handle all timers
  CTimer_TimerFunc_internal();
}


#pragma inline_depth()

#define MAX_MEASURE_TRIES 5
static INDEX _aiTries[MAX_MEASURE_TRIES];

// Get processor speed in Hertz
static __int64 GetCPUSpeedHz(void)
{
  // get the frequency of the 'high' precision timer
  __int64 llTimerFrequency;
  BOOL bPerformanceCounterPresent = QueryPerformanceFrequency((LARGE_INTEGER*)&llTimerFrequency);
  // fail if the performance counter is not available on this system
  if( !bPerformanceCounterPresent) {
    CPrintF( TRANS("PerformanceTimer is not available!\n"));
    return 1;
  }

  INDEX iSpeed, iTry;
  INDEX ctTotalFaults=0;
  __int64 llTimeLast, llTimeNow;
  __int64 llCPUBefore, llCPUAfter; 
  __int64 llTimeBefore, llTimeAfter;
  __int64 llSpeedMeasured;

  // try to measure 10 times
  INDEX iSet=0;
  for( ; iSet<10; iSet++)
  { // one time has several tries
    for( iTry=0; iTry<MAX_MEASURE_TRIES; iTry++)
    { // wait the state change on the timer
      QueryPerformanceCounter((LARGE_INTEGER*)&llTimeNow);
      do {
        llTimeLast = llTimeNow;
        QueryPerformanceCounter((LARGE_INTEGER*)&llTimeNow);
      } while( llTimeLast==llTimeNow);
      // wait for some time, and count the CPU clocks passed
      llCPUBefore  = ReadTSC();
      llTimeBefore = llTimeNow;
      llTimeAfter  = llTimeNow + llTimerFrequency/4;
      do {
        QueryPerformanceCounter((LARGE_INTEGER*)&llTimeNow);
      } while( llTimeNow<llTimeAfter );
      llCPUAfter = ReadTSC();
      // calculate the CPU clock frequency from gathered data
      llSpeedMeasured = (llCPUAfter-llCPUBefore)*llTimerFrequency / (llTimeNow-llTimeBefore);
      _aiTries[iTry]  = llSpeedMeasured/1000000;
    }
    // see if we had good measurement
    INDEX ctFaults = 0;
    iSpeed = _aiTries[0];
    const INDEX iTolerance = iSpeed *1/100; // %1 tolerance should be enough
    for( iTry=1; iTry<MAX_MEASURE_TRIES; iTry++) {
      if( abs(iSpeed-_aiTries[iTry]) > iTolerance) ctFaults++;
    }
    // done if no faults
    if( ctFaults==0) break;
    Sleep(1000);
  }

  // fail if couldn't readout CPU speed
  if( iSet==10) {
    CPrintF( TRANS("PerformanceTimer is not vaild!\n"));
    //return 1; 
    // NOTE: this function must never fail, or the engine will crash! 
    // if this failed, the speed will be read from registry (only happens on Win2k)
  }

  // keep readout speed and read speed from registry
  const SLONG slSpeedRead = _aiTries[0];
  SLONG slSpeedReg = 0;
  BOOL bFoundInReg = REG_GetLong("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\\~MHz", (ULONG&)slSpeedReg);

  // if not found in registry
  if( !bFoundInReg) {
    // use measured
    CPrintF(TRANS("  CPU speed not found in registry, using calculated value\n\n"));
    return (__int64)slSpeedRead*1000000;
  // if found in registry
  } else {
    // if different than measured
    const INDEX iTolerance = slSpeedRead *1/100; // %1 tolerance should be enough
    if( abs(slSpeedRead-slSpeedReg) > iTolerance) {
      // report warning and use registry value
      CPrintF(TRANS("  WARNING: calculated CPU speed different than stored in registry!\n\n"));
      return (__int64)slSpeedReg*1000000;
    }
    // use measured value
    return (__int64)slSpeedRead*1000000;
  }
}


/*
 * Constructor.
 */
CTimer::CTimer(BOOL bInterrupt /*=TRUE*/)
{
  tm_csHooks.cs_iIndex = 1000;
  // set global pointer
  ASSERT(_pTimer == NULL);
  _pTimer = this;
  tm_bInterrupt = bInterrupt;

  { // this part of code must be executed as precisely as possible
    CSetPriority sp(REALTIME_PRIORITY_CLASS, THREAD_PRIORITY_TIME_CRITICAL);
    tm_llCPUSpeedHZ = GetCPUSpeedHz();
    tm_llPerformanceCounterFrequency = tm_llCPUSpeedHZ;

    // measure profiling errors and set epsilon corrections
    CProfileForm::CalibrateProfilingTimers();
  }

  // clear counters
  _CurrentTickTimer = TIME(0);
  tm_RealTimeTimer = TIME(0);

  tm_tmLastTickOnTime = TIME(0);
  tm_tvLastTimeOnTime = GetHighPrecisionTimer();
  // disable lerping by default
  tm_fLerpFactor = 1.0f;
  tm_fLerpFactor2 = 1.0f;

  // start interrupt (eventually)
  if( tm_bInterrupt)
  {
    tm_TimerID = timeSetEvent(
      ULONG(TickQuantum*1000.0f),	  // period value [ms]
      0,	                          // resolution (0==max. possible)
      &CTimer_TimerFunc,	          // callback
      0,                            // user
      TIME_PERIODIC);               // event type

    // check that interrupt was properly started
    if( tm_TimerID==NULL) FatalError(TRANS("Cannot initialize multimedia timer!"));

    // make sure that timer interrupt is ticking
    INDEX iTry=1;
    for( ; iTry<=3; iTry++) {
      const TIME tmTickBefore = GetRealTimeTick();
      Sleep(1000* iTry*3 *TickQuantum);
      const TIME tmTickAfter = GetRealTimeTick();
      if( tmTickBefore!=tmTickAfter) break;
      Sleep(1000*iTry);
    }
    // report fatal
    if( iTry>3) FatalError(TRANS("Problem with initializing multimedia timer - please try again."));
  }
}

/*
 * Destructor.
 */
CTimer::~CTimer(void)
{
  ASSERT(_pTimer == this);

  // destroy timer
  if (tm_bInterrupt) {
    ASSERT(tm_TimerID!=NULL);
    ULONG rval = timeKillEvent(tm_TimerID);
    ASSERT(rval == TIMERR_NOERROR);
  }
  // check that all handlers have been removed
  ASSERT(tm_lhHooks.IsEmpty());

  // clear global pointer
  _pTimer = NULL;
}

/*
 * Add a timer handler.
 */
void CTimer::AddHandler(CTimerHandler *pthNew)
{
  // access to the list of handlers must be locked
  CTSingleLock slHooks(&tm_csHooks, TRUE);

  ASSERT(this!=NULL);
  tm_lhHooks.AddTail(pthNew->th_Node);
}

/*
 * Remove a timer handler.
 */
void CTimer::RemHandler(CTimerHandler *pthOld)
{
  // access to the list of handlers must be locked
  CTSingleLock slHooks(&tm_csHooks, TRUE);

  ASSERT(this!=NULL);
  pthOld->th_Node.Remove();
}

/* Handle timer handlers manually. */
void CTimer::HandleTimerHandlers(void)
{
  // access to the list of handlers must be locked
  CTSingleLock slHooks(&_pTimer->tm_csHooks, TRUE);
  // handle all timers
  CTimer_TimerFunc_internal();
}


/*
 * Set the real time tick value.
 */
void CTimer::SetRealTimeTick(TIME tNewRealTimeTick)
{
  ASSERT(this!=NULL);
  tm_RealTimeTimer = tNewRealTimeTick;
}

/*
 * Get the real time tick value.
 */
TIME CTimer::GetRealTimeTick(void) const
{
  ASSERT(this!=NULL);
  return tm_RealTimeTimer;
}

/*
 * Set the current game tick used for time dependent tasks (animations etc.).
 */
void CTimer::SetCurrentTick(TIME tNewCurrentTick) {
  ASSERT(this!=NULL);
  _CurrentTickTimer = tNewCurrentTick;
}

/*
 * Get current game time, always valid for the currently active task.
 */
const TIME CTimer::CurrentTick(void) const {
  ASSERT(this!=NULL);
  return _CurrentTickTimer;
}
const TIME CTimer::GetLerpedCurrentTick(void) const {
  ASSERT(this!=NULL);
  return _CurrentTickTimer+tm_fLerpFactor*TickQuantum;
}
// Set factor for lerping between ticks.
void CTimer::SetLerp(FLOAT fFactor) // sets both primary and secondary
{
  ASSERT(this!=NULL);
  tm_fLerpFactor = fFactor;
  tm_fLerpFactor2 = fFactor;
}
void CTimer::SetLerp2(FLOAT fFactor)  // sets only secondary
{
  ASSERT(this!=NULL);
  tm_fLerpFactor2 = fFactor;
}
// Disable lerping factor (set both factors to 1)
void CTimer::DisableLerp(void)
{
  ASSERT(this!=NULL);
  tm_fLerpFactor =1.0f;
  tm_fLerpFactor2=1.0f;
}

// convert a time value to a printable string (hh:mm:ss)
CTString TimeToString(FLOAT fTime)
{
  CTString strTime;
  int iSec = floor(fTime);
  int iMin = iSec/60;
  iSec = iSec%60;
  int iHou = iMin/60;
  iMin = iMin%60;
  strTime.PrintF("%02d:%02d:%02d", iHou, iMin, iSec);
  return strTime;
}
