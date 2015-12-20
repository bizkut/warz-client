//=========================================================================
//	Module: obj_AnimalDummy.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "gameobjects/obj_Mesh.h"

#define ANIMAL_VARIANTS_COUNT 2

//////////////////////////////////////////////////////////////////////////
class r3dPhysSkeleton;
class r3dPhysDogSkeleton;

class obj_AnimalDummy : public GameObject
{
	DECLARE_CLASS(obj_AnimalDummy, GameObject)

public:
	r3dAnimation	anim_;

	//r3dPhysSkeleton* physSkeleton;
	r3dPhysDogSkeleton* physSkeleton;

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
	obj_AnimalDummy();
	~obj_AnimalDummy();

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

