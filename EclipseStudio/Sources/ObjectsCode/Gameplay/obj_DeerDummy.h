//=========================================================================
//	Module: obj_DeerDummy.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "gameobjects/obj_Mesh.h"

#define DEER_VARIANTS_COUNT 2

//////////////////////////////////////////////////////////////////////////
class r3dPhysSkeleton;

class obj_DeerDummy : public GameObject
{
	DECLARE_CLASS(obj_DeerDummy, GameObject)

public:
	r3dAnimation	anim_;

	r3dPhysSkeleton* physSkeleton;

	int			variant;
	r3dMesh*	mesh;
	void		ReloadMesh();
	
	char		sAnimSelected[128];
	float		fAnimListOffset;
	void		SwitchToSelectedAnim();
	int			AddAnimation(const char* anim);
	
	float		AnimSpeed;
	float		WalkSpeed;
	int			Walking;
	int			ApplyWalkSpeed;
	int			WalkIdx;
	r3dPoint3D	SpawnPos;
	
	void		DoHit();

public:
	obj_DeerDummy();
	~obj_DeerDummy();

	virtual	BOOL OnCreate();
	virtual BOOL Update();
	virtual BOOL OnDestroy();
	virtual void OnPreRender();

	virtual void AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam);
	virtual void AppendRenderables(RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam);

#ifndef FINAL_BUILD
	virtual	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);
};

