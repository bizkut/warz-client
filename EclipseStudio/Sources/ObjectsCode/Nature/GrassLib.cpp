#include "r3dPCH.h"
#include "r3d.h"

#include "r3dBudgeter.h"

#include "r3dDeviceQueue.h"

#include "../sf/RenderBuffer.h"

#include "TSG_STL/BigEndian.h"

#include "GrassGen.h"
#include "GrassMap.h"
#include "GrassLib.h"

using namespace r3dTL;

extern bool g_bEditMode ;

GrassLib* g_pGrassLib = 0;

//------------------------------------------------------------------------

bool ReadString( r3dString& oString, r3dFile* fin )
{
	char ch;

	int len = 0;
	char buf[ 512 ];

	int nCount = 0;
	while( len < sizeof buf )
	{
		nCount = fread( &ch, 1,1,fin );
		if ( ! nCount || ! ch )
			break;
		buf[ len ++ ] = ch;
	}

	if( len == sizeof buf )
		return false;

	buf[ len ++ ] = 0;

	if( !nCount )
		return false;


	oString = buf;

	return true;
}

//------------------------------------------------------------------------

namespace
{
	void ReleaseEntry( GrassLibEntry& entry );

//------------------------------------------------------------------------

	template <typename FileType>
	struct AutoClose
	{
		~AutoClose()
		{
			fclose( closeMe );
		}

		FileType* closeMe;

	};
}

//------------------------------------------------------------------------

int GetGrassDownScale()
{
	return r_environment_quality->GetInt() == 1 ? 2 : 1 ;
}

static void grReloadTexture( r3dTexture*& tex )
{
	if( !tex )
		return;

	char FileName[ 1024 ];
	r3dscpy( FileName, tex->getFileLoc().FileName );
	FileName[ 1023 ] = 0;

	tex->Unload();
	tex->Load( FileName, D3DFMT_FROM_FILE, GetGrassDownScale() );
}

//------------------------------------------------------------------------

GrassChunk::GrassChunk()
: NumVariations( 0 )
, VerticesPerItem( 0 )
, IndicesPerItem( 0 )
, IndexBuffer( NULL )
, Texture( NULL )
, AlphaRef( 0.25f )
, TintStrength( 1.f )
, TintFade( 0.f )
, TintFadeStrength( 1.0f )
, IsStiff( 0 )
{
	memset( VertexBuffs, 0, sizeof VertexBuffs );
}

GrassChunk::~GrassChunk()
{
}

//------------------------------------------------------------------------

GrassLib::ExtraSettings::ChunkSettings::ChunkSettings()
: TintFade( 0.f )
, TintFadeStrength( 1.0f )
{

}

//------------------------------------------------------------------------
/*static*/

LPDIRECT3DVERTEXDECLARATION9	GrassChunk::VDecl = NULL;

//------------------------------------------------------------------------

GrassLib::Settings::Settings()
: CellScale( 64.f )
, MaxMeshScale( 16.f )
, UniformMaskThreshold( 0.99951171875f )
, BlankMaskThreshold( 0.00390625f )
, MaxBlankMaskThreshold( 0.2f )
{

}

//------------------------------------------------------------------------

GrassLib::GrassLib()
: mVersion( 0 )
{

}

//------------------------------------------------------------------------

GrassLib::~GrassLib()
{
	Unload();
}

//------------------------------------------------------------------------

static void InMainThread( void* p )
{
	GrassLibEntry* en = (GrassLibEntry*) p ;

	ReleaseEntry( *en ) ;
}

//------------------------------------------------------------------------

namespace
{
	char Graz_SIG[] = "GRAZ100";
	char Graz_SIG2[] = "\3\3GRAZ100";
	char Graz_SIG3[] = "\3\3GRAZ102";
	char Graz_SIG4[] = "\3\3GRAZ103";
	char Graz_SIG_CUR[] = "\3\3GRAZ104";
	char GrazLib_FILE[] = "grassmeshes.bin";
	char GrazLib_Settings_FILE[] = "grassmeshes.xml";
}

