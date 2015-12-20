#pragma once

struct ImpactParticleEntry
{
static	uint32_t NumRegistered;
	int impactIdx;
	r3dString name;
	int sndID;
};
struct DecalMapEntry
{
	DecalMapEntry();

	typedef r3dTL::TArray< r3dString > Strings;
	typedef r3dTL::TArray< ImpactParticleEntry > Particles;
	typedef r3dTL::TArray< int > Ints;

	uint32_t SourceNameHash;

	Strings DecalNames;
	Ints	DecalIDs;
	Particles ImpactParticles; // when you shoot at something, spawn those particles
};

struct MaterialType
{
	typedef r3dTL::TArray< DecalMapEntry > DecalEntries;

	DecalEntries	entries;
	int				footStepsSound;
	int				crouchFootstepsSound;
	int				proneFootstepsSound;
	int				hasResourcesToCollect; // if 0 - no resources. 1-wood, 2-stone, 3-metal

#ifdef VEHICLES_ENABLED
	int	vehicleSound;
	int vehicleCrashSound;
#endif

	MaterialType(): 
		footStepsSound(-1), crouchFootstepsSound(-1), proneFootstepsSound(-1), hasResourcesToCollect(0) 
#ifdef VEHICLES_ENABLED
		, vehicleSound(-1), vehicleCrashSound(-1)
#endif
	{}
};

class MaterialTypes
{
	// types
public:
	struct Entry
	{
		uint32_t		nameHash;
		r3dString		name; // for editor only
		MaterialType	type;
	};

	typedef r3dTL::TArray< Entry >			Entries;

	// 'editor compliant'
	typedef r3dgameVector(r3dSTLString )		Strings;


	// construction/ destruction
public:
	MaterialTypes();
	~MaterialTypes();

	// manipulation/ access
public:
	const MaterialType* 		Get( uint32_t nameHash ) const;
	const MaterialType* 		GetByIdx( uint32_t idx ) const;
	const MaterialType*			GetDefaultMaterial() const;

	uint32_t					GetIdx( const r3dString& name ) const;

	int							GetDefaultDecal( uint32_t sourceHash ) const;
	const ImpactParticleEntry*	GetDefaultParticle( uint32_t sourceHash ) const;
	
	bool						Load();

	Strings						GetTypeNames() const;

	bool						CheckNeedReload( const char* fileName ) const;

private:

	Entries			mEntries;
	MaterialType	mDefaultMaterial;

};
extern MaterialTypes* g_pMaterialTypes;
extern const uint32_t DEFAULT_DECAL_SOURCE_HASH;

int DeriveDecalID( const DecalMapEntry& en );

int GetDecalID( uint32_t matTypeHash, uint32_t sourceHash);
int SpawnImpactParticle(  uint32_t matTypeHash, uint32_t sourceHash, const r3dPoint3D& pos, const r3dPoint3D& norm );
void SpawnImpactParticle(  uint32_t impactIdx, const r3dPoint3D& pos, const r3dPoint3D& norm );
