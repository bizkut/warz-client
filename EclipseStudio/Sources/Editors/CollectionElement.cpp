//=========================================================================
//	Module: CollectionElement.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "CollectionsManager.h"
#include "CollectionType.h"

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
extern float g_fCollectionsRandomColor;

namespace
{
	D3DXMATRIX GetRotationFromTerrainNormalAtPos(const r3dPoint3D &worldPos)
	{
		r3dPoint3D T(0, 0, 0), B(0, 0, 0), N(0, 0, 0);
		Terrain->GetNormalTangentBitangent(worldPos, &N, &T, &B);

		D3DXMATRIX rv;
		D3DXMatrixIdentity(&rv);

		rv._11 = T.x;
		rv._12 = T.y;
		rv._13 = T.z;

		rv._21 = B.x;
		rv._22 = B.y;
		rv._23 = B.z;

		rv._31 = N.x;
		rv._32 = N.y;
		rv._33 = N.z;

		D3DXMATRIX rot, rot2;
		D3DXMatrixRotationZ(&rot, -D3DX_PI / 2.85f);
		D3DXMatrixRotationX(&rot2, D3DX_PI / 2);
		D3DXMatrixMultiply(&rot, &rot2, &rot);
		D3DXMatrixMultiply(&rv, &rot, &rv);

		return rv;
	}
}
#endif

//-------------------------------------------------------------------------
//	CollectionElementSerializableData class
//-------------------------------------------------------------------------

CollectionElementSerializableData::CollectionElementSerializableData()
: pos(0, 0, 0)
, scale(1.0f)
, angle(0)
, tint(0)
, typeIndex(INVALID_COLLECTION_INDEX)
{

}

//////////////////////////////////////////////////////////////////////////

void CollectionElementSerializableData::LoadFromMemory(const char *buf)
{
	size_t size = sizeof(CollectionElementSerializableData);
	memcpy_s(this, size, buf, size);
}

//////////////////////////////////////////////////////////////////////////

void CollectionElementSerializableData::SaveToMemory(char *buf) const
{
	size_t size = sizeof(CollectionElementSerializableData);
	memcpy_s(buf, size, this, size);
}

//-------------------------------------------------------------------------
//	CollectionElement class
//-------------------------------------------------------------------------

CollectionElement::CollectionElement()
: physObj(0)
, physCallbackObj(0)
, shadowExDataDirty(true)
, wasVisible(false)
, bendVal(0.5f)
, bendSpeed(0)
, windPower(0)
, randomColor(0)
, curLod(0)
{
	D3DXMatrixIdentity(&rotMat);
}

//////////////////////////////////////////////////////////////////////////

CollectionElement::~CollectionElement()
{
}

//////////////////////////////////////////////////////////////////////////

void CollectionElement::InitPhysicsData()
{
	CollectionType *ct = gCollectionsManager.GetType(typeIndex);

#ifndef WO_SERVER
	bool hasMesh = ct->meshLOD[0]!=NULL;
#else
	bool hasMesh = ct->serverMeshName[0]!=0;
#endif

	if (ct && hasMesh && ct->physicsEnable && !physObj)
	{
		PhysicsObjectConfig physicsConfig;
#ifndef WO_SERVER
		GameObject::LoadPhysicsConfig(ct->meshLOD[0]->FileName.c_str(), physicsConfig);
#else
		GameObject::LoadPhysicsConfig(ct->serverMeshName, physicsConfig);
#endif

		D3DXMATRIX rotation = rotMat;
		r3dVector size(5, 5, 5);

		physCallbackObj = game_new CollectionPhysicsCallbackObject();
#ifndef WO_SERVER
		physCallbackObj->mesh = ct->meshLOD[0];
#else
		physCallbackObj->mesh = NULL; // it is okay to pass NULL into createStaticObject for PhysX
#endif
		physObj = BasePhysicsObject::CreateStaticObject(physicsConfig, physCallbackObj, &pos, &size, physCallbackObj->mesh, &rotation);
		if (physObj)
		{
			physObj->SetScale(r3dPoint3D(scale, scale, scale));
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CollectionElement::ClearPhysicsData()
{
	SAFE_DELETE(physObj);
	SAFE_DELETE(physCallbackObj);
}

//////////////////////////////////////////////////////////////////////////

void CollectionElement::LoadFromMemory(const char *buf)
{
	CollectionElementSerializableData::LoadFromMemory(buf);
#ifndef WO_SERVER
	RecalcRotation();
#else
	D3DXMatrixIdentity(&rotMat);
	D3DXMATRIX rotY;
	D3DXMatrixRotationY(&rotY, R3D_DEG2RAD(angle));
	D3DXMatrixMultiply(&rotMat, &rotY, &rotMat);
#endif
}

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
r3dBoundBox CollectionElement::GetWorldAABB() const
{
	r3dBoundBox bb;
	bb.Org  = r3dPoint3D(0, 0, 0);
	bb.Size = r3dPoint3D(0, 0, 0);

	CollectionType * ct = gCollectionsManager.GetType(typeIndex);
	if (ct && ct->meshLOD[0])
	{
		bb = ct->meshLOD[0]->localBBox;

		D3DXMATRIX final, scaling;
		D3DXMatrixScaling(&scaling, scale, scale, scale);
		D3DXMatrixMultiply(&final, &scaling, &rotMat);

		bb.Transform(reinterpret_cast<r3dMatrix*>(&final));
		bb.Org += pos;
	}
	return bb;
}

//////////////////////////////////////////////////////////////////////////


void CollectionElement::GenerateRandomColor()
{
	randomColor = u_GetRandom(1.0f - g_fCollectionsRandomColor, 1.0f);
}

//////////////////////////////////////////////////////////////////////////

void CollectionElement::RecalcRotation()
{
	CollectionType * ct = gCollectionsManager.GetType(typeIndex);
	if (!ct)
		return;

	if (ct->terrainAligned)
	{
		rotMat = GetRotationFromTerrainNormalAtPos(pos);
	}
	else
	{
		D3DXMatrixIdentity(&rotMat);
	}
	D3DXMATRIX rotY;
	D3DXMatrixRotationY(&rotY, R3D_DEG2RAD(angle));
	D3DXMatrixMultiply(&rotMat, &rotY, &rotMat);
}

#endif
//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
r3dBoundBox CollectionElement_GetAABB(QuadTreeObjectID id)
{
	r3dBoundBox bb;
	bb.Org  = r3dPoint3D(0, 0, 0);
	bb.Size = r3dPoint3D(0, 0, 0);

	CollectionElement *e = gCollectionsManager.GetElement(id);
	if (!e)
		return bb;

	return e->GetWorldAABB();
}
#endif
