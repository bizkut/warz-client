//=========================================================================
//	Module: CollectionElementProxyObject.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once
#include "../../../GameEngine/gameobjects/GameObj.h"
#include "LevelEditor_Collections.h"
#include "CollectionElement.h"

#ifndef FINAL_BUILD

//////////////////////////////////////////////////////////////////////////

class CollectionElementObjectProxy: public GameObject
{
	friend void DeleteProxyObjectsForCollections();

	DECLARE_CLASS(CollectionElementObjectProxy, GameObject)
	int collectionElementIdx;
	bool delAttachedElement;

public:
	CollectionElementObjectProxy();

	bool				AssignToCollectionElement(int idx);

	virtual	void 		SetPosition(const r3dPoint3D& pos);
	virtual void		SetRotationVector(const r3dVector& Angles);
	virtual BOOL		OnDestroy();
	r3dMesh	*			GetMesh() const;
	CollectionElement * GetCollectionElement();
};

//////////////////////////////////////////////////////////////////////////

void CreateProxyObjectsForAllCollections();
void DeleteProxyObjectsForCollections();
void CreateProxyForCollectionElement(int idx);
void DeleteProxyForCollectionElement(int idx);

#endif