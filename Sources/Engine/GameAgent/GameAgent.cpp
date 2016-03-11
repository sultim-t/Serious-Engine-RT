/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

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

#pragma comment(lib, "wsock32.lib")

WSADATA* _wsaData = NULL;
SOCKET _socket = NULL;

sockaddr_in* _sin = NULL;
sockaddr_in* _sinLocal = NULL;
sockaddr_in _sinFrom;

CHAR* _szBuffer = NULL;

BOOL _bServer = FALSE;
static BOOL _bInitialized = FALSE;

TIME _tmLastHeartbeat = 0;

CDynamicStackArray<CServerRequest> ga_asrRequests;

extern CTString ga_strServer = "master1.croteam.org";

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
  _szBuffer = new char[1400];

  // start WSA
  if(WSAStartup(MAKEWORD(2, 2), _wsaData) != 0) {
    CPrintF("Error initializing winsock!\n");
    _uninitWinsock();
    return;
  }

  // get the host IP
  hostent* phe = gethostbyname(ga_strServer);

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
  _sin->sin_port = htons(9005);

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
  return recvfrom(_socket, _szBuffer, 1024, 0, (sockaddr*)&_sinFrom, &fromLength);
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
  strPacket.PrintF("0;challenge;%d;players;%d;maxplayers;%d;level;%s;gametype;%s;version;%s;product;%s",
    iChallenge,
    _pNetwork->ga_srvServer.GetPlayersCount(),
    _pNetwork->ga_sesSessionState.ses_ctMaxPlayers,
    _pNetwork->ga_World.wo_strName,
    _getGameModeName(_getSP()->sp_gmGameMode),
    _SE_VER_STRING,
    _pShell->GetString("sam_strGameName"));
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
  _sendPacket("q");
}

/// Let GameAgent know that the server has stopped.
extern void GameAgent_ServerEnd(void)
{
  if (!_bInitialized) {
    return;
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
            if(strlen(strPacket) + strlen(strPlayer) > 1024) {
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
  }

  // send a heartbeat every 150 seconds
  if(_pTimer->GetRealTimeTick() - _tmLastHeartbeat >= 150.0f) {
    _sendHeartbeat(0);
  }
}

/// Notify GameAgent that the server state has changed.
extern void GameAgent_ServerStateChanged(void)
{
  if (_bInitialized) {
    _sendPacket("u");
  }
}

/// Request serverlist enumeration.
extern void GameAgent_EnumTrigger(BOOL bInternet)
{
  if (!_bInitialized) {
    return;
  }
  // make sure that there are no requests still stuck in buffer
  ga_asrRequests.Clear();

  // we're not a server
  _bServer = FALSE;

  // send enumeration packet to masterserver
  _sendPacket("e");
  _setStatus("");
}

/// GameAgent client update for enumerations.
extern void GameAgent_EnumUpdate(void)
{
  if((_socket == NULL) || (!_bInitialized)) {
    return;
  }

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
}

/// Cancel the GameAgent serverlist enumeration.
extern void GameAgent_EnumCancel(void)
{
  if (_bInitialized) {
    ga_asrRequests.Clear();
    _uninitWinsock();
  }
}
