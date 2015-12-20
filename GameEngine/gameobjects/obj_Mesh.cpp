#include "r3dPCH.h"
#include "r3d.h"

#include "r3dBackgroundTaskDispatcher.h"

#include "GameObj.h"
#include "obj_Mesh.h"
#include "GameCommon.h"

#include "DamageLib.h"
#include "MeshPropertyLib.h"

#include "../../Eternity/Include/r3dDeviceQueue.h"

extern bool g_bEditMode;
extern int g_bForceQualitySelectionInEditor;

int		g_DrawCollisionMeshes = 0;
int		g_DrawPlayerOnlyCollisionMeshes = 0;

// static to prevent extern
static r3dgameVector(r3dMesh*) s_MeshCache;

#define COMMON_DEPOT_PREFIX "data/objectsdepot/"

static bool FillDestructionMeshName( char (&oName) [ MAX_PATH ], DamageLibEntry* dlibEntry );

extern CRITICAL_SECTION g_ResourceCritSection ;

typedef MeshDeferredRenderable MeshDeffRenderableBase ;
typedef MeshShadowRenderable MeshShadowRenderableBase ;

//--------------------------------------------------------------------------------------------------------

struct CacheMeshParams
{
	r3dString fname;
	r3dMesh *mesh;
	bool delParams;
	bool use_default_material;
	bool allow_async;
	bool use_thumbnails;
};

void CacheMeshFunc(void *params)
{
	CacheMeshParams *p = reinterpret_cast<CacheMeshParams*>(params);
	r3dMesh *pMesh = p->mesh;
	const char *fname = p->fname.c_str();

	pMesh->Load(fname, p->use_default_material, !p->allow_async, p->use_thumbnails );

	if( p->allow_async )
	{
		pMesh->FillBuffersAsync() ;
	}
	else
	{
		pMesh->FillBuffers();
	}

	if (p->delParams)
		delete p;
}

//--------------------------------------------------------------------------------------------------------
r3dMesh* CacheMesh( const char* fname, bool use_default_material, bool allow_async, bool player_mesh, bool use_thumbnails )
{
	R3DPROFILE_FUNCTION("CacheMesh");
	r3dMesh * pMesh = game_new r3dMesh( 0 );

	if( player_mesh )
	{
		pMesh->SetFlag( r3dMesh::obfPlayerMesh, TRUE ) ;
	}

	CacheMeshParams params;
	params.fname = fname;
	params.mesh = pMesh;
	params.delParams = false;
	params.use_default_material = use_default_material;
	params.allow_async = allow_async;
	params.use_thumbnails = use_thumbnails;

	CacheMeshFunc(&params);
	if(!allow_async && !pMesh->IsLoaded())
	{
		delete pMesh;
		return NULL;
	}

	// can be safe deleted
	pMesh = params.mesh ;

	return pMesh;
}

//------------------------------------------------------------------------

void ReloadObjectAnim( const char* fname )
{
	char szFixedName[MAX_PATH];	
	FixFileName(fname, szFixedName);

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* pObj = iter.current;

		if ( ! pObj->isObjType( OBJTYPE_Building ) )
			continue;

		obj_Building* bld = (obj_Building *)pObj;
#ifndef WO_SERVER
		bld->ReloadAnimations( fname );
#endif
	}
}

//--------------------------------------------------------------------------------------------------------
void ReloadMesh(const char* fname)
{


	char szFixedName[MAX_PATH];	
	FixFileName(fname, szFixedName);

	for (size_t i=0;i<s_MeshCache.size();++i)
	{
		if ( !r3dIsSamePath(s_MeshCache[i]->FileName.c_str(), szFixedName) ) 
			continue;

		r3dMesh* pFreeMesh = s_MeshCache[i];

		bool was_player = pFreeMesh->Flags & r3dMesh::obfPlayerMesh ? true : false ;

		SAFE_DELETE( s_MeshCache[i] )
		s_MeshCache[i] = CacheMesh( szFixedName, false, false, was_player, false );

		for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
		{
			GameObject* pObj = iter.current;

			if ( ! pObj->isObjType(OBJTYPE_Mesh) )
				continue;
			 ((MeshGameObject *)pObj)->ReloadMeshData( pFreeMesh );
		}
		break;
	}
}


//--------------------------------------------------------------------------------------------------------
r3dMesh *r3dGOBAddMesh(const char* fname, bool addToLibrary, bool use_default_material, bool allow_async, bool player_mesh, bool use_thumbnails )
{
	char szFixedName[MAX_PATH];	
	FixFileName(fname, szFixedName);

	for (size_t i=0;i<s_MeshCache.size();++i)
	{
		r3dMesh* mesh = s_MeshCache[i] ;
		if( mesh->FileName == szFixedName ) 
		{
			if( !allow_async && !mesh->IsDrawablePure() )
			{
				r3dOutToLog( "r3dGOBAddMesh: Lazy mesh '%s' encountered on sync loading request. Waiting for completion\n", mesh->FileName.c_str() ) ;
				for( ; !mesh->IsDrawablePure(); )
				{
					ProcessDeviceQueue( r3dGetTime(), 0.067f );
					Sleep( 1 );
				}
			}

			mesh->RefCount ++ ;

			return mesh ;
		}
	}

	if(s_MeshCache.size() > 2048)
	{
		r3dOutToLog("MESH:  Mesh Cache is too big!\n" );
	}

	r3dMesh* loadedMesh = CacheMesh( szFixedName, use_default_material, allow_async, player_mesh, use_thumbnails );
	if( loadedMesh )
	{
		r3d_assert( 0 == loadedMesh->RefCount ) ;
		loadedMesh->RefCount = 1 ;

		if( use_thumbnails && !r_sector_loading->GetInt() )
		{
			loadedMesh->TouchMaterials();
		}

		if(addToLibrary)
			s_MeshCache.push_back(loadedMesh);
		return loadedMesh;
	}

	return 0;
}

static r3dTL::TArray< r3dMesh* > g_ReleasedMeshes ;

void r3dGOBReleaseMesh( r3dMesh* mesh )
{
	if( r_allow_ingame_unloading->GetInt() )
	{
		g_ReleasedMeshes.PushBack( mesh ) ;
	}
}

void r3dResetCachedMeshSectorReferences()
{
	r3dgameVector(r3dMesh*)::iterator it;

	for(it = s_MeshCache.begin(); it!=s_MeshCache.end(); ++it)
	{
		(*it)->SectorRefCount = 0;
	}
}

static void DoReleaseMesh( r3dMesh* mesh )
{
	r3dgameVector(r3dMesh*)::iterator it;
	for(it = s_MeshCache.begin(); it!=s_MeshCache.end(); ++it)
	{
		if(*it==mesh)
		{
			break ;
		}
	}

	if(it==s_MeshCache.end())
	{
		r3dOutToLog( "r3dGOBReleaseMesh: can't find mesh %s(%p) in cache\n", mesh->Name, mesh ) ;
		return ;
	}

	mesh->RefCount -- ;

	if( mesh->RefCount <= 0 )
	{
		if( mesh->RefCount < 0 )
		{
			r3dOutToLog( "r3dGOBReleaseMesh: negative reference count on mesh %s(%p)\n", mesh->Name, mesh ) ;
		}

		r3dOutToLog( "Unloading mesh %s\n", mesh->Name ) ;
		s_MeshCache.erase(it); // firstly remove from array, and only then delete pointer
		SAFE_DELETE( mesh ) ;

#ifndef FINAL_BUILD
		int playerBuff = r3dRenderer->Stats.GetPlayerBufferMem() ;
		int playerTex = r3dRenderer->Stats.GetPlayerTexMem() ;
		r3dOutToLog( "Player memory: %.2f + %.2f = %.2fMB\n",	playerBuff / 1024.f / 1024.f, 
																playerTex / 1024.f / 1024.f, 
																( playerBuff + playerTex ) / 1024.f / 1024.f ) ;
#endif

	}
}

void DoProcessReleasedMeshes( int allowStayCount, int allowWait )
{
	if( !TryEnterCriticalSection( &g_ResourceCritSection ) )
	{
		if( !allowWait )
		{
			// do it some other time
			return ;
		}
		else
		{
			for( ; !TryEnterCriticalSection( &g_ResourceCritSection ) ; )
			{
				ProcessDeviceQueue( r3dGetTime(), 0.033f ) ;
				Sleep( 1 ) ;
			}
		}
	}

	int toRelease = g_ReleasedMeshes.Count() - allowStayCount ;

	for( int i = 0 ; i < toRelease ; i ++ )
	{
		DoReleaseMesh( g_ReleasedMeshes[ i ] ) ;
	}

	if( toRelease > 0 )
	{
		g_ReleasedMeshes.Erase( 0, toRelease ) ;
	}

	LeaveCriticalSection( &g_ResourceCritSection ) ;
}

void r3dProcessReleasedMeshes()
{
	DoProcessReleasedMeshes( 32, 0 ) ;
}

