#include "r3dPCH.h"
#include "r3d.h"

#include "ClientGameLogic.h"

#include "GameObjects/ObjManag.h"
#include "GameObjects/EventTransport.h"
#include "MasterServerLogic.h"

#include "multiplayer/P2PMessages.h"
#include "ObjectsCode/Gameplay/obj_UAV.h"

#include "ObjectsCode/AI/AI_Player.h"
#include "ObjectsCode/Gameplay/BasePlayerSpawnPoint.h"
#include "ObjectsCode/Gameplay/obj_DroppedItem.h"
#include "ObjectsCode/Gameplay/obj_Gravestone.h"
#include "ObjectsCode/Gameplay/obj_MissionTrigger.h"
#include "ObjectsCode/Gameplay/obj_Note.h"
#include "ObjectsCode/Gameplay/obj_Door.h"
#include "ObjectsCode/Gameplay/obj_Traps.h"
#include "ObjectsCode/Gameplay/obj_Zombie.h"

// CAT todo - relocate obj_Vehicle to a more organized player, such as where obj_Zombie is
#include "../GameEngine/gameobjects/obj_Vehicle.h"

#include "ObjectsCode/weapons/Weapon.h"
#include "ObjectsCode/weapons/WeaponArmory.h"
#include "ObjectsCode/weapons/Ammo.h"
#include "ObjectsCode/weapons/Barricade.h"
#include "ObjectsCode/weapons/FarmBlock.h"
#include "ObjectsCode/weapons/Grenade.h"
#include "ObjectsCode/weapons/Safelock.h"

#include "Rendering/Deffered/D3DMiscFunctions.h"

#include "GameCode/UserProfile.h"
#include "GameCode/UserSettings.h"
#include "Gameplay_Params.h"
#include "GameLevel.h"

#include "ui/m_LoadingScreen.h"
#include "ui/HUDDisplay.h"
#include "ui/HUDPause.h"
#include "ui/HUDSafelock.h"
#include "ui/HUDTrade.h"

#include "HWInfo.h"
extern CHWInfo g_HardwareInfo;

extern IDirect3DTexture9* _r3d_screenshot_copy;
extern char FoundPlayerCheat[512];

// PunkBuster SDK
#ifdef __WITH_PB__
#include "../../../External/PunkBuster/pbcommon.h"
#endif

extern HUDDisplay*	hudMain;
extern HUDPause*	hudPause;
extern HUDSafelock*	hudSafelock;
extern HUDTrade*	hudTrade;
extern int g_RenderScopeEffect;

extern float getWaterDepthAtPos(const r3dPoint3D& pos);

// VMProtect code block
#if USE_VMPROTECT
  #pragma optimize("g", off)
#endif

static r3dSec_type<ClientGameLogic*, 0x176A1435> g_pClientLogic = NULL;

char g_Custom_KickMsg[256] = {0};

int tempCountInsertKeyPressedNum = 0;
int	tempCountDeleteKeyPressedNum = 0;
int	tempCountMinusKeyPressedNum = 0;

void ClientGameLogic::CreateInstance()
{
	VMPROTECT_BeginVirtualization("ClientGameLogic::CreateInstance");
	r3d_assert(g_pClientLogic == NULL);
	g_pClientLogic = game_new ClientGameLogic();
	VMPROTECT_End();
}

void ClientGameLogic::DeleteInstance()
{
	VMPROTECT_BeginVirtualization("ClientGameLogic::DeleteInstance");
	SAFE_DELETE(g_pClientLogic);
	VMPROTECT_End();
}

ClientGameLogic* ClientGameLogic::GetInstance()
{
	VMPROTECT_BeginMutation("ClientGameLogic::GetInstance");
	r3d_assert(g_pClientLogic);
	return g_pClientLogic;
	VMPROTECT_End();
}

obj_Player* ClientGameLogic::GetPlayer(int idx) const
{
	VMPROTECT_BeginMutation("ClientGameLogic::GetPlayer");
	r3d_assert(idx < MAX_NUM_PLAYERS);
	return players2_[idx].ptr;
	VMPROTECT_End();
}

void ClientGameLogic::SetPlayerPtr(int idx, obj_Player* ptr)
{
	VMPROTECT_BeginMutation("ClientGameLogic::SetPlayerPtr");
	r3d_assert(idx < MAX_NUM_PLAYERS);
	players2_[idx].ptr  = ptr;

	if(idx >= CurMaxPlayerIdx) 
		CurMaxPlayerIdx = idx + 1;

	VMPROTECT_End();
}

#if USE_VMPROTECT
  #pragma optimize("g", on)
#endif



static const int HOST_TIME_SYNC_SAMPLES	= 20;

static void preparePacket(const GameObject* from, DefaultPacket* packetData)
{
	r3d_assert(packetData);
	//r3d_assert(packetData->EventID >= 0);

	if(from) {
		r3d_assert(from->GetNetworkID());
		//r3d_assert(from->NetworkLocal);

		packetData->FromID = toP2pNetId(from->GetNetworkID());
	} else {
		packetData->FromID = 0; // world event
	}

	return;
}

bool g_bDisableP2PSendToHost = false;
void p2pSendToHost(const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
	extern bool g_bEditMode;
	if(g_bEditMode)
		return;
	if(g_bDisableP2PSendToHost)
		return;
	if(!gClientLogic().serverConnected_)
		return;
	
	preparePacket(from, packetData);
	gClientLogic().net_->SendToHost(packetData, packetSize, guaranteedAndOrdered);
}

ClientGameLogic::ClientGameLogic()
{
	isGameStarted = 0; //gamehardcore
	Reset();
}

void ClientGameLogic::Reset()
{
	net_lastFreeId    = NETID_OBJECTS_START;

	serverConnected_ = false;
	gameShuttedDown_ = false;
	gameDoNotAllowToPlaceObjects = false;
	
	disconnectStatus_ = 0;

	nextSecTimeReport_    = 0xFFFFFFFF;
	gppDataSeed_          = 0;
	d3dCheatSent2_		  = false;

	//m_highPingTimer		  = 0;
	
	m_gameCreatorCustomerID = 0;
	gameJoinAnswered_ = 0;
	gameStartAnswered_ = false;
	serverVersionStatus_ = 0;
	m_gameInfo = GBGameInfo();
	m_sessionId = 0;
	gameServerTimeOffset = 0;

	localPlayerIdx_   = -1;
	localPlayer_      = NULL;
	localPlayerConnectedTime = 0;

	CurMaxPlayerIdx = 0;
	for(int i=0; i<MAX_NUM_PLAYERS; i++) {
		SetPlayerPtr(i, NULL);
	}
	CurMaxPlayerIdx = 0; // reset it after setting to NULLs
	
	for(int i=0; i<R3D_ARRAYSIZE(playerNames); i++)
	{
		playerNames[i].Gamertag[0] = 0;
		playerNames[i].reputation = 0;
		playerNames[i].accountFlags = 0;
		playerNames[i].groupFlags = 0;
		playerNames[i].c4h_activeUntil = 0;
		playerNames[i].voiceClientID = 0;
		playerNames[i].voiceIsMuted  = false;
		playerNames[i].voiceEnabled  = true;
		playerNames[i].DevIsHide = false;
	}
	
	// clearing scoping.  Particularly important for Spectator modes. 
	g_RenderScopeEffect = 0;

	// clear saved screenshots from previous session
	SAFE_RELEASE(_r3d_screenshot_copy);
	ReleaseCheatScreenshot();
}

ClientGameLogic::~ClientGameLogic()
{
	g_net.Deinitialize();
}

void ClientGameLogic::OnNetPeerConnected(DWORD peerId)
{
#ifndef FINAL_BUILD
	r3dOutToLog("peer%02d connected\n", peerId);
#endif
	serverConnected_ = true;
	return;
}

void ClientGameLogic::OnNetPeerDisconnected(DWORD peerId)
{
	r3dOutToLog("***** disconnected from game server\n");
	serverConnected_ = false;
	return;
}

void ClientGameLogic::OnNetData(DWORD peerId, const r3dNetPacketHeader* packetData, int packetSize)
{
	R3DPROFILE_FUNCTION("ClientGameLogic::OnNetData");

#ifdef __WITH_PB__
	// Handle OOB PunkBuster packet
	if ( packetSize > 7 && memcmp ( packetData, "\xff\xff\xff\xffPB_", 7 ) == 0 )
	{
		PbClAddEvent ( PB_EV_PACKET, packetSize-4, (char*)packetData+4 ) ;
		return ;
	}
#endif


	r3d_assert(packetSize >= sizeof(DefaultPacket));
	const DefaultPacket* evt = static_cast<const DefaultPacket*>(packetData);

	GameObject* fromObj = NULL;
	if(evt->FromID != 0) 
	{
		fromObj = GameWorld().GetNetworkObject(evt->FromID);
	}

	//r3dOutToLog("OnNetData from peer%d, obj:%d(%s), event:%d\n", peerId, evt->FromID, fromObj ? fromObj->Name.c_str() : "", evt->EventID);

	// pass to world event processor first.
	if(ProcessWorldEvent(fromObj, evt->EventID, peerId, packetData, packetSize)) 
		return;

	if(evt->FromID && fromObj == NULL) 
	{
		r3dError("bad event %d sent from non registered object %d\n", evt->EventID, evt->FromID);
		return; 
	}

	if(fromObj) 
	{
		r3d_assert(!(fromObj->ObjFlags & OBJFLAG_JustCreated)); // just to make sure

		if(!fromObj->OnNetReceive(evt->EventID, packetData, packetSize)) 
			r3dError("bad event %d for %s", evt->EventID, fromObj->Class->Name.c_str());
		return;
	}

	r3dError("bad world event %d", evt->EventID);
	return;
}

