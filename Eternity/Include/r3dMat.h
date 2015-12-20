#ifndef	__R3D_MAT_H
#define	__R3D_MAT_H

#include "r3dColor.h"
#include "r3dRender.h"

class r3dFile;

enum r3dMatFlags_e
{
	R3D_MAT_ADDOP					= (1<<8),
	R3D_MAT_SUBOP					= (1<<9),

	R3D_MAT_DOUBLESIDED				= (1<<21),
	R3D_MAT_FORCEHASALPHA			= (1<<22),
	R3D_MAT_HASALPHA				= (1<<23),
	R3D_MAT_TRANSPARENT				= (1<<24),
	R3D_MAT_CAMOUFLAGE				= (1<<25),
	R3D_MAT_TRANSPARENT_CAMOUFLAGE	= (1<<26),
	R3D_MAT_TRANSPARENT_CAMO_FP		= (1<<27),
	R3D_MAT_SKIP_DRAW				= (1<<28), // Do not draw batch with this material
	R3D_MAT_PLAYER					= (1<<29),
	R3D_MAT_THUMBNAILS				= (1<<30),
};

enum r3dMatVSType
{
	R3D_MATVS_DEFAULT,
	R3D_MATVS_DECALMASK,		// 2nd UV with multiplied texture
	R3D_MATVS_SKIN,
	R3D_MATVS_FOREST,
	R3D_MATVS_ANIMATEDFOREST,
	R3D_MATVS_FOREST_NONINSTANCED,
	R3D_MATVS_ANIMATEDFOREST_NONINSTANCED,
	R3D_MATVS_INSTANCING,
	R3D_MATVS_APEX_DESTRUCTIBLE,
	R3D_MATVS_INSTANCING_2UV,

	R3D_MATVS_COUNT
};

enum r3dMatPathFlags
{
	R3D_MATF_ROAD				= 0x01,
	R3D_MATF_FORCE_ALPHATEST	= 0x02,
	R3D_MATF_NO_SHADERS			= 0x08,
	R3D_MATF_NO_PIXEL_SHADER	= 0x10,
	R3D_MATF_INSTANCING			= 0x20
};

//////////////////////////////////////////////////////////////////////////

union FillbufferShaderKey
{
	uint8_t key;
	struct Flags
	{
		uint8_t low_q		: 1;
		uint8_t disp		: 1;
		uint8_t alpha_test	: 1;
		uint8_t roads		: 1;
		uint8_t camouflage	: 1;
		uint8_t simple		: 1;
		uint8_t instancing	: 1;
		uint8_t unused_		: 1;
	};

	Flags flags;
	FillbufferShaderKey(): key(0) {}
};

R3D_FORCEINLINE void SetLQ( FillbufferShaderKey* key )
{
	if( r_lighting_quality->GetInt() == 1 )
	{
		key->flags.low_q = 1;
	}
}

//------------------------------------------------------------------------

struct FillGBufferConstantId
{
	union
	{
		struct
		{
			UINT32 aux	: 1;
		};

		UINT32 Id;
	};

	FillGBufferConstantId();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 2 > FillGBufferConstantPSIds;

extern FillGBufferConstantPSIds gFillGBufferConstantPSIds;

//------------------------------------------------------------------------

/**	Pixel shader id storage. Modify size of this array when new pins are introduced. */
typedef r3dTL::TFixedArray<int, 256> FillbufferPixelShadersArr;
extern FillbufferPixelShadersArr gFillbufferPixelShaders;
void FillMacrosForShaderKey(FillbufferShaderKey k, ShaderMacros &outMacro);
r3dString ConstructShaderNameForKey(FillbufferShaderKey k);
extern int PS_TRANSPARENT_ID;

#define CAMOUFLAGE_LERP_DURATION 2.0f

//////////////////////////////////////////////////////////////////////////

class r3dMaterial
{
public:
	enum TexType
	{
		TEX_DIFFUSE,
		TEX_NORMAL,
		TEX_GLOSS,
		TEX_ENVPOWER,
		TEX_SELFILLUM,
		TEX_NORMALDETAIL,
		TEX_THICKNESS,
		TEX_CAMOUFLAGEMASK,
		TEX_DISTORTION,
		TEX_SPECULARPOW,
		TEX_DECALMASK,
		TEX_COUNT
	};