void r3dFlushReleasedMeshes()
{
	DoProcessReleasedMeshes( 0, 1 ) ;
}

void r3dUpdateMeshMaterials()
{
	for (size_t i=0;i<s_MeshCache.size(); ++i)
	{
		s_MeshCache[i]->FindAlphaTextures();
	}
}

void r3dFreeGOBMeshes()
{
	for (size_t i=0;i<s_MeshCache.size();++i)
	{
		SAFE_DELETE(s_MeshCache[i]);
	}
	s_MeshCache.clear();

	g_ReleasedMeshes.Clear() ;
}

IMPLEMENT_CLASS(MeshGameObject, "MeshGameObject", "Object");
AUTOREGISTER_CLASS(MeshGameObject);

#include "ObjectRegistration.h"

//////////////////////////////////////////////////////////////////////////

MeshGameObject::MeshGameObject()
: m_ThumbnailsEnabled( 1 )
, m_AllowAsyncLoading( 0 )
, m_IsDestroyed( 0 )
{
	ObjTypeFlags |= OBJTYPE_Mesh;
	ObjFlags |= OBJFLAG_AsyncLoading;

	CustomLODDistances = 0;

	for( size_t i = 0, e = R3D_ARRAYSIZE(MeshLOD); i < e; i ++ )
	{
		MeshLOD[ i ] = NULL;
	}

	for( size_t i = 0, e = R3D_ARRAYSIZE(DestructionMeshLOD); i < e; i ++ )
	{
		DestructionMeshLOD[ i ] = NULL;
	}

	PlayerOnly_CollisionMesh = NULL;

	for( size_t i = 0, e = R3D_ARRAYSIZE(MeshLODDistSq); i < e; i ++ )
	{
		MeshLODDistSq[ i ] = 0;
	}

	DrawOrder	= OBJ_DRAWORDER_NORMAL;
	CompoundID	= 0;
	CompoundOffset = r3dPoint3D(0,0,0);
	m_ObjectColor = r3dColor24::white;
	m_HitPoints = 0XA107;
	m_pDamageLibEntry = NULL;

	m_FillGBufferTarget = rsFillGBuffer;
	currentInstancingMesh = 0;
}

//--------------------------------------------------------------------------------------------------------
MeshGameObject::~MeshGameObject()
{
	
}

static R3D_FORCEINLINE float OptimalDistance(float boundSphereRadius)
{
	return boundSphereRadius / ((gCam.FOV * D3DX_PI) / 180.0f);
}

static R3D_FORCEINLINE float CalculateLodMeasure(float boundSphereRadius, float dist)
{
	return OptimalDistance(boundSphereRadius) / dist;
}

//------------------------------------------------------------------------

int	MeshGameObject::ChoseMeshLOD( float distSq, r3dMesh* (&TargetLOD)[ NUM_LODS ] )
{
	int maxLOD = r_max_mesh_lod->GetInt();

	int meshLodIndex = 0;

	const r3dBoundBox& bblocal = GetBBoxLocal();

	float currentMeasure;

	float MeshLODMeasure[3] = { 0.2f, 0.1f, 0.05f };

	if( CustomLODDistances )
	{
		const int ARR_SIZE = R3D_ARRAYSIZE( MeshLODDistSq );

		COMPILE_ASSERT( R3D_ARRAYSIZE( MeshLODMeasure ) == ARR_SIZE );

		for( int i = 0, e = R3D_ARRAYSIZE(MeshLODMeasure); i < e; i ++ )
		{
			MeshLODMeasure[ i ] = MeshLODDistSq[ i ];
		}

		currentMeasure = distSq;

		if( ( currentMeasure >= MeshLODMeasure[0] || maxLOD > 0 ) && TargetLOD[1] )
			meshLodIndex = 1;					
		if( ( currentMeasure >= MeshLODMeasure[1] || maxLOD > 1 ) && TargetLOD[2] )
			meshLodIndex = 2;					
		if( ( currentMeasure >= MeshLODMeasure[2] || maxLOD > 2 ) && TargetLOD[3] )
			meshLodIndex = 3;
	}
	else
	{
		float radius = R3D_MAX(R3D_MAX(bblocal.Size.x, bblocal.Size.y), bblocal.Size.z) * 0.5f;
		currentMeasure = CalculateLodMeasure(radius, sqrtf(distSq));

		if( ( currentMeasure < MeshLODMeasure[0] || maxLOD > 0 ) && TargetLOD[1] )
			meshLodIndex = 1;					
		if( ( currentMeasure < MeshLODMeasure[1] || maxLOD > 1 ) && TargetLOD[2] )
			meshLodIndex = 2;					
		if( ( currentMeasure < MeshLODMeasure[2] || maxLOD > 2 ) && TargetLOD[3] )
			meshLodIndex = 3;
	}

	return meshLodIndex;

}

//------------------------------------------------------------------------

