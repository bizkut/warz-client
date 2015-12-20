//=========================================================================
//	Module: obj_MissionTrigger.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_MissionTrigger.h"

#if defined(MISSIONS) && defined(MISSION_TRIGGERS)
IMPLEMENT_CLASS(obj_MissionTrigger, "obj_MissionTrigger", "Object");
AUTOREGISTER_CLASS(obj_MissionTrigger);

obj_MissionTrigger::obj_MissionTrigger()
	: m_GotData( false )
	, m_missionID( UINT_MAX )
{
	serializeFile = SF_ServerData;
	DisablePhysX = true;
	memset( m_missionTitleID, 0, IDLength );
	memset( m_missionNameID, 0, IDLength );
	memset( m_missionDescID, 0, IDLength );
}

obj_MissionTrigger::~obj_MissionTrigger()
{
}

#ifndef FINAL_BUILD
float obj_MissionTrigger::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry;

	y += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);
	y += 5.0f;

	y += imgui_Static(scrx, y, "Mission Trigger parameters:");

	int missionID = (int)m_missionID;
	imgui_Value_SliderI(scrx, y, "Mission ID:", &missionID, 0, 999999, "%d", false);
	m_missionID = (uint32_t)missionID;
	y += 26; 
	imgui2_StringValueEx(scrx, y, 360, "Title StringID:", m_missionTitleID);
	y += 26; 
	imgui2_StringValueEx(scrx, y, 360, "Name StringID:", m_missionNameID);
	y += 26; 
	imgui2_StringValueEx(scrx, y, 360, "Desc StringID:", m_missionDescID);
	y += 26; 

	return y - scry;
}
#endif

BOOL obj_MissionTrigger::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_MsgBottle.sco";
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_MissionTrigger::OnCreate()
{
	if(!PhysicsConfig.ready)
		ReadPhysicsConfig();
	PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
	PhysicsConfig.isDynamic = false;
	PhysicsConfig.isFastMoving = false;

	if(parent::OnCreate())
	{
		m_GotData = true;
		m_ActionUI_Title = gLangMngr.getString( m_missionTitleID );
		m_ActionUI_Msg = gLangMngr.getString("HoldEToReadNote"); // TODO: Create and change to HoldEToReadMissionDesc

		m_spawnPos = GetPosition();

		r3dOutToLog("obj_MissionTrigger created. %d %s %s %s\n", m_missionID, m_missionTitleID, m_missionNameID, m_missionDescID);
		return TRUE;
	}
	return FALSE;
}

BOOL obj_MissionTrigger::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_MissionTrigger::Update()
{
	return parent::Update();
}

void obj_MissionTrigger::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData( node );
	m_missionID = node.attribute("missionID").as_uint();
	r3dscpy( m_missionTitleID, node.attribute("titleID").value() );
	r3dscpy( m_missionNameID, node.attribute("nameID").value() );
	r3dscpy( m_missionDescID, node.attribute("descID").value() );

	r3d_assert(m_missionID > 0);
}

void obj_MissionTrigger::WriteSerializedData(pugi::xml_node& node)
{
	r3d_assert(m_missionID > 0);

	parent::WriteSerializedData(node);
	node.append_attribute("missionID")	= m_missionID;
	node.append_attribute("titleID")	= m_missionTitleID;
	node.append_attribute("nameID")		= m_missionNameID;
	node.append_attribute("descID")		= m_missionDescID;
}
#endif