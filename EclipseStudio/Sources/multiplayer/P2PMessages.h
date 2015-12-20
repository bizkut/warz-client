#ifndef __P2PMESSAGES_H__
#define __P2PMESSAGES_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "r3dNetwork.h"
#include "GameObjects/EventTransport.h"
#include "GameCode/UserProfile.h"
#include "GameCode/UserSkills.h"
#include "../../../ServerNetPackets/NetPacketsGameInfo.h"
#include "../../../ServerNetPackets/NetPacketsWeaponInfo.h"
#include "Gameplay_Params.h"

class GameObject;

// all data packets should be as minimal as possible - so, no data aligning
#pragma pack(push)
#pragma pack(1)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// needs to be upped EACH time WarZ.exe changes to make sure that PunkBuster will work properly
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define P2PNET_VERSION		(0x000001ce + GBWEAPINFO_VERSION + GBGAMEINFO_VERSION + GAMEPLAYPARAM_VERSION)

#define NETID_PLAYERS_START	1		// players [1--MAX_NUM_PLAYERS]
#define MAX_NUM_PLAYERS		512
#define NETID_OBJECTS_START	1026		// various spawned objects

static const int NUM_WEAPONS_ON_PLAYER = wiCharDataFull::CHAR_LOADOUT_ITEM4 + 1 + 1; // one more for unarmed weapon
static const int HANDS_WEAPON_IDX      = NUM_WEAPONS_ON_PLAYER - 1;

enum pkttype_e
{
  PKT_C2S_ValidateConnectingPeer = r3dNetwork::FIRST_FREE_PACKET_ID,

  PKT_C2C_PacketBarrier,		// server<->client per object packet indicating logical barrier for received packets

  PKT_C2S_JoinGameReq,
  PKT_S2C_JoinGameAns,
  PKT_S2C_ShutdownNote,
  PKT_S2C_SetGamePlayParams,
  PKT_S2C_SetGameInfoFlags,
  
  PKT_C2S_StartGameReq,
  PKT_S2C_StartGameAns,
  PKT_C2S_DisconnectReq,		// server<-> client disconnect request

  PKT_C2S_ReviveFast, // revive player fast

  PKT_S2C_PlayerNameJoined,
  PKT_S2C_PlayerNameLeft,
  PKT_S2C_PlayerStatusUpdate,

  PKT_C2S_CallForHelpReq,
  PKT_S2C_CallForHelpAns,
  PKT_S2C_CallForHelpEvent,

  PKT_S2C_CreatePlayer,

  PKT_C2C_PlayerSwitchFlashlight, // todo: reuse with some other packets maybe

  PKT_C2C_DevHidePlayer,

  // UAV packets
  PKT_S2C_UAVSetState,
  PKT_C2S_GetUAV,

  PKT_C2C_PlayerReadyGrenade,
  PKT_C2C_PlayerThrewGrenade,
  PKT_C2C_PlayerReload,
  PKT_C2S_PlayerUnloadClip,
  PKT_C2S_PlayerCombineClip,
  // weapon fire packets
  PKT_C2C_PlayerFired,		// player fired event. play muzzle effect, etc. actual HIT event is separate event.
							// this will increment server side counter, and HIT packet will decrement it. So, need to make sure that for each FIRE packet
							// we send HIT packet to prevent bullet cheating

  PKT_C2C_PlayerHitNothing,
  PKT_C2C_PlayerHitStatic, // hit static geometry
  PKT_C2C_PlayerHitStaticPierced, // hit static geometry and pierced through it, will be followed up by another HIT event
  PKT_C2C_PlayerHitDynamic, // hit something that can be damaged (player, UAV, etc)
  PKT_C2C_PlayerHitResource,

  PKT_S2C_SetPlayerVitals,
  PKT_S2C_SetPlayerLoadout,	// indicate loadout change for not local players
  PKT_S2C_SetPlayerAttachments,
  PKT_S2C_SetPlayerWorldFlags,
  PKT_S2C_PlayerRawDamage,
  PKT_C2S_PlayerEquipAttachment,// equip weapon attachment
  PKT_C2S_PlayerRemoveAttachment,
  PKT_C2C_PlayerSwitchWeapon,
  PKT_C2C_PlayerUseItem,
  PKT_S2C_PlayerUsedItemAns, // this packet is sent for immediate action items, like bandages, or morphine shot
  PKT_C2S_PlayerChangeBackpack,
  PKT_S2C_PlayerRemoveAttachments, // this packet is needed to clear remote players attachments when they swap pistol from main/secondary slot
  PKT_C2S_BackpackDrop,		// player backpack operation
  PKT_C2S_BackpackSwap,
  PKT_C2S_BackpackJoin,
  PKT_C2S_BackpackDisassembleItem,
  PKT_S2C_BackpackAddNew,	// item added to backpack
  PKT_S2C_BackpackModify,	// item quantity changed in backpack
  PKT_S2C_BackpackReplace,	// replace slot in backpack
  PKT_S2C_BackpackUnlock,
  PKT_S2C_InventoryAddNew,	// item added to inventory
  PKT_S2C_InventoryModify,	// item quantity changed in inventory
  PKT_C2S_ShopBuyReq,		// in-game buy item
  PKT_C2S_FromInventoryReq,	// inventory to backpack move
  PKT_C2S_ToInventoryReq,	// backpack to inventory
  PKT_S2C_InventoryOpAns,	// final result for BOTH ShopBuy & InventoryOp
  PKT_C2S_RepairItemReq,
  PKT_S2C_RepairItemAns,
  PKT_S2C_CreateNetObject,
  PKT_S2C_DestroyNetObject,
  PKT_C2S_UseNetObject,
  PKT_C2S_PreparingUseNetObject, // client has to sent this to inform server that he started process to use object (takes 1 sec to complete)
  // special packet for special server objects (we still have NetIds so lets use different packets)
  PKT_S2C_CreateDroppedItem,
  PKT_S2C_UseNetObjectAns, // to inform about some actions regarding using items

  PKT_S2C_DropItemYPosition,
	
  // trade packets
  PKT_C2C_TradeRequest,
  PKT_C2C_TradeItem,

  // craft packets
  PKT_C2S_LearnRecipe,
  PKT_C2S_CraftItem,
  PKT_S2C_CraftAns,

  PKT_S2C_SetupTraps,

  // groups (all group packets are sent with the fact that player who receives packet might not have actual player data present on client)
  PKT_C2S_GroupInvitePlayer,
  PKT_C2S_GroupAcceptInvite,
  PKT_C2S_GroupLeaveGroup,
  PKT_C2S_GroupKickPlayer,

  PKT_S2C_GroupAns,
  PKT_S2C_GroupInviteReceived,
  PKT_S2C_GroupAddMember,
  PKT_S2C_GroupRemoveMember,
  PKT_S2C_GroupNewLeader,
  PKT_S2C_GroupNotify,

  // server notes
  PKT_C2S_CreateNote,
  PKT_S2C_CreateNote,
  PKT_S2C_SetNoteData,

  // server gravestones
  PKT_S2C_CreateGravestone,

  PKT_S2C_SendDataDoor,

  // server lockboxes
  PKT_S2C_LockboxOpReq, // request new code or ask for code, or confirm moving item from\to lockbox
  PKT_C2S_LockboxOpAns, // sent new code or send current code for lockbox
  PKT_C2S_LockboxKeyReset, // when owner wants to reset key code
  PKT_S2C_LockboxContent, // server sends a bunch of those for each item in lockbox, once code was accepted
  PKT_C2S_LockboxItemBackpackToLockbox, // request to move item from backpack to lockbox
  PKT_C2S_LockboxItemLockboxToBackpack, // request to mvoe item from lockbox to backpack
  PKT_C2S_LockboxPickup,
  
#ifdef MISSIONS
  // server mission triggers
#ifdef MISSION_TRIGGERS
  PKT_S2C_CreateMissionTrigger,
  PKT_S2C_ShowMissionTrigger,
#endif
  PKT_C2S_AcceptMission,
  PKT_C2S_AbandonMission,
  PKT_C2S_MissionStateObjectUse,
  PKT_S2C_MissionActivate,
  PKT_S2C_MissionComplete,
  PKT_S2C_MissionRemove,
  PKT_S2C_MissionMapUpdate,
  PKT_S2C_MissionActionUpdate,
#endif

