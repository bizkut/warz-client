//=========================================================================
//	Module: CollectionElementProxyObject.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#ifndef FINAL_BUILD

#include "CollectionElementProxyObject.h"
#include "ObjectManipulator3d.h"
#include "CollectionsManager.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CollectionElementObjectProxy, "CollectionElementObjectProxy", "Object");
AUTOREGISTER_CLASS(CollectionElementObjectProxy);

//-------------------------------------------------------------------------
//	class DecalGameObjectProxy
//-------------------------------------------------------------------------

CollectionElementObjectProxy::CollectionElementObjectProxy()
: delAttachedElement(true)
, collectionElementIdx(INVALID_COLLECTION_INDEX)
{
	m_isSerializable = false; // do not allow this object to save itself
	ObjTypeFlags |= OBJTYPE_CollectionProxy;
}

//////////////////////////////////////////////////////////////////////////

void CollectionElementObjectProxy::SetPosition(const r3dPoint3D& pos)
{
	r3dPoint3D oldPos = GetPosition();

	parent::SetPosition(pos);

	CollectionElement *ce = gCollectionsManager.GetElement(collectionElementIdx);

	if (ce)
	{
		ce->pos = pos;
		gCollectionsManager.UpdateElementQuadTreePlacement(collectionElementIdx);
	}
}

//////////////////////////////////////////////////////////////////////////

void CollectionElementObjectProxy::SetRotationVector(const r3dVector& Angles)
{
	parent::SetRotationVector(Angles);
	CollectionElement * ce = gCollectionsManager.GetElement(collectionElementIdx);
	if (ce)
	{
		ce->angle = Angles.x;
		ce->RecalcRotation();
	}
}

//////////////////////////////////////////////////////////////////////////

bool CollectionElementObjectProxy::AssignToCollectionElement(int idx)
{
	const CollectionElement *ce = gCollectionsManager.GetElement(idx);
	if (!ce)
		return false;

	CollectionType * ct = gCollectionsManager.GetType(ce->typeIndex);
	if (!ct || !ct->meshLOD[0])
		return false;

	r3dBoundBox bb = ct->meshLOD[0]->localBBox;

	SetBBoxLocal(bb);
	parent::SetPosition(ce->pos);
	float angle = ce->angle;
	r3dVector rotV(angle, 0, 0);
	parent::SetRotationVector(rotV);
	collectionElementIdx = idx;
	return true;
}

//////////////////////////////////////////////////////////////////////////

BOOL CollectionElementObjectProxy::OnDestroy()
{
	CollectionElement *ce = gCollectionsManager.GetElement(collectionElementIdx);
	if (ce && delAttachedElement)
	{
		gCollectionsManager.DeleteElement(collectionElementIdx);
	}
	return parent::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

r3dMesh * CollectionElementObjectProxy::GetMesh() const
{
	CollectionElement *ce = gCollectionsManager.GetElement(collectionElementIdx);
	if (!ce)
		return 0;

	CollectionType *ct = gCollectionsManager.GetType(ce->typeIndex);
	if (!ct)
		return 0;

	return ct->meshLOD[0];
}

//////////////////////////////////////////////////////////////////////////

CollectionElement * CollectionElementObjectProxy::GetCollectionElement()
{
	return gCollectionsManager.GetElement(collectionElementIdx);
}

//-------------------------------------------------------------------------
//	Standalone functions
//-------------------------------------------------------------------------

void CreateProxyObjectsForAllCollections()
{
	DeleteProxyObjectsForCollections();

	int total = 0;
	//	Create proxy objects for all collection elements
	for (uint32_t i = 0; i < gCollectionsManager.GetElementsCount(); ++i)
	{
		CreateProxyForCollectionElement(i);
	}
}

//////////////////////////////////////////////////////////////////////////

void DeleteProxyObjectsForCollections()
{
	GameObject* obj = GameWorld().GetFirstObject();
	while (obj)
	{
		GameObject *nextObj = GameWorld().GetNextObject(obj);
		if (obj->isObjType(OBJTYPE_CollectionProxy))
		{
			CollectionElementObjectProxy *ce = static_cast<CollectionElementObjectProxy*>(obj);
			ce->delAttachedElement = false;
			GameWorld().DeleteObject(obj);
		}
		obj = nextObj;
	}
}

//////////////////////////////////////////////////////////////////////////

void CreateProxyForCollectionElement(int idx)
{
	const CollectionElement *ce = gCollectionsManager.GetElement(idx);
	if (!ce)
		return;

	CollectionElementObjectProxy* cpe = (CollectionElementObjectProxy*)srv_CreateGameObject("CollectionElementObjectProxy", "objCollectionElementObjectProxy", ce->pos);
	if (!cpe->AssignToCollectionElement(idx))
	{
		GameWorld().DeleteObject(cpe);
	}
}

//////////////////////////////////////////////////////////////////////////

void DeleteProxyForCollectionElement(int idx)
{
	for (GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if (obj->isObjType(OBJTYPE_CollectionProxy))
		{
			CollectionElementObjectProxy *ceop = static_cast<CollectionElementObjectProxy*>(obj);
			if (ceop->collectionElementIdx == idx)
			{
				ceop->delAttachedElement = false;
				GameWorld().DeleteObject(obj);
				break;
			}
		}
	}
}

#endif