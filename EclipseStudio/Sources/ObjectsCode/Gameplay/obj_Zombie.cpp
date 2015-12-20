//=========================================================================
//	Module: obj_Zombie.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_Zombie.h"
#include "../AI/r3dPhysSkeleton.h"
#include "obj_ZombieSpawn.h"
#include "ObjectsCode/Weapons/WeaponArmory.h"
#include "ObjectsCode/Weapons/HeroConfig.h"
#include "ObjectsCode/Weapons/Barricade.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "..\world\MaterialTypes.h"
#include "../AI/AI_Player.H"
#include "../../../../GameEngine/ai/AutodeskNav/AutodeskNavAgent.h"
#include "r3dInterpolator.h"

#ifdef VEHICLES_ENABLED
#include "../../../../GameEngine/gameobjects/obj_Vehicle.h"
#endif
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_Zombie, "obj_Zombie", "Object");
AUTOREGISTER_CLASS(obj_Zombie);

extern r3dSkeleton*	g_zombieBindSkeleton[3];
extern r3dAnimPool*	g_zombieAnimPool[3];

const uint32_t			g_FemaleZombieItemID	= 20183;
extern const uint32_t	g_SuperZombieItemID		= 20207;
const uint32_t			g_DogZombieItemID		= 20219;

const float obj_Zombie::RequestAnimTimers::MinReqTime = 0.15f;
const float obj_Zombie::RequestAnimTimers::MinCurAnimTime = 1.0f;

const float				_zai_AttackRadius		= 1.2f;

void enableAnimBones(const char* boneName, const r3dSkeleton* skel, r3dAnimData* ad, int enable);

/////////////////////////////////////////////////////////////////////////

namespace
{
	struct ZombieMeshesDeferredRenderable: public MeshDeferredRenderable
	{
		ZombieMeshesDeferredRenderable()
		: Parent(0)
		{
		}

		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw(Renderable* RThis, const r3dCamera& Cam)
		{
			R3DPROFILE_FUNCTION("ZombieMeshesDeferredRenderable");

			ZombieMeshesDeferredRenderable* This = static_cast<ZombieMeshesDeferredRenderable*>(RThis);

			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

			This->Parent->OnPreRender();

			MeshDeferredRenderable::Draw(RThis, Cam);
		}

		obj_Zombie *Parent;
	};

//////////////////////////////////////////////////////////////////////////

	struct ZombieMeshesShadowRenderable: public MeshShadowRenderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			R3DPROFILE_FUNCTION("ZmbShadowRend");
			ZombieMeshesShadowRenderable* This = static_cast< ZombieMeshesShadowRenderable* >( RThis );

			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

			This->Parent->OnPreRender();

			This->SubDrawFunc( RThis, Cam );
		}

		obj_Zombie *Parent;
	};
} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

obj_Zombie::r3dVectorAverage::r3dVectorAverage()
	: CurrentSample( 0 )
	, MinTime( 0.05f )
{
	Reset();
}

void obj_Zombie::r3dVectorAverage::Reset ()
{
	memset( Samples, 0, sizeof( r3dVector ) * SampleSize );
	SetTimer = r3dGetTime();
}

void obj_Zombie::r3dVectorAverage::SetCurSample( const r3dVector& sample )
{
	if( ( r3dGetTime() - SetTimer ) > MinTime )
	{
		Samples[ CurrentSample++ ] = sample;
		CurrentSample %= SampleSize;
		SetTimer = r3dGetTime();
	}
}

r3dVector obj_Zombie::r3dVectorAverage::GetAverage() const
{
	float InvSampleSize = 1.0f / SampleSize;

	r3dVector sum( 0, 0, 0 );
	for( int i = 0; i < SampleSize; ++i )
		sum += Samples[ i ];
	return (sum *= InvSampleSize);
}

//////////////////////////////////////////////////////////////////////////

void obj_Zombie::OnPreRender()
{
	anim_.GetCurrentSkeletonNoUpdate()->SetShaderConstants();
}

struct ZombieDebugRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		ZombieDebugRenderable* This = static_cast<ZombieDebugRenderable*>( RThis );

#ifndef FINAL_BUILD
		float speed = This->Parent->GetVelocity().Length();
		This->Parent->m_minSpeed = R3D_MIN( This->Parent->m_minSpeed, speed );
		This->Parent->m_maxSpeed = R3D_MAX( This->Parent->m_maxSpeed, speed );
		This->Parent->DrawDebugInfo();
#endif
	}

	obj_Zombie* Parent;
};
#ifndef FINAL_BUILD
void obj_Zombie::DrawDebugInfo() const
{
	r3dRenderer->SetMaterial(NULL);

	struct PushRestoreStates
	{
		PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
		}

		~PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}
	} pushRestoreStates; (void)pushRestoreStates;

	const char* stateTextArr[] = {"Dead", "Sleep", "Awaking", "CallForHelp", "Idle", "TurnLeft", "TurnRight", "Walk", "Chasing", "SprintChase", "Attack", "Barricade", "MovingAttack"
#ifdef VEHICLES_ENABLED
, "FakeDead"
#endif
};
	COMPILE_ASSERT(R3D_ARRAYSIZE(stateTextArr) == EZombieStates::ZState_NumStates);

	if(gClientLogic().localPlayer_)
	{
		r3dVector dirFromPlayer = GetPosition() - gClientLogic().localPlayer_->GetPosition();
		float dist = dirFromPlayer.Length();
		if(dist < 50)
		{
			r3dPoint3D scrCoord;
			if(r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1, 0), &scrCoord))
			{
				r3dColor24 clr = CreateParams.FastZombie ? r3dColor(255,255,255) : r3dColor(165, 165, 165);
				switch( r_show_zombie_stats->GetInt() )
				{
				case 1: // Distance and Direction to Player, and Current State (not Anim State)
					Font_Label->PrintF(scrCoord.x, scrCoord.y,   clr, "%.1f<%0.2f, %0.2f, %0.2f>, %s\n", dist, dirFromPlayer.x, dirFromPlayer.y, dirFromPlayer.z, stateTextArr[ZombieState] );
					break;

				case 2: // State, Anim State, Animation Group, Anim State Level, Current Anims, Speed, Min/Max/Walk/Run/Sprint Speed, Sprint Timer/FalloffTimer, Anim Timers, Avg Velocity
					{
						r3dString animsPlaying;
						for(r3dAnimation::AnimTracksVec::const_iterator iter = anim_.AnimTracks.begin();
							iter != anim_.AnimTracks.end(); ++iter)
						{
							if( iter->GetStatus() & ANIMSTATUS_Playing )
							{
								const char* shortName =  strrchr( iter->GetAnim()->GetAnimFileName(), '\\' );
								if( shortName == NULL )
									shortName = iter->GetAnim()->GetAnimFileName();
								else
									shortName = shortName + 1;
								char animSpeed[ 10 ];
								char animCurFrame[ 10 ];
								char animFrames[ 10 ];
								char animRate[ 10 ];
								snprintf( animSpeed, 9, "(%.2f", iter->GetSpeed() );
								snprintf( animCurFrame, 9, "[%.0f/", iter->GetCurFrame() );
								snprintf( animFrames, 9, "%d,", iter->GetAnim()->GetNumFrames() );
								snprintf( animRate, 9, "%.2f])", iter->GetAnim()->GetFrameRate() );
								if( 0 < animsPlaying.Length() )
									animsPlaying = r3dString( shortName ) + r3dString( animSpeed ) + r3dString( animCurFrame ) + r3dString( animFrames ) + r3dString( animRate ) + r3dString("\n") + animsPlaying;
								else
									animsPlaying = r3dString( shortName ) + r3dString( animSpeed ) + r3dString( animCurFrame ) + r3dString( animFrames ) + r3dString( animRate );
							}
						}
						Font_Label->PrintF(scrCoord.x, scrCoord.y,   clr, "%s, %s\n(%d : %d)%s\n%.4f[%.4f, %.4f](%0.2f, %0.2f, %0.2f)\n[%.2f, %.2f]\n%.3f[%.3f, %.3f, %.3f, %.3f]\n%.4f\n%s(%.2f)", stateTextArr[ZombieState], stateTextArr[ZombieAnimState], ZombieAnimGroup, ZombieAnimStateLevel, animsPlaying.c_str(), Velocity.Length(), m_minSpeed, m_maxSpeed, CreateParams.WalkSpeed, CreateParams.RunSpeed, CreateParams.SprintMaxSpeed, SprintTimer, SprintFalloffTimer, ZombieReqAnimTimers.GetCurAnimTime(), ZombieReqAnimTimers.GetIdleTime(), ZombieReqAnimTimers.GetWalkTime(), ZombieReqAnimTimers.GetRunTime(), ZombieReqAnimTimers.GetSprintTime(), AvgVelocity.GetAverage().Length(), turnInterp.IsPaused() ? "Paused" : ( turnInterp.IsFinished() ? "Finished" : "Active" ), turnInterp.GetFinishAngle() );
					}
					break;

				case 3: // State, Anim State, Animation Group, Current Anims, Anim Timers, Avg Velocity
					{
						r3dString animsPlaying;
						for(r3dAnimation::AnimTracksVec::const_iterator iter = anim_.AnimTracks.begin();
							iter != anim_.AnimTracks.end(); ++iter)
						{
							if( iter->GetStatus() & ANIMSTATUS_Playing )
							{
								const char* shortName =  strrchr( iter->GetAnim()->GetAnimFileName(), '\\' );
								if( shortName == NULL )
									shortName = iter->GetAnim()->GetAnimFileName();
								else
									shortName = shortName + 1;
								char animSpeed[ 10 ];
								char animCurFrame[ 10 ];
								char animFrames[ 10 ];
								char animRate[ 10 ];
								snprintf( animSpeed, 9, "(%.5f", iter->GetSpeed() );
								snprintf( animCurFrame, 9, "[%.0f/", iter->GetCurFrame() );
								snprintf( animFrames, 9, "%d,", iter->GetAnim()->GetNumFrames() );
								snprintf( animRate, 9, "%.2f])", iter->GetAnim()->GetFrameRate() );
								if( 0 < animsPlaying.Length() )
									animsPlaying = r3dString( shortName ) + r3dString( animSpeed ) + r3dString( animCurFrame ) + r3dString( animFrames ) + r3dString( animRate ) + r3dString("\n") + animsPlaying;
								else
									animsPlaying = r3dString( shortName ) + r3dString( animSpeed ) + r3dString( animCurFrame ) + r3dString( animFrames ) + r3dString( animRate );
							}
						}
						Font_Label->PrintF(scrCoord.x, scrCoord.y,   clr, "%s, %s\n(%d)%s\n%.3f[%.3f, %.3f, %.3f, %.3f]\n%.4f", stateTextArr[ZombieState], stateTextArr[ZombieAnimState], ZombieAnimGroup, animsPlaying.c_str(), ZombieReqAnimTimers.GetCurAnimTime(), ZombieReqAnimTimers.GetIdleTime(), ZombieReqAnimTimers.GetWalkTime(), ZombieReqAnimTimers.GetRunTime(), ZombieReqAnimTimers.GetSprintTime(), AvgVelocity.GetAverage().Length() );
					}
					break;

				case 4: // Draw Circle and Line anchoring to Spawn Position, Draw Line showing original Facing Vector
					{
					// draw where they spawned, so we can see them
					r3dPoint3D off(0, 4, 0);
					r3dColor   clr(0, 255, 0);
					
					r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH | R3D_BLEND_NOALPHA);
					
					r3dDrawLine3D(CreateParams.spawnPos, CreateParams.spawnPos + r3dPoint3D(0, 20.0f, 0), gCam, 0.03f, clr);
					r3dDrawLine3D(CreateParams.spawnPos + r3dPoint3D(0, 1.0f, 0), GetPosition() + r3dPoint3D(0, 1.0f, 0), gCam, 0.03f, clr);

					// draw a circle with a line pointing to their spawn facing vector
					r3dVector fwd( 0, 0, 1 );
					fwd.RotateAroundY( CreateParams.spawnDir + 90.0f ); // RotationVector only stores angle - 90.0f around y, in x component.
					r3dDrawCircle3D(CreateParams.spawnPos, 2.0f, gCam, 0.03f, r3dColor24::red);
					r3dDrawLine3D(CreateParams.spawnPos, CreateParams.spawnPos + fwd, gCam, 0.03f, r3dColor24::grey);

					r3dRenderer->Flush();
					r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
					}
					break;

				default:
					break;
				}
				
			}
		}
	}

	if(ZombieState == EZombieStates::ZState_Walk || ZombieState == EZombieStates::ZState_Pursue || ZombieState == EZombieStates::ZState_PursueSprint)
	{
		if(gotDebugMove)
		{
			float w = 0.1f;
			r3dDrawLine3D(dbgAiInfo.to, dbgAiInfo.to + r3dPoint3D(0, 10, 0), gCam, w, r3dColor(255, 255, 0));
			r3dDrawLine3D(dbgAiInfo.from, dbgAiInfo.from + r3dPoint3D(0, 10, 0), gCam, w, r3dColor(0, 0, 255));
			r3dDrawLine3D(dbgAiInfo.from + r3dPoint3D(0, 0.5f, 0), dbgAiInfo.to + r3dPoint3D(0, 0.5f, 0), gCam, w, r3dColor(0, 0, 255));
			r3dDrawLine3D(GetPosition() + r3dPoint3D(0, 1, 0), dbgAiInfo.from, gCam, w, r3dColor(0, 255, 0));
			r3dDrawLine3D(GetPosition() + r3dPoint3D(0, 1, 0), dbgAiInfo.to, gCam, w, r3dColor(0, 255, 0));
		}
	}

	r3dPoint3D pos = GetPosition();
	const float eyeHeight = 1.6f;
	r3dPoint3D origin( pos.x, pos.y + eyeHeight, pos.z );
	switch( d_show_zombie_frame->GetInt() )
	{
	case 4:	// Display the Zombie's DebugHistory
		{
			r3dPoint3D scrCoord;
			if(r3dProjectToScreen(GetPosition(), &scrCoord))
			{
				Font_Editor->PrintF(scrCoord.x, scrCoord.y, r3dColor24( 0, 255, 200 ), "NetID: %d", GetNetworkID() );
			}
			if( (DWORD)d_show_zombie_history->GetInt() == GetNetworkID() )
			{
				for( uint32_t i = 1; i < m_debugHistory.size; ++i )
				{
					uint32_t index		= ( m_debugHistory.m_index + i ) % m_debugHistory.size;
					uint32_t prevIndex	= index - 1;
					// Protect against Underflow (uint wrap-around);
					if( prevIndex > m_debugHistory.size - 1 )
						prevIndex = m_debugHistory.size - 1;

					// When i == 0, use the zombie's current data to generate DebugData,
					// and use the last DebugData as the prevData, so we don't draw a
					// loop from the current position to the first DebugData.
					const DebugData& prevData = ( i != 1 ) ? m_debugHistory.m_data[ prevIndex ] : m_debugHistory.m_data[ m_debugHistory.m_index ];
					const DebugData& data = ( i != 1 ) ? m_debugHistory.m_data[ index ] : DebugData( (EZombieStates::EGlobalStates)ZombieState, (EZombieStates::EGlobalStates)ZombieAnimState, GetPosition(), GetVelocity() );

					// Don't draw lines to Origin.
					if( prevData.m_point.AlmostEqual( r3dPoint3D( 0, 0, 0 ) ) ||
						data.m_point.AlmostEqual( r3dPoint3D( 0, 0, 0 ) ) )
						continue;

					// Draw the path
					r3dDrawLine3D( prevData.m_point, data.m_point, gCam, 0.03f, (i != 1) ? r3dColor( 255, 0, 0 ) : r3dColor( 0, 0, 255 ) );

					// Draw the data
					if(r3dProjectToScreen(prevData.m_point + r3dPoint3D(0, 0.5f, 0), &scrCoord))
					{
						Font_Editor->PrintF(scrCoord.x, scrCoord.y, r3dColor24( 0, 255, 200 ),
							"Pos: <%.2f, %.2f, %.2f>\nVel: <%.2f, %.2f, %.2f>(%.2f)\nStates: %s, %s",
							prevData.m_point.x, prevData.m_point.y, prevData.m_point.z,
							prevData.m_velocity.x, prevData.m_velocity.y, prevData.m_velocity.z, prevData.m_velocity.Length(),
							stateTextArr[ (int)prevData.m_zombieState ], stateTextArr[ (int)prevData.m_zombieAnimState ] );
					}
					// Make sure the last point's data is visible.
					if( i == m_debugHistory.size - 1 )
					{
						r3dDrawLine3D( data.m_point, m_debugHistory.m_data[ m_debugHistory.m_index ].m_point, gCam, 0.03f, r3dColor( 255, 0, 0 ) );

						if(r3dProjectToScreen(data.m_point + r3dPoint3D(0, 0.5f, 0), &scrCoord))
						{
							Font_Editor->PrintF(scrCoord.x, scrCoord.y, r3dColor24( 200, 255, 0 ),
								"Pos: <%.2f, %.2f, %.2f>\nVel: <%.2f, %.2f, %.2f>(%.2f)\nStates: %s, %s",
								data.m_point.x, data.m_point.y, data.m_point.z,
								data.m_velocity.x, data.m_velocity.y, data.m_velocity.z, data.m_velocity.Length(),
								stateTextArr[ (int)data.m_zombieState ], stateTextArr[ (int)data.m_zombieAnimState ] );
						}
					}
				}
			}
		}
		break;

	case 3:	// Display the Line of Sight to the player, and what intersects that.
		if( gClientLogic().localPlayer_ )
		{
			// Line of Sight to player
			r3dColor24 clr = r3dColor24::yellow;
			r3dString collisions;

			// Issue raycast query to check visibility occluders
			r3dPoint3D dir = (gClientLogic().localPlayer_->GetPosition() - GetPosition());
			float dist = dir.Length();
			dir.Normalize();

			if( dist < 200 )
			{
				PxVec3 porigin(pos.x, pos.y + eyeHeight, pos.z);
				PxVec3 pdir(dir.x, dir.y, dir.z);
				PxSceneQueryFlags flags = PxSceneQueryFlag::eDISTANCE;
				PxRaycastHit hit;
				PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eDYNAMIC | PxSceneQueryFilterFlag::eSTATIC));
				if(g_pPhysicsWorld->PhysXScene->raycastSingle(porigin, pdir, dist, flags, hit, filter))
				{
					PhysicsCallbackObject* target = NULL;
					if(hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
					{
						GameObject* obj = target->isGameObject();
						if(obj)
						{
							if( collisions.Length() > 0 )
								collisions = collisions + r3dString(", ") + obj->Name;
							else
								collisions = obj->Name;

							clr = r3dColor24::black;
						}
					}
				}
				r3dDrawLine3D( origin, gClientLogic().localPlayer_->GetPosition(), gCam, 0.01f, clr );
				if( collisions.Length() > 0 )
				{
					r3dPoint3D scrCoord;
					r3dPoint3D labelPos = origin + 0.5f * ( gClientLogic().localPlayer_->GetPosition() - origin ) + r3dPoint3D(0, 1, 0);
					if(r3dProjectToScreen(labelPos, &scrCoord))
					{
						Font_Label->PrintF(scrCoord.x, scrCoord.y,   r3dColor(255,255,255), "%s", collisions.c_str());
					}
				}
			}
		}
		// Intentional fall-through.
	case 2:	// Display the Zombie's Turn Interopolator Data.
		{
			r3dDrawLine3D( pos, pos + turnInterp.GetCurrent(), gCam, 0.01f, r3dColor( 255, 0, 0 ) );

			r3dDrawLine3D( pos, pos + turnInterp.m_startVec, gCam, 0.01f, r3dColor( 255, 255, 255 ) );
			r3dDrawLine3D( pos, pos + turnInterp.m_finishVec, gCam, 0.01f, r3dColor( 255, 0, 255 ) );
		}
		// Intentional fall-through.
	case 1:	// Display the Zombie's Frame and Heading
		{
			// Frame
			r3dVector fwd( 0, 0, 1 );
			fwd.RotateAroundY( GetRotationVector().x + 90.0f ); // RotationVector only stores angle - 90.0f around y, in x component.
			r3dVector up = r3dVector( 0.0f, 1.0f, 0.0f );
			r3dVector right = fwd.Cross( up );
			r3dDrawLine3D( pos, pos + 0.25f * fwd, gCam, 0.01f, r3dColor24::blue );
			r3dDrawLine3D( pos, pos + 0.25f * up, gCam, 0.01f, r3dColor24::red );
			r3dDrawLine3D( pos, pos + 0.25f * right, gCam, 0.01f, r3dColor24::green );

			// Heading (more obvious than trying to discern the Frame when running away from a zombie)
			r3dVector invFwd( 0, 0, -1 );
			invFwd.RotateAroundY( GetRotationVector().x );
			r3dDrawLine3D( pos + r3dVector( 0, 1, 0 ), pos + invFwd + r3dVector( 0, 1, 0 ), gCam, 0.01f, r3dColor( 0, 128, 204 ) );
		}
		break;

	}

	// Display Zombie's detection ranges.
	if( d_show_zombie_range->GetBool() )
	{
		const static float AudibleDetectRanges[] = {
			75,	// default, storecat_SNP, .50 cal
			50, // storecat_ASR, storecat_SHTG, storecat_MG
			30, // storecat_HG, storecat_SMG
			26.25, // VSS
			15, // storecat_MELEE, crossbow, nailgun
		};

		// Taken from sobj_Zombie::IsPlayerDetectable
		const static float VisionDetectRangesByState[] = {
			 4.0f, //PLAYER_IDLE = 0,
			 4.0f, //PLAYER_IDLEAIM,
			 7.0f, //PLAYER_MOVE_CROUCH,
			 7.0f, //PLAYER_MOVE_CROUCH_AIM,
			10.0f, //PLAYER_MOVE_WALK_AIM,
			15.0f, //PLAYER_MOVE_RUN,
			30.0f, //PLAYER_MOVE_SPRINT,
			 4.0f, //PLAYER_MOVE_PRONE,
			 4.0f, //PLAYER_MOVE_PRONE_AIM,
			 1.8f, //PLAYER_PRONE_UP,
			 1.8f, //PLAYER_PRONE_DOWN,
			 1.8f, //PLAYER_PRONE_IDLE,
			 4.0f, //PLAYER_SWIM_IDLE,
			 7.0f, //PLAYER_SWIM_SLOW,
			15.0f, //PLAYER_SWIM,
			30.0f, //PLAYER_SWIM_FAST,
#ifdef VEHICLES_ENABLED
			100.0f, // VEHICLE DRIVER,
			100.0f, // VEHICLE PASSENGER,
#endif 
			 0.0f, //PLAYER_DIE,
		};
		if( ZombieState != EZombieStates::ZState_Dead )
		{
			r3dPoint3D pos = GetPosition();
			obj_Player* plr = gClientLogic().localPlayer_;
			if( plr && plr->PlayerState != PLAYER_DIE &&
				(plr->GetPosition() - pos).Length() < (VisionDetectRangesByState[ PLAYER_MOVE_SPRINT ] + 10.0f) )
			{
				r3dPoint3D p1	= pos + r3dPoint3D(0,0.2f,0);

				switch( d_show_zombie_range->GetInt() )
				{
				case 3: // Audible ranges
					{
						float radius	= AudibleDetectRanges[ 0 ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(0, 225, 255));

						radius	= AudibleDetectRanges[ 1 ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(0, 105, 255));

						radius	= AudibleDetectRanges[ 2 ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(0, 135, 255));

						radius	= AudibleDetectRanges[ 3 ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(0, 165, 255));

						radius	= AudibleDetectRanges[ 4 ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(0, 195, 255));
					}
					// Intentional fall-through
				case 2: // Visual ranges
					{
						float radius	= VisionDetectRangesByState[ PLAYER_PRONE_IDLE ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(255, 75, 0));

						radius	= VisionDetectRangesByState[ PLAYER_IDLE ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(255, 105, 0));

						radius	= VisionDetectRangesByState[ PLAYER_MOVE_CROUCH ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(255, 135, 0));

						radius	= VisionDetectRangesByState[ PLAYER_MOVE_WALK_AIM ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(255, 165, 0));

						radius	= VisionDetectRangesByState[ PLAYER_MOVE_RUN ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(255, 195, 0));

						radius	= VisionDetectRangesByState[ PLAYER_MOVE_SPRINT ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor(255, 225, 0));
					}
					// Intentional fall-through
				default:
				case 1: // Detection radius of player's current posture
					{
						float radius = VisionDetectRangesByState[ plr->PlayerState ];
						r3dDrawCircle3D(p1, radius, gCam, 0.1f, r3dColor::red);
					}
					break;
				};
			}
		}
	}

	//r3dDrawBoundBox(GetBBoxWorld(), gCam, r3dColor24::green, 0.05f);
}
#endif
//////////////////////////////////////////////////////////////////////////

