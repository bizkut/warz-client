//=========================================================================
//	Module: VehicleDescriptor.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once
#include "r3dPCH.h"
#include "r3d.h"

#ifdef VEHICLES_ENABLED

#include "VehicleDescriptor.h"
#include "XMLHelpers.h"

#ifndef WO_SERVER
#include "PhysXWorld.h"

struct BoneSearcher
{
	const char *toFind;
	explicit BoneSearcher(const char *nameToFind): toFind(nameToFind) {}
	bool operator() (const r3dBone &b)
	{
		return strcmpi(toFind, b.Name) == 0;
	}
};
#endif


//////////////////////////////////////////////////////////////////////////

VehicleDescriptor::VehicleDescriptor()
#ifdef WO_SERVER
: numWheels(4)
#else
: numWheels(0)
, numHullParts(0)
, numSeats(0)
, numTurrets(0)
, numCannons(0)
, vehicle(0)
, skl(0)
, PositionFire(0)
#endif
, durability(1500)
, armorExterior(100)
, armorInterior(100)
, thresholdIgnoreBullets(80)
, thresholdIgnoreMelee(60)
, maxFuel(3000)
, weight(2500)
, owner(NULL)
, wheelsData(0)
, hasTracks(false)
{
#ifndef WO_SERVER
	for (uint32_t i = 0; i < _countof(wheelBonesRemapIndices); ++i)
	{
		wheelBonesRemapIndices[i] = INVALID_INDEX;
	}
	for (uint32_t i = 0; i < _countof(hullBonesRemapIndices); ++i)
	{
		hullBonesRemapIndices[i] = INVALID_INDEX;
	}
	for (uint32_t i = 0; i < _countof(seatBonesRemapIndices); ++i)
	{
		seatBonesRemapIndices[i] = INVALID_INDEX;
	}

	::ZeroMemory(wheelCenterOffsets, sizeof(wheelCenterOffsets));
	::ZeroMemory(hullCenterOffsets, sizeof(hullCenterOffsets));
	::ZeroMemory(seatCenterOffsets, sizeof(seatCenterOffsets));
#endif
}

//////////////////////////////////////////////////////////////////////////