  PKT_S2C_AirDropMapUpdate,
  PKT_S2C_AirDropOnMap,

  // server zombies
  PKT_S2C_CreateZombie,
  PKT_S2C_ZombieSetState,
  PKT_S2C_ZombieSetTurnState,
  PKT_S2C_ZombieCycleFidget,
  PKT_S2C_ZombieAttack,
  PKT_S2C_ZombieSprint,
  PKT_S2C_ZombieTarget,
  PKT_C2S_Zombie_DBG_AIReq,
  PKT_S2C_Zombie_DBG_AIInfo,
  
  // movement packets
  PKT_S2C_MoveTeleport,
  PKT_C2C_MoveSetCell,			// set cell origin for PKT_C2C_MoveRel updates
  PKT_C2C_MoveRel,
  PKT_C2S_MoveCameraLocation, // for FairFight
  PKT_C2C_PlayerJump,

  PKT_S2C_Explosion,			// explosion damage packet, server sends them. used for server grenades.
  PKT_C2S_FallingDamage,		// player fell

  PKT_S2C_Damage,
  PKT_S2C_KillPlayer,
  
  PKT_S2C_AddScore,			// single player message to display hp/gp gain
  PKT_S2C_AddResource,

  PKT_S2C_SpawnExplosion, // spawn visual effect of explosion only

  PKT_C2C_ChatMessage,			// client sends to server chat message, server will relay it to everyone except for sender

  // data update packets
  PKT_C2S_DataUpdateReq,
  PKT_S2C_UpdateWeaponData,
  PKT_S2C_UpdateGearData,
  PKT_S2C_UpdateAttmData,
  
  // light meshes
  PKT_S2C_LightMeshStatus, // sends this when we need to turn off light in light mesh

  // special packets
  PKT_S2C_Test_UpdateClientTime,

  // some security
  PKT_C2S_SecurityRep,
  PKT_S2C_PlayerWeapDataRepReq,
  PKT_C2S_PlayerWeapDataRepAns,
  PKT_S2C_CheatWarning,
  PKT_C2S_ScreenshotData,
  PKT_C2S_CameraPos,

  PKT_S2C_CamuDataS,

  PKT_C2S_PunkBuster,
  PKT_S2C_PunkBuster,
  
  PKT_S2C_CustomKickMsg, // to send custom message to client about why he was kicked
  
  // admin
  PKT_C2S_Admin_GiveItem,

  // some test things
  PKT_C2S_TEST_SpawnDummyReq,
  PKT_C2S_DBG_LogMessage,

#ifdef VEHICLES_ENABLED
  PKT_C2C_VehicleMoveSetCell,
  PKT_C2C_VehicleMoveRel,
  PKT_S2C_VehicleSpawn,
  PKT_C2S_TurrerAngles,
  PKT_C2S_VehicleEnter,
  PKT_S2C_VehicleEntered,
  PKT_C2S_VehicleExit,
  PKT_S2C_VehicleExited,
  PKT_C2C_VehicleAction,
  PKT_S2C_VehicleUnstuck,

  PKT_C2S_VehicleHitTarget,
  PKT_S2C_VehicleHitTargetFail,
  PKT_C2S_VehicleStopZombie,
  PKT_S2C_VehicleDurability,
  PKT_C2S_VehicleRepair,
  PKT_S2C_VehicleFuel,
  PKT_C2S_VehicleRefuel,
  PKT_C2C_VehicleHeadlights,
#endif
  PKT_LAST_PACKET_ID
};

#if PKT_LAST_PACKET_ID > 255
  #error Shit happens, more that 255 packet ids
#endif

#define DEFINE_PACKET_HANDLER(xxx) \
  case xxx: { \
    const xxx##_s&n = *(xxx##_s*)packetData; \
    if(packetSize != sizeof(n)) { \
      r3dOutToLog("!!!!errror!!!! %d packetSize %d != %d\n", xxx, packetSize, sizeof(n)); \
      return TRUE; \
    } \
    bool needPassThru = false; \
    On##xxx(n, fromObj, peerId, needPassThru); \
    return needPassThru ? FALSE : TRUE; \
  }


struct PKT_C2S_ValidateConnectingPeer_s : public DefaultPacketMixin<PKT_C2S_ValidateConnectingPeer>
{
	DWORD		protocolVersion;
	// must be set by client to correctly connect to game server
	__int64		sessionId;
};

struct PKT_C2C_PacketBarrier_s : public DefaultPacketMixin<PKT_C2C_PacketBarrier>
{
};

struct PKT_C2S_JoinGameReq_s : public DefaultPacketMixin<PKT_C2S_JoinGameReq>
{
	DWORD		CustomerID;
	DWORD		SessionID;
	DWORD		CharID;		// character id of 
};

struct PKT_S2C_JoinGameAns_s : public DefaultPacketMixin<PKT_S2C_JoinGameAns>
{
	BYTE		success;
	WORD		playerIdx;

	DWORD		ownerCustomerID; // customerID of owner of the server, if any
	GBGameInfo	gameInfo;
	__int64		gameTime;	// UTC game time
	DWORD		voicePwd;	// password to server
	DWORD		voiceId;	// unique assigned ID for client
};

struct PKT_S2C_ShutdownNote_s : public DefaultPacketMixin<PKT_S2C_ShutdownNote>
{
	BYTE		reason;
	float		timeLeft;
};

struct PKT_S2C_SetGamePlayParams_s : public DefaultPacketMixin<PKT_S2C_SetGamePlayParams>
{
	DWORD		GPP_Seed;	// per-session value used to xor crc of gpp
	CGamePlayParams	GPP_Data;
};

struct PKT_S2C_SetGameInfoFlags_s : public DefaultPacketMixin<PKT_S2C_SetGameInfoFlags>
{
	DWORD		gameInfoFlags;
};

struct PKT_C2S_StartGameReq_s : public DefaultPacketMixin<PKT_C2S_StartGameReq>
{
	DWORD		lastNetID; // to check sync
	DWORD		ArmoryItems;
	__int64		uniqueID; // HWID for FF
};

struct PKT_S2C_StartGameAns_s : public DefaultPacketMixin<PKT_S2C_StartGameAns>
{
	enum EResult {
	  RES_Unactive,
	  RES_Ok      = 1,
	  RES_Pending = 2,		// server still getting your profile
	  RES_Timeout = 3,		// server was unable to get your profile
	  RES_Failed  = 4,
	  RES_UNSYNC  = 5,
	  RES_InvalidLogin = 6,
	  RES_StillInGame  = 7,
	};
	BYTE		result;		// status of joining
};

struct PKT_C2S_DisconnectReq_s : public DefaultPacketMixin<PKT_C2S_DisconnectReq>
{
};

struct PKT_C2S_ReviveFast_s : public DefaultPacketMixin<PKT_C2S_ReviveFast>
{
	gp2pnetid_t	PlayerID;
	int SpawnSelect;
};

struct PKT_S2C_PlayerNameJoined_s : public DefaultPacketMixin<PKT_S2C_PlayerNameJoined>
{
	int		reputation; // -1000 to 1000
	int		ClanID;
	char		gamertag[32*2];
	WORD		playerIdx;
	BYTE		flags; // 1=isLegend, 2=isDev, 4=isPunisher, 8-PremiumAccount
	BYTE		m_DevPlayerHide;
};

