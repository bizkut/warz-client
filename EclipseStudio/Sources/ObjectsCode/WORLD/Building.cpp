#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "ObjectsCode\Effects/obj_ParticleSystem.H"
#include "Particle.h"

#include "Editors/LevelEditor.h"

#include "building.h"

#include "DamageLib.h"

IMPLEMENT_CLASS(obj_Building, "obj_Building", "Object");
AUTOREGISTER_CLASS(obj_Building);
obj_Building::obj_Building() 
: m_DestructionParticles( NULL )
, m_DestructionSoundID( -1 )
, m_IsSkeletonShared( 0 )
, m_DestroyedBindSkeleton( 0 )
, m_DestroyedBindSkeletonShared( 0 )
, m_DestroyedAnimationId( -1 )
{
	ObjTypeFlags |= OBJTYPE_Building;

	m_sAnimName[0] = 0;
	m_bAnimated = 0;
	m_bGlobalAnimFolder = 0;
	m_BindSkeleton = NULL;
}

obj_Building::~obj_Building()
{
	if( !m_IsSkeletonShared )
		SAFE_DELETE( m_BindSkeleton );

	if( !m_DestroyedBindSkeletonShared )
		SAFE_DELETE( m_DestroyedBindSkeleton );
}


int __RepositionObjectsOnTerrain = 0;
extern float GetTerrainFollowPosition( const r3dBoundBox& BBox);


void obj_Building::SetPosition(const r3dPoint3D& pos)
{
	parent::SetPosition( pos );
	r3dPoint3D v = pos;

	if (!MeshLOD[0]) 
		return;

#ifndef FINAL_BUILD
	if( __RepositionObjectsOnTerrain )
	{
		if( terra_IsWithinPreciseHeightmap( GetBBoxWorld() ) )
		{
			float h = GetTerrainFollowPosition(GetBBoxWorld());
			float fRes = h;
			parent::SetPosition( r3dPoint3D( GetPosition().X, fRes,GetPosition().Z) );	
		}
	}
#endif
}

#ifndef FINAL_BUILD
float obj_Building::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )	
	{
		starty += imgui_Static(scrx, starty, "Building properties");

		r3dSTLString sDir = FileName.c_str();
		int iPos1 = sDir.find_last_of('\\');
		int iPos2 = sDir.find_last_of('/');

		if ( ( iPos1 < iPos2 && iPos2 != r3dSTLString::npos ) || ( iPos1 == r3dSTLString::npos ) )
			iPos1 = iPos2;

		if ( iPos1 != r3dSTLString::npos )
			sDir.erase(iPos1 + 1,sDir.length() - iPos1 - 1 );

		r3dSTLString sDirFind = sDir + "*.sco";

		if(m_Animation.pSkeleton && MeshLOD[0]->IsSkeletal() )
		{
			int useAnim = m_bAnimated;
			starty += imgui_Checkbox(scrx, starty, "Animated", &m_bAnimated, 1);
			if(useAnim != m_bAnimated)
			{
				PropagateChange( &obj_Building::ChangeAnim, selected ) ;
			}

			if(m_bAnimated)
			{
				int check = m_bGlobalAnimFolder ;
				starty += imgui_Checkbox(scrx, starty, "Global Anim Folder", &check, 1);

				for( int i = 0, e = selected.Count() ; i < e ; i ++ )
				{
					obj_Building* bld = static_cast< obj_Building* >( selected[ i ] );
					if( bld->m_Animation.pSkeleton )
					{
						bld->m_bAnimated = 1;
					}
				}

				PropagateChange( check, &obj_Building::m_bGlobalAnimFolder, this, selected ) ;
				
				static char sAnimSelected[256] = {0};
				static float fAnimListOffset = 0;

				r3dSTLString sDirFind ;

				if( m_bGlobalAnimFolder )
				{
					// try global animation folder
					sDirFind = GLOBAL_ANIM_FOLDER "\\*.anm" ;				
				}
				else
				{
					sDirFind = sDir + "Animations\\*.anm";
				}

				r3dscpy(sAnimSelected, m_sAnimName);
				if ( imgui_FileList (scrx, starty, 360, 200, sDirFind.c_str (), sAnimSelected, &fAnimListOffset, true ) )
				{
					r3dscpy(m_sAnimName, sAnimSelected);
					PropagateChange( &obj_Building::ChangeAnim, selected ) ;
				}
				starty += 200;
			}
		}

		if( m_HitPoints > 0 )
		{
			if ( imgui_Button ( scrx, starty, 360, 20, "Destroy" ) )
			{
				PropagateChange( &obj_Building::ZeroHitpoints, selected ) ;
			}
		}
		else
		{
			if ( imgui_Button ( scrx, starty, 360, 20, "Ressurect" ) )
			{
				PropagateChange( &obj_Building::FixBuilding, selected ) ;
			}
		}

		starty += 22.f ;

		if( selected.Count() <= 1 )
		{
			if ( imgui_Button ( scrx, starty, 360, 20, m_pDamageLibEntry ? "To Destruction Params" : "Create Destruction Params" ) )
			{
				LevelEditor.ToDamageLib( GetMeshLibKey().c_str() );
			}
		}

		starty += 22.0f;

		int oldEnable = AreThumbnailsEnabled();
		int newEnable = !!oldEnable;

		starty += imgui_Checkbox( scrx, starty, 360, 22, "Enable Thumbnails", &newEnable, 1 );

		if( oldEnable != newEnable )
		{
			char msg[ 1024 ];

			sprintf( msg, "This will affect all meshes '%s' on the level. Are you sure?", MeshLOD[ 0 ]->FileName );

			if( MessageBoxA( r3dRenderer->HLibWin, msg, "WARNING", MB_YESNO ) == IDYES )
			{
				int curThumbs = !!( MeshLOD[ 0 ]->MatChunks[ 0 ].Mat->Flags & R3D_MAT_THUMBNAILS );

				if( curThumbs != newEnable )
				{
					for( int i = 0, e = NUM_LODS; i < e; i ++ )
					{
						if( r3dMesh* mesh = MeshLOD[ i ] )
						{
							if( newEnable )
							{
								mesh->EnableThumbnails();
							}
							else
							{
								mesh->DisableThumbnails();
							}
						}
					}
				}
			}
		}

		m_ThumbnailsEnabled = newEnable;
	}
	return starty - scry ;
}
#endif


