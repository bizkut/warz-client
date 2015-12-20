//=========================================================================
//	Module: DecalProxyObject.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#ifndef FINAL_BUILD

#include "DecalProxyObject.h"
#include "..\ObjectsCode\world\DecalChief.h"
#include "ObjectManipulator3d.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(DecalGameObjectProxy, "DecalGameObjectProxy", "Object");
AUTOREGISTER_CLASS(DecalGameObjectProxy);

//-------------------------------------------------------------------------
//	class DecalGameObjectProxy
//-------------------------------------------------------------------------

DecalGameObjectProxy::DecalGameObjectProxy()
: selectedDecalIdx(INVALID_DECAL_ID)
, decalType(INVALID_DECAL_ID)
{
	ObjTypeFlags |= OBJTYPE_DecalProxy;
	m_isSerializable = false; // do not allow this object to save itself
}

//////////////////////////////////////////////////////////////////////////

BOOL DecalGameObjectProxy::OnPositionChanged()
{
	GameObject::OnPositionChanged();

	if (decalType == INVALID_DECAL_ID)
		return true;

	const r3dVector& pos = GetPosition();
	const DecalParams *dp = g_pDecalChief->GetStaticDecal(decalType, selectedDecalIdx);
	if (!dp)
		return false;

	DecalParams p = *dp;
	p.Pos = pos;
	g_pDecalChief->UpdateStaticDecal(p, selectedDecalIdx);

	return true;
}

//////////////////////////////////////////////////////////////////////////

BOOL DecalGameObjectProxy::OnOrientationChanged()
{
	GameObject::OnOrientationChanged();

	if (decalType == INVALID_DECAL_ID)
		return true;

	const r3dVector& angles = GetRotationVector();
	const DecalParams *dp = g_pDecalChief->GetStaticDecal(decalType, selectedDecalIdx);
	if (!dp)
		return false;

	DecalParams p = *dp;
	p.ZRot = R3D_DEG2RAD(angles.x);
	g_pDecalChief->UpdateStaticDecal(p, selectedDecalIdx);
	return true;
}

//////////////////////////////////////////////////////////////////////////

void DecalGameObjectProxy::SelectDecal(UINT typeIdx, UINT idx)
{
	selectedDecalIdx = idx;
	decalType = typeIdx;

	const DecalParams *p = g_pDecalChief->GetStaticDecal(decalType, selectedDecalIdx);
	if (!p)
		return;

	SetPosition(r3dPoint3D(p->Pos.x, p->Pos.y, p->Pos.z));
	r3dVector angles( R3D_RAD2DEG( p->ZRot ), 0.f, 0.f ) ;
	SetRotationVector( angles ) ;

	const DecalType& dt = g_pDecalChief->GetTypeByIdx( p->TypeID ) ;

	float s = sqrtf( dt.ScaleX * dt.ScaleX + dt.ScaleY * dt.ScaleY ) * 0.5f * p->ScaleCoef ;

	r3dBoundBox bbox;
	bbox.Org	= r3dPoint3D( -s, -s, -s );
	bbox.Size.x	= s * 2;
	bbox.Size.y	= s * 2;
	bbox.Size.z	= s * 2;

	SetBBoxLocal(bbox);
}

//////////////////////////////////////////////////////////////////////////

void DecalGameObjectProxy::SetPosition(const r3dPoint3D& pos)
{
	GameObject::SetPosition(pos);
	OnPositionChanged();
}

//////////////////////////////////////////////////////////////////////////

void DecalGameObjectProxy::SetRotationVector(const r3dVector& Angles)
{
	GameObject::SetRotationVector(Angles);
	OnOrientationChanged();
}

//////////////////////////////////////////////////////////////////////////

#endif // FINAL BUILD