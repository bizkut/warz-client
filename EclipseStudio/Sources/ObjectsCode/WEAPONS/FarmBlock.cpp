#include "r3dPCH.h"
#include "r3d.h"

#include "FarmBlock.h"
#include "WeaponConfig.h"
#include "WeaponArmory.h"
#include "..\ai\AI_Player.H"
#include "..\..\multiplayer\ClientGameLogic.h"


IMPLEMENT_CLASS(obj_FarmBlock, "obj_FarmBlock", "Object");
AUTOREGISTER_CLASS(obj_FarmBlock);

obj_FarmBlock::obj_FarmBlock()
{
	m_ItemID = -1;
	m_RotX = 0;
	m_TimeUntilRipe = 0;
	m_AllowAsyncLoading = 1;
	m_Activated = false;
}

obj_FarmBlock::~obj_FarmBlock()
{

}

BOOL obj_FarmBlock::Load(const char *fname)
{
	return TRUE;
}

BOOL obj_FarmBlock::OnCreate()
{
	r3d_assert(m_ItemID > 0);
	const char* cpMeshName = "";
	if(m_ItemID == WeaponConfig::ITEMID_FarmBlock)
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\Block_Farm_01.sco";
	if(m_ItemID == WeaponConfig::ITEMID_SolarWaterPurifier)
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\Block_SolarWater_01.sco";
	if(m_ItemID == WeaponConfig::ITEMID_GardenTrap_Rabbit)
		cpMeshName = "Data\\ObjectsDepot\\WZ_Consumables\\inb_gardenTrap_rabbit_01_Open.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	m_ActionUI_Title = g_pWeaponArmory->getConfig(m_ItemID)->m_StoreName; // save to copy ptr here, item config should always be there
	m_ActionUI_Msg = gLangMngr.getString("HoldEToHarvest");

	ReadPhysicsConfig();

	//PhysicsConfig.group = PHYSCOLL_NETWORKPLAYER;
	//PhysicsConfig.isDynamic = 0;

	SetBBoxLocal( GetObjectMesh()->localBBox ) ;

	// raycast and see where the ground is and place dropped box there
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
	if(g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y+1, GetPosition().z), PxVec3(0, -1, 0), 50.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
	{
		SetPosition(r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z));
		SetRotationVector(r3dPoint3D(m_RotX, 0, 0));
	}

	m_spawnPos = GetPosition();

	UpdateTransform();

	return parent::OnCreate();
}

BOOL obj_FarmBlock::OnDestroy()
{
	return parent::OnDestroy();
}

void obj_FarmBlock::Deactivate()
{
	parent::MeshLOD[0] = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_gardenTrap_rabbit_01_Open.sco", true, false, true, true );
	m_Activated = false;
}

void obj_FarmBlock::Activate(bool isme)
{
	parent::MeshLOD[0] = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Consumables\\inb_gardenTrap_rabbit_01_closed.sco", true, false, true, true );
	m_Activated = true;
	if (isme)
	{
		PKT_S2C_SetupTraps_s n;
		n.spawnID = toP2pNetId(GetNetworkID());
		n.m_Activate = 1;
		p2pSendToHost(NULL, &n, sizeof(n));
	}
}

BOOL obj_FarmBlock::Update()
{
	if(m_TimeUntilRipe > 0)
	{
		m_TimeUntilRipe = R3D_MAX(m_TimeUntilRipe-r3dGetFrameTime(), 0.0f);
	}
	return parent::Update();
}
