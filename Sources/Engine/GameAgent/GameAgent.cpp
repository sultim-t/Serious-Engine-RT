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

#include "StdH.h"

#include <Engine/Engine.h>
#include <Engine/CurrentVersion.h>
#include <Engine/Entities/Entity.h>
#include <Engine/Base/Shell.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/CTString.h>
#include <Engine/Network/Server.h>
#include <Engine/Network/Network.h>
#include <Engine/Network/SessionState.h>
#include <GameMP/SessionProperties.h>
#include <Engine/GameAgent/GameAgent.h>

#include <Engine/GameAgent/MSLegacy.h>

#define MSPORT      28900
#define BUFFSZ      8192
#define BUFFSZSTR   4096

#define PCK         "\\gamename\\%s" \
                    "\\enctype\\%d" \
                    "\\validate\\%s" \
                    "\\final\\" \
                    "\\queryid\\1.1" \
                    "\\list\\cmp" \
                    "\\gamename\\%s" \
                    "\\gamever\\1.05" \
                    "%s%s" \
                    "\\final\\"

#define PCKQUERY    "\\gamename\\%s" \
                    "\\gamever\\%s" \
                    "\\location\\%s" \
                    "\\hostname\\%s" \
                    "\\hostport\\%hu" \
                    "\\mapname\\%s" \
                    "\\gametype\\%s" \
                    "\\activemod\\" \
                    "\\numplayers\\%d" \
                    "\\maxplayers\\%d" \
                    "\\gamemode\\openplaying" \
                    "\\difficulty\\Normal" \
                    "\\friendlyfire\\%d" \
                    "\\weaponsstay\\%d" \
                    "\\ammosstay\\%d" \
                    "\\healthandarmorstays\\%d" \
                    "\\allowhealth\\%d" \
                    "\\allowarmor\\%d" \
                    "\\infinitearmor\\%d" \
                    "\\respawninplace\\%d" \
                    "\\password\\0" \
                    "\\vipplayers\\1"

#define PCKINFO     "\\hostname\\%s" \
                    "\\hostport\\%hu" \
                    "\\mapname\\%s" \
                    "\\gametype\\%s" \
                    "\\numplayers\\%d" \
                    "\\maxplayers\\%d" \
                    "\\gamemode\\openplaying" \
                    "\\final\\" \
                    "\\queryid\\8.1"

#define PCKBASIC    "\\gamename\\%s" \
                    "\\gamever\\%s" \
                    "\\location\\EU" \
                    "\\final\\" \
                    "\\queryid\\1.1"


#define CHK_BUFFSTRLEN if((iLen < 0) || (iLen > BUFFSZSTR)) { \
                        CPrintF("\n" \
                            "Error: the used buffer is smaller than how much needed (%d < %d)\n" \
                            "\n", iLen, BUFFSZSTR); \
                            if(cMsstring) free (cMsstring); \
                            closesocket(_sock); \
                            WSACleanup(); \
                        }

#define CLEANMSSRUFF1       closesocket(_sock); \
                            WSACleanup();

#define CLEANMSSRUFF2       if(cResponse) free (cResponse); \
                            closesocket(_sock); \
                            WSACleanup();

#define SERIOUSSAMKEY       "AKbna4\0"
#define SERIOUSSAMSTR       "serioussamse"

#pragma comment(lib, "wsock32.lib")

WSADATA* _wsaData = NULL;
SOCKET _socket = NULL;

sockaddr_in* _sin = NULL;
sockaddr_in* _sinLocal = NULL;
sockaddr_in _sinFrom;

CHAR* _szBuffer = NULL;
CHAR* _szIPPortBuffer = NULL;
INT   _iIPPortBufferLen = 0;
CHAR* _szIPPortBufferLocal = NULL;
INT   _iIPPortBufferLocalLen = 0;

BOOL _bServer = FALSE;
BOOL _bInitialized = FALSE;
BOOL _bActivated = FALSE;
BOOL _bActivatedLocal = FALSE;

TIME _tmLastHeartbeat = 0;

CDynamicStackArray<CServerRequest> ga_asrRequests;

//extern CTString ga_strServer = "master1.croteam.org";
extern CTString ga_strServer = "master1.42amsterdam.net";
//extern CTString ga_strMSLegacy = "master1.croteam.org";
extern CTString ga_strMSLegacy = "42amsterdam.net";

extern BOOL ga_bMSLegacy = TRUE;
//BOOL ga_bMSLegacy = FALSE;

void _uninitWinsock();
void _initializeWinsock(void)
{
  if(_wsaData != NULL && _socket != NULL) {
    return;
  }

  _wsaData = new WSADATA;
  _socket = NULL;

  // make the buffer that we'll use for packet reading
  if(_szBuffer != NULL) {
    delete[] _szBuffer;
  }
  _szBuffer = new char[2050];

  // start WSA
  if(WSAStartup(MAKEWORD(2, 2), _wsaData) != 0) {
    CPrintF("Error initializing winsock!\n");
    _uninitWinsock();
    return;
  }

  // get the host IP
  hostent* phe;
  if(!ga_bMSLegacy) {
    phe = gethostbyname(ga_strServer);
  } else {
    phe = gethostbyname(ga_strMSLegacy);
  }
  // if we couldn't resolve the hostname
  if(phe == NULL) {
    // report and stop
    CPrintF("Couldn't resolve GameAgent server %s.\n", ga_strServer);
    _uninitWinsock();
    return;
  }

  // create the socket destination address
  _sin = new sockaddr_in;
  _sin->sin_family = AF_INET;
  _sin->sin_addr.s_addr = *(ULONG*)phe->h_addr_list[0];
  if(!ga_bMSLegacy) {
    _sin->sin_port = htons(9005);
  } else {
    _sin->sin_port = htons(27900);
  }

  // create the socket
  _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // if we're a server
  if(_bServer) {
    // create the local socket source address
    _sinLocal = new sockaddr_in;
    _sinLocal->sin_family = AF_INET;
    _sinLocal->sin_addr.s_addr = inet_addr("0.0.0.0");
    _sinLocal->sin_port = htons(_pShell->GetINDEX("net_iPort") + 1);

    // bind the socket
    bind(_socket, (sockaddr*)_sinLocal, sizeof(*_sinLocal));
  }

  // set the socket to be nonblocking
  DWORD dwNonBlocking = 1;
  if(ioctlsocket(_socket, FIONBIO, &dwNonBlocking) != 0) {
    CPrintF("Error setting socket to nonblocking!\n");
    _uninitWinsock();
    return;
  }
}

