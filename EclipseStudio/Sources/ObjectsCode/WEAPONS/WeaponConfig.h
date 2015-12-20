#pragma once

#include "BaseItemConfig.h"

enum WeaponAnimTypeEnum
{
	WPN_ANIM_ASSAULT,
	WPN_ANIM_PISTOL,
	WPN_ANIM_GRENADE,
	WPN_ANIM_RPG,
	WPN_ANIM_SMG,
	WPN_ANIM_MINE,
	WPN_ANIM_MELEE,

	WPN_ANIM_COUNT
};

enum WeaponFiremodeEnum
{
	WPN_FRM_SINGLE=1<<0,
	WPN_FRM_TRIPLE=1<<1,
	WPN_FRM_AUTO=1<<2,
};

#include "../../../ServerNetPackets/NetPacketsWeaponInfo.h"

extern const char* WeaponAttachmentBoneNames[WPN_ATTM_MAX];

class WeaponAttachmentConfig : public ModelItemConfig
{
private:
	mutable r3dMesh* m_Model_AIM; // second model for when you are aiming. enabled only for scopes right now
public:
	// config
	WeaponAttachmentTypeEnum m_type;
	int m_specID;

	char* m_MuzzleParticle;

	char* m_ScopeAnimPath;

	// mods
	float	m_Damage;
	float	m_Range;
	int		m_Firerate;
	float	m_Recoil;
	float	m_Spread;
	int		m_Clipsize;
	const struct ScopeConfig* m_scopeConfig;
	const struct ScopeConfig* m_scopeConfigTPS; // spec.config for when you are switching to TPS mode
	float	m_scopeZoom; // 0..1; 0 - no zoom. 1 - maximum zoom

	// new weapon sounds IDs
	int		m_sndFireID_single;
	int		m_sndFireID_auto;
	int		m_sndFireID_single_player; // for local player
	int		m_sndFireID_auto_player; // for local player

	// durability & repairing
	float	m_DurabilityUse;	// per use, in %
	float	m_RepairAmount;
	float	m_PremRepairAmount;
	int	m_RepairPriceGD;

public:
	WeaponAttachmentConfig(uint32_t id) : ModelItemConfig(id)
	{
		category = storecat_FPSAttachment;
		m_type = WPN_ATTM_INVALID;
		m_specID = 0;
		m_Model_AIM = NULL;
		m_MuzzleParticle = NULL;

		m_ScopeAnimPath = NULL;

		m_Damage = 0.0f;
		m_Range = 0.0f;
		m_Firerate = 0;
		m_Recoil = 0.0f;
		m_Spread = 0.0f;
		m_Clipsize = 0;
		m_scopeConfig = NULL;
		m_scopeConfigTPS = NULL;
		m_scopeZoom= 1.0f; 

		m_sndFireID_single = -1;
		m_sndFireID_auto   = -1;
		m_sndFireID_single_player = -1;
		m_sndFireID_auto_player = -1;

		m_DurabilityUse = 0.0f;
		m_RepairAmount = 0.0f;
		m_PremRepairAmount = 0.0f;
		m_RepairPriceGD = 0;

	}
	virtual ~WeaponAttachmentConfig() 
	{
		free(m_MuzzleParticle);
		free(m_ScopeAnimPath);
	}
	virtual bool loadBaseFromXml(pugi::xml_node& xmlAttachment);

	void copyParametersTo(GBAttmInfo& wi) const
	{
#define DO(XX) wi.XX = XX
		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
#undef DO
	}
	void copyParametersFrom(const GBAttmInfo& wi)
	{
#define DO(XX) XX = wi.XX
		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
#undef DO
	}

	r3dMesh* getMesh( bool allow_async_loading, bool aim_model ) const;

	int getAimMeshRefs() const ;

	// called when unloading level
	virtual void resetMesh() { ModelItemConfig::resetMesh(); m_Model_AIM = 0; }
};

class WeaponConfig : public ModelItemConfig
{
private:
	mutable r3dMesh* m_Model_FPS;
	mutable int	m_ModelRefCount ;

public:

	mutable r3dSkeleton* m_Model_FPS_Skeleton;
	mutable r3dAnimPool*  m_AnimPool_FPS;
	
