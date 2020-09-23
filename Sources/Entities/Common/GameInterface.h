// interfacing between CGame and entities
#ifndef SE_INCL_GAMEINTERFACE_H
#define SE_INCL_GAMEINTERFACE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// controls' declarations

// these define address and size of player controls structure
DECL_DLL extern void *ctl_pvPlayerControls;
DECL_DLL extern const SLONG ctl_slPlayerControlsSize;
// called to compose action packet from current controls
DECL_DLL extern void ctl_ComposeActionPacket(const CPlayerCharacter &pc, CPlayerAction &paAction, BOOL bPreScan);

// game sets this for player hud and statistics
DECL_DLL extern INDEX plr_iHiScore;

// computer's declarations

// type of computer message
enum CompMsgType {
  CMT_INFORMATION = 0,
  CMT_BACKGROUND  = 1,
  CMT_WEAPONS     = 2,
  CMT_ENEMIES     = 3,
  CMT_STATISTICS  = 4,

  CMT_COUNT = 5,
};


// type of statistic comp message
enum CompStatType {
  CST_SHORT  = 1,
  CST_DETAIL = 2,
};


// identifier of a computer message
class DECL_DLL CCompMessageID {
public:
  enum CompMsgType cmi_cmtType; // message category
  CTFileName cmi_fnmFileName;   // message filename
  BOOL cmi_bRead;               // true if message is read
  ULONG cmi_ulHash;             // filename hash for fast searching

  void Clear(void);
  void Read_t(CTStream &strm);    // throw char *
  void Write_t(CTStream &strm);   // throw char *
  void NewMessage(const CTFileName &fnm);
};

// !=NULL if some player wants to call computer
DECL_DLL extern class CPlayer *cmp_ppenPlayer;
// !=NULL for rendering computer on secondary display in dualhead
DECL_DLL extern class CPlayer *cmp_ppenDHPlayer;
// set to update current message in background mode (for dualhead)
DECL_DLL extern BOOL cmp_bUpdateInBackground;
// set for initial calling computer without rendering game
DECL_DLL extern BOOL cmp_bInitialStart;


#endif  /* include-once check. */

