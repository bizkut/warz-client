#include "r3dpch.h"
#include "r3d.h"

#include "APIScaleformGfx.h"
#include "AI_Player.h"
#include "AI_PlayerAnim.h"

#include "r3dPhysSkeleton.h"

#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "multiplayer/MasterServerLogic.h"

#include "Particle.h"

#include "ObjectsCode/world/DecalChief.h"
#include "ObjectsCode/world/MaterialTypes.h"

#include "ObjectsCode/weapons/Weapon.h"

#include "ObjectsCode/weapons/Gear.h"
#include "ObjectsCode/WEAPONS/Ammo.h"
#include "ObjectsCode/weapons/Grenade.h"
#include "ObjectsCode/weapons/WeaponArmory.h"
#include "ObjectsCode/weapons/ExplosionVisualController.h"

#include "ObjectsCode/Gameplay/obj_PlayerSpawnPoint.h"
#include "ObjectsCode/Gameplay/obj_DamageArea.h"
#include "ObjectsCode/Gameplay/obj_DroppedItem.h"
#include "ObjectsCode/Gameplay/obj_Note.h"
#include "ObjectsCode/Gameplay/obj_PostBox.h"
#include "ObjectsCode/Gameplay/obj_StoreNPC.h"
#include "ObjectsCode/Gameplay/obj_Gravestone.h"
#include "ObjectsCode/Gameplay/obj_Door.h"
#include "ObjectsCode/Weapons/FarmBlock.h"
#include "ObjectsCode/Gameplay/obj_Traps.h"
#include "ObjectsCode/Weapons/Barricade.h"
#include "../../GameEngine/gameobjects/obj_Vehicle.h"
#include "../Gameplay/obj_Zombie.h"
#include "../Gameplay/obj_ZombieDummy.h"
#include "../Gameplay/obj_AnimalDummy.h"
#include "ObjectsCode/Gameplay/obj_UAV.h"
#ifdef MISSIONS
#include "../Gameplay/obj_MissionStateObject.h"
#endif

#include "ObjectsCode/WORLD/obj_LightMesh.h"

#include "multiplayer/ClientGameLogic.h"
#include "GameCode/UserProfile.h"
#include "GameCode/UserRewards.h"

#include "Gameplay_Params.h"

#include "CamouflageDataManager.h"

#include "../SF/CmdProcessor/CmdProcessor.h"
#include "../SF/CmdProcessor/CmdConsole.h"

#include "..\..\ui\HUDDisplay.h"
#include "..\..\ui\HUD_EditorGame.h"
#include "..\..\ui\FrontEndShared.h"
#include "..\..\ui\HUDPause.h"
#include "..\..\ui\HUDVault.h"
#include "..\..\ui\HUDSafelock.h"
#include "..\..\ui\HUDTrade.h"
#include "..\..\ui\HUDStore.h"
#include "..\..\ui\HUDActionUI.h"
#include "..\..\ui\HUDAttachments.h"
#include "..\..\ui\HUDRepair.h"
#include "..\..\ui\HUDCrafting.h"

extern HUDDisplay*	hudMain;
extern HUDPause*	hudPause;
extern HUDVault*	hudVault;
extern HUDSafelock*	hudSafelock;
extern HUDTrade*	hudTrade;
extern HUDStore*	hudStore;
extern HUDAttachments*	hudAttm;
extern HUDActionUI*	hudActionUI;
extern HUDRepair*	hudRepair;
extern HUDCrafting* hudCraft;

extern 	PlayerStateVars_s CurrentRig;

extern bool g_bEditMode;

extern void TPSGameHud_UnlockAchievement( int achievementID );

extern float getWaterDepthAtPos(const r3dPoint3D& pos);

IMPLEMENT_CLASS(obj_Player, "obj_Player", "Object");
AUTOREGISTER_CLASS(obj_Player);

float		_ai_fTurnSpeedIdle      = 360;
float		_ai_fTurnSpeedWalk	= 720;
float		_ai_NetPosUpdateDelta = 1.0f / 10;	// "30"

bool		g_CameraInsidePlayer = false;

#include "../../rendering/Deffered/PFX_ScopeEffect.h"
extern int g_RenderScopeEffect;
extern PFX_ScopeEffect gPFX_ScopeEffect;

r3dSec_type<CUberData*, 0xA45AdFCa> AI_Player_UberData = NULL;

r3dTexture* obj_Player::s_laserPointerTex = NULL;
r3dTexture* obj_Player::s_flashlightFlashTex = NULL;
r3dTexture* obj_Player::s_laserFlashTex = NULL;
r3dTexture* obj_Player::s_lightHaloTex = NULL;

r3dTexture* obj_Player::s_UAVFriendlyBlinkTex = NULL;

r3dTexture* gPlayer_AuraTexture;
r3dTexture* gPlayer_CamoTexture;

gobjid_t m_LocalPlayer_CurrentAimAt;
float	 m_LocalPlayer_CurrentAimAt_StartTime;

#define TIME_FOR_SECOND_WIND 5.0f
#define KILLSTREAK_FOR_SECOND_WIND 3
static const float gDist_To_Allow_Vehicle_Entry = 5;
bool gDisableShortcuts = false;
struct PhysSkeletonCacheEntry
{
	int Taken ;
	r3dPhysSkeleton* Skel ;
};

///////////////////////////////////////////////////////////////////////////////

ThreatStats::ThreatStats()
	: visualSum( 0.0f )
	, audibleSum( 0.0f )
	, interceptSum( 0.0f )
	, nearbySum( 0.0f )
	, closestZombie( FLT_MAX )
	, zombieNearbyCount( 0 )
	, zombieInterceptCount( 0 )
	, inSightRangeCount( 0 )
	, inAudioRangeCount( 0 )
	, weightedClosest( 0.0f )
	, weightedNearby( 0.0f )
	, weightedIntercept( 0.0f )
	, weightedVisible( 0.0f )
	, weightedAudible( 0.0f )
	, threat( 0.0f )
	, timer( 0.0f )
#ifndef FINAL_BUILD
	, plrVisibilitySq( 0.0f )
	, plrAudibilitySq( 0.0f )
	, plrMaxPerceptionSq( 0.0f )
	, plrMaxPerception( 0.0f )
	, plrPosition( 0.0f, 0.0f, 0.0f )
	, plrVelocity( 0.0f, 0.0f, 0.0f )
	, plrFuturePos( 0.0f, 0.0f, 0.0f )
	, zmbPosition( 0.0f, 0.0f, 0.0f )
	, zmbVector( 0.0f, 0.0f, 0.0f )
#endif
{ }

// Clear only clears the initial values, not the calculated values, use Reset for that.
void ThreatStats::Clear()
{
	visualSum = 0.0f;
	audibleSum = 0.0f;
	interceptSum = 0.0f;
	nearbySum = 0.0f;
	closestZombie = FLT_MAX;
	zombieNearbyCount = 0;
	zombieInterceptCount = 0;
	inSightRangeCount = 0;
	inAudioRangeCount = 0;
}

void ThreatStats::Reset()
{
	Clear();
	weightedClosest = 0.0f;
	weightedNearby = 0.0f;
	weightedIntercept = 0.0f;
	weightedVisible = 0.0f;
	weightedAudible = 0.0f;
	threat = 0.0f;
	timer = 0.0f;
}

ThreatStats	threatStats[2]; // One for the current player position, and one for the future position.

///////////////////////////////////////////////////////////////////////////////


typedef r3dTL::TArray< PhysSkeletonCacheEntry > PhysSkeletonCache ;

int g_bPhysSkeletonCacheInitialized ;
PhysSkeletonCache g_PhysSkeletonCache ;

// per shot
float SpreadIncrease[] = 
{
	0.15f, // ASR
	1.0f, // SNP
	0.5f, // SHTG
	0.45f, // MG
	0.0f, // SUPPORT
	0.5f, // HG
	0.1f, // SMG
	0.0f, // GRENADES
};
// per second
float SpreadDecrease[] = 
{
	4.0f, // ASR
	1.0f, // SNP
	4.0f, // SHTG
	2.0f, // MG
	4.0f, // SUPPORT
	1.0f, // HG
	4.0f, // SMG
	4.0f, // GRENADES
};
// percentage of weapon's spread
float SpreadMin[] = 
{
	.2f, // ASR
	1.0f, // SNP
	.25f, // SHTG
	.2f, // MG
	.1f, // SUPPORT
	.1f, // HG
	.15f, // SMG
	.1f, // GRENADES
};

void AddCachedPhysSkeleton()
{
	PhysSkeletonCacheEntry entry ;

	entry.Skel	= game_new r3dPhysSkeleton( "data/ObjectsDepot/Characters/RagDoll.RepX" ) ;
	entry.Taken	= 0 ;

	g_PhysSkeletonCache.PushBack( entry ) ;
}

void InitializePhysSkeletonCache( int count, float progressStart, float progressEnd )
{
	r3d_assert( !g_bPhysSkeletonCacheInitialized ) ;

	R3D_LOG_TIMESPAN_START(InitializeSkeletonCache);
	for( int i = 0, e = count ; i < e ; i ++ )
	{
		AddCachedPhysSkeleton() ;

		void SetLoadingProgress( float progress ) ;
		SetLoadingProgress( progressStart + ( progressEnd - progressStart ) * i / ( count - 1 ) ) ;
	}
	R3D_LOG_TIMESPAN_END(InitializeSkeletonCache);	

	g_bPhysSkeletonCacheInitialized = 1 ;
}

void DestroyPhysSkeletonCache()
{
	r3d_assert( g_bPhysSkeletonCacheInitialized ) ;

	for( int i = 0, e = g_PhysSkeletonCache.Count() ; i < e ; i ++ )
	{
		PhysSkeletonCacheEntry& entry = g_PhysSkeletonCache[ i ] ;
		delete entry.Skel ;
	}

	g_PhysSkeletonCache.Clear() ;

	g_bPhysSkeletonCacheInitialized = 0 ;
}

void ReleaseCachedPhysSkeleton( r3dPhysSkeleton* skel )
{
	if( skel )
	{
		for( int i = 0, e = g_PhysSkeletonCache.Count() ; i < e ; i ++ )
		{
			PhysSkeletonCacheEntry& entry = g_PhysSkeletonCache[ i ] ;
			if( entry.Skel == skel )
			{
				entry.Skel->unlink() ;
				entry.Skel->SwitchToRagdoll(false);
				entry.Taken = 0 ;
				return ;
			}
		}

		r3dError( "ReleaseCacheSkeleton: skeleton not found!" ) ;
	}
}

r3dPhysSkeleton* AquireCacheSkeleton()
{
	for( int i = 0, e = g_PhysSkeletonCache.Count() ; i < e ; i ++ )
	{
		PhysSkeletonCacheEntry& entry = g_PhysSkeletonCache[ i ] ;

		if( !entry.Taken)
		{
			entry.Taken = 1 ;
			return entry.Skel ;
		}
	}

	AddCachedPhysSkeleton() ;

	PhysSkeletonCacheEntry& entry = g_PhysSkeletonCache.GetLast() ;

	entry.Taken = 1 ;

	return entry.Skel ;
}


struct PhysObstacleCacheEntry
{
	int Taken;
	ObstacleHandle Handle;
	PxBoxObstacle Obstacle;
};
PhysObstacleCacheEntry*	g_PhysObstacleCache = NULL;
int		g_PhysObstacleSize  = 0;
struct PhysObstacleCacheEntryCapsule
{
	int Taken;
	ObstacleHandle Handle;
	PxCapsuleObstacle Obstacle;
};
PhysObstacleCacheEntryCapsule*	g_PhysObstacleCacheCapsule = NULL;
int		g_PhysObstacleSizeCapsule  = 0;

void InitializePhysObstacleCache(int count, int capsuleCount)
{
	r3d_assert(g_PhysObstacleCache == NULL) ;
	r3d_assert(g_PhysObstacleCacheCapsule == NULL) ;
	
	g_PhysObstacleCache = game_new PhysObstacleCacheEntry[count];
	g_PhysObstacleSize  = count;

	g_PhysObstacleCacheCapsule = game_new PhysObstacleCacheEntryCapsule[capsuleCount];
	g_PhysObstacleSizeCapsule  = capsuleCount;

	for(int i=0; i<count; i++)
	{
		PxBoxObstacle obstacle;
		obstacle.mHalfExtents = PxVec3(0.25f, 1.0f, 0.25f);
		obstacle.mPos = PxExtendedVec3(0, 9000 + (float)i * 10, 0); // teleport way up

		g_PhysObstacleCache[i].Obstacle = obstacle;
		g_PhysObstacleCache[i].Handle = g_pPhysicsWorld->m_PlayerObstaclesManager->addObstacle(obstacle);
		g_PhysObstacleCache[i].Taken  = 0;
	}

	for(int i=0; i<capsuleCount; i++)
	{
		PxCapsuleObstacle obstacle;
		obstacle.mHalfHeight = 1.0f;
		obstacle.mRadius = 0.25f;
		obstacle.mPos = PxExtendedVec3(0, 9000 + (float)i * 10, 0); // teleport way up

		g_PhysObstacleCacheCapsule[i].Obstacle = obstacle;
		g_PhysObstacleCacheCapsule[i].Handle = g_pPhysicsWorld->m_PlayerObstaclesManager->addObstacle(obstacle);
		g_PhysObstacleCacheCapsule[i].Taken  = 0;
	}
}

void DestroyPhysObstacleCache()
{
	r3d_assert(g_PhysObstacleCache);
	r3d_assert(g_PhysObstacleCacheCapsule);

	// destroy in reverse order 
	for(int i=g_PhysObstacleSize-1; i>=0; i--)
	{
		g_pPhysicsWorld->m_PlayerObstaclesManager->removeObstacle(g_PhysObstacleCache[i].Handle);
		g_PhysObstacleCache[i].Handle = 0;
		g_PhysObstacleCache[i].Taken  = 0;
	}
	
	SAFE_DELETE_ARRAY(g_PhysObstacleCache);
	g_PhysObstacleSize = 0;

	// destroy in reverse order 
	for(int i=g_PhysObstacleSizeCapsule-1; i>=0; i--)
	{
		g_pPhysicsWorld->m_PlayerObstaclesManager->removeObstacle(g_PhysObstacleCacheCapsule[i].Handle);
		g_PhysObstacleCacheCapsule[i].Handle = 0;
		g_PhysObstacleCacheCapsule[i].Taken  = 0;
	}

	SAFE_DELETE_ARRAY(g_PhysObstacleCacheCapsule);
	g_PhysObstacleSizeCapsule = 0;
}

int AcquirePlayerObstacle(const r3dPoint3D& pos, const PxBoxObstacle& obstacle)
{
	r3d_assert(g_PhysObstacleCache);

	for(int i=0; i<g_PhysObstacleSize; i++)
	{
		if(g_PhysObstacleCache[i].Taken)
			continue;
			
		g_PhysObstacleCache[i].Taken = 1;
		g_PhysObstacleCache[i].Obstacle = obstacle;

		UpdatePlayerObstacle(i, pos);
		return i;
	}
	
	return -1;
}

void ReleasePlayerObstacle(int* out_idx)
{
	int idx = *out_idx;
	if(idx == -1)
		return;

	r3d_assert(idx >= 0 && idx < g_PhysObstacleSize);
	r3d_assert(g_PhysObstacleCache[idx].Taken);

	UpdatePlayerObstacle(idx, r3dPoint3D(0, 9000 + (float)idx * 10, 0)); // teleport way up
	g_PhysObstacleCache[idx].Taken = 0;
	*out_idx = -1;
}

void UpdatePlayerObstacle(int idx, const r3dPoint3D& pos)
{
	if(idx == -1)
		return;

	R3DPROFILE_FUNCTION("UpdateObstacle");
	
	r3d_assert(idx >= 0 && idx < g_PhysObstacleSize);
	r3d_assert(g_PhysObstacleCache[idx].Taken);
	
	//PxBoxObstacle obstacle;
	//obstacle.mHalfExtents = PxVec3(0.25f, 1.0f, 0.25f);
	//obstacle.mPos = PxExtendedVec3(pos.x, pos.y + 1.0f, pos.z);

	g_PhysObstacleCache[idx].Obstacle.mPos = PxExtendedVec3(pos.x, pos.y + 1.0f, pos.z);

	g_pPhysicsWorld->m_PlayerObstaclesManager->updateObstacle(g_PhysObstacleCache[idx].Handle, g_PhysObstacleCache[idx].Obstacle);

	return;
}

void UpdatePlayerObstacle(int idx, const r3dPoint3D& pos, const PxQuat& rot)
{
	if(idx == -1)
		return;

	R3DPROFILE_FUNCTION("UpdateObstacle");

	r3d_assert(idx >= 0 && idx < g_PhysObstacleSize);
	r3d_assert(g_PhysObstacleCache[idx].Taken);

	//PxBoxObstacle obstacle;
	//obstacle.mHalfExtents = PxVec3(0.25f, 1.0f, 0.25f);
	//obstacle.mPos = PxExtendedVec3(pos.x, pos.y + 1.0f, pos.z);

	g_PhysObstacleCache[idx].Obstacle.mPos = PxExtendedVec3(pos.x, pos.y + 1.0f, pos.z);
	g_PhysObstacleCache[idx].Obstacle.mRot = rot;

	g_pPhysicsWorld->m_PlayerObstaclesManager->updateObstacle(g_PhysObstacleCache[idx].Handle, g_PhysObstacleCache[idx].Obstacle);

	return;
}

int AcquirePlayerObstacleCapsule(const r3dPoint3D& pos, const PxCapsuleObstacle& obstacle)
{
	r3d_assert(g_PhysObstacleCacheCapsule);

	for(int i=0; i<g_PhysObstacleSizeCapsule; i++)
	{
		if(g_PhysObstacleCacheCapsule[i].Taken)
			continue;

		g_PhysObstacleCacheCapsule[i].Taken = 1;
		g_PhysObstacleCacheCapsule[i].Obstacle = obstacle;

		UpdatePlayerObstacleCapsule(i, pos);
		return i;
	}

	return -1;
}

void ReleasePlayerObstacleCapsule(int* out_idx)
{
	int idx = *out_idx;
	if(idx == -1)
		return;

	r3d_assert(idx >= 0 && idx < g_PhysObstacleSizeCapsule);
	r3d_assert(g_PhysObstacleCacheCapsule[idx].Taken);

	UpdatePlayerObstacleCapsule(idx, r3dPoint3D(0, 9000 + (float)idx * 10, 0)); // teleport way up
	g_PhysObstacleCacheCapsule[idx].Taken = 0;
	*out_idx = -1;
}

void UpdatePlayerObstacleCapsule(int idx, const r3dPoint3D& pos)
{
	if(idx == -1)
		return;

	R3DPROFILE_FUNCTION("UpdateObstacleCapsule");

	r3d_assert(idx >= 0 && idx < g_PhysObstacleSizeCapsule);
	r3d_assert(g_PhysObstacleCacheCapsule[idx].Taken);

	//PxBoxObstacle obstacle;
	//obstacle.mHalfExtents = PxVec3(0.25f, 1.0f, 0.25f);
	//obstacle.mPos = PxExtendedVec3(pos.x, pos.y + 1.0f, pos.z);

	g_PhysObstacleCacheCapsule[idx].Obstacle.mPos = PxExtendedVec3(pos.x, pos.y + 1.0f, pos.z);

	g_pPhysicsWorld->m_PlayerObstaclesManager->updateObstacle(g_PhysObstacleCacheCapsule[idx].Handle, g_PhysObstacleCacheCapsule[idx].Obstacle);

	return;
}

static void AdjustHitPosition(DecalParams& params, GameObject* target);

void AI_Player_FreeStuff()
{
	SAFE_DELETE(AI_Player_UberData);

	if(gPlayer_AuraTexture)
	{
		r3dRenderer->DeleteTexture(gPlayer_AuraTexture);

		gPlayer_AuraTexture = 0;
	}

	if(gPlayer_CamoTexture)
	{
		r3dRenderer->DeleteTexture(gPlayer_CamoTexture);

		gPlayer_CamoTexture = 0;
	}
}

#ifndef FINAL_BUILD
DECLARE_CMD(binv)
{
	if(ev.NumArgs() != 1)
	{
		ConPrint("binv");
		return;
	}

	obj_Player* plr = gClientLogic().localPlayer_;
	wiCharDataFull& slot = plr->CurLoadout;

	ConPrint("Backpack id:%d, size:%d\n", slot.BackpackID, slot.BackpackSize);
	ConPrint("Inventory List:\n");
	for(int i=0; i<slot.BackpackSize; i++)
	{
		wiInventoryItem& wi = slot.Items[i];
		if(wi.itemID == 0)
			continue;
		const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(wi.itemID);
		ConPrint("%02d: ItemID:%d, q:%d, Var:%d %d %d, %s, InvId:%I64d", i, wi.itemID, wi.quantity, wi.Var1, wi.Var2, wi.Var3, cfg->m_StoreName, wi.InventoryID);
	}
	
	return;
}

DECLARE_CMD(giveitem)
{
	if(ev.NumArgs() < 2)
	{
		ConPrint("giveitem {itemid} [num] [var1]");
		return;
	}

	wiInventoryItem itm;
	itm.itemID   = ev.GetInteger(1);
	itm.quantity = ev.NumArgs() >= 3 ? ev.GetInteger(2) : 1;
	itm.Var1     = ev.NumArgs() >= 4 ? ev.GetInteger(3) : -1;
	itm.Var2     = ev.NumArgs() >= 5 ? ev.GetInteger(4) : -1;
	itm.Var3     = ev.NumArgs() >= 6 ? ev.GetInteger(5) : wiInventoryItem::MAX_DURABILITY;

	PKT_C2S_Admin_GiveItem_s n;
	n.Item = itm;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

DECLARE_CMD(alist)
{
	if(ev.NumArgs() != 1)
	{
		ConPrint("alist {slot}");
		return;
	}

	obj_Player* plr = gClientLogic().localPlayer_;
	wiCharDataFull& slot = plr->CurLoadout;
	
	int wid      = 0; // test - by default modify weapon 0
	Weapon* wpn = plr->m_Weapons[wid];
	if(!wpn) {
		ConPrint("no weapon");
		return;
	}
	
	ConPrint("Weapon: %s\n", wpn->getConfig()->m_StoreName);
	for(int i=0; i<WPN_ATTM_MAX; i++)
	{
		ConPrint(" Slot: %d, cur %d, def %d\n", i, slot.Attachment[wid].attachments[i], wpn->getConfig()->FPSDefaultID[i]);
		if(!wpn->getConfig()->FPSSpecID[i])
			continue;
		
		char buf[2084] = "  ";
		g_pWeaponArmory->startItemSearch();
		while(g_pWeaponArmory->searchNextItem())
		{
			uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
			const WeaponAttachmentConfig* config = g_pWeaponArmory->getAttachmentConfig(itemID);
			if(!config) continue;
			if(config->m_specID == wpn->getConfig()->FPSSpecID[i])
				sprintf(buf + strlen(buf), ", %d %s", config->m_itemID, config->m_StoreName);
			if(config->m_itemID == wpn->getConfig()->FPSDefaultID[i])
				sprintf(buf + strlen(buf), ", (def) %d %s", config->m_itemID, config->m_StoreName);
		}
		ConPrint(buf);
	}
}

DECLARE_CMD(slist)
{
	if(ev.NumArgs() != 1)
	{
		ConPrint("slist");
		return;
	}
	
	for(uint32_t i=0; i<g_NumStoreItems; i++)
	{
		const BaseItemConfig* ic = g_pWeaponArmory->getConfig(g_StoreItems[i].itemID);
		ConPrint("%d: %d cash %d gold - %s\n", g_StoreItems[i].itemID, g_StoreItems[i].pricePerm, g_StoreItems[i].gd_pricePerm, ic ? ic->m_StoreName : "NONE");
	}
}

DECLARE_CMD(sbuy)
{
	if(ev.NumArgs() != 3)
	{
		ConPrint("sbuy {itemid} {method - 4:cash, 8:gold");
		return;
	}

	PKT_C2S_ShopBuyReq_s n;
	n.ItemID = ev.GetInteger(1);
	n.BuyIdx = ev.GetInteger(2);
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	return;
}

DECLARE_CMD(invlist)
{
	if(ev.NumArgs() != 1)
	{
		ConPrint("invlist");
		return;
	}
	
	for(uint32_t i=0; i<gUserProfile.ProfileData.NumItems; i++)
	{
		wiInventoryItem& wi = gUserProfile.ProfileData.Inventory[i];
		const BaseItemConfig* ic = g_pWeaponArmory->getConfig(wi.itemID);
		if(wi.itemID)
			ConPrint("slot:%2d %I64d %dx%d - %s\n", i, wi.InventoryID, wi.itemID, wi.quantity, ic ? ic->m_StoreName : "NONE" );
	}
}

DECLARE_CMD(toinv)
{
	if(ev.NumArgs() != 2)
	{
		ConPrint("toinv slot");
		return;
	}
	
	obj_Player* plr = gClientLogic().localPlayer_;
	
	PKT_C2S_ToInventoryReq_s n;
	n.SlotFrom     = ev.GetInteger(1);
	n.Amount       = 1; //ev.GetInteger(2);
	n.dbg_ItemID   = plr->CurLoadout.Items[n.SlotFrom].itemID;
	n.dbg_Quantity = plr->CurLoadout.Items[n.SlotFrom].quantity;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

DECLARE_CMD(frominv)
{
	if(ev.NumArgs() != 3)
	{
		ConPrint("frominv inventorySlot backpackSlot(or -1)");
		return;
	}
	
	obj_Player* plr = gClientLogic().localPlayer_;
	int InvSlot = ev.GetInteger(1);
	int SlotTo  = ev.GetInteger(2);
	
	PKT_C2S_FromInventoryReq_s n;
	n.InventoryID     = gUserProfile.ProfileData.Inventory[InvSlot].InventoryID;
	n.SlotTo          = SlotTo == -1 ? 0xFF : SlotTo;		// or 0xFF if server should decide it
	n.Amount          = 1;
	if(n.SlotTo != 0xFF)
	{
	n.dbg_ItemID      = plr->CurLoadout.Items[n.SlotTo].itemID;
	n.dbg_Quantity    = plr->CurLoadout.Items[n.SlotTo].itemID;
	}
	else
	{
	n.dbg_ItemID      = 0;
	n.dbg_Quantity    = 0;
	}
	n.dbg_InvItemID   = gUserProfile.ProfileData.Inventory[InvSlot].itemID;
	n.dbg_InvQuantity = gUserProfile.ProfileData.Inventory[InvSlot].quantity;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

#endif

bool obj_Player::ChangeBackpack(int slotFrom)
{
	wiCharDataFull& slot = CurLoadout;

	const BackpackConfig* cfg = g_pWeaponArmory->getBackpackConfig(slot.Items[slotFrom].itemID);
	if(!cfg) {
		return false;
	}

	// do not allow to change backpack if new backpack has less slots then current one (to prevent hack when player can drop 300+ chemlights and cause other clients to freeze
	{
		int numItemsCurrently = 0;
		for (int a = 0; a < slot.BackpackSize; a++)
		{	
			if (slot.Items[a].itemID != 0)
				numItemsCurrently++;
		}

		if(numItemsCurrently > cfg->m_maxSlots)
		{
			return false;
		}
	}

	PKT_C2S_PlayerChangeBackpack_s n;
	n.SlotFrom     = slotFrom;
	n.BackpackSize = cfg->m_maxSlots;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));

	// replace backpack in used slot with current one (SERVER CODE SYNC POINT)
	slot.Items[slotFrom].itemID = slot.BackpackID;

	// change it
	slot.BackpackID   = cfg->m_itemID;
	slot.BackpackSize = cfg->m_maxSlots;
	UpdateLoadoutSlot(slot, slotFrom);

	return true;
}

void obj_Player::DropItem(int slotFrom)
{
	int itemID = CurLoadout.Items[slotFrom].itemID;

	float DropitemDistance = 0.1f;

	if (itemID == WeaponConfig::ITEMID_ConstructorWODBOX)
		DropitemDistance = 2.0f;

	PKT_C2S_BackpackDrop_s n;
	n.SlotFrom = slotFrom;
	n.pos      = GetPosition() + GetvForw()*DropitemDistance;
	p2pSendToHost(this, &n, sizeof(n));

	

	//local logic
	CurLoadout.Items[slotFrom].quantity--;
	if(CurLoadout.Items[slotFrom].quantity <= 0) {
		CurLoadout.Items[slotFrom].Reset();
	}

	// fucked up attachment design!
	const WeaponAttachmentConfig* wac = g_pWeaponArmory->getAttachmentConfig(itemID);
	if(wac)
	{
		int quantity = CurLoadout.Items[slotFrom].quantity; // if attm is droppen, then here will be zero
		for(int i=0; i<CurLoadout.BackpackSize; ++i)
		{
			if(CurLoadout.Items[i].itemID == itemID)
				quantity += CurLoadout.Items[i].quantity;
		}
		if(quantity == 0) // check if that attm is equipped and if yes, remove it from weapon
		{
			uint32_t curAttm[WPN_ATTM_MAX] = {0};
			if(m_Weapons[0])
			{
				m_Weapons[0]->getCurrentAttachmentIDs(curAttm);
				if(curAttm[wac->m_type] == itemID)
					RemoveWpnAttm(0, wac->m_type);
			}
			if(m_Weapons[1])
			{
				m_Weapons[1]->getCurrentAttachmentIDs(curAttm);
				if(curAttm[wac->m_type] == itemID)
					RemoveWpnAttm(1, wac->m_type);
			}			
		}
	}

	OnBackpackChanged(slotFrom);

	if(NetworkLocal)
	{
		const WeaponConfig* wc = g_pWeaponArmory->getWeaponConfig(itemID);
		if(wc)
			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_DROPGUN"), GetPosition());
		else
			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_DROPITEM"), GetPosition());
	}
}

void obj_Player::RemoveWpnAttm(int wid, int attmType, bool isPacketRequired)
{
	wiCharDataFull& slot = CurLoadout;

	if(attmType >= WPN_ATTM_MAX)  {
		return;
	}

	Weapon* wpn = m_Weapons[wid];
	if(!wpn) {
		return;
	}

	if (isPacketRequired)
	{
		PKT_C2S_PlayerRemoveAttachment_s n;
		n.wid         = wid;
		n.WpnAttmType = attmType;
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	}

	// set wpn attachment
	slot.Attachment[wid].attachments[attmType] = 0;
	wpn->setWeaponAttachmentsByIDs(slot.Attachment[wid].attachments);
}

void obj_Player::EquipWpnAttm(int wid, int backpackSlot)
{
	wiCharDataFull& slot = CurLoadout;

	int slotFrom = backpackSlot;

	const WeaponAttachmentConfig* cfg = g_pWeaponArmory->getAttachmentConfig(slot.Items[slotFrom].itemID);
	if(!cfg) {
		return;
	}

	Weapon* wpn = m_Weapons[wid];
	if(!wpn) {
		return;
	}

	if(cfg->m_type == WPN_ATTM_CLIP && wpn->isReloading())
		return; // we are reloading weapon already, wait until it is done


	PKT_C2S_PlayerEquipAttachment_s n;
	n.wid          = wid;
	n.AttmSlot     = slotFrom;
	n.dbg_AttmID   = cfg->m_itemID;
	n.dbg_WeaponID = wpn->getConfig()->m_itemID;
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));

	// set wpn attachment
	slot.Attachment[wid].attachments[cfg->m_type] = cfg->m_itemID;
	wpn->setWeaponAttachmentsByIDs(slot.Attachment[wid].attachments);

	// we're changing clip, reload weapon
	if(cfg->m_type == WPN_ATTM_CLIP)
	{
		wpn->Reload(slotFrom);
	}
}


void _player_AdjustBoneCallback(DWORD dwData, int boneId, D3DXMATRIX &mp, D3DXMATRIX &anim)
{
	obj_Player* npc = (obj_Player*)dwData;
	if (!npc)
		return;

	if(npc->m_PhysSkeleton && npc->m_PhysSkeleton->IsRagdollMode())
		return;

	if(g_camera_mode->GetInt()==2 && npc->NetworkLocal) // FPS bone adjustments
	{
	}
	else // TPS bone adjustments
	{
		if(boneId == npc->boneId_Bip01_Spine1 || boneId == npc->boneId_Bip01_Neck) 
		{
			float xRot = npc->bodyAdjust_x;
			float yRot = npc->bodyAdjust_y[0];

			// do not bend body in prone position
			if(npc->PlayerState == PLAYER_PRONE_DOWN || npc->PlayerState == PLAYER_PRONE_UP || npc->PlayerState == PLAYER_MOVE_PRONE || npc->PlayerState == PLAYER_PRONE_IDLE || npc->PlayerState == PLAYER_PRONE_AIM)
				yRot = 0;

			if (npc->PlayerState == PLAYER_VEHICLE_DRIVER || npc->PlayerState == PLAYER_VEHICLE_PASSENGER)
				yRot = 0;

			// add recoil for spine1
			if(boneId == npc->boneId_Bip01_Spine1)
			{
				if(!(npc->NetworkLocal && npc->hasScopeMode() && CurrentRig.allowScope)) // don't add recoil in scope mode to a weapon, as it messes up muzzle flash rendering
				{
					float recoil = npc->RecoilViewMod.y*0.5f;
					recoil = R3D_MIN(recoil, 0.0f);

					yRot += recoil;
				}
			}

			D3DXMATRIX r1, r2;

			// rotate previous matrix, with saved position
			r3dPoint3D vv = r3dPoint3D(mp._41, mp._42, mp._43);
			D3DXMatrixRotationY(&r2, xRot/2);
			D3DXMatrixRotationX(&r1, -yRot/2);
			mp = mp * (r1 * r2);
			mp._41 = vv.x;
			mp._42 = vv.y;
			mp._43 = vv.z;
		}
		else if(boneId == npc->boneId_Bip01_LUpperArm || boneId == npc->boneId_Bip01_RUpperArm)
		{
			float xRot = npc->RecoilViewMod.x*0.1f;
			float yRot = npc->RecoilViewMod.y*0.5f;

			xRot = 0;//-R3D_MIN(xRot, 0.5f); 
			yRot = R3D_MIN(yRot, 0.1f);

			D3DXMATRIX r1, r2;

			// rotate previous matrix, with saved position
			r3dPoint3D vv = r3dPoint3D(mp._41, mp._42, mp._43);
			D3DXMatrixRotationY(&r2, xRot/2);
			D3DXMatrixRotationAxis(&r1, npc->GetvRightDraw().d3dx(), -yRot/2);
			mp = mp * (r1 * r2);
			mp._41 = vv.x;
			mp._42 = vv.y;
			mp._43 = vv.z;
		}
	}

	return;    
}

void obj_Player::UpdateLoadoutSlot(const wiCharDataFull& slot, int slotIdxToUpdate)
{
	R3DPROFILE_FUNCTION("obj_Player::UpdateLoadoutSlot");

	CurLoadout = slot; // save loadout
	m_GearWeight = 0;
	m_isFemaleHero = CurLoadout.HeroItemID==20184||CurLoadout.HeroItemID==20194||CurLoadout.HeroItemID==20195;

	for(int i=0; i<NUM_WEAPONS_ON_PLAYER; ++i)
	{
		if(slotIdxToUpdate != -1)
			if(i!=slotIdxToUpdate)
				continue;
		SAFE_DELETE(m_Weapons[i]);
	}

	bool isFirstPerson = g_camera_mode->GetInt()==2 && NetworkLocal;

	// (SERVER CODE SYNC POINT) override to hands weapon if there is no selected weapon
	if(slot.Items[m_SelectedWeapon].itemID == 0)
		m_SelectedWeapon = HANDS_WEAPON_IDX;

	// create weapons & items
	for(int i=0; i<NUM_WEAPONS_ON_PLAYER; i++)
	{
		int ItemID = (i == HANDS_WEAPON_IDX ? WeaponConfig::ITEMID_UnarmedMelee : slot.Items[i].itemID);
		if(ItemID == 0)
			continue;
		if(slotIdxToUpdate != -1)
			if(i!=slotIdxToUpdate)
				continue;
			
		const WeaponConfig* wcfg = g_pWeaponArmory->getWeaponConfig(ItemID);
		if(wcfg == NULL) {
			//r3dOutToLog("!!!! there is no weapon id %d\n", ItemID);
			continue;
		}

		m_Weapons[i] = game_new Weapon(this, i, wcfg, isFirstPerson, true, i <= wiCharDataFull::CHAR_LOADOUT_WEAPON2 ? &CurLoadout.Attachment[i] : NULL);
		r3d_assert(m_Weapons[i]); // to check for out of memory
	}

	if(NetworkLocal)
	{
		updateWeaponUI();
	}

	uberEquip_->SetSlot(SLOT_Weapon, m_Weapons[m_SelectedWeapon]);
	uberEquip_->SetSlot(SLOT_WeaponBackRight, (Weapon*)NULL);
	uberEquip_->SetSlot(SLOT_WeaponSide, (Weapon*)NULL);

	if(m_Weapons[m_SelectedWeapon])
	{
		if(slotIdxToUpdate == -1 || slotIdxToUpdate==m_SelectedWeapon)
			m_Weapons[m_SelectedWeapon]->OnEquip(isFlashlightOn);
	}

	if(m_Weapons[1] && m_SelectedWeapon != 1)
		uberEquip_->SetSlot(SLOT_WeaponSide, m_Weapons[1]);

	const HeroConfig* heroConfig = g_pWeaponArmory->getHeroConfig(CurLoadout.HeroItemID);
	loadHeroMeshes(heroConfig);
	if(heroConfig)
		m_GearWeight += heroConfig->m_Weight;

	const BackpackConfig* backpack = g_pWeaponArmory->getBackpackConfig(CurLoadout.BackpackID);
	if(backpack)
		uberEquip_->SetSlot(SLOT_Backpack, backpack->getMesh());
	else
		uberEquip_->SetSlot(SLOT_Backpack, (r3dMesh*)NULL);

	uint32_t SlotArmorID    = slot.Items[wiCharDataFull::CHAR_LOADOUT_ARMOR].itemID;
	uint32_t SlotHeadGearID = slot.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID;
	if(SlotArmorID>0)
	{
		uberEquip_->SetSlot(SLOT_Armor, g_pWeaponArmory->createGear(SlotArmorID));

		if(uberEquip_->slots_[SLOT_Armor].gear)
			m_GearWeight += uberEquip_->slots_[SLOT_Armor].gear->GetWeight();
	}
	else
		uberEquip_->SetSlot(SLOT_Armor, (Gear*)NULL);

	if(SlotHeadGearID>0)
	{
		uberEquip_->SetSlot(SLOT_Helmet, g_pWeaponArmory->createGear(SlotHeadGearID));

		if(uberEquip_->slots_[SLOT_Helmet].gear)
			m_GearWeight += uberEquip_->slots_[SLOT_Helmet].gear->GetWeight();
	}
	else
		uberEquip_->SetSlot(SLOT_Helmet, (Gear*)NULL);

	// start loading meshes
	for(int i=0; i<NUM_WEAPONS_ON_PLAYER; ++i)
	{
		if(m_Weapons[i])
			m_Weapons[i]->getModel(true, isFirstPerson);
	}
	for(int i=0; i<SLOT_Max; ++i)
	{
		if(uberEquip_->slots_[i].gear)
			uberEquip_->slots_[i].gear->getModel(isFirstPerson);
	}

	uberAnim_->CurrentWeapon = NULL;
	SyncAnimation(true);

	if(NetworkLocal)
	{
		if(r_hud_filter_mode->GetInt() == HUDFilter_NightVision) // check if we still have nightvision goggles
		{
			if(! (CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20188 ||
				CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20187 ||
				CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20067) )
				r_hud_filter_mode->SetInt(HUDFilter_Default);
		}
	}
}

void obj_Player::loadHeroMeshes(const HeroConfig* heroConfig)
{
	bool isFirstPerson = g_camera_mode->GetInt()==2 && NetworkLocal;
	if(heroConfig)
	{
		uberEquip_->SetSlot(SLOT_UpperBody, heroConfig->getBodyMesh(CurLoadout.BodyIdx, isFirstPerson));
		uberEquip_->SetSlot(SLOT_LowerBody, heroConfig->getLegMesh(CurLoadout.LegsIdx));
		uberEquip_->SetSlot(SLOT_Head, heroConfig->getHeadMesh(CurLoadout.HeadIdx));
	}
	else
	{
		uberEquip_->SetSlot(SLOT_UpperBody, (r3dMesh*)NULL);
		uberEquip_->SetSlot(SLOT_LowerBody, (r3dMesh*)NULL);
		uberEquip_->SetSlot(SLOT_Head, (r3dMesh*)NULL);
	}
}

obj_Player::obj_Player()
: footStepsSnd(0)
, footstepsSndBaseVolume(1.0f)
, currentFootStepsSoundID(-1)
, boneId_Bip01_Spine1(-1)
, boneId_Bip01_Neck(-1)
, boneId_Bip01_LUpperArm(-1)
, boneId_Bip01_RUpperArm(-1)
, boneId_Bip01_Head(-1)
, boneId_Bip01_R_Hand(-1)
, boneId_Bip01_L_Hand(-1)
, m_AuraType(AT_NONE)
, netMover(this, _ai_NetPosUpdateDelta, (float)PKT_C2C_MoveSetCell_s::PLAYER_CELL_RADIUS)
, m_enableRendering(true)
, lifeProperties(this)
, m_ItemSimulateLeftClick(false)
, showSlotsInfoTimer(0.0f)
, m_sndBreathSprint(0)
, m_sndInPain(0)
, m_sndClothMovementSprint(0)
, m_sndBreathBaseVolume(1)
, m_BarricadeMeshPlacement(NULL)
, m_isFemaleHero(false)
, m_sndSniperBreath(0)
, m_sndSniperHeartSlow(0)
, m_sndSniperExhale(0)
, m_sndSwimIdle(0)
, m_sndSwimSlow(0)
, m_sndSwim(0)
, m_sndSwimFast(0)
, m_sndSwimIdleId( -1 )
, m_sndSwimSlowId( -1 )
, m_sndSwimId( -1 )
, m_sndSwimFastId( -1 )
, Darkness( 0 )
, isFlashlightOn(true)
, m_SpawnProtectedUntil( 0.0f )
, m_AuraTransarency( 0.35f ) // make aura barely visible, so that at night you are not shinining like a sun
, CreatedFromEditor( 0 )
#ifdef VEHICLES_ENABLED
, isPlayerInVehicle(false)
, isHitByVehicle(false)
, pushVelocity(0.0f)
, hasOpenVehicleRequest(false)
, preparedToFakeDie(false)
, seatPosition(-1)
#endif
{
	uberEquip_ = NULL;
	m_PhysSkeleton = NULL;

	ActionUI_pressedKeyTimer = 0;

	m_disablePhysSkeleton = false;

	PhysXObstacleIndex = -1;
	PhysXTerra3WarmUp  = 0;

	lastTimeHit = 0;
	bOnGround =1;
	fHeightAboveGround = 0.0f;
	StartFallingHeight = 0;
	StartFallingTime = 0;
	bIsSwimming = false;
	bCrouch = 0;
	bProne = 0;
	TimeOfDeath = 0;
	PosOfDeath.Assign(0,0,0);
	DisableKillerView = false;
	Dead_KillerID = 0;
	m_BloodTimer = 0;
	m_GearWeight = 0;
	CustomerID     = 0;
	ClanID         = 0;
	ClanTagColor   = 0xFFFFFFFF;
	r3dscpy(ClanTag, "CLAN");
	GroupID = 0;
	m_isVoiceActive = false;
	m_DevPlayerHide = false;

	uavRequested_  = 0;
	uavViewActive_ = false;

	viewTargetPos		= r3dPoint3D(0, 0, 0);
	viewTargetNorm		= r3dPoint3D(0, 1, 0);

	for(int i=0; i<NUM_WEAPONS_ON_PLAYER; ++i)
		m_Weapons[i] = NULL;
	m_SelectedWeapon = 0;
	m_PrevSelectedWeapon = -1;
	
	rewards_.reserve(32);
	lastRewardShowTime_ = 0;

	threatStats[0].Reset();
	threatStats[1].Reset();
#ifdef VEHICLES_ENABLED
	vehicleViewActive_ = VehicleView_None; 
#endif

#ifndef FINAL_BUILD
	static bool cmd_registered = false;
	if(!cmd_registered)
	{
		cmd_registered = true;
 		REG_CCOMMAND(binv, 0, "show backpack");
 		REG_CCOMMAND(giveitem, 0, "give item to player");
 		REG_CCOMMAND(alist, 0, "list attachments");
 		REG_CCOMMAND(slist, 0, "list shop content");
 		REG_CCOMMAND(sbuy, 0, "buy item");
 		REG_CCOMMAND(invlist, 0, "list inventory content");
 		REG_CCOMMAND(toinv, 0, "backpack to inventory");
 		REG_CCOMMAND(frominv, 0, "inventory to backpack");
	}
#endif
	
	if(g_pDecalChief && g_pDecalChief->GetTypeCount() > 254)
	{
		r3dError("more that 254 decals"); 
	}
	if(ImpactParticleEntry::NumRegistered > 254)
	{
		r3dError("more that 254 impact particles");
	}

	// create animation data first time player is created
	if(!AI_Player_UberData)
	{
		AI_Player_UberData = game_new CUberData();
	}

	if(!gPlayer_AuraTexture)
	{
		gPlayer_AuraTexture = r3dRenderer->LoadTexture("Data/Shaders/TEXTURE/Aura.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
	}

	if(!gPlayer_CamoTexture)
	{
		gPlayer_CamoTexture = r3dRenderer->LoadTexture("Data/Shaders/TEXTURE/plastic_nm.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
	}

#ifndef FINAL_BUILD
	Permanufactura			= 0;
	Permanufactura_Counter	= 0.0f;
#endif
}

obj_Player::~obj_Player()
{
	r3d_assert(m_PhysSkeleton == NULL);
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_Player::Load(const char *fname)
{
	if(!parent::Load(fname)) return FALSE;

	bPersistent 	= 0;

	ObjTypeFlags |= OBJTYPE_Human;

	Height 	 	= 1.8f;
	m_EncryptedUserName.set("---");

	lastDamageRcvd = 0;
	lastTimeHitForce  = r3dPoint3D(1, 0, 0);
	lastTimeHitBone = 0;
	bDead                 = 0;
	BloodEffect = 0;
	m_ReticleCurrentScale = 1.0f;
	m_ReticleTargetScale = 1.0f;
	m_isAiming = false;
	m_isChangedAiming1 = false;
	m_isPressedFireTrigger = false;
	m_isFinishedAiming = false;
	m_isInScope = false;

	grenadeReadyTime = 0;

	m_OrigBBox.Org.Assign( -0.5, 0, -0.5 );
	m_OrigBBox.Size.Assign( 1, 2, 1 );

	SetBBoxLocal( m_OrigBBox ) ;

	m_vVision.Assign(1, 0, 0);
	ViewAngle.Assign(0, 0, 0);
	RecoilViewMod.Assign(0,0,0);
	PermRecoilViewMod.Assign(0,0,0);
	RecoilViewModTarget.Assign(0,0,0);
	RecoilViewModTarget2.Assign(0,0,0);
	SniperViewMod.Assign(0,0,0);
	RecoilCooldown = 0;
	m_fPlayerRotation      = 0;
	m_fPlayerRotationTarget     = 0;
	//UpdateRotation();

	D3DXMatrixIdentity(&DrawRotMatrix);
	D3DXMatrixIdentity(&DrawFullMatrix);
	D3DXMatrixIdentity(&DrawFullMatrix_Localized);
	D3DXMatrixIdentity(&MoveMatrix);

	JumpVelocity = 0;
	m_StaminaPenaltyTime = 0;
	//m_BreathPenaltyTime = 0;

	m_LastKillTime = 0;
	m_RemainingSecondWindTime = 0.0f;
	m_isHoldingBreath = false;

	bodyAdjust_x    = 0;
	bodyAdjust_y[0] = 0;
	bodyAdjust_y[1] = 0;

	RealAcceleration    = r3dPoint3D(0, 0, 0);
	InputAcceleration    = r3dPoint3D(0, 0, 0);
	SetVelocity(r3dPoint3D(0, 0, 0));

	PlayerState	= PLAYER_IDLE;
	PlayerMoveDir   = 0;
	// we pack both vars into an unsigned short in PKT_C2C_MoveRel.state, so check for overflow
	COMPILE_ASSERT(PLAYER_NUM_STATES <= 0xFF);
	COMPILE_ASSERT(CUberData::ANIMDIR_COUNT <= 0xF);

	m_HitMarkerFadeout = 0;
	m_HitMarkerTex = NULL;

	UpdateTransform();
	return TRUE;
}

BOOL obj_Player::OnCreate()
{
	R3DPROFILE_FUNCTION("obj_Player::OnCreate");

	R3D_LOG_TIMESPAN_START(Ai_Player_OnCreate);

	parent::OnCreate();

	r3d_assert(PhysXObstacleIndex == -1);
	if(!NetworkLocal)
	{
		//physXObstacle.mHalfExtents = PxVec3(0.25f, 1.0f, 0.25f);
		physXObstacle.mHalfHeight = 1.0f;
		physXObstacle.mRadius = 0.25f;
		physXObstacle.mPos = PxExtendedVec3(GetPosition().x, GetPosition().y, GetPosition().z);
		physXObstacle.mRot = PxQuat(0.0f, 0.0f, 0.7071f, 0.7071f); // rotated by 90deg on Z axis, to bring capsule up (otherwise it is horizontal).
		physXObstacle.mUserData = this;

		// create obstacle for network players, so we can't bump to them.
		PhysXObstacleIndex = AcquirePlayerObstacleCapsule(GetPosition(), physXObstacle);
	}

	nextSendCamPosTime = r3dGetTime() + 300.0f; // send camera check only after some time to prevent first frame false positives
	bAllowToUseWeapons	= 1;
	afterRespawnTimer = 0;
	lastTimeWeaponSwitch = 0;
	TimeOfLastRespawn = r3dGetTime();
	m_AuraType = AT_NONE;
	footsteps_timeSinceLastMissingHitResult = 0;
	lastTradeRequestedFrom = 0;
	lastTimeUsedConsumable = -99999.0f;
	currentConsumableCooldownTime = 0;
	m_ZombieRepelentTime = 0;
	lastTimeAirHornUse = 0.0f;
	m_ModeCreator = false;
	UpDownEditor=r3dPoint3D(0,0,0);
	LeftRight=0.0f;
	UpDown=2.0f;
	m_NumbeAirDrop=0;

	HideChat = false;

	m_needToDrawBarricadePlacement = 0;
	m_BarricadeMeshPlacement = NULL;
	m_canPlaceConsumableItem = false;

	m_SpeedBoost = 0;
	m_SpeedBoostTime = 0;
	m_MorhineShotTime =0;
	m_BandagesEffectTime = 0;

	CurrentSpread = 0;

	isGameHardcore = gClientLogic().m_gameInfo.mapId==9?true:false;//gamehardcore
	HardMaxPlayers = gClientLogic().m_gameInfo.maxPlayers;//gamehardcore

	if(NetworkLocal)
	{
		extern bool g_AutoRunToggled;
		g_AutoRunToggled = false; // do not auto run automatically

		// otherwise when you are in scope mode you might not see your own player, and so scope will not render. Anyway, we should see our player always
		setSkipOcclusionCheck(true);
		ObjFlags |=	OBJFLAG_ForceSceneBoxBBox | OBJFLAG_AlwaysDraw; 
		PhysicsConfig.group = PHYSCOLL_CHARACTERCONTROLLER;
		PhysicsConfig.type = PHYSICS_TYPE_CONTROLLER;
		PhysicsConfig.mass = 100.0f;
        PhysicsConfig.controllerBehaviorCallback = true;
		PhysicsConfig.ready = true;
		PhysicsObject = BasePhysicsObject::CreateCharacterController(PhysicsConfig, this);

		if(s_laserPointerTex == 0)
		{
			s_laserPointerTex = r3dRenderer->LoadTexture("data/shaders/texture/laserPointerDot.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem );
			r3d_assert(s_laserPointerTex!=NULL);
		}

		if(s_flashlightFlashTex == 0)
		{
			s_flashlightFlashTex = r3dRenderer->LoadTexture("data/shaders/texture/Light_Flashlight.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
			r3d_assert(s_flashlightFlashTex!=NULL);
		}

		if(s_laserFlashTex == 0)
		{
			s_laserFlashTex = r3dRenderer->LoadTexture("data/shaders/texture/Light_Laser.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
			r3d_assert(s_laserFlashTex!=NULL);
		}
	
		if(s_lightHaloTex == 0)
		{
			s_lightHaloTex = r3dRenderer->LoadTexture("data/shaders/texture/Light_Halo_01.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
			r3d_assert(s_lightHaloTex!=NULL);
		}

		m_HitMarkerTex = r3dRenderer->LoadTexture("data/weapons/hitmarker.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem );
		r3d_assert(m_HitMarkerTex != NULL);
	}
	else
	{
		PhysicsConfig.group = PHYSCOLL_CHARACTERCONTROLLER;
		PhysicsConfig.type = PHYSICS_TYPE_RAYCAST_BOX;
		PhysicsConfig.mass = 100.0f;
		// NOTE : if you change this, sync with PHYSICS_TYPE_CONTROLLER offset...
		PhysicsConfig.offset = r3dPoint3D( 0, 0.9f, 0 );
		PhysicsObject = BasePhysicsObject::CreateDynamicObject(PhysicsConfig, this);
	}

	m_Stamina = getMaxStamina();
	m_Breath = getMaxBreath();

	// init animation
	uberEquip_ = game_new CUberEquip(this);
	r3d_assert(AI_Player_UberData);

	CreateUberAnim();

	UpdateLoadoutSlot(CurLoadout, -1);

	PlayerState = PLAYER_IDLE;
	SyncAnimation(true);

	// force animation update here, so that phys skeleton can be updated, otherwise creating too many of them at 0,0,0 causing physX to crash (too many pairs created)
	{
		D3DXMATRIX mr;
		D3DXMatrixRotationY(&mr, R3D_PI);
		D3DXMATRIX CharDrawMatrix = mr;

		UpdateAnimations(CharDrawMatrix);
		UpdateRotation();
	}

	// create phys skeleton after we have animation data
	if(!m_disablePhysSkeleton)
	{
		m_PhysSkeleton = AquireCacheSkeleton() ;
		m_PhysSkeleton->linkParent(GetSkeleton(), DrawFullMatrix, this, NetworkLocal ? PHYSCOLL_LOCALPLAYER : PHYSCOLL_NETWORKPLAYER) ;
		m_PhysSkeleton->SwitchToRagdoll(false);
	}

	if (NetworkLocal)
		OnlyFPSCameraTime = r3dGetTime() + 0.5f;

	// if we was dead on spawn
	if(bDead) 
	{
		r3d_assert(!NetworkLocal);

		PlayerState = PLAYER_DIE;
		uberAnim_->AnimPlayerState = PLAYER_DIE;
		uberAnim_->StartDeathAnim();
		DoRagdoll(true);

		// fucking physx
		SetPosition(GetPosition());
	}

	// load local player sounds
	if(NetworkLocal)
	{
		m_sndBreathSprint = SoundSys.Play(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_BREATHE_F_SLOW":"Sounds/WarZ/PlayerSounds/PLAYER_BREATHE_M_SLOW"), GetPosition(), true);
		SoundSys.Stop(m_sndBreathSprint);
		//r3d_assert(m_sndBreathSprint);
		SoundSys.GetProperty(m_sndBreathSprint, FMOD_EVENTPROPERTY_VOLUME, &m_sndBreathBaseVolume);

		m_sndInPain = SoundSys.Play(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_in_pain_F":"Sounds/WarZ/PlayerSounds/PLAYER_in_pain_M"), GetPosition(), true);
		SoundSys.Stop(m_sndInPain);
		//r3d_assert(m_sndInPain);

		m_sndClothMovementSprint = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_PANTS_MOVE"), GetPosition(), true);
		SoundSys.Stop(m_sndClothMovementSprint);
		//r3d_assert(m_sndClothMovementSprint);

		m_sndSniperBreath = SoundSys.Play(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_SNIPER_BREATHE_F":"Sounds/WarZ/PlayerSounds/PLAYER_SNIPER_BREATHE_M"), GetPosition(), true);
		SoundSys.Stop(m_sndSniperBreath);
		//r3d_assert(m_sndSniperBreath);
		m_sndSniperHeartSlow = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_HEART_SLOW"), GetPosition(), true);
		SoundSys.Stop(m_sndSniperHeartSlow);
		//r3d_assert(m_sndSniperHeartSlow);
		m_sndSniperExhale = SoundSys.Play(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_SNIPER_EXHALE_F":"Sounds/WarZ/PlayerSounds/PLAYER_SNIPER_EXHALE_M"), GetPosition(), true);
		SoundSys.Stop(m_sndSniperExhale);
		//r3d_assert(m_sndSniperExhale);
	}
	// swimming sounds
	m_sndSwimIdleId	= SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_SWIM_IDLE");
	m_sndSwimSlowId	= SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_SWIM_SLOW");
	m_sndSwimId		= SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_SWIM_NORMAL");
	m_sndSwimFastId	= SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_SWIM_FAST");

	m_sndSwimIdle = SoundSys.Play(m_sndSwimIdleId, GetPosition(), true);
	SoundSys.Stop(m_sndSwimIdle);
	//r3d_assert(m_sndSwimIdle);
	m_sndSwimSlow = SoundSys.Play(m_sndSwimSlowId, GetPosition(), true);
	SoundSys.Stop(m_sndSwimSlow);
	//r3d_assert(m_sndSwimSlow);
	m_sndSwim = SoundSys.Play(m_sndSwimId, GetPosition(), true);
	SoundSys.Stop(m_sndSwim);
	//r3d_assert(m_sndSwim);
	m_sndSwimFast = SoundSys.Play(m_sndSwimFastId, GetPosition(), true);
	SoundSys.Stop(m_sndSwimFast);
	//r3d_assert(m_sndSwimFast);

	if(NetworkLocal && hudMain)
	{
		hudMain->setTPSReticleVisibility(g_camera_mode->GetInt()==0?1:0);
	}

	TeleportPlayer(GetPosition(), "OnCreate");

	R3D_LOG_TIMESPAN_END(Ai_Player_OnCreate);

	if (!NetworkLocal)
	{
		if (!IsInVehicle())
		{
			if (m_DevPlayerHide == true)
				TogglePhysicsSimulation(false);
		}
	}


	return TRUE;
}

#ifdef VEHICLES_ENABLED
void obj_Player::SetCurrentVehicleId(DWORD vehicleId, int seatPosition)
{
	currentVehicleId = vehicleId;

	if (currentVehicleId != 0)
	{
		obj_Vehicle* vehicle = g_pPhysicsWorld->m_VehicleManager->GetVehicleById(vehicleId);
		if (!vehicle)
		{
			isPlayerInVehicle = false;
			return;
		}

		enterVehicle(vehicle, seatPosition);
	}
	else
	{
		if (NetworkLocal)
		{
			g_pPhysicsWorld->m_VehicleManager->SetHasDrivableCar(false);
			ClearVehicle();
		}
	}
}

void obj_Player::PhysicsEnable( const int& physicsEnabled )
{
	UpdateTransform();

	if(physicsEnabled != (int)m_bEnablePhysics)
	{
		m_bEnablePhysics = physicsEnabled?true:false;
		if (!m_bEnablePhysics)
		{
			SAFE_DELETE(PhysicsObject);
		}
		else
			CreatePhysicsData();
	}
}
#endif

BOOL obj_Player::OnDestroy()
{
	if(GetNetworkID())
	{
		// ptumik: duplicate this here too, for when we are unloading level, we might get into a race condition:
		// player would be already deleted, but pointer would still be left in ClientLogic and later it would crash in VOIP in OnClientDissapear()
		int playerIdx = GetNetworkID() - NETID_PLAYERS_START;
		gClientLogic().SetPlayerPtr(playerIdx, NULL); 
	}

	m_BarricadeMeshPlacement = NULL;

	for(int i=0; i<NUM_WEAPONS_ON_PLAYER; ++i)
	{
		SAFE_DELETE(m_Weapons[i]);
	}

	ReleasePlayerObstacleCapsule(&PhysXObstacleIndex);	// player is gone, clear obstacle

	if(NetworkLocal)
	{
		if(gClientLogic().localPlayer_ == this)
			gClientLogic().localPlayer_ = 0;

		r3dRenderer->DeleteTexture(s_laserPointerTex);
		s_laserPointerTex = NULL;

		r3dRenderer->DeleteTexture(s_UAVFriendlyBlinkTex);
		s_UAVFriendlyBlinkTex = NULL;

		r3dRenderer->DeleteTexture(s_flashlightFlashTex);
		s_flashlightFlashTex = NULL;

		r3dRenderer->DeleteTexture(s_laserFlashTex);
		s_laserFlashTex = NULL;

		r3dRenderer->DeleteTexture(s_lightHaloTex);
		s_lightHaloTex = NULL;
	}

	SAFE_DELETE(uberAnim_);
	SAFE_DELETE(uberEquip_);

	// temporary fix until I can talk with Pavel about this
	// I have to TogglePhysicsSimulation off to remove the actor from the scene
	// otherwise when entering the vehicle, the vehicle "blows up"
	// when a player quits the game inside of the vehicle with physics off, 
	// the unlink method inside this method blows up.
#ifdef VEHICLES_ENABLED
	if (!IsInVehicle())
#endif
		ReleaseCachedPhysSkeleton( m_PhysSkeleton ) ;
#ifdef VEHICLES_ENABLED
	else
	{
		ClearVehicle();
	}
#endif
	m_PhysSkeleton = 0 ;
	
	for(uint32_t i=0; i<footStepsArr.Count(); ++i) 
	{
		SoundSys.Stop(footStepsArr[i].snd);
		SoundSys.Release(footStepsArr[i].snd);
	}
	footStepsArr.Clear();

	if(NetworkLocal)
	{
		SoundSys.Stop(m_sndBreathSprint); SoundSys.Release(m_sndBreathSprint);
		SoundSys.Stop(m_sndInPain); SoundSys.Release(m_sndInPain);
		SoundSys.Stop(m_sndClothMovementSprint); SoundSys.Release(m_sndClothMovementSprint);

		SoundSys.Stop(m_sndSniperBreath); SoundSys.Release(m_sndSniperBreath);
		SoundSys.Stop(m_sndSniperHeartSlow); SoundSys.Release(m_sndSniperHeartSlow);
		SoundSys.Stop(m_sndSniperExhale); SoundSys.Release(m_sndSniperExhale);

	// swimming sounds
		SoundSys.Stop(m_sndSwimIdle); SoundSys.Release(m_sndSwimIdle);
		SoundSys.Stop(m_sndSwimSlow); SoundSys.Release(m_sndSwimSlow);
		SoundSys.Stop(m_sndSwim); SoundSys.Release(m_sndSwim);
		SoundSys.Stop(m_sndSwimFast); SoundSys.Release(m_sndSwimFast);
	}

	if(hudMain)
	{
		if(!m_CharIcon.IsUndefined())
			hudMain->removeUserIcon(m_CharIcon);

	}

	return parent::OnDestroy();
}

float getMinimumAngleDistance(float from, float to)
{
	//make sure the angles are in range [0..360)
	const float da = 1.0f/360.0f;
	from = from - floorf(from * da)*360.0f;
	to = to - floorf(to * da)*360.0f;

	float d = to - from;
	if(d <-180.0f)	d += 360.0f;
	if(d > 180.0f)	d -= 360.0f;
	return d;
}

void Fix_Angle(float& deg)
{
	if(deg < -R3D_PI)
		deg += R3D_PI * 2;
	else if(deg >= R3D_PI)
		deg -= R3D_PI * 2;
}

void obj_Player::UpdateRotationToTarget(float fY, float fTurnSpeed)
{
	// if don't need to rotate
	if(R3D_ABS(fY) < 0.2f|| R3D_ABS(fY) > 120)
	{
		m_fPlayerRotation = m_fPlayerRotationTarget;
		uberAnim_->StopTurnInPlaceAnim();
		return;
	}

	float fTimePassed = r3dGetFrameTime();

	// turn character to the needed angle
	float step = fTurnSpeed * fTimePassed;

	m_fPlayerRotation += fY < 0.0f ? -step : +step;
		
	// check if we finished rotation, overrotated
	float fY2 = getMinimumAngleDistance(m_fPlayerRotation, m_fPlayerRotationTarget);
	if((fY > 0 && fY2 <= 0) || (fY < 0 && fY2 >= 0) 
		
		|| 
		// don't overreact - can get to any angle with this sort of step anyway (happens on low fps)
		step >= 180.f )
	{
		m_fPlayerRotation = m_fPlayerRotationTarget;
		uberAnim_->StopTurnInPlaceAnim();
	}
}

R3D_FORCEINLINE void UpdateBodyAdjustX( float* ioAdjust, float newadjust, float dt )
{
	float bodyAdjust_x = *ioAdjust ;
	bool was_less = bodyAdjust_x < newadjust ;

	bodyAdjust_x += (newadjust - bodyAdjust_x) * dt * 4.0f;

	if( was_less )
	{
		if( bodyAdjust_x > newadjust )
			bodyAdjust_x = newadjust ;
	}
	else
	{
		if( bodyAdjust_x < newadjust )
			bodyAdjust_x = newadjust ;
	}

	*ioAdjust = bodyAdjust_x ;
}

void obj_Player::UpdateUpperBodyAngLegs()
{
	float fTimePassed = r3dGetFrameTime();
	const float fMaxBodyAngle = R3D_DEG2RAD(45.0f);

	// turn firstly upper body only and only then legs
	float fY = getMinimumAngleDistance(m_fPlayerRotation, m_fPlayerRotationTarget);
	float newadjust = R3D_DEG2RAD(fY);

	UpdateBodyAdjustX( &bodyAdjust_x, newadjust, fTimePassed ) ;

	if(NetworkLocal && g_camera_mode->GetInt()==2)
	{
		UpdateRotationToTarget(fY, _ai_fTurnSpeedIdle);
	}
	else
	{
		if(uberAnim_->turnInPlaceTrackID == CUberAnim::INVALID_TRACK_ID)
		{
			if(R3D_ABS(bodyAdjust_x) > fMaxBodyAngle)
			{
				uberAnim_->StartTurnInPlaceAnim();
				bodyAdjust_x = R3D_CLAMP(bodyAdjust_x, -fMaxBodyAngle, fMaxBodyAngle);
			}
		}
		else
		{
			UpdateRotationToTarget(fY, _ai_fTurnSpeedIdle);
			// set new body adjust after turning body
			float fY = getMinimumAngleDistance(m_fPlayerRotation, m_fPlayerRotationTarget);
			float newadjust = R3D_DEG2RAD(fY);
			bodyAdjust_x = newadjust;
		}
	}
}

void obj_Player::UpdateRotation()
{
	R3DPROFILE_FUNCTION("UpdateRotation");

	uberAnim_->UpdateTurnInPlaceAnim();

	float fPrevRotation = m_fPlayerRotation;
	float fTimePassed = r3dGetFrameTime();
	if(PlayerState == PLAYER_IDLE || (PlayerState == PLAYER_MOVE_CROUCH && PlayerMoveDir == CUberData::ANIMDIR_Stand))
	{
		UpdateUpperBodyAngLegs();
	}
	else
	{
		uberAnim_->StopTurnInPlaceAnim();
		
		UpdateBodyAdjustX(&bodyAdjust_x, 0.f, fTimePassed * 4.0f);

		float fY = getMinimumAngleDistance(m_fPlayerRotation, m_fPlayerRotationTarget);
		UpdateRotationToTarget(fY, _ai_fTurnSpeedWalk);
	}
	
	// lerp body bending to target
	if(bodyAdjust_y[0] < bodyAdjust_y[1]) {
		bodyAdjust_y[0] = R3D_MIN(bodyAdjust_y[0] + fTimePassed * 3.2f, bodyAdjust_y[1]);
	} else if(bodyAdjust_y[0] > bodyAdjust_y[1]) {
		bodyAdjust_y[0] = R3D_MAX(bodyAdjust_y[0] - fTimePassed * 3.2f, bodyAdjust_y[1]);
	} else
		bodyAdjust_y[0] = bodyAdjust_y[1];

#ifdef VEHICLES_ENABLED
	if (IsInVehicle() && currentVehicle)
	{
		// create rotation matrix
		D3DXMatrixRotationYawPitchRoll(&MoveMatrix, R3D_DEG2RAD(currentVehicle->GetRotationVector().x), R3D_DEG2RAD(currentVehicle->GetRotationVector().y), R3D_DEG2RAD(currentVehicle->GetRotationVector().z));
		D3DXMatrixRotationYawPitchRoll(&DrawRotMatrix, R3D_DEG2RAD(currentVehicle->GetRotationVector().x), R3D_DEG2RAD(currentVehicle->GetRotationVector().y), R3D_DEG2RAD(currentVehicle->GetRotationVector().z));
	}
	else
#endif
	{
		// create rotation matrix
		D3DXMatrixRotationYawPitchRoll(&MoveMatrix, R3D_DEG2RAD(m_fPlayerRotation) + bodyAdjust_x, 0, 0);
		D3DXMatrixRotationYawPitchRoll(&DrawRotMatrix, R3D_DEG2RAD(m_fPlayerRotation), 0, 0);
	}

	r3dPoint3D pos = GetPosition();

	extern int CurHUDID;
	if(g_camera_mode->GetInt()==2 && NetworkLocal && ((g_bEditMode && ((CurHUDID==1 && strncmp("RespawnPlayer", Name.c_str(), 13) != 0) || CurHUDID==6) )||!g_bEditMode) && !bDead && !uavViewActive_)
	{
		float hands_height = 0.0f;
		{
			D3DXMATRIX tmp, res;
			D3DXMatrixIdentity(&tmp);
			uberAnim_->anim.GetCurrentSkeleton()->GetBoneWorldTM(boneId_Bip01_Head, &res, tmp);
			hands_height = res._42;
		}
		// weapon movement when turning
		D3DXMATRIX m3, m4;
		float limit = 2.0f;
		if(m_isAiming)
			limit = 0.5f;
		// when firing, don't delay movement
		if(m_Weapons[m_SelectedWeapon] && (r3dGetTime() < m_Weapons[m_SelectedWeapon]->getLastTimeFired() + 0.5f))
			limit = 0.0f;
		float fRot = -R3D_CLAMP(getMinimumAngleDistance(fPrevRotation, m_fPlayerRotation)*10.0f, -limit, limit);
		static float EMAPrev = 0.0f;
		
		float EMACur = EMAPrev + 0.20f * (fRot - EMAPrev); // exponential moving average of about 15 prev.values
		EMAPrev = EMACur;

		D3DXMatrixRotationYawPitchRoll(&m3, R3D_DEG2RAD(EMACur), 0, 0);

		D3DXMatrixTranslation(&m4, 0, -hands_height, 0.1f);
		D3DXMatrixMultiply(&DrawFullMatrix, &m3, &m4);
		
		D3DXMatrixMultiply(&DrawFullMatrix_Localized, &DrawFullMatrix, &r3dRenderer->InvViewMatrix_Localized);		

		D3DXMatrixMultiply(&DrawFullMatrix, &DrawFullMatrix, &r3dRenderer->InvViewMatrix);
	}
	else
	{
		DrawFullMatrix = DrawRotMatrix;

		DrawFullMatrix._41 += pos.x;
		DrawFullMatrix._42 += pos.y;
		DrawFullMatrix._43 += pos.z;

		DrawFullMatrix_Localized = DrawRotMatrix;

		DrawFullMatrix_Localized._41 += pos.x - r3dRenderer->LocalizationPos.x;
		DrawFullMatrix_Localized._42 += pos.y - r3dRenderer->LocalizationPos.y;
		DrawFullMatrix_Localized._43 += pos.z - r3dRenderer->LocalizationPos.z;
	}

	// set physx rotation vectors
	SetRotationVector(r3dVector(m_fPlayerRotation, 0, 0));
}

static bool updateShootingTarget(obj_Player* pl, const r3dPoint3D& shootFrom, const r3dPoint3D& muzzlePos, r3dPoint3D& sPos, r3dPoint3D& sNorm, r3dMaterial** sMat, GameObject** gameObj, const char** actorName, float spread, bool recheckCollisionAgainsMuzzler,
								 const r3dPoint3D* opt_FireDir)
{
	r3d_assert(pl);
	r3d_assert(sMat);
	r3d_assert(gameObj);
	
	VMPROTECT_BeginMutation("updateShootingTarget");
	
	const float MAX_CASTING_DISTANCE = 20000.0f;
	r3dPoint3D dir;
	if(opt_FireDir == NULL)
	{
		if(pl->m_isInScope || g_camera_mode->GetInt() != 1)
			r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &dir);
		else
			r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &dir);
	}
	else
		dir = *opt_FireDir;

	*sMat = NULL;
	*gameObj = NULL;

	// convert spread from diameter at 50 meter range to radius at 1meter range
	spread = (spread*0.5f)/50.0f; // 0.5 - because spread initially set as diameter, and we need radius. 50-because spread is set at 50 meter range. formula is: tanA=opp/adj = so that is angle that we need to feed into raycast

	// limit spread to prevent ppl shooting behind, etc.
	spread = R3D_CLAMP(spread, -0.5f, 0.5f); 
	D3DXMATRIX rotMat;
	D3DXMatrixRotationYawPitchRoll(&rotMat, u_GetRandom(-spread, spread), u_GetRandom(-spread, spread), u_GetRandom(-spread, spread));
	r3dPoint3D dirWithSpread;
	D3DXVec3TransformNormal((D3DXVECTOR3*)&dirWithSpread, (D3DXVECTOR3*)&dir, &rotMat);
	{
		PxVec3 testDir = PxVec3(dirWithSpread.x, dirWithSpread.y, dirWithSpread.z);

		// shoot from camera :)
		PxRaycastHit hit;
		PhysicsCallbackObject* target = NULL;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
		g_pPhysicsWorld->raycastSingle(PxVec3(shootFrom.x, shootFrom.y, shootFrom.z), testDir, MAX_CASTING_DISTANCE, 
													PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL, hit, filter);
		

		if( recheckCollisionAgainsMuzzler && hit.shape )
		{
			r3dPoint3D impactPos = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);
			r3dPoint3D dir2 = (impactPos - pl->GetPosition()).Normalize();
			if (dirWithSpread.Dot(dir2) < 0.0f)
			{
				r3dPoint3D forwV = pl->GetvForw();
				float yRot = pl->bodyAdjust_y[1];

				D3DXMATRIX r1;
				D3DXMatrixRotationYawPitchRoll(&r1, R3D_DEG2RAD(pl->m_fPlayerRotationTarget), -yRot, 0);
				D3DXVECTOR3 res, orig;
				orig = D3DXVECTOR3(0, 0, 1);
				D3DXVec3TransformNormal(&res, &orig, &r1);
				forwV = r3dPoint3D(res.x, res.y, res.z);

				// get muzzler position to find proper rotation
				PxRaycastHit hit2;
				g_pPhysicsWorld->raycastSingle(PxVec3(muzzlePos.x, muzzlePos.y, muzzlePos.z), PxVec3(forwV.x, forwV.y, forwV.z), MAX_CASTING_DISTANCE, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL, hit2, filter);
				if(hit2.shape != hit.shape)
				{
					// replace old collision data with new one
					hit = hit2;
				}
			}
		}
		
		if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
		{
			if(actorName)
				*actorName = hit.shape->getActor().getName();

			PxU32 faceIndex = hit.faceIndex;
			{
				PxTriangleMeshGeometry pxGeometry;
				if(hit.shape->getTriangleMeshGeometry(pxGeometry))
				{
					r3d_assert(pxGeometry.triangleMesh);
					const PxU32* remap = pxGeometry.triangleMesh->getTrianglesRemap();
					r3d_assert(remap);
					PxU32 numTriangles = pxGeometry.triangleMesh->getNbTriangles();
					r3d_assert(faceIndex < numTriangles);
					faceIndex = remap[faceIndex];
				}
			}


			*gameObj = target->isGameObject();
			if(*gameObj)
			{
				if( (*gameObj)->isObjType( OBJTYPE_Mesh ) )
					*sMat = static_cast< MeshGameObject* > ( target )->MeshLOD[ 0 ]->GetFaceMaterial( faceIndex );
			}
			else if(target->hasMesh())
			{
				*sMat = target->hasMesh()->GetFaceMaterial( faceIndex );
			}
			if (!*sMat)
				*sMat = target->GetMaterial(faceIndex);

			sPos.x = hit.impact.x;
			sPos.y = hit.impact.y;
			sPos.z = hit.impact.z;

			sNorm.x = hit.normal.x;
			sNorm.y = hit.normal.y;
			sNorm.z = hit.normal.z;

			return true;
		}
	}
	
	sPos = muzzlePos + dirWithSpread * 500.0f;      //sPos = muzzlePos + dir * 500.0f; Cynthia: fix bugID 1161, dirWithSpread is supposed to be used here instread of dir.
	sNorm = -dirWithSpread;							//sNorm = -dir;
	
	// do not allow to shoot behind you, like when there a pole between your character and camera
	{
		r3dPoint3D dir2 = (sPos - muzzlePos).Normalize();
		if(dir.Dot(dir2) < 0.0f)
		{
			*sMat = NULL;
			*gameObj = NULL;
		}
	}

	return false;

	VMPROTECT_End();
}

static bool updateMeleeTarget(obj_Player* pl, const r3dPoint3D& shootFrom, const r3dPoint3D& muzzlePos, r3dPoint3D& sPos, r3dPoint3D& sNorm, r3dMaterial** sMat, GameObject** gameObj, const char** actorName, float spread, bool recheckCollisionAgainsMuzzler)
{
	r3d_assert(pl);
	r3d_assert(sMat);
	r3d_assert(gameObj);

	VMPROTECT_BeginMutation("updateMeleeTarget");

	*sMat = NULL;
	*gameObj = NULL;

	// for melee we need to check if we hit anyone in front of us. PhysX doesn't allow to do this check easily (overlaps do not return hit information, just shapes), so instead 
	// I'm going to do 9 ray casts in front of player. They are super short, so it shouldn't cause any performance issues 
	// and we going to hit first player, if any
	// TODO: we need to handle a case when we hit static object only and do not hit any player, but for melee this is minor
	const float MAX_CASTING_DISTANCE = 2.0f;
	float rayDegrees[9] = {-30.0f, -20.0f, -10.0f, -5.0f, 0.0f, 5.0f, 10.0f, 20.0f, 30.0f};
	bool hasHitAnything = false;
	for(int iRay = 0; iRay <9; ++iRay)
	{
		D3DXMATRIX rotMat;
		D3DXMatrixRotationYawPitchRoll(&rotMat, R3D_DEG2RAD(rayDegrees[iRay]), 0, 0);
		r3dPoint3D dirWithSpread;
		D3DXVec3TransformNormal((D3DXVECTOR3*)&dirWithSpread, (D3DXVECTOR3*)&gCam.vPointTo/*(D3DXVECTOR3*)&pl->GetvForw()*/, &rotMat);

		{
			PxRaycastHit hit;
			PhysicsCallbackObject* target = NULL;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
			g_pPhysicsWorld->raycastSingle(PxVec3(shootFrom.x, shootFrom.y, shootFrom.z), PxVec3(dirWithSpread.x, dirWithSpread.y, dirWithSpread.z), MAX_CASTING_DISTANCE, 
				PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL, hit, filter);

			if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
			{
				if(actorName)
					*actorName = hit.shape->getActor().getName();

				PxU32 faceIndex = hit.faceIndex;
				{
					PxTriangleMeshGeometry pxGeometry;
					if(hit.shape->getTriangleMeshGeometry(pxGeometry))
					{
						r3d_assert(pxGeometry.triangleMesh);
						const PxU32* remap = pxGeometry.triangleMesh->getTrianglesRemap();
						r3d_assert(remap);
						PxU32 numTriangles = pxGeometry.triangleMesh->getNbTriangles();
						r3d_assert(faceIndex < numTriangles);
						faceIndex = remap[faceIndex];
					}
				}

				*gameObj = target->isGameObject();
				if(*gameObj)
				{
					if( (*gameObj)->isObjType( OBJTYPE_Mesh ) )
						*sMat = static_cast< MeshGameObject* > ( target )->MeshLOD[ 0 ]->GetFaceMaterial( faceIndex );
				}
				else if(target->hasMesh())
				{
					*sMat = target->hasMesh()->GetFaceMaterial( faceIndex );
				}
				if (!*sMat)
					*sMat = target->GetMaterial(faceIndex);

				sPos.x = hit.impact.x;
				sPos.y = hit.impact.y;
				sPos.z = hit.impact.z;

				sNorm.x = hit.normal.x;
				sNorm.y = hit.normal.y;
				sNorm.z = hit.normal.z;

				hasHitAnything = true; 

				// for melee we will skip hits until we hit human or zombie, in case if we do not hit any of them, but we hit some other static object, than we will return it instead
				if(!*gameObj)
					continue;
				if(!((*gameObj)->isObjType(OBJTYPE_Human) || (*gameObj)->Class->Name == "obj_AnimalDummy" || (*gameObj)->isObjType(OBJTYPE_Zombie) || (*gameObj)->Class->Name == "obj_ZombieDummy"))
					continue;

				return true;
			}
		}
	}
	if(hasHitAnything)
		return true;

	r3dPoint3D dir;
	if(pl->m_isInScope || g_camera_mode->GetInt() != 1)
		r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &dir);
	else
		r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &dir);

	sPos = muzzlePos + dir * 500.0f;
	sNorm = -dir;

	// do not allow to shoot behind you, like when there a pole between your character and camera
	{
		r3dPoint3D dir2 = (sPos - muzzlePos).Normalize();
		if(dir.Dot(dir2) < 0.0f)
		{
			*sMat = NULL;
			*gameObj = NULL;
		}
	}

	return false;

	VMPROTECT_End();
}

static bool isMuzzlerBlocked(obj_Player* pl, const r3dPoint3D& muzzlePos)
{
	// check if muzzler is visible, and if not (inside of wall) then we do not shoot anything
	bool muzzlerBlocked = false;
	{
		r3dVector temp = muzzlePos-gCam;
		float len = temp.Length();
		temp.Normalize();
		if(len > 0)
		{
			PxRaycastHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eDYNAMIC|PxSceneQueryFilterFlag::eSTATIC));
			muzzlerBlocked = g_pPhysicsWorld->raycastSingle( PxVec3(gCam.x,gCam.y,gCam.z), PxVec3(temp.x, temp.y, temp.z), len, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
		}
	}
	if(muzzlerBlocked)
		return true;

	const float MAX_CASTING_DISTANCE = 20000.f;
	r3dPoint3D dir;
	if(pl->m_isInScope || g_camera_mode->GetInt() != 1)
		r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &dir);
	else
		r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &dir);

	// check if camera->shoot_target position agains camera->muzzler (disable shooting when close to wall, but looking at step edge)
	float distToMuzzler = (gCam-muzzlePos).Length();
	{
		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eDYNAMIC|PxSceneQueryFilterFlag::eSTATIC));
		if(g_pPhysicsWorld->raycastSingle(PxVec3(gCam.x, gCam.y, gCam.z), PxVec3(dir.x, dir.y, dir.z), MAX_CASTING_DISTANCE, PxSceneQueryFlags(PxSceneQueryFlag::eDISTANCE), hit, filter))
		{
            if(hit.distance < distToMuzzler)
            {
                // shooting blocked before muzzler
                return true;
            }
		}
	}

	return false;
}

void obj_Player::UpdateTargetting()
{
	R3DPROFILE_FUNCTION("UpdateTargetting");
	r3d_assert(NetworkLocal);

	if(uavViewActive_)
		return;

	r3dMaterial* temp;
	GameObject* tempObj;
	r3dPoint3D muzzlerPos = GetMuzzlerPosition();
	
	{
		r3dPoint3D tmpPos, tmpNorm;
		// we always accept this position now, sometimes this jiggles, if that happens we'll have to look at the angle of the aiming and maybe factor out parts of it. 
		updateShootingTarget(this, gCam, muzzlerPos, tmpPos, tmpNorm, &temp, &tempObj, NULL, 0.0f, false, NULL);

		{
			viewTargetPos = tmpPos;
			viewTargetNorm = tmpNorm;
		}
	}

	const char* reticleColor = "white";
	if(tempObj)
	{
		gobjid_t safeID = tempObj->GetSafeID();
		if(m_LocalPlayer_CurrentAimAt!=safeID)
		{	
			m_LocalPlayer_CurrentAimAt = safeID;
			m_LocalPlayer_CurrentAimAt_StartTime = r3dGetTime();
		}
		if(tempObj->isObjType(OBJTYPE_Human))
		{
			obj_Player* target = (obj_Player*)tempObj;
			if(!target->bDead)
			{
				reticleColor = "red";
			}
		}
	}
	else
	{
		m_LocalPlayer_CurrentAimAt = invalidGameObjectID;
		m_LocalPlayer_CurrentAimAt_StartTime = 0.0f;
	}

	r3dPoint3D bendTarget = GetPosition();
	bendTarget.Y += Height * 0.9f;
	bendTarget   += GetvForw() * 0.2f;

	r3dPoint3D v = (viewTargetPos - bendTarget);
	float dist = v.Length();
	float bendUp = asinf(v.y / v.Length());
	if(dist<1.8f)
		bendUp = R3D_LERP(0.0f, bendUp, dist/1.8f);

	bodyAdjust_y[1] = bendUp;

#ifndef FINAL_BUILD
	if( g_char_spect->GetInt() )
	{
		bodyAdjust_y[1] = 0 ;
	}
#endif
}

r3dPoint3D obj_Player::GetMuzzlerPosition()
{
	if(GetSkeleton() && m_Weapons[m_SelectedWeapon])
	{
		return m_Weapons[m_SelectedWeapon]->getMuzzlePos(uberEquip_->getWeaponBone(GetSkeleton(), DrawFullMatrix));
	}
	
	return GetPosition() + GetvForw() * 1.0f + r3dPoint3D(0, 0.5f, 0);
}

// 0 - body
// 1 - head
// 2 - hands
// 3 - legs
BYTE obj_Player::getBodyNameFromBoneName(const char* boneName)
{
	struct boneToBodyPart_s {
		BYTE part;
		const char* bone;
	};
	const static boneToBodyPart_s map[] = {
	  {1, "Bip01_Head"},
	  {2, "Bip01_L_UpperArm"},
	  {2, "Bip01_L_Forearm"},
	  {2, "Bip01_L_Hand"},
	  {2, "Bip01_R_UpperArm"},
	  {2, "Bip01_R_Forearm"},
	  {2, "Bip01_R_Hand"},
	  {3, "Bip01_L_Thigh"},
	  {3, "Bip01_L_Calf"},
	  {3, "Bip01_L_Foot"},
	  {3, "Bip01_L_Toe0"},
	  {3, "Bip01_R_Thigh"},
	  {3, "Bip01_R_Calf"},
	  {3, "Bip01_R_Foot"},
	  {3, "Bip01_R_Toe0"},
	};
	
	// not optimal, but numbers CAN change, because of skeleton modification
	for(int i=0; i<R3D_ARRAYSIZE(map); i++)
	{
		if(strcmp(map[i].bone, boneName) == 0) {
#ifndef FINAL_BUILD
			r3dOutToLog("%s mapped to %d\n", boneName, map[i].part);
#endif
			return map[i].part;
		}
	}
	
	return 0;
}

void obj_Player::CheckFireWeapon()
{
	r3d_assert(NetworkLocal);

	VMPROTECT_BeginMutation("obj_Player::CheckFireWeapon");
	
	static bool wasReloading = false; // static is okay, this code is running for local player only
	lifeProperties.weaponFired = false;

	Weapon* wpn = m_Weapons[m_SelectedWeapon];
	if(wpn == NULL)
	{	
		m_needToDrawBarricadePlacement = 0;	//Cynthia:1299. force to un-draw barricades if it's been saved in locker.
		m_BarricadeMeshPlacement = NULL;
		
		if(hudMain) hudMain->hideWriteNote(); //Cynthia: 1788
		if(! ( hudPause && hudPause->isActive() ) ){//Cynthia: 100. allow empty hands aim only not at pause screen. 
			if(g_toggle_aim->GetBool())
			{
				if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_AIM))
					m_isAiming = !m_isAiming;
			}
			else
			{
				m_isAiming = InputMappingMngr->isPressed(r3dInputMappingMngr::KS_AIM);
			}

			// Can't aim while swimming
			if(IsSwimming())
				m_isAiming = false;
		}
		return;
	}

	bool needExit = false;
	if(d_video_spectator_mode->GetBool()) needExit = true;
	if(d_observer_mode->GetBool()) needExit = true;
	if(hudPause && hudPause->isActive()) needExit = true;
	if(hudVault && hudVault->isActive()) needExit = true;
	if(hudRepair && hudRepair->isActive()) needExit = true;
	if(hudCraft && hudCraft->isActive()) needExit = true;
	if(hudSafelock && hudSafelock->isActive()) needExit = true;
	if(hudTrade && hudTrade->isActive()) needExit = true;
	if(hudStore && hudStore->isActive()) needExit = true;
	if(hudAttm && hudAttm->isActive()) needExit = true;
	if(Mouse->GetMouseVisibility()) needExit = true;
	if(hudMain && hudMain->isSafelockPinActive()){
		if(hudMain) hudMain->hideWriteNote();
		r3dMouse::Show();
		needExit = true;
	}
	if(hudMain && hudMain->isChatInputActive()) {//Cynthia: 1112, disable aiming when chatting
		m_isAiming = false;
		m_isFinishedAiming = false;
		needExit = true;
	}
	if(hudMain && hudMain->isPlayersListVisible()) needExit = true;
	if((PlayerState == PLAYER_MOVE_SPRINT && wpn->getCategory()!=storecat_MELEE) || bDead)  
	{
		if(wasReloading)
		{
			wasReloading = false;
		}
		needExit = true;
	}
	if(PlayerState == PLAYER_PRONE_DOWN || PlayerState == PLAYER_PRONE_UP)
		needExit = true;

#ifdef	DISABLE_GI_ACCESS_ON_PTE_STRONGHOLD_MAP
	// do not allow to use weapons on stronghold except for pickaxe
	if(gClientLogic().m_gameInfo.channel==6 && gClientLogic().m_gameInfo.mapId==GBGameInfo::MAPID_WZ_Cliffside)
	{
		if(wpn->getCategory()>=storecat_ASR && wpn->getCategory()<=storecat_GRENADE)
			needExit = true;
		if(wpn->getCategory() == storecat_MELEE && wpn->getItemID()!=WeaponConfig::ITEMID_ResourceCollectionTool && wpn->getItemID()!=WeaponConfig::ITEMID_ResourceCollectionToolPremium)
			needExit=true;
	}
#endif

	float gamepadLeftTrigger, gamepadRightTrigger;
	Gamepad->GetTrigger(gamepadLeftTrigger, gamepadRightTrigger);

	bool wpnIsNoLongerAvailable = false;     //workaround for the fast that when you use last item, it will call OnBackpackChanged and will DELETE current weapon that you are using
	if(hudMain)								 //not sure what is the proper way to fix this. Use smart pointers maybe?? 
		hudMain->showRangeFinderUI(false);	 //Cynthia:bug 1111: before checking all items, force to close rangefinderUI, because the below if-else are differentiated special items-vised. 
										     //binoculars and range finder are valid and triggered in their own block, even though m_isAiming always set to false when in safe zone. 
	
	if(uavViewActive_) needExit = true;

	if(wpn->isUsableItem() && !needExit)
	{
		if (isGameHardcore == true) //gamehardcore
			return;

		if(wpn->getItemID() == WeaponConfig::ITEMID_Binoculars || wpn->getItemID() == WeaponConfig::ITEMID_RangeFinder) // binoculars, range finder, etc
		{
			bool oldAiming = m_isAiming;
			if(g_toggle_aim->GetBool())
			{
				if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_AIM))
					m_isAiming = !m_isAiming;
			}
			else
			{
				m_isAiming = InputMappingMngr->isPressed(r3dInputMappingMngr::KS_AIM) || gamepadLeftTrigger > 0;
			}

			extern 	PlayerStateVars_s CurrentRig;
			m_isInScope = hasScopeMode() && CurrentRig.allowScope;

			// remember that aiming was changed
			if(m_isAiming != oldAiming)
				m_isChangedAiming1 = true;

			extern float LerpValue;
			m_isFinishedAiming = m_isAiming && LerpValue==1.0f;

			if(wpn->getItemID() == WeaponConfig::ITEMID_RangeFinder && hudMain) 
				hudMain->showRangeFinderUI(m_isFinishedAiming && m_isInScope);
		}
		else if(wpn->getItemID() == WeaponConfig::ITEMID_PieceOfPaper)
		{
			m_isAiming = false;
			m_isFinishedAiming = false;
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_PRIMARY_FIRE) || m_ItemSimulateLeftClick)
			{
				m_ItemSimulateLeftClick = false;
				if(hudMain && hudMain->canShowWriteNote())
					hudMain->showWriteNote(wpn->m_BackpackIdx);
			}
		}
		else if(wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWODBOX)
		{
			return;
		}
		else if(wpn->getItemID() == WeaponConfig::ITEMID_AirHorn)
		{
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_PRIMARY_FIRE) || m_ItemSimulateLeftClick)
			{
				if((r3dGetTime() - lastTimeAirHornUse) > 5.0f && bAllowToUseWeapons)
				{
					m_ItemSimulateLeftClick = false;
					wiInventoryItem wi = wpn->getPlayerItem();
					m_SpawnProtectedUntil = 0.0f;
					lastTimeAirHornUse = r3dGetTime();
					localPlayer_UseItem(wpn->m_BackpackIdx, wi.itemID, GetPosition(), 0.0f);
				}
			}
		}
		else
		{	
			m_isAiming = false;
			m_isFinishedAiming = false;
			if(m_needToDrawBarricadePlacement && InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_AIM))
			{
				m_needToDrawBarricadePlacement = 0;
				m_BarricadeMeshPlacement = NULL;
			}

			// consumables
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_PRIMARY_FIRE) || m_ItemSimulateLeftClick)
			{
				m_ItemSimulateLeftClick = false;

				wiInventoryItem wi = wpn->getPlayerItem(); // COPY of item, not reference. because it can be reset in Fire()
				if(wi.quantity > 0 && wpn->isReadyToFire(true, false)) // consumables have rate of fire (cooldown)
				{
					/*if(gClientLogic().m_gameInfo.mapId!=GBGameInfo::MAPID_WZ_Cliffside && 
						(wi.itemID == WeaponConfig::ITEMID_WoodenDoorBlock ||
					wi.itemID == WeaponConfig::ITEMID_MetalWallBlock ||
					wi.itemID == WeaponConfig::ITEMID_TallBrickWallBlock ||
					wi.itemID == WeaponConfig::ITEMID_WoodenWallPiece ||
					wi.itemID == WeaponConfig::ITEMID_ShortBrickWallPiece ||
					wi.itemID == WeaponConfig::ITEMID_FarmBlock ||
					wi.itemID == WeaponConfig::ITEMID_SolarWaterPurifier ||
					wi.itemID == WeaponConfig::ITEMID_PlaceableLight ||
					wi.itemID == WeaponConfig::ITEMID_SmallPowerGenerator ||
					wi.itemID == WeaponConfig::ITEMID_BigPowerGenerator) )
					{
						if(hudMain) hudMain->showMessage(gLangMngr.getString("InfoMsg_StrongholdOnly"));
						return;
					}*/

					if(wpn->getItemID() == WeaponConfig::ITEMID_BarbWireBarricade || 
						wpn->getItemID() == WeaponConfig::ITEMID_WoodShieldBarricade ||
						wpn->getItemID() == WeaponConfig::ITEMID_WoodShieldBarricadeZB ||
						wpn->getItemID() == WeaponConfig::ITEMID_RiotShieldBarricade ||
						wpn->getItemID() == WeaponConfig::ITEMID_RiotShieldBarricadeZB ||
						wpn->getItemID() == WeaponConfig::ITEMID_SandbagBarricade ||
						wpn->getItemID() == WeaponConfig::ITEMID_Lockbox ||
						wpn->getItemID() == WeaponConfig::ITEMID_WoodenDoorBlock ||
						wpn->getItemID() == WeaponConfig::ITEMID_MetalWallBlock ||
						wpn->getItemID() == WeaponConfig::ITEMID_TallBrickWallBlock ||
						wpn->getItemID() == WeaponConfig::ITEMID_WoodenWallPiece ||
						wpn->getItemID() == WeaponConfig::ITEMID_ShortBrickWallPiece ||
						wpn->getItemID() == WeaponConfig::ITEMID_FarmBlock ||
						wpn->getItemID() == WeaponConfig::ITEMID_SolarWaterPurifier ||
						wpn->getItemID() == WeaponConfig::ITEMID_PlaceableLight ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorBlockSmall ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorBlockBig ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorBlockCircle ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorColum1 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorColum2 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorColum3 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorFloor1 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorFloor2 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorCeiling1 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorCeiling2 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorCeiling3 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWallMetalic ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorSlope ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWall1 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWall2 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWall3 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWall4 ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWall5 ||
						wpn->getItemID() == WeaponConfig::ITEMID_Traps_Bear ||
						wpn->getItemID() == WeaponConfig::ITEMID_Traps_Spikes ||
						wpn->getItemID() == WeaponConfig::ITEMID_GardenTrap_Rabbit ||
						wpn->getItemID() == WeaponConfig::ITEMID_Campfire ||
						wpn->getItemID() == WeaponConfig::ITEMID_SmallPowerGenerator ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorWODBOX ||
						wpn->getItemID() == WeaponConfig::ITEMID_ConstructorBaseBunker ||
						wpn->getItemID() == WeaponConfig::ITEMID_BigPowerGenerator)
					{
						if(gClientLogic().gameDoNotAllowToPlaceObjects)
						{
							if(hudMain) hudMain->showMessage(gLangMngr.getString("ServerShutdownPlaceableObjectTried"));
							return;
						}
						if(m_needToDrawBarricadePlacement == 0)
						{
							m_needToDrawBarricadePlacement = wpn->getItemID();
							return;
						}
						else
						{
							if(m_canPlaceConsumableItem)
							{
								m_needToDrawBarricadePlacement = 0; // and let user place riot shield by sending packet to server
								m_BarricadeMeshPlacement = NULL;
							}
							else
								return;
						}
					}

					if((wpn->getItemID() == WeaponConfig::ITEMID_Bandages || wpn->getItemID() == WeaponConfig::ITEMID_Bandages2 || wpn->getItemID() == WeaponConfig::ITEMID_Antibiotics ||
						wpn->getItemID() == WeaponConfig::ITEMID_Painkillers || wpn->getItemID() == WeaponConfig::ITEMID_Medkit || wpn->getItemID() == WeaponConfig::ITEMID_CraftedBandages) 
						)
					{
						if(CurLoadout.Health > 99)
						{
							if(hudMain) hudMain->showMessage(gLangMngr.getString("InfoMsg_MaxHealthAlready"));
							return;
						}
						if((r3dGetTime() - lastTimeUsedConsumable) < currentConsumableCooldownTime)
							return; // cooldown
					}
					if((wpn->getItemID() == WeaponConfig::ITEMID_C01Vaccine || wpn->getItemID() == WeaponConfig::ITEMID_C04Vaccine) 
						&& CurLoadout.Toxic < 1.0f)
					{
						if(hudMain) hudMain->showMessage(gLangMngr.getString("InfoMsg_NoTocixInBlood"));
						return;
					}
					if(wpn->getItemID()==WeaponConfig::ITEMID_RepairKit || wpn->getItemID()==WeaponConfig::ITEMID_PremRepairKit)
					{
						return;
					}

					uberAnim_->StartShootAnim();
					wpn->Fire(r3dPoint3D(0,0,0), D3DXMATRIX(), true); // to update last fire time

					showSlotsInfoTimer = r3dGetTime() + 5.0f; // show weapon info for X seconds
					if(hudMain)
						hudMain->showSlots(true);

					// todo: we need to sync this crap over network, so that network players will see us using item
					// todo: also, we need to sync when we sent this packet, so that it is in sync with animation
					float var1 = 0.0f;
					r3dPoint3D pos;
					if(m_canPlaceConsumableItem)
						pos = m_BarricadeMeshPlacementPosition;
					else
						pos = GetPosition() + GetvForw()*1.0f;

					var1 = (m_fPlayerRotation) + R3D_RAD2DEG(bodyAdjust_x) + 180; // always send proper orientation

					bool needCorrectPos = false; // those objects require precise position, otherwise there will be desync between client\server (mainly spawn beacon problem)
					if(wi.itemID == WeaponConfig::ITEMID_BarbWireBarricade ||
						wi.itemID == WeaponConfig::ITEMID_WoodShieldBarricade ||
						wi.itemID == WeaponConfig::ITEMID_WoodShieldBarricadeZB ||
						wi.itemID == WeaponConfig::ITEMID_RiotShieldBarricade || 
						wi.itemID == WeaponConfig::ITEMID_RiotShieldBarricadeZB || 
						wi.itemID == WeaponConfig::ITEMID_SandbagBarricade || 
						wi.itemID == WeaponConfig::ITEMID_Lockbox ||
						wi.itemID == WeaponConfig::ITEMID_WoodenDoorBlock ||
						//wi.itemID == WeaponConfig::ITEMID_MetalWallBlock ||
						//wi.itemID == WeaponConfig::ITEMID_TallBrickWallBlock ||
						//wi.itemID == WeaponConfig::ITEMID_WoodenWallPiece ||
						//wi.itemID == WeaponConfig::ITEMID_ShortBrickWallPiece ||
						wi.itemID == WeaponConfig::ITEMID_FarmBlock ||
						wi.itemID == WeaponConfig::ITEMID_SolarWaterPurifier ||
						wi.itemID == WeaponConfig::ITEMID_PlaceableLight ||
						wi.itemID == WeaponConfig::ITEMID_SmallPowerGenerator ||
						wi.itemID == WeaponConfig::ITEMID_BigPowerGenerator)
					{
						pos.y += 1; // +1 Y for ray cast down
						needCorrectPos = true; 
					}
					if(wi.itemID == WeaponConfig::ITEMID_ZombieRepellent)
					{
						m_ZombieRepelentTime = r3dGetTime() + 26.0f;
					}

					
					if (m_ModeEditor == true)
					{
						var1+=LeftRight;
						UpDownEditor=r3dPoint3D(0,0,0);
						LeftRight=0.0f;
						UpDown=2.0f;
						m_ModeEditor=false;
					}

					if(needCorrectPos)
					{
						PxRaycastHit hit;
						PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_PLAYER_COLLIDABLE_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
						if(g_pPhysicsWorld->raycastSingle(PxVec3(pos.x, pos.y, pos.z), PxVec3(0,-1,0), 3.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
							pos = r3dPoint3D(hit.impact.x, hit.impact.y+0.1f, hit.impact.z);
					}
					localPlayer_UseItem(wpn->m_BackpackIdx, wi.itemID, pos, var1);

					m_BandagesEffectTime = r3dGetTime() + 0.5f;

					// after Fire() check new quantity
					{
						if(wpn->getConfig()->m_isConsumable)
							wi.quantity--; // simulate what fire() does
						if(wi.quantity <= 0) {
							wpnIsNoLongerAvailable = true;
							OnBackpackChanged(m_SelectedWeapon);
						}
						else
						{
							if(hudMain)
								hudMain->updateSlotInfo(m_SelectedWeapon, wi.quantity);
						}
					}

				}
			}
		}
		needExit = true;
	}
	
	if(!bAllowToUseWeapons && !wpn->isReloading()){ // otherwise in safezone reloading animation isn't playing
		needExit = true;
	}

	// check weapon restrictions
	if(gClientLogic().localPlayer_) // check that we are in game
	{
		if((gClientLogic().m_gameInfo.flags & GBGameInfo::SFLAGS_DisableASR) && wpn->getCategory() == storecat_ASR)
			needExit = true;
		if((gClientLogic().m_gameInfo.flags & GBGameInfo::SFLAGS_DisableSNP) && wpn->getCategory() == storecat_SNP)
			needExit = true;
		if((gClientLogic().m_gameInfo.flags & GBGameInfo::SFLAGS_DisableWPN) && m_SelectedWeapon != HANDS_WEAPON_IDX)
			needExit = true;
	}

	//if (m_ModeCreator == true)
	//	return;

	if(needExit)
	{
		if(!wpnIsNoLongerAvailable)
			wpn->isReadyToFire(false, false); // very important to call ready to fire with false to reset internal counter of how long trigger was pressed
		return;
	}

	m_isPressedFireTrigger = InputMappingMngr->isPressed(r3dInputMappingMngr::KS_PRIMARY_FIRE) || gamepadRightTrigger > 0;
	// check muzzle block except when using scope
	if(!m_isInScope && wpn->getCategory()!=storecat_MELEE && isMuzzlerBlocked(this, GetMuzzlerPosition()))
		m_isPressedFireTrigger = false;

	bool oldAiming = m_isAiming;
	if(g_toggle_aim->GetBool())
	{
		if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_AIM))
			m_isAiming = !m_isAiming;
	}
	else
	{
		m_isAiming = InputMappingMngr->isPressed(r3dInputMappingMngr::KS_AIM) || gamepadLeftTrigger > 0;
	}
	
	// check if we just switched weapon and do not allow to fire
	if((r3dGetTime() - lastTimeWeaponSwitch) < 0.4f)
	{
		m_isPressedFireTrigger = false;
		m_isAiming = false;
	}

	extern 	PlayerStateVars_s CurrentRig;
	m_isInScope = hasScopeMode() && CurrentRig.allowScope;

	if(wpn->isReloading())
		m_isAiming = false;

	//if(wpn->getAnimType()== WPN_ANIM_MELEE) // no aiming in melee
	//	m_isAiming = false;

	if(wpn->getItemID() == WeaponConfig::ITEMID_UnarmedMelee) // no aiming in unarmed, it look silly.
		m_isAiming = false;

	// remember that aiming was changed
	if(m_isAiming != oldAiming)
		m_isChangedAiming1 = true;

	extern float LerpValue;
	if (wpn->getCategory() != storecat_SNP)
	{
		m_isFinishedAiming = m_isAiming && LerpValue==1.0f;
	}
	else 
	{
		m_isFinishedAiming = m_isAiming && LerpValue==1.0f && m_isInScope;
	}

	bool playerCrouching = PlayerState==PLAYER_MOVE_CROUCH || PlayerState==PLAYER_MOVE_CROUCH_AIM; 
	bool playerProning = PlayerState==PLAYER_MOVE_PRONE || PlayerState==PLAYER_PRONE_AIM || PlayerState == PLAYER_PRONE_IDLE;
	bool playerMoving  = InputAcceleration.Length() > 0.01f;
	bool playerRunning = PlayerState==PLAYER_MOVE_RUN;
	int triggerPressedCounter = wpn->getTriggerPressedCounter();

	float recoilCoolDownMod = 1.0f;
	if(triggerPressedCounter < 3)
		recoilCoolDownMod = 2.50f;
	else
		recoilCoolDownMod = 3.2f;

	// BEGIN SPREAD\RECOIL LOGIC
	/*if(!m_isPressedFireTrigger) // if user let go of trigger, start return recoil back to normal right away, so that it doesn't linger at the top for a second or two after stopping firing
	{
		if(wpn->getCategory() != storecat_SNP)
		{
			RecoilCooldown = 0;
		}
		}*/ //Cynthia: 233 comment this block to bring recoil back when in single mode.  

	//[ST] trying to get higher recoil without changing database. hack :)
	float recoil = wpn->getRecoil()*1.1f;
		
	float spread = CurrentSpread; // spread will be affected now by player state and other modificators

	//[ST] - just in case - not sure why we needed it
	//spread *= GPP->c_fGlobalSpreadMod;

	// Okay by default in NON ADS mode we're increasing spread
	spread *= 1.1f;

	if (wpn->getCategory()==storecat_SHTG) spread *= 1.2f;


	if(playerCrouching)
	{
		//recoil *= GPP->c_fSpreadRecoilMod_Crouch;
		if(wpn->getCategory()==storecat_SHTG)
			spread *= GPP->c_fSpreadModShotgun_Crouch;
		else
			spread *= GPP->c_fSpreadRecoilMod_Crouch;
	}
	if(playerProning)
	{
		if(wpn->getCategory()==storecat_SHTG)
			spread *= GPP->c_fSpreadModShotgun_Crouch*0.75f;
		else
			spread *= GPP->c_fSpreadRecoilMod_Crouch*0.75f;
	}
	if(m_isFinishedAiming)
	{
		recoil *= GPP->c_fSpreadRecoilMod_Aim*2.0f;
		if(wpn->getCategory()==storecat_SHTG)
			spread *= GPP->c_fSpreadModShotgun_Aim;
		else
			spread *= GPP->c_fSpreadRecoilMod_Aim;
	}
	if(playerRunning)
	{
		recoil *= GPP->c_fSpreadRecoilMod_Running;
		spread *= GPP->c_fSpreadRecoilMod_Running;
	}
	//if(playerMoving)
	//{
	//	recoil *= GPP->c_fSpreadRecoilMod_Moving;
	//	spread *= GPP->c_fSpreadRecoilMod_Moving;
	//}

	/*if(wpn->getCategory() == storecat_SNP && !m_isAiming)
	{
		m_ReticleTargetScale = 25.0f;
	}*/

	if(CurLoadout.Skills[CUserSkills::SKILL_Weapons2])
		spread *= 0.98f;
	if(CurLoadout.Skills[CUserSkills::SKILL_Weapons4])
		recoil *= 0.95f;

	// first bullet logic
	if(!playerRunning && m_isFinishedAiming && wpn->getCategory() != storecat_SHTG && bOnGround)
	{
		// spr_mod - to prevent player from shooting straight at target by just clicking on a mouse, rather than holding fire
		float spr_mod = 1.0f - R3D_CLAMP(((r3dGetTime() - wpn->getLastTimeFired())/GPP->c_fZeroSpreadDelayBetweenShots), 0.0f, 0.9f); // 0.9 - first bullet 10% spread
		// if shooting for 10 bullets straight you spread will slowly increase
		//[ST] - Removed this code
		//spread = R3D_LERP(spread*spr_mod, spread, R3D_CLAMP(float(triggerPressedCounter)/10.0f, 0.0f, 1.0f)); 

		// special case for sniper rifle
		if(wpn->getCategory() == storecat_SNP && m_isInScope && triggerPressedCounter==0)
			spread = 0;
	}

	// if in air
	if(!bOnGround)
	{
		if(wpn->getCategory() == storecat_HG)
			spread *= 2.0f;
		else
			spread *= 4.0f; // hard coded, if jumping = spread increased by X times
	}

	// special case for sniper rifle
	if(wpn->getCategory() == storecat_SNP && !m_isInScope)
		spread = 11; // fixed spread at 11 for sniper rifles if not aiming - per sergey's request


	// FINISHED SPREAD\RECOIL LOGIC

	{
		bool isReloading = wpn->isReloading();
		if(isReloading && !wasReloading)
		{
			wasReloading = true;
		}
		if(!isReloading && wasReloading)
		{
			wasReloading = false;
		}
		if(isReloading)
		{
			float progress = R3D_CLAMP((wpn->getReloadProgress()/wpn->getReloadTime()), 0.0f, 1.0f);
		}
	}

	if((InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_RELOAD)||Gamepad->WasReleased(gpX)) && !wpn->isReloading())
		wpn->Reload();

	if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_CHANGE_RATE_FIRE) && !wpn->isReloading())
	{
		wpn->switchFiremode();
//		if(hudMain)
//			hudMain->setFireMode(wpn->getFiremode());
	}

	// check for grenade throw, if successful it will go to normal fire code
	if(wpn->getCategory() == storecat_GRENADE)
	{
		const wiInventoryItem& wi = wpn->getPlayerItem(); 
		if(!uberAnim_->IsGrenadePinPullActive() && wi.quantity > 0)
		{
			if(wpn->isReadyToFire(m_isPressedFireTrigger, m_isFinishedAiming)) 
			{
				uberAnim_->StartGrenadePinPullAnimation();

				m_SpawnProtectedUntil = 0.0f;

				PKT_C2C_PlayerReadyGrenade_s n;
				n.wid = m_SelectedWeapon;
				p2pSendToHost(this, &n, sizeof(n));
				return;
			}
		}
	}


	bool ReadyToFire_ButNoAmmo = false;
	bool isReadyToFire = wpn->isReadyToFire(m_isPressedFireTrigger, m_isFinishedAiming, &ReadyToFire_ButNoAmmo);
	float holdingDelay = 0;

	if(ReadyToFire_ButNoAmmo && wpn->getCategory() != storecat_MELEE && wpn->getCategory() != storecat_GRENADE)
	{
		static int dryFireSoundEvent = 0;
		if(dryFireSoundEvent == 0)
			dryFireSoundEvent = SoundSys.GetEventIDByPath("Sounds/Misc/emptyclip");
		//r3dOutToLog("@@@ event %d\n", dryFireSoundEvent);
		if(dryFireSoundEvent)
			SoundSys.PlayAndForget(dryFireSoundEvent, GetPosition());
	}

	// for grenades, override regular logic a little bit
	if(wpn->getCategory()==storecat_GRENADE && uberAnim_->IsGrenadePinPullActive())
	{
		// override previous readytofire, grenade will be ready only after animation is finished
		isReadyToFire = false;
		if(uberAnim_->IsGrenadePinPullFinished()) // ready to throw
		{
			if(grenadeReadyTime == 0)
				grenadeReadyTime = r3dGetTime(); // first frame when we finished playing pin pull animation, store time, as after that grenade timer is start to tick :)
			if(!m_isPressedFireTrigger || (r3dGetTime()-grenadeReadyTime)>wpn->getConfig()->m_AmmoDelay) // released trigger -> throw
			{
				holdingDelay = r3dGetTime()-grenadeReadyTime;
				grenadeReadyTime = 0;
				isReadyToFire = true;
			}
		}
	}

	// sniper aiming and holding breath logic
	{
		// reset flag that we're holding breath only when key is released. used for prevent sprinting (same key)
		if(m_isHoldingBreath && !InputMappingMngr->isPressed(r3dInputMappingMngr::KS_HOLD_BREATH))
			m_isHoldingBreath = false;
		
		static float last_timer = 0;
		if(wpn->getCategory() == storecat_SNP && m_isAiming) // sniper rifle and in scope mode
		{
			r3d_assert(NetworkLocal); // just to make sure, as this code should run only for local player (using statics here)

			if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_HOLD_BREATH) && m_Breath>0/* && m_BreathPenaltyTime<=0*/)
			{
				m_isHoldingBreath = true;
			
				//ViewAngle += SniperViewMod;
				//SniperViewMod.Assign(0,0,0);
				float d = r3dGetFrameTime();
				if(CurLoadout.Skills[CUserSkills::SKILL_Weapons5])
					d *= 0.95f;
				if(CurLoadout.Skills[CUserSkills::SKILL_Weapons7])
					d *= 0.9f;

				m_Breath = m_Breath - d;
				if(m_Breath < 0)
					m_Breath = 0;
				//if(m_Breath <= 0)
				//	m_BreathPenaltyTime = GPP->c_fSniperRifleAfterHoldBounceTime;
			}
			else
			{
				float swayDecrease = 0.75f;

				SniperViewMod.Assign((r3dSin(last_timer/3)*3)*swayDecrease, (r3dCos(last_timer))*swayDecrease, 0);
				last_timer += r3dGetFrameTime();
			}
		}
		else
		{
			SniperViewMod.Assign(0,0,0);
			last_timer = 0;
		}
	}

	if(isReadyToFire) 
	{
		m_SpawnProtectedUntil = 0.0f;

		if(wpn->getCategory() == storecat_GRENADE)
		{
			uberAnim_->StartGrenadeThrowAnimation();
		}

		int numActualShots = wpn->getNumShotsRequired();
		int numVirtualShots = 0; // for shotgun
		if(numActualShots && wpn->getCategory() == storecat_SHTG)
			numVirtualShots = 7; // sync this number on server if changing it in fn: obj_ServerPlayer::FireWeapon

		while(numActualShots>0)
		{	
			executeWeaponFireLocalPlayer(gCam, recoil, spread, recoilCoolDownMod, holdingDelay, 0, !m_isInScope, 0);
			--numActualShots;

			// update spread after shooting
			CurrentSpread += SpreadIncrease[wpn->getCategory()-storecat_ASR]*wpn->getSpread();
			CurrentSpread = R3D_MIN(CurrentSpread, wpn->getSpread());
			lifeProperties.weaponFired = true;
		}

		// exec shotgun virtual shots
		int fireSeqNo = 1;
		while(numVirtualShots>0)
		{	
			executeWeaponFireLocalPlayer(gCam, recoil, spread, recoilCoolDownMod, holdingDelay, fireSeqNo++, !m_isInScope, 0);	
			--numVirtualShots;
			lifeProperties.weaponFired = true;
		}

		uberAnim_->StartRecoilAnim();
	}

	m_ReticleTargetScale = R3D_MIN(1.0f + spread, 12.0f);

	// update reload anim
	if(wpn->isReloading())
	{
		uberAnim_->StartReloadAnim();
	}
	else
	{
		uberAnim_->StopReloadAnim();
	}

	VMPROTECT_End();
}

// grenades are thrown at specific animation frame, so we cannot remove weapon (grenade) before it executed. So we have to wait for grenade to be thrown before updating our inventory
// this callback handles this
void obj_Player::GrenadeCallbackFromWeapon(const wiInventoryItem& wi)
{
	// simulate what fire() will do
	if(hudMain)
		hudMain->showSlots(true);

	if(wi.quantity <= 0) {
		OnBackpackChanged(m_SelectedWeapon);
	}
	else
	{
		if(hudMain)
			hudMain->updateSlotInfo(m_SelectedWeapon, wi.quantity);
	}
}

float obj_Player::GetGrenadeLaunchDelay()
{
	return uberAnim_->GetGrenadeLaunchFrame() / 25.0f;
}


// make sure this function is the same on server: bool ServerGameLogic::CanDamageThisObject(const GameObject* targetObj)
bool canDamageTarget(const GameObject* obj)
{
	if(obj->isObjType(OBJTYPE_Human))
		return true;
	if(obj->isObjType(OBJTYPE_Zombie))
		return true;
	if(obj->Class->Name == VMPROTECT_DecryptStringA("obj_LightMesh"))
	{
		obj_LightMesh* lmesh = (obj_LightMesh*)obj;
		if(lmesh->isLightOn())
			return true;
		return false;
	}
	if(obj->Class->Name == VMPROTECT_DecryptStringA("obj_Barricade"))
	{
		return true;
	}

	return false;
}

float g_ScopeFeedbackTimer = 0.0f;

void obj_Player::executeWeaponFireLocalPlayer(const r3dPoint3D& from, float recoil, float spread, float recoilCoolDownMod, float holdingDelay, int fireSeqNo, bool checkAgainsMuzzler, int damagePercFromPiercing)
{
	r3d_assert(NetworkLocal);

	VMPROTECT_BeginMutation("obj_Player::executeWeaponFireLocalPlayer");

	// Can't fire a weapon while swimming
	if(IsSwimming())
		return;

	Weapon* wpn = m_Weapons[m_SelectedWeapon];
	r3d_assert(wpn);
	r3d_assert(!wpn->isUsableItem());

	r3dPoint3D	shootTargetPos;
	r3dPoint3D	shootTargetNorm;
	GameObject*	shootTarget    = NULL;
	r3dMaterial*	shootMaterial  = NULL;
	const char*	shootActorName = "";

	// reset rotation if we're firing from idle mode
	if(PlayerState == PLAYER_IDLE)
	{
		bodyAdjust_x = 0;
		m_fPlayerRotation = m_fPlayerRotationTarget;
	}

	if(wpn->getCategory() == storecat_SNP)
	{
		// increase recoil for sniper rifle artificially
		recoil *= 10.0f;
		//if(!m_isFinishedAiming && spread>0) // if not aiming, make spread really huge
		//	spread = 50.0f;
	}

	r3dPoint3D muzzlerPos = GetMuzzlerPosition();
	{
		if(wpn->getCategory() == storecat_MELEE)
			updateMeleeTarget(this, GetPosition()+r3dPoint3D(0, bProne?0.2f:(bCrouch?0.6f:1.5f), 0.0f), muzzlerPos, shootTargetPos, shootTargetNorm, &shootMaterial, &shootTarget, &shootActorName, spread, checkAgainsMuzzler);
		else
			updateShootingTarget(this, from, muzzlerPos, shootTargetPos, shootTargetNorm, &shootMaterial, &shootTarget, &shootActorName, spread, checkAgainsMuzzler, NULL);
	}

	 // in prone, have recoil snap back to original aim
	if(PlayerState>=PLAYER_MOVE_PRONE && PlayerState<=PLAYER_PRONE_IDLE)
	{
		RecoilViewModTarget.y += recoil * u_GetRandom(0.85f, 1.75f);
		if(RecoilViewModTarget.y > GPP->c_MaxVerticalRecoil) // limit how much gun goes up
		{
			RecoilViewModTarget.y = u_GetRandom(GPP->c_MaxVerticalRecoil*0.95f, GPP->c_MaxVerticalRecoil*1.1f);
		}
		RecoilViewModTarget.x += recoil*u_GetRandom(-0.15f, 0.3f); // + to a side
	}
	else // otherwise player has to put aim back manually
	{
		PermRecoilViewMod.y += recoil * u_GetRandom(0.85f, 1.75f);
		if(PermRecoilViewMod.y > GPP->c_MaxVerticalRecoil) // limit how much gun goes up
		{
			PermRecoilViewMod.y = u_GetRandom(GPP->c_MaxVerticalRecoil*0.95f, GPP->c_MaxVerticalRecoil*1.1f);
		}
		PermRecoilViewMod.x += recoil*u_GetRandom(-0.15f, 0.3f); // + to a side
	}

	if(fireSeqNo == 0) // set cooldown only for actual fire
	{
		RecoilCooldown = R3D_MAX(wpn->getRateOfFire()*recoilCoolDownMod, r3dGetFrameTime()*2.50f);
	}

	if(wpn->getCategory()==storecat_SNP || wpn->getCategory()==storecat_SHTG)
		if(RecoilCooldown > r3dGetFrameTime()*8.0f)
			RecoilCooldown = r3dGetFrameTime()*8.0f;

	GameObject* bulletObj = NULL;
	wpn->Fire(shootTargetPos, uberEquip_->getWeaponBone(GetSkeleton(), DrawFullMatrix), fireSeqNo==0, holdingDelay, R3D_ZERO_VECTOR, &bulletObj);
	if (bulletObj && 
		(wpn->getCategory() == storecat_GRENADE || WeaponConfig::ITEMID_FlareGun == wpn->m_pConfig->m_itemID))
	{
		((obj_Grenade*)bulletObj)->m_ItemID = wpn->m_pConfig->m_itemID;
	}

	// send weapon fired event
	if(wpn->getCategory() != storecat_GRENADE)
	{
		r3d_assert(wpn->getCategory() == storecat_MELEE || bulletObj);
	
		PKT_C2C_PlayerFired_s n; 
		n.localId       = bulletObj->GetSafeID();	// can be NULL for melee
		n.spawnID		= 0;
		n.fireSeqNo     = fireSeqNo;
		n.fire_from     = muzzlerPos; // GetPosition();
		n.fire_to       = shootTargetPos;
		n.holding_delay = holdingDelay;
		n.debug_wid     = m_SelectedWeapon;
		p2pSendToHost(this, &n, sizeof(n), true);
	}
	
	if(fireSeqNo == 0)
		wpn->ReloadIfNeeded();

	if(wpn->isImmediateShooting())
	{
		// adjust durability for melee on hit
		if(wpn->getCategory() == storecat_MELEE)
		{
			bool hitSomething = true;
			// note: must be synced with melee weapon degradation PKT_C2C_PlayerHitNothing_s
			if(shootTarget == NULL && shootMaterial == NULL)
				hitSomething = false;
			if(shootTarget && shootTarget->isObjType(OBJTYPE_Human) && ((obj_Player*)shootTarget)->bDead)
				hitSomething = false;
			if(hitSomething)
				wpn->AdjustWeaponDurability();
		}

		if ( ProcessBulletHit(damagePercFromPiercing, this, shootTargetPos, shootTargetNorm, shootTarget, shootMaterial, shootActorName,  wpn->getConfig(), GetMuzzlerPosition(), invalidGameObjectID )  == false ) 
		{	
			r3dPoint3D dir = shootTargetPos - from;
			dir.Normalize();
			executeWeaponFireLocalPlayer(shootTargetPos + dir*0.1f, 0.0f, 0.0f, 0.0f, holdingDelay, false, false, damagePercFromPiercing);
		}
	}
		
	if(fireSeqNo == 0)
	{
		extern float g_shootCameraShakeTimer;
		g_shootCameraShakeTimer = 1.0f;
		if(m_isAiming && hasScopeMode())
		{
			g_ScopeFeedbackTimer = 1.0f;
		}
	}

	//	Change player noise value
	lifeProperties.weaponFired = true;

	VMPROTECT_End();
}

void obj_Player::UpdateAnimations(const D3DXMATRIX& CharDrawMatrix)
{
	R3DPROFILE_FUNCTION("UpdateAnimations");
	const float TimePassed = r3dGetFrameTime();
#if ENABLE_RAGDOLL
	bool ragdoll = m_PhysSkeleton && m_PhysSkeleton->IsRagdollMode();
	if (!ragdoll)
#endif
	{
		uberAnim_->anim.Update(TimePassed, r3dPoint3D(0,0,0), CharDrawMatrix);
		uberAnim_->anim.Recalc();
	}

	if(m_PhysSkeleton)
		m_PhysSkeleton->syncAnimation(GetSkeleton(), DrawFullMatrix, GetAnimation());

#if ENABLE_RAGDOLL
	if (ragdoll)
	{
		r3dBoundBox bbox = m_PhysSkeleton->getWorldBBox();
		bbox.Org -= GetPosition();
		SetBBoxLocal(bbox);
	}
#endif
}

r3dPoint3D obj_Player::GetHeadPosition()
{
	D3DXMATRIX m;
	uberAnim_->anim.GetCurrentSkeleton()->GetBoneWorldTM(boneId_Bip01_Head, &m, DrawFullMatrix);

	return r3dPoint3D(m._41, m._42, m._43);
}

int obj_Player::CalcZombieThreatToPlayer(ThreatStats& threatStat, const r3dPoint3D& atPosition)
{
	// The minimum distance used is maxPerception.  Most distances
	// are squared, so care has been taken to square other values
	// when needed.

	// Update threatStats approx every 1/3rd of a second.
	if( r3dGetTime() < threatStat.timer)
		return (uint32_t)( threatStat.threat * 100.0f );
	threatStat.timer = r3dGetTime() + 0.333f;

	threatStat.Clear();

	// Zombie's ability to perceive the player is based on the player's posture.
	// Default distances for reference:
	//	standStillVisiblity(20.0f)
	//	walkVisibility(50.0f)
	//	runVisibility(70.0f)
	//	sprintVisibility(100.0f)
	//	walkNoise(2.0f)
	//	runNoise(5.0f)
	//	sprintNoise(10.0f)
	float visibilitySq = lifeProperties.getPlayerVisibility() * 100.0f;
	visibilitySq = visibilitySq * visibilitySq;
	float invVisibilitySq = 1.0f / visibilitySq;
	float audibilitySq = lifeProperties.getPlayerHearRadius() * 100.0f;
	audibilitySq = audibilitySq * audibilitySq;
	float invAudibilitySq = 1.0f / audibilitySq;
	float maxPerceptionSq = R3D_MAX( visibilitySq, audibilitySq );
	float invMaxPerceptionSq = 1.0f / maxPerceptionSq;
	float maxPerception = sqrt( maxPerceptionSq );

#ifndef FINAL_BUILD
	threatStat.plrVisibilitySq = visibilitySq;
	threatStat.plrAudibilitySq = audibilitySq;
	threatStat.plrMaxPerceptionSq = maxPerceptionSq;
	threatStat.plrMaxPerception = maxPerception;
	threatStat.plrPosition = GetPosition();
	threatStat.plrVelocity = GetVelocity() + r3dVector(0.0f, 9.81f, 0.0f);
	threatStat.plrFuturePos = atPosition;
#endif

	uint32_t	count = obj_Zombie::ZombieList.Count();
	for(uint32_t i = 0; i < count; ++i)
	{
		if( obj_Zombie::ZombieList[ i ].zombie && obj_Zombie::ZombieList[ i ].distance > 0.001f )
		{
			obj_Zombie::ZombieSortEntry& entry = obj_Zombie::ZombieList[ i ];
			
			// What is the vector to the zombie from the given position.
			r3dVector zombieVec = entry.zombie->GetPosition() - atPosition;
			float zombieDistSq = zombieVec.LengthSq();
			float invDistSq = 1.0f / zombieDistSq;

#ifndef FINAL_BUILD
	threatStat.zmbPosition = entry.zombie->GetPosition();
	threatStat.zmbVector = zombieVec;
#endif

			// Is the zombie targeting the local player? (Don't care what the distance is)
			if( entry.zombie->targetId != invalidGameObjectID &&
				entry.zombie->targetId == GetSafeID() )
			{
				threatStat.interceptSum += 1.0f; // = 1.0f - zombieDistSq * invMaxPerceptionSq;
				++threatStat.zombieInterceptCount;
			}

			// Is the zombie too far away to consider? 
			// Is the zombie alive?
			if( maxPerceptionSq < zombieDistSq || EZombieStates::ZState_Dead == entry.zombie->ZombieState )
				continue;

			// How many zombies are nearby?
			++threatStat.zombieNearbyCount;

			// Keep a weighted sum of each zombie's distance.
			threatStat.nearbySum += 1.0f - zombieDistSq * invMaxPerceptionSq;

			// How close is the closest zombie?
			if( zombieDistSq < threatStat.closestZombie ) threatStat.closestZombie = zombieDistSq;

			// Will the player be visible to the zombie?
			if( 1.0f <= visibilitySq * invDistSq )
			{
				threatStat.visualSum += 1.0f - zombieDistSq * invVisibilitySq;
				++threatStat.inSightRangeCount;
			}

			// Will the player be audible to the zombie?
			if( 1.0f <= audibilitySq * invDistSq )
			{
				threatStat.audibleSum += 1.0f - zombieDistSq * invAudibilitySq;
				++threatStat.inAudioRangeCount;
			}

			//// Does the zombie appear to be attacking?
			//r3dVector plrToZombie	= zombieVec.NormalizeTo();
			//r3dVector zombieDir		= entry.zombie->GetVelocity().NormalizeTo();
			//float zombieHeading		= plrToZombie.Dot( zombieDir );
			//if( -0.4226f > zombieHeading ) // -sin(25 degrees) = -0.4226
			//{
			//	// Scale the intercept value by the max perception distance
			//	threatStat.interceptSum += -zombieHeading * (1.0f - zombieDistSq * invMaxPerceptionSq);
			//	++threatStat.zombieInterceptCount;
			//}
			//else if( EZombieStates::ZState_Attack == entry.zombie->ZombieState &&
			//		 6.25f <= entry.distance ) // 2.5f * 2.5f = 6.25f
			//{
			//	// Give the zombie's interceptSum threat stat 10x the weight if he is nearby and attacking (i.e. not moving)
			//	threatStat.interceptSum += 10.0f;
			//	++threatStat.zombieInterceptCount;
			//}
		}
	}
	if( 0 < threatStat.zombieInterceptCount )
		return (int)(threatStat.threat = 1.0f) * 100;
	if( 0 == threatStat.zombieNearbyCount )
		return (int)(threatStat.threat = 0.0f);

	threatStat.closestZombie	= sqrt( threatStat.closestZombie );
	threatStat.factorClosest	= ( maxPerception > threatStat.closestZombie ) ? 1.0f - ( threatStat.closestZombie / maxPerception ) : 0.0f;
	threatStat.factorNearby		= threatStat.nearbySum / threatStat.zombieNearbyCount;
	threatStat.factorIntercept	= ( 0 < threatStat.zombieInterceptCount ) ? threatStat.interceptSum / threatStat.zombieInterceptCount : 0.0f;
	threatStat.factorVisible	= ( 0 < threatStat.inSightRangeCount ) ? threatStat.visualSum / threatStat.inSightRangeCount : 0.0f;
	threatStat.factorAudible	= ( 0 < threatStat.inAudioRangeCount ) ? threatStat.audibleSum / threatStat.inAudioRangeCount : 0.0f;
	
	threatStat.weightedClosest		= 0.0f;
	threatStat.weightedNearby		= threatStat.factorNearby		* ((bProne)? 0.15f : ((bCrouch) ? 0.20f : 0.25f));
	threatStat.weightedIntercept	= 0.0f;
	threatStat.weightedVisible		= threatStat.factorVisible		* ((bProne)? 0.55f : ((bCrouch) ? 0.50f : 0.45f));
	threatStat.weightedAudible		= threatStat.factorAudible		* ((bProne)? 0.35f : ((bCrouch) ? 0.30f : 0.30f));

	threatStat.threat	= R3D_MAX( threatStat.factorClosest,
						  R3D_MAX( threatStat.factorIntercept,
								   threatStat.weightedNearby + threatStat.weightedVisible + threatStat.weightedAudible ) );

	return (int)( threatStat.threat * 100.0f );
}

int obj_Player::GetGraduatedZombieThreat()
{
	// Where might the player be in 4s, at their current velocity(heading)?
	r3dVector plrVelWithoutGravity = GetVelocity() + r3dVector(0.0f, 9.81f, 0.0f);
	r3dPoint3D futurePos = plrVelWithoutGravity * 4.0f + GetPosition();

	// Snap the future position to the ground.
	PxSweepHit hitDown, hitUp;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
	PxBoxGeometry boxg(0.3f, 0.1f, 0.3f);
	PxTransform pose(PxVec3(futurePos.x, futurePos.y+0.5f, futurePos.z));
	if( g_pPhysicsWorld->PhysXScene->sweepSingle(boxg, pose, PxVec3(0,-1,0), 10.0f, PxSceneQueryFlag::eDISTANCE, hitDown, filter) )
	{	
		if( hitDown.distance >= 0.6f )
			futurePos.y -= hitDown.distance - 0.5f;
	}
	else if( g_pPhysicsWorld->PhysXScene->sweepSingle(boxg, pose, PxVec3(0,1,0), 10.0f, PxSceneQueryFlag::eDISTANCE, hitUp, filter) )
	{
		if( hitUp.distance >= 0.6f )
			futurePos.y += hitUp.distance - 0.5f;
	}

	int threatNearby = CalcZombieThreatToPlayer( threatStats[0], GetPosition() );
	int threatFuture = CalcZombieThreatToPlayer( threatStats[1], futurePos );
	int threat = R3D_MAX( threatNearby, threatFuture );

	if( 60 > threat )
		return 0; // Green
	else if( 72 > threat )
		return 70; // Orangish
	else if( 85 > threat )
		return 90; // Red
	else
		return 100; // Red Pulsating
}

void obj_Player::DoDeath(int networdIDKiller, bool forced_by_server, STORE_CATEGORIES damageSource)
{
#ifdef VEHICLES_ENABLED
	PhysicsEnable(false);

	// this code resolves a rare crash
	// in some rare cases the exit vehicle packet is delayed for whatever reason
	// which allows the player to believe he is still in the vehicle at the same time as a vehicle explodes
	// when the vehicle explodes and the player still believes he is in the vehicle for whatever short amount of time,
	// the player attempts to update his rotation information to the now non-existant vehicle.
	// NOTE: eventually the exit packet WILL come in and clean up the rest of the required information.
	//		 we have to wait for that information because it also contains the correct exit position information.
	if (IsInVehicle())
		ClearVehicle();
#endif

	//r3dOutToLog("%s: DoDeath\n", Name.c_str());
	r3d_assert(!bDead);

	CurLoadout.Health = 0; // to make sure that UI will show zero health

	SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/BulletHits/Hit_Death"), GetPosition());		

	CurrentSpread = 0;
	
	ReleasePlayerObstacleCapsule(&PhysXObstacleIndex);	// player is dead, allow passage thru him

	m_needToDrawBarricadePlacement = 0;
	m_BarricadeMeshPlacement = NULL;
	m_canPlaceConsumableItem = false;

	m_ItemSimulateLeftClick = false;

	m_SpeedBoost = 0;
	m_SpeedBoostTime = 0;
	m_MorhineShotTime = 0;
	m_BandagesEffectTime = 0;

	bCrouch = 0;
	bProne = 0;
	m_isAiming = 0;
	m_isPressedFireTrigger = false;
	lastDamageRcvd = 0;
	BloodEffect = 0;
	bDead = 1;
	TimeOfDeath = r3dGetTime();
	DeathDamageSource = damageSource;
	PosOfDeath = GetPosition();
	Dead_KillerID = networdIDKiller;
	DisableKillerView = false;

#ifdef VEHICLES_ENABLED
	pushVelocity = 0;
#endif

	m_AuraType = AT_NONE;
	grenadeReadyTime = 0;

	// if we have not default backpack - remove it as it will be dropped as item on server
	if(CurLoadout.BackpackID != 20176) {
		uberEquip_->SetSlot(SLOT_Backpack, (r3dMesh*)NULL);
	}

	bIsSwimming = false;
	PlayerState = PLAYER_DIE;
	uberAnim_->AnimPlayerState = PLAYER_DIE;
	uberAnim_->StartDeathAnim();
	DoRagdoll(true, lastTimeHitBone, &lastTimeHitForce);

	// clear rotation so it won't affect ragdoll bbox
	SetRotationVector(r3dPoint3D(0, 0, 0));
	
	RealAcceleration.Assign(0,0,0);
	InputAcceleration.Assign(0,0,0);

	if(!NetworkLocal)
	{
		const ClientGameLogic& CGL = gClientLogic();
		if(CGL.localPlayer_ && CGL.localPlayer_->GetNetworkID() == Dead_KillerID)
		{
			if(!CGL.localPlayer_->bDead)
			{
				CGL.localPlayer_->m_LastKillTime = r3dGetTime();
			}
		}
	}
	else
	{
		
//		hudMain->setHealth(0);

		r_hud_filter_mode->SetInt(0); // turn off NVG
	}

	if(NetworkLocal)
	{
		if(hudAttm && hudAttm->isActive())
			hudAttm->Deactivate();
		if(hudPause && hudPause->isActive())
			hudPause->Deactivate();
		if(hudVault && hudVault->isActive())
			hudVault->Deactivate();
		if(hudRepair && hudRepair->isActive())
			hudRepair->Deactivate();
		if(hudCraft && hudCraft->isActive())
			hudCraft->Deactivate();
		if(hudSafelock && hudSafelock->isActive())
			hudSafelock->Deactivate();
		if(hudTrade && hudTrade->isActive())
			hudTrade->Deactivate();
		if(hudStore && hudStore->isActive())
			hudStore->Deactivate();
		if(hudActionUI->isActive())	//Cynthia:fixed bugID638, deactivate ActionUI here. 
			hudActionUI->Deactivate(); 
		


		g_camera_mode->SetInt(0);
		UpdateLoadoutSlot(CurLoadout, -1); // reload meshes
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_player_death"), GetPosition());

		if(hudMain)
		{
			hudMain->hideGravestone();
			hudMain->hideReadNote();
			hudMain->hideWriteNote();
			hudMain->hideSafelockPin();


			char deadMsg[128] = {0};
			GameObject* killedByObj = GameWorld().GetNetworkObject(networdIDKiller);
			if(killedByObj)
			{
				if(killedByObj == this)
				{
					r3dscpy(deadMsg, "KILLED BY ENVIRONMENT");
				}
				else if(killedByObj->isObjType(OBJTYPE_Human))
				{
					obj_Player* killedByPlr = (obj_Player*)killedByObj;
					char tmpName[64] = {0};
					killedByPlr->GetUserName(tmpName);
					sprintf(deadMsg, "KILLED BY %s", tmpName);
				}
				else if(killedByObj->isObjType(OBJTYPE_Zombie))
					r3dscpy(deadMsg, "EATEN BY ZOMBIE");
			}
			hudMain->showYouAreDead(deadMsg);
		}
	}

	return;
}

void obj_Player::ResPawnFast()
{
	afterRespawnTimer = 2.0f;

	bDead = 0;

	if(NetworkLocal)
		r_hud_filter_mode->SetInt(0); // turn off NVG

	m_SpawnProtectedUntil = r3dGetTime() + 30.0f;

	//extern void TPSGameHUD_OnStartGame();
	//TPSGameHUD_OnStartGame();

	/////////////////////////////////////

	if(!NetworkLocal)
	{
		//physXObstacle.mHalfExtents = PxVec3(0.25f, 1.0f, 0.25f);
		physXObstacle.mHalfHeight = 1.0f;
		physXObstacle.mRadius = 0.25f;
		physXObstacle.mPos = PxExtendedVec3(GetPosition().x, GetPosition().y, GetPosition().z);
		physXObstacle.mRot = PxQuat(0.0f, 0.0f, 0.7071f, 0.7071f); // rotated by 90deg on Z axis, to bring capsule up (otherwise it is horizontal).
		physXObstacle.mUserData = this;

		// create obstacle for network players, so we can't bump to them.
		PhysXObstacleIndex = AcquirePlayerObstacleCapsule(GetPosition(), physXObstacle);
	}

	nextSendCamPosTime = r3dGetTime() + 300.0f; // send camera check only after some time to prevent first frame false positives
	bAllowToUseWeapons	= 1;
	afterRespawnTimer = 0;
	lastTimeWeaponSwitch = 0;
	TimeOfLastRespawn = r3dGetTime();
	m_AuraType = AT_NONE;
	footsteps_timeSinceLastMissingHitResult = 0;
	lastTradeRequestedFrom = 0;
	lastTimeUsedConsumable = -99999.0f;
	currentConsumableCooldownTime = 0;
	m_ZombieRepelentTime = 0;
	lastTimeAirHornUse = 0.0f;
	m_ModeCreator = false;
	UpDownEditor=r3dPoint3D(0,0,0);
	LeftRight=0.0f;
	UpDown=2.0f;
	m_NumbeAirDrop=0;

	HideChat = false;
	if (hudMain) hudMain->HideChat(HideChat);

	uavRequested_  = 0;
	uavViewActive_ = false;

	m_needToDrawBarricadePlacement = 0;
	m_BarricadeMeshPlacement = NULL;
	m_canPlaceConsumableItem = false;

	m_SpeedBoost = 0;
	m_SpeedBoostTime = 0;
	m_MorhineShotTime =0;
	m_BandagesEffectTime = 0;

	m_SelectedWeapon = HANDS_WEAPON_IDX;

	wiCharDataFull slot = CurLoadout;

	for (int i=0;i<slot.BackpackSize;i++)
	{
		if (slot.Items[i].itemID != WeaponConfig::ITEMID_UnarmedMelee)
		{
			slot.Items[i].Reset();
		}
	}
	slot.Attachment[0].Reset();
	slot.Attachment[1].Reset();
	
	slot.BackpackID = 20176;
	UpdateLoadoutSlot(slot, -1);

	CurrentSpread = 0;

	isGameHardcore = gClientLogic().m_gameInfo.mapId==9?true:false;//gamehardcore
	HardMaxPlayers = gClientLogic().m_gameInfo.maxPlayers;//gamehardcore	

	if(NetworkLocal)
	{
		extern int g_CCBlackWhite;
		extern float g_fCCBlackWhitePwr;
		g_CCBlackWhite = false;
		g_fCCBlackWhitePwr = 0.0f;
		g_pPostFXChief->ResetBuffers();

		extern bool g_AutoRunToggled;
		g_AutoRunToggled = false; // do not auto run automatically

		// otherwise when you are in scope mode you might not see your own player, and so scope will not render. Anyway, we should see our player always
		setSkipOcclusionCheck(true);
		ObjFlags |=	OBJFLAG_ForceSceneBoxBBox | OBJFLAG_AlwaysDraw; 
		SAFE_DELETE(PhysicsObject);
		PhysicsConfig.group = PHYSCOLL_CHARACTERCONTROLLER;
		PhysicsConfig.type = PHYSICS_TYPE_CONTROLLER;
		PhysicsConfig.mass = 100.0f;
        PhysicsConfig.controllerBehaviorCallback = true;
		PhysicsConfig.ready = true;
		PhysicsObject = BasePhysicsObject::CreateCharacterController(PhysicsConfig, this);

		if(s_laserPointerTex == 0)
		{
			s_laserPointerTex = r3dRenderer->LoadTexture("data/shaders/texture/laserPointerDot.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem );
			r3d_assert(s_laserPointerTex!=NULL);
		}

		if(s_UAVFriendlyBlinkTex == 0)
		{
			s_UAVFriendlyBlinkTex = r3dRenderer->LoadTexture("data/shaders/texture/PlayerBlinkSphere.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);;
			r3d_assert(s_UAVFriendlyBlinkTex!=NULL);
		}

		if(s_flashlightFlashTex == 0)
		{
			s_flashlightFlashTex = r3dRenderer->LoadTexture("data/shaders/texture/Light_Flashlight.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
			r3d_assert(s_flashlightFlashTex!=NULL);
		}

		if(s_laserFlashTex == 0)
		{
			s_laserFlashTex = r3dRenderer->LoadTexture("data/shaders/texture/Light_Laser.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
			r3d_assert(s_laserFlashTex!=NULL);
		}
	
		if(s_lightHaloTex == 0)
		{
			s_lightHaloTex = r3dRenderer->LoadTexture("data/shaders/texture/Light_Halo_01.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem);
			r3d_assert(s_lightHaloTex!=NULL);
		}

		m_HitMarkerTex = r3dRenderer->LoadTexture("data/weapons/hitmarker.dds", D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem );
		r3d_assert(m_HitMarkerTex != NULL);
	}
	else
	{
		SAFE_DELETE(PhysicsObject);
		PhysicsConfig.group = PHYSCOLL_CHARACTERCONTROLLER;
		PhysicsConfig.type = PHYSICS_TYPE_RAYCAST_BOX;
		PhysicsConfig.mass = 100.0f;
		// NOTE : if you change this, sync with PHYSICS_TYPE_CONTROLLER offset...
		PhysicsConfig.offset = r3dPoint3D( 0, 0.9f, 0 );
		PhysicsObject = BasePhysicsObject::CreateDynamicObject(PhysicsConfig, this);
	}

	m_Stamina = getMaxStamina();
	m_Breath = getMaxBreath();

	// clear death anim
	uberAnim_->anim.AnimTracks.clear();

	DoRagdoll(false);

	PlayerState = PLAYER_IDLE;
	SyncAnimation(true);

	// force animation update here, so that phys skeleton can be updated, otherwise creating too many of them at 0,0,0 causing physX to crash (too many pairs created)
	{
		D3DXMATRIX mr;
		D3DXMatrixRotationY(&mr, R3D_PI);
		D3DXMATRIX CharDrawMatrix = mr;

		UpdateAnimations(CharDrawMatrix);
		UpdateRotation();
	}

	// create phys skeleton after we have animation data
	if(!m_disablePhysSkeleton)
	{
		m_PhysSkeleton = AquireCacheSkeleton() ;
		m_PhysSkeleton->linkParent(GetSkeleton(), DrawFullMatrix, this, NetworkLocal ? PHYSCOLL_LOCALPLAYER : PHYSCOLL_NETWORKPLAYER) ;
		m_PhysSkeleton->SwitchToRagdoll(false);
	}

	if(NetworkLocal && hudMain)
	{
		hudMain->setTPSReticleVisibility(g_camera_mode->GetInt()==0?1:0);
		hudMain->isShowingYouAreDead = false;
	}

	TeleportPlayer(GetPosition(), "ResPawnFast");

}

void obj_Player::Undie()
{
	afterRespawnTimer = 2.0f;

	bDead = 0;

	// clear death anim
	uberAnim_->anim.AnimTracks.clear();

	DoRagdoll(false);

	PlayerState = PLAYER_IDLE;
	SyncAnimation(true);

}

void obj_Player::ProcessSpawnUAV()
{
	// can spawn UAV only one time per session
	if(uavRequested_ == 1 || uavRequested_ == 2)
	{
		if(hudMain) hudMain->showMessage(gLangMngr.getString("$HUD_Msg_UAVOnline"));
		return;
	}
	if(uavRequested_ == 3)
	{
		if(hudMain) hudMain->showMessage(gLangMngr.getString("$HUD_Msg_UAVOffline"));
		return;
	}
	
	// dont spawn UAV if no item
	if(m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getItemID() != WeaponConfig::ITEMID_Cypher2) {
		return;
	}

	r3d_assert(!uavRequested_);

	r3dPoint3D pos;
	if(!GetUAVSpawnPos(&pos))
	{
		if(hudMain) hudMain->showMessage(gLangMngr.getString("$HUD_Msg_NoUAVSpace"));
		return;
	}

	uavRequested_ = 1;

#ifndef FINAL_BUILD
	if(g_bEditMode)
	{
		// spawn test uav in editor
		GameObject* uav = srv_CreateGameObject("obj_UAV", "uav", pos);
		uav->ownerID = GetSafeID();
		uav->NetworkLocal = true;
		uav->SetNetworkID(0x454444);
		uavId_ = uav->GetSafeID();
		return;
	}
#endif

	r3d_assert(NetworkLocal);
	localPlayer_UseItem(m_Weapons[m_SelectedWeapon]->m_BackpackIdx, m_Weapons[m_SelectedWeapon]->getItemID(), pos, m_fPlayerRotationTarget);
	/*PKT_C2C_PlayerUseItem_s n;
	n.dbg_ItemID = WeaponConfig::ITEMID_Cypher2;
	n.pos     = pos;
	n.var1    = m_fPlayerRotationTarget;
	p2pSendToHost(this, &n, sizeof(n), true);*/
	return;
}

void obj_Player::ToggleUAVView(bool force_disable)
{
	r3d_assert(NetworkLocal);
	if(hudPause && hudPause->isActive())
		return;

	GameObject* uav = GameWorld().GetObject(uavId_);
	if(uavViewActive_ || force_disable)
	{
		uavViewActive_ = false;
		if (hudMain) hudMain->HideMenus(false);
		r_hud_filter_mode->SetInt(HUDFilter_Default);
		if(uav) uav->ObjFlags &= ~OBJFLAG_DisableShadows;
		if(uav) uav->ObjFlags &= ~OBJFLAG_SkipDraw;	// need that
	} 
	else if(uav)
	{
		uavViewActive_ = true;
		if (g_camera_mode->GetInt()==2)
		{
				switchFPS_TPS();
		}
		if (hudMain) hudMain->HideMenus(true);
		r_hud_filter_mode->SetInt(HUDFilter_CameraDrone);
		uav->ObjFlags |= OBJFLAG_DisableShadows;
		uav->ObjFlags |= OBJFLAG_SkipDraw;	// need that, because camera is inside body
	}
	
}

bool obj_Player::GetUAVSpawnPos(r3dPoint3D* pos)
{
	r3dPoint3D uavPos = GetPosition() + GetvForw() * 1.0f;
	uavPos.y += 4;

	// loop 100m up to check if we can spawn uav
	for(int i=0; i<100; i++)
	{
		// half sizes from obj_Uav mesh
		float uavSizeX = 4.7f/2;
		float uavBelowSpace = 3.0f;  // needed free meters below UAV
		float uavTopSpace   = 10.0f; // needed free meters above UAV
		
		PxBoxGeometry bbox(uavSizeX, (uavBelowSpace+uavTopSpace)*0.5f, uavSizeX);
		PxTransform pose(PxVec3(uavPos.x, uavPos.y+3.0f, uavPos.z), PxQuat(0,0,0,1));
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_PLAYER_COLLIDABLE_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
		PxShape* shape;
		if(!g_pPhysicsWorld->PhysXScene->overlapAny(bbox, pose, shape, filter))
		{
			*pos = uavPos;
			return true;
		}

		uavPos.y += 1.0f;
	}

	*pos = uavPos;
	return false;
}

static void dumpAnim(r3dAnimation& anim)
{
  for(size_t i=0; i<anim.AnimTracks.size(); i++) 
  {
    const r3dAnimation::r3dAnimInfo& ai = anim.AnimTracks[i];
          
    char st[256] = "";
    if(ai.GetStatus() & ANIMSTATUS_Playing) strcat(st, "Play ");
    if(ai.GetStatus() & ANIMSTATUS_Paused) strcat(st, "Pause ");
    if(ai.GetStatus() & ANIMSTATUS_Finished) strcat(st, "Finish ");
    if(ai.GetStatus() & ANIMSTATUS_Fading) strcat(st, "Fade ");
    if(ai.GetStatus() & ANIMSTATUS_Expiring) strcat(st, "Expire ");
          
    r3dOutToLog("%d: %s %f, %X %s\n", ai.GetTrackID(), ai.GetAnim()->GetAnimName(), ai.GetInfluence(), ai.GetStatus(), st);
  }
}

void obj_Player::CreateUberAnim()
{

	uberAnim_ = game_new CUberAnim(this, AI_Player_UberData);

	boneId_Bip01_Spine1    = uberAnim_->GetBoneID("Bip01_Spine1");
	boneId_Bip01_Spine2    = uberAnim_->GetBoneID("Bip01_Spine2");
	boneId_Bip01_Neck      = uberAnim_->GetBoneID("Bip01_Neck");
	boneId_Bip01_LUpperArm = uberAnim_->GetBoneID("Bip01_L_UpperArm");
	boneId_Bip01_RUpperArm = uberAnim_->GetBoneID("Bip01_R_UpperArm");
	boneId_Bip01_Head      = uberAnim_->GetBoneID("Bip01_Head");
	boneId_Bip01_R_Hand	   = uberAnim_->GetBoneID("Bip01_R_Hand");
	boneId_Bip01_L_Hand	   = uberAnim_->GetBoneID("Bip01_L_Hand");
}

void obj_Player::EDITOR_RecreateUberAnim()
{
	delete uberAnim_;

	CreateUberAnim();

	uberAnim_->CurrentWeapon = NULL;
	SyncAnimation(true);
}

r3dSkeleton* obj_Player::GetSkeleton()
{
	return uberAnim_->anim.pSkeleton;
}

r3dAnimation& obj_Player::GetAnimation()
{
	return uberAnim_->anim;
}

void obj_Player::SyncAnimation(bool force)
{
	R3DPROFILE_FUNCTION("obj_Player::SyncAnimation");
	if(bDead)
		return;

	const Weapon* wpn = m_Weapons[m_SelectedWeapon];
	bool isInAttmMenu = false;
	if(wpn && hudAttm && hudAttm->isActive() && wpn->getCategory()>=storecat_ASR && wpn->getCategory()<=storecat_SMG)
		isInAttmMenu = true;

	uberAnim_->SyncAnimation(PlayerState, PlayerMoveDir, force, wpn, isInAttmMenu); // firstly sync to update weapon pointer and only then update jump
	uberAnim_->UpdateJump(bOnGround);
}

static float accumLastDamageRcvd = 0;
void obj_Player::ApplyDamage(const r3dPoint3D& dmgPos, float damage, GameObject* fromObj, int bodyBone, int dmgType)
{
	lastTimeHit = r3dGetTime();
	lastTimeHitBone = bodyBone;
	lastDamageRcvd = damage;
	accumLastDamageRcvd += damage;

	/*if (fromObj == gClientLogic().localPlayer_)
	{
		if(bodyBone == uberAnim_->GetBoneID("Bip01_Head"))
			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/BulletHits/Hit_Headshot"), GetPosition());
		else if(dmgType==storecat_MELEE)
			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/WarZ/Impacts/Impacts-KnifeStab"), GetPosition());		
		else
			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/WarZ/Impacts/Impacts-Body"), GetPosition());		
	}*/

	// apply ragdoll force from object position, or up if too close
	r3dPoint3D ragHitPos = GetPosition();
	lastTimeHitForce = r3dPoint3D(0, 1, 0);
	if(bodyBone < GetSkeleton()->NumBones)
		ragHitPos = uberEquip_->getBonePos(bodyBone, GetSkeleton(), DrawFullMatrix);
	if((ragHitPos - fromObj->GetPosition()).LengthSq() > 0.001f)
		lastTimeHitForce = (ragHitPos - fromObj->GetPosition()).NormalizeTo();

	float dmgForce = 10.0f;
	switch(dmgType)
	{
	case storecat_ASR:
		dmgForce = 30.0f;
		break;
	case storecat_SNP:
		dmgForce = 60.0f;
		break;
	case storecat_SHTG:
		dmgForce = 60.0f;
		break;
	case storecat_MG:
		dmgForce = 40.0f;
		break;
	case storecat_HG:
		dmgForce = 10.0f;
		break;
	case storecat_SMG:
		dmgForce = 20.0f;
		break;
	}
	lastTimeHitForce *= dmgForce;

#ifndef FINAL_BUILD
	if(NetworkLocal && m_isAiming)
	{
		float recoil = 3.5f;
		RecoilViewModTarget2.x = recoil * (random(2)==1?u_GetRandom(-1.0f, -0.5f):u_GetRandom(0.5f, 1.0f));
		RecoilViewModTarget2.y = recoil * (random(2)==1?u_GetRandom(-1.0f, -0.5f):u_GetRandom(0.5f, 1.0f));
	}
#endif

#ifdef VEHICLES_ENABLED
	if (isHitByVehicle)
	{
		isHitByVehicle = false;

		if (!bDead)
			StartPushFromVehicle(4, 32);
	}
#endif
}

bool obj_Player::ChangeWeaponByIndex(int index)
{
	r3d_assert(index>=0 && index < NUM_WEAPONS_ON_PLAYER);
	if(m_SelectedWeapon == index)
		return false;

#ifdef VEHICLES_ENABLED
	if (IsInVehicle())
		return false;
#endif

	m_needToDrawBarricadePlacement = 0;
	m_BarricadeMeshPlacement = NULL;
	m_canPlaceConsumableItem = false;
	lastTimeWeaponSwitch = r3dGetTime();
	lastTimeAirHornUse = 0.0f;
	m_ZombieRepelentTime = 0;

	m_PrevSelectedWeapon = m_SelectedWeapon;
	m_SelectedWeapon = index;

	uberAnim_->StopReloadAnim();

	if(m_PrevSelectedWeapon >= 0 && m_Weapons[m_PrevSelectedWeapon])
	{
		m_Weapons[m_PrevSelectedWeapon]->OnUnequip();
	}
	
	// override to hands weapon if there is no selected weapon
	if(m_Weapons[m_SelectedWeapon] == NULL)
		m_SelectedWeapon = HANDS_WEAPON_IDX;

	r3d_assert(m_Weapons[m_SelectedWeapon]);
	m_Weapons[m_SelectedWeapon]->OnEquip(isFlashlightOn);

	if(hudMain && NetworkLocal)
	{
		showSlotsInfoTimer = r3dGetTime() + 5.0f;
		hudMain->showSlots(true);

		updateWeaponUI();
	}

	UpdateCharWeaponMeshes();

	SyncAnimation(true);

	if(NetworkLocal)
	{
		PKT_C2C_PlayerSwitchWeapon_s n;
		n.wid = m_SelectedWeapon;
		p2pSendToHost(this, &n, sizeof(n), true);
	}

	return true;
}

void obj_Player::UpdateCharWeaponMeshes()
{
	//r3d_assert(!m_Weapons[m_SelectedWeapon].Empty());

	bool isFirstPerson = g_camera_mode->GetInt()==2 && NetworkLocal;
	uberEquip_->SetSlot(SLOT_Weapon, m_Weapons[m_SelectedWeapon]);
	if(m_SelectedWeapon==0)
	{
		uberEquip_->SetSlot(SLOT_WeaponBackRight, (Weapon*)NULL);
	}
	else
	{
		if(m_Weapons[0])
		{
			uberEquip_->SetSlot(SLOT_WeaponBackRight, m_Weapons[0]);
		}
	}
	if(m_SelectedWeapon==1)
		uberEquip_->SetSlot(SLOT_WeaponSide, (Weapon*)NULL);
	else
	{
		if(m_Weapons[1])
			uberEquip_->SetSlot(SLOT_WeaponSide, m_Weapons[1]);
	}
}

void obj_Player::ReloadWeaponFromSlot(int wpnSlot, int ammoSlot, int amount)
{
	r3d_assert(NetworkLocal);
	
	// remove clip (SERVER CODE SYNC POINT)
	wiInventoryItem& wi = CurLoadout.Items[ammoSlot];
	r3d_assert(wi.quantity > 0);
	wi.quantity--;
	if(wi.quantity <= 0)
		wi.Reset();
		
	OnBackpackChanged(ammoSlot);

	PKT_C2C_PlayerReload_s n;
	n.WeaponSlot = (BYTE)wpnSlot;
	n.AmmoSlot   = (BYTE)ammoSlot;
	n.dbg_Amount = (BYTE)amount;
	p2pSendToHost(this, &n, sizeof(n));
	
	return;
}

void obj_Player::UnloadWeaponClip(int wpnSlot)
{
	r3d_assert(NetworkLocal);
	
	wiInventoryItem& bckItm = CurLoadout.Items[wpnSlot];
	if(bckItm.Var1 == 0)
		return;
	
	const WeaponConfig* wcfg = g_pWeaponArmory->getWeaponConfig(bckItm.itemID);
	if(wcfg == NULL || wcfg->category == storecat_MELEE || wcfg->category == storecat_GRENADE || wcfg->category == storecat_UsableItem)
		return;
	
	PKT_C2S_PlayerUnloadClip_s n;
	n.WeaponSlot = wpnSlot;
	p2pSendToHost(this, &n, sizeof(n));
	
	// clear ammo inside weapon, server will drop clip with ammo
	bckItm.Var1 = 0;
	
	return;
}

float obj_Player::getMaxStamina()
{
	float maxStamina = GPP->c_fSprintMaxEnergy;
	return maxStamina;
}

float obj_Player::getMaxBreath()
{
	float maxBreath = GPP->c_fSniperRifleHoldBreathTime;
	return maxBreath;
}

bool obj_Player::IsSwimming()
{
	return bIsSwimming;
}

bool obj_Player::IsInWater( float& waterDepth, float& waterLevel )
{
	const r3dPoint3D& pos = GetPosition();
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
	bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(pos.x, pos.y + 0.5f, pos.z), PxVec3(0, -1, 0), 500.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
	r3dPoint3D posForWater = pos;
	if( !hitResult )
	{
		// Can't find the ground, assume false
		waterDepth = -100;
		waterLevel = -100;
		return false;
	}

	posForWater = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);
	waterDepth = getWaterDepthAtPos(posForWater);
	waterLevel = hit.impact.y + waterDepth;

	// In the water
	if( waterDepth >= 0 )
		return true;

	// Above the water
	return false;
}

#ifdef MISSIONS
void obj_Player::UpdateMissionActionData( const PKT_S2C_MissionActionUpdate_s& actionData )
{
	MissionActionData data;
	data.missionID		= actionData.missionID;
	data.actionID		= actionData.actionID;
	data.actionType		= actionData.actionType & 0x0000FFFF;
	data.subActionType	= (actionData.actionType & 0xFFFF0000) >> 16;
	data.amount			= actionData.amount;
	data.count			= actionData.count;
	data.itemID			= actionData.itemID;
	data.objType		= actionData.objType;
	data.position		= actionData.position;
	data.extents		= actionData.extents;
	data.inArea			= false;
	data.completed		= actionData.completed;
	r3dscpy( data.missionName, actionData.missionName );

	//r3dOutToLog( "Missions Action Data(%d): %s contain Mission(%d), Action(%d)\n",
	//	missionsActionData.size(),
	//	(missionsActionData.find( actionData.actionID ) != missionsActionData.end()) ? "Does" : "Does NOT",
	//	data.missionID, data.actionID );

	if( NetworkLocal && hudMain )
	{
		const char* missionName = gLangMngr.getString(data.missionName);
		char progress[ 32 ];
		char actionText[ 128 ];
		switch( data.actionType )
		{
		case 1:	// ACT_Goto
			sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_Goto") );
			sprintf( progress, "\0" );
			break;
		case 2: // ACT_Item
			{
				char* storeName;
				const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(data.itemID);
				if( cfg )
					storeName = cfg->m_StoreName;
				else
					storeName = "\0";
				
				switch( data.subActionType )
				{
				case 1: sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_ItemCollect"), data.amount, storeName ); break;	// ITEM_Collect
				case 2: sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_ItemDrop"), data.amount, storeName ); break;		// ITEM_Drop
				case 3: sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_ItemUse"), data.amount, storeName ); break;		// ITEM_Use
				case 4: sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_ItemCraft"), data.amount, storeName ); break;		// ITEM_Craft
				case 5: sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_ItemDeliver"), data.amount, storeName ); break;	// ITEM_Deliver
				default:
#ifndef FINAL_BUILD
					sprintf( actionText, "Unknown Item Action(%d)", data.subActionType ); break;
#else
					actionText[ 0 ] = '\0';
#endif
				}
			}
			sprintf( progress, "%d/%d", data.count, data.amount );
			break;
		case 3: // ACT_Kill
			{
				if( data.amount > 1 )
				{
					switch( data.objType )
					{
					default:
					case OBJTYPE_Zombie:
						if( data.itemID == UINT_MAX )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillZombiesWithWeapons"), data.amount );
						}
						else if( data.itemID == 0 )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillZombies"), data.amount );
						}
						else
						{
							char* storeName;
							const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(data.itemID);
							if( cfg )
								storeName = cfg->m_StoreName;
							else
								storeName = "\0";
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillZombiesWithWeapon"), data.amount, storeName );
						}
						break;
					case OBJTYPE_Human:
						if( data.itemID == UINT_MAX )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillPlayersWithWeapons"), data.amount );
						}
						else if( data.itemID == 0 )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillPlayers"), data.amount );
						}
						else
						{
							char* storeName;
							const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(data.itemID);
							if( cfg )
								storeName = cfg->m_StoreName;
							else
								storeName = "\0";
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillPlayersWithWeapon"), data.amount, storeName );
						}
						break;
					}
				}
				else
				{
					switch( data.objType )
					{
					default:
					case OBJTYPE_Zombie:
						if( data.itemID == UINT_MAX )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillZombieWithWeapons") );
						}
						else if( data.itemID == 0 )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillZombie") );
						}
						else
						{
							char* storeName;
							const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(data.itemID);
							if( cfg )
								storeName = cfg->m_StoreName;
							else
								storeName = "\0";
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillZombieWithWeapon"), storeName );
						}
						break;
					case OBJTYPE_Human:
						if( data.itemID == UINT_MAX )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillPlayerWithWeapons") );
						}
						else if( data.itemID == 0 )
						{
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillPlayer") );
						}
						else
						{
							char* storeName;
							const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(data.itemID);
							if( cfg )
								storeName = cfg->m_StoreName;
							else
								storeName = "\0";
							sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_KillPlayerWithWeapon"), storeName );
						}
						break;
					}
				}
			}
			sprintf( progress, "%d/%d", data.count, data.amount );
			break;
		case 4: // ACT_State
			switch( data.subActionType )
			{
			case 1: sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_StateSetInactive") ); break;	// STATE_Inactive
			case 2: sprintf( actionText, gLangMngr.getString("HUD_Info_MissionAction_StateSetActive") ); break;	// STATE_Active
			default:
#ifndef FINAL_BUILD
				sprintf( actionText, "Unknown Action State(%d)", data.subActionType ); break;
#else
				actionText[ 0 ] = '\0';
#endif
			}
			sprintf( progress, "%s", (data.count > 1) ? gLangMngr.getString("HUD_Info_MissionAction_StateProgressActive") : gLangMngr.getString("HUD_Info_MissionAction_StateProgressInactive") );
			break;
		}

		// Obtain MissionActionData
		std::map< uint32_t, MissionActionData >::iterator mapIter = missionsActionData.find( data.actionID );
		if( mapIter == missionsActionData.end() )
		{
			bool hasAreaRestriction = false;
			if( data.extents.x >= 0.5f || data.extents.y >= 0.5f || data.extents.z >= 0.5f )
				hasAreaRestriction = true;

			if( std::find(acceptedMissions.begin(), acceptedMissions.end(), data.missionID) == acceptedMissions.end() )
				acceptedMissions.push_back( data.missionID );
			hudMain->addMissionInfo( data.missionID, missionName );
			hudMain->addMissionAction( data.missionID, data.actionID, data.completed, actionText, progress, hasAreaRestriction);

			// In Area Restriction
			if( hasAreaRestriction )
			{
				const r3dPoint3D& pos			= GetPosition();
				const r3dPoint3D& areaPos		= data.position;
				const r3dVector& areaExtents	= data.extents;
				if( pos.x <= (areaPos.x + areaExtents.x)  && pos.x >= (areaPos.x - areaExtents.x) &&
					pos.y <= (areaPos.y + areaExtents.y)  && pos.y >= (areaPos.y - areaExtents.y) &&
					pos.z <= (areaPos.z + areaExtents.z)  && pos.z >= (areaPos.z - areaExtents.z) )
				{
					data.inArea = true;
					hudMain->setMissionActionInArea( data.missionID, data.actionID, data.inArea );
				}
			}
			//r3dOutToLog("Mission(%d): New Action(%d) - Progress:%s, Completed:%s, InArea:%s, Extents:(%.2f, %.2f, %.2f), Position<%.2f, %.2f, %.2f>, PlrPos<%.2f, %.2f, %.2f>\n",
			//	data.missionID, data.actionID,
			//	progress,
			//	(data.completed) ? "True" : "False",
			//	(data.inArea) ? "True" : "False",
			//	data.extents.x, data.extents.y, data.extents.z,
			//	data.position.x, data.position.y, data.position.z,
			//	GetPosition().x, GetPosition().y, GetPosition().z);
		}
		else
		{
			// Completed
			if( mapIter->second.completed != data.completed )
			{
				static int sndActionCompleteID = SoundSys.GetEventIDByPath("Sounds/WarZ/Mission/ActionComplete");
				hudMain->setMissionActionComplete( data.missionID, data.actionID, data.completed );
				SoundSys.PlayAndForget(sndActionCompleteID, GetPosition());
			}

			// Progress
			if( mapIter->second.count != data.count )
			{
				hudMain->setMissionActionProgress( data.missionID, data.actionID, progress );
			}

			// In Area Restriction
			bool bPrevCompletedValue = mapIter->second.completed;
			bool bPrevInAreaValue = mapIter->second.inArea;
			if( mapIter->second.extents.x >= 0.5f || mapIter->second.extents.y >= 0.5f || mapIter->second.extents.z >= 0.5f )
			{
				bool bInsideArea = false;
				const r3dPoint3D& pos			= GetPosition();
				const r3dPoint3D& areaPos		= mapIter->second.position;
				const r3dVector& areaExtents	= mapIter->second.extents;
				if( pos.x <= (areaPos.x + areaExtents.x)  && pos.x >= (areaPos.x - areaExtents.x) &&
					pos.y <= (areaPos.y + areaExtents.y)  && pos.y >= (areaPos.y - areaExtents.y) &&
					pos.z <= (areaPos.z + areaExtents.z)  && pos.z >= (areaPos.z - areaExtents.z) )
					bInsideArea = true;
				if( mapIter->second.inArea != bInsideArea )
				{
					data.inArea = bInsideArea;
					hudMain->setMissionActionInArea( data.missionID, data.actionID, data.inArea );
				}
			}
			//r3dOutToLog("Mission(%d): Update Action(%d) - Progress:%s, Completed:%s/%s, InArea:%s/%s, Extents:(%.2f, %.2f, %.2f), Position<%.2f, %.2f, %.2f>, PlrPos<%.2f, %.2f, %.2f>\n",
			//	data.missionID, data.actionID,
			//	progress,
			//	(bPrevCompletedValue) ? "True" : "False",
			//	(data.completed) ? "True" : "False",
			//	(bPrevInAreaValue) ? "True" : "False",
			//	(data.inArea) ? "True" : "False",
			//	data.extents.x, data.extents.y, data.extents.z,
			//	data.position.x, data.position.y, data.position.z,
			//	GetPosition().x, GetPosition().y, GetPosition().z);
		}

		// This will either insert or update, depending on whether it exists already.
		missionsActionData[ actionData.actionID ] = data;
	}
}

void obj_Player::UpdateMissionAreaRestrictions()
{
	if( NetworkLocal && hudMain )
	{
		for( std::map< uint32_t, MissionActionData >::iterator mapIter = missionsActionData.begin();
			 mapIter != missionsActionData.end(); ++mapIter )
		{
			if( mapIter->second.extents.x >= 0.5f || mapIter->second.extents.y >= 0.5f || mapIter->second.extents.z >= 0.5f )
			{
				// In Area Restriction
				bool bInsideArea = false;
				const r3dPoint3D& pos			= GetPosition();
				const r3dPoint3D& areaPos		= mapIter->second.position;
				const r3dVector& areaExtents	= mapIter->second.extents;
				if( pos.x <= (areaPos.x + areaExtents.x)  && pos.x >= (areaPos.x - areaExtents.x) &&
					pos.y <= (areaPos.y + areaExtents.y)  && pos.y >= (areaPos.y - areaExtents.y) &&
					pos.z <= (areaPos.z + areaExtents.z)  && pos.z >= (areaPos.z - areaExtents.z) )
					bInsideArea = true;
				//bool bPrevCompletedValue = mapIter->second.completed;
				//bool bPrevInAreaValue = mapIter->second.inArea;
				if( mapIter->second.inArea != bInsideArea )
				{
					mapIter->second.inArea = bInsideArea;
					hudMain->setMissionActionInArea( mapIter->second.missionID, mapIter->second.actionID, mapIter->second.inArea );
				}
				//r3dOutToLog("Mission(%d): Update Action(%d) - Progress:%s, Completed:%s/%s, InArea:%s/%s, Extents:(%.2f, %.2f, %.2f), Position<%.2f, %.2f, %.2f>, PlrPos<%.2f, %.2f, %.2f>\n",
				//	mapIter->second.missionID, mapIter->second.actionID,
				//	"N/A",
				//	(bPrevCompletedValue) ? "True" : "False",
				//	(mapIter->second.completed) ? "True" : "False",
				//	(bPrevInAreaValue) ? "True" : "False",
				//	(mapIter->second.inArea) ? "True" : "False",
				//	mapIter->second.extents.x, mapIter->second.extents.y, mapIter->second.extents.z,
				//	mapIter->second.position.x, mapIter->second.position.y, mapIter->second.position.z,
				//	GetPosition().x, GetPosition().y, GetPosition().z);
			}
		}
	}
}

void obj_Player::RemoveMissionActionData( uint32_t missionID, uint32_t reason )
{
	if( NetworkLocal && hudMain )
	{
		static int sndMissionCompleteID = SoundSys.GetEventIDByPath("Sounds/WarZ/Mission/MissionComplete");
		switch( reason )
		{
		default:
		case 0:		// RMV_Administration
			hudPause->removeMissionButton( missionID, reason );
			break;
		case 1:		// RMV_MissionCompleted
#ifndef FINAL_BUILD
			if( !d_enable_mission_repeat->GetBool() )
				hudPause->removeMissionButton( missionID, reason );
#endif
			hudMain->showMessage(gLangMngr.getString("HUD_Msg_MissionCompleted"));
			SoundSys.PlayAndForget(sndMissionCompleteID, GetPosition());
			break;
		case 2:		// RMV_MissionAbandoned
			break;
		case 3:		// RMV_MissionExpired
			hudPause->removeMissionButton( missionID, reason );
			hudMain->showMessage(gLangMngr.getString("HUD_Msg_MissionExpired"));
			break;
		}

		for( std::map< uint32_t, MissionActionData >::iterator mapIter = missionsActionData.begin();
			 mapIter != missionsActionData.end(); )
		{
			if( mapIter->second.missionID == missionID )
				missionsActionData.erase( mapIter++ );
			else
				++mapIter;
		}
		std::vector<uint32_t>::const_iterator iter = std::find(acceptedMissions.begin(), acceptedMissions.end(), missionID);
		if( iter != acceptedMissions.end() )
			acceptedMissions.erase( iter );
		hudMain->removeMissionInfo( missionID );
	}
}
#endif

#ifdef VEHICLES_ENABLED
void obj_Player::StartPushFromVehicle(float vehicleSpeed, float maxSpeed)
{
	const float maxVel = 4;
	pushVelocity = (vehicleSpeed / maxSpeed) * maxVel;
	pushStartTime = r3dGetTime();
}

bool obj_Player::IsPushActive()
{
	return pushVelocity > 0;
}
#endif

void obj_Player::StartJump()
{
	r3d_assert(NetworkLocal);

	//check if we are joing to jump into water, cancel jump in that case
	{
		r3dPoint3D vel    = r3dPoint3D(0,0,0);
		vel    += (GetvForw()  * InputAcceleration.Z);
		vel    += (GetvRight() * InputAcceleration.X);
		vel.Y  += InputAcceleration.Y;

		r3dPoint3D newPos = vel + GetPosition();
		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
		bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(newPos.x, newPos.y + 0.5f, newPos.z), PxVec3(0, -1, 0), 10.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
		r3dPoint3D posForWater = newPos;
		if( hitResult )
			posForWater = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);			

		float waterDepth = getWaterDepthAtPos(posForWater);
		float allowedDepth = 1.5f;
		if(waterDepth > allowedDepth) // too deep
		{
			return; // cancel jump
		}
	}
	
	uberAnim_->StartJump();
	JumpVelocity  = 17;
#ifndef FINAL_BUILD
	if(d_enable_super_jump->GetInt())
		JumpVelocity = (float)d_enable_super_jump->GetInt();
#endif
	JumpStartTime = r3dGetTime() + uberAnim_->jumpStartTime;

	PKT_C2C_PlayerJump_s n;
	p2pSendToHost(this, &n, sizeof(n));

	SoundSys.PlayAndForget(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_JUMPING_F":"Sounds/WarZ/PlayerSounds/PLAYER_JUMPING_M"), GetPosition());
}

bool obj_Player::IsJumpActive()
{
	// going up on jump, or in air.
	return (JumpVelocity > 0 || !bOnGround);
}

void obj_Player::DetectIfOnGround()
{
	//static bool prevOnGround = bOnGround;
	
	{	
		PxSweepHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
		PxBoxGeometry boxg(0.3f, 0.1f, 0.3f);
		PxTransform pose(PxVec3(GetPosition().x, GetPosition().y+0.5f, GetPosition().z));
		if(g_pPhysicsWorld->PhysXScene->sweepSingle(boxg, pose, PxVec3(0,-1,0), 1.0f, PxSceneQueryFlag::eDISTANCE, hit, filter))
		{	
			bOnGround = (hit.distance < 0.6);	
			fHeightAboveGround = hit.distance - 0.5f;
		}
		else
		{
			bOnGround = false;
			fHeightAboveGround = 10.0f;
		}
	}

	/*PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
	if(g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y + 0.5f, GetPosition().z), PxVec3(0, -1, 0), 1.0f, PxSceneQueryFlags(PxSceneQueryFlag::eDISTANCE), hit, filter))
	{
		bOnGround = (hit.distance < 0.6f);
		fHeightAboveGround = hit.distance - 0.5f;
	}
	else
	{

		bOnGround = false;
		fHeightAboveGround = 10.0f;
	}*/

	// hack to check if player got glued to some geometry while jumping. Happen if he jumped on something that has some bumps and character controller thinks that he can stay on that tiny bump
	if(!bOnGround && R3D_ABS(oldstate.Position.y - GetPosition().y) < 0.01f)
	{
		bOnGround = true;
		fHeightAboveGround = 0.0f;
	}
	
	//if(prevOnGround != bOnGround)
	//{
	//	prevOnGround = bOnGround;
	//	r3dOutToLog("onGround=%d\n", bOnGround);
	//}
}

void obj_Player::UpdateLocalPlayerMovement()
{
	r3d_assert(NetworkLocal);
	
	VMPROTECT_BeginMutation("obj_Player::UpdateLocalPlayerMovement");
	
	// do not allow movement in first few frames, Terrain3 will create physx chunks on it first Update()
	if(Terrain3 && PhysXTerra3WarmUp < 3)
		return;
	
	// important: limit movement to max for 1 sec. so server can check for teleports
	const float TimePassed = R3D_MIN(r3dGetFrameTime(), 0.1f);
	const float curTime    = r3dGetTime();

	RealAcceleration = InputAcceleration;

	// apply weight to moving speed before gravity and jumping
	float weightAdj = 1.0f + ((m_GearWeight/100.0f)); // each 100kg half your speed
	RealAcceleration = RealAcceleration/weightAdj;

	// locally add gravity to acceleration
	float Gravity = 9.81f;
	float deltaTimeInAir =(curTime-StartFallingTime);
	if(!bOnGround && JumpVelocity<=0) // don't accelerate falling while jumping, only when actually falling
		Gravity += Gravity * deltaTimeInAir;

	if(!IsSwimming())
		RealAcceleration.Y -= Gravity;

	if(JumpVelocity > 0 && curTime >= JumpStartTime)
	{
		JumpVelocity -= Gravity * TimePassed * 2.5f;
		RealAcceleration.Y += JumpVelocity;
		// reset falling time
		if(JumpVelocity<=0)
			StartFallingTime = curTime;
	}

#ifdef VEHICLES_ENABLED
	if (!IsInVehicle())
	{
		g_pPhysicsWorld->m_VehicleManager->cameraContoller.SilentlyFollowPlayer(GetPosition());

		if (pushVelocity > 0 && curTime >= pushStartTime)
		{
			pushVelocity -= TimePassed * 2.5f;

			RealAcceleration -= pushVelocity;
		}
	}
#endif

	if(bOnGround || IsSwimming())
		StartFallingTime = curTime; // keep reseting timer, otherwise when jump it will not be null

	r3dPoint3D vel    = r3dPoint3D(0,0,0);
	vel    += (GetvForw()  * RealAcceleration.Z);
	vel    += (GetvRight() * RealAcceleration.X);
	vel.Y  += RealAcceleration.Y;
	
	// send some info to server about fast movement
	if(!gUserProfile.ProfileData.isDevAccount)
	{
		static float lastUpd = -999;
		r3dPoint3D velXZ(vel.x, 0, vel.z);
		if(velXZ.Length() > 8.0f && curTime > lastUpd + 60.0f)
		{
			lastUpd = curTime;
			char buf[1024];
			sprintf(buf, "%.2f %.2f %.1f/%.1f %.2f %.1f", 
				velXZ.x, velXZ.z, GPP->AI_SPRINT_SPEED, GPP->AI_RUN_SPEED, TimePassed, weightAdj);

			PKT_C2S_DBG_LogMessage_s n;
			r3dscpy(n.msg, buf);
			p2pSendToHost(this, &n, sizeof(n));
		}
	}

	ControllerPhysObj* controller = (ControllerPhysObj*)PhysicsObject;

	controller->Move(vel * TimePassed, 0.5f);
	r3dPoint3D prevPos = GetPosition();
	parent::Update(); // manually update position after updating physics. Needed for camera, otherwise it will be lagging one frame behind

	// check if player is trying to get into water
	// use raycast to fix bug when player able to walk on water by jumping
	bool bWasSwimming = IsSwimming();
	{
		const float allowedDepth = 1.5f;
		const float crouchStopDepth = 1.0f;
		float waterDepth;
		float waterLevel;
		if(IsInWater( waterDepth, waterLevel ))
		{
			bIsSwimming = false;
			if((PlayerState == PLAYER_MOVE_CROUCH || PlayerState == PLAYER_MOVE_CROUCH_AIM) && waterDepth >= crouchStopDepth)
			{
				SetPosition(prevPos);
				parent::Update();
				SetVelocity(r3dPoint3D(0,0,0));
			}
			else if(waterLevel >= GetPosition().y && waterDepth > allowedDepth) // too deep, start swimming
			{
				// Force TPS camera mode, before setting bIsSwimming to true.
				if( g_camera_mode->GetInt() == 2 )
					switchFPS_TPS();

				bIsSwimming = true;

				// Keep the player from sinking too far into the water,
				// especially for first person view.
				if( ( waterLevel - GetPosition().y ) < ( waterLevel - allowedDepth ) )
					SetPosition( r3dPoint3D( GetPosition().x, waterLevel - allowedDepth, GetPosition().z ) );

				parent::Update();
				SetVelocity(r3dVector(vel.x, 0, vel.z));
				bool canSwitch = !(m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getCategory() == storecat_GRENADE && InputMappingMngr->isPressed(r3dInputMappingMngr::KS_PRIMARY_FIRE));
				if(m_SelectedWeapon != HANDS_WEAPON_IDX && canSwitch)
				{
					ChangeWeaponByIndex(HANDS_WEAPON_IDX);
					lastTimeWeaponSwitch = curTime;
				}
			}
		}
		else
		{
			bIsSwimming = false;

			// Force TPS camera mode, if FPS camera is underwater.
			if( g_camera_mode->GetInt() == 2 && (getPlayerHeightForCameraWithPlayerState() - 0.3f) < waterDepth )
				switchFPS_TPS();
			SetVelocity(vel);
		}
	}

	float ll = (prevPos-GetPosition()).LengthSq();
	if(ll > 0.0001f)
	{
		extern void WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx);
		WaterSplash(GetPosition() - 1.0f * ll, 2.0f, 5.0f, 0.1f*ll, 3);
	}

	bool bWasOnGround = bOnGround;
	DetectIfOnGround();

	if(bWasOnGround && !bOnGround && !IsSwimming())
	{
		StartFallingHeight = GetPosition().y;
		StartFallingTime = r3dGetTime();
	}
	if(!bWasOnGround && bOnGround && !bWasSwimming)
	{
		// calculate damage from falling if any
		float newHeight = GetPosition().y;
		if((StartFallingHeight - newHeight) > 15.0f) // more than 15 meters, we gonna break something (increased to 15 meters, as sometimes on downhill we do not detect properly that we are on the ground and don't want to break legacy code)
		{
			float damageAmount = ((StartFallingHeight - newHeight))*2.0f; // if falling more than 50 meters - dies immediatelly (damage >=100)
			damageAmount = R3D_CLAMP(damageAmount, 0.0f, 255.0f);

			// send damage to server
			PKT_C2S_FallingDamage_s n;
			n.damage = (BYTE)damageAmount;
			p2pSendToHost(this, &n, sizeof(n));

			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_GRUNTING_F":"Sounds/WarZ/PlayerSounds/PLAYER_GRUNTING_M"), GetPosition());
		}
	}

	// STAMINA MED SYSTEM SYNC POINT
	if(m_Stamina>0)
	{
		if(CurLoadout.MedBleeding > 0)
			m_Stamina -= TimePassed*GPP->c_fMedSys_Bleeding_Stamina;
		if(CurLoadout.MedFeverCold > 0)
			m_Stamina -= TimePassed*GPP->c_fMedSys_Fever_Stamina;
		if(CurLoadout.MedBloodInfection > 0)
			m_Stamina -= TimePassed*GPP->c_fMedSys_BloodInfection_Stamina;
	}
	// STAMINA MED SYSTEM SYNC POINT END

	// STAMINA LOGIC SHOULD BE SYNCED WITH SERVER CODE!
	if(PlayerState == PLAYER_MOVE_SPRINT || PlayerState == PLAYER_SWIM_FAST )
	{
		float d = TimePassed;
		if(CurLoadout.Skills[CUserSkills::SKILL_Physical2])
			d *= 0.95f;
		if(CurLoadout.Skills[CUserSkills::SKILL_Physical5])
			d *= 0.9f;

		m_Stamina = m_Stamina - d;
		if(m_Stamina <= 0)
		{
			m_StaminaPenaltyTime = 3.0f;
		}
	}
	else if(bOnGround || IsSwimming()) // regen only if standing on ground or swimming
	{
		float regen_rate = CurLoadout.Health<50?GPP->c_fSprintRegenRateLowHealth:GPP->c_fSprintRegenRateNormal;
		if(CurLoadout.Skills[CUserSkills::SKILL_Physical4])
			regen_rate *= 1.05f;
		if(CurLoadout.Skills[CUserSkills::SKILL_Physical6])
			regen_rate *= 1.10f;

		m_Stamina = m_Stamina + TimePassed*regen_rate; // regeneration rate
		if(m_StaminaPenaltyTime > 0.0f)
			m_StaminaPenaltyTime = m_StaminaPenaltyTime - TimePassed;
	}
	m_Stamina = R3D_CLAMP((float)m_Stamina, 0.0f, getMaxStamina());

	bool scopeRendering = hasScopeMode() && CurrentRig.allowScope;
	if(m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getCategory() == storecat_SNP && m_isFinishedAiming) 
	{
		if(hudMain)
		{
//			hudMain->SetEnergyValue(1.0f); // hide energy bar
//			hudMain->SetBreathValue(R3D_CLAMP((m_Breath/getMaxBreath()), 0.0f, 1.0f));
		}
	}
	else
	{
		if(hudMain)
		{
//			hudMain->SetBreathValue(2.0f); // hide breath
//			hudMain->SetEnergyValue(m_Stamina/getMaxStamina());
		}
	}

	VMPROTECT_End();
}

void obj_Player::localPlayer_UseItem(int slotID, uint32_t itemID, const r3dPoint3D& pos, float var1, float var2, float var3, DWORD var4)
{
	r3d_assert(NetworkLocal);

	PKT_C2C_PlayerUseItem_s n;
	n.SlotFrom  = (BYTE)slotID;
	n.dbg_ItemID= itemID;
	n.pos       = pos;
	n.var1		= var1;
	n.var2		= var2;
	n.var3		= var3;
	n.var4		= var4;
	p2pSendToHost(this, &n, sizeof(n), true);

	const BaseItemConfig* itmC = g_pWeaponArmory->getConfig(itemID);
	r3d_assert(itmC);

	if (itemID==WeaponConfig::ITEMID_Antibiotics||itemID==WeaponConfig::ITEMID_Bandages||itemID==WeaponConfig::ITEMID_Bandages2||
		itemID==WeaponConfig::ITEMID_Painkillers||itemID==WeaponConfig::ITEMID_Medkit||itemID==WeaponConfig::ITEMID_CraftedBandages)
	{
		lastTimeUsedConsumable = r3dGetTime();
		currentConsumableCooldownTime = GPP->c_fConsumableCooldownTimeTier[itmC->m_LevelRequired];
	}

	const FoodConfig* fc = g_pWeaponArmory->getFoodConfig(itemID);
	if(fc)
	{
		lastTimeUsedConsumable = r3dGetTime();
		currentConsumableCooldownTime = GPP->c_fConsumableCooldownTimeTier[itmC->m_LevelRequired];

		if(fc->category == storecat_Food)
			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_EAT"), GetPosition());
		else
			SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_DRINK"), GetPosition());

		m_Stamina += GPP->c_fSprintMaxEnergy*fc->Stamina;
	}
	if (itemID==WeaponConfig::ITEMID_Antibiotics||itemID==WeaponConfig::ITEMID_Bandages||itemID==WeaponConfig::ITEMID_Bandages2||
		itemID==WeaponConfig::ITEMID_Painkillers||itemID==WeaponConfig::ITEMID_Medkit||itemID==WeaponConfig::ITEMID_CraftedBandages||
		itemID==WeaponConfig::ITEMID_C01Vaccine||itemID==WeaponConfig::ITEMID_C04Vaccine)
	{
		int soundID = -1;
		if(itemID==WeaponConfig::ITEMID_Bandages||itemID==WeaponConfig::ITEMID_Bandages2||itemID==WeaponConfig::ITEMID_CraftedBandages)
			soundID = SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/HEAL_Bandages");
		else if(itemID==WeaponConfig::ITEMID_Antibiotics||itemID==WeaponConfig::ITEMID_Painkillers)
			soundID = SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/HEAL_Pills");
		else if(itemID==WeaponConfig::ITEMID_C01Vaccine||itemID==WeaponConfig::ITEMID_C04Vaccine)
			soundID = SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/HEAL_Inject");

		if(var4!=0)
		{
			GameObject* obj = GameWorld().GetNetworkObject(var4);
			if(obj && obj->isObjType(OBJTYPE_Human))
			{
				obj_Player* plr = (obj_Player*)obj;
				SoundSys.PlayAndForget(soundID==-1?(SoundSys.GetEventIDByPath(plr->m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_HEAL_F":"Sounds/WarZ/PlayerSounds/PLAYER_HEAL_M")):soundID, plr->GetPosition());
			}
		}
		else
			SoundSys.PlayAndForget(soundID==-1?(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_HEAL_F":"Sounds/WarZ/PlayerSounds/PLAYER_HEAL_M")):soundID, GetPosition());
	}


}

BOOL obj_Player::Update()
{
	R3DPROFILE_FUNCTION("obj_Player::Update");
#ifdef VEHICLES_ENABLED
	if (preparedToFakeDie)
	{
		preparedToFakeDie = false;
		PhysicsEnable(false);
		DoRagdoll(true, 0, 0, true);
		return TRUE;
	}
#endif

	if(!NetworkLocal) // for local already done in UpdateLocalPlayerMovement
		parent::Update();

	camoTimeLine.Update();

	if (Keyboard->WasPressed( kbsF5 ))
	{
		HideChat=!HideChat;
		if (hudMain) hudMain->HideChat(HideChat);
	}

	// show pending rewards, even if game finished.
	if(NetworkLocal)
		ProcessRewardQueue();

	const ClientGameLogic& CGL = gClientLogic();
	if(gClientLogic().disconnectStatus_)
		return TRUE;

	const float curTime = r3dGetTime();

	if(PhysXTerra3WarmUp < 10)
		PhysXTerra3WarmUp++;

	if (InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_CONSTRUCTOR) && NetworkLocal && (hudMain && !hudMain->isChatInputActive()) && !IsInVehicle() && isGameHardcore == false)//gamehardcore
	{
		m_ModeCreator=!m_ModeCreator;
		if (hudMain) hudMain->ConstructionMode(m_ModeCreator);
		ChangeWeaponByIndex(HANDS_WEAPON_IDX);
	}

	if(NetworkLocal)
	{
		if(curTime > nextSendCamPosTime)
		{
			nextSendCamPosTime = curTime + 15.0f;
			PKT_C2S_CameraPos_s n;
			n.camPos = gCam;
			p2pSendToHost(this, &n, sizeof(n),true);
		}
	}

	if (NetworkLocal && r3dGetTime() > OnlyFPSCameraTime)
	{
		if (g_camera_mode->GetInt()==2)
		{
			if (m_ModeCreator == true)
				switchFPS_TPS();
#ifdef VEHICLES_ENABLED
			else if (IsInVehicle())
				switchFPS_TPS();
#endif
		}
		else if (gClientLogic().m_gameInfo.flags & GBGameInfo::SFLAGS_OnlyFPS)
		{
			if (m_ModeCreator == false && !IsInVehicle())
				switchFPS_TPS();
		}
	}

	if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_PUTBLOCK) && m_ModeCreator == true)
		m_ItemSimulateLeftClick = true;

#ifndef FINAL_BUILD
	if(NetworkLocal && gDisableShortcuts == false )
	{
		if( Keyboard->IsPressed( kbsLeftAlt ) && Keyboard->WasPressed( kbsK ) )
		{
			g_char_spect->SetInt( !g_char_spect->GetInt() ) ;
		}

// 		if(Keyboard->WasPressed(kbsF4))
// 		{
// 			// send disconnect request to server
// 			PKT_C2S_DisconnectReq_s n;
// 			p2pSendToHost(this, &n, sizeof(n), true);
// 		}

		if(Keyboard->IsPressed(kbsLeftAlt) && hudMain)
		{
			if(Keyboard->WasPressed(kbs9))
			{
				PKT_C2C_ChatMessage_s n;
				n.userFlag = 0; // server will init it for others
				n.msgChannel = 0;
				r3dscpy(n.msg, "/gi 101002");
				r3dscpy(n.gamertag, "permaban");
				p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
			}

			if(Keyboard->WasPressed(kbs8))
			{
				hudMain->showChat(true);
				hudMain->addChatMessage(0, "blabla", "rtjdkfhdkjfhdf dfd f", 1);
			}

			if(Keyboard->WasPressed(kbsF5))
			{
				// send data update request to server
				PKT_C2S_DataUpdateReq_s n;
				p2pSendToHost(this, &n, sizeof(n), true);
				hudMain->showMessage("Data update request sent");
			}

			if(Keyboard->WasPressed(kbs8))
			{
				PKT_C2S_TEST_SpawnDummyReq_s n;
				n.pos = GetPosition() + GetvForw() * 4.0f;
				PxRaycastHit hit;
				PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
				if(g_pPhysicsWorld->raycastSingle(PxVec3(n.pos.x, n.pos.y+2, n.pos.z), PxVec3(0, -1, 0), 4, PxSceneQueryFlag::eIMPACT, hit, filter))
				{
					n.pos.x = hit.impact.x;
					n.pos.y = hit.impact.y;
					n.pos.z = hit.impact.z;
				}

				p2pSendToHost(this, &n, sizeof(n));
			}
		}
	}
#endif

	/*{
		// check that last known position is good (player didn't log out while in air, or underground, etc) to prevent being stuck
		{
			PxSweepHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_PLAYER_COLLIDABLE_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
			PxBoxGeometry boxg(0.5f, 0.1f, 0.5f);
			PxTransform pose(PxVec3(GetPosition().x, GetPosition().y+0.5f, GetPosition().z));
			if(g_pPhysicsWorld->PhysXScene->sweepSingle(boxg, pose, PxVec3(0,-1,0), 2000.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
			{
				r3dOutToLog("@@@@ %.2f, %.2f, %.2f\n", hit.impact.x, hit.impact.y, hit.impact.z);
			}
		}
	}*/

	float timePassed = r3dGetFrameTime();

	if(NetworkLocal && hudMain && !hudMain->isChatInputActive() && !hudMain->isPlayersListVisible())
	{
		if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_TOGGLE_FLASHLIGHT))
		{
			isFlashlightOn = !isFlashlightOn;
			if(m_Weapons[m_SelectedWeapon])
				m_Weapons[m_SelectedWeapon]->switchFlashlight(isFlashlightOn);
			PKT_C2C_PlayerSwitchFlashlight_s n;
			n.isFlashlightOn = isFlashlightOn;
			p2pSendToHost(this, &n, sizeof(n), true);
		}
	}

	// SPREAD/RECOIL LOGIC
	if(NetworkLocal && m_Weapons[m_SelectedWeapon])
	{
		if(CurrentSpread > 0 && !m_isPressedFireTrigger)
		{
			CurrentSpread -= timePassed*SpreadDecrease[m_Weapons[m_SelectedWeapon]->getCategory()-storecat_ASR];
		}
		CurrentSpread = R3D_MAX(CurrentSpread, SpreadMin[m_Weapons[m_SelectedWeapon]->getCategory()-storecat_ASR]*m_Weapons[m_SelectedWeapon]->getSpread());

		//r3dOutToLog("1) Recoil=%.2f, cooldown=%.2f\n", RecoilViewModTarget.y, RecoilCooldown.get());
		// update recoil
		if(!RecoilViewMod.AlmostEqual((RecoilViewModTarget), 0.01f))
		{
			RecoilViewMod = R3D_LERP(RecoilViewMod, RecoilViewModTarget, R3D_MIN(timePassed*GPP->c_fRecoilCooldownSpeed*4.0f * u_GetRandom(0.75f, 1.25f), 1.0f)); // lerp to make it smooth, less jerky
		}
		RecoilViewMod = R3D_LERP(RecoilViewMod, (RecoilViewMod+RecoilViewModTarget2), timePassed*2.0f);

		if(!PermRecoilViewMod.AlmostEqual(r3dPoint3D(0,0,0), 0.01f))
		{
			r3dPoint3D d = PermRecoilViewMod*0.1f;
			ViewAngle += d;
			PermRecoilViewMod -= d;
		}

		if(RecoilCooldown>0)
			RecoilCooldown = RecoilCooldown-timePassed;

		if(RecoilCooldown<=0)
		{
			// slowly make recoil go back to zero
			RecoilViewModTarget = R3D_LERP(RecoilViewModTarget, r3dVector(0,0,0), R3D_MIN(timePassed*GPP->c_fRecoilCooldownSpeed*8.0f, 1.0f));
		}
		// recoil effect when you are hit
		RecoilViewModTarget2 = R3D_LERP(RecoilViewModTarget2, r3dVector(0,0,0), R3D_MIN(timePassed*GPP->c_fRecoilCooldownSpeed*2.0f, 1.0f));

		//r3dOutToLog("2) Recoil=%.2f, cooldown=%.2f\n", RecoilViewModTarget.y, RecoilCooldown.get());
	}
	// FINISHED SPREAD/RECOIL LOGIC

	// resource collection tool
	if(NetworkLocal)
	{
		bool hasTool = m_Weapons[m_SelectedWeapon] && (m_Weapons[m_SelectedWeapon]->getItemID()==WeaponConfig::ITEMID_ResourceCollectionTool || m_Weapons[m_SelectedWeapon]->getItemID()==WeaponConfig::ITEMID_ResourceCollectionToolPremium);
		if(r_rc_highlight_radius->GetFloat() > 0.0f && !hasTool)
			r_rc_highlight_radius->SetFloat(0.0f);
		else if(r_rc_highlight_radius->GetFloat() == 0 && hasTool)
			r_rc_highlight_radius->SetFloat(200.0f);
	}

	// if mouse is visible - then we are in control menu mode, so DO NOT update player weapon (shooting) or player movement
	// update weapon selection
	if(NetworkLocal && !Mouse->GetMouseVisibility() && !uavViewActive_ && !bDead && !(g_pDefaultConsole && g_pDefaultConsole->IsVisible()) && !(hudMain && (hudMain->isChatInputActive() || hudMain->isPlayersListVisible())))
	{
		R3DPROFILE_FUNCTION("WeaponSelection");
		int prevSelectedItem = m_SelectedWeapon;
		{
			int buttonPressed = 0;
			//if(PlayerState != PLAYER_MOVE_SPRINT )
			{
				if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_PRIMARY_WEAPON) && !IsSwimming()
#ifdef VEHICLES_ENABLED 
					&& !IsInVehicle()
#endif
				)

					buttonPressed = 1;
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_HANDGUN_WEAPON) && !IsSwimming()
#ifdef VEHICLES_ENABLED 
					&& !IsInVehicle()
#endif			
					)
					buttonPressed = 2;
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_ITEM1))
					buttonPressed = 3;
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_ITEM2))
					buttonPressed = 4;
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_ITEM3))
					buttonPressed = 5;
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_ITEM4))
					buttonPressed = 6;
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_QUICK_SWITCH_WEAPON) && !IsSwimming()
#ifdef VEHICLES_ENABLED 
					&& !IsInVehicle()
#endif
					)
				{
					if(m_PrevSelectedWeapon == -1 || m_PrevSelectedWeapon == HANDS_WEAPON_IDX)
						buttonPressed = 1;	// switch to second weapon if no previous weapon was selected
					else
						buttonPressed = m_PrevSelectedWeapon+1; // +1 to map from index to button pressed
				}
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_NEXTITEM) && !IsSwimming()
#ifdef VEHICLES_ENABLED 
					&& !IsInVehicle()
#endif
					)
				{
					// scan for 4 item slots and see if we can switch to next item
					int selWeapon = R3D_MAX(m_SelectedWeapon, 1);
					for(int i=0; i<4; i++)
					{
						if(++selWeapon >= NUM_WEAPONS_ON_PLAYER - 1) // except HANDS_WEAPON_IDX
							selWeapon = 2;
						if(m_Weapons[selWeapon] != NULL)
						{
							buttonPressed = selWeapon + 1;
							break;
						}
					}
				}
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_TOGGLE_NIGHTVISION))
				{
					// check if player has night vision goggles
					if(CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20188 ||
						CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20187 ||
						CurLoadout.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID == 20067)
						r_hud_filter_mode->SetInt(r_hud_filter_mode->GetInt() == HUDFilter_NightVision ? HUDFilter_Default : HUDFilter_NightVision);
				}//Cynthia: 57 Allow nightvision when sprinting.
				else if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_FREE_HANDS) && !IsSwimming()
#ifdef VEHICLES_ENABLED 
					&& !IsInVehicle()
#endif
					)
				{
					// can't switch while throwing grenade
					bool canSwitch = !(m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getCategory() == storecat_GRENADE && InputMappingMngr->isPressed(r3dInputMappingMngr::KS_PRIMARY_FIRE));
					if(m_SelectedWeapon != HANDS_WEAPON_IDX && canSwitch)
					{
						ChangeWeaponByIndex(HANDS_WEAPON_IDX);
					}
				}
				else if(g_mouse_wheel->GetBool() && !IsSwimming() && isGameHardcore == false //gamehardcore
#ifdef VEHICLES_ENABLED 
					&& !IsInVehicle()
#endif					
					)//cynthia: 742
				{
					// query mouse scroll
					int mouseScroll = Mouse->m_WheelScroll;
					if(Gamepad->WasPressed(gpY))
						mouseScroll = 1;

#if !defined(FINAL_BUILD)
					if( r_do_hackerzoom->GetInt() )
					{
						r_hackerzoom->SetFloat( R3D_MAX( 0.5f, r_hackerzoom->GetFloat() + mouseScroll / 128.0f ) );
					}
					else
#endif

					if(mouseScroll!=0)
					{
						int selWeapon = m_SelectedWeapon;
						for(int i=0; i<NUM_WEAPONS_ON_PLAYER - 1; i++) // except HANDS_WEAPON_IDX
						{
							if(mouseScroll>0) {
								if(++selWeapon >= NUM_WEAPONS_ON_PLAYER - 1)
									selWeapon = 0;
							} else {
								if(--selWeapon < 0)
									selWeapon = NUM_WEAPONS_ON_PLAYER - 1 - 1;
							}

							if(m_Weapons[selWeapon] != NULL) {
								buttonPressed = selWeapon + 1;
								break;
							}
						}
					}
				}
			} 
			/*else if ( PlayerState == PLAYER_MOVE_SPRINT ) 
			{
				// clear the mouse wheel. 
				Mouse->m_WheelScroll = 0;
			}*/
			
			r3d_assert(buttonPressed>=0 && buttonPressed <= NUM_WEAPONS_ON_PLAYER - 1); // except HANDS_WEAPON_IDX
			const FoodConfig* foodItem = NULL;
			if((buttonPressed-1)>=wiCharDataFull::CHAR_LOADOUT_ITEM1 && (buttonPressed-1)<=wiCharDataFull::CHAR_LOADOUT_ITEM4)
				foodItem = g_pWeaponArmory->getFoodConfig(CurLoadout.Items[buttonPressed-1].itemID);
			if(foodItem)
			{
				wiInventoryItem& wi = CurLoadout.Items[buttonPressed-1];
				if(wi.quantity > 0)
				{
					showSlotsInfoTimer = r3dGetTime() + 5.0f; // show weapon info for X seconds
					if(hudMain)
						hudMain->showSlots(true);
					
					if((r3dGetTime() - lastTimeUsedConsumable) < currentConsumableCooldownTime)
					{ // cooldown
					}
					else
					{
						if(hudMain)
							hudMain->setActivatedSlot(buttonPressed-1);
						localPlayer_UseItem(buttonPressed-1, foodItem->m_itemID, GetPosition() + GetvForw()*1.0f + r3dPoint3D(0,1,0));

						if(--wi.quantity <= 0) {
							wi.Reset();
							OnBackpackChanged(buttonPressed-1);
						}
						else
						{
							if(hudMain)
								hudMain->updateSlotInfo(buttonPressed-1, wi.quantity);
						}
					}
				}
			}
			else if(buttonPressed && m_Weapons[buttonPressed-1] && !IsSwimming() && isGameHardcore == false)//gamehardcore
			{
				if(buttonPressed>2 && (!(m_Weapons[buttonPressed-1]->isUsableItem() || m_Weapons[buttonPressed-1]->getCategory()==storecat_GRENADE) || 
					(m_Weapons[buttonPressed-1]->getItemID()==WeaponConfig::ITEMID_RepairKit ||  m_Weapons[buttonPressed-1]->getItemID()==WeaponConfig::ITEMID_ConstructorWODBOX || m_Weapons[buttonPressed-1]->getItemID()==WeaponConfig::ITEMID_AerialDelivery && gClientLogic().localPlayer_ && gClientLogic().localPlayer_->m_NumbeAirDrop<=0 || m_Weapons[buttonPressed-1]->getItemID()==WeaponConfig::ITEMID_PremRepairKit)))
				{
					// do not allow to switch to quick slots if they are not usable items or grenades
				}
				else
				{
					if((m_Weapons[buttonPressed-1]->isUsableItem() || m_Weapons[buttonPressed-1]->getCategory()==storecat_GRENADE || (m_Weapons[prevSelectedItem] && m_Weapons[prevSelectedItem]->getCategory()==storecat_GRENADE)) && InputMappingMngr->isPressed(r3dInputMappingMngr::KS_PRIMARY_FIRE))//Cynthia: 766
					{
						// do not allow switching to usable item while fire key is pressed
					}
					else
					{
						if(buttonPressed-1 != m_SelectedWeapon)
						{
							ChangeWeaponByIndex(buttonPressed-1);
						}
						else if(buttonPressed >= 3 && buttonPressed <= 6) // sergey's request: already was pressed and this is an item, simulate left click!
						{
							if(m_Weapons[buttonPressed-1]->isUsableItem())
								m_ItemSimulateLeftClick = true;
						}
					}
				}
			}
		}
	}

	// WARNING: current model is rotated 180 around y
	D3DXMATRIX mr;
	D3DXMatrixRotationY(&mr, R3D_PI);
	D3DXMATRIX CharDrawMatrix = mr;

	if(NetworkLocal) 
	{
		if(m_HitMarkerFadeout > 0)
			m_HitMarkerFadeout -= timePassed * GPP->c_fHitMarkerFadeoutSpeed;

		UpdateTargetting();
	}



	plr_local_moving_speed = (GetPosition() - oldstate.Position).Length();
	if(NetworkLocal) 
	{
		// moved to separate function and called from UpdatePlayerMovement, to make sure that player position and camera position are in sync
	} 
	else if(GetVelocity().LengthSq() > 0.0001f)
	{
		R3DPROFILE_FUNCTION("NetPlayer_Move");
		r3dPoint3D prevPos = GetPosition();
		SetPosition(GetPosition() + GetVelocity() * timePassed);

		DetectIfOnGround();

		// check if we overmoved to target position
		r3dPoint3D v = netMover.GetNetPos() - GetPosition();
		float d = GetVelocity().Dot(v);
		if(d < 0) {
			SetPosition(netMover.GetNetPos());
			SetVelocity(r3dPoint3D(0, 0, 0));
		}

		float ll = (prevPos-GetPosition()).LengthSq();
		if(ll > 0.0001f)
		{
			extern void WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx);
			WaterSplash(GetPosition(), 2.0f, 5.0f, 0.1f*ll, 3);
		}
	}
	if(afterRespawnTimer>0)
		afterRespawnTimer -= timePassed;
	//r3dOutToLog("Player Update: %s to %.2f, %.2f, %.2f\n", UserName, GetPosition().x, GetPosition().y, GetPosition().z);

	// update footsteps after moving player
	UpdateFootsteps();
	/*
		// update camo only if player has special items
		gCamouflageDataManager.UpdateCamouflageData(*this);
	*/

	UpdateSounds();
 

	// even if dead, still need to send pos, otherwise if died in the air - you will still be in the air.
	if(NetworkLocal) 
	{
		CNetCellMover::moveData_s md;
		md.pos       = GetPosition();
#ifndef FINAL_BUILD
		if(d_enable_dev_flycam->GetBool())
			md.pos = gCam;
#endif
		md.turnAngle = m_fPlayerRotationTarget;
		md.bendAngle = bodyAdjust_y[1];
		md.state     = (PlayerState&0xFF) | ((PlayerMoveDir&0xF) << 8);

		PKT_C2C_MoveSetCell_s n1;
		PKT_C2C_MoveRel_s     n2;

#ifdef VEHICLES_ENABLED
		if (!IsInVehicle())
#endif
		{
			DWORD pktFlags = netMover.SendPosUpdate(md, &n1, &n2);

			if(pktFlags & 0x1)
				p2pSendToHost(this, &n1, sizeof(n1), true);
			if(pktFlags & 0x2)
				p2pSendToHost(this, &n2, sizeof(n2), false);
		}

		static r3dPoint3D prevCamDir = r3dPoint3D(0,0,0); // static is ok here, as this part runs only for local player
		static r3dPoint3D prevCamLoc = r3dPoint3D(0,0,0);
		bool camDirChanged = !gCam.vPointTo.AlmostEqual(prevCamDir, 0.0001f);
		bool camLocChanged = !gCam.AlmostEqual(prevCamLoc, 0.01f);
		if(camDirChanged)
			prevCamDir = gCam.vPointTo;
		if(camLocChanged)
			prevCamLoc = gCam;

		static float nextTimeSendCamUpdate = 0.0f;
		if((camDirChanged || camLocChanged) && r3dGetTime() > nextTimeSendCamUpdate)
		{
			nextTimeSendCamUpdate = r3dGetTime() + 0.1f; // not more than 10 times per second

			PKT_C2S_MoveCameraLocation_s n3;
			n3.cam_dir_x = gCam.vPointTo.x;
			n3.cam_dir_y = gCam.vPointTo.y;
			n3.cam_dir_z = gCam.vPointTo.z;
			n3.cam_loc_x = gCam.x;
			n3.cam_loc_y = gCam.y;
			n3.cam_loc_z = gCam.z;
			p2pSendToHost(this, &n3, sizeof(n3), false);
		}
	}
	
	if(NetworkLocal && !bAllowToUseWeapons)
	{
		uberAnim_->StopGrenadeAnimations();
		grenadeReadyTime = 0.0f;
	}

	if(NetworkLocal && hudMain)
	{
		R3DPROFILE_FUNCTION("update hud");
		// update HUD
		{
			if(bDead)
			{
				float time = r3dGetTime() - TimeOfDeath;
				int t;
				if (gUserProfile.ProfileData.PremiumAcc > 0 || gUserProfile.ProfileData.isDevAccount)
					t = R3D_MAX(30-int(time), 0);
				else
					t = R3D_MAX(60-int(time), 0);
				hudMain->updateDeadTimer(t);
			}

			{
				float bloodAlpha = 1.0f-R3D_MIN(r3dGetTime()-lastTimeHit, 1.0f);
				hudMain->setBloodAlpha(bloodAlpha);
			}

			bAllowToUseWeapons = true;
			if(CurLoadout.GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox){//Cynthia:bugID 1111
				bAllowToUseWeapons = false;
				m_isAiming = false;
				m_isFinishedAiming = false;
			}
			hudMain->showSafeZoneWarning(!bAllowToUseWeapons);
			
			// invisible admins must not shoot, they'll desync grenades
			//if(gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_INVISIBLE) {
			if (m_DevPlayerHide == true) {
				bAllowToUseWeapons = false;
				m_isAiming = false;
				m_isFinishedAiming = false;
			}

			hudMain->setThreatValue(GetGraduatedZombieThreat());

			//hudMain->setTime(CGL.GetServerGameTime());
			if(hudPause->isActive())
				hudPause->setTime(CGL.GetServerGameTime());

			hudMain->setLifeParams((int)CurLoadout.Hunger, (int)CurLoadout.Thirst, (int)CurLoadout.Health, (int)CurLoadout.Toxic, int(((getMaxStamina()-m_Stamina)/getMaxStamina())*100.0f));
			static int prevToxicity = 0;
			if(prevToxicity==0 && int(CurLoadout.Toxic)>0)
			{
				prevToxicity = 1;
				hudMain->setLifeConditions("IconVirus", true);
			}
			else if(prevToxicity==1 && int(CurLoadout.Toxic)==0)
			{
				prevToxicity = 0;
				hudMain->setLifeConditions("IconVirus", false);
			}

			if(m_Weapons[m_SelectedWeapon] && hudMain->isWeaponInfoVisible()==1)//Cynthia:554
			{	
				if(m_Weapons[m_SelectedWeapon]->getCategory() == storecat_GRENADE) // treated as items
				{	
					const wiInventoryItem& wi = m_Weapons[m_SelectedWeapon]->getPlayerItem();
					hudMain->setWeaponInfo(R3D_MIN(1, wi.quantity), R3D_MIN(0, wi.quantity-1), 1, 100);
				}
				else
					hudMain->setWeaponInfo(m_Weapons[m_SelectedWeapon]->getNumBulletsLeft(), m_Weapons[m_SelectedWeapon]->getNumClipsLeft(), (int)m_Weapons[m_SelectedWeapon]->getFiremode(), m_Weapons[m_SelectedWeapon]->getPlayerItem().Var3/100);

			}

			if(r3dGetTime() > showSlotsInfoTimer && showSlotsInfoTimer>0)
			{
				hudMain->showSlots(false);
				showSlotsInfoTimer = 0;
			}
		}
		{
			// update items
			for(int i=wiCharDataFull::CHAR_LOADOUT_WEAPON1; i<=wiCharDataFull::CHAR_LOADOUT_ITEM4; ++i)
			{
				const Weapon* wpn = m_Weapons[i];
				if(!wpn)
				{
					if (g_pWeaponArmory->getFoodConfig(CurLoadout.Items[i].itemID)
						&& (r3dGetTime() - lastTimeUsedConsumable) < currentConsumableCooldownTime)
					{
						int timeLeft = int(currentConsumableCooldownTime-(r3dGetTime() - lastTimeUsedConsumable));
						int progress = (int)R3D_CLAMP(float(float(timeLeft)/currentConsumableCooldownTime)*100.0f, 1.0f, 100.0f);
						hudMain->setSlotCooldown(i, progress, timeLeft);
					}
					else
						hudMain->setSlotCooldown(i, 101, 0);
					continue;
				}

				if(wpn->isUsableItem())
				{
					hudMain->updateSlotInfo(i, CurLoadout.Items[i].quantity);
					if((wpn->getItemID() == WeaponConfig::ITEMID_Bandages || wpn->getItemID() == WeaponConfig::ITEMID_Bandages2 || wpn->getItemID() == WeaponConfig::ITEMID_Antibiotics ||
						wpn->getItemID() == WeaponConfig::ITEMID_Painkillers || wpn->getItemID() == WeaponConfig::ITEMID_Medkit || wpn->getItemID() == WeaponConfig::ITEMID_CraftedBandages) 
						&& (r3dGetTime() - lastTimeUsedConsumable) < currentConsumableCooldownTime)
					{
						int timeLeft = int(currentConsumableCooldownTime-(r3dGetTime() - lastTimeUsedConsumable));
						int progress = (int)R3D_CLAMP(float(float(timeLeft)/currentConsumableCooldownTime)*100.0f, 1.0f, 100.0f);
						hudMain->setSlotCooldown(i, progress, timeLeft);
					}
					else
						hudMain->setSlotCooldown(i, 101, 0);
					
				}
				else
					hudMain->setSlotCooldown(i, 101, 0);
			}

			float bloodLevel = 0;
			if(CurLoadout.Health < 25.0f)
				bloodLevel = 100.0f - ((CurLoadout.Health/25.0f)*100.0f); // show blood effect when health is between 0 and 25
			PFX_3DLUTColorCorrection::s_damageHitColorCorrectionVal = (bloodLevel/100.0f);
			//hudMain->SetBloodLevel(bloodLevel);
		}
	}
	// CLAN TAG
	if(!NetworkLocal && hudMain && uavViewActive_ == false)
	{
		if(m_CharIcon.IsUndefined() && CGL.localPlayer_ && (CGL.m_gameInfo.flags & GBGameInfo::SFLAGS_Nameplates))
		{
			char plrUserName[256]; GetUserNameAndClanTag(plrUserName);
			// todo: need a better way to find player's reputation
			int rep = 0;
			{
				char tmpUsername[64];
				m_EncryptedUserName.get(tmpUsername);
				for(int i=0; i<256; ++i)
				{
					if(strcmp(CGL.playerNames[i].Gamertag, tmpUsername)==0)
					{
						rep = CGL.playerNames[i].reputation;
						break;
					}
				}
			}
			hudMain->addCharTag(plrUserName, ClanID == CGL.localPlayer_->ClanID && ClanID!=0, rep, m_CharIcon);
		}

if(!m_CharIcon.IsUndefined())
                {
                        bool showTag = true;
                        bool showName = false;
                        bool sameGroup = CGL.localPlayer_->GroupID==GroupID && GroupID!=0;
                        bool sameClan = CurLoadout.ClanID == CGL.localPlayer_->CurLoadout.ClanID && CurLoadout.ClanID != 0;
                        bool sameClan2 = CGL.localPlayer_->ClanID==ClanID && ClanID!=0;
 
                        float dist = (CGL.localPlayer_->GetPosition()-GetPosition()).Length();
                        if(this->GetSafeID() == m_LocalPlayer_CurrentAimAt)
                        {
                               
                                float time = r3dGetTime() - m_LocalPlayer_CurrentAimAt_StartTime;
                                if(dist < 50.0f)
                                        showName = true;
                                else if(dist < 100.0f && time > 0.5f)
                                        showName = true;
                                else if(dist >= 100.0f && time > 2.0f)
                                        showName = true;
                        }
                       
                        bool alwaysShow = true;
                       
                        if(bDead)
                                showTag = false;
                       
                        if(LOCAL_ENABLE_GAMERTAG->GetBool() && (sameGroup || sameClan || sameClan2) && dist < 600)
                        {
                                showName = true;
                        }
                       
                        if (m_DevPlayerHide == true)
                        {
                                showName = false;
                                alwaysShow = false;
                        }
                       
                        hudMain->setCharTagTextVisible(m_CharIcon, showName, sameGroup, m_isVoiceActive&&g_voip_showChatBubble->GetBool());
                        r3dPoint3D offset(0,0.3f,0);
                        // get head position
                        hudMain->moveUserIcon(m_CharIcon, GetHeadPosition()+offset, alwaysShow);
                }
	}

	// spawn protection logic
	{
		if(r3dGetTime() < m_SpawnProtectedUntil && m_AuraType != AT_SPAWNPROTECTION)
			m_AuraType = AT_SPAWNPROTECTION;
		else if(r3dGetTime() > m_SpawnProtectedUntil && m_AuraType == AT_SPAWNPROTECTION)
			m_AuraType = AT_NONE;
	}

	// check for dead after moving player, otherwise might die in the air and be stuck in the air (dead body)
	if(bDead)
	{
		if(NetworkLocal)
			PFX_3DLUTColorCorrection::s_damageHitColorCorrectionVal = 0;

		UpdateRotation();

		UpdateAnimations(CharDrawMatrix);

		if(m_Weapons[m_SelectedWeapon])
			m_Weapons[m_SelectedWeapon]->Update(uberEquip_->getWeaponBone(GetSkeleton(), DrawFullMatrix));

		return TRUE;
	}

	if(!NetworkLocal)
	{
		UpdatePlayerObstacleCapsule(PhysXObstacleIndex, GetPosition());
	}

	SyncAnimation();

	UpdateRotation();

	UpdateAnimations(CharDrawMatrix);

	if(m_Weapons[m_SelectedWeapon])
	{
		m_Weapons[m_SelectedWeapon]->Update(uberEquip_->getWeaponBone(GetSkeleton(), DrawFullMatrix));
	}

	if(NetworkLocal)
	{
		// breath timer
		/*if(m_BreathPenaltyTime > 0)
		{
			m_BreathPenaltyTime = m_BreathPenaltyTime - r3dGetFrameTime();
			if(m_BreathPenaltyTime < 0)
				m_BreathPenaltyTime = 0;
		}
		else*/
		{
			if(!InputMappingMngr->isPressed(r3dInputMappingMngr::KS_HOLD_BREATH))
			{
				if(m_Breath < getMaxBreath())
					m_Breath = m_Breath + timePassed;
			}
		}
	}

	{
		if(r3dGetTime() < m_MorhineShotTime && m_AuraType == AT_NONE)
			m_AuraType = AT_MORHINE;
		if(m_AuraType == AT_MORHINE && r3dGetTime() > m_MorhineShotTime)
			m_AuraType = AT_NONE;
	}

	if(NetworkLocal)
	{
		if(BloodEffect > 0)
		{
			BloodEffect -= timePassed;
			if(BloodEffect < 0) BloodEffect = 0;
		}
	}
	
	// update fire
	if(NetworkLocal && !Mouse->GetMouseVisibility())
	{
		if(fabs(m_ReticleCurrentScale - m_ReticleTargetScale)>0.01f)
			m_ReticleCurrentScale = R3D_LERP(m_ReticleCurrentScale, m_ReticleTargetScale, timePassed * 5.0f);

		bool scopeRendering = (hasScopeMode() && CurrentRig.allowScope && m_isAiming) || (g_camera_mode->GetInt()==2 && m_isAiming);
	}

	// if low health - start dropping blood every second
	m_BloodTimer += timePassed;
	if(CurLoadout.Health < 30 && m_BloodTimer > 1.0f)
	{
		r3dPoint3D pos = GetPosition(); pos.y += 2.0f;

#ifdef VEHICLES_ENABLED
		if (!IsInVehicle())
#endif
			AddBloodOnGround(pos);

		m_BloodTimer = 0;
	}

	if(NetworkLocal)
		UpdateActionUI();

#ifndef FINAL_BUILD
	if( Permanufactura )
	{
		Permanufactura_Counter += r3dGetFrameTime();

		if( Permanufactura_Counter > 1.0f )
		{
			Permanufactura_Counter = 0;

			int SelectRandomDropItem();

			int itemId = SelectRandomDropItem();

			char msg[ 128 ];

			sprintf( msg, "/gi %d", itemId );

			PKT_C2C_ChatMessage_s n;
			n.userFlag = 0; // server will init it for others
			n.msgChannel = 0;
			r3dscpy( n.msg, msg );
			r3dscpy( n.gamertag, "permaban" );
			p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
		}

		for( int i = 0, e = wiCharDataFull::CHAR_MAX_BACKPACK_SIZE; i < e; i ++ )
		{
			if( CurLoadout.Items[ i ].itemID > 0 )
			{
				DropItem( i );
				break;
			}
		}
	}
#endif

	return TRUE;
}

float dist_Point_to_Line( const r3dPoint3D& p, const r3dPoint3D& l1, const r3dPoint3D& l2, bool clampToSegment=false)
{
	r3dVector v = l2 - l1;
	r3dVector w = p - l1;

	float c1 = w.Dot(v);
	float c2 = v.Dot(v);
	float b = c1 / c2;
	if(clampToSegment)
		b = R3D_CLAMP(b, 0.0f, 1.0f);

	r3dPoint3D Pb = l1 + b * v;
	return (p-Pb).Length();
}

void obj_Player::UpdateActionUI()
{
	R3DPROFILE_FUNCTION("UpdateActionUI");
	r3d_assert(NetworkLocal);
	
#ifndef FINAL_BUILD
#ifdef VEHICLES_ENABLED
	if (g_bEditMode)
	{
		if (InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_INTERACT))
		{
			if (!IsInVehicle())
			{
				obj_Vehicle* vehicle = IsNearVehicle();
				if (!vehicle)
					return;

				enterVehicle(vehicle, 0);
			}
			else
			{
				exitVehicle(currentVehicle, currentVehicle->GetPosition() + -(currentVehicle->GetvRight() * 2.5f));
			}
		}
	}
#endif
#endif

	if(hudMain==NULL || hudActionUI == NULL || hudPause == NULL || hudAttm == NULL || hudVault == NULL || hudStore == NULL || hudSafelock == NULL || hudTrade==NULL)
		return;

#ifdef VEHICLES_ENABLED
	if (IsInVehicle()) return;
#endif

	if(hudMain->isChatInputActive()) return;
	if(hudMain->isPlayersListVisible()) return;
	if(hudPause->isActive()) return;
	if(hudVault->isActive()) return;
	if(hudRepair->isActive()) return;
	if(hudCraft->isActive()) return;
	if(hudSafelock->isActive()) return;
	if(hudTrade->isActive()) return;
	if(hudStore->isActive()) return;
	if(hudAttm->isActive()) return;
	if(bDead)return;
	

	float maxDist = g_camera_mode->GetInt()==2?3.0f:4.0f;

	r3dVector forw = gCam.vPointTo;//GetvForw();
	r3dVector searchStartLine = gCam;
	r3dVector searchEndLine = searchStartLine + forw*maxDist;
	r3dVector searchDirN = forw;

	// slow, need to rewrite
	GameObject* dropObj = 0;

	float distance = 999999999.0f;

	float curTime = r3dGetTime();

	bool selectHumans = true; // allow to trade by default, if player has med item, then it will default to that action firstly
	bool usingMedItem = false;

	{
		uint32_t wpnItemID = 0;
		if(m_Weapons[m_SelectedWeapon])
			wpnItemID = m_Weapons[m_SelectedWeapon]->getItemID();
		usingMedItem = wpnItemID==WeaponConfig::ITEMID_Antibiotics||wpnItemID==WeaponConfig::ITEMID_Bandages||wpnItemID==WeaponConfig::ITEMID_Bandages2||wpnItemID==WeaponConfig::ITEMID_CraftedBandages||
			wpnItemID==WeaponConfig::ITEMID_Painkillers||wpnItemID==WeaponConfig::ITEMID_ZombieRepellent||wpnItemID==WeaponConfig::ITEMID_C01Vaccine||
			wpnItemID==WeaponConfig::ITEMID_C04Vaccine||wpnItemID==WeaponConfig::ITEMID_Medkit;
		if(m_Weapons[m_SelectedWeapon] && usingMedItem)
		{
			wiInventoryItem wi = CurLoadout.Items[m_Weapons[m_SelectedWeapon]->m_BackpackIdx];
			selectHumans = wi.quantity!=0;
		}
	}
    bool isTankToEnter = false;
	{
		SharedUsableItemList::iterator it;
		for(it=m_SharedUsableItemList.begin(); it!=m_SharedUsableItemList.end(); ++it)
		{
			if (uavViewActive_)
				break;

			SharedUsableItem* obj = *it;
			r3dPoint3D objCenter;
			if(obj->isObjType(OBJTYPE_Vehicle))
			{
				obj_Vehicle* vehicleinfo = (obj_Vehicle*)obj;
				if (vehicleinfo->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_TANK_T80 || vehicleinfo->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER)
				{
					objCenter = obj->GetPosition();
					isTankToEnter=true;
				}
				else {
					objCenter = obj->GetBBoxWorld().Center();
				}
			}
			else {
				objCenter = obj->GetBBoxWorld().Center();
			}
			float d = dist_Point_to_Line(objCenter, searchStartLine, searchEndLine, true); 
			if(distance > d)
			{
				// check dist to player
				if((objCenter - GetPosition()).LengthSq() > (maxDist*maxDist))
					continue;
				// check if both player and camera are facing object
				if((GetPosition()-objCenter).Dot(gCam-objCenter) < 0.0f) 
					continue;
				// check ray cast that you can see that item
				bool isVisible = false;
				r3dVector objSizeBBox = obj->GetBBoxWorld().Size/4;
				r3dVector posToTest[5] = {objCenter,objCenter+r3dVector(-objSizeBBox.x,	objSizeBBox.y, -objSizeBBox.z),
													objCenter+r3dVector( objSizeBBox.x,	objSizeBBox.y, -objSizeBBox.z),
													objCenter+r3dVector(-objSizeBBox.x,	objSizeBBox.y,  objSizeBBox.z),
													objCenter+r3dVector( objSizeBBox.x,	objSizeBBox.y,  objSizeBBox.z) };
				for(int i=0; i<5; ++i)
				{
					PxRaycastHit hit;
					PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
					r3dVector rayDir = (posToTest[i] - searchStartLine);
					float rayLen = rayDir.Length();
					r3dVector rayDirN = rayDir.Normalize();
					if(g_pPhysicsWorld->raycastSingle(PxVec3(searchStartLine.x, searchStartLine.y, searchStartLine.z), PxVec3(rayDirN.x, rayDirN.y, rayDirN.z), rayLen, PxSceneQueryFlag::eIMPACT, hit, filter))
					{
						// check if we hit our object
						PhysicsCallbackObject* target = NULL;
						if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
						{
							if(target->isGameObject() == obj)
								isVisible = true;
						}
					}
					else
						isVisible = true;
					//else
					//	continue; // didn't hit our object for some reason, bad raycast?
				}

				if(!isVisible)
					continue;

				if(obj->m_lastPickupAttempt < curTime)
				{
					dropObj = obj;
					distance = d;
				}
			}
		}

		if (uavId_ != invalidGameObjectID) // UAV Code
		{
			obj_UAV* uav = (obj_UAV*)GameWorld().GetObject(uavId_);

			if (uav)
			{
				if (uavId_ == uav->GetSafeID() && !uavViewActive_)
				{
					if (uav->state_ != obj_UAV::UAV_Killed)
					{
						float d = dist_Point_to_Line(uav->GetPosition(), searchStartLine, searchEndLine, true);
						if(distance > d) {
								dropObj = uav;
								distance = d;
						}
					}
				}
			}			 
		}

		if(selectHumans && !uavViewActive_)
		{
			ClientGameLogic& CGL = gClientLogic();
			for(int i=0; i<MAX_NUM_PLAYERS; ++i)
			{
				obj_Player* plr = CGL.GetPlayer(i);
				if(plr && plr != this && !plr->bDead)
				{
					float d = dist_Point_to_Line(plr->GetBBoxWorld().Center(), searchStartLine, searchEndLine, true); 
					if(distance > d)
					{
						// check if player can see this player
						bool isVisible = false;
						{
							PxRaycastHit hit;
							PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|1<<PHYSCOLL_NETWORKPLAYER, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
							
							r3dPoint3D dir;
							if(m_isInScope || g_camera_mode->GetInt() != 1)
								r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &dir);
							else
								r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &dir);

							r3dVector rayDirN = dir.Normalize();
							if(g_pPhysicsWorld->raycastSingle(PxVec3(gCam.x, gCam.y, gCam.z), PxVec3(rayDirN.x, rayDirN.y, rayDirN.z), maxDist, PxSceneQueryFlag::eIMPACT, hit, filter))
							{
								// check if we hit our object
								PhysicsCallbackObject* target = NULL;
								if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
								{
									if(target->isGameObject() == plr)
										isVisible = true;
								}
							}
						}
						if(isVisible)
						{
							dropObj = plr;
							distance = d;
						}
					}
				}
			}
		}
	}

	float MaxDistanceUI = 1.5f;

	if (isTankToEnter==true && dropObj && dropObj->isObjType(OBJTYPE_Vehicle))
	{
		MaxDistanceUI=3.0f;
	}

	if(distance < MaxDistanceUI) // distance to ray. if more than 1, then it is outside of ray
	{
		// in case if you are standing right next to pile of objects, we need to re-init ui each time we pick different item from pile
		static GameObject* prevHUDItem = NULL;
		static uint32_t meantToShare = 0;//Cynthia:1142. medical items local player intended to heal other player.
		
		static bool bIsPrimaryInteraction = true;
		if(hudActionUI->isActive() && (prevHUDItem != dropObj || (m_Weapons[m_SelectedWeapon] && meantToShare!=m_Weapons[m_SelectedWeapon]->getItemID())))
		{
			bIsPrimaryInteraction = true;
			hudActionUI->Deactivate(); 
			ActionUI_pressedKeyTimer = 0;
		}
		prevHUDItem = dropObj;
		if(m_Weapons[m_SelectedWeapon]) meantToShare = m_Weapons[m_SelectedWeapon]->getItemID();

		if(!hudActionUI->isActive() )
		{
			hudActionUI->Activate();
			
			if(dropObj->isObjType(OBJTYPE_SharedUsableItem))
			{	
				char pickUpItemMsg[128];
				if(dropObj->Class->Name == "obj_Gravestone")
				{
					bIsPrimaryInteraction = false;
					sprintf(pickUpItemMsg,((SharedUsableItem*)dropObj)->m_ActionUI_Msg,InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_SECONDARY_INTERACT));
					hudActionUI->setText(((SharedUsableItem*)dropObj)->m_ActionUI_Title, pickUpItemMsg, InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_SECONDARY_INTERACT));
				}
#ifdef MISSIONS
				else if(dropObj->Class->Name == "obj_MissionStateObject")
				{
					bIsPrimaryInteraction = true;
					if( ((obj_MissionStateObject*)dropObj)->PlayerHasValidMission( this ) )
					{
						sprintf(pickUpItemMsg,((SharedUsableItem*)dropObj)->m_ActionUI_Msg,InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
						hudActionUI->setText(((SharedUsableItem*)dropObj)->m_ActionUI_Title, pickUpItemMsg, InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
					}
					else
					{
						hudActionUI->Deactivate();
					}
				}
#endif
				else
				{
					bIsPrimaryInteraction = true;
					sprintf(pickUpItemMsg,((SharedUsableItem*)dropObj)->m_ActionUI_Msg,InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
					hudActionUI->setText(((SharedUsableItem*)dropObj)->m_ActionUI_Title, pickUpItemMsg, InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));

#ifdef VEHICLES_ENABLED
					if (dropObj->isObjType(OBJTYPE_Vehicle))
					{
						obj_Vehicle* vehicle = (obj_Vehicle*)dropObj;

						if (vehicle->hasExploded)
						{
							hudActionUI->Deactivate();
							return;
						}

						hudActionUI->setCarInfo(vehicle->GetDisplayDurability(), (int)floor(vehicle->vd->GetMaxSpeed()), vehicle->GetArmor(), (int)((float)vehicle->GetWeight() / (float)vehicle->vd->maxWeight * 100), vehicle->GetFuelAsPercent());
						hudActionUI->showCarInfo(true);
					}
#endif
				}
			}
			else if (dropObj->isObjType(OBJTYPE_UAV))
			{
					char pickUpItemMsg[128];
					sprintf(pickUpItemMsg,gLangMngr.getString("HoldEToGetUAV"),InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
					hudActionUI->setText(gLangMngr.getString("$FR_GETUAV"), pickUpItemMsg, InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
			}
			else // human
			{
				if(usingMedItem)
					hudActionUI->setText(gLangMngr.getString("$FR_PAUSE_USE_ITEM"), m_Weapons[m_SelectedWeapon]->getConfig()->m_StoreName, InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
				else
				{
					char pickUpItemMsg[128];
					sprintf(pickUpItemMsg,gLangMngr.getString("HoldEToStartTrade"),InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
					hudActionUI->setText(gLangMngr.getString("$FR_TRADE"), pickUpItemMsg, InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_INTERACT));
				}
			}
			hudActionUI->enableRegularBlock();
		}
		if(hudActionUI->isActive())
		{
			r3dPoint3D scrPos;
			if (isTankToEnter == true && dropObj->isObjType(OBJTYPE_Vehicle))
				r3dProjectToScreen(dropObj->GetPosition()+r3dPoint3D(0,1.4,0), &scrPos);
			else
				r3dProjectToScreen(dropObj->GetBBoxWorld().Center(), &scrPos);
			hudActionUI->setScreenPos((int)scrPos.x, (int)scrPos.y);
		}

		if (m_DevPlayerHide == true && NetworkLocal)
		{
			hudActionUI->Deactivate();
		}

		if (dropObj->isObjType(OBJTYPE_Human))
		{
			obj_Player* PlayerTrade = ((obj_Player*)dropObj);
			if (PlayerTrade->m_DevPlayerHide == true)
				hudActionUI->Deactivate();
		}

		if(dropObj->Class->Name == "obj_Door")
		{
			obj_Door* Door = ((obj_Door*)dropObj);
				if (Door->DoorIsDestroy == 1)
					hudActionUI->Deactivate();
		}

		if(dropObj->Class->Name == "obj_Traps") // traps
		{
			obj_Traps* Traps = ((obj_Traps*)dropObj);
			if (Traps->m_Activated || Traps->m_ItemID == WeaponConfig::ITEMID_Campfire)
					hudActionUI->Deactivate();
		}

		if(dropObj->Class->Name == "obj_DroppedItem")
		{
			obj_DroppedItem* DroppedItem = ((obj_DroppedItem*)dropObj);
				if (DroppedItem->m_Item.itemID == 'FLPS')
					hudActionUI->Deactivate();
		}

#ifdef VEHICLES_ENABLED
		if (InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_VEHICLE_UNSTUCK) && !gClientLogic().localPlayer_->IsInVehicle())
		{
			if (dropObj->isObjType(OBJTYPE_Vehicle))
			{
				obj_Vehicle* vehicle = (obj_Vehicle*)dropObj;

				if (vehicle->wasNetworkLocal)
					vehicle->ReleaseDrivable();

				BYTE b = 1 << 0;
				vehicle->SendActions(b);
			}
		}
#endif

		if(bIsPrimaryInteraction && InputMappingMngr->isPressed(r3dInputMappingMngr::KS_INTERACT))
		{
			//Cynthia: 202 Delay 0.5 second picking up items when shooting.
			if(m_Weapons[m_SelectedWeapon] && (r3dGetTime() < m_Weapons[m_SelectedWeapon]->getLastTimeFired() + 0.5f)) return;

			hudActionUI->enableProgressBlock();
			hudActionUI->setProgress(int(ActionUI_pressedKeyTimer*100));
			if(ActionUI_pressedKeyTimer == 0)
			{
				// inform server about item pick up
				PKT_C2S_PreparingUseNetObject_s n;
				n.clientTime = r3dGetTime();
				p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
			}
			// send request to pickup weapon
			if(ActionUI_pressedKeyTimer > 1.0f) 
			{
				ActionUI_pressedKeyTimer = 0.0f;
				if(dropObj->isObjType(OBJTYPE_SharedUsableItem))
					((SharedUsableItem*)dropObj)->m_lastPickupAttempt = curTime+1.0f; // do not attempt to pick this item for 1 sec (wait for server)

				// hack for notes
				if(dropObj->Class->Name == "obj_Note" || dropObj->Class->Name == "obj_PermanentNote")
				{
					obj_Note* note = (obj_Note*)dropObj;
					if(note->m_GotData) // do not send packet to server again
					{
						hudMain->showReadNote(note->m_Text.c_str());
					}
					else
					{
						PKT_C2S_UseNetObject_s n;
						n.spawnID = toP2pNetId(dropObj->GetNetworkID());
						n.clientTime = r3dGetTime();
						p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
					}
				}
				else if(dropObj->Class->Name == "obj_Door")
				{
					obj_Door* Door = ((obj_Door*)dropObj);
					if (Door->DoorIsDestroy == 0)
						Door->Set(!Door->m_OpenDoor);
				}
				else if(dropObj->Class->Name == "obj_Traps")
				{
					obj_Traps* Traps = ((obj_Traps*)dropObj);
					Traps->Activate(true);
				}
				else if(dropObj->Class->Name == "obj_StoreNPC")
				{
					hudStore->Activate();
				}
				else if(dropObj->Class->Name == "obj_VaultNPC")
				{
					hudVault->Activate();
				}
				else if(dropObj->Class->Name == "obj_RepairBench")
				{
					hudRepair->Activate();
				}
#ifdef MISSIONS
				else if(dropObj->Class->Name == "obj_MissionStateObject")
				{
					// Send interaction notification to the server.
					PKT_C2S_MissionStateObjectUse_s n;
					n.objHash = dropObj->GetHashID();
					p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));

					// If there is text attached to the MissionStateObject, it is
					// assumed to be a Note to display to the user.
					obj_MissionStateObject* mObj = ((obj_MissionStateObject*)dropObj);
					if( mObj->IsNote() )
						hudMain->showReadNote(mObj->GetText());
				}
#endif
#ifdef VEHICLES_ENABLED
				else if (dropObj->isObjType(OBJTYPE_Vehicle))
				{
					RequestVehicleEnter(((obj_Vehicle*)dropObj)->vehicleId);
				}
#endif
				else if(dropObj->isObjType(OBJTYPE_SharedUsableItem))
				{
					if(dropObj->Class->Name == "obj_FarmBlock")
					{
						obj_FarmBlock* Traps = ((obj_FarmBlock*)dropObj);
						if (!Traps->m_Activated && Traps->m_ItemID == WeaponConfig::ITEMID_GardenTrap_Rabbit)
						{
							Traps->Activate(true);
						}
						else {
							PKT_C2S_UseNetObject_s n;
							n.spawnID = toP2pNetId(dropObj->GetNetworkID());
							n.clientTime = r3dGetTime();
							p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
						}
					}
					else {
						PKT_C2S_UseNetObject_s n;
						n.spawnID = toP2pNetId(dropObj->GetNetworkID());
						n.clientTime = r3dGetTime();
						p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
					}
				}
				else if (dropObj->isObjType(OBJTYPE_UAV))
				{
					obj_UAV* uav = (obj_UAV*)dropObj;
					if (uav)
					{
						PKT_C2S_GetUAV_s n;
						n.OwnerDrone = toP2pNetId(gClientLogic().localPlayer_->GetNetworkID());
						n.UAVID =  toP2pNetId(uav->GetNetworkID());
						p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
					}
				}
				else // human
				{
					r3d_assert(dropObj->isObjType(OBJTYPE_Human));
					if(usingMedItem)
					{
						DWORD netID = dropObj->GetNetworkID();
						obj_Player* rmtPlr = (obj_Player*)dropObj;

						wiInventoryItem wi = CurLoadout.Items[m_Weapons[m_SelectedWeapon]->m_BackpackIdx];
						if(wi.quantity > 0 && m_Weapons[m_SelectedWeapon]->isReadyToFire(true, false)) // consumables have rate of fire (cooldown)
						{
							showSlotsInfoTimer = r3dGetTime() + 5.0f; // show weapon info for X seconds
							if(hudMain)
								hudMain->showSlots(true);

							// check if meds and if yes, check cooldown
							bool disableAction = false;
							if((m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_Bandages || m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_Bandages2 || m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_Antibiotics ||
								m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_Painkillers || m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_Medkit || m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_CraftedBandages) 
								)
							{
								if((r3dGetTime() - lastTimeUsedConsumable) < currentConsumableCooldownTime)
									disableAction = true; // cooldown

								if(rmtPlr->CurLoadout.Health > 99)
								{
									if(hudMain) hudMain->showMessage(gLangMngr.getString("InfoMsg_MaxHealthAlready"));
									disableAction = true;
								}
							}

							if((m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_C01Vaccine || m_Weapons[m_SelectedWeapon]->getItemID() == WeaponConfig::ITEMID_C04Vaccine) )
							{
								if(rmtPlr->CurLoadout.Toxic < 1.0f)
								{
									if(hudMain) hudMain->showMessage(gLangMngr.getString("InfoMsg_NoTocixInBlood"));
									disableAction = true;
								}
							}

							if(!disableAction)
							{
								uberAnim_->StartShootAnim();
								m_Weapons[m_SelectedWeapon]->Fire(r3dPoint3D(0,0,0), D3DXMATRIX(), true); // to update last fire time

								localPlayer_UseItem(m_Weapons[m_SelectedWeapon]->m_BackpackIdx, m_Weapons[m_SelectedWeapon]->getItemID(), dropObj->GetPosition(), 0.0f, 0.0f, 0.0f, netID);
								if(--wi.quantity <= 0) {
									wi.Reset();
									OnBackpackChanged(m_Weapons[m_SelectedWeapon]->m_BackpackIdx);
								}
								else
								{
									if(hudMain)
										hudMain->updateSlotInfo(m_Weapons[m_SelectedWeapon]->m_BackpackIdx, wi.quantity);
								}
							}
						}
					}
					else // trade
					{
						// check if we are answering to trade or initiating new one
						obj_Player* netPl = (obj_Player*)dropObj;
						PKT_C2C_TradeRequest_s n;
						n.targetId = toP2pNetId(netPl->GetNetworkID());
						if(lastTradeRequestedFrom == netPl->GetNetworkID()) // answering to request
						{
							n.code = PKT_C2C_TradeRequest_s::TRADE_Answer;
						}
						else // initiate new one
						{
							n.code = PKT_C2C_TradeRequest_s::TRADE_Request;

							char tmpName[128] = {0};
							netPl->GetUserName(tmpName);
							char tempStr[256] = {0};
							sprintf(tempStr, gLangMngr.getString("SentTradeRequestToPlr"), tmpName);
							hudMain->showMessage(tempStr);
							hudMain->addChatMessage(0, "<trade>", (tempStr), 0);
						}
						p2pSendToHost(this, &n, sizeof(n));
					}
				}
				hudActionUI->Deactivate(); 
			}
			ActionUI_pressedKeyTimer += r3dGetFrameTime();
		}
		else if(!bIsPrimaryInteraction && InputMappingMngr->isPressed(r3dInputMappingMngr::KS_SECONDARY_INTERACT))
		{
			//Cynthia: 202 Delay 0.5 second picking up items when shooting.
			if(m_Weapons[m_SelectedWeapon] && (r3dGetTime() < m_Weapons[m_SelectedWeapon]->getLastTimeFired() + 0.5f)) return;

			hudActionUI->enableProgressBlock();
			hudActionUI->setProgress(int(ActionUI_pressedKeyTimer*100));
			if(ActionUI_pressedKeyTimer == 0)
			{
				// inform server about item pick up
				PKT_C2S_PreparingUseNetObject_s n;
				n.clientTime = r3dGetTime();
				p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
			}
			// send request to pickup weapon
			if(ActionUI_pressedKeyTimer > 1.0f) 
			{
				ActionUI_pressedKeyTimer = 0.0f;
				if(dropObj->isObjType(OBJTYPE_SharedUsableItem))
					((SharedUsableItem*)dropObj)->m_lastPickupAttempt = curTime+1.0f; // do not attempt to pick this item for 1 sec (wait for server)

				if( InputMappingMngr->isPressed(r3dInputMappingMngr::KS_SECONDARY_INTERACT ) )
				{
					if(dropObj->Class->Name == "obj_Gravestone")
					{
						obj_Gravestone* gravestone = (obj_Gravestone*)dropObj;
						if(gravestone->m_GotData)
						{
							switch( gravestone->m_KilledBy )
							{
							case obj_Gravestone::KilledBy_Self:
								hudMain->showGravestone( gLangMngr.getString("GravestoneSuicide"), gravestone->m_Victim.c_str(), gravestone->m_Aggressor.c_str() );
								break;

							case obj_Gravestone::KilledBy_Player:
								hudMain->showGravestone( gLangMngr.getString("GravestoneKilledByPlayer"), gravestone->m_Victim.c_str(), gravestone->m_Aggressor.c_str() );
								break;

							default:
							case obj_Gravestone::KilledBy_Zombie:
								hudMain->showGravestone( gLangMngr.getString("GravestoneKilledByZombie"), gravestone->m_Victim.c_str(), gravestone->m_Aggressor.c_str() );
								break;
							}
						}
						else
						{
							PKT_C2S_UseNetObject_s n;
							n.spawnID = toP2pNetId(dropObj->GetNetworkID());
							n.clientTime = r3dGetTime();
							p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
						}
					}
				}
				hudActionUI->Deactivate(); 
			}
			ActionUI_pressedKeyTimer += r3dGetFrameTime();

		}
		else
		{
			hudActionUI->enableRegularBlock();
			ActionUI_pressedKeyTimer = 0.0f;
		}
	}
	else
	{
		if(hudActionUI->isActive())
		{
			hudActionUI->Deactivate();  
		}
		ActionUI_pressedKeyTimer = 0.0f;
	}
}

void obj_Player::AddBloodOnGround(const r3dPoint3D& posOfShot)
{
	{
		r3dPoint3D blood_pos;
		r3dPoint3D blood_norm;
		bool has_collision = false;
		{
			PxRaycastHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
			if(g_pPhysicsWorld->raycastSingle(PxVec3(posOfShot.x, posOfShot.y, posOfShot.z), PxVec3(0, -1, 0), 10.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL), hit, filter))
			{
				blood_pos = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);
				blood_norm = r3dPoint3D(hit.normal.x, hit.normal.y, hit.normal.z);
				has_collision = true;
			}
			else
			{
				//WTF??? How no collision? Is player up in the sky??
			}
		}
		if(has_collision)
		{
			DecalParams params;
			params.Pos		= blood_pos;
			params.Dir		= blood_norm;
			params.TypeID	= GetDecalID( r3dHash::MakeHash("default"), r3dHash::MakeHash("blood_ground") );
			if( params.TypeID != INVALID_DECAL_ID )
				g_pDecalChief->Add( params );
		}
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_MoveTeleport_s& n)
{
	TeleportPlayer(n.teleport_pos, "PlayerMoveTeleport");
}

void obj_Player::OnNetPacket(const PKT_C2C_MoveSetCell_s& n)
{
#ifdef VEHICLES_ENABLED
	if (IsInVehicle())
		return;
#endif

	netMover.SetCell(n);
}

void obj_Player::OnNetPacket(const PKT_C2C_MoveRel_s& n)
{
#ifdef VEHICLES_ENABLED
	if (IsInVehicle())
		return;
#endif

	const CNetCellMover::moveData_s& md = netMover.DecodeMove(n);
	
	if(afterRespawnTimer>0) // check for old player moves and discard them
	{
		if((md.pos - GetPosition()).Length() > 10.0f)
			return;
	}
			
	m_fPlayerRotationTarget = md.turnAngle;
	bodyAdjust_y[1]         = md.bendAngle;
	PlayerState             = (md.state & 0xFF);
	PlayerMoveDir           = (md.state >> 8) & 0xF;
	
	if(PlayerState == PLAYER_IDLE) {
		// limit idle bend angle to some reasonable value in IDLE mode - by some reason animation look like crap when bending there.
		bodyAdjust_y[1] = R3D_CLAMP(bodyAdjust_y[1], -0.4f, 0.4f);
	}

	// calc velocity to reach position on time for next update
	r3dPoint3D vel = netMover.GetVelocityToNetTarget(
		GetPosition(),
		GPP->AI_SPRINT_SPEED * 1.5f,
		1.0f);

	SetVelocity(vel);
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerJump_s& n)
{
	uberAnim_->StartJump();
	return;
}

void obj_Player::ProcessRewardQueue()
{
	r3d_assert(NetworkLocal);
	
	if(rewards_.size() == 0)
		return;
		
	// one reward show per sec
	const float curTime = r3dGetTime();
	if(curTime < lastRewardShowTime_ + 1.0f)
		return;
	lastRewardShowTime_ = curTime;
	
	PKT_S2C_AddScore_s n = *rewards_.begin();
	rewards_.erase(rewards_.begin());

	if((n.ID & 0x8000) == 0)
	{
		GameStats.XP += n.XP;
		GameStats.GD += n.GD;
	}
	else
	{
		// reward without adding score/gd
		n.ID &= ~0x8000;
	}
	
	if(!g_GameRewards) // can happen in server quickjoin from Main_Network
		return;

	const CGameRewards::rwd_s& rwd = g_GameRewards->GetRewardData(n.ID);

	char rewardName[128] = "";	// localized reward name
	char iconName[128] = "";
	if(rwd.IsSet)
	{
		sprintf(rewardName, "Reward_%s", rwd.Name.c_str());
		sprintf(iconName, "$Data/Menu/Rewards/RWD_%s.png", rwd.Name.c_str());
	}
	else
	{
		sprintf(rewardName, "RWD_%d", n.ID);
	}

	// and always show some crap about that reward
	const char* rewardString=gLangMngr.getString(rewardName);
//	hudMain->ShowAchievementCustom(rewardString, "", iconName, "");

//	hudMain->ShowScore(n.XP, 0, n.GD, GameStats.GD, GameStats.XP);

	if(n.XP > 0)
	{
		char tmpStr[64];
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_XPAdded"), n.XP);
		hudMain->showMessage(tmpStr);
	}
	if(n.Rep != 0)
	{
		char tmpStr[64];
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_ReputationAdded"), n.Rep);
		hudMain->showMessage(tmpStr);
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_AddScore_s& n)
{
	if(NetworkLocal)
	{
		rewards_.push_back(n);
	}
	else
	{
		GameStats.XP += n.XP;
		GameStats.GD += n.GD;
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_AddResource_s& n)
{
	r3d_assert(NetworkLocal);
	
	gUserProfile.ProfileData.ResWood  += n.ResWood;
	gUserProfile.ProfileData.ResStone += n.ResStone;
	gUserProfile.ProfileData.ResMetal += n.ResMetal;
	
	char tmpStr[64];
	if(n.ResWood > 0)
	{
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_WoodAdded"), n.ResWood);
		hudMain->showMessage(tmpStr);
	}
	else if(n.ResWood < 0)
	{
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_WoodRemoved"), -n.ResWood);
		hudMain->showMessage(tmpStr);
	}
	if(n.ResStone > 0)
	{
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_StoneAdded"), n.ResStone);
		hudMain->showMessage(tmpStr);
	}
	else if(n.ResStone < 0)
	{
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_StoneRemoved"), -n.ResStone);
		hudMain->showMessage(tmpStr);
	}
	if(n.ResMetal > 0)
	{
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_MetalAdded"), n.ResMetal);
		hudMain->showMessage(tmpStr);
	}
	else if(n.ResMetal < 0)
	{
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_MetalAdded"), -n.ResMetal);
		hudMain->showMessage(tmpStr);
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_SetPlayerVitals_s& n)
{
	if(NetworkLocal && CurLoadout.Toxic==0 && n.Toxic>0 && hudMain)
	{
		hudMain->showMessage(gLangMngr.getString("InfoMsg_ToxicityInBlood"));
	}

	CurLoadout.Health = (float)n.Health;
	CurLoadout.Thirst = (float)n.Thirst;
	CurLoadout.Hunger = (float)n.Hunger;
	CurLoadout.Toxic  = (float)n.Toxic;

	// med system
	CurLoadout.MedBleeding = n.MedSystemStatus&PKT_S2C_SetPlayerVitals_s::MSE_Bleeding?100.0f:0.0f;
	CurLoadout.MedFeverCold = n.MedSystemStatus&PKT_S2C_SetPlayerVitals_s::MSE_Fever?100.0f:0.0f;
	CurLoadout.MedBloodInfection = n.MedSystemStatus&PKT_S2C_SetPlayerVitals_s::MSE_BloodInfection?100.0f:0.0f;
	if(NetworkLocal && hudMain)
	{
		hudMain->setLifeConditions("IconBleed", CurLoadout.MedBleeding>0);
		hudMain->setLifeConditions("IconTemp", CurLoadout.MedFeverCold>0);
		hudMain->setLifeConditions("IconInfection", CurLoadout.MedBloodInfection>0);
	}
	
	//if(NetworkLocal)
	//	r3dOutToLog("My params: %.1f, %.1f, %.1f, %.1f\n", CurLoadout.Health, CurLoadout.Toxic, CurLoadout.Hunger, CurLoadout.Thirst);
}

void obj_Player::OnNetPacket(const PKT_S2C_SetPlayerLoadout_s& n)
{
	// only remote players can change loadout this way
	if(NetworkLocal)
		return;
	
	// create new loadout
	wiCharDataFull slot = CurLoadout;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_WEAPON1 ].itemID = n.WeaponID0;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_WEAPON2 ].itemID = n.WeaponID1;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM1   ].itemID = n.QuickSlot1;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM2   ].itemID = n.QuickSlot2;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM3   ].itemID = n.QuickSlot3;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_ITEM4   ].itemID = n.QuickSlot4;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_ARMOR   ].itemID = n.ArmorID;
	slot.Items[wiCharDataFull::CHAR_LOADOUT_HEADGEAR].itemID = n.HeadGearID;
	slot.BackpackID = n.BackpackID;

	UpdateLoadoutSlot(slot, -1);
}

void obj_Player::OnNetPacket(const PKT_S2C_SetPlayerAttachments_s& n)
{
	// only remote players can change loadout this way
	if(NetworkLocal)
		return;
	
	r3d_assert(n.wid == wiCharDataFull::CHAR_LOADOUT_WEAPON1 || n.wid == wiCharDataFull::CHAR_LOADOUT_WEAPON2);
	Weapon* wpn = m_Weapons[n.wid];
	if(!wpn) {
		return;
	}
	
	// set wpn attachment
	CurLoadout.Attachment[n.wid].attachments[WPN_ATTM_MUZZLE]    = n.Attm.MuzzleID;
	CurLoadout.Attachment[n.wid].attachments[WPN_ATTM_LEFT_RAIL] = n.Attm.LeftRailID;
	wpn->setWeaponAttachmentsByIDs(CurLoadout.Attachment[n.wid].attachments);
	
	// workaround to enable flashlight particle (OnEquip will enable flashlight light)!
	// also we assume that remote player can change attachment only for currently equipped weapon
	wpn->OnUnequip();
	wpn->OnEquip(isFlashlightOn);
}

void obj_Player::OnNetPacket(const PKT_S2C_PlayerRawDamage_s& n)
{
	r3d_assert(NetworkLocal);
	
	wiInventoryItem& itm = CurLoadout.Items[n.bodyPart == 1 ? wiCharDataFull::CHAR_LOADOUT_HEADGEAR : wiCharDataFull::CHAR_LOADOUT_ARMOR];
	if(itm.itemID == 0)
	{
		// valid situation, no armor or armor was swapped on client, will result in durability desync on client-server
		return;
	}
	
	const GearConfig* gc = g_pWeaponArmory->getGearConfig(itm.itemID);
	r3d_assert(gc);
	
	// adjust durability (SERVER_SYNC_POINT ARMOR_DUR)
	float absorb = n.damage * gc->m_damagePerc;
	float left   = n.damage - absorb;
	// armor durability is mapped from [0..m_damageMax] to [0..100]
	float durLoss = left / gc->m_damageMax * 100.0f;
	itm.adjustDurability(-durLoss);
}

void obj_Player::OnNetPacket(const PKT_S2C_SetPlayerWorldFlags_s& n)
{
	CurLoadout.GameFlags = n.flags;
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerSwitchFlashlight_s& n)
{
	r3d_assert(!NetworkLocal);

	isFlashlightOn = n.isFlashlightOn==1?true:false;
	if(m_Weapons[m_SelectedWeapon])
		m_Weapons[m_SelectedWeapon]->switchFlashlight(isFlashlightOn);
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerSwitchWeapon_s& n)
{
	r3d_assert(!NetworkLocal);
	r3d_assert(m_Weapons[n.wid]);
			
	if(m_Weapons[n.wid] == NULL)
	{
#ifndef FINAL_BUILD
		// do not crash in public build
		char plrUserName[64]; GetUserName(plrUserName);
		r3dOutToLog("NET_DEBUG: %s tried to change to empty weapon slot %d", plrUserName, n.wid);
#endif
	}
	else
	{
		uberAnim_->StopGrenadeAnimations();	// we now can switch weapon while grenade animation active, so need to abort

		ChangeWeaponByIndex(n.wid);
	}
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerReadyGrenade_s& n)
{
	//r3dOutToLog("PKT_C2C_PlayerReadyGrenade\n");
	m_SpawnProtectedUntil = 0.0f;
	m_SelectedWeapon = n.wid;
	r3d_assert(m_SelectedWeapon>=0 && m_SelectedWeapon < NUM_WEAPONS_ON_PLAYER);
	r3d_assert(m_Weapons[m_SelectedWeapon]);

	uberAnim_->StartGrenadePinPullAnimation();
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerThrewGrenade_s& n)
{
	m_SpawnProtectedUntil = 0.0f;
	m_SelectedWeapon = n.wid;
	r3d_assert(m_SelectedWeapon>=0 && m_SelectedWeapon < NUM_WEAPONS_ON_PLAYER);
	r3d_assert(m_Weapons[m_SelectedWeapon]);

	D3DXMATRIX fireFrom ;
	fireFrom = DrawRotMatrix ;

	fireFrom._41 = n.fire_from.x ;
	fireFrom._42 = n.fire_from.y ;
	fireFrom._43 = n.fire_from.z ;

	// get the weapon bone.  Fire requires it. 
	D3DXMATRIX weaponBone = uberEquip_->getWeaponBone(GetSkeleton(),fireFrom);

	// remove translation, the rotation is all we wanted for grenades.
	weaponBone._41 = n.fire_from.x ;
	weaponBone._42 = n.fire_from.y ;
	weaponBone._43 = n.fire_from.z ;
	
	GameObject* pGrenade = NULL;
	m_Weapons[m_SelectedWeapon]->Fire(n.fire_to, weaponBone, true, n.holding_delay, n.fire_from, &pGrenade);
	if( pGrenade )
		pGrenade->SetNetworkID(n.spawnID);

	uberAnim_->StartGrenadeThrowAnimation();
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerReload_s& n)
{
	r3d_assert(!NetworkLocal);
	
	Weapon* wpn = m_Weapons[n.WeaponSlot];
	if(!wpn) return;
	
	wpn->StartReloadSequence();
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerFired_s& n)
{
	m_SpawnProtectedUntil = 0.0f;
	m_SelectedWeapon = n.debug_wid;
	r3d_assert(m_SelectedWeapon>=0 && m_SelectedWeapon < NUM_WEAPONS_ON_PLAYER);
	r3d_assert(m_Weapons[m_SelectedWeapon]);

	D3DXMATRIX fireFrom ;
	fireFrom = DrawRotMatrix ;

	fireFrom._41 = n.fire_from.x ;
	fireFrom._42 = n.fire_from.y ;
	fireFrom._43 = n.fire_from.z ;

	// get the weapon bone.  Fire requires it. 
	D3DXMATRIX weaponBone = uberEquip_->getWeaponBone(GetSkeleton(),fireFrom);

	if( m_Weapons[m_SelectedWeapon]->getCategory() == storecat_GRENADE ) {
		// remove translation, the rotation is all we wanted for grenades.
		weaponBone._41 = n.fire_from.x ;
		weaponBone._42 = n.fire_from.y ;
		weaponBone._43 = n.fire_from.z ;
	}
	
	GameObject* pBullet = NULL;
	m_Weapons[m_SelectedWeapon]->Fire(n.fire_to, weaponBone, n.fireSeqNo==0, n.holding_delay, n.fire_from, &pBullet);
	if (pBullet && n.spawnID > 0 && m_Weapons[m_SelectedWeapon]->m_pConfig &&
		WeaponConfig::ITEMID_FlareGun == m_Weapons[m_SelectedWeapon]->m_pConfig->m_itemID)
		pBullet->SetNetworkID(n.spawnID);

	uberAnim_->StartRecoilAnim();
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerHitNothing_s& n)
{
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerHitStatic_s& n)
{
	if(!m_Weapons[m_SelectedWeapon])
		return;

	const ClientGameLogic& CGL = gClientLogic();

	// create decal
	if(n.decalIdx != 0 )
	{
		DecalParams params;
		params.Dir	= n.hit_norm;
		params.Pos	= n.hit_pos;
		params.TypeID	= (uint32_t)(n.decalIdx) - 1;
		if(params.TypeID >= 0 && params.TypeID < (int)g_pDecalChief->GetTypeCount())
		{
			g_pDecalChief->Add( params );
		}
		else
		{
			// some cheat or invalid data
		}
	}

	// impact particle
	if(n.particleIdx != 0)
	{
		uint32_t particleIdx = (uint32_t)(n.particleIdx) - 1;
		if(particleIdx < ImpactParticleEntry::NumRegistered)
		{
			SpawnImpactParticle(particleIdx, n.hit_pos, n.hit_norm);
		}
		else
		{
			// some cheat or invalid data
		}
	}

	if(GameObject* from = GameWorld().GetNetworkObject(n.FromID))
	{
		r3dPoint3D waterSplashPos;
		extern bool TraceWater(const r3dPoint3D& start, const r3dPoint3D& finish, r3dPoint3D& waterSplashPos);
		if( TraceWater( from->GetPosition(), n.hit_pos, waterSplashPos))
		{	
			extern void WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx);
			waterSplashPos.y -= 0.1f;
			WaterSplash(waterSplashPos, 0.1f, 30.0f, 0.04f, 2);
			}
	}

	// add impulse
	GameObject* target = GameWorld().GetObjectByHash(n.hash_obj);
	if(m_Weapons[m_SelectedWeapon]->isImmediateShooting() && target)
		if(target->PhysicsObject)
			target->PhysicsObject->AddImpulseAtPos(-n.hit_norm*m_Weapons[m_SelectedWeapon]->getAmmoSpeed()*m_Weapons[m_SelectedWeapon]->getAmmoMass()/150.0f, n.hit_pos);

	// play near miss sound
	if(m_Weapons[m_SelectedWeapon]->isImmediateShooting() && m_Weapons[m_SelectedWeapon]->getCategory() != storecat_MELEE)
	{
		if(CGL.localPlayer_)
		{
			float dist = dist_Point_to_Line(CGL.localPlayer_->GetPosition(), GetPosition(), n.hit_pos);
			if(dist < 5.0f && rand()%5==0)
			{
				static int bulletHitMissSoundID = -1;
				if(bulletHitMissSoundID==-1)
					bulletHitMissSoundID = SoundSys.GetEventIDByPath("Sounds/BulletHits/Miss");
				SoundSys.PlayAndForget(bulletHitMissSoundID, CGL.localPlayer_->GetPosition());
			}
		}
	}
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerHitStaticPierced_s& n)
{
	PKT_C2C_PlayerHitStatic_s n2;
	r3d_assert(sizeof(n) == sizeof(n2));
	memcpy(&n2, &n, sizeof(n));
	OnNetPacket(n2);
}

void obj_Player::OnNetPacket(const PKT_S2C_PlayerRemoveAttachments_s& n)
{
	OnRemoveAttachments(n.idx);
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerHitDynamic_s& n)
{
	if(!m_Weapons[m_SelectedWeapon])
		return;

	r3dPoint3D dir = (GetPosition()-n.hit_pos).NormalizeTo();

	GameObject* target = GameWorld().GetNetworkObject(n.targetId);
	if(!target) // current player doesn't see target
		return;

	const ClientGameLogic& CGL = gClientLogic();

	// if we hit local player, display hit effect
	if(target->isObjType(OBJTYPE_Human))
	{
		AddBloodOnGround(n.hit_pos);
		SpawnImpactParticle(r3dHash::MakeHash("player"), r3dHash::MakeHash(m_Weapons[m_SelectedWeapon]->getDecalSource()), n.hit_pos, dir);

		if(target->NetworkLocal == false || CGL.localPlayer_==NULL) 
			return;

		// if hit local player
		extern void TPSGameHUD_AddHitEffect(GameObject* from);
		TPSGameHUD_AddHitEffect(this);
	}
	else if(target->isObjType(OBJTYPE_Zombie))
	{
		// hit zombie
		AddBloodOnGround(n.hit_pos);
		SpawnImpactParticle(r3dHash::MakeHash("zombie"), r3dHash::MakeHash(m_Weapons[m_SelectedWeapon]->getDecalSource()), n.hit_pos, dir);
		
		obj_Zombie* z = (obj_Zombie*)target;

		// apply ragdoll force from object position, or up if too close
		r3dPoint3D zragHitPos = z->GetPosition();
		r3dPoint3D zlastTimeHitForce = r3dPoint3D(0, 1, 0);
		if(n.hit_body_bone < GetSkeleton()->NumBones) // zombies use same skeleton as player
			zragHitPos = uberEquip_->getBonePos(n.hit_body_bone, GetSkeleton(), z->GetTransformMatrix());
		if((zragHitPos - this->GetPosition()).LengthSq() > 0.001f)
			zlastTimeHitForce = (zragHitPos - this->GetPosition()).NormalizeTo();

		z->lastTimeHitPos  = zlastTimeHitForce;
		z->lastTimeDmgType = m_Weapons[m_SelectedWeapon]->getCategory();
		z->lastTimeHitBoneID = n.hit_body_bone;
		z->PlayHurtSound();
	}
	else // hit something else, for now that is UAV only
	{
		SpawnImpactParticle(r3dHash::MakeHash("Metal"),r3dHash::MakeHash(m_Weapons[m_SelectedWeapon]->getDecalSource()), n.hit_pos, dir);
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_SpawnExplosion_s& n)
{
	GameObject* from = GameWorld().GetNetworkObject(n.FromID);
	if(from)
	{
		// add decal
		DecalParams params;
		params.Dir		= r3dPoint3D(0,1,0);
		params.Pos		= from->GetPosition();
		params.TypeID	= GetDecalID( r3dHash::MakeHash(""), r3dHash::MakeHash("grenade") );
		if( params.TypeID != INVALID_DECAL_ID )
			g_pDecalChief->Add( params );
		SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("grenade"), from->GetPosition(), r3dPoint3D(0,1,0));

		//	Start radial blur effect
		gExplosionVisualController.AddExplosion(from->GetPosition(), n.radius);
	}
}

void obj_Player::OnNetPacket(const PKT_C2C_PlayerUseItem_s& n)
{
	// assumption: current weapon is set to whatever player is about to use so just play it's animation.
	if(m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->isUsableItem())
		uberAnim_->StartShootAnim();

	if (n.dbg_ItemID==WeaponConfig::ITEMID_AirHorn)
	{
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Effects/AirHorn"), n.pos);
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_PlayerUsedItemAns_s& n)
{
}

void obj_Player::OnNetPacket(const PKT_S2C_BackpackAddNew_s& n)
{
	// NOTE: only for local players
	r3d_assert(NetworkLocal);
	
	if(n.Item.itemID == 'GOLD')
	{
		gUserProfile.ProfileData.GameDollars += n.Item.quantity;
		char tmpStr[64];
		sprintf(tmpStr, gLangMngr.getString("InfoMsg_DollarsPickedUp"), n.Item.quantity);
		hudMain->showMessage(tmpStr);
		return;
	}
	
	r3d_assert(CurLoadout.Items[n.SlotTo].itemID == 0);
	CurLoadout.Items[n.SlotTo] = n.Item;
	
	OnBackpackChanged(n.SlotTo);
	
	// if we picked up weapon with empty hands - switch to it
	if(m_SelectedWeapon == HANDS_WEAPON_IDX && (n.SlotTo == wiCharDataFull::CHAR_LOADOUT_WEAPON1 || n.SlotTo == wiCharDataFull::CHAR_LOADOUT_WEAPON2))
	{
		ChangeWeaponByIndex(n.SlotTo);
	}
	
	hudMain->showMessage(gLangMngr.getString("InfoMsg_NewItemAdded"));

	SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_menu_button_use"), GetPosition());
}

void obj_Player::OnNetPacket(const PKT_S2C_BackpackModify_s& n)
{
	// NOTE: only for local players
	r3d_assert(NetworkLocal);
	
	if(n.SlotTo == 0xFF)
	{
		hudMain->showMessage(gLangMngr.getString("InfoMsg_NoFreeBackpackSlots"));
		if(hudPause->isActive())
			hudPause->showMessage(gLangMngr.getString("InfoMsg_NoFreeBackpackSlots"));
		return;
	}

	if(n.SlotTo == 0xFE)
	{
		hudMain->showMessage(gLangMngr.getString("InfoMsg_TooMuchWeight"));
		if(hudPause->isActive())
			hudPause->showMessage(gLangMngr.getString("InfoMsg_TooMuchWeight"));
		return;
	}

	r3d_assert(n.SlotTo < wiCharDataFull::CHAR_MAX_BACKPACK_SIZE);
	wiInventoryItem& wi = CurLoadout.Items[n.SlotTo];
	// ptumik: workaround for following situation: 
	// user uses medkit and at the same time got killed. locally medkit was already removed from inventory, but server didn't receive that packet yet and killed player, 
	// sending drop all items packet. It will assert here in this case. So decided to add if not killed, only then assert if mismatch in itemID.
	if(!bDead)
	{
		if(wi.itemID != n.dbg_ItemID)
		{
			#ifndef FINAL_BUILD		
			r3dError("backpack was modified %d vs %d", wi.itemID, n.dbg_ItemID);
			#endif
			r3dOutToLog("backpack out of sync\n");
			gClientLogic().Disconnect();
			return;
		}
	}

	if(wi.quantity < n.Quantity)
	{
		if(g_pWeaponArmory->getBackpackConfig(wi.itemID))
			hudMain->showMessage(gLangMngr.getString("InfoMsg_BackpackAdded"));
		else
			hudMain->showMessage(gLangMngr.getString("InfoMsg_ItemAdded"));

		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_menu_button_use"), GetPosition());
	}
	else
		hudMain->showMessage(gLangMngr.getString("InfoMsg_ItemRemoved"));

	wi.quantity = n.Quantity;
	if(wi.quantity <= 0)
	{
		uint32_t wID = wi.itemID;
		wi.Reset();

		// check if removed item was part of weapon attachment system
		{
			const WeaponAttachmentConfig* wac = g_pWeaponArmory->getAttachmentConfig(wID);
			if(wac)
			{
				// check if it is installed in any weapon
				bool isInstalled0, isInstalled1 = false;
				isInstalled0 = (m_Weapons[0] && m_Weapons[0]->getWeaponAttachmentID(wac->m_type)==wac->m_itemID);
				isInstalled1 = (m_Weapons[1] && m_Weapons[1]->getWeaponAttachmentID(wac->m_type)==wac->m_itemID);

				bool hasDuplicate = false;
				for(int i=0; i<CurLoadout.BackpackSize; ++i)
				{
					hasDuplicate = CurLoadout.Items[i].itemID == wID && CurLoadout.Items[i].quantity > 0;
					if(hasDuplicate)
						break;
				}

				if((isInstalled0 || isInstalled1))// && !hasDuplicate)
				{
					// remove it from weapon
					if(isInstalled0)
						RemoveWpnAttm(0, wac->m_type, n.IsAttachmentReplyReq);
					if(isInstalled1)
						RemoveWpnAttm(1, wac->m_type, n.IsAttachmentReplyReq);
				}
			}
		}

	}
	
	OnBackpackChanged(n.SlotTo);
}

void obj_Player::OnNetPacket(const PKT_S2C_BackpackReplace_s& n)
{
	// NOTE: only for local players
	r3d_assert(NetworkLocal);

	r3d_assert(n.SlotTo < wiCharDataFull::CHAR_MAX_BACKPACK_SIZE);
	wiInventoryItem& wi = CurLoadout.Items[n.SlotTo];
	wi = n.Item;
	
	OnBackpackChanged(n.SlotTo);
}


void obj_Player::OnNetPacket(const PKT_S2C_BackpackUnlock_s& n)
{
	if(hudPause && hudPause->isUILocked())
	{
		hudPause->UnlockUI();
		hudPause->reloadBackpackInfo();
	}
	
	if(hudRepair && hudRepair->isUILocked())
	{
		hudRepair->unlockUI();
		hudRepair->reloadBackpackInfo();
	}

	if(hudCraft && hudCraft->isUILocked())
	{
		hudCraft->unlockUI();
		hudCraft->reloadBackpackInfo();
	}

	// if we have something selected (like vehicle) refresh it parameters
	if(hudActionUI && hudActionUI->isActive())
	{
		hudActionUI->Deactivate(); 
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_RepairItemAns_s& n)
{
	gUserProfile.ProfileData.GameDollars = n.GameDollars;
	
	if(n.SlotFrom == 0xFF)
	{
		if(hudRepair)
		{
			hudRepair->setErrorMsg(gLangMngr.getString("FR_RepairNotEnoughGD"));
		}
		return;
	}
	
	CurLoadout.Items[n.SlotFrom].Var3 = n.Var3;
}

void obj_Player::OnNetPacket(const PKT_S2C_CraftAns_s& n)
{
	if(hudCraft)
	{
		if(n.ans == 1)
			hudCraft->unlockUI();
		else
			hudCraft->setErrorMsg(gLangMngr.getString("FailedToCraftItem"));
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_InventoryAddNew_s& n)
{
	// NOTE: only for local players
	r3d_assert(NetworkLocal);
	
#ifndef FINAL_BUILD	
	ConPrint("new item added to inventory, %I64d %dx%d, %d %d", n.Item.InventoryID, n.Item.itemID, n.Item.quantity, n.Item.Var1, n.Item.Var2);
#endif	
	
	wiInventoryItem* itm = gUserProfile.getFreeInventorySlot();
	if(itm == NULL)
	{
		//TODO: we have a desync, exit the game somehow
		r3dError("out of inventory slots");
		return;
	}
	
	*itm = n.Item;

	if(hudVault && hudVault->isActive())
	{
		hudVault->updateInventory();
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_InventoryModify_s& n)
{
	// NOTE: only for local players
	r3d_assert(NetworkLocal);

	wiInventoryItem* wi = gUserProfile.getInventorySlot(n.InventoryID);
	r3d_assert(wi);
	if(!wi)
		return;

#ifndef FINAL_BUILD	
	ConPrint("inventory modified %I64d %d %d->%d", wi->InventoryID, wi->itemID, wi->quantity, n.Quantity);
#endif	

	wi->quantity = n.Quantity;
	if(wi->quantity <= 0)
		wi->Reset();
	
	if(hudVault && hudVault->isActive())
	{
		hudVault->updateInventory();
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_InventoryOpAns_s& n)
{
	// we should receive this only while hudVault is active
	r3d_assert(hudVault);
	r3d_assert(hudStore);
	r3d_assert(hudSafelock);

	if(n.OpAns != PKT_S2C_InventoryOpAns_s::ANS_Success)
	{
		const char* errMsg = NULL;
		switch(n.OpAns)
		{
		case PKT_S2C_InventoryOpAns_s::ANS_NoItem:
			errMsg = gLangMngr.getString("InGameUI_ErrorMsg_NoItem");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_NoPrice:
			errMsg = gLangMngr.getString("InGameUI_ErrorMsg_NoPrice");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_NoMoney:
			errMsg = gLangMngr.getString("InGameUI_ErrorMsg_NoMoney");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_NoBackpackSpace:
			errMsg = gLangMngr.getString("InGameUI_ErrorMsg_NoBackpackSpace");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_WeightLimit:
			errMsg = gLangMngr.getString("InGameUI_ErrorMsg_WeightLimit");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_NoInventorySpace:
			if(hudSafelock->isUILocked())
				errMsg = gLangMngr.getString("InGameUI_ErrorMsg_SafelockNoSpace");
			else
				errMsg = gLangMngr.getString("InGameUI_ErrorMsg_NoInventorySpace");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_Desync:
			if(hudSafelock->isUILocked())
			{
				// special handling, if desynced, close safelock and display message in hud
				hudSafelock->unlockUI();
				hudSafelock->Deactivate();
				hudMain->showMessage(gLangMngr.getString("InGameUI_ErrorMsg_SafelockDesync"));
			}
			else
				errMsg = gLangMngr.getString("InGameUI_ErrorMsg_Desync");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_BadTarget:
			errMsg = gLangMngr.getString("InGameUI_ErrorMsg_BadTarget");
			break;
		case PKT_S2C_InventoryOpAns_s::ANS_Fail:
			errMsg = gLangMngr.getString("InGameUI_ErrorMsg_Fail");
			break;
		default:
			errMsg = "Unknown error";
			break;
		}
		
		if(hudVault->isUILocked())
			hudVault->setErrorMsg(errMsg);
		if(hudStore->isUILocked())
			hudStore->setErrorMsg(errMsg);
		if(hudSafelock->isUILocked())
			hudSafelock->setErrorMsg(errMsg);
	}
	else
	{
		if(hudVault->isUILocked())
			hudVault->unlockUI();
		if(hudSafelock->isUILocked())
			hudSafelock->unlockUI();
		gUserProfile.ProfileData.GamePoints  = n.GamePoints;
		gUserProfile.ProfileData.GameDollars = n.GameDollars;
		if(hudStore->isUILocked())
		{
			hudStore->updateMoney(n.GamePoints, n.GameDollars);
			hudStore->unlockUI();
		}
	}
}

void obj_Player::OnNetPacket(const PKT_S2C_PlayerWeapDataRepReq_s& n)
{
	r3d_assert(NetworkLocal);
	r3d_assert(!bDead);

	//
	// send hash of currently equipped weapon configs
	//
	PKT_C2S_PlayerWeapDataRepAns_s n2;
	r3d_assert(2 == R3D_ARRAYSIZE(n2.weaponsDataHash));
	for(int i=0; i<2; i++)
	{
		n2.weaponsDataHash[i] = 0;
		n2.debug_wid[i] = 0;
		if(m_Weapons[i] == NULL)
			continue;

		n2.weaponsDataHash[i] = m_Weapons[i]->getConfig()->GetClientParametersHash();
		n2.debug_wid[i] = m_Weapons[i]->getItemID();
		m_Weapons[i]->getConfig()->copyParametersTo(n2.debug_winfo[i]);
	}
		
	p2pSendToHost(this, &n2, sizeof(n2), true);
}

void obj_Player::OnNetPacket(const PKT_C2C_TradeRequest_s& n)
{
	r3d_assert(NetworkLocal);

	char tmpStr[256] = {0};
	
	// cases without targetId
	switch(n.code)
	{
		case PKT_C2C_TradeRequest_s::TRADE_Busy:
			hudMain->showMessage(gLangMngr.getString("TradeOtherPlayerBusy"));
			return;

		case PKT_C2C_TradeRequest_s::TRADE_Close:
			if(hudTrade->isActive())
				hudTrade->Deactivate();
			return;
			
		case PKT_C2C_TradeRequest_s::TRADE_NoSpace:
			hudTrade->showNoSpace();
			return;
		case PKT_C2C_TradeRequest_s::TRADE_NoWeight:
			hudTrade->showNoWeight();
			return;
	}

	obj_Player* target = (obj_Player*)GameWorld().GetNetworkObject(n.targetId);
	r3d_assert(target && target->isObjType(OBJTYPE_Human));

	char tmpName[128];
	target->GetUserName(tmpName);
	
	switch(n.code)
	{
		default:	
			r3dError("trade code %d", n.code);

		case PKT_C2C_TradeRequest_s::TRADE_Request:
		{
			if(hudTrade->isActive())
				return;
				
			sprintf(tmpStr, gLangMngr.getString("ReceivedTradeRequestFromPlr"), tmpName);
			hudMain->showMessage(tmpStr);
			hudMain->addChatMessage(0, "<trade>", (tmpStr), 0);

			lastTradeRequestedFrom = n.targetId;
			return;
		}
		
		case PKT_C2C_TradeRequest_s::TRADE_Open:
			lastTradeRequestedFrom = 0;
			// check if we can open trade window, if not, send close event and discard this trade
			if(hudPause->isActive() || hudVault->isActive() || hudSafelock->isActive() || hudStore->isActive() || hudAttm->isActive() || 
				hudMain->isSafelockPinActive() || hudMain->isChatInputActive() || hudRepair->isActive() || hudCraft->isActive() || IsInVehicle())
			{
				PKT_C2C_TradeRequest_s n;
				n.code     = PKT_C2C_TradeRequest_s::TRADE_Close;
				n.targetId = 0;
				p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
			}
			else
				hudTrade->Activate(tmpName);
			return;

		case PKT_C2C_TradeRequest_s::TRADE_Confirm:
			hudTrade->setOpposeTradeIndicator(true);
			return;
		case PKT_C2C_TradeRequest_s::TRADE_Clear:
			hudTrade->setOpposeTradeIndicator(false);
			return;
	}
}

void obj_Player::OnNetPacket(const PKT_C2C_TradeItem_s& n)
{
	r3d_assert(NetworkLocal);
	if(!hudTrade->isActive())
		return;

	hudTrade->setOffer(n.TradeSlot, n.Item);
}

BOOL obj_Player::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	R3DPROFILE_FUNCTION("obj_Player::OnNetReceive");
	r3d_assert(!(ObjFlags & OBJFLAG_JustCreated)); // make sure that object was actually created before processing net commands

#undef DEFINE_GAMEOBJ_PACKET_HANDLER
#define DEFINE_GAMEOBJ_PACKET_HANDLER(xxx) \
	case xxx: { \
		const xxx##_s&n = *(xxx##_s*)packetData; \
		r3d_assert(packetSize == sizeof(n)); \
		OnNetPacket(n); \
		return TRUE; \
	}

	switch(EventID)
	{
	default: return FALSE;
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_MoveTeleport);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_MoveSetCell);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_MoveRel);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerJump);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_AddScore);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_AddResource);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_SetPlayerVitals);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_SetPlayerLoadout);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_SetPlayerAttachments);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_SetPlayerWorldFlags);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_PlayerRawDamage);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerSwitchWeapon);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerSwitchFlashlight);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerReadyGrenade);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerThrewGrenade);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerReload);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerFired);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerHitNothing);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerHitStatic);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerHitStaticPierced);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerHitDynamic);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_PlayerRemoveAttachments);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_SpawnExplosion);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_PlayerUseItem);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_PlayerUsedItemAns);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_BackpackAddNew);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_BackpackModify);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_BackpackReplace);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_BackpackUnlock);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_RepairItemAns);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_InventoryAddNew);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_InventoryModify);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_InventoryOpAns);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_PlayerWeapDataRepReq);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_TradeRequest);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_TradeItem);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_CraftAns);
	}

	return TRUE;
}

bool obj_Player::hasScopeMode() const 
{
	if(bDead)
		return false;

	bool isFirstPerson = g_camera_mode->GetInt()==2 && NetworkLocal;
	return (m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->hasScopeMode(isFirstPerson)); 
}

bool obj_Player::hasScopeMode_hidePlayerModel() const 
{
	bool isFirstPerson = g_camera_mode->GetInt()==2 && NetworkLocal;
	if(m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getScopeConfig(isFirstPerson))
		return m_Weapons[m_SelectedWeapon]->getScopeConfig(isFirstPerson)->hide_player_model;
	
	return false;
}

void obj_Player::DrawReticle()
{
	if(!NetworkLocal)
		return;
	
	g_RenderScopeEffect = 0;
	
	if(bDead || gClientLogic().disconnectStatus_ || (m_Weapons[m_SelectedWeapon] == NULL && !(r_hud_filter_mode->GetInt()==HUDFilter_NightVision)))
	{
		CurrentRig.allowScope = false; // to make sure that scope will not be rendered when you are dead
		g_ScopeFeedbackTimer = 0.0f;
		return;
	}

	bool isFirstPerson = g_camera_mode->GetInt()==2 && NetworkLocal;
	r3dRenderer->SetRenderingMode(R3D_BLEND_PUSH|R3D_BLEND_NZ|R3D_BLEND_ALPHA);

	float cx = r3dRenderer->ScreenW/2;
	float cy = r3dRenderer->ScreenH/2;
	float cy2 = r3dRenderer->ScreenH*0.32f;

	// draw laser pointer if any
	if(m_Weapons[m_SelectedWeapon])
	{
		if(PlayerState != PLAYER_MOVE_SPRINT && !m_Weapons[m_SelectedWeapon]->isReloading() && g_camera_mode->GetInt() == 2 && !(hudAttm && hudAttm->isActive()) && !IsSwimming())
		{
			r3dPoint3D laserPos;
			if(m_Weapons[m_SelectedWeapon]->hasLaserPointer(laserPos))
			{
				r3dRenderer->SetRenderingMode( R3D_BLEND_PUSH | R3D_BLEND_ADD | R3D_BLEND_ZC );

				float size  = R3D_MIN(r3dRenderer->ScreenW*0.02f, r3dRenderer->ScreenH*0.02f);

				// gradually decrease size over distance
				const float maxDecDist = 20.0f;
				float dist = (gCam - laserPos).Length();
				if(dist >= maxDecDist) dist = maxDecDist;
				size = R3D_LERP(size, size*0.25f, dist / maxDecDist);

				r3dPoint3D laserPosScreen;
				r3dProjectToScreen(laserPos, &laserPosScreen);
				if(m_isAiming)
				{
					laserPosScreen.x = cx;
					laserPosScreen.y = cy;
				}
				r3dColor laserColor = r3dColor::red;
				laserColor.A = (int)R3D_LERP(196.0f, 0.0f, dist/400.0f);
				r3dDrawBox2D(float(laserPosScreen.x-(size/2)), float(laserPosScreen.y-(size/2)), size, size, laserColor, s_laserPointerTex);

				r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
			}
		}
	}

	// check if blinded by flash light or laser
	{
		// check all other players, see if they are close enough, have attachment and pointing it at you
		ClientGameLogic& CGL = gClientLogic();
		for(int i=0; i<MAX_NUM_PLAYERS; ++i)
		{
			obj_Player* plr = CGL.GetPlayer(i);
			if(g_bEditMode)
			{
				plr = this;
				i = MAX_NUM_PLAYERS;
			}
			if(!plr)
				continue;
			if(plr == this && !g_bEditMode)
				continue;
			if( !(plr->m_SelectedWeapon >= 0 && plr->m_SelectedWeapon <= NUM_WEAPONS_ON_PLAYER) )
				continue;
			if(plr->m_Weapons[plr->m_SelectedWeapon] == NULL)
				continue;
			if(!plr->isFlashlightOn)
				continue;
			int attmID = plr->m_Weapons[plr->m_SelectedWeapon]->getWeaponAttachmentID(WPN_ATTM_LEFT_RAIL);
			bool hasFlashlight = attmID == 400018 || attmID == 400022 || plr->m_Weapons[plr->m_SelectedWeapon]->getItemID()==101306;
			bool hasLaser = attmID == 400004 || attmID == 400021;

			if( hasFlashlight || hasLaser ) // laser, flashlight
			{
				float dist = (plr->GetPosition() - gCam).Length();

				// other players do not have FPS model, so just use muzzle pos
				D3DXMATRIX weaponBone = plr->uberEquip_->getWeaponBone(plr->GetSkeleton(), plr->DrawFullMatrix);
				r3dPoint3D attmPos = plr->m_Weapons[plr->m_SelectedWeapon]->getMuzzlePos(weaponBone);
				{
					// check if attachment is visible
					r3dPoint3D attmScreenPos;
					if(!r3dProjectToScreen(attmPos, &attmScreenPos))
						continue;

					r3dPoint3D attmDir = r3dPoint3D(weaponBone._11, weaponBone._12, weaponBone._13);
					if(attmDir.Dot(gCam.vPointTo)>0.0f)
						continue;
					float distToCamera = (attmPos - gCam).Length();

					// check that attachment is visible and not blocked
					PxRaycastHit hit;
					PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER),0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
					r3dPoint3D dirToAttm = (attmPos - gCam).NormalizeTo();
					bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(gCam.x, gCam.y, gCam.z), PxVec3(dirToAttm.x, dirToAttm.y, dirToAttm.z), distToCamera, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
					if(!hitResult)
					{
						/*{
							float size = r3dRenderer->ScreenH*0.1f;
							r3dDrawBox2DRotate(attmScreenPos.x, attmScreenPos.y, size, size, 0.0f, r3dColor::white, s_lightHaloTex);
						}*/

						float viewDot = -attmDir.Dot(dirToAttm);
						if(hasFlashlight) // flashlight
						{
							if(viewDot < 0.75f)
								viewDot = 0;
							else
								viewDot = viewDot*4-3.0f;
						}
						else // laser
						{
							if(viewDot < 0.997f)
								viewDot = 0;
							else
								viewDot = ((viewDot*100.0f)-99.7f); // convert from [0.99, 1.0] to [0, 1]
						}
						
						{
							r3dTexture* tex = (hasLaser)?s_laserFlashTex:s_flashlightFlashTex;
							float size = R3D_MIN(r3dRenderer->ScreenW, r3dRenderer->ScreenH);
							float scale = viewDot*0.5f;
							if(distToCamera < 10.0f)
								scale = scale * (1.0f+2.0f*R3D_CLAMP((distToCamera/10.0f), 0.0f, 1.0f));
							else if(attmID == 400018 || attmID == 400022) // flashlight, max range 20m
								scale = scale * (3.0f-(3*(R3D_CLAMP((distToCamera-10.0f)/10.0f, 0.0f, 0.95f))));
							else // laser, max range 40m
								scale = scale * (3.0f-(3*(R3D_CLAMP((distToCamera-10.0f)/30.0f, 0.0f, 1.0f))));
							size = size * scale;
							r3dDrawBox2DRotate(attmScreenPos.x, attmScreenPos.y, size, size, r3dSin(r3dGetTime()*0.01f)*360.0f, r3dColor::white, tex);
							r3dDrawBox2DRotate(attmScreenPos.x, attmScreenPos.y, size, size, r3dCos(r3dGetTime()*0.012f)*360.0f, r3dColor::white, tex);
						}
					}
				}
			}
		}
	}
	bool scopeRendering = hasScopeMode() && CurrentRig.allowScope ;

	if(hudMain)//Cynthia: 252 show reticle in nightvision mode while disable it when aiming.
		hudMain->setTPSReticleVisibility((g_camera_mode->GetInt()==0 && !scopeRendering  && m_Weapons[m_SelectedWeapon] && m_SelectedWeapon != HANDS_WEAPON_IDX || uavViewActive_)?1:0);

	const ScopeConfig* scopeConfig = NULL;
	if(m_Weapons[m_SelectedWeapon])
		scopeConfig = m_Weapons[m_SelectedWeapon]->getScopeConfig(isFirstPerson);
	if(r_hud_filter_mode->GetInt()==HUDFilter_NightVision && !scopeRendering)// nightvision
	{
		scopeConfig = g_pWeaponArmory->getScopeConfig("nvgframe");
		scopeRendering = true;
	}
	r3d_assert(scopeConfig);

	if(scopeRendering) 	// draw sniper scope
	{
		g_RenderScopeEffect = 1;
		gPFX_ScopeEffect.SetScopeReticle(scopeConfig->scope_reticle);
		gPFX_ScopeEffect.SetScopeNormal(scopeConfig->scope_normal);
		gPFX_ScopeEffect.SetScopeMask(scopeConfig->scope_mask);
		gPFX_ScopeEffect.SetScopeBlurMask(scopeConfig->scopeBlur_mask);

		ScopeMaterialParams params = gPFX_ScopeEffect.GetMaterialParams() ;
		params.Lighting = scopeConfig->lighting ;
		gPFX_ScopeEffect.SetMaterialParams( params ) ;

		float scaleScope = scopeConfig->scale;
		float scopeScaleMod = 0.20f;
		if(g_camera_mode->GetInt() == 2)
			scopeScaleMod = 0.10f;

		scaleScope += g_ScopeFeedbackTimer*scopeScaleMod;
		if(g_ScopeFeedbackTimer>0)
		{
			g_ScopeFeedbackTimer = R3D_MAX(g_ScopeFeedbackTimer-r3dGetFrameTime()*8.0f, 0.0f);
		}
		gPFX_ScopeEffect.SetScopeScale(scaleScope);
	}
#ifndef FINAL_BUILD
	else if(g_bEditMode && r_show_player_debug_data->GetBool() && scopeConfig->reticule) // draw reticule only in editor, in game reticule is in flash
	{
		float rw = (float)scopeConfig->reticule->GetWidth();
		float rh = (float)scopeConfig->reticule->GetHeight();
		float yy = g_camera_mode->GetInt() == 1?cy2:cy;
		r3dDrawBox2D(float(cx-(rw/2)), float(yy-(rh/2)), rw, rh, r3dColor::white, scopeConfig->reticule);
	}
#endif

	if(!scopeRendering)
		g_ScopeFeedbackTimer = 0.0f;

	if(m_HitMarkerFadeout>0)
	{
		float rw = (float)m_HitMarkerTex->GetWidth();
		float rh = (float)m_HitMarkerTex->GetHeight();
		r3dColor clr = r3dColor::white; clr.A = BYTE(m_HitMarkerFadeout*255.0f);
		if(m_isInScope)
			r3dDrawBox2D(float(cx-(rw/2)), float(cy-(rh/2)), rw, rh, r3dColor::white, m_HitMarkerTex);
		else
		{
			float yy = g_camera_mode->GetInt() == 1?cy2:cy;
			r3dDrawBox2D(float(cx-(rw/2)), float(yy-(rh/2)), rw, rh, r3dColor::white, m_HitMarkerTex);
		}
	}

	if(r3dGetTime() < m_BandagesEffectTime)
	{
		// !! hardcoded for 0.5 duration
		float time = m_BandagesEffectTime - r3dGetTime(); // values: 0.5...0.0
		float infl = 0;
		time -= 0.25f;  // 0.25...-0.25
		time *= 4.0f; // 1.0...-1.0
		time = R3D_ABS(time); // 1.0...0.0...1.0
		time = 1.0f-time; // 0.0...1.0...0.0
		time = pow(time, 2.0f); // curve
		infl = R3D_CLAMP(time, 0.0f, 1.0f);

		r3dColor24 effectColor(153, 217, 255, 196);
		effectColor = effectColor * infl;
		r3dDrawBox2D(0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, effectColor);
	}
	
	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
}

void obj_Player::DrawLabel()
{
	r3dRenderer->SetMaterial(NULL);

	struct PushRestoreStates
	{
		PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
		}

		~PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}
	} pushRestoreStates; (void)pushRestoreStates;

	if(NetworkLocal && gClientLogic().localPlayer_)
	{
		// show a warning if FPS or PING is low
		if (m_ZombieRepelentTime>0 && hudPause && !hudPause->isActive())
		{
			float time = m_ZombieRepelentTime - r3dGetTime();
			Font_Label->PrintF(2, 0, r3dColor(243,43,37), "Repelent Time: %.1f",time);
			if (time<=0)
				m_ZombieRepelentTime=0;
		}

		if (isGameHardcore == true) //gamehardcore
		{
			int Players = 0;
			for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
			{
				if(obj->isObjType(OBJTYPE_Human))
				{
					Players++;
				}
			}
			char text[512];
			sprintf(text,"Players Online: %i Missing: %i", Players,HardMaxPlayers-Players);
			Font_Label->PrintF((r3dRenderer->ScreenW/2)-(strlen(text)+50), 0, r3dColor(243,43,37), "%s", text);
		}
		float fps = r3dGetAvgFPS();
		//if(fps < 10.0f)
		Font_Label->PrintF((r3dRenderer->ScreenW) / 2, 0, r3dColor(243,43,37), "FPS: %.1f", r3dGetAvgFPS());
        if(gClientLogic().net_->lastPing_ > 50)
            Font_Label->PrintF((r3dRenderer->ScreenW) / 2, 15, r3dColor(255,0,0), "PING: %i", gClientLogic().net_->lastPing_);
        else Font_Label->PrintF((r3dRenderer->ScreenW) / 2, 15, r3dColor(0,255,68), "PING: %i", gClientLogic().net_->lastPing_);
	}

#ifndef FINAL_BUILD

	if (gUserProfile.ProfileData.isDevAccount && d_show_dev_event_info->GetBool())
	{
		r3dColor clanTagColor;
		switch (ClanTagColor)
		{
		default: clanTagColor = r3dColor::yellow;
		case 1: clanTagColor = r3dColor24(170,0,0); break;
		case 2: clanTagColor = r3dColor24(166,120,12); break;
		case 3: clanTagColor = r3dColor24(16,164,158); break;
		case 4: clanTagColor = r3dColor24(32,164,20); break;
		case 5: clanTagColor = r3dColor24(16,78,164); break;
		case 6: clanTagColor = r3dColor24(150,16,164); break;
		case 7: clanTagColor = r3dColor24(68,68,68); break;
		case 8: clanTagColor = r3dColor24(164,164,164); break;
		}

		r3dPoint3D scrCoord;
		if(r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1.8f, 0), &scrCoord))
		{
			char plrUserName[64]; GetUserName(plrUserName);

			if(ClanID!=0)
				Font_Editor->PrintF(scrCoord.x - 50, scrCoord.y, clanTagColor, "[%s]", ClanTag);
			
			Font_Editor->PrintF(scrCoord.x, scrCoord.y, r3dColor::yellow, "%s", plrUserName);

			r3dColor health = r3dColor::yellow;
			if (CurLoadout.Health < 25)
				health = r3dColor::red;
			else if (CurLoadout.Health < 50)
				health = r3dColor::orange; 
			Font_Editor->PrintF(scrCoord.x, scrCoord.y + 20, health, "%.0f", CurLoadout.Health);
		}
	}

	// draw player CustomerID/gamertag for admins
	if(!NetworkLocal && gUserProfile.ProfileData.isDevAccount && d_dev_show_player_names->GetBool())
	{
		float dist = (gCam - GetPosition()).Length();
		int a    = int(R3D_MAX(1.0f - (dist / 300.0f), 0.1f)*255.0f);
		r3dPoint3D scrCoord;
		if(r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1.8f, 0), &scrCoord))
		{
			char plrUserName[64]; GetUserName(plrUserName);
			if(!d_dev_do_not_show_player_id->GetBool())
				Font_Editor->PrintF(scrCoord.x, scrCoord.y+12, r3dColor(255,255,255,a), "ID:%d", CustomerID);
			if(ClanID!=0)
				Font_Editor->PrintF(scrCoord.x, scrCoord.y,    r3dColor(255,255,255,a), "[%s]%s", ClanTag, plrUserName);
			else
				Font_Editor->PrintF(scrCoord.x, scrCoord.y,    r3dColor(255,255,255,a), "%s", plrUserName);
		}
	}
#endif

#ifndef FINAL_BUILD
	if(!g_bEditMode && NetworkLocal && r_show_player_debug_data->GetInt() >= 4 )
	{
#ifdef MISSIONS
		std::map< uint32_t, std::list< MissionActionData* > > sortedByMission;
		for( std::map< uint32_t, MissionActionData >::iterator mapIter = missionsActionData.begin();
			 mapIter != missionsActionData.end(); ++mapIter )
		{
			sortedByMission[ mapIter->second.missionID ].push_back( &mapIter->second );
		}
		r3dPoint3D scrCoord = r3dPoint3D( 1550, 100, 0 );
		Font_Editor->PrintF(scrCoord.x, scrCoord.y, r3dColor(255, 255, 128), "Mission HUD should be %s", (hudMain->isMissionHUDVisible()) ? "On" : "Off" ); scrCoord.y += 12;
		for( std::map< uint32_t, std::list< MissionActionData* > >::iterator mapIter = sortedByMission.begin();
			mapIter != sortedByMission.end(); ++mapIter )
		{
			Font_Editor->PrintF( scrCoord.x, scrCoord.y, r3dColor( 255, 255, 128 ), "%s", gLangMngr.getString(mapIter->second.front()->missionName) );
			scrCoord.y += 12;
			for( std::list< MissionActionData* >::iterator listIter = mapIter->second.begin();
				 listIter != mapIter->second.end(); ++listIter )
			{
				char tmpStr[ 128 ];
				r3dPoint3D pos = GetPosition();
				switch( (*listIter)->actionType )
				{
				case 1:	// ACT_Goto
					r3dscpy( tmpStr, "Goto location marked on map." );
					break;
				case 2: // ACT_Item
					switch( (*listIter)->subActionType )
					{
					case 1: // ITEM_Collect
						sprintf( tmpStr, "Collect %d/%d", (*listIter)->count, (*listIter)->amount ); break;
					case 2: // ITEM_Drop
						sprintf( tmpStr, "Drop %d/%d", (*listIter)->count, (*listIter)->amount ); break;
					case 3: // ITEM_Use
						sprintf( tmpStr, "Use %d/%d", (*listIter)->count, (*listIter)->amount ); break;
					case 4: // ITEM_Craft
						sprintf( tmpStr, "Craft %d/%d", (*listIter)->count, (*listIter)->amount ); break;
					default:
						sprintf( tmpStr, "Unknown Item Action(%d)", (*listIter)->subActionType ); break;
					}
					break;
				case 3: // ACT_Kill
						sprintf( tmpStr, "Kill %d/%d", (*listIter)->count, (*listIter)->amount );
					break;
				case 4: // ACT_State
					switch( (*listIter)->subActionType )
					{
					case 1: // STATE_Inactive
						sprintf( tmpStr, "Set to Inactive: %s", ((*listIter)->count == 1) ? "Inactive" : "Active" ); break;
					case 2: // STATE_Active
						sprintf( tmpStr, "Set to Active: %s", ((*listIter)->count == 1) ? "Inactive" : "Active" ); break;
					default:
						sprintf( tmpStr, "Unknown Action State(%d)", (*listIter)->subActionType ); break;
					}
					break;
				}
				r3dColor color( 255, 255, 128 );
				if( (*listIter)->extents.x >= 0.5f || (*listIter)->extents.y >= 0.5f || (*listIter)->extents.z >= 0.5f )
				{
					if ( pos.x <= ((*listIter)->position.x + (*listIter)->extents.x)  && pos.x >= ((*listIter)->position.x - (*listIter)->extents.x) &&
						 pos.y <= ((*listIter)->position.y + (*listIter)->extents.y)  && pos.y >= ((*listIter)->position.y - (*listIter)->extents.y) &&
						 pos.z <= ((*listIter)->position.z + (*listIter)->extents.z)  && pos.z >= ((*listIter)->position.z - (*listIter)->extents.z) )
					{
						color = r3dColor( 0, 255, 0 );
					}
					else
					{
						color = r3dColor( 255, 0, 0 );
					}
				}
				Font_Editor->PrintF( scrCoord.x + 5, scrCoord.y, color, "(%d)(%s)%s", (*listIter)->actionID, ((*listIter)->completed) ? "Y" : "N" , tmpStr );
				scrCoord.y += 12;
			}
		}
#endif
	}
#endif

	/*
	{
	  // muzzler position debug
	  D3DXMATRIX mm = uberEquip_->getWeaponBone(GetSkeleton(),GetPosition());
	  r3dPoint3D pos = m_Weapons[m_SelectedWeapon]->getMuzzlePos(mm);
	  r3dDrawLine3D(pos, pos + r3dPoint3D(mm._11, mm._12, mm._13), gCam, 0.005f, r3dColor(255, 255, 255));
	  r3dDrawLine3D(pos, pos + r3dPoint3D(0, 1, 0), gCam, 0.005f, r3dColor(255, 255, 255));

	 // r3dDrawLine3D(gTempTargetPos, gTempTargetPos + r3dPoint3D(mm._11, mm._12, mm._13), gCam, 0.005f, r3dColor(255, 0, 0));
	 // r3dDrawLine3D(gTempTargetPos, gTempTargetPos + r3dPoint3D(0, 1, 0), gCam, 0.005f, r3dColor(255, 0, 0));
	  
	 // r3dDrawLine3D(gTempTargetPos, gTempMuzzlePos, gCam, 0.005f, r3dColor(0,255,0));
	  r3dRenderer->Flush();
	}
	*/

#ifndef FINAL_BUILD
	if(NetworkLocal && ( !g_bEditMode || CreatedFromEditor ) && r_show_player_debug_data->GetBool())
	{
		Font_Label->PrintF(10, r3dRenderer->ScreenH-240,r3dColor(255,255,255), "m_Health %.2f", CurLoadout.Health);
		Font_Label->PrintF(10, r3dRenderer->ScreenH-220,r3dColor(255,255,255), "DmgRcvd %.2f", lastDamageRcvd);
		Font_Label->PrintF(10, r3dRenderer->ScreenH-200,r3dColor(255,255,255), "AccumDmgRcvd %.2f", accumLastDamageRcvd);
		Font_Label->PrintF(10, r3dRenderer->ScreenH-180,r3dColor(255,255,255), "Recoil %.2f, %.2f", RecoilViewModTarget.x, RecoilViewModTarget.y);
		Font_Label->PrintF(10, r3dRenderer->ScreenH-160,r3dColor(255,255,255), "Pos %.2f, %.3f, %.2f", GetPosition().x, GetPosition().y, GetPosition().z);
		Font_Label->PrintF(10, r3dRenderer->ScreenH-140,r3dColor(255,255,252), "State %s", PlayerStateNames[PlayerState]);

		if( r_show_player_debug_data->GetInt() == 2 )
		{
			// Useful for tweaking ThreatStats
			int threatNdx = ( threatStats[0].threat > threatStats[1].threat ) ? 0 : 1;
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-740,r3dColor(255,255,255), "V.Sum %.2f", threatStats[ threatNdx ].visualSum);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-728,r3dColor(255,255,255), "A.Sum %.2f", threatStats[ threatNdx ].audibleSum);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-716,r3dColor(255,255,255), "I.Sum %.2f", threatStats[ threatNdx ].interceptSum);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-704,r3dColor(255,255,255), "N.Sum %.2f", threatStats[ threatNdx ].nearbySum);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-692,r3dColor(255,255,255), "Closest %.2f", threatStats[ threatNdx ].closestZombie);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-680,r3dColor(255,255,255), "NearCnt %d", threatStats[ threatNdx ].zombieNearbyCount);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-668,r3dColor(255,255,255), "I.Count %d", threatStats[ threatNdx ].zombieInterceptCount);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-656,r3dColor(255,255,255), "V.Count %d", threatStats[ threatNdx ].inSightRangeCount);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-644,r3dColor(255,255,255), "A.Count %d", threatStats[ threatNdx ].inAudioRangeCount);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-632,r3dColor(255,255,255), "F.Close %.2f", threatStats[ threatNdx ].factorClosest);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-620,r3dColor(255,255,255), "F.Nearby %.2f", threatStats[ threatNdx ].factorNearby);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-608,r3dColor(255,255,255), "F.Incpt %.2f", threatStats[ threatNdx ].factorIntercept);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-596,r3dColor(255,255,255), "F.Vsble %.2f", threatStats[ threatNdx ].factorVisible);
			Font_Editor->PrintF(10, r3dRenderer->ScreenH-584,r3dColor(255,255,255), "F.Adble %.2f", threatStats[ threatNdx ].factorAudible);
			Font_Editor->PrintF(150, r3dRenderer->ScreenH-632,r3dColor(255,255,255), "W.Close %.2f", threatStats[ threatNdx ].weightedClosest);
			Font_Editor->PrintF(150, r3dRenderer->ScreenH-620,r3dColor(255,255,255), "W.Nearby %.2f", threatStats[ threatNdx ].weightedNearby);
			Font_Editor->PrintF(150, r3dRenderer->ScreenH-608,r3dColor(255,255,255), "W.Incpt %.2f", threatStats[ threatNdx ].weightedIntercept);
			Font_Editor->PrintF(150, r3dRenderer->ScreenH-596,r3dColor(255,255,255), "W.Vsble %.2f", threatStats[ threatNdx ].weightedVisible);
			Font_Editor->PrintF(150, r3dRenderer->ScreenH-584,r3dColor(255,255,255), "W.Adble %.2f", threatStats[ threatNdx ].weightedAudible);
			Font_Editor->PrintF(150, r3dRenderer->ScreenH-572,r3dColor(255,255,255), "Threat %.2f", threatStats[ threatNdx ].threat * 100.0f);
#ifndef FINAL_BUILD
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-680,r3dColor(255,255,255), "VisSq %.2f", threatStats[ threatNdx ].plrVisibilitySq );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-668,r3dColor(255,255,255), "AudSq %.2f", threatStats[ threatNdx ].plrAudibilitySq );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-656,r3dColor(255,255,255), "MaxPercepSq %.2f", threatStats[ threatNdx ].plrMaxPerceptionSq );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-644,r3dColor(255,255,255), "MaxPercep %.2f", threatStats[ threatNdx ].plrMaxPerception );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-632,r3dColor(255,255,255), "PlayerPos <%.2f, %.2f, %.2f>", threatStats[ threatNdx ].plrPosition.x, threatStats[ threatNdx ].plrPosition.y, threatStats[ threatNdx ].plrPosition.z );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-620,r3dColor(255,255,255), "PlayerVel <%.2f, %.2f, %.2f>(%.4f)", threatStats[ threatNdx ].plrVelocity.x, threatStats[ threatNdx ].plrVelocity.y, threatStats[ threatNdx ].plrVelocity.z, threatStats[ threatNdx ].plrVelocity.Length() );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-608,r3dColor(255,255,255), "FuturePos <%.2f, %.2f, %.2f>", threatStats[ threatNdx ].plrFuturePos.x, threatStats[ threatNdx ].plrFuturePos.y, threatStats[ threatNdx ].plrFuturePos.z );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-596,r3dColor(255,255,255), "ZombiePos <%.2f, %.2f, %.2f>", threatStats[ threatNdx ].zmbPosition.x, threatStats[ threatNdx ].zmbPosition.y, threatStats[ threatNdx ].zmbPosition.z );
			Font_Editor->PrintF(290, r3dRenderer->ScreenH-584,r3dColor(255,255,255), "ZombieVec <%.2f, %.2f, %.2f>(%.4f)", threatStats[ threatNdx ].zmbVector.x, threatStats[ threatNdx ].zmbVector.y, threatStats[ threatNdx ].zmbVector.z, threatStats[ threatNdx ].zmbVector.Length() );

			r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH);
			r3dDrawSphereSolid(threatStats[ threatNdx ].plrFuturePos, 1.0f, gCam, r3dColor24( 200, 128, 128, 200 ) );
			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}
#endif
		if(Keyboard->WasPressed(kbsO))
			accumLastDamageRcvd = 0;
	}
#endif

#ifndef FINAL_BUILD
	// editing rig animation speed
	if(0)
	{
		int astate = uberAnim_->AnimPlayerState;
		Font_Label->PrintF(10, r3dRenderer->ScreenH-300, r3dColor(255,255,255), "AnimState:%d, Speed:%f", astate, uberAnim_->AnimSpeedStates[astate]);
		
		if(Keyboard->IsPressed(kbsT)) uberAnim_->AnimSpeedStates[astate] -= r3dGetFrameTime() * 0.2f;
		if(Keyboard->IsPressed(kbsY)) uberAnim_->AnimSpeedStates[astate] += r3dGetFrameTime() * 0.2f;
	}
#endif

#ifndef FINAL_BUILD
	char plrUserName[64]; GetUserName(plrUserName);
	if(strncmp(plrUserName, "@@TestDummy", 10) == 0)
	{
		r3dPoint3D scrCoord;
		if(r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1, 0), &scrCoord))
		{
			float dist = gClientLogic().localPlayer_ ? (gClientLogic().localPlayer_->GetPosition() - GetPosition()).Length() : 0;
			Font_Label->PrintF(scrCoord.x+20, scrCoord.y,   r3dColor(255,255,255), "LastDmg %.2f", lastDamageRcvd);
			Font_Label->PrintF(scrCoord.x+20, scrCoord.y+25,r3dColor(255,255,255), "Distance %.2f", dist);
		}
	}
#endif

#ifndef FINAL_BUILD
	if( ( !g_bEditMode || CreatedFromEditor ) && r_show_player_debug_data->GetBool() )
	{
		float x = 10;
		float y = 100;
		for(size_t i=0; i<uberAnim_->anim.AnimTracks.size(); i++) 
		{
			const r3dAnimation::r3dAnimInfo& ai = uberAnim_->anim.AnimTracks[i];

			char st[256] = "";
			if(ai.dwStatus & ANIMSTATUS_Playing) strcat(st, "Play ");
			if(ai.dwStatus & ANIMSTATUS_Paused) strcat(st, "Pause ");
			if(ai.dwStatus & ANIMSTATUS_Finished) strcat(st, "Finish ");
			if(ai.dwStatus & ANIMSTATUS_Fading) strcat(st, "Fade ");
			if(ai.dwStatus & ANIMSTATUS_Expiring) strcat(st, "Expire ");

			_r3dSystemFont->PrintF(x, y, r3dColor(255, 255, 255), "%s: %02.1f, %X %s %.0f/%d", 
				ai.pAnim->pAnimName, ai.fInfluence, ai.dwStatus, st, ai.fCurFrame, ai.pAnim->NumFrames);

			y += 20;
		}
	}
#endif
#ifndef FINAL_BUILD
	if( ( !g_bEditMode || CreatedFromEditor ) && r_show_player_debug_data->GetInt() >= 3 )
	{
		float x = 400;
		float y = 100;

		void* sounds[] = {

			 footStepsSnd
			,m_sndBreathSprint
			,m_sndInPain
			,m_sndClothMovementSprint
			,m_sndSniperBreath
			,m_sndSniperHeartSlow
			,m_sndSniperExhale
			,m_sndSwimIdle
			,m_sndSwimSlow
			,m_sndSwim
			,m_sndSwimFast
		};
		int soundCount = 11;

		for( int i = 0; i < soundCount; ++i )
		{
			if( sounds[ i ] && SoundSys.isPlaying( sounds[ i ] ) )
			{
				void*		snd = sounds[ i ];
				char*		name;
				float		volume;
				FMOD_MODE	mode;
				FMOD_MODE	relative3D;
				SoundSys.GetProperty( snd, FMOD_EVENTPROPERTY_NAME, &name );
				SoundSys.GetProperty( snd, FMOD_EVENTPROPERTY_VOLUME, &volume );
				SoundSys.GetProperty( snd, FMOD_EVENTPROPERTY_MODE, &mode );
				SoundSys.GetProperty( snd, FMOD_EVENTPROPERTY_3D_POSITION, &relative3D );

				_r3dSystemFont->PrintF(x, y, r3dColor(255, 255, 255), "%s(%s:%s): %0.2f",
					name,
					(mode == FMOD_3D ? "3D" : "2D"),
					(mode == FMOD_3D ? (relative3D == FMOD_3D_WORLDRELATIVE ? "World Relative" : "Head Relative") : "N/A" ),
					volume);

				y += 20;
			}
		}
	}
#endif
#ifndef FINAL_BUILD
	if(r_show_player_health->GetBool())
	{
		r3dPoint3D scrCoord;
		if(r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1, 0), &scrCoord))
		{
			Font_Label->PrintF(scrCoord.x, scrCoord.y,   r3dColor(255,255,255), "%.1f", CurLoadout.Health);
		}
	}
	if(r_show_zombie_stats->GetBool())
	{
		if(NetworkLocal)
			lifeProperties.DebugVisualizeRanges();
	}
#endif

#ifndef FINAL_BUILD
	if(r_show_player_aiming->GetBool() && !NetworkLocal)
	{
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH);
		int psId = r3dRenderer->GetCurrentPixelShaderIdx();
		int vsId = r3dRenderer->GetCurrentVertexShaderIdx();
		r3dRenderer->SetPixelShader();
		r3dRenderer->SetVertexShader();
		r3dRenderer->SetTex(0, 0);


		r3dMaterial* temp;
		GameObject* tempObj=NULL;
		r3dPoint3D muzzlerPos = GetMuzzlerPosition();

		r3dPoint3D targetPos, targetNorm;
		r3dPoint3D forwV = GetvForw();

		float yRot = bodyAdjust_y[1];

		D3DXMATRIX r1;
		D3DXMatrixRotationYawPitchRoll(&r1, R3D_DEG2RAD(m_fPlayerRotationTarget), -yRot, 0);
		D3DXVECTOR3 res, orig;
		orig = D3DXVECTOR3(0, 0, 1);
		D3DXVec3TransformNormal(&res, &orig, &r1);
		forwV = r3dPoint3D(res.x, res.y, res.z);

		updateShootingTarget(this, muzzlerPos, muzzlerPos, targetPos, targetNorm, &temp, &tempObj, NULL, 0.0f, false, &forwV);

		r3dColor lineColor = r3dColor::blue;
		if(tempObj)
		{
			if(tempObj->isObjType(OBJTYPE_Human))
				lineColor = r3dColor::red;
			else if(tempObj->isObjType(OBJTYPE_Zombie))
				lineColor = r3dColor::green;
		}

		r3dDrawLine3DColors(muzzlerPos, targetPos, gCam, 0.1f, lineColor, lineColor);

		r3dRenderer->Flush();
		r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		r3dRenderer->SetPixelShader(psId);
		r3dRenderer->SetVertexShader(vsId);
	}
#endif
}

//------------------------------------------------------------------------
struct PlayerRenderable : Renderable
{
	void Init( obj_Player::AuraType at, int Transparent )
	{
		DrawFunc = Draw;
		PlayerAuraType = at ;

		this->Transparent = Transparent ;
		matFlagsModifier = 0;
		FirstPerson = 0 ;
	}

	void ApplyCamouflageSettings()
	{
		if (!Parent)
			return;

		gCamouflageDataManager.SetCurrentData(*Parent);
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		PlayerRenderable* This = static_cast<PlayerRenderable*>( RThis );

		bool draw_weapon = true;

		struct RenderEvents
		{
			RenderEvents()
			{
				D3DPERF_BeginEvent( 0, L"PlayerRenderable::Draw" ) ;
			}

			~RenderEvents()
			{
				D3DPERF_EndEvent() ;
			}

		} renderEvents; renderEvents ;

		CUberEquip::DrawType dt;

		extern int PS_TRANSPARENT_ID		;
		extern int PS_TRANSPARENT_AURA_ID	;

		struct RestoreCamOnExit
		{
			RestoreCamOnExit()
			{
				needRestore = false ;
			}

			~RestoreCamOnExit()
			{
				if( needRestore )
				{
					r3dRenderer->SetCamera( gCam, true ) ;
					if( r_environment_quality->GetInt() > 2 )
					{
						D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ZENABLE, TRUE ) ) ;
					}
				}
			}

			bool needRestore ;
		} restoreCamOnExit ;

		if( ( This->Transparent || This->PlayerAuraType != obj_Player::AT_NONE )
				&&
			This->FirstPerson			
			)
		{
			r3dCamera firstPersonCam = gCam ;
			firstPersonCam.NearClip = r_first_person_render_z_start->GetFloat();
			firstPersonCam.FarClip = r_first_person_render_z_end->GetFloat();
			firstPersonCam.FOV = r_first_person_fov->GetFloat();

			restoreCamOnExit.needRestore = true ;

			r3dRenderer->SetCamera( firstPersonCam, true ) ;
		}

		const Weapon* wpn = This->Parent->m_Weapons[This->Parent->m_SelectedWeapon];

		if( This->PlayerAuraType != obj_Player::AT_NONE )
		{
			draw_weapon = false ;
			dt = CUberEquip::DT_AURA ;
			r3dMaterial::SetupTransparentStates( 1 );

			void SetParticleDepth();
			SetParticleDepth();

			float r(1.0f), g(1.0), b(1.0f);

			if( This->PlayerAuraType == obj_Player::AT_HEALING )
			{
				r = 0.f ;
				g = 1.f ;
				b = 0.25f ;
			}
			else if(This->PlayerAuraType == obj_Player::AT_MORHINE)
			{
				r = 0.f ;
				g = 0.f ;
				b = 1.0f ;
			}
			else if(This->PlayerAuraType == obj_Player::AT_BERSERK)
			{
				r = 1.f ;
				g = 0.f ;
				b = 0.f ;
			}
			else if(This->PlayerAuraType == obj_Player::AT_SPAWNPROTECTION) // golden color
			{
				r = 1.0f ;
				g = 0.62f ;
				b = 0.15f ;
			}

			r3dRenderer->SetTex( gPlayer_AuraTexture ) ;

			{
				float at = This->Parent->m_AuraTransarency;
				float vConst[ 2 ][ 4 ] = {	{ r, g, b, r3dGetTime() },
											{ at * at, 0.f, 0.f, 0.f } };
				D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_AURA, vConst[ 0 ], 2 ) );
			}

			void SetupFilmToneConstants( int reg ) ;
			SetupFilmToneConstants( 24 ) ;

			r3dRenderer->SetPixelShader( PS_TRANSPARENT_AURA_ID ) ;
			r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);
			r3dRenderer->pd3ddev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
			r3dRenderer->pd3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
		{
			if(wpn && wpn->getCategory() == storecat_GRENADE)
			{
				if(This->Parent->uberAnim_->IsGrenadeLaunched())
					draw_weapon = false; // throwing grenade, so do not render grenade in hand
			}
			if(wpn && wpn->getItemID() == WeaponConfig::ITEMID_UnarmedMelee)
				draw_weapon = false; // don't draw melee_hands weapon

			if( This->Transparent )
			{
				const CamoTimeLine &ctl = This->Parent->camoTimeLine;
				float s = ctl.transparencyOnShock;

				float vConst[ 4 ][ 4 ] =
				{
					// float4 Aura				: register(c14); // (c65);
					{ 1.0f,	s * (1 - s) * 4, ctl.transparentCamoAlpha, 0},
					// float4 TCamoExtra		: register(c15);
					{ 0, 0, 0, 0.f },
					// float4 TCamoColor0			: register(c16);
					{ 0, 0, 0, 0.f },
					// float4 TCamoColor1			: register(c17);
					{ 0, 0, 0, 0 }
				};
				D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_AURA, (float*)vConst, 4 ) );
				
				vConst[0][3] = 0.01f;
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 23, (float*)vConst, 1 ) ) ;

				for(int i=0; i<=SLOT_Helmet; i++)
				{
					if(This->Parent->uberEquip_->slots_[i].gear)
					{
						bool isFirstPerson = g_camera_mode->GetInt()==2 && This->Parent->NetworkLocal;
						if( r3dMesh* mesh = This->Parent->uberEquip_->slots_[i].gear->getModel(isFirstPerson) )
						{
							mesh->ExtrudeAmmount = 0.01f ;
						}
					}
				}

				r3dRenderer->SetTex( gPlayer_CamoTexture, 9 ) ;

				r3dRenderer->pd3ddev->SetSamplerState(9, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
				r3dRenderer->pd3ddev->SetSamplerState(9, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

			}

			if(This->Parent->bDead)
				draw_weapon = false;

			This->ApplyCamouflageSettings();
			dt = CUberEquip::DT_DEFERRED;
		}

		float vCData[ 4 ] = { 1, 1, 1, 0 } ;
#if 0
 		if(gClientLogic().localPlayer_ && This->Parent->TeamID != gClientLogic().localPlayer_->TeamID)
 		{
 			vCData[ 0 ] = 1 ; vCData[ 1 ] = 0 ; vCData[ 2 ] = 0 ; 
 		}
 		else
 		{
 			vCData[ 0 ] = 0 ; vCData[ 1 ] = 0 ; vCData[ 2 ] = 1 ; 
 		}
#endif
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_MASKCOLOR, vCData, 1 ) );

		if (This->matFlagsModifier)
		{
			for (int i = 0; i < SLOT_Max; ++i)
			{
				r3dMesh *m = This->Parent->uberEquip_->getSlotMesh((ESlot)i);
				if(!m) continue;
				if(!m->IsDrawable()) continue;

				for (int j = 0, e = m->GetNumMaterials(); j < e; ++j)
				{
					r3dTriBatch &tb = m->MatChunks[j];
					r3dMaterial *mat = tb.Mat;
					if (!mat) continue;

					This->matFlagsModifier(mat);
				}
			}
		}

		if( This->Transparent )
		{
			void SetVolumeFogParams() ;
			SetVolumeFogParams() ;

			// if it's local, draw depth prepass to disable backfaces for camo so that everything is not uggly.
			// TODO : if we spectate we should do this on spectate target!
			if( This->Parent->NetworkLocal && r3dRenderer->SupportsR32FBlending && !This->FirstPerson )
			{
				void ActivateTransparentBuffer();
				void ActivateParticleDepth();
				void DeactivateTransparentBuffer();
				void DeactivateParticleDepth();
				void SetParticleDepth();
				void RemoveParticleDepth();

				RemoveParticleDepth();
				r3dTransparentDistortFlush() ;
				DeactivateTransparentBuffer();
				ActivateParticleDepth();

				r3dRenderer->SetPixelShader( PS_DEPTH_ID ) ;

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE ) );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE ) );
				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ) );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_MIN ) );

				This->Parent->uberEquip_->Draw( This->Parent->GetSkeleton(), This->Parent->DrawFullMatrix_Localized, draw_weapon, CUberEquip::DT_DEPTH, This->FirstPerson ? true : false );

				DeactivateParticleDepth();
				ActivateTransparentBuffer();
				r3dTransparentSetDistort( 1 ) ;
				SetParticleDepth();

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ) );
			}

			{
				D3DXMATRIX sm0, sm1 ;

				D3DXMatrixTranspose( &sm0, &This->Parent->DrawFullMatrix_Localized ) ;

				sm1 = This->Parent->DrawFullMatrix_Localized * r3dRenderer->ViewMatrix_Localized ;

				D3DXMatrixTranspose( &sm1, &sm1 ) ;

				float consts[ 6 ][ 4 ] =
				{
					// float4x3 mWorld : register( c32 ) ;
					{ sm0._11, sm0._12, sm0._13, sm0._14 },
					{ sm0._21, sm0._22, sm0._23, sm0._24 },
					{ sm0._31, sm0._32, sm0._33, sm0._34 },
					// float4x3 mWorldView : register( c35 ) ;
					{ sm1._11, sm1._12, sm1._13, sm1._14 },
					{ sm1._21, sm1._22, sm1._23, sm1._24 },
					{ sm1._31, sm1._32, sm1._33, sm1._34 },
				} ;

				D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 32, (float*)consts, 6 ) ) ;
			}

			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );
			D3D_V( r3dRenderer->pd3ddev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE ) );
			D3D_V( r3dRenderer->pd3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );

			r3dTransparentSetDistort( 1 ) ;

			if( This->FirstPerson && r_environment_quality->GetInt() > 2 )
			{
				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ZENABLE, FALSE ) ) ;
			}

		}

		if(wpn && wpn->getItemID() == WeaponConfig::ITEMID_UnarmedMelee)
			draw_weapon = false; // don't draw melee_hands weapon

		if( This->Parent->Darkness )
			dt = CUberEquip::DT_DARKNESS;

		This->Parent->uberEquip_->Draw( This->Parent->GetSkeleton(), This->Parent->DrawFullMatrix_Localized, draw_weapon, dt, This->FirstPerson ? true : false );

		if( This->PlayerAuraType != obj_Player::AT_NONE )
		{
			r3dRenderer->SetPixelShader( PS_TRANSPARENT_ID ) ;
		}

		if(This->Parent->s_UAVFriendlyBlinkTex && gClientLogic().localPlayer_ && gClientLogic().localPlayer_->uavViewActive_ && gClientLogic().localPlayer_->GroupID == This->Parent->GroupID)
		{
			r3dRenderer->SetTex(NULL);
			r3dRenderer->SetMaterial(NULL);
			r3dRenderer->SetRenderingMode( R3D_BLEND_PUSH | R3D_BLEND_ADD | R3D_BLEND_ZC );

			void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
			int alpha = int(floorf(R3D_ABS(r3dSin(r3dGetTime()*6.0f))*255.0f));
			r3dDrawIcon3D(This->Parent->GetPosition() + r3dPoint3D(0,2,0), This->Parent->s_UAVFriendlyBlinkTex, r3dColor24(alpha,alpha,alpha, alpha), 18);

			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}
	}

	obj_Player* Parent ;
	typedef void (*MaterialModifyFlags)(r3dMaterial *mat);
	MaterialModifyFlags matFlagsModifier;

	char PlayerAuraType ;
	char Transparent ;
	char FirstPerson ;
};

//------------------------------------------------------------------------
struct PlayerShadowRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		PlayerShadowRenderable* This = static_cast<PlayerShadowRenderable*>(RThis);

		bool draw_weapon = true;
		const Weapon* wpn = This->Parent->m_Weapons[This->Parent->m_SelectedWeapon];
		if(wpn && wpn->getCategory() == storecat_GRENADE)
		{
			if(This->Parent->uberAnim_->IsGrenadeLaunched())
				draw_weapon = false; // throwing grenade, so do not render grenade in hand
		}
		if(wpn && wpn->getItemID() == WeaponConfig::ITEMID_UnarmedMelee)
			draw_weapon = false;
		This->Parent->uberEquip_->Draw(This->Parent->GetSkeleton(), This->Parent->DrawFullMatrix_Localized, draw_weapon, CUberEquip::DT_SHADOWS, false );
	}

	obj_Player* Parent;
};

struct PlayerFlashRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		PlayerFlashRenderable* This = static_cast<PlayerFlashRenderable*>( RThis );

		This->Parent->DrawLabel();
		This->Parent->DrawReticle();
	}

	obj_Player* Parent;
};

struct PlayerDebugRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		PlayerDebugRenderable* This = static_cast<PlayerDebugRenderable*>( RThis );
		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

		This->Parent->GetSkeleton()->DrawSkeleton(Cam, This->Parent->GetPosition());

		/*if(This->Parent->explosionTime > 0)
		{
			This->Parent->explosionTime-=r3dGetFrameTime();
			r3dDrawSphereSolid(This->Parent->explosionCenter, This->Parent->explosionRadius, Cam, r3dColor::red);
		}*/
		//r3dDrawBoundBox(This->Parent->GetBBoxWorld(), Cam, r3dColor24::green/*debugColorLevels[m_Level]*/, 0.05f);
	}

	obj_Player* Parent;
};

#define	RENDERABLE_AI_PLAYER_SORT_VALUE (2*RENDERABLE_USER_SORT_VALUE)

void
obj_Player::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam )
{

	if (m_DevPlayerHide==true && !NetworkLocal)
		return;

#ifdef VEHICLES_ENABLED
	if(!m_enableRendering)
		return;
#endif

	int firstPerson = g_camera_mode->GetInt() == 2 && NetworkLocal && !bDead && !uavViewActive_;

	if(firstPerson) // do not render our own shadow in FPS mode (same as in BF3)
		return;

	if( !gDisableDynamicObjectShadows && sliceIndex != r_active_shadow_slices->GetInt() - 1 )
	{
		PlayerShadowRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_AI_PLAYER_SORT_VALUE;

		rarr.PushBack( rend );
	}
}

//////////////////////////////////////////////////////////////////////////

void SetTransparentCamoMatFlags(r3dMaterial *mat )
{
	mat->Flags |= R3D_MAT_TRANSPARENT | R3D_MAT_TRANSPARENT_CAMOUFLAGE ;
	if (strstr(mat->Name, "nocamo"))
		mat->Flags |= R3D_MAT_SKIP_DRAW;
}

void SetTransparentCamoMatFlags_FP(r3dMaterial *mat )
{
	mat->Flags |= R3D_MAT_TRANSPARENT | R3D_MAT_TRANSPARENT_CAMO_FP ;
	if (strstr(mat->Name, "nocamo"))
		mat->Flags |= R3D_MAT_SKIP_DRAW;
}

//////////////////////////////////////////////////////////////////////////

void UnsetTransparentCamoMatFlags(r3dMaterial *mat)
{
	mat->Flags &= ~(R3D_MAT_TRANSPARENT | R3D_MAT_TRANSPARENT_CAMOUFLAGE | R3D_MAT_TRANSPARENT_CAMO_FP |  R3D_MAT_SKIP_DRAW);
}

struct PlaceableConsumableItemRenderable : MeshDeferredRenderable
{
	void Init( int a_transparent )
	{
		DrawFunc = Draw;
		Transparent = a_transparent ;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		PlaceableConsumableItemRenderable* This = static_cast< PlaceableConsumableItemRenderable* >( RThis );
		r3dApplyPreparedMeshVSConsts( This->Parent->m_ConsumableItemPlacementMeshVSConsts );

		int wasTransparent = 0 ;

		float oldTranspMultiplier = 1.0f ;

		bool hasMatForTrasnparency = This && This->Transparent && This->Mesh && This->Mesh->MatChunks[This->BatchIdx].Mat ;

		if ( hasMatForTrasnparency )
		{
			// because previous material may be the same
			// and thus it will filter our transparency hacks that follow
			r3dMaterial::ResetMaterialFilter() ;

			r3dMaterial* mat = This->Mesh->MatChunks[This->BatchIdx].Mat ;

			wasTransparent = mat->Flags & R3D_MAT_TRANSPARENT ;
			oldTranspMultiplier = mat->TransparencyMultiplier ;

			if( !wasTransparent )
				mat->TransparencyMultiplier = 0.75f ;

			mat->Flags |= R3D_MAT_TRANSPARENT ;

			SetLightsIfTransparent( mat, This->Parent->GetBBoxWorld() ) ;
		}

		if(This->Mesh->IsSkeletal())
			r3dSkeleton::SetDefaultShaderConstants();

		MeshDeferredRenderable::Draw( RThis, Cam );

		if ( hasMatForTrasnparency )
		{
			r3dMaterial* mat = This->Mesh->MatChunks[This->BatchIdx].Mat ;

			int& flags = mat->Flags ;
			flags &= ~R3D_MAT_TRANSPARENT ;
			flags |= wasTransparent ;

			mat->TransparencyMultiplier = oldTranspMultiplier ;
		}
	}

	obj_Player* Parent ;
	int Transparent ;
};

//------------------------------------------------------------------------
// special callback to ignore terrain for beacon placement, as otherwise it is almost impossible to place beacon on uneven terrain
class beaconFilterCallback : public PxSceneQueryFilterCallback
{
public:
	beaconFilterCallback(){};

	virtual PxSceneQueryHitType::Enum preFilter(const PxFilterData& filterData, PxShape* shape, PxSceneQueryFilterFlags& filterFlags)
	{
		if(shape)
		{
			PxRigidActor& actor = shape->getActor();
			PhysicsCallbackObject* target = static_cast<PhysicsCallbackObject*>(actor.userData);
			if(target)
			{
				GameObject *gameObj = target->isGameObject();
				if(gameObj)
				{
					if(gameObj->isObjType(OBJTYPE_Terrain))
						return PxSceneQueryHitType::eNONE;
				}
			}
		}
		return PxSceneQueryHitType::eBLOCK;
	}

	virtual PxSceneQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxSceneQueryHit& hit)
	{
		// shouldn't be called!
		return PxSceneQueryHitType::eBLOCK;
	}
};


void
obj_Player::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{

	if (m_DevPlayerHide==true && !NetworkLocal)
		return;

#ifdef VEHICLES_ENABLED
	if(!m_enableRendering)
		return;
#endif

	int firstPerson = g_camera_mode->GetInt() == 2 && NetworkLocal && !bDead && !uavViewActive_;

	if(firstPerson)
	{
		if(hasScopeMode() && CurrentRig.allowScope)
		{
			r_first_person_render_z_start->SetFloat(0.01f);
			r_first_person_render_z_end->SetFloat(0.7f);
		}
		else
		{
			r_first_person_render_z_start->SetFloat(0.1f);
			r_first_person_render_z_end->SetFloat(1.5f);
		}
	}

	// deferred
	if( !NetworkLocal || (NetworkLocal && (!g_CameraInsidePlayer || g_camera_mode->GetInt()==2)) ) // do not render player if camera is inside of player
	{
		if(!(NetworkLocal && hasScopeMode() && hasScopeMode_hidePlayerModel() && CurrentRig.allowScope)) // do not render yourself in scope mode
		{
			bool transparentCamo = camoTimeLine.camoMasterTransparency < 1.0f;
			if (camoTimeLine.camoMasterTransparency > 0)
			{
				PlayerRenderable rend;

				rend.Init( AT_NONE, 0 );
				rend.Parent		= this;
				rend.SortValue	= RENDERABLE_AI_PLAYER_SORT_VALUE;
				rend.matFlagsModifier = &UnsetTransparentCamoMatFlags;
				rend.FirstPerson = firstPerson ;

				render_arrays[ firstPerson ? rsFillGBufferFirstPerson : rsFillGBuffer ].PushBack( rend );
			}
			
			if ( transparentCamo )
			{
				PlayerRenderable rend ;

				rend.Init( AT_NONE, 1 ) ;
				rend.Parent		= this ;
				rend.FirstPerson = firstPerson ;

				rend.matFlagsModifier = firstPerson ? 
											&SetTransparentCamoMatFlags_FP
												:
											&SetTransparentCamoMatFlags ;

				float len = ( Cam - GetPosition() ).Length() ;
				rend.SortValue	= RENDERABLE_EMITTER_USER_SORT_VALUE | GetRevIDist( len );

				render_arrays[ rsDrawTransparents ].PushBack( rend ) ;			
			}

			if ( m_AuraType != AT_NONE )
			{
				PlayerRenderable rend ;

				rend.Init( m_AuraType, 0 ) ;
				//rend.Init( AT_HEALING , 0 ) ;
				rend.Parent		= this ;
				rend.FirstPerson = firstPerson ;

				rend.matFlagsModifier = &UnsetTransparentCamoMatFlags;

				float len = ( Cam - GetPosition() ).Length() ;

				// render after tcamo hecne +1
				rend.SortValue	= RENDERABLE_EMITTER_USER_SORT_VALUE | ( GetRevIDist( len ) + 1 );

				render_arrays[ rsDrawTransparents ].PushBack( rend ) ;			
			}

			if(m_needToDrawBarricadePlacement)
			{
				bool showMessageWarning = false;
				if(m_BarricadeMeshPlacement == NULL)
				{
					m_BarricadeMeshAllowToPlaceOnTop = false;
					UpDownEditor=r3dPoint3D(0,0,0);
					LeftRight=0.0f;
					UpDown=4.0f;
					if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_BarbWireBarricade)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Item_Barricade_BarbWire_Built.sco", true, false, true, true );
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_WoodShieldBarricade)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Item_Barricade_WoodShield_Built.sco", true, false, true, true );
						m_ModeEditor=false;
					}					
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_WoodShieldBarricadeZB)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Item_Barricade_WoodShield_Built_ZB.sco", true, false, true, true );
						m_ModeEditor=false;
					}						
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_RiotShieldBarricade)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Item_Riot_Shield_01.sco", true, false, true, true );
						m_ModeEditor=false;
					}						
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_RiotShieldBarricadeZB)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Item_Riot_Shield_ZB.sco", true, false, true, true );
						m_ModeEditor=false;
					}						
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_SandbagBarricade)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\item_barricade_Sandbag_built.sco", true, false, true, true );
						m_ModeEditor=false;
					}						
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Lockbox)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Item_Lockbox_01.sco", true, false, true, true );
						m_ModeEditor=false;
					}						
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_WoodenDoorBlock)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_Door_Wood_2M_01.sco", true, false, true, true );
						m_ModeEditor=false;
					}						
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_MetalWallBlock)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_Wall_Metal_2M_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_TallBrickWallBlock)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_Wall_Brick_Tall_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_WoodenWallPiece)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_Wall_Wood_2M_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ShortBrickWallPiece)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_Wall_Brick_Short_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Traps_Bear)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\INB_Traps_Bear_01_disarmed_tps.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Traps_Spikes)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\INB_Traps_Spikes_01_disarmed_tps.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_GardenTrap_Rabbit)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\INB_GardenTrap_Rabbit_01_Open.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Campfire)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\INB_prop_campfire_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorBlockSmall)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorBlockBig)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_02.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorBlockCircle)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_03.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorColum1)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorColum2)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_02.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorColum3)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_03.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorFloor1)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_flr_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorFloor2)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_flr_02.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorCeiling1)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorCeiling2)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_02.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorCeiling3)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_03.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorWallMetalic)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_metalicpole_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorSlope)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_str_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorWall1)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorWall2)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_02.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorWall3)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_03.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorWall4)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_01_01x04m.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorWall5)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_01_02x04m.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_ConstructorBaseBunker)
					{
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\base_lm_infantrybunker_01.sco", true, false, true, true );
						m_BarricadeMeshAllowToPlaceOnTop = true;
						m_ModeEditor=true;
						showMessageWarning=true;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_FarmBlock)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_Farm_01.sco", true, false, true, true );
						m_ModeEditor=false;
					}					
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_SolarWaterPurifier)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_SolarWater_01.sco", true, false, true, true );
						m_ModeEditor=false;
					}					
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_PlaceableLight)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_Light_01.sco", true, false, true, true );
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_SmallPowerGenerator)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_PowerGen_01_Small.sco", true, false, true, true );
						m_ModeEditor=false;
					}
					else if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_BigPowerGenerator)
					{					
						m_BarricadeMeshPlacement = r3dGOBAddMesh("Data\\ObjectsDepot\\Weapons\\Block_PowerGen_01_Industrial.sco", true, false, true, true );
						m_ModeEditor=false;
					}
				}
				if (m_ModeEditor == true && m_ModeCreator== false)
				{
					if(hudMain && showMessageWarning == true) 
					{
						hudMain->showMessage(gLangMngr.getString("NeedEnterOnEditorMode"));
						showMessageWarning=false;
					}
					return;
				}

				if(m_BarricadeMeshPlacement && m_BarricadeMeshPlacement->IsDrawable())
				{
					r3dPoint3D *scale ;
					r3dPoint2D *texcScale ;
					scale = m_BarricadeMeshPlacement->IsSkeletal() ? 0 : &m_BarricadeMeshPlacement->unpackScale;
					texcScale = &m_BarricadeMeshPlacement->texcUnpackScale ;
					
					if (m_ModeEditor == true)
					{
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_UPBLOCK))
							UpDownEditor.y+=0.1f;
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_DOWNBLOCK))
							UpDownEditor.y-=0.1f;
						
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_RLEFTBLOCK))
							LeftRight-=2.5f;
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_RRIGHTBLOCK))
							LeftRight+=2.5f;
						
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MLEFTBLOCK))
							UpDownEditor.x-=0.1f;
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MRIGHTBLOCK))
							UpDownEditor.x+=0.1f;

						if (UpDownEditor.x<-10.0f)
							UpDownEditor.x=-10.0f;
						else if (UpDownEditor.x>10)
							UpDownEditor.x=10;

						if (UpDownEditor.y<-1.0)
							UpDownEditor.y=-1.0;
						else if (UpDownEditor.y>20)
							UpDownEditor.y=20;
						
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_FRONTBLOCK))
							UpDown+=0.1f;
						if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_BACKBLOCK))
							UpDown-=0.1f;

						if (UpDown<2.0f)
							UpDown=2.0f;
						else if (UpDown>20)
							UpDown=20;
					}
					
					float defaultDistance = 1.0f;

					if (m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Traps_Bear ||
						m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Traps_Spikes ||
						m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Campfire)
						defaultDistance = 3.0f;

					D3DXMATRIX m1, m2, m3, world;
					if (m_ModeEditor == true)
						m_BarricadeMeshPlacementPosition = GetPosition() + r3dPoint3D(0,UpDownEditor.y,0) + GetvForw() * R3D_MAX(2.0f, UpDown) + GetvRight() * R3D_MAX(-10.0f, UpDownEditor.x);
					else
						m_BarricadeMeshPlacementPosition = GetPosition() + GetvForw() * R3D_MAX(m_BarricadeMeshPlacement->localBBox.Size.z, defaultDistance);

					if(m_BarricadeMeshAllowToPlaceOnTop && m_needToDrawBarricadePlacement != WeaponConfig::ITEMID_ConstructorSlope && m_needToDrawBarricadePlacement != WeaponConfig::ITEMID_ConstructorFloor1 && m_needToDrawBarricadePlacement != WeaponConfig::ITEMID_ConstructorFloor1)// && m_ModeEditor == false)
					{
						PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
						PxRaycastHit hit;
						if(g_pPhysicsWorld->raycastSingle(PxVec3(m_BarricadeMeshPlacementPosition.x, m_BarricadeMeshPlacementPosition.y+4.5f, m_BarricadeMeshPlacementPosition.z), PxVec3(0,-1,0), 100.5f, PxSceneQueryFlags(PxSceneQueryFlag::eDISTANCE|PxSceneQueryFlag::eIMPACT), hit, filter))
						{
							PhysicsCallbackObject* target = NULL;
							if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
							{
								GameObject *gameObj = target->isGameObject();
								if(gameObj && gameObj->isObjType(OBJTYPE_GameplayItem) && gameObj->Class->Name == "obj_Barricade")
								{
									m_BarricadeMeshPlacementPosition.y = gameObj->GetPosition().y + gameObj->GetBBoxWorld().Size.y-0.1f;
								}
							}
						}
					}

					D3DXMatrixTranslation(&m1, m_BarricadeMeshPlacementPosition.x, m_BarricadeMeshPlacementPosition.y, m_BarricadeMeshPlacementPosition.z);
					D3DXMatrixRotationY(&m2, R3D_PI);
					D3DXMatrixMultiply(&m3, &DrawRotMatrix, &m2);

					if (m_ModeEditor == true)
					{
						D3DXMATRIX RotateMatrix,rt2;
						D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(LeftRight),0,0);
						D3DXMatrixMultiply(&m3, &m3, &RotateMatrix);
					}

					D3DXMatrixMultiply(&world, &m3, &m1);
					r3dPrepareMeshShaderConsts(m_ConsumableItemPlacementMeshVSConsts, world, scale, texcScale, r3dRenderer->ViewMatrix, r3dRenderer->ViewProjMatrix, NULL);

					// check if we can place our riot shield. I don't want to do this in update, as anyway this function is called every frame, so should be fine(

					{
						PxBoxGeometry bbox(m_BarricadeMeshPlacement->localBBox.Size.x*0.5f, m_BarricadeMeshPlacement->localBBox.Size.y*0.5f, m_BarricadeMeshPlacement->localBBox.Size.z*0.5f);
						if(m_needToDrawBarricadePlacement == WeaponConfig::ITEMID_Lockbox)
						{
							bbox.halfExtents.x *= 3.0f; // to make sure that you cannot abuse lockboxes in doorways
							bbox.halfExtents.z *= 3.0f; // to make sure that you cannot abuse lockboxes in doorways
						}
						D3DXMATRIX rot = GetRotationMatrix();
						PxMat33 meshRot(PxVec3(rot._11, rot._21, rot._31), PxVec3(rot._12, rot._22, rot._32), PxVec3(rot._13, rot._23, rot._33));
						PxTransform pose(PxVec3(m_BarricadeMeshPlacementPosition.x, m_BarricadeMeshPlacementPosition.y+(m_BarricadeMeshPlacement->localBBox.Size.y*0.5f)+0.5f, m_BarricadeMeshPlacementPosition.z), PxQuat(meshRot));

						PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
						PxShape* shape;
						m_canPlaceConsumableItem = !g_pPhysicsWorld->PhysXScene->overlapAny(bbox, pose, shape, filter) && bOnGround && !(CurLoadout.GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox);
						// check if there is anything right underneath it, so that we are not trying to drop barricade from the sky (or standing at the edge of the building)
						if(m_canPlaceConsumableItem && m_ModeEditor == false)
						{
							PxRaycastHit hit;
							PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_PLAYER_COLLIDABLE_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
							if(g_pPhysicsWorld->raycastSingle(PxVec3(m_BarricadeMeshPlacementPosition.x, m_BarricadeMeshPlacementPosition.y+0.5f, m_BarricadeMeshPlacementPosition.z), PxVec3(0,-1,0), 5.0f, PxSceneQueryFlags(PxSceneQueryFlag::eDISTANCE), hit, filter))
							{
								if(hit.distance > 1.0f)
									m_canPlaceConsumableItem = false;
							}
							else
								m_canPlaceConsumableItem = false;
						}
						if(m_canPlaceConsumableItem && !m_BarricadeMeshAllowToPlaceOnTop) // check if we are trying to place it on top of usable item
						{
							bbox.halfExtents.y *= 4.0f;
							PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
							PxShape* results[64] = {0};
							int numRes = g_pPhysicsWorld->PhysXScene->overlapMultiple(bbox, pose, results, 64, filter);
							for(int i=0; i<numRes; ++i)
							{
								PhysicsCallbackObject* clbObj = NULL;
								if( results[i] && (clbObj = static_cast<PhysicsCallbackObject*>(results[i]->getActor().userData)))
								{
									GameObject* gameObj = clbObj->isGameObject();
									if(gameObj && gameObj->isObjType(OBJTYPE_GameplayItem))
									{
										if (gameObj->Class->Name == "obj_Barricade")
										{
											obj_Barricade* barricade = ((obj_Barricade*)gameObj);
											if (barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorBlockSmall ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorBlockBig ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorBlockCircle ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorColum1 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorColum2 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorColum3 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorFloor1 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorFloor2 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling1 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling2 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling3 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorSlope ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorWall1 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorWall2 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorWall3 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorWall4 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorWall5 ||
												barricade->m_ItemID == WeaponConfig::ITEMID_ConstructorBaseBunker)
												break;
										}
										m_canPlaceConsumableItem = false;
										break;
									}
								}
							}
						}
					}

					{
						float dist = (Cam - GetPosition()).Length();

						uint32_t prevTranspCount = render_arrays[rsDrawTransparents].Count();

						m_BarricadeMeshPlacement->AppendTransparentRenderables( render_arrays[rsDrawTransparents], m_canPlaceConsumableItem?r3dColor::green:r3dColor::red, dist, 1 );

						for( uint32_t i = prevTranspCount, e = render_arrays[rsDrawTransparents].Count(); i < e; i ++ )
						{
							PlaceableConsumableItemRenderable& rend = static_cast<PlaceableConsumableItemRenderable&>( render_arrays[rsDrawTransparents][ i ] ) ;
							rend.Init( 1 ) ;
							rend.Parent = this;
						}
					}
				}
			}
		}
	}

	// debug
	{
		PlayerDebugRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_AI_PLAYER_SORT_VALUE;

		render_arrays[ rsDrawBoundBox ].PushBack( rend );
	}

	// Flash UI
	if((!d_disable_game_hud->GetBool() && !d_observer_mode->GetBool()) || d_force_scope_rendering->GetBool())
	{
		PlayerFlashRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_AI_PLAYER_SORT_VALUE;

		render_arrays[ rsDrawFlashUI ].PushBack( rend );
	}
}

void obj_Player::UpdateTransform()
{
	GameObject::UpdateTransform();
}

//////////////////////////////////////////////////////////////////////////
void obj_Player::stopFootstepsSound()
{
	if (SoundSys.IsHandleValid(footStepsSnd))
	{
		SoundSys.Stop(footStepsSnd);
	}
}

void obj_Player::UpdateSounds()
{
	R3DPROFILE_FUNCTION("UpdateSounds");

	if(NetworkLocal) // local sounds
	{
		if(!SoundSys.isPlaying(m_sndClothMovementSprint) && PlayerState == PLAYER_MOVE_SPRINT) // started sprinting
		{
			SoundSys.Start(m_sndClothMovementSprint);
		}
		else if(SoundSys.isPlaying(m_sndClothMovementSprint) && PlayerState != PLAYER_MOVE_SPRINT) // stopped sprinting
		{
			SoundSys.Stop(m_sndClothMovementSprint);
		}

		float breathVolume = 0.0f;
		float curStaminaR = m_Stamina/getMaxStamina();
		if(curStaminaR < 0.15f && curStaminaR > 0.1f)
			breathVolume = 1.0f-((curStaminaR-0.1f)*20.0f);
		else if(curStaminaR <= 0.1f)
			breathVolume = 1.0f;
		else if(curStaminaR > 0.15f)
			breathVolume = 0.0f;

		if(SoundSys.isPlaying(m_sndBreathSprint) && breathVolume <= 0.0f)
			SoundSys.Stop(m_sndBreathSprint);
		else if(!SoundSys.isPlaying(m_sndBreathSprint) && breathVolume > 0.0f)
			SoundSys.Start(m_sndBreathSprint);
		
		if(SoundSys.isPlaying(m_sndBreathSprint))
		{
			breathVolume *= m_sndBreathBaseVolume;
			SoundSys.SetProperty(m_sndBreathSprint, FMOD_EVENTPROPERTY_VOLUME, &breathVolume);
		}

		if(CurLoadout.Health < 15.0f && !SoundSys.isPlaying(m_sndInPain) && !bDead)
			SoundSys.Start(m_sndInPain);
		else if((CurLoadout.Health >= 15.0f || bDead) && SoundSys.isPlaying(m_sndInPain))
			SoundSys.Stop(m_sndInPain);

	
		bool scopeRendering = hasScopeMode() && CurrentRig.allowScope && m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getCategory() == storecat_SNP;
		if(scopeRendering)
		{
			if(m_isHoldingBreath && m_Breath > 0)
			{
				if(!SoundSys.isPlaying(m_sndSniperHeartSlow))
				{
					SoundSys.Stop(m_sndSniperBreath);
					SoundSys.Start(m_sndSniperHeartSlow);
					SoundSys.PlayAndForget(SoundSys.GetEventIDByPath(m_isFemaleHero?"Sounds/WarZ/PlayerSounds/PLAYER_SNIPER_INHALE_F":"Sounds/WarZ/PlayerSounds/PLAYER_SNIPER_INHALE_M"), GetPosition());
				}
			}
			else
			{
				if(SoundSys.isPlaying(m_sndSniperHeartSlow))
				{
					SoundSys.Stop(m_sndSniperHeartSlow);
					SoundSys.Start(m_sndSniperExhale);
				}
				if(!SoundSys.isPlaying(m_sndSniperExhale))
					if(!SoundSys.isPlaying(m_sndSniperBreath))
						SoundSys.Start(m_sndSniperBreath);
			}
		}
		else
		{
			if(SoundSys.isPlaying(m_sndSniperBreath))
				SoundSys.Stop(m_sndSniperBreath);
			if(SoundSys.isPlaying(m_sndSniperHeartSlow))
				SoundSys.Stop(m_sndSniperHeartSlow);
		}
	}
	else // for all players (except NetworkLocal), 3D sounds
	{

	}

	// for all players, 3D sounds
	UpdateSwimSounds();
}

void obj_Player::UpdateFootsteps()
{
	R3DPROFILE_FUNCTION("UpdateFootsteps");
	R3DPROFILE_START("RayCast");
	// cast ray to see on what material player is standing
	PhysicsCallbackObject* target = NULL;
	const MaterialType *mt = 0;

	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));

	// did we hit anything?
	bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y + 0.5f, GetPosition().z), PxVec3(0, -1, 0), 1.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
	if( hitResult )
	{
		if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
		{
			PxU32 faceIndex = hit.faceIndex;
			{
				PxTriangleMeshGeometry pxGeometry;
				if(hit.shape->getTriangleMeshGeometry(pxGeometry))
				{
					r3d_assert(pxGeometry.triangleMesh);
					const PxU32* remap = pxGeometry.triangleMesh->getTrianglesRemap();
					r3d_assert(remap);
					PxU32 numTriangles = pxGeometry.triangleMesh->getNbTriangles();
					r3d_assert(faceIndex < numTriangles);
					faceIndex = remap[faceIndex];
				}
			}
			r3dMaterial* material = 0;
			GameObject *gameObj = target->isGameObject();
			if(gameObj)
			{
				if( gameObj->isObjType( OBJTYPE_Mesh ) )
					material = static_cast< MeshGameObject* > ( target )->MeshLOD[ 0 ]->GetFaceMaterial( faceIndex );
				if(gameObj->isObjType(OBJTYPE_Terrain))
				{
					mt = Terrain->GetMaterialType(GetPosition());
				}
			}
			else if(target->hasMesh())
			{
				material = target->hasMesh()->GetFaceMaterial( faceIndex );
			}
			if (!material)
				material = target->GetMaterial(faceIndex);

			if(material && !mt) // get material type from mesh material
			{
				mt = g_pMaterialTypes->Get(r3dHash::MakeHash(material->TypeName));
			}

			float waterDepth = getWaterDepthAtPos(GetPosition());
			if(waterDepth > 0.1f) 
			{
				mt = g_pMaterialTypes->Get(r3dHash::MakeHash("Water"));
			}

		}
	}
	R3DPROFILE_END("RayCast");
	{
		R3DPROFILE_FUNCTION("play sound");
		// hack: right now we have a problem with faceID in raycast not matching faceID of the actual mesh (collision mesh vs regular mesh). 
		// So, in case if we failed to find a proper material because of that, let's just use old foot step sound and that's it
		int fts = currentFootStepsSoundID; 
		if(mt)
		{
			if((PlayerState>=PLAYER_MOVE_CROUCH && PlayerState<=PLAYER_MOVE_CROUCH_AIM) && mt->crouchFootstepsSound>=0)
				fts = mt->crouchFootstepsSound;
			else if((PlayerState>=PLAYER_MOVE_PRONE && PlayerState<=PLAYER_PRONE_IDLE) && mt->proneFootstepsSound>=0)
				fts = mt->proneFootstepsSound;
			else
				fts = mt->footStepsSound;
		}
		if (!SoundSys.IsHandleValid(footStepsSnd) || fts != currentFootStepsSoundID)
		{
			SoundSys.Stop(footStepsSnd);
			footStepsSnd = NULL;
			footstepsSndBaseVolume = 1.0f;
			
			for(uint32_t i=0; i<footStepsArr.Count(); ++i) 
			{
				if(footStepsArr[i].id == fts)
				{
					footStepsSnd = footStepsArr[i].snd;
					footstepsSndBaseVolume = footStepsArr[i].base_volume;

					// check if handle is valid, sometimes it becomes invalid for some reason
					if(!SoundSys.IsHandleValid(footStepsSnd))
					{
						// handle is broken, remove it from the list, and remove it from FMOD
						SoundSys.Stop(footStepsArr[i].snd);
						SoundSys.Release(footStepsArr[i].snd);
						footStepsArr.Erase(i);	
						footStepsSnd = NULL;
						footstepsSndBaseVolume = 1.0f;
					}
					break;
				}
			}
			
			if(footStepsSnd == NULL)
			{
				footStepsSnd = SoundSys.Play(fts, GetPosition());
				if (!SoundSys.IsHandleValid(footStepsSnd))
					return;
				SoundSys.GetProperty(footStepsSnd, FMOD_EVENTPROPERTY_VOLUME, &footstepsSndBaseVolume);
				footStepsArr.PushBack(footStepEntry_s(fts, footStepsSnd, footstepsSndBaseVolume));
			}

			currentFootStepsSoundID = fts;
		}

		SoundSys.Set3DAttributes(footStepsSnd, &GetPosition(), 0, 0);
		
		static const float SpawnRatesByState[] = {
			0,	//PLAYER_IDLE,
			0,	//PLAYER_IDLEAIM,
			0.6f,	//PLAYER_MOVE_CROUCH,
			0.54f,	//PLAYER_MOVE_CROUCH_AIM,
			0.49f,	//PLAYER_MOVE_WALK_AIM,
			0.95f,	//PLAYER_MOVE_RUN,
			1.1f,	//PLAYER_MOVE_SPRINT,
			1.0f,	//PRONE
			1.0f,	//PRONE_AIM
			1.0f,	//PRONE_UP
			0,		//PRONE_DOWN
			0,		//PRONE_IDLE
			0,		//PLAYER_SWIM_IDLE
			0,		//PLAYER_SWIM_SLOW
			0,		//PLAYER_SWIM
			0,		//PLAYER_SWIM_FAST
#ifdef VEHICLES_ENABLED
			0,		//VEHICLE DRIVER
			0,		//VEHICLE PASSENGER
#endif
			0,		//PLAYER_DIE
		};
		COMPILE_ASSERT( R3D_ARRAYSIZE(SpawnRatesByState) == PLAYER_NUM_STATES ) ;
		float spawnRate = SpawnRatesByState[PlayerState];
		if(PlayerMoveDir == CUberData::ANIMDIR_Stand || hitResult == false )
			spawnRate = 0.0f;

		if(spawnRate > 0)
		{
			footsteps_timeSinceLastMissingHitResult = r3dGetTime();
			if (SoundSys.GetState(footStepsSnd) != FMOD_EVENT_STATE_PLAYING)
			{
				SoundSys.Start(footStepsSnd);
			}

			SoundSys.SetProperty(footStepsSnd, FMOD_EVENTPROPERTY_SPAWNINTENSITY, &spawnRate);
			if(CurLoadout.Skills[CUserSkills::SKILL_Survival3])
			{
				float v = footstepsSndBaseVolume*0.66f;
				SoundSys.SetProperty(footStepsSnd, FMOD_EVENTPROPERTY_VOLUME, &v);
			}
		}
		else
		{
			if((r3dGetTime() - footsteps_timeSinceLastMissingHitResult)>0.25f) // workaround when you are running across wooden planks and sounds restart each time you step on new wooden plank
				SoundSys.Stop(footStepsSnd);
		}
	}
}

void obj_Player::UpdateSwimSounds()
{
	if(!SoundSys.isPlaying( m_sndSwimIdle ) && PlayerState == PLAYER_SWIM_IDLE) // started idle swimming
	{
		if(!SoundSys.IsHandleValid( m_sndSwimIdle ) )
		{
			SoundSys.Stop( m_sndSwimIdle );
			m_sndSwimIdle = SoundSys.Play( m_sndSwimIdleId, GetPosition() );
		}
		else
		{
			SoundSys.Set3DAttributes( m_sndSwimIdle, &GetPosition(), 0, 0 );
			SoundSys.Start( m_sndSwimIdle );
		}
	}
	else if(SoundSys.isPlaying( m_sndSwimIdle ))
	{
		if( PlayerState != PLAYER_SWIM_IDLE )
			SoundSys.Stop( m_sndSwimIdle );		// stopped idle swimming
		else
			SoundSys.Set3DAttributes( m_sndSwimIdle, &GetPosition(), 0, 0 );
	}
	if(!SoundSys.isPlaying( m_sndSwimSlow ) && PlayerState == PLAYER_SWIM_SLOW) // started swimming slow
	{
		if(!SoundSys.IsHandleValid( m_sndSwimSlow ))
		{
			SoundSys.Stop( m_sndSwimSlow );
			m_sndSwimSlow = SoundSys.Play( m_sndSwimSlowId, GetPosition() );
		}
		else
		{
			SoundSys.Set3DAttributes( m_sndSwimSlow, &GetPosition(), 0, 0 );
			SoundSys.Start( m_sndSwimSlow );
		}
	}
	else if(SoundSys.isPlaying( m_sndSwimSlow ))
	{
		if( PlayerState != PLAYER_SWIM_SLOW )
			SoundSys.Stop( m_sndSwimSlow );		// stopped swimming slow
		else
			SoundSys.Set3DAttributes( m_sndSwimSlow, &GetPosition(), 0, 0 );
	}
	if(!SoundSys.isPlaying( m_sndSwim ) && PlayerState == PLAYER_SWIM) // started swimming
	{
		if(!SoundSys.IsHandleValid( m_sndSwim ))
		{
			SoundSys.Stop( m_sndSwim );
			m_sndSwim = SoundSys.Play( m_sndSwimId, GetPosition() );
		}
		else
		{
			SoundSys.Set3DAttributes( m_sndSwim, &GetPosition(), 0, 0 );
			SoundSys.Start( m_sndSwim );
		}
	}
	else if(SoundSys.isPlaying( m_sndSwim ))
	{
		if( PlayerState != PLAYER_SWIM )
			SoundSys.Stop( m_sndSwim );			// stopped swimming
		else
			SoundSys.Set3DAttributes( m_sndSwim, &GetPosition(), 0, 0 );
	}
	if(!SoundSys.isPlaying( m_sndSwimFast ) && PlayerState == PLAYER_SWIM_FAST) // started swimming fast
	{
		if(!SoundSys.IsHandleValid( m_sndSwimFast ))
		{
			SoundSys.Stop( m_sndSwimFast );
			m_sndSwimFast = SoundSys.Play( m_sndSwimFastId, GetPosition() );
		}
		else
		{
			SoundSys.Set3DAttributes( m_sndSwimFast, &GetPosition(), 0, 0 );
			SoundSys.Start( m_sndSwimFast );
		}
	}
	else if(SoundSys.isPlaying( m_sndSwimFast ))
	{
		if( PlayerState != PLAYER_SWIM_FAST )
			SoundSys.Stop( m_sndSwimFast );		// stopped swimming fast
		else
			SoundSys.Set3DAttributes( m_sndSwimFast, &GetPosition(), 0, 0 );
	}
}

void obj_Player::updateWeaponUI()
{
	if(!hudMain)
		return;
	if(!NetworkLocal)
		return;

	for(int i=0; i<6; ++i)
	{
		const BaseItemConfig* baseItem = g_pWeaponArmory->getConfig(CurLoadout.Items[i].itemID);
		int numCharges = CurLoadout.Items[i].quantity;
		if(baseItem)
		{
			hudMain->setSlotInfo(i, baseItem->m_StoreName, numCharges, baseItem->m_StoreIcon);
		}
		else
			hudMain->setSlotInfo(i, "", 0, "");
	}
	if(m_SelectedWeapon != HANDS_WEAPON_IDX && m_Weapons[m_SelectedWeapon])
		hudMain->setActiveSlot(m_SelectedWeapon);
	else
		hudMain->setActiveSlot(100);

	{
		bool isMelee = false;
		if(m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getCategory() == storecat_MELEE)
			isMelee = true;
		if((m_SelectedWeapon < 2 && !isMelee && m_Weapons[m_SelectedWeapon]) || (m_SelectedWeapon>=2 && m_Weapons[m_SelectedWeapon] && m_Weapons[m_SelectedWeapon]->getCategory() == storecat_GRENADE))
		{
			hudMain->showWeaponInfo(true);
		}
		else
		{
			hudMain->showWeaponInfo(false);
		}
	}
}

void obj_Player::switchFPS_TPS()
{
	r3d_assert(NetworkLocal);
	if(bDead || IsSwimming())	return;			//Cynthia: fix bugID975 dead is dead.
	
	g_camera_mode->SetInt(g_camera_mode->GetInt()==2?0:2);
	
	if(hudMain)
		hudMain->setTPSReticleVisibility(g_camera_mode->GetInt()==0?1:0);

	// we only need to reload body meshes
	const HeroConfig* heroConfig = g_pWeaponArmory->getHeroConfig(CurLoadout.HeroItemID);
	loadHeroMeshes(heroConfig);
	// reload weapon meshes (to load skeleton for FPS mode)
	// create weapons & items
	for(int i=0; i<NUM_WEAPONS_ON_PLAYER; i++)
	{
		if(m_Weapons[i])
		{
			m_Weapons[i]->setFirstPersonMeshFlag(g_camera_mode->GetInt()==2);
			m_Weapons[i]->ResetMeshLoadedFlag();
			uint32_t attmIDs[WPN_ATTM_MAX];
			m_Weapons[i]->getCurrentAttachmentIDs(attmIDs);
			uint32_t zeroID[WPN_ATTM_MAX] = {0};
			m_Weapons[i]->setWeaponAttachmentsByIDs(zeroID); // reset attachments if any
			m_Weapons[i]->setWeaponAttachmentsByIDs(attmIDs); // reload attachments
		}
	}

	uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
	uberAnim_->AnimPlayerState = -1;
	SyncAnimation(true);
}

float obj_Player::getPlayerHeightForCamera() const
{
	return Height;
}

float obj_Player::getPlayerHeightForCameraWithPlayerState() const
{
	if(PlayerState == PLAYER_MOVE_CROUCH || PlayerState == PLAYER_MOVE_CROUCH_AIM)
		return 1.2f;
	else if(PlayerState >= PLAYER_MOVE_PRONE && PlayerState <= PLAYER_PRONE_IDLE)
		return 0.8f;
	else
		return 1.8f;
}

//////////////////////////////////////////////////////////////////////////

void obj_Player::DoRagdoll(bool toRagdoll, int hitBone, const r3dPoint3D* impulse, bool isVehicleHit)
{
#if ENABLE_RAGDOLL
	if (m_PhysSkeleton)
	{
		if (impulse)
		{
#ifdef VEHICLES_ENABLED
			if (isVehicleHit)
				m_PhysSkeleton->SwitchToRagdollWithImpulseForce(true, hitBone, impulse);
			else
#endif
				m_PhysSkeleton->SwitchToRagdollWithForce(toRagdoll, hitBone, impulse);
		}
		else
		{
			m_PhysSkeleton->SwitchToRagdoll(toRagdoll);
		}

		if (!m_PhysSkeleton->IsRagdollMode())
		{
			SetBBoxLocal(m_OrigBBox);
		}
    }
#endif
}

void obj_Player::OnGameEnded()
{
	if(m_SelectedWeapon >= 0 && m_Weapons[m_SelectedWeapon] )
	{
		// currently we only need to do this to the equipped weapon, it can be expanded as needed.
		m_Weapons[m_SelectedWeapon]->OnGameEnded();
	}
}

void obj_Player::BackpackGridSwap(int gridFrom, int gridTo)
{
	r3d_assert(NetworkLocal);
	
	//local logic
	wiCharDataFull& slot = CurLoadout;
	wiInventoryItem& wi1 = slot.Items[gridFrom];
	wiInventoryItem& wi2 = slot.Items[gridTo];

	// if we can stack slots - do it
	if(wi1.CanStackWith(wi2))
	{
		wi2.quantity += wi1.quantity;
		wi1.Reset();

		PKT_C2S_BackpackJoin_s n;
		n.SlotFrom = gridFrom;
		n.SlotTo   = gridTo;
		p2pSendToHost(this, &n, sizeof(n));
	}
	else
	{
		R3D_SWAP(wi1, wi2);

		PKT_C2S_BackpackSwap_s n;
		n.SlotFrom = gridFrom;
		n.SlotTo   = gridTo;
		p2pSendToHost(this, &n, sizeof(n));
	}

	OnBackpackChanged(gridFrom);
	OnBackpackChanged(gridTo);
}

void obj_Player::OnBackpackChanged(int idx)
{
	r3d_assert(NetworkLocal);
	
	if(idx == wiCharDataFull::CHAR_LOADOUT_WEAPON1 || idx == wiCharDataFull::CHAR_LOADOUT_WEAPON2)
	{
		// attachments are reset on item change (SERVER CODE SYNC POINT)
		CurLoadout.Attachment[idx].Reset();
		if(CurLoadout.Items[idx].Var2 > 0)
			CurLoadout.Attachment[idx].attachments[WPN_ATTM_CLIP] = CurLoadout.Items[idx].Var2;
	}

	// check if we need to modify loadout
	if(idx >= wiCharDataFull::CHAR_LOADOUT_WEAPON1 && idx <= wiCharDataFull::CHAR_LOADOUT_HEADGEAR)
	{
		UpdateLoadoutSlot(CurLoadout, idx);

		updateWeaponUI();
	}

	// if trying to put weapon into quick slot that is active, then force empty hands
	if(idx >= wiCharDataFull::CHAR_LOADOUT_ITEM1 && idx <= wiCharDataFull::CHAR_LOADOUT_ITEM4 && 
		m_SelectedWeapon == idx && m_Weapons[m_SelectedWeapon] && !m_Weapons[m_SelectedWeapon]->isUsableItem())
	{	
		ChangeWeaponByIndex(HANDS_WEAPON_IDX);
	}

	if(hudPause && hudPause->isActive())
	{
		hudPause->reloadBackpackInfo();
	}
	if(hudVault && hudVault->isActive())
	{
		hudVault->reloadBackpackInfo();
	}
	if(hudRepair && hudRepair->isActive())
	{
		hudRepair->reloadBackpackInfo(); // it should not happen though...
	}
	if(hudCraft && hudCraft->isActive())
	{
		hudCraft->reloadBackpackInfo(); 
	}
	if(hudSafelock && hudSafelock->isActive())
	{
		hudSafelock->reloadBackpackInfo();
	}

	if(NetworkLocal && (idx == wiCharDataFull::CHAR_LOADOUT_ARMOR || idx == wiCharDataFull::CHAR_LOADOUT_HEADGEAR))
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_CHANGECLOTHES"), GetPosition());
}

void obj_Player::OnRemoveAttachments(int idx)
{
	if(idx == wiCharDataFull::CHAR_LOADOUT_WEAPON1 || idx == wiCharDataFull::CHAR_LOADOUT_WEAPON2)
	{
		CurLoadout.Attachment[idx].Reset();
		if(CurLoadout.Items[idx].Var2 > 0)
			CurLoadout.Attachment[idx].attachments[WPN_ATTM_CLIP] = CurLoadout.Items[idx].Var2;
	}
}

void obj_Player::TogglePhysicsSimulation(bool on)
{
	if (PhysicsObject)
	{
		PxActor *a = PhysicsObject->getPhysicsActor();
		if (a)
		{
			on ? g_pPhysicsWorld->AddActor(*a) : g_pPhysicsWorld->RemoveActor(*a);
		}
		if (m_PhysSkeleton)
		{
			m_PhysSkeleton->TogglePhysicsSimulation(on);
		}
	}
}

#ifdef VEHICLES_ENABLED

obj_Vehicle* obj_Player::IsNearVehicle()
{
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_PLAYER_COLLIDABLE_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC | PxSceneQueryFilterFlag::eDYNAMIC);
	r3dVector rayDir = gCam.vPointTo;
	float rayLen = 10.0f;
	r3dVector rayDirN = rayDir.Normalize();
	if(g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y + 1.5f, GetPosition().z), PxVec3(rayDir.x, rayDir.y, rayDir.z), rayLen, PxSceneQueryFlag::eIMPACT, hit, filter))
	{
		PhysicsCallbackObject* callbackObj = NULL;
		if(hit.shape && (callbackObj = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
		{
			GameObject* gameObj = callbackObj->isGameObject();
			if(gameObj && gameObj->isObjType(OBJTYPE_Vehicle))
				return (obj_Vehicle*)gameObj;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

void obj_Player::enterVehicle(obj_Vehicle* targetVehicle, int seat)
{
	r3d_assert(!IsInVehicle());

	isPlayerInVehicle = true;

	TogglePhysicsSimulation(false);

	targetVehicle->TypeCamera = 0;
	targetVehicle->enableInteriorCam = false;
	if (targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HUMMER || 
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_ABANDONEDSUV ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_DUNEBUGGY ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_JEEP ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE)
		m_enableRendering = true;
	else 
		m_enableRendering = false;

	currentVehicle = targetVehicle;
	seatPosition = seat;
	currentVehicleId = targetVehicle->vehicleId;

	if (NetworkLocal)
	{
		g_pPhysicsWorld->m_VehicleManager->DriveCar(targetVehicle->vd);

		if (seatPosition == 0)
		{
			vehicleViewActive_ = VehicleView_Driver;
		}
		else
		{
			vehicleViewActive_ = VehicleView_Passenger;
		}

		if( hudTrade && hudTrade->isActive() )
			hudTrade->requestClose();
	}
	
	targetVehicle->EnterVehicle(this);

	if (seatPosition == 0)
		PlayerState = PLAYER_VEHICLE_DRIVER; // this is a temporary solution for animations.
	else
		PlayerState = PLAYER_VEHICLE_PASSENGER;
	
	hasOpenVehicleRequest = false;

#ifndef FINAL_BUILD
	if (g_bEditMode)
	{
		m_enableRendering = false;
		return;
	}
#endif

	//if (targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_BUGGY)
		SyncAnimation(true);
	if (targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HUMMER || 
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_ABANDONEDSUV ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_DUNEBUGGY ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_JEEP ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE)
		m_enableRendering = true;
	else 
		m_enableRendering = false;
}

void obj_Player::exitVehicle(obj_Vehicle* targetVehicle, r3dPoint3D exitPosition)
{
	r3d_assert(targetVehicle);

	targetVehicle->ExitVehicle(this);

	targetVehicle->TypeCamera = 0;
	targetVehicle->enableInteriorCam = false;
	if (targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HUMMER || 
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_ABANDONEDSUV ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_DUNEBUGGY ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_JEEP ||
		targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE)
		m_enableRendering = true;
	else 
		m_enableRendering = false;

	if (NetworkLocal)
	{
		vehicleViewActive_ = VehicleView_None; 

		if (seatPosition == 0)
			g_pPhysicsWorld->m_VehicleManager->SetHasDrivableCar(false);
	}
	if(targetVehicle->isTank)
	{
		exitPosition = targetVehicle->GetPosition() + r3dPoint3D( 6.0f, 0.5f, 6.0f); // Server Vehicles
	}
	TeleportPlayer(exitPosition, "Exit vehicle"); // temporary code.
	TogglePhysicsSimulation(true);

	ClearVehicle();

#ifndef FINAL_BUILD
	if (g_bEditMode)
	{
		m_enableRendering = true;
		return;
	}
#endif

	//if (targetVehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_BUGGY)
		SyncAnimation(true);
	//else 
		m_enableRendering = true;
}

void obj_Player::ClearVehicle()
{
	if (NetworkLocal)
		g_pPhysicsWorld->m_VehicleManager->DriveCar(0);

	currentVehicleId = -1;
	seatPosition = -1;
	isPlayerInVehicle = false;
	hasOpenVehicleRequest = false;
	PlayerState = PLAYER_IDLE;
	currentVehicle = 0;
}

bool obj_Player::IsInVehicle() 
{
	return isPlayerInVehicle;
}

bool obj_Player::TestExitPosition( r3dVector& exitPosition, bool ignoreGround)
{
	// snap to ground
	if ( ignoreGround == false ) {
		// first find out if there's ground. 
		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
		if(g_pPhysicsWorld->raycastSingle(PxVec3(exitPosition.x, exitPosition.y+1, exitPosition.z), PxVec3(0, -1, 0), 2, PxSceneQueryFlag::eIMPACT, hit, filter))
		{
			exitPosition.x = hit.impact.x;
			exitPosition.y = hit.impact.y;
			exitPosition.z = hit.impact.z;
		} 
		else 
		{
			return false;
		}
	}

	// check spawn area. 
	PxShape* overlapHit = NULL;
	PxU32 collisionFlag = COLLIDABLE_STATIC_MASK ;
	PxBoxGeometry playerBox( 0.2f, 0.9f, 0.2f) ;
	PxTransform pose(PxVec3(exitPosition.x, exitPosition.y +1.1f, exitPosition.z), PxQuat(0,0,0,1));
	PxSceneQueryFilterData capsulefilter(PxFilterData(collisionFlag, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
	if ( g_pPhysicsWorld->PhysXScene->overlapAny( playerBox, pose, overlapHit, capsulefilter))
	{
		return false;
	}

	return true;
}

void obj_Player::RequestVehicleEnter(DWORD vehicleId)
{
	r3d_assert(gClientLogic().localPlayer_);

	if (hasOpenVehicleRequest)
		return;

	hasOpenVehicleRequest = true;

	if(m_SelectedWeapon != HANDS_WEAPON_IDX)
		ChangeWeaponByIndex(HANDS_WEAPON_IDX);

	if(hudMain)
	{
		hudMain->showRangeFinderUI(false);	
		hudMain->setTPSReticleVisibility(0);
	}

	m_isAiming = false;

	PKT_C2S_VehicleEnter_s packet;
	packet.vehicleId = vehicleId;

	p2pSendToHost(this, &packet, sizeof(packet));
}

void obj_Player::RequestVehicleExit(DWORD vehicleId)
{
	r3d_assert(gClientLogic().localPlayer_);

	if (hasOpenVehicleRequest)
		return;

	hasOpenVehicleRequest = true;

	PKT_C2S_VehicleExit_s packet;
	packet.vehicleId = vehicleId;

	p2pSendToHost(this, &packet, sizeof(packet));
}

void obj_Player::AttemptVehicleHit(PxF32 vehicleSpeed, obj_Vehicle* vehicle)
{
	float distance = (vehicle->GetPosition() - GetPosition()).Length();
	if (distance > 10.0f || vehicleSpeed < 8.0f || isHitByVehicle || (this->GroupID != 0 && this->GroupID == gClientLogic().localPlayer_->GroupID) || m_AuraType == AT_SPAWNPROTECTION || CurLoadout.Health == 0)
		return;

	isHitByVehicle = true;

	if (vehicleSpeed * 15.0f > 100.0f)
		preparedToFakeDie = true;

	if (vehicle->NetworkLocal)
	{
		PKT_C2S_VehicleHitTarget_s n;
		n.targetId = toP2pNetId(GetNetworkID());
		p2pSendToHost(vehicle, &n, sizeof(n));
	}
	
	r3dVector vec = vehicle->GetvForw();
	lastTimeHitForce = r3dPoint3D(vec.x, vec.y, vec.z);
	lastTimeHitBone = 2;
	lastTimeHit = r3dGetTime();
}

void obj_Player::OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace)
{
	if (bDead || !obj)
		return;

	GameObject* gameObj = obj->isGameObject();
	if (!gameObj)
		return;

	if (gameObj->isObjType(OBJTYPE_Vehicle))
	{
		obj_Vehicle* vehicle = (obj_Vehicle*)gameObj;
		StartPushFromVehicle(vehicle->vd->GetSpeed(), vehicle->vd->GetMaxSpeed());
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

void ProcessCharacterEditor(obj_Player* pl, float left, float top, float height);
void ProcessCharacterEditor()
{
	obj_Player* pl = (obj_Player *)GameWorld().GetObject(EditorGameHUD::editorPlayerId);
	if (!pl)
	{
		imgui_Static(r3dRenderer->ScreenW-375, 85, "Spawn player firstly by pressing F8");
		return;
	}

	ProcessCharacterEditor(pl, 100, 100, 150);
}

void ProcessCharacterEditor(obj_Player* pl, float left, float top, float height)
{
	stringlist_t weaponDB_nameList;
	uint32_t* weaponDB_idList = 0;

	stringlist_t attachmentDB_muzzle_nameList;
	uint32_t* attachmentDB_muzzle_idList = 0;
	stringlist_t attachmentDB_upperRail_nameList;
	uint32_t* attachmentDB_upperRail_idList = 0;
	stringlist_t attachmentDB_leftRail_nameList;
	uint32_t* attachmentDB_leftRail_idList = 0;
	stringlist_t attachmentDB_bottomRail_nameList;
	uint32_t* attachmentDB_bottomRail_idList = 0;
	stringlist_t attachmentDB_clip_nameList;
	uint32_t* attachmentDB_clip_idList = 0;

	static int selectedPrimaryWeaponIndex = -1;
	static int selectedSecondaryWeaponIndex = -1;
	static int selectedBodyIndex = 0;
	static int selectedHeroIndex = 1;
	static int selectedLegsIndex = 0;
	static int selectedArmorIndex = 0;
	static int selectedHeadIndex = 0;
	static int selectedHeadArmorIndex = 0;

	stringlist_t armorDBnames, bodyDBnames, heroDBnames, legsDBnames, headDBnames, headarmorBDnames;
	uint32_t *armorDBids = 0, *bodyDBids = 0, *heroDBids = 0, *legsDBids = 0, *headDBids = 0, *headarmorDBids = 0;
	if(weaponDB_nameList.empty()) // fill in list
	{
		weaponDB_nameList.push_back("empty");
		attachmentDB_muzzle_nameList.push_back("empty");
		attachmentDB_upperRail_nameList.push_back("empty");
		attachmentDB_leftRail_nameList.push_back("empty");
		attachmentDB_bottomRail_nameList.push_back("empty");
		attachmentDB_clip_nameList.push_back("empty");

		// sort item names there.
		struct CWpnNameSort {
			static int sort(const void* P1, const void* P2)	{
				const WeaponConfig* g1 = *(const WeaponConfig**)P1;
				const WeaponConfig* g2 = *(const WeaponConfig**)P2;
				return strcmp(g1->m_StoreIcon, g2->m_StoreIcon);
			}
		};
		struct CWpnAttachmentNameSort {
			static int sort(const void* P1, const void* P2)	{
				const WeaponAttachmentConfig* g1 = *(const WeaponAttachmentConfig**)P1;
				const WeaponAttachmentConfig* g2 = *(const WeaponAttachmentConfig**)P2;
				return strcmp(g1->m_StoreIcon, g2->m_StoreIcon);
			}
		};
		r3dgameVector(const WeaponConfig*) allWpns;
		g_pWeaponArmory->startItemSearch();
		while(g_pWeaponArmory->searchNextItem())
		{
			uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
			const WeaponConfig* config = g_pWeaponArmory->getWeaponConfig(itemID);
			if(config)
			{
				allWpns.push_back(config);
			}
		}
		qsort(&allWpns[0], allWpns.size(), sizeof(WeaponConfig*), CWpnNameSort::sort);

		int numItemsInWeaponList = g_pWeaponArmory->getNumWeapons() + 1;
		weaponDB_idList = game_new uint32_t[numItemsInWeaponList];
		weaponDB_idList[0] = -1;
		for(int i=1; i<numItemsInWeaponList; ++i)
		{
			const WeaponConfig* config = allWpns[i-1];

			// get item FNAME without .dds
			char wname[256];
			sprintf(wname, strrchr(config->m_StoreIcon, '/') + 1);
			wname[strlen(wname)-4] = 0;

			weaponDB_nameList.push_back(wname);
			weaponDB_idList[i] = config->m_itemID;

			if(selectedPrimaryWeaponIndex==-1 && config->m_itemID == pl->CurLoadout.Items[0].itemID)
				selectedPrimaryWeaponIndex = i;
			if(selectedSecondaryWeaponIndex==-1 && config->m_itemID == pl->CurLoadout.Items[1].itemID)
				selectedSecondaryWeaponIndex = i;
		}

		if(selectedPrimaryWeaponIndex==-1)
			selectedPrimaryWeaponIndex = 0;
		if(selectedSecondaryWeaponIndex==-1)
			selectedSecondaryWeaponIndex = 0;

		int numItemsInAttachmentList = g_pWeaponArmory->getNumAttachments() + 1;
		attachmentDB_muzzle_idList = game_new uint32_t[numItemsInAttachmentList]; int numMuzzles = 0;
		attachmentDB_muzzle_idList[0] = 0;
		attachmentDB_upperRail_idList = game_new uint32_t[numItemsInAttachmentList]; int numUpperRails = 0;
		attachmentDB_upperRail_idList[0] = 0;
		attachmentDB_leftRail_idList = game_new uint32_t[numItemsInAttachmentList]; int numLeftRails = 0;
		attachmentDB_leftRail_idList[0] = 0;
		attachmentDB_bottomRail_idList = game_new uint32_t[numItemsInAttachmentList]; int numBottomRails = 0;
		attachmentDB_bottomRail_idList[0] = 0;
		attachmentDB_clip_idList = game_new uint32_t[numItemsInAttachmentList]; int numClips = 0;
		attachmentDB_clip_idList[0] = 0;

		g_pWeaponArmory->startItemSearch();
		while(g_pWeaponArmory->searchNextItem())
		{
			uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
			const WeaponAttachmentConfig* config = g_pWeaponArmory->getAttachmentConfig(itemID);
			if(config)
			{
				switch(config->m_type)
				{
				case WPN_ATTM_MUZZLE:
					attachmentDB_muzzle_nameList.push_back(config->m_StoreName);
					attachmentDB_muzzle_idList[++numMuzzles] = config->m_itemID;
					break;
				case WPN_ATTM_UPPER_RAIL:
					attachmentDB_upperRail_nameList.push_back(config->m_StoreName);
					attachmentDB_upperRail_idList[++numUpperRails] = config->m_itemID;
					break;
				case WPN_ATTM_LEFT_RAIL:
					attachmentDB_leftRail_nameList.push_back(config->m_StoreName);
					attachmentDB_leftRail_idList[++numLeftRails] = config->m_itemID;
					break;
				case WPN_ATTM_BOTTOM_RAIL:
					attachmentDB_bottomRail_nameList.push_back(config->m_StoreName);
					attachmentDB_bottomRail_idList[++numBottomRails] = config->m_itemID;
					break;
				case WPN_ATTM_CLIP:
					attachmentDB_clip_nameList.push_back(config->m_StoreName);
					attachmentDB_clip_idList[++numClips] = config->m_itemID;
					break;
				default:
					break;
				}
			}
		}
		int numItemsInGearList = g_pWeaponArmory->getNumGears() + 1;
		armorDBids = game_new uint32_t[numItemsInGearList]; int numArmor = 0;
		headarmorDBids = game_new uint32_t[numItemsInGearList]; int numHeadarmor = 0;

		headarmorBDnames.push_back("empty");
		armorDBnames.push_back("empty");
		armorDBids[0] = 0;
		headarmorDBids[0] = 0;
	
		g_pWeaponArmory->startItemSearch();
		while(g_pWeaponArmory->searchNextItem())
		{
			uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
			const GearConfig* config = g_pWeaponArmory->getGearConfig(itemID);
			if(config)
			{
				switch(config->category)
				{
				case storecat_Armor:
					armorDBnames.push_back(config->m_StoreName);
					armorDBids[++numArmor] = config->m_itemID;
					if(selectedArmorIndex==-1 && config->m_itemID == pl->CurLoadout.Items[6].itemID)
						selectedArmorIndex = numArmor;
					break;
				case storecat_Helmet:
					headarmorBDnames.push_back(config->m_StoreName);
					headarmorDBids[++numHeadarmor] = config->m_itemID;
					if(selectedHeadArmorIndex==-1 && config->m_itemID == pl->CurLoadout.Items[7].itemID)
						selectedHeadArmorIndex = numHeadarmor;
					break;
				default:
					break;
				}
			}
		}
		int numItemsInHeroList = g_pWeaponArmory->getNumHeroes();
		heroDBids = game_new uint32_t[numItemsInHeroList]; int numHero = 0;

		g_pWeaponArmory->startItemSearch();
		while(g_pWeaponArmory->searchNextItem())
		{
			uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
			const HeroConfig* config = g_pWeaponArmory->getHeroConfig(itemID);
			if(config)
			{
				heroDBnames.push_back(config->m_StoreName);
				heroDBids[numHero] = config->m_itemID;
				if(selectedHeroIndex==-1 && config->m_itemID == pl->CurLoadout.HeroItemID)
					selectedHeroIndex = numHero;
				numHero++;
			}
		}
	}

	static const int width = 150;
//	static const int height = 150;
	static const int shift = 25;

	static int selectedPrimaryWeapon_Muzzle = 0;
	static int selectedPrimaryWeapon_UpperRail = 0;
	static int selectedPrimaryWeapon_LeftRail = 0;
	static int selectedPrimaryWeapon_BottomRail= 0;
	static int selectedPrimaryWeapon_Clip = 0;

	static int selectedSecondaryWeapon_Muzzle = 0;
	static int selectedSecondaryWeapon_UpperRail = 0;
	static int selectedSecondaryWeapon_LeftRail = 0;
	static int selectedSecondaryWeapon_BottomRail= 0;
	static int selectedSecondaryWeapon_Clip = 0;

	float offset = top + shift + height - 20;
	{
		static float primWeaponOffset = 0;
		imgui_Static(left + width * 7, top, "Primary Weapon", width);
		imgui_DrawList(left + width * 7, top + shift, (float)width, (float)height, weaponDB_nameList, &primWeaponOffset, &selectedPrimaryWeaponIndex);

		if(g_camera_mode->GetInt() == 2)
		{
			static float primWeaponAttachmentMuzzleOffset = 0;
			imgui_Static(left + width * 7, offset, "Muzzle", width);
			imgui_DrawList(left + width * 7, offset + 20, (float)width, (float)100.0f, attachmentDB_muzzle_nameList, &primWeaponAttachmentMuzzleOffset, &selectedPrimaryWeapon_Muzzle);

			static float primWeaponAttachmentUpperRailOffset = 0;
			imgui_Static(left + width * 7, offset + 100, "Upper Rail", width);
			imgui_DrawList(left + width * 7, offset + 20 + 100, (float)width, (float)100.0f, attachmentDB_upperRail_nameList, &primWeaponAttachmentUpperRailOffset, &selectedPrimaryWeapon_UpperRail);

			static float primWeaponAttachmentLeftRailOffset = 0;
			imgui_Static(left + width * 7, offset + 200, "Left Rail", width);
			imgui_DrawList(left + width * 7, offset + 20 + 200, (float)width, (float)100.0f, attachmentDB_leftRail_nameList, &primWeaponAttachmentLeftRailOffset, &selectedPrimaryWeapon_LeftRail);

			static float primWeaponAttachmentBottomRailOffset = 0;
			imgui_Static(left + width * 7, offset + 300, "Bottom Rail", width);
			imgui_DrawList(left + width * 7, offset + 20 + 300, (float)width, (float)100.0f, attachmentDB_bottomRail_nameList, &primWeaponAttachmentBottomRailOffset, &selectedPrimaryWeapon_BottomRail);

			static float primWeaponAttachmentClipOffset = 0;
			imgui_Static(left + width * 7, offset + 400, "Clip", width);
			imgui_DrawList(left + width * 7, offset + 20 + 400, (float)width, (float)100.0f, attachmentDB_clip_nameList, &primWeaponAttachmentClipOffset, &selectedPrimaryWeapon_Clip);
		}
	}

	{
		static float secWeaponOffset = 0;
		imgui_Static(left + width * 6, top, "Secondary Weapon", width);
		imgui_DrawList(left + width * 6, top + shift, (float)width, (float)height, weaponDB_nameList, &secWeaponOffset, &selectedSecondaryWeaponIndex);

		if(g_camera_mode->GetInt() == 2)
		{
			static float primWeaponAttachmentMuzzleOffset = 0;
			imgui_Static(left + width * 6, offset, "Muzzle", width);
			imgui_DrawList(left + width * 6, offset + 20, (float)width, (float)100.0f, attachmentDB_muzzle_nameList, &primWeaponAttachmentMuzzleOffset, &selectedSecondaryWeapon_Muzzle);

			static float primWeaponAttachmentUpperRailOffset = 0;
			imgui_Static(left + width * 6, offset + 100, "Upper Rail", width);
			imgui_DrawList(left + width * 6, offset + 20 + 100, (float)width, (float)100.0f, attachmentDB_upperRail_nameList, &primWeaponAttachmentUpperRailOffset, &selectedSecondaryWeapon_UpperRail);

			static float primWeaponAttachmentLeftRailOffset = 0;
			imgui_Static(left + width * 6, offset + 200, "Left Rail", width);
			imgui_DrawList(left + width * 6, offset + 20 + 200, (float)width, (float)100.0f, attachmentDB_leftRail_nameList, &primWeaponAttachmentLeftRailOffset, &selectedSecondaryWeapon_LeftRail);

			static float primWeaponAttachmentBottomRailOffset = 0;
			imgui_Static(left + width * 6, offset + 300, "Bottom Rail", width);
			imgui_DrawList(left + width * 6, offset + 20 + 300, (float)width, (float)100.0f, attachmentDB_bottomRail_nameList, &primWeaponAttachmentBottomRailOffset, &selectedSecondaryWeapon_BottomRail);

			static float primWeaponAttachmentClipOffset = 0;
			imgui_Static(left + width * 6, offset + 400, "Clip", width);
			imgui_DrawList(left + width * 6, offset + 20 + 400, (float)width, (float)100.0f, attachmentDB_clip_nameList, &primWeaponAttachmentClipOffset, &selectedSecondaryWeapon_Clip);
		}
	}

	static float heroOffset = 0;
	imgui_Static(left + width * 5, top, "Heroes", width);
	int oldIndex = selectedHeroIndex;
	imgui_DrawList(left + width * 5, top + shift, (float)width, (float)height, heroDBnames, &heroOffset, &selectedHeroIndex);
	if(oldIndex != selectedHeroIndex)
	{
		selectedBodyIndex = 0;
		selectedLegsIndex = 0;
		selectedHeadIndex = 0;
	}

	{
		int heroItemID = heroDBids[selectedHeroIndex];
		const HeroConfig* heroConf = g_pWeaponArmory->getHeroConfig(heroItemID);
		if(heroConf)
		{
			bodyDBids = game_new uint32_t[heroConf->getNumBodys()]; int numBody = 0;
			legsDBids = game_new uint32_t[heroConf->getNumLegs()]; int numLegs = 0;
			headDBids = game_new uint32_t[heroConf->getNumHeads()]; int numHead = 0;

			char tmpStr[16];
			for(size_t i=0; i<heroConf->getNumBodys(); ++i)
			{
				sprintf(tmpStr, "Body %d", i+1);
				bodyDBnames.push_back(tmpStr);
				bodyDBids[numBody] = i;
				if(pl->CurLoadout.BodyIdx == i)
					selectedBodyIndex = numBody;
				numBody++;
			}
			for(size_t i=0; i<heroConf->getNumLegs(); ++i)
			{
				sprintf(tmpStr, "Legs %d", i+1);
				legsDBnames.push_back(tmpStr);
				legsDBids[numLegs] = i;
				if(pl->CurLoadout.LegsIdx == i)
					selectedLegsIndex = numLegs;
				numLegs++;
			}
			for(size_t i=0; i<heroConf->getNumHeads(); ++i)
			{
				sprintf(tmpStr, "Head %d", i+1);
				headDBnames.push_back(tmpStr);
				headDBids[numHead] = i;
				if(pl->CurLoadout.HeadIdx == i)
					selectedHeadIndex = numHead;
				numHead++;
			}
		}
	}

	static float legsOffset = 0;
	imgui_Static(left + width * 4, top, "Legs", width);
	imgui_DrawList(left + width * 4, top + shift, (float)width, (float)height, legsDBnames, &legsOffset, &selectedLegsIndex);

	static float bodyOffset = 0;
	imgui_Static(left + width * 3, top, "Body", width);
	imgui_DrawList(left + width * 3, top + shift, (float)width, (float)height, bodyDBnames, &bodyOffset, &selectedBodyIndex);

	static float headOffset = 0;
	imgui_Static(left + width * 2, top, "Head", width);
	imgui_DrawList(left + width * 2, top + shift, (float)width, (float)height, headDBnames, &headOffset, &selectedHeadIndex);

	static float armorOffset = 0;
	imgui_Static(left + width * 1, top, "Armor", width);
	imgui_DrawList(left + width * 1, top + shift, (float)width, (float)height, armorDBnames, &armorOffset, &selectedArmorIndex);

	static float headArmorOffset = 0;
	imgui_Static(left + width * 0, top, "Head Armor", 150);
	imgui_DrawList(left + width * 0, top + shift, (float)width, (float)height, headarmorBDnames, &headArmorOffset, &selectedHeadArmorIndex);


	//if(imgui_Button(500, 550, 300, 30, "Apply Changes"))
	{
		wiCharDataFull loadout;
		loadout.Health = 100;

		wiInventoryItem wpn1;
		wiInventoryItem wpn2;
		wpn1.itemID = weaponDB_idList[selectedPrimaryWeaponIndex];
		wpn2.itemID = weaponDB_idList[selectedSecondaryWeaponIndex];
		loadout.Items[0]        = wpn1;
		loadout.Items[1]        = wpn2;
		loadout.Items[6].itemID = armorDBids[selectedArmorIndex];
		loadout.Items[7].itemID = headarmorDBids[selectedHeadArmorIndex];

		loadout.HeroItemID = heroDBids[selectedHeroIndex];
		loadout.HeadIdx = headDBids[selectedHeadIndex];
		loadout.BodyIdx = bodyDBids[selectedBodyIndex];
		loadout.LegsIdx = legsDBids[selectedLegsIndex];

		loadout.Attachment[0].attachments[WPN_ATTM_MUZZLE] = attachmentDB_muzzle_idList[selectedPrimaryWeapon_Muzzle];
		loadout.Attachment[0].attachments[WPN_ATTM_UPPER_RAIL] = attachmentDB_upperRail_idList[selectedPrimaryWeapon_UpperRail];
		loadout.Attachment[0].attachments[WPN_ATTM_LEFT_RAIL] = attachmentDB_leftRail_idList[selectedPrimaryWeapon_LeftRail];
		loadout.Attachment[0].attachments[WPN_ATTM_BOTTOM_RAIL] = attachmentDB_bottomRail_idList[selectedPrimaryWeapon_BottomRail];
		loadout.Attachment[0].attachments[WPN_ATTM_CLIP] = attachmentDB_clip_idList[selectedPrimaryWeapon_Clip];

		loadout.Attachment[1].attachments[WPN_ATTM_MUZZLE] = attachmentDB_muzzle_idList[selectedSecondaryWeapon_Muzzle];
		loadout.Attachment[1].attachments[WPN_ATTM_UPPER_RAIL] = attachmentDB_upperRail_idList[selectedSecondaryWeapon_UpperRail];
		loadout.Attachment[1].attachments[WPN_ATTM_LEFT_RAIL] = attachmentDB_leftRail_idList[selectedSecondaryWeapon_LeftRail];
		loadout.Attachment[1].attachments[WPN_ATTM_BOTTOM_RAIL] = attachmentDB_bottomRail_idList[selectedSecondaryWeapon_BottomRail];
		loadout.Attachment[1].attachments[WPN_ATTM_CLIP] = attachmentDB_clip_idList[selectedSecondaryWeapon_Clip];

		if(pl->CurLoadout.Items[0].itemID != loadout.Items[0].itemID ||
		   pl->CurLoadout.Items[1].itemID != loadout.Items[1].itemID ||
		   pl->CurLoadout.Items[6].itemID != loadout.Items[6].itemID ||
		   pl->CurLoadout.Items[7].itemID != loadout.Items[7].itemID ||
		   pl->CurLoadout.HeroItemID != loadout.HeroItemID ||
		   pl->CurLoadout.HeadIdx != loadout.HeadIdx ||
		   pl->CurLoadout.BodyIdx != loadout.BodyIdx ||
		   pl->CurLoadout.LegsIdx != loadout.LegsIdx ||
		   pl->CurLoadout.Attachment[0] != loadout.Attachment[0] || 
		   pl->CurLoadout.Attachment[1] != loadout.Attachment[1]
		)
		{
			pl->UpdateLoadoutSlot(loadout, -1);
		}
	}

	delete [] weaponDB_idList;
	delete [] attachmentDB_muzzle_idList;
	delete [] attachmentDB_upperRail_idList;
	delete [] attachmentDB_leftRail_idList;
	delete [] attachmentDB_bottomRail_idList;
	delete [] attachmentDB_clip_idList;
	delete [] armorDBids;
	delete [] bodyDBids;
	delete [] legsDBids;
	delete [] headDBids;
	delete [] headarmorDBids;
}

//------------------------------------------------------------------------

static void AdjustHitPosition( DecalParams& params, GameObject* target )
{
	R3DPROFILE_FUNCTION( "AdjustHitPosition" );
	if( target && target->isObjType( OBJTYPE_Mesh ) )
	{
		MeshGameObject* mobj = static_cast< MeshGameObject* >(target);

		float interSearchLength = mobj->GetObjectsRadius() * 0.33f;

		float dist;

		r3dMaterial* mtl;
		int minFace;

		r3dPoint3D start = params.Pos + params.Dir * interSearchLength * 0.5f;

		if( mobj->MeshLOD[ 0 ]->ContainsRay( start, -params.Dir, interSearchLength, &dist, &mtl, mobj->GetPosition(), mobj->GetRotationMatrix(), &minFace ) )
		{
			params.Pos = start - params.Dir * dist;
		}
	}
}

//------------------------------------------------------------------------

bool ProcessBulletHit( int &damageFromPiercable, GameObject* owner, const r3dPoint3D &hitPoint, const r3dPoint3D &hitNormal, GameObject* shootTarget, const r3dMaterial* shootMaterial, const char* hitActorName, const WeaponConfig *weaponInfo, const r3dPoint3D& muzzlerPosAtFireStart, gobjid_t bulletObjID) 
{	
	r3d_assert(owner);
	r3d_assert(owner->NetworkLocal==true);
	
	obj_Player* ownerPlayer = NULL;
	if(owner->isObjType(OBJTYPE_Human))
		ownerPlayer = (obj_Player*)owner;
	
	if( shootTarget == NULL && shootMaterial == NULL) // hit nothing
	{
		// note: must be synced with melee weapon degradation PKT_C2C_PlayerHitNothing_s
		PKT_C2C_PlayerHitNothing_s n;
		n.localId = bulletObjID;
		p2pSendToHost(owner, &n, sizeof(n), true);
		
	} 
	else if(shootTarget && shootTarget->isObjType(OBJTYPE_Human)) // hit other player
	{
		r3d_assert(shootTarget->GetNetworkID());
		obj_Player* trgt = (obj_Player*)shootTarget;

		if(!trgt->bDead)
		{
			PKT_C2C_PlayerHitDynamic_s n;
			n.localId = bulletObjID;
			n.muzzler_pos = muzzlerPosAtFireStart;
			n.hit_pos = hitPoint;
			n.targetId = toP2pNetId(shootTarget->GetNetworkID());
			n.hit_body_part = trgt->getBodyNameFromBoneName(hitActorName);
			n.hit_body_bone = trgt->uberAnim_->GetBoneID(hitActorName);
			n.damageFromPiercing = damageFromPiercable;
			// NOTE: This can be broken if the player jumps after firing a sniper rifle, but shouldn't happen as it's too fast.
			n.state = 0;
			if(ownerPlayer)
				n.state |= (!ownerPlayer->bOnGround)?(0x1): 0 ; // [0]-in air 
			n.state |= (!trgt->bOnGround)?0x2:0 ; // [1]-in air
			p2pSendToHost( owner, &n, sizeof(n), true);

			if(!trgt->bDead && ownerPlayer) 
			{
				ownerPlayer->m_HitMarkerFadeout = 1.0f;
			}
		}
		else
		{
			// note: must be synced with melee weapon degradation PKT_C2C_PlayerHitNothing_s
			PKT_C2C_PlayerHitNothing_s n;
			n.localId = bulletObjID;
			p2pSendToHost(owner, &n, sizeof(n), true);
		}

		if(ownerPlayer)
		{
			{
				ownerPlayer->AddBloodOnGround(hitPoint);
				SpawnImpactParticle(r3dHash::MakeHash("player"),r3dHash::MakeHash(weaponInfo->m_PrimaryAmmo->getDecalSource()), hitPoint, hitNormal);
			}
		}
	}
	else if(shootTarget && shootTarget->isObjType(OBJTYPE_Zombie)) // hit zombie
	{
		r3d_assert(shootTarget->GetNetworkID());

		PKT_C2C_PlayerHitDynamic_s n;
		n.localId = bulletObjID;
		n.muzzler_pos = muzzlerPosAtFireStart;
		n.hit_pos  = hitPoint;
		n.targetId = toP2pNetId(shootTarget->GetNetworkID());
		n.hit_body_part = strcmp("Bip01_Head", hitActorName) == 0 ? 1 : 0;
		if(ownerPlayer)
			n.hit_body_bone = ownerPlayer->uberAnim_->GetBoneID(hitActorName); // zombies use same skeleton as player
		else
			n.hit_body_bone = 0xFF;

		n.damageFromPiercing = damageFromPiercable;
		n.state = 0;

		p2pSendToHost(owner, &n, sizeof(n), true);

		if(owner)
		{
			obj_Zombie* z = (obj_Zombie*)shootTarget;

			// apply ragdoll force from object position, or up if too close
			r3dPoint3D zragHitPos = z->GetPosition();
			r3dPoint3D zlastTimeHitForce = r3dPoint3D(0, 1, 0);
			if(ownerPlayer)
			{
				if(n.hit_body_bone < ownerPlayer->GetSkeleton()->NumBones) // zombies use same skeleton as player
					zragHitPos = ownerPlayer->uberEquip_->getBonePos(n.hit_body_bone, ownerPlayer->GetSkeleton(), z->GetTransformMatrix());
				if((zragHitPos - ownerPlayer->GetPosition()).LengthSq() > 0.001f)
					zlastTimeHitForce = (zragHitPos - ownerPlayer->GetPosition()).NormalizeTo();
			}

			z->lastTimeHitPos  = zlastTimeHitForce;//ownerPlayer->GetPosition();
			z->lastTimeDmgType = weaponInfo->category;
			z->lastTimeHitBoneID = n.hit_body_bone;
			z->PlayHurtSound();
		}
		if(ownerPlayer)
		{
			{
				ownerPlayer->AddBloodOnGround(hitPoint);
				SpawnImpactParticle(r3dHash::MakeHash("zombie"),r3dHash::MakeHash(weaponInfo->m_PrimaryAmmo->getDecalSource()), hitPoint, hitNormal);
			}
		}
	}
#ifdef VEHICLES_ENABLED
	else if (shootTarget && shootTarget->isObjType(OBJTYPE_Vehicle))
	{
		PKT_C2C_PlayerHitDynamic_s n;
		n.localId = bulletObjID;
		n.muzzler_pos = muzzlerPosAtFireStart;
		n.hit_pos  = hitPoint;
		n.targetId = toP2pNetId(shootTarget->GetNetworkID());
		n.hit_body_part = 0;
		n.hit_body_bone = 0xFF;
		n.damageFromPiercing = damageFromPiercable;
		n.state = 0;

		p2pSendToHost(owner, &n, sizeof(n), true);

		SpawnImpactParticle(r3dHash::MakeHash("Metal"),r3dHash::MakeHash(weaponInfo->m_PrimaryAmmo->getDecalSource()), hitPoint, hitNormal);
	}
#endif

	else if (shootTarget && shootTarget->isObjType(OBJTYPE_UAV))
	{
		PKT_C2C_PlayerHitDynamic_s n;
		n.localId = bulletObjID;
		n.muzzler_pos = muzzlerPosAtFireStart;
		n.hit_pos  = hitPoint;
		n.targetId = toP2pNetId(shootTarget->GetNetworkID());
		n.hit_body_part = 0;
		n.hit_body_bone = 0xFF;
		n.damageFromPiercing = damageFromPiercable;
		n.state = 0;

		p2pSendToHost(owner, &n, sizeof(n), true);

		SpawnImpactParticle(r3dHash::MakeHash("Metal"),r3dHash::MakeHash(weaponInfo->m_PrimaryAmmo->getDecalSource()), hitPoint, hitNormal);
	}

#ifndef FINAL_BUILD
	else if(shootTarget && shootTarget->Class->Name == "obj_ZombieDummy")
	{
		obj_ZombieDummy* trgt = (obj_ZombieDummy*)shootTarget;

		r3dPoint3D hitForce = (hitPoint - ownerPlayer->GetPosition()).NormalizeTo();
		int hit_body_bone = trgt->anim_.GetCurrentSkeleton()->GetBoneID(hitActorName);
		
		if(strcmp("Bip01_Head", hitActorName) == 0)
		{
			r3dPoint3D hitForce_amp = hitForce * 30;
			trgt->physSkeleton->SwitchToRagdollWithForce(true, hit_body_bone, &hitForce_amp);
		}
		else
		{
			trgt->DoHit();
		}

		if(ownerPlayer)
		{
			ownerPlayer->AddBloodOnGround(hitPoint);
			SpawnImpactParticle(r3dHash::MakeHash("player"),r3dHash::MakeHash(weaponInfo->m_PrimaryAmmo->getDecalSource()), hitPoint, hitNormal);
		}
	}
	else if(shootTarget && shootTarget->Class->Name == "obj_AnimalDummy")
	{
		obj_AnimalDummy* trgt = (obj_AnimalDummy*)shootTarget;

		//r3dPoint3D hitForce = (hitPoint - ownerPlayer->GetPosition()).NormalizeTo();
		//int hit_body_bone = trgt->anim_.GetCurrentSkeleton()->GetBoneID(hitActorName);
		
		/*if(strcmp("Bip01_Head", hitActorName) == 0)
		{
			r3dPoint3D hitForce_amp = hitForce * 30;
			trgt->physSkeleton->SwitchToRagdollWithForce(true, hit_body_bone, &hitForce_amp);
		}
		else
		{*/
			trgt->DoHit();
		//}

		if(ownerPlayer)
		{
			ownerPlayer->AddBloodOnGround(hitPoint);
			SpawnImpactParticle(r3dHash::MakeHash("player"),r3dHash::MakeHash(weaponInfo->m_PrimaryAmmo->getDecalSource()), hitPoint, hitNormal);
		}
	}
#endif	
	else if(shootTarget && shootTarget->GetNetworkID() && canDamageTarget(shootTarget))
	{
		// hit something that have network Id
		r3d_assert(shootTarget->GetNetworkID());
		PKT_C2C_PlayerHitDynamic_s n;
		n.localId = bulletObjID;
		n.muzzler_pos = muzzlerPosAtFireStart;		
		n.hit_pos = hitPoint;
		n.targetId = toP2pNetId(shootTarget->GetNetworkID());
		n.hit_body_bone = 0;
		n.hit_body_part = 0;
		n.state = 0;
		n.damageFromPiercing = damageFromPiercable; 
		p2pSendToHost( owner, &n, sizeof(n), true);

		//TODO: add decals
		SpawnImpactParticle(r3dHash::MakeHash("Metal"),r3dHash::MakeHash(weaponInfo->m_PrimaryAmmo->getDecalSource()), hitPoint, hitNormal);
		
	} 
	else 
	{
		DecalParams params;
		params.Dir	= hitNormal;
		params.Pos	= hitPoint;
		params.TypeID	= INVALID_DECAL_ID;

		AdjustHitPosition( params, shootTarget );

		uint32_t sourceHash = r3dHash::MakeHash( weaponInfo->m_PrimaryAmmo->getDecalSource());
		uint32_t matHash    = 0;
		int      particleIdx= -1;

		r3dPoint3D waterSplashPos;
		extern bool TraceWater(const r3dPoint3D& start, const r3dPoint3D& finish, r3dPoint3D& waterSplashPos);
		if( TraceWater( r3dPoint3D(gCam.x, gCam.y, gCam.z), hitPoint, waterSplashPos))
		{	
			matHash = r3dHash::MakeHash("Thirst");
			particleIdx = SpawnImpactParticle(matHash, sourceHash, waterSplashPos, r3dPoint3D(0,1,0));
			extern void WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx);
			waterSplashPos.y -= 0.1f;
			WaterSplash(waterSplashPos, 0.1f, 30.0f, 0.04f, 0);
		}
		else
		{
			{
				if( shootTarget && shootTarget->isObjType(OBJTYPE_Terrain) )
				{
					params.TypeID	= Terrain->GetDecalID(params.Pos, sourceHash);
					particleIdx = SpawnImpactParticle(matHash, sourceHash, hitPoint, hitNormal);
				}
				else
				{
					if( shootMaterial )
					{
						matHash = r3dHash::MakeHash(shootMaterial->TypeName);
						params.TypeID	= GetDecalID(matHash, sourceHash);
						particleIdx = SpawnImpactParticle(matHash, sourceHash, hitPoint, hitNormal);
					}
				}

				if( params.TypeID != INVALID_DECAL_ID )
				{
					g_pDecalChief->Add( params );
				}

				// add impulse
				if(shootTarget && shootTarget->PhysicsObject)
				{
					shootTarget->PhysicsObject->AddImpulseAtPos(-hitNormal*weaponInfo->m_AmmoSpeed*weaponInfo->m_AmmoMass/150.0f, hitPoint);
				}
			}
		}
		int hitPierceableObject = 0;
		if(shootTarget && shootTarget->m_BulletPierceable > 0  && damageFromPiercable == 0 && weaponInfo->category!=storecat_MELEE ) // melee cannot pierce objects
		{
			hitPierceableObject = shootTarget->m_BulletPierceable;
		}

		// check if we hit collectible resource item
		if(shootMaterial && (weaponInfo->m_itemID == WeaponConfig::ITEMID_ResourceCollectionTool || weaponInfo->m_itemID == WeaponConfig::ITEMID_ResourceCollectionToolPremium))
		{
			matHash = r3dHash::MakeHash(shootMaterial->TypeName);
			const MaterialType* matType = g_pMaterialTypes->Get(matHash);
			if(matType && matType->hasResourcesToCollect)
			{
				// send request to server to collect resources based on matType->hasResourcesToCollect
				PKT_C2C_PlayerHitResource_s n;
				n.ResType = (BYTE)matType->hasResourcesToCollect;
				p2pSendToHost(owner, &n, sizeof(n), true);
			}
		}

		if(hitPierceableObject)
		{
			PKT_C2C_PlayerHitStaticPierced_s n; 
			n.localId = bulletObjID;
			n.hit_pos  = hitPoint;
			n.hit_norm = hitNormal;
			n.decalIdx = (BYTE)(params.TypeID + 1);  // -1 is invalid decal, bring to 0-xx range
			n.particleIdx = (BYTE)(particleIdx + 1); // same here
			n.hash_obj = shootTarget?shootTarget->GetHashID():0;
			p2pSendToHost( owner, &n, sizeof(n));

			damageFromPiercable = shootTarget->m_BulletPierceable;

			return false;

		}
		else
		{
			PKT_C2C_PlayerHitStatic_s n;
			n.localId = bulletObjID;
			n.hit_pos  = hitPoint;
			n.hit_norm = hitNormal;
			n.decalIdx = (BYTE)(params.TypeID + 1);  // -1 is invalid decal, bring to 0-xx range
			n.particleIdx = (BYTE)(particleIdx + 1); // same here
			n.hash_obj = shootTarget?shootTarget->GetHashID():0;
			p2pSendToHost( owner, &n, sizeof(n));
		}
	}
	
	return true;
}

void AI_PlayerManufacture()
{
#ifndef FINAL_BUILD
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if( obj->ObjTypeFlags & OBJTYPE_Human )
		{
			obj_Player* player = static_cast< obj_Player* >( obj );

			if( player->NetworkLocal )
			{
				if( !player->Permanufactura )
				{
					player->Permanufactura = 1;
					player->Permanufactura_Counter = 0;
				}
				else
				{
					player->Permanufactura = 0;
				}
			}
		}
	}
#endif
}

void obj_Player::setVoiceActive(bool set)
{
	if(m_isVoiceActive != set)
	{
		if(hudMain)
		{
			char tmpStr[128];
			GetUserName(tmpStr);
			if(m_isVoiceActive)
				hudMain->removePlayerFromVoipList(tmpStr);
			else
				hudMain->addPlayerToVoipList(tmpStr);
		}
		m_isVoiceActive = set;
	}
}

void RebuildUberAnimOnObjAIPlayer()
{
	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* pObj = iter.current;

		if( pObj->isObjType( OBJTYPE_Human ) )
		{
			obj_Player* player = static_cast< obj_Player* > ( pObj );

			player->EDITOR_RecreateUberAnim();
		}
	}
}