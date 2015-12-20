#include "r3dPCH.h"
#include "r3d.h"


#include "GameCommon.h"

#include "r3dPhysSkeleton.h"

#include "extensions/PxRigidBodyExt.h"

#include "RepX\RepX.h"
#include "RepX\RepXUtility.h"
#include "extensions\PxStringTableExt.h"
#include "../../../../GameEngine/gameobjects/PhysXRepXHelpers.h"
#include "r3dBackgroundTaskDispatcher.h"

static class physx::repx::RepXCollection* m_sCollection = NULL;
static class PxStringTable* m_sStringTable = NULL;
static int m_sCollectionRef = 0;

static class physx::repx::RepXCollection* m_sDogCollection = NULL;
static class PxStringTable* m_sDogStringTable = NULL;
static int m_sDogCollectionRef = 0;


namespace
{
	bool gBonesSkipUpdateFlagsArr[0x100] = {0};
} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

struct RepXItemAdder
{
	PxScene* mScene;
	r3dPhysSkeleton* m_Skeleton;

	RepXItemAdder( PxScene* inScene, r3dPhysSkeleton* skel)
	: mScene( inScene ), m_Skeleton(skel)
	{
	}
	void operator()( const physx::repx::TRepXId inId, PxConvexMesh* ) {}
	void operator()( const physx::repx::TRepXId inId, PxTriangleMesh* ) {}
	void operator()( const physx::repx::TRepXId inId, PxHeightField* ) {}
	void operator()( const physx::repx::TRepXId inId, PxClothFabric* ) {}
	void operator()( const physx::repx::TRepXId inId, PxMaterial* ) {}
	void operator()( const physx::repx::TRepXId inId, PxParticleSystem* ) {}
	void operator()( const physx::repx::TRepXId inId, PxParticleFluid* ) {}
	void operator()( const physx::repx::TRepXId inId, PxAggregate* ) {}
	void operator()( const physx::repx::TRepXId inId, PxRigidStatic* inActor ) 
	{
		r3d_assert(false); // rag doll should be dynamic only, if you have static than you are trying to import static object
		//mScene->addActor( *inActor ); 
	}

	void operator()( const physx::repx::TRepXId inId, PxRigidDynamic* inActor ) 
	{ 
		if (!inActor)
			return;

		// pre notify
		PxShape* shapes[64] = {0};
		inActor->getShapes(&shapes[0], 64);

		D3DXQUATERNION test;
		D3DXQuaternionRotationYawPitchRoll(&test, 0, D3DX_PI / 2, 0); // rotX
		PxQuat rotQ(test.x, test.y, test.z, test.w);

		for (uint32_t i = 0; i < inActor->getNbShapes(); ++i)
		{
			PxTransform pose = shapes[i]->getLocalPose();
			pose.q = rotQ * pose.q;
			std::swap(pose.p.y, pose.p.z);

			shapes[i]->setLocalPose(pose);
			shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		}

		PxTransform actorMassPose = inActor->getCMassLocalPose();
		std::swap(actorMassPose.p.y, actorMassPose.p.z);
		inActor->setCMassLocalPose(actorMassPose);

		m_Skeleton->m_Bones[m_Skeleton->m_CurrentBone] = ActorBone(inActor, -1);
		m_Skeleton->m_CurrentBone++;

		// on add
		r3d_assert(m_Skeleton->m_CurrentBone<=m_Skeleton->m_NumBones);

		// not sure if that is too much mass, or if it should be set from Max, but otherwise PhysX outputs a lot of warnings
		//PxRigidBodyExt::setMassAndUpdateInertia(*inActor, 10.0f);

		inActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		inActor->setSleepThreshold(0.1f);

		//mScene->addActor(*inActor);
	}