r3dPoint3D ClientGameLogic::AdjustSpawnPositionToGround(const r3dPoint3D& pos)
{
	//
	// detect 'ground' under spawn position. 
	// because server now send exact position and it might be under geometry if it was changed
	//
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_PLAYER_COLLIDABLE_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
	if(!g_pPhysicsWorld->raycastSingle(PxVec3(pos.x, pos.y+1.0f, pos.z), PxVec3(0,-1,0), 1.2f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
		return pos + r3dPoint3D(0, 1.0f, 0);
		
	return r3dPoint3D(hit.impact.x, hit.impact.y + 0.1f, hit.impact.z);
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_C2S_ValidateConnectingPeer)
{
	serverVersionStatus_ = 1;
	if(n.protocolVersion != P2PNET_VERSION)
	{
		r3dOutToLog("Version mismatch our:%d, server:%d\n", P2PNET_VERSION, n.protocolVersion);
		serverVersionStatus_ = 2;
	}
		
	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_C2C_PacketBarrier)
{
	// sanity check: must be only for local networked objects
	if(fromObj && !fromObj->NetworkLocal) {
		r3dError("PKT_C2C_PacketBarrier for %s, %s, %d\n", fromObj->Name.c_str(), fromObj->Class->Name.c_str(), fromObj->GetNetworkID());
	}

	// reply back
	PKT_C2C_PacketBarrier_s n2;
	p2pSendToHost(fromObj, &n2, sizeof(n2));
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_JoinGameAns)
{
#ifndef FINAL_BUILD
	r3dOutToLog("PKT_S2C_JoinGameAns: %d %d\n", n.success, n.playerIdx);
#endif

	if(n.success != 1) {
		r3dOutToLog("Can't join to game server - session is full");
		gameJoinAnswered_ = 2;
		return;
	}

	gClientLogic().isGameStarted = n.gameInfo.isGameStarted;
	if (n.gameInfo.isGameStarted == 1) { //gamehardcore

		gameJoinAnswered_ = 10;
		return;
	}

	localPlayerIdx_   = n.playerIdx;
	gameJoinAnswered_ = 1;

	m_gameCreatorCustomerID = n.ownerCustomerID;
	m_gameInfo        = n.gameInfo;
	m_gameVoicePwd    = n.voicePwd;
	m_gameVoiceId     = n.voiceId;
	gameStartUtcTime_ = n.gameTime;
	gameStartTime_    = r3dGetTime();

	lastShadowCacheReset_ = -1;
	UpdateTimeOfDay();
	
	g_num_matches_played->SetInt(g_num_matches_played->GetInt()+1);
	void writeGameOptionsFile();
	writeGameOptionsFile();

	gUserSettings.addGameToRecent(n.gameInfo);
	gUserSettings.saveSettings();

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CustomKickMsg)
{
#ifndef FINAL_BUILD
	r3dOutToLog("PKT_S2C_CustomKickMsg: %s\n", n.msg);
#endif
	r3dscpy(g_Custom_KickMsg, n.msg);
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_Test_UpdateClientTime)
{
	gClientLogic().gameStartUtcTime_ -= gameServerTimeOffset;
	gameServerTimeOffset = n.gameTimeOffset;
	gClientLogic().gameStartUtcTime_ += gameServerTimeOffset;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_ShutdownNote)
{
	gameShuttedDown_ = true;
	
	char msg[128];
	if(n.reason == 2)
		sprintf(msg, gLangMngr.getString("ServerInstanceIsShuttingDown"), n.timeLeft);
	else
		sprintf(msg, gLangMngr.getString("ServerShuttingDown"), n.timeLeft);

	if(hudMain)
	{
		hudMain->showChat(true);
		hudMain->addChatMessage(1, "<system>", msg, 2);
		if(n.timeLeft < 180)
		{
			hudMain->addChatMessage(1, "<system>", gLangMngr.getString("ServerShutdownNoPlaceableObjects"), 2);
			gameDoNotAllowToPlaceObjects = true;
		}
	}

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_SetGamePlayParams)
{
	// replace our game parameters with ones from server.
	r3d_assert(GPP);
	*const_cast<CGamePlayParams*>(GPP) = n.GPP_Data;
	gppDataSeed_ = n.GPP_Seed;

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_SetGameInfoFlags)
{
	DWORD oldFlags = m_gameInfo.flags;
	m_gameInfo.flags = n.gameInfoFlags;
	
	// we can be in game loading stage atm, TODO: maybe store to new var and check inside hudMain?
	if(hudMain == NULL)
		return;

	if((oldFlags & GBGameInfo::SFLAGS_DisableASR) == 0 && (m_gameInfo.flags & GBGameInfo::SFLAGS_DisableASR))
	{
		hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("ServerRuleChange_ASRRestricted"), 0);
	}
	else if((oldFlags & GBGameInfo::SFLAGS_DisableASR) && (m_gameInfo.flags & GBGameInfo::SFLAGS_DisableASR)==0)
	{
		hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("ServerRuleChange_ASREnabled"), 0);
	}
	if((oldFlags & GBGameInfo::SFLAGS_DisableSNP) == 0 && (m_gameInfo.flags & GBGameInfo::SFLAGS_DisableSNP))
	{
		hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("ServerRuleChange_SNPRestricted"), 0);
	}
	else if((oldFlags & GBGameInfo::SFLAGS_DisableSNP) && (m_gameInfo.flags & GBGameInfo::SFLAGS_DisableSNP)==0)
	{
		hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("ServerRuleChange_SNPEnabled"), 0);
	}

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_StartGameAns)
{
#ifndef FINAL_BUILD
	r3dOutToLog("OnPKT_S2C_StartGameAns, %d\n", n.result);
#endif

	r3d_assert(gameStartAnswered_ == false);
	gameStartAnswered_ = true;
	gameStartResult_   = n.result;
	hudPause->setAllowItemDropTime(r3dGetTime()+5.0f); // do not allow to drop items for first 5 seconds after entering game
	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_C2C_ChatMessage)
{
	hudMain->showChat(true);
	hudMain->addChatMessage(n.msgChannel, n.gamertag, n.msg, n.userFlag);
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_UpdateWeaponData)
{
	WeaponConfig* wc = const_cast<WeaponConfig*>(g_pWeaponArmory->getWeaponConfig(n.itemId));
#ifdef FINAL_BUILD
	if(wc)
		wc->copyParametersFrom(n.wi);
	return;
#endif

	if(wc == NULL) {
		r3dOutToLog("!!! got update for not existing weapon %d\n", n.itemId);
		return;
	}

	wc->copyParametersFrom(n.wi);
	//r3dOutToLog("got update for weapon %s\n", wc->m_StoreName);

	static float lastMsgTime = 0;
	if(r3dGetTime() > lastMsgTime + 1.0f) {
		lastMsgTime = r3dGetTime();
		hudMain->showMessage(gLangMngr.getString("InfoMsg_WeaponDataUpdated"));
	}

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_UpdateGearData)
{
	GearConfig* gc = const_cast<GearConfig*>(g_pWeaponArmory->getGearConfig(n.itemId));
#ifdef FINAL_BUILD
	if(gc)
		gc->copyParametersFrom(n.gi);
	return;
#endif

	if(gc == NULL) {
		r3dOutToLog("!!! got update for not existing gear %d\n", n.itemId);
		return;
	}

	gc->copyParametersFrom(n.gi);
	//r3dOutToLog("got update for gear %s\n", gc->m_StoreName);

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_UpdateAttmData)
{
	WeaponAttachmentConfig* ac = const_cast<WeaponAttachmentConfig*>(g_pWeaponArmory->getAttachmentConfig(n.itemId));
#ifdef FINAL_BUILD
	if(ac)
		ac->copyParametersFrom(n.ai);
	return;
#endif

	if(ac == NULL) {
		r3dOutToLog("!!! got update for not existing attachment %d\n", n.itemId);
		return;
	}

	ac->copyParametersFrom(n.ai);
	//r3dOutToLog("got update for attm %s\n", ac->m_StoreName);

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_C2S_GetUAV)
{
	obj_UAV* uav = (obj_UAV*)GameWorld().GetNetworkObject(n.UAVID);
	if (!uav)
		return;

	if (gClientLogic().localPlayer_->UavNid_ == n.UAVID)
	{
		//gClientLogic().localPlayer_->ToggleUAVView(true);
		gClientLogic().localPlayer_->uavId_ = invalidGameObjectID;
		gClientLogic().localPlayer_->uavRequested_ = 0;
		gClientLogic().localPlayer_->uavViewActive_ = false;
	}

	uav->setActiveFlag(0);
	GameWorld().DeleteObject( uav );
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CreateNetObject)
{
	if(n.itemID == WeaponConfig::ITEMID_BarbWireBarricade ||
		n.itemID == WeaponConfig::ITEMID_WoodShieldBarricade ||
		n.itemID == WeaponConfig::ITEMID_WoodShieldBarricadeZB ||
		n.itemID == WeaponConfig::ITEMID_RiotShieldBarricade ||
		n.itemID == WeaponConfig::ITEMID_RiotShieldBarricadeZB ||
		n.itemID == WeaponConfig::ITEMID_SandbagBarricade ||
		n.itemID == WeaponConfig::ITEMID_WoodenDoorBlock ||
		n.itemID == WeaponConfig::ITEMID_MetalWallBlock ||
		n.itemID == WeaponConfig::ITEMID_TallBrickWallBlock ||
		n.itemID == WeaponConfig::ITEMID_WoodenWallPiece ||
		n.itemID == WeaponConfig::ITEMID_ShortBrickWallPiece ||
		n.itemID == WeaponConfig::ITEMID_PlaceableLight ||
		n.itemID == WeaponConfig::ITEMID_SmallPowerGenerator ||
		n.itemID == WeaponConfig::ITEMID_ConstructorBlockSmall ||
		n.itemID == WeaponConfig::ITEMID_ConstructorBlockBig ||
		n.itemID == WeaponConfig::ITEMID_ConstructorBlockCircle ||
		n.itemID == WeaponConfig::ITEMID_ConstructorColum1 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorColum2 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorColum3 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorFloor1 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorFloor2 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorCeiling1 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorCeiling2 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorCeiling3 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorWallMetalic ||
		n.itemID == WeaponConfig::ITEMID_ConstructorSlope ||
		n.itemID == WeaponConfig::ITEMID_ConstructorWall1 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorWall2 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorWall3 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorWall4 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorWall5 ||
		n.itemID == WeaponConfig::ITEMID_ConstructorBaseBunker ||
		n.itemID == WeaponConfig::ITEMID_BigPowerGenerator)
	{
		obj_Barricade* barricade= (obj_Barricade*)srv_CreateGameObject("obj_Barricade", "barricade", n.pos);
		barricade->m_ItemID	= n.itemID;
		barricade->SetNetworkID(n.spawnID);
		barricade->m_RotX		= n.var1;
		barricade->SetRotationVector(r3dPoint3D(n.var1, 0, 0));
		barricade->OnCreate();
	}
	else if(n.itemID == WeaponConfig::ITEMID_Traps_Bear ||
			n.itemID == WeaponConfig::ITEMID_Traps_Spikes ||
			n.itemID == WeaponConfig::ITEMID_Campfire)
	{
		obj_Traps* Traps= (obj_Traps*)srv_CreateGameObject("obj_Traps", "barricade", n.pos);
		Traps->m_ItemID	= n.itemID;
		Traps->SetNetworkID(n.spawnID);
		Traps->m_RotX		= n.var1;
		Traps->SetRotationVector(r3dPoint3D(n.var1, 0, 0));
		Traps->OnCreate();
		if (n.var3>0.0)
			Traps->Activate(false);
	}
	else if(n.itemID == WeaponConfig::ITEMID_Cypher2)
	{
		GameObject* User = GameWorld().GetNetworkObject(n.userID);
		obj_Player* plr = (obj_Player*)User;

		if (!plr)
			return;

		r3d_assert(plr->uavId_ == invalidGameObjectID);
		//r3dOutToLog("PKT_S2C_UAVSpawnAns for %s\n", plr->UserName); CLOG_INDENT;

		// spawn uav with manual OnCreate call
		char name[128];
		sprintf(name, "uav_%p", plr);
		obj_UAV* uav = (obj_UAV*)srv_CreateGameObject("obj_UAV", name, n.pos);
		uav->ownerID      = plr->GetSafeID();
		uav->NetworkLocal = plr->NetworkLocal;
		uav->SetNetworkID(n.spawnID);
		uav->SetRotationVector(r3dPoint3D(n.var1, 0, 0));
		uav->OnCreate();

		// start damage particle
		if((float)n.var5 >= 1) uav->SetDamagedState();

		// set base cell for movement data (must do it AFTER OnCreate())
		if(!uav->NetworkLocal) uav->netMover.SetStartCell(r3dPoint3D(n.var2, n.var3, (float)n.var4));

		// set that player have it
		plr->UavNid_ = uav->GetNetworkID();
		plr->uavId_ = uav->GetSafeID();

		hudMain->showMessage(gLangMngr.getString("$HUD_Msg_UAVSpawned"));
	}
	else if(n.itemID == WeaponConfig::ITEMID_FarmBlock || n.itemID == WeaponConfig::ITEMID_SolarWaterPurifier || n.itemID == WeaponConfig::ITEMID_GardenTrap_Rabbit)
	{
		obj_FarmBlock* block = (obj_FarmBlock*)srv_CreateGameObject("obj_FarmBlock", "farmblock", n.pos);
		block->m_ItemID	= n.itemID;
		block->SetNetworkID(n.spawnID);
		block->m_RotX		= n.var1;
		block->m_TimeUntilRipe = n.var2;
		block->SetRotationVector(r3dPoint3D(n.var1, 0, 0));
		block->OnCreate();
		if (n.var3>0.0 && n.itemID == WeaponConfig::ITEMID_GardenTrap_Rabbit)
			block->Activate(false);
	}
	else if(n.itemID == WeaponConfig::ITEMID_Lockbox)
	{
		obj_Safelock* safelock= (obj_Safelock*)srv_CreateGameObject("obj_Safelock", "safelock", n.pos);
		safelock->m_ItemID	= n.itemID;
		safelock->SetNetworkID(n.spawnID);
		safelock->m_RotX		= n.var1;
		safelock->m_OwnerCustomerID = n.var4;
		safelock->SetRotationVector(r3dPoint3D(n.var1, 0, 0));
		safelock->OnCreate();
	}
	else if(n.itemID == WeaponConfig::ITEMID_FlareGun ||	// used to get the flare launched by the flare-gun
			n.itemID == WeaponConfig::ITEMID_FragGrenade ||
			n.itemID == WeaponConfig::ITEMID_SharpnelBomb ||
			n.itemID == WeaponConfig::ITEMID_SharpnelTripWireBomb ||
			n.itemID == WeaponConfig::ITEMID_ChemLight ||
			n.itemID == WeaponConfig::ITEMID_Flare ||
			n.itemID == WeaponConfig::ITEMID_ChemLightBlue ||
			n.itemID == WeaponConfig::ITEMID_ChemLightGreen ||
			n.itemID == WeaponConfig::ITEMID_ChemLightOrange ||
			n.itemID == WeaponConfig::ITEMID_ChemLightRed ||
			n.itemID == WeaponConfig::ITEMID_ChemLightYellow)
	{
		const WeaponConfig* pWeaponConfig = g_pWeaponArmory->getWeaponConfig(n.itemID);
		
		obj_Grenade* grenade = NULL;
		grenade = (obj_Grenade*)GameWorld().GetNetworkObject(n.spawnID);
		if(!grenade)
		{
			if(n.var4 == 0)
			{
				// The owning player left the game, and there's no use searching for a
				// grenade thrown by them.  Create a new one and move on.
				grenade = (obj_Grenade*)srv_CreateGameObject(pWeaponConfig->m_PrimaryAmmo->getBulletClass(), "grenade", n.pos);
			}
			else
			{
				GameObject* pGameObj = GameWorld().GetObject(gobjid_t(n.var5));
				// (IT'S NOT SAFE TO ASSUME THE OBJECT RETURNED IS THE GRENADE, BECAUSE A
				// COLLISION MAY EXIST ON THE ID FOR OTHER CLIENTS, so we verify by making
				// sure the owner matches)
				GameObject* pobj = GameWorld().GetNetworkObject((gp2pnetid_t)n.var4);
				if(pobj && pobj->isObjType(OBJTYPE_Human) && pGameObj && pGameObj->ownerID == pobj->GetSafeID())
					grenade = (obj_Grenade*)pGameObj;
				else
					grenade = (obj_Grenade*)srv_CreateGameObject(pWeaponConfig->m_PrimaryAmmo->getBulletClass(), "grenade", n.pos);
			}
			grenade->SetNetworkID(n.spawnID);
		}
		grenade->m_ItemID = n.itemID;
		grenade->m_NetMover.SetStartCell(n.pos);
		grenade->SetPosition(n.pos);
		grenade->m_Weapon = pWeaponConfig;
		if((grenade->ObjFlags & OBJFLAG_JustCreated) > 0)
		{
			BOOL bCreated = grenade->OnCreate();
			r3d_assert(bCreated);
			r3d_assert((grenade->ObjFlags & OBJFLAG_JustCreated) == 0);
		}
	}


	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_DestroyNetObject)
{
	GameObject* obj = GameWorld().GetNetworkObject(n.spawnID);
	//r3d_assert(obj);
	if (!obj)
		return;

	if(obj->isObjType(OBJTYPE_Human))
	{
		int playerIdx = obj->GetNetworkID() - NETID_PLAYERS_START;
		SetPlayerPtr(playerIdx, NULL);
	}

	if(obj->isObjType(OBJTYPE_UAV))
	{
		obj_UAV* uav = (obj_UAV*)obj;
		if (uav)
		{
			if (SoundSys.isPlaying(uav->UavSnd))
			{
				SoundSys.Stop(uav->UavSnd);
			}
		}
	}

	obj->setActiveFlag(0);

	//@TODO: do something when local player was dropped
	if(obj == localPlayer_)
	{
		r3dOutToLog("local player dropped by server\n");
	}

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_AirDropOnMap)
{
	if (gClientLogic().localPlayer_)
	{
		gClientLogic().localPlayer_->m_NumbeAirDrop = n.NumbeAirDrop;
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CreateDroppedItem)
{
	//r3dOutToLog("obj_DroppedItem %d %d\n", n.spawnID, n.Item.itemID);
	r3d_assert(GameWorld().GetNetworkObject(n.spawnID) == NULL);

	obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", n.pos);
	obj->SetNetworkID(n.spawnID);
	if ( n.Item.itemID == 'FLPS')
		obj->DrawDistanceSq = FLT_MAX;

	if ( n.Item.itemID == 'ARDR')
	{
		obj->DrawDistanceSq = FLT_MAX;
		obj->AirDropPos = n.pos;
		obj->SetPosition(obj->AirDropPos);
		obj->NetworLocal = true;
	}
	obj->m_FirstTime = n.FirstTime;
	obj->m_Item    = n.Item;
	obj->OnCreate();
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CreateNote)
{
	r3dOutToLog("obj_Note %d\n", n.spawnID);
	r3d_assert(GameWorld().GetNetworkObject(n.spawnID) == NULL);
	
	obj_Note* obj = (obj_Note*)srv_CreateGameObject("obj_Note", "obj_Note", n.pos);
	obj->SetNetworkID(n.spawnID);
	obj->OnCreate();
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_SetNoteData)
{
	obj_Note* obj = (obj_Note*)fromObj;
	r3d_assert(obj);
	
	obj->m_GotData = true;
	obj->m_TextFrom = n.TextFrom;
	obj->m_Text = n.TextSubj;

	hudMain->showReadNote(obj->m_Text.c_str());
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_UseNetObjectAns)
{
	if(hudMain)
	{
		switch(n.ans)
		{
		case PKT_S2C_UseNetObjectAns_s::Ans_HarvestedFarm:
			hudMain->showMessage(gLangMngr.getString("HarvestedFarm"));
			break;
		case PKT_S2C_UseNetObjectAns_s::Ans_HarvestNotReady:
			hudMain->showMessage(gLangMngr.getString("HarvestNotReadyFarm"));
			break;
		default:
			break;
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_SetupTraps)
{
	GameObject* obj = GameWorld().GetNetworkObject(n.spawnID);
	if (!obj)
		return;

	if (obj->Class->Name == "obj_Traps")
	{
		obj_Traps* Traps = (obj_Traps*)obj;
		if (n.m_Activate > 0)
			Traps->Activate(false);
		else
			Traps->Deactivate();
	}
	else {
		obj_FarmBlock* Farm = (obj_FarmBlock*)obj;
		if (n.m_Activate > 0)
			Farm->Activate(false);
		else
			Farm->Deactivate();
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CreateGravestone)
{
	r3dOutToLog("obj_Gravestone %d\n", n.spawnID);
	r3d_assert(GameWorld().GetNetworkObject(n.spawnID) == NULL);
	
	obj_Gravestone* obj	= (obj_Gravestone*)srv_CreateGameObject("obj_Gravestone", "obj_Gravestone", n.pos, 0, (void*)&n.GravestoneInfo);
	obj->SetNetworkID(n.spawnID);
	obj->m_Aggressor		= n.Aggressor;
	obj->m_Victim			= n.Victim;
	obj->m_GotData			= true;
	obj->OnCreate();
}

#ifdef MISSIONS
#ifdef MISSION_TRIGGERS
IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CreateMissionTrigger)
{
	r3dOutToLog("obj_MissionTrigger %d\n", n.spawnID);
	r3d_assert(GameWorld().GetNetworkObject(n.spawnID) == NULL);
	
	obj_MissionTrigger* obj = (obj_MissionTrigger*)srv_CreateGameObject("obj_MissionTrigger", "obj_MissionTrigger", n.pos);
	obj->SetNetworkID(n.spawnID);
	obj->m_missionID = n.missionID;
	r3dscpy( obj->m_missionTitleID, n.missionTitleStringID );
	r3dscpy( obj->m_missionNameID, n.missionNameStringID );
	r3dscpy( obj->m_missionDescID, n.missionDescStringID );
	obj->OnCreate();
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_ShowMissionTrigger)
{
	obj_MissionTrigger* obj = (obj_MissionTrigger*)fromObj;
	r3d_assert(obj);
	
	hudMain->showMission(obj->m_missionID, gLangMngr.getString(obj->m_missionNameID), gLangMngr.getString(obj->m_missionDescID));
}
#endif

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_MissionActivate)
{
	int index = hudPause->FindMissionButtonIndex( n.missionID );
	if( index == -1 )
	{
		if( !(n.data & 0x1) )
			return;

		index = hudPause->FindFirstAvailableMissionButtonIndex();
		if( index == -1 )
		{
			r3dOutToLog("No Mission Buttons available to list mission '%s'\n", gLangMngr.getString(n.missionNameStringID));
			return;
		}
	}
	HUDPause::HUDMissionButton button;
	button.m_missionID		= n.missionID;
	button.m_accepted		= (n.data & 0x2) > 0;
	button.m_declined		= false;
	button.m_canRepeat		= (n.data & 0x4) > 0;
	r3dscpy( button.m_missionNameStringID, n.missionNameStringID );
	r3dscpy( button.m_missionDescStringID, n.missionDescStringID );
	r3dscpy( button.m_missionIconPath, n.missionIconPath );
	hudPause->SetMissionButton( index, (n.data & 0x01) > 0, button );
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_MissionComplete)
{
	if( localPlayer_ )
	{
		int index = hudPause->FindMissionButtonIndex( n.missionID );
		if( index == -1 )
			return;

#ifndef FINAL_BUILD
		if( d_enable_mission_repeat->GetBool() )
		{
			//r3dOutToLog("!!! Mission(%d): Enabling Repeat of Mission Button(%d): %s!\n", n.missionID, index, gLangMngr.getString(hudPause->missionButtons[index].m_missionNameStringID));
			HUDPause::HUDMissionButton button;
			hudPause->CopyMissionButton( index, button );
			button.m_accepted = false;
			button.m_declined = false;
			hudPause->SetMissionButton( index, true, button );
		}
		else
#endif
		{
			hudPause->removeMissionButton( n.missionID, 1 /*Mission::RMV_MissionCompleted*/ );
		}
	
		// RemoveMissionActionData is handled by the PKT_S2C_MissionRemove that follows.
		//localPlayer_->RemoveMissionActionData( n.missionID, 1 /*Mission::RMV_MissionCompleted*/ );
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_MissionRemove)
{
	if( localPlayer_ )
	{
		hudPause->setMissionButton( n.missionID, false, false );

		localPlayer_->RemoveMissionActionData( n.missionID, n.reason );
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_MissionMapUpdate)
{
	HUDPause::HUDMissionIcon icon;
	icon.m_mapIcon	= n.actionIcon;
	icon.m_location	= n.location;
	icon.m_active	= n.active;
	hudPause->SetMissionIcon( n.actionID, icon );
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_AirDropMapUpdate)
{
	if (r3dGetTime() > n.m_time)
		return;

	HUDPause::HUDAirDropIcon AirDrop;
	AirDrop.m_time	    = n.m_time;
	AirDrop.m_location	= n.location;
	hudPause->SetAirDropIcon( AirDrop );
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_MissionActionUpdate)
{
	if( localPlayer_ )
	{
		//r3dOutToLog("Updating Mission Action: Mission(%d), Action(%d)\n", n.missionID, n.actionID);
		localPlayer_->UpdateMissionActionData( n );
	}
}
#endif

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CreateZombie)
{
	//r3dOutToLog("obj_Zombie %d\n", n.spawnID);
	r3d_assert(GameWorld().GetNetworkObject(n.spawnID) == NULL);
	
	obj_Zombie* obj = (obj_Zombie*)srv_CreateGameObject("obj_Zombie", "obj_Zombie", n.spawnPos);
	obj->SetNetworkID(n.spawnID);
	obj->NetworkLocal = false;
	memcpy(&obj->CreateParams, &n, sizeof(n));
	obj->OnCreate();

	// set base cell for movement data (must do it AFTER OnCreate())
	obj->netMover.SetStartCell(n.moveCell);
}

#ifdef VEHICLES_ENABLED

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_VehicleSpawn)
{
	r3d_assert(GameWorld().GetNetworkObject(n.spawnID) == NULL);

	obj_Vehicle* vehicle;

	if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_BUGGY)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\Drivable_buggy_02.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_STRYKER)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\Drivable_Stryker.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_ZOMBIEKILLER)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\Zombie_killer_car.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_HUMMER)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\driveable_hummer.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\driveable_Police.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_ABANDONEDSUV)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_abandoned_suv.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_BONECRUSHER)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_bonecrusher.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_COPCAR)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_cop_car.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_DUNEBUGGY)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_dune_buggy.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_ECONOLINE)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_econo_line.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_LARGETRUCK)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_large_truck.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_MILITARYAPC)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_military_apc.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_PARAMEDIC)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_paramedic_car.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_SUV)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\G3_Vehicles\\drivable_suv.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\drive_bell212.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_JEEP)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\driveable_jeep.sco", n.position);
	else if ((obj_Vehicle::VehicleTypes)n.vehicleType == obj_Vehicle::VEHICLETYPE_TANK_T80)
		vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", "Data\\ObjectsDepot\\Vehicles\\drivable_t80.sco", n.position);

	vehicle->SetNetworkID(n.spawnID);
	vehicle->NetworkLocal = false;
	memcpy(&vehicle->CreateParams, &n, sizeof(n));
	vehicle->OnCreate();
	vehicle->camangle2 = n.camangle + vehicle->GetRotationVector().x;
	vehicle->cameraUpDown = n.cameraUpDown + vehicle->GetRotationVector().y;
	vehicle->camangleBackup  = n.camangle;
	vehicle->camangle2Backup = n.cameraUpDown;

	r3d_assert(vehicle->vehicleId != 0);
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_VehicleEntered)
{	
	R3DPROFILE_FUNCTION("ClientGameLogic::OnPKT_S2C_VehicleEntered");

	if (!fromObj)
		return;

	if (n.isSuccess)
	{
		GameObject* vehicleObj = GameWorld().GetNetworkObject(n.networkId);
		if(!vehicleObj)
			return;

		r3d_assert(vehicleObj->isObjType(OBJTYPE_Vehicle));

		obj_Vehicle* vehicle = (obj_Vehicle*)vehicleObj;
		vehicle->SetFuel(n.curFuel);

		if (localPlayer_ && fromObj->GetNetworkID() == toP2pNetId(localPlayer_->GetNetworkID()))
		{		
			localPlayer_->enterVehicle(vehicle, (int)n.seat);
		}
		else
		{	
			GameObject* remotePlayerObj = GameWorld().GetNetworkObject(fromObj->GetNetworkID());
			if(!remotePlayerObj)
				return;

			if(remotePlayerObj->isObjType(OBJTYPE_Human))
			{
				obj_Player* remotePlayer = (obj_Player*)remotePlayerObj;
				remotePlayer->enterVehicle(vehicle, (int)n.seat);
			}
		}
	}
	else
	{		
		if (localPlayer_)
		{
			localPlayer_->hasOpenVehicleRequest = false;
			hudMain->showMessage(gLangMngr.getString("VehicleCantEnter"));
		}
	}

	return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_VehicleExited)
{
	R3DPROFILE_FUNCTION("ClientGameLogic::OnPKT_S2C_VehicleExited");

	if (!fromObj)
		return;

	GameObject* vehicleObj = GameWorld().GetNetworkObject(n.vehicleId);
	if(!vehicleObj)
		return;

	r3d_assert(vehicleObj->isObjType(OBJTYPE_Vehicle));
	obj_Vehicle* targetVehicle = (obj_Vehicle*)vehicleObj;

	if (n.isSuccess)
	{
		if (localPlayer_ && fromObj->GetNetworkID() == toP2pNetId(localPlayer_->GetNetworkID()))
		{	
			localPlayer_->exitVehicle(targetVehicle, n.exitPosition);
		}
		else
		{	
			GameObject* remotePlayerObj = GameWorld().GetNetworkObject(fromObj->GetNetworkID());
			if(remotePlayerObj && remotePlayerObj->isObjType(OBJTYPE_Human))
			{
				obj_Player* remotePlayer = (obj_Player*)remotePlayerObj;
				remotePlayer->exitVehicle(targetVehicle, n.exitPosition);
			}
		}
	}
	else
	{
		if (localPlayer_ && fromObj->GetNetworkID() == toP2pNetId(localPlayer_->GetNetworkID()))
		{	
			if (n.isDisconnecting)
				targetVehicle->RemovePlayerBySeat(localPlayer_->seatPosition);

			localPlayer_->hasOpenVehicleRequest = false;
			hudMain->showMessage(gLangMngr.getString("VehicleCantExit"));		
		}
		else
		{
			if (n.isDisconnecting)
				targetVehicle->RemovePlayerBySeat((int)n.seat);
		}
	}
}
IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_VehicleHitTargetFail)
{
	GameObject* gameObj = GameWorld().GetNetworkObject(n.targetId);
	if(!gameObj)
		return;

	if (gameObj->isObjType(OBJTYPE_Zombie))
	{
		obj_Zombie* zombie = (obj_Zombie*)gameObj;
		zombie->bDead = false;
		zombie->PhysicsEnable(true);
		zombie->SetZombieState(EZombieStates::ZState_Idle, true);
	}
	else if (gameObj->isObjType(OBJTYPE_Human))
	{
		obj_Player* player = (obj_Player*)gameObj;
		player->PhysicsEnable(true);
		player->DoRagdoll(false);
	}
}
#endif

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_PlayerNameJoined)
{
	r3d_assert(n.playerIdx < R3D_ARRAYSIZE(playerNames));
	r3dscpy(playerNames[n.playerIdx].Gamertag, n.gamertag);
	playerNames[n.playerIdx].reputation = n.reputation;
	playerNames[n.playerIdx].accountFlags = n.flags;
	playerNames[n.playerIdx].clanID = n.ClanID;
	playerNames[n.playerIdx].groupFlags = 0;
	playerNames[n.playerIdx].c4h_activeUntil = 0;
	playerNames[n.playerIdx].DevIsHide = n.m_DevPlayerHide==1?true:false;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_PlayerNameLeft)
{
	r3d_assert(n.playerIdx < R3D_ARRAYSIZE(playerNames));
	playerNames[n.playerIdx].Gamertag[0] = 0;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_PlayerStatusUpdate)
{
	r3d_assert(n.playerIdx < R3D_ARRAYSIZE(playerNames));
	playerNames[n.playerIdx].lastPosX = n.posX;
	playerNames[n.playerIdx].lastPosZ = n.posZ;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CallForHelpAns)
{
	if(hudPause && hudPause->isActive())
	{
		if(n.ansCode == PKT_S2C_CallForHelpAns_s::CFH_Error)
		{
			Scaleform::GFx::Value var[3];
			var[0].SetString(gLangMngr.getString("HUD_CallForHelp_Error"));
			var[1].SetBoolean(true);
			var[2].SetString("");
			hudPause->gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
		}
		else if(n.ansCode == PKT_S2C_CallForHelpAns_s::CFH_Duplicate)
		{
			Scaleform::GFx::Value var[3];
			var[0].SetString(gLangMngr.getString("HUD_CallForHelp_Duplicate"));
			var[1].SetBoolean(true);
			var[2].SetString("");
			hudPause->gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
		}
		else if(n.ansCode == PKT_S2C_CallForHelpAns_s::CFH_Time)
		{
			Scaleform::GFx::Value var[3];
			var[0].SetString(gLangMngr.getString("HUD_CallForHelp_Time"));
			var[1].SetBoolean(true);
			var[2].SetString("");
			hudPause->gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_DropItemYPosition)
{
	GameObject* targetObj = GameWorld().GetNetworkObject(n.spawnID);
	if(!targetObj)
		return;
	
	obj_DroppedItem* AirDrop = (obj_DroppedItem*)targetObj;
	AirDrop->ServerPost(n.YPos);
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_SendDataDoor)
{
	GameObject* targetObj = GameWorld().GetNetworkObject(n.DoorID);
	//r3dOutToLog("###### entra 1 DoorID %i, OpenDoor %i, IsDestroy %i, ExcuteParticle %i\n",n.DoorID,n.m_OpenDoor,n.DoorIsDestroy,n.ExecuteParticle);
	if(!targetObj)
		return;
	obj_Door* obj = (obj_Door*)targetObj;
	obj->m_OpenDoor = n.m_OpenDoor==1?true:false;
	obj->DoorIsDestroy = n.DoorIsDestroy;
	if (obj->m_OpenDoor == true)
		obj->OpenOrClose=80.0f;
	else
		obj->OpenOrClose=10.0f;
	obj->SetRotationVector(obj->GetRotationVector()+r3dVector(obj->OpenOrClose,0,0));
	if (n.DoorIsDestroy == 1)
	{
		obj->RemovePhyxDoor=true;
		if (n.ExecuteParticle == 1)
			obj->ExeParticle();
	}
	else if (n.DoorIsDestroy == 0)
	{
		obj->EnablePhyxDoor=true;
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_LockboxOpReq)
{
	if(hudMain && hudSafelock)
	{
		if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_AskForCode)
			hudMain->showSafelockPin(true, n.lockboxID);
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_SetNewCode)
			hudMain->showSafelockPin(false, n.lockboxID);
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_WrongCode)
			hudMain->showMessage(gLangMngr.getString("HUD_Safelock_WrongCode"));
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_CodeChanged)
			hudMain->showMessage(gLangMngr.getString("HUD_Safelock_CodeChanged"));
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_SecurityLockdown)
			hudMain->showMessage(gLangMngr.getString("HUD_Safelock_SecurityLockdown"));
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_StartingToSendContent)
		{
			hudSafelock->NumItems = 0;
			hudSafelock->CurrentLockboxID = n.lockboxID;
		}
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_DoneSendingContent)
		{
			if(hudSafelock->isActive())
				hudSafelock->reloadLockboxInfo();
			else
				hudSafelock->Activate();
		}
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_NotOwner)
		{
			hudMain->showMessage(gLangMngr.getString("HUD_Safelock_NotOwner"));	
		}
		else if(n.op == PKT_S2C_LockboxOpReq_s::LBOR_Close)
		{
			if(hudSafelock->isUILocked())
				hudSafelock->unlockUI();
			hudSafelock->Deactivate();
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_LockboxContent)
{
	if(hudSafelock)
	{
		if(hudSafelock->NumItems < HUDSafelock::MAX_LOCKBOX_SIZE)
		{
			hudSafelock->LockboxContent[hudSafelock->NumItems++] = n.item;
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_C2C_DevHidePlayer)
{
	GameObject* targetObj = GameWorld().GetNetworkObject(n.PlayerID);
	if(!targetObj)
		return;

	obj_Player* targetPlr = (obj_Player*)targetObj;
	if (!targetPlr)
		return;

	targetPlr->m_DevPlayerHide = n.m_DevPlayerHide==1?true:false;
	playerNames[n.peerID].DevIsHide = n.m_DevPlayerHide==1?true:false;

	if (targetPlr->m_DevPlayerHide == true)
		targetPlr->TogglePhysicsSimulation(false);
	else
		targetPlr->TogglePhysicsSimulation(true);

}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_C2S_ReviveFast)// Respawnfast
{
	GameObject* targetObj = GameWorld().GetNetworkObject(n.PlayerID);
	if(!targetObj)
		return;

	obj_Player* targetPlr = (obj_Player*)targetObj;
	if (!targetPlr)
		return;

	if (n.SpawnSelect == 99 && n.PlayerID == gClientLogic().localPlayer_->GetNetworkID())
	{
		if (hudMain) hudMain->showMessage("$HUD_NoSafeZoneInMap");
		return;
	}
	if (n.SpawnSelect == 98 && n.PlayerID == gClientLogic().localPlayer_->GetNetworkID())
	{
		if (hudMain) hudMain->showMessage("$HUD_NotInGrouop");
		return;
	}

	if (n.PlayerID == gClientLogic().localPlayer_->GetNetworkID())
	{
		if (hudMain) hudMain->ShowMsgDeath("out");
	}

	targetPlr->ResPawnFast();
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CallForHelpEvent)
{
	r3d_assert(n.playerIdx < R3D_ARRAYSIZE(playerNames));
	if(hudMain)
	{
		r3dscpy(playerNames[n.playerIdx].c4h_distress, hudMain->applyProfanityFilter(n.distress));
		r3dscpy(playerNames[n.playerIdx].c4h_reward, hudMain->applyProfanityFilter(n.reward));
		playerNames[n.playerIdx].c4h_activeUntil = r3dGetTime() + n.timeLeft;
		playerNames[n.playerIdx].c4h_locX = n.locX;
		playerNames[n.playerIdx].c4h_locZ = n.locZ;

		hudMain->showMessage(gLangMngr.getString("HUD_CallForHelp_HUDInfoMsg"));
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CreatePlayer)
{
	R3DPROFILE_FUNCTION("ClientGameLogic::OnPKT_S2C_CreatePlayer");
	//r3dOutToLog("PKT_S2C_CreatePlayer: %d at %f %f %f\n", n.playerIdx, n.spawnPos.x, n.spawnPos.y, n.spawnPos.z);
#ifndef FINAL_BUILD
	r3dOutToLog("Create player: %s\n", n.gamertag);
#endif

	r3dPoint3D spawnPos = n.spawnPos;
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
	bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(n.spawnPos.x, n.spawnPos.y + 0.5f, n.spawnPos.z), PxVec3(0, -1, 0), 500.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
	r3dPoint3D posForWater = n.spawnPos;
	if( hitResult )
		posForWater = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);	// This is the ground position underwater.

	float waterDepth = getWaterDepthAtPos(posForWater);
	const float allowedDepth = 1.5f;
	if(waterDepth < allowedDepth) // not swimming, adjust to ground
		spawnPos = AdjustSpawnPositionToGround(n.spawnPos);

	wiCharDataFull slot;
	
	if(n.playerIdx == localPlayerIdx_)
	{
		// server sync point
		bool wipeInventory=false;
		bool giveLoadout=false;
#ifdef DISABLE_GI_ACCESS_ON_PTE_MAP
		if(m_gameInfo.channel == 6) // PTE map, wipe inventory and give one flashlight only
			wipeInventory=true;
#endif
#ifdef DISABLE_GI_ACCESS_ON_PTE_STRONGHOLD_MAP
		if(m_gameInfo.channel == 6 && m_gameInfo.mapId==GBGameInfo::MAPID_WZ_Cliffside) // PTE map, wipe inventory and give one flashlight only
			wipeInventory=true;
#endif
#ifdef DISABLE_GI_ACCESS_FOR_MMZ_SERVER
		if(m_gameInfo.gameServerId==134216 || m_gameInfo.gameServerId==148646) // MMZ
			wipeInventory=true;
#endif
#ifdef DISABLE_GI_ACCESS_FOR_DEV_EVENT_SERVER
		if(gClientLogic().m_gameInfo.gameServerId==148353 || gClientLogic().m_gameInfo.gameServerId==150340 || gClientLogic().m_gameInfo.gameServerId==150341|| gClientLogic().m_gameInfo.gameServerId==151732 || gClientLogic().m_gameInfo.gameServerId==151733 || gClientLogic().m_gameInfo.gameServerId==151734 || gClientLogic().m_gameInfo.gameServerId==151736
			// for testing on dev server
			//|| m_gameInfo.gameServerId==11
			) 
		{
			wipeInventory=true;
			giveLoadout=true;
		}
#endif
#ifdef DISABLE_GI_ACCESS_FOR_CALI_SERVER
		if(m_gameInfo.mapId==GBGameInfo::MAPID_WZ_California)
			wipeInventory=true;
#endif

		if(wipeInventory)
		{
			for(int i=0; i<wiCharDataFull::CHAR_MAX_BACKPACK_SIZE; ++i)
				gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Items[i].Reset();

			if (giveLoadout)
			{
				GiveDevEventLoadout();
			}
			else
			{
				wiInventoryItem flashLightItem;
				flashLightItem.InventoryID = 0;
				flashLightItem.itemID = 101306;
				flashLightItem.quantity = 1;
				gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Items[wiCharDataFull::CHAR_LOADOUT_WEAPON2] = flashLightItem;
			}
		}

		// med system sync begin
		// when creating player, erase med values from profile, then server will send setVitals to player and we will enable proper med system icons
		// do not want to duplicate code here and there
		gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].MedBleeding = 0.0f;
		gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].MedFeverCold = 0.0f;
		gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].MedBloodInfection = 0.0f;
		// med system sync end

		slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];

		if(hudMain && n.ClanID != 0)
			hudMain->enableClanChannel();

		if(hudMain)
		{
			char tmpStr[512]={0};
			switch(m_gameInfo.channel)
			{
			case 1:
				r3dscpy(tmpStr, (gLangMngr.getString("JoinedTrialServer"))); 
				break;
			case 2:
				r3dscpy(tmpStr, (gLangMngr.getString("JoinedOfficialServer")));
				break;
			case 3:
				r3dscpy(tmpStr, (gLangMngr.getString("JoinedPrivateServer"))); 
				break;
			case 4:
				r3dscpy(tmpStr, (gLangMngr.getString("JoinedPremiumServer"))); 
				break;
			case 5:
				r3dscpy(tmpStr, (gLangMngr.getString("JoinedStrongholdServer")));
				break;
			case 6:
				r3dscpy(tmpStr, (gLangMngr.getString("JoinedPublicTestEnvServer")));
				break;
			case 7:
				r3dscpy(tmpStr, (gLangMngr.getString("JoinedVeteranServer"))); 
				break;
			default:
				r3dscpy(tmpStr, "unknown channel!!!");
			}
			hudMain->addChatMessage(0, "<SYSTEM>", tmpStr, 0);

			if(m_gameInfo.flags & GBGameInfo::SFLAGS_DisableASR)
			{
				hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("JoinedServerWithASRRestriction"), 0);
			}
			if(m_gameInfo.flags & GBGameInfo::SFLAGS_DisableSNP)
			{
				hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("JoinedServerWithSNPRestriction"), 0);
			}
			if(m_gameInfo.flags & GBGameInfo::SFLAGS_OnlyFPS)
			{
				hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("OnlyFPSMode"), 0);
			}