inline
int
MeshGameObject::ChoseMeshLOD( float distSq )  
{
	return ChoseMeshLOD( distSq, GetPreferredMeshLODSet() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int
MeshGameObject::ChooseMainCameraLOD()
{
	float distSq = (gCam - GetPosition()).LengthSq();
	return ChoseMeshLOD( distSq ) ;
}

//------------------------------------------------------------------------

void
MeshGameObject::ReloadDestructionMesh( const char* nameOfChangedMesh )
{	
	char szFileName [ MAX_PATH ] ;
	if( FillDestructionMeshName ( szFileName, m_pDamageLibEntry ) )
	{
		if( !DestructionMeshLOD[ 0 ] || stricmp( DestructionMeshLOD[ 0 ]->FileName.c_str(), nameOfChangedMesh ) )
		{
			DoLoad( DestructionMeshLOD, szFileName, false );
		}

		if( m_pDamageLibEntry->DestroyedMeshCollision )
		{
			char fullPath[ 1024 ] = "Data/ObjectsDepot/";
			strcat( fullPath, nameOfChangedMesh );

			PhysicsConfig.SetDestroyedMesh( fullPath );
		}
	}
}

//------------------------------------------------------------------------

void
MeshGameObject::RemoveDestructionMesh()
{
	for( size_t i = 0, e = R3D_ARRAYSIZE( DestructionMeshLOD ); i < e; i ++ )
	{
		DestructionMeshLOD[ i ] = 0 ;
	}

	free( PhysicsConfig.destroyedMeshFilename );
	PhysicsConfig.destroyedMeshFilename = NULL;
}

//------------------------------------------------------------------------

void
MeshGameObject::UpdateDestructionData()
{
	if( g_DamageLib )
		m_pDamageLibEntry = g_DamageLib->GetEntry( GetMeshLibKey() );

	if( m_pDamageLibEntry && m_pDamageLibEntry->MeshName[ 0 ] && m_pDamageLibEntry->UseDestructionMesh )
	{
		ReloadDestructionMesh( m_pDamageLibEntry->MeshName.c_str() );
	}
	else
	{
		RemoveDestructionMesh();
	}
}

//------------------------------------------------------------------------

r3dSTLString
MeshGameObject::GetMeshLibKey() const
{
	const char* DestructFileName = 0 ;

	r3dMesh* srcMesh = MeshLOD[ 0 ] ;
	if(srcMesh == NULL)
		return "";

	char* tempFName = (char*)_alloca( srcMesh->FileName.Length() + 1 );
	r3d_assert( tempFName );

	strcpy( tempFName, srcMesh->FileName.c_str() );
	strlwr( tempFName );

	if( const char* start = strstr( tempFName, COMMON_DEPOT_PREFIX ) )
	{
		start += sizeof COMMON_DEPOT_PREFIX - 1 ;

		char Dir[ 256], File[ 256 ], Ext[ 16 ];

		_splitpath( start, NULL, Dir, File, Ext );

		size_t dirslen = strlen( Dir ) ;

		if( Dir[ dirslen - 1 ] == '/' || Dir[ dirslen - 1 ] == '\\' )
		{
			Dir[ dirslen - 1 ] = 0 ;
		}

		return g_DamageLib->ComposeKey( Dir, r3dSTLString( File ) + Ext );
	}

	return "" ;

}

//------------------------------------------------------------------------

/*virtual*/
r3dMesh*
MeshGameObject::GetObjectLodMesh()
{
	r3dMesh* ( &TargetLODSet ) [ NUM_LODS ] = GetPreferredMeshLODSet();
	return TargetLODSet[ ChooseMainCameraLOD() ] ;
}

//--------------------------------------------------------------------------------------------------------

void MeshGameObject::ReloadMeshData( r3dMesh * pFreeMesh )
{
	bool bFound = false;

	for ( int i = 0; i < NUM_LODS; i++ )
	{
		if(		
				MeshLOD[ i ] != pFreeMesh 
					&&
				DestructionMeshLOD[ i ] != pFreeMesh
				)
			continue;
		
		bFound = true;
		break;
	}

	if ( ! bFound )
		return;

	this->bLoaded = false;

	Load( FileName.c_str() );

	bLoaded = UpdateLoading();

	while( !this->bLoaded )
	{
		ProcessDeviceQueue( r3dGetTime(), 1.0f );
	}
}

//-----------------------------------------------------------------------

static bool FillDestructionMeshName( char (&oName) [ MAX_PATH ], DamageLibEntry* dlibEntry )
{
	if( dlibEntry && dlibEntry->MeshName[ 0 ] && dlibEntry->UseDestructionMesh )
	{
		char FullName[ MAX_PATH ] ;
		FullName[ MAX_PATH - 1 ] = 0 ;

		_snprintf( FullName, MAX_PATH - 1, "%s%s", COMMON_DEPOT_PREFIX, dlibEntry->MeshName.c_str() );
		FixFileName( FullName, oName );

		return true ;
	}

	return false;
}

void MeshGameObject::UpdateLODDistances( MeshPropertyLibEntry * entry )
{
	CustomLODDistances = 0;

	for( int i = 0, e = (int)entry->LODDistances.COUNT; i < e; i ++ )
	{
		if( entry->LODDistances[ i ] )
		{
			CustomLODDistances = 1;
		}
	}

	if( CustomLODDistances )
	{
		COMPILE_ASSERT( R3D_ARRAYSIZE( entry->LODDistances ) == R3D_ARRAYSIZE( MeshLODDistSq ) );

		for( int i = 0, e = (int)entry->LODDistances.COUNT; i < e; i ++ )
		{
			MeshLODDistSq[ i ] = entry->LODDistances[ i ] * entry->LODDistances[ i ];
		}

		for( int i = 1, e = (int)entry->LODDistances.COUNT; i < e; i ++ )
		{
			MeshLODDistSq[ i ] = R3D_MAX( MeshLODDistSq[ i ], MeshLODDistSq[ i - 1 ] + 1.0f );
		}
	}
}

BOOL MeshGameObject::Load(const char* fname)
{
	char *tempFName = (char*)_alloca( strlen( fname ) + 1 );

	r3d_assert( tempFName );

	strcpy( tempFName, fname );
	strlwr( tempFName );

	char szFixedName[MAX_PATH];	
	FixFileName(fname, szFixedName);

	if(!GameObject::Load(szFixedName))
		return FALSE;

	if(!DoLoad( MeshLOD, szFixedName, true ))
		return FALSE;

	if( g_DamageLib )
		m_pDamageLibEntry = g_DamageLib->GetEntry( GetMeshLibKey() );

	DrawDistanceSq = 0.f;

	if( g_MeshPropertyLib )
	{
		if( MeshPropertyLibEntry* entry = g_MeshPropertyLib->GetEntry( GetMeshLibKey() ) )
		{
			DrawDistanceSq = entry->DrawDistance * entry->DrawDistance;

			UpdateLODDistances( entry );
		}
	}

	if( FillDestructionMeshName ( szFixedName, m_pDamageLibEntry ) )
	{
		DoLoad( DestructionMeshLOD, szFixedName, false );
	}
	
	r3dBoundBox bboxLocal ;

	bboxLocal.Org	= r3dPoint3D( -0.5, -0.5, -0.5 ) ;
	bboxLocal.Size	= r3dPoint3D( +0.5, +0.5, +0.5 ) ;

	SetBBoxLocal( bboxLocal ) ;

	return TRUE;
}

BOOL MeshGameObject::UpdateLoading() 
{
	for( int i = 0, e = NUM_LODS ; i < e; i ++ )
	{
		if( r3dMesh* m = MeshLOD[ i ] )
		{
			if( !m->IsDrawable() )
				return false ;
		}

		if( r3dMesh* m = DestructionMeshLOD[ i ] )
		{
			if( !m->IsDrawable() )
				return false ;
		}

	}

	// update BBox after finished loading mesh (for async loading)
	if(MeshLOD[0])
		SetBBoxLocal( MeshLOD[0]->localBBox );
	OnFinishedLoadingMeshes(); // let parent classes know about that

	return true ;
}

bool MeshGameObject::AreThumbnailsEnabled() const
{
	return MeshLOD[ 0 ] ? !!(MeshLOD[ 0 ]->MatChunks[ 0 ].Mat->Flags & R3D_MAT_THUMBNAILS) : true;
}

void MeshGameObject::OnFinishedLoadingMeshes()
{
}

/*virtual*/
void MeshGameObject::OnMeshDestroyed()
{

}

#ifndef FINAL_BUILD

void MeshGameObject::UpdateShadows( const int& Shadows )
{
	if( Shadows )
		this->ObjFlags &= ~OBJFLAG_DisableShadows ; 
	else
		this->ObjFlags |= OBJFLAG_DisableShadows ;
}

float MeshGameObject::DrawDrawDistancePropertyEditor( float scrx, float scry, float* oNewDistance, int* isOtherwiseDirty )
{
	float starty = scry;

	starty += imgui_Static( scrx, starty, "Distances & LODs" );

	//scrx += 3;

	*isOtherwiseDirty = 0;

	static float DrawDistance = 0.f;
	static float LODDistances[ MeshPropertyLibEntry::LOD_COUNT ] = { 0.f };
	static MeshPropertyLibEntry* last_Entry = NULL;

	const r3dSTLString& meshLibKey = GetMeshLibKey();

	MeshPropertyLibEntry* entry = g_MeshPropertyLib->GetEntry( meshLibKey );

	if( entry )
	{
		DrawDistance = entry->DrawDistance;
	}
	else
	{
		DrawDistance = 0.0f;
	}

	int infinity = DrawDistance > 0.5 * FLT_MAX;

	int prevInfinity = infinity;
	starty += imgui_Checkbox( scrx, starty, "Infinite Draw Distance", &infinity, 1 );

	if( infinity )
	{
		DrawDistance = FLT_MAX;
	}
	else
	{
		if( prevInfinity )
			DrawDistance = 0;
		starty += imgui_Value_Slider( scrx, starty, "Draw Distance", &DrawDistance, 0.f, 4096.f, "%.0f" );
	}

	if( imgui_Button( scrx, starty, 360.f, 22.f, "Set to Camera" ) )
	{
		float distance = ( gCam - GetPosition() ).Length();

		*oNewDistance = distance;
		DrawDistance = distance;
	}

	starty += 24.f;

	if( last_Entry != entry && entry )
	{
		for( int i = 0, e = R3D_ARRAYSIZE(LODDistances); i < e; i ++ )
		{
			LODDistances[ i ] = entry->LODDistances[ i ];
		}
		last_Entry = entry;
	}

	for( int i = 0, e = R3D_ARRAYSIZE(LODDistances); i < e; i ++ )
	{
		char label[ 64 ];

		sprintf( label, "LOD %d Dist", i );

		starty += imgui_Value_Slider( scrx, starty, label, &LODDistances[ i ], i ? LODDistances[ i - 1 ] : 0.f, 2048.f, "%.2f" );
	}

	if( DrawDistance && !entry )
	{
		g_MeshPropertyLib->AddEntry( meshLibKey );
		entry = g_MeshPropertyLib->GetEntry( meshLibKey );
	}

	if( !entry )
	{
		int newGlobal = 1;

		starty += imgui_Checkbox( scrx, starty, "Distances Are Global", &newGlobal, 1 );

		if( newGlobal )
		{
			g_MeshPropertyLib->AddGlobalEntry( meshLibKey );
			MeshPropertyLibEntry* globalEntry = g_MeshPropertyLib->GetGlobalEntry( meshLibKey );

			if( globalEntry )
			{
				g_MeshPropertyLib->AddEntry( meshLibKey );

				if( MeshPropertyLibEntry* en = g_MeshPropertyLib->GetEntry( meshLibKey ) )
				{
					*en = *globalEntry;
				}
			}
		}
	}
	else
	{
		entry->DrawDistance = DrawDistance;

		for( int i = 0, e = (int)entry->LODDistances.COUNT; i < e; i ++ )
		{
			if( entry->LODDistances[ i ] != LODDistances[ i ] )
				*isOtherwiseDirty = 1;
			entry->LODDistances[ i ] = LODDistances[ i ];
		}

		int wasGlobal = entry->IsGlobal;
		starty += imgui_Checkbox( scrx, starty, "Distances Are Global", &entry->IsGlobal, 1 );

		if( !wasGlobal && entry->IsGlobal )
		{
			MeshPropertyLibEntry* globalEntry = g_MeshPropertyLib->GetGlobalEntry( meshLibKey );

			if( !globalEntry )
			{
				g_MeshPropertyLib->AddGlobalEntry( meshLibKey );
				globalEntry = g_MeshPropertyLib->GetGlobalEntry( meshLibKey );

				if( globalEntry )
				{
					*globalEntry = *entry;
				}
			}
			else
			{
				*entry = *globalEntry;

				for( int i = 0, e = (int)entry->LODDistances.COUNT; i < e; i ++ )
				{
					LODDistances[ i ] = entry->LODDistances[ i ];
				}
			}
		}

	}

	*oNewDistance = DrawDistance;

	return starty - scry;
}

float MeshGameObject::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		r3dColor24 color = m_ObjectColor ;
		starty += imgui_DrawColorPicker(scrx, starty, "Object tint", &color, 200, false);

		PropagateChange( color, &MeshGameObject::m_ObjectColor, this, selected );

		int Shadows = !( this->ObjFlags & OBJFLAG_DisableShadows );
		int newShadows = Shadows;

		starty += imgui_Checkbox( scrx, starty, 360, 22, "Enable Shadows", &newShadows, 1 );

		if( Shadows != newShadows )
		{
			PropagateChange( newShadows, &MeshGameObject::UpdateShadows, selected );
		}

		float newDistance = 0.f;

		int isOtherwiseDirty = 0;

		starty += DrawDrawDistancePropertyEditor( scrx, starty, &newDistance, &isOtherwiseDirty );

		float newDistSq = newDistance*newDistance;

		if( newDistSq != DrawDistanceSq || isOtherwiseDirty )
		{
			PropagateChange( newDistSq, &MeshGameObject::DrawDistanceSq, this, selected );
			PropagateChange( &MeshGameObject::OnDrawDistanceChanged, selected );
		}
	}

	return starty-scry;
}
#endif