struct PKT_S2C_PlayerNameLeft_s : public DefaultPacketMixin<PKT_S2C_PlayerNameLeft>
{
	WORD		playerIdx;
};

// this packet will be send only to players who are in same clan\group, and will be send every 30 sec
struct PKT_S2C_PlayerStatusUpdate_s : public DefaultPacketMixin<PKT_S2C_PlayerStatusUpdate>
{
	WORD		playerIdx;
	float		posX;
	float		posZ;
};

struct PKT_C2S_CallForHelpReq_s : public DefaultPacketMixin<PKT_C2S_CallForHelpReq>
{
	char distress[810]; // current max is 400 symbols
	char reward[300]; // current max is 148 symbols
};

struct PKT_S2C_CallForHelpAns_s : public DefaultPacketMixin<PKT_S2C_CallForHelpAns>
{
	enum CFH_Answer
	{
		CFH_Error = 0, // general error
		CFH_OK = 1, // c4h was posted
		CFH_Duplicate, // already have active c4h
		CFH_Time, // cannot post another c4h due to timeout
	};
	BYTE ansCode;
};

struct PKT_S2C_CallForHelpEvent_s : public DefaultPacketMixin<PKT_S2C_CallForHelpEvent>
{
	WORD		playerIdx;
	char		distress[810]; // current max is 400 symbols
	char		reward[300]; // current max is 148 symbols
	float		timeLeft; // time left for this call for help to stay active
	float		locX;
	float		locZ;
};

// struct used to pass network weapon attachments that affect remote player
struct wiNetWeaponAttm
{
	DWORD		LeftRailID;
	DWORD		MuzzleID;
	
	wiNetWeaponAttm()
	{
	  LeftRailID = 0;
	  MuzzleID   = 0;
	}
};

struct PKT_C2C_PlayerSwitchFlashlight_s : public DefaultPacketMixin<PKT_C2C_PlayerSwitchFlashlight>
{
	BYTE isFlashlightOn;
};

struct PKT_S2C_CreatePlayer_s : public DefaultPacketMixin<PKT_S2C_CreatePlayer>
{
	WORD		playerIdx;
	char		gamertag[32*2];
	r3dPoint3D	spawnPos;
	float		spawnDir;
	float		spawnProtection; // if any
	r3dPoint3D	moveCell;	// cell position from PKT_C2C_MoveSetCell
	int		weapIndex;	// index of equipped weapon (-1 for default)
	BYTE		isFlashlightOn; // global switch if flashlights on weapons turned on or not

	DWORD		HeroItemID;	// ItemID of base character
	BYTE		HeadIdx;
	BYTE		BodyIdx;
	BYTE		LegsIdx;

	BYTE		PlayerState;
	
	// equipped things
	DWORD		WeaponID0;
	DWORD		WeaponID1;
	DWORD		ArmorID;
	DWORD		HeadGearID;
	DWORD		Item0;
	DWORD		Item1;
	DWORD		Item2;
	DWORD		Item3;
	DWORD		BackpackID;
	
	// used for remote player creation
	wiNetWeaponAttm	Attm0;
	wiNetWeaponAttm	Attm1;

	DWORD		CustomerID;	// for our in-game admin purposes

	int		ClanID;
	char		ClanTag[5*2]; // utf8
	int		ClanTagColor;

	BYTE m_DevPlayerHide;

	uint32_t groupID;

#ifdef VEHICLES_ENABLED
	DWORD vehicleId;
	BYTE seatPosition;
#endif
};

struct PKT_C2C_DevHidePlayer_s : public DefaultPacketMixin<PKT_C2C_DevHidePlayer>
{
	BYTE m_DevPlayerHide;
	gp2pnetid_t	PlayerID;
	DWORD peerID;
};

struct PKT_C2C_PlayerReadyGrenade_s : public DefaultPacketMixin<PKT_C2C_PlayerReadyGrenade>
{
	BYTE		wid;
};

struct PKT_C2C_PlayerThrewGrenade_s : public DefaultPacketMixin<PKT_C2C_PlayerThrewGrenade>
{
	gobjid_t	localId;	// local game object ID for server tracking purpuses
	gp2pnetid_t	spawnID;	// network ID, for reconnecting a RelayPacket generated grenade with the network mover
	r3dPoint3D	fire_from;	// position of character when he is firing
	r3dPoint3D	fire_to;
	float		holding_delay;	// if any, used for grenades. THIS IS TEMP until grenades are moved to server(!!)
	BYTE		wid;
};

struct PKT_C2C_PlayerReload_s : public DefaultPacketMixin<PKT_C2C_PlayerReload>
{
	BYTE		WeaponSlot;
	BYTE		AmmoSlot;
	BYTE		dbg_Amount;	// actual number of bullets reloaded
};

struct PKT_C2S_PlayerUnloadClip_s : public DefaultPacketMixin<PKT_C2S_PlayerUnloadClip>
{
	BYTE		WeaponSlot;
};

struct PKT_C2S_PlayerCombineClip_s : public DefaultPacketMixin<PKT_C2S_PlayerCombineClip>
{
	BYTE		SlotFrom;
};

struct PKT_C2C_PlayerFired_s : public DefaultPacketMixin<PKT_C2C_PlayerFired>
{
	gobjid_t	localId;	// local game object ID for server tracking purpuses
	gp2pnetid_t	spawnID;	// network ID, for reconnecting a RelayPacket generated flare-gun flare with the network mover
	BYTE		fireSeqNo;	// used for shotgun bullets

	r3dPoint3D	fire_from; // position of character when he is firing
	r3dPoint3D	fire_to;
	float		holding_delay; // if any, used for grenades. THIS IS TEMP until grenades are moved to server(!!)
	BYTE		debug_wid;	// weapon index for debug
};

struct PKT_C2C_PlayerHitNothing_s : public DefaultPacketMixin<PKT_C2C_PlayerHitNothing>
{
	gobjid_t	localId;	// local game object ID for server tracking purpuses
};

struct PKT_C2C_PlayerHitStatic_s : public DefaultPacketMixin<PKT_C2C_PlayerHitStatic>
{
	gobjid_t	localId;	// local game object ID for server tracking purpuses
	r3dPoint3D	hit_pos;
	r3dPoint3D	hit_norm;
	uint32_t	hash_obj;
	BYTE		decalIdx;
	BYTE		particleIdx;
};

//IMPORTANT: This packet should be equal to PKT_C2C_PlayerHitStatic_s!!!
struct PKT_C2C_PlayerHitStaticPierced_s : public DefaultPacketMixin<PKT_C2C_PlayerHitStaticPierced>
{
	gobjid_t	localId;	// local game object ID for server tracking purpuses
	r3dPoint3D	hit_pos;
	r3dPoint3D	hit_norm;
	uint32_t	hash_obj;
	BYTE		decalIdx;
	BYTE		particleIdx;
};

struct PKT_C2C_PlayerHitDynamic_s : public DefaultPacketMixin<PKT_C2C_PlayerHitDynamic>
{
	gobjid_t	localId;	// local game object ID for server tracking purpuses
	r3dPoint3D	muzzler_pos; // for anti cheat
	r3dPoint3D	hit_pos; // where your bullet hit
	gp2pnetid_t	targetId; // what you hit
	BYTE		hit_body_bone; // which bone we hit (for ragdoll)
	BYTE		state; // [0] - from player in air [1] target player in air
	
	// NEEDED FOR SERVER ONLY. TEMP. WILL BE REFACTORED AND MOVED TO SERVER.
	BYTE		hit_body_part;// where we hit player (head, body, etc)
	BYTE		damageFromPiercing; // 0 - no reduction, 100 - no damage at all
};

struct PKT_C2C_PlayerHitResource_s : public DefaultPacketMixin<PKT_C2C_PlayerHitResource>
{
	// just "give me warez packet"
	BYTE		ResType;	// enum from MaterialType::hasResourcesToCollect
};

