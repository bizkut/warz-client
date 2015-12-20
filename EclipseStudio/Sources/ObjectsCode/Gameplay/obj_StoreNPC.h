#pragma once

#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "obj_Note.h"

class obj_BaseNPC : public SharedUsableItem
{
	DECLARE_CLASS(obj_BaseNPC, SharedUsableItem)
private:
	r3dSkeleton*	m_BindSkeleton; 
	r3dAnimPool		m_AnimPool;
	r3dAnimation	m_Animation;

protected:
	const char*		m_NPCMeshName;
	const char*		m_NPCAnimName;

public:
	obj_BaseNPC();
	virtual ~obj_BaseNPC();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();

	virtual	BOOL		Update();

	void			DrawAnimated(const r3dCamera& Cam, bool shadow_pass);

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendShadowRenderables( RenderArray& rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;

	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};

class obj_StoreNPC : public obj_BaseNPC
{
	DECLARE_CLASS(obj_StoreNPC, obj_BaseNPC)
public:
	obj_StoreNPC();
};

class obj_VaultNPC : public obj_BaseNPC
{
	DECLARE_CLASS(obj_VaultNPC, obj_BaseNPC)
public:
	obj_VaultNPC();
};