void _uninitWinsock()
{
  if(_wsaData != NULL) {
    closesocket(_socket);
    delete _wsaData;
    _wsaData = NULL;
  }
  _socket = NULL;
}

void _sendPacketTo(const char* pubBuffer, INDEX iLen, sockaddr_in* sin)
{
  sendto(_socket, pubBuffer, iLen, 0, (sockaddr*)sin, sizeof(sockaddr_in));
}
void _sendPacketTo(const char* szBuffer, sockaddr_in* addsin)
{
  sendto(_socket, szBuffer, strlen(szBuffer), 0, (sockaddr*)addsin, sizeof(sockaddr_in));
}

void _sendPacket(const char* pubBuffer, INDEX iLen)
{
  _initializeWinsock();
  _sendPacketTo(pubBuffer, iLen, _sin);
}
void _sendPacket(const char* szBuffer)
{
  _initializeWinsock();
  _sendPacketTo(szBuffer, _sin);
}

int _recvPacket()
{
  int fromLength = sizeof(_sinFrom);
  return recvfrom(_socket, _szBuffer, 2048, 0, (sockaddr*)&_sinFrom, &fromLength);
}

CTString _getGameModeName(INDEX iGameMode)
{
  // get function that will provide us the info about gametype
  CShellSymbol *pss = _pShell->GetSymbol("GetGameTypeNameSS", /*bDeclaredOnly=*/ TRUE);

  if(pss == NULL) {
    return "";
  }

  CTString (*pFunc)(INDEX) = (CTString (*)(INDEX))pss->ss_pvValue;
  return pFunc(iGameMode);
}

const CSessionProperties* _getSP()
{
  return ((const CSessionProperties *)_pNetwork->GetSessionProperties());
}

void _sendHeartbeat(INDEX iChallenge)
{
  CTString strPacket;
  if (!ga_bMSLegacy) {
    strPacket.PrintF("0;challenge;%d;players;%d;maxplayers;%d;level;%s;gametype;%s;version;%s;product;%s",
        iChallenge,
        _pNetwork->ga_srvServer.GetPlayersCount(),
        _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
        _pNetwork->ga_World.wo_strName,
        _getGameModeName(_getSP()->sp_gmGameMode),
        _SE_VER_STRING,
        _pShell->GetString("sam_strGameName"));
  } else {
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse", (_pShell->GetINDEX("net_iPort") + 1));
  }
  _sendPacket(strPacket);
  _tmLastHeartbeat = _pTimer->GetRealTimeTick();
}

static void _setStatus(const CTString &strStatus)
{
  _pNetwork->ga_bEnumerationChange = TRUE;
  _pNetwork->ga_strEnumerationStatus = strStatus;
}

CServerRequest::CServerRequest(void)
{
  Clear();
}
CServerRequest::~CServerRequest(void) { }
void CServerRequest::Clear(void)
{
  sr_ulAddress = 0;
  sr_iPort = 0;
  sr_tmRequestTime = 0;
}

/// Initialize GameAgent.
extern void GameAgent_ServerInit(void)
{
  // join
  _bServer = TRUE;
  _bInitialized = TRUE;

  if (!ga_bMSLegacy) {
    _sendPacket("q");
  } else {
    CTString strPacket;
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse", (_pShell->GetINDEX("net_iPort") + 1));
    _sendPacket(strPacket);
  }
}

/// Let GameAgent know that the server has stopped.
extern void GameAgent_ServerEnd(void)
{
  if (!_bInitialized) {
    return;
  }

  if (ga_bMSLegacy) {
    CTString strPacket;
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse\\statechanged", (_pShell->GetINDEX("net_iPort") + 1));
    _sendPacket(strPacket);
  }

  _uninitWinsock();
  _bInitialized = FALSE;
}

