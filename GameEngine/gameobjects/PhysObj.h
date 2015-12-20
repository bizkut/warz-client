#ifndef __PHYS_OBJ_H__
#define __PHYS_OBJ_H__

class GameObject;
class PhysicsCallbackObject;
class r3dMesh;

enum PHYSICS_COLLISION_GROUPS
{
	PHYSCOLL_COLLISION_GEOMETRY = 1,
	PHYSCOLL_DETAIL_GEOMETRY = 2, // not used yet
	PHYSCOLL_STATIC_GEOMETRY = 3, // that's basically COLLISION_GEOMETRY|DETAIL_GEOMETRY
	PHYSCOLL_TINY_GEOMETRY = 4, // special group that only collides with world, but players, ray tracing ignores it
	PHYSCOLL_TRIGGER	   = 5, // special group for triggers
	PHYSCOLL_PLAYER_ONLY_GEOMETRY = 6, // special group for invisible objects that have collision for player only
	PHYSCOLL_NON_PLAYER_GEOMETRY = 7, // special type of PHYSCOLL_COLLISION_GEOMETRY that player shouldn't collide with, but everything else will collide
	PHYSCOLL_PROJECTILES = 8, // group for rockets, grenades, etc.
#ifdef VEHICLES_ENABLED
	PHYSCOLL_VEHICLE_WHEEL = 9, // group for vehicle wheels
	PHYSCOLL_VEHICLE = 10, // vehicle
#endif

	PHYSCOLL_NETWORKPLAYER = 16,
	PHYSCOLL_LOCALPLAYER = 17,

	PHYSCOLL_CHARACTERCONTROLLER = 31,
};

enum PHYSICS_COLLISION_GROUPS2
{
	PHYSCOLL2_FAST_MOVING_OBJECT = 1,	// enable CCT for those objects
};

#define COLLIDABLE_STATIC_MASK ((1<<PHYSCOLL_COLLISION_GEOMETRY)|(1<<PHYSCOLL_DETAIL_GEOMETRY)|(1<<PHYSCOLL_STATIC_GEOMETRY)|(1<<PHYSCOLL_NON_PLAYER_GEOMETRY))
#ifndef VEHICLES_ENABLED
#define COLLIDABLE_PLAYER_COLLIDABLE_MASK ((1<<PHYSCOLL_COLLISION_GEOMETRY)|(1<<PHYSCOLL_DETAIL_GEOMETRY)|(1<<PHYSCOLL_STATIC_GEOMETRY)|(1<<PHYSCOLL_PLAYER_ONLY_GEOMETRY)) // everything that player should collide with
#else
#define COLLIDABLE_PLAYER_COLLIDABLE_MASK ((1<<PHYSCOLL_COLLISION_GEOMETRY)|(1<<PHYSCOLL_DETAIL_GEOMETRY)|(1<<PHYSCOLL_STATIC_GEOMETRY)|(1<<PHYSCOLL_PLAYER_ONLY_GEOMETRY)|(1<<PHYSCOLL_VEHICLE)) // everything that player should collide with
#define COLLIDABLE_PLAYER_COLLIDABLE_MASK_NOVEH ((1<<PHYSCOLL_COLLISION_GEOMETRY)|(1<<PHYSCOLL_DETAIL_GEOMETRY)|(1<<PHYSCOLL_STATIC_GEOMETRY)|(1<<PHYSCOLL_PLAYER_ONLY_GEOMETRY)) // everything that player should collide with except vehicle
#endif

enum PHYSICS_TYPE
{
	PHYSICS_TYPE_UNKNOWN = 0,
	PHYSICS_TYPE_BOX,
	PHYSICS_TYPE_SPHERE,
	PHYSICS_TYPE_MESH,
	PHYSICS_TYPE_CONVEX,

	// make sure those are last for now:
	PHYSICS_TYPE_RAYCAST_BOX,
	PHYSICS_TYPE_CONTROLLER, // special type for player controller
	PHYSICS_TYPE_CONTROLLER_ZOMBIE,
};

struct PhysicsObjectConfig
{
	PHYSICS_COLLISION_GROUPS group;
	PHYSICS_TYPE	type;
	float			mass; //0..inf
	float			contactOffset;
	float			controllerRadius;
	float			controllerHeight;
	r3dPoint3D		offset;
	bool			isDynamic;
	bool			isKinematic;
	bool			isTrigger;
	bool			isFastMoving;

#ifdef VEHICLES_ENABLED
	bool			isVehicle;
#endif