	void operator()( const physx::repx::TRepXId inId, PxArticulation* inArticulation ) { }
	void operator()( const physx::repx::TRepXId inId, PxCloth* inData ) {}
	void operator()( const physx::repx::TRepXId inId, PxJoint* inJoint ) 
	{
		PxTransform p0 = inJoint->getLocalPose(PxJointActorIndex::eACTOR0);
		PxTransform p1 = inJoint->getLocalPose(PxJointActorIndex::eACTOR1);

		std::swap(p0.q.y, p0.q.z);
		std::swap(p1.q.y, p1.q.z);
		p0.q.y *= -1;
		p0.q.z *= -1;
		p0.q.x *= -1;
		p1.q.y *= -1;
		p1.q.z *= -1;
		p1.q.x *= -1;

		std::swap(p0.p.y, p0.p.z);
		std::swap(p1.p.y, p1.p.z);

		inJoint->setLocalPose(PxJointActorIndex::eACTOR0, p0);
		inJoint->setLocalPose(PxJointActorIndex::eACTOR1, p1);

		PxD6Joint *j = inJoint->is<PxD6Joint>();
		if (j)
		{
			PxJointLimitCone lc = j->getSwingLimit();
			std::swap(lc.yAngle, lc.zAngle);
			j->setSwingLimit(lc);

			PxD6Motion::Enum s1 = j->getMotion(PxD6Axis::eSWING1);
			PxD6Motion::Enum s2 = j->getMotion(PxD6Axis::eSWING2);
			j->setMotion(PxD6Axis::eSWING1, s2);
			j->setMotion(PxD6Axis::eSWING2, s1);
		}

		m_Skeleton->m_Joints[m_Skeleton->m_CurrentJoint] = ActorJoint(inJoint);
		m_Skeleton->m_CurrentJoint++;
		r3d_assert(m_Skeleton->m_CurrentJoint<=m_Skeleton->m_NumJoints);

		inJoint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
		inJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, false); // slows down PVD a lot
	}

};


//////////////////////////////////////////////////////

struct RepXItemDogAdder
{
	PxScene* mScene;
	r3dPhysDogSkeleton* m_Skeleton;

	RepXItemDogAdder( PxScene* inScene, r3dPhysDogSkeleton* skel)
	: mScene( inScene ), m_Skeleton(skel)
	{
	}
	void operator()( const physx::repx::TRepXId inId, PxConvexMesh* ) {}
	void operator()( const physx::repx::TRepXId inId, PxTriangleMesh* ) {}
	void operator()( const physx::repx::TRepXId inId, PxHeightField* ) {}
	void operator()( const physx::repx::TRepXId inId, PxClothFabric* ) {}
	void operator()( const physx::repx::TRepXId inId, PxMaterial* ) {}
	void operator()( const physx::repx::TRepXId inId, PxParticleSystem* ) {}
	void operator()( const physx::repx::TRepXId inId, PxParticleFluid* ) {}
	void operator()( const physx::repx::TRepXId inId, PxAggregate* ) {}
	void operator()( const physx::repx::TRepXId inId, PxRigidStatic* inActor ) 
	{
		r3d_assert(false); // rag doll should be dynamic only, if you have static than you are trying to import static object
		//mScene->addActor( *inActor ); 
	}

	void operator()( const physx::repx::TRepXId inId, PxRigidDynamic* inActor ) 
	{ 
		if (!inActor)
			return;

		// pre notify
		PxShape* shapes[64] = {0};
		inActor->getShapes(&shapes[0], 64);

		D3DXQUATERNION test;
		D3DXQuaternionRotationYawPitchRoll(&test, 0, D3DX_PI / 2, 0); // rotX
		PxQuat rotQ(test.x, test.y, test.z, test.w);

		for (uint32_t i = 0; i < inActor->getNbShapes(); ++i)
		{
			PxTransform pose = shapes[i]->getLocalPose();
			pose.q = rotQ * pose.q;
			std::swap(pose.p.y, pose.p.z);

			shapes[i]->setLocalPose(pose);
			shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		}

		PxTransform actorMassPose = inActor->getCMassLocalPose();
		std::swap(actorMassPose.p.y, actorMassPose.p.z);
		inActor->setCMassLocalPose(actorMassPose);

		m_Skeleton->m_Bones[m_Skeleton->m_CurrentBone] = ActorBone(inActor, -1);
		m_Skeleton->m_CurrentBone++;

		// on add
		r3d_assert(m_Skeleton->m_CurrentBone<=m_Skeleton->m_NumBones);

		// not sure if that is too much mass, or if it should be set from Max, but otherwise PhysX outputs a lot of warnings
		//PxRigidBodyExt::setMassAndUpdateInertia(*inActor, 10.0f);

		inActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		inActor->setSleepThreshold(0.1f);

		//mScene->addActor(*inActor);
	}

