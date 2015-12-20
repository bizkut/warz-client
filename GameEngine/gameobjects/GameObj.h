#ifndef	__PWAR_GAMEOBJ_H
#define	__PWAR_GAMEOBJ_H

#include "AObject.h"
#include "PhysObj.h"
#include "sceneBox.h"

// classes defined here..
class PhysicsCallbackObject;
class GameObject;
class MeshGameObject;
class DummyObject;
class INetworkHelper;
class obj_Prefab;

// stack for special object drawing order
#define OBJ_DRAWORDER_FIRST		100
#define OBJ_DRAWORDER_NORMAL		200
#define OBJ_DRAWORDER_LAST		300


//
// types of game objects, used for type checking/casting
//
enum EGameObjectType
{
	OBJTYPE_Generic				= (1<<0),
	OBJTYPE_Terrain				= (1<<1),		// terrain object
	OBJTYPE_Mesh				= (1<<2),		// mesh object
	OBJTYPE_Road				= (1<<3),		// road object
	OBJTYPE_Building			= (1<<4),		// building object
	OBJTYPE_Human				= (1<<5), // obj_Player
	OBJTYPE_Particle			= (1<<6), // particle emitter
	OBJTYPE_Sound				= (1<<7), // sound emitter
	OBJTYPE_Trees				= (1<<8), // all world trees
	OBJTYPE_ApexDestructible	= (1<<11),
	OBJTYPE_AnimMesh			= (1<<12),
	OBJTYPE_DecalProxy			= (1<<13),
	OBJTYPE_Sprite				= (1<<15),
	OBJTYPE_Vehicle				= (1<<16),
	OBJTYPE_Zombie				= (1<<17),
	OBJTYPE_GameplayItem		= (1<<18),
	OBJTYPE_CollectionProxy		= (1<<19),
	OBJTYPE_ItemSpawnPoint		= (1<<20),
	OBJTYPE_SharedUsableItem	= (1<<21),
	OBJTYPE_PlayerSpawnPoint	= (1<<22),
	OBJTYPE_Prefab				= (1<<23),
	OBJTYPE_MechWarrior			= (1<<24),
	OBJTYPE_Barricade			= (1<<25),
	OBJTYPE_VehicleSpawnPoint	= (1<<26),
	OBJTYPE_UAV					= (1<<27)

// TODO: Remove the NoClipBox enum when we get the FairFight API.
#if !defined(WO_SERVER) && !defined(FINAL_BUILD)
	,OBJTYPE_NoClipBox			= (1<<30)
	,OBJTYPE_NoClipPlane	    = (1<<31)
#endif // !defined(WO_SERVER) && !defined(FINAL_BUILD)
};


enum EGameObjectDrawType
{
	OBJ_DRAW_DEFAULT	= 0,
	OBJ_DRAW_SIMPLE	= 1,
	OBJ_DRAW_BLACK	= 2,

	OBJ_DRAW_USERDEFINED	= 100,
};


enum EGameObjectEventType
{
	OBJ_EVENT_USER	= 100,

	OBJ_EVENT_GETEDITORCOMMANDS = 10000,
};


enum EGameObjectFlags
{
	// object need be skipped in world object list when user traverse objects, 
	// usually used for all special objects like terrain, etc..
	OBJFLAG_SkipCastRay	= (1<<4),
	// object need be skipped when ObjectManager::Draw() loop occur.
	// usually used for objects with special drawing order.
	OBJFLAG_SkipDraw	= (1<<5),		

	// do not check occlusion for this object (usually huge objects, or objects that do not have proper aabb for whatever reason)
	//OBJFLAG_SkipOcclusionCheck = (1<<6),

	OBJFLAG_ForceSceneBoxBBox = (1<<7),

	OBJFLAG_DisableShadows = (1<<8),

	// for editor and debug, will always draw an object, no matter what
	OBJFLAG_AlwaysDraw	= (1<<9),		

	OBJFLAG_PlayerCollisionOnly = (1<<10), // object is collision only for player only

	OBJFLAG_AsyncLoading = (1<<11),

