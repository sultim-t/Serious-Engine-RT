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

#include <Engine/Base/Profiling.h>

#include <Engine/Templates/StaticArray.cpp>
template CStaticArray<CProfileCounter>;
template CStaticArray<CProfileTimer>;


static inline __int64 ReadTSC_profile(void)
{
  __int64 mmRet;
  __asm {
    rdtsc
    mov   dword ptr [mmRet+0],eax
    mov   dword ptr [mmRet+4],edx
  }
  return mmRet;
}


/////////////////////////////////////////////////////////////////////
// CProfileForm
// how much it takes to start a profiling timer
CTimerValue _tvStartEpsilon;
// how much it takes to stop a profiling timer
CTimerValue _tvStopEpsilon;
// how much it takes to just start and stop a profiling timer (measure zero time)
CTimerValue _tvStartStopEpsilon;

// total sum of all timing offsets induced by all Starts and Stops so far
CTimerValue _tvCurrentProfilingEpsilon;

// Measure profiling errors and set epsilon corrections.
static CTimerValue _tvTest;
void CProfileForm::CalibrateProfilingTimers(void)
{
  enum Epsilons {
    ETI_TOTAL,
    ETI_STARTSTOP,
    ETI_START,
    ETI_STOP,
    ETI_DUMMY,
    ETI_COUNT
  };

  CProfileForm pfCalibration("", "", 0, ETI_COUNT);
  // set all epsilons to zero, so that they don't interfere with the measuring
  _tvStartEpsilon.Clear();
  _tvStopEpsilon.Clear();
  _tvStartStopEpsilon.Clear();
  _tvCurrentProfilingEpsilon.Clear();

  /* NOTE: we must use one more counter (ETI_TOTAL), so that we don't overestimate
   * time spent in StartTimer(), StopTimer() if it is the first timer started.
   */

#define REPEATCOUNT 10000
  // measure how much it takes to start and stop timer
  __int64 llMinStartStopTime(0x7fffffffffffffff);
  {for (INDEX i=0; i<REPEATCOUNT; i++) {
    pfCalibration.Reset();
    pfCalibration.StartTimer(ETI_TOTAL);
    pfCalibration.StartTimer(ETI_STARTSTOP);
    pfCalibration.StopTimer(ETI_STARTSTOP);
    pfCalibration.StopTimer(ETI_TOTAL);
    __int64 llThisStartStopTime = pfCalibration.pf_aptTimers[ETI_STARTSTOP].
      pt_tvElapsed.tv_llValue;
    if (llThisStartStopTime<llMinStartStopTime) {
      llMinStartStopTime = llThisStartStopTime;
    }
  }}
  _tvStartStopEpsilon = llMinStartStopTime;

  // measure how much it takes to start timer
  __int64 llMinStartTime(0x7fffffffffffffff);
  {for (INDEX i=0; i<REPEATCOUNT; i++) {
    pfCalibration.Reset();
    pfCalibration.StartTimer(ETI_TOTAL);
    pfCalibration.StartTimer(ETI_START);
    pfCalibration.StartTimer(ETI_DUMMY);
    pfCalibration.StopTimer(ETI_START);
    pfCalibration.StopTimer(ETI_TOTAL);
    pfCalibration.StopTimer(ETI_DUMMY);
    __int64 llThisStartTime = pfCalibration.pf_aptTimers[ETI_START].
      pt_tvElapsed.tv_llValue;
    if (llThisStartTime<llMinStartTime) {
      llMinStartTime = llThisStartTime;
    }
  }}
  _tvStartEpsilon = llMinStartTime;

  // measure how much it takes to stop timer
  __int64 llMinStopTime(0x7fffffffffffffff);
  {for (INDEX i=0; i<REPEATCOUNT; i++) {
    pfCalibration.Reset();
    pfCalibration.StartTimer(ETI_TOTAL);
    pfCalibration.StartTimer(ETI_DUMMY);
    pfCalibration.StartTimer(ETI_STOP);
    pfCalibration.StopTimer(ETI_DUMMY);
    pfCalibration.StopTimer(ETI_STOP);
    pfCalibration.StopTimer(ETI_TOTAL);
    __int64 llThisStopTime = pfCalibration.pf_aptTimers[ETI_STOP].
      pt_tvElapsed.tv_llValue;
    if (llThisStopTime<llMinStopTime) {
      llMinStopTime = llThisStopTime;
    }
  }}
  _tvStopEpsilon = llMinStopTime;

  pfCalibration.Reset();
  pfCalibration.StartTimer(ETI_TOTAL);
  pfCalibration.StartTimer(ETI_STARTSTOP);
  pfCalibration.StopTimer(ETI_STARTSTOP);
  pfCalibration.StopTimer(ETI_TOTAL);
  _tvTest = pfCalibration.pf_aptTimers[ETI_STARTSTOP].pt_tvElapsed;

}

