/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */


// DLLFUNCTION(dll, output, name, inputs, params, required)

DLLFUNCTION( OVF, int, ov_clear, (OggVorbis_File *vf), 4, 0)
DLLFUNCTION( OVF, int, ov_open, (FILE *f, OggVorbis_File *vf, char *initial, long ibytes), 16,0);
DLLFUNCTION( OVF, int, ov_open_callbacks, (void *datasource, OggVorbis_File *vf, char *initial, long ibytes, ov_callbacks callbacks), 32,0);
DLLFUNCTION( OVF, long, ov_read, (OggVorbis_File *vf,char *buffer,int length, int bigendianp,int word,int sgned,int *bitstream), 28,0);
DLLFUNCTION( OVF, vorbis_info *, ov_info, (OggVorbis_File *vf,int link), 8,0);
DLLFUNCTION( OVF, int, ov_time_seek, (OggVorbis_File *vf, double pos), 12,0);
