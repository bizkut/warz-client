#include "r3dPCH.h"
#include "r3d.h"

#include "NetCellMover.h"

bool CNetCellMover::IsDataChanged(const netMoveData_s& md)
{
	// always send update every 60 sec (just in case)
	if(r3dGetTime() >= nextUpdate + 60.0f)
		return true;

	if(md.state != lastMd.state)
		return true;

	// 1 degree difference
	if(md.turnAngle != lastMd.turnAngle)
		return true;
	if(md.bendAngle != lastMd.bendAngle)
		return true;
		
	// 10cm diff
	if((md.pos - lastMd.pos).Length() > 0.1f)
		return true;

	return false;			
}

DWORD CNetCellMover::SendPosUpdate(const moveData_s& in_data, PKT_C2C_MoveSetCell_s* n1, PKT_C2C_MoveRel_s* n2)
{
	r3d_assert(owner->NetworkLocal);
	R3DPROFILE_FUNCTION("CNetCellMover_SendPosUpdate");
	
	DWORD pktFlags = 0;

	// do not send anything before update ticks
	const float curTime = r3dGetTime();
	if(curTime < nextUpdate)
	  return pktFlags;
	
	// bring angle to [0..360] and pack to [0..255]
	int turnAngle = (int)in_data.turnAngle;
	while(turnAngle < 0)   turnAngle += 360;
	while(turnAngle > 360) turnAngle -= 360;
	turnAngle *= 255;
	turnAngle /= 360;
	r3d_assert(turnAngle >= 0 && turnAngle <= 255);
	
	// bring bend angle (it's in radian) from [-PI/2...PI/2] to [0..255]
	float bendAngle = R3D_CLAMP(in_data.bendAngle, -R3D_PI_2, R3D_PI_2);
	bendAngle = (bendAngle + R3D_PI_2) / R3D_PI * 255.0f;
	r3d_assert(bendAngle >= 0 && bendAngle <= 255);
	
	// build new movement data
	netMoveData_s md;
	md.pos       = in_data.pos;
	md.turnAngle = (BYTE)turnAngle;
	md.bendAngle = (BYTE)bendAngle;
	md.state     = (USHORT)in_data.state;

	// check if we moved away from our cell, if so, send absolute position update
	float dx = md.pos.x - lastMd.cell.x;
	float dy = md.pos.y - lastMd.cell.y;
	float dz = md.pos.z - lastMd.cell.z;
	if(fabs(dx) >= cellSize || 
	   fabs(dy) >= cellSize ||
	   fabs(dz) >= cellSize)
	{
		// fill change cell packet
		n1->pos = md.pos;
		pktFlags |= 1;
		
		lastMd.cell = md.pos;
		dx = 0;
		dy = 0;
		dz = 0;
	}

	// don't send update if nothing was changed
	if(!IsDataChanged(md))
	  return pktFlags;
	
	// convert to [0..254] within cell of last update
	int rel_x = 127 + (int)(dx * 127.0f / cellSize);
	int rel_y = 127 + (int)(dy * 127.0f / cellSize);
	int rel_z = 127 + (int)(dz * 127.0f / cellSize);
	r3d_assert(rel_x < 255);
	r3d_assert(rel_y < 255);
	r3d_assert(rel_z < 255);

	// build delta packet
	pktFlags |= 2;
	n2->rel_x     = (BYTE)rel_x;
	n2->rel_y     = (BYTE)rel_y;
	n2->rel_z     = (BYTE)rel_z;
	n2->turnAngle = md.turnAngle;
	n2->bendAngle = md.bendAngle;
	n2->state     = md.state;
	
	// update last send values
	lastMd.turnAngle = md.turnAngle;
	lastMd.bendAngle = md.bendAngle;
	lastMd.state     = md.state;
	lastMd.pos       = md.pos;

	// update next send time
	nextUpdate += updateDelta;
	if(nextUpdate < curTime) 
		nextUpdate = curTime + updateDelta;
		
	return pktFlags;
}


