#pragma once

#ifdef WO_SERVER
#include "..\..\..\..\server\src\WO_GameServer\Sources\ObjectsCode\Missions\MissionProgress.h"
#endif

/// STATS TRACKING ///
struct wiStatsTracking
{
// NOTE: you MUST increase P2PNET_VERSION if you change this structure
	int RewardID;

	int GD; // game dollars
	int GP; // game points
	int XP; // XP
	wiStatsTracking& operator+=(const wiStatsTracking& rhs) 
	{ 
		XP+=rhs.XP;
		GP+=rhs.GP; 
		GD+=rhs.GD;
		return *this;
	}
	wiStatsTracking() { memset(this, 0, sizeof(*this)); }
	wiStatsTracking(int rewardId, int gd, int gp, int xp)
		: RewardID(rewardId), XP(xp), GP(gp), GD(gd) {}
};

// Items categories
enum STORE_CATEGORIES
{
	storecat_INVALID	= 0,
	storecat_Account	= 1,
	storecat_Boost		= 2,
	storecat_LootBox	= 7,

	storecat_Armor		= 11,
	storecat_Backpack	= 12,	
	storecat_Helmet		= 13,
	storecat_HeroPackage	= 16,

	storecat_FPSAttachment  = 19,

	storecat_ASR		= 20,	// Assault Rifles
	storecat_SNP		= 21,	// Sniper rifles
	storecat_SHTG		= 22,	// Shotguns
	storecat_MG			= 23,	// Machine guns
	storecat_HG			= 25,	// handguns
	storecat_SMG		= 26,	// submachineguns
	storecat_GRENADE	= 27,	// grenades and everything that you can throw. Mines shouldn't be in this group!!!
	storecat_UsableItem  = 28,	// usable items
	storecat_MELEE		 =29,   // melee items (knifes, etc)
	storecat_Food		= 30,	// food 
	storecat_Water		= 33,	// water
	
	storecat_Components = 50,	// components for crafting (not usable items)
	storecat_CraftRecipe= 51,

	storecat_Vehicle	= 55,  // vehicle info

	storecat_NUM_ITEMS, // should be the last one!!
};

class BaseItemConfig;
extern bool storecat_IsItemStackable(uint32_t ItemID);
extern bool storecat_CanPlaceItemToSlot(const BaseItemConfig* itemCfg, int idx);
extern int  storecat_GetItemBuyStackSize(uint32_t ItemID);

struct wiStoreItem
{
	uint32_t itemID;

	uint32_t pricePerm;
	uint32_t gd_pricePerm;
	
	bool	isNew;

	bool hasAnyPrice()
	{
		return pricePerm > 0 || gd_pricePerm > 0;
	}
};

static const uint32_t MAX_NUM_STORE_ITEMS = 10000; // fixed at 10000 for now
extern wiStoreItem g_StoreItems[MAX_NUM_STORE_ITEMS]; 
extern uint32_t g_NumStoreItems;

struct wiStats
{
	// character stats
	int		XP;
	int		SpendXP;
	int		TimePlayed;
	int		Reputation;

	// generic trackable stats
	int		KilledZombies;	// normal zombie kills
	int		KilledSurvivors;
	int		KilledBandits;
	int		VictorysHardGames;//gamehardcore

	// those stats are saved in CharData
	uint32_t		Deaths;
	uint32_t		ShotsFired;
	uint32_t		ShotsHits;
	uint32_t		ShotsHeadshots;		

	wiStats()
	{
		memset(this, 0, sizeof(*this));
	}
};

enum WeaponAttachmentTypeEnum
{
	WPN_ATTM_INVALID=-1,
	WPN_ATTM_MUZZLE=0,
	WPN_ATTM_UPPER_RAIL=1,
	WPN_ATTM_LEFT_RAIL=2,
	WPN_ATTM_BOTTOM_RAIL=3,
	WPN_ATTM_CLIP=4,
	WPN_ATTM_RECEIVER=5, // not visual
	WPN_ATTM_STOCK=6, // not visual
	WPN_ATTM_BARREL=7, // not visual
	WPN_ATTM_MAX
};

