//
// simple GameObject
//
#include "r3dPCH.h"
#include "r3d.h"

#include "GameObjects/GameObj.h"
#include "GameObjects/EventTransport.h"

#include "GameCommon.h"

#include "sceneBox.h"
#include "../../../EclipseStudio/Sources/ObjectsCode/WORLD/obj_Prefab.h"

extern bool g_bEditMode;
extern float getWaterDepthAtPos(const r3dPoint3D& pos);

r3dgameVector(GameObject*) gSkipOcclusionCheckGameObjects;

IMPLEMENT_CLASS(GameObject, "GameObject", "Object");
AUTOREGISTER_CLASS(GameObject);

void GameObject::setSkipOcclusionCheck(bool set)
{
	if(!wasSetSkipOcclusionCheck && set)
		gSkipOcclusionCheckGameObjects.push_back(this);
	else if(wasSetSkipOcclusionCheck && !set)
		gSkipOcclusionCheckGameObjects.erase(std::find(gSkipOcclusionCheckGameObjects.begin(), gSkipOcclusionCheckGameObjects.end(), this));
	wasSetSkipOcclusionCheck = set;
}

//
// GameObject
//

static int g_EditIdGen = 1;

GameObject::GameObject()
	: vPos( 0, 0, 0 )
	, vScl( 1, 1, 1 )	
	, vRot( 0, 0, 0 )
	, vLoadTimePos(0 ,0, 0)
	, m_isActive(1)
	, ShadowExDirty(1)
	, DrawDistanceSq( 0 )
	, wasSetSkipOcclusionCheck(false)
	, serializeFile(SF_LevelData)
	, selfIllumMultiplier(1.0f)
{
	m_bEnablePhysics = true;
	m_isSerializable = true;
	InMainFrustum = false;
	FirstUpdate = 10;
	m_SceneBox  = 0;
	hashID		 = 0;
	ID           = invalidGameObjectID;
	ownerID      = invalidGameObjectID;
	CompoundID	 = 0;
	bPersistent	 = 1;
	
	pPrevObject  = NULL;
	pNextObject  = NULL;

	ObjTypeFlags = OBJTYPE_Generic;
	PrivateFlags = 0 ;
	ObjFlags     = 0;
	m_MinQualityLevel = OBJQ_Low;
	
	Name         = "GameObject";

	NetworkID    = 0;
	NetworkLocal = false;

	PhysicsObject = 0;
	RecreatePhysicsObject = false;
	Velocity     = r3dPoint3D(0, 0, 0);

	m_BulletPierceable = 0;
	

	time_LifeTime   = 0;

	DrawOrder	= OBJ_DRAWORDER_NORMAL;
	
	bbox_local.Org        = r3dPoint3D(0, 0, 0);
	bbox_local.Size       = r3dPoint3D(-1, -1, -1);
	bbox_world = bbox_local;
	bbox_radius = 0.0f;

	LastShadowExtrusion = 0.f;

	bLoaded = 0 ;
	ownerPrefab = 0;

	EditorID = g_EditIdGen++;

	return;
}

//-----------------------------------------------------------------------
GameObject::~GameObject()
//-----------------------------------------------------------------------
{
#ifndef FINAL_BUILD
	r3d_assert(pPrevObject == NULL);
	r3d_assert(pNextObject == NULL);
#endif

	if(PhysicsObject) delete PhysicsObject;

	setSkipOcclusionCheck(false);	

	if(NetworkID>0)
	{
		GameWorld().NetworkIDMap.erase(NetworkID);
		NetworkID = 0;
	}
}

void MatrixGetYawPitchRoll ( const D3DXMATRIX & mat, float & fYaw, float & fPitch, float & fRoll );
void NormalizeYPR ( r3dVector & ypr );

BOOL GameObject::Update()
{
	// NOTE : IsSleeping perfomance is slow when scaled to 1000 objects,
	// that's why we use perma sleep flag ( most of these objects always sleep )
	if(
			!( ObjFlags & OBJFLAG_ForceSleep )
				&&
			PhysicsObject
				&& 
			(!PhysicsObject->IsSleeping() || FirstUpdate) )
	{
		if(FirstUpdate != 0)
			--FirstUpdate;
		// physics object is not sleeping, so let's update our position to match physics, and update transform
		r3dVector physPos = PhysicsObject->GetPosition();

		// sometimes physics returns QNAN position, not sure why
		if( r3d_float_isFinite(physPos.x) && r3d_float_isFinite(physPos.y) && r3d_float_isFinite(physPos.z) )
		{
			if( ( vPos - physPos ).LengthSq() > 32 * FLT_EPSILON )
				ShadowExDirty = true ;

			vPos = physPos;

			D3DXMATRIX mat = PhysicsObject->GetRotation();
			mat.m[3][0] = 0.0f; mat.m[3][1] = 0.0f; mat.m[3][2] = 0.0f; mat.m[3][3] = 1.0f;
			r3dVector rot;
			MatrixGetYawPitchRoll ( mat, rot.x, rot.y, rot.z );
			NormalizeYPR ( rot );
			rot.x = R3D_RAD2DEG ( rot.x ); rot.y = R3D_RAD2DEG ( rot.y ); rot.z = R3D_RAD2DEG ( rot.z );
			if(!vRot.AlmostEqual(rot, 0.01f)) // do not change rotation if it didn't change
			{
				vRot = rot;
				ShadowExDirty = true ;
			}

			Velocity = PhysicsObject->GetVelocity();

			UpdateTransform();
		}
	}
	return TRUE;
}

/*virtual*/
BOOL GameObject::UpdateLoading()
{
	return TRUE ;
}

void GameObject::SetPosition(const r3dPoint3D& pos)
{
	if(r3d_float_isFinite(pos.x) && r3d_float_isFinite(pos.y) && r3d_float_isFinite(pos.z)) // sanity check
	{
		if(PhysicsObject)
			PhysicsObject->SetPosition(pos);
		vPos    = pos;
		vLoadTimePos = pos;

		ShadowExDirty = true ;

		UpdateTransform();
	}
}

void GameObject::SetRotationVector(const r3dVector& Angles)
{
	if(vRot.AlmostEqual(Angles))
		return;

	if(PhysicsObject)
		PhysicsObject->SetRotation(Angles);
 	vRot = Angles;
	vLoadTimeRot = Angles;

	ShadowExDirty = true ;

	UpdateTransform();
}

