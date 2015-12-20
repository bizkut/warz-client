#include "r3dPCH.h"
#include "r3d.h"

#include "Ammo.h"

#include "GameCommon.h"
#include "multiplayer\P2PMessages.h"

#include "..\AI\AI_Player.H"

Ammo::Ammo(const char* ammoName)
{
	r3d_assert(ammoName);
	r3d_assert(strlen(ammoName)<32);
	r3dscpy(m_Name, ammoName);

	m_PrivateModel = NULL;
	m_ModelPath = NULL;
	m_ParticleTracer = 0;
	m_ShellExtractParticle = 0;
	m_DecalSource = NULL;
	m_BulletClass = 0;
}

Ammo::~Ammo()
{
	unloadModel();
	free(m_ModelPath);
	free(m_ParticleTracer);
	free(m_ShellExtractParticle);
	free(m_DecalSource);
	free(m_BulletClass);
}

r3dMesh* Ammo::getModel()
{
	if(m_PrivateModel == 0 && m_ModelPath)
	{
		m_PrivateModel = r3dGOBAddMesh(m_ModelPath, true, false, false, true );
		r3d_assert(m_PrivateModel);
	}
	return m_PrivateModel;
}

void Ammo::unloadModel()
{
	m_PrivateModel = NULL;
}

const char* Ammo::getName() const
{
	return m_Name ;
}

int Ammo::getModelRefCount()
{
	return m_PrivateModel ? m_PrivateModel->RefCount : 0 ;
}

void MatrixGetYawPitchRoll ( const D3DXMATRIX & mat, float & fYaw, float & fPitch, float & fRoll );
void NormalizeYPR ( r3dVector & ypr );
GameObject* Ammo::Fire(const r3dPoint3D& hitPos, const r3dPoint3D& muzzlerPos, const D3DMATRIX& weaponBone, GameObject* owner, const WeaponConfig* weapon, float added_delay)
{
	if(weapon->m_AmmoImmediate)
	{
		return NULL;
	}

	// not immediate damage. create gameobject
	if(!(r3d_float_isFinite(muzzlerPos.x) && r3d_float_isFinite(muzzlerPos.y) && r3d_float_isFinite(muzzlerPos.z))) // in case if animation is fucked up somewhere, hapens very rarely
		return NULL;

	r3d_assert(owner->isObjType(OBJTYPE_Human));
	const obj_Player* plr = (obj_Player*)owner;

	r3dPoint3D spawnPos = muzzlerPos;
	r3dPoint3D spawnRot = r3dPoint3D(0, 0, 0);
	
	bool isMine = strcmp(m_BulletClass, "obj_Mine")==0;
	if(isMine)
	{
		// cast ray down and find where we should place mine. should be in front of character, facing away from him
		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
		// muzzlePos is the correct position for the hands.
		if(g_pPhysicsWorld->raycastSingle(PxVec3(muzzlerPos.x, muzzlerPos.y+1, muzzlerPos.z), PxVec3(0, -1, 0), 50.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
		{
			spawnPos.x = hit.impact.x;
			spawnPos.y = hit.impact.y;
			spawnPos.z = hit.impact.z;
		}
			
		spawnRot.x = (plr->m_fPlayerRotation) + R3D_RAD2DEG(plr->bodyAdjust_x) + 180;
	}
	else
	{
		// set rotation vector equal to weapon bone, so that when you throw a grenade it will have same orientation as when in hand
		D3DMATRIX mat = weaponBone;
		mat.m[3][0] = 0.0f; mat.m[3][1] = 0.0f; mat.m[3][2] = 0.0f; mat.m[3][3] = 1.0f;
		r3dVector rot;
		MatrixGetYawPitchRoll ( mat, rot.x, rot.y, rot.z );
		NormalizeYPR ( rot );
		rot.x = R3D_RAD2DEG ( rot.x ); rot.y = R3D_RAD2DEG ( rot.y ); rot.z = R3D_RAD2DEG ( rot.z );
		
		if(!(r3d_float_isFinite(rot.x) && r3d_float_isFinite(rot.y) && r3d_float_isFinite(rot.z))) // in case if animation is fucked up somewhere, happens very rarely
			return NULL;
			
		spawnRot = rot;
	}

	AmmoShared* ammoSh = (AmmoShared*)srv_CreateGameObject(m_BulletClass, "bullet", spawnPos);
	ammoSh->m_Ammo = this;
	ammoSh->m_Weapon = weapon;
	ammoSh->ownerID = owner->GetSafeID();
	ammoSh->m_MuzzlerStartPos = muzzlerPos;
	ammoSh->m_FireDirection = (hitPos-muzzlerPos).NormalizeTo();
	ammoSh->m_AddedDelay = added_delay;
	ammoSh->SetRotationVector(spawnRot);
	return ammoSh;
}

struct AmmoSharedDeferredRenderable : MeshDeferredRenderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		AmmoSharedDeferredRenderable* This = static_cast< AmmoSharedDeferredRenderable* >( RThis );
		r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

		MeshDeferredRenderable::Draw( RThis, Cam );
	}

	AmmoShared* Parent;
};

//------------------------------------------------------------------------
/*virtual*/

BOOL
AmmoShared::OnCreate()	/*OVERRIDE*/
{
	AcquireWeaponMesh();
	return parent::OnCreate() ;
}

//------------------------------------------------------------------------
/*virtual*/

BOOL
AmmoShared::OnDestroy()	/*OVERRIDE*/
{
	ReleaseWeaponMesh();
	return parent::OnDestroy() ;
}

void AmmoShared::AcquireWeaponMesh()
{
	if(!weaponMeshAquired)
	{
		if( m_Weapon )
			m_Weapon->aquireMesh( true );
		weaponMeshAquired = true;
	}
}

void AmmoShared::ReleaseWeaponMesh()
{
	if(weaponMeshAquired)
	{
		if( m_Weapon )
			m_Weapon->releaseMesh();
		weaponMeshAquired = false;
	}
}

void AmmoShared::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	if(getModel())
	{
		uint32_t prevCount = render_arrays[ rsFillGBuffer ].Count();
		getModel()->AppendRenderablesDeferred( render_arrays[ rsFillGBuffer ], r3dColor::white, GetSelfIllumMultiplier() );
		for( uint32_t i = prevCount, e = render_arrays[ rsFillGBuffer ].Count(); i < e; i ++ )
		{
			AmmoSharedDeferredRenderable& rend = static_cast<AmmoSharedDeferredRenderable&>( render_arrays[ rsFillGBuffer ][ i ] );
			rend.Init();
			rend.Parent = this;
		}
	}
}