volatile bool test_instancing = false;
void MeshGameObject::Draw(const r3dCamera& Cam, eRenderStageID DrawState)
{
	R3DPROFILE_FUNCTION("MeshGameObject::Draw");

	static bool render = true;

	if (!render)
		return;

	int IsShadowPath = DrawState >= rsCreateSM && DrawState < rsCreateSM + NumShadowSlices ;

	int mainCameraLod = ChooseMainCameraLOD() ;

	if( DrawState == rsFillGBuffer || IsShadowPath )
	{
		//if(test_instancing)
		//	MeshLOD[mainCameraLod]->DrawMeshInstanced( &mTransform, IsShadowPath );
		//else
		{
			MeshLOD[mainCameraLod]->SetShaderConsts(mTransform);
			if( IsShadowPath )
				MeshLOD[mainCameraLod]->DrawMeshShadows();
			else
			{
				MeshLOD[mainCameraLod]->DrawMeshDeferred( m_ObjectColor, 0 );
			}
		}
	}
}

struct MeshObjDeferredRenderableInstancing : MeshDeferredRenderableInstancing
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("MeshObjDeferredRenderableInstancing");

		MeshObjDeferredRenderableInstancing* This = static_cast< MeshObjDeferredRenderableInstancing* >( RThis );

		This->Mesh->DrawMeshInstanced2();


//		if(This->Parent->ObjFlags & OBJFLAG_PlayerCollisionOnly )
//		{
//#ifndef FINAL_BUILD
//#ifndef WO_SERVER
//			extern bool g_bIsMinimapRendering ;
//			if( r_hide_icons->GetInt()==0 && !g_bIsMinimapRendering && g_bEditMode )
//			{
//				This->Parent->DrawSelected(Cam, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.223f));
//			}
//#endif
//#endif
//			return;
//		}
//
//		if( r_show_draw_order->GetInt() == 1 )
//		{
//			gDEBUG_DrawPositions.PushBack( This->Parent->GetPosition() + This->Parent->GetBBoxLocal().Center() );
//		}

		//r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);
		//r3dApplyPreparedMeshPSConsts(This->Parent->preparedVSConsts);

		//if (This && This->Mesh && This->Mesh->MatChunks[This->BatchIdx].Mat)
		//{
		//	SetLightsIfTransparent(This->Mesh->MatChunks[This->BatchIdx].Mat, This->Parent->GetBBoxWorld());
		//}

		//This->Parent->OnPreRender();
		//MeshDeffRenderableBase::Draw( RThis, Cam );
	}

	MeshGameObject* Parent;
};

struct MeshObjDeferredRenderable : MeshDeffRenderableBase
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("MeshObjDeferredRenderable");

		MeshObjDeferredRenderable* This = static_cast< MeshObjDeferredRenderable* >( RThis );

		if(This->Parent->ObjFlags & OBJFLAG_PlayerCollisionOnly )
		{
#ifndef FINAL_BUILD
#ifndef WO_SERVER
			extern bool g_bIsMinimapRendering ;
			if( r_hide_icons->GetInt()==0 && !g_bIsMinimapRendering && g_bEditMode )
			{
				This->Parent->DrawSelected(Cam, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.223f));
			}
#endif
#endif
			return;
		}

		if( r_show_draw_order->GetInt() == 1 )
		{
			gDEBUG_DrawPositions.PushBack( This->Parent->GetPosition() + This->Parent->GetBBoxLocal().Center() );
		}

		if( r_enable_matrix_cache->GetInt() )
			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);
		else
			r3dMeshSetShaderConsts( This->Parent->GetTransformMatrix(), This->Mesh );

		if (This && This->Mesh && This->Mesh->MatChunks[This->BatchIdx].Mat)
		{
			SetLightsIfTransparent(This->Mesh->MatChunks[This->BatchIdx].Mat, This->Parent->GetBBoxWorld());
		}

		This->Parent->OnPreRender();
		MeshDeffRenderableBase::Draw( RThis, Cam );
	}

	MeshGameObject* Parent;
};

void MeshObjDeferredHighlightRenderable::Init( r3dMesh* mesh, MeshGameObject* parent, DWORD color )
{
	ParentType::Init( mesh, color );

	Parent = parent;
	DrawFunc = Draw;
}

//------------------------------------------------------------------------

void MeshObjDeferredHighlightRenderable::Draw( Renderable* RThis, const r3dCamera& Cam )
{
	MeshObjDeferredHighlightRenderable* This = static_cast< MeshObjDeferredHighlightRenderable* >( RThis );

	PrecalculatedMeshShaderConsts consts;

	This->Parent->PrecalculateMatricesIgnoreSkinning( &consts );

	r3dApplyPreparedMeshVSConsts( consts );

	float distance = ( This->Parent->GetPosition() - Cam ).Length();

	ParentType::DoDraw( RThis, distance, Cam );
}

#ifndef FINAL_BUILD
struct MeshPhysicsDebugRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		MeshPhysicsDebugRenderable* This = static_cast<MeshPhysicsDebugRenderable*>( RThis );
		if(g_DrawCollisionMeshes && This->Parent->PhysicsObject && This->Parent->MeshLOD[0])
		{
			D3DXVECTOR4 color(0.63f, 0.28f, 0.64f, 0.4f);
			D3DXVECTOR4 color_plrOnly(237.0f/255.0f, 28.0f/255.0f, 36.0f/255.0f, 0.4f);
			if(This->Parent->ObjFlags & OBJFLAG_PlayerCollisionOnly)
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, (float*)&color_plrOnly, 1);
			else
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, (float*)&color, 1);

			This->Parent->MeshLOD[0]->SetShaderConsts( This->Parent->GetTransformMatrix() );
			This->Parent->MeshLOD[0]->DrawMeshSimple( 0 );
			r3dRenderer->pd3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			This->Parent->MeshLOD[0]->DrawMeshSimple( 0 );
			r3dRenderer->pd3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		if(g_DrawPlayerOnlyCollisionMeshes && This->Parent->PhysicsObject && This->Parent->PlayerOnly_CollisionMesh)
		{
			D3DXVECTOR4 color(0.0f/255.0f, 255.0f/255.0f, 64.0f/255.0f, 0.4f);
			r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, (float*)&color, 1);

			This->Parent->PlayerOnly_CollisionMesh->SetShaderConsts( This->Parent->GetTransformMatrix() );
			This->Parent->PlayerOnly_CollisionMesh->DrawMeshSimple( 0 );
			r3dRenderer->pd3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			This->Parent->PlayerOnly_CollisionMesh->DrawMeshSimple( 0 );
			r3dRenderer->pd3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		/*else
		{
			This->Parent->MeshLOD[0]->SetWorldMatrix( This->Parent->GetTransformMatrix() );
			This->Parent->MeshLOD[0]->DrawMeshSimple();
		}*/
	}

	MeshGameObject* Parent;
};
#endif

struct DepthMeshRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		DepthMeshRenderable* This = static_cast<DepthMeshRenderable*>( RThis );

		This->Mesh->SetShaderConsts( This->Parent->GetTransformMatrix() );
		This->Mesh->DrawMeshSimple( 0 );
	}

	int					BatchIdx;
	r3dMesh*			Mesh;
	MeshGameObject*		Parent;
};

