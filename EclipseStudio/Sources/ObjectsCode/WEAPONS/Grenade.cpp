#include "r3dpch.h"
#include "r3d.h"

#include "Grenade.h"
#include "ObjectsCode\weapons\WeaponArmory.h"
#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "..\world\DecalChief.h"
#include "..\world\MaterialTypes.h"
#include "ExplosionVisualController.h"
#include "FlashbangVisualController.h"
#include "Gameplay_Params.h"

#include "..\..\multiplayer\ClientGameLogic.h"
#include "..\ai\AI_Player.H"
#include "WeaponConfig.h"
#include "Weapon.h"

#include "..\..\ui\HUDDisplay.h"
extern HUDDisplay*	hudMain;

IMPLEMENT_CLASS(obj_Grenade, "obj_Grenade", "Object");
AUTOREGISTER_CLASS(obj_Grenade);

obj_Grenade::obj_Grenade()
	: bHadCollided( false )
	, lastCollisionNormal( 0, 1, 0 )
	, m_ItemID( 0 )
	, m_bExplode( false )
	, m_NetMover(this, 1.0f / 10, (float)PKT_C2C_MoveSetCell_s::PLAYER_CELL_RADIUS)
	, m_bIgnoreNetMover( false )
{
	m_Ammo = NULL;
	m_Weapon = NULL;
	m_ParticleTracer = NULL;
}

r3dMesh* obj_Grenade::GetObjectMesh()
{
	r3d_assert(m_Ammo);
	return getModel();
}

/*virtual*/
r3dMesh*
obj_Grenade::GetObjectLodMesh()
{
	r3d_assert(m_Ammo);
	return getModel();
}

BOOL obj_Grenade::OnCreate()
{
	if(!m_Weapon)
		m_Weapon = g_pWeaponArmory->getWeaponConfig(m_ItemID);

	if(!m_Ammo)
		m_Ammo = m_Weapon->m_PrimaryAmmo;

	r3d_assert(m_Ammo);
	r3d_assert(m_Weapon);

	AcquireWeaponMesh();
	m_isSerializable = false;

	ReadPhysicsConfig();
	PhysicsConfig.group = PHYSCOLL_PROJECTILES;
	PhysicsConfig.isFastMoving = true;

	parent::OnCreate();

	m_CreationTime = r3dGetTime() - m_AddedDelay;
	m_CreationPos = GetPosition();

	if(m_Ammo->getParticleTracer())
		m_ParticleTracer = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", m_Ammo->getParticleTracer(), GetPosition() );

	SetBBoxLocal( GetObjectMesh()->localBBox ) ;
	UpdateTransform();

	if (WeaponConfig::ITEMID_FlareGun != m_ItemID)
		m_FireDirection.y += 0.1f; // to make grenade fly where you point
	m_AppliedVelocity = m_FireDirection*m_Weapon->m_AmmoSpeed;
	SetVelocity(m_AppliedVelocity);

//	r3dOutToLog("!!! Creating Grenade with Velocity: <%0.4f, %0.4f, %0.4f>\tLength: %0.4f\n", m_AppliedVelocity.x, m_AppliedVelocity.y, m_AppliedVelocity.z, m_AppliedVelocity.Length());
//	r3dOutToLog("!!! Creating Grenade with FireDirection: <%0.4f, %0.4f, %0.4f>\t AmmoSpeed: %0.4f\n", m_FireDirection.x, m_FireDirection.y, m_FireDirection.z, m_Weapon->m_AmmoSpeed);

	return TRUE;
}

BOOL obj_Grenade::OnDestroy()
{
	if(m_ParticleTracer)
		m_ParticleTracer->bKillDelayed = 1;

	return parent::OnDestroy();
}

