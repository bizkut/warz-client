#include "r3dPCH.h"
#include "r3d.h"

#include "../SF/RenderBuffer.h"

#include "DecalChief.h"

DecalChief* g_pDecalChief = 0;
extern r3dScreenBuffer *gScreenSmall;
extern r3dScreenBuffer *gBuffer_Color;

namespace
{
	int PS_ID			= -1;
	int PS_RELIEF_ID	= -1;
	int VS_ID			= -1;

	UINT64 GetTexCplTypeID( const DecalType& dtype );
	void Conform( DecalParams& parms );

	R3D_FORCEINLINE int GetDecalDownScale()
	{
		return r_environment_quality->GetInt() < 2 ? 2 : 1 ;
	}

	const char *const GLOBAL_DECAL_LIB = "library.xml";

	r3dString GetFullFilePath( const r3dString& LevelPath )
	{
		r3dString res( LevelPath );

		res += "\\decals.bin";

		return res;
	}

	r3dString GetFullXMLPath( const r3dString& LevelPath )
	{
		r3dString res( LevelPath );
		res += "\\decals.xml";

		return res;
	}

	char FILE_SIG[] = 
	{
		"\3\3DEKALZ103"
	};
}

extern r3dCamera gCam;

DecalFileRecord::DecalFileRecord()
: TypeID ( INVALID_DECAL_ID )
, Pos( 0.f, 0.f, 0.f )
, Dir( 0.f, 1.f, 0.f )
, LifeTime( -1.f )
, ZRot( 0.f )
, ScaleCoef( 0.f )
{

}

//------------------------------------------------------------------------

DecalType::DecalType()
: DiffuseTex( NULL )
, NormalTex( NULL )
, LifeTime( 0.f )
, ScaleX( 0.33f )
, ScaleY( 0.33f )
, UniformScale( 1 )
, ScaleVar( 0.f )
, RandomRotation( true )
, BackgroundBlendFactor( 0 )
, ClipFarFactor( 0.125f )
, ClipNearFactor( 1.f )
, MinQuality( 2 )
, globalDecal( true )
{
	UVStart[ 0 ] = 0.f;
	UVStart[ 1 ] = 0.f;

	UVEnd[ 0 ] = 1.f;
	UVEnd[ 1 ] = 1.f;
}

//------------------------------------------------------------------------

/*static*/ const float DecalChief::MIN_PROXIMITY = 0.33f;

//------------------------------------------------------------------------

DecalChief::ProximityEntry::ProximityEntry()
: Pos( FLT_MAX, FLT_MAX, FLT_MAX )
, LifeTime( 0.f )
, Radius( 1.f )
{

}

//------------------------------------------------------------------------

DecalChief::Settings::Settings()
: MaxDecalSize( 10.f )
, ReliefMappingDepth( 0.125f )
, AlphaRef( 0.033f )
{

}

//------------------------------------------------------------------------

DecalChief::DecalChief()
: mVB( NULL )
, mIB( NULL )
, mVDecl( NULL )
, mVBOffset( 0 )
, mIBOffset( 0 )
, mUsedVCount( 0 )
, mNoiseTexture( NULL )
, mLastTime( r3dGetTime() )
, mProximitySkipCount( 0 )
, mProximityBufferPtr( 0 )
, mAmmountStaticTexturesLoaded( 0 )
, mAmmountDynamicTexturesLoaded( 0 )
, mLastTexDownScale( 1 )
, mLastLoadDecoQL( 0 )
{
	mDecals.Reserve( MAX_DECALS );
}

//------------------------------------------------------------------------

DecalChief::~DecalChief()
{

}

//------------------------------------------------------------------------

void
DecalChief::AddType( const r3dString& typeName )
{
	DecalType type;

	type.Name = typeName;

	AddType( type );
}

//------------------------------------------------------------------------

void
DecalChief::AddType( const DecalType& type )
{
	r3d_assert( GetTypeID( type.Name ) == INVALID_DECAL_ID );
	r3d_assert( mTypes.Count() == mTypeTexCplIDs.Count() );

	mTypes.PushBack( type );

	mTypeTexCplIDs.PushBack( GetTexCplTypeID( type ) );

	UpdateRegisteredCpls();
}

//------------------------------------------------------------------------

void
DecalChief::RemoveType( const r3dString& typeName )
{
	int id = GetTypeID( typeName );

	r3d_assert( id != INVALID_DECAL_ID );

	mTypes.Erase( id );
	mTypeTexCplIDs.Erase( id );

	for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
	{
		DecalParams& parms = mDecals[ i ];

		if( parms.TypeID == id )
		{
			parms.TypeID = INVALID_DECAL_ID ;
			continue ;
		}

		if( parms.TypeID > id )
		{
			parms.TypeID --;
		}
	}
}

//------------------------------------------------------------------------

uint32_t
DecalChief::GetTypeCount() const
{
	return mTypes.Count();
}

//------------------------------------------------------------------------

const DecalType&
DecalChief::GetTypeByIdx( uint32_t idx ) const
{
	return mTypes[ idx ];
}

//------------------------------------------------------------------------

void
DecalChief::UpdateType( uint32_t idx, const DecalType& type )
{
	DecalType& toUpdate = mTypes[ idx ];

	toUpdate = type;

	mTypeTexCplIDs[ idx ] = GetTexCplTypeID( toUpdate );

	UpdateRegisteredCpls();

	for( int i = 0, e = (int)mDecals.Count(); i < e; i ++ )
	{
		if( mDecals[ i ].TypeID == idx )
		{
			UpdateDecal( i, toUpdate );
		}
	}
}

//------------------------------------------------------------------------

void
DecalChief::UpdateAll()
{
	for (size_t i = 0; i < mTypes.Count(); ++i)
	{
		for (int j = 0, e = (int)mDecals.Count(); j < e; ++j)
		{
			if(mDecals[j].TypeID == i)
			{
				UpdateDecal(j, mTypes[i]);
			}
		}
	}
}

//------------------------------------------------------------------------

uint32_t
DecalChief::GetStaticDecalCount( uint32_t typeIdx ) const
{
	r3d_assert( typeIdx < mTypes.Count() );

	uint32_t count = 0;

	for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
	{
		const DecalParams& parms = mDecals[ i ];
		count += parms.TypeID == typeIdx && !parms.LifeTime ;
	}

	return count;
}

//------------------------------------------------------------------------

uint32_t
DecalChief::GetDecalCount() const
{
	return mDecals.Count();
}

//------------------------------------------------------------------------

const DecalParams*
DecalChief::GetStaticDecal( uint32_t typeIdx, uint32_t idx ) const
{
	r3d_assert( typeIdx != INVALID_DECAL_ID );

	uint32_t ii = 0;

	{
		for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
		{
			const DecalParams& parms = mDecals[ i ];
			if( parms.TypeID == typeIdx && !parms.LifeTime )
			{
				if( ii == idx )
				{				
					return &parms;
				}
				else
					ii ++;
			}
		}
	}

	return NULL;
}

//------------------------------------------------------------------------

void
DecalChief::UpdateStaticDecal( const DecalParams& parms, uint32_t idx )
{
	r3d_assert( parms.TypeID != INVALID_DECAL_ID );

	uint32_t ii = 0;

	for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
	{
		DecalParams& toUpdate = mDecals[ i ];
		if( toUpdate.TypeID == parms.TypeID && !toUpdate.LifeTime )
		{
			if( ii == idx )
			{				
				toUpdate = parms;
				Conform( toUpdate );
				UpdateDecal( i, mTypes[ toUpdate.TypeID ] );
				return;
			}
			else
			{
				ii ++;
			}
		}
	}
}

