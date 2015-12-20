#pragma once

#include "r3dTypeTuples.h"

#include "GrassLib.h"

struct GrassLibEntry;

typedef r3dgameVector(r3dSTLString) string_vec;

#pragma pack(push)
#pragma pack(1)

#pragma pack(pop)

struct GrassChunkGenSettings
{
	GrassChunkGenSettings();

	r3dString		MeshName;
	r3dString		TextureName;
	int			NumVariations;
	int			Density;
	float		MeshScale;
	float		MinMeshScaling;
	float		MaxMeshScaling;
	float		RotationVariation;
	float		AlphaRef;
	float		TintStrength;
	float		TintFadeStrength;
	float		TintFade;

	int			IsStiff;
};

struct GrassPatchGenSettings
{
	typedef r3dTL::TArray< GrassChunkGenSettings > ChunkSettingsVec;

	GrassPatchGenSettings();

	r3dString				Name;

	float				Density;
	float				FadeDistance;

	ChunkSettingsVec	ChunkSettings;
};

class GrassGen
{
	// types & constants
public:
	static const uint32_t MAX_MESHES_PER_BATCH = 4096;

	typedef r3dTL::TArray< GrassPatchGenSettings > PatchSettingsVec;

	// construction/ destruction
public:
	GrassGen();
	~GrassGen();

	// manipulation/ access
public:
	bool					Load();

	bool					IsNewerThan( INT64 stamp );

	void					Save();
	void					Save( const r3dString& entryName );

	bool					GenerateAll();
	bool					Generate( uint32_t idx );

	void					FillTypes( string_vec& oVec );

	uint32_t						GetTypeCount() const;
	const GrassPatchGenSettings&	GetPatchSettings( uint32_t idx ) const;
	const GrassPatchGenSettings*	GetPatchSettings( const r3dString& name ) const;
	void							SetPatchSettings( uint32_t idx, const GrassPatchGenSettings& sts );

	int								IsDirty() const ;

	// helpers
private:
	bool					AddSettingsFromFile( const r3dString& fname );
	bool					SaveSettingsToFile( const GrassPatchGenSettings& settings ) const;

	// data
private:
	PatchSettingsVec	mPatchSettings;
	int					mDirty ;

};
extern GrassGen* g_pGrassGen;

extern char g_LastGrassGenError[ 512 ];

void GrassGenSettingsToChunkSettings( struct GrassChunk& oChunk, const GrassChunkGenSettings& chunkSettings );