void obj_Zombie::AppendRenderables(RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam)
{
	for (int i = 0; i < 4; ++i)
	{
		r3dMesh *mesh = zombieParts[i];

		if (!mesh)
			continue;

		uint32_t prevCount = render_arrays[ rsFillGBuffer ].Count();
		mesh->AppendRenderablesDeferred( render_arrays[ rsFillGBuffer ], r3dColor::white, GetSelfIllumMultiplier() );

		for( uint32_t i = prevCount, e = render_arrays[ rsFillGBuffer ].Count(); i < e; i ++ )
		{
			ZombieMeshesDeferredRenderable& rend = static_cast<ZombieMeshesDeferredRenderable&>( render_arrays[ rsFillGBuffer ][ i ] ) ;

			rend.Init();
			rend.Parent = this;
		}
	}

#ifndef FINAL_BUILD
	if(r_show_zombie_stats->GetBool())
	{
		ZombieDebugRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= 2*RENDERABLE_USER_SORT_VALUE;
		render_arrays[ rsDrawFlashUI ].PushBack( rend );
	}
#endif

	wasVisible = 1;
}

//////////////////////////////////////////////////////////////////////////

void obj_Zombie::AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam)
{
	float distSq = (gCam - GetPosition()).LengthSq();
	float dist = sqrtf( distSq );
	UINT32 idist = (UINT32) R3D_MIN( dist * 64.f, (float)0x3fffffff );

	if( !gDisableDynamicObjectShadows && sliceIndex != r_active_shadow_slices->GetInt() - 1 )
	{
		for (int i = 0; i < 4; ++i)
		{
			r3dMesh *mesh = zombieParts[i];

			if (!mesh)
				continue;

			uint32_t prevCount = rarr.Count();

			mesh->AppendShadowRenderables( rarr );

			for( uint32_t i = prevCount, e = rarr.Count(); i < e; i ++ )
			{
				ZombieMeshesShadowRenderable& rend = static_cast<ZombieMeshesShadowRenderable&>( rarr[ i ] );

				rend.Init() ;
				rend.SortValue |= idist;
				rend.Parent = this ;
			}
		}

		wasVisible = 1;
	}
}

obj_Zombie::obj_Zombie() 
	: m_typeIndex( 0 )
	, m_isFemale( false )
	, attackCounter( 0 )
	, isPhysInRagdoll( false )
	, UpdateWarmUp( 0 )
	, PhysicsOn( 1 )
	, ZombieAnimState( -1 )
	, ZombieAnimStateLevel( 0 )
	, physSkeletonIndex( -1 )
	, wasVisible( 1 )
	, targetId( invalidGameObjectID )
	, netMover(this, 1.0f / 10, (float)PKT_C2C_MoveSetCell_s::PLAYER_CELL_RADIUS)
	, PhysXObstacleIndex( -1 )
	, lastTimeHitPos( r3dPoint3D(0, 0, 0) )
	, lastTimeDmgType( storecat_ASR )
	, lastTimeHitBoneID( 0xFF )
	, staggeredTrack( 0 )
	, attackTrack( 0 )
	, superAttackDir( 0 )
	, holdAttackTrack( 0 )
	, WalkTrack( 0 )
	, RunTrack( 0 )
	, SprintTrack( 0 )
	, SprintTimer( -1.0f )
	, SprintFalloffTimer( -1.0f )
	, bDead( false )
#ifndef FINAL_BUILD
	, m_minSpeed( 1000.0f )
	, m_maxSpeed( 0.0f )
	, m_debugHistory( DebugHistory() )
#endif
{
	ObjTypeFlags |= OBJTYPE_Zombie;

	m_bEnablePhysics = false;
	m_isSerializable = false;

	if(g_zombieBindSkeleton[0] == NULL) 
	{
		g_zombieBindSkeleton[0] = game_new r3dSkeleton();
		g_zombieBindSkeleton[0]->LoadBinary("Data\\ObjectsDepot\\Characters\\ProperScale_AndBiped.skl");
		
		g_zombieAnimPool[0] = game_new r3dAnimPool();
	}

	if(g_zombieBindSkeleton[1] == NULL) 
	{
		g_zombieBindSkeleton[1] = game_new r3dSkeleton();
		g_zombieBindSkeleton[1]->LoadBinary("Data\\ObjectsDepot\\Characters\\Super_Zombie.skl");
		
		g_zombieAnimPool[1] = game_new r3dAnimPool();
	}

	if(g_zombieBindSkeleton[2] == NULL) 
	{
		g_zombieBindSkeleton[2] = game_new r3dSkeleton();
		g_zombieBindSkeleton[2]->LoadBinary("Data\\ObjectsDepot\\WZ_Animals\\Zombie_Dog.skl");
		
		g_zombieAnimPool[2] = game_new r3dAnimPool();
	}

	ZeroMemory(zombieParts, _countof(zombieParts) * sizeof(zombieParts[0]));
	ZeroMemory(&CreateParams, sizeof(CreateParams));

	gotDebugMove = false;
}

//////////////////////////////////////////////////////////////////////////

obj_Zombie::~obj_Zombie()
{
	r3d_assert(physSkeletonIndex==-1); // make sure that skeleton was released
}