void CNetCellMover::SetCell(const PKT_C2C_MoveSetCell_s& n)
{
	r3d_assert(!owner->NetworkLocal);
	//r3dOutToLog("... %p PKT_C2C_MoveSetCell_s %f\n", this, lastMd.cell.y);

	lastMd.cell = n.pos;
}

const CNetCellMover::moveData_s& CNetCellMover::DecodeMove(const PKT_C2C_MoveRel_s& n)
{
	r3d_assert(!owner->NetworkLocal);
	
	// relative packet must always follow correct cell
	if(!(lastMd.cell.y > -99999)) 
	{
		// for now, just DROP it.
#ifndef FINAL_BUILD
		r3dOutToLog("!!!! PKT_C2C_MoveRel_s %p move %f\n", this, lastMd.cell.y);
#endif
		return lastMove;
	}
	r3d_assert(lastMd.cell.y > -99999);
	
	// build new target position
	r3dPoint3D pos = lastMd.cell;
	pos.x += (float)(n.rel_x - 127) * cellSize / 127.0f;
	pos.y += (float)(n.rel_y - 127) * cellSize / 127.0f;
	pos.z += (float)(n.rel_z - 127) * cellSize / 127.0f;
	
	lastMove.pos        = pos;
	lastMove.turnAngle  = (float)n.turnAngle / 255.0f * 360.0f;
	lastMove.bendAngle  = ((float)n.bendAngle / 255.0f) * R3D_PI - R3D_PI_2;
	lastMove.state      = n.state;

	lastRecv = r3dGetTime();
	return lastMove;
}

r3dPoint3D CNetCellMover::GetVelocityToNetTarget(const r3dPoint3D& pos, float chase_speed, float teleport_delta_sec)
{
	r3d_assert(!owner->NetworkLocal);
	
	// point character to new position
	r3dPoint3D mdiff = lastMove.pos - pos;
	if(mdiff.Length() < 0.001) {
		return r3dPoint3D(0, 0, 0);
	}

	// calc velocity to reach position on time for next update
	r3dPoint3D vel = mdiff / updateDelta;

	// check if we can reach it in some reasonable time
	if(mdiff.Length() > chase_speed * teleport_delta_sec)
	{
#ifndef FINAL_BUILD
		r3dOutToLog("!!!! %s teleported, too big distance %f\n", owner->Name.c_str(), mdiff.Length());
#endif
		return vel;
	}

	// limit velocity to maximum chase speed
	if(vel.Length() > chase_speed)
	{
		//r3dOutToLog("%s chasing, vel %f\n", UserName, vel.Length());
		vel = vel.NormalizeTo() * chase_speed;
	}
	return vel;
}


#ifdef VEHICLES_ENABLED

bool CVehicleNetCellMover::IsDataChanged(const netMoveData_s& md)
{
	// always send update every 60 sec (just in case)
	if(r3dGetTime() >= nextUpdate + 60.0f)
		return true;

	if(md.state != lastMd.state)
		return true;

	if((md.pos - lastMd.pos).Length() > 0.1f)
		return true;

	if ((md.rot - lastMd.rot).Length() > 0.1f)
		return true;

	return false;			
}