struct wiWeaponAttachment
{
	uint32_t attachments[WPN_ATTM_MAX];
	wiWeaponAttachment() { Reset(); }
	void Reset() {memset(this, 0, sizeof(*this)); }

	bool operator==(const wiWeaponAttachment& rhs)
	{
		bool res = true;
		for(int i=0; i<WPN_ATTM_MAX; ++i)
			res = res && (attachments[i]==rhs.attachments[i]);
		return res;
	}
	bool operator!=(const wiWeaponAttachment& rhs) { return !((*this)==rhs); }
};

struct wiInventoryItem
{
	__int64		InventoryID;	// unique identifier for that inventory slot
	uint32_t	itemID;		// ItemID inside that slot
	int		quantity;
	int		Var1;		// inventory specific vars. -1 for default
	int		Var2;		//
	enum { MAX_DURABILITY = 10000,};
	int		Var3;		// used for durability, 0-100% mapped to [0..10000]
	
  public:
	wiInventoryItem() 
	{
		Reset();
	}
	
	void		Reset()
	{
		InventoryID = 0;
		itemID   = 0;
		quantity = 0;
		Var1     = -1;
		Var2     = -1;
		Var3     = MAX_DURABILITY;
	}

	bool operator==(const wiInventoryItem& rhs) const
	{
		if(InventoryID != rhs.InventoryID) return false;
		if(itemID != rhs.itemID) return false;
		if(quantity != rhs.quantity) return false;
		if(Var1 != rhs.Var1) return false;
		if(Var2 != rhs.Var2) return false;
		if(Var3 != rhs.Var3) return false;
		return true;
	}
	bool operator!=(const wiInventoryItem& rhs) const { return !((*this)==rhs); }
	
	bool CanStackWith(const wiInventoryItem& wi2, bool movingToGI = false) const;
	
	int adjustDurability(float perc)
	{
		if(perc != 0.0f)
		{
			if(Var3 < 0) Var3 = MAX_DURABILITY;
			Var3 += int(perc * 100.0f);	// each 1% is 100
			Var3 = R3D_CLAMP(Var3, 0, (int)MAX_DURABILITY);
		}
		return Var3;
	}
	
#ifdef WO_SERVER	
	void ResetClipIfFull(); // reset clip attachment Var1 to -1 if clip is full
#endif	
};

struct wiCharDataFull
{
// NOTE: you MUST increase P2PNET_VERSION if you change this structure
	uint32_t	LoadoutID;

	// character stats
	char		Gamertag[32*2];
	int		Hardcore;

	// defined on char creation, can't be modified
	uint32_t	HeroItemID;		// ItemID of base character
	int		HeadIdx;
	int		BodyIdx;
	int		LegsIdx;
	uint32_t	BackpackID; // itemID of backpack to render

	// vars that is used only on client/server
	int		Alive;	// 0 - dead, 1 - alive, 2 - revived, 3 - new character
	__int64		DeathUtcTime;
	int		SecToRevive;
	float		Health; // 0..100; 0-dead. 100 - healthy
	float		Hunger; // 0..100; 0-not hungry, 100 - starving
	float		Thirst; // 0..100; 0-not thirsty, 100 - super thirsty!
	float		Toxic; // 0..100; 0-no toxic, 100 - high toxicity, slowly dying

	float		MedBleeding; // 0..; 0-no bleeding. >0 - time in sec left for this effect
	float		MedFeverCold; // 0..; 0-no fever. >0 - time in sec left for this effect
	//float		MedFoodPoison; // 0..; 0-no effect. >0 - time in sec left for this effect
	float		MedBloodInfection;// 0..; 0-no effect. >0 - time in sec left for this effect

	enum
	{
	  GAMEFLAG_NearPostBox = (1 << 0),
	};

