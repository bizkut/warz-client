//=========================================================================
//	Module: VehicleDescriptor.h
//	Copyright (C) 2012.
//=========================================================================

#pragma once

#ifdef VEHICLES_ENABLED

#include "vehicle/PxVehicleSDK.h"
//#include "PxVehicleWheels.h"

//////////////////////////////////////////////////////////////////////////

const char * const VEHICLE_PART_NAMES[] =
{
	"Bone_Root",
	"Bone_Wheel_001",
	"Bone_Wheel_002",
	"Bone_Wheel_003",
	"Bone_Wheel_004",
 	"Bone_Wheel_005",
 	"Bone_Wheel_006",
 	"Bone_Wheel_007",
 	"Bone_Wheel_008",
 	"Bone_Wheel_009",
 	"Bone_Wheel_010",
 	"Bone_Wheel_011",
 	"Bone_Wheel_012",
 	"Bone_Wheel_013",
 	"Bone_Wheel_014",
 	"Bone_Wheel_015",
 	"Bone_Wheel_016",
	"Bone_Seat_001",
	"Bone_Seat_002",
	"Bone_Seat_003",
	"Bone_Seat_004",
	"Bone_Seat_005",
	"Bone_Seat_006",
	"Bone_Seat_007",
	"Bone_Seat_008",
	"Bone_Seat_009",
	"Bone_Turrent_Base",
	"Bone_Turrent_Gun"
};

const char * const VEHICLE_EXIT_NAMES[] =
{	
	"ExitPos2", // 2 is driver side (we use seatPosition to get the right index, seatPosition 0 is driver)
	"ExitPos1",
	"ExitPos0"	
	//"ExitPos3", -- new buggy model does not have index at 4
};

const char * const VEHICLE_SEAT_NAMES[] =
{
	"Bone_Seat_001",
	"Bone_Seat_002",
	"Bone_Seat_003",
	"Bone_Seat_004",
	"Bone_Seat_005",
	"Bone_Seat_006",
	"Bone_Seat_007",
	"Bone_Seat_008",
	"Bone_Seat_009"
};

const char * const VEHICLE_DAMPER_NAMES[] =
{
	"Bone_Damper_001",
	"Bone_Damper_002",
	"Bone_Damper_003",
	"Bone_Damper_004",
 	"Bone_Damper_005",
 	"Bone_Damper_006",
 	"Bone_Damper_007",
 	"Bone_Damper_008",
 	"Bone_Damper_009",
 	"Bone_Damper_010",
 	"Bone_Damper_011",
 	"Bone_Damper_012",
 	"Bone_Damper_013",
 	"Bone_Damper_014",
 	"Bone_Damper_015",
 	"Bone_Damper_016"
};

const char * const VEHICLE_TURRET_NAMES[] =
{
	"Bone_Turrent_Base"
};

const char * const VEHICLE_CANNON_NAMES[] =
{
	"Bone_Turrent_Gun"
};

const char * const FIRE_TANK_POSITION[] =
{
	"Bone_Turrent_Gun_Tip"
};

const char * const HELICOPTER_ROTOR_NAMES[] =
{
	"Rotor_001",
	"Rotor_002"
};

const uint32_t VEHICLE_PARTS_COUNT = _countof(VEHICLE_PART_NAMES);
const uint32_t VEHICLE_DAMPER_COUNT = _countof(VEHICLE_DAMPER_NAMES);
const uint32_t HELICOPTER_ROTOR_COUNT = _countof(HELICOPTER_ROTOR_NAMES);
const uint32_t MAX_HULL_PARTS_COUNT = 1;
const uint32_t MAX_SEATS_COUNT = _countof(VEHICLE_SEAT_NAMES);
const uint32_t MAX_TURRETS_COUNT = _countof(VEHICLE_TURRET_NAMES);
const uint32_t MAX_CANNONS_COUNT = _countof(VEHICLE_CANNON_NAMES);
const uint32_t MAX_WHEELS_COUNT = VEHICLE_PARTS_COUNT - MAX_HULL_PARTS_COUNT - MAX_SEATS_COUNT - MAX_TURRETS_COUNT - MAX_CANNONS_COUNT;
const uint32_t MAX_EXITS_COUNT = _countof(VEHICLE_EXIT_NAMES);


const uint32_t INCORRECT_INDEX = 0xffffffff;

class obj_Vehicle;

//////////////////////////////////////////////////////////////////////////

struct VehicleDescriptor
{
#ifndef WO_SERVER
	physx::PxVehicleDrive4W *vehicle;
	physx::PxVehicleDriveTank *tank;
	r3dSkeleton *skl;
#endif
	r3dSTLString driveFileDefinitionPath;

	PxVehicleAckermannGeometryData ackermannData;
	PxVehicleChassisData chassisData;
	PxVehicleEngineData engineData;
	PxVehicleGearsData gearsData;
	PxVehicleClutchData clutchData;
	PxVehicleDifferential4WData diffData;
	