BOOL obj_Zombie::OnCreate()
{
	r3d_assert(CreateParams.HeroItemID);
	
	bool HalloweenZombie = false;
	if(CreateParams.HeroItemID > 1000000)
	{
		// special Halloween zombie
		CreateParams.HeroItemID -= 1000000;
		HalloweenZombie = true;
	}

	switch( CreateParams.HeroItemID )
	{
		case g_SuperZombieItemID:
			m_typeIndex = 1;
			break;
		case g_DogZombieItemID:
			m_typeIndex = 2;
			break;
		default:
			m_typeIndex = 0;
			break;
	}
	//m_typeIndex = (CreateParams.HeroItemID == g_SuperZombieItemID) ? 1 : 0;
	anim_.Init( g_zombieBindSkeleton[ m_typeIndex ], g_zombieAnimPool[ m_typeIndex ], NULL, (DWORD)this);

	const HeroConfig* heroConfig = g_pWeaponArmory->getHeroConfig(CreateParams.HeroItemID);
	if(!heroConfig) r3dError("there is no zombie hero %d", CreateParams.HeroItemID);
	
	zombieParts[0] = heroConfig->getHeadMesh(CreateParams.HeadIdx);
	if ( IsDogZombie() )
	{
		int variant = u_random(9);
		char AnimalSCO[512];

		if ( variant == 0 )
			strcpy(AnimalSCO,"Data\\ObjectsDepot\\WZ_Animals\\zdog_10_body_01.sco");
		else
			sprintf(AnimalSCO,"Data\\ObjectsDepot\\WZ_Animals\\zdog_0%i_body_01.sco",variant);

		zombieParts[1] = r3dGOBAddMesh(AnimalSCO);
	}
	else {
		zombieParts[1] = heroConfig->getBodyMesh(CreateParams.HeadIdx, false);
	}
	zombieParts[2] = heroConfig->getLegMesh(CreateParams.LegsIdx);

	if(HalloweenZombie)
		zombieParts[3] = r3dGOBAddMesh("Data/ObjectsDepot/Characters/HGear_Santa_Beard_01.sco", true, false, true, true );

	// temporary boundbox until all parts are loaded
	r3dBoundBox bbox;
	bbox.Org	= r3dPoint3D(-0.2f, 0.0f, -0.2f);
	bbox.Size	= r3dPoint3D(+0.2f, 2.0f, +0.2f);
	SetBBoxLocal(bbox);
	gotLocalBbox = false;

	r3d_assert(!NetworkLocal);
	r3d_assert(PhysXObstacleIndex == -1);

	PxBoxObstacle obstacle;
	obstacle.mHalfExtents = PxVec3(0.25f, 1.0f, 0.25f);
	obstacle.mPos = PxExtendedVec3(GetPosition().x, GetPosition().y, GetPosition().z);

	PhysXObstacleIndex = AcquirePlayerObstacle(GetPosition(), obstacle);

	SetRotationVector(r3dVector(CreateParams.spawnDir, 0, 0));
	
	// set zombie to starting state.
	ZombieState = -1;
	m_isIdleCrouched = false;
	m_isIdleEating = false;
	if(CreateParams.State == EZombieStates::ZState_Dead)
	{
		ZombieState = EZombieStates::ZState_Dead;
		ZombieAnimState = EZombieStates::ZState_Dead;
		// special case if zombie is dead already, just spawn dead pose anim
		// there is 1-7 dead poses
		char aname[128];
		if( IsSuperZombie() )
			sprintf(aname, "Super_Zombie_Dead_%c_01", (u_random(2) < 1) ? 'F' : 'B');
		else if ( IsDogZombie() )
			strcpy(aname, "hunzi_idle_02");
		else
			sprintf(aname, "Zombie_Pose_dead%d", 1 + u_random(7));
		int aid = AddAnimation(aname);
		anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherNow | ANIMFLAG_Looped, 1.0f, 1.0f, 0.0f);
	}
	else
	{
		SetZombieState(CreateParams.State, true);
		
		// remove fading from animation started in SetZombieState
		for(size_t i=0; i<anim_.AnimTracks.size(); i++)
		{
			anim_.AnimTracks[i].dwStatus  &= ~ANIMSTATUS_Fading;
			anim_.AnimTracks[i].fInfluence = 1.0f;
		}
	}
	// ZombieAnimState starts out the same as ZombieState
	ZombieAnimState = ZombieState;
	ZombieAnimGroup	= (!IsSuperZombie() && CreateParams.FastZombie) ? ( 1 + u_random(2) ) : 0;

	UpdateAnimations( 0 );

	m_isFemale = (CreateParams.HeroItemID == g_FemaleZombieItemID);

	parent::OnCreate();

	if( IsSuperZombie() )
	{
		m_sndMaxDistScream = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_SCREAM"));
		m_sndMaxDistIdle = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_IDLE"));
		m_sndMaxDistChase = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_CHASING_M"));
		m_sndMaxDistAttack = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_ATTACK"));
		m_sndMaxDistAll = R3D_MAX(R3D_MAX(R3D_MAX(m_sndMaxDistIdle, m_sndMaxDistChase), m_sndMaxDistAttack), m_sndMaxDistScream);
		m_sndIdleHandle = NULL;
		m_sndChaseHandle = NULL;
		m_sndAttackHandle = NULL;
		m_sndHurtHandle = NULL;
		m_sndDeathHandle = NULL;
		m_sndScreamHandle = NULL;
	}
	else if ( IsDogZombie() )
	{
		m_sndMaxDistScream = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_DOG"));
		m_sndMaxDistIdle = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_IDLE_DOG"));
		m_sndMaxDistChase = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_CHASING_DOG"));
		m_sndMaxDistAttack = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_ATTACK_DOG"));
		m_sndMaxDistAll = R3D_MAX(R3D_MAX(R3D_MAX(m_sndMaxDistIdle, m_sndMaxDistChase), m_sndMaxDistAttack), m_sndMaxDistScream);
		m_sndIdleHandle = NULL;
		m_sndChaseHandle = NULL;
		m_sndAttackHandle = NULL;
		m_sndHurtHandle = NULL;
		m_sndDeathHandle = NULL;
		m_sndScreamHandle = NULL;
	}
	else
	{
		m_sndMaxDistScream = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_M"));
		m_sndMaxDistIdle = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_IDLE_M"));
		m_sndMaxDistChase = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_CHASING_M"));
		m_sndMaxDistAttack = SoundSys.getEventMax3DDistance(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_ATTACK_M"));
		m_sndMaxDistAll = R3D_MAX(R3D_MAX(R3D_MAX(m_sndMaxDistIdle, m_sndMaxDistChase), m_sndMaxDistAttack), m_sndMaxDistScream);
		m_sndIdleHandle = NULL;
		m_sndChaseHandle = NULL;
		m_sndAttackHandle = NULL;
		m_sndHurtHandle = NULL;
		m_sndDeathHandle = NULL;
		m_sndScreamHandle = NULL;
	}
	
#ifndef FINAL_BUILD
	// send request for AI information updates to server
	PKT_C2S_Zombie_DBG_AIReq_s n;
	p2pSendToHost(this, &n, sizeof(n));
#endif
	ZombieSortEntry entry;
	entry.zombie = this;
	entry.distance = 0;
	ZombieList.PushBack( entry );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void obj_Zombie::UpdateSounds()
{
	if(gClientLogic().localPlayer_ == NULL)
		return;

	if(bDead)
	{
		if(m_sndDeathHandle)
		{
			if(!SoundSys.isPlaying(m_sndDeathHandle))
			{
				SoundSys.Release(m_sndDeathHandle); m_sndDeathHandle = NULL;
			}
		}
		return;
	}

	float distToPlayer = (gClientLogic().localPlayer_->GetPosition()-GetPosition()).Length();

	if(!isSoundAudible())
	{
		DestroySounds();
	}
	else
	{
		if(ZombieAnimState == EZombieStates::ZState_Idle || ZombieAnimState == EZombieStates::ZState_Walk)
		{
			if(m_sndChaseHandle) { SoundSys.Release(m_sndChaseHandle); m_sndChaseHandle = NULL;}
			if(m_sndAttackHandle) { SoundSys.Release(m_sndAttackHandle); m_sndAttackHandle = NULL;}
			if(m_sndScreamHandle) { SoundSys.Release(m_sndScreamHandle); m_sndScreamHandle = NULL; }
			if(distToPlayer <= m_sndMaxDistIdle && !m_sndIdleHandle)
			{
				if( IsSuperZombie() )
					m_sndIdleHandle = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_IDLE"), GetPosition());
				else if ( IsDogZombie() )
					m_sndIdleHandle = SoundSys.Play(SoundSys.GetEventIDByPath(m_isIdleEating?"Sounds/WarZ/Zombie sounds/ZOMBIE_EATING":"Sounds/WarZ/Zombie sounds/ZOMBIE_IDLE_DOG"), GetPosition());
				else
					m_sndIdleHandle = SoundSys.Play(SoundSys.GetEventIDByPath(m_isIdleEating?"Sounds/WarZ/Zombie sounds/ZOMBIE_EATING":(m_isFemale?"Sounds/WarZ/Zombie sounds/ZOMBIE_IDLE_F":"Sounds/WarZ/Zombie sounds/ZOMBIE_IDLE_M")), GetPosition());
			}
			else if(distToPlayer > m_sndMaxDistIdle && m_sndIdleHandle)
			{
				SoundSys.Release(m_sndIdleHandle); m_sndIdleHandle = NULL;
			}
		}
		else if(ZombieAnimState == EZombieStates::ZState_Pursue || ZombieAnimState == EZombieStates::ZState_PursueSprint)
		{
			if(m_sndIdleHandle) { SoundSys.Release(m_sndIdleHandle); m_sndIdleHandle = NULL;}
			if(m_sndAttackHandle) { SoundSys.Release(m_sndAttackHandle); m_sndAttackHandle = NULL;}
			if(m_sndScreamHandle) { SoundSys.Release(m_sndScreamHandle); m_sndScreamHandle = NULL; }
			if(distToPlayer <= m_sndMaxDistChase && !m_sndChaseHandle)
			{
				if( IsSuperZombie() )
					m_sndChaseHandle = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_CHASING_M"), GetPosition());
				else if ( IsDogZombie() )
					m_sndChaseHandle = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_CHASING_DOG"), GetPosition());
				else
					m_sndChaseHandle = SoundSys.Play(SoundSys.GetEventIDByPath(m_isFemale?"Sounds/WarZ/Zombie sounds/ZOMBIE_CHASING_F":"Sounds/WarZ/Zombie sounds/ZOMBIE_CHASING_M"), GetPosition());
			}
			else if(distToPlayer > m_sndMaxDistChase && m_sndChaseHandle)
			{
				SoundSys.Release(m_sndChaseHandle); m_sndChaseHandle = NULL;
			}
		}
		else if(ZombieAnimState == EZombieStates::ZState_CallForHelp )
		{
			if(m_sndIdleHandle) { SoundSys.Release(m_sndIdleHandle); m_sndIdleHandle = NULL;}
			if(m_sndAttackHandle) { SoundSys.Release(m_sndAttackHandle); m_sndAttackHandle = NULL;}
			if(m_sndChaseHandle) { SoundSys.Release(m_sndChaseHandle); m_sndChaseHandle = NULL;}
			if(distToPlayer <= m_sndMaxDistScream && !m_sndScreamHandle)
			{
				if( IsSuperZombie() )
					m_sndScreamHandle = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_SCREAM"), GetPosition());
				else if ( IsDogZombie() )
					m_sndScreamHandle = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_DOG"), GetPosition());
				else
					m_sndScreamHandle = SoundSys.Play(SoundSys.GetEventIDByPath(m_isFemale?"Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_F":"Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_M"), GetPosition());
			}
			else if(distToPlayer > m_sndMaxDistScream && m_sndScreamHandle)
			{
				SoundSys.Release(m_sndScreamHandle); m_sndScreamHandle = NULL;
			}
		}
		else if(ZombieAnimState == EZombieStates::ZState_Attack || ZombieAnimState == EZombieStates::ZState_BarricadeAttack)
		{
			if(m_sndIdleHandle) { SoundSys.Release(m_sndIdleHandle); m_sndIdleHandle = NULL;}
			if(m_sndChaseHandle) { SoundSys.Release(m_sndChaseHandle); m_sndChaseHandle = NULL;}
			if(m_sndScreamHandle) { SoundSys.Release(m_sndScreamHandle); m_sndScreamHandle = NULL; }
			if(distToPlayer <= m_sndMaxDistAttack && !m_sndAttackHandle)
			{
				PlayAttackSound();
			}
			else if(distToPlayer > m_sndMaxDistAttack && m_sndAttackHandle)
			{
				SoundSys.Release(m_sndAttackHandle); m_sndAttackHandle = NULL;
			}
		}

		if(m_sndIdleHandle)
			SoundSys.SetSoundPos(m_sndIdleHandle, GetPosition());
		if(m_sndChaseHandle)
			SoundSys.SetSoundPos(m_sndChaseHandle, GetPosition());
		if(m_sndScreamHandle)
			SoundSys.SetSoundPos(m_sndScreamHandle, GetPosition());
		if(m_sndAttackHandle)
		{
			if(!SoundSys.isPlaying(m_sndAttackHandle))
			{
				if(r3dGetTime() > m_sndAttackNextPlayTime)
				{
					SoundSys.Start(m_sndAttackHandle);
					m_sndAttackNextPlayTime = r3dGetTime() + m_sndAttackLength;
				}
			}
			SoundSys.SetSoundPos(m_sndAttackHandle, GetPosition());
		}
		if(m_sndHurtHandle)
		{
			if(!SoundSys.isPlaying(m_sndHurtHandle))
			{
				SoundSys.Release(m_sndHurtHandle); m_sndHurtHandle = NULL;
			}
			else
				SoundSys.SetSoundPos(m_sndHurtHandle, GetPosition());
		}
	}
	PlaySuperZombieFootstepSound();
}

void obj_Zombie::PlayAttackSound()
{
	if(gClientLogic().localPlayer_ == NULL)
		return;

	if(!isSoundAudible())
		return;

	static int superZombieAttackSoundID		= SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_ATTACK");
	static int superZombieSuperAttackSoundID= SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_SUPER_ATTACK");
	static int femaleZombieAttackSoundID	= SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_ATTACK_F");
	static int maleZombieAttackSoundID		= SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_ATTACK_M");
	static int DogZombieAttackSoundID		= SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_ATTACK_DOG");
	if( IsSuperZombie() )
	{
		if( attackCounter == 2 )
		{
			m_sndAttackHandle = SoundSys.Play(superZombieSuperAttackSoundID, GetPosition());
		}
		else
		{
			m_sndAttackHandle = SoundSys.Play(superZombieAttackSoundID, GetPosition());
		}
	}
	else if ( IsDogZombie() )
	{
		m_sndAttackHandle = SoundSys.Play(DogZombieAttackSoundID, GetPosition());
	}
	else
	{
		m_sndAttackHandle = SoundSys.Play(m_isFemale ? femaleZombieAttackSoundID : maleZombieAttackSoundID, GetPosition());
	}
	m_sndAttackNextPlayTime = r3dGetTime() + m_sndAttackLength;
}

void obj_Zombie::PlayHurtSound()
{
	if(gClientLogic().localPlayer_ == NULL || bDead || !isSoundAudible())
		return;

	static int superZombieHurt  = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DAMAGE_M");
	static int zombieHurtMale   = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DAMAGE_M");
	static int zombieHurtFemale = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DAMAGE_F");
	static int zombieHurtDog   = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DAMAGE_DOG");

	if( IsSuperZombie() )
		m_sndHurtHandle = SoundSys.Play(superZombieHurt, GetPosition());
	else if ( IsDogZombie() )
		m_sndHurtHandle = SoundSys.Play(zombieHurtDog, GetPosition());
	else
		m_sndHurtHandle = SoundSys.Play(m_isFemale?zombieHurtFemale:zombieHurtMale, GetPosition());
}

void obj_Zombie::PlayDeathSound()
{
	if(gClientLogic().localPlayer_ == NULL || !isSoundAudible())
		return;

	static int superZombieDeath  = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DEATH_M");
	static int zombieDeathMale   = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DEATH_M");
	static int zombieDeathFemale = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DEATH_F");
	static int zombieDeathDog   = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_DAMAGE_DOG"); // falta

	if( IsSuperZombie() )
		m_sndDeathHandle = SoundSys.Play(superZombieDeath, GetPosition());
	else if ( IsDogZombie() )
		m_sndDeathHandle = SoundSys.Play(zombieDeathDog, GetPosition());
	else
		m_sndDeathHandle = SoundSys.Play(m_isFemale?zombieDeathFemale:zombieDeathMale, GetPosition());
}

void obj_Zombie::PlayScreamSound()
{
	if(gClientLogic().localPlayer_ == NULL || bDead || !isSoundAudible())
		return;

	static int superZombieScream  = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_SCREAM");
	static int zombieScreamMale   = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_M");
	static int zombieScreamFemale = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_F");
	static int zombieScreamDog = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/ZOMBIE_SCREAM_DOG");

	if( IsSuperZombie() )
		m_sndScreamHandle = SoundSys.Play(superZombieScream, GetPosition());
	else if ( IsDogZombie() )
		m_sndScreamHandle = SoundSys.Play(zombieScreamDog, GetPosition());
	else
		m_sndScreamHandle = SoundSys.Play(m_isFemale?zombieScreamFemale:zombieScreamMale, GetPosition());
}

void obj_Zombie::PlaySuperZombieAlertSound(int alert)
{
	if(gClientLogic().localPlayer_ == NULL || bDead || !isSoundAudible() || !IsSuperZombie())
		return;

	static int alertEventID = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_ALERT");
	static int alertSniffEventID = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_ALERT_SNIFF");

	switch( alert )
	{
	default:
	case EZombieStates::ZFidget_SZIdleAlert:
		SoundSys.PlayAndForget(alertEventID, GetPosition());
		break;
	case EZombieStates::ZFidget_SZIdleAlertSniff:
		SoundSys.PlayAndForget(alertSniffEventID, GetPosition());
		break;
	}
}

void obj_Zombie::PlaySuperZombieChestBeatSound()
{
	if(gClientLogic().localPlayer_ == NULL || bDead || !isSoundAudible() || !IsSuperZombie())
		return;

	static int chestBeatEventID = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_CHEST_BEAT");
	SoundSys.PlayAndForget(chestBeatEventID, GetPosition());
}

void obj_Zombie::PlaySuperZombieDestroyBarricadeSound()
{
	if(gClientLogic().localPlayer_ == NULL || bDead || !isSoundAudible() || !IsSuperZombie())
		return;

	static int destroyBarricadeEventID = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_DESTROY_BARRICADE");
	SoundSys.PlayAndForget(destroyBarricadeEventID, GetPosition());
}

void obj_Zombie::PlaySuperZombieFootstepSound()
{
	if(gClientLogic().localPlayer_ == NULL || bDead || !isSoundAudible() || !IsSuperZombie())
		return;

	static int footstepLeftEventID	= SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_FOOTSTEPS_L");
	static int footstepRightEventID	= SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_FOOTSTEPS_R");

	if( WalkTrack )
	{
		r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack( WalkTrack );
		if( ai )
		{
			int currentFrame = (int)fabs(ai->fCurFrame);
			switch( ZombieAnimStateLevel )
			{
			default:
			case 0: // Walk
				{
					const int leftFrame = 25;
					const int rightFrame = 7;
					if( leftFrame > prevWalkFrame && leftFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepLeftEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Walk Left: %d < %d <= %d\n", prevWalkFrame, leftFrame, currentFrame);
					}
					else if( rightFrame > prevWalkFrame && rightFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepRightEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Walk Right: %d < %d <= %d\n", prevWalkFrame, rightFrame, currentFrame);
					}
					else
					{
						//r3dOutToLog("!!! Super Zombie Walk No Play: %d < (%d, %d) <= %d\n", prevWalkFrame, leftFrame, rightFrame, currentFrame);
					}
				}
				break;

			case 1: // Chase Walk
				{
					const int leftFrame = 26;
					const int rightFrame = 8;
					if( leftFrame > prevWalkFrame && leftFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepLeftEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Walk2 Left: %d < %d <= %d\n", prevWalkFrame, leftFrame, currentFrame);
					}
					else if( rightFrame > prevWalkFrame && rightFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepRightEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Walk2 Right: %d < %d <= %d\n", prevWalkFrame, rightFrame, currentFrame);
					}
					else
					{
						//r3dOutToLog("!!! Super Zombie Walk2 No Play: %d < (%d, %d) <= %d\n", prevWalkFrame, leftFrame, rightFrame, currentFrame);
					}
				}
				break;
			}
			prevWalkFrame = currentFrame;
		}
	}
	else if( RunTrack )
	{
		r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack( RunTrack );
		if( ai )
		{
			int currentFrame = (int)fabs(ai->fCurFrame);
			switch( ZombieAnimStateLevel )
			{
			default:
			case 0:	// Run
				{
					const int leftFrame = 26;
					const int rightFrame = 12;
					if( leftFrame > prevRunFrame && leftFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepLeftEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Run Left: %d < %d <= %d\n", prevRunFrame, leftFrame, currentFrame);
					}
					else if( rightFrame > prevRunFrame && rightFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepRightEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Run Right: %d < %d <= %d\n", prevRunFrame, rightFrame, currentFrame);
					}
					else
					{
						//r3dOutToLog("!!! Super Zombie Run No Play: %d < (%d, %d) <= %d\n", prevRunFrame, leftFrame, rightFrame, currentFrame);
					}
				}
				break;

			case 1:	// Run Faster
				{
					const int leftFrame = 26;
					const int rightFrame = 12;
					if( leftFrame > prevRunFrame && leftFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepLeftEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Run2 Left: %d < %d <= %d\n", prevRunFrame, leftFrame, currentFrame);
					}
					else if( rightFrame > prevRunFrame && rightFrame <= currentFrame )
					{
						SoundSys.PlayAndForget(footstepRightEventID, GetPosition());
						//r3dOutToLog("!!! Super Zombie Run2 Right: %d < %d <= %d\n", prevRunFrame, rightFrame, currentFrame);
					}
					else
					{
						//r3dOutToLog("!!! Super Zombie Run2 No Play: %d < (%d, %d) <= %d\n", prevRunFrame, leftFrame, rightFrame, currentFrame);
					}
				}
				break;
			}
			prevRunFrame = currentFrame;
		}
	}
	else if( SprintTrack )
	{
		r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack( SprintTrack );
		if( ai )
		{
			int currentFrame = (int)fabs(ai->fCurFrame);
			const int leftFrame = 0;
			const int rightFrame = 15;
			if( leftFrame > prevSprintFrame && leftFrame <= currentFrame )
			{
				SoundSys.PlayAndForget(footstepLeftEventID, GetPosition());
				//r3dOutToLog("!!! Super Zombie Sprint Left: %d < %d <= %d\n", prevSprintFrame, leftFrame, currentFrame);
			}
			else if( rightFrame > prevSprintFrame && rightFrame <= currentFrame )
			{
				SoundSys.PlayAndForget(footstepRightEventID, GetPosition());
				//r3dOutToLog("!!! Super Zombie Sprint Right: %d < %d <= %d\n", prevSprintFrame, rightFrame, currentFrame);
			}
			else
			{
				//r3dOutToLog("!!! Super Zombie Sprint No Play: %d < (%d, %d) <= %d\n", prevSprintFrame, leftFrame, rightFrame, currentFrame);
			}
			prevSprintFrame = currentFrame;
		}
	}
}

void obj_Zombie::PlaySuperZombieRunAttackSound()
{
	if(gClientLogic().localPlayer_ == NULL || bDead || !isSoundAudible() || !IsSuperZombie())
		return;

	static int runAttackEventID = SoundSys.GetEventIDByPath("Sounds/WarZ/Zombie sounds/SUPER_ZOMBIE_CHASE_SWIPE");
	SoundSys.PlayAndForget(runAttackEventID, GetPosition());
}

bool obj_Zombie::isSoundAudible()
{
	float distToPlayer = (gClientLogic().localPlayer_->GetPosition()-GetPosition()).Length();
	return distToPlayer < m_sndMaxDistAll; 
}

void obj_Zombie::UpdateStart()
{
	parent::Update();
	UpdateSounds();
}

void obj_Zombie::UpdateConcurrent( int fullAnimUpdate )
{
	if(!gotLocalBbox)
	{
		r3dBoundBox bbox;
		bbox.InitForExpansion();
		for(int i=0; i < 3; i++) { // not 4 - special mesh isn't counted for bbox
			if(zombieParts[i] && zombieParts[i]->IsLoaded()) {
				bbox.ExpandTo(zombieParts[i]->localBBox);
				gotLocalBbox = true;
			} else {
				gotLocalBbox = false;
				break;
			}
		}
		if(gotLocalBbox) SetBBoxLocal(bbox);
	}

	if( g_enable_zombie_sprint->GetBool() ) // Turn on/off Sprinting
	{
		if( ZombieState == EZombieStates::ZState_PursueSprint &&
			( 0.0f < SprintTimer || 0.0f < SprintFalloffTimer ) )
		{
			float spd = CreateParams.RunSpeed;

			if( 0.0f < SprintTimer )
			{
				SprintTimer -= r3dGetFrameTime();
				if( 0.0f >= SprintTimer )
				{
					SprintTimer = -1.0f;
					SprintFalloffTimer = CreateParams.SprintMaxTime * 0.5f;
				}
				else
				{
					//spd = -CreateParams.SprintMaxSpeed /( 1 + pow( CreateParams.SprintMaxTime, 2.0f + ( CreateParams.SprintSlope - 1.0f ) * 4.0f ) * (float)pow( M_E, (double)-( SprintTimer * CreateParams.SprintSlope ) ) ) + CreateParams.SprintMaxSpeed;
					double xAxis = ( CreateParams.SprintSlope - 1 ) * CreateParams.SprintMaxTime;
					double halfTime = ( CreateParams.SprintMaxTime * 0.5 );
					if( xAxis < halfTime )
						xAxis = halfTime;
					double exp = -CreateParams.SprintSlope * SprintTimer + xAxis;
					spd = -CreateParams.SprintMaxSpeed /( 1 + (float)pow( M_E, exp ) ) + CreateParams.SprintMaxSpeed;
					if( spd < CreateParams.RunSpeed )
						spd = CreateParams.RunSpeed;
				}
			}
			else if( 0.0f < SprintFalloffTimer )
			{
				SprintFalloffTimer -= r3dGetFrameTime();
				if( 0.0f >= SprintFalloffTimer )
				{
					SprintFalloffTimer = -1.0f;
					spd = CreateParams.RunSpeed;
				}
				else
				{
					spd = CreateParams.SprintMaxSpeed * (float)pow( M_E, ( ( SprintFalloffTimer - CreateParams.SprintMaxSpeed ) * 0.25 ) );
				}
			}
			if(IsSuperZombie() || !staggeredTrack)
			{
				// calc velocity to reach position on time for next update
				r3dPoint3D vel = netMover.GetVelocityToNetTarget(
					GetPosition(),
					spd * 1.5f,
					1.0f);
				SetVelocity(vel);
			}
		}
	}

	if( !turnInterp.IsFinished() )
	{
		turnInterp.Update();
		SetFacingVector( turnInterp.GetCurrent() );
	}

	// Update the speed of the animation, to prevent sliding.
	if( ( IsSuperZombie() || !staggeredTrack ) && ( EZombieStates::ZState_Walk == ZombieAnimState || EZombieStates::ZState_Pursue == ZombieAnimState || EZombieStates::ZState_PursueSprint == ZombieAnimState ) )
	{
		if ( IsDogZombie() )
		VelocityDog = GetVelocity().Length();
		float animMovementSpeed = 1.0f;
		switch( ZombieAnimState )
		{
		case EZombieStates::ZState_Walk:
			if( IsSuperZombie() )
				animMovementSpeed = (ZombieAnimStateLevel > 0) ? 2.3f : 1.35f;
			else if ( IsDogZombie() )
				animMovementSpeed = 1.35f;
			else
				animMovementSpeed = CreateParams.FastZombie ? 1.358f : 0.97f; // THESE VALUES MUST MATCH THE MOVEMENT RATE IN THE WALK ANIMS
			break;
		case EZombieStates::ZState_Pursue:
			if( IsSuperZombie() )
				animMovementSpeed = (ZombieAnimStateLevel > 0) ? 5.4f : 2.9f;
			else if ( IsDogZombie() )
				animMovementSpeed = 2.9f;
			else
				animMovementSpeed = CreateParams.FastZombie ? 2.37f : 2.17f; // THESE VALUES MUST MATCH THE MOVEMENT RATE IN THE RUN ANIMS
			break;
		case EZombieStates::ZState_PursueSprint:
			if( IsSuperZombie() )
				animMovementSpeed = 5.8f;
			else if ( IsDogZombie() )
				animMovementSpeed = 2.9f;
			else
				animMovementSpeed = ( ZombieAnimGroup == 0 ) ? 3.2f : 3.066f; // THESE VALUES MUST MATCH THE MOVEMENT RATE IN THE SPRINT ANIMS
			break;
		}

		for(r3dAnimation::AnimTracksVec::iterator iter = anim_.AnimTracks.begin();
			iter != anim_.AnimTracks.end(); ++iter)
		{
			if( iter->GetStatus() & ANIMSTATUS_Playing )
			{
				const r3dAnimData* pAnimData = iter->GetAnim();
				iter->fSpeed = R3D_MAX(0.01f, pAnimData->GetNumFrames() / pAnimData->GetFrameRate() * GetVelocity().Length() / animMovementSpeed );
			}
		}

		ForceIdleAnimCheck();
	}
	else if ( ( IsSuperZombie() || !staggeredTrack ) && EZombieStates::ZState_Attack == ZombieAnimState )
	{
		if( IsSuperZombie() && attackTrack )
		{
			r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack( attackTrack );
			if( ai )
			{
				DWORD status = ai->GetStatus();
				if( (status & ANIMSTATUS_Finished) > 0 )
				{
					attackCounter++;
					attackCounter %= 3;

					if( attackCounter == 2 )
					{
						holdAttackTrack = attackTrack;
						attackTrack = PlaySuperZombieSuperAttackAnim();
					}
					else
					{
						// Resume the Normal Attack.
						if( holdAttackTrack != 0 )
							attackTrack = holdAttackTrack;
						r3dAnimation::r3dAnimInfo* ai2 = anim_.GetTrack( attackTrack );
						if( ai2 )
						{
							DWORD status2 = ai2->GetStatus();
							status2 &= ~ANIMSTATUS_Finished;
							ai2->SetStatus(status2);
							ai2->fCurFrame = 0.0f;
							ai2->fInfluence = 1.0f;
						}
						// Release the Super Attack anim.
						if( holdAttackTrack != 0 )
						{
							status &= ~ANIMSTATUS_Playing;
							ai->SetStatus(status);

							holdAttackTrack = 0;
						}
					}
					m_sndAttackLength = float(anim_.GetTrack(attackTrack)->pAnim->GetNumFrames())/anim_.GetTrack(attackTrack)->pAnim->GetFrameRate();
					PlayAttackSound();
				}
			}
		}
	}
	UpdateAnimations( UpdateWarmUp && !fullAnimUpdate );

	ProcessMovement();

	wasVisible = 0;
}

void obj_Zombie::UpdateStop()
{
	if(!bDead)
	{
		UpdatePlayerObstacle(PhysXObstacleIndex, GetPosition());
	}
}

BOOL obj_Zombie::Update()
{	
	UpdateStart();
	UpdateConcurrent( 0 );
	UpdateStop();

	return TRUE;
}

void obj_Zombie::ProcessMovement()
{
	r3d_assert(!NetworkLocal);
	
	float timePassed = r3dGetFrameTime();
	
	if(GetVelocity().LengthSq() > 0.0001f)
	{
		R3DPROFILE_FUNCTION("NetZombie_Move");
		r3dPoint3D nextPos = GetPosition() + GetVelocity() * timePassed;
		
		// check if we overmoved to target position
		r3dPoint3D v = netMover.GetNetPos() - nextPos;
		float d = GetVelocity().Dot(v);
		if(d < 0) {
			nextPos = netMover.GetNetPos();
			SetVelocity(r3dPoint3D(0, 0, 0));
		}

#if 0
		// adjust zombie to geometry, as it is walking on navmesh on server side - looking jerky right now...
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eDYNAMIC|PxSceneQueryFilterFlag::eSTATIC));
		PxSceneQueryFlags queryFlags(PxSceneQueryFlag::eIMPACT);
		PxRaycastHit hit;
		if (g_pPhysicsWorld->raycastSingle(PxVec3(nextPos.x, nextPos.y + 0.5f, nextPos.z), PxVec3(0, -1, 0), 1, queryFlags, hit, filter))
		{
			nextPos.y = hit.impact.y;
		}
#endif
		SetPosition(nextPos);
	}
	AvgVelocity.SetCurSample( GetVelocity() );