#ifndef FINAL_BUILD
void
obj_Building::DrawSelected( const r3dCamera& Cam, eRenderStageID DrawState )
{
	if( m_bAnimated )
	{
		// do nothing for now..
	}
	else
	{
		MeshGameObject::DrawSelected( Cam, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.223f) ) ;
	}
}
#endif

BOOL obj_Building::OnCreate()
{
	parent::OnCreate();
	
	ChangeAnim();
	ChangeDestroyedAnim();

	return 1;
}

/*static*/
void
obj_Building::LoadSkeleton( const char* baseMeshFName, r3dSkeleton** oSkeleton, int * oIsSkeletonShared )
{
	const char* fname = baseMeshFName ;

	char skelname[MAX_PATH];
	r3dscpy(skelname, fname);
	r3dscpy(&skelname[strlen(skelname)-3], "skl");

	bool loadSkel = false ;

	r3dSkeleton* bindSkeleton = 0 ;

	if( oIsSkeletonShared )
		*oIsSkeletonShared = 0 ;

	if(r3d_access(skelname, 0) == 0)
	{
		bindSkeleton = game_new r3dSkeleton();
		bindSkeleton->LoadBinary(skelname);
	}
#ifndef FINAL_BUILD // required only in editor, to place armor in editor and for armor to automatically load default skeleton. not needed in the game
	else
	{
		bindSkeleton		= GetDefaultSkeleton( fname );

		if( oIsSkeletonShared )
			*oIsSkeletonShared	= 1 ;
	}
#endif

	*oSkeleton = bindSkeleton ;
}

void obj_Building::ReloadAnimations( const char* fname )
{
#ifndef FINAL_BUILD

	char ourPath[ MAX_PATH ];
	char ourPathFixed[ MAX_PATH ];
	char theirPathFixed[ MAX_PATH ];

	FixFileName( fname, theirPathFixed );

	GetAnimPath( ourPath, m_bGlobalAnimFolder, m_sAnimName );
	FixFileName( ourPath, ourPathFixed );

	if( !stricmp( ourPathFixed, theirPathFixed ) )
	{
		if( !m_IsSkeletonShared )
		{
			SAFE_DELETE( m_BindSkeleton );
			LoadSkeleton( Name.c_str(), &m_BindSkeleton, &m_IsSkeletonShared );
		}

		m_AnimPool.Reload( ourPathFixed );
	}
#endif
}