/*
 * Constructor for profile form with given number of counters and timers.
 */
CProfileForm::CProfileForm(
  const CTString &strTitle, const CTString &strAveragingUnits,
  INDEX ctCounters, INDEX ctTimers)
{
  // remember title and averaging units
  pf_strTitle = strTitle;
  pf_strAveragingUnits = strAveragingUnits;

  // create given number of counters
  pf_apcCounters.New(ctCounters);
  // create given number of timers
  pf_aptTimers.New(ctTimers);
  // reset all values
  pf_ctRunningTimers = 0;

  // for all timers
  FOREACHINSTATICARRAY(pf_aptTimers, CProfileTimer, itpt) {
    // clear the timer
    itpt->pt_tvElapsed.Clear();
    itpt->pt_tvStarted.tv_llValue = -__int64(1);
    itpt->pt_ctAveraging = 0;
  }
}

void CProfileForm::Clear(void)
{
  pf_strTitle.Clear();
  pf_strAveragingUnits.Clear();

  pf_apcCounters.Clear();
  pf_aptTimers.Clear();
}

/*
 * Get name of a counter.
 */
const CTString &CProfileForm::GetCounterName(INDEX iCounter)
{
  return pf_apcCounters[iCounter].pc_strName;
}

/*
 * Get name of a timer.
 */
const CTString &CProfileForm::GetTimerName(INDEX iTimer)
{
  return pf_aptTimers[iTimer].pt_strName;
}

/* Start a timer. */
void CProfileForm::StartTimer_internal(INDEX iTimer)
{
  CProfileTimer &pt = pf_aptTimers[iTimer];
  //ASSERT(pt.pt_tvStarted.tv_llValue<0);
  CTimerValue tvNow = CTimerValue(ReadTSC_profile())-_tvCurrentProfilingEpsilon;
  pt.pt_tvStarted = tvNow;
  pf_ctRunningTimers++;
  if (pf_ctRunningTimers==1) {
    pf_tvOverAllStarted = tvNow;
  }
  _tvCurrentProfilingEpsilon += _tvStartEpsilon;
}

/* Stop a timer. */
void CProfileForm::StopTimer_internal(INDEX iTimer)
{
  CProfileTimer &pt = pf_aptTimers[iTimer];
  //ASSERT(pt.pt_tvStarted.tv_llValue>0);
  CTimerValue tvNow = CTimerValue(ReadTSC_profile())-_tvCurrentProfilingEpsilon;
  pt.pt_tvElapsed +=
    tvNow - pf_aptTimers[iTimer].pt_tvStarted - _tvStartStopEpsilon + _tvStartEpsilon;
  pf_ctRunningTimers--;
  if (pf_ctRunningTimers==0) {
    pf_tvOverAllElapsed += tvNow-pf_tvOverAllStarted;
  }
  IFDEBUG(pt.pt_tvStarted.tv_llValue = -__int64(1));
  _tvCurrentProfilingEpsilon += _tvStopEpsilon;
}

/* Get current value of a counter. */
INDEX CProfileForm::GetCounterCount(INDEX iCounter) {
  return pf_apcCounters[iCounter].pc_ctCount;
};

// override to provide external averaging
INDEX CProfileForm::GetAveragingCounter(void)
{
  return pf_ctAveragingCounter;
}

/* Get current value of a timer in seconds or in percentage of module time. */
double CProfileForm::GetTimerAverageTime(INDEX iTimer) {
  // must not report while some timers are active!
  ASSERT(pf_ctRunningTimers==0);
  return pf_aptTimers[iTimer].pt_tvElapsed.GetSeconds()/GetAveragingCounter();
}
double CProfileForm::GetTimerPercentageOfModule(INDEX iTimer) {
  // must not report while some timers are active!
  ASSERT(pf_ctRunningTimers==0);
  return pf_aptTimers[iTimer].pt_tvElapsed.GetSeconds()
    /pf_tvOverAllElapsed.GetSeconds()*100;
}

/* Get percentage of module time in application time. */
double CProfileForm::GetModulePercentage(void) {
  // must not report while some timers are active!
  ASSERT(pf_ctRunningTimers==0);
  // calculate total application time since profile form was reset
  CTimerValue tvNow = _pTimer->GetHighPrecisionTimer();
  CTimerValue tvApplicationElapsed = tvNow-pf_tvLastReset;
  return pf_tvOverAllElapsed.GetSeconds()/tvApplicationElapsed.GetSeconds()*100;
}

/*
 * Reset all profiling values.
 */
