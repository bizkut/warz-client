#pragma once

#include "r3dProtect.h"
#include "r3dNetwork.h"
#include "multiplayer/P2PMessages.h"

class GameObject;
class obj_Player;

#include "../../ServerNetPackets/NetPacketsGameInfo.h"

#define FULL_AREA_EXPLOSION  360.0f

class ClientGameLogic : public r3dNetCallback
{
  protected:
	r3dNetwork	g_net;
	// r3dNetCallback virtuals
virtual	void		OnNetPeerConnected(DWORD peerId);
virtual	void		OnNetPeerDisconnected(DWORD peerId);
virtual	void		OnNetData(DWORD peerId, const r3dNetPacketHeader* packetData, int packetSize);

private:
	wiInventoryItem devEventItems[wiCharDataFull::CHAR_MAX_BACKPACK_SIZE];
	int				devEventItemsCount;
	int				devEventBackpackId;
	void			GiveDevEventLoadout();

public:
	void			LoadDevEventLoadout();

public:
  public:
	// player struct for hiding actual player pointer
	struct {
		r3dSec_type<obj_Player*, 0x4A6389C3> ptr;
	}		players2_[MAX_NUM_PLAYERS];
	obj_Player*	GetPlayer(int idx) const;
	void		SetPlayerPtr(int idx, obj_Player* ptr);
	int		CurMaxPlayerIdx;	// current max used index in players2_
	
	// list of players on server
	struct PlayerName_s
	{
		int	reputation;
		int	groupFlags; //1-pending invite, 2-in group with you
		int	clanID;
		float	lastPosX;
		float	lastPosZ;
		char	Gamertag[32*2];
		BYTE	accountFlags;
		char	c4h_distress[1024];
		char	c4h_reward[512];
		float	c4h_activeUntil;
		float	c4h_locX;
		float	c4h_locZ;
		bool	DevIsHide;
		
		int	voiceClientID;
		bool	voiceIsMuted;		// muted by our request
		bool	voiceEnabled;		// if 0 then muted because of distance
	};
	PlayerName_s	playerNames[MAX_NUM_PLAYERS];

	DWORD		net_lastFreeId; // !!!ONLY use it for assigning networkID during loading map!!!
	float		localPlayerConnectedTime; // used for calculating how much time player was playing

	r3dSec_type<obj_Player*, 0x836F8147> localPlayer_;
	bool		serverConnected_;
	volatile LONG	serverVersionStatus_;
	volatile int	gameJoinAnswered_;
	int		localPlayerIdx_;
	volatile bool	gameStartAnswered_;
	int		gameStartResult_; // as PKT_S2C_StartGameAns_s::EResult
	GBGameInfo	m_gameInfo;
	uint32_t	m_gameCreatorCustomerID; // customerID of server owner, if any
	DWORD		m_gameVoicePwd;
	DWORD		m_gameVoiceId;

	BYTE		isGameStarted; //gamehardcore

	__int64		gameServerTimeOffset;
	__int64		gameStartUtcTime_;
	float		lastShadowCacheReset_;
	float		gameStartTime_;
	__int64		GetServerGameTime() const;
	void		UpdateTimeOfDay();
	
	__int64		m_sessionId;
	__int64		GetGameSessionID() {
		if(serverConnected_)
			return m_sessionId;
		else
			return 0;
	}

	//float	m_highPingTimer;

	int		disconnectStatus_;	// 0 - playing, 1 - requested, 2 - acked
	float		disconnectReqTime_;	// time when disconnect was requested
	
	bool		gameShuttedDown_;
	bool		gameDoNotAllowToPlaceObjects; // when server is shutting down, do not allow to place any objects due to potential dupe when server is closed
	
	// cheat things
	DWORD		nextSecTimeReport_;	// time when we need send next PKT_C2S_SecurityRep_s
	DWORD		gppDataSeed_;		// seed for sending crc of game player parameters
	bool		d3dCheatSent2_;
	float		nextTimeToSendCameraPos;
	
