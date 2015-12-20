//=========================================================================
//	Module: obj_Door.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "SharedUsableItem.h"
#include "multiplayer/P2PMessages.h"

class obj_Door : public SharedUsableItem
{
	DECLARE_CLASS(obj_Door, SharedUsableItem)
public:

	bool		m_OpenDoor;
	r3dVector	OriginalRotation;
	float		OpenOrClose;
	int			SelectDoor;
	BYTE		DoorIsDestroy;
	bool		RemovePhyxDoor;
	bool		EnablePhyxDoor;

public:
	obj_Door();
	virtual ~obj_Door();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
	void	Set(bool Open);
	void	SelectionDoor(int Selection);
	void	ExeParticle();

	virtual void WriteSerializedData(pugi::xml_node& node);
	virtual void ReadSerializedData(pugi::xml_node& node);

	void CloneParameters(obj_Door *o);
	virtual	GameObject * Clone();

#ifndef FINAL_BUILD
	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected);
#endif
};