#ifndef FINAL_BUILD
		DebugData data( (EZombieStates::EGlobalStates)ZombieState, (EZombieStates::EGlobalStates)ZombieAnimState, GetPosition(), GetVelocity() );
		m_debugHistory.Add( data );
#endif
}

void obj_Zombie::CancelSprint()
{
	if( g_enable_zombie_sprint->GetBool() )
	{
		// Reset the sprint timers, and animation speeds.
		if( 0.0f < SprintTimer || 0.0f < SprintFalloffTimer )
		{
			SprintTrack = 0;
			SprintTimer = -1.0;
			SprintFalloffTimer = -1.0f;
			r3dAnimation::AnimTracksVec::iterator it;
			for(it = anim_.AnimTracks.begin(); it != anim_.AnimTracks.end(); ++it) 
			{
				r3dAnimation::r3dAnimInfo &ai = *it;
				ai.fSpeed = 1.0f;
			}
		}
	}
}

void obj_Zombie::SetFacingVector( const r3dPoint3D& v )
{
	float angle = atan2f(-v.z, v.x);
	angle = R3D_RAD2DEG(angle);
	SetRotationVector(r3dVector(angle - 90, 0, 0));
}

r3dVector obj_Zombie::GetFacingVector() const
{
	r3dVector fwd( 0, 0, -1 );
	fwd.RotateAroundY( GetRotationVector().x );
	return fwd;
}