	int	      	ID;
	int         Flags;
	int			WasEdited;	// if it was touched by material editor
	int			AlphaFlagSet;

	int			RefCount;
	int			TextureRefCount;

	char        Name[ R3D_MAX_MATERIAL_NAME ];
	char        TypeName[ 32 ]; // from g_pMaterialType
	char        DepotName[ MAX_PATH ];
	char        OriginalDir[ 256 ];

	r3dColor	EmissiveColor;
	r3dColor	DiffuseColor;
	r3dColor	SpecularColor;

	r3dColor	FGlassColor; // FakeGlass

	r3dThumbnailTexture*	Texture;
	r3dTexture*				NormalTexture;
	r3dTexture*				DecalMaskTexture;
	r3dTexture*				GlossTexture;
	r3dTexture*				EnvPowerTexture;
	r3dTexture*				SelfIllumTexture;

	r3dTexture*				ThicknessTexture;
	r3dTexture*				CamouflageMask;
	r3dTexture*				DistortionTexture;

	r3dTexture*				NormalDetailTexture;
	r3dTexture*				SpecularPowTexture;

	float		DetailScale;
	float		DetailAmmount;

	int			m_DoDispl;
	float		m_DisplDepthVal;

	float		SpecularPower;
	float		SpecularPower1;
	float		ReflectionPower;

	/**	Low quality parameters replacements for textures. */
	float		lowQSelfIllum;
	float		lowQMetallness;

	float		AlphaRef;

	float		TransparencyMultiplier ;
	float		SelfIllumMultiplier ;

	float		SnowAmount;

	int			matID;
	/**	Camouflage data. */
	struct CamoData
	{
		r3dTexture *prevTex;
		r3dTexture *curTex;
		float transitionStartTime;
		CamoData(): prevTex(0), curTex(0), transitionStartTime(0) {}
	};

	static r3dColor RenderedObjectColor ;

private:
	void		Init( int useThumbnails );
	void	  	Reset();

	void					AdjustLoadPath( char* adjustedPath, const char* fname );
	r3dTexture*				MatLoadTexture(const char* fname, const char* slot, int DownScale, int MaxDim, bool player, bool deferLoading );
	r3dThumbnailTexture*	MatLoadThumbTexture(const char* fname, const char* slot, int DownScale, int MaxDim, bool player );

	void		StartCommonProperties();
	void		EndCommonProperties();

	void		StartTransparent();
	void		EndTransparent();


public:
	r3dMaterial( int useThumbnails );
	~r3dMaterial();

	static void	SetRenderedObjectColor( const r3dColor& color ) ;
	void		SetupCompoundDiffuse( D3DXVECTOR4 * outVec, int forTransparency ) ;

	int		LoadAscii(r3dFile *f, const char* szTexturePath);
	int		SaveAscii(FILE *f);

	int		CopyTextures(const char* outDir);

	void		ReloadTextures();

	void		Set();
	void		RenderPrimitive(int Start1, int End1, int Start2, int End2);

	static void	SetupTransparentStates( int UseSkins, int psID = PS_TRANSPARENT_ID );

	void		Start( r3dMatVSType VSType, UINT SetupFlags );
	void		End();

	void		StartShadows( r3dMatVSType VSType );
	void		EndShadows();

	void		SetAlphaFlag();

	void		TouchTextures();
	void		ReleaseTextures();

	void		GetTextures( r3dTexture* (&texArr)[ TEX_COUNT ] );
	void		SetTextures( r3dTexture* (&texArr)[ TEX_COUNT ] );

	void		DisableThumbnails();
	void		EnableThumbnails();

