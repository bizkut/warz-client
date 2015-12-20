//=========================================================================
//	Module: obj_Gravestone.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#include "obj_Gravestone.h"

IMPLEMENT_CLASS(obj_Gravestone, "obj_Gravestone", "Object");
AUTOREGISTER_CLASS(obj_Gravestone);

obj_Gravestone::obj_Gravestone()
	: m_GotData( false )
	, m_GravestoneType( 0 )
	, m_KilledBy( KilledBy_Unknown )
{
	DisablePhysX = true;
}

obj_Gravestone::~obj_Gravestone()
{
}

void obj_Gravestone::PreLoadObjectData( void* data )
{
	m_GravestoneType	= *((BYTE*)data) & 0x03;
	m_KilledBy			= ( *((BYTE*)data) >> 2 ) & 0x0F;
}

BOOL obj_Gravestone::Load(const char *fname)
{
	const char* cpMeshName;
	switch( m_GravestoneType )
	{
	default:
	case 0:	cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_Gravestone_01.sco";	break;
	case 1:	cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_Gravestone_02.sco";	break;
	case 2:	cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_Gravestone_03.sco";	break;
	};

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_Gravestone::OnCreate()
{
	m_ActionUI_Title = gLangMngr.getString("Gravestone");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToReadGravestone");

	m_spawnPos = GetPosition();

	if(DisablePhysX)
	{
		ReadPhysicsConfig();
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false;
	}

	return parent::OnCreate();
}

BOOL obj_Gravestone::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_Gravestone::Update()
{
	return parent::Update();
}