	// current game data
	int		GameMapId;
	DWORD		GameServerId;
	DWORD		isPVE;
	r3dPoint3D	GamePos;
	float		GameDir;
	DWORD		GameFlags;
	__int64		SecFromLastGame;	// note: it can be 0 for new characters

	wiStats		Stats;
	int		Skills[50];

#if defined(MISSIONS) && defined(WO_SERVER)
	Mission::MissionsProgress*	missionsProgress;	// Because of all the usage of memsets for containers of this
													// data type, MissionsProgress has to be a pointer, otherwise
													// the contained std::maps and std::vectors will become corrupted
													// when overwritten with memset.  Make sure to initialize the
													// pointer after the last memset performed by the containers.
#endif

	// clan info
	int		ClanID;
	int		ClanRank; // 0 - leader, 99 - regular member
	char		ClanTag[5*2]; //utf8
	int		ClanTagColor;

	// backpack content, including loadout
	enum {
	  // indices of equipped items inside backpack
	  CHAR_LOADOUT_WEAPON1   = 0,
	  CHAR_LOADOUT_WEAPON2,
	  CHAR_LOADOUT_ITEM1,
	  CHAR_LOADOUT_ITEM2,
	  CHAR_LOADOUT_ITEM3,
	  CHAR_LOADOUT_ITEM4,
	  CHAR_LOADOUT_ARMOR,
	  CHAR_LOADOUT_HEADGEAR,
	  CHAR_REAL_BACKPACK_IDX_START,
	  
	  CHAR_MAX_BACKPACK_SIZE = 64 + CHAR_REAL_BACKPACK_IDX_START
	};
	int		BackpackSize;
	wiInventoryItem Items[CHAR_MAX_BACKPACK_SIZE];

	// installed attachments for weapon.
	wiWeaponAttachment Attachment[2];

	// learned recipes
	static const int MAX_NUM_RECIPES_IN_LOADOUT=64;
	uint32_t	RecipesLearned[MAX_NUM_RECIPES_IN_LOADOUT];	// itemids of learned recipes
	int		NumRecipes;
	
	wiCharDataFull()
#if defined(MISSIONS) && defined(WO_SERVER)
		: missionsProgress( NULL )
#endif
	{
		memset(this, 0, sizeof(*this));
	}
	~wiCharDataFull()
	{
	}

	bool hasItem(uint32_t itemID)
	{
		for(int i=0; i<BackpackSize; ++i)
			if(Items[i].itemID == itemID && Items[i].quantity > 0)
				return true;
		return false;
	}
	bool hasItemWithQuantity(uint32_t itemID, int q)
	{
		int numQ = 0;
		for(int i=0; i<BackpackSize; ++i)
			if(Items[i].itemID == itemID && Items[i].quantity > 0)
				numQ += Items[i].quantity;

		return numQ >= q;
	}
	bool hasRecipe(uint32_t itemID)
	{
		for(int i=0; i<NumRecipes; ++i)
		{
			if(RecipesLearned[i]==itemID)
				return true;
		}
		return false;
	}

	float getTotalWeight() const;
};

struct wiUserProfile
{
	enum DEV_ACCOUNT_ACCESS
	{
		DAA_ENABLED = 1<<0, // legacy, shouldn't be used
		DAA_DEV_ICON = 1<<1, // dev icon in player list, red text in chat
		DAA_TELEPORT = 1<<2, // can teleport or teleport to player, or get player location
		DAA_SPAWN_ITEM = 1<<3, // can spawn items via /gi command
		DAA_KICK = 1<<4, // can kick players from server
		DAA_BAN = 1<<5, // can permanently ban ppl
		DAA_GOD = 1<<6, // can enable god mode
		DAA_INVISIBLE = 1<<7, // player model is totally invisible!
		DAA_HIDDEN = 1<<8, // do not show up in player list
	};
	uint32_t	isDevAccount;
	uint32_t	TimePlayed;	// account time played (not char!), in seconds
	int		PremiumAcc;	// if more than 0 - time in minutes until premium account expiration
	