struct PKT_S2C_MoveTeleport_s : public DefaultPacketMixin<PKT_S2C_MoveTeleport>
{
	r3dPoint3D	teleport_pos; // don't forget to PKT_C2C_PacketBarrier ir
};

struct PKT_C2C_MoveSetCell_s : public DefaultPacketMixin<PKT_C2C_MoveSetCell>
{
	// define radius where relative packets will be sent
	const static int PLAYER_CELL_RADIUS = 5;  // packed difference: 0.04m
	const static int UAV_CELL_RADIUS    = 10;
	const static int VEHICLE_CELL_RADIUS = 10;

	r3dPoint3D	pos;
};

struct PKT_C2C_MoveRel_s : public DefaultPacketMixin<PKT_C2C_MoveRel>
{
	// (CELL_RADIUS*2)/[0-255] offset from previously received absolute position
	BYTE		rel_x;
	BYTE		rel_y;
	BYTE		rel_z;

	BYTE		turnAngle;	// [0..360] packed to [0..255]
	BYTE		bendAngle;	// [-PI/2..PI/2] packet to [0..255]
	USHORT		state;		// reflected PlayerState. [0..8] bits - state, [9,11] - dir
};

struct PKT_C2S_MoveCameraLocation_s : public DefaultPacketMixin<PKT_C2S_MoveCameraLocation>
{
	float		cam_loc_x;
	float		cam_loc_y;
	float		cam_loc_z;
	float		cam_dir_x;
	float		cam_dir_y;
	float		cam_dir_z;
};

struct PKT_C2C_PlayerJump_s : public DefaultPacketMixin<PKT_C2C_PlayerJump>
{
};

struct PKT_S2C_SetPlayerVitals_s : public DefaultPacketMixin<PKT_S2C_SetPlayerVitals>
{
	BYTE		Health;
	BYTE		Thirst;
	BYTE		Hunger;
	BYTE		Toxic;
	enum MedSystemEnum
	{
		MSE_None = 0,
		MSE_Bleeding = 0x01,
		MSE_Fever = 0x02,
		MSE_BloodInfection = 0x04,
	};
	BYTE		MedSystemStatus; // for now we will support only 15 med states, which should be plenty for now
									// client will not know how long effect lasts, only that it is active and server will sends regarding player's vitals
	
	void FromChar(const wiCharDataFull* slot)
	{
		Health = (BYTE)slot->Health;
		Thirst = (BYTE)slot->Thirst;
		Hunger = (BYTE)slot->Hunger;
		Toxic  = (BYTE)slot->Toxic;
		MedSystemStatus = MSE_None;
		if(slot->MedBleeding>0)
			MedSystemStatus |= MSE_Bleeding;
		if(slot->MedFeverCold)
			MedSystemStatus |= MSE_Fever;
		if(slot->MedBloodInfection)
			MedSystemStatus |= MSE_BloodInfection;
	}
	bool operator==(const PKT_S2C_SetPlayerVitals_s& rhs) const
	{
		if(Health != rhs.Health) return false;
		if(Thirst != rhs.Thirst) return false;
		if(Hunger != rhs.Hunger) return false;
		if(Toxic  != rhs.Toxic)  return false;
		if(MedSystemStatus != rhs.MedSystemStatus) return false;
		return true;
	}
	bool operator!=(const PKT_S2C_SetPlayerVitals_s& rhs) const { return !((*this)==rhs); }
};

struct PKT_S2C_SetPlayerLoadout_s : public DefaultPacketMixin<PKT_S2C_SetPlayerLoadout>
{
	DWORD		WeaponID0;
	DWORD		WeaponID1;
	DWORD		QuickSlot1;
	DWORD		QuickSlot2;
	DWORD		QuickSlot3;
	DWORD		QuickSlot4;
	DWORD		ArmorID;
	DWORD		HeadGearID;
	DWORD		BackpackID;
};

struct PKT_S2C_SetPlayerAttachments_s : public DefaultPacketMixin<PKT_S2C_SetPlayerAttachments>
{
	BYTE		wid;
	wiNetWeaponAttm	Attm;
};

struct PKT_S2C_SetPlayerWorldFlags_s : public DefaultPacketMixin<PKT_S2C_SetPlayerWorldFlags>
{
	DWORD		flags;
};

struct PKT_S2C_PlayerRawDamage_s : public DefaultPacketMixin<PKT_S2C_PlayerRawDamage>
{
	BYTE		bodyPart;
	float		damage;
};

struct PKT_C2S_PlayerEquipAttachment_s : public DefaultPacketMixin<PKT_C2S_PlayerEquipAttachment>
{
	BYTE		wid;
	BYTE		AttmSlot;
	DWORD		dbg_WeaponID;
	DWORD		dbg_AttmID;
	BYTE		dbg_Amount;	// in case of clip attachment - number of bullets
};

struct PKT_C2S_PlayerRemoveAttachment_s : public DefaultPacketMixin<PKT_C2S_PlayerRemoveAttachment>
{
	BYTE		wid;
	BYTE		WpnAttmType;	// of WeaponAttachmentTypeEnum
};

struct PKT_C2C_PlayerSwitchWeapon_s : public DefaultPacketMixin<PKT_C2C_PlayerSwitchWeapon>
{
	BYTE		wid; // 255 - means empty hands
};

struct PKT_C2C_PlayerUseItem_s : public DefaultPacketMixin<PKT_C2C_PlayerUseItem>
{
	BYTE		SlotFrom;	// backpack slot
	DWORD		dbg_ItemID;
	r3dPoint3D	pos;
	// various parameters for items
	float		var1;
	float		var2;
	float		var3;
	DWORD		var4;
};

struct PKT_S2C_PlayerUsedItemAns_s : public DefaultPacketMixin<PKT_S2C_PlayerUsedItemAns>
{
	DWORD		itemId;
	// various parameters for items
	float		var1;
	float		var2;
	float		var3;
	float		var4;
};

struct PKT_C2S_PlayerChangeBackpack_s : public DefaultPacketMixin<PKT_C2S_PlayerChangeBackpack>
{
	BYTE		SlotFrom;
	BYTE		BackpackSize;	// to verify against server
};

struct PKT_S2C_PlayerRemoveAttachments_s : public DefaultPacketMixin<PKT_S2C_PlayerRemoveAttachments>
{
	BYTE idx;
};

struct PKT_C2S_BackpackDrop_s : public DefaultPacketMixin<PKT_C2S_BackpackDrop>
{
	BYTE		SlotFrom;
	r3dPoint3D	pos;
};

struct PKT_C2S_BackpackSwap_s : public DefaultPacketMixin<PKT_C2S_BackpackSwap>
{
	BYTE		SlotFrom;
	BYTE		SlotTo;
};

struct PKT_C2S_BackpackJoin_s : public DefaultPacketMixin<PKT_C2S_BackpackJoin>
{
	BYTE		SlotFrom;
	BYTE		SlotTo;
};

struct PKT_C2S_BackpackDisassembleItem_s : public DefaultPacketMixin<PKT_C2S_BackpackDisassembleItem>
{
	BYTE		SlotFrom;
};

struct PKT_S2C_BackpackAddNew_s : public DefaultPacketMixin<PKT_S2C_BackpackAddNew>
{
	BYTE		SlotTo;
	wiInventoryItem	Item;
};

struct PKT_S2C_BackpackModify_s : public DefaultPacketMixin<PKT_S2C_BackpackModify>
{
	BYTE		SlotTo;					// or 0xFF when there is NO free slot
	WORD		Quantity;				// target quantity, 0 for removing item
	DWORD		dbg_ItemID;				// to check
	bool		IsAttachmentReplyReq;	// in the event an attachment and weapon destroy at the same time, we don't need to know to remove the attachment
};