	#define DEFINE_PACKET_FUNC(XX) \
	  void On##XX(const XX##_s& n, GameObject* fromObj, DWORD peerId, bool& needPassThru);
	#define IMPL_PACKET_FUNC(CLASS, XX) \
	  void CLASS::On##XX(const XX##_s& n, GameObject* fromObj, DWORD peerId, bool& needPassThru)
	//  
	int		ProcessWorldEvent(GameObject* fromObj, DWORD eventId, DWORD peerId, const void* packetData, int packetSize);
	 DEFINE_PACKET_FUNC(PKT_C2S_ValidateConnectingPeer);
	 DEFINE_PACKET_FUNC(PKT_C2C_PacketBarrier);
	 DEFINE_PACKET_FUNC(PKT_S2C_JoinGameAns);
	 DEFINE_PACKET_FUNC(PKT_S2C_ShutdownNote);
	 DEFINE_PACKET_FUNC(PKT_S2C_SetGamePlayParams);
	 DEFINE_PACKET_FUNC(PKT_S2C_SetGameInfoFlags);
	 DEFINE_PACKET_FUNC(PKT_S2C_StartGameAns);
	 DEFINE_PACKET_FUNC(PKT_S2C_PlayerNameJoined);
	 DEFINE_PACKET_FUNC(PKT_S2C_PlayerNameLeft);
	 DEFINE_PACKET_FUNC(PKT_S2C_PlayerStatusUpdate);
	 DEFINE_PACKET_FUNC(PKT_S2C_CreatePlayer);
	 DEFINE_PACKET_FUNC(PKT_S2C_Damage);
	 DEFINE_PACKET_FUNC(PKT_S2C_ZombieAttack);
	 DEFINE_PACKET_FUNC(PKT_S2C_KillPlayer);
	 DEFINE_PACKET_FUNC(PKT_C2S_DisconnectReq);
	 DEFINE_PACKET_FUNC(PKT_C2C_ChatMessage);
	 DEFINE_PACKET_FUNC(PKT_S2C_UpdateWeaponData);
	 DEFINE_PACKET_FUNC(PKT_S2C_UpdateGearData);
	 DEFINE_PACKET_FUNC(PKT_S2C_UpdateAttmData);
	 DEFINE_PACKET_FUNC(PKT_S2C_CreateNetObject);
	 DEFINE_PACKET_FUNC(PKT_C2S_GetUAV); // UAV Code
	 DEFINE_PACKET_FUNC(PKT_S2C_DestroyNetObject);
	 DEFINE_PACKET_FUNC(PKT_S2C_CreateDroppedItem);
	 DEFINE_PACKET_FUNC(PKT_S2C_CreateNote);
	 DEFINE_PACKET_FUNC(PKT_S2C_SetNoteData);
	 DEFINE_PACKET_FUNC(PKT_S2C_SetupTraps);
	 DEFINE_PACKET_FUNC(PKT_S2C_CreateGravestone);
	 DEFINE_PACKET_FUNC(PKT_S2C_UseNetObjectAns);
	 DEFINE_PACKET_FUNC(PKT_C2S_ReviveFast);
#ifdef MISSIONS
#ifdef MISSION_TRIGGERS
	 DEFINE_PACKET_FUNC(PKT_S2C_CreateMissionTrigger);
	 DEFINE_PACKET_FUNC(PKT_S2C_ShowMissionTrigger);
#endif
	 DEFINE_PACKET_FUNC(PKT_S2C_MissionActivate);
	 DEFINE_PACKET_FUNC(PKT_S2C_MissionComplete);
	 DEFINE_PACKET_FUNC(PKT_S2C_MissionRemove);
	 DEFINE_PACKET_FUNC(PKT_S2C_MissionMapUpdate);
	 DEFINE_PACKET_FUNC(PKT_S2C_MissionActionUpdate);
#endif
	 DEFINE_PACKET_FUNC(PKT_S2C_AirDropMapUpdate);
	 DEFINE_PACKET_FUNC(PKT_S2C_CreateZombie);
	 DEFINE_PACKET_FUNC(PKT_S2C_CamuDataS);
	 DEFINE_PACKET_FUNC(PKT_S2C_CheatWarning);
	 DEFINE_PACKET_FUNC(PKT_S2C_GroupAns);
	 DEFINE_PACKET_FUNC(PKT_S2C_GroupInviteReceived);
	 DEFINE_PACKET_FUNC(PKT_S2C_GroupAddMember);
	 DEFINE_PACKET_FUNC(PKT_S2C_GroupRemoveMember);
	 DEFINE_PACKET_FUNC(PKT_S2C_GroupNewLeader);
	 DEFINE_PACKET_FUNC(PKT_S2C_GroupNotify);
	 DEFINE_PACKET_FUNC(PKT_S2C_CallForHelpAns);
	 DEFINE_PACKET_FUNC(PKT_S2C_CallForHelpEvent);
	 DEFINE_PACKET_FUNC(PKT_S2C_DropItemYPosition);
	 DEFINE_PACKET_FUNC(PKT_S2C_SendDataDoor);
	 DEFINE_PACKET_FUNC(PKT_S2C_LockboxOpReq);
	 DEFINE_PACKET_FUNC(PKT_S2C_LockboxContent);
	 DEFINE_PACKET_FUNC(PKT_S2C_CustomKickMsg);
	 DEFINE_PACKET_FUNC(PKT_S2C_Test_UpdateClientTime);
	 DEFINE_PACKET_FUNC(PKT_S2C_AirDropOnMap);
	 DEFINE_PACKET_FUNC(PKT_C2C_DevHidePlayer);
#ifdef VEHICLES_ENABLED
	 DEFINE_PACKET_FUNC(PKT_S2C_VehicleSpawn);
	 //DEFINE_PACKET_FUNC(PKT_C2S_TurrerAngles);
	 DEFINE_PACKET_FUNC(PKT_S2C_VehicleEntered);
	 DEFINE_PACKET_FUNC(PKT_S2C_VehicleExited);
	 DEFINE_PACKET_FUNC(PKT_S2C_VehicleHitTargetFail);
#endif

