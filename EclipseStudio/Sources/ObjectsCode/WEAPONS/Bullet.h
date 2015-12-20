#ifndef __BULLET_H__
#define __BULLET_H__

#include "GameCommon.h"
#include "Ammo.h"

class obj_Bullet : public AmmoShared
{
	DECLARE_CLASS(obj_Bullet, AmmoShared)
	friend Ammo;
	int m_DamageFromPiercable;
public:
	obj_Bullet();

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();
	virtual	BOOL		Update();

	bool OnHit( PxSweepHit &hit );

	virtual r3dMesh*	GetObjectMesh();
	virtual r3dMesh*	GetObjectLodMesh() OVERRIDE;
	virtual r3dMesh*	getModel();
};


#endif	// __BULLET_H__