	static void ResetMaterialFilter();
	static void SetVertexShader( r3dMatVSType VSType, int DoDisplace );
	static void SetShadowVertexShader ( r3dMatVSType VSType );

};

enum
{
	MAX_MAT_FRAME_SCORES	= 4096,
	MAT_FRAME_SCORE_MASK	= 0xFFF
};

extern int gMatFrameScores[MAX_MAT_FRAME_SCORES];

void ResetMatFrameScores();

class r3dMaterialLibrary
{
public:
	static	void	Reset();
	static	int		UnloadManaged();
	static	void	Destroy();

	static	int	LoadLibrary(const char* szFileName, const char* szTexPath);
	static	int	SaveLibrary(const char* szFileName, const char* szTexPath, int bCompact=0);

	static int SaveMaterialSources();

	static	int& GetCurrent(r3dMaterial**& materialsArray);

	static  void ReloadMaterialTextures();

	static	r3dMaterial*	FindMaterial(const char* MatName);
	static	r3dMaterial*	HasMaterial(const char* MatName);
	static	r3dMaterial*	GetDefaultMaterial() ;

	// will try load material by depot mesh name
	static	bool		IsDynamic;
	static	r3dMaterial*	RequestMaterialByMesh(const char* MatName, const char* MeshName, bool playerMaterial, bool use_thumbnails );
	static	r3dMaterial*	RequestMaterialByName(const char* MatName, const char* depot, bool playerMaterial, bool use_thumbnails );
	static	void		UpdateDepotMaterials();

	static  void UnloadManagedMaterial( r3dMaterial* mat ) ;

	static void LoadExtendedMaterialParameters(const char * szFileName);
	static void SaveExtendedMaterialParameters(const char * szFileName);
	static void LoadDefaultExtendedMaterialParameters();
	static void SaveDefaultExtendedMaterialParameters();

	static void UpdateExtendedMaterialData(int index, float fresnel[4], float color[4]);
	static void GetExtendedMaterialData(int index, float (&fresnel)[4], float (&color)[4]);
	static r3dString GetExtendedMaterialName(int index);

	static int GetExtendedMaterialDataCount();

	static void UpdateExtendedMaterialsTexture();
	static void UpdateExtendedMaterialsName(int index, const char * newName);

	static void SetupExtendedMaterialParameters();
};

void r3dInitMaterials();
void r3dCloseMaterials();

//------------------------------------------------------------------------
// Material shaders

extern int VS_SKIN_ID											;
extern int VS_SKIN_DEPTH_ID										;
extern int VS_DEPTH_ID											;
extern int VS_DEPTH_PREPASS_ID									;
extern int VS_FILLGBUFFER_ID									;
extern int VS_FILLGBUFFER_2UV_ID									;
extern int VS_FILLGBUFFER_EXTRUDE_ID							;
extern int VS_FILLGBUFFER_DISP_ID								;
extern int VS_FILLGBUFFER_APEX_ID								;
extern int VS_FILLGBUFFER_INSTANCED_ID							;
extern int VS_FILLGBUFFER_INSTANCED_2UV_ID						;

const int MAX_LIGHTS_FOR_TRANSPARENT = 3;
extern int VS_TRANSPARENT_ID[2][MAX_LIGHTS_FOR_TRANSPARENT];
extern int PS_DEPTH_ID;
extern int PS_TRANSPARENT_ID;
extern int PS_TRANSPARENT_AURA_ID;
extern int PS_TRANSPARENT_CAMOUFLAGE_ID;
extern int PS_TRANSPARENT_CAMOUFLAGE_FP_ID;

void SetFillGBufferPixelShader( FillbufferShaderKey k );
void SetFillGBufferConstantPixelShader( const r3dColor& color, float colorAmplify, const r3dPoint3D& normal, float metalness, float chromness, float glow, float specPower );

enum
{
	// float4 	Material_Params		: register(c0);
	MC_MATERIAL_PARAMS,
	// float4 	Mat_Diffuse			: register(c1);
	MC_MAT_DIFFUSE,
	// float4 	Mat_Specular		: register(c2);
	MC_MAT_SPECULAR,
	// float4	Mat_Glow			: register(c3);
	MC_MAT_GLOW,

	// float4 	CamVec				: register(c4);
	MC_CAMVEC,
	
	// float4	DDepth				: register(c5);
	MC_DDEPTH,