#ifdef DISABLE_GI_ACCESS_ON_PTE_STRONGHOLD_MAP
			if(m_gameInfo.channel == 6 && m_gameInfo.mapId==GBGameInfo::MAPID_WZ_Cliffside) // PTE map, wipe inventory and give one flashlight only
				hudMain->addChatMessage(0, "<SYSTEM>", gLangMngr.getString("JoinedPTEServerWithGIRestriciton"), 0);
#endif
		}

		// make sure that our loadout is same as server reported. if not - disconnect and try again
		if(slot.Items[wiCharDataFull::CHAR_LOADOUT_WEAPON1 ].itemID != n.WeaponID0 ||
		   slot.Items[wiCharDataFull::CHAR_LOADOUT_WEAPON2 ].itemID != n.WeaponID1 ||
		   slot.Items[wiCharDataFull::CHAR_LOADOUT_ARMOR   ].itemID != n.ArmorID ||
		   slot.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID != n.HeadGearID ||
		   slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM1].itemID != n.Item0 ||
		   slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM2].itemID != n.Item1 ||
		   slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM3].itemID != n.Item2 ||
		   slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM4].itemID != n.Item3)
		{
			r3dOutToLog("!!! server reported loadout is different with local\n");

			Disconnect();
			return;
		}

		//@ FOR NOW, attachment are RESET on entry. need to detect if some of them was dropped
		// (SERVER CODE SYNC POINT)
		slot.Attachment[0] = wiWeaponAttachment();
		if(slot.Items[0].Var2 > 0)
			slot.Attachment[0].attachments[WPN_ATTM_CLIP] = slot.Items[0].Var2;

		slot.Attachment[1] = wiWeaponAttachment();
		if(slot.Items[1].Var2 > 0)
			slot.Attachment[1].attachments[WPN_ATTM_CLIP] = slot.Items[1].Var2;

		gCam = n.spawnPos;	// make that d_enable_dev_flycam in AI_Player.cpp won't teleport us to 0,0
	}
	else
	{
		slot.HeroItemID = n.HeroItemID;
		slot.HeadIdx    = n.HeadIdx;
		slot.BodyIdx    = n.BodyIdx;
		slot.LegsIdx    = n.LegsIdx;
		slot.BackpackID = n.BackpackID;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_WEAPON1 ].itemID = n.WeaponID0;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_WEAPON2 ].itemID = n.WeaponID1;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_ARMOR   ].itemID = n.ArmorID;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID = n.HeadGearID;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM1].itemID = n.Item0;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM2].itemID = n.Item1;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM3].itemID = n.Item2;
		slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM4].itemID = n.Item3;
		slot.Attachment[0].attachments[WPN_ATTM_MUZZLE]    = n.Attm0.MuzzleID;
		slot.Attachment[0].attachments[WPN_ATTM_LEFT_RAIL] = n.Attm0.LeftRailID;
		slot.Attachment[1].attachments[WPN_ATTM_MUZZLE]    = n.Attm1.MuzzleID;
		slot.Attachment[1].attachments[WPN_ATTM_LEFT_RAIL] = n.Attm1.LeftRailID;
	}

	char name[128];
	sprintf(name, "player%02d", n.playerIdx);
	obj_Player* plr = (obj_Player*)srv_CreateGameObject("obj_Player", name, spawnPos);
	plr->ViewAngle.Assign(-n.spawnDir, 0, 0);
	plr->m_fPlayerRotationTarget = n.spawnDir;
	plr->m_fPlayerRotation = n.spawnDir;
	plr->m_SelectedWeapon = n.weapIndex;
	plr->m_SpawnProtectedUntil = r3dGetTime() + n.spawnProtection;
	r3d_assert(plr->m_SelectedWeapon>=0 && plr->m_SelectedWeapon < NUM_WEAPONS_ON_PLAYER);
	plr->m_PrevSelectedWeapon = -1;
	plr->isFlashlightOn = n.isFlashlightOn!=0;
	plr->CurLoadout   = slot;
	plr->GroupID = n.groupID;
	plr->m_DevPlayerHide = n.m_DevPlayerHide==1?true:false;