struct HighlightMeshRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
#ifndef WO_SERVER
		HighlightMeshRenderable* This = static_cast<HighlightMeshRenderable*>( RThis );

		int curVs = r3dRenderer->CurrentVertexShaderID ;
		int curPs = r3dRenderer->CurrentPixelShaderID ;

		This->Mesh->SetShaderConsts( This->Parent->GetTransformMatrix() );

		extern int PS_FWD_COLOR_ID ;
		extern int VS_FWD_COLOR_ID ;
		r3dRenderer->SetVertexShader( VS_FWD_COLOR_ID ) ;
		r3dRenderer->SetPixelShader( PS_FWD_COLOR_ID ) ;
		float colr[ 4 ] = { 1, 0, 0, 0.5 } ;
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, colr, 1 ) ) ;
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH ) ;
		This->Mesh->DrawMeshSimple( 0 );
		r3dRenderer->SetVertexShader( curVs ) ;
		r3dRenderer->SetPixelShader( curPs ) ;
		r3dRenderer->SetRenderingMode( R3D_BLEND_POP ) ;
#endif
	}

	r3dMesh*			Mesh;
	MeshGameObject*			Parent;
};

struct DepthPrepassMeshRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		DepthPrepassMeshRenderable* This = static_cast<DepthPrepassMeshRenderable*>( RThis );

		r3dApplyPreparedMeshVSConsts_DepthPrepass( This->Parent->preparedVSConsts ) ;
		This->Mesh->DrawMeshSimple( 0 );
	}

	r3dMesh*			Mesh;
	MeshGameObject*			Parent;
};


#define	RENDERABLE_PHYSICS_MESHES_SORT_VALUE (33*RENDERABLE_USER_SORT_VALUE)
#define	RENDERABLE_HIGHLIGHT_SORT_VALUE (34*RENDERABLE_USER_SORT_VALUE)

R3D_FORCEINLINE int TransformBoxPoint( const r3dPoint3D& p, const D3DXMATRIX& worldView, r3dPoint2D* Result )
{
	D3DXVECTOR4 outVec ;
	D3DXVECTOR3 inVec ( p.x, p.y, p.z ) ;

	D3DXVec3Transform( &outVec, &inVec, &worldView );

	int Clip = 0 ;

	if( r3dRenderer->NearClip > outVec.z )
	{
		outVec.z = r3dRenderer->NearClip ;
		Clip = 1 ;
	}

	D3DXVec4Transform( &outVec, &outVec, &r3dRenderer->ProjMatrix );

	outVec *= 1.f / outVec.w ;

	*Result = r3dPoint2D( outVec.x, outVec.y ) ;

	return Clip ;
}

R3D_FORCEINLINE int IsFrontFace( const r3dPoint2D& a, const r3dPoint2D& b, const r3dPoint2D& c )
{
	float x1 = a.x ;
	float x2 = b.x ;

	float y1 = a.y ;
	float y2 = b.y ;

	float x = c.x ;
	float y = c.y ;

	return ( y1 - y2 ) * x + ( x2 - x1 ) * y + ( x1 * y2 - x2 * y1 ) > 0.f ;
}

R3D_FORCEINLINE void AddIfFront( float* oRes, const r3dPoint2D& a, const r3dPoint2D& b, const r3dPoint2D& c
#if 0
								, const r3dPoint3D& aa, const r3dPoint3D& bb, const r3dPoint3D& cc 
#endif
								
								)
{
	if( IsFrontFace( a, b, c ) )
	{
#if 0
		extern r3dTL::TArray< r3dPoint3D > debug_shadowFrustum ;

		debug_shadowFrustum.PushBack( aa ) ;
		debug_shadowFrustum.PushBack( bb ) ;
		debug_shadowFrustum.PushBack( cc ) ;
#endif

		*oRes += r3dViewCulledTriangleArea_x2( a, b, c ) ;
	}
}

R3D_FORCEINLINE int GetBboxFrameScore( const r3dBoundBox& box, float dist, const r3dBoundBox& worldBox, const D3DXMATRIX& worldView )
{
	// transform bbox corners

	r3dPoint2D pts[ 8 ] ;

	int i = 0 ;

	int clip_count = 0 ;
	
	clip_count += TransformBoxPoint( box.Org															, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	0.f,		0.f )			, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	box.Size.y,	0.f )			, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( 0.f,			box.Size.y,	0.f )			, worldView, pts + i++ );

	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( 0.f		,	0.f,		box.Size.z )	, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	0.f,		box.Size.z )	, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	box.Size.y,	box.Size.z )	, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( 0.f,			box.Size.y,	box.Size.z )	, worldView, pts + i++ );

	float area = 0.f ;

	if ( clip_count == 8 )
	{
		return 0 ;
	}

	AddIfFront( &area, pts[0+0], pts[1+0], pts[2+0] ) ;
	AddIfFront( &area, pts[0+0], pts[2+0], pts[3+0] ) ;

	AddIfFront( &area, pts[0+4], pts[2+4], pts[1+4] ) ;
	AddIfFront( &area, pts[0+4], pts[3+4], pts[2+4] ) ;

	//------------------------------------------------------------------------

	AddIfFront( &area, pts[0], pts[5], pts[1] ) ;
	AddIfFront( &area, pts[0], pts[4], pts[5] ) ;
											
	AddIfFront( &area, pts[2], pts[6], pts[3] ) ;
	AddIfFront( &area, pts[3], pts[6], pts[7] ) ;

	//------------------------------------------------------------------------
	AddIfFront( &area, pts[1], pts[5], pts[2] ) ;
	AddIfFront( &area, pts[2], pts[5], pts[6] ) ;
											
	AddIfFront( &area, pts[0], pts[3], pts[4] ) ;
	AddIfFront( &area, pts[3], pts[7], pts[4] ) ;

	if( area * r3dRenderer->ScreenW * r3dRenderer->ScreenW < 1.f )
		return 0 ;

	return R3D_MIN( int(area * 2048.f + 			
						R3D_MIN( worldBox.Size.y * 16384.f, 16384.f ) + // to ban floor stuff
						R3D_MIN( 32768.f - dist * 32.f, 32768.f ) )				
						, 65535 );
}

R3D_FORCEINLINE float GetBBoxArea( const r3dBoundBox& box, const D3DXMATRIX& worldView )
{
	// transform bbox corners

	r3dPoint2D pts[ 8 ] ;

	int i = 0 ;

	int clip_count = 0 ;

	clip_count += TransformBoxPoint( box.Org															, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	0.f,		0.f )			, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	box.Size.y,	0.f )			, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( 0.f,			box.Size.y,	0.f )			, worldView, pts + i++ );

	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( 0.f		,	0.f,		box.Size.z )	, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	0.f,		box.Size.z )	, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( box.Size.x,	box.Size.y,	box.Size.z )	, worldView, pts + i++ );
	clip_count += TransformBoxPoint( box.Org	+ r3dPoint3D( 0.f,			box.Size.y,	box.Size.z )	, worldView, pts + i++ );

	float area = 0.f ;

	if ( clip_count == 8 )
	{
		return 0 ;
	}

	AddIfFront( &area, pts[0+0], pts[1+0], pts[2+0] ) ;
	AddIfFront( &area, pts[0+0], pts[2+0], pts[3+0] ) ;

	AddIfFront( &area, pts[0+4], pts[2+4], pts[1+4] ) ;
	AddIfFront( &area, pts[0+4], pts[3+4], pts[2+4] ) ;

	//------------------------------------------------------------------------

	AddIfFront( &area, pts[0], pts[5], pts[1] ) ;
	AddIfFront( &area, pts[0], pts[4], pts[5] ) ;

	AddIfFront( &area, pts[2], pts[6], pts[3] ) ;
	AddIfFront( &area, pts[3], pts[6], pts[7] ) ;

	//------------------------------------------------------------------------
	AddIfFront( &area, pts[1], pts[5], pts[2] ) ;
	AddIfFront( &area, pts[2], pts[5], pts[6] ) ;

	AddIfFront( &area, pts[0], pts[3], pts[4] ) ;
	AddIfFront( &area, pts[3], pts[7], pts[4] ) ;

	return area ;
}

R3D_FORCEINLINE bool EstimateArea( r3dPoint3D worldCentre, float radius ) 
{
	D3DXVECTOR4 viewCentre ;

	D3DXVec3Transform( &viewCentre, (D3DXVECTOR3*)&worldCentre, &r3dRenderer->ViewMatrix ) ;

	float divz = viewCentre.z - radius ;

	if( divz < 1 )
		return true ;

	float projK = R3D_MAX( r3dRenderer->ProjMatrix._11, r3dRenderer->ProjMatrix._22 ) ;

	float projR = projK * radius / divz ;

	return projR * projR * M_PI > r_z_prepass_area->GetFloat() ;
}

BOOL MeshGameObject::Update()
{
	if( m_HitPoints <= 0 && !m_IsDestroyed )
	{
		OnMeshDestroyed();
		m_IsDestroyed = 1;
	}

	// check if this object is visible due to min quality settings, and update physics
	{
		bool visible = true;
#ifndef FINAL_BUILD
		if(!g_bEditMode || g_bForceQualitySelectionInEditor)
#endif
		{
			int meshQuality = (int)m_MinQualityLevel;
			if(meshQuality > 3) meshQuality = 3;
			visible = !(meshQuality > r_mesh_quality->GetInt());
		}

		if(!visible && PhysicsObject)
		{
			SAFE_DELETE(PhysicsObject);
		}
		else if(visible && !PhysicsObject && m_bEnablePhysics)
		{
			CreatePhysicsData();
		}

	}

	return parent::Update();
}

