//=========================================================================
//	Module: obj_MechStates.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "obj_MechStates.h"

//////////////////////////////////////////////////////////////////////////

namespace MechAnimationsFSM
{

Idle gStateIdle;
MovementIdle gStateMovementIdle;

WalkRun gStateRun(MECH_ANIM_WALK, MECH_ANIM_RUN);

WalkRun gStateWalkForward(MECH_ANIM_RUN, MECH_ANIM_WALK);
WalkRun gStateWalkForwardLimpRight(MECH_ANIM_RUN, MECH_ANIM_WALK_LIMP_RIGHT);
WalkRun gStateWalkForwardLimpLeft(MECH_ANIM_RUN, MECH_ANIM_WALK_LIMP_LEFT);

WalkBack gStateWalkBackward(MECH_ANIM_WALK_BACK);
WalkBack gStateWalkBackwardLimpRight(MECH_ANIM_WALK_BACK_LIMP_RIGHT);
WalkBack gStateWalkBackwardLimpLeft(MECH_ANIM_WALK_BACK_LIMP_LEFT);

GenericAnim gStateRotate90Right(MECH_ANIM_LOOP_TURN_90_RIGHT, ANIMFLAG_Looped);
GenericAnim gStateRotate90Left(MECH_ANIM_LOOP_TURN_90_LEFT, ANIMFLAG_Looped);
Bump gStateBumpFront(MECH_ANIM_WALK_WALL_BUMP_FRONT);
Bump gStateBumpBack(MECH_ANIM_WALK_WALL_BUMP_BACK);

Falling gStateFalling;
LandingAfterFall gStateLandingShort(true);
LandingAfterFall gStateLandingLong(false);

GenericShootingAnim gStateFireMachinegun(MECH_ANIM_MACHINE_GUN_SHAKING, MECH_WEAPON_MACHINEGUN, ANIMFLAG_Looped);
GenericShootingAnim gStateFireSniper(MECH_ANIM_SNIPER_AND_OTHER_SINGLE_SHOOTING, MECH_WEAPON_SNIPER, ANIMFLAG_Looped);

//////////////////////////////////////////////////////////////////////////

r3dAnimation::r3dAnimInfo * StartAnimationHelper(obj_Mech *o, MechAnimationsID animID, DWORD flags, float *overrideFadeInTime)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];

	float fadeInTime = overrideFadeInTime ? *overrideFadeInTime : ad.fadeOutTime;

	ad.animTrackId = o->m_Animation->StartAnimation(MECH_ANIMATION_LIST[animID], flags, ad.fadeStart, ad.fadeEnd, fadeInTime);

	//	Adjust start and end frames
	r3dAnimation::r3dAnimInfo *ai = o->m_Animation->GetTrack(ad.animTrackId);
	if (ai)
	{
		ai->SetStartFame(static_cast<float>(ad.startFrame));
	}
	return ai;
}

//////////////////////////////////////////////////////////////////////////

void SyncAnimationFrames(obj_Mech *o, MechAnimationsID fromAnimID, MechAnimationsID toAnimID)
{
	if (!o)
		return;

	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[toAnimID];
	obj_Mech::AdditionalAnimationData &adf = o->additionalAnimData[fromAnimID];
	r3dAnimation::r3dAnimInfo *animInfoFrom = o->m_Animation->GetTrack(adf.animTrackId);
	r3dAnimation::r3dAnimInfo *animInfoTo = o->m_Animation->GetTrack(ad.animTrackId);
	if (animInfoFrom && animInfoTo)
	{
		r3dAnimData *a0 = o->m_AnimPool.Get(adf.animIndexRemap);
		r3dAnimData *a1 = o->m_AnimPool.Get(ad.animIndexRemap);
		if (a0 && a1)
		{
			animInfoTo->SetCurFrame(animInfoFrom->GetCurFrame());
		}
	}
}

//-------------------------------------------------------------------------
//	Mech idle
//-------------------------------------------------------------------------

