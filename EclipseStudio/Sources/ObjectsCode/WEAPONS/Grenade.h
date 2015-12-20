#pragma once

#include "GameCommon.h"
#include "Ammo.h"
#include "APIScaleformGfx.h"

#include "multiplayer/P2PMessages.h"
#include "multiplayer/NetCellMover.h"

// data driven class. It is created by Ammo class, for bullets that are not immediate action, for example rockets.
class obj_Grenade : public AmmoShared
{
	DECLARE_CLASS(obj_Grenade, AmmoShared)
	friend Ammo;
protected:
	r3dPoint3D			lastCollisionNormal;
	bool				bHadCollided;
	virtual void		OnExplode();
public:
	obj_Grenade();

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();
	virtual	void		OnCollide(PhysicsCallbackObject *tobj, CollisionInfo &trace);
	virtual	BOOL		Update();
	void				ProcessMovement();

	virtual r3dMesh*	GetObjectMesh();
	virtual r3dMesh*	GetObjectLodMesh() OVERRIDE;

	virtual	BOOL		OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	void				OnNetPacket(const PKT_C2C_MoveSetCell_s& n);
	void				OnNetPacket(const PKT_C2C_MoveRel_s& n);
	void				OnNetPacket(const PKT_S2C_Explosion_s& n);

public:
	uint32_t			m_ItemID;
	CNetCellMover		m_NetMover;
	bool				m_bExplode;
protected:
	bool				m_bIgnoreNetMover;	// For Flare-Gun flares we don't sync the position when they are falling, because the update rates of upward impulses are causing them to fall at different rates.
};
