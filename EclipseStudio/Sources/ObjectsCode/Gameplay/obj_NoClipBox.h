//=========================================================================
//	Module: obj_NoClipbox.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "gameobjects/GameObj.h"

#ifndef FINAL_BUILD

// TODO: Change NoClipData to only be on the Server when we get the FairFight API.
#if !defined(WO_SERVER) && !defined(FINAL_BUILD)
struct NoClipData
{
	r3dPoint3D points[8];
};
#endif // !defined(WO_SERVER) && !defined(FINAL_BUILD)

class obj_NoClipBox : public GameObject
{

	DECLARE_CLASS(obj_NoClipBox, GameObject)

public:
// TODO: Change s_vNoClipData to only be on the Server when we get the FairFight API.
#if !defined(WO_SERVER) && !defined(FINAL_BUILD)
	static r3dgameVector(NoClipData) s_vNoClipData;
#endif // !defined(WO_SERVER) && !defined(FINAL_BUILD)

public:
	obj_NoClipBox();
	~obj_NoClipBox();

	virtual BOOL Update();
	virtual BOOL OnCreate();
	virtual BOOL OnDestroy();

	void GetPoints(r3dPoint3D (&points)[8]);

	void ReadSerializedData(pugi::xml_node& node);
#ifndef WO_SERVER
	void WriteSerializedData(pugi::xml_node& node);

	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected );
	
	virtual void AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam  );
#endif // WO_SERVER
};
#endif