	void operator()( const physx::repx::TRepXId inId, PxArticulation* inArticulation ) { }
	void operator()( const physx::repx::TRepXId inId, PxCloth* inData ) {}
	void operator()( const physx::repx::TRepXId inId, PxJoint* inJoint ) 
	{
		PxTransform p0 = inJoint->getLocalPose(PxJointActorIndex::eACTOR0);
		PxTransform p1 = inJoint->getLocalPose(PxJointActorIndex::eACTOR1);

		std::swap(p0.q.y, p0.q.z);
		std::swap(p1.q.y, p1.q.z);
		p0.q.y *= -1;
		p0.q.z *= -1;
		p0.q.x *= -1;
		p1.q.y *= -1;
		p1.q.z *= -1;
		p1.q.x *= -1;

		std::swap(p0.p.y, p0.p.z);
		std::swap(p1.p.y, p1.p.z);

		inJoint->setLocalPose(PxJointActorIndex::eACTOR0, p0);
		inJoint->setLocalPose(PxJointActorIndex::eACTOR1, p1);

		PxD6Joint *j = inJoint->is<PxD6Joint>();
		if (j)
		{
			PxJointLimitCone lc = j->getSwingLimit();
			std::swap(lc.yAngle, lc.zAngle);
			j->setSwingLimit(lc);

			PxD6Motion::Enum s1 = j->getMotion(PxD6Axis::eSWING1);
			PxD6Motion::Enum s2 = j->getMotion(PxD6Axis::eSWING2);
			j->setMotion(PxD6Axis::eSWING1, s2);
			j->setMotion(PxD6Axis::eSWING2, s1);
		}

		m_Skeleton->m_Joints[m_Skeleton->m_CurrentJoint] = ActorJoint(inJoint);
		m_Skeleton->m_CurrentJoint++;
		r3d_assert(m_Skeleton->m_CurrentJoint<=m_Skeleton->m_NumJoints);

		inJoint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
		inJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, false); // slows down PVD a lot
	}

};

//////////////////////////////////////////////////////////////////////////

r3dPhysSkeleton::r3dPhysSkeleton( const char* fname )
: m_isRagdollMode(false)
, isLoaded(0)
{
	m_Bones = 0;
	m_NumBones = 0;
	m_CurrentBone = 0;
	m_Joints= 0;
	m_NumJoints= 0;
	m_CurrentJoint= 0;
	m_Aggregate = 0;

	loadSkeleton(fname);
}

r3dPhysSkeleton::~r3dPhysSkeleton()
{
	r3d_assert(m_sCollectionRef>0);
	--m_sCollectionRef;
	if(m_sCollectionRef == 0)
	{
		m_sCollection->destroy();
		m_sCollection = NULL;

		m_sStringTable->release();
		m_sStringTable = NULL;
	}

	for(int i=0; i<m_NumBones; ++i)
	{
		m_Bones[i].actor->userData = NULL;
		m_Bones[i].actor->release();
	}
	SAFE_DELETE_ARRAY(m_Bones);
	for(int i=0; i<m_NumJoints; ++i)
	{
		m_Joints[i].joint->release();
	}
	SAFE_DELETE_ARRAY(m_Joints);
	if(m_Aggregate)
	{
		m_Aggregate->release();
		m_Aggregate = NULL;
	}
}

bool r3dPhysSkeleton::loadSkeleton(const char* fname)
{
	R3DPROFILE_FUNCTION("r3dPhysSkeleton::loadSkeleton");

	r3dCSHolder block( g_pPhysicsWorld->GetConcurrencyGuard() ) ;

	if(m_sCollectionRef == 0)
	{
		m_sStringTable = &PxStringTableExt::createStringTable( g_pPhysicsWorld->GetAllocator() );
		m_sCollection = loadCollection( fname, g_pPhysicsWorld->PhysXFoundation->getAllocatorCallback() );
	}
	++m_sCollectionRef;

	int numActors = 0;
	int numJoints = 0;
	for(const physx::repx::RepXCollectionItem* iter=m_sCollection->begin(); iter!=m_sCollection->end(); ++iter)
	{
		if(strcmp(iter->mLiveObject.mTypeName, "PxRigidDynamic")==0)
			++numActors;
		if(strcmp(iter->mLiveObject.mTypeName, "PxD6Joint")==0)
			++numJoints;
	}

	m_CurrentBone = 0;
	m_NumBones = numActors;
	m_Bones = game_new ActorBone[m_NumBones];
	m_CurrentJoint= 0;
	m_NumJoints= numJoints;
	m_Joints= game_new ActorJoint[m_NumJoints];
	
	RepXItemAdder itemAdder(g_pPhysicsWorld->PhysXScene, this);

	//R3D_LOG_TIMESPAN_START(instantiateCollection);
	instantiateCollection( *m_sCollection, *g_pPhysicsWorld->PhysXSDK, *g_pPhysicsWorld->Cooking, m_sStringTable, itemAdder );
	//R3D_LOG_TIMESPAN_END(instantiateCollection);

	m_Aggregate = g_pPhysicsWorld->PhysXSDK->createAggregate(m_NumBones, false);
	for(int i=0; i<m_NumBones; ++i)
		m_Aggregate->addActor(*m_Bones[i].actor);

	//unlink();

	isLoaded = 1;

	return true;
}

