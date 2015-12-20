#pragma once

#include "Grenade.h"

// data driven class. It is created by Ammo class, for bullets that are not immediate action, for example rockets.
class obj_ChemLight : public obj_Grenade
{
	DECLARE_CLASS(obj_ChemLight, obj_Grenade)
	friend Ammo;
protected:
	virtual void		OnExplode();
public:
	obj_ChemLight();
};