	int	durability;
	int armorExterior;
	int armorInterior;
	int maxFuel;
	float weight;
	float tireFriction;
	int thresholdIgnoreMelee;
	int thresholdIgnoreBullets;
	int hasTracks;
	r3dString startEngine;
	r3dString stopEngine;
	r3dString engineLoop;
	int NewISSVehicle;

	struct WheelData
	{
		PxVehicleWheelData wheelData;
		PxVehicleSuspensionData suspensionData;
		PxVehicleTireData tireData;
		PxVec3 suspensionTravelDir;
#ifndef WO_SERVER
		r3dBone* wheelBone;
		r3dBone* DamperBone;
#endif

		WheelData()
		: suspensionTravelDir(0, -1, 0)
#ifndef WO_SERVER
		, wheelBone( NULL )
#endif
		{
			wheelData.mRadius = 1.0f;
			suspensionData.mSpringStrength = 35000.0f;
			suspensionData.mSpringDamperRate = 4500.0f;
			wheelData.mMOI = 1.0f;
		}
	};
	r3dTL::TArray<WheelData> wheelsData;

	static const uint32_t maxWeight = 10000;
	uint32_t numWheels;
#ifndef WO_SERVER
	uint32_t numHullParts;
	uint32_t numSeats;
	uint32_t numTurrets;
	uint32_t numCannons;
	uint32_t numDampers;
	uint32_t numRotors;

	/**
	* Wheel remap indices. Bone indices in order corresponding strings in VEHICLE_PART_NAMES array
	*/
	uint32_t seatBonesRemapIndices[MAX_SEATS_COUNT];
	uint32_t wheelBonesRemapIndices[MAX_WHEELS_COUNT];
	uint32_t hullBonesRemapIndices[MAX_HULL_PARTS_COUNT];

	uint32_t turretBonesRemapIndices[MAX_TURRETS_COUNT];
	uint32_t cannonBonesRemapIndices[MAX_CANNONS_COUNT];

	uint32_t RotorBonesRemapIndices[HELICOPTER_ROTOR_COUNT]; //Helicopter rotor

	uint32_t DamperBonesRemapIndices[VEHICLE_DAMPER_COUNT];

	PxVec3 wheelCenterOffsets[MAX_WHEELS_COUNT];
	PxVec3 seatCenterOffsets[MAX_SEATS_COUNT];
	PxVec3 hullCenterOffsets[MAX_HULL_PARTS_COUNT];
	PxVec3 exitCenterOffsets[MAX_EXITS_COUNT];
	PxVec3 turretCenterOffets[MAX_TURRETS_COUNT];
	PxVec3 cannonCenterOffsets[MAX_TURRETS_COUNT];
	PxVec3 damperCenterOffsets[VEHICLE_DAMPER_COUNT];
	PxVec3 RotorCenterOffsets[HELICOPTER_ROTOR_COUNT]; //Helicopter rotor
#endif

	obj_Vehicle *owner;

	VehicleDescriptor();
	~VehicleDescriptor();
	bool Init(const r3dMesh *m);

#ifndef WO_SERVER
	/** Get wheel index corresponding given bone. If requested bone not a wheel bone, return INCORRECT_INDEX.  */
	uint32_t GetWheelIndex(uint32_t boneId) const;
	
	/** Get hull index corresponding given bone. If requested bone not a hull bone, return INCORRECT_INDEX.  */
	uint32_t GetHullIndex(uint32_t boneId) const;
	uint32_t GetDamperIndex(uint32_t boneId) const;

	void ConfigureVehicleSimulationData(PxVehicleDriveSimData4W *dd = 0, PxVehicleWheelsSimData *wd = 0);

	/** Save vehicle parameters to xml file. */
	bool Save(const char *fileName);

	// true if found, false if not found.
	bool GetExitIndex( r3dVector& outVector, uint32_t exitIndex) const;
	bool GetSeatIndex(r3dVector& outVector, uint32_t boneId) const;
	bool GetTurretPositionIndex(r3dVector& outVector, uint32_t boneId) const;
	bool GetFireIndex(r3dVector& outVector, uint32_t boneId) const;
	uint32_t GetTurretIndex(uint32_t boneId) const;
	uint32_t GetCannonIndex(uint32_t boneId) const;
	r3dBone* GetTurretBone();

	PxF32 GetSpeed();
	PxF32 GetLongTireSlip(const PxU32 tireIndex);
	PxF32 GetLatTireSlip(const PxU32 tireIndex);
	PxF32 GetSideSpeed();

	PxReal GetEngineRpm();

	float PositionFire;
#endif

	PxReal GetMaxSpeed();
#ifdef WO_SERVER
void ReadSerializedData(pugi::xml_node& node);
#endif

private:
#ifndef WO_SERVER
	uint32_t GetIndex(const uint32_t *arr, uint32_t arrCount, uint32_t boneIndex) const;
	bool Load(const r3dMesh *m);
	bool InitSkeleton(const r3dSkeleton *s);
#endif
	void InitToDefault();
};

#endif VEHICLES_ENABLED