	 void		OnPKT_S2C_PunkBuster(DWORD peerId, const int size, const char* data);

	r3dPoint3D	AdjustSpawnPositionToGround(const r3dPoint3D& pos);

  protected:
	typedef bool (ClientGameLogic::*fn_wait)();
	int		WaitFunc(fn_wait fn, float timeout, const char* msg);
	
	// wait functions
	bool		wait_IsConnected() {
	  return net_->IsConnected();
	}
	bool		wait_ValidateVersion() {
          return serverVersionStatus_ != 0;
        }
	bool		wait_GameJoin() {
	  return gameJoinAnswered_ != 0;
	}
	bool		wait_GameStart();

  private:	
	// make copy constructor and assignment operator inaccessible
	ClientGameLogic(const ClientGameLogic& rhs);
	ClientGameLogic& operator=(const ClientGameLogic& rhs);

  private: // this is singleton, can't create directly.
	ClientGameLogic();
	virtual ~ClientGameLogic();
	
  public:
	static void CreateInstance();
	static void DeleteInstance();
	static ClientGameLogic* GetInstance();

	void		Reset();

	bool		Connect(const char* host, int port);
	void		Disconnect();

	int		RequestToJoinGame();

	int		RequestToStartGame();
	int		ValidateServerVersion(__int64 sessionId);
	
	void		Tick();
	void		SendScreenshotToServer(const char* FoundPlayer);
	void		SendScreenshot(IDirect3DTexture9* texture,const char* FoundPlayer);
	void		 SendScreenshotFailed(int code);
};

__forceinline ClientGameLogic& gClientLogic() {
	return *ClientGameLogic::GetInstance();
}

