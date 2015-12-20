#include "r3dPCH.h"
#include "r3d.h"

#include "Particle.h"

#include "r3dBinMat.h"
#include "../../../GameEngine/TrueNature/Sun.h"

const char* DigitToCString( int digit );
void MakeMaterialFileName( char* MatFile, const char* MatName, const char* DepotName );

static	int 	r3dMaterialIDGenerator = 100;
extern 	IDirect3DBaseTexture9* CurrentTexID[16];

	// flag for skipping material loading
	int	_r3d_MatLib_SkipAllMaterials = 0;

const static char* DEPOT_TAG  = "data/objectsdepot";
const static int   DEPOT_TLEN = 17;

int VS_SKIN_ID											= -1 ;
int VS_SKIN_DEPTH_ID									= -1 ;
int VS_DEPTH_ID											= -1 ;
int VS_DEPTH_PREPASS_ID									= -1 ;
int VS_FILLGBUFFER_ID									= -1 ;
int VS_FILLGBUFFER_2UV_ID								= -1 ;
int VS_FILLGBUFFER_EXTRUDE_ID							= -1 ;
int VS_FILLGBUFFER_DISP_ID								= -1 ;
int VS_FILLGBUFFER_APEX_ID								= -1 ;
int VS_FILLGBUFFER_INSTANCED_ID							= -1 ;
int VS_FILLGBUFFER_INSTANCED_2UV_ID						= -1 ;

int VS_TRANSPARENT_ID[2][MAX_LIGHTS_FOR_TRANSPARENT] = 
{
	{ -1, -1, -1 }, 
	{ -1, -1, -1 } 
};

int PS_DEPTH_ID							= -1;
int PS_TRANSPARENT_ID					= -1;
int PS_TRANSPARENT_AURA_ID				= -1;
int PS_TRANSPARENT_CAMOUFLAGE_ID		= -1;
int PS_TRANSPARENT_CAMOUFLAGE_FP_ID		= -1;

static r3dMaterial* gPrevMaterial = NULL ;
static r3dMaterial* gPrevShadowMaterial = NULL ;
extern r3dScreenBuffer*	DepthBuffer;
extern r3dSun *Sun;

int gNumForwardLightsForTransparent = 0;

FillbufferPixelShadersArr gFillbufferPixelShaders;

void SetFillGBufferPixelShader( FillbufferShaderKey k )
{
	int id = gFillbufferPixelShaders[ k.key ] ;

	r3d_assert( id >= 0 ) ;

	r3dRenderer->SetPixelShader( id );
}

void SetFillGBufferConstantPixelShader( const r3dColor& color, float colorAmplify, const r3dPoint3D& normal, float metalness, float chromness, float glow, float specPower)
{
	const float CA = colorAmplify;

	float vsConsts[ 3 ][ 4 ] =
	{
		// float3 gColor_DefSSAO                        : register( c0 );
		{ CA * color.R / 255.0f, CA * color.G / 255.0f, CA * color.B / 255.0f, r_ssao_clear_val->GetFloat() },
		// float4 gMetalness_Chromeness_Glow_SpecPower  : register( c1 );
		{ metalness, chromness, glow, specPower },
		// float3 gNormal                               : register( c2 );
		{ normal.x * 0.5f + 0.5f, normal.y * 0.5f + 0.5f, normal.z * 0.5f + 0.5f }
	};

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, vsConsts[ 0 ], R3D_ARRAYSIZE( vsConsts ) ) );

	FillGBufferConstantId psid;
	psid.aux = r_lighting_quality->GetInt() != 1;

	r3dRenderer->SetPixelShader( gFillGBufferConstantPSIds[ psid.Id ] );

}

int gMatFrameScores[MAX_MAT_FRAME_SCORES];

// pointer to envmap probes function
r3dTexture* (*r3dMat_EnvmapProbes_GetClosestTexture)(const r3dPoint3D& pos) = NULL;
// pointer to gCamouflageDataManager::GetCurrentData
const r3dMaterial::CamoData& (*r3dMat_gCamouflageDataManager_GetCurrentData)() = NULL;
             
void ( *g_SetupFilmToneConstants )( int reg ) ;

//------------------------------------------------------------------------

FillGBufferConstantId::FillGBufferConstantId()
{
	Id = 0;
}

//------------------------------------------------------------------------

void FillGBufferConstantId::ToString( char* str )
{
	strcpy( str, "PS_FILLBUFFER_CONSTANT" );
	if( aux )
	{
		strcat( str, "_AUX" );
	}
}

//------------------------------------------------------------------------

FillGBufferConstantPSIds gFillGBufferConstantPSIds;

//------------------------------------------------------------------------

void FillGBufferConstantId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 1 );

	defines[ 0 ].Name		= "AUX";
	defines[ 0 ].Definition	= aux ? "1" : "0";
}

//////////////////////////////////////////////////////////////////////////

void FillMacrosForShaderKey(FillbufferShaderKey k, ShaderMacros &outMacro)
{
	outMacro.Resize(6);

	outMacro[0].Name		= "HIGH_QUALITY";
	outMacro[0].Definition	= k.flags.low_q ? "0" : "1";

	outMacro[1].Name		= "ALPHA_CUT";
	outMacro[1].Definition	= k.flags.alpha_test ? "1" : "0";

	outMacro[2].Name		= "FILL_ROADS";
	outMacro[2].Definition	= k.flags.roads ? "1" : "0";

	outMacro[3].Name		= "CAMOUFLAGE";
	outMacro[3].Definition	= k.flags.camouflage ? "1" : "0";

	outMacro[4].Name		= "SIMPLE";
	outMacro[4].Definition	= k.flags.simple ? "1" : "0";

	outMacro[5].Name		= "USE_INSTANCING";
	outMacro[5].Definition	= k.flags.instancing ? "1" : "0";

	extern int _r3d_Mesh_LoadSecondUV;
	if(_r3d_Mesh_LoadSecondUV)
	{
		outMacro.Resize(7);
		outMacro[6].Name		= "USE_DIFFUSE_MASK";
		outMacro[6].Definition	= "1";
	}
}

//////////////////////////////////////////////////////////////////////////

r3dString ConstructShaderNameForKey(FillbufferShaderKey k)
{
	r3dString name = "PS_FILLGBUFFER";
	if (k.flags.low_q) name += "_LQ";
	if (k.flags.disp ) name += "_DISP";
	if (k.flags.roads) name += "_FILL_ROADS";
	if (k.flags.alpha_test) name += "_ATEST";
	if (k.flags.camouflage) name += "_CAMOUFLAGE";
	if (k.flags.simple) name += "_SIMPLE";
	if (k.flags.instancing) name += "_INST";

	return name;
}

//////////////////////////////////////////////////////////////////////////

r3dMaterial::r3dMaterial( int useThumbnails )
{
	Init( useThumbnails );
}

r3dMaterial::~r3dMaterial()
{
	Reset();
}

//------------------------------------------------------------------------

void r3dMaterial::Init( int useThumbnails )
{
	r3dscpy(Name, "_undefined_");
	DepotName[0] = 0;
	r3dscpy(OriginalDir, "Data\\Images");

	TypeName[ 0 ] = 0;

	RefCount = 0;
	TextureRefCount = 1;

	Flags          = useThumbnails ? R3D_MAT_THUMBNAILS : 0;
	WasEdited      = 0;
	AlphaFlagSet = 0 ;
	EmissiveColor		= r3dColor(0,0,0);
	DiffuseColor		= r3dColor(255, 255, 255);
	SpecularColor		= r3dColor(255,255,255);
	FGlassColor			= r3dColor(44, 44, 44); // FakeGlass

	SelfIllumMultiplier = 0.f ;

	GlossTexture		= NULL;
	Texture				= NULL;
	NormalTexture		= NULL;
	DecalMaskTexture	= NULL;
	EnvPowerTexture		= NULL;
	SelfIllumTexture	= NULL;

	ThicknessTexture	= NULL;
	CamouflageMask		= NULL;
	DistortionTexture	= NULL;
	SpecularPowTexture	= NULL;

	NormalDetailTexture	= NULL;

	DetailScale		= 10.0f;
	DetailAmmount	= 0.3f;

	SpecularPower	= 0;
	ReflectionPower	= 0;
	SpecularPower1	= 0.5;

	AlphaRef		= 0;

	ID             = r3dMaterialIDGenerator++;

	m_DoDispl = false;
	m_DisplDepthVal = 0.1f;

	CamouflageMask = 0;

	lowQSelfIllum = 0;
	lowQMetallness = 0;

	TransparencyMultiplier = 1 ;

	SnowAmount = 1.0f;

	matID = 0;

}


void r3dMaterial::Reset()
{
	// no need to delete this now because all thumbnails are deleted at level unload
#if 0
	if(Texture)
		r3dRenderer->DeleteTextureDelayed(Texture);
#endif

	if(NormalTexture)
		r3dRenderer->DeleteTextureDelayed(NormalTexture);

	if(DecalMaskTexture)
		r3dRenderer->DeleteTextureDelayed(DecalMaskTexture);

	if(GlossTexture)
		r3dRenderer->DeleteTextureDelayed(GlossTexture);

	if(EnvPowerTexture)
		r3dRenderer->DeleteTextureDelayed(EnvPowerTexture);

	if(SelfIllumTexture)
		r3dRenderer->DeleteTextureDelayed(SelfIllumTexture);

	if(NormalDetailTexture)
		r3dRenderer->DeleteTextureDelayed(NormalDetailTexture);

	if(ThicknessTexture)
		r3dRenderer->DeleteTextureDelayed(ThicknessTexture);

	if(CamouflageMask)
		r3dRenderer->DeleteTextureDelayed(CamouflageMask);

	if(DistortionTexture)
		r3dRenderer->DeleteTextureDelayed(DistortionTexture);

	if(SpecularPowTexture)
		r3dRenderer->DeleteTextureDelayed(SpecularPowTexture);
}


void r3dMaterial::Set()
{
}

#ifndef FINAL_BUILD

template< typename T > R3D_FORCEINLINE void SetTextureAsMipmapTestT( T* tex, int stageID )
{
	extern r3dTexture *__r3dMipLevelTextures[5];
	if(tex->GetNumMipmaps() < 9)
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], stageID);
	else if(tex->GetNumMipmaps() > 13)
		r3dError("Texture (%s) has more than 13 mip levels!!! WTF!!! It's more than 4096x4096\n", mlGetFileNameFromTextureHelper( tex ) );
	else
		r3dRenderer->SetTex(__r3dMipLevelTextures[13-tex->GetNumMipmaps()], stageID);
}

void SetTextureAsMipmapTest(r3dTexture* tex, int stageID)
{
	SetTextureAsMipmapTestT( tex, stageID );
}

void SetTextureAsMipmapTest(r3dThumbnailTexture* tex, int stageID)
{
	SetTextureAsMipmapTestT( tex, stageID );
}

//////////////////////////////////////////////////////////////////////////

void SetTextureDensityChecker(int stageID)
{
	r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_DENSITY_CHECKER], stageID);
}
#endif

static int VSShaderMap	[ R3D_MATVS_COUNT ]
						[ 2 ]	// displacement on / off
						;

static int VSShadowShaderMap	[ R3D_MATVS_COUNT ]
								[ SMVSTYPE_COUNT ] // perspective or ortho
								;


enum
{
	DISPLACE_OFF,
	DISPLACE_ON
};