void GameObject::SetVelocity(const r3dPoint3D& speed)
{
	if(PhysicsObject)
		PhysicsObject->SetVelocity(speed);
	Velocity    = speed;
}

const r3dPoint3D& GameObject::GetPosition() const
{
	return vPos;
}

const r3dVector& GameObject::GetRotationVector() const
{
	return vRot;
}

D3DXMATRIX GameObject::GetRotationMatrix() const
{
	D3DXMATRIX res;
	D3DXMatrixRotationYawPitchRoll(&res, R3D_DEG2RAD(vRot.X), R3D_DEG2RAD(vRot.Y), R3D_DEG2RAD(vRot.Z));
	return res;
}

const r3dPoint3D& GameObject::GetVelocity() const
{
	return Velocity;
}

void GameObject::SetTransformMatrix(const D3DXMATRIX &worldTransfrorm)
{
	void MatrixGetYawPitchRoll(const D3DXMATRIX &, float &, float &, float &);
	void NormalizeYPR(r3dVector &);

	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &worldTransfrorm);

	D3DXMATRIX rotM;
	D3DXMatrixRotationQuaternion(&rotM, &rot);

	r3dVector a;
	MatrixGetYawPitchRoll(rotM, a.x, a.y, a.z );
	NormalizeYPR (a);

	r3dVector rotDegree = R3D_RAD2DEG(a);
	SetRotationVector(rotDegree);
	SetPosition(r3dVector(pos));
	SetScale(r3dVector(scale));
}

bool GameObject::SetNetworkID(DWORD id)
{
	r3d_assert(NetworkID == 0);
	
	NetworkID = id;
	std::pair<ObjectManager::NetMapType::iterator, bool> result = GameWorld().NetworkIDMap.insert(std::pair<DWORD, GameObject*>(NetworkID, this));
	if(!result.second)
	{
		GameObject* dup = GameWorld().GetNetworkObject(id);
		r3dOutToLog("@@@ DUP Obj pointer: 0x%X\n", (uint32_t)dup);
		r3d_assert(dup); // it SHOULD exist if failed to insert into map
		r3dOutToLog("@@ NetID DUPLICATE!!! ObjectName: %s, ClassName: %s, ActiveFlag: %d\n", dup->Name.c_str(), dup->Class->Name.c_str(), dup->getActiveFlag());

		// and assert!
		r3d_assert(result.second);
	}
	
	return true;
}

#ifdef WO_SERVER
INetworkHelper* GameObject::GetNetworkHelper()
{
	r3dError("implement GameObject::GetNetworkHelper() for object class %s", Class->Name.c_str());
	return NULL;
}
#endif	


void GameObject::AppendSlicedShadowRenderables( RenderArray *render_arrays, ShadowMapOptimizationData* shadowOptimizationData, int inMainFrustum, const r3dCamera& Cam )
{
	// find in which slice out object is
	// squared distances are not working in some edge cases, so using normal distances

	extern float gShadowSunMultiplier;

	r3dPoint3D pos = GetBBoxWorld().Center();

	D3DXVECTOR4 viewPos;

	D3DXVec3Transform( &viewPos, (D3DXVECTOR3*)&pos, &r3dRenderer->ViewMatrix );

	uint8_t shadow_slice = 0;
	float dist = (gCam - pos).Length();

	float objRadiusRaw = this->GetObjectsRadius();
	float objRadius =  objRadiusRaw + this->GetBBoxWorld().Size.y*gShadowSunMultiplier;

	float shadowSlice0 = ShadowSplitDistancesOpaque[0];
	float shadowSlice1 = ShadowSplitDistancesOpaque[1];
	float shadowSlice2 = ShadowSplitDistancesOpaque[2];
	float shadowSlice3 = ShadowSplitDistancesOpaque[3];

	extern float SunVectorXZProj ;
	float shadowDim = objRadius + LastShadowExtrusion * SunVectorXZProj ;

	float shadowsq_max = dist + shadowDim;
	float shadowsq_min = R3D_MAX(dist - shadowDim, shadowSlice0);
	
	const float sC1 = 0.9f; // some overlapping required
	const float sC2 = 1.0f;

	float maX = viewPos.x + objRadius ;
	float miX = viewPos.x - objRadius ;
	float maY = viewPos.y + objRadius ;
	float miY = viewPos.y - objRadius ;

	r3d_assert( r_active_shadow_slices->GetInt() <= 3 ) ;

	int num_slices = r_active_shadow_slices->GetInt() ;
	int cache_last = r_lfsm_wrap_mode->GetInt() ;

	if( (shadowsq_min >= shadowSlice0*sC1 && shadowsq_min < shadowSlice1*sC2) || (shadowsq_max > shadowSlice0*sC1 && shadowsq_max < shadowSlice1*sC2) )
	{
		if( inMainFrustum || ( num_slices == 1 && cache_last ) )
		{
			if( objRadiusRaw > r_shadow_slice0_min_size->GetFloat() )
			{
				AppendShadowRenderables( render_arrays[ 0 ], 0, Cam );
				AppendShadowOptimizations( &shadowOptimizationData[ 0 ], miX, maX, miY, maY );
			}
		}
	}
	
	if( ( ( shadowsq_min >= shadowSlice1*sC1 || num_slices == 2 ) && shadowsq_min < shadowSlice2*sC2) || (shadowsq_max > shadowSlice1*sC1 && shadowsq_max < shadowSlice2*sC2) || (shadowsq_min <= shadowSlice1*sC1 && shadowsq_max > shadowSlice2*sC2) )
	{
		if( inMainFrustum || ( num_slices == 2 && cache_last ) )
		{
			if( objRadiusRaw > r_shadow_slice1_min_size->GetFloat() )
			{
				AppendShadowRenderables( render_arrays[ 1 ], 1, Cam );
				AppendShadowOptimizations( &shadowOptimizationData[ 1 ], miX, maX, miY, maY );
			}
		}
	}

	if( ( ( shadowsq_min >= shadowSlice2*sC1 || num_slices == 3 ) && shadowsq_min < shadowSlice3*sC2) || (shadowsq_max > shadowSlice2*sC1 && shadowsq_max < shadowSlice3*sC2) || (shadowsq_min <= shadowSlice2*sC1 && shadowsq_max > shadowSlice3*sC2)  )
	{
		if( inMainFrustum || ( num_slices == 3 && cache_last ) )
		{
			if( objRadiusRaw > r_shadow_slice2_min_size->GetFloat() )
			{
				AppendShadowRenderables( render_arrays[ 2 ], 2, Cam );
				AppendShadowOptimizations( &shadowOptimizationData[ 2 ], miX, maX, miY, maY );
			}
		}
	}

}