DWORD CVehicleNetCellMover::SendPosUpdate(const moveData_s& in_data, PKT_C2C_VehicleMoveSetCell_s* n1, PKT_C2C_VehicleMoveRel_s* n2)
{
	// todo - readd this over the weekend, temporary code.
	//r3d_assert(owner->NetworkLocal);
	R3DPROFILE_FUNCTION("CNetCellMover_SendPosUpdate");

	DWORD pktFlags = 0;

	// do not send anything before update ticks
	const float curTime = r3dGetTime();
	if(curTime < nextUpdate)
		return pktFlags;

	// build new movement data
	netMoveData_s md;
	md.pos   = in_data.pos;
	md.rot	 = in_data.rot;
	md.state = (USHORT)in_data.state;

	// check if we moved away from our cell, if so, send absolute position update
	float dx = md.pos.x - lastMd.cell.x;
	float dy = md.pos.y - lastMd.cell.y;
	float dz = md.pos.z - lastMd.cell.z;
	if(fabs(dx) >= cellSize || 
		fabs(dy) >= cellSize ||
		fabs(dz) >= cellSize)
	{
		// fill change cell packet
		n1->pos = md.pos;
		pktFlags |= 1;

		lastMd.cell = md.pos;
		dx = 0;
		dy = 0;
		dz = 0;
	}

	// don't send update if nothing was changed
	if(!IsDataChanged(md))
		return pktFlags;

	// convert to [0..254] within cell of last update
	int rel_x = 127 + (int)(dx * 127.0f / cellSize);
	int rel_y = 127 + (int)(dy * 127.0f / cellSize);
	int rel_z = 127 + (int)(dz * 127.0f / cellSize);
	r3d_assert(rel_x < 255);
	r3d_assert(rel_y < 255);
	r3d_assert(rel_z < 255);

	// build delta packet
	pktFlags |= 2;
	n2->rel_x     = (BYTE)rel_x;
	n2->rel_y     = (BYTE)rel_y;
	n2->rel_z     = (BYTE)rel_z;
	n2->rot_x	  = (short)floor(in_data.rot.x * 65535 / 1000);
	n2->rot_y	  = (short)floor(in_data.rot.y * 65535 / 1000);
	n2->rot_z	  = (short)floor(in_data.rot.z * 65535 / 1000);

	n2->state     = md.state;

	lastMd.rot		 = md.rot;
	lastMd.state     = md.state;
	lastMd.pos       = md.pos;

	// update next send time
	nextUpdate += updateDelta;
	if(nextUpdate < curTime) 
		nextUpdate = curTime + updateDelta;

	return pktFlags;
}

void CVehicleNetCellMover::SetCell(const PKT_C2C_VehicleMoveSetCell_s& n)
{
	r3d_assert(!owner->NetworkLocal);

	lastMd.cell = n.pos;
}

const CVehicleNetCellMover::moveData_s& CVehicleNetCellMover::DecodeMove(const PKT_C2C_VehicleMoveRel_s& n)
{
	r3d_assert(!owner->NetworkLocal);

	if(!(lastMd.cell.y > -99999)) 
	{
#ifndef FINAL_BUILD
		r3dOutToLog("!!!! PKT_C2C_MoveRel_s %p move %f\n", this, lastMd.cell.y);
#endif
		return lastMove;
	}
	r3d_assert(lastMd.cell.y > -99999);

	r3dPoint3D pos = lastMd.cell;
	pos.x += (float)(n.rel_x - 127) * cellSize / 127.0f;
	pos.y += (float)(n.rel_y - 127) * cellSize / 127.0f;
	pos.z += (float)(n.rel_z - 127) * cellSize / 127.0f;
	
	r3dPoint3D rot;
	rot.x = (float)n.rot_x * 1000 / 65535;
	rot.y = (float)n.rot_y * 1000 / 65535;
	rot.z = (float)n.rot_z * 1000 / 65535;

	lastMove.pos        = pos;
	lastMove.rot		= rot;
	lastMove.state      = n.state;

	lastRecv = r3dGetTime();
	return lastMove;
}

r3dPoint3D CVehicleNetCellMover::GetVelocityToNetTarget(const r3dPoint3D& pos, float chase_speed, float teleport_delta_sec)
{
	r3d_assert(!owner->NetworkLocal);

	// point character to new position
	r3dPoint3D mdiff = lastMove.pos - pos;
	if(mdiff.Length() < 0.001) {
		return r3dPoint3D(0, 0, 0);
	}

	// calc velocity to reach position on time for next update
	r3dPoint3D vel = mdiff / updateDelta;

	// check if we can reach it in some reasonable time
	if(mdiff.Length() > chase_speed * teleport_delta_sec)
	{
#ifndef FINAL_BUILD
		r3dOutToLog("!!!! %s teleported, too big distance %f\n", owner->Name.c_str(), mdiff.Length());
#endif
		return vel;
	}

	// limit velocity to maximum chase speed
	if(vel.Length() > chase_speed)
	{
		//r3dOutToLog("%s chasing, vel %f\n", UserName, vel.Length());
		vel = vel.NormalizeTo() * chase_speed;
	}
	return vel;
}
#endif


