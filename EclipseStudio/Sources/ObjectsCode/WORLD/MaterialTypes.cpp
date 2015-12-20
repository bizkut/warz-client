#include "r3dPCH.h"
#include "r3d.h"

#include "DecalChief.h"

#include "MaterialTypes.h"

#include "GameCommon.h"
#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "Particle.h"

MaterialTypes* g_pMaterialTypes = 0;

// array of all impact particles, to reference them with id
static ImpactParticleEntry* g_ImpactParticles[1024] = {0};
	uint32_t	ImpactParticleEntry::NumRegistered = 0;

DecalMapEntry::DecalMapEntry()
{
}

//------------------------------------------------------------------------

MaterialTypes::MaterialTypes()
{

}

//------------------------------------------------------------------------

MaterialTypes::~MaterialTypes()
{

}

//------------------------------------------------------------------------
const MaterialType* MaterialTypes::Get( uint32_t hashName ) const
{
	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		const Entry& entry = mEntries[ i ];
		if( entry.nameHash == hashName)
			return &entry.type;
	}

	return NULL;
}

//------------------------------------------------------------------------

const MaterialType* MaterialTypes::GetByIdx( uint32_t idx ) const
{
	if( idx < mEntries.Count() )
	{
		return &mEntries[ idx ].type;
	}
	else
	{
		return &mDefaultMaterial;
	}
}

//------------------------------------------------------------------------

uint32_t
MaterialTypes::GetIdx( const r3dString& name ) const
{
	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		if( mEntries[ i ].name == name )
			return i;
	}

	return uint32_t( -1 );
}

//------------------------------------------------------------------------

const uint32_t DEFAULT_DECAL_SOURCE_HASH = r3dHash::MakeHash("default");

int DeriveDecalID( const DecalMapEntry& en )
{
	return en.DecalIDs.Count() ? en.DecalIDs[ rand() % en.DecalIDs.Count() ] : INVALID_DECAL_ID;
}


namespace
{
	const char* MATERIAL_CFG_PATH = "data\\MaterialTypes.xml";

	void FillDecalEntry( DecalMapEntry& oEn, pugi::xml_node node )
	{
		oEn.SourceNameHash	= r3dHash::MakeHash(node.attribute( "source" ).value());

		for( pugi::xml_node::iterator i = node.begin(), e = node.end(); i != e; ++ i )
		{
			const char* name = i->name();
			if(strstr(name, "Impact_Particle"))
			{
				ImpactParticleEntry ParticleEn;
				ParticleEn.name = i->attribute("name").value();
#ifndef WO_SERVER
				ParticleEn.sndID = SoundSys.GetEventIDByPath(i->attribute("sndID").value());
#endif
				oEn.ImpactParticles.PushBack(ParticleEn);
			}
			else			
			{
				int id = g_pDecalChief->GetTypeID( name );

				if( id != -1 )
				{
					oEn.DecalNames.PushBack( name );
					oEn.DecalIDs.PushBack( id );
				}
			}
		}
	}

	const ImpactParticleEntry* DeriveImpactParticle( const DecalMapEntry& en )
	{
		if(en.ImpactParticles.Count())
			return &en.ImpactParticles[ rand() % en.ImpactParticles.Count() ];
		return NULL;
	}
}

const MaterialType*
MaterialTypes::GetDefaultMaterial() const
{
	return &mDefaultMaterial;
}

int MaterialTypes::GetDefaultDecal( uint32_t sourceHash ) const
{
	const DecalMapEntry* defEntry = NULL;

	for( uint32_t i = 0, e = mDefaultMaterial.entries.Count(); i < e; i ++ )
	{
		const DecalMapEntry& en = mDefaultMaterial.entries[ i ];
		if( en.SourceNameHash == sourceHash )
		{
			return DeriveDecalID( en );
		}
		else
		if( en.SourceNameHash == DEFAULT_DECAL_SOURCE_HASH )
		{
			defEntry = & en;
		}
	}

	if( defEntry )
	{
		return DeriveDecalID( *defEntry );
	}

	return INVALID_DECAL_ID;
}