//------------------------------------------------------------------------

void
DecalChief::RemoveStaticDecal( uint32_t typeIdx, uint32_t idx )
{
	if( const DecalParams* parms = GetStaticDecal( typeIdx, idx ) )
	{
		const_cast< DecalParams* > ( parms )->TypeID = INVALID_DECAL_ID;
	}
}

//------------------------------------------------------------------------

void
DecalChief::RemoveStaticDecalsOfType( uint32_t typeIdx )
{
	for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
	{
		DecalParams& toUpdate = mDecals[ i ];
		if( toUpdate.TypeID == typeIdx && !toUpdate.LifeTime )
		{
			toUpdate.TypeID = INVALID_DECAL_ID ;
		}
	}
}

//------------------------------------------------------------------------

int
DecalChief::PickStaticDecal( const r3dPoint3D& start, const r3dPoint3D& ray, float len ) const
{
	for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
	{
		const DecalParams& parms = mDecals[ i ];

		if( parms.TypeID != INVALID_DECAL_ID && !parms.LifeTime )
		{
			r3dBoundBox bbox;

			float sx = mTypes[ parms.TypeID ].ScaleX * 0.5f;
			float sy = mTypes[ parms.TypeID ].ScaleY * 0.5f;

			float s = R3D_MAX( sx, sy );

			bbox.Org	= parms.Pos - r3dPoint3D( s, s, s );
			bbox.Size	= r3dPoint3D( s, s, s ) * 2;

			float dummy;

			if( bbox.ContainsRay( start, ray, len, &dummy ) )
			{
				return i;
			}
		}
	}

	return INVALID_DECAL_ID;
}

//------------------------------------------------------------------------

const DecalParams&
DecalChief::GetDecal( uint32_t idx ) const
{
	return mDecals[ idx ];
}

//------------------------------------------------------------------------

int
DecalChief::GetDecalIdxInType( int idx ) const
{
	int typeId = mDecals[ idx ].TypeID;

	int stIdx = 0;

	for( uint32_t i = 0, e = idx; i < e; i ++ )
	{
		if( mDecals[ i ].TypeID == typeId )
		{
			stIdx ++;
		}
	}

	return stIdx;
}

//------------------------------------------------------------------------

const DecalChief::Settings&
DecalChief::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
DecalChief::SetSettings( const Settings& settings )
{
	mSettings = settings;
}

//------------------------------------------------------------------------

int
DecalChief::GetTypeID( const r3dString& typeName )
{
	for( uint32_t i = 0, e = mTypes.Count(); i < e; i ++ )
	{
		if( mTypes[ i ].Name == typeName )
			return i;
	}

	return INVALID_DECAL_ID;
}

//------------------------------------------------------------------------

int
DecalChief::Add( const DecalParams& params, bool checkProximity /*= true*/ )
{
	r3d_assert( params.TypeID != INVALID_DECAL_ID );

	if( checkProximity )
	{
		for( uint32_t i = 0, e = ProximityBuffer::COUNT; i < e; i ++ )
		{
			const ProximityEntry& entry = mProximityBuffer[ i ];

			if( entry.LifeTime >= 0.f )
			{
				if(	( entry.Pos - params.Pos ).Length() / entry.Radius < MIN_PROXIMITY * r_decals_proximity_multiplier->GetFloat() )
				{
					mProximitySkipCount ++;
					return INVALID_DECAL_ID;
				}
			}
		}
	}

	int idx = INVALID_DECAL_ID;

	if( mDecals.Count() < MAX_DECALS )
	{
		idx = mDecals.Count();
		mDecals.Resize( idx + 1 );
	}
	else
	{
		for( uint32_t i = 0, e = MAX_DECALS; i < e; i ++ )
		{
			if( mDecals[ i ].TypeID == INVALID_DECAL_ID )
			{
				idx = (int)i;
				break;
			}
		}
	}

	if( idx != INVALID_DECAL_ID )
	{		
		const DecalType& type = mTypes[ params.TypeID ];

		DecalParams& toUpdate = mDecals[ idx ];

		toUpdate		= params;

		Conform( toUpdate );

		if( params.LifeTime < 0 )
			toUpdate.LifeTime = type.LifeTime;

		if( !params.ScaleCoef )
		{
			toUpdate.ScaleCoef = R3D_MIN( R3D_MAX( 1.01f - rand() * type.ScaleVar / RAND_MAX, 0.f ), 1.f );
		}

		if( type.RandomRotation )
		{
			toUpdate.ZRot = 2.F * R3D_PI * rand() / RAND_MAX;
		}

		UpdateDecal( idx, type );

		if( checkProximity )
		{
			ProximityEntry& en = mProximityBuffer[ mProximityBufferPtr++ ];
			mProximityBufferPtr %= PROXIMITY_BUFF_SIZE;

			en.Pos		= params.Pos;
			en.LifeTime	= toUpdate.LifeTime;
			en.Radius	= toUpdate.ScaleCoef * sqrtf( type.ScaleX * type.ScaleX + type.ScaleY * type.ScaleY );
		}
	}

	return idx;
}

//------------------------------------------------------------------------

void
DecalChief::Remove( int idx )
{
	mDecals[ idx ].TypeID = INVALID_DECAL_ID;
}

//------------------------------------------------------------------------

void
DecalChief::Move( int idx, const r3dPoint3D& pos, const r3dPoint3D& norm )
{
	if( idx != INVALID_DECAL_ID && idx < (int)mDecals.Count() )
	{
		DecalParams& parms = mDecals[ idx ];

		// paranoia
		if( parms.TypeID != INVALID_DECAL_ID )
		{
			parms.Pos = pos;
			parms.Dir = norm;

			UpdateDecal( idx, mTypes[ parms.TypeID ] );
		}
	}
}

//------------------------------------------------------------------------

namespace
{
	const char* LIB_FILE_PATH = "Data\\Decals\\";

	r3dString GetFullLibPath( const r3dString& fname )
	{
		return LIB_FILE_PATH + fname;
	}


}

//------------------------------------------------------------------------

bool DecalChief::LoadLib(const r3dString& levelPath)
{
	UnloadTextures( false );

	mTypes.Clear();
	mTypeTexCplIDs.Clear();
	mRegisteredTexCplIDs.Clear();

	r3dString libPaths[] = 
	{
		GetFullLibPath(GLOBAL_DECAL_LIB),
		GetFullXMLPath(levelPath)
	};

	uint32_t numEntries[_countof(libPaths)] = {0};

	bool rv = true;
	for (uint32_t i = 0; i < _countof(libPaths); ++i)
	{
		r3dOutToLog( "Loading decal library %s.\n", libPaths[i].c_str() );
		if (!LoadLibInternal(libPaths[i], i == 0))
		{
			r3dArtBug( "Error: couldn't load decal lib %s!\n", libPaths[i].c_str()) ;
			rv = false;
		}
		numEntries[i] = mTypes.Count();
	}

	//	Copy all entries from global lib if local lib empty
	if (numEntries[0] > 0 && numEntries[1] == numEntries[0])
	{
		for (uint32_t i = 0; i < numEntries[0]; ++i)
		{
			DecalType t = mTypes[i];
			t.Name += "(L)";
			t.LifeTime = 0;
			t.globalDecal = false;
			AddType(t);
		}
	}

	return rv;
}