void obj_Grenade::OnExplode()
{
	bool closeToGround = false;

	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
	if(g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y+1, GetPosition().z), PxVec3(0, -1, 0), 50.0f, PxSceneQueryFlag::eDISTANCE, hit, filter))
	{
		if(hit.distance < 5.0f)
			closeToGround = true;
	}

	if(closeToGround)
	{
		// add decal
		DecalParams params;
		params.Dir		= lastCollisionNormal;
		params.Pos		= GetPosition();
		params.TypeID	= GetDecalID( r3dHash::MakeHash(""), r3dHash::MakeHash(m_Weapon->m_PrimaryAmmo->getDecalSource()) );
		if( params.TypeID != INVALID_DECAL_ID )
			g_pDecalChief->Add( params );
	}

	//check for underwater splash
	r3dPoint3D waterSplashPos;
	extern bool TraceWater(const r3dPoint3D& start, const r3dPoint3D& finish, r3dPoint3D& waterSplashPos);
	if( TraceWater( GetPosition() - r3dPoint3D(0.0f, m_Weapon->m_AmmoArea, 0.0f), GetPosition() + r3dPoint3D(0.0f, m_Weapon->m_AmmoArea, 0.0f), waterSplashPos))
	{
		SpawnImpactParticle(r3dHash::MakeHash("Water"), r3dHash::MakeHash(m_Weapon->m_PrimaryAmmo->getDecalSource()), waterSplashPos, r3dPoint3D(0,1,0));
		extern void WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx);
		WaterSplash(GetPosition(), m_Weapon->m_AmmoArea, 10.0f/m_Weapon->m_AmmoArea, 0.04f, 1);
	}

	SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash(m_Weapon->m_PrimaryAmmo->getDecalSource()), GetPosition(), r3dPoint3D(0,1,0));

	//	Start radial blur effect
	gExplosionVisualController.AddExplosion(GetPosition(), m_Weapon->m_AmmoArea);
	// for now hard coded for flash bang grenade
	if(m_Weapon->m_itemID == 101137)
	{
		// only apply flashbang for local player
		if(gClientLogic().localPlayer_ && !gClientLogic().localPlayer_->bDead)
		{
			// check that player can actually see explosion (need better test, for when explosion is behind wall)
			bool explosionVisible = r3dRenderer->IsSphereInsideFrustum(GetPosition(), 2.0f)>0;
			if(explosionVisible)
			{
				float radiusAdd = 0.0f;
				float durationAdd = 0.0f;
				float dist = (GetPosition()-gClientLogic().localPlayer_->GetPosition()).Length();
				float str = 1.0f - R3D_CLAMP(dist / (GPP->c_fFlashBangRadius+radiusAdd), 0.0f, 1.0f);
				if (str > 0.01f)
				{
					FlashBangEffectParams fbep;
					fbep.duration = (GPP->c_fFlashBangDuration+durationAdd) * str;
					fbep.pos = GetPosition();
					gFlashbangVisualController.StartEffect(fbep);
				}
			}
		}
	}

#if APEX_ENABLED
	//	Apply apex destructions
	g_pApexWorld->ApplyAreaDamage(m_Weapon->m_AmmoDamage * 0.01f, 1, GetPosition(), m_Weapon->m_AmmoArea, true);
#endif
}

void obj_Grenade::OnCollide(PhysicsCallbackObject *tobj, CollisionInfo &trace)
{
	lastCollisionNormal = trace.Normal;
	bHadCollided = true;
}

void MatrixGetYawPitchRoll ( const D3DXMATRIX & , float &, float &, float & );
BOOL obj_Grenade::Update()
{
	parent::Update();

	if(!isActive())
		return FALSE;

	if((ObjFlags & OBJFLAG_JustCreated) > 0)
		return TRUE;

	if( m_bExplode )
	{
		return TRUE;
	}

	if( GetSafeNetworkID() > 0 )
		ProcessMovement();

	if(m_ParticleTracer)
		m_ParticleTracer->SetPosition(GetPosition());

	return TRUE;
}