//------------------------------------------------------------------------


BOOL obj_Building::Load(const char* fname)
{
	if(!parent::Load(fname))
		return FALSE;
			
	// try to load default skeleton
	if( MeshLOD[0]->IsSkeletal() )
	{
		LoadSkeleton( fname, &m_BindSkeleton, &m_IsSkeletonShared ) ;

		if( m_BindSkeleton )
		{
			m_Animation.Init(m_BindSkeleton, &m_AnimPool);
			m_Animation.Update(0.0f, r3dPoint3D(0, 0, 0), mTransform);
		}
	}


	if( m_pDamageLibEntry )
	{
		ReloadDestructionAnimation();

		if( m_pDamageLibEntry->HasParicles )
		{
			char particlePath[ 512 ];
			obj_ParticleSystem::GetParticlePath( particlePath, m_pDamageLibEntry->ParticleName );
			r3dCacheParticleData( particlePath );
		}
	}
	
	return TRUE;
}

BOOL obj_Building::Update()
{
	if( IsDestroyed() && m_pDamageLibEntry )
	{
		if( m_pDamageLibEntry->HasAnimation && DestructionMeshLOD[ 0 ] && DestructionMeshLOD[ 0 ]->IsSkeletal() && m_DestroyedBindSkeleton )
			m_DestroyedAnimation.Update( r3dGetAveragedFrameTime(), r3dPoint3D(0, 0, 0), mTransform );
	}

	if( m_bAnimated || (MeshLOD[ 0 ] && MeshLOD[ 0 ]->IsSkeletal() && m_BindSkeleton) )
		m_Animation.Update(r3dGetAveragedFrameTime(), r3dPoint3D(0, 0, 0), mTransform);

	if( m_DestructionParticles )
	{
		if( !m_DestructionParticles->isActive() )
		{
			m_DestructionParticles = 0 ;
		}
	}

	if( m_bAnimated )
	{
		ObjFlags |= OBJFLAG_AlwaysUpdate;
	}

	return parent::Update();
}

//------------------------------------------------------------------------

void obj_Building::GetAnimPath( char* oPath, int isGlobal, const char* animName )
{
	char tempPath[ MAX_PATH ];

	if( isGlobal )
	{
		// try global folder
		sprintf( tempPath, GLOBAL_ANIM_FOLDER "\\%s", animName ) ;
	}
	else
	{
		FixFileName(FileName.c_str(), tempPath);
		char* p = strrchr(tempPath, '/');
		r3d_assert(p);
		sprintf( p, "\\Animations\\%s", animName );
	}

	FixFileName( tempPath, oPath );
}

//------------------------------------------------------------------------

void obj_Building::ChangeAnim()
{
	if(!m_Animation.pSkeleton)
		return;
		
	m_Animation.StopAll();
	if(!m_bAnimated)
		return;
		
	if(m_sAnimName[0] == 0)
		return;

	char animname[MAX_PATH];	

	GetAnimPath( animname, m_bGlobalAnimFolder, m_sAnimName );

	if(r3d_access(animname, 0) != 0)
	{
		m_sAnimName[0] = 0;
		m_bAnimated = 0;
		return;
	}
		
	int aid = m_AnimPool.Add(m_sAnimName, animname);
	if(aid == -1)
	{
		m_sAnimName[0] = 0;
		m_bAnimated = 0;
		return;
	}
	
	m_Animation.StartAnimation(aid, ANIMFLAG_Looped, 0.0f, 0.0f, 0.0f);
}

void
obj_Building::ChangeDestroyedAnim()
{
	if( !m_pDamageLibEntry || !m_pDamageLibEntry->HasAnimation )
		return;

	if( !m_DestroyedAnimation.pSkeleton )
		return;

	m_DestroyedAnimation.StopAll();

	if( m_pDamageLibEntry->AnimName[0] == 0)
		return;

	char animname[ MAX_PATH ];	

	GetAnimPath( animname, m_DestroyedBindSkeletonShared, m_pDamageLibEntry->AnimName );

	if( r3d_access(animname, 0) != 0 )
	{
		return;
	}

	m_DestroyedAnimationId = m_AnimPool.Add(m_pDamageLibEntry->AnimName, animname);
}