r3dVector obj_Zombie::GetRightVector() const
{
	r3dVector fwd( 0, 0, -1 );
	fwd.RotateAroundY( GetRotationVector().x + 90.0f );
	return fwd;
}

int obj_Zombie::AddAnimation(const char* anim)
{
	char buf[MAX_PATH];
	sprintf(buf, "Data\\Animations5\\%s.anm", anim);
	int aid = g_zombieAnimPool[ m_typeIndex ]->Add(anim, buf);
	
	return aid;
}	


void obj_Zombie::RecalcAnimations( int advanceOnly )
{
	if( wasVisible || !UpdateWarmUp )
	{
		const float TimePassed = r3dGetFrameTime();

		D3DXMATRIX CharDrawMatrix;
		D3DXMatrixIdentity(&CharDrawMatrix);
		anim_.Update(TimePassed, r3dPoint3D(0,0,0), CharDrawMatrix);

		if( !advanceOnly )
		{
			anim_.Recalc();
		}
	}	
}

void obj_Zombie::UpdateAnimations( int advanceOnly )
{
	float zombRadiusSqr = g_zombie_update_radius->GetFloat();
	zombRadiusSqr *= zombRadiusSqr;

	if ( IsDogZombie() )
	{
	r3dPhysDogSkeleton* phySkeleton = GetPhysSkeletonDog();

	if( ( gCam - GetPosition() ).LengthSq() > zombRadiusSqr )
	{
		if( UpdateWarmUp > 5 )
		{
			if( PhysicsOn )
			{
				if( phySkeleton ) 
					phySkeleton->SetBonesActive( false );

				PhysicsOn = 0;
			}
			return;
		}
	}

	int bonesJustEnabled = 0;

	if( !PhysicsOn )
	{
		if( phySkeleton )
		{
			phySkeleton->SetBonesActive( true );
			bonesJustEnabled = 1;
		}
		PhysicsOn = 1;
	}

	UpdateWarmUp ++;

#if ENABLE_RAGDOLL
	bool ragdoll = phySkeleton && phySkeleton->IsRagdollMode();
	if (!ragdoll)
#endif
	{
		RecalcAnimations( advanceOnly );
	}

	int doPhysics = ( !advanceOnly || bonesJustEnabled );

	if( phySkeleton && PhysicsOn && doPhysics )
		phySkeleton->syncAnimation(anim_.GetCurrentSkeleton(), GetTransformMatrix(), anim_);

#if ENABLE_RAGDOLL
	if( ragdoll && doPhysics )
	{
		r3dBoundBox bbox = phySkeleton->getWorldBBox();
		bbox.Org -= GetPosition();
		SetBBoxLocal(bbox);
	}
#endif

	}
	else {

	r3dPhysSkeleton* phySkeleton = GetPhysSkeleton();

	if( ( gCam - GetPosition() ).LengthSq() > zombRadiusSqr )
	{
		if( UpdateWarmUp > 5 )
		{
			if( PhysicsOn )
			{
				if( phySkeleton ) 
					phySkeleton->SetBonesActive( false );

				PhysicsOn = 0;
			}
			return;
		}
	}

	int bonesJustEnabled = 0;

	if( !PhysicsOn )
	{
		if( phySkeleton )
		{
			phySkeleton->SetBonesActive( true );
			bonesJustEnabled = 1;
		}
		PhysicsOn = 1;
	}

	UpdateWarmUp ++;

#if ENABLE_RAGDOLL
	bool ragdoll = phySkeleton && phySkeleton->IsRagdollMode();
	if (!ragdoll)
#endif
	{
		RecalcAnimations( advanceOnly );
	}

	int doPhysics = ( !advanceOnly || bonesJustEnabled );

	if( phySkeleton && PhysicsOn && doPhysics )
		phySkeleton->syncAnimation(anim_.GetCurrentSkeleton(), GetTransformMatrix(), anim_);

#if ENABLE_RAGDOLL
	if( ragdoll && doPhysics )
	{
		r3dBoundBox bbox = phySkeleton->getWorldBBox();
		bbox.Org -= GetPosition();
		SetBBoxLocal(bbox);
	}
#endif
	}

}

void obj_Zombie::UpdateZombieAnimForVelocity()
{
	float walkIdleThreshold = CreateParams.FastZombie ? 0.01f : 0.01f;
	float idleWalkThreshold = CreateParams.WalkSpeed * ( CreateParams.FastZombie ? 0.02857f : 0.05263f );
	float runWalkThreshold = CreateParams.WalkSpeed * ( CreateParams.FastZombie ? 1.4f : 1.665f );
	float walkRunThreshold = CreateParams.RunSpeed * ( CreateParams.FastZombie ? 1.0f : 0.745f );
	float sprintRunThreshold = CreateParams.RunSpeed * ( CreateParams.FastZombie ? 1.2f : 1.15f );
	float runSprintThreshold = CreateParams.RunSpeed * ( CreateParams.FastZombie ? 1.4f : 1.35f );
	float vel = GetVelocity().Length();
	if( ZombieAnimState != EZombieStates::ZState_Idle && vel < walkIdleThreshold ) {
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetIdleTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartIdleAnim();
		}
		ZombieReqAnimTimers.ReqIdle();
	} else if( ( ZombieAnimState != EZombieStates::ZState_Walk && vel > idleWalkThreshold && vel < runWalkThreshold ) ||
			   ( ZombieAnimState == EZombieStates::ZState_Idle && vel > idleWalkThreshold ) ) {
		if( ( ZombieAnimState == EZombieStates::ZState_Idle && vel > idleWalkThreshold ) ||
			( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetWalkTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) ) {
			StartWalkAnim();
		}
		ZombieReqAnimTimers.ReqWalk();
	} else if( ZombieAnimState != EZombieStates::ZState_Pursue && vel > walkRunThreshold && ( vel < sprintRunThreshold || 0.0f > SprintTimer ) ) {
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetRunTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartRunAnim();
		}
		ZombieReqAnimTimers.ReqRun();
	} else if( ZombieAnimState != EZombieStates::ZState_PursueSprint && vel > runSprintThreshold && 0.0f < SprintTimer ) {
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetSprintTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartSprintAnim();
		}
		ZombieReqAnimTimers.ReqSprint();
	}
}

void obj_Zombie::UpdateSuperZombieAnimForVelocity()
{
	float walk1IdleThreshold	= 0.01f;														// 0.01f				
	float idleWalk1Threshold	= CreateParams.WalkSpeed * 0.02f;		// 1.9f * 0.02f	= 0.038f						
	float walk2Walk1Threshold	= CreateParams.WalkSpeed * 0.78f;								// 1.9f * 0.78f	= 1.482f
	float walk1Walk2Threshold	= CreateParams.WalkSpeed * 1.03f;		// 1.9f * 1.03f	= 1.957f						
	float run1Walk2Threshold	= CreateParams.WalkSpeed * 1.18f;								// 1.9f * 1.18f	= 2.242f
	float walk2Run1Threshold	= CreateParams.RunSpeed * 0.6f;			// 5.4f * 0.6f	= 3.24f							
	float run2Run1Threshold		= CreateParams.RunSpeed * 0.76f;								// 5.4f * 0.76f	= 4.104f
	float run1Run2Threshold		= CreateParams.RunSpeed * 1.0f;			// 5.4f * 1.0f	= 5.4f							
	float sprint1Run2Threshold	= CreateParams.RunSpeed * 1.15f;								// 5.4f * 1.15f	= 6.21f	
	float run2Sprint1Threshold	= CreateParams.RunSpeed * 1.3f;			// 5.4f * 1.3f	= 7.02f							
	float sprint2Sprint1Threshold = CreateParams.RunSpeed * 1.5f;								// 5.4f * 1.5f	= 8.1f	
	float sprint1Sprint2Threshold = CreateParams.RunSpeed * 1.7f;		// 5.4f * 1.7f	= 9.18f							
	float vel = GetVelocity().Length();																				
	if( ZombieAnimState != EZombieStates::ZState_Idle && vel < walk1IdleThreshold ) {
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetIdleTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartIdleAnim();
	}	
		ZombieReqAnimTimers.ReqIdle();
	} else if( ( ZombieAnimState != EZombieStates::ZState_Walk && vel > idleWalk1Threshold && vel < walk1Walk2Threshold ) ||
			   ( ZombieAnimState == EZombieStates::ZState_Idle && vel > idleWalk1Threshold ) ) {
		if( ( ZombieAnimState == EZombieStates::ZState_Idle && vel > idleWalk1Threshold ) ||
			( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetWalkTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) ) {
			StartWalkAnim();
		}
		ZombieReqAnimTimers.ReqWalk();
	} else if( ( ZombieAnimState != EZombieStates::ZState_Walk && vel > walk2Walk1Threshold && vel < run1Walk2Threshold ) ||
			   ( ZombieAnimState == EZombieStates::ZState_Walk && ZombieAnimStateLevel != 1 && vel > walk1Walk2Threshold ) ) {
		if( ( ZombieAnimState == EZombieStates::ZState_Walk && vel > walk1Walk2Threshold ) ||
			( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetWalkTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) ) {
			StartWalk2Anim();
		}
		ZombieReqAnimTimers.ReqWalk();
	} else if( ZombieAnimState != EZombieStates::ZState_Pursue && vel > walk2Run1Threshold && vel < run1Run2Threshold ) {
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetRunTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartRunAnim();
		}
		ZombieReqAnimTimers.ReqRun();
	} else if( ( ZombieAnimState != EZombieStates::ZState_Pursue && vel > run2Run1Threshold && ( vel < sprint1Run2Threshold || 0.0f > SprintTimer ) ) ||
			   ( ZombieAnimState == EZombieStates::ZState_Pursue && ZombieAnimStateLevel != 1 && vel > run1Run2Threshold ) ) {
		if( ( ZombieAnimState == EZombieStates::ZState_Pursue && vel > run1Run2Threshold ) ||
			( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetRunTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) ) {
			StartRun2Anim();
		}
		ZombieReqAnimTimers.ReqRun();
	} else if( ZombieAnimState != EZombieStates::ZState_PursueSprint && vel > run2Sprint1Threshold && vel < sprint1Sprint2Threshold ) {
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetSprintTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartSprintAnim();
		}
		ZombieReqAnimTimers.ReqSprint();
	} else if( ZombieAnimState != EZombieStates::ZState_PursueSprint && vel > sprint2Sprint1Threshold && ( vel < sprint1Sprint2Threshold || 0.0f < SprintTimer ) ) {
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetSprintTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartSprint2Anim();
		}
		ZombieReqAnimTimers.ReqSprint();
	}
}

void obj_Zombie::UpdateDogAnimForVelocity()
{
	float walkIdleThreshold = 0.01f;
	float idleWalkThreshold = CreateParams.WalkSpeed * 0.02f;	
	float runWalkThreshold = CreateParams.WalkSpeed * 1.38f;
	float walkRunThreshold = CreateParams.RunSpeed * 0.6f;
	float sprintRunThreshold = CreateParams.RunSpeed * 1.15f;
	float runSprintThreshold = CreateParams.RunSpeed * 1.5f;
	float vel = GetVelocity().Length();
	if ( ZombieAnimState != EZombieStates::ZState_Idle && vel < idleWalkThreshold )
	{
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetIdleTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartIdleAnim();
		}
		ZombieReqAnimTimers.ReqIdle();
	}
	else if ( ZombieAnimState != EZombieStates::ZState_Walk && vel > walkIdleThreshold && vel < runWalkThreshold ||
		      ZombieAnimState == EZombieStates::ZState_Idle && vel > walkIdleThreshold && vel < runWalkThreshold )
	{
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetWalkTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartWalkAnim();
		}
		ZombieReqAnimTimers.ReqWalk();
	}
	else if ( ZombieAnimState != EZombieStates::ZState_Pursue && vel > runWalkThreshold && vel < walkRunThreshold ||
		      ZombieAnimState == EZombieStates::ZState_Idle && vel > runWalkThreshold && vel < walkRunThreshold )
	{
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetRunTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartRunAnim();
		}
		ZombieReqAnimTimers.ReqRun();
	}
	else if ( ZombieAnimState != EZombieStates::ZState_PursueSprint && vel > walkRunThreshold ||
		      ZombieAnimState == EZombieStates::ZState_Idle && vel > walkRunThreshold )
	{
		if( ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetSprintTime() || ZombieReqAnimTimers.MinCurAnimTime < ZombieReqAnimTimers.GetCurAnimTime() ) {
			StartSprintAnim();
		}
		ZombieReqAnimTimers.ReqSprint();
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_Zombie::OnDestroy()
{
	parent::OnDestroy();

	if ( IsDogZombie() )
		SwitchPhysToRagdoll( true,&r3dPoint3D(0,0,0), 2, false );

	ReleasePlayerObstacle(&PhysXObstacleIndex);

	UnlinkSkeleton();

	DestroySounds();

	for( int i = 0, e = (int)ZombieList.Count(); i < e; i ++ )
	{
		if( ZombieList[ i ].zombie == this )
		{
			ZombieList.Erase( i );
			break;
		}
	}

	return TRUE;
}

void obj_Zombie::DoDeath()
{
	if ( IsDogZombie() )
	{
		int aid = 0;
		switch(u_random(2))
		{
			default:
			case 0:	aid = AddAnimation("hunzi_death_01"); break;
			case 1:	aid = AddAnimation("hunzi_death_02"); break;
		}
		anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherNow | ANIMFLAG_PauseOnEnd, 1.0f, 1.0f, 1.0f);
	}

	bDead = true;

	ReleasePlayerObstacle(&PhysXObstacleIndex);

	r3dPoint3D hitForce = lastTimeHitPos;//(GetPosition() - lastTimeHitPos).NormalizeTo();

	float dmgForce = 10.0f;
	bool isVehicleHit = false;
	switch(lastTimeDmgType)
	{
	case storecat_ASR:
		dmgForce = 30.0f;
		break;
	case storecat_SNP:
		dmgForce = 60.0f;
		break;
	case storecat_SHTG:
		dmgForce = 60.0f;
		break;
	case storecat_MG:
		dmgForce = 40.0f;
		break;
	case storecat_HG:
		dmgForce = 10.0f;
		break;
	case storecat_SMG:
		dmgForce = 20.0f;
		break;
	case storecat_GRENADE:
		isVehicleHit = true;
		lastTimeHitBoneID = 2;
		hitForce = r3dPoint3D(200,200,200);
		break;
	}
	
#ifdef VEHICLES_ENABLED
	if (ZombieState == EZombieStates::ZState_FakeDead)
		isVehicleHit = true;
#endif

	if (!isVehicleHit)
		hitForce *= dmgForce*2;

	// switch to ragdoll with random bone
	if ( !IsDogZombie() )
	SwitchPhysToRagdoll( true, &hitForce, lastTimeHitBoneID, isVehicleHit );

	// clear rotation so it won't affect ragdoll bbox
	SetRotationVector(r3dPoint3D(0, 0, 0));
	
	DestroySounds();
	PlayDeathSound();

	ZombieAnimState = EZombieStates::ZState_Dead;
	if ( !IsDogZombie() )
	anim_.StopAll();
}