	// float4	CamoInterpolator	: register(c6);
	MC_CAMOINTERPOLATOR,
	// float4	UNUSED				: register(c7);
	MC_DEF_SSAO,
	// float4	MaskColor			: register(c8);
	MC_MASKCOLOR,
	// float	fDisplace			: register(c9);
	MC_DISPLACE,

	// transparent stuff
	// float2	PixelSize			: register(c10);
	MC_PIXELSIZE,
	// float4	SunDir				: register(c11);
	MC_SUNDIR,
	// float4	SunColor			: register(c12);
	MC_SUNCOLOR,
	// float4	AmbientColor		: register(c13);
	MC_AMBIENTCOLOR,

	// float4 Aura					: register(c14);
	MC_AURA,
	// float4 TCamoExtra			: register(c15);
	MC_TCAMO_EXTRA,

	// float4 TCamoColor0			: register(c16);
	MC_TCAMO_COLOR0,

	// float4 TCamoColor1			: register(c17);
	MC_TCAMO_COLOR1,

	//float  SelfIllum		: register(c18);
	MC_SELFILLUM_MULT,

	// float4 AerialDensity_Distance_Bias	: register (FOG_CONST0); // x - density, y - distance, z - bias
	FOGC_CONST0 = 29,
	// float4 g_fogColor		: register (FOG_CONST1);	//a = density
	FOGC_CONST1 = FOGC_CONST0 + 1,
	// float4 g_fogParams		: register (FOG_CONST2);	//fadeDist, top, _middle, dist
	FOGC_CONST2 = FOGC_CONST0 + 2,
	FOGC_CONST3 = FOGC_CONST0 + 3,
	FOGC_CONST4 = FOGC_CONST0 + 4,

	SHADOWC_PIXELDIAMETER = 31

};

R3D_FORCEINLINE
/*static*/
void
r3dMaterial::SetRenderedObjectColor( const r3dColor& color )
{
	RenderedObjectColor = color ;
}

extern void ( *g_SetupFilmToneConstants )( int reg ) ;

//------------------------------------------------------------------------

enum SMVSType
{
	SMVSTYPE_PERSP,
	SMVSTYPE_PERSP_PARABOLOID,
	SMVSTYPE_ORTHO,
	SMVSTYPE_ORTHO_WARPED,
	SMVSTYPE_COUNT
};

struct SMDepthVShaderID
{
	union
	{
		struct
		{
			UINT32 apex			: 1;
			UINT32 intanced		: 1;
			UINT32 type			: 2;
		};

		UINT32 Id;
	};

	SMDepthVShaderID();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 16 > SMDepthVShaderIDs;

extern SMDepthVShaderIDs gSMDepthVSIds;

//------------------------------------------------------------------------

struct SkinSMDepthVShaderID
{
	union
	{
		struct
		{
			UINT32 type			: 2;
		};

		UINT32 Id;
	};

	SkinSMDepthVShaderID();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 4 > SkinSMDepthVShaderIDs;

extern SkinSMDepthVShaderIDs gSkinSMDepthVSIds;

//------------------------------------------------------------------------

struct ForestVShaderID
{
	union
	{
		struct
		{
			UINT32 noninstanced		: 1;
			UINT32 animated			: 1;
			UINT32 shadows			: 1;
			UINT32 shadow_type		: 2;
		};

		UINT32 Id;
	};

	ForestVShaderID();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 32 > ForestVShaderIDs;

extern ForestVShaderIDs gForestVSIds;

//------------------------------------------------------------------------

enum SMPSType
{
	SMPSTYPE_DEFAULT,
	SMPSTYPE_HW,
	SMPSTYPE_NORMAL_OFFSET,
	SMPSTYPE_SIMPLE,
	SMPSTYPE_COUNT
};

struct SMDepthPShaderID
{
	union
	{
		struct
		{
			UINT32 type			: 2;
		};

		UINT32 Id;
	};

	SMDepthPShaderID();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 4 > SMDepthPShaderIDs;

extern SMDepthPShaderIDs gSMDepthPSIds;


#endif //__R3D_MAT_H

