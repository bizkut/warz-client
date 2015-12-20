#pragma once

#include "GameCommon.h"
#include "GameCode/UserProfile.h"

// object that is lying on the ground and player can interact with it. 
// this is base class for such objects
class SharedUsableItem : public MeshGameObject
{
public:
	bool		DisablePhysX; // for permanent notes

	float			m_lastPickupAttempt; // time until you cannot try to pick up item again
	float		m_StayStillTime;
	float		m_NotSleepTime;

	const char*	 m_ActionUI_Title;
	const char*	 m_ActionUI_Msg;

	bool	m_IsAirDrop;

public:
	SharedUsableItem();
	virtual ~SharedUsableItem();

	virtual	BOOL		Update();

protected:
	r3dPoint3D			m_spawnPos;
};

typedef r3dgameList(SharedUsableItem*) SharedUsableItemList;
extern SharedUsableItemList m_SharedUsableItemList;