	// PTUMIK: if adding new skill based items, make sure to add them to DB FN_VALIDATE_LOADOUT proc and also to CUserProfile::isValidInventoryItem()
	enum EUsableItemIDs
	{
		ITEMID_Antibiotics  = 101256, 
		ITEMID_Bandages  = 101261,
		ITEMID_Bandages2 = 101262,
		ITEMID_CraftedBandages = 101405,
		ITEMID_Painkillers = 101300,
		ITEMID_Medkit = 101304,

		ITEMID_ZombieRepellent = 101301,
		ITEMID_C01Vaccine = 101302,
		ITEMID_C04Vaccine = 101303,
		
		ITEMID_PieceOfPaper = 101305,	// used to create world notes
		ITEMID_AirHorn = 101323,
		ITEMID_Binoculars = 101315,
		ITEMID_RangeFinder = 101319,
		ITEMID_Lockbox = 101348,
		ITEMID_UnarmedMelee = 101398,

		ITEMID_BarbWireBarricade = 101316,
		ITEMID_WoodShieldBarricade = 101317,
		ITEMID_RiotShieldBarricade = 101318,
		ITEMID_SandbagBarricade = 101324,
		ITEMID_RiotShieldBarricadeZB = 101416, // zombie barricade version
		ITEMID_WoodShieldBarricadeZB = 101417,
		
		ITEMID_RepairKit = 101399,
		ITEMID_PremRepairKit = 101400,

		// Grenade items
		ITEMID_FragGrenade = 101310,
		ITEMID_ChemLight = 101311,
		ITEMID_Flare = 101312,
		ITEMID_FlareGun = 101320,
		ITEMID_ChemLightBlue = 101325,
		ITEMID_ChemLightGreen = 101326,
		ITEMID_ChemLightOrange = 101327,
		ITEMID_ChemLightRed = 101328,
		ITEMID_ChemLightYellow = 101329,
		ITEMID_SharpnelBomb = 101403,
		ITEMID_SharpnelTripWireBomb = 101404,

		// Resource collection tool (melee weapon that allows to collect resources)
		ITEMID_ResourceCollectionTool = 101408,
		ITEMID_ResourceCollectionToolPremium = 101409,

		// Building blocks and stuff like that
		ITEMID_WoodenDoorBlock = 101352, // acts like barricade
		ITEMID_MetalWallBlock = 101353, // acts like barricade
		ITEMID_TallBrickWallBlock = 101354, // barricade
		ITEMID_WoodenWallPiece = 101355, // baricade
		ITEMID_ShortBrickWallPiece = 101356, // barricade

		ITEMID_FarmBlock = 101357,
		ITEMID_SolarWaterPurifier = 101360,
		ITEMID_PlaceableLight = 101361,
		ITEMID_SmallPowerGenerator = 101359,
		ITEMID_BigPowerGenerator = 101358,

		ITEMID_Gas = 301321,
		ITEMID_EmptyJerryCan = 301325,

		ITEMID_TankShell = 400160,

		ITEMID_ConstructorBlockSmall = 103000,
		ITEMID_ConstructorBlockBig = 103001,
		ITEMID_ConstructorBlockCircle = 103002,
		ITEMID_ConstructorColum1 = 103003,
		ITEMID_ConstructorColum2 = 103004,
		ITEMID_ConstructorColum3 = 103005,
		ITEMID_ConstructorFloor1 = 103006,
		ITEMID_ConstructorFloor2 = 103007,
		ITEMID_ConstructorCeiling1 = 103008,
		ITEMID_ConstructorCeiling2 = 103009,
		ITEMID_ConstructorCeiling3 = 103010,
		ITEMID_ConstructorWallMetalic = 103011,
		ITEMID_ConstructorSlope = 103012,
		ITEMID_ConstructorWall1 = 103013,
		ITEMID_ConstructorWall2 = 103014,
		ITEMID_ConstructorWall3 = 103015,
		ITEMID_ConstructorWall4 = 103016,
		ITEMID_ConstructorWall5 = 103017,
		ITEMID_ConstructorWODBOX = 103018,
		ITEMID_ConstructorBaseBunker = 103019,
		ITEMID_AerialDelivery = 103020,

		ITEMID_Traps_Bear = 103021,
		ITEMID_Traps_Spikes = 103022,
		ITEMID_GardenTrap_Rabbit = 103023,
		ITEMID_Campfire = 103024,
		ITEMID_Cypher2 = 103025,

	};