/// GameAgent server update call which responds to enumeration pings and sends pings to masterserver.
extern void GameAgent_ServerUpdate(void)
{
  if((_socket == NULL) || (!_bInitialized)) {
    return;
  }

  int iLen = _recvPacket();
  if(iLen > 0) {
    if (!ga_bMSLegacy) {
    // check the received packet ID
    switch(_szBuffer[0]) {
    case 1: // server join response
      {
        int iChallenge = *(INDEX*)(_szBuffer + 1);
        // send the challenge
        _sendHeartbeat(iChallenge);
        break;
      }

    case 2: // server status request
      {
        // send the status response
        CTString strPacket;
        strPacket.PrintF("0;players;%d;maxplayers;%d;level;%s;gametype;%s;version;%s;gamename;%s;sessionname;%s",
          _pNetwork->ga_srvServer.GetPlayersCount(),
          _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
          _pNetwork->ga_World.wo_strName,
          _getGameModeName(_getSP()->sp_gmGameMode),
          _SE_VER_STRING,
          _pShell->GetString("sam_strGameName"),
          _pShell->GetString("gam_strSessionName"));
        _sendPacketTo(strPacket, &_sinFrom);
        break;
      }

    case 3: // player status request
      {
        // send the player status response
        CTString strPacket;
        strPacket.PrintF("\x01players\x02%d\x03", _pNetwork->ga_srvServer.GetPlayersCount());
        for(INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayersCount(); i++) {
          CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
          CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
          if(plt.plt_bActive) {
            CTString strPlayer;
            plt.plt_penPlayerEntity->GetGameAgentPlayerInfo(plb.plb_Index, strPlayer);

            // if we don't have enough space left for the next player
            if(strlen(strPacket) + strlen(strPlayer) > 2048) {
              // send the packet
              _sendPacketTo(strPacket, &_sinFrom);
              strPacket = "";
            }

            strPacket += strPlayer;
          }
        }

        strPacket += "\x04";
        _sendPacketTo(strPacket, &_sinFrom);
        break;
      }

    case 4: // ping
      {
        // just send back 1 byte and the amount of players in the server (this could be useful in some cases for external scripts)
        CTString strPacket;
        strPacket.PrintF("\x04%d", _pNetwork->ga_srvServer.GetPlayersCount());
        _sendPacketTo(strPacket, &_sinFrom);
        break;
      }
     }
   } else {

      _szBuffer[iLen] = 0;
      char *sPch1 = NULL, *sPch2 = NULL, *sPch3 = NULL, *sPch4 = NULL;
      sPch1 = strstr(_szBuffer, "\\status\\");
      sPch2 = strstr(_szBuffer, "\\info\\");
      sPch3 = strstr(_szBuffer, "\\basic\\");
      sPch4 = strstr(_szBuffer, "\\players\\");
      if(sPch1) {
        CTString strPacket;
        CTString strLocation;
        strLocation = _pShell->GetString("net_strLocalHost");
        if ( strLocation == ""){
          strLocation = "Heartland";
        }
        strPacket.PrintF( PCKQUERY,
          _pShell->GetString("sam_strGameName"),
          _SE_VER_STRING,
          //_pShell->GetString("net_strLocalHost"),
          strLocation,
          _pShell->GetString("gam_strSessionName"),
          _pShell->GetINDEX("net_iPort"),
          _pNetwork->ga_World.wo_strName,
          _getGameModeName(_getSP()->sp_gmGameMode),
          _pNetwork->ga_srvServer.GetPlayersCount(),
          _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
          _pShell->GetINDEX("gam_bFriendlyFire"),
          _pShell->GetINDEX("gam_bWeaponsStay"),
          _pShell->GetINDEX("gam_bAmmoStays"),
          _pShell->GetINDEX("gam_bHealthArmorStays"),
          _pShell->GetINDEX("gam_bAllowHealth"),
          _pShell->GetINDEX("gam_bAllowArmor"),
          _pShell->GetINDEX("gam_bInfiniteAmmo"),
          _pShell->GetINDEX("gam_bRespawnInPlace"));

          for(INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayersCount(); i++) {
            CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
            CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
            if(plt.plt_bActive) {
              CTString strPlayer;
              plt.plt_penPlayerEntity->GetMSLegacyPlayerInf(plb.plb_Index, strPlayer);

              // if we don't have enough space left for the next player
              if(strlen(strPacket) + strlen(strPlayer) > 2048) {
                // send the packet
                _sendPacketTo(strPacket, &_sinFrom);
                strPacket = "";
              }
              strPacket += strPlayer;
            }
          }

        strPacket += "\\final\\\\queryid\\333.1";
        _sendPacketTo(strPacket, &_sinFrom);

      } else if (sPch2){

        CTString strPacket;
        strPacket.PrintF( PCKINFO,
          _pShell->GetString("gam_strSessionName"),
          _pShell->GetINDEX("net_iPort"),
          _pNetwork->ga_World.wo_strName,
          _getGameModeName(_getSP()->sp_gmGameMode),
          _pNetwork->ga_srvServer.GetPlayersCount(),
          _pNetwork->ga_sesSessionState.ses_ctMaxPlayers);
        _sendPacketTo(strPacket, &_sinFrom);

      } else if (sPch3){

        CTString strPacket;
        CTString strLocation;
        strLocation = _pShell->GetString("net_strLocalHost");
        if ( strLocation == ""){
          strLocation = "Heartland";
        }
        strPacket.PrintF( PCKBASIC,
          _pShell->GetString("sam_strGameName"),
          _SE_VER_STRING,
          //_pShell->GetString("net_strLocalHost"));
          strLocation);
        _sendPacketTo(strPacket, &_sinFrom);

      } else if (sPch4){

        // send the player status response
        CTString strPacket;
        strPacket = "";
        for(INDEX i=0; i<_pNetwork->ga_srvServer.GetPlayersCount(); i++) {
          CPlayerBuffer &plb = _pNetwork->ga_srvServer.srv_aplbPlayers[i];
          CPlayerTarget &plt = _pNetwork->ga_sesSessionState.ses_apltPlayers[i];
          if(plt.plt_bActive) {
            CTString strPlayer;
            plt.plt_penPlayerEntity->GetMSLegacyPlayerInf(plb.plb_Index, strPlayer);

            // if we don't have enough space left for the next player
            if(strlen(strPacket) + strlen(strPlayer) > 2048) {
              // send the packet
              _sendPacketTo(strPacket, &_sinFrom);
              strPacket = "";
            }

            strPacket += strPlayer;
          }
        }

        strPacket += "\\final\\\\queryid\\6.1";
        _sendPacketTo(strPacket, &_sinFrom);

      } else {
        CPrintF("Unknown query server response!\n");
        return;
      }
   }
 }

 // send a heartbeat every 150 seconds
 if(_pTimer->GetRealTimeTick() - _tmLastHeartbeat >= 150.0f) {
    _sendHeartbeat(0);
 }
}