	// 0 - legend, 1 - pioneer, 2 - survivor, 3 - guest
	// 4 - 15$, 5-25$, 6-50$
	// 10 - steam
	// 50-53 - RU
	// 20-29 - trial account (20-us, 21-ru)
	int		AccountType;

	int		GamePoints;
	int		GameDollars;
	
	int		ResWood;
	int		ResStone;
	int		ResMetal;

	bool isGameHardcore; //gamehardcore
	int  HardMaxPlayers;//gamehardcore
	
	enum { MAX_LOADOUT_SLOTS = 5, };
	wiCharDataFull	ArmorySlots[MAX_LOADOUT_SLOTS];
	int		NumSlots;

	enum { MAX_INVENTORY_SIZE = 2048, };  // maximum global inventory size
	enum { INVENTORY_SIZE_LIMIT = 2048, }; // current logical limit
	uint32_t	NumItems;
	wiInventoryItem	Inventory[MAX_INVENTORY_SIZE];
};

class CUserProfile
{
  public:
	wiUserProfile	ProfileData;

	DWORD		CustomerID;
	DWORD		SessionID;
	int		AccountStatus;

	struct tm	ServerTime;
	int		ProfileDataDirty; // seconds after last game update with not closed game session

	int		ShopClanCreate;
	int		ShopClanAddMembers_GP[6];	// price for adding clan members
	int		ShopClanAddMembers_Num[6];	// number of adding members
	int		ShopCharRevive;
	int		ShopPremiumAcc;
	int		ShopSkillCosts[50][5]; //[MAX_SKILL_ID][NUM_RANKS].
	int		ShopCharRename;
	void		DeriveGamePricesFromItems();

	bool		IsTrialAccount() { 
		return ProfileData.AccountType >= 20 && ProfileData.AccountType <= 29;
	}
	bool		IsRussianAccount() {
		return ProfileData.AccountType >= 50 && ProfileData.AccountType <= 59;
	}
	
  public:
	CUserProfile();
	virtual ~CUserProfile();

	int 		GetProfile(int CharID = 0);
	void		 ParseLoadouts(pugi::xml_node& xmlItem);
	void		 ParseInventory(pugi::xml_node& xmlItem);
	void		 ParseBackpacks(pugi::xml_node& xmlItem);

	wiInventoryItem* getInventorySlot(__int64 InventoryID);
	bool		 haveFreeInventorySlot(bool useLogicalLimit = true);
	wiInventoryItem* getFreeInventorySlot();

	int		ApiGetShopData();

	static void	ParseCharData(wiCharDataFull& loadout, pugi::xml_node& xmlItem);
#ifdef WO_SERVER
	static void	SaveCharData(const wiCharDataFull& loadout, pugi::xml_node& xmlItem);
#endif
#if defined(MISSIONS) && defined(WO_SERVER)
	static void	ParseMissionData(wiCharDataFull& loadout, pugi::xml_node& xmlItem);
	static void	SaveMissionData(const wiCharDataFull& loadout, pugi::xml_node& xmlItem);
#endif
};

#ifndef WO_SERVER	

class CSteamClientCallbacks;
class CUserFriends;
class CUserClans;
class CUserServerRental;

class CClientUserProfile : public CUserProfile
{
  public:
	void		GenerateSessionKey(char* outKey);
	
	int		SelectedCharID;	// currently selected INDEX inside of ArmorySlots
	CUserFriends*	friends;

  public:
	CClientUserProfile();
	~CClientUserProfile();
	
	int		ApiGetItemsInfo();
	int		ApiBuyItem(int itemId, int buyIdx, __int64* out_InventoryID);