	char* m_MuzzleParticle;

	char* FNAME;

	int	  m_isConsumable; // for usableitems
	int	  m_ShopStackSize;

	char* m_ModelPath_1st;
	char* m_AnimPrefix; // used for FPS mode, for grips


	class Ammo*	m_PrimaryAmmo;

	float m_AmmoMass;
	float m_AmmoSpeed;
	float m_AmmoDamage;
	float m_AmmoDecay;
	float m_AmmoArea;
	float m_AmmoDelay;
	float m_AmmoTimeout;
	bool  m_AmmoImmediate;

	float m_reloadActiveTick; // when active reload becomes available. Duration is not longer than 10% of reload time or 0.4sec maximum; for grenades used as a time when grenade should be launched from a hand
	r3dSec_type<float, 0xAB434A6C> m_spread;  // spread set as diameter at 50meter range

	WeaponAnimTypeEnum m_AnimType;
	r3dSec_type<float, 0xC2A9FAC3> m_recoil;
	uint32_t m_fireModeAvailable; // flags
	r3dSec_type<float, 0xAF1DA2F9> m_fireDelay;
	const struct ScopeConfig* m_scopeConfig;
	float						m_scopeZoom; // 0..1; 0 - no zoom. 1 - maximum zoom
	r3dSec_type<float, 0xAC56F451> m_reloadTime;

	mutable r3dPoint3D	muzzlerOffset;
	mutable bool		muzzleOffsetDetected;

	r3dPoint3D adjMuzzlerOffset; // used privately, do not use it
	mutable r3dPoint3D shellOffset; // where shells are coming out

	int		m_sndReloadID;

	int		m_sndFireID_single;
	int		m_sndFireID_auto;
	int		m_sndFireID_single_player; // for local player
	int		m_sndFireID_auto_player; // for local player
	
	int*		m_animationIds;
	int*		m_animationIds_FPS;
	
	// fps item attachments
	int		IsFPS;
	int		FPSSpecID[WPN_ATTM_MAX];	// m_specID in WeaponAttachmentConfig for each attachment slot
	int		FPSDefaultID[WPN_ATTM_MAX];	// default attachment item ids in each slot

	// durability & repairing
	float	m_DurabilityUse;	// per use, in %
	float	m_RepairAmount;
	float	m_PremRepairAmount;
	int	m_RepairPriceGD;

  private:	
	// make copy constructor and assignment operator inaccessible
	WeaponConfig(const WeaponConfig& rhs);
	WeaponConfig& operator=(const WeaponConfig& rhs);

  public:
	WeaponConfig(uint32_t id) : ModelItemConfig(id)
	{
		m_ModelRefCount = 0 ;

		muzzleOffsetDetected = false ;

		m_Model_FPS = NULL;
		m_Model_FPS_Skeleton = NULL;
		m_AnimPool_FPS = NULL;
		m_MuzzleParticle = NULL;
		FNAME = NULL;
		m_ModelPath_1st = NULL;
		m_PrimaryAmmo = NULL;
		m_scopeConfig = NULL;

		m_AnimPrefix = NULL;

		m_isConsumable = 0;
		m_ShopStackSize = 1;

		m_AmmoMass			= 0.1f;
		m_AmmoSpeed			= 100.f;
		m_AmmoDamage		= 1.0f;
		m_AmmoDecay			= 0.1f;
		m_AmmoArea			= 0.1f;
		m_AmmoDelay			= 0.f;
		m_AmmoTimeout		= 1.0f;
		m_AmmoImmediate		= true;

		m_reloadTime		= 1.0f;
		m_reloadActiveTick	= 0.f; // when active reload becomes available. Duration is not longer than 10% of reload time or 0.4sec maximum; for grenades used as a time when grenade should be launched from a hand
		m_fireDelay			= 0.5f;
		m_spread			= 0.01f; 
		m_recoil			= 0.1f;
		m_AnimType			= WPN_ANIM_ASSAULT;
		m_fireModeAvailable	= WPN_FRM_SINGLE; // flags
		m_scopeZoom			= 0; // 0..1; 0 - no zoom. 1 - maximum zoom

		muzzlerOffset		= r3dPoint3D( 0.25f, 0.f, 0.f );
		adjMuzzlerOffset	= muzzlerOffset ; // used privately, do not use it
		shellOffset			= r3dPoint3D( 0, 0, 0 ); // where shells are coming out

		m_sndReloadID		= -1 ;

		m_sndFireID_single = -1;
		m_sndFireID_auto   = -1;
		m_sndFireID_single_player = -1;
		m_sndFireID_auto_player = -1;
		
		m_animationIds          = NULL;
		m_animationIds_FPS		= NULL;
		
		IsFPS = 0;
		memset(FPSSpecID, 0, sizeof(FPSSpecID));
		memset(FPSDefaultID, 0, sizeof(FPSDefaultID));

		m_DurabilityUse = 0.0f;
		m_RepairAmount = 0.0f;
		m_PremRepairAmount = 0.0f;
		m_RepairPriceGD = 0;
	}
	virtual ~WeaponConfig() 
	{
		free(m_MuzzleParticle);
		free(FNAME);
		free(m_ModelPath_1st);
		free(m_AnimPrefix);
		SAFE_DELETE_ARRAY(m_animationIds);
		SAFE_DELETE_ARRAY(m_animationIds_FPS);
	}
	virtual bool loadBaseFromXml(pugi::xml_node& xmlWeapon);