static void FillVSShaderMaps()
{


	//------------------------------------------------------------------------
	{
		VSShaderMap	[ R3D_MATVS_DEFAULT							][ DISPLACE_OFF		] = VS_FILLGBUFFER_ID;
		VSShaderMap	[ R3D_MATVS_DECALMASK						][ DISPLACE_OFF		] = VS_FILLGBUFFER_2UV_ID;
		VSShaderMap	[ R3D_MATVS_SKIN							][ DISPLACE_OFF		] = VS_SKIN_ID;
	}

	{
		VSShaderMap	[ R3D_MATVS_FOREST							][ DISPLACE_OFF		] = gForestVSIds[ ForestVShaderID().Id ];
	}

	{
		ForestVShaderID vsid;	vsid.animated = 0;	vsid.noninstanced = 1;
		VSShaderMap	[ R3D_MATVS_FOREST_NONINSTANCED				][ DISPLACE_OFF		] = gForestVSIds[ vsid.Id ];
	}

	{
		ForestVShaderID vsid;	vsid.animated = 1;
		VSShaderMap	[ R3D_MATVS_ANIMATEDFOREST					][ DISPLACE_OFF		] = gForestVSIds[ vsid.Id ] ;
	}

	{
		ForestVShaderID vsid;	vsid.animated = 1;	vsid.noninstanced = 1;
		VSShaderMap	[ R3D_MATVS_ANIMATEDFOREST_NONINSTANCED		][ DISPLACE_OFF		] = gForestVSIds[ vsid.Id ] ;
	}

	{
		VSShaderMap	[ R3D_MATVS_INSTANCING						][ DISPLACE_OFF		] = VS_FILLGBUFFER_INSTANCED_ID ;
		VSShaderMap	[ R3D_MATVS_INSTANCING_2UV					][ DISPLACE_OFF		] = VS_FILLGBUFFER_INSTANCED_2UV_ID ;
		
		VSShaderMap	[ R3D_MATVS_APEX_DESTRUCTIBLE				][ DISPLACE_OFF		] = VS_FILLGBUFFER_APEX_ID;

		VSShaderMap	[ R3D_MATVS_DEFAULT							][ DISPLACE_ON		] = VS_FILLGBUFFER_DISP_ID ;
		VSShaderMap	[ R3D_MATVS_DECALMASK							][ DISPLACE_ON		] = VS_FILLGBUFFER_DISP_ID ;
		VSShaderMap	[ R3D_MATVS_SKIN							][ DISPLACE_ON		] = VS_SKIN_ID ;
		VSShaderMap	[ R3D_MATVS_FOREST							][ DISPLACE_ON		] = VSShaderMap	[ R3D_MATVS_FOREST						][ DISPLACE_OFF ];
		VSShaderMap	[ R3D_MATVS_FOREST_NONINSTANCED				][ DISPLACE_ON		] = VSShaderMap	[ R3D_MATVS_FOREST_NONINSTANCED			][ DISPLACE_OFF ];
		VSShaderMap	[ R3D_MATVS_ANIMATEDFOREST					][ DISPLACE_ON		] = VSShaderMap	[ R3D_MATVS_ANIMATEDFOREST				][ DISPLACE_OFF ];
		VSShaderMap	[ R3D_MATVS_ANIMATEDFOREST_NONINSTANCED		][ DISPLACE_ON		] = VSShaderMap	[ R3D_MATVS_ANIMATEDFOREST_NONINSTANCED	][ DISPLACE_OFF ];
		VSShaderMap	[ R3D_MATVS_INSTANCING						][ DISPLACE_ON		] = VS_FILLGBUFFER_INSTANCED_ID ;
		VSShaderMap	[ R3D_MATVS_INSTANCING_2UV					][ DISPLACE_ON		] = VS_FILLGBUFFER_INSTANCED_2UV_ID ;
		VSShaderMap	[ R3D_MATVS_APEX_DESTRUCTIBLE				][ DISPLACE_ON		] = VS_FILLGBUFFER_APEX_ID;
	}

	//------------------------------------------------------------------------
	{
		SMDepthVShaderID vsid;

		vsid.type = SMVSTYPE_PERSP;
		VSShadowShaderMap[ R3D_MATVS_DEFAULT		][ SPT_PERSP				]	= gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap[ R3D_MATVS_DEFAULT		][ SPT_PERSP_PARABOLOID	]	= gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO;
		VSShadowShaderMap[ R3D_MATVS_DEFAULT		][ SPT_ORTHO			]	= gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap[ R3D_MATVS_DEFAULT		][ SPT_ORTHO_WARPED		]	= gSMDepthVSIds[ vsid.Id ];
	}

	{
		SkinSMDepthVShaderID vsid;

		vsid.type = SMVSTYPE_PERSP;
		VSShadowShaderMap[ R3D_MATVS_SKIN			][ SPT_PERSP				] = gSkinSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap[ R3D_MATVS_SKIN			][ SPT_PERSP_PARABOLOID	] = gSkinSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO;
		VSShadowShaderMap[ R3D_MATVS_SKIN			][ SPT_ORTHO			] = gSkinSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap[ R3D_MATVS_SKIN			][ SPT_ORTHO_WARPED		] = gSkinSMDepthVSIds[ vsid.Id ];
	}

	{
		ForestVShaderID vsid;
		vsid.shadows = 1;

		vsid.shadow_type = SMVSTYPE_PERSP;
		VSShadowShaderMap	[ R3D_MATVS_FOREST		][ SPT_PERSP			] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap	[ R3D_MATVS_FOREST		][ SPT_PERSP_PARABOLOID	] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO;
		VSShadowShaderMap	[ R3D_MATVS_FOREST		][ SPT_ORTHO			] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap	[ R3D_MATVS_FOREST		][ SPT_ORTHO_WARPED		] = gForestVSIds[ vsid.Id ];
	}

	{
		ForestVShaderID vsid;
		vsid.shadows = 1;
		vsid.noninstanced = 1;

		vsid.shadow_type = SMVSTYPE_PERSP;
		VSShadowShaderMap	[ R3D_MATVS_FOREST_NONINSTANCED		][ SPT_PERSP			] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap	[ R3D_MATVS_FOREST_NONINSTANCED		][ SPT_PERSP_PARABOLOID	] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO;
		VSShadowShaderMap	[ R3D_MATVS_FOREST_NONINSTANCED		][ SPT_ORTHO			] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap	[ R3D_MATVS_FOREST_NONINSTANCED		][ SPT_ORTHO_WARPED		] = gForestVSIds[ vsid.Id ];
	}

	{
		ForestVShaderID vsid;
		vsid.shadows = 1;
		vsid.animated = 1;

		vsid.shadow_type = SMVSTYPE_PERSP;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST ][ SPT_PERSP			] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST ][ SPT_PERSP_PARABOLOID	] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST ][ SPT_ORTHO			] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST ][ SPT_ORTHO_WARPED		] = gForestVSIds[ vsid.Id ];
	}

	{
		ForestVShaderID vsid;
		vsid.shadows = 1;
		vsid.animated = 1;
		vsid.noninstanced = 1;
		
		vsid.shadow_type = SMVSTYPE_PERSP;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST_NONINSTANCED ][ SPT_PERSP				] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST_NONINSTANCED ][ SPT_PERSP_PARABOLOID	] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST_NONINSTANCED ][ SPT_ORTHO				] = gForestVSIds[ vsid.Id ];

		vsid.shadow_type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap[ R3D_MATVS_ANIMATEDFOREST_NONINSTANCED ][ SPT_ORTHO_WARPED		] = gForestVSIds[ vsid.Id ];
	}

	{
		SMDepthVShaderID vsid;
		vsid.intanced = 1;

		vsid.type = SMVSTYPE_PERSP;
		VSShadowShaderMap[ R3D_MATVS_INSTANCING ][ SPT_PERSP				] = gSMDepthVSIds[ vsid.Id ];
		VSShadowShaderMap[ R3D_MATVS_INSTANCING_2UV ][ SPT_PERSP			] = gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap[ R3D_MATVS_INSTANCING ][ SPT_PERSP_PARABOLOID		] = gSMDepthVSIds[ vsid.Id ];
		VSShadowShaderMap[ R3D_MATVS_INSTANCING_2UV ][ SPT_PERSP_PARABOLOID	] = gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO;
		VSShadowShaderMap[ R3D_MATVS_INSTANCING ][ SPT_ORTHO				] = gSMDepthVSIds[ vsid.Id ];
		VSShadowShaderMap[ R3D_MATVS_INSTANCING_2UV ][ SPT_ORTHO			] = gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap[ R3D_MATVS_INSTANCING ][ SPT_ORTHO_WARPED			] = gSMDepthVSIds[ vsid.Id ];
		VSShadowShaderMap[ R3D_MATVS_INSTANCING_2UV ][ SPT_ORTHO_WARPED		] = gSMDepthVSIds[ vsid.Id ];
	}

	{
		SMDepthVShaderID vsid;
		vsid.apex = 1;

		vsid.type = SMVSTYPE_PERSP;
		VSShadowShaderMap[ R3D_MATVS_APEX_DESTRUCTIBLE ][ SPT_PERSP				] = gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_PERSP_PARABOLOID;
		VSShadowShaderMap[ R3D_MATVS_APEX_DESTRUCTIBLE ][ SPT_PERSP_PARABOLOID	] = gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO;
		VSShadowShaderMap[ R3D_MATVS_APEX_DESTRUCTIBLE ][ SPT_ORTHO				] = gSMDepthVSIds[ vsid.Id ];

		vsid.type = SMVSTYPE_ORTHO_WARPED;
		VSShadowShaderMap[ R3D_MATVS_APEX_DESTRUCTIBLE ][ SPT_ORTHO_WARPED		] = gSMDepthVSIds[ vsid.Id ];
	}
}

void r3dMaterial::SetVertexShader( r3dMatVSType VSType, int DoDisplace )
{
	r3dRenderer->SetVertexShader( VSShaderMap	[ VSType		]
												[ DoDisplace	] );
}

void r3dMaterial::SetShadowVertexShader ( r3dMatVSType VSType )
{
	r3dRenderer->SetVertexShader( VSShadowShaderMap	[ VSType						]
													[ r3dRenderer->ShadowPassType	] );

}


/*static*/
void r3dMaterial::SetupTransparentStates( int UseSkins, int psID )
{
	r3d_assert(gNumForwardLightsForTransparent < MAX_LIGHTS_FOR_TRANSPARENT);
	if (gNumForwardLightsForTransparent < MAX_LIGHTS_FOR_TRANSPARENT)
		r3dRenderer->SetVertexShader(VS_TRANSPARENT_ID[ UseSkins ][ gNumForwardLightsForTransparent ] );
	else
		r3dRenderer->SetVertexShader();

	if( g_SetupFilmToneConstants )
		g_SetupFilmToneConstants ( 24 ) ;

	r3dTransparentSetDistort( 1 ) ;

	r3dRenderer->SetPixelShader(psID);
}

void
r3dMaterial::SetupCompoundDiffuse( D3DXVECTOR4 * outVec, int forTransaprency )
{
	* outVec = D3DXVECTOR4(	DiffuseColor.R * RenderedObjectColor.R / ( 255.0f * 255.0f ),
							DiffuseColor.G * RenderedObjectColor.G / ( 255.0f * 255.0f ),
							DiffuseColor.B * RenderedObjectColor.B / ( 255.0f * 255.0f ),
							forTransaprency ? TransparencyMultiplier : /*lowQSelfIllum */ (float)RenderedObjectColor.A / 255.0f ) ;
}

