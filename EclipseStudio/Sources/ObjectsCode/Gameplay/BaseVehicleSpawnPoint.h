#pragma once

#include "GameCommon.h"
#include "..\..\GameCode\UserProfile.h"

class BaseVehicleSpawnPoint : public MeshGameObject
{
	DECLARE_CLASS(BaseVehicleSpawnPoint, MeshGameObject)

public:
	struct VehicleSpawn
	{
		r3dPoint3D position;
		float coolDown;
		uint32_t vehicleID; // the vehicle id to spawn at this object.
		gobjid_t spawnedVehicleId; // which vehicle was actually spawned in this location. if vehicle is taken, reset to 0.

		VehicleSpawn() : position (0, 0, 0), coolDown(0.0f), vehicleID(0) {}
		r3dBoundBox GetDebugBBox() const;
	};

	typedef r3dgameVector(VehicleSpawn) VEHICLE_SPAWN_POINT_VECTOR;

	VEHICLE_SPAWN_POINT_VECTOR spawnPoints;
	float tickPeriod;
	float coolDown;
	uint32_t lootBoxId;

public:
	BaseVehicleSpawnPoint();
	virtual ~BaseVehicleSpawnPoint();

	virtual void WriteSerializedData(pugi::xml_node& node);
	virtual void ReadSerializedData(pugi::xml_node& node);
};

extern wiInventoryItem RollVehicle(const class LootBoxConfig* lootCfg, int depth);