//=========================================================================
//	Module: obj_NoClipPlane.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "gameobjects/GameObj.h"

#ifndef FINAL_BUILD

#if !defined(WO_SERVER) && !defined(FINAL_BUILD)
struct NoClipPlaneData
{
	r3dPoint3D points[4];
};
#endif // !defined(WO_SERVER) && !defined(FINAL_BUILD)


class obj_NoClipPlane : public GameObject
{
	DECLARE_CLASS(obj_NoClipPlane, GameObject)

public:

#if !defined(WO_SERVER) && !defined(FINAL_BUILD)
	static r3dgameVector(NoClipPlaneData) noClipDatas;
#endif // !defined(WO_SERVER) && !defined(FINAL_BUILD)

public:
	obj_NoClipPlane();
	~obj_NoClipPlane();

	virtual BOOL Update();
	virtual BOOL OnCreate();
	virtual BOOL OnDestroy();

	void GetPoints(r3dPoint3D (&points)[4]);

	void ReadSerializedData(pugi::xml_node& node);
#ifndef WO_SERVER
	void WriteSerializedData(pugi::xml_node& node);

	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected );

	virtual void AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam  );
#endif // WO_SERVER
};

#endif