void obj_Grenade::ProcessMovement()
{
	if( m_bIgnoreNetMover )
		return;

	//R3DPROFILE_FUNCTION("NetGrenade_Move");
	r3d_assert(!NetworkLocal);
	
	float timePassed = r3dGetFrameTime();
	
	if(0xFF != m_NetMover.lastMove.state &&
		0.0f < m_NetMover.lastRecv &&
		0.0001f < GetVelocity().LengthSq())
	{
		// Runs PhysX locally, and adjusts if the difference in position is too much. 
		r3dPoint3D pos = GetPosition();
		r3dPoint3D mdiff = m_NetMover.lastMove.pos - pos;
		if(mdiff.Length() > m_Weapon->m_AmmoSpeed * 1.5f)
		{
#ifndef FINAL_BUILD
			if( m_NetMover.lastMove.pos.Length() < 1.0f )
				r3dOutToLog("!!! Net: <%0.2f, %0.2f, %0.2f>\tCur: <%0.2f, %0.2f, %0.2f>\tDiff: <%0.2f, %0.2f, %0.2f>\tLen: %0.2f\n",
					m_NetMover.lastMove.pos.x, m_NetMover.lastMove.pos.y, m_NetMover.lastMove.pos.z,
					pos.x, pos.y, pos.z, mdiff.x, mdiff.y, mdiff.z, mdiff.Length());
			r3dOutToLog("!!!! %s teleported, too big distance %f\n", Name.c_str(), mdiff.Length());
#endif
			SetPosition(m_NetMover.lastMove.pos);
		}
	}
}

BOOL obj_Grenade::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	R3DPROFILE_FUNCTION("obj_Grenade::OnNetReceive");
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
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_MoveSetCell);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_C2C_MoveRel);
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_Explosion);
	}

	return FALSE;
}

void obj_Grenade::OnNetPacket(const PKT_C2C_MoveSetCell_s& n)
{
	m_NetMover.SetCell(n);
}

void obj_Grenade::OnNetPacket(const PKT_C2C_MoveRel_s& n)
{
	if( m_bIgnoreNetMover )
		return;

	const CNetCellMover::moveData_s& md = m_NetMover.DecodeMove(n);
	
	//SetRotationVector(r3dVector(md.turnAngle, 0, 0));

	r3d_assert(m_Weapon);
	r3dPoint3D pos = GetPosition();
	r3dPoint3D mdiff = md.pos - pos;
#ifndef FINAL_BUILD
	if (md.pos.Length() < 1.0f)
		r3dOutToLog("!!! Net: <%0.2f, %0.2f, %0.2f>\tCur: <%0.2f, %0.2f, %0.2f>\tDiff: <%0.2f, %0.2f, %0.2f>\tLen: %0.2f\tSpeed: %0.2f\n",
			md.pos.x, md.pos.y, md.pos.z,
			pos.x, pos.y, pos.z, mdiff.x, mdiff.y, mdiff.z, mdiff.Length(), m_Weapon->m_AmmoSpeed);
#endif
	if(mdiff.Length() > m_Weapon->m_AmmoSpeed * 0.25f)
	{
#ifndef FINAL_BUILD
		r3dOutToLog("!!!! %s teleported, too big distance %f\n", Name.c_str(), mdiff.Length());
#endif
		// calc velocity to reach position on time for next update
		r3dPoint3D vel = m_NetMover.GetVelocityToNetTarget(
			m_NetMover.lastMove.pos,
			m_Weapon->m_AmmoSpeed * 1.5f,
			1.0f);
		SetVelocity(vel);

		// Reset position.
		SetPosition(md.pos);
	}
}

void obj_Grenade::OnNetPacket(const PKT_S2C_Explosion_s& n)
{
	if( !m_bIgnoreNetMover )
	{
		SetPosition(n.explosion_pos);
		lastCollisionNormal = n.lastCollisionNormal;
	}

	m_bExplode = true;
	OnExplode();
}
