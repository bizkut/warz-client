//=========================================================================
//	Module: obj_FakeGlass.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "gameobjects/GameObj.h"

class obj_FakeGlass : public GameObject
{

	DECLARE_CLASS(obj_FakeGlass, GameObject)


public:
	obj_FakeGlass();
	~obj_FakeGlass();

	PrecalculatedMeshShaderConsts	m_FakeGlassMeshVSConsts;

	bool m_changeglass;

	char MeshFilenameGlass[512];

	void SetFakeGlassObject(GameObject* obj);
	void SetInteriorGlass(bool enable);
	virtual BOOL Update();
	virtual BOOL OnCreate();
	virtual BOOL OnDestroy();

	D3DXMATRIX		DrawRotMatrix;	// character rotation matrix
	uint32_t		m_FakeGlassID; // itemID of what shield to draw
	r3dMesh*		m_FakeGlass;
	r3dPoint3D		m_FakeGlassPosition;
	r3dVector		m_FakeGlassRotation;

	virtual void AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam  );
};