//------------------------------------------------------------------------

bool
DecalChief::LoadLibInternal(const r3dString &libPath, bool globalLib)
{
	pugi::xml_attribute attrib;

#define PUGI_GET_IF_SET(var,node,name,type) attrib = node.attribute(name); if( !attrib.empty() ) { var = attrib.as_##type(); }

	r3dFile* f = r3d_open( libPath.c_str(), "rb" );
	if ( ! f )
	{
		return false;
	}

	r3dTL::TArray< char > fileBuffer( f->size + 1 );

	fread( &fileBuffer[ 0 ], f->size, 1, f );
	fileBuffer[ f->size ] = 0;

	pugi::xml_document xmlLevelFile;
	pugi::xml_parse_result parseResult = xmlLevelFile.load_buffer_inplace( &fileBuffer[0], f->size );
	fclose( f );

	if( !parseResult )
	{
		r3dError( "DecalChief::LoadLib: Failed to parse %s, error: %s", libPath.c_str(), parseResult.description() );
		return false;
	}

	pugi::xml_node xmlItems	= xmlLevelFile.child( "decals" );

	PUGI_GET_IF_SET( mSettings.MaxDecalSize, xmlItems, "max_decal_size", float );
	PUGI_GET_IF_SET( mSettings.ReliefMappingDepth, xmlItems, "relief_mapping_depth", float );
	PUGI_GET_IF_SET( mSettings.AlphaRef, xmlItems, "alpha_ref", float );	

	pugi::xml_node xmlItem	= xmlItems.child( "decal" );

	int ordinal = 0;

	while( !xmlItem.empty() )
	{
		DecalType type;

		type.Name					= xmlItem.attribute( "name" ).value();
		
		PUGI_GET_IF_SET( type.UVStart[ 0 ], xmlItem, "u_start", float );
		PUGI_GET_IF_SET( type.UVStart[ 1 ], xmlItem, "v_start", float );

		PUGI_GET_IF_SET( type.UVEnd[ 0 ], xmlItem, "u_end", float );
		PUGI_GET_IF_SET( type.UVEnd[ 1 ], xmlItem, "v_end", float );

		PUGI_GET_IF_SET( type.LifeTime, xmlItem, "life_time", float );
		PUGI_GET_IF_SET( type.ScaleX, xmlItem, "scale", float );

		if( !xmlItem.attribute( "scale_y" ).empty() )
		{
			PUGI_GET_IF_SET( type.ScaleY, xmlItem, "scale_y", float );
		}
		else
		{
			type.ScaleY = type.ScaleX ;
		}

		PUGI_GET_IF_SET( type.UniformScale, xmlItem, "uniform_scale", int );

		if( type.UniformScale )
		{
			type.ScaleY = type.ScaleX ;
		}

		PUGI_GET_IF_SET( type.ScaleVar, xmlItem, "scale_var", float );
		PUGI_GET_IF_SET( type.RandomRotation, xmlItem, "random_rot", int );
		PUGI_GET_IF_SET( type.BackgroundBlendFactor, xmlItem, "bkg_blend_factor", float );
		PUGI_GET_IF_SET( type.ClipFarFactor, xmlItem, "clip_far_factor", float );
		PUGI_GET_IF_SET( type.ClipNearFactor, xmlItem, "clip_near_factor", float );
		PUGI_GET_IF_SET( type.MinQuality, xmlItem, "min_quality", int );

		attrib = xmlItem.attribute( "diffuse_tex" );
		if( !attrib.empty() )
		{
			type.DiffuseTexName = attrib.value();
		}

		attrib = xmlItem.attribute( "normal_tex" );
		if( !attrib.empty() )
		{
			type.NormalTexName = attrib.value();
		}

		type.RandomRotation = !!type.RandomRotation;

		type.globalDecal = globalLib;

		AddType( type );
		xmlItem = xmlItem.next_sibling();
	}

	return true;

#undef PUGI_GET_IF_SET

}

//------------------------------------------------------------------------

void DecalChief::SaveLib(const r3dString& levelPath) const
{
	r3dString libPaths[] = 
	{
		GetFullLibPath(GLOBAL_DECAL_LIB),
		GetFullXMLPath(levelPath)
	};

	for (uint32_t i = 0; i < _countof(libPaths); ++i)
	{
		SaveLibInternal(libPaths[i], i == 0);
	}
}

//------------------------------------------------------------------------

void DecalChief::SaveLocalLib(const r3dString& levelPath) const
{
	SaveLibInternal( GetFullXMLPath(levelPath), false );
}

//------------------------------------------------------------------------

void
DecalChief::SaveLibInternal(const r3dString &libPath, bool globalLib) const
{
	pugi::xml_document xmlLibFile;
	xmlLibFile.append_child( pugi::node_comment ).set_value("Decal File");
	pugi::xml_node xmlItems = xmlLibFile.append_child();
	xmlItems.set_name( "decals" );

	xmlItems.append_attribute( "max_decal_size" )		= mSettings.MaxDecalSize;
	xmlItems.append_attribute( "alpha_ref" )			= mSettings.AlphaRef;
	xmlItems.append_attribute( "relief_mapping_depth" )	= mSettings.ReliefMappingDepth;

	for( uint32_t i = 0, e = mTypes.Count(); i < e; i ++ )
	{
		const DecalType& type =  mTypes[ i ];

		if (type.globalDecal != globalLib)
			continue;

		pugi::xml_node xmlItem = xmlItems.append_child();

		xmlItem.set_name( "decal" );

		xmlItem.append_attribute( "name" )				= type.Name.c_str();

		xmlItem.append_attribute( "diffuse_tex" )		= type.DiffuseTexName.c_str();
		xmlItem.append_attribute( "normal_tex" )		= type.NormalTexName.c_str();

		xmlItem.append_attribute( "u_start" )			= type.UVStart[0];
		xmlItem.append_attribute( "v_start" )			= type.UVStart[1];

		xmlItem.append_attribute( "u_end" )				= type.UVEnd[0];
		xmlItem.append_attribute( "v_end" )				= type.UVEnd[1];

		xmlItem.append_attribute( "life_time" )			= type.LifeTime;

		xmlItem.append_attribute( "scale" )				= type.ScaleX;
		xmlItem.append_attribute( "scale_y" )			= type.ScaleY;

		xmlItem.append_attribute( "uniform_scale" )		= type.UniformScale ;

		xmlItem.append_attribute( "scale_var" )			= type.ScaleVar;

		xmlItem.append_attribute( "random_rot" )		= type.RandomRotation ? 1 : 0;
		xmlItem.append_attribute( "bkg_blend_factor" )	= type.BackgroundBlendFactor;
		xmlItem.append_attribute( "clip_far_factor" )	= type.ClipFarFactor;
		xmlItem.append_attribute( "clip_near_factor" )	= type.ClipNearFactor;
		xmlItem.append_attribute( "min_quality" )		= type.MinQuality;
	}

	xmlLibFile.save_file( libPath.c_str() );
}

//------------------------------------------------------------------------

