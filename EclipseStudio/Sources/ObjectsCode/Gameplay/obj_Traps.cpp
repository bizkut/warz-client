//=========================================================================
//	Module: obj_Traps.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#include "obj_Traps.h"
#include "../../EclipseStudio/Sources/multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"
#include "../../EclipseStudio/Sources/objectscode/weapons/weaponconfig.h"
#include "../../EclipseStudio/Sources/ObjectsCode/EFFECTS/obj_ParticleSystem.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_Traps, "obj_Traps", "Object");
AUTOREGISTER_CLASS(obj_Traps);


obj_Traps::obj_Traps()
	: m_ItemID( -1 )
{
	DisablePhysX = false;
	m_RotX = 0;
	ActualMesh = NULL;
	m_Activated = false;
}

obj_Traps::~obj_Traps()
{
}

BOOL obj_Traps::Load(const char *fname)
{
	return TRUE;
}

BOOL obj_Traps::OnCreate()
{
	m_ActionUI_Title = gLangMngr.getString("ActivateTramp");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToActivateTramp");


	switch (m_ItemID)
	{
		case WeaponConfig::ITEMID_Traps_Bear:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_traps_bear_01_armed_tps.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_traps_bear_01_disarmed_tps.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_Traps_Spikes:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_traps_spikes_01_armed_tps.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_traps_spikes_01_disarmed_tps.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_Campfire:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_prop_campfire_01.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_prop_campfire_01.sco", true, false, true, true );
				particlefire = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "vehicle_damage_02", GetPosition());
				LightFire = (obj_LightHelper*)srv_CreateGameObject("obj_LightHelper", "Omni", GetPosition() + r3dPoint3D(0,0.61,0));
				LightFire->LT.Intensity = 3.8f;
				LightFire->bOn = true;
				LightFire->innerRadius = 0.0f;
				LightFire->outerRadius = 3.0f;
				LightFire->bKilled = false;
				break;
	}
	parent::MeshLOD[0] = ActualMesh;
	m_spawnPos = GetPosition();

	SAFE_DELETE(PhysicsObject);
	ReadPhysicsConfig();
	PhysicsConfig.group = PHYSCOLL_STATIC_GEOMETRY; // skip collision with players
	PhysicsConfig.requireNoBounceMaterial = true;
	PhysicsConfig.isFastMoving = true;
	if (m_ItemID != WeaponConfig::ITEMID_Campfire)
	PhysicsConfig.isKinematic = true;

	/*if(DisablePhysX)
	{
		SAFE_DELETE(PhysicsObject);
		ReadPhysicsConfig();
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false;
	}*/
	SetRotationVector(r3dPoint3D(m_RotX, 0, 0));
	return parent::OnCreate();
}

BOOL obj_Traps::OnDestroy()
{
		if(particlefire)
			particlefire->bKillDelayed = 1;
	if (LightFire != NULL)
	{
		LightFire = NULL;
	}
	return parent::OnDestroy();
}

BOOL obj_Traps::Update()
{
	return parent::Update();
}

void obj_Traps::Deactivate()
{
	parent::MeshLOD[0] = ActualMesh;
	SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Items/Trap/TrapSound"),GetPosition());
	//ExeParticle();
	m_Activated = false;
}

void obj_Traps::Activate(bool isme)
{
	parent::MeshLOD[0] = ActivateMesh;
	m_Activated = true;
	if (isme)
	{
		PKT_S2C_SetupTraps_s n;
		n.spawnID = toP2pNetId(GetNetworkID());
		n.m_Activate = 1;
		p2pSendToHost(NULL, &n, sizeof(n));
	}
}

void obj_Traps::ExeParticle()
{
	SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_Riot"), GetPosition(), r3dPoint3D(0,1,0));
}