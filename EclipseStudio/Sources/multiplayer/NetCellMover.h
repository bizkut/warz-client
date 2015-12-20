#pragma once

#include "multiplayer/P2PMessages.h"

class CNetCellMover
{
  protected:
	GameObject*	owner;
	
	float		updateDelta;
	float		nextUpdate;
	float		cellSize;	// radius of cell
	
	// last sended values of moveData_s
	struct netMoveData_s {
	  r3dPoint3D	cell;		// center position of last sended cell
	  r3dPoint3D	pos;
	  BYTE		turnAngle;
	  BYTE		bendAngle;
	  USHORT	state;
	};
	netMoveData_s	lastMd;
	bool		IsDataChanged(const netMoveData_s& md);
	
  public:
	// input-output move data
	struct moveData_s {
	  r3dPoint3D	pos;
	  float		turnAngle;
	  float		bendAngle;
	  USHORT	state;
	};
		
  public:	  
	CNetCellMover(GameObject* in_owner, float in_updateDelta, float in_cellSize)
	{
		owner	    = in_owner;
		updateDelta = in_updateDelta;
		cellSize    = in_cellSize;
		nextUpdate  = 0;
		lastRecv    = 0;

		lastMd.pos        = r3dPoint3D(-99999, -99999, -99999);
		lastMd.cell       = r3dPoint3D(-99999, -99999, -99999);
		lastMd.turnAngle  = 0;
		lastMd.bendAngle  = 0;
		lastMd.state      = 0xFFFF;

		lastMove.pos       = r3dPoint3D(-99999, -99999, -99999);
		lastMove.bendAngle = 0.0f;
		lastMove.turnAngle = 0.0f;
		lastMove.state     = 0;
	};

	// local functions, build movement update packet
	DWORD		SendPosUpdate(const moveData_s& in_data, PKT_C2C_MoveSetCell_s* n1, PKT_C2C_MoveRel_s* n2);

	// remote functions
	float		lastRecv;	// last time network update was received
	void		SetCell(const PKT_C2C_MoveSetCell_s& n);
	const moveData_s& DecodeMove(const PKT_C2C_MoveRel_s& n);
	
  	moveData_s	lastMove;	// last data from network update
	const moveData_s& NetData() {
		r3d_assert(!owner->NetworkLocal);
		return lastMove;
	}
	const r3dPoint3D& GetNetPos() {
		return NetData().pos;
	}
	void		SetStartCell(const r3dPoint3D& pos) {
		r3d_assert(!owner->NetworkLocal);
		lastMd.cell = pos;
		lastMove.pos = pos;
	}
	
	r3dPoint3D	GetVelocityToNetTarget(const r3dPoint3D& pos, float chase_speed, float teleport_delta_sec);

	// reset mover state to specific position	
	void		Teleport(const r3dPoint3D& pos) {
		lastMd.pos   = r3dPoint3D(-99999, -99999, -99999);
		lastMd.cell  = r3dPoint3D(-99999, -99999, -99999);
		lastMove.pos = pos;
	}

#ifdef WO_SERVER	
	// server helper functions - only it can directly modify cell
	const r3dPoint3D&	SrvGetCell() const { 
		return lastMd.cell;
	}
	void			SrvSetCell(const r3dPoint3D& pos) { 
		lastMd.cell = pos;
	}
#endif
};

#ifdef VEHICLES_ENABLED	

class CVehicleNetCellMover : public CNetCellMover
{
private:
	struct netMoveData_s 
	{
		r3dPoint3D	cell;
		r3dPoint3D	pos;
		r3dPoint3D	rot;
		USHORT	state;
	};

	netMoveData_s lastMd;
	bool IsDataChanged(const netMoveData_s& md);

public:
	struct moveData_s 
	{
		r3dPoint3D	pos;
		r3dPoint3D	rot;
		USHORT	state;
	};

public:
	CVehicleNetCellMover(GameObject* in_owner, float in_updateDelta, float in_cellSize) :
	  	CNetCellMover(in_owner, in_updateDelta, in_cellSize)
	{
		owner	    = in_owner;
		updateDelta = in_updateDelta;
		cellSize    = in_cellSize;
		nextUpdate  = 0;
		lastRecv    = 0;

		lastMd.pos        = r3dPoint3D(-99999, -99999, -99999);
		lastMd.cell       = r3dPoint3D(-99999, -99999, -99999);
		lastMd.rot		  = r3dPoint3D(0, 0 , 0);
		lastMd.state      = 0xFFFF;

		lastMove.pos       = r3dPoint3D(-99999, -99999, -99999);
		lastMove.rot	   = r3dPoint3D(0, 0 , 0);
		lastMove.state     = 0;
	};

	DWORD SendPosUpdate(const moveData_s& in_data, PKT_C2C_VehicleMoveSetCell_s* n1, PKT_C2C_VehicleMoveRel_s* n2);
	void SetCell(const PKT_C2C_VehicleMoveSetCell_s& n);
	const moveData_s& DecodeMove(const PKT_C2C_VehicleMoveRel_s& n);
	r3dPoint3D	GetVelocityToNetTarget(const r3dPoint3D& pos, float chase_speed, float teleport_delta_sec);

	moveData_s lastMove;
	const moveData_s& VehicleNetData() 
	{
		r3d_assert(!owner->NetworkLocal);
		return lastMove;
	}

	const r3dPoint3D& GetVehicleNetPos()
	{
		return VehicleNetData().pos;
	}

	void SetStartCell(const r3dPoint3D& pos) 
	{
		r3d_assert(!owner->NetworkLocal);
		lastMd.cell = pos;
		lastMove.pos = pos;
	}

	void Teleport(const r3dPoint3D& pos) 
	{
		lastMd.pos   = r3dPoint3D(-99999, -99999, -99999);
		lastMd.cell  = r3dPoint3D(-99999, -99999, -99999);
		lastMove.pos = pos;
	}

#ifdef WO_SERVER	
	const r3dPoint3D& SrvGetCell() const 
	{ 
		return lastMd.cell;
	}
	void SrvSetCell(const r3dPoint3D& pos) 
	{ 
		lastMd.cell = pos;
	}
#endif
};

#endif
