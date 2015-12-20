//=========================================================================
//	Module: obj_Sprite.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

#include "GameObj.h"

class obj_Sprite: public MeshGameObject
{
	DECLARE_CLASS(obj_Sprite, MeshGameObject)
	r3dMaterial *customMaterial;

	void LoadSpriteCustomMaterial(const char *name);
	void MakeMeshUnique();

public:
	obj_Sprite();
	~obj_Sprite();

	virtual void UpdateTransform();
	virtual BOOL Update();
	virtual GameObject * Clone();
	virtual	BOOL Load(const char* name);

	virtual void ReadSerializedData(pugi::xml_node& node);
	virtual void WriteSerializedData(pugi::xml_node& node);

#ifndef FINAL_BUILD
	virtual	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif
};