bool obj_Building::CanBeDestroyed()
{
	return !!m_pDamageLibEntry && m_HitPoints;
}

void
obj_Building::DestroyBuilding()
{
	this->m_HitPoints = 0 ;

	if( m_pDamageLibEntry )
	{
		if( m_pDamageLibEntry->HasParicles && !m_DestructionParticles )
		{
			m_DestructionParticles = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", m_pDamageLibEntry->ParticleName.c_str(), GetPosition() );
		}
		else
		{
			if( m_DestructionParticles && m_DestructionParticles->isActive() )
				m_DestructionParticles->Restart();
		}

		if( m_pDamageLibEntry->HasSound && m_DestructionSoundID != -1 )
		{
			SoundSys.Play( m_DestructionSoundID, GetPosition() );
		}

		if( m_DestroyedAnimationId != -1 )
		{
			ObjFlags |= OBJFLAG_AlwaysUpdate;

			m_DestroyedAnimation.StartAnimation( m_DestroyedAnimationId, ANIMFLAG_PauseOnEnd, 0.0f, 0.0f, 0.0f );
			m_DestroyedAnimation.Update( 0.f, r3dPoint3D( 0.f, 0.f, 0.f), mTransform );
		}

		if( m_pDamageLibEntry->DestroyedMeshCollision )
		{
			SAFE_DELETE( PhysicsObject );

			PhysicsConfig.isDestroyed = true;

			char meshPath[ 1024 ] = "Data/ObjectsDepot/";
			strcat( meshPath, m_pDamageLibEntry->MeshName.c_str() );

			PhysicsConfig.SetDestroyedMesh( meshPath );

			CreatePhysicsData();
		}
	}
}

void obj_Building::ZeroHitpoints()
{
	m_HitPoints = -11;
}

void obj_Building::FixBuilding()
{
	m_HitPoints = 0XA107;

	if( m_pDamageLibEntry )
	{
		m_HitPoints = m_pDamageLibEntry->HitPoints ;
		m_IsDestroyed = 0;

		if( m_pDamageLibEntry->DestroyedMeshCollision )
		{
			SAFE_DELETE( PhysicsObject );
			PhysicsConfig.isDestroyed = false;
			CreatePhysicsData();
		}
	}

	if( m_DestructionParticles )
	{
		GameWorld().DeleteObject( m_DestructionParticles );
		m_DestructionParticles = 0 ;
	}
}

GameObject *obj_Building::Clone ()
{
	obj_Building * pNew = (obj_Building*)srv_CreateGameObject("obj_Building", FileName.c_str(), GetPosition () );
	
	PostCloneParamsCopy(pNew);

	return pNew;
}

//////////////////////////////////////////////////////////////////////////

void obj_Building::PostCloneParamsCopy(GameObject *pNewObj)
{
	GameObject::PostCloneParamsCopy(pNewObj);

	obj_Building *pNew = static_cast<obj_Building*>(pNewObj);

	if (!pNew) return;
	pNew->SetRotationVector(GetRotationVector());
	pNew->SetScale(GetScale());
	pNew->m_ObjectColor = m_ObjectColor;
	pNew->m_bEnablePhysics = m_bEnablePhysics;
	pNew->ObjFlags |= ObjFlags;

	r3dscpy(pNew->m_sAnimName, m_sAnimName);
	pNew->m_bAnimated = m_bAnimated;
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_Building::GetObjStat ( char * sStr, int iLen)
{
	char sAddStr [MAX_PATH];
	sAddStr[0] = 0;
	if (MeshGameObject::GetObjStat (sStr, iLen))
		r3dscpy(sAddStr, sStr);

	return TRUE;
}

void obj_Building::PreLoadReadSerializedData(pugi::xml_node& node)
{
	parent::PreLoadReadSerializedData(node);
	pugi::xml_node buildingNode = node.child("building");

	pugi::xml_attribute enableThumbs = buildingNode.attribute( "enable_thumbnails" );

	if( enableThumbs.empty() )
	{
		m_ThumbnailsEnabled = 1;
	}
	else
	{
		m_ThumbnailsEnabled = enableThumbs.as_int();
	}
}

void obj_Building::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node buildingNode = node.child("building");
	
	r3dscpy(m_sAnimName, buildingNode.attribute("AnimName").value());
	m_bAnimated = buildingNode.attribute("Animated").as_int();

	pugi::xml_attribute gloanfol = buildingNode.attribute("GlobalAnimFolder");

	if( !gloanfol.empty() )
	{
		m_bGlobalAnimFolder = !!gloanfol.as_int();
	}
}