void r3dMaterial::Start( r3dMatVSType VSType, UINT SetupFlags )
{
	if( gPrevMaterial == this )
	{
		D3DXVECTOR4 diffuse ;
		SetupCompoundDiffuse( &diffuse, Flags & R3D_MAT_TRANSPARENT ) ;
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_MAT_DIFFUSE, (float*)&diffuse, 1 ) ) ;
		return;
	}
	else
	{
		if( r3dMaterial* mat = gPrevMaterial )
		{
			gPrevMaterial = NULL;
			mat->End();
		}
	}

	if( !AlphaFlagSet )
	{
		if( !Texture || Texture->GetFullSizeTexture()->IsDrawable() )
		{
			SetAlphaFlag();
			AlphaFlagSet = 1;
		}
	}

	if( !( SetupFlags & R3D_MATF_NO_SHADERS )  )
	{
		bool lowQLighting = r_lighting_quality->GetInt() == 1;
		bool highQLighting = r_lighting_quality->GetInt() == 3;

		bool doDispl = m_DoDispl > 0 && highQLighting;

		if( SetupFlags & R3D_MATF_ROAD )
		{
			// not implemented... implement.
			r3d_assert( !m_DoDispl );
		}
		
		if (Flags & R3D_MAT_TRANSPARENT)
		{
			int psid = PS_TRANSPARENT_ID ;

			if( Flags & R3D_MAT_TRANSPARENT_CAMOUFLAGE )
			{
				psid = PS_TRANSPARENT_CAMOUFLAGE_ID ;
			}

			if( Flags & R3D_MAT_TRANSPARENT_CAMO_FP )
			{
				psid = PS_TRANSPARENT_CAMOUFLAGE_FP_ID ;
			}

			SetupTransparentStates( VSType == R3D_MATVS_SKIN, psid );
		}
		else
		{
			SetVertexShader( VSType, doDispl );

			int NeedAlphaCut = SetupFlags & R3D_MATF_FORCE_ALPHATEST | Flags & R3D_MAT_HASALPHA ;
			FillbufferShaderKey key;
			key.flags.alpha_test = NeedAlphaCut > 0 ? 1 : 0;
			key.flags.disp = doDispl ? 1 : 0;

			SetLQ( &key ) ;

			key.flags.camouflage = (Flags & R3D_MAT_CAMOUFLAGE) ? 1 : 0;
			key.flags.roads = (SetupFlags & R3D_MATF_ROAD) ? 1 : 0;
			key.flags.instancing = (SetupFlags & R3D_MATF_INSTANCING) ? 1 : 0;		

			if(key.flags.camouflage)
			{
				if (r3dMat_gCamouflageDataManager_GetCurrentData)
				{
					const CamoData & cm = r3dMat_gCamouflageDataManager_GetCurrentData();
					if(cm.curTex==0) // don't run camo if curTex is null, otherwise camo will be just pitch black
						key.flags.camouflage = 0;
				}
				else
					key.flags.camouflage = 0;
			}

			if( r_in_minimap_render->GetInt() )
			{
				key.flags.simple = 1 ;
			}

			if( !( SetupFlags & R3D_MATF_NO_PIXEL_SHADER ) )
			{
				SetFillGBufferPixelShader( key ) ;
			}
		}
	}

	StartCommonProperties();

	bool lowQ = r_lighting_quality->GetInt() == 1;

	extern int __r3dDisplayMipLevels;

#ifndef FINAL_BUILD
	int AllowMeshTextures = !r_disable_mesh_textures->GetInt();
#else
	int AllowMeshTextures = 1;
#endif

	if (Texture && AllowMeshTextures)
	{
#ifndef FINAL_BUILD
		if (__r3dDisplayMipLevels)
			SetTextureAsMipmapTest(Texture, 0);
		else if (d_show_checker_texture->GetBool())
			SetTextureDensityChecker(0);
		else
#endif
			r3dRenderer->SetThumbTex(Texture);
	}
	else
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE]);

	if ((Flags & R3D_MAT_CAMOUFLAGE) && !(Flags & ( R3D_MAT_TRANSPARENT_CAMOUFLAGE | R3D_MAT_TRANSPARENT_CAMO_FP ) ) && r3dMat_gCamouflageDataManager_GetCurrentData)
	{
		const CamoData & cm = r3dMat_gCamouflageDataManager_GetCurrentData();
		if(cm.curTex) // don't run camo if curTex is null, otherwise camo will be just pitch black
		{
			if( cm.prevTex )
				r3dRenderer->SetTex( cm.prevTex, 8);
			else
				r3dRenderer->SetThumbTex( Texture, 8);

			if( cm.curTex->IsDrawable() )
			{
				r3dRenderer->SetTex(cm.curTex, 9);
			}
			else
			{
				r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_GREY]);
			}
			if( CamouflageMask && CamouflageMask->IsDrawable() )
				r3dRenderer->SetTex( CamouflageMask, 10 );
			else
				r3dRenderer->SetTex( NULL );

			float currT = r3dGetTime();
			float camoLerpF = (currT - cm.transitionStartTime) / CAMOUFLAGE_LERP_DURATION;
			camoLerpF = r3dTL::Clamp(camoLerpF, 0.0f, 1.0f);

			D3DXVECTOR4 v(camoLerpF, 0, 0, 0);
			r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_CAMOINTERPOLATOR, &v.x, 1);
		}
		else
		{
			r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 8);
			r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 9);
			r3dRenderer->SetTex(CamouflageMask, 10);
		}
	}
	else
	{
		if (r_show_winter->GetBool())
		{
			r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WINTER_DIFFUSE], 8);
			r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WINTER_NORMAL], 9);
			r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WINTER_SPECULAR], 10);
		}
	}

	if (NormalTexture && NormalTexture->IsDrawable() && AllowMeshTextures) r3dRenderer->SetTex(NormalTexture,1);
	else
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_DEFAULT_NORMAL],1);

	if (GlossTexture && GlossTexture->IsDrawable() && AllowMeshTextures)
	{
		if (!lowQ )
			r3dRenderer->SetTex(GlossTexture,2);
	}
	else
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_DEFAULT_METALNESS], 2);

	if(EnvPowerTexture && EnvPowerTexture->IsDrawable() && AllowMeshTextures)
		r3dRenderer->SetTex(EnvPowerTexture,3);
	else
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 3);

	if(SelfIllumTexture && SelfIllumTexture->IsDrawable() && AllowMeshTextures)
	{
		if (!lowQ)
			r3dRenderer->SetTex(SelfIllumTexture,4);
	}
	else
	{		
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 4);
	}

	if (NormalDetailTexture && NormalDetailTexture->IsDrawable() && AllowMeshTextures)
	{
		if (!lowQ)
			r3dRenderer->SetTex(NormalDetailTexture,5);
	}
	else
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_GREY],5);

	if (ThicknessTexture && ThicknessTexture->IsDrawable() && AllowMeshTextures) r3dRenderer->SetTex(ThicknessTexture,6);
	else
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE],6);

	// transparent doesn't need this texture yet it overrides skydome cubemap which is needed
	// by transparent
	if( !( Flags & R3D_MAT_TRANSPARENT ) )
	{
		if (SpecularPowTexture && SpecularPowTexture->IsDrawable() && AllowMeshTextures )
			r3dRenderer->SetTex(SpecularPowTexture,7);
		else
			r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE],7);
	}

	if (DecalMaskTexture && DecalMaskTexture->IsDrawable() && AllowMeshTextures) 
		r3dRenderer->SetTex(DecalMaskTexture, 11);
	else
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE], 11);



	if( !( SetupFlags & R3D_MATF_NO_PIXEL_SHADER ) )
	{
		D3DXVECTOR4 vConsts[ 4 ] =
		{
			D3DXVECTOR4(SpecularPower, ReflectionPower, SpecularPower1, 1.0f),
			D3DXVECTOR4( 0.f, 0.f, 0.f, lowQSelfIllum),
			//[Alexey] Seems we are not using raw  SpecularColor in shaders. We're started to use matID as SpecularColor.R component instead!
			//D3DXVECTOR4( SpecularColor.R/255.0f, SpecularColor.G/255.0f, SpecularColor.B/255.0f, lowQMetallness),
			D3DXVECTOR4( (float)matID/256.0f, SnowAmount, 0.0f, lowQMetallness),
			D3DXVECTOR4( SelfIllumMultiplier, 0.0f, DetailScale, DetailAmmount )
		} ;

		SetupCompoundDiffuse( &vConsts[ 1 ], Flags & R3D_MAT_TRANSPARENT ) ;

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF(  MC_MATERIAL_PARAMS, (float *)vConsts,  R3D_ARRAYSIZE( vConsts ) ) );

		COMPILE_ASSERT( MC_MAT_DIFFUSE - MC_MATERIAL_PARAMS >= 0 ) ;
		COMPILE_ASSERT( MC_MAT_SPECULAR - MC_MATERIAL_PARAMS >= 0 ) ;
		COMPILE_ASSERT( MC_MAT_GLOW - MC_MATERIAL_PARAMS >= 0 ) ;

		COMPILE_ASSERT( MC_MAT_DIFFUSE - MC_MATERIAL_PARAMS < R3D_ARRAYSIZE( vConsts ) ) ;
		COMPILE_ASSERT( MC_MAT_SPECULAR - MC_MATERIAL_PARAMS < R3D_ARRAYSIZE( vConsts ) ) ;
		COMPILE_ASSERT( MC_MAT_GLOW - MC_MATERIAL_PARAMS < R3D_ARRAYSIZE( vConsts ) ) ;

		if ( m_DoDispl )
		{
			D3DXVECTOR4 v = D3DXVECTOR4( m_DisplDepthVal, 0, 0, 0 );
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_DISPLACE, (float*)&v, 1 ) );
		}
	}

	StartTransparent();
}



void r3dMaterial::End()
{
	if( gPrevMaterial == this )
		return;

	EndCommonProperties();
	EndTransparent();
}

void r3dMaterial::StartShadows( r3dMatVSType VSType )
{
	SetShadowVertexShader( VSType );

	SMDepthPShaderID psid;

	psid.type = SMPSTYPE_DEFAULT;

	r3dRenderer->SetPixelShader( gSMDepthPSIds[ psid.Id ] );

	StartCommonProperties();

	if (Texture)
		r3dRenderer->SetThumbTex(Texture);
	else
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE]);

}

void r3dMaterial::EndShadows()
{

}

//////////////////////////////////////////////////////////////////////////

void SetParticleDepth();