const ImpactParticleEntry* MaterialTypes::GetDefaultParticle( uint32_t sourceHash ) const
{
	const DecalMapEntry* defEntry = NULL;

	for( uint32_t i = 0, e = mDefaultMaterial.entries.Count(); i < e; i ++ )
	{
		const DecalMapEntry& en = mDefaultMaterial.entries[ i ];
		if( en.SourceNameHash == sourceHash )
		{
			return DeriveImpactParticle( en );
		}
		else
			if( en.SourceNameHash == DEFAULT_DECAL_SOURCE_HASH )
			{
				defEntry = & en;
			}
	}

	if( defEntry )
	{
		return DeriveImpactParticle( *defEntry );
	}

	return NULL;
}

static void FillImpactArray(MaterialType& mt)
{
	for(uint32_t eId = 0; eId < mt.entries.Count(); eId++)
	{
		DecalMapEntry& dme = mt.entries[eId];
		for(uint32_t i = 0; i < dme.ImpactParticles.Count(); i++)
		{
			dme.ImpactParticles[i].impactIdx = ImpactParticleEntry::NumRegistered;
			
			r3d_assert(ImpactParticleEntry::NumRegistered < 1024);
			g_ImpactParticles[ImpactParticleEntry::NumRegistered] = &dme.ImpactParticles[i];
			ImpactParticleEntry::NumRegistered++;
		}
	}
}

//------------------------------------------------------------------------
bool MaterialTypes::Load()
{
	pugi::xml_attribute attrib;

	r3dFile* f = r3d_open( MATERIAL_CFG_PATH, "rb" );
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
		r3dArtBug( "MaterialTypes::Load: Failed to parse %s, error: %s", MATERIAL_CFG_PATH, parseResult.description() );
		return false;
	}

	mEntries.Clear();
	mDefaultMaterial.entries.Clear();
	mDefaultMaterial.footStepsSound = -1;

	pugi::xml_node root = xmlLevelFile.child( "root" );

	pugi::xml_node xmlDefDecs = root.child( "def_decals" );

	pugi::xml_node xmlDefDec = xmlDefDecs.child( "def_decal" );

	while( !xmlDefDec.empty() )
	{
		DecalMapEntry en;

		FillDecalEntry( en, xmlDefDec );

		for( uint32_t i = 0, e = mDefaultMaterial.entries.Count(); i < e; i ++ )
		{
			if( mDefaultMaterial.entries[ i ].SourceNameHash == en.SourceNameHash )
			{
				r3dError( "MaterialTypes::Load: duplicate default decal with hash '%d'!", en.SourceNameHash );
			}
		}

		mDefaultMaterial.entries.PushBack( en );

		xmlDefDec		= xmlDefDec.next_sibling();
	}

	pugi::xml_node xmlTypes = root.child( "types" );

	pugi::xml_node xmlType = xmlTypes.child( "type" );

	while( !xmlType.empty() )
	{
		Entry en;

		en.name = xmlType.attribute( "name" ).value();
		en.nameHash = r3dHash::MakeHash(en.name.c_str());

		pugi::xml_node xmlEntry = xmlType.child( "entry" );

		while( !xmlEntry.empty() )
		{
			DecalMapEntry entry;

			FillDecalEntry( entry, xmlEntry );

			for( uint32_t i = 0, e = en.type.entries.Count(); i < e; i ++ )
			{
				if( en.type.entries[ i ].SourceNameHash == entry.SourceNameHash )
					r3dError( "MaterialTypes::Load: duplicate source with hashID '%d' entries...", entry.SourceNameHash );
			}

			en.type.entries.PushBack( entry );

			xmlEntry		= xmlEntry.next_sibling();
		}

		//	Footsteps sound
		xmlEntry = xmlType.child("footsteps");
#ifndef WO_SERVER
		en.type.footStepsSound = SoundSys.GetEventIDByPath(xmlEntry.attribute("sound").value());
		en.type.crouchFootstepsSound = SoundSys.GetEventIDByPath(xmlEntry.attribute("crouch").value());
		en.type.proneFootstepsSound = SoundSys.GetEventIDByPath(xmlEntry.attribute("prone").value());
#endif

#ifdef VEHICLES_ENABLED
		xmlEntry = xmlType.child("vehicle");
#ifndef WO_SERVER
		en.type.vehicleSound = SoundSys.GetEventIDByPath(xmlEntry.attribute("sound").value());
#endif
#endif

		// resources (gameplay)
		xmlEntry = xmlType.child("resources");
		if(!xmlEntry.empty())
		{
			const char* resourceType = xmlEntry.attribute("type").value();
			if(strcmp(resourceType, "wood")==0)
				en.type.hasResourcesToCollect = 1;
			else if(strcmp(resourceType, "stone")==0)
				en.type.hasResourcesToCollect = 2;
			else if(strcmp(resourceType, "metal")==0)
				en.type.hasResourcesToCollect = 3;
			else
				r3dOutToLog("!!! unknown resource (%s) in material (%s)\n", resourceType, en.name.c_str());
		}

		mEntries.PushBack( en );


		xmlType = xmlType.next_sibling();
	}
	
	// make a direct array references to impact particle
	ImpactParticleEntry::NumRegistered = 0;
	FillImpactArray(mDefaultMaterial);
	for(uint32_t i = 0; i < mEntries.Count(); i++)
		FillImpactArray(mEntries[i].type);
	
	return true;

}