void GameObject::AppendTransparentShadowRenderables( RenderArray & rarr, const r3dCamera& cam )
{
	(void)rarr, (void)cam;
}

void GameObject::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& cam )
{
	(void)rarr, (void)cam;
}

void GameObject::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam  )
{
	(void)render_arrays, (void)cam;
}

void GameObject::AppendRenderablesInstanced( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam )
{
	(void)render_arrays, (void)cam;
}

void GameObject::OnPickerMoved ()
{

}

void GameObject::OnPickerScaled ()
{

}

void GameObject::OnPickerRotated ()
{

}

void GameObject::OnPickerActionEnd()
{

}

void GameObject::OnPickerPick(int someData)
{
#ifndef WO_SERVER
	if (ownerPrefab)
	{
		ownerPrefab->SelectPrefab();
	}
#endif
}

void GameObject::OnPickerDrop()
{

}

void GameObject::RegenerateHash()
{
	// create a new hash for this object
	uint32_t counter = timeGetTime();
	time_t seconds = time(NULL);
	tm* date = localtime(&seconds);
	int rnd = rand();
	char hash_string[512]= {0};
	sprintf_s(hash_string, _countof(hash_string), "%s_%d_%d%d%d_%d:%d:%d_%d", FileName.c_str(), counter, date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec, rnd);
	hashID = r3dHash::MakeHash(hash_string);
}

BOOL GameObject::Load(const char* fname)
{
	char temp_name[512];
	r3dscpy(temp_name, fname);
	for(uint32_t i=0; i<strlen(temp_name); ++i)
	{
		if(temp_name[i] == '\\')
		{
			temp_name[i] = '/';
		}
	}	

	char* res = strrchr(temp_name, '/');
	if(res)
		Name = res;
	else 
		Name = temp_name;
	FileName = fname;

	// create a hash for this object, if we are serializing it - we will overwrite it with proper hash id right away during object construction
	// if we are creating it in editor - than we will have a proper hashID right away
	RegenerateHash();

	if( !( ObjFlags & OBJFLAG_AsyncLoading ) )
	{
		bLoaded = 1 ;
	}

	return TRUE;
}


BOOL GameObject::OnPositionChanged()
{
	if(m_SceneBox)
		m_SceneBox->Move(this);

	return TRUE;
};

BOOL GameObject::OnOrientationChanged()
{
	return TRUE;
}


BOOL GameObject::OnEvent(int eventId, void *data)
{
	return FALSE;
}



BOOL GameObject::OnCreate()
{
	R3DPROFILE_FUNCTION("GameObject::OnCreate");

	// we should only call it once
	r3d_assert((ObjFlags & OBJFLAG_JustCreated) && "multiple OnCreate calls");
	ObjFlags &= ~OBJFLAG_JustCreated;
	//r3dOutToLog("!!!!!!! ONCREATE %s\n", Name.c_str());

	if(!PhysicsConfig.ready)
		ReadPhysicsConfig(); // in case if we are creating object in editor, we need to load physics separately, as we are not calling ReadSerializedData
	CreatePhysicsData();

	extern void ObjectManager_Copy_OldState(GameObject *obj);
	ObjectManager_Copy_OldState(this);

	return TRUE;
}

BOOL GameObject::OnDestroy()
{
	if (ownerPrefab)
	{
		ownerPrefab->NotifyObjectDestruction(this);
	}
	return TRUE;
}

void GameObject::OnDelete()
{

}

GameObject * GameObject::Clone ()
{
	return NULL;
}

void GameObject::PostCloneParamsCopy(GameObject *pNew)
{
	if (pNew && ownerPrefab)
	{
		ownerPrefab->AddNewObject(pNew);
		pNew->ownerPrefab = ownerPrefab;
	}
}

BOOL GameObject::GetObjStat ( char * sStr, int iLen )
{
	sprintf_s (sStr, iLen, "Obj type \'%s\'\nObj name \'%s\'\nPOS \'[%f,%f,%f]\'\n", Class->Name.c_str (), Name.c_str(),GetPosition().x,GetPosition().y,GetPosition().z);
	return TRUE;
}


/*BOOL GameObject::CheckCollision(GameObject *obj, CollisionMove &move, CollisionInfo &trace)
{
	return FALSE;
}
*/
BOOL GameObject::CheckCastRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float fRaytLen, float *fDist, int *facen)
{
	return FALSE;
}

void GameObject::SetColor(r3dColor24 &clr)
{
}

void GameObject::PreLoadObjectData(void* data)
{

}

void GameObject::PreLoadReadSerializedData(pugi::xml_node& node)
{

}

void GameObject::ReadSerializedData(pugi::xml_node& node)
{
	vLoadTimePos = vPos;
	// position is already set when object was created
	r3dVector A;

	pugi::xml_node gameObjNode = node.child("gameObject");
	hashID = gameObjNode.attribute("hash").as_uint();
	if(hashID == 0x7FFFFFFF)
		hashID = 0;
	if(!gameObjNode.attribute("PhysEnable").empty())
		m_bEnablePhysics = gameObjNode.attribute("PhysEnable").as_bool();
	if(!gameObjNode.attribute("MinQuality").empty())
		m_MinQualityLevel = (EGameObjQualityLevel)gameObjNode.attribute("MinQuality").as_int();
	if(!gameObjNode.attribute("BulletPierceable").empty())
		m_BulletPierceable = gameObjNode.attribute("BulletPierceable").as_int();

	if (!gameObjNode.attribute("DisableShadows").empty() && gameObjNode.attribute("DisableShadows").as_bool())
		ObjFlags |= OBJFLAG_DisableShadows;
	if (!gameObjNode.attribute("CollisionPlayerOnly").empty() && gameObjNode.attribute("CollisionPlayerOnly").as_bool())
	{
		setSkipOcclusionCheck( true );
		ObjFlags |= OBJFLAG_PlayerCollisionOnly | OBJFLAG_DisableShadows;
		if(!g_bEditMode)
			ObjFlags |= OBJFLAG_SkipDraw;
	}
	if (!gameObjNode.attribute("DetailObject").empty() && gameObjNode.attribute("DetailObject").as_bool())
	{
		ObjFlags |= OBJFLAG_Detailed;
	}
		
	pugi::xml_node rotNode = gameObjNode.child("rotation");
	if(!rotNode.empty())
	{
		A.x = rotNode.attribute("x").as_float();
		A.y = rotNode.attribute("y").as_float();
		A.z = rotNode.attribute("z").as_float();
		// fixes a case when there is a trash in rotation. not sure where it's coming from though
		if(A.x < -100000 || A.x > 100000) A.x = 0;
		if(A.y < -100000 || A.y > 100000) A.y = 0;
		if(A.z < -100000 || A.z > 100000) A.z = 0;

		// fixup orientation
		while ( A.X < 0.0f ) 
			A.X += 360.0f;
		while ( A.X > 360.0f ) 
			A.X -= 360.0f;
		while ( A.Y < 0.0f ) 
			A.Y += 360.0f;
		while ( A.Y > 360.0f ) 
			A.Y -= 360.0f;
		while ( A.Z < 0.0f ) 
			A.Z += 360.0f;
		while ( A.Z > 360.0f ) 
			A.Z -= 360.0f;
		SetRotationVector(A);
	}

	pugi::xml_node scaleNode = gameObjNode.child("scale");
	if(!scaleNode.empty())
		SetScale(r3dPoint3D(scaleNode.attribute("x").as_float(),scaleNode.attribute("y").as_float(),scaleNode.attribute("z").as_float()));

	ReadPhysicsConfig();
}

