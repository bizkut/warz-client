//=========================================================================
//	Module: CollectionElement.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "gameobjects/PhysObj.h"
#include "gameobjects/GameObj.h"
#ifndef WO_SERVER
#include "gameobjects/GenericQuadTree.h"
#endif

//////////////////////////////////////////////////////////////////////////

class CollectionPhysicsCallbackObject: public PhysicsCallbackObject
{
public:
	CollectionPhysicsCallbackObject(): mesh(NULL) {}

	virtual	void OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace) {}; // do nothing
	virtual GameObject* isGameObject() { return NULL; }
	virtual r3dMesh* hasMesh() { return mesh; }

	r3dMesh* mesh;
};

//////////////////////////////////////////////////////////////////////////

struct CollectionElementSerializableData
{
	r3dPoint3D pos;
	float scale;
	int tint;
	float angle;
	int typeIndex;

	void LoadFromMemory(const char *buf);
	void SaveToMemory(char *buf) const;
	CollectionElementSerializableData();
};

//////////////////////////////////////////////////////////////////////////

struct CollectionElement: public CollectionElementSerializableData
{
	PhysicsObjectConfig physicsConfig;
	ShadowExtrusionData shadowExData;

	BasePhysicsObject *physObj;
	CollectionPhysicsCallbackObject *physCallbackObj;

	float bendSpeed;
	float bendVal;
	float windPower;

	float randomColor;
	bool shadowExDataDirty;
	bool wasVisible;

	int curLod;

	D3DXMATRIX rotMat;

	CollectionElement();
	~CollectionElement();
	void InitPhysicsData();
	void ClearPhysicsData();
	void LoadFromMemory(const char *buf);

#ifndef WO_SERVER
	r3dBoundBox GetWorldAABB() const;
	void GenerateRandomColor();
	void RecalcRotation();
#endif
};

//////////////////////////////////////////////////////////////////////////

/**	This function needed for quadtree class. */
#ifndef WO_SERVER
r3dBoundBox CollectionElement_GetAABB(QuadTreeObjectID objId);
#endif