/// Notify GameAgent that the server state has changed.
extern void GameAgent_ServerStateChanged(void)
{
  if (!_bInitialized) {
    return;
  }
  if (!ga_bMSLegacy) {
    _sendPacket("u");
  } else {
    CTString strPacket;
    strPacket.PrintF("\\heartbeat\\%hu\\gamename\\serioussamse\\statechanged", (_pShell->GetINDEX("net_iPort") + 1));
    _sendPacket(strPacket);
  }
}

/// Request serverlist enumeration.
extern void GameAgent_EnumTrigger(BOOL bInternet)
{

  if ( _pNetwork->ga_bEnumerationChange ) {
    return;
  }
  
  if ( !bInternet && ga_bMSLegacy) {
    // make sure that there are no requests still stuck in buffer
    ga_asrRequests.Clear();
    // we're not a server
    _bServer = FALSE;
    _pNetwork->ga_strEnumerationStatus = ".";
	
	WORD     _wsaRequested;
	WSADATA  wsaData;
	PHOSTENT _phHostinfo;
	ULONG    _uIP,*_pchIP = &_uIP;
	USHORT   _uPort,*_pchPort = &_uPort;
	INT      _iLen;
	char     _cName[256],*_pch,_strFinal[8] = {0};

	struct in_addr addr;

    // make the buffer that we'll use for packet reading
    if(_szIPPortBufferLocal != NULL) {
       return;
    }
    _szIPPortBufferLocal = new char[1024];
	
	// start WSA
	_wsaRequested = MAKEWORD( 2, 2 );
    if( WSAStartup(_wsaRequested, &wsaData) != 0) {
		CPrintF("Error initializing winsock!\n");
		if(_szIPPortBufferLocal != NULL) {
			delete[] _szIPPortBufferLocal;
		}
		_szIPPortBufferLocal = NULL;
		_uninitWinsock();
		_bInitialized = FALSE;
		_pNetwork->ga_bEnumerationChange = FALSE;
		_pNetwork->ga_strEnumerationStatus = "";
		WSACleanup();
        return;
    }

    _pch = _szIPPortBufferLocal;
	_iLen = 0;
	strcpy(_strFinal,"\\final\\");
	
    if( gethostname ( _cName, sizeof(_cName)) == 0)
	{
		if((_phHostinfo = gethostbyname(_cName)) != NULL)
		{
			int _iCount = 0;
			while(_phHostinfo->h_addr_list[_iCount])
			{
				addr.s_addr = *(u_long *) _phHostinfo->h_addr_list[_iCount];
				_uIP = htonl(addr.s_addr);
				
				for (UINT uPort = 25601; uPort < 25622; ++uPort){
					_uPort = htons(uPort);
					memcpy(_pch,_pchIP,4);
					_pch  +=4;
					_iLen +=4;
					memcpy(_pch,_pchPort,2);
					_pch  +=2;
					_iLen +=2;
				}
				++_iCount;
			}
			memcpy(_pch,_strFinal, 7);
			_pch  +=7;
			_iLen +=7;
			_pch[_iLen] = 0x00;
		}
	}
    _iIPPortBufferLocalLen = _iLen;

    _bActivatedLocal = TRUE;
    _bInitialized = TRUE;
    _initializeWinsock();	
    return;
	
  } else {

  if (!ga_bMSLegacy) {
    // make sure that there are no requests still stuck in buffer
    ga_asrRequests.Clear();
    // we're not a server
    _bServer = FALSE;
    // Initialization
    _bInitialized = TRUE;
    // send enumeration packet to masterserver
    _sendPacket("e");
    _setStatus(".");
  }
  else
  { /* MSLegacy */
    // make sure that there are no requests still stuck in buffer
    ga_asrRequests.Clear();
    // we're not a server
    _bServer = FALSE;
    _pNetwork->ga_strEnumerationStatus = ".";

    struct  sockaddr_in peer;

    SOCKET  _sock               = NULL;
    u_int   uiMSIP;
    int     iErr,
            iLen,
            iDynsz,
            iEnctype             = 0;
    u_short usMSport             = MSPORT;

    u_char  ucGamekey[]          = {SERIOUSSAMKEY},
            ucGamestr[]          = {SERIOUSSAMSTR},
            *ucSec               = NULL,
            *ucKey               = NULL;

    char    *cFilter             = "",
            *cWhere              = "",
            cMS[128]             = {0},
            *cResponse           = NULL,
            *cMsstring           = NULL,
            *cSec                = NULL;


    strcpy(cMS,ga_strMSLegacy);

    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
        CPrintF("Error initializing winsock!\n");
        return;
    }

/* Open a socket and connect to the Master server */

    peer.sin_addr.s_addr = uiMSIP = resolv(cMS);
    peer.sin_port        = htons(usMSport);
    peer.sin_family      = AF_INET;

    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sock < 0) {
        CPrintF("Error creating TCP socket!\n");
        WSACleanup();
        return;
    }
    if(connect(_sock, (struct sockaddr *)&peer, sizeof(peer)) < 0) {
        CPrintF("Error connecting to TCP socket!\n");
        CLEANMSSRUFF1;
        return;
    }

/* Allocate memory for a buffer and get a pointer to it */

    cResponse = (char*) malloc(BUFFSZSTR + 1);
    if(!cResponse) {
        CPrintF("Error initializing memory buffer!\n");
        CLEANMSSRUFF1;
        return;
    }

/* Reading response from Master Server - returns the string with the secret key */

    iLen = 0;
    iErr = recv(_sock, (char*)cResponse + iLen, BUFFSZSTR - iLen, 0);
    if(iErr < 0) {
        CPrintF("Error reading from TCP socket!\n");
        CLEANMSSRUFF2;
        return;
    }

    iLen += iErr;
    cResponse[iLen] = 0x00;

/* Allocate memory for a buffer and get a pointer to it */

    ucSec = (u_char*) malloc(BUFFSZSTR + 1);
    if(!ucSec) {
        CPrintF("Error initializing memory buffer!\n");
        CLEANMSSRUFF2;
        return;
    }
    memcpy ( ucSec, cResponse,  BUFFSZSTR);
    ucSec[iLen] = 0x00;

/* Geting the secret key from a string */

    cSec = strstr(cResponse, "\\secure\\");
    if(!cSec) {
        CPrintF("Not valid master server response!\n");
        CLEANMSSRUFF2;
        return;
    } else {
        ucSec  += 15;

/* Creating a key for authentication (Validate key) */

        ucKey = gsseckey(ucSec, ucGamekey, iEnctype);
    }
    ucSec -= 15;
    if(cResponse) free (cResponse);
    if(ucSec) free (ucSec);

/* Generate a string for the response (to Master Server) with the specified (Validate ucKey) */

    cMsstring = (char*) malloc(BUFFSZSTR + 1);
    if(!cMsstring) {
        CPrintF("Not valid master server response!\n");
        CLEANMSSRUFF1;
        return;
    }

    iLen = _snprintf(
        cMsstring,
        BUFFSZSTR,
        PCK,
        ucGamestr,
        iEnctype,
        ucKey,
        ucGamestr,
        cWhere,
        cFilter);

/* Check the buffer */

    CHK_BUFFSTRLEN;

/* The string sent to master server */

    if(send(_sock,cMsstring, iLen, 0) < 0){
        CPrintF("Error reading from TCP socket!\n");
        if(cMsstring) free (cMsstring);
        CLEANMSSRUFF1;
        return;
    }
    if(cMsstring) free (cMsstring);

 /* Allocate memory for a buffer and get a pointer to it */

    if(_szIPPortBuffer ) {
        CLEANMSSRUFF1;
        return;
    };

    _szIPPortBuffer = (char*) malloc(BUFFSZ + 1);
    if(!_szIPPortBuffer) {
        CPrintF("Error reading from TCP socket!\n");
        CLEANMSSRUFF1;
        return;
    }
    iDynsz = BUFFSZ;


/* The received encoded data after sending the string (Validate key) */

    iLen = 0;
    while((iErr = recv(_sock, _szIPPortBuffer + iLen, iDynsz - iLen, 0)) > 0) {
        iLen += iErr;
        if(iLen >= iDynsz) {
            iDynsz += BUFFSZ;
            _szIPPortBuffer = (char*)realloc(_szIPPortBuffer, iDynsz);
            if(!_szIPPortBuffer) {
                CPrintF("Error reallocation memory buffer!\n");
                if(_szIPPortBuffer) free (_szIPPortBuffer);
                CLEANMSSRUFF1;
                return;
            }
        }
    }
    CLEANMSSRUFF1;
    _iIPPortBufferLen = iLen;

    _bActivated = TRUE;
    _bInitialized = TRUE;
    _initializeWinsock();
   
    }
  }
}