void GameObject::WriteSerializedData(pugi::xml_node& node)
{
	pugi::xml_node gameObjNode = node.append_child();
	gameObjNode.set_name("gameObject");
	gameObjNode.append_attribute("hash") = hashID;
	gameObjNode.append_attribute("PhysEnable") = m_bEnablePhysics;
	gameObjNode.append_attribute("MinQuality") = (int)m_MinQualityLevel;
	gameObjNode.append_attribute("BulletPierceable") = m_BulletPierceable;
	r3dPoint3D rot = vRot;
	if(PhysicsConfig.isDynamic)
		rot = vLoadTimeRot;
	if(!rot.AlmostEqual(r3dPoint3D(0.0f,0.0f,0.0f)))
	{
//		if(!vLoadTimeRot.AlmostEqual(rot))
//			__asm nop;

		pugi::xml_node rotNode = gameObjNode.append_child();
		rotNode.set_name("rotation");
		char temp_str[128];
		// save floats at %.4f to make merging easier
		sprintf(temp_str, "%.4f", rot.x);
		rotNode.append_attribute("x") = temp_str;
		sprintf(temp_str, "%.4f", rot.y);
		rotNode.append_attribute("y") = temp_str;
		sprintf(temp_str, "%.4f", rot.z);
		rotNode.append_attribute("z") = temp_str;
	}

	if(!vScl.AlmostEqual(r3dPoint3D(1.0f,1.0f,1.0f)))
	{
		pugi::xml_node scaleNode = gameObjNode.append_child();
		scaleNode.set_name("scale");
		char temp_str[128];
		// save floats at %.4f to make merging easier
		sprintf(temp_str, "%.4f", vScl.x);
		scaleNode.append_attribute("x") = temp_str;
		sprintf(temp_str, "%.4f", vScl.y);
		scaleNode.append_attribute("y") = temp_str;
		sprintf(temp_str, "%.4f", vScl.z);
		scaleNode.append_attribute("z") = temp_str;
	}

	if (ObjFlags & OBJFLAG_DisableShadows)
		gameObjNode.append_attribute("DisableShadows") = true;
	if (ObjFlags & OBJFLAG_PlayerCollisionOnly)
		gameObjNode.append_attribute("CollisionPlayerOnly") = true;
	if (ObjFlags & OBJFLAG_Detailed)
		gameObjNode.append_attribute("DetailObject") = true;
}


BOOL GameObject::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
  return FALSE;
}

void GameObject::SavePhysicsData()
{
	r3dMesh* mesh = GetObjectMesh();
	r3d_assert(mesh);
	char physicsFilename[256];
	r3dscpy(physicsFilename, mesh->FileName.c_str());
	int len = strlen(physicsFilename);
	r3dscpy(&physicsFilename[len-3], "phx");
	pugi::xml_document xmlFile;
	xmlFile.append_child(pugi::node_comment).set_value("physics data");
	pugi::xml_node xmlPhysics = xmlFile.append_child();
	xmlPhysics.set_name("physics");
	xmlPhysics.append_attribute("collision_group") = PhysicsConfig.group;
	xmlPhysics.append_attribute("type") = PhysicsConfig.type;
	xmlPhysics.append_attribute("mass") = PhysicsConfig.mass;
	xmlPhysics.append_attribute("dynamic") = PhysicsConfig.isDynamic;
	xmlPhysics.append_attribute("explMesh") = PhysicsConfig.needExplicitCollisionMesh;
	xmlPhysics.append_attribute("fastMoving") = PhysicsConfig.isFastMoving;
	xmlPhysics.append_attribute("can_modify_contacts") = PhysicsConfig.canModifyContacts;

	xmlFile.save_file(physicsFilename);
}

void GameObject::LoadPhysicsConfig(const char* meshName, PhysicsObjectConfig& result)
{
	r3d_assert(meshName);
	
	if(strlen(meshName) < 3)
	{
		r3dArtBug("meshName '%s' is too small in LoadPhysicsConfig", meshName);
		return;
	}

	// not sure about this, it might be slow to do that for each game object
	char physicsFilename[256];
	r3dscpy(physicsFilename, meshName);
	int len = strlen(physicsFilename);
	r3dscpy(&physicsFilename[len-3], "phx");
	if(r3d_access(physicsFilename, 0) == 0)
	{
		r3dFile* f = r3d_open(physicsFilename, "rb");
		r3d_assert(f);
		char* fileBuffer = game_new char[f->size + 1];
		fread(fileBuffer, f->size, 1, f);
		fileBuffer[f->size] = 0;
		pugi::xml_document xmlFile;
		pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
		fclose(f);
		if(!parseResult)
			r3dError("Failed to parse XML, error: %s", parseResult.description());
		pugi::xml_node xmlPhysics = xmlFile.child("physics");
		if(xmlPhysics.attribute("collision_group").empty())
		{
			result.group = PHYSCOLL_COLLISION_GEOMETRY;
			int group = xmlPhysics.attribute("group").as_uint();
			if(group == 1)
				result.isDynamic = true;
		}
		else
		{
			result.group = (PHYSICS_COLLISION_GROUPS)xmlPhysics.attribute("collision_group").as_uint();
			result.isDynamic = xmlPhysics.attribute("dynamic").as_bool();
		}
		if(!xmlPhysics.attribute("explMesh").empty())
			result.needExplicitCollisionMesh = xmlPhysics.attribute("explMesh").as_bool();
		if(!xmlPhysics.attribute("fastMoving").empty())
			result.isFastMoving = xmlPhysics.attribute("fastMoving").as_bool();
		if(!xmlPhysics.attribute("can_modify_contacts").empty())
			result.canModifyContacts = xmlPhysics.attribute("can_modify_contacts").as_bool();
		
		result.type = (PHYSICS_TYPE)xmlPhysics.attribute("type").as_uint();
		result.mass = xmlPhysics.attribute("mass").as_float();
		result.ready = true;
		//r3d_assert(result.meshFilename==NULL);
		result.meshFilename = strdup(meshName); 

		delete [] fileBuffer;
	}
}

