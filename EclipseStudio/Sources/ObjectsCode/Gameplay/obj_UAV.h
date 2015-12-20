#pragma once

#include "GameCommon.h"
#include "ObjectsCode\World\Building.h"
#include "multiplayer/NetCellMover.h"
#include "APIScaleformGfx.h"

class obj_UAV : public obj_Building
{
	DECLARE_CLASS(obj_UAV, obj_Building)
 
 public:
	void		UpdatePositionBounds();

	r3dPoint3D	ViewAngle;
	r3dPoint3D	vVision;

	enum EUAVState {
		UAV_Active,
		UAV_Damaged,
		UAV_Killed,
	};
	EUAVState	state_;

	bool UaVisDestroy;

	void	*UavSnd;

	Scaleform::GFx::Value m_TagIcon;
	
	CNetCellMover	netMover;
	r3dPoint3D	netVelocity;
	void		UpdatePositionFromRemote();
	
	bool		isDamaged;
	gobjid_t	damageFx;
	void		SetDamagedState();
	
	r3dPoint3D	vDrift[2];
	float		tDrift[2];

virtual	BOOL		OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	void		 OnNetPacket(const PKT_C2C_MoveSetCell_s& n);
	void		 OnNetPacket(const PKT_C2C_MoveRel_s& n);
	void		 OnNetPacket(const PKT_S2C_UAVSetState_s& n);
virtual	void	 SetVelocity(const r3dPoint3D& vel);

 public:
	obj_UAV();
virtual ~obj_UAV();

virtual	BOOL		Load(const char *name);

virtual	BOOL		OnCreate();
virtual	BOOL		OnDestroy();

virtual	BOOL		Update();
};
