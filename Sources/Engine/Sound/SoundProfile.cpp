/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Sound/SoundProfile.h>

// profile form for profiling sounds
CSoundProfile _spSoundProfile;
CProfileForm &_pfSoundProfile = _spSoundProfile;

/////////////////////////////////////////////////////////////////////
// CSoundProfile

CSoundProfile::CSoundProfile(void)
 : CProfileForm ("Sound", "updates",
    CSoundProfile::PCI_COUNT, CSoundProfile::PTI_COUNT)
{
  SETTIMERNAME( PTI_MIXSOUNDS,    "MixSounds()", "");
  SETTIMERNAME( PTI_DECODESOUND,  "  DecodeSound()", "");
  SETTIMERNAME( PTI_MIXSOUND,     "  MixSound()", "");
  SETTIMERNAME( PTI_RAWMIXER,     "    Raw Mixer Loop", "");
  SETTIMERNAME( PTI_UPDATESOUNDS, "UpdateSounds()", "");

  SETCOUNTERNAME( PCI_MIXINGS,       "number of mixings");
  SETCOUNTERNAME( PCI_SOUNDSMIXED,   "sounds mixed");
  SETCOUNTERNAME( PCI_SOUNDSSKIPPED, "sounds skipped for low volume");
  SETCOUNTERNAME( PCI_SOUNDSDELAYED, "sounds delayed for sound speed latency");
  SETCOUNTERNAME( PCI_SAMPLES,       "samples mixed");
}