void GameObject::ReadPhysicsConfig()
{
	if(PhysicsObject == NULL)
	{
#ifndef WO_SERVER
		r3dMesh* mesh = GetObjectMesh();
		if(mesh) 
			 LoadPhysicsConfig(mesh->FileName.c_str(), PhysicsConfig);
#else // WO_SERVER
		// on server, load physics without mesh. mesh->FileName is equal to FileName of the object. 
		// todo: maybe it will be a good idea to rewrite this fn and use only object's FileName, but as of this time I don't want to break anything on client side
		 LoadPhysicsConfig(FileName.c_str(), PhysicsConfig);
#endif
	}

	if(ObjFlags & OBJFLAG_PlayerCollisionOnly)
		PhysicsConfig.group = PHYSCOLL_PLAYER_ONLY_GEOMETRY;
}

void GameObject::CreatePhysicsData()
{
	UpdateTransform();
	if(PhysicsConfig.ready && PhysicsObject == 0 && m_bEnablePhysics)
	{
		if(PhysicsConfig.isDynamic || PhysicsConfig.isKinematic
#ifdef VEHICLES_ENABLED
			|| PhysicsConfig.isVehicle
#endif
			)
		{
			PhysicsObject = BasePhysicsObject::CreateDynamicObject(PhysicsConfig, this);
			ObjFlags &= ~OBJFLAG_ForceSleep ;
		}
		else
		{
			PhysicsObject = BasePhysicsObject::CreateStaticObject(PhysicsConfig, this);
			ObjFlags |= OBJFLAG_ForceSleep ;
		}
	}	
}

bool GameObject::isDetailedVisible() const
{
	if (ObjFlags & OBJFLAG_Detailed)
	{
		r3dVector objVec = gCam - GetPosition();
		float distSquared = objVec.LengthSq();
		float detailRadiusSquared = r_level_detail_radius->GetFloat();
		detailRadiusSquared *= detailRadiusSquared;
		return distSquared < detailRadiusSquared;
	}
	return true;
}


void GameObject::UpdatePhysicsEnabled( const int& physicsEnabled )
{
	if(physicsEnabled != (int)m_bEnablePhysics)
	{
		m_bEnablePhysics = physicsEnabled?true:false;
		if(!m_bEnablePhysics)
		{
			SAFE_DELETE(PhysicsObject);
		}
		else
			CreatePhysicsData();
	}
}

void GameObject::UpdatePosition( const r3dPoint3D &vPos )
{
	SetPosition( vPos );
}

#ifndef FINAL_BUILD
void GameObject::UpdateCollisionOnly( const int& newCollisionOnly )
{
	int collisionOnly = (ObjFlags & OBJFLAG_PlayerCollisionOnly)?1:0;

	if(collisionOnly != newCollisionOnly)
	{
		if(newCollisionOnly)
		{
			setSkipOcclusionCheck(true);
			ObjFlags |= OBJFLAG_PlayerCollisionOnly;
		}
		else
		{
			ObjFlags &= ~(OBJFLAG_PlayerCollisionOnly);
			setSkipOcclusionCheck(false);
		}

		// now we also need to recreate our physics data to update collision
		if(m_bEnablePhysics)
		{
			ReadPhysicsConfig();
			SAFE_DELETE(PhysicsObject);
			CreatePhysicsData();
		}

	}
}

void GameObject::UpdateDetailObject( const int& detailObject )
{
	if (detailObject)
		ObjFlags |= OBJFLAG_Detailed;
	else
		ObjFlags &= ~OBJFLAG_Detailed;
}

void GameObject::UpdateScale( const r3dVector& scale )
{
	SetScale( scale ) ;
}

void GameObject::UpdateRotation( const r3dVector& vRot )
{
	SetRotationVector ( vRot );
}