bool
GrassLib::Save()
{
	const r3dString path = GetGrassPath( GrazLib_FILE );

	// should be able to save only after full regen
	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		const GrassLibEntry& en = GetEntryInternal( i ); en ;
		for( int i = 0, e = en.Chunks.Count() ; i < e ; i ++ )
		{
			r3d_assert( en.Chunks[ i ].SysmemIndices.Count() ) ;
		}
	}

	FILE* fout = fopen( path.c_str(), "wb" );

	if( !fout )
		return false;

	AutoClose<FILE> autoClose = { fout }; (void)autoClose;

	fwrite( Graz_SIG_CUR, sizeof Graz_SIG_CUR, 1, fout );

	uint32_t entryCount = mEntries.Count();

	fwrite_be( entryCount, fout );

	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		const GrassLibEntry& en = GetEntryInternal( i );

		fwrite( en.Name.c_str(), strlen( en.Name.c_str() ) + 1, 1, fout );
		
		//	Memorize current position, to write entire chunk size here
		long chunkSizePos = ftell(fout);
		//	Write dummy data in place of chunk file size entry
		fwrite_be(0L, fout);

		uint32_t chunkCount = en.Chunks.Count();

		fwrite_be( chunkCount, fout );

		for( uint32_t i = 0, e = chunkCount; i < e; i ++ )
		{
			const GrassChunk& chunk = en.Chunks[ i ];

			fwrite_be( chunk.VerticesPerItem, fout ) ;
			fwrite_be( chunk.IndicesPerItem, fout ) ;

			fwrite_be( chunk.AlphaRef, fout );
			fwrite_be( chunk.TintStrength, fout );

			fwrite_be( chunk.NumVariations, fout );
			fwrite( chunk.Texture->getFileLoc().FileName, strlen( chunk.Texture->getFileLoc().FileName ) + 1, 1, fout );

			const void* data = &chunk.SysmemIndices[ 0 ];

			r3d_assert( chunk.SysmemIndices.Count() * sizeof( UINT16 ) == chunk.IndexBuffer->GetDataLength() ) ;

			uint32_t itemCount = chunk.IndexBuffer->GetItemCount();
			fwrite_be( itemCount, fout );

			fwrite( data, chunk.SysmemIndices.Count() * sizeof( UINT16 ), 1, fout );

			for( uint32_t i = 0, e = chunk.NumVariations; i < e; i ++ )
			{
				const GrassChunk::Vertices& systemVertices = chunk.SysmemVertices[ i ] ;

				const void * data = &systemVertices[ 0 ] ;
				uint32_t itemCount = chunk.VertexBuffs[ i ]->GetItemCount();

				r3d_assert( systemVertices.Count() * sizeof( GrassVertex ) == chunk.VertexBuffs[ i ]->GetDataLength() ) ;

				fwrite_be( itemCount, fout );
				fwrite( data, chunk.VertexBuffs[ i ]->GetDataLength(), 1, fout );
			}
		}

		//	Now calculate total bytes written, and write this number into position memorized at chunk entry start
		long curFilePos = ftell(fout);
		fseek(fout, chunkSizePos, SEEK_SET);

		//	Exclude position entry
		chunkSizePos += sizeof(long);
		long chunkSize = curFilePos - chunkSizePos;

		fwrite_be(chunkSize, fout);

		//	Return back to original write position
		fseek(fout, curFilePos, SEEK_SET);
	}

	const r3dString settingsPath = GetGrassPath( GrazLib_Settings_FILE );

	pugi::xml_document xmlFile;
	xmlFile.append_child( pugi::node_comment ).set_value("Grass Settings File") ;
	pugi::xml_node xmlRoot = xmlFile.append_child() ;
	xmlRoot.set_name( "root" ) ;

	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		const GrassLibEntry& en = GetEntryInternal( i );

		pugi::xml_node entry = xmlRoot.append_child() ;

		entry.set_name( "entry" );

		entry.append_attribute( "name" ) = en.Name.c_str();

		uint32_t chunkCount = en.Chunks.Count();

		for( uint32_t i = 0, e = chunkCount; i < e; i ++ )
		{
			pugi::xml_node chunk = entry.append_child();

			chunk.set_name( "chunk" );

			chunk.append_attribute( "index" ) = i;

			chunk.append_attribute( "tint_fade" ) = en.Chunks[ i ].TintFade;

			chunk.append_attribute( "tint_fade_strength" ) = en.Chunks[ i ].TintFadeStrength;

			chunk.append_attribute( "is_stiff" ) = en.Chunks[ i ].IsStiff;
		}
	}

	xmlFile.save_file( settingsPath.c_str() );

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool
GrassLib::LoadGrassLibEntry(GrassLibEntry &en, r3dFile *fin)
{
	r3d_assert(en.fileStartOffset > 0);
	r3d_assert(fin);

	int skipEachSecondInBuffer = r_grass_skip_step->GetInt() ? 1 : 0 ;

	int seekResult = fseek(fin, en.fileStartOffset, SEEK_SET);
	r3d_assert(seekResult == 0);

	uint32_t chunkCount;

	if( fread_be( chunkCount, fin ) != 1 )
		return false;

	en.Chunks.Resize( chunkCount );

	for( int i = 0, e = mExtraSettings.Count(); i < e; i ++ )
	{
		const ExtraSettings& ests = mExtraSettings[ i ];

		if( ests.name == en.Name )
		{
			for( int i = 0, e = en.Chunks.Count(); i < e; i ++ )
			{
				if( i < (int)ests.TheChunkSettings.Count() )
				{
					en.Chunks[ i ].TintFade			= ests.TheChunkSettings[ i ].TintFade;
					en.Chunks[ i ].TintFadeStrength	= ests.TheChunkSettings[ i ].TintFadeStrength;
					en.Chunks[ i ].IsStiff			= ests.TheChunkSettings[ i ].IsStiff;
				}
			}
			break;
		}
	}

	for( uint32_t i = 0, e = chunkCount; i < e; i ++ )
	{
		GrassChunk& chunk = en.Chunks[ i ];

		if( fread_be( chunk.VerticesPerItem, fin ) != 1 )
			return false ;

		if( fread_be( chunk.IndicesPerItem, fin ) != 1 )
			return false ;

		if( fread_be( chunk.AlphaRef, fin ) != 1 )
			return false;

		if( fread_be( chunk.TintStrength, fin ) != 1 )
			return false;

		if( fread_be( chunk.NumVariations,	fin ) != 1 )
			return false;

		r3dString texLoc;
		if( !ReadString( texLoc, fin ) )
			return false;

		chunk.Texture = r3dRenderer->LoadTexture( texLoc.c_str(), D3DFMT_UNKNOWN, false, GetGrassDownScale() );

		uint32_t itemCount;

		if( fread_be( itemCount, fin ) != 1 )
			return false;

		int ibItemCount = itemCount ;

		int meshCountAsFromIb = ibItemCount / chunk.IndicesPerItem ;

		if( skipEachSecondInBuffer )
		{
			meshCountAsFromIb /= 2 ; 
			ibItemCount = meshCountAsFromIb * chunk.IndicesPerItem ;
		}

		chunk.IndexBuffer = gfx_new r3dIndexBuffer( ibItemCount, false, 2, GrassBufferMem );

		void* data = chunk.IndexBuffer->Lock();
		{
			TUnlock< r3dIndexBuffer > delay_unlock( chunk.IndexBuffer ) ;

			unsigned short * indices = 0 ;
			if( g_bEditMode )
			{
				chunk.SysmemIndices.Resize( itemCount ) ;
				indices = &chunk.SysmemIndices[ 0 ] ;

				if( fread( indices, itemCount * chunk.IndexBuffer->GetItemSize() , 1, fin ) != 1 )
					return false ;
			}
			else
			{
				if( fread( data, chunk.IndexBuffer->GetDataLength(), 1, fin ) != 1 )
					return false ;

				if( skipEachSecondInBuffer )
				{
					fseek( fin, itemCount * chunk.IndexBuffer->GetItemSize() - chunk.IndexBuffer->GetDataLength(), SEEK_CUR ) ;
				}
			}

			if( g_bEditMode )
			{
				memcpy( data, indices, ibItemCount * chunk.IndexBuffer->GetItemSize() ) ;
			}
		}

		for( uint32_t i = 0, e = chunk.NumVariations; i < e; i ++ )
		{
			uint32_t itemCount;
			if( fread_be( itemCount, fin ) != 1 )
				return false;

			int origMeshCount = itemCount / chunk.VerticesPerItem ;
			int meshCount = origMeshCount ;

			int vbItemCount = itemCount ;
			if( skipEachSecondInBuffer )
			{
				meshCount /= 2 ;
				vbItemCount = meshCount * chunk.VerticesPerItem ;
			}

			r3d_assert( meshCount == meshCountAsFromIb ) ;

			r3dVertexBuffer* vertexBuffer = gfx_new r3dVertexBuffer( vbItemCount, sizeof (GrassVertex), 0, false, false, GrassBufferMem ) ;
			chunk.VertexBuffs[ i ] = vertexBuffer ;

			GrassChunk::Vertices& sysmemVertices = chunk.SysmemVertices[ i ] ;

			void * data = vertexBuffer->Lock( 0, vbItemCount );
			{
				TUnlock< r3dVertexBuffer > delay_unlock( vertexBuffer ) ;

				GrassVertex * vertices = 0 ;

				if( g_bEditMode )
				{
					sysmemVertices.Resize( itemCount ) ;
					vertices = &sysmemVertices[ 0 ] ;

					if( fread( vertices, itemCount * sizeof( GrassVertex ), 1, fin ) != 1 )
						return false;

					r3d_assert( sysmemVertices.Count() * sizeof ( sysmemVertices[ 0 ] ) == vertexBuffer->GetDataLength() 
						|| 
						meshCount * chunk.VerticesPerItem * sizeof( GrassVertex ) == vertexBuffer->GetDataLength()
						) ;
				}
				else
				{
					int chunkByteSize = chunk.VerticesPerItem * sizeof( GrassVertex ) ;

					for( int i = 0, e = vertexBuffer->GetItemCount() ; i < e ; i += chunk.VerticesPerItem )
					{
						if( fread( ( GrassVertex*)data + i, chunkByteSize, 1, fin ) != 1 )
							return false;

						if( skipEachSecondInBuffer )
						{
							fseek( fin, chunkByteSize, SEEK_CUR ) ;
						}
					}

					if( origMeshCount & 1 && skipEachSecondInBuffer )
					{
						fseek( fin, chunkByteSize, SEEK_CUR ) ;
					}
				}

				if( g_bEditMode )
				{
					if( skipEachSecondInBuffer )
					{
						for( int i = 0, j = 0, e = vertexBuffer->GetItemCount() ; j < e ; 
							i += 2 * chunk.VerticesPerItem, 
							j += chunk.VerticesPerItem )
						{
							memcpy( (GrassVertex*)data + j , 
								(GrassVertex*)vertices + i , sizeof(GrassVertex) * chunk.VerticesPerItem ) ;
						}
					}
					else
					{
						memcpy( data, vertices, sizeof( GrassVertex ) * itemCount ) ;
					}
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool
GrassLib::LoadGrassLibEntry(GrassLibEntry &en)
{
	if( r3d_access( GetGrassLibFile().c_str(), 0 ) )
	{
		return false ;
	}

	const r3dString path = GetGrassPath( GrazLib_FILE );

	r3dFile* fin = r3d_open( path.c_str(), "rb" );

	if( !fin )
		return false;

	AutoClose<r3dFile> autoClose = { fin }; autoClose;
	return LoadGrassLibEntry(en, fin);
}

//////////////////////////////////////////////////////////////////////////

bool
GrassLib::Load()
{
	r3d_assert( mEntries.Count() == 0 ) ;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	pugi::xml_attribute attrib;

#define PUGI_GET_IF_SET(var,node,name,type) attrib = node.attribute(name); if( !attrib.empty() ) { var = attrib.as_##type(); }
	mExtraSettings.Clear();

	r3dFile* f = r3d_open( GetGrassPath( GrazLib_Settings_FILE ).c_str(), "rb" );
	if ( f )
	{
		r3dTL::TArray< char > fileBuffer( f->size + 1 );

		fread( &fileBuffer[ 0 ], f->size, 1, f );
		fileBuffer[ f->size ] = 0;

		pugi::xml_document xmlLevelFile;
		pugi::xml_parse_result parseResult = xmlLevelFile.load_buffer_inplace( &fileBuffer[0], f->size );
		fclose( f );

		if( !parseResult )
		{
			r3dOutToLog( "GrassLib::LoadGrassLibEntry: Failed to parse %s, error: %s", GrazLib_Settings_FILE, parseResult.description() );
		}
		else
		{
			pugi::xml_node xmlRoot = xmlLevelFile.child( "root" );
			pugi::xml_node xmlEntry = xmlRoot.child( "entry" );

			while( !xmlEntry.empty() )
			{
				ExtraSettings sts;

				sts.name = xmlEntry.attribute( "name" ).value();

				pugi::xml_node xmlChunk = xmlEntry.child( "chunk" );

				while( !xmlChunk.empty() )
				{
					ExtraSettings::ChunkSettings csts;

					int idx = xmlChunk.attribute("index").as_int();

					sts.TheChunkSettings.Resize( idx + 1 );

					float tintFade = xmlChunk.attribute( "tint_fade" ).as_float();
					sts.TheChunkSettings[ idx ].TintFade = tintFade;

					pugi::xml_attribute tintFadeStrengthAttrib = xmlChunk.attribute( "tint_fade_strength" );
					if( !tintFadeStrengthAttrib.empty() )
					{
						sts.TheChunkSettings[ idx ].TintFadeStrength = tintFadeStrengthAttrib.as_float();
					}

					int isStiff = xmlChunk.attribute( "is_stiff" ).as_int();
					sts.TheChunkSettings[ idx ].IsStiff = isStiff;

					xmlChunk = xmlChunk.next_sibling( "chunk" );
				}

				mExtraSettings.PushBack( sts );

				xmlEntry = xmlEntry.next_sibling("entry");
			}
		}
	}
	//------------------------------------------------------------------------

	if( r3d_access( GetGrassLibFile().c_str(), 0 ) )
	{
		return false ;
	}

	const r3dString path = GetGrassPath( GrazLib_FILE );

	r3dFile* fin = r3d_open( path.c_str(), "rb" );

	if( !fin )
		return false;

	AutoClose<r3dFile> autoClose = { fin }; (void)autoClose;

	char CheckSig[ sizeof Graz_SIG2 ];
	fread( CheckSig, sizeof CheckSig, 1, fin );

	mVersion = 2;

	if (strcmp( CheckSig, Graz_SIG_CUR ))
	{
		r3dOutToLog( "GrassLib::Load: unsupported version: expecting %s but got %s!\n", Graz_SIG_CUR, CheckSig );
		return false;
	}

	uint32_t entryCount;

	if( fread_be( entryCount, fin ) != 1 )
		return false;

	mEntries.Resize( entryCount );

	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		GrassLibEntry& en = mEntries[ i ];

		if( !ReadString( en.Name, fin ) )
			return false;

		if( const GrassPatchGenSettings * sts = g_pGrassGen->GetPatchSettings( en.Name ) )
		{
			en.FadeDistance = sts->FadeDistance ;
		}
		else
		{
			r3dOutToLog( "Warning: couldn't find grass settings '%s'\n", en.Name.c_str() ) ;
			en.FadeDistance = 1.0f ;
		}

		//	Lazy loading is supported. Read chunk file offsets only.
		long entrySize = 0;
		fread_be(entrySize, fin);
		//	Record file offset for this entry
		en.fileStartOffset = ftell(fin);
		//	Seek to the next entry
		fseek(fin, entrySize, SEEK_CUR);
	}
	return true;
}

//------------------------------------------------------------------------

INT64 GrassLib::GetStamp() const
{
	const r3dString& path = GetGrassPath( GrazLib_FILE );
	return r3d_fstamp( path.c_str() ) ;
}

//------------------------------------------------------------------------

void
GrassLib::Unload()
{
	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		ReleaseEntry( mEntries[ i ] );
	}

	mEntries.Clear();
}

//------------------------------------------------------------------------

void
GrassLib::ReloadTextures()
{
	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		GrassLibEntry& en = mEntries[ i ];

		for( uint32_t i = 0, e = en.Chunks.Count(); i < e; i ++ )
		{
			grReloadTexture( en.Chunks[ i ].Texture );
		}
	}
}

//------------------------------------------------------------------------

const GrassLibEntry&
GrassLib::GetEntry( const r3dString& name )
{
	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		if( mEntries[ i ].Name == name )
			return GetEntry(i);
	}

	r3d_assert( false );
	return GetEntry(0);
}

//------------------------------------------------------------------------

GrassLibEntry&
GrassLib::GetEntryInternal( uint32_t idx )
{
	r3d_assert(idx < mEntries.Count());
	GrassLibEntry & en = mEntries[ idx ];
	if (en.Chunks.Count() == 0)
	{
		bool rv = LoadGrassLibEntry(en);
		r3d_assert(rv);
	}
	return en;
}

//------------------------------------------------------------------------

const GrassLibEntry&
GrassLib::GetEntry( uint32_t idx )
{
	return GetEntryInternal(idx);
}

//------------------------------------------------------------------------

uint32_t
GrassLib::GetEntryCount() const
{
	return mEntries.Count();
}

//------------------------------------------------------------------------

uint32_t
GrassLib::GetEntryIdxByName( const r3dString& name ) const
{
	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		if( !strcmpi( mEntries[ i ].Name.c_str(), name.c_str() ) )
			return i;
	}

	return uint32_t( -1 );
}