	// internal flags
	OBJFLAG_JustCreated	= (1<<20),		// object is just created
	OBJFLAG_WasDestroyed = (1<<21),		// set if OnDestroy was called for this object

	OBJFLAG_Removed	= (1<<23),			// in undo buffer

	//	Is object should be culled using map detailed objects cull radius
	OBJFLAG_Detailed = (1<<24),

	OBJFLAG_ForceSleep = (1<<25),

	OBJFLAG_IconSelection = (1<<26),

	OBJFLAG_UserFlag = (1<<27), // temporary user flag for different purposes.
	
	OBJFLAG_AlwaysUpdate = (1<<28)
};

#define DISABLE_SHADOWS_FLAGS ( OBJFLAG_DisableShadows | OBJFLAG_SkipDraw | OBJFLAG_PlayerCollisionOnly )

enum ECollisionTypes
{
	CLT_Nothing = 0,
	CLT_Face    = 100,
	CLT_Side,
	CLT_Vertex,
	CLT_Box,
	CLT_EnumSize,
};

enum EGameObjQualityLevel
{
	OBJQ_Low=1,
	OBJQ_Medium,
	OBJQ_High,
	OBJQ_Ultra
};

enum ESerializeFile
{
	SF_LevelData = 0,
	SF_SoundData,
	SF_ServerData
};

struct CollisionInfo
{
	int		Type;			// CLT_*
	PhysicsCallbackObject *pObj;			

	// common data
	float	  	ClipCoef;
	float		Distance;		// distance between objects
	r3dPoint3D	NewPosition;
	r3dVector 	Normal;
	r3dMaterial* Material;

	//	Object specific info about collision
	int objectInfo;

	// for CLT_Face type
	r3dMesh	*pMeshObj;		// collided mesh object
	CollisionInfo() {pMeshObj=0; Material = 0; Distance = 9999999; objectInfo = 0;};
};

enum gobjid_e { invalidGameObjectID = 0 };
#pragma pack(push)
#pragma pack(1)
class gobjid_t
{
private:
	DWORD	id;
public:
	__forceinline gobjid_t() : id(invalidGameObjectID) {};
	__forceinline gobjid_t(gobjid_e in_id) : id(in_id) {}
	__forceinline explicit gobjid_t(DWORD in_id) : id(in_id) {}
	__forceinline gobjid_t &gobjid_t::operator=(const gobjid_t& rhs) {
		id = rhs.id;
		return *this;
	}
	__forceinline gobjid_t &gobjid_t::operator=(const gobjid_e rhs) {
		id = rhs;
		return *this;
	}
	__forceinline int operator==(const gobjid_t& rhs) const {
		return id == rhs.id;
	}
	__forceinline int operator!=(const gobjid_t& rhs) const {
		return id != rhs.id;
	}
	__forceinline int operator==(const gobjid_e rhs) const {
		return id == rhs;
	}
	__forceinline int operator!=(const gobjid_e rhs) const {
		return id != rhs;
	}
	__forceinline DWORD get() const {
		return id;
	}
	__forceinline void set(DWORD in_id) {
		id = in_id;
	}
	__forceinline bool valid() const {
		return id != invalidGameObjectID;
	}
};
#pragma pack(pop)

struct ShadowExtrusionData
{
	D3DXMATRIX ToExtrusionBox;

	float MinX, MinY, MinZ;
	float MaxX, MaxY, Extrude;

	ShadowExtrusionData();

	void Displace( const r3dPoint3D& delta );
};

class PhysicsCallbackObject
{
public:

	virtual void OnContactModify(PhysicsCallbackObject *obj, PxContactSet& contacts) {};
	virtual	void OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace) = 0;
	virtual void OnTrigger(PxPairFlag::Enum status, PhysicsCallbackObject* otherObj) {};

	virtual	void OnControllerHit(PhysicsCallbackObject *obj) {};

	virtual GameObject* isGameObject() = 0;
	virtual r3dMesh* hasMesh() = 0;
	virtual r3dMaterial * GetMaterial(uint32_t faceID) { return 0; }
};


