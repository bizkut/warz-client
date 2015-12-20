#include "r3dpch.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Ammo.h"
#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "Bullet.h"
#include "..\world\DecalChief.h"
#include "..\world\MaterialTypes.h"
#include "ExplosionVisualController.h"

#include "..\AI\AI_Player.H"
#include "WeaponConfig.h"
#include "Weapon.h"

#include "multiplayer/P2PMessages.h"
#include "..\..\multiplayer\ClientGameLogic.h"

IMPLEMENT_CLASS(obj_Bullet, "obj_Bullet", "Object");
AUTOREGISTER_CLASS(obj_Bullet);


obj_Bullet::obj_Bullet()
{
	m_Ammo = NULL;
	m_Weapon = 0;
	m_ParticleTracer = NULL;
}

r3dMesh*  obj_Bullet::getModel()
{
	r3d_assert(m_Ammo);
	return m_Ammo->getModel();
}


r3dMesh* obj_Bullet::GetObjectMesh()
{
	r3d_assert(m_Ammo);
	return m_Ammo->getModel();
}

/*virtual*/
r3dMesh*
obj_Bullet::GetObjectLodMesh() /*OVERRIDE*/
{
	r3d_assert(m_Ammo);
	return m_Ammo->getModel();
}

BOOL obj_Bullet::OnCreate()
{
	const GameObject* owner = GameWorld().GetObject(ownerID);
	if(!owner)
		return FALSE;
		
	m_isSerializable = false;

	ReadPhysicsConfig();
	PhysicsConfig.group = PHYSCOLL_PROJECTILES;

	// perform our own movement to sync over network properly
	PhysicsConfig.isKinematic = true; 
	PhysicsConfig.isDynamic = true;

	r3d_assert(m_Ammo);
	r3d_assert(m_Weapon);
	parent::OnCreate();

	m_CreationTime = r3dGetTime();
	m_CreationPos = GetPosition();

	bool enableTracers = true;
#ifdef FINAL_BUILD
	enableTracers = (gClientLogic().m_gameInfo.flags & GBGameInfo::SFLAGS_Tracers)?true:false;
#else
	extern bool g_bEditMode;
	if(!g_bEditMode)
		enableTracers = (gClientLogic().m_gameInfo.flags & GBGameInfo::SFLAGS_Tracers)?true:false;
#endif
	if(m_Ammo->getParticleTracer() && enableTracers)
		m_ParticleTracer = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", m_Ammo->getParticleTracer(), GetPosition() );

	r3dBoundBox bboxLocal ;

	bboxLocal.Org.Assign( -0.5f, -0.25f, -0.25f );
	bboxLocal.Size.Assign(1.0f, 0.1f, 0.1f);

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

	m_AppliedVelocity = m_FireDirection*m_Weapon->m_AmmoSpeed;// * 2;

	m_DamageFromPiercable = 0;
	return TRUE;
}

BOOL obj_Bullet::OnDestroy()
{
	if(m_ParticleTracer)
		m_ParticleTracer->bKill = 1;

	return parent::OnDestroy();
}

void MatrixGetYawPitchRoll ( const D3DXMATRIX & , float &, float &, float & );

BOOL obj_Bullet::Update()
{
	parent::Update();
	const GameObject* owner = GameWorld().GetObject(ownerID);

	if( m_CreationTime+5.0f < r3dGetTime() ) // Bullets should only live for 5 seconds. 
	{
		if(owner && owner->NetworkLocal)
		{
			PKT_C2C_PlayerHitNothing_s n;
			n.localId = GetSafeID();
			p2pSendToHost(owner, &n, sizeof(n), true);
		}
		return false;
	}

	r3dVector dir = (GetPosition() - oldstate.Position);
	if(dir.Length()==0)
		dir = m_FireDirection;
	// perform collision check
	m_AppliedVelocity += r3dVector(0, -9.81f, 0) * m_Weapon->m_AmmoMass * r3dGetFrameTime();
	r3dPoint3D motion = m_AppliedVelocity * r3dGetFrameTime();
	float motionLen = motion.Length();
	motion.Normalize();

	PxU32 collisionFlag = COLLIDABLE_STATIC_MASK;
	if(owner && owner->NetworkLocal)
		collisionFlag |= (1<<PHYSCOLL_NETWORKPLAYER);

	PxSphereGeometry sphere(0.0025f); // Make it tiny. 5 milimeter diameter
	PxTransform pose(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z), PxQuat(0,0,0,1));

	PxSweepHit hit;
	PxSceneQueryFilterData filter(PxFilterData(collisionFlag, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
	
	
	if(g_pPhysicsWorld->PhysXScene->sweepSingle(sphere, pose, PxVec3(motion.x, motion.y, motion.z), motionLen, PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL, hit, filter))
	{

		// if the hit is the final stop. 
		if ( OnHit(hit) ) {
			return false;
		}
	}
	else 
	{
		// perform movement
		SetPosition(GetPosition() + m_AppliedVelocity * r3dGetFrameTime());
	}

	
	if(m_ParticleTracer)
		m_ParticleTracer->SetPosition(GetPosition());

	return true;
}

// return true if the bullet is still in flight. 
bool obj_Bullet::OnHit( PxSweepHit &hit )
{
	GameObject* owner = GameWorld().GetObject(ownerID);
	if(!owner || (owner->NetworkLocal == false ) )
		return true;

	obj_Player* ownerPlayer = (obj_Player*)owner;

	GameObject* shootTarget = NULL; 
	r3dMaterial* shootMaterial = NULL;
	PhysicsCallbackObject* target = NULL;
	const char * hitActorName = NULL;

	r3dVector hitPoint = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);
	r3dVector hitNormal = r3dPoint3D(hit.normal.x, hit.normal.y, hit.normal.z);
	if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
	{
		PxU32 faceIndex = hit.faceIndex;
		{
			PxTriangleMeshGeometry pxGeometry;
			if(hit.shape->getTriangleMeshGeometry(pxGeometry))
			{
				r3d_assert(pxGeometry.triangleMesh);
				const PxU32* remap = pxGeometry.triangleMesh->getTrianglesRemap();
				r3d_assert(remap);
				PxU32 numTriangles = pxGeometry.triangleMesh->getNbTriangles();
				r3d_assert(faceIndex < numTriangles);
				faceIndex = remap[faceIndex];
			}
		}

		hitActorName = hit.shape->getActor().getName(); 
		shootTarget= target->isGameObject();

		if( shootTarget)
		{
			if( shootTarget->isObjType( OBJTYPE_Mesh ) )
				shootMaterial = static_cast< MeshGameObject* > ( target )->MeshLOD[ 0 ]->GetFaceMaterial( faceIndex );
		}
		else if(target->hasMesh())
		{
			shootMaterial = target->hasMesh()->GetFaceMaterial( faceIndex );
		}
		if (!shootMaterial)
			shootMaterial = target->GetMaterial(faceIndex);

	}

	if ( ProcessBulletHit( m_DamageFromPiercable, ownerPlayer, hitPoint, hitNormal, shootTarget, shootMaterial, hitActorName,  m_Weapon, m_MuzzlerStartPos, GetSafeID() )  == false ) 
	{
		// the bullet is still live. 
		SetPosition( hitPoint + m_AppliedVelocity * 0.1f);
		return false;
	}
	return true;
}