float GameObject::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected )
{
	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		float starty = scry;

		r3dVector vRotation = vRot;
		if ( vRotation.x < 0.0f )
			vRotation.x += 360.0f;
		if ( vRotation.y < 0.0f )
			vRotation.y += 360.0f;
		if ( vRotation.z < 0.0f )
			vRotation.z += 360.0f;

		r3dVector scale = vScl;

		starty += imgui_Static ( scrx, starty, "Base Parameters" );
		int physicsEnabled = m_bEnablePhysics;
		starty += imgui_Checkbox(scrx, starty, "Enable Physics", &physicsEnabled, 1);

		if( physicsEnabled != (int)m_bEnablePhysics )
		{
			PropagateChange( physicsEnabled, &GameObject::UpdatePhysicsEnabled, selected ) ;
		}

		//------------------------------------------------------------------------
		if( selected.Count() == 1 && this->m_bEnablePhysics )
		{
			if( r3dMesh* mesh = GetObjectMesh() )
			{
				bool hasCookedMesh = g_pPhysicsWorld->HasCookedMesh(mesh);
				bool hasConvexMesh = g_pPhysicsWorld->HasConvexMesh(mesh);

				bool needRecreatePhysObj = false;

				if(imgui_Button(scrx, starty, 160, 22, hasCookedMesh?"Re-cook mesh":"Cook mesh"))
				{
					g_pPhysicsWorld->CookMesh(mesh);
					needRecreatePhysObj = true;
					MessageBoxA( r3dRenderer->HLibWin, hasCookedMesh ? "Re-cooked." : "Cooked.", "Info", MB_OK );
				}
				if(imgui_Button(scrx+160.0f, starty, 160, 22, hasConvexMesh ? "Re-cook convex" : "Cook convex" ) )
				{
					g_pPhysicsWorld->CookConvexMesh(mesh);
					needRecreatePhysObj = true;
					MessageBoxA( r3dRenderer->HLibWin,  hasConvexMesh ? "Re-cooked convex." : "Cooked convex.", "Info", MB_OK );
				}

				if( needRecreatePhysObj )
				{
					if( PhysicsConfig.type == PHYSICS_TYPE_MESH && PhysicsConfig.isDynamic )
						 PhysicsConfig.isDynamic = 0; // mesh cannot be dynamic

					if( PhysicsObject ) 
						delete PhysicsObject; 

					PhysicsObject = 0;

					if( PhysicsConfig.meshFilename )
						free( PhysicsConfig.meshFilename );

					if( PhysicsConfig.destroyedMeshFilename )
					{
						free( PhysicsConfig.destroyedMeshFilename );
						PhysicsConfig.destroyedMeshFilename = NULL;
					}

					PhysicsConfig.meshFilename = strdup( FileName.c_str() );

					if( PhysicsConfig.isDynamic || PhysicsConfig.isKinematic )
						PhysicsObject = BasePhysicsObject::CreateDynamicObject(PhysicsConfig, this);
					else
						PhysicsObject = BasePhysicsObject::CreateStaticObject(PhysicsConfig, this);
				}

				starty += 23.f;
			}
		}
		//------------------------------------------------------------------------

		int collisionOnly = (ObjFlags & OBJFLAG_PlayerCollisionOnly)?1:0;
		int newCollisionOnly = collisionOnly;
		starty += imgui_Checkbox(scrx, starty, "Player only collision", &newCollisionOnly, 1);

		if( newCollisionOnly != collisionOnly )
		{
			PropagateChange( newCollisionOnly, &GameObject::UpdateCollisionOnly, selected ) ;
		}

		int BulletPierceable = m_BulletPierceable;
		starty += imgui_Value_SliderI(scrx, starty, "Bullet Pierceable", &BulletPierceable,	0.0f, 100.0f, "%d");
		if ( (BulletPierceable-m_BulletPierceable) != 0)
			PropagateChange( BulletPierceable, &GameObject::m_BulletPierceable, this, selected ) ;

		const char* quality_level_str[] = {"Low", "Medium", "High", "Ultra"};
		if(imgui_Button(scrx+150, starty, 30, 25, "<"))
		{
			int curQ = (int)m_MinQualityLevel;
			curQ--;
			if(curQ<(int)OBJQ_Low)
				curQ = (int)OBJQ_Ultra;

			PropagateChange( (EGameObjQualityLevel)curQ, &GameObject::m_MinQualityLevel, this, selected );
		}
		if(imgui_Button(scrx+180, starty, 30, 25, ">"))
		{
			int curQ = (int)m_MinQualityLevel;
			curQ++;
			if(curQ>(int)OBJQ_Ultra)
				curQ = (int)OBJQ_Low;

			PropagateChange( (EGameObjQualityLevel)curQ, &GameObject::m_MinQualityLevel, this, selected );
		}

		imgui_Static(scrx, starty, "Minimum Quality: ", 100);
		starty += imgui_Static(scrx+100, starty, quality_level_str[(int)m_MinQualityLevel-1], 50);

		starty += imgui_Value_Slider(scrx, starty, "Scale X", &scale.x,	0.001f, 200.0f,	"%3.2f", 1);
		starty += imgui_Value_Slider(scrx, starty, "Scale Y", &scale.y,	0.001f, 200.0f,	"%3.2f", 1);
		starty += imgui_Value_Slider(scrx, starty, "Scale Z", &scale.z,	0.001f, 200.0f,	"%3.2f", 1);

		starty += imgui_Value_Slider(scrx, starty, "Rotation X", &vRotation.x,	0.0f, 360.0f,	"%3.2f", 1);
		starty += imgui_Value_Slider(scrx, starty, "Rotation Y", &vRotation.y,	0.0f, 360.0f,	"%3.2f", 1);
		starty += imgui_Value_Slider(scrx, starty, "Rotation Z", &vRotation.z,	0.0f, 360.0f,	"%3.2f", 1);
		starty += 1;

		r3dPoint3D vPos = GetPosition();
		starty += imgui_Value_Slider(scrx, starty, "Position X", &vPos.x, -FLT_MAX, FLT_MAX,	"%3.2f", 0);
		starty += imgui_Value_Slider(scrx, starty, "Position Y", &vPos.y, -FLT_MAX, FLT_MAX,	"%3.2f", 0);
		starty += imgui_Value_Slider(scrx, starty, "Position Z", &vPos.z, -FLT_MAX, FLT_MAX,	"%3.2f", 0);
		starty += 1;

		if ( fabsf( scale.x - vScl.x ) > 0.00001f ||
			 fabsf( scale.y - vScl.y ) > 0.00001f ||
			 fabsf( scale.z - vScl.z ) > 0.00001f )
		{
			PropagateChange( scale, &GameObject::UpdateScale, selected ) ;
		}

		if ( fabsf( vRotation.x - vRot.x ) > 0.00001f ||
			 fabsf( vRotation.y - vRot.y ) > 0.00001f ||
			 fabsf( vRotation.z - vRot.z ) > 0.00001f )
		{
			PropagateChange( vRotation, &GameObject::UpdateRotation, selected ) ;
		}

		r3dPoint3D vPosOrig = GetPosition();
		if ( fabsf( vPosOrig.x - vPos.x ) > 0.00001f ||
			 fabsf( vPosOrig.y - vPos.y ) > 0.00001f ||
			 fabsf( vPosOrig.z - vPos.z ) > 0.00001f )
		{
			PropagateChange( vPos, &GameObject::UpdatePosition, selected ) ;
		}

		int detObj = ObjFlags & OBJFLAG_Detailed ? 1 : 0;
		int detObjOld = detObj ;
		starty += imgui_Checkbox(scrx, starty, "Detail object", &detObj, 1);

		if( !!detObj != !!detObjOld )
		{
			PropagateChange(detObj, &GameObject::UpdateDetailObject, selected);
		}

		if (ownerPrefab && ownerPrefab->IsOpenedForEdit())
		{
			if (imgui_Button(scrx, starty, 360.0f, 22.0f, "Select owner prefab"))
			{
				ownerPrefab->SelectPrefab(true, true);
			}
			starty += 22.0f;
		}


		float selfIllum = selfIllumMultiplier;

		starty += imgui_Value_Slider(scrx, starty, "Self Illum Modifier", &selfIllum,	0.0f, 1.0f,	"%3.2f", 1);

		selfIllumMultiplier = selfIllum;

		return starty - scry;
	}
	else
	{
		return 0 ;
	}
}