r3dPhysSkeleton* obj_Zombie::GetPhysSkeleton() const
{
	if( physSkeletonIndex >= 0 )
		return PhysSkeletonCache[ physSkeletonIndex ].skeleton;
	else
		return NULL;
}

r3dPhysDogSkeleton* obj_Zombie::GetPhysSkeletonDog() const
{
	if( physSkeletonIndex >= 0 )
		return PhysSkeletonCache[ physSkeletonIndex ].skeletonDog;
	else
		return NULL;
}

void obj_Zombie::ForceIdleAnimCheck()
{
	bool animFrozen = false;
	if( AvgVelocity.GetAverage().Length() < 0.5f )
	{
		r3dString animsPlaying;
		for(r3dAnimation::AnimTracksVec::const_iterator iter = anim_.AnimTracks.begin();
			iter != anim_.AnimTracks.end(); ++iter)
		{
			if( iter->GetStatus() & ANIMSTATUS_Playing && iter->GetSpeed() < 0.015f )
			{
				animFrozen = true;
				break;
			}
		}
	}
	// if the zombie is not moving, force the Idle anim, as long as this is not a back and forth issue.
	if( ZombieAnimState != EZombieStates::ZState_Idle && ( animFrozen || AvgVelocity.GetAverage().Length() < 0.01f || ZombieState == EZombieStates::ZState_Idle ) ) {
		if( animFrozen || ZombieReqAnimTimers.MinReqTime < ZombieReqAnimTimers.GetIdleTime() || ZombieState == EZombieStates::ZState_Idle ) {
			StartIdleAnim();
			// Don't mess with the Velocity or Average Velocity when it's just a frozen animation,
			// let those go to zero naturally.
			if( !animFrozen )
			{
				if( EZombieStates::ZState_Idle == ZombieState) {
					SetVelocity(r3dPoint3D(0, 0, 0));
				}
				AvgVelocity.Reset();
			}
		}
		ZombieReqAnimTimers.ReqIdle();
	}
}

int obj_Zombie::StartIdleAnim()
{
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;
	if( IsSuperZombie() )
	{
		aid = AddAnimation("Super_Zombie_Idle_01");
	}
	else if ( IsDogZombie() )
	{
		switch(u_random(3))
		{
		default:
		case 0:	aid = AddAnimation("hunzi_idle_01"); break;
		case 1:	aid = AddAnimation("hunzi_idle_02"); break;
		case 2:	aid = AddAnimation("hunzi_idle_03"); break;
		}
	}
	else
	{
		switch(ZombieAnimGroup)
		{
		default:
		case 0:	aid = AddAnimation("Zombie_Idle_01"); break;
		case 1:	aid = AddAnimation("Zombie_Idle_02"); break;
		case 2:	aid = AddAnimation("Zombie_Idle_03"); break;
		}
	}

	int tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.2f);

	// start with randomized frame
	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
		ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);

	ZombieAnimStateLevel = 0;
	ZombieAnimState = EZombieStates::ZState_Idle;

	return tr;
}

int obj_Zombie::StartIdleCrouchedAnim()
{
	int aid = 0;
	m_isIdleCrouched = true;
	m_isIdleEating = false;
	if( IsSuperZombie() )
	{
		aid = AddAnimation("Super_Zombie_Idle_04");
	}
	else if ( IsDogZombie() )
	{
		switch(u_random(3))
		{
		default:
		case 0:	aid = AddAnimation("hunzi_idle_01"); break;
		case 1:	aid = AddAnimation("hunzi_idle_02"); break;
		case 2:	aid = AddAnimation("hunzi_idle_03"); break;
		}
	}
	else
	{
		switch(ZombieAnimGroup)
		{
		default:
		case 0: aid = AddAnimation("Zombie_Crouch_Idle_1"); break;
		case 1: aid = AddAnimation("Zombie_Crouch_Idle_2"); break;
		case 2: aid = AddAnimation("Zombie_Crouch_Idle_3"); break;
		}
	}

	int tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.2f);

	// start with randomized frame
	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
		ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
	ZombieAnimState = EZombieStates::ZState_Idle;

	return tr;
}

int obj_Zombie::StartIdleEatCrouchedAnim()
{
	int aid = 0;
	m_isIdleCrouched = true;
	m_isIdleEating = true;

	if ( IsDogZombie() )
	{
		aid = AddAnimation("hunzi_eat_husi");
	}
	else {
		switch(u_random(3))
		{
		default:
		case 0: aid = AddAnimation("Zombie_Eat_Crouched_01"); break;
		case 1: aid = AddAnimation("Zombie_Eat_Crouched_02"); break;
		case 2: aid = AddAnimation("Zombie_Eat_Crouched_03"); break;
		}
	}

	int tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.2f);

	// start with randomized frame
	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
		ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
	ZombieAnimState = EZombieStates::ZState_Idle;

	return tr;
}

void obj_Zombie::StartWalkAnim()
{
#ifndef FINAL_BUILD
	m_minSpeed = 1000.0f;
	m_maxSpeed = 0.0f;
#endif
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	if( IsSuperZombie() )
	{
		switch(u_random(3))
		{
		default:
		case 0: aid = AddAnimation("Super_Zombie_walk_01"); break;
		case 1: aid = AddAnimation("Super_Zombie_walk_02"); break;
		case 2: aid = AddAnimation("Super_Zombie_walk_03"); break;
		}
	}
	else if ( IsDogZombie() )
	{
		switch(u_random(3))
		{
		default:
		case 0: aid = AddAnimation("hunzi_walk"); break;
		case 1: aid = AddAnimation("hunzi_walk_injured"); break;
		case 2: aid = AddAnimation("hunzi_walk_silent"); break;
		}
	}
	else
	{
		switch( ZombieAnimGroup )
		{
		default:
		case 0: aid = AddAnimation("Zombie_walk_01"); break;
		case 1: aid = AddAnimation("Zombie_walk_02"); break;
		case 2: aid = AddAnimation("Zombie_walk_03"); break;
		/* TODO: Waiting on fixed anims
		case 3: aid = AddAnimation("Zombie_walk_LegDrag"); break;
		case 4: aid = AddAnimation("Zombie_walk_Wander"); break;
		*/
		}
	}

	int tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);

	// start with randomized frame
	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
	{
		ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
		prevWalkFrame = R3D_MAX(0, (int)fabs(ai->fCurFrame) - 1);
	}

	WalkTrack = tr;
	RunTrack = 0;
	SprintTrack = 0;
	ZombieAnimStateLevel = 0;
	ZombieAnimState = EZombieStates::ZState_Walk;
}

void obj_Zombie::StartWalk2Anim()
{
	if( !IsSuperZombie() ||
		( ZombieAnimState == EZombieStates::ZState_Walk && ZombieAnimStateLevel == 1 ) )
		return;

#ifndef FINAL_BUILD
	m_minSpeed = 1000.0f;
	m_maxSpeed = 0.0f;
#endif
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	switch(u_random(3))
	{
	default:
	case 0: aid = AddAnimation("Super_Zombie_ChaseWalk"); break;
	case 1: aid = AddAnimation("Super_Zombie_ChaseWalk"); break;
	case 2: aid = AddAnimation("Super_Zombie_ChaseWalk"); break;
	}

	int tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);

	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
	{
		r3dAnimation::r3dAnimInfo* aiWalk1 = anim_.GetTrack(WalkTrack);
		if( ZombieAnimState == EZombieStates::ZState_Walk && aiWalk1 )
		{
			ai->fCurFrame = aiWalk1->fCurFrame;
		}
		else
		{
			// start with randomized frame
			ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
			prevWalkFrame = R3D_MAX(0, (int)fabs(ai->fCurFrame) - 1);
		}
	}

	WalkTrack = tr;
	RunTrack = 0;
	SprintTrack = 0;
	ZombieAnimStateLevel = 1;
	ZombieAnimState = EZombieStates::ZState_Walk;
}

void obj_Zombie::StartRunAnim()
{
#ifndef FINAL_BUILD
	m_minSpeed = 1000.0f;
	m_maxSpeed = 0.0f;
#endif
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	if( IsSuperZombie() )
	{
		switch(u_random(1 /*2*/))
		{
		default:
		case 0: aid = AddAnimation("Super_Zombie_run_01"); break;
		case 1: aid = AddAnimation("Super_Zombie_run_02"); break;
		}
	}
	else if ( IsDogZombie() )
	{
		aid = AddAnimation("hunzi_run");
	}
	else
	{
		switch( ZombieAnimGroup )
		{
		default:
		case 0:
			switch( u_random(2) )
			{
			default:
			case 0: aid = AddAnimation("Zombie_run_02"); break;
			//case 1: aid = AddAnimation("Zombie_run_6"); break;
			}
			break;
		case 1:
			switch( u_random(2) )
			{
			default:
			case 0: aid = AddAnimation("Zombie_run_01"); break;
			case 1: aid = AddAnimation("Zombie_run_5"); break;
			}
			break;
		case 2:
			aid = AddAnimation("Zombie_run_03");
			break;
		};
	}

	int tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);

	// start with randomized frame
	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
	{
		ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
		prevRunFrame = R3D_MAX(0, (int)fabs(ai->fCurFrame) - 1);
	}

	WalkTrack = 0;
	RunTrack = tr;
	SprintTrack = 0;
	ZombieAnimStateLevel = 0;
	ZombieAnimState = EZombieStates::ZState_Pursue;
}

void obj_Zombie::StartRun2Anim()
{
	if( !IsSuperZombie() ||
		( ZombieAnimState == EZombieStates::ZState_Pursue && ZombieAnimStateLevel == 1 ) )
		return;

#ifndef FINAL_BUILD
	m_minSpeed = 1000.0f;
	m_maxSpeed = 0.0f;
#endif
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	switch(u_random(1 /*2*/))
	{
	default:
	case 0: aid = AddAnimation("Super_Zombie_Run_01_Faster"); break;
	//case 1: aid = AddAnimation("Super_Zombie_run_02"); break;
	}
	int tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);

	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
	{
		r3dAnimation::r3dAnimInfo* aiRun1 = anim_.GetTrack(RunTrack);
		if( ZombieAnimState == EZombieStates::ZState_Pursue && aiRun1 )
		{
			ai->fCurFrame = aiRun1->fCurFrame;
		}
		else
		{
			// start with randomized frame
			ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
			prevRunFrame = R3D_MAX(0, (int)fabs(ai->fCurFrame) - 1);
		}
	}

	WalkTrack = 0;
	RunTrack = tr;
	SprintTrack = 0;
	ZombieAnimStateLevel = 1;
	ZombieAnimState = EZombieStates::ZState_Pursue;
}

void obj_Zombie::StartSprintAnim()
{
#ifndef FINAL_BUILD
	m_minSpeed = 1000.0f;
	m_maxSpeed = 0.0f;
#endif
	int aid = 0;
	int aid2 = -1;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	if( IsSuperZombie() )
	{
		aid = AddAnimation("Super_Zombie_Sprint_1");
		aid2 = -1;
	}
	else if ( IsDogZombie() )
	{
		aid = AddAnimation("hunzi_run_fast");
		aid2 = -1;
	}
	else
	{
		switch( ZombieAnimGroup )
		{
		default:
		case 0:
			aid = AddAnimation("Zombie_Sprint_01");
			aid2 = AddAnimation("Zombie_Run_Sprint_Trans");
			break;
		case 1:
			aid = AddAnimation("Zombie_Sprint_02");
			aid2 = AddAnimation("Zombie_Run_Sprint_2_Trans");
			break;
		case 2:
			aid = AddAnimation("Zombie_Sprint_03");
			aid2 = AddAnimation("Zombie_Run_Sprint_3_Trans");
			break;
		};
	}

	int tr;
	if( aid2 >= 0 )
	{
		tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 1.5f);
		int tr2 = anim_.StartAnimation(aid2, 0, 0.0f, 1.0f, 0.1f);

		// start transition with first frame
		r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr2);
		ai->fCurFrame = 0;
	}
	else
	{
		tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);
	}

	if( 0.0f < SprintTimer )
		SprintTrack = tr;

	// start with randomized frame
	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
	{
		ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
		prevSprintFrame = R3D_MAX(0, (int)fabs(ai->fCurFrame) - 1);
	}

	WalkTrack = 0;
	RunTrack = 0;
	ZombieAnimStateLevel = 0;
	ZombieAnimState = EZombieStates::ZState_PursueSprint;
}

void obj_Zombie::StartSprint2Anim()
{
	if( !IsSuperZombie() ||
		( ZombieAnimState == EZombieStates::ZState_PursueSprint && ZombieAnimStateLevel == 1 ) )
		return;

#ifndef FINAL_BUILD
	m_minSpeed = 1000.0f;
	m_maxSpeed = 0.0f;
#endif
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	aid = AddAnimation("Super_Zombie_Sprint_1");

	int tr;
	tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.1f);

	if( 0.0f < SprintTimer )
		SprintTrack = tr;

	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	if( ai )
	{
		r3dAnimation::r3dAnimInfo* aiSprint1 = anim_.GetTrack(SprintTrack);
		if( ZombieAnimState == EZombieStates::ZState_PursueSprint && aiSprint1 )
		{
			ai->fCurFrame = aiSprint1->fCurFrame;
		}
		else
		{
			// start with randomized frame
			ai->fCurFrame = u_GetRandom(0, (float)ai->pAnim->NumFrames - 1);
			prevSprintFrame = R3D_MAX(0, (int)fabs(ai->fCurFrame) - 1);
		}
	}

	WalkTrack = 0;
	RunTrack = 0;
	ZombieAnimStateLevel = 1;
	ZombieAnimState = EZombieStates::ZState_PursueSprint;
}