bool
DecalChief::LoadLevel( const r3dString& levelPath )
{
	mLastLoadDecoQL = r_environment_quality->GetInt() ;

	//------------------------------------------------------------------------
	r3dString fullXMLPath = GetFullXMLPath( levelPath );

	bool libFileRead = false;

	if( r3dFile* f = r3d_open( fullXMLPath.c_str(), "rb" ) )
	{
		r3dTL::TArray< char > fileBuffer( f->size + 1 );

		fread( &fileBuffer[ 0 ], f->size, 1, f );
		fileBuffer[ f->size ] = 0;

		pugi::xml_document xmlLevelFile;
		pugi::xml_parse_result parseResult = xmlLevelFile.load_buffer_inplace( &fileBuffer[0], f->size );
		fclose( f );

		if( !parseResult )
		{
			r3dArtBug( "DecalChief::LoadLevel: Failed to parse %s, error: %s\n", fullXMLPath.c_str(), parseResult.description() );
		}
	}
	else
	{
		r3dArtBug( "DecalChief::LoadLevel: Failed to open file %s\n", fullXMLPath.c_str() );
	}

	LoadLib(levelPath);

	//------------------------------------------------------------------------
	r3dString fullPath = GetFullFilePath( levelPath );

	r3dFile* fin = r3d_open( fullPath.c_str(), "rb" );

	if( !fin->IsValid() )
	{
		r3dArtBug( "DecalChief::LoadLevel: Couldn't open %s for reading...\n", fullPath.c_str() );
		return false;
	}

	struct fclose_on_exit
	{
		~fclose_on_exit()
		{
			fclose( closeMe );
		}

		r3dFile* closeMe;
	} fclose_on_exit_var = { fin }; (void)fclose_on_exit_var;

	char CheckSig[ sizeof FILE_SIG ];

	if( fread( CheckSig, sizeof CheckSig, 1, fin ) != 1 )
		return false;

	if( memcmp( CheckSig, FILE_SIG, sizeof CheckSig ) )
	{
		r3dArtBug( "DecalChief::LoadLevel: Unknown file version!\n" );
		return false;
	}

	char LibNameBuf[ 256 ];
	uint8_t libNameLen;
	if( fread( &libNameLen, sizeof libNameLen, 1, fin ) != 1 )
		return false;

	if( fread( LibNameBuf, libNameLen, 1, fin ) != 1 )
		return false;

	LibNameBuf[ libNameLen ] = 0;

	mDecals.Clear();

	r3dTL::TArray< r3dString > typeNames;

	uint32_t typeCount;

	if( fread( &typeCount, sizeof typeCount, 1, fin ) != 1 )
		return false;

	typeNames.Resize( typeCount );

	for( uint32_t i = 0, e  = typeCount; i < e; i ++ )
	{
		char buff[256];

		uint8_t nameLength;
		if( fread( &nameLength, sizeof nameLength, 1, fin ) != 1 )
			return false;

		if( fread( buff, nameLength, 1, fin ) != 1 )
			return false;

		buff[ nameLength ] = 0;

		typeNames[ i ] = buff;
	}

	uint32_t count;

	if( fread( &count, sizeof count, 1, fin ) != 1 )
		return false;

	r3d_assert( count < MAX_DECALS );

	for( uint32_t i = 0, e = count; i < e; i ++ )
	{
		DecalParams parms;
		parms.Static = 1;
		size_t entrySize = sizeof(DecalFileRecord);

		if( fread( &parms, entrySize, 1, fin ) != 1 )
			return false;

		if( parms.TypeID != INVALID_DECAL_ID )
		{
			const r3dString& name = typeNames[ parms.TypeID ];

			parms.TypeID = GetTypeID( name );

			if( parms.TypeID == INVALID_DECAL_ID )
			{
				r3dArtBug( "Decal type %s is referenced, but it's not available!", name.c_str() );
			}
			else
			{
				Add( parms );
			}
		}
	}

	LoadTextures( false );

	return true;
}

//------------------------------------------------------------------------

void
DecalChief::SaveLevel( const r3dString& levelPath ) const
{
	//------------------------------------------------------------------------

	SaveLocalLib( levelPath );

	//------------------------------------------------------------------------

	r3dString fullPath = GetFullFilePath( levelPath );

	FILE* fout = fopen_for_write( fullPath.c_str(), "wb" );

	if( fout == NULL )
	{
		r3dOutToLog( "Couldn't open %s for writing...\n", fullPath.c_str() );
		return;
	}
	
	fwrite( FILE_SIG, sizeof FILE_SIG, 1, fout );

	uint8_t libNameLength = strlen(GLOBAL_DECAL_LIB);
	fwrite( &libNameLength, sizeof libNameLength, 1, fout );
	fwrite( GLOBAL_DECAL_LIB, libNameLength, 1, fout );

	uint32_t typeCount = mTypes.Count();
	fwrite( &typeCount, sizeof typeCount, 1, fout );

	for( uint32_t i = 0, e = typeCount; i < e; i ++ )
	{
		const r3dString& name = mTypes[ i ].Name;

		r3d_assert(name.Length() <= 255); // uint8 nameLength is 0..255
		uint8_t nameLength = name.Length();
	
		//r3d_assert( nameLength < 256 );

		fwrite( &nameLength, sizeof nameLength, 1, fout );
		fwrite( name.c_str(), nameLength, 1, fout );
	}

	uint32_t persistentCount = 0;

	Decals saveus;
	saveus.Reserve( mDecals.Count() );

	for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
	{
		const DecalParams& parms = mDecals[ i ];
		if( parms.TypeID != INVALID_DECAL_ID )
		{
			if( !mTypes[ parms.TypeID ].LifeTime )
			{
				saveus.PushBack( parms );
			}
		}
	}

	uint32_t count = saveus.Count();
	fwrite( &count, sizeof count, 1, fout );

	for (uint32_t i = 0; i < count; ++i)
	{
		size_t s = sizeof(DecalFileRecord);
		fwrite( &saveus[ i ], s, 1, fout );
	}

	fclose( fout );
}

//------------------------------------------------------------------------

void
DecalChief::UnloadTextures( bool staticOnly )
{
	uint32_t staticUnloadSize = 0 ;
	uint32_t dynaUnloadSize = 0 ;

	for( uint32_t i = 0, e = mTypes.Count(); i < e; i ++ )
	{
		DecalType& dt = mTypes[ i ] ;

		bool isStatic = !dt.LifeTime ;

		// unload only static ones
		// ( dynamic ones are bullet holes and are always needed! )
		if( isStatic || !staticOnly )
		{
			if( dt.DiffuseTex )
			{
				// will get deleted
				if( dt.DiffuseTex->GetRefs() == 1 )
				{
					( isStatic ? staticUnloadSize : dynaUnloadSize ) += dt.DiffuseTex->GetTextureSizeInVideoMemory() ;
				}

				r3dRenderer->DeleteTexture( dt.DiffuseTex ) ;
				dt.DiffuseTex = NULL ;
			}

			if( dt.NormalTex )
			{
				// will get deleted
				if( dt.NormalTex->GetRefs() == 1 )
				{
					( isStatic ? staticUnloadSize : dynaUnloadSize ) += dt.NormalTex->GetTextureSizeInVideoMemory() ;
				}

				r3dRenderer->DeleteTexture( dt.NormalTex ) ;
				dt.NormalTex = NULL ;
			}
		}
	}

	if( staticUnloadSize < mAmmountStaticTexturesLoaded )
	{
		r3dOutToLog( "FIXME: some STATIC decal textures weren't unloaded!\n");
	}

	if( staticUnloadSize > mAmmountStaticTexturesLoaded )
	{
		r3dOutToLog( "FIXME: unloaded more STATIC decal textures than loaded! This isn't right!\n");
	}

	r3dOutToLog( "Unloaded %.1f MB STATIC decal textures\n", staticUnloadSize / 1024.f / 1024.f ) ;

	if( !staticOnly )
	{
		if( dynaUnloadSize < mAmmountDynamicTexturesLoaded )
		{
			r3dOutToLog( "FIXME: some DYNAMIC decal textures weren't unloaded!\n");
		}

		if( dynaUnloadSize > mAmmountDynamicTexturesLoaded )
		{
			r3dOutToLog( "FIXME: unloaded more DYNAMIC decal textures than loaded! This isn't right!\n");
		}

		r3dOutToLog( "Unloaded %.1f MB DYNAMIC decal textures\n", dynaUnloadSize / 1024.f / 1024.f ) ;
	}
}