VehicleDescriptor::~VehicleDescriptor()
{
#ifndef WO_SERVER
	if (vehicle)
		vehicle->free();
	delete skl;
#else
	wheelsData.Clear();
	owner = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////////

bool VehicleDescriptor::Init(const r3dMesh *m)
{
#ifndef WO_SERVER
	if (!Load(m))
	{
		r3dArtBug("Mesh does not have associated vehicle drive information");
		return false;
	}
#else
	InitToDefault();
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER

bool VehicleDescriptor::InitSkeleton(const r3dSkeleton *s)
{
	for (uint32_t i = 0; i < MAX_WHEELS_COUNT; ++i)
	{
		wheelBonesRemapIndices[i] = INCORRECT_INDEX;
		wheelCenterOffsets[i] = PxVec3(0, 0, 0);
	}

	for (uint32_t i = 0; i < MAX_HULL_PARTS_COUNT; ++i)
	{
		hullBonesRemapIndices[i] = INCORRECT_INDEX;
		hullCenterOffsets[i] = PxVec3(0, 0, 0);
	}

	for (uint32_t i = 0; i < MAX_SEATS_COUNT + 1; ++i)
	{
		seatBonesRemapIndices[i] = INCORRECT_INDEX;
		seatCenterOffsets[i] = PxVec3(0, 0, 0);
	}

	for (uint32_t i = 0; i < MAX_TURRETS_COUNT; ++i)
	{
		turretBonesRemapIndices[i] = INCORRECT_INDEX;
		turretCenterOffets[i] = PxVec3(0,0,0);
	}

	for (uint32_t i = 0; i < MAX_CANNONS_COUNT; ++i)
	{
		cannonBonesRemapIndices[i] = INCORRECT_INDEX;
		cannonCenterOffsets[i] = PxVec3(0,0,0);
	}

	for (uint32_t i = 0; i < VEHICLE_DAMPER_COUNT; ++i)
	{
		DamperBonesRemapIndices[i] = INCORRECT_INDEX;
		damperCenterOffsets[i] = PxVec3(0,0,0);
	}
	for (uint32_t i = 0; i < HELICOPTER_ROTOR_COUNT; ++i)
	{
		RotorBonesRemapIndices[i] = INCORRECT_INDEX;
		RotorCenterOffsets[i] = PxVec3(0,0,0);
	}
	

	//	4 wheels + 1 hull part at least
	if (!s || s->NumBones < 5)
		return false;


	const r3dBone *bonesStart = s->Bones;
	const r3dBone *bonesEnd = s->Bones + s->NumBones;

	numWheels = 0;
	numHullParts = 0;
	numSeats = 0;
	numTurrets = 0;
	numCannons = 0;
	numDampers = 0;
	numRotors = 0;

	for (uint32_t i = 0; i < MAX_WHEELS_COUNT; ++i)
	{
		const r3dBone *partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_PART_NAMES[i + MAX_HULL_PARTS_COUNT]));
		if (partName != bonesEnd)
		{
			size_t idx = std::distance(bonesStart, partName);
			wheelBonesRemapIndices[i] = idx;
			++numWheels;
		}
	}

	for (uint32_t i = 0; i < MAX_HULL_PARTS_COUNT; ++i)
	{
		const r3dBone *partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_PART_NAMES[i]));
		if (partName != bonesEnd)
		{
			size_t idx = std::distance(bonesStart, partName);
			hullBonesRemapIndices[i] = idx;
			++numHullParts;
		}
	}

	for (uint32_t i = 0; i < MAX_SEATS_COUNT; ++i)
	{
		const r3dBone *partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_PART_NAMES[i + MAX_WHEELS_COUNT + MAX_HULL_PARTS_COUNT]));
		if (partName != bonesEnd)
		{
			size_t idx = std::distance(bonesStart, partName);
			seatBonesRemapIndices[i] = idx;
			++numSeats;
		}
	}

	for (uint32_t i = 0; i < MAX_TURRETS_COUNT; ++i)
	{
		const r3dBone* partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_PART_NAMES[i + MAX_WHEELS_COUNT + MAX_HULL_PARTS_COUNT + MAX_SEATS_COUNT]));
		if (partName != bonesEnd)
		{
			size_t idx = std::distance(bonesStart, partName);
			turretBonesRemapIndices[i] = idx;
			++numTurrets;
		}
	}

	for (uint32_t i = 0; i < MAX_CANNONS_COUNT; ++i)
	{
		const r3dBone* partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_PART_NAMES[i + MAX_WHEELS_COUNT + MAX_HULL_PARTS_COUNT + MAX_SEATS_COUNT + MAX_TURRETS_COUNT]));
		if (partName != bonesEnd)
		{
			size_t idx = std::distance(bonesStart, partName);
			cannonBonesRemapIndices[i] = idx;
			++numCannons;
		}
	}

	for (uint32_t i = 0; i < VEHICLE_DAMPER_COUNT; ++i)
	{
		const r3dBone* partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_DAMPER_NAMES[i]));
		if (partName != bonesEnd)
		{
			size_t idx = std::distance(bonesStart, partName);
			DamperBonesRemapIndices[i] = idx;
			++numDampers;
		}
	}

	for (uint32_t i = 0; i < HELICOPTER_ROTOR_COUNT; ++i)
	{
		const r3dBone* partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(HELICOPTER_ROTOR_NAMES[i]));
		if (partName != bonesEnd)
		{
			size_t idx = std::distance(bonesStart, partName);
			RotorBonesRemapIndices[i] = idx;
			++numRotors;
		}
	}

	//	Calculate center offsets
	r3dVector tmp = s->Bones[hullBonesRemapIndices[0]].vRelPlacement;
	PxVec3 centerPos(tmp.x, tmp.y, tmp.z);
	for (uint32_t i = 0; i < MAX_WHEELS_COUNT; ++i)
	{
		uint32_t boneIdx = wheelBonesRemapIndices[i];
		if (boneIdx == INCORRECT_INDEX)
			continue;

        if (NewISSVehicle==1)
		{
			if (i==0) i = 2; else if (i==1) i = 3; else if (i==2) i = 0; else if (i==3) i = 1;
		}

		r3dVector tmp = s->Bones[boneIdx].vRelPlacement;
		PxVec3 bonePos(tmp.x, tmp.y, tmp.z);
		wheelCenterOffsets[i] = bonePos;
		std::swap(wheelCenterOffsets[i].x, wheelCenterOffsets[i].z);
		wheelCenterOffsets[i].z *= -1;
	}
 	std::swap(wheelCenterOffsets[0], wheelCenterOffsets[2]);
 	std::swap(wheelCenterOffsets[1], wheelCenterOffsets[3]);

	for (uint32_t i = 0; i < MAX_HULL_PARTS_COUNT; ++i)
	{
		uint32_t boneIdx = hullBonesRemapIndices[i];
		if (boneIdx == INCORRECT_INDEX)
			continue;

		r3dVector tmp = s->Bones[boneIdx].vRelPlacement;
		PxVec3 bonePos(tmp.x, tmp.y, tmp.z);
		hullCenterOffsets[i] = bonePos;
		std::swap(hullCenterOffsets[i].x, hullCenterOffsets[i].z);
	}

	for (uint32_t i = 0; i < MAX_SEATS_COUNT; ++i)
	{
		uint32_t boneIndex = seatBonesRemapIndices[i];
		if (boneIndex == INCORRECT_INDEX)
			continue;

		r3dVector tmp = s->Bones[boneIndex].vRelPlacement;
		PxVec3 bonePos(tmp.x, tmp.y, tmp.z);
		seatCenterOffsets[i] = bonePos;
		std::swap(seatCenterOffsets[i].x, seatCenterOffsets[i].z);
		seatCenterOffsets[i].z *= -1;
	}

	for (uint32_t i = 0; i < MAX_TURRETS_COUNT; ++i)
	{
		uint32_t boneIndex = turretBonesRemapIndices[i];
		if (boneIndex == INCORRECT_INDEX)
			continue;

		r3dVector tmp = s->Bones[boneIndex].vRelPlacement;
		PxVec3 bonePos(tmp.x, tmp.y, tmp.z);
		turretCenterOffets[i] = bonePos;
		std::swap(turretCenterOffets[i].x, turretCenterOffets[i].z);
		turretCenterOffets[i].z *= -1;
	}

	for (uint32_t i = 0; i < MAX_CANNONS_COUNT; ++i)
	{
		uint32_t boneIndex = cannonBonesRemapIndices[i];
		if (boneIndex == INCORRECT_INDEX)
			continue;

		s->Bones[boneIndex].vRelPlacement.y=2.26f;
		s->Bones[boneIndex].vRelPlacement.x=1.443f;

		r3dVector tmp = s->Bones[boneIndex].vRelPlacement;
		PxVec3 bonePos(tmp.x, tmp.y, tmp.z);

		cannonCenterOffsets[i] = bonePos;
		std::swap(cannonCenterOffsets[i].x, cannonCenterOffsets[i].z);
		cannonCenterOffsets[i].z *= -1;
	}

	for (uint32_t i = 0; i < HELICOPTER_ROTOR_COUNT; ++i)
	{
		uint32_t boneIndex = RotorBonesRemapIndices[i];
		if (boneIndex == INCORRECT_INDEX)
			continue;

		r3dVector tmp = s->Bones[boneIndex].vRelPlacement;
		PxVec3 bonePos(tmp.x, tmp.y, tmp.z);
		RotorCenterOffsets[i] = bonePos;
		std::swap(RotorCenterOffsets[i].x, RotorCenterOffsets[i].z);
		RotorCenterOffsets[i].z *= -1;
	}

	for (uint32_t i = 0; i < VEHICLE_DAMPER_COUNT; ++i)
	{
		uint32_t boneIndex = DamperBonesRemapIndices[i];
		if (boneIndex == INCORRECT_INDEX)
			continue;

		//s->Bones[boneIndex].vRelPlacement.x=wheelCenterOffsets[i].x;
		if (i<=11)
			s->Bones[boneIndex].vRelPlacement.y=1.6f;
		else
			s->Bones[boneIndex].vRelPlacement.y+=1.0f;
		//s->Bones[boneIndex].vRelPlacement.z=wheelCenterOffsets[i].z;
		r3dVector tmp = s->Bones[boneIndex].vRelPlacement;
		PxVec3 bonePos(tmp.x, tmp.y, tmp.z);

		damperCenterOffsets[i] = bonePos;
		std::swap(damperCenterOffsets[i].x, damperCenterOffsets[i].z);
		damperCenterOffsets[i].z *= -1;

	}
 	std::swap(damperCenterOffsets[0], damperCenterOffsets[2]);
 	std::swap(damperCenterOffsets[1], damperCenterOffsets[3]);

	return true;
}

