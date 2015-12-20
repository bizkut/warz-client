#pragma once

enum
{
	INVALID_DECAL_ID = -1
};

struct DecalFileRecord
{
	DecalFileRecord();

	int			TypeID;
	r3dPoint3D	Pos;
	r3dPoint3D	Dir;
	float		LifeTime;
	float		ZRot;
	float		ScaleCoef;
};

struct DecalParams: public DecalFileRecord
{
	DecalParams(): Static(0) {}
	int Static;
};

enum
{
	DECAL_MAX_SCALE_COEF = 8
};

struct DecalType
{
	DecalType();

	r3dString		Name;

	r3dTexture* DiffuseTex;
	r3dTexture* NormalTex;

	r3dString		DiffuseTexName;
	r3dString		NormalTexName;

	float		UVStart[2];
	float		UVEnd[2];

	float		LifeTime;
	float		ScaleX;
	float		ScaleY;

	int			UniformScale ;

	float		ScaleVar;
	int			RandomRotation;
	float		BackgroundBlendFactor;
	float		ClipFarFactor;
	float		ClipNearFactor;

	int			MinQuality;
	bool		globalDecal;
};

bool operator != ( const DecalType& t0, const DecalType& t1 );

#pragma pack(push)
#pragma pack(1)

struct DecalVertex
{
	float3		pos;
	ushort		pos_xy_scale_x ;
	ushort		pos_xy_scale_y ;

	ubyte		corner_x;
	ubyte		corner_y;
	ubyte		z_rot;
	ubyte		dummy2;

	ubyte		dir_x;
	ubyte		dir_y;
	ubyte		dir_z;
	ubyte		time_left;

	ubyte		tex_tile_scale_u;
	ubyte		tex_tile_scale_v;
	ubyte		tex_tile_u;
	ubyte		tex_tile_v;

	ubyte		background_blend_factor;
	ubyte		clip_far_factor;
	ubyte		clip_near_factor;
	ubyte		unusued2 ;
};

#pragma pack(pop)

class DecalGameObjectProxy;

class DecalChief
{
	// types
public:
	enum
	{
		MAX_DECALS			= 2048,
		PROXIMITY_BUFF_SIZE = 32
	};	
	
	typedef r3dTL::TArray< DecalType > DecalTypes;
	typedef r3dTL::TArray< DecalParams > Decals;
	typedef r3dTL::TArray< DecalVertex > Vertices;
	typedef r3dTL::TArray< UINT16 > Indices;
	typedef r3dTL::TArray< UINT64 > TexCoupleIDs;
	typedef r3dTL::TFixedArray< UINT64, MAX_DECALS >				DecalTexCplIDs;
	typedef r3dTL::TFixedArray< UINT16, MAX_DECALS >				DecalVIdxes;

	struct ProximityEntry
	{
		ProximityEntry();

		r3dPoint3D	Pos;
		float		LifeTime;
		float		Radius;
	};

	typedef r3dTL::TFixedArray< ProximityEntry, PROXIMITY_BUFF_SIZE >	ProximityBuffer;

	// relative to decal size
	static const float MIN_PROXIMITY;

	struct Settings
	{
		Settings();

		float	MaxDecalSize;
		float	ReliefMappingDepth;
		float	AlphaRef;
	};

	// construction/ destruction
public:
	DecalChief();
	~DecalChief();

	// manipulation/ access
public:
	void	AddType( const r3dString& typeName );
	void	AddType( const DecalType& type );

	void	RemoveType( const r3dString& typeName );

	uint32_t			GetTypeCount() const;
	const DecalType&	GetTypeByIdx( uint32_t idx ) const;
	void				UpdateType( uint32_t idx, const DecalType& type );
	void				UpdateAll();

	uint32_t			GetStaticDecalCount( uint32_t typeIdx ) const;
	uint32_t			GetDecalCount() const;
	const DecalParams*	GetStaticDecal( uint32_t typeIdx, uint32_t idx ) const;
	void				UpdateStaticDecal( const DecalParams& parms, uint32_t idx );
	void				RemoveStaticDecal( uint32_t typeIdx, uint32_t idx );
	void				RemoveStaticDecalsOfType( uint32_t typeIdx ) ;

	int					PickStaticDecal( const r3dPoint3D& start, const r3dPoint3D& ray, float len ) const;

	const DecalParams&	GetDecal( uint32_t idx ) const;
	int					GetDecalIdxInType( int idx ) const;

	const Settings&		GetSettings() const;
	void				SetSettings( const Settings& settings );

	int		GetTypeID( const r3dString& typeName );

	int		Add( const DecalParams& params, bool checkProximity = true );
	void	Remove( int idx );
	void	Move( int idx, const r3dPoint3D& pos, const r3dPoint3D& norm );

	bool	LoadLib(const r3dString& levelPath);
	void	SaveLib(const r3dString& levelPath) const;
	void	SaveLocalLib(const r3dString& levelPath) const;

	bool	LoadLevel( const r3dString& levelPath );
	void	SaveLevel( const r3dString& levelPath ) const;

	void	UnloadTextures( bool staticOnly );
	void	LoadTextures( bool staticOnly );

	void	ReloadTextures();
	void	UpdateTexturesForQualitySettings();

	void	Update();

	void	Draw();
	void	DebugDraw();

	void	Init();
	void	Close();

	void	AutoLoadTextures( int idx );

	// helpers
private:
	bool	LoadLibInternal(const r3dString &libPath, bool globalLib);
	void	SaveLibInternal(const r3dString &libPath, bool globalLib) const;

	void	UpdateTexCpls();
	void	UpdateRegisteredCpls();
	void	UpdateDecal( int idx, const DecalType& type );
	void	AutoLoadTextures( DecalType& type );

	// data
private:
	DecalTypes						mTypes;
	Decals							mDecals;

	class r3dVertexBuffer*			mVB;
	class r3dIndexBuffer*			mIB;

	uint32_t							mVBOffset;
	uint32_t							mIBOffset;
	uint32_t							mUsedVCount;

	Vertices						mVertices;
	Indices							mIndices;

	DecalVIdxes						mDecalVIdxes;
	DecalTexCplIDs					mDecalTexCplIDs;

	TexCoupleIDs					mRegisteredTexCplIDs;
	TexCoupleIDs					mTypeTexCplIDs;

	IDirect3DVertexDeclaration9*	mVDecl;

	r3dTexture*						mNoiseTexture;

	float							mLastTime;

	ProximityBuffer					mProximityBuffer;
	uint32_t						mProximityBufferPtr;

	uint32_t						mProximitySkipCount;

	uint32_t						mAmmountStaticTexturesLoaded;
	uint32_t						mAmmountDynamicTexturesLoaded;

	uint32_t						mLastTexDownScale ;
	uint32_t						mLastLoadDecoQL ;

	r3dString							mCurLibName ;

	Settings						mSettings;
};
extern DecalChief* g_pDecalChief;
