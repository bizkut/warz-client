//=========================================================================
//	Module: obj_Traps.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "SharedUsableItem.h"
#include "multiplayer/P2PMessages.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/Lamp.h"

class obj_Traps : public SharedUsableItem
{
	DECLARE_CLASS(obj_Traps, SharedUsableItem)
public:

	uint32_t			m_ItemID;
	float				m_RotX;
	r3dMesh* 	ActualMesh;
	r3dMesh* 	ActivateMesh;
	bool		m_Activated;

	class obj_ParticleSystem* particlefire;
	class obj_LightHelper* LightFire;

public:
	obj_Traps();
	virtual ~obj_Traps();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
	void	Deactivate();
	void	Activate(bool isme);
	void	ExeParticle();
};