struct PKT_S2C_BackpackReplace_s : public DefaultPacketMixin<PKT_S2C_BackpackReplace>
{
	BYTE		SlotTo;
	wiInventoryItem	Item;
};

struct PKT_S2C_BackpackUnlock_s : public DefaultPacketMixin<PKT_S2C_BackpackUnlock>
{
};

struct PKT_S2C_InventoryAddNew_s : public DefaultPacketMixin<PKT_S2C_InventoryAddNew>
{
	wiInventoryItem	Item;
};

struct PKT_S2C_InventoryModify_s : public DefaultPacketMixin<PKT_S2C_InventoryModify>
{
	__int64		InventoryID;
	WORD		Quantity;	// target quantity, 0 for removing item
	
	DWORD		dbg_ItemID;	// to check
};

struct PKT_C2S_ShopBuyReq_s : public DefaultPacketMixin<PKT_C2S_ShopBuyReq>
{
	DWORD		ItemID;
	BYTE		BuyIdx;		// 4 - cash, 8 - gold
};

struct PKT_C2S_FromInventoryReq_s : public DefaultPacketMixin<PKT_C2S_FromInventoryReq>
{
	__int64		InventoryID;
	BYTE		SlotTo;		// or 0xFF if server should decide target slot
	int		Amount;
	
	DWORD		dbg_ItemID;
	WORD		dbg_Quantity;
	DWORD		dbg_InvItemID;
	WORD		dbg_InvQuantity;
};

struct PKT_C2S_ToInventoryReq_s : public DefaultPacketMixin<PKT_C2S_ToInventoryReq>
{
	BYTE		SlotFrom;
	int		Amount;
	
	DWORD		dbg_ItemID;
	WORD		dbg_Quantity;
};

struct PKT_S2C_InventoryOpAns_s : public DefaultPacketMixin<PKT_S2C_InventoryOpAns>
{
	enum EAns
	{
	  ANS_Success = 0,
	  ANS_NoItem,
	  ANS_NoPrice,
	  ANS_NoMoney,
	  ANS_NoBackpackSpace,
	  ANS_WeightLimit,
	  ANS_NoInventorySpace,
	  ANS_Desync,
	  ANS_BadTarget,
	  ANS_Fail,
	};
	BYTE		OpAns;
	// current balance (for SHOP operation)
	int		GamePoints;
	int		GameDollars;
};

struct PKT_C2S_RepairItemReq_s : public DefaultPacketMixin<PKT_C2S_RepairItemReq>
{
	BYTE		SlotFrom;
	BYTE		RepairMode;	// 0: cash single, 2: repair kit, 3: premium repair kit
};

struct PKT_S2C_SetupTraps_s : public DefaultPacketMixin<PKT_S2C_SetupTraps>
{
	gp2pnetid_t	spawnID;
	BYTE	m_Activate;
};

struct PKT_S2C_RepairItemAns_s : public DefaultPacketMixin<PKT_S2C_RepairItemAns>
{
	BYTE		SlotFrom;	// 0xFF: failed to repair, not enough GD
	int		Var3;
	int		GameDollars;
};

struct PKT_S2C_CreateNetObject_s : public DefaultPacketMixin<PKT_S2C_CreateNetObject>
{
	gp2pnetid_t	spawnID;
	DWORD		itemID;

	gp2pnetid_t	userID;

	r3dPoint3D	pos;
	// various parameters for items
	float		var1;
	float		var2;
	float		var3;
	DWORD		var4;	// Grenades use this for the Player's Network ID
	DWORD		var5;	// Grenades use this to associate the original throwing client's grenade with the grenade's Network ID.
};

struct PKT_S2C_DestroyNetObject_s : public DefaultPacketMixin<PKT_S2C_DestroyNetObject>
{
	gp2pnetid_t	spawnID;
};

struct PKT_S2C_DropItemYPosition_s : public DefaultPacketMixin<PKT_S2C_DropItemYPosition>
{
	float YPos;
	gp2pnetid_t	spawnID;
};

struct PKT_C2S_UseNetObject_s : public DefaultPacketMixin<PKT_C2S_UseNetObject>
{
	gp2pnetid_t	spawnID;
	float clientTime;
};

struct PKT_C2S_GetUAV_s : public DefaultPacketMixin<PKT_C2S_GetUAV>
{
	gp2pnetid_t	OwnerDrone;
	gp2pnetid_t	UAVID;
};

struct PKT_S2C_UseNetObjectAns_s : public DefaultPacketMixin<PKT_S2C_UseNetObjectAns>
{
	enum NetObjectAnswer
	{
		And_Error = 0,
		Ans_HarvestNotReady = 1,
		Ans_HarvestedFarm,
	};
	BYTE ans;
};

struct PKT_C2S_PreparingUseNetObject_s : public DefaultPacketMixin<PKT_C2S_PreparingUseNetObject>
{
	float clientTime;
};

struct PKT_S2C_CreateDroppedItem_s : public DefaultPacketMixin<PKT_S2C_CreateDroppedItem>
{
	gp2pnetid_t	spawnID;
	r3dPoint3D	pos;
	BYTE		FirstTime;
	
	wiInventoryItem	Item;
};

struct PKT_C2C_TradeRequest_s : public DefaultPacketMixin<PKT_C2C_TradeRequest>
{
	enum EOp
	{
	  TRADE_Request,	// C2C
	  TRADE_Answer,		// C2S
	  TRADE_Open,
	  TRADE_Close,
	  TRADE_Confirm,	// S2C: other player accept set
	  TRADE_Clear,		// S2C: other player accept cleared
	  TRADE_Busy,		// other player is busy
	  TRADE_NoSpace,	// other player don't have enough free space
	  TRADE_NoWeight,	// other player can't carry
	};

	PKT_C2C_TradeRequest_s() {}
	PKT_C2C_TradeRequest_s(BYTE in_code, const GameObject* target)
	{
		code     = in_code;
		targetId = toP2pNetId(target->GetSafeNetworkID());
	}
	
	gp2pnetid_t	targetId;
	BYTE		code;
};

struct PKT_C2C_TradeItem_s : public DefaultPacketMixin<PKT_C2C_TradeItem>
{
	BYTE		BckSlot;	// backpack slot
	BYTE		TradeSlot;	// trade slot
	wiInventoryItem Item;		// expected trade item
};

struct PKT_C2S_LearnRecipe_s : public DefaultPacketMixin<PKT_C2S_LearnRecipe>
{
	BYTE slotFrom;
};

struct PKT_C2S_CraftItem_s : public DefaultPacketMixin<PKT_C2S_CraftItem>
{
	uint32_t recipeID;
};

struct PKT_S2C_CraftAns_s : public DefaultPacketMixin<PKT_S2C_CraftAns>
{
	BYTE ans; // 0-item not crafted, error; 1-item crafted
};

struct PKT_C2S_GroupInvitePlayer_s : public DefaultPacketMixin<PKT_C2S_GroupInvitePlayer>
{
	char		gamertag[32*2];
};

struct PKT_C2S_GroupAcceptInvite_s : public DefaultPacketMixin<PKT_C2S_GroupAcceptInvite>
{
	char		gamertag[32*2]; // accepted invite from whom
};

struct PKT_C2S_GroupLeaveGroup_s : public DefaultPacketMixin<PKT_C2S_GroupLeaveGroup>
{

};

struct PKT_C2S_GroupKickPlayer_s : public DefaultPacketMixin<PKT_C2S_GroupKickPlayer>
{
	char		gamertag[32*2];
};

struct PKT_S2C_GroupAns_s : public DefaultPacketMixin<PKT_S2C_GroupAns>
{
	enum GroupServerOpError
	{
		GSE_NoSuchPlayer,
		GSE_PlayerAlreadyInGroup,
		GSE_SuccessSentInvite,
		GSE_InviteExpired,
		GSE_CannotFindInviteToAccept,
		GSE_GroupIsFull,
		GSE_OnlyLeaderCanInvite,
	};
	BYTE result; 
};

