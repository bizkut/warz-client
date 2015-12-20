#pragma once

#include "Grenade.h"

// data driven class. It is created by Ammo class, for bullets that are not immediate action, for example rockets.
class obj_Flare : public obj_Grenade
{
	DECLARE_CLASS(obj_Flare, obj_Grenade)
	friend Ammo;

	void*	m_sndThrow;
	void*	m_sndBurn;

	float	m_HangTime;
	float	m_LastTimeUpdateCalled;
protected:
	virtual void		OnExplode();
public:
	obj_Flare();

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();
	virtual	BOOL		Update();

};