/// GameAgent client update for enumerations.
extern void GameAgent_EnumUpdate(void)
{

  if((_socket == NULL) || (!_bInitialized)) {
    return;
  }

  if (!ga_bMSLegacy) {
   int iLen = _recvPacket();
   if(iLen != -1) {
    // null terminate the buffer
    _szBuffer[iLen] = 0;
    switch(_szBuffer[0]) {
    case 's':
      {
        struct sIPPort {
          UBYTE bFirst;
          UBYTE bSecond;
          UBYTE bThird;
          UBYTE bFourth;
          USHORT iPort;
        };
        _pNetwork->ga_strEnumerationStatus = "";

        sIPPort* pServers = (sIPPort*)(_szBuffer + 1);
        while(iLen - ((CHAR*)pServers - _szBuffer) >= sizeof(sIPPort)) {
          sIPPort ip = *pServers;

          CTString strIP;
          strIP.PrintF("%d.%d.%d.%d", ip.bFirst, ip.bSecond, ip.bThird, ip.bFourth);

          sockaddr_in sinServer;
          sinServer.sin_family = AF_INET;
          sinServer.sin_addr.s_addr = inet_addr(strIP);
          sinServer.sin_port = htons(ip.iPort + 1);

          // insert server status request into container
          CServerRequest &sreq = ga_asrRequests.Push();
          sreq.sr_ulAddress = sinServer.sin_addr.s_addr;
          sreq.sr_iPort = sinServer.sin_port;
          sreq.sr_tmRequestTime = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

          // send packet to server
          _sendPacketTo("\x02", &sinServer);

          pServers++;
        }
      }
      break;

    case '0':
      {
        CTString strPlayers;
        CTString strMaxPlayers;
        CTString strLevel;
        CTString strGameType;
        CTString strVersion;
        CTString strGameName;
        CTString strSessionName;

        CHAR* pszPacket = _szBuffer + 2; // we do +2 because the first character is always ';', which we don't care about.

        BOOL bReadValue = FALSE;
        CTString strKey;
        CTString strValue;

        while(*pszPacket != 0) {
          switch(*pszPacket) {
          case ';':
            if(strKey != "sessionname") {
              if(bReadValue) {
                // we're done reading the value, check which key it was
                if(strKey == "players") {
                  strPlayers = strValue;
                } else if(strKey == "maxplayers") {
                  strMaxPlayers = strValue;
                } else if(strKey == "level") {
                  strLevel = strValue;
                } else if(strKey == "gametype") {
                  strGameType = strValue;
                } else if(strKey == "version") {
                  strVersion = strValue;
                } else if(strKey == "gamename") {
                  strGameName = strValue;
                } else {
                  CPrintF("Unknown GameAgent parameter key '%s'!", strKey);
                }

                // reset temporary holders
                strKey = "";
                strValue = "";
              }
            }
            bReadValue = !bReadValue;
            break;

          default:
            // read into the value or into the key, depending where we are
            if(bReadValue) {
              strValue.InsertChar(strlen(strValue), *pszPacket);
            } else {
              strKey.InsertChar(strlen(strKey), *pszPacket);
            }
            break;
          }

          // move to next character
          pszPacket++;
        }

        // check if we still have a sessionname to back up
        if(strKey == "sessionname") {
          strSessionName = strValue;
        }

        // insert the server into the serverlist
        CNetworkSession &ns = *new CNetworkSession;
        _pNetwork->ga_lhEnumeratedSessions.AddTail(ns.ns_lnNode);

        long long tmPing = -1;
        // find the request in the request array
        for(INDEX i=0; i<ga_asrRequests.Count(); i++) {
          CServerRequest &req = ga_asrRequests[i];
          if(req.sr_ulAddress == _sinFrom.sin_addr.s_addr && req.sr_iPort == _sinFrom.sin_port) {
            tmPing = _pTimer->GetHighPrecisionTimer().GetMilliseconds() - req.sr_tmRequestTime;
            ga_asrRequests.Delete(&req);
            break;
          }
        }

        if(tmPing == -1) {
          // server status was never requested
          break;
        }

        // add the server to the serverlist
        ns.ns_strSession = strSessionName;
        ns.ns_strAddress = inet_ntoa(_sinFrom.sin_addr) + CTString(":") + CTString(0, "%d", htons(_sinFrom.sin_port) - 1);
        ns.ns_tmPing = (tmPing / 1000.0f);
        ns.ns_strWorld = strLevel;
        ns.ns_ctPlayers = atoi(strPlayers);
        ns.ns_ctMaxPlayers = atoi(strMaxPlayers);
        ns.ns_strGameType = strGameType;
        ns.ns_strMod = strGameName;
        ns.ns_strVer = strVersion;
      }
      break;

    default:
      CPrintF("Unknown enum packet ID %x!\n", _szBuffer[0]);
      break;
    }
  }
 } else {
 /* MSLegacy */
    if(_bActivated) {
        HANDLE  _hThread;
        DWORD   _dwThreadId;

        _hThread = CreateThread(NULL, 0, _MS_Thread, 0, 0, &_dwThreadId);
        if (_hThread != NULL) {
            CloseHandle(_hThread);
        }
        _bActivated = FALSE;		
    }
    if(_bActivatedLocal) {
        HANDLE  _hThread;
        DWORD   _dwThreadId;

        _hThread = CreateThread(NULL, 0, _LocalNet_Thread, 0, 0, &_dwThreadId);
        if (_hThread != NULL) {
            CloseHandle(_hThread);
        }
        _bActivatedLocal = FALSE;		
    }	
  }
}

