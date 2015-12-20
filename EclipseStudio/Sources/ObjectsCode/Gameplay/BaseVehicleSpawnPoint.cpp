#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "BaseVehicleSpawnPoint.h"
#include "../WEAPONS/BaseItemConfig.h"
#include "../WEAPONS/WeaponArmory.h"

#include "multiplayer/P2PMessages.h"

IMPLEMENT_CLASS(BaseVehicleSpawnPoint, "BaseVehicleSpawnPoint", "Object");

wiInventoryItem RollVehicle(const LootBoxConfig* lootCfg, int depth)
{
	r3d_assert(lootCfg);

	wiInventoryItem wi;
	if (depth > 4)
	{
		r3dOutToLog("!!! obj_VehicleSpawnPoint: arrived to lootbox %d with big depth\n", lootCfg->m_itemID);
		return wi;
	}

	double roll = (double)u_GetRandom();

	for (size_t i = 0; i < lootCfg->entries.size(); ++i)
	{
		const LootBoxConfig::LootEntry& le = lootCfg->entries[i];
		if (roll > le.chance)
			continue;

		wi.itemID = le.itemID;
		wi.quantity = 1;

		if (wi.itemID == -1)
		{
			wi.itemID = 0;
			break;
		}

		const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(wi.itemID);
		if (!cfg)
		{
			r3dOutToLog("!!! lootbox %d does not contain existing item %d\n", lootCfg->m_itemID, wi.itemID);
			wi.itemID = 0;
			break;
		}

		if (cfg->category == storecat_LootBox)
			return RollVehicle((const LootBoxConfig*)cfg, ++depth);

		break;
	}

	return wi;
}

BaseVehicleSpawnPoint::BaseVehicleSpawnPoint()
{
	serializeFile = SF_ServerData;

	tickPeriod = 60.0f;
	coolDown = 15*60.0f;
}

BaseVehicleSpawnPoint::~BaseVehicleSpawnPoint()
{
}

void BaseVehicleSpawnPoint::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node cNode = node.child("BaseVehicleSpawnPoint");
	tickPeriod = cNode.attribute("TickPeriod").as_float();
	coolDown = cNode.attribute("CoolDown").as_float();
	lootBoxId = cNode.attribute("LootBoxId").as_uint();

	int numPoints = cNode.attribute("numPoints").as_int();
	char tempStr[32];

	for (int i = 0; i < numPoints; ++i)
	{
		sprintf(tempStr, "point%d", i);

		pugi::xml_node spawnNode = cNode.child(tempStr);
		r3d_assert(!spawnNode.empty());

		VehicleSpawn vehicleSpawn;
		vehicleSpawn.position.x = spawnNode.attribute("x").as_float();
		vehicleSpawn.position.y = spawnNode.attribute("y").as_float();
		vehicleSpawn.position.z = spawnNode.attribute("z").as_float();

		spawnPoints.push_back(vehicleSpawn);
	}
}

void BaseVehicleSpawnPoint::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node cNode = node.append_child();
	cNode.set_name("BaseVehicleSpawnPoint");
	cNode.append_attribute("TickPeriod") = tickPeriod;
	cNode.append_attribute("CoolDown") = coolDown;
	cNode.append_attribute("LootBoxId") = lootBoxId;
	cNode.append_attribute("numPoints") = (int)spawnPoints.size();

	for (size_t i = 0; i < spawnPoints.size(); ++i)
	{
		pugi::xml_node spawnNode = cNode.append_child();
		char tempStr[32];
		sprintf(tempStr, "point%d", i);

		spawnNode.set_name(tempStr);
		spawnNode.append_attribute("x") = spawnPoints[i].position.x;
		spawnNode.append_attribute("y") = spawnPoints[i].position.y;
		spawnNode.append_attribute("z") = spawnPoints[i].position.z;
	}
}

r3dBoundBox BaseVehicleSpawnPoint::VehicleSpawn::GetDebugBBox() const
{
	r3dBoundBox boundBox;
	boundBox.Org = position - r3dPoint3D(0.125f, 0.0f, 0.125f);
	boundBox.Size = r3dPoint3D(0.25f, 0.25f, 0.25f);
	
	return boundBox;
}