void r3dMaterial::StartTransparent()
{
	extern r3dCamera gCam;
	if (Flags & R3D_MAT_TRANSPARENT)
	{
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH);
		SetParticleDepth();
		r3dRenderer->SetTex(r3dMat_EnvmapProbes_GetClosestTexture(gCam), 8);

		r3dRenderer->SetTex( ( DistortionTexture && DistortionTexture->IsDrawable() ) ? DistortionTexture : __r3dShadeTexture[SHADETEXT_DEFAULT_DISTORTION], 2);

		D3DXVECTOR4 vecs[3];
		vecs[0] = D3DXVECTOR4(gCam.x, gCam.y, gCam.z, 1);
		// float3 vCamera					: register( c17 );
		r3dRenderer->pd3ddev->SetVertexShaderConstantF(17, &vecs[0].x, 1);
		if (Sun)
		{
			r3dVector v(-Sun->SunDir);
			v.Normalize();
			vecs[0] = D3DXVECTOR4(v.x, v.y, v.z, 0.f );
			vecs[1] = D3DXVECTOR4(Sun->SunLight.R / 255, Sun->SunLight.G / 255, Sun->SunLight.B / 255, 1);
		}
		else
		{
			vecs[0] = vecs[1] = D3DXVECTOR4(0, 0, 0, 0.f);
		}
		vecs[2] = D3DXVECTOR4(r3dRenderer->AmbientColor.R / 255.0f, r3dRenderer->AmbientColor.G / 255.0f, r3dRenderer->AmbientColor.B / 255.0f, 1);
		if (!(Flags & (R3D_MAT_TRANSPARENT_CAMOUFLAGE | R3D_MAT_TRANSPARENT_CAMO_FP ) ) )
		{
			float extrudeAmount[4] = {0};
			D3D_V(r3dRenderer->pd3ddev->SetVertexShaderConstantF(23, &extrudeAmount[0], 1));
		}
		
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_SUNDIR, &vecs[0].x, R3D_ARRAYSIZE(vecs)) );

		COMPILE_ASSERT( MC_SUNCOLOR - MC_SUNDIR >= 0 ) ;
		COMPILE_ASSERT( MC_AMBIENTCOLOR - MC_SUNDIR >= 0 ) ;

		COMPILE_ASSERT( MC_SUNCOLOR - MC_SUNDIR < R3D_ARRAYSIZE(vecs) ) ;
		COMPILE_ASSERT( MC_AMBIENTCOLOR - MC_SUNDIR < R3D_ARRAYSIZE(vecs) ) ;

		vecs[0] = D3DXVECTOR4(SpecularPower1, SpecularPower, ReflectionPower, 0.0f);

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_MATERIAL_PARAMS, &vecs[0].x, 1) );
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dMaterial::EndTransparent()
{
	if (Flags & R3D_MAT_TRANSPARENT)
	{
		r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		r3dRenderer->SetTex(0, 2);
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dMaterial::StartCommonProperties()
{
	if(Flags & R3D_MAT_DOUBLESIDED)
	{
		r3dRenderer->SetCullMode( D3DCULL_NONE );
	}
}

void r3dMaterial::EndCommonProperties()
{
	if(Flags & R3D_MAT_DOUBLESIDED)
	{
		r3dRenderer->RestoreCullMode();
	}
}

/*static*/
void
r3dMaterial::ResetMaterialFilter()
{
	gPrevMaterial = NULL;
}

static void _GetTexName(const char* param, char* TexName)
{
	const char* p1;

	*TexName = 0;
	for(p1 = param; *p1 == ' ' || *p1 == '\t'; p1++) ;
	r3dscpy(TexName, p1);

	char* p2;
	for(p2 = TexName + strlen(TexName) - 1; *p2 == ' '; *p2-- = 0) ;

	if(stricmp(TexName, "NONE") == 0)
		*TexName = 0;

	return;
}

enum TexType
{
	TT_DIFFUSE,
	TT_METALNESS,
	TT_GLOW,
	TT_NORMAL,
	TT_NORMALDETAIL,
	TT_DENSITYMAP,
	TT_ROUGHNESS,
	TT_CAMOUFLAGEMASK,
	TT_DISTORTION,
	TT_SPECPOW,
	TT_COUNT
};

int MaxDimmensionTable[ TT_COUNT ][ 3 ] = 
{
	/*TT_DIFFUSE		*/	{  512, 1024, 4096 } ,
	/*TT_METALNESS		*/	{  256,  512, 4096 } ,
	/*TT_GLOW			*/	{  256,  512, 4096 } ,
	/*TT_NORMAL			*/	{  512,  512, 4096 } ,
	/*TT_NORMALDETAIL	*/	{  256,  512, 4096 } ,
	/*TT_DENSITYMAP		*/	{  256,  512, 4096 } ,
	/*TT_ROUGHNESS		*/	{  256,  512, 4096 } ,
	/*TT_CAMOUFLAGEMASK	*/	{  512,  1024, 4096 } ,
	/*TT_DISTORTION		*/	{  512,  512, 4096 } ,
	/*TT_SPECPOW		*/	{  256,  512, 4096 }
};

int DownScaleTable[ TT_COUNT ][ 3 ] =
{
	/*TT_DIFFUSE		*/	{ 1, 1, 1 } ,
	/*TT_METALNESS		*/	{ 1, 1, 1 } ,
	/*TT_GLOW			*/	{ 1, 1, 1 } ,
	/*TT_NORMAL			*/	{ 1, 1, 1 } ,
	/*TT_NORMALDETAIL	*/	{ 1, 1, 1 } ,
	/*TT_DENSITYMAP		*/	{ 1, 1, 1 } ,
	/*TT_ROUGHNESS		*/	{ 1, 1, 1 } ,
	/*TT_CAMOUFLAGEMASK	*/	{ 1, 1, 1 } ,
	/*TT_DISTORTION		*/	{ 1, 1, 1 } ,
	/*TT_SPECPOW		*/	{ 1, 1, 1 } 
} ;

static inline int GetMaxScaleDim( TexType ttype )
{
	r3d_assert( ttype >= 0 && ttype < TT_COUNT ) ;

	int ql = R3D_MAX( R3D_MIN( r_texture_quality->GetInt(), 3 ), 1 ) ;

	return MaxDimmensionTable[ ttype ][ ql - 1 ] ;
}

static inline int GetDownScale( TexType ttype )
{
	r3d_assert( ttype >= 0 && ttype < TT_COUNT ) ;

	int ql = R3D_MAX( R3D_MIN( r_texture_quality->GetInt(), 3 ), 1 ) ;

	return DownScaleTable[ ttype ][ ql - 1 ] ;
}

void r3dMaterial::AdjustLoadPath( char* adjustedPath, const char* fname )
{
	sprintf(adjustedPath, "%s\\%s", OriginalDir, fname);

	if(r3d_access(adjustedPath, 0) != 0) {
		// try .dds
		r3dscpy(adjustedPath + strlen(adjustedPath) - 3, "dds");
	}
}

static D3DPOOL getTexLoadPool()
{
	D3DPOOL pool = D3DPOOL_MANAGED;

	if( r_no_managed_textures->GetInt() )
		pool = D3DPOOL_DEFAULT;

	return pool;
}

r3dTexture* r3dMaterial::MatLoadTexture(const char* fname, const char* slot, int DownScale, int MaxDim, bool player, bool deferLoading )
{
	if(*fname == 0)
		return NULL;

	char TempPath[MAX_PATH];
	AdjustLoadPath( TempPath, fname );

	D3DPOOL pool = getTexLoadPool();

	r3dTexture* tex = r3dRenderer->LoadTexture( TempPath, D3DFMT_UNKNOWN, false, DownScale, MaxDim, pool, player ? PlayerTexMem : TexMem, deferLoading );

	if( deferLoading )
	{
		tex->bPersistent = 1;
	}

#ifndef FINAL_BUILD
	if(tex == NULL)
		r3dArtBug( "Material: %s texture %s cannot be loaded\n", slot, TempPath );
	else
		tex->CheckPow2();
#endif

	return tex;
}

r3dThumbnailTexture* r3dMaterial::MatLoadThumbTexture(const char* fname, const char* slot, int DownScale, int MaxDim, bool player )
{
	if(*fname == 0)
		return NULL;

	char TempPath[MAX_PATH];
	AdjustLoadPath( TempPath, fname );

	D3DPOOL pool = getTexLoadPool();

	r3dThumbnailTexture* tex = r3dRenderer->LoadThumbnailTexture( TempPath, DownScale, MaxDim, pool, player ? PlayerTexMem : TexMem );

#ifndef FINAL_BUILD
	if(tex == NULL)
		r3dArtBug( "Material: %s texture %s cannot be loaded\n", slot, TempPath );
	else
	if( r3dTexture* full = tex->GetFullSizeTexture() )
		full->CheckPow2();
#endif

	return tex;
}

void SetFlag(int& flags, int flag) { flags |= flag; }
void ResetFlag(int& flags, int flag) { flags &= ~flag; }
void SetFlag(int& flags, int flag, bool invert)
{
	invert ? ResetFlag(flags, flag) : SetFlag(flags, flag);
}

int r3dMaterial::LoadAscii(r3dFile *f, const char* szTexPath)
{
	bool player = Flags & R3D_MAT_PLAYER ? true : false ;

	char 	buf[256];
	char 	*param;

	if(!f->IsValid()) 
		return 0;

	while(!feof(f)) {
		*buf = 0;
		if(fgets(buf, sizeof(buf), f) == NULL)
			return 0;
		if(strstr(buf,"[MaterialBegin]") != NULL)
			break;
	}

	TypeName[ 0 ] = 0;

	if(feof(f))
		return FALSE;

	char	TextureName[128]			= "";
	char	NormalMapName[256]			= "";
	char	DecalMaskName[256]			= "";
	char	EnvMapName[128]				= "";
	char	GlossMapName[128]			= "";
	char	IBLMapName[128]				= "";
	char	DetailNMapName[128]			= "";
	char	DensityMapName[128]			= "";
	char	CamouflageMaskName[128]		= "";
	char	DistortionMapName[128]		= "";
	char	SpecPowMapName[128]			= "";

	TextureName[0]			= 0;
	NormalMapName[0]		= 0;
	DecalMaskName[0]		= 0;
	EnvMapName[0]			= 0;
	GlossMapName[0]			= 0;
	IBLMapName[0]			= 0;
	DetailNMapName[0]		= 0;
	DensityMapName[0]		= 0;
	CamouflageMaskName[0]	= 0;
	DistortionMapName[0]	= 0;
	SpecPowMapName[0]		= 0;

	sprintf(OriginalDir,  "%s", szTexPath);

	while(!feof(f))
	{
		*buf = 0;
		if(fgets(buf, sizeof(buf), f) == NULL)
			return FALSE;
		buf[strlen(buf)-1] = 0;

		if(strstr(buf,"[MaterialEnd]"))
			break;

		if((param = strchr(buf, '=')) == NULL)
			continue;
		param++;

		if(!strnicmp(buf,"Name",4)) {
			sscanf(param,"%s", Name);
			//r3dOutToLog("Material: Starting %s\n", Name);
			continue;
		}

		if(!strnicmp(buf,"Flags",5)) {
			char FlagsS[128];
			sscanf(param,"%s",FlagsS);
			//r3dOutToLog("Material: Material flags are %s\n", FlagsS);

			continue;
		}

		if(!strnicmp(buf,"matID", 5)) {
			sscanf(param,"%d", &matID);
			continue;
		}

		if(!strnicmp(buf,"SpecularPower",13)) {
			sscanf(param,"%f", &SpecularPower);
			continue;
		}

		if(!strnicmp(buf,"DetailScale",11)) {
			sscanf(param,"%f", &DetailScale);
			continue;
		}

		if(!strnicmp(buf,"DetailAmmount",13)) {
			sscanf(param,"%f", &DetailAmmount);
			continue;
		}

		if(!strnicmp(buf,"Specular1Power",14)) {
			sscanf(param,"%f", &SpecularPower1);
			continue;
		}

		if(!strnicmp(buf,"SnowAmount",10)) {
			sscanf(param,"%f", &SnowAmount);
			continue;
		}

		if(!strnicmp(buf,"displace",8)) 
		{
			int n;
			sscanf(param,"%d", &n);
			m_DoDispl = n != 0;
			continue;
		}

		if(!strnicmp(buf,"displ_val",9)) 
		{
			sscanf(param,"%f", &m_DisplDepthVal);
			continue;
		}

		if(!strnicmp(buf,"ReflectionPower",15)) {
			sscanf(param, "%f", &ReflectionPower);
			continue;
		}

		if (!strnicmp(buf, "AlphaTransparent", 16))
		{
			int transparencyFlag = 0;
			sscanf(param, "%d", &transparencyFlag);
			SetFlag(Flags, R3D_MAT_TRANSPARENT, transparencyFlag == 0);
			continue;
		}

		if (!strnicmp(buf, "Camouflage", 10))
		{
			int camoFlag = 0;
			sscanf(param, "%d", &camoFlag);
			SetFlag(Flags, R3D_MAT_CAMOUFLAGE, camoFlag == 0);
			continue;
		}

		if(!strnicmp(buf, "DoubleSided", sizeof "DoubleSided" - 1))
		{
			int DoubleSided;
			sscanf(param, "%d", &DoubleSided);
			SetFlag(Flags, R3D_MAT_DOUBLESIDED, DoubleSided == 0);
			continue;
		}

		if(!strnicmp(buf,"Color24",7)) {
			int	iR, iG, iB;
			sscanf(param,"%d %d %d", &iR, &iG, &iB);
			DiffuseColor.R = iR;
			DiffuseColor.G = iG;
			DiffuseColor.B = iB;
			continue;
		}

		if(!strnicmp(buf,"FGColor",7)) {  // FakeGlass
			int	iR, iG, iB;
			sscanf(param,"%d %d %d", &iR, &iG, &iB);
			FGlassColor.R = iR;
			FGlassColor.G = iG;
			FGlassColor.B = iB;
			continue;
		}

		if( !strnicmp(buf, "Type", 4 ) )
		{
			char Buf[ 256 ];
			sscanf( param, "%s", Buf );
			r3dscpy( TypeName, Buf );
			continue;
		}

		if( !strnicmp(buf, "lowQSelfIllum", 13 ) )
		{
			sscanf(param, "%f", &lowQSelfIllum);
			continue;
		}

		if( !strnicmp(buf, "lowQMetallness", 14 ) )
		{
			sscanf(param, "%f", &lowQMetallness);
			continue;
		}

		if( !strnicmp( buf, "TransparentShadows", sizeof "TransparentShadows" - 1 ) || 
			!strnicmp( buf, "ForceTransparent", sizeof "ForceTransparent" - 1 )
			)
		{
			int ForceTransparent;
			sscanf( param, "%d", &ForceTransparent );
			SetFlag(Flags, R3D_MAT_FORCEHASALPHA, ForceTransparent == 0);
			SetAlphaFlag();
			continue;
		}

		if(!strnicmp(buf,"Texture",        7)) { _GetTexName(param, TextureName);	continue; }

		if(!strnicmp(buf,"NormalMap",      9)) { _GetTexName(param, NormalMapName);  continue; }
		
		if(!strnicmp(buf,"DecalMask",      9)) { _GetTexName(param, DecalMaskName); continue; }

		if(!strnicmp(buf,"EnvMap",         6)) { _GetTexName(param, EnvMapName);   	continue; }

		if(!strnicmp(buf,"SpecularMap",    8)) { _GetTexName(param, GlossMapName);  continue; }

		if(!strnicmp(buf,"GlowMap",        7)) { _GetTexName(param, IBLMapName);  continue; }

		if(!strnicmp(buf,"DetailNMap",    10)) { _GetTexName(param, DetailNMapName);  continue; }

		if(!strnicmp(buf,"DensityMap",    10)) { _GetTexName(param, DensityMapName);  continue; }

		if(!strnicmp(buf,"CamoMask",       8)) { _GetTexName(param, CamouflageMaskName);  continue; }

		if(!strnicmp(buf,"ImagesDir",      9)) { _GetTexName(param, OriginalDir);  continue; }

		if(!strnicmp(buf,"DistortionMap", 13)) { _GetTexName(param, DistortionMapName);  continue; }

		if(!strnicmp(buf,"SpecPowMap",    10)) { _GetTexName(param, SpecPowMapName);  continue; }

		if(!strnicmp(buf,"SelfIllumMultiplier", 19)) { sscanf(param, "%f", &SelfIllumMultiplier);  continue; }		

	} // while not end of material description

	// load textures

	bool use_thumbnails = Flags & R3D_MAT_THUMBNAILS ? true : false;

	if( use_thumbnails )
		TextureRefCount = 0;
	else
		TextureRefCount = 1;

	Texture             = MatLoadThumbTexture(TextureName,      "Diffuse"           , GetDownScale( TT_DIFFUSE          ), GetMaxScaleDim( TT_DIFFUSE       ), player );

	if( Texture )
	{
		if( use_thumbnails )
		{
			if( !Texture->HasPoolDefaultThumbnail() && !Texture->IsFullSize() )
				Texture->MakeThumbnail();
		}
		else
		{
			Texture->LoadFullSizeTexture();
		}
	}

	GlossTexture        = MatLoadTexture(GlossMapName,          "Metalness"         , GetDownScale( TT_METALNESS        ), GetMaxScaleDim( TT_METALNESS             ), player, use_thumbnails );
	SelfIllumTexture    = MatLoadTexture(IBLMapName,            "GlowMap"           , GetDownScale( TT_GLOW             ), GetMaxScaleDim( TT_GLOW                  ), player, use_thumbnails );
	NormalTexture       = MatLoadTexture(NormalMapName,         "Normalmap"         , GetDownScale( TT_NORMAL           ), GetMaxScaleDim( TT_NORMAL                ), player, use_thumbnails );
	DecalMaskTexture    = MatLoadTexture(DecalMaskName,         "DecalMask"         , GetDownScale( TT_DIFFUSE          ), GetMaxScaleDim( TT_DIFFUSE               ), player, use_thumbnails );
	NormalDetailTexture = MatLoadTexture(DetailNMapName,        "DetailN"           , GetDownScale( TT_NORMALDETAIL     ), GetMaxScaleDim( TT_NORMALDETAIL          ), player, use_thumbnails );
	ThicknessTexture    = MatLoadTexture(DensityMapName,        "DensityMap"        , GetDownScale( TT_DENSITYMAP       ), GetMaxScaleDim( TT_DENSITYMAP            ), player, use_thumbnails );
	EnvPowerTexture     = MatLoadTexture(EnvMapName,            "Roughness"         , GetDownScale( TT_ROUGHNESS        ), GetMaxScaleDim( TT_ROUGHNESS             ), player, use_thumbnails );
	CamouflageMask      = MatLoadTexture(CamouflageMaskName,    "CamouflageMask"    , GetDownScale( TT_CAMOUFLAGEMASK   ), GetMaxScaleDim( TT_CAMOUFLAGEMASK        ), player, use_thumbnails );
	DistortionTexture   = MatLoadTexture(DistortionMapName,     "DistortionMap"     , GetDownScale( TT_DISTORTION       ), GetMaxScaleDim( TT_DISTORTION            ), player, use_thumbnails );
	SpecularPowTexture  = MatLoadTexture(SpecPowMapName,        "DistortionMap"     , GetDownScale( TT_SPECPOW          ), GetMaxScaleDim( TT_SPECPOW               ), player, use_thumbnails );	

	// NOTE : because textures may be delay loaded, this function is called once more in r3dMaterial::Start
	SetAlphaFlag();

	//r3dOutToLog("Material: Material %s [%s] loaded, ID: %d\n", Name, DepotName, ID);

	return TRUE;
}

const char* mlGetFileNameFromTextureHelper( const r3dTexture* tex )
{
	return tex->getFileLoc().FileName;
}

const char* mlGetFileNameFromTextureHelper( const r3dThumbnailTexture* tex )
{
	return tex->GetFilePath();
}

template< typename T>
static const char* mlGetFileNameFromTexture(const T* tex)
{
	const char* f = mlGetFileNameFromTextureHelper( tex );
	const char* p = f;
	if((p=strrchr(f, '\\')) != NULL) return p + 1;
	if((p=strrchr(f, '/')) != NULL) return p + 1;
	return f;
}

template< typename T>
static void mlSaveTextureName(FILE* f, const char* texId, T* tex, r3dMaterial *mat)
{
	if (tex)
	{
		const char *name = mlGetFileNameFromTexture(tex);
		if (mat)
		{
			static char buf[MAX_PATH] = {0};
			sprintf_s(buf, _countof(buf), "%s/%s", mat->OriginalDir, name);
			if (r3d_access(buf, 0))
			{
				r3dOutToLog("Material warning: referenced texture name '%s' in material '%s' does not exist\n", name, mat->Name);
			}
		}
		fprintf(f, "%s= %s\n", texId, name);
	}
}

int r3dMaterial::SaveAscii(FILE* f)
{
	r3d_assert(f);

	fprintf(f, "[MaterialBegin]\n");
	fprintf(f, "Name= %s\n", Name);
	fprintf(f, "Flags= _unused_\n");
	fprintf(f, "SpecularPower= %.2f\n", SpecularPower);
	fprintf(f, "Specular1Power= %.2f\n", SpecularPower1);
	fprintf(f, "ReflectionPower= %.2f\n", ReflectionPower);
	fprintf(f, "DoubleSided= %d\n", (Flags & R3D_MAT_DOUBLESIDED) ? 1 : 0);
	fprintf(f, "Color24= %d %d %d\n", DiffuseColor.R, DiffuseColor.G, DiffuseColor.B);
	fprintf(f, "FGColor= %d %d %d\n", FGlassColor.R, FGlassColor.G, FGlassColor.B);  // FakeGlass
	fprintf(f, "DetailScale= %.2f\n", DetailScale);
	fprintf(f, "DetailAmmount= %.2f\n", DetailAmmount);
	fprintf(f, "displace= %d\n", m_DoDispl ? 1 : 0 );
	fprintf(f, "displ_val= %.3f\n", m_DisplDepthVal );
	fprintf(f, "ForceTransparent= %d\n", (Flags & R3D_MAT_FORCEHASALPHA) ? 1 : 0 );
	fprintf(f, "AlphaTransparent= %d\n", (Flags & R3D_MAT_TRANSPARENT) ? 1 : 0 );
	fprintf(f, "Camouflage= %d\n", (Flags & R3D_MAT_CAMOUFLAGE) ? 1 : 0 );
	fprintf(f, "lowQSelfIllum= %.2f\n", lowQSelfIllum);
	fprintf(f, "lowQMetallness= %.2f\n", lowQMetallness);
	fprintf(f, "SelfIllumMultiplier=%.4f\n", SelfIllumMultiplier);
	fprintf(f, "SnowAmount=%.4f\n", SnowAmount);
	
	fprintf(f, "matID= %d\n", matID);

	if( TypeName[0] )
		fprintf( f, "Type= %s\n", TypeName );

	if(!DepotName[0]) 
		fprintf(f, "ImagesDir= %s\n", OriginalDir);

	mlSaveTextureName(f, "Texture",			Texture, this);
	mlSaveTextureName(f, "NormalMap",		NormalTexture, this);
	mlSaveTextureName(f, "DecalMask",		DecalMaskTexture, this);
	mlSaveTextureName(f, "EnvMap",			EnvPowerTexture, this);
	mlSaveTextureName(f, "SpecularMap",		GlossTexture, this);
	mlSaveTextureName(f, "GlowMap",			SelfIllumTexture, this);
	mlSaveTextureName(f, "DetailNMap",		NormalDetailTexture, this);
	mlSaveTextureName(f, "DensityMap",		ThicknessTexture, this);
	mlSaveTextureName(f, "CamoMask",		CamouflageMask, this);
	mlSaveTextureName(f, "DistortionMap",		DistortionTexture, this);
	mlSaveTextureName(f, "SpecPowMap",		SpecularPowTexture, this);

	fprintf(f, "[MaterialEnd]\n");
	fprintf(f, "\n");

	return TRUE;
}

//------------------------------------------------------------------------

template < typename T >
static void mlCopyTexture(const T* tex, const char* outDir)
{
	if(!tex)
		return;

	const char* in = mlGetFileNameFromTextureHelper( tex );
	char out[MAX_PATH];
	sprintf(out, "%s\\%s", outDir, mlGetFileNameFromTexture(tex));
	r3dOutToLog("copying %s->%s\n", in, out);

	if(::CopyFile(in, out, FALSE) == 0) {
		r3dOutToLog("!!! CopyFile failed, err:%d\n", GetLastError());
	}
}

int r3dMaterial::CopyTextures(const char* outDir)
{
	mlCopyTexture(Texture,      outDir);
	mlCopyTexture(GlossTexture, outDir);
	mlCopyTexture(NormalTexture,  outDir);
	mlCopyTexture(DecalMaskTexture, outDir);
	mlCopyTexture(EnvPowerTexture,  outDir);
	mlCopyTexture(SelfIllumTexture,   outDir);
	mlCopyTexture(NormalDetailTexture, outDir);
	mlCopyTexture(ThicknessTexture, outDir);
	mlCopyTexture(CamouflageMask, outDir);
	mlCopyTexture(DistortionTexture, outDir);
	mlCopyTexture(SpecularPowTexture, outDir);

	return TRUE;
}

static void mlReloadTexture( r3dTexture*& tex, TexType tt )
{
	if( !tex )
		return;

	char FileName[ 1024 ];
	r3dscpy( FileName, tex->getFileLoc().FileName );
	FileName[ 1023 ] = 0;

	tex->SetLoadData( FileName, GetDownScale( tt ), GetMaxScaleDim( tt ), tex->GetPool(), tex->GetD3DFormat() );

	if( tex->GetRefs() )
	{
		tex->DestroyResources( false );
		tex->LoadResources();
	}
}

static void mlReloadTexture( r3dThumbnailTexture*& thumb, TexType tt )
{
	if( !thumb )
		return;

	r3dTexture* tex = thumb->GetFullSizeTexture();

	char FileName[ 1024 ];
	r3dscpy( FileName, tex->getFileLoc().FileName );
	FileName[ 1023 ] = 0;

	tex->SetLoadData( FileName, GetDownScale( tt ), GetMaxScaleDim( tt ), tex->GetPool(), tex->GetD3DFormat() );

	if( tex->GetRefs() )
	{
		tex->DestroyResources( false );
		tex->LoadResources();
	}
}

void r3dMaterial::ReloadTextures()
{
	mlReloadTexture( Texture				, TT_DIFFUSE		);
	mlReloadTexture( GlossTexture			, TT_GLOW			);
	mlReloadTexture( NormalTexture			, TT_NORMAL			);
	mlReloadTexture( DecalMaskTexture		, TT_DIFFUSE		);
	mlReloadTexture( EnvPowerTexture		, TT_ROUGHNESS		);
	mlReloadTexture( SelfIllumTexture		, TT_GLOW			);
	mlReloadTexture( NormalDetailTexture	, TT_NORMALDETAIL	);
	mlReloadTexture( ThicknessTexture		, TT_DENSITYMAP		);
	mlReloadTexture( CamouflageMask			, TT_CAMOUFLAGEMASK );
	mlReloadTexture( DistortionTexture		, TT_DISTORTION		);
	mlReloadTexture( SpecularPowTexture		, TT_SPECPOW		);

	SetAlphaFlag();
}

void r3dMaterial::SetAlphaFlag()
{
	Flags &= ~R3D_MAT_HASALPHA ;

	if( Flags & R3D_MAT_FORCEHASALPHA )
	{
		Flags |= R3D_MAT_HASALPHA ;
	}
	else
	{
		if( Texture && Texture->IsValid())
		{
			D3DFORMAT format = Texture->GetD3DFormat();

			if( format != D3DFMT_DXT1 && format != D3DFMT_R8G8B8 )
			{
				Flags |= R3D_MAT_HASALPHA ;
			}
		}
	}
}

//------------------------------------------------------------------------

void r3dMaterial::TouchTextures()
{
	if( Flags & R3D_MAT_THUMBNAILS )
	{
		if( !TextureRefCount )
		{
			if( Texture ) Texture->LoadFullSizeTexture();
			if( NormalTexture ) NormalTexture->Load();
			if( DecalMaskTexture ) DecalMaskTexture->Load();

			if( GlossTexture ) GlossTexture->Load();
			if( EnvPowerTexture ) EnvPowerTexture->Load();
			if( SelfIllumTexture ) SelfIllumTexture->Load();

			if( ThicknessTexture ) ThicknessTexture->Load();
			if( CamouflageMask ) CamouflageMask->Load();
			if( DistortionTexture ) DistortionTexture->Load();

			if( NormalDetailTexture ) NormalDetailTexture->Load();
			if( SpecularPowTexture ) SpecularPowTexture->Load();
		}

		TextureRefCount ++;
	}
}

//------------------------------------------------------------------------

void r3dMaterial::ReleaseTextures()
{
	if( Flags & R3D_MAT_THUMBNAILS )
	{
		TextureRefCount --;

		r3d_assert( TextureRefCount >= 0 );

		if( !TextureRefCount )
		{
			if( Texture ) Texture->Unload();
			if( NormalTexture ) NormalTexture->Unload();
			if( DecalMaskTexture ) DecalMaskTexture->Unload();

			if( GlossTexture ) GlossTexture->Unload();
			if( EnvPowerTexture ) EnvPowerTexture->Unload();
			if( SelfIllumTexture ) SelfIllumTexture->Unload();

			if( ThicknessTexture ) ThicknessTexture->Unload();
			if( CamouflageMask ) CamouflageMask->Unload();
			if( DistortionTexture ) DistortionTexture->Unload();

			if( NormalDetailTexture ) NormalDetailTexture->Unload();
			if( SpecularPowTexture ) SpecularPowTexture->Unload();
		}
	}
}

void r3dMaterial::GetTextures( r3dTexture* (&texArr)[ TEX_COUNT ] )
{
	texArr[ TEX_DIFFUSE ]			= Texture ? Texture->GetFullSizeTexture() : NULL;
	texArr[ TEX_NORMAL ]			= NormalTexture;
	texArr[ TEX_GLOSS ]				= GlossTexture;
	texArr[ TEX_ENVPOWER ]			= EnvPowerTexture;
	texArr[ TEX_SELFILLUM ]			= SelfIllumTexture;
	texArr[ TEX_NORMALDETAIL ]		= NormalDetailTexture;
	texArr[ TEX_THICKNESS ]			= ThicknessTexture;
	texArr[ TEX_CAMOUFLAGEMASK ]	= CamouflageMask;
	texArr[ TEX_DISTORTION ]		= DistortionTexture;
	texArr[ TEX_SPECULARPOW ]		= SpecularPowTexture;
	texArr[ TEX_DECALMASK ]		= DecalMaskTexture;
}

//------------------------------------------------------------------------

void r3dMaterial::SetTextures( r3dTexture* (&texArr)[ TEX_COUNT ] )
{
	if( r3dTexture * tex = texArr[ TEX_DIFFUSE ] )
	{
		if( Texture )
			Texture->UpdateTexture( texArr[ TEX_DIFFUSE ] );
		else
		{
			Texture = r3dRenderer->LoadThumbnailTexture( tex->getFileLoc().FileName, tex->GetDownScale(), tex->GetMaxDim(), tex->GetPool(), tex->GetFlags() & r3dTexture::fPlayerTexture ? PlayerTexMem : TexMem );
			Texture->LoadFullSizeTexture();
		}
	}
	else
	{
		if( Texture && Texture->GetRefs() )
		{
			Texture->Unload();
		}
		// NOTE : We do not delete the thumbnail wrapper itself. This is actually not supported.
		Texture = NULL;
	}

	NormalTexture			= texArr[ TEX_NORMAL ];
	GlossTexture			= texArr[ TEX_GLOSS ];
	EnvPowerTexture			= texArr[ TEX_ENVPOWER ];
	SelfIllumTexture		= texArr[ TEX_SELFILLUM ];
	NormalDetailTexture		= texArr[ TEX_NORMALDETAIL ];
	ThicknessTexture		= texArr[ TEX_THICKNESS ];
	CamouflageMask			= texArr[ TEX_CAMOUFLAGEMASK ];
	DistortionTexture		= texArr[ TEX_DISTORTION ];
	SpecularPowTexture		= texArr[ TEX_SPECULARPOW ];
	DecalMaskTexture		= texArr[ TEX_DECALMASK ];
}

//------------------------------------------------------------------------

void r3dMaterial::DisableThumbnails()
{
	if( Flags & R3D_MAT_THUMBNAILS )
	{
		if( !TextureRefCount )
		{
			TouchTextures();
		}

		Flags &= ~R3D_MAT_THUMBNAILS;

		TextureRefCount = RefCount;
	}
}

//------------------------------------------------------------------------

void r3dMaterial::EnableThumbnails()
{
	if( !( Flags & R3D_MAT_THUMBNAILS ) )
	{
		Flags |= R3D_MAT_THUMBNAILS;

		// This function is called from editor UI. If the editor UI is outside the of the Map settings 
		// ( where sector loading can be tested in editor ), then r_sector_loading should be automatically
		// turned off, all textures should be loaded on all instanses and the ref count should be maximal
		TextureRefCount = RefCount;
	}
}

/*static*/
r3dColor r3dMaterial::RenderedObjectColor = r3dColor(255, 255, 255, 0);// selfIllumm in alpha component! ::white ;


int	     NumManagedMaterialsInLibrary = 0;
r3dMaterial* ManagedMatlibPointers[4096];

int& r3dMaterialLibrary::GetCurrent(r3dMaterial**& materialsArray)
{
	materialsArray = ManagedMatlibPointers;
	return NumManagedMaterialsInLibrary;
}

void ResetMatFrameScores()
{
	memset( &gMatFrameScores[0], 0, sizeof( gMatFrameScores[0] ) * MAX_MAT_FRAME_SCORES ) ;
}

void r3dMaterialLibrary::Reset()
{
#ifndef WO_SERVER
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;
#endif
	ResetMatFrameScores();

	r3d_assert(NumManagedMaterialsInLibrary == 0);
	NumManagedMaterialsInLibrary = 0;

	memset (&ManagedMatlibPointers[0], 0, sizeof(ManagedMatlibPointers) );

	// Add default material everytime
 	ManagedMatlibPointers[0] = gfx_new r3dMaterial( false );
 	r3dscpy(ManagedMatlibPointers[0]->Name, "-DEFAULT-");
 	NumManagedMaterialsInLibrary = 1;
}

int r3dMaterialLibrary::UnloadManaged()
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	for(int i=0; i<NumManagedMaterialsInLibrary; i++)
	{
		SAFE_DELETE(ManagedMatlibPointers[i]);
	}
	NumManagedMaterialsInLibrary = 0;

	return 1;
}

void	r3dMaterialLibrary::Destroy()
{
	UnloadManaged();
}


int r3dMaterialLibrary::LoadLibrary(const char* szFileName, const char* szTexPath)
{
	r3dFile		*f;
	long		FileStart;
	char		buf[256];
	int		NumMats;

	r3dMaterial** MatlibPointers;
	int NumMaterialsInLibrary = GetCurrent(MatlibPointers);

	r3d_assert(NumMaterialsInLibrary > 0 && "r3dMaterialLibrary::Reset wasn't called");

	// create default library.
	if((f=r3d_open(szFileName, "rt")) == NULL) 
	{
		r3dArtBug("MatLib: can't open %s\n", szFileName);
		return 0;
	}

	// detect number of materials
	NumMats   = 0;
	FileStart = ftell(f);
	while(!feof(f))
	{
		if(fgets(buf, sizeof(buf), f) == NULL)
			break;
		if(strstr(buf, "[MaterialBegin]") != NULL)
			NumMats++;
	}
	fseek(f, FileStart, SEEK_SET);
	r3dOutToLog("MatLib: Loading %s, %d materials\n", szFileName, NumMats);

	for (int i=0;i<NumMats;i++)
	{
		MatlibPointers[NumMaterialsInLibrary] = gfx_new r3dMaterial( false );
		if (MatlibPointers[NumMaterialsInLibrary]->LoadAscii(f, szTexPath))
			NumMaterialsInLibrary ++;
		else
			SAFE_DELETE(MatlibPointers[NumMaterialsInLibrary]);
	}

	fclose(f);

	return 1;
}

const int MaxExtendedMaterialInfo = 256;
static int parsedExtendedMaterials = 0;
static r3dTexture  * materialsTexture = NULL;
static r3dString materialNames[MaxExtendedMaterialInfo];
static float fresnelCoeffs[MaxExtendedMaterialInfo][4] = {0};
static float specularColor[MaxExtendedMaterialInfo][4] = {0};

int r3dMaterialLibrary::GetExtendedMaterialDataCount()
{
	return parsedExtendedMaterials;
}


void r3dMaterialLibrary::UpdateExtendedMaterialData(int index, float fresnel[4], float color[4])
{
	r3d_assert(index < MaxExtendedMaterialInfo);

	fresnelCoeffs[index][0] = fresnel[0];
	fresnelCoeffs[index][1] = fresnel[1];
	fresnelCoeffs[index][2] = fresnel[2];
	fresnelCoeffs[index][3] = fresnel[3];

	specularColor[index][0] = color[0];
	specularColor[index][1] = color[1];
	specularColor[index][2] = color[2];
	specularColor[index][3] = color[3];
}

r3dString r3dMaterialLibrary::GetExtendedMaterialName(int index)
{
	r3d_assert(index < MaxExtendedMaterialInfo);

	if (index < parsedExtendedMaterials)
		return materialNames[index];
	else
		return r3dString("NotValidMaterial");
}

void r3dMaterialLibrary::UpdateExtendedMaterialsName(int index, const char * newName)
{
	r3d_assert(index < MaxExtendedMaterialInfo);

	if (index < parsedExtendedMaterials)
		materialNames[index] = newName;
}

void r3dMaterialLibrary::GetExtendedMaterialData(int index, float (&fresnel)[4], float (&color)[4])
{
	r3d_assert(index < MaxExtendedMaterialInfo);

	fresnel[0] = fresnelCoeffs[index][0];
	fresnel[1] = fresnelCoeffs[index][1];
	fresnel[2] = fresnelCoeffs[index][2];
	fresnel[3] = fresnelCoeffs[index][3];

	color[0] = specularColor[index][0];
	color[1] = specularColor[index][1];
	color[2] = specularColor[index][2];
	color[3] = specularColor[index][3];
}

void r3dMaterialLibrary::LoadExtendedMaterialParameters(const char * szFileName)
{
	r3d_assert(szFileName);
	r3d_assert(parsedExtendedMaterials == 0);

	r3dFile* f = r3d_open( szFileName, "rb" );
	if ( ! f )
	{
		r3dArtBug( "r3dMaterialLibrary::LoadExtendedMaterialParameters Failed to load %s", szFileName );
		return;
	}

	r3dTL::TArray< char > fileBuffer( f->size + 1 );

	fread( &fileBuffer[ 0 ], f->size, 1, f );
	fileBuffer[ f->size ] = 0;

	pugi::xml_document xmlLevelFile;
	pugi::xml_parse_result parseResult = xmlLevelFile.load_buffer_inplace( &fileBuffer[0], f->size );	
	fclose( f );

	if( !parseResult )
	{
		r3dArtBug( "r3dMaterialLibrary::LoadExtendedMaterialParameters: Failed to parse %s, error: %s", szFileName, parseResult.description() );
		return;
	}

	pugi::xml_node root = xmlLevelFile.child( "materials" );
	if (root.empty())
	{
		r3dArtBug( "r3dMaterialLibrary::LoadExtendedMaterialParameters: No 'materials' root node");
		return;
	}

	pugi::xml_node xmlDefDec = root.child( "material0" );

	char nodeName[256];


	while( !xmlDefDec.empty() )
	{

		materialNames[parsedExtendedMaterials] = xmlDefDec.attribute( "name" ).value();
		pugi::xml_node fresnel = xmlDefDec.child( "fresnel" );
		if (fresnel.empty())
		{
			r3dArtBug( "r3dMaterialLibrary::LoadExtendedMaterialParameters: Material %d missing 'fresnel' node", parsedExtendedMaterials);
			return;
		}

		pugi::xml_node specular = xmlDefDec.child( "specular" );
		if (specular.empty())
		{
			r3dArtBug( "r3dMaterialLibrary::LoadExtendedMaterialParameters: Material %d missing 'specular' node", parsedExtendedMaterials);
			return;
		}

		pugi::xml_node blend = xmlDefDec.child( "blend" );
		if (blend.empty())
		{
			r3dArtBug( "r3dMaterialLibrary::LoadExtendedMaterialParameters: Material %d missing 'blend' node", parsedExtendedMaterials);
			return;
		}

		fresnelCoeffs[parsedExtendedMaterials][0] = fresnel.attribute( "x" ).as_float();
		fresnelCoeffs[parsedExtendedMaterials][1] = fresnel.attribute( "y" ).as_float();
		fresnelCoeffs[parsedExtendedMaterials][2] = fresnel.attribute( "z" ).as_float();
		fresnelCoeffs[parsedExtendedMaterials][3] = fresnel.attribute( "w" ).as_float();

		specularColor[parsedExtendedMaterials][0] = specular.attribute( "r" ).as_float() / 255.0f;
		specularColor[parsedExtendedMaterials][1] = specular.attribute( "g" ).as_float() / 255.0f;
		specularColor[parsedExtendedMaterials][2] = specular.attribute( "b" ).as_float() / 255.0f;
		specularColor[parsedExtendedMaterials][3] = blend.attribute( "x" ).as_float();


		++parsedExtendedMaterials;
		sprintf_s(nodeName, "material%d", parsedExtendedMaterials);
		xmlDefDec = root.child( nodeName );

		r3d_assert(parsedExtendedMaterials <= MaxExtendedMaterialInfo);
	}

	UpdateExtendedMaterialsTexture();
}

void r3dMaterialLibrary::SaveExtendedMaterialParameters(const char * szFileName)
{
	r3d_assert(szFileName);

	pugi::xml_document xmlLevelFile;
	pugi::xml_node root = xmlLevelFile.append_child( ) ;
	root.set_name( "materials" );

	for (int i = 0; i < parsedExtendedMaterials; ++i)
	{
		pugi::xml_node material = root.append_child( ) ;
		char materialName[256];
		sprintf_s(materialName, "material%d", i);

		material.set_name( materialName );
		material.append_attribute("name").set_value(materialNames[i].c_str());

		pugi::xml_node fresnel = material.append_child( ) ;
		fresnel.set_name( "fresnel" );

		pugi::xml_node specular = material.append_child( ) ;
		specular.set_name( "specular" );

		pugi::xml_node blend = material.append_child( ) ;
		blend.set_name( "blend" );

		fresnel.append_attribute("x").set_value(fresnelCoeffs[i][0]);
		fresnel.append_attribute("y").set_value(fresnelCoeffs[i][1]);
		fresnel.append_attribute("z").set_value(fresnelCoeffs[i][2]);
		fresnel.append_attribute("w").set_value(fresnelCoeffs[i][3]);

		specular.append_attribute("r").set_value((int)(specularColor[i][0] * 255.0f));
		specular.append_attribute("g").set_value((int)(specularColor[i][1] * 255.0f));
		specular.append_attribute("b").set_value((int)(specularColor[i][2] * 255.0f));

		blend.append_attribute("x").set_value((int)(specularColor[i][3]));
	}

	xmlLevelFile.save_file(szFileName);
}

static const char * defaultExtendedMaterialsFileName = "Data\\ExtendedMaterials.xml";

void r3dMaterialLibrary::LoadDefaultExtendedMaterialParameters()
{
	LoadExtendedMaterialParameters(defaultExtendedMaterialsFileName);
}

void r3dMaterialLibrary::SaveDefaultExtendedMaterialParameters()
{
	SaveExtendedMaterialParameters(defaultExtendedMaterialsFileName);
}

void r3dMaterialLibrary::UpdateExtendedMaterialsTexture()
{
	// create texture

	materialsTexture = r3dRenderer->AllocateTexture();
	materialsTexture->Create(256, 2, D3DFMT_A16B16G16R16F, 1);

	void * ptr = materialsTexture->Lock(1);

	short int* spec = (short int*)ptr;
	short int* fres = (short int*)((char*)ptr +materialsTexture->GetLockPitch());
	for (int i = 0; i < parsedExtendedMaterials; ++i)
	{
		float r = pow( specularColor[i][0], 2.2f ) ;
		float g = pow( specularColor[i][1], 2.2f ) ;
		float b = pow( specularColor[i][2], 2.2f ) ;

		*(spec++) = r3dFloatToHalf(r);
		*(spec++) = r3dFloatToHalf(g);
		*(spec++) = r3dFloatToHalf(b);
		*(spec++) = r3dFloatToHalf(specularColor[i][3]);

		*(fres++) = r3dFloatToHalf(fresnelCoeffs[i][0]);
		*(fres++) = r3dFloatToHalf(fresnelCoeffs[i][1]);
		*(fres++) = r3dFloatToHalf(fresnelCoeffs[i][2]);

		float fres_w = fresnelCoeffs[i][3] * 2.2f;
		*(fres++) = r3dFloatToHalf(fres_w);
	}

	materialsTexture->Unlock();
}

void r3dMaterialLibrary::SetupExtendedMaterialParameters()
{
	r3dRenderer->SetTex(materialsTexture, 15);
}

r3dMaterial* r3dMaterialLibrary::FindMaterial(const char* MatName)
{
//	r3d_assert(NumMaterialsInLibrary > 0 && "r3dMaterialLibrary::Reset wasn't called");
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	if(_r3d_MatLib_SkipAllMaterials)
		return ManagedMatlibPointers[0];

	r3dMaterial* m = HasMaterial(MatName);
	
	return m ? m : ManagedMatlibPointers[0];
}

r3dMaterial* r3dMaterialLibrary::HasMaterial(const char* MatName)
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	int NumMaterialsInLibrary;
	r3dMaterial** MatlibPointers;
	NumMaterialsInLibrary = GetCurrent(MatlibPointers);
	for (int i=0;i<NumMaterialsInLibrary;i++)
		if(!stricmp(MatName, MatlibPointers[i]->Name))
			return MatlibPointers[i];

	return 0;
}

