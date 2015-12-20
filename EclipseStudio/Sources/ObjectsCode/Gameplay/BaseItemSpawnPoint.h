#pragma once

#include "GameCommon.h"
#include "..\..\GameCode\UserProfile.h"

// base class, shared for client and server
class BaseItemSpawnPoint : public MeshGameObject
{
	DECLARE_CLASS(BaseItemSpawnPoint, MeshGameObject)

public:
	struct ItemSpawn
	{
		r3dPoint3D	pos;
		float		cooldown; // if any. will set time when cooldown will expire
		uint32_t	itemID; // which item was spawned in this location. if item was picked up, it should set itemID to 0 and cooldown to m_Cooldown
		gobjid_t	spawnedObjID;
		ItemSpawn() : pos(0,0,0), cooldown(0.0f), itemID(0) {}
		//	This function used only for debug visualization and selection purposes
		r3dBoundBox GetDebugBBox() const;
	};
	typedef r3dgameVector(ItemSpawn) ITEM_SPAWN_POINT_VECTOR;
	ITEM_SPAWN_POINT_VECTOR m_SpawnPointsV;
	float					m_TickPeriod; // how often this point should check its spawn points for spawning new item
	float					m_Cooldown; // cooldown for each spawn point. cannot spawn item until cooldown has passed
	uint32_t				m_LootBoxID; // used to select which item to spawn
	bool					m_OneItemSpawn; // special item spawn point that spawn only ONE item instead of lootbox
	float					m_DestroyItemTimer; // how long item should be laying around until server will de-spawn it
	bool					m_SpawnLootCrate; // spawn loot crate instead of actual item on client
	bool					m_UniqueSpawnSystem; // spawns only one item for all spawn items, rest spawns remain empty until item picked up\despawned

public:
	BaseItemSpawnPoint();
	virtual ~BaseItemSpawnPoint();

	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};

extern wiInventoryItem RollItem(const class LootBoxConfig* lootCfg, int depth);