//------------------------------------------------------------------------

void r3dPhysSkeleton::linkParent(const r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, GameObject *parent, PHYSICS_COLLISION_GROUPS collisionGroup)
{
	for( int i = 0, e = m_NumBones ; i < e; i ++ )
	{
		PxRigidDynamic* inActor = m_Bones[ i ].actor ;

		int boneID = skel->GetBoneID(inActor->getName());
		
		if(boneID==-1)
			r3dError("Ragdoll: cannot find bone '%s' in skeleton\n", inActor->getName());

		m_Bones[ i ].boneID = boneID ;

		D3DXMATRIX mat;
		D3DXMATRIX offset = DrawFullMatrix;

		skel->GetBoneWorldTM(boneID, &mat, offset);

		PxTransform actorPose = inActor->getGlobalPose();
		actorPose.p = PxVec3(mat._41, mat._42, mat._43);
		PxMat33 orientation(
							PxVec3(mat._11, mat._12, mat._13),
							PxVec3(mat._21, mat._22, mat._23),
							PxVec3(mat._31, mat._32, mat._33)
							);

		actorPose.q = PxQuat(orientation);
		inActor->setGlobalPose(actorPose);

		inActor->userData = parent; // for collision report

		PxShape* shapes[64] = {0};
		inActor->getShapes(&shapes[0], 64);

		for (uint32_t i = 0, e = inActor->getNbShapes() ; i < e ; ++ i )
		{
			PxFilterData filterData(collisionGroup, 0, 0, reinterpret_cast<PxU32>(parent));
			shapes[i]->setSimulationFilterData(filterData);
			PxFilterData qfilterData(1 << collisionGroup, 0, 0, 0);
			shapes[i]->setQueryFilterData(qfilterData);
			shapes[i]->userData = parent;
		}
	}

	/*for( int i = 0, e = m_NumBones ; i < e; i ++ )
	{
		m_Bones[ i ].actor->setSerialFlag(PxSerialFlag::eIN_SCENE, true) ;
	}*/
	g_pPhysicsWorld->PhysXScene->addAggregate(*m_Aggregate);
}

void r3dPhysSkeleton::unlink()
{
	for( int i = 0, e = m_NumBones ; i < e; i ++ )
	{
		PxRigidDynamic* actor = m_Bones[ i ].actor ;

		//actor->setSerialFlag(PxSerialFlag::eIN_SCENE, false) ;
		actor->userData = 0 ;

		PxShape* shapes[64] = {0};
		actor->getShapes(&shapes[0], 64);

		for (uint32_t i = 0, e = actor->getNbShapes() ; i < e ; ++ i )
		{
			shapes[i]->userData = 0;
		}
	}
	g_pPhysicsWorld->PhysXScene->removeAggregate(*m_Aggregate);
}

//////////////////////////////////////////////////////////////////////////

