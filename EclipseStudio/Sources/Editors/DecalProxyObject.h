//=========================================================================
//	Module: DecalProxyObject.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "..\..\..\GameEngine\gameobjects\GameObj.h"

//////////////////////////////////////////////////////////////////////////

class DecalGameObjectProxy: public GameObject
{
	DECLARE_CLASS(DecalGameObjectProxy, GameObject)
	UINT selectedDecalIdx;
	UINT decalType;

public:
	DecalGameObjectProxy();

	virtual BOOL		OnPositionChanged();			// ObjMan: when position changed
	virtual	BOOL		OnOrientationChanged();			// ObjMan: when direction changed
	void				SelectDecal(UINT typeIdx, UINT idx);
	virtual	void 		SetPosition(const r3dPoint3D& pos);
	virtual void		SetRotationVector(const r3dVector& Angles);
};