//------------------------------------------------------------------------

const r3dString&
GrassLib::GetEntryNameByIdx( uint32_t idx )
{
	r3d_assert( idx < mEntries.Count() );

	return mEntries[ idx ].Name;
}

//------------------------------------------------------------------------

const GrassLib::Settings&
GrassLib::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
GrassLib::UpdateSettings( const Settings& settings )
{
	bool do_map_update = false;

	if( settings.CellScale != mSettings.CellScale )
	{
		do_map_update = true;
	}

	mSettings = settings;

	if( do_map_update )
	{
		g_pGrassGen->GenerateAll();
		g_pGrassMap->ConformWithNewCellSize();
	}
}


//------------------------------------------------------------------------

void
GrassLib::UpdateEntry( const GrassLibEntry& entry )
{
	// do we have that? 
	uint32_t idx = GetEntryIdxByName( entry.Name );
	if( idx != uint32_t(-1) )
	{
		GrassLibEntry& existingEntry = mEntries[ idx ];

		ProcessCustomDeviceQueueItem( InMainThread, &existingEntry ) ;

		existingEntry = entry;
	}
	else
	{
		mEntries.PushBack( entry );
	}
}

//------------------------------------------------------------------------

void
GrassLib::MildUpdateExistingEntry( const GrassLibEntry& entry )
{
	uint32_t idx = GetEntryIdxByName( entry.Name );

	r3d_assert( idx != uint32_t(-1) );

	GrassLibEntry& toReplace = GetEntryInternal(idx);

	r3d_assert( entry.Chunks.Count() == toReplace.Chunks.Count() );

	for( uint32_t i = 0, e = entry.Chunks.Count(); i < e; i ++ )
	{
		const GrassChunk& gc0 = entry.Chunks[ i ];
		const GrassChunk& gc1 = toReplace.Chunks[ i ];
		r3d_assert( gc0.IndexBuffer == gc1.IndexBuffer );
		r3d_assert( gc0.NumVariations == gc1.NumVariations );
		r3d_assert( !memcmp( gc0.VertexBuffs, gc1.VertexBuffs, sizeof( void* ) * gc0.NumVariations  ) );
	}

	toReplace = entry;
}