//---------------------------------------------------------

typedef r3dTL::TArray< GameObject* > GameObjects;

extern r3dgameVector(GameObject*) gSkipOcclusionCheckGameObjects;

class GameObject : public AObject, public PhysicsCallbackObject
{
	DECLARE_CLASS(GameObject, AObject)

private:
	uint32_t		hashID; // every object has a unique hash id, that is created when object is created and also serialized
	// do not use it until object is fully created (after serialization and after OnCreate())
	// it will load one from serialization, or will create a new one in OnCreate()
	
	r3dPoint3D		vLoadTimePos; // for dynamic objects that are moved by physics in editor, we don't want to update their position in this case. only if moved by designer
	r3dPoint3D		vScl;
	r3dPoint3D		vPos;
	r3dPoint3D		vLoadTimeRot;
	r3dPoint3D		vRot;

	DWORD		NetworkID; // should be private! Do not change visibility

	enum EPrivateFlags
	{
		PRIVFLAG_TransparentShadowCaster	= 1 << 0
	};

	int				PrivateFlags ;

protected:
	// linked list pointers inside ObjectManager
	friend class ObjectManager;
	r3dSec_type<GameObject*, 0x37BFD5F5> pPrevObject;
	r3dSec_type<GameObject*, 0x1FDA937A> pNextObject;

protected:
	int				FirstUpdate;

public:
	bool			wasSetSkipOcclusionCheck;
	void			setSkipOcclusionCheck(bool set);

	bool			m_isSerializable; // set it to false if you don't want that object to write to leveldata.xml (for example particle emmiters)
	bool			InMainFrustum;

	EGameObjQualityLevel m_MinQualityLevel; // objects have to implement their own logic for this variable, by default set to low
	
	/**	Internal storage for vertex shader constants precalculated values. */
	PrecalculatedMeshShaderConsts preparedVSConsts;
public:

	bool			isSerializable() const { return m_isSerializable; }
	uint32_t		GetHashID() const { return hashID; }
	
	/** Regenerate new hash for this object. */
	void			RegenerateHash();

	class SceneBox*	m_SceneBox;

private:

	r3dBoundBox		bbox_local;
	r3dBoundBox		bbox_world;
	float			bbox_radius;
	float     selfIllumMultiplier;

protected:
	D3DXMATRIX		mTransform;

	virtual void UpdateTransform()
	{
		// calc mTransform
		D3DXMATRIX mS, mR, mT;
		D3DXMatrixTranslation(&mT, GetPosition().x, GetPosition().y, GetPosition().z );
		mR = GetRotationMatrix();
		D3DXMatrixScaling( &mS, vScl.x, vScl.y, vScl.z );

		mTransform  = ( mS * mR * mT );

		UpdateWorldBBox();
	}

	void UpdateWorldBBox()
	{
		bbox_world = bbox_local;
		bbox_world.Transform( ( r3dMatrix * )&mTransform );
		bbox_radius = R3D_MAX(R3D_MAX(bbox_world.Size.x, bbox_world.Size.y), bbox_world.Size.z);

		if(m_SceneBox)
			m_SceneBox->Move(this);
	}

	int		m_isActive;	// if > 0 = active. if<=0 - not active and will be deleted soon!
public:

	const r3dBoundBox &		GetBBoxLocal		() const		{ return bbox_local; }
	const r3dBoundBox &		GetBBoxWorld		() const		{ return bbox_world; }
	float					GetObjectsRadius	() const		{ return bbox_radius; }

	void					SetBBoxLocal		( const r3dBoundBox &bbox ) 	{ bbox_local = bbox; UpdateWorldBBox(); }
	void					SetBBoxLocalAndWorld( const r3dBoundBox &local, const r3dBoundBox& world ) 	
	{ 
		bbox_local = local; bbox_world = world ; 
	}

	virtual void			SetScale			( const r3dPoint3D & v );
	const r3dPoint3D & 		GetScale			() const						{ return vScl; }


	void AddImpulseAtPos(const r3dVector& impulse, const r3dPoint3D& pos)
	{
		if(PhysicsObject)
			PhysicsObject->AddImpulseAtPos(impulse, pos);
	}