void obj_Zombie::StartAttackAnim()
{
	// select new attack animation
	int aid = 0;
	int aid2 = -1;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	DWORD flags = 0;
	if( IsSuperZombie() )
	{
		switch(u_random(2)) {
			default:
			case 0:	aid = AddAnimation("Super_Zombie_Swing_Attack01"); break;
			case 1:	aid = AddAnimation("Super_Zombie_Swing_Attack02"); break;
		}
		flags = ANIMFLAG_RemoveOtherFade | ANIMFLAG_PauseOnEnd;
	}
	else if ( IsDogZombie() )
	{
		switch(u_random(4)) {
			default:
			case 0:	aid = AddAnimation("hunzi_attack_01"); break;
			case 1:	aid = AddAnimation("hunzi_attack_02"); break;
			case 2:	aid = AddAnimation("hunzi_attack_04"); break;
			case 3:	aid = AddAnimation("hunzi_attack_05"); break;
		}
		flags = ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped;
	}
	else
	{
		switch(u_random(2)) {
			default:
			case 0:	aid = AddAnimation("Zombie_Swing_Attack01"); break;
			case 1:	aid = AddAnimation("Zombie_Swing_Attack02"); break;
		}
		flags = ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped;
	}

	attackTrack = anim_.StartAnimation(aid, flags, 0.0f, 1.0f, 0.2f);

	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(SprintTrack);
	if( 0.0f < SprintTimer && ai && ( ai->dwStatus & ANIMSTATUS_Playing ) && 0.1f < GetVelocity().Length() )
	{
		if( IsSuperZombie() )
			aid2 = AddAnimation("Super_Zombie_Swing_Attack03");
		else if ( IsDogZombie() )
			aid2 = AddAnimation("hunzi_attack_04");
		else
			aid2 = AddAnimation("Zombie_Sprint_01_Hit");

		int tr2 = anim_.StartAnimation(aid2, 0, 0.0f, 1.0f, 0.1f);

		// start transition with first frame
		r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr2);
		ai->fCurFrame = 0;
	}

	attackCounter			= 0;
	superAttackDir			= 0;
	holdAttackTrack			= 0;
	ZombieAnimStateLevel	= 0;
	ZombieAnimState			= EZombieStates::ZState_Attack;
}

void obj_Zombie::StartTurnAnim( float finishAngle )
{
	int aid = 0;
	int tr = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	float timeToFinish = 1.0f;
	r3dVector fwd = GetFacingVector();
	r3dVector invFwd = r3dVector( 0, 0, -1 );
	invFwd.RotateAroundY( finishAngle );
	float turnAngle = r3dAcos( fwd.Dot( invFwd ) ); // in radians

	switch( ZombieAnimState )
	{
	case EZombieStates::ZState_Attack:
	case EZombieStates::ZState_BarricadeAttack:
	case EZombieStates::ZState_Idle:
	case EZombieStates::ZState_CallForHelp:
		{
			if( ZombieState == EZombieStates::ZState_TurnLeft )
			{
				if( IsSuperZombie() )
					aid = AddAnimation("Super_Zombie_Turn_L");
				else if ( IsDogZombie() )
					aid = AddAnimation("hunzi_turn_left");
				else
					aid = AddAnimation("Zombie_Turn_L");
				timeToFinish = fabs( 1.68f  * turnAngle/R3D_PI ); // Anim takes 1.68s to do a 180 degree turn.
			}
			else
			{
				if( IsSuperZombie() )
					aid = AddAnimation("Super_Zombie_Turn_R");
				else if ( IsDogZombie() )
					aid = AddAnimation("hunzi_turn_right");
				else
					aid = AddAnimation("Zombie_Turn_R");
				timeToFinish = fabs( 1.68f  * turnAngle/R3D_PI ); // Anim takes 1.68s to do a 180 degree turn.
			}
			break;
		}
	default:
		return;
	}

	tr = anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherFade | ANIMFLAG_Looped, 0.0f, 1.0f, 0.2f);

	// Start turn from the first frame.
	r3dAnimation::r3dAnimInfo* ai = anim_.GetTrack(tr);
	ai->fCurFrame = 0;

	// Perform Interpolation from current heading to final heading.
	turnInterp.Reset( fwd, invFwd, timeToFinish );
	
	ZombieAnimStateLevel = 0;
	ZombieAnimState = ZombieState;
}

int obj_Zombie::PlayCallForHelpAnim()
{
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	if( IsSuperZombie() )
	{
		aid = AddAnimation("Super_Zombie_Idle_02");
	}
	else if ( IsDogZombie() )
	{
		aid = AddAnimation("hunzi_ulos_idle");
	}
	else
	{
		if( m_isFemale )
			aid = AddAnimation("Zombie_Scream_2");
		else
			aid = AddAnimation("Zombie_Scream_1");
	}

	ZombieAnimStateLevel = 0;
	ZombieAnimState = EZombieStates::ZState_CallForHelp;
	return anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherNow | ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.2f);
}

int obj_Zombie::PlayDeadStandAnim()
{
	int aid = 0;
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	if( IsSuperZombie() )
	{
		switch(u_random(2))
		{
			default:
			case 0:	aid = AddAnimation("Super_Zombie_Dead_Stand_B_01"); break;
			case 1:	aid = AddAnimation("Super_Zombie_Dead_Stand_F_01"); break;
		}
	}
	else if ( IsDogZombie() )
	{
		switch(u_random(2))
		{
			default:
			case 0:	aid = AddAnimation("hunzi_idle_01"); break;
			case 1:	aid = AddAnimation("hunzi_idle_02"); break;
		}
	}
	else
	{
		switch(u_random(2))
		{
			default:
			case 0:	aid = AddAnimation("Zombie_Dead_Stand_B_01"); break;
			case 1:	aid = AddAnimation("Zombie_Dead_Stand_F_01"); break;
		}
	}
	return anim_.StartAnimation(aid, ANIMFLAG_RemoveOtherNow | ANIMFLAG_PauseOnEnd | ANIMSTATUS_Paused, 1.0f, 1.0f, 1.0f);
}