void obj_Building::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	pugi::xml_node buildingNode = node.append_child();
	buildingNode.set_name("building");
	
	if(m_sAnimName[0])
	{
		buildingNode.append_attribute("AnimName") = m_sAnimName;
		buildingNode.append_attribute("Animated") = m_bAnimated;

		// don't spam it..
		if( m_bGlobalAnimFolder )
		{
			buildingNode.append_attribute("GlobalAnimFolder") = m_bGlobalAnimFolder;
		}
	}

	int thumbsEnabled = AreThumbnailsEnabled();
	if( !thumbsEnabled )
	{
		buildingNode.append_attribute( "enable_thumbnails" ) = 0;
	}
}

//------------------------------------------------------------------------

void obj_Building::ReloadDestructionAnimation()
{
	if( m_pDamageLibEntry && m_pDamageLibEntry->HasAnimation && m_pDamageLibEntry->UseDestructionMesh )
	{
		if( m_DestroyedBindSkeletonShared )
			SAFE_DELETE( m_DestroyedBindSkeleton );

		LoadSkeleton( ( r3dString( "Data\\ObjectsDepot\\" ) + m_pDamageLibEntry->MeshName.GetDataPtr() ).c_str(), &m_DestroyedBindSkeleton, &m_DestroyedBindSkeletonShared );

		if( m_DestroyedBindSkeleton )
		{
			m_DestroyedAnimation.Init( m_DestroyedBindSkeleton, &m_AnimPool );
			m_DestroyedAnimation.Update( 0.0f, r3dPoint3D(0, 0, 0), mTransform );

			ChangeDestroyedAnim();
		}
	}
}


//------------------------------------------------------------------------
/*virtual*/
void
obj_Building::UpdateDestructionData()
{
	MeshGameObject::UpdateDestructionData();

	if( !m_pDamageLibEntry )
	{
		if( m_DestructionParticles )
		{
			GameWorld().DeleteObject( m_DestructionParticles );
			m_DestructionParticles = NULL ;
		}

		m_DestructionSoundID = -1 ;
	}
	else
	{
		if( !m_pDamageLibEntry->HasParicles  )
		{
			GameWorld().DeleteObject( m_DestructionParticles );
			m_DestructionParticles = NULL ;
		}

		if( !m_pDamageLibEntry->HasSound )
		{
			m_DestructionSoundID = -1 ;
		}
		else
		{
			m_DestructionSoundID = SoundSys.GetEventIDByPath( m_pDamageLibEntry->SoundName.c_str() );
		}
	 }

	ReloadDestructionAnimation();
}

int obj_Building::IsStatic()
{
	return true;
}

/*virtual*/ void obj_Building::OnMeshDestroyed() /*OVERRIDE*/
{
	DestroyBuilding();
}

bool obj_Building::NeedDrawAnimated(const r3dCamera& Cam) 
{
	float distSq = (Cam - GetPosition()).LengthSq();

	if( IsDestroyed() && m_pDamageLibEntry )
	{
		if( m_pDamageLibEntry->HasAnimation )
		{
			float distSq = (Cam - GetPosition()).LengthSq();
			int meshLodIndex = ChoseMeshLOD( distSq, DestructionMeshLOD );

			if( DestructionMeshLOD[ meshLodIndex ]->IsSkeletal() && m_DestroyedBindSkeleton )
				return true;
			else
				return false;
		}

		return false;
	}
	else
	{
		float distSq = (Cam - GetPosition()).LengthSq();
		int meshLodIndex = ChoseMeshLOD( distSq, MeshLOD );

		if( MeshLOD[ meshLodIndex ]->IsSkeletal() && m_BindSkeleton )
			return true ;

		if(!m_bAnimated)
			return false;

		return true;
	}

	return false;
}

