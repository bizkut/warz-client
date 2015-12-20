#pragma once

class obj_Player;

// physics skeleton
struct ActorBone
{
	PxRigidDynamic* actor;
	int		boneID;
	ActorBone():actor(0), boneID(-1) {}
	ActorBone(PxRigidDynamic* a, int b) : actor(a), boneID(b) {}
};

struct ActorJoint
{
	PxJoint* joint;
	ActorJoint():joint(0) {}
	ActorJoint(PxJoint* j) :joint(j) {}
};

class r3dPhysSkeleton
{
	friend struct RepXItemAdder;
	PxAggregate* m_Aggregate;
	ActorBone* m_Bones;
	int		m_NumBones;
	int		m_CurrentBone; // for loading
	ActorJoint* m_Joints;
	int		m_NumJoints;
	int		m_CurrentJoint; // for loading

	bool	m_isRagdollMode;
	//	Used for background loading completion indication
	volatile LONG isLoaded;

	bool loadSkeleton(const char* fname);
public:
	r3dPhysSkeleton( const char* fname );
	virtual ~r3dPhysSkeleton();

	void linkParent(const r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, GameObject *parent, PHYSICS_COLLISION_GROUPS collisionGroup);
	void unlink() ;

	void syncAnimation(r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, r3dAnimation &anim);
	void SwitchToRagdoll(bool toRagdoll);
	void SwitchToRagdollWithForce(bool toRagdoll, int boneId, const r3dPoint3D* force);
#ifdef VEHICLES_ENABLED
	void SwitchToRagdollWithImpulseForce(bool toRagdoll, int boneId, const r3dPoint3D* force);
#endif
	bool IsRagdollMode() const { return m_isRagdollMode; }
	r3dBoundBox getWorldBBox() const;
	void TogglePhysicsSimulation(bool on);
	void SetBonesActive( bool active );
};

class r3dPhysDogSkeleton
{
	friend struct RepXItemDogAdder;
	PxAggregate* m_Aggregate;
	ActorBone* m_Bones;

	int		m_CurrentBone; // for loading
	ActorJoint* m_Joints;
	int		m_NumJoints;
	int		m_CurrentJoint; // for loading

	bool	m_isRagdollMode;
	//	Used for background loading completion indication
	volatile LONG isLoaded;

	bool loadSkeleton(const char* fname);
public:
	int		m_NumBones;
	r3dPhysDogSkeleton( const char* fname );
	virtual ~r3dPhysDogSkeleton();

	void linkParent(const r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, GameObject *parent, PHYSICS_COLLISION_GROUPS collisionGroup);
	void unlink() ;

	void syncAnimation(r3dSkeleton *skel, const D3DXMATRIX &DrawFullMatrix, r3dAnimation &anim);
	void SwitchToRagdoll(bool toRagdoll);
	void SwitchToRagdollWithForce(bool toRagdoll, int boneId, const r3dPoint3D* force);
	bool IsRagdollMode() const { return m_isRagdollMode; }
	r3dBoundBox getWorldBBox() const;
	void TogglePhysicsSimulation(bool on);
	void SetBonesActive( bool active );
#ifdef VEHICLES_ENABLED
	void SwitchToRagdollWithImpulseForce(bool toRagdoll, int boneId, const r3dPoint3D* force);
#endif
};