struct PKT_S2C_GroupInviteReceived_s : public DefaultPacketMixin<PKT_S2C_GroupInviteReceived>
{
	char		gamertag[32*2];
};

struct PKT_S2C_GroupAddMember_s : public DefaultPacketMixin<PKT_S2C_GroupAddMember>
{
	uint32_t	groupID;
	char		gamertag[32*2];
	BYTE		isLeader;
};

struct PKT_S2C_GroupRemoveMember_s : public DefaultPacketMixin<PKT_S2C_GroupRemoveMember>
{
	char		gamertag[32*2];
};

struct PKT_S2C_GroupNewLeader_s : public DefaultPacketMixin<PKT_S2C_GroupNewLeader>
{
	char		gamertag[32*2];
};

struct PKT_S2C_GroupNotify_s : public DefaultPacketMixin<PKT_S2C_GroupNotify>
{
	BYTE status; // 0 - player about to leave from group, 1-player about to be kicked from group
	char gamertag[32*2];
};

struct PKT_C2S_CreateNote_s : public DefaultPacketMixin<PKT_C2S_CreateNote>
{
	BYTE		SlotFrom;	// backpack slot
	r3dPoint3D	pos;
	char		TextFrom[128];
	char		TextSubj[1024];
};

struct PKT_S2C_CreateNote_s : public DefaultPacketMixin<PKT_S2C_CreateNote>
{
	gp2pnetid_t	spawnID;
	r3dPoint3D	pos;
};

struct PKT_S2C_SetNoteData_s : public DefaultPacketMixin<PKT_S2C_SetNoteData>
{
	char		TextFrom[128];
	char		TextSubj[1024];
};

struct PKT_S2C_CreateGravestone_s : public DefaultPacketMixin<PKT_S2C_CreateGravestone>
{
	gp2pnetid_t	spawnID;
	r3dPoint3D	pos;

	BYTE		GravestoneInfo;
	char		Aggressor[64];
	char		Victim[64];
};

struct PKT_S2C_SendDataDoor_s : public DefaultPacketMixin<PKT_S2C_SendDataDoor>
{
	gp2pnetid_t	DoorID;
	BYTE		m_OpenDoor;
	BYTE		DoorIsDestroy;
	BYTE		ExecuteParticle;
};

struct PKT_S2C_LockboxOpReq_s : public DefaultPacketMixin<PKT_S2C_LockboxOpReq>
{
	enum LockBoxOpReq
	{
		LBOR_Error = 0,
		LBOR_Ok,
		LBOR_SetNewCode,
		LBOR_AskForCode,
		LBOR_WrongCode,
		LBOR_StartingToSendContent,
		LBOR_DoneSendingContent, // after openning lockbox, server will send content and will finish with this packet
		LBOR_CodeChanged,
		LBOR_SecurityLockdown,
		LBOR_NotOwner,
		LBOR_Close,
	};
	BYTE		op; 
	gp2pnetid_t	lockboxID;
};

struct PKT_C2S_LockboxOpAns_s : public DefaultPacketMixin<PKT_C2S_LockboxOpAns>
{
	gp2pnetid_t	lockboxID;
	char		AccessCodeS[32];
};

struct PKT_C2S_LockboxKeyReset_s : public DefaultPacketMixin<PKT_C2S_LockboxKeyReset>
{
	gp2pnetid_t	lockboxID;
	char		old_AccessCodeS[32];
	char		new_AccessCodeS[32];
};

struct PKT_S2C_LockboxContent_s : public DefaultPacketMixin<PKT_S2C_LockboxContent>
{
	wiInventoryItem item;
};

struct PKT_C2S_LockboxItemLockboxToBackpack_s : public DefaultPacketMixin<PKT_C2S_LockboxItemLockboxToBackpack>
{
	gp2pnetid_t	lockboxID;
	char		AccessCodeS[32]; // todo: maybe replace with some temp pin?
	uint32_t	LockboxInventoryID;	// don't confuse with InventoryID of item, this is index inside lockbox
	BYTE		SlotTo;			// or 0xFF if server should decide target slot
	int		Amount;

	DWORD		dbg_ItemID;
	WORD		dbg_Quantity;
	DWORD		dbg_InvItemID;
	WORD		dbg_InvQuantity;
};

struct PKT_C2S_LockboxItemBackpackToLockbox_s : public DefaultPacketMixin<PKT_C2S_LockboxItemBackpackToLockbox>
{
	gp2pnetid_t	lockboxID;
	char		AccessCodeS[32];
	BYTE		SlotFrom;
	int		Amount;

	DWORD		dbg_ItemID;
	WORD		dbg_Quantity;
};

struct PKT_C2S_LockboxPickup_s : public DefaultPacketMixin<PKT_C2S_LockboxPickup>
{
	gp2pnetid_t	lockboxID;
	char		AccessCodeS[32];
};

#ifdef MISSIONS
#ifdef MISSION_TRIGGERS
struct PKT_S2C_CreateMissionTrigger_s : public DefaultPacketMixin<PKT_S2C_CreateMissionTrigger>
{
	gp2pnetid_t	spawnID;
	r3dPoint3D	pos;
	uint32_t	missionID;
	char		missionTitleStringID[16];
	char		missionNameStringID[16];
	char		missionDescStringID[16];
};

struct PKT_S2C_ShowMissionTrigger_s : public DefaultPacketMixin<PKT_S2C_ShowMissionTrigger>
{
};
#endif

struct PKT_C2S_AcceptMission_s : public DefaultPacketMixin<PKT_C2S_AcceptMission>
{
	uint32_t	missionID;
};

struct PKT_C2S_AbandonMission_s : public DefaultPacketMixin<PKT_C2S_AbandonMission>
{
	uint32_t	missionID;
};

struct PKT_C2S_MissionStateObjectUse_s : public DefaultPacketMixin<PKT_C2S_MissionStateObjectUse>
{
	uint32_t	objHash;
};

struct PKT_S2C_MissionActivate_s : public DefaultPacketMixin<PKT_S2C_MissionActivate>
{
	uint32_t	missionID;
	BYTE		data;	// bit 1 = active, bit 2 = accepted, bit3 = save/button-remove disallowed
	char		missionNameStringID[16];
	char		missionDescStringID[16];
	char		missionIconPath[64];
};

struct PKT_S2C_MissionComplete_s : public DefaultPacketMixin<PKT_S2C_MissionComplete>
{
	uint32_t	missionID;
};

struct PKT_S2C_MissionRemove_s : public DefaultPacketMixin<PKT_S2C_MissionRemove>
{
	uint32_t	missionID;
	uint32_t	reason;
};

struct PKT_S2C_MissionMapUpdate_s : public DefaultPacketMixin<PKT_S2C_MissionMapUpdate>
{
	uint32_t	actionID;
	uint32_t	actionIcon;
	r3dPoint3D	location;
	bool		active;
};

struct PKT_S2C_AirDropMapUpdate_s : public DefaultPacketMixin<PKT_S2C_AirDropMapUpdate>
{
	r3dPoint3D	location;
	float		m_time;
};

struct PKT_S2C_AirDropOnMap_s : public DefaultPacketMixin<PKT_S2C_AirDropOnMap>
{
	uint32_t NumbeAirDrop;
};

