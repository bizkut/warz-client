#include "r3dPCH.h"
#include "r3d.h"
#include "r3dLight.h"
#include "d3dfont.h"
#include "r3dConePrim.h"

#include "../GameEngine/TrueNature2/Terrain3.h"
#include "../GameEngine/TrueNature2/Terrain2.h"

#include "GameCommon.h"
#include "GameLevel.h"

#include "CommonPostFX.h"
#include "RenderDeferredPointLightsOptimized.h"

#include "rendering/Deffered/CommonPostFX.h"
#include "rendering/Deffered/PostFXChief.h"

#include "ObjectsCode/world/DecalChief.h"
#include "ObjectsCode/world/EnvmapProbes.h"
#include "ObjectsCode/Nature/GrassMap.h"

#include "Editors/LevelEditor_Collections.h"

#include "../../ObjectsCode/weapons/ExplosionVisualController.h"
#include "../../ObjectsCode/weapons/FlashbangVisualController.h"
#include "../../ObjectsCode/ai/AI_Player.H"
#include "../../Editors/CollectionsManager.h"

#include "Particle.h"

#include "../SF/Console/Config.h"

#include "rendering/Deffered/D3DMiscFunctions.h"

#include "..\..\..\bin\Data\Shaders\DX9_P1\system\LibSM\shadow_config.h" // shader config file

r3dScreenBuffer*	DynamicEnvMap0;
r3dScreenBuffer*	DynamicEnvMap1;

r3dScreenBuffer*	AvgSceneLuminanceBuffer;
r3dScreenBuffer*	SceneExposure0;
r3dScreenBuffer*	SceneExposure1;

r3dD3DQuery*		AntiCheatQuery;
r3dScreenBuffer*	PrevDepthBuffer;
r3dScreenBuffer*	PrevSSAOBuffer;

r3dScreenBuffer*	CurrentSSAOBuffer;

r3dScreenBuffer*	Scaleform_RenderToTextureRT;

#define BUFSIZE 512
#define REFBUFSIZE 256
#define LOW_Q_SHADOWS_MAX_DIST 45.0f

r3dScreenBuffer*	gBuffer_StagedRender_Color;		// rgb - albedo, a - gloss
r3dScreenBuffer*	gBuffer_StagedRender_Normal;	// rgb - normal, a - self illum
r3dScreenBuffer*	gBuffer_StagedRender_Depth;		// R32F - depth
r3dScreenBuffer*	gBuffer_StagedRender_Temporary;

r3dScreenBuffer*	gBuffer_Color;	// rgb - albedo, a - gloss
r3dScreenBuffer*	gBuffer_Normal;			// rgb - normal, a - self illum
r3dScreenBuffer*	gBuffer_Depth;			// R32F - depth
r3dScreenBuffer*	gBuffer_PrimaryDepth;	// R32F depth for objects
r3dScreenBuffer*	gBuffer_SecondaryDepth;	// R32F depth for level geomtry (e.g. terrain)
r3dScreenBuffer*	gBuffer_HalvedR32F_0;	// R32F for depth blurring
r3dScreenBuffer*	gBuffer_HalvedR32F_1;	// R32F for depth blurring
r3dScreenBuffer*	gBuffer_Aux;  // r - mat ID channel, gb - motion blur velocities, a - reflectivity

r3dScreenBuffer*	gBuffer_Particles;  // rgb - Light transfer function params, a - self illum

r3dScreenBuffer*	gScreenSmall;  // 1/4th of screen size
r3dScreenBuffer*	gScreenSmall2;  // 1/4th of screen size


struct GBuffer_SSR_Info
{
	static r3dScreenBuffer*	gBuffer_SSR;	// ssr reflections
	static r3dScreenBuffer*	gBuffer_SSR_PreviousFrame;	// ssr previuos frame
	static bool gDeviceCreated;

	enum eSSRMode
	{
		eUseScreenBufferCopy,
		eUseOwnCopy
	};

	static eSSRMode gSSRMode;

	static void CreateBuffers()
	{
		if (!gDeviceCreated )
			return;

		float rtWidth ;
		float rtHeight ;

		GetDesiredRTDimmensions( &rtWidth, &rtHeight ) ;

		//	Choose high precision format if supported
		D3DFORMAT reflectionFromat = ScreenBuffer->BufferFormat;

		// Should not be 0 here. DeleteSSRBuffer should be called instead!
		r3d_assert(r_use_ssr->GetInt());

		// SSR modes: O - disabled, 1 - full res, 2 - half res, 3 - quad res
		int divider = 1 <<  (r_use_ssr->GetInt() - 1);
		rtWidth /= (float)divider;
		rtHeight /= (float)divider;

		if (gBuffer_SSR && gBuffer_SSR->Tex)
		{
			// Do we have exactly the same size?
			if (gBuffer_SSR->Tex->GetWidth() == rtWidth && gBuffer_SSR->Tex->GetHeight() == rtHeight)
				return;

			DeleteBuffers();
		}

		int mipCount = (int)log(R3D_MIN(rtWidth, rtHeight)) + 1;

		gBuffer_SSR = r3dScreenBuffer::CreateClass("gBuffer_Reflections", rtWidth, rtHeight, reflectionFromat, r3dScreenBuffer::Z_NO_Z, FALSE, mipCount );


		// TODO! Need to deal with hdr format, can't just use backbuffer copy
		//if (r_use_ssr->GetInt() == 1)
		//{
			gSSRMode = eUseOwnCopy;
		//	// Use temporary copy only for mode 1. For downgraded versions use ScreenBuffer copy (that's much cheaper), but we will have no reflections from transparent objects
			gBuffer_SSR_PreviousFrame = r3dScreenBuffer::CreateClass("gBuffer_Reflections", rtWidth, rtHeight, reflectionFromat, r3dScreenBuffer::Z_NO_Z, FALSE, mipCount );
		//}
		//else
		//	gSSRMode = eUseScreenBufferCopy;

	}

	static void DeleteBuffers()
	{
		SAFE_DELETE( gBuffer_SSR );
		SAFE_DELETE( gBuffer_SSR_PreviousFrame );
	}

	static void SSRVarChangeCallback(int oldI, float oldF)
	{
		if (oldI == r_use_ssr->GetInt())
			return;

		if (oldI)
		{
			DeleteBuffers();
		}

		if (r_use_ssr->GetInt())
			CreateBuffers();

		g_pPostFXChief->ResetBuffers();
	}

	static void UpdateFrame()
	{
		if (gSSRMode == eUseOwnCopy)
		{
			IDirect3DSurface9	*src;
			IDirect3DSurface9	*dst;
			//D3D_V(r3dRenderer->pd3ddev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &src ));
			D3D_V(ScreenBuffer->Tex->AsTex2D()->GetSurfaceLevel(0, &src));
			r3d_assert(gBuffer_SSR_PreviousFrame);
			D3D_V(gBuffer_SSR_PreviousFrame->Tex->AsTex2D()->GetSurfaceLevel(0, &dst));
			D3D_V(r3dRenderer->pd3ddev->StretchRect( src, NULL, dst, NULL, D3DTEXF_POINT));
			src->Release();
			dst->Release();
		}
	}

	static void BindFrameTexture()
	{
		if (gSSRMode == eUseOwnCopy)
			r3dRenderer->SetTex(gBuffer_SSR_PreviousFrame->Tex);
		else
			r3dRenderer->SetTex(ScreenBuffer->Tex);
	}
} ;



r3dScreenBuffer* GBuffer_SSR_Info::gBuffer_SSR = NULL;
r3dScreenBuffer* GBuffer_SSR_Info::gBuffer_SSR_PreviousFrame = NULL;
GBuffer_SSR_Info::eSSRMode GBuffer_SSR_Info::gSSRMode = eUseScreenBufferCopy;
bool GBuffer_SSR_Info::gDeviceCreated = false;

static GBuffer_SSR_Info gBuffer_SSR;

r3dScreenBuffer* GetSSRBuffer()
{
	return gBuffer_SSR.gBuffer_SSR;
}

r3dScreenBuffer* GetPrevSSRBuffer()
{
	return gBuffer_SSR.gBuffer_SSR_PreviousFrame;
};
//r3dTexture*			gNoiseTexture2 = NULL;

void LoadGrassShaders(bool reload)
{
	{
		for( int do_clip = 0 ; do_clip < 2 ; do_clip ++ )
		{
			for( int aux = 0 ; aux < 2 ; aux ++ )
			{
				for( int depth = 0; depth < 2; depth ++ )
				{
					for( int winter = 0; winter < 2; winter ++ )
					{

						ShaderMacros macros;

						GrassPShaderID psid;

						psid.aux			= aux;
						psid.do_clip		= do_clip;
						psid.output_depth	= depth;

						psid.FillMacros( macros );

						char name[ 256 ];
						psid.ToString( name );

						if (r_show_winter->GetBool())
						{
							int count = macros.Count();

							macros.Resize(count + 1);
							macros[ count ].Name		= "WINTER";
							macros[ count ].Definition	= "1";
						}

						if (reload)
							r3dRenderer->ReloadPixelShader( name, "Nature\\Grass2_ps.hls", macros);
						else
							gGrassPSIds[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\Grass2_ps.hls", 0, macros );
					}
				}
			}
		}
	}
}

void LoadFillBufferShaders(bool reload)
{
	FillbufferShaderKey key;
	for (int i = 0; i < 2; ++i)
	{
		key.flags.simple = i == 0 ? 0 : 1 ;

		for (int i = 0; i < 2; ++i)
		{
			key.flags.disp = i;

			const char * sourceName = NULL ;

			if( key.flags.simple )
			{
				sourceName = "DS_Simple_PS.hls";
			}
			else
			{
				sourceName = "DS_fillbuffer_disp_ps.hls";
				if (i == 0)
				{
					sourceName = "DS_Fillbuffer_PS.hls";
				}
			}

			for (int i = 0; i < 2; ++i)
			{
				key.flags.low_q = i;
				for (int i = 0; i < 2; ++i)
				{
					key.flags.alpha_test = i;
					for (int i = 0; i < 2; ++i)
					{
						key.flags.roads = i;
						for (int i = 0; i < 2; ++i)
						{
							key.flags.camouflage = i;
							for (int i = 0; i < 2; ++i)
							{

								key.flags.instancing = i;
								key.flags.unused_ = 0 ;

								ShaderMacros macros;
								FillMacrosForShaderKey(key, macros);

								if (r_show_winter->GetBool())
								{
									int count = macros.Count();

									macros.Resize(count + 1);
									macros[ count ].Name		= "WINTER";
									macros[ count ].Definition	= "1";
								}

								if (reload)
									r3dRenderer->ReloadPixelShader( ConstructShaderNameForKey(key).c_str(), sourceName, macros);
								else
									gFillbufferPixelShaders[key.key] = r3dRenderer->AddPixelShaderFromFile(ConstructShaderNameForKey(key).c_str(), sourceName, 0, macros);
							}
						}
					}
				}
			}
		}
	}
}

void ReloadWaterShaders()
{
	ShaderMacros macros( 2 );
	macros[ 0 ].Name	= "WATER_PLANE" ;
	macros[ 0 ].Definition	= "0" ;
	macros[ 1 ].Name	= "RIPPLES" ;
	macros[ 1 ].Definition	= "0" ;

	if (r_show_winter->GetBool())
	{
		int count = macros.Count();

		macros.Resize(count + 1);
		macros[ count ].Name		= "WINTER";
		macros[ count ].Definition	= "1";
	}

	r3dRenderer->ReloadPixelShader("WATER_HP",  "Nature\\OceanV2high_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATER_NP",  "Nature\\OceanV2normal_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATER_LP",  "Nature\\OceanV2low_ps.hls", macros);

	macros[ 0 ].Definition	= "1" ;


	r3dRenderer->ReloadPixelShader("WATERF_HP",  "Nature\\OceanV2high_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATERF_NP",  "Nature\\OceanV2normal_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATERF_LP",  "Nature\\OceanV2low_ps.hls", macros);

	macros[ 0 ].Definition	= "0" ;
	macros[ 1 ].Definition	= "1" ;

	r3dRenderer->ReloadPixelShader("WATERR_HP",  "Nature\\OceanV2high_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATERR_NP",  "Nature\\OceanV2normal_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATERR_LP",  "Nature\\OceanV2low_ps.hls", macros);

	macros[ 0 ].Definition	= "1" ;

	r3dRenderer->ReloadPixelShader("WATERFR_HP",  "Nature\\OceanV2high_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATERFR_NP",  "Nature\\OceanV2normal_ps.hls", macros);
	r3dRenderer->ReloadPixelShader("WATERFR_LP",  "Nature\\OceanV2low_ps.hls", macros);

}

static void ReloadLightingShaders()

{
	ShaderMacros macros ;
	char shaderName[ 512 ] ;

	for( int ssr = 0 ; ssr < 2 ; ssr ++ )
	{
		for( int sss = 0 ; sss < 2 ; sss ++ )
		{
			for( int transp_shadows = 0 ; transp_shadows < 2 ; transp_shadows ++ )
			{
				for( int white_albedo = 0 ; white_albedo < 2 ; white_albedo ++ )
				{
					for( int ambient_only = 0 ; ambient_only < 2 ; ambient_only ++ )
					{
						for( int quality = 0; quality < 2; quality ++ )
						{
							SunHDRLightPShaderId pshadId;

							pshadId.ssr				= ssr;
							pshadId.sss				= sss;
							pshadId.transp_shadows	= transp_shadows;
							pshadId.white_albedo	= white_albedo;
							pshadId.ambient_only	= ambient_only;
							pshadId.quality			= quality;

							pshadId.FillMacros( macros );
							pshadId.ToString( shaderName );

							if (r_show_winter->GetBool())
							{
								int count = macros.Count();

								macros.Resize(count + 1);
								macros[ count ].Name		= "WINTER";
								macros[ count ].Definition	= "1";
							}

							r3dRenderer->ReloadPixelShader( shaderName, "DS_dir_HDR_ps.hls", macros );
						}
					}
				}
			}
		}
	}

	{
		char shaderName[ 512 ] ;

		ShaderMacros shmacros ;

		for( int ssr = 0 ; ssr < 2 ; ssr ++ )
		{
			for( int sss = 0 ; sss < 2 ; sss ++ )
			{
				for( int diffuse_only = 0 ; diffuse_only < 2 ; diffuse_only ++ )
				{
					for( int ssao = 0 ; ssao < 2 ; ssao ++ )
					{
						for( int proj_texture = 0 ; proj_texture < 2 ; proj_texture ++ )
						{
							for( int shadow_mode = 0 ; shadow_mode < 5 ; shadow_mode ++ )
							{
								for( int quality = 0; quality < 2; quality ++ )
								{
									PointLightPShaderId psid ;

									psid.sss			= sss ;
									psid.ssr			= ssr ;
									psid.diffuse_only	= diffuse_only ;
									psid.ssao			= ssao ;
									psid.proj_texture	= proj_texture ;
									psid.shadow_mode	= shadow_mode ;
									psid.quality		= quality;

									psid.ToString( shaderName ) ;
									psid.FillMacros( shmacros ) ;

									if (r_show_winter->GetBool())
									{
										int count = macros.Count();

										macros.Resize(count + 1);
										macros[ count ].Name		= "WINTER";
										macros[ count ].Definition	= "1";
									}


									r3dRenderer->ReloadPixelShader( shaderName, "DS_Point_HDR_ps.hls", shmacros ) ;
								}
							}
						}
					}
				}
			}
		}
	}

	{
		char shaderName[ 512 ] ;

		ShaderMacros shmacros ;

		for( int aux_enabled = 0; aux_enabled < 2; aux_enabled ++ )
		{
			for( int shadow_mode = 0; shadow_mode < 4; shadow_mode ++ )
			{
				for( int proj_tex = 0; proj_tex < 2; proj_tex ++ )
				{
					for( int quality = 0; quality < 2; quality ++ )
					{
						SpotLightPShaderId psid ;

						psid.aux_enabled = aux_enabled;
						psid.shadow_mode = shadow_mode;
						psid.proj_texture = proj_tex;
						psid.quality = quality;

						psid.ToString( shaderName );
						psid.FillMacros( shmacros );

						if (r_show_winter->GetBool())
						{
							int count = macros.Count();

							macros.Resize(count + 1);
							macros[ count ].Name		= "WINTER";
							macros[ count ].Definition	= "1";
						}

						r3dRenderer->ReloadPixelShader( shaderName, "DS_Spot_HDR_ps.hls", shmacros) ;
					}
				}
			}
		}
	}


};

static void ShowWinterCallback(int oldI, float oldF)
{
	if (oldI == r_show_winter->GetInt())
		return;

	struct EnableDisableShaderLoading
	{
		EnableDisableShaderLoading()
		{
			r3dRenderer->SetShaderLoadingEnabled( true );
		}

		~EnableDisableShaderLoading()
		{
			r3dRenderer->SetShaderLoadingEnabled( false );
		}
	} enableDisableShaderLoading; ( void )enableDisableShaderLoading;

	LoadGrassShaders(true);
	LoadFillBufferShaders(true);
	ReloadWaterShaders();
	r3dTerrain3::ReloadShaders();
	ReloadLightingShaders();
}


r3dTexture*	gCloudTexture;
r3dScreenBuffer* gCloudShadow = 0;

#include "FogEffects.hpp"
#include "PostEffects.hpp"

#define BIAS_RESCALE 4.f

int CloudsShadows_Enable = 1;
char CloudsShadows_TexName[ R3D_MAX_FILE_NAME ];
float CloudsShadows_BlendFactor = 1.0f;
TCloudsShadowParams CloudsShadows_Mask = { 1, 1, 1 };
TCloudsShadowParams CloudsShadows_Noise1 = { 1, 1, 1 };
TCloudsShadowParams CloudsShadows_Noise2 = { 1, 1, 1 };


ShadowSlice CompoundSlice( 0, 500.0f, 0.00005f * BIAS_RESCALE );
ShadowSlice ShadowSlices[NumShadowSlices] = {	ShadowSlice(0, 100.0f, 0.00005f * BIAS_RESCALE ),
												ShadowSlice(1, 300.0f, 0.00005f * BIAS_RESCALE ),
												ShadowSlice(2, 500.0f, 0.00005f * BIAS_RESCALE ) };
ShadowSlice TransparentShadowSlice = ShadowSlice( 0, 500.0f, 0.00005f * BIAS_RESCALE );

float ShadowSplitDistancesOpaqueHigh[NumShadowSlices+1];
float ShadowSplitDistancesOpaqueMed[NumShadowSlices+1];
float ShadowSplitDistancesOpaqueLow[NumShadowSlices+1];
float ShadowSplitDistancesOpaque_DynamicCubemap[NumShadowSlices+1];

float *ShadowSplitDistancesOpaque = &ShadowSplitDistancesOpaqueHigh[0];

float ShadowSplitDistancesTransparentHigh[NumTransparentShadowSlices+1] = { 0.125f, 250.f } ;
float *ShadowSplitDistancesTransparent = &ShadowSplitDistancesTransparentHigh[ 0 ] ;

float ShadowCameraNear = 0.1f;
float ShadowDrawingDistance = 500;
float ShadowSunOffset = 500.f;

float __WorldRenderBias = -1.0f;

// camera motion blur
int g_cameraMotionBlurEnabled = 0;
int g_ObjectMotionBlurEnabled = 0;
int g_LensDirtEnabled = 0;

int g_UseOBBox = 1;
int g_SortByMaterial = 1;

int g_CCBlackWhite = 0;
float g_fCCBlackWhitePwr = 0.0f;
float g_SceneVisualizerDepthMultiplier = 0;
D3DXVECTOR4 g_SceneVisualizerColorMask = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

int DS_TUBELIGHT_ID = - 1 ;
int DS_TUBELIGHT_SM_ID = -1 ;
int DS_TUBELIGHT_SSM_ID = -1 ;

int DS_PLANELIGHT_ID = - 1 ;
int DS_PLANELIGHT_SM_ID = -1 ;
int DS_PLANELIGHT_SSM_ID = -1 ;

int VS_SPOT_ID				= -1;

int PS_CLEAR_FLOAT_ID		= -1;
int VS_CLEAR_FLOAT_ID		= -1;

int PS_MINIMUM_ID				= -1;
int PS_PCF_MINIMUM_ID			= -1;
int PS_MINIMUM_MASK_ID			= -1;
int PS_ADAPTIVEDEPTHBLUR7_ID	= -1;

int PS_POINTLIGHT_V2_ID			= -1;
int VS_POINTLIGHT_V2_ID			= -1;

int VS_POSTFX_GEOM_TRANSFORM	= -1;

int PS_VOLUME_POINTLIGHT_ID		= -1;

int PS_FWD_COLOR_ID = -1 ;
int VS_FWD_COLOR_ID = -1 ;

extern bool g_bEditMode;

r3dPoint3D LastLFShadowSliceUpdatePos ;
extern int gSLI_Crossfire_NumGPUs;

#pragma warning( disable: 4244 )

void CopySurface( r3dScreenBuffer* source, r3dScreenBuffer* target );
void PerformRESZResolve(r3dScreenBuffer *src);
void SetupSpotLightMatrices( r3dLight* l, D3DXMATRIX* oLightView, r3dCamera* oLightCam );

void FinalizeSSAORender() ;
void CompositeSSAO( r3dScreenBuffer* currSSAO ) ;
void DiluteSSAOMask( r3dScreenBuffer *sourceTex ) ;

void SetSunParams();
void SetAmbientParams();

void BlendMax( r3dTexture* src, r3dScreenBuffer* target );

const char* DigitToCString( int digit );

//------------------------------------------------------------------------

int IsRecticularWarpActive()
{
	return r_active_shadow_slices->GetInt() == 1 && r_lfsm_recticular_warp->GetInt();
}


//////////////////////////////////////////////////////////////////////////

void CalcShaderTextIdx(char *buf, int idx)
{
	buf[0] = '0';
	buf[1] = '0';
	r3d_assert( idx < 100 );
	if (idx / 10 == 0)
	{
		buf[0] += idx % 10;
		buf[1] = 0;
	}
	else
	{
		buf[0] += idx / 10;
		buf[1] += idx % 10;
	}
}

//////////////////////////////////////////////////////////////////////////

void HalfResParticleVarChangeCallback(int oldI, float oldF)
{
	if (oldI == r_half_res_particles->GetInt())
		return;

	float rtWidth, rtHeight ;
	GetDesiredRTDimmensions( &rtWidth, &rtHeight ) ;

	if (r_half_res_particles->GetBool())
	{
		SAFE_DELETE(gBuffer_Particles);
		SAFE_DELETE(DistortionBuffer);
		DistortionBuffer = r3dScreenBuffer::CreateClass("DistortionBuffer", rtWidth / 2, rtHeight / 2, D3DFMT_A8R8G8B8);
		gBuffer_Particles = r3dScreenBuffer::CreateClass("gBuffer_Particles", rtWidth / 2, rtHeight / 2, D3DFMT_A16B16G16R16F);//D3DFMT_A8R8G8B8);
	}
	else
	{
		SAFE_DELETE(gBuffer_Particles);
		SAFE_DELETE(DistortionBuffer);
		DistortionBuffer = r3dScreenBuffer::CreateClass("DistortionBuffer", rtWidth, rtHeight, D3DFMT_A8R8G8B8);
	}
}




//////////////////////////////////////////////////////////////////////////

void HardwareShadowMapsChangeCallback(int oldI, float oldF)
{
	int currI = r_hardware_shadow_method->GetInt();
	if (oldI == currI)
		return;

	int supportedMethod = HW_SHADOW_METHOD_R32F;

	switch (currI)
	{
	case HW_SHADOW_METHOD_R32F:
		break;
	case HW_SHADOW_METHOD_HW_PCF:
		if (r3dRenderer->SupportsHWShadowMapping)
		{
			supportedMethod = currI;
		}
		else
		{
			r3dOutToLog("Hardware does not support hardware shadow mapping\n");
		}
		break;
	case HW_SHADOW_METHOD_INTZ:
		if (r3dRenderer->SupportsINTZTextures)
		{
			supportedMethod = currI;
		}
		else
		{
			r3dOutToLog("Hardware does not support INTZ textures. Switching to hardware shadow is not possible\n");
		}
		break;
	default:
		r3d_assert(!"Unknown shadow method");
	}

	if (supportedMethod != currI)
	{
		r_hardware_shadow_method->SetChangeCallback(0);
		r_hardware_shadow_method->SetInt(supportedMethod);
		r_hardware_shadow_method->SetChangeCallback(&HardwareShadowMapsChangeCallback);
	}

	if (oldI != supportedMethod)
	{
		CurRenderPipeline->DestroyShadowResources() ;
		CurRenderPipeline->CreateShadowResources() ;

		//	Force low frequency SM update
		LastLFShadowSliceUpdatePos = LastLFShadowSliceUpdatePos + r3dVector(2, 0, 0) * r_lfsm_cache_dist->GetFloat();
	}

	void UpdateFXAAShadowBlurSettings();
	UpdateFXAAShadowBlurSettings();
}

//////////////////////////////////////////////////////////////////////////

void UpdateFXAAShadowBlurSettings()
{
	//	Force fxaa shadows for ultra shadow quality regardless of hardware shadow method
	int fxaaShadow = r_shadows_quality->GetInt() == 4 ? 1 : 0;
	r_fxaa_shadow_blur->SetInt( fxaaShadow ) ;

	if( r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF )
	{
		if( r_active_shadow_slices->GetInt() > 1 )
		{
			r_fxaa_shadow_blur->SetInt( 1 ) ;
		}
	}
}

void ShadowBlurChangeCallback(int oldI, float oldF)
{
	if (oldI == r_shadow_blur->GetInt())
		return;

	if( r_allow_hw_shadows->GetInt() )
	{
		if (r_shadow_blur->GetInt())
		{
#if 0
			if( r3dRenderer->SupportsINTZTextures )
				r_hardware_shadow_method->SetInt(HW_SHADOW_METHOD_INTZ);
			else
#endif
				r_hardware_shadow_method->SetInt(HW_SHADOW_METHOD_R32F);
		}
		else
		{
			if( r3dRenderer->SupportsHWShadowMapping )
			{
				r_hardware_shadow_method->SetInt(HW_SHADOW_METHOD_HW_PCF);
			}
			else
				r_hardware_shadow_method->SetInt(HW_SHADOW_METHOD_R32F);
		}
	}
	else
	{
		r_hardware_shadow_method->SetInt(HW_SHADOW_METHOD_R32F);
	}

	UpdateFXAAShadowBlurSettings();
}

//////////////////////////////////////////////////////////////////////////

void TurnOffSSS(int reg)
{
	float vConst[4] = { 0.0f, 0.0f, 0.f, 1.0f };
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( reg, vConst, 1 );
}

void SetSSSParams(int reg, bool dirlight, const r3dSSScatterParams& SSSParams )
{
	bool on = false ;

	if(r_sss->GetBool())
	{
		if( SSSParams.On )
		{
			on = true ;

			float trans_r 	= SSSParams.translucency.R / 255.f ;
			float trans_g 	= SSSParams.translucency.G / 255.f ;
			float trans_b 	= SSSParams.translucency.B / 255.f ;
			float scale		= SSSParams.scale ;
			float ambient	= SSSParams.ambient ;

			// dir light does stuff 'in linear space'
			if( dirlight )
			{
				trans_r = pow( trans_r, 2.2f ) ;
				trans_g = pow( trans_g, 2.2f ) ;
				trans_b = pow( trans_b, 2.2f ) ;

				scale = pow( scale, 2.2f ) ;
				ambient = pow( ambient, 2.2f );
			}
			else
			{
				scale *= 2 ; // ...
			}

			float vConst[8] = {
				SSSParams.distortion, SSSParams.power, scale, ambient,
				trans_r, trans_g, trans_b, SSSParams.translucency.A / 255.f
			};
			r3dRenderer->pd3ddev->SetPixelShaderConstantF( reg, vConst, 2 );
		}
	}

	if( !on )
	{
		TurnOffSSS( reg ) ;
	}
}

void CalculateMotionMatrix( D3DXMATRIX& output, D3DXMATRIX& lastViewProj )
{
	// Maps [-1,+1,0]..[+1,-1,+1] onto [0,0,0]..[1,1,1]
	const D3DXMATRIX projectionToScreen =
		D3DXMATRIX(	0.5f,  0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f, 0.0f, 1.0f );

	// Inverse of above
	const D3DXMATRIX screenToProjection =
		D3DXMATRIX( 2.0f,  0.0f, 0.0f, 0.0f,
		0.0f, -2.0f, 0.0f, 0.0f,
		0.0f,  0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f );

	D3DXMATRIX ViewProj;
	D3DXMATRIX View = r3dRenderer->ViewMatrix;
	View._41 = 0; View._42 = 0; View._43 = 0; // disable motion blur for moving, we have blur only for camera rotation
	D3DXMatrixMultiply( &ViewProj, &View, &r3dRenderer->ProjMatrix);

	D3DXMATRIX projToWorld_Curr;
	D3DXMatrixInverse( &projToWorld_Curr, NULL, &ViewProj );

	D3DXMATRIX screenToWorld_Curr;
	D3DXMatrixMultiply( &screenToWorld_Curr, &screenToProjection, &projToWorld_Curr);

	D3DXMATRIX worldToScreen_Prev;
	D3DXMatrixMultiply( &worldToScreen_Prev, &lastViewProj, &projectionToScreen);

	D3DXMATRIX screenCurrentToPrevious;
	D3DXMatrixMultiply( &screenCurrentToPrevious, &screenToWorld_Curr, &worldToScreen_Prev);

	D3DXMATRIX Identity; D3DXMatrixIdentity(&Identity);
	D3DXMATRIX screenToVelocity =
		D3DXMATRIX(	screenCurrentToPrevious(0,0) - Identity(0,0),
		screenCurrentToPrevious(0,1) - Identity(0,1),
		screenCurrentToPrevious(0,2) - Identity(0,2),
		screenCurrentToPrevious(0,3) - Identity(0,3),
		screenCurrentToPrevious(1,0) - Identity(1,0),
		screenCurrentToPrevious(1,1) - Identity(1,1),
		screenCurrentToPrevious(1,2) - Identity(1,2),
		screenCurrentToPrevious(1,3) - Identity(1,3),
		screenCurrentToPrevious(2,0) - Identity(2,0),
		screenCurrentToPrevious(2,1) - Identity(2,1),
		screenCurrentToPrevious(2,2) - Identity(2,2),
		screenCurrentToPrevious(2,3) - Identity(2,3),
		screenCurrentToPrevious(3,0) - Identity(3,0),
		screenCurrentToPrevious(3,1) - Identity(3,1),
		screenCurrentToPrevious(3,2) - Identity(3,2),
		screenCurrentToPrevious(3,3) - Identity(3,3));

	float w = screenToVelocity(3,3);
	D3DXMATRIX wMatrix =
		D3DXMATRIX( w,    0.0f, 0.0f, 0.0f,
		0.0f, w,    0.0f, 0.0f,
		0.0f, 0.0f, w,    0.0f,
		0.0f, 0.0f, 0.0f, w );

	D3DXMATRIX screenToVelocityRefined =
		D3DXMATRIX(	screenToVelocity(0,0) - wMatrix(0,0),
		screenToVelocity(0,1) - wMatrix(0,1),
		screenToVelocity(0,2) - wMatrix(0,2),
		screenToVelocity(0,3) - wMatrix(0,3),
		screenToVelocity(1,0) - wMatrix(1,0),
		screenToVelocity(1,1) - wMatrix(1,1),
		screenToVelocity(1,2) - wMatrix(1,2),
		screenToVelocity(1,3) - wMatrix(1,3),
		screenToVelocity(2,0) - wMatrix(2,0),
		screenToVelocity(2,1) - wMatrix(2,1),
		screenToVelocity(2,2) - wMatrix(2,2),
		screenToVelocity(2,3) - wMatrix(2,3),
		screenToVelocity(3,0) - wMatrix(3,0),
		screenToVelocity(3,1) - wMatrix(3,1),
		screenToVelocity(3,2) - wMatrix(3,2),
		screenToVelocity(3,3) - wMatrix(3,3) );

	lastViewProj = ViewProj;
	D3DXMatrixTranspose(&output, &screenToVelocityRefined);
}

// Computes corner points of a frustum
void CalculateFrustumCorners(D3DXVECTOR3 *pPoints,
							 const D3DXVECTOR3 &vSource,
							 const D3DXVECTOR3 &vTarget,
							 const D3DXVECTOR3 &vUp,
							 float fNear, float fFar,
							 float fFOV, float fAspect)
{
	R3DPROFILE_FUNCTION("CalculateFrustumCorners");
	D3DXVECTOR3 vZ=vTarget-vSource;
	D3DXVec3Normalize(&vZ,&vZ);

	D3DXVECTOR3 vX;
	D3DXVec3Cross(&vX,&vUp,&vZ);
	D3DXVec3Normalize(&vX,&vX);

	D3DXVECTOR3 vY;
	D3DXVec3Cross(&vY,&vZ,&vX);


	float fNearPlaneHeight = tanf(R3D_DEG2RAD(fFOV) * 0.5f) * fNear;
	float fNearPlaneWidth = fNearPlaneHeight * fAspect;

	float fFarPlaneHeight = tanf(R3D_DEG2RAD(fFOV) * 0.5f) * fFar;
	float fFarPlaneWidth = fFarPlaneHeight * fAspect;

	D3DXVECTOR3 vNearPlaneCenter = vSource + vZ * fNear;
	D3DXVECTOR3 vFarPlaneCenter = vSource + vZ * fFar;

	pPoints[0]=D3DXVECTOR3(vNearPlaneCenter - vX*fNearPlaneWidth - vY*fNearPlaneHeight);
	pPoints[1]=D3DXVECTOR3(vNearPlaneCenter - vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[2]=D3DXVECTOR3(vNearPlaneCenter + vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[3]=D3DXVECTOR3(vNearPlaneCenter + vX*fNearPlaneWidth - vY*fNearPlaneHeight);

	pPoints[4]=D3DXVECTOR3(vFarPlaneCenter - vX*fFarPlaneWidth - vY*fFarPlaneHeight);
	pPoints[5]=D3DXVECTOR3(vFarPlaneCenter - vX*fFarPlaneWidth + vY*fFarPlaneHeight);
	pPoints[6]=D3DXVECTOR3(vFarPlaneCenter + vX*fFarPlaneWidth + vY*fFarPlaneHeight);
	pPoints[7]=D3DXVECTOR3(vFarPlaneCenter + vX*fFarPlaneWidth - vY*fFarPlaneHeight);
}

static float GetCamPadding()
{
	float camX = r3dRenderer->NearClip / r3dRenderer->ProjMatrix.m[0][0];
	float camY = r3dRenderer->NearClip / r3dRenderer->ProjMatrix.m[1][1];

	return sqrtf( r3dRenderer->NearClip * r3dRenderer->NearClip + camY*camY + camX*camX ) * 1.01f;
}

// Builds a standard perspective view and projection matrix
R3D_FORCEINLINE void CalculateViewProj(D3DXMATRIX *oView, D3DXMATRIX* oProj,
					   D3DXVECTOR3 vSource, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUpVector,
					   float fFOV, float fNear, float fFar, float fAspect)
{
	D3DXMatrixLookAtLH(oView, &vSource, &vTarget, &vUpVector);

	float n = fNear;
	float f = fFar;

	r3dRenderer->BuildMatrixPerspectiveFovLH(oProj, R3D_DEG2RAD(fFOV), fAspect, n, f);
}

void CalculateLightForFrustum(ShadowSlice* ioSlice, D3DXVECTOR3 (&oCorners)[ 8 ], D3DXVECTOR3* ioLightSource, D3DXVECTOR3* ioLightTarget, const r3dPoint3D& sunVector )
{
	R3DPROFILE_FUNCTION("CalculateLightForFrustum");
	float fLightFar = ShadowSunOffset * 2.f;
	float fLightNear = 1;

	// calculate standard view and projection matrices for light
	CalculateViewProj(	&ioSlice->lightView, &ioSlice->lightProj,
						*ioLightSource,*ioLightTarget, D3DXVECTOR3(0,1,0),
						90, fLightNear, fLightFar,1 );

	// Next we will find the min and max values of the current
	// frustum split in lights post-projection space
	// (where coordinate range is from -1.0 to 1.0)
	float fMaxX=-FLT_MAX;
	float fMaxY=-FLT_MAX;
	float fMinX=FLT_MAX;
	float fMinY=FLT_MAX;
	float fMaxZ=0;
	float fMinZ=FLT_MAX;

	D3DXMATRIX mLightViewProj=ioSlice->lightView;// * slice.lightProj;

	// find the center of the frustum and the radius.
	// this isn't an optimal bounding sphere necessarily..
	// even more wasted shadow map!

	D3DXVECTOR2 sphereCenter(0,0);

	float	miX = +FLT_MAX,
			maX = -FLT_MAX,
			miY = +FLT_MAX,
			maY = -FLT_MAX;

	for(int i=0; i<8; i++)
	{
		D3DXVECTOR4 temp;
		D3DXVec3Transform( &temp, &oCorners[i], &mLightViewProj );

		// paranoid
		temp *= 1.f / temp.w ;

		miX = R3D_MIN( temp.x, miX );
		miY = R3D_MIN( temp.y, miY );

		maX = R3D_MAX( temp.x, maX );
		maY = R3D_MAX( temp.y, maY );
	}

	sphereCenter.x  = ( miX + maX ) * 0.5f;
	sphereCenter.y  = ( miY + maY ) * 0.5f;

	const float SNAP = 2.f;

	// in order to be able to snap without artifacts, add SNAP
	float sphereRadius = R3D_MAX( maX - miX, maY - miY ) * 0.5f + SNAP;

	// check if radius changed significantly, otherwise it's precision problem and we don't want to change radius -> jittering problem
	if( fabs( ioSlice->sphereRadius - sphereRadius ) > SNAP )
		ioSlice->sphereRadius = sphereRadius;

	float texelWidth = (ioSlice->sphereRadius*2) / ioSlice->shadowMapSize;
	// this will stabilize projection when rotation camera
	sphereCenter.x -= fmod(sphereCenter.x,texelWidth);
	sphereCenter.y -= fmod(sphereCenter.y,texelWidth);

	//Use the sphere to choose coords
	fMaxX = sphereCenter.x + ioSlice->sphereRadius ;
	fMaxY = sphereCenter.y + ioSlice->sphereRadius ;
	fMinX = sphereCenter.x - ioSlice->sphereRadius ;
	fMinY = sphereCenter.y - ioSlice->sphereRadius ;

	// this will stabilize projection when moving camera
	{
		//get texCam orientation
		r3dPoint3D up = r3dPoint3D(0,1,0);
		r3dPoint3D dir = sunVector;
		// Check it's not coincident with dir
		if (R3D_ABS(up.Dot(dir))>= 1.0f)
		{
			// Use camera up
			up = r3dPoint3D(0,0,1);
		}
		// cross twice to rederive, only direction is unaltered
		r3dPoint3D left = dir.Cross(up);
		left.Normalize();
		up = dir.Cross(left);
		up.Normalize();
		// Derive quaternion from axes
		D3DXQUATERNION q, qInv;
		D3DXMATRIX kRot, kRot2;

		kRot(0,0) = left.x;
		kRot(1,0) = left.y;
		kRot(2,0) = left.z;

		kRot(0,1) = up.x;
		kRot(1,1) = up.y;
		kRot(2,1) = up.z;

		kRot(0,2) = dir.x;
		kRot(1,2) = dir.y;
		kRot(2,2) = dir.z;

		D3DXMatrixTranspose(&kRot2, &kRot);

		D3DXQuaternionRotationMatrix(&q, &kRot2);
		D3DXQuaternionInverse(&qInv, &q);
		//convert world space camera position into light space
		r3dPoint3D lightSpacePos;
		r3dPoint3D camPos = r3dPoint3D(ioLightTarget->x, ioLightTarget->y, ioLightTarget->z);
		// multiply quaternion by vector
		{
			// nVidia SDK implementation
			r3dPoint3D uv, uuv;
			r3dPoint3D qvec(qInv.x, qInv.y, qInv.z);
			uv = qvec.Cross(camPos);
			uuv = qvec.Cross(uv);
			uv *= (2.0f * qInv.w);
			uuv *= 2.0f;

			lightSpacePos = camPos + uv + uuv;
		}

		//snap to nearest texel
		lightSpacePos.x -= fmod(lightSpacePos.x, texelWidth);
		lightSpacePos.y -= fmod(lightSpacePos.y, texelWidth);

		//convert back to world space
		{
			// nVidia SDK implementation
			r3dPoint3D uv, uuv;
			r3dPoint3D qvec(q.x, q.y, q.z);
			uv = qvec.Cross(lightSpacePos);
			uuv = qvec.Cross(uv);
			uv *= (2.0f * q.w);
			uuv *= 2.0f;

			camPos = lightSpacePos + uv + uuv;
		}

		*ioLightTarget = D3DXVECTOR3(camPos.x, camPos.y, camPos.z);
		D3DXVECTOR3 sunV = D3DXVECTOR3(SunVector.x, SunVector.y, SunVector.z);
		*ioLightSource = *ioLightTarget - sunV*ShadowSunOffset;
	}

	// re-calculate lights matrices with the new far plane
	CalculateViewProj( &ioSlice->lightView, &ioSlice->lightProj,
						*ioLightSource, *ioLightTarget, D3DXVECTOR3(0,1,0),
						90, fLightNear, fLightFar, 1 );

	ioSlice->pixelDiameter = texelWidth * 1.41421356237f / ( fLightFar - fLightNear );

	//Calculate projection around the sphere.
	r3dRenderer->BuildMatrixOrthoOffCenterLH(&ioSlice->lightProj,fMinX,fMaxX,fMinY,fMaxY,fLightNear,fLightFar);
}

void CalculateLightForFrustum_Optimized(ShadowSlice* ioSlice, float* shadowSplitDistances, ShadowMapOptimizationData* optimizationData,
										D3DXVECTOR3 (&oCorners)[ 8 ], D3DXVECTOR3* ioLightSource, D3DXVECTOR3* ioLightTarget, const r3dPoint3D& sunVector, float border )
{
	R3DPROFILE_FUNCTION("CalculateLightForFrustum_Optimized");
	float fLightFar = ShadowSunOffset * 2.f;
	float fLightNear = 1;

	// calculate standard view and projection matrices for light
	CalculateViewProj(	&ioSlice->lightView, &ioSlice->lightProj,
						*ioLightSource,*ioLightTarget, D3DXVECTOR3(0,1,0),
						90, fLightNear, fLightFar, 1 );

	// Next we will find the min and max values of the current
	// frustum split in lights post-projection space
	// (where coordinate range is from -1.0 to 1.0)
	float fMaxX=-FLT_MAX;
	float fMaxY=-FLT_MAX;
	float fMinX=FLT_MAX;
	float fMinY=FLT_MAX;
	float fMaxZ=0;
	float fMinZ=FLT_MAX;

	D3DXMATRIX mLightViewProj=ioSlice->lightView;// * slice.lightProj;

	float	miX = +FLT_MAX,
			maX = -FLT_MAX,
			miY = +FLT_MAX,
			maY = -FLT_MAX;

	for(int i=0; i<8; i++)
	{
		D3DXVECTOR4 temp;
		D3DXVec3Transform( &temp, &oCorners[i], &mLightViewProj );

		// paranoid
		temp *= 1.f / temp.w ;

		miX = R3D_MIN( temp.x, miX );
		miY = R3D_MIN( temp.y, miY );

		maX = R3D_MAX( temp.x, maX );
		maY = R3D_MAX( temp.y, maY );
	}

	if( optimizationData )
	{
		float newMiX = optimizationData[ ioSlice->index ].fMinX ;
		float newMaX = optimizationData[ ioSlice->index ].fMaxX ;
		float newMiY = optimizationData[ ioSlice->index ].fMinY ;
		float newMaY = optimizationData[ ioSlice->index ].fMaxY ;

		if( miX < newMiX && newMiX < maX ) { miX = newMiX ; }
		if( maX > newMaX && newMaX > miX ) { maX = newMaX ;	}
		if( miY < newMiY && newMiY < maY ) { miY = newMiY ; }
		if( maY > newMaY && newMaY > miY ) { maY = newMaY ;	}
	}

	{
		float sx = maX - miX ;
		float sy = maY - miY ;

		miX -= border * sx ;
		miY -= border * sy ;

		maX += border * sx ;
		maY += border * sy ;
	}


	fMinX = miX ;
	fMaxX = maX ;
	fMinY = miY ;
	fMaxY = maY ;

	float d ;

	d = shadowSplitDistances[ ioSlice->index + 1 ] - shadowSplitDistances[ ioSlice->index ] ;

	const float SNAP = d * 0.09f ;

	// in order to be able to snap without artifacts, add SNAP
	float sizeX = maX - miX + SNAP ;
	float sizeY = maY - miY + SNAP ;

	// check if radius changed significantly, otherwise it's precision problem and we don't want to change radius -> jittering problem
	if( fabs( ioSlice->worldSizeX - sizeX ) > SNAP )
		ioSlice->worldSizeX = sizeX ;

	if( fabs( ioSlice->worldSizeY - sizeY ) > SNAP )
		ioSlice->worldSizeY = sizeY ;

	float texelSizeX = ioSlice->worldSizeX / ioSlice->shadowMapSize;
	float texelSizeY = ioSlice->worldSizeY / ioSlice->shadowMapSize;

	float dx = fmod( fMinX, texelSizeX );
	float dy = fmod( fMinY, texelSizeY );

	// this will stabilize projection when rotation camera
	fMinX -= dx;
	fMaxX = fMinX + ioSlice->worldSizeX;

	fMinY -= dy;
	fMaxY = fMinY + ioSlice->worldSizeY;

	// this will stabilize projection when moving camera
	{
		//get texCam orientation
		r3dPoint3D up = r3dPoint3D(0,1,0);
		r3dPoint3D dir = sunVector;
		// Check it's not coincident with dir
		if (R3D_ABS(up.Dot(dir))>= 1.0f)
		{
			// Use camera up
			up = r3dPoint3D(0,0,1);
		}
		// cross twice to rederive, only direction is unaltered
		r3dPoint3D left = dir.Cross(up);
		left.Normalize();
		up = dir.Cross(left);
		up.Normalize();
		// Derive quaternion from axes
		D3DXQUATERNION q, qInv;
		D3DXMATRIX kRot, kRot2;

		kRot(0,0) = left.x;
		kRot(1,0) = left.y;
		kRot(2,0) = left.z;

		kRot(0,1) = up.x;
		kRot(1,1) = up.y;
		kRot(2,1) = up.z;

		kRot(0,2) = dir.x;
		kRot(1,2) = dir.y;
		kRot(2,2) = dir.z;

		D3DXMatrixTranspose(&kRot2, &kRot);

		D3DXQuaternionRotationMatrix(&q, &kRot2);
		D3DXQuaternionInverse(&qInv, &q);
		//convert world space camera position into light space
		r3dPoint3D lightSpacePos;
		r3dPoint3D camPos = r3dPoint3D(ioLightTarget->x, ioLightTarget->y, ioLightTarget->z);
		// multiply quaternion by vector
		{
			// nVidia SDK implementation
			r3dPoint3D uv, uuv;
			r3dPoint3D qvec(qInv.x, qInv.y, qInv.z);
			uv = qvec.Cross(camPos);
			uuv = qvec.Cross(uv);
			uv *= (2.0f * qInv.w);
			uuv *= 2.0f;

			lightSpacePos = camPos + uv + uuv;
		}

		//snap to nearest texel
		lightSpacePos.x -= fmod(lightSpacePos.x, texelSizeX);
		lightSpacePos.y -= fmod(lightSpacePos.y, texelSizeY);

		//convert back to world space
		{
			// nVidia SDK implementation
			r3dPoint3D uv, uuv;
			r3dPoint3D qvec(q.x, q.y, q.z);
			uv = qvec.Cross(lightSpacePos);
			uuv = qvec.Cross(uv);
			uv *= (2.0f * q.w);
			uuv *= 2.0f;

			camPos = lightSpacePos + uv + uuv;
		}

		*ioLightTarget = D3DXVECTOR3(camPos.x, camPos.y, camPos.z);
		D3DXVECTOR3 sunV = D3DXVECTOR3(SunVector.x, SunVector.y, SunVector.z);
		*ioLightSource = *ioLightTarget - sunV*ShadowSunOffset;
	}

	// re-calculate lights matrices with the new far plane
	CalculateViewProj(	&ioSlice->lightView, &ioSlice->lightProj,
						*ioLightSource,*ioLightTarget, D3DXVECTOR3(0,1,0),
						90, fLightNear, fLightFar, 1 );

	ioSlice->pixelDiameter = sqrtf( texelSizeX * texelSizeX + texelSizeY * texelSizeY ) / ( fLightFar - fLightNear );

	//Calculate projection around the sphere.
	r3dRenderer->BuildMatrixOrthoOffCenterLH(&ioSlice->lightProj,fMinX,fMaxX,fMinY,fMaxY,fLightNear,fLightFar);
}

void SetupSMTransform( ShadowSlice& slice )
{
	D3DXVECTOR4 From;
	From =  D3DXVECTOR4(slice.camPos.X,slice.camPos.Y,slice.camPos.Z, 0);

	// This will scale and offset -1 to 1 range of x, y
	// coords output by projection matrix to 0-1 texture coord range.
	float fTexOffset=0.5f+(0.5f/SunShadowMap->Width);
	D3DXMATRIX mTexScale(   0.5f,               0.0f,      0.0f,   0.0f,
							0.0f,              -0.5f,      0.0f,   0.0f,
							0.0f,               0.0f,      1.0f,   0.0f,
							fTexOffset,    fTexOffset,     0.0f,   1.0f );

	D3DXMATRIX mat = slice.lightView * slice.lightProj * mTexScale;
	D3DXMatrixTranspose(&mat, &mat);

	D3DXVECTOR4 ShadowParams = D3DXVECTOR4(slice.depthBias, 1.0f/ SunShadowMap->Width, 0.0f, ShadowSplitDistancesOpaque[slice.index+1]*0.925f);

	// float4	vLightPos  		: register(c4);		// (c40); +
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  4, (float *)&From,  1 );
	// float4x4	mShadowProj 	: register(c5);		// (c41); +
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  5, (float *)&mat,  4 );
	// float4	ShadowParams	: register(c9);		// (c45); +
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  9, (float *)&ShadowParams,  1 );
}

static void ClearFloatSurface( const D3DXVECTOR4& value )
{
	D3DPERF_BeginEvent( 0, L"ClearFloatSurface" );
	R3DPROFILE_FUNCTION("ClearFloatSurface");
	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

	r3dRenderer->SetVertexShader( VS_CLEAR_FLOAT_ID );
	r3dRenderer->SetPixelShader( PS_CLEAR_FLOAT_ID );

	r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, (float*) &value, 1 );

	r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::black );
	D3DPERF_EndEvent();
}

//	Return true if SceneExposure0 RT is source for current avg luminance calculation
bool IsSceneExposure0IsSource()
{
	static bool effectState[32] = {0};

	//	Init
	if (effectState[0] == 0)
	{
		for (int i = 0; i < gSLI_Crossfire_NumGPUs; ++i)
			effectState[i] = 1;
	}

	const int numSteps = gSLI_Crossfire_NumGPUs * 2;
	extern int gRenderFrameCounter;
	int stepIdx = gRenderFrameCounter % numSteps;
	bool isSE1 = effectState[stepIdx];
	return !isSE1;
}

void ClearRenderTargetsForMultiGPU()
{
	//	Clear all frame-independent
	if (gSLI_Crossfire_NumGPUs < 2)
		return;

	R3DPROFILE_FUNCTION("ClearRenderTargetsForMultiGPU");
	D3DPERF_BeginEvent(0, L"ClearRenderTargetsForMultiGPU");

	gBuffer_Color->Activate(0);
	gBuffer_Normal->Activate(1);
	gBuffer_Depth->Activate(2);
	Scaleform_RenderToTextureRT->Activate(3);

	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);

	gBuffer_Color->Deactivate(0);
	gBuffer_Normal->Deactivate(0);
	gBuffer_Depth->Deactivate(0);
	Scaleform_RenderToTextureRT->Deactivate(0);

	gBuffer_HalvedR32F_0->Activate(0);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	gBuffer_HalvedR32F_0->Deactivate(0);

	if (gBuffer_PrimaryDepth)
	{
		gBuffer_PrimaryDepth->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		gBuffer_PrimaryDepth->Deactivate(0);
	}

	if (gBuffer_SecondaryDepth)
	{
		gBuffer_SecondaryDepth->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		gBuffer_SecondaryDepth->Deactivate(0);
	}

	if (gBuffer_HalvedR32F_1)
	{
		gBuffer_HalvedR32F_1->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		gBuffer_HalvedR32F_1->Deactivate(0);
	}

	gBuffer_Aux->Activate(0);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	gBuffer_Aux->Deactivate(0);

	gScreenSmall->Activate();
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	gScreenSmall->Deactivate(0);

	gScreenSmall2->Activate();
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	gScreenSmall2->Deactivate(0);

	ScreenBuffer->Activate(0);
	TempScreenBuffer->Activate(1);
	BlurredScreenBuffer->Activate(2);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);

	ScreenBuffer->Deactivate(0);
	TempScreenBuffer->Deactivate(0);
	BlurredScreenBuffer->Deactivate(0);

	BlurBuffer->Activate(0);
	TempBuffer->Activate(1);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	BlurBuffer->Deactivate(0);
	TempBuffer->Deactivate(0);

	One8Buffer0->Activate(0);
	One8Buffer1->Activate(1);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	One8Buffer0->Deactivate(0);
	One8Buffer1->Deactivate(0);

	One16Buffer0->Activate(0);
	One16Buffer1->Activate(1);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	One16Buffer0->Deactivate(0);
	One16Buffer1->Deactivate(0);

	One32Buffer0->Activate(0);
	One32Buffer1->Activate(1);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	One32Buffer0->Deactivate(0);
	One32Buffer1->Deactivate(0);

	One64Buffer0->Activate(0);
	One64Buffer1->Activate(1);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	One64Buffer1->Deactivate(0);
	One64Buffer0->Deactivate(0);

	TransparentShadowMap->Activate(0);
	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	TransparentShadowMap->Deactivate(0);

 	AvgSceneLuminanceBuffer->Activate(0);
 	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
 	AvgSceneLuminanceBuffer->Deactivate(0);

	r3dScreenBuffer *toClear = IsSceneExposure0IsSource() ? SceneExposure1 : SceneExposure0;
   	toClear->Activate(0);
  	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
  	toClear->Deactivate(0);

	if (r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_R32F)
	{
		LFUpdateShadowMap->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		LFUpdateShadowMap->Deactivate(0);

		SunShadowMap->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		SunShadowMap->Deactivate(0);

		SharedShadowMap->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		SharedShadowMap->Deactivate(0);
	}
	else
	{
		LFUpdateShadowMap->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		LFUpdateShadowMap->Deactivate(0);

		SunShadowMap->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		SunShadowMap->Deactivate(0);

		SharedShadowMap->Activate(0);
		r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		SharedShadowMap->Deactivate(0);
	}

	D3DPERF_EndEvent();
	R3DPROFILE_END("ClearRenderTargetsForMultiGPU");
}

namespace
{
	struct SetRestoreClampAddressMode
	{
		explicit SetRestoreClampAddressMode( DWORD samp )
		{
			Sampler = samp;

			D3D_V( r3dRenderer->pd3ddev->GetSamplerState( Sampler, D3DSAMP_ADDRESSU, &PrevAddressU ) );
			D3D_V( r3dRenderer->pd3ddev->GetSamplerState( Sampler, D3DSAMP_ADDRESSV, &PrevAddressV ) );

			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );
		}

		~SetRestoreClampAddressMode()
		{
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSU, PrevAddressU ) );
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( Sampler, D3DSAMP_ADDRESSV, PrevAddressV ) );
		}

		DWORD PrevAddressU;
		DWORD PrevAddressV;

		DWORD Sampler;

	};
}

void CopySurface( r3dScreenBuffer* source, r3dScreenBuffer* target )
{
	if (!source || !target)
		return;

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

	SetRestoreClampAddressMode setRestoreClampAddressMode( 0 ); (void)setRestoreClampAddressMode;
	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	target->Activate();

	float vSubPix[4] = {	+0.5f / target->Width,
							+0.5f / target->Height, 0.f, 0.f };

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vSubPix, 1 ) );

	r3dRenderer->SetVertexShader( "VS_POSTFX" );
	r3dRenderer->SetPixelShader( "PS_COPY" );

	r3dRenderer->SetTex( source->Tex );

	r3dSetFiltering( R3D_POINT, 0 );

	r3dDrawFullScreenQuad(false);

	target->Deactivate();

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
}

static void AdaptiveBlurDepth( r3dScreenBuffer* source, r3dScreenBuffer* temp, r3dScreenBuffer* target )
{
	D3DPERF_BeginEvent( 0, L"BlurSurface" );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
	r3dSetFiltering( R3D_POINT, 0 );

	r3dRenderer->SetPixelShader( PS_ADAPTIVEDEPTHBLUR7_ID );
	r3dRenderer->SetVertexShader( "VS_POSTFX" );

	SetRestoreClampAddressMode setRestoreAdress0( 0 ); (void)setRestoreAdress0;
	SetRestoreClampAddressMode setRestoreAdress1( 1 ); (void)setRestoreAdress1;

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	extern float g_DoubleDepthSSAO_Blur;
	extern float g_DoubleDepthSSAO_BlurSens;

	float commonPSConsts[ 4 ] = { g_DoubleDepthSSAO_Blur, g_DoubleDepthSSAO_BlurSens, 0.f, 0.f };
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)commonPSConsts, sizeof commonPSConsts / sizeof commonPSConsts[0] ) );

	//------------------------------------------------------------------------
	// first pass
	{
		temp->Activate();

		float invWidth	= 1.f / source->Width;
		float invHeight	= 1.f / source->Height;
		float steps[ 1 ][ 4 ] =
		{
			0.f, source->Height / temp->Height * invHeight, 0.f, 0.f
		};

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 1, (float*)steps, sizeof steps / sizeof steps[0] ) );

		float vSubPix[4] = {	+0.0f / temp->Width		+ 0.25f / source->Width,
								+0.0f / temp->Height	+ 0.25f / source->Height, 0.f, 0.f };
		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vSubPix, 1 ) );

		r3dRenderer->SetTex( source->Tex );

		r3dDrawFullScreenQuad(false);

		temp->Deactivate();
	}

	//------------------------------------------------------------------------
	// second path
	{
		target->Activate();

		float invWidth	= 1.f / temp->Width;
		float invHeight	= 1.f / temp->Height;

		float steps[ 1 ][ 4 ] =
		{
			invWidth, 0.f, 0.f, 0.f
		};

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 1, (float*)steps, sizeof steps / sizeof steps[0] ) );

		float vSubPix[4] = {	+0.0f / target->Width + 0.25f / temp->Width,
								+0.0f / target->Height + 0.25f / temp->Height, 0.f, 0.f };
		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vSubPix, 1 ) );

		r3dRenderer->SetTex( temp->Tex );

		r3dDrawFullScreenQuad(false);

		target->Deactivate();
	}

	D3DPERF_EndEvent();

}

static void DrawMinimum( r3dScreenBuffer* source0, r3dScreenBuffer* source1, r3dScreenBuffer* target, bool PCF )
{
	D3DPERF_BeginEvent( 0, L"DrawMinimum" );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
	r3dSetFiltering( R3D_POINT, 0 );
	r3dSetFiltering( R3D_POINT, 1 );

	if( PCF )
	{
		float invW = 1.f / source0->Width;
		float invH = 1.f / source0->Height;

		float invW_1_5 = invW * 1.5f;
		float invW_0_5 = invW * 0.5f;

		float invH_1_5 = invH * 1.5f;
		float invH_0_5 = invH * 0.5f;

		float vConsts[ 3 ][ 4 ] =
		{
			source0->Width, source0->Height, invW, invH,
			invW_0_5, invH_0_5, invW_1_5, invH_0_5,
			invW_0_5, invH_1_5, invW_1_5, invH_1_5
		};

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)vConsts, sizeof vConsts / sizeof vConsts[ 0 ] ) );

		r3dRenderer->SetPixelShader( PS_PCF_MINIMUM_ID );
	}
	else
	{
		r3dRenderer->SetPixelShader( PS_MINIMUM_ID );
	}

	r3dRenderer->SetVertexShader( "VS_POSTFX" );

	SetRestoreClampAddressMode setRestoreAdress0(0); (void)setRestoreAdress0;
	SetRestoreClampAddressMode setRestoreAdress1(1); (void)setRestoreAdress1;
	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	target->Activate();

	r3dRenderer->SetTex( source0->Tex, 0 );
	r3dRenderer->SetTex( source1->Tex, 1 );
	float vSubPix[4] = { 0.5f / target->Width, 0.5f / target->Height, 0.f, 0.f };
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vSubPix, 1 ) );

	r3dDrawFullScreenQuad(false);

	target->Deactivate();

	D3DPERF_EndEvent();
}

static void DrawMinimumMask( r3dScreenBuffer* source0, r3dScreenBuffer* source1, r3dScreenBuffer* target )
{
	D3DPERF_BeginEvent( 0, L"DrawMinimumMask" );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
	r3dSetFiltering( R3D_POINT, 0 );
	r3dSetFiltering( R3D_POINT, 1 );

	r3dRenderer->SetPixelShader( PS_MINIMUM_MASK_ID );
	r3dRenderer->SetVertexShader( "VS_POSTFX" );

	SetRestoreClampAddressMode setRestoreAdress0(0); (void)setRestoreAdress0;
	SetRestoreClampAddressMode setRestoreAdress1(1); (void)setRestoreAdress1;
	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	target->Activate();

	r3dRenderer->SetTex( source0->Tex, 0 );
	r3dRenderer->SetTex( source1->Tex, 1 );
	float vSubPix[4] = { 0.5f / target->Width, 0.5f / target->Height, 0.f, 0.f };
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vSubPix, 1 ) );

	r3dDrawFullScreenQuad(false);

	D3DPERF_EndEvent();
}

void SetupShadowTexMatrix( D3DXMATRIX* oMtx, r3dScreenBuffer* screen, float smSize, bool texPack )
{
	float kx = smSize / screen->Width;
	float ky = smSize / screen->Height;

	float tpc = texPack ? 0.5f : 1.0f;
	float tpo = texPack ? 0.5f : 0.0f;

	float off_x = tpo + ( tpc / screen->Width );
	float off_y = tpo + ( tpc / screen->Height );

	*oMtx =	D3DXMATRIX (	tpc * kx,	0.0f,		0.0f,		0.0f,
							0.0f,		-tpc * ky,	0.0f,		0.0f,
							0.0f,		0.0f,		1.0f,		0.0f,
							off_x * kx,	off_y * ky,	0.0f,		1.0f );

}

void SetupAccumSMConstants(	float gameNearClip, float gameFarClip,
							const D3DXMATRIX& gameV,
							const D3DXMATRIX& gameP,
							const D3DXMATRIX& lightT,
							const r3dPoint3D& lightPos,
							float smSize,
							float shadowBias,
							float sliceDist,
							float shadowDist,
							float physRange,
							r3dScreenBuffer* SM,
							bool applyTexXForm,
							bool lastSlice,
							float PLDepthScale )
{
	float gameZProj0 = gameP._33;
	float gameZProj1 = gameP._43;

	D3DXMATRIX scale;

	memset( &scale, 0, sizeof scale );

	float planeDistance = R3D_MIN( ( gameNearClip + gameFarClip ) * 0.5f, gameNearClip * 512.f );

	scale._11 = planeDistance;
	scale._22 = planeDistance;
	scale._33 = planeDistance;
	scale._44 = planeDistance;

	// also move .z further from the camera to battle
	// precision issues when reconstructing world
	// position
	scale._43 = gameZProj0 * planeDistance + gameZProj1;

	// less noise if we inverse these things separately

	r3dPoint3D lp = r3dRenderer->LocalizationPos;

	// inverse view
	D3DXMATRIX invV;
	{
		D3DXMATRIX TInv(	1,					0,					0,				0,
							0,					1,					0,				0,
							0,					0,					1,				0,
							-gameV._41,			-gameV._42,			-gameV._43,		1 );

		D3DXMATRIX RInv;

		D3DXMatrixTranspose( &RInv, &gameV );
		RInv._41 = RInv._42 = RInv._43 = 0.f;

		invV = TInv * RInv;
	}

	// inverse proj (assumes perspective proj)
	D3DXMATRIX invP ;
	r3dPerspProjInverse( invP, gameP );

	D3DXMATRIX sm = scale * invP * invV;

	D3DXVECTOR4 VSConsts[ ] =
	{
		// float4x4 	g_mInvViewProj 		: register ( c32 );
		D3DXVECTOR4( sm._11, sm._21, sm._31, sm._41 ),
		D3DXVECTOR4( sm._12, sm._22, sm._32, sm._42 ),
		D3DXVECTOR4( sm._13, sm._23, sm._33, sm._43 ),
		D3DXVECTOR4( sm._14, sm._24, sm._34, sm._44 ),
		// float4 		g_vTexcTransform	: register ( c36 );
		D3DXVECTOR4( 1.f, 1.f, 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH )
	};

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 32, (float*)VSConsts, sizeof VSConsts / sizeof VSConsts[ 0 ] ) );

	D3DXMATRIX mTexScale ;
	SetupShadowTexMatrix( &mTexScale, SM, smSize, r3dRenderer->ShadowPassType != SPT_ORTHO_WARPED ) ;

	D3DXMATRIX lm = applyTexXForm ? lightT * mTexScale : lightT;

	float sliceDistSQ = sliceDist;
	sliceDistSQ *= sliceDistSQ;

	float edgeFadeFactor = lastSlice ? sliceDistSQ * 0.1f : 0.000001f;

	float fxaaAdditionalBias = 0;
	if (r_active_shadow_slices->GetInt() == 2)
	{
		fxaaAdditionalBias = shadowBias * 0.5f;
	}

	D3DXVECTOR4 PSConsts[ ] =
	{
		// float4x4 LightMtx         : register(c0);
		D3DXVECTOR4( lm._11, lm._21, lm._31, lm._41 ),
		D3DXVECTOR4( lm._12, lm._22, lm._32, lm._42 ),
		D3DXVECTOR4( lm._13, lm._23, lm._33, lm._43 ),
		D3DXVECTOR4( lm._14, lm._24, lm._34, lm._44 ),
		// float4   ReconstructParam : register(c4);
		D3DXVECTOR4( gCam.x, gCam.y, gCam.z, 1.f / planeDistance ),
		// float4   ShadowTexScale   : register(c5);
		D3DXVECTOR4( SM->Width, SM->Height, 1.f / SM->Width, 1.f / SM->Height ),
		// float4   ShadowParam0     : register(c6);
		D3DXVECTOR4( 1.f / SM->Width, fxaaAdditionalBias, 0.f, 1.f / SM->Height ),
		// float4   ShadowParam1     : register(c7);
		D3DXVECTOR4( 1.f / SM->Width, 1.f / SM->Height, sliceDistSQ, shadowBias ),
		// float4   ShadowParam2     : register(c8);
		D3DXVECTOR4( physRange, 1.f / shadowDist, edgeFadeFactor, PLDepthScale ),
		// float3   LightPos         : register(c9);
		D3DXVECTOR4( lightPos.x, lightPos.y, lightPos.z, 0.f ),
		// float3   Offset           : register(c10);
		D3DXVECTOR4( lp.x, lp.y, lp.z, 0.f ),
		// float SM offset
		D3DXVECTOR4( r_shadow_normals_offset->GetFloat() / SM->Width, r_shadow_normals_offset->GetFloat() / SM->Height, 0, 0 ),
	};

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)PSConsts, sizeof PSConsts / sizeof PSConsts[ 0 ] ) );
}

ShadowMapOptimizationData gShadowMapOptimizationDataOpaque[ NumShadowSlices ];
int gCurrentShadowSlice ;

void FillSliceForSplitDistances( ShadowSlice* ioSlice, float* shadowSplitDistances, ShadowMapOptimizationData* optimizationData, r3dPoint3D* oLightSource, r3dPoint3D* oLightTarget, float fNear, float fFar, bool allow_optimize, float border )
{
	// Calculate corner points of frustum split
	D3DXVECTOR3 pCorners[8];
	CalculateFrustumCorners(	pCorners,
								D3DXVECTOR3(gCam.x, gCam.y, gCam.z),
								D3DXVECTOR3(gCam.x + gCam.vPointTo.x*100, gCam.y + gCam.vPointTo.y*100, gCam.z + gCam.vPointTo.z*100),
								D3DXVECTOR3(0, 1, 0),
								fNear,fFar,gCam.FOV,gCam.Aspect );

	// Calculate view and projection matrices
	r3dPoint3D sunLookAt = gCam;
	*oLightSource = sunLookAt - SunVector*ShadowSunOffset;
	*oLightTarget = sunLookAt;

	if( r_optimize_shadow_map->GetInt() && allow_optimize )
		CalculateLightForFrustum_Optimized(	ioSlice, shadowSplitDistances, optimizationData, pCorners,
											(D3DXVECTOR3*)oLightSource, (D3DXVECTOR3*)oLightTarget,
											Sun->SunLight.Direction, border );
	else
		CalculateLightForFrustum( ioSlice, pCorners, (D3DXVECTOR3*)oLightSource, (D3DXVECTOR3*)oLightTarget, Sun->SunLight.Direction);
}

void FillLFSliceForSplitDistances( ShadowSlice* ioSlice, r3dPoint3D* oLightSource, r3dPoint3D* oLightTarget, float fNear, float fFar, bool allow_optimize )
{
	D3DXVECTOR3 pCorners[8];
	CalculateFrustumCorners(	pCorners,
								D3DXVECTOR3(0.f, 0.f, 0.f),
								D3DXVECTOR3(gCam.vPointTo.x*100, gCam.vPointTo.y*100, gCam.vPointTo.z*100),
								D3DXVECTOR3(0, 1, 0),
								fNear,fFar,gCam.FOV,gCam.Aspect );

	const D3DXVECTOR3& c = pCorners[ 4 ] ;

	float maxDist = sqrtf( c.x * c.x + c.y * c.y + c.z * c.z ) ;

	*oLightTarget = r3dPoint3D(gCam.x, gCam.y, gCam.z);
	r3dPoint3D sunV = r3dPoint3D(SunVector.x, SunVector.y, SunVector.z);
	*oLightSource = *oLightTarget - sunV*ShadowSunOffset;

	float fLightFar = ShadowSunOffset * 2.f;
	float fLightNear = 1;

	D3DXVECTOR3 lsource ( oLightSource->x, oLightSource->y, oLightSource->z ) ;
	D3DXVECTOR3 ltarget ( oLightTarget->x, oLightTarget->y, oLightTarget->z ) ;

	CalculateViewProj(	&ioSlice->lightView, &ioSlice->lightProj,
						lsource, ltarget, D3DXVECTOR3(0,1,0),
						90, fLightNear, fLightFar, 1 );

	r3dRenderer->BuildMatrixOrthoOffCenterLH( &ioSlice->lightProj, -maxDist, +maxDist, -maxDist, +maxDist, fLightNear, fLightFar );

	ioSlice->sphereRadius = maxDist ;
	ioSlice->worldSizeX = maxDist ;
	ioSlice->worldSizeY = maxDist ;
	ioSlice->camPos = *oLightSource ;

	ioSlice->pixelDiameter = 2 * maxDist * 1.41421356f / ioSlice->shadowMapSize / ( fLightFar - fLightNear );
}

void PrepareSlicedShadowMapRender()
{
	r3dPoint3D lightSource, lightTarget;

	for( uint32_t i = 0, e = R3D_ARRAYSIZE(gShadowMapOptimizationDataOpaque); i < e ; i ++ )
	{
		gShadowMapOptimizationDataOpaque[ i ].fMinX = + FLT_MAX ;
		gShadowMapOptimizationDataOpaque[ i ].fMaxX = - FLT_MAX ;
		gShadowMapOptimizationDataOpaque[ i ].fMinY = + FLT_MAX ;
		gShadowMapOptimizationDataOpaque[ i ].fMaxY = - FLT_MAX ;
	}

	if( r_lfsm_wrap_mode->GetInt() )
	{
		FillLFSliceForSplitDistances(	&CompoundSlice, &lightSource, &lightTarget,
										ShadowSplitDistancesOpaque[ 0 ], ShadowSplitDistancesOpaque[ NumShadowSlices ], false );
	}
	else
	{
		FillSliceForSplitDistances(	&CompoundSlice, ShadowSplitDistancesOpaque, gShadowMapOptimizationDataOpaque,
									&lightSource, &lightTarget,
									ShadowSplitDistancesOpaque[ 0 ], ShadowSplitDistancesOpaque[ NumShadowSlices ], false, 0.f );
	}

	r3dCamera shadowCam;
	shadowCam.FOV = 90;
	shadowCam.SetPosition(lightSource);
	shadowCam.PointTo(lightTarget);
	shadowCam.NearClip = 0.1f;
	shadowCam.FarClip = ShadowSunOffset * 2.f;

	r3dRenderer->SetCamera( gCam, false );

	D3DXPLANE mainFrustumPlanes[ 6 ];
	memcpy( mainFrustumPlanes, r3dRenderer->FrustumPlanes, sizeof mainFrustumPlanes );

	r3dRenderer->SetCameraEx( CompoundSlice.lightView, CompoundSlice.lightProj, 0.f, ShadowSplitDistancesOpaque[ NumShadowSlices ], false );

	GameWorld().PrepareSlicedShadowsInterm( shadowCam, mainFrustumPlanes );
}

int NeedUpdateCachedSlice()
{
	int NeedUpdateSM = 1;

	if( float d = r_lfsm_cache_dist->GetFloat() )
	{
		r3dPoint3D dvec = gCam - LastLFShadowSliceUpdatePos ;

		dvec.y = 0 ;

		float curD = dvec.Length() ;

		if
			(
			curD <= d
#if ENABLE_MULTI_GPU_OPTIMIZATIONS
			&& gSLI_Crossfire_NumGPUs < 2
#endif
			)
		{
			NeedUpdateSM = 0 ;
		}
	}

	return NeedUpdateSM;
}

void RenderShadowMap( ShadowSlice& slice, r3dScreenBuffer* depthBuffer, bool recticular_warp )
{
	R3DPROFILE_FUNCTION("RenderShadowMap");

	D3DPERF_BeginEvent( 0, L"RenderShadowMap" );

	r3dRenderer->SetCamera( gCam, false );

	int NeedUpdateSM = 1 ;

	int isLastSlice = slice.index == r_active_shadow_slices->GetInt() - 1 ;

	if( isLastSlice )
	{
		NeedUpdateSM = NeedUpdateCachedSlice();

		if( NeedUpdateSM )
		{
			LastLFShadowSliceUpdatePos = gCam ;
		}
	}

	r3dRenderer->ShadowPassType = recticular_warp ? SPT_ORTHO_WARPED : SPT_ORTHO;
	r3dRenderer->ShadowSliceIndexBit = 1<<slice.index;

	r3dScreenBuffer* origTraget = isLastSlice ? LFUpdateShadowMap : SunShadowMap ;
	r3dScreenBuffer* targSM = origTraget ;

	D3DXMATRIX gameV = r3dRenderer->ViewMatrix;
	D3DXMATRIX gameV_Loc = r3dRenderer->ViewMatrix_Localized;
	D3DXMATRIX gameP = r3dRenderer->ProjMatrix;

	float gameNearClip = r3dRenderer->NearClip;
	float gameFarClip = r3dRenderer->FarClip;

	if( NeedUpdateSM )
	{
		gCurrentShadowSlice = slice.index ;

		SunVector = GetEnvLightDir();

		D3DXVECTOR4 LightVec;

		// near and far planes for current frustum split
		float fNear = ShadowSplitDistancesOpaque[slice.index];
		float fFar = ShadowSplitDistancesOpaque[slice.index+1];

		// extend frustum backwards to include areas due to frustum space 'holes' with respect to 'slice sphere'
		if( slice.index )
		{
			float fTanSqr = tanf( R3D_DEG2RAD( gCam.FOV * 0.5f ) ) ;
			fTanSqr *= fTanSqr ;

			fNear = fNear / sqrtf( 1 + fTanSqr + gCam.Aspect * gCam.Aspect * fTanSqr );
		}

		for( int i = 0, e = 8 ; i < e ; i ++ )
		{
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );

			r3dSetFiltering( R3D_BILINEAR, i );
		}

		r3dPoint3D lightSource, lightTarget;

		{
			FillSliceForSplitDistances( &slice, ShadowSplitDistancesOpaque, gShadowMapOptimizationDataOpaque, &lightSource, &lightTarget, fNear, fFar, false, 0.f );

			// set 'suboptimal' camera so that trees check themselves correctly and apply their optimizations
			// which will be accounted in next FillSlice.. in case optimization is enabled
			r3dRenderer->SetCameraEx(slice.lightView, slice.lightProj, 0.1f, ShadowSunOffset * 2.f, false);
			gCollectionsManager.ComputeVisibility(true, true);
		}

		if( r_lfsm_wrap_mode->GetInt() && isLastSlice )
		{
			FillLFSliceForSplitDistances(	&slice, &lightSource, &lightTarget, fNear, fFar, true );
		}
		else
		{
			FillSliceForSplitDistances( &slice, ShadowSplitDistancesOpaque, gShadowMapOptimizationDataOpaque,
										&lightSource, &lightTarget, fNear, fFar, true, 0.f );
		}

		float PSConst[ 4 ] = { slice.pixelDiameter, 0.f, 0.f, 0.f };
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( SHADOWC_PIXELDIAMETER, PSConst, 1 ) );

		slice.camPos = lightSource;
		r3dRenderer->SetCameraEx(slice.lightView, slice.lightProj, 0.1f, ShadowSunOffset * 2.f, false);

		targSM->Activate();

		r3dRenderer->StartRender(0, false);

		// override viewport
		r3dRenderer->SetViewport( 0, 0, slice.shadowMapSize, slice.shadowMapSize );

		GameWorld().RecalcIntermObjectMatrices();

		{
			struct SetRestoreStates
			{
				SetRestoreStates()
				{
					D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_SCISSORTESTENABLE, &prevValue) );
					D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

					if( r_hardware_shadow_method->GetInt() != HW_SHADOW_METHOD_R32F )
					{
						D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, 0 ) );
					}
				}

				~SetRestoreStates()
				{
					D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, prevValue ) );

					if( r_hardware_shadow_method->GetInt() != HW_SHADOW_METHOD_R32F )
					{
						D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED		|
																								D3DCOLORWRITEENABLE_GREEN	|
																								D3DCOLORWRITEENABLE_BLUE	|
																								D3DCOLORWRITEENABLE_ALPHA
																								) );
					}

				}

				DWORD  prevValue;
			} setRestoreScissor; (void)setRestoreScissor;

			D3D_V( r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) );

			if ( targSM->AsTex2D() && !IsDepthTextureFormat( targSM->BufferFormat ) )
				ClearFloatSurface( D3DXVECTOR4( 33.f, 0.f, 0.f, 0.f ) );

			r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

			LightVec = D3DXVECTOR4(lightSource.X,lightSource.Y,lightSource.Z, 0);
			r3dRenderer->pd3ddev->SetPixelShaderConstantF( 20, (float *)&LightVec,	1 );


			SMDepthVShaderID vsid;

			vsid.type = recticular_warp ? SMVSTYPE_ORTHO_WARPED : SMVSTYPE_ORTHO;

			r3dRenderer->SetVertexShader( gSMDepthVSIds[ vsid.Id ] );


			bool hwShadows = r_hardware_shadow_method->GetInt() > HW_SHADOW_METHOD_R32F;

			SMDepthPShaderID psid;

			psid.type = hwShadows ? SMPSTYPE_HW : SMPSTYPE_DEFAULT;

			r3dRenderer->SetPixelShader( gSMDepthPSIds[ psid.Id ] );

			if( r_cw_shadows->GetBool() )
			{
				r3dRenderer->SetDefaultCullMode( D3DCULL_CW );
			}
			else
			{
				r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
			}

			GameWorld().DrawIntermediate( eRenderStageID( rsCreateSM + slice.index ) );

			r3dRenderer->pd3ddev->SetPixelShaderConstantF( 20, (float *)&LightVec,	1 );

			r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

			if( r_terra_shadows->GetBool() )
			{
				if( Terrain3 && r_terrain3->GetInt() )
				{
					Terrain3->RenderShadows();
				}
				if( Terrain2 && r_terrain2->GetInt() )
				{
					Terrain2->RenderShadows() ;
				}
			}

			r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
			r3dRenderer->RestoreCullMode();

			r3dRenderer->SetVertexShader();
			r3dRenderer->SetPixelShader();

			r3dRenderer->EndRender();

			targSM->Deactivate();
		}
	}

	targSM = origTraget;

	// accumulate shadows into temporary buffer
	{
		R3DPROFILE_START( "RenderShadowMap: Accumulate Mask" ) ;

		SetRestoreClampAddressMode setRestoreClamp0( 0 ); setRestoreClamp0;
		SetRestoreClampAddressMode setRestoreClamp1( 1 ); setRestoreClamp1;

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

		TempShadowBuffer->Activate();

		r3dSetFiltering( R3D_POINT, 0 );
		r3dSetFiltering( R3D_POINT, 1 );
		r3dSetFiltering( R3D_BILINEAR, 2 );

		r3dRenderer->SetTex( targSM->Tex, 0 );
		r3dRenderer->SetTex( depthBuffer->Tex, 1 );
		r3dRenderer->SetTex( targSM->Tex, 2 );

		if (r_shadow_use_normals->GetInt())
			r3dRenderer->SetTex( gBuffer_Normal->Tex, 3 );

		float sliceDist = ShadowSplitDistancesOpaque[slice.index + 1] * 0.925f;
		float totalDist = ShadowSplitDistancesOpaque[r_active_shadow_slices->GetInt()];

		float biasCoef = (isLastSlice && r_lfsm_wrap_mode->GetInt()) ? 16.f : 2.f ;

		if( r_active_shadow_slices->GetInt() == 1 && isLastSlice )
		{
			biasCoef *= 2.f * r_shadow_low_size_coef->GetFloat() ;
		}

		float shadowBias = slice.depthBias ;

		if( r_hardware_shadow_method->GetFloat() == HW_SHADOW_METHOD_HW_PCF )
		{
			shadowBias = slice.depthBias_HW ;
		}

		SetupAccumSMConstants(	gameNearClip, gameFarClip,
								gameV_Loc, gameP,
								slice.lightView * slice.lightProj,
								r3dPoint3D( 0, 0, 0 ),
								slice.shadowMapSize,
								shadowBias * biasCoef,
								sliceDist,
								totalDist,
								r_shadows_blur_phys_range->GetFloat(),
								targSM,
								true,
								isLastSlice ? true : false,
								1.f
								);

		AccumShadowsPShaderID accumPSId;

		accumPSId.light_type		= SHADOWACCUM_LIGHT_DIRECT;
		accumPSId.fxaa_blur			= r_fxaa_shadow_blur->GetBool();
		accumPSId.hw_shadowmaps		= r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF;
		accumPSId.recticular_warp	= recticular_warp;
		accumPSId.use_normals = r_shadow_use_normals->GetInt();

		// PS_DS_SHADOW_ACCUM

		r3dRenderer->SetPixelShader( gAccumShadowsPShaderIDs[ accumPSId.Id ] );

		r3dRenderer->SetVertexShader( "VS_POSTFX_RECONST_W" );

		r3dSetRestoreFSQuadVDecl postFXVDecl; (void)postFXVDecl;

		r3dDrawFullScreenQuad(false);

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

		TempShadowBuffer->Deactivate();

		R3DPROFILE_END( "RenderShadowMap: Accumulate Mask" ) ;
	}

	D3DPERF_EndEvent(  );
}

//------------------------------------------------------------------------

void RenderTransparentShadowMap()
{
	R3DPROFILE_FUNCTION("RenderTransparentShadowMap");

	D3DPERF_BeginEvent( 0, L"RenderTransparentShadowMap" );

	r3dRenderer->SetCamera( gCam, false );

	r3dScreenBuffer* targSM = TransparentShadowMap ;

	D3DXMATRIX gameV = r3dRenderer->ViewMatrix;
	D3DXMATRIX gameP = r3dRenderer->ProjMatrix;

	float gameNearClip = r3dRenderer->NearClip;
	float gameFarClip = r3dRenderer->FarClip;

	ShadowSlice& slice = TransparentShadowSlice ;

	// update SM
	{
		SetRestoreZDir setRestoreZDir( r3dRenderLayer::ZDIR_NORMAL ); (void)setRestoreZDir;

		SunVector = GetEnvLightDir();

		// near and far planes for current frustum split
		float fNear = ShadowSplitDistancesTransparent[slice.index];
		float fFar = ShadowSplitDistancesTransparent[slice.index+1];

		for( int i = 0, e = 8 ; i < e ; i ++ )
		{
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );

			r3dSetFiltering( R3D_BILINEAR, i );
		}

		r3dPoint3D lightSource, lightTarget;

		// consider optimizing particle shadow maps like in the code below
#if 0
		if( gInstance_Compute_Visibility )
		{
			FillSliceForSplitDistances( &slice, &lightSource, &lightTarget, fNear, fFar, false );

			// set 'suboptimal' camera so that trees check themselves correctly and apply their optimizations
			// which will be accounted in next FillSlice.. in case optimization is enabled
			r3dRenderer->SetCameraEx(slice.lightView, slice.lightProj, lightSource, 0.1f, ShadowSunOffset * 2.f);
			gCollectionsManager.ComputeVisibility(true, true);
			gInstance_Compute_Visibility ( true, true );
		}
#endif

		FillSliceForSplitDistances( &slice, ShadowSplitDistancesTransparent, NULL, &lightSource, &lightTarget, fNear, fFar, true, 1.f / r_transp_shadowmap_fade->GetFloat() );

		slice.camPos = lightSource;

		const float NEAR_PLANE = 0.1f;
		const float FAR_PLANE = ShadowSunOffset * 2.f;

		r3dRenderer->SetCameraEx(slice.lightView, slice.lightProj, NEAR_PLANE, FAR_PLANE, false );

		r3dCamera shadowCam;
		shadowCam.FOV = 90;
		shadowCam.SetPosition(lightSource);
		shadowCam.PointTo(lightTarget);
		shadowCam.NearClip = NEAR_PLANE;
		shadowCam.FarClip = FAR_PLANE;
		shadowCam.SetOrtho( slice.sphereRadius * 2, slice.sphereRadius * 2 ) ;

		GameWorld().PrepareTransparentShadowsInterm( shadowCam ) ;

		targSM->Activate();

		r3dRenderer->StartRender(0, false);

		GameWorld().RecalcIntermObjectMatrices();

		{
			struct SetRestoreStates
			{
				SetRestoreStates()
				{
					D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_SCISSORTESTENABLE, &prevValue) );
					D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );
				}

				~SetRestoreStates()
				{
					D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, prevValue ) );
				}

				DWORD  prevValue;
			} setRestoreScissor; (void)setRestoreScissor;

#if 0
			D3D_V( r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) );
#endif

			if ( targSM->AsTex2D() && !IsDepthTextureFormat( targSM->BufferFormat ) )
				ClearFloatSurface( D3DXVECTOR4( 33.f, 1.f, 0.f, 0.f ) );

			r3dRenderer->SetRenderingMode( R3D_BLEND_MIN );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

			r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
			r3dRenderer->RestoreCullMode() ;

			r3dRenderer->ShadowPassType = SPT_ORTHO;
			r3dRenderer->ShadowSliceIndexBit = 0 ;

			GameWorld().DrawIntermediate( rsCreateTransparentSM );

			r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
			r3dRenderer->RestoreCullMode();

			r3dRenderer->SetVertexShader();
			r3dRenderer->SetPixelShader();

			r3dRenderer->EndRender();

			targSM->Deactivate();
		}
	}

	D3DPERF_EndEvent(  );
}


//------------------------------------------------------------------------

void DoRenderShadowScheme( float sx, float sy, float width, float height, ShadowSlice** slices, int slice_count, float* shadowSplitDistances )
{
	r3dPoint3D lightSource, lightTarget;

	float PixelDiameter;

	static r3dTL::TArray< D3DXVECTOR3 > frustumPoints;
	static r3dTL::TArray< r3dPoint2D > smPoints;

	frustumPoints.Clear();
	smPoints.Clear();

	for( size_t i = 0, e = slice_count ; i < e; i ++ )
	{
		ShadowSlice& slice = *slices[ i ] ;

		PixelDiameter ;

		float fNear	= shadowSplitDistances[ slice.index + 0 ];
		float fFar	= shadowSplitDistances[ slice.index + 1 ];

		D3DXVECTOR3 pCorners[8];
		CalculateFrustumCorners(	pCorners,
									D3DXVECTOR3(gCam.x, gCam.y, gCam.z),
									D3DXVECTOR3(gCam.x + gCam.vPointTo.x*100, gCam.y + gCam.vPointTo.y*100, gCam.z + gCam.vPointTo.z*100),
									D3DXVECTOR3(0, 1, 0),
									fNear,fFar,gCam.FOV,gCam.Aspect );

		for( size_t i = 0, e = R3D_ARRAYSIZE( pCorners ); i < e; i += 4 )
		{
			D3DXVECTOR4 p ;

			D3DXVec3Transform( &p, &pCorners[ i + 0 ], &slice.lightView );
			frustumPoints.PushBack( D3DXVECTOR3( p.x, p.y, p.z ) );
			D3DXVec3Transform( &p, &pCorners[ i + 1 ], &slice.lightView );
			frustumPoints.PushBack( D3DXVECTOR3( p.x, p.y, p.z ) );
			D3DXVec3Transform( &p, &pCorners[ i + 3 ], &slice.lightView );
			frustumPoints.PushBack( D3DXVECTOR3( p.x, p.y, p.z ) );
			D3DXVec3Transform( &p, &pCorners[ i + 2 ], &slice.lightView );
			frustumPoints.PushBack( D3DXVECTOR3( p.x, p.y, p.z ) );
		}

		D3DXVECTOR4 p ;

		D3DXMATRIX invP ;
		D3DXMatrixInverse( &invP, NULL, &slice.lightProj );

		p = D3DXVECTOR4( -1, -1, 0, 1 );
		D3DXVec4Transform( &p, &p, &invP );
		p *= 1 / p.w ;
		smPoints.PushBack( r3dPoint2D( p.x, p.y ) );

		p = D3DXVECTOR4( +1, -1, 0, 1 );
		D3DXVec4Transform( &p, &p, &invP );
		p *= 1 / p.w ;
		smPoints.PushBack( r3dPoint2D( p.x, p.y ) );

		p = D3DXVECTOR4( +1, +1, 0, 1 );
		D3DXVec4Transform( &p, &p, &invP );
		p *= 1 / p.w ;
		smPoints.PushBack( r3dPoint2D( p.x, p.y ) );

		p = D3DXVECTOR4( -1, +1, 0, 1 );
		D3DXVec4Transform( &p, &p, &invP );
		p *= 1 / p.w ;
		smPoints.PushBack( r3dPoint2D( p.x, p.y ) );
	}

	{
		float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX ;

		for( uint32_t i = 0, e = frustumPoints.Count(); i < e; i ++ )
		{
			minX = R3D_MIN( minX, frustumPoints[ i ].x );
			minY = R3D_MIN( minY, frustumPoints[ i ].y );
			maxX = R3D_MAX( maxX, frustumPoints[ i ].x );
			maxY = R3D_MAX( maxY, frustumPoints[ i ].y );
		}

		for( uint32_t i = 0, e = smPoints.Count(); i < e; i ++ )
		{
			minX = R3D_MIN( minX, smPoints[ i ].x );
			minY = R3D_MIN( minY, smPoints[ i ].y );
			maxX = R3D_MAX( maxX, smPoints[ i ].x );
			maxY = R3D_MAX( maxY, smPoints[ i ].y );
		}

		float dx = maxX - minX ;
		float dy = maxY - minY ;

		for( uint32_t i = 0, e = frustumPoints.Count(); i < e; i ++ )
		{
			frustumPoints[ i ].x -= minX ;
			frustumPoints[ i ].x /= dx ;
			frustumPoints[ i ].x *= width ;
			frustumPoints[ i ].x += sx ;

			frustumPoints[ i ].y -= minY ;
			frustumPoints[ i ].y /= dy ;
			frustumPoints[ i ].y *= height ;
			frustumPoints[ i ].y += sy ;

			frustumPoints[ i ].z = 0 ;
		}

		for( uint32_t i = 0, e = smPoints.Count(); i < e; i ++ )
		{
			smPoints[ i ].x -= minX ;
			smPoints[ i ].x /= dx ;
			smPoints[ i ].x *= width ;
			smPoints[ i ].x += sx ;

			smPoints[ i ].y -= minY ;
			smPoints[ i ].y /= dy ;
			smPoints[ i ].y *= height ;
			smPoints[ i ].y += sy ;
		}

		r3dDrawLine2D( sx, sy, sx + width, sy, 1.0f, r3dColor::white );
		r3dDrawLine2D( sx + width, sy, sx + width, sy + height, 1.0f, r3dColor::white );
		r3dDrawLine2D( sx + width, sy + height, sx, sy + height, 1.0f, r3dColor::white );
		r3dDrawLine2D( sx, sy + height, sx, sy, 1.0f, r3dColor::white );

		r3dColor colors[ 3 ] = { r3dColor( 255, 0, 0, 192 ), r3dColor( 0, 255, 0, 192 ), r3dColor( 0, 0, 255, 192 ) };

		for( uint32_t i = smPoints.Count() - 4, e = smPoints.Count(), f = 1 ; i < e; i -= 4 )
		{
			r3dColor drawColr = colors[ i % R3D_ARRAYSIZE( colors ) ] ;
			if( f )
			{
				drawColr.A = 127 ;
				f = 0 ;
			}

			r3dDrawBox2D( smPoints[ 0 + i ], smPoints[ 1 + i ], smPoints[ 2 + i ], smPoints[ 3 + i ], drawColr );
		}

		for( uint32_t i = 0, e = frustumPoints.Count() ; i < e; i += 8 )
		{
			r3dDrawLine2D( frustumPoints[ i + 0 ].x, frustumPoints[ i + 0 ].y, frustumPoints[ i + 1 ].x, frustumPoints[ i + 1 ].y, 2.0f, r3dColor::blue );
			r3dDrawLine2D( frustumPoints[ i + 1 ].x, frustumPoints[ i + 1 ].y, frustumPoints[ i + 3 ].x, frustumPoints[ i + 3 ].y, 2.0f, r3dColor::blue );
			r3dDrawLine2D( frustumPoints[ i + 3 ].x, frustumPoints[ i + 3 ].y, frustumPoints[ i + 2 ].x, frustumPoints[ i + 2 ].y, 2.0f, r3dColor::blue );
			r3dDrawLine2D( frustumPoints[ i + 2 ].x, frustumPoints[ i + 2 ].y, frustumPoints[ i + 0 ].x, frustumPoints[ i + 0 ].y, 2.0f, r3dColor::blue );

			r3dDrawLine2D( frustumPoints[ i + 0 ].x, frustumPoints[ i + 0 ].y, frustumPoints[ i + 4 ].x, frustumPoints[ i + 4 ].y, 2.0f, r3dColor::white );
			r3dDrawLine2D( frustumPoints[ i + 1 ].x, frustumPoints[ i + 1 ].y, frustumPoints[ i + 5 ].x, frustumPoints[ i + 5 ].y, 2.0f, r3dColor::white );
			r3dDrawLine2D( frustumPoints[ i + 2 ].x, frustumPoints[ i + 2 ].y, frustumPoints[ i + 6 ].x, frustumPoints[ i + 6 ].y, 2.0f, r3dColor::white );
			r3dDrawLine2D( frustumPoints[ i + 3 ].x, frustumPoints[ i + 3 ].y, frustumPoints[ i + 7 ].x, frustumPoints[ i + 7 ].y, 2.0f, r3dColor::white );

			r3dDrawLine2D( frustumPoints[ i + 4 ].x, frustumPoints[ i + 4 ].y, frustumPoints[ i + 5 ].x, frustumPoints[ i + 5 ].y, 2.0f, r3dColor::blue );
			r3dDrawLine2D( frustumPoints[ i + 5 ].x, frustumPoints[ i + 5 ].y, frustumPoints[ i + 7 ].x, frustumPoints[ i + 7 ].y, 2.0f, r3dColor::blue );
			r3dDrawLine2D( frustumPoints[ i + 7 ].x, frustumPoints[ i + 7 ].y, frustumPoints[ i + 6 ].x, frustumPoints[ i + 6 ].y, 2.0f, r3dColor::blue );
			r3dDrawLine2D( frustumPoints[ i + 6 ].x, frustumPoints[ i + 6 ].y, frustumPoints[ i + 4 ].x, frustumPoints[ i + 4 ].y, 2.0f, r3dColor::blue );
		}
	}

	r3dRenderer->Flush();
}

//------------------------------------------------------------------------

void RenderShadowScheme( float sx, float sy, float width, float height )
{
	ShadowSlice* slices[ NumShadowSlices ] ;

	int active_count = r_active_shadow_slices->GetInt() ;

	for( size_t i = 0, e = active_count ; i < e; i ++ )
	{
		slices[ i ] = ShadowSlices + i ;
	}

	DoRenderShadowScheme( sx, sy, width, height, slices, active_count, ShadowSplitDistancesOpaque ) ;
}

//------------------------------------------------------------------------

void RenderTransparentShadowScheme( float sx, float sy, float width, float height )
{
	ShadowSlice* slices[ 1 ] = { &TransparentShadowSlice } ;

	DoRenderShadowScheme( sx, sy, width, height, slices, 1, ShadowSplitDistancesTransparent ) ;
}

//------------------------------------------------------------------------

void BlurShadowMap( r3dScreenBuffer* ShadowBuffer, r3dScreenBuffer* TempBuffer, float SX, float SY, float EX, float EY, const r3dSSSBParams& SSSBParms )
{
	D3DPERF_BeginEvent( 0, L"BlurShadowMap" );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

#if 0
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) );
#endif

	TempBuffer->Activate();

	r3dSetFiltering( R3D_POINT, 0 );

	r3dRenderer->SetTex( ShadowBuffer->Tex );

	SetRestoreClampAddressMode setRestoreClamp( 0 ); (void)setRestoreClamp;
	r3dSetRestoreFSQuadVDecl setRestorePostFXVdecl; (void)setRestorePostFXVdecl;

	float rad = SSSBParms.Radius ;

	const int HQ_BLUR_SHADOW_QL = 2 ;

	if( r_active_shadow_slices->GetInt() >= HQ_BLUR_SHADOW_QL )
	{
		// NOTE: sync with NUM_SAMPLES in DS_BlurShadows_ps.hls
		rad *= 5.0f / 9.f ;
	}

	// PS Consts

	// float4 Params0 : register( c0 );
	D3DXVECTOR4 PSConst = D3DXVECTOR4( SSSBParms.Bias, SSSBParms.Sense, rad / ShadowBuffer->Width, 0.f );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, (float*)&PSConst, 1 ) );

	if( r_active_shadow_slices->GetInt() >= HQ_BLUR_SHADOW_QL )
	{
		r3dRenderer->SetPixelShader( "PS_BLUR_SHADOWS_HQ" );
	}
	else
	{
		if( r_do_dyn_shadow_blur->GetBool() )
		{
			r3dRenderer->SetPixelShader( "PS_BLUR_SHADOWS_DYN" );
		}
		else
		{
			r3dRenderer->SetPixelShader( "PS_BLUR_SHADOWS" );
		}
	}

	float sizeX = ( EX - SX ) * 0.5f;
	float sizeY = ( EY - SY ) * 0.5f;

	D3DXVECTOR4 vsConsts[] =
	{
		// vSubPix
		D3DXVECTOR4(0.5f / ShadowBuffer->Width, 0.5f / ShadowBuffer->Height, 0.f, 0.f),
		//	Pos transform
		D3DXVECTOR4(sizeX, sizeY, sizeX + SX, sizeY + SY),
		//	Tc transform
		D3DXVECTOR4(sizeX, sizeY, 0.5f * SX + 0.5f, 0.5f - 0.5f * EY)
	};

	// VS Consts
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[0].x, _countof(vsConsts) ) );

	int prevShader = r3dRenderer->GetCurrentPixelShaderIdx();
	r3dRenderer->SetVertexShader(VS_POSTFX_GEOM_TRANSFORM);

	r3dDrawFullScreenQuad(false);

	TempBuffer->Deactivate();

	ShadowBuffer->Activate();

	r3dRenderer->SetTex( TempBuffer->Tex );

	PSConst.z = 0.f;
	PSConst.w = rad / ShadowBuffer->Height;

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, (float*)&PSConst, 1 ) );

	r3dDrawFullScreenQuad(false);

	r3dRenderer->SetVertexShader(prevShader);
	ShadowBuffer->Deactivate();

#if 0
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED		|
																			D3DCOLORWRITEENABLE_GREEN	|
																			D3DCOLORWRITEENABLE_BLUE	|
																			D3DCOLORWRITEENABLE_ALPHA ) );
#endif

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------

void FillParaboloidVertexConstant( float (&vsConst)[ 4 ], const r3dCamera* cam )
{
	vsConst[ 0 ] = 1.0f / ( cam->FarClip - cam->NearClip );
	vsConst[ 1 ] = -cam->NearClip / ( cam->FarClip - cam->NearClip );
	vsConst[ 2 ] = 0.0f;
	vsConst[ 3 ] = 0.0f;
}

//------------------------------------------------------------------------

void RenderShadowMap( const r3dCamera& lightCam, bool doubleDepth, bool paraboloid )
{
	R3DPROFILE_FUNCTION("RenderShadowMap");

	r3dSetFiltering( R3D_BILINEAR, 0 );

	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	r3dRenderer->SetCamera( lightCam, false );

	gCollectionsManager.SetInstanceViewRefPos(lightCam);

	float Const[ 8 ] =
	{
		lightCam.X, lightCam.Y, lightCam.Z, 0,
		1.0f / r3dRenderer->ScreenW, 0.f, 0.f, 0.f
	};
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( SHADOWC_PIXELDIAMETER - 1, Const, 2 ) );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	SMDepthVShaderID vsid;

	if( paraboloid )
	{
		vsid.type = SMVSTYPE_PERSP_PARABOLOID;
		r3dRenderer->ShadowPassType = SPT_PERSP_PARABOLOID;

		// float4  vParaboloidParams   : register( c30 );

		float vsConst[ 4 ];
		FillParaboloidVertexConstant( vsConst, &lightCam );

		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 30, vsConst, 1 );
	}
	else
	{
		vsid.type = SMVSTYPE_PERSP;
		r3dRenderer->ShadowPassType = SPT_PERSP;
	}

	// we should disregard slices when rendering these sort of shadows
	r3dRenderer->ShadowSliceIndexBit = 0xff;

	GameWorld().PrepareShadowsInterm( lightCam );

	GameWorld().RecalcIntermObjectMatrices();

	if( doubleDepth || paraboloid )
	{
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
		r3dRenderer->DoubleDepthShadowPath = 1;

		SMDepthPShaderID psid;
		psid.type = SMPSTYPE_SIMPLE;
		r3dRenderer->SetPixelShader( gSMDepthPSIds[ psid.Id ] );
	}
	else
	{
		r3dRenderer->DoubleDepthShadowPath = 0;

		SMDepthPShaderID psid;
		psid.type = SMPSTYPE_NORMAL_OFFSET;
		r3dRenderer->SetPixelShader( gSMDepthPSIds[ psid.Id ] );
	}

	r3dRenderer->SetVertexShader( gSMDepthVSIds[ vsid.Id ] );

	GameWorld().DrawIntermediate( rsCreateSM );

	if( doubleDepth )
	{
		r3dRenderer->DoubleDepthShadowPath = 0;
		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
	}

	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );

#if 0
	if (Terrain1)
	{
		Terrain1->Update( true );
		Terrain1->DrawBlack(gCam);
	}
#endif

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	gCollectionsManager.SetInstanceViewRefPos(gCam);
}


r3dDefferedRenderer::r3dDefferedRenderer()
: mPreCCScreenshotTarget( NULL )
{
}

r3dDefferedRenderer::~r3dDefferedRenderer()
{
}

void CreateDDRenderTargets()
{
	float rtWidth, rtHeight ;
	GetDesiredRTDimmensions( &rtWidth, &rtHeight ) ;

	R3DPROFILE_FUNCTION("CreateDDRenderTargets");
	gBuffer_PrimaryDepth	= r3dScreenBuffer::CreateClass( "gBuffer_PrimaryDepth", rtWidth, rtHeight, D3DFMT_R32F );
	gBuffer_SecondaryDepth	= r3dScreenBuffer::CreateClass( "gBuffer_SecondaryDepth", rtWidth, rtHeight, D3DFMT_R32F );
	gBuffer_HalvedR32F_1	= r3dScreenBuffer::CreateClass( "gBuffer_HalvedR32F_1", rtWidth / 2, rtHeight / 2, D3DFMT_R32F );
}

bool UseL8TextureForHZB = true;

//------------------------------------------------------------------------

StaticSkyPShaderID::StaticSkyPShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void StaticSkyPShaderID::ToString( char* str )
{
	strcpy( str, "PS_STATIC_SKY" );

	if( twoTextures )
	{
		strcat( str, "_DOUBLE" );
	}

	if( outputNormals )
	{
		strcat( str, "_NORMALS" );
	}

	if( scatterSun )
	{
		strcat( str, "_SCATTERSUN" );
	}

	if( hdr )
	{
		strcat( str, "_HDR" );
	}
}

//------------------------------------------------------------------------

void StaticSkyPShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 4 );

	defines[ 0 ].Name		= "DOUBLE";
	defines[ 0 ].Definition	= twoTextures ? "1" : "0";

	defines[ 1 ].Name		= "NORMAL_ENABLED";
	defines[ 1 ].Definition	= outputNormals ? "1" : "0";

	defines[ 2 ].Name		= "SCATTER_SUN";
	defines[ 2 ].Definition	= scatterSun ? "1" : "0";

	defines[ 3 ].Name		= "HDR";
	defines[ 3 ].Definition	= hdr ? "1" : "0";
}

//------------------------------------------------------------------------

StaticSkyPSIds gStaticSkyPSIds;

//------------------------------------------------------------------------

GrassVShaderID::GrassVShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void GrassVShaderID::ToString( char* str )
{
	strcpy( str, "VS_GRASS" );

	if( masked )
	{
		strcat( str, "_MASKED" );
	}

	if( has_normals	)
	{
		strcat( str, "_HASNORMALS" );
	}

	if( is_stiff )
	{
		strcat( str, "_ISSTIFF" );
	}
}

//------------------------------------------------------------------------

void GrassVShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 3 );

	defines[ 0 ].Name		= "IS_MASKED";
	defines[ 0 ].Definition	= DigitToCString( masked );

	defines[ 1 ].Name		= "HAS_NORMALS";
	defines[ 1 ].Definition	= DigitToCString( has_normals );

	defines[ 2 ].Name		= "IS_STIFF";
	defines[ 2 ].Definition	= DigitToCString( is_stiff );
}

//------------------------------------------------------------------------

GrassVSIds gGrassVSIds;

//------------------------------------------------------------------------

GrassPShaderID::GrassPShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void GrassPShaderID::ToString( char* str )
{
	strcpy( str, "PS_GRASS" );

	if( do_clip )
	{
		strcat( str, "_CLIP" );
	}

	if( aux )
	{
		strcat( str, "_AUX" );
	}

	if( output_depth )
	{
		strcat( str, "_OUTPUTDEPTH" );
	}
}

//------------------------------------------------------------------------

void GrassPShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 3 );

	defines[ 0 ].Name		= "DO_CLIP";
	defines[ 0 ].Definition	= DigitToCString( do_clip );

	defines[ 1 ].Name		= "AUX";
	defines[ 1 ].Definition	= DigitToCString( aux );

	defines[ 2 ].Name		= "OUTPUT_DEPTH";
	defines[ 2 ].Definition	= DigitToCString( output_depth );
}

//------------------------------------------------------------------------

GrassPSIds gGrassPSIds;

//------------------------------------------------------------------------

AccumShadowsPShaderID::AccumShadowsPShaderID()
{
	Id = 0;
}

//------------------------------------------------------------------------

void AccumShadowsPShaderID::ToString( char* str )
{
	strcpy( str, "PS_ACCUMSHADOWS" );
	strcat( str, "_LTYPE" );
	strcat( str, DigitToCString( light_type ) );

	if( fxaa_blur )
	{
		strcat( str, "_FXAABLUR" );
	}

	if( hw_shadowmaps )
	{
		strcat( str, "_HWSHADOWMAPS" );
	}

	if( recticular_warp )
	{
		strcat( str, "_RECTIWARP" );
	}

	if( use_normals )
	{
		strcat( str, "_NORMAL" );
	}

}

//------------------------------------------------------------------------

void AccumShadowsPShaderID::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 5 );

	defines[ 0 ].Name		= "LIGHT_TYPE";
	defines[ 0 ].Definition	= DigitToCString( light_type );

	defines[ 1 ].Name		= "FXAA_SHADOW_BLUR";
	defines[ 1 ].Definition	= DigitToCString( fxaa_blur );

	defines[ 2 ].Name		= "HW_SHADOW_MAPS";
	defines[ 2 ].Definition	= DigitToCString( hw_shadowmaps );

	defines[ 3 ].Name		= "RECTICULAR_WARP";
	defines[ 3 ].Definition	= DigitToCString( recticular_warp );

	defines[ 4 ].Name		= "USE_NORMALS";
	defines[ 4 ].Definition	= DigitToCString( use_normals );

}

AccumShadowsPShaderIDs gAccumShadowsPShaderIDs;

//------------------------------------------------------------------------

LensDirtShaderId::LensDirtShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void LensDirtShaderId::ToString( char* str )
{
	strcpy( str, "PS_LENS_DIRT" );

	strcat( str, "_LIGHTS" );
	strcat( str, DigitToCString( light_count ) );
}

//------------------------------------------------------------------------

void LensDirtShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 1 );

	defines[ 0 ].Name		= "NUM_LIGHTS";
	defines[ 0 ].Definition	= DigitToCString( light_count );
}

LensDirtShaderIDs gLensDirtShaderIDs;

//------------------------------------------------------------------------

void GetDesiredRTDimmensions( float* oWidth, float* oHeight )
{
	float rtWidth = r3dRenderer->ScreenW ;
	float rtHeight = r3dRenderer->ScreenH ;

	if( r_internal_width->GetInt() )
	{
		rtWidth = r_internal_width->GetFloat() ;
	}

	if( r_internal_height->GetInt() )
	{
		rtHeight = r_internal_height->GetFloat() ;
	}

	*oWidth = rtWidth ;
	*oHeight = rtHeight ;
}

void CreateDynamicEnvMapResources( int minDim )
{
	int dim = 512;

	for( ; dim >= minDim ; )
	{
		dim /= 2;
	}

	r3dOutToLog( "CreateDynamicEnvMap: creating of size %dx%dx6", dim );

	DynamicEnvMap0 = r3dScreenBuffer::CreateClass( "DynamicEnvMap0", dim, dim, D3DFMT_A16B16G16R16F, r3dScreenBuffer::Z_NO_Z, 1, 0 );
	DynamicEnvMap1 = r3dScreenBuffer::CreateClass( "DynamicEnvMap1", dim, dim, D3DFMT_A16B16G16R16F, r3dScreenBuffer::Z_NO_Z, 1, 0 );

	gBuffer_StagedRender_Color		= r3dScreenBuffer::CreateClass( "gBuffer_StagedRender_Color", dim, dim, D3DFMT_A8R8G8B8, r3dScreenBuffer::Z_OWN );	// rgb - albedo, a - gloss
	gBuffer_StagedRender_Normal		= r3dScreenBuffer::CreateClass( "gBuffer_StagedRender_Normal", dim, dim, D3DFMT_A8R8G8B8 );	// rgb - normal, a - self illum
	gBuffer_StagedRender_Depth		= r3dScreenBuffer::CreateClass( "gBuffer_StagedRender_Depth", dim, dim, D3DFMT_R32F );	// R32F - depth
	gBuffer_StagedRender_Temporary	= r3dScreenBuffer::CreateClass( "gBuffer_StagedRender_Temporary", dim, dim, D3DFMT_A16B16G16R16F );
}

void
r3dDefferedRenderer::CreateResolutionBoundResources() /*OVERRIDE*/
{
	r3dOutToLog("Creating render targets\n");

	float rtWidth ;
	float rtHeight ;

	GetDesiredRTDimmensions( &rtWidth, &rtHeight ) ;

	if( r_dynamic_envmap_probes->GetInt() )
	{
		CreateDynamicEnvMapResources( R3D_MIN( rtWidth, rtHeight ) );
	}

	gBuffer_Color			= r3dScreenBuffer::CreateClass("gBuffer_Color", rtWidth, rtHeight, D3DFMT_A8R8G8B8, r3dScreenBuffer::Z_SYSTEM );
	gBuffer_Normal 			= r3dScreenBuffer::CreateClass("gBuffer_Normal", rtWidth, rtHeight, D3DFMT_A8R8G8B8 );
	gBuffer_Depth			= r3dScreenBuffer::CreateClass("gBuffer_Depth", rtWidth, rtHeight, D3DFMT_R32F );

	Scaleform_RenderToTextureRT			= r3dScreenBuffer::CreateClass("Scaleform_RenderToTextureRT", rtWidth, rtHeight, D3DFMT_A8R8G8B8, r3dScreenBuffer::Z_NO_Z);
	TempShadowBuffer = Scaleform_RenderToTextureRT;

	gBuffer_HalvedR32F_0	= r3dScreenBuffer::CreateClass( "gBuffer_HalvedR32F_0", rtWidth / 2, rtHeight / 2, D3DFMT_R32F );

	if( r_double_depth_ssao->GetInt() )
	{
		CreateDDRenderTargets();
	}

	CreateAuxResource();

	HalfResParticleVarChangeCallback(-1, -1.0f);

	D3DFORMAT screenSmallFormat = D3DFMT_A8R8G8B8;
	D3DFORMAT highPrecisionFmt = D3DFMT_A2R10G10B10;
	//	Choose high precision format if supported
	bool supportsHighPrecisionFmt = r3dRenderer->IsRenderTargetFormatAvailable(highPrecisionFmt);
	bool stretchFmtSupport = r3dRenderer->pd3d->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT, r3dRenderer->DeviceType, gBuffer_Color->BufferFormat, highPrecisionFmt) == D3D_OK;
	if (supportsHighPrecisionFmt && stretchFmtSupport)
	{
		screenSmallFormat = highPrecisionFmt;
	}


	gBuffer_SSR.gDeviceCreated = true;

	if (r_use_ssr->GetInt())
		gBuffer_SSR.CreateBuffers();

	gScreenSmall = r3dScreenBuffer::CreateClass("gScreenSmall", rtWidth / 2, rtHeight / 2, screenSmallFormat);
	gScreenSmall2 = r3dScreenBuffer::CreateClass("gScreenSmall2", rtWidth / 2, rtHeight / 2, screenSmallFormat);

	DepthBuffer = gBuffer_Depth;

	D3DFORMAT HDR_FMT ;

	if( r_hdr_format->GetInt() == R3D_HDR_FMT_A16R16G16B16F )
	{
		HDR_FMT = D3DFMT_A16B16G16R16F ;
	}
	else
	{
		HDR_FMT = D3DFMT_A2B10G10R10 ;
	}


	ScreenBuffer = r3dScreenBuffer::CreateClass( "ScreenBuffer", rtWidth, rtHeight, HDR_FMT, r3dScreenBuffer::Z_SYSTEM ) ;
	TempScreenBuffer = r3dScreenBuffer::CreateClass( "TempScreenBuffer", rtWidth, rtHeight, HDR_FMT, r3dScreenBuffer::Z_SYSTEM );

	BlurredScreenBuffer = r3dScreenBuffer::CreateClass( "BlurredScreenBuffer", rtWidth, rtHeight, HDR_FMT, r3dScreenBuffer::Z_SYSTEM );

	int AuxBufW = int( rtWidth / 4 );
	int AuxBufH = int( rtHeight / 4 );

	// There's no need for Z Bufer for auxilary buffers
	BlurBuffer  = r3dScreenBuffer::CreateClass("BlurBuffer", AuxBufW, AuxBufH, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	TempBuffer  = r3dScreenBuffer::CreateClass("TempBuffer", AuxBufW, AuxBufH, HDR_FMT, r3dScreenBuffer::Z_NO_Z );

	One8Buffer0  = r3dScreenBuffer::CreateClass("One8Buffer0", AuxBufW / 2, AuxBufH / 2, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	One8Buffer1  = r3dScreenBuffer::CreateClass("One8Buffer1", AuxBufW / 2, AuxBufH / 2, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	One16Buffer0 = r3dScreenBuffer::CreateClass("One16Buffer0", AuxBufW / 4, AuxBufH / 4, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	One16Buffer1 = r3dScreenBuffer::CreateClass("One16Buffer1", AuxBufW / 4, AuxBufH / 4, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	One32Buffer0 = r3dScreenBuffer::CreateClass("One32Buffer0", AuxBufW / 8, AuxBufH / 8, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	One32Buffer1 = r3dScreenBuffer::CreateClass("One32Buffer1", AuxBufW / 8, AuxBufH / 8, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	One64Buffer0 = r3dScreenBuffer::CreateClass("One64Buffer0", AuxBufW / 16, AuxBufH / 16, HDR_FMT, r3dScreenBuffer::Z_NO_Z );
	One64Buffer1 = r3dScreenBuffer::CreateClass("One64Buffer1", AuxBufW / 16, AuxBufH / 16, HDR_FMT, r3dScreenBuffer::Z_NO_Z );

	gFlashbang_Multiframe = r3dScreenBuffer::CreateClass("gFlashbang_Multiframe", rtWidth / 2, rtHeight / 2, D3DFMT_R5G6B5);

	r3dSetParticlesDepthRef( DepthBuffer, gBuffer_HalvedR32F_0 );

	if ( r_lighting_quality->GetInt() > 1 )
	{
		CurrentSSAOBuffer = r3dScreenBuffer::CreateClass( "CurrSSAOBuffer", rtWidth, rtHeight, D3DFMT_A8R8G8B8 );
	}

#if R3D_ALLOW_TEMPORAL_SSAO
	{
		PrevDepthBuffer = r3dScreenBuffer::CreateClass( "PrevDepthBuffer", rtWidth, rtHeight, D3DFMT_R32F ) ;
		PrevSSAOBuffer = r3dScreenBuffer::CreateClass( "PrevSSAOBuffer", rtWidth, rtHeight, D3DFMT_A8R8G8B8 );
		CurrentSSAOBuffer = r3dScreenBuffer::CreateClass( "CurrSSAOBuffer", rtWidth, rtHeight, D3DFMT_A8R8G8B8 );
	}
#endif
}

//------------------------------------------------------------------------
/*virtual*/
void
r3dDefferedRenderer::DestroyResolutionBoundResources() /*OVERRIDE*/
{
	r3dOutToLog("Deleting render targets\n");
	SAFE_DELETE( ScreenBuffer );
	SAFE_DELETE( DistortionBuffer );
	SAFE_DELETE( TempScreenBuffer );
	SAFE_DELETE( BlurredScreenBuffer );

	SAFE_DELETE( gBuffer_Particles );

	SAFE_DELETE( gBuffer_Color );
	SAFE_DELETE( gBuffer_Normal );
	SAFE_DELETE( gBuffer_Depth );

	gBuffer_SSR.DeleteBuffers();

	SAFE_DELETE( Scaleform_RenderToTextureRT );

	SAFE_DELETE( gBuffer_PrimaryDepth	);
	SAFE_DELETE( gBuffer_SecondaryDepth );
	SAFE_DELETE( gBuffer_HalvedR32F_0	);
	SAFE_DELETE( gBuffer_HalvedR32F_1	);

	SAFE_DELETE( gBuffer_Aux );

	SAFE_DELETE( BlurBuffer );
	SAFE_DELETE( TempBuffer );
	SAFE_DELETE( gFlashbang_Multiframe );

	SAFE_DELETE(gScreenSmall);
	SAFE_DELETE(gScreenSmall2);

	SAFE_DELETE(PrevDepthBuffer);
	SAFE_DELETE(PrevSSAOBuffer);
	SAFE_DELETE(CurrentSSAOBuffer);
}

//------------------------------------------------------------------------
/*virtual*/
void
r3dDefferedRenderer::CreateShadowResources() /*OVERRIDE*/
{
	ShadowSlices[0].shadowMapSize = r_dir_sm_size->GetInt();
	ShadowSlices[1].shadowMapSize = r_dir_sm_size->GetInt();
	ShadowSlices[2].shadowMapSize = r_dir_sm_size->GetInt();

	int maxSMSize = 0 ;

	if( r_active_shadow_slices->GetInt() > 1
			||
		r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_R32F
		)
	{
		maxSMSize = 	R3D_MAX(
							R3D_MAX(
								r_shared_sm_size->GetInt(),
									r_shared_sm_cube_size->GetInt() ),
										r_dir_sm_size->GetInt() );

	}
	else
	{
		// LF shadow map will have its own depth/texture in case of HW ( need to keep it unchanged from frame to frame ),
		// and rest shadowmaps can share much smaller depth/stencil
		maxSMSize = 	R3D_MAX(
								r_shared_sm_size->GetInt(),
									r_shared_sm_cube_size->GetInt()
									);
	}


	r3dScreenBuffer::CreateShadowZBuffer( maxSMSize, maxSMSize );

	D3DFORMAT sharedShadowFormat = D3DFMT_R32F ;
	if( r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF )
	{
		sharedShadowFormat = D3DFMT_D24X8 ;
	}

	SharedShadowMap		= r3dScreenBuffer::CreateClass( "SharedShadowMap", r_shared_sm_size->GetInt(), r_shared_sm_size->GetInt(), sharedShadowFormat, r3dScreenBuffer::Z_SHADOW, 0 );

	SharedShadowCubeMap	= r3dScreenBuffer::CreateClass( "SharedShadowCubeMap", r_shared_sm_cube_size->GetInt(), r_shared_sm_cube_size->GetInt(), D3DFMT_R32F, r3dScreenBuffer::Z_SHADOW, 1 );

	const int TRANSP_SHADOWMAP_SIZE = r_transp_shadowmap_size->GetInt() ;

	if( r_shadows_quality->GetInt() > 2 )
	{
		TransparentShadowMap = r3dScreenBuffer::CreateClass( "TransparentShadowMap", TRANSP_SHADOWMAP_SIZE, TRANSP_SHADOWMAP_SIZE, D3DFMT_G32R32F, r3dScreenBuffer::Z_NO_Z ) ;
	}

	TransparentShadowSlice.shadowMapSize = TRANSP_SHADOWMAP_SIZE ;

	r3dScreenBuffer::ZType zType = r3dScreenBuffer::Z_SHADOW;
	D3DFORMAT rtFmt = D3DFMT_UNKNOWN;

	switch (r_hardware_shadow_method->GetInt())
	{
	case HW_SHADOW_METHOD_R32F:
		rtFmt = D3DFMT_R32F;
		break;
	case HW_SHADOW_METHOD_HW_PCF:
		rtFmt = D3DFMT_D24X8;
		break;
	case HW_SHADOW_METHOD_INTZ:
		rtFmt = INTZ_FORMAT;
		break;
	default:
		r3d_assert(!"Unknown shadow method");
	}

	r3dScreenBuffer::CreationParameters p;
	p.width = ShadowSlices[0].shadowMapSize;
	p.height = ShadowSlices[0].shadowMapSize;
	p.Format = rtFmt;
	p.ZT = zType;
	p.name = "SunShadowMap";


	if( r_active_shadow_slices->GetInt() == 1
			&&
		r_hardware_shadow_method->GetInt() != HW_SHADOW_METHOD_R32F
		)
	{
		p.ZT = r3dScreenBuffer::Z_OWN ;
	}


	SunShadowMap = r3dScreenBuffer::CreateClass(p);

	if( r_active_shadow_slices->GetInt() > 1 )
	{
		p.name = "LFUpdateShadowMap";
		if( r_hardware_shadow_method->GetInt() != HW_SHADOW_METHOD_R32F  )
		{
			p.ZT = r3dScreenBuffer::Z_OWN ;
		}
		LFUpdateShadowMap	= r3dScreenBuffer::CreateClass(p);
	}
	else
	{
		LFUpdateShadowMap	= SunShadowMap;
	}

}

//------------------------------------------------------------------------
void DestroySunShadowResources()
{
	bool uniqueLFUpdateShadowMap = SunShadowMap != LFUpdateShadowMap ;

	if( uniqueLFUpdateShadowMap )
	{
		SAFE_DELETE( LFUpdateShadowMap );
	}

	SAFE_DELETE( SunShadowMap );
}

//------------------------------------------------------------------------
/*virtual*/
void
r3dDefferedRenderer::DestroyShadowResources() /*OVERRIDE*/
{
	SAFE_DELETE( SharedShadowMap );
	SAFE_DELETE( SharedShadowCubeMap );
	SAFE_DELETE( TransparentShadowMap );

	r3dScreenBuffer::ReleaseShadowZBuffer();
	DestroySunShadowResources();
}

//------------------------------------------------------------------------
/*virtual*/
void r3dDefferedRenderer::CreateAuxResource()
{
	float rtWidth, rtHeight ;
	GetDesiredRTDimmensions( &rtWidth, &rtHeight ) ;

	r3d_assert(gBuffer_Aux == 0);
	if (r_lighting_quality->GetInt() == 1)
		gBuffer_Aux 		= r3dScreenBuffer::CreateEmptyClass("gBuffer_Aux");
	else
		gBuffer_Aux 		= r3dScreenBuffer::CreateClass("gBuffer_Aux", rtWidth, rtHeight, D3DFMT_A8R8G8B8 );
}


//------------------------------------------------------------------------
/*virtual*/
void r3dDefferedRenderer::DestroyAuxResource()
{
	SAFE_DELETE(gBuffer_Aux);
}

//------------------------------------------------------------------------
/*virtual*/
void
r3dDefferedRenderer::Finalize() /*OVERRIDE*/
{
	D3DPERF_BeginEvent( 0, L"Brightness & Contrast" ) ;

#if 0
	bool need_brightness = fabs( r_brightness->GetFloat() - 0.5f ) > 1.0f / 256.f;
	bool need_contrast = fabs( r_contrast->GetFloat() - 0.5f ) > 1.0f / 256.f;

	float contrast = r_contrast->GetFloat();
	float brightness = r_brightness->GetFloat();
#else
	bool need_brightness = fabs( r_gamma_pow->GetFloat() - 2.2f ) > 1.0f / 256.f;
	bool need_contrast = false;

	float contrast = 0.5f;
	float brightness = ( r_gamma_pow->GetFloat() - 2.2f ) * 0.375f + 0.5f;
#endif

#if 1
	if( need_brightness || need_contrast )
	{
		IDirect3DVertexDeclaration9* prevDecl;

		prevDecl = d3dc.pDecl;
		d3dc._SetDecl( g_ScreenSpaceQuadVertexDecl );

		r3dRenderer->SetVertexShader( VS_FWD_COLOR_ID ) ;
		r3dRenderer->SetPixelShader( PS_FWD_COLOR_ID ) ;

		r3dRenderer->SetCullMode( D3DCULL_NONE ) ;

		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH ) ;

		D3DXMATRIX ident ;
		D3DXMatrixIdentity( &ident ) ;

		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &ident._11, 4 ) ) ;

		// our original formula is " ( x - 0.5 ) * 2 * c + 0.5 + b - 0.5 "
		// which converts to  2 * ( c * x + 0.5 * ( b - c ) )
		// so we first do * c using full screen quad with modulations
		// and then + 0.5 ( b - c ) with additive blending
		// and then * 2 using x2 DEST_COLOR ONE with SRC being 1
		// we need * 2 in the end and not in the start to prevent clamping in the middle of the formula.
		// yes, this reduces bitness of our color


		// * c follows
		{
			float c = contrast ;

			float colr[ 4 ] = { c, c, c, c } ;

			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, colr, 1 ) ) ;

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ) ) ;

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO ) ) ;
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR ) ) ;

			r3dDrawFullScreenQuad( false ) ;
		}

		// 0.5 * ( b - c ) follows
		{
			float b = 0.5 * ( brightness - contrast ) ;
			float bs = fabs( b ) ;
			float colr[ 4 ] = { bs, bs, bs, bs } ;
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, colr, 1 ) ) ;


			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ) ) ;
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ) ) ;

			if( b > 0.0f )
			{
				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ) ) ;
			}
			else
			{
				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT ) ) ;
			}

			r3dDrawFullScreenQuad( false ) ;
		}

		// * 2 follows
		{
			float colr[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f } ;
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, colr, 1 ) ) ;

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR ) ) ;
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ) ) ;

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ) ) ;

			r3dDrawFullScreenQuad( false ) ;
		}

		r3dRenderer->SetVertexShader() ;
		r3dRenderer->SetPixelShader() ;

		r3dRenderer->RestoreCullMode() ;

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP ) ;

		d3dc._SetDecl( prevDecl );
	}
#endif

	D3DPERF_EndEvent() ;
}

//------------------------------------------------------------------------
/*virtual*/

void
r3dDefferedRenderer::SetPreColorCorrectionScreenshotTarget( r3dScreenBuffer* target ) /*OVERRIDE*/
{
	mPreCCScreenshotTarget = target ;
}

//------------------------------------------------------------------------

/*virtual*/ void r3dDefferedRenderer::OnDynamicObjectDeleted( GameObject* obj ) /*OVERRIDE*/
{
	for( int i = 0; i < mTempObjectArrayCount;  )
	{
		if( mTempObjectArray[ i ] == obj )
		{
			for( int ii = i; ii < mTempObjectArrayCount - 1; ii ++ )
			{
				mTempObjectArray[ ii ] = mTempObjectArray[ ii + 1 ];
			}
			mTempObjectArrayCount --;
		}
		else
			i ++;
	}
}

//------------------------------------------------------------------------

void r3dInitShaders()
{
	struct EnableDisableShaderLoading
	{
		EnableDisableShaderLoading()
		{
			r3dRenderer->SetShaderLoadingEnabled( true );
		}

		~EnableDisableShaderLoading()
		{
			r3dRenderer->SetShaderLoadingEnabled( false );
		}
	} enableDisableShaderLoading; ( void )enableDisableShaderLoading;

	r3dOutToLog("Loading shaders...\n");

	r3dRenderer->UpdateShaderProfiles();

	float LoadingStart = r3dGetTime();

	extern char __r3dBaseShaderPath[256];
	extern char __r3dBaseShaderCachePath[256];

	// Initialize shaders for current rendering path
	sprintf( __r3dBaseShaderPath, "%s", "Data\\Shaders\\DX9_P1" );
	sprintf( __r3dBaseShaderCachePath, "%s", R3D_BASE_SHADER_CACHE_PATH "\\DX9_P1" );

	// THOSE TWO SHOULD BE THE FIRST ONE!!!
	r3dRenderer->AddVertexShaderFromFile("VS_DRAW3D_SIMPLE", "Render3d_simple_vs.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_DRAW3D_SIMPLE", "Render3d_simple_ps.hls");
	// END
	r3dRenderer->AddVertexShaderFromFile("VS_DRAW3D_DEFERRED", "Render3d_deferred_vs.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_DRAW3D_DEFERRED", "Render3d_deferred_ps.hls");

	r3dRenderer->AddPixelShaderFromFile("PS_BLUR",  "Filters\\Blur.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_BLURG", "Filters\\Blur_Gaus.hls");

	r3dRenderer->AddPixelShaderFromFile("PS_BLURNEAR1", "Filters\\BlurDOF1.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_BLURNEAR2", "Filters\\BlurDOF2.hls");

	//r3dRenderer->AddPixelShaderFromFile("PS_CLEARDEPTH", "Filters\\zclear_ps.hls");

	//r3dRenderer->AddVertexShaderFromFile("VS_SPEEDSTORE", "Filters\\speed_vs.hls");
	//r3dRenderer->AddPixelShaderFromFile("PS_SPEEDSTORE", "Filters\\speed_ps.hls");

	{
		ShaderMacros macros ( 2 );

		macros[ 0 ].Name		= "AUX_ENABLED" ;
		macros[ 0 ].Definition	= "1" ;

		macros[ 1 ].Name		= "DEPTH_ONLY" ;
		macros[ 1 ].Definition	= "0" ;

		r3dRenderer->AddPixelShaderFromFile("PS_DS_CLEAR", "DS_ClearBuffer_ps.hls", 0, macros);

		macros[ 0 ].Definition	= "0" ;
		r3dRenderer->AddPixelShaderFromFile("PS_DS_CLEAR_NO_AUX", "DS_ClearBuffer_ps.hls", 0, macros);

		macros[ 1 ].Definition	= "1" ;
		r3dRenderer->AddPixelShaderFromFile("PS_DS_CLEAR_DEPTHONLY", "DS_ClearBuffer_ps.hls", 0, macros);
	}

	//!
	r3dRenderer->AddPixelShaderFromFile("PS_DS_SSR_PS", "DS_SSR_ps.hls");
	r3dRenderer->AddVertexShaderFromFile("PS_DS_SSR_VS", "DS_SSR_vs.hls");


	r3dRenderer->AddVertexShaderFromFile("VS_DS_CLEAR", "DS_ClearBuffer_vs.hls");

	for( int light_type = 0 ; light_type < SHADOWACCUM_LIGHT_TYPE_COUNT; light_type ++ )
	{
		for( int fxaa_blur = 0 ; fxaa_blur < 2; fxaa_blur ++ )
		{
			for( int hw_shadowmaps = 0 ; hw_shadowmaps < 2; hw_shadowmaps ++ )
			{
				for( int recticular_warp = 0 ; recticular_warp < 2; recticular_warp ++ )
				{
					for( int use_normals = 0 ; use_normals < 2; use_normals ++ )
					{

						AccumShadowsPShaderID psid;

						psid.light_type			= light_type;
						psid.fxaa_blur			= fxaa_blur;
						psid.hw_shadowmaps		= hw_shadowmaps;
						psid.recticular_warp	= recticular_warp;
						psid.use_normals = use_normals;


						char name[ 512 ];

						ShaderMacros macros;

						psid.FillMacros( macros );
						psid.ToString( name );

						gAccumShadowsPShaderIDs[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "DS_AccumShadows_ps.hls", 0, macros );
					}
				}
			}
		}
	}

	{
		ShaderMacros macros( 2 );

		macros[ 0 ].Name = "DYN_BRANCH" ;
		macros[ 0 ].Definition = "0" ;

		macros[ 1 ].Name = "NUM_SAMPLES" ;
		macros[ 1 ].Definition = "5" ;

		r3dRenderer->AddPixelShaderFromFile( "PS_BLUR_SHADOWS",	"DS_BlurShadows_ps.hls", 0, macros ) ;

		macros[ 0 ].Definition = "1" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_BLUR_SHADOWS_DYN",	"DS_BlurShadows_ps.hls", 0, macros ) ;

		macros[ 0 ].Definition = "0" ;
		macros[ 1 ].Definition = "9" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_BLUR_SHADOWS_HQ",	"DS_BlurShadows_ps.hls", 0, macros ) ;

	}

	// Some general effects shaders


	VS_CLEAR_FLOAT_ID = r3dRenderer->AddVertexShaderFromFile( "VS_CLEAR_FLOAT", "DS_ClearFloat_vs.hls" );
	PS_CLEAR_FLOAT_ID = r3dRenderer->AddPixelShaderFromFile( "PS_CLEAR_FLOAT", "DS_ClearFloat_ps.hls" );

	r3dRenderer->AddVertexShaderFromFile("VS_DS_DIRLIGHT", "DS_dir_vs.hls");

	{
		ShaderMacros macros ;
		char shaderName[ 512 ] ;

		for( int ssr = 0 ; ssr < 2 ; ssr ++ )
		{
			for( int sss = 0 ; sss < 2 ; sss ++ )
			{
				for( int transp_shadows = 0 ; transp_shadows < 2 ; transp_shadows ++ )
				{
					for( int white_albedo = 0 ; white_albedo < 2 ; white_albedo ++ )
					{
						for( int ambient_only = 0 ; ambient_only < 2 ; ambient_only ++ )
						{
							for( int quality = 0; quality < 2; quality ++ )
							{
								SunHDRLightPShaderId pshadId;

								pshadId.ssr				= ssr;
								pshadId.sss				= sss;
								pshadId.transp_shadows	= transp_shadows;
								pshadId.white_albedo	= white_albedo;
								pshadId.ambient_only	= ambient_only;
								pshadId.quality			= quality;

								pshadId.FillMacros( macros );
								pshadId.ToString( shaderName );

								gSunHDRLightPSIds[ pshadId.Id ] = r3dRenderer->AddPixelShaderFromFile( shaderName, "DS_dir_HDR_ps.hls", 0, macros );
							}
						}
					}
				}
			}
		}
	}

	{
		ShaderMacros macros( 0 ) ;

		r3dRenderer->AddPixelShaderFromFile( "PS_DS_FILMTONE", "DS_FilmTone_ps.hls", 0, macros ) ;
	}

	{
		ShaderMacros macros( 0 ) ;

		r3dRenderer->AddPixelShaderFromFile( "PS_COPY_OUTPUT", "DS_CopyOutput_ps.hls", 0, macros ) ;
	}

	{
		ShaderMacros macros( 0 ) ;

		r3dRenderer->AddPixelShaderFromFile( "PS_CONVERT_TO_LDR", "DS_ConvertToLDR_ps.hls", 0, macros ) ;
	}

	{
		char shaderName[ 512 ] ;

		ShaderMacros shmacros ;

		for( int ssr = 0 ; ssr < 2 ; ssr ++ )
		{
			for( int sss = 0 ; sss < 2 ; sss ++ )
			{
				for( int diffuse_only = 0 ; diffuse_only < 2 ; diffuse_only ++ )
				{
					for( int ssao = 0 ; ssao < 2 ; ssao ++ )
					{
						for( int proj_texture = 0 ; proj_texture < 2 ; proj_texture ++ )
						{
							for( int shadow_mode = 0 ; shadow_mode < 5 ; shadow_mode ++ )
							{
								for( int quality = 0; quality < 2; quality ++ )
								{
									PointLightPShaderId psid ;

									psid.sss			= sss ;
									psid.ssr			= ssr ;
									psid.diffuse_only	= diffuse_only ;
									psid.ssao			= ssao ;
									psid.proj_texture	= proj_texture ;
									psid.shadow_mode	= shadow_mode ;
									psid.quality		= quality;

									psid.ToString( shaderName ) ;
									psid.FillMacros( shmacros ) ;

									gPointLightPSIds[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( shaderName, "DS_Point_HDR_ps.hls", 0, shmacros ) ;
								}
							}
						}
					}
				}
			}
		}
	}

	{
		char shaderName[ 512 ] ;

		ShaderMacros shmacros ;

		for( int aux_enabled = 0; aux_enabled < 2; aux_enabled ++ )
		{
			for( int shadow_mode = 0; shadow_mode < 4; shadow_mode ++ )
			{
				for( int proj_tex = 0; proj_tex < 2; proj_tex ++ )
				{
					for( int quality = 0; quality < 2; quality ++ )
					{
						SpotLightPShaderId psid ;

						psid.aux_enabled = aux_enabled;
						psid.shadow_mode = shadow_mode;
						psid.proj_texture = proj_tex;
						psid.quality = quality;

						psid.ToString( shaderName );
						psid.FillMacros( shmacros );

						gSpotLightPSIds[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( shaderName, "DS_Spot_HDR_ps.hls", 0, shmacros) ;
					}
				}
			}
		}
	}

	{
		ShaderMacros macros( 1 ) ;
		macros[ 0 ].Name = "SHOW_NORMALS" ;
		macros[ 0 ].Definition = "1";
		r3dRenderer->AddPixelShaderFromFile( "PS_DS_SCENE_DEBUG_VISUALIZER_NORMALS", "DS_Scene_Debug_Visualizer_ps.hls", 0, macros ) ;

		macros[ 0 ].Name = "SHOW_DEPTH" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_DS_SCENE_DEBUG_VISUALIZER_DEPTH", "DS_Scene_Debug_Visualizer_ps.hls", 0, macros ) ;

		macros[ 0 ].Name = "SHOW_AUX" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_DS_SCENE_DEBUG_VISUALIZER_AUX", "DS_Scene_Debug_Visualizer_ps.hls", 0, macros ) ;

		macros[ 0 ].Name = "SHOW_DIFFUSE" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_DS_SCENE_DEBUG_VISUALIZER_DIFFUSE", "DS_Scene_Debug_Visualizer_ps.hls", 0, macros ) ;

		macros[ 0 ].Name = "SHOW_REFLECTIONS" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_DS_SCENE_DEBUG_VISUALIZER_REFLECTIONS", "DS_Scene_Debug_Visualizer_ps.hls", 0, macros ) ;


	}

	{
		ShaderMacros macros( 1 ) ;

		macros[ 0 ].Name = "HQ" ;
		macros[ 0 ].Definition = "0" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_DS_AMBIENT", "DS_AmbientHDR_ps.hls", 0, macros ) ;


		macros[ 0 ].Definition = "1" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_DS_AMBIENT_HQ", "DS_AmbientHDR_ps.hls", 0, macros ) ;
	}

	{
		DS_TUBELIGHT_ID = r3dRenderer->AddPixelShaderFromFile("PS_DS_TUBELIGHT", "DS_TubeLight_ps.hls");
		{
			ShaderMacros macros( 1 );

			macros[ 0 ].Name = "SHADOW_MAP" ;
			macros[ 0 ].Definition = "";

			DS_TUBELIGHT_SM_ID = r3dRenderer->AddPixelShaderFromFile( "PS_DS_TUBELIGHT_SM", "DS_TubeLight_ps.hls", 0, macros );

			macros[ 0 ].Name = "SS_SHADOW_MAP" ;

			DS_TUBELIGHT_SSM_ID = r3dRenderer->AddPixelShaderFromFile( "PS_DS_TUBELIGHT_SSM", "DS_TubeLight_ps.hls", 0, macros );
		}

		DS_PLANELIGHT_ID = r3dRenderer->AddPixelShaderFromFile("PS_DS_PLANELIGHT", "DS_PlaneLight_ps.hls");

		{
			ShaderMacros macros( 1 );

			macros[ 0 ].Name = "SHADOW_MAP" ;
			macros[ 0 ].Definition = "";

			DS_PLANELIGHT_SM_ID = r3dRenderer->AddPixelShaderFromFile( "PS_DS_PLANELIGHT_SM", "DS_PlaneLight_ps.hls", 0, macros );

			macros[ 0 ].Name = "SS_SHADOW_MAP" ;

			DS_PLANELIGHT_SSM_ID = r3dRenderer->AddPixelShaderFromFile( "PS_DS_PLANELIGHT_SSM", "DS_PlaneLight_ps.hls", 0, macros );
		}
	}

	r3dRenderer->AddVertexShaderFromFile("VS_DS_POINTLIGHT", "DS_point_vs.hls");

	VS_SPOT_ID			= r3dRenderer->AddVertexShaderFromFile( "VS_DS_SPOTLIGHT", "DS_spot_vs.hls" );

	// Compositing effects
	//r3dRenderer->AddPixelShaderFromFile("PS_GAUSSBLUR", "Filter_GBlur.hls");

	r3dRenderer->AddPixelShaderFromFile( "PS_RADIALBLUR", "radialblur_modulate_ps.hls" );
	r3dRenderer->AddVertexShaderFromFile( "VS_RADIALBLUR", "radialblur_modulate_vs.hls" );

	{
		r3dRenderer->AddPixelShaderFromFile( "PS_CC_LUT1D_RGB", "cc_lut_1d_rgb.hls" );
		r3dRenderer->AddPixelShaderFromFile( "PS_CC_LUT1D_HSV", "cc_lut_1d_hsv.hls" );
	}

	{
		ShaderMacros macros( 1 );
		macros[0].Name = "LUT_COUNT";

		macros[0].Definition = "1";
		r3dRenderer->AddPixelShaderFromFile( "PS_CC_LUT3D_1", "cc_lut_3d.hls", 0, macros );

		macros[0].Definition = "2";
		r3dRenderer->AddPixelShaderFromFile( "PS_CC_LUT3D_2", "cc_lut_3d.hls", 0, macros );

		macros[0].Definition = "3";
		r3dRenderer->AddPixelShaderFromFile( "PS_CC_LUT3D_3", "cc_lut_3d.hls", 0, macros );
	}

	{
		ShaderMacros macros(1);
		macros[0].Name = "GEOMETRY_TRANSFORM";
		macros[0].Definition = "0";
		r3dRenderer->AddVertexShaderFromFile( "VS_POSTFX", "postfx_vs.hls" );
		macros[0].Definition = "1";
		VS_POSTFX_GEOM_TRANSFORM = r3dRenderer->AddVertexShaderFromFile( "VS_POSTFX_GEOM_TRANSFORM", "postfx_vs.hls", 0, macros );

	}
	r3dRenderer->AddPixelShaderFromFile( "PS_FSCOPY", "fscopy_ps.hls" );

	{
		r3dRenderer->AddVertexShaderFromFile( "VS_SSAO", "SSAO_VS.hls" );
		r3dRenderer->AddPixelShaderFromFile( "PS_SSAO_REF", "SSAO_REF_PS.hls" );
	}

	r3dRenderer->AddPixelShaderFromFile( "PS_MIN_EXPAND", "MinExpand_ps.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_SSAO_COMPOSITE", "ssao_composite_ps.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_SSAO_MASK_DILUTE", "ssao_mask_dilute_ps.hls" );

	// alt SSAO
	{
		for( int num_rays = 0 ; num_rays < 2 ; num_rays ++ )
		{
			for( int optimized = 0 ; optimized < 2 ; optimized ++ )
			{
				for( int detail_radius = 0 ; detail_radius < 2 ; detail_radius ++ )
				{
#if R3D_ALLOW_TEMPORAL_SSAO
					for( int temporal_optimisation = 0 ; temporal_optimisation < 2 ; temporal_optimisation ++ )
					{
						for( int temporal_show_passed = 0 ; temporal_show_passed < 2 ; temporal_show_passed	++ )
						{
							for( int output_stability_mask = 0 ; output_stability_mask < 2 ; output_stability_mask ++ )
							{
#endif
								SSAOPShaderId shid ;

								shid.num_rays				= num_rays ;
								shid.optimized				= optimized ;
								shid.detail_radius			= detail_radius ;

#if R3D_ALLOW_TEMPORAL_SSAO
								shid.temporal_optimisation	= temporal_optimisation ;
								shid.temporal_show_passed	= temporal_show_passed ;
								shid.output_stability_mask	= output_stability_mask ;

								if( temporal_show_passed )
								{
									if( temporal_optimisation
											||
										!temporal_optimisation  && output_stability_mask
										)
									{
										SSAOPShaderId srcShid = shid ;
										srcShid.temporal_optimisation = 0 ;
										srcShid.output_stability_mask = 0 ;

										gSSAOPSIds[ shid.Id ] = gSSAOPSIds[ srcShid.Id ] ;

										continue ;
									}
								}
#endif

								ShaderMacros macros ;
								char name[ 512 ] ;

								shid.FillMacros( macros ) ;
								shid.ToString( name ) ;

								gSSAOPSIds[ shid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, shid.GetShaderFile(), 0, macros ) ;
#if R3D_ALLOW_TEMPORAL_SSAO
							}
						}
					}
#endif
				}
			}
		}
	}


	{
		for( int gauss = 0 ; gauss < 2 ; gauss ++ )
		{

			for( int normals = 0 ; normals < 2 ; normals ++ )
			{
				for( int i = SSAOConstraints::MIN_BLUR_TAP_COUNT, e = SSAOConstraints::MAX_BLUR_TAP_COUNT; i <= e; i ++ )
				{
					char buf[ 32 ];
					ShaderMacros macros;

					GetSSAOBlurPSName( buf, i, 0, normals, gauss );
					FillSSAOBlurMacros( macros, i, 0, normals, gauss );

					r3dRenderer->AddPixelShaderFromFile( buf, "SSAO_Blur.hls", 0, macros );

					GetSSAOBlurPSName( buf, i, 1, normals, gauss );
					FillSSAOBlurMacros( macros, i, 1, normals, gauss );

					r3dRenderer->AddPixelShaderFromFile( buf, "SSAO_Blur.hls", 0, macros );
				}
			}
		}

		r3dRenderer->AddPixelShaderFromFile("PS_SSAO_OUT", "SSAO_Out_ps.hls");
	}

	r3dRenderer->AddPixelShaderFromFile("PS_FILM_GRAIN", "FilmGrain_ps.hls");

	r3dRenderer->AddPixelShaderFromFile("PS_CREATEBLOOM", "CreateBloom_HDR_ps.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_CREATEGLOW", "CreateGlow_ps.hls");

	//r3dRenderer->AddPixelShaderFromFile("PS_DEPTH_BLUR_GAUSSIAN_H", "depthblurH_ps.hls");
	//r3dRenderer->AddPixelShaderFromFile("PS_DEPTH_BLUR_GAUSSIAN_V", "depthblurV_ps.hls");

	/*{
		ShaderMacros macros( 1 );
		macros[0].Name = "NUM_ITER";
		macros[0].Definition = "1";
		r3dRenderer->AddPixelShaderFromFile("PS_GAUSSIAN_BLUR", "gaussian_blur_ps.hls", 0, macros);
		macros[0].Definition = "4";
		r3dRenderer->AddPixelShaderFromFile("PS_GAUSSIAN_BLUR4", "gaussian_blur_ps.hls", 0, macros);
		macros[0].Definition = "6";
		r3dRenderer->AddPixelShaderFromFile("PS_GAUSSIAN_BLUR6", "gaussian_blur_ps.hls", 0, macros);

	}*/


	{
		ShaderMacros macros( 1 );

		macros[0].Name				= "USE_PCF";

		macros[0].Definition		= "0";
		PS_MINIMUM_ID				= r3dRenderer->AddPixelShaderFromFile( "PS_MINIMUM", "Minimum_PS.hls", 0, macros );

		macros[0].Definition		= "1";
		PS_PCF_MINIMUM_ID			= r3dRenderer->AddPixelShaderFromFile( "PS_MINIMUM_PCF", "Minimum_PS.hls", 0, macros );
	}

	PS_MINIMUM_MASK_ID				= r3dRenderer->AddPixelShaderFromFile( "PS_MINIMUM_MASK", "Minimum_Mask_PS.hls" );

	{
		ShaderMacros macros( 1 );

		macros[0].Name				= "NUM_TAPS";
		macros[0].Definition		= "7";

		PS_ADAPTIVEDEPTHBLUR7_ID	= r3dRenderer->AddPixelShaderFromFile( "PS_ADAPTIVEDEPTHBLUR7", "AdaptiveDepthBlur_ps.hls", 0, macros );
	}

	r3dRenderer->AddPixelShaderFromFile("PS_BLUR_GAUSSIAN_H", "BlurGaussianH_ps.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_BLUR_GAUSSIAN_V", "BlurGaussianV_ps.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_OBJECT_MOTION_BLUR", "ObjectMotionBlur_ps.hls");

	r3dRenderer->AddVertexShaderFromFile("VS_DS_CAMERA_MOTION", "DS_CameraMotionBlur_vs.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_DS_CAMERA_MOTION", "DS_CameraMotionBlur_ps.hls");

	{
		r3dRenderer->AddPixelShaderFromFile("PS_DOF_NEAR_MASK", "DOF_near_ps.hls");
	}

	r3dRenderer->AddPixelShaderFromFile("PS_COPY", "copy_ps.hls");

	r3dRenderer->AddPixelShaderFromFile("PS_DISTORT_SCREEN", "DistortScreen_ps.hls");

	r3dRenderer->AddPixelShaderFromFile("PS_SHOW_SHADOWBUFFER", "ShowShadowBuffer.hls");

	{
		ShaderMacros macros;

		macros.Resize( 1 );

		macros[ 0 ].Name			= "EXTRUDE";

		macros[ 0 ].Definition		= "0";
		VS_FILLGBUFFER_ID			= r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER", "DS_fillbuffer_vs.hls", 0, macros );

		macros[ 0 ].Definition		= "1";
		VS_FILLGBUFFER_EXTRUDE_ID	= r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER_EXTRUDE", "DS_fillbuffer_vs.hls", 0, macros);
	}

	extern int _r3d_Mesh_LoadSecondUV;
	if(_r3d_Mesh_LoadSecondUV)
	{
		ShaderMacros macros;

		macros.Resize( 2 );

		macros[ 0 ].Name			= "EXTRUDE";
		macros[ 1 ].Name			= "HAVE_SECOND_UV";

		macros[ 0 ].Definition		= "0";
		macros[ 1 ].Definition		= "1";
		VS_FILLGBUFFER_2UV_ID			= r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER_2UV", "DS_fillbuffer_vs.hls", 0, macros );
	}


	VS_DEPTH_PREPASS_ID = r3dRenderer->AddVertexShaderFromFile("VS_DEPTH_PREPASS", "DS_DepthPrepass_VS.hls");

	VS_FILLGBUFFER_DISP_ID = r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER_DISP", "DS_fillbuffer_disp_vs.hls");

	VS_FILLGBUFFER_APEX_ID = r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER_APEX", "DS_fillbuffer_apex_vs.hls");

	for( int i = 0, e = FillbufferPixelShadersArr::COUNT ; i < e ; i ++ )
	{
		gFillbufferPixelShaders[ i ] = -1 ;
	}

	LoadFillBufferShaders(false);

#ifndef FINAL_BUILD
	r3dRenderer->AddPixelShaderFromFile("PS_FILLGBUFFER_TEXEL_DENSITY", "DS_Fillbuffer_TexelDensity_ps.hls");
	r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER_TEXEL_DENSITY", "DS_Fillbuffer_TexelDensity_vs.hls");
#endif

	VS_FILLGBUFFER_INSTANCED_ID = r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER_INSTANCED", "DS_Fillbuffer_Instanced_VS.hls");

	if(_r3d_Mesh_LoadSecondUV)
	{
		ShaderMacros macros;

		macros.Resize( 1 );
		macros[ 0 ].Name			= "HAVE_SECOND_UV";
		macros[ 0 ].Definition		= "1";

		VS_FILLGBUFFER_INSTANCED_2UV_ID = r3dRenderer->AddVertexShaderFromFile("VS_FILLGBUFFER_INSTANCED_2UV", "DS_Fillbuffer_Instanced_VS.hls", 0, macros );
	}

	//r3dRenderer->AddVertexShaderFromFile("VS_COMPOSITE", "composite_vs.hls");
	//r3dRenderer->AddPixelShaderFromFile( "PS_COMPOSITE", "composite_ps.hls" );

	{
		for( int noninstanced = 0; noninstanced < 2; noninstanced ++ )
		{
			for( int animated = 0; animated < 2; animated ++ )
			{
				for( int shadows = 0; shadows < 2; shadows ++ )
				{
					for( int shadow_type = 0; shadow_type < SMVSTYPE_COUNT; shadow_type ++ )
					{
						ForestVShaderID psid;

						psid.noninstanced	= noninstanced;
						psid.animated		= animated;
						psid.shadows		= shadows;
						psid.shadow_type	= shadow_type;

						char name[ 512 ];
						ShaderMacros macros;

						psid.FillMacros( macros );
						psid.ToString( name );

						gForestVSIds[ psid.Id ] = r3dRenderer->AddVertexShaderFromFile( name, "DS_forest_vs.hls", 0, macros );
					}
				}
			}
		}


// 		r3dRenderer->AddVertexShaderFromFile("VS_FOREST_SM", "SM_Forest_VS.hls");
//
// 		r3dRenderer->AddPixelShaderFromFile("PS_FOREST", "DS_forest_ps.hls");
//
// 		r3dRenderer->AddVertexShaderFromFile("VS_FOREST_BILLBOARDS", "DS_forest_billboards_vs.hls");
// 		r3dRenderer->AddPixelShaderFromFile("PS_FOREST_BILLBOARDS", "DS_forest_billboards_ps.hls");

	}


	// skin shaders
	{
		VS_SKIN_ID  = r3dRenderer->AddVertexShaderFromFile("VS_SKIN", "Skin_VS.hls");

		for( int type = 0; type < SMVSTYPE_COUNT; type ++ )
		{
			SkinSMDepthVShaderID vsid;

			vsid.type = type;

			char name[ 512 ];

			ShaderMacros macros;

			vsid.FillMacros( macros );
			vsid.ToString( name );

			gSkinSMDepthVSIds[ vsid.Id ] = r3dRenderer->AddVertexShaderFromFile( name , "SkinSM_VS.hls", 0, macros );
		}
	}

	VS_SKIN_DEPTH_ID	= r3dRenderer->AddVertexShaderFromFile( "VS_SKIN_DEPTH", "SkinDepth_VS.hls" ) ;
	VS_DEPTH_ID			= r3dRenderer->AddVertexShaderFromFile( "VS_DEPTH", "Depth_VS.hls" ) ;
	PS_DEPTH_ID			= r3dRenderer->AddPixelShaderFromFile( "PS_DEPTH", "Depth_ps.hls" ) ;

	{
		ShaderMacros macros( 1 );
		macros[ 0 ].Name	= "WATER_PLANE" ;
		macros[ 0 ].Definition	= "0" ;

		r3dRenderer->AddPixelShaderFromFile("PS_ZDRAW", "zdraw_ps.hls", 0, macros);
		r3dRenderer->AddVertexShaderFromFile("VS_ZDRAW", "zdraw_vs.hls", 0, macros);

		macros[ 0 ].Definition	= "1" ;

		r3dRenderer->AddPixelShaderFromFile("PS_ZDRAW_WATER_PLANE", "zdraw_ps.hls", 0, macros);
		r3dRenderer->AddVertexShaderFromFile("VS_ZDRAW_WATER_PLANE", "zdraw_vs.hls", 0, macros);
	}

	r3dRenderer->AddPixelShaderFromFile("PS_ZDRAW_FULL", "zdraw_full_ps.hls");
	r3dRenderer->AddVertexShaderFromFile("VS_ZDRAW_FULL", "zdraw_full_vs.hls");

	r3dRenderer->AddPixelShaderFromFile("PS_VOLUMEFOG", "Volume_FogE_ps.hls");
	r3dRenderer->AddVertexShaderFromFile("VS_VOLUMEFOG", "Volume_FogE_vs.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_VOLUMEFOGNOISE", "Volume_FogNoise_ps.hls");

	char vsName0[] = "VS_TRANSPARENT_ID_00" ;
	char vsName1[] = "VS_SKEL_TRANSPARENT_ID_00" ;
	char *vsName[2] = { vsName0, vsName1 };

	for( int j = 0; j < 2 ; j ++ )
	{
		for (int i = 0; i < MAX_LIGHTS_FOR_TRANSPARENT; ++i)
		{
			char buf[2] = {0};
			sprintf_s(buf, _countof(buf), "%i", i);

			ShaderMacros macros(2);

			macros[0].Name = "BONES";
			macros[0].Definition = j ? "1" : "0";

			macros[1].Name = "NUM_POINT_LIGHTS";
			macros[1].Definition = buf;

			vsName[j][strlen(vsName[j]) - 1] = buf[0];

			VS_TRANSPARENT_ID[j][i] = r3dRenderer->AddVertexShaderFromFile(vsName[j], "Transparent_vs.hls", 0, macros);
		}
	}

	{
		ShaderMacros macros( 3 );

		macros[ 0 ].Name		= "AURA_EFFECT" ;
		macros[ 0 ].Definition	= "0" ;
		macros[ 1 ].Name		= "TRANSPARENT_CAMO" ;
		macros[ 1 ].Definition	= "0" ;
		macros[ 2 ].Name		= "FIRST_PERSON" ;
		macros[ 2 ].Definition	= "0" ;


		PS_TRANSPARENT_ID		= r3dRenderer->AddPixelShaderFromFile("PS_TRANSPARENT_ID", "Transparent_ps.hls", 0, macros );

		macros[ 1 ].Definition	= "1" ;
		PS_TRANSPARENT_CAMOUFLAGE_ID = r3dRenderer->AddPixelShaderFromFile("PS_TRANSPARENT_CAMOUFLAGE_ID", "Transparent_ps.hls", 0, macros );

		macros[ 2 ].Definition	= "1" ;
		PS_TRANSPARENT_CAMOUFLAGE_FP_ID = r3dRenderer->AddPixelShaderFromFile("PS_TRANSPARENT_CAMOUFLAGE_FP_ID", "Transparent_ps.hls", 0, macros );
		macros[ 2 ].Definition	= "0" ;


		macros[ 1 ].Definition	= "0" ;
		macros[ 0 ].Definition	= "1" ;

		PS_TRANSPARENT_AURA_ID	= r3dRenderer->AddPixelShaderFromFile("PS_TRANSPARENT_AURA_ID", "Transparent_ps.hls", 0, macros );


	}

	// shadow passes
	{
		ShaderMacros macros( 4 );

		macros[ 0 ].Name		= "ORTHO";
		macros[ 0 ].Definition	= "0";
		macros[ 1 ].Name		= "INSTANCED";
		macros[ 1 ].Definition	= "0";
		macros[ 2 ].Name		= "APEX";
		macros[ 2 ].Definition	= "0";
		macros[ 3 ].Name		= "PARABOLOID";
		macros[ 3 ].Definition	= "0";

		for( int apex = 0; apex < 2; apex ++ )
		{
			for( int instanced = 0; instanced < 2; instanced ++ )
			{
				for( int type = 0, e = SMVSTYPE_COUNT; type < e; type ++ )
				{
					SMDepthVShaderID vsid;

					vsid.apex		= apex;
					vsid.intanced	= instanced;
					vsid.type		= type;

					ShaderMacros macros;

					vsid.FillMacros( macros );

					char name[ 512 ];
					vsid.ToString( name );

					gSMDepthVSIds[ vsid.Id ] = r3dRenderer->AddVertexShaderFromFile( name, "smdepth_vs.hls", 0, macros );
				}
			}
		}
	}

	for( int type = 0; type < SMPSTYPE_COUNT; type ++ )
	{
		SMDepthPShaderID psid;

		psid.type = type;

		ShaderMacros defines;
		char name[ 512 ];

		psid.FillMacros( defines );
		psid.ToString( name );

		gSMDepthPSIds[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "smdepth_ps.hls", 0, defines );
	}

	// SFX shaders and 2D image manipulators
	//r3dRenderer->AddPixelShaderFromFile( "PS_GLARE", "glare.hls" );

	//Post filters / effects
	r3dRenderer->AddPixelShaderFromFile("POST_GODRAYS", "Post_GodRays.hls");

	//r3dRenderer->AddPixelShaderFromFile("POST_AA", "Post_AA.hls");
	//r3dRenderer->AddPixelShaderFromFile("POST_SETBLOOM", "Post_SetBloom.hls");
	//r3dRenderer->AddPixelShaderFromFile("POST_SUNGLARE", "Post_SunGlare.hls");


	r3dRenderer->AddVertexShaderFromFile("VS_POST_QUAD", "Post\\Post_Quad_VS.hls");
	r3dRenderer->AddPixelShaderFromFile("POST_TERRAINBOUNDS", "Post\\Post_TerrainBounds.hls");
	r3dRenderer->AddPixelShaderFromFile("POST_TERRAINMAT", "Post\\Post_TerrainMat.hls");

	//------------------------------------------------------------------------
	// particle vertex shaders
	{
		ShaderMacros macros;
		macros.Resize( 2 );

		macros[ 0 ].Name		= "USE_NORMALMAP";
		macros[ 0 ].Definition	= "0";

		macros[ 1 ].Name		= "SHADOW_MODE";
		macros[ 1 ].Definition	= "0";

		r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE_GPU", "ParticleGPU_vs.hls", 0, macros);

		macros[ 1 ].Name		= "SHADOW_MODE";
		macros[ 1 ].Definition	= "1";

		r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE_GPU_SHADOW", "ParticleGPU_vs.hls", 0, macros);

		macros[ 1 ].Name		= "SHADOW_MODE";
		macros[ 1 ].Definition	= "2";

		r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE_GPU_FILLSM", "ParticleGPU_vs.hls", 0, macros);

		macros[ 0 ].Name		= "USE_NORMALMAP";
		macros[ 0 ].Definition	= "1";

		macros[ 1 ].Name		= "SHADOW_MODE";
		macros[ 1 ].Definition	= "0";

		r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE_GPU_W_BUMP", "ParticleGPU_vs.hls", 0, macros);

		macros[ 1 ].Name		= "SHADOW_MODE";
		macros[ 1 ].Definition	= "1";

		r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE_GPU_W_BUMP_SHADOW", "ParticleGPU_vs.hls", 0, macros);
	}

	//------------------------------------------------------------------------
	// particle pixel shaders
	{
		ShaderMacros macros ;

		for( int normal = 0 ; normal < 2 ; normal ++ )
		{
			for( int distort_buf = 0 ; distort_buf < 2 ; distort_buf ++ )
			{
				for( int distort_tex = 0 ; distort_tex < 2 ; distort_tex ++ )
				{
					for( int half_res = 0 ; half_res < 2 ; half_res ++ )
					{
						for( int shadow_type = 0 ; shadow_type < 3 ; shadow_type ++ )
						{
							ParticlePShaderId id ;

							id.normal = normal ;
							id.distort_buf = distort_buf ;
							id.distort_tex = distort_tex ;
							id.half_res = half_res ;
							id.shadow_type = shadow_type ;

							char name[ 1024 ] ;

							id.ToString( name ) ;
							id.FillMacros( macros ) ;

							gParticlePSIds[ id.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Particle_ps.hls", 0, macros ) ;
						}
					}
				}
			}
		}
	}

	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE_FILLSM", "ParticleSM_ps.hls" );

	ShaderMacros macros;
	macros.Resize( 3 );
	macros[ 0 ].Name		= "USE_NORMALMAP";
	macros[ 0 ].Definition	= "0";

	macros[ 1 ].Name		= "USE_DISTORT";
	macros[ 1 ].Definition	= "0";

	macros[ 2 ].Name		= "HALF_RES_PARTICLES";
	macros[ 2 ].Definition	= "0";

	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1_NODISTORT", "Particle_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1_NODISTORT_HR", "Particle_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "0";

	macros[ 1 ].Definition	= "1";

	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1", "Particle_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1_HR", "Particle_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "0";

	r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE_CPU", "ParticleCPU_vs.hls", 0, macros);

	macros[ 0 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1_W_BUMP", "Particle_ps.hls", 0, macros );
	macros[ 2 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1_W_BUMP_HR", "Particle_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "0";

	r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE_CPU_W_BUMP", "ParticleCPU_vs.hls", 0, macros);

	r3dRenderer->AddVertexShaderFromFile("VS_PARTICLE2", "ParticleMesh_vs.hls");

	macros[ 1 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE2", "ParticleMesh_ps.hls", 0, macros );
	macros[ 2 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE2_HR", "ParticleMesh_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "0";

	macros[ 1 ].Definition	= "0";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE2_NODISTORT", "ParticleMesh_ps.hls", 0, macros );
	macros[ 2 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE2_NODISTORT_HR", "ParticleMesh_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "0";

	macros[ 1 ].Definition	= "0";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1_W_BUMP_NODISTORT", "Particle_ps.hls", 0, macros );
	macros[ 2 ].Definition	= "1";
	r3dRenderer->AddPixelShaderFromFile("PS_PARTICLE1_W_BUMP_NODISTORT_HR", "Particle_ps.hls", 0, macros);
	macros[ 2 ].Definition	= "0";


	r3dRenderer->AddVertexShaderFromFile("VS_PHYSICS_MESH", "PhysicsMesh_vs.hls");
	r3dRenderer->AddPixelShaderFromFile("PS_PHYSICS_MESH", "PhysicsMesh_ps.hls");

	//------------------------------------------------------------------------
	// decals

	{
		ShaderMacros macros( 1 );

		macros[ 0 ].Name		= "DO_RELIEF";

		macros[ 0 ].Definition	= "0";
		r3dRenderer->AddPixelShaderFromFile( "PS_DECAL", "Decal_ps.hls", 0, macros );

		macros[ 0 ].Definition	= "1";
		r3dRenderer->AddPixelShaderFromFile( "PS_RELIEF_DECAL", "Decal_ps.hls", 0, macros );
	}

	r3dRenderer->AddVertexShaderFromFile( "VS_DECAL", "Decal_vs.hls" );

	//------------------------------------------------------------------------
	// r3d util

	PS_FWD_COLOR_ID = r3dRenderer->AddPixelShaderFromFile( "PS_FWD_COLOR", "fwd_color_ps.hls" );
	VS_FWD_COLOR_ID = r3dRenderer->AddVertexShaderFromFile( "VS_FWD_COLOR", "fwd_color_vs.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_FWD_DEBUG_COLOR", "fwd_debug_ps.hls" );
	r3dRenderer->AddVertexShaderFromFile( "VS_FWD_DEBUG_COLOR", "fwd_debug_vs.hls" );

	//------------------------------------------------------------------------
	// occlusion system

	r3dRenderer->AddVertexShaderFromFile( "VS_OCCLUSION_QUERY", "System/Occlusion_Query_vs.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_OCCLUSION_QUERY", "System/Occlusion_Query_ps.hls" );

	//------------------------------------------------------------------------
	// post fx system

	r3dRenderer->AddVertexShaderFromFile( "VS_POSTFX_DEFAULT",	"postfx_default_vs.hls"				);
	{
		ShaderMacros macros(1);
		macros[0].Name = "GEOMETRY_TRANSFORM";
		macros[0].Definition = "0";
		r3dRenderer->AddVertexShaderFromFile( "VS_POSTFX_RECONST_W","postfx_reconstructw_vs.hls"		);
		macros[0].Definition = "1";
		r3dRenderer->AddVertexShaderFromFile( "VS_POSTFX_RECONST_W_GEOM_TRANSFORM","postfx_reconstructw_vs.hls", 0, macros );
	}

	{
		ShaderMacros macros( 1 );
		macros[0].Name = "USE_LOCAL";
		macros[0].Definition = "0";
		r3dRenderer->AddPixelShaderFromFile( "PS_CC_LUT3D", "cc_lut_3d.hls", 0, macros );
	}

	{
		ShaderMacros macros( 1 );
		macros[0].Name = "USE_LOCAL";
		macros[0].Definition = "1";
		r3dRenderer->AddPixelShaderFromFile( "PS_CC_LUT3D_WITH_LOCAL", "cc_lut_3d.hls", 0, macros );
	}

	r3dRenderer->AddPixelShaderFromFile( "PS_BLACK_WHITE", "cc_black_white.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_COPY", "copy_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_FILL", "fill_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_FILM_GRAIN", "FilmGrain_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_RADIALBLUR", "radialblur_modulate_ps.hls" );
	r3dRenderer->AddVertexShaderFromFile( "VS_RADIALBLUR", "radialblur_modulate_vs.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_CREATEGLOW", "CreateGlow_ps.hls" );

	r3dRenderer->AddPixelShaderFromFile( "POST_GODRAYS", "Post_GodRays.hls" );
	r3dRenderer->AddPixelShaderFromFile( "POST_STARSHAPEBLUR", "Post_StarShapeBlur.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_OBJECT_MOTION_BLUR", "ObjectMotionBlur_ps.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_DS_CAMERA_MOTION", "DS_CameraMotionBlur_ps.hls" );
	r3dRenderer->AddVertexShaderFromFile("VS_DS_CAMERA_MOTION", "DS_CameraMotionBlur_vs.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_MLAA_DISCONT_MAP", "MLAA_DiscontMap_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_MLAA_LINE_DETECT_H", "MLAA_LinesDetectH_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_MLAA_LINE_DETECT_V", "MLAA_LinesDetectV_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_MLAA_ALPHA_CALC", "MLAA_AlphaCalc_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_MLAA_BLEND", "MLAA_Blend_ps.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_DOF_EXTRACT_NEAR", "DOFExtractNear_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_COMBINE", "combine_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_GAMMA_CORRECT", "GammaCorrect_ps.hls" );


	{
		ShaderMacros macros( 2 ) ;

		macros[ 0 ].Name = "SHADOW_TYPE" ;
		macros[ 1 ].Name = "NO_LIGHTING" ;

		macros[ 0 ].Definition = "0" ;
		macros[ 1 ].Definition = "0" ;

		r3dRenderer->AddPixelShaderFromFile( "PS_SCOPE_FILTER_NO_SHADOWS", "ScopeFilter_ps.hls", 0, macros );

		macros[ 0 ].Definition = "1" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_SCOPE_FILTER_NORMAL_SHADOWS", "ScopeFilter_ps.hls", 0, macros );

		macros[ 0 ].Definition = "2" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_SCOPE_FILTER_HW_SHADOWS", "ScopeFilter_ps.hls", 0, macros );

		macros[ 0 ].Definition = "0" ;
		macros[ 1 ].Definition = "1" ;
		r3dRenderer->AddPixelShaderFromFile( "PS_SCOPE_FILTER_NO_LIGHTING", "ScopeFilter_ps.hls", 0, macros );
	}


	r3dRenderer->AddVertexShaderFromFile( "VS_SCOPE_FILTER", "ScopeFilter_vs.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_EXPLOSION_BLUR", "explosion_blur_ps.hls" );
	r3dRenderer->AddVertexShaderFromFile( "VS_EXPLOSION_BLUR", "explosion_blur_vs.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_FXAA", "FXAA_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_FXAA_LumPass", "fxaa_lumpass_ps.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_DIRECTIONAL_STREAKS", "directional_streaks_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_INTERPOLATE", "linear_interpolate_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_BRIGHT_PASS", "bright_pass_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_NIGHT_VISION", "night_vision_ps.hls" );

	{
		ShaderMacros macros ( 1 );

		char Value[] = "+0";

		macros[ 0 ].Name		= "NUM_GLARES";
		macros[ 0 ].Definition	= Value;

		char name[] = "PS_SUNGLARE00";

		for( int i = 0; i < MAX_SUNGLARES; i ++ )
		{
			name[ sizeof name - 3 ] = '0' + i / 10;
			name[ sizeof name - 2 ] = '0' + i % 10;

			if( i + 1 >= 10 )
			{
				Value[ sizeof Value - 3 ] = '0' + ( i + 1 ) / 10;
			}

			Value[ sizeof Value - 2 ] = '0' + ( i + 1 ) % 10;

			r3dRenderer->AddPixelShaderFromFile( name, "SunGlare.hls", 0, macros );
		}
	}

	r3dRenderer->AddPixelShaderFromFile( "PS_SUNSPOT", "SunSpot_ps.hls" );
	r3dRenderer->AddVertexShaderFromFile( "VS_SUNSPOT", "SunSpot_vs.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_BRIGHTNESS_CONTRAST", "BrightnessContrast_ps.hls" );

	for( int i = 2; i < 17; i += 2 )
	{
		r3dTL::TArray <D3DXMACRO> defines( 1 );

		defines[ 0 ].Name		= "NUM_TAPS";

		char def[] = "00";
		CalcShaderTextIdx(def, i);
		defines[ 0 ].Definition = def;

		char name[] = "PS_MULTITAP_BLUR00";
		name[ sizeof name - 3 ] += i / 10;
		name[ sizeof name - 2 ] += i % 10;

		r3dRenderer->AddPixelShaderFromFile( name, "Multitap_Blur_ps.hls", 0, defines );
	}

	for (int i = 0; i < 5; ++i)
	{
		r3dTL::TArray <D3DXMACRO> defines( 1 );

		defines[0].Name = "NUM_SOURCE_IMAGES";

		char def[] = "0";
		def[0] += i;
		defines[0].Definition = def;

		char name[] = "PS_COMPOSE_MULTIBLOOM0";
		name[strlen(name) - 1] += i;

		r3dRenderer->AddPixelShaderFromFile(name, "ComposeMultibloom_ps.hls", 0, defines);
	}

	{
		r3dTL::TArray <D3DXMACRO> defines( 2 );

		defines[ 0 ].Name		= "NUM_TAPS";

		char def[] = "00";
		int i = gPFX_DirectionalBloomWithGlowBlurV.TapCount();
		CalcShaderTextIdx(def, i);
		defines[ 0 ].Definition = def;

		defines[1].Name = "PRESERVE_GLOW";
		defines[1].Definition = "1";

		char name[] = "PS_MULTITAP_PRESERVRE_GLOW_BLUR00";
		name[ sizeof name - 3 ] += i / 10;
		name[ sizeof name - 2 ] += i % 10;

		r3dRenderer->AddPixelShaderFromFile( name, "Multitap_Blur_ps.hls", 0, defines );
	}

	{
		for( int i = 1; i < 9; ++i )
		{
			r3dTL::TArray <D3DXMACRO> defines( 3 );

			defines[ 0 ].Name		= "NUM_TAPS";

			int nTapCount = i;

			char def[] = "00";
			CalcShaderTextIdx(def, i);
			defines[ 0 ].Definition = def;

			char name[] = "PS_MULTITAP_DEPTH_BLUR00_F";
			name[ sizeof name - 4 ] += nTapCount % 10;
			name[ sizeof name - 5 ] += nTapCount / 10;

			defines[ 1 ].Name		= "DEPTH";
			defines[ 1 ].Definition = "";

			defines[ 2 ].Name		= "REVERSE";
			defines[ 2 ].Definition	= "0";

			r3dRenderer->AddPixelShaderFromFile( name, "Multitap_DOF_Blur_ps.hls", 0, defines );

			name[ sizeof name - 2 ] = 'R';
			defines[ 2 ].Definition	= "1";
			r3dRenderer->AddPixelShaderFromFile( name, "Multitap_DOF_Blur_ps.hls", 0, defines );
		}
	}

	LoadGrassShaders(false);

	r3dRenderer->AddPixelShaderFromFile( "PS_GRASS_P0", "NATURE\\Grass2_p0_ps.hls", 0, macros );

	if( r3dRenderer->SupportsVertexTextureFetch )
	{
		for( int masked = 0; masked < 2; masked ++ )
		{
			for( int has_normals = 0; has_normals < 2; has_normals ++ )
			{
				for( int is_stiff = 0; is_stiff < 2; is_stiff ++ )
				{
					GrassVShaderID vsid;

					vsid.masked	= masked;
					vsid.has_normals = has_normals;
					vsid.is_stiff = is_stiff;

					char Name[ 512 ];
					ShaderMacros macros;

					vsid.FillMacros( macros );
					vsid.ToString( Name );

					gGrassVSIds[ vsid.Id ] = r3dRenderer->AddVertexShaderFromFile( Name, "Nature\\Grass2_vs.hls", 0, macros );
				}
			}
		}
	}

	r3dRenderer->AddPixelShaderFromFile("PS_GRASS_TINT_BLUR", "Nature\\GrassTintBlur_ps.hls");
	r3dRenderer->AddVertexShaderFromFile("VS_GRASS_TINT_BLUR", "Nature\\GrassTintBlur_vs.hls");

	r3dRenderer->AddVertexShaderFromFile("VS_SKYHOFFMAN", "Nature\\SkyHoffman_vs.hls");

	{
		ShaderMacros macros( 1 ) ;

		macros[ 0 ].Name		= "TEXGEN_ENABLE" ;
		macros[ 0 ].Definition	= "0" ;

		r3dRenderer->AddVertexShaderFromFile( "VS_SKYSTATIC", "Nature\\SkyStatic_vs.hls", 0, macros );

		macros[ 0 ].Definition	= "1" ;

		r3dRenderer->AddVertexShaderFromFile( "VS_SKYSTATIC_TEXG", "Nature\\SkyStatic_vs.hls", 0, macros );
	}

	{
		ShaderMacros macros( 1 ) ;

		macros[ 0 ].Name		= "NORMAL_ENABLED" ;
		macros[ 0 ].Definition	= "0" ;

		r3dRenderer->AddPixelShaderFromFile( "PS_SKYHOFFMAN"		, "Nature\\SkyHoffman_ps.hls"	, 0, macros );
		r3dRenderer->AddPixelShaderFromFile( "PS_SKYHOFFMAN_LQ"		, "Nature\\SkyHoffmanLQ_ps.hls"	, 0, macros );

		macros[ 0 ].Name		= "NORMAL_ENABLED" ;
		macros[ 0 ].Definition	= "1" ;

		r3dRenderer->AddPixelShaderFromFile( "PS_SKYHOFFMAN_W_NORMALS"		, "Nature\\SkyHoffman_ps.hls"	, 0, macros );
		r3dRenderer->AddPixelShaderFromFile( "PS_SKYHOFFMAN_W_NORMALS_LQ"	, "Nature\\SkyHoffmanLQ_ps.hls"	, 0, macros );
	}

	{
		ShaderMacros macros;
		char name[128];

		for( int tt = 0; tt < 2; tt ++ )
		{
			for( int n = 0; n < 2; n ++ )
			{
				for( int ss = 0; ss < 2; ss ++ )
				{
					for( int hdr = 0; hdr < 2; hdr ++ )
					{
						StaticSkyPShaderID psid;

						psid.twoTextures = tt;
						psid.outputNormals = n;
						psid.scatterSun = ss;
						psid.hdr = hdr;

						psid.ToString( name );
						psid.FillMacros( macros );

						gStaticSkyPSIds[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\SkyStatic_ps.hls", 0, macros );
					}
				}
			}
		}
	}

	{
		ShaderMacros macros( 1 );
		macros[ 0 ].Name		= "RIGHT_EYE"	;
		macros[ 0 ].Definition	= "0" ;

		r3dRenderer->AddPixelShaderFromFile("PS_STEREOREPROJECT_LEFT", "StereoReproject_ps.hls", 0, macros );

		macros[ 0 ].Definition	= "1" ;

		r3dRenderer->AddPixelShaderFromFile("PS_STEREOREPROJECT_RIGHT", "StereoReproject_ps.hls", 0, macros );
	}

	r3dRenderer->AddPixelShaderFromFile("PS_ANAGLYPHCOMPOSITE", "AnaglyphComposite_ps.hls");

	if( r3dRenderer->SupportsVertexTextureFetch )
	{
		r3dTerrain3::LoadShaders();
		r3dTerrain2::LoadShaders();
	}
	else
	{
		r3d_assert(0);
	}

	//------------------------------------------------------------------------
	// lake...
	{
		{
			r3dRenderer->AddPixelShaderFromFile("RAINRIPPLES_PS",  "Nature\\rainRipples_ps.hls");

			r3dRenderer->AddVertexShaderFromFile("RIPPLES_VS", "Nature\\ripples_vs.hls");
			ShaderMacros macros1( 2 );
			macros1[ 0 ].Name	= "LOWQ" ;	macros1[ 0 ].Definition	= "0";
			macros1[ 1 ].Name	= "SPLASH" ;	macros1[ 1 ].Definition	= "0";
			r3dRenderer->AddPixelShaderFromFile("RIPPLES_PS",  "Nature\\ripples_ps.hls",0, macros1);
			macros1[ 0 ].Definition	= "1";
			r3dRenderer->AddPixelShaderFromFile("RIPPLES_N_PS",  "Nature\\ripples_ps.hls",0, macros1);
			macros1[ 1 ].Definition	= "1";
			r3dRenderer->AddPixelShaderFromFile("RIPPLESR_N_PS",  "Nature\\ripples_ps.hls",0, macros1);
			macros1[ 0 ].Definition	= "0";
			r3dRenderer->AddPixelShaderFromFile("RIPPLESR_PS",  "Nature\\ripples_ps.hls",0, macros1);
		}

		ShaderMacros macros( 2 );
		macros[ 0 ].Name	= "WATER_PLANE" ;
		macros[ 0 ].Definition	= "0" ;
		macros[ 1 ].Name	= "RIPPLES" ;
		macros[ 1 ].Definition	= "0" ;

		if (r_show_winter->GetBool())
		{
			int count = macros.Count();

			macros.Resize(count + 1);
			macros[ count ].Name		= "WINTER";
			macros[ count ].Definition	= "1";
		}

		// WATER[Follow terrain][Ripples]_[High | Normal | Low][Pixel | Vertex]
		r3dRenderer->AddPixelShaderFromFile("WATER_HP",  "Nature\\OceanV2high_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATER_HV", "Nature\\OceanV2high_vs.hls",0, macros);
		r3dRenderer->AddPixelShaderFromFile("WATER_NP",  "Nature\\OceanV2normal_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATER_NV", "Nature\\OceanV2normal_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATER_LP",  "Nature\\OceanV2low_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATER_LV", "Nature\\OceanV2low_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATERU_HP",  "Nature\\OceanV2uH_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERU_HV",  "Nature\\OceanV2uH_vs.hls",0, macros);
		r3dRenderer->AddPixelShaderFromFile("WATERU_NP",  "Nature\\OceanV2uM_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERU_NV",  "Nature\\OceanV2uM_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATERU_LP",  "Nature\\OceanV2uL_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERU_LV",  "Nature\\OceanV2uL_vs.hls",0, macros);

		macros[ 0 ].Definition	= "1" ;

		r3dRenderer->AddPixelShaderFromFile("WATERF_HP",  "Nature\\OceanV2high_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERF_HV", "Nature\\OceanV2high_vs.hls",0, macros);
		r3dRenderer->AddPixelShaderFromFile("WATERF_NP",  "Nature\\OceanV2normal_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERF_NV", "Nature\\OceanV2normal_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATERF_LP",  "Nature\\OceanV2low_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERF_LV", "Nature\\OceanV2low_vs.hls",0, macros);

		macros[ 0 ].Definition	= "0" ;
		macros[ 1 ].Definition	= "1" ;

		r3dRenderer->AddPixelShaderFromFile("WATERR_HP",  "Nature\\OceanV2high_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERR_HV", "Nature\\OceanV2high_vs.hls",0, macros);
		r3dRenderer->AddPixelShaderFromFile("WATERR_NP",  "Nature\\OceanV2normal_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERR_NV", "Nature\\OceanV2normal_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATERR_LP",  "Nature\\OceanV2low_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERR_LV", "Nature\\OceanV2low_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATERUR_HP",  "Nature\\OceanV2uH_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERUR_HV",  "Nature\\OceanV2uH_vs.hls",0, macros);
		r3dRenderer->AddPixelShaderFromFile("WATERUR_NP",  "Nature\\OceanV2uM_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERUR_NV",  "Nature\\OceanV2uM_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATERUR_LP",  "Nature\\OceanV2uL_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERUR_LV",  "Nature\\OceanV2uL_vs.hls",0, macros);

		macros[ 0 ].Definition	= "1" ;

		r3dRenderer->AddPixelShaderFromFile("WATERFR_HP",  "Nature\\OceanV2high_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERFR_HV", "Nature\\OceanV2high_vs.hls",0, macros);
		r3dRenderer->AddPixelShaderFromFile("WATERFR_NP",  "Nature\\OceanV2normal_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERFR_NV", "Nature\\OceanV2normal_vs.hls",0, macros);

		r3dRenderer->AddPixelShaderFromFile("WATERFR_LP",  "Nature\\OceanV2low_ps.hls",0, macros);
		r3dRenderer->AddVertexShaderFromFile("WATERFR_LV", "Nature\\OceanV2low_vs.hls",0, macros);
	}

	{
		ShaderMacros smacros ( 2 );

		smacros[ 0 ].Name			= "SRC_SEL" ;
		smacros[ 0 ].Definition		= "rgbb" ;

		r3dRenderer->AddPixelShaderFromFile( "PS_MIXIN_RGBB", "DS_MixIn_ps.hls", 0, macros );
	}

	r3dRenderer->AddPixelShaderFromFile( "PS_TRANSFORM", "Transform_ps.hls" );

	PS_POINTLIGHT_V2_ID = r3dRenderer->AddPixelShaderFromFile("PS_POINTLIGHT_V2_IS", "DS_Point_v2_ps.hls");
	VS_POINTLIGHT_V2_ID = r3dRenderer->AddVertexShaderFromFile("VS_POINTLIGHT_V2_IS", "DS_Point_v2_vs.hls");

	PS_VOLUME_POINTLIGHT_ID = r3dRenderer->AddPixelShaderFromFile("PS_VOLUME_POINTLIGHT_ID", "DS_Volume_Point_ps.hls");

	r3dRenderer->AddPixelShaderFromFile( "PS_DOWNSAMPLE", "DownSample_ps.hls" ) ;

	r3dRenderer->AddPixelShaderFromFile( "PS_CALC_LUMA", "CalcLuma_ps.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_EXPOSUREBLEND", "ExposureBlend_ps.hls" );

	r3dRenderer->AddVertexShaderFromFile( "VS_REMOVECUBEMAPEDGE", "RemoveCubemapEdge_vs.hls" );

	r3dRenderer->AddPixelShaderFromFile( "PS_VISUALIZE_3DSLICE", "Visualize3DSlice_ps.hls" );
	r3dRenderer->AddVertexShaderFromFile( "VS_VISUALIZE_3DSLICE", "Visualize3DSlice_vs.hls" );

	{
		ShaderMacros macros;

		macros.Resize( 1 );

		macros[ 0 ].Name		= "ONE_PIXEL_MIP";
		macros[ 0 ].Definition	= "0";

		r3dRenderer->AddPixelShaderFromFile( "PS_REMOVECUBEMAPEDGE", "RemoveCubemapEdge_ps.hls", 0, macros );

		macros[ 0 ].Definition	= "1";
		r3dRenderer->AddPixelShaderFromFile( "PS_REMOVECUBEMAPEDGE_ONE_PIXEL_MIP", "RemoveCubemapEdge_ps.hls", 0, macros );
	}

	r3dRenderer->AddVertexShaderFromFile( "VS_REMOVECUBEMAPEDGE_CUBESOURCE", "RemoveCubemapEdge_CubeSource_vs.hls" );

	{
		ShaderMacros macros;

		macros.Resize( 1 );

		macros[ 0 ].Name		= "ONE_PIXEL_MIP";
		macros[ 0 ].Definition	= "0";

		r3dRenderer->AddPixelShaderFromFile( "PS_REMOVECUBEMAPEDGE_CUBESOURCE", "RemoveCubemapEdge_CubeSource_ps.hls", 0, macros );

		macros[ 0 ].Definition	= "1";
		r3dRenderer->AddPixelShaderFromFile( "PS_REMOVECUBEMAPEDGE_CUBESOURCE_ONE_PIXEL_MIP", "RemoveCubemapEdge_CubeSource_ps.hls", 0, macros );
	}

	r3dRenderer->AddPixelShaderFromFile( "PS_DARKNESS", "Darkness_ps.hls" );

	for( int aux = 0; aux < 2; aux ++ )
	{
		FillGBufferConstantId psid;

		psid.aux = aux;

		ShaderMacros macros;

		psid.FillMacros( macros );

		char name[ 512 ];

		psid.ToString( name );

		gFillGBufferConstantPSIds[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "DS_FillbufferConst_PS.hls", 0, macros );
	}

	{
		for( int light_count = 0; light_count <= LensDirtShaderId::MAX_LIGHTS; light_count ++ )
		{
			LensDirtShaderId id;

			id.light_count = light_count;

			char name[ 256 ];

			id.ToString( name );

			ShaderMacros macros;

			id.FillMacros( macros );

			gLensDirtShaderIDs[ id.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "DS_LensDirt_ps.hls", 0, macros );
		}
	}

	r3dRenderer->AddPixelShaderFromFile( "PS_LENSDIRT_EXTRACTLUMA", "DS_LensDirtExtractLuma_ps.hls" );
	r3dRenderer->AddPixelShaderFromFile( "PS_COLOR_AND_ALPHATEXTURE", "color_and_alphatexture_ps.hls" );

	r3dRenderer->AddVertexShaderFromFile( "FORVARD_VS", "forward_vs.hls" );
	r3dRenderer->AddPixelShaderFromFile( "FORVARD_PS", "forward_ps.hls" );

	r3dOutToLog( "Finished loading shaders (%f sec)!\n", r3dGetTime() - LoadingStart );
}

void DeferredDevicePreResetCallback()
{
	if( Terrain3 )
	{
		Terrain3->DestroyDefaultPoolResources();
	}
}

void DeferredDeviceResetCallback()
{
	ResetShadowCache();

	if( Terrain3 )
	{
		Terrain3->CreateDefaultPoolResources();
	}

	if( Terrain2 )
	{
		Terrain2->ResetAtlasTiles();
	}

	g_pPostFXChief->ResetBuffers();

	WaterBase::UpdateRefractionBuffer( true );

	AnticheatOnDeviceReset();
}

extern void ( *g_SetupFilmToneConstants )( int reg ) ;

void r3dDefferedRenderer::Init()
{
	g_SetupFilmToneConstants = SetupFilmToneConstants;

	LastLFShadowSliceUpdatePos = r3dPoint3D( FLT_MAX, FLT_MAX, FLT_MAX );

	AvgSceneLuminanceBuffer = r3dScreenBuffer::CreateClass( "AvgSceneLuminance", 1, 1, D3DFMT_R32F, r3dScreenBuffer::Z_NO_Z );

	SceneExposure0 = r3dScreenBuffer::CreateClass( "SceneExposure0", 1, 1, D3DFMT_R32F, r3dScreenBuffer::Z_NO_Z );
	SceneExposure1 = r3dScreenBuffer::CreateClass( "SceneExposure1", 1, 1, D3DFMT_R32F, r3dScreenBuffer::Z_NO_Z );

	if( r3dRenderer->SupportsOQ )
	{
		AntiCheatQuery = r3dD3DQuery::CreateClass( D3DQUERYTYPE_OCCLUSION ) ;
	}

	UpdateHWSchadowScheme();

	CreateResolutionBoundResources();
	CreateShadowResources();

	UpdateMLAA();

	r3dDeviceResetCallback = DeferredDeviceResetCallback;
	r3dDevicePreResetCallback = DeferredDevicePreResetCallback;
	r_half_res_particles->SetChangeCallback(&HalfResParticleVarChangeCallback);
	r_use_ssr->SetChangeCallback(&GBuffer_SSR_Info::SSRVarChangeCallback);
	r_show_winter->SetChangeCallback(ShowWinterCallback);

	mTempObjectArrayCount = 0;
}

void r3dDefferedRenderer::Close()
{
	r3dRenderer->ResetShaders();

	DestroyShadowResources();
	DestroyResolutionBoundResources();

	SAFE_DELETE( DynamicEnvMap0 );
	SAFE_DELETE( DynamicEnvMap1 );

	SAFE_DELETE( gBuffer_StagedRender_Color		);
	SAFE_DELETE( gBuffer_StagedRender_Normal	);
	SAFE_DELETE( gBuffer_StagedRender_Depth		);
	SAFE_DELETE( gBuffer_StagedRender_Temporary );

	SAFE_DELETE( AvgSceneLuminanceBuffer );

	SAFE_DELETE( SceneExposure0 );
	SAFE_DELETE( SceneExposure1 );

	SAFE_DELETE( AntiCheatQuery );

	//r3dRenderer->DeleteTexture(gNoiseTexture2);

	//	SAFE_DELETE(D1);
	//	SAFE_DELETE(D2);

	CloseMLAA();
}



#include "RenderDefferedScene.hpp"
#include "RenderShadowMap.hpp"


extern float __CurTime;

void Render_Deffered_Sunlight( bool ambient_only );
void Render_Deffered_Pointlights();
void Render_Deffered_Planelights();
void Render_Deffered_Tubelights();
void Render_Deffered_Spotlights();
void Render_Deffered_Volume_Lights();
void CalculateSplitDistances();



void SetupLightMaskStencilStates( StencilCheckMode scm )
{
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, true )			);
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP )	);

	if( scm == SCM_GRASS )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILREF, 2 )				);
	}
	else
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILREF, 1 )				);
	}

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF )		);

	switch( scm )
	{
	case SCM_LITAREA:
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL ) );
		break ;

	case SCM_UNLITAREA:
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_GREATER ) );
		break ;

	case SCM_SSAO:
	case SCM_GRASS:
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL ) );
		break ;
	}

}

void SetupMaskWriteStencilStates( int Ref )
{
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILREF, Ref );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF );
}

#ifndef FINAL_BUILD
void RenderEditorPreview()
{
	extern bool g_EditorPreviewMaterialRecreate;
	if ( !g_EditorPreviewMaterialRecreate )
		return;

	extern char gPreviewCurrSelection[ MAX_PATH ];
	r3dMaterial* pMat = NULL;

	r3dFile* f = r3d_open( gPreviewCurrSelection, "rt" );
	r3d_assert(f);

	char* pChar1 = strrchr( gPreviewCurrSelection, '/' );
	*pChar1 = 0;
	char* pChar2 = strrchr( gPreviewCurrSelection, '/' );
	*pChar2 = 0;

	char previewTexturePath[ R3D_MAX_MATERIAL_NAME ];
	sprintf( previewTexturePath, "%s/Textures", gPreviewCurrSelection );
	*pChar1 = *pChar2 = '/';

	pMat = gfx_new r3dMaterial( 0 );
	if( !pMat->LoadAscii( f, previewTexturePath ) )
	{
		r3dOutToLog("RenderEditorPreview: failed to load material %s\n", gPreviewCurrSelection);
		SAFE_DELETE( pMat );
	}
	fclose(f);

	if ( !pMat )
		return;

	r3dMesh mesh( 0 );
	if ( mesh.Load( "Data/Editor/Objects/Sphere.sco", false, true ) && mesh.IsLoaded() )
	{
		mesh.FillBuffers();

		mesh.MatChunks[ 0 ].Mat = pMat;
		mesh.MatChunks[ 0 ].StartIndex = 0;
		mesh.MatChunks[ 0 ].EndIndex = mesh.NumIndices;
		mesh.NumMatChunks = 1;

		r3dCamera camera;
		camera.FOV = gCam.FOV;
		camera.Aspect = gBuffer_Color->Width / gBuffer_Color->Height;
		D3DXVECTOR3 vPos( 80, 80, 0 );
		D3DXVECTOR3 vDir = -vPos;
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDirProj( vDir.x, 0, vDir.z );
		D3DXVECTOR3 vUp( 0, 1, 0 );
		D3DXVec3Cross( &vUp, &vDirProj, &vDir );
		D3DXVec3Cross( &vUp, &vDir, &vUp );
		D3DXVec3Normalize( &vUp, &vUp );

		camera.SetPosition( r3dPoint3D( vPos ) );
		camera.PointTo( r3dPoint3D( vPos + vDir ) );
		camera.vUP = r3dPoint3D( vUp );

		r3dCamera oldCamera = gCam;
		gCam = camera;

		D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Material Preview");
		R3DPROFILE_START("DR: Material Preview");

		//------------------------------------------------------------------

		int __NumLights = 0;
		int __NumObjects = 0;
		int __NumPasses = 0;

		SetSunParams();

		r3dRenderer->AmbientColor = r3dColor( 128, 128, 128 );// r3dGameLevel::Environment->SunAmbientColor.GetColorValue(r3dGameLevel::Environment->__CurTime/24.0f);

		SunVector = GetEnvLightDir();

		if( r_double_depth_ssao->GetInt() && !gBuffer_SecondaryDepth )
		{
			CreateDDRenderTargets();
		}

		r3dVector V;

		r3dRenderer->SetMipMapBias(__WorldRenderBias);
		r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );

		if( r_double_depth_ssao->GetInt() )
		{
			gBuffer_SecondaryDepth->Activate();
			ClearFloatSurface( D3DXVECTOR4( gCam.FarClip, 0, 0, 0 ) );
		}

		gBuffer_Color->Activate();

		gBuffer_Normal->Activate(1);

		ActivatePrimaryDepth( 2 );

		gBuffer_Aux->Activate(3);

		r3dRenderer->StartRender(1);

		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);
		SetMRTClearShaders( false );

		D3DXVECTOR4 pconst0 = D3DXVECTOR4 ( gCam.NearClip, gCam.FarClip, 0.0f, 0.0f );
		r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, (float*) pconst0, 1 );

		r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::black );

		r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, true);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILREF, 1);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		r3dRenderer->SetPixelShader();

		r3dRenderer->SetCamera ( gCam, true );

		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

		r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

		r3dRenderer->SetVertexShader(VS_FILLGBUFFER_ID);
		FillbufferShaderKey key;

		SetLQ( &key ) ;

		SetFillGBufferPixelShader( key ) ;

		D3DXVECTOR4 CamVec = D3DXVECTOR4(gCam.x, gCam.y, gCam.z, 1);
		r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_CAMVEC, (float*)&CamVec, 1);

		float defSSAO[ 4 ] = { r_ssao_clear_val->GetFloat(), 0.f, 0.f, 0.f };
		r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_DEF_SSAO, defSSAO, 1);

		D3DXMATRIX m;
		D3DXMatrixIdentity( &m );
		mesh.SetShaderConsts( m );
		mesh.DrawMeshDeferred( r3dColor::white, 0 );

		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();
		r3dRenderer->SetFog(0);

		r3dRenderer->EndRender();

		gBuffer_Aux->Deactivate();

		DeactivatePrimaryDepth();

		gBuffer_Normal->Deactivate();

		gBuffer_Color->Deactivate();

		r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, false);

		ScreenBuffer->Activate();
		r3dRenderer->StartRender(0);
		r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, r3dColor::black.GetPacked(), r3dRenderer->GetClearZValue(), 0 );

		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

		for (int i=0;i<8;i++)
		{
			r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
			r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
		}

		r3dRenderer->SetTex(gBuffer_Color->Tex);
		r3dRenderer->SetTex(gBuffer_Normal->Tex,1);
		r3dRenderer->SetTex(gBuffer_Depth->Tex,2);
		r3dRenderer->SetTex(gBuffer_Aux->Tex,3);
		r3dRenderer->SetTex(gBuffer_Aux->Tex,4);

		r3dSetFiltering( R3D_POINT );

		// copy of aux with bilinear on ( to handle downscaled ssao nicely )
		r3dSetFiltering( R3D_BILINEAR, 4 );

		// envmap
		r3dSetFiltering( R3D_BILINEAR, 7 );
		r3dSetFiltering( R3D_BILINEAR, 8 );

		r3dVector oldSun = Sun->SunLight.Direction;
		Sun->SunLight.Direction = r3dVector( 0, -1.5, -1 );
		r3dPoint3D oldColor( Sun->SunLight.R, Sun->SunLight.G, Sun->SunLight.B );
		Sun->SunLight.R = Sun->SunLight.G = Sun->SunLight.B = 128;
		Render_Deffered_Sunlight( false );
		Sun->SunLight.Direction = oldSun.Normalize();
		Sun->SunLight.R = oldColor.x;
		Sun->SunLight.G = oldColor.y;
		Sun->SunLight.B = oldColor.z;

		SetupLightMaskStencilStates( SCM_LITAREA );

		r3dRenderer->SetTex(gBuffer_Color->Tex);
		r3dRenderer->SetTex(gBuffer_Normal->Tex,1);
		r3dRenderer->SetTex(gBuffer_Depth->Tex,2);
		r3dRenderer->SetTex(gBuffer_Aux->Tex,3);

		Render_Deffered_Pointlights();
		Render_Deffered_Tubelights();
		Render_Deffered_Planelights();
		Render_Deffered_Spotlights();
		Render_Deffered_Volume_Lights();

		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();

		r3dSetFiltering( R3D_BILINEAR );

		ScreenBuffer->Deactivate();

		extern float g_EditorPreviewSize;
		r3dScreenBuffer* pBuffer = r3dScreenBuffer::CreateClass( "Editor Preview Temp Buffer", g_EditorPreviewSize, g_EditorPreviewSize, D3DFMT_A8R8G8B8 );

		pBuffer->Activate();

		float TC[16];

		r3dTexture* pTex = ScreenBuffer->Tex;

		float fWH = float( pTex->GetWidth() ) / float( pTex->GetHeight() );
		float fHW = float( pTex->GetHeight() ) / float( pTex->GetWidth() );

		TC[0]	= 0.5f * ( 1 - fHW );	TC[1]	= 0;
		TC[2]	= 0.5f * ( 1 + fHW );	TC[3]	= 0;
		TC[4]	= TC[2];	TC[5]	= 1;
		TC[6]	= TC[ 0 ];	TC[7]	= 1;

		r3dDrawBox2D( 0, 0, pBuffer->Width, pBuffer->Height, r3dColor::white, pTex, TC );

		int nNumTextures = 0;
		if (pMat->Texture) nNumTextures++;
		if (pMat->NormalTexture) nNumTextures++;
		if (pMat->GlossTexture) nNumTextures++;
		if (pMat->EnvPowerTexture) nNumTextures++;
		if (pMat->SelfIllumTexture) nNumTextures++;
		if (pMat->NormalDetailTexture) nNumTextures++;

		MenuFont_Editor->PrintF( 5, g_EditorPreviewSize - 15, r3dColor::white, "Num Textures: %i", nNumTextures );

		pBuffer->Deactivate();

		//--------------------------------------------------------------

		R3DPROFILE_END("DR: Material Preview");
		D3DPERF_EndEvent();

		String_T<> sFullPath( gPreviewCurrSelection );
		char cacheName[ MAX_PATH ];
		sprintf( cacheName, "Data/Editor/PreviewCache/Materials/%s.dds", sFullPath.GetName().c_str() );

		pBuffer->Tex->Save( cacheName );

		SAFE_DELETE( pBuffer );

		extern bool getFileTimestamp(const char*, FILETIME&);
		extern void setFileTimestamp(const char*, const FILETIME&);
		FILETIME creationTime;
		getFileTimestamp( gPreviewCurrSelection, creationTime );
		setFileTimestamp( cacheName, creationTime );

		extern bool g_EditorPreviewMaterialTexReady;
		g_EditorPreviewMaterialTexReady = true;

		gCam = oldCamera;
	}

	mesh.Unload();

	SAFE_DELETE( pMat );

	g_EditorPreviewMaterialRecreate = false;
}
#endif

void r3dDefferedRenderer::PreRender()
{
	r3dRenderer->ResetMaterial();
	gCollectionsManager.SetInstanceViewRefPos(gCam);
#ifndef FINAL_BUILD
	RenderEditorPreview();
#endif

	void UpdateFrozenLightShadowMaps();
	UpdateFrozenLightShadowMaps();
}

extern float __VarianceLimit;
extern float __MinLimit;
extern float __DepthBias;

void SetupFilmToneConstants( int reg );

void SetupSunlightPS()
{
	bool whiteAlbedo = false;

#ifndef FINAL_BUILD
	int visualizeMode = r_scene_visualize_mode->GetInt();
	whiteAlbedo = visualizeMode == SCENE_VISUALIZE_LIGHTING;
	D3DXVECTOR4 depthMultiplier(g_SceneVisualizerDepthMultiplier, 0, 0, 0);

	if (visualizeMode != SCENE_VISUALIZE_DEPTH && visualizeMode != SCENE_VISUALIZE_STANDARD)
	{
		D3DXVECTOR4 vColor = g_SceneVisualizerColorMask;
		if (visualizeMode == SCENE_VISUALIZE_LIGHTING)
		{
			vColor = D3DXVECTOR4
			(
				(Sun->SunLight.R/255.0f)*Sun->SunLight.Intensity * g_SceneVisualizerColorMask.x,
				(Sun->SunLight.G/255.0f)*Sun->SunLight.Intensity * g_SceneVisualizerColorMask.y,
				(Sun->SunLight.B/255.0f)*Sun->SunLight.Intensity * g_SceneVisualizerColorMask.z,
				Sun->SunLight.Intensity * g_SceneVisualizerColorMask.w
			);
		}
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 1, &vColor.x, 1 );
	}

	if (visualizeMode == SCENE_VISUALIZE_NORMALS)
	{
		r3dRenderer->SetPixelShader("PS_DS_SCENE_DEBUG_VISUALIZER_NORMALS");
	}
	else if (visualizeMode == SCENE_VISUALIZE_AUX)
	{
		r3dRenderer->SetPixelShader("PS_DS_SCENE_DEBUG_VISUALIZER_AUX");
	}
	else if (visualizeMode == SCENE_VISUALIZE_DEPTH)
	{
		r3dRenderer->SetPixelShader("PS_DS_SCENE_DEBUG_VISUALIZER_DEPTH");
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &depthMultiplier.x, 1 );
	}
	else if (visualizeMode == SCENE_VISUALIZE_DIFFUSE)
	{
		r3dRenderer->SetPixelShader("PS_DS_SCENE_DEBUG_VISUALIZER_DIFFUSE");
	}
	else if (visualizeMode == SCENE_VISUALIZE_REFLECTIONS)
	{
		r3dRenderer->SetPixelShader("PS_DS_SCENE_DEBUG_VISUALIZER_REFLECTIONS");
	}
	else
	{
#endif
		{
			SunHDRLightPShaderId psid;

			extern r3dSSScatterParams gSSSParams;

			psid.sss			= !!gSSSParams.On;
			psid.transp_shadows	= ( r_transp_shadows->GetInt() && r_instanced_particles->GetInt() ) ? 1 : 0;
			psid.white_albedo	= whiteAlbedo;
			psid.ambient_only	= 0;
			psid.quality		= r_lighting_quality->GetInt() - r_lighting_quality->GetMinVal();
			psid.ssr = r_use_ssr->GetInt() > 0 ;

#ifndef FINAL_BUILD
			if( r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AMBIENT )
			{
				psid.ambient_only = 1;
			}
#endif

			r3dRenderer->SetPixelShader( gSunHDRLightPSIds[ psid.Id ] );
		}
#ifndef FINAL_BUILD
	}
#endif
}

void Render_Deffered_Sunlight( bool ambient_only )
{
	R3DPROFILE_FUNCTION("DR: Sun light");
	D3DPERF_BeginEvent(0, L"Render Deferred Sunlight");

	Sun->SunLight.bCastShadows = 1;
	Sun->SunLight.Intensity = r3dGameLevel::Environment->SunIntensity;

	Sun->SunLight.ProcessLightFunction(r3dGetFrameTime());

	Sun->SunLight.SetShaderConstants(gCam);

	float DepthZ = r3dRenderer->FarClip * 0.9375f;


#if 0
	for(int i=0; i<NumShadowSlices; ++i)
	{
		SetupSMTransform(ShadowSlices[i]);
		r3dRenderer->SetTex(ShadowSlices[i].shadowBuffer->Tex,5+i);
	}
#else
	SetupSMTransform( ShadowSlices[ 0 ] );
#endif

#ifdef USE_VSM
	r3dSetFiltering( R3D_BILINEAR, 5 );
	r3dSetFiltering( R3D_BILINEAR, 6 );
	r3dSetFiltering( R3D_BILINEAR, 7 );
#endif


	// VS constants
	{
		D3DXMATRIX ShaderMat;

		D3DXMatrixTranspose( &ShaderMat, &r3dRenderer->ViewProjMatrix );

		// float4x4 mProj 	: register(c0);
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

		float uv2Scale = 1.0f;

		float vScreenT[4] = { 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, uv2Scale, uv2Scale };
		// float4 vScreenT : register( c24 );
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 24, vScreenT, 1 );
	}

	extern r3dSSScatterParams gSSSParams ;

	r3dPoint3D lightDir	= GetEnvLightDir();
	float4 lightColor	= GetEnvLightColor();
	float4 ambientColor	= GetEnvLightAmbient();

	// PS constants
	{
		// this overrides color set by sun light. We need it because
		// we have moon color mixed in
		// float4	 Color 			: register(c1);
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 1, &lightColor.x, 1 );

		float vConst[4] = { lightDir.x, lightDir.y, lightDir.z, 1.f/DepthZ };

		// float4   vLight_InvRefZ : register(c3);
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 3, vConst, 1 );

		r3dColor BLight = GetEnvBackLightColor();

		float vBacklight[4] = { float(BLight.R)/255.0f,float(BLight.G)/255.0f,float(BLight.B)/255.0f, r3dGameLevel::Environment->BacklightIntensity };

		vBacklight[0] = powf( vBacklight[0], 2.2f ) ;
		vBacklight[1] = powf( vBacklight[1], 2.2f ) ;
		vBacklight[2] = powf( vBacklight[2], 2.2f ) ;

		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 30, vBacklight, 1 );

		SetSSSParams(22, true, gSSSParams );
	}

	if( ambient_only )
	{
		if(r_lighting_quality->GetInt()==r_lighting_quality->GetMinVal())
			r3dRenderer->SetPixelShader( "PS_DS_AMBIENT" ) ;
		else
			r3dRenderer->SetPixelShader( "PS_DS_AMBIENT_HQ" ) ;
	}
	else
	{
		SetupSunlightPS();

		if (r_lighting_quality->GetInt() != r_lighting_quality->GetMinVal())
		{
			if( r_transp_shadows->GetInt() && r_shadows->GetBool() )
			{
				r3dRenderer->SetTex( TransparentShadowMap->Tex, 10 ) ;
				r3dSetFiltering( R3D_POINT, 10 ) ;
				D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 10, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) ) ;
				D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 10, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) ) ;

				D3DXMATRIX shmat ;

				D3DXMatrixMultiply( &shmat, &TransparentShadowSlice.lightView, &TransparentShadowSlice.lightProj ) ;

				D3DXMATRIX toTex(
						0.5f, +0.0f, 0.0f, 0.0f,
						0.0f, -0.5f, 0.0f, 0.0f,
						0.0f, +0.0f, 1.0f, 0.0f,
						0.5f, +0.5f, 0.0f, 1.0f
					) ;

				D3DXMatrixMultiply( &shmat, &shmat, &toTex ) ;

				D3DXVECTOR4 psConsts[ 5 ] ;

				D3DXMatrixTranspose( (D3DXMATRIX*)psConsts, &shmat ) ;

				// override cause this row( column? ) is unused

				float wi = TransparentShadowMap->Width ;
				float he = TransparentShadowMap->Height ;
				float invWi = 1.f / wi ;
				float invHe = 1.f / he ;

				psConsts[ 3 ] = D3DXVECTOR4( invWi, 0.f, invHe, r_transp_shadowmap_fade->GetFloat() ) ;
				psConsts[ 4 ] = D3DXVECTOR4( wi, he, invWi, invHe ) ;

				r3dRenderer->pd3ddev->SetPixelShaderConstantF( 31, (float*)&psConsts, R3D_ARRAYSIZE( psConsts ) ) ;
			}
		}
	}

	r3dRenderer->SetVertexShader("VS_DS_DIRLIGHT");

	r3dRenderer->SetTex( g_EnvmapProbes.GetClosestTexture( gCam ), 8 );
	// cube envmap
#if 0
	r3dSetFiltering( R3D_POINT, 8 );
#else
	r3dSetFiltering( R3D_BILINEAR, 8 );
#endif

	r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_ADDRESSW,   D3DTADDRESS_WRAP );

	if (r_use_ssr->GetInt())
	{
		r3dSetFiltering( R3D_BILINEAR, 7 );

		r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSW,   D3DTADDRESS_CLAMP );

		r3dRenderer->SetTex( gBuffer_SSR.gBuffer_SSR->Tex, 7 );


		float maxLod = gBuffer_SSR.gBuffer_SSR->Tex->GetNumMipmaps();
		float lodFactor = 2.0f + ((float)(r_ssr_mips_factor->GetInt()))/50.0f;
		float ssr_setup[2][4]  = {
			{1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetWidth(), 1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetHeight(), 0, -1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetWidth() },
			{ maxLod, lodFactor, 0.0f, 0.0f}
		};

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 51, ssr_setup[0], 2 ) ) ;

	}

	r3dMaterialLibrary::SetupExtendedMaterialParameters();

	r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

	if(gCloudShadow)
	{
		r3dRenderer->SetTex(gCloudShadow->Tex, 9);
	}

	/*unsigned int R[] = {0,1,2,3};
	for(unsigned int i=0; i<sizeof(R)/sizeof(R[0]); i++)
	{
		r3dRenderer->pd3ddev->SetSamplerState( R[i], D3DSAMP_MAGFILTER,   D3DTEXF_POINT );
		r3dRenderer->pd3ddev->SetSamplerState( R[i], D3DSAMP_MINFILTER,   D3DTEXF_POINT );
		r3dRenderer->pd3ddev->SetSamplerState( R[i], D3DSAMP_MIPFILTER,   D3DTEXF_LINEAR );
	}*/

	r3dDrawBox2DZ(0,0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, DepthZ, r3dColor(255,150,0));

	D3DPERF_EndEvent();
}

struct SetRestoreRender2DStates
{
	SetRestoreRender2DStates()
	{
		D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_STENCILENABLE, &PrevStencil ) );
		D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_SCISSORTESTENABLE, &PrevScissor ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );
	}

	~SetRestoreRender2DStates()
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, PrevStencil ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, PrevStencil ) );
	}

	DWORD PrevStencil;
	DWORD PrevScissor;
};

void BlurPosLightShadow( r3dLight* l, const D3DXMATRIX& lightMtx, const D3DXMATRIX& lightP )
{
	D3DPERF_BeginEvent( 0, L"BlurPointLightShadow" );

	SetRestoreClampAddressMode setRestoreClamp0( 0 ); setRestoreClamp0;
	SetRestoreClampAddressMode setRestoreClamp1( 1 ); setRestoreClamp1;

	// get desired projection area & location
	float smSizeX;
	float smSizeY;
	float smX;
	float smY;
	float smXE;
	float smYE;

	{
		D3DXVECTOR3 centrePoint ( l->x, l->y, l->z );

		float r = l->GetVisibilityRadius();

		D3DXVECTOR4 lpos( centrePoint.x, centrePoint.y, centrePoint.z, 1.f );
		D3DXVec4Transform( &lpos, &lpos, &r3dRenderer->ViewMatrix );

		// do not fall beyond or on projection plane to avoid backprojecting & perspective divide by 0
		float z = r + R3D_MIN( lpos.z - r - r3dRenderer->NearClip, 0.f );

		lpos.w = 1.f;

		D3DXVECTOR4 p[ 8 ] =
		{
			D3DXVECTOR4( -r, -r, -z, 0.f ) + lpos,
			D3DXVECTOR4( -r, +r, -z, 0.f ) + lpos,
			D3DXVECTOR4( +r, -r, -z, 0.f ) + lpos,
			D3DXVECTOR4( +r, +r, -z, 0.f ) + lpos,
			D3DXVECTOR4( -r, -r, +r, 0.f ) + lpos,
			D3DXVECTOR4( -r, +r, +r, 0.f ) + lpos,
			D3DXVECTOR4( +r, -r, +r, 0.f ) + lpos,
			D3DXVECTOR4( +r, +r, +r, 0.f ) + lpos
		};

		float mix = FLT_MAX, max = - FLT_MAX, miy = FLT_MAX, may = -FLT_MAX;

		for( uint32_t i = 0, e = sizeof p / sizeof p[ 0 ]; i < e; i ++ )
		{
			D3DXVECTOR4 t;
			D3DXVec4Transform( &t, p + i, &r3dRenderer->ProjMatrix );

			t *= 1.f / t.w;

			mix = R3D_MIN( t.x, mix );
			miy = R3D_MIN( t.y, miy );

			max = R3D_MAX( t.x, max );
			may = R3D_MAX( t.y, may );
		}

		smX		= R3D_MIN( R3D_MAX( mix, -1.f ), 1.f );
		smY		= R3D_MIN( R3D_MAX( miy, -1.f ), 1.f );

		smXE	= R3D_MIN( R3D_MAX( max, -1.f ), 1.f );
		smYE	= R3D_MIN( R3D_MAX( may, -1.f ), 1.f );

		smSizeX	= R3D_MIN( R3D_MAX( ( smXE - smX ) * 0.5f, 0.f ), 1.f );
		smSizeY	= R3D_MIN( R3D_MAX( ( smYE - smY ) * 0.5f, 0.f ), 1.f );
	}

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

	TempShadowBuffer->Activate();

	r3dSetFiltering( R3D_POINT, 0 );
	r3dSetFiltering( R3D_POINT, 1 );

	bool CubeMapBased = l->Type == R3D_OMNI_LIGHT || l->Type == R3D_TUBE_LIGHT ;

	if( l->IsShadowFrozen() )
		r3dRenderer->SetTex( l->ShadowDepth, 0 );
	else
	{
		if( CubeMapBased )
		{
			r3dRenderer->SetTex( SharedShadowCubeMap->Tex , 0 );
		}
		else
		{
			r3dRenderer->SetTex( SharedShadowMap->Tex, 0 );
		}
	}


	r3dRenderer->SetTex( gBuffer_Depth->Tex, 1 );

	if (r_shadow_use_normals->GetInt())
		r3dRenderer->SetTex( gBuffer_Normal->Tex, 3 );

	D3DXMATRIX ident;
	D3DXMatrixIdentity( &ident );

	r3dSSSBParams parms;

	if( l->bUseGlobalSSSBParams )
	{
		parms.PhysRange	= r_shadows_blur_phys_range->GetFloat();
		parms.Bias		= r_shadows_blur_bias->GetFloat();
		parms.Sense		= r_shadows_blur_sense->GetFloat();
		parms.Radius	= r_shadows_blur_radius->GetFloat();
	}
	else
	{
		parms			= l->SSSBParams;
		parms.PhysRange	/= l->GetOuterRadius() * 8;
	}

	SetupAccumSMConstants(	r3dRenderer->NearClip,
							r3dRenderer->FarClip,
							r3dRenderer->ViewMatrix,
							r3dRenderer->ProjMatrix,
							lightMtx,
							*l,
							1.0f,
							0.05f,
							l->GetVisibilityRadius(),
							l->GetVisibilityRadius(),
							parms.PhysRange,
							SharedShadowMap,
							false,
							false,
							1.f / r3dGetLightSMDepthCoef( l )
							);

	AccumShadowsPShaderID psid;

	psid.light_type			= CubeMapBased ? SHADOWACCUM_LIGHT_POINT : SHADOWACCUM_LIGHT_SPOT;
	psid.fxaa_blur			= 0;
	psid.hw_shadowmaps		= 0;
	psid.recticular_warp	= 0;
	psid.use_normals = r_shadow_use_normals->GetInt();

	r3dRenderer->SetPixelShader( gAccumShadowsPShaderIDs[ psid.Id ] );

	r3dRenderer->SetVertexShader( "VS_POSTFX_RECONST_W_GEOM_TRANSFORM" );

	r3dSetRestoreFSQuadVDecl postFXVDecl; (void)postFXVDecl;

	float smXN = ( smX * 0.5f + 0.5f );
	float smYN = ( -smY * 0.5f + 0.5f );

	D3DRECT rect;

	rect.x1 = R3D_MAX( long( smXN * TempShadowBuffer->Width ) - 16l, 0l );
	rect.x2 = R3D_MIN( long( ( smXN + smSizeX ) * TempShadowBuffer->Width ) + 16l, (long)TempShadowBuffer->Width );
	rect.y1 = R3D_MAX( long( ( smYN - smSizeY ) * TempShadowBuffer->Height ) - 16l, 0l );
	rect.y2 = R3D_MIN( long( smYN * TempShadowBuffer->Height ) + 16l, (long)TempShadowBuffer->Height );

	D3D_V( r3dRenderer->pd3ddev->Clear( 1, &rect, D3DCLEAR_TARGET, 0xffffffff, r3dRenderer->GetClearZValue(), 0 ) );

	D3DXVECTOR4 vsConsts[] =
	{
		// vSubPix
		D3DXVECTOR4(0.5f / TempShadowBuffer->Width, 0.5f / TempShadowBuffer->Height, 0.f, 0.f),
		//	Pos transform
		D3DXVECTOR4(smSizeX, smSizeY, smSizeX + smX, smSizeY + smY),
		//	Tc transform
		D3DXVECTOR4(smSizeX, smSizeY, 0.5f * smX + 0.5f, 0.5f - 0.5f * smYE)
	};

	// VS Consts
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[0].x, _countof(vsConsts) ) );
	r3dDrawFullScreenQuad(false);

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	TempShadowBuffer->Deactivate();

	BlurShadowMap( TempShadowBuffer, TempScreenBuffer, smX, smY, smXE, smYE, parms );

	D3DPERF_EndEvent();
}

enum DesiredLightState
{
	NONE,
	STATES,
	STATES_SM
};

static r3dPoint3D vFaceCamZDir[ r3dScreenBuffer::FACE_COUNT ] =
{
	r3dPoint3D( +1.f, +0.f, +0.f ),
	r3dPoint3D( -1.f, +0.f, +0.f ),
	r3dPoint3D( +0.f, +1.f, +0.f ),
	r3dPoint3D( +0.f, -1.f, +0.f ),
	r3dPoint3D( +0.f, +0.f, +1.f ),
	r3dPoint3D( +0.f, +0.f, -1.f )
};

static r3dPoint3D vFaceCamYDir[ r3dScreenBuffer::FACE_COUNT ] =
{
	r3dPoint3D( +0.f, +1.f, +0.f ),
	r3dPoint3D( +0.f, +1.f, +0.f ),
	r3dPoint3D( +0.f, +0.f, -1.f ),
	r3dPoint3D( +0.f, +0.f, +1.f ),
	r3dPoint3D( +0.f, +1.f, +0.f ),
	r3dPoint3D( +0.f, +1.f, +0.f )
};

R3D_FORCEINLINE void SetupCamForFace( r3dCamera& lightCam, r3dLight* l, int f )
{
	float fov = 90.f;

	float lightFar = l->GetVisibilityRadius() ;
	float lightNear = 0.05f;

	(r3dPoint3D&)lightCam	= *l;

	lightCam.NearClip	= lightNear;
	lightCam.FarClip	= lightFar;

	lightCam.vPointTo	= vFaceCamZDir[ f ];
	lightCam.vUP		= vFaceCamYDir[ f ];

	// perspective projection matrix
	lightCam.FOV		= fov;

	lightCam.ProjectionType	= r3dCamera::PROJTYPE_PRESPECTIVE;
	lightCam.Width			= 0.f;
	lightCam.Height			= 0.f;
	lightCam.Aspect			= 1.0f;
}

void RenderCubemapShadowmapFace( r3dLight* l, int f )
{
	D3D_V( r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) );

	// this is for ClearFloatSurface
	r3dRenderer->SetCullMode( D3DCULL_CCW );

	float lightFar = l->GetVisibilityRadius() ;
	ClearFloatSurface( D3DXVECTOR4( lightFar, lightFar, lightFar, lightFar ) );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	r3dCamera lightCam;

	SetupCamForFace( lightCam, l, f );

	bool doubleDepth = r3dRenderer->SupportsR32FBlending && r_dd_pointlight_shadows->GetBool();

	RenderShadowMap( lightCam, doubleDepth, false );
}

void SetupCameraForParaboloidFace( r3dCamera* oCam, r3dLight* l, int face )
{
	static_cast<r3dPoint3D&>( *oCam ) = *l;
	oCam->PointTo( *oCam + r3dPoint3D( 0,0, face ? -1 : +1 ) );

	oCam->NearClip = 0.05f;
	oCam->FarClip = l->GetOuterRadius();

	oCam->ProjectionType = r3dCamera::PROJTYPE_CUSTOM;
}

void RenderParaboloidShadowmapFace( r3dLight* l, int face )
{
	struct PerfEvent
	{
		PerfEvent()
		{
			D3DPERF_BeginEvent( 0, L"RenderParaboloidShadowmapFace" );
		}
		~PerfEvent()
		{
			D3DPERF_EndEvent();
		}
	} perfEvent; (void)perfEvent;

	D3D_V( r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) );

	if( face )
	{
		r3dRenderer->SetViewport( 0, r3dRenderer->ScreenH2, r3dRenderer->ScreenW, r3dRenderer->ScreenH2 ) ;
	}
	else
	{
		r3dRenderer->SetViewport( 0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH2 ) ;
	}

	// this is for ClearFloatSurface
	r3dRenderer->SetCullMode( D3DCULL_CCW );

	float lightFar = l->GetVisibilityRadius() ;
	ClearFloatSurface( D3DXVECTOR4( lightFar, lightFar, lightFar, lightFar ) );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	r3dCamera lightCam;

	SetupCameraForParaboloidFace( &lightCam, l, face );

	RenderShadowMap( lightCam, false, true );
}

void RenderParaboloidShadowmap( r3dLight* l, DesiredLightState& desiredLightState, DesiredLightState& lastLightState, bool do_shadow_blur )
{
	struct PerfEvent
	{
		PerfEvent()
		{
			D3DPERF_BeginEvent( 0, L"RenderParaboloidShadowmap" );
		}
		~PerfEvent()
		{
			D3DPERF_EndEvent();
		}
	} perfEvent; (void)perfEvent;

	ScreenBuffer->Deactivate();

	desiredLightState = STATES_SM;

	if( lastLightState != NONE )
	{
		r3dDrawGeoSpheresEnd();
	}

	r3dRenderer->RestoreCullMode();

	SetRestoreRender2DStates setRestoreRender2DStates; (void)setRestoreRender2DStates;

	{
		SetRestoreZDir setRestoreZDir( r3dRenderLayer::ZDIR_NORMAL ); (void)setRestoreZDir;

		r3dRenderer->SetFrustumType( R3D_FRUSTUM_PARABOLOID );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) );

		SharedShadowMap->Activate();

		for( int i = 0, e = 2 ; i < 2; i ++ )
		{
			RenderParaboloidShadowmapFace( l, i );
		}

		SharedShadowMap->Deactivate( 0 ) ;


		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED |
																				D3DCOLORWRITEENABLE_GREEN |
																				D3DCOLORWRITEENABLE_BLUE |
																				D3DCOLORWRITEENABLE_ALPHA ) );

		r3dRenderer->SetFrustumType( R3D_FRUSTUM_NORMAL );

	}

	r3dRenderer->SetCamera( gCam, true );

	ScreenBuffer->Activate();
}

void RenderCubemapShadowmap( r3dLight* l, DesiredLightState& desiredLightState, DesiredLightState& lastLightState, bool blur_shadows )
{
	ScreenBuffer->Deactivate();

	desiredLightState = STATES_SM;

	if( lastLightState != NONE )
	{
		r3dDrawGeoSpheresEnd();
	}

	r3dRenderer->RestoreCullMode();

	SetRestoreRender2DStates setRestoreRender2DStates; (void)setRestoreRender2DStates;

	if( !l->IsShadowFrozen() )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) );

		for( int i = 0, e = r3dScreenBuffer::FACE_COUNT; i < e; i ++ )
		{
			wchar_t* faceName;

			switch( i ) {
				case 0: faceName = L"X+"; break;
				case 1: faceName = L"X-"; break;
				case 2: faceName = L"Y+"; break;
				case 3: faceName = L"Y-"; break;
				case 4: faceName = L"Z+"; break;
				case 5: faceName = L"Z-"; break;
			}

			D3DPERF_BeginEvent( 0, faceName );

			SharedShadowCubeMap->Activate( 0, i );

			RenderCubemapShadowmapFace( l, i );

			SharedShadowCubeMap->Deactivate(0);

			D3DPERF_EndEvent();
		}

		r3dRenderer->SetCamera( gCam, true );


		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED |
																				D3DCOLORWRITEENABLE_GREEN |
																				D3DCOLORWRITEENABLE_BLUE |
																				D3DCOLORWRITEENABLE_ALPHA ) );
	}

	r3dRenderer->RestoreCullMode();

	if( blur_shadows )
	{
		D3DXMATRIX ident;
		D3DXMatrixIdentity( &ident );

		D3DPERF_BeginEvent( 0, L"Blur Shadows" ) ;
		R3DPROFILE_START( "Blur Shadows" ) ;

		D3DXMATRIX lightP ;

		// this is not really needed - for the future coz it'll be forgotten!
		//
		if( r_hardware_shadow_method->GetInt() != HW_SHADOW_METHOD_R32F && !l->IsShadowFrozen() )
		{
			r3dCamera lightCam ;
			SetupCamForFace( lightCam, l, 0 );
			r3dRenderer->SetCamera( lightCam, false );
			lightP = r3dRenderer->ProjMatrix ;
			r3dRenderer->SetCamera( gCam, true );
		}
		else
		{
			D3DXMatrixIdentity( &lightP );
		}

		BlurPosLightShadow( l, ident, lightP );

		R3DPROFILE_END( "Blur Shadows" ) ;
		D3DPERF_EndEvent( ) ;
	}

	ScreenBuffer->Activate();

}

void MarkFrozenShadowsDirty()
{
	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || ( l->Type != R3D_OMNI_LIGHT && l->Type != R3D_SPOT_LIGHT ) ) continue;

		l->bFrozenShadowDepthDirty = 1;
	}
}

void UpdateLightFrozenDepthMaps()
{
	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || ( l->Type != R3D_OMNI_LIGHT && l->Type != R3D_SPOT_LIGHT ) ) continue;

		l->UpdateDepthMap();
	}
}

void DumpLightDepthStats()
{
	void ConPrint( const char* fmt, ... );

	ConPrint( "Frozen Shadow Depths\n" );

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || ( l->Type != R3D_OMNI_LIGHT && l->Type != R3D_SPOT_LIGHT ) ) continue;

		if( l->ShadowDepth )
		{
			ConPrint( "%dx%d%s (%c)\n", l->ShadowDepth->GetWidth(), l->ShadowDepth->GetHeight(),
					l->ShadowDepth->isCubemap() ? "x6" : "x1",
					l->bIsShadowDepthFrozen ? 'f' : 'n' );
		}
	}

	ConPrint( "done.\n" );
}

void UpdateFrozenLightShadowMaps()
{
	D3DPERF_BeginEvent( 0x0, L"UpdateFrozenLightShadowMaps" );

	struct BakedSizeEntry
	{
		r3dScreenBuffer*	buf ;
		IDirect3DTexture9*	tex ;
	};

	typedef r3dgfxMap( int, BakedSizeEntry ) BufferMap ;

	BufferMap cubeBufferMap ;
	BufferMap texBufferMap ;

	IDirect3DSurface9* depthBuf = 0 ;

	struct OnExit
	{
		OnExit()
		: prevDepth ( 0 )
		{
			r3dRenderer->GetDSS( &prevDepth ) ;
			gDisableDynamicObjectShadows = 1 ;
		}

		~OnExit()
		{
			r3dRenderer->SetDSS( prevDepth ) ;
			prevDepth->Release();

			for( BufferMap::iterator i = bmap->begin(), e = bmap->end() ; i != e ; ++ i )
			{
				BakedSizeEntry& en = i->second ;
				SAFE_DELETE( en.buf ) ;
				SAFE_RELEASE( en.tex ) ;
			}

			for( BufferMap::iterator i = bmapCube->begin(), e = bmapCube->end() ; i != e ; ++ i )
			{
				BakedSizeEntry& en = i->second ;
				SAFE_DELETE( en.buf ) ;
				SAFE_RELEASE( en.tex ) ;
			}

			SAFE_RELEASE( *pDepthBuf ) ;

			gDisableDynamicObjectShadows = 0 ;
		}

		IDirect3DSurface9** pDepthBuf ;
		BufferMap* bmap ;
		BufferMap* bmapCube ;
		IDirect3DSurface9* prevDepth ;

	} onExit ;

	onExit.pDepthBuf	= &depthBuf ;
	onExit.bmap			= &texBufferMap ;
	onExit.bmapCube		= &cubeBufferMap ;

	bool lost = false ;

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || ( l->Type != R3D_OMNI_LIGHT && l->Type != R3D_SPOT_LIGHT ) ) continue;

		if( !l->bCastShadows )
			continue;

		if( l->IsShadowFrozen() && l->bFrozenShadowDepthDirty )
		{
			r3dCamera saveCam = gCam;

			struct SaveRestoreCam
			{
				SaveRestoreCam()
				{
					cam = gCam;
				}

				~SaveRestoreCam()
				{
					 gCam = cam;
				}

				r3dCamera cam;
			} saveRestoreCam; (void)saveRestoreCam;

			(r3dPoint3D&)gCam = *l;

			if( !depthBuf )
			{
				D3D_V( r3dRenderer->pd3ddev->CreateDepthStencilSurface( r3dLight::MAX_FORZEN_SHADOW_DEPTH_SIZE, r3dLight::MAX_FORZEN_SHADOW_DEPTH_SIZE, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &depthBuf, 0 ) ) ;
			}

			l->UpdateDepthMap() ;

			int depthSize = l->FrozenShadowDepthSize ;

			r3d_assert( depthSize <= r3dLight::MAX_FORZEN_SHADOW_DEPTH_SIZE ) ;
			r3d_assert( r3dIsPow2( depthSize ) ) ;

			struct PerfMarkers
			{
				PerfMarkers()
				{
					D3DPERF_BeginEvent( 0, L"FrozenShadowMap" ) ;
				}

				~PerfMarkers()
				{
					D3DPERF_EndEvent() ;
				}

			} perfMarkers ; (void) perfMarkers ;

			if( l->Type == R3D_OMNI_LIGHT )
			{
				BufferMap::iterator found = cubeBufferMap.find( depthSize ) ;

				if( found == cubeBufferMap.end() )
				{
					BakedSizeEntry bse ;

					bse.buf = r3dScreenBuffer::CreateClass( "one_of_many", depthSize, depthSize, D3DFMT_R32F, r3dScreenBuffer::Z_NO_Z, 1 );

					// this is system mem thus unlikely to fail.
					D3D_V( r3dRenderer->pd3ddev->CreateTexture( depthSize, depthSize, 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &bse.tex, NULL ) );

					cubeBufferMap[ l->FrozenShadowDepthSize ] = bse ;
				}

				const BakedSizeEntry& bse = cubeBufferMap[ l->FrozenShadowDepthSize ] ;

				for( int i = 0, e = r3dScreenBuffer::FACE_COUNT ; i < e ; i ++ )
				{
					bse.buf->Activate( 0 ) ;

					r3dRenderer->SetDSS( depthBuf ) ;

					RenderCubemapShadowmapFace( l, i ) ;

					bse.buf->Deactivate() ;

					//	Restore camera
					r3dRenderer->SetCamera( gCam, true );

					IDirect3DSurface9* targSurf ;

					D3D_V( bse.tex->GetSurfaceLevel( 0, &targSurf ) );

					HRESULT hr = r3dRenderer->pd3ddev->GetRenderTargetData( bse.buf->GetTex2DSurface(), targSurf ) ;

					if( hr == D3DERR_DEVICELOST )
					{
						// finish some other time
						r3dRenderer->SetDeviceLost() ;
						targSurf->Release() ;
						return ;
					}

					r3d_assert( hr == S_OK ) ;

					IDirect3DCubeTexture9* cubeTex = l->ShadowDepth->AsTexCUBE();

					D3DLOCKED_RECT lrect_dest, lrect_src ;

					D3D_V( cubeTex->LockRect( D3DCUBEMAP_FACES( i ), 0, &lrect_dest, 0, 0 ) ) ;

					D3D_V( bse.tex->LockRect( 0, &lrect_src, 0, D3DLOCK_READONLY ) );

					r3d_assert( lrect_src.Pitch == lrect_dest.Pitch * 2 ) ;

					{
						float* src = (float*) lrect_src.pBits ;
						uint16_t* dest = (uint16_t*) lrect_dest.pBits ;

						float convCoef = 65535.f * r3dGetLightSMDepthCoef( l ) ;

						for( int i = 0, e = lrect_src.Pitch / 4 * depthSize ; i < e; i ++ )
						{
							*dest ++ = (uint16_t) R3D_MAX( R3D_MIN ( int( ( * src ++ ) * convCoef ), 65535 ), 0 ) ;
						}
					}

					D3D_V( cubeTex->UnlockRect( D3DCUBEMAP_FACES( i ), 0 ) );

					D3D_V( bse.tex->UnlockRect( 0 ) );

					targSurf->Release() ;
				}
			}
			else
			if( l->Type == R3D_SPOT_LIGHT )
			{
				BufferMap::iterator found = texBufferMap.find( depthSize ) ;

				if( found == texBufferMap.end() )
				{
					BakedSizeEntry bse ;

					bse.buf = r3dScreenBuffer::CreateClass( "one_of_many", depthSize, depthSize, D3DFMT_R32F, r3dScreenBuffer::Z_NO_Z );

					// this is system mem thus unlikely to fail.
					D3D_V( r3dRenderer->pd3ddev->CreateTexture( depthSize, depthSize, 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &bse.tex, NULL ) );

					texBufferMap[ l->FrozenShadowDepthSize ] = bse ;
				}

				const BakedSizeEntry& bse = texBufferMap[ l->FrozenShadowDepthSize ] ;


				bse.buf->Activate( 0 ) ;

				r3dRenderer->SetDSS( depthBuf ) ;

				D3D_V( r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) );

				// this is for ClearFloatSurface
				r3dRenderer->SetCullMode( D3DCULL_CCW );

				float lightFar = l->GetVisibilityRadius() ;
				ClearFloatSurface( D3DXVECTOR4( lightFar, lightFar, lightFar, lightFar ) );

				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

				D3DXMATRIX lightMtx ;
				r3dCamera lightCam ;
				SetupSpotLightMatrices( l, &lightMtx, &lightCam ) ;
				RenderShadowMap( lightCam, true, false ) ;

				bse.buf->Deactivate() ;

				IDirect3DSurface9* targSurf ;

				D3D_V( bse.tex->GetSurfaceLevel( 0, &targSurf ) );

				HRESULT hr = r3dRenderer->pd3ddev->GetRenderTargetData( bse.buf->GetTex2DSurface(), targSurf ) ;

				if( hr == D3DERR_DEVICELOST )
				{
					// finish some other time
					r3dRenderer->SetDeviceLost() ;
					targSurf->Release() ;
					return ;
				}

				r3d_assert( hr == S_OK ) ;

				IDirect3DTexture9* tex = l->ShadowDepth->AsTex2D() ;

				D3DLOCKED_RECT lrect_dest, lrect_src ;

				D3D_V( tex->LockRect( 0, &lrect_dest, 0, 0 ) ) ;

				D3D_V( bse.tex->LockRect( 0, &lrect_src, 0, D3DLOCK_READONLY ) );

				r3d_assert( lrect_src.Pitch == lrect_dest.Pitch * 2 ) ;

				{
					float* src = (float*) lrect_src.pBits ;
					uint16_t* dest = (uint16_t*) lrect_dest.pBits ;

					float convCoef = 65535.f * r3dGetLightSMDepthCoef( l ) ;

					for( int i = 0, e = lrect_src.Pitch / 4 * depthSize ; i < e; i ++ )
					{
						*dest ++ = (uint16_t) R3D_MAX( R3D_MIN ( int( ( * src ++ ) * convCoef ), 65535 ), 0 ) ;
					}
				}

				D3D_V( tex->UnlockRect( 0 ) );

				D3D_V( bse.tex->UnlockRect( 0 ) );

				targSurf->Release() ;
			}

			l->bFrozenShadowDepthDirty = false ;
		}

	}

	D3DPERF_EndEvent() ;
}

//////////////////////////////////////////////////////////////////////////

static float pointProjTexMessageTime = 0;

void Render_Deffered_Pointlights()
{
#ifndef FINAL_BUILD
	if( r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AMBIENT )
		return;
#endif

	R3DPROFILE_FUNCTION("DR: Pointlights");
	D3DPERF_BeginEvent ( 0x0, L"PointLights" );

	r3dRenderer->Flush();

	float camPadding = GetCamPadding();

	DesiredLightState desiredLightState, lastLightState = NONE;

	bool castShadows = r_lighting_quality->GetInt() > 1 ;

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || l->Type != R3D_OMNI_LIGHT ) continue;

#ifndef FINAL_BUILD
		if( g_bEditMode && l->ProjectMap && !l->ProjectMap->isCubemap() )
		{
			if( r3dGetTime() - pointProjTexMessageTime > 45 )
			{
				char text[ 512 ];
				sprintf( text, "Please change projection texture of POINT light at [%.1f  %.1f  %.1f] to be Cube Texture and not a 2D( or Volume) Texture", l->x, l->y, l->z );
				MessageBoxA( r3dRenderer->HLibWin, text, "Suggestion", MB_OK );

				pointProjTexMessageTime = r3dGetTime();
			}
		}
#endif

		l->ProcessLightFunction(r3dGetFrameTime());

		if(!l->IsOn()) continue;

		l->RecalcBoundBox();
		if (!r3dRenderer->IsBoxInsideFrustum(l->BBox)) continue;

		float VIS_RANGE = RenderDistance + l->GetOuterRadius();
		r3dVector V = gCam - (r3dPoint3D)*l;
		if (V.Length() > VIS_RANGE) continue;

		bool do_shadow_blur = r_shadow_blur->GetInt() && l->bSSShadowBlur ;

		if( castShadows && l->bCastShadows && ((V.Length()-l->GetOuterRadius())<r_shadow_light_source_render_dist->GetFloat()) && r_shadows->GetBool() )
		{
			r3dRenderer->SetZRange( R3D_ZRANGE_WHOLE ) ;
#ifndef FINAL_BUILD
			if( r_parabolo_pointlight_shadows->GetInt() )
			{
				RenderParaboloidShadowmap( l, desiredLightState, lastLightState, do_shadow_blur ) ;
			}
			else
#endif
			{
				RenderCubemapShadowmap( l, desiredLightState, lastLightState, do_shadow_blur ) ;
			}

			r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;
		}
		else
		{
			desiredLightState = STATES;
		}

		// setup common states
		if( lastLightState == NONE || desiredLightState == STATES_SM )
		{
			// Common VS consts
			{
				float vScreenT[4] = { 0.5f, -0.5f, 0.5f + 0.5f / r3dRenderer->ScreenW, 0.5f + 0.5f / r3dRenderer->ScreenH };
				// float4 vScreenT : register( c24 );
				r3dRenderer->pd3ddev->SetVertexShaderConstantF( 24, vScreenT, 1 );
			}

			r3dRenderer->SetVertexShader( "VS_DS_POINTLIGHT" );

			r3dRenderer->SetTex( gBuffer_Color->Tex			, 0 );
			r3dRenderer->SetTex( gBuffer_Normal->Tex		, 1 );
			r3dRenderer->SetTex( gBuffer_Depth->Tex			, 2 );
			r3dRenderer->SetTex( gBuffer_Aux->Tex			, 3 );

			r3dSetFiltering( R3D_POINT, 0 );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			if (r_use_ssr->GetInt())
			{
				r3dSetFiltering( R3D_BILINEAR, 7 );

				r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
				r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
				r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSW,   D3DTADDRESS_CLAMP );

				r3dRenderer->SetTex( gBuffer_SSR.gBuffer_SSR->Tex, 7 );


				float maxLod = gBuffer_SSR.gBuffer_SSR->Tex->GetNumMipmaps();
				float lodFactor = 2.0f + ((float)(r_ssr_mips_factor->GetInt()))/50.0f;
				float ssr_setup[2][4]  = {
					{1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetWidth(), 1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetHeight(), 0, -1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetWidth() },
					{ maxLod, lodFactor, 0.0f, 0.0f}
				};

				D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 51, ssr_setup[0], 2 ) ) ;

			}

			r3dMaterialLibrary::SetupExtendedMaterialParameters();

			r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
			r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );


			r3dDrawGeoSpheresStart();
		}

		lastLightState = desiredLightState;

		// setup varying states
		bool ShadowMap = desiredLightState == STATES_SM;
		float smoothSpread = 4.f / SharedShadowCubeMap->Width;

		if( ShadowMap )
		{
			float scale = r3dGetLightSMDepthCoef( l ) ;

			float vConst[4] = { smoothSpread, scale, 0, 0 };
			// float4  vMultiShadowParms   : register(c7);
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 7, vConst, 1 ) );

			if( do_shadow_blur )
			{
				r3dRenderer->SetTex( TempShadowBuffer->Tex, 4 );
			}
			else
			{
				float shadowDim = 512.f;

				if( l->IsShadowFrozen() )
				{
					if( l->ShadowDepth )
					{
						shadowDim = l->ShadowDepth->GetWidth();
						r3dRenderer->SetTex( l->ShadowDepth, 4 );
					}
					else
					{
						r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE], 4 );
					}
				}
				else
				{
					r3dSetFiltering( R3D_POINT, 4 );

#ifndef FINAL_BUILD
					if( r_parabolo_pointlight_shadows->GetInt() )
					{
						shadowDim = SharedShadowMap->Width;
						r3dRenderer->SetTex( SharedShadowMap->Tex, 4 );
					}
					else
#endif
					{
						shadowDim = SharedShadowCubeMap->Width;
						r3dRenderer->SetTex( SharedShadowCubeMap->Tex, 4 );
					}
				}

				float coef = 2.0f;

				float psConst[ 4 ] = { shadowDim / coef, coef / shadowDim, 0, 0 };
				// float4     vShadowMapRes        : register(c13);
				r3dRenderer->pd3ddev->SetPixelShaderConstantF( 13, psConst, 1 );
			}
		}

		PointLightPShaderId psid ;

		psid.ssao = r_lighting_quality->GetInt() == 1 ? 1 : 0 ;
		psid.quality = r_lighting_quality->GetInt() - r_lighting_quality->GetMinVal();
		psid.diffuse_only = !!l->bDiffuseOnly ;
		if( ShadowMap )
		{
			if( do_shadow_blur )
				psid.shadow_mode = 3;
			else
			{
#ifndef FINAL_BUILD
				if( r_parabolo_pointlight_shadows->GetInt() )
				{
					r3dCamera paraCamera;
					SetupCameraForParaboloidFace( &paraCamera, l, 0 );

					D3DXMATRIX view;
					r3dRenderer->BuildViewMtx( paraCamera, &view, NULL );

					// float4x3   vLightViewMtx        : register(c14);
					float psConsts[ 4 ][ 4 ];
					D3DXMatrixTranspose( (D3DXMATRIX*)psConsts, &view );
					// float4     vParaboloidParams    : register(c17);
					FillParaboloidVertexConstant( psConsts[ 3 ], &paraCamera );
					r3dRenderer->pd3ddev->SetPixelShaderConstantF( 14, psConsts[ 0 ], R3D_ARRAYSIZE( psConsts ) );

					psid.shadow_mode = 4;
				}
				else
#endif
				{
					if( r_shadows_quality->GetInt() > 2 )
					{
						psid.shadow_mode = 2;
					}
					else
					{
						psid.shadow_mode = 1;
					}
				}
			}
		}

		psid.sss = !!l->SSScatParams.On ;
		psid.proj_texture = !!l->ProjectMap ;
		psid.ssr = r_use_ssr->GetInt() > 0 ;

		r3dRenderer->SetPixelShader( gPointLightPSIds[ psid.Id ] ) ;

		if( l->ProjectMap )
		{
			// TODO : inverse light rotation when rotation is
			// available for lights
			float vConst[3][4] = {
				// float3x3 mLightMtx			: register(c4);
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
			};

			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 4, vConst[0], 3 ) );

			r3dRenderer->SetTex( l->ProjectMap, 6 );
		}

		SetSSSParams( 9, false, l->SSScatParams ) ;

		// grow the sphere a lil' cause its shape is not perfect
		float extraRad = l->GetOuterRadius() * 1.05f;

		// check against a bigger sphere so that we don't
		// turn z check off too late when we fly into the sphere
		// ( a part of lit area may be omitted due to z clipping )
		bool bInLight = (*l-gCam).Length() <= extraRad * 1.05f + camPadding;
		if(!bInLight)
		{
			r3dRenderer->SetCullMode( D3DCULL_CCW );
			r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_ZC);
		}
		else
		{
			r3dRenderer->SetCullMode( D3DCULL_CW );
			r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_NZ);
		}

		// Now Draw light pass for each object using shadow mask
		l->SetShaderConstants(gCam);

		D3DXMATRIX scale, transf;

		D3DXMatrixScaling( &scale, extraRad, extraRad, extraRad );
		D3DXMatrixTranslation( &transf, l->X, l->Y, l->Z );
		D3DXMatrixMultiply( &transf, &scale, &transf );

		r3dMeshSetShaderConsts( transf, NULL );

		r3dDrawGeoSphere();
	} // FOR EACH LIGHT

	if( lastLightState != NONE )
	{
		r3dDrawGeoSpheresEnd();
	}

	r3dRenderer->RestoreCullMode();

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	D3DPERF_EndEvent ();
}

void Render_Deffered_Planelights()
{
#ifndef FINAL_BUILD
	if( r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AMBIENT )
		return;
#endif

	if(r_lighting_quality->GetInt()==r_lighting_quality->GetMinVal())
		return;

	R3DPROFILE_FUNCTION("DR: Planelights");
	D3DPERF_BeginEvent ( 0x0, L"Planelights" );

	r3dRenderer->Flush();

	float camPadding = GetCamPadding();

	DesiredLightState desiredLightState, lastLightState = NONE;

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if ( !l || l->Type != R3D_PLANE_LIGHT ) continue;

		l->ProcessLightFunction(r3dGetFrameTime());

		if(!l->IsOn()) continue;

		l->RecalcBoundBox();

		bool do_shadow_blur = l->bSSShadowBlur && r_shadow_blur->GetInt() ;

		if (!r3dRenderer->IsBoxInsideFrustum(l->BBox)) continue;

		float VIS_RANGE = RenderDistance + l->GetVisibilityRadius();
		r3dVector V = gCam - (r3dPoint3D)*l;
		if (V.Length() > VIS_RANGE) continue;

		if( l->bCastShadows && ((V.Length()-l->GetOuterRadius())<r_shadow_light_source_render_dist->GetFloat()) && r_shadows->GetBool() )
		{
			r3dRenderer->SetZRange( R3D_ZRANGE_WHOLE ) ;
			RenderCubemapShadowmap( l, desiredLightState, lastLightState, false );
			r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;
		}
		else
		{
			desiredLightState = STATES;
		}

		// setup common states
		if( lastLightState == NONE || desiredLightState == STATES_SM )
		{
			// Common VS consts
			{
				float vScreenT[4] = { 0.5f, -0.5f, 0.5f + 0.5f / r3dRenderer->ScreenW, 0.5f + 0.5f / r3dRenderer->ScreenH };
				// float4 vScreenT : register( c24 );
				r3dRenderer->pd3ddev->SetVertexShaderConstantF( 24, vScreenT, 1 );
			}

			r3dRenderer->SetVertexShader( "VS_DS_POINTLIGHT" );

			r3dRenderer->SetTex( gBuffer_Color->Tex			, 0 );
			r3dRenderer->SetTex( gBuffer_Normal->Tex		, 1 );
			r3dRenderer->SetTex( gBuffer_Depth->Tex			, 2 );
			r3dRenderer->SetTex( gBuffer_Aux->Tex			, 3 );

			r3dSetFiltering( R3D_POINT, 0 );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			r3dDrawChamferBoxStart();
		}

		lastLightState = desiredLightState;

		// setup varying states
		float smoothSpread = 4.f / SharedShadowCubeMap->Width;
		bool ShadowMap = desiredLightState == STATES_SM;

		if( ShadowMap )
		{
			if( do_shadow_blur )
			{
				r3dRenderer->SetTex( TempShadowBuffer->Tex, 4 );
			}
			else
			{
				r3dRenderer->SetTex( SharedShadowCubeMap->Tex, 4 );
			}
		}


		SetSSSParams(11,false,l->SSScatParams);
		if( ShadowMap )
		{
			float vConst[4] = { smoothSpread, 0, 0, 0 };
			// float4  vMultiShadowParms   : register(c10);
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 10, vConst, 1 ) );

			if( do_shadow_blur )
			{
				r3dRenderer->SetPixelShader( DS_PLANELIGHT_SSM_ID );
			}
			else
			{
				r3dRenderer->SetPixelShader( DS_PLANELIGHT_SM_ID );
			}
		}
		else
		{
			r3dRenderer->SetPixelShader( DS_PLANELIGHT_ID );
		}

		// grow the sphere a lil' cause its shape is not perfect
		float visRad = l->GetVisibilityRadius() * 1.05f;

		// check against a bigger sphere so that we don't
		// turn z check off too late when we fly into the sphere
		// ( a part of lit area may be omitted due to z clipping )
		bool bInLight = (*l-gCam).Length() <= visRad * 1.05f + camPadding;
		if(!bInLight)
		{
			r3dRenderer->SetCullMode( D3DCULL_CCW );
			r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_ZC);
		}
		else
		{
			r3dRenderer->SetCullMode( D3DCULL_CW );
			r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_NZ);
		}

		// Now Draw light pass for each object using shadow mask
		l->SetShaderConstants(gCam);

		D3DXMATRIX lm ;

		l->GetPlaneWorldMatrix( &lm, true ) ;
		r3dMeshSetShaderConsts( lm, NULL );

		r3dDrawChamferBox();
	} // FOR EACH LIGHT

	if( lastLightState != NONE )
	{
		r3dDrawChamferBoxEnd();
	}

	r3dRenderer->RestoreCullMode();

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	D3DPERF_EndEvent ();
}

void Render_Deffered_Tubelights()
{
#ifndef FINAL_BUILD
	if( r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AMBIENT )
		return;
#endif

	if(r_lighting_quality->GetInt()==r_lighting_quality->GetMinVal())
		return;

	R3DPROFILE_FUNCTION("DR: Tubelights");
	D3DPERF_BeginEvent ( 0x0, L"Tubelights" );

	r3dRenderer->Flush();

	float camPadding = GetCamPadding();

	DesiredLightState desiredLightState, lastLightState = NONE;

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || l->Type != R3D_TUBE_LIGHT ) continue;

		l->ProcessLightFunction(r3dGetFrameTime());

		if(!l->IsOn()) continue;

		l->RecalcBoundBox();
		if (!r3dRenderer->IsBoxInsideFrustum(l->BBox)) continue;

		bool do_shadow_blur = l->bSSShadowBlur && r_shadow_blur->GetInt() ;

		float VIS_RANGE = RenderDistance + l->GetVisibilityRadius();
		r3dVector V = gCam - (r3dPoint3D)*l;
		if (V.Length() > VIS_RANGE) continue;

		if( l->bCastShadows && ((V.Length()-l->GetOuterRadius())<r_shadow_light_source_render_dist->GetFloat()) && r_shadows->GetBool() )
		{
			r3dRenderer->SetZRange( R3D_ZRANGE_WHOLE ) ;
			RenderCubemapShadowmap( l, desiredLightState, lastLightState, false );
			r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;
		}
		else
		{
			desiredLightState = STATES;
		}

		// setup common states
		if( lastLightState == NONE || desiredLightState == STATES_SM )
		{
			// Common VS consts
			{
				float vScreenT[4] = { 0.5f, -0.5f, 0.5f + 0.5f / r3dRenderer->ScreenW, 0.5f + 0.5f / r3dRenderer->ScreenH };
				// float4 vScreenT : register( c24 );
				r3dRenderer->pd3ddev->SetVertexShaderConstantF( 24, vScreenT, 1 );
			}

			r3dRenderer->SetVertexShader( "VS_DS_POINTLIGHT" );

			r3dRenderer->SetTex( gBuffer_Color->Tex			, 0 );
			r3dRenderer->SetTex( gBuffer_Normal->Tex		, 1 );
			r3dRenderer->SetTex( gBuffer_Depth->Tex			, 2 );
			r3dRenderer->SetTex( gBuffer_Aux->Tex			, 3 );

			r3dSetFiltering( R3D_POINT, 0 );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			r3dDrawGeoSpheresStart();
		}

		lastLightState = desiredLightState;

		// setup varying states
		float smoothSpread = 4.f / SharedShadowCubeMap->Width;
		bool ShadowMap = desiredLightState == STATES_SM;

		if( ShadowMap )
		{
			if( do_shadow_blur )
			{
				r3dRenderer->SetTex( TempShadowBuffer->Tex, 4 );
			}
			else
			{
				r3dRenderer->SetTex( SharedShadowCubeMap->Tex, 4 );
			}
		}


		SetSSSParams(11, false,l->SSScatParams);
		if( ShadowMap )
		{
			float vConst[4] = { smoothSpread, 0, 0, 0 };
			// float4  vMultiShadowParms   : register(c10);
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 10, vConst, 1 ) );

			if( do_shadow_blur )
			{
				r3dRenderer->SetPixelShader( DS_TUBELIGHT_SSM_ID );
			}
			else
			{
				r3dRenderer->SetPixelShader( DS_TUBELIGHT_SM_ID );
			}
		}
		else
		{
			r3dRenderer->SetPixelShader( DS_TUBELIGHT_ID );
		}


		// grow the sphere a lil' cause its shape is not perfect
		float visRad = l->GetVisibilityRadius() * 1.05f;

		// check against a bigger sphere so that we don't
		// turn z check off too late when we fly into the sphere
		// ( a part of lit area may be omitted due to z clipping )
		bool bInLight = (*l-gCam).Length() <= visRad * 1.05f + camPadding;
		if(!bInLight)
		{
			r3dRenderer->SetCullMode( D3DCULL_CCW );
			r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_ZC);
		}
		else
		{
			r3dRenderer->SetCullMode( D3DCULL_CW );
			r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_NZ);
		}

		// Now Draw light pass for each object using shadow mask
		l->SetShaderConstants(gCam);

		D3DXMATRIX lm ;

		l->GetTubeWorldMatrix( &lm, true ) ;
		r3dMeshSetShaderConsts( lm, NULL );

		r3dDrawGeoSphere();
	} // FOR EACH LIGHT

	if( lastLightState != NONE )
	{
		r3dDrawGeoSpheresEnd();
	}

	r3dRenderer->RestoreCullMode();

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	D3DPERF_EndEvent ();
}

void SetupSpotLightMatrices( r3dLight* l, D3DXMATRIX* oLightView, r3dCamera* oLightCam )
{
	D3DXMATRIX& lightView = *oLightView ;
	r3dComposeLightMatrix( lightView, l );

	D3DXMATRIX lightProj;
	float fov = l->SpotAngleOuter * 2;
	float lightFar = l->GetOuterRadius() * 2;
	float lightNear = 0.05f;

	r3dCamera& lightCam = *oLightCam ;

	(r3dPoint3D&)lightCam	= *l;

	lightCam.NearClip	= lightNear;
	lightCam.FarClip	= lightFar;

	lightCam.vPointTo	= r3dVector( lightView.m[2][0], lightView.m[2][1], lightView.m[2][2] );
	lightCam.vUP		= r3dVector( lightView.m[1][0], lightView.m[1][1], lightView.m[1][2] );

	// perspective projection matrix
	lightCam.FOV		= fov;

	// orthographic projection matrix
	lightCam.ProjectionType	= r3dCamera::PROJTYPE_PRESPECTIVE;
	lightCam.Width			= 0.f;
	lightCam.Height			= 0.f;
	lightCam.Aspect			= 1.0f;
}

static float spotProjTexMessageTime = 0;

void Render_Deffered_Spotlights()
{
#ifndef FINAL_BUILD
	if( r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AMBIENT )
		return;
#endif
	R3DPROFILE_FUNCTION("DR: Spotlights");
	D3DPERF_BeginEvent( 0, L"Spotlights" );

	r3dRenderer->Flush();


	D3DXMATRIX 	mWorld;
	D3DXMATRIX ShaderMat;
	D3DXMATRIX InvShaderMat;
	D3DXMATRIX WVP;

	D3DXMatrixIdentity(&mWorld);

	float camPadding = GetCamPadding();

	r3dSetFiltering( R3D_BILINEAR, 0 );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	enum
	{
		NONE,
		STATES,
		STATES_SM
	} desiredLightState, lastLightState = NONE;

	bool castShadows = r_lighting_quality->GetInt() > 1 ;

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || l->Type != R3D_SPOT_LIGHT ) continue; //l->OccluderID = -1;

		l->ProcessLightFunction(r3dGetFrameTime());

		if(!l->IsOn()) continue;

		l->RecalcBoundBox();
		if (!r3dRenderer->IsBoxInsideFrustum(l->BBox)) continue;

		float VIS_RANGE = RenderDistance + l->GetOuterRadius();
		r3dVector V = gCam - (r3dPoint3D)*l;
		if (V.Length() > VIS_RANGE) continue;

		float shadowMapCoef = 0.f ;

		if( r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF && !l->IsShadowFrozen() )
		{
			float sharZBufSize = r3dScreenBuffer::GetShadowZBufSize() ;
			shadowMapCoef = SharedShadowMap->Width / sharZBufSize ;
		}
		else
		{
			shadowMapCoef = 1.0f ;
		}

		bool do_shadow_blur = l->bSSShadowBlur && r_hardware_shadow_method->GetInt() != HW_SHADOW_METHOD_HW_PCF && r_shadow_blur->GetInt() ;

		int needShadows = castShadows && l->bCastShadows  && ((V.Length()-l->GetOuterRadius())<r_shadow_light_source_render_dist->GetFloat()) && r_shadows->GetBool();
		int needProjTex = l->ProjectMap && r_lighting_quality->GetInt() != 1 ? 1 : 0 ;

#ifndef FINAL_BUILD
		if( g_bEditMode && l->ProjectMap && l->ProjectMap->isCubemap() )
		{
			if( r3dGetTime() - spotProjTexMessageTime > 45 )
			{
				char text[ 512 ];
				sprintf( text, "Please change projection texture of SPOT light at [%.1f  %.1f  %.1f] to be 2D Texture and not Cube Texture", l->x, l->y, l->z );
				MessageBoxA( r3dRenderer->HLibWin, text, "Suggestion", MB_OK );

				spotProjTexMessageTime = r3dGetTime();
			}
		}
#endif

		D3DXMATRIX lightView;
		D3DXMATRIX lightMtx;
		D3DXMATRIX lightP;
		r3dCamera lightCam;

		if( needShadows || needProjTex )
		{
			// block with forced normal direction in zbuffer
			{
				SetRestoreZDir setRestoreZDir( r3dRenderLayer::ZDIR_NORMAL ); (void)setRestoreZDir;

				SetupSpotLightMatrices( l, &lightView, &lightCam );
				r3dRenderer->SetCamera( lightCam, false );
				lightMtx = r3dRenderer->ViewProjMatrix;
				lightP = r3dRenderer->ProjMatrix;
			}

			r3dRenderer->SetCamera( gCam, true );
		}

		if( needShadows )
		{
			SetRestoreRender2DStates setRestoreRender2DStates; (void)setRestoreRender2DStates;

			if( lastLightState != NONE )
			{
				r3dDrawConesEnd();
			}

			ScreenBuffer->Deactivate();

			if( !l->IsShadowFrozen() )
			{
				// block with forced normal direction in zbuffer
				{
					SetRestoreZDir setRestoreZDir( r3dRenderLayer::ZDIR_NORMAL ); (void)setRestoreZDir;

					SharedShadowMap->Activate();
					float fLightFar = l->GetOuterRadius();
					D3D_V( r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) );

					// this is for ClearFloatSurface
					r3dRenderer->SetCullMode( D3DCULL_CCW );

					if( r_hardware_shadow_method->GetInt() != HW_SHADOW_METHOD_HW_PCF )
					{
						ClearFloatSurface( D3DXVECTOR4( fLightFar, fLightFar, fLightFar, fLightFar ) );
					}

					r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

					D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) );

					r3dRenderer->SetZRange( R3D_ZRANGE_WHOLE ) ;
					RenderShadowMap( lightCam, false, false );
					r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;

					SharedShadowMap->Deactivate(0);
				}
				r3dRenderer->SetCamera( gCam, true );
			}

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED |
																					D3DCOLORWRITEENABLE_GREEN |
																					D3DCOLORWRITEENABLE_BLUE |
																					D3DCOLORWRITEENABLE_ALPHA ) );

			if( do_shadow_blur )
			{
				BlurPosLightShadow( l, lightMtx, lightP );
			}

			ScreenBuffer->Activate();

			desiredLightState	= STATES_SM;
		}
		else
		{
			desiredLightState	= STATES;
		}

		if( lastLightState == NONE || desiredLightState == STATES_SM )
		{
			r3dRenderer->SetVertexShader( VS_SPOT_ID );

			// Common VS consts
			{
				float vScreenT[4] = { 0.5f, -0.5f, 0.5f + 0.5f / r3dRenderer->ScreenW, 0.5f + 0.5f / r3dRenderer->ScreenH };
				// float4 vScreenT : register( c24 );
				r3dRenderer->pd3ddev->SetVertexShaderConstantF( 24, vScreenT, 1 );
			}

			r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_NZ);

			r3dRenderer->SetTex( gBuffer_Color->Tex		, 0 );
			r3dRenderer->SetTex( gBuffer_Normal->Tex	, 1 );
			r3dRenderer->SetTex( gBuffer_Depth->Tex		, 2 );
			r3dRenderer->SetTex( gBuffer_Aux->Tex		, 3 );

			r3dSetFiltering( R3D_POINT, 0 );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			if (r_use_ssr->GetInt())
			{
				r3dSetFiltering( R3D_BILINEAR, 7 );

				r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
				r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
				r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSW,   D3DTADDRESS_CLAMP );

				r3dRenderer->SetTex( gBuffer_SSR.gBuffer_SSR->Tex, 7 );


				float maxLod = gBuffer_SSR.gBuffer_SSR->Tex->GetNumMipmaps();
				float lodFactor = 2.0f + ((float)(r_ssr_mips_factor->GetInt()))/50.0f;
				float ssr_setup[2][4]  = {
					{1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetWidth(), 1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetHeight(), 0, -1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetWidth() },
					{ maxLod, lodFactor, 0.0f, 0.0f}
				};

				D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 51, ssr_setup[0], 2 ) ) ;

			}

			r3dMaterialLibrary::SetupExtendedMaterialParameters();

			r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
			r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

			r3dDrawConesStart();
		}

		lastLightState = desiredLightState;

		SpotLightPShaderId psid ;

		psid.aux_enabled = r_lighting_quality->GetInt() != 1 ;
		psid.proj_texture = needProjTex;

		if( desiredLightState == STATES_SM )
		{
			if( do_shadow_blur )
				psid.shadow_mode = 3 ;
			else
			{
				if( r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF
					&&
					!l->IsShadowFrozen() )
				{
					psid.shadow_mode = 2 ;
				}
				else
				{
					psid.shadow_mode = 1 ;
				}
			}
		}
		else
		{
			psid.shadow_mode = 0 ;
		}

		psid.quality = r_lighting_quality->GetInt() - r_lighting_quality->GetMinVal();

		r3dRenderer->SetPixelShader( gSpotLightPSIds[ psid.Id ] ) ;

		if( r_lighting_quality->GetInt() > 1 )
		{
			if( l->ProjectMap )
			{
				r3dRenderer->SetTex( l->ProjectMap, 6 );
			}

			D3DXVECTOR4 vConsts[ 9 ];

			int vConstIdx = 0;

			// float4 vUnused              : register(c6);
			vConsts[ vConstIdx ++ ] = D3DXVECTOR4( 0.f, 0.f, 0.f, 0.f );

			if( desiredLightState == STATES_SM || l->ProjectMap )
			{
				// float4x4 mLightMtx          : register(c7);
				D3DXMatrixTranspose( (D3DXMATRIX*)&vConsts [ vConstIdx ] , &lightMtx );
				vConstIdx += 4;
			}

			if( desiredLightState == STATES_SM )
			{
				float shmapWidth, shmapHeight ;

				if( l->IsShadowFrozen() )
				{
					shmapWidth	= l->ShadowDepth->GetWidth() ;
					shmapHeight	= l->ShadowDepth->GetHeight() ;
				}
				else
				{
					shmapWidth	= SharedShadowMap->Width ;
					shmapHeight	= SharedShadowMap->Height ;
				}

				float bias = 0.f;

				if( psid.shadow_mode == 2 )
				{
					bias = r_spot_light_shadow_bias_hw->GetFloat();
				}
				else
				{
					bias = r_spot_light_shadow_bias_pcf->GetFloat();
				}

				// float4 vPCFSettings         : register(c11);
				vConsts[ vConstIdx ++ ] = D3DXVECTOR4( shmapWidth, shmapHeight, 1.f / shmapWidth, 1.f / shmapHeight );
				// float4 vPCFSteps0           : register(c12);
				vConsts[ vConstIdx ++ ] = D3DXVECTOR4( 1.f / shmapWidth, 0.f, 0.f, 1.f / shmapHeight );
				// float4 vPCFSteps1_Scale_Bia : register(c13);
				vConsts[ vConstIdx ++ ] = D3DXVECTOR4( 1.f / shmapWidth, 1.f / shmapHeight, r3dGetLightSMDepthCoef( l ), bias );
				// float4 SM_Scale             : register(c14);
				vConsts[ vConstIdx ++ ] = D3DXVECTOR4( 0.5f * shadowMapCoef, -0.5f * shadowMapCoef, 0.5f * shadowMapCoef, 0.5f * shadowMapCoef );

				r3d_assert( vConstIdx == sizeof vConsts / sizeof vConsts[0] );

				if( do_shadow_blur )
				{
					r3dRenderer->SetTex( TempShadowBuffer->Tex, 4 );
				}
				else
				{
					if( l->IsShadowFrozen() )
					{
						r3dRenderer->SetTex( l->ShadowDepth, 4 );
					}
					else
					{
						r3dRenderer->SetTex( SharedShadowMap->Tex, 4 );
					}
				}
				if( !do_shadow_blur && r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF )
				{
					r3dSetFiltering( R3D_BILINEAR, 4 );
				}
				else
				{
					r3dSetFiltering( R3D_POINT, 4 );
				}
			}
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 6, (float*)&vConsts, vConstIdx ) );
		}

		SetSSSParams(15, false,l->SSScatParams);

		l->SetShaderConstants(gCam);

		D3DXMATRIX lmatrix, lmatrix_inv;
		r3dComposeLightMatrix( lmatrix, l );

		r3dOrthoInverse( lmatrix_inv, lmatrix );

		D3DXVECTOR3 camPos( gCam.x, gCam.y, gCam.z );
		D3DXVec3TransformCoord( &camPos, &camPos, &lmatrix_inv );

		float radius, zScale;
		r3dGetSpotLightScaleAttribs( radius, zScale, l );

		D3DXMATRIX smtx;
		r3dGetSpotLightScaleMatrix( smtx, radius, zScale );

		D3DXMatrixMultiply( &lmatrix, &smtx, &lmatrix );

		r3dMeshSetShaderConsts( lmatrix, NULL );

		bool insideLight = IsInsideCone( radius, camPadding, zScale, r3dPoint3D( camPos.x, camPos.y, camPos.z ) );

		if( !insideLight )
		{
			r3dRenderer->SetCullMode( D3DCULL_CCW );
			r3dRenderer->SetRenderingMode( R3D_BLEND_ADD | R3D_BLEND_ZC );
		}
		else
		{
			r3dRenderer->SetCullMode( D3DCULL_CW );
			r3dRenderer->SetRenderingMode( R3D_BLEND_ADD | R3D_BLEND_NZ );
		}

		r3dDrawCone();

	} // FOR EACH LIGHT

	if( lastLightState != NONE )
	{
		r3dDrawConesEnd();
	}

	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	r3dRenderer->RestoreCullMode();

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	D3DPERF_EndEvent();
}

//////////////////////////////////////////////////////////////////////////

void Render_Deffered_Volume_Lights()
{
#ifndef FINAL_BUILD
	if( r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AMBIENT )
		return;
#endif
	R3DPROFILE_FUNCTION("DR: Volume lights");
	D3DPERF_BeginEvent ( 0x0, L"Volume lights" );

	r3dRenderer->Flush();

	float camPadding = GetCamPadding();
	DWORD prevStencil = 0;
	r3dRenderer->pd3ddev->GetRenderState( D3DRS_STENCILENABLE, &prevStencil );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	// VS constants
	{
		D3DXMATRIX ShaderMat;

		ShaderMat =  r3dRenderer->ViewProjMatrix;
		D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

		// float4x4 mProj 	: register(c0);
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

		float vScreenT[4] = { 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, 1.0f, 1.0f};
		// float4 vScreenT : register( c24 );
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 24, vScreenT, 1 );
	}

	r3dRenderer->SetVertexShader("VS_DS_DIRLIGHT");
	r3dRenderer->SetPixelShader( PS_VOLUME_POINTLIGHT_ID );

	r3dRenderer->SetTex( gBuffer_Color->Tex			, 0 );
	r3dRenderer->SetTex( gBuffer_Normal->Tex		, 1 );
	r3dRenderer->SetTex( gBuffer_Depth->Tex			, 2 );
	r3dRenderer->SetTex( gBuffer_Aux->Tex			, 3 );

	r3dSetFiltering( R3D_POINT, 0 );
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	float DepthZ = r3dRenderer->FarClip * 0.9375f;
	r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_NZ);

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.Lights[i];
		if (!l || l->Type != R3D_VOLUME_LIGHT) continue;

		l->ProcessLightFunction(r3dGetFrameTime());

		if(!l->IsOn()) continue;

		l->SetRadius(l->Radius1, 1.0f / pow(l->Intensity, 0.6f) * 20);
		l->RecalcBoundBox();
		if (!r3dRenderer->IsBoxInsideFrustum(l->BBox)) continue;

		float VIS_RANGE = RenderDistance + l->GetOuterRadius();
		r3dVector V = gCam - (r3dPoint3D)*l;
		if (V.Length() > VIS_RANGE) continue;

		l->SetShaderConstants(gCam);

		r3dDrawBox2DZ(0,0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, DepthZ, r3dColor(0, 0, 0));
	} // FOR EACH LIGHT

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, prevStencil );

	r3dRenderer->RestoreCullMode();

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	D3DPERF_EndEvent ();
}

//////////////////////////////////////////////////////////////////////////

// calculate split distances for shadows
void CalculateSplitDistances()
{
	// Practical split scheme:
	// CLi = n*(f/n)^(i/numsplits)
	// CUi = n + (f-n)*(i/numsplits)
	// Ci = CLi*(lambda) + CUi*(1-lambda)
	//
	// lambda scales between logarithmic and uniform
/*	ShadowSplitLambda=R3D_CLAMP(ShadowSplitLambda,0.0f,1.0f);

	for(int i=0;i<NumShadowSlices;++i)
	{
		float fIDM=i/(float)NumShadowSlices;
		float fLog=ShadowCameraNear*powf((ShadowDrawingDistance/ShadowCameraNear),fIDM);
		float fUniform=ShadowCameraNear+(ShadowDrawingDistance-ShadowCameraNear)*fIDM;
		ShadowSplitDistances[i]=fLog*ShadowSplitLambda+fUniform*(1-ShadowSplitLambda);
	}
*/
	// make sure border values are right
	ShadowSplitDistancesOpaque[0]=ShadowCameraNear;
	ShadowSplitDistancesOpaque[NumShadowSlices]=ShadowDrawingDistance;
}

void WarmUpTextures()
{
	r3dRenderer->SetPixelShader();
	r3dRenderer->SetVertexShader();
	{
		for( r3dTexture* Tex = r3dRenderer->FirstTexture; Tex; Tex = Tex->pNext )
		{
			if(Tex->GetDepth() == 1 && !Tex->isCubemap()) // cannot render volume or cubetexture in r3dDrawBox2D, dx error
				r3dDrawBox2D( 0.f, 0.f, 9.f, 9.f, r3dColor::white, Tex, NULL );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void ActivateTransparentBuffer()
{
	if (gBuffer_Particles)
		gBuffer_Particles->Activate();
	else
		ScreenBuffer->Activate();
}

void DeactivateTransparentBuffer( )
{
	if (gBuffer_Particles)
	{
		gBuffer_Particles->Deactivate();
	}
	else
	{
		ScreenBuffer->Deactivate() ;
	}
}

void ActivateDistortBuffer()
{
	if( r_distort->GetInt() )
	{
		DistortionBuffer->Activate(1);
	}
}

void DeactivateDistortBuffer()
{
	if( r_distort->GetInt() )
	{
		DistortionBuffer->Deactivate();
	}
}

void DrawTansparentObjects()
{
	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Render Transparent Objects");
	R3DPROFILE_START("DR: Render Transparent");

	// setup particle parameters
	{
		const ShadowSlice& lastSlice = ShadowSlices[ r_active_shadow_slices->GetInt() - 1 ] ;

		D3DXMATRIX texMat ;

		SetupShadowTexMatrix( &texMat, LFUpdateShadowMap, lastSlice.shadowMapSize, IsRecticularWarpActive() ? 0 : 1 ) ;

		D3DXMATRIX compoundMtx = lastSlice.lightView * lastSlice.lightProj * texMat ;

		gPFX_ScopeEffect.SetLightMatrix( compoundMtx ) ;

		r3dSetParticleVars( Sun->SunLight.Direction, LFUpdateShadowMap, DistortionBuffer, compoundMtx ) ;
	}

	if( r_half_depth_particles->GetBool() )
	{
		if( r_half_depth_par_strerect->GetBool() )
		{
			r3dRenderer->StretchRect( gBuffer_Depth, gBuffer_HalvedR32F_0, 0 );
		}
		else
		{
			CopySurface( gBuffer_Depth, gBuffer_HalvedR32F_0 );
		}
	}

	D3DXVECTOR4 DepthPixelSize = D3DXVECTOR4(1.0f/gBuffer_Depth->Width, 1.0f/gBuffer_Depth->Height, 0, 0 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_PIXELSIZE, (float*)&DepthPixelSize, 1 ) ;

	if( r_distort->GetInt() )
	{
		DistortionBuffer->Activate();
		D3D_V( r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255,127,127,0), r3dRenderer->GetClearZValue(), 0 ) );
		DistortionBuffer->Deactivate();
	}

	ActivateTransparentBuffer();

	r3dRenderer->StartRender(0);

	if (gBuffer_Particles)
	{
		r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), r3dRenderer->GetClearZValue(), 0 );
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);
	}
	else
	{
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
		r3dSetFiltering( R3D_BILINEAR );
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_ZENABLE,       	0);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	 	D3DBLEND_ONE);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_DESTBLEND, 		D3DBLEND_INVSRCALPHA);
	}

	//r3dDrawBox2D(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(0,0,0,0), gBuffer_Particles->Tex);

	void SetParticleDepth();
	SetParticleDepth();

	r3dSetFiltering( R3D_BILINEAR );
	r3dSetFiltering( R3D_POINT, 1 );

	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );

	{
		// float4   WorldScale  		: register( c24 );
		D3DXVECTOR4 unity( 1.f, 1.f, 1.f, 1.f ) ;
		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 24, (float*)&unity, 1 ) ) ;
	}

	r3dRenderer->SetFog(0);
	GameWorld().Draw( rsDrawTransparents );
	r3dRenderer->SetFog(1);

	r3dRenderer->SetTex(gBuffer_Depth->Tex,1);

	R3DPROFILE_END("DR: Render Transparent");
	D3DPERF_EndEvent();

	r3dRenderer->EndRender();

	DeactivateTransparentBuffer() ;
	r3dTransparentDistortFlush();

}

//////////////////////////////////////////////////////////////////////////

void ApplyTemporalSSAOFilters( r3dScreenBuffer* ssaoRT ) ;
void Render_SSR_Reflections();

//------------------------------------------------------------------------

void r3dDefferedRenderer::RenderShadows( r3dScreenBuffer* depthBuffer )
{
	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Create Shadowmap");
	R3DPROFILE_START("DR: Create Shadowmap");

	CalculateSplitDistances();

	TempShadowBuffer->Activate();
	D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET,0xffffffff, r3dRenderer->GetClearZValue(), 0 ) );
	TempShadowBuffer->Deactivate();

	// NOTE : important to go from least detailed map
	// so that it doesn't overwrite with whiteness...

	if( r_shadows->GetBool() && r3dGameLevel::Environment->SunLightOn )
	{
		int dipsBefore = r3dRenderer->Stats.GetNumDraws();

		SetRestoreZDir setRestoreZDir( r3dRenderLayer::ZDIR_NORMAL ); (void)setRestoreZDir;

		r3dRenderer->SetZRange( R3D_ZRANGE_WHOLE );

		int NeedUpdateCachedSlice();
		if( r_active_shadow_slices->GetInt() != 1 || NeedUpdateCachedSlice() )
		{
			PrepareSlicedShadowMapRender();
		}

		bool makeFirstSliceWarped = IsRecticularWarpActive() ? true : false ;

		for( int i = r_active_shadow_slices->GetInt() - 1; i >= r_min_shadow_slice->GetInt(); --i )
		{
			RenderShadowMap( ShadowSlices[i], depthBuffer, makeFirstSliceWarped );
			makeFirstSliceWarped = false;
		}

		if( r_shadow_blur->GetBool() )
		{
			r3dSSSBParams parms;

			parms.PhysRange = r_shadows_blur_phys_range->GetFloat();
			parms.Bias		= r_shadows_blur_bias->GetFloat();
			parms.Sense		= r_shadows_blur_sense->GetFloat();
			parms.Radius	= r_shadows_blur_radius->GetFloat();

			BlurShadowMap( TempShadowBuffer, TempScreenBuffer, -1.f, -1.f, 1.f, 1.f, parms );
		}

		int dipDiff = r3dRenderer->Stats.GetNumDraws() - dipsBefore;
		r3dRenderer->Stats.AddNumShadowDips( dipDiff );
	}
	else
	{
		if( r_shadow_blur->GetBool() )
		{
			TempShadowBuffer->Activate();
			D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255,255,255,255), r3dRenderer->GetClearZValue(), 0 ) );
			TempShadowBuffer->Deactivate();
		}
	}

	R3DPROFILE_END("DR: Create Shadowmap");
	D3DPERF_EndEvent();
}

//////////////////////////////////////////////////////////////////////////
void r3dDefferedRenderer::Render()
{
	r3dProcessReleasedMeshes() ;

#if ENABLE_MULTI_GPU_OPTIMIZATIONS
	ClearRenderTargetsForMultiGPU();
#endif

	// this may update terrain ortho diffuse texture which activates screen buffers
	// hence it is invoked when no other screen buffers are around
	PrepareGrass() ;
	WaterBase::UpdateAllRipples() ;

	// before deferred targets are set, update r3dTerrain2 atlas
	if( Terrain3 && r_terrain3->GetInt())
	{
		Terrain3->Update( gCam );
	}

	if( Terrain2 && r_terrain2->GetInt())
	{
		Terrain2->UpdateAtlas( gCam );
	}

	struct AllowForbidNullViewport
	{
		AllowForbidNullViewport()
		{
			r3dRenderer->AllowNullViewport = 1 ;
		}

		~AllowForbidNullViewport()
		{
			r3dRenderer->AllowNullViewport = 0 ;
		}

	} allowForbidNullViewport ;

	if( r_allow_warmup->GetInt() )
	{
		if( int wup = r_warmup->GetInt() )
		{
			wup -- ;
			wup = R3D_MAX( wup, 0 );

			r_warmup->SetInt( wup );

			r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
			gBuffer_Color->Activate();
			WarmUpTextures();
			gBuffer_Color->Deactivate();
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}
	}

	ResetMatFrameScores();

	gCollectionsManager.SetInstanceViewRefPos(gCam);

	SetSunParams();

	SetAmbientParams();

	SunVector = GetEnvLightDir();

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Create GBuffer");
	R3DPROFILE_START("DR: Create GBuffer");

	r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;

	if( r_double_depth_ssao->GetInt() && !gBuffer_SecondaryDepth )
	{
		CreateDDRenderTargets();
	}

	r3dSetDefaultMaxAnisotropy();

	for( int i = 0; i < 6; i ++ )
	{
		r3dSetFiltering( R3D_ANISOTROPIC, i );
	}

	// setup stencil, geometry rendered will set stencil to 1. Then when rendering directional light, it will do it only for pixels that have stencil in 1
	RenderDeferredScene1();

	if( r_split_grass_render->GetInt() )
	{
		gBuffer_Color->Activate( 0 );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, 0 ) );
		SetupMaskWriteStencilStates( 2 );
		DrawGrass( GrassMap::DEPTH_PATH, false, r_lighting_quality->GetInt() > 1 );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState(	D3DRS_COLORWRITEENABLE,
														D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
														D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );
		gBuffer_Color->Deactivate( 0 );
	}

	for( int i = 0; i < 6; i ++ )
	{
		r3dSetFiltering( R3D_BILINEAR, i );
	}

	R3DPROFILE_END("DR: Create GBuffer");
	D3DPERF_EndEvent();

	r3dRenderer->SetCamera ( gCam, true );
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA );

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"SSAO Part 1");
	R3DPROFILE_START( "DR: SSAO Part 1" );

	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );

	if( r_double_depth_ssao->GetInt()  )
	{
		r3dRenderer->SetTex(gBuffer_Depth->Tex, 1);
		r3dRenderer->SetTex(gBuffer_Normal->Tex, 2);

		AdaptiveBlurDepth( gBuffer_SecondaryDepth, gBuffer_HalvedR32F_0, gBuffer_HalvedR32F_1 );
		DrawMinimum( gBuffer_HalvedR32F_1, gBuffer_PrimaryDepth, gBuffer_Depth, true );
	}

	r3dScreenBuffer* ssaoRT = NULL ;

	if( CurrentSSAOBuffer && !CurrentSSAOBuffer->IsNull() && r_ssao->GetInt() )
	{

#if R3D_ALLOW_TEMPORAL_SSAO
		r3dScreenBuffer* ssaoRT = CurrentSSAOBuffer;
			/*
			r_ssao_temporal_filter->GetInt() == R3D_SSAO_TEMPORAL_FILTER
																	?
																CurrentSSAOBuffer
																	:
																gBuffer_Aux ;
																*/
#else
		if( r_half_scale_ssao->GetInt() )
		{
			ssaoRT = g_pPostFXChief->GetBuffer( PostFXChief::RTT_HALVED0_32BIT );
		}
		else
			ssaoRT = CurrentSSAOBuffer ;
#endif

		ssaoRT->Activate() ;
		r3dRenderer->StartRender(0);

		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

		r3dRenderer->SetTex(gBuffer_Depth->Tex, 1);

		if( !r_half_scale_ssao->GetInt() && r_ssao_stencil_cut->GetBool() )
		{
			r3dRenderer->SetDSS( ScreenBuffer->ZBuf.Get() ) ;
			SetupLightMaskStencilStates( SCM_SSAO );
		}
		else
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) ) ;
		}

		RenderSSAOEffect();

		r3dRenderer->EndRender();
		ssaoRT->Deactivate();
	}

	R3DPROFILE_END( "DR: SSAO Part 1" );
	D3DPERF_EndEvent();

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"SSAO Part 2");
	R3DPROFILE_START("DR: SSAO Part 2");

	if( CurrentSSAOBuffer && !CurrentSSAOBuffer->IsNull() && ssaoRT )
	{
		if( r_double_depth_ssao->GetInt() && r_ssao->GetInt() )
		{
			DrawMinimum( gBuffer_PrimaryDepth, gBuffer_SecondaryDepth, gBuffer_Depth, false );
		}

		ApplyTemporalSSAOFilters( ssaoRT ) ;

		extern int __SSAOBlurEnable;
		void BlurSSAO(r3dScreenBuffer *SourceTex, r3dScreenBuffer *TempTex);

		if( r_ssao->GetBool() )
		{
			if( __SSAOBlurEnable )
			{
				BlurSSAO( ssaoRT, r_half_scale_ssao->GetInt() ?
														g_pPostFXChief->GetBuffer( PostFXChief::RTT_HALVED1_32BIT )
															:
														TempScreenBuffer
															);
			}
		}

		FinalizeSSAORender() ;

		if( !r_half_scale_ssao->GetInt() && r_ssao_stencil_cut->GetBool() )
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );
		}
	}

	R3DPROFILE_END( "DR: SSAO Part 2" );
	D3DPERF_EndEvent();

	//------------------------------------------------------------------------
	// Grass
	{
		gBuffer_Color->Activate( 0 );
		gBuffer_Normal->Activate( 1 );

		if( !r_grass_ssao->GetInt() )
		{

			ActivatePrimaryDepth( 2 );

			bool useAux = r_lighting_quality->GetInt() > 1;

			if( useAux )
			{
				gBuffer_Aux->Activate( 3 );
			}

			if( r_split_grass_render->GetInt() )
			{
				SetupLightMaskStencilStates( SCM_GRASS );
			}
			else
			{
				SetupMaskWriteStencilStates( 1 );
			}

			DrawGrass( GrassMap::COMBINED_PATH, r_split_grass_render->GetInt() ? true : false, useAux );

			if( useAux )
			{
				gBuffer_Aux->Deactivate();
			}

			DeactivatePrimaryDepth();

			if( ssaoRT && r_half_scale_ssao->GetInt() && CurrentSSAOBuffer && !CurrentSSAOBuffer->IsNull() )
			{
				gBuffer_Normal->Deactivate();
				gBuffer_Color->Deactivate();

				// AUX.R contains B/W contrasted 'grass ssao'. We now have to MAX it with
				// rest of the scene's ssao.

				BlendMax( CurrentSSAOBuffer->Tex, ssaoRT );

				gBuffer_Color->Activate();
				gBuffer_Normal->Activate( 1 );
			}
		}

		gBuffer_Color->Deactivate();
		gBuffer_Normal->Deactivate();
	}

	//------------------------------------------------------------------------
	// Decals

	gBuffer_Color->Activate( 0 );
	gBuffer_Normal->Activate( 1 );

	SetupLightMaskStencilStates( SCM_LITAREA );

	if( r_decals->GetInt() )
	{
		g_pDecalChief->Draw();
	}

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	//------------------------------------------------------------------------

	gBuffer_Color->Deactivate();

	gBuffer_Normal->Deactivate();

	RenderShadows( gBuffer_Depth );

	//------------------------------------------------------------------------
	// create transparent shadowmap

	if( r_shadows->GetInt() && r_transp_shadows->GetInt() && r_instanced_particles->GetInt() )
	{
		r3dRenderer->SetZRange( R3D_ZRANGE_WHOLE ) ;

		ShadowSplitDistancesTransparent[ 1 ] = r_transp_shadow_dist->GetFloat() ;
		RenderTransparentShadowMap() ;
	}

	//------------------------------------------------------------------------

	r3dRenderer->SetCamera ( gCam, true );
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA );


#if 0
	// prelighting post fxes
	{
		if( r_smooth_normals->GetBool() )
		{
			PFX_BlurEdges::Params parms;

			parms.threshold = r_smooth_normals_thresh->GetFloat();
			parms.maxd		= r_smooth_normals_maxd->GetFloat();
			parms.amplify	= r_smooth_normals_amplify->GetFloat();

			int numTaps		= R3D_MIN( R3D_MAX( r_smooth_normals_taps->GetInt() - 1, 0 ), (int)EBT_9 );

			gPFX_BlurEdgesH[ numTaps ].SetParams( parms );
			gPFX_BlurEdgesV[ numTaps ].SetParams( parms );

			g_pPostFXChief->AddFX( gPFX_BlurEdgesH[ numTaps ], PostFXChief::RTT_TEMP0,	PostFXChief::RTT_NORMALS	);
			g_pPostFXChief->AddFX( gPFX_BlurEdgesV[ numTaps ], PostFXChief::RTT_NORMALS,	PostFXChief::RTT_TEMP0		);
		}

		g_pPostFXChief->Execute( false, true );
	}
#endif

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Lighting");
	R3DPROFILE_START("DR: Lighting");

	TempScreenBuffer->Activate();
	r3dRenderer->StartRender(0);

	r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, false);
	r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, r3dRenderer->GetClearZValue(), 0 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA|R3D_BLEND_NZ);

	void DrawDepthEffectMask ();
	DrawDepthEffectMask ();

	r3dRenderer->EndRender();
	TempScreenBuffer->Deactivate();

	UpdateSkyDome( gCam ) ;

	r3dRenderer->StartRender(0);

	for (int i=0;i<8;i++)
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	}


	r3dRenderer->SetTex(gBuffer_Normal->Tex,1);

	r3dRenderer->SetTex(gBuffer_Depth->Tex,2);
	r3dRenderer->SetTex(gBuffer_Aux->Tex,3);



	if (r_use_ssr->GetInt())
	{
		r3dRenderer->SetTex( g_EnvmapProbes.GetClosestTexture( gCam ), 8 );
		Render_SSR_Reflections();
		//!
		r3dRenderer->SetTex(gBuffer_SSR.gBuffer_SSR->Tex, 7);
	}

	r3dRenderer->SetTex(gBuffer_Color->Tex);

	ScreenBuffer->Activate();


	Sun->Update();

	r3dColor Cl = r3dGameLevel::Environment->Fog_Color.GetColorValue(r3dGameLevel::Environment->__CurTime/24.0f);

	r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, Cl.GetPacked(), r3dRenderer->GetClearZValue(), 0 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);





	if( ssaoRT )
		r3dRenderer->SetTex(ssaoRT->Tex,4);
	else
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 4);

	if( r_shadows->GetInt() )
		r3dRenderer->SetTex(TempShadowBuffer->Tex,5);
	else
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE],5);

	r3dSetFiltering( R3D_POINT );

	if( r_half_scale_ssao->GetInt() )
	{
		r3dSetFiltering( R3D_BILINEAR, 4 );
	}

	// envmap
	r3dSetFiltering( R3D_BILINEAR, 8 );


	// sunlight must process some sky attributes including sun glow
	SetupLightMaskStencilStates( SCM_LITAREA );

	Render_Deffered_Sunlight( !r3dGameLevel::Environment->SunLightOn );

	SetupLightMaskStencilStates( SCM_UNLITAREA );

	r3dSetFiltering( R3D_BILINEAR, 0 );

	DrawSkyDome( false, false );

	ScreenBuffer->Deactivate();

	gBuffer_Normal->Activate();
	r3dRenderer->SetDSS( r3dRenderer->GetMainDSS() );

	SetupLightMaskStencilStates( SCM_UNLITAREA );

	// remove normal texture to be able to set it as RT and fill it's alpha with
	// sky glow
	r3dRenderer->SetTex( NULL, 1 );

	DrawSkyDome( true, false );

	gBuffer_Normal->Deactivate();

	ScreenBuffer->Activate();

	r3dSetFiltering( R3D_POINT, 0 );

	SetupLightMaskStencilStates( SCM_LITAREA );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );



	// render SSR target






	// rest of lights may not render where the sky is

	r3dRenderer->SetTex(gBuffer_Color->Tex);
	r3dRenderer->SetTex(gBuffer_Normal->Tex,1);

	r3dRenderer->SetTex(gBuffer_Depth->Tex,2);

	r3dRenderer->SetTex(gBuffer_Aux->Tex,3);

#if 0
	static bool ABC = false;
	static bool keyHold = false;
	if (Keyboard->IsPressed(kbsL))
	{
		if (!keyHold)
		{
			ABC = !ABC;
			keyHold = true;
		}
	}
	else
	{
		keyHold = false;
	}

	if (ABC)
	{
		RenderDeferredPointLightsOptimized();
	}
	else
	{
		Render_Deffered_Pointlights();
	}
#else
	Render_Deffered_Pointlights();
#endif
	Render_Deffered_Tubelights();
	Render_Deffered_Planelights();
	Render_Deffered_Spotlights();
	Render_Deffered_Volume_Lights();

	GameWorld().ResetObjFlags();


	r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, false);

	r3dRenderer->SetTex(BlurBuffer->Tex,4);

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	r3dSetFiltering( R3D_BILINEAR );

	R3DPROFILE_END("DR: Lighting");
	D3DPERF_EndEvent();

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Render misc");
	R3DPROFILE_START("DR: Render misc");

	for (int i=0;i<8;i++)
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	}

	GameWorld().Draw( rsDrawComposite1 );  // SFX

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Volume effects");
	R3DPROFILE_START("DR: Volume effects");

	IDirect3DSurface9 	*BBuf;
	IDirect3DSurface9	*surf1;

	if (bRenderRefraction && gWaterRefractionBuffer) // && (gCam.Y > LakePlaneY))
	{
		gWaterRefractionBuffer->AsTex2D()->GetSurfaceLevel(0, &surf1);
		ScreenBuffer->Tex->AsTex2D()->GetSurfaceLevel(0, &BBuf);
		r3dRenderer->pd3ddev->StretchRect( BBuf , NULL, surf1, NULL, D3DTEXF_POINT);
		surf1->Release();
		BBuf->Release();
	}

	r3dRenderer->SetTex ( TempScreenBuffer->Tex, 5 );
	r3dSetFiltering( R3D_POINT, 5 );
	void DrawDepthEffect();
	static bool bDrawDepth = true;
	if (bDrawDepth)	DrawDepthEffect();

	if ( r3dGameLevel::Environment->bVolumeFog ) DrawVolumeFogEffect( DepthBuffer, gBuffer_Normal );

	R3DPROFILE_END("DR: Volume effects");
	D3DPERF_EndEvent();

#ifndef FINAL_BUILD
	{ // reset world transform
		D3DXMATRIX  mWorld;
		D3DXMatrixIdentity(&mWorld);
		r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);

		//TODO: maybe set shader constants 0 & 4 ?
	}
#endif

	D3DPERF_BeginEvent(0x0, L"rsDrawComposite2");

	if( r_draw_composite->GetInt() )
	{
		GameWorld().Draw( rsDrawComposite2 );
	}

	// water maddles with max anisotropy
	r3dSetDefaultMaxAnisotropy();
	//g_pDecalChief->DebugDraw();

	D3DPERF_EndEvent ();

	if(g_bEditMode)
	{
		D3DPERF_BeginEvent(0x0, L"PhysicsMeshes");

		r3dRenderer->SetVertexShader("VS_PHYSICS_MESH");
		r3dRenderer->SetPixelShader("PS_PHYSICS_MESH");
		r3dRenderer->SetRenderingMode(R3D_BLEND_PUSH|R3D_BLEND_ALPHA|R3D_BLEND_ZC);

		GameWorld().Draw( rsDrawPhysicsMeshes );

		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();
		r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

		// set vertex shader constants to defaults after rendering meshes
		D3DXMATRIX ShaderMat; D3DXMatrixIdentity(&ShaderMat);
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 4, (float *)&ShaderMat,  4 );
		ShaderMat =  r3dRenderer->ViewProjMatrix;
		D3DXMatrixTranspose( &ShaderMat, &ShaderMat );
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );
		D3DPERF_EndEvent ();

		/*D3DPERF_BeginEvent(0x0, L"Debug Data");
		r3dRenderer->SetVertexShader("VS_DRAW3D_SIMPLE");
		r3dRenderer->SetPixelShader("PS_DRAW3D_SIMPLE");
		r3dRenderer->SetRenderingMode(R3D_BLEND_PUSH|R3D_BLEND_ALPHA|R3D_BLEND_ZC);

		GameWorld().Draw( rsDrawDebugData );

		r3dRenderer->Flush();
		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();
		r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		D3DPERF_EndEvent ();*/
	}

	R3DPROFILE_END("DR: Render misc");
	D3DPERF_EndEvent();

	ScreenBuffer->Deactivate();

	DrawTansparentObjects();

	if (gBuffer_Particles)
	{
		ScreenBuffer->Activate();

		r3dRenderer->SetVertexShader(-1);
		r3dRenderer->SetPixelShader(-1);

		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
		r3dSetFiltering( R3D_BILINEAR );
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_ZENABLE,       	0);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	 	D3DBLEND_ONE);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_DESTBLEND, 		D3DBLEND_INVSRCALPHA);

		r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255,255,255), gBuffer_Particles->Tex, -1.0f);

		ScreenBuffer->Deactivate();
	}

	r3dRenderer->EndRender();

	if( r_distort->GetInt() )
	{
		D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"distort screen");
		// distort screen
		TempScreenBuffer->Activate();
		r3dRenderer->StartRender(0);
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);
		r3dRenderer->SetPixelShader("PS_DISTORT_SCREEN");
		r3dRenderer->SetTex(DistortionBuffer->Tex, 2);
		r3dSetFiltering( R3D_POINT, 0 );
		r3dSetFiltering( R3D_POINT, 1 );
		r3dSetFiltering( R3D_POINT, 2 );
		r3dDrawBoxFS( TempScreenBuffer->Width, TempScreenBuffer->Height, r3dColor(255,255,255), ScreenBuffer->Tex);
		r3dRenderer->EndRender();
		TempScreenBuffer->Deactivate();

		// copy distorted screen back to screenBuffer
		r3dRenderer->StretchRect( TempScreenBuffer, ScreenBuffer ) ;

		D3DPERF_EndEvent();
	}

	extern int __RenderSSAOOnScreen;
	if ( __RenderSSAOOnScreen && r_ssao->GetBool() )
	{
		ScreenBuffer->Activate();
		r3dRenderer->StartRender(0);

		float pickCh[4] = { 0, 0, 0, 0 };

#if R3D_ALLOW_TEMPORAL_SSAO
		pickCh[ r_ssao_temporal_showmask->GetInt() ? 1 : 0 ] = 1.0f ;
#else
		pickCh[ 0 ] = 1.0f ;
#endif

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 65, pickCh, 1 ) );

		r3dRenderer->SetPixelShader("PS_SSAO_OUT");

		r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, r3dRenderer->GetClearZValue(), 0 );

		r3dRenderer->SetRenderingMode(R3D_BLEND_NZ);

#if 0
		float k = r_half_scale_ssao->GetInt() ? 0.5f : 1.f;
#else
		float k = 1.0f;
#endif

		float texcoord[16] =
		{
			0, 0, k, 0,
			k, k, 0, k,
			0, 0, 0, 0,
			0, 0, 0, 0
		};

		r3dScreenBuffer* srcBuffer = r_half_scale_ssao->GetInt() ?
											g_pPostFXChief->GetBuffer( PostFXChief::RTT_HALVED0_32BIT)
												:
											CurrentSSAOBuffer;

											r3dDrawBox2D( 0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::white, srcBuffer ? srcBuffer->Tex : NULL, texcoord );

		r3dRenderer->SetPixelShader();

		r3dRenderer->EndRender();
		ScreenBuffer->Deactivate();
	}
#ifndef FINAL_BUILD
	if( r_show_reflectivity->GetBool() )
	{
		ScreenBuffer->Activate();
		r3dRenderer->StartRender(0);

		float pickA[4] = { 0, 0, 0, 1 };
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 65, pickA, 1 ) );

		r3dRenderer->SetPixelShader( "PS_SSAO_OUT" );

		r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, r3dRenderer->GetClearZValue(), 0 );

		r3dRenderer->SetRenderingMode(R3D_BLEND_NZ);

		r3dDrawBox2D( 0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::white, gBuffer_Normal->Tex );
		r3dRenderer->SetPixelShader();

		r3dRenderer->EndRender();
		ScreenBuffer->Deactivate();
	}
#endif

	if (r_use_ssr->GetInt())
	{
		gBuffer_SSR.UpdateFrame();
	}
}


extern int _FAR_DOF;
extern int _NEAR_DOF;
extern int g_RenderRadialBlur;
extern int g_RenderScopeEffect;

void DoDepthScreenShot()
{
	float f = r_depth_screenshot_start->GetFloat() ;
	float e = r_depth_screenshot_end->GetFloat() ;

	float k0 = 1.f / ( e - f );
	float k1 = -f / ( e - f );

	D3DXVECTOR4 a0( k0, k0, k0, k0 );
	D3DXVECTOR4 a1( k1, k1, k1, k1 );

	gPFX_Transform.SetSettings( a0, a1 );
	g_pPostFXChief->AddFX( gPFX_Transform, PostFXChief::RTT_TEMP0_64BIT, PostFXChief::RTT_DEPTH_32BIT );
	g_pPostFXChief->Execute( false, true );

	r3dScreenBuffer* t0 = g_pPostFXChief->GetBuffer( PostFXChief::RTT_TEMP0_64BIT ) ;

	IDirect3DTexture9* tex( NULL );
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( (int)r3dRenderer->ScreenW, (int)r3dRenderer->ScreenH, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &tex, NULL ) );

	IDirect3DSurface9* surf ;
	D3D_V( tex->GetSurfaceLevel( 0, &surf ) );

	D3D_V( r3dRenderer->pd3ddev->GetRenderTargetData( t0->GetTex2DSurface(), surf ) );

	D3D_V( D3DXSaveTextureToFile( r_depth_screenshot_name->GetString(), D3DXIFF_TGA, tex, NULL ) );

	SAFE_RELEASE( surf );

	SAFE_RELEASE( tex );
}

extern PFX_ExtractBloom::Settings gBloomSettings ;
extern PFX_FilmGrain::Settings gFilmGrainSettings ;

extern int _UsedBloomBlurPasses			;
extern int _UsedBloomBlurTaps ;
extern float DirectionalStreaksOnOffCoef	;

void UpdateHUDFilterSettings( int* colorCorrectionEnable, int *filmGrainEnabled )
{
	HUDFilterSettings &hfs = gHUDFilterSettings[r_hud_filter_mode->GetInt()];

	int fgOn = 0 ;

	if( hfs.filmGrain )
	{
		gPFX_FilmGrain.SetSettings( hfs.filmGrainSettings ) ;
		fgOn = 1 ;
	}
	else
	{
		gPFX_FilmGrain.SetSettings( gFilmGrainSettings ) ;
		fgOn = FilmGrainOn ;
	}

	gPFX_ExtractBloom.SetDefaultSettings( hfs.bloomSettings ) ;
	_UsedBloomBlurPasses = hfs.bloomBlurPasses ;
	DirectionalStreaksOnOffCoef = hfs.directionalStreaksOnOffCoef ;
	_UsedBloomBlurTaps = hfs.bloomBlurTaps ;

	if( colorCorrectionEnable )
		*colorCorrectionEnable = hfs.enableColorCorrection;

	if( filmGrainEnabled )
		*filmGrainEnabled = fgOn ;

	g_OverrideAmbientAndIntensity	= hfs.overrideAmbientAndIntensity;
	g_OverrideIntensity				= hfs.overrideIntensity;
	g_OverrideAmbientColor			= hfs.overrideAmbient;
}

//------------------------------------------------------------------------

void r3dDefferedRenderer::AppendPostFXes()
{
	extern int __RenderSSAOOnScreen;

	int renderColorCorrection ;
	int fgEnabled ;

	UpdateHUDFilterSettings( &renderColorCorrection, &fgEnabled );

	if( r_do_depth_screenshot->GetInt() )
	{
		r_do_depth_screenshot->SetInt( 0 );
		DoDepthScreenShot();
	}

#if 0
	g_pPostFXChief->AddFX( gPFX_FilmTone ) ;
	g_pPostFXChief->AddSwapBuffers() ;
#endif


	if (__RenderSSAOOnScreen == 0)
	{
		if( r_distort->GetInt() && r_bloom->GetInt() )
		{
			// append distortion.b channel to normal.a cause distortion.b holdz particle GLOW

			PFX_MixIn::Settings sts ;

			sts.Sel				= PFX_MixIn::SEL_RGBB ;
			sts.ColorWriteMask	= D3DCOLORWRITEENABLE_ALPHA ;
			sts.BlendMode		= R3D_BLEND_ADD | R3D_BLEND_NZ ;

			// NOTE : when you start using mixin several times implement & replace with PUSH settings
			gPFX_MixIn.SetSettings( sts ) ;
			g_pPostFXChief->AddFX( gPFX_MixIn, PostFXChief::RTT_NORMALS_32BIT, PostFXChief::RTT_DISTORTION_32BIT ) ;
		}

		if ( r_mlaa->GetInt() )
		{
			AddMLAAStack();
		}

		if (r_fxaa->GetInt() )
		{
			AddFXAAStack();
		}

		//	Explosion radial blurs
		gExplosionVisualController.ApplyPostFXExplosionEffects();

		// bloom effect
		if ( LevelBloom && r_bloom->GetInt() )
		{
			AddBloomStack();

			extern int _SunGlare;
			if(_SunGlare && r_glow->GetInt())
			{
				g_pPostFXChief->AddFX( gPFX_SunGlare, PostFXChief::RTT_PINGPONG_LAST, PostFXChief::RTT_ONEFOURTH0_64BIT ) ;
			}

			if(r_light_streaks->GetInt())
				AddDirectionalStreaksStack( PostFXChief::RTT_ONEFOURTH0_64BIT ) ;
		}

		// depth of field
		if( LevelDOF && r_dof->GetInt() && ( _NEAR_DOF || _FAR_DOF ) )
		{
			AddDepthOfFieldStack();
		}

		AddCalcAvarageLumaStack() ;

		gFlashbangVisualController.ApplyPostFXEffects();

		if( r_lens_dirt->GetInt() && g_LensDirtEnabled )
		{
			Sun->SetOcclusionQueryRequired( 1 );
			AddLensDirtStack();
		}
		else
		{
			Sun->SetOcclusionQueryRequired( 0 );
		}

		// film grain
		if ( fgEnabled && r_film_grain->GetInt() )
		{
			g_pPostFXChief->AddFX( gPFX_FilmGrain );
			g_pPostFXChief->AddSwapBuffers();
		}

		// radial blur + modulate
		if( g_RenderRadialBlur )
		{
			g_pPostFXChief->AddFX( gPFX_RadialBlur );
			g_pPostFXChief->AddSwapBuffers();
		}

		if (g_RenderScopeEffect
#ifndef FINAL_BUILD
			|| r_force_scope->GetInt()
#endif
			)
		{
			AddScopeEffectStack();
		}

		if ( LevelSunRays && r_sun_rays->GetInt() )
		{
			g_pPostFXChief->AddClear( 0, PostFXChief::RTT_TEMP0_64BIT );

			g_pPostFXChief->AddFX( gPFX_SeedSunThroughStencil, PostFXChief::RTT_TEMP0_64BIT );
  			g_pPostFXChief->AddFX( gPFX_Copy, PostFXChief::RTT_HALVED0_32BIT, PostFXChief::RTT_TEMP0_64BIT );

  			g_pPostFXChief->AddFX( gPFX_GodRays, PostFXChief::RTT_HALVED1_32BIT, PostFXChief::RTT_HALVED0_32BIT );
			g_pPostFXChief->AddFX( gPFX_StarShapeBlur, PostFXChief::RTT_HALVED0_32BIT, PostFXChief::RTT_HALVED1_32BIT );

			g_pPostFXChief->AddFX( *gPFX_BlurH[ 1 ], PostFXChief::RTT_HALVED1_32BIT, PostFXChief::RTT_HALVED0_32BIT );
  			g_pPostFXChief->AddFX( *gPFX_BlurV[ 1 ], PostFXChief::RTT_HALVED0_32BIT, PostFXChief::RTT_HALVED1_32BIT );

			g_pPostFXChief->AddFX( gPFX_Add, PostFXChief::RTT_PINGPONG_LAST, PostFXChief::RTT_HALVED0_32BIT);
		}


		if ( g_ObjectMotionBlurEnabled )
		{
			AddObjectMotionBlurStack();
		}

		bool bCameraMotion = false;

		if( g_cameraMotionBlurEnabled )
		{
			gPFX_CameraMotionBlur.Update();
			bCameraMotion = gPFX_CameraMotionBlur.IsCameraInMotion();
		}

		if ( bCameraMotion )
		{
			g_pPostFXChief->AddFX( gPFX_CameraMotionBlur );
			g_pPostFXChief->AddSwapBuffers();
		}

		if( r_gameplay_blur_strength->GetFloat() > 0.001f )
		{
			int width = r_gameplay_blur_strength->GetFloat() * ( BT_COUNT - 1 );

			width = R3D_MAX( R3D_MIN( width, BT_COUNT - 1 ), 0 );

			g_pPostFXChief->AddFX( *gPFX_BlurH[ width ],	PostFXChief::RTT_TEMP0_64BIT	,	PostFXChief::RTT_PINGPONG_LAST	);
			g_pPostFXChief->AddFX( *gPFX_BlurV[ width ],	PostFXChief::RTT_PINGPONG_LAST	,	PostFXChief::RTT_TEMP0_64BIT	);
		}

		g_pPostFXChief->AddFX( gPFX_ConvertToLDR ) ;

		g_pPostFXChief->AddSwapBuffers() ;

		if( mPreCCScreenshotTarget )
		{
			g_pPostFXChief->AddGrabScreen( mPreCCScreenshotTarget, PostFXChief::RTT_PINGPONG_LAST );
			mPreCCScreenshotTarget = NULL;
		}

		if( renderColorCorrection )
		{
			extern r3dTexture* CC_LUT1D_HSV_Tex, *CC_LUT1D_RGB_Tex;

			switch( g_ColorCorrectionSettings.scheme )
			{
			case ColorCorrectionSettings::CCS_USE_RGB_CURVES:
				if( CC_LUT1D_RGB_Tex )
				{
					g_pPostFXChief->AddFX( gPFX_1DLUTColorCorrectionRGB );
					g_pPostFXChief->AddSwapBuffers();
				}
				break;
			case ColorCorrectionSettings::CCS_USE_HSV_CURVES:
				if( CC_LUT1D_HSV_Tex )
				{
					g_pPostFXChief->AddFX( gPFX_1DLUTColorCorrectionHSV );
					g_pPostFXChief->AddSwapBuffers();
				}
				break;
			case ColorCorrectionSettings::CCS_USE_RGB_HSV_CURVES:
				if( CC_LUT1D_HSV_Tex && CC_LUT1D_RGB_Tex )
				{
					g_pPostFXChief->AddFX( gPFX_1DLUTColorCorrectionRGB );
					g_pPostFXChief->AddSwapBuffers();

					g_pPostFXChief->AddFX( gPFX_1DLUTColorCorrectionHSV );
					g_pPostFXChief->AddSwapBuffers();
				}
				break;
			case ColorCorrectionSettings::CCS_CUSTOM_3DLUT:
				if( gHUDFilterSettings[r_hud_filter_mode->GetInt()].HasActiveColorCorrection() )
				{
					g_pPostFXChief->AddFX( gPFX_3DLUTColorCorrection );
					g_pPostFXChief->AddSwapBuffers();
				}
				break;
			default:
				r3d_assert( false && "Unknown color correction scheme!" );
			}

		}

		if(g_CCBlackWhite)
		{
			gPFX_BlackWhiteColorCorrection.SetPower(g_fCCBlackWhitePwr);
			g_pPostFXChief->AddFX( gPFX_BlackWhiteColorCorrection );
			g_pPostFXChief->AddSwapBuffers();
		}

		if( r_do_internal_screenshot->GetInt() && !InternalScreenshotBuffer )
		{
			float rtWidth, rtHeight ;
			GetDesiredRTDimmensions( &rtWidth, &rtHeight ) ;
			InternalScreenshotBuffer = r3dScreenBuffer::CreateClass( "InternalScreenshot", rtWidth, rtHeight, D3DFMT_A8R8G8B8 ) ;

			g_pPostFXChief->AddGrabScreen( InternalScreenshotBuffer, PostFXChief::RTT_PINGPONG_LAST ) ;
		}

#if 0
		gPFX_GammaCorrect.SetPower( r_gamma_pow->GetFloat() );

		g_pPostFXChief->AddFX( gPFX_GammaCorrect );
		g_pPostFXChief->AddSwapBuffers();
#endif

		if( r_3d_stereo_post_fx->GetInt() || r3dRenderer->IsStereoActive() )
		{
			AddSeparateEyesStereoReprojectionStack(PostFXChief::RTT_PINGPONG_LAST, PostFXChief::RTT_PINGPONG_NEXT, PostFXChief::RTT_TEMP0_64BIT);
			g_pPostFXChief->AddSwapBuffers() ;

			if (!r3dRenderer->IsStereoActive())
			{
				gPFX_AnaglyphComposite.SetRightBufferID( PostFXChief::RTT_TEMP0_64BIT );
				g_pPostFXChief->AddFX( gPFX_AnaglyphComposite );
			}
			g_pPostFXChief->AddSwapBuffers() ;
		}

	}
}

//////////////////////////////////////////////////////////////////////////

void r3dDefferedRenderer::PostProcess()
{
	AppendPostFXes();

#ifndef FINAL_BUILD
	if( d_print_postfx->GetInt() )
	{
		g_pPostFXChief->DEBUG_MakePostFXSnapshot();
	}
#endif

	if( r3dRenderer->IsStereoActive() )
	{
		g_pPostFXChief->Execute( false, true );
		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();
		r3dRenderer->SetEye(R3D_STEREO_EYE_LEFT);
		r3dDrawBoxFS(r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::white ,g_pPostFXChief->GetBuffer(PostFXChief::RTT_PINGPONG_NEXT)->Tex);
		r3dRenderer->SetEye(R3D_STEREO_EYE_RIGHT);
		r3dDrawBoxFS(r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::white, g_pPostFXChief->GetBuffer(PostFXChief::RTT_TEMP0_64BIT)->Tex);
		r3dRenderer->SetEye(R3D_STEREO_EYE_MONO);
	}
	else
	{
		g_pPostFXChief->Execute( true, true );
	}

	for (int i=0;i<8;i++)
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	}

	r3dRenderer->SetPixelShader();
	r3dRenderer->SetVertexShader();

#ifndef FINAL_BUILD
	extern int TerrainDrawBounds;
	extern r3dTerrainPaintBoundControl g_TerraPaintBoundCtrl ;
	extern void Post_DrawTerrainBounds( const r3dTerrainPaintBoundControl& boundCtrl );
	if (TerrainDrawBounds) Post_DrawTerrainBounds( g_TerraPaintBoundCtrl );

	r3dRenderer->SetPixelShader();
	r3dRenderer->SetVertexShader();
#endif
}

//------------------------------------------------------------------------
/*virtual*/
void r3dDefferedRenderer::RenderStage_Prepare() /*OVERRIDE*/
{
	extern draw_s draw[ OBJECTMANAGER_MAXOBJECTS ];
	extern int n_draw;
	extern RenderArray g_render_arrays[ rsCount ];

	GameWorld().AddObjectListener( this );
	GameWorld().PrepareObjectArray( gCam );

	for( int i = 0, e = n_draw; i < e; i ++ )
	{
		mTempObjectArray[ i ] = draw[ i ].obj;
	}

	mTempObjectArrayCount = n_draw;

	r3dRenderer->SetDSS( 0 ) ;

	D3DPERF_BeginEvent( 0x0, L"ClearScreen" );

	if( r_device_clear->GetInt() )
	{
		gBuffer_StagedRender_Depth->Activate();
		SetMRTClearShaders( true ) ;
		r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::black );
		gBuffer_StagedRender_Depth->Deactivate();

		gBuffer_StagedRender_Normal->Activate(0);
		// NOTE : sync clear value with DS_ClearBuffer_ps.hls
		D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, 0x7fff7f, r3dRenderer->GetClearZValue(), 0 ) );
		gBuffer_StagedRender_Normal->Deactivate();

		gBuffer_StagedRender_Color->Activate(0);
		// NOTE : sync clear value with DS_ClearBuffer_ps.hls
		D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, 0xffffffff, r3dRenderer->GetClearZValue(), 0 ) );
		gBuffer_StagedRender_Color->Deactivate();
	}
	else
	{
		gBuffer_StagedRender_Color->Activate();
		gBuffer_StagedRender_Normal->Activate( 1 );
		gBuffer_StagedRender_Depth->Activate( 2 );

		ClearMRTUsingShaders();

		gBuffer_StagedRender_Color->Deactivate();
		gBuffer_StagedRender_Normal->Deactivate();
		gBuffer_StagedRender_Depth->Deactivate();
	}

	gBuffer_StagedRender_Color->Activate();

	D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, r3dRenderer->GetClearZValue(), 0 ) );

	gBuffer_StagedRender_Color->Deactivate();

	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------
/*virtual*/
void r3dDefferedRenderer::RenderStage_Terrain() /*OVERRIDE*/
{
	RenderStageHelper_StartDeferred();

	if( Terrain3 )
		Terrain3->Render( gCam );

	if( Terrain2 )
		Terrain2->Render( gCam );

	RenderStageHelper_EndDeferred();
}

//------------------------------------------------------------------------
void r3dDefferedRenderer::RenderStageHelper_StartDeferred()
{
	gCollectionsManager.SetInstanceViewRefPos( gCam );

	SetSunParams();

	SetAmbientParams();

	SunVector = GetEnvLightDir();

	r3dRenderer->SetZRange( R3D_ZRANGE_WORLD );

	// setup stencil, geometry rendered will set stencil to 1. Then when rendering directional light, it will do it only for pixels that have stencil in 1
	r3dVector V;

	SetVolumeFogParams();

	void UpdateSSAOClearValue();
	UpdateSSAOClearValue();

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ ) ;

	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );

	d3dc._SetDecl( R3D_MESH_VERTEX::getDecl() );

	// sometimes scaleform or whatever ruins our cache so we make sure depth stencil is refreshed here
	r3dRenderer->SetDSS( 0 ) ;

	r3dRenderer->SetZRange( R3D_ZRANGE_WORLD );

	gBuffer_StagedRender_Color->Activate();
	gBuffer_StagedRender_Normal->Activate(1);
	gBuffer_StagedRender_Depth->Activate(2);

	r3dRenderer->StartRender(0);

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	SetupMaskWriteStencilStates( 1 );

	r3dRenderer->SetCamera ( gCam, true );

	r3dRenderer->SetVertexShader(VS_FILLGBUFFER_ID);
	FillbufferShaderKey key;

	SetLQ( &key ) ;

	SetFillGBufferPixelShader( key ) ;

	D3DXVECTOR4 CamVec = D3DXVECTOR4( gCam.x, gCam.y, gCam.z, 1);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_CAMVEC, (float*)&CamVec, 1);

	float defSSAO[ 4 ] = { r_ssao_clear_val->GetFloat(), 0.f, 0.f, 0.f };
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_DEF_SSAO, defSSAO, 1);

	r3dRenderer->SetMipMapBias(__WorldRenderBias);

	r3dSetDefaultMaxAnisotropy();

	for( int i = 0; i < 6; i ++ )
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

		r3dSetFiltering( R3D_ANISOTROPIC, i );
	}

	extern int _r3d_Mesh_LoadSecondUV;
	if(_r3d_Mesh_LoadSecondUV)
	{
		// decal map at s11 must be wrapped
		r3dRenderer->pd3ddev->SetSamplerState( 11, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( 11, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	}

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
}

//------------------------------------------------------------------------
/*virtual*/
void r3dDefferedRenderer::RenderStage_Deferred( int startObject, int objectCount ) /*OVERRIDE*/
{
	RenderStageHelper_StartDeferred();

	extern RenderArray g_render_arrays[ rsCount ];

	for( int i = 0, e = rsCount; i < e; i ++ )
	{
		g_render_arrays[ i ].Clear();
	}

	for( int i = startObject, e = startObject + objectCount; i < e; i ++ )
	{
		GameObject* obj = mTempObjectArray[ i ];

		if( obj->ObjFlags & OBJFLAG_WasDestroyed )
			continue;

		obj->AppendRenderables( g_render_arrays, gCam );
	}

	GameWorld().Draw( rsFillGBuffer );

	RenderStageHelper_EndDeferred();
}

//------------------------------------------------------------------------

void r3dDefferedRenderer::RenderStageHelper_EndDeferred()
{
	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();
	r3dRenderer->SetFog(0);

	gBuffer_StagedRender_Depth->Deactivate();

	gBuffer_StagedRender_Normal->Deactivate();

	gBuffer_StagedRender_Color->Deactivate();

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );

	r3dRenderer->EndRender();

	for( int i = 0; i < 6; i ++ )
	{
		r3dSetFiltering( R3D_BILINEAR, i );
	}

	r3dRenderer->SetCamera ( gCam, true );
	r3dRenderer->SetMaterial( NULL );
	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA );
}

//------------------------------------------------------------------------
/*virtual*/

void r3dDefferedRenderer::RenderStage_Finilize( const D3DVIEWPORT9* viewport )
{
	mTempObjectArrayCount = 0;

	r3dRenderer->SetZRange( R3D_ZRANGE_WORLD );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	RenderShadows( gBuffer_StagedRender_Depth );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
	r3dRenderer->SetZRange( R3D_ZRANGE_WORLD );

	r3dRenderer->SetCamera( gCam, true );
	r3dRenderer->SetMaterial( NULL );
	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA );

	D3DPERF_BeginEvent( D3DCOLOR_XRGB(255,255,255), L"Lighting" );
	R3DPROFILE_START( "DR: Lighting" );

	UpdateSkyDome( gCam );

	r3dRenderer->StartRender( 0 );

	for (int i=0;i<8;i++)
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	}

	r3dRenderer->SetTex(gBuffer_StagedRender_Normal->Tex,1);

	r3dRenderer->SetTex(gBuffer_StagedRender_Depth->Tex,2);

	// AUX
	r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_BLUE],3);

	r3dRenderer->SetTex(gBuffer_StagedRender_Color->Tex);

	gBuffer_StagedRender_Temporary->Activate();

	r3dRenderer->SetDSS( NULL );
	r3dRenderer->SetDSS( gBuffer_StagedRender_Color->OurZBuf.Get() );

	Sun->Update();

	r3dColor Cl = r3dGameLevel::Environment->Fog_Color.GetColorValue(r3dGameLevel::Environment->__CurTime/24.0f);

	r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, Cl.GetPacked(), r3dRenderer->GetClearZValue(), 0 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

	r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 4);

	if( r_shadows->GetInt() )
		r3dRenderer->SetTex(TempShadowBuffer->Tex,5);
	else
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE],5);

	r3dSetFiltering( R3D_POINT );

	// envmap
	r3dSetFiltering( R3D_BILINEAR, 8 );

	// sunlight must process some sky attributes including sun glow
	SetupLightMaskStencilStates( SCM_LITAREA );

	Render_Deffered_Sunlight( !r3dGameLevel::Environment->SunLightOn );

	SetupLightMaskStencilStates( SCM_UNLITAREA );

	r3dSetFiltering( R3D_BILINEAR, 0 );

	DrawSkyDome( false, false );

	gBuffer_StagedRender_Temporary->Deactivate();

	gBuffer_StagedRender_Normal->Activate();
	r3dRenderer->SetDSS( gBuffer_StagedRender_Color->OurZBuf.Get() );

	SetupLightMaskStencilStates( SCM_UNLITAREA );

	// remove normal texture to be able to set it as RT and fill it's alpha with
	// sky glow
	r3dRenderer->SetTex( NULL, 1 );

	DrawSkyDome( true, false );

	gBuffer_StagedRender_Normal->Deactivate();

	gBuffer_StagedRender_Temporary->Activate();

	r3dSetFiltering( R3D_POINT, 0 );

	SetupLightMaskStencilStates( SCM_LITAREA );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );

	// rest of lights may not render where the sky is

	r3dRenderer->SetTex(gBuffer_StagedRender_Color->Tex);
	r3dRenderer->SetTex(gBuffer_StagedRender_Normal->Tex,1);

	r3dRenderer->SetTex(gBuffer_StagedRender_Depth->Tex,2);

	// AUX
	r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_BLUE], 3 );

	Render_Deffered_Pointlights();
	Render_Deffered_Tubelights();
	Render_Deffered_Planelights();
	Render_Deffered_Spotlights();
	Render_Deffered_Volume_Lights();

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, false);

	// BlurBuffer was here instead of NULL
	r3dRenderer->SetTex( NULL, 4 );

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	r3dSetFiltering( R3D_BILINEAR );

	R3DPROFILE_END("DR: Lighting");
	D3DPERF_EndEvent();

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Render misc");
	R3DPROFILE_START("DR: Render misc");

	for (int i=0;i<8;i++)
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	}

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"Volume effects");
	R3DPROFILE_START("DR: Volume effects");

	// NULL instead of TempScreenBuffer->Tex
	r3dRenderer->SetTex ( NULL, 5 );
	r3dSetFiltering( R3D_POINT, 5 );

	if ( r3dGameLevel::Environment->bVolumeFog ) DrawVolumeFogEffect( gBuffer_StagedRender_Depth, gBuffer_StagedRender_Normal );

	R3DPROFILE_END("DR: Volume effects");
	D3DPERF_EndEvent();

	// water maddles with max anisotropy
	r3dSetDefaultMaxAnisotropy();

	D3DPERF_EndEvent();

	R3DPROFILE_END("DR: Render misc");
	D3DPERF_EndEvent();

	gBuffer_StagedRender_Temporary->Deactivate();
	r3dRenderer->EndRender();
}

//------------------------------------------------------------------------

int r3dDefferedRenderer::GetStagedRenderObjectCount() /*OVERRIDE*/
{
	return mTempObjectArrayCount;
}

//------------------------------------------------------------------------

void ResetShadowCache()
{
	LastLFShadowSliceUpdatePos = r3dPoint3D( FLT_MAX, FLT_MAX, FLT_MAX );
}


void UpdateHWSchadowScheme()
{
	if( r_allow_hw_shadows->GetInt() )
	{
		if (r_shadow_blur->GetBool())
		{
#if 0
			if ( r3dRenderer->SupportsINTZTextures )
				r_hardware_shadow_method->SetInt( HW_SHADOW_METHOD_INTZ );
#else
			r_hardware_shadow_method->SetInt( HW_SHADOW_METHOD_R32F );
#endif
		}
		else
		{
			if ( r3dRenderer->SupportsHWShadowMapping )
			{
				r_hardware_shadow_method->SetInt( HW_SHADOW_METHOD_HW_PCF );
			}
			else
				r_hardware_shadow_method->SetInt( HW_SHADOW_METHOD_R32F );
		}
	}
	else
	{
		r_hardware_shadow_method->SetInt( HW_SHADOW_METHOD_R32F );
	}

	void UpdateFXAAShadowBlurSettings();
	UpdateFXAAShadowBlurSettings();
}

void UpdateZPrepassSettings()
{
	if( !r_z_allow_prepass->GetInt() )
	{
		r_z_prepass_method->SetInt( Z_PREPASS_METHOD_NONE ) ;
	}
}

int D3DPROFILE_DEPTH_PREPASS ;
int D3DPROFILE_FILLGBUFFER ;
int D3DPROFILE_SSAO ;
int D3DPROFILE_SCALEFORM ;
int D3DPROFILE_FULLFRAME ;

void InitDedicatedD3DProfilerStamps()
{
#ifndef FINAL_BUILD
	D3DPROFILE_DEPTH_PREPASS	= r3dProfiler::CreateDedicatedD3DStamp( "Depth Prepass" ) ;
	D3DPROFILE_FILLGBUFFER		= r3dProfiler::CreateDedicatedD3DStamp( "FillGBuffer" ) ;
	D3DPROFILE_SSAO				= r3dProfiler::CreateDedicatedD3DStamp( "SSAO" ) ;
	D3DPROFILE_SCALEFORM		= r3dProfiler::CreateDedicatedD3DStamp( "ScaleForm" ) ;
	D3DPROFILE_FULLFRAME		= r3dProfiler::CreateDedicatedD3DStamp( "Full Frame" ) ;
#endif
}
//------------------------------------------------------------------------


SunHDRLightPShaderId::SunHDRLightPShaderId()
{
	Id = 0 ;
}

//------------------------------------------------------------------------

void
SunHDRLightPShaderId::ToString( char* str )
{
	strcpy( str, "PS_SUNLIGHT_HDR" ) ;

	if( ssr )
	{
		strcat ( str, "_SSR" ) ;
	}

	if( sss )
	{
		strcat ( str, "_SSS" ) ;
	}

	if( transp_shadows )
	{
		strcat( str, "_TRANSP_SHADOWS" ) ;
	}

	if( white_albedo )
	{
		strcat( str, "_WHITE_ALBEDO" ) ;
	}

	if( ambient_only )
	{
		strcat( str, "_AMBIENT_ONLY" ) ;
	}

	strcat( str, "_QL" );
	strcat( str, DigitToCString( quality ) );
}

//------------------------------------------------------------------------

void
SunHDRLightPShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 6 );

	defines[ 0 ].Name = "SSR_ON";
	defines[ 0 ].Definition = ssr ? "1" : "0";

	defines[ 1 ].Name = "SSS_ON";
	defines[ 1 ].Definition = sss ? "1" : "0";

	defines[ 2 ].Name = "TRANSPARENT_SHADOWS";
	defines[ 2 ].Definition = transp_shadows ? "1" : "0";

	defines[ 3 ].Name = "WHITE_ALBEDO";
	defines[ 3 ].Definition = white_albedo ? "1" : "0";

	defines[ 4 ].Name = "AMBIENT_ONLY";
	defines[ 4 ].Definition = ambient_only ? "1" : "0";

	defines[ 5 ].Name = "QUALITY";
	defines[ 5 ].Definition = DigitToCString( quality );
}

SunHDRLightPSIds gSunHDRLightPSIds ;

//------------------------------------------------------------------------

PointLightPShaderId::PointLightPShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
PointLightPShaderId::ToString( char* str )
{
	strcpy( str, "PS_POINTLIGHT_HDR" ) ;

	if( ssr )
	{
		strcat( str, "_SSR" ) ;
	}

	if( sss )
	{
		strcat( str, "_SSS" ) ;
	}

	if( diffuse_only )
	{
		strcat( str, "_DIFFONLY" ) ;
	}

	if( ssao )
	{
		strcat( str, "_SSAO" ) ;
	}

	if( proj_texture )
	{
		strcat( str, "_PROJTEXTURE" ) ;
	}

	if( shadow_mode == 1 )
	{
		strcat( str, "_SHADOWS" ) ;
	}

	if( shadow_mode == 2 )
	{
		strcat( str, "_SHADOWS_PCF" ) ;
	}

	if( shadow_mode == 3 )
	{
		strcat( str, "_SSMSHADOWS" ) ;
	}

	if( shadow_mode == 4 )
	{
		strcat( str, "_PARABOLOID_SHADOWS" ) ;
	}

	strcat( str, "_QL" );
	strcat( str, DigitToCString( quality ) );

	r3d_assert( shadow_mode < 5 ) ;

}

//------------------------------------------------------------------------

void
PointLightPShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 7 ) ;

	defines[ 0 ].Name = "SSR_ON" ;
	defines[ 0 ].Definition = DigitToCString( ssr ) ;

	defines[ 1 ].Name = "SSS_ON" ;
	defines[ 1 ].Definition = DigitToCString( sss ) ;

	defines[ 2 ].Name = "DIFFUSE_ONLY" ;
	defines[ 2 ].Definition = DigitToCString( diffuse_only ) ;

	defines[ 3 ].Name = "USE_SSAO" ;
	defines[ 3 ].Definition = DigitToCString( ssao );

	defines[ 4 ].Name = "USE_PROJ_MAP" ;
	defines[ 4 ].Definition = DigitToCString( proj_texture ) ;

	defines[ 5 ].Name = "SHADOW_MODE" ;
	defines[ 5 ].Definition = DigitToCString( shadow_mode ) ;

	defines[ 6 ].Name = "QUALITY";
	defines[ 6 ].Definition = DigitToCString( quality );


}

PointLightPSIds gPointLightPSIds ;

//------------------------------------------------------------------------

SpotLightPShaderId::SpotLightPShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
SpotLightPShaderId::ToString( char* str )
{
	strcpy( str, "PS_SPOTLIGHT_HDR" );

	if( aux_enabled )
	{
		strcat( str, "_AUX" );
	}

	if( shadow_mode == 1 )
	{
		strcat( str, "_SHADOWS" );
	}

	if( shadow_mode == 2 )
	{
		strcat( str, "_HWSHADOWS" );
	}

	if( shadow_mode == 3 )
	{
		strcat( str, "_SSMSHADOWS" );
	}

	if( proj_texture )
	{
		strcat( str, "_PROJTEX" );
	}

	strcat( str, "_QL" );
	strcat( str, DigitToCString( quality ) );

}

//------------------------------------------------------------------------

void
SpotLightPShaderId::FillMacros( ShaderMacros& defines )
{
	const char* DigitToCString( int digit ) ;

	defines.Resize( 4 ) ;

	defines[ 0 ].Name = "AUX_ENABLED";
	defines[ 0 ].Definition = DigitToCString( aux_enabled );

	defines[ 1 ].Name = "SHADOW_MODE";
	defines[ 1 ].Definition = DigitToCString( shadow_mode );

	defines[ 2 ].Name = "PROJ_TEXTURE";
	defines[ 2 ].Definition = DigitToCString( proj_texture );

	defines[ 3 ].Name = "QUALITY";
	defines[ 3 ].Definition = DigitToCString( quality );

}

SpotLightPSIds gSpotLightPSIds ;

//------------------------------------------------------------------------

SSAOPShaderId::SSAOPShaderId()
{
	Id = 0 ;
}

void
SSAOPShaderId::ToString( char* str )
{
	strcpy( str, "PS_SSAO" ) ;

	if( num_rays )
		strcat( str, "_16" ) ;
	else
		strcat( str, "_8" ) ;

	if( optimized )
		strcat( str, "_OPTIMIZED" ) ;

	if( detail_radius )
		strcat( str, "_DETAIL" ) ;

#if R3D_ALLOW_TEMPORAL_SSAO
	if( temporal_optimisation )
		strcat( str, "_TEMPORAL" ) ;

	if( temporal_show_passed )
		strcat( str, "_TEMPOREVEAL" ) ;

	if( output_stability_mask )
		strcat( str, "_STABILITYMASK" ) ;
#endif
}


void
SSAOPShaderId::FillMacros( ShaderMacros& defines )
{
#if R3D_ALLOW_TEMPORAL_SSAO
	defines.Resize( 5 );
#else
	defines.Resize( 2 );
#endif

#define DEMACRO_TEXTIZE(val) #val
#define TEXTIZE(val) DEMACRO_TEXTIZE(val)

	defines[ 0 ].Name		= "DETAIL_RADIUS";
	defines[ 0 ].Definition	= detail_radius ? "1" : "0" ;

	defines[ 1 ].Name		= "NUM_BASE_ELEMS";
	defines[ 1 ].Definition	= num_rays ? TEXTIZE(SSAO_ALT_NUM_RAYS) : TEXTIZE(SSAO_LW_NUM_RAYS) ;

#if R3D_ALLOW_TEMPORAL_SSAO
	defines[ 2 ].Name		= "TEMPORAL_OPTIMIZATION";

	// options with temporal optimizations require more slots than some cards allow ( up to 523 )
	if( r3dRenderer->d3dCaps.MaxPixelShader30InstructionSlots < 512 + 32 )
		defines[ 2 ].Definition	= "0" ;
	else
		defines[ 2 ].Definition	= temporal_optimisation ? "1" : "0" ;

	defines[ 3 ].Name		= "TEMPORAL_SHOW_PASSED" ;
	defines[ 3 ].Definition	= temporal_show_passed ? "1" : "0" ;

	defines[ 4 ].Name		= "OUTPUT_STABILITY_MASK" ;
	defines[ 4 ].Definition	= output_stability_mask ? "1" : "0" ;
#endif
}

const char*
SSAOPShaderId::GetShaderFile()
{
	return optimized ? "SSAO_NEW_PS.hls" : "SSAO_PS.hls" ;
}

void ApplyTemporalSSAOFilters( r3dScreenBuffer* ssaoRT )
{
	if( !r_ssao->GetInt() )
		return ;

#if R3D_ALLOW_TEMPORAL_SSAO
	if( R3D_SSAO_TEMPORAL_FILTER == r_ssao_temporal_filter->GetInt() )
	{
		TempScreenBuffer->Activate() ;
		DiluteSSAOMask( ssaoRT ) ;
		TempScreenBuffer->Deactivate() ;

		CurrentSSAOBuffer->Activate() ;
		CompositeSSAO( TempScreenBuffer ) ;
		CurrentSSAOBuffer->Deactivate() ;

		extern int __RenderSSAOOnScreen ;
		if( r_ssao_temporal_showmask->GetInt() && __RenderSSAOOnScreen )
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN ) ) ;
			CopySurface( TempScreenBuffer, gBuffer_Aux ) ;
		}
		else
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) ) ;
			CopySurface( CurrentSSAOBuffer, gBuffer_Aux ) ;
		}

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,
															D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
															D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA
															) ) ;

		CopySurface( gBuffer_Depth, PrevDepthBuffer ) ;

		R3D_SWAP( PrevSSAOBuffer, CurrentSSAOBuffer ) ;
	}
	else
	{
		if( R3D_SSAO_TEMPORAL_OPTIMIZE == r_ssao_temporal_filter->GetInt() )
		{
			g_pPostFXChief->AddFX( gPFX_Copy, PostFXChief::RTT_PREV_FRAME_DEPTH, PostFXChief::RTT_DEPTH_32BIT ) ;
			g_pPostFXChief->AddFX( gPFX_Copy, PostFXChief::RTT_PREV_FRAME_SSAO, PostFXChief::RTT_AUX_32BIT ) ;

			g_pPostFXChief->Execute( false, true );
		}
	}
#endif
}

void UpdateInternalScreenshot()
{
#ifndef FINAL_BUILD
	if( r_do_internal_screenshot->GetInt() && InternalScreenshotBuffer )
	{
		IDirect3DTexture9* sysmemTex ;

		D3D_V( r3dRenderer->pd3ddev->CreateTexture( InternalScreenshotBuffer->Width, InternalScreenshotBuffer->Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &sysmemTex, NULL ) ) ;

		IDirect3DSurface9* sysmemSurf ;
		D3D_V( sysmemTex->GetSurfaceLevel( 0, &sysmemSurf ) ) ;

		D3D_V( r3dRenderer->pd3ddev->GetRenderTargetData( InternalScreenshotBuffer->GetTex2DSurface(), sysmemSurf ) ) ;

		time_t curtime;
		time(&curtime);
		struct tm curdate = *localtime(&curtime);

		char			FileName[512];

		sprintf(FileName, "ScreenInternal_%02d%02d%04d_%02d%02d%02d.tga", curdate.tm_mday, curdate.tm_mon, 1900+curdate.tm_year, curdate.tm_hour, curdate.tm_min, curdate.tm_sec);

		D3D_V( D3DXSaveTextureToFile( FileName, D3DXIFF_TGA, sysmemTex, NULL ) ) ;

		SAFE_RELEASE( sysmemSurf ) ;
		SAFE_RELEASE( sysmemTex ) ;

		SAFE_DELETE( InternalScreenshotBuffer ) ;
		r_do_internal_screenshot->SetInt( 0 ) ;
	}
#endif
}

void SetupFilmToneConstants( int reg )
{
	HUDFilterSettings &hfs = gHUDFilterSettings[r_hud_filter_mode->GetInt()];

	float eb = r_exposure_bias->GetFloat() ;

	if( hfs.overrideHDRControls )
	{
		eb = hfs.hdrExposureBias ;
	}

	enum
	{
		VEC_COUNT = 4
	};

#if 0
	float gamma = 1.0f / r_gamma_pow->GetFloat();
#else
	float gamma = 1.0f / 2.2f;
#endif

	float exp_min = r_exposure_minl->GetFloat();
	float exp_max = r_exposure_maxl->GetFloat();
	float exp_tar = r_exposure_targetl->GetFloat();

	const float MIN_DELTA = 16.0F * FLT_EPSILON;

	if( exp_max < exp_min + MIN_DELTA )
		exp_max = exp_min + MIN_DELTA;

	exp_tar = R3D_MAX( exp_tar, MIN_DELTA );

	float vConsts[ VEC_COUNT ][ 4 ] = {
		// float4      vFilmToneABCD       : register(c0);
		{ r_film_tone_a->GetFloat(), r_film_tone_b->GetFloat(), r_film_tone_c->GetFloat(), r_film_tone_d->GetFloat() },
		// float4      vFilmToneEF_EB_WL   : register(c1);
		{ r_film_tone_e->GetFloat(), r_film_tone_f->GetFloat(), eb, r_white_level->GetFloat() },
		// float4      vFilmToneEXPADJ     : register(c2);
		{ exp_min, exp_max, exp_tar, r_exposure_rangeMax->GetFloat() },
		// float4      vExposure_InvGamma  : register(c3);
		{ r_exposure_rangeMin->GetFloat(), gamma, 0, 0 }
	};

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( reg, vConsts[ 0 ], VEC_COUNT ) );
}

void SetSunParams()
{
	float dawn = r3dGameLevel::Environment->DawnStart;
	float dusk = r3dGameLevel::Environment->DuskEnd;

	Sun->SetParams(	r3dGameLevel::Environment->__CurTime, dawn, dusk,
					r3dGameLevel::Environment->SunDirectionAngle,
					r3dGameLevel::Environment->SunElevationAngle,
					r3dGameLevel::Environment->GetCurrentSunColor(),
					r3dGameLevel::Environment->SunMovementAngleRange );
}

void SetAmbientParams()
{
	float curTime = r3dGameLevel::Environment->__CurTime/24.0f ;
	r3dRenderer->Fog.Color	= r3dGameLevel::Environment->Fog_Color.GetColorValue(curTime) ;

	float4 ambiColor = GetEnvLightAmbient();

	// super hack - store ambient intensity in .w
	r3dRenderer->AmbientColor = r3dColor(	R3D_CLAMP( int( ambiColor.x * 255.f), 0, 255 ),
											R3D_CLAMP( int( ambiColor.y * 255.f), 0, 255 ),
											R3D_CLAMP( int( ambiColor.z * 255.f), 0, 255 ), ambiColor.w*(255.0/4.0)  );
}

SSAOPSIds gSSAOPSIds;

void BlendMax( r3dTexture* src, r3dScreenBuffer* target )
{
	struct D3DMark
	{
		D3DMark()
		{
			D3DPERF_BeginEvent( 0, L"BlendMax" );
		}

		~D3DMark()
		{
			D3DPERF_EndEvent();
		}
	} d3dMark; (void)d3dMark;

	SetRestoreClampAddressMode setRestoreClampAddressMode( 0 ); (void)setRestoreClampAddressMode;
	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	r3d_assert( target );

	target->Activate();

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED );
	r3dRenderer->SetRenderingMode( R3D_BLEND_NZ | R3D_BLEND_MAX | R3D_BLEND_PUSH );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED );

	r3dRenderer->SetPixelShader( "PS_COPY" );

	float vSubPix[4] = {	+0.5f / target->Width,
							+0.5f / target->Height, 0.f, 0.f };

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vSubPix, 1 ) );

	r3dRenderer->SetTex( src, 0 );
	r3dSetFiltering( R3D_POINT, 0 );

	r3dRenderer->SetVertexShader( "VS_POSTFX" );

	r3dDrawFullScreenQuad(false);

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED |
																	D3DCOLORWRITEENABLE_GREEN |
																	D3DCOLORWRITEENABLE_BLUE |
																	D3DCOLORWRITEENABLE_ALPHA );

	target->Deactivate();
}

void CopySSRMipmaps( int i )
{
	IDirect3DSurface9	*dst;
	IDirect3DSurface9 *src;
	D3D_V(gBuffer_SSR.gBuffer_SSR->AsTex2D()->GetSurfaceLevel(i-1, &src));
	D3D_V(gBuffer_SSR.gBuffer_SSR->Tex->AsTex2D()->GetSurfaceLevel(i, &dst));
	D3D_V(r3dRenderer->pd3ddev->StretchRect( src , NULL, dst, NULL, D3DTEXF_LINEAR));

	dst->Release();
	src->Release();
}

void Render_SSR_Reflections()// ( CameraDescriptor * pCamera, GBuffer * pGBuffer, Texture * pCubeMap )
{
	R3DPROFILE_FUNCTION("DR: SSR Reflections");
	D3DPERF_BeginEvent ( 0x0, L"SSRReflectionsBuf" );

	gBuffer_SSR.gBuffer_SSR->Activate(0);

	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_MIRROR );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_MIRROR );

	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_MIRROR );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_MIRROR );

	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_MIRROR );
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_MIRROR );

	IDirect3DVertexDeclaration9* prevDecl;

	prevDecl = d3dc.pDecl;
	d3dc._SetDecl( g_ScreenSpaceQuadVertexDecl );

	gBuffer_SSR.BindFrameTexture();

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, false);

	r3dRenderer->SetVertexShader( "PS_DS_SSR_VS" );
	r3dRenderer->SetPixelShader( "PS_DS_SSR_PS" );

	float _x = 1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetWidth(), _y = 1.f / gBuffer_SSR.gBuffer_SSR->Tex->GetHeight(), pixelWidth = sqrtf( _x * _x + _y * _y ) / r3dRenderer->NearClip;
	const int blendFactor = r_ssr_blendfactor->GetInt();

	float screen_size[4] = { pixelWidth, (float)blendFactor / 15.0f, 0.0f, 0.0f} ;//r3dRenderer->NearClip*r3dRenderer->FarClip, r3dRenderer->FarClip, r3dRenderer->NearClip - r3dRenderer->FarClip };

		// Amount of steps should be 1 and bigger. Bigger is slower!
	r3d_assert(r_ssr_steps->GetInt() > 0);
	const int mSteps = r_ssr_steps->GetInt();

	int intConst[4] = { mSteps, 0, 0, 0};

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantI( 0, intConst, 1 ) );

	D3DXMATRIX psConsts[4];

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 1, screen_size, 1 ) );
	D3DXMatrixTranspose( (D3DXMATRIX*)&psConsts[0], &r3dRenderer->ProjMatrix ) ;
	D3DXMatrixTranspose( (D3DXMATRIX*)&psConsts[1], &r3dRenderer->InvProjMatrix ) ;
	D3DXMatrixTranspose( (D3DXMATRIX*)&psConsts[2], &r3dRenderer->ViewMatrix ) ;
	D3DXMatrixTranspose( (D3DXMATRIX*)&psConsts[3], &r3dRenderer->InvViewMatrix ) ;

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 2, (float*)&psConsts, 16 ) );

	D3DXVECTOR4 proj = D3DXVECTOR4( 2.0f / r3dRenderer->ProjMatrix._11, 2.0f / r3dRenderer->ProjMatrix._22, -1.0f / r3dRenderer->ProjMatrix._11, -1.0f / r3dRenderer->ProjMatrix._22 );
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 18, (float*)&proj, 1 ) );

	// TODO! Maybe need to check states here, but in current state they are exactly what we need!

	//r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_BORDER );
	//r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_BORDER );
	//r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	//r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	//r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	//r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

	r3dSetFiltering( R3D_BILINEAR, 0 );
	r3dSetFiltering( R3D_POINT, 1 );
	r3dSetFiltering( R3D_POINT, 2 );

	r3dDrawFullScreenQuad(false);

	gBuffer_SSR.gBuffer_SSR->Deactivate(0);

	// TODO! Maybe it's woth to return back to this solution, but in general it looks ugly and fast
	// Create mip map chain.

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	d3dc._SetDecl( prevDecl );

	// SSR blur modes: O - disabled, 1 - blur only mip 0, 2 - blur all mips
	bool blurMip0 = r_ssr_mips_blur->GetInt() > 0;// && r_ssr_mips_blur->GetInt() < 3;
	bool blurMips = r_ssr_mips_blur->GetInt() > 1;

	const int blurStrength = r_ssr_mips_blur_strength->GetInt() - 1;
	r3d_assert(blurStrength >= 0 );

	if (blurMip0)
	{
		int mip0blur = blurStrength;

		if (r_ssr_mips_blur->GetInt() == 3)
		{
			//mip0blur = 0;
			g_pPostFXChief->AddFX( gPFX_Copy, PostFXChief::RTT_SCENE_PREVSSR, PostFXChief::RTT_SCENE_SSR );
		}
		else
		{
			g_pPostFXChief->AddFX( *gPFX_BlurH[ mip0blur ], PostFXChief::RTT_SCENE_PREVSSR, PostFXChief::RTT_SCENE_SSR );
			g_pPostFXChief->AddFX( *gPFX_BlurV[ mip0blur ], PostFXChief::RTT_SCENE_SSR, PostFXChief::RTT_SCENE_PREVSSR );

		}

		g_pPostFXChief->Execute( false, true );
	}

	// Blur RT
	if (blurMips)
	{
		for (int i = 1, count = gBuffer_SSR.gBuffer_SSR->Tex->GetNumMipmaps(); i < count; ++i)
		{

			CopySSRMipmaps(i);

			gBuffer_SSR.gBuffer_SSR->SetActiveMip(i);
			gBuffer_SSR.gBuffer_SSR_PreviousFrame->SetActiveMip(i);

			g_pPostFXChief->AddFX( *gPFX_BlurH[ blurStrength ], PostFXChief::RTT_SCENE_PREVSSR, PostFXChief::RTT_SCENE_SSR );
			g_pPostFXChief->AddFX( *gPFX_BlurV[ blurStrength ], PostFXChief::RTT_SCENE_SSR, PostFXChief::RTT_SCENE_PREVSSR );

			g_pPostFXChief->Execute( false, true );

			//if (i == count - 1)
			//{
			//	g_pPostFXChief->AddFX( *gPFX_BlurH[ blurStrength ], PostFXChief::RTT_SCENE_PREVSSR, PostFXChief::RTT_SCENE_SSR );
			//	g_pPostFXChief->AddFX( *gPFX_BlurV[ blurStrength ], PostFXChief::RTT_SCENE_SSR, PostFXChief::RTT_SCENE_PREVSSR );

			//	g_pPostFXChief->Execute( false, true );

				//g_pPostFXChief->AddFX( *gPFX_BlurH[ blurStrength ], PostFXChief::RTT_SCENE_PREVSSR, PostFXChief::RTT_SCENE_SSR );
				//g_pPostFXChief->AddFX( *gPFX_BlurV[ blurStrength ], PostFXChief::RTT_SCENE_SSR, PostFXChief::RTT_SCENE_PREVSSR );

				//g_pPostFXChief->Execute( false, true );

				//g_pPostFXChief->AddFX( *gPFX_BlurH[ blurStrength ], PostFXChief::RTT_SCENE_PREVSSR, PostFXChief::RTT_SCENE_SSR );
				//g_pPostFXChief->AddFX( *gPFX_BlurV[ blurStrength ], PostFXChief::RTT_SCENE_SSR, PostFXChief::RTT_SCENE_PREVSSR );

				//g_pPostFXChief->Execute( false, true );
			//}
		}

		gBuffer_SSR.gBuffer_SSR->SetActiveMip(-1);
		gBuffer_SSR.gBuffer_SSR_PreviousFrame->SetActiveMip(-1);
	}
	else
	{
		for (int i = 1, count = gBuffer_SSR.gBuffer_SSR->Tex->GetNumMipmaps(); i < count; ++i)
		{
				CopySSRMipmaps(i);
		}
	}


	r3dRenderer->StartRender(0);

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, false);

	for (int i=0;i<8;i++)
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	}

	r3dRenderer->SetTex(gBuffer_Normal->Tex,1);
	r3dRenderer->SetTex(gBuffer_Depth->Tex,2);
	r3dRenderer->SetTex(gBuffer_Aux->Tex,3);



	D3DPERF_EndEvent();
}