	bool			needBoxCollision;
	bool			needExplicitCollisionMesh; // for static meshes only for now
	bool			canModifyContacts;
	bool			controllerHitCallback;
	bool			controllerBehaviorCallback;
	bool			ready;

	bool			requireNoBounceMaterial; // for objects that shouldn't slide or move to much after impact (like dropped objects from server)

	bool			isDestroyed;

	char*			meshFilename;
	char*			destroyedMeshFilename;

	PhysicsObjectConfig() // set to default, if you try to create object with those params it will fail!
	{
		group = PHYSCOLL_COLLISION_GEOMETRY;
		type = PHYSICS_TYPE_UNKNOWN;
		mass = 1.0f;
		contactOffset = 0.1f;
		controllerRadius = 0.3f;
		controllerHeight = 1.4f;
		offset = r3dPoint3D( 0, 0, 0 );
		isFastMoving = 0;
		isDynamic = 0;
		isKinematic = 0;
		isTrigger = 0;
#ifdef VEHICLES_ENABLED
		isVehicle = 0;
#endif

		needBoxCollision = 0;
		needExplicitCollisionMesh = 0;
		canModifyContacts = 0;
		ready = 0; // set to true once config is ready
		requireNoBounceMaterial = false;
		meshFilename = NULL;
		destroyedMeshFilename = NULL;

		isDestroyed = false;
		controllerHitCallback = false;
		controllerBehaviorCallback = false;
	}

	~PhysicsObjectConfig()
	{
		free(meshFilename);
	}

	// copy constructor due to meshFilename
	PhysicsObjectConfig(const PhysicsObjectConfig& rhs)
	{
		memcpy(this, &rhs, sizeof(PhysicsObjectConfig));

		meshFilename = NULL;
		if(rhs.meshFilename)
			meshFilename = strdup(rhs.meshFilename);

		destroyedMeshFilename = NULL;
		if( rhs.destroyedMeshFilename)
			destroyedMeshFilename = strdup(rhs.destroyedMeshFilename);
	}
	PhysicsObjectConfig& operator=(const PhysicsObjectConfig& rhs)
	{
		free(this->meshFilename);
		memcpy(this, &rhs, sizeof(PhysicsObjectConfig));
		meshFilename = NULL;
		if(rhs.meshFilename)
			meshFilename = strdup(rhs.meshFilename);

		SetDestroyedMesh( rhs.destroyedMeshFilename );
	}

	void SetDestroyedMesh( const char* newDestroyedMeshFilename );
	const char* GetMeshName() const;

};

// used as interface
class BasePhysicsObject
{
public:
	virtual ~BasePhysicsObject() {}
	virtual void			Move(const r3dPoint3D& move, float sharpness=0.5f)=0; // move object, used for controller
	virtual void 			SetPosition(const r3dPoint3D& pos)=0;
	virtual void 			SetRotation(const r3dVector& Angles)=0;
	virtual void 			SetVelocity(const r3dPoint3D& vel)=0;
	virtual void			SetScale(const r3dPoint3D& scale)=0;
	virtual r3dPoint3D 		GetPosition() const =0;
	virtual D3DXMATRIX 		GetRotation() const =0;
	virtual r3dPoint3D 		GetVelocity() const =0;
	virtual r3dPoint3D		GetScale() const = 0;

	// physics object position is in a center of object. 
	// in engine, center of object is in pivot.
	// so this diff is to make sure that position in physics and engine is in sync
	virtual void			SetPositionCorrection(const r3dPoint3D& diff)=0;

	virtual bool			IsSleeping() = 0;	
	virtual void			ForceToSleep() = 0;

	virtual void			addSmoothVelocity(const r3dVector& vel) {};
	virtual void			addImpulse(const r3dVector& impulse) {};
	virtual void			AddImpulseAtPos(const r3dPoint3D& impulse, const r3dPoint3D& pos) {};
	virtual void			AddImpulseAtLocalPos(const r3dPoint3D& impulse, const r3dPoint3D& pos) {};
	virtual void			AdjustControllerSize(float new_radius, float new_height, float) {}; 


