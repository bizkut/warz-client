#pragma once

struct GrassVertex
{
	short4 pos;
	short2 texc;
	ubyte4 normal;
};

struct GrassChunk
{
	enum
	{
		MAX_VARIATIONS	= 5
	};

	GrassChunk();
	~GrassChunk();

	int						NumVariations;

	int						VerticesPerItem ;
	int						IndicesPerItem ;

	r3dVertexBuffer*		VertexBuffs[ MAX_VARIATIONS ];

	r3dIndexBuffer*			IndexBuffer;

	r3dTexture*				Texture;

	float					AlphaRef;

	float					TintStrength;

	float					TintFade;

	float					TintFadeStrength;

	int						IsStiff;

	typedef r3dTL::TArray<GrassVertex>	Vertices ;
	typedef r3dTL::TArray<UINT16>		Indices ;

	// filled only when generated - not loaded
	Indices					SysmemIndices ;
	Vertices				SysmemVertices[ MAX_VARIATIONS ] ;

	static LPDIRECT3DVERTEXDECLARATION9 VDecl;
};

typedef r3dTL::TArray< GrassChunk > GrassChunks;

struct GrassLibEntry
{
	GrassChunks				Chunks;

	r3dString		Name;
	float			FadeDistance;
	/**	File offset to the entry start record in grass library binary file. It is used for lazy entry loading. */
	long			fileStartOffset;
	GrassLibEntry(): fileStartOffset(0), FadeDistance(1.0f) {}
};

class GrassLib
{
	// types
public:
	typedef r3dTL::TArray< GrassLibEntry > GrassLibEntries;

	struct ExtraSettings
	{
		struct ChunkSettings
		{
			float TintFade;
			float TintFadeStrength;
			int IsStiff;

			ChunkSettings();
		};

		r3dString name;

		r3dTL::TArray< ChunkSettings > TheChunkSettings;
	};

	typedef r3dTL::TArray< ExtraSettings > ExtraSettingsArr;

	struct Settings
	{
		Settings();

		float	CellScale;
		float	MaxMeshScale;
		float	UniformMaskThreshold;
		float	BlankMaskThreshold;
		float	MaxBlankMaskThreshold;
	};

	// construction/ destruction
public:
	GrassLib();
	~GrassLib();

public:
	bool Save();
	bool Load();

	INT64 GetStamp() const ;

	void Unload();

	void ReloadTextures();

	const GrassLibEntry&	GetEntry( const r3dString& name );
	const GrassLibEntry&	GetEntry( uint32_t idx );

	uint32_t				GetEntryCount() const;
	uint32_t				GetEntryIdxByName( const r3dString& name ) const;
	const r3dString&		GetEntryNameByIdx( uint32_t idx );

	const Settings&			GetSettings() const;
	void					UpdateSettings( const Settings& settings );

	void					UpdateEntry( const GrassLibEntry& entry );
	void					MildUpdateExistingEntry( const GrassLibEntry& entry );

private:
	GrassLibEntries		mEntries;

	ExtraSettingsArr	mExtraSettings;

	Settings			mSettings;

	int					mVersion;

	bool					LoadGrassLibEntry(GrassLibEntry &en);
	bool					LoadGrassLibEntry(GrassLibEntry &en, r3dFile *fin);
	/**	Nonconst version for internal use. */
	GrassLibEntry&			GetEntryInternal( uint32_t idx );

};
extern GrassLib* g_pGrassLib;

r3dPoint3D	GetGrassChunkScale();
r3dString	GetGrassPath( const char* file );
r3dString	GetGrassLibFile();