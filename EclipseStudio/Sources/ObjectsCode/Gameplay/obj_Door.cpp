//=========================================================================
//	Module: obj_Door.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#include "obj_Door.h"
#include "../../EclipseStudio/Sources/multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_Door, "obj_Door", "Object");
AUTOREGISTER_CLASS(obj_Door);


obj_Door::obj_Door()
	: m_OpenDoor( false )
{
	DisablePhysX = false;
	SelectDoor = 0;
	RemovePhyxDoor = false;
	EnablePhyxDoor = false;
	DoorIsDestroy = 0;
	OriginalRotation.x = 0;
	OriginalRotation.y = 0;
	OriginalRotation.z = 0;
}

obj_Door::~obj_Door()
{
}

BOOL obj_Door::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\WZ_Doors\\Door.sco";

	if(!parent::Load(cpMeshName))
		return FALSE;

	return TRUE;
}

BOOL obj_Door::OnCreate()
{
	m_ActionUI_Title = gLangMngr.getString("OpenDoor");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToOpenDoor");

	m_spawnPos = GetPosition();
	OpenOrClose = 0.0f;

	if(!g_bEditMode)
	{
		SetNetworkID(gClientLogic().net_lastFreeId++);
	}

	SelectionDoor(SelectDoor);

	if(DisablePhysX)
	{
		ReadPhysicsConfig();
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false;
	}
	SetRotationVector(OriginalRotation);
	return parent::OnCreate();
}

BOOL obj_Door::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_Door::Update()
{
	if (RemovePhyxDoor)
	{
		ReadPhysicsConfig();
		SAFE_DELETE(PhysicsObject);
		ReadPhysicsConfig();
		SetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows); 
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false;
		RemovePhyxDoor = false;
	}
	if (EnablePhyxDoor)
	{
		ReadPhysicsConfig();
		SAFE_DELETE(PhysicsObject);
		ReadPhysicsConfig();
		ResetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows);
		PhysicsConfig.group = PHYSCOLL_STATIC_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = true;
		PhysicsConfig.isFastMoving = true;
		EnablePhyxDoor=false;
	}
	if (m_OpenDoor==true)
	{
		m_ActionUI_Title = gLangMngr.getString("CloseDoor");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToCloseDoor");
		if (OpenOrClose<90)
		{
			OpenOrClose+=10.0f;
			SetRotationVector(OriginalRotation+r3dVector(OpenOrClose,0,0));
		}
	}
	else
	{
		m_ActionUI_Title = gLangMngr.getString("OpenDoor");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToOpenDoor");
		if (OpenOrClose>0.0)
		{
			OpenOrClose-=10.0f;
			SetRotationVector(OriginalRotation+r3dVector(OpenOrClose,0,0));
		}
	}

	return parent::Update();
}
void obj_Door::Set(bool Open)
{
	m_OpenDoor=Open;
	if (m_OpenDoor == true)
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Effects/Door/OpenDoor"),GetPosition());
	else
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Effects/Door/CloseDoor"),GetPosition());

	PKT_S2C_SendDataDoor_s n;
	n.DoorID = toP2pNetId(GetNetworkID());
	n.m_OpenDoor = m_OpenDoor;
	n.DoorIsDestroy = DoorIsDestroy;
	n.ExecuteParticle = 0;
	p2pSendToHost(this, &n, sizeof(n));
}

void obj_Door::SelectionDoor(int Selection)
{
	switch(Selection)
	{
		case 0:
			parent::MeshLOD[0] = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Doors\\Door.sco");
			break;
		case 1:
			parent::MeshLOD[0] = r3dGOBAddMesh("Data\\ObjectsDepot\\WZ_Doors\\Door2.sco");
	}
}

void obj_Door::ExeParticle()
{
	SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_Wood"), GetPosition(), r3dPoint3D(0,1,0));
}

void obj_Door::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node cNode = node.child("Door");
	SelectDoor = cNode.attribute("Number").as_int();
	pugi::xml_node cNodeRot = node.child("gameObject");
	OriginalRotation.x = cNodeRot.child("rotation").attribute("x").as_float();
}

void obj_Door::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node cNode = node.append_child();
	cNode.set_name("Door");
	cNode.append_attribute("Number") = SelectDoor;
}

GameObject * obj_Door::Clone()
{
	obj_Door * newSpawnPoint = static_cast<obj_Door*>(srv_CreateGameObject("obj_Door", "Data\\ObjectsDepot\\WZ_Doors\\Door.sco", GetPosition()));
	newSpawnPoint->CloneParameters(this);
	return newSpawnPoint;
}

//////////////////////////////////////////////////////////////////////////

void obj_Door::CloneParameters(obj_Door *o)
{
	m_spawnPos = o->m_spawnPos;
	OpenOrClose = o->OpenOrClose;
	SelectionDoor(o->SelectDoor);
	SetRotationVector(o->OriginalRotation);
}

#ifndef FINAL_BUILD
//#define EXTENDED_VEHICLE_CONFIG
float obj_Door::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry + parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected );

	if( !IsParentOrEqual( &ClassData, startClass ))
		return y;

	y += 10.0f;
	y += imgui_Static(scrx, y, "Door Selection");
	y += imgui_Value_SliderI(scrx, y, "Door Number", &SelectDoor, 0, 1, "%d");

	SelectionDoor(SelectDoor);

	return y - scry;
}
#endif