void Idle::Enter(obj_Mech *o)
{

}

//-------------------------------------------------------------------------
//	Movement idle
//-------------------------------------------------------------------------

void MovementIdle::SwitchUpperBodyAnimForShooting(obj_Mech *o, bool on)
{
	for (int i = MECH_ANIM_GRENADE_LAUNCHING; i <= MECH_ANIM_SNIPER_AND_OTHER_SINGLE_SHOOTING; ++i)
	{
		if (o->additionalAnimData[i].animIndexRemap == -1)
			continue;

		r3dAnimData *ad = o->m_AnimPool.Get(o->additionalAnimData[i].animIndexRemap);
		if (ad)
		{
			o->SwitchUpperBodyAnimation(on, ad);
		}
	}
	o->weaponsFireFSM.ChangeState(&MechAnimationsFSM::gStateIdle);
}

//////////////////////////////////////////////////////////////////////////

void MovementIdle::Enter(obj_Mech *o)
{
	//	Force zero transition time
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[MECH_ANIM_IDLE];

	float fadeInTime = ad.fadeInTime;

	if (o->handleAnimationTeleport)
	{
		fadeInTime = 0;
	}

	StartAnimationHelper(o, MECH_ANIM_IDLE, ANIMFLAG_Looped, &fadeInTime);
	SwitchUpperBodyAnimForShooting(o, false);

	if (o->handleAnimationTeleport)
	{

	}
}

//////////////////////////////////////////////////////////////////////////

void MovementIdle::Exit(obj_Mech *o)
{
	SwitchUpperBodyAnimForShooting(o, true);
	/*
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[MECH_ANIM_IDLE];
	o->m_Animation->FadeOut(ad.animTrackId, ad.fadeOutTime);
*/
}

//-------------------------------------------------------------------------
//	Mech walkrun
//-------------------------------------------------------------------------

WalkRun::WalkRun(MechAnimationsID fromAnim, MechAnimationsID toAnim)
: fromAnimID(fromAnim)
, toAnimID(toAnim)
, switchTime(0.0f)
{

}

//////////////////////////////////////////////////////////////////////////

void WalkRun::Enter(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[toAnimID];
	StartAnimationHelper(o, toAnimID, ANIMFLAG_Looped);

	obj_Mech::AdditionalAnimationData &adf = o->additionalAnimData[fromAnimID];
	r3dAnimation::r3dAnimInfo *animInfoFrom = o->m_Animation->GetTrack(adf.animTrackId);
	r3dAnimation::r3dAnimInfo *animInfoTo = o->m_Animation->GetTrack(ad.animTrackId);
	if (animInfoFrom && animInfoTo)
	{
		r3dAnimData *a0 = o->m_AnimPool.Get(adf.animIndexRemap);
		r3dAnimData *a1 = o->m_AnimPool.Get(ad.animIndexRemap);
		if (a0 && a1)
		{
			animInfoTo->SetCurFrame(animInfoFrom->GetCurFrame());
		}
	}
	switchTime = r3dGetTime();

	o->mechEvents.OnEvent(MechFXEvents::EVENT_WalkStart);

//	SyncAnimationsAtStart(o);
	Execute(o);
}

//////////////////////////////////////////////////////////////////////////

void WalkRun::Execute(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[toAnimID];
	r3dAnimation::r3dAnimInfo *ai = o->m_Animation->GetTrack(ad.animTrackId);
	if (ai)
	{
		bool notLimping = toAnimID == MECH_ANIM_WALK;
		float maxSpeed = notLimping ? o->MoveSpeed : o->MoveLimpSpeed;
		if (toAnimID == MECH_ANIM_RUN)
			maxSpeed = o->RunSpeed;

		float switchInfluence = R3D_CLAMP((r3dGetTime() - switchTime) / 0.1f, 0.0f, 1.0f);

		ai->fInfluence = o->forwardSpeed / maxSpeed * switchInfluence;
	}
}

//////////////////////////////////////////////////////////////////////////