/*static*/
r3dMaterial*
r3dMaterialLibrary::GetDefaultMaterial()
{
	r3d_assert( NumManagedMaterialsInLibrary > 0 );
	ManagedMatlibPointers[ 0 ]->RefCount ++;

	// 0 is supposed to always be default material.
	return ManagedMatlibPointers[ 0 ];
}

int r3dMaterialLibrary::SaveLibrary(const char* szFileName, const char* szTexPath, int bCompact)
{
	r3dOutToLog("r3dMaterialLibrary::SaveLibrary to %s, texDir:%s\n", szFileName, szTexPath);
	CLOG_INDENT;

	FILE* f = fopen(szFileName, "wt");
	if(!f) {
		r3dError("can't open %s for writing\n", szFileName);
		return 0;
	}

	int NumMaterialsInLibrary;
	r3dMaterial** MatlibPointers;

	NumMaterialsInLibrary = GetCurrent(MatlibPointers);
	for(int i=0;i<NumMaterialsInLibrary;i++) {
		MatlibPointers[i]->SaveAscii(f);
	}

	fclose(f);

	// create texture directory
	CreateDirectory(szTexPath, NULL);

	// copy textures
	r3dOutToLog("Copying textures\n"); CLOG_INDENT;

	NumMaterialsInLibrary = GetCurrent(MatlibPointers);
	for(int i=0;i<NumMaterialsInLibrary;i++) 
		MatlibPointers[i]->CopyTextures(szTexPath);

	return 1;
}

