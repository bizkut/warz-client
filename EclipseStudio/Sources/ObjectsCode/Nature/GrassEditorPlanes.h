//=========================================================================
//	Module: GrassEditorPlanes.h
//	Copyright (C) 2012.
//=========================================================================

#pragma once

#include "..\..\..\..\GameEngine\gameobjects\GameObj.h"

//////////////////////////////////////////////////////////////////////////

const int INVALID_GRASS_PLANE_INDEX = -1;

class GrassPlane
{
	r3dBoundBox bounds;
	
	void UpdateGrassMap();

public:
	GrassPlane();

	/**
	* Calculate position (x, z) height (y) that lie on rectangle specified by points.
	* If given coordinates reside outside of plane rect, return -FLT_MAX
	*/
	float GetHeight(float x, float z) const;

	void SetOrigin(const r3dPoint3D &pos, bool updateGrassMap);
	const r3dBoundBox & GetBounds() const { return bounds; }
	void SetSize(float x, float z, bool updateGrassMap);
	bool IsPointInside(float x, float z) const;

	void Draw() const;

	r3dString name;
};

//////////////////////////////////////////////////////////////////////////

class GrassPlanesManager
{
	r3dTL::TArray<GrassPlane> planes;

public:
	GrassPlanesManager();
	~GrassPlanesManager();

	/**	Return dummy object if no plane is found. */
	GrassPlane & GetPlane(const r3dString &name);
	GrassPlane & GetPlane(int idx);
	GrassPlane & AddPlane();
	void DeletePlane(int idx);
	uint32_t GetPlanesCount() const { return planes.Count(); }

	bool Save(const r3dString& levelHomeDir);
	bool Load(const r3dString& levelHomeDir);

	/**	Return INVALID_PLANE_INDEX if plane with specific name is not found.  */
	int	GetPlaneIndex(const r3dString &name) const;

	/**	Find plane that cover requested coordinates and return height of it. */
	float GetHeight(float x, float z) const;

	void DrawPlanes() const;

	void GetCombinedPlaneBounds( r3dBoundBox* oBox );
};

GrassPlanesManager * GetGrassPlaneManager();
void FreeGrassPlaneManager();
extern float g_NoTerrainLevelBaseHeight;

//////////////////////////////////////////////////////////////////////////

class GrassPlaneGameObjectProxy: public GameObject
{
	DECLARE_CLASS(GrassPlaneGameObjectProxy, GameObject)
	int planeIdx;
	bool resizeMode;

public:
	GrassPlaneGameObjectProxy();

	void				SelectGrassPlane(int idx);
	virtual	void 		SetPosition(const r3dPoint3D& pos);
	virtual void		OnPickerActionEnd();
	void				SetResizeMode(bool doResizeMode);
};
