//=========================================================================
//	Module: obj_MechStates.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "../ai/StateMachine.h"
#include "obj_Mech.h"

//////////////////////////////////////////////////////////////////////////

namespace MechAnimationsFSM
{

/**	Helper start animaition with custom parameters function. */
r3dAnimation::r3dAnimInfo * StartAnimationHelper(obj_Mech *o, MechAnimationsID animID, DWORD flags, float *overrideFadeInTime = 0);

//////////////////////////////////////////////////////////////////////////
	
class Idle: public r3dState<obj_Mech>
{
public:
	virtual void Enter(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class MovementIdle: public Idle
{
	void SwitchUpperBodyAnimForShooting(obj_Mech *o, bool on);
public:
	virtual void Enter(obj_Mech *o);
	virtual void Exit(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class GenericAnim: public r3dState<obj_Mech>
{
protected:
	MechAnimationsID animID;
	int animFlags;

public:
	explicit GenericAnim(MechAnimationsID animID_, int animFlags_);
	virtual void Enter(obj_Mech *o);
	virtual void Exit(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class WalkRun: public r3dState<obj_Mech>
{
	float switchTime;
	void SyncAnimationsAtStart(obj_Mech *o);

public:
	MechAnimationsID fromAnimID, toAnimID;

	WalkRun(MechAnimationsID fromAnim, MechAnimationsID toAnim);
	virtual void Enter(obj_Mech *o);
	virtual void Execute(obj_Mech *o);
	virtual void Exit(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class WalkBack: public GenericAnim
{
public:
	explicit WalkBack(MechAnimationsID animID_);
	virtual void Execute(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class Bump: public r3dState<obj_Mech>
{
	MechAnimationsID animID;

public:
	Bump(MechAnimationsID animID_);
	virtual void Enter(obj_Mech *o);
	virtual void Execute(obj_Mech *o);
	virtual void Exit(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class Falling: public r3dState<obj_Mech>
{
public:
	virtual void Enter(obj_Mech *o);
	virtual void Execute(obj_Mech *o);
	virtual void Exit(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class LandingAfterFall: public r3dState<obj_Mech>
{
	MechAnimationsID animID;

public:
	explicit LandingAfterFall(bool smallLand);
	virtual void Enter(obj_Mech *o);
	virtual void Execute(obj_Mech *o);
	virtual void Exit(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

class GenericShootingAnim: public GenericAnim
{
	MECH_WEAPON_TYPES wpnType;

public:
	explicit GenericShootingAnim(MechAnimationsID animID_, MECH_WEAPON_TYPES wpnType_, int animFlags_);
	virtual void Enter(obj_Mech *o);
	virtual void Exit(obj_Mech *o);
};

//////////////////////////////////////////////////////////////////////////

extern Idle gStateIdle;
extern MovementIdle gStateMovementIdle;
extern WalkRun gStateRun;

extern WalkRun gStateWalkForward;
extern WalkRun gStateWalkForwardLimpRight;
extern WalkRun gStateWalkForwardLimpLeft;

extern WalkBack gStateWalkBackward;
extern WalkBack gStateWalkBackwardLimpRight;
extern WalkBack gStateWalkBackwardLimpLeft;

extern GenericAnim gStateRotate90Right;
extern GenericAnim gStateRotate90Left;
extern Falling gStateFalling;
extern LandingAfterFall gStateLandingShort;
extern LandingAfterFall gStateLandingLong;
extern Bump gStateBumpFront;
extern Bump gStateBumpBack;
extern GenericShootingAnim gStateFireMachinegun;
extern GenericShootingAnim gStateFireSniper;

}