struct PKT_S2C_MissionActionUpdate_s : public DefaultPacketMixin<PKT_S2C_MissionActionUpdate>
{
	uint32_t	missionID;
	uint32_t	actionID;
	uint32_t	actionType;	// Lower 16 bits is ActionType, Upper 16 bits is ItemActionType OR ActionState depending on the ActionType
	uint32_t	amount;		// How many to Kill/Use/Drop/Collect
	uint32_t	count;		// How many have been Killed/Used/Dropped/Collected, OR, what is the current state
	uint32_t	itemID;		// Item/Weapon required to complete action.  UINT_MAX is used to signify multiple items, and 0 signifies no items.
	uint32_t	objType;	// What to kill for KillActions
	r3dPoint3D	position;
	r3dVector	extents;
	bool		completed;
	char		missionName[16];
};

#endif

struct PKT_S2C_CreateZombie_s : public DefaultPacketMixin<PKT_S2C_CreateZombie>
{
	gp2pnetid_t	spawnID;
	r3dPoint3D	spawnPos;
	float		spawnDir;
	r3dPoint3D	moveCell;	// cell position from PKT_C2C_MoveSetCell
	DWORD		HeroItemID;	// ItemID of base character
	BYTE		HeadIdx;
	BYTE		BodyIdx;
	BYTE		LegsIdx;
	BYTE		State;		// ZombieStates::EZombieStates
	BYTE		FastZombie;
	float		WalkSpeed;
	float		RunSpeed;
	float		SprintMaxSpeed;
	float		SprintMaxTime;
	float		SprintSlope;
	float		SprintCooldownTime;
};

struct PKT_S2C_ZombieSetState_s : public DefaultPacketMixin<PKT_S2C_ZombieSetState>
{
	BYTE		State;		// ZombieStates::EZombieStates
};

struct PKT_S2C_ZombieCycleFidget_s : public DefaultPacketMixin<PKT_S2C_ZombieCycleFidget>
{
	BYTE		Fidget;
};

struct PKT_S2C_ZombieSetTurnState_s : public DefaultPacketMixin<PKT_S2C_ZombieSetTurnState>
{
	BYTE		State;		// ZombieStates::EZombieStates
	float		TargetAngle;
};

struct PKT_S2C_ZombieAttack_s : public DefaultPacketMixin<PKT_S2C_ZombieAttack>
{
	gp2pnetid_t	targetId;
};

struct PKT_S2C_ZombieSprint_s : public DefaultPacketMixin<PKT_S2C_ZombieSprint>
{
};

struct PKT_S2C_ZombieTarget_s : public DefaultPacketMixin<PKT_S2C_ZombieTarget>
{
	gp2pnetid_t targetId;
};

struct PKT_C2S_Zombie_DBG_AIReq_s : public DefaultPacketMixin<PKT_C2S_Zombie_DBG_AIReq>
{
};

struct PKT_S2C_Zombie_DBG_AIInfo_s : public DefaultPacketMixin<PKT_S2C_Zombie_DBG_AIInfo>
{
	r3dPoint3D	from;
	r3dPoint3D	to;
};

//This packet will always apply damage, even to friendlies!!!
struct PKT_S2C_Explosion_s : public DefaultPacketMixin<PKT_S2C_Explosion>
{
	gp2pnetid_t	spawnID;
	r3dPoint3D	explosion_pos;
	r3dVector	forwVector;
	r3dVector	lastCollisionNormal;	// used for orienting decals on the client
	float		direction;
};

struct PKT_C2S_FallingDamage_s : public DefaultPacketMixin<PKT_C2S_FallingDamage>
{
	BYTE		damage;
};

struct PKT_S2C_Damage_s : public DefaultPacketMixin<PKT_S2C_Damage>
{
	r3dPoint3D	dmgPos; // position of the damage. for bullets: player position. for grenades\mines\rpg - position of explosion
	gp2pnetid_t	targetId;
	BYTE		damage;
	BYTE		dmgType;
	BYTE		bodyBone;
};

struct PKT_S2C_KillPlayer_s : public DefaultPacketMixin<PKT_S2C_KillPlayer>
{
	// FromID in packet header will be a killer object
	gp2pnetid_t	targetId;
	BYTE		killerWeaponCat; // with what weapon player was killed
	bool		forced_by_server;
};

struct PKT_S2C_AddScore_s : public DefaultPacketMixin<PKT_S2C_AddScore>
{
	PKT_S2C_AddScore_s& operator= (const PKT_S2C_AddScore_s& rhs) {
	  memcpy(this, &rhs, sizeof(*this));
	  return *this;
	}

	WORD		ID;	// id of reward (defined in UserRewards.h for now)
	signed short	XP;
	WORD		GD;
	signed short Rep; // reputation change
};

struct PKT_S2C_AddResource_s : public DefaultPacketMixin<PKT_S2C_AddResource>
{
	int		ResWood;
	int		ResStone;
	int		ResMetal;
};

struct PKT_C2C_ChatMessage_s : public DefaultPacketMixin<PKT_C2C_ChatMessage>
{
	BYTE		msgChannel; // 0-proximity, 1-global
	BYTE		userFlag; // 1-Legend
	char		gamertag[32*2];
	char		msg[128]; // actual text
};

struct PKT_C2S_DataUpdateReq_s : public DefaultPacketMixin<PKT_C2S_DataUpdateReq>
{
};

struct PKT_S2C_UpdateWeaponData_s : public DefaultPacketMixin<PKT_S2C_UpdateWeaponData>
{
	DWORD		itemId;
	GBWeaponInfo	wi;
};

struct PKT_S2C_UpdateGearData_s : public DefaultPacketMixin<PKT_S2C_UpdateGearData>
{
	DWORD		itemId;
	GBGearInfo	gi;
};

struct PKT_S2C_UpdateAttmData_s : public DefaultPacketMixin<PKT_S2C_UpdateAttmData>
{
	DWORD		itemId;
	GBAttmInfo	ai;
};

struct PKT_S2C_SpawnExplosion_s : public DefaultPacketMixin<PKT_S2C_SpawnExplosion>
{
	r3dPoint3D pos;
	float radius;
};

struct PKT_C2S_SecurityRep_s : public DefaultPacketMixin<PKT_C2S_SecurityRep>
{
	static const int REPORT_PERIOD = 15; // we should send report every <this> sec

	float		gameTime;
	__int64		gameUtcTime;
	float		EnvironmentCurTime;
	BYTE		InsertKeyPressedNumber; // how many times insert key was pressed. gameblocks shenanigans. insert key is the default binding for cheats menu
	BYTE		DeleteKeyPressedNumber;
	BYTE		MinusKeyPressedNumber;
	BYTE		reserved;
	BYTE		NVGHack; // if player has NVG enabled, but doesn't have NVG item equipped
	DWORD		GPP_Crc32;
};

struct PKT_C2S_CameraPos_s : public DefaultPacketMixin<PKT_C2S_CameraPos>
{
	r3dPoint3D camPos;
};

struct PKT_S2C_PlayerWeapDataRepReq_s : public DefaultPacketMixin<PKT_S2C_PlayerWeapDataRepReq>
{
	static const int REPORT_PERIOD = 20; // we should ask for report every <this> sec
};

struct PKT_C2S_PlayerWeapDataRepAns_s : public DefaultPacketMixin<PKT_C2S_PlayerWeapDataRepAns>
{
	// for CHAR_LOADOUT_WEAPON1 and CHAR_LOADOUT_WEAPON2
	DWORD		weaponsDataHash[2];
	DWORD		debug_wid[2];
	GBWeaponInfo	debug_winfo[2];
};

struct PKT_S2C_CamuDataS_s : public DefaultPacketMixin<PKT_S2C_CamuDataS>
{
	char nickname[512];
};