//////////////////////////////////////////////////////////////////////////

uint32_t VehicleDescriptor::GetIndex(const uint32_t *arr, uint32_t arrCount, uint32_t boneIndex) const
{
	for (uint32_t i = 0; i < arrCount; ++i)
	{
		if (arr[i] == boneIndex)
			return i;
	}
	return INVALID_INDEX;
}

//////////////////////////////////////////////////////////////////////////

uint32_t VehicleDescriptor::GetWheelIndex(uint32_t boneId) const
{
	return GetIndex(wheelBonesRemapIndices, MAX_WHEELS_COUNT, boneId);
}

//////////////////////////////////////////////////////////////////////////

uint32_t VehicleDescriptor::GetHullIndex(uint32_t boneId) const
{
	return GetIndex(hullBonesRemapIndices, MAX_HULL_PARTS_COUNT, boneId);
}

uint32_t VehicleDescriptor::GetTurretIndex(uint32_t boneId) const
{
	return GetIndex(turretBonesRemapIndices, MAX_TURRETS_COUNT, boneId);
}

uint32_t VehicleDescriptor::GetCannonIndex(uint32_t boneId) const
{
	return GetIndex(cannonBonesRemapIndices, MAX_CANNONS_COUNT, boneId);
}

r3dBone* VehicleDescriptor::GetTurretBone()
{
	r3dBone* bonesStart = skl->Bones;
	r3dBone* bonesEnd = skl->Bones + skl->NumBones;

	r3dBone* partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_TURRET_NAMES[0]));
	if (partName != bonesEnd)
		return partName;

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

bool VehicleDescriptor::GetSeatIndex(r3dVector& outVector, uint32_t boneId) const
{
	const r3dBone *bonesStart = skl->Bones;
	const r3dBone *bonesEnd = skl->Bones + skl->NumBones;

	if (boneId > sizeof(VEHICLE_SEAT_NAMES))
		return false;

	const r3dBone *partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_SEAT_NAMES[boneId]));
	if (partName != bonesEnd)
	{
		size_t idx = std::distance(bonesStart, partName);

		r3dBone& b = skl->Bones[idx];
		PxTransform t;
		if (hasTracks == 0)
			t = vehicle->getRigidDynamicActor()->getGlobalPose();
		else
			t = tank->getRigidDynamicActor()->getGlobalPose();
		D3DXQUATERNION q, q1, q2 = D3DXQUATERNION( t.q.x, t.q.y, t.q.z, t.q.w );
		D3DXQuaternionRotationMatrix( &q1, &b.CurrentTM );
		q = q1 * q2;
		D3DXMATRIX m1;
		D3DXMatrixRotationQuaternion( &m1, &q );
		r3dMatrix m2;
		m2.Assign( m1._11, m1._12, m1._13,
				   m1._21, m1._22, m1._23,
				   m1._31, m1._32, m1._33 );
		b.vRelPlacement.Rotate( m2, outVector );
		return true;
	}
	return false;
}

