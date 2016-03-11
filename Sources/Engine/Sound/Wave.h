/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_WAVE_H
#define SE_INCL_WAVE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

/*
 *  PCM Wave Input
 */
class PCMWaveInput {
private:
  // Wave data
  WAVEFORMATEX pwi_wfeWave;
  WAVEFORMATEX pwi_wfeDesired;
  ULONG  pwi_ulRiffLength, pwi_ulDataLength;
  BOOL   pwi_bInfoLoaded,  pwi_bDataLoaded; // Status
  SWORD *pwi_pswMemory; // Memory

  /* Conversion */
  DOUBLE pwi_dRatio;
  // get and store data
  inline ULONG GetData_t( CTStream *pCstrInput);
  inline void  StoreData( ULONG ulData);
  void CopyData_t(   CTStream *pCstrInput);
  void ShrinkData_t( CTStream *pCstrInput);

public:
  // Check wave format
  static void CheckWaveFormat_t( WAVEFORMATEX SwfeCheck, char *pcErrorString);

  /* Constructor */
  inline PCMWaveInput(void) { pwi_bInfoLoaded = FALSE; pwi_bDataLoaded = FALSE; };
  /* Load Wave info */
  WAVEFORMATEX LoadInfo_t( CTStream *pCstrInput);
  /* Load and convert Wave data */
  void LoadData_t( CTStream *pCstrInput, SWORD *pswMemory, WAVEFORMATEX &SwfeDesired);

  /* Length in bytes / blocks / seconds */
  ULONG  GetByteLength(void);
  ULONG  GetDataLength(void);
  ULONG  GetDataLength( WAVEFORMATEX SwfeDesired);
  DOUBLE GetSecondsLength(void);

  /* Buffer length in bytes */
  ULONG DetermineBufferSize(void);
  ULONG DetermineBufferSize( WAVEFORMATEX SwfeDesired);
};



#endif  /* include-once check. */