void WalkRun::Exit(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[toAnimID];
	r3dAnimation::r3dAnimInfo *ai = o->m_Animation->GetTrack(ad.animTrackId);
	if (ai)
	{
		//	ai->dwStatus &= ~ANIMFLAG_Looped;
	}
	o->m_Animation->FadeOut(ad.animTrackId, ad.fadeOutTime);
	
	o->mechEvents.OnEvent(MechFXEvents::EVENT_WalkEnd);
//	ad.animTrackId = 0;
}

//////////////////////////////////////////////////////////////////////////

void WalkRun::SyncAnimationsAtStart(obj_Mech *o)
{
	bool needSync = false;
	if (toAnimID == MECH_ANIM_WALK || toAnimID == MECH_ANIM_WALK_LIMP_LEFT || toAnimID == MECH_ANIM_WALK_LIMP_RIGHT)
	{
		if
		(
			o->animFSM.WasInState(gStateWalkForward) ||
			o->animFSM.WasInState(gStateWalkForwardLimpLeft) ||
			o->animFSM.WasInState(gStateWalkForwardLimpRight)
		)
		{
			WalkRun *prevState = static_cast<WalkRun*>(o->animFSM.GetPreviousState());
			SyncAnimationFrames(o, prevState->toAnimID, toAnimID);
		}
	}
}

//-------------------------------------------------------------------------
//	Walk back
//-------------------------------------------------------------------------

WalkBack::WalkBack(MechAnimationsID animID_)
: GenericAnim(animID_, ANIMFLAG_Looped)
{

}

//////////////////////////////////////////////////////////////////////////

void WalkBack::Execute(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];
	r3dAnimation::r3dAnimInfo *ai = o->m_Animation->GetTrack(ad.animTrackId);
	if (ai)
	{
		bool notLimping = animID == MECH_ANIM_WALK_BACK;
		float maxSpeed = notLimping ? o->MoveBackSpeed : o->MoveBackLimpSpeed;
		ai->fInfluence= std::abs(o->forwardSpeed / maxSpeed);
	}

	GenericAnim::Execute(o);
}

//-------------------------------------------------------------------------
//	Generic anim
//-------------------------------------------------------------------------

GenericAnim::GenericAnim(MechAnimationsID animID_, int animFlags_)
: animID(animID_)
, animFlags(animFlags_)
{

}

//////////////////////////////////////////////////////////////////////////

void GenericAnim::Enter(obj_Mech *o)
{
	StartAnimationHelper(o, animID, animFlags);
}

//////////////////////////////////////////////////////////////////////////

void GenericAnim::Exit(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];
	o->m_Animation->FadeOut(ad.animTrackId, ad.fadeOutTime);
	ad.animTrackId = 0;
}

//-------------------------------------------------------------------------
//	Bump
//-------------------------------------------------------------------------

Bump::Bump(MechAnimationsID animID_)
: animID(animID_)
{

}

//////////////////////////////////////////////////////////////////////////

void Bump::Enter(obj_Mech *o)
{
	//	Force zero transition time
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];
	ad.fadeInTime = 0;

	StartAnimationHelper(o, animID, ANIMFLAG_PauseOnEnd);

	o->usePivotAsCameraAnchor = false;
	o->mechEvents.OnEvent(MechFXEvents::EVENT_Bump);
}

//////////////////////////////////////////////////////////////////////////

void Bump::Execute(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];
	if (ad.animTrackId == 0)
		return;

	r3dAnimation::r3dAnimInfo *ai = o->m_Animation->GetTrack(ad.animTrackId);
	if (!ai || (ai->dwStatus & ANIMSTATUS_Finished) || !(ai->dwStatus & ANIMSTATUS_Playing))
	{
		o->animFSM.ChangeState(&gStateIdle);
		o->handleAnimationTeleport = true;
	}
}

//////////////////////////////////////////////////////////////////////////

void Bump::Exit(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];
	ad.animTrackId = 0;
}

