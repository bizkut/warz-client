#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "BaseItemSpawnPoint.h"
#include "../WEAPONS/BaseItemConfig.h"
#include "../WEAPONS/WeaponArmory.h"

#include "multiplayer/P2PMessages.h"

IMPLEMENT_CLASS(BaseItemSpawnPoint, "BaseItemSpawnPoint", "Object");
//AUTOREGISTER_CLASS(BaseControlPoint);

wiInventoryItem RollItem(const LootBoxConfig* lootCfg, int depth)
{
	wiInventoryItem wi;
	if(depth > 4)
	{
		r3dOutToLog("!!! obj_ServerItemSpawnPoint: arrived to lootbox %d with big depth\n", lootCfg->m_itemID);
		return wi;
	}

	r3d_assert(lootCfg);
	double roll = (double)u_GetRandom();
	//r3dOutToLog("Rolling %d %s, %d entries, roll:%f\n", lootCfg->m_itemID, lootCfg->m_StoreName, lootCfg->entries.size(), roll); CLOG_INDENT;

	for(size_t i=0; i<lootCfg->entries.size(); i++)
	{
		const LootBoxConfig::LootEntry& le = lootCfg->entries[i];
		if(roll > le.chance)
			continue;
		if(le.itemID == 0)
		{
			wi.itemID   = 'GOLD'; // special item for GameDollars
			wi.quantity = (int)u_GetRandom((float)le.GDMin, (float)le.GDMax);
			break;
		}

		wi.itemID   = le.itemID;
		wi.quantity = 1;

		// -1 is special "no roll" code
		if(wi.itemID == -1) {
			wi.itemID = 0;
			break;
		}

		// check if this is nested lootbox
		const BaseItemConfig* cfg2 = g_pWeaponArmory->getConfig(wi.itemID);
		if(!cfg2) {
			r3dOutToLog("!! lootbox %d contain not existing item %d\n", lootCfg->m_itemID, wi.itemID);
			wi.itemID = 0;
			break;
		}
		//r3dOutToLog("won %d %s\n", cfg2->m_itemID, cfg2->m_StoreName);
		if(cfg2->category == storecat_LootBox)
			return RollItem((const LootBoxConfig*)cfg2, ++depth);
		break;
	}

	return wi;
}

BaseItemSpawnPoint::BaseItemSpawnPoint()
{
	serializeFile = SF_ServerData;

	m_TickPeriod = 60.0f; 
	m_Cooldown = 15*60.0f;
	m_DestroyItemTimer = 50.0f*60.0f; 
	m_OneItemSpawn = false;
	m_SpawnLootCrate = false;
	m_UniqueSpawnSystem = false;
}

BaseItemSpawnPoint::~BaseItemSpawnPoint()
{
}

void BaseItemSpawnPoint::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node cpNode = node.child("BaseItemSpawnPoint");
	m_TickPeriod = cpNode.attribute("m_TickPeriod").as_float();
	m_Cooldown = cpNode.attribute("m_Cooldown").as_float();
	m_LootBoxID = cpNode.attribute("m_LootBoxID").as_uint();
	if(!cpNode.attribute("m_OneItemSpawn").empty())
		m_OneItemSpawn = cpNode.attribute("m_OneItemSpawn").as_bool();
	if(!cpNode.attribute("m_DestroyItemTimer").empty())
		m_DestroyItemTimer = cpNode.attribute("m_DestroyItemTimer").as_float();
	if(!cpNode.attribute("m_SpawnLootCrate").empty())
		m_SpawnLootCrate = cpNode.attribute("m_SpawnLootCrate").as_bool();
	if(!cpNode.attribute("m_UniqueSpawnSystem").empty())
		m_UniqueSpawnSystem = cpNode.attribute("m_UniqueSpawnSystem").as_bool();

	int numPoints = cpNode.attribute("numPoints").as_int();
	char tempStr[32];
	for(int i=0; i<numPoints; ++i)
	{
		sprintf(tempStr, "point%d", i);
		pugi::xml_node spawnNode = cpNode.child(tempStr);
		r3d_assert(!spawnNode.empty());
		ItemSpawn itemSpawn;
		itemSpawn.pos.x = spawnNode.attribute("x").as_float();
		itemSpawn.pos.y = spawnNode.attribute("y").as_float();
		itemSpawn.pos.z = spawnNode.attribute("z").as_float();
		m_SpawnPointsV.push_back(itemSpawn);
	}
}

void BaseItemSpawnPoint::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node cpNode = node.append_child();
	cpNode.set_name("BaseItemSpawnPoint");
	cpNode.append_attribute("m_TickPeriod") = m_TickPeriod;
	cpNode.append_attribute("m_Cooldown") = m_Cooldown;
	cpNode.append_attribute("m_LootBoxID") = m_LootBoxID;
	cpNode.append_attribute("m_OneItemSpawn") = m_OneItemSpawn;
	cpNode.append_attribute("m_DestroyItemTimer") = m_DestroyItemTimer;
	cpNode.append_attribute("m_SpawnLootCrate") = m_SpawnLootCrate;
	cpNode.append_attribute("m_UniqueSpawnSystem") = m_UniqueSpawnSystem;
	cpNode.append_attribute("numPoints") = (int)m_SpawnPointsV.size();
	for(size_t i=0; i<m_SpawnPointsV.size(); ++i)
	{
		pugi::xml_node spawnNode = cpNode.append_child();
		char tempStr[32];
		sprintf(tempStr, "point%d", i);
		spawnNode.set_name(tempStr);
		spawnNode.append_attribute("x") = m_SpawnPointsV[i].pos.x;
		spawnNode.append_attribute("y") = m_SpawnPointsV[i].pos.y;
		spawnNode.append_attribute("z") = m_SpawnPointsV[i].pos.z;
	}
}

//////////////////////////////////////////////////////////////////////////

r3dBoundBox BaseItemSpawnPoint::ItemSpawn::GetDebugBBox() const
{
	r3dBoundBox bb;
	bb.Org = pos - r3dPoint3D(0.125f, 0.0f, 0.125f);
	bb.Size = r3dPoint3D(0.25f, 0.25f, 0.25f);;
	return bb;
}