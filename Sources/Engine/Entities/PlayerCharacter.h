/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_PLAYERCHARACTER_H
#define SE_INCL_PLAYERCHARACTER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/CTString.h>

/*
 * Class describing one player character in game.
 */
class ENGINE_API CPlayerCharacter {
public:
  // globally unique identifier of the player
  // this is so that player can be identified even after renaming
  #define PLAYERGUIDSIZE 16
  UBYTE pc_aubGUID[PLAYERGUIDSIZE];

public:
  CTString pc_strName;      // name of the character
  CTString pc_strTeam;      // team of the character
  // buffer for custom use by CPlayerEntity derived class to describe player
  #define MAX_PLAYERAPPEARANCE 32
  UBYTE pc_aubAppearance[MAX_PLAYERAPPEARANCE];

  /* Default constructor. */
  CPlayerCharacter(void);
  /* Create a new character with its name. */
  CPlayerCharacter(const CTString &strName);
  /* Get character name. */
  const CTString &GetName(void) const;
  const CTString GetNameForPrinting(void) const;
  /* Set character name. */
  void SetName(CTString strName);
  /* Get character team. */
  const CTString &GetTeam(void) const;
  const CTString GetTeamForPrinting(void) const;
  /* Set character team. */
  void SetTeam(CTString strTeam);

  void Load_t( const CTFileName &fnFile); // throw char *
  void Save_t( const CTFileName &fnFile); // throw char *
  /* Read character from a stream. */
  void Read_t(CTStream *pstr);       // throw char *
  /* Write character into a stream. */
  void Write_t(CTStream *pstr);      // throw char *

  /* Assignment operator. */
  CPlayerCharacter &operator=(const CPlayerCharacter &pcOther);

  /* Comparison operator. */
  BOOL operator==(const CPlayerCharacter &pcOther) const;

  // stream operations
  ENGINE_API friend CTStream &operator<<(CTStream &strm, CPlayerCharacter &pc);
  ENGINE_API friend CTStream &operator>>(CTStream &strm, CPlayerCharacter &pc);
  // message operations
  friend CNetworkMessage &operator<<(CNetworkMessage &nm, CPlayerCharacter &pc);
  friend CNetworkMessage &operator>>(CNetworkMessage &nm, CPlayerCharacter &pc);
};


#endif  /* include-once check. */