/*static*/ int r3dMaterialLibrary::SaveMaterialSources()
{
	int NumMaterialsInLibrary;
	r3dMaterial** MatlibPointers;

	NumMaterialsInLibrary = GetCurrent(MatlibPointers);
	for(int i=0;i<NumMaterialsInLibrary;i++)
	{
		r3dMaterial* EditMaterial = MatlibPointers[i];

		char MatFile[ MAX_PATH ];
		MakeMaterialFileName( MatFile, EditMaterial->Name, EditMaterial->DepotName );

		if( _access_s( MatFile, 0 ) == 0 )
		{
			FILE* file = fopen( MatFile, "wt" ) ;

			EditMaterial->SaveAscii( file ) ;

			fclose( file ) ;
		}
	}

	return 1;
}

/*static */ void r3dMaterialLibrary::ReloadMaterialTextures()
{
	int NumMaterialsInLibrary;
	r3dMaterial** MatlibPointers;

	NumMaterialsInLibrary = GetCurrent(MatlibPointers);
	for(int i=0;i<NumMaterialsInLibrary;i++) 
		MatlibPointers[i]->ReloadTextures();
}

//
// handling depot per-file dynamic materials
//
bool	r3dMaterialLibrary::IsDynamic = false;

void MakeMaterialFileName( char* MatFile, const char* MatName, const char* DepotName )
{
	sprintf( MatFile, "%s\\%s\\Materials\\%s.mat", DEPOT_TAG, DepotName, MatName );
}