#endif
#ifndef FINAL_BUILD
/*virtual*/
void
GameObject::DrawSelected( const r3dCamera& Cam, eRenderStageID DrawState )
{
	(void)Cam, (void)DrawState;
}

void
GameObject::DrawPhysxGeometry( const r3dCamera& Cam, eRenderStageID DrawState )
{
	if( PhysicsObject )
	{
		extern int g_FwdColorVS;
		extern int g_FwdColorPS;

		if( PxActor* actor = PhysicsObject->getPhysicsActor() )
		{
			if( PxRigidStatic* rigid = actor->isRigidStatic() )
			{
				PxShape* shapes[ 4 ] = { NULL };

				rigid->getShapes( shapes, sizeof shapes, 0 );

				if( shapes[ 0 ] )
				{
					PxTransform trans = rigid->getGlobalPose();
					r3dPoint3D centre = r3dPoint3D( trans.p.x, trans.p.y, trans.p.z );

					PxGeometryHolder geomHolder = shapes[ 0 ]->getGeometry();
					PxGeometryType::Enum type = geomHolder.getType();

					DWORD prevFillMode;
					D3D_V ( r3dRenderer->pd3ddev->GetRenderState( D3DRS_FILLMODE, &prevFillMode ) );
					D3D_V ( r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME ) );

					r3dRenderer->SetCullMode( D3DCULL_CCW );

					switch( type )
					{
					case PxGeometryType::eTRIANGLEMESH:
						{
							float4 color( 0, 0, 1, 1 );

							D3D_V ( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)&color, 1 ) );

							r3dRenderer->SetPixelShader( g_FwdColorPS );
							r3dRenderer->SetVertexShader( g_FwdColorVS );

							D3DXMATRIX modProj = r3dRenderer->ProjMatrix;

							modProj._43 -= 0.00033f * ( r3dRenderer->ZDir == r3dRenderLayer::ZDIR_INVERSED ? -1.0f : 1.0f );

							D3DXMATRIX modTransform = mTransform;

							modTransform._41 = centre.x;
							modTransform._42 = centre.y;
							modTransform._43 = centre.z;

							D3DXMATRIX modelViewProj = modTransform * r3dRenderer->ViewMatrix * r3dRenderer->ProjMatrix;

							D3DXMatrixTranspose( &modelViewProj, &modelViewProj );

							r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &modelViewProj.m[ 0 ][ 0 ], 4 );

							PxTriangleMeshGeometry tg;
							shapes[ 0 ]->getTriangleMeshGeometry(tg);

							const PxVec3* vertices = tg.triangleMesh->getVertices();

							union
							{
								const UINT16* p16;
								const UINT32* p32;
							};

							p16 = static_cast< const UINT16* >( tg.triangleMesh->getTriangles() );

							int is16Bit = tg.triangleMesh->has16BitTriangleIndices();

							int capacity = r3dRenderer->GetPolygon3DBufferCapacity();

							int offset = 0;

							for( ; ; )
							{
								int toDraw = R3D_MIN( int( tg.triangleMesh->getNbTriangles() - offset ), capacity / 3 );

								if( !toDraw )
									break;

								r3dRenderer->BeginFill3DPolygon(  toDraw * 3 );

								R3D_DEBUG_VERTEX v[ 3 ] = { };

								memset( v, 0, sizeof v );

								v[ 0 ].color = r3dColor::blue;
								v[ 1 ].color = r3dColor::blue;
								v[ 2 ].color = r3dColor::blue;

								for( int i = offset, e = offset + (int)toDraw; i < e; i ++ )
								{
									UINT32 idx0;
									UINT32 idx1;
									UINT32 idx2;

									if( is16Bit )
									{
										idx0 = p16[ i * 3 + 0 ];
										idx1 = p16[ i * 3 + 1 ];
										idx2 = p16[ i * 3 + 2 ];
									}
									else
									{
										idx0 = p32[ i * 3 + 0 ];
										idx1 = p32[ i * 3 + 1 ];
										idx2 = p32[ i * 3 + 2 ];
									}

									v[ 0 ].Pos = (r3dPoint3D&)vertices[ idx0 ];
									v[ 1 ].Pos = (r3dPoint3D&)vertices[ idx1 ];
									v[ 2 ].Pos = (r3dPoint3D&)vertices[ idx2 ];

									r3dRenderer->Fill3DPolygon( 3, v );
								}

								offset += toDraw;

								r3dRenderer->EndFill3DPolygon();
								r3dRenderer->Flush();
							}
						}
						break;

					case PxGeometryType::eBOX:
						{
							r3dRenderer->SetPixelShader();
							r3dRenderer->SetVertexShader();

							PxBoxGeometry boxGeom;
							shapes[ 0 ]->getBoxGeometry( boxGeom );

							r3dPoint3D halfExtent = r3dPoint3D( boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z );

							r3dBoundBox bbox;
							bbox.Org = -halfExtent;
							bbox.Size = halfExtent * 2.f;


							D3DXMATRIX modTransform = mTransform;

							modTransform._41 = centre.x;
							modTransform._42 = centre.y;
							modTransform._43 = centre.z;

							r3dDrawUniformBoundBox( bbox, modTransform, gCam, r3dColor( 255, 0, 255 ) );
						}
						break;
					case PxGeometryType::eCONVEXMESH:
						{
							r3dRenderer->SetPixelShader();
							r3dRenderer->SetVertexShader();

							PxConvexMeshGeometry meshGeom;
							shapes[ 0 ]->getConvexMeshGeometry( meshGeom );

							const PxVec3* vertices = meshGeom.convexMesh->getVertices();

							const PxU8* ibuffer = (PxU8*)meshGeom.convexMesh->getIndexBuffer();

							int dynaVertexCount = 0;

							for( int i = 0, e = (int)meshGeom.convexMesh->getNbPolygons(); i < e; i ++ )
							{
								PxHullPolygon data;
								meshGeom.convexMesh->getPolygonData( i, data );

								if( r3dPoint3D( data.mPlane[ 0 ], data.mPlane[ 1 ], data.mPlane[ 2 ] ).Dot( gCam.vPointTo ) >= 0.f )
									continue;

								dynaVertexCount += data.mNbVerts * 2;
							}

							r3dRenderer->BeginFill3DLine( dynaVertexCount );

							R3D_DEBUG_VERTEX vtx[ 3 ] = { };

							memset( vtx, 0, sizeof vtx );

							vtx[ 0 ].color = r3dColor( 255, 0, 255 );
							vtx[ 1 ].color = r3dColor( 255, 0, 255 );
							vtx[ 2 ].color = r3dColor( 255, 0, 255 );

							for( int i = 0, e = (int)meshGeom.convexMesh->getNbPolygons(); i < e; i ++ )
							{
								PxHullPolygon data;
								meshGeom.convexMesh->getPolygonData( i, data );

								if( r3dPoint3D( data.mPlane[ 0 ], data.mPlane[ 1 ], data.mPlane[ 2 ] ).Dot( gCam.vPointTo ) >= 0.f )
									continue;

								for( int v = 0, e = (int)data.mNbVerts; v < e; v ++ )
								{
									const PxVec3& vert0 = vertices[ ibuffer[ data.mIndexBase + v ] ];
									const PxVec3& vert1 = vertices[ ibuffer[ data.mIndexBase + ( v + 1 >= e ?  0 : v + 1 ) ] ];

									vtx[ 0 ].Pos = r3dPoint3D( vert0.x, vert0.y, vert0.z );
									vtx[ 1 ].Pos = r3dPoint3D( vert1.x, vert1.y, vert1.z );

									vtx[ 0 ].Pos += centre;
									vtx[ 1 ].Pos += centre;

									r3dRenderer->Fill3DLine( 2, vtx );
								}
							}

							r3dRenderer->EndFill3DLine();
							r3dRenderer->Flush();
						}
						break;
					case PxGeometryType::eSPHERE:
						{
							float4 color( 0, 1, 0, 1 );

							D3D_V ( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)&color, 1 ) );

							r3dRenderer->SetPixelShader( g_FwdColorPS );
							r3dRenderer->SetVertexShader( g_FwdColorVS );

							PxSphereGeometry sphereGeom;
							shapes[ 0 ]->getSphereGeometry( sphereGeom );
							
							r3dDrawGeoSpheresStart();

							D3DXMATRIX scale;
							D3DXMatrixScaling( &scale, sphereGeom.radius, sphereGeom.radius, sphereGeom.radius );

							D3DXMATRIX modTransform = mTransform;

							modTransform._41 = centre.x;
							modTransform._42 = centre.y;
							modTransform._43 = centre.z;

							D3DXMATRIX scaledTransform = scale * modTransform;
							r3dMeshSetShaderConsts( scaledTransform, NULL );

							r3dDrawGeoSphere();
							r3dDrawGeoSpheresEnd();
						}
						break;
					}

					r3dRenderer->RestoreCullMode();

					D3D_V ( r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, prevFillMode ) );

					r3dRenderer->SetPixelShader();
					r3dRenderer->SetVertexShader();
				}
			}
		}
	}
}

