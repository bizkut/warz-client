//=========================================================================
//	Module: obj_LightMesh.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "Building.h"

//////////////////////////////////////////////////////////////////////////

class obj_LightMesh: public obj_Building
{
	DECLARE_CLASS(obj_LightMesh, obj_Building)

	/** Array of attached lights object id */
	typedef r3dTL::TArray<gobjid_t> LightIDs;
	LightIDs lightIDs;

	GameObject * CreateLight(const char *fn);
	bool destroyMeshOnKillLight;
	r3dColor killedLightObjColor;
	void DeleteLight(uint32_t idx);

	void DestroyLights();
#ifndef FINAL_BUILD
	int selectedLight;
#endif

public:
	void KillLight() ;
	void ResurrectLight() ;
	bool isLightOn();

public:
	obj_LightMesh();
	virtual ~obj_LightMesh();
	gobjid_t GetLightObjectID(uint32_t idx = 0) const;
	virtual	void SetPosition(const r3dPoint3D& pos);
	virtual void ReadSerializedData(pugi::xml_node& node);
	virtual void WriteSerializedData(pugi::xml_node& node);
#ifndef FINAL_BUILD
	virtual	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif
	virtual	BOOL OnCreate();
	virtual GameObject *Clone();
	virtual void SetObjFlags(int objFlags);

	virtual BOOL OnNetReceive(DWORD EventID, const void* packetData, int packetSize) OVERRIDE;

	// NOTE : disable staticness derived from obj_Building as this object can be damaged and thus should go to non-static list
	virtual int	IsStatic() OVERRIDE;
};