//------------------------------------------------------------------------
MaterialTypes::Strings MaterialTypes::GetTypeNames() const
{
	Strings result;

	for( uint32_t i = 0, e = mEntries.Count(); i < e; i ++ )
	{
		result.push_back( mEntries[ i ].name.c_str() );
	}

	return result;
}

//------------------------------------------------------------------------
bool MaterialTypes::CheckNeedReload( const char* fileName ) const
{
	return r3dIsSamePath(fileName, MATERIAL_CFG_PATH);
}

//------------------------------------------------------------------------
int GetDecalID( uint32_t matTypeHash, uint32_t sourceHash )
{
	const DecalMapEntry* defEntry = NULL;

	if( const MaterialType* type = g_pMaterialTypes->Get( matTypeHash ) )
	{
		for( uint32_t i = 0, e = type->entries.Count(); i < e; i ++ )
		{
			const DecalMapEntry& en = type->entries[ i ];

			if( en.SourceNameHash == sourceHash )
			{
				return DeriveDecalID( en );
			}
			else
			if( en.SourceNameHash == DEFAULT_DECAL_SOURCE_HASH )
			{
				defEntry = & en;
			}
		}
	}

	if( defEntry )
	{
		return DeriveDecalID( *defEntry );
	}

	return g_pMaterialTypes->GetDefaultDecal( sourceHash );
}

const ImpactParticleEntry* GetImpactParticle( uint32_t matTypeHash, uint32_t sourceHash )
{
	const DecalMapEntry* defEntry = NULL;

	if( const MaterialType* type = g_pMaterialTypes->Get( matTypeHash ) )
	{
		for( uint32_t i = 0, e = type->entries.Count(); i < e; i ++ )
		{
			const DecalMapEntry& en = type->entries[ i ];

			if( en.SourceNameHash == sourceHash )
			{
				return DeriveImpactParticle( en );
			}
			else
				if( en.SourceNameHash == DEFAULT_DECAL_SOURCE_HASH )
				{
					defEntry = & en;
				}
		}
	}

	if( defEntry )
	{
		return DeriveImpactParticle( *defEntry );
	}

	return g_pMaterialTypes->GetDefaultParticle( sourceHash );
}

void SpawnImpactParticle( uint32_t impactIdx, const r3dPoint3D& pos, const r3dPoint3D& norm )
{
	r3d_assert(impactIdx < ImpactParticleEntry::NumRegistered);
	
	const ImpactParticleEntry* entry = g_ImpactParticles[impactIdx];
	r3d_assert(entry);
	
	obj_ParticleSystem* em = NULL;
	em = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", entry->name.c_str(), pos);
	r3d_assert(em);
	if(em->Torch)
		em->Torch->Direction = norm;
	em->GlobalScale = 1.0f;
#ifndef WO_SERVER
	SoundSys.PlayAndForget(entry->sndID, pos);
#endif
	em->ObjFlags |= OBJFLAG_SkipCastRay;

	return;
}

int SpawnImpactParticle( uint32_t matTypeHash, uint32_t sourceHash, const r3dPoint3D& pos, const r3dPoint3D& norm )
{
	const ImpactParticleEntry* entry = GetImpactParticle(matTypeHash, sourceHash);
	if(entry == 0)
		return -1;
		
	r3d_assert(g_ImpactParticles[entry->impactIdx] == entry);
	SpawnImpactParticle(entry->impactIdx, pos, norm);
	return entry->impactIdx;
}