void obj_Building::DrawAnimated(const r3dCamera& Cam, bool shadow_pass)
{
	r3dAnimation* targetAnimation = NULL;

	if( IsDestroyed() )
	{
		targetAnimation = &m_DestroyedAnimation;
	}
	else
	{
		targetAnimation = &m_Animation;
	}
	r3d_assert( targetAnimation->pSkeleton );

	// recalc animation if it's dirty
	targetAnimation->Recalc();
	
	int oldVsId = r3dRenderer->GetCurrentVertexShaderIdx();

	targetAnimation->pSkeleton->SetShaderConstants();

	r3dMesh* ( &TargetLODSet ) [ NUM_LODS ] = GetPreferredMeshLODSet();

	r3dMesh* mesh = TargetLODSet[0];

	if( !mesh )
		return;

	r3d_assert( mesh->IsSkeletal() );

	D3DXMATRIX world;
	r3dPoint3D pos = GetPosition();
	D3DXMatrixTranslation(&world, pos.x, pos.y, pos.z );
	r3dMeshSetShaderConsts( world, NULL );
	
	if(!shadow_pass)
	{
		mesh->DrawMeshDeferred( m_ObjectColor, 0 );
	}
	else
	{
		mesh->DrawMeshShadows();
	}
	
	r3dRenderer->SetVertexShader(oldVsId);
}

struct BuildingAniDeferredRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		BuildingAniDeferredRenderable* This = static_cast<BuildingAniDeferredRenderable*>( RThis );
		This->Parent->DrawAnimated(Cam, false);
	}

	obj_Building* Parent;
};

struct BuildingAniShadowRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		BuildingAniShadowRenderable* This = static_cast<BuildingAniShadowRenderable*>( RThis );
		This->Parent->DrawAnimated(Cam, true);
	}

	obj_Building* Parent;
};

struct BuildingAniDebugRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		BuildingAniDebugRenderable* This = static_cast<BuildingAniDebugRenderable*>( RThis );
		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

		This->Parent->m_Animation.pSkeleton->DrawSkeleton(Cam, This->Parent->GetPosition());
	}

	obj_Building* Parent;
};

#define	RENDERABLE_BUILDING_SORT_VALUE (31*RENDERABLE_USER_SORT_VALUE)

void obj_Building::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam )
{
	// [from MeshGameObject code] always use main camera to determine shadow LOD
	if(!NeedDrawAnimated(gCam))
	{
		parent::AppendShadowRenderables(rarr, sliceIndex, Cam);
		return;
	}

	if( !gDisableDynamicObjectShadows && sliceIndex != r_active_shadow_slices->GetInt() - 1 )
	{
		BuildingAniShadowRenderable rend;
		rend.Init();
		rend.Parent	= this;
		rend.SortValue	= RENDERABLE_BUILDING_SORT_VALUE;

		rarr.PushBack( rend );
	}
}



bool obj_Building::IsInstancingEligible( const r3dCamera& Cam )  
{
	// FIXME: 
	return !NeedDrawAnimated(Cam);
}

bool obj_Building::IsInstancedRendered() const 
{
	return parent::IsInstancedRendered();
}

void obj_Building::SetInstancingFlag( const r3dCamera& Cam ) 
{
	parent::SetInstancingFlag(Cam);	
}

void obj_Building::AppendRenderablesInstanced( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	parent::AppendRenderablesInstanced(render_arrays, Cam);
}


void obj_Building::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	// FIXME: We have NeedDrawAnimated in IsInstancingEligible
	if(!NeedDrawAnimated(Cam))
	{
		parent::AppendRenderables(render_arrays, Cam);
		return;
	}

	// deferred
	{
		BuildingAniDeferredRenderable rend;
		rend.Init();
		rend.Parent	= this;
		rend.SortValue	= RENDERABLE_BUILDING_SORT_VALUE;

		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}

	/*// skeleton draw
	{
		BuildingAniDebugRenderable rend;
		rend.Init();
		rend.Parent	= this;
		rend.SortValue	= RENDERABLE_BUILDING_SORT_VALUE;

		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}*/
}


