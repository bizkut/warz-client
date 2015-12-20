//=========================================================================
//	Module: obj_Gravestone.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "SharedUsableItem.h"

class obj_Gravestone : public SharedUsableItem
{
	DECLARE_CLASS(obj_Gravestone, SharedUsableItem)
public:
	// Enum must match obj_ServerGravestone::EKilledBy enums
	enum EKilledBy
	{
		 KilledBy_Self = 0
		,KilledBy_Zombie
		,KilledBy_Player

		,KilledBy_Unknown = 15
	};

	bool		m_GotData;
	uint8_t			m_KilledBy;
	uint8_t			m_GravestoneType;
	r3dSTLString	m_Aggressor;
	r3dSTLString	m_Victim;
	
public:
	obj_Gravestone();
	virtual ~obj_Gravestone();

	virtual void		PreLoadObjectData(void* data);
	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
};