/// Cancel the GameAgent serverlist enumeration.
extern void GameAgent_EnumCancel(void)
{
  if (_bInitialized) {
    CPrintF("...GameAgent_EnumCancel!\n");
    ga_asrRequests.Clear();
    _uninitWinsock();
  }
}

DWORD WINAPI _MS_Thread(LPVOID lpParam) {
    SOCKET _sockudp = NULL;
    struct _sIPPort {
        UBYTE bFirst;
        UBYTE bSecond;
        UBYTE bThird;
        UBYTE bFourth;
        USHORT iPort;
      };

    _setStatus("");
    _sockudp = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockudp == INVALID_SOCKET){
        WSACleanup();
        return -1;
    }

    _sIPPort* pServerIP = (_sIPPort*)(_szIPPortBuffer);
    while(_iIPPortBufferLen >= 6) {
        if(!strncmp((char *)pServerIP, "\\final\\", 7)) {
                break;
            }

        _sIPPort ip = *pServerIP;

        CTString strIP;
        strIP.PrintF("%d.%d.%d.%d", ip.bFirst, ip.bSecond, ip.bThird, ip.bFourth);

        sockaddr_in sinServer;
        sinServer.sin_family = AF_INET;
        sinServer.sin_addr.s_addr = inet_addr(strIP);
        sinServer.sin_port = ip.iPort;

        // insert server status request into container
        CServerRequest &sreq = ga_asrRequests.Push();
        sreq.sr_ulAddress = sinServer.sin_addr.s_addr;
        sreq.sr_iPort = sinServer.sin_port;
        sreq.sr_tmRequestTime = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

        // send packet to server
        sendto(_sockudp,"\\status\\",8,0,
            (sockaddr *) &sinServer, sizeof(sinServer));

        sockaddr_in _sinClient;
        int _iClientLength = sizeof(_sinClient);

        fd_set readfds_udp;                         // declare a read set
        struct timeval timeout_udp;                 // declare a timeval for our timer
        int iRet = -1;

        FD_ZERO(&readfds_udp);                      // zero out the read set
        FD_SET(_sockudp, &readfds_udp);                // add socket to the read set
        timeout_udp.tv_sec = 0;                     // timeout = 0 seconds
        timeout_udp.tv_usec = 50000;               // timeout += 0.05 seconds
        int _iN = select(_sockudp + 1, &readfds_udp, NULL, NULL, &timeout_udp);
        if (_iN > 0) {
          /** do recvfrom stuff **/
          iRet =  recvfrom(_sockudp, _szBuffer, 2048, 0, (sockaddr*)&_sinClient, &_iClientLength);
          FD_CLR(_sockudp, &readfds_udp);
          if(iRet != -1 && iRet > 100 && iRet != SOCKET_ERROR) {
            // null terminate the buffer
            _szBuffer[iRet] = 0;
            char *sPch = NULL;
            sPch = strstr(_szBuffer, "\\gamename\\serioussamse\\");
            if(!sPch) {
                CPrintF("Unknown query server response!\n");
                return -1;
            } else {

                CTString strPlayers;
                CTString strMaxPlayers;
                CTString strLevel;
                CTString strGameType;
                CTString strVersion;
                CTString strGameName;
                CTString strSessionName;

                CTString strGamePort;
                CTString strServerLocation;
                CTString strGameMode;
                CTString strActiveMod;

                CHAR* pszPacket = _szBuffer + 1; // we do +1 because the first character is always '\', which we don't care about.

                BOOL bReadValue = FALSE;
                CTString strKey;
                CTString strValue;

                while(*pszPacket != 0) {
                switch(*pszPacket) {
                case '\\':
                    if(strKey != "gamemode") {
                      if(bReadValue) {
                        // we're done reading the value, check which key it was
                        if(strKey == "gamename") {
                            strGameName = strValue;
                        } else if(strKey == "gamever") {
                            strVersion = strValue;
                        } else if(strKey == "location") {
                            strServerLocation = strValue;
                        } else if(strKey == "hostname") {
                            strSessionName = strValue;
                        } else if(strKey == "hostport") {
                            strGamePort = strValue;
                        } else if(strKey == "mapname") {
                            strLevel = strValue;
                        } else if(strKey == "gametype") {
                            strGameType = strValue;
                        } else if(strKey == "activemod") {
                            strActiveMod = strValue;
                        } else if(strKey == "numplayers") {
                            strPlayers = strValue;
                        } else if(strKey == "maxplayers") {
                            strMaxPlayers = strValue;
                        } else {
                            //CPrintF("Unknown GameAgent parameter key '%s'!", strKey);
                        }
                        // reset temporary holders
                        strKey = "";
                        strValue = "";
                      }
                    }
                    bReadValue = !bReadValue;
                    break;

                default:
                    // read into the value or into the key, depending where we are
                    if(bReadValue) {
                        strValue.InsertChar(strlen(strValue), *pszPacket);
                    } else {
                        strKey.InsertChar(strlen(strKey), *pszPacket);
                    }
                    break;
                  }
                  // move to next character
                  pszPacket++;
                }

                // check if we still have a maxplayers to back up
                if(strKey == "gamemode") {
                    strGameMode = strValue;
                }
                if(strActiveMod != "") {
                    strGameName = strActiveMod;
                }
				
                long long tmPing = -1;
                // find the request in the request array
                for(INDEX i=0; i<ga_asrRequests.Count(); i++) {
                    CServerRequest &req = ga_asrRequests[i];
                    if(req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
                        tmPing = _pTimer->GetHighPrecisionTimer().GetMilliseconds() - req.sr_tmRequestTime;
                        ga_asrRequests.Delete(&req);
                        break;
                    }
                }

                if(tmPing > 0 && tmPing < 2500000) {
				    // insert the server into the serverlist
                    CNetworkSession &ns = *new CNetworkSession;
                    _pNetwork->ga_lhEnumeratedSessions.AddTail(ns.ns_lnNode);
					
                    // add the server to the serverlist
                    ns.ns_strSession = strSessionName;
                    ns.ns_strAddress = inet_ntoa(_sinClient.sin_addr) + CTString(":") + CTString(0, "%d", htons(_sinClient.sin_port) - 1);
                    ns.ns_tmPing = (tmPing / 1000.0f);
                    ns.ns_strWorld = strLevel;
                    ns.ns_ctPlayers = atoi(strPlayers);
                    ns.ns_ctMaxPlayers = atoi(strMaxPlayers);
                    ns.ns_strGameType = strGameType;
                    ns.ns_strMod = strGameName;
                    ns.ns_strVer = strVersion;
                }
            }
          } else {
            // find the request in the request array
            for(INDEX i=0; i<ga_asrRequests.Count(); i++) {
              CServerRequest &req = ga_asrRequests[i];
              if(req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
                ga_asrRequests.Delete(&req);
                break;
              }
            }
          }
        }
        pServerIP++;
        _iIPPortBufferLen -= 6;
    }
    if(_szIPPortBuffer) free (_szIPPortBuffer);
    _szIPPortBuffer = NULL;

    closesocket(_sockudp);
    _uninitWinsock();
    _bInitialized = FALSE;
    _pNetwork->ga_bEnumerationChange = FALSE;
    WSACleanup();
    return 0;
}