r3dMaterial* r3dMaterialLibrary::RequestMaterialByMesh(const char* MatName, const char* szMeshNameIn, bool playerMaterial, bool use_thumbnails )
{

//	r3d_assert(NumMaterialsInLibrary > 0 && "r3dMaterialLibrary::Reset wasn't called");
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	if(_r3d_MatLib_SkipAllMaterials)
		return ManagedMatlibPointers[0];

	char MeshName[ MAX_PATH ];
	FixFileName( szMeshNameIn, MeshName );

	// see if we already have it
	r3dMaterial* m = HasMaterial(MatName);
	if(m)
	{
		if( !use_thumbnails )
		{
			m->TouchTextures();
		}
		
		m->RefCount ++;

		return m;
	}

	// determine depot
	if(strnicmp(MeshName, DEPOT_TAG, DEPOT_TLEN) != 0) {
		r3dArtBug("MatLib: can't determine depot from file %s\n", MeshName);
		return ManagedMatlibPointers[0];
	}
	char depot[MAX_PATH];
	char* p;
	r3dscpy(depot, MeshName + DEPOT_TLEN + 1);
	if((p = strrchr(depot, '/')) != 0) *p = 0;
	if((p = strrchr(depot, '\\')) != 0) *p = 0;

	return RequestMaterialByName(MatName, depot, playerMaterial, use_thumbnails);
}