void CProfileForm::Reset(void)
{
  // must not reset profiling form while some timers are active!
  //ASSERT(pf_ctRunningTimers==0);

  // remember time when form was reset
  pf_tvLastReset = _pTimer->GetHighPrecisionTimer();

  // clear global entries
  pf_tvOverAllElapsed.Clear();
  pf_ctAveragingCounter = 0;

  // for all counters
  FOREACHINSTATICARRAY(pf_apcCounters, CProfileCounter, itpc) {
    // clear the counter
    itpc->pc_ctCount = 0;
  }
  // for all timers
  FOREACHINSTATICARRAY(pf_aptTimers, CProfileTimer, itpt) {
    // clear the timer
    itpt->pt_tvElapsed.Clear();
    itpt->pt_tvStarted.tv_llValue = -__int64(1);
    itpt->pt_ctAveraging = 0;
  }
}

/* Print one counter in report. */
void CProfileCounter::Report(char *&strBuffer, INDEX ctAveragingCount)
{
  if (ctAveragingCount==0) {
    ctAveragingCount = 1;
  }
  strBuffer += sprintf(strBuffer, "%-45s: %7d %7.2f\n",
    pc_strName, pc_ctCount, (double)pc_ctCount/ctAveragingCount);
}

/* Print one timer in report. */
void CProfileTimer::Report(char *&strBuffer,
                           INDEX ctAveragingCount,
                           CTimerValue tvAppElapsed, CTimerValue tvModElapsed)
{
  if (ctAveragingCount==0) {
    ctAveragingCount = 1;
  }

  if (pt_strAveragingName=="") {
    strBuffer += sprintf(strBuffer, "%-45s: %6.2f%% %6.2f%% %6.2f ms\n",
      pt_strName,
      pt_tvElapsed.GetSeconds()/tvAppElapsed.GetSeconds()*100,
      pt_tvElapsed.GetSeconds()/tvModElapsed.GetSeconds()*100,
      pt_tvElapsed.GetSeconds()/ctAveragingCount*1000
      );
  } else {
    INDEX ctLocalAveraging = pt_ctAveraging;
    if (ctLocalAveraging==0) {
      ctLocalAveraging = 1;
    }
    strBuffer += sprintf(strBuffer, "%-45s: %6.2f%% %6.2f%% %6.2f ms (%4.0fc/%s x%d)\n",
      pt_strName,
      pt_tvElapsed.GetSeconds()/tvAppElapsed.GetSeconds()*100,
      pt_tvElapsed.GetSeconds()/tvModElapsed.GetSeconds()*100,
      pt_tvElapsed.GetSeconds()/ctAveragingCount*1000,
      pt_tvElapsed.GetSeconds()/ctLocalAveraging*_pTimer->tm_llCPUSpeedHZ,
      pt_strAveragingName,
      pt_ctAveraging/ctAveragingCount
      );
  }
}
/*
 * Report profiling results.
 */
void CProfileForm::Report(CTString &strReport)
{
  static char aBuffer[16000];
  char *strBuffer = aBuffer;

  // must not report profiling form while some timers are active!
  if (pf_ctRunningTimers!=0) {
    strBuffer += sprintf(strBuffer,
      "WARNING: Some timers are still active - the results are wrong!\n");
  }
  //ASSERT(pf_ctRunningTimers==0);

  // calculate total application time since profile form was reset
  CTimerValue tvNow = _pTimer->GetHighPrecisionTimer();
  CTimerValue tvApplicationElapsed = tvNow-pf_tvLastReset;
  // calculate total time spent in profiled module
  CTimerValue tvModuleElapsed = pf_tvOverAllElapsed;
  // print the main header
  strBuffer += sprintf(strBuffer, "%s profile for last %d %s:\n",
    pf_strTitle, GetAveragingCounter(), pf_strAveragingUnits);

  // print header for timers
  strBuffer += sprintf(strBuffer,
    "Module time: %6.2f%% of application time. Average time: %6.2fms\n",
    tvModuleElapsed.GetSeconds()/tvApplicationElapsed.GetSeconds()*100.0,
    tvModuleElapsed.GetSeconds()/GetAveragingCounter()*1000.0
    );

  strBuffer += sprintf(strBuffer, "\n");
  // for all timers
  FOREACHINSTATICARRAY(pf_aptTimers, CProfileTimer, itpt) {
    // print the timer
    itpt->Report(strBuffer, GetAveragingCounter(), tvApplicationElapsed, tvModuleElapsed);
  }

  strBuffer += sprintf(strBuffer, "\n");
  // for all counters
  FOREACHINSTATICARRAY(pf_apcCounters, CProfileCounter, itpc) {
    // print the counter
    itpc->Report(strBuffer, GetAveragingCounter());
  }
  // print the footer
  strBuffer += sprintf(strBuffer, "--------------------\n");

  // return the buffer
  strReport = aBuffer;
}
