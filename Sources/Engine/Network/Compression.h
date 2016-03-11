/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_COMPRESSION_H
#define SE_INCL_COMPRESSION_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

/*
 * Abstract base class for objects that can compress memory blocks.
 */
class CCompressor {
public:
  /* Calculate needed size for destination buffer when packing memory with given compression. */
  virtual SLONG NeededDestinationSize(SLONG slSourceSize) = 0;

  // on entry, slDstSize holds maximum size of output buffer,
  // on exit, it is filled with resulting size
  /* Pack a chunk of data using given compression. */
  virtual BOOL   Pack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize) = 0;
  /* Unpack a chunk of data using given compression. */
  virtual BOOL Unpack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize) = 0;

  /* Pack/unpack from stream to stream. */
  void UnpackStream_t(CTMemoryStream &strmSrc, CTStream &strmDst); // throw char *
  void PackStream_t(CTMemoryStream &strmSrc, CTStream &strmDst); // throw char *
};

/*
 * Compressor for compressing memory blocks using RLE BYTE-BYTE compression
 */
class CRLEBBCompressor : public CCompressor {
public:
  /* Calculate needed size for destination buffer when packing memory. */
  SLONG NeededDestinationSize(SLONG slSourceSize);

  // on entry, slDstSize holds maximum size of output buffer,
  // on exit, it is filled with resulting size
  /* Pack a chunk of data using given compression. */
  BOOL   Pack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize);
  /* Unpack a chunk of data using given compression. */
  BOOL Unpack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize);
};


/*
 * Compressor for compressing memory blocks using LZ compression
 * (uses LZRW1 - a modification by Ross Williams)
 */
class CLZCompressor : public CCompressor {
public:
  /* Calculate needed size for destination buffer when packing memory. */
  SLONG NeededDestinationSize(SLONG slSourceSize);

  // on entry, slDstSize holds maximum size of output buffer,
  // on exit, it is filled with resulting size
  /* Pack a chunk of data using given compression. */
  BOOL   Pack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize);
  /* Unpack a chunk of data using given compression. */
  BOOL Unpack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize);
};

/*
 * Compressor for compressing memory blocks using zlib compression
 * (zlib uses LZ77 - algorithm)
 */
class CzlibCompressor : public CCompressor {
public:
  /* Calculate needed size for destination buffer when packing memory. */
  SLONG NeededDestinationSize(SLONG slSourceSize);

  // on entry, slDstSize holds maximum size of output buffer,
  // on exit, it is filled with resulting size
  /* Pack a chunk of data using given compression. */
  BOOL   Pack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize);
  /* Unpack a chunk of data using given compression. */
  BOOL Unpack(const void *pvSrc, SLONG slSrcSize, void *pvDst, SLONG &slDstSize);
};


#endif  /* include-once check. */