//------------------------------------------------------------------------

void
DecalChief::LoadTextures( bool staticOnly )
{
	int DownScale = GetDecalDownScale();

	mLastTexDownScale = DownScale ;

	mAmmountStaticTexturesLoaded = 0 ;

	if( !staticOnly )
	{
		mAmmountDynamicTexturesLoaded = 0 ;
	}

	typedef r3dgfxSet( DecalType* ) DecalTypePtrSet ;

	DecalTypePtrSet touchedStaticTypes ;

	for( uint32_t i = 0, e = mDecals.Count() ; i < e; i ++ )
	{
		const DecalParams& param = mDecals[ i ] ;

		if( param.TypeID != INVALID_DECAL_ID )
		{
			DecalType& dt = mTypes[ param.TypeID ] ;

			// static?
			if( !dt.LifeTime )
			{
				touchedStaticTypes.insert( &dt ) ;
			}
		}
	}

	// load all non dynamic types' and all touched static types' textures

#if 0
	if( r_environment_quality->GetInt() > 1 )
#endif
	{
		for( DecalTypePtrSet::iterator	i = touchedStaticTypes.begin(), 
										e = touchedStaticTypes.end() ;
										i != e ;
										++ i )
		{
			DecalType& dt = **i ;		

			dt.DiffuseTex	= r3dRenderer->LoadTexture( dt.DiffuseTexName.c_str(), D3DFMT_UNKNOWN, FALSE, DownScale ) ;
			dt.NormalTex	= r3dRenderer->LoadTexture( dt.NormalTexName.c_str(), D3DFMT_UNKNOWN, FALSE, DownScale ) ;

			if( dt.DiffuseTex->GetRefs() == 1 )
			{
				mAmmountStaticTexturesLoaded += dt.DiffuseTex->GetTextureSizeInVideoMemory() ;
			}

			if( dt.NormalTex->GetRefs() == 1 )
			{
				mAmmountStaticTexturesLoaded += dt.NormalTex->GetTextureSizeInVideoMemory() ;
			}
		}

		r3dOutToLog( "Loaded %.1f MB STATIC decal textures\n", mAmmountStaticTexturesLoaded / 1024.f / 1024.f );
	}
#if 0
	else
	{
		r3dOutToLog( "Skipped STATIC decal loading because Effect Quality = LOW\n" ) ;
	}
#endif

	if( !staticOnly )
	{
		for( uint32_t i = 0, e = mTypes.Count() ; i < e; i ++ )
		{
			DecalType& dt =	mTypes[ i ] ;

			// dynamic?
			if( dt.LifeTime )
			{
				dt.DiffuseTex	= r3dRenderer->LoadTexture( dt.DiffuseTexName.c_str(), D3DFMT_UNKNOWN, FALSE, DownScale ) ;
				dt.NormalTex	= r3dRenderer->LoadTexture( dt.NormalTexName.c_str(), D3DFMT_UNKNOWN, FALSE, DownScale ) ;

				if( dt.DiffuseTex->GetRefs() == 1 )
				{
					mAmmountDynamicTexturesLoaded += dt.DiffuseTex->GetTextureSizeInVideoMemory() ;
				}

				if( dt.NormalTex->GetRefs() == 1 )
				{
					mAmmountDynamicTexturesLoaded += dt.NormalTex->GetTextureSizeInVideoMemory() ;
				}
			}
		}

		r3dOutToLog( "Loaded %.1f MB DYNAMIC decal textures\n", mAmmountDynamicTexturesLoaded / 1024.f / 1024.f );
	}

	UpdateTexCpls();
}

//------------------------------------------------------------------------

static void ReloadTexture( r3dTexture*& tex )
{
	if( !tex )
		return;

	char FileName[ 1024 ];
	r3dscpy( FileName, tex->getFileLoc().FileName );
	FileName[ 1023 ] = 0;

	tex->Unload();
	tex->Load( FileName, D3DFMT_FROM_FILE, GetDecalDownScale() );
}


void
DecalChief::ReloadTextures()
{
	// textures can repeat so we build a map to avoid excessive work
	typedef r3dgfxMap( r3dSTLString, r3dTexture* ) TextureMap ;

	TextureMap tmap ;

	for( uint32_t i = 0, e = mTypes.Count() ; i < e ; i ++ )
	{
		DecalType& dt = mTypes[ i ] ;

		if( dt.LifeTime || r_environment_quality->GetInt() > 1 )
		{
			if( dt.NormalTex )
				tmap[ dt.NormalTexName.c_str() ] = dt.NormalTex ;

			if( dt.DiffuseTex )
				tmap[ dt.DiffuseTexName.c_str() ] = dt.DiffuseTex ;
		}
		else
		{
			if( dt.NormalTex || dt.DiffuseTex )
			{
				r3dOutToLog( "ERROR: decal STATIC texture loaded for Effect Quality = LOW\n" ) ;
			}
		}
	}

	for( TextureMap::iterator i = tmap.begin(), e = tmap.end(); i != e ; ++ i )
	{
		ReloadTexture( i->second );
	}

	// hope this works right.
	int newDownScale = GetDecalDownScale();

	if( (int)mLastTexDownScale > newDownScale )
	{
		int k = mLastTexDownScale / newDownScale ;
		mAmmountStaticTexturesLoaded *= k * k ;
		mAmmountDynamicTexturesLoaded *= k * k ;
	}
	else
	{
		int k = newDownScale / mLastTexDownScale ;
		mAmmountStaticTexturesLoaded /= k * k ;
		mAmmountDynamicTexturesLoaded /= k * k ;
	}

	mLastTexDownScale = newDownScale ;
}

//------------------------------------------------------------------------

void
DecalChief::UpdateTexturesForQualitySettings()
{
#if 0
	switch( mLastLoadEffectsQL  )
	{
	case 0:
		// not loaded - do nothing
		break ;
	case 1:
		if( r_environment_quality->GetInt() != 1 )
		{
			LoadTextures( true ) ;
		}
		break ;
	default:
		if( r_environment_quality->GetInt() == 1 )
		{
			UnloadTextures( true ) ;
		}
		break ;
	}
#endif

	mLastLoadDecoQL = r_environment_quality->GetInt() ;
}

//------------------------------------------------------------------------