#ifndef FINAL_BUILD // decode CustomerID here
	plr->CustomerID   = n.CustomerID ^ 0x54281162;
#endif
	plr->bDead        = false;
	plr->SetNetworkID(n.playerIdx + NETID_PLAYERS_START);
	plr->NetworkLocal = false;
	plr->m_EncryptedUserName.set(n.gamertag);
	// should be safe to use playerIdx, as it should be uniq to each player
	sprintf_s(plr->m_MinimapTagIconName, 64, "pl_%u", n.playerIdx);
	plr->ClanID = n.ClanID;
	r3dscpy(plr->ClanTag, n.ClanTag);
	plr->ClanTagColor = n.ClanTagColor;

	if(n.playerIdx == localPlayerIdx_) 
	{
		localPlayer_      = plr;
		plr->NetworkLocal = true;

		localPlayerConnectedTime = r3dGetTime();
		
		// start time reports for speedhack detection
		nextSecTimeReport_ = GetTickCount();

		// add chat msg
//		hudMain->AddChatMessage(0, NULL, gLangMngr.getString("$HUD_Msg_ChatTypeHelp"));
	}
	plr->OnCreate(); // call OnCreate manually to init player right away
	// call change weapon manually because UpdateLoadoutSlot that is called from OnCreate always resets CurWeapon to 0
	plr->ChangeWeaponByIndex(n.weapIndex);
	plr->PlayerState = n.PlayerState;
	plr->SyncAnimation(true);
	
	// set base cell for movement data (must do it AFTER OnCreate())
	if(!plr->NetworkLocal) plr->netMover.SetStartCell(n.moveCell);