//------------------------------------------------------------------------

r3dPoint3D GetGrassChunkScale()
{
	float cellScale = g_pGrassLib->GetSettings().CellScale;
	float meshScale = g_pGrassLib->GetSettings().MaxMeshScale;

	return r3dPoint3D( cellScale + meshScale, meshScale * 2, cellScale + meshScale );
}


//------------------------------------------------------------------------


r3dString GetGrassPath( const char* file )
{
	r3dString str( "Data/Grass/");

	str += file;

	return str;
}

//------------------------------------------------------------------------

r3dString	GetGrassLibFile()
{
	return GetGrassPath( GrazLib_FILE );
}

//------------------------------------------------------------------------

namespace
{
	void ReleaseEntry( GrassLibEntry& entry )
	{
		for( uint32_t i = 0, e = entry.Chunks.Count(); i < e; i ++ )
		{
			GrassChunk& chunk =  entry.Chunks[ i ];

			delete chunk.IndexBuffer;	chunk.IndexBuffer = NULL;

			for( uint32_t i = 0, e = chunk.NumVariations; i < e; i ++ )
			{
				delete chunk.VertexBuffs[ i ];
				chunk.VertexBuffs[ i ] = NULL;
			}

			chunk.NumVariations = 0;

			// texture may be used by editor etc. so don't delete it
#if 0
			r3dRenderer->DeleteTexture( chunk.Texture );
#endif
			chunk.Texture = NULL;
		}

		// death is near, where's my fear?
		entry.Name = "";
	}
}