const int minInstancesCount = 10;

bool MeshGameObject::IsInstancedRendered() const 
{
	return 
		r_use_instancing->GetInt() 
			&& 
		currentInstancingMesh && currentInstancingMesh->IsDrawable() && currentInstancingMesh->numInstances > minInstancesCount;
}

void MeshGameObject::SetInstancingFlag( const r3dCamera& Cam ) 
{
	currentInstancingMesh = 0;

	// Copy-paste from AppendRenderables
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	COMPILE_ASSERT( sizeof(MeshObjDeferredRenderable) <= MAX_RENDERABLE_SIZE );

	float distSq = (Cam - GetPosition()).LengthSq();

#ifndef FINAL_BUILD
	if(!g_bEditMode || g_bForceQualitySelectionInEditor)
#endif
	{
		int meshQuality = (int)m_MinQualityLevel;
		if(meshQuality > 3) meshQuality = 3;
		if(meshQuality > r_mesh_quality->GetInt())	
			return;
	}

	int newScore = 0 ;

	float dist = sqrtf( distSq );

	if( r_score_sort->GetInt() )
	{
		D3DXMATRIX worldView ;

		D3DXMatrixMultiply( &worldView, &mTransform, &r3dRenderer->ViewMatrix ) ;

		newScore = GetBboxFrameScore( GetBBoxLocal(), dist, GetBBoxWorld(), worldView ) ;

		// this is like refined bbox testing, if this is zero we don't need to append anything
		if( !newScore )
		{
			return ;
		}
	}

	int meshLodIndex = ChoseMeshLOD( distSq );

	int idist = R3D_MIN( (int)dist, 0xffff );

	r3dMesh* ( &TargetLODSet ) [ NUM_LODS ] = GetPreferredMeshLODSet();
	if(!TargetLODSet[ meshLodIndex ])
		return;
	if(!TargetLODSet[ meshLodIndex ]->IsDrawable())
		return;

	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	currentInstancingMesh = TargetLODSet[ meshLodIndex ];
	currentInstancingMesh->numInstances++;
}

void MeshGameObject::AppendRenderablesInstanced( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	// [Alexey] FIXME: Need to refactor common code with AppendRenderables!
	float distSq = (Cam - GetPosition()).LengthSq();
	float dist = sqrtf( distSq );
	uint32_t prevCount = render_arrays[ m_FillGBufferTarget ].Count();
	uint32_t prevTranspCount = render_arrays[rsDrawTransparents].Count();

	currentInstancingMesh->AppendTransparentRenderables( render_arrays[rsDrawTransparents], m_ObjectColor, dist, 0 );

	r3dMesh* mesh = GetObjectLodMesh();

	r3dPoint3D scaleBox = mesh ? mesh->unpackScale : r3dPoint3D( 1.0f, 1.0f, 1.0f );
	r3dVector scl = GetScale();

	scaleBox.x *= scl.x;
	scaleBox.y *= scl.y;
	scaleBox.z *= scl.z;

	D3DXQUATERNION quat;
	r3dVector angles = GetRotationVector();
	D3DXQuaternionRotationYawPitchRoll(&quat, R3D_DEG2RAD(angles.x), R3D_DEG2RAD(angles.y), R3D_DEG2RAD(angles.z));

	r3dColor premultColor = m_ObjectColor;
	float a = GetSelfIllumMultiplier();
	a *= 255.0f;

	if (a < 0.0f)
		a = 0;
	if (a > 255.0f)
		a = 255.0f;

	premultColor.A = BYTE(a);

	if (currentInstancingMesh->AppendRenderablesDeferredInstanced( render_arrays[ m_FillGBufferTarget ], premultColor, quat, GetPosition(), scaleBox))
	{
		MeshObjDeferredRenderableInstancing& rend = static_cast<MeshObjDeferredRenderableInstancing&>( render_arrays[ m_FillGBufferTarget ][ prevCount ] ) ;
		rend.Init() ;
		rend.Parent = this ;
	}

	for( uint32_t i = prevTranspCount, e = render_arrays[rsDrawTransparents].Count(); i < e; i ++ )
	{
		MeshObjDeferredRenderable& rend = static_cast<MeshObjDeferredRenderable&>( render_arrays[rsDrawTransparents][ i ] ) ;
		rend.Init() ;
		rend.Parent = this ;
	}

	// collision meshes
#ifndef FINAL_BUILD
	if(g_DrawCollisionMeshes || g_DrawPlayerOnlyCollisionMeshes)
	{
		MeshPhysicsDebugRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_PHYSICS_MESHES_SORT_VALUE;
		render_arrays[ rsDrawPhysicsMeshes ].PushBack( rend );
	}
#endif

	if( r_depth_mode->GetInt() )
	{
		DepthMeshRenderable rend ;
		rend.Init() ;		

		rend.Parent		= this ;
		rend.SortValue	= RENDERABLE_PHYSICS_MESHES_SORT_VALUE;
		rend.Mesh		= currentInstancingMesh;

		render_arrays[ rsDrawDepth ].PushBack( rend );
	}

	int need_append = 0 ;

	if( r_z_prepass_method->GetInt() == Z_PREPASS_METHOD_DIST )
	{
		if( r_z_prepass_dist->GetFloat() > dist )
		{
			need_append = 1 ;
		}
	}
	else
	{
		if( r_z_prepass_method->GetInt() == Z_PREPASS_METHOD_AREA )
		{
			R3DPROFILE_START( "MeshObj ZPrepass" ) ;

			if( EstimateArea( GetBBoxWorld().Center(), GetObjectsRadius() ) )
			{
				D3DXMATRIX worldView ;

				D3DXMatrixMultiply( &worldView, &mTransform, &r3dRenderer->ViewMatrix ) ;

				if( r_z_prepass_area->GetFloat() < GetBBoxArea( GetBBoxLocal(), worldView ) )
				{
					need_append = 1 ;
				}
			}

			R3DPROFILE_END( "MeshObj ZPrepass" ) ;
		}
	}

	if( need_append && !( ObjFlags & OBJFLAG_PlayerCollisionOnly ) )
	{
		r3dMesh* mesh = currentInstancingMesh;
		if( !mesh->HasAlphaTextures )
		{
			DepthPrepassMeshRenderable dprend ;

			dprend.Init() ;

			dprend.Parent		= this ;
			dprend.SortValue	= 0 ;
			dprend.Mesh			= mesh ;

			render_arrays[ rsDepthPrepass ].PushBack( dprend ) ;

#ifndef FINAL_BUILD
			if( r_highlight_prepass->GetInt() )
			{
				HighlightMeshRenderable	rend ;
				rend.Init() ;

				rend.Parent		= this ;
				rend.SortValue	= RENDERABLE_HIGHLIGHT_SORT_VALUE ;
				rend.Mesh		= mesh ;

				render_arrays[ rsDrawDebugData ].PushBack( rend ) ;
			}
#endif
		}
	}

	if(r_rc_highlight_radius->GetFloat() > 0.0f)
	{
		float rc_R = r_rc_highlight_radius->GetFloat();
		rc_R *= rc_R;
		if((gCam - GetPosition()).LengthSq() <= rc_R )
		{
			// FIXME: WTF????? Why just replace it with flag?
			if( currentInstancingMesh->HasMaterialOfType( "Concrete_Resource" ) || currentInstancingMesh->HasMaterialOfType( "Wood_Resources" ) || currentInstancingMesh->HasMaterialOfType( "Metal_Resources" ) )
			{
				MeshObjDeferredHighlightRenderable rend;

				rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::yellow.GetPacked()  );
				rend.SortValue = 0;
				rend.DoExtrude = 0;

				render_arrays[ rsDrawHighlightPass0 ].PushBack( rend );

				rend.DoExtrude = 1;
				render_arrays[ rsDrawHighlightPass1 ].PushBack( rend );
			}
		}
	}

#ifndef FINAL_BUILD
	if( r_highlight_casters->GetInt() && !( this->ObjFlags & ( OBJFLAG_DisableShadows | OBJFLAG_PlayerCollisionOnly ) ) )
	{
		HighlightMeshRenderable	rend ;
		rend.Init() ;

		rend.Parent = this ;
		rend.SortValue = RENDERABLE_HIGHLIGHT_SORT_VALUE ;
		rend.Mesh		= currentInstancingMesh;

		render_arrays[ rsDrawDebugData ].PushBack( rend ) ;
	}
#endif

}

void MeshGameObject::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	COMPILE_ASSERT( sizeof(MeshObjDeferredRenderable) <= MAX_RENDERABLE_SIZE );

	float distSq = (Cam - GetPosition()).LengthSq();

#ifndef FINAL_BUILD
	if(!g_bEditMode || g_bForceQualitySelectionInEditor)