//-------------------------------------------------------------------------
//	Falling
//-------------------------------------------------------------------------

void Falling::Enter(obj_Mech *o)
{
	StartAnimationHelper(o, MECH_ANIM_FALLING_LOOP, ANIMFLAG_Looped);
}

//////////////////////////////////////////////////////////////////////////

void Falling::Execute(obj_Mech *o)
{
	if (o->bOnGround)
	{
		if (o->fallingVelocity > 10.0f)
			o->animFSM.ChangeState(&gStateLandingLong);
		else
			o->animFSM.ChangeState(&gStateLandingShort);
	}
}

//////////////////////////////////////////////////////////////////////////

void Falling::Exit(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[MECH_ANIM_FALLING_LOOP];
	o->m_Animation->FadeOut(ad.animTrackId, 0);
	ad.animTrackId = 0;
}

//-------------------------------------------------------------------------
//	Landing
//-------------------------------------------------------------------------

LandingAfterFall::LandingAfterFall(bool smallLand)
: animID(smallLand ? MECH_ANIM_FALLING_SHORT_BUMP : MECH_ANIM_FALLING_LONG_BUMP)
{

}

//////////////////////////////////////////////////////////////////////////

void LandingAfterFall::Enter(obj_Mech *o)
{
	StartAnimationHelper(o, animID, 0);
	o->mechEvents.OnEvent(MechFXEvents::EVENT_Landing);
}

//////////////////////////////////////////////////////////////////////////

void LandingAfterFall::Exit(obj_Mech *o)
{

}

//////////////////////////////////////////////////////////////////////////

void LandingAfterFall::Execute(obj_Mech *o)
{
	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];
	r3dAnimation::r3dAnimInfo *ai = o->m_Animation->GetTrack(ad.animTrackId);
	if (!ai || ai->dwStatus & ANIMSTATUS_Finished)
	{
		o->animFSM.ChangeState(&gStateMovementIdle);
	}
}

//-------------------------------------------------------------------------
//	GenericShootingAnim
//-------------------------------------------------------------------------

GenericShootingAnim::GenericShootingAnim(MechAnimationsID animID_, MECH_WEAPON_TYPES wpnType_, int animFlags_)
: GenericAnim(animID_, animFlags_)
, wpnType(wpnType_)
{
}

//////////////////////////////////////////////////////////////////////////

void GenericShootingAnim::Enter(obj_Mech *o)
{
	GenericAnim::Enter(o);

	for (int i = 0; i < _countof(o->wpnAnimations); ++i)
	{
		obj_Mech::WeaponsAnimData &wad = o->wpnAnimations[i];
		if (wad.anim && wpnType == wad.wpnType)
		{
			wad.anim->StartAnimation(wad.fireAnimID, ANIMFLAG_Looped);
		}
	}

	o->mechEvents.OnEvent(MechFXEvents::EVENT_FireStart);
}

//////////////////////////////////////////////////////////////////////////

void GenericShootingAnim::Exit(obj_Mech *o)
{
	for (int i = 0; i < _countof(o->wpnAnimations); ++i)
	{
		obj_Mech::WeaponsAnimData &wad = o->wpnAnimations[i];
		if (wad.anim)
		{
			for (r3dAnimation::AnimTracksVec::iterator it = wad.anim->AnimTracks.begin(); it != wad.anim->AnimTracks.end(); ++it)
			{
				r3dAnimation::r3dAnimInfo &ai = *it;
				ai.dwStatus &= ~ANIMFLAG_Looped;
			}
		}
	}

	obj_Mech::AdditionalAnimationData &ad = o->additionalAnimData[animID];
	r3dAnimation::r3dAnimInfo *ai = o->m_Animation->GetTrack(ad.animTrackId);
	if (ai)
		ai->dwStatus &= ~ANIMFLAG_Looped;
	ad.animTrackId = 0;

	o->mechEvents.OnEvent(MechFXEvents::EVENT_FireEnd);
}

}; // MechAnimationsFSM