void
DecalChief::Update()
{
	float newTime = r3dGetTime();
	float dt = newTime - mLastTime;
	mLastTime = newTime;

	for( uint32_t i = 0, e = PROXIMITY_BUFF_SIZE; i < e; i ++ )
	{
		ProximityEntry& en = mProximityBuffer[ i ];

		if( en.LifeTime > 0.f )
		{
			en.LifeTime -= dt;

			if( !en.LifeTime )
			{
				en.LifeTime = -1.f;
			}
		}
	}

	uint32_t contiguousInvalids = 0;

	uint32_t drawIdx = 0;

	uint32_t validCount = 0;

	for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
	{
		DecalParams& parms = mDecals[ i ];
		if( parms.TypeID != INVALID_DECAL_ID )
		{
			bool valid = true;

			if( parms.LifeTime )
			{
				parms.LifeTime -= dt;

				if( parms.LifeTime <= 0.f )
				{
					parms.TypeID = INVALID_DECAL_ID;
					valid = false;
				}
			}

			if( valid )
			{
				validCount ++;
				contiguousInvalids = 0;
			}
		}

		if( parms.TypeID == INVALID_DECAL_ID )
		{
			contiguousInvalids ++;
		}
	}

	mDecals.Resize( mDecals.Count() - contiguousInvalids );

	if( validCount )
	{
		int totalVertCount = validCount * 4;

		if( mVBOffset + totalVertCount > (uint32_t)mVB->GetItemCount() )
		{
			mVBOffset = 0;
		}

		DecalVertex* vert = ( DecalVertex* )mVB->Lock( mVBOffset, totalVertCount );

		int vcount = 0;

		for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
		{
			DecalParams& parms = mDecals[ i ];

			if( parms.TypeID != INVALID_DECAL_ID )
			{
				ubyte fade = parms.LifeTime ? (ubyte)R3D_MAX( R3D_MIN( int( parms.LifeTime * 255.f ), 255 ), 0 ) : 255;

				uint32_t vidx = vcount * 4;

				mDecalVIdxes[ i ] = vidx;

				int source = i * 4;

				*vert = mVertices[ source + 0 ];
				vert->time_left = fade;
				vert ++;
				
				*vert = mVertices[ source + 1 ];
				vert->time_left = fade;
				vert ++;
				
				*vert = mVertices[ source + 2 ];
				vert->time_left = fade;
				vert ++;

				*vert = mVertices[ source + 3 ];
				vert->time_left = fade;
				vert ++;

				vcount ++;
			}
		}
		
		mVB->Unlock();

		mUsedVCount = totalVertCount;
		mVBOffset += totalVertCount;
	}

}

//------------------------------------------------------------------------

void
DecalChief::Draw()
{
#ifndef WO_SERVER
	D3DPERF_BeginEvent( 0, L"DecalChief::Draw" );
	R3DPROFILE_FUNCTION("DecalChief::Draw");

	struct EndEventOnExit
	{
		~EndEventOnExit()
		{
			D3DPERF_EndEvent();
		}

	} endEventOnExit; (void)endEventOnExit;

	if( !mUsedVCount )
		return;

	struct SetRestoreStates
	{
		explicit SetRestoreStates( DWORD samp )
		{
			Sampler = samp;

			D3D_V( r3dRenderer->pd3ddev->GetSamplerState( Sampler, D3DSAMP_ADDRESSU, &PrevAddressU ) );
			D3D_V( r3dRenderer->pd3ddev->GetSamplerState( Sampler, D3DSAMP_ADDRESSV, &PrevAddressV ) );

			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE ) );
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE1, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE ) );

			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
		}

		~SetRestoreStates()
		{
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSU, PrevAddressU ) );
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSV, PrevAddressV ) );

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE1, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );

			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}

		DWORD PrevAddressU;
		DWORD PrevAddressV;

		DWORD Sampler;

	} setRestoreStates ( 0 ); (void)setRestoreStates;

	r3dRenderer->SetCamera( gCam, false );

	d3dc._SetDecl( mVDecl );

	mVB->Set( 0, mVBOffset - mUsedVCount );
	mIB->Set();

	D3DXMATRIX mVP = r3dRenderer->ViewProjMatrix;

	const D3DXMATRIX& mCam = r3dRenderer->ViewMatrix;

	D3DXVECTOR4 vsConst[] = 
	{
		// float4x4	mVP : register( c0 );
		D3DXVECTOR4( mVP._11, mVP._21, mVP._31, mVP._41 ),
		D3DXVECTOR4( mVP._12, mVP._22, mVP._32, mVP._42 ),
		D3DXVECTOR4( mVP._13, mVP._23, mVP._33, mVP._43 ),
		D3DXVECTOR4( mVP._14, mVP._24, mVP._34, mVP._44 ),

		// float3x3	mVStepT : register( c4 );
		D3DXVECTOR4( mCam._11, mCam._12, mCam._13, 0.f ),
		D3DXVECTOR4( mCam._21, mCam._22, mCam._23, 0.f ),
		D3DXVECTOR4( mCam._31, mCam._32, mCam._33, 0.f ),

		// float4 		vTexcTransform : register( c7 );
		D3DXVECTOR4( 0.5f, -0.5f, 0.5f + 0.5f / r3dRenderer->ScreenW, 0.5f + 0.5f / r3dRenderer->ScreenH ),

		// float4 vScaleParams: register(c8);
		D3DXVECTOR4(mSettings.MaxDecalSize * DECAL_MAX_SCALE_COEF, 0.0f, 0.0f, 0.0f)
	};

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float*) vsConst, sizeof vsConst / sizeof vsConst[ 0 ] ) );

	float worldW = 2.f / r3dRenderer->ProjMatrix._11 / r3dRenderer->ScreenW;
	float worldH = 2.f / r3dRenderer->ProjMatrix._22 / r3dRenderer->ScreenH;

	r3dPoint3D wiVec = r3dPoint3D( mCam._11, mCam._21, mCam._31 ) * worldW;
	r3dPoint3D heVec = r3dPoint3D( mCam._12, mCam._22, mCam._32 ) * -worldH;

	const Settings& dc_sts = g_pDecalChief->GetSettings();

	D3DXVECTOR4 psConst[] =
	{						
		// float3 CamPos		: register( c0 );
		D3DXVECTOR4( gCam.x, gCam.y, gCam.z, 0.f ),

		// float4 NormStep0		: register( c1 );
		D3DXVECTOR4( 1.f / r3dRenderer->ScreenW, 0.f, 0.f, 1.f / r3dRenderer->ScreenH ),

		// float3 WNormStep0	: register( c2 );
		D3DXVECTOR4( wiVec.x, wiVec.y, wiVec.z, 0.f ),

		// float3 WNormStep1	: register( c3 );
		D3DXVECTOR4( heVec.x, heVec.y, heVec.z, 0.f ),

		// float Displace      : register( c4 );
		D3DXVECTOR4( dc_sts.ReliefMappingDepth, dc_sts.AlphaRef, 0.f, 0.f )
	};

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)psConst, sizeof psConst / sizeof psConst[ 0 ] ) );

	r3dRenderer->SetVertexShader( VS_ID );
	r3dRenderer->SetPixelShader( r_relief_decals->GetBool() ? PS_RELIEF_ID : PS_ID );

	COMPILE_ASSERT( DecalTexCplIDs::COUNT == DecalVIdxes::COUNT );

	r3d_assert( mDecals.Count() <= DecalTexCplIDs::COUNT );

	extern r3dScreenBuffer* gBuffer_Depth;

	r3dSetFiltering( R3D_POINT,			0 );
	r3dSetFiltering( R3D_BILINEAR,		1 );
	r3dSetFiltering( R3D_ANISOTROPIC,	2 );
	r3dSetFiltering( R3D_ANISOTROPIC,	3 );
	r3dSetFiltering( R3D_ANISOTROPIC,	4 );

	r3dRenderer->SetTex( gBuffer_Depth->Tex, 0 );
	r3dRenderer->SetTex( mNoiseTexture, 1  );

	//	Copy RT into frame buffer to use it as source texture during decal render.

	extern r3dScreenBuffer* gScreenSmall2;
	r3dScreenBuffer* halvedScrTex = gScreenSmall2; // NOTE: gScreenSmall without 2 holds SSAO

	D3D_V( r3dRenderer->pd3ddev->StretchRect(gBuffer_Color->GetTex2DSurface(), 0, halvedScrTex->GetTex2DSurface(), 0, D3DTEXF_POINT) );

	r3dRenderer->SetTex(halvedScrTex->Tex, 4);

	r3d_assert( mDecals.Count() <= mDecalTexCplIDs.COUNT );

	int eql = r_environment_quality->GetInt() ;

	for( uint32_t i = 0, e = mRegisteredTexCplIDs.Count(); i < e; i ++ )
	{

		UINT64 ID = mRegisteredTexCplIDs[ i ];

		int icount = 0;

		int vused = 0;

		uint32_t oneOff = 0;

		for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
		{
			if( mDecals[ i ].TypeID != INVALID_DECAL_ID )
			{
				int ql = mTypes[ mDecals[ i ].TypeID ].MinQuality ;
				if( mDecalTexCplIDs[ i ] == ID && 				 
					eql >= ql )
				{
					int vidx = mDecalVIdxes[ i ];

					mIndices[ icount ++ ] = vidx + 0 ;
					mIndices[ icount ++ ] = vidx + 1 ;
					mIndices[ icount ++ ] = vidx + 2 ;
					mIndices[ icount ++ ] = vidx + 3 ;
					mIndices[ icount ++ ] = vidx + 2 ;
					mIndices[ icount ++ ] = vidx + 1 ;

					vused = R3D_MAX( vused, vidx + 4 );

					oneOff = i;
				}
			}
		}

		if( icount )
		{
			if( (mIBOffset + icount) >= (uint32_t)mIB->GetItemCount() )
			{
				mIBOffset = 0;
			}

			void* locked = mIB->Lock( mIBOffset, icount );

			memcpy( locked, &mIndices[ 0 ], icount * sizeof mIndices[ 0 ] );

			mIB->Unlock();

			DecalType& type = mTypes[ mDecals[ oneOff ].TypeID ];

			AutoLoadTextures( type );

			r3dRenderer->SetTex( type.DiffuseTex, 2 );
			r3dRenderer->SetTex( type.NormalTex, 3 );

			r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST,	0, 0, vused, mIBOffset, icount / 3 );

			mIBOffset += icount;
		}
	}
