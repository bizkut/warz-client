//=========================================================================
//	Module: ZombieStates.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "ZombieStates.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "gameobjects/ObjManag.h"
#include "../../UI/HUD_EditorGame.h"
#include "../ai/AI_Player.H"
#include "../ai/r3dPhysSkeleton.h"
#include "obj_ZombieSpawn.h"
#include "fmod/SoundSys.h"

#if 0

//////////////////////////////////////////////////////////////////////////

ZombieLookForTargetState	gZombieLookForTargetState;
ZombieMoveToTargetState		gZombieMoveToTargetState;
ZombieAttackTargetState		gZombieAttackTargetState;
ZombieDieState				gZombieDieState;
ZombieGlobalState			gZombieGlobalState;

//-------------------------------------------------------------------------
//	Look for target
//-------------------------------------------------------------------------

void ZombieLookForTargetState::Enter(obj_Zombie *o)
{
	//o->uberAnim->SwitchToState(PLAYER_IDLE, CUberData::ANIMDIR_Stand);
	o->chasedTarget = invalidGameObjectID;
	o->playingSndHandle = SoundSys.Play(gSndZombieBreatheID, o->GetPosition());
}

//////////////////////////////////////////////////////////////////////////

void ZombieLookForTargetState::Execute(obj_Zombie *o)
{
	//	Iterate through players and search for chased target
	const ClientGameLogic& CGL = gClientLogic();
	//		for (int i = 0; i < CGL.CurMaxPlayerIdx; ++i)
	{
		//			obj_Player *plr = CGL.GetPlayer(i);
		obj_Player* plr = static_cast<obj_Player *>(GameWorld().GetObject(EditorGameHUD::editorPlayerId));

		if (plr)
		{
			bool hardLock = false;
			if (plr->lifeProperties.DetectByZombie(*o, hardLock))
			{
				if (hardLock)
				{
					o->chasedTarget = plr->GetSafeID();
				}
				o->lastTargetPos = plr->GetPosition();
				o->fsm->ChangeState(&gZombieMoveToTargetState);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void ZombieLookForTargetState::Exit(obj_Zombie *o)
{
	SoundSys.Stop(o->playingSndHandle);
	o->playingSndHandle = 0;
}

//-------------------------------------------------------------------------
//	Chase target state
//-------------------------------------------------------------------------

void ZombieMoveToTargetState::Enter(obj_Zombie *o)
{
	// Create navigation agent
	o->navAgentIdx = gNavMeshActorsManager.CreateAgent(o->GetPosition(), o->aiParams);

	GameObject *plr = GameWorld().GetObject(o->chasedTarget);
	r3dVector targetPos(o->lastTargetPos);
	int chaseAnim = PLAYER_MOVE_RUN;
	if (plr)
	{
		targetPos = plr->GetPosition();
		chaseAnim = PLAYER_MOVE_SPRINT;
	}

	gNavMeshActorsManager.NavigateTo(o->navAgentIdx, targetPos);

	//	Switch to run animation
	//o->uberAnim->SwitchToState(chaseAnim, CUberData::ANIMDIR_Str);

	o->playingSndHandle = SoundSys.Play(gSndZombieGrowlID, o->GetPosition());
}

//////////////////////////////////////////////////////////////////////////

void ZombieMoveToTargetState::Execute(obj_Zombie *o)
{
	r3dPoint3D vel;
	float dist = 0;

	//	Propagate AI agent position to zombie position
	r3dPoint3D pos = gNavMeshActorsManager.GetPosition(o->navAgentIdx);
	pos.y -= 0.2f;
	o->SetPosition(pos);

	GameObject *obj = GameWorld().GetObject(o->chasedTarget);
	if (obj)
	{
		float targetOffset = (o->lastTargetPos - obj->GetPosition()).LengthApprox();
		vel = o->GetPosition() - obj->GetPosition();
		dist = vel.Length();

		if (targetOffset > 0.2f)
		{
			gNavMeshActorsManager.AdjustMoveTarget(o->navAgentIdx, obj->GetPosition());
			o->lastTargetPos = obj->GetPosition();
		}

		//	If zombie reach player, switch to attack state
		if (dist < 1.0f)
			o->fsm->ChangeState(&gZombieAttackTargetState);
	}
	else
	{
		vel = o->GetPosition() - o->lastTargetPos;
		dist = (o->GetPosition() - o->lastTargetPos).Length();
		if (dist < 1.0f)
		{
			o->fsm->ChangeState(&gZombieLookForTargetState);
		}
	}

	if (dist > 0.1f)
	{
		vel.y = 0;
		vel.Normalize();

		float angle = atan2f(-vel.z, vel.x) * 0.5f;

		angle = R3D_RAD2DEG(angle);
		r3dVector rotV = o->GetRotationVector();
		rotV.x = angle + 45.0f;

		o->SetRotationVector(rotV);
	}
}

//////////////////////////////////////////////////////////////////////////

void ZombieMoveToTargetState::Exit(obj_Zombie *o)
{
	//	Remove nav agent
	gNavMeshActorsManager.RemoveAgent(o->navAgentIdx);
	o->navAgentIdx = INVALID_NAVIGATION_AGENT_ID;
}

//-------------------------------------------------------------------------
//	Chase target state
//-------------------------------------------------------------------------

void ZombieAttackTargetState::Enter(obj_Zombie *o)
{
	//o->uberAnim->SwitchToState(PLAYER_IDLEAIM, CUberData::ANIMDIR_Stand);
	o->playingSndHandle = SoundSys.Play(gSndZombieAttack2ID, o->GetPosition());
}

//////////////////////////////////////////////////////////////////////////

void ZombieAttackTargetState::Execute(obj_Zombie *o)
{
	GameObject *obj = GameWorld().GetObject(o->chasedTarget);
	if (obj)
	{
		r3dPoint3D vel = obj->GetPosition() - o->GetPosition();
		float dist = vel.Length();
		o->lastTargetPos = obj->GetPosition();
		//	If target too far switch to chase state
		if (dist > 2.0f)
			o->fsm->ChangeState(&gZombieMoveToTargetState);
	}
	else
	{
		//	Back to search target state
		o->fsm->ChangeState(&gZombieLookForTargetState);
	}
}

//////////////////////////////////////////////////////////////////////////

void ZombieAttackTargetState::Exit(obj_Zombie *o)
{
	SoundSys.Stop(o->playingSndHandle);
}

//-------------------------------------------------------------------------
//	Die state
//-------------------------------------------------------------------------

void ZombieDieState::Enter(obj_Zombie *o)
{
	o->physSkeleton->SwitchToRagdollWithForce(true, 100000, o->deathHitVel);
	o->deadTime = r3dGetTime();
}

//////////////////////////////////////////////////////////////////////////

void ZombieDieState::Execute(obj_Zombie *o)
{
	//	Destroy object if corpse stays long enough
	if (o->isActive() && r3dGetTime() - o->deadTime > 5.0f)
	{
		o->setActiveFlag(0);
	}
}

//////////////////////////////////////////////////////////////////////////

void ZombieDieState::Exit(obj_Zombie *o)
{

}

//-------------------------------------------------------------------------
//	Global state
//-------------------------------------------------------------------------

void ZombieGlobalState::Enter(obj_Zombie *o)
{

}

//////////////////////////////////////////////////////////////////////////

void ZombieGlobalState::Execute(obj_Zombie *o)
{
	//	Update sound position
	r3dPoint3D pos = o->GetPosition();
	//	Update sound pos
	SoundSys.SetSoundPos(o->playingSndHandle, pos);
}

//////////////////////////////////////////////////////////////////////////

void ZombieGlobalState::Exit(obj_Zombie *o)
{

}

#endif