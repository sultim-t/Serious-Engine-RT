/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#pragma once

class CSoundDecoder {
public:
  class CDecodeData_MPEG *sdc_pmpeg;
  class CDecodeData_OGG  *sdc_pogg ;

  // initialize/end the decoding support engine(s)
  static void InitPlugins(void);
  static void EndPlugins(void);

  // create a decoder that streams from file
  CSoundDecoder(const CTFileName &fnmStream);
  ~CSoundDecoder(void);
  void Clear(void);

  // check if a decoder is succefully opened
  BOOL IsOpen(void);
  // get wave format of the decoder (invaid if it is not open)
  void GetFormat(WAVEFORMATEX &wfe);

  // decode a block of bytes
  INDEX Decode(void *pvDestBuffer, INDEX ctBytesToDecode);
  // reset decoder to start of sample
  void Reset(void);
};