	bool isAttachmentValid(const WeaponAttachmentConfig* wac) const;
	
	__forceinline void copyParametersTo(GBWeaponInfo& wi) const
	{
		#define DO(XX) wi.XX = XX
		DO(m_AmmoMass);
		DO(m_AmmoSpeed);
		DO(m_AmmoDamage);
		DO(m_AmmoDecay);
		DO(m_AmmoArea);
		DO(m_AmmoDelay);
		DO(m_AmmoTimeout);

		DO(m_reloadTime);
		DO(m_reloadActiveTick);
		DO(m_fireDelay);
		DO(m_spread); 
		DO(m_recoil);

		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
		#undef DO
	}
	__forceinline void copyParametersFrom(const GBWeaponInfo& wi)
	{
		#define DO(XX) XX = wi.XX
		DO(m_AmmoMass);
		DO(m_AmmoSpeed);
		DO(m_AmmoDamage);
		DO(m_AmmoDecay);
		DO(m_AmmoArea);
		DO(m_AmmoDelay);
		DO(m_AmmoTimeout);

		DO(m_reloadTime);
		DO(m_reloadActiveTick);
		DO(m_fireDelay);
		DO(m_spread); 
		DO(m_recoil);

		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
		#undef DO
	}

	__forceinline DWORD GetClientParametersHash() const
	{
		VMPROTECT_BeginVirtualization("GetClientParametersHash");
		// hold copy of variables to hash, work with r3dSecType
#pragma pack(push,1)
		struct hash_s 
		{
			float m_spread;
			float m_fireDelay;
			float m_recoil;
			float m_AmmoSpeed;
			float m_reloadTime;
		};
#pragma pack(pop)

		hash_s h;
		h.m_reloadTime = m_reloadTime;
		h.m_fireDelay  = m_fireDelay;
		h.m_spread     = m_spread;
		h.m_recoil     = m_recoil;
		h.m_AmmoSpeed  = m_AmmoSpeed;
		DWORD crc32 = r3dCRC32((BYTE*)&h, sizeof(h));
		VMPROTECT_End();
		return crc32;
	}

	r3dMesh* getMesh( bool allow_async_loading, bool first_person ) const;

	int getConfigMeshRefs() const ;

	// called when unloading level
	virtual void resetMesh() { 
		ModelItemConfig::resetMesh();
		m_Model_FPS = 0; 
		SAFE_DELETE(m_Model_FPS_Skeleton); 
		SAFE_DELETE(m_AnimPool_FPS); 
	}
	void detectMuzzleOffset(bool first_person) const;

	// because mesh can be delay-loaded
	void updateMuzzleOffset(bool first_person) const ;

	r3dSkeleton* getSkeleton() const { return m_Model_FPS_Skeleton; }

	void aquireMesh( bool allow_async_loading ) const ;
	void releaseMesh() const ;

	R3D_FORCEINLINE bool hasFPSModel() const
	{
		return m_Model_FPS ? true : false ;
	}

	R3D_FORCEINLINE bool isFPSModelSkeletal() const
	{
		return m_Model_FPS->IsSkeletal() ? true : false ;
	}
};
