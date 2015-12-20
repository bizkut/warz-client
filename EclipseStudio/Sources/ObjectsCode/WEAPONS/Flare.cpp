#include "r3dpch.h"
#include "r3d.h"

#include "Flare.h"
#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "..\world\DecalChief.h"
#include "..\world\MaterialTypes.h"
#include "ExplosionVisualController.h"
#include "FlashbangVisualController.h"
#include "Gameplay_Params.h"

#include "multiplayer/P2PMessages.h"

#include "..\..\multiplayer\ClientGameLogic.h"
#include "..\ai\AI_Player.H"
#include "WeaponConfig.h"
#include "Weapon.h"

IMPLEMENT_CLASS(obj_Flare, "obj_Flare", "Object");
AUTOREGISTER_CLASS(obj_Flare);

obj_Flare::obj_Flare()
	: m_sndThrow( NULL )
	, m_sndBurn( NULL )
	, m_HangTime( 0.0f )
	, m_LastTimeUpdateCalled( 0.0f )
{
}

void obj_Flare::OnExplode()
{
	// Do Nothing!
}

BOOL obj_Flare::OnCreate()
{
	parent::OnCreate();
	PhysicsConfig.requireNoBounceMaterial = true;

	m_sndThrow = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_THROWFLARE"), GetPosition());
	m_sndBurn = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_FLARELOOP"), GetPosition());

	return TRUE;
}

BOOL obj_Flare::OnDestroy()
{
	if(m_sndThrow)
	{
		SoundSys.Release(m_sndThrow); 
		m_sndThrow = NULL;
	}
	if(m_sndBurn)
	{
		SoundSys.Release(m_sndBurn); 
		m_sndBurn = NULL;
	}

	return parent::OnDestroy();
}

void MatrixGetYawPitchRoll ( const D3DXMATRIX & , float &, float &, float & );
BOOL obj_Flare::Update()
{
	if(!parent::Update())
		return FALSE;

	if (WeaponConfig::ITEMID_FlareGun == m_ItemID && PhysicsObject)
	{
		const r3dPoint3D& vel = GetVelocity();
		if (vel.y < 0.0f)
		{
			m_bIgnoreNetMover = true;

			float curTime = r3dGetTime();
			float diffTime = curTime - m_LastTimeUpdateCalled;
			m_LastTimeUpdateCalled = curTime;
			m_HangTime += diffTime;

			//r3dOutToLog("!!! [%0.6f (+%0.6f)] - Grenade(%d) Vel: <%0.2f, %0.2f, %0.2f> Len:%0.6f\n", m_HangTime, diffTime, GetNetworkID(), vel.x, vel.y, vel.z, vel.Length());
			float coeff = (float)(0.9 - pow(M_E, double(-m_HangTime)));
			//r3dOutToLog("Coeff: %0.2f\n", coeff);
			r3dPoint3D imp = r3dPoint3D(-vel.x * coeff, -vel.y * coeff, -vel.z * coeff);
			PhysicsObject->addImpulse( imp );
			//r3dOutToLog("Impuse: <%0.2f, %0.2f, %0.2f>\n", imp.x, imp.y, imp.z);
		}
		else
		{
			m_LastTimeUpdateCalled = r3dGetTime();
		}
	}

	if(m_sndBurn)
	{
		r3dPoint3D pos = GetPosition();
		SoundSys.SetSoundPos(m_sndBurn, pos);
	}

	if(m_ParticleTracer)
	{
		D3DXMATRIX rot = GetRotationMatrix();
		r3dPoint3D offset = r3dPoint3D(rot._21, rot._22, rot._23);
		m_ParticleTracer->SetPosition(GetPosition() + offset*0.25f);
	}

	return TRUE;
}