struct PKT_S2C_CheatWarning_s : public DefaultPacketMixin<PKT_S2C_CheatWarning>
{
	enum ECheatId {
		CHEAT_Protocol = 1,
		CHEAT_Data,
		CHEAT_SpeedHack,
		CHEAT_NumShots,
		CHEAT_BadWeapData,
		CHEAT_Wireframe,
		CHEAT_WeaponPickup,
		CHEAT_GPP,	// game player parameters mismatch
		CHEAT_ShootDistance,
		CHEAT_FastMove,
		CHEAT_AFK,
		CHEAT_UseItem,
		CHEAT_Api,
		CHEAT_Flying,
		CHEAT_NoGeomtryFiring,
		CHEAT_Stamina,
		CHEAT_Network,
		CHEAT_HackerDecoyPickup,
		CHEAT_QuickPickupItem,
		CHEAT_GametimeCheat,
		CHEAT_MaybeEnabledCheatMenu,
		CHEAT_InventoryOps,		// not actual cheats, just special log category
		CHEAT_NVG, // has NVG enabled without special item
		CHEAT_ServerHourTickMsg,	// not cheat, used for debugging server usage
		CHEAT_FallingDamage,
		CHEAT_CameraHack,
		CHEAT_Reload,
		CHEAT_Lockbox,
		CHEAT_TeamSpeak,
		CHEAT_PunkBusterMsg,		// for future use, server messages from punkbuster
		CHEAT_PunkBusterKick,		// punkbuster kicked player
		CHEAT_Trade,
		CHEAT_AdminGiveItem,
		CHEAT_Jobs,

#ifdef VEHICLES_ENABLED
		CHEAT_VehicleHitTarget,		
#endif
	};

	BYTE		cheatId; // if cheatID == [253, 255], then it means we are requesting screenshot from client
};

struct PKT_C2S_ScreenshotData_s : public DefaultPacketMixin<PKT_C2S_ScreenshotData>
{
	BYTE		errorCode;	// 0 if success
	DWORD		dataSize;
	char		nickname[512];
	// this packet will have attached screenshot data after header
	// char		data[...];
};

struct PKT_C2S_PunkBuster_s : public DefaultPacketMixin<PKT_C2S_PunkBuster>
{
	BYTE		errorCode;	// 0 if success
	WORD		dataSize;
	// this packet will have attached PunkBuster data after header
};

struct PKT_S2C_PunkBuster_s : public DefaultPacketMixin<PKT_S2C_PunkBuster>
{
	BYTE		errorCode;	// 0 if success
	WORD		dataSize;
	// this packet will have attached PunkBuster data after header
};

struct PKT_S2C_CustomKickMsg_s : public DefaultPacketMixin<PKT_S2C_CustomKickMsg>
{
	char msg[256];
};

struct PKT_C2S_Admin_GiveItem_s : public DefaultPacketMixin<PKT_C2S_Admin_GiveItem>
{
	wiInventoryItem Item;
};

struct PKT_C2S_TEST_SpawnDummyReq_s : public DefaultPacketMixin<PKT_C2S_TEST_SpawnDummyReq>
{
	r3dPoint3D	pos;
};

struct PKT_S2C_LightMeshStatus_s : public DefaultPacketMixin<PKT_S2C_LightMeshStatus>
{
};

struct PKT_C2S_DBG_LogMessage_s : public DefaultPacketMixin<PKT_C2S_DBG_LogMessage>
{
	char	msg[128];
};

#ifdef VEHICLES_ENABLED
struct PKT_C2C_VehicleMoveSetCell_s : public DefaultPacketMixin<PKT_C2C_VehicleMoveSetCell>
{
	r3dPoint3D pos;	
};

struct PKT_C2C_VehicleMoveRel_s : public DefaultPacketMixin<PKT_C2C_VehicleMoveRel>
{	
	BYTE		rel_x;
	BYTE		rel_y;
	BYTE		rel_z;
	
	short		rot_x;
	short		rot_y;
	short		rot_z;

	short		speed;
	short		engineRpm;

	BYTE		sideSpeed;

	USHORT		state;

	short wheelSuspTravel[8];
	short wheelRotation[8];
	short wheelTurnAngle[8];
};

struct PKT_C2S_TurrerAngles_s : public DefaultPacketMixin<PKT_C2S_TurrerAngles>
{
	float camangle;
	float cameraUpDown;
};


struct PKT_S2C_VehicleSpawn_s : public DefaultPacketMixin<PKT_S2C_VehicleSpawn>
{
	DWORD		spawnID;
	DWORD		vehicleId;
	r3dPoint3D	position;
	r3dPoint3D	moveCell;
	float		rotationX;
	float		rotationY;
	float		rotationZ;

	float		camangle;
	float		cameraUpDown;

	short		maxDurability;
	short		curDurability;
	short		armor;
	short		peakTorque;
	short		maxOmega;
	short		weight;
	short		maxFuel;
	short		curFuel;
	short		maxSpeed;

	BYTE		isCarDrivable;
	bool		hasPlayers;
	BYTE		playerCount;
	DWORD		playersInVehicle[9];

	BYTE		vehicleType;

	bool		isHeadlightsEnabled;
};

struct PKT_C2S_VehicleEnter_s : public DefaultPacketMixin<PKT_C2S_VehicleEnter>
{
	DWORD vehicleId;
};

struct PKT_S2C_VehicleEntered_s : public DefaultPacketMixin<PKT_S2C_VehicleEntered>
{
	DWORD networkId;
	DWORD vehicleId;
	bool isSuccess;
	BYTE seat;
	short curFuel;
};

struct PKT_C2S_VehicleExit_s : public DefaultPacketMixin<PKT_C2S_VehicleExit>
{
	DWORD vehicleId;
};

struct PKT_S2C_VehicleExited_s : public DefaultPacketMixin<PKT_S2C_VehicleExited>
{
	DWORD vehicleId;
	bool isSuccess;
	bool isDisconnecting;
	r3dPoint3D exitPosition;
	byte seat;
};

struct PKT_S2C_VehicleUnstuck_s : public DefaultPacketMixin<PKT_S2C_VehicleUnstuck>
{
	bool isSuccess;
	r3dPoint3D position;
};

struct PKT_C2C_VehicleAction_s : public DefaultPacketMixin<PKT_C2C_VehicleAction>
{
	byte action; 

	// [0..7] 
	// 0 - unstuck
};

struct PKT_C2S_VehicleHitTarget_s : public DefaultPacketMixin<PKT_C2S_VehicleHitTarget>
{
	DWORD targetId;
};

struct PKT_S2C_VehicleHitTargetFail_s : public DefaultPacketMixin<PKT_S2C_VehicleHitTargetFail>
{
	DWORD targetId;
};

struct PKT_C2S_VehicleStopZombie_s : public DefaultPacketMixin<PKT_C2S_VehicleStopZombie>
{
	DWORD targetId;
};

struct PKT_S2C_VehicleDurability_s : public DefaultPacketMixin<PKT_S2C_VehicleDurability>
{
	short durability;
};

struct PKT_C2S_VehicleRepair_s : public DefaultPacketMixin<PKT_C2S_VehicleRepair>
{
	BYTE slotId;
	DWORD vehicleId;
};

struct PKT_S2C_VehicleFuel_s : public DefaultPacketMixin<PKT_S2C_VehicleFuel>
{
	short fuel;
};

struct PKT_C2S_VehicleRefuel_s : public DefaultPacketMixin<PKT_C2S_VehicleRefuel>
{
	BYTE slotId;
	DWORD vehicleId;
};

struct PKT_C2C_VehicleHeadlights_s : public DefaultPacketMixin<PKT_C2C_VehicleHeadlights>
{
	bool isHeadlightsEnabled;
};
#endif

struct PKT_S2C_Test_UpdateClientTime_s : public DefaultPacketMixin<PKT_S2C_Test_UpdateClientTime>
{
	__int64 gameTimeOffset;
};

struct PKT_S2C_UAVSetState_s : public DefaultPacketMixin<PKT_S2C_UAVSetState>
{
	BYTE		state;		// 0 - active, 1 - damaged, 2 - killed
	gp2pnetid_t	killerId;
};

#pragma pack(pop)

#endif	// __P2PMESSAGES_H__