	int		ApiCharCreate(const char* Gamertag, int Hardcore, int HeroItemID, int HeadIdx, int BodyIdx, int LegsIdx);
	int		ApiCharDelete();
	int		ApiCharRevive(int NeedMoney);
	int		ApiCharReviveCheck(int* out_needMoney, int* out_secToRevive);
	int		ApiCharChangeSkin(int HeadIdx, int BodyIdx, int LegsIdx); // change skin of current survivor
	int		ApiCharRename(const char* Gamertag, int* out_MinutesLeft);

	int		ApiSkillLearn(int SkillID, int SkillLevel);

	int		ApiUpgradeTrialAccount(const char* code);
	
	// GP->GD conversion
	struct gp2gd_s
	{
	  int		GamePoints;	// upper value for this ConversionRate
	  int		ConversionRate;
	};
	std::vector<gp2gd_s> m_GPConvertRates;
	int		m_GCPriceTable[64];
	int		ApiGetGamePointsConvertsionRates();
	int		ApiConvertGamePoints(int GamePoints);
	int		GetConvertedGP2GD(int GamePoints, int& convRate);

	// client backpack APIs
	int		ApiBackpackToInventory(int GridFrom, int amount);
	int		ApiBackpackFromInventory(__int64 InventoryID, int GridTo/* or -1 for FREE slot*/, int amount);
	int		ApiBackpackGridSwap(int GridFrom, int GridTo);
	int		ApiBackpackGridJoin(int GridFrom, int GridTo);
	int		ApiChangeBackpack(__int64 InventoryID);

	// friends APIs
	int		ApiFriendAddReq(const char* gamertag, int* outFriendStatus);
	int		ApiFriendAddAns(DWORD friendId, bool allow);
	int		ApiFriendRemove(DWORD friendId);
	int		ApiFriendGetStats(DWORD friendId);
	
	// leaderboard
	struct LBEntry_s
	{
	  int		Alive;
	  char		gamertag[32*2];
	  wiStats	stats;
	  DWORD		ClanID;
	  char		ClanTag[5*2];
	  int		ClanTagColor;
	};
	/*
	-- Leaderboard IDs:
	-- SOFTCORE:
	-- 00 - XP
	-- 01 - TimePlayed
	-- 02 - KilledZombies
	-- 03 - KilledSurvivors
	-- 04 - KilledBandits
	-- 05 - Reputation DESC - heroes
	-- 06 - Reputation ASC - bandits
	-- HARDCORE:
	-- 50 - XP
	-- 51 - TimePlayed
	-- 52 - KilledZombies
	-- 53 - KilledSurvivors
	-- 54 - KilledBandits
	-- 55 - Reputation DESC - heroes
	-- 56 - Reputation ASC - bandits
	*/
	r3dgameVector(LBEntry_s) m_lbData[100];
	int		ApiGetLeaderboard(int TableID, int StartPos, int* out_CurPos, int* lbSize);
	
	// leaderboard
	struct GPLog_s
	{
	  int		TransactionID;
	  long		Time;
	  int		Amount;
	  int		Previous;
	  r3dSTLString	Description;
	};
	r3dgameVector(GPLog_s) GPLog_;
	int		ApiGetGPTransactions();
	
	//
	// Clans API is inside this class
	//
	CUserClans*	clans[wiUserProfile::MAX_LOADOUT_SLOTS];

	//
	// Server Rent API is inside this class
	//
	CUserServerRental* serverRent;

	//
	// steam APIs
	//
	CSteamClientCallbacks* steamCallbacks;
	void		RegisterSteamCallbacks();
	void		DeregisterSteamCallbacks();
	
	struct SteamAuthResp_s {
 	  bool		gotResp;
	  int		bAuthorized;
	  __int64	ulOrderID;
	}		steamAuthResp;
	int		ApiSteamStartBuyGP(int priceInCents);

	int		ApiSteamFinishBuyGP(__int64 orderId);
};

// gUserProfile should be defined only in game mode, server must not use this global
extern  CClientUserProfile gUserProfile;	 
#endif	