#endif
	{
		int meshQuality = (int)m_MinQualityLevel;
		if(meshQuality > 3) meshQuality = 3;
		if(meshQuality > r_mesh_quality->GetInt())	
			return;
	}

	int newScore = 0 ;

	float dist = sqrtf( distSq );

	if( r_score_sort->GetInt() )
	{
		D3DXMATRIX worldView ;

		D3DXMatrixMultiply( &worldView, &mTransform, &r3dRenderer->ViewMatrix ) ;

		newScore = GetBboxFrameScore( GetBBoxLocal(), dist, GetBBoxWorld(), worldView ) ;

		// this is like refined bbox testing, if this is zero we don't need to append anything
		if( !newScore )
		{
			return ;
		}
	}

	int meshLodIndex = ChoseMeshLOD( distSq );

	int idist = R3D_MIN( (int)dist, 0xffff );

	r3dMesh* ( &TargetLODSet ) [ NUM_LODS ] = GetPreferredMeshLODSet();
	if(!TargetLODSet[ meshLodIndex ])
		return;
	if(!TargetLODSet[ meshLodIndex ]->IsDrawable())
		return;

	uint32_t prevCount = render_arrays[ m_FillGBufferTarget ].Count();
	uint32_t prevTranspCount = render_arrays[rsDrawTransparents].Count();

	TargetLODSet[ meshLodIndex ]->AppendRenderablesDeferred( render_arrays[ m_FillGBufferTarget ], m_ObjectColor, GetSelfIllumMultiplier() );
	TargetLODSet[ meshLodIndex ]->AppendTransparentRenderables( render_arrays[rsDrawTransparents], m_ObjectColor, dist, 0 );

	for( uint32_t i = prevCount, e = render_arrays[ m_FillGBufferTarget ].Count(); i < e; i ++ )
	{
		MeshObjDeferredRenderable& rend = static_cast<MeshObjDeferredRenderable&>( render_arrays[ m_FillGBufferTarget ][ i ] ) ;

		int MatScoreID = ( rend.SortValue >> 32 ) & MAT_FRAME_SCORE_MASK ;

		int score = gMatFrameScores[ MatScoreID ] ;
		gMatFrameScores[ MatScoreID ] = R3D_MAX( score, newScore ) ;

		rend.Init() ;
		rend.SortValue |= idist ;
		rend.Parent = this ;
	}

	for( uint32_t i = prevTranspCount, e = render_arrays[rsDrawTransparents].Count(); i < e; i ++ )
	{
		MeshObjDeferredRenderable& rend = static_cast<MeshObjDeferredRenderable&>( render_arrays[rsDrawTransparents][ i ] ) ;
		rend.Init() ;
		rend.Parent = this ;
	}

	// collision meshes
#ifndef FINAL_BUILD
	if(g_DrawCollisionMeshes || g_DrawPlayerOnlyCollisionMeshes)
	{
		MeshPhysicsDebugRenderable rend;

		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_PHYSICS_MESHES_SORT_VALUE;
		render_arrays[ rsDrawPhysicsMeshes ].PushBack( rend );
	}
#endif

	if( r_depth_mode->GetInt() )
	{
		DepthMeshRenderable rend ;
		rend.Init() ;		

		rend.Parent		= this ;
		rend.SortValue	= RENDERABLE_PHYSICS_MESHES_SORT_VALUE;
		rend.Mesh		= TargetLODSet[ meshLodIndex ] ;

		render_arrays[ rsDrawDepth ].PushBack( rend );
	}

	int need_append = 0 ;

	if( r_z_prepass_method->GetInt() == Z_PREPASS_METHOD_DIST )
	{
		if( r_z_prepass_dist->GetFloat() > dist )
		{
			need_append = 1 ;
		}
	}
	else
	{
		if( r_z_prepass_method->GetInt() == Z_PREPASS_METHOD_AREA )
		{
			R3DPROFILE_START( "MeshObj ZPrepass" ) ;

			if( EstimateArea( GetBBoxWorld().Center(), GetObjectsRadius() ) )
			{
				D3DXMATRIX worldView ;

				D3DXMatrixMultiply( &worldView, &mTransform, &r3dRenderer->ViewMatrix ) ;

				if( r_z_prepass_area->GetFloat() < GetBBoxArea( GetBBoxLocal(), worldView ) )
				{
					need_append = 1 ;
				}
			}

			R3DPROFILE_END( "MeshObj ZPrepass" ) ;
		}
	}

	if( need_append && !( ObjFlags & OBJFLAG_PlayerCollisionOnly ) )
	{
		r3dMesh* mesh = TargetLODSet[ meshLodIndex ] ;
		if( !mesh->HasAlphaTextures )
		{
			DepthPrepassMeshRenderable dprend ;

			dprend.Init() ;

			dprend.Parent		= this ;
			dprend.SortValue	= 0 ;
			dprend.Mesh			= mesh ;

			render_arrays[ rsDepthPrepass ].PushBack( dprend ) ;

#ifndef FINAL_BUILD
			if( r_highlight_prepass->GetInt() )
			{
				HighlightMeshRenderable	rend ;
				rend.Init() ;

				rend.Parent		= this ;
				rend.SortValue	= RENDERABLE_HIGHLIGHT_SORT_VALUE ;
				rend.Mesh		= mesh ;

				render_arrays[ rsDrawDebugData ].PushBack( rend ) ;
			}
#endif
		}
	}

	if(r_rc_highlight_radius->GetFloat() > 0.0f)
	{
		float rc_R = r_rc_highlight_radius->GetFloat();
		rc_R *= rc_R;
		if((gCam - GetPosition()).LengthSq() <= rc_R )
		{
			if( TargetLODSet[ meshLodIndex ]->HasMaterialOfType( "Concrete_Resource" ) || TargetLODSet[ meshLodIndex ]->HasMaterialOfType( "Wood_Resources" ) || TargetLODSet[ meshLodIndex ]->HasMaterialOfType( "Metal_Resources" ) )
			{
				MeshObjDeferredHighlightRenderable rend;

				rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::yellow.GetPacked()  );
				rend.SortValue = 0;
				rend.DoExtrude = 0;

				render_arrays[ rsDrawHighlightPass0 ].PushBack( rend );

				rend.DoExtrude = 1;
				render_arrays[ rsDrawHighlightPass1 ].PushBack( rend );
			}
		}
	}

#ifndef FINAL_BUILD
	if( r_highlight_casters->GetInt() && !( this->ObjFlags & ( OBJFLAG_DisableShadows | OBJFLAG_PlayerCollisionOnly ) ) )
	{
		HighlightMeshRenderable	rend ;
		rend.Init() ;

		rend.Parent = this ;
		rend.SortValue = RENDERABLE_HIGHLIGHT_SORT_VALUE ;
		rend.Mesh		= TargetLODSet[ meshLodIndex ] ;

		render_arrays[ rsDrawDebugData ].PushBack( rend ) ;
	}
#endif

}

//------------------------------------------------------------------------

struct MeshObjShadowRenderable : MeshShadowRenderableBase
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("MeshObjShadowRenderable");
		MeshObjShadowRenderable* This = static_cast< MeshObjShadowRenderable* >( RThis );

		r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);
		//This->Mesh->SetWorldMatrix( This->Parent->GetTransformMatrix() );

		This->Parent->OnPreRender();

		This->SubDrawFunc( RThis, Cam );
	}

	MeshGameObject* Parent;
};

/*virtual*/
void
MeshGameObject::AppendShadowRenderables( RenderArray& rarr, int sliceIndex, const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifndef FINAL_BUILD
	if(!g_bEditMode || g_bForceQualitySelectionInEditor)
#endif
	{
		int meshQuality = (int)m_MinQualityLevel;
		if(meshQuality > 3) meshQuality = 3;
		if(meshQuality > r_mesh_quality->GetInt())
			return;
	}

	// always use main camera to determine shadow LOD
	// TODO : mark invisible (in main frustum) objects and use lowest LOD for their shadows
	float distSq = (gCam - GetPosition()).LengthSq();

	float dist = sqrtf( distSq );

	UINT32 idist = (UINT32) R3D_MIN( dist * 64.f, (float)0x3fffffff );

	int meshLodIndex;

	r3dMesh* (&TargetLOD) [ NUM_LODS ] = GetPreferredMeshLODSet();
	
	if( InMainFrustum || !r_simplify_pure_shadows->GetInt() || TargetLOD[ 0 ]->HasAlphaTextures )
	{
		meshLodIndex = ChoseMeshLOD( distSq );
	}
	else
	{
		meshLodIndex = TargetLOD[3] ? 3 : TargetLOD[ 2 ] ? 2 : TargetLOD[ 1 ] ? 1 : 0;
	}

	if(!TargetLOD[ meshLodIndex ])
		return;
	if(!TargetLOD[ meshLodIndex ]->IsDrawable())
		return;

	uint32_t prevCount = rarr.Count();

	TargetLOD[ meshLodIndex ]->AppendShadowRenderables( rarr );

	for( uint32_t i = prevCount, e = rarr.Count(); i < e; i ++ )
	{
		MeshObjShadowRenderable& rend = static_cast<MeshObjShadowRenderable&>( rarr[ i ] );

		rend.SortValue |= idist ;
		rend.Init() ;
		rend.Parent = this ;
	}
}

