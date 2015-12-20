//=========================================================================
//	Module: obj_Apex.hpp
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#if APEX_ENABLED

#include "GameCommon.h"
#include "ApexActor.h"


//////////////////////////////////////////////////////////////////////////

class obj_ApexDestructible: public GameObject
{
	DECLARE_CLASS(obj_ApexDestructible, GameObject)
	bool makePreviewActor;

public:
	obj_ApexDestructible();
	~obj_ApexDestructible();
	virtual BOOL Update();
	virtual void AppendRenderables(RenderArray(&render_arrays)[rsCount], const r3dCamera& Cam);
	virtual void AppendShadowRenderables(RenderArray &rarr, int sliceIndex,  const r3dCamera& Cam);
	virtual BOOL OnCreate();
	virtual void SetPosition(const r3dPoint3D& pos);
	virtual void SetRotationVector(const r3dVector& Angles);
#ifndef FINAL_BUILD
	virtual	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	virtual	BOOL Load(const char* name);
	virtual	GameObject * Clone();
	virtual	void ReadSerializedData(pugi::xml_node& node);
	virtual void WriteSerializedData(pugi::xml_node& node);

	/**	Physics callback overrides. */
	virtual r3dMaterial * GetMaterial(uint32_t faceID);

	void ConvertToDestructible();
	void ConvertToPreview();

	ApexActorBase *destructibleActor;
};

#endif