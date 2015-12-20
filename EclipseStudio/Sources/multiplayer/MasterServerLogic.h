#pragma once

#include "r3dNetwork.h"
#include "../../ServerNetPackets/NetPacketsGameBrowser.h"
using namespace NetPacketsGameBrowser;

class MasterServerLogic : public r3dNetCallback
{
  protected:
	CRITICAL_SECTION csNetwork;
	r3dNetwork	g_net;
	bool		isConnected_;

	// r3dNetCallback virtuals
virtual	void		OnNetPeerConnected(DWORD peerId);
virtual	void		OnNetPeerDisconnected(DWORD peerId);
virtual	void		OnNetData(DWORD peerId, const r3dNetPacketHeader* packetData, int packetSize);

  public:
	volatile int	masterServerId_;
  
	volatile bool	gameListReceived_;
	r3dgameVector(GBPKT_M2C_SupervisorData_s) supers_;
	r3dgameVector(GBPKT_M2C_GameData_s) games_;
	const GBPKT_M2C_GameData_s* FindGameById(DWORD gameServerId);
	
	volatile bool	gameJoinAnswered_;
	GBPKT_M2C_JoinGameAns_s gameJoinAnswer_;
	
	volatile bool	serverInfoAnswered_;
	GBPKT_M2C_MyServerInfoAns_s serverInfoAnswer_;
	std::vector<GBPKT_M2C_MyServerAddPlayer_s> serverInfoPlayers_;
	
	volatile bool	versionChecked_;
	volatile bool	shuttingDown_;
	// this should not be reinited on connect()
	// bad version should be fatal error
	bool		badClientVersion_;

  protected:
	typedef bool (MasterServerLogic::*fn_wait)();
	int		WaitFunc(fn_wait fn, float timeout, const char* msg);
	
	// wait functions
	bool		wait_GameListReceived() {
	  return gameListReceived_;
	}
	bool		wait_GameJoinAsnwer() {
	  return gameJoinAnswered_;
	}
	
  public:
	MasterServerLogic();
	virtual ~MasterServerLogic();

	int		StartConnect(const char* host, int port);
	void		Disconnect();

	bool		IsConnected() { return isConnected_; }
	
	void		RequestGameList(const NetPacketsGameBrowser::GBPKT_C2M_RefreshList_s& n);
	int		WaitForGameList();

	void		SendJoinGame(DWORD gameServerId, const char* pwd = "");
	void		SendJoinQuickGame(const NetPacketsGameBrowser::GBPKT_C2M_QuickGameReq_s& n);
	int		WaitForGameJoin();
	void		GetJoinedGameServer(char* out_ip, int* out_port, __int64* out_sessionId);
	
	void		SendMyServerGetInfo(DWORD gameServerId, DWORD AdminKey);
	void		SendMyServerKickPlayer(DWORD gameServerId, DWORD AdminKey, DWORD CharID);
	void		SendMyServerSetParams(DWORD gameServerId, DWORD AdminKey, const char* pwd, int flags, int gameTimeLimit);
	
	void		Tick();
};

extern	MasterServerLogic gMasterServerLogic;