bool VehicleDescriptor::GetTurretPositionIndex(r3dVector& outVector, uint32_t boneId) const
{
	const r3dBone *bonesStart = skl->Bones;
	const r3dBone *bonesEnd = skl->Bones + skl->NumBones;

	if (boneId > sizeof(VEHICLE_PART_NAMES))
		return false;

	const r3dBone *partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_PART_NAMES[boneId]));
	if (partName != bonesEnd)
	{
		size_t idx = std::distance(bonesStart, partName);

		r3dBone& b = skl->Bones[idx];

		if (boneId == 26) // BASE
		{
			b.vRelPlacement.x=0.405f;
			b.vRelPlacement.z=0.422f;
			b.vRelPlacement.y=1.888f;
		}
		else if (boneId == 27) // GUN
		{
			b.vRelPlacement.x=0.405f;
			b.vRelPlacement.z=-0.422f;
			b.vRelPlacement.y=1.888f;
		}

		D3DXMATRIX boneTransform;
		PxTransform t;
		if (hasTracks == 0)
			t = vehicle->getRigidDynamicActor()->getGlobalPose();
		else
			t = tank->getRigidDynamicActor()->getGlobalPose();


		D3DXQUATERNION q, q1, q2 = D3DXQUATERNION( t.q.x, t.q.y, t.q.z, t.q.w );
		D3DXQuaternionRotationMatrix( &q1, &b.CurrentTM );
		PxVec3 bonePos = PxVec3(b.vRelPlacement.x, b.vRelPlacement.y, b.vRelPlacement.z);
		D3DXMATRIX toOrigin, fromOrigin;
		D3DXMatrixTranslation(&toOrigin, -bonePos.x, -bonePos.y, -bonePos.z);
		D3DXMatrixTranslation(&fromOrigin, t.p.x, t.p.y, t.p.z);
		
		q = q1 * q2;

		D3DXMatrixRotationQuaternion(&boneTransform, &q);

		D3DXMATRIX RotateMatrix;
		D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(t.q.x),R3D_DEG2RAD(t.q.y),R3D_DEG2RAD(t.q.z));
		D3DXMatrixMultiply(&fromOrigin, &fromOrigin, &RotateMatrix);

		D3DXMatrixMultiply(&boneTransform, &toOrigin, &boneTransform);
		D3DXMatrixMultiply(&boneTransform, &boneTransform, &fromOrigin);

		r3dMatrix m2;
		m2.Assign( boneTransform._11, boneTransform._12, boneTransform._13,
				   boneTransform._21, boneTransform._22, boneTransform._23,
				   boneTransform._31, boneTransform._32, boneTransform._33 );
		b.vRelPlacement.Rotate( m2, outVector );
		return true;
	}
	return false;
}

bool VehicleDescriptor::GetFireIndex(r3dVector& outVector, uint32_t boneId) const
{
	const r3dBone *bonesStart = skl->Bones;
	const r3dBone *bonesEnd = skl->Bones + skl->NumBones;

	if (boneId > sizeof(VEHICLE_TURRET_NAMES))
		return false;

	const r3dBone *partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(FIRE_TANK_POSITION[boneId]));
	if (partName != bonesEnd)
	{
		size_t idx = std::distance(bonesStart, partName);

		r3dBone& b = skl->Bones[idx];
		b.vRelPlacement.x=-8.017f;
		b.vRelPlacement.y=2.552f;

		D3DXMATRIX boneTransform;
		PxTransform t;
		if (hasTracks == 0)
			t = vehicle->getRigidDynamicActor()->getGlobalPose();
		else
			t = tank->getRigidDynamicActor()->getGlobalPose();


		D3DXQUATERNION q, q1, q2 = D3DXQUATERNION( t.q.x, t.q.y, t.q.z, t.q.w );
		D3DXQuaternionRotationMatrix( &q1, &b.CurrentTM );
		PxVec3 bonePos = PxVec3(b.vRelPlacement.x, b.vRelPlacement.y, b.vRelPlacement.z);
		D3DXMATRIX toOrigin, fromOrigin;
		D3DXMatrixTranslation(&toOrigin, -bonePos.x, -bonePos.y, -bonePos.z);
		D3DXMatrixTranslation(&fromOrigin, t.p.x, t.p.y, t.p.z);
		
		q = q1 * q2;

		D3DXMatrixRotationQuaternion(&boneTransform, &q);

		D3DXMATRIX RotateMatrix;
		D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(PositionFire),0,0);
		D3DXMatrixMultiply(&fromOrigin, &fromOrigin, &RotateMatrix);
		
		D3DXMatrixMultiply(&boneTransform, &toOrigin, &boneTransform);
		D3DXMatrixMultiply(&boneTransform, &boneTransform, &fromOrigin);

		r3dMatrix m2;
		m2.Assign( boneTransform._11, boneTransform._12, boneTransform._13,
				   boneTransform._21, boneTransform._22, boneTransform._23,
				   boneTransform._31, boneTransform._32, boneTransform._33 );
		b.vRelPlacement.Rotate( m2, outVector );
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////