void r3dPhysSkeleton::syncAnimation(r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, r3dAnimation &anim)
{
	R3DPROFILE_FUNCTION("r3dPhysSkeleton::syncAnimation");

	if (!skel || !m_Bones || isLoaded == 0)
		return;

	r3dCSHolder block( g_pPhysicsWorld->GetConcurrencyGuard() ) ;

	if (!m_isRagdollMode)
	{
		for(int i=0; i<m_NumBones; ++i)
		{
			D3DXMATRIX mat;
			skel->GetBoneWorldTM(m_Bones[i].boneID, &mat, DrawFullMatrix);
			PxTransform actorPose;
			actorPose.p = PxVec3(mat._41, mat._42, mat._43);
			PxMat33 orientation(PxVec3(mat._11, mat._12, mat._13),
				PxVec3(mat._21, mat._22, mat._23),
				PxVec3(mat._31, mat._32, mat._33));
			actorPose.q = PxQuat(orientation);

			//m_Bones[i].actor->setKinematicTarget(actorPose); 
			m_Bones[i].actor->setGlobalPose(actorPose, false); // this should be faster than setKinematicActor (cuts from 10ms to 5-6ms physx update), but skeleton will not react to other meshes properly. 
																// might need to turn this on only for zombies. 
		}
	}
	else
	{
		D3DXMATRIX m(DrawFullMatrix);
		D3DXMatrixInverse(&m, 0, &m);

		r3d_assert(_countof(gBonesSkipUpdateFlagsArr) > skel->GetNumBones());
		::ZeroMemory(&gBonesSkipUpdateFlagsArr[0], sizeof(gBonesSkipUpdateFlagsArr));

		for (int i = 0; i < m_NumBones; ++i)
		{
			D3DXMATRIX res; D3DXMatrixIdentity(&res);
			ActorBone &b = m_Bones[i];
			PxTransform pose = b.actor->getGlobalPose();
			PxMat33 mat(pose.q);
			res._11 = mat.column0.x;
			res._12 = mat.column0.y;
			res._13 = mat.column0.z;

			res._21 = mat.column1.x;
			res._22 = mat.column1.y;
			res._23 = mat.column1.z;

			res._31 = mat.column2.x;
			res._32 = mat.column2.y;
			res._33 = mat.column2.z;

 			res._41 = pose.p.x;
 			res._42 = pose.p.y;
 			res._43 = pose.p.z;

			D3DXMatrixMultiply(&res, &res, &m);

			const char* nnn = b.actor->getName();

			int boneIdx = b.boneID;

			skel->SetBoneWorldTM(boneIdx, &res);
			gBonesSkipUpdateFlagsArr[boneIdx] = true;
		}
		D3DXMATRIX basePose;
		D3DXMatrixIdentity(&basePose);
		anim.CalcBasePose(basePose);
		skel->Recalc(&basePose, 0, gBonesSkipUpdateFlagsArr);
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dPhysSkeleton::SwitchToRagdoll(bool toRagdoll)
{
#if ENABLE_RAGDOLL
	if (isLoaded == 0)
		return;

	m_isRagdollMode = toRagdoll;
	const PxTransform identity(PxTransform::createIdentity());
	for (int i = 0; i < m_NumBones; ++i)
	{
		PxRigidDynamic *a = m_Bones[i].actor;
		if (!a) continue;

		if (m_isRagdollMode)
			a->setKinematicTarget(identity);

		a->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, !m_isRagdollMode);

		if(m_isRagdollMode)
		{
			PxVec3 zero(0, 0, 0);
			a->setAngularVelocity(zero);
 			a->setLinearVelocity(zero);
		}
	}
	for (int i = 0; i < m_NumJoints; ++i)
	{
		m_Joints[i].joint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, m_isRagdollMode);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

void r3dPhysSkeleton::SwitchToRagdollWithForce(bool toRagdoll, int boneId, const r3dPoint3D* force)
{
#if ENABLE_RAGDOLL
	if (isLoaded == 0)
		return;

	SwitchToRagdoll(toRagdoll);

	if (m_isRagdollMode && force)
	{
		if(boneId >= m_NumBones)
			boneId = random(m_NumBones-1);
		PxRigidDynamic *ab = m_Bones[boneId].actor;
		if(ab)
		{
			ab->addForce(PxVec3(force->x, force->y, force->z), PxForceMode::eVELOCITY_CHANGE);
		}
	}
#endif
}

#ifdef VEHICLES_ENABLED
void r3dPhysSkeleton::SwitchToRagdollWithImpulseForce(bool toRagdoll, int boneId, const r3dPoint3D* force)
{
#if ENABLE_RAGDOLL
	if (isLoaded == 0)
		return;

	SwitchToRagdoll(toRagdoll);

	if (m_isRagdollMode && force)
	{
		if(boneId >= m_NumBones)
			boneId = random(m_NumBones-1);
		PxRigidDynamic *ab = m_Bones[boneId].actor;
		if(ab)
		{
			ab->addForce(PxVec3(force->x, force->y, force->z), PxForceMode::eIMPULSE);
		}
	}
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////

void r3dPhysSkeleton::TogglePhysicsSimulation(bool on)
{
	for (int i = 0; i < m_NumBones; ++i)
	{
		PxRigidDynamic *a = m_Bones[i].actor;
		if (a)
		{
			on ? g_pPhysicsWorld->AddActor(*a) : g_pPhysicsWorld->RemoveActor(*a);
		}
	}
}

//------------------------------------------------------------------------

void r3dPhysSkeleton::SetBonesActive( bool active )
{
	r3dCSHolder block( g_pPhysicsWorld->GetConcurrencyGuard() ) ;

	for( int i = 0; i < m_NumBones; ++i )
	{
		PxRigidDynamic *a = m_Bones[i].actor;
		if ( a )
		{
			if( active )
			{
				//a->wakeUp(); // no need to wake them up, in case of physX will wake them up
			}
			else
			{
				a->putToSleep();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

r3dBoundBox r3dPhysSkeleton::getWorldBBox() const
{
	PxBounds3 bbox(PxBounds3::empty());

	if (isLoaded)
	{
		r3dCSHolder block( g_pPhysicsWorld->GetConcurrencyGuard() ) ;

		for (int i = 0; i < m_NumBones; ++i)
		{
			PxRigidDynamic *a = m_Bones[i].actor;
			if (!a) continue;

			PxShape* shapes[16] = {0};
			a->getShapes(&shapes[0], 16);
			PxU32 numShapes = a->getNbShapes();
			for (PxU32 j = 0; j < numShapes; ++j)
			{
				PxShape *s = shapes[j];
				PxBounds3 shapeBox = s->getWorldBounds();
				bbox.include(shapeBox);
			}
		}
	}
	r3dBoundBox rv;
	rv.Org = r3dPoint3D(bbox.minimum.x, bbox.minimum.y, bbox.minimum.z);
	rv.Size = r3dPoint3D(bbox.maximum.x - bbox.minimum.x, bbox.maximum.y - bbox.minimum.y, bbox.maximum.z - bbox.minimum.z);
	return rv;
}
///////////////////////////////////////

r3dPhysDogSkeleton::r3dPhysDogSkeleton( const char* fname )
: m_isRagdollMode(false)
, isLoaded(0)
{
	m_Bones = 0;
	m_NumBones = 0;
	m_CurrentBone = 0;
	m_Joints= 0;
	m_NumJoints= 0;
	m_CurrentJoint= 0;
	m_Aggregate = 0;

	loadSkeleton(fname);
}

r3dPhysDogSkeleton::~r3dPhysDogSkeleton()
{
	r3d_assert(m_sDogCollectionRef>0);
	--m_sDogCollectionRef;
	if(m_sDogCollectionRef == 0)
	{
		m_sDogCollection->destroy();
		m_sDogCollection = NULL;

		m_sDogStringTable->release();
		m_sDogStringTable = NULL;
	}

	for(int i=0; i<m_NumBones; ++i)
	{
		m_Bones[i].actor->userData = NULL;
		m_Bones[i].actor->release();
	}
	SAFE_DELETE_ARRAY(m_Bones);
	for(int i=0; i<m_NumJoints; ++i)
	{
		m_Joints[i].joint->release();
	}
	SAFE_DELETE_ARRAY(m_Joints);
	if(m_Aggregate)
	{
		m_Aggregate->release();
		m_Aggregate = NULL;
	}
}

bool r3dPhysDogSkeleton::loadSkeleton(const char* fname)
{
	R3DPROFILE_FUNCTION("r3dPhysSkeleton::loadSkeleton");
	r3dCSHolder block( g_pPhysicsWorld->GetConcurrencyGuard() ) ;

	if(m_sDogCollectionRef == 0)
	{
		m_sDogStringTable = &PxStringTableExt::createStringTable( g_pPhysicsWorld->GetAllocator() );
		m_sDogCollection = loadCollection( fname, g_pPhysicsWorld->PhysXFoundation->getAllocatorCallback() );
	}
	++m_sDogCollectionRef;

	int numActors = 0;
	int numJoints = 0;
	for(const physx::repx::RepXCollectionItem* iter=m_sDogCollection->begin(); iter!=m_sDogCollection->end(); ++iter)
	{
		if(strcmp(iter->mLiveObject.mTypeName, "PxRigidDynamic")==0)
			++numActors;
		if(strcmp(iter->mLiveObject.mTypeName, "PxD6Joint")==0)
			++numJoints;
	}
	m_CurrentBone = 0;
	m_NumBones = numActors;
	m_Bones = new ActorBone[m_NumBones];
	m_CurrentJoint= 0;
	m_NumJoints= numJoints;
	m_Joints= new ActorJoint[m_NumJoints];
	
	RepXItemDogAdder itemAdder(g_pPhysicsWorld->PhysXScene, this);

	//R3D_LOG_TIMESPAN_START(instantiateCollection);
	instantiateCollection( *m_sDogCollection, *g_pPhysicsWorld->PhysXSDK, *g_pPhysicsWorld->Cooking, m_sDogStringTable, itemAdder );
	//R3D_LOG_TIMESPAN_END(instantiateCollection);

	m_Aggregate = g_pPhysicsWorld->PhysXSDK->createAggregate(m_NumBones, false);
	for(int i=0; i<m_NumBones; ++i)
		m_Aggregate->addActor(*m_Bones[i].actor);

	//unlink();

	isLoaded = 1;

	return true;
}

//------------------------------------------------------------------------

void r3dPhysDogSkeleton::linkParent(const r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, GameObject *parent, PHYSICS_COLLISION_GROUPS collisionGroup)
{

	for( int i = 0, e = m_NumBones ; i < e; i ++ )
	{

		PxRigidDynamic* inActor = m_Bones[ i ].actor ;
		int boneID = skel->GetBoneID(inActor->getName());
		
		if(boneID==-1)
			r3dError("Ragdoll: cannot find bone '%s' in skeleton\n", inActor->getName());

		m_Bones[ i ].boneID = boneID ;

		D3DXMATRIX mat;
		D3DXMATRIX offset = DrawFullMatrix;

		skel->GetBoneWorldTM(boneID, &mat, offset);

		PxTransform actorPose = inActor->getGlobalPose();
		actorPose.p = PxVec3(mat._41, mat._42, mat._43);
		PxMat33 orientation(
							PxVec3(mat._11, mat._12, mat._13),
							PxVec3(mat._21, mat._22, mat._23),
							PxVec3(mat._31, mat._32, mat._33)
							);

		actorPose.q = PxQuat(orientation);
		inActor->setGlobalPose(actorPose);

		inActor->userData = parent; // for collision report

		PxShape* shapes[64] = {0};
		inActor->getShapes(&shapes[0], 64);

		for (uint32_t i = 0, e = inActor->getNbShapes() ; i < e ; ++ i )
		{
			PxFilterData filterData(collisionGroup, 0, 0, reinterpret_cast<PxU32>(parent));
			shapes[i]->setSimulationFilterData(filterData);
			PxFilterData qfilterData(1 << collisionGroup, 0, 0, 0);
			shapes[i]->setQueryFilterData(qfilterData);
			shapes[i]->userData = parent;
		}
	}

	/*for( int i = 0, e = m_NumBones ; i < e; i ++ )
	{
		m_Bones[ i ].actor->setSerialFlag(PxSerialFlag::eIN_SCENE, true) ;
	}*/
	g_pPhysicsWorld->PhysXScene->addAggregate(*m_Aggregate);
}

void r3dPhysDogSkeleton::unlink()
{
	for( int i = 0, e = m_NumBones ; i < e; i ++ )
	{
		PxRigidDynamic* actor = m_Bones[ i ].actor ;

		//actor->setSerialFlag(PxSerialFlag::eIN_SCENE, false) ;
		actor->userData = 0 ;

		PxShape* shapes[64] = {0};
		actor->getShapes(&shapes[0], 64);

		for (uint32_t i = 0, e = actor->getNbShapes() ; i < e ; ++ i )
		{
			shapes[i]->userData = 0;
		}
	}
	g_pPhysicsWorld->PhysXScene->removeAggregate(*m_Aggregate);
}

//////////////////////////////////////////////////////////////////////////

void r3dPhysDogSkeleton::syncAnimation(r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, r3dAnimation &anim)
{
	R3DPROFILE_FUNCTION("r3dPhysSkeleton::syncAnimation");

	if (!skel || !m_Bones || isLoaded == 0)
		return;

	r3dCSHolder block( g_pPhysicsWorld->GetConcurrencyGuard() ) ;

	if (!m_isRagdollMode)
	{
		for(int i=0; i<m_NumBones; ++i)
		{
			D3DXMATRIX mat;
			skel->GetBoneWorldTM(m_Bones[i].boneID, &mat, DrawFullMatrix);
			PxTransform actorPose;
			actorPose.p = PxVec3(mat._41, mat._42, mat._43);
			PxMat33 orientation(PxVec3(mat._11, mat._12, mat._13),
				PxVec3(mat._21, mat._22, mat._23),
				PxVec3(mat._31, mat._32, mat._33));
			actorPose.q = PxQuat(orientation);

			//m_Bones[i].actor->setKinematicTarget(actorPose); 
			m_Bones[i].actor->setGlobalPose(actorPose, false); // this should be faster than setKinematicActor (cuts from 10ms to 5-6ms physx update), but skeleton will not react to other meshes properly. 
																// might need to turn this on only for zombies. 
		}
	}
	else
	{
		D3DXMATRIX m(DrawFullMatrix);
		D3DXMatrixInverse(&m, 0, &m);

		r3d_assert(_countof(gBonesSkipUpdateFlagsArr) > skel->GetNumBones());
		::ZeroMemory(&gBonesSkipUpdateFlagsArr[0], sizeof(gBonesSkipUpdateFlagsArr));

		for (int i = 0; i < m_NumBones; ++i)
		{
			D3DXMATRIX res; D3DXMatrixIdentity(&res);
			ActorBone &b = m_Bones[i];
			PxTransform pose = b.actor->getGlobalPose();
			PxMat33 mat(pose.q);
			res._11 = mat.column0.x;
			res._12 = mat.column0.y;
			res._13 = mat.column0.z;

			res._21 = mat.column1.x;
			res._22 = mat.column1.y;
			res._23 = mat.column1.z;

			res._31 = mat.column2.x;
			res._32 = mat.column2.y;
			res._33 = mat.column2.z;

 			res._41 = pose.p.x;
 			res._42 = pose.p.y;
 			res._43 = pose.p.z;

			D3DXMatrixMultiply(&res, &res, &m);

			const char* nnn = b.actor->getName();

			int boneIdx = b.boneID;

			skel->SetBoneWorldTM(boneIdx, &res);
			gBonesSkipUpdateFlagsArr[boneIdx] = true;
		}
		D3DXMATRIX basePose;
		D3DXMatrixIdentity(&basePose);
		anim.CalcBasePose(basePose);
		skel->Recalc(&basePose, 0, gBonesSkipUpdateFlagsArr);
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dPhysDogSkeleton::SwitchToRagdoll(bool toRagdoll)
{
#if ENABLE_RAGDOLL
	if (isLoaded == 0)
		return;

	m_isRagdollMode = toRagdoll;
	const PxTransform identity(PxTransform::createIdentity());
	for (int i = 0; i < m_NumBones; ++i)
	{
		PxRigidDynamic *a = m_Bones[i].actor;
		if (!a) continue;

		if (m_isRagdollMode)
			a->setKinematicTarget(identity);

		a->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, !m_isRagdollMode);

		if(m_isRagdollMode)
		{
			PxVec3 zero(0, 0, 0);
			a->setAngularVelocity(zero);
 			a->setLinearVelocity(zero);
		}
	}
	for (int i = 0; i < m_NumJoints; ++i)
	{
		m_Joints[i].joint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, m_isRagdollMode);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

void r3dPhysDogSkeleton::SwitchToRagdollWithForce(bool toRagdoll, int boneId, const r3dPoint3D* force)
{
#if ENABLE_RAGDOLL
	if (isLoaded == 0)
		return;

	SwitchToRagdoll(toRagdoll);

	if (m_isRagdollMode && force)
	{
		if(boneId >= m_NumBones)
			boneId = random(m_NumBones-1);
		PxRigidDynamic *ab = m_Bones[boneId].actor;
		if(ab)
		{
			ab->addForce(PxVec3(force->x, force->y, force->z), PxForceMode::eVELOCITY_CHANGE);
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

void r3dPhysDogSkeleton::TogglePhysicsSimulation(bool on)
{
	for (int i = 0; i < m_NumBones; ++i)
	{
		PxRigidDynamic *a = m_Bones[i].actor;
		if (a)
		{
			on ? g_pPhysicsWorld->AddActor(*a) : g_pPhysicsWorld->RemoveActor(*a);
		}
	}
}

//------------------------------------------------------------------------

void r3dPhysDogSkeleton::SetBonesActive( bool active )
{
	for( int i = 0; i < m_NumBones; ++i )
	{
		PxRigidDynamic *a = m_Bones[i].actor;
		if ( a )
		{
			if( active )
			{
				//a->wakeUp(); // no need to wake them up, in case of physX will wake them up
			}
			else
			{
				a->putToSleep();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

r3dBoundBox r3dPhysDogSkeleton::getWorldBBox() const
{
	PxBounds3 bbox(PxBounds3::empty());

	if (isLoaded)
	{
		for (int i = 0; i < m_NumBones; ++i)
		{
			PxRigidDynamic *a = m_Bones[i].actor;
			if (!a) continue;

			PxShape* shapes[16] = {0};
			a->getShapes(&shapes[0], 16);
			PxU32 numShapes = a->getNbShapes();
			for (PxU32 j = 0; j < numShapes; ++j)
			{
				PxShape *s = shapes[j];
				PxBounds3 shapeBox = s->getWorldBounds();
				bbox.include(shapeBox);
			}
		}
	}
	r3dBoundBox rv;
	rv.Org = r3dPoint3D(bbox.minimum.x, bbox.minimum.y, bbox.minimum.z);
	rv.Size = r3dPoint3D(bbox.maximum.x - bbox.minimum.x, bbox.maximum.y - bbox.minimum.y, bbox.maximum.z - bbox.minimum.z);
	return rv;
}

#ifdef VEHICLES_ENABLED
void r3dPhysDogSkeleton::SwitchToRagdollWithImpulseForce(bool toRagdoll, int boneId, const r3dPoint3D* force)
{
#if ENABLE_RAGDOLL
	if (isLoaded == 0)
		return;

	SwitchToRagdoll(toRagdoll);

	if (m_isRagdollMode && force)
	{
		if(boneId >= m_NumBones)
			boneId = random(m_NumBones-1);
		PxRigidDynamic *ab = m_Bones[boneId].actor;
		if(ab)
		{
			ab->addForce(PxVec3(force->x, force->y, force->z), PxForceMode::eIMPULSE);
		}
	}
#endif
}
#endif