int obj_Zombie::PlayStaggeredAnim()
{
	m_isIdleCrouched = false;
	m_isIdleEating = false;

	if( IsSuperZombie() )
	{
		static const char* anims[] = {
			"Super_Zombie_Staggered_01_B",
			"Super_Zombie_Staggered_01_F",
			"Super_Zombie_Staggered_01_L",
			"Super_Zombie_Staggered_01_R",
			};

		const char* hitAnim = anims[u_random(4)];
		int aid = AddAnimation(hitAnim);
		r3dAnimData* ad = g_zombieAnimPool[ m_typeIndex ]->Get(aid);
		enableAnimBones( "Bip01_Spine1", g_zombieBindSkeleton[ 1 ], ad, true );
		staggeredTrack = anim_.StartAnimation(aid, ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
	}
	else if ( IsDogZombie() )
	{
		static const char* anims[] = {
			"hunzi_hit01",
			"hunzi_hit02",
			};

		const char* hitAnim = anims[u_random(2)];
		int aid = AddAnimation(hitAnim);
		r3dAnimData* ad = g_zombieAnimPool[ m_typeIndex ]->Get(aid);
		enableAnimBones( "Bip01_Spine1", g_zombieBindSkeleton[ 1 ], ad, true );
		staggeredTrack = anim_.StartAnimation(aid, ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
	}
	else
	{
		static const char* anims[] = {
			"Zombie_Staggered_01_B",
			"Zombie_Staggered_01_L",
			"Zombie_Staggered_01_R",
			"Zombie_Staggered_Small_01_B",
			"Zombie_Staggered_Small_01_F"
			};

		const char* hitAnim = anims[u_random(5)];
		int aid = AddAnimation(hitAnim);
		staggeredTrack = anim_.StartAnimation(aid, ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.1f);
		SetVelocity(r3dPoint3D(0, 0, 0));
	}

	return staggeredTrack;
}

int obj_Zombie::PlaySuperZombieAlertAnim(int alert)
{
	int tr = 0;
	if( IsSuperZombie() )
	{
		int aid = 0;
		m_isIdleCrouched = false;
		m_isIdleEating = false;

		switch( alert )
		{
		default:
		case EZombieStates::ZFidget_SZIdleAlert:
			aid = AddAnimation("Super_Zombie_Alert_1");
			break;
		case EZombieStates::ZFidget_SZIdleAlertSniff:
			aid = AddAnimation("Super_Zombie_Alert_2");
			break;
		};

		tr = anim_.StartAnimation(aid, 0, 0.0f, 1.0f, 0.2f);

		ZombieAnimStateLevel = 0;
		ZombieAnimState = EZombieStates::ZState_Idle;
	}

	return tr;
}

int obj_Zombie::PlaySuperZombieChestBeatAnim()
{
	int tr = 0;
	if( IsSuperZombie() )
	{
		int aid = 0;
		m_isIdleCrouched = false;
		m_isIdleEating = false;

		aid = AddAnimation("Super_Zombie_Idle_03");
		tr = anim_.StartAnimation(aid, 0, 0.0f, 1.0f, 0.2f);

		ZombieAnimStateLevel = 1;
		ZombieAnimState = EZombieStates::ZState_Idle;
	}

	return tr;
}

int obj_Zombie::PlaySuperZombieRunAttackAnim()
{
	int tr = 0;
	if( IsSuperZombie() )
	{
		int aid = 0;
		m_isIdleCrouched = false;
		m_isIdleEating = false;

		aid = AddAnimation("Super_Zombie_Run_03");
		tr = anim_.StartAnimation(aid, 0, 0.0f, 1.0f, 0.2f);
		ZombieAnimState = EZombieStates::ZState_MovingAttack;
	}

	return tr;
}

int obj_Zombie::PlaySuperZombieSuperAttackAnim()
{
	int tr = 0;
	if( IsSuperZombie() )
	{
		int aid = 0;
		m_isIdleCrouched = false;
		m_isIdleEating = false;

		superAttackDir++;
		superAttackDir %= 2;

		switch(superAttackDir)
		{
		default:
		case 0:	aid = AddAnimation("Super_Zombie_Super_Attack_V1"); break;
		case 1:	aid = AddAnimation("Super_Zombie_Super_Attack_V2"); break;
		}
		tr = anim_.StartAnimation(aid, ANIMFLAG_PauseOnEnd, 0.0f, 1.0f, 0.2f);
		ZombieAnimState = EZombieStates::ZState_Attack;
	}

	return tr;
}

r3dTL::TArray< obj_Zombie::ZombieSortEntry > obj_Zombie::ZombieList;
r3dTL::TArray< obj_Zombie::CacheEntry > obj_Zombie::PhysSkeletonCache;

void obj_Zombie::InitPhysSkeletonCache( float progressStart, float progressEnd )
{
	r3dOutToLog( "obj_Zombie::InitPhysSkeletonCache..\n" );

	const int PHYS_SKELETON_CACHE_SIZE = 256;

	if( PhysSkeletonCache.Count() < PHYS_SKELETON_CACHE_SIZE )
	{
		PhysSkeletonCache.Resize( PHYS_SKELETON_CACHE_SIZE );

		for( int i = 0; i < PHYS_SKELETON_CACHE_SIZE; i ++ )
		{
			void SetLoadingProgress( float progress );
			SetLoadingProgress( progressStart + ( progressEnd - progressStart ) * i / ( PHYS_SKELETON_CACHE_SIZE - 1 ) );

			//if( !( i & 0x07 ) )
			//	r3dOutToLog( "." );

			PhysSkeletonCache[ i ].skeleton = game_new r3dPhysSkeleton( "data/ObjectsDepot/Characters/RagDoll.RepX" );
			PhysSkeletonCache[ i ].skeletonDog = game_new r3dPhysDogSkeleton( "data/ObjectsDepot/WZ_Animals/ZDog.RepX" );
			PhysSkeletonCache[ i ].allocated = false;
		}
	}

	r3dOutToLog( "done\n" );
}

void obj_Zombie::FreePhysSkeletonCache()
{
	r3dOutToLog( "obj_Zombie::FreePhysSkeletonCache:" );

	float start = r3dGetTime();

	for( int i = 0, e = PhysSkeletonCache.Count(); i < e ; i ++ )
	{
		delete PhysSkeletonCache[ i ].skeleton;
		delete PhysSkeletonCache[ i ].skeletonDog;

		if( !(i & 0x07) )
			r3dOutToLog(".");
	}

	r3dOutToLog( "done in %.2f seconds\n", r3dGetTime() - start );

	PhysSkeletonCache.Clear();
}

static r3dTL::TArray< int > TempPhysSkelIndexArray;

void obj_Zombie::ReassignSkeletons()
{
	if( !ZombieList.Count() )
		return;

	R3DPROFILE_FUNCTION( "obj_Zombie::ReassignSkeletons" );

	TempPhysSkelIndexArray.Clear();

	struct sortfunc
	{
		bool operator()( const ZombieSortEntry& e0, const ZombieSortEntry& e1 )
		{
			return e0.distance < e1.distance;
		}
	};

	for( int i = 0, e = ZombieList.Count(); i < e; i ++ )
	{
		ZombieSortEntry& entry = ZombieList[ i ];
		entry.distance = ( entry.zombie->GetPosition() - gCam ).LengthSq();
	}

	std::sort( &ZombieList[ 0 ], &ZombieList[ 0 ] + ZombieList.Count(), sortfunc() );

	for( int i = PhysSkeletonCache.Count(), e = ZombieList.Count(); i < e; i ++ )
	{
		ZombieSortEntry& entry = ZombieList[ i ];

		if( entry.zombie->physSkeletonIndex >= 0  )
		{
			TempPhysSkelIndexArray.PushBack( entry.zombie->physSkeletonIndex );

			entry.zombie->UnlinkSkeleton();
		}
	}

	for( int i = 0, e = R3D_MIN( PhysSkeletonCache.Count(), ZombieList.Count() ); i < e; i ++ )
	{
		ZombieSortEntry& entry = ZombieList[ i ];

		if( entry.zombie->physSkeletonIndex < 0 && entry.zombie->PhysicsOn)
		{
			if( TempPhysSkelIndexArray.Count() )
			{
				entry.zombie->LinkSkeleton( TempPhysSkelIndexArray.GetLast() );
				TempPhysSkelIndexArray.Resize( TempPhysSkelIndexArray.Count() - 1 );
			}
			else
			{
				for( int i = 0, e = PhysSkeletonCache.Count(); i < e; i ++ )
				{
					if( !PhysSkeletonCache[ i ].allocated )
					{
						entry.zombie->LinkSkeleton( i );
						break;
					}
				}
			}
		}
	}
}

void obj_Zombie::LinkSkeleton( int cacheIndex )
{
	if ( IsDogZombie() )
	{
		r3dPhysDogSkeleton* skeleton = PhysSkeletonCache[ cacheIndex ].skeletonDog;
		PhysSkeletonCache[ cacheIndex ].allocated = true;

		physSkeletonIndex = cacheIndex;

		skeleton->linkParent(anim_.GetCurrentSkeleton(), GetTransformMatrix(), this, PHYSCOLL_NETWORKPLAYER) ;
	}
	else {
		r3dPhysSkeleton* skeleton = PhysSkeletonCache[ cacheIndex ].skeleton;
		PhysSkeletonCache[ cacheIndex ].allocated = true;

		physSkeletonIndex = cacheIndex;

		skeleton->linkParent(anim_.GetCurrentSkeleton(), GetTransformMatrix(), this, PHYSCOLL_NETWORKPLAYER) ;
	}

	SwitchPhysToRagdoll( isPhysInRagdoll ? true : false, NULL, 0xFF );
}

void obj_Zombie::UnlinkSkeleton()
{
	if ( IsDogZombie() )
	{
		if( physSkeletonIndex >= 0 )
		{
			PhysSkeletonCache[ physSkeletonIndex ].skeletonDog->unlink();
			PhysSkeletonCache[ physSkeletonIndex ].allocated = false;

			physSkeletonIndex = -1;
		}
	}
	else {
		if( physSkeletonIndex >= 0 )
		{
			PhysSkeletonCache[ physSkeletonIndex ].skeleton->unlink();
			PhysSkeletonCache[ physSkeletonIndex ].allocated = false;

			physSkeletonIndex = -1;
		}
	}
}

void obj_Zombie::SwitchPhysToRagdoll( bool ragdollOn, r3dPoint3D* hitForce, int BoneID, bool isVehicleHit )
{
	isPhysInRagdoll = ragdollOn;

	if( physSkeletonIndex >= 0 )
	{
		if ( IsDogZombie() )
		{
			r3dPhysDogSkeleton* physSkel = GetPhysSkeletonDog();

			if( ragdollOn )
			{
#ifdef VEHICLES_ENABLED
				if (isVehicleHit)
					physSkel->SwitchToRagdollWithImpulseForce(true, BoneID, hitForce);
				else
#endif
					physSkel->SwitchToRagdollWithForce( true, BoneID, hitForce );
			}
			else
			{
				if( physSkel->IsRagdollMode() )
				{
					physSkel->SwitchToRagdoll( false );
				}
			}
		}
		else
		{
			r3dPhysSkeleton* physSkel = GetPhysSkeleton();

			if( ragdollOn )
			{
#ifdef VEHICLES_ENABLED
				if (isVehicleHit)
					physSkel->SwitchToRagdollWithImpulseForce(true, BoneID, hitForce);
				else
#endif
					physSkel->SwitchToRagdollWithForce( true, BoneID, hitForce );
			}
			else
			{
				if( physSkel->IsRagdollMode() )
				{
					physSkel->SwitchToRagdoll( false );
				}
			}
		}

	}
}

void obj_Zombie::SetZombieState(int state, bool atStart)
{
	r3d_assert(ZombieState != state);

	int prevState = ZombieState;
	ZombieState = state;
	
#ifdef VEHICLES_ENABLED
	if (prevState == EZombieStates::ZState_FakeDead)
	{
		ZombieAnimState = ZombieState;
	}
#endif

	switch(ZombieState)
	{
		default:
			r3dError("unknown zombie state %d", ZombieState);
			return;
		
#ifdef VEHICLES_ENABLED
		case EZombieStates::ZState_FakeDead:
		{
			break;
		}
#endif
		case EZombieStates::ZState_Sleep:
		{
			PlayDeadStandAnim();
			break;
		}
		
		// wake up zombie, unpause animation from Sleep
		case EZombieStates::ZState_Waking:
			if(!atStart)
			{
				r3d_assert(anim_.AnimTracks.size() == 1);
				if(anim_.AnimTracks.size() > 0)
					anim_.AnimTracks[0].dwStatus &= ~ANIMSTATUS_Paused;
				break;
			}
			// note - waking zombies created at object creation phase will fallthru to idle phase.

		case EZombieStates::ZState_Idle:
		{
			// select new idle animation
			if( ObjFlags & OBJFLAG_JustCreated ||
				ZombieAnimState == EZombieStates::ZState_Attack ||
				ZombieAnimState == EZombieStates::ZState_BarricadeAttack ||
				ZombieAnimState == EZombieStates::ZState_MovingAttack ||
				ZombieAnimState == EZombieStates::ZState_CallForHelp )
				StartIdleAnim();
			CancelSprint();
			break;
		}

		case EZombieStates::ZState_TurnLeft:
		case EZombieStates::ZState_TurnRight:
		{
			break;
		}

		case EZombieStates::ZState_CallForHelp:
		{
			PlayCallForHelpAnim();
			PlayScreamSound();
			break;
		}

		case EZombieStates::ZState_Walk:
			break;
		
		case EZombieStates::ZState_Pursue:
		case EZombieStates::ZState_PursueSprint:
			break;

		case EZombieStates::ZState_Dead:
#ifdef VEHICLES_ENABLED
			if (ZombieState != EZombieStates::ZState_FakeDead)
#endif
				DoDeath();
			return;
			
		case EZombieStates::ZState_Attack:
		case EZombieStates::ZState_BarricadeAttack:
			StartAttackAnim();
			CancelSprint();
			SetVelocity(r3dPoint3D(0, 0, 0));
			break;

		case EZombieStates::ZState_MovingAttack:
			if( IsSuperZombie() )
			{
				GameObject* target = GameWorld().GetObject( targetId );
				if( target && target->isObjType( OBJTYPE_Barricade ) )
				{
					obj_Barricade* barricade = (obj_Barricade*)target;
					switch( barricade->m_ItemID )
					{
					default:
					case WeaponConfig::ITEMID_WoodShieldBarricade:
					case WeaponConfig::ITEMID_WoodShieldBarricadeZB:
						SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_Wood"), GetPosition() + GetFacingVector() * 2.0f, r3dPoint3D(0,1,0));
						break;
					case WeaponConfig::ITEMID_BarbWireBarricade:
						SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_BarbWire"), GetPosition() + GetFacingVector() * 2.0f, r3dPoint3D(0,1,0));
						break;
					case WeaponConfig::ITEMID_RiotShieldBarricade:
					case WeaponConfig::ITEMID_RiotShieldBarricadeZB:
						SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_Riot"), GetPosition() + GetFacingVector() * 2.0f, r3dPoint3D(0,1,0));
						break;
					case WeaponConfig::ITEMID_SandbagBarricade:
						SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_Sandbag"), GetPosition() + GetFacingVector() * 2.0f, r3dPoint3D(0,1,0));
						break;
					}

				}
				PlaySuperZombieRunAttackAnim();
				PlaySuperZombieRunAttackSound();
				PlaySuperZombieDestroyBarricadeSound();
			}
			else if ( IsDogZombie() )
			{
				StartAttackAnim();
				SetVelocity(r3dPoint3D(0, 0, 0));
				CancelSprint();
			}
			else
			{
				StartAttackAnim();
				SetVelocity(r3dPoint3D(0, 0, 0));
				CancelSprint();
			}
			break;
	}
}

void obj_Zombie::OnNetPacket(const PKT_S2C_MoveTeleport_s& n)
{
	// set player position and reset cell base, so it'll be updated on next movement
	SetPosition(n.teleport_pos);
	netMover.Teleport(n.teleport_pos);
}

void obj_Zombie::OnNetPacket(const PKT_C2C_MoveSetCell_s& n)
{
	netMover.SetCell(n);
}

void obj_Zombie::OnNetPacket(const PKT_C2C_MoveRel_s& n)
{
	const CNetCellMover::moveData_s& md = netMover.DecodeMove(n);
	
	int state = md.state & 0xFF;
	int avoidanceResult = md.state & 0xF00 >> 8;

	// might be unsafe, ignore movement while we staggered
	if(IsSuperZombie() || IsDogZombie() || !staggeredTrack)
	{
		if( EZombieStates::ZState_TurnLeft != ZombieState &&
			EZombieStates::ZState_TurnRight != ZombieState )
		{
			float timeToFinish = 0.1f;
			r3dVector fwd = GetFacingVector();
			r3dVector heading = r3dVector( 0, 0, -1 );
			heading.RotateAroundY( md.turnAngle );
			turnInterp.Reset( fwd, heading, timeToFinish );
		}
		else
		{
			SetRotationVector(r3dVector(md.turnAngle, 0, 0));
		}

		// we are ignoring md.state & md.bendAngle for now
	
		// calc velocity to reach position on time for next update
		r3dPoint3D vel = netMover.GetVelocityToNetTarget(
			GetPosition(),
			CreateParams.RunSpeed * 1.5f,
			1.0f);
		SetVelocity(vel);
	}
	
	if(state == 1 && !staggeredTrack)
	{
		PlayStaggeredAnim();
	}
	else if(state == 0 && staggeredTrack)
	{
		anim_.FadeOut(staggeredTrack, 0.1f);
		staggeredTrack = 0;
	}

	// Swap animations (but not ZombieState) based on velocity, to prevent slo-motion anims, or frozen anims.
	else if( ZombieState == EZombieStates::ZState_Idle || ZombieState == EZombieStates::ZState_Walk || ZombieState == EZombieStates::ZState_Pursue || ZombieState == EZombieStates::ZState_PursueSprint )
	{
		if( IsSuperZombie() )
			UpdateSuperZombieAnimForVelocity();
		else if ( IsDogZombie() )
			UpdateDogAnimForVelocity();
		else
			UpdateZombieAnimForVelocity();
	}

#ifndef FINAL_BUILD
	DebugData data( (EZombieStates::EGlobalStates)ZombieState, (EZombieStates::EGlobalStates)ZombieAnimState, netMover.lastMove.pos, GetVelocity() );
	m_debugHistory.Add( data );
#endif
}

void obj_Zombie::OnNetPacket(const PKT_S2C_ZombieSetState_s& n)
{
	SetZombieState(n.State, false);
}

void obj_Zombie::OnNetPacket(const PKT_S2C_ZombieSetTurnState_s& n)
{
	if( ZombieState != n.State )
	{
		SetZombieState(n.State, false);
		StartTurnAnim(n.TargetAngle);
	}
}

void obj_Zombie::OnNetPacket(const PKT_S2C_ZombieSprint_s& n)
{
	// Start Sprinting
	if( g_enable_zombie_sprint->GetBool() && 0.0f > SprintTimer )
	{
		SprintTimer = CreateParams.SprintMaxTime;
	}
}

void obj_Zombie::OnNetPacket(const PKT_S2C_ZombieTarget_s& n)
{
	GameObject* targetObj = GameWorld().GetNetworkObject(n.targetId);
	if( !targetObj )
	{
		targetId = invalidGameObjectID;
		return;
	}
	targetId = targetObj->GetSafeID();
}

void obj_Zombie::OnNetPacket(const PKT_S2C_ZombieCycleFidget_s& n)
{
	if( IsSuperZombie() )
	{
		switch( n.Fidget )
		{
		case EZombieStates::ZFidget_SZIdleChestBeat:
			PlaySuperZombieChestBeatAnim();
			PlaySuperZombieChestBeatSound();
			break;

		case EZombieStates::ZFidget_SZIdleAlert:
		case EZombieStates::ZFidget_SZIdleAlertSniff:
			PlaySuperZombieAlertAnim( n.Fidget );
			PlaySuperZombieAlertSound( n.Fidget );
			break;

		default:
			StartIdleAnim();
			break;
		}

		// Prepare for the next cycle.
		ZombieReqAnimTimers.ResetTimers();
		ZombieReqAnimTimers.ReqIdle();
	}
	else
	{
		switch( n.Fidget )
		{
		case EZombieStates::ZFidget_IdleCrouched:
			StartIdleCrouchedAnim();
			break;

		case EZombieStates::ZFidget_IdleEating:
			StartIdleEatCrouchedAnim();
			break;

		default:
			StartIdleAnim();
			break;
		}

		// Prepare for the next cycle.
		ZombieReqAnimTimers.ResetTimers();
		ZombieReqAnimTimers.ReqIdle();
	}
}

BOOL obj_Zombie::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	R3DPROFILE_FUNCTION("obj_Zombie::OnNetReceive");
	r3d_assert(!(ObjFlags & OBJFLAG_JustCreated)); // make sure that object was actually created before processing net commands

#undef DEFINE_GAMEOBJ_PACKET_HANDLER
#define DEFINE_GAMEOBJ_PACKET_HANDLER(xxx) \
	case xxx: { \
		const xxx##_s&n = *(xxx##_s*)packetData; \
		r3d_assert(packetSize == sizeof(n)); \
		OnNetPacket(n); \
		return TRUE; \
	}

	switch(EventID)
	{
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_MoveTeleport);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_MoveSetCell);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_MoveRel);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_ZombieSetState);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_ZombieSetTurnState);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_ZombieSprint);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_ZombieTarget);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_ZombieCycleFidget);
		
		case PKT_S2C_Zombie_DBG_AIInfo:
		{
			const PKT_S2C_Zombie_DBG_AIInfo_s&n = *(PKT_S2C_Zombie_DBG_AIInfo_s*)packetData;
			gotDebugMove = true;
			memcpy(&dbgAiInfo, &n, sizeof(n));
			return TRUE;
		}
	}

	return FALSE;
}


void obj_Zombie::DestroySounds()
{
	if(m_sndIdleHandle) { SoundSys.Release(m_sndIdleHandle); m_sndIdleHandle = NULL;}
	if(m_sndChaseHandle) { SoundSys.Release(m_sndChaseHandle); m_sndChaseHandle = NULL;}
	if(m_sndAttackHandle) { SoundSys.Release(m_sndAttackHandle); m_sndAttackHandle = NULL;}
	if(m_sndHurtHandle) { SoundSys.Release(m_sndHurtHandle); m_sndHurtHandle = NULL;}
	if(m_sndDeathHandle) { SoundSys.Release(m_sndDeathHandle); m_sndDeathHandle = NULL;}
	if(m_sndScreamHandle) { SoundSys.Release(m_sndScreamHandle); m_sndScreamHandle = NULL; }
}

#ifdef VEHICLES_ENABLED
void obj_Zombie::AttemptVehicleHit(PxF32 vehicleSpeed, obj_Vehicle* vehicle)
{
	if (vehicle->GetSpeed() < 7.0f || ZombieState == EZombieStates::ZState_FakeDead || IsSuperZombie())
		return;

	SetZombieState(EZombieStates::ZState_FakeDead, false);

	lastTimeHitBoneID = 2;
	lastTimeHitPos = (GetPosition() - vehicle->GetPosition()).NormalizeTo();

	if (vehicle->NetworkLocal)
	{
		PKT_C2S_VehicleHitTarget_s n;
		n.targetId = toP2pNetId(GetNetworkID());
		p2pSendToHost(vehicle, &n, sizeof(n));
	}

	PhysicsEnable(false);

	DoDeath();
}

void obj_Zombie::PhysicsEnable( const int& physicsEnabled )
{
	UpdateTransform();

	if(physicsEnabled != (int)m_bEnablePhysics)
	{
		m_bEnablePhysics = physicsEnabled?true:false;
		if (!m_bEnablePhysics)
		{
			SAFE_DELETE(PhysicsObject);
		}
		else
			CreatePhysicsData();
	}
}
#endif

void obj_Zombie::OnContactModify(PhysicsCallbackObject *obj, PxContactSet& contacts)
{
	// TODO - allow one hit and then ignore.
	if (!obj)
		return;

	GameObject* gameObj = obj->isGameObject();
	if (!gameObj)
		return;

#ifdef VEHICLES_ENABLED
	if (gameObj->isObjType(OBJTYPE_Vehicle) && ((obj_Vehicle*)gameObj)->CanSmashBarricade())
	{
		for(uint32_t i = 0; i < contacts.size(); ++i)
		{
			contacts.ignore(i);
		}
	}
#endif
}