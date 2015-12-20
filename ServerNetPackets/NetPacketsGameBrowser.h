#pragma once

#include "r3dNetwork.h"
#include "NetPacketsGameInfo.h"

namespace NetPacketsGameBrowser
{
#pragma pack(push)
#pragma pack(1)

#define GBNET_VERSION		(0x0000000F + GBGAMEINFO_VERSION)
#define GBNET_KEY1		0x45AB6541

//
// Game Browser Packet IDs
// 
enum gbpktType_e
{
  GBPKT_ValidateConnectingPeer = r3dNetwork::FIRST_FREE_PACKET_ID,

  // client vs master server codes
  GBPKT_C2M_RefreshList,
  GBPKT_M2C_StartGamesList,
  GBPKT_M2C_SupervisorData,
  GBPKT_M2C_GameData,
  GBPKT_M2C_EndGamesList,

  GBPKT_C2M_JoinGameReq,
  GBPKT_C2M_QuickGameReq,
  GBPKT_M2C_JoinGameAns,
  
  GBPKT_C2M_MyServerInfoReq,
  GBPKT_M2C_MyServerAddPlayer,
  GBPKT_M2C_MyServerInfoAns,
  GBPKT_C2M_MyServerKickPlayer,
  GBPKT_C2M_MyServerSetParams,

  GBPKT_M2C_MasterInfo,
  GBPKT_M2C_ShutdownNote,

  GBPKT_LAST_PACKET_ID,
};
#if GBPKT_LAST_PACKET_ID > 255
  #error Shit happens, more that 255 packet ids
#endif

#ifndef CREATE_PACKET
#define CREATE_PACKET(PKT_ID, VAR) PKT_ID##_s VAR
#endif

struct GBPKT_ValidateConnectingPeer_s : public r3dNetPacketMixin<GBPKT_ValidateConnectingPeer>
{
	DWORD		version;
	DWORD		key1;
};

struct GBPKT_C2M_RefreshList_s : public r3dNetPacketMixin<GBPKT_C2M_RefreshList>
{
	BYTE		region;

	// filters from client
	BYTE		browseChannel; // 1-TRIAL, 2-OFFICIAL, 3-PRIVATE, 4-PREMIUM, 5-STRONGHOLDS, 6-PUBLIC TEST ENVIRONMENT
	bool		hideempty;
	bool		hidefull;

	bool		enable_options;
	bool		tracers2;
	bool		nameplates2;
	bool		crosshair2;
	bool		password;
	int			timeLimit;
};

struct GBPKT_M2C_StartGamesList_s : public r3dNetPacketMixin<GBPKT_M2C_StartGamesList>
{
};

struct GBPKT_M2C_SupervisorData_s : public r3dNetPacketMixin<GBPKT_M2C_SupervisorData>
{
	GBPKT_M2C_SupervisorData_s& operator=(const GBPKT_M2C_SupervisorData_s& rhs) {
          memcpy(this, &rhs, sizeof(*this));
          return *this;
	}

	BYTE		region;
	WORD		ID;
	DWORD		ip;
};

struct GBPKT_M2C_GameData_s : public r3dNetPacketMixin<GBPKT_M2C_GameData>
{
	GBPKT_M2C_GameData_s& operator=(const GBPKT_M2C_GameData_s& rhs) {
          memcpy(this, &rhs, sizeof(*this));
          return *this;
	}

	WORD		superId; // ID of supervisor
	GBGameInfo	info;
	BYTE		status;  // not used
	WORD		curPlayers;
};

struct GBPKT_M2C_EndGamesList_s : public r3dNetPacketMixin<GBPKT_M2C_EndGamesList>
{
	DWORD		numFiltered;
};

struct GBPKT_C2M_JoinGameReq_s : public r3dNetPacketMixin<GBPKT_C2M_JoinGameReq>
{
	GBPKT_C2M_JoinGameReq_s() {
	  pwd[0] = 0;
	}
	
	DWORD		CustomerID;
	DWORD		gameServerId;
	char		pwd[16];
};

struct GBPKT_C2M_QuickGameReq_s : public r3dNetPacketMixin<GBPKT_C2M_QuickGameReq>
{
	DWORD		CustomerID;
	BYTE		region;		// EGBGameRegion
	BYTE		gameMap;	// 0xFF for any map
	BYTE		browseChannel; // see GBPKT_C2M_RefreshList_s
	int			playerGameTime; // to filter games that have gametime limit
};

struct GBPKT_M2C_JoinGameAns_s : public r3dNetPacketMixin<GBPKT_M2C_JoinGameAns>
{
	GBPKT_M2C_JoinGameAns_s& operator=(const GBPKT_M2C_JoinGameAns_s &rhs) {
          memcpy(this, &rhs, sizeof(*this));
          return *this;
	}

	enum {
	  rUnknown = 0,
	  rNoGames,
	  rGameFull,
	  rGameFinished,
	  rGameNotFound,
	  rGameStarting,
	  rWrongPassword,
	  rLevelTooLow,
	  rLevelTooHigh,
	  rJoinDelayActive,
	  rOk,
	  rMasterStarting,
	};
	GBPKT_M2C_JoinGameAns_s() {
	  result = rUnknown;
	}

	BYTE		result;
	DWORD		ip;
	WORD		port;
	__int64		sessionId;
};

struct GBPKT_C2M_MyServerInfoReq_s : public r3dNetPacketMixin<GBPKT_C2M_MyServerInfoReq>
{
	DWORD		gameServerId;
	DWORD		AdminKey;
};

struct GBPKT_M2C_MyServerAddPlayer_s : public r3dNetPacketMixin<GBPKT_M2C_MyServerAddPlayer>
{
	GBPKT_M2C_MyServerAddPlayer_s& operator=(const GBPKT_M2C_MyServerAddPlayer_s &rhs) {
          memcpy(this, &rhs, sizeof(*this));
          return *this;
	}

	DWORD		CharID;
	char		gamertag[32*2];
	int		reputation;
	int		XP;
};

struct GBPKT_M2C_MyServerInfoAns_s : public r3dNetPacketMixin<GBPKT_M2C_MyServerInfoAns>
{
	BYTE		status;	// 1 - not registered yet, 2 - offline, 3 - online
};

struct GBPKT_C2M_MyServerKickPlayer_s : public r3dNetPacketMixin<GBPKT_C2M_MyServerKickPlayer>
{
	DWORD		gameServerId;
	DWORD		AdminKey;
	DWORD		CharID;
};

struct GBPKT_C2M_MyServerSetParams_s : public r3dNetPacketMixin<GBPKT_C2M_MyServerSetParams>
{
	DWORD		gameServerId;
	DWORD		AdminKey;
	int			gameTimeLimit;
	char		pwd[16];
	BYTE		flags;
};

struct GBPKT_M2C_MasterInfo_s : public r3dNetPacketMixin<GBPKT_M2C_MasterInfo>
{
	BYTE		serverId;
};

struct GBPKT_M2C_ShutdownNote_s : public r3dNetPacketMixin<GBPKT_M2C_ShutdownNote>
{
	BYTE		reason;
};

#pragma pack(pop)

}; // namespace NetPacketsGameBrowser
