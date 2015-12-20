//=========================================================================
//	Module: obj_ZombieDummy.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#include "gameobjects/obj_Mesh.h"

#define ZOMBIE_BODY_PARTS_COUNT 3

//////////////////////////////////////////////////////////////////////////
class r3dPhysSkeleton;

class obj_ZombieDummy : public GameObject
{
	DECLARE_CLASS(obj_ZombieDummy, GameObject)

	bool	m_isSuperZombie;
	int		m_typeIndex;		// 0 - normal zombie, 1 - super zombie

public:
	r3dAnimation	anim_;

	r3dPhysSkeleton* physSkeleton;

	int		partIds[ZOMBIE_BODY_PARTS_COUNT];
	r3dMesh*	zombieParts[ZOMBIE_BODY_PARTS_COUNT];
	void		ReloadZombiePart(int type, int slot);
	
	char		sAnimSelected[128];
	float		fAnimListOffset;
	void		SwitchToSelectedAnim();
	int		AddAnimation(const char* anim);
	
	float		AnimSpeed;
	float		WalkSpeed;
	int		Walking;
	int		ApplyWalkSpeed;
	int		WalkIdx;
	r3dPoint3D	SpawnPos;
	
	void		DoHit();

public:
	obj_ZombieDummy();
	~obj_ZombieDummy();

	virtual	BOOL OnCreate();
	virtual BOOL Update();
	virtual BOOL OnDestroy();
	virtual void OnPreRender();
#ifndef FINAL_BUILD
	void DrawDebugInfo() const;
#endif

	virtual void AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam);
	virtual void AppendRenderables(RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam);

#ifndef FINAL_BUILD
	virtual	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);
};