/*virtual*/
BOOL
MeshGameObject::OnCreate()
{
	if( MeshLOD[0] )
	{
		SetBBoxLocal( MeshLOD[0]->localBBox );

		if( !this->CustomLODDistances )
		{
			const r3dBoundBox& localBox = GetBBoxLocal() ;

			float meshSize = R3D_MAX(R3D_MAX(localBox.Size.x, localBox.Size.y), localBox.Size.z);
			MeshLODDistSq[0] = pow(meshSize * 2, 2.0f);
			MeshLODDistSq[1] = pow(meshSize * 5, 2.0f);
			MeshLODDistSq[2] = pow(meshSize * 10, 2.0f);
		}

		Name = MeshLOD[0]->Name ;
	}

	return parent::OnCreate();
}

GameObject *MeshGameObject::Clone ()
{
	MeshGameObject * pNew = (MeshGameObject*)srv_CreateGameObject("MeshGameObject", FileName.c_str(), GetPosition () );

	pNew->SetRotationVector(GetRotationVector());

	return pNew;
}

BOOL MeshGameObject::GetObjStat ( char * sStr, int iLen )
{
	char sAddStr [MAX_PATH];
	sAddStr[0] = 0;
	if (GameObject::GetObjStat (sStr, iLen))
		r3dscpy(sAddStr, sStr);

	int iFaces = MeshLOD[0] ? MeshLOD[0]->NumIndices/3 : 0;
	int iVertices = MeshLOD[0] ? MeshLOD[0]->NumVertices : 0;
	
	sprintf_s(sStr, iLen, "%sFaces %d\nVertexes %d\n", sAddStr, iFaces, iVertices);

	return TRUE;
}

void MeshGameObject::SetColor(r3dColor24 &clr)
{
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
/*virtual*/
void
MeshGameObject::DrawSelected( const r3dCamera& Cam, const D3DXVECTOR4& color )
{
	if( MeshLOD[0] )
	{
		extern int g_FwdColorVS;
		extern int g_FwdColorPS;

		D3D_V ( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)&color, 1 ) );

		r3dRenderer->SetPixelShader( g_FwdColorPS );
		r3dRenderer->SetVertexShader( g_FwdColorVS );

		DWORD prevFillMode;

		D3D_V ( r3dRenderer->pd3ddev->GetRenderState( D3DRS_FILLMODE, &prevFillMode ) );
		D3D_V ( r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME ) );

		// evil proj mtx hack
		float prev43 = r3dRenderer->ProjMatrix._43;
		r3dRenderer->ProjMatrix._43 -= 0.00033f * ( r3dRenderer->ZDir == r3dRenderLayer::ZDIR_INVERSED ? -1.0f : 1.0f );
		r3dRenderer->ViewProjMatrix = r3dRenderer->ViewMatrix * r3dRenderer->ProjMatrix;

		MeshLOD[0]->SetShaderConsts( mTransform );

		r3dRenderer->ProjMatrix._43 = prev43;
		r3dRenderer->ViewProjMatrix = r3dRenderer->ViewMatrix * r3dRenderer->ProjMatrix;

		MeshLOD[0]->DrawMeshWithoutMaterials();

		D3D_V ( r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, prevFillMode ) );

		r3dRenderer->SetPixelShader();
		r3dRenderer->SetVertexShader();
	}
}
#endif

//--------------------------------------------------------------------------------------------------------
void MeshGameObject::UpdateTransform()
{
	if ( MeshLOD[0] )
	{
		MeshLOD[0]->GetWorldMatrix( GetPosition(), GetScale(), GetRotationVector(), mTransform );
		UpdateWorldBBox();
	}
	else
	{
		GameObject::UpdateTransform();
	}
}


void MeshGameObject::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node meshNode = node.child("mesh");
	if(!meshNode.empty())
	{
		m_ObjectColor.R = R3D_CLAMP(meshNode.attribute("color.r").as_int(), 0, 255);
		m_ObjectColor.G = R3D_CLAMP(meshNode.attribute("color.g").as_int(), 0, 255);
		m_ObjectColor.B = R3D_CLAMP(meshNode.attribute("color.b").as_int(), 0, 255);
	}

	pugi::xml_attribute enableThumbs = meshNode.attribute( "enable_thumbnails" );

	if( enableThumbs.empty() )
	{
		m_ThumbnailsEnabled = 1;
	}
	else
	{
		m_ThumbnailsEnabled = enableThumbs.as_int();
	}
}

void MeshGameObject::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	if(m_ObjectColor.R != 255 || m_ObjectColor.G != 255 || m_ObjectColor.B != 255) // save only if not white
	{
		pugi::xml_node meshNode = node.append_child();
		meshNode.set_name("mesh");
		meshNode.append_attribute("color.r") = m_ObjectColor.R;
		meshNode.append_attribute("color.g") = m_ObjectColor.G;
		meshNode.append_attribute("color.b") = m_ObjectColor.B;
	}
}

//------------------------------------------------------------------------

BOOL MeshGameObject::DoLoad(	r3dMesh* (TargetLODs)[ NUM_LODS ], 
								const char* szFileName, bool LoadCollision )
{
	bool use_thumbnails = false;

	if( IsStatic() && m_ThumbnailsEnabled )
	{
		use_thumbnails = true;
	}

	bool use_default_mat = false;
	if(ObjFlags & OBJFLAG_PlayerCollisionOnly)
	{
		use_default_mat = true; // do not load materials for player only collision meshes
		use_thumbnails = false;
	}

	bool async = ( m_AllowAsyncLoading && g_async_loading->GetInt() ) ? true : false;

	TargetLODs[0] = r3dGOBAddMesh( szFileName, true, use_default_mat, async, false, use_thumbnails );

	char TempStr1[128];

	r3dscpy( TempStr1, szFileName );
	TempStr1[ strlen(TempStr1) - 4 ]=0;
	if(!TargetLODs[0]) 
		return FALSE;

	if( g_bEditMode && LoadCollision )
	{
		char collisionMeshFilename[512];
		r3dscpy(collisionMeshFilename, szFileName);
		int len = strlen(collisionMeshFilename);
		r3dscpy(&collisionMeshFilename[len-4], "_playerOnly.sco");
		if(r3dFileExists(collisionMeshFilename))
			PlayerOnly_CollisionMesh = r3dGOBAddMesh(collisionMeshFilename, true, true);
	}
	//	bool has_alpha = false;

	// load LODs if available
	r3dscpy(TempStr1, szFileName);
	TempStr1[strlen(TempStr1)-4]=0;
	char TempLodName[128];
	sprintf(TempLodName, "%s_LOD1.sco", TempStr1);
	if(r3d_access(TempLodName, 0)==0)
	{
		TargetLODs[1] = r3dGOBAddMesh(TempLodName, true, false, async, false, use_thumbnails );
	}
	sprintf(TempLodName, "%s_LOD2.sco", TempStr1);
	if(r3d_access(TempLodName, 0)==0)
	{
		TargetLODs[2] = r3dGOBAddMesh(TempLodName, true, false, async, false, use_thumbnails);
	}
	sprintf(TempLodName, "%s_LOD3.sco", TempStr1);
	if(r3d_access(TempLodName, 0)==0)
	{
		TargetLODs[3] = r3dGOBAddMesh(TempLodName, true, false, async, false, use_thumbnails);
	}

	return TRUE ;
}

//------------------------------------------------------------------------

r3dMaterial * MeshGameObject::GetMaterial(uint32_t faceID)
{
	r3dMaterial *material = 0;
	r3dMesh *m = GetObjectMesh();
	if (m)
	{
		material = m->GetFaceMaterial(faceID);
	}
	return material;
}

//------------------------------------------------------------------------

void MeshGameObject::OnDrawDistanceChanged()
{
	const r3dSTLString& key = GetMeshLibKey();

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;

		if( obj->ObjTypeFlags & OBJTYPE_Mesh )
		{
			MeshGameObject* mgo = static_cast< MeshGameObject* >( obj );

			if( mgo->GetMeshLibKey() == key )
			{
				mgo->DrawDistanceSq = DrawDistanceSq;

				if( MeshPropertyLibEntry* entry = g_MeshPropertyLib->GetEntry( GetMeshLibKey() ) )
				{
					mgo->UpdateLODDistances( entry );
				}
			}
		}
	}
}

//------------------------------------------------------------------------

void MeshGameObject::OnPreRender()
{
	r3dMesh *m = GetObjectMesh();
	if (m && m->IsSkeletal())
	{
		r3dSkeleton::SetDefaultShaderConstants();
	}
}

//------------------------------------------------------------------------

Positions gDEBUG_DrawPositions ;