#endif
}

//------------------------------------------------------------------------

void
DecalChief::DebugDraw()
{
	if( r_decal_normals->GetBool() )
	{
		for( uint32_t i = 0, e = mDecals.Count(); i < e; i ++ )
		{
			DecalParams& params = mDecals[ i ];

			if( params.TypeID != INVALID_DECAL_ID )
			{
				r3dPoint3D vec = params.Dir * 0.33f;

				r3dPoint3D perp;

				if( params.Dir.x > 0.f )
				{
					perp = r3dPoint3D( -params.Dir.y, params.Dir.x, 0.f );
				}
				else
				{
					perp = r3dPoint3D( 0.f, -params.Dir.z, params.Dir.y );
				}

				perp.Normalize();

				r3dPoint3D binorm = perp.Cross( params.Dir );

				r3dPoint3D arr0 = ( perp - 2*params.Dir ) * 0.033f;
				r3dPoint3D arr1 = ( - perp - 2*params.Dir ) * 0.033f;

				r3dPoint3D arr2 = ( binorm - 2*params.Dir ) * 0.033f;
				r3dPoint3D arr3 = ( - binorm - 2*params.Dir ) * 0.033f;

				r3dPoint3D end = params.Pos + vec;

				r3dDrawUniformLine3D( params.Pos, end, gCam, r3dColor24::green );
				r3dDrawUniformLine3D( end, end + arr0, gCam, r3dColor24::green );
				r3dDrawUniformLine3D( end, end + arr1, gCam, r3dColor24::green );
				r3dDrawUniformLine3D( end, end + arr2, gCam, r3dColor24::green );
				r3dDrawUniformLine3D( end, end + arr3, gCam, r3dColor24::green );

			}
		}

		r3dRenderer->Flush();
	}

}

//------------------------------------------------------------------------

void
DecalChief::Init()
{
	PS_ID			= r3dRenderer->GetPixelShaderIdx( "PS_DECAL" );
	PS_RELIEF_ID	= r3dRenderer->GetPixelShaderIdx( "PS_RELIEF_DECAL" );
	VS_ID			= r3dRenderer->GetVertexShaderIdx( "VS_DECAL");

	const uint32_t VERTEX_COUNT = MAX_DECALS * 4;
	const uint32_t INDEX_COUNT = MAX_DECALS * 6;

	COMPILE_ASSERT( VERTEX_COUNT < 65536 );

	mVB = gfx_new r3dVertexBuffer( VERTEX_COUNT * 4, sizeof(DecalVertex), 0, true );
	mIB = gfx_new r3dIndexBuffer( INDEX_COUNT * 4, true );

#if 0
	UINT16* data = (UINT16*)mIB->Lock();

	for( uint32_t i = 0, e = MAX_DECALS; i < e; i ++ )
	{
		uint32_t decIdx = i * 6;

		data[ i * 6 + 0 ] = decIdx + 0 ;
		data[ i * 6 + 1 ] = decIdx + 1 ;
		data[ i * 6 + 2 ] = decIdx + 2 ;
		data[ i * 6 + 3 ] = decIdx + 3 ;
		data[ i * 6 + 4 ] = decIdx + 2 ;
		data[ i * 6 + 5 ] = decIdx + 1 ;
	}

	mIB->Unlock();
#endif

	mVertices.Resize( VERTEX_COUNT );
	mIndices.Resize( INDEX_COUNT );

	D3DVERTEXELEMENT9 declArr[] = 
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_SHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 16, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 20, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 0, 24, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 0, 28, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		D3DDECL_END()
	};

	( r3dDeviceTunnel::CreateVertexDeclaration( declArr, &mVDecl ) );

	mNoiseTexture = r3dRenderer->LoadTexture( "Data\\Decals\\Noise.dds" );
}

//------------------------------------------------------------------------

void
DecalChief::Close()
{
	r3d_assert( mVB );

	delete mVB; mVB = NULL;
	delete mIB; mIB = NULL;

	mVertices.Swap( Vertices() );
	mIndices.Swap( Indices() );
}

//------------------------------------------------------------------------

void
DecalChief::AutoLoadTextures( int idx )
{
	AutoLoadTextures( mTypes[ idx ] );
}

//------------------------------------------------------------------------

void
DecalChief::UpdateTexCpls()
{
	for( uint32_t i = 0, e = mTypes.Count() ; i < e; i ++ )
	{
		mTypeTexCplIDs[ i ] = GetTexCplTypeID( mTypes[ i ] ) ;
	}

	for( uint32_t i = 0, e = mDecals.Count() ; i < e ; i ++ )
	{
		int typeId = mDecals[ i ].TypeID ;

		if( typeId != INVALID_DECAL_ID )
		{
			mDecalTexCplIDs[ i ] = mTypeTexCplIDs[ typeId ];
		}
	}

	UpdateRegisteredCpls();
}