	// will create a proper physics object
	// if gameObj is null, will use optional params
	static BasePhysicsObject*	CreateCharacterController(const PhysicsObjectConfig& params, PhysicsCallbackObject* gameObj, const r3dVector* optionalPos=0, const r3dVector* optionalSize=0, const r3dMesh* optionalMesh=0, const D3DXMATRIX* optionalRotation=0);
	static BasePhysicsObject*	CreateStaticObject(const PhysicsObjectConfig& params, PhysicsCallbackObject* gameObj, const r3dVector* optionalPos=0, const r3dVector* optionalSize=0, const r3dMesh* optionalMesh=0, const D3DXMATRIX* optionalRotation=0);
	static BasePhysicsObject*	CreateDynamicObject(const PhysicsObjectConfig& params, PhysicsCallbackObject* gameObj, const r3dVector* optionalPos=0, const r3dVector* optionalSize=0, const r3dMesh* optionalMesh=0, const D3DXMATRIX* optionalRotation=0);

	virtual PxActor* getPhysicsActor() = 0;

	bool IsStatic() ;
};

class PhysObj : public BasePhysicsObject
{
	friend BasePhysicsObject;
protected:
	PhysObj();
	void SetActor(PxActor* actor) { Actor = actor; }

	r3dPoint3D m_PositionDifference;
public:
	virtual PxActor* getPhysicsActor() { return Actor; }
	virtual ~PhysObj();

	virtual void			Move(const r3dPoint3D& move, float sharpness); // move object, used for controller
	virtual void 			SetPosition(const r3dPoint3D& pos);
	virtual void 			SetRotation(const r3dVector& Angles);
	virtual void 			SetVelocity(const r3dPoint3D& vel);
	virtual void 			SetScale(const r3dPoint3D& scale);
	virtual r3dPoint3D 		GetPosition() const;
	virtual D3DXMATRIX 		GetRotation() const;
	virtual r3dPoint3D 		GetVelocity() const;
	virtual r3dPoint3D 		GetScale() const;
	virtual bool			IsSleeping();
	virtual void			ForceToSleep();

	virtual void			SetPositionCorrection(const r3dPoint3D& diff) { m_PositionDifference = diff; }


	virtual void			AddImpulseAtPos(const r3dPoint3D& impulse, const r3dPoint3D& pos);
	virtual void			AddImpulseAtLocalPos(const r3dPoint3D& impulse, const r3dPoint3D& pos);
	virtual void			addSmoothVelocity(const r3dVector& vel);
	virtual void			addImpulse(const r3dVector& impulse);
private:
	PxActor* Actor;
};

class RaycastDummyPhysObj : public PhysObj
{
	friend BasePhysicsObject;
protected:
	RaycastDummyPhysObj();

public:
	virtual ~RaycastDummyPhysObj();

	void SetOffset( const r3dPoint3D& offset );

	virtual void			SetPosition( const r3dPoint3D& pos ) OVERRIDE ;
	virtual r3dPoint3D 		GetPosition() const OVERRIDE ;

	virtual void 			SetVelocity(const r3dPoint3D& vel) OVERRIDE ;
	virtual bool			IsSleeping() OVERRIDE ;

	virtual void			AddImpulseAtPos(const r3dPoint3D& impulse, const r3dPoint3D& pos) OVERRIDE ;

	virtual void			AdjustControllerSize(float new_radius, float new_height, float offset)
	{
		Offset.y = offset;
	}
private:
	r3dPoint3D	Offset;

};

class ControllerPhysObj : public BasePhysicsObject
{
	friend BasePhysicsObject;
protected:
	ControllerPhysObj();
	void SetController(PxController* contr) { Controller = contr; }

	float m_HeightOffset;
public:
	virtual PxActor* getPhysicsActor();
	virtual ~ControllerPhysObj();

	virtual void			Move(const r3dPoint3D& move, float sharpness); // move object, used for controller
	virtual void 			SetPosition(const r3dPoint3D& pos);
	virtual void 			SetRotation(const r3dVector& Angles);
	virtual void 			SetVelocity(const r3dPoint3D& vel);
	virtual void 			SetScale(const r3dPoint3D& vel);
	virtual r3dPoint3D 		GetPosition() const;
	virtual D3DXMATRIX 		GetRotation() const;
	virtual r3dPoint3D 		GetVelocity() const;
	virtual r3dPoint3D 		GetScale() const;
	virtual bool			IsSleeping();
	virtual void			ForceToSleep() {};

	virtual void			AdjustControllerSize(float new_radius, float new_height, float offset);

	// todo: implement it!
	virtual void			SetPositionCorrection(const r3dPoint3D& diff) {}

private:
	class PxController*		Controller;
};

#endif //__PHYS_OBJ_H__