bool VehicleDescriptor::Load(const r3dMesh *m)
{
	r3dSTLString skeletonPath = m->FileName.c_str();
	if (skeletonPath.size() < 3)
		return false;

	skeletonPath.replace(skeletonPath.size() - 3, 3, "skl");

	driveFileDefinitionPath = m->FileName.c_str();
	driveFileDefinitionPath.erase(driveFileDefinitionPath.size() - 4);
	driveFileDefinitionPath += "_DriveData.xml";

	r3dFile* f = r3d_open(driveFileDefinitionPath.c_str(), "rb");
	if ( !f )
	{
		r3dOutToLog("Failed to open vehicle definition file: %s\n", driveFileDefinitionPath.c_str());
		return false;
	}

	char* fileBuffer = game_new char[f->size + 1];
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlDoc;
	pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(fileBuffer, f->size);
	fclose(f);
	if (!parseResult)
		r3dError("Failed to parse XML, error: %s", parseResult.description());

	pugi::xml_node xml = xmlDoc.child("vehicle");

	// New Vehicle of ISS Game
	NewISSVehicle = 0;
	GetXMLVal("SpecialVehicle", xml, &NewISSVehicle);

	skl = game_new r3dSkeleton;
	skl->LoadBinary(skeletonPath.c_str());

	if (!InitSkeleton(skl))
		return false;

	wheelsData.Resize(numWheels);
	InitToDefault();

	GetXMLVal("hasTracks", xml, &hasTracks);
	GetXMLVal("durability", xml, &durability);
	GetXMLVal("armorExterior", xml, &armorExterior);
	GetXMLVal("armorInterior", xml, &armorInterior);
	GetXMLVal("fuel", xml, &maxFuel);
	GetXMLVal("weight", xml, &weight);
	GetXMLVal("tireFriction", xml, &tireFriction);	
	GetXMLVal("thresholdIgnoreMelee", xml, &thresholdIgnoreMelee);
	GetXMLVal("thresholdIgnoreBullets", xml, &thresholdIgnoreBullets);

	//	Ackerman data
	GetXMLVal("ackermann_accuracy", xml, &ackermannData.mAccuracy);;

	//	Sound data
	pugi::xml_node soundNode = xml.child("sounds");
	GetXMLVal("startEngine", soundNode, &startEngine);
	GetXMLVal("stopEngine", soundNode, &stopEngine);
	GetXMLVal("engineLoop", soundNode, &engineLoop);

	//	Gears data
	pugi::xml_node gearsNode = xml.child("gears");
	GetXMLVal("switch_time", gearsNode, &gearsData.mSwitchTime);
	gearsData.mNumRatios = gearsNode.attribute("num_ratios").as_uint();
	for(uint32_t i = 0; i < gearsData.mNumRatios; ++i)
	{
		char ratio[5];
		sprintf(ratio, "r%d", i);
		GetXMLVal(ratio, gearsNode, &gearsData.mRatios[ i ]);
	}
	GetXMLVal("final_ratio", gearsNode, &gearsData.mFinalRatio);

	// chassis data
	pugi::xml_node chassisNode = xml.child("chassis");
	GetXMLVal("mass", chassisNode, &chassisData.mMass);
	r3dVector v1(chassisData.mMOI.x, chassisData.mMOI.y, chassisData.mMOI.z);
	GetXMLVal("moi_offset", chassisNode, &v1);
	chassisData.mMOI = PxVec3(v1.x, v1.y, v1.z);
	r3dVector v2(chassisData.mCMOffset.x, chassisData.mCMOffset.y, chassisData.mCMOffset.z);
	GetXMLVal("cm_offset", chassisNode, &v2);
	chassisData.mCMOffset = PxVec3(v2.x, v2.y, v2.z);;

	//r3dOutToLog("VEHICLE CoM: %f, %f, %f\n", v2.x, v2.y, v2.z);

	//	Engine data
	pugi::xml_node engineNode = xml.child("engine");
	GetXMLVal("peak_torque", engineNode, &engineData.mPeakTorque);
	GetXMLVal("max_omega", engineNode, &engineData.mMaxOmega);
// 	GetXMLVal("damping_rate_full_throttle", engineNode, &engineData.mDampingRateFullThrottle);
// 	GetXMLVal("damping_rate_zero_throttle_clutch_disengaged", engineNode, &engineData.mDampingRateZeroThrottleClutchDisengaged);
// 	GetXMLVal("damping_rate_zero_throttle_clutch_engaged", engineNode, &engineData.mDampingRateZeroThrottleClutchEngaged);

	// Differential data
	pugi::xml_node diffNode = xml.child("differential");
	int type = diffData.mType;
	GetXMLVal("type", diffNode, &type);
	diffData.mType = type;
	GetXMLVal("front_rear_torque_split", diffNode, &diffData.mFrontRearSplit);
// 	GetXMLVal("front_left_right_torque_split", diffNode, &diffData.mFrontLeftRightSplit);
// 	GetXMLVal("rear_left_right_torque_split", diffNode, &diffData.mRearLeftRightSplit);

	//	Wheels data
	pugi::xml_node wheelNode = xml.child("wheel");

	struct WheelIndexSearcher
	{
		const char *nameToFind;
		WheelIndexSearcher(const char *n): nameToFind(n) {}
		bool operator() (const char *a)
		{
			return strcmpi(a, nameToFind) == 0;
		}
	};

	const char * const * start = VEHICLE_PART_NAMES + MAX_HULL_PARTS_COUNT;
	const char * const * end = start + MAX_WHEELS_COUNT;

	while (wheelNode)
	{
		const char * const *n = std::find_if(start, end, WheelIndexSearcher(wheelNode.attribute("val").value()));
		if (n != end)
		{
			size_t idx = std::distance(start, n);
			WheelData &wd = wheelsData[idx];
			
			PxVehicleWheelData &wheelData = wd.wheelData;
			GetXMLVal("radius_multiplier", wheelNode, &wheelData.mRadius);
			GetXMLVal("mass", wheelNode, &wheelData.mMass);
			GetXMLVal("damping_rate", wheelNode, &wheelData.mDampingRate);
			GetXMLVal("max_break_torque", wheelNode, &wheelData.mMaxBrakeTorque);
			GetXMLVal("max_hand_break_torque", wheelNode, &wheelData.mMaxHandBrakeTorque);
			GetXMLVal("max_steer_angle", wheelNode, &wheelData.mMaxSteer);
			wheelData.mMaxSteer = R3D_DEG2RAD(wheelData.mMaxSteer);
			GetXMLVal("toe_angle", wheelNode, &wheelData.mToeAngle);
			wheelData.mToeAngle = R3D_DEG2RAD(wheelData.mToeAngle);

			PxVehicleSuspensionData &suspensionData = wd.suspensionData;
			GetXMLVal("suspension_spring_strength", wheelNode, &suspensionData.mSpringStrength);
			GetXMLVal("suspension_spring_damper_rate", wheelNode, &suspensionData.mSpringDamperRate);
			GetXMLVal("suspension_max_compression", wheelNode, &suspensionData.mMaxCompression);
			GetXMLVal("suspension_max_droop", wheelNode, &suspensionData.mMaxDroop);

			r3dVector v1(wd.suspensionTravelDir.x, wd.suspensionTravelDir.y, wd.suspensionTravelDir.z);
			GetXMLVal("suspension_travel_dir", wheelNode, &v1);
			wd.suspensionTravelDir = PxVec3(v1.x, v1.y, v1.z);

			//GetXMLVal("tire_logitudal_stiffness_per_unit_gravity", wheelNode, &wd.tireData.mLongitudinalStiffnessPerUnitGravity);
		}
		wheelNode = wheelNode.next_sibling();
	}
	delete [] fileBuffer;
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool VehicleDescriptor::Save(const char *fileName)
{
	if (!fileName)
	{
		fileName = driveFileDefinitionPath.c_str();
	}

	pugi::xml_document xmlDoc;
	pugi::xml_node v = xmlDoc.append_child();
	v.set_name("vehicle");
	
	SetXMLVal("hasTracks", v, &hasTracks);
	SetXMLVal("durability", v, &durability);
	SetXMLVal("armorExterior", v, &armorExterior);
	SetXMLVal("armorInterior", v, &armorInterior);
	SetXMLVal("maxFuel", v, &maxFuel);
	SetXMLVal("weight", v, &weight);
	SetXMLVal("tireFriction", v, &tireFriction);
	SetXMLVal("thresholdIgnoreMelee", v, &thresholdIgnoreMelee);
	SetXMLVal("thresholdIgnoreBullets", v, &thresholdIgnoreBullets);

	SetXMLVal("ackermann_accuracy", v, &ackermannData.mAccuracy);

	// New Vehicle of ISS Game
	SetXMLVal("SpecialVehicle", v, &NewISSVehicle);

	pugi::xml_node sounds = v.append_child();
	sounds.set_name("sounds");
	SetXMLVal("startEngine", sounds, &startEngine);
	SetXMLVal("stopEngine", sounds, &stopEngine);
	SetXMLVal("engineLoop", sounds, &engineLoop);

	pugi::xml_node gears = v.append_child();
	gears.set_name("gears");
	SetXMLVal("switch_time", gears, &gearsData.mSwitchTime);
	gears.append_attribute("num_ratios") = gearsData.mNumRatios;
	for(uint32_t i = 0; i < gearsData.mNumRatios; ++i)
	{
		char ratio[5];
		sprintf(ratio, "r%d", i);
		SetXMLVal(ratio, gears, &gearsData.mRatios[ i ]);
	}
	SetXMLVal("final_ratio", gears, &gearsData.mFinalRatio);

	pugi::xml_node chassis = v.append_child();
	chassis.set_name("chassis");
	SetXMLVal("mass", chassis, &chassisData.mMass);

	pugi::xml_node engine = v.append_child();
	engine.set_name("engine");
	SetXMLVal("peak_torque", engine, &engineData.mPeakTorque);
	SetXMLVal("max_omega", engine, &engineData.mMaxOmega);

	pugi::xml_node differential = v.append_child();
	differential.set_name("differential");
	SetXMLVal("front_rear_torque_split", differential, &diffData.mFrontRearSplit);

	for (PxU32 i = 0; i < numWheels; ++i)
	{
		WheelData &wd = wheelsData[i];
		pugi::xml_node wheel = v.append_child();
		wheel.set_name("wheel");
		wheel.append_attribute("val").set_value(VEHICLE_PART_NAMES[i + MAX_HULL_PARTS_COUNT]);

		SetXMLVal("radius_multiplier", wheel, &wd.wheelData.mRadius);
		SetXMLVal("mass", wheel, &wd.wheelData.mMass);
		SetXMLVal("damping_rate", wheel, &wd.wheelData.mDampingRate);
		SetXMLVal("max_break_torque", wheel, &wd.wheelData.mMaxBrakeTorque);
		SetXMLVal("max_hand_break_torque", wheel, &wd.wheelData.mMaxHandBrakeTorque);
		float f2 = R3D_RAD2DEG(wd.wheelData.mMaxSteer);
		SetXMLVal("max_steer_angle", wheel, &f2);
		float f1 = R3D_RAD2DEG(wd.wheelData.mToeAngle);
		SetXMLVal("toe_angle", wheel, &f1);
		SetXMLVal("suspension_spring_strength", wheel, &wd.suspensionData.mSpringStrength);
		SetXMLVal("suspension_spring_damper_rate", wheel, &wd.suspensionData.mSpringDamperRate);
		SetXMLVal("suspension_max_compression", wheel, &wd.suspensionData.mMaxCompression);
		SetXMLVal("suspension_max_droop", wheel, &wd.suspensionData.mMaxDroop);
	}

	xmlDoc.save_file(fileName);
	extern float g_ShowSaveSign ;
	g_ShowSaveSign = r3dGetTime() + 2.2f ;

	return true;
}

//////////////////////////////////////////////////////////////////////////

void VehicleDescriptor::ConfigureVehicleSimulationData(PxVehicleDriveSimData4W *dd, PxVehicleWheelsSimData *wd)
{
	if (!vehicle && (!dd || !wd))
		return;

	if (!dd)
		dd = &vehicle->mDriveSimData;

	dd->setDiffData(diffData);
	dd->setAckermannGeometryData(ackermannData);
	dd->setClutchData(clutchData);
	dd->setEngineData(engineData);
	dd->setGearsData(gearsData);

	if (!wd)
		wd = &vehicle->mWheelsSimData;

	for (uint32_t i = 0; i < numWheels; ++i)
	{
		wd->setSuspensionData(i, wheelsData[i].suspensionData);
		wd->setSuspTravelDirection(i, wheelsData[i].suspensionTravelDir);
		wd->setTireData(i, wheelsData[i].tireData);
		wd->setWheelData(i, wheelsData[i].wheelData);
	}
}

#endif
//////////////////////////////////////////////////////////////////////////

void VehicleDescriptor::InitToDefault()
{
	gearsData.mNumRatios = 5;
	gearsData.mRatios[ 0 ] = -8.0f;
	gearsData.mRatios[ 1 ] = 0.0f;
	gearsData.mRatios[ 2 ] = 8.0f;
	gearsData.mRatios[ 3 ] = 4.0f;
	gearsData.mRatios[ 4 ] = 3.0f;
	gearsData.mFinalRatio = 4.0f;
	//gearsData.mRatios[ 0 ] = -3.38f;
	//gearsData.mRatios[ 1 ] = 0.0f;
	//gearsData.mRatios[ 2 ] = 2.97f;
	//gearsData.mRatios[ 3 ] = 2.07f;
	//gearsData.mRatios[ 4 ] = 1.43f;
	//gearsData.mRatios[ 5 ] = 1.0f;
	//gearsData.mFinalRatio = 1.0f;

	gearsData.mSwitchTime = 0.5f;
	diffData.mType = PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_4WD;

	for (PxU32 i = 0; i < numWheels; ++i)
	{
		PxVehicleWheelData &wd = wheelsData[i].wheelData;
		wd.mMaxBrakeTorque = 25000.0f;

		PxVehicleSuspensionData &sd = wheelsData[i].suspensionData;
		sd.mSpringStrength = 40000.0f;
		sd.mSpringDamperRate = 3000.0f;
		
#ifndef WO_SERVER
		wheelsData[i].wheelBone = &(skl->Bones[ wheelBonesRemapIndices[ i ] ]);
		wheelsData[i].DamperBone = &(skl->Bones[ DamperBonesRemapIndices[ i ] ]);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER

bool VehicleDescriptor::GetExitIndex( r3dVector& outVector, uint32_t exitIndex ) const
{
	const r3dBone *bonesStart = skl->Bones;
	const r3dBone *bonesEnd = skl->Bones + skl->NumBones;
	
	if (exitIndex > sizeof(VEHICLE_EXIT_NAMES))
		return false;

	const r3dBone *partName = std::find_if(bonesStart, bonesEnd, BoneSearcher(VEHICLE_EXIT_NAMES[exitIndex]));
	if (partName != bonesEnd)
	{
		size_t idx = std::distance(bonesStart, partName);
		
		r3dBone& b = skl->Bones[idx];
		PxTransform t = vehicle->getRigidDynamicActor()->getGlobalPose();
		D3DXQUATERNION q, q1, q2 = D3DXQUATERNION( t.q.x, t.q.y, t.q.z, t.q.w );
		D3DXQuaternionRotationMatrix( &q1, &b.CurrentTM );
		q = q1 * q2;
		D3DXMATRIX m1;
		D3DXMatrixRotationQuaternion( &m1, & q );
		r3dMatrix m2;
		m2.Assign( m1._11, m1._12, m1._13,
				   m1._21, m1._22, m1._23,
				   m1._31, m1._32, m1._33 );
		b.vRelPlacement.Rotate( m2, outVector );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

PxF32 VehicleDescriptor::GetSpeed()
{
	float speed = 0.0f;
	if (hasTracks == 0)
		speed = vehicle->computeForwardSpeed();
	else
		speed = tank->computeForwardSpeed();

	if (speed < 0.0f)
		speed = -speed;

	return speed;
}

PxF32 VehicleDescriptor::GetLongTireSlip(const PxU32 tireIndex)
{
	float value = 0.0f;
	if (hasTracks == 0)
		value = vehicle->mWheelsDynData.getTireLongSlip(tireIndex);
	else
		value = tank->mWheelsDynData.getTireLongSlip(tireIndex);

	if (value < 0.0f)
		value = -value;

	return value;
}

PxF32 VehicleDescriptor::GetLatTireSlip(const PxU32 tireIndex)
{
	float value = 0.0f;
	if (hasTracks == 0)
		value = vehicle->mWheelsDynData.getTireLatSlip(tireIndex);
	else
		value = tank->mWheelsDynData.getTireLatSlip(tireIndex);

	if (value < 0.0f)
		value = -value;

	return value;
}

PxF32 VehicleDescriptor::GetSideSpeed()
{
	float value = 0.0f;
	if (hasTracks == 0)
		value = vehicle->computeSidewaysSpeed();
	else
		value = tank->computeSidewaysSpeed();

	if (value < 0.0f)
		value = -value;

	return value;
}

//////////////////////////////////////////////////////////////////////////

PxReal VehicleDescriptor::GetEngineRpm()
{
	return hasTracks == 0 ? 
		vehicle->mDriveDynData.getEngineRotationSpeed() * 60.0f / (2 * R3D_PI) :
		tank->mDriveDynData.getEngineRotationSpeed() * 60.0f / (2 * R3D_PI);
}

#endif

//////////////////////////////////////////////////////////////////////////

PxReal VehicleDescriptor::GetMaxSpeed()
{
	// Returns Estimated Max Speed per Hour (this is slightly off, because we aren't computing tire's contact patch or rolling radius)
	// Assumption: All wheels are the same size.

	// The following simplifies to what is used.
	//PxReal maxRPMs = engineData.mMaxOmega * 60.0f / (2 * R3D_PI);
	//return (2 * R3D_PI * wheelsData[ 0 ].wheelData.mRadius * maxRPMs / (gearsData.mRatios[ gearsData.mNumRatios - 1 ] * gearsData.mFinalRatio)) / 60.0f;

	return wheelsData[ 0 ].wheelData.mRadius * engineData.mMaxOmega / (gearsData.mRatios[ gearsData.mNumRatios - 1 ] * gearsData.mFinalRatio);
}

//////////////////////////////////////////////////////////////////////////
#ifdef WO_SERVER

void VehicleDescriptor::ReadSerializedData(pugi::xml_node& node)
{
	pugi::xml_node xml = node.child("vehicle");

	wheelsData.Resize(numWheels);
	InitToDefault();

	GetXMLVal("hasTracks", xml, &hasTracks);
	GetXMLVal("durability", xml, &durability);
	GetXMLVal("armorExterior", xml, &armorExterior);
	GetXMLVal("armorInterior", xml, &armorInterior);
	GetXMLVal("fuel", xml, &maxFuel);
	GetXMLVal("weight", xml, &weight);
	GetXMLVal("tireFriction", xml, &tireFriction);
	GetXMLVal("thresholdIgnoreMelee", xml, &thresholdIgnoreMelee);
	GetXMLVal("thresholdIgnoreBullets", xml, &thresholdIgnoreBullets);

	//	Ackerman data
	GetXMLVal("ackermann_accuracy", xml, &ackermannData.mAccuracy);

	//	Gears data
	pugi::xml_node gearsNode = xml.child("gears");
	GetXMLVal("switch_time", gearsNode, &gearsData.mSwitchTime);
	gearsData.mNumRatios = gearsNode.attribute("num_ratios").as_uint();
	for(uint32_t i = 0; i < gearsData.mNumRatios; ++i)
	{
		char ratio[5];
		sprintf(ratio, "r%d", i);
		GetXMLVal(ratio, gearsNode, &gearsData.mRatios[ i ]);
	}
	GetXMLVal("final_ratio", gearsNode, &gearsData.mFinalRatio);

	// chassis data
	pugi::xml_node chassisNode = xml.child("chassis");
	GetXMLVal("mass", chassisNode, &chassisData.mMass);
	r3dVector v1(chassisData.mMOI.x, chassisData.mMOI.y, chassisData.mMOI.z);
	GetXMLVal("moi_offset", chassisNode, &v1);
	chassisData.mMOI = PxVec3(v1.x, v1.y, v1.z);
	r3dVector v2(chassisData.mCMOffset.x, chassisData.mCMOffset.y, chassisData.mCMOffset.z);
	GetXMLVal("cm_offset", chassisNode, &v2);
	chassisData.mCMOffset = PxVec3(v2.x, v2.y, v2.z);;

	//	Engine data
	pugi::xml_node engineNode = xml.child("engine");
	GetXMLVal("peak_torque", engineNode, &engineData.mPeakTorque);
	GetXMLVal("max_omega", engineNode, &engineData.mMaxOmega);
// 	GetXMLVal("damping_rate_full_throttle", engineNode, &engineData.mDampingRateFullThrottle);
// 	GetXMLVal("damping_rate_zero_throttle_clutch_disengaged", engineNode, &engineData.mDampingRateZeroThrottleClutchDisengaged);
// 	GetXMLVal("damping_rate_zero_throttle_clutch_engaged", engineNode, &engineData.mDampingRateZeroThrottleClutchEngaged);

	// Differential data
	pugi::xml_node diffNode = xml.child("differential");
	int type = diffData.mType;
	GetXMLVal("type", diffNode, &type);
	diffData.mType = type;
	GetXMLVal("front_rear_torque_split", diffNode, &diffData.mFrontRearSplit);
// 	GetXMLVal("front_left_right_torque_split", diffNode, &diffData.mFrontLeftRightSplit);
// 	GetXMLVal("rear_left_right_torque_split", diffNode, &diffData.mRearLeftRightSplit);

	//	Wheels data
	pugi::xml_node wheelNode = xml.child("wheel");

	struct WheelIndexSearcher
	{
		const char *nameToFind;
		WheelIndexSearcher(const char *n): nameToFind(n) {}
		bool operator() (const char *a)
		{
			return strcmpi(a, nameToFind) == 0;
		}
	};

	const char * const * start = VEHICLE_PART_NAMES + MAX_HULL_PARTS_COUNT;
	const char * const * end = start + MAX_WHEELS_COUNT;

	while (wheelNode)
	{
		const char * const *n = std::find_if(start, end, WheelIndexSearcher(wheelNode.attribute("val").value()));
		if (n != end)
		{
			size_t idx = std::distance(start, n);

#ifdef WO_SERVER
			// apparently we only care about the first wheel in the wheels data on the server, so just skip out
			if (idx >= numWheels)
				break;
#endif
			WheelData &wd = wheelsData[idx];
			
			PxVehicleWheelData &wheelData = wd.wheelData;
			GetXMLVal("radius_multiplier", wheelNode, &wheelData.mRadius);
			GetXMLVal("mass", wheelNode, &wheelData.mMass);
			GetXMLVal("damping_rate", wheelNode, &wheelData.mDampingRate);
			GetXMLVal("max_break_torque", wheelNode, &wheelData.mMaxBrakeTorque);
			GetXMLVal("max_hand_break_torque", wheelNode, &wheelData.mMaxHandBrakeTorque);
			GetXMLVal("max_steer_angle", wheelNode, &wheelData.mMaxSteer);
			wheelData.mMaxSteer = R3D_DEG2RAD(wheelData.mMaxSteer);
			GetXMLVal("toe_angle", wheelNode, &wheelData.mToeAngle);
			wheelData.mToeAngle = R3D_DEG2RAD(wheelData.mToeAngle);

			PxVehicleSuspensionData &suspensionData = wd.suspensionData;
			GetXMLVal("suspension_spring_strength", wheelNode, &suspensionData.mSpringStrength);
			GetXMLVal("suspension_spring_damper_rate", wheelNode, &suspensionData.mSpringDamperRate);
			GetXMLVal("suspension_max_compression", wheelNode, &suspensionData.mMaxCompression);
			GetXMLVal("suspension_max_droop", wheelNode, &suspensionData.mMaxDroop);

			r3dVector v1(wd.suspensionTravelDir.x, wd.suspensionTravelDir.y, wd.suspensionTravelDir.z);
			GetXMLVal("suspension_travel_dir", wheelNode, &v1);
			wd.suspensionTravelDir = PxVec3(v1.x, v1.y, v1.z);

			//GetXMLVal("tire_logitudal_stiffness_per_unit_gravity", wheelNode, &wd.tireData.mLongitudinalStiffnessPerUnitGravity);
		}
		wheelNode = wheelNode.next_sibling();
	}
}

#endif

#endif VEHICLES_ENABLED