	gobjid_t	ID;
	gobjid_t	ownerID;
	int		CompoundID;
	r3dPoint3D	CompoundOffset;

	gobjid_t	GetSafeID() const {
		if(this == NULL) return invalidGameObjectID;
		else             return ID;
	};

	bool		NetworkLocal;		// TRUE if local computer process this object

	DWORD		GetNetworkID() const { return NetworkID; }
	bool		SetNetworkID(DWORD id);

	DWORD		GetSafeNetworkID() const {
		if(this == NULL) return invalidGameObjectID;
		else             return NetworkID;
	}

#ifdef WO_SERVER
	virtual INetworkHelper*	GetNetworkHelper();
#endif	

	// engine variables
	int			ObjTypeFlags;		// type of world object
	int			ObjFlags;
	//	This functions are needed to perform additional operations during flags setting
	virtual void SetObjFlags(int objFlags)
	{
		ObjFlags |= objFlags;
	}
	virtual void ResetObjFlags(int objFlags)
	{
		ObjFlags &= ~objFlags;
	}

	int			bPersistent;
	r3dString		Name;			// name of object
	r3dString		FileName;		// parameter passed to loading

	bool	isActive() const { return m_isActive>0; }
	void	setActiveFlag(int var)  // var can be zero - means schedule a deletion of this object. 1 - active. <0 - set only by object manager
	{ 
		if(var>=0)
		{
			r3d_assert(m_isActive>=0); // make sure that object manager isn't trying to schedule a deletion of this object already! otherwise it might not be deleted ever
			m_isActive = 0; 
		}
		else
			m_isActive = var;
	}
	int		getActiveFlag() const { return m_isActive; }
	bool	isDetailedVisible() const;

	struct oldstate_s
	{
		r3dPoint3D	Position;
		r3dPoint3D	Velocity;
		r3dPoint3D	Orientation;
	};
	oldstate_s	oldstate;

	float		time_LifeTime;

	int		DrawOrder;
	float	DrawDistanceSq;
	int		bLoaded;

	obj_Prefab * ownerPrefab;

	//	Which file serialized data should go
	ESerializeFile	serializeFile;

	class BasePhysicsObject* PhysicsObject; // may be null if no physics object
	bool				m_bEnablePhysics; // per object physics enable/disable flag. By default enabled
	PhysicsObjectConfig	PhysicsConfig;
	bool				RecreatePhysicsObject;

	virtual r3dMesh* GetObjectMesh() { return 0; }
	virtual r3dMesh* GetObjectLodMesh() { return 0 ;}

	int					m_BulletPierceable; // 0 - not pierceable, 100 - fully pierceable, no loss of damage

	void SavePhysicsData();

	static void LoadPhysicsConfig(const char* meshName, PhysicsObjectConfig& result); // static function to load physics settings, to prevent code duplication
protected:
	void CreatePhysicsData();
	void ReadPhysicsConfig();

	r3dPoint3D	Velocity;

public:
	bool IsOverWater(float& waterDepth);

public:

	ShadowExtrusionData ShadowExData;
	float				LastShadowExtrusion;

	ptrdiff_t   instancingMeshPtr;

	bool				ShadowExDirty;

	int			EditorID;

	virtual	void 			SetPosition(const r3dPoint3D& pos);
	virtual	void 			SetRotationVector(const r3dVector& Angles);
	virtual	void 			SetVelocity(const r3dPoint3D& vel);
	virtual	const r3dPoint3D&	GetPosition() const;
	const r3dPoint3D& GetLoadTimePosition() const { return vLoadTimePos; }
	virtual	const r3dVector&	GetRotationVector() const; // return yaw pitch roll
	virtual D3DXMATRIX	GetRotationMatrix() const; // return rotation matrix
	virtual	const r3dPoint3D&	GetVelocity() const;

	const D3DXMATRIX&		GetTransformMatrix() const		{ return mTransform; }
	void					SetTransformMatrix(const D3DXMATRIX &worldTransfrorm);

	void AppendSlicedShadowRenderables( RenderArray * render_arrays, struct ShadowMapOptimizationData* shadowOptimizationData, int inMainFrustum, const r3dCamera& Cam );

public:
	// object functions
	GameObject();
	virtual			~GameObject();

	// few inline functions

	inline	int		isObjType(int type) const { 
		return ObjTypeFlags & type; 
	}

	// Generic object functions
	virtual	BOOL		Load(const char* name);

	virtual void		AppendTransparentShadowRenderables( RenderArray & rarr, const r3dCamera& cam ) ;

	virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& cam );
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam );
	virtual void		AppendRenderablesInstanced( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam );

	// Return true if gameobject is eligible for instancing rendering
	virtual bool		IsInstancingEligible( const r3dCamera& Cam ) { return false; }
	// Return true if number of inctances is more than threshold to render utilize insdtancing 
	virtual bool		IsInstancedRendered() const { return false; }
	// Increment count of instanced objects
	virtual void		SetInstancingFlag( const r3dCamera& Cam ) {};

	virtual	BOOL		CheckCastRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float fRayLen, float *fDist, int *facen);

	virtual void		SetColor(r3dColor24 &clr);

	virtual void		OnPickerMoved ();
	virtual void		OnPickerScaled ();
	virtual void		OnPickerRotated ();
	virtual void		OnPickerActionEnd ();
	virtual void		OnPickerPick(int someData);
	virtual void		OnPickerDrop();

	// block of network functions
	virtual	BOOL		OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	//virtual	int		OnNetSerializeTo(BYTE* data, int& dataSize) { return 1; }
	//virtual	int		OnNetSerializeFrom(const BYTE* data, int dataSize) { return 1;}

	// User defined functions
	// function called from object manager when something happens		
	virtual	BOOL		OnCreate();				// ObjMan: called before objman take control of this object
	virtual BOOL		OnDestroy();				// ObjMan: calls this right after object was marked as inactive. Destructor will be called only 4 frames later(!!!)
	virtual void		OnDelete();
	
	virtual	GameObject *Clone ();						// Clone
	virtual void		PostCloneParamsCopy(GameObject *pNew);


	virtual BOOL		GetObjStat ( char * sStr, int iLen );

	virtual BOOL		OnPositionChanged();			// ObjMan: when position changed
	virtual	BOOL		OnOrientationChanged();			// ObjMan: when direction changed

	virtual BOOL		Update();				// ObjMan: tick update

	// should return TRUE if done loading, FALSE otherwise
	virtual BOOL		UpdateLoading() ;

	virtual	BOOL		OnEvent(int eventId, void *data);

	virtual void		PreLoadObjectData(void* data);
	virtual	void		PreLoadReadSerializedData(pugi::xml_node& node);

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

	virtual void		OnGameEnded() {};				// ObjMan: called after the current round of gameplay is over. 
	virtual int			IsStatic() { return false; }

	void				UpdatePhysicsEnabled( const int& physicsEnabled ) ;
	void				UpdatePosition( const r3dPoint3D &vPos ) ;
#ifndef FINAL_BUILD

	void				UpdateCollisionOnly( const int& newCollisionOnly ) ;
	void				UpdateDetailObject( const int& detailObject );
	void				UpdateScale( const r3dVector& scale ) ;
	void				UpdateRotation( const r3dVector& vRot ) ;

	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected );
	virtual void		DrawSelected( const r3dCamera& Cam, eRenderStageID DrawState );
	void				DrawPhysxGeometry( const r3dCamera& Cam, eRenderStageID DrawState );
#endif

/// PhysicsCallbackObject functions ////
	virtual	void	 	OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace) {}
	virtual GameObject* isGameObject() { return this; }
	virtual r3dMesh*	hasMesh() { return GetObjectMesh(); }
	void				PrecalculateMatrices();
	void				PrecalculateMatricesIgnoreSkinning( PrecalculatedMeshShaderConsts* oConsts );

	void				SetTransparentShadowCasting( bool enabled ) ;	
	int					IsTransparentShadowCaster() const ;

	void				SetSelfIllumMultiplier(float val) {	selfIllumMultiplier = val;	}
	float				GetSelfIllumMultiplier() const { return selfIllumMultiplier; }

};

R3D_FORCEINLINE
int
GameObject::IsTransparentShadowCaster() const
{
	return PrivateFlags & PRIVFLAG_TransparentShadowCaster ;
}

R3D_FORCEINLINE
void ShadowExtrusionData::Displace( const r3dPoint3D& delta )
{
	ToExtrusionBox.m[3][0] += delta.x;
	ToExtrusionBox.m[3][1] += delta.y;
	ToExtrusionBox.m[3][2] += delta.z;

	MinX += delta.x;
	MinY += delta.y;
	MinZ += delta.z;

	MaxX	+= delta.x;
	MaxY	+= delta.y;
	Extrude	+= delta.z;	
}

int IsParentOrEqual( const AClass* This,  const AClass* SuspectedChild );

template< typename T, typename C >
void PropagateChange( T newValue, T C::*ptr, GameObject* obj, const r3dTL::TArray< GameObject* >& objects )
{
	if( ((C*)obj)->*ptr != newValue )
	{
		for( int i = 0, e = objects.Count() ; i < e ; i ++ )
		{
			((C*)objects[ i ])->*ptr = newValue ;
		}
	}
}

template< typename T, typename C >
void PropagateChange( T newValue, T C::*ptr, const r3dTL::TArray< GameObject* >& objects )
{
	for( int i = 0, e = objects.Count() ; i < e ; i ++ )
	{
		((C*)objects[ i ])->*ptr = newValue ;
	}

}


template< typename T, typename U, typename C, typename S0 >
void PropagateChange( U newValue, T C::*ptr, U S0::*sptr0, GameObject* obj, const r3dTL::TArray< GameObject* >& objects )
{
	if( (((C*)obj)->*ptr).*sptr0 != newValue )
	{
		for( int i = 0, e = objects.Count() ; i < e ; i ++ )
		{
			(((C*)objects[ i ])->*ptr).*sptr0 = newValue ;
		}
	}
}

template< typename T, typename U, typename V, typename C, typename S0, typename S1 >
void PropagateChange( V newValue, T C::*ptr, U S0::*sptr0, V S1::*sptr1, GameObject* obj, const r3dTL::TArray< GameObject* >& objects )
{
	if( ((((C*)obj)->*ptr).*sptr0).*sptr1 != newValue )
	{
		for( int i = 0, e = objects.Count() ; i < e ; i ++ )
		{
			((((C*)objects[ i ])->*ptr).*sptr0).*sptr1 = newValue ;
		}
	}
}

template< typename T, typename C >
void PropagateChange( T newValue, void (C::*updateFunc) ( const T& newValue ), const r3dTL::TArray< GameObject* >& objects )
{
	for( int i = 0, e = objects.Count() ; i < e ; i ++ )
	{
		(((C*)objects[ i ])->*updateFunc) ( newValue ) ;
	}
}

template< typename C >
void PropagateChange( void (C::*updateFunc) (), const r3dTL::TArray< GameObject* >& objects )
{
	for( int i = 0, e = objects.Count() ; i < e ; i ++ )
	{
		(((C*)objects[ i ])->*updateFunc) () ;
	}
}


R3D_FORCEINLINE int CheckObjectDistance( GameObject* obj, float distSq, float defDistSq )
{
	float cmpDistSq = obj->DrawDistanceSq ? obj->DrawDistanceSq : defDistSq;

	return distSq <= cmpDistSq || !r_allow_distance_cull->GetInt();
}

R3D_FORCEINLINE int CheckObjectSize( GameObject* obj, float minObjectSize )
{
	if( r_allow_size_cull->GetInt() )
	{
		return obj->GetObjectsRadius() > r_cull_size->GetFloat();
	}
	else
	{
		return obj->GetObjectsRadius() > minObjectSize;
	}
}

#include "obj_Mesh.h"

#endif	// __PWAR_GAMEOBJ_H
