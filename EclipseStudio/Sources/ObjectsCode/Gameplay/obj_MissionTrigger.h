//=========================================================================
//	Module: obj_MissionTrigger.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#pragma once

#if defined(MISSIONS) && defined(MISSION_TRIGGERS)

#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "SharedUsableItem.h"

class obj_MissionTrigger : public SharedUsableItem
{
	DECLARE_CLASS(obj_MissionTrigger, SharedUsableItem)

	static const int IDLength = 16;

public:
	bool		m_GotData;
	uint32_t	m_missionID;
	char		m_missionTitleID[ IDLength ];
	char		m_missionNameID[ IDLength ];
	char		m_missionDescID[ IDLength ];
	
public:
	obj_MissionTrigger();
	virtual ~obj_MissionTrigger();

#ifndef FINAL_BUILD
	virtual float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected);
#endif
	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

};
#endif
