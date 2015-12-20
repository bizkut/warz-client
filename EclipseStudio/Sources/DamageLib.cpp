#include "r3dPCH.h"
#include "r3d.h"

#include "DamageLib.h"

DamageLib* g_DamageLib ;

//------------------------------------------------------------------------

DamageLibEntry::DamageLibEntry()
: UseDestructionMesh( true )
, HitPoints( 100 )
, HasParicles( false )
, HasSound( false )
, HasAnimation( false )
, DestroyedMeshCollision( false )
{

}

//------------------------------------------------------------------------

DamageLib::DamageLib()
{
	
}

//------------------------------------------------------------------------

DamageLib::~DamageLib()
{

}

//------------------------------------------------------------------------

DamageLib::Entry*
DamageLib::GetEntry( const string& key )
{
	Data::iterator found = mData.find( key );

	if( found != mData.end() )
		return &found->second ;
	else
		return NULL ;
}

//------------------------------------------------------------------------

DamageLib::Entry*
DamageLib::GetEntry( const string& category, const string& meshName )
{
	return GetEntry( ComposeKey( category, meshName ) );
}

//------------------------------------------------------------------------

#define DAMAGE_FILE_NAME "Data/Damage.xml"

int
DamageLib::Load()
{
	r3dFile* f = r3d_open( DAMAGE_FILE_NAME, "rb" );
	if ( ! f )
		return 0;

	char* fileBuffer = game_new char[f->size + 1];
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlFile;
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);

	fclose(f);

	if( !parseResult )
		r3dError("Failed to parse XML, error: %s", parseResult.description());

	pugi::xml_node xmlLib = xmlFile.child( "damage_lib" );

	pugi::xml_node xmlEntry = xmlLib.child( "entry" );

	while( !xmlEntry.empty() )
	{
		Entry en ;

		string name			= xmlEntry.attribute( "name" ).value();

		pugi::xml_attribute useMeshAttrib = xmlEntry.attribute( "use_destruction_mesh" );

		if( !useMeshAttrib.empty() )
		{
			en.UseDestructionMesh = useMeshAttrib.as_int();
		}

		en.MeshName			= xmlEntry.attribute( "destruction_mesh" ).value();
		en.ParticleName		= xmlEntry.attribute( "destruction_particles" ).value();

		en.HitPoints		= xmlEntry.attribute( "hit_points" ).as_int() ;
		en.HasParicles		= xmlEntry.attribute( "has_particles" ).as_int() ;

		en.HasParicles		= !!en.HasParicles ;

		if( !en.ParticleName.Length() )
		{
			en.HasParicles = 0 ;
		}

		en.SoundName		= xmlEntry.attribute( "sound" ).value();

		if( !en.SoundName.Length() )
		{
			en.HasSound = 0 ;
		}

		en.HasSound			= xmlEntry.attribute( "has_sound" ).as_int();

		en.HasSound			= !!en.HasSound ;

		en.HasAnimation		= xmlEntry.attribute( "has_animation" ).as_int();
		en.HasAnimation		= !!en.HasAnimation;

		en.AnimName			= xmlEntry.attribute( "animation" ).value();

		en.DestroyedMeshCollision	= xmlEntry.attribute( "custom_collision" ).as_int();
		en.DestroyedMeshCollision	= !!en.DestroyedMeshCollision;

		if( !en.AnimName.Length() )
		{
			en.HasAnimation = 0;
		}

		if( mData.find( name ) != mData.end() )
		{
			r3dError( "Duplicate entry %s in %s", name.c_str(), DAMAGE_FILE_NAME );
		}

		mData[ name ] = en ;
	
		xmlEntry = xmlEntry.next_sibling();
	}

	delete [] fileBuffer;

	return 1 ;
}

//------------------------------------------------------------------------

void
DamageLib::Save() const
{
	pugi::xml_document xmlFile;
	xmlFile.append_child( pugi::node_comment ).set_value("Damage Lib");
	
	pugi::xml_node xmlLib = xmlFile.append_child();
	xmlLib.set_name( "damage_lib" );

	for( Data::const_iterator i = mData.begin(), e = mData.end(); i != e ; ++ i )
	{
		pugi::xml_node xmlEntry = xmlLib.append_child() ;

		xmlEntry.set_name( "entry" );

		const Entry& en = i->second ;

		xmlEntry.append_attribute( "name"					)	= i->first.c_str();

		xmlEntry.append_attribute( "use_destruction_mesh"	)	= en.UseDestructionMesh;
		xmlEntry.append_attribute( "destruction_mesh"		)	= en.MeshName;
		xmlEntry.append_attribute( "destruction_particles"	)	= en.ParticleName;

		xmlEntry.append_attribute( "has_particles"			)	= en.HasParicles;

		xmlEntry.append_attribute( "has_sound"				)	= en.HasSound;
		xmlEntry.append_attribute( "sound"					)	= en.SoundName;

		xmlEntry.append_attribute( "has_animation"			)	= en.HasAnimation;
		xmlEntry.append_attribute( "animation"				)	= en.AnimName;

		xmlEntry.append_attribute( "custom_collision"		)	= en.DestroyedMeshCollision;

		xmlEntry.append_attribute( "hit_points"				)	= en.HitPoints;
	}

	xmlFile.save_file( DAMAGE_FILE_NAME );
}

//------------------------------------------------------------------------

void
DamageLib::RemoveEntry( const string& key )
{
	Data::iterator found = mData.find( key );

	r3d_assert( found != mData.end() );

	mData.erase( found );
}

//------------------------------------------------------------------------

void
DamageLib::AddEntry( const string& key )
{
	r3d_assert( mData.find( key )  == mData.end() );

	mData.insert( Data::value_type( key, Entry() ) );
}

//------------------------------------------------------------------------

bool
DamageLib::Exists( const string& key )
{
	return mData.find( key ) != mData.end() ;
}

//------------------------------------------------------------------------

bool
DamageLib::Exists( const string& category, const string& meshName )
{
	return mData.find( ComposeKey( category, meshName ) ) != mData.end() ;
}

//------------------------------------------------------------------------

void
DamageLib::GetEntryNames( stringlist_t* oNames ) const
{
	oNames->clear();

	for( Data::const_iterator i = mData.begin(), e = mData.end() ; i != e ; ++ i )
	{
		oNames->push_back( i->first );
	}
}

//------------------------------------------------------------------------
/*static*/

DamageLib::string
DamageLib::ComposeKey( const string& category, const string& meshName )
{
	string res = category + ":" + meshName ;

	char* temp = (char*)_alloca( res.size() + 1 );

	r3d_assert( temp );

	strcpy( temp, res.c_str() );
	strlwr( temp );

	return temp ;
}

//------------------------------------------------------------------------
/*static*/

bool
DamageLib::DecomposeKey( const string& key, string* oCat, string* oMeshName )
{
	size_t pos = key.find(":") ;
	if( pos != string::npos )
	{
		*oCat		= string( key.begin(), key.begin() + pos );
		*oMeshName	= string( key.begin() + pos + 1, key.end() );

		return true ;
	}

	return false ;
}