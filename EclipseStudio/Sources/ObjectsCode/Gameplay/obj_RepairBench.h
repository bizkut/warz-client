#pragma once

#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "SharedUsableItem.h"

class obj_RepairBench : public SharedUsableItem
{
	DECLARE_CLASS(obj_RepairBench, SharedUsableItem)
public:

public:
	obj_RepairBench();
	virtual ~obj_RepairBench();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};