//------------------------------------------------------------------------

void
DecalChief::UpdateRegisteredCpls()
{
	mRegisteredTexCplIDs.Clear();

	r3d_assert( mTypeTexCplIDs.Count() == mTypes.Count() );

	for( uint32_t i = 0, e = mTypeTexCplIDs.Count(); i < e; i ++ )
	{
		UINT64 id = mTypeTexCplIDs[ i ];

		bool found = false;

		for( uint32_t i = 0, e = mRegisteredTexCplIDs.Count(); i < e ; i ++ )
		{
			if( mRegisteredTexCplIDs[ i ] == id )
			{
				found = true;
				break;
			}
		}

		if( !found )
		{
			mRegisteredTexCplIDs.PushBack( id );
		}
	}
}

//------------------------------------------------------------------------

void
DecalChief::UpdateDecal( int idx, const DecalType& type )
{
	const DecalParams& parms = mDecals[ idx ];

	mDecalTexCplIDs[ idx ]	= mTypeTexCplIDs[ parms.TypeID ];

	int vidx = idx * 4;
	DecalVertex& v0 = mVertices[ vidx++ ];

	v0.pos.x				= parms.Pos.x;
	v0.pos.y				= parms.Pos.y;
	v0.pos.z				= parms.Pos.z;

	ubyte ZRot = ubyte( parms.ZRot * 255.f / R3D_PI / 2 );

	v0.pos_xy_scale_x			= static_cast<ushort>(type.ScaleX * parms.ScaleCoef / mSettings.MaxDecalSize / DECAL_MAX_SCALE_COEF * 32767);
	v0.pos_xy_scale_y			= static_cast<ushort>(type.ScaleY * parms.ScaleCoef / mSettings.MaxDecalSize / DECAL_MAX_SCALE_COEF * 32767);

	v0.z_rot					= ZRot;

	v0.corner_x					= 0;
	v0.corner_y					= 0;

	v0.tex_tile_scale_u			= ubyte ( ( type.UVEnd[ 0 ] - type.UVStart[ 0 ] ) * 255 );
	v0.tex_tile_scale_v			= ubyte ( ( type.UVEnd[ 1 ] - type.UVStart[ 1 ] ) * 255 );
	v0.tex_tile_u				= ubyte ( type.UVStart[ 0 ] * 255 );
	v0.tex_tile_v				= ubyte ( type.UVStart[ 1 ] * 255 );
	v0.dir_x					= ubyte ( R3D_MAX( R3D_MIN( int( ( parms.Dir.x * 0.5f + 0.5f ) * 255 ), 255 ), 0 ) );
	v0.dir_y					= ubyte ( R3D_MAX( R3D_MIN( int( ( parms.Dir.y * 0.5f + 0.5f ) * 255 ), 255 ), 0 ) );
	v0.dir_z					= ubyte ( R3D_MAX( R3D_MIN( int( ( parms.Dir.z * 0.5f + 0.5f ) * 255 ), 255 ), 0 ) );
	v0.time_left				= 255;

	v0.background_blend_factor	= static_cast<ubyte>(type.BackgroundBlendFactor * 255);
	v0.clip_far_factor			= static_cast<ubyte>(type.ClipFarFactor * 255) ;
	v0.clip_near_factor			= static_cast<ubyte>(type.ClipNearFactor * 255)  ;
	v0.unusued2					= 0 ;


	DecalVertex& v1 = mVertices[ vidx ++ ] = v0;

	v1.corner_x		= 0;
	v1.corner_y		= 255;

	DecalVertex& v2 = mVertices[ vidx ++ ] = v0;

	v2.corner_x		= 255;
	v2.corner_y		= 0;

	DecalVertex& v3 = mVertices[ vidx ++ ] = v0;

	v3.corner_x		= 255;
	v3.corner_y		= 255;
}

//------------------------------------------------------------------------

void
DecalChief::AutoLoadTextures( DecalType& type )
{
	extern bool g_bEditMode ;

	bool loaded = false ;

	if( !type.DiffuseTex )
	{			
		// don't wine if we're in editor - this can happen there...
		if( !g_bEditMode )
		{
			r3dOutToLog( "ERROR: Encountered empty DIFFUSE texture on decal %s!\n", type.Name.c_str() ) ;
		}

		type.DiffuseTex = r3dRenderer->LoadTexture( type.DiffuseTexName.c_str() ) ;

		if( type.DiffuseTex->GetRefs() == 1 )
		{
			if( !type.LifeTime )
			{
				mAmmountStaticTexturesLoaded += type.DiffuseTex->GetTextureSizeInVideoMemory();
			}
			else
			{
				mAmmountDynamicTexturesLoaded += type.DiffuseTex->GetTextureSizeInVideoMemory();
			}
		}

		loaded = true ;
	}

	if( !type.NormalTex )
	{			
		// don't wine if we're in editor - this can happen there...
		if( !g_bEditMode )
		{
			r3dOutToLog( "ERROR: Encountered empty NORMAL texture on decal %s!\n", type.Name.c_str() ) ;
		}

		type.NormalTex = r3dRenderer->LoadTexture( type.NormalTexName.c_str() ) ;

		if( type.NormalTex->GetRefs() == 1 )
		{
			if( !type.LifeTime )
			{
				mAmmountStaticTexturesLoaded += type.NormalTex->GetTextureSizeInVideoMemory();
			}
			else
			{
				mAmmountDynamicTexturesLoaded += type.NormalTex->GetTextureSizeInVideoMemory();
			}
		}

		loaded = true ;
	}

	if( loaded )
	{
		UpdateTexCpls();
	}
}

//------------------------------------------------------------------------

namespace
{
	UINT64 GetTexCplTypeID( const DecalType& dtype )
	{
		return	((UINT64)( dtype.DiffuseTex ? dtype.DiffuseTex->GetID() : 0 ) & 0xffffffff ) + 
				((UINT64)( ( dtype.NormalTex ? dtype.NormalTex->GetID() : 0 ) & 0xffffffff ) << 32 );
	}

	void Conform( DecalParams& parms )
	{
		parms.ZRot	= fmodf( parms.ZRot, R3D_PI * 2 );
		parms.Dir.Normalize();
	}
}

//------------------------------------------------------------------------

bool operator != ( const DecalType& t0, const DecalType& t1 )
{
	return	t0.Name				!= t1.Name								||
			t0.DiffuseTex		!= t1.DiffuseTex						||
			t0.NormalTex		!= t1.NormalTex							||
			memcmp( t0.UVStart, t1.UVStart, sizeof t0.UVStart )			||
			memcmp( t0.UVEnd, t1.UVEnd, sizeof t0.UVEnd )				||
			t0.LifeTime			!= t1.LifeTime							||
			t0.ScaleX			!= t1.ScaleX							||
			t0.ScaleY			!= t1.ScaleY							||
			t0.UniformScale		!= t1.UniformScale						||
			t0.ScaleVar			!= t1.ScaleVar							||
			t0.RandomRotation	!= t1.RandomRotation					||
			t0.BackgroundBlendFactor != t1.BackgroundBlendFactor		||
			t0.ClipFarFactor != t1.ClipFarFactor						||
			t0.ClipNearFactor != t1.ClipNearFactor						||
			t0.MinQuality != t1.MinQuality
			;
			
}