//////////////////////////////////////////////////////////////////////////

r3dMaterial* r3dMaterialLibrary::RequestMaterialByName(const char* MatName, const char* depot, bool playerMaterial, bool use_thumbnails )
{
	if(_r3d_MatLib_SkipAllMaterials)
		return ManagedMatlibPointers[0];

	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	// make material file name
	char matname[MAX_PATH];

	// see if we already have it
	r3dMaterial* m = HasMaterial(MatName);
	if(m)
	{
		if( !use_thumbnails )
		{
			m->TouchTextures();
		}
		
		m->RefCount ++;

		return m;
	}

	MakeMaterialFileName( matname, MatName, depot ) ;

	if(r3d_access(matname, 0) != 0) {
		r3dArtBug("MatLib: there is no material %s\n", matname);
		return ManagedMatlibPointers[0];
	}

	// texture dir
	char texpath[MAX_PATH];
	sprintf(texpath, "%s/%s/Textures", DEPOT_TAG, depot);

	// load it
	r3dFile* f = r3d_open(matname, "rt");
	r3d_assert(f);

	r3dMaterial* mat = gfx_new r3dMaterial( use_thumbnails );

	if( playerMaterial )
	{
		mat->Flags |= R3D_MAT_PLAYER;
	}

	r3dscpy(mat->DepotName, depot);
	if(!mat->LoadAscii(f, texpath)) 
	{
		r3dArtBug("MatLib: failed to load material %s\n", matname);
		delete mat;
		fclose(f);

		return ManagedMatlibPointers[0];
	}
	fclose(f);

	r3dMaterial** MatlibPointers;
	int& NumMaterialsInLibrary = GetCurrent(MatlibPointers);


	// add to matlib
	MatlibPointers[NumMaterialsInLibrary] = mat;
	NumMaterialsInLibrary ++;

	mat->RefCount = 1;

	return mat;
}

//////////////////////////////////////////////////////////////////////////


void r3dMaterialLibrary::UpdateDepotMaterials()
{
	r3dMaterial** MatlibPointers;
	int NumMaterialsInLibrary = GetCurrent(MatlibPointers);

	r3d_assert(NumMaterialsInLibrary > 0 && "r3dMaterialLibrary::Reset wasn't called");

	for(int i=1; i<NumMaterialsInLibrary; i++)
	{
		r3dMaterial* mat = MatlibPointers[i];
		if(mat->DepotName[0] == 0)
			continue;

		if(!mat->WasEdited)
			continue;

		char matname[MAX_PATH];
		sprintf(matname, "%s\\%s\\Materials\\%s.mat", DEPOT_TAG, mat->DepotName, mat->Name);
		FILE* f = fopen(matname, "wt");
		if(f == NULL) {
			r3dOutToLog("MatLib: can't open %s for writing\n", matname);
			continue;
		}
		mat->SaveAscii(f);
		fclose(f);
	}

	return;
}

//------------------------------------------------------------------------
/*static */

void
r3dMaterialLibrary::UnloadManagedMaterial( r3dMaterial* mat )
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	int foundIdx = -1 ;

	for( int i = 0, e = NumManagedMaterialsInLibrary ; i < e ; i ++ )
	{
		if( ManagedMatlibPointers[ i ] == mat )
		{
			foundIdx = i ;
			break ;
		}
	}

	if( foundIdx < 0 )
	{
		r3dOutToLog( "r3dMaterialLibrary::UnloadMaterial: material %s(%p) is not in the library!\n", mat->Name, mat ) ;
		return ;
	}

	mat->RefCount -- ;

	if( mat->RefCount <= 0 )
	{
		if( mat->RefCount < 0 )
		{
			r3dOutToLog( "r3dMaterialLibrary::UnloadMaterial: negative reference count!\n" ) ;
		}

		SAFE_DELETE( mat ) ;

		for( int i = foundIdx ; i < NumManagedMaterialsInLibrary ; i ++ )
		{
			ManagedMatlibPointers[ i ] = ManagedMatlibPointers[ i + 1 ] ;
		}

		NumManagedMaterialsInLibrary -- ;

		ManagedMatlibPointers[ NumManagedMaterialsInLibrary ] = NULL ;
	}

}

//------------------------------------------------------------------------

void r3dInitMaterials()
{
	FillVSShaderMaps();
	r3dMaterialLibrary::LoadDefaultExtendedMaterialParameters();
}

void r3dCloseMaterials()
{
	r3dRenderer->DeleteTexture(materialsTexture, 1);
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------

SMDepthVShaderID::SMDepthVShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void SMDepthVShaderID::ToString( char* str )
{
	strcpy( str, "VS_SMDEPTH" );

	if( apex )
	{
		strcat( str, "_APEX" );
	}

	if( intanced )
	{
		strcat( str, "_INSTANCED" );
	}

	strcat( str, "_TYPE" );
	strcat( str, DigitToCString( type ) );
}

//------------------------------------------------------------------------

void SMDepthVShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 3 );

	defines[ 0 ].Name		= "APEX";
	defines[ 0 ].Definition	= DigitToCString( apex );

	defines[ 1 ].Name		= "INSTANCED";
	defines[ 1 ].Definition	= DigitToCString( apex );

	defines[ 2 ].Name		= "SHADOW_TYPE";
	defines[ 2 ].Definition	= DigitToCString( type );
}

//------------------------------------------------------------------------

SMDepthVShaderIDs gSMDepthVSIds;

//------------------------------------------------------------------------

SkinSMDepthVShaderID::SkinSMDepthVShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void SkinSMDepthVShaderID::ToString( char* str )
{
	strcpy( str, "VS_SKIN_SMDEPTH" );

	strcat( str, "_TYPE" );
	strcat( str, DigitToCString( type ) );
}

//------------------------------------------------------------------------

void SkinSMDepthVShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 1 );

	defines[ 0 ].Name		= "SHADOW_TYPE";
	defines[ 0 ].Definition	= DigitToCString( type );
}

//------------------------------------------------------------------------

SkinSMDepthVShaderIDs gSkinSMDepthVSIds;

//------------------------------------------------------------------------

ForestVShaderID::ForestVShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void ForestVShaderID::ToString( char* str )
{
	strcpy( str, "VS_FOREST" );

	if( noninstanced )
	{
		strcat( str, "_NONINSTANCED" );
	}

	if( animated )
	{
		strcat( str, "_ANIMATED" );
	}

	if( shadows )
	{
		strcat( str, "_SHADOWS" );
	}

	strcat( str, "_SHADOWTYPE" );
	strcat( str, DigitToCString(shadow_type) );
}

//------------------------------------------------------------------------

void ForestVShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 4 );

	defines[ 0 ].Name		= "NONINSTANCED_RENDER";
	defines[ 0 ].Definition	= DigitToCString( noninstanced );

	defines[ 1 ].Name		= "ANIMATED";
	defines[ 1 ].Definition	= DigitToCString( animated );

	defines[ 2 ].Name		= "SHADOW_PASS";
	defines[ 2 ].Definition	= DigitToCString( shadows );

	defines[ 3 ].Name		= "SHADOW_TYPE";
	defines[ 3 ].Definition	= DigitToCString( shadow_type );
}

//------------------------------------------------------------------------

ForestVShaderIDs gForestVSIds;

//------------------------------------------------------------------------

SMDepthPShaderID::SMDepthPShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void SMDepthPShaderID::ToString( char* str )
{
	strcpy( str, "PS_SMDEPTH" );

	strcat( str, "_TYPE" );
	strcat( str, DigitToCString( type ) );
}

//------------------------------------------------------------------------

void SMDepthPShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 1 );

	defines[ 0 ].Name		= "SHADOW_TYPE";
	defines[ 0 ].Definition = DigitToCString( type );
}

//------------------------------------------------------------------------

SMDepthPShaderIDs gSMDepthPSIds;

//------------------------------------------------------------------------

const char* DigitToCString( int digit )
{
	r3d_assert( digit < 10 && digit >= 0 ) ;

#define DIG_CASE(num) case num: return #num ;
	switch( digit )
	{
		DIG_CASE(0)
		DIG_CASE(1)
		DIG_CASE(2)
		DIG_CASE(3)
		DIG_CASE(4)
		DIG_CASE(5)
		DIG_CASE(6)
		DIG_CASE(7)
		DIG_CASE(8)
		DIG_CASE(9)
	}
#undef DIG_CASE

	return "" ;
}

//------------------------------------------------------------------------

const char* TwoDigitsToCString( int digit )
{
	r3d_assert( digit < 100 && digit >= 0 ) ;

#define DIG_CASE(num) case num: return #num ;
	switch( digit )
	{
		DIG_CASE(0)DIG_CASE(10)DIG_CASE(30)DIG_CASE(40)DIG_CASE(50)DIG_CASE(60)DIG_CASE(70)DIG_CASE(80)DIG_CASE(90)
		DIG_CASE(1)DIG_CASE(11)DIG_CASE(31)DIG_CASE(41)DIG_CASE(51)DIG_CASE(61)DIG_CASE(71)DIG_CASE(81)DIG_CASE(91)
		DIG_CASE(2)DIG_CASE(12)DIG_CASE(32)DIG_CASE(42)DIG_CASE(52)DIG_CASE(62)DIG_CASE(72)DIG_CASE(82)DIG_CASE(92)
		DIG_CASE(3)DIG_CASE(13)DIG_CASE(33)DIG_CASE(43)DIG_CASE(53)DIG_CASE(63)DIG_CASE(73)DIG_CASE(83)DIG_CASE(93)
		DIG_CASE(4)DIG_CASE(14)DIG_CASE(34)DIG_CASE(44)DIG_CASE(54)DIG_CASE(64)DIG_CASE(74)DIG_CASE(84)DIG_CASE(94)
		DIG_CASE(5)DIG_CASE(15)DIG_CASE(35)DIG_CASE(45)DIG_CASE(55)DIG_CASE(65)DIG_CASE(75)DIG_CASE(85)DIG_CASE(95)
		DIG_CASE(6)DIG_CASE(16)DIG_CASE(36)DIG_CASE(46)DIG_CASE(56)DIG_CASE(66)DIG_CASE(76)DIG_CASE(86)DIG_CASE(96)
		DIG_CASE(7)DIG_CASE(17)DIG_CASE(37)DIG_CASE(47)DIG_CASE(57)DIG_CASE(67)DIG_CASE(77)DIG_CASE(87)DIG_CASE(97)
		DIG_CASE(8)DIG_CASE(18)DIG_CASE(38)DIG_CASE(48)DIG_CASE(58)DIG_CASE(68)DIG_CASE(78)DIG_CASE(88)DIG_CASE(98)
		DIG_CASE(9)DIG_CASE(19)DIG_CASE(39)DIG_CASE(49)DIG_CASE(59)DIG_CASE(69)DIG_CASE(79)DIG_CASE(89)DIG_CASE(99)
	}
#undef DIG_CASE

	return "" ;
}

//------------------------------------------------------------------------