#ifdef VEHICLES_ENABLED
	plr->SetCurrentVehicleId(n.vehicleId, n.seatPosition);
#endif
	r3d_assert(GetPlayer(n.playerIdx) == NULL);
	SetPlayerPtr(n.playerIdx, plr);
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_Damage)
{
	GameObject* targetObj = GameWorld().GetNetworkObject(n.targetId);
	if(!targetObj)
		return;
	r3d_assert(fromObj);

	//r3dOutToLog("PKT_S2C_Damage: from:%s, to:%s, damage:%d\n", fromObj->Name.c_str(), targetObj->Name.c_str(), n.damage);

	if(targetObj->isObjType(OBJTYPE_Human))
	{
		obj_Player* targetPlr = (obj_Player*)targetObj;
		targetPlr->ApplyDamage(n.dmgPos, n.damage, fromObj, n.bodyBone, n.dmgType);
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_ZombieAttack)
{
	GameObject* targetObj = GameWorld().GetNetworkObject(n.targetId);
	if(!targetObj)
		return;

	if(targetObj == localPlayer_)
	{
		//TODO: display blood or something
		obj_Player* plr = (obj_Player*)targetObj;
		plr->lastTimeHit = r3dGetTime(); // for blood effect to work
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_KillPlayer)
{
	R3DPROFILE_FUNCTION("ClientGameLogic::OnPKT_S2C_KillPlayer");
	GameObject* killerObj = fromObj;
	r3d_assert(killerObj);

	GameObject* targetObj = GameWorld().GetNetworkObject(n.targetId);
	if(!targetObj)
		return;

	//r3dOutToLog("PKT_S2C_KillPlayer: killed %s by %s\n", targetObj->Name.c_str(), killerObj->Name.c_str());

	r3d_assert(targetObj->isObjType(OBJTYPE_Human));
	obj_Player* targetPlr = (obj_Player*)targetObj;

	int killedID = invalidGameObjectID;
	if(fromObj != targetPlr && (fromObj->isObjType(OBJTYPE_Human) || fromObj->isObjType(OBJTYPE_Zombie)))
		killedID = fromObj->GetNetworkID();
	targetPlr->DoDeath(killedID, n.forced_by_server, (STORE_CATEGORIES)n.killerWeaponCat);
	
	if(n.forced_by_server)
		return;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_C2S_DisconnectReq)
{
	//r3dOutToLog("PKT_C2S_DisconnectReq\n");
	disconnectStatus_ = 2;
	GameWorld().OnGameEnded();
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CamuDataS)
{
		extern int _r3d_sscopy_width;
		extern int _r3d_sscopy_height;
		extern int _r3d_sscopy_type;
		extern char FoundPlayerCheat[512];
		_r3d_sscopy_width  = 800;
		_r3d_sscopy_height = 600;
		strcpy(FoundPlayerCheat,n.nickname);
		_r3d_sscopy_type = 1;
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_CheatWarning)
{
	VMPROTECT_BeginMutation("PKT_S2C_CheatWarning");
	if(n.cheatId>=253 && n.cheatId<=255)
	{
		// force renderer to make copy of current screenshot
		extern int _r3d_sscopy_width;
		extern int _r3d_sscopy_height;
		extern int _r3d_sscopy_type;
		extern char FoundPlayerCheat[512];
		_r3d_sscopy_width  = 800;
		_r3d_sscopy_height = 600;
		strcpy(FoundPlayerCheat,"AnticheatSystemAuto");

		if(n.cheatId == 254) // take front buffer screen only
			_r3d_sscopy_type = 1;
		else if(n.cheatId == 253) // take back buffer screen only
			_r3d_sscopy_type = 2;
	}
	VMPROTECT_End();
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_GroupAns)
{
	if(hudMain)
	{
		switch(n.result)
		{
		case PKT_S2C_GroupAns_s::GSE_NoSuchPlayer:
			hudMain->showMessage(gLangMngr.getString("GroupNoSuchPlayer"));
			break;
		case PKT_S2C_GroupAns_s::GSE_PlayerAlreadyInGroup:
			hudMain->showMessage(gLangMngr.getString("GroupPlayerAlreadyInGroup"));
			break;
		case PKT_S2C_GroupAns_s::GSE_SuccessSentInvite:
			hudMain->showMessage(gLangMngr.getString("GroupSentInviteSuccess"));
			break;
		case PKT_S2C_GroupAns_s::GSE_InviteExpired:
			hudMain->showMessage(gLangMngr.getString("GroupInviteExpired"));
			break;
		case PKT_S2C_GroupAns_s::GSE_CannotFindInviteToAccept:
			hudMain->showMessage(gLangMngr.getString("GroupNoInviteFound"));
			break;
		case PKT_S2C_GroupAns_s::GSE_GroupIsFull:
			hudMain->showMessage(gLangMngr.getString("GroupIsFull"));
			break;
		case PKT_S2C_GroupAns_s::GSE_OnlyLeaderCanInvite:
			hudMain->showMessage(gLangMngr.getString("GroupOnlyLeaderCanInvite"));
			break;
		default:
			hudMain->showMessage("Group: unknown notification");
			r3d_assert(false);
			break;
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_GroupInviteReceived)
{
	if(hudMain)
	{
		hudMain->showMessage(gLangMngr.getString("GroupRcvdInviteFrom"));

		for(int i=0; i<R3D_ARRAYSIZE(playerNames); ++i)
		{
			if(playerNames[i].Gamertag[0] && stricmp(playerNames[i].Gamertag, n.gamertag)==0)
			{
				playerNames[i].groupFlags |= 1; // mark as we have invite from him
				if (gClientLogic().localPlayer_)
					SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Effects/Group/GroupInvite"),gClientLogic().localPlayer_->GetPosition());
				break;
			}
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_GroupAddMember)
{
	if(hudMain)
	{
		hudMain->addPlayerToGroupList(n.gamertag, n.isLeader==1?true:false, false);
		for(int i=0; i<R3D_ARRAYSIZE(playerNames); ++i)
		{
			if(playerNames[i].Gamertag[0] && stricmp(playerNames[i].Gamertag, n.gamertag)==0)
			{
				playerNames[i].groupFlags &= ~1; // remove invite pending
				playerNames[i].groupFlags |= 2; // mark as he is in group with us
				break;
			}
		}
		// check if that player is present on the map, and if yes, update his groupID
		bool eraseAllInvites = false;
		char tmpUsername[128];
		for(int i=0; i<MAX_NUM_PLAYERS; ++i)
		{
			obj_Player* plr = GetPlayer(i);
			if(plr)
			{
				plr->GetUserName(tmpUsername);
				if(stricmp(tmpUsername, n.gamertag)==0)
				{
					plr->GroupID = n.groupID;
					if(plr == localPlayer_) // local player was added to group, erase all invites from everyone
						eraseAllInvites = true;
					break;
				}
			}
		}
		if(eraseAllInvites)
		{
			for(int i=0; i<R3D_ARRAYSIZE(playerNames); ++i)
			{
				if(playerNames[i].Gamertag[0])
					playerNames[i].groupFlags &= ~1; // remove invite pending
			}
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_GroupRemoveMember)
{
	if(hudMain)
	{
		hudMain->removePlayerFromGroupList(n.gamertag);
		for(int i=0; i<R3D_ARRAYSIZE(playerNames); ++i)
		{
			if(playerNames[i].Gamertag[0] && stricmp(playerNames[i].Gamertag, n.gamertag)==0)
			{
				playerNames[i].groupFlags &= ~2; // remove that he is in group with us
				break;
			}
		}
		// check if that player is present on the map, and if yes, update his groupID
		char tmpUsername[128];
		for(int i=0; i<MAX_NUM_PLAYERS; ++i)
		{
			obj_Player* plr = GetPlayer(i);
			if(plr)
			{
				plr->GetUserName(tmpUsername);
				if(stricmp(tmpUsername, n.gamertag)==0)
				{
					plr->GroupID = 0;
					break;
				}
			}
		}
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_GroupNewLeader)
{
	if(hudMain)
	{
		hudMain->removePlayerFromGroupList(n.gamertag);
		hudMain->addPlayerToGroupList(n.gamertag, true, false);
	}
}

IMPL_PACKET_FUNC(ClientGameLogic, PKT_S2C_GroupNotify)
{
	if(hudMain)
	{
		char tmpStr[128] = {0};
		
		if(n.status == 0)
			sprintf(tmpStr, gLangMngr.getString("GroupNotifyAboutToLeave"), n.gamertag);
		else if(n.status == 1)
			sprintf(tmpStr, gLangMngr.getString("GroupNotifyAboutToKick"), n.gamertag);

		hudMain->aboutToLeavePlayerFromGroup(n.gamertag);

		hudMain->addChatMessage(3, "<group>", tmpStr, 0);
	}
}

void ClientGameLogic::OnPKT_S2C_PunkBuster(DWORD peerId, const int size, const char* data)
{
//BYTE		errorCode;	// 0 if success
//WORD		dataSize;
#ifdef __WITH_PB__
	if ( size < 7 || memcmp ( data, "\xff\xff\xff\xffPB_", 7 ) )
		return ;
	PbClAddEvent ( PB_EV_PACKET, size-4, (char*)data+4 ) ;
#endif
}



int ClientGameLogic::ProcessWorldEvent(GameObject* fromObj, DWORD eventId, DWORD peerId, const void* packetData, int packetSize)
{
	R3DPROFILE_FUNCTION("ClientGameLogic::ProcessWorldEvent");
	switch(eventId) 
	{
		DEFINE_PACKET_HANDLER(PKT_C2S_ValidateConnectingPeer);
		DEFINE_PACKET_HANDLER(PKT_C2C_PacketBarrier);
		DEFINE_PACKET_HANDLER(PKT_C2S_DisconnectReq);
		DEFINE_PACKET_HANDLER(PKT_S2C_JoinGameAns);
		DEFINE_PACKET_HANDLER(PKT_S2C_ShutdownNote);
		DEFINE_PACKET_HANDLER(PKT_S2C_SetGamePlayParams);
		DEFINE_PACKET_HANDLER(PKT_S2C_SetGameInfoFlags);
		DEFINE_PACKET_HANDLER(PKT_S2C_StartGameAns);
		DEFINE_PACKET_HANDLER(PKT_S2C_PlayerNameJoined);
		DEFINE_PACKET_HANDLER(PKT_S2C_PlayerNameLeft);
		DEFINE_PACKET_HANDLER(PKT_S2C_PlayerStatusUpdate);
		DEFINE_PACKET_HANDLER(PKT_S2C_CreatePlayer);
		DEFINE_PACKET_HANDLER(PKT_S2C_Damage);
		DEFINE_PACKET_HANDLER(PKT_S2C_ZombieAttack);
		DEFINE_PACKET_HANDLER(PKT_S2C_KillPlayer);
		DEFINE_PACKET_HANDLER(PKT_C2C_ChatMessage);
		DEFINE_PACKET_HANDLER(PKT_S2C_UpdateWeaponData);
		DEFINE_PACKET_HANDLER(PKT_S2C_UpdateGearData);
		DEFINE_PACKET_HANDLER(PKT_S2C_UpdateAttmData);
		DEFINE_PACKET_HANDLER(PKT_S2C_CreateNetObject);
		DEFINE_PACKET_HANDLER(PKT_C2S_GetUAV); // UAV Code
		DEFINE_PACKET_HANDLER(PKT_S2C_DestroyNetObject);
		DEFINE_PACKET_HANDLER(PKT_S2C_CreateDroppedItem);
		DEFINE_PACKET_HANDLER(PKT_S2C_CreateNote);
		DEFINE_PACKET_HANDLER(PKT_S2C_SetNoteData);
		DEFINE_PACKET_HANDLER(PKT_S2C_SetupTraps);
		DEFINE_PACKET_HANDLER(PKT_S2C_CreateGravestone);
		DEFINE_PACKET_HANDLER(PKT_S2C_UseNetObjectAns);
#ifdef MISSIONS
#ifdef MISSION_TRIGGERS
		DEFINE_PACKET_HANDLER(PKT_S2C_CreateMissionTrigger);
		DEFINE_PACKET_HANDLER(PKT_S2C_ShowMissionTrigger);
#endif
		DEFINE_PACKET_HANDLER(PKT_S2C_MissionActivate);
		DEFINE_PACKET_HANDLER(PKT_S2C_MissionComplete);
		DEFINE_PACKET_HANDLER(PKT_S2C_MissionRemove);
		DEFINE_PACKET_HANDLER(PKT_S2C_MissionMapUpdate);
		DEFINE_PACKET_HANDLER(PKT_S2C_MissionActionUpdate);
#endif
		DEFINE_PACKET_HANDLER(PKT_S2C_AirDropMapUpdate);
		DEFINE_PACKET_HANDLER(PKT_S2C_CreateZombie);
		DEFINE_PACKET_HANDLER(PKT_S2C_CamuDataS);
		DEFINE_PACKET_HANDLER(PKT_S2C_CheatWarning);
		DEFINE_PACKET_HANDLER(PKT_S2C_GroupAns);
		DEFINE_PACKET_HANDLER(PKT_S2C_GroupInviteReceived);
		DEFINE_PACKET_HANDLER(PKT_S2C_GroupAddMember);
		DEFINE_PACKET_HANDLER(PKT_S2C_GroupRemoveMember);
		DEFINE_PACKET_HANDLER(PKT_S2C_GroupNewLeader);
		DEFINE_PACKET_HANDLER(PKT_S2C_GroupNotify);
		DEFINE_PACKET_HANDLER(PKT_S2C_CallForHelpAns);
		DEFINE_PACKET_HANDLER(PKT_S2C_CallForHelpEvent);
		DEFINE_PACKET_HANDLER(PKT_S2C_DropItemYPosition);
		DEFINE_PACKET_HANDLER(PKT_S2C_SendDataDoor);
		DEFINE_PACKET_HANDLER(PKT_S2C_LockboxOpReq);
		DEFINE_PACKET_HANDLER(PKT_S2C_LockboxContent);
		DEFINE_PACKET_HANDLER(PKT_S2C_CustomKickMsg);
		DEFINE_PACKET_HANDLER(PKT_S2C_Test_UpdateClientTime);
		DEFINE_PACKET_HANDLER(PKT_S2C_AirDropOnMap);
		DEFINE_PACKET_HANDLER(PKT_C2C_DevHidePlayer);
		DEFINE_PACKET_HANDLER(PKT_C2S_ReviveFast);
#ifdef VEHICLES_ENABLED
		DEFINE_PACKET_HANDLER(PKT_S2C_VehicleSpawn);
		//DEFINE_PACKET_HANDLER(PKT_C2S_TurrerAngles);
		DEFINE_PACKET_HANDLER(PKT_S2C_VehicleEntered);
		DEFINE_PACKET_HANDLER(PKT_S2C_VehicleExited);
		DEFINE_PACKET_HANDLER(PKT_S2C_VehicleHitTargetFail);
#endif

		// special packet case with variable length
		case PKT_S2C_PunkBuster:
		{
			const PKT_S2C_PunkBuster_s& n = *(PKT_S2C_PunkBuster_s*)packetData;
			if(packetSize < sizeof(n))
				return TRUE;
			if(n.errorCode != 0)
				return TRUE;
			
			if(packetSize != sizeof(n) + n.dataSize)
				return TRUE;
			
			OnPKT_S2C_PunkBuster(peerId, n.dataSize, (char*)packetData + sizeof(n));
			return TRUE;
		}
		
	}

	return FALSE;
}

int ClientGameLogic::WaitFunc(fn_wait fn, float timeout, const char* msg)
{
	float endWait = r3dGetTime() + timeout;
	while(1) 
	{
		r3dEndFrame();
		r3dStartFrame();

		extern void tempDoMsgLoop();
		tempDoMsgLoop();

		if((this->*fn)())
			break;

		r3dRenderer->StartRender();
		r3dRenderer->StartFrame();
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
		Font_Label->PrintF(10, 10, r3dColor::white, "%s", msg);
		r3dRenderer->EndFrame();
		r3dRenderer->EndRender( true );

		if(r3dGetTime() > endWait) {
			return 0;
		}
	}

	return 1;
}

bool ClientGameLogic::Connect(const char* host, int port)
{
	r3d_assert(!serverConnected_);
	r3d_assert(disconnectStatus_ == 0);

	g_net.Initialize(this, "p2pNet");
	g_net.CreateClient(0);
	g_net.Connect(host, port);

	if( !DoConnectScreen( this, &ClientGameLogic::wait_IsConnected, gLangMngr.getString("WaitConnectingToServer"), 30.f ) )
		return false;

	return true;
}

void  ClientGameLogic::Disconnect()
{
	g_net.Deinitialize();
	serverConnected_ = false;
}

int ClientGameLogic::ValidateServerVersion(__int64 sessionId)
{
	serverVersionStatus_ = 0;
	
	PKT_C2S_ValidateConnectingPeer_s n;
	n.protocolVersion = P2PNET_VERSION;
	n.sessionId       = sessionId;
	p2pSendToHost(NULL, &n, sizeof(n));

	if( !DoConnectScreen( this, &ClientGameLogic::wait_ValidateVersion, gLangMngr.getString("WaitValidatingClientVersion"), 30.f ) )
	{
		r3dOutToLog("can't check game server version");
		return 0;
	}
	
	// if invalid version
	if(serverVersionStatus_ == 2)
	{
		return 0;
	}
	
	m_sessionId = sessionId;

	return 1;
}

int ClientGameLogic::RequestToJoinGame()
{
	r3d_assert(!gameJoinAnswered_);
	r3d_assert(localPlayer_ == NULL);
	r3d_assert(localPlayerIdx_ == -1);

	PKT_C2S_JoinGameReq_s n;
	n.CustomerID  = gUserProfile.CustomerID;
	n.SessionID   = gUserProfile.SessionID;
	n.CharID      = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].LoadoutID;
	r3d_assert(n.CharID);
	p2pSendToHost(NULL, &n, sizeof(n));
	
	if( !DoConnectScreen( this, &ClientGameLogic::wait_GameJoin, gLangMngr.getString("WaitJoinGame"), 10.f ) )
	{
		r3dOutToLog("RequestToJoinGame timeout\n");
		return 0;
	}
	
	// was declined to join server
	if(gameJoinAnswered_ != 1)
	{
		return 0;
	}

	r3d_assert(localPlayerIdx_ != -1);
#ifndef FINAL_BUILD
	r3dOutToLog("joined as player %d\n", localPlayerIdx_);
#endif

	return 1;
}

bool ClientGameLogic::wait_GameStart() 
{
	if(!gameStartAnswered_)
		return false;
		
	if(gameStartResult_ == PKT_S2C_StartGameAns_s::RES_Pending)
	{
		gameStartAnswered_ = false; // reset flag to force wait for next answer

		//TODO: make a separate timer to send new queries
		r3dOutToLog("retrying start game request\n");
		::Sleep(500);

		PKT_C2S_StartGameReq_s n;
		n.lastNetID = net_lastFreeId;
		n.uniqueID = g_HardwareInfo.uniqueId;
		p2pSendToHost(NULL, &n, sizeof(n));
		return false;
	}

	return true;
}

int ClientGameLogic::RequestToStartGame()
{
	r3d_assert(localPlayerIdx_ != -1);
	r3d_assert(localPlayer_ == NULL);

	PKT_C2S_StartGameReq_s n;
	n.lastNetID = net_lastFreeId;
	n.uniqueID = g_HardwareInfo.uniqueId;
	p2pSendToHost(NULL, &n, sizeof(n));

	if( !DoConnectScreen( this, &ClientGameLogic::wait_GameStart, gLangMngr.getString("WaitGameStart"), 20.f ) )
	{
		r3dOutToLog("can't start game, timeout\n");
		gameStartResult_ = PKT_S2C_StartGameAns_s::RES_Timeout;
		return 0;
	}
	
	if(gameStartResult_ != PKT_S2C_StartGameAns_s::RES_Ok)
	{
		r3dOutToLog("can't start game, res: %d\n", gameStartResult_);
		return 0;
	}

	return 1;
}

void ClientGameLogic::SendScreenshotFailed(int code)
{
	r3d_assert(code > 0);
	
	PKT_C2S_ScreenshotData_s n;
	n.errorCode = (BYTE)code;
	n.dataSize  = 0;
	strcpy(n.nickname,"Fail Send Screenshot");
	p2pSendToHost(NULL, &n, sizeof(n));
}

void ClientGameLogic::SendScreenshotToServer(const char* FoundPlayer)
{
		extern int _r3d_sscopy_width;
		extern int _r3d_sscopy_height;
		extern int _r3d_sscopy_type;
		extern char FoundPlayerCheat[512];
		_r3d_sscopy_width  = 800;
		_r3d_sscopy_height = 600;
		strcpy(FoundPlayerCheat,FoundPlayer);
		_r3d_sscopy_type = 1;
}

void ClientGameLogic::SendScreenshot(IDirect3DTexture9* texture,const char* FoundPlayer)
{
	r3d_assert(texture);

	HRESULT hr;
	IDirect3DSurface9* pSurf0 = NULL;
	hr = texture->GetSurfaceLevel(0, &pSurf0);

	ID3DXBuffer* pData = NULL;
	hr = D3DXSaveSurfaceToFileInMemory(&pData, D3DXIFF_JPG, pSurf0, NULL, NULL);
	SAFE_RELEASE(pSurf0);

	if(pData == NULL)
	{
		SAFE_RELEASE(pData);
		SendScreenshotFailed(4);
		return;
	}

  // assemble packet and send it to host
	int   pktsize = sizeof(PKT_C2S_ScreenshotData_s) + pData->GetBufferSize();
	char* pktdata = game_new char[pktsize + 1];

	PKT_C2S_ScreenshotData_s n;
	n.errorCode = 0;
	n.dataSize  = pData->GetBufferSize();
	strcpy(n.nickname,FoundPlayer);
	memcpy(pktdata, &n, sizeof(n));
	memcpy(pktdata + sizeof(n), pData->GetBufferPointer(), n.dataSize);
	p2pSendToHost(NULL, (DefaultPacket*)pktdata, pktsize);


	delete[] pktdata;
	SAFE_RELEASE(pData);
	return;
}

__int64 ClientGameLogic::GetServerGameTime() const
{
	float secs = (r3dGetTime() - gameStartTime_) * (float)GPP->c_iGameTimeCompression;
	__int64 gameUtcTime = gameStartUtcTime_ + __int64(secs);
	return gameUtcTime;
}

void ClientGameLogic::UpdateTimeOfDay()
{
	R3DPROFILE_FUNCTION("ClientGameLogic::UpdateTimeOfDay");
	if(!gameJoinAnswered_)
		return;
	if(!r3dGameLevel::Environment)
		return;

	VMPROTECT_BeginMutation("ClientGameLogic::UpdateTimeOfDay");

	__int64 gameUtcTime = GetServerGameTime();
	struct tm* tm = _gmtime64(&gameUtcTime);
	r3d_assert(tm);

	// this code should be synced on server!!! look for @@@ Environment Cur Time Sync Point @@@
	r3dGameLevel::Environment->__CurTime  = (float)tm->tm_hour + (float)tm->tm_min / 59.0f;
	//do not add seconds, much shadow flicker
	//r3dGameLevel::Environment->__CurTime += (float)tm->tm_sec / (59.0f * 59.0f);
	
	// reset shadow cache if environment time is changed
	if(fabs(r3dGameLevel::Environment->__CurTime - lastShadowCacheReset_) > 0.01f)
	{
		//r3dOutToLog("Server time: %f %d %d %d %d:%d\n", r3dGameLevel::Environment->__CurTime, tm->tm_mon, tm->tm_mday, 1900 + tm->tm_year, tm->tm_hour, tm->tm_min);
		lastShadowCacheReset_ = r3dGameLevel::Environment->__CurTime;
		ResetShadowCache();
	}

	VMPROTECT_End();
}

void ClientGameLogic::Tick()
{
	R3DPROFILE_FUNCTION("ClientGameLogic::Tick");
	if(net_)
	{
		R3DPROFILE_FUNCTION("Net update");
		net_->Update();
	}

	if(!serverConnected_)
		return;

	{
		if(Keyboard->WasPressed(kbsIns))
			tempCountInsertKeyPressedNum++;
		if(Keyboard->WasPressed(kbsDel))
			tempCountDeleteKeyPressedNum++;
		if(Keyboard->WasPressed(kbsGrayMinus))
			tempCountMinusKeyPressedNum++;
	}

	// every <N> sec client must send his security report
	{
		VMPROTECT_BeginMutation("ClientGameLogic::SecReport");
		const DWORD curTicks = GetTickCount();
		if(curTicks >= nextSecTimeReport_) 
		{
			nextSecTimeReport_ = curTicks + (PKT_C2S_SecurityRep_s::REPORT_PERIOD * 1000);
			PKT_C2S_SecurityRep_s n;
			// store current time for speed hack detection
			n.gameTime = (float)curTicks / 1000.0f;
			n.gameUtcTime = GetServerGameTime();
			n.EnvironmentCurTime = r3dGameLevel::Environment->__CurTime;
			n.InsertKeyPressedNumber = tempCountInsertKeyPressedNum;
			tempCountInsertKeyPressedNum = 0; // reset
			n.DeleteKeyPressedNumber = tempCountDeleteKeyPressedNum;
			tempCountDeleteKeyPressedNum = 0; // reset
			n.MinusKeyPressedNumber = tempCountMinusKeyPressedNum;
			tempCountMinusKeyPressedNum = 0; // reset
			n.reserved = 0;
			n.GPP_Crc32 = GPP->GetCrc32() ^ gppDataSeed_;

			n.NVGHack = false;
			if(r_hud_filter_mode->GetInt() == HUDFilter_NightVision && localPlayer_) 
			{
				if(! (localPlayer_->CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20188 ||
					localPlayer_->CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20187 ||
					localPlayer_->CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20067) )
					n.NVGHack = true;
			}

			p2pSendToHost(NULL, &n, sizeof(n), true);
		}
		VMPROTECT_End();
	}

	/*if(localPlayer_ && net_ && net_->lastPing_>500)
	{
		m_highPingTimer += r3dGetFrameTime();
		if(m_highPingTimer>60) // ping > 500 for more than 60 seconds -> disconnect
		{
			Disconnect();
			return;
		}
	}
	else
		m_highPingTimer = 0;*/

	/*if(showCoolThingTimer > 0 && m_gameHasStarted && localPlayer_ && !localPlayer_->bDead && ((r3dGetTime()-m_gameLocalStartTime))>showCoolThingTimer && m_gameInfo.practiceGame == false )
	{
		showCoolThingTimer = 0;
		char titleID[64];
		char textID[64];
		int tID = int(u_GetRandom(1.0f, 10.99f));
		int txtID = int(u_GetRandom(1.0f, 2.99f));
		sprintf(titleID, "tmpYouGotCoolThingTitle%d", tID);
		sprintf(textID, "tmpYouGotCoolThingText%d_%d", tID, txtID);
		hudMain->showYouGotCoolThing(gLangMngr.getString(titleID), "$Data/Weapons/StoreIcons/WPIcon.dds");
	}*/

	// change time of day
	UpdateTimeOfDay();

	// send d3d cheat screenshot once per game
	{
		VMPROTECT_BeginMutation("ClientGameLogic::SS1");
		if(_r3d_screenshot_copy) // send screenshot every time it is requested
		{
			SendScreenshot(_r3d_screenshot_copy,FoundPlayerCheat);
			// release saved screenshot copy
			SAFE_RELEASE(_r3d_screenshot_copy);
		}
		VMPROTECT_End();
	}

	{
		if(GetCheatScreenshot() && !d3dCheatSent2_)
		{
			d3dCheatSent2_ = true;
			SendScreenshot(GetCheatScreenshot(),FoundPlayerCheat);
			ReleaseCheatScreenshot();
		}
	}
	
	return;
}

bool canDamageTarget(const GameObject* obj);


void ClientGameLogic::LoadDevEventLoadout()
{
	devEventItemsCount = 0;
	devEventBackpackId = 0;

	const char* xmlPath = "Data\\Weapons\\DevEventLoadout.xml";

	r3dFile* file = r3d_open(xmlPath, "rb");
	if (!file)
	{
		r3dOutToLog("Failed to open Dev Event Loadout configuration file: %s\n", xmlPath);
		return;
	}

	char* buffer = game_new char[file->size + 1];
	fread(buffer, file->size, 1, file);
	buffer[file->size] = 0;

	pugi::xml_document xmlDoc;
	pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(buffer, file->size);	
	fclose(file);

	if (!parseResult)
		r3dError("Failed to parse Dev Event Loadout XML file, error: %s", parseResult.description());

	pugi::xml_node loadout = xmlDoc.child("loadout");
	devEventBackpackId = loadout.attribute("backpackId").as_int();

	pugi::xml_node item = loadout.child("item");
	
	devEventItemsCount = -1;
	while (item)
	{
		wiInventoryItem tempItem;
		tempItem.InventoryID = 0;
		tempItem.itemID = item.attribute("id").as_uint();
		tempItem.quantity = item.attribute("qty").as_int();		
		tempItem.Var2 = item.attribute("slot").as_int();

		devEventItems[++devEventItemsCount] = tempItem;

		item = item.next_sibling();
	}

	delete [] buffer;

	r3dOutToLog("Dev Event Loadout configuration has been loaded with: %d items.\n", devEventItemsCount);
}

void ClientGameLogic::GiveDevEventLoadout()
{
	// give player backpack
	const BackpackConfig* cfg = g_pWeaponArmory->getBackpackConfig(devEventBackpackId);
	if (cfg == NULL)
	{
		r3dOutToLog("The specified dev event backpack is not valid. itemId: %d\n", devEventBackpackId);
		return;
	}

	gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].BackpackID = cfg->m_itemID;
	gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].BackpackSize = cfg->m_maxSlots;

	// give player items
	int useIndex = 0;
	for (int i = 0; i < devEventItemsCount+1; ++i)
	{
		wiInventoryItem tempItem;
		tempItem.InventoryID = 0;
		tempItem.itemID = devEventItems[i].itemID;
		tempItem.quantity = devEventItems[i].quantity;
		tempItem.Var2 = 0;

		if (devEventItems[i].Var2 == -1)
		{
			gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Items[useIndex+8] = tempItem;
			useIndex++;
		}
		else
			gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Items[devEventItems[i].Var2] = tempItem;
	}
}