#endif

//////////////////////////////////////////////////////////////////////////

void GameObject::PrecalculateMatrices()
{
	r3dMesh *m = GetObjectLodMesh() ;
	r3dPoint3D *scale ;
	r3dPoint2D *texcScale ;
		
	if( m )
	{
		scale = m->IsSkeletal() ? 0 : &m->unpackScale;
		texcScale = &m->texcUnpackScale ;
	}
	else
	{
		scale = 0 ;
		texcScale = 0 ;
	}

	r3dPrepareMeshShaderConsts(preparedVSConsts, GetTransformMatrix(), scale, texcScale, r3dRenderer->ViewMatrix, r3dRenderer->ViewProjMatrix, m ? &m->texc2UnpackScale : NULL);
}

void GameObject::PrecalculateMatricesIgnoreSkinning( PrecalculatedMeshShaderConsts* oConsts )
{
	r3dMesh *m = GetObjectLodMesh() ;
	r3dPoint3D *scale ;
	r3dPoint2D *texcScale ;

	if( m )
	{
		scale = &m->unpackScale;
		texcScale = &m->texcUnpackScale ;
	}
	else
	{
		scale = 0 ;
		texcScale = 0 ;
	}

	r3dPrepareMeshShaderConsts(*oConsts, GetTransformMatrix(), scale, texcScale, r3dRenderer->ViewMatrix, r3dRenderer->ViewProjMatrix, m ? &m->texc2UnpackScale : NULL);
}

void
GameObject::SetTransparentShadowCasting( bool enabled )
{
	if( enabled )
	{
		// first set flag, then provoke adding
		PrivateFlags |= PRIVFLAG_TransparentShadowCaster ;
		GameWorld().UpdateTransparentShadowCaster( this ) ;
	}
	else
	{
		// first provoke removal, then clear flag
		GameWorld().UpdateTransparentShadowCaster( this ) ;
		PrivateFlags &= ~PRIVFLAG_TransparentShadowCaster ;
	}
}

void GameObject::SetScale(const r3dPoint3D &v)
{
	vScl = v;
	ShadowExDirty = true;
	UpdateTransform();
	if (PhysicsObject)
	{
		PhysicsObject->SetScale(v);
	}
}

bool GameObject::IsOverWater(float& waterDepth)
{
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
	bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y + 0.5f, GetPosition().z), PxVec3(0, -1, 0), 500.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);

	// If can't find the ground, then the answer is indeterminable, assumption is not over water.
	if( !hitResult )
		return false;

	r3dPoint3D posForWater = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);	// This is the ground position over/underwater.
	waterDepth = getWaterDepthAtPos(posForWater);
	if( waterDepth < 0 )
		return false;
	float waterLevel = hit.impact.y + waterDepth;

	return GetPosition().y >= waterLevel;
}

ShadowExtrusionData::ShadowExtrusionData()
: MinX		( 0.f )
, MinY		( 0.f )
, MinZ		( 0.f )
, MaxX		( 0.f )
, MaxY		( 0.f )
, Extrude	( 0.f )
{
	D3DXMatrixIdentity( &ToExtrusionBox );
}

int IsParentOrEqual( const AClass* This,  const AClass* SuspectedChild )
{
	if( This == SuspectedChild )
		return 1 ;

	for( const AClass* expert = SuspectedChild ; expert != &AObject::ClassData ; expert = expert->BaseClass )
	{
		if( expert == This )
			return 1 ;
	}

	return 0 ;

}