DWORD WINAPI _LocalNet_Thread(LPVOID lpParam) {
    SOCKET _sockudp = NULL;
    struct _sIPPort {
        UBYTE bFirst;
        UBYTE bSecond;
        UBYTE bThird;
        UBYTE bFourth;
        USHORT iPort;
      };

    _sockudp = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockudp == INVALID_SOCKET){
        WSACleanup();
        _pNetwork->ga_strEnumerationStatus = "";
		if(_szIPPortBufferLocal != NULL) {
			delete[] _szIPPortBufferLocal;
		}
		_szIPPortBufferLocal = NULL;		
		return -1;
    }

    _sIPPort* pServerIP = (_sIPPort*)(_szIPPortBufferLocal);
    while(_iIPPortBufferLocalLen >= 6) {
        if(!strncmp((char *)pServerIP, "\\final\\", 7)) {
                break;
            }

        _sIPPort ip = *pServerIP;

        CTString strIP;
        strIP.PrintF("%d.%d.%d.%d", ip.bFourth, ip.bThird, ip.bSecond, ip.bFirst);

        sockaddr_in sinServer;
        sinServer.sin_family = AF_INET;
        sinServer.sin_addr.s_addr = inet_addr(strIP);
        sinServer.sin_port = ip.iPort;

        // insert server status request into container
        CServerRequest &sreq = ga_asrRequests.Push();
        sreq.sr_ulAddress = sinServer.sin_addr.s_addr;
        sreq.sr_iPort = sinServer.sin_port;
        sreq.sr_tmRequestTime = _pTimer->GetHighPrecisionTimer().GetMilliseconds();

        // send packet to server
        sendto(_sockudp,"\\status\\",8,0,
            (sockaddr *) &sinServer, sizeof(sinServer));

        sockaddr_in _sinClient;
        int _iClientLength = sizeof(_sinClient);

        fd_set readfds_udp;                         // declare a read set
        struct timeval timeout_udp;                 // declare a timeval for our timer
        int iRet = -1;

        FD_ZERO(&readfds_udp);                      // zero out the read set
        FD_SET(_sockudp, &readfds_udp);             // add socket to the read set
        timeout_udp.tv_sec = 0;                     // timeout = 0 seconds
        timeout_udp.tv_usec = 50000;                // timeout += 0.05 seconds
        int _iN = select(_sockudp + 1, &readfds_udp, NULL, NULL, &timeout_udp);
        if (_iN > 0) {
          /** do recvfrom stuff **/
          iRet =  recvfrom(_sockudp, _szBuffer, 2048, 0, (sockaddr*)&_sinClient, &_iClientLength);
          FD_CLR(_sockudp, &readfds_udp);
          if(iRet != -1 && iRet > 100 && iRet != SOCKET_ERROR) {
            // null terminate the buffer
            _szBuffer[iRet] = 0;
            char *sPch = NULL;
            sPch = strstr(_szBuffer, "\\gamename\\serioussamse\\");
            if(!sPch) {
                CPrintF("Unknown query server response!\n");
				if(_szIPPortBufferLocal != NULL) {
					delete[] _szIPPortBufferLocal;
				}
				_szIPPortBufferLocal = NULL;               
				WSACleanup();
				return -1;
            } else {

                CTString strPlayers;
                CTString strMaxPlayers;
                CTString strLevel;
                CTString strGameType;
                CTString strVersion;
                CTString strGameName;
                CTString strSessionName;

                CTString strGamePort;
                CTString strServerLocation;
                CTString strGameMode;
                CTString strActiveMod;

                CHAR* pszPacket = _szBuffer + 1; // we do +1 because the first character is always '\', which we don't care about.

                BOOL bReadValue = FALSE;
                CTString strKey;
                CTString strValue;

                while(*pszPacket != 0) {
                switch(*pszPacket) {
                case '\\':
                    if(strKey != "gamemode") {
                      if(bReadValue) {
                        // we're done reading the value, check which key it was
                        if(strKey == "gamename") {
                            strGameName = strValue;
                        } else if(strKey == "gamever") {
                            strVersion = strValue;
                        } else if(strKey == "location") {
                            strServerLocation = strValue;
                        } else if(strKey == "hostname") {
                            strSessionName = strValue;
                        } else if(strKey == "hostport") {
                            strGamePort = strValue;
                        } else if(strKey == "mapname") {
                            strLevel = strValue;
                        } else if(strKey == "gametype") {
                            strGameType = strValue;
                        } else if(strKey == "activemod") {
                            strActiveMod = strValue;
                        } else if(strKey == "numplayers") {
                            strPlayers = strValue;
                        } else if(strKey == "maxplayers") {
                            strMaxPlayers = strValue;
                        } else {
							//CPrintF("Unknown GameAgent parameter key '%s'!", strKey);
                        }
                        // reset temporary holders
                        strKey = "";
                        strValue = "";
                      }
                    }
                    bReadValue = !bReadValue;
                    break;

                default:
                    // read into the value or into the key, depending where we are
                    if(bReadValue) {
                        strValue.InsertChar(strlen(strValue), *pszPacket);
                    } else {
                        strKey.InsertChar(strlen(strKey), *pszPacket);
                    }
                    break;
                  }
                  // move to next character
                  pszPacket++;
                }

                // check if we still have a maxplayers to back up
                if(strKey == "gamemode") {
                    strGameMode = strValue;
                }
                if(strActiveMod != "") {
                    strGameName = strActiveMod;
                }

                long long tmPing = -1;
                // find the request in the request array
                for(INDEX i=0; i<ga_asrRequests.Count(); i++) {
                    CServerRequest &req = ga_asrRequests[i];
                    if(req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
                        tmPing = _pTimer->GetHighPrecisionTimer().GetMilliseconds() - req.sr_tmRequestTime;
                        ga_asrRequests.Delete(&req);
                        break;
                    }
                }

                if(tmPing > 0 && tmPing < 2500000) {
				    // insert the server into the serverlist
                    _pNetwork->ga_strEnumerationStatus = "";
					CNetworkSession &ns = *new CNetworkSession;
                    _pNetwork->ga_lhEnumeratedSessions.AddTail(ns.ns_lnNode);
					
                    // add the server to the serverlist
                    ns.ns_strSession = strSessionName;
                    ns.ns_strAddress = inet_ntoa(_sinClient.sin_addr) + CTString(":") + CTString(0, "%d", htons(_sinClient.sin_port) - 1);
                    ns.ns_tmPing = (tmPing / 1000.0f);
                    ns.ns_strWorld = strLevel;
                    ns.ns_ctPlayers = atoi(strPlayers);
                    ns.ns_ctMaxPlayers = atoi(strMaxPlayers);
                    ns.ns_strGameType = strGameType;
                    ns.ns_strMod = strGameName;
                    ns.ns_strVer = strVersion;
                }
            }
          } else {
            // find the request in the request array
            for(INDEX i=0; i<ga_asrRequests.Count(); i++) {
              CServerRequest &req = ga_asrRequests[i];
              if(req.sr_ulAddress == _sinClient.sin_addr.s_addr && req.sr_iPort == _sinClient.sin_port) {
                ga_asrRequests.Delete(&req);
                break;
              }
            }
          }
        }
        pServerIP++;
        _iIPPortBufferLocalLen -= 6;
    }
	if(_szIPPortBufferLocal != NULL) {
      delete[] _szIPPortBufferLocal;
    }
	_szIPPortBufferLocal = NULL;
	
    closesocket(_sockudp);
    _uninitWinsock();
    _bInitialized = FALSE;
    _pNetwork->ga_bEnumerationChange = FALSE;
	_pNetwork->ga_strEnumerationStatus = "";
    WSACleanup();
    return 0;
}