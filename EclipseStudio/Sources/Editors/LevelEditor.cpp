#include "r3dPCH.h"
#include <limits>

#ifndef FINAL_BUILD
#include <ShellAPI.h>
#include <psapi.h>
#include <Shlwapi.h>
#pragma comment(lib,"Psapi.lib")
#endif

#include "r3d.h"
#include "r3dLight.h"
#include "r3dBudgeter.h"
#include "r3dUtils.h"
#include "d3dfont.h"

#include "GameCommon.h"
#include "GameLevel.h"
#include "UI/UIimEdit2.h"

#include "r3dProfilerRender.h"

#include "ObjectsCode\World\Lamp.h"

#include "../ObjectsCode/Nature/obj_LocalColorCorrection.h"
#include "../ObjectsCode/Nature/GrassMap.h"
#include "../ObjectsCode/Nature/GrassGen.h"
#include "../ObjectsCode/Nature/GrassLib.h"
#include "../ObjectsCode/Nature/GrassEditorPlanes.h"
#include "../ObjectsCode/WORLD/obj_EnvmapProbe.h"
#include "../ObjectsCode/WORLD/EnvmapProbes.h"
#include "../ObjectsCode/WORLD/WaterPlane.h"
#include "../ObjectsCode/WORLD/obj_Group.h"
#include "../ObjectsCode/WORLD/tree.h"
#include "../ObjectsCode/AI/AI_Player.h"
#include "../ObjectsCode/WORLD/obj_DebugTexture.h"
#include "../ObjectsCode/effects/obj_ParticleSystem.h"
#include "../ObjectsCode/Nature/wind.h"
#include "../ui/FrontendShared.h"
#include "../ui/m_LoadingScreen.h"

#include "Particle.h"

#include "rendering/Deffered/CommonPostFX.h"
#include "ObjectManipulator3d.h"

#include "LevelEditor.h"
#include "ObjectsCode\WORLD\obj_Road.h"

#include "ObjectsCode\world\DecalChief.h"
#include "ObjectsCode\world\MaterialTypes.h"
#include "ObjectsCode\weapons\ExplosionVisualController.h"
#include "ObjectsCode\weapons\HeroConfig.h"
#include "ObjectsCode\weapons\WeaponArmory.h"

#include "Rendering\Deffered\VisibilityGrid.h"
#include "SectorMaster.h"


#include "TerrainMods.h"

#include "LevelEditor_Decorators.h"
#include "DecalProxyObject.h"

#include "DamageLib.h"

#include "XMLHelpers.h"
#include "../RENDERING/Deffered/HUDFilters.h"

#include "../SF/CmdProcessor/CmdProcessor.h"

#include "../ObjectsCode/WORLD/DecalChiefUndoRedoActions.h"
#include "EditedValueTracker.h"
#include "WaterEditorUndoSubsystem.h"
#include "CameraSpotsManager.h"
#include "../ObjectsCode/WORLD/PrefabUndoActions.h"

#include "TrueNature2\Terrain3.h"
#include "TrueNature2\Terrain2.h"

#include "Terrain3Editor.h"
#include "Terrain2Editor.h"

#include "../../../GameEngine/ai/NavGenerationHelpers.h"
#include "../../../GameEngine/ai/AutodeskNav/AutodeskNavMesh.h"
#include "../../../GameEngine/ai/AutodeskNav/AutodeskNavAgent.h"
#include "../ObjectsCode/Gameplay/obj_Zombie.h"
#include "CollectionsManager.h"

#include "../MeshPropertyLib.h"

#include "Helper.hpp"
#include "r3dBackgroundTaskDispatcher.h"

bool g_bEditMode = false;

#include "..\rendering\Deffered\RenderDeffered.h"
extern ShadowSlice ShadowSlices[NumShadowSlices];

#include "../ObjectsCode/WORLD/PrefabManager.h"

#include "../SF/Console/EngineConsole.h"

#include "../ObjectsCode/Gameplay/obj_NoClipBox.h"
#include "../ObjectsCode/Gameplay/obj_NoClipPlane.h"
#include "../../../GameEngine/CSVLog.h"

//////////////////////////////////////////////////////////////////////////

struct SerializableSettingSet
{
	PFX_ExtractGlow::Settings			GlowSettings;
	PFX_CameraMotionBlur::Settings		CamMotionBlurSettings;
	PFX_RadialBlur::Settings			RadialBlurSettings;
	PFX_ObjectMotionBlur::Settings		ObjMotionBlurSettings;
	PFX_SunGlare::Settings				SunGlareSettings;
	PFX_LensDirt::Settings				LensDirtSettings;
	PFX_LensDirt_ExtractLuma::Settings	LensDirtExtractLumaSettings;

	PFX_GodRays::Settings				GodRaysSettings;
	PFX_SeedSunThroughStencil::Settings SunThroughStencilSettings;

	float ExplosionMaxStrength ;
	float ExplosionDuration ;
	float ExplosionMaxDistance ;
	float ExplosionBrightThreshold ;
};

static r3dTexture* ScopeTest_Reticle ;
static r3dTexture* ScopeTest_Normal ;
static r3dTexture* ScopeTest_Mask ;
r3dTexture* ScopeTest_BlurMask ;
r3dTexture* SimpleCrosshair_Reticle = 0;
r3dTexture* SimpleCrosshair_Mask = 0;

DecalGameObjectProxy *gDecalProxyObj = 0;
r3dTerrainPaintBoundControl g_TerraPaintBoundCtrl ;

float _ColorGrading_Levels_midlights[4] = {1.0f, 1.0f, 1.0f, 1.0f};
int _ColorGrading_Levels_shadows[4] = {0, 0, 0, 0};
int _ColorGrading_Levels_hightlights[4] = {255, 255, 255, 255};
int _ColorGrading_Levels_low_output[4] = {0, 0, 0, 0};
int _ColorGrading_Levels_high_output[4] = {255, 255, 255, 255};

static int g_EditedTerrain2Data;
static int Terrain2EditMode;

const static char* terrain2PaintList[  ] =	{ "ERASER",		"BRUSH"		};
enum										{ TE2_ERASE,	TE2_BRUSH	};

static int Terrain2PaintMode = TE2_BRUSH;
static int Terrain2Editor_EraseAll = 0;

//------------------------------------------------------------------------

static int g_EditedTerrain3Data;
static int Terrain3EditMode;

const static char* terrain3PaintList[  ] =	{ "ERASER",		"BRUSH"		};
enum										{ TE3_ERASE,	TE3_BRUSH	};

static int Terrain3PaintMode = TE3_BRUSH;
static int Terrain3Editor_EraseAll = 0;

//------------------------------------------------------------------------

static float g_ObjectsSectionsStarty = 85.0f;

float g_ShowSaveSign;
float g_ShowNoClipExportSign;

float g_fWaterPlaneBrushRadius = 150.0f;

int g_RoadEditingSwtichedOffTerrain2RoadBaking = 0;
int g_RoadEditingSwtichedOffTerrain3RoadBaking = 0;

static r3dMaterial *EditMaterial = NULL;
static r3dMaterial *PrevEditMaterial = NULL;

static void drawRamp(const r3dPoint3D& rp0, const r3dPoint3D& rp1, float w1, float w2);

template <bool Write>
void SerializeBloom( pugi::xml_node node, PFX_ExtractBloom::Settings* settings, int* BlurPasses, BlurTaps* BlurTaps )
{
	const bool W = Write ;

	SerializeXMLVal<W>( "glow_amplify"				, node, &settings->GlowAmplify			);
	SerializeXMLVal<W>( "glow_tint"					, node, &settings->GlowTint				);
	SerializeXMLVal<W>( "glow_threshold"			, node, &settings->GlowThreshold		);	

	SerializeXMLVal<W>( "bloom_threshold"			, node, &settings->Threshold			);
	SerializeXMLVal<W>( "bloom_power"				, node, &settings->Power				);

	SerializeXMLVal<W>( "bloom_tint"				, node, &settings->MultiplyColor		);
	SerializeXMLVal<W>( "bloom_blur_magnitude"		, node, BlurPasses						);
	SerializeXMLVal<W>( "bloom_blur_taps"			, node, (int*)BlurTaps					);
}

template <bool Write>
void SerializeFG( pugi::xml_node node, PFX_FilmGrain::Settings* settings )
{
	const bool W = Write ;

	SerializeXMLVal<W>( "film_grain_scale"		, node, &settings->GrainScale		);
	SerializeXMLVal<W>( "film_grain_strength"	, node, &settings->Strength			);
	SerializeXMLVal<W>( "film_grain_fps"		, node, &settings->FPS				);
}


template <bool Write>
void SerializeCommonSettings( pugi::xml_node root )
{
	const bool W = Write ;

#if 0
	if( pugi::xml_node pfxNode = SerializeXMLNode<W>( root, "postfx" ) )
	{
		if( pugi::xml_node nwNode = SerializeXMLNode<W>( pfxNode, "night_vision" ) )
		{
			HUDFilterSettings &hfs = gHUDFilterSettings[HUDFilter_NightVision];
			SerializeBloom<W>( nwNode, &hfs.bloomSettings, &hfs.bloomBlurMagnitude );
			SerializeFG<W>( nwNode, &hfs.filmGrainSettings );

			SerializeXMLVal<W>( "lut3d_tex"	, nwNode, hfs.colorCorrectionTextureName );
		}		
	}
#endif
}

template < bool Write >
void SetDefaultDepthBias_HW( ShadowSlice& slice )
{

}

template <>
void SetDefaultDepthBias_HW< false > ( ShadowSlice& slice )
{
	slice.depthBias_HW = slice.depthBias * 3.f ;
}

template <bool Write>
void SerializeXMLShadowSlice( pugi::xml_node shadow, int SliceNo )
{
	const bool W = Write ;

	char SliceName[ ] = "slice_0";

	SliceName[ sizeof SliceName - 2 ] += SliceNo ;

	if( pugi::xml_node sliceNode = SerializeXMLNode<W>( shadow, SliceName ) )
	{
		SerializeXMLVal<W>( "depth_bias"		, sliceNode, &ShadowSlices[ SliceNo ].depthBias		);
		SetDefaultDepthBias_HW<W>( ShadowSlices[ SliceNo ] ) ;
		SerializeXMLVal<W>( "depth_bias_hw"		, sliceNode, &ShadowSlices[ SliceNo ].depthBias_HW	);
	}
}

template <bool Write>
void SerializeAOSettings( pugi::xml_node root, const char* childName, SSAOSettings* sts )
{
	const bool W = Write ;

	if( pugi::xml_node aoNode = SerializeXMLNode<W>( root, childName ) )
	{
		SerializeXMLVal<W>( "radius"						, aoNode, &sts->Radius						) ;
		SerializeXMLVal<W>( "depth_range"					, aoNode, &sts->DepthRange					) ;
		SerializeXMLVal<W>( "expand_start"					, aoNode, &sts->RadiusExpandStart			) ;
		SerializeXMLVal<W>( "expand_coef"					, aoNode, &sts->RadiusExpandCoef			) ;
		SerializeXMLVal<W>( "brightness"					, aoNode, &sts->Brightness					) ;
		SerializeXMLVal<W>( "contrast"						, aoNode, &sts->Contrast					) ;
		SerializeXMLVal<W>( "blur_depth_sensitivity"		, aoNode, &sts->BlurDepthSensitivity		) ;
		SerializeXMLVal<W>( "blur_strength"					, aoNode, &sts->BlurStrength				) ;
		SerializeXMLVal<W>( "blur_taps"						, aoNode, &sts->BlurTapCount				) ;
		SerializeXMLVal<W>( "blur_passes"					, aoNode, &sts->BlurPassCount				) ;
		SerializeXMLVal<W>( "blur_gaussian"					, aoNode, &sts->BlurGaussian			) ;
		SerializeXMLVal<W>( "detail_path"					, aoNode, &sts->DetailPathEnable			) ;
		SerializeXMLVal<W>( "detail_strength"				, aoNode, &sts->DetailStrength				) ;
		SerializeXMLVal<W>( "detail_radius"					, aoNode, &sts->DetailRadius				) ;
		SerializeXMLVal<W>( "detail_depth_range"			, aoNode, &sts->DetailDepthRange			) ;
		SerializeXMLVal<W>( "detail_radius_expand_start"	, aoNode, &sts->DetailRadiusExpandStart	) ;
		SerializeXMLVal<W>( "detail_radius_expand_coef"		, aoNode, &sts->DetailRadiusExpandCoef		) ;
		SerializeXMLVal<W>( "detail_fade_out"				, aoNode, &sts->DetailFadeOut				) ;
	}
}

template <bool Write>
void SerializeColorGrading( pugi::xml_node root, const char* childName, int n )
{
	const bool W = Write ;

	if( pugi::xml_node cgNode = SerializeXMLNode<W>( root, childName ) )
	{
		SerializeXMLVal<W>( "midlights"		, cgNode, &_ColorGrading_Levels_midlights[n]	);
		SerializeXMLVal<W>( "shadows"		, cgNode, &_ColorGrading_Levels_shadows[n]		);
		SerializeXMLVal<W>( "hightlights"	, cgNode, &_ColorGrading_Levels_hightlights[n]	);
		SerializeXMLVal<W>( "low_output"	, cgNode, &_ColorGrading_Levels_low_output[n]	);
		SerializeXMLVal<W>( "high_output"	, cgNode, &_ColorGrading_Levels_high_output[n]	);
	}
}

template< bool W >
void Serialize( pugi::xml_node node, r3dSSScatterParams* params )
{
	SerializeXMLVal<W> ( "on"				, node, &params->On									) ;
	SerializeXMLVal<W> ( "distortion"		, node, &params->distortion						) ;
	SerializeXMLVal<W> ( "power"			, node, &params->power						) ;
	SerializeXMLVal<W> ( "scale"			, node, &params->scale					) ;
	SerializeXMLVal<W> ( "ambient"			, node, &params->ambient			) ;
	SerializeXMLVal<W> ( "translucency"		, node, &params->translucency	) ;
}

extern int g_RenderRadialBlur;
extern int   g_cameraMotionBlurEnabled;
extern int g_ObjectMotionBlurEnabled;
extern int g_LensDirtEnabled;
extern float DirectionalStreaks_Strength;
extern float DirectionalStreaks_Length;

template <bool Write>
void SerializeLevelSettingsXML( pugi::xml_node root, SerializableSettingSet* ssSet )
{
	extern float g_DoubleDepthSSAO_Blur;
	extern int gCollectionsCastShadows;
	extern float gCollectionsCastShadowDistance;
	extern float g_DoubleDepthSSAO_BlurSens;

	const bool W = Write ;

	if( pugi::xml_node miscNode = SerializeXMLNode<W>( root, "misc" ) )
	{
		if(g_level_settings_ver->GetInt() <= 2)
		{
			SerializeXMLVal<W>( "minimap_origin"	, miscNode, &GameWorld().m_MinimapOrigin	);
			SerializeXMLVal<W>( "minimap_size"		, miscNode, &GameWorld().m_MinimapSize	);
		}

		SerializeXMLCmdVarF<W>( "shadow_limit_height"	, miscNode, r_shadow_extrusion_limit	);
		SerializeXMLCmdVarF<W>( "near_plane"			, miscNode, r_near_plane				);
		SerializeXMLCmdVarF<W>( "far_plane"				, miscNode, r_far_plane					);

		SerializeXMLCmdVarF<W>( "z_prepass_method"		, miscNode, r_z_prepass_method			);
		SerializeXMLCmdVarF<W>( "z_prepass_dist"		, miscNode, r_z_prepass_dist			);
		SerializeXMLCmdVarF<W>( "z_prepass_area"		, miscNode, r_z_prepass_area			);

		SerializeXMLCmdVarF<W>( "detail_radius"			, miscNode, r_level_detail_radius		);
		SerializeXMLCmdVarF<W>( "grass_radius"			, miscNode, r_grass_view_dist			);

		SerializeXMLCmdVarF<W>( "default_draw_distance"	, miscNode, r_default_draw_distance		);
	}

	if( pugi::xml_node sssNode = SerializeXMLNode<W> ( root, "sss" ) )
	{
		Serialize<W>( sssNode, &gSSSParams );
	}

	if( pugi::xml_node shadowNode = SerializeXMLNode<W>( root, "shadows" ) )
	{
		SerializeXMLVal<W>( "collections_cast_shadows"			, shadowNode, &gCollectionsCastShadows			);
		SerializeXMLVal<W>( "collections_cast_shadow_distance"	, shadowNode, &gCollectionsCastShadowDistance	);

		SerializeXMLVal<W>( "size"						, shadowNode, &ShadowDrawingDistance			);
		SerializeXMLVal<W>( "distance"					, shadowNode, &ShadowSunOffset					);
		SerializeXMLVal<W>( "split_distances_1"			, shadowNode, &ShadowSplitDistancesOpaqueHigh[ 1 ]	);
		SerializeXMLVal<W>( "split_distances_2"			, shadowNode, &ShadowSplitDistancesOpaqueHigh[ 2 ]	);

		SerializeXMLCmdVarF<W>( "lq_distance_coef"		, shadowNode, r_shadow_low_size_coef	);

		SerializeXMLCmdVarF<W>( "blur_phys_range"				, shadowNode, r_shadows_blur_phys_range		);
		SerializeXMLCmdVarF<W>( "blur_bias"						, shadowNode, r_shadows_blur_bias			);
		SerializeXMLCmdVarF<W>( "blur_sense"					, shadowNode, r_shadows_blur_sense			);
		SerializeXMLCmdVarF<W>( "blur_radius"					, shadowNode, r_shadows_blur_radius			);

		SerializeXMLCmdVarF<W>( "spot_light_shadow_bias_hw"	,	shadowNode, r_spot_light_shadow_bias_hw		);
		SerializeXMLCmdVarF<W>( "spot_light_shadow_bias_pcf",	shadowNode, r_spot_light_shadow_bias_pcf	);

		SerializeXMLShadowSlice<W>( shadowNode, 0 );
		SerializeXMLShadowSlice<W>( shadowNode, 1 );
		SerializeXMLShadowSlice<W>( shadowNode, 2 );
	}

	if( pugi::xml_node pfxNode = SerializeXMLNode<W>( root, "postfx" ) )
	{
		HUDFilterSettings &nwHfs = gHUDFilterSettings[HUDFilter_NightVision];
		HUDFilterSettings &defaultHfs = gHUDFilterSettings[HUDFilter_Default];
		if( pugi::xml_node nwBloomNode = SerializeXMLNode<W>( pfxNode, "nw_bloom" ) )
		{
			SerializeBloom<W>( nwBloomNode, &nwHfs.bloomSettings, &nwHfs.bloomBlurPasses, &nwHfs.bloomBlurTaps );
		}

		if( pugi::xml_node nwFGNode = SerializeXMLNode<W>( pfxNode, "nw_film_grain" ) )
		{
			SerializeFG<W>( nwFGNode, &nwHfs.filmGrainSettings);
		}

		SerializeXMLVal<W>( "nw_lut3d_tex", pfxNode, &nwHfs.colorCorrectionTextureNames[ r3dAtmosphere::SKY_PHASE_DAY ] );

		if( !W )
		{
			for( int i = 0, e = r3dAtmosphere::SKY_PHASE_COUNT; i < e; i ++ )
			{
				nwHfs.colorCorrectionTextureNames[ i ] = nwHfs.colorCorrectionTextureNames[ r3dAtmosphere::SKY_PHASE_DAY ];
			}
		}

		SerializeXMLVal<W>( "nw_override_hdr"				, pfxNode, &nwHfs.overrideHDRControls );
		SerializeXMLVal<W>( "nw_hdr_exposure_bias"			, pfxNode, &nwHfs.hdrExposureBias );

		SerializeXMLVal<W>( "nw_override_ambient_and_int"	, pfxNode, &nwHfs.overrideAmbientAndIntensity );
		SerializeXMLVal<W>( "nw_override_ambient"			, pfxNode, &nwHfs.overrideAmbient );
		SerializeXMLVal<W>( "nw_override_intensity"			, pfxNode, &nwHfs.overrideIntensity );

		SerializeXMLVal<W>( "dof_enable"					, pfxNode, &LevelDOF			);
		SerializeXMLVal<W>( "dof_near"						, pfxNode, &_NEAR_DOF				);
		SerializeXMLVal<W>( "dof_far"						, pfxNode, &_FAR_DOF					);
		SerializeXMLVal<W>( "dof_near_start"				, pfxNode, &DepthOfField_NearStart			);
		SerializeXMLVal<W>( "dof_near_end"					, pfxNode, &DepthOfField_NearEnd				);
		SerializeXMLVal<W>( "dof_near_amplify" 				, pfxNode, &DepthOfField_NearAmplify				);
		SerializeXMLVal<W>( "dof_far_start"					, pfxNode, &DepthOfField_FarStart						);
		SerializeXMLVal<W>( "dof_far_end"					, pfxNode, &DepthOfField_FarEnd								);
		SerializeXMLVal<W>( "dof_far_taps"					, pfxNode, &DepthOfField_FarTaps							);
		SerializeXMLVal<W>( "dof_near_taps"					, pfxNode, &DepthOfField_NearTaps							);

		SerializeXMLVal<W>( "film_grain_enable"				, pfxNode, &FilmGrainOn										);

		SerializeFG<W> ( pfxNode, &gFilmGrainSettings ) ;

		SerializeXMLVal<W>( "lens_dirt_enabled"				, pfxNode, &g_LensDirtEnabled								);
		SerializeXMLVal<W>( "lens_dirt_extract_threshold"	, pfxNode, &ssSet->LensDirtExtractLumaSettings.Threshold	);
		SerializeXMLCmdVarI<W>( "lens_dirt_blur_count"		, pfxNode, r_lens_dirt_blur_count							);
		SerializeXMLCmdVarI<W>( "lens_dirt_blur_width"		, pfxNode, r_lens_dirt_blur_width							);
		SerializeXMLVal<W>( "lens_dirt_area"				, pfxNode, &ssSet->LensDirtSettings.Area					);
		SerializeXMLVal<W>( "lens_dirt_glow_strength"		, pfxNode, &ssSet->LensDirtSettings.GlowStrength			);
		SerializeXMLVal<W>( "lens_dirt_light_strength"		, pfxNode, &ssSet->LensDirtSettings.LightStrength			);
		SerializeXMLVal<W>( "lens_dirt_pow"					, pfxNode, &ssSet->LensDirtSettings.Pow						);
		SerializeXMLVal<W>( "lens_dirt_opacity"				, pfxNode, &ssSet->LensDirtSettings.Opacity					);

		SerializeXMLVal<W>( "radial_blur_enable"			, pfxNode, &g_RenderRadialBlur								);
		SerializeXMLVal<W>( "radial_blur_start"				, pfxNode, &ssSet->RadialBlurSettings.BlurStart				);
		SerializeXMLVal<W>( "radial_blur_strength"			, pfxNode, &ssSet->RadialBlurSettings.BlurStrength			);

		SerializeXMLVal<W>( "object_motion_blur_enable"		, pfxNode, &g_ObjectMotionBlurEnabled						);
		SerializeXMLVal<W>( "object_motion_blur_strength"	, pfxNode, &ssSet->ObjMotionBlurSettings.BlurStrength		);

		SerializeXMLVal<W>( "camera_motion_blur_enable"		, pfxNode, &g_cameraMotionBlurEnabled						);
		SerializeXMLVal<W>( "camera_motion_scale"			, pfxNode, &ssSet->CamMotionBlurSettings.MotionScale		);
		SerializeXMLVal<W>( "camera_motion_threshold"		, pfxNode, &ssSet->CamMotionBlurSettings.MotionThreshold	);

		SerializeXMLVal<W>( "bloom_enable"					, pfxNode, &LevelBloom										);

		SerializeBloom<W> ( pfxNode, &defaultHfs.bloomSettings, &defaultHfs.bloomBlurPasses, &defaultHfs.bloomBlurTaps	) ;

		SerializeXMLVal<W>( "dir_streaks_strength"			, pfxNode, &DirectionalStreaks_Strength						);
		SerializeXMLVal<W>( "dir_streaks_length"			, pfxNode, &DirectionalStreaks_Length						);

		SerializeXMLVal<W>( "glow_multipiler"				, pfxNode, &ssSet->GlowSettings.Multiplier					);

		SerializeXMLVal<W>( "sun_rays_enable"				, pfxNode, &LevelSunRays									);

		SerializeXMLVal<W>( "sun_rays_density"				, pfxNode, &ssSet->GodRaysSettings.Density					);
		SerializeXMLVal<W>( "sun_rays_decay"				, pfxNode, &ssSet->GodRaysSettings.Decay					);
		SerializeXMLVal<W>( "sun_rays_exposure"				, pfxNode, &ssSet->GodRaysSettings.Exposure					);

		SerializeXMLVal<W>( "sun_rays_weight"				, pfxNode, &ssSet->SunThroughStencilSettings.Weight			);
		SerializeXMLVal<W>( "sun_rays_radius"				, pfxNode, &ssSet->SunThroughStencilSettings.Radius			);

		SerializeXMLVal<W>( "explosion_max_strength"		, pfxNode, &ssSet->ExplosionMaxStrength						);
		SerializeXMLVal<W>( "explosion_duration"			, pfxNode, &ssSet->ExplosionDuration						);
		SerializeXMLVal<W>( "explosion_max_distance"		, pfxNode, &ssSet->ExplosionMaxDistance						);
		SerializeXMLVal<W>( "explosion_bright_threshold"	, pfxNode, &ssSet->ExplosionBrightThreshold					);

		SerializeXMLVal<W>( "mlaa_discont_factor"			, pfxNode, &MLAA_DiscontFactor					);

		SerializeXMLCmdVarF<W>( "r_film_tone_a"				, pfxNode, r_film_tone_a						);
		SerializeXMLCmdVarF<W>( "r_film_tone_b"				, pfxNode, r_film_tone_b						);
		SerializeXMLCmdVarF<W>( "r_film_tone_c"				, pfxNode, r_film_tone_c						);
		SerializeXMLCmdVarF<W>( "r_film_tone_d"				, pfxNode, r_film_tone_d						);
		SerializeXMLCmdVarF<W>( "r_film_tone_e"				, pfxNode, r_film_tone_e						);
		SerializeXMLCmdVarF<W>( "r_film_tone_f"				, pfxNode, r_film_tone_f						);

		SerializeXMLCmdVarF<W>( "r_exposure_bias"			, pfxNode, r_exposure_bias						);
		SerializeXMLCmdVarF<W>( "r_white_level"				, pfxNode, r_white_level						);

		SerializeXMLCmdVarF<W>( "r_exposure_minl"			, pfxNode, r_exposure_minl						);
		SerializeXMLCmdVarF<W>( "r_exposure_maxl"			, pfxNode, r_exposure_maxl						);
		SerializeXMLCmdVarF<W>( "r_exposure_targetl"		, pfxNode, r_exposure_targetl					);
		SerializeXMLCmdVarF<W>( "r_exposure_range"			, pfxNode, r_exposure_rangeMax						);
		SerializeXMLCmdVarF<W>( "r_exposure_rangeMin"			, pfxNode, r_exposure_rangeMin						);
		SerializeXMLCmdVarF<W>( "r_light_adapt_speed_pos"	, pfxNode, r_light_adapt_speed_pos				);
		SerializeXMLCmdVarF<W>( "r_light_adapt_speed_neg"	, pfxNode, r_light_adapt_speed_neg				);

		SerializeXMLVal<W>( "clouds_shadows_enable"			, pfxNode, &CloudsShadows_Enable				);
		SerializeXMLVal<W>( "clouds_shadows_tex_name"		, pfxNode, CloudsShadows_TexName				);
		SerializeXMLVal<W>( "clouds_shadows_blend_factor"	, pfxNode, &CloudsShadows_BlendFactor			);

		SerializeXMLVal<W>( "clouds_shadows_mask_tiling"	, pfxNode, &CloudsShadows_Mask.fTiling			);
		SerializeXMLVal<W>( "clouds_shadows_mask_sspeedu"	, pfxNode, &CloudsShadows_Mask.fScrollSpeedU	);
		SerializeXMLVal<W>( "clouds_shadows_mask_sspeedv"	, pfxNode, &CloudsShadows_Mask.fScrollSpeedV	);

		SerializeXMLVal<W>( "clouds_shadows_noise1_tiling"	, pfxNode, &CloudsShadows_Noise1.fTiling		);
		SerializeXMLVal<W>( "clouds_shadows_noise1_sspeedu"	, pfxNode, &CloudsShadows_Noise1.fScrollSpeedU	);
		SerializeXMLVal<W>( "clouds_shadows_noise1_sspeedv"	, pfxNode, &CloudsShadows_Noise1.fScrollSpeedV	);

		SerializeXMLVal<W>( "clouds_shadows_noise2_tiling"	, pfxNode, &CloudsShadows_Noise2.fTiling		);
		SerializeXMLVal<W>( "clouds_shadows_noise2_sspeedu"	, pfxNode, &CloudsShadows_Noise2.fScrollSpeedU	);
		SerializeXMLVal<W>( "clouds_shadows_noise2_sspeedv"	, pfxNode, &CloudsShadows_Noise2.fScrollSpeedV	);

		SerializeAOSettings<W>( pfxNode, "ao_default"	, &g_SSAOSettings[ SSM_DEFAULT ]	);
		SerializeAOSettings<W>( pfxNode, "ao_hq"		, &g_SSAOSettings[ SSM_HQ ]			);

		SerializeColorGrading<W>( pfxNode, "color_grading_0", 0 );
		SerializeColorGrading<W>( pfxNode, "color_grading_1", 1 );
		SerializeColorGrading<W>( pfxNode, "color_grading_2", 2 );
		SerializeColorGrading<W>( pfxNode, "color_grading_3", 3 );

		if( pugi::xml_node ccNode = SerializeXMLNode<W>( pfxNode, "color_correction" ) )
		{
			SerializeXMLVal<W>( "enable"		, ccNode, &defaultHfs.enableColorCorrection			);

			SerializeXMLVal<W>( "scheme"			, ccNode, (int*)&g_ColorCorrectionSettings.scheme	);

			HUDFilterSettings& sts = gHUDFilterSettings[HUDFilter_Default];

			if( !W )
			{
				r3dString oneToMany;
				SerializeXMLVal<W>( "lut3d_tex", ccNode, &oneToMany );
				sts.SetAllColorCorrectionTexturesTo( oneToMany );
			}

			SerializeXMLVal<W>( "lut3d_tex_dawn"	, ccNode, &sts.colorCorrectionTextureNames[ r3dAtmosphere::SKY_PHASE_DAWN ]	);
			SerializeXMLVal<W>( "lut3d_tex_day"		, ccNode, &sts.colorCorrectionTextureNames[ r3dAtmosphere::SKY_PHASE_DAY ]	);
			SerializeXMLVal<W>( "lut3d_tex_dusk"	, ccNode, &sts.colorCorrectionTextureNames[ r3dAtmosphere::SKY_PHASE_DUSK ]	);
			SerializeXMLVal<W>( "lut3d_tex_night"	, ccNode, &sts.colorCorrectionTextureNames[ r3dAtmosphere::SKY_PHASE_NIGHT ]	);

			SerializeXMLCurve3f<W>( "r_curve"	, ccNode, &g_ColorCorrectionSettings.RGBCurves[ 0 ]	);
			SerializeXMLCurve3f<W>( "g_curve"	, ccNode, &g_ColorCorrectionSettings.RGBCurves[ 1 ]	);
			SerializeXMLCurve3f<W>( "b_curve"	, ccNode, &g_ColorCorrectionSettings.RGBCurves[ 2 ]	);

			SerializeXMLCurve3f<W>( "h_curve"	, ccNode, &g_ColorCorrectionSettings.HSVCurves[ 0 ]	);
			SerializeXMLCurve3f<W>( "s_curve"	, ccNode, &g_ColorCorrectionSettings.HSVCurves[ 1 ]	);
			SerializeXMLCurve3f<W>( "v_curve"	, ccNode, &g_ColorCorrectionSettings.HSVCurves[ 2 ]	);
		}

		if( pugi::xml_node sunGlareNode = SerializeXMLNode<W>( pfxNode, "sun_glare" ) )
		{
			SerializeXMLVal<W>( "enable", sunGlareNode, &_SunGlare ) ;
			SerializeXMLVal<W>( "num_slices", sunGlareNode, &SG_SlicesNum ) ;

			for( int i = 0, e = std::min( SG_SlicesNum, (int)MAX_SUNGLARES ) ; i < e ; i ++ )
			{
				char SliceNodeName[] = { "slice_00" } ;

				SliceNodeName[ sizeof SliceNodeName - 3 ] += i / 10 ;
				SliceNodeName[ sizeof SliceNodeName - 2 ] += i % 10 ;

				if( pugi::xml_node sliceNode = SerializeXMLNode<W>( sunGlareNode, SliceNodeName ) )
				{
					SerializeXMLVal<W>( "tint"			, sliceNode, &ssSet->SunGlareSettings.Tint[ i ]			);
					SerializeXMLVal<W>( "opacity"		, sliceNode, &ssSet->SunGlareSettings.Opacity[ i ]		);
					SerializeXMLVal<W>( "blur_scale"	, sliceNode, &ssSet->SunGlareSettings.BlurScale	[ i ]	);
					SerializeXMLVal<W>( "tex_scale"		, sliceNode, &ssSet->SunGlareSettings.TexScale	[ i ]	);
					SerializeXMLVal<W>( "threshold"		, sliceNode, &ssSet->SunGlareSettings.Threshold	[ i ]	);
				}
			}
		}
		SerializeXMLVal<W>( "fxaa_subpix_quality"			, pfxNode, &gPFX_FXAA.subpixQuality					);
		SerializeXMLVal<W>( "fxaa_edge_threshold"			, pfxNode, &gPFX_FXAA.edgeThreshold					);
		SerializeXMLVal<W>( "fxaa_edge_threshold_min"		, pfxNode, &gPFX_FXAA.edgeThresholdMin				);
	}

	if( pugi::xml_node ssaoNode = SerializeXMLNode<W>( root, "double_depth" ) )
	{
		SerializeXMLCmdVarI<W>( "ao_double_depth_ssao"			, ssaoNode, r_double_depth_ssao				);
		SerializeXMLVal<W>( "ao_double_depth_ssao_blur"			, ssaoNode, &g_DoubleDepthSSAO_Blur			);
		SerializeXMLVal<W>( "ao_double_depth_ssao_blur_sens"	, ssaoNode, &g_DoubleDepthSSAO_BlurSens		);
	}
}

void RepositionObjectsOnTerrain( const r3dPoint3D &vPos, float fRadius );

namespace
{
	struct TerrainDirtyness
	{
		void Reset( int dimX, int dimZ )
		{
			DimX = dimX ;
			DimZ = dimZ ;

			DirtyField.Resize( 0, 0 );
			DirtyField.Resize( DimX, DimZ, 0 );

			const r3dTerrainDesc& terraDesc = Terrain->GetDesc() ;

			CellSizeX = terraDesc.CellSize * terraDesc.CellCountX / DimX;
			CellSizeZ = terraDesc.CellSize * terraDesc.CellCountZ / DimZ;
		}

		void Add( float x, float z, float rad )
		{
			int sx = int( (x - rad) / CellSizeX );
			int sz = int( (z - rad) / CellSizeZ );
			int ex = int( (x + rad) / CellSizeX );
			int ez = int( (z + rad) / CellSizeZ );

			sx = R3D_MAX( sx, 0 );
			sz = R3D_MAX( sz, 0 );

			ex = R3D_MIN( ex, DimX - 1 );
			ez = R3D_MIN( ez, DimZ - 1 );

			for( int z = sz; z <= ez; z ++ )
			{
				for( int x = sx; x <= ex; x ++ )
				{
					DirtyField[ z ][ x ] = 1;
				}
			}
		}

		float			CellSizeX;
		float			CellSizeZ;

		int				DimX;
		int				DimZ;

		r3dTL::T2DArray< int >	DirtyField;
	};

	struct TerrainHeightDirtiness : TerrainDirtyness
	{
		void Update( bool terrain2 )
		{
			float rad = sqrtf( CellSizeX*CellSizeX + CellSizeZ *CellSizeZ );


			for( int z = 0, e = DimZ; z < e ; z ++ )
			{
				for( int x = 0, e = DimX; x < e ; x ++ )
				{
					if( DirtyField[ z ][ x ] )
					{
						float wx = ( x + 0.5f ) * CellSizeX;
						float wz = ( z + 0.5f ) * CellSizeZ;

						g_pGrassMap->UpdateHeight( wx, wz, rad );

						r3dPoint3D targetPos;
						targetPos.x = wx;
						targetPos.z = wz;

						targetPos.y = Terrain->GetHeight( targetPos );
						RepositionObjectsOnTerrain( targetPos, rad );

					}
				}
			}

		}

	} gTerrainHeightDirtiness;


	char EnumLayerName[] = { "Layer 0" };

	char* GetLayerName( int idx )
	{
		char* MatLayerName( NULL );

		if( idx )
		{
			EnumLayerName[ sizeof EnumLayerName - 2 ] = '0' + (idx - 1) % TERRAIN_LAYERS_PER_MAT;
			MatLayerName = EnumLayerName;
		}
		else
		{
			MatLayerName = "Base Layer";
		}

		return MatLayerName;
	}
}

void SaveXMLSettings( const char* targetDir );
void SaveEnvironmentToXML( const char* fileName );
int LoadEnvironmentFromXML( const char* fileName );

int _ColorGrading = 0;
int _Motionblur = 0;
int _NEAR_DOF = 1;
int _FAR_DOF = 1;
int _UsedBloomBlurPasses = 2;
int _UsedBloomBlurTaps = BT_17 ;

int SG_SlicesNum = 0; // limited to 10
int _SunGlare=0;

r3dSSScatterParams gSSSParams ;

stringlist_t gErosionPatternTextureNames;

bool ReadTimeCurve( FILE* fin, r3dTimeGradient2& curve )
{
	int count;
	if( fscanf( fin, "point_count %d\n", &count ) != 1 )
		return false;

	if( count < 0 || count > curve.MAX_VALUES )
		return false;

	for( int i = 0, e = count; i < e; i ++ )
	{
		float time, value;
		if( fscanf( fin, "(%f,%f)\n", &time, &value ) != 2 )
			return false;

		curve.AddValue( time, value );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------
void WriteTimeCurve( FILE * hFile, r3dBezierGradient& curve, const char * szName )
{
	fprintf( hFile, "%s\n{\n", szName );
	for( int i = 0; i < curve.NumValues; i ++ )
	{
		fprintf( hFile, "\tval: %f %f %f %f\n", curve.Values[ i ].time, curve.Values[ i ].val[ 0 ], curve.Values[ i ].val[ 1 ], curve.Values[ i ].val[ 2 ] );
	}

	fprintf( hFile, "\tsmooth: %d\n", 0/*curve.Smooth*/ );
	fprintf( hFile, "}\n" );

}

void WriteTimeCurve( FILE * hFile, r3dTimeGradient2& curve, const char * szName )
{
	fprintf( hFile, "%s\n{\n", szName );
	for( int i = 0; i < curve.NumValues; i ++ )
	{
		fprintf( hFile, "\tval: %f %f %f %f\n", curve.Values[ i ].time, curve.Values[ i ].val[ 0 ], curve.Values[ i ].val[ 1 ], curve.Values[ i ].val[ 2 ] );
	}

	fprintf( hFile, "\tsmooth: %d\n", curve.Smooth );
	fprintf( hFile, "\n}\n\n" );

}

//--------------------------------------------------------------------------------------------------------
void ReadTimeCurveNew( Script_c &script, r3dBezierGradient& curve )
{
	int nCount = 0;
	char buffer[ MAX_PATH ];


	script.SkipToken( "{" );
	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( *buffer == '}' )
			break;

		if ( ! strcmp( buffer, "val:" ) )
		{
			assert( nCount < curve.MAX_VALUES );
			if ( nCount >= curve.MAX_VALUES )
				continue;

			float fTime = script.GetFloat();
			float fVal1 = script.GetFloat();
			float fVal2 = script.GetFloat();
			float fVal3 = script.GetFloat();

			curve.AddValue3f( fTime, fVal1, fVal2, fVal3 );
			nCount++;
		}
	}
}

void ReadTimeCurveNew( Script_c &script, r3dTimeGradient2& curve )
{
	int nCount = 0;
	char buffer[ MAX_PATH ];

	curve.Smooth = false;

	script.SkipToken( "{" );
	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( *buffer == '}' )
			break;

		if ( ! strcmp( buffer, "val:" ) )
		{
			assert( nCount <= curve.MAX_VALUES );
			if ( nCount > curve.MAX_VALUES )
				continue;

			float fTime = script.GetFloat();
			float fVal1 = script.GetFloat();
			float fVal2 = script.GetFloat();
			float fVal3 = script.GetFloat();

			curve.AddValue3f( fTime, fVal1, fVal2, fVal3 );
			nCount++;
		}

		if( !strcmp( buffer, "smooth:") )
		{
			curve.Smooth = script.GetInt() ? true : false;
		}
	}
}

void UpdateLowShadowSplitDistances()
{
	extern float ShadowCameraNear ;

	ShadowSplitDistancesOpaqueHigh[0]=ShadowCameraNear;
	ShadowSplitDistancesOpaqueHigh[NumShadowSlices]=ShadowDrawingDistance;

	ShadowSplitDistancesOpaqueMed[0]=ShadowCameraNear;
	ShadowSplitDistancesOpaqueMed[NumShadowSlices]=ShadowDrawingDistance;

	ShadowSplitDistancesOpaqueLow[0]=ShadowCameraNear;
	ShadowSplitDistancesOpaqueLow[NumShadowSlices]=ShadowDrawingDistance;

	ShadowSplitDistancesOpaque_DynamicCubemap[0]=ShadowCameraNear;
	ShadowSplitDistancesOpaque_DynamicCubemap[NumShadowSlices]=ShadowDrawingDistance;


	ShadowSplitDistancesOpaqueMed[1] = ShadowSplitDistancesOpaqueHigh[1];
	ShadowSplitDistancesOpaqueMed[2] = ShadowSplitDistancesOpaqueHigh[2];
	ShadowSplitDistancesOpaqueMed[3] = ShadowSplitDistancesOpaqueHigh[3];

#if 0
	ShadowSplitDistancesOpaqueLow[1] = ShadowSplitDistancesOpaqueHigh[3] * r_shadow_low_size_coef->GetFloat() ;
	ShadowSplitDistancesOpaqueLow[2] = ShadowSplitDistancesOpaqueLow[1];
	ShadowSplitDistancesOpaqueLow[3] = ShadowSplitDistancesOpaqueLow[2];
#else
	ShadowSplitDistancesOpaqueLow[1] = ShadowSplitDistancesOpaqueHigh[2] * 0.5f;
	ShadowSplitDistancesOpaqueLow[2] = ShadowSplitDistancesOpaqueHigh[3] * 0.7f;
	ShadowSplitDistancesOpaqueLow[3] = ShadowSplitDistancesOpaqueLow[2];

	ShadowSplitDistancesOpaque_DynamicCubemap[1] = ShadowSplitDistancesOpaque_DynamicCubemap[NumShadowSlices];
	ShadowSplitDistancesOpaque_DynamicCubemap[2] = ShadowSplitDistancesOpaque_DynamicCubemap[NumShadowSlices];
#endif
}

#include "r3dDeviceQueue.h"

extern r3dTexture* CC_LUT1D_RGB_Tex;
extern r3dTexture* CC_LUT1D_HSV_Tex;
void DoUpdateColorCorrectionTextures( void* )
{
	R3D_ENSURE_MAIN_THREAD();

	r3dBezierGradient *targetCurves = g_ColorCorrectionSettings.RGBCurves;
	FillColorCorrectionTexture( targetCurves[0], targetCurves[1], targetCurves[2], false, CC_LUT1D_RGB_Tex );

	targetCurves = g_ColorCorrectionSettings.HSVCurves;
	FillColorCorrectionTexture( targetCurves[0], targetCurves[1], targetCurves[2], true, CC_LUT1D_HSV_Tex );
}


void UpdateColorCorrectionTextures()
{
	ProcessCustomDeviceQueueItem( DoUpdateColorCorrectionTextures, 0 ) ;
}

int LoadLevelSettingsXML( pugi::xml_node root )
{
	SerializableSettingSet ssSet ;

	SerializeLevelSettingsXML< false >( root, &ssSet ) ;

	// only SSM_HQ supports detail path
	g_SSAOSettings[ SSM_DEFAULT ].DetailPathEnable = 0 ;

	RestoreCCLUT3DTexture();

	UpdateColorCorrectionTextures();

	if( g_ColorCorrectionSettings.scheme == ColorCorrectionSettings::CCS_USE_RGB_HSV_CURVES )
		g_ColorCorrectionSettings.uiScheme = 0;
	else
		g_ColorCorrectionSettings.uiScheme = g_ColorCorrectionSettings.scheme;

	ssSet.RadialBlurSettings.Restrict();

	gPFX_ExtractGlow.SetSettings( ssSet.GlowSettings );
	gPFX_CameraMotionBlur.SetSettings( ssSet.CamMotionBlurSettings );
	gPFX_RadialBlur.SetDefaultSettings( ssSet.RadialBlurSettings );
	gPFX_ObjectMotionBlur.SetSettings( ssSet.ObjMotionBlurSettings );
	gPFX_GodRays.SetSettings( ssSet.GodRaysSettings );
	gPFX_SeedSunThroughStencil.SetSettings( ssSet.SunThroughStencilSettings );

	gPFX_LensDirt.SetSettings( ssSet.LensDirtSettings );
	gPFX_LensDirt_ExtractLuma.SetSettings( ssSet.LensDirtExtractLumaSettings );

	ssSet.SunGlareSettings.NumSunglares = SG_SlicesNum;
	gPFX_SunGlare.SetSettings( ssSet.SunGlareSettings );
	UpdateLowShadowSplitDistances();

	g_SSAOConstraints[ SSM_DEFAULT ].Restrict( g_SSAOSettings[ SSM_DEFAULT ] );
	g_SSAOConstraints[ SSM_HQ ].Restrict( g_SSAOSettings[ SSM_HQ ] );

	gExplosionVisualController.SetDefaultMaxStrength( ssSet.ExplosionMaxStrength ) ;
	gExplosionVisualController.SetDefaultDuration( ssSet.ExplosionDuration ) ;
	gExplosionVisualController.SetMaxVisibleDistance( ssSet.ExplosionMaxDistance ) ;
	gExplosionVisualController.SetDefaultBrightThreshold( ssSet.ExplosionBrightThreshold ) ;

	void SyncLightingAndSSAO();
	SyncLightingAndSSAO();

	return 1;
};

int LoadXMLSettings()
{
	char FName[ 512 ] ;

	FName[ sizeof FName - 1 ] = 0;

	_snprintf( FName, sizeof FName - 1, LEVEL_SETTINGS_FILE, r3dGameLevel::GetHomeDir() ) ;

	bool XMLExists = false;

	Bytes xmlFileBuffer;

	pugi::xml_document xmlLevelFile;
	pugi::xml_node xmlRoot ;

	r3dFile* f = r3d_open( FName, "rb" );
	if ( f )
	{
		r3dOutToLog( "Loading '%s'\n", FName ) ;
		XMLExists = true ;

		if( !ParseXMLInMemory( f, &xmlFileBuffer, &xmlLevelFile ) )
		{
			fclose( f );
			return 0 ;
		}

		xmlRoot = xmlLevelFile.root().child( "root" ) ;

		fclose( f );
	}
	else
	{
		r3dOutToLog( "Couldn't open '%s'\n", FName ) ;
	}


	int levelXmlLoadSuccess = 0 ;

	if( XMLExists && xmlRoot.attribute("version").as_int() >= 2 )
	{
		g_level_settings_ver->SetInt( xmlRoot.attribute("version").as_int() ) ;

		levelXmlLoadSuccess = LoadLevelSettingsXML( xmlRoot );

		if( !levelXmlLoadSuccess )
			r3dOutToLog( "LoadLevelSettingsXML failed.\n" ) ;

		r3dGameLevel::Environment->LoadFromXML( xmlRoot );
	}

	int LoadCommonSettings();
	int commXmlLoadSuccess = LoadCommonSettings();

	if( !commXmlLoadSuccess )
		r3dOutToLog( "LoadCommonSettings failed.\n" ) ;

	void UpdateHUDFilterSettings( int*, int* );
	UpdateHUDFilterSettings( 0, 0 );

	return levelXmlLoadSuccess && commXmlLoadSuccess ;
}

void InitSectorMaster()
{
	g_pSectorMaster->Reset();

	if( r_sector_loading->GetInt() )
	{
		Sector::MeshArr meshes;
		meshes.Reserve( 256 );

		ObjectManager& GW = GameWorld();

		for( int i = 0, e = GW.GetStaticObjectCount(); i < e; i ++ )
		{
			meshes.Clear();
			SectorMaster::ExtractMeshes( &meshes,  GW.GetStaticObject( i ) );

			for( int i = 0, e = meshes.Count(); i < e; i ++ )
			{
				meshes[ i ]->TouchMaterials();
			}
		}
	}
}

int LoadLevel( float startLoadingProgress )
{
	float LoadProgress = startLoadingProgress;

	g_MeshPropertyLib->Load( "Data/", true );
	g_MeshPropertyLib->Load( r3dGameLevel::GetHomeDir(), false );

	r3dPurgeArtBugs();

	g_pWind->Load(r3dGameLevel::GetHomeDir());

	SetLoadingProgress( LoadProgress += 0.015f );

	char FName[ 512 ];

	FName[ sizeof FName - 1 ] = 0;

	_snprintf( FName, sizeof FName - 1, "%s\\vars.ini", r3dGameLevel::GetHomeDir() );

	if( !r3d_access( FName, 4 ) )
	{
		ExecVarIni( FName );
	}

	g_level_settings_ver->SetInt( 0 ) ;

	extern bool gNewLevelCreated ;

	if ( !LoadXMLSettings() && !gNewLevelCreated )
	{
		r3dError("Failed to load XML settings!");
		//r3dGameLevel::Environment->Load(r3dGameLevel::GetHomeDir());
		//LoadPostprocessSettingsINI();
	}

	SetLoadingProgress( LoadProgress += 0.015f );

	g_pDecalChief->LoadLevel( r3dGameLevel::GetHomeDir() );
	// need to reload to fetch new ids from possibly changed decal lib.
	g_pMaterialTypes->Load();

	SetLoadingProgress( LoadProgress += 0.015f );

	SetLoadingProgress( LoadProgress += 0.015f );

	r3dOutToLog( "LoadLevel_MatLibs...\n" );
	LoadLevel_MatLibs();

	SetLoadingProgress( LoadProgress += 0.015f );

	r3dOutToLog( "LoadLevel_Objects...\n" );

	float GROUPS_PROGRESS = 0.015f;

	LoadLevel_Objects( PROGRESS_LOAD_LEVEL_END - LoadProgress - GROUPS_PROGRESS );

	LoadProgress = GetLoadingProgress();

#ifndef FINAL_BUILD
	if(g_bEditMode)
	{
		r3dOutToLog( "LoadLevel_Groups...\n" );
		LoadLevel_Groups();
	}
#endif

	SetLoadingProgress( LoadProgress += GROUPS_PROGRESS );

	//------------------------------------------------------------------------	

	{
		if (r3dGameLevel::GetGassEnable()) // Disable o enable Grass on server
		{
			LoadGrassLib();

			r3dOutToLog( "g_pGrassMap->Load...\n" );
			g_pGrassMap->Load( r3dGameLevel::GetHomeDir() );
#ifndef	FINAL_BUILD
			GetGrassPlaneManager()->Load(r3dGameLevel::GetHomeDir());
#endif
		}
	}

	//------------------------------------------------------------------------	

#if ENABLE_AUTODESK_NAVIGATION
	{
		r3dOutToLog( "gAutodeskNavMesh.Load...\n" );
		gAutodeskNavMesh.Init();
	}
#endif // ENABLE_AUTODESK_NAVIGATION

	//------------------------------------------------------------------------	

	r3dOutToLog( "LoadLevel_MatLibs...\n" );

	extern r3dTexture* gCloudTexture;
	if ( gCloudTexture )
	{
		r3dRenderer->DeleteTexture(gCloudTexture);
		gCloudTexture = NULL;
	}
	if ( r3dFileExists( CloudsShadows_TexName ) )
		gCloudTexture = r3dRenderer->LoadTexture( CloudsShadows_TexName );

	SetLoadingProgress( PROGRESS_LOAD_LEVEL_END );

	ResetShadowCache();

#ifndef FINAL_BUILD
	if( g_pVisibilityGrid )
	{
		g_pVisibilityGrid->Load();
	}
#endif

#ifndef FINAL_BUILD
	if( gNewLevelCreated )
	{
		LoadEnvironmentFromXML("Data/EnvPresets/NewLevel.xml");
		SaveXMLSettings( r3dGameLevel::GetHomeDir() );
		gNewLevelCreated = false ;
	}
#endif

	r3dMesh::FlushLoadingBuffers() ;

#ifndef FINAL_BUILD
	r3dProfileRender::Instance()->SetLevelFolder( r3dGameLevel::GetHomeDir() ) ;
#endif

	if( g_bEditMode )
	{
		r_hardware_shadow_method->SetChangeCallback(&HardwareShadowMapsChangeCallback);
		r_shadow_blur->SetChangeCallback(&ShadowBlurChangeCallback);

		r3dShowArtBugs() ;
	}

#ifndef FINAL_BUILD
	if( Terrain2 )
	{
		 Terrain2->UpdateRoadInfo() ;
	}
#endif

#if R3D_ALLOW_SECTOR_LOADING
	InitSectorMaster();
#endif

	GameWorld().JustLoaded = 1;

	if( Terrain )
	{
#ifndef FINAL_BUILD
		extern float __WorldScale;
		__WorldScale = Terrain->GetDesc().WorldSize / 100.f;
#endif
	}

#ifndef FINAL_BUILD
	if( g_bEditMode )
	{
		r3dTL::TArray< GameObject* > allObjects;

		allObjects.Reserve( 8192 );

		for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects(iter) )
		{
			allObjects.PushBack( iter.current );
		}

		for( int j = 0, e = (int)allObjects.Count(); j < e; j ++ )
		{
			GameObject* obj0 = allObjects[ j ];

			for( int i = j + 1, e = (int)allObjects.Count(); i < e; i ++ )
			{
				GameObject* obj1 = allObjects[ i ];

				const r3dPoint3D& p0 = obj0->GetPosition();
				const r3dPoint3D& p1 = obj1->GetPosition();

				const float epsilon = 16 * FLT_EPSILON;

				if( obj0->ObjTypeFlags & OBJTYPE_Mesh
					&&
					obj1->ObjTypeFlags & OBJTYPE_Mesh
					&& ! Terrain2
					)
				{
					if( fabs( p0.x - p1.x ) < epsilon
							&& 
						fabs( p0.y - p1.y ) < epsilon
							&&
						fabs( p0.z - p1.z ) < epsilon						
							)
					{
						if( ((MeshGameObject*)obj0)->GetObjectMesh() == 
							((MeshGameObject*)obj1)->GetObjectMesh()
							)
						{
							r3dArtBug(	"Object at [ %.2f, %.2f, %.2f ] with mesh %s has duplicate!\n",
										p0.x, p0.y, p0.z,
										((MeshGameObject*)obj0)->GetObjectMesh()->FileName.c_str()
										);
						}
					}
				}
			}
		}
	}
#endif

#if 0
	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		for( ObjectIterator iter2 = GameWorld().GetFirstOfAllObjects(); iter2.current ; iter2 = GameWorld().GetNextOfAllObjects( iter2 ) )
		{
			if( iter.current == iter2.current )
				continue;

			if( iter.current->ID == iter2.current->ID )
			{
				r3dOutToLog( "Duplicate object id (%d)!\n", iter.current->ID.get() );
			}
		}
	}
#endif

	return 1;
}

int DeleteObjectDuplicates()
{
#ifndef FINAL_BUILD
	r3dTL::TArray< GameObject* > allObjects;
	typedef std::set< GameObject* > ObjectSet;
	ObjectSet objectsForDeletion;

	allObjects.Reserve( 8192 );

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects(iter) )
	{
		allObjects.PushBack( iter.current );
	}

	for( int j = 0, e = (int)allObjects.Count(); j < e; j ++ )
	{
		GameObject* obj0 = allObjects[ j ];

		if( obj0->ownerPrefab )
			continue;

		for( int i = j + 1, e = (int)allObjects.Count(); i < e; i ++ )
		{
			GameObject* obj1 = allObjects[ i ];

			if( obj1->ownerPrefab )
				continue;

			const r3dPoint3D& p0 = obj0->GetPosition();
			const r3dPoint3D& p1 = obj1->GetPosition();

			const float epsilon = 16 * FLT_EPSILON;

			if( obj0->ObjTypeFlags & OBJTYPE_Mesh
					&&
				obj1->ObjTypeFlags & OBJTYPE_Mesh
				)
			{
				MeshGameObject* mgo0 = static_cast< MeshGameObject* >( obj0 );
				MeshGameObject* mgo1 = static_cast< MeshGameObject* >( obj1 );

				if( fabs( p0.x - p1.x ) < epsilon
						&& 
					fabs( p0.y - p1.y ) < epsilon
						&&
					fabs( p0.z - p1.z ) < epsilon						
						&&
					(
						mgo0->GetObjectMesh() == mgo1->GetObjectMesh()
							&&
						mgo0->GetObjectMesh()
							)
						)
				{
					objectsForDeletion.insert( obj0 );
					break;
				}
			}

			if( obj0->ObjTypeFlags & OBJTYPE_Prefab 
					&&
				obj1->ObjTypeFlags & OBJTYPE_Prefab 
				)
			{
				obj_Prefab* pf0 = static_cast< obj_Prefab* > ( obj0 );
				obj_Prefab* pf1 = static_cast< obj_Prefab* > ( obj1 );

				if( 
					fabs( p0.x - p1.x ) < epsilon
						&& 
					fabs( p0.y - p1.y ) < epsilon
						&&
					fabs( p0.z - p1.z ) < epsilon						
						&&
					pf0->prefabName ==  pf1->prefabName )
				{
					objectsForDeletion.insert( obj0 );
					break;
				}
			}
		}
	}

	g_Manipulator3d.PickerResetPicked();

	for( ObjectSet::iterator	i = objectsForDeletion.begin(),
								e = objectsForDeletion.end();
								i != e;
								++ i )
	{
		GameWorld().DeleteObject( *i );
	}

	return (int)objectsForDeletion.size();
#endif
	return 0;
}

const char* COMMON_SETTINGS_FILE = "Data/CommonSettings.xml" ;
int LoadCommonSettings( )
{
	Bytes xmlFileBuffer;

	pugi::xml_document xmlLevelFile;
	pugi::xml_node xmlRoot ;

	r3dFile* f = r3d_open( COMMON_SETTINGS_FILE, "rb" );
	if ( f )
	{
		if( !ParseXMLInMemory( f, &xmlFileBuffer, &xmlLevelFile ) )
		{
			fclose( f );
			return 0 ;
		}

		xmlRoot = xmlLevelFile.root().child( "root" ) ;

		fclose( f );
	}

	SerializeCommonSettings<false>( xmlRoot ) ;

	void UpdateZPrepassSettings() ;
	UpdateZPrepassSettings() ;

#if 0
	RestoreCCLUT3DTexture();
#endif

	extern int gSLI_Crossfire_NumGPUs;
	r_need_reset_exposure->SetInt( gSLI_Crossfire_NumGPUs ) ;

	if( !r3dGameLevel::Environment->bStaticSkyEnable )
		r3dGameLevel::Environment->EnableStaticSky();

	return 1 ;
}

#ifndef FINAL_BUILD


#pragma warning(disable:4244 4800)

Editor_Level	LevelEditor;

void MouseMoveUpdate();

int g_bForceQualitySelectionInEditor = 0;
bool g_bStartedAsParticleEditor = false;

extern	r3dPoint3D	UI_TargetPos;		// snapped to object position (object center)
extern	r3dPoint3D	UI_TargetNorm;
extern  r3dPoint3D	UI_TerraTargetPos;
extern	gobjid_t	UI_TargetObjID;
extern	r3dMaterial	*UI_TargetMaterial;
extern char	LevelEditName[64];

#include "..\..\bin\Data\Shaders\DX9_P1\system\LibSM\shadow_config.h" // shader config file

#define LEVELEDITOR_SETTINGS_FILE	"%s/EditorSettings.xml"

//

extern	bool g_bSkyDomeNeedFullUpdate;

extern float  LakePlaneY;


// SSAO extern

extern int __RenderSSAOOnScreen;
extern int __SSAOBlurEnable;

// collections extern 
extern float BrushRadius;

// camera motion blur

int __HaveOcean = 0;
static int __HaveTerrain  = 0;


static float _LastTimeBrush = r3dGetTime();

float __WorldScale = 10;

static int TSize;
static float TScale;

static int CurrentPaintLayerIdx = 0;
static int TerrainPaintMode = 1;
static int g_nLayerTypeProjAxis = 1;
static int g_iTerrainNormalMapEditMode = 0;
static int g_iTerrainGlobalNormalMapEditMode = 0;

int g_iEditorSelectShadingGroup = -1;
int g_iEditorSelectCloudTemplate = -1;
bool g_bEditorShadingGroupModeSel = false;

// picker settings
int g_AngleSnappingEnable = 0;
int g_AngleSnappingIndex = 0;
int g_MoveSnappingEnable = 0;
int g_MoveSnappingIndex = 0;

int DrawTerrainMatHeaviness = 0;

int TerrainDrawBounds = 0;


void Unload_Collections();
void UpdateDestructionMeshSettings( const char* target );

extern int g_FwdColorVS;
extern int g_FwdColorPS;
extern int g_FwdDebugColorVS;
extern int g_FwdDebugColorPS;

GrassPlaneGameObjectProxy * g_GrassPlaneProxyObj = 0;

struct ErosionPattern;
extern ErosionPattern *gErosionPattern;

float FillBloomSettings( PFX_ExtractBloom::Settings *settings, int* blurPasses, BlurTaps* blurTaps, float SliderX, float SliderY, bool streaks );

namespace
{
	const float DEFAULT_CONTROLS_WIDTH = 360.0f;
	const float DEFAULT_CONTROLS_HEIGHT = 22.0f;
	const float DEFAULT_LIST_HEIGHT = 150.0f;
	const float CLOUD_SELECT_LIST_HEIGHT = 150.0f;
	const float LOCAL_CC_SELECT_LIST_HEIGHT = 150.0f;

	bool				InColorCorrection	= false;
	bool				ColorCorrectionTicked= false;

	const static char* ListObjectEditModes[] =	{ "OBJECTS",	"GROUPS",	 "PREFABS",		"DAMAGE LIB",		"TCAMO",		"UTILS",		"CAMERA SPOTS",			"OBJ DRAW DIST."		};
	enum										{ OBEM_OBJECTS,	OBEM_GROUPS, OBEM_PREFABS,	OBEM_DAMAGELIB,		OBEM_TCAMO,		OBEM_UTILS,		OBEM_CAMERA_SPOTS,		OBEM_DRAW_DITANCES		};


	void DrawColorTriangles( r3dPoint3D* position, uint16_t vertCount, uint16_t* indices, uint32_t primCount )
	{

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH );

		r3dRenderer->SetCullMode( D3DCULL_CCW );

		float data[ 4 ] = { 0.0f, 1.0f, 0.0f, 1.0f };
		D3D_V ( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, data, 1 ) );

		r3dRenderer->SetPixelShader( g_FwdColorPS );
		r3dRenderer->SetVertexShader( g_FwdColorVS );

		d3dc._SetDecl( R3D_POS_VERTEX::getDecl() );

		D3DXMATRIX ident;

		D3DXMatrixIdentity( &ident );

		r3dMeshSetShaderConsts( ident, NULL );

		r3dRenderer->DrawIndexedUP( 
						D3DPT_TRIANGLELIST, 
						0,
						vertCount,
						primCount,
						indices,
						D3DFMT_INDEX16,
						position,
						sizeof position[ 0 ] ) ;

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

		r3dRenderer->SetPixelShader();
		r3dRenderer->SetVertexShader();
	}
	void DrawColorTriangles2( R3D_DEBUG_VERTEX* position, uint16_t vertCount, uint16_t* indices, uint32_t primCount)
	{
		//r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH );
		r3dRenderer->SetRenderingMode(R3D_BLEND_ZC | R3D_BLEND_PUSH );

		r3dRenderer->SetCullMode( D3DCULL_CCW );

		r3dRenderer->SetPixelShader( g_FwdDebugColorPS );
		r3dRenderer->SetVertexShader( g_FwdDebugColorVS );

		d3dc._SetDecl( R3D_DEBUG_VERTEX::getDecl() );

		D3DXMATRIX ident;

		D3DXMatrixIdentity( &ident );

		r3dMeshSetShaderConsts( ident, NULL );

		r3dRenderer->DrawIndexedUP( 
						D3DPT_TRIANGLELIST, 
						0,
						vertCount,
						primCount,
						indices,
						D3DFMT_INDEX16,
						position,
						sizeof position[ 0 ] );

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

		r3dRenderer->SetPixelShader();
		r3dRenderer->SetVertexShader();
	}

	struct GUIHelperStackElem
	{
		enum { eWIDTH_LINE_DEFAULT = 16 };

		r3dPoint3D	origin;
		r3dPoint3D	size;
		r3dColor24	colr;
		float		width_line;

		GameObject*	obj;

		float		brushRadius;
		float		brushHardness;

		r3dPoint3D* positions;
		R3D_DEBUG_VERTEX *debugpos;
		uint16_t	positionCount;
		uint16_t*	indices;
		uint32_t	primitiveCount;

		enum
		{
			UNIFORM_GRID_CELLS_X = 64,
			UNIFORM_GRID_CELLS_Z = 64
		};

		enum Type
		{
			GRID,
			UNIFORM_GRID,
			UNIFORM_LINE3D,
			BOX_SOLID,
			BOX_WIREFRAME,
			SPHERE_SOLID,
			SPHERE_WIREFRAME,
			TERRAIN_BRUSH,
			GRASS_BRUSH,
			SELECTED_OBJECT,
			// use to test procedural geometry...
			TEST_MESH,
			EROSION_MESH, // for testing erosion code
			WATERPLANE_BRUSH,

		} type;

		GUIHelperStackElem() 
		: width_line( eWIDTH_LINE_DEFAULT )
		, obj( NULL )
		, positions( NULL )
		, debugpos(NULL)
		, positionCount( 0 )
		, indices( NULL )
		, primitiveCount( 0 )
		{
		}
	};

	r3dTL::TArray< GUIHelperStackElem > g_GUIHelperStack;

	//------------------------------------------------------------------------

	r3dBoundBox ExtendBoundingBox( const r3dBoundBox& bbox, float ammount )
	{
		r3dBoundBox extended ( bbox );

		extended.Org	-= bbox.Size * ammount * 0.5f;
		extended.Size	+= bbox.Size * ammount;

		return extended;
	}

	//------------------------------------------------------------------------

	r3dgfxSet( int ) DirtyCloudIDs;

	obj_LocalColorCorrection * CreateLocalColorCorrection( const r3dPoint3D& pos )
	{
		obj_LocalColorCorrection* pLocalCC = static_cast<obj_LocalColorCorrection*>( srv_CreateGameObject( "obj_LocalColorCorrection", "__local_cc", pos ) );
		return pLocalCC;
	}

	void DeleteLocalColorCorrection (obj_LocalColorCorrection * pCC )
	{
		r3d_assert ( pCC );
		GameWorld().DeleteObject ( pCC );
	}

	stringlist_t FillLocalColorCorrectionList()
	{
		char szName[128];

		stringlist_t localCCList;
		for ( unsigned int i = 0, e = obj_LocalColorCorrection::LocalColorCorrectionCount(); i < e; i ++  )
		{
			obj_LocalColorCorrection* pLocalCC = obj_LocalColorCorrection::LocalColorCorrectionGet( i );
			sprintf( szName, "Local Color Correction %i", i );
			localCCList.push_back ( szName );
		}

		return localCCList;
	}

	const float ADD_LOCAL_CC_DIST_TO_CAM = 200.0f;

	// color correction settings
	static int g_iLocalCCSelected = -1;
	static float g_iLocalCCListOffset = 0;

	stringlist_t FillEnvmapProbeList()
	{
		char szName[128];

		stringlist_t envmapList;
		for ( int i = 0, e = g_EnvmapProbes.GetCount(); i < e; i ++  )
		{
			obj_EnvmapProbe* probe = g_EnvmapProbes.GetProbe( i );
			sprintf( szName, "Probe %i", probe->GetID() );
			envmapList.push_back ( szName );
		}

		return envmapList;
	}

	int g_iEnvmapProbeSelected			= -1;
	float g_iEnvmapProbeListOffset		= 0.f;

	int g_GrassTypeSelected				= -1;
	float g_GrassTypeOffset				= 0.f;

	r3dTL::TArray<int> g_PaintGrassTypeIdxes;

	int g_GrassChunkSelected			= -1;
	float g_GrassChunkOffset			= 0.f;

	float g_GrassAvailableMeshOffset	= 0.f;

	int g_GrassTextureSelected			= -1;
	float g_GrassTextureOffset			= 0.f;
	float g_GrassTintTextureOffset		= 0.f;

//------------------------------------------------------------------------

	// game play settings
	stringlist_t g_dTypesList;

	int g_iDoubleClickCheckerCounter = 0;

	int g_iProcessLightProbesTicked = 0;

	int g_iPickerEnableTicked = 0;
	int g_iProcessObjectsTicked = 0;
	int g_iProcessGroupsTicked = 0;
	int g_iProcessWaterPlanesTicked = 0;
	int g_iProcessBattleZoneTicked = 0;
	int g_iGamePlayTicked = 0;
	int g_iRoadsTicked = 0;
	int g_iObjectSelectedType = 0;
	float g_fObjectSelectedTypeOffset = 0;
	int g_iSelectedObject = 0;
	float g_fSelectedObjectOffset = 0;
	r3dgameVector(GameObject*) g_dObjects;
	stringlist_t g_dObjectNames;

	int g_iSelectedDecalType = -1;
	int g_iDecalListSelectedItem = -1;
	int g_iSelectedDecalOffset = 0;

	void FillObjectListByType ( const char * sSelectedType )
	{
		g_dObjects.clear();
		g_dObjectNames.clear();

		for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
		{
			GameObject* obj = iter.current;

			if( obj->Class->Name == sSelectedType )
			{
				char objName [256];
				sprintf ( objName, "[%i] %s", g_dObjectNames.size (), obj->Name.c_str() );
				g_dObjects.push_back( obj );
				g_dObjectNames.push_back( objName );
			}
		}
	}

	//------------------------------------------------------------------------

	obj_WaterPlane::Settings g_WaterPlaneSettings;

	float g_NominalWaterPlaneWidth;
	float g_NominalWaterPlaneHeight;

	r3dTL::T2DArray<r3dVector> g_dWaterPlaneGrid;

	int g_iWaterEditMode = 0;
	int g_iWaterPropertiesShown = 0;

	obj_WaterPlane* CreateWaterPlane()
	{
		//float alt_len = ( UI_TargetPos - gCam ).Length();
		//r3dPoint3D pos = gCam + gCam.vPointTo * R3D_MIN( alt_len, 233.f );
		char name[256];
		sprintf(name, "water_plane_%d", timeGetTime());

		r3dPoint3D pos = r3dVector(0,0,0);
		return static_cast<obj_WaterPlane*>( srv_CreateGameObject( "obj_WaterPlane", name, pos ) );
	}

	void DeleteWaterPlane(obj_WaterPlane* plane )
	{
		r3d_assert ( plane );
		uint32_t tag = reinterpret_cast<uint32_t>(plane);
		g_pUndoHistory->RemoveUndoActionsByTag(tag);
		GameWorld().DeleteObject ( plane );
	}

	int GetWaterPlaneIndex(obj_WaterPlane* plane )
	{
		for ( uint32_t i = 0; i < obj_WaterPlane::m_dWaterPlanes.Count(); i++ )
		{
			if ( obj_WaterPlane::m_dWaterPlanes[i] == plane )
				return i;
		}
		r3d_assert ( false );
		return -1;
	}

	void UpdateWaterPlaneGrids( obj_WaterPlane::Settings* sts )
	{
		float cellSize = sts->CellGridSize;

		if( Terrain && Terrain->IsLoaded() )
		{
			const r3dTerrainDesc& desc = Terrain->GetDesc() ;

			float expandedSizeX = sts->PlaneXSize;
			float expandedSizeZ = sts->PlaneZSize;

			if( expandedSizeX < desc.XSize )
				expandedSizeX = desc.XSize;

			if( expandedSizeZ < desc.ZSize )
				expandedSizeZ = desc.ZSize;

			sts->PlaneXSize = expandedSizeX;
			sts->PlaneZSize = expandedSizeZ;

			g_NominalWaterPlaneWidth	= desc.XSize;
			g_NominalWaterPlaneHeight	= desc.ZSize;

			sts->PlaneCentreX = desc.XSize * 0.5f;
			sts->PlaneCentreZ = desc.ZSize * 0.5f;

			float offsetX = ( desc.XSize - expandedSizeX ) * 0.5f;
			float offsetZ = ( desc.ZSize - expandedSizeZ ) * 0.5f;

			int iCellCountW = int( expandedSizeX / cellSize ) + 1;
			int iCellCountH = int( expandedSizeZ / cellSize ) + 1;

			g_dWaterPlaneGrid.Resize(iCellCountW, iCellCountH);
			for ( int i = 0; i < iCellCountW; i++ )
			{
				for ( int j = 0; j < iCellCountH; j++ )
				{
					float x = i*cellSize + offsetX;
					float z = j*cellSize + offsetZ;

					float fH = terra_GetH(r3dVector (x, 0.0f, z));
					g_dWaterPlaneGrid[j][i] = r3dVector (x, fH, z);
				}
			}
		}
		else // use minimap size instead of terrain
		{
			int iCellCountW = int(GameWorld().m_MinimapSize.x / cellSize) + 1;
			int iCellCountH = int(GameWorld().m_MinimapSize.z / cellSize) + 1;

			g_NominalWaterPlaneWidth	= GameWorld().m_MinimapSize.x;
			g_NominalWaterPlaneHeight	= GameWorld().m_MinimapSize.z;

			sts->PlaneCentreX = GameWorld().m_MinimapSize.x * 0.5f;
			sts->PlaneCentreZ = GameWorld().m_MinimapSize.z * 0.5f;

			g_dWaterPlaneGrid.Resize(iCellCountW, iCellCountH);
			for ( int i = 0; i < iCellCountW; i++ )
			{
				for ( int j = 0; j < iCellCountH; j++ )
				{
					g_dWaterPlaneGrid[j][i] = r3dVector (GameWorld().m_MinimapOrigin.x + i*cellSize, 100.0f, GameWorld().m_MinimapOrigin.z + j*cellSize);
				}
			}
		}
	}

	void GetCCRefTexParams( const int CC_TEX_SOURCE_DIM, int iwidth, int& oNumCellsPerLine, int& oExtraHeight )
	{
		oNumCellsPerLine = iwidth / CC_TEX_SOURCE_DIM ;
		oExtraHeight = CC_TEX_SOURCE_DIM * CC_TEX_SOURCE_DIM / oNumCellsPerLine + 1 ;
	}


//------------------------------------------------------------------------

	const char* Terrain_Edit_List[] =	{ "OPTIONS",	"DOWN",		"UP",	"LEVEL",	"SMOOTH",	"NOISE",	"RAMP",		"EROSION",	"PAINT",	"COLOR",	"HEIGHTMAP",	"NEW_FUNC"		};
	enum EditMode_t						{ TE_SETTINGS,	TE_DOWN,	TE_UP,	TE_LEVEL,	TE_SMOOTH,	TE_NOISE,	TE_RAMP,	TE_EROSION,	TE_PAINT,	TE_COLOR,	TE_HMAP,		TE_NEW_FUNC,	TE_NUM,	};

	float	TerrainBrushRadiusVal[TE_NUM];
	float	TerrainBrushHardnessVal[TE_NUM];
	float	TerrainBrushStrengthVal[TE_NUM];
	float	TerrainBrushHeightVal[TE_NUM];
	float	TerrainBrushHeightAdjustVal[TE_NUM];
	float	TerrainBrushNoiseVal[TE_NUM];
	int		TerrainBrushNoiseRaiseVal[TE_NUM];

	const char * EditModeToString( EditMode_t eMode )
	{
		switch ( eMode )
		{
		case TE_SETTINGS:	return "TEDIT_SETTINGS";
		case TE_DOWN:		return "TEDIT_DOWN";
		case TE_UP:			return "TEDIT_UP";
		case TE_LEVEL:		return "TEDIT_LEVEL";
		case TE_NEW_FUNC:	return "TEDIT_NEW_FUNC";
		case TE_SMOOTH:		return "TEDIT_SMOOTH";
		case TE_NOISE:		return "TEDIT_NOISE";
		case TE_RAMP:		return "TEDIT_RAMP";
		case TE_EROSION:	return "TEDIT_EROSION";
		case TE_PAINT:		return "TEDIT_PAINT";
		case TE_COLOR:		return "TEDIT_COLOR";
		case TE_HMAP:		return "TEDIT_HMAP";
		}
		r3d_assert( false );
		return "unknown";
	};

	const char* Terrain2_Edit_List[] =	{ "OPTIONS",	"DOWN",		"UP",	"LEVEL",	"SMOOTH",	"NOISE",	"RAMP",		"EROSION",		"PAINT",		"COLOR",		"HEIGHTMAP",	};
	enum EditMode2_t					{ TE2_SETTINGS,	TE2_DOWN,	TE2_UP,	TE2_LEVEL,	TE2_SMOOTH,	TE2_NOISE,	TE2_RAMP,	TE2_EROSION,	TE2_PAINT,		TE2_COLOR,		TE2_HMAP,		TE2_NUM	};

	float	Terrain2BrushRadiusVal[TE2_NUM];
	float	Terrain2BrushHardnessVal[TE2_NUM];
	float	Terrain2BrushStrengthVal[TE2_NUM];
	float	Terrain2BrushHeightVal[TE2_NUM];
	float	Terrain2BrushHeightAdjustVal[TE2_NUM];
	float	Terrain2BrushNoiseVal[TE2_NUM];
	int		Terrain2BrushNoiseRaiseVal[TE2_NUM];

	const float TERRAIN2_BRUSH_DELTA = 0.025f;

	const char * EditModeToString2( EditMode2_t eMode )
	{
		switch ( eMode )
		{
		case TE2_SETTINGS:	return "TERRA2_EDIT_SETTINGS" ;
		case TE2_DOWN:		return "TERRA2_EDIT_DOWN" ;
		case TE2_UP:		return "TERRA2_EDIT_UP" ;
		case TE2_LEVEL:		return "TERRA2_EDIT_LEVEL" ;
		case TE2_SMOOTH:	return "TERRA2_EDIT_SMOOTH" ;
		case TE2_NOISE:		return "TERRA2_EDIT_NOISE" ;
		case TE2_RAMP:		return "TERRA2_EDIT_RAMP" ;
		case TE2_EROSION:	return "TERRA2_EDIT_EROSION" ;
		case TE2_PAINT:		return "TERRA2_EDIT_PAINT" ;
		case TE2_COLOR:		return "TERRA2_EDIT_COLOR" ;
		case TE2_HMAP:		return "TERRA2_EDIT_HMAP" ;
		}
		r3d_assert( false );
		return "unknown";
	};

	//------------------------------------------------------------------------

	const char* Terrain3_Edit_List[] =	{ "OPTIONS",	"DOWN",		"UP",	"LEVEL",	"SMOOTH",	"NOISE",	"RAMP",		"EROSION",		"PAINT",		"FAR LAYER",		"COLOR",		"HEIGHTMAP",	"MASKS",	"DEBUG"				};
	enum EditMode3_t					{ TE3_SETTINGS,	TE3_DOWN,	TE3_UP,	TE3_LEVEL,	TE3_SMOOTH,	TE3_NOISE,	TE3_RAMP,	TE3_EROSION,	TE3_PAINT,		TE3_GLOBAL_LAYER,	TE3_COLOR,		TE3_HMAP,		TE3_MASKS,	TE3_DEBUG,	TE3_NUM	};

	float	Terrain3BrushRadiusVal[TE3_NUM];
	float	Terrain3BrushHardnessVal[TE3_NUM];
	float	Terrain3BrushStrengthVal[TE3_NUM];

	float	Terrain3BrushHeightVal[TE3_NUM];
	float	Terrain3BrushHeightAdjustVal[TE3_NUM];

	struct InitBrushes3
	{
		InitBrushes3()
		{
			for( int i = 0, e = TE3_NUM; i < e; i ++ )
			{
				Terrain3BrushRadiusVal[ i ] = 2.0f;
				Terrain3BrushHardnessVal[ i ] = 0.5f;
				Terrain3BrushStrengthVal[ i ] = 0.5f;

				Terrain3BrushHeightVal[ i ] = 100.0f;
				Terrain3BrushHeightAdjustVal[ i ] = 1.f;
			}
		}
	} static g_initBrushes3;

	const float TERRAIN3_BRUSH_DELTA = 0.025f;

	const char * EditModeToString3( EditMode3_t eMode )
	{
		switch ( eMode )
		{
		case TE3_SETTINGS:		return "TERRA3_EDIT_SETTINGS" ;
		case TE3_DOWN:			return "TERRA3_EDIT_DOWN" ;
		case TE3_UP:			return "TERRA3_EDIT_UP" ;
		case TE3_LEVEL:			return "TERRA3_EDIT_LEVEL" ;
		case TE3_SMOOTH:		return "TERRA3_EDIT_SMOOTH" ;
		case TE3_NOISE:			return "TERRA3_EDIT_NOISE" ;
		case TE3_RAMP:			return "TERRA3_EDIT_RAMP" ;
		case TE3_EROSION:		return "TERRA3_EDIT_EROSION" ;
		case TE3_PAINT:			return "TERRA3_EDIT_PAINT" ;
		case TE3_GLOBAL_LAYER:	return "TERRA3_GLOBAL_LAYER" ;
		case TE3_COLOR:			return "TERRA3_EDIT_COLOR" ;
		case TE3_HMAP:			return "TERRA3_EDIT_HMAP" ;
		case TE3_MASKS:			return "TERRA3_EDIT_HMASKS";
		case TE3_DEBUG:			return "TERRA3_EDIT_DEBUG";
		}
		r3d_assert( false );
		return "unknown";
	};

	EditMode_t StringToEditMode( const char * szMode )
	{
		if ( ! strcmp( szMode, "TEDIT_SETTINGS" ) )		return TE_SETTINGS;
		else if ( ! strcmp( szMode, "TEDIT_DOWN" ) )	return TE_DOWN;
		else if ( ! strcmp( szMode, "TEDIT_UP" ) )		return TE_UP;
		else if ( ! strcmp( szMode, "TEDIT_LEVEL" ) )	return TE_LEVEL;
		else if ( ! strcmp( szMode, "TEDIT_NEW_FUNC" ) )return TE_NEW_FUNC;
		else if ( ! strcmp( szMode, "TEDIT_SMOOTH" ) )	return TE_SMOOTH;
		else if ( ! strcmp( szMode, "TEDIT_NOISE" ) )	return TE_NOISE;
		else if ( ! strcmp( szMode, "TEDIT_RAMP" ) )	return TE_RAMP;
		else if ( ! strcmp( szMode, "TEDIT_EROSION" ) ) return TE_EROSION;
		else if ( ! strcmp( szMode, "TEDIT_PAINT" ) )	return TE_PAINT;
		else if ( ! strcmp( szMode, "TEDIT_COLOR" ) )	return TE_COLOR;
		else if ( ! strcmp( szMode, "TEDIT_HMAP" ) )	return TE_HMAP;

		r3d_assert( false );
		return TE_SETTINGS;
	};

//////////////////////////////////////////////////////////////////////////

	void ModifyShadowsFlagForTransparentObject(GameObject *obj)
	{
		bool haveShadows = (obj->ObjFlags & OBJFLAG_DisableShadows) == 0;
		if (obj && obj->ObjTypeFlags & OBJTYPE_Mesh && haveShadows)
		{
			for (int iMesh = 0; iMesh < 4; ++iMesh)
			{
				r3dMesh* mesh = ((MeshGameObject *)obj)->MeshLOD[iMesh];
				if (mesh)
				{
					for (int idx = 0; idx < mesh->NumMatChunks; idx++)
					{
						r3dMaterial* mat = mesh->MatChunks[idx].Mat;
						if (mat)
						{
							if (mat->Flags & R3D_MAT_TRANSPARENT)
								obj->ObjFlags |= OBJFLAG_DisableShadows;
							else
								obj->ObjFlags &= ~OBJFLAG_DisableShadows;
						}
					}
				}
			}
		}
	}

//////////////////////////////////////////////////////////////////////////
	
	void ModifyShadowsFlagForTransparentObjects()
	{
		for (GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
		{
			ModifyShadowsFlagForTransparentObject(obj);
		}

		for( int i = 0, e = GameWorld().GetStaticObjectCount(); i < e; i ++ )
		{
			ModifyShadowsFlagForTransparentObject( GameWorld().GetStaticObject( i ) );
		}
	}

//////////////////////////////////////////////////////////////////////////

	void FillFileListFromDir(const char *path, stringlist_t &toFill)
	{
		WIN32_FIND_DATA ffblk;
		HANDLE h = FindFirstFile(path, &ffblk);
		if(h != INVALID_HANDLE_VALUE) do 
		{
			toFill.push_back(ffblk.cFileName);
		} while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}
} // unnamed namespace

//--------------------------------------------------------------------------------------------------------
void FocusOnObject()
{
	GameObject* obj = g_Manipulator3d.PickedObject();
	if( ! obj )
		return;

	r3dVector vH = obj->GetBBoxWorld().Size / 2.f;
	r3dVector vDest = obj->GetBBoxWorld().Org + vH;

	Hud->FPS_Position = vDest - gCam.vPointTo * vH.Length();
}

struct SetupOnDblClick
{
	SetupOnDblClick()
	{
		OnDblClick = FocusOnObject;
	}
} static gSetupOnDblClick ;

void ResetObjectRotation()
{
	GameObject* obj = g_Manipulator3d.PickedObject();
	if( !obj )
		return;

	obj->SetRotationVector(r3dVector(0,0,0));
}

//------------------------------------------------------------------------

template < int N >
float DrawMaterialTypeSelection( float SliderX, float SliderY, char (&TypeName)[ N ] );

//------------------------------------------------------------------------

static void SetIdentityTransform()
{
#ifndef FINAL_BUILD
	D3DXMATRIX ident ;
	D3DXMatrixIdentity( &ident );
	D3D_V( r3dRenderer->pd3ddev->SetTransform( D3DTS_WORLD, &ident ) );
#endif
}

void DrawPreGUIHelpers()
{

	struct RModePushPop
	{
		explicit RModePushPop( int Mode )
		{
			r3dRenderer->SetRenderingMode( Mode | R3D_BLEND_PUSH );
		}

		~RModePushPop()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}

	} rmode_push_pop( R3D_BLEND_ALPHA | R3D_BLEND_ZC ); (void)rmode_push_pop;

	d3dc._SetDecl( R3D_DEBUG_VERTEX::getDecl() );
	SetIdentityTransform();

	for( uint32_t i = 0, e = g_GUIHelperStack.Count(); i < e; i ++ )
	{
		const GUIHelperStackElem& elem = g_GUIHelperStack[ i ];
		r3dBox3D box;

		switch( elem.type )
		{
		case GUIHelperStackElem::GRID:
			r3dDrawGridPlane( elem.origin, gCam, elem.size.x, elem.size.z, 64, 64, elem.colr, elem.width_line );
			break;

		case GUIHelperStackElem::UNIFORM_GRID:
			r3dDrawUniformGridPlane( elem.origin, gCam, elem.size.x, elem.size.z, GUIHelperStackElem::UNIFORM_GRID_CELLS_X, GUIHelperStackElem::UNIFORM_GRID_CELLS_Z, elem.colr );
			break;

		case GUIHelperStackElem::UNIFORM_LINE3D:
			r3dDrawUniformLine3D(elem.origin, elem.size, gCam, elem.colr );
			break;

		case GUIHelperStackElem::BOX_SOLID:
			box.Org = elem.origin;
			box.Size = elem.size;
			r3d_assert( false && "Implement me!" );
			break;

		case GUIHelperStackElem::BOX_WIREFRAME:
			box.Org = elem.origin;
			box.Size = elem.size;
			r3dDrawUniformBoundBox ( box, gCam, elem.colr );
			break;

		case GUIHelperStackElem::SPHERE_WIREFRAME:
			r3dDrawUniformSphere ( elem.origin, elem.size.x, gCam, elem.colr );
			break;

		case GUIHelperStackElem::SPHERE_SOLID:
			r3d_assert( false && "Implement me!" );
			break;

		case GUIHelperStackElem::TERRAIN_BRUSH:
			void DrawTerrainBrush( float radius, float hardness, r3dPoint3D pos );
			DrawTerrainBrush( elem.brushRadius, elem.brushHardness, elem.origin );
			break;

		case GUIHelperStackElem::GRASS_BRUSH:
			void DrawGrassBrush( float radius, float hardness, r3dPoint3D pos );
			DrawGrassBrush( elem.brushRadius, elem.brushHardness, elem.origin );
			break;

		case GUIHelperStackElem::WATERPLANE_BRUSH:
			void DrawWaterBrush(const r3dPoint3D& vBrushPos, float radius);
			DrawWaterBrush(elem.origin, elem.brushRadius);
			break;

		case GUIHelperStackElem::SELECTED_OBJECT:
			r3d_assert( elem.obj );

			elem.obj->DrawSelected( gCam, rsDrawComposite1 );

			d3dc._SetDecl( R3D_DEBUG_VERTEX::getDecl() );
			SetIdentityTransform();
			break;

		case GUIHelperStackElem::TEST_MESH:
			DrawColorTriangles( elem.positions, elem.positionCount, elem.indices, elem.primitiveCount );
			break;

		case GUIHelperStackElem::EROSION_MESH:
			DrawColorTriangles2( elem.debugpos, elem.positionCount, elem.indices, elem.primitiveCount);

			break;

		default:
			r3d_assert( false && "Unknown GUI helper type" );
		};
	}

	r3dRenderer->Flush();

	g_GUIHelperStack.Clear();
}


//--------------------------------------------------------------------------------------------------------
void PushUniformGrid( const r3dPoint3D &vPos, float fSizeX, float fSizeZ, const r3dColor24 &color = r3dColor::white )
{
	GUIHelperStackElem el;

	el.origin		= vPos;
	el.size			= r3dPoint3D( fSizeX, 0, fSizeZ );
	el.colr			= color;
	el.type			= GUIHelperStackElem::UNIFORM_GRID;
	el.width_line	= 1;

	g_GUIHelperStack.PushBack( el );
}

void PushUniformLine3D( const r3dPoint3D &vFrom, const r3dPoint3D &vTo, const r3dColor24 &color = r3dColor::white )
{
	GUIHelperStackElem el;

	el.origin		= vFrom;
	el.size			= vTo;
	el.colr			= color;
	el.type			= GUIHelperStackElem::UNIFORM_LINE3D;
	el.width_line	= 1;

	g_GUIHelperStack.PushBack( el );
}

//------------------------------------------------------------------------

void PushSelectedObject( GameObject* obj )
{
	GUIHelperStackElem el;
	el.obj	= obj;
	el.type	= GUIHelperStackElem::SELECTED_OBJECT;

	g_GUIHelperStack.PushBack( el );
}

//------------------------------------------------------------------------

void RemoveObjectFromGUIHelperStack(GameObject *obj)
{
	for (uint32_t i = 0; i < g_GUIHelperStack.Count(); ++i)
	{
		if (g_GUIHelperStack[i].obj == obj)
		{
			g_GUIHelperStack.Erase(i);
			return;
		}
	}
}

//------------------------------------------------------------------------

void PushUniformLimitGrid( float height, float scale, bool top, bool cameraCentered )
{
	float fRandW = 2000.0f;
	float fRandH = 2000.0f;
	r3dPoint3D vOrign = gCam;
	vOrign.y = height;

	if ( Terrain )
	{
		const r3dTerrainDesc& desc = Terrain->GetDesc() ;

		fRandW = desc.XSize * 1.1f;
		fRandH = desc.ZSize * 1.1f;

		if( !cameraCentered )
		{
			vOrign.x = fRandW * 0.5f;
			vOrign.z = fRandH * 0.5f;
		}
	}

	GUIHelperStackElem el;

	el.origin		= vOrign;
	el.size			= r3dPoint3D( fRandW, 0, fRandH ) * scale;
	el.colr			= top ? r3dColor( 0, 255, 0, 128 ) : r3dColor( 0, 128, 0, 192 );
	el.type			= GUIHelperStackElem::UNIFORM_GRID;

	g_GUIHelperStack.PushBack( el );
}

//--------------------------------------------------------------------------------------------------------
void PushBoundingBox( const r3dBoundBox &Box, const r3dColor24 &color = r3dColor::white, bool wireframe = true, float width_line = GUIHelperStackElem::eWIDTH_LINE_DEFAULT )
{
	GUIHelperStackElem el;

	el.origin		= Box.Org;
	el.size			= Box.Size;
	el.colr			= color;
	el.type			= wireframe ? GUIHelperStackElem::BOX_WIREFRAME : GUIHelperStackElem::BOX_SOLID;
	el.width_line	= width_line;

	g_GUIHelperStack.PushBack( el );
}
//------------------------------------------------------------------------
void PushBoundingSphere( const r3dVector &vPos, float fRadius, const r3dColor24 &color = r3dColor::white, bool wireframe = true, float width_line = GUIHelperStackElem::eWIDTH_LINE_DEFAULT )
{
	GUIHelperStackElem el;

	el.origin		= vPos;
	el.size			= r3dVector( fRadius, fRadius, fRadius );
	el.colr			= color;
	el.type			= wireframe ? GUIHelperStackElem::SPHERE_WIREFRAME : GUIHelperStackElem::SPHERE_SOLID;
	el.width_line	= width_line;

	g_GUIHelperStack.PushBack( el );
}

//------------------------------------------------------------------------

void PushTerrainBrush( float Radius, float Hardness, const r3dPoint3D& origin = UI_TerraTargetPos )
{
	GUIHelperStackElem el;
	el.type				= GUIHelperStackElem::TERRAIN_BRUSH;
	el.brushRadius		= Radius;
	el.brushHardness	= Hardness;
	el.origin			= origin;

	g_GUIHelperStack.PushBack( el );
}

void PushGrassBrush( float Radius, float Hardness, const r3dPoint3D& origin = UI_TerraTargetPos )
{
	GUIHelperStackElem el;
	el.type				= GUIHelperStackElem::GRASS_BRUSH;
	el.brushRadius		= Radius;
	el.brushHardness	= Hardness;
	el.origin			= origin;

	g_GUIHelperStack.PushBack( el );
}

void PushWaterBrush( const r3dPoint3D& origin, float radius )
{
	GUIHelperStackElem el;
	el.type				= GUIHelperStackElem::WATERPLANE_BRUSH;
	el.brushRadius		= radius;
	el.origin			= origin;

	g_GUIHelperStack.PushBack( el );
}

//------------------------------------------------------------------------

void PushTestMesh( r3dPoint3D* positions, uint16_t positionCount, uint16_t* indices, uint16_t primitiveCount )
{
	GUIHelperStackElem el;
	el.type				= GUIHelperStackElem::TEST_MESH;
	el.positions		= positions;
	el.positionCount	= positionCount;
	el.indices			= indices;
	el.primitiveCount	= primitiveCount;

	g_GUIHelperStack.PushBack( el );
}
void PushErosionMesh( R3D_DEBUG_VERTEX* positions, uint16_t positionCount, uint16_t* indices, uint16_t primitiveCount )
{
	GUIHelperStackElem el;
	el.type				= GUIHelperStackElem::EROSION_MESH;
	el.debugpos		= positions;
	el.positionCount	= positionCount;
	el.indices			= indices;
	el.primitiveCount	= primitiveCount;

	g_GUIHelperStack.PushBack( el );
}

void PushCross3D( const r3dPoint3D& pos, float len )
{
	PushUniformLine3D( 	r3dPoint3D( pos.x - len, pos.y, pos.z ),
						r3dPoint3D( pos.x + len, pos.y, pos.z ),
						r3dColor::red ) ;

	PushUniformLine3D( 	r3dPoint3D( pos.x, pos.y - len, pos.z ),
						r3dPoint3D( pos.x, pos.y + len, pos.z ),
						r3dColor::green ) ;

	PushUniformLine3D( 	r3dPoint3D( pos.x, pos.y, pos.z - len ),
						r3dPoint3D( pos.x, pos.y, pos.z + len ),
						r3dColor::blue ) ;
}

namespace
{
	typedef r3dgfxMap( r3dSTLString, int ) RegisteredMeshes_t;
	RegisteredMeshes_t g_dRegisteredMeshes;

	void MeshLoaded ( const char* sMesh )
	{
		if(sMesh[0]!='\0')
		{
			RegisteredMeshes_t::iterator pIt = g_dRegisteredMeshes.find ( sMesh );
			if ( pIt != g_dRegisteredMeshes.end () )
				pIt->second++;
			else
				g_dRegisteredMeshes.insert(std::pair<r3dSTLString, int> (sMesh, 1) );
		}
	}

	void MeshUnloaded ( const char* sMesh )
	{
		if(sMesh[0]!='\0')
		{
			RegisteredMeshes_t::iterator pIt = g_dRegisteredMeshes.find ( sMesh );
			if ( pIt != g_dRegisteredMeshes.end () )
			{
				pIt->second--;
				if ( pIt->second <= 0 )
					g_dRegisteredMeshes.erase ( pIt );
			}
			//else
			//{
			//	r3d_assert ( false );
			//}
		}
	}
}

extern	void		r3dDrawLine3DT(const r3dPoint3D& p1, const r3dPoint3D& p2, r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex=NULL);

typedef float (*GetHeightFunc)(const r3dPoint3D &pos);

void r3dDrawCircle3D_Height(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex, GetHeightFunc fn)
{
	// add vertices
	float    g_dDegToRad     =   3.1415926f / 180.0f;

	r3dPoint3D VV, VV1;

	int Segments = 32;
	float terrain_height_offset = 0.5f;
	for( int x = 0; x<Segments; x++ )
	{
		float theta = float( x ) / float(Segments) * 360.0f * g_dDegToRad;
		float theta1 = float( x+1 ) / float(Segments) * 360.0f * g_dDegToRad;

		VV.Assign(	P1.X+static_cast< float >( cosf( theta ) * Radius ),
			0,
			P1.Z+static_cast< float >( sinf( theta ) * Radius ) );
		VV.Y = fn(VV) + terrain_height_offset;

		VV1.Assign(	P1.X+static_cast< float >( cosf( theta1 ) * Radius ),
			0,
			P1.Z+static_cast< float >(sinf( theta1 ) * Radius ) );
		VV1.Y = fn(VV1) + terrain_height_offset;

		r3dDrawLine3D(VV,VV1, Cam, Width, clr, Tex);
	}
}

void r3dDrawCircle3DT_GrassPlanes(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex = NULL)
{
	r3dDrawCircle3D_Height(P1, Radius, Cam, Width, clr, Tex, &GetGrassHeight);
}

void r3dDrawCircle3DT(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex = NULL)
{
	r3dDrawCircle3D_Height(P1, Radius, Cam, Width, clr, Tex, &terra_GetH);
}

void r3dDrawCircle3D_Height(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex=NULL)
{
	// add vertices
	float    g_dDegToRad     =   3.1415926f / 180.0f;

	r3dPoint3D VV, VV1;

	int Segments = 32;
	float terrain_height_offset = 0.5f;
	for( int x = 0; x<Segments; x++ )
	{
		float theta = float( x ) / float(Segments) * 360.0f * g_dDegToRad;
		float theta1 = float( x+1 ) / float(Segments) * 360.0f * g_dDegToRad;

		VV.Assign(	P1.X+static_cast< float >( cosf( theta ) * Radius ),
			0,
			P1.Z+static_cast< float >( sinf( theta ) * Radius ) );
		VV.Y = terra_GetH(VV) + terrain_height_offset;

		VV1.Assign(	P1.X+static_cast< float >( cosf( theta1 ) * Radius ),
			0,
			P1.Z+static_cast< float >(sinf( theta1 ) * Radius ) );
		VV1.Y = terra_GetH(VV1) + terrain_height_offset;

		r3dDrawLine3D(VV,VV1, Cam, Width, clr, Tex);
	}
}

void r3dDrawUniformCircle3DT(const r3dPoint3D& P1, const float Radius, r3dCamera &Cam, float terrainOffset, const r3dColor24& clr)
{
	// add vertices
	float    g_dDegToRad     =   3.1415926f / 180.0f;

	r3dPoint3D VV, VV1;

	int Segments = 32;
	float terrain_height_offset = 1.0f;
	for( int x = 0; x<Segments; x++ )
	{
		float theta = float( x ) / float(Segments) * 360.0f * g_dDegToRad;
		float theta1 = float( x+1 ) / float(Segments) * 360.0f * g_dDegToRad;

		VV.Assign(	P1.X+static_cast< float >( cosf( theta ) * Radius ),
			P1.Y + terrainOffset,
			P1.Z+static_cast< float >( sinf( theta ) * Radius ) );
		if(Terrain)
			VV.Y = terra_GetH(VV) + terrainOffset;

		VV1.Assign(	P1.X+static_cast< float >( cosf( theta1 ) * Radius ),
			P1.Y + terrainOffset,
			P1.Z+static_cast< float >(sinf( theta1 ) * Radius ) );
		if(Terrain)
			VV1.Y = terra_GetH(VV1) + terrainOffset;

		r3dDrawUniformLine3D(VV,VV1, Cam, clr);
	}
}

float frac ( float fVal )
{
	float fres = ( fVal - floor ( fVal ) );
	//if ( fVal < 0.0f )
	//	fres = 1.0f - fres;
	return fres;

}

bool IsPower2 ( int iDim )
{
	// binary power of 2 is (100....000)
	int iPow = 2;
	while ( iPow < iDim )
		iPow *= 2;

	return iPow == iDim;
}

void VFS_CreateDirectoryRecursive( const char * szName, bool bAddSlash = true )
{
	char data[ MAX_PATH ];

	size_t szNameLen = strlen ( szName );
	if ( bAddSlash && szName[szNameLen - 1] != '\\' && szName[szNameLen - 1] != '/' )
		sprintf ( data, "%s/", szName );
	else
		r3dscpy( data, szName );

	char creation_path[ MAX_PATH ];

	char * pdata;
	char * pdata1;

	char szStoreDir[ MAX_PATH ];

	szStoreDir[ 0 ] = '\0';

	do
	{
		pdata = strchr( data, '/' );
		pdata1 = strchr( data, '\\' );
		if ( !pdata || ( pdata > pdata1 && pdata1 != NULL ) )
		{
			pdata = pdata1;
		}

		if ( ! pdata )
			return;

		size_t ipos = strlen( data ) - strlen( pdata );
		memcpy( creation_path, data, ipos );
		creation_path[ ipos ] = '\0';
		strcat( szStoreDir, creation_path );
		strcat( szStoreDir, "/" );
		CreateDirectory( szStoreDir, NULL );
		r3dscpy( data, data + ipos + 1 );
	}
	while ( pdata );
}

void CopyRecursive ( const char * szFrom, const char * szTo, const char **dIgnoreFiles )
{
	// copy recursive
	char szFindStr [MAX_PATH];
	sprintf ( szFindStr, "%s/*.*", szFrom );

	WIN32_FIND_DATA	 win32_data;
	HANDLE pFile = FindFirstFile ( szFindStr, &win32_data );
	if ( pFile != INVALID_HANDLE_VALUE )
	{
		do 
		{
			// special case for "." as it doesn't work with strstr
			if(win32_data.cFileName[0] == '.') // should be safe, hopefully no one will name their resource with the dot in the beggining of the filename :)
				continue;

			if ( dIgnoreFiles )
			{
				bool bSkip = false;
				int i = 0;
				while ( dIgnoreFiles[i] )
				{
					if ( strstr( win32_data.cFileName, dIgnoreFiles[i]) )
					{
						bSkip = true;
						break;
					}
					i++;
				}

				if ( bSkip )
					continue;
			}

			if ( win32_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				char szFromNew [MAX_PATH];
				char szToNew [MAX_PATH];
				sprintf ( szFromNew, "%s/%s", szFrom, win32_data.cFileName );
				sprintf ( szToNew, "%s/%s", szTo, win32_data.cFileName );
				
				CreateDirectory ( szToNew, NULL );
				CopyRecursive ( szFromNew, szToNew, dIgnoreFiles );
			}
			else
			{
				char szFileFrom [MAX_PATH];
				char szFileTo [MAX_PATH];
				sprintf ( szFileFrom, "%s/%s", szFrom, win32_data.cFileName );
				sprintf ( szFileTo, "%s/%s", szTo, win32_data.cFileName );

				CopyFile ( szFileFrom, szFileTo, FALSE );
			}

		} 
		while ( FindNextFile ( pFile, &win32_data ) );

		FindClose ( pFile );
	}
}

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

bool g_bIsMinimapRendering = false;
void RenderLevelMinimap ( const char* TargetFile )
{
	struct EnableDisableDistanceCull
	{
		EnableDisableDistanceCull()
		{
			oldValue = r_allow_distance_cull->GetInt();
			r_allow_distance_cull->SetInt( 0 );
		}

		~EnableDisableDistanceCull()
		{
			r_allow_distance_cull->SetInt( oldValue );
		}

		int oldValue;

	} enableDisableDistanceCull; (void)enableDisableDistanceCull;

	g_bIsMinimapRendering = true;
	extern int g_bTerrainUseLightHack;

	g_bTerrainUseLightHack = 1;

	int texWidth	= 2048;
	int texHeight	= 2048;

	R3D_ENSURE_MAIN_THREAD();

	IDirect3DTexture9* tex( NULL );
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( texWidth, texHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &tex, NULL ) );

	IDirect3DSurface9* rt( NULL );
	D3D_V( r3dRenderer->pd3ddev->CreateRenderTarget( texWidth, texHeight, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &rt, NULL ) );
	SetD3DResourcePrivateData(rt, "RenderLevelMinimap");

	IDirect3DSurface9* depth( NULL );
	D3D_V( r3dRenderer->pd3ddev->CreateDepthStencilSurface( texWidth, texHeight, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &depth, NULL ) );
	SetD3DResourcePrivateData(depth, "RenderLevelMinimap_DSS");

	D3DPERF_BeginEvent( 0, L"RenderLevelMinimap" );

	struct SaveRestoreStates
	{
		explicit SaveRestoreStates( IDirect3DSurface9* rt )
		{
			//------------------------------------------------------------------------
			// get previous states
			for( int i = 0, e = sizeof PrevRTs / sizeof PrevRTs[ 0 ]; i < e; i ++ )
			{
				r3dRenderer->GetRT( i, &PrevRTs[ i ] );
			}

			r3dRenderer->GetDSS( &PrevDepth ) ;
			r3dRenderer->GetViewport( &PrevVP ) ;

			PrevView			= r3dRenderer->ViewMatrix;
			PrevProj			= r3dRenderer->ProjMatrix;
			PrevNear			= r3dRenderer->NearClip;
			PrevFar				= r3dRenderer->FarClip;			

			r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW | R3D_BLEND_PUSH );

			D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_SCISSORTESTENABLE, &PrevScissor ) );

			D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_STENCILENABLE, &PrevStencil ) );
		}

		~SaveRestoreStates()
		{
			for( int i = 0, e = sizeof PrevRTs / sizeof PrevRTs[ 0 ]; i < e; i ++ )
			{
				r3dRenderer->SetRT( i, PrevRTs[ i ] ) ;
				if ( PrevRTs[ i ] )
					PrevRTs[ i ]->Release();
			}

			PrevDepth->Release();

			r3dRenderer->SetDSS( PrevDepth ) ;
			r3dRenderer->SetViewport( (float)PrevVP.X, (float)PrevVP.Y, (float)PrevVP.Width, (float)PrevVP.Height ) ;

			r3dRenderer->SetCameraEx( PrevView, PrevProj, PrevNear, PrevFar, false );

			r3dRenderer->SetRenderingMode( R3D_BLEND_POP  );

			r3dRenderer->RestoreCullMode();

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, PrevScissor ) );
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, PrevStencil ) );
		}

		IDirect3DSurface9*	PrevRTs[ 4 ];
		IDirect3DSurface9*	PrevDepth;
		D3DVIEWPORT9		PrevVP;

		D3DXMATRIX	PrevView;
		D3DXMATRIX	PrevProj;
		float		PrevNear;
		float		PrevFar;
		DWORD		PrevScissor;
		DWORD		PrevStencil;

	} saveRestoreStates( rt ); (void)saveRestoreStates;

	if( Terrain )
	{
		Terrain->PrepareOrthographicTerrainRender( texWidth, texHeight );
	}

	r3dRenderer->SetRT( 0, rt );

	for( int i = 1, e = sizeof saveRestoreStates.PrevRTs / sizeof saveRestoreStates.PrevRTs[ 0 ]; i < e; i ++ )
	{
		r3dRenderer->SetRT( i, NULL ) ;
	}

	r3dRenderer->SetDSS( depth ) ;

	D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, r3dRenderer->GetClearZValue(), 0 ) );

	D3DVIEWPORT9 viewport;

	memset( &viewport, 0, sizeof viewport );

	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	viewport.Height = texHeight;
	viewport.Width	= texWidth;

	r3dRenderer->SetViewport( (float)viewport.X, (float)viewport.Y, (float)viewport.Width, (float)viewport.Height ) ;

	r3dRenderer->SetCullMode( D3DCULL_NONE );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );

	r3dPoint3D worldOrigin = GameWorld().m_MinimapOrigin;
	r3dPoint3D worldSize = GameWorld().m_MinimapSize;

	float nearClip = 1.f;
	float farClip = 30000.0f;

	D3DXMATRIX proj;
	r3dRenderer->BuildMatrixOrthoOffCenterLH( &proj, 0, worldSize.x, 0, worldSize.z, nearClip, farClip );

	D3DXMATRIX view;
	D3DXVECTOR3 vEye( worldOrigin.x, 10000, worldOrigin.z );
	D3DXVECTOR3 vAt( worldOrigin.x, 0, worldOrigin.z );
	D3DXVECTOR3 vUp( 0, 0, 1 );

	D3DXMatrixLookAtLH( &view, &vEye, &vAt, &vUp );

	r3dRenderer->SetCameraEx( view, proj, nearClip, farClip, false );

	r3dCamera cam ( r3dPoint3D( vEye.x, vEye.y, vEye.z ) );

	cam.NearClip	= nearClip;
	cam.FarClip		= farClip;

	cam.vPointTo	= r3dPoint3D( 0, -1, 0 );
	cam.vUP			= r3dPoint3D( vUp.x, vUp.y, vUp.z );

	cam.FOV			= 90;

	cam.ProjectionType	= r3dCamera::PROJTYPE_ORTHO;
	cam.Width			= worldSize.x;
	cam.Height			= worldSize.z;
	cam.Aspect			= 1.f;

	extern r3dSun		*Sun;
	const r3dVector& sunDir = Sun->SunLight.Direction;
	

	r3dRenderer->SetRenderingMode(R3D_BLEND_PUSH | R3D_BLEND_ZC | R3D_BLEND_ZW | R3D_BLEND_NOALPHA);

	r3dRenderer->SetMipMapBias( -2 );

	r_in_minimap_render->SetInt( 1 );

	if( Terrain )
	{
		Terrain->DrawOrthographicTerrain( cam, true );
	}

	r3dRenderer->SetVertexShader(VS_FILLGBUFFER_ID);

	FillbufferShaderKey key;
	key.flags.low_q = 1 ;
	SetFillGBufferPixelShader( key ) ;

	D3DXVECTOR4 CamVec = D3DXVECTOR4(cam.x, cam.y, cam.z, 1);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_CAMVEC, (float*)&CamVec, 1);

	float defSSAO[ 4 ] = { r_ssao_clear_val->GetFloat(), 0.f, 0.f, 0.f };
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_DEF_SSAO, defSSAO, 1);

	int old_OQ = r_use_oq->GetInt() ;
	int old_LQ = r_lighting_quality->GetInt() ;

	r_use_oq->SetInt(0);
	r_lighting_quality->SetInt((int)r_lighting_quality->GetMaxVal());

	int totalObjectCount = 0;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		if( iter.current->ObjFlags & OBJFLAG_SkipDraw )
		{
			iter.current->ObjFlags |= OBJFLAG_UserFlag;
		}
		else
		{
			iter.current->ObjFlags &= ~OBJFLAG_UserFlag;
		}

		totalObjectCount ++ ;
	}

	for( int startObjIdx = 0, e = totalObjectCount; startObjIdx < e; startObjIdx += OBJECTMANAGER_MAXOBJECTS )
	{
		int count = OBJECTMANAGER_MAXOBJECTS;
		int idx = 0;

		for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects( iter ), idx ++ )
		{
			if( idx < startObjIdx || idx >= startObjIdx + OBJECTMANAGER_MAXOBJECTS )
			{
				iter.current->ObjFlags |= OBJFLAG_SkipDraw;
			}
			else
			{
				if( !( iter.current->ObjFlags & OBJFLAG_UserFlag ) )
				{
					iter.current->ObjFlags &= ~OBJFLAG_SkipDraw;
				}
			}
		}
		
		GameWorld().Prepare( cam );

		float fbias = 0.f;
		DWORD dbias = (DWORD&)fbias;

		for( int i = 0; i < 8; i ++ )
		{
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, dbias ) );

			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) ) ;
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) ) ;
		}

		// important to refresh this constant as water constants break it ( in rsDrawComposite2 )
		D3DXVECTOR4 vSun(sunDir.x, sunDir.y, sunDir.z, 0.0f);
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 20,(float *)&vSun, 1);

		GameWorld().Draw( rsFillGBuffer );

		GameWorld().Draw( rsDrawComposite2 );
	}

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		if( iter.current->ObjFlags & OBJFLAG_UserFlag )
		{
			iter.current->ObjFlags |= OBJFLAG_SkipDraw;
		}
		else
		{
			iter.current->ObjFlags &= ~OBJFLAG_SkipDraw;
		}

		iter.current->ObjFlags &= ~OBJFLAG_UserFlag;
	}

	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

	r_in_minimap_render->SetInt( 0 );
	r_use_oq->SetInt( old_OQ ) ;
	r_lighting_quality->SetInt( old_LQ ) ;

	extern float __WorldRenderBias;
	r3dRenderer->SetMipMapBias( __WorldRenderBias );

	D3DPERF_EndEvent();

	if( TargetFile )
	{
		IDirect3DSurface9* texSurf;

		D3D_V( tex->GetSurfaceLevel( 0, &texSurf ) );

		D3D_V( r3dRenderer->pd3ddev->GetRenderTargetData( rt, texSurf ) );

		texSurf->Release();

		D3D_V( D3DXSaveTextureToFile( TargetFile, D3DXIFF_DDS, tex, NULL ) );
	}

	SAFE_RELEASE( tex )

	SAFE_RELEASE( rt );

	SAFE_RELEASE( depth );

	g_bTerrainUseLightHack = 0;
	g_bIsMinimapRendering = false;
}

void DrawTerrainBrush( float radius, float hardness, r3dPoint3D pos )
{
	float distToCam = (gCam-pos).Length();
	float size = R3D_LERP(0.5f, 5.0f, R3D_CLAMP(distToCam/1000.0f, 0.0f, 1.0f));
	r3dDrawCircle3DT(pos, radius , gCam, size, r3dColor(100,100,100));
	r3dDrawCircle3DT(pos, radius*hardness , gCam, size/2, r3dColor(255,255,50));
	r3dDrawLine3D(pos, pos+r3dPoint3D(0,4,0), gCam, size/2, r3dColor(255,155,0));
}

void DrawGrassBrush( float radius, float hardness, r3dPoint3D pos )
{
	float distToCam = (gCam-pos).Length();
	float size = R3D_LERP(0.5f, 5.0f, R3D_CLAMP(distToCam/1000.0f, 0.0f, 1.0f));
	r3dDrawCircle3DT_GrassPlanes(pos, radius , gCam, size, r3dColor(100,100,100));
	r3dDrawCircle3DT_GrassPlanes(pos, radius*hardness , gCam, size/2, r3dColor(255,255,50));
	r3dDrawLine3D(pos, pos+r3dPoint3D(0,4,0), gCam, size/2, r3dColor(255,155,0));
}

void DrawWaterBrush(const r3dPoint3D& vBrushPos, float radius)
{
	r3dDrawUniformCircle3D(vBrushPos, radius, gCam, r3dColor::red );
}

//--------------------------------------------------------------------------------------------------------
void LoadShadowSlice( Script_c &script, int nIndex ) 
{
	char buffer[ MAX_PATH ];
	script.SkipToken( "{" );
	assert( nIndex < NumShadowSlices );

	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( *buffer == '}' )
			break;

		if ( ! strcmp( buffer, "DepthBias" ) )
		{
			ShadowSlices[nIndex].depthBias = script.GetFloat();
		}
	}
}


//////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------
int SaveCommonSettings( )
{
	pugi::xml_document xmlFile;
	xmlFile.append_child( pugi::node_comment ).set_value("Level Settings File") ;
	pugi::xml_node xmlRoot = xmlFile.append_child() ;
	xmlRoot.set_name( "root" ) ;

	SerializeCommonSettings<true>( xmlRoot ) ;

	xmlFile.save_file( COMMON_SETTINGS_FILE );

	return 1;
}

int SaveLevelSettingsXML( pugi::xml_node root )
{
	SerializableSettingSet ssSet ;

	root.append_attribute( "version" ) = 3 ;

	ssSet.GlowSettings					= gPFX_ExtractGlow.GetSettings() ;
	ssSet.CamMotionBlurSettings			= gPFX_CameraMotionBlur.GetSettings() ;
	ssSet.RadialBlurSettings			= gPFX_RadialBlur.GetDefaultSettings() ;
	ssSet.ObjMotionBlurSettings			= gPFX_ObjectMotionBlur.GetSettings() ;
	ssSet.SunGlareSettings				= gPFX_SunGlare.GetSettings() ;

	ssSet.LensDirtSettings				= gPFX_LensDirt.GetSettings();
	ssSet.LensDirtExtractLumaSettings	= gPFX_LensDirt_ExtractLuma.GetSettings();

	ssSet.GodRaysSettings				= gPFX_GodRays.GetSettings() ;
	ssSet.SunThroughStencilSettings		= gPFX_SeedSunThroughStencil.GetSettings() ;

	ssSet.ExplosionMaxStrength			= gExplosionVisualController.GetDefaultMaxStrength() ;
	ssSet.ExplosionDuration				= gExplosionVisualController.GetDefaultDuration() ;
	ssSet.ExplosionMaxDistance			= gExplosionVisualController.GetMaxVisibleDistance() ;
	ssSet.ExplosionBrightThreshold		= gExplosionVisualController.GetDefaultBrightThreshold() ;

	SerializeLevelSettingsXML< true >( root, &ssSet );

	return 1 ;
}

//------------------------------------------------------------------------

static int WriteLevelMatLib(r3dgfxVector(r3dMaterial*)& usedMats, const char* matFile, const char* outTexDir)
{
  FILE* f = fopen_for_write(matFile, "wt");
  if(!f) {
    r3dError("unable to open %s for writing\n", matFile);
    return 0;
  }
    
  // write matlib file
  for(size_t i=0; i<usedMats.size(); i++) 
  {
    // minor hack to save OriginalDir to room.mat
    char depot[MAX_PATH];
    sprintf(depot, usedMats[i]->DepotName);
    *usedMats[i]->DepotName = 0;
    usedMats[i]->SaveAscii(f);
    sprintf(usedMats[i]->DepotName, depot);
  }
  fclose(f);

  return 1;
}

//////////////////////////////////////////////////////////////////////////

struct MatNameCompare
{
	bool operator () (const r3dMaterial *l, const r3dMaterial *r) const
	{
		if (!l) return true;
		if (!r) return false;

		return l->Name < r->Name;
	}
};

//////////////////////////////////////////////////////////////////////////

r3dgfxVector(r3dMaterial*) GetUsedMaterials ()
{
	r3dgfxVector(r3dMaterial*) usedMats;

	// collect all used materials in all static mesh objects
	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;

		if( obj->ObjTypeFlags & OBJTYPE_Mesh )
		{
			for(int iMesh = 0; iMesh < 4; ++iMesh)
			{
				r3dMesh* mesh = ((MeshGameObject *)obj)->MeshLOD[iMesh];
				if(mesh)
				{
					for(int idx = 0; idx < mesh->NumMatChunks; idx++)
					{
						r3dMaterial* mat = mesh->MatChunks[idx].Mat;
						if(std::find(usedMats.begin(), usedMats.end(), mat) != usedMats.end())
							continue;

						usedMats.push_back(mat);
					}
				}
			}
		}
		if( obj->ObjTypeFlags & OBJTYPE_Road )
		{
			r3dMesh* mesh = ( ( obj_Road* ) obj )->mesh_;
			r3dMaterial* mat = mesh->MatChunks[ 0 ].Mat;
			if(std::find(usedMats.begin(), usedMats.end(), mat) == usedMats.end())
				usedMats.push_back(mat);
		}

 		if( obj->ObjTypeFlags & OBJTYPE_Trees )
 		{
 			obj_Tree* trees = ( ( obj_Tree* ) obj );
 			trees->AppendMaterials(usedMats);
 		}

	}

	std::sort(usedMats.begin(), usedMats.end(), MatNameCompare());

	return usedMats;
}

static int CreateLevelMatLib()
{
  char matLibFile[MAX_PATH];
  char matLibTextures[MAX_PATH];

  sprintf(matLibFile, "%s\\room.mat", r3dGameLevel::GetSaveDir());
  sprintf(matLibTextures, "%s\\textures\\", r3dGameLevel::GetSaveDir());
  
  r3dgfxVector(r3dMaterial*) usedMats = GetUsedMaterials ();
  
  // save matlib
  WriteLevelMatLib(usedMats, matLibFile, matLibTextures);

  return 1;
}

void SaveLevelObject ( pugi::xml_node & curNode, GameObject *obj )
{
	pugi::xml_node xmlObject = curNode.append_child();
	xmlObject.set_name("object");
	xmlObject.append_attribute("className").set_value(obj->Class->Name.c_str());
	xmlObject.append_attribute("fileName").set_value(obj->FileName.c_str());
	pugi::xml_node xmlObjectPos = xmlObject.append_child();
	xmlObjectPos.set_name("position");
	char temp_str[128];

	r3dPoint3D pos = obj->GetPosition();
	if(obj->PhysicsConfig.isDynamic)
		pos = obj->GetLoadTimePosition();

//	if(!obj->GetLoadTimePosition().AlmostEqual(pos))
//		__asm nop;

	// save floats at %.4f to make merging easier
	sprintf(temp_str, "%.4f", pos.x);
	xmlObjectPos.append_attribute("x") = temp_str;
	sprintf(temp_str, "%.4f", pos.y);
	xmlObjectPos.append_attribute("y") = temp_str;
	sprintf(temp_str, "%.4f", pos.z);
	xmlObjectPos.append_attribute("z") = temp_str;
	obj->WriteSerializedData(xmlObject);
}

void SaveLevelObjects ( pugi::xml_node & curNode, r3dTL::TArray < GameObject * > & dObjects )
{
	for ( uint32_t i = 0; i < dObjects.Count (); i++ )
	{
		GameObject * obj = dObjects[i];
		r3d_assert ( obj );
		if(obj->isSerializable())
			SaveLevelObject ( curNode, obj );
	}
}

void SaveLevelData ( pugi::xml_node & curNode, ESerializeFile sfType )
{
	typedef r3dgfxMap( r3dSTLString, int ) MissingMap;
	MissingMap missingMeshes;
	typedef r3dgfxMap(r3dSTLString, obj_Prefab*) PrefabsToClean;
	PrefabsToClean prefabsToClean;

	ObjectIterator next;

	for(ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = next )
	{
		// bcoz we can delete object right here in case of missing mesh
		next = GameWorld().GetNextOfAllObjects(iter) ;

		GameObject* obj = iter.current;

		if (obj->serializeFile != sfType)
			continue;

		if (!obj->CompoundID )
		{
			if (obj->bPersistent )
				if(obj->isSerializable())
				{
					char buf[ 1024 ] = {0};
					if (obj->isObjType(OBJTYPE_Prefab))
					{
						obj_Prefab *p = static_cast<obj_Prefab*>(obj);

						if (!p->IsInternalObjectsValid())
						{
							PrefabsToClean::iterator found = prefabsToClean.find(p->prefabName);
							if (found == prefabsToClean.end())
							{
								sprintf(buf, "Prefab '%s' have objects with improperly loaded meshes! Delete those objects from prefab definition?", p->Name.c_str());
								int cleanPrefab = MessageBox(r3dRenderer->HLibWin, buf, "Attention!", MB_YESNO) == IDYES;

								prefabsToClean.insert(PrefabsToClean::value_type(p->prefabName, cleanPrefab ? p : 0)) ;
							}
						}
					}

					int do_delete = 0 ;

					if( r3dMesh* mesh = obj->GetObjectMesh() )
					{
						if( mesh->Flags & r3dMesh::obfMissingSource )
						{
							MissingMap::iterator found = missingMeshes.find( mesh->FileName.c_str() ) ;

							if( found == missingMeshes.end() )
							{
								sprintf(	buf, "Mesh '%s' is missing! Delete objects with '%s' from level?", 
											mesh->FileName.c_str(),
											mesh->FileName.c_str() ) ;

								do_delete = MessageBox( r3dRenderer->HLibWin, buf, "Attention!", MB_YESNO ) == IDYES ;

								missingMeshes.insert( MissingMap::value_type( mesh->FileName.c_str(), do_delete ) ) ;
							}
							else
							{
								do_delete = found->second ;
							}

							if( do_delete )
							{
								GameWorld().DeleteObject( obj ) ;
							}
						}
					}

					if( !do_delete )
						SaveLevelObject ( curNode, obj );
				}
		}
	}

	for (PrefabsToClean::iterator it = prefabsToClean.begin(); it != prefabsToClean.end(); ++it)
	{
		if (it->second)
		{
			it->second->CleanInvalidObjects();
			g_PrefabManager.SavePrefabs();
		}
	}
}

void SaveLevelData( char* Str )
{
	pugi::xml_document xmlLevelFile;
	xmlLevelFile.append_child(pugi::node_comment).set_value("Level data");
	pugi::xml_node xmlLevel = xmlLevelFile.append_child();
	xmlLevel.set_name("level");
	
	xmlLevel.append_attribute("version") = 100;

	xmlLevel.append_attribute("minimapOrigin.x") = GameWorld().m_MinimapOrigin.x;
	xmlLevel.append_attribute("minimapOrigin.z") = GameWorld().m_MinimapOrigin.z;
	xmlLevel.append_attribute("minimapSize.x") = GameWorld().m_MinimapSize.x;
	xmlLevel.append_attribute("minimapSize.z") = GameWorld().m_MinimapSize.z;

	//	Saving objects into LevelData
	SaveLevelData ( xmlLevel, SF_LevelData);
	sprintf(Str, "%s\\LevelData.xml", r3dGameLevel::GetSaveDir());
	xmlLevelFile.save_file(Str);

	//	Saving objects into SoundData
	pugi::xml_document xmlSoundFile;
	xmlSoundFile.append_child(pugi::node_comment).set_value("Sound data");
	pugi::xml_node xmlSound = xmlSoundFile.append_child();
	xmlSound.set_name("sounds");

	SaveLevelData(xmlSound, SF_SoundData);
	sprintf(Str, "%s\\SoundData.xml", r3dGameLevel::GetSaveDir());
	xmlSoundFile.save_file(Str);

	//	Saving objects into ServerData
	pugi::xml_document xmlServerFile;
	xmlServerFile.append_child(pugi::node_comment).set_value("Server data");
	pugi::xml_node xmlServer = xmlServerFile.append_child();
	xmlServer.set_name("server_objects");

	SaveLevelData(xmlServer, SF_ServerData);
	sprintf(Str, "%s\\ServerData.xml", r3dGameLevel::GetSaveDir());
	xmlServerFile.save_file(Str);
}

Editor_Level::Editor_Level() 
: m_tPaintTerrainColor( 0xffffffff )
, m_DebugTexture( 0 )
{ 
	ObjcategoryEditMode = 0 ;

	MainToolIdx = EDITMODE_TERRAIN; //@SETTINGS;
	TerrainEditMode = 4;
	ObjectEditMode = 0;
	EnvironmentEditMode = 0;

	DesiredDamageLibEntry[ 0 ] = 0 ;

	for(int i=0; i<TE_NUM; ++i)
	{
		TerrainBrushRadiusVal[i] = 10.0f;
		TerrainBrushHardnessVal[i] = 0.8f;
		TerrainBrushStrengthVal[i] = 0.5f;

		TerrainBrushHeightVal[i] = 10.0f;
		TerrainBrushHeightAdjustVal[i] = 1.0f;
		TerrainBrushNoiseVal[i] =0.5f;
		TerrainBrushNoiseRaiseVal[i] = 0;
	}

	m_pPreviewBuffer = NULL;

	FillFileListFromDir("data\\Editor\\ErosionPatterns\\*.dds", gErosionPatternTextureNames);
}

//////////////////////////////////////////////////////////////////////////

Editor_Level::~Editor_Level()
{
	FreeErosionPattern();
}

//////////////////////////////////////////////////////////////////////////

void SaveXMLSettings( const char* targetDir )
{
	pugi::xml_document xmlFile;
	xmlFile.append_child( pugi::node_comment ).set_value("Level Settings File") ;
	pugi::xml_node xmlRoot = xmlFile.append_child() ;
	xmlRoot.set_name( "root" ) ;

	SaveLevelSettingsXML( xmlRoot ) ;

	r3dGameLevel::Environment->SaveToXML( xmlRoot );

	char XMLFilePath[ MAX_PATH ] ;
	sprintf( XMLFilePath, "%s/LevelSettings.xml", targetDir );

	xmlFile.save_file( XMLFilePath );
}

void SaveEnvironmentToXML( const char* filePath )
{
	pugi::xml_document xmlFile;
	xmlFile.append_child( pugi::node_comment ).set_value("Environment Settings File");
	pugi::xml_node xmlRoot = xmlFile.append_child();
	xmlRoot.set_name( "root" );

	r3dGameLevel::Environment->SaveToXML( xmlRoot );

	xmlFile.save_file( filePath );
}

int LoadEnvironmentFromXML( const char* fileName )
{
	bool XMLExists = false;

	Bytes xmlFileBuffer;

	pugi::xml_document xmlLevelFile;
	pugi::xml_node xmlRoot;

	r3dFile* f = r3d_open( fileName, "rb" );
	if ( f )
	{
		r3dOutToLog( "Loading environment from '%s'\n", fileName );
		XMLExists = true;

		if( !ParseXMLInMemory( f, &xmlFileBuffer, &xmlLevelFile ) )
		{
			fclose( f );
			return 0;
		}

		xmlRoot = xmlLevelFile.root().child( "root" );

		fclose( f );
	}
	else
	{
		r3dOutToLog( "Couldn't open '%s'\n", fileName );
	}

	if( !xmlRoot.empty() )
	{
		return r3dGameLevel::Environment->LoadFromXML( xmlRoot );
	}

	return 0;
}

void Editor_Level::SaveLevel( const char* targetDir, bool showSign, bool autoSave )
{
	char FName[MAX_PATH];

	struct LevelIsSaving
	{
		LevelIsSaving()
		{
			r3dGameLevel::IsSaving = 1;
		}

		~LevelIsSaving()
		{
			r3dGameLevel::IsSaving = 0;
		}
	} levelIsSaving; (void)levelIsSaving;

	if( showSign )
	{
		g_ShowSaveSign = r3dGetTime() + 2.2f;
	}

	struct SetRestoreSaveDir
	{
		SetRestoreSaveDir( const char* dir )
		{
			r3dGameLevel::SetSaveDir( dir );
		}

		~SetRestoreSaveDir()
		{
			r3dGameLevel::SetSaveDir( r3dGameLevel::GetHomeDir() );
		}

	} setRestoreDir( targetDir ) ;

	g_MeshPropertyLib->Save( "Data/", true );
	g_MeshPropertyLib->Save( r3dGameLevel::GetSaveDir(), false );


#if 0
	if( Terrain1 && Terrain1->IsLoaded() )
	{
		Terrain1->ExtractHeightFieldData();
		Terrain1->UpdateAllVertexData();
	}
#endif

	if( showSign )
	{
		const float WI = 220.f ;
		imgui_Static( r3dRenderer->ScreenW2 - WI * 0.5f, r3dRenderer->ScreenH2, "Saving...", WI, false, 22, false ) ;
	}

	UpdateDestructionMeshSettings( NULL );

	SaveSettings( targetDir );

	sprintf( FName, FNAME_CLOUD_LIB, targetDir );
	

	g_pDecalChief->SaveLevel( targetDir );

	// save this as well cause people will forgEt
	g_DamageLib->Save();

	SaveLevelData( FName );

#if 0
	obj_Group::Save();
#endif


	if( g_pTerrain3Editor )
	{
		g_pTerrain3Editor->SaveTerrain( targetDir );
	}

	if( Terrain2 )
	{
		if( g_pTerrain2Editor->IsHeightDirty() )
		{
			g_pTerrain2Editor->SaveHeightsToTerrain() ;
		}

		Terrain2->Save( targetDir ) ;
	}

	{
		g_pGrassMap->Save( targetDir );
		GetGrassPlaneManager()->Save( targetDir );
	}

	gCollectionsManager.Save();

	SaveXMLSettings( targetDir );
	
	for (GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if (obj->ObjTypeFlags & OBJTYPE_Human)
		{
			obj_Player *aip = static_cast<obj_Player*>(obj);
			aip->lifeProperties.SaveAISettingsXML();
		}
	}

#if 0 
	SaveCommonSettings();
#endif

	CreateLevelMatLib();
	
	r3dMaterialLibrary::UpdateDepotMaterials();

#if ENABLE_AUTODESK_NAVIGATION
	gAutodeskNavMesh.SaveBuildConfig();
#endif

	g_PrefabManager.SavePrefabs();

#ifndef FINAL_BUILD
#ifndef WO_SERVER
	CleanOrphanedRoadFiles();
#endif	
#endif
}

void
Editor_Level::ToDamageLib( const char* MeshName )
{
	MainToolIdx			= EDITMODE_OBJECTS ;
	ObjcategoryEditMode	= OBEM_DAMAGELIB ;
	strncpy( DesiredDamageLibEntry, MeshName, sizeof DesiredDamageLibEntry - 1 );
}

extern int __RepositionObjectsOnTerrain;
void RepositionObjectsOnTerrain( const r3dPoint3D &vPos, float fRadius )
{
	if (!__RepositionObjectsOnTerrain) 
		return;

	IUndoItem * pUndoParent =  g_pUndoHistory->GetUndoObject();

	float fRadiusSq = fRadius * fRadius;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects(iter) )
	{
		GameObject *obj = iter.current;

		r3dPoint3D v = obj->GetPosition();


		r3dBoundBox bbox = obj->GetBBoxWorld();
		bbox.Org.y = vPos.y;
		bbox.Grow( fRadius );

		if ( ! bbox.ContainsPoint( vPos ) )
			continue;

		if( !terra_IsWithinPreciseHeightmap( vPos ) )
			continue;

		obj->SetPosition( v );
		
		UndoEntityChanged * pUndo = ( UndoEntityChanged * ) g_pUndoHistory->CreateUndoItem( UA_ENT_CHANGED, pUndoParent );
		assert( pUndo );
		if ( pUndo )
		{
			EntInfo_t st_old;
			EntInfo_t st_new;

			st_old.objHashID = st_new.objHashID = obj->GetHashID();
			st_old.vRot = st_new.vRot = obj->GetRotationVector();
			st_old.vScl = st_new.vScl = obj->GetScale();
			st_old.vPos = v;
			st_new.vPos = obj->GetPosition();			

			pUndo->Create( st_old, st_new );
		}		
	}
}

void RepositionAllObjectsOnTerrain()
{
	if (!__RepositionObjectsOnTerrain) 
		return;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;

		r3dPoint3D v = obj->GetPosition();

		if( terra_IsWithinPreciseHeightmap( v ) )
		{
			obj->SetPosition( v );
		}
	}
}

void MatrixGetYawPitchRoll ( const D3DXMATRIX & mat, float & fYaw, float & fPitch, float & fRoll );

//--------------------------------------------------------------------------------------------------------
void LandscapeCorrect( GameObject& object, const r3dPoint3D& vTargetPos )
{
	if(!Terrain)
	{
		object.SetPosition( vTargetPos );
		return;
	}
		
	r3dBoundBox bbox = object.GetBBoxLocal();

	const int nPointsNum = 5;

	float fAngle = object.GetRotationVector().x;

	r3dPoint3D vOrg;
	r3dPoint3D vSize;

	bbox.Org.RotateAroundY( fAngle, vOrg );
	bbox.Size.RotateAroundY( fAngle, vSize );
	
	vOrg += object.GetPosition();

	r3dPoint3D points[ nPointsNum ];

	points[ 0 ] = vOrg;												// left bottom
	points[ 1 ] = vOrg + r3dPoint3D( 0, 0, vSize.z );				// left top
	points[ 2 ] = vOrg + r3dPoint3D( vSize.x, 0, vSize.z );			// right top
	points[ 3 ] = vOrg + r3dPoint3D( vSize.x, 0, 0 );				// right bottom
	points[ 4 ] = vOrg + r3dPoint3D( vSize.x / 2, 0, vSize.z / 2 );	// center

	r3dPoint3D vObjectX( 1, 0, 0 );
	r3dPoint3D vObjectZ( 0, 0, 1 );
	
	vObjectX.RotateAroundY( fAngle );
	vObjectZ.RotateAroundY( fAngle );

	for (int i = 0; i < nPointsNum; i++)
	{
		float fHeight = Terrain->GetHeight( points[ i ] );
		points[ i ].y = fHeight;
	}
	
	r3dPoint3D v1 = points[ 2 ]	- points[ 0 ];
	r3dPoint3D v2 = points[ 1 ]	- points[ 3 ];
	
	r3dPoint3D vUp = v2.Cross( v1 );
	vUp.Normalize();
	r3dPoint3D vUpProj( vUp.x, 0, vUp.z );
	if ( vUpProj.LengthSq() > FLT_EPSILON )
	{
		r3dPoint3D vRight = vUp.Cross( vObjectZ ).Normalize();
		r3dPoint3D vForward = vObjectX.Cross( vUp ).Normalize();
		vUp = vForward.Cross( vRight );
		D3DXMATRIX m;
		D3DXMatrixIdentity( &m );
		
		m._11 = vRight.x;
		m._12 = vRight.y;
		m._13 = vRight.z;

		m._21 = vUp.x;
		m._22 = vUp.y;
		m._23 = vUp.z;

		m._31 = vForward.x;
		m._32 = vForward.y;
		m._33 = vForward.z;

		r3dPoint3D vAngles;
		MatrixGetYawPitchRoll( m, vAngles.x, vAngles.y, vAngles.z );
		vAngles = R3D_RAD2DEG( vAngles );

		object.SetRotationVector( vAngles );

		D3DXVec3TransformNormal( ( D3DXVECTOR3* ) &vOrg, ( D3DXVECTOR3* ) &bbox.Org, &m );
		D3DXVec3TransformNormal( ( D3DXVECTOR3* ) &vSize, ( D3DXVECTOR3* ) &bbox.Size, &m );

		vOrg += object.GetPosition();
	}

	{
		r3dPoint3D points[ nPointsNum ];

		points[ 0 ] = vOrg;												// left bottom
		points[ 1 ] = vOrg + r3dPoint3D( 0, 0, vSize.z );				// left top
		points[ 2 ] = vOrg + r3dPoint3D( vSize.x, 0, vSize.z );			// right top
		points[ 3 ] = vOrg + r3dPoint3D( vSize.x, 0, 0 );				// right bottom
		points[ 4 ] = vOrg + r3dPoint3D( vSize.x / 2, 0, vSize.z / 2 );	// center

		float fAverageHeight = 0.0f;
		for (int i = 0; i < nPointsNum; i++)
		{
			fAverageHeight += Terrain->GetHeight( points[ i ] );
		}

		fAverageHeight /= nPointsNum;

		object.SetPosition( r3dPoint3D( vTargetPos.x, fAverageHeight, vTargetPos.z ) );
	}
}

float GetTerrainFollowPosition( const r3dBoundBox& BBox)
{
	if (!Terrain) 
		return 0;

	if ( e_reposition_obj_5d->GetBool() )
	{
		r3dPoint3D vMin( BBox.Org );
		r3dPoint3D vMax( BBox.Org + BBox.Size );
		r3dPoint3D vCenter( BBox.Center() );

		r3dPoint3D v1( r3dPoint3D( vMin.x, vMin.y, vMax.z ) );
		r3dPoint3D v2( r3dPoint3D( vMax.x, vMin.y, vMin.z ) );

		float fRes = Terrain->GetHeight( vMin );
		fRes += Terrain->GetHeight( vMax );
		fRes += Terrain->GetHeight( vCenter );
		fRes += Terrain->GetHeight( v1 );
		fRes += Terrain->GetHeight( v2 );

		return fRes / 5.f;
	}
	
	r3dPoint3D tmpP(BBox.Org + BBox.Size);

	float H  = Terrain->GetHeight(BBox.Org);
	float H1 = Terrain->GetHeight(tmpP);

	if(H1 < H) H = H1;

	return H;
}


static r3dTL::TArray< r3dPoint3D > debug_Boxes ;
static r3dTL::TArray< r3dColor > debug_Colors ;

void PushDebugBox(	r3dPoint3D p0, r3dPoint3D p1, r3dPoint3D p2, r3dPoint3D p3,
					r3dPoint3D p4, r3dPoint3D p5, r3dPoint3D p6, r3dPoint3D p7,
					r3dColor color )
{
	debug_Boxes.PushBack( p0 ) ;
	debug_Boxes.PushBack( p1 ) ;
	debug_Boxes.PushBack( p2 ) ;
	debug_Boxes.PushBack( p3 ) ;

	debug_Boxes.PushBack( p4 ) ;
	debug_Boxes.PushBack( p5 ) ;
	debug_Boxes.PushBack( p6 ) ;
	debug_Boxes.PushBack( p7 ) ;

	debug_Colors.PushBack( color ) ;
}

void ResetDebugBoxes()
{
	debug_Boxes.Clear();
	debug_Colors.Clear();
}

void DEBUG_DrawDebugBoxes()
{
	r3d_assert( !( debug_Boxes.Count() % 8 ) );
	r3d_assert( debug_Boxes.Count() == debug_Colors.Count() * 8 ) ;

	for( uint32_t i = 0, c = 0, e = debug_Boxes.Count(); i < e; i += 8, c += 1 )
	{
		r3dColor col = debug_Colors[ c ] ;

		PushUniformLine3D( debug_Boxes[ i + 0 ], debug_Boxes[ i + 1 ], col );
		PushUniformLine3D( debug_Boxes[ i + 1 ], debug_Boxes[ i + 3 ], col );
		PushUniformLine3D( debug_Boxes[ i + 3 ], debug_Boxes[ i + 2 ], col );
		PushUniformLine3D( debug_Boxes[ i + 2 ], debug_Boxes[ i + 0 ], col );

		PushUniformLine3D( debug_Boxes[ i + 4 ], debug_Boxes[ i + 5 ], col );
		PushUniformLine3D( debug_Boxes[ i + 5 ], debug_Boxes[ i + 7 ], col );
		PushUniformLine3D( debug_Boxes[ i + 7 ], debug_Boxes[ i + 6 ], col );
		PushUniformLine3D( debug_Boxes[ i + 6 ], debug_Boxes[ i + 4 ], col );

		PushUniformLine3D( debug_Boxes[ i + 0 ], debug_Boxes[ i + 4 ], col );
		PushUniformLine3D( debug_Boxes[ i + 1 ], debug_Boxes[ i + 5 ], col );
		PushUniformLine3D( debug_Boxes[ i + 2 ], debug_Boxes[ i + 6 ], col );
		PushUniformLine3D( debug_Boxes[ i + 3 ], debug_Boxes[ i + 7 ], col );
	}

	ResetDebugBoxes() ;
}

void DEBUG_DrawSceneBoxes()
{
	if( r_show_bbox->GetInt() )
	{
		for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
		{
			GameObject* obj = iter.current;

			const r3dBoundBox& bbox = obj->GetBBoxWorld() ;

			r3dPoint3D v0( bbox.Org + bbox.Size );
			r3dPoint3D v1( bbox.Org );

			r3dPoint3D pts[ 8 ] ;

			int i = 0 ;

			pts[ i ++ ] = r3dPoint3D( v0.x, v0.y, v0.z ) ;
			pts[ i ++ ] = r3dPoint3D( v1.x, v0.y, v0.z ) ;
			pts[ i ++ ] = r3dPoint3D( v0.x, v1.y, v0.z ) ;
			pts[ i ++ ] = r3dPoint3D( v1.x, v1.y, v0.z ) ;
			pts[ i ++ ] = r3dPoint3D( v0.x, v0.y, v1.z ) ;
			pts[ i ++ ] = r3dPoint3D( v1.x, v0.y, v1.z ) ;
			pts[ i ++ ] = r3dPoint3D( v0.x, v1.y, v1.z ) ;
			pts[ i ++ ] = r3dPoint3D( v1.x, v1.y, v1.z ) ;

			PushUniformLine3D( pts[ 0 ], pts[ 1 ], r3dColor::red );
			PushUniformLine3D( pts[ 1 ], pts[ 3 ], r3dColor::red );
			PushUniformLine3D( pts[ 3 ], pts[ 2 ], r3dColor::red );
			PushUniformLine3D( pts[ 2 ], pts[ 0 ], r3dColor::red );

			PushUniformLine3D( pts[ 4 ], pts[ 5 ], r3dColor::red );
			PushUniformLine3D( pts[ 5 ], pts[ 7 ], r3dColor::red );
			PushUniformLine3D( pts[ 7 ], pts[ 6 ], r3dColor::red );
			PushUniformLine3D( pts[ 6 ], pts[ 4 ], r3dColor::red );

			PushUniformLine3D( pts[ 0 ], pts[ 4 ], r3dColor::red );
			PushUniformLine3D( pts[ 1 ], pts[ 5 ], r3dColor::red );
			PushUniformLine3D( pts[ 2 ], pts[ 6 ], r3dColor::red );
			PushUniformLine3D( pts[ 3 ], pts[ 7 ], r3dColor::red );
		}		
	}

	if( r_show_scenebox->GetInt() )
	{
		GameWorld().AppendDebugBoxes();

		extern r3dTL::TArray< r3dPoint3D > debug_SceneBox;
		extern r3dTL::TArray< r3dColor24 > debug_SceneBoxColors;

		r3d_assert( !( debug_SceneBox.Count() % 8 ) );
		r3d_assert( debug_SceneBoxColors.Count() * 8 == debug_SceneBox.Count() );

		for( uint32_t i = 0, j = 0, e = debug_SceneBox.Count(); i < e; i += 8, j ++ )
		{
			r3dColor24 clr = debug_SceneBoxColors[ j ];

			PushUniformLine3D( debug_SceneBox[ i + 0 ], debug_SceneBox[ i + 1 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 1 ], debug_SceneBox[ i + 3 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 3 ], debug_SceneBox[ i + 2 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 2 ], debug_SceneBox[ i + 0 ], clr );

			PushUniformLine3D( debug_SceneBox[ i + 4 ], debug_SceneBox[ i + 5 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 5 ], debug_SceneBox[ i + 7 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 7 ], debug_SceneBox[ i + 6 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 6 ], debug_SceneBox[ i + 4 ], clr );

			PushUniformLine3D( debug_SceneBox[ i + 0 ], debug_SceneBox[ i + 4 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 1 ], debug_SceneBox[ i + 5 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 2 ], debug_SceneBox[ i + 6 ], clr );
			PushUniformLine3D( debug_SceneBox[ i + 3 ], debug_SceneBox[ i + 7 ], clr );
		}

		debug_SceneBox.Clear();
		debug_SceneBoxColors.Clear();

		const SceneTraversalStats& stats = GetSceneTraversalStats();

		char sceneBoxMsg[ 256 ];

		sprintf( sceneBoxMsg, "Traversed %d( max %d )", stats.NumTraversedNodes, stats.MaxTraversedNodes );
		
		_r3dSystemFont->PrintF( 10, 220, r3dColor24::white, sceneBoxMsg );
	}
}

void DEBUG_Player()
{
#ifndef FINAL_BUILD
	if( d_show_muzzle->GetInt() )
	{
		for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->ObjTypeFlags & OBJTYPE_Human )
			{
				obj_Player* aip = static_cast< obj_Player* > ( obj );

				PushCross3D( aip->GetMuzzlerPosition(), 0.165f ) ;
			}
		}
	}
#endif
}

static int g_ZombieModDebugVisFlags = 0;
void DEBUG_DrawZombieModHelpers()
{
#ifndef FINAL_BUILD
	if (g_ZombieModDebugVisFlags == 0)
		return;

	obj_Player* aip = 0;
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if( obj->ObjTypeFlags & OBJTYPE_Human )
		{
			aip = static_cast< obj_Player* > ( obj );
		}
		if (obj->ObjTypeFlags & OBJTYPE_Zombie)
		{
			obj_Zombie *z = static_cast<obj_Zombie*>(obj);
			//z->DrawDebugInfo();
		}
	}

	if (!aip)
		return;

	aip->lifeProperties.DebugVisualizeRanges();
//	aip->lifeProperties.DebugDrawZombieSenses();
#endif
}


static float g_LastAutoSave = - 1;

static void UpdateAutoSave( Editor_Level* lev )
{
	float curTime = r3dGetTime() ;

	if( g_LastAutoSave < 0 )
	{
		g_LastAutoSave = curTime ;
	}

	if( curTime - g_LastAutoSave > e_auto_save_interval->GetFloat() * 60.f )
	{
		g_LastAutoSave = curTime ;

		const char* autoSaveFolder = e_auto_save_folder->GetString() ;
		if( !strlen( autoSaveFolder ) )
		{
			autoSaveFolder = "AutoSaves" ;
		}

		time_t tt = time( NULL ) ;
		tm* ttm = localtime( &tt ) ;

		char path[ MAX_PATH * 3 ] ;

		const char* levelless = r3dGameLevel::GetHomeDir() ;

		if( !strnicmp( levelless, "Levels\\", sizeof "Levels\\" - 1 )
				||
			!strnicmp( levelless, "Levels/", sizeof "Levels\\" - 1 )			
			)
		{
			levelless += sizeof "Levels\\" - 1 ;
		}

		sprintf(	path, "%s/%s/AutoSave_%d.%02d.%02d_%02d-%02d-%02d", 
					autoSaveFolder, 
					levelless,
					ttm->tm_year + 1900, ttm->tm_mon + 1, ttm->tm_mday,
					ttm->tm_hour, ttm->tm_min, ttm->tm_sec ) ;

		r3d_create_path( path ) ;


		// in the end, this function will restore save dir to home dir
		lev->SaveLevel( path, true, true ) ;

		// gather directories

		typedef r3dgameMap( INT64, r3dString ) DirMap ;
		DirMap mapped ;

		char multipleAutoFolder[ MAX_PATH * 3 ] ;
		sprintf( multipleAutoFolder, "%s/%s/*.*", autoSaveFolder, levelless ) ;

		HANDLE hFind;
		BOOL bContinue = TRUE;
		WIN32_FIND_DATA data;

		hFind = FindFirstFile(multipleAutoFolder, &data); 

		for ( ; hFind && bContinue ; )
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				int year, month, date, hour, minute, second ;

				if( sscanf( data.cFileName, "AutoSave_%d.%02d.%02d_%02d-%02d-%02d", &year, &month, &date, &hour, &minute, &second ) == 6 )
				{
					mapped.insert(	DirMap::value_type
										(	
											year * 12 * 31 * 24 * 60 * 60 +
											month * 31 * 24 * 60 * 60 +
											date * 24 * 60 * 60 +
											hour * 60 * 60 +
											minute * 60 +
											second
											,
											data.cFileName
										)
									) ;
				}
			}	
			bContinue = FindNextFile(hFind, &data);
		}

		FindClose( hFind ) ;

		int toDelete = (int)mapped.size() - e_auto_save_depth->GetInt() ;

		if( toDelete > 0 )
		{
			multipleAutoFolder[ strlen( multipleAutoFolder ) - 3 ] = 0 ;

			for( DirMap::const_iterator i = mapped.begin(), e = mapped.end() ; i != e && toDelete ; ++ i, toDelete -- )
			{
				sprintf( path, "%s%s", multipleAutoFolder, i->second.c_str() ) ;
				r3d_delete_dir( path ) ;
			}
		}
	}
}

void DEBUG_Draw_Instance_Wind() ;

int g_DrawSunPath;
int g_DrawMoonPath;

void DrawPhysXCollisionGeometry()
{
	if( e_render_collision_geometry->GetFloat() > 0.05f )
	{
		float distSq = e_render_collision_geometry->GetFloat();
		distSq *= distSq;

		for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
		{
			GameObject* obj = iter.current;
			r3dPoint3D distVec = obj->GetPosition() - gCam;

			if( distVec.LengthSq() < distSq )
			{
				obj->DrawPhysxGeometry( gCam, rsDrawComposite1 );
			}
		}
	}
}

void Editor_Level :: Process(bool enable)
{
	R3DPROFILE_FUNCTION( "Editor_Level::Process" );

#if ENABLE_AUTODESK_NAVIGATION
	enable &= Nav::gNavMeshBuildComplete != 0;
#endif

	void DEBUG_ProcessSpawnPlayers();
	DEBUG_ProcessSpawnPlayers();

	if( g_pTerrain3Editor )
		g_pTerrain3Editor->Update();

	if( d_print_postfx->GetInt() )
	{
		g_pPostFXChief->DEBUG_PrintPostFXes();
	}

	if( g_DrawSunPath )
	{
		void DrawSunPath();
		DrawSunPath();

		if( enable )
		{
			g_DrawSunPath = 0;
		}
	}

	if( g_DrawMoonPath )
	{
		void DrawMoonSpot();
		DrawMoonSpot();

		if( enable )
		{
			g_DrawMoonPath = 0;
		}
	}

	DrawPhysXCollisionGeometry();

	if( r_show_luma->GetInt() )
	{
		void DisplayLumaInfo();
		DisplayLumaInfo();
	}

	if( e_auto_save->GetInt() )
	{
		UpdateAutoSave( this ) ;
	}


	if( Terrain2 || Terrain3 )
	{
		__HaveTerrain = 1 ;
	}

	g_Manipulator3d.Draw();

	if( r_show_draw_order->GetInt() )
	{
		void DrawOverlappingPositions( const Positions& poses );
		DrawOverlappingPositions( gDEBUG_DrawPositions );
	}

	gDEBUG_DrawPositions.Clear();

	if(!enable)
	{
		g_GUIHelperStack.Clear();
	}

	// draw these anyways
	DEBUG_DrawDebugBoxes();
	DEBUG_DrawSceneBoxes();
	DEBUG_Player();
	DEBUG_Draw_Instance_Wind() ;
	DEBUG_DrawZombieModHelpers();

	void DrawPreGUIHelpers ();
	DrawPreGUIHelpers();

	if( r_show_shadow_scheme->GetInt() )
	{
		const float viewWidth = r3dRenderer->ScreenW * 0.333f ;
		const float viewHeight = r3dRenderer->ScreenW * 0.333f ;

		float startx = r3dRenderer->ScreenW2 - viewWidth * 0.5f ;
		float starty = r3dRenderer->ScreenH2 - viewHeight * 0.5f ;

		if( r_show_shadow_scheme->GetInt() == 1 )
			RenderShadowScheme( startx, starty, viewWidth, viewHeight );
		else
			RenderTransparentShadowScheme( startx, starty, viewWidth, viewHeight );
	}

	int numObjects = GameWorld().NumObjects + GameWorld().NumStaticObjects;

	const char* dir = r3dGameLevel::GetHomeDir() ;

	char LevelName[ 256 ] = "UNKNOWN" ;

	for( size_t i = strlen( dir ) ; i >= 0 ; i --  )
	{
		if( dir[ i ] == '\\' || dir[ i ] == '/' )
		{
			strcpy( LevelName, dir + i + 1 ) ;

			if( enable )
			{
				if( strlen( LevelName ) > 7 )
				{
					LevelName[ 7 ] = 0 ;
				}
			}

			strupr( LevelName ) ;

			break ;
		}
	}

	if( !r_hide_editor_statusbar->GetBool() )
	{
		
		HANDLE hProcess;
		PROCESS_MEMORY_COUNTERS pmc;

		ZeroMemory( &pmc, sizeof pmc );
		pmc.cb = sizeof pmc;

		hProcess = GetCurrentProcess();

		GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc) );

		r3dDrawBox2D(5, r3dRenderer->ScreenH-30, r3dRenderer->ScreenW-10, 25, imgui_bkgDlg);

#if ENABLE_AUTODESK_NAVIGATION
		if (Nav::gNavMeshBuildComplete == 0)
		{
			Nav::gDrawBuildInfo.Draw();
		}
		else
#endif
		{
			char text[ 1024 ];

			sprintf( text, "%s FPS %3.1f[%02.2fms] Objs:[%d] Cam:[%3.1f|%3.1f|%3.1f] Tri:%d ", LevelName, r3dGetAvgFPS(), 1000.0f / r3dGetAvgFPS(), numObjects , gCam.x, gCam.y, gCam.z, r3dRenderer->Stats.GetNumTrianglesRendered() );

			SIZE sz;
			Font_Label->GetTextExtent( text, &sz );

			int widthaccum = sz.cx + 5;

			Font_Label->PrintF(	10, r3dRenderer->ScreenH-25, r3dColor(255,255,255), "%s", text );

			int numDraws = gBudgeter.currentData.values[NumDraws];

			r3dColor drawsColor = r3dColor(255,255,255);

			if( numDraws > 3000 )
			{
				drawsColor = r3dColor::red;
			}
			else
			if( numDraws > 2000 )
			{
				drawsColor = r3dColor::yellow;
			}

			sprintf( text, "Draw:%d", numDraws );

			Font_Label->GetTextExtent( text, &sz );

			Font_Label->PrintF(	10 + widthaccum, r3dRenderer->ScreenH-25, drawsColor, "%s", text );

			widthaccum += sz.cx + 5;

			Font_Label->PrintF(	
				10 + widthaccum, r3dRenderer->ScreenH-25,600, 22, r3dColor(255,255,255), 
				"TerraDraw:%d (%d tri) Mem:%.2f(%.2f)GB PF:%.2f(%.2f)GB", 												
				r3dRenderer->Stats.GetNumTerrainDraws(), r3dRenderer->Stats.GetNumTerrainTris(), 
				float(pmc.WorkingSetSize)/1024.f/1024.f/1024.f,
				float(pmc.PeakWorkingSetSize)/1024.f/1024.f/1024.f,
				float(pmc.PagefileUsage)/1024.f/1024.f/1024.f,
				float(pmc.PeakPagefileUsage)/1024.f/1024.f/1024.f
				);
		}
	}

	gCollectionsManager.DEBUG_Draw();

	if( g_ShowSaveSign > r3dGetTime() )
	{
		const float WI = 220.f ;
		imgui_Static( r3dRenderer->ScreenW2 - WI * 0.5f, r3dRenderer->ScreenH2, "Saved!", WI, false, 22, false ) ;
	}

	if( g_ShowNoClipExportSign > r3dGetTime() )
	{
		const float WI = 220.f ;
		imgui_Static( r3dRenderer->ScreenW2 - WI * 0.5f, r3dRenderer->ScreenH2, "Exported!", WI, false, 22, false ) ;
	}

#if ENABLE_AUTODESK_NAVIGATION
	Nav::gDrawBuildInfo.Draw();
#endif

	if( !enable )
		return ;

	r_terrain3_showeditable->SetInt( 0 );

	R3DPROFILE_START( "Hud->ProcessPick" );

	Hud->ProcessPick();

	R3DPROFILE_END( "Hud->ProcessPick" );

#if !DISABLE_CONSOLE
	g_pEngineConsole->Update();
#endif

	imgui_Update();
	imgui2_Update();

	const static char* list[11] = { "Settings", "Terrain", "Objects", "Materials", "Environment", "Collections", "Decorators", "Roads", "Gameplay", "Post FX", "Color Correction" };

	imgui_Toolbar(5, 5, r3dRenderer->ScreenW-10, 35, &MainToolIdx, Editor_Level::EDITMODE_SETTINGS, list, sizeof list / sizeof list[ 0 ], false );

	if( g_RoadEditingSwtichedOffTerrain3RoadBaking && MainToolIdx != EDITMODE_ROADS && Terrain3 )
	{
		r3dTerrain3::QualitySettings sts = Terrain3->GetCurrentQualitySettings();

		sts.BakeRoads = 1 ;

		Terrain3->SetQualitySettings( sts, true, r_terrain_quality->GetInt() - 1 );

		Terrain3->RefreshAtlasTiles();

		g_RoadEditingSwtichedOffTerrain3RoadBaking = 0;
	}

	if( g_RoadEditingSwtichedOffTerrain2RoadBaking && MainToolIdx != EDITMODE_ROADS && Terrain2 )
	{
		r3dTerrain2::QualitySettings sts = Terrain2->GetCurrentQualitySettings() ;

		sts.BakeRoads = 1 ;

		Terrain2->SetQualitySettings( sts, true, r_terrain_quality->GetInt() - 1 ) ;

		Terrain2->RefreshAtlasTiles() ;

		g_RoadEditingSwtichedOffTerrain2RoadBaking = 0 ;
	}

	bool showD3DMarks = r_show_d3dmarks->GetBool() ;

	if (imgui_Button(r3dRenderer->ScreenW-10-350-90, r3dRenderer->ScreenH-28, 80, 20, "D3D Marks", showD3DMarks, false))
	{
		showD3DMarks = !showD3DMarks ;

		if( showD3DMarks )
		{
			r_show_d3dmarks->SetBool( true ) ;
			r_show_profiler->SetBool( false ) ;
			r_profiler_d3d->SetBool( false );
		}
		else
		{
			r_show_d3dmarks->SetBool( false ) ;
		}
	}

	int bShowProfiler = r_show_profiler->GetBool();
	int bShowBudgeter = r_show_budgeter->GetBool();

	if (imgui_Button(r3dRenderer->ScreenW-10-350, r3dRenderer->ScreenH-28, 90, 20, "Profiler", bShowProfiler, false) )
	{
		bShowProfiler = !bShowProfiler;

		if( bShowProfiler )
		{
			r_show_d3dmarks->SetBool( false ) ;
		}

		//turn off D3D profiling
		if(bShowProfiler==false)	r_profiler_d3d->SetBool(false);
	}

	if (imgui_Button(r3dRenderer->ScreenW-10-350 + 95, r3dRenderer->ScreenH-28, 90, 20, "Budgeter", bShowBudgeter, false))
		bShowBudgeter = !bShowBudgeter;

	r_show_profiler->SetBool(bShowProfiler!=0);
	r_show_budgeter->SetBool(bShowBudgeter!=0);

	if ( imgui_Button( 5, r3dRenderer->ScreenH-200, DEFAULT_CONTROLS_WIDTH / 2.f, 20,  Va( "Undo: %s", g_pUndoHistory->CanUndo() ? g_pUndoHistory->GetUndoInfo() : "empty" ), 0, false) || ( Keyboard->IsPressed( kbsLeftControl ) && Keyboard->WasPressed( kbsZ ) ))
	{
		g_pUndoHistory->Undo();
	}
	
	if ( imgui_Button( 5 + DEFAULT_CONTROLS_WIDTH/2.f, r3dRenderer->ScreenH-200, DEFAULT_CONTROLS_WIDTH/2.f, 20, Va( "Redo: %s", g_pUndoHistory->CanRedo() ? g_pUndoHistory->GetRedoInfo() : "empty" ), 0, false) || ( Keyboard->IsPressed( kbsLeftControl ) && Keyboard->WasPressed( kbsU ) ) )
	{
		g_pUndoHistory->Redo();
	}


	if( e_hotkill->GetInt() )
	{
		if( Keyboard->IsPressed( kbsK ) )
		{
			ProcessKill();
		}

		if( Keyboard->IsPressed( kbsR ) )
		{
			ProcessReveal();
		}
	}

	//imgui_Button(r3dRenderer->ScreenW-10-150, r3dRenderer->ScreenH-28, 100, 20, "TRANSFER TO", 0, false);

	float oldTime = r3dGameLevel::Environment->__CurTime ;
	
	float tempTime = r3dGameLevel::Environment->__CurTime;
	imgui_Value_Slider(5, r3dRenderer->ScreenH-35-25, "Time of Day",			&tempTime,		0,24,		"%-02.2f",1, false);
	r3dGameLevel::Environment->__CurTime = tempTime;

	if( oldTime != r3dGameLevel::Environment->__CurTime )
	{
		ResetShadowCache();
	}

	float editor_speed = r_editor_move_speed->GetFloat();
	imgui_Value_Slider( 370, r3dRenderer->ScreenH-35-25, "Speed",			&editor_speed,		1,50,		"%-02.0f",1, false);
	r_editor_move_speed->SetFloat(editor_speed);

	//  //Font_Label->PrintF(10, r3dRenderer->ScreenH-35+5,r3dColor(255,255,255), "fps %3.1f[%02.2f]   POS: %.2f %.2f %.2f", 1.0f/r3dGetFrameTime(), r3dGetFrameTime()*1000.0f,FPS_Position.X, FPS_Position.Y, FPS_Position.Z);

	/*
	if ( imgui_Button(r3dRenderer->ScreenW-10-35, r3dRenderer->ScreenH-28, 40, 20, "PS3", 0, false) )
	{
		if( MessageBox( r3dRenderer->HLibWin, "Export for ps3?", "Confirm", MB_YESNO ) == IDYES )
		{
			SaveLevel( r3dGameLevel::GetHomeDir(), true, false );
			TransferToPS3 ();
		}
	}
*/

	float fStatisticYStart = r3dRenderer->ScreenH - 200;
	float fStatisticHeight = 120;
	// picker
	{
		// fix me!!!
		int dAngles[] = {5,10,15,20,30,45,90};
		const char *dAngleSnappingList[] = { "5", "10", "15", "20", "30", "45", "90" };
		
		float dMoves[] = {0.1f,0.5f,1.0f,2.0f,5.0f,10.0f};
		const char *dMoveSnappingList[] = { "0.1", "0.5", "1.0", "2.0", "5.0", "10.0" };
		
		const float PICKER_LIST_HEIGHT = 50;
		float pickerSliderX = 5.0f;
		float pickerSliderY = r3dRenderer->ScreenH - 35-25 - (10 * 2 + 3 * DEFAULT_CONTROLS_HEIGHT);

		if ( g_AngleSnappingEnable )
			pickerSliderY -= DEFAULT_CONTROLS_HEIGHT;
		else
			pickerSliderY -= 10;
		if ( g_MoveSnappingEnable )
			pickerSliderY -= DEFAULT_CONTROLS_HEIGHT;
		else
			pickerSliderY -= 10;

		fStatisticYStart = pickerSliderY;
		fStatisticHeight = (r3dRenderer->ScreenH - 35-25) - fStatisticYStart - 10;

		pickerSliderY += imgui_Static ( pickerSliderX, pickerSliderY, "Picker Settings", DEFAULT_CONTROLS_WIDTH, false );
		pickerSliderY += 10;
		pickerSliderY += imgui_Checkbox ( pickerSliderX, pickerSliderY, "Angle Snapping Enable", &g_AngleSnappingEnable, 1, 0, false );
		if ( g_AngleSnappingEnable )
		{
			Edit_Value_List ( pickerSliderX+5, pickerSliderY, "Angle Snapping", &g_AngleSnappingIndex, dAngleSnappingList, sizeof ( dAngleSnappingList ) / sizeof ( dAngleSnappingList [0] ) );
			pickerSliderY += DEFAULT_CONTROLS_HEIGHT;
		}
		else
			pickerSliderY += 10;

		pickerSliderY += imgui_Checkbox ( pickerSliderX, pickerSliderY, "Move Snapping Enable", &g_MoveSnappingEnable, 1, 0, false );
		if ( g_MoveSnappingEnable )
		{
			Edit_Value_List ( pickerSliderX+5, pickerSliderY, "Move Snapping", &g_MoveSnappingIndex, dMoveSnappingList, sizeof ( dMoveSnappingList ) / sizeof ( dMoveSnappingList [0] ) );
			pickerSliderY += DEFAULT_CONTROLS_HEIGHT;
		}
		else
			pickerSliderY += 10;

		g_Manipulator3d.SetMoveSnapping ( g_MoveSnappingEnable != 0, dMoves[g_MoveSnappingIndex] );
		g_Manipulator3d.SetAngleSnapping ( g_AngleSnappingEnable != 0, (float)dAngles[g_AngleSnappingIndex] );
	}

	{
		char sStatStr [MAX_PATH];
		sStatStr[0] = 0;
		GameObject * pPickedObj = g_Manipulator3d.PickedObject();
		if ( pPickedObj && pPickedObj->GetObjStat ( sStatStr, MAX_PATH - 1 ) )
		{
			imgui_Static ( 370, fStatisticYStart, sStatStr, 360, true, fStatisticHeight );
		}
	}

	// shotkeys

	if ( Keyboard->IsPressed(kbsLeftControl) && Keyboard->IsPressed(kbsS) )
		SaveLevel( r3dGameLevel::GetHomeDir(), true, false );

	if ( Keyboard->IsPressed(kbsLeftControl) && Keyboard->IsPressed(kbsM) )
	{
		r3dMaterialLibrary::UpdateDepotMaterials();
		MessageBoxA( r3dRenderer->HLibWin, "Saved level materials!", "Message", MB_OK );
	}

	if(Keyboard->WasPressed(kbsF))
		FocusOnObject();
	if(Keyboard->IsPressed(kbsLeftAlt) && Keyboard->WasPressed(kbsW))
		ResetObjectRotation();
	if(Keyboard->WasPressed(kbsEsc))
		g_Manipulator3d.PickerResetPicked();

	static bool CtrlBIsPressed = false;
	if (Keyboard->IsPressed(kbsLeftControl) && Keyboard->IsPressed(kbsB))
	{
		if (!CtrlBIsPressed)
		{
			CtrlBIsPressed = true;

			int mx, my;
			Mouse->GetXY(mx, my);
			r3dPoint3D pos, dir;
			r3dScreenPosTo3DPos(mx, my, &pos);
			r3dScreenTo3D(mx, my, &dir);

			PxRaycastHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
			if(g_pPhysicsWorld->raycastSingle(PxVec3(pos.x, pos.y, pos.z), PxVec3(dir.x, dir.y, dir.z), 20000, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
			{
				gExplosionVisualController.AddExplosion(r3dVector(hit.impact.x, hit.impact.y, hit.impact.z), 20.0f);
			}
		}
	}
	else
	{
		CtrlBIsPressed = false;
	}

	if( MainToolIdx != EDITMODE_SETTINGS )
	{
		// turn off, as e.g. moving objects will crash everything.
		r_sector_loading->SetInt( 0 );
	}

	if( MainToolIdx != EDITMODE_OBJECTS )
	{
		g_Manipulator3d.SetSnapVetexes( false );
	}

	switch (MainToolIdx)
	{
	case	EDITMODE_SETTINGS:
		ProcessSettings();
		break;

	case	EDITMODE_TERRAIN:
		{
				if( Terrain2 )
				{
					ProcessTerrain2() ;
				}
				if( Terrain3 )
				{
					ProcessTerrain3();
				}
		}
		break;

	case	EDITMODE_OBJECTS:
		{
			float toolBarW = 950;
			imgui_Toolbar(5, 45, toolBarW, 35, &ObjcategoryEditMode, 0, ListObjectEditModes, R3D_ARRAYSIZE(ListObjectEditModes), false);

			if (r3dRenderer->ScreenW - DEFAULT_CONTROLS_WIDTH < toolBarW)
			{
				g_ObjectsSectionsStarty = 85.0f;
			}
			else
			{
				g_ObjectsSectionsStarty = 45.0f;
			}

			if( ObjcategoryEditMode != OBEM_OBJECTS )
			{
				g_Manipulator3d.SetSnapVetexes( false );
			}

			switch ( ObjcategoryEditMode )
			{
			case OBEM_OBJECTS:
				ProcessObjects();
				break;
			case OBEM_GROUPS:
				ProcessGroups();
				break;
			case OBEM_PREFABS:
				ProcessPrefabs();
				break;
			case OBEM_DAMAGELIB:
				ProcessDamageLib();
				break;
			case OBEM_TCAMO:
				break;
			case OBEM_UTILS:
				ProcessUtils();
				break;
			case OBEM_CAMERA_SPOTS:
				ProcessCameraSpots();
				break;
			case OBEM_DRAW_DITANCES:
				ProcessObjectDistances();
				break;
			}
		}
		break;

	case	EDITMODE_MATERIALS:
		ProcessMaterials();
		break;


	case	EDITMODE_ENVIRONMENT:
		ProcessEnvironment();
		break;

	case	EDITMODE_COLLECTION:
		ProcessCollections();
		break;

	case	EDITMODE_DECORATORS:
		ProcessDecorators();
		break;

	case	EDITMODE_ROADS:
		ProcessRoads();
		break;

	case	EDITMODE_GAMEPLAY:
		{
			const static char* listego[] = { "OBJECTS", "MESHES", "ASSETS", "PARTICLE GUN", "BATTLE ZONE", "CAMERA RIG", "MINIMAP", "CHARACTER", "DYN BATTLEFIELD", "NAVIGATION", "ZOMBIE MOD" };

			static int GameplayEditMode = 0;
			imgui_Toolbar(5, 45, 1050, 35, &GameplayEditMode, 0, listego, sizeof( listego ) / sizeof( listego[ 0 ] ), false);

			switch (GameplayEditMode)
			{
				case 0:
					ProcessGamePlay();
					break;
				
				case 1:	ProcessMeshes();
					break;

				case 2:	ProcessAssets();
					break;

				case 3:	ProcessParticleGun();
					break;
				case 4: 
					break;

				case 5: 
					{
						void ProcessCameraRigEditor ();
						ProcessCameraRigEditor ();
					}
					break;
				case 6:
					{
						void ProcessMinimapEditor ();
						ProcessMinimapEditor ();
					}
					break;
				case 7: // character
					{
						void ProcessCharacterEditor();
						ProcessCharacterEditor();
					}
					break;

				case 8: // Dynamic Battlefield
					break;
				case 9: // Navigation mesh
					{
						ProcessNavigation();
					}
					break;
				case 10: // Zombie mod
					{
						void ProcessZombieMod();
						ProcessZombieMod();
					}
					break;

			}

		}
		break;

	case	EDITMODE_POSTFX:
		ProcessPostFX();
		break;
	case EDITMODE_COLORGRADING:
		ProcessColorGrading();
		break;
	};

	MouseMoveUpdate();

	FlushStates();

	win::input_Flush();
	g_pDesktopManager->Draw();

}





void Editor_Level :: ProcessSettings()
{
	const static char* list[] =		{ "System Settings",	"Options Menu"					};
	enum							{ SE_SYSTEM,			SE_OPTIONS_MENU,	SE_COUNT	};

	static int OptionsEditMode = 0;

	imgui_Toolbar( 5, 45, SE_COUNT * 120, 35, &OptionsEditMode, 0, list, SE_COUNT, false );

	float SliderX = r3dRenderer->ScreenW - 375 ;
	float SliderY = 50;

	if ( imgui_Button( 5, 85, 100, 35, "SAVE MAP", 0, false ) ) 
		SaveLevel( r3dGameLevel::GetHomeDir(), true, false );

	if ( imgui_Button( 110, 85, 100, 35, "SAVE GLOBAL", 0, false ) ) 
		SaveCommonSettings();

	switch( OptionsEditMode )
	{
	case SE_SYSTEM:
		{

			const static char* list[] =		{ "Camera",		"Map",		"Shadows",		"Misc"					};
			enum							{ SS_CAMERA,	SS_MAP,		SS_SHADOWS,		SS_MISC,	SS_COUNT	};

			static int ssIndex = 0 ;
			imgui_Toolbar( SliderX, SliderY, 360, 33, &ssIndex, 0, list, SS_COUNT, false );
			SliderY += 33 ;

			g_pDesktopManager->Begin( "ed_sys_settings" );

			switch( ssIndex )
			{
			case SS_CAMERA:
				{
					float near_plane( 0 ), far_plane( 0 );

					// near plane
					near_plane = r_near_plane->GetFloat();
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Camera Z Near", &near_plane, 0.2f, 32.f, "%.2f", 1 );
					r_near_plane->SetFloat( near_plane );

					// far plane
					far_plane = r_far_plane->GetFloat();
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Camera Z Far", &far_plane, 0.05f, 128000.f, "%.2f", 1 );
					r_far_plane->SetFloat( far_plane );

					SliderY += 30;
				}
				break ;
			case SS_MAP:
				{
					SliderY += imgui_Checkbox(SliderX, SliderY, "HAVE TERRAIN",	    &__HaveTerrain, 1);
					if (__HaveTerrain)
					{
						if (!objTerrain)
							srv_CreateGameObject("obj_Terrain", "Terrain", r3dPoint3D(0,0,0));
					}
					else
						if (objTerrain) GameWorld().DeleteObject(objTerrain);

#if 0
					int tempFixObject = 0;
					SliderY += imgui_Checkbox(SliderX, SliderY, "Fix Level",	    &tempFixObject, 1);
					if(tempFixObject)
					{
						for(GameObject *obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
						{
							if(obj->GetObjectMesh())
							{
								obj->SetPosition(obj->GetPosition()-r3dPoint3D(0,obj->GetBBoxWorld().Size.y*0.5f,0));
							}
						}
						tempFixObject = 0;
					}
#endif

					int DrawPhysicsDebug = d_physx_debug->GetInt();
					extern int DisablePhysXSimulation;
					extern int g_DrawCollisionMeshes;
					SliderY += imgui_Checkbox(SliderX, SliderY, "Draw Collision Meshes",	    &g_DrawCollisionMeshes, 1);
					extern int g_DrawPlayerOnlyCollisionMeshes;
					SliderY += imgui_Checkbox(SliderX, SliderY, "Draw Player Only Collision Meshes",	    &g_DrawPlayerOnlyCollisionMeshes, 1);
					SliderY += imgui_Checkbox(SliderX, SliderY, "Enable Physics Debug",	    &DrawPhysicsDebug, 1);

					d_physx_debug->SetInt(DrawPhysicsDebug);

					static float physx_Range = 0.f;
					physx_Range = d_physx_debug_range->GetFloat();

					SliderY += imgui_Value_Slider(SliderX + 3.f, SliderY, "Debug Range", &physx_Range, 16.f, 1024.f, "%.2f" );

					d_physx_debug_range->SetFloat( physx_Range );

					static float colGeom = 0.f;
					colGeom = e_render_collision_geometry->GetFloat();
					SliderY += imgui_Static( SliderX, SliderY, "Draw Collisions Geometry" );
					SliderY += imgui_Value_Slider( SliderX + 3, SliderY, "Distance", &colGeom, 0.f, 512.f, "%.2f" );
					e_render_collision_geometry->SetInt( colGeom );

					if( DrawPhysicsDebug )
					{
						g_pPhysicsWorld->UpdateDebugBounds();
					}

					SliderY += imgui_Checkbox(SliderX, SliderY, "Disable physics",			&DisablePhysXSimulation, 1);

					static int use_oq;

					use_oq = r_use_oq->GetInt();
					SliderY += imgui_Checkbox(SliderX, SliderY, "Enable Occlusion Queries",	    &use_oq, 1);
					r_use_oq->SetInt( use_oq );


					extern int gDisableNodeRecycling;
					SliderY += imgui_Checkbox(SliderX, SliderY, "[DEBUG] Disable node recycling",	    &gDisableNodeRecycling, 1);
			
					if(gDisableNodeRecycling && DisablePhysXSimulation)
					{
						float moveX = 0, moveZ=0;
						SliderY += imgui_Value_Slider(SliderX, SliderY, "Move Level in X", &moveX, -100000, 100000, "%f", false);
						SliderY += imgui_Value_Slider(SliderX, SliderY, "Move Level in Z", &moveZ, -100000, 100000, "%f", false);
						if(moveX || moveZ)
						{
							for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
							{
								GameObject* obj = iter.current;
								obj->SetPosition(obj->GetPosition() + r3dPoint3D(moveX, 0, moveZ));
							}
						}
					}

					int nTreeVal = g_trees->GetBool() ? 1 : 0;
					SliderY += imgui_Checkbox(SliderX, SliderY, "Enable trees",	&nTreeVal, 1 );
					g_trees->SetBool( nTreeVal );
					int nGrassVal = r_grass_draw->GetBool() ? 1 : 0;
					SliderY += imgui_Checkbox(SliderX, SliderY, "Enable grass",	&nGrassVal, 1 );
					r_grass_draw->SetBool( nGrassVal );

					if( nGrassVal )
					{
						int SplitGrass = r_split_grass_render->GetInt() ? 1 : 0 ;
						SliderY += imgui_Checkbox( SliderX, SliderY, "Split grass render", &SplitGrass, 1 );
						r_split_grass_render->SetInt( SplitGrass );
					}

					extern int g_UseOBBox;
					extern int g_SortByMaterial;

					SliderY += imgui_Checkbox( SliderX, SliderY, "Use OBBox", & g_UseOBBox, 1 );
					SliderY += imgui_Checkbox( SliderX, SliderY, "Sort By Material(free)", &g_SortByMaterial, 1 );

					static int instancedParticles;

					{
						instancedParticles = r_instanced_particles->GetInt();
						SliderY += imgui_Checkbox( SliderX, SliderY, "Instanced particles", &instancedParticles, 1 );

#if 0
						static float GammaPow;

						GammaPow = r_gamma_pow->GetFloat() ;
						SliderY += imgui_Value_Slider( SliderX, SliderY, "Gamma Corr.", &GammaPow, 1.f, 3.f, "%.2f" );
						r_gamma_pow->SetVal( GammaPow );
#endif

						r_instanced_particles->SetInt( instancedParticles ? true : false );
					}
					float dr = r_level_detail_radius->GetFloat();
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Detail radius", &dr, 0, 1000.0f, "%-02.0f", 1);
					r_level_detail_radius->SetFloat(dr);
					if (!Terrain)
					{
						float prevVal = g_NoTerrainLevelBaseHeight;
						SliderY += imgui_Value_Slider(SliderX, SliderY, "Level \"zero\" height", &g_NoTerrainLevelBaseHeight, -10.0f, 200.0f, "%-02.2f");
						if (prevVal != g_NoTerrainLevelBaseHeight)
						{
							g_pGrassMap->UpdateHeight();
						}
					}

					SliderY += 30;

#if R3D_ALLOW_SECTOR_LOADING
					{
						int sectorLoading = !!r_sector_loading->GetInt();
						SliderY += imgui_Checkbox( SliderX, SliderY, "Allow Sector Loading", &sectorLoading, 1 );

						if( sectorLoading && !r_sector_loading->GetInt() )
						{
							// reassing objects to sectors because level editing could have moved some
							// NOTE: should be done before r_sector_loading is set to true, because in case r_sector_loading is true
							// it is considered, that none of the meshes are loaded whereas currently they should be all loaded
							InitSectorMaster();
						}

						r_sector_loading->SetInt( !!sectorLoading );

						if( sectorLoading )
						{
							static SectorMaster::Settings sts;

							sts = g_pSectorMaster->GetSettings();

							SliderY += imgui_Value_Slider( SliderX, SliderY, "Load Radius", &sts.KeepAliveRadius, 256.f, 3072.f, "%.0f" );

							char msg[ 512 ];
							sprintf( msg, "Radius according to QL settings: %.2f", sts.KeepAliveRadius * r_sector_keep_alive_coef->GetFloat() );

							SliderY += imgui_Static( SliderX, SliderY, msg );

							g_pSectorMaster->SetSettings( sts );
						}

						SliderY += 30;
					}
#endif
					
extern float	DayDuration;
extern int		bDaySim;

					SliderY += imgui_Checkbox( SliderX, SliderY, "Simulate Day/Night", &bDaySim, 1 );
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Day Length (SEC )", &DayDuration, 10.0f, 600.0f, "%-02.2f");


				}
				break ;

			case SS_SHADOWS:
				{
					static int optimizeShadowMaps = 0 ;
					
					optimizeShadowMaps = r_optimize_shadow_map->GetInt() ;
					SliderY += imgui_Checkbox( SliderX, SliderY, "Optimize Shadow Maps", &optimizeShadowMaps, 1 );
					r_optimize_shadow_map->SetInt( optimizeShadowMaps ? 1 : 0 );

					int lfsmWrapMode = r_lfsm_wrap_mode->GetInt() ;
					int prevLfsmWrapMode = lfsmWrapMode ;
					SliderY += imgui_Checkbox( SliderX, SliderY, "Cache Last Shadow Slice", &lfsmWrapMode, 1 );
					r_lfsm_wrap_mode->SetInt( lfsmWrapMode ? 1 : 0 );

					if( r_lfsm_wrap_mode->GetInt() )
					{
						if( !prevLfsmWrapMode )
							ResetShadowCache();

						r_lfsm_cache_dist->SetInt( 8.f ) ;
					}
					else
					{
						r_lfsm_cache_dist->SetInt( 0.f ) ;
					}

					static int showShadowScheme = 0 ;

					showShadowScheme = !!r_show_shadow_scheme->GetInt() ;
					SliderY += imgui_Checkbox( SliderX, SliderY, "Show Shadow Scheme", &showShadowScheme, 1 );

					if( showShadowScheme )
					{
						// otherwise preserve value ( may be 2 )
						if( !r_show_shadow_scheme->GetInt() )
						{
							r_show_shadow_scheme->SetInt( 1 );
						}
					}
					else
					{
						r_show_shadow_scheme->SetInt( 0 ) ;
					}

					if( r_transp_shadows->GetInt() && r_show_shadow_scheme->GetInt() )
					{
						stringlist_t shadowSchemeNames ;

						shadowSchemeNames.push_back( "Opaque scheme" ) ;
						shadowSchemeNames.push_back( "Transparent scheme" ) ;

						static float offset = 0;
						static int index = 0;

						const int HEIGHT = 55 ;
						
						imgui_DrawList( SliderX + 22, SliderY, 360 - 22, HEIGHT, shadowSchemeNames, &offset, &index );

						if( index == 1 )
							r_show_shadow_scheme->SetInt( 2 ) ;
						else
							r_show_shadow_scheme->SetInt( 1 ) ;

						SliderY += HEIGHT ;
					}

					SliderY += imgui_Value_Slider(SliderX, SliderY, "World Scale", &__WorldScale,	1,100,	"%-02.0f",1);
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Shadow Size", &ShadowDrawingDistance, 1,250*__WorldScale, "%-02.0f",1);
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Second slice begin", &ShadowSplitDistancesOpaqueHigh[1], 1,ShadowDrawingDistance, "%-02.1f",1);
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Third slice begin", &ShadowSplitDistancesOpaqueHigh[2], 1,ShadowDrawingDistance, "%-02.1f",1);


					/*
					REG_VAR( r_shadow_use_normals, true,			0 );
					REG_VAR( r_shadow_normals_offset, 5.0f,			0 );

					*/

					static int useNormals = r_shadow_use_normals->GetInt();
					SliderY += imgui_Checkbox( SliderX, SliderY, "Use normals", &useNormals, 1 );
					r_shadow_use_normals->SetInt(useNormals);
			
					if (useNormals)
					{
						static float normalsOffset = r_shadow_normals_offset->GetFloat();
						SliderY += imgui_Value_Slider(SliderX, SliderY, "Normals offset", &normalsOffset, -5, 5, "%-02.3f",1);
						r_shadow_normals_offset->SetFloat(normalsOffset);
					}


					float prevShadowSunOffset = ShadowSunOffset;
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Shadow Sun Offset", &ShadowSunOffset, 1,200.0f*__WorldScale, "%-02.0f",1);

					if( prevShadowSunOffset != ShadowSunOffset )
					{
						ResetShadowCache();
					}

					static float shadow_coef = r_shadow_low_size_coef->GetFloat() ;
					SliderY += imgui_Value_Slider(SliderX, SliderY, "LQ Shadow Dist K",		&shadow_coef,						0.5f, 1.f,	"%-02.2f", 1 );

					if( fabs( shadow_coef - r_shadow_low_size_coef->GetFloat() ) > FLT_EPSILON )
					{
						ResetShadowCache();
						r_shadow_low_size_coef->SetVal( shadow_coef ) ;
					}
					
					UpdateLowShadowSplitDistances();

					char StrS1[256];
					SliderY += imgui_Static( SliderX, SliderY, "ULTRA (SOFT) SHADOW BIAS" );
					for(int i=0; i<NumShadowSlices; ++i)
					{
						sprintf(StrS1, "Slice %d bias", i+1);
						SliderY += imgui_Value_Slider(SliderX, SliderY, StrS1, &ShadowSlices[i].depthBias, 0,0.00025f,		"%-02.8f",1);
					}

					SliderY += imgui_Static( SliderX, SliderY, "HARDWARE SHADOW BIAS ( MED-HIGH )" );
					for(int i=0; i<NumShadowSlices; ++i)
					{
						sprintf(StrS1,"Slice %d bias", i+1);
						SliderY += imgui_Value_Slider(SliderX, SliderY, StrS1, &ShadowSlices[i].depthBias_HW, 0,0.00025f,	"%-02.8f",1);
					}

					//------------------------------------------------------------------------

					SliderY += imgui_Static( SliderX, SliderY, "MINIMUM SHADOW OBJECT SIZES" );

					static float slice0_min_size = r_shadow_slice0_min_size->GetFloat();
					static float slice1_min_size = r_shadow_slice1_min_size->GetFloat();
					static float slice2_min_size = r_shadow_slice2_min_size->GetFloat();

					SliderY += imgui_Value_Slider( SliderX, SliderY, "Slice 0", &slice0_min_size, 0.f, 32.f, "%.2f" );
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Slice 1", &slice1_min_size, 0.f, 32.f, "%.2f" );
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Slice 2", &slice2_min_size, 0.f, 32.f, "%.2f" );

					r_shadow_slice0_min_size->SetFloat( slice0_min_size );
					r_shadow_slice1_min_size->SetFloat( slice1_min_size );
					r_shadow_slice2_min_size->SetFloat( slice2_min_size );

					char shadowDipsMsg[ 512 ];
					sprintf( shadowDipsMsg, "Shadow dips: %d\n", r3dRenderer->Stats.GetNumShadowDips() );
					SliderY += imgui_Static( SliderX, SliderY, shadowDipsMsg );

					//------------------------------------------------------------------------

					if( Terrain3 )
					{
						r3dTerrain3::Settings sts = Terrain3->GetSettings();

						static float shadowOffset = 0.f;

						shadowOffset = sts.ShadowOffset;

						SliderY += imgui_Value_Slider(SliderX, SliderY, "Terrain Bias", &shadowOffset, 0, 0.0075f, "%-02.6f", 1 );

						if( sts.ShadowOffset != shadowOffset )
						{
							ResetShadowCache();

							sts.ShadowOffset = shadowOffset;
							Terrain3->SetSettings( sts );
						}
					}

					int FXAABlur = r_fxaa_shadow_blur->GetBool();
					SliderY += imgui_Checkbox( SliderX, SliderY, "FXAA Shadow Blur", &FXAABlur, 1 );
					r_fxaa_shadow_blur->SetBool(!!FXAABlur);

					int DoShadowBlur = r_shadow_blur->GetBool();
					SliderY += imgui_Checkbox( SliderX, SliderY, "Shadow Blur", &DoShadowBlur, 1 );
					r_shadow_blur->SetBool( DoShadowBlur ? true : false );

					if( r_shadow_blur->GetBool() )
					{
						int DoDynShadowBlur = r_do_dyn_shadow_blur->GetBool();
						SliderY += imgui_Checkbox( SliderX + 5, SliderY, "Use Dyn. Branching", &DoDynShadowBlur, 1 );
						r_do_dyn_shadow_blur->SetBool( DoDynShadowBlur ? true : false );

						float SB_Bias = r_shadows_blur_bias->GetFloat();
						SliderY += imgui_Value_Slider( SliderX + 5, SliderY, "Blur Bias", &SB_Bias, 0.0f, 0.5f, "%-02.2f", 1 );
						r_shadows_blur_bias->SetFloat( SB_Bias );

						float SB_PhysRange = r_shadows_blur_phys_range->GetFloat();
						SliderY += imgui_Value_Slider( SliderX + 5, SliderY, "Physicality", &SB_PhysRange, 0.0f, 1024.0f, "%-02.2f", 1 );
						r_shadows_blur_phys_range->SetFloat( SB_PhysRange );

						float SB_Sense = r_shadows_blur_sense->GetFloat();
						SliderY += imgui_Value_Slider( SliderX + 5, SliderY, "Depth Sens.", &SB_Sense, 0.0f, 1024.0f, "%-02.2f", 1 );
						r_shadows_blur_sense->SetFloat( SB_Sense );

						float SB_Radius = r_shadows_blur_radius->GetFloat();
						SliderY += imgui_Value_Slider( SliderX + 5, SliderY, "Radius", &SB_Radius, 0.0f, 12.0f, "%-02.2f", 1 );
						r_shadows_blur_radius->SetFloat( SB_Radius );
					}

					float shadowExtusionLimitHeight = r_shadow_extrusion_limit->GetFloat();
					float prevLimit = shadowExtusionLimitHeight;
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Shadow extrusion limit", &shadowExtusionLimitHeight, -100.0f, 300.0f, "%-02.2f", 1);
					r_shadow_extrusion_limit->SetFloat(shadowExtusionLimitHeight);
					if (prevLimit != shadowExtusionLimitHeight)
					{
						for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
						{
							GameObject* obj = iter.current;
							if (obj) obj->ShadowExDirty = true;
						}
					}

					static int showShadowExtrusionLimitingHeight = 0;
					SliderY += imgui_Checkbox(SliderX, SliderY, "Show shadow extrusion limit", &showShadowExtrusionLimitingHeight, 1);
					if (showShadowExtrusionLimitingHeight)
					{
						PushUniformGrid(r3dPoint3D(gCam.x, shadowExtusionLimitHeight, gCam.z), 1000.0f, 1000.0f, r3dColor::green);
					}

					SliderY += imgui_Static( SliderX, SliderY, " " ) ;
					SliderY += imgui_Static( SliderX, SliderY, "Transparent Shadows" ) ;

					if( r_shadows_quality->GetInt() <= 2 )
					{
						SliderY += imgui_Static( SliderX, SliderY, "!!!!!" ) ;
						SliderY += imgui_Static( SliderX, SliderY, "Set Shadow quality above medium" ) ;
						SliderY += imgui_Static( SliderX, SliderY, "To setup transparent shadows" ) ;
						SliderY += imgui_Static( SliderX, SliderY, "!!!!!" ) ;
					}
					else
					{

						int transpShadsEnabled = !!r_transp_shadows->GetInt() ;
						SliderY += imgui_Checkbox( SliderX, SliderY, "Enable Transparent Shadows", &transpShadsEnabled, 1 );
						r_transp_shadows->SetInt( transpShadsEnabled ) ;

						static float transpShadDist = 0.f ;
						transpShadDist = r_transp_shadow_dist->GetFloat() ;
						SliderY += imgui_Value_Slider( SliderX, SliderY, "Shadowmap distance", &transpShadDist, 16.f, 256.f, "%.2f" );
						r_transp_shadow_dist->SetFloat( transpShadDist ) ;

						static float masterTransp = 0.f ;
						masterTransp = r_transp_shadow_coef->GetFloat() ;
						SliderY += imgui_Value_Slider( SliderX, SliderY, "Master transparency", &masterTransp, 0.f, 1.f, "%.2f" );
						r_transp_shadow_coef->SetFloat( masterTransp ) ;

						int forceParticleCast = !!r_force_parts_transp_shad->GetInt() ;
						SliderY += imgui_Checkbox( SliderX, SliderY, "Force particles cast shadows", &forceParticleCast, 1 );					
						r_force_parts_transp_shad->SetInt( forceParticleCast ) ;
					}
					
					SliderY += 5.0f;
					if (imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Regenerate all static shadows"))
					{
						WorldLightSystem.RegenAllFrozenLightShadows();
					}
					SliderY += DEFAULT_CONTROLS_HEIGHT;

				}
				break;


			case SS_MISC:
				{
					//------------------------------------------------------------------------
					// distance cull setting
					{
						static float val = 0.f;

						val = r_default_draw_distance->GetFloat();

						SliderY += imgui_Static( SliderX, SliderY, "Distance object culling" );
						SliderY += imgui_Value_Slider( SliderX, SliderY, "Default Distance", &val, 0.f, 4096.f, "%.0f" );

						r_default_draw_distance->SetFloat( val );
					}

					//------------------------------------------------------------------------
					// z prepass settings
					{
						SliderY += imgui_Static(SliderX, SliderY, "Depth prepass");

						const int PREPASS_LIST_HEIGHT = 110;

						stringlist_t methodNames;

						methodNames.push_back( "NONE" );
						methodNames.push_back( "DISTANCE" );
						methodNames.push_back( "AREA" );

						static int prepassMethod = 0;
						static float offset = 0.f;

						prepassMethod = r_z_prepass_method->GetInt() ;

						if ( imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, PREPASS_LIST_HEIGHT, methodNames, &offset, &prepassMethod ) )
						{
							if( prepassMethod >= 0 && r_z_prepass_method->GetInt() <= Z_PREPASS_METHOD_AREA )
							{
								r_z_prepass_method->SetInt( prepassMethod ) ;
							}
						}

						SliderY += PREPASS_LIST_HEIGHT ;

						static float distValue = 0.f, areaValue = 0.f ;

						distValue = r_z_prepass_dist->GetFloat() ;
						areaValue = r_z_prepass_area->GetFloat() ;

						switch( prepassMethod )
						{
						case Z_PREPASS_METHOD_NONE:
							break ;
						case Z_PREPASS_METHOD_DIST:
							SliderY += imgui_Value_Slider( SliderX, SliderY, "Distance", &distValue, 0.f, 300.f, "%.1f" ) ;
							break ;
						case Z_PREPASS_METHOD_AREA:
							SliderY += imgui_Value_Slider( SliderX, SliderY, "Area", &areaValue, 0.f, 0.5f, "%.3f"  ) ;
							break ;
						}

						r_z_prepass_dist->SetFloat( distValue ) ;
						r_z_prepass_area->SetFloat( areaValue ) ;

						if( prepassMethod != Z_PREPASS_METHOD_NONE )
						{
							int highlightCasters = !!r_highlight_prepass->GetInt() ;
							SliderY += imgui_Checkbox( SliderX, SliderY, 360.f, 22.f, "Highlight Z Prepass", &highlightCasters, 1 )  ;
							r_highlight_prepass->SetInt( highlightCasters ) ;
						}

					}

					//------------------------------------------------------------------------

					SliderY += 22;
					SliderY += imgui_Static(SliderX, SliderY, "Mouse Smoothing");
					{
						extern int g_mouseSmoothingNumFrames;
						SliderY += imgui_Value_SliderI(SliderX+5, SliderY, "Num Frames",		&g_mouseSmoothingNumFrames,	0,31, "%d");
						extern float g_mouseSmoothingWeight;
						SliderY += imgui_Value_Slider(SliderX+5, SliderY, "Weight",		&g_mouseSmoothingWeight, 0.0f,1.0f, "%.3f");
					}

					extern float __WorldRenderBias;
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Render Bias",			&__WorldRenderBias,		-6.0f,6.0f,		"%-02.2f",1);

					// per shot
					extern float SpreadIncrease[];
					extern float SpreadDecrease[];
					extern float SpreadMin[];

					SliderY += imgui_Value_Slider(SliderX, SliderY, "Spread Inc",			&SpreadIncrease[0],		0.0f,1.0f,		"%-02.2f",1);
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Spread Dec",			&SpreadDecrease[0],		0.0f,3.0f,		"%-02.2f",1);
					SliderY += imgui_Value_Slider(SliderX, SliderY, "Spread Min",			&SpreadMin[0],			0.0f,5.0f,		"%-02.2f",1);

					extern int	__r3dDisplayMipLevels;
					SliderY += imgui_Checkbox(SliderX, SliderY, "Display Mip Levels",	&__r3dDisplayMipLevels, 1 );
					int texDensityMode1 = d_show_checker_texture->GetInt();
					SliderY += imgui_Checkbox(SliderX, SliderY, "Fill with checker tex",	&texDensityMode1, 1 );
					d_show_checker_texture->SetInt(texDensityMode1);
					texDensityMode1 = d_visualize_tex_density->GetInt();
					SliderY += imgui_Checkbox(SliderX, SliderY, "Visualize texel density (texels per unit)",	&texDensityMode1, 1 );
					d_visualize_tex_density->SetInt(texDensityMode1);

					extern D3DXVECTOR4 gMinTexDensityParams;
					extern D3DXVECTOR4 gOptTexDensityParams;
					extern D3DXVECTOR4 gMaxTexDensityParams;

					if (texDensityMode1)
					{
						SliderY += imgui_Value_Slider(SliderX + 5, SliderY, "Min (blue)", &gMinTexDensityParams.w, 0.0f, 1000.0f, "%-02.2f");
						if (gMinTexDensityParams.w > gMaxTexDensityParams.w)
							gMaxTexDensityParams.w = gMinTexDensityParams.w;

						SliderY += imgui_Value_Slider(SliderX + 5, SliderY, "Optimal (green)", &gOptTexDensityParams.w, gMinTexDensityParams.w, gMaxTexDensityParams.w, "%-02.2f");

						SliderY += imgui_Value_Slider(SliderX + 5, SliderY, "Max (red)", &gMaxTexDensityParams.w, 0.0f, 1000.0f, "%-02.2f");
						if (gMinTexDensityParams.w > gMaxTexDensityParams.w)
							gMinTexDensityParams.w = gMaxTexDensityParams.w;
					}

					static int S;
					S = r_show_light_helper->GetInt();
					SliderY += imgui_Checkbox(SliderX, SliderY, "Display LightHelpers",	&S, 1 );
					r_show_light_helper->SetInt( S );

					static int DepthScr = 0;
					DepthScr = r_allow_depth_screenshot->GetInt();
					SliderY += imgui_Checkbox(SliderX, SliderY, "Allow Depth Screenshots",	&DepthScr, 1 );
					r_allow_depth_screenshot->SetInt( DepthScr );

					if( DepthScr )
					{
						static float DepthStart = r_depth_screenshot_start->GetFloat() ;
						static float DepthEnd = r_depth_screenshot_end->GetFloat() ;

						SliderY += imgui_Value_Slider(SliderX, SliderY, "Depth Start", &DepthStart, 0.0f, 1024.0f, "%-02.2f", 1 );
						SliderY += imgui_Value_Slider(SliderX, SliderY, "Depth End", &DepthEnd, 0.0f, 16000.0f, "%-02.2f", 1 );

						if( DepthStart >= DepthEnd )
						{
							DepthEnd = DepthStart + 1.f;
						}

						r_depth_screenshot_start->SetFloat( DepthStart ) ;
						r_depth_screenshot_end->SetFloat( DepthEnd ) ;
					}

					//------------------------------------------------------------------------
					// dynamic envmaps
					int dynaEnvMaps = !!r_dynamic_envmap_probes->GetInt();
					SliderY += imgui_Checkbox( SliderX, SliderY, "Dynamic EnvMap", &dynaEnvMaps, 1 );
					r_dynamic_envmap_probes->SetInt( !!dynaEnvMaps );

				}
				break ;

			}

			g_pDesktopManager->End();
		}
		break;

	case SE_OPTIONS_MENU:
		{
			g_pDesktopManager->Begin( "ed_menu_options" );

			const int SSAO_LIST_HEIGHT = 110;

			stringlist_t ssaoNames;

			ssaoNames.push_back( "LOW" );
			ssaoNames.push_back( "MEDIUM" );
			ssaoNames.push_back( "HIGH" );
			ssaoNames.push_back( "ULTRA" );
			ssaoNames.push_back( "CUSTOM" );

			static int OverallQuality;
			static float offset = 0.f;

			OverallQuality = r_overall_quality->GetInt() - 1;

			void applyGraphicsOptions( uint32_t );

			if ( imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, SSAO_LIST_HEIGHT, ssaoNames, &offset, &OverallQuality ) )
			{
				r_overall_quality->SetInt( OverallQuality + 1 );
				if( OverallQuality <= S_ULTRA )
				{
					applyGraphicsOptions( SetDefaultSettings( r3dDevStrength( OverallQuality ) ) );
				}
			}

			SliderY += SSAO_LIST_HEIGHT;

			if( OverallQuality == 4 )
			{
				static GraphicSettings settings;

				FillSettingsFromVars( settings );

				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Mesh qlt",			&settings.mesh_quality,			(int)r_mesh_quality->GetMinVal(),			(int)r_mesh_quality->GetMaxVal()		, "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Texture qlt",		&settings.texture_quality,		(int)r_texture_quality->GetMinVal(),		(int)r_texture_quality->GetMaxVal()		, "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Terrain qlt",		&settings.terrain_quality,		(int)r_terrain_quality->GetMinVal(),		(int)r_terrain_quality->GetMaxVal()		, "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Shadows qlt",		&settings.shadows_quality,		(int)r_shadows_quality->GetMinVal(),		(int)r_shadows_quality->GetMaxVal()		, "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Lighting qlt",		&settings.lighting_quality,		(int)r_lighting_quality->GetMinVal(),		(int)r_lighting_quality->GetMaxVal()	, "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Environment qlt",	&settings.environment_quality,	(int)r_environment_quality->GetMinVal(),	(int)r_environment_quality->GetMaxVal()	, "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Antialiasing qlt",	&settings.antialiasing_quality,	(int)r_antialiasing_quality->GetMinVal(),	(int)r_antialiasing_quality->GetMaxVal(), "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Anisotropy qlt",		&settings.anisotropy_quality,	(int)r_anisotropy_quality->GetMinVal(),		(int)r_anisotropy_quality->GetMaxVal()	, "%d" );
				SliderY += imgui_Value_SliderI( SliderX, SliderY, "Postprocess qlt",	&settings.postprocess_quality,	(int)r_postprocess_quality->GetMinVal(),	(int)r_postprocess_quality->GetMaxVal()	, "%d" );
				if (r_lighting_quality->GetInt() > 1)
					SliderY += imgui_Value_SliderI( SliderX, SliderY, "Ssao qlt",		&settings.ssao_quality,			(int)r_ssao_quality->GetMinVal(),			(int)r_ssao_quality->GetMaxVal()		, "%d" );

				DWORD flags = GraphSettingsToVars( settings );

				// do not reload textures, offset bias instead ( for artist estimation only )
				applyGraphicsOptions( flags );
			}

			SliderY += imgui_Checkbox(SliderX, SliderY, "Force object quality", &g_bForceQualitySelectionInEditor, 1);

			if( imgui_Button( SliderX + 245, SliderY, 110, 35, "Save Settings", 0, false ) )
			{
				void writeGameOptionsFile();
				writeGameOptionsFile();
			}
			SliderY += 35;

			g_pDesktopManager->End();
		}
		break;
	}



}

static int g_NeedPrintThisAmountOfLoadingTiles;
static int g_NeedPrintThisAmountOfBkTasks;
int g_NeedLoadingTilesSign = 0;
int g_NeedLoadingGrassSign = 0;

bool Editor_Level :: ProcessTerrain3_EditingAllowed( int leftButtonOnly )
{
	if( imgui_lbp || !leftButtonOnly )
	{
		g_NeedPrintThisAmountOfLoadingTiles = Terrain3->GetNumLoadingMegaTiles();
		g_NeedPrintThisAmountOfBkTasks = g_pBackgroundTaskDispatcher->GetTaskCount( r3dBackgroundTaskDispatcher::TASK_CLASS_MEGATEXTURE );

		if( g_NeedPrintThisAmountOfLoadingTiles )
		{
			g_NeedLoadingTilesSign = 1;
			return false;
		}

		if( !imgui_IsCursorOver2d() && !imgui_WasCursorOver2d() && Keyboard->IsPressed( kbsLeftControl ) )
		{
			const r3dTerrainDesc& desc = Terrain3->GetDesc();

			if( UI_TerraTargetPos.x >= 0.f && UI_TerraTargetPos.x <= desc.XSize 
					&&
				UI_TerraTargetPos.z >= 0.f && UI_TerraTargetPos.z <= desc.ZSize 
				)
			{
				if( !Terrain3->IsPosWithinPreciseHeights( UI_TerraTargetPos ) )
					return false;
			}
		}
	}

	if( g_NeedPrintThisAmountOfLoadingTiles )
	{
		g_NeedPrintThisAmountOfLoadingTiles = Terrain3->GetNumLoadingMegaTiles();
	}

	return true;
}

float
Editor_Level::ProcessTerrain2_Settings( float SliderX, float SliderY )
{
#if 0
	if( r_terrain_quality->GetInt() < 3
			||
		r_texture_quality->GetInt() < 3
		)
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) ) ;
		imgui_Static( r3dRenderer->ScreenW2 - 350, r3dRenderer->ScreenH2, "Please set Terrain Quality and Texture Quality to 3 in Game Options to modify Terrain Settings",700, false, 22, true ) ;
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) ) ;
		return SliderY ;
	}
#endif

	int allowTerrain2 = !!r_terrain2->GetInt() ;

	SliderY += imgui_Checkbox( SliderX, SliderY, "Allow Terrain2", &allowTerrain2, 1 ) ;

	if( !allowTerrain2 && r_terrain2->GetInt() )
	{
		Terrain2->ReleaseRoads() ;
	}

	if( allowTerrain2 && !r_terrain2->GetInt() )
	{
		for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->ObjTypeFlags & OBJTYPE_Road )
			{
				obj->ObjFlags |= OBJFLAG_SkipDraw ;
			}
		}

		Terrain = Terrain2 ;
	}

	r_terrain2->SetInt( !!allowTerrain2 ) ;

	if( allowTerrain2 )
	{
		static r3dTerrain2::Settings settings ;

		settings = Terrain2->GetSettings() ;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Specular", &settings.Specular, 0.f, 1.f, "%.3f" ) ;

		Terrain2->SetSettings( settings ) ;

		char buf[ 512 ] ;

		sprintf( buf, "Max Card Anisotropy: %d", r3dRenderer->d3dCaps.MaxAnisotropy  ) ;
		SliderY += imgui_Static( SliderX, SliderY, buf ) ;

		static float maxAnisotropy = 0 ;
		maxAnisotropy = (float)r_terrain2_anisotropy->GetInt() ;
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Anisotropy", &maxAnisotropy, 0.f, 16.f, "%.0f" ) ;
		r_terrain2_anisotropy->SetInt( maxAnisotropy ) ;

		static float padding = 0.f ;
		padding = r_terrain2_padding->GetInt() ;
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Padding", &padding, 0.f, 32.f, "%.0f" ) ;

		if( (int)padding != r_terrain2_padding->GetInt() )
		{
			Terrain2->RefreshAtlasTiles() ;
		}

		r_terrain2_padding->SetInt( (int) padding ) ;

		//------------------------------------------------------------------------
		// quality settings
		{
			stringlist_t qualityList ;

			qualityList.push_back( "Low" ) ;
			qualityList.push_back( "Medium" ) ;
			qualityList.push_back( "High" ) ;

			static float qlListOffset = 0.f ;
			static int editQl = 0 ;

			SliderY += imgui_Static( SliderX, SliderY, "Edit Terrain Quality Level" ) ;

			float QL_LIST_HEIGHT = 90.0f ;

			editQl = r_terrain_quality->GetInt() - 1 ;

			imgui_DrawList( SliderX, SliderY, 360, QL_LIST_HEIGHT, qualityList, &qlListOffset, &editQl ) ;

			r_terrain_quality->SetInt( editQl + 1 ) ;
			SliderY += QL_LIST_HEIGHT ;


			r3dTerrain2::QualitySettings qsts = Terrain2->GetBaseQualitySettings( editQl ) ;

			SliderY += imgui_Static( SliderX, SliderY, "Tile Resolution" ) ;

			stringlist_t strList ;
			strList.push_back( "64x64" ) ;
			strList.push_back( "128x128" ) ;
			strList.push_back( "256x256" ) ;
			strList.push_back( "512x512" ) ;

			static float offset = 0.f ;

			int selected = 1 ;
			switch( qsts.AtlasTileDim )
			{
			case 64:
				selected = 0 ;
				break ;
			case 128:
				selected = 1 ;
				break ;
			case 256:
				selected = 2 ;
				break ;
			case 512:
				selected = 3 ;
				break ;
			}

			const float LIST_HEIGHT = 110.f ;

			imgui_DrawList( SliderX, SliderY, 360.f, LIST_HEIGHT, strList, &offset, &selected ) ;

			SliderY += LIST_HEIGHT ;

			switch( selected )
			{
			case 0:
				qsts.AtlasTileDim = 64 ;
				break ;
			case 1:
				qsts.AtlasTileDim = 128 ;
				break ;
			case 2:
				qsts.AtlasTileDim = 256 ;
				break ;
			case 3:
				qsts.AtlasTileDim = 512 ;
				break ;
			}

			static float offset1 = 0.f ;

			SliderY += imgui_Static( SliderX, SliderY, "Tile Vertex Size" ) ;

			strList.clear() ;

			strList.push_back( "1" ) ;
			strList.push_back( "2" ) ;
			strList.push_back( "4" ) ;
			strList.push_back( "8" ) ;

			selected = 0 ;

			switch( qsts.VertexTileDim )
			{
			case 1:
				selected = 0 ;
				break ;
			case 2:
				selected = 1 ;
				break ;
			case 4:
				selected = 2 ;
				break ;
			case 8:
				selected = 3 ;
				break ;
			}

			imgui_DrawList( SliderX, SliderY, 360.f, LIST_HEIGHT, strList, &offset1, &selected ) ;

			SliderY += LIST_HEIGHT ;

			switch( selected )
			{
			case 0:
				qsts.VertexTileDim = 1 ;
				break ;
			case 1:
				qsts.VertexTileDim = 2 ;
				break ;
			case 2:
				qsts.VertexTileDim = 4 ;
				break ;
			case 3:
				qsts.VertexTileDim = 8 ;
				break ;
			}

			// if this fails make MIN_VERTEX_TILE_DIM less ( 1? ; )
			r3d_assert( qsts.VertexTileDim >= r3dTerrain2::QualitySettings::MIN_VERTEX_TILE_DIM ) ;

			static float lodCounts[ r3dTerrain2::NUM_QUALITY_LAYERS ] ;

			for( int i = 0, e = r3dTerrain2::NUM_QUALITY_LAYERS ; i < e ; i ++ )
			{
				lodCounts[ i ] = qsts.TileCounts[ i ] / 2 ;

				char name[ 64 ] ;

				sprintf( name, "Tex Lod %d count", i ) ;

				SliderY += imgui_Value_Slider( SliderX, SliderY, name, &lodCounts[ i ], 1.f, 8.f, "%.0f" ) ;

				qsts.TileCounts[ i ] = int( lodCounts[ i ] ) * 2 ;
			}

#if 0
			static float geomLodSteps[ r3dTerrain2::QualitySettings::NUM_DENSITY_STEPS ] ;

			for( int i = 0, e = r3dTerrain2::QualitySettings::NUM_DENSITY_STEPS ; i < e ; i ++ )
			{
				geomLodSteps[ i ] = qsts.TileVertexDensitySteps[ i ] ;

				char name[ 64 ] ;

				sprintf( name, "Geom Lod %d", i ) ;

				SliderY += imgui_Value_Slider( SliderX, SliderY, name, &geomLodSteps[ i ], 8.f, 128.f, "%.0f" ) ;

				qsts.TileVertexDensitySteps[ i ] = geomLodSteps[ i ] ;
			}
#else
			{
				static float offset1 = 0.f ;

				SliderY += 11 ;

				SliderY += imgui_Static( SliderX, SliderY, "Vertex Density" ) ;

				strList.clear() ;

				strList.push_back( "1" ) ;
				strList.push_back( "2" ) ;
				strList.push_back( "4" ) ;
				strList.push_back( "8" ) ;
				strList.push_back( "16" ) ;
				strList.push_back( "32" ) ;
				strList.push_back( "64" ) ;
				strList.push_back( "128" ) ;

				selected = 0 ;

				switch( qsts.VertexDensity )
				{
				case 1:
					selected = 0 ;
					break ;
				case 2:
					selected = 1 ;
					break ;
				case 4:
					selected = 2 ;
					break ;
				case 8:
					selected = 3 ;
					break ;
				case 16:
					selected = 4 ;
					break ;
				case 32:
					selected = 5 ;
					break ;
				case 64:
					selected = 6 ;
					break ;
				case 128:
					selected = 7 ;
					break ;
				}

				const int DENS_LIST_HEIGHT = 180 ;

				imgui_DrawList( SliderX, SliderY, 360.f, DENS_LIST_HEIGHT, strList, &offset1, &selected ) ;

				SliderY += DENS_LIST_HEIGHT ;

				selected = R3D_MIN( R3D_MAX( selected, 0 ), 7 ) ;

				qsts.VertexDensity = 1 << selected ;
			}

#endif

			Terrain2->SetQualitySettings( qsts, true, editQl ) ;
		}

		static int showStats = 0 ;

		SliderY += imgui_Checkbox( SliderX, SliderY, "Show Stats", &showStats, 1 ) ;

		if( showStats )
		{
			r3dTerrainStats stats ;
			Terrain2->GetStats( &stats ) ;

			char buf[ 512 ] ;

			sprintf( buf, "Volumes: %d", stats.VolumeCount ) ;
			SliderY += imgui_Static( SliderX, SliderY, buf );

			sprintf( buf, "Tiles: %d", stats.TileCount ) ;
			SliderY += imgui_Static( SliderX, SliderY, buf );

			float toMegs = 1.0f / 1024.f / 1024.f ;

			sprintf( buf, "Tile VMem: %.2f MB", stats.TileTextureMemory * toMegs ) ;
			SliderY += imgui_Static( SliderX, SliderY, buf );

			sprintf( buf, "Mask VMem: %.2f MB", stats.MaskTextureMemory * toMegs ) ;
			SliderY += imgui_Static( SliderX, SliderY, buf );

			sprintf( buf, "Layer VMem: %.2f MB", stats.LayerTextureMemory * toMegs ) ;
			SliderY += imgui_Static( SliderX, SliderY, buf );

			int total = stats.LayerTextureMemory + stats.MaskTextureMemory + stats.TileTextureMemory ;

			sprintf( buf, "Total VMem: %.2f MB", total * toMegs ) ;
			SliderY += imgui_Static( SliderX, SliderY, buf );

		}

		int showTiles = !!r_terrain2_show_tiles->GetInt() ;

		SliderY += imgui_Checkbox( SliderX, SliderY, "Show Tiles", &showTiles, 1 ) ;

		r_terrain2_show_tiles->SetInt( !!showTiles ) ;

		int showAtlas = r_terrain2_show_atlas->GetInt() ;

		SliderY += imgui_Checkbox( SliderX, SliderY, "Show Atlas", &showAtlas, 1 ) ;

		r_terrain2_show_atlas->SetInt( showAtlas ) ;

		if( showAtlas )
		{
			static float scale = 1.0f ;

			scale = r_terrain2_show_atlas_scale->GetFloat() ;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Atlas Scale", &scale, 1.0f, 32.f, "%.2f" ) ;
			r_terrain2_show_atlas_scale->SetFloat( scale ) ;

			static float volume = 0.0f ;

			r3dTerrainStats stats ;
			Terrain2->GetStats( &stats ) ;

			volume = r_terrain2_show_atlas_volume->GetFloat() ;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Atlas Volume", &volume, 0.f, stats.VolumeCount - 1, "%.0f" ) ;
			r_terrain2_show_atlas_volume->SetFloat( volume ) ;

			static float off_x = 0.f ;

			off_x = r_terrain2_show_atlas_off_x->GetFloat() ;
			SliderY += imgui_Value_Slider(	SliderX, SliderY, "Offset X", &off_x,
											-r3dTerrain2::ATLAS_TEXTURE_DIM / 2,
											+r3dTerrain2::ATLAS_TEXTURE_DIM / 2,
											"%.0f" ) ;

			r_terrain2_show_atlas_off_x->SetFloat( off_x ) ;

			static float off_y = 0.f ;

			off_y = r_terrain2_show_atlas_off_y->GetFloat() ;
			SliderY += imgui_Value_Slider(	SliderX, SliderY, "Offset Y", &off_y,
											-r3dTerrain2::ATLAS_TEXTURE_DIM / 2,
											+r3dTerrain2::ATLAS_TEXTURE_DIM / 2,
											"%.0f" ) ;

			r_terrain2_show_atlas_off_y->SetFloat( off_y ) ;

			static float opacity = 0.f ;

			opacity = r_terrain2_show_atlas_oppa->GetFloat() ;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Atlas Opacity", &opacity, 0.f, 1.f, "%.2f" ) ;
			r_terrain2_show_atlas_oppa->SetFloat( opacity ) ;
		}

		{
			SliderY += imgui_Static( SliderX, SliderY, "Resize terrain:" ) ;

			stringlist_t sizes ;

			int curPow = 0 ;

			for( int n = Terrain2->GetDesc().CellCountX ; n ; n /= 2 )
			{
				curPow ++ ;
			}

			const int RESIZE_OFFSET = 9 ;

			static int resizeTo = curPow - RESIZE_OFFSET ;

			sizes.push_back( "256" ) ;
			sizes.push_back( "512" ) ;
			sizes.push_back( "1024" ) ;
			sizes.push_back( "2048" ) ;
			sizes.push_back( "4096" ) ;

			static float offset = 0.f ;

			const int RESIZE_HEIGHT = 133 ;

			imgui_DrawList( SliderX, SliderY, 360, RESIZE_HEIGHT, sizes, &offset, &resizeTo ) ;
			SliderY += RESIZE_HEIGHT ;

			if( imgui_Button( SliderX, SliderY, 360, 22, "Resize" ) )
			{

				if( MessageBox( r3dRenderer->HLibWin, "Resize terrain?", "Confirm", MB_YESNO ) == IDYES )
				{
					int newSize = 1 << ( resizeTo + RESIZE_OFFSET - 1 ) ;
					Terrain2->Resize( newSize, newSize ) ;
				}
			}
		}
	}

	return SliderY ;
}

float
Editor_Level::ProcessTerrain2_UpDown( float SliderX, float SliderY, int editMode, bool up )
{
	const int EDIT_MODE_SEL = up ? TE2_UP : TE2_DOWN ;

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, up ? "UP" : "DOWN" ) ;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Delta Value", &Terrain2BrushHeightAdjustVal[ EDIT_MODE_SEL ],	0,20,	"%-02.2f",1);

	if( editMode && imgui_lbp )
	{
		if ( ! g_pTerrain2Editor->IsUndoRecord() )
		{
			g_pTerrain2Editor->BeginUndoRecord( up ? "Up terrain" : "Down terrain", UA_TERRAIN2_HEIGHT );
		}

		g_EditedTerrain2Data = 1 ;

		if( r3dGetTime() - _LastTimeBrush > TERRAIN2_BRUSH_DELTA )
		{
			float Val = up ? 1.0f : -1.0f ;

			g_pTerrain2Editor->ApplyHeightBrush( UI_TerraTargetPos, Terrain2BrushHeightAdjustVal[ EDIT_MODE_SEL ] * Val * Terrain2BrushStrengthVal[ EDIT_MODE_SEL ], Terrain2BrushRadiusVal[ EDIT_MODE_SEL ], Terrain2BrushHardnessVal[ EDIT_MODE_SEL ] ) ;
			gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain2BrushRadiusVal[Terrain2EditMode] );
			_LastTimeBrush = r3dGetTime();
		}
	}

	return SliderY ;
}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_Level( float SliderX, float SliderY, int editMode )
{
	if (Keyboard->IsPressed(kbsLeftAlt)) Terrain2BrushHeightVal[ TE2_LEVEL ] = UI_TerraTargetPos.y;

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "LEVEL");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Height", &Terrain2BrushHeightVal[ TE2_LEVEL ],	0,1500,	"%-02.2f",1);

	if( editMode && imgui_lbp )
	{
		g_EditedTerrain2Data = 1 ;

		if ( ! g_pTerrain2Editor->IsUndoRecord() )
		{
			g_pTerrain2Editor->BeginUndoRecord( "Set level terrain", UA_TERRAIN2_HEIGHT );
		}

		if( r3dGetTime() - _LastTimeBrush > TERRAIN2_BRUSH_DELTA )
		{
			g_pTerrain2Editor->ApplyHeightLevel(UI_TerraTargetPos, Terrain2BrushHeightVal[ TE2_LEVEL ], Terrain2BrushStrengthVal[ TE2_LEVEL ], Terrain2BrushRadiusVal[ TE2_LEVEL ], Terrain2BrushHardnessVal[ TE2_LEVEL ]);
			gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain2BrushRadiusVal[Terrain2EditMode] );
			_LastTimeBrush = r3dGetTime();
		}
	}

	return SliderY ;
}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_Smooth( float SliderX, float SliderY, int editMode )
{
	extern int   _terra_smoothBoxOff;
	extern float _terra_smoothSpeed;

	const r3dTerrainDesc& desc = Terrain2->GetDesc() ;

	if( editMode )
	{
		// draw box filter size
		r3dPoint3D box[4];
		const float off = _terra_smoothBoxOff * desc.CellSize;
		box[0] = UI_TerraTargetPos + r3dPoint3D(-off, 0, -off);
		box[1] = UI_TerraTargetPos + r3dPoint3D(-off, 0,  off);
		box[2] = UI_TerraTargetPos + r3dPoint3D( off, 0,  off);
		box[3] = UI_TerraTargetPos + r3dPoint3D( off, 0, -off);
		r3dDrawLine3DT(box[0], box[1], gCam, 0.1f, r3dColor(255, 0, 0));
		r3dDrawLine3DT(box[1], box[2], gCam, 0.1f, r3dColor(255, 0, 0));
		r3dDrawLine3DT(box[2], box[3], gCam, 0.1f, r3dColor(255, 0, 0));
		r3dDrawLine3DT(box[3], box[0], gCam, 0.1f, r3dColor(255, 0, 0));
	}

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "SMOOTH");
	SliderY += imgui_Value_SliderI(SliderX, SliderY, "BoxSize/2", &_terra_smoothBoxOff, 1, (int)(Terrain2BrushRadiusVal[TE2_SMOOTH] / desc.CellSize), "%d", 1);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Speed(in sec)", &_terra_smoothSpeed, 0, 2.0f, "%.1f", 1);

	if( editMode && imgui_lbp)
	{
		g_EditedTerrain2Data = true;
		if ( ! g_pTerrain2Editor->IsUndoRecord() )
		{
			g_pTerrain2Editor->BeginUndoRecord( "Smooth terrain", UA_TERRAIN2_HEIGHT );
		}

		g_pTerrain2Editor->ApplyHeightSmooth(UI_TerraTargetPos, Terrain2BrushRadiusVal[TE2_SMOOTH] );
		gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain2BrushRadiusVal[TE2_SMOOTH] );
		_LastTimeBrush = r3dGetTime();
	}

	return SliderY ;
}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_Noise( float SliderX, float SliderY, int editMode )
{
	static int painting = 0;

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "NOISE");

	SliderY += imgui_Value_SliderI(SliderX, SliderY, "Octaves",      &g_pTerrain2Editor->noiseParams.noise.octaves_, 1, 8, "%d", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Persistence",  &g_pTerrain2Editor->noiseParams.noise.persistence_, 0.0001f, 2.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Frequency",    &g_pTerrain2Editor->noiseParams.noise.frequency_, 0.0001f, 1.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Amplitude",    &g_pTerrain2Editor->noiseParams.noise.amplitude_, 0.0001f, 1.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "MinHeight",    &g_pTerrain2Editor->noiseParams.minHeight, -500.0f, 500.0f, "%-02.2f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "MaxHeight",    &g_pTerrain2Editor->noiseParams.maxHeight, -500.0f, 500.0f, "%-02.2f", 1);

	if( imgui_Button(SliderX, SliderY, 360, 25, "flush cache", 0) )
	{
		g_pTerrain2Editor->noiseParams.ResetCache();
	}

	SliderY += 30;

	SliderY += 10;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "MorphSec", &g_pTerrain2Editor->noiseParams.heightApplySpeed, 0.0001f, 2.0f, "%-02.2f", 1 ) ;
	SliderY += imgui_Checkbox( SliderX, SliderY, "HeightRelative", &g_pTerrain2Editor->noiseParams.heightRelative, 1 ) ;

	// start new painting
	if(!painting)
	{
		if( editMode && imgui_lbp )
		{
			painting = true;
			if ( ! g_pTerrain2Editor->IsUndoRecord() )
			{
				g_pTerrain2Editor->BeginUndoRecord( "Noise terrain", UA_TERRAIN2_HEIGHT );
			}
		}
	}

	if(painting)
	{
		// check for painting stop
		if(!editMode || !imgui_lbp)
		{
			painting = false;
			return SliderY ;
		}

		g_EditedTerrain2Data = 1 ;
		g_pTerrain2Editor->ApplyHeightNoise( UI_TerraTargetPos, Terrain2BrushRadiusVal[ TE2_NOISE ], Terrain2BrushHardnessVal[ TE2_NOISE ] ) ;
		gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain2BrushRadiusVal[TE2_NOISE] );
	}

	return SliderY ;
}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_Ramp( float SliderX, float SliderY, int editMode )
{
	static float rampWidthOuter = 20.0f;
	static float rampWidthInner = 15.0f;

	SliderY += 10;
	SliderY += imgui_Static( SliderX, SliderY, "Ramp Creation" ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "WidthOuter", &rampWidthOuter, 1, 100, "%-02.2f", 1 ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "WidthInner", &rampWidthInner, 1, rampWidthOuter - 0.01f, "%-02.2f", 1 ) ;

	static int ramp_step = 0;
	static r3dPoint3D ramp_pnt[2];

	switch( ramp_step )
	{
		case 0:
			if( !editMode )
				break;

			ramp_pnt[0] = UI_TerraTargetPos;
			r3dDrawCircle3DT(ramp_pnt[0], 4.0f, gCam, 0.5f, r3dColor(0, 255, 0));
			drawRamp(ramp_pnt[0], ramp_pnt[0] + r3dPoint3D(0, 0, rampWidthOuter*2), rampWidthOuter, rampWidthInner);

			if(imgui_lbr) {
				ramp_step++;
			}

			break;

		case 1:
			if( !editMode )
			{
				ramp_step = 0;
				break;
			}
			ramp_pnt[1] = UI_TerraTargetPos;

			r3dDrawCircle3DT(ramp_pnt[0], 4.0f, gCam, 0.5f, r3dColor(0, 255, 0));
			r3dDrawCircle3DT(ramp_pnt[1], 4.0f, gCam, 0.5f, r3dColor(0, 0, 255));
			drawRamp(ramp_pnt[0], ramp_pnt[1], rampWidthOuter, rampWidthInner);

			if(imgui_lbr)
			{
				g_EditedTerrain2Data = true;
				if ( ! g_pTerrain2Editor->IsUndoRecord() )
				{
					g_pTerrain2Editor->BeginUndoRecord( "Create ramp", UA_TERRAIN2_HEIGHT );
				}
				g_pTerrain2Editor->ApplyHeightRamp(ramp_pnt[0], ramp_pnt[1], rampWidthOuter, rampWidthInner);
				ramp_step = 0;
			}
			break;
	}

	return SliderY ;
}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_Erosion( float SliderX, float SliderY, int editMode )
{
	if( Keyboard->IsPressed( kbsLeftAlt ) )
		Terrain2BrushHeightVal[ TE2_EROSION ] = UI_TerraTargetPos.y;

	{
		SliderY += 10;
		SliderY += imgui_Static(SliderX, SliderY, "Erosion pattern list: ");
		static int patternTexID = 0;
		static float patternIDOffset = 0;
		int prevID = patternTexID;
		imgui_DrawList( SliderX, SliderY, 360, 200, gErosionPatternTextureNames, &patternIDOffset, &patternTexID, true, false, true );

		if (prevID != patternTexID || !gErosionPattern)
		{
			r3dSTLString path = "data\\Editor\\ErosionPatterns\\" + gErosionPatternTextureNames[patternTexID];
			InitErosionPattern(path.c_str());
		}
	}

	if( editMode && imgui_lbp )
	{
		g_EditedTerrain2Data = 1 ;

		if ( ! g_pTerrain2Editor->IsUndoRecord() )
		{
			g_pTerrain2Editor->BeginUndoRecord( "Set level terrain", UA_TERRAIN2_HEIGHT ) ;
		}

		if( r3dGetTime() - _LastTimeBrush > 0.05 )
		{
			g_pTerrain2Editor->ApplyHeightErosion(UI_TerraTargetPos, Terrain2BrushStrengthVal[ TE2_EROSION ], Terrain2BrushRadiusVal[ TE2_EROSION ], Terrain2BrushHardnessVal[ TE2_EROSION ] ) ;
			gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain2BrushRadiusVal[ TE2_EROSION ] ) ;
			_LastTimeBrush = r3dGetTime();
		}
	}

	return SliderY ;
}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_Paint( float SliderX, float SliderY, int editMode )
{
	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "PAINT");

	static int CurrentPaintLayerIdx2 = 0 ;

	if( CurrentPaintLayerIdx2 >= Terrain2->GetDesc().LayerCount )
	{
		// delete through undo happened
		CurrentPaintLayerIdx2 = 0 ;
	}

	SliderY += imgui_Toolbar(SliderX, SliderY, 360, 45, &Terrain2PaintMode, 0, terrain2PaintList, sizeof terrain2PaintList / sizeof terrain2PaintList[ 0 ] );

	SliderY += imgui_Checkbox( SliderX, SliderY, "Show Material Heaviness", &DrawTerrainMatHeaviness, 1 );

	SliderY += 11 ;

	SliderY = DrawPaintBounds( SliderX, SliderY ) + 10;

	const r3dTerrainDesc& desc = Terrain2->GetDesc() ;

	int needUpdateTilesWithCurLayer = 0 ;

	// + 1 for base layer
	r3dTexture **MatTexArray		[ r3dTerrain2::MAX_TEXTURE_LAYERS + 1 ];
	r3dTexture **MatTexNormalArray	[ r3dTerrain2::MAX_TEXTURE_LAYERS + 1 ];
	float	   *MatScaleArray		[ r3dTerrain2::MAX_TEXTURE_LAYERS + 1 ];

	static r3dTL::TArray< r3dTerrainLayer > layers ;

	layers.Resize( desc.LayerCount ) ;

	for( int i = 0, e = desc.LayerCount ; i < e; i ++ )
	{
		r3dTerrainLayer& lay = layers[ i ] ;

		lay = Terrain2->GetLayer( i ) ;

		MatTexArray[ i ]		= &lay.DiffuseTex ;
		MatTexNormalArray[ i ]	= &lay.NormalTex ;
		MatScaleArray[ i ]		= &lay.ScaleU ;
	}

	const r3dString& detTexName = Terrain2->GetNormalDetailTextureName() ;

	int detailEnabled = !!detTexName.Length() ;

	SliderY += imgui_Checkbox( SliderX, SliderY, "Enable Detail Normals", &detailEnabled, 1 ) ;

	if( detailEnabled )
	{
		static float nmMix = 0.f ;
		nmMix = Terrain2->GetSettings().DetailNormalMix ;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Normal Mix", &nmMix, 0.f, 1.f, "%.2f" ) ;

		r3dTerrain2::Settings sts = Terrain2->GetSettings() ;

		bool needUpdateNormals = false ;

		if( sts.DetailNormalMix != nmMix )
			needUpdateNormals = true ;

		sts.DetailNormalMix = nmMix ;

		Terrain2->SetSettings( sts ) ;

		if( needUpdateNormals )
		{
			Terrain2->UpdateNormals() ;
		}

		if( !detTexName.Length() )
		{
			Terrain2->SetNormalDetailTexture( "Data/Shaders/Texture/DefaultNormal.dds" ) ;
		}

		if ( imgui_Button(SliderX+8,SliderY+8, 350+3, 350+3, NULL, g_iTerrainNormalMapEditMode == 2 ) )
		{
			g_iTerrainNormalMapEditMode = 2;
		}

		r3dDrawBox2D(SliderX+10+5 - Desktop().GetX(),SliderY+10+5- Desktop().GetY(), 350-8, 350-8, r3dColor(255,255,255), Terrain2->GetNormalDetailTexture() ) ;

		SliderY += 360 ;
	}
	else
	{
		Terrain2->SetNormalDetailTexture( "" ) ;
	}

	if( Terrain2PaintMode == TE2_ERASE )
	{
		SliderY += imgui_Checkbox( SliderX, SliderY, 360.0f, 22.f, "Erase All", &Terrain2Editor_EraseAll, 1 );
	}

	bool bChangedVal = false;

	r3dTerrainLayer* CurrentLayer = NULL ;

	int insertLayerAt = - 1 ;
	int removeLayerIdx = - 1 ;

	static char MatFName[64];

	if( Terrain2PaintMode != TE2_ERASE || !Terrain2Editor_EraseAll )
	{
		SliderY += imgui_Static( SliderX, SliderY, "Layers" );

		for( int i = 0, e = desc.LayerCount; i < e; i ++ )
		{
			if( imgui_Button( SliderX + 10, SliderY+1, 250, 20, layers[ i ].Name.c_str(), i == CurrentPaintLayerIdx2 ) )
			{
				CurrentPaintLayerIdx2 = i ;
				bChangedVal = true ;
			}

			if( imgui_Button( SliderX + 265, SliderY + 1, 33, 20, "+" ) )
			{
				insertLayerAt = i ;
			}

			if( i )
			{
				if( imgui_Button( SliderX + 265 + 44, SliderY + 1, 22, 20, "-" ) )
				{
					if( insertLayerAt < 0 )
					{
						removeLayerIdx = i ;
					}
				}
			}

			if( CurrentPaintLayerIdx2 >= 0 && CurrentPaintLayerIdx2 < desc.LayerCount )
			{
				CurrentLayer = &layers[ CurrentPaintLayerIdx2 ] ;
			}
			else
			{
				CurrentLayer = NULL;
			}

			SliderY += 22;
		}

		SliderY += 11;

		static char FullName[ 256 ];

		if( CurrentLayer )
		{
			r3dscpy( FullName, CurrentLayer->Name.c_str() );
		}
		else
		{
			r3dscpy( FullName, "Dummy Layer" ) ;
		}

		// skip name setup for base layer
		if( CurrentLayer && CurrentPaintLayerIdx2 )
		{
			imgui2_StringValueEx( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, "Layer Name: ", FullName ) ;

			CurrentLayer->Name = FullName ;

			SliderY += 22 ;
		}

		float prevTileFactor ;
		prevTileFactor = *MatScaleArray[ CurrentPaintLayerIdx2 ] ;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Tile Factor",	MatScaleArray[ CurrentPaintLayerIdx2 ], 1, 4096, "%-02.0f", 1 ) ;

		if( prevTileFactor != *MatScaleArray[ CurrentPaintLayerIdx2 ] )
		{
			needUpdateTilesWithCurLayer = 1 ;
		}

		r3dDrawBox2D(SliderX - Desktop().GetX(),SliderY - Desktop().GetY(), 360, 120, imgui_bkgDlg);

		if ( imgui_Button(SliderX+10,SliderY+10, 100, 100, NULL, g_iTerrainNormalMapEditMode == 0 ) )
		{
			g_iTerrainNormalMapEditMode = 0;
			bChangedVal = true;
		}

		r3dDrawBox2D(SliderX+10+5 - Desktop().GetX(),SliderY+10+5- Desktop().GetY(), 100-8, 100-8, r3dColor(255,255,255), *MatTexArray[ CurrentPaintLayerIdx2 ]);

		if ( imgui_Button(SliderX+10+110,SliderY+10, 100, 100, NULL, g_iTerrainNormalMapEditMode == 1 ) )
		{
			g_iTerrainNormalMapEditMode = 1;
			bChangedVal = true;
		}

		if( CurrentPaintLayerIdx2 >= 0 )
		{
			if( r3dTexture* tex = *( g_iTerrainNormalMapEditMode ? MatTexNormalArray : MatTexArray ) [ CurrentPaintLayerIdx2 ] )
			{
				float fStatisticYStart = r3dRenderer->ScreenH - 200;
				float fStatisticHeight = 80;

				char sStatStr[ MAX_PATH * 2 ];

				strcpy( sStatStr, tex->getFileLoc().FileName );
				strlwr( sStatStr );

				for( size_t i = 0, e = strlen( sStatStr ) ; i < e; i ++ )
				{
					if( sStatStr[ i ] == '/' )
						sStatStr[ i ] = '\\' ;
				}

				int toAdd = 0 ;

				if( char* subs = strstr( sStatStr, "data\\terraindata\\materials"  ) )
				{
					toAdd = sizeof "data\\terraindata\\materials" ;
				}

				sprintf( sStatStr, "%s texture\n%s\n%dx%d", g_iTerrainNormalMapEditMode ? "Normal" : "Diffuse", tex->getFileLoc().FileName + toAdd, tex->GetWidth(), tex->GetHeight() );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

				imgui_Static ( 370, fStatisticYStart, sStatStr, 360, false, fStatisticHeight );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) );

				if (imgui_Button(SliderX+10+2+110+106,SliderY+42+2, 60, 20, "EDIT", 0))
				{
#ifndef FINAL_BUILD
					const char* name = tex->getFileLoc().FileName ;

					char fullPath[ 4096 ] ;
					GetCurrentDirectory( sizeof fullPath - 2, fullPath ) ;
					strcat( fullPath, "\\" ) ;
					strcat( fullPath, name ) ;

					for( int i = 0, e = strlen( fullPath) ; i < e ; i ++ )
					{
						if( fullPath[ i ] == '/' )
							fullPath[ i ] = '\\' ;
					}

					const char* execStr = g_texture_edit_cmd->GetString() ;

					ShellExecute( NULL, "open", execStr, fullPath, NULL, SW_SHOWNORMAL ) ;
#endif
				}
			}
		}

		r3dDrawBox2D(SliderX+10+5+110-Desktop().GetX(),SliderY+10+5- Desktop().GetY(), 100-8, 100-8, r3dColor(255,255,255), *MatTexNormalArray[ CurrentPaintLayerIdx2 ]);

		SliderY += 122 ;

		if ( CurrentPaintLayerIdx2 >= 0 )
		{
			r3dTexture * pTex = g_iTerrainNormalMapEditMode ? *MatTexNormalArray[ CurrentPaintLayerIdx2 ] : *MatTexArray[ CurrentPaintLayerIdx2 ];
			if ( pTex )
			{
				FixedString s( pTex->getFileLoc().FileName );
				FixedString sName = s.GetName() + s.GetExt();
				r3dscpy( MatFName, sName.c_str() );
			}
		}

		if( CurrentLayer )
		{
			char MatTypeName[ 2048 ] ;

			strncpy( MatTypeName, CurrentLayer->MaterialTypeName.c_str(), sizeof MatTypeName - 1 ) ;

			SliderY = DrawMaterialTypeSelection( SliderX, SliderY, MatTypeName );

			if( strcmp( MatTypeName, CurrentLayer->MaterialTypeName.c_str() ) != 0 )
			{
				CurrentLayer->MaterialTypeName = MatTypeName;
				Terrain2->RecalcLayerVars();
			}
		}
	}

	g_pDesktopManager->End();

	static float matSelOffset = 0.f ;

	if( CurrentLayer )
	{
		if( imgui_FileList( 5, 120, 300, 380, "Data\\TerrainData\\Materials\\*.*", MatFName, &matSelOffset, false, bChangedVal ) )
		{
			char SS1[256];

			needUpdateTilesWithCurLayer = 1 ;

			sprintf(SS1, "Data\\TerrainData\\Materials\\%s", MatFName);

			switch( g_iTerrainNormalMapEditMode )
			{
			case 2:
				Terrain2->SetNormalDetailTexture( SS1 ) ;
				break ;

			case 1:
				r3dRenderer->DeleteTexture(*MatTexNormalArray[ CurrentPaintLayerIdx2 ]);
				*MatTexNormalArray[ CurrentPaintLayerIdx2 ] = r3dRenderer->LoadTexture(SS1);

				break ;

			case 0:
				r3dRenderer->DeleteTexture(*MatTexArray[ CurrentPaintLayerIdx2 ]);
				*MatTexArray[ CurrentPaintLayerIdx2 ] = r3dRenderer->LoadTexture(SS1);

				break ;
			}

		}

		CurrentLayer->ScaleV = CurrentLayer->ScaleU ;

		Terrain2->SetLayer( CurrentPaintLayerIdx2, *CurrentLayer ) ;

		if( needUpdateTilesWithCurLayer )
		{
			Terrain2->UpdateTilesWithLayer( CurrentPaintLayerIdx2 ) ;
		}
	}

	g_pDesktopManager->Begin( "ed_terrain" ) ;

	if ( editMode && imgui_lbp )
	{
		if( Terrain2Editor_EraseAll )
		{
			g_EditedTerrain2Data = 1 ;

			if( !g_pTerrain2Editor->IsUndoRecord() )
			{
				g_pTerrain2Editor->BeginUndoRecord( "Terrain2 Erase All Layers", UA_TERRAIN2_MASK_ERASEALL );
				g_pTerrain2Editor->StartEraseAllBrush();
			}

			if( r3dGetTime() - _LastTimeBrush > 0.025 )
			{
				g_pTerrain2Editor->ApplyEraseAllBrush( g_TerraPaintBoundCtrl, UI_TerraTargetPos, 255.f * Terrain2BrushStrengthVal[Terrain2EditMode], Terrain2BrushRadiusVal[Terrain2EditMode], Terrain2BrushHardnessVal[Terrain2EditMode] ) ;

				_LastTimeBrush = r3dGetTime();
			}
		}
		else
		{
			if ( CurrentPaintLayerIdx2 )
			{
				g_EditedTerrain2Data = 1 ;

				if( ! g_pTerrain2Editor->IsUndoRecord() )
				{
					g_pTerrain2Editor->BeginUndoRecord( Terrain2PaintMode ? "Paint mask" : "Erase mask", UA_TERRAIN2_MASK_PAINT );
					g_pTerrain2Editor->StartLayerBrush( CurrentPaintLayerIdx2 ) ;
				}

				if( r3dGetTime() - _LastTimeBrush > 0.025 )
				{

					g_pTerrain2Editor->ApplyLayerBrush( g_TerraPaintBoundCtrl, UI_TerraTargetPos, Terrain2PaintMode, CurrentPaintLayerIdx2, 255.f * Terrain2BrushStrengthVal[Terrain2EditMode], Terrain2BrushRadiusVal[Terrain2EditMode], Terrain2BrushHardnessVal[Terrain2EditMode] ) ;

					_LastTimeBrush = r3dGetTime();
				}
			}
		}
	}

	if( imgui_Button( SliderX, SliderY, 360, 22, "Optimize masks" ) )
	{
		Terrain2->OptimizeLayerMasks() ;
		Terrain2->RefreshAtlasTiles() ;
	}

	int needResetSelection = 0 ;

	if( insertLayerAt >= 0 )
	{
		needResetSelection = 1 ;

		if( !g_pTerrain2Editor->IsUndoRecord() )
		{
			g_pTerrain2Editor->BeginUndoRecord( "Terrain Insert Layer", UA_TERRAIN2_INSERT_LAYER ) ;

			CTerrain2InsertLayer* insert = 	static_cast<CTerrain2InsertLayer*>( g_pTerrain2Editor->GetUndoRecord() ) ;

			insert->SetData( insertLayerAt ) ;

			g_pTerrain2Editor->EndUndoRecord() ;
		}
		else
		{
			r3dOutToLog( "Couldn't add undo bcoz designer is pressing to many buttons at once!\n" ) ;
		}


		Terrain2->InsertLayer( insertLayerAt, NULL, NULL ) ;
	}
	else
	if( removeLayerIdx >= 0 )
	{
		needResetSelection = 1 ;

		int began = 0 ;

		if( !g_pTerrain2Editor->IsUndoRecord() )
		{
			g_pTerrain2Editor->BeginUndoRecord( "Terrain Destroy Layer", UA_TERRAIN2_DESTROY_LAYER ) ;

			CTerrain2DestroyLayer* destroy = static_cast<CTerrain2DestroyLayer*>( g_pTerrain2Editor->GetUndoRecord() ) ;

			destroy->SetData( removeLayerIdx - 1 ) ;

			g_pTerrain2Editor->EndUndoRecord() ;
		}
		else
		{
			r3dOutToLog( "Couldn't add undo bcoz designer is pressing to many buttons at once!\n" ) ;
		}

		Terrain2->DestroyLayer( removeLayerIdx - 1 ) ;
	}

	if( needResetSelection )
	{
		CurrentLayer = NULL ;
		CurrentPaintLayerIdx2 = 0 ;
	}

	SliderY += 22 ;

	return SliderY ;
}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_ColorPaint( float SliderX, float SliderY, int editMode )
{
	SliderY = DrawPaintBounds( SliderX, SliderY ) + 10;
	SliderY += imgui_DrawColorPicker( SliderX, SliderY, "Color", &m_tPaintTerrainColor, 360, false, true, true );

	SliderY += 22.f;

	if ( editMode && imgui_lbp )
	{
		g_EditedTerrain2Data = true;
		if ( ! g_pTerrain2Editor->IsUndoRecord() )
		{
			g_pTerrain2Editor->BeginUndoRecord( "Color paint", UA_TERRAIN2_COLOR_PAINT );
			g_pTerrain2Editor->StartColorBrush() ;
		}

		if( r3dGetTime() - _LastTimeBrush > 0.025 )
		{
			g_pTerrain2Editor->ApplyColorBrush( g_TerraPaintBoundCtrl, UI_TerraTargetPos, m_tPaintTerrainColor, Terrain2BrushStrengthVal[Terrain2EditMode], Terrain2BrushRadiusVal[Terrain2EditMode], Terrain2BrushHardnessVal[Terrain2EditMode] ) ;
			_LastTimeBrush = r3dGetTime();
		}
	}

	SliderY += 20;
	SliderY += imgui_Static(SliderX, SliderY, "Available color maps");

	static char HMapName[256] ;
	static char Path[256];
	static float offset = 0.f ;

	if (imgui_FileList(SliderX, SliderY, 360, 200, "Data\\TerrainData\\Color\\*.dds", HMapName, &offset ))
	{
		sprintf(Path,"Data\\TerrainData\\Color\\%s", HMapName);
	}

	SliderY += 206;

#if 0
	if (imgui_Button(SliderX, SliderY, 360, 25, "Import color", 0))
	{
		Terrain1->ImportColor(Path);
	}

	SliderY += 28;

	if (imgui_Button(SliderX, SliderY, 360, 25, "Export color", 0))
	{
		char drive[ 16 ], folder[ 1024 ] = "", name[ 1024], ext[ 256 ] ;
		_splitpath( LevelEditName, drive, folder, name, ext ) ;

		for( int i = 0, e = strlen( folder ) ; i < e; i ++ )
		{
			char &ch = folder[ i ] ;
			if( ch == '\\' ||ch == '/' )
			{
				ch = '_' ;
			}
		}

		sprintf( Path,"Data/TerrainData/Color/%s%s_export.dds", folder, name );

		if( Terrain1->ExportColor( Path ) )
			MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Saved to " ) + Path ).c_str(), "Information", MB_OK ) ;
		else
			MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Failed to saved to " ) + Path ).c_str(), "Information", MB_ICONERROR ) ;

	}
#endif

	SliderY += 35;

	return SliderY ;

}

//------------------------------------------------------------------------

float
Editor_Level::ProcessTerrain2_HMap( float SliderX, float SliderY )
{
	SliderY += 20;
	SliderY += imgui_Static( SliderX, SliderY, "Available Heightmaps" ) ;

	const r3dTerrainDesc& desc = Terrain2->GetDesc() ;

	static char HMapName[256] ;
	static char Path[256] ;

	static float terraYScale = desc.MaxHeight - desc.MinHeight ;
	static float terraYOffset = desc.MinHeight ;

	static float offset = 0.f ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Terrain scale", &terraYScale, 0, 1000, "%-02.2f", 1 ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Terrain offset", &terraYOffset, 0, 1000, "%-02.2f", 1 ) ;

	if (imgui_FileList(SliderX, SliderY, 360, 200, "Data\\TerrainData\\Heightmaps\\*.dds", HMapName, &offset ))
	{
		sprintf(Path,"Data\\TerrainData\\Heightmaps\\%s", HMapName);
	}

	SliderY += 206;

	if (imgui_Button(SliderX, SliderY, 360, 25, "Import heightmap", 0))
	{
		g_pTerrain2Editor->ImportHeight( Path, terraYScale, terraYOffset ) ;
		g_pGrassMap->UpdateHeight();

		RepositionAllObjectsOnTerrain();
	}
	SliderY += 28;

	if (imgui_Button(SliderX, SliderY, 360, 25, "Export heightmap", 0))
	{
		char drive[ 16 ], folder[ 1024 ] = "", name[ 1024], ext[ 256 ] ;
		_splitpath( LevelEditName, drive, folder, name, ext ) ;

		for( int i = 0, e = strlen( folder ) ; i < e; i ++ )
		{
			char &ch = folder[ i ] ;
			if( ch == '\\' ||ch == '/' )
			{
				ch = '_' ;
			}
		}

		sprintf( Path, "Data/TerrainData/Heightmaps/%s%s_export.dds", folder, name );

		if( g_pTerrain2Editor->ExportHeight( Path ) )
			MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Saved to " ) + Path ).c_str(), "Information", MB_OK ) ;
		else
			MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Failed to saved to " ) + Path ).c_str(), "Information", MB_ICONERROR ) ;

	}

	SliderY += 35;

	return SliderY ;
}

//------------------------------------------------------------------------

void
Editor_Level::ProcessTerrain2()
{
	R3DPROFILE_FUNCTION("Editor_Level::ProcessTerrain2");

	if( r_terrain2->GetInt() )
	{
		imgui_Toolbar(5, 80, 950, 35, &Terrain2EditMode, 0, Terrain2_Edit_List, sizeof( Terrain2_Edit_List ) / sizeof( char* ), false );
	}
	else
	{
		imgui_Toolbar(5, 80, 90, 35, &Terrain2EditMode, 0, Terrain2_Edit_List, 1, false ) ;
		Terrain2EditMode = TE2_SETTINGS ;
	}


	g_pDesktopManager->Begin( "ed_terrain" );

	float SliderX = r3dRenderer->ScreenW - 375;
	float SliderY = 105;

	int editMode = 0 ;

	if( Keyboard->IsPressed( kbsLeftControl ) ) editMode = 1;

	const r3dTerrainDesc& desc = Terrain2->GetDesc() ;

	g_EditedTerrain2Data = 0 ;

	if( !g_pTerrain2Editor->AreHeightsLoaded() )
	{
		g_pTerrain2Editor->LoadHeightsFromTerrain() ;
	}

	if( Terrain2EditMode != TE2_SETTINGS && Terrain2EditMode != TE2_HMAP )
	{
		if( Terrain2EditMode != TE2_EROSION)
		{
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Radius",		&Terrain2BrushRadiusVal[Terrain2EditMode],	desc.CellSize,desc.CellSize * 100.f,	"%-02.2f", 1 ) ;
		}
		else
		{
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Radius",		&Terrain2BrushRadiusVal[Terrain2EditMode],	desc.CellSize,desc.CellSize * 50.f,		"%-02.2f", 1 ) ;
		}
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Hardness",			&Terrain2BrushHardnessVal[Terrain2EditMode],	0,1,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Strength",			&Terrain2BrushStrengthVal[Terrain2EditMode],	0,1,	"%-02.2f",1);
		SliderY += imgui_Checkbox(SliderX, SliderY, "Reposition Objects",	&__RepositionObjectsOnTerrain , 1);
	}

	int show_terra_circle = 1;
	if(Terrain2EditMode == TE_RAMP) show_terra_circle = 0;

	if( editMode && show_terra_circle)
	{
		PushTerrainBrush(	Terrain2BrushRadiusVal[Terrain2EditMode],
							Terrain2BrushHardnessVal[Terrain2EditMode] );
	}

	bool isHeightEditing = Terrain2EditMode != TE2_SETTINGS
								&&
							Terrain2EditMode != TE2_COLOR ;

	if( isHeightEditing )
	{
		if( editMode && imgui_lbp )
		{
#if ENABLE_AUTODESK_NAVIGATION
			Nav::ResetCachedLevelGeometry();
#endif
			if ( ! g_pTerrain2Editor->IsUndoRecord() )
			{
				gTerrainHeightDirtiness.Reset( 128, 128 );
			}
		}
	}

	switch( Terrain2EditMode )
	{
	case TE2_SETTINGS:
		SliderY = ProcessTerrain2_Settings( SliderX, SliderY ) ;
		break ;

	case TE2_DOWN:
		SliderY = ProcessTerrain2_UpDown( SliderX, SliderY, editMode, false ) ;
		break ;

	case TE2_UP:
		SliderY = ProcessTerrain2_UpDown( SliderX, SliderY, editMode, true ) ;
		break ;
	case TE2_LEVEL:
		SliderY = ProcessTerrain2_Level( SliderX, SliderY, editMode ) ;
		break ;
	case TE2_SMOOTH:
		SliderY = ProcessTerrain2_Smooth( SliderX, SliderY, editMode ) ;
		break ;
	case TE2_NOISE:
		SliderY = ProcessTerrain2_Noise( SliderX, SliderY, editMode ) ;
		break ;
	case TE2_RAMP:
		SliderY = ProcessTerrain2_Ramp( SliderX, SliderY, editMode ) ;
		break ;
	case TE2_EROSION:
		SliderY = ProcessTerrain2_Erosion( SliderX, SliderY, editMode ) ;
		break ;
	case TE2_PAINT:
		SliderY = ProcessTerrain2_Paint( SliderX, SliderY, editMode ) ;
		break ;
	case TE2_COLOR:
		SliderY = ProcessTerrain2_ColorPaint( SliderX, SliderY, editMode ) ;
		break ;
	case TE2_HMAP:
		SliderY = ProcessTerrain2_HMap( SliderX, SliderY ) ;
		break ;
	}

	if ( !g_EditedTerrain2Data && g_pTerrain2Editor->IsUndoRecord() )
	{
		g_pTerrain2Editor->EndUndoRecord() ;

		switch( Terrain2EditMode )
		{
		case TE2_PAINT:

			if( Terrain2PaintMode == TE2_ERASE && Terrain2Editor_EraseAll )
			{
				g_pTerrain2Editor->EndEraseAllBrush() ;
			}
			else
			{
				g_pTerrain2Editor->EndLayerBrush() ;
			}
			break ;
		case TE2_COLOR:
			g_pTerrain2Editor->EndColorBrush() ;
			break ;
		default :
			g_pTerrain2Editor->FinalizeHeightEditing() ;
			break ;
		}

		if( isHeightEditing )
		{
			gTerrainHeightDirtiness.Update( true ) ;
		}
	}

	g_pDesktopManager->End() ;
}

void
Editor_Level::ProcessCreateTerrain2()
{
#ifndef FINAL_BUILD
	g_pDesktopManager->Begin( "ed_terrain" );

	float SliderX = r3dRenderer->ScreenW - 375;
	float SliderY = 85;

	SliderY += imgui_Static( SliderX, SliderY, "Create from" ) ;

	static int currentMap = 1 ;

	SliderY += imgui_Checkbox( SliderX, SliderY, "Current level's terrain", &currentMap, 1 ) ;

#define LEVELS_FOLDER "Levels/"

	static char fullDir[ 1024 ] ;

	fullDir[ 0 ] = 0 ;

	if( !currentMap )
	{
		static char selectedDir[ 512 ] = { 0 };

		static float dirOffset = 0.f ;

		SliderY += imgui_Static( SliderX, SliderY, selectedDir ) ;

		SliderY += 8 ;

		imgui_FileList( SliderX, SliderY, 360.f, 180.f, LEVELS_FOLDER"*.*", selectedDir, &dirOffset ) ;
		SliderY += 188.f ;

		strcpy( fullDir, LEVELS_FOLDER ) ;
		strcat( fullDir, selectedDir ) ;
	}
	else
	{
		strcpy( fullDir, r3dGameLevel::GetHomeDir() ) ;
	}

	if( fullDir[ 0 ] )
	{
		if( imgui_Button( SliderX, SliderY, 360.f, 33.f, "Create" ) )
		{
			Terrain2 = gfx_new r3dTerrain2 ;
			Terrain2->Load1( fullDir ) ;

			r_terrain2->SetInt( 1 ) ;

			Terrain = Terrain2 ;
		}
	}

#undef LEVELS_FOLDER

	g_pDesktopManager->End() ;
#endif
}

float Editor_Level :: ProcessTerrain3_Settings( float SliderX, float SliderY )
{
#if 0
	if( r_terrain_quality->GetInt() < 3 
		||
		r_texture_quality->GetInt() < 3
		)
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) ) ;
		imgui_Static( r3dRenderer->ScreenW2 - 350, r3dRenderer->ScreenH2, "Please set Terrain Quality and Texture Quality to 3 in Game Options to modify Terrain Settings",700, false, 22, true ) ;
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) ) ;
		return SliderY ;
	}
#endif

	int allowTerrain3 = !!r_terrain3->GetInt() ;

	SliderY += imgui_Checkbox( SliderX, SliderY, "Allow Terrain3", &allowTerrain3, 1 ) ;

	if( !allowTerrain3 && r_terrain3->GetInt() )
	{
		Terrain3->ReleaseRoads() ;
	}

	if( allowTerrain3 && !r_terrain3->GetInt() )
	{
		for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->ObjTypeFlags & OBJTYPE_Road )
			{
				obj->ObjFlags |= OBJFLAG_SkipDraw ;
			}
		}

		Terrain = Terrain3 ;
	}

	r_terrain3->SetInt( !!allowTerrain3 ) ;

	if( allowTerrain3 )
	{
		static r3dTerrain3::Settings settings ;

		settings = Terrain3->GetSettings() ;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Specular", &settings.Specular, 0.f, 1.f, "%.3f" );

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Fringe Height", &settings.FringeHeight, 0.03125, 0.5f, "%.3f" ) ;

		Terrain3->SetSettings( settings ) ;

		char buf[ 512 ] ;

		sprintf( buf, "Max Card Anisotropy: %d", r3dRenderer->d3dCaps.MaxAnisotropy  ) ;
		SliderY += imgui_Static( SliderX, SliderY, buf ) ;

		static float maxAnisotropy = 0 ;		
		maxAnisotropy = (float)r_terrain2_anisotropy->GetInt() ;
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Anisotropy", &maxAnisotropy, 0.f, 16.f, "%.0f" ) ;
		r_terrain2_anisotropy->SetInt( maxAnisotropy ) ;

#if 0
		static float padding = 0.f;
		padding = r_terrain2_padding->GetInt();
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Padding", &padding, 0.f, 32.f, "%.0f" );

		if( (int)padding != r_terrain2_padding->GetInt() )
		{
			Terrain3->RefreshAtlasTiles();
		}

		r_terrain2_padding->SetInt( (int) padding );
#endif

		//------------------------------------------------------------------------
		// quality settings
		{
			stringlist_t qualityList ;

			qualityList.push_back( "Low" ) ;
			qualityList.push_back( "Medium" ) ;
			qualityList.push_back( "High" ) ;

			static float qlListOffset = 0.f ;
			static int editQl = 0 ;

			SliderY += imgui_Static( SliderX, SliderY, "Edit Terrain Quality Level" ) ;

			float QL_LIST_HEIGHT = 90.0f ;

			editQl = r_terrain_quality->GetInt() - 1 ;

			imgui_DrawList( SliderX, SliderY, 360, QL_LIST_HEIGHT, qualityList, &qlListOffset, &editQl ) ;

			r_terrain_quality->SetInt( editQl + 1 ) ;
			SliderY += QL_LIST_HEIGHT ;


			r3dTerrain3::QualitySettings qsts = Terrain3->GetBaseQualitySettings( editQl ) ;

			SliderY += imgui_Static( SliderX, SliderY, "Tile Resolution" ) ;

			stringlist_t strList ;
			strList.push_back( "64x64" ) ;
			strList.push_back( "128x128" ) ;
			strList.push_back( "256x256" ) ;
			strList.push_back( "512x512" ) ;

			static float offset = 0.f ;

			int selected = 1 ;
			switch( qsts.AtlasTileDim )
			{
			case 64:
				selected = 0 ;
				break ;
			case 128:
				selected = 1 ;
				break ;
			case 256:
				selected = 2 ;
				break ;
			case 512:
				selected = 3 ;
				break ;
			}

			const float LIST_HEIGHT = 110.f ;

			imgui_DrawList( SliderX, SliderY, 360.f, LIST_HEIGHT, strList, &offset, &selected ) ;

			SliderY += LIST_HEIGHT ;

			switch( selected )
			{
			case 0:
				qsts.AtlasTileDim = 64 ;
				break ;
			case 1:
				qsts.AtlasTileDim = 128 ;
				break ;
			case 2:
				qsts.AtlasTileDim = 256 ;
				break ;
			case 3:
				qsts.AtlasTileDim = 512 ;
				break ;
			}

			static float offset1 = 0.f ;

			SliderY += imgui_Static( SliderX, SliderY, "Tile Vertex Size" ) ;

			strList.clear() ;

			strList.push_back( "1" ) ;
			strList.push_back( "2" ) ;
			strList.push_back( "4" ) ;
			strList.push_back( "8" ) ;

			selected = 0 ;

			switch( qsts.VertexTileDim )
			{
			case 1:
				selected = 0 ;
				break ;
			case 2:
				selected = 1 ;
				break ;
			case 4:
				selected = 2 ;
				break ;
			case 8:
				selected = 3 ;
				break ;
			}

			imgui_DrawList( SliderX, SliderY, 360.f, LIST_HEIGHT, strList, &offset1, &selected ) ;

			SliderY += LIST_HEIGHT ;

			switch( selected )
			{
			case 0:
				qsts.VertexTileDim = 1 ;
				break ;
			case 1:
				qsts.VertexTileDim = 2 ;
				break ;
			case 2:
				qsts.VertexTileDim = 4 ;
				break ;
			case 3:
				qsts.VertexTileDim = 8 ;
				break ;
			}

			// if this fails make MIN_VERTEX_TILE_DIM less ( 1? ; )
			r3d_assert( qsts.VertexTileDim >= r3dTerrain3::QualitySettings::MIN_VERTEX_TILE_DIM ) ;

			static float lodCounts[ r3dTerrain3::NUM_QUALITY_LAYERS ] ;

			for( int i = 0, e = r3dTerrain3::NUM_QUALITY_LAYERS ; i < e ; i ++ )
			{
				lodCounts[ i ] = qsts.TileCounts[ i ] / 2 ;

				char name[ 64 ] ;

				sprintf( name, "Tex Lod %d count", i ) ;

				SliderY += imgui_Value_Slider( SliderX, SliderY, name, &lodCounts[ i ], 1.f, 8.f, "%.0f" ) ;

				qsts.TileCounts[ i ] = int( lodCounts[ i ] ) * 2 ;
			}

#if 0
			static float geomLodSteps[ r3dTerrain3::QualitySettings::NUM_DENSITY_STEPS ] ;

			for( int i = 0, e = r3dTerrain3::QualitySettings::NUM_DENSITY_STEPS ; i < e ; i ++ )
			{
				geomLodSteps[ i ] = qsts.TileVertexDensitySteps[ i ] ;

				char name[ 64 ] ;

				sprintf( name, "Geom Lod %d", i ) ;

				SliderY += imgui_Value_Slider( SliderX, SliderY, name, &geomLodSteps[ i ], 8.f, 128.f, "%.0f" ) ;

				qsts.TileVertexDensitySteps[ i ] = geomLodSteps[ i ] ;
			}
#else
			{
				static float offset1 = 0.f ;

				SliderY += 11 ;

				SliderY += imgui_Static( SliderX, SliderY, "Vertex Density" ) ;

				strList.clear() ;

				strList.push_back( "1" ) ;
				strList.push_back( "2" ) ;
				strList.push_back( "4" ) ;
				strList.push_back( "8" ) ;
				strList.push_back( "16" ) ;
				strList.push_back( "32" ) ;
				strList.push_back( "64" ) ;
				strList.push_back( "128" ) ;

				selected = 0 ;

				switch( qsts.VertexDensity )
				{
				case 1:
					selected = 0 ;
					break ;
				case 2:
					selected = 1 ;
					break ;
				case 4:
					selected = 2 ;
					break ;
				case 8:
					selected = 3 ;
					break ;
				case 16:
					selected = 4 ;
					break ;
				case 32:
					selected = 5 ;
					break ;
				case 64:
					selected = 6 ;
					break ;
				case 128:
					selected = 7 ;
					break ;
				}

				const int DENS_LIST_HEIGHT = 180 ;

				imgui_DrawList( SliderX, SliderY, 360.f, DENS_LIST_HEIGHT, strList, &offset1, &selected ) ;

				SliderY += DENS_LIST_HEIGHT ;

				selected = R3D_MIN( R3D_MAX( selected, 0 ), 7 ) ;

				qsts.VertexDensity = 1 << selected ;
			}

#endif
			int IsTerrain3Frozen();

			if( !IsTerrain3Frozen() )
			{
				Terrain3->SetQualitySettings( qsts, true, editQl );
			}
		}

		static int showStats = 0;

		SliderY += imgui_Checkbox( SliderX, SliderY, "Show Stats", &showStats, 1 );

		if( showStats )
		{
			r3dTerrain3Stats stats;
			Terrain3->GetStats( &stats );

			char buf[ 512 ];

			sprintf( buf, "Volumes: %d", stats.VolumeCount );
			SliderY += imgui_Static( SliderX, SliderY, buf );

			sprintf( buf, "Tiles: %d", stats.TileCount );
			SliderY += imgui_Static( SliderX, SliderY, buf );

			sprintf( buf, "MegaTiles: %d", stats.MegaTileCount );
			SliderY += imgui_Static( SliderX, SliderY, buf );

			float toMegs = 1.0f / 1024.f / 1024.f;

			sprintf( buf, "Tile VMem: %.2f MB", stats.TileTextureMemory * toMegs );
			SliderY += imgui_Static( SliderX, SliderY, buf ); 

			sprintf( buf, "Mask VMem: %.2f MB", stats.MaskTextureMemory * toMegs );
			SliderY += imgui_Static( SliderX, SliderY, buf );

			sprintf( buf, "Layer VMem: %.2f MB", stats.LayerTextureMemory * toMegs );
			SliderY += imgui_Static( SliderX, SliderY, buf );

			int total = stats.LayerTextureMemory + stats.MaskTextureMemory + stats.TileTextureMemory;

			sprintf( buf, "Total VMem: %.2f MB", total * toMegs );
			SliderY += imgui_Static( SliderX, SliderY, buf );

		}

		int showTiles = !!r_terrain2_show_tiles->GetInt();
		SliderY += imgui_Checkbox( SliderX, SliderY, "Show Tiles", &showTiles, 1 );
		r_terrain2_show_tiles->SetInt( !!showTiles );

		int showMega = !!r_terrain2_show_mega_struture->GetInt();
		SliderY += imgui_Checkbox( SliderX, SliderY, "Show Mega Scheme", &showMega, 1 );
		r_terrain2_show_mega_struture->SetInt( !!showMega );

		stringlist_t megaTexSelections;

		SliderY += imgui_Static( SliderX, SliderY, "Show megatex atlas" );

		megaTexSelections.push_back( "None" );
		megaTexSelections.push_back( "Masks" );
		megaTexSelections.push_back( "Heights" );
		megaTexSelections.push_back( "Normals" );

		static float tex_sel_offset = 0.0f;
		static int tex_selection = 0;

		const float LIST_HEIGHT = 90.0f;
		imgui_DrawList( SliderX, SliderY, 360.0f, LIST_HEIGHT, megaTexSelections, &tex_sel_offset, &tex_selection );
		SliderY += LIST_HEIGHT;

		tex_selection = R3D_MAX( R3D_MIN( tex_selection, (int)megaTexSelections.size() ), 0 );
		r_terrain2_show_mega_tex->SetInt( tex_selection );

		if( tex_selection )
		{
			static float scale; scale = r_terrain2_show_mt_scale->GetFloat();
			SliderY += imgui_Value_Slider( SliderX, SliderY, "MM Scale", &scale, 1.f, 32.f, "%.2f" );
			r_terrain2_show_mt_scale->SetFloat( scale );

			static float offx; offx = r_terrain2_show_mt_offx->GetFloat();
			SliderY += imgui_Value_Slider( SliderX, SliderY, "MM Offset X", &offx, -1.f, 1.f, "%.2f" );
			r_terrain2_show_mt_offx->SetFloat( offx );

			static float offy; offy = r_terrain2_show_mt_offy->GetFloat();
			SliderY += imgui_Value_Slider( SliderX, SliderY, "MM Offset Y", &offy, -1.f, 1.f, "%.2f" );
			r_terrain2_show_mt_offy->SetFloat( offy );

			int showGrid = !!r_terrain2_show_mt_grid->GetInt();
			SliderY += imgui_Checkbox( SliderX, SliderY, "Show MM Grid", &showGrid, 1 );
			r_terrain2_show_mt_grid->SetInt( !!showGrid );
		}

		if( tex_selection )
		{
			r3dTerrain3Stats stats;
			Terrain3->GetStats( &stats );

			static float megaIdxSlider;
			megaIdxSlider = r_terrain2_show_mega_tex_idx->GetInt();

			int maxIdx = 0;

			switch( tex_selection )
			{
			case SHOWMEGA_MASKS:
				maxIdx = R3D_MAX( stats.MaskVolumeCount - 1, 0 );
				break;
			case SHOWMEGA_HEIGHT:
			case SHOWMEGA_NORMAL:
				maxIdx = R3D_MAX( stats.NormalAndHeightVolumeCount - 1, 0 );
				break;
			}

			SliderY += imgui_Value_Slider( SliderX, SliderY, "Tex Idx", &megaIdxSlider, 0.0f, (float)( maxIdx ), "%.0f" );
			r_terrain2_show_mega_tex_idx->SetInt( (int)megaIdxSlider );
		}

		//------------------------------------------------------------------------

		stringlist_t atlasTexSelections;

		SliderY += imgui_Static( SliderX, SliderY, "Show Atlas" );

		atlasTexSelections.push_back( "None" );
		atlasTexSelections.push_back( "Diffuse" );
		atlasTexSelections.push_back( "Normals" );

		static float atlas_tex_sel_offset = 0.0f;
		static int atlas_tex_selection = 0;

		atlas_tex_selection = r_terrain2_show_atlas->GetInt();

		const float ATLAS_LIST_HEIGHT = 70.0f;
		imgui_DrawList( SliderX, SliderY, 360.0f, ATLAS_LIST_HEIGHT, atlasTexSelections, &atlas_tex_sel_offset, &atlas_tex_selection );
		SliderY += ATLAS_LIST_HEIGHT;

		r_terrain2_show_atlas->SetInt( atlas_tex_selection );

		//------------------------------------------------------------------------

		if( r_terrain2_show_atlas->GetInt() )
		{
			static float scale = 1.0f;

			scale = r_terrain2_show_atlas_scale->GetFloat();
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Atlas Scale", &scale, 0.0625f, 32.f, "%.2f" );
			r_terrain2_show_atlas_scale->SetFloat( scale );

			static float volume = 0.0f;

			r3dTerrain3Stats stats;
			Terrain3->GetStats( &stats );

			volume = r_terrain2_show_atlas_volume->GetFloat();
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Atlas Volume", &volume, 0.f, stats.VolumeCount - 1, "%.0f" );
			r_terrain2_show_atlas_volume->SetFloat( volume );

			static float off_x = 0.f;

			off_x = r_terrain2_show_atlas_off_x->GetFloat();
			SliderY += imgui_Value_Slider(	SliderX, SliderY, "Offset X", &off_x, 
											-r3dTerrain3::ATLAS_TEXTURE_DIM / 2, 
											+r3dTerrain3::ATLAS_TEXTURE_DIM / 2,
											"%.0f" );

			r_terrain2_show_atlas_off_x->SetFloat( off_x );

			static float off_y = 0.f;

			off_y = r_terrain2_show_atlas_off_y->GetFloat();
			SliderY += imgui_Value_Slider(	SliderX, SliderY, "Offset Y", &off_y, 
											-r3dTerrain3::ATLAS_TEXTURE_DIM / 2, 
											+r3dTerrain3::ATLAS_TEXTURE_DIM / 2,
											"%.0f" );

			r_terrain2_show_atlas_off_y->SetFloat( off_y );

			static float opacity = 0.f;

			opacity = r_terrain2_show_atlas_oppa->GetFloat();
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Atlas Opacity", &opacity, 0.f, 1.f, "%.2f" );
			r_terrain2_show_atlas_oppa->SetFloat( opacity );

			int showGrid = r_terrain2_show_atlas_grid->GetInt();
			SliderY += imgui_Checkbox( SliderX, SliderY, "Show Grid", &showGrid, 1 );
			r_terrain2_show_atlas_grid->SetInt( !!showGrid );
		}

		{
			SliderY += imgui_Static( SliderX, SliderY, "Resize terrain:" );

			stringlist_t sizes;

			int curPow = 0;

			for( int n = Terrain3->GetDesc().CellCountX ; n ; n /= 2 )
			{
				curPow ++;
			}

			const int RESIZE_OFFSET = 9;

			static int resizeTo = curPow - RESIZE_OFFSET;

			sizes.push_back( "256" );
			sizes.push_back( "512" );
			sizes.push_back( "1024" );
			sizes.push_back( "2048" );
			sizes.push_back( "4096" );

			static float offset = 0.f;

			const int RESIZE_HEIGHT = 133;

			imgui_DrawList( SliderX, SliderY, 360, RESIZE_HEIGHT, sizes, &offset, &resizeTo );
			SliderY += RESIZE_HEIGHT;
		}
	}

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_UpDown( float SliderX, float SliderY, int editMode, bool up )
{
	const int EDIT_MODE_SEL = up ? TE3_UP : TE3_DOWN ;

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, up ? "UP" : "DOWN" ) ;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Delta Value", &Terrain3BrushHeightAdjustVal[ EDIT_MODE_SEL ],	0,20,	"%-02.2f",1);

	if( editMode && imgui_lbp )
	{
		if ( ! g_pTerrain3Editor->IsUndoRecord() )
		{
			g_pTerrain3Editor->BeginUndoRecord( up ? "Up terrain" : "Down terrain", UA_TERRAIN3_HEIGHT );
			g_pTerrain3Editor->StartHeightEditing();
		}

		g_EditedTerrain3Data = 1 ;

		if( r3dGetTime() - _LastTimeBrush > TERRAIN3_BRUSH_DELTA )
		{
			float Val = up ? 1.0f : -1.0f ;

			g_pTerrain3Editor->ApplyHeightBrush( UI_TerraTargetPos, Terrain3BrushHeightAdjustVal[ EDIT_MODE_SEL ] * Val * Terrain3BrushStrengthVal[ EDIT_MODE_SEL ], Terrain3BrushRadiusVal[ EDIT_MODE_SEL ], Terrain3BrushHardnessVal[ EDIT_MODE_SEL ] );
			_LastTimeBrush = r3dGetTime();
		}
	}

	if( editMode )
	{
		char msg[ 128 ];
		sprintf( msg, "[%.3f, %.3f, %3.f]", UI_TerraTargetPos.x, UI_TerraTargetPos.y, UI_TerraTargetPos.z );
		SliderY += imgui_Static( SliderX, SliderY, msg );
	}

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_Level( float SliderX, float SliderY, int editMode )
{
	if (Keyboard->IsPressed(kbsLeftAlt)) Terrain3BrushHeightVal[ TE3_LEVEL ] = UI_TerraTargetPos.y;

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "LEVEL");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Height", &Terrain3BrushHeightVal[ TE3_LEVEL ],	0,5000,	"%-02.2f",1);

	if( editMode && imgui_lbp )
	{
		g_EditedTerrain3Data = 1 ;

		if ( ! g_pTerrain3Editor->IsUndoRecord() )
		{				
			g_pTerrain3Editor->BeginUndoRecord( "Set level terrain", UA_TERRAIN3_HEIGHT );
			g_pTerrain3Editor->StartHeightEditing();
		}

		if( r3dGetTime() - _LastTimeBrush > TERRAIN3_BRUSH_DELTA )
		{
			g_pTerrain3Editor->ApplyHeightLevel(UI_TerraTargetPos, Terrain3BrushHeightVal[ TE3_LEVEL ], Terrain3BrushStrengthVal[ TE3_LEVEL ], Terrain3BrushRadiusVal[ TE3_LEVEL ], Terrain3BrushHardnessVal[ TE3_LEVEL ]);
			gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain3BrushRadiusVal[Terrain3EditMode] );
			_LastTimeBrush = r3dGetTime();
		}
	}

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_Smooth( float SliderX, float SliderY, int editMode )
{
	extern int   _terra_smoothBoxOff;
	extern float _terra_smoothSpeed;

	const r3dTerrainDesc& desc = Terrain3->GetDesc() ;

	if( editMode ) 
	{
		// draw box filter size
		r3dPoint3D box[4];
		const float off = _terra_smoothBoxOff * desc.CellSize;
		box[0] = UI_TerraTargetPos + r3dPoint3D(-off, 0, -off);
		box[1] = UI_TerraTargetPos + r3dPoint3D(-off, 0,  off);
		box[2] = UI_TerraTargetPos + r3dPoint3D( off, 0,  off);
		box[3] = UI_TerraTargetPos + r3dPoint3D( off, 0, -off);
		r3dDrawLine3DT(box[0], box[1], gCam, 0.1f, r3dColor(255, 0, 0));
		r3dDrawLine3DT(box[1], box[2], gCam, 0.1f, r3dColor(255, 0, 0));
		r3dDrawLine3DT(box[2], box[3], gCam, 0.1f, r3dColor(255, 0, 0));
		r3dDrawLine3DT(box[3], box[0], gCam, 0.1f, r3dColor(255, 0, 0));
	}

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "SMOOTH");
	SliderY += imgui_Value_SliderI(SliderX, SliderY, "BoxSize/2", &_terra_smoothBoxOff, 1, (int)(Terrain3BrushRadiusVal[TE3_SMOOTH] / desc.CellSize), "%d", 1);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Speed", &_terra_smoothSpeed, 0, 2.0f, "%.1f", 1);

	if( editMode && imgui_lbp) 
	{
		g_EditedTerrain3Data = true;
		if ( ! g_pTerrain3Editor->IsUndoRecord() )
		{				
			g_pTerrain3Editor->BeginUndoRecord( "Smooth terrain", UA_TERRAIN3_HEIGHT );
			g_pTerrain3Editor->StartHeightEditing();
		}

		g_pTerrain3Editor->ApplyHeightSmooth(UI_TerraTargetPos, Terrain3BrushRadiusVal[TE3_SMOOTH], Terrain3BrushHardnessVal[TE3_SMOOTH] );
		gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain3BrushRadiusVal[TE3_SMOOTH] );
		_LastTimeBrush = r3dGetTime();
	}

	return SliderY ;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_Noise( float SliderX, float SliderY, int editMode )
{
	static int painting = 0;

	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "NOISE");

	SliderY += imgui_Value_SliderI(SliderX, SliderY, "Octaves",      &g_pTerrain3Editor->noiseParams.noise.octaves_, 1, 8, "%d", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Persistence",  &g_pTerrain3Editor->noiseParams.noise.persistence_, 0.0001f, 2.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Frequency",    &g_pTerrain3Editor->noiseParams.noise.frequency_, 0.0001f, 1.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Amplitude",    &g_pTerrain3Editor->noiseParams.noise.amplitude_, 0.0001f, 1.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "MinHeight",    &g_pTerrain3Editor->noiseParams.minHeight, -500.0f, 500.0f, "%-02.2f", 1);	  
	SliderY += imgui_Value_Slider (SliderX, SliderY, "MaxHeight",    &g_pTerrain3Editor->noiseParams.maxHeight, -500.0f, 500.0f, "%-02.2f", 1);

	SliderY += 30;

	SliderY += 10;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "MorphSec", &g_pTerrain3Editor->noiseParams.heightApplySpeed, 0.0001f, 2.0f, "%-02.2f", 1 ) ;
	SliderY += imgui_Checkbox( SliderX, SliderY, "HeightRelative", &g_pTerrain3Editor->noiseParams.heightRelative, 1 ) ;

	// start new painting
	if(!painting) 
	{
		if( editMode && imgui_lbp ) 
		{
			painting = true;
			if ( ! g_pTerrain3Editor->IsUndoRecord() )
			{				
				g_pTerrain3Editor->BeginUndoRecord( "Noise terrain", UA_TERRAIN2_HEIGHT );
				g_pTerrain3Editor->StartHeightEditing();
			}
		}
	}

	if(painting) 
	{
		// check for painting stop
		if(!editMode || !imgui_lbp) 
		{
			painting = false;
			return SliderY ;
		}

		g_EditedTerrain3Data = 1 ;
		g_pTerrain3Editor->ApplyHeightNoise( UI_TerraTargetPos, Terrain3BrushRadiusVal[ TE3_NOISE ], Terrain3BrushHardnessVal[ TE3_NOISE ] ) ;
		gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain3BrushRadiusVal[TE3_NOISE] );
	}

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_Ramp( float SliderX, float SliderY, int editMode )
{
	static float rampWidthOuter = 20.0f;
	static float rampWidthInner = 15.0f;

	SliderY += 10;
	SliderY += imgui_Static( SliderX, SliderY, "Ramp Creation" ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "WidthOuter", &rampWidthOuter, 1, 100, "%-02.2f", 1 ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "WidthInner", &rampWidthInner, 1, rampWidthOuter - 0.01f, "%-02.2f", 1 ) ;

	static int ramp_step = 0;
	static r3dPoint3D ramp_pnt[2];

	switch( ramp_step )
	{
	case 0: 
		if( !editMode ) 
			break;

		ramp_pnt[0] = UI_TerraTargetPos;
		r3dDrawCircle3DT(ramp_pnt[0], 4.0f, gCam, 0.5f, r3dColor(0, 255, 0));
		drawRamp(ramp_pnt[0], ramp_pnt[0] + r3dPoint3D(0, 0, rampWidthOuter*2), rampWidthOuter, rampWidthInner);

		if(imgui_lbr) {
			ramp_step++;
		}

		break;

	case 1:
		if( !editMode ) 
		{
			ramp_step = 0;
			break;
		}
		ramp_pnt[1] = UI_TerraTargetPos;

		r3dDrawCircle3DT(ramp_pnt[0], 4.0f, gCam, 0.5f, r3dColor(0, 255, 0));
		r3dDrawCircle3DT(ramp_pnt[1], 4.0f, gCam, 0.5f, r3dColor(0, 0, 255));
		drawRamp(ramp_pnt[0], ramp_pnt[1], rampWidthOuter, rampWidthInner);

		if(imgui_lbr) 
		{
			g_EditedTerrain3Data = true;
			if ( ! g_pTerrain3Editor->IsUndoRecord() )
			{				
				g_pTerrain3Editor->BeginUndoRecord( "Create ramp", UA_TERRAIN3_HEIGHT );
				g_pTerrain3Editor->StartHeightEditing();
			}
			g_pTerrain3Editor->ApplyHeightRamp(ramp_pnt[0], ramp_pnt[1], rampWidthOuter, rampWidthInner);
			ramp_step = 0;
		}
		break;
	}

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_Erosion( float SliderX, float SliderY, int editMode )
{
	if( Keyboard->IsPressed( kbsLeftAlt ) ) 
		Terrain3BrushHeightVal[ TE3_EROSION ] = UI_TerraTargetPos.y;

	{
		SliderY += 10;
		SliderY += imgui_Static(SliderX, SliderY, "Erosion pattern list: ");
		static int patternTexID = 0;
		static float patternIDOffset = 0;
		int prevID = patternTexID;
		imgui_DrawList( SliderX, SliderY, 360, 200, gErosionPatternTextureNames, &patternIDOffset, &patternTexID, true, false, true );

		if (prevID != patternTexID || !gErosionPattern)
		{
			r3dSTLString path = "data\\Editor\\ErosionPatterns\\" + gErosionPatternTextureNames[patternTexID];
			InitErosionPattern(path.c_str());
		}
	}

	if( editMode && imgui_lbp )
	{
		g_EditedTerrain3Data = 1 ;

		if ( ! g_pTerrain3Editor->IsUndoRecord() )
		{				
			g_pTerrain3Editor->BeginUndoRecord( "Set level terrain", UA_TERRAIN3_HEIGHT ) ;
			g_pTerrain3Editor->StartHeightEditing();
		}

		if( r3dGetTime() - _LastTimeBrush > 0.05 )
		{			
			g_pTerrain3Editor->ApplyHeightErosion(UI_TerraTargetPos, Terrain3BrushStrengthVal[ TE3_EROSION ], Terrain3BrushRadiusVal[ TE3_EROSION ], Terrain3BrushHardnessVal[ TE3_EROSION ] ) ;
			gTerrainHeightDirtiness.Add( UI_TerraTargetPos.x, UI_TerraTargetPos.z, Terrain3BrushRadiusVal[ TE3_EROSION ] ) ;
			_LastTimeBrush = r3dGetTime();
		}
	}

	return SliderY;
}

//------------------------------------------------------------------------

template< int N >
void ExtractDirName( char ( &buff )[ N ], const char* src )
{
	char drive[ 16 ], split_path[ 512 ], file[ 256 ], ext[ 32 ];

	_splitpath( src, drive, split_path, file, ext );

	strcpy( buff, split_path );	
}

void FixDirSlashes( char * MatDirName )
{
	for( size_t i = 0, e = strlen( MatDirName ); i < e; i ++ )
	{
		if( MatDirName[ i ] == '/' )
		{
			MatDirName[ i ] = '\\';
		}
	}
}

float Editor_Level :: ProcessTerrain3_Paint( float SliderX, float SliderY, int editMode )
{
	SliderY += 10;
	SliderY += imgui_Static(SliderX, SliderY, "PAINT");

	static int CurrentPaintLayerIdx3 = 0 ;

	if( CurrentPaintLayerIdx3 >= Terrain3->GetDesc().LayerCount )
	{
		// delete through undo happened
		CurrentPaintLayerIdx3 = 0 ;
	}

	SliderY += imgui_Toolbar(SliderX, SliderY, 360, 45, &Terrain3PaintMode, 0, terrain3PaintList, sizeof terrain3PaintList / sizeof terrain3PaintList[ 0 ] );

	SliderY += imgui_Checkbox( SliderX, SliderY, "Show Material Heaviness", &DrawTerrainMatHeaviness, 1 );

	SliderY += 11;

	SliderY = DrawPaintBounds( SliderX, SliderY ) + 10;

	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	int needUpdateTilesWithCurLayer = 0 ;

	// + 1 for base layer
	r3dTexture **MatTexArray		[ r3dTerrain3::MAX_TEXTURE_LAYERS + 1 ];
	r3dTexture **MatTexNormalArray	[ r3dTerrain3::MAX_TEXTURE_LAYERS + 1 ];
	float	   *MatScaleArray		[ r3dTerrain3::MAX_TEXTURE_LAYERS + 1 ];
	float	   *MatNormScaleArray	[ r3dTerrain3::MAX_TEXTURE_LAYERS + 1 ];

	static r3dTL::TArray< r3dTerrain3Layer > layers ;

	layers.Resize( desc.LayerCount ) ;

	for( int i = 0, e = desc.LayerCount ; i < e; i ++ )
	{
		r3dTerrain3Layer& lay = layers[ i ];

		lay = Terrain3->GetLayer( i );

		MatTexArray[ i ]		= &lay.DiffuseTex;
		MatTexNormalArray[ i ]	= &lay.NormalTex;
		MatScaleArray[ i ]		= &lay.ScaleU;
		MatNormScaleArray[ i ]	= &lay.NormUVMult;
	}

	if( Terrain3PaintMode == TE3_ERASE )
	{
		SliderY += imgui_Checkbox( SliderX, SliderY, 360.0f, 22.f, "Erase All", &Terrain3Editor_EraseAll, 1 );
	}

	bool bChangedVal = false;

	r3dTerrain3Layer* CurrentLayer = NULL ;

	int insertLayerAt = - 1;
	int removeLayerIdx = - 1;
	int swapLayerIdx = -1;

	#define TERRAIN_MAT_DIR_NAME "Data\\TerrainData\\Materials\\"

	static char MatFName[64];
	static char MatDirName[64] = { TERRAIN_MAT_DIR_NAME };

	static float matDirNameOffset = 0.f;
	static int matDirCurSel = 0;

	int bChangedPath = 0;
	char newPath[ 512 ] = { };

	if( Terrain3PaintMode != TE3_ERASE || !Terrain3Editor_EraseAll )
	{
		SliderY += imgui_Static( SliderX, SliderY, "Layers" );

		for( int i = 0, e = desc.LayerCount; i < e; i ++ )
		{
			if( imgui_Button( SliderX + 10, SliderY+1, 250, 20, layers[ i ].Name.c_str(), i == CurrentPaintLayerIdx3 ) )
			{
				CurrentPaintLayerIdx3 = i;
				bChangedVal = true;
				bChangedPath = 1;
			}

			if( i == desc.LayerCount - 1 )
			{
				if( imgui_Button( SliderX + 265, SliderY + 1, 33, 20, "+" ) )
				{
					insertLayerAt = i ;
				}
			}

#if 1
			if( i )
			{
				if( imgui_Button( SliderX + 265 + 37, SliderY + 1, 22, 20, "-" ) )
				{
					if( insertLayerAt < 0 )
					{
						removeLayerIdx = i;
					}
				}
			}

			if( i > 1 )
			{
				if( imgui_Button( SliderX + 265 + 66, SliderY + 1, 22, 20, "^" ) )
				{
					if( insertLayerAt < 0 && removeLayerIdx < 0 )
					{
						swapLayerIdx = i;
					}
				}
			}
#endif

			if( CurrentPaintLayerIdx3 >= 0 && CurrentPaintLayerIdx3 < desc.LayerCount )
			{
				CurrentLayer = &layers[ CurrentPaintLayerIdx3 ] ;
			}
			else
			{
				CurrentLayer = NULL;
			}

			SliderY += 22;
		}

		SliderY += 11;

		static char FullName[ 256 ];

		if( CurrentLayer )
		{
			r3dscpy( FullName, CurrentLayer->Name.c_str() );
		}
		else
		{
			r3dscpy( FullName, "Dummy Layer" ) ;
		}

		// skip name setup for base layer
		if( CurrentLayer && CurrentPaintLayerIdx3 )
		{
			imgui2_StringValueEx( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, "Layer Name: ", FullName ) ;

			CurrentLayer->Name = FullName ;

			SliderY += 22 ;
		}

		float prevTileFactor;
		prevTileFactor = *MatScaleArray[ CurrentPaintLayerIdx3 ];

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Tile Factor",	MatScaleArray[ CurrentPaintLayerIdx3 ], 1, 4096, "%-02.0f" );

		float prevNormalScale;
		prevNormalScale = *MatNormScaleArray[ CurrentPaintLayerIdx3 ];
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Normal Tile Multiplier",	MatNormScaleArray[ CurrentPaintLayerIdx3 ], 1.0f / 32.0f, 32.0f, "%.2f" );

		if( CurrentLayer )
		{
			float prevSpecPow = CurrentLayer->SpecularPow;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Spec Pow", &CurrentLayer->SpecularPow, 0.f, 1.f, "%-02.2f" );

			if( prevSpecPow != CurrentLayer->SpecularPow )
			{
				Terrain3->UpdateTilesWithLayer( CurrentPaintLayerIdx3 );
			}
		}

		if( prevTileFactor != *MatScaleArray[ CurrentPaintLayerIdx3 ] )
		{
			needUpdateTilesWithCurLayer = 1;
		}

		if( prevNormalScale != *MatNormScaleArray[ CurrentPaintLayerIdx3 ] )
		{
			needUpdateTilesWithCurLayer = 1;
		}

		r3dDrawBox2D(SliderX - Desktop().GetX(),SliderY - Desktop().GetY(), 360, 120, imgui_bkgDlg);

		if ( imgui_Button(SliderX+10,SliderY+10, 100, 100, NULL, g_iTerrainNormalMapEditMode == 0 ) )
		{
			g_iTerrainNormalMapEditMode = 0;
			bChangedVal = true;
			bChangedPath = 1;
		}

		r3dDrawBox2D(SliderX+10+5 - Desktop().GetX(),SliderY+10+5- Desktop().GetY(), 100-8, 100-8, r3dColor(255,255,255), *MatTexArray[ CurrentPaintLayerIdx3 ]);

		if ( imgui_Button(SliderX+10+110,SliderY+10, 100, 100, NULL, g_iTerrainNormalMapEditMode == 1 ) )
		{
			g_iTerrainNormalMapEditMode = 1;
			bChangedVal = true;
			bChangedPath = 1;
		}

		if( CurrentPaintLayerIdx3 >= 0 )
		{
			if( bChangedPath )
			{
				if( r3dTexture* tex = *( g_iTerrainNormalMapEditMode ? MatTexNormalArray : MatTexArray ) [ CurrentPaintLayerIdx3 ] )
				{
					strcpy( newPath, tex->getFileLoc().FileName );
				}
				else
				{
					strcpy( newPath, TERRAIN_MAT_DIR_NAME );
				}
			}

			if( r3dTexture* tex = *( g_iTerrainNormalMapEditMode ? MatTexNormalArray : MatTexArray ) [ CurrentPaintLayerIdx3 ] )
			{
				float fStatisticYStart = r3dRenderer->ScreenH - 200;
				float fStatisticHeight = 80;

				char sStatStr[ MAX_PATH * 2 ];

				strcpy( sStatStr, tex->getFileLoc().FileName );
				strlwr( sStatStr );

				for( size_t i = 0, e = strlen( sStatStr ) ; i < e; i ++ )
				{
					if( sStatStr[ i ] == '/' )
						sStatStr[ i ] = '\\' ;
				}

				int toAdd = 0 ;

				if( char* subs = strstr( sStatStr, "data\\terraindata\\materials"  ) )
				{
					toAdd = sizeof "data\\terraindata\\materials" ;
				}

				sprintf( sStatStr, "%s texture\n%s\n%dx%d", g_iTerrainNormalMapEditMode ? "Normal" : "Diffuse", tex->getFileLoc().FileName + toAdd, tex->GetWidth(), tex->GetHeight() );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

				imgui_Static ( 370, fStatisticYStart, sStatStr, 360, false, fStatisticHeight );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) );

				if (imgui_Button(SliderX+10+2+110+106,SliderY+42+2, 60, 20, "EDIT", 0))
				{
#ifndef FINAL_BUILD
					const char* name = tex->getFileLoc().FileName ;

					char fullPath[ 4096 ] ;
					GetCurrentDirectory( sizeof fullPath - 2, fullPath ) ;
					strcat( fullPath, "\\" ) ;
					strcat( fullPath, name ) ;

					for( size_t i = 0, e = strlen( fullPath) ; i < e ; i ++ )
					{
						if( fullPath[ i ] == '/' )
							fullPath[ i ] = '\\' ;
					}

					const char* execStr = g_texture_edit_cmd->GetString() ;

					ShellExecute( NULL, "open", execStr, fullPath, NULL, SW_SHOWNORMAL ) ;
#endif
				}
			}
		}

		r3dDrawBox2D(SliderX+10+5+110-Desktop().GetX(),SliderY+10+5- Desktop().GetY(), 100-8, 100-8, r3dColor(255,255,255), *MatTexNormalArray[ CurrentPaintLayerIdx3 ]);

		SliderY += 122 ;

		if ( CurrentPaintLayerIdx3 >= 0 )
		{
			r3dTexture * pTex = g_iTerrainNormalMapEditMode ? *MatTexNormalArray[ CurrentPaintLayerIdx3 ] : *MatTexArray[ CurrentPaintLayerIdx3 ];
			if ( pTex )
			{
				FixedString s( pTex->getFileLoc().FileName );
				FixedString sName = s.GetName() + s.GetExt();
				r3dscpy( MatFName, sName.c_str() );
			}
		}

		if( CurrentLayer )
		{
			char MatTypeName[ 2048 ] ;

			strncpy( MatTypeName, CurrentLayer->MaterialTypeName.c_str(), sizeof MatTypeName - 1 ) ;

			SliderY = DrawMaterialTypeSelection( SliderX, SliderY, MatTypeName );

			if( strcmp( MatTypeName, CurrentLayer->MaterialTypeName.c_str() ) != 0 )
			{
				CurrentLayer->MaterialTypeName = MatTypeName;
				Terrain3->RecalcLayerVars();
			}
		}
	}

	g_pDesktopManager->End();

	static float matSelOffset = 0.f ;

	if( CurrentLayer )
	{
		const float DIR_LIST_HEIGHT = 180.f;

		const float DIR_LIST_START = 120.f;

		if( bChangedPath )
		{
			ExtractDirName( MatDirName, newPath );
		}

		FixDirSlashes( MatDirName );

		if( imgui_DirList( 5, DIR_LIST_START, 300, DIR_LIST_HEIGHT, TERRAIN_MAT_DIR_NAME, ".dds|.bmp|.tga", MatDirName, &matDirNameOffset, &matDirCurSel, true, false, bChangedPath ? true : false, 0, 1 ) )
		{
		}

		const float FILE_LIST_HEIGHT = R3D_MIN( r3dRenderer->ScreenH - DIR_LIST_START - DIR_LIST_HEIGHT - 200.f, 380.f );

		char DirWithWCard[ 512 ];

		strcpy( DirWithWCard, MatDirName );
		strcat( DirWithWCard, "\\*.*" );

		if( imgui_FileList( 5, DIR_LIST_START + DIR_LIST_HEIGHT + 5, 300, FILE_LIST_HEIGHT, DirWithWCard, MatFName, &matSelOffset, false, bChangedVal, 0, ".dds|.tga|.bmp" ) )
		{
			char SS1[256];

			needUpdateTilesWithCurLayer = 1 ;

			if( MatDirName[ strlen( MatDirName ) - 1 ] == '\\' )
			{
				sprintf( SS1, "%s%s", MatDirName, MatFName );
			}
			else
			{
				sprintf( SS1, "%s\\%s", MatDirName, MatFName );
			}

			switch( g_iTerrainNormalMapEditMode )
			{
			case 1:
				r3dRenderer->DeleteTexture(*MatTexNormalArray[ CurrentPaintLayerIdx3 ]);
				*MatTexNormalArray[ CurrentPaintLayerIdx3 ] = r3dRenderer->LoadTexture(SS1);

				break ;

			case 0:
				r3dRenderer->DeleteTexture(*MatTexArray[ CurrentPaintLayerIdx3 ]);
				*MatTexArray[ CurrentPaintLayerIdx3 ] = r3dRenderer->LoadTexture(SS1);

				break ;
			}
		}

		CurrentLayer->ScaleV = CurrentLayer->ScaleU ;

		Terrain3->SetLayer( CurrentPaintLayerIdx3, *CurrentLayer ) ;

		if( needUpdateTilesWithCurLayer )
		{
			Terrain3->UpdateTilesWithLayer( CurrentPaintLayerIdx3 ) ;
		}
	}

	g_pDesktopManager->Begin( "ed_terrain" ) ;

	if ( editMode && imgui_lbp )
	{
		if( Terrain3Editor_EraseAll )
		{
			g_EditedTerrain3Data = 1 ;

			if( !g_pTerrain3Editor->IsUndoRecord() )
			{				
				g_pTerrain3Editor->BeginUndoRecord( "Terrain3 Erase All Layers", UA_TERRAIN3_MASK_ERASEALL );
				g_pTerrain3Editor->StartEraseAllBrush();
			}

			if( r3dGetTime() - _LastTimeBrush > 0.025 )
			{
				g_pTerrain3Editor->ApplyEraseAllBrush( g_TerraPaintBoundCtrl, UI_TerraTargetPos, 255.f * Terrain3BrushStrengthVal[Terrain3EditMode], Terrain3BrushRadiusVal[Terrain3EditMode], Terrain3BrushHardnessVal[Terrain3EditMode] ) ;

				_LastTimeBrush = r3dGetTime();
			}
		}
		else
		{
			if ( CurrentPaintLayerIdx3 )
			{
				g_EditedTerrain3Data = 1;

				if( ! g_pTerrain3Editor->IsUndoRecord() )
				{
					g_pTerrain3Editor->BeginUndoRecord( Terrain3PaintMode ? "Paint mask" : "Erase mask", UA_TERRAIN3_MASK_PAINT );

					IUndoItem* undoItem = g_pTerrain3Editor->GetUndoRecord();

					r3d_assert( undoItem );

					CLayerMaskPaint3 * pUndoItem = static_cast< CLayerMaskPaint3 * > ( undoItem );

					pUndoItem->LayerIdx = CurrentPaintLayerIdx3;

					g_pTerrain3Editor->StartLayerBrush_G( CurrentPaintLayerIdx3 );
				}

				if( r3dGetTime() - _LastTimeBrush > 0.025 )
				{
					g_pTerrain3Editor->ApplyLayerBrush_G( g_TerraPaintBoundCtrl, UI_TerraTargetPos, Terrain3PaintMode, CurrentPaintLayerIdx3, 255.f * Terrain3BrushStrengthVal[Terrain3EditMode], Terrain3BrushRadiusVal[Terrain3EditMode], Terrain3BrushHardnessVal[Terrain3EditMode] ) ;
					_LastTimeBrush = r3dGetTime();
				}
			}
		}
	}

	if( imgui_Button( SliderX, SliderY, 360, 22, "Optimize Layer Masks" ) )
	{
		g_pTerrain3Editor->OptimizeLayerMasks();
	}

	SliderY += 22.0f;

	int needResetSelection = 0 ;

	if( insertLayerAt >= 0 )
	{
		needResetSelection = 1 ;
		Terrain3->InsertLayer( insertLayerAt, NULL, NULL, NULL, NULL, NULL ) ;
	}
	else
	if( removeLayerIdx >= 0 )
	{
		needResetSelection = 1 ;

		if( MessageBox( r3dRenderer->HLibWin, "This operation can not be undone. Terrain will be saved in the process. This operation may take several minutes to complete. Proceed?", "Warning", MB_YESNO ) == IDYES )
		{
			g_pTerrain3Editor->DeleteLayer( removeLayerIdx );
		}
	}
	else
	if( swapLayerIdx > 1 )
	{
		needResetSelection = 1;

		if( MessageBox( r3dRenderer->HLibWin, "This operation can not be undone and may take several minutes to complete. Proceed?", "Warning", MB_YESNO ) == IDYES )
		{
			g_pTerrain3Editor->SwapLayers( swapLayerIdx - 1, swapLayerIdx - 2 );
		}
	}

	if( needResetSelection )
	{
		CurrentLayer = NULL ;
		CurrentPaintLayerIdx3 = 0 ;
	}

	SliderY += 22;

	return SliderY;

}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_GlobalLayer( float SliderX, float SliderY, int editMode )
{
	r3dTerrain3::Settings sts = Terrain3->GetSettings();

	SliderY += imgui_Checkbox( SliderX, SliderY, "Far Tile Enabled", &sts.FarLayerEnabled, 1 );
	sts.FarLayerEnabled = !!sts.FarLayerEnabled;

	//------------------------------------------------------------------------

#define TERRAIN_MAT_DIR_NAME "Data\\TerrainData\\Materials\\"

	if( sts.FarLayerEnabled )
	{
		int bChangedPath = 0;
		int bChangedVal = 0;

		char newPath[ 512 ] = { };

		SliderY += imgui_Static( SliderX, SliderY, "  Diffuse                 Normal" );

		static char MatFName[64];
		static char MatDirName[64] = { TERRAIN_MAT_DIR_NAME };

		//------------------------------------------------------------------------

		r3dDrawBox2D(SliderX - Desktop().GetX(),SliderY - Desktop().GetY(), 360, 120, imgui_bkgDlg);

		if ( imgui_Button(SliderX+10,SliderY+10, 100, 100, NULL, 1 - g_iTerrainGlobalNormalMapEditMode ) )
		{
			g_iTerrainGlobalNormalMapEditMode = 0;
			bChangedVal = true;
			bChangedPath = 1;
		}

		r3dDrawBox2D(SliderX+10+5 - Desktop().GetX(),SliderY+10+5- Desktop().GetY(), 100-8, 100-8, r3dColor(255,255,255), sts.FarDiffuseTexture );

		if ( imgui_Button(SliderX+10+110,SliderY+10, 100, 100, NULL, g_iTerrainGlobalNormalMapEditMode ) )
		{
			g_iTerrainGlobalNormalMapEditMode = 1;
			bChangedVal = true;
			bChangedPath = 1;
		}

		r3dDrawBox2D(SliderX+10+5 + 110 - Desktop().GetX(),SliderY+10+5- Desktop().GetY(), 100-8, 100-8, r3dColor(255,255,255), sts.FarNormalTexture );

		if( imgui_Button( SliderX + 227.f, SliderY + 11.f, 122.f, 22.f, "Clear" ) )
		{
			sts.FarDiffuseTextureName = "";
			sts.FarNormalTextureName = "";
		}

		SliderY += 121.0f;

		if( bChangedPath )
		{
			if( g_iTerrainGlobalNormalMapEditMode )
			{
				strcpy( newPath, sts.FarNormalTextureName.c_str() );
			}
			else
			{
				strcpy( newPath, sts.FarDiffuseTextureName.c_str() );
			}
		}

		if( sts.FarDiffuseTextureName.Length() > 0 )
		{
			char buff[ 128 ];
			char drive[ 32 ], path[ 512 ], fileName[ 128 ], ext[ 32 ];

			_splitpath( sts.FarDiffuseTextureName.c_str(), drive, path, fileName, ext );
			sprintf( buff, "Diffuse Texture: %s.%s", fileName, ext );
			SliderY += imgui_Static( SliderX, SliderY, buff );
		}

		if( sts.FarNormalTextureName.Length() > 0 )
		{
			char buff[ 128 ];
			char drive[ 32 ], path[ 512 ], fileName[ 128 ], ext[ 32 ];

			_splitpath( sts.FarNormalTextureName.c_str(), drive, path, fileName, ext );
			sprintf( buff, "Diffuse Texture: %s.%s", fileName, ext );
			SliderY += imgui_Static( SliderX, SliderY, buff );
		}

		//------------------------------------------------------------------------
		g_pDesktopManager->End();
		{
			char drive[32], path[512], fileName[128], ext[32];

			switch( g_iTerrainGlobalNormalMapEditMode )
			{
			case 0:
				_splitpath( sts.FarDiffuseTextureName.c_str(), drive, path, fileName, ext );
				if( bChangedPath )
				{
					strcpy( MatDirName, path );
				}

				strcpy( MatFName, fileName );
				strcat( MatFName, ext );
				break;

			case 1:
				_splitpath( sts.FarNormalTextureName.c_str(), drive, path, fileName, ext );

				if( bChangedPath )
				{
					strcpy( MatDirName, path );
				}

				strcpy( MatFName, fileName );
				strcat( MatFName, ext );
				break;
			};

			if( !strlen( MatDirName ) )
			{
				strcpy( MatDirName, TERRAIN_MAT_DIR_NAME );
			}
		}
		//------------------------------------------------------------------------

		static float matSelOffset = 0.0f;

		static float matDirNameOffset = 0.f;
		static int matDirCurSel = 0;

		const float DIR_LIST_HEIGHT = 180.f;

		const float DIR_LIST_START = 120.f;

		if( bChangedPath )
		{
			ExtractDirName( MatDirName, newPath );
		}

		FixDirSlashes( MatDirName );

		if( imgui_DirList( 5, DIR_LIST_START, 300, DIR_LIST_HEIGHT, TERRAIN_MAT_DIR_NAME, ".dds|.bmp|.tga", MatDirName, &matDirNameOffset, &matDirCurSel, true, false, bChangedPath ? true : false, 0, 1 ) )
		{
		}

		const float FILE_LIST_HEIGHT = R3D_MIN( r3dRenderer->ScreenH - DIR_LIST_START - DIR_LIST_HEIGHT - 200.f, 380.f );

		char DirWithWCard[ 512 ];

		strcpy( DirWithWCard, MatDirName );
		strcat( DirWithWCard, "\\*.*" );

		if( imgui_FileList( 5, DIR_LIST_START + DIR_LIST_HEIGHT + 5, 300, FILE_LIST_HEIGHT, DirWithWCard, MatFName, &matSelOffset, false, bChangedVal, 0, ".dds|.tga|.bmp" ) )
		{
			char SS1[256];

			if( MatDirName[ strlen( MatDirName ) - 1 ] == '\\' )
			{
				sprintf( SS1, "%s%s", MatDirName, MatFName );
			}
			else
			{
				sprintf( SS1, "%s\\%s", MatDirName, MatFName );
			}

			switch( g_iTerrainGlobalNormalMapEditMode )
			{
			case 1:
				sts.FarNormalTextureName = SS1;
				break ;

			case 0:
				sts.FarDiffuseTextureName = SS1;
				break ;
			}
		}

		g_pDesktopManager->Begin( "ed_terrain" );

		//------------------------------------------------------------------------

		static float fade_start, fade_end;

		fade_start = sts.FarFadeStart;
		fade_end = sts.FarFadeEnd;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Fade Start", &fade_start, 0.f, 16384.f, "%.2f" );
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Fade End", &fade_end, 0.f, 16384.f, "%.2f" );

		sts.FarFadeStart = fade_start;
		sts.FarFadeEnd = fade_end;
	}

	Terrain3->SetSettings( sts );

	if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Reset tile allocations" ) )
	{
		Terrain3->RefreshAtlasTilesAndFreeAtlas();
	}

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_ColorPaint( float SliderX, float SliderY, int editMode )
{
	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_HMap( float SliderX, float SliderY )
{
	SliderY += 20;
	SliderY += imgui_Static( SliderX, SliderY, "Available Heightmaps" ) ;

	const r3dTerrainDesc& desc = Terrain3->GetDesc() ;

	static char HMapName[256];
	static char Path[256];

	float HRange = desc.MaxHeight - desc.MinHeight;

	static float terraYScale = HRange;
	static float terraYOffset = desc.MinHeight;

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Height scale", &terraYScale, 0, HRange, "%-02.2f", 1 ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Height offset", &terraYOffset, 0, HRange, "%-02.2f", 1 ) ;

	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	static float cellOffsetX = 0.f;
	static float cellOffsetZ = 0.f;

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Import Offset X", &cellOffsetX, 0, tdesc.CellCountX - 1, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Import Offset Z", &cellOffsetZ, 0, tdesc.CellCountZ - 1, "%-02.0f", 1 );

	cellOffsetX = (int)cellOffsetX;
	cellOffsetZ = (int)cellOffsetZ;

	static float offset = 0.f ;

	if (imgui_FileList(SliderX, SliderY, 360, 200, "Data\\TerrainData\\Heightmaps\\*.dds", HMapName, &offset ))
	{
		sprintf(Path,"Data\\TerrainData\\Heightmaps\\%s", HMapName);
	}

	SliderY += 206;

	if (imgui_Button(SliderX, SliderY, 360, 25, "Import heightmap", 0)) 
	{
		g_pTerrain3Editor->ImportHeight( Path, terraYScale, terraYOffset, (int)cellOffsetX, (int)cellOffsetZ ) ;
		g_pGrassMap->UpdateHeight();

		RepositionAllObjectsOnTerrain();

		MessageBoxA( r3dRenderer->HLibWin, "Imported!", "Success", MB_OK );
	}

	SliderY += 25.0f;

	static float exportOffsetX = 0.f;
	static float exportOffsetZ = 0.f;
	static float exportSizeX = 0.f;
	static float exportSizeZ = 0.f;

	if( !exportSizeX )
	{
		exportSizeX = R3D_MIN( tdesc.CellCountX, 4096 );
		exportSizeZ = R3D_MIN( tdesc.CellCountZ, 4096 );
	}

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Offset X", &exportOffsetX, 0, tdesc.CellCountX - 1, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Offset Z", &exportOffsetZ, 0, tdesc.CellCountZ - 1, "%-02.0f", 1 );

	int maxSizeX = int( tdesc.CellCountX - exportOffsetX );
	int maxSizeZ = int( tdesc.CellCountZ - exportOffsetZ );

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Size X", &exportSizeX, 1.0f, maxSizeX, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Size Z", &exportSizeZ, 1.0f, maxSizeZ, "%-02.0f", 1 );

	exportOffsetX = (int)exportOffsetX;
	exportOffsetZ = (int)exportOffsetZ;
	exportSizeX = (int)exportSizeX;
	exportSizeZ = (int)exportSizeZ;

	if( imgui_Button( SliderX, SliderY, 360, 25, "Export heightmap", 0 ) )
	{
		char drive[ 16 ], folder[ 1024 ] = "", name[ 1024], ext[ 256 ] ;
		_splitpath( LevelEditName, drive, folder, name, ext ) ;

		for( size_t i = 0, e = strlen( folder ) ; i < e; i ++ )
		{
			char &ch = folder[ i ] ;
			if( ch == '\\' ||ch == '/' )
			{
				ch = '_' ;
			}
		}

		sprintf( Path, "Data/TerrainData/Heightmaps/%4s_%s_(%d,%d)_%dx%d_exp.dds", folder, name, (int)exportOffsetX, (int)exportOffsetZ, (int)exportSizeX, (int)exportSizeZ );

		if( g_pTerrain3Editor->ExportHeight( Path, (int)exportOffsetX, (int)exportOffsetZ, (int)exportSizeX, (int)exportSizeZ ) )
			MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Saved to " ) + Path ).c_str(), "Information", MB_OK ) ;
		else
			MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Failed to save to " ) + Path ).c_str(), "Information", MB_ICONERROR ) ;

	}

	SliderY += 35;

	if( imgui_Button( SliderX, SliderY, 360, 22, "Recalc Normal Edges" ) )
	{
		g_pTerrain3Editor->RecalcNormalEdges();
		MessageBoxA( r3dRenderer->HLibWin, "Recalculated!", "Information", MB_OK ) ;
	}

	SliderY += 35;

	if( imgui_Button( SliderX, SliderY, 360, 22, "Recalc Height Mips" ) )
	{
		g_pTerrain3Editor->RecalcAllHeightMipMaps();
		MessageBoxA( r3dRenderer->HLibWin, "Recalculated!", "Information", MB_OK ) ;
	}

	SliderY += 35;

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_ImportMask( float SliderX, float SliderY )
{
	static int CurrentImportLayerIdx3 = 0;

	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Optimize Layer Masks" ) )
	{
		g_pTerrain3Editor->OptimizeLayerMasks();
	}

	SliderY += 33.f;

	if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Regenerate Mask Mips" ) )
	{
		g_pTerrain3Editor->RecalcAllMasksMipMaps();
	}

	SliderY += 33.f;

	if( Terrain3PaintMode != TE3_ERASE || !Terrain3Editor_EraseAll )
	{
		SliderY += imgui_Static( SliderX, SliderY, "Layers" );

		for( int i = 1, e = desc.LayerCount; i < e; i ++ )
		{
			if( imgui_Button( SliderX + 10, SliderY+1, 250, 20, Terrain3->GetLayer( i ).Name.c_str(), i == CurrentImportLayerIdx3 ) )
			{
				CurrentImportLayerIdx3 = i;
			}

			SliderY += 22;
		}
	}

	//------------------------------------------------------------------------

	SliderY += 20;
	SliderY += imgui_Static( SliderX, SliderY, "Available Masks" ) ;

	static char MaskName[512];
	static char Path[256];

	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	static float cellOffsetX = 0.f;
	static float cellOffsetZ = 0.f;

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Import Offset X", &cellOffsetX, 0, tdesc.CellCountX - 1, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Import Offset Z", &cellOffsetZ, 0, tdesc.CellCountZ - 1, "%-02.0f", 1 );

	cellOffsetX = (int)cellOffsetX;
	cellOffsetZ = (int)cellOffsetZ;

	static float offset = 0.f ;

	if( imgui_FileList(SliderX, SliderY, 360, 200, "Data\\TerrainData\\Masks\\*.dds", MaskName, &offset ) )
	{
		sprintf( Path, "Data\\TerrainData\\Masks\\%s", MaskName );
	}

	SliderY += 206;

	if( CurrentImportLayerIdx3 > 0 && CurrentImportLayerIdx3 < desc.LayerCount && strlen( Path ) )
	{
		if( imgui_Button( SliderX, SliderY, 360, 25, "Import Layer Mask", 0 ) ) 
		{
			g_pTerrain3Editor->ImportLayer( Path, CurrentImportLayerIdx3 - 1, (int)cellOffsetX, (int)cellOffsetZ ) ;
		}
	}

	SliderY += 25.0f;

	static float exportOffsetX = 0.f;
	static float exportOffsetZ = 0.f;
	static float exportSizeX = 0.f;
	static float exportSizeZ = 0.f;

	if( !exportSizeX )
	{
		exportSizeX = R3D_MIN( tdesc.CellCountX, 4096 );
		exportSizeZ = R3D_MIN( tdesc.CellCountZ, 4096 );
	}

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Offset X", &exportOffsetX, 0, tdesc.MaskResolutionU - 1, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Offset Z", &exportOffsetZ, 0, tdesc.MaskResolutionV - 1, "%-02.0f", 1 );

	int maxSizeX = int( tdesc.CellCountX - exportOffsetX );
	int maxSizeZ = int( tdesc.CellCountZ - exportOffsetZ );

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Size X", &exportSizeX, 1.0f, maxSizeX, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Size Z", &exportSizeZ, 1.0f, maxSizeZ, "%-02.0f", 1 );

	exportOffsetX = (int)exportOffsetX;
	exportOffsetZ = (int)exportOffsetZ;
	exportSizeX = (int)exportSizeX;
	exportSizeZ = (int)exportSizeZ;

	if( imgui_Button( SliderX, SliderY, 360, 25, "Export Layer Mask", 0 ) )
	{
		if( CurrentImportLayerIdx3 >= 0 && CurrentImportLayerIdx3 < desc.LayerCount )
		{
			char drive[ 16 ], folder[ 1024 ] = "", name[ 1024], ext[ 256 ] ;
			_splitpath( LevelEditName, drive, folder, name, ext ) ;

			for( size_t i = 0, e = strlen( folder ) ; i < e; i ++ )
			{
				char &ch = folder[ i ] ;
				if( ch == '\\' ||ch == '/' )
				{
					ch = '_' ;
				}
			}

			sprintf( Path, "Data/TerrainData/Masks/%s%s_%d_mask.dds", folder, name, CurrentImportLayerIdx3 );

			if( g_pTerrain3Editor->ExportLayer( Path, CurrentImportLayerIdx3 - 1, (int)exportOffsetX, (int)exportOffsetZ, (int)exportSizeX, (int)exportSizeZ ) )
				MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Saved to " ) + Path ).c_str(), "Information", MB_OK ) ;
			else
				MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Failed to save to " ) + Path ).c_str(), "Information", MB_ICONERROR ) ;
		}
	}

	SliderY += 35;

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level :: ProcessTerrain3_Debug( float SliderX, float SliderY )
{
	int showTiles = !!r_terrain2_show_tiles->GetInt();
	SliderY += imgui_Checkbox( SliderX, SliderY, "Show Tiles", &showTiles, 1 );
	r_terrain2_show_tiles->SetInt( !!showTiles );

	int forceMinLod = !!r_force_min_terramegalod->GetInt();
	SliderY += imgui_Checkbox( SliderX, SliderY, "Force LOD", &forceMinLod, 1 );
	r_force_min_terramegalod->SetInt( !!forceMinLod );

	char debugOffsetText[ 512 ];
	sprintf( debugOffsetText, "MegaTile Min LOD: %d", r_min_terramegalod->GetInt() );

	SliderY += imgui_Static( SliderX, SliderY, debugOffsetText );

	r3dTerrain3::Info info = Terrain3->GetInfo();

	if( r_min_terramegalod->GetInt() > 0 )
	{
		if( imgui_Button( SliderX + 180.f, SliderY, 178.f, 22.f, "-" ) )
		{
			r_min_terramegalod->SetInt( r_min_terramegalod->GetInt() - 1 );
		}
	}

	if( r_min_terramegalod->GetInt() < info.NumActiveMegaTexLayers - 1 )
	{
		if( imgui_Button( SliderX, SliderY, 178.f, 22.f, "+" ) )
		{
			r_min_terramegalod->SetInt( r_min_terramegalod->GetInt() + 1 );
		}
	}

	SliderY += 24.f;

	return SliderY;
}

//------------------------------------------------------------------------

void Editor_Level :: ProcessTerrain3()
{
	R3DPROFILE_FUNCTION("Editor_Level::ProcessTerrain3");

	if( !g_pTerrain3Editor )
	{
		return;
	}

	if( r_terrain3->GetInt() )
	{
		imgui_Toolbar(5, 40, 1130, 35, &Terrain3EditMode, 0, Terrain3_Edit_List, sizeof( Terrain3_Edit_List ) / sizeof( char* ), false );
	}
	else
	{
		imgui_Toolbar(5, 80, 90, 35, &Terrain3EditMode, 0, Terrain3_Edit_List, 1, false );
		Terrain2EditMode = TE2_SETTINGS;
	}

	if( Terrain3 )
	{
		if( r_show_megatile->GetInt() )
		{
			r3dTerrain3::DebugTileInfo info;

			Terrain3->GetDebugTileInfoAt( UI_TerraTargetPos, &info, r_show_megatile_mask->GetInt() );

			r3dTerrain3::Info tinfo = Terrain3->GetInfo();

			char msg[ 512 ];
			sprintf( msg, "Tile at cursor: (%d,%d,%d), (%d,%d,%d), (%d,%d,%d)", info.X, info.Z, info.L, info.MegaX, info.MegaZ, info.MegaL, info.MegaMaskX, info.MegaMaskZ, info.MegaMaskAtlasVolumeId );

			_r3dSystemFont->PrintF( 10, 144, r3dColor24::white, msg );

			if( info.MegaMaskAtlasVolume )
			{
				float u0 = float( info.MegaMaskX ) / tinfo.MaskAtlasTileCountPerSide;
				float v0 = float( info.MegaMaskZ ) / tinfo.MaskAtlasTileCountPerSide;

				float wh = 1.0f / tinfo.MaskAtlasTileCountPerSide;

				float TC[ 8 ] = {	u0, v0 + wh, 
					u0 + wh, v0  + wh,
					u0 + wh, v0,
					u0, v0 };

				r3dDrawBox2D( 110, 177, 256, 256, r3dColor24::white, info.MegaMaskAtlasVolume, TC );
			}
		}

		if( r_show_terratile->GetInt() )
		{
			r3dTerrain3::DebugTileInfo info;

			Terrain3->GetDebugTileInfoAt( UI_TerraTargetPos, &info, 0 );

			r3dTerrain3::Info tinfo = Terrain3->GetInfo();

			char msg[ 512 ];
			sprintf( msg, "Tile at cursor: (%d,%d,%d), (%d,%d,%d), (%d,%d,%d)", info.X, info.Z, info.L, info.AtlasTileX, info.AtlasTileZ, info.AtlasVolumeID, info.MegaX, info.MegaZ, info.MegaL );

			_r3dSystemFont->PrintF( 10, 144, r3dColor24::white, msg );

			if( info.TileAtlasVolumeDiffuse )
			{
				float u0 = float( info.AtlasTileX ) / tinfo.AtlasTileCountPerSide;
				float v0 = float( info.AtlasTileZ ) / tinfo.AtlasTileCountPerSide;

				float wh = 1.0f / tinfo.AtlasTileCountPerSide;

				float TC[ 8 ] = {	u0,			1.0f - ( v0 + wh ), 
									u0 + wh,	1.0f - ( v0  + wh ),
									u0 + wh,	1.0f - v0,
									u0,			1.0f - v0 };

				r3dDrawBox2D( 110, 177, 256, 256, r3dColor24::white, info.TileAtlasVolumeDiffuse, TC );

				r3dDrawBox2D( 110 + 256 + 2, 177, 256, 256, r3dColor24::white, info.TileAtlasVolumeNormal, TC );
			}
		}
	}

	g_pDesktopManager->Begin( "ed_terrain" );

	float SliderX = r3dRenderer->ScreenW - 375;
	float SliderY = 120;

	int editMode = 0;

	if( Keyboard->IsPressed( kbsLeftControl ) ) editMode = 1;

	const r3dTerrainDesc& desc = Terrain3->GetDesc() ;

	g_EditedTerrain3Data = 0 ;

	if( Terrain3EditMode != TE3_SETTINGS && Terrain3EditMode != TE3_HMAP && Terrain3EditMode != TE3_MASKS 
		&& Terrain3EditMode != TE3_GLOBAL_LAYER && Terrain3EditMode != TE3_DEBUG )
	{
		if( Terrain3EditMode != TE3_EROSION)
		{
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Radius",		&Terrain3BrushRadiusVal[Terrain3EditMode],	desc.CellSize,desc.CellSize * 100.f,	"%-02.2f", 1 ) ;
		}
		else
		{
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Radius",		&Terrain3BrushRadiusVal[Terrain3EditMode],	desc.CellSize,desc.CellSize * 50.f,		"%-02.2f", 1 ) ;
		}
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Hardness",			&Terrain3BrushHardnessVal[Terrain3EditMode],	0,1,	"%-02.2f",1);

		if( Terrain3EditMode != TE3_SMOOTH )
		{
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Strength",			&Terrain3BrushStrengthVal[Terrain3EditMode],	0,1,	"%-02.2f",1);
		}

		SliderY += imgui_Checkbox(SliderX, SliderY, "Reposition Objects",	&__RepositionObjectsOnTerrain , 1);
	}

	int show_terra_circle = 1;
	if(Terrain3EditMode == TE_RAMP) show_terra_circle = 0;

	if( editMode && show_terra_circle) 
	{
		PushTerrainBrush(	Terrain3BrushRadiusVal[Terrain3EditMode], 
							Terrain3BrushHardnessVal[Terrain3EditMode] );

		if (Keyboard->IsPressed(kbsLeftShift) || Keyboard->IsPressed(kbsRightShift))
		{
			Terrain3BrushHardnessVal[Terrain3EditMode] += R3D_SIGN(Mouse->m_WheelScroll) * 0.01f;
		}
		else
		{
			Terrain3BrushRadiusVal[Terrain3EditMode] += R3D_SIGN(Mouse->m_WheelScroll);
		}
	}

	bool isHeightEditing = Terrain3EditMode != TE3_SETTINGS 
								&&
							Terrain3EditMode != TE3_COLOR ;

	if( isHeightEditing )
	{
		if( editMode && imgui_lbp )
		{
#if ENABLE_AUTODESK_NAVIGATION
			Nav::ResetCachedLevelGeometry();
#endif
			if ( ! g_pTerrain3Editor->IsUndoRecord() )
			{
				gTerrainHeightDirtiness.Reset( 128, 128 );
			}
		}
	}

	int needShowEditBoundaries = 0;

	switch( Terrain3EditMode )
	{
	case TE3_SETTINGS:
		if( ProcessTerrain3_EditingAllowed( 0 ) )
		{
			SliderY = ProcessTerrain3_Settings( SliderX, SliderY );
		}
		break;

	case TE3_DOWN:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_UpDown( SliderX, SliderY, editMode, false );
		}
		needShowEditBoundaries = 1;
		break;

	case TE3_UP:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_UpDown( SliderX, SliderY, editMode, true );
		}
		needShowEditBoundaries = 1;
		break;
	case TE3_LEVEL:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_Level( SliderX, SliderY, editMode );
		}
		needShowEditBoundaries = 1;
		break;
	case TE3_SMOOTH:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_Smooth( SliderX, SliderY, editMode );
		}
		needShowEditBoundaries = 1;
		break;
	case TE3_NOISE:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_Noise( SliderX, SliderY, editMode );
		}
		needShowEditBoundaries = 1;
		break;
	case TE3_RAMP:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_Ramp( SliderX, SliderY, editMode );
		}
		needShowEditBoundaries = 1;
		break;
	case TE3_EROSION:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_Erosion( SliderX, SliderY, editMode );
		}
		needShowEditBoundaries = 1;
		break;
	case TE3_PAINT:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_Paint( SliderX, SliderY, editMode );
		}
		needShowEditBoundaries = 1;
		break;
	case TE3_GLOBAL_LAYER:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_GlobalLayer( SliderX, SliderY, editMode );
		}
		break;
	case TE3_COLOR:
		if( ProcessTerrain3_EditingAllowed( 1 ) )
		{
			SliderY = ProcessTerrain3_ColorPaint( SliderX, SliderY, editMode );
		}
		break;
	case TE3_HMAP:
		if( ProcessTerrain3_EditingAllowed( 0 ) )
		{
			SliderY = ProcessTerrain3_HMap( SliderX, SliderY );
		}
		break;
	case TE3_MASKS:
		if( ProcessTerrain3_EditingAllowed( 0 ) )
		{
			SliderY = ProcessTerrain3_ImportMask( SliderX, SliderY );
		}
		break;
	case TE3_DEBUG:
		if( ProcessTerrain3_EditingAllowed( 0 ) )
		{
			SliderY = ProcessTerrain3_Debug( SliderX, SliderY );
		}
	}

	if ( !g_EditedTerrain3Data && g_pTerrain3Editor->IsUndoRecord() )
	{
		g_pTerrain3Editor->EndUndoRecord();

		switch( Terrain3EditMode )
		{
		case TE3_DOWN:
		case TE3_UP:
		case TE3_LEVEL:
		case TE3_SMOOTH:
		case TE3_NOISE:
		case TE3_RAMP:
		case TE3_EROSION:
			{
				g_pTerrain3Editor->EndHeightEditing();
			}
			break;

		case TE3_PAINT:
			if( Terrain3PaintMode == TE3_ERASE && Terrain3Editor_EraseAll )
			{
				g_pTerrain3Editor->EndEraseAllBrush();
			}
			else
			{
				g_pTerrain3Editor->EndLayerBrush_G();
			}
			break;
		case TE3_COLOR:
			g_pTerrain3Editor->EndColorBrush();
			break;
		default :
			g_pTerrain3Editor->FinalizeHeightEditing();
			break;
		}

		if( isHeightEditing )
		{
			gTerrainHeightDirtiness.Update( true );
		}
	}

#if 0
	static float sliderVal = 0.f;

	sliderVal = r_0debug_helper->GetFloat();

	SliderY += imgui_Value_Slider( SliderX, SliderY, "DEBUG", &sliderVal, -0.0125f, 0.0125f, "%.4f" );

	if( sliderVal != r_0debug_helper->GetFloat() )
	{
		Terrain3->RefreshAtlasTiles();
		r_0debug_helper->SetFloat( sliderVal );
	}

	static float sliderVal1 = 0.f;

	sliderVal1 = r_debug_helper->GetFloat();

	SliderY += imgui_Value_Slider( SliderX, SliderY, "DEBUG0", &sliderVal1, -0.0625f, 0.0625f, "%.4f" );

	if( sliderVal1 != r_debug_helper->GetFloat() )
	{
		Terrain3->RefreshAtlasTiles();
		r_debug_helper->SetFloat( sliderVal1 );
	}

#endif
	
	g_pDesktopManager->End();

	int IsTerrain3Frozen();

	if( IsTerrain3Frozen() && !g_NeedLoadingTilesSign )
	{
		g_NeedPrintThisAmountOfLoadingTiles = Terrain3->GetNumLoadingMegaTiles();
	}

	if( g_NeedLoadingTilesSign || IsTerrain3Frozen() )
	{
		char message[ 256 ];
		sprintf( message, "Please wait for %d(%d) loading tiles before editing", g_NeedPrintThisAmountOfLoadingTiles, g_NeedPrintThisAmountOfBkTasks );

		imgui_Static( r3dRenderer->ScreenW2 - 160, r3dRenderer->ScreenH2, message, 360, false );

		float sliceStart = r3dGetTime();

		const float SLICE_LENGTH = 0.125f;

		if( g_NeedLoadingTilesSign )
		{
			for( ; r3dGetTime() - sliceStart < SLICE_LENGTH ; )
			{
				ProcessDeviceQueue( sliceStart, SLICE_LENGTH );
				Sleep( 0 );
			}
		}
	}

	if( needShowEditBoundaries )
	{
		r_terrain3_showeditable->SetInt( 1 );
	}
}


static void drawRamp(const r3dPoint3D& rp0, const r3dPoint3D& rp1, float w1, float w2)
{
	r3dPoint3D nrm = (rp1 - rp0).Cross(r3dPoint3D(0, 1, 0));
	nrm.Normalize();

	r3dDrawLine3DT(rp0, rp1, gCam, 0.5f, r3dColor(255, 255, 255));

	r3dDrawLine3DT(rp0 - nrm * w1/2, rp0 + nrm * w1/2, gCam, 0.5f, r3dColor(255, 255, 255));
	r3dDrawLine3DT(rp1 - nrm * w1/2, rp1 + nrm * w1/2, gCam, 0.5f, r3dColor(255, 255, 255));

	r3dDrawLine3DT(rp0 - nrm * w1/2, rp1 - nrm * w1/2, gCam, 0.5f, r3dColor(255, 255, 255));
	r3dDrawLine3DT(rp0 + nrm * w1/2, rp1 + nrm * w1/2, gCam, 0.5f, r3dColor(255, 255, 255));
	r3dDrawLine3DT(rp0 - nrm * w2/2, rp1 - nrm * w2/2, gCam, 0.5f, r3dColor(180, 180, 180));
	r3dDrawLine3DT(rp0 + nrm * w2/2, rp1 + nrm * w2/2, gCam, 0.5f, r3dColor(180, 180, 180));

	return;
}

class TerrainEditNoise
{
public:
	r3dPerlinNoise	noise;

	float		maxHeight;
	float		minHeight;
	float		heightApplySpeed;
	int		heightRelative;

	bool		painting_;

	struct cell_s {
		float		org;		// original terrain height
		float		trg;		// calculated noisy terrain
		float		mc;		// current morph coef
	};
	cell_s*		noiseData;
	int		Width;
	int		Height;

public:
	TerrainEditNoise();
	~TerrainEditNoise();

	void		DrawNoiseParams(float& SliderX, float& SliderY);

	void		ResetCache();

	float		GetNoise(int x, int y) {
		r3d_assert(noiseData);

		float h = noiseData[y*Width+x].trg;
		if(h <= -99999) {
			h = noise.PerlinNoise_2D(x, y);
			h += 1.0f / 2.0f; // convert to [0..1];
			h = minHeight + h * (maxHeight - minHeight);

			if(heightRelative) {
				h += noiseData[y*Width+x].org;
			}
			noiseData[y*Width+x].trg = h;
		}

		return h;
	}

	void		Apply(const r3dPoint3D& pos, float radius, float hardiness);
};

static TerrainEditNoise terraEditNoise;

TerrainEditNoise::TerrainEditNoise()
{
	maxHeight         = 100.0f;
	minHeight         = 0.0;
	heightApplySpeed  = 1.0f;
	heightRelative    = 0;

	noiseData         = NULL;
	Height            = 0;
	Width             = 0;
}


TerrainEditNoise::~TerrainEditNoise()
{
}

void TerrainEditNoise::DrawNoiseParams(float& SliderX, float& SliderY)
{
	SliderY += imgui_Value_SliderI(SliderX, SliderY, "Octaves",      &noise.octaves_, 1, 8, "%d", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Persistence",  &noise.persistence_, 0.0001f, 2.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Frequency",    &noise.frequency_, 0.0001f, 1.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "Amplitude",    &noise.amplitude_, 0.0001f, 1.0f, "%-02.4f", 1);
	SliderY += imgui_Value_Slider (SliderX, SliderY, "MinHeight",    &minHeight, -500.0f, 500.0f, "%-02.2f", 1);	  
	SliderY += imgui_Value_Slider (SliderX, SliderY, "MaxHeight",    &maxHeight, -500.0f, 500.0f, "%-02.2f", 1);

	if(imgui_Button(SliderX, SliderY, 360, 25, "flush cache", 0)) ResetCache();
	SliderY += 30;

	SliderY += 10;
	SliderY += imgui_Value_Slider (SliderX, SliderY, "MorphSec", &heightApplySpeed, 0.0001f, 2.0f, "%-02.2f", 1);
	SliderY += imgui_Checkbox(SliderX, SliderY, "HeightRelative", &heightRelative, 1);
}

void TerrainEditNoise::ResetCache()
{
	noise.seed_ = u_random(0x7FFF);

	const r3dTerrainDesc& desc = Terrain->GetDesc() ;

	if( desc.CellCountZ != Height || desc.CellCountX != Width || !noiseData )
	{
		SAFE_DELETE_ARRAY(noiseData);
		Height = desc.CellCountZ ;
		Width  = desc.CellCountX ;

		noiseData = gfx_new cell_s[Height * Width];
	}

	for( int j = 0, e = Height; j < e; j ++ )
	{
		for( int i = 0, e = Width; i < e; i ++ )
		{
			noiseData[ i + j * e ].org = Terrain->GetHeight( i, j );
			noiseData[ i + j * e ].trg = -99999;
			noiseData[ i + j * e ].mc  = 0;
		}
	}

	return;
}


//--------------------------------------------------------------------------------------------------------
float Editor_Level::DrawPaintBounds( float SliderX, float SliderY )
{
	SliderY += imgui_Checkbox(SliderX, SliderY, "Use Bounds", &g_TerraPaintBoundCtrl.PaintUseBounds,1 );
	if ( g_TerraPaintBoundCtrl.PaintUseBounds )
	{
		SliderY += imgui_Checkbox(SliderX, SliderY, "Invert Bounds", &g_TerraPaintBoundCtrl.InvertBounds, 1 );

		static float boundRange = 500.f ;

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Bounds Range",		&boundRange, 0, 4000.f,	"%-02.2f",1);

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Min Height",		&g_TerraPaintBoundCtrl.PaintMinHeight,		0, boundRange - 1.0f,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Max Height",		&g_TerraPaintBoundCtrl.PaintMaxHeight,		g_TerraPaintBoundCtrl.PaintMinHeight, boundRange,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Min Slope",		&g_TerraPaintBoundCtrl.PaintMinSlope,		0,90,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Max Slope",		&g_TerraPaintBoundCtrl.PaintMaxSlope,		g_TerraPaintBoundCtrl.PaintMinSlope,90,	"%-02.2f",1);

		SliderY += imgui_Checkbox( SliderX, SliderY, "Draw Bounds", &TerrainDrawBounds, 1 );
	}

	if( !g_TerraPaintBoundCtrl.PaintUseBounds )
	{
		TerrainDrawBounds = 0 ;
	}

	return SliderY ;
}


int				ObjCatInit = 0;
int				NumCategories = 0;

CatSkeletonConfig::CatSkeletonConfig()
: DefaultSkeleton( 0 )
{

}

void LoadSkelConfig( CatSkeletonConfig* skel, const r3dSTLString& path, const r3dSTLString& fileName )
{
	r3dFile* f = r3d_open( ( path + fileName ).c_str(), "rb" );
	if ( ! f )
		return ;

	char* fileBuffer = game_new char[f->size + 1];
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlFile;
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);	

	fclose(f);

	if( !parseResult )
		r3dError("Failed to parse XML, error: %s", parseResult.description());
	
	pugi::xml_node xmlCfg = xmlFile.child( "SkeletonConfig" );

	pugi::xml_node defaultSkel = xmlCfg.child( "DefaultSkeleton" );

	if( !defaultSkel.empty() )
	{
		pugi::xml_attribute fileAtt = defaultSkel.attribute( "file" );

		if( !fileAtt.empty() )
		{
			skel->DefaultSkeleton = game_new r3dSkeleton();
			skel->DefaultSkeleton->LoadBinary( (path + fileAtt.value()).c_str() );
		}
	}

	delete [] fileBuffer ;

}

typedef r3dgameVector(CategoryStruct)  catlist;
catlist  ObjectCategories;
stringlist_t 	CatNames;
float			CatOffset = 0.0f;

//////////////////////////////////////////////////////////////////////////

void FillCatFromDirRecursive(CategoryStruct& cat, const char* clazz, const r3dSTLString &path, const char *ext, const r3dSTLString &relFolder)
{
	r3dSTLString s(path);
	s += "*";

	WIN32_FIND_DATA ffblk;
	HANDLE h = FindFirstFile(s.c_str(), &ffblk);

	if(h != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (ffblk.cFileName[0] != '.')
			{
				if(stristr(ffblk.cFileName, "_collision.sco"))
					continue;

				if (ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					r3dSTLString rf = relFolder + "\\" + ffblk.cFileName;
					FillCatFromDirRecursive(cat, clazz, r3dSTLString(path) + ffblk.cFileName + '\\', ext, rf);
				}
				else
				{
					const char *localExt = PathFindExtension(ffblk.cFileName);
					if (localExt && strcmpi(localExt, ext) == 0)
					{
						cat.NumObjects ++;
						cat.ObjectsNames.push_back(ffblk.cFileName);
						cat.ObjectsClasses.push_back( clazz );
						cat.ObjectsSubfolders.push_back(relFolder);
					}
				}
			}
		}
		while(FindNextFile(h, &ffblk) != 0);
		
		FindClose(h);
	}
}

//////////////////////////////////////////////////////////////////////////

void FillCatFromDir( CategoryStruct& cat, const char* clazz, const char* path, const char *ext = ".sco", bool recursive = false)
{
	char CatPath[ 256 ];

	sprintf( CatPath, "Data\\ObjectsDepot\\%s\\", path );

	cat.NumObjects = 0;

#define SKEL_CFG_FILE_NAME "SkeletonConfig.xml"

	r3dSTLString skelCfgPath = r3dSTLString( CatPath ) + SKEL_CFG_FILE_NAME ;

	if( r3dFileExists( skelCfgPath.c_str() ) )
	{
		LoadSkelConfig( &cat.SkelConfig, CatPath, SKEL_CFG_FILE_NAME );
	}

	FillCatFromDirRecursive(cat, clazz, CatPath, ext, "");
}

void IntroduceCat( CategoryStruct& cat, const char* name )
{
	CatNames.push_back( name );
	CatOffset = 0;
	cat.ObjectsNames.clear();
	cat.ObjectsClasses.clear();
	cat.ObjectsSubfolders.clear();
	cat.Offset = 0;
	cat.Type = 0;
}

//////////////////////////////////////////////////////////////////////////

void InitObjCategories()
{
	if(ObjCatInit) return;

	ObjCatInit = 1;

	ObjectCategories.clear();
	CatNames.clear();

	pugi::xml_document doc;
	Bytes xmlFileBuffer;
	bool rv = LoadAndParseXMLFile("Data\\ObjectsDepot\\DepotConfig.xml", doc, xmlFileBuffer);
	r3d_assert(rv);

	NumCategories = 0;

	typedef r3dgameSet( r3dSTLString ) CatSet;
	CatSet catSet;

	const char * const xmlCatNodeName = "category";
	const char * const xmlRootName = "root";
	pugi::xml_node catEntry = doc.root().child(xmlRootName).child(xmlCatNodeName);
	while (!catEntry.empty())
	{
		const char * xmlCatName = catEntry.attribute("name").value();
		const char * xmlCatValue = catEntry.attribute("value").value();

		CategoryStruct cat;

		char loweredCat[64] = {0};
		r3dscpy(loweredCat, xmlCatName);
		strlwr(loweredCat);

		r3d_assert(catSet.find( loweredCat ) == catSet.end());

		catSet.insert(loweredCat);

		IntroduceCat(cat, xmlCatName);

		char TempS[MAX_PATH] = {0};
		if (!strcmp(xmlCatValue, "OBJLIST"))
		{
			sprintf(TempS, "Data\\ObjectsDepot\\%s\\Objects.xml", xmlCatName);
			cat.Type = 1;

			pugi::xml_document objListXML;
			Bytes xmlObjListBuffer;

			bool rv = LoadAndParseXMLFile(TempS, objListXML, xmlObjListBuffer);
			r3d_assert(rv);
			
			const char * const objNodeName = "object";

			pugi::xml_node objDef = objListXML.root().child(xmlRootName).child(objNodeName);
			while (!objDef.empty())
			{
				const char * xmlObjName = objDef.attribute("name").value();
				const char * xmlObjType = objDef.attribute("type").value();

				cat.ObjectsNames.push_back(xmlObjName);
				cat.ObjectsClasses.push_back(xmlObjType);
				cat.ObjectsSubfolders.push_back("");

				objDef = objDef.next_sibling(objNodeName);
			}
		}
		else if (!strcmp(xmlCatValue, "PARTICLELIST"))
		{
			WIN32_FIND_DATA ffblk;
			HANDLE h = FindFirstFile("data\\particles\\*.prt", &ffblk);
			if(h != INVALID_HANDLE_VALUE) do 
			{
				// strip .prt
				if(strlen(ffblk.cFileName) > 4)
					ffblk.cFileName[strlen(ffblk.cFileName) - 4] = 0;
				cat.ObjectsNames.push_back(ffblk.cFileName);
				cat.ObjectsClasses.push_back("obj_ParticleSystem");
				cat.ObjectsSubfolders.push_back("");
			} while(FindNextFile(h, &ffblk) != 0);
			FindClose(h);

			cat.Type = 1;
			cat.NumObjects = (size2int_t)cat.ObjectsNames.size();
		}
		else
		{
			FillCatFromDir(cat, xmlCatValue, xmlCatName);
		}

		ObjectCategories.push_back(cat);
		catEntry = catEntry.next_sibling(xmlCatNodeName);
	}

	// parse remaining dirs
	{
		CatSet unspecifiedCatSet;

		WIN32_FIND_DATA ffblk;
		HANDLE h = FindFirstFile("Data\\ObjectsDepot\\*.*", &ffblk);
		if(h != INVALID_HANDLE_VALUE)
		{
			do 
			{
				char lowered[256];

				r3dscpy( lowered, ffblk.cFileName );
				strlwr( lowered );

				if( ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && 
					strcmp( ffblk.cFileName, "." )!=0 &&
					strcmp( ffblk.cFileName, ".." )!=0 && 
					!( ffblk.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) && 
					catSet.find( lowered ) == catSet.end() )
				{
					unspecifiedCatSet.insert( ffblk.cFileName );
				}

			} while(FindNextFile(h, &ffblk) != 0);
			FindClose(h);
		}

		for( CatSet::const_iterator i = unspecifiedCatSet.begin(), e = unspecifiedCatSet.end(); i != e; ++i )
		{
			CategoryStruct cat;
			const char* name = (*i).c_str();
			IntroduceCat( cat, name );
			FillCatFromDir(cat, "obj_Building", name);
#if APEX_ENABLED
			FillCatFromDir(cat, "obj_ApexDestructible", name, ".apb");
#endif
			ObjectCategories.push_back(cat);
		}
	}
}


void CloseCategories()
{
	if( !ObjCatInit )
		return ;

	for( size_t i = 0, e = ObjectCategories.size(); i < e ; i ++ )
	{
		SAFE_DELETE( ObjectCategories[ i ].SkelConfig.DefaultSkeleton );
	}

	ObjectCategories.clear();
	CatNames.clear();

	ObjCatInit = 0 ;
}

r3dSkeleton* GetDefaultSkeleton( const char* MeshName )
{
	char Path[ MAX_PATH ];

	_splitpath( MeshName, 0, Path, 0, 0 );

	char Cat[ MAX_PATH ] ;

	strlwr( Path );

	for( size_t i = 0, e = strlen( Path ) ; i < e; i ++ )
	{
		if( Path[ i ] == '/' ) Path[ i ] = '\\' ;
	}

	if( sscanf( Path, "data\\objectsdepot\\%s" , Cat ) == 1 )
	{
		size_t strl = strlen( Cat ) - 1 ;

		if( strl )
		{
			if( Cat[ strl ] == '\\' ) Cat[ strl ] = 0 ;

			for( size_t i = 0, e = CatNames.size(); i < e; i ++ )
			{
				if( !stricmp( CatNames[ i ].c_str(), Cat ) )
				{
					return ObjectCategories[ i ].SkelConfig.DefaultSkeleton ;
				}
			}
		}
	}

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////

void DrawObjectPickControls(float &SliderX, float &SliderY, char *CategoryName, char *ClassName, char *FileName, char *Str)
{
	static char RelFolder[256] = "";

	SliderY += imgui_Static(SliderX, SliderY, CategoryName, 200);
	static int idx = -1;
	imgui_DrawList(SliderX, SliderY, 360, 200, CatNames, &CatOffset, &idx, true, false, true);

	if( idx >= 0 && idx < (int)CatNames.size() )
	{
		sprintf(CategoryName, "%s", CatNames.at(idx).c_str());

		sprintf(ClassName,"");
		sprintf(FileName, "");
		if(ObjectCategories.at(idx).ObjectsClasses.size() > 0)
		{
			sprintf(ClassName, "%s", ObjectCategories.at(idx).ObjectsClasses.at(0).c_str());
			sprintf(FileName, "%s", ObjectCategories.at(idx).ObjectsNames.at(0).c_str());
			sprintf(RelFolder, "%s", ObjectCategories.at(idx).ObjectsSubfolders.at(0).c_str());
		}
	}

	SliderY += 210;

	SliderY += imgui_Static(SliderX, SliderY, FileName);

	//sprintf (Str,"Data\\ObjectsDepot\\%s\\*.*", CategoryName);
	//imgui_FileList(SliderX, SliderY, 360, r3dRenderer->ScreenH-40-SliderY, Str, FileName);
	static int idx1 = 0;
	if( idx >= 0 && idx < (int)ObjectCategories.size() )
	{
		imgui_DrawList(SliderX, SliderY, 360, r3dRenderer->ScreenH-40-SliderY, ObjectCategories.at(idx).ObjectsNames, &ObjectCategories.at(idx).Offset, &idx1, true, false, true);

		CategoryStruct& cat = ObjectCategories.at(idx);

		if( idx1 >= 0 && idx1 < (int)cat.ObjectsNames.size() )
		{
			sprintf (ClassName,"%s", cat.ObjectsClasses.at(idx1).c_str());
			sprintf (FileName,"%s", cat.ObjectsNames.at(idx1).c_str());
			sprintf (RelFolder, "%s", cat.ObjectsSubfolders.at(idx1).c_str());
		}

		if(ClassName[0] == 0 || FileName[0] == 0)
			return;

		if (ObjectCategories.at(idx).Type)
			sprintf (Str,"%s", FileName);
		else
			sprintf (Str,"Data\\ObjectsDepot\\%s%s\\%s", CategoryName, RelFolder, FileName);
	}
}

//////////////////////////////////////////////////////////////////////////

bool CheckForPrefabRecuriveness(const GameObjects &gs, const r3dSTLString &prefabName)
{
	for (uint32_t i = 0; i < gs.Count(); ++i)
	{
		GameObject *go = gs[i];
		if (go && go->isObjType(OBJTYPE_Prefab))
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(go);
			if (p->prefabName == prefabName)
			{
				return true;
			}
			GameObjects childObjs;
			p->GetGameObjects(childObjs);
			return CheckForPrefabRecuriveness(childObjs, prefabName);
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

int ObjectEditMove = 1;

void Editor_Level :: ProcessObjects()
{
	static char CategoryName[64] = "";
	static char ClassName[64] = "";
	static char FileName[64] = "";
	
	static int ObjectEditRotate = 1;
	static float YRotation = 0;

	static GameObject *EditObject = NULL;
	static GameObject *EditObject1 = NULL;
	static GameObject *EditObject1_Last = NULL;


	bool bJustStarted = g_iProcessObjectsTicked == 0;
	g_iProcessObjectsTicked = 2;

	bool bNeedPickerEnable = g_iPickerEnableTicked == 0;

	if ( bNeedPickerEnable )
		g_Manipulator3d.Enable ();

	if ( g_Manipulator3d.IsLocked() )
		g_Manipulator3d.Unlock();

	if ( !g_Manipulator3d.IsEnable () )
		g_Manipulator3d.Enable ();

	g_iPickerEnableTicked = 2;

	g_Manipulator3d.TypeFilterSet("");

	g_pDesktopManager->Begin( "ed_objects" );

	int _objRotate = Keyboard->IsPressed(kbsLeftShift);

	InitObjCategories();

	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = g_ObjectsSectionsStarty;

	const static char* list[] = { "ADD", "EDIT" };
	const static char* terrainmove[] = { "FREE", "FOLLOW TERRAIN" };
	const static char* rotatetype[] = { "CENTER", "PIVOT" };

	SliderY += imgui_Toolbar(SliderX, SliderY, 360, 35, &ObjectEditMode, 0, list, 2);
	SliderY += imgui_Static(SliderX, SliderY, "PLACEMENT MODE" );
	SliderY += imgui_Toolbar(SliderX, SliderY, 360, 35, &ObjectEditMove, 0, terrainmove, 2);
	SliderY += imgui_Static(SliderX, SliderY, "ORIGIN MODE");
	SliderY += imgui_Toolbar(SliderX, SliderY, 360, 35, &ObjectEditRotate, 0, rotatetype, 2);

	int iEditMode = 0;

	if (Keyboard->IsPressed(kbsLeftControl)) iEditMode = 1;

	EditObject1 = g_Manipulator3d.PickedObject ();

	//	We need this because if DeletePickedObjects was called later (object replace code need this)
	//	we get strange memory corruption
	static bool delSelected = false;
	if (Keyboard->WasPressed(kbsDel) || delSelected)
	{
		EditObject1 = NULL;
		g_Manipulator3d.DeletePickedObjects();
		delSelected = false;
	}

	switch (ObjectEditMode)
	{
	case 0:
		{
			char Str[256];

			if(0)
			{
				if (imgui_Button(SliderX+200, SliderY, 160, 25, "Inject as Group", 0))
				{
					srv_CreateGameObject("obj_Depot", CategoryName, r3dPoint3D(-1,-1,-1), OBJ_CREATE_SKIP_POS);
				}
			}
			stringlist_t ClassNames;
			ClassNames.push_back( "default" );
			ClassNames.push_back( "obj_Building" );
			ClassNames.push_back( "obj_LightMesh" );
			ClassNames.push_back( "obj_Sprite" );
#ifdef VEHICLES_ENABLED
			ClassNames.push_back( "obj_Vehicle" );
#endif

			static float groupOffset;
			static int classNameID = 0;
			imgui_DrawList( SliderX, SliderY, 360, 100, ClassNames, &groupOffset, &classNameID, true, false, true );
			SliderY += 100;

			DrawObjectPickControls(SliderX, SliderY, CategoryName, ClassName, FileName, Str);

			if (iEditMode)
			{
				if (*ClassName == 0)
					break;

				if (!EditObject )
				{
					if(classNameID==0)
						EditObject = srv_CreateGameObject(ClassName, Str, UI_TargetPos);
					else
						EditObject = srv_CreateGameObject(ClassNames[classNameID].c_str(), Str, UI_TargetPos);

					if( EditObject )
					{
						EditObject->UpdatePhysicsEnabled(0);

						if( EditObject->Class->Name == "obj_LightHelper" )
						{
							static_cast<obj_LightHelper*>( EditObject )->LT.IsLevelLight = 1;
						}

						ModifyShadowsFlagForTransparentObject(EditObject);
					}
				}

				if (EditObject )
				{
					EditObject->SetObjFlags(OBJFLAG_SkipCastRay);
					if (!_objRotate) 
					{
						if(EditObject->ObjTypeFlags & OBJTYPE_Particle)
						{
							EditObject->m_isSerializable = true; // if partilce placed in editor - then save it by default
							((obj_ParticleSystem*)EditObject)->bKeepAlive = true; // and keep it alive
							EditObject->SetPosition(UI_TargetPos);
						}
						else
						{
							if (ObjectEditMove)
								LandscapeCorrect( *EditObject, UI_TargetPos );
							else
								EditObject->SetPosition(UI_TargetPos);
						}
					}
				}
			}
			else
			{
				//Remove edit obejct from the world
				if (EditObject )  
					GameWorld().DeleteObject(EditObject);

				EditObject = NULL;
			}

			if (iEditMode)
			{
				if (_objRotate) 
				{
					YRotation += imgui_mmx /2.0f;
					if (YRotation >  360.0f ) YRotation -= 360.0f;
					if (YRotation <  0.0f ) YRotation = 360.0f - YRotation;

					EditObject->SetRotationVector(r3dVector(0,YRotation,0));
				}

				if (imgui_lbr)
				{
					if( EditObject && r3dGetTime() - _LastTimeBrush > 0.1 )
					{
						EditObject->ObjFlags &= ~OBJFLAG_SkipCastRay;
						_LastTimeBrush = r3dGetTime();

						UndoEntityAddDel * pUndo = ( UndoEntityAddDel * ) g_pUndoHistory->CreateUndoItem( UA_ENT_ADDDEL );
						assert( pUndo );
						if ( pUndo )
						{
							EntAddDel_t st;

							st.bDelete = false;
							st.pEnt = EditObject;

							pUndo->Create( st );
						}

						EditObject->UpdatePhysicsEnabled(1);

						EditObject = NULL;
					}
				}
			}
		}

		break;

	case 1:
		//	Snap pivots related code
		static bool doSnapPivots = false;
		static GameObject * origPickedObject = 0;
		if (doSnapPivots)
		{
			SliderY += imgui_Static(SliderX, SliderY, "Select object to snap pivot with");
			GameObject * pickedObject = g_Manipulator3d.PickedObject();
			if (origPickedObject && origPickedObject != pickedObject)
			{
				doSnapPivots = false;
				origPickedObject->SetPosition(pickedObject->GetPosition());
				origPickedObject->SetRotationVector(pickedObject->GetRotationVector());
				g_Manipulator3d.PickerResetPicked();
				g_Manipulator3d.PickerAddToPicked(origPickedObject);
			}
			break;
		}

		static bool doSnapVertices = false;
		if ( imgui_Button( SliderX + 245, SliderY, 115, 25, g_Manipulator3d.IsSnapVertexes() ? "Snap vertex (on)" : "Snap vertex (off)" ) )
		{
			doSnapVertices = !doSnapVertices;
		}

		bool doSnap = doSnapVertices;

		if( !e_noalt_vertexsnap->GetInt() )
		{
			doSnap |= Keyboard->IsPressed(kbsLeftAlt) || Keyboard->IsPressed(kbsRightAlt);
		}

		//	Handle pivot snap using key combo
		if (!e_noalt_vertexsnap->GetInt())
		{
			bool isKeyCombination = Keyboard->IsPressed(kbsLeftAlt) && Keyboard->IsPressed(kbsLeftShift);
			isKeyCombination |= Keyboard->IsPressed(kbsRightAlt) && Keyboard->IsPressed(kbsLeftShift);
			isKeyCombination |= Keyboard->IsPressed(kbsLeftAlt) && Keyboard->IsPressed(kbsRightShift);
			isKeyCombination |= Keyboard->IsPressed(kbsRightAlt) && Keyboard->IsPressed(kbsRightShift);

			if (isKeyCombination)
			{
				doSnap = false;
				origPickedObject = g_Manipulator3d.PickedObject();
				doSnapPivots = true;
			}
		}

		g_Manipulator3d.SetSnapVetexes(doSnap);

		static bool replaceMode = false;

		//	Snap pivots mode
		if (!replaceMode && imgui_Button(SliderX + 125, SliderY, 115, 25, "Snap pivots"))
		{
			origPickedObject = g_Manipulator3d.PickedObject();
			doSnapPivots = true;
		}

		//	Object replace code
		if (g_Manipulator3d.PickedObjectCount() > 0)
		{
			if (imgui_Button(SliderX, SliderY, 115, 25, "Replace objects"))
			{
				replaceMode = !replaceMode;
			}
		}

		SliderY += 27;
		if (!replaceMode && g_Manipulator3d.PickedObjectCount() > 0 && r_hide_no_clip_editor->GetBool() == false)
		{
			if (imgui_Button(SliderX, SliderY, 115, 25, "New Area Clip"))
			{
				r3dTL::TArray<GameObject*> selectedObjects;
				g_Manipulator3d.GetPickedObjects( &selectedObjects );
				for( uint32_t i = 0; i < selectedObjects.Count(); ++i )
				{
					GameObject* obj = selectedObjects[ i ];
					r3dPoint3D pos = obj->GetPosition();
					r3dMesh* mesh = obj->GetObjectMesh();
					if( mesh )
					{
						pos += mesh->getCentralPoint() - mesh->getPivot();
					}
					GameObject* noClipObj = srv_CreateGameObject("obj_NoClipBox", CategoryName, pos, OBJ_CREATE_SKIP_LOAD);
					if( !noClipObj )
					{
						r3dOutToLog("!!! Failed to generate No Clip Box from %s\n", obj->Name.c_str());
						continue;
					}
					noClipObj->SetScale( obj->GetBBoxLocal().Size*0.9f );
					noClipObj->SetRotationVector( obj->GetRotationVector() );

					// Add the NoClipBox to the Undo list.
					UndoEntityAddDel * pUndo = ( UndoEntityAddDel * ) g_pUndoHistory->CreateUndoItem( UA_ENT_ADDDEL );
					assert( pUndo );
					if ( pUndo )
					{
						EntAddDel_t st;

						st.bDelete = false;
						st.pEnt = noClipObj;

						pUndo->Create( st );
					}
				}
			}

			if (imgui_Button(SliderX, SliderY+27, 115, 25, "New Wall Clip"))
			{
				r3dTL::TArray<GameObject*> selectedObjects;
				g_Manipulator3d.GetPickedObjects( &selectedObjects );
				for( uint32_t i = 0; i < selectedObjects.Count(); ++i )
				{
					GameObject* obj = selectedObjects[ i ];
					r3dPoint3D pos = obj->GetPosition();
					r3dMesh* mesh = obj->GetObjectMesh();
					if( mesh )
					{
						pos += mesh->getCentralPoint() - mesh->getPivot();
					}
					GameObject* noClipObj = srv_CreateGameObject("obj_NoClipPlane", CategoryName, pos, OBJ_CREATE_SKIP_LOAD);
					if( !noClipObj )
					{
						r3dOutToLog("!!! Failed to generate No Clip Box from %s\n", obj->Name.c_str());
						continue;
					}
					noClipObj->SetScale( obj->GetBBoxLocal().Size*0.9f );
					noClipObj->SetRotationVector( obj->GetRotationVector() );

					// Add the NoClipBox to the Undo list.
					UndoEntityAddDel * pUndo = ( UndoEntityAddDel * ) g_pUndoHistory->CreateUndoItem( UA_ENT_ADDDEL );
					assert( pUndo );
					if ( pUndo )
					{
						EntAddDel_t st;

						st.bDelete = false;
						st.pEnt = noClipObj;

						pUndo->Create( st );
					}
				}
			}
		}

		if(!replaceMode && r_hide_no_clip_editor->GetBool() == false)
		{
			if (imgui_Button(SliderX + 245, SliderY, 115, 25, (r_show_no_clip_x_ray->GetBool()) ? "NoClip X-Ray(on)" : "NoClip X-Ray(off)"))
			{
				r_show_no_clip_x_ray->SetBool( !r_show_no_clip_x_ray->GetBool() );
			}

			// TODO: Delete the Export NoClip code below when we get the FairFight API.
			if (imgui_Button(SliderX + 125, SliderY, 115, 25, "Export NoClip"))
			{
				for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
				{
					if( iter.current->isObjType( OBJTYPE_NoClipBox ) )
					{
						obj_NoClipBox* ncb = (obj_NoClipBox*)iter.current;
						r3dPoint3D points[8];
						ncb->GetPoints( points );
						NoClipData c;
						memcpy(c.points, points, sizeof( r3dPoint3D ) * 8);
						obj_NoClipBox::s_vNoClipData.push_back(c);
					}
					else if (iter.current->isObjType(OBJTYPE_NoClipPlane))
					{
						obj_NoClipPlane* ncp = (obj_NoClipPlane*)iter.current;
						r3dPoint3D points[4];
						ncp->GetPoints(points);
						NoClipPlaneData c;
						memcpy(c.points, points, sizeof(r3dPoint3D) * 4);
						obj_NoClipPlane::noClipDatas.push_back(c);
					}
				}

				if( obj_NoClipBox::s_vNoClipData.size() > 0 || obj_NoClipPlane::noClipDatas.size() > 0)
				{
					//r3dOutToLog("!!! Found %d No Clip Boxes\n", obj_NoClipBox::s_vNoClipData.size() );
					char areasFilename[64] = { 0 };
					char wallsFilename[64] = { 0 };
					const char* mapname = strrchr( r3dGameLevel::GetHomeDir(), '\\' );
					int mapId = 0;
					if( mapname )
					{
						sprintf( areasFilename, "AreasNoClip_%s.csv", mapname + 1 );
						sprintf( wallsFilename, "WallsNoClip_%s.csv", mapname + 1 );
						if(stricmp(mapname+1, "colorado_v2")==0)
							mapId = GBGameInfo::MAPID_WZ_Colorado;
						else if(stricmp(mapname+1, "california_v2")==0)
							mapId = GBGameInfo::MAPID_WZ_California;
						else if(stricmp(mapname+1, "wz_cliffside")==0)
							mapId = GBGameInfo::MAPID_WZ_Cliffside;
						else if(stricmp(mapname+1, "caliwood")==0)
							mapId = GBGameInfo::MAPID_WZ_Caliwood;
						else if(stricmp(mapname+1, "colorado_v1")==0)
							mapId = GBGameInfo::MAPID_WZ_ColoradoV1;
						else if(stricmp(mapname+1, "wz_san_diego")==0)
							mapId = GBGameInfo::MAPID_WZ_SanDiego;
						else if(stricmp(mapname+1, "wz_devmap")==0)
							mapId = GBGameInfo::MAPID_WZ_Devmap;
						else if(stricmp(mapname+1, "wz_gamehard1")==0)
							mapId = GBGameInfo::MAPID_WZ_GameHard1;
					}
					else
					{
						sprintf( areasFilename, "AreasNoClipBoxes_UNKNOWN.csv" );
						sprintf( wallsFilename, "WallsNoClipBoxes_UNKNOWN.csv" );
					}

					// Area conditions
					CSVLog* csv = new CSVLog( areasFilename );

					for( uint32_t i = 0; i < obj_NoClipBox::s_vNoClipData.size(); ++i )
					{
						csv->StartEntry();

						for( uint32_t j = 4; j < 8; ++j ) // 4,5,6,7 indexes are lower part of bbox and designers were told not to rotate noclip boxes
						{
							char label[16] = { 0 };
							csv->Log("map_name", mapId );
							csv->Log("rule_id", 129); // 129 no clip, 128 no block, 150 kick

							sprintf( label, "p%d_x", j-3 );
							csv->Log( label, obj_NoClipBox::s_vNoClipData[ i ].points[ j ].x );
							// 						sprintf( label, "_%02dy", j-4 );
							// 						csv.Log( label, obj_NoClipBox::s_vNoClipData[ i ].points[ j ].y );
							sprintf( label, "p%d_z", j-3 );
							csv->Log( label, obj_NoClipBox::s_vNoClipData[ i ].points[ j ].z );
						}
						csv->Log("vertical_top", obj_NoClipBox::s_vNoClipData[ i ].points[ 0 ].y );
						csv->Log("vertical_bottom", obj_NoClipBox::s_vNoClipData[ i ].points[ 7 ].y );
						csv->Log("timeframe_seconds", 2); // 2 for no clip, 20 for no block
					}
					csv->EndEntry();
					csv->Update();
					delete csv;

					// Wall conditions
					CSVLog* csvWalls = new CSVLog( wallsFilename );

					for( uint32_t i = 0; i < obj_NoClipPlane::noClipDatas.size(); ++i )
					{
						csvWalls->StartEntry();

						char label[16] = { 0 };
						csvWalls->Log("map_name", mapId );
						csvWalls->Log("rule_id", 150); // 129 no clip, 128 no block, 150 kick

						csvWalls->Log( "p1_x", obj_NoClipPlane::noClipDatas[ i ].points[ 0 ].x );
						csvWalls->Log( "p1_z", obj_NoClipPlane::noClipDatas[ i ].points[ 0 ].z );
						csvWalls->Log( "p2_x", obj_NoClipPlane::noClipDatas[ i ].points[ 1 ].x );
						csvWalls->Log( "p2_z", obj_NoClipPlane::noClipDatas[ i ].points[ 1 ].z );

						csvWalls->Log("vertical_top", obj_NoClipPlane::noClipDatas[ i ].points[ 3 ].y );
						csvWalls->Log("vertical_bottom", obj_NoClipPlane::noClipDatas[ i ].points[ 1 ].y );
					}
					csvWalls->EndEntry();
					csvWalls->Update();
					delete csvWalls;

					obj_NoClipBox::s_vNoClipData.clear();
					obj_NoClipPlane::noClipDatas.clear();
					g_ShowNoClipExportSign = r3dGetTime() + 2.2f;
				}
			}
		}

		float buttonsSliderY = SliderY;
		SliderY += 54;

		if (replaceMode)
		{
			static char catName[64] = "dummy";
			static char ClassName[64] = "";
			static char FileName[64] = "";
			static char Str[256] = "";
			SliderY += imgui_Static(SliderX, SliderY, "Select replacement object:") + 10;
			DrawObjectPickControls(SliderX, SliderY, catName, ClassName, FileName, Str);

			if (strlen(Str) > 0 && imgui_Button(SliderX + 125, buttonsSliderY, 115, 25, "Replace"))
			{
				if( UndoReplaceObjects* undo = static_cast<UndoReplaceObjects*>( g_pUndoHistory->CreateUndoItem( UA_REPLACE_OBJECTS ) ) )
				{
					undo->SetTitle( "Replace Objects" );

					for (uint32_t i = 0; i < g_Manipulator3d.PickedObjectCount(); ++i)
					{
						GameObject * oldObj = g_Manipulator3d.PickedObjectGet( i );

						UndoReplaceObjects::UndoEntry entry;

						entry.Object = oldObj;
						entry.SrcType = oldObj->FileName;
						entry.TargType = Str;

						undo->AddItem( entry );
					}

					undo->Redo();

					EditObject = NULL;
					EditObject1 = NULL;
				}
				replaceMode = false;
			}
			break;
		}

		if (EditObject1 )
		{
			SliderY += imgui_Static (SliderX, SliderY, EditObject1->Name.c_str());
			SliderY += imgui_Static (SliderX, SliderY, EditObject1->Class->Name.c_str());			
			GameObjects objects ;
			g_Manipulator3d.GetPickedObjects( &objects ) ;
			SliderY += EditObject1->DrawPropertyEditor(SliderX, SliderY+10, 300, 200, g_Manipulator3d.GetPickedMostDerivedClass(), objects );
		}

		SliderY += 10.0f;

		if( EditObject1 && EditObject1->isObjType( OBJTYPE_Mesh ) )
		{
			MeshGameObject* mgo = static_cast< MeshGameObject* >( EditObject1 );

			if( r3dMesh* mesh = mgo->GetObjectMesh() )
			{
				SliderY += imgui_Static( SliderX, SliderY, "Materials" );

				for( int i = 0, e = (int)mesh->GetNumMaterials(); i < e; i ++ )
				{
					r3dTriBatch& triBatch = mesh->MatChunks[ i ];

					if( imgui_Button( SliderX, SliderY, 360.f, 22.f, triBatch.MatName.c_str() ) )
					{
						EditMaterial = mesh->MatChunks[ i ].Mat;
						MainToolIdx = EDITMODE_MATERIALS;
					}

					SliderY += 22.f;
				}
			}
		}

		// ptumik: not needed. controlled by code only!
		/*if (EditObject1)
		{
			SliderY += imgui_Static( SliderX, SliderY, "Save type:", 360 ) ;

			const static char* serializeFiles[] = { "Level", "Sound", "Server" };

			int sfType = static_cast<int>(EditObject1->serializeFile);
			SliderY += imgui_Toolbar(SliderX, SliderY, 360, 45, &sfType, 0, serializeFiles, _countof(serializeFiles) );
			EditObject1->serializeFile = static_cast<ESerializeFile>(sfType);
		}*/

		SliderY += 22.f ;

		SliderY += imgui_Static( SliderX, SliderY, "Select by name", 360 ) ;

		static char ObjectName[ 1024 ] = { 0 } ;

		if( EditObject1 && EditObject1_Last != EditObject1 )
		{
			char drive[ 16 ], dir[ 1024 ], file[ 1024 ], ext[ 1024 ] ;
			_splitpath( EditObject1->FileName.c_str(), drive, dir, file, ext ) ;

			strcpy( ObjectName, file ) ;
			EditObject1_Last = EditObject1 ;
		}

		imgui2_StartArea ( "area1", SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT );
		imgui2_StringValue ( "Object Name:", ObjectName );
		imgui2_EndArea ();

		SliderY += DEFAULT_CONTROLS_HEIGHT ;

		if( imgui_Button( SliderX, SliderY, 360, 22, "Select All" ) )
		{
			g_Manipulator3d.PickByName( ObjectName, true ) ;
		}

		SliderY += 24.f;

		if( imgui_Button( SliderX, SliderY, 360, 22, "Select All (Partial Match)" ) )
		{
			g_Manipulator3d.PickByName( ObjectName, false ) ;
		}

		SliderY += DEFAULT_CONTROLS_HEIGHT + 5;

		static char prefabName[ 1024 ] = { 0 } ;
		imgui2_StartArea ( "area2", SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT );
		imgui2_StringValue ( "Prefab Name:", prefabName );
		imgui2_EndArea ();

		SliderY += DEFAULT_CONTROLS_HEIGHT ;

		if (imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Create prefab"))
		{
			GameObjects gs;
			g_Manipulator3d.GetPickedObjects(&gs);

			//	Check for recursive prefab inclusion
			bool isRecusrive = CheckForPrefabRecuriveness(gs, prefabName);
			if (isRecusrive)
			{
				MessageBox(r3dRenderer->HLibWin, "Recursive prefab objects is not allowed! Pick a different prefab name.", "Prefab creation error", MB_ICONERROR | MB_OK);
				break;
			}

			g_PrefabManager.CreatePrefab(gs, prefabName, 0);

			r3dVector center(0, 0, 0);
			for (uint32_t i = 0; i < gs.Count(); ++i)
			{
				center += gs[i]->GetPosition();
				RemoveObjectFromGUIHelperStack(gs[i]);
			}
			center /= gs.Count();

			static const uint32_t dummyTag = 0xbaadf00d;
			g_pUndoHistory->SetTagForAllNextActions(dummyTag);
			g_Manipulator3d.DeletePickedObjects();
			g_pUndoHistory->SetTagForAllNextActions(0);

			g_pUndoHistory->RemoveUndoActionsByTag(dummyTag);

			obj_Prefab * pNew = g_PrefabManager.InstantiatePrefab(prefabName, center);

			CreatePrefabUndo *pu = static_cast<CreatePrefabUndo*>(g_pUndoHistory->CreateUndoItem(UA_PREFAB_CREATE));
			pu->SetPrefab(pNew);
			pNew->SelectPrefab();
			EditObject1 = 0;
		}

		SliderY += DEFAULT_CONTROLS_HEIGHT + 5;

		break;
	}

	//	Don't push prefabs, because them don't have any custom editor draw function, but "Break" functionality
	//	that destroy self object will lead to dangling pointer in GUIHelperOBjects stack.
	if( EditObject1 && !EditObject1->isObjType(OBJTYPE_Prefab))
	{
		PushSelectedObject( EditObject1 );
	}

	g_pDesktopManager->End();
}


void Editor_Level :: ProcessGroups()
{
	float SliderX = r3dRenderer->ScreenW-365;
	float SliderY = g_ObjectsSectionsStarty;

	static GameObject *SelObj = NULL;

	bool bJustStarted = g_iProcessGroupsTicked == 0;
	g_iProcessGroupsTicked = 2;

	bool bNeedPickerEnable = g_iPickerEnableTicked == 0;
	if ( bNeedPickerEnable )
		g_Manipulator3d.Enable ();
	
	if ( bJustStarted )
	{
		g_Manipulator3d.Lock ();
		//g_Manipulator3d.PickerResetPicked();
	}

	g_iPickerEnableTicked = 2;

	g_Manipulator3d.TypeFilterSet("");

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;

		if(!(obj->Class->Name == "obj_Depot")) {
			continue;
		}

		if(imgui_Button(SliderX, SliderY, 360, 25, obj->Name.c_str(), 0)) {
			SelObj = obj;
		}
		SliderY += 30;
	}

	if (SelObj)
	{
		if (Keyboard->IsPressed(kbsLeftControl))
			if (imgui_lbr)
			{
				SelObj->SetPosition(UI_TargetPos);
			}

	}

	stringlist_t sGroupList;

	static int g_iGroupListCurSel = -1;
	static float g_iGroupListOffset = 0.0f;

	for ( uint32_t i = 0; i < obj_Group::m_dGroups.Count(); i++ )
	{
		char sNameNew [512];
		int iCnt = obj_Group::m_dGroups[i]->m_dObjects.Count ();
		if ( iCnt == 0 )
			iCnt = obj_Group::m_dGroups[i]->m_iPreviewObjectsInGroup;
		sprintf ( sNameNew, "%s (%i)%s", obj_Group::m_dGroups[i]->m_szName, iCnt, obj_Group::m_dGroups[i]->IsGroupJustCreated () ? " - edit mode" : "" );
		sGroupList.push_back ( sNameNew );
	}

	if ( imgui_Button ( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 8, DEFAULT_CONTROLS_HEIGHT, "Add New Group ( Empty )", 0 ) ) 
	{
		obj_Group * pGroup = obj_Group::CreateNewGroup();

		if ( pGroup )
			pGroup->SetJustCreatedFlag ();

		g_iGroupListCurSel = (int)obj_Group::m_dGroups.Count () - 1;

		g_Manipulator3d.PickerResetPicked();
	}
	
	if ( imgui_Button ( SliderX + DEFAULT_CONTROLS_WIDTH * 0.5f + 4, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 8, DEFAULT_CONTROLS_HEIGHT, "Delete Group", 0 ) ) 
	{
		if ( g_iGroupListCurSel >= 0 && g_iGroupListCurSel < (int)obj_Group::m_dGroups.Count () )
		{
			obj_Group::DeleteGroup( *obj_Group::m_dGroups[g_iGroupListCurSel] );

			g_iGroupListCurSel = 0;

			g_Manipulator3d.PickerResetPicked();

			if ( g_iGroupListCurSel >= 0 && g_iGroupListCurSel < (int)obj_Group::m_dGroups.Count () )
			{
				for ( uint32_t i = 0; i < obj_Group::m_dGroups[g_iGroupListCurSel]->m_dObjects.Count(); i++ )
				{
					g_Manipulator3d.PickerAddToPicked ( obj_Group::m_dGroups[g_iGroupListCurSel]->m_dObjects[i] );
				}		
			}
		}
	}

	SliderY += DEFAULT_CONTROLS_HEIGHT;

	if ( g_Manipulator3d.PickedObjectCount () > 0 )
	{
		if ( imgui_Button ( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Add New Group ( Push Selected Objects To Group )", 0 ) ) 
		{
			if ( g_iGroupListCurSel >= 0 && g_iGroupListCurSel < (int)obj_Group::m_dGroups.Count () )
				obj_Group::m_dGroups[g_iGroupListCurSel]->ResetJustCreatedFlag( false );
			
			obj_Group * pGroup = obj_Group::CreateNewGroup();

			if ( pGroup )
				pGroup->SetJustCreatedFlag ();

			for ( uint32_t i = 0; i < g_Manipulator3d.PickedObjectCount (); i++ )
			{
				GameObject * pObj = g_Manipulator3d.PickedObjectGet (i);
				r3d_assert ( pObj );
				pGroup->AddObject ( pObj );
			}

			g_iGroupListCurSel = (int)obj_Group::m_dGroups.Count () - 1;
		}

		SliderY += DEFAULT_CONTROLS_HEIGHT;
	}


	if ( imgui_DrawList ( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_LIST_HEIGHT, sGroupList, &g_iGroupListOffset, &g_iGroupListCurSel ) )
	{
		if ( g_iGroupListCurSel >= 0 && g_iGroupListCurSel < (int)obj_Group::m_dGroups.Count () )
		{
			g_Manipulator3d.PickerResetPicked();

			if ( obj_Group::m_dGroups[g_iGroupListCurSel]->IsGroupJustCreated () )
			{
				for ( uint32_t i = 0; i < obj_Group::m_dGroups[g_iGroupListCurSel]->m_dObjects.Count(); i++ )
				{
					g_Manipulator3d.PickerAddToPicked ( obj_Group::m_dGroups[g_iGroupListCurSel]->m_dObjects[i] );
				}	
			}
		}
	}

	SliderY += DEFAULT_LIST_HEIGHT;

	static int dropMode = 0;

	if( Terrain )
		SliderY += imgui_Checkbox( SliderX, SliderY, "Drop on Ground", &dropMode, 1 );
	else
		dropMode = 0;

	obj_Group * pCurGroup = NULL;
	if ( g_iGroupListCurSel >= 0 && g_iGroupListCurSel < (int)obj_Group::m_dGroups.Count () )
		pCurGroup = obj_Group::m_dGroups[g_iGroupListCurSel];
	else
		pCurGroup = NULL;

	if( pCurGroup )
	{
		if( imgui_Button( SliderX, SliderY, 360, 22, "Inject into map" ) )
		{
			if( !pCurGroup->GetPreviewMode() )
			{
				pCurGroup->LoadGroupObjects();
			}

			for ( uint32_t i = 0; i < pCurGroup->m_dObjects.Count (); i++ )
			{
				GameObject * pObj = pCurGroup->m_dObjects[i]->Clone ();

				if ( pObj )
				{
					pObj->ObjFlags &= ~OBJFLAG_SkipCastRay;
					pObj->bPersistent = 1;
				}
			}

			if( !pCurGroup->GetPreviewMode() )
			{
				pCurGroup->UnloadGroupObjects();
			}
		}

		SliderY += 22;
	}

	if( imgui_Button( SliderX, SliderY, 360, 22, "Save Groups" ) )
	{
		obj_Group::Save();
	}

	SliderY += 22;

	for ( uint32_t i = 0; i < obj_Group::m_dGroups.Count (); i++ )
	{
		if ( obj_Group::m_dGroups[i] != pCurGroup )
		{
			obj_Group::m_dGroups[i]->SetPreviewMode(false);
			obj_Group::m_dGroups[i]->ResetJustCreatedFlag ();
		}
	}

	if ( pCurGroup )
	{
		imgui2_StartArea ( "area1", SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT );
		imgui2_StringValue ( "Group Name", pCurGroup->m_szName );
		imgui2_EndArea ();

		if ( Keyboard->IsPressed ( kbsLeftControl ) )
		{
			if ( pCurGroup->IsGroupJustCreated () )
			{
				if ( imgui_lbr )
				{
					if( GameObject* obj = GameWorld().GetObjectA( UI_TargetObjID ) )
					{
						if ( !pCurGroup->IsObjectInGroup ( obj ) )
						{
							pCurGroup->AddObject(obj);
							g_Manipulator3d.PickerAddToPicked ( obj );
						}
						else
						{
							pCurGroup->RemoveObject(obj);
							g_Manipulator3d.PickerRemoveFromPicked ( obj );
						}
					}
				}
			}
			else
			{
				r3dBoundBox tBoxUnion;

				r3dPoint3D vCenter = r3dPoint3D( 0, 0, 0 );

				if( pCurGroup->m_dObjects.Count() )
				{
					tBoxUnion.Org = r3dPoint3D( FLT_MAX, FLT_MAX, FLT_MAX );
					tBoxUnion.Size = r3dPoint3D( -FLT_MAX, -FLT_MAX, -FLT_MAX );

					for ( uint32_t i = 0; i < pCurGroup->m_dObjects.Count (); i++ )
					{
						tBoxUnion.ExpandTo(pCurGroup->m_dObjects[i]->GetBBoxWorld());
					}
				}
				else
				{
					tBoxUnion.Org = r3dPoint3D( 0, 0, 0 );
					tBoxUnion.Size = r3dPoint3D( 0, 0, 0 );
				}

				vCenter = tBoxUnion.Center();

				if ( imgui_lbr )
				{
					g_Manipulator3d.PickerResetPicked();

					for ( uint32_t i = 0; i < pCurGroup->m_dObjects.Count (); i++ )
					{
						GameObject * pObj = pCurGroup->m_dObjects[i]->Clone ();

						if ( pObj )
						{
							r3dPoint3D pos;

							pos = UI_TargetPos + pObj->GetPosition() -  vCenter + r3dPoint3D( 0, tBoxUnion.Size.y / 2, 0 );

							if( dropMode )
							{
								pos.y = Terrain->GetHeight( pos )- pObj->GetBBoxLocal().Org.y;
							}

							pObj->SetPosition ( pos );
							pObj->ObjFlags &= ~(OBJFLAG_SkipCastRay | OBJFLAG_Removed);
							pObj->bPersistent = 1;
							g_Manipulator3d.PickerAddToPicked ( pObj );
						}
					}
				}
				else
				{
					pCurGroup->SetPreviewMode ( true );
					pCurGroup->SetPreviewObjectsPos ( UI_TargetPos + r3dPoint3D( 0, tBoxUnion.Size.y / 2, 0 ), dropMode ? true : false );
					
					tBoxUnion.Org = UI_TargetPos - r3dVector(tBoxUnion.Size.x,0,tBoxUnion.Size.z)*0.5f;
					r3dDrawBoundBox(tBoxUnion, gCam, r3dColor::white, 1 );
				}
			}
		}
		else
		{
			if ( !pCurGroup->IsGroupJustCreated () )
			{
				if( Keyboard->WasPressed(kbsDel)
					)
				{
					g_Manipulator3d.DeletePickedObjects();
				}
			}
				 
			pCurGroup->SetPreviewMode(false);
		}
	}
}



void UpdateDestructionMeshSettings( const char* target )
{
	char TargetLWR[ MAX_PATH ];

	if( target )
	{
		strcpy( TargetLWR, target );
		strlwr( TargetLWR );
	}

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;

		if( obj->isObjType( OBJTYPE_Mesh ) )
		{
			MeshGameObject* mgo = static_cast< MeshGameObject* > ( obj );

			if( target )
			{
				char FileNameLwr[ MAX_PATH ];
				strcpy( FileNameLwr, mgo->MeshLOD[0]->FileName.c_str() );
				strlwr( FileNameLwr );

				if( !strstr( FileNameLwr, TargetLWR ) )
					continue;
			}

			mgo->UpdateDestructionData();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void Editor_Level::ProcessPrefabs()
{
	float SliderX = r3dRenderer->ScreenW-365;
	float SliderY = g_ObjectsSectionsStarty;

	if (imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Save prefabs"))
	{
		g_PrefabManager.SavePrefabs();
	}
	SliderY += 25.0f;

	static stringlist_t prefabs;
	prefabs.clear();
	static float offset = 0;
	static int index = 0;

	g_PrefabManager.FillNameList(prefabs);
	if (prefabs.empty())
		return;

	int listHeight = 600;
	imgui_DrawList(SliderX, SliderY, 360, listHeight, prefabs, &offset, &index, true, false, true);
	SliderY += listHeight + 10;
	const r3dSTLString &name = prefabs[index];

	if (imgui_Button(SliderX, SliderY, 360, 22.0f, "Delete Selected Prefab"))
	{
		g_PrefabManager.DeletePrefab(name);
		if (index == prefabs.size() - 1)
		{
			index = std::max(0, index - 1);
		}
	}
	SliderY += 25.0f;
	
	if (imgui_Button(SliderX, SliderY, 360.0f, 22.0f, "Break All Prefabs"))
	{
		g_PrefabManager.BreakAllPrefabs();		
	}
	SliderY += 25.0f;

	static bool ctrlWasPressed = false;
	static obj_Prefab *spawnPrefab = 0;
	if (Keyboard->IsPressed(kbsLeftControl))
	{
		if (!ctrlWasPressed)
		{
			spawnPrefab = g_PrefabManager.InstantiatePrefab(name, UI_TargetPos);

			spawnPrefab->SetPreviewMode(true);
			ctrlWasPressed = true;
		}
		if (spawnPrefab)
		{
			spawnPrefab->SetPosition(UI_TargetPos);

			if (imgui_lbr)
			{
				spawnPrefab->SetPreviewMode(false);
				spawnPrefab = 0;
				ctrlWasPressed = false;
			}
		}
	}
	else
	{
		if (spawnPrefab)
		{
			GameWorld().DeleteObject(spawnPrefab);
			spawnPrefab = 0;
		}
		ctrlWasPressed = false;
	}


	SliderY += 25.0f;
}

//////////////////////////////////////////////////////////////////////////


void
Editor_Level::ProcessDamageLib()
{
	float SliderX = r3dRenderer->ScreenW-378;
	float SliderY = g_ObjectsSectionsStarty;

	static char MeshName[ 256 ] = {},
				CatName[ 256 ] = {};

	MeshName[ 0 ] = 0 ;
	CatName[ 0 ] = 0 ;

	g_pDesktopManager->Begin( "ed_dmglib" );

	static float catOffset = 0.f ;
	static int catIdx = -1 ;

	static float itemOffset = 0.f ;
	static int itemIdx = -1 ;

	static stringlist_t dirBoundCatList ;
	static r3dTL::TArray< int > indexMap ;

	dirBoundCatList.clear() ;
	indexMap.Clear();

	if ( imgui_Button ( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 2, DEFAULT_CONTROLS_HEIGHT, "Save" ) )
	{
		g_DamageLib->Save() ;
	}

	if ( imgui_Button ( SliderX + DEFAULT_CONTROLS_WIDTH * 0.5f + 2, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 2, DEFAULT_CONTROLS_HEIGHT, "Update Meshes" ) )
	{
		UpdateDestructionMeshSettings( NULL );
	}

	SliderY += DEFAULT_CONTROLS_HEIGHT + 2 ;

	static int AddModeOn = 0 ;

	SliderY += imgui_Checkbox( SliderX, SliderY, "Show add entry params", &AddModeOn, 1 );

	// mesh categories
	{
		if( AddModeOn )
		{
			SliderY += imgui_Static( SliderX, SliderY, "Mesh Folders" );
		}

		for( int i = 0, e = (size2int_t)ObjectCategories.size() ; i < e; i ++ )
		{
			const CategoryStruct& cs = ObjectCategories[ i ] ;			

			if( cs.Type == 0 )
			{
				dirBoundCatList.push_back( CatNames[ i ] );
				indexMap.PushBack( i );
			}
		}

		if( AddModeOn )
		{
			imgui_DrawList( SliderX, SliderY, 360, 200, dirBoundCatList, &catOffset, &catIdx, true, false, true );
			SliderY += 210;
		}

		if( catIdx >= 0 && catIdx < (int)dirBoundCatList.size() )
		{
			strcpy( CatName, dirBoundCatList[ catIdx ].c_str() );

			const CategoryStruct& cat = ObjectCategories[ indexMap[ catIdx ] ];

			if( AddModeOn )
			{
				imgui_DrawList( SliderX, SliderY, 360, 200, cat.ObjectsNames, &itemOffset, &itemIdx, true, false, true );
				SliderY += 210;
			}

			if( itemIdx >= 0 && itemIdx < (int)cat.ObjectsNames.size() )
			{
				strcpy( MeshName, cat.ObjectsNames[ itemIdx ].c_str() );
			}
		}
	}

	SliderY += imgui_Static( SliderX, SliderY, "LIB ENTRIES" );

	static stringlist_t entryList ;
	g_DamageLib->GetEntryNames( &entryList );

	static float entryOffset = 0.f ;
	static int entryIdx = -1 ;

	static float damagedOffset = 0.f ;
	static int damagedIdx = 0 ;

	static float soundOffset = 0.f ;
	static int soundIdx = 0 ;

	bool ForceEntrySelection = false ;
	bool ForceEntryCreated = false ;

	if( DesiredDamageLibEntry[ 0 ] )
	{
		if( !g_DamageLib->GetEntry( DesiredDamageLibEntry ) )
		{
			g_DamageLib->AddEntry( DesiredDamageLibEntry );
			g_DamageLib->GetEntryNames( &entryList );
			ForceEntryCreated = true ;
		}

		for( int i = 0, e = (size2int_t)entryList.size(); i < e; i ++ )
		{
			if( entryList[ i ] == DesiredDamageLibEntry )
			{
				entryIdx = i ;
				entryOffset = r3dTL::Max( float( i - 7 ), 0.f ) ;
				break ;
			}
		}

		ForceEntrySelection = true ;

		DesiredDamageLibEntry[ 0 ] = 0 ;
	}

	if( ForceEntrySelection || imgui_DrawList( SliderX, SliderY, 360, 200, entryList , &entryOffset, &entryIdx, true, false, true ) )
	{
		if( entryIdx >= 0 && entryIdx < (int)entryList.size() )
		{
			if( DamageLibEntry* en = g_DamageLib->GetEntry( entryList[ entryIdx ] ) )
			{
				r3dSTLString cat, mesh;
				if( g_DamageLib->DecomposeKey( entryList[ entryIdx ], &cat, &mesh ) )
				{
					bool foundCat = false;

					for( int i = 0, e = indexMap.Count(); i < e; i ++ )
					{
						if( !stricmp( CatNames[ indexMap[ i ] ].c_str(), cat.c_str() ) )
						{
							catIdx = i;
							catOffset = r3dTL::Max( catIdx - 5.f, 0.f );
							foundCat = true;
							break;
						}						
					}

					if( foundCat )
					{
						const CategoryStruct& ct = ObjectCategories[ indexMap[ catIdx ] ];

						for( int i = 0, e = (size2int_t)ct.ObjectsNames.size() ; i < e ; i ++ )
						{
							if( !stricmp( ct.ObjectsNames[ i ].c_str(), mesh.c_str() ) )
							{
								itemIdx = i;
								itemOffset = r3dTL::Max( itemIdx - 5.f, 0.f );

								if( ForceEntrySelection && ForceEntryCreated )
								{
									damagedIdx = i;
									damagedOffset = r3dTL::Max( damagedIdx - 5.f, 0.f );
								}

								break;
							}
						}

						if( !ForceEntryCreated && ForceEntrySelection )
						{
							const char *dmgMesh = en->MeshName.c_str();

							const char* lastPos = dmgMesh;
							for( const char* newPos; newPos = strchr( lastPos, '/' ); )
							{
								lastPos = newPos + 1;
							}

							dmgMesh = lastPos;

							for( int i = 0, e = (size2int_t)ct.ObjectsNames.size() ; i < e ; i ++ )
							{
								if( !stricmp( ct.ObjectsNames[ i ].c_str(), dmgMesh ) )
								{
									damagedIdx = i;
									damagedOffset = r3dTL::Max( damagedIdx - 5.f, 0.f );

									break;
								}
							}
						}
					}

					const stringlist_t& sl = SoundSys.GetSoundsList();

					for( int i = 0, e = (size2int_t)sl.size(); i < e; i ++ )
					{
						if( !stricmp( en->SoundName.c_str(), sl[ i ].c_str() ) )
						{
							soundIdx = i;
							soundOffset = r3dTL::Max( soundIdx - 5.f, 0.f );

							break;
						}
					}
				}				
			}
		}
	}

	SliderY += 210;

	if( MeshName[ 0 ] && AddModeOn )
	{
		if ( imgui_Button ( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 2, DEFAULT_CONTROLS_HEIGHT, "Add Entry" ) )
		{
			if( g_DamageLib->Exists( CatName, MeshName ) )				
				MessageBox( NULL, "This entry already exists!", "Apologies", MB_OK );
			else
				g_DamageLib->AddEntry( CatName, MeshName );

			UpdateDestructionMeshSettings( MeshName );
		}
	}

	if( entryIdx >= 0 && entryIdx < (int)entryList.size() )
	{
		if ( imgui_Button ( SliderX + DEFAULT_CONTROLS_WIDTH * 0.5f + 2, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 2, DEFAULT_CONTROLS_HEIGHT, "Delete Entry" ) )
		{
			if( g_DamageLib->Exists( entryList[ entryIdx ] ) )
			{
				g_DamageLib->RemoveEntry( entryList[ entryIdx ] );
				UpdateDestructionMeshSettings( MeshName );
			}
		}
	}

	SliderY += DEFAULT_CONTROLS_HEIGHT + 2 ;

	SliderY += imgui_Static( SliderX, SliderY, "Selected Entry Settings" );

	if( entryIdx >= 0 && entryIdx < (int)entryList.size() )
	{
		if( DamageLib::Entry* en = g_DamageLib->GetEntry( entryList[ entryIdx ] ) )
		{
			static float HitPoints ;

			SliderY += imgui_Static( SliderX, SliderY, entryList[ entryIdx ].c_str() );

			HitPoints = en->HitPoints;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Hit Points", &HitPoints, 1.f, 100.f, "%2.0f", 1 );

			en->HitPoints = (int)HitPoints ;

			// damaged mesh
			{
				int prevUseDestructionMesh = en->UseDestructionMesh;

				SliderY += imgui_Checkbox( SliderX, SliderY, "Use Destruction Mesh", &en->UseDestructionMesh, 1 );

				bool needUpdate = ForceEntrySelection;

				if( prevUseDestructionMesh != en->UseDestructionMesh )
				{
					needUpdate = true;
				}

				if( en->UseDestructionMesh )
				{
					if( catIdx >= 0 && catIdx < (int)dirBoundCatList.size() )
					{
						const CategoryStruct& cat = ObjectCategories[ indexMap[ catIdx ] ];

						if( imgui_DrawList( SliderX, SliderY, 360, 200, cat.ObjectsNames, &damagedOffset, &damagedIdx, true, false, true ) )
						{
							needUpdate = true ;
						}

						if( damagedIdx >= 0 && damagedIdx < (int)cat.ObjectsNames.size() )
						{
							strcpy( en->MeshName.str(), CatNames[ indexMap[ catIdx ] ].c_str() );
							strcat( en->MeshName.str(), "/" );
							strcat( en->MeshName.str(), cat.ObjectsNames[ damagedIdx ].c_str() );
						}

						SliderY += 202;
					}
				}

				if( needUpdate )
				{
					r3dSTLString cat, mesh ;
					if( g_DamageLib->DecomposeKey( entryList[ entryIdx ], &cat, &mesh ) )
					{
						UpdateDestructionMeshSettings( mesh.c_str() );
					}
				}
			}

			int HasParticles = en->HasParicles ? 1 : 0 ;
			SliderY += imgui_Checkbox( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Destruction Particles", & HasParticles, 1 );

			int OldHasParticles = en->HasParicles ;

			en->HasParicles = HasParticles ? 1 : 0 ;

			bool NeedUpdate2 = false ; 

			// particles
			if( HasParticles )
			{
				const char PARTICLE_DIR [] = "data\\particles\\";

				r3dSTLString sDirFindParticles = PARTICLE_DIR;
				sDirFindParticles += "*.prt";

				static char sParticleFileSelected[256] = {0};
				static float fDestroyedParticleListOffset = 0;

				sprintf ( sParticleFileSelected, "%s.prt", en->ParticleName.c_str() );
				if ( imgui_FileList (SliderX, SliderY, 360, 200, sDirFindParticles.c_str (), sParticleFileSelected, &fDestroyedParticleListOffset, true ) )
				{
					_splitpath ( sParticleFileSelected, NULL, NULL, en->ParticleName.str(), NULL );

					NeedUpdate2 = true ;
				}

				SliderY += 202;
			}

			int HasSound = en->HasSound ;

			SliderY += imgui_Checkbox( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Destruction Sound", & HasSound, 1 );		

			int OldHasSound = en->HasSound ;

			en->HasSound = !!HasSound ;

			// particles
			if( HasSound )
			{
				const stringlist_t& sl = SoundSys.GetSoundsList();

				if( imgui_DrawList( SliderX, SliderY, 360, 200, sl, &soundOffset, &soundIdx, true, false, true ) )
				{
					NeedUpdate2 = true ;
				}

				if( soundIdx >= 0 && soundIdx < (int)sl.size() )
				{
					strcpy( en->SoundName.str(), sl[ soundIdx ].c_str() );
				}

				SliderY += 202;
			}

			if( !!OldHasParticles ^ !!HasParticles || !!OldHasSound ^ !! HasSound || NeedUpdate2 )
			{
				r3dSTLString cat, mesh ;
				if( g_DamageLib->DecomposeKey( entryList[ entryIdx ], &cat, &mesh ) )
				{
					UpdateDestructionMeshSettings( mesh.c_str() );
				}
			}

			if( en->UseDestructionMesh )
			{
				int HasAnimation = en->HasAnimation;

				SliderY += imgui_Checkbox( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Destruction Animation", &HasAnimation, 1 );

				if( HasAnimation )
				{
					r3dSTLString cat, mesh;
					if( g_DamageLib->DecomposeKey( entryList[ entryIdx ], &cat, &mesh ) )
					{
						static char sAnimSelected[256] = {0};
						static float fAnimListOffset = 0;

						r3dSTLString sDirFind ;

						sDirFind = "Data\\ObjectsDepot\\" + cat + "\\Animations\\*.anm";

						r3dscpy( sAnimSelected, en->AnimName );

						const float FILE_LIST_HEIGHT = 200.f;

						if ( imgui_FileList ( SliderX, SliderY, 360, FILE_LIST_HEIGHT, sDirFind.c_str (), sAnimSelected, &fAnimListOffset ) )
						{
							strncpy( en->AnimName.GetDataPtr(), sAnimSelected, en->AnimName.GetMaxLength() );

							UpdateDestructionMeshSettings( mesh.c_str() );
						} 

						SliderY += FILE_LIST_HEIGHT + 3;
					}
				}

				en->HasAnimation = HasAnimation;
			}
			else
			{
				en->HasAnimation = false;
			}

			int CustomCollision = !!en->DestroyedMeshCollision;

			SliderY += imgui_Checkbox( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Use Destroyed Mesh Collision", &CustomCollision, 1 );

			en->DestroyedMeshCollision = !!CustomCollision;

			// so that ppl don't press that export to ps3 all the time
			SliderY += imgui_Static( SliderX, SliderY, "", 360, true, 99 );
		}
	}

	g_pDesktopManager->End();

}

//------------------------------------------------------------------------

r3dString ExtractFileName( const char* path )
{
	char drive[ 16 ], dir[ MAX_PATH ], name[ MAX_PATH ], ext[ MAX_PATH ] ;

	_splitpath( path, drive, dir, name, ext ) ;

	return name ;
}

//------------------------------------------------------------------------

void ReadCorrespondingSCOMaterial( char* output, const char* fileName )
{
	WIN32_FIND_DATA ffblk;

	const char* Path = "Data/ObjectsDepot/Characters/" ;

	char pathWithMask [ MAX_PATH ] ;

	strcpy( pathWithMask, Path ) ;
	strcat( pathWithMask, "*.sco" ) ;

	HANDLE h = FindFirstFile( pathWithMask, &ffblk );

	r3dString noExtFileName = ExtractFileName( fileName ) ;

	output[ 0 ] = 0 ;

	if( h != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if( ffblk.cFileName[0] != '.') 
			{
				if( !stricmp( ExtractFileName( ffblk.cFileName ).c_str(), noExtFileName.c_str() ) )
				{
					r3dMesh* m = r3dGOBAddMesh( ( r3dString( Path ) + ffblk.cFileName ).c_str() ) ;

					if( m->NumMatChunks != 1 )
					{
						MessageBox( NULL, "Currently we support only 1 material per character mesh", ffblk.cFileName, MB_OK ) ;
						break ;
					}

					strcpy( output, m->MatChunks[ 0 ].Mat->Name ) ;
					break ;
				}
			}
		} while(FindNextFile(h, &ffblk) != 0);

		FindClose(h);
	}
}

//////////////////////////////////////////////////////////////////////////

void Editor_Level::ProcessUtils()
{
	float SliderX = r3dRenderer->ScreenW - 365 ;
	float SliderY = g_ObjectsSectionsStarty;

	SliderY += imgui_Static( SliderX, SliderY, "Utilites" );

	if ( imgui_Button ( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH - 8, DEFAULT_CONTROLS_HEIGHT, "Remove Positional Light Shadows", 0 ) )
	{
		for( GameObject *obj = GameWorld().GetFirstObject() ; obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->Class->Name == "obj_LightHelper" )
			{
				static_cast<obj_LightHelper*>( obj )->LT.bCastShadows = false ;
			}
		}
	}

	SliderY += DEFAULT_CONTROLS_HEIGHT ;

	int drawOrderMode = !!r_limit_renderables->GetInt() ;
	SliderY += imgui_Checkbox( SliderX, SliderY, "Debug Deferred Order", &drawOrderMode, 1 ) ;
	r_limit_renderables->SetInt( !!drawOrderMode ) ;

	if( r_limit_renderables->GetInt() )
	{
		static float rendDrawLimit = 0.f ;
		rendDrawLimit = r_rend_draw_limit->GetInt() ;
		imgui_Value_Slider ( 200.f, r3dRenderer->ScreenH - 256.f, "Deff. draw", &rendDrawLimit, 0.f, r_last_def_rend_count->GetFloat(), "%.0f", true, true, r3dRenderer->ScreenW - 400.f );
		r_rend_draw_limit->SetInt( (int)rendDrawLimit ) ;
	}

	static float CellCountX = 32 ;
	static float CellCountY = 4 ;
	static float CellCountZ = 32 ;

	static bool inited = false ;

	if( !inited && g_pVisibilityGrid)
	{
		CellCountX = g_pVisibilityGrid->GetCellCountX( ) ;
		CellCountY = g_pVisibilityGrid->GetCellCountY( ) ;
		CellCountZ = g_pVisibilityGrid->GetCellCountZ( ) ;

		inited = true ;
	}

	SliderY += imgui_Static( SliderX, SliderY, "Visibility Grid" ) ;

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Cell Count X", &CellCountX, 1, 128, "%2.0f" ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Cell Count Y", &CellCountY, 1,   4, "%2.0f" ) ;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Cell Count Z", &CellCountZ, 1, 128, "%2.0f" ) ;

	bool calc = imgui_Button( SliderX, SliderY, 360.f, 22.f, "Calculate Visibility Grid" ) ;

	SliderY += 22 ;

	bool show = imgui_Button( SliderX, SliderY, 360.f, 22.f, r_show_vis_grid->GetInt() ? "Hide Visibility Grid" : "Show Visibility Grid" ) ;

	if( calc || show )
	{
		if( g_pVisibilityGrid && !( r_show_vis_grid->GetInt() && show && !calc ) 

			&& !( !calc && g_pVisibilityGrid->IsInited() && 
					g_pVisibilityGrid->GetCellCountX() == CellCountX &&
					g_pVisibilityGrid->GetCellCountY() == CellCountY &&
					g_pVisibilityGrid->GetCellCountZ() == CellCountZ 
					)
					
			)			
		{

			
			float miX = +FLT_MAX,	miY = +FLT_MAX,		miZ = +FLT_MAX;
			float maX = -FLT_MAX,	maY = -FLT_MAX,		maZ = -FLT_MAX;

			for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
			{
				GameObject* obj = iter.current;

				if( !VisibiltyGrid::IsVisGridObj( obj ) )
					continue ;

				// force update world bbox
				obj->SetBBoxLocal( obj->GetBBoxLocal() ) ;

				const r3dBoundBox& bbox = obj->GetBBoxWorld() ;

				miX = R3D_MIN( bbox.Org.x, miX ) ;
				miY = R3D_MIN( bbox.Org.y, miY ) ;
				miZ = R3D_MIN( bbox.Org.z, miZ ) ;

				maX = R3D_MAX( bbox.Org.x + bbox.Size.x, maX ) ;
				maY = R3D_MAX( bbox.Org.y + bbox.Size.y, maY ) ;
				maZ = R3D_MAX( bbox.Org.z + bbox.Size.z, maZ ) ;
			}

			if( Terrain )
			{
				const r3dTerrainDesc& desc = Terrain->GetDesc() ;

				miX = R3D_MIN( miX, 0.f ) ;
				miY = R3D_MIN( miY, desc.MinHeight ) ;
				miZ = R3D_MIN( miZ, 0.f ) ;

				maX = R3D_MAX( desc.XSize, maX ) ;
				maY = R3D_MAX( desc.MaxHeight, maY ) ;
				maZ = R3D_MAX( desc.ZSize, maZ ) ;
			}

			g_pVisibilityGrid->Init( CellCountX, CellCountY, CellCountZ, maX - miX, maY - miY, maZ - miZ, 4.f, r3dPoint3D( miX, miY, miZ ), true ) ;

			// do this later out of frame cause a lot of device stuff happens there
			if( calc )
			{
				r_vgrid_calc_one_cell->SetInt( 0 ) ;
				r_need_calc_vis_grid->SetInt( 1 ) ;
			}
		}

		if( show )
		{
			r_show_vis_grid->SetInt( !r_show_vis_grid->GetInt() ) ;
		}
	}

	SliderY += 22 ;

	if( r_show_vis_grid->GetInt() )
	{
		int debugCellVis = !!r_debug_vis_grid_cell->GetInt() ;

		SliderY += imgui_Checkbox( SliderX, SliderY, 360.f, 22.f, "Debug Cell Visibility", &debugCellVis, 1 ) ;

		r_debug_vis_grid_cell->SetInt( debugCellVis ) ;

		if( debugCellVis )
		{
			static float CellX = r_debug_vis_grid_cell_x->GetInt() ;
			static float CellY = r_debug_vis_grid_cell_y->GetInt() ;
			static float CellZ = r_debug_vis_grid_cell_z->GetInt() ;

			SliderY += imgui_Value_Slider( SliderX, SliderY, "Cell X", &CellX, 0.f, CellCountX - 1 , "%2.0f" ) ;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Cell Y", &CellY, 0.f, CellCountY - 1 , "%2.0f" ) ;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Cell Z", &CellZ, 0.f, CellCountZ - 1 , "%2.0f" ) ;

			r_debug_vis_grid_cell_x->SetInt( (int) CellX ) ;
			r_debug_vis_grid_cell_y->SetInt( (int) CellY ) ;
			r_debug_vis_grid_cell_z->SetInt( (int) CellZ ) ;

			// step more cause it's easy to hit it when you navigate debug cell
			SliderY += 33 ;

			if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Calc One Cell" ) )
			{
				r_vgrid_calc_one_cell->SetInt( 1 ) ;
				r_need_calc_vis_grid->SetInt( 1 ) ;
			}

			SliderY += 22.f ;

		}
	}

	SliderY += 22.f ;

	if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Dump textures" ) )
	{
		void DumpTextures();
		DumpTextures();
	}

	SliderY += 44.f ;

	int highlightCasters = !!r_highlight_casters->GetInt() ;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360.f, 22.f, "Highlight Shadow Casters", &highlightCasters, 1 ) ;
	r_highlight_casters->SetInt( highlightCasters ) ;

	SliderY += 22.0f;
	bool isPressed = imgui_Button( SliderX, SliderY, 360.f, 22.f, "Dump profile data" ) ;
	if( isPressed )
	{
		gProfileDataXMLOutFile = r3dGameLevel::GetHomeDir() + r3dString("/profile_data.xml");
		gProfileDataTXTOutFile = r3dGameLevel::GetHomeDir() + r3dString("/profile_data.txt");
		ScheduleProfileDataDump();
	}

	SliderY += 44.0f ;

	//	Massive objects offset by value
	static r3dVector positionOffset(0, 0, 0);
	const float maxF = std::numeric_limits<float>::max();
	
	SliderY += imgui_Static(SliderX, SliderY, "All objects global offset");

	SliderY += imgui_Value_Slider(SliderX, SliderY, "X Offset", &positionOffset.x, -maxF, maxF, "%-02.2f", false);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Y Offset", &positionOffset.y, -maxF, maxF, "%-02.2f", false);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Z Offset", &positionOffset.z, -maxF, maxF, "%-02.2f", false);

	int numObjects = 0;
	if (imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Move game objects"))
	{
		for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
		{
			GameObject* obj = iter.current;
			obj->SetPosition(obj->GetPosition() + positionOffset);
			++numObjects;
		}
	}
	numObjects;
	SliderY += 33.0f;

	if( imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Delete duplicate objects") )
	{
		char msg[ 512 ];
		sprintf( msg, "%d object(s) deleted",	 DeleteObjectDuplicates() );

		MessageBoxA( r3dRenderer->HLibWin, msg, "Info", MB_OK );		
	}

	SliderY += 33.0f;

	if( imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Regenerate all envmap probes") )
	{
		r_need_gen_envmap->SetInt( 0x7fffffff );
	}

	SliderY += 33.0f;

	if( imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Save level materials") )
	{
		r3dMaterialLibrary::UpdateDepotMaterials();
		MessageBoxA( r3dRenderer->HLibWin, "Saved!", "Message", MB_OK );
	}

	SliderY += 33.0f;

	if( imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Reload Object Categories" ) )
	{
		ObjCatInit = 0;
		InitObjCategories();

		MessageBoxA( r3dRenderer->HLibWin, "Reloaded!", "Message", MB_OK );
	}

	SliderY += 33.0f;
}

//////////////////////////////////////////////////////////////////////////

void Editor_Level::ProcessCameraSpots()
{
	static stringlist_t cameraSpotsList;

	float SliderX = r3dRenderer->ScreenW - 365 ;
	float SliderY = g_ObjectsSectionsStarty ;

	static float fOffset = 0;
	static int nIndex = -1;

	static char nameHolder[256] = {0};
	if (nIndex != -1)
	{
		CameraSpotsManager::CameraSpotData &csd = CameraSpotsManager::Instance().GetSpotData(nIndex);
		strcpy_s(nameHolder, _countof(nameHolder), csd.name.c_str());
	}

	if (imgui2_StringValueEx(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, "Spot name:", nameHolder))
	{
		if (nIndex != -1)
		{
			CameraSpotsManager::CameraSpotData &csd = CameraSpotsManager::Instance().GetSpotData(nIndex);
			csd.name = nameHolder;
			cameraSpotsList.clear();
			CameraSpotsManager::Instance().SaveToXML();
		}
	}
	SliderY += 25.0f;
	float btnsY = SliderY;

	if (imgui_Button( SliderX, btnsY, 120.f, 22.f, "Save camera" ))
	{
		if (nameHolder[0]!='\0')
			CameraSpotsManager::Instance().CaptureCurrentCamera(nameHolder);
		else
			CameraSpotsManager::Instance().CaptureCurrentCamera("cam1");
		CameraSpotsManager::Instance().SaveToXML();
		cameraSpotsList.clear();
	}

	if (cameraSpotsList.empty())
	{
		uint32_t count = CameraSpotsManager::Instance().GetSpotsCount();
		cameraSpotsList.reserve(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			cameraSpotsList.push_back(CameraSpotsManager::Instance().GetSpotData(i).name.c_str());
		}
	}

	SliderY += 25.0f;
	bool restoreCamera = false;
	static int prevIdx = nIndex;

	if (imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, r3dRenderer->ScreenH - SliderY * 2, cameraSpotsList, &fOffset, &nIndex ))
	{
		if (g_iDoubleClickCheckerCounter > 0 && nIndex == prevIdx)
		{
			restoreCamera = true;
			g_iDoubleClickCheckerCounter = 0;
		}
		else
		{
			g_iDoubleClickCheckerCounter = 15;
		}
	}
	prevIdx = nIndex;

	SliderY += r3dRenderer->ScreenH - SliderY * 2 + 3;
	if (nIndex != -1)
	{
		if (imgui_Button( SliderX + 120, btnsY, 120.f, 22.f, "Restore camera") || restoreCamera)
		{
			CameraSpotsManager::Instance().RestoreCamera(nIndex);
		}
		if (imgui_Button( SliderX + 240, btnsY, 120.f, 22.f, "Delete spot"))
		{
			CameraSpotsManager::Instance().RemoveSpot(nIndex);
			cameraSpotsList.clear();
			CameraSpotsManager::Instance().SaveToXML();
		}
	}
}

//------------------------------------------------------------------------

void Editor_Level::ProcessObjectDistances()
{
	float SliderX = r3dRenderer->ScreenW - 365;
	float SliderY = g_ObjectsSectionsStarty;

	stringlist_t slist;

	static int bShowOnlyCustomized = 0;

	SliderY += imgui_Checkbox( SliderX, SliderY, "Show only custom", &bShowOnlyCustomized, 1 );

	typedef std::set< r3dMesh* > MeshSet;

	r3dTL::TArray< MeshGameObject* > meshObjs;
	MeshSet meshSet;

	static float minSizeFilter = 0.f;
	static float maxSizeFilter = 100.f;

	static int unlimited = 0;

	if( !unlimited )
	{
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Min Obj Size", &minSizeFilter, 0.f, maxSizeFilter, "%.2f" );
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Max Obj Size", &maxSizeFilter, minSizeFilter, 200.f, "%.2f" );
	}

	SliderY += imgui_Checkbox( SliderX, SliderY, "All Sizes", &unlimited, 1 );

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		if( iter.current->ObjTypeFlags & OBJTYPE_Mesh )
		{
			if( r3dMesh* mesh = iter.current->GetObjectMesh() )
			{
				if( meshSet.find( mesh ) == meshSet.end() )
				{
					float l = mesh->localBBox.Size.Length();

					if( !unlimited && ( l < minSizeFilter || l > maxSizeFilter ) )
						continue;

					meshSet.insert( mesh );
					meshObjs.PushBack( static_cast< MeshGameObject * >( iter.current ) );
				}
			}
		}
	}

	slist.reserve( meshObjs.Count() );

	for( int i = 0, e = meshObjs.Count(); i < e; ++ i )
	{
		MeshGameObject* mgo = meshObjs[ i ];

		int canShow = 1;

		MeshPropertyLib::Entry* entry = g_MeshPropertyLib->GetEntry( mgo->GetMeshLibKey() );

		if( bShowOnlyCustomized && !entry )
			continue;

		slist.push_back( mgo->GetMeshLibKey() );
	}

	const float LIST_HEIGHT = R3D_MAX( 320.f, r3dRenderer->ScreenH - 500.f );

	static float listOffset = 0.f;
	static int selectedIndex = -1;

	imgui_DrawList( SliderX, SliderY, 360.f, LIST_HEIGHT, slist, &listOffset, &selectedIndex );

	SliderY += LIST_HEIGHT + 2;

	if( selectedIndex >= 0 && selectedIndex < (int)meshObjs.Count() )
	{
		MeshGameObject* mgo = meshObjs[ selectedIndex ];

		float newDistance( 0.f );

		int otherwiseDirty = 0;

		SliderY += mgo->DrawDrawDistancePropertyEditor( SliderX, SliderY, &newDistance, &otherwiseDirty );

		if( newDistance != mgo->DrawDistanceSq || otherwiseDirty )
		{
			mgo->DrawDistanceSq;
			mgo->OnDrawDistanceChanged();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void DumpTextures()
{
	FILE* out = fopen( "TexStats.txt", "wt" ) ;

	typedef r3dgfxMultiMap( int, r3dTexture* ) TexesBySize ;
	typedef r3dgameMap( int, int ) TexCountsByID ;

	TexesBySize texesBySize;
	TexCountsByID texCountsByID;

	struct AddTexture
	{
		void operator() ( r3dTexture* tex )
		{
			if( tex )
			{
				(*targIds)[ tex->GetID() ] ++;
			}
		}

		TexCountsByID* targIds;
	} addTexture = { &texCountsByID };

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;

		if( r3dMesh* mesh = obj->GetObjectMesh() )
		{
			for( int i = 0, e = mesh->NumMatChunks; i < e ; i ++ )
			{
				const r3dTriBatch& batch = mesh->MatChunks[ i ];

				if( batch.Mat )
				{
					if( batch.Mat->Texture )
					{
						addTexture( batch.Mat->Texture->GetFullSizeTexture()	);
					}
					addTexture( batch.Mat->NormalTexture					);
					addTexture( batch.Mat->GlossTexture 					);
					addTexture( batch.Mat->EnvPowerTexture					);
					addTexture( batch.Mat->SelfIllumTexture					);

					addTexture( batch.Mat->ThicknessTexture					);
					addTexture( batch.Mat->CamouflageMask					);
					addTexture( batch.Mat->DistortionTexture				);

					addTexture( batch.Mat->NormalDetailTexture				);
					addTexture( batch.Mat->SpecularPowTexture				);
				}
			}
		}
	}

	for( r3dTexture* tex = r3dRenderer->FirstTexture ; tex ; tex = tex->pNext )
	{
		if( !tex->IsDrawable() )
			continue ;
		
		if( tex->GetFlags() & ( r3dTexture::fRenderTarget ) )
			continue ;

		texesBySize.insert( TexesBySize::value_type( tex->GetTextureSizeInVideoMemory(), tex ) ) ;
	}

	int total = 0 ;

	fprintf( out, "%9s - %9s - %4s(%4s) - %8s - %s\n", "Size", "Dims", "Objs", "Refs", "Format", "File" );

	int objectTextureSize = 0;
	int objectTextureCount = 0;

	for( TexesBySize::const_reverse_iterator i = texesBySize.rbegin(), e = texesBySize.rend() ; i != e ; ++ i )
	{
		r3dTexture* tex = i -> second;

		char fmtName[ 64 ];

		void ToString( D3DFORMAT fmt, char* szBuffer );
		ToString( tex->GetD3DFormat(), fmtName );

		if( !( tex->GetFlags() & r3dTexture::fCreated ) )
		{
			if( stristr( tex->getFileLoc().FileName, "ObjectsDepot" ) )
			{
				objectTextureCount ++;
				objectTextureSize += i->first;
			}
		}

		fprintf( out, "%9d - %4dx%4d - %4d(%4d) - %8s - %s\n", i -> first, tex->GetWidth(), tex->GetHeight(), texCountsByID[ tex->GetID() ], tex->GetRefs(), fmtName, tex->GetFlags() & r3dTexture::fCreated ? "PROCEDURAL" : tex->getFileLoc().FileName );
		total += i -> first;
	}

	fprintf( out, "\nObjects: %.2f MB in %d textures\n", objectTextureSize / 1024.f / 1024.f, objectTextureCount );

	fprintf( out, "\nTotal: %.2f MB in %d textures\n", total / 1024.f / 1024.f, (int)texesBySize.size() );

	fclose( out );

	system( "notepad TexStats.txt" );
}

void DumpObjTextures()
{
	FILE* out = fopen( "TexStats.txt", "wt" ) ;

	typedef r3dgfxMultiMap( int, r3dTexture* )TexesBySize ;
	typedef r3dgameMap( int, int ) TexCountsByID ;

	TexesBySize texesBySize;
	TexCountsByID texCountsByID;

	struct AddTexture
	{
		void operator() ( r3dTexture* tex )
		{
			if( tex )
			{
				int& idcount = (*targIds)[ tex->GetID() ];
				
				if( !idcount )
				{
					texesBySize->insert( TexesBySize::value_type( tex->IsDrawable() ? tex->GetTextureSizeInVideoMemory() : 0, tex ) );
				}

				idcount ++;
			}
		}

		TexCountsByID* targIds;
		TexesBySize* texesBySize;
	} addTexture = { &texCountsByID, &texesBySize };

	for( int i = 0, e = GameWorld().GetStaticObjectCount(); i < e ; i ++ )
	{
		GameObject* obj = GameWorld().GetStaticObject( i );

		if( r3dMesh* mesh = obj->GetObjectMesh() )
		{
			for( int i = 0, e = mesh->NumMatChunks; i < e ; i ++ )
			{
				const r3dTriBatch& batch = mesh->MatChunks[ i ];

				if( batch.Mat )
				{
					if( batch.Mat->Texture )
					{
						addTexture( batch.Mat->Texture->GetFullSizeTexture());
					}

					addTexture( batch.Mat->NormalTexture					);
					addTexture( batch.Mat->GlossTexture 					);
					addTexture( batch.Mat->EnvPowerTexture					);
					addTexture( batch.Mat->SelfIllumTexture					);

					addTexture( batch.Mat->ThicknessTexture					);
					addTexture( batch.Mat->CamouflageMask					);
					addTexture( batch.Mat->DistortionTexture				);

					addTexture( batch.Mat->NormalDetailTexture				);
					addTexture( batch.Mat->SpecularPowTexture				);
				}
			}
		}
	}

	int total = 0 ;

	fprintf( out, "%9s - %9s - %4s(%4s) - %8s - %s\n", "Size", "Dims", "Objs", "Refs", "Format", "File" );

	int objectTextureSize = 0;
	int objectTextureCount = 0;

	int unloaded = 0;
	int contradictive = 0;

	for( TexesBySize::const_reverse_iterator i = texesBySize.rbegin(), e = texesBySize.rend() ; i != e ; ++ i )
	{
		r3dTexture* tex = i -> second;

		char fmtName[ 64 ];

		void ToString( D3DFORMAT fmt, char* szBuffer );
		ToString( tex->GetD3DFormat(), fmtName );

		fprintf( out, "%9d - %4dx%4d - %4d(%4d) - %8s - %s\n", i -> first, tex->GetWidth(), tex->GetHeight(), texCountsByID[ tex->GetID() ], tex->GetRefs(), fmtName, tex->GetFlags() & r3dTexture::fCreated ? "PROCEDURAL" : tex->getFileLoc().FileName );

		if( !tex->IsDrawable() )
		{
			unloaded ++;

			if( tex->GetRefs() )
			{
				contradictive ++;
			}
		}

		total += i -> first;
	}

	fprintf( out, "\nUnloaded: %d (%d contradictive)\n", unloaded, contradictive );

	fprintf( out, "\nTotal: %.2f MB in %d textures\n", total / 1024.f / 1024.f, (int)texesBySize.size() );

	fclose( out );

	system( "notepad TexStats.txt" );
}

void DumpObjects()
{
	typedef r3dgameMap( r3dSTLString, int ) string_to_int;

	string_to_int objectMap;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;
		objectMap[ r3dSTLString( obj->Class->Name.c_str() ) ] ++;		
	}

	int total = 0;

	void ConPrint( const char* fmt, ... );

	FILE* fout = fopen( "dumpobj.txt", "wt" );

	for( string_to_int::const_iterator i = objectMap.begin(), e = objectMap.end(); i != e; ++i )
	{
		ConPrint( "%s - %d\n", i->first.c_str(), i->second );
		fprintf( fout, "%s - %d\n", i->first.c_str(), i->second );

		total += i->second;
	}

	ConPrint( "Total: %d\n", total );
	fprintf( fout, "Total: %d\n", total );

	fclose( fout );
	
}

void ThumbStats()
{
	void ConPrint( const char* fmt, ... );

	int fullSize = 0;

	for( int i = 0, e = (int)r3dRenderer->GetThumbnailTextureCount(); i < e; i ++ )
	{
		r3dThumbnailTexture* tt = r3dRenderer->GetThumbnailTexture( i );
		
		if( tt->IsFullSize() )
			fullSize ++;
	}

	ConPrint( "%d total, %d full\n", r3dRenderer->GetThumbnailTextureCount(), fullSize );
	ConPrint( "VRam: %.2f", r3dThumbnailTexture::TotalThumbnailSizeInVMem / 1024.f / 1024.f );
	ConPrint( "SysRam: %.2f", r3dThumbnailTexture::TotalThumbnailSizeInSysMem  / 1024.f / 1024.f );
}

void MemCrash( bool vmem )
{
	r3dOutToLog( "MemCrash: trying to crash the game...\n" );

	HRESULT hr;

	for( int i = 0; ; i ++ )
	{
		IDirect3DTexture9* tex( NULL );
		hr = r3dRenderer->pd3ddev->CreateTexture( 8192, 8192, 0, 0, D3DFMT_A8R8G8B8, vmem ? D3DPOOL_DEFAULT : D3DPOOL_SYSTEMMEM, &tex, NULL );
		D3D_V( hr );

		if( hr == S_OK )
			r3dOutToLog( "MemCrash: texture %d still no crash\n", i );
		else
			break;
	}
}

void DumpHeaps()
{
	HANDLE heaps[ 256 ];
	DWORD c = ::GetProcessHeaps (100, heaps);

	FILE* fout = fopen( "heaps.txt", "wt" );

	fprintf ( fout, "The process has %d heaps.\n", c);

	//get the default heap and the CRT heap (both are among
	//those retrieved above)
	const HANDLE default_heap = ::GetProcessHeap ();

	const HANDLE crt_heap = (HANDLE) _get_heap_handle ();

	INT64 crtTotal = 0;
	INT64 crtCount = 0;

	for (unsigned int i = 0; i < c; i++)
	{
		//query the heap attributes
		ULONG heap_info = 0;

		SIZE_T ret_size = 0;

		int iscrt = 0;

		BOOL res = HeapLock( heaps[ i ] );
		r3d_assert( res );

		if (::HeapQueryInformation (heaps [i],
			HeapCompatibilityInformation,
			&heap_info,
			sizeof (heap_info),
			&ret_size))
		{
			//show the heap attributes

			switch (heap_info)
			{

			case 0:
				fprintf ( fout, "Heap %d is a regular heap.\n", (i + 1));
				break;

			case 1:
				fprintf ( fout, "Heap %d is a heap with look-asides (fast heap).\n", (i + 1));
				break;

			case 2:
				fprintf ( fout, "Heap %d is a LFH (low-fragmentation) heap.\n", (i + 1));
				break;

			default:
				fprintf ( fout, "Heap %d is of unknown type.\n", (i + 1));
				break;
			}

			if (heaps [i] == default_heap)
			{
				fprintf( fout, " This the DEFAULT process heap.\n");
			}

			if (heaps [i] == crt_heap)
			{
				fprintf( fout, " This the heap used by the CRT.\n");  
				iscrt = 1;
			}

			//walk the heap and show each allocated block inside it
			//(the attributes of each entry will differ between
			//DEBUG and RELEASE builds)

			PROCESS_HEAP_ENTRY entry;

			memset (&entry, 0, sizeof (entry));

			int count = 0;

			while (::HeapWalk (heaps [i], &entry))
			{
				if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
				{
					fprintf(fout, " Allocated entry %d: size: %d, overhead: %d.\n", ++count, entry.cbData, entry.cbOverhead);

					if( iscrt )
					{
						crtTotal += entry.cbData;
						crtCount ++;
					}
				}
			}
		}

		res = HeapUnlock( heaps[ i ] );
		r3d_assert( res );
	}

	void ConPrint( const char* fmt, ... );

	ConPrint( "CRT total: %lld", crtTotal );
	ConPrint( "CRT count: %lld", crtCount );
	ConPrint( "CRT avg: %lld", crtTotal / crtCount );

	fclose( fout );
}

static bool MeshPolyComp( GameObject* obj1, GameObject* obj2 )
{
	return obj1->GetObjectMesh()->GetNumPolygons() > obj2->GetObjectMesh()->GetNumPolygons();
}

void Editor_Level::ProcessMeshes()
{
	r3dgameVector( GameObject* ) objects;
	typedef r3dgameHashMap( r3dSTLString, int ) TObjectsList;
	TObjectsList tmpObjectNames;

	ObjectIterator obIter = GameWorld().GetFirstOfAllObjects();

	GameObject* pObject = obIter.current;
	while ( pObject )
	{
		if ( !pObject->GetObjectMesh() )
		{
			obIter = GameWorld().GetNextOfAllObjects( obIter );
			pObject = obIter.current;
			continue;
		}
		r3dSTLString sName( pObject->Name.c_str() );
		TObjectsList::iterator iter = tmpObjectNames.find( sName );
		if ( iter == tmpObjectNames.end() )
		{
			tmpObjectNames.insert( TObjectsList::value_type( sName, 1 ) );
			objects.push_back( pObject );
		}
		else
			iter->second++;

		obIter = GameWorld().GetNextOfAllObjects( obIter );
		pObject = obIter.current;
	}

	std::sort( objects.begin(), objects.end(), MeshPolyComp );
	stringlist_t objectsNames;
	for ( r3dgameVector( GameObject* )::iterator iter = objects.begin(), end = objects.end(); iter != end; ++iter )
		objectsNames.push_back( ( *iter )->Name.c_str() );
	
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	static float fOffset = 0.f;
	static int nIndex = -1;

	imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, r3dRenderer->ScreenH - SliderY * 2, objectsNames, &fOffset, &nIndex );
	if ( nIndex >= 0 && nIndex < int( objectsNames.size() ) )
	{
		r3dSTLString sName = objectsNames[ nIndex ];
		r3dMesh* pMesh = objects[ nIndex ]->GetObjectMesh();
		char buffer[ 256 ];
		sprintf_s (buffer, 256, "Instances: %i\nVertexes: %i\nFaces: %i\nMaterials: %i\nMaterials Memory: %i",
								tmpObjectNames[ sName ],
								pMesh->GetNumVertexes(),
								pMesh->GetNumPolygons(),
								pMesh->GetNumMaterials(),
								pMesh->GetMaterialsSize() );
		imgui_Static ( 370, r3dRenderer->ScreenH - 200, buffer, 360, true, 120 );
	}
}


void Editor_Level :: ProcessDecorators()
{
	DrawDecoratorsUI();
	HandleDecoratorsDraw();
	// 	SliderX = r3dRenderer->ScreenW-365;
	// 	SliderY = 45;
	// 	Draw_Collection_Toolbar();
	// 	iEditMode = (Keyboard->IsPressed(kbsLeftControl)) ? 1 : 0;
	// 	
	// 	if (Keyboard->IsPressed(kbsLeftAlt)) {
	// 		gCam.SetPosition(UI_TargetPos);
	// 	}
	// 
	// 	if(iEditMode) {
	// 		DrawCollectionBrush(); 
	// 	}
	// 	Handle_Brush_Draw();
}

void Editor_Level :: ProcessRoads()
{
	bool bNeedPickerEnable = g_iPickerEnableTicked == 0;

	if ( bNeedPickerEnable )
		g_Manipulator3d.Enable ();

	g_iPickerEnableTicked = 2;

	bool bJustStarted = g_iRoadsTicked == 0;
	g_iRoadsTicked = 2;

	if ( bJustStarted )
	{
		g_Manipulator3d.TypeFilterSet("obj_Road");
		g_Manipulator3d.PickerResetPicked();
	}

	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	extern void gDoRoadEditor(float x, float y);
	gDoRoadEditor(SliderX, SliderY);

	return;
}

void Editor_Level :: ProcessGamePlay ()
{
	bool bJustStarted = g_iGamePlayTicked == 0;
	g_iGamePlayTicked = 2;
	g_iDoubleClickCheckerCounter--;

	bool bNeedPickerEnable = g_iPickerEnableTicked == 0;
	if ( bNeedPickerEnable )
		g_Manipulator3d.Enable ();

	if ( g_Manipulator3d.IsLocked() )
		g_Manipulator3d.Unlock();

	g_iPickerEnableTicked = 2;

	if ( bJustStarted )
		g_Manipulator3d.TypeFilterSet("");

	if ( !g_dTypesList.size () )
		g_dTypesList = AObjectTable_GetRegisteredClassesList ();

	float SliderX = r3dRenderer->ScreenW-365;
	float SliderY = 85;

	SliderY += imgui_Static(SliderX, SliderY, "GAMEPLAY OBJECTS");

	bool bListChanged = imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_LIST_HEIGHT, g_dTypesList, &g_fObjectSelectedTypeOffset, &g_iObjectSelectedType );
	if ( bListChanged || bJustStarted )
	{
		const char * sSelectedType = g_dTypesList[g_iObjectSelectedType].c_str ();
		g_iSelectedObject = 0;
		FillObjectListByType ( sSelectedType );
		
		if ( g_iSelectedObject >= 0 && g_iSelectedObject < (int)g_dObjects.size () )
			g_Manipulator3d.PickedObjectSet ( g_dObjects[g_iSelectedObject] );
		else
			g_Manipulator3d.PickedObjectSet ( NULL );
	}

	SliderY += DEFAULT_LIST_HEIGHT;
	SliderY += 10;

	const char * sSelectedType = g_iObjectSelectedType >= 0 ? g_dTypesList[g_iObjectSelectedType].c_str () :  "";

	char sObjectTitle[256];
	sprintf ( sObjectTitle, "OBJECTS \"%s\"", sSelectedType );
	SliderY += imgui_Static(SliderX, SliderY, sObjectTitle);
	SliderY += 10;

	int iOldObjectIdx = g_iSelectedObject;
	if ( imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_LIST_HEIGHT, g_dObjectNames, &g_fSelectedObjectOffset, &g_iSelectedObject ) )
	{
		if ( g_iSelectedObject >= 0 && g_iSelectedObject < (int)g_dObjects.size () )
			g_Manipulator3d.PickedObjectSet ( g_dObjects[g_iSelectedObject] );
		else
			g_Manipulator3d.PickedObjectSet ( NULL );
	
		// check on double click 
		if ( g_iDoubleClickCheckerCounter > 0 && iOldObjectIdx == g_iSelectedObject )
		{
			float fCameraDirToCam = 200.0f;
			extern BaseHUD* Hud;

			if ( g_iSelectedObject >= 0 && g_iSelectedObject < (int)g_dObjects.size () )
			{
				GameObject * pObjTo = g_dObjects[g_iSelectedObject];
				r3d_assert(pObjTo);

				fCameraDirToCam = pObjTo->GetBBoxWorld().Size.Length() * 2;

				r3dVector vCameraDir = r3dVector ( -1,-1,-1 );
				vCameraDir.Normalize();
				Hud->FPS_Position = pObjTo->GetPosition() - fCameraDirToCam * vCameraDir;
				Hud->SetCameraDir ( vCameraDir );
			}

			g_iDoubleClickCheckerCounter = 0;
		}
		else
		{
			// fix me! check on double click on next 15 frames
			g_iDoubleClickCheckerCounter = 15;
		}
	}

	// update selected item if selection changed
	GameObject * pNewSelObj =  g_Manipulator3d.PickedObject ();
	GameObject * pOldSelObj = NULL;
	if ( g_iSelectedObject >= 0 && g_iSelectedObject < (int)g_dObjects.size () )
		pOldSelObj = g_dObjects[g_iSelectedObject];
	if ( pNewSelObj != pOldSelObj )
	{
		if ( !pNewSelObj )
			g_iSelectedObject = -1;
		else
		{
			int iNewType = -1;
			for ( unsigned int i = 0; i < g_dTypesList.size (); i++ )
			{
				if ( pNewSelObj->Class->Name == g_dTypesList[i].c_str() )
				{
					iNewType = i;
					g_fObjectSelectedTypeOffset = (float)i;
					break;
				}
			}

			if ( iNewType >= 0 )
			{
				g_iObjectSelectedType = iNewType;
				FillObjectListByType ( sSelectedType );

				// find cur element
				for ( unsigned int i = 0; i < g_dObjects.size(); i++ )
				{
					if ( g_dObjects[i] == pNewSelObj )
					{
						g_iSelectedObject = i;
						g_fSelectedObjectOffset = (float)i;
						break;
					}
				}
			}
		}
	}

	if ( g_iSelectedObject >= 0 && g_iSelectedObject < (int)g_dObjects.size () )
	{

		// checkz if OBBox builder works right
#if 0
		static r3dPoint3D buff[ 8 ];
		void buildOBBox( r3dPoint3D* , GameObject* );
		buildOBBox( buff, g_dObjects[g_iSelectedObject] );

		extern uint16_t* g_temproraritetness;

		PushTestMesh( buff, 8, g_temproraritetness, 12 );

		PushUniformLine3D( buff[ 0 ], buff[ 1 ], r3dColor::red );
		PushUniformLine3D( buff[ 1 ], buff[ 3 ], r3dColor::red );
		PushUniformLine3D( buff[ 3 ], buff[ 2 ], r3dColor::red );
		PushUniformLine3D( buff[ 2 ], buff[ 0 ], r3dColor::red );

		PushUniformLine3D( buff[ 4 ], buff[ 5 ], r3dColor::red );
		PushUniformLine3D( buff[ 5 ], buff[ 7 ], r3dColor::red );
		PushUniformLine3D( buff[ 7 ], buff[ 6 ], r3dColor::red );
		PushUniformLine3D( buff[ 6 ], buff[ 4 ], r3dColor::red );

		PushUniformLine3D( buff[ 0 ], buff[ 4 ], r3dColor::red );
		PushUniformLine3D( buff[ 1 ], buff[ 5 ], r3dColor::red );
		PushUniformLine3D( buff[ 2 ], buff[ 6 ], r3dColor::red );
		PushUniformLine3D( buff[ 3 ], buff[ 7 ], r3dColor::red );
#endif

		PushBoundingBox ( g_dObjects[g_iSelectedObject]->GetBBoxWorld() );
	}
}

void CloudsShadowsparams( float& SliderX, float& SliderY, TCloudsShadowParams& params )
{
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Tiling",		&params.fTiling,			0.1f,50,	"%-02.2f",1);	
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Speed U",		&params.fScrollSpeedU,		-100,100,	"%-02.2f",1);	
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Speed V",		&params.fScrollSpeedV,		-100,100,	"%-02.2f",1);	
}

void ProcessCloudsShadows()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	SliderY += imgui_Checkbox( SliderX, SliderY, "Enable", &CloudsShadows_Enable, 1 );

	if ( CloudsShadows_Enable > 0 )
	{
		SliderY += 5;

		SliderY += imgui_Static( SliderX, SliderY, "Texture:" );

		static char szTexName[ R3D_MAX_FILE_NAME ] = { 0 };
		char* p = strrchr( CloudsShadows_TexName, '/' );
		if ( p )
			r3dscpy( szTexName, p + 1 );

		static float offset = 0.f ;

		if ( imgui_FileList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, 200, "Data/clouds/ShadowTex/*", szTexName, &offset, true, false, NULL, ".dds|.tga" ) )
		{
			extern r3dTexture* gCloudTexture;
			if ( gCloudTexture )
				r3dRenderer->DeleteTexture( gCloudTexture );
			sprintf( CloudsShadows_TexName, "Data/clouds/ShadowTex/%s", szTexName );
			gCloudTexture = r3dRenderer->LoadTexture( CloudsShadows_TexName );
		}

		SliderY += 205;

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Blend Factor", &CloudsShadows_BlendFactor, 0, 1.0f, "%-02.2f", 1 );

		SliderY += 5;

		SliderY += imgui_Static( SliderX, SliderY, "Mask Parameters" );
		CloudsShadowsparams( SliderX, SliderY, CloudsShadows_Mask );
		SliderY += 5;

		SliderY += imgui_Static( SliderX, SliderY, "Noise1 Parameters" );
		CloudsShadowsparams( SliderX, SliderY, CloudsShadows_Noise1 );
		SliderY += 5;

		SliderY += imgui_Static( SliderX, SliderY, "Noise2 Parameters" );
		CloudsShadowsparams( SliderX, SliderY, CloudsShadows_Noise2 );
		SliderY += 5;
	}
}

//////////////////////////////////////////////////////////////////////////

void ProcessDecalsEditor(bool globalLib, float SliderX, float SliderY)
{
	static int DecalEditMode = 0;

	const static char* listpaint[ ] =	{ "PLACE/CONFIGURE",	"EDIT"			};

	enum								{ DE_EDIT_CONFIGURE,	DE_EDIT			};

	SliderY += imgui_Toolbar(SliderX, SliderY, 360, 45, &DecalEditMode, 0, listpaint, sizeof listpaint / sizeof listpaint[ 0 ] );

	stringlist_t decalTypeNames;
	r3dTL::TArray<int> indexRemapArr;

	for( UINT i = 0, e = g_pDecalChief->GetTypeCount(); i < e; i ++ )
	{
		const DecalType &t = g_pDecalChief->GetTypeByIdx(i);
		if (t.globalDecal == globalLib)
		{
			indexRemapArr.PushBack(i);
			decalTypeNames.push_back( t.Name.c_str() );
		}
	}

	g_pDesktopManager->Begin( "ed_decals" );

	float offset = g_iSelectedDecalOffset;
	imgui_DrawList( SliderX, SliderY, 360.f, 220.f, decalTypeNames, &offset, &g_iDecalListSelectedItem );
	if (g_iDecalListSelectedItem >= 0)
		g_iSelectedDecalType = indexRemapArr[g_iDecalListSelectedItem];
	g_iSelectedDecalOffset = (int)offset;

	SliderY += 220.f;

	switch( DecalEditMode )
	{
	case DE_EDIT_CONFIGURE:
		{
			static char NewName[ 256 ] = {};

			imgui2_StartArea ( "area1", SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT );
			imgui2_StringValue ( "New DecalName", NewName );
			imgui2_EndArea ();

			SliderY += DEFAULT_CONTROLS_HEIGHT + 3;

			if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Add" ) )
			{
				bool validName = true;
				if( !NewName[ 0 ] || g_pDecalChief->GetTypeID( NewName ) != INVALID_DECAL_ID )
				{
					validName = false;
				}

				if( validName )
				{
					g_pDecalChief->AddType( NewName );
					int idx = g_pDecalChief->GetTypeID(NewName);
					DecalType t = g_pDecalChief->GetTypeByIdx(idx);
					t.globalDecal = globalLib;
					if (!globalLib)
						t.LifeTime = 0;
					g_pDecalChief->UpdateType(idx, t);
				}
				else
				{
					MessageBox( NULL, "Please, specify a valid unique name!", "Error", MB_ICONEXCLAMATION );
				}
			}

			if( imgui_Button( SliderX + 180.f, SliderY, 180.f, 22.f, "Remove" ) )
			{
				if( g_iSelectedDecalType >= 0 && g_iSelectedDecalType < (int)g_pDecalChief->GetTypeCount() )
				{
					r3dString typeName = g_pDecalChief->GetTypeByIdx( g_iSelectedDecalType ).Name;
					g_pDecalChief->RemoveType( typeName );

					g_iSelectedDecalType = -1;
				}
			}

			SliderY += 24.f;

			if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Save" ) )
			{
				//g_pDecalChief->LoadLib();
				g_pDecalChief->SaveLib(r3dGameLevel::GetHomeDir());
			}

			if (globalLib)
			{
				if( imgui_Button( SliderX + 180.0f, SliderY, 180.f, 22.f, "Copy To Local" ) )
				{
					DecalType t = g_pDecalChief->GetTypeByIdx(g_iSelectedDecalType);
					t.Name += "(L)";
					t.globalDecal = false;
					t.LifeTime = 0;
					g_pDecalChief->AddType(t);
				}
			}

			SliderY += 24.f;

			SliderY += imgui_Static( SliderX, SliderY, "Common Parameters" );

			SliderY += 2;

			DecalChief::Settings settings = g_pDecalChief->GetSettings();

			float prevMaxDecalSize = settings.MaxDecalSize;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Max Decal Size", &settings.MaxDecalSize, 1.f, 100.f, "%-2.1f" );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Alpha Cut", &settings.AlphaRef, 0.f, 1.f, "%-2.3f" );

			int relief_val = r_relief_decals->GetBool();
			SliderY += imgui_Checkbox( SliderX, SliderY, "Relief Mapping", &relief_val, 1 );
			r_relief_decals->SetBool( relief_val ? 1 : 0 );

			if( r_relief_decals->GetBool() )
			{
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Relief Depth", &settings.ReliefMappingDepth, 0.0f, 0.25f, "%-2.3f" );
			}

			g_pDecalChief->SetSettings( settings );

			SliderY += imgui_Static( SliderX, SliderY, "Selected Decal Parameters" );

			SliderY += 2;

			if( g_iSelectedDecalType >= 0 && g_iSelectedDecalType < (int)g_pDecalChief->GetTypeCount() )
			{
				static char TexFileName[ 256 ] = {};
				enum TexEditMode
				{
					TEM_DIFFUSE,
					TEM_NORMAL
				} static texEditMode = TEM_DIFFUSE;

				static DecalType type;
				type = g_pDecalChief->GetTypeByIdx( g_iSelectedDecalType );

				SliderY += imgui_Value_Slider( SliderX, SliderY, "U Start",				&type.UVStart[ 0 ], 0.f, 1.f, "%-2.2f" );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "V Start",				&type.UVStart[ 1 ], 0.f, 1.f, "%-2.2f" );

				SliderY += imgui_Value_Slider( SliderX, SliderY, "U End",				&type.UVEnd[ 0 ], 0.f, 1.f, "%-2.2f" );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "V End",				&type.UVEnd[ 1 ], 0.f, 1.f, "%-2.2f" );

				if (globalLib)
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Life Time",			&type.LifeTime, 0.f, 30.f, "%-2.2f" );

				type.UniformScale = !!type.UniformScale ;
				SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Uniform Scale",	&type.UniformScale, 1 );

				if( type.UniformScale )
				{
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Scale",			&type.ScaleX, 0.f, settings.MaxDecalSize, "%-2.2f" );
					type.ScaleY = type.ScaleX ;
				}
				else
				{
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Scale X",			&type.ScaleX, 0.f, settings.MaxDecalSize, "%-2.2f" );
					SliderY += imgui_Value_Slider( SliderX, SliderY, "Scale Y",			&type.ScaleY, 0.f, settings.MaxDecalSize, "%-2.2f" );
				}

				SliderY += imgui_Value_Slider( SliderX, SliderY, "Bkg Blend",			&type.BackgroundBlendFactor, 0.f, 1.f, "%-2.3f" );

				SliderY += imgui_Value_Slider( SliderX, SliderY, "Scale Rnd Var",		&type.ScaleVar, 0.f, 1.f, "%-2.2f" );

				SliderY += imgui_Checkbox( SliderX, SliderY, "Random Rotatoin",			&type.RandomRotation, 1 );

				SliderY += imgui_Value_Slider( SliderX, SliderY, "Far Clip",			&type.ClipFarFactor, 0.0f, 0.5f, "%-2.3f" );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Near Clip",			&type.ClipNearFactor, 0.0f, 1.0f, "%-2.3f" );

				const char* quality_level_str[] = {"Low", "Medium", "High"};
				if(imgui_Button(SliderX+150, SliderY, 30, 20, "<"))
				{
					int curQ = type.MinQuality;
					curQ--;
					if(curQ < r_environment_quality->GetMinVal() )
						curQ = r_environment_quality->GetMinVal() ;

					type.MinQuality = curQ ;
				}
				if(imgui_Button(SliderX+180, SliderY, 30, 20, ">"))
				{
					int curQ = (int)type.MinQuality;
					curQ++;
					if(curQ > r_environment_quality->GetMaxVal() )
						curQ = r_environment_quality->GetMaxVal() ;

					type.MinQuality = curQ ;
				}

				imgui_Static(SliderX, SliderY, "Minimum Quality: ", 100);
				SliderY += imgui_Static(SliderX+100, SliderY, quality_level_str[(int)type.MinQuality-1], 50) ;

				SliderY += imgui_Static( SliderX, SliderY, "Diffuse & Normal+Height Textures" );

				bool changedVal = false;

				float tc[ 16 ] = {	type.UVStart[0],	type.UVStart[1], 
					type.UVEnd[0],		type.UVStart[1],
					type.UVEnd[0],		type.UVEnd[1],
					type.UVStart[0],	type.UVEnd[1],
				};

				r3dDrawBox2D(SliderX - Desktop().GetX() ,SliderY + 2 - Desktop().GetY(), 360, 180, imgui_bkgDlg );

				int DiffX = SliderX + 4 ;
				int DiffY = SliderY + 6 ;
				int DiffW = 180-8;
				int DiffH = 180-8;

				if ( imgui_Button( DiffX - 5, DiffY - 5, DiffW + 8, DiffH + 8, NULL, texEditMode == TEM_DIFFUSE ) )
				{
					texEditMode = TEM_DIFFUSE;
					changedVal = true;
				}

				if( !type.DiffuseTex || !type.NormalTex )
				{
					g_pDecalChief->AutoLoadTextures( g_iSelectedDecalType ) ;

					const DecalType& loadedTexType = g_pDecalChief->GetTypeByIdx( g_iSelectedDecalType ) ;
					type.DiffuseTex = loadedTexType.DiffuseTex ;
					type.NormalTex = loadedTexType.NormalTex ;
				}

				r3dDrawBox2D( DiffX - Desktop().GetX(), DiffY - Desktop().GetY(), DiffW, DiffH, r3dColor(255,255,255), type.DiffuseTex, tc );

				int NormX = DiffX + 180;
				int NormY = DiffY;
				int NormW = DiffW;
				int NormH = DiffH;

				if ( imgui_Button( NormX - 5, NormY - 5, NormW + 8, NormH + 8, NULL, texEditMode == TEM_NORMAL ) )
				{
					texEditMode = TEM_NORMAL;
					changedVal = true;
				}
				r3dDrawBox2D( NormX - Desktop().GetX(), NormY - Desktop().GetY(), NormW, NormH, r3dColor(255,255,255), type.NormalTex, tc );

				SliderY += 182;

				r3dDrawBox2D(SliderX - Desktop().GetX(),SliderY + 2 - Desktop().GetY(), 360, 180, imgui_bkgDlg);

				g_pDesktopManager->End();

				static float offset = 0.f ;

				char drive[ 16 ], dir[ 512 ], name[ 512 ], ext[ 16 ] ;

				_splitpath( type.DiffuseTexName.c_str(), drive, dir, name, ext ) ;

				strcpy( TexFileName, name ) ;
				strcat( TexFileName, ext ) ;

				if ( imgui_FileList(5, 80, 300, 380, "Data\\Decals\\*.dds", TexFileName, &offset, false ) )
				{
					char FullPath[256];

					sprintf( FullPath, "Data\\Decals\\%s", TexFileName );

					switch( texEditMode )
					{
					case TEM_DIFFUSE:
						type.DiffuseTex			= r3dRenderer->LoadTexture( FullPath );
						type.DiffuseTexName		= FullPath ;
						break;

					case TEM_NORMAL:
						type.NormalTex		= r3dRenderer->LoadTexture( FullPath );
						type.NormalTexName	= FullPath ;
						break;
					}
				}

				if (prevMaxDecalSize != settings.MaxDecalSize)
					g_pDecalChief->UpdateAll();

				if( type != g_pDecalChief->GetTypeByIdx( g_iSelectedDecalType ) )
				{
					g_pDecalChief->UpdateType( g_iSelectedDecalType, type );
				}

			}
			else
			{
				g_pDesktopManager->End();
			}

			static int EditDecalIdx = INVALID_DECAL_ID;
			static bool makeUndoEntry = false;

			if( Keyboard->IsPressed(kbsLeftControl) && g_iSelectedDecalType >= 0 && g_iSelectedDecalType < (int) g_pDecalChief->GetTypeCount() )
			{
				if( EditDecalIdx == INVALID_DECAL_ID )
				{
					DecalParams parms;		

					parms.TypeID	= g_iSelectedDecalType;
					parms.Pos		= UI_TargetPos;
					parms.Dir		= UI_TargetNorm;

					EditDecalIdx = g_pDecalChief->Add( parms, false );
					if (makeUndoEntry)
					{
						makeUndoEntry = false;
						const DecalParams &p = g_pDecalChief->GetDecal(EditDecalIdx);
						//	Apply undo entry only for static decals
						if (p.LifeTime == 0)
						{
							DecalUndoAdd *ua = static_cast<DecalUndoAdd*>(g_pUndoHistory->CreateUndoItem(UA_DECAL_ADD));
							if (ua)
							{
								ua->SetDecalParams(p);
							}
						}
					}
				}
				else
				{
					g_pDecalChief->Move( EditDecalIdx, UI_TargetPos, UI_TargetNorm );
				}

				if( imgui_lbr )
				{
					EditDecalIdx = INVALID_DECAL_ID;
					makeUndoEntry = true;
				}
			}
			else
			{
				if( EditDecalIdx != INVALID_DECAL_ID )
				{
					//	This is hack actually, but creation scheme wont give enough controls to solve it 
					//	in more elegant way without big code changes
					int decalCount = static_cast<int>(g_pDecalChief->GetDecalCount());
					if (EditDecalIdx >= decalCount)
					{
						EditDecalIdx = decalCount - 1;
					}
					g_pDecalChief->Remove( EditDecalIdx );
					EditDecalIdx = INVALID_DECAL_ID;
				}
			}

		}
		break;

	case DE_EDIT:
		{
			static int SelectedDecal = -1;
			static int SelectedDecalOffset = 0.f;

			SliderY += 2;
			SliderY += imgui_Static( SliderX, SliderY, "Static Decals" );
			SliderY += 2;

			if( g_iSelectedDecalType >= 0 && g_iSelectedDecalType < (int)g_pDecalChief->GetTypeCount() )
			{
				UINT count = g_pDecalChief->GetStaticDecalCount( g_iSelectedDecalType );

				stringlist_t decalNamez;

				for( UINT i = 0, e = count; i < e; i ++ )
				{
					char buff[ 256 ];

					_snprintf( buff, sizeof buff, "Decal %d", i );

					decalNamez.push_back( buff );
				}

				float offset = (float)SelectedDecalOffset;
				imgui_DrawList( SliderX, SliderY, 360.f, 220.f, decalNamez, &offset, &SelectedDecal );
				SelectedDecalOffset = (int)offset;

				SliderY += 222.f;

				if( SelectedDecal >= 0 && SelectedDecal < (int)count )
				{
					if( const DecalParams* pparms = g_pDecalChief->GetStaticDecal( g_iSelectedDecalType, SelectedDecal ) )
					{
						if (gDecalProxyObj)
							gDecalProxyObj->SelectDecal(g_iSelectedDecalType, SelectedDecal);

						DecalParams parms = *pparms;

						static float rot = 0.f ;

						rot = parms.ZRot * 180.f / R3D_PI;

						SliderY += imgui_Value_Slider( SliderX, SliderY, "Rotation", &rot, 0.f, 360.f, "%-03.1f" );

						parms.ZRot = rot / 180.f * R3D_PI;

						static float scale = 0.f ;

						scale = parms.ScaleCoef ;

						SliderY += imgui_Value_Slider( SliderX, SliderY, "Scale", &scale, 0.f, (float)DECAL_MAX_SCALE_COEF, "%-03.1f" );

						parms.ScaleCoef = scale ;

						g_pDecalChief->UpdateStaticDecal( parms, SelectedDecal );

						SliderY += 2.f;

						if( imgui_Button( SliderX + 180.f, SliderY, 180.f, 22.f, "Delete" ) || 
							Keyboard->WasPressed(kbsDel) 
							)
						{
							const DecalParams *p = g_pDecalChief->GetStaticDecal(g_iSelectedDecalType, SelectedDecal);
							if (p)
							{
								DecalUndoDel *ua = static_cast<DecalUndoDel*>(g_pUndoHistory->CreateUndoItem(UA_DECAL_DEL));
								if (ua)
								{
									ua->SetDecalParams(*p);
								}
							}
							g_pDecalChief->RemoveStaticDecal( g_iSelectedDecalType, SelectedDecal );
						}
					}
				}

				if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Delete All" ) )
				{
					g_pDecalChief->RemoveStaticDecalsOfType( g_iSelectedDecalType );
				}

				SliderY += 22.f;
			}

			if( imgui_lbr && !imgui_IsCursorOver2d() )
			{
				g_iSelectedDecalType = -1;
				SelectedDecal = -1;

				int mx, my;
				Mouse->GetXY( mx, my );

				r3dPoint3D dir;
				r3dScreenTo3D( mx, my, &dir );

				dir.Normalize();

				int picked = g_pDecalChief->PickStaticDecal( gCam, dir, gCam.FarClip );

				if( picked != INVALID_DECAL_ID )
				{
					g_iSelectedDecalType = g_pDecalChief->GetDecal( picked ).TypeID;
					g_iDecalListSelectedItem = -1;

					g_Manipulator3d.PickerResetPicked();
					if( g_iSelectedDecalType == INVALID_DECAL_ID )
					{
						SelectedDecal = -1;
					}
					else
					{
						SelectedDecal = g_pDecalChief->GetDecalIdxInType( picked );

						if (!gDecalProxyObj)
						{
							gDecalProxyObj = (DecalGameObjectProxy*)srv_CreateGameObject("DecalGameObjectProxy", "objDecalGameObjectProxy", r3dPoint3D(0,0,0));
							g_Manipulator3d.AddImmortal( gDecalProxyObj ) ;
						}

						g_Manipulator3d.PickerAddToPicked(gDecalProxyObj);
						g_Manipulator3d.ScaleEnable();
						g_Manipulator3d.Enable();
					}
				}
			}

			g_pDesktopManager->End();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void ProcessWaterEditor ()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	static int g_iWaterListCur = 0;
	static float g_fWaterListOffset = 0.0f;
	obj_WaterPlane * pWaterSelected = NULL;

	bool bJustStarted = g_iProcessWaterPlanesTicked == 0;
	
	g_iProcessWaterPlanesTicked = 2;
	
	if ( bJustStarted )
		UpdateWaterPlaneGrids ( &g_WaterPlaneSettings );
/*
	g_Manipulator3d.TypeFilterSet("obj_WaterPlane");

	bool bNeedPickerEnable = g_iPickerEnableTicked == 0;
	if ( bNeedPickerEnable )
		g_Manipulator3d.Enable ();

	if ( g_Manipulator3d.IsLocked() )
		g_Manipulator3d.Unlock();

	g_iPickerEnableTicked = 2;
*/

	g_pDesktopManager->Begin( "ed_env" );

	SliderY += imgui_Static( SliderX, SliderY, "WATER PLANES EDITOR" );
	if ( imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 4.0f, DEFAULT_CONTROLS_HEIGHT, "Add Water Plane" ) )
	{	
		CreateWaterPlane();
		g_iWaterListCur = obj_WaterPlane::m_dWaterPlanes.Count() - 1;
		/*
		if ( g_iWaterListCur >= 0 )
			g_Manipulator3d.PickedObjectSet(obj_WaterPlane::m_dWaterPlanes[g_iWaterListCur]);
		else
			g_Manipulator3d.PickedObjectSet(NULL);*/
	}

	if ( imgui_Button(SliderX + DEFAULT_CONTROLS_WIDTH * 0.5f + 4, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 4.0f, DEFAULT_CONTROLS_HEIGHT, "Delete Water Plane" ) )
	{
		if ( ( g_iWaterListCur >= 0 ) && ( g_iWaterListCur < (int)obj_WaterPlane::m_dWaterPlanes.Count () ) )
		{
			DeleteWaterPlane(obj_WaterPlane::m_dWaterPlanes[g_iWaterListCur]);
			if ( g_iWaterListCur >= (int)obj_WaterPlane::m_dWaterPlanes.Count () )
				g_iWaterListCur--;
		}
	}

	SliderY += DEFAULT_CONTROLS_HEIGHT;
	
	stringlist_t tWaterList;

	for ( uint32_t i = 0; i < obj_WaterPlane::m_dWaterPlanes.Count (); i++ )
	{
		char szName [MAX_PATH];
		sprintf ( szName, "%s", obj_WaterPlane::m_dWaterPlanes[i]->Name.c_str() );
		tWaterList.push_back(szName);
	}

	if ( imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_LIST_HEIGHT, tWaterList, &g_fWaterListOffset, &g_iWaterListCur ) )
	{
		//if ( ( g_iWaterListCur >= 0 ) && ( g_iWaterListCur < (int)obj_WaterPlane::m_dWaterPlanes.Count () ) )
		//	g_Manipulator3d.PickedObjectSet(obj_WaterPlane::m_dWaterPlanes[g_iWaterListCur]);
	}

	SliderY += DEFAULT_LIST_HEIGHT;
	static obj_WaterPlane *prevSelected = 0;

	if ( g_iWaterListCur >= 0 && g_iWaterListCur < (int)obj_WaterPlane::m_dWaterPlanes.Count () )
		pWaterSelected = obj_WaterPlane::m_dWaterPlanes[g_iWaterListCur];

	if (prevSelected != pWaterSelected)
	{
		InitWaterValueTrackers(pWaterSelected);
		prevSelected = pWaterSelected;
	}

	// water name edit
	if(pWaterSelected)
	{
		imgui2_StartArea("area222", SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT);
		char water_name[128];
		r3dscpy(water_name, pWaterSelected->Name.c_str());
		imgui2_StringValue("Name:", water_name);
		pWaterSelected->Name = water_name;
		pWaterSelected->FileName = water_name;
		SliderY += 22;
		imgui2_EndArea();
		SliderY += 30;
	}

	// restore editor params from water plane
	if(pWaterSelected)
	{

		obj_WaterPlane::Settings oldSettings = g_WaterPlaneSettings;
		g_WaterPlaneSettings = pWaterSelected->GetSettings();


		if (	fabsf(oldSettings.CellGridSize - g_WaterPlaneSettings.CellGridSize) > FLT_EPSILON 
					||
				fabsf(oldSettings.PlaneXSize - g_WaterPlaneSettings.PlaneXSize ) > FLT_EPSILON 
					||
				fabsf(oldSettings.PlaneZSize - g_WaterPlaneSettings.PlaneZSize ) > FLT_EPSILON 
				)
		{
			UpdateWaterPlaneGrids( &g_WaterPlaneSettings );

			pWaterSelected->SetSettings( g_WaterPlaneSettings );
		}
	}

	const static char* listwateredit[2] = { "PAINT", "ERASER" };
	SliderY += imgui_Toolbar(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, 35, &g_iWaterEditMode, 0, listwateredit, 2);

	SliderY += imgui_Value_Slider(SliderX, SliderY, "Brush Radius", &g_fWaterPlaneBrushRadius, 0.1f, 2000.0f, "%-02.2f");

	float fWaterPlaneCellSizeLast = g_WaterPlaneSettings.CellGridSize;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Cell Size", &g_WaterPlaneSettings.CellGridSize, 10.0f, 500.0f, "%-02.2f");

	float fWaterPlaneWidthLast = g_WaterPlaneSettings.PlaneXSize;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Width", &g_WaterPlaneSettings.PlaneXSize, g_NominalWaterPlaneWidth, 8.0f * g_NominalWaterPlaneWidth, "%-02.2f");

	float fWaterPlaneHeightLast = g_WaterPlaneSettings.PlaneZSize;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Height", &g_WaterPlaneSettings.PlaneZSize, g_NominalWaterPlaneHeight, 8.0f * g_NominalWaterPlaneHeight, "%-02.2f");

	if(	fWaterPlaneCellSizeLast != g_WaterPlaneSettings.CellGridSize
			||
		fWaterPlaneWidthLast != g_WaterPlaneSettings.PlaneXSize
			||
		fWaterPlaneHeightLast != g_WaterPlaneSettings.PlaneZSize )
	{
		UpdateWaterPlaneGrids ( &g_WaterPlaneSettings );
	}

	SliderY += imgui_Value_Slider(SliderX, SliderY, "Plane Height", &g_WaterPlaneSettings.WaterPlaneHeight, 0.0f, 1000.0f, "%-02.2f");

	bool bValidIndex = ( g_iWaterListCur >= 0 && g_iWaterListCur < (int)obj_WaterPlane::m_dWaterPlanes.Count () );
	/*
	if ( ( !bValidIndex || obj_WaterPlane::m_dWaterPlanes[g_iWaterListCur] != g_Manipulator3d.PickedObject () ) && g_Manipulator3d.PickedObject () )
	{
		r3d_assert ( g_Manipulator3d.PickedObject ()->Class->Name == "obj_WaterPlane" );
		g_iWaterListCur = GetWaterPlaneIndex ((obj_WaterPlane*)g_Manipulator3d.PickedObject ());
		bValidIndex = true;
	}

	if ( !g_Manipulator3d.PickedObject () )
		g_iWaterListCur = -1;
	else */
	if ( bValidIndex )
		pWaterSelected = obj_WaterPlane::m_dWaterPlanes[g_iWaterListCur];

	// controls
	bool bCanEditWaterPlane = true;
	if( Keyboard->IsPressed(kbsLeftAlt) && (Mouse->IsPressed(r3dMouse::mLeftButton)))
	{
		g_WaterPlaneSettings.WaterPlaneHeight = UI_TargetPos.y;
		bCanEditWaterPlane = false;
	}
	
	if ( pWaterSelected )
	{
		pWaterSelected->SetSettings( g_WaterPlaneSettings );

		SliderY+= 10;

		if ( imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_HEIGHT, DEFAULT_CONTROLS_HEIGHT, g_iWaterPropertiesShown?"-":"+" ) )
		{
			g_iWaterPropertiesShown = 1 - g_iWaterPropertiesShown;
		}
		SliderY+= imgui_Static(SliderX+DEFAULT_CONTROLS_HEIGHT+2, SliderY, "Water Properties", DEFAULT_CONTROLS_WIDTH - DEFAULT_CONTROLS_HEIGHT - 2);

		if ( g_iWaterPropertiesShown )
			SliderY += pWaterSelected->DrawPropertyEditorWater(SliderX, SliderY, 300, 200, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT);

		int32_t iGridW = (int32_t)g_dWaterPlaneGrid.Width();
		int32_t iGridH = (int32_t)g_dWaterPlaneGrid.Height();
		r3dVector vWaterPlaneOffset = r3dVector(0,g_WaterPlaneSettings.WaterPlaneHeight,0);

		// draw grid
		for ( int32_t i = 0; i < iGridW - 1; i++ )
		{
			for ( int32_t j = 0; j < iGridH - 1; j++ )
			{
				uint8_t iGrid = pWaterSelected->GetWaterGridValue(i,j);
				uint8_t iGrid1, iGrid2;
				iGrid1 = iGrid2 = iGrid;
				if(!iGrid)
				{
					if(j>0)
						iGrid1 = pWaterSelected->GetWaterGridValue(i,j-1);
					if(i>0)
						iGrid2 = pWaterSelected->GetWaterGridValue(i-1,j);
				}

				r3dVector v0 = g_dWaterPlaneGrid[j][i];
				r3dVector v1 = g_dWaterPlaneGrid[j][i+1];
				r3dVector v2 = g_dWaterPlaneGrid[j+1][i];

				v0.y = v1.y = v2.y = 0.0f;

				PushUniformLine3D(v0+vWaterPlaneOffset, v1+vWaterPlaneOffset, iGrid1 ? r3dColor::red : r3dColor::white);
				PushUniformLine3D(v0+vWaterPlaneOffset, v2+vWaterPlaneOffset, iGrid2 ? r3dColor::red : r3dColor::white);
			}
		}

		for ( int32_t i = 0; i < iGridW - 1; i++ )
		{
			uint8_t iGrid = pWaterSelected->GetWaterGridValue(i,iGridH-2);

			r3dVector v0 = g_dWaterPlaneGrid[iGridH-1][i];
			r3dVector v1 = g_dWaterPlaneGrid[iGridH-1][i+1];

			v0.y = v1.y = 0.0f;

			PushUniformLine3D(v0+vWaterPlaneOffset, v1+vWaterPlaneOffset, iGrid ? r3dColor::red : r3dColor::white);
		}
		for ( int32_t j = 0; j < iGridH - 1; j++ )
		{
			uint8_t iGrid = pWaterSelected->GetWaterGridValue(iGridW-2,j);

			r3dVector v0 = g_dWaterPlaneGrid[j][iGridW-1];
			r3dVector v1 = g_dWaterPlaneGrid[j+1][iGridW-1];

			v0.y = v1.y = 0.0f;

			PushUniformLine3D(v0+vWaterPlaneOffset, v1+vWaterPlaneOffset, iGrid ? r3dColor::red : r3dColor::white);
		}

		// draw brush
		bool bBrushVisible = false;
		r3dVector vBrushPos(0,0,0);
		{
			int mx, my;
			Mouse->GetXY(mx,my);
			r3dPoint3D dir;
			r3dScreenTo3D(mx, my, &dir);

			if ( fabsf ( dir.y ) > 0.01f )
			{
				float fL = -(gCam.y - g_WaterPlaneSettings.WaterPlaneHeight) / dir.y;
				if ( fL >= 0.0f )
				{
					vBrushPos = gCam + fL*dir;
					bBrushVisible = true;
				}
			}
		}

		if(bBrushVisible && Keyboard->IsPressed(kbsLeftControl))
		{
			PushWaterBrush(vBrushPos, g_fWaterPlaneBrushRadius);
		}

		if(bCanEditWaterPlane && Keyboard->IsPressed(kbsLeftControl) && (Mouse->IsPressed(r3dMouse::mLeftButton)) && !imgui_IsCursorOver2d() && !imgui2_IsCursorOver2d())
		{
			WaterCellsTrackUndoStart(pWaterSelected);

			r3dVector vBrushPos2D = vBrushPos;
			vBrushPos2D.y = 0.0f;
			int iX = int( ( vBrushPos.x - g_dWaterPlaneGrid[ 0 ][ 0 ].x ) / g_WaterPlaneSettings.CellGridSize);
			int iZ = int( ( vBrushPos.z - g_dWaterPlaneGrid[ 0 ][ 0 ].z ) / g_WaterPlaneSettings.CellGridSize);
			int iCount = int(g_fWaterPlaneBrushRadius / g_WaterPlaneSettings.CellGridSize) + 3;

			bool bAnyCellChanged = false;
			for ( int i = iX - iCount; i < iX + iCount; i++ )
			{
				if ( i < 0 || i >= (int)g_dWaterPlaneGrid.Width() - 1 )
					continue;
				for ( int j = iZ - iCount; j < iZ + iCount; j++ )
				{
					if ( j < 0 || j >= (int)g_dWaterPlaneGrid.Height () - 1 )
						continue;

					r3dVector vCellCenter = g_dWaterPlaneGrid[j][i] + 0.5f * r3dVector(g_WaterPlaneSettings.CellGridSize, 0.0f, g_WaterPlaneSettings.CellGridSize);
					vCellCenter.y = 0.0f;

					if ((vCellCenter-vBrushPos2D).LengthSq() < g_fWaterPlaneBrushRadius*g_fWaterPlaneBrushRadius )
					{
						if(g_iWaterEditMode == 0)
							bAnyCellChanged |= pWaterSelected->FillWaterGrid (i,j,false);
						else
							bAnyCellChanged |= pWaterSelected->EraseWaterGrid (i,j,false);
					}
				}
			}

			if(bAnyCellChanged)
			{
				pWaterSelected->UpdateWaterPlane ();
				WaterCellsTrackUndoEnd(pWaterSelected);
			}
		}
	}

	g_pDesktopManager->End ();
}

void ProcessMinimapEditor ()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	SliderY += imgui_Static( SliderX, SliderY, "MINIMAP EDITOR" );
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Minimap Origin.x", &GameWorld().m_MinimapOrigin.x, -10000, 10000, "%.2f", false );
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Minimap Origin.z", &GameWorld().m_MinimapOrigin.z, -10000, 10000, "%.2f", false );
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Minimap Size.x", &GameWorld().m_MinimapSize.x, -10000, 10000, "%.2f", false );
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Minimap Size.z", &GameWorld().m_MinimapSize.z, -10000, 10000, "%.2f", false );
	SliderY+= 10;

	r3dBoundBox bbox;
	bbox.Org.Assign(GameWorld().m_MinimapOrigin.x, 0, GameWorld().m_MinimapOrigin.z);
	bbox.Size.Assign(GameWorld().m_MinimapSize.x, 1000, GameWorld().m_MinimapSize.z);
	r3dRenderer->SetRenderingMode(R3D_BLEND_PUSH|R3D_BLEND_NZ|R3D_BLEND_NOALPHA);
	r3dDrawUniformBoundBox(bbox, gCam, r3dColor::red);
	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

	static r3dTexture* minimapTex = 0 ;

	if ( imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Generate Minimap") )
	{
		if( minimapTex )
		{
			r3dRenderer->DeleteTexture( minimapTex ) ;
		}

		char sFile[1024];
		sprintf(sFile, "%s\\%s", r3dGameLevel::GetHomeDir(), "minimap.dds");

		RenderLevelMinimap( sFile );

		char sMsg[2048];
		sprintf( sMsg, "Saved minimap to: %s", sFile ) ;
		MessageBox( 0, sMsg, "Minimap", MB_OK ) ;

		minimapTex = r3dRenderer->LoadTexture( sFile ) ;
	}

#if 0
	RenderLevelMinimap( NULL );
#endif

	SliderY += DEFAULT_CONTROLS_HEIGHT + 4 ;

	if( minimapTex )
	{
		r3dDrawBox2D( SliderX - Desktop().GetX(), SliderY - Desktop().GetY(), 360, 360, r3dColor(255,255,255), minimapTex ) ;
	}
}

//////////////////////////////////////////////////////////////////////////

bool GetMouseAndPlaneIntersection(float planeHeight, r3dVector *rv)
{
	int x, y;
	Mouse->GetXY(x, y);
	POINT pt;
	pt.x = x;
	pt.y = y;
	r3dRay r = Picker_GetClickRay(pt);
	r3dPlane pln(r3dPoint3D(0, planeHeight, 0), r3dPoint3D(10.0f, planeHeight, 0), r3dPoint3D(0, planeHeight, 10.0f));
	
	return GeomRayPlaneIntersection(r, pln, rv);
}

//////////////////////////////////////////////////////////////////////////

r3dVector GetMouseAndGrassPlanesIntersection()
{
	int x, y;
	Mouse->GetXY(x, y);
	POINT pt;
	pt.x = x;
	pt.y = y;
	r3dRay r = Picker_GetClickRay(pt);

	r3dVector rv(0, 0, 0);
	bool found = false;

	for (uint32_t i = 0; i < GetGrassPlaneManager()->GetPlanesCount(); ++i)
	{
		const GrassPlane &p = GetGrassPlaneManager()->GetPlane(i);
		r3dPlane pln(p.GetBounds().Org, p.GetBounds().Org + r3dVector(p.GetBounds().Size.x, 0, 0), p.GetBounds().Org + r3dVector(0, 0, p.GetBounds().Size.z));
		r3dVector v;
		if (GeomRayPlaneIntersection(r, pln, &v))
		{
			if (p.IsPointInside(v.x, v.z))
			{
				found = true;
				rv = v;
			}
		}
	}

	if (!found)
	{
		GetMouseAndPlaneIntersection(g_NoTerrainLevelBaseHeight, &rv);
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////

int g_GrassPlaneDrawState = 0;

float ProcessGrassConfigure( float SliderX, float SliderY )
{
	float grassViewDistance = r_grass_view_dist->GetFloat();
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Grass View Distance", &grassViewDistance, 4.0f, 512.f, "%-02.2f" );
	r_grass_view_dist->SetFloat( grassViewDistance );

	static int showGrassPlanes = 0;
	SliderY += imgui_Checkbox(SliderX, SliderY, "Show planes", &showGrassPlanes, 1);
	if (showGrassPlanes)
	{
		GetGrassPlaneManager()->DrawPlanes();
	}

	int loadAllGrass = !!r_editor_load_all_grass->GetInt();
	SliderY += imgui_Checkbox( SliderX, SliderY, "Load All Grass", &loadAllGrass, 1 );
	r_editor_load_all_grass->SetInt( !!loadAllGrass );

	if( imgui_Button( SliderX, SliderY, 180.0f, 22.f, "Recalc Grass Height" ) )
	{
		g_pGrassMap->UpdateHeight();
	}

	if( Terrain )
	{
		if( imgui_Button( SliderX + 180.0, SliderY, 180.f, 22.f, "Update Grass Modulation" ) )
		{
			Terrain->SetOrthoDiffuseTextureDirty() ;
		}
	}

	SliderY += 24.f;

	if( imgui_Button( SliderX, SliderY, 180.0, 22.f, "Clear All Grass" ) )
	{
		g_pGrassMap->ClearCells();
	}

	stringlist_t grassTypes;
	g_pGrassGen->FillTypes( grassTypes );

	if( imgui_Button( SliderX + 180.f, SliderY, 180.f, 22.f, "Clear Sel. Grass" ) )
	{
		if ( g_GrassTypeSelected >= 0 && g_GrassTypeSelected < (int)grassTypes.size() )
		{
			g_pGrassMap->ClearGrassType( grassTypes[ g_GrassTypeSelected ].c_str() ) ;
		}
		else
		{
			MessageBox( 0, "Error", "No selected grass type to clear!", MB_OK ) ;
		}				
	}

	SliderY += 33.f;

	if( imgui_Button( SliderX, SliderY, 360, 22.f, "Optimize masks" ) )
	{
		g_pGrassMap->OptimizeMasks();
	}			

	SliderY += 24.f;

	const int GRASS_LIST = 333;

	if ( imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, GRASS_LIST, grassTypes, &g_GrassTypeOffset, &g_GrassTypeSelected ) )
	{
	}

	SliderY += GRASS_LIST;

	if ( g_GrassTypeSelected >= 0 && g_GrassTypeSelected < (int)grassTypes.size() )
	{
		GrassPatchGenSettings sts = g_pGrassGen->GetPatchSettings( g_GrassTypeSelected );

		stringlist_t chunkNamez;

		for( uint32_t i = 0, e = sts.ChunkSettings.Count(); i < e; i ++ )
		{
			chunkNamez.push_back( sts.ChunkSettings[ i ].MeshName.c_str() );
		}

		if( imgui_Button( SliderX, SliderY, 180, 22, "Generate Selected" ) )
		{
			if( !g_pGrassGen->Generate( g_GrassTypeSelected ) )
			{
				MessageBox( NULL, g_LastGrassGenError, "Error", MB_ICONEXCLAMATION );
			}

			g_pGrassLib->Save();
		}

		if( imgui_Button( SliderX + 180, SliderY, 180.f, 22.f, "Save Selected" ) )
		{
			g_pGrassGen->Save( grassTypes[ g_GrassTypeSelected ].c_str() ) ;
		}

		SliderY += 22;

		if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Save All" ) )
		{
			g_pGrassGen->Save();
		}

		SliderY += 22;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Master Density", &sts.Density, 0.01f, 16.f, "%-02.2f", 1 ) ;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Fade Distance", &sts.FadeDistance, 0.05f, R3D_MAX_GRASS_PATCH_DISTANCE_COEF, "%-02.2f", 1 ) ;

		for( int i = 0, e = g_pGrassLib->GetEntryCount() ; i < e ; i ++ )
		{
			const GrassLibEntry& ge = g_pGrassLib->GetEntry( i ) ;

			if( ge.Name == sts.Name )
			{
				GrassLibEntry cge = ge ;
				cge.FadeDistance = sts.FadeDistance ;
				g_pGrassLib->MildUpdateExistingEntry( cge ) ;
			}
		}

		SliderY += imgui_Static( SliderX, SliderY, "Available Meshes" );

		static char SelectedMesh[ MAX_PATH ];

		static bool meshSelected = false ;

		if( imgui_FileList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, 133, GetGrassPath( "*.sco" ).c_str(), SelectedMesh, &g_GrassAvailableMeshOffset) )
		{
			meshSelected = true ;
		}

		SliderY += 133;

		if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Add" ) )
		{
			if( meshSelected )
			{

				GrassChunkGenSettings csts;

				csts.MeshName		= SelectedMesh;

				WIN32_FIND_DATA fd;

				HANDLE h = FindFirstFile( GetGrassPath("*.dds").c_str(), &fd );

				if( h != INVALID_HANDLE_VALUE )
				{
					csts.TextureName = fd.cFileName;
					FindClose( h );
				}

				sts.ChunkSettings.PushBack( csts );

				g_GrassChunkSelected = -1;
			}
		}

		SliderY += 23.f;

		SliderY += imgui_Static( SliderX, SliderY, "Assigned Meshes" );
		SliderY --;

		const int GRASS_CHUNK_LIST = 88;

		if ( imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, GRASS_CHUNK_LIST, chunkNamez, &g_GrassChunkOffset, &g_GrassChunkSelected ) )
		{
			g_GrassTextureOffset	= -1;
			g_GrassTextureSelected	= -1;
		}

		SliderY += GRASS_CHUNK_LIST + 1;

		if( g_GrassChunkSelected >= 0 && g_GrassChunkSelected < (int)sts.ChunkSettings.Count() )
		{
			GrassChunkGenSettings& csts = sts.ChunkSettings[ g_GrassChunkSelected ];

			if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Remove" ) )
			{
				sts.ChunkSettings.Erase( g_GrassChunkSelected );

				g_GrassChunkSelected = -1;

				SliderY += 22.f;
			}
			else
			{
				SliderY += 22.f;

				char SelectedTex[ MAX_PATH ];

				r3dscpy( SelectedTex, csts.TextureName.c_str() );

				SliderY += imgui_Static( SliderX, SliderY, "Textures" );
				SliderY --;

				if( imgui_FileList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, 87, GetGrassPath( "*.dds" ).c_str(), SelectedTex, &g_GrassTextureOffset ) )
				{
					csts.TextureName = SelectedTex;

					if( (size_t)g_GrassTypeSelected < grassTypes.size() )
					{
						int libIdx = g_pGrassLib->GetEntryIdxByName( grassTypes[ g_GrassTypeSelected ].c_str() ) ;

						if( libIdx >= 0 )
						{
							GrassLibEntry gle = g_pGrassLib->GetEntry( libIdx );

							if( g_GrassChunkSelected < (int)gle.Chunks.Count() )
							{
								gle.Chunks[ g_GrassChunkSelected ].Texture = r3dRenderer->LoadTexture( GetGrassPath( SelectedTex ).c_str() );
								g_pGrassLib->MildUpdateExistingEntry( gle );
							}
						}

					}
				}

				static r3dTexture* DisplayTexture( NULL );
				static char CurrentTexName[ MAX_PATH ] = {};

				r3dString CurrentTexPath = GetGrassPath( sts.ChunkSettings[ g_GrassChunkSelected ].TextureName.c_str() );

				if( stricmp( CurrentTexName, CurrentTexPath.c_str()) )
				{
					DisplayTexture = r3dRenderer->LoadTexture( CurrentTexPath.c_str() );
					r3dscpy( CurrentTexName, CurrentTexPath.c_str() );
				}

				SliderY += 88;

				if( DisplayTexture )
				{
					int DIM = 132;

					float sx = SliderX + 180 - DIM / 2 ;
					float sy = SliderY + 20;

					sx -= Desktop().GetX();
					sy -= Desktop().GetY();

					imgui_Static( SliderX, SliderY, "Current Texture" , 360, true, DIM + 24, true );

					r3dDrawBox2D( sx, sy, DIM, DIM, r3dColor(255,255,255) );
					r3dSetFiltering( R3D_BILINEAR, 0 );
					r3dDrawBox2D( sx+2, sy+2, DIM-2, DIM-2, r3dColor(255,255,255), DisplayTexture );

					SliderY += DIM + 25;
				}

				float numVars = csts.NumVariations;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Variations",			&numVars,					1.f,	GrassChunk::MAX_VARIATIONS, 
					"%-02.0f", 1 );
				csts.NumVariations = (int)numVars;

				float density = csts.Density;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Density",				&density,					1.f,	GrassGen::MAX_MESHES_PER_BATCH, 
					"%-02.0f", 1 );
				csts.Density = (int)density;

				SliderY += imgui_Value_Slider( SliderX, SliderY, "Mesh Scale",			&csts.MeshScale,			0.01f,	8.f,		"%-02.2f", 1 );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Min Mesh Scl.",		&csts.MinMeshScaling,		0.1f,	1.f,		"%-02.2f", 1 );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Max Mesh Scl.",		&csts.MaxMeshScaling,		0,		1.f,		"%-02.2f", 1 );
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Rotation Variation",	&csts.RotationVariation,	0,		1.f,		"%-02.2f", 1 );

				bool needUpdate = false ;

				static float alphaRef ;
				alphaRef = csts.AlphaRef * 255.f;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Alpha Ref",			&alphaRef,					0,		255.f,		"%-02.0f", 1 );

				alphaRef /= 255.f ;

				if( alphaRef != csts.AlphaRef )
					needUpdate = true ;

				csts.AlphaRef = alphaRef ;

				static float tintStrength ;
				tintStrength = csts.TintStrength ;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Tint Strength",		&tintStrength,				0,		1.f,		"%-01.2f", 1 );

				if( tintStrength != csts.TintStrength )
					needUpdate = true ;

				csts.TintStrength = tintStrength ;

				static float tintFade ;
				tintFade = csts.TintFade ;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Tint Fade",			&tintFade,					0,		1.f,		"%-01.2f", 1 );

				if( tintFade != csts.TintFade )
					needUpdate = true;

				csts.TintFade = tintFade;

				static float tintFadeStrength;
				tintFadeStrength = csts.TintFadeStrength;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Tint Fade Strength",	&tintFadeStrength,			0,		1.f,		"%-01.2f", 1 );

				if( tintFadeStrength != csts.TintFadeStrength )
					needUpdate = true;

				csts.TintFadeStrength = tintFadeStrength;

				int isStiff = !!csts.IsStiff;

				SliderY += imgui_Checkbox( SliderX, SliderY, "Stiff", &isStiff, 1 );

				if( isStiff != csts.IsStiff )
					needUpdate = true;

				csts.IsStiff = !!isStiff;

				if( needUpdate )
				{
					if( (size_t)g_GrassTypeSelected < grassTypes.size() )
					{
						int libIdx = g_pGrassLib->GetEntryIdxByName( grassTypes[ g_GrassTypeSelected ].c_str() ) ;

						if( libIdx >= 0 )
						{
							GrassLibEntry gle = g_pGrassLib->GetEntry( libIdx );

							if( g_GrassChunkSelected < (int)gle.Chunks.Count() )
							{
								GrassGenSettingsToChunkSettings( gle.Chunks[ g_GrassChunkSelected ], csts );
								g_pGrassLib->MildUpdateExistingEntry( gle );
							}
						}
					}
				}
			}
		}

		g_pGrassGen->SetPatchSettings( g_GrassTypeSelected, sts );
	}

	SliderY += 50;

	static float CellScale = g_pGrassLib->GetSettings().CellScale;

	char CurrentPatchSizeText[ 128 ];
	sprintf( CurrentPatchSizeText, "Current Patch Size: %.2f", g_pGrassLib->GetSettings().CellScale );

	SliderY += imgui_Static( SliderX, SliderY, CurrentPatchSizeText );

	static int MaintainDensity = 1;
	SliderY += imgui_Checkbox( SliderX, SliderY, "Maintain density", &MaintainDensity, 1 );

	if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Apply New Patch Size" ) )
	{
		GrassLib::Settings settings = g_pGrassLib->GetSettings();

		if( MaintainDensity )
		{
			float densityCoef = CellScale / settings.CellScale ;

			densityCoef *= densityCoef;

			for( uint32_t i = 0, e = g_pGrassGen->GetTypeCount(); i < e; i ++ )
			{
				GrassPatchGenSettings settings = g_pGrassGen->GetPatchSettings( i );
				settings.Density *= densityCoef;
				g_pGrassGen->SetPatchSettings( i, settings );
			}
		}

		settings.CellScale = CellScale;
		g_pGrassLib->UpdateSettings( settings );
	}

	SliderY += 24.f ;

	if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Show used types" ) )
	{
		MessageBox( 0, g_pGrassMap->GetUsedTypes().c_str(), "Used Grass Types", MB_OK ) ;
	}

	SliderY += 24.f ;

	int grazBox = !!r_grass_show_boxes->GetInt() ;

	SliderY += imgui_Checkbox( SliderX, SliderY, 180, 22, "Show batches", &grazBox, 1 ) ;

	if( grazBox )
	{
		if ( g_GrassTypeSelected >= 0 && g_GrassTypeSelected < (int)grassTypes.size() )
		{
			char buf[ 512 ] ;
			sprintf( buf, "Showing for %s", grassTypes[ g_GrassTypeSelected ].c_str() ) ;
			SliderY += imgui_Static( SliderX, SliderY, buf ) ;
			g_pGrassMap->SetDebugType( grassTypes[ g_GrassTypeSelected ].c_str() ) ;
		}
		else
		{
			SliderY += imgui_Static( SliderX, SliderY, "Please select grass type to show!" ) ;
		}
	}

	r_grass_show_boxes->SetInt( grazBox ) ;

	if( g_pGrassMap )
	{
		GrassMap::Settings sts = g_pGrassMap->GetSettings();

		sts.CustomTintEnabled = !!sts.CustomTintEnabled;

		SliderY += imgui_Checkbox( SliderX, SliderY, "Custom Grass Tint", &sts.CustomTintEnabled, 1 );

		sts.CustomTintEnabled = !!sts.CustomTintEnabled;

		if( sts.CustomTintEnabled )
		{
			static char SelectedTex[ MAX_PATH ] = { 0 };

			if( sts.CustomTintPath.Length() )
			{
				strcpy( SelectedTex, sts.CustomTintPath.c_str() );
			}

			if( imgui_FileList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, 87, GetGrassPath( "*.dds" ).c_str(), SelectedTex, &g_GrassTintTextureOffset ) )
			{
				sts.CustomTintPath = SelectedTex;
			}

			SliderY += 87;
		}
		else
		{
			if( Terrain && !Terrain->GetDesc().OrthoDiffuseTex )
			{
				r3dOutToLog( "Rebuilding Terrain Ortho Diffuse Tex because it wasn't there!\n" );

				Terrain->SetOrthoDiffuseTextureDirty();
				Terrain->UpdateOrthoDiffuseTexture();
			}

			if( Terrain && Terrain->GetDesc().OrthoDiffuseTex )
			{
				if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Export Automatic Tint" ) )
				{
					r3dString path = GetGrassPath( "autotint.dds" );

					if( D3DXSaveTextureToFile( path.c_str(), D3DXIFF_DDS, Terrain->GetDesc().OrthoDiffuseTex->AsTex2D(), NULL ) == D3D_OK )
					{
						MessageBoxA( r3dRenderer->HLibWin, ( "Saved to " + path ).c_str(), "Success", MB_OK );
					}
					else
					{
						MessageBoxA( r3dRenderer->HLibWin, ( "Failed to save to" + path ).c_str(), "Failure", MB_ICONERROR );
					}
				}
			}

			SliderY += 24.f;
		}

		g_pGrassMap->SetSettings( sts );
	}

	if( Terrain )
	{
		SliderY += imgui_Static( SliderX, SliderY, " " );

		SliderY += imgui_Static( SliderX, SliderY, "Show Grass Texture" );

		stringlist_t strs;

		strs.push_back( "None" );
		strs.push_back( "Tint Texture" );
		strs.push_back( "Normal Texture" );

		static int showWhat = 0;
		static float offset = 0.f;

		imgui_DrawList( SliderX, SliderY, 360.0f, 80.f, strs, &offset, &showWhat );

		SliderY += 80.f;

		r_show_grass_texture->SetInt( showWhat );

		static float GrassTintScale = 0.f ;
		GrassTintScale = r_show_grass_texture_scale->GetFloat() ;

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Tint Tex Scale", &GrassTintScale, 0.125f, 16.f, "%-2.2f" );

		r_show_grass_texture_scale->SetFloat( GrassTintScale );
	}

	// debug grass tile coverage
	if( r_grass_show_debug->GetBool() )
	{
		r3dPoint3D pos;
		pos.x = gCam.x;
		pos.y = 0.f;
		pos.z = gCam.z;
		PushGrassBrush( GrassMap::GetVisRad(), 1.0f, pos );
	}

	return SliderY;
}

float ProcessPaintErase( float SliderX, float SliderY, int bPaint )
{
	r_terrain3_showeditable->SetInt( 1 );

	static float radius = 32.f;

	static float grassTypeOffset;

	const int GRASS_LIST = 433;

	stringlist_t grassTypes;
	g_pGrassGen->FillTypes( grassTypes );

	if ( imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, GRASS_LIST, grassTypes, &grassTypeOffset, &g_PaintGrassTypeIdxes ) )
	{
	}

	SliderY += GRASS_LIST;

	SliderY += 3.f;

	float cellSize = g_pGrassMap->GetTextureCellSize();

	SliderY += imgui_Value_Slider(SliderX, SliderY, "Radius", &radius, cellSize / GrassMap::CELL_MASK_TEX_DIM, cellSize, "%-02.2f", 1 );

	if( Keyboard->IsPressed(kbsLeftControl) && !imgui_IsCursorOver2d() )
	{
		r3dPoint3D brushPT = GetMouseAndGrassPlanesIntersection();
		if (Terrain)
			brushPT = brushPT.y > UI_TerraTargetPos.y ? brushPT : UI_TerraTargetPos;
		PushGrassBrush( radius, 1.0f,  brushPT );

		if ( imgui_lbp && g_PaintGrassTypeIdxes.Count() )
		{
			if( r3dGetTime() - _LastTimeBrush > 0.05f )
			{
				for( int i = 0, e = (int)g_PaintGrassTypeIdxes.Count(); i < e; i ++ )
				{
					int selectionIdx = g_PaintGrassTypeIdxes[ i ];

					const r3dSTLString& grassType = grassTypes[ selectionIdx ] ;
					int idx = g_pGrassLib->GetEntryIdxByName( grassType.c_str() ) ;

					int doable = false ;

					if( idx < 0 )
					{
						if( !g_pGrassGen->Generate( selectionIdx ) )
						{
							MessageBoxA( r3dRenderer->HLibWin, "Couldn't generate selected grass type! Check if corresponding meshes and textures exist or adjust density settings down!", "Error", MB_ICONERROR );
						}
						else
						{
							doable = true ;
						}
					}
					else
					{
						doable = true ;
					}

					if( doable )
					{
						const r3dString& grassType = grassTypes[ g_PaintGrassTypeIdxes[ i ] ].c_str();

						if( !g_pGrassMap->GetUndoItem() )
						{
							g_pGrassMap->CreateUndoItem();
						}

						g_pGrassMap->Paint( brushPT.x, brushPT.z, radius, bPaint ? 1.0f : -1.0f, grassType );
					}
				}

				_LastTimeBrush = r3dGetTime();
			}
		}
		else
		{
			imgui_Static( r3dRenderer->ScreenH2, r3dRenderer->ScreenW2 - 200, "Please add grass types to paint/erase with!", 400, false, 22, true ) ;
		}

		if( !imgui_lbp )
		{
			g_pGrassMap->FlushUndoItem();
		}
	}

	return SliderY;
}

float ProcessGrassPlanes( float SliderX, float SliderY )
{
	GrassPlanesManager *gpm = GetGrassPlaneManager();
	if (!g_GrassPlaneProxyObj)
	{
		g_GrassPlaneProxyObj = (GrassPlaneGameObjectProxy*)srv_CreateGameObject("GrassPlaneGameObjectProxy", "objGrassPlaneGameObjectProxy", r3dPoint3D(0,0,0));
		g_Manipulator3d.AddImmortal( g_GrassPlaneProxyObj ) ;
	}

	static float planesHeight = 3.0f;
	if (g_GrassPlaneDrawState == 0)
	{
		if( imgui_Button( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Add plane" ) )
		{
			//	Start drawing
			g_GrassPlaneDrawState = 1;
		}
		SliderY += 24.0f;

		static int selectedIdx = 0;
		if (imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Delete plane"))
		{
			if (GetGrassPlaneManager()->GetPlanesCount() > 0)
			{
				GetGrassPlaneManager()->DeletePlane(selectedIdx);
			}
		}
		SliderY += 24.0f;

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Planes base height", &planesHeight, -10.0f, 200.0f, "%-02.2f");

		static char nameHolder[256] = {0};
		if (imgui2_StringValueEx(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, "Rename plane:", nameHolder))
		{
			GetGrassPlaneManager()->GetPlane(selectedIdx).name = nameHolder;
			nameHolder[0] = 0;
		}
		SliderY += 25.0f;

		stringlist_t planesList;

		for (uint32_t i = 0; i < gpm->GetPlanesCount(); ++i)
		{
			GrassPlane &gp = gpm->GetPlane(i);
			planesList.push_back(gp.name.c_str());
		}

		static float arrOffset = 0;
		float listHeight = 330.0f;
		if (imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, listHeight, planesList, &arrOffset, &selectedIdx))
		{
			g_GrassPlaneProxyObj->SelectGrassPlane(selectedIdx);
		}
		SliderY += listHeight;

		static int resizeMode = 0;
		if (imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Plane resize mode", resizeMode))
		{
			resizeMode = (resizeMode + 1) % 2;
			g_GrassPlaneProxyObj->SetResizeMode(!!resizeMode);
		}

		SliderY += DEFAULT_CONTROLS_HEIGHT + 2;

		if( !Terrain )
		{
			if( imgui_Button( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Reset Grass Bounds" ) )
			{
				if( MessageBoxA( r3dRenderer->HLibWin, "This will erase all grass on the level. Are you sure?", "WARNING", MB_YESNO ) == IDYES )
				{
					g_pGrassMap->ResetBounds();
				}
			}
		}
	}
	else
	{
		SliderY += imgui_Static(SliderX, SliderY, "Click and hold left mouse button, to draw plane.");
		SliderY += imgui_Static(SliderX, SliderY, "NOTE: Plane can be not visible if it is drawn below level");
		SliderY += imgui_Static(SliderX, SliderY, "geometry. Use \"Planes base height\" slider to adjust");
		SliderY += imgui_Static(SliderX, SliderY, "initial height.");
	}

	static bool leftButtonPressed = false;
	static GrassPlane initialPln;
	if (imgui_lbp)
	{
		if (g_GrassPlaneDrawState == 1)
		{
			leftButtonPressed = true;
			r3dVector drawStart(0, 0, 0);
			if (GetMouseAndPlaneIntersection(planesHeight, &drawStart))
			{
				initialPln.SetOrigin(drawStart, false);
				g_GrassPlaneDrawState = 2;
			}
		}
		else if (g_GrassPlaneDrawState == 2)
		{
			//	Resize our plane according to mouse pos
			r3dVector drawEnd(0, 0, 0);
			if (GetMouseAndPlaneIntersection(planesHeight, &drawEnd))
			{
				drawEnd -= initialPln.GetBounds().Org;
				initialPln.SetSize(drawEnd.x, drawEnd.z, false);
				initialPln.Draw();
			}
		}
	}
	else
	{
		if (g_GrassPlaneDrawState == 2 && leftButtonPressed)
		{
			GrassPlane &pln = GetGrassPlaneManager()->AddPlane();
			pln.SetOrigin(initialPln.GetBounds().Org, false);
			pln.SetSize(initialPln.GetBounds().Size.x, initialPln.GetBounds().Size.z, true);
			g_GrassPlaneDrawState = 0;
		}
		leftButtonPressed = 0;
	}

	gpm->DrawPlanes();
	
	return SliderY;
}
//------------------------------------------------------------------------

float ProcessGrassMasks( float SliderX, float SliderY )
{
	static float grassTypeOffset;
	static int selectedGrassType = -1;

	const int GRASS_LIST = 332;

	stringlist_t grassTypes;
	g_pGrassGen->FillTypes( grassTypes );

	if( imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, GRASS_LIST, grassTypes, &grassTypeOffset, &selectedGrassType ) )
	{

	}


	int grassTypeIdx = -1;
	
	if( selectedGrassType >= 0 && selectedGrassType < (int)grassTypes.size() )
	{
		grassTypeIdx = g_pGrassLib->GetEntryIdxByName( grassTypes[ selectedGrassType ].c_str() );
	}

	SliderY += GRASS_LIST;

	static char MaskName[512];
	static char Path[256];

	static float cellOffsetX = 0.f;
	static float cellOffsetZ = 0.f;

	GrassMap::Info info = g_pGrassMap->GetInfo();

	int texelCountX = info.MaskTexelCountX;
	int texelCountZ = info.MaskTexelCountZ;

	char maskSizeMessage[ 256 ];

	if( Terrain )
	{
		r3dTerrainDesc tdesc = Terrain->GetDesc();

		texelCountX = tdesc.XSize / info.GrassSizeX * texelCountX;
		texelCountZ = tdesc.ZSize / info.GrassSizeZ * texelCountZ;
	}

	sprintf( maskSizeMessage, "Grass mask size: %dx%d", texelCountX, texelCountZ );

	SliderY += imgui_Static( SliderX, SliderY, maskSizeMessage );

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Import Offset X", &cellOffsetX, 0, texelCountX - 1, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Import Offset Z", &cellOffsetZ, 0, texelCountZ - 1, "%-02.0f", 1 );

	cellOffsetX = (int)cellOffsetX;
	cellOffsetZ = (int)cellOffsetZ;

	static float offset = 0.f ;

	if( imgui_FileList(SliderX, SliderY, 360, 200, "Data\\TerrainData\\GrassMasks\\*.dds", MaskName, &offset ) )
	{
		sprintf( Path, "Data\\TerrainData\\GrassMasks\\%s", MaskName );
	}

	SliderY += 206;

	if( grassTypeIdx >= 0 )
	{
		if( imgui_Button( SliderX, SliderY, 360, 25, "Import Grass Mask", 0 ) ) 
		{
			if( g_pGrassMap->ImportMask( Path, grassTypeIdx, cellOffsetX, cellOffsetZ ) )
			{
				char message[ 512 ];
				sprintf( message, "Imported '%s' into %s!", Path, grassTypes[ selectedGrassType ].c_str() );

				MessageBoxA( r3dRenderer->HLibWin, message, "Success", MB_OK );
			}
			else
			{
				MessageBoxA( r3dRenderer->HLibWin, "Failed to import!", "Failure", MB_ICONERROR );
			}
		}
	}

	SliderY += 25.0f;

	static float exportOffsetX = 0.f;
	static float exportOffsetZ = 0.f;
	static float exportSizeX = 0.f;
	static float exportSizeZ = 0.f;

	if( !exportSizeX )
	{
		exportSizeX = R3D_MIN( texelCountX, 4096 );
		exportSizeZ = R3D_MIN( texelCountZ, 4096 );
	}

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Offset X", &exportOffsetX, 0, texelCountX - 1, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Offset Z", &exportOffsetZ, 0, exportSizeZ - 1, "%-02.0f", 1 );

	int maxSizeX = int( texelCountX - exportOffsetX );
	int maxSizeZ = int( texelCountZ - exportOffsetZ );

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Size X", &exportSizeX, 1.0f, maxSizeX, "%-02.0f", 1 );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Export Size Z", &exportSizeZ, 1.0f, maxSizeZ, "%-02.0f", 1 );

	exportOffsetX = (int)exportOffsetX;
	exportOffsetZ = (int)exportOffsetZ;
	exportSizeX = (int)exportSizeX;
	exportSizeZ = (int)exportSizeZ;

	if( imgui_Button( SliderX, SliderY, 360, 25, "Export Grass Mask", 0 ) )
	{
		if( selectedGrassType >= 0 && selectedGrassType < (int)grassTypes.size() )
		{
			char drive[ 16 ], folder[ 1024 ] = "", name[ 1024], ext[ 256 ] ;
			_splitpath( LevelEditName, drive, folder, name, ext ) ;

			for( size_t i = 0, e = strlen( folder ) ; i < e; i ++ )
			{
				char &ch = folder[ i ] ;
				if( ch == '\\' || ch == '/' )
				{
					ch = '_' ;
				}
			}

			sprintf( Path, "Data/TerrainData/GrassMasks/%s%s_%d_mask.dds", folder, name, selectedGrassType );

			if( g_pGrassMap->ExportMask( Path, selectedGrassType, (int)exportOffsetX, (int)exportOffsetZ, (int)exportSizeX, (int)exportSizeZ ) )
				MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Saved to " ) + Path ).c_str(), "Information", MB_OK ) ;
			else
				MessageBoxA( r3dRenderer->HLibWin, ( r3dString( "Failed to save to " ) + Path ).c_str(), "Information", MB_ICONERROR ) ;
		}
	}

	SliderY += 35;

	return SliderY;
}

//------------------------------------------------------------------------

void Editor_Level :: ProcessEnvironment()
{
#if 0
	const static char* list[] =		{ "OCEAN",		"SKY/SUN",		"CLOUD PLANE",	"ATMOSPHERE",		"SIMPLE CLOUDS",	"ENVMAP PROBES",	"GRASS",	"WATER PLANES",		"DECALS",	"SSS",		"RAIN"					};
	enum							{ PE_WATER,		PE_SKY_SUN,		PE_CLOUDPLANE,	PE_ATMSOSPHERE,		PE_SIMPLE_CLOUDS,	PE_ENVMAPPROBE,		PE_GRASS,	PE_WATER_EDITOR,	PE_DECALS,	PE_SSS,		PE_RAIN,	PE_COUNT	};
#else
	const static char* list[] =		{ "OCEAN",		"LIGHT SETUP",	"ATMOSPHERE",	"VOLUMETRIC FOG"	    ,	"CLOUD PLANE",	"GRASS",	"WATER PLANES",		"DECALS",	"SSS",		"RAIN"					};
	enum							{ PE_WATER,		PE_LIGHT_SETUP,	PE_ATMOSPHERE,	PE_FOG3,					PE_CLOUDPLANE,	PE_GRASS,	PE_WATER_EDITOR,	PE_DECALS,	PE_SSS,		PE_RAIN,	PE_COUNT	};
#endif

	imgui_Toolbar( 5, 45, PE_COUNT * 120, 35, &EnvironmentEditMode, 0, list, PE_COUNT, false );

	
	static int __HaveAS = 1;

	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	g_bSkyDomeNeedFullUpdate = false;
	g_bEditorShadingGroupModeSel = false;	

	switch (EnvironmentEditMode )
	{
	case	PE_WATER:
		{
			g_pDesktopManager->Begin( "ed_env" );
			SliderY += imgui_Checkbox(SliderX, SliderY, "OCEAN PLANE",	    &__HaveOcean, 1);
			
			if (__HaveOcean)
			{
				if (!objOceanPlane)
				{
					objOceanPlane = (obj_Lake*)srv_CreateGameObject("obj_Lake", "Ocean", r3dPoint3D(0,0,0));
				}

				SliderY += imgui_Value_Slider(SliderX, SliderY, "Ocean Level",			&LakePlaneY,	0,500,	"%-02.2f",1);

				// NOTE : offsets are OFF because they currently spoil reflection - have to fix that first
#if 0
				SliderY += imgui_Value_Slider( SliderX, SliderY, "OffsetX", &objOceanPlane->PlaneOffsetX, -1000, 1000, "%-2.2f" ) ;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "OffsetZ", &objOceanPlane->PlaneOffsetZ, -1000, 1000, "%-2.2f" ) ;
#endif

				SliderY += imgui_Value_Slider( SliderX, SliderY, "Plane Scale", &objOceanPlane->PlaneScale, 1.0f, 8.0f, "%.2f" );

				SliderY += objOceanPlane->DrawPropertyEditorWater(SliderX, SliderY, 300, 200, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT);

			}
			else if(objOceanPlane) 
			{
				GameWorld().DeleteObject(objOceanPlane);
			}

			
			g_pDesktopManager->End ();
	

		}
		break;


case	PE_FOG3:
		{
			g_pDesktopManager->Begin( "ed_env" );

			float atmoT = r3dGameLevel::Environment->__CurTime / 24.0f ;

			SliderY += imgui_Checkbox(SliderX, SliderY, "VOLUMETRIC FOG",	&r3dGameLevel::Environment->bVolumeFog, 1);


			if(r3dGameLevel::Environment->bVolumeFog)
			{
				SliderY += imgui_Static(SliderX, SliderY, "GENERAL PARAMETERS", 360);

				SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Fog Color", &r3dGameLevel::Environment->Fog_Color, 360, r3dGameLevel::Environment->__CurTime / 24.0f );
				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Fog Density", &r3dGameLevel::Environment->Fog_MaxDensity, 360, 200, 0, 1.0f, 10, 10, 2, 2, atmoT );
				SliderY += 30;

				SliderY += imgui_Static(SliderX, SliderY, "HEIGHT LAYER", 360);
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Max Height", &r3dGameLevel::Environment->Fog_MaxHeight, 1000.0f, 6000.f,	"%-02.2f",1 );
				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Fog Layer Height", &r3dGameLevel::Environment->Fog_HStart, 360, 200, 0, r3dGameLevel::Environment->Fog_MaxHeight, 10, 10, 2, 2, atmoT );
				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Fog Layer Fadeout Range", &r3dGameLevel::Environment->Fog_HRange, 360, 200, 1.0f, 4000, 10, 10, 2, 2, atmoT );
				SliderY += 30;
	
				SliderY += imgui_Static(SliderX, SliderY, "SKYDOME MULTIPLIERS", 360);
				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Height multiplier", &r3dGameLevel::Environment->Fog_SkydomeMult1, 360, 200, 10, 200, 10, 10, 2, 2, atmoT );
				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Range multiplier", &r3dGameLevel::Environment->Fog_SkydomeMult2, 360, 200, 10, 1000, 10, 10, 2, 2, atmoT );
				SliderY += 30;
				
				SliderY += imgui_Static(SliderX, SliderY, "DISTANCE FOG PARAMETERS", 360);
				static float MaxFogDist = 20000.0f;
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Max Distance", &MaxFogDist, 1000.0f, 30000.f,	"%-02.2f",1 );

				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Fog Start Distance", &r3dGameLevel::Environment->Fog_CameraDist, 360, 200, 0, MaxFogDist, 10, 10, 2, 2, atmoT );
				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Fog Range", &r3dGameLevel::Environment->Fog_CameraRange, 360, 200, 1.0f, 20000, 10, 10, 2, 2, atmoT );
				SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Fog Thickness power", &r3dGameLevel::Environment->Fog_CameraExp, 360, 200, 0, 2.5f, 10, 10, 2, 2, atmoT );

				SliderY+= 5;
			}
		
			g_pDesktopManager->End();
		}
		break;


	case	PE_ATMOSPHERE:
		{
			g_pDesktopManager->Begin( "ed_env" );

			float atmoT = r3dGameLevel::Environment->__CurTime / 24.0f ;

	
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Aerial Mip Bias", &r3dGameLevel::Environment->Aerial_MipBias, 0.0f, 10.f,	"%-02.2f",1 );

			SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Aerial Density", &r3dGameLevel::Environment->Aerial_Density, 360, 200, 0.0f, 1.0f, 10, 10, 2, 2, atmoT );
			SliderY += imgui_DrawFloatGradient( SliderX, SliderY, "Aerial Distance", &r3dGameLevel::Environment->Aerial_Distance, 360, 200, 0.5f, 1.5f, 10, 10, 2, 2, atmoT );
		
			g_pDesktopManager->End();
		}
		break;

	case	PE_LIGHT_SETUP:
#if 0
		SliderY = ProcessSkySun( SliderX, SliderY );
#else
		SliderY = ProcessLightSetup( SliderX, SliderY );
#endif
		break;

	case	PE_SSS:
		{
			SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "SSS Enable", &gSSSParams.On, 1 ) ;

			if( gSSSParams.On )
			{
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Scale", &gSSSParams.scale, 0.0f, 4.0f,	"%-02.02f",1);
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Distort", &gSSSParams.distortion, 0.0f, 2.0f,	"%-02.02f",1);
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Power", &gSSSParams.power, 0.01f, 16.0f,	"%-02.02f",1);
				SliderY += imgui_Value_Slider( SliderX, SliderY, "Ambient", &gSSSParams.ambient, 0.0f, 1.0f,	"%-02.02f",1);
				SliderY += imgui_DrawColorPicker( SliderX, SliderY, "Translucency Color", &gSSSParams.translucency, 360, 1 );
			}
		}
		break;

	case PE_GRASS:
		if( r_environment_quality->GetInt() > 1 )
		{
			GrassStats stats = g_pGrassMap->GetGrassStats();

			if( r_show_grass_tile->GetInt() )
			{
				GrassDebugTextureCell debugTexCell;

				int grassType = 0;

				g_pGrassMap->GetDebugGrassTile( &debugTexCell, grassType, UI_TerraTargetPos );

				char msg[ 512 ];
				sprintf( msg, "Tile at cursor: (%d,%d,%d)", debugTexCell.X, debugTexCell.Z, grassType );

				_r3dSystemFont->PrintF( 10, 144, r3dColor24::white, msg );

				if( debugTexCell.MaskTexture )
				{
					r3dDrawBox2D( 110, 177, 256, 256, r3dColor24::white, debugTexCell.MaskTexture );
				}
			}

			if( !( stats.TextureCellsLoading || stats.TextureCellsUnloading ) )
			{
				g_NeedLoadingGrassSign = 0;

				const char* list[] = { "Configure", "Paint", "Erase", "Planes", "Masks" };
				static int EditMode = 0;

				enum
				{
					EG_CONFIGURE,
					EG_PAINT,
					EG_ERASE,
					EG_PLANES,
					EG_MASKS
				};

				int prevMode ( EditMode );

				switch( r_show_grass_texture->GetInt() )
				{
				case 1:
					{
						r3dTexture* tex( NULL );
						if( g_pGrassMap && ( tex = g_pGrassMap->GetTintTexture() ) )
						{
							float scale = r_show_grass_texture_scale->GetFloat();

							float w = scale * tex->GetWidth();
							float h = scale * tex->GetHeight();

							const r3dTerrainDesc& tdesc = Terrain->GetDesc();

							// where are we on the texture?
							float cx = ( gCam.x / tdesc.XSize - 0.5f ) * w;
							float cy = ( 0.5f - gCam.z / tdesc.ZSize ) * h;

							r3dDrawBox2D( r3dRenderer->ScreenW2 - w / 2 - cx, r3dRenderer->ScreenH2 - h / 2 - cy, w, h, r3dColor::white, tex );

							r3dDrawLine2D( r3dRenderer->ScreenW2 - 22.f, r3dRenderer->ScreenH2, r3dRenderer->ScreenW2 + 22.f, r3dRenderer->ScreenH2, 1.f, r3dColor::red );
							r3dDrawLine2D( r3dRenderer->ScreenW2, r3dRenderer->ScreenH2 - 22.f, r3dRenderer->ScreenW2, r3dRenderer->ScreenH2 + 22.f, 1.f, r3dColor::red );
						}
						else
						{
							imgui_Static( r3dRenderer->ScreenW2 - 180, r3dRenderer->ScreenH2, "No grass on level or low decoration quality settings!",  360, false, 25, true );
						}
					}
					break;

				case 2:
					if( r3dTexture* tex = Terrain->GetNormalTexture() )
					{
						float scale = r_show_grass_texture_scale->GetFloat() ;

						float w = scale * tex->GetWidth() ;
						float h = scale * tex->GetHeight() ;

						const r3dTerrainDesc& tdesc = Terrain->GetDesc() ;

						// where are we on the texture?
						float cx = ( gCam.x / tdesc.XSize - 0.5f ) * w ;
						float cy = ( 0.5f - gCam.z / tdesc.ZSize ) * h ;

						r3dDrawBox2D( r3dRenderer->ScreenW2 - w / 2 - cx, r3dRenderer->ScreenH2 - h / 2 - cy, w, h, r3dColor::white, tex );

						r3dDrawLine2D( r3dRenderer->ScreenW2 - 22.f, r3dRenderer->ScreenH2, r3dRenderer->ScreenW2 + 22.f, r3dRenderer->ScreenH2, 1.f, r3dColor::red );
						r3dDrawLine2D( r3dRenderer->ScreenW2, r3dRenderer->ScreenH2 - 22.f, r3dRenderer->ScreenW2, r3dRenderer->ScreenH2 + 22.f, 1.f, r3dColor::red );
					}
					else
					{
						imgui_Static( r3dRenderer->ScreenW2 - 180, r3dRenderer->ScreenH2, "No grass on level or low decoration quality settings!",  360, false, 25, true ) ;
					}
					break;
				}

				imgui_Toolbar( 5, 80, 633, 35, &EditMode, 0, list, sizeof list / sizeof list[0] );

				g_pDesktopManager->Begin( "ed_env" );

				if( g_pGrassMap )
				{
					const GrassMap::Settings& sts = g_pGrassMap->GetSettings();

					r3dBoundBox bbox;

					bbox.Org = r3dPoint3D( sts.XOffset, sts.YMin, sts.ZOffset );
					bbox.Size = r3dPoint3D( sts.XLength, sts.YMax - sts.YMin, sts.ZLength );

					PushBoundingBox( bbox, r3dColor::green, true );
				}

				switch( EditMode )
				{
				case EG_CONFIGURE:
					SliderY = ProcessGrassConfigure( SliderX, SliderY );
					break;
				case EG_PAINT:
				case EG_ERASE:
					SliderY += ProcessPaintErase(SliderX, SliderY, EditMode == EG_PAINT );
					break;
				case EG_PLANES:
					SliderY += ProcessGrassPlanes(SliderX, SliderY);
					break;
				case EG_MASKS:
					SliderY += ProcessGrassMasks(SliderX, SliderY);
					break;
				}
			}
			else
			{
				g_NeedLoadingGrassSign = 1;

				char buffer[ 2048 ];

				int terrainTiles = 0;

				if( Terrain3 )
				{
					terrainTiles = Terrain3->GetNumLoadingMegaTiles();
				}

				sprintf( buffer, "Please wait for %d grass tiles ( + %d terrain tiles ) to load before editing", stats.TextureCellsLoading + stats.TextureCellsUnloading, terrainTiles );

				float sliceStart = r3dGetTime();

				const float SLICE_LENGTH = 0.125f;

				if( g_NeedLoadingTilesSign )
				{
					for( ; r3dGetTime() - sliceStart < SLICE_LENGTH ; )
					{
						ProcessDeviceQueue( sliceStart, SLICE_LENGTH );
						Sleep( 0 );
					}
				}

				imgui_Static( r3dRenderer->ScreenH2, r3dRenderer->ScreenW2 - 220, buffer, 440, false, 22, true ) ;
			}
		}

		g_pDesktopManager->End();

		if( r_environment_quality->GetInt() < 2 )
		{
			imgui_Static( r3dRenderer->ScreenH2, r3dRenderer->ScreenW2 - 200, "Please set environment quality to at least 2 to paint grass!", 400, false, 22, true ) ;
		}

		break;
	case PE_WATER_EDITOR:
		{
			ProcessWaterEditor ();
		}
		break;

	case PE_DECALS:
		{
			const static char* listpaint[] = { "LOCAL(STATIC)", "GLOBAL(DYNAMIC)" };
			enum
			{
				DE_EDIT_TYPE_LOCAL,
				DE_EDIT_TYPE_GLOBAL,
			};
			static int decalType = 0;
			int prevDecalType = decalType;
			SliderY += imgui_Toolbar(SliderX, SliderY, 360, 45, &decalType, 0, listpaint, sizeof listpaint / sizeof listpaint[0]);
			if (decalType != prevDecalType)
			{
				g_iSelectedDecalType = -1;
				g_iDecalListSelectedItem = -1;
			}

			ProcessDecalsEditor(decalType == 1, SliderX, SliderY);
		}
		break;

		case PE_RAIN:
			{
				static bool InSelectParticleMode = false ;

				bool valueChanged = false ;

				if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Select Rain Particle", InSelectParticleMode ) )
				{
					InSelectParticleMode = true ;
					valueChanged = true ;
				}
				SliderY += 22.f ;

				static char ParticlePath[ MAX_PATH ] ;

				strcpy( ParticlePath, r3dGameLevel::Environment->RainParticleSystemName ) ;

				if( InSelectParticleMode )
				{
					static float offset = 0.f ;

					if (imgui_FileList(5, 80, 300, 380, "Data/Particles/*.prt", ParticlePath, &offset, false, valueChanged ))
					{
						for( size_t i = strlen( ParticlePath ) ; i >= 0 ; i -- )
						{
							if( ParticlePath[ i ] == '.' )
							{
								ParticlePath[ i ] = 0 ;
								break ;
							}
						}

						r3dGameLevel::Environment->SetRainParticle( ParticlePath ) ;

						InSelectParticleMode = 0 ;
					}
				}

				char InfoLine[ 256 ] ;

				sprintf( InfoLine, "Selected Particle: %s", ParticlePath ) ;

				SliderY += imgui_Static( SliderX, SliderY, InfoLine ) ;

				if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Clear Rain Particle" ) )
				{
					r3dGameLevel::Environment->ClearRainParticle();
				}
			}
			break ;

	} //switch edit mode
}

float GR_Density	= 0.6f;
float GR_Weight	= 1.0f;
float GR_Decay 	= 0.8f;
float GR_Exposure	= 1.0f;

float FillFilmGrainSettings( PFX_FilmGrain::Settings* fgSettings, float SliderX, float SliderY )
{
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Scale",			&fgSettings->GrainScale,	1.0f,20.0f,	"%-02.2f",1);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Strength",			&fgSettings->Strength,		0.0f,1.0f,	"%-02.2f",1);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "FPS",				&fgSettings->FPS,			4.0f,60.0f,	"%-02.2f",1);

	return SliderY ;
}


void	Editor_Level :: ProcessPost_Camera()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	int lensDirt = !!g_LensDirtEnabled;
	SliderY += imgui_Checkbox(SliderX, SliderY, "Lens Dirt", &lensDirt, 1 );
	g_LensDirtEnabled = !!lensDirt;

	if( lensDirt )
	{
		static PFX_LensDirt_ExtractLuma::Settings sts;
		sts = gPFX_LensDirt_ExtractLuma.GetSettings();

		static PFX_LensDirt::Settings ldsts;		
		ldsts = gPFX_LensDirt.GetSettings();

#if 0
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Opacity", &ldsts.Opacity, 0.f, 1.f, "%.2f" );
#endif

		SliderY += imgui_Static( SliderX, SliderY, "Light Settings" );

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Glow threshold", &sts.Threshold, 0.f, 8.f, "%.2f" );

		static float blurStrength = 0.f;

		blurStrength = r_lens_dirt_blur_width->GetFloat();
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Blur strength", &blurStrength, 1.f, BT_COUNT, "%.0f" );
		r_lens_dirt_blur_width->SetFloat( blurStrength );

		static float blurPassCount = 0.f;
		blurPassCount = r_lens_dirt_blur_count->GetInt();
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Blur passes", &blurPassCount, 2.f, 8.f, "%.0f" );
		r_lens_dirt_blur_count->SetInt( blurPassCount );

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Light Strength", &ldsts.GlowStrength, 0.f, 16.f, "%.2f" );

		SliderY += imgui_Static( SliderX, SliderY, "Sun Settings" );

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Area of Effect", &ldsts.Area, 0.f, 2.f, "%.2f" );
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Pow", &ldsts.Pow, 0.05f, 16.f, "%.2f" );
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Sun Strength", &ldsts.LightStrength, 0.f, 16.f, "%.2f" );

		gPFX_LensDirt_ExtractLuma.SetSettings( sts );
		gPFX_LensDirt.SetSettings( ldsts );
	}

	SliderY += 20;


	SliderY += imgui_Checkbox(SliderX, SliderY, "Film Grain", &FilmGrainOn, 1 );

	if ( FilmGrainOn )
	{
		SliderY = FillFilmGrainSettings( &gFilmGrainSettings, SliderX, SliderY ) ;
	}

	SliderY += 20;
	SliderY += imgui_Checkbox(SliderX, SliderY, "Camera Motion Blur",	    &g_cameraMotionBlurEnabled, 1);

	if ( g_cameraMotionBlurEnabled )
	{
		static PFX_CameraMotionBlur::Settings settings;
		settings = gPFX_CameraMotionBlur.GetSettings();

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Motion Scale", 	&settings.MotionScale,	 0.1f,10.0f, "%-02.02f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Motion Threshold",	&settings.MotionThreshold, 0.0f,0.1f, "%-01.04f",1);

		gPFX_CameraMotionBlur.SetSettings( settings );
	}

	SliderY += 20;
	SliderY += imgui_Checkbox(SliderX, SliderY, "Object Motion Blur",	    &g_ObjectMotionBlurEnabled, 1);

	if ( g_ObjectMotionBlurEnabled )
	{
		PFX_ObjectMotionBlur::Settings sts = gPFX_ObjectMotionBlur.GetSettings();
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Strength", 		&sts.BlurStrength,	 0.1f,5.0f, "%-02.02f",1);
		gPFX_ObjectMotionBlur.SetSettings( sts );
	}
	
	SliderY += 20;

	static int DOF;
	
	SliderY += imgui_Checkbox(SliderX, SliderY, "Depth of field",	&LevelDOF, 1);

	if( LevelDOF )
	{
		SliderY += imgui_Checkbox( SliderX, SliderY, "Near DOF",	&_NEAR_DOF, 1 );
		if ( _NEAR_DOF )
		{
			SliderY += imgui_Value_SliderI( SliderX, SliderY, "Near Taps"	, &DepthOfField_NearTaps	, 1		, 8			, "%d"		, 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Near Start"	, &DepthOfField_NearStart	, 0		, 100		, "%-02.02f"	, 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Near End"		, &DepthOfField_NearEnd		, 0		, 100		, "%-02.02f"	, 1 );

			SliderY += imgui_Value_Slider( SliderX, SliderY, "Near Amp"		, &DepthOfField_NearAmplify	, 1.0f	, 2.0f		, "%-02.3f"	, 1 );						
		}

		SliderY += imgui_Checkbox( SliderX, SliderY, "Far DOF",	&_FAR_DOF, 1 );
		if ( _FAR_DOF )
		{
			SliderY += imgui_Value_SliderI( SliderX, SliderY, "Far Taps"	, &DepthOfField_FarTaps		, 1		, 8			, "%d"		, 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Far Start"	, &DepthOfField_FarStart	, 1	, 2000		, "%-02.02f"	, 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Far End"		, &DepthOfField_FarEnd		, 1	, 2000		, "%-02.02f"	, 1 );
		}
	}

	SliderY += 20;
	SliderY += imgui_Checkbox(SliderX, SliderY, "RadialBlur+Modulate",	&g_RenderRadialBlur, 1);
	if( g_RenderRadialBlur )
	{
		PFX_RadialBlur::Settings sts = gPFX_RadialBlur.GetDefaultSettings();

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Blur start",		&sts.BlurStart,		sts.MIN_BLURSTART,		sts.MAX_BLURSTART,		"%-02.2f", 1 );
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Blur strength",	&sts.BlurStrength,	sts.MIN_BLURSTRENGTH,	sts.MAX_BLURSTRENGTH,	"%-01.3f", 1 );		

		gPFX_RadialBlur.SetDefaultSettings( sts );
	}

	SliderY += 20;

	static int MLAA;

	MLAA = r_mlaa->GetInt();

	SliderY += imgui_Checkbox(SliderX, SliderY, "MLAA",	&MLAA, 1);

	if( r_mlaa->GetInt() != MLAA )
	{
		r_mlaa->SetInt( MLAA );
		UpdateMLAA();
	}

	if( MLAA )
	{
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Color Diff Factor",	&MLAA_DiscontFactor,		0.1f,		10,		"%-02.2f",	1 );

		static int lineLenInPow2;
		
		lineLenInPow2 = r_mlaa_pow2_len->GetInt();

		SliderY += imgui_Value_SliderI( SliderX, SliderY, "Pow2 Line Length",	&lineLenInPow2,				1,			15,		"%d",		1 );

		r_mlaa_pow2_len->SetInt( lineLenInPow2 );
	}

	static int FXAA;
	SliderY += imgui_Checkbox(SliderX, SliderY, "FXAA",	&FXAA, 1);

	if (r_fxaa->GetInt() != FXAA)
	{
		r_fxaa->SetInt(FXAA);
	}

	if (FXAA)
	{
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Subpix quality", &gPFX_FXAA.subpixQuality, 0.0f, 1.0f, "%-02.2f", 1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Edge threshold", &gPFX_FXAA.edgeThreshold, 0.063f, 0.333f, "%-02.2f", 1);
	}

	static int STEREO = 0 ;
	
	STEREO = r_3d_stereo_post_fx->GetInt();

	SliderY += imgui_Checkbox( SliderX, SliderY, "Stereo", &STEREO, 1 );

	r_3d_stereo_post_fx->SetInt( STEREO );

	if( STEREO )
	{
		static float separation, convergance ;

		separation = r_3d_stereo_separation->GetFloat();
		convergance = r_3d_stereo_convergance->GetFloat();

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Setereo Separation",	&separation,		0.0f,		1.f,		"%-02.2f",	1 );
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Setereo Convergance",	&convergance,		0.0f,		1.f,		"%-02.2f",	1 );

		r_3d_stereo_separation->SetFloat( separation );
		r_3d_stereo_convergance->SetFloat( convergance );
	}
}

// turn off optimization for this function, otherwise compiler glitches & ties some parameters' sliders together

float FillBloomSettings( PFX_ExtractBloom::Settings *settings, int* blurPasses, BlurTaps* blurTaps, float SliderX, float SliderY, bool streaks )
{
	SliderY += imgui_Static( SliderX, SliderY, "BLOOM EXTRACTION PARAMETERS" );

	const float UPPER_THRESHOLD = 16.0f ;
	const float MAX_MULTIPLY = 16.0f ;

	SliderY += imgui_Value_Slider(SliderX, SliderY, "Luma Threshold", &settings->Threshold, 0, UPPER_THRESHOLD,	"%-02.2f", 1);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Bloom Amplify", &settings->Power, 0.1f, 2,	"%-02.2f", 1);

	SliderY += imgui_DrawColorPicker(SliderX, SliderY, "Bloom Tint", &settings->MultiplyColor,	360, true );


	SliderY += 5;
	SliderY += imgui_Static( SliderX, SliderY, "GLOW EXTRACTION PARAMETERS" );
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Glow Luma Threshold", &settings->GlowThreshold, 0.f, 1.0f, "%-02.2f" ) ;

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Glow Amplify", &settings->GlowAmplify, 0.01f, 8.0f, "%-02.2f", 1 );

	SliderY += imgui_DrawColorPicker( SliderX, SliderY, "Glow Tint", &settings->GlowTint, 360, false );

	SliderY += 5;
	SliderY += imgui_Static( SliderX, SliderY, "BLUR PARAMETERS" );
	static float fBlurPasses = 0.f ;
	fBlurPasses = (float)*blurPasses;
	SliderY += imgui_Value_Slider( SliderX, SliderY, "Blur Passes", &fBlurPasses, 1.0f, gPFX_ComposeMultibloom.COUNT + 0.01f, "%-02.0f", 1 );
	*blurPasses = (int)fBlurPasses;

	static float fBlurTaps = 0.f ;

	fBlurTaps = ( *blurTaps + 1 ) * 4 + 1 ;

	SliderY += imgui_Value_Slider( SliderX, SliderY, "Blur Taps", &fBlurTaps, 5.f, 33.f, "%.0f" ) ;
	
	int itaps = ( ( ( int )fBlurTaps - 1 ) / 4  ) - 1 ;
	itaps = R3D_MIN( R3D_MAX( itaps, 0 ), BT_COUNT - 1 );
	*blurTaps = (BlurTaps)itaps; 

	SliderY += 5;
	SliderY += imgui_Static( SliderX, SliderY, "LENS STREAKS PARAMETERS" );

	if( streaks )
	{
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Streaks strength", &DirectionalStreaks_Strength, 0.0f, 5.0f, "%-02.3f", 1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Streaks len", &DirectionalStreaks_Length, 0.0f, 0.5f, "%-02.2f", 1);
	}

	return SliderY ;
}

void	Editor_Level :: ProcessPost_Glow()
{
		float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	SliderY += imgui_Checkbox(SliderX, SliderY, "Bloom", &LevelBloom, 1);

	if ( LevelBloom )
	{
		// need to be static cause imgui doesn't support temps
		SliderY = FillBloomSettings( &gHUDFilterSettings[HUDFilter_Default].bloomSettings, &gHUDFilterSettings[HUDFilter_Default].bloomBlurPasses, &gHUDFilterSettings[HUDFilter_Default].bloomBlurTaps, SliderX, SliderY, true );
	}

	SliderY += 20;

	static int SunRays;
	
	SliderY += imgui_Checkbox( SliderX, SliderY, "Volumetric God Rays",	    &LevelSunRays, 1 );

	if( LevelSunRays )
	{
		// need to be static cause imgui doesn't support temps
		static PFX_GodRays::Settings sts;
		sts = gPFX_GodRays.GetSettings();

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Density",			&sts.Density,	0,1,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Decay",			&sts.Decay,		0,1,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Exposure",			&sts.Exposure,	0,3,	"%-02.2f",1);

		gPFX_GodRays.SetSettings( sts );

		static PFX_SeedSunThroughStencil::Settings ssts_sts;

		ssts_sts = gPFX_SeedSunThroughStencil.GetSettings();		

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Weight",			&ssts_sts.Weight,	0,1,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Radius",			&ssts_sts.Radius,	0,0.5,	"%-02.2f",1);

		gPFX_SeedSunThroughStencil.SetSettings( ssts_sts );

	}


	if (LevelBloom)
	{
		SliderY += 20;
		SliderY += imgui_Checkbox(SliderX, SliderY, "Sun Glare",	    &_SunGlare, 1);
		if (_SunGlare)
		{
			SliderY += imgui_Value_SliderI(SliderX, SliderY, "Num Slices", &SG_SlicesNum, 0.0f, 10.0f, "%d");

			// need to be static cause imgui doesn't support temps
			static PFX_SunGlare::Settings sts = gPFX_SunGlare.GetSettings();

			sts.NumSunglares = SG_SlicesNum;

			for( int i = 0, e = sts.NumSunglares; i < e ; i ++ )
			{
				SliderY += imgui_DrawColorPicker(SliderX, SliderY, "tint",			&sts.Tint[ i ],			360,		true		);
				SliderY += imgui_Value_Slider	(SliderX, SliderY, "opacity",		&sts.Opacity[ i ],		0.0f,1.0f,	"%.3f"		);
				SliderY += imgui_Value_Slider	(SliderX, SliderY, "texture scale",	&sts.TexScale[ i ],		-5.0f,5.0f,	"%-02.3f"	);
#if 0
				SliderY += imgui_Value_Slider	(SliderX, SliderY, "blur scale",	&sts.BlurScale[ i ],	-5.0f,5.0f,	"%-02.3f"	);
#endif

				SliderY += imgui_Value_Slider	(SliderX, SliderY, "threshold",		&sts.Threshold[ i ],	0.0f,1.0f,	"%-02.3f"	);
			}

			gPFX_SunGlare.SetSettings( sts );
		}
	}

	//	Explosion effect
	SliderY += 20;

	SliderY += imgui_Static(SliderX, SliderY, "Explosion effect");
	float flt1 = gExplosionVisualController.GetDefaultDuration();
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Duration", &flt1, 0.0f, 0.5f, "%.3f");
	gExplosionVisualController.SetDefaultDuration(flt1);

	float flt2 = gExplosionVisualController.GetMaxVisibleDistance();
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Max visible distance", &flt2, 10.0f, 150.0f, "%-02.3f");
	gExplosionVisualController.SetMaxVisibleDistance(flt2);

	float flt3 = gExplosionVisualController.GetDefaultMaxStrength();
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Max strength", &flt3, 0.05f, 0.3f, "%.3f");
	gExplosionVisualController.SetDefaultMaxStrength(flt3);

	float flt4 = gExplosionVisualController.GetDefaultBrightThreshold();
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Bright Threshold", &flt4, 0.0f, 1.0f, "%.3f");
	gExplosionVisualController.SetDefaultBrightThreshold(flt4);

}

void	Editor_Level :: ProcessPost_Lighting()
{
		float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	static int __RenderGI = 0;


	SliderY += imgui_Checkbox(SliderX, SliderY, "Indirect Lighting",	    &__RenderGI, 1);
	if (__RenderGI)
	{
		SliderY += imgui_Static(SliderX+10, SliderY, "SHADER MODULE MISSING");
	}

#if 0
	int smoothNormals = r_smooth_normals->GetInt();
	SliderY += imgui_Checkbox(SliderX, SliderY, "Smooth normals", &smoothNormals, 1 );;
	r_smooth_normals->SetBool( smoothNormals ? true : false );

	if( smoothNormals )
	{
		float thresh = r_smooth_normals_thresh->GetFloat();
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Smooth Threshhold", &thresh, 0.f, 1.f, "%-2.2f" );
		r_smooth_normals_thresh->SetFloat( thresh );

		float maxd = r_smooth_normals_maxd->GetFloat();
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Distance Clamp", &maxd, 0.0f, 4.f, "%-2.2f" );
		r_smooth_normals_maxd->SetFloat( maxd );

		float amplify = r_smooth_normals_amplify->GetFloat();
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Amplify", &amplify, 0.0f, 128.f, "%-2.2f" );
		r_smooth_normals_amplify->SetFloat( amplify );		

		int taps = r_smooth_normals_taps->GetInt();
		SliderY += imgui_Value_SliderI( SliderX, SliderY, "Tap Count", &taps, 1, 4, "%d" );
		r_smooth_normals_taps->SetVal( taps );
	}
#endif


	static int SSR;
	static int lastSSRMode = r_use_ssr->GetInt();

	SSR = r_use_ssr->GetInt() ? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, "Screen Space Reflections", &SSR, 1 );

	if ( SSR )
	{
		SSR = lastSSRMode;
		float mode = r_use_ssr->GetInt(), steps = r_ssr_steps->GetInt(), blurMode = r_ssr_mips_blur->GetInt(); 
		SliderY += imgui_Value_Slider(SliderX, SliderY, "SSR Mode",				&mode,				1,					3,	"%-02.2f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "SSR Steps",			&steps,				1,					64,	"%-02.2f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "SSR Blur Mode",	&blurMode,		0,					3,	"%-02.2f");

		if (blurMode)
		{
			float blurStrength = r_ssr_mips_blur_strength->GetInt();
			SliderY += imgui_Value_Slider(SliderX, SliderY, "SSR Blur strength",	&blurStrength,		1,					8,	"%-02.2f");
			r_ssr_mips_blur_strength->SetInt((int)blurStrength);
		}

		float blendFactor = r_ssr_blendfactor->GetInt();
		SliderY += imgui_Value_Slider(SliderX, SliderY, "SSR Blend factor",	&blendFactor,		1,					100,	"%-02.2f");
		r_ssr_blendfactor->SetInt((int)blendFactor);

		//float mipsFactor = r_ssr_mips_factor->GetInt();
		//SliderY += imgui_Value_Slider(SliderX, SliderY, "SSR Blur factor",	&mipsFactor,		1,					100,	"%-02.2f");
		//r_ssr_mips_factor->SetInt((int)mipsFactor);



		lastSSRMode = SSR = (int)mode;
		r_ssr_steps->SetInt((int)steps);
		r_ssr_mips_blur->SetInt((int)blurMode);
	}


	r_use_ssr->SetInt(SSR);


	static int SSAO;

	SSAO = r_ssao->GetInt();
	SliderY += imgui_Checkbox( SliderX, SliderY, "Ambient Occlusion", &SSAO, 1 );
	r_ssao->SetInt( SSAO );

	if ( r_ssao->GetBool() )
	{
		static int newSSAOMethod;
		
		newSSAOMethod = r_ssao_method->GetInt() - 1;

		stringlist_t ssaoNames;

		ssaoNames.push_back( "DEFAULT" );
		ssaoNames.push_back( "HQ" );

		static float fOffset = 0.f ;

		const int SSAO_LIST_HEIGHT = 60;

		if ( imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, SSAO_LIST_HEIGHT, ssaoNames, &fOffset, &newSSAOMethod ) )
		{
			r_ssao_method->SetInt( newSSAOMethod + SSM_DEFAULT );

			if( r_ssao_method->GetInt() < SSM_DEFAULT || r_ssao_method->GetInt() >= SSM_COUNT )
			{
				r_ssao_method->SetInt( SSM_DEFAULT );
			}
		}

		SliderY += SSAO_LIST_HEIGHT;

		const SSAOConstraints& constr = g_SSAOConstraints[ r_ssao_method->GetInt() ];
		SSAOSettings& sts = g_SSAOSettings[ r_ssao_method->GetInt() ];

		int optimize = !!r_optimized_ssao->GetInt() ;

		SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Optimize", & optimize, 1 ) ;

		r_optimized_ssao->SetInt( !!optimize ) ;

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Radius",			&sts.Radius,				constr.MinRadius,					constr.MaxRadius,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Depth Range",		&sts.DepthRange,			constr.MinDepthRange,				constr.MaxDepthRange,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Blur Depth Sens.",	&sts.BlurDepthSensitivity,	constr.MinBlurDepthSensitivity,	constr.MaxBlurDepthSensitivity, "%-02.2f", 1 );
		
		float tapCount = (float)sts.BlurTapCount;
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Blur Tap Count",	&tapCount,					constr.MIN_BLUR_TAP_COUNT,			constr.MAX_BLUR_TAP_COUNT, "%-02.0f", 1 );
		sts.BlurTapCount = (int) tapCount;

		float passCount = (float)sts.BlurPassCount;
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Blur Pass Count",	&passCount,					constr.MIN_BLUR_PASS_COUNT,			constr.MAX_BLUR_PASS_COUNT, "%-02.0f", 1 );
		sts.BlurPassCount = (int) passCount;
		
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Blur Strength",	&sts.BlurStrength,			constr.MinBlurStrength,			constr.MaxBlurStrength, "%-02.2f", 1 );
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Expansion Start",	&sts.RadiusExpandStart,		constr.MinRadiusExpandStart,	constr.MaxRadiusExpandStart,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Expansion Coef",	&sts.RadiusExpandCoef,		constr.MinRadiusExpandCoef,		constr.MaxRadiusExpandCoef,	"%-02.3f",1);

#if R3D_ALLOW_TEMPORAL_SSAO
		{
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Temporal tolerance",	&sts.TemporalTolerance,		0.125f,							16.0f,	"%-02.3f", 1 ) ;
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Temporal history",		&sts.TemporalHistoryDepth,	2.0f,							16.0f,	"%-02.0f", 1 ) ;
		}
#endif

		if( r_ssao_method->GetInt() == SSM_HQ )
		{
			SliderY += imgui_Checkbox(SliderX, SliderY,		"Detail Path",		&sts.DetailPathEnable,	1 );

			SliderY += imgui_Value_Slider(SliderX, SliderY, "Detail Ammount",	&sts.DetailStrength,			constr.MinDetailStrength,			constr.MaxDetailStrength,			"%-02.2f",1);
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Detail Radius",	&sts.DetailRadius,				constr.MinDetailRadius,			constr.MaxDetailRadius,			"%-02.2f",1);
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Det. Depth Range",	&sts.DetailDepthRange,			constr.MinDetailDepthRange,		constr.MaxDetailDepthRange,		"%-02.2f",1);
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Det. Exp. Start",	&sts.DetailRadiusExpandStart,	constr.MinDetailRadiusExpandStart,	constr.MaxDetailRadiusExpandStart,	"%-02.2f",1);
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Det. Exp. Coef",	&sts.DetailRadiusExpandCoef,	constr.MinDetailRadiusExpandCoef,	constr.MaxDetailRadiusExpandCoef,	"%-02.2f",1);
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Detail Fadeout",	&sts.DetailFadeOut,			constr.MinDetailFadeOut,			constr.MaxDetailFadeOut,			"%-02.2f",1);
		}

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Brightness",		&sts.Brightness,			constr.MinBrightness,				constr.MaxBrightness,	"%-02.2f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Contrast",			&sts.Contrast,				constr.MinContrast,				constr.MaxContrast,	"%-02.2f",1);

		SliderY += imgui_Static( SliderX, SliderY, "Common Parameters" );
	
		static int halfScaleSSAO;

		halfScaleSSAO = r_half_scale_ssao->GetInt();

		SliderY += imgui_Checkbox( SliderX, SliderY, "Half Scale SSAO",		&halfScaleSSAO, 1 );

		r_half_scale_ssao->SetInt( halfScaleSSAO );

		int gaussianBlur = sts.BlurGaussian;
		SliderY += imgui_Checkbox( SliderX, SliderY, "SSAO Gaussian Blur",	&gaussianBlur, 1 );
		sts.BlurGaussian = gaussianBlur;

		int doubleDepthSSAO = !!r_double_depth_ssao->GetInt();

		SliderY += imgui_Checkbox( SliderX, SliderY, "Double Depth SSAO",	&doubleDepthSSAO, 1 );

		r_double_depth_ssao->SetInt( !!doubleDepthSSAO );

		if( r_double_depth_ssao->GetInt() )
		{
			extern float g_DoubleDepthSSAO_Blur;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Blur Sterngth", &g_DoubleDepthSSAO_Blur, 0.f, 128.f, "%-02.2f", 1, true );
#if 0
			extern float g_DoubleDepthSSAO_BlurSens;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Blur Sens", &g_DoubleDepthSSAO_BlurSens, 0.f, 16.f, "%-02.2f", 1, true );
#endif
		}

		SliderY += imgui_Checkbox( SliderX, SliderY, "Render SSAO target",	&__RenderSSAOOnScreen, 1 );
		SliderY += imgui_Checkbox( SliderX, SliderY, "SSAO blur enable",	&__SSAOBlurEnable, 1 );

		if( __SSAOBlurEnable )
		{
			int blur_w_normals = !!r_ssao_blur_w_normals->GetInt() ;

			SliderY += imgui_Checkbox( SliderX, SliderY, "SSAO blur with normals",	&blur_w_normals, 1 );

			r_ssao_blur_w_normals->SetInt( !!blur_w_normals ) ;
		}

#if R3D_ALLOW_TEMPORAL_SSAO
		if( !optimize )
		{
			SliderY += imgui_Static( SliderX, SliderY, "Temporal Function" ) ;

			stringlist_t tempoFilterModes ;

			tempoFilterModes.push_back( "Disable" ) ;
			tempoFilterModes.push_back( "Filter" ) ;
			tempoFilterModes.push_back( "Optimize" ) ;

			const float LIHEIGHT = 80.f ;
			static float offset = 0.f ;
			static int tempoModeSelection = 0 ;

			tempoModeSelection = R3D_MAX( R3D_MIN( r_ssao_temporal_filter->GetInt(), R3D_SSAO_TEMPORAL_OPTIMIZE ), 0 ) ;

			imgui_DrawList( SliderX, SliderY, 360.f, 80.f, tempoFilterModes, &offset, &tempoModeSelection ) ;

			r_ssao_temporal_filter->SetInt( R3D_MAX( R3D_MIN( tempoModeSelection, R3D_SSAO_TEMPORAL_OPTIMIZE ), 0 ) ) ;

			SliderY += LIHEIGHT ;

			extern int g_SSAO_Temporal_Reveal ;

			if( R3D_SSAO_TEMPORAL_OPTIMIZE == r_ssao_temporal_filter->GetInt() )
			{
				SliderY += imgui_Checkbox( SliderX, SliderY, "SSAO Temporal Reveal", &g_SSAO_Temporal_Reveal, 1 ) ;
			}
		}
		else
		{
			r_ssao_temporal_filter->SetInt( 0 ) ;
		}
#endif
	}

}


void Editor_Level :: ProcessPost_NightVision()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	int nv = 0;

	nv = r_hud_filter_mode->GetInt() == HUDFilter_NightVision;
	SliderY += imgui_Checkbox( SliderX, SliderY, "Night Vision Enable (for testing)", &nv, 1 );

	r_hud_filter_mode->SetInt( nv ? HUDFilter_NightVision : HUDFilter_Default ) ;


#if 0
	nv == 1 ? gNightVisionController.On() : gNightVisionController.Off();
#endif

	SliderY += imgui_Static( SliderX, SliderY, "Night Vision Bloom" ) ;
	SliderY = FillBloomSettings( &gHUDFilterSettings[HUDFilter_NightVision].bloomSettings, &gHUDFilterSettings[HUDFilter_NightVision].bloomBlurPasses, &gHUDFilterSettings[HUDFilter_Default].bloomBlurTaps, SliderX, SliderY, false );
	SliderY += imgui_Static( SliderX, SliderY, "Night Vision Film Grain" ) ;
	SliderY = FillFilmGrainSettings( &gHUDFilterSettings[HUDFilter_NightVision].filmGrainSettings, SliderX, SliderY );

	SliderY += imgui_Static( SliderX, SliderY, "Color Correction" ) ;
	char fullPath[ 512 ];
	GetCCLUT3DTextureFullPath( fullPath, "*.dds" );

	static char textureName[ 256 ] = {0};

	const int CC_FILE_LIST_SIZE = 300;

	static float offset = 0.f ;

	const r3dString& GetCCLUT3DTextureName( HUDFilters filter, r3dAtmosphere::SkyPhase phase );

	char drive[ 16 ], dir[ 512 ], name[ 512 ], ext[ 16 ] ;

	_splitpath( GetCCLUT3DTextureName( HUDFilter_NightVision, r3dAtmosphere::SKY_PHASE_DAY ).c_str(), drive, dir, name, ext );

	strcpy( textureName, name );
	strcat( textureName, ext );

	if ( imgui_FileList(SliderX, SliderY, 360, CC_FILE_LIST_SIZE, fullPath, textureName, &offset ) )
	{
		void ReloadAllCCLUT3DTextures( const char* newName, HUDFilters filter );
		ReloadAllCCLUT3DTextures( textureName, HUDFilter_NightVision );
	}

	SliderY += CC_FILE_LIST_SIZE;

	int overrideHDR = !! gHUDFilterSettings[ HUDFilter_NightVision ].overrideHDRControls ;
	SliderY += imgui_Checkbox( SliderX, SliderY, "Override HDR Settings", &overrideHDR, 1 ) ;
	gHUDFilterSettings[ HUDFilter_NightVision ].overrideHDRControls = !!overrideHDR ;

	if( overrideHDR )
	{
		SliderY += imgui_Value_Slider( SliderX, SliderY, "HDR Exposure Bias", &gHUDFilterSettings[ HUDFilter_NightVision ].hdrExposureBias, -10.0f, 10.f, "%.2f" );
	}

	int overrideAmbientAndIntensity = !!gHUDFilterSettings[ HUDFilter_NightVision ].overrideAmbientAndIntensity;
	SliderY += imgui_Checkbox( SliderX, SliderY, "Override Ambient & Intensity", &overrideAmbientAndIntensity, 1 ) ;
	gHUDFilterSettings[ HUDFilter_NightVision ].overrideAmbientAndIntensity = !!overrideAmbientAndIntensity;

	if( overrideAmbientAndIntensity )
	{
		SliderY += imgui_DrawColorPicker( SliderX, SliderY, "Override Ambient", &gHUDFilterSettings[ HUDFilter_NightVision ].overrideAmbient, 360.0f, false );
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Override intensity", &gHUDFilterSettings[ HUDFilter_NightVision ].overrideIntensity, 0.0f, 10.0f, "%.2f" );
	}
}

void Editor_Level :: ProcessPost_Scopes()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	SliderY += imgui_Static( SliderX, SliderY, "Scope Material Params" ) ;

	int forceScope = r_force_scope->GetInt() ;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Force Scope", &forceScope, 1 ) ;

	extern int g_RenderScopeEffect;
	if( !r_force_scope->GetInt() && forceScope && !g_RenderScopeEffect )
	{
		gPFX_ScopeEffect.SetScopeReticle( ScopeTest_Reticle );
		gPFX_ScopeEffect.SetScopeNormal( ScopeTest_Normal );
		gPFX_ScopeEffect.SetScopeMask( ScopeTest_Mask );
		gPFX_ScopeEffect.SetScopeBlurMask( ScopeTest_BlurMask );
	}

	r_force_scope->SetInt( forceScope ) ;

	static int OverrideScopeMaterial = 0 ;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Override Scope Material", &OverrideScopeMaterial, 1 ) ;

	if( OverrideScopeMaterial )
	{
		static ScopeMaterialParams params ;

		params = gPFX_ScopeEffect.GetMaterialParams() ;

		params.Lighting = !!params.Lighting ;

		SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Enable Lighting", &params.Lighting, 1 ) ;

		params.Lighting = !!params.Lighting ;

		if( params.Lighting )
		{
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Metalness", &params.Metalness, 0.f, 1.f, "%.3f" ) ;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Chromness", &params.Chromeness, 0.f, 1.f, "%.3f" ) ;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Specular Power", &params.SpecularPower, 0.f, 1.f, "%.3f" ) ;
		}

		gPFX_ScopeEffect.SetMaterialParams( params ) ;
	}


}

extern float g_SceneVisualizerDepthMultiplier;
extern D3DXVECTOR4 g_SceneVisualizerColorMask;

void Editor_Level::ProcessPost_Debug()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	int param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_STANDARD ? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Standard rendering", &param, 1 ) ;
	if (param)
		r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_STANDARD);

	if( r_lighting_quality->GetInt() > 1 )
	{
		param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_LIGHTING ? 1 : 0;
		SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Show lighting", &param, 1 ) ;
		if (param)
			r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_LIGHTING);
	}

	param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AMBIENT ? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Show ambient", &param, 1 ) ;
	if (param)
		r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_AMBIENT);

	param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_DIFFUSE? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Show diffuse buffer", &param, 1 ) ;
	if (param)
		r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_DIFFUSE);

	param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_NORMALS ? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Show normal buffer", &param, 1 ) ;
	if (param)
		r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_NORMALS);

	param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_REFLECTIONS ? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Show reflections buffer", &param, 1 ) ;
	if (param)
		r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_REFLECTIONS);

	param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_DEPTH ? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Show depth buffer", &param, 1 ) ;
	if (param)
	{
		r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_DEPTH);
		//	Dynamic range slider for depth renderer
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Depth multiplier", &g_SceneVisualizerDepthMultiplier, 0.0001f, 0.01f, "%-1.03f");
	}

	param = r_scene_visualize_mode->GetInt() == SCENE_VISUALIZE_AUX? 1 : 0;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Show aux buffer", &param, 1 ) ;
	if (param)
		r_scene_visualize_mode->SetInt(SCENE_VISUALIZE_AUX);

	//	Color channels mask

	static int r = 1, g = 1, b = 1, a = 1;
	SliderY += 20.0f;

	SliderY += imgui_Static(SliderX, SliderY, "Color mask (uncheck rgb channels to see alpha in rgb)");
	imgui_Checkbox(SliderX, SliderY, 90, 22, "R", &r, 1);
	imgui_Checkbox(SliderX + 90, SliderY, 90, 22, "G", &g, 1);
	imgui_Checkbox(SliderX + 180, SliderY, 90, 22, "B", &b, 1);
	SliderY += imgui_Checkbox(SliderX + 270, SliderY, 90, 22, "A", &a, 1);

	g_SceneVisualizerColorMask = D3DXVECTOR4(1.0f * r, 1.0f * g, 1.0f * b, 1.0f * a);
}

void Editor_Level :: ProcessPostFX()
{

	const static char* listpost[] = { "CAMERA",			"ATMOSPHERE",			"LIGHTING",			"NIGHT VISION",			"SCOPES",			"DEBUG"			};
	enum							{ POSTMODE_CAMERA,	POSTMODE_ATMOSPHERIC,	POSTMODE_LIGHTING,	POSTMODE_NIGHTVISION,	POSTMODE_SCOPES,	POSTMODE_DEBUG	};

	static int PostprocessEditorMode = POSTMODE_ATMOSPHERIC;

	imgui_Toolbar(5, 45, 650, 35, &PostprocessEditorMode, 0, listpost, R3D_ARRAYSIZE(listpost), false );

	g_pDesktopManager->Begin( "ed_postfx" );

	switch (PostprocessEditorMode)
	{
		case	POSTMODE_CAMERA:
		{
			ProcessPost_Camera();
		}
		break;


		case POSTMODE_ATMOSPHERIC:
		{
			ProcessPost_Glow();
		}
		break;
		
		case POSTMODE_LIGHTING:
		{
			ProcessPost_Lighting();
		}
		break;

		case POSTMODE_NIGHTVISION:
		{
			ProcessPost_NightVision();
		}
		break;

		case POSTMODE_SCOPES:
		{
			ProcessPost_Scopes();
		}
		break;

		case POSTMODE_DEBUG:
		{
			ProcessPost_Debug();
		}
		break;
	}

	g_pDesktopManager->End();
}

int _ColorGrading_Levels = 0;

IDirect3DVolumeTexture9* Generate3DLut( const char* Source, const int SOURCE_DIM, const int TARGET_DIM )
{
	struct LocalObjs
	{
		explicit LocalObjs( const char* Source )
		{
			SourceTex = r3dRenderer->LoadTexture( Source, D3DFMT_A8R8G8B8, false, 1, 0, D3DPOOL_SYSTEMMEM ) ;
		}

		~LocalObjs()
		{
			r3dRenderer->DeleteTexture( SourceTex );
		}

		r3dTexture* SourceTex ;

	} lobj( Source ) ;

	if( lobj.SourceTex->IsMissing() )
	{
		MessageBox( NULL, "Couldn't find source image! Please, select it!", "Error", MB_OK );
		return NULL;
	}

	IDirect3DTexture9* sourceTex = lobj.SourceTex->AsTex2D();

	int numCellsPerLine ;
	int extraHeight ;

	int iwidth = lobj.SourceTex->GetWidth() ;

	GetCCRefTexParams( SOURCE_DIM, iwidth, numCellsPerLine, extraHeight );

	if( lobj.SourceTex->GetHeight() < extraHeight )
	{
		MessageBox( NULL, "Source Image Size too Small!", "Error", MB_ICONEXCLAMATION );
		return NULL ;
	}

	r3dTL::TArray< DWORD > ColorCorrectionData ;

	D3DLOCKED_RECT lrect ;

	D3D_V( sourceTex->LockRect( 0, &lrect, NULL, 0 ) );

	if( lrect.Pitch != (int)lobj.SourceTex->GetWidth() * 4 )
	{
		MessageBox( NULL, "Unsupported texture format or size!", "Error", MB_ICONEXCLAMATION );
		D3D_V( sourceTex->UnlockRect( 0 ) );

		return NULL;
	}

	DWORD* source = (DWORD*)( ( char* )lrect.pBits + lrect.Pitch * ( lobj.SourceTex->GetHeight() - extraHeight ) ) ;

	ColorCorrectionData.Resize( SOURCE_DIM * SOURCE_DIM * SOURCE_DIM );

	int cellLinePixelWidth = numCellsPerLine * SOURCE_DIM ;

	for( int i = 0, k = 0; i < extraHeight; i ++ )
	{
		int left = R3D_MAX( R3D_MIN( cellLinePixelWidth, (int)ColorCorrectionData.Count() - k ), 0 );

		memcpy( &ColorCorrectionData[ k ], source, 4 * left );

		k += cellLinePixelWidth;

		if( k >= (int)ColorCorrectionData.Count() )
			break ;

		source += iwidth ;
	}

	D3D_V( sourceTex->UnlockRect( 0 ) );

	const int MAX_DIM = 256 ;

	const int CNT = SOURCE_DIM / TARGET_DIM ;

	r3d_assert( TARGET_DIM <= SOURCE_DIM  );

	if( TARGET_DIM >= 64 )
	{
		MessageBox(0, "You are generating LUT texture bigger than 64 pixels.\nTHAT IS VERY EXPENSIVE!!!\nPlease regenerate it as 32x32x32 or even better as 16x16x16", "Warning!", MB_OK);
	}

	IDirect3DVolumeTexture9* pVol = NULL;
	D3D_V ( r3dRenderer->pd3ddev->CreateVolumeTexture( TARGET_DIM, TARGET_DIM, TARGET_DIM, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pVol, NULL ) );

	r3d_assert( pVol );

	D3DLOCKED_BOX lockedVol;
	pVol->LockBox ( 0, &lockedVol, NULL, 0 );
	byte * pData = (byte *) lockedVol.pBits;

	for ( int i = 0; i < TARGET_DIM; i++ )
	{
		byte * pDataCpy = pData;
		for ( int j = 0; j < TARGET_DIM; j++ )
		{
			byte * pDataCpy2 = pDataCpy;
			for ( int k = 0; k < TARGET_DIM; k++ )
			{
				unsigned R_Avg = 0;
				unsigned G_Avg = 0;
				unsigned B_Avg = 0;

				for ( int l = i * CNT; l < i * CNT + CNT; l++ )
					for ( int m = j * CNT; m < j * CNT + CNT; m++ )
						for ( int n = k * CNT; n < k * CNT + CNT; n++ )
						{
							byte iR = l;
							byte iG = m;
							byte iB = n;

							DWORD val = ColorCorrectionData[ l + m * SOURCE_DIM + n * SOURCE_DIM * SOURCE_DIM ];

							R_Avg += val & 0xff ;
							G_Avg += val >> 8 & 0xff ;
							B_Avg += val >> 16 & 0xff ;
						}

						R_Avg /= ( CNT * CNT * CNT );
						G_Avg /= ( CNT * CNT * CNT );
						B_Avg /= ( CNT * CNT * CNT );

						r3d_assert( R_Avg <= 255 );
						r3d_assert( G_Avg <= 255 );
						r3d_assert( B_Avg <= 255 );

						pDataCpy2[0] = R_Avg;
						pDataCpy2[1] = G_Avg;
						pDataCpy2[2] = B_Avg;
						pDataCpy2[3] = 0;
						pDataCpy2 += 4;
			}
			pDataCpy += lockedVol.RowPitch;
		}
		pData += lockedVol.SlicePitch;
	}

	pVol->UnlockBox ( 0 );

	return pVol;
}


IDirect3DVolumeTexture9* Generate3DLut( int uiTexDim )
{
	const int MAX_DIM = 256;
	const int DIM = 256 >> uiTexDim;
	const int CNT = MAX_DIM / DIM;

	if(DIM >= 64)
	{
		MessageBox(0, "You are generating LUT texture bigger than 64 pixels.\nTHAT IS VERY EXPENSIVE!!!\nPlease regenerate it as 32x32x32 or even better as 16x16x16", "Warning!", MB_OK);
	}

	IDirect3DVolumeTexture9* pVol = NULL;
	D3D_V ( r3dRenderer->pd3ddev->CreateVolumeTexture( DIM, DIM, DIM, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pVol, NULL ) );

	r3d_assert( pVol );

	D3DLOCKED_BOX lockedVol;
	pVol->LockBox ( 0, &lockedVol, NULL, 0 );
	byte * pData = (byte *) lockedVol.pBits;

	bool doRGB = false;
	bool doHSV = false;

	switch( g_ColorCorrectionSettings.scheme )
	{
	case ColorCorrectionSettings::CCS_USE_HSV_CURVES:
		doHSV = true;
		break;

	case ColorCorrectionSettings::CCS_USE_RGB_CURVES:
		doRGB = true;
		break;

	case ColorCorrectionSettings::CCS_USE_RGB_HSV_CURVES:
		doRGB = true;
		doHSV = true;
		break;
	}

	r3d_assert( doRGB || doHSV );

	for ( int i = 0; i < DIM; i++ )
	{
		byte * pDataCpy = pData;
		for ( int j = 0; j < DIM; j++ )
		{
			byte * pDataCpy2 = pDataCpy;
			for ( int k = 0; k < DIM; k++ )
			{
				unsigned R_Avg = 0;
				unsigned G_Avg = 0;
				unsigned B_Avg = 0;

				for ( int l = i * CNT; l < i * CNT + CNT; l++ )
					for ( int m = j * CNT; m < j * CNT + CNT; m++ )
						for ( int n = k * CNT; n < k * CNT + CNT; n++ )
						{
							byte iR = l;
							byte iG = m;
							byte iB = n;

							if( doRGB )
							{
								iR = (byte)R3D_CLAMP( int(255.0f * g_ColorCorrectionSettings.RGBCurves[2].GetFloatValue( (float)iR / 255.0f ) ), 0, 255 );
								iG = (byte)R3D_CLAMP( int(255.0f * g_ColorCorrectionSettings.RGBCurves[1].GetFloatValue( (float)iG / 255.0f ) ), 0, 255 );
								iB = (byte)R3D_CLAMP( int(255.0f * g_ColorCorrectionSettings.RGBCurves[0].GetFloatValue( (float)iB / 255.0f ) ), 0, 255 );
							}

							if( doHSV )
							{
								r3dVector HSV = r3dRGBtoHSV( r3dColor ( iB, iG, iR ) );

								float fH = HSV.x + g_ColorCorrectionSettings.HSVCurves[0].GetFloatValue( HSV.x );

								fH = fH - floorf( fH );

								float fS = g_ColorCorrectionSettings.HSVCurves[1].GetFloatValue( HSV.y );
								float fV = g_ColorCorrectionSettings.HSVCurves[2].GetFloatValue( HSV.z );

								r3dColor clr = r3dHSVtoRGB ( r3dVector ( fH, fS, fV ) );

								iR = clr.B;
								iG = clr.G;
								iB = clr.R;
							}

							R_Avg += iR;
							G_Avg += iG;
							B_Avg += iB;
							
						}

						R_Avg /= ( CNT * CNT * CNT );
						G_Avg /= ( CNT * CNT * CNT );
						B_Avg /= ( CNT * CNT * CNT );

						r3d_assert( R_Avg <= 255 );
						r3d_assert( G_Avg <= 255 );
						r3d_assert( B_Avg <= 255 );

						pDataCpy2[0] = R_Avg;
						pDataCpy2[1] = G_Avg;
						pDataCpy2[2] = B_Avg;
						pDataCpy2[3] = 0;
						pDataCpy2 += 4;
			}
			pDataCpy += lockedVol.RowPitch;
		}
		pData += lockedVol.SlicePitch;
	}

	pVol->UnlockBox ( 0 );

	return pVol;
}


//--------------------------------------------------------------------------------------------------------
void SaveCCCurves(	const char * szLevelName, r3dBezierGradient * pRGBCurves, r3dBezierGradient * pHSVCurves )
{
	FILE * hFile = fopen( Va( FNAME_LEVEL_CCCURVES, szLevelName ), "wt" );
	if ( ! hFile )
		return;

	for ( int i = 0; i < 3; i++ )
	{
		WriteTimeCurve( hFile, pRGBCurves[ i ], "rgb" );
	}

	for ( int i = 0; i < 3; i++ )
	{
		WriteTimeCurve( hFile, pHSVCurves[ i ], "hsv" );
	}	

	fclose( hFile );
}

//--------------------------------------------------------------------------------------------------------
void LoadCCCurves( const char * szLevelName, r3dBezierGradient * pRGBCurves, r3dBezierGradient * pHSVCurves )
{
	Script_c script;

	if ( ! script.OpenFile( Va( FNAME_LEVEL_CCCURVES, szLevelName ) ) )
		return;

	char buffer[MAX_PATH];

	int nRGBCount = 0;
	int nHSVCount = 0;

	for ( int i = 0; i < 3; i++ )
	{
		pRGBCurves[ i ].Reset();
		pHSVCurves[ i ].Reset();
	}

	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( ! *buffer )
			break;

		if ( ! strcmp( buffer, "rgb" ) )
		{
			r3d_assert( nRGBCount < 3 );
			ReadTimeCurveNew( script, pRGBCurves[ nRGBCount ] );
			nRGBCount++;
		}
		else if ( ! strcmp( buffer, "hsv" ) )
		{
			r3d_assert( nHSVCount < 3 );
			ReadTimeCurveNew( script, pHSVCurves[ nHSVCount ] );
			nHSVCount++;
		}
	}
	script.CloseFile();
}

//--------------------------------------------------------------------------------------------------------
void Editor_Level :: ProcessColorGrading()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 50;

	int SliderXLeft = 20;
	int SliderYLeft = 90;

	ColorCorrectionTicked = true;
	InColorCorrection = true;

	const char* list[] = { "Global", "Local", "Curve LUT Generation", "External LUT Generation", "HDR Tone Mapping" };
	static int EditMode = 0;
	static int lastEditMode = 0;

	enum
	{
		CCEM_GLOBAL,
		CCEM_LOCAL,
		CCEM_CURVELUTGEN,
		CCEM_EXTERNALLUTGEN,
		CCEM_FILM_TONE
	};

	imgui_Toolbar(5, 45, 800, 35, &EditMode, 0, list, sizeof list / sizeof list[0]);

	g_pDesktopManager->Begin( "ed_colorcorr" );

	if ( lastEditMode != EditMode && lastEditMode == CCEM_CURVELUTGEN )
		RestoreCCLUT3DTexture();

	lastEditMode = EditMode;

	static char textureName[ 256 ] = {0};
	static char textureNameLocal[ 256 ] = {0};
	const int FILE_LIST_SIZE = 200;

	if( EditMode != CCEM_FILM_TONE &&
		EditMode != CCEM_EXTERNALLUTGEN )
	{
		SliderY += imgui_Checkbox( SliderX, SliderY, "Color Correction" , &gHUDFilterSettings[r_hud_filter_mode->GetInt()].enableColorCorrection, 1 );
	}

	// reset anything

	if ( g_iLocalCCSelected < 0 || g_iLocalCCSelected >= (int)obj_LocalColorCorrection::LocalColorCorrectionCount() )
		g_iLocalCCSelected = -1;

	const int CC_FILE_LIST_SIZE = 300;

	switch ( EditMode )
	{
	case CCEM_GLOBAL:
		{
			g_ColorCorrectionSettings.scheme = ColorCorrectionSettings::CCS_CUSTOM_3DLUT;
			HUDFilterSettings &hfs = gHUDFilterSettings[HUDFilter_Default];
			if ( hfs.enableColorCorrection )
			{
				SliderY = ProcessColorCorrectionMenu( hfs, SliderX, SliderY );
			}

			break;
		}

	case CCEM_LOCAL:
		{
			g_Manipulator3d.TypeFilterSet("obj_LocalColorCorrection");

			bool bNeedPickerEnable = g_iPickerEnableTicked == 0;
			if ( bNeedPickerEnable )
				g_Manipulator3d.Enable ();

			if ( g_Manipulator3d.IsLocked() )
				g_Manipulator3d.Unlock();

			g_iPickerEnableTicked = 2;

			g_ColorCorrectionSettings.scheme = ColorCorrectionSettings::CCS_CUSTOM_3DLUT;
			if ( imgui_Button ( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 2, DEFAULT_CONTROLS_HEIGHT, "Add Local Color Correction" ) )
			{
				float alt_len = ( UI_TargetPos - gCam ).Length();

				CreateLocalColorCorrection ( gCam + gCam.vPointTo * R3D_MIN( alt_len, ADD_LOCAL_CC_DIST_TO_CAM ) );
				g_iLocalCCSelected = obj_LocalColorCorrection::LocalColorCorrectionCount() - 1;
				g_Manipulator3d.PickedObjectSet( obj_LocalColorCorrection::LocalColorCorrectionGet ( g_iLocalCCSelected ) );
			}

			if ( imgui_Button ( SliderX + DEFAULT_CONTROLS_WIDTH * 0.5f + 2, SliderY, DEFAULT_CONTROLS_WIDTH * 0.5f - 2, DEFAULT_CONTROLS_HEIGHT, "Delete Local Color Correction" ) )
			{
				DeleteLocalColorCorrection ( obj_LocalColorCorrection::LocalColorCorrectionGet ( g_iLocalCCSelected ) );
				g_iLocalCCSelected = obj_LocalColorCorrection::LocalColorCorrectionCount() > 0 ? r3dTL::Max ( 0, g_iLocalCCSelected - 1 ) : -1;
				if ( g_iLocalCCSelected >= 0 )
					g_Manipulator3d.PickedObjectSet( obj_LocalColorCorrection::LocalColorCorrectionGet ( g_iLocalCCSelected ) );
			}

			SliderY += DEFAULT_CONTROLS_HEIGHT;
			SliderY += 10.0f;
				
			stringlist_t localCCList = FillLocalColorCorrectionList();
			
			if ( imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, LOCAL_CC_SELECT_LIST_HEIGHT, localCCList, &g_iLocalCCListOffset, &g_iLocalCCSelected ) )
			{
				if ( g_iLocalCCSelected >= 0 )
					g_Manipulator3d.PickedObjectSet( obj_LocalColorCorrection::LocalColorCorrectionGet ( g_iLocalCCSelected ) );
			}

			SliderY += LOCAL_CC_SELECT_LIST_HEIGHT;

			obj_LocalColorCorrection * pEditCC = NULL;
			{
				GameObject * pObj = g_Manipulator3d.PickedObject ();
				if ( pObj && pObj->Class->Name == "obj_LocalColorCorrection" )
					pEditCC = static_cast < obj_LocalColorCorrection* > ( pObj );
			}

			// update selection in list if picker changed picked obj
			if ( !pEditCC )
				g_iLocalCCSelected = -1;
			else
			{
				if ( g_iLocalCCSelected < 0 || pEditCC != obj_LocalColorCorrection::LocalColorCorrectionGet (g_iLocalCCSelected) )
				{
					g_iLocalCCSelected = -1;
					for ( unsigned int i = 0; i < obj_LocalColorCorrection::LocalColorCorrectionCount(); i++ )
					{
						if ( obj_LocalColorCorrection::LocalColorCorrectionGet(i) == pEditCC )
						{
							g_iLocalCCSelected = i;
							break;
						}
					}
				}	
			}

			obj_LocalColorCorrection::DeselectAll();

			if ( pEditCC )
			{
				// render selected sphere
				pEditCC->SetSelected( true );

				GameObjects selected ;
				g_Manipulator3d.GetPickedObjects( &selected ) ;
				SliderY += pEditCC->DrawPropertyEditor (SliderX, SliderY, 300, 200, &obj_LocalColorCorrection::ClassData, selected );
			}

			break;
		}

	case CCEM_CURVELUTGEN:
		{
			const HUDFilterSettings &hfs = gHUDFilterSettings[HUDFilter_Default];
			if( hfs.enableColorCorrection )
			{
				stringlist_t texDimensions;

				int iDim = 256;
				for ( int i = 0; i < 5; i++ )
				{
					char str[256];
					sprintf ( str, "(%ix%ix%i)", iDim, iDim, iDim );
					texDimensions.push_back ( str );
					iDim /= 2;
				}

				const int CC_LIST_HEIGHT = 60;
				const int CC_DIM_LIST_HEIGHT = 120;

				float fDummy;

				enum
				{
					CURVES_RGB,
					CURVES_HSV
				};

				int& uiTexDim = g_ColorCorrectionSettings.uiTexDim;

				static int RGBOn = 1;
				static int HSVOn = 1;

				switch( g_ColorCorrectionSettings.scheme )
				{
				case ColorCorrectionSettings::CCS_USE_RGB_CURVES:
					RGBOn = 1;
					break;

				case ColorCorrectionSettings::CCS_USE_HSV_CURVES:
					HSVOn = 1;
					break;

				case ColorCorrectionSettings::CCS_USE_RGB_HSV_CURVES:
					RGBOn = 1;
					HSVOn = 1;
					break;
				};

				if ( imgui_Button( SliderX + 5 , SliderY, 170, 22, "RGB", RGBOn, false ) )
				{
					RGBOn = !RGBOn;
				}

				if ( imgui_Button( SliderX + 185, SliderY, 170, 22, "HSV", HSVOn, false ) )
				{
					HSVOn = !HSVOn;
				}

				if( !RGBOn && !HSVOn )
				{
					RGBOn = 1 ;
				}

				if( RGBOn && HSVOn )
				{
					g_ColorCorrectionSettings.scheme = ColorCorrectionSettings::CCS_USE_RGB_HSV_CURVES ;
				}
				else
				{
					if( RGBOn )
					{
						g_ColorCorrectionSettings.scheme = ColorCorrectionSettings::CCS_USE_RGB_CURVES ;
					}
					else
					{
						g_ColorCorrectionSettings.scheme = ColorCorrectionSettings::CCS_USE_HSV_CURVES ;
					}
				}

				SliderY += 24 ;

				static char sTexName [256] = {0};

				stringlist_t curveNames;

				if( RGBOn )
				{
					curveNames.push_back( "R curve" );
					curveNames.push_back( "G curve" );
					curveNames.push_back( "B curve" );
				}

				if( HSVOn )
				{
					curveNames.push_back( "H curve" );
					curveNames.push_back( "S curve" );
					curveNames.push_back( "V curve" );
				}

				const int CURVE_LIST_HEIGHT = RGBOn && HSVOn ? 120 : 70 ;

				static int sel_curve = 0;

				sel_curve = R3D_MIN( sel_curve, (int)curveNames.size() - 1 );

				imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, CURVE_LIST_HEIGHT, curveNames, &fDummy, &sel_curve );
				SliderY += CURVE_LIST_HEIGHT;

				int curve = sel_curve;

				if( !RGBOn )
				{
					curve += 3 ;
				}

				const int CURVE_COUNT = 6;

				const char* RGBNames[ CURVE_COUNT ] = { "Red Curve", "Green Curve", "Blue Curve", "Hue Curve", "Saturation Curve", "Value Curve" };

				float CurveRanges[ CURVE_COUNT ][2] = {	0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
														-0.5f, 0.5f, 0.f, 1.f, 0.f, 1.f };

				r3dBezierGradient *targetCurves[ CURVE_COUNT ] = {
						g_ColorCorrectionSettings.RGBCurves + 0,
						g_ColorCorrectionSettings.RGBCurves + 1,
						g_ColorCorrectionSettings.RGBCurves + 2,
						g_ColorCorrectionSettings.HSVCurves + 0,
						g_ColorCorrectionSettings.HSVCurves + 1,
						g_ColorCorrectionSettings.HSVCurves + 2,
				};
				

				r3d_assert( targetCurves );
				r3d_assert( CURVE_COUNT >= curveNames.size() );

				SliderY += imgui_DrawBezierGradient( SliderX, SliderY, curveNames[ sel_curve ].c_str(), targetCurves[ curve ], DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_WIDTH, 10, 5, 1, 1 );

				UpdateColorCorrectionTextures();

				if( imgui_Button( SliderX, SliderY, 120.f, 24.f, "Reset curves" ) )
				{
					g_ColorCorrectionSettings.ResetCurvesRGB();
					g_ColorCorrectionSettings.ResetCurvesHSV();
				}

				if( imgui_Button( SliderX + 120, SliderY, 120.f, 24.f, "Save curves" ) )
				{	
					SaveCCCurves( r3dGameLevel::GetHomeDir(), g_ColorCorrectionSettings.RGBCurves, g_ColorCorrectionSettings.HSVCurves);
				}

				if( imgui_Button( SliderX + 240, SliderY, 120.f, 24.f, "Load curves" ) )
				{		
					LoadCCCurves( r3dGameLevel::GetHomeDir(), g_ColorCorrectionSettings.RGBCurves, g_ColorCorrectionSettings.HSVCurves );
				}
				
				SliderY += 27;

				SliderY += imgui_Static(SliderX, SliderY, "Curve LUT Generation");
				SliderY += 2;

#if 0
				imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, CC_DIM_LIST_HEIGHT, texDimensions, uiTexDim, &fDummy, &uiTexDim );
				SliderY += CC_DIM_LIST_HEIGHT;
#else
				uiTexDim = 3;
#endif

				if( imgui_Button( SliderX, SliderY, 180.f, 24.f, "Generate 3D LUT" ) )
				{		
					IDirect3DVolumeTexture9* pVol = Generate3DLut( uiTexDim );

					char sFile[ 512 ];
					GetCCLUT3DTextureFullPath( sFile, "out.dds" );

					if ( sTexName[0] )
					{
						char nameAndExt[ 128 ];
						sprintf( nameAndExt, "%s.dds", sTexName );
						GetCCLUT3DTextureFullPath( sFile, nameAndExt );
					}

					D3DXSaveTextureToFile( sFile, D3DXIFF_DDS, pVol, NULL );

					pVol->Release ();
					pVol = NULL;

					void imgui_FileListReset (); 
					imgui_FileListReset ();
				}

				imgui2_StartArea ( "area1", SliderX + 180.f + 1, SliderY, 180.f, 24.f );
				imgui2_StringValue ( "tex name", sTexName );
				imgui2_EndArea ();

				SliderY += 20;

			}
			break;
		}

	case CCEM_EXTERNALLUTGEN:
		{
			char fullPath[ 512 ], wildCards[ 512 ];
			GetCCLUT3DTextureFullPath( fullPath, "" );

			static bool FirstEntry = true;

			const int THUMB_DOWN_SCALE = 1;

#define LUT3D_EXTERNAL_SUPBATH "External\\"

			strcat( fullPath, LUT3D_EXTERNAL_SUPBATH );
			r3dscpy( wildCards, fullPath );
			strcat( wildCards, "*.bmp" );

			if( FirstEntry )
			{
				if( r3d_access( fullPath, 0 ) )
				{
					mkdir( fullPath );
				}

				FirstEntry = false;
			}

			static float lastReset = r3dGetTime();

			if( r3dGetTime() - lastReset > 1.f )
			{
				extern void imgui_FileListReset();
				imgui_FileListReset();

				lastReset = r3dGetTime();
			}

			static r3dTexture* ThumbNailTex = r3dRenderer->AllocateTexture() ;

			static float offset = 0.f ;

			if ( imgui_FileList( SliderX, SliderY, 360, CC_FILE_LIST_SIZE, wildCards, textureName, &offset ) )
			{
				char fullName[ 512 ];
				r3dscpy( fullName, fullPath );
				strcat( fullName, textureName );

				if( ThumbNailTex->IsDrawable() )
				{
					ThumbNailTex->Unload();
				}
				ThumbNailTex->Load( fullName, D3DFMT_FROM_FILE, THUMB_DOWN_SCALE );
			}

			SliderY += CC_FILE_LIST_SIZE + 2 ;

			static r3dScreenBuffer* GrabbedPreCCScreen = NULL ;

			const int CC_TEX_SOURCE_DIM = 64 ;

			if( GrabbedPreCCScreen )
			{
				r3dTexture* tempTex = r3dRenderer->AllocateTexture();
				r3dTexture* targetTex = r3dRenderer->AllocateTexture();

				int numCellsPerLine ;
				int extraHeight ; 
				int iwidth = (int)GrabbedPreCCScreen->Width ;

				GetCCRefTexParams( CC_TEX_SOURCE_DIM, iwidth, numCellsPerLine, extraHeight );

				tempTex->Create( (int)GrabbedPreCCScreen->Width, (int)GrabbedPreCCScreen->Height + extraHeight, D3DFMT_A8R8G8B8, 1, D3DPOOL_SYSTEMMEM );
				targetTex->Create( (int)GrabbedPreCCScreen->Width, (int)GrabbedPreCCScreen->Height, D3DFMT_A8R8G8B8, 1, D3DPOOL_SYSTEMMEM );

				IDirect3DSurface9 *targetSurf( NULL ), *tempSurf ( NULL ), *GrabbedPreCCScreenSurf(NULL) ;

				D3D_V( targetTex->AsTex2D()->GetSurfaceLevel( 0, &targetSurf ) ) ;
				D3D_V( r3dRenderer->pd3ddev->GetRenderTargetData( GrabbedPreCCScreen->GetTex2DSurface(), targetSurf ) );

				D3D_V( tempTex->AsTex2D()->GetSurfaceLevel( 0, &tempSurf ) );

				D3DLOCKED_RECT destRect ;
				D3DLOCKED_RECT sourceRect ;

				D3D_V( tempSurf->LockRect ( &destRect, NULL, 0 ) );
				D3D_V( targetSurf->LockRect ( &sourceRect, NULL, 0 ) );

				r3d_assert( destRect.Pitch == GrabbedPreCCScreen->Width * 4 );
				r3d_assert( destRect.Pitch == sourceRect.Pitch );

				int sourceSize = (int)GrabbedPreCCScreen->Width * (int)GrabbedPreCCScreen->Height * 4 ;
				memcpy( destRect.pBits, sourceRect.pBits, sourceSize );

				DWORD* refStart = (DWORD*)( (char*)destRect.pBits + sourceSize );

				r3d_assert( !( 256 % CC_TEX_SOURCE_DIM ) );

				for( int i = 0, c = 0 ; i < extraHeight; i ++ )
				{
					int cellPixelWidth = numCellsPerLine * CC_TEX_SOURCE_DIM ;

					for( int i = 0, e = cellPixelWidth ; i < e; i ++ )
					{
						*refStart ++ = c ;

						 c += 256 / CC_TEX_SOURCE_DIM;

						 if( ! ( c % 256 ) )
						 {
							 c += 256 * ( 256 / CC_TEX_SOURCE_DIM - 1 );
						 }

						 if( ! ( c % ( 256 * 256 ) ) )
						 {
							 c += 256 * 256 * ( 256 / CC_TEX_SOURCE_DIM - 1 );
						 }
					}

					refStart += iwidth - cellPixelWidth ;
				}

				D3D_V( tempSurf->UnlockRect() );
				D3D_V( targetSurf->UnlockRect() );

				SAFE_RELEASE( targetSurf );
				SAFE_RELEASE( tempSurf );

				char FullName[ 512 ];

				r3dscpy( FullName, fullPath );
				strcat( FullName, "\\CC_Reference.bmp" );

				tempTex->Save( FullName );

				r3dRenderer->DeleteTexture( targetTex );
				r3dRenderer->DeleteTexture( tempTex );

				SAFE_DELETE( GrabbedPreCCScreen );

				void imgui_FileListReset (); 
				imgui_FileListReset ();

				if( ThumbNailTex->IsValid() )
				{
					ThumbNailTex->Unload();
					ThumbNailTex->Load( ThumbNailTex->getFileLoc().FileName, D3DFMT_FROM_FILE, THUMB_DOWN_SCALE );
				}
			}

			if( imgui_Button( SliderX + 2.f, SliderY, 176.f, 22.f, "Generate Source" ) )
			{
				float rtWidth, rtHeight ;
				GetDesiredRTDimmensions( &rtWidth, &rtHeight ) ;

				GrabbedPreCCScreen = r3dScreenBuffer::CreateClass( "ColorCorrection screen capture texture", rtWidth, rtHeight, D3DFMT_A8R8G8B8 );
				CurRenderPipeline->SetPreColorCorrectionScreenshotTarget( GrabbedPreCCScreen );
			}

			if( imgui_Button( SliderX + 182.f, SliderY, 176.f, 22.f, "Edit" ) )
			{
				if ( textureName[0] )
				{
					char FileName[ 512 ];
					char fullPath[ 1024 ];

					const char* execStr = g_texture_edit_cmd->GetString();

					char nameWithExternal[ 256 ];
					strcpy( nameWithExternal, LUT3D_EXTERNAL_SUPBATH );
					strcat( nameWithExternal, textureName );

					GetCCLUT3DTextureFullPath( FileName, nameWithExternal );

					GetCurrentDirectory( sizeof fullPath - 2, fullPath ) ;

					strcat( fullPath, "\\" );
					strcat( fullPath, FileName );

					ShellExecute( NULL, "open", execStr, fullPath, NULL, SW_SHOWNORMAL ) ;
				}
			}

			SliderY += 26.f ;

			if( imgui_Button( SliderX + 2.f, SliderY, 176.f, 22.f, "Generate LUT" ) )
			{
				if ( textureName[0] )
				{

					char fullName[ 512 ];
					r3dscpy( fullName, fullPath );
					strcat( fullName, textureName );

					if( IDirect3DVolumeTexture9* pVol = Generate3DLut( fullName, CC_TEX_SOURCE_DIM, 32 ) )
					{
						char FileName[ 512 ];

						char nameAndExt[ 128 ];
						sprintf( nameAndExt, "%s.dds", textureName );
						GetCCLUT3DTextureFullPath( FileName, nameAndExt );

						D3DXSaveTextureToFile( FileName, D3DXIFF_DDS, pVol, NULL );

						char msg[ 512 ];
						sprintf( msg, "Generated %s", FileName );

						MessageBoxA( r3dRenderer->HLibWin, msg, "Success", MB_OK );

						pVol->Release ();
						pVol = NULL;

						void imgui_FileListReset (); 
						imgui_FileListReset ();
					}
				}
				else
				{
					MessageBoxA( r3dRenderer->HLibWin, "Enter texture name in 'tex name' first!", "Error", MB_OK );
				}
			}

			SliderY -= 4.0f;

			imgui2_StartArea ( "area1", SliderX + 180.f + 1, SliderY, 180.f, 24.f );
			imgui2_StringValue ( "tex name", textureName );
			imgui2_EndArea ();

			SliderY += 77.f ;

			if( ThumbNailTex->IsValid() )
			{
				int deskX = g_pDesktopManager->GetActiveDesctop().GetX();
				int deskY = g_pDesktopManager->GetActiveDesctop().GetY();

				float aspect = (float)ThumbNailTex->GetHeight() / ThumbNailTex->GetWidth() ;
			
				SliderY += imgui_Static( SliderX - deskX , SliderY - deskY, textureName, 360, false, 22.f );

				r3dDrawBox2D( SliderX - deskX + 1 , SliderY - deskY, 360, 360 * aspect , imgui_bkgDlg,  NULL );

				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

				r3dDrawBox2D( SliderX + 5 - deskX + 1, SliderY - deskY + 5 * aspect, 350, 350 * aspect, r3dColor(255,255,255),  ThumbNailTex );

				r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
			}
		}
		break;

	case CCEM_FILM_TONE:
		{
			static float a, b, c, d, e, f, eb, wl ;

			a = r_film_tone_a->GetFloat();
			b = r_film_tone_b->GetFloat();
			c = r_film_tone_c->GetFloat();
			d = r_film_tone_d->GetFloat();
			e = r_film_tone_e->GetFloat();
			f = r_film_tone_f->GetFloat();
			eb = r_exposure_bias->GetFloat();
			wl = r_white_level->GetFloat();

			int showLuma = r_show_luma->GetInt();

			SliderY += imgui_Checkbox( SliderX, SliderY, "Show Exposure&Luma", &showLuma, 1 );

			r_show_luma->SetInt( !!showLuma );

			SliderY += imgui_Value_Slider( SliderX, SliderY, "Shoulder Strength", &a, 0.0f, 1.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Linear Strength", &b, 0.0f, 1.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Linear Angle", &c, 0.0f, 1.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Toe Strength", &d, 0.0f, 1.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Toe Numerator", &e, 0.0f, 1.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Toe Denumerator", &f, 0.0f, 1.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "White Level:", &wl, 0.0f, 20.f, "%.2f", 1 );


			if( imgui_Button( SliderX, SliderY, 120.f, 24.f, "Preset Bright" ) )
			{
				a = 0.32f;//Shoulder Strength
				b = 0.3f; //Linear Strength
				c = 0.1f; //Linear Angle
				d = 0.2f;//Toe Strength
				e = 0.01f; //Toe Numerator
				f = 0.3f; //Toe Denominator
				eb = 0.0f;
				wl = 11.2f;
			}

			SliderY += 27;
			if( imgui_Button( SliderX, SliderY, 120.f, 24.f, "Preset Real Film" ) )
			{
				a = 0.22f;
				b = 0.30f;
				c = 0.10f;
				d = 0.20f;
				e = 0.01f;
				f = 0.30f;
				eb = 0.0f;
				wl = 11.2f;
			}

			static float	MinLuma ;
			static float	MaxLuma ;
			static float	MedLuma ;
			static float	ExpRange1 ;
			static float	ExpRange2 ;
			static float	AdaptSpeedPos ;
			static float	AdaptSpeedNeg ;

			MinLuma = r_exposure_minl->GetFloat();
			MaxLuma = r_exposure_maxl->GetFloat();
			MedLuma = r_exposure_targetl->GetFloat();
			ExpRange1 = r_exposure_rangeMax->GetFloat();
			ExpRange2 = r_exposure_rangeMin->GetFloat();
			AdaptSpeedPos = r_light_adapt_speed_pos->GetFloat() ;
			AdaptSpeedNeg = r_light_adapt_speed_neg->GetFloat() ;
	
			SliderY += 30;
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Exposure Bias:", &eb, -10.0f, 10.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Min Scene Lum:", &MinLuma, 0.0f, 2.f, "%.4f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Max Scene Lum:", &MaxLuma, MinLuma, 2.f, "%.4f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Target Scene Lum:", &MedLuma, MinLuma, MaxLuma, "%.4f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Adaptation Max:", &ExpRange1, 0.0f, 6.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Adaptation Min:", &ExpRange2, -6.0f, 0.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Adapt Speed (+):", &AdaptSpeedPos, 0.0f, 5.f, "%.2f", 1 );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Adapt Speed (-):", &AdaptSpeedNeg, 0.0f, 5.f, "%.2f", 1 );

			r_exposure_minl->SetFloat(MinLuma);
			r_exposure_maxl->SetFloat(MaxLuma);
			r_exposure_targetl->SetFloat(MedLuma);
			r_exposure_rangeMin->SetFloat(ExpRange2);
			r_exposure_rangeMax->SetFloat(ExpRange1);

			r_light_adapt_speed_pos->SetFloat(AdaptSpeedPos) ;
			r_light_adapt_speed_neg->SetFloat(AdaptSpeedNeg) ;

			r_film_tone_a->SetFloat( a );
			r_film_tone_b->SetFloat( b );
			r_film_tone_c->SetFloat( c );
			r_film_tone_d->SetFloat( d );
			r_film_tone_e->SetFloat( e );
			r_film_tone_f->SetFloat( f );
			r_exposure_bias->SetFloat( eb );
			r_white_level->SetFloat( wl );
		}
		break;
	};

	g_pDesktopManager->End();
}

float Editor_Level::ProcessColorCorrectionMenu( HUDFilterSettings &hfs, float SliderX, float SliderY )
{
	r3dAtmosphere::SkyPhase phase0, phase1;
	float lerpT;

	GetAdjecantSkyPhasesAndLerpT( &phase0, &phase1, &lerpT );

	char CurrPhase[ 256 ];

	if( phase0 == phase1 )
	{
		sprintf( CurrPhase, "Current phase: %s", SkyPhaseToName( phase0 ) );
	}
	else
	{
		sprintf( CurrPhase, "Current phase: %s->%s", SkyPhaseToName( phase0 ), SkyPhaseToName( phase1 ) );
	}

	SliderY += imgui_Static( SliderX, SliderY, CurrPhase );

	stringlist_t phaseList;

	phaseList.push_back( "Dawn" );
	phaseList.push_back( "Day" );
	phaseList.push_back( "Dusk" );
	phaseList.push_back( "Night" );

	static float listOffset;
	static int selectedPhase;

	const float PHASE_LIST_HEIGHT = 88.f;

	imgui_DrawList( SliderX, SliderY, 360.f, PHASE_LIST_HEIGHT, phaseList, &listOffset, &selectedPhase );

	selectedPhase = R3D_MAX( R3D_MIN( selectedPhase, r3dAtmosphere::SKY_PHASE_COUNT - 1 ), 0 );

	SliderY += PHASE_LIST_HEIGHT;

	char buf[512];
	sprintf(buf, "Texture: %s", hfs.colorCorrectionTextureNames[ selectedPhase ] );
	SliderY += imgui_Static(SliderX, SliderY, buf);

	char fullPath[ 512 ];
	GetCCLUT3DTextureFullPath( fullPath, "*.dds" );

	static float offset = 0.f ;

	const r3dString& GetCCLUT3DTextureName( HUDFilters filter, r3dAtmosphere::SkyPhase phase );

	char drive[ 16 ], dir[ 512 ], name[ 512 ], ext[ 16 ] ;

	_splitpath( GetCCLUT3DTextureName( HUDFilter_Default, (r3dAtmosphere::SkyPhase)selectedPhase ).c_str(), drive, dir, name, ext ) ;

	static char textureName[ 256 ] = {0};

	strcpy( textureName, name ) ;
	strcat( textureName, ext ) ;

	const int CC_FILE_LIST_SIZE = 300;

	if ( imgui_FileList(SliderX, SliderY, 360, CC_FILE_LIST_SIZE, fullPath, textureName, &offset ) )
	{
		ReloadCCLUT3DTexture( textureName, r3dAtmosphere::SkyPhase( selectedPhase ), HUDFilter_Default );
	}

	SliderY += CC_FILE_LIST_SIZE;

	return SliderY;
}

void ToString( D3DFORMAT fmt, char* szBuffer )
{
	if ( fmt == D3DFMT_UNKNOWN )
		r3dscpy( szBuffer, "UNKNOWN" );
	else if ( fmt == D3DFMT_R8G8B8 )
		r3dscpy( szBuffer, "R8G8B8" );
	else if ( fmt == D3DFMT_A8R8G8B8 )
		r3dscpy( szBuffer, "A8R8G8B8" );
	else if ( fmt == D3DFMT_X8R8G8B8 )
		r3dscpy( szBuffer, "X8R8G8B8" );
	else if ( fmt == D3DFMT_R5G6B5 )
		r3dscpy( szBuffer, "R5G6B5" );
	else if ( fmt == D3DFMT_X1R5G5B5 )
		r3dscpy( szBuffer, "X1R5G5B5" );
	else if ( fmt == D3DFMT_A1R5G5B5 )
		r3dscpy( szBuffer, "A1R5G5B5" );
	else if ( fmt == D3DFMT_A4R4G4B4 )
		r3dscpy( szBuffer, "A4R4G4B4" );
	else if ( fmt == D3DFMT_R3G3B2 )
		r3dscpy( szBuffer, "R3G3B2" );
	else if ( fmt == D3DFMT_A8 )
		r3dscpy( szBuffer, "A8" );
	else if ( fmt == D3DFMT_A8R3G3B2 )
		r3dscpy( szBuffer, "A8R3G3B2" );
	else if ( fmt == D3DFMT_X4R4G4B4 )
		r3dscpy( szBuffer, "X4R4G4B4" );
	else if ( fmt == D3DFMT_A2B10G10R10 )
		r3dscpy( szBuffer, "A2B10G10R10" );
	else if ( fmt == D3DFMT_A8B8G8R8 )
		r3dscpy( szBuffer, "A8B8G8R8" );
	else if ( fmt == D3DFMT_X8B8G8R8 )
		r3dscpy( szBuffer, "X8B8G8R8" );
	else if ( fmt == D3DFMT_G16R16 )
		r3dscpy( szBuffer, "G16R16" );
	else if ( fmt == D3DFMT_A2R10G10B10 )
		r3dscpy( szBuffer, "A2R10G10B10" );
	else if ( fmt == D3DFMT_A16B16G16R16 )
		r3dscpy( szBuffer, "A16B16G16R16" );
	else if ( fmt == D3DFMT_A8P8 )
		r3dscpy( szBuffer, "A8P8" );
	else if ( fmt == D3DFMT_P8 )
		r3dscpy( szBuffer, "P8" );
	else if ( fmt == D3DFMT_L8 )
		r3dscpy( szBuffer, "L8" );
	else if ( fmt == D3DFMT_A8L8 )
		r3dscpy( szBuffer, "A8L8" );
	else if ( fmt == D3DFMT_A4L4 )
		r3dscpy( szBuffer, "A4L4" );
	else if ( fmt == D3DFMT_V8U8 )
		r3dscpy( szBuffer, "V8U8" );
	else if ( fmt == D3DFMT_L6V5U5 )
		r3dscpy( szBuffer, "L6V5U5" );
	else if ( fmt == D3DFMT_X8L8V8U8 )
		r3dscpy( szBuffer, "X8L8V8U8" );
	else if ( fmt == D3DFMT_Q8W8V8U8 )
		r3dscpy( szBuffer, "Q8W8V8U8" );
	else if ( fmt == D3DFMT_V16U16 )
		r3dscpy( szBuffer, "V16U16" );
	else if ( fmt == D3DFMT_A2W10V10U10 )
		r3dscpy( szBuffer, "A2W10V10U10" );
	else if ( fmt == D3DFMT_UYVY )
		r3dscpy( szBuffer, "UYVY" );
	else if ( fmt == D3DFMT_R8G8_B8G8 )
		r3dscpy( szBuffer, "R8G8_B8G8" );
	else if ( fmt == D3DFMT_YUY2 )
		r3dscpy( szBuffer, "YUY2" );
	else if ( fmt == D3DFMT_G8R8_G8B8 )
		r3dscpy( szBuffer, "G8R8_G8B8" );
	else if ( fmt == D3DFMT_DXT1 )
		r3dscpy( szBuffer, "DXT1" );
	else if ( fmt == D3DFMT_DXT2 )
		r3dscpy( szBuffer, "DXT2" );
	else if ( fmt == D3DFMT_DXT3 )
		r3dscpy( szBuffer, "DXT3" );
	else if ( fmt == D3DFMT_DXT4 )
		r3dscpy( szBuffer, "DXT4" );
	else if ( fmt == D3DFMT_DXT5 )
		r3dscpy( szBuffer, "DXT5" );
	else if ( fmt == D3DFMT_D16_LOCKABLE )
		r3dscpy( szBuffer, "D16_LOCKABLE" );
	else if ( fmt == D3DFMT_D32 )
		r3dscpy( szBuffer, "D32" );
	else if ( fmt == D3DFMT_D15S1 )
		r3dscpy( szBuffer, "D15S1" );
	else if ( fmt == D3DFMT_D24S8 )
		r3dscpy( szBuffer, "D24S8" );
	else if ( fmt == D3DFMT_D24X8 )
		r3dscpy( szBuffer, "D24X8" );
	else if ( fmt == D3DFMT_D24X4S4 )
		r3dscpy( szBuffer, "D24X4S4" );
	else if ( fmt == D3DFMT_D16 )
		r3dscpy( szBuffer, "D16" );
	else if ( fmt == D3DFMT_D32F_LOCKABLE )
		r3dscpy( szBuffer, "D32F_LOCKABLE" );
	else if ( fmt == D3DFMT_D24FS8 )
		r3dscpy( szBuffer, "D24FS8" );
/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)
    /* Z-Stencil formats valid for CPU access */
	else if ( fmt == D3DFMT_D32_LOCKABLE )
		r3dscpy( szBuffer, "D32_LOCKABLE" );
	else if ( fmt == D3DFMT_S8_LOCKABLE )
		r3dscpy( szBuffer, "S8_LOCKABLE" );
#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */
	else if ( fmt == D3DFMT_L16 )
		r3dscpy( szBuffer, "L16" );
	else if ( fmt == D3DFMT_VERTEXDATA )
		r3dscpy( szBuffer, "VERTEXDATA" );
	else if ( fmt == D3DFMT_INDEX16 )
		r3dscpy( szBuffer, "INDEX16" );
	else if ( fmt == D3DFMT_INDEX32 )
		r3dscpy( szBuffer, "INDEX32" );
	else if ( fmt == D3DFMT_Q16W16V16U16 )
		r3dscpy( szBuffer, "Q16W16V16U16" );
	else if ( fmt == D3DFMT_MULTI2_ARGB8 )
		r3dscpy( szBuffer, "MET1" );
	else if ( fmt == D3DFMT_R16F )
		r3dscpy( szBuffer, "R16F" );
	else if ( fmt == D3DFMT_G16R16F )
		r3dscpy( szBuffer, "G16R16F" );
	else if ( fmt == D3DFMT_A16B16G16R16F )
		r3dscpy( szBuffer, "A16B16G16R16F" );
	else if ( fmt == D3DFMT_R32F )
		r3dscpy( szBuffer, "R32F" );
	else if ( fmt == D3DFMT_G32R32F )
		r3dscpy( szBuffer, "G32R32F" );
	else if ( fmt == D3DFMT_A32B32G32R32F )
		r3dscpy( szBuffer, "A32B32G32R32F" );
	else if ( fmt == D3DFMT_CxV8U8 )
		r3dscpy( szBuffer, "CxV8U8" );
/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)
	else if ( fmt == D3DFMT_A1 )
		r3dscpy( szBuffer, "A1" );
	else if ( fmt == D3DFMT_BINARYBUFFER )
		r3dscpy( szBuffer, "BINARYBUFFER" );
#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */
}

char gPreviewCurrSelection[ MAX_PATH ];
bool g_EditorPreviewMaterialRecreate = false;
bool g_EditorPreviewMaterialTexReady = false;

float g_EditorPreviewSize = 256;

void Editor_Level::ProcessAssets()
{
	if ( !m_pPreviewBuffer )
		m_pPreviewBuffer = r3dScreenBuffer::CreateClass( "Preview Editor Assets", g_EditorPreviewSize, g_EditorPreviewSize, D3DFMT_A8R8G8B8, r3dScreenBuffer::Z_OWN );

	float fTmp = 14000.0f;
	DWORD nTmp = * ( ( DWORD* ) &fTmp );

	float SliderX = r3dRenderer->ScreenW - DEFAULT_CONTROLS_WIDTH - 5;
	float SliderY = 85;
	float fTopY = 45;
	float fBottomY = r3dRenderer->ScreenH - 45;
	float fCenterY = fTopY + 200;

	static const char* rootDir = "Data/ObjectsDepot/";
	
	static const char* extList[] = { ".sco", ".dds", ".tga", ".mat" };
	const int nExtNum = sizeof( extList ) / sizeof( extList[ 0 ] );
	static const char* ExtFilter = ".sco|.mat|.dds|.tga";
	
	static int nIndex = 0;	
	static float fOffset = 0;
	static bool bShowPreview = false;
	static int nDragIndex = -1;
	static char dirName[MAX_PATH] = { 0 };
	static char fileName[MAX_PATH] = { 0 };
	static char fileFind[MAX_PATH] = { 0 };
	static GameObject* pPreviewObj = NULL;
	static bool bNeedObjPrev = false;

	bool bNeedPickerEnable = g_iPickerEnableTicked == 0;

	if ( bNeedPickerEnable )
		g_Manipulator3d.Enable ();

	if ( g_Manipulator3d.IsLocked() )
		g_Manipulator3d.Unlock();

	if ( !g_Manipulator3d.IsEnable () )
		g_Manipulator3d.Enable ();

	g_iPickerEnableTicked = 2;

	if ( nDragIndex >= 0 )
	{
		if ( nIndex == 0 )
		{
			Hud->ProcessPick();
			if ( ( UI_TargetPos - Hud->FPS_Position ).LengthSq() > R3D_EPSILON )
			{
				bNeedObjPrev = false;
				if (!pPreviewObj )
				{
					pPreviewObj = srv_CreateGameObject( "obj_Building", gPreviewCurrSelection, UI_TargetPos );
					pPreviewObj->ObjFlags |= OBJFLAG_SkipCastRay;
				}

				if ( pPreviewObj )
				{
					if ( Keyboard->IsPressed( kbsLeftShift ) || Keyboard->IsPressed( kbsRightShift ) )
						LandscapeCorrect( *pPreviewObj, UI_TargetPos );
					else
					{
						pPreviewObj->SetPosition(UI_TargetPos);
						pPreviewObj->SetRotationVector( r3dVector( 0, 0, 0 ) );
					}
				}
				if ( imgui_lbr )
				{
					pPreviewObj->ObjFlags &= ~( OBJFLAG_SkipCastRay );
					pPreviewObj = NULL;
				}
			}
			else
			{
				bNeedObjPrev = true;
				if ( pPreviewObj )
				{
					GameWorld().DeleteObject( pPreviewObj );
					pPreviewObj = NULL;
				}
			}
		}
		else if ( nIndex == 4 )
		{
			if ( imgui_lbr )
				srv_CreateGameObject( "obj_ParticleSystem", fileName, UI_TerraTargetPos );
		}
	}
	else
	{
		if ( pPreviewObj )
		{
			GameWorld().DeleteObject( pPreviewObj );
			pPreviewObj = NULL;
		}
	}

	int nSelectIndex = 0 ;

	{
		static float offset ;
		static int index ;

		if ( imgui_DirList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, fCenterY - fTopY - 5, rootDir, ExtFilter, dirName, &offset, &index ) )
		{
			nSelectIndex = -1;
			fOffset = 0;
		}
		SliderY += fCenterY - fTopY;
	}

	static bool bNewSelect = false;
	if ( strstr( dirName, "particles" ) )
	{
		CategoryStruct cat;
		FILE *f = fopen("Data/ObjectsDepot/Particles/Objects.dat","rt");
		fscanf(f, "%d", &cat.NumObjects);
		for (int y=0;y<cat.NumObjects;y++)
		{
			char D1[64], D2[64];
			fscanf(f,"%s %s", D1, D2);
			cat.ObjectsNames.push_back(D1);
			cat.ObjectsClasses.push_back(D2);
			cat.ObjectsSubfolders.push_back("");
		}
		fclose(f);

		static int selIdx = -1 ;

		if( imgui_DrawList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, fBottomY - SliderY, cat.ObjectsNames, &fOffset, &selIdx, true, false, false, &nDragIndex ) )
			bNewSelect = true;
		if ( selIdx < 0 )
			fileName[ 0 ] = 0;
		else
			r3dscpy( fileName, cat.ObjectsNames[ selIdx ].c_str() );
		nIndex = 4;

		nSelectIndex = selIdx ;
	} 
	else
	{
		sprintf( fileFind, "%s/*", dirName );
		if ( imgui_FileList( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, fBottomY - SliderY, fileFind, fileName, &fOffset, true, false, &nDragIndex, ExtFilter, &nSelectIndex ) )
		{
			sprintf( gPreviewCurrSelection, "%s/%s", dirName, fileName );
			bNewSelect = true;
			nIndex = -1;
		}
	}

	if ( nSelectIndex < 0 )
		return;
	
	const float fPButtonSize = 14;
	float fY = SliderY + 17 * ( nSelectIndex - fOffset ) + 5;
	if ( fY < fCenterY )
		fY = fCenterY;
	if ( fY > fBottomY - fPButtonSize )
		fY = fBottomY - fPButtonSize;
	if ( imgui_Button( SliderX - fPButtonSize, fY, fPButtonSize, fPButtonSize, "P", bShowPreview ) )
		bShowPreview = !bShowPreview;

	bool bDragPreview = nDragIndex >= 0;
	const float fDragPreviewSize = 100.0f;

	if ( bShowPreview || bDragPreview )
	{
		float fPreviewHalfSize = g_EditorPreviewSize / 2;
		float fX;

		char buffer[ 256 ];

		if ( nIndex == -1 )
		{
			String_T<> sExt = String_T<>( fileName ).GetExt();
			for ( int i = 0; i < nExtNum; i++ )
			{
				if ( strcmp( sExt, extList[ i ] ) == 0 )
				{
					nIndex = i;
					break;
				}
			}
		}

		if ( bDragPreview && nIndex != 4 && !bNeedObjPrev )
		{
			bDragPreview = false;
			if ( !bShowPreview )
				return;
		}

		if ( bDragPreview )
		{
			fX = imgui_mx + 10;
			fY = imgui_my;
		}
		else
		{
			fX = SliderX - g_EditorPreviewSize - fPButtonSize;
			fY -= fPreviewHalfSize;
			if ( fY < fCenterY )
				fY = fCenterY;
			if ( fY + g_EditorPreviewSize + fPreviewHalfSize / 2 > fBottomY )
				fY = fBottomY - g_EditorPreviewSize - fPreviewHalfSize / 2;
		}

		if ( nIndex == 0 )
		{
			static r3dTexture* pMeshTex = NULL;
			if ( bNewSelect )
			{
				bNewSelect = false;
				if ( pMeshTex )
				{
					r3dRenderer->DeleteTexture( pMeshTex );
					pMeshTex = NULL;
				}

				extern bool getFileTimestamp(const char*, FILETIME&);
				extern void setFileTimestamp(const char*, const FILETIME&);
				FILETIME creationTime;
				getFileTimestamp(gPreviewCurrSelection, creationTime);

				String_T<> sFullName( fileName );
				char cacheName[ MAX_PATH ];
				sprintf( cacheName, "Data/Editor/PreviewCache/Meshes/%s.dds", sFullName.GetName().c_str() );
				bool bLoadFromMesh = false;
				if ( r3dFileExists( cacheName ) )
				{
					FILETIME texTime;
					getFileTimestamp(cacheName, texTime);
					bLoadFromMesh = texTime.dwHighDateTime != creationTime.dwHighDateTime || texTime.dwLowDateTime != creationTime.dwLowDateTime;
				}
				else
					bLoadFromMesh = true;
				
				if ( bLoadFromMesh )
				{
					r3dMesh mesh( 0 );
					mesh.Load(gPreviewCurrSelection, false, true);

					if(mesh.IsLoaded()) 
					{
						mesh.FillBuffers();
						r3dCamera camera;
						camera.FOV = gCam.FOV;
						camera.Aspect = 1.0f;
						D3DXVECTOR3 vPos( 1.5f * r3dTL::Max( mesh.localBBox.Size.x, mesh.localBBox.Size.z ), mesh.localBBox.Size.y, 0 );
						D3DXVECTOR3 vDir = -vPos;
						D3DXVec3Normalize( &vDir, &vDir );
						D3DXVECTOR3 vDirProj( vDir.x, 0, vDir.z );
						D3DXVECTOR3 vUp( 0, 1, 0 );
						D3DXVec3Cross( &vUp, &vDirProj, &vDir );
						D3DXVec3Cross( &vUp, &vDir, &vUp );
						D3DXVec3Normalize( &vUp, &vUp );
						D3DXMATRIX m;
						D3DXMatrixRotationY( &m, -R3D_PI_4 );
						D3DXVec3TransformNormal( &vPos, &vPos, &m );
						D3DXVec3TransformNormal( &vUp, &vUp, &m );
						D3DXVec3TransformNormal( &vDir, &vDir, &m );
						
						camera.SetPosition( r3dPoint3D( vPos ) );
						camera.PointTo( r3dPoint3D( vPos + vDir ) );
						camera.vUP = r3dPoint3D( vUp );

						r3dRenderer->SetCamera( camera, false );
						D3DXMatrixIdentity( &m );
						mesh.SetShaderConsts( m );

						r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZW | R3D_BLEND_ZC | R3D_BLEND_PUSH);

						m_pPreviewBuffer->zType = r3dScreenBuffer::Z_SYSTEM;
						m_pPreviewBuffer->Activate();
						r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB( 0, 0, 0, 0 ), r3dRenderer->GetClearZValue(), 0 );
						r3dRenderer->SetVertexShader( "FORVARD_VS" );
						r3dRenderer->SetPixelShader( "FORVARD_PS" );
						mesh.DrawMeshSimple( 1 );
						MenuFont_Editor->PrintF( 5, g_EditorPreviewSize - 45, r3dColor::white, "Materials: %i\nVertexes: %i\nFaces: %i\n", mesh.GetNumMaterials(), mesh.GetNumVertexes(), mesh.GetNumPolygons() );
						m_pPreviewBuffer->Deactivate();
						m_pPreviewBuffer->zType = r3dScreenBuffer::Z_OWN;

						r3dRenderer->SetVertexShader();
						r3dRenderer->SetPixelShader();

						r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

						m_pPreviewBuffer->Tex->Save( cacheName );

						setFileTimestamp( cacheName, creationTime );

						mesh.Unload();
					}
				}

				pMeshTex = r3dRenderer->LoadTexture( cacheName );
			}

			r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA |R3D_BLEND_PUSH);
			float fSize;
			if ( bDragPreview )
				fSize = fDragPreviewSize;
			else
				fSize = g_EditorPreviewSize;
			r3dDrawBox2D( fX, fY, fSize, fSize, r3dColor::white, pMeshTex );
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}
		else if ( nIndex == 1 || nIndex == 2 )
		{
			static r3dTexture* pTex = NULL;
			if ( bNewSelect )
			{
				bNewSelect = false;
				if ( pTex )
				{
					r3dRenderer->DeleteTexture( pTex );
					pTex = NULL;
				}
				pTex = r3dRenderer->LoadTexture( gPreviewCurrSelection );
			}
			if ( pTex )
			{
				float fWH = float( pTex->GetWidth() ) / float( pTex->GetHeight() );
				float fHW = float( pTex->GetHeight() ) / float( pTex->GetWidth() );
				float TC[16] = { 0 };
				if ( fWH > 1 )
				{
					TC[2]	= 1;	TC[5]	= fWH;
					TC[4]	= 1;	TC[7]	= fWH;
				}
				else
				{
					TC[2]	= fHW;	TC[5]	= 1;
					TC[4]	= fHW;	TC[7]	= 1;
				}

				DWORD dwOldStateU = D3DTADDRESS_CLAMP;
				DWORD dwOldStateV = D3DTADDRESS_CLAMP;

				r3dRenderer->pd3ddev->GetSamplerState( 0, D3DSAMP_ADDRESSU, &dwOldStateU);
				r3dRenderer->pd3ddev->GetSamplerState( 0, D3DSAMP_ADDRESSV, &dwOldStateV);

				r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
				r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );
				
				float fSize;
				if ( bDragPreview )
					fSize = fDragPreviewSize;
				else
					fSize = g_EditorPreviewSize;
				r3dDrawBox2D( fX, fY, fSize, fSize, r3dColor( 255, 255, 255 ), pTex, TC );
				
				r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, dwOldStateU );
				r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, dwOldStateV );

				if ( !bDragPreview )
				{
					int nMips = pTex->GetNumMipmaps() < 0 ? 0 : pTex->GetNumMipmaps(); 
					char szFormat[ 24 ];
					ToString( pTex->GetD3DFormat(), szFormat );
					sprintf_s ( buffer, 256, "Format: %s\nSize: %ix%i\nMips: %i", szFormat, pTex->GetWidth(), pTex->GetHeight(), nMips );
					imgui_Static( fX, fY + g_EditorPreviewSize, buffer, g_EditorPreviewSize, true, fPreviewHalfSize / 2 );
				}
			}
		}
		else if ( nIndex == 3 )
		{
			static r3dTexture* pMatTex = NULL;

			if ( bNewSelect )
			{
				bNewSelect = false;
				if ( pMatTex )
				{
					r3dRenderer->DeleteTexture( pMatTex );
					pMatTex = NULL;
				}

				extern bool getFileTimestamp(const char*, FILETIME&);
				extern void setFileTimestamp(const char*, const FILETIME&);
				FILETIME creationTime;
				getFileTimestamp(gPreviewCurrSelection, creationTime);

				String_T<> sFullPath( gPreviewCurrSelection );
				char cacheName[ MAX_PATH ];
				sprintf( cacheName, "Data/Editor/PreviewCache/Materials/%s.dds", sFullPath.GetName().c_str() );
				if ( r3dFileExists( cacheName ) )
				{
					FILETIME texTime;
					getFileTimestamp(cacheName, texTime);
					g_EditorPreviewMaterialRecreate = texTime.dwHighDateTime != creationTime.dwHighDateTime || texTime.dwLowDateTime != creationTime.dwLowDateTime;
				}
				else
					g_EditorPreviewMaterialRecreate = true;
				g_EditorPreviewMaterialTexReady = !g_EditorPreviewMaterialRecreate;
			}

			if ( g_EditorPreviewMaterialTexReady )
			{
				String_T<> sFullName( fileName );
				char cacheName[ MAX_PATH ];
				sprintf( cacheName, "Data/Editor/PreviewCache/Materials/%s.dds", sFullName.GetName().c_str() );
				pMatTex = r3dRenderer->LoadTexture( cacheName );
				g_EditorPreviewMaterialTexReady = false;
			}

			r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA |R3D_BLEND_PUSH);
			float fSize;
			if ( bDragPreview )
				fSize = fDragPreviewSize;
			else
				fSize = g_EditorPreviewSize;
			r3dDrawBox2D( fX, fY, fSize, fSize, r3dColor::white, pMatTex );
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}
		else if ( nIndex == 4 )
		{
			static r3dParticleSystem* pPartSys = NULL;
			
			if ( bNewSelect )
			{
				bNewSelect = false;
				SAFE_DELETE( pPartSys );
				sprintf(buffer,"Data\\Particles\\%s.prt", fileName);
				pPartSys = r3dParticleSystemLoad( buffer );
				if (pPartSys)
				{
					pPartSys->Position = r3dPoint3D( 0, 0, 0 );
					pPartSys->Restart(r3dGetTime());
				}
			}

			if ( pPartSys )
			{
				if( pPartSys->bEmit == 0 && pPartSys->NumAliveParticles == 0 )
				{
					 pPartSys->Restart(r3dGetTime());
				}

				pPartSys->Update(r3dGetTime());

				r3dCamera camera;
				camera.FOV = gCam.FOV;
				camera.Aspect = 1.0f;
				D3DXVECTOR3 vPos( 50, 50, 50 );
				D3DXVECTOR3 vDir = -vPos;
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXVECTOR3 vDirProj( vDir.x, 0, vDir.z );
				D3DXVec3Normalize( &vDirProj, &vDirProj );
				D3DXVECTOR3 vUp( 0, 1, 0 );
				D3DXVec3Cross( &vUp, &vDirProj, &vDir );
				D3DXVec3Cross( &vUp, &vDirProj, &vUp );
				D3DXMATRIX m;
				
				camera.SetPosition( r3dPoint3D( vPos ) );
				camera.PointTo( r3dPoint3D( vPos + vDir ) );
				camera.vUP = r3dPoint3D( vUp );
				r3dRenderer->SetCamera( camera, false );

				if ( !bDragPreview )
				{
					int nSystemsNum = 0;
					for ( int i = 0; i < r3dParticleData::MAX_EMITTER_SLOTS; i++ )
						if ( pPartSys->PD->PType[ i ] != NULL )
							nSystemsNum++;

					sprintf_s ( buffer, 256, "Systems Num: %i\nMax Particles: %i\n", nSystemsNum, pPartSys->CalculatedArraySize );
					imgui_Static( fX, fY + g_EditorPreviewSize, buffer, g_EditorPreviewSize, true, fPreviewHalfSize / 2 );
				}

				extern r3dScreenBuffer* DepthBuffer;
				DepthBuffer->Activate();
				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
				extern int PS_CLEAR_FLOAT_ID;
				extern int VS_CLEAR_FLOAT_ID;
				r3dRenderer->SetPixelShader( VS_CLEAR_FLOAT_ID );
				r3dRenderer->SetPixelShader( PS_CLEAR_FLOAT_ID );
				r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, D3DXVECTOR4( r3dRenderer->FarClip, 0, 0, 0 ), 1 );
				r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::white );
				r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
				DepthBuffer->Deactivate();

				m_pPreviewBuffer->Activate();
				r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB( 0, 0, 0, 0 ), r3dRenderer->GetClearZValue(), 0 );
				pPartSys->Draw( camera, false );
				m_pPreviewBuffer->Deactivate();

				r3dRenderer->Flush();
				r3dRenderer->SetMaterial(NULL);
					
				r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA |R3D_BLEND_PUSH);
				float fSize;
				if ( bDragPreview )
					fSize = fDragPreviewSize;
				else
					fSize = g_EditorPreviewSize;
				r3dDrawBox2D( fX, fY, fSize, fSize, r3dColor::white, m_pPreviewBuffer->Tex );
				r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
			}
		}
	}
}

void Editor_Level::ProcessParticleGun()
{
	float SliderX = r3dRenderer->ScreenW - DEFAULT_CONTROLS_WIDTH - 5;
	float SliderY = 45;
}

//////////////////////////////////////////////////////////////////////////

r3dPoint3D Navigation_GetWorldCursorPos()
{
	return UI_TargetPos;
}

//////////////////////////////////////////////////////////////////////////

void Editor_Level::ProcessNavigation()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	const static char* listpaint[] = { "REGIONS", "NAVMESH" };

	static int navEditTab = 1;
	int prev = navEditTab;
	SliderY += imgui_Toolbar(SliderX, SliderY, 360, 45, &navEditTab, 0, listpaint, sizeof listpaint / sizeof listpaint[0]);

	switch (navEditTab)
	{
	case 1:
		ProcessAutodeskNavigation(SliderX, SliderY);
		break;
	case 0:
		ProcessNavigationRegions(SliderX, SliderY);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void Editor_Level::ProcessNavigationRegions(float SliderX, float SliderY)
{
	static int uiMode = 0;
	static int selectedRegion = 0;
	static int prevSelectedRegion = 0;

	if (uiMode == 1)
	{
		static bool wasPressed = false;
		if (!imgui_lbp)
			wasPressed = false;

		SliderY += imgui_Static(SliderX, SliderY, "Click on map to define region corner.");
		SliderY += imgui_Static(SliderX, SliderY, "When done press \"Complete region\"");

		if (imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Complete region"))
		{
			uiMode = 0;
		}
		else if (imgui_lbr && !wasPressed)
		{
			wasPressed = true;
			Nav::gConvexRegionsManager.AddPointToRegion(Navigation_GetWorldCursorPos(), selectedRegion);
		}

	}
	else if (uiMode == 2)
	{
		// add seed point
		SliderY += imgui_Static(SliderX, SliderY, "Click on map to define seed point.");
		if (imgui_lbr)
		{
			Nav::ConvexRegionsManager::Convex& c = Nav::gConvexRegionsManager.regions[selectedRegion];
			c.haveSeed  = true;
			c.seedPoint = Navigation_GetWorldCursorPos();
			
			uiMode = 0;
		}
	}
	else
	{
		static int visNavRegions = 1;
		SliderY += imgui_Checkbox(SliderX, SliderY, "Visualize Regions", &visNavRegions, 1);
		Nav::gConvexRegionsManager.doDebugDraw = visNavRegions != 0;

		static stringlist_t regionNames;
		regionNames.clear();

		for (uint32_t i = 0; i < Nav::gConvexRegionsManager.GetNumRegions(); ++i)
		{
			char buf[5] = {0};
			regionNames.push_back(r3dSTLString("Region") + itoa(i, buf, 10));
		}

		SliderY += imgui_Static(SliderX, SliderY, "Navigation regions:");
		float listHeight = 400.0f;
		static float selectedOffset = 0.0f;
		imgui_DrawList(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, listHeight, regionNames, &selectedOffset, &selectedRegion);
		Nav::gConvexRegionsManager.HightlightRegion(selectedRegion);

		SliderY += listHeight;
		float buttonW = DEFAULT_CONTROLS_WIDTH / 3;
		if (imgui_Button(SliderX, SliderY, buttonW, DEFAULT_CONTROLS_HEIGHT, "Add new region"))
		{
			Nav::gConvexRegionsManager.CreateNewRegion();
		}
		if (imgui_Button(SliderX + buttonW, SliderY, buttonW, DEFAULT_CONTROLS_HEIGHT, "Delete region"))
		{
			Nav::gConvexRegionsManager.DeleteRegion(selectedRegion);
		}
		if (imgui_Button(SliderX + buttonW * 2, SliderY, buttonW, DEFAULT_CONTROLS_HEIGHT, "Modify region"))
		{
			uiMode = 1;
		}

		if(selectedRegion < (int)Nav::gConvexRegionsManager.GetNumRegions())
		{
			SliderY += DEFAULT_CONTROLS_HEIGHT + 5;
			SliderY += imgui_Static(SliderX, SliderY, "Region config:");
			Nav::ConvexRegionsManager::Convex& c = Nav::gConvexRegionsManager.regions[selectedRegion];
			if(imgui_Button(SliderX, SliderY, buttonW, DEFAULT_CONTROLS_HEIGHT, "Next Region"))
			{
				selectedRegion++;
				if(selectedRegion >= (int)Nav::gConvexRegionsManager.GetNumRegions())
					selectedRegion = 0;
			}

			// go to new region if selection was changed
			if (prevSelectedRegion != selectedRegion)
			{
				prevSelectedRegion = selectedRegion;
				
				r3dPoint3D pos = r3dPoint3D((c.maxPt.x + c.minPt.x) / 2, 0, (c.maxPt.y + c.minPt.y) / 2);
				if(Terrain) pos.y = Terrain->GetHeight(pos) + 30.0f;
			
				if(Hud)	Hud->SetCamPos(pos);
				else	gCam = pos;
			}

			if(c.haveSeed)
			{
				if (imgui_Button(SliderX + buttonW, SliderY, buttonW, DEFAULT_CONTROLS_HEIGHT, "Remove Seed"))
				{
					c.haveSeed = false;
				}

				if (imgui_Button(SliderX + buttonW * 2, SliderY, buttonW, DEFAULT_CONTROLS_HEIGHT, "Go To Seed"))
				{
					r3dPoint3D pos = c.seedPoint + r3dPoint3D(0, 1, 0);
					if(Hud)	Hud->SetCamPos(pos);
					else	gCam = pos;
				}
			}
			else
			{
				if (imgui_Button(SliderX + buttonW, SliderY, buttonW, DEFAULT_CONTROLS_HEIGHT, "Set Seed"))
				{
					uiMode = 2;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void Editor_Level::ProcessAutodeskNavigation(float SliderX, float SliderY)
{
#if ENABLE_AUTODESK_NAVIGATION

	static int selectedRegion = 0;
	//	0 - normal mode, 1 - create agent mode, 2 - navigate agent mode
	//	3 - create obstacle
	//	4 - create 100 agents around the point
	//	5 - define seed point
	static int uiMode = 0;

	static bool doReinit = false;
	static r3dTL::TArray<AutodeskNavAgent *> testAgents;

	if (uiMode == 1)
	{
		SliderY += imgui_Static(SliderX, SliderY, "Click on map to place test navigation agent");
		if (imgui_lbr)
		{
 			AutodeskNavAgent *a = gAutodeskNavMesh.CreateNavAgent(Navigation_GetWorldCursorPos());
 			testAgents.PushBack(a);
 			uiMode = 0;
		}
	}
	else if (uiMode == 2)
	{
		SliderY += imgui_Static(SliderX, SliderY, "Click on map to define navigation target");
		if (imgui_lbr)
		{
			r3dPoint3D target = Navigation_GetWorldCursorPos();
			for (uint32_t i = 0; i < testAgents.Count(); ++i)
			{
				testAgents[i]->StartMove(target);
				if(Keyboard->IsPressed(kbsLeftShift)) break;
			}
			uiMode = 0;
		}
	}
	else if (uiMode == 3)
	{
		SliderY += imgui_Static(SliderX, SliderY, "Click on map to define obstacle position");
		if (imgui_lbr)
		{
			r3dPoint3D target = Navigation_GetWorldCursorPos();
			r3dBoundBox bb;
			bb.Org  = target;
			bb.Size = r3dVector(4.0f, 4.0f, 4.0f);
			bb.Org.x -= bb.Size.x / 2;
			bb.Org.z -= bb.Size.z / 2;
			
			float rot = 0;
			gAutodeskNavMesh.AddObstacle(NULL, bb, rot);
			uiMode = 0;
		}
	}
	else if (uiMode == 4)
	{
		SliderY += imgui_Static(SliderX, SliderY, "Click on map to define area of agents spawn");
		if (imgui_lbr)
		{
			for (uint32_t i = 0; i < 100; ++i)
			{
				PxVec3 randomVec(r3dRandomFloat(), 0, r3dRandomFloat());
				randomVec.normalize();
				PxVec3 tp(UI_TargetPos.x, UI_TargetPos.y, UI_TargetPos.z);
				randomVec = tp + randomVec * random(RAND_MAX) / static_cast<float>(RAND_MAX) * 20;
				randomVec.y += 1000.0f;

				PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eDYNAMIC|PxSceneQueryFilterFlag::eSTATIC));
				PxSceneQueryFlags queryFlags(PxSceneQueryFlag::eIMPACT);
				PxRaycastHit hit;
				if (g_pPhysicsWorld->raycastSingle(randomVec, PxVec3(0, -1, 0), 20000.0f, queryFlags, hit, filter))
				{
					randomVec.y = hit.impact.y;
				}

				AutodeskNavAgent *a = gAutodeskNavMesh.CreateNavAgent(r3dPoint3D(randomVec.x, randomVec.y, randomVec.z));
				testAgents.PushBack(a);
			}
			uiMode = 0;
		}
	}
	else
	{
		Kaim::GeneratorParameters &gc = gAutodeskNavMesh.buildGlobalConfig;

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Entity Height (m)", &gc.m_entityHeight, 0.5f, 2.5f, "%-02.02f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Entity Radius (m)", &gc.m_entityRadius, 0.1f, 0.7f, "%-02.02f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Step Max (m)", &gc.m_stepMax, 0.1f, 1.5f, "%-02.02f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Slope Max (deg)", &gc.m_slopeMax, 0.1f, 90.0f, "%-02.02f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Raster Precision (m)", &gc.m_rasterPrecision, 0.01f, 1.0f, "%-02.02f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Cell size (m)", &gc.m_cellSize, 1.0f, 40.0f, "%-02.02f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Alt. Precision (m)", &gc.m_altitudeTolerance, 0.05f, 2.0f, "%-02.02f");
		SliderY += imgui_Value_Slider(SliderX, SliderY, "V. Sample Step (m)", &gc.m_advancedParameters.m_altitudeToleranceSamplingStep, 0.01f, 0.7f, "%-02.02f");

		static int visNavMesh = 1;
		SliderY += imgui_Checkbox(SliderX, SliderY, "Visualize Navigation Mesh", &visNavMesh, 1);
		gAutodeskNavMesh.doDebugDraw = visNavMesh != 0;
#ifndef FINAL_BUILD
		d_navmesh_debug->SetBool( gAutodeskNavMesh.doDebugDraw );
#endif // FINAL_BUILD

		float button2W = DEFAULT_CONTROLS_WIDTH / 2;
		if (imgui_Button(SliderX, SliderY, button2W, DEFAULT_CONTROLS_HEIGHT, "Build Navigation Mesh"))
		{
			// gAutodeskNavMesh.BuildForCurrentLevel(); - need to use external tool
			gAutodeskNavMesh.ExportToObj();
		}
		if (imgui_Button(SliderX + button2W, SliderY, button2W, DEFAULT_CONTROLS_HEIGHT, "Reload NavMesh"))
		{
			doReinit = true;
		}
		SliderY += DEFAULT_CONTROLS_HEIGHT;
		SliderY += imgui_Static(SliderX, SliderY, "Navigation test:");
		float button3W = DEFAULT_CONTROLS_WIDTH / 3;

		if (imgui_Button(SliderX, SliderY, button3W, DEFAULT_CONTROLS_HEIGHT, "Create agent"))
		{
			uiMode = 1;
		}
		if (imgui_Button(SliderX + button3W, SliderY, button3W, DEFAULT_CONTROLS_HEIGHT, "Navigate to"))
		{
			uiMode = 2;
		}
		if (imgui_Button(SliderX + button3W * 2, SliderY, button3W, DEFAULT_CONTROLS_HEIGHT, "Create obstacle"))
		{
			uiMode = 3;
		}
		SliderY += DEFAULT_CONTROLS_HEIGHT;
		if (imgui_Button(SliderX, SliderY, button2W, DEFAULT_CONTROLS_HEIGHT, "Create 100 agents"))
		{
			uiMode = 4;
		}
		if (imgui_Button(SliderX + button2W, SliderY, button2W, DEFAULT_CONTROLS_HEIGHT, "Remove all agents"))
		{
			for (uint32_t i = 0; i < testAgents.Count(); ++i)
			{
				gAutodeskNavMesh.DeleteNavAgent(testAgents[i]);
			}
			testAgents.Clear();
		}
	}
	
	if (doReinit/* && Nav::gNavMeshBuildComplete != 0*/)
	{
		gAutodeskNavMesh.Init();
		doReinit = false;
		testAgents.Clear();
	}

#endif
}

//------------------------------------------------------------------------

float Editor_Level::ProcessStaticSky( float SliderX, float SliderY )
{
	int StaticCustomMeshEnable = r3dGameLevel::Environment->bCustomStaticMeshEnable ;
	int StaticSkyEnable = r3dGameLevel::Environment->bStaticSkyEnable;

	if( StaticSkyEnable )
	{
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Skydome Rot Y", &r3dGameLevel::Environment->SkyDomeRotationY, 0.f, 360.f, "%.1f" );

		SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Use Custom Skydome Mesh", &StaticCustomMeshEnable, 1 ) ;
		r3dGameLevel::Environment->bCustomStaticMeshEnable = !!StaticCustomMeshEnable ;

		static char SkyTexName[ MAX_PATH ] ;
		static char SkyGlowTexName[ MAX_PATH ] ;
		static char SkyMeshName[ MAX_PATH ] ;

#define STATICSKY_PATH "Data/SkyDome/"
		const int FILE_LIST_HEIGHT = 330 ;

		if( !StaticCustomMeshEnable )
		{
			int PlanarMapping = !!r3dGameLevel::Environment->bStaticSkyPlanarMapping ;

			SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Planar Mapping", &PlanarMapping, 1 ) ;

			r3dGameLevel::Environment->bStaticSkyPlanarMapping = !!PlanarMapping ;

			if( PlanarMapping )
			{
				static float TexScale = 0.f ; 
				static float TexOffset = 0.f ; 

				const float UPSCALE = 100000.f ;

				TexScale	= r3dGameLevel::Environment->StaticTexGenScaleX * UPSCALE ;
				TexOffset	= r3dGameLevel::Environment->StaticTexGetOffsetX ;

				SliderY += imgui_Value_Slider(SliderX, SliderY, "Texcoord Scale", &TexScale, 0.001f, 16.f, "%-02.3f", 1, false);
				SliderY += imgui_Value_Slider(SliderX, SliderY, "Texcoord Offset", &TexOffset, -1.f, 1.f, "%-02.2f", 1, false);

				r3dGameLevel::Environment->StaticTexGenScaleX = TexScale / UPSCALE ;
				r3dGameLevel::Environment->StaticTexGenScaleY = r3dGameLevel::Environment->StaticTexGenScaleX ;

				r3dGameLevel::Environment->StaticTexGetOffsetX = TexOffset ;
				r3dGameLevel::Environment->StaticTexGetOffsetY = r3dGameLevel::Environment->StaticTexGetOffsetX ;
			}
		}
		else
		{
			static float offset = 0.f ;

			SliderY += imgui_Static( SliderX, SliderY, "Mesh List:", 360);

			char drive[ 16 ], dir[ 512 ], name[ 512 ], ext[ 16 ] ;

			_splitpath( r3dGameLevel::Environment->StaticSkyMeshName.c_str(), drive, dir, name, ext ) ;

			strcpy( SkyMeshName, name ) ;
			strcat( SkyMeshName, ext ) ;

			if( imgui_FileList( SliderX, SliderY, 360, 60, STATICSKY_PATH "*.sco", SkyMeshName, &offset ) )
			{
				char SS1[ MAX_PATH ];

				sprintf( SS1, STATICSKY_PATH"%s", SkyMeshName ) ;

				r3dGameLevel::Environment->SetStaticSkyMesh( SS1 );
			}

			SliderY += FILE_LIST_HEIGHT ;
		}

		// phases
		{
			SliderY += imgui_Static( SliderX, SliderY, "Phases", 360 );

			SliderY += imgui_Value_Slider( SliderX, SliderY, "Dawn Start", &r3dGameLevel::Environment->DawnStart, 0.f, r3dGameLevel::Environment->DawnEnd - 0.5f, "%.2f" );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Dawn End", &r3dGameLevel::Environment->DawnEnd, r3dGameLevel::Environment->DawnStart + 0.5f, r3dGameLevel::Environment->DuskStart - 0.5f, "%.2f" );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Dusk Start", &r3dGameLevel::Environment->DuskStart, r3dGameLevel::Environment->DawnEnd + 0.5f, r3dGameLevel::Environment->DuskEnd - 0.5f, "%.2f" );
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Dusk End", &r3dGameLevel::Environment->DuskEnd, r3dGameLevel::Environment->DuskStart + 0.5f, 24.f, "%.2f" );

			// current phase
			{
				char CurrPhase[ 256 ];

				r3dAtmosphere::SkyPhase phase0, phase1;
				float lerpT;

				GetAdjecantSkyPhasesAndLerpT( &phase0, &phase1, &lerpT );			

				if( phase0 == phase1 )
				{
					sprintf( CurrPhase, "Current phase: %s", SkyPhaseToName( phase0 ) );
				}
				else
				{
					sprintf( CurrPhase, "Current phase: %s->%s", SkyPhaseToName( phase0 ), SkyPhaseToName( phase1 ) );
				}

				SliderY += imgui_Static( SliderX, SliderY, CurrPhase );
			}

			SliderY += imgui_Static( SliderX, SliderY, "Set parameters for:", 360 );

			stringlist_t phaseNames;

			phaseNames.push_back( "Dawn" );
			phaseNames.push_back( "Day" );
			phaseNames.push_back( "Dusk" );
			phaseNames.push_back( "Night" );

			static float phases_offset;
			static int index;

			const int HEIGHT = 88;

			imgui_DrawList( SliderX, SliderY, 360 - 22, HEIGHT, phaseNames, &phases_offset, &index );

			SliderY += HEIGHT;

			index = R3D_MIN( R3D_MAX( index, 0 ), r3dAtmosphere::SKY_PHASE_COUNT - 1 );

			SliderY += imgui_Value_Slider( SliderX, SliderY, "Sky Intensity", &r3dGameLevel::Environment->StaticSkyIntensities[ index ], 0, 20.f, "%.2f" );

			SliderY += imgui_Static( SliderX, SliderY, "Sky Texture:", 360);

			static float sky_tex_offset = 0.f, sky_glow_tex_offset = 0.0f;

			// get sky texture names without path
			{
				char drive[ 16 ], dir[ 512 ], name[ 512 ], ext[ 16 ] ;

				_splitpath( r3dGameLevel::Environment->StaticSkyTextureNames[ index ].c_str(), drive, dir, name, ext ) ;

				strcpy( SkyTexName, name );
				strcat( SkyTexName, ext );

				_splitpath( r3dGameLevel::Environment->StaticSkyGlowTextureNames[ index ].c_str(), drive, dir, name, ext ) ;

				strcpy( SkyGlowTexName, name );
				strcat( SkyGlowTexName, ext );
			}

			bool texesChanged = false;

			r3dAtmosphere::SkyPhaseTextureNames skyTexNames = r3dGameLevel::Environment->StaticSkyTextureNames;
			r3dAtmosphere::SkyPhaseTextureNames skyGlowTexNames = r3dGameLevel::Environment->StaticSkyGlowTextureNames;

			const float TEX_FILE_LIST_HEIGHT = 200.0f;

			if( imgui_FileList( SliderX, SliderY, 360, TEX_FILE_LIST_HEIGHT, STATICSKY_PATH "*.dds", SkyTexName, &sky_tex_offset ) )
			{
				char SS1[ MAX_PATH ];

				sprintf( SS1, STATICSKY_PATH"%s", SkyTexName );

				skyTexNames[ index ] = SS1;

				texesChanged = true;				
			}

			SliderY += TEX_FILE_LIST_HEIGHT;

			float delta = imgui_Static( SliderX, SliderY, "Glow Texture:", 360);

			if( imgui_Button( SliderX + 95, SliderY + 6, 80, 16, "Clear" ) )
			{
				SkyGlowTexName[ 0 ] = 0;
				skyGlowTexNames[ index ] = SkyGlowTexName;
				texesChanged = true;
			}

			SliderY += delta;

			if( imgui_FileList( SliderX, SliderY, 360, TEX_FILE_LIST_HEIGHT, STATICSKY_PATH "*.dds", SkyGlowTexName, &sky_glow_tex_offset ) )
			{
				char SS1[ MAX_PATH ];

				sprintf( SS1, STATICSKY_PATH"%s", SkyGlowTexName );

				skyGlowTexNames[ index ] = SS1;

				texesChanged = true;			
			}

			SliderY += TEX_FILE_LIST_HEIGHT;

			if( texesChanged )
			{
				r3dGameLevel::Environment->SetStaticSkyTextures( skyTexNames, skyGlowTexNames );
			}

			SliderY += imgui_Static( SliderX, SliderY, "", 360);
		}
	}

	return SliderY;
}

//------------------------------------------------------------------------

float Editor_Level::ProcessSkySun( float SliderX, float SliderY )
{
	extern int g_DrawSunPath;
	g_DrawSunPath = 1;

	g_bSkyDomeNeedFullUpdate = true;

	static int HoffmanParamID = 0;

	const static char* list2[] =	{ "HG g"	, "Inscatter M"		, "Ray M"	, "Mie M"	, "Sun Int"	, "Turbidity"	, "Static Sky"	};
	enum							{ HG_g		, Inscatter_M		, Ray_M		, Mie_M		, Sun_Int	, Turbidity		, Static_Sky	};

	imgui_Toolbar(5, 80, 770, 35, &HoffmanParamID, 0, list2, R3D_ARRAYSIZE( list2 ), false );

	g_pDesktopManager->Begin( "ed_env" );

	SliderY += imgui_Value_Slider(SliderX, SliderY, "ParticlesShading Coeff", &r3dGameLevel::Environment->ParticleShadingCoef , 0, 1, "%-02.2f" );

	r3dGameLevel::Environment->SunLightOn = !!r3dGameLevel::Environment->SunLightOn ;
	SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Sun Light", &r3dGameLevel::Environment->SunLightOn, 1 ) ;

	if( HoffmanParamID != Static_Sky )
	{
		static float bloom_coef = 0.f ;

		SliderY += imgui_Static( SliderX, SliderY, "SUN LIGHT PARAMETERS", 360);

		float dayT = EnvGetDayT();

		float lastDirectionAngle = r3dGameLevel::Environment->SunDirectionAngle;
		float lastSunElevationAngle = r3dGameLevel::Environment->SunElevationAngle;

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Sun Direction", &r3dGameLevel::Environment->SunDirectionAngle, -180, 180, "%-02.0f" );
		SliderY += imgui_Value_Slider(SliderX, SliderY, "North<->South", &r3dGameLevel::Environment->SunElevationAngle, -90, 90, "%-02.0f" );

		if( lastDirectionAngle != r3dGameLevel::Environment->SunDirectionAngle 
				||
			lastSunElevationAngle != r3dGameLevel::Environment->SunElevationAngle )
		{
			ResetShadowCache();
		}

		SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Sun Color", &r3dGameLevel::Environment->SunColor, 360, dayT );

		SliderY += imgui_Value_Slider( SliderX, SliderY, "Sun Intensity", &r3dGameLevel::Environment->SunIntensity, 1.0f, 16.0f, "%-02.2f" );

		SliderY += imgui_Static( SliderX, SliderY, "BACKLIGHTING PARAMETERS", 360);

		SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Backlight color", &r3dGameLevel::Environment->BacklightColor, 360, dayT );
		SliderY += imgui_Value_Slider( SliderX, SliderY, "Backlight Power", &r3dGameLevel::Environment->BacklightIntensity, 0.0f, 1.0f, "%-02.2f" );

		SliderY += imgui_Static( SliderX, SliderY, "PROCEDURAL SKYDOME", 360);
		SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Skydome color", &r3dGameLevel::Environment->SunAmbientColor, 360, dayT );
		SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Lambda", &r3dGameLevel::Environment->LambdaCol, 360, dayT);
	}

	extern float fLambda[3];

	r3dColor LCol = r3dGameLevel::Environment->GetCurrentLambdaColor();

	fLambda[0] = float(LCol.R) / 256.0f;
	fLambda[1] = float(LCol.G) / 256.0f;
	fLambda[2] = float(LCol.B) / 256.0f;

	SliderY += 10;

	float dayT = EnvGetDayT();

	if (HoffmanParamID == HG_g )		SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "HG g",			&r3dGameLevel::Environment->HGg,					360, 300, 0.9f,0.999f, 10, 10, 2, 2, dayT ); 
	if (HoffmanParamID == Inscatter_M )	SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Inscatter M",		&r3dGameLevel::Environment->InscatteringMultiplier,360, 300, 0.0f,0.5f, 10, 10, 2, 2, dayT); 
	if (HoffmanParamID == Ray_M )		SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Ray",				&r3dGameLevel::Environment->BetaRayMultiplier,		360, 300, 0.0f,20.0f, 10, 10, 2, 2, dayT);
	if (HoffmanParamID == Mie_M )		SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Mie",				&r3dGameLevel::Environment->BetaMieMultiplier,		360, 300, 0.0f,0.05f, 10, 10, 2, 2, dayT);
	if (HoffmanParamID == Sun_Int )		SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Sun Int",			&r3dGameLevel::Environment->SunIntensityCoef,			360, 300, 0.0f,3.0f, 10, 10, 2, 2, dayT);
	if (HoffmanParamID == Turbidity )	SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Turbidity",		&r3dGameLevel::Environment->Turbitity,				360, 300, 0.0f,20.0f, 10, 10, 2, 2, dayT); 
	if (HoffmanParamID == Static_Sky )
	{
		r3dGameLevel::Environment->bStaticSkyEnable = !!r3dGameLevel::Environment->bStaticSkyEnable ;

		int StaticSkyEnable = r3dGameLevel::Environment->bStaticSkyEnable ;

		SliderY += imgui_Checkbox( SliderX, SliderY, 360, 22, "Enable Static Sky", &StaticSkyEnable, 1 ) ;

		if( r3dGameLevel::Environment->bStaticSkyEnable ^ StaticSkyEnable )
		{
			if( StaticSkyEnable )
				r3dGameLevel::Environment->EnableStaticSky();
			else
				r3dGameLevel::Environment->DisableStaticSky();
		}

		SliderY = ProcessStaticSky( SliderX, SliderY );
	}

	g_pDesktopManager->End();

	return SliderY;
}

//------------------------------------------------------------------------

static void FixPresetExt( char* preset )
{
	char drive[ 16 ], folder[ MAX_PATH ], name[ MAX_PATH ], ext[ MAX_PATH ];

	_splitpath( preset, drive, folder, name, ext );

	if( stricmp( ext, ".xml" ) )
	{
		strcpy( preset, drive );
		strcat( preset, folder );
		strcat( preset, name );
		strcat( preset, ext );
		strcat( preset, ".xml" );
	}
}

float Editor_Level::ProcessLightSetup( float SliderX, float SliderY )
{
	const static char* tablist[]	=	{ "Presets"	, "Sun Setup"	,	"Moon Setup",	"Sky Setup"	};
	enum								{ PRESETS	, SUN_SETUP		,	MOON_SETUP,		SKY_SETUP	};

	static int SelectedTab;

	imgui_Toolbar( 5, 80, 770, 35, &SelectedTab, 0, tablist, R3D_ARRAYSIZE( tablist ), false );

	g_pDesktopManager->Begin( "ed_env" );

	float dayT = EnvGetDayT();
	float nightT = EnvGetNightT();

	if( SelectedTab != PRESETS )
	{
		SliderY += imgui_Static(SliderX, SliderY, "Shadowing of particle systems" );
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Value", &r3dGameLevel::Environment->ParticleShadingCoef , 0, 1, "%-02.2f" );
	}

	switch( SelectedTab )
	{
	case PRESETS:
		{
			static char PresetName[ 512 ];
			static float offset = 0.f;

			static bool listValueChanged = false;

			int needReload = 0;

			const float FL_HEIGHT = 200.f;

#define R3D_ENV_PRESET_PATH "Data\\EnvPresets\\"

			if( imgui_FileList( SliderX, SliderY, 360, FL_HEIGHT, R3D_ENV_PRESET_PATH"*.xml", PresetName, &offset, true, listValueChanged ) )
			{
				needReload = 1;
			}

			SliderY += FL_HEIGHT;

			listValueChanged = false;

			if( imgui2_StringValueEx( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, "Preset Name: ", PresetName ) )
			{
				listValueChanged = true;
			}

			SliderY += 22.f;

			const float BTN_HEIGHT = 22.f;

			if( imgui_Button( SliderX, SliderY, 180, BTN_HEIGHT, "Load" ) )
			{
				char fullPath[ MAX_PATH * 2 ];
				sprintf( fullPath, R3D_ENV_PRESET_PATH"%s", PresetName );
				LoadEnvironmentFromXML( fullPath );
			}

			if( imgui_Button( SliderX + 180, SliderY, 180, BTN_HEIGHT, "Save" ) )
			{
				char fullPath[ MAX_PATH * 2 ];
				FixPresetExt( PresetName );
				sprintf( fullPath, R3D_ENV_PRESET_PATH"%s", PresetName );
				mkdir( R3D_ENV_PRESET_PATH );
				SaveEnvironmentToXML( fullPath );
			}

			SliderY += BTN_HEIGHT + 1;
		}
		break;

	case SUN_SETUP:
		{
			extern int g_DrawSunPath;
			g_DrawSunPath = 1;

			SliderY += imgui_Static(SliderX, SliderY, "SUN LIGHT PARAMETERS", 360);

			float lastDirectionAngle = r3dGameLevel::Environment->SunDirectionAngle;
			float lastSunElevationAngle = r3dGameLevel::Environment->SunElevationAngle;

			SliderY += imgui_Value_Slider(SliderX, SliderY, "Sun Direction", &r3dGameLevel::Environment->SunDirectionAngle, -180, 180, "%-02.0f");
			SliderY += imgui_Value_Slider(SliderX, SliderY, "North<->South", &r3dGameLevel::Environment->SunElevationAngle, -90, 90, "%-02.0f");
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Sun Angle Range", &r3dGameLevel::Environment->SunMovementAngleRange, 160.f, 220.f, "%.1f" );

			if( lastDirectionAngle != r3dGameLevel::Environment->SunDirectionAngle 
				||
			lastSunElevationAngle != r3dGameLevel::Environment->SunElevationAngle )
			{
				ResetShadowCache();
			}

			SliderY += imgui_Static( SliderX, SliderY, "Sun Color" );
			SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Sun Light", &r3dGameLevel::Environment->SunColor, 360, dayT );
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Sun Color multiplier", &r3dGameLevel::Environment->SunIntensity, 1.0f, 16.0f, "%-02.2f");		

			if( imgui_Button( SliderX, SliderY, 220.f, 22.f, "Sync Sun Light with Night Settings" ) )
			{
				memcpy( r3dGameLevel::Environment->SunColor.Values[ 0 ].val, 
						r3dGameLevel::Environment->MoonLightColor.Values[ r3dGameLevel::Environment->MoonLightColor.NumValues - 1 ].val,
						sizeof r3dTimeGradient2::val_s().val );
				memcpy( r3dGameLevel::Environment->SunColor.Values[ r3dGameLevel::Environment->SunColor.NumValues - 1 ].val,
						r3dGameLevel::Environment->MoonLightColor.Values[ 0 ].val,
						sizeof r3dTimeGradient2::val_s().val );
			}
			SliderY += 30.f;

			SliderY += imgui_Static( SliderX, SliderY, "Sky Color - Ambient");
			SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Sky Light - Ambient", &r3dGameLevel::Environment->SunAmbientColor, 360, dayT);
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Sky Color multiplier", &r3dGameLevel::Environment->SkyIntensity, 1.0f, 4.0f, "%-02.2f");		

			if( imgui_Button( SliderX, SliderY, 220.f, 22.f, "Sync Sky Light with Night Settings" ) )
			{
				memcpy(	r3dGameLevel::Environment->SunAmbientColor.Values[ 0 ].val, 
						r3dGameLevel::Environment->MoonAmbientColor.Values[ r3dGameLevel::Environment->MoonAmbientColor.NumValues - 1 ].val,
						sizeof r3dTimeGradient2::val_s().val );

				memcpy(	r3dGameLevel::Environment->SunAmbientColor.Values[ r3dGameLevel::Environment->SunAmbientColor.NumValues - 1 ].val, 
						r3dGameLevel::Environment->MoonAmbientColor.Values[ 0 ].val,
						sizeof r3dTimeGradient2::val_s().val );
			}

			SliderY += 30.f;

			SliderY += imgui_Static( SliderX, SliderY, "GROUND REFLECTION PARAMETERS");

			SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Ground Color", &r3dGameLevel::Environment->BacklightColor, 360, dayT);
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Reflection Intensity", &r3dGameLevel::Environment->BacklightIntensity, 0.0f, 1.0f, "%-02.2f" );
			SliderY += 30.f;

			SliderY += imgui_Static( SliderX, SliderY, "PROCEDURAL SUN RENDERING" );
			char lamdaformecmp[ sizeof r3dGameLevel::Environment->LambdaCol ];
			memcpy( lamdaformecmp, &r3dGameLevel::Environment->LambdaCol, sizeof lamdaformecmp );
			SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Sun Color", &r3dGameLevel::Environment->LambdaCol, 360, dayT);
			if( memcmp( lamdaformecmp, &r3dGameLevel::Environment->LambdaCol, sizeof lamdaformecmp ) )
			{
				g_bSkyDomeNeedFullUpdate = 1;
			}

			SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Size",		&r3dGameLevel::Environment->InscatteringMultiplier,	360, 300, 0.01f,0.06f, 10, 10, 2, 2, dayT); 
			SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Intensity",	&r3dGameLevel::Environment->SunIntensityCoef,		360, 300, 1.0f,8.0f, 10, 10, 2, 2, dayT);

			//SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "HG g",			&r3dGameLevel::Environment->HGg,						360, 300, 0.90f,0.999f, 10, 10, 2, 2, dayT); 
			//SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Inscatter M",		&r3dGameLevel::Environment->InscatteringMultiplier,	360, 300, 0.0f,0.5f, 10, 10, 2, 2, dayT); 
			//SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Ray",				&r3dGameLevel::Environment->BetaRayMultiplier,		360, 300, 0.0f,20.0f, 10, 10, 2, 2, dayT);
			//SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Mie",				&r3dGameLevel::Environment->BetaMieMultiplier,		360, 300, 0.0f,0.05f, 10, 10, 2, 2, dayT);
			//SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Sun Int",			&r3dGameLevel::Environment->SunIntensityCoef,		360, 300, 0.0f,3.0f, 10, 10, 2, 2, dayT);
			//SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Turbidity",		&r3dGameLevel::Environment->Turbitity,				360, 300, 0.0f,20.0f, 10, 10, 2, 2, dayT); 
#if 0
			SliderY += imgui_Static( SliderX, SliderY, "Sun Spot Color");
			SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Sun Spot Color", &r3dGameLevel::Environment->SunSpotColor, 36, dayT0);
			
			SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Sun Spot Size",	&r3dGameLevel::Environment->SunSpotAmplify, 360, 300, 10.0f, 16.0f );
			SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Sun Spot Falloff",	&r3dGameLevel::Environment->SunSpotPow, 360, 300, 0.25f, 256.0f );
			SliderY += imgui_DrawFloatGradient(SliderX, SliderY, "Sun Spot Intensity",	&r3dGameLevel::Environment->SunSpotIntensity, 360, 300, 0.25f, 16.0f );
#endif
		}
		break;

	case MOON_SETUP:
		{
			g_DrawMoonPath = 1;

			int needReset = 0;

			float prevVal = r3dGameLevel::Environment->MoonLongitude;
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Moon Longitude", &r3dGameLevel::Environment->MoonLongitude, -180, +180, "%-02.0f");
			if( prevVal != r3dGameLevel::Environment->MoonLongitude )
				needReset = 1;

			prevVal = r3dGameLevel::Environment->MoonLatitude;
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Moon Latitude", &r3dGameLevel::Environment->MoonLatitude, 0, 90, "%-02.0f");
			if( prevVal != r3dGameLevel::Environment->MoonLatitude )
				needReset = 1;

			SliderY += imgui_Static( SliderX, SliderY, "Moon Light Color" );
			SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Moon Light Color", &r3dGameLevel::Environment->MoonLightColor, 360, nightT);
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Moon Light multiplier", &r3dGameLevel::Environment->MoonIntensity, 0.0f, 16.0f, "%-02.2f");
			
			SliderY += imgui_Static( SliderX, SliderY, "Moon Sky Light Color - ambient" );
			SliderY += imgui_DrawColorGradient(SliderX, SliderY, "Moon Ambient Color", &r3dGameLevel::Environment->MoonAmbientColor, 360, nightT);

			if( needReset )
			{
				ResetShadowCache();
			}
		}
		break;

	case SKY_SETUP:
		{
			SliderY = ProcessStaticSky( SliderX, SliderY );
		}
		break;
	}

	g_pDesktopManager->End();

	return SliderY;
}

//------------------------------------------------------------------------

void hotkill_remove( const char* file )
{
	if( r3dFileExists( file ))
	{
		if( remove( file ) )
		{
			r3dArtBug( "Couldn't delete '%s' during hotkill!\n", file );
		}
	}
}

void KillLonelyTexture( r3dTexture* tex, r3dMesh* orgMesh, r3dTL::TArray< r3dString > * killedList )
{
	if( !tex )
		return;

	if( tex->IsMissing() )
		return;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		r3dMesh* mesh = iter.current->GetObjectMesh();

		if( !mesh )
			continue;

		if( mesh != orgMesh )
		{
			for( int i = 0, e = mesh->NumMatChunks; i < e; i ++ )
			{
				r3dMaterial* mat = mesh->MatChunks[ i ].Mat;

				if( mat->NormalTexture == tex )
					return;

				if( mat->DecalMaskTexture == tex )
					return;

				if( mat->GlossTexture == tex )
					return;

				if( mat->EnvPowerTexture == tex )
					return;

				if( mat->SelfIllumTexture == tex )
					return;

				if( mat->ThicknessTexture == tex )
					return;

				if( mat->CamouflageMask == tex )
					return;

				if( mat->DistortionTexture == tex )
					return;

				if( mat->NormalDetailTexture == tex )
					return;

				if( mat->SpecularPowTexture == tex )
					return;
			}
		}
	}

	hotkill_remove( tex->getFileLoc().FileName );

	if( killedList )
	{
		killedList->PushBack( tex->getFileLoc().FileName );
	}

	return;
}

void KillLonelyMaterial( r3dMaterial* material, r3dMesh* orgMesh, r3dTL::TArray< r3dString > * killedList )
{
	if( !material )
		return;

	if( material == r3dMaterialLibrary::GetDefaultMaterial() )
		return;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		r3dMesh* mesh = iter.current->GetObjectMesh();

		if( !mesh )
			continue;

		if( mesh != orgMesh )
		{
			for( int i = 0, e = mesh->NumMatChunks; i < e; i ++ )
			{
				r3dMaterial* mat = mesh->MatChunks[ i ].Mat;

				if( mat == material )
					return;
			}
		}
	}

	r3dString toDelete;
	
	toDelete += "Data/ObjectsDepot/";
	toDelete += material->DepotName;

	toDelete += "/Materials/";
	toDelete += material->Name;

	toDelete += ".mat";

	hotkill_remove( toDelete.c_str() );

	if( killedList )
	{
		killedList->PushBack( material->Name );
	}
}

typedef std::set<r3dTexture*> TextureSet;
typedef std::set<r3dMaterial*> MaterialSet;

void FillTextureSet( r3dMesh* mesh, TextureSet * textureSet )
{
	textureSet->clear();

	for( int i = 0, e = (int)mesh->NumMatChunks; i < e; i ++ )
	{
		r3dMaterial* mat = mesh->MatChunks[ i ].Mat;

		textureSet->insert( mat->NormalTexture );
		textureSet->insert( mat->DecalMaskTexture );
		textureSet->insert( mat->GlossTexture );
		textureSet->insert( mat->EnvPowerTexture );
		textureSet->insert( mat->SelfIllumTexture );
		textureSet->insert( mat->ThicknessTexture );
		textureSet->insert( mat->CamouflageMask );
		textureSet->insert( mat->DistortionTexture );
		textureSet->insert( mat->NormalDetailTexture );
		textureSet->insert( mat->SpecularPowTexture );
	}
}

void FillMaterialSet( r3dMesh* mesh, MaterialSet * materialSet )
{
	materialSet->clear();

	for( int i = 0, e = (int)mesh->NumMatChunks; i < e; i ++ )
	{
		r3dMaterial* mat = mesh->MatChunks[ i ].Mat;
		materialSet->insert( mat );
	}
}

void Editor_Level::ProcessKill()
{
	r3dTL::TArray< GameObject* > pickedObjects;

	g_Manipulator3d.GetPickedObjects( &pickedObjects );
	g_Manipulator3d.PickerResetPicked();

	r3dTL::TArray< r3dString > killedMaterials;
	r3dTL::TArray< r3dString > killedTextures;
	r3dTL::TArray< r3dString > killedMeshes;

	for( int i = 0, e = (int)pickedObjects.Count(); i < e ; i ++ )
	{
		GameObject* obj = pickedObjects[ i ];

		if( !( obj->ObjTypeFlags & OBJTYPE_Mesh ) )
			continue;

		if( r3dMesh* mesh = obj->GetObjectMesh() )
		{
			char fileName[ 1024 ];
			strcpy( fileName, mesh->FileName.c_str() );

			killedMeshes.PushBack( fileName );

			for( int j = i + 1; j < e; )
			{
				if( pickedObjects[ j ]->GetObjectMesh() == mesh )
				{
					pickedObjects.Erase( j, 1 );
					e --;
				}
				else
					j ++;
			}

			r3dTL::TArray< GameObject* > objectsToDelete;

			for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
			{
				if( iter.current->GetObjectMesh() == mesh && iter.current->ObjTypeFlags & OBJTYPE_Mesh )
				{
					objectsToDelete.PushBack( iter.current );
				}
			}

			for( int i = 0, e = objectsToDelete.Count(); i < e; i ++ )
			{
				GameWorld().DeleteObject( objectsToDelete[ i ] );
			}

			TextureSet texSet;

			FillTextureSet( mesh, &texSet );

			for( TextureSet::iterator	i = texSet.begin(), 
										e = texSet.end();
										i != e;
										++ i )
			{
				KillLonelyTexture( *i, mesh, &killedTextures );
			}

			MaterialSet mset;
			FillMaterialSet( mesh, &mset );

			for( MaterialSet::iterator		i = mset.begin(),
											e = mset.end();
											i != e;
											++ i )
			{
				KillLonelyMaterial( *i, mesh, &killedMaterials );
			}

			hotkill_remove( fileName );

			size_t extStart = strlen( fileName ) - 3;

			strcpy( fileName + extStart, "scb" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "skl" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "wgt" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "mpx" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "phx" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart - 1, "_Collision.sco" );
			hotkill_remove( fileName );

			extStart = strlen( fileName ) - 3;

			strcpy( fileName + extStart, "scb" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "skl" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "wgt" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "mpx" );
			hotkill_remove( fileName );

			strcpy( fileName + extStart, "phx" );
			hotkill_remove( fileName );
		}
	}

	if( killedMeshes.Count() )
	{
		char* message( NULL );

		int totalSize = 512;

		for( int i = 0, e = (int)killedMeshes.Count(); i < e; i ++ )
		{
			totalSize += killedMeshes[ i ].Length() + 2;
		}

		for( int i = 0, e = (int)killedMaterials.Count(); i < e; i ++ )
		{
			totalSize += killedMaterials[ i ].Length() + 2;
		}

		for( int i = 0, e = (int)killedTextures.Count(); i < e; i ++ )
		{
			totalSize += killedTextures[ i ].Length() + 2;
		}

		message = (char*)malloc( totalSize );

		sprintf( message, "Deleted meshes(%d): ", killedMeshes.Count() );

		int ADJ_SIZE = sizeof message - 8;

		for( int i = 0, e = killedMeshes.Count(); i < e; i ++ )
		{
			strcat( message, killedMeshes[ i ].c_str() );

			if( i != e - 1 )
			{
				strcat( message, "," );
			}
		}

		char submsg[ 64 ];
		sprintf( submsg, "\nDeleted materials(%d): ", killedMaterials.Count() );

		strcat( message, submsg);

		for( int i = 0, e = killedMaterials.Count(); i < e; i ++ )
		{
			strcat( message, killedMaterials[ i ].c_str() );

			if( i != e - 1 )
			{
				strcat( message, "," );
			}
		}

		sprintf( submsg, "\nDeleted textures(%d): ", killedTextures.Count() );
		strcat( message, submsg);

		for( int i = 0, e = killedTextures.Count(); i < e; i ++ )
		{
			strcat( message, killedTextures[ i ].c_str() );

			if( i != e - 1 )
			{
				strcat( message, "," );
			}
		}

		if( strlen( message ) > 8192 )
		{
			strcpy( message + 8192 - 8, " ..." );
		}

		MessageBoxA( r3dRenderer->HLibWin, message, "Information", MB_OK );

		free( message );
	}
}

//------------------------------------------------------------------------

r3dString ReportUsedTexture( r3dTexture* tex, r3dMesh* parentMesh )
{
	r3dString res;

	bool first = true;

	std::set< r3dMesh* > meshesReported;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		r3dMesh* mesh = iter.current->GetObjectMesh();

		if( !mesh || mesh == parentMesh )
			continue;

		for( int i = 0, e = (int)mesh->NumMatChunks; i < e; i ++ )
		{
			r3dMaterial* mat = mesh->MatChunks[ i ].Mat;

			if( mat->NormalTexture == tex ||
				mat->DecalMaskTexture == tex ||
				mat->GlossTexture == tex ||
				mat->EnvPowerTexture == tex ||
				mat->SelfIllumTexture == tex ||
				mat->ThicknessTexture == tex ||
				mat->CamouflageMask == tex ||
				mat->DistortionTexture == tex ||
				mat->NormalDetailTexture == tex ||
				mat->SpecularPowTexture == tex )
			{
				if( meshesReported.find( mesh ) == meshesReported.end() )
				{
					meshesReported.insert( mesh );

					res += r3dString( first ? "" : "," ) + mesh->FileName;
					first = false;
				}
			}
		}
	}

	return res;
}

r3dString ReportUsedMaterial( r3dMaterial* mat, r3dMesh* parentMesh )
{
	r3dString res;

	bool first = true;

	std::set< r3dMesh* > meshesReported;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		r3dMesh* mesh = iter.current->GetObjectMesh();

		if( !mesh || mesh == parentMesh )
			continue;

		for( int i = 0, e = (int)mesh->NumMatChunks; i < e; i ++ )
		{
			if( mat == mesh->MatChunks[ i ].Mat )
			{
				if( meshesReported.find( mesh ) == meshesReported.end() )
				{
					meshesReported.insert( mesh );

					res += r3dString( first ? "" : "," ) + mesh->FileName;
					first = false;
				}
			}
		}
	}

	return res;
}

void Editor_Level::ProcessReveal()
{
	r3dTL::TArray< GameObject* > pickedObjects;
	g_Manipulator3d.GetPickedObjects( &pickedObjects );

	TextureSet textureSet;
	MaterialSet materialSet;

	r3dString message;

	if( pickedObjects.Count() == 1 )
	{
		GameObject* obj = pickedObjects[ 0 ];

		if( r3dMesh* mesh = obj->GetObjectMesh() )
		{
			FillTextureSet( mesh, &textureSet );

			for( TextureSet::iterator i = textureSet.begin(), e = textureSet.end(); i != e; ++ i )
			{
				if( !(*i) )
					continue;

				r3dString usedIn = ReportUsedTexture( *i, mesh );

				if( usedIn.Length() > 0 )
				{
					message += "Texture '"  + r3dString( (*i)->getFileLoc().FileName ) + "' is also used in: " + usedIn;
					message += "\n";
				}
				else
				{
					message += "Texture '"  + r3dString( (*i)->getFileLoc().FileName ) + "' is unique!";
					message += "\n";
				}
			}

			message += "\n";

			FillMaterialSet( mesh, &materialSet );

			for( MaterialSet::iterator i = materialSet.begin(), e = materialSet.end(); i != e; ++ i )
			{
				if( !(*i) )
					continue;

				r3dString usedIn = ReportUsedMaterial( *i, mesh );

				if( usedIn.Length() > 0 )
				{
					message += "Material '"  + r3dString( (*i)->Name ) + "' is also used in: " + usedIn;
					message += "\n";
				}
				else
				{
					message += "Material '"  + r3dString( (*i)->Name ) + "' is unique!";
					message += "\n";
				}
			}
		}

		if( message.Length() > 8192 )
		{
			strcpy( (char*)message.c_str() + 8192 - 8, " ..." );
		}

		if( message.Length() > 3 )
		{
			MessageBox( r3dRenderer->HLibWin, message.c_str(), "Info", MB_OK );
		}
		else
		{
			MessageBox( r3dRenderer->HLibWin, "All resources are unique!", "Info", MB_OK );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void ProcessZombieMod()
{
	obj_Player* plr = 0;
	for(GameObject *obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if( obj->isObjType(OBJTYPE_Human) )
		{
			plr = static_cast< obj_Player* >( obj );
			break;
		}
	}

	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 85;

	if (!plr)
	{
		SliderY += imgui_Static(SliderX, SliderY, "Spawn player first");
		return;
	}

	PlayerLifeProps & plf = plr->lifeProperties;

	SliderY += imgui_Static(SliderX, SliderY, "Player life properties:");
	SliderY += 5;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Stand visibility", &plf.standStillVisiblity, 0, 100.0f, "%-02.02f");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Walk visibility", &plf.walkVisibility, 0, 100.0f, "%-02.02f");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Run visibility", &plf.runVisibility, 0, 100.0f, "%-02.02f");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Sprint visibility", &plf.sprintVisibility, 0, 100.0f, "%-02.02f");

	SliderY += 5;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Walk noise", &plf.walkNoise, 0, 100.0f, "%-02.02f");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Run noise", &plf.runNoise, 0, 100.0f, "%-02.02f");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Sprint noise", &plf.sprintNoise, 0, 100.0f, "%-02.02f");
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Shoot noise", &plf.fireNoise, 0, 100.0f, "%-02.02f");

	SliderY += 5;
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Smell", &plf.smell, 0, 100.0f, "%-02.02f");

	SliderY += 5;
	static int v1 = 0;
	SliderY += imgui_Checkbox(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Debug visualizer", &v1, 1);
	v1 ? g_ZombieModDebugVisFlags |= 1 : g_ZombieModDebugVisFlags &= ~1;
	if (g_ZombieModDebugVisFlags & 1)
	{
		SliderY += imgui_Static(SliderX, SliderY, "Blue – visibility, green – sound, yellow – smell");
	}
}

//////////////////////////////////////////////////////////////////////////

template < int N > 
float DrawMaterialTypeSelection( float SliderX, float SliderY, char (&TypeName)[ N ] )
{
	SliderY += imgui_Static( SliderX, SliderY, "Material Type" );

	SliderY += 2.f;

	const stringlist_t& items = g_pMaterialTypes->GetTypeNames();

	static int SelectedType = -1;
	static float SelectedOffset = 0.f;
	bool foundMat = false;
	for( int i = 0, e = (size2int_t)items.size(); i < e; i ++ )
	{
		if ( items[ i ] == TypeName )
		{
			SelectedType = i;
			foundMat = true;
			break;
		}
	}
	if(!foundMat)
	{
		SelectedType = -1;
	}

	const float HEIGHT = 180.f ;

	imgui_DrawList( SliderX, SliderY, 360.f, HEIGHT, items, &SelectedOffset, &SelectedType );

	SliderY += HEIGHT ;

	if( SelectedType >= 0 && SelectedType < (int)items.size() )
	{
		if( items[ SelectedType ].length() >= (sizeof TypeName - 1) ) 
		{
			r3dError( "Material type name '%s' is too long!", items[ SelectedType ].c_str() );
		}

		r3dscpy( TypeName, items[ SelectedType ].c_str() );
	}

	return SliderY;
}

//------------------------------------------------------------------------

template float DrawMaterialTypeSelection< 32 >( float SliderX, float SliderY, char (&TypeName)[ 32 ] ) ;
template float DrawMaterialTypeSelection< 2048 >( float SliderX, float SliderY, char (&TypeName)[ 2048 ] ) ;


//////////////////////////////////////////////////////////////////////////

ValueTracker<float> metalnessTracker;
ValueTracker<float> specularTracker;
ValueTracker<float> chromeTracker;
ValueTracker<float> detailScaleTracker;
ValueTracker<float> detailAmmTracker;
ValueTracker<int> flagsTracker;
ValueTracker<r3dColor> colorTracker;
ValueTracker<int> displacementTracker;
ValueTracker<float> displacementDepthTracker;

//////////////////////////////////////////////////////////////////////////

void InitValueTrackers()
{
	if (!UI_TargetMaterial)
		return;

	metalnessTracker.Init(UI_TargetMaterial->SpecularPower);
	specularTracker.Init(UI_TargetMaterial->SpecularPower1);
	chromeTracker.Init(UI_TargetMaterial->ReflectionPower);
	detailScaleTracker.Init(UI_TargetMaterial->DetailScale);
	detailAmmTracker.Init(UI_TargetMaterial->DetailAmmount);

	flagsTracker.Init(UI_TargetMaterial->Flags);
	colorTracker.Init(UI_TargetMaterial->DiffuseColor);
	displacementTracker.Init(UI_TargetMaterial->m_DoDispl);
	displacementDepthTracker.Init(UI_TargetMaterial->m_DisplDepthVal);

}

//////////////////////////////////////////////////////////////////////////

void Editor_Level :: ProcessMaterials()
{
	float SliderX = r3dRenderer->ScreenW-375;
	float SliderY = 45;

	if( imgui_Button( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Save level materials" ) )
	{
		r3dMaterialLibrary::UpdateDepotMaterials();
		MessageBoxA( r3dRenderer->HLibWin, "Saved!", "Message", MB_OK );
	}

	SliderY += 33.f;

	GameObject* obj = GameWorld().GetObject(UI_TargetObjID);
	if(obj)
	{
		if
		(
			obj->Class->Name == "obj_Building" ||
			obj->Class->Name == "obj_Road" ||
			obj->Class->Name == "obj_LightMesh" ||
			obj->Class->Name == "obj_ApexDestructible" ||
			obj->Class->Name == "obj_Sprite" ||
			obj->Class->Name == "CollectionElementObjectProxy"
		)
		{
			if (UI_TargetMaterial)
			{
				Font_Label->PrintF(20,90,r3dColor(255,255,255), "MAT: %s [%s]", UI_TargetMaterial->Name, UI_TargetMaterial->DepotName);
			}

			if (Keyboard->IsPressed(kbsLeftControl)) 
			{
				if (EditMaterial != UI_TargetMaterial && UI_TargetMaterial)
				{
					InitValueTrackers();
				}

				EditMaterial = UI_TargetMaterial;
				if(EditMaterial) EditMaterial->WasEdited = 1;
			}
		}
	}
	else
	{
		if (Keyboard->IsPressed(kbsLeftControl)) 
		{
			if (EditMaterial != UI_TargetMaterial && UI_TargetMaterial)
			{
				InitValueTrackers();
			}

			EditMaterial = UI_TargetMaterial;
			if(EditMaterial) EditMaterial->WasEdited = 1;
		}
	} 	

	if(EditMaterial == 0) // show material list
	{
		static stringlist_t matNames;
		static int curSelection = 0;
		static int numMatsInLibrary = 0;
		static float arrOffset = 0;

		r3dMaterial** ppMatLibrary = 0;
		int numMats = r3dMaterialLibrary::GetCurrent(ppMatLibrary);
		if(numMats != numMatsInLibrary)
		{
			curSelection = 0;
			arrOffset = 0;
			numMatsInLibrary = numMats;
			matNames.clear();
			for(int i=0; i<numMatsInLibrary; ++i)
			{
				matNames.push_back(ppMatLibrary[i]->Name);
			}
		}
		imgui_Static(5, 45, "Material List:", 300);
		if(imgui_DrawList(5, 70, 300, 450, matNames, &arrOffset, &curSelection, true, false, true))
			EditMaterial = ppMatLibrary[curSelection];
	}

//	int selectMatID = -1
	static int showExtendedMaterialsEditor = 0;
	if (EditMaterial)	
	{

		g_pDesktopManager->Begin( "ed_materials" );

		char fullname[256];
		sprintf(fullname, "%s [%s]", EditMaterial->Name, EditMaterial->DepotName);
		SliderY += imgui_Static(SliderX, SliderY, fullname);
		SliderY += 10;

		static const char* TNames[] = {"DIFFUSE MAP", "NORMAL MAP", "METALNESS MAP", "ROUGHNESS MAP", "SELF ILLUMINATION MAP", "DETAIL MAP", "DENSITY MAP", "CAMOUFLAGE MASK", "DISTORTION MAP", "SPECULAR POW MAP", "DECAL MASK MAP" };
		COMPILE_ASSERT( _countof(TNames) == r3dMaterial::TEX_COUNT );
		r3dTexture *MatTexArray[ r3dMaterial::TEX_COUNT ];
		int texSkipList[_countof(TNames)] = { 0 };

		EditMaterial->GetTextures( MatTexArray );

		static int i= INVALID_INDEX;

		SliderY += imgui_Value_Slider(SliderX, SliderY, "Metalness", &EditMaterial->SpecularPower,	0,1,	"%-02.02f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Specular Power", &EditMaterial->SpecularPower1,	0,1,	"%-02.02f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Chrome", &EditMaterial->ReflectionPower,	0,1,	"%-02.02f",1);
		SliderY += 5;
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Detail Scale", &EditMaterial->DetailScale,	0.1f,30,	"%-02.02f",1);
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Detail Amount", &EditMaterial->DetailAmmount,	0.0f,1.0f,	"%-02.02f",1);		

		if (r_show_winter->GetBool())
		{
			float snowAmount = EditMaterial->SnowAmount;
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Snow amount", &snowAmount, 0, 1, "%-1.03f");		// Material type selection
			if (snowAmount != EditMaterial->SnowAmount)
				EditMaterial->SnowAmount = snowAmount;
		}


		float matID = EditMaterial->matID;
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Shared Material", &matID,	0, r3dMaterialLibrary::GetExtendedMaterialDataCount(),	"%-02.02f",1);
		EditMaterial->matID = matID;

		char matIdName[256];
		sprintf_s(matIdName, "%s (%d)", r3dMaterialLibrary::GetExtendedMaterialName(EditMaterial->matID).c_str(), EditMaterial->matID);
		if (imgui_Button(SliderX + 200, SliderY, 160, 25, "EDIT", showExtendedMaterialsEditor ))
		{
			showExtendedMaterialsEditor=!showExtendedMaterialsEditor;

		}

		SliderY += imgui_Static(SliderX, SliderY, matIdName, 200);

		static int nTypeBtn = 0;
		static char * szButtonNames[] = {
			"Switch to Displacement",
			"Switch to Normal Maps"
		};
		
		nTypeBtn = EditMaterial->m_DoDispl ? 1 : 0;
		if ( imgui_Button(SliderX, SliderY, 360, 25, szButtonNames[ nTypeBtn ] ) )
		{
			nTypeBtn = !nTypeBtn;
			EditMaterial->m_DoDispl = ! EditMaterial->m_DoDispl;
		}
		SliderY += 26;

		if ( nTypeBtn )
		{
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Depth Scale", &EditMaterial->m_DisplDepthVal,	0,0.16f, "%-1.03f" );
		}

		extern void SetFlag(int& flags, int flag, bool invert);

		int DoubleSided = EditMaterial->Flags & R3D_MAT_DOUBLESIDED ? 1 : 0;
		SliderY += imgui_Checkbox( SliderX, SliderY, "Double Sided", &DoubleSided, 1 );
		SetFlag(EditMaterial->Flags, R3D_MAT_DOUBLESIDED,  DoubleSided == 0);

		int ForceTransparent = EditMaterial->Flags & R3D_MAT_FORCEHASALPHA ? 1 : 0;
		int OldForceTransparent = ForceTransparent;

		SliderY += imgui_Checkbox( SliderX, SliderY, "Force Transparent", &ForceTransparent, 1 );
		SetFlag(EditMaterial->Flags, R3D_MAT_FORCEHASALPHA,  ForceTransparent == 0);

		EditMaterial->SetAlphaFlag();

		if( ForceTransparent != OldForceTransparent )
		{
			r3dUpdateMeshMaterials();
		}

		int transparent = EditMaterial->Flags & R3D_MAT_TRANSPARENT ? 1 : 0;
		int oldTransparent = transparent;
		SliderY += imgui_Checkbox( SliderX, SliderY, "Alpha Transparent", &transparent, 1 );
		SetFlag(EditMaterial->Flags, R3D_MAT_TRANSPARENT, transparent == 0);
		if (oldTransparent != transparent)
			ModifyShadowsFlagForTransparentObjects();

		int camouflage = EditMaterial->Flags & R3D_MAT_CAMOUFLAGE ? 1 : 0;
		SliderY += imgui_Checkbox( SliderX, SliderY, "Camouflage", &camouflage, 1 );
		SetFlag(EditMaterial->Flags, R3D_MAT_CAMOUFLAGE, camouflage == 0);


		SliderY += imgui_DrawColorPicker(SliderX, SliderY, "Diffuse Color", &EditMaterial->DiffuseColor, 360, 0);
		//SliderY += imgui_DrawColorPicker(SliderX, SliderY, "Specular Color", &EditMaterial->SpecularColor, 360, 0);

		SliderY += 10;
		SliderY += imgui_Static(SliderX, SliderY, "SELECT MAP CHANNEL");
		bool bChangedVal = false;

		if ( i == INVALID_INDEX )
		{
			bChangedVal = true;
			i = 0;
		}

		if( PrevEditMaterial != EditMaterial )
		{
			r3dOutToLog("");
			PrevEditMaterial = EditMaterial;
			bChangedVal = true;
		}

		texSkipList[7] = camouflage ? 0 : 1;
		texSkipList[8] = EditMaterial->Flags & R3D_MAT_TRANSPARENT ? 0 : 1;

		// do not allow to set decal mask if 2nd uv isn't enabled
		extern int _r3d_Mesh_LoadSecondUV;
		texSkipList[r3dMaterial::TEX_DECALMASK] = _r3d_Mesh_LoadSecondUV == 0;

		for (int k = 0; k < _countof(TNames); k++)
		{
			if (texSkipList[k] == 1)
				continue;

			if(imgui_Button(SliderX, SliderY, 360, 25, TNames[k], (i==k)))
			{
				i = k;
				bChangedVal = true;
			}
			SliderY += 30;
		}

		SliderY += 10;
		SliderY += imgui_Static(SliderX, SliderY, TNames[i]);

		int deskX = g_pDesktopManager->GetActiveDesctop().GetX();
		int deskY = g_pDesktopManager->GetActiveDesctop().GetY();

		r3dDrawBox2D( SliderX - deskX, SliderY - deskY, 360, 120, imgui_bkgDlg );
		r3dDrawBox2D( SliderX + 10 - deskX, SliderY + 10 - deskY, 100, 100, r3dColor(255,255,255) );

		char MatFName[64];

		if( MatTexArray[i] )
		{
			char full[ 256 ], drive[ 8 ], file[ 128 ], ext[ 32 ];

			_splitpath( MatTexArray[i]->getFileLoc().FileName, drive, full, file, ext );

			sprintf( full, "%s%s", file, ext );

			imgui_Static(SliderX + 22 + 100			, SliderY + 88, full, 220 );
			r3dDrawBox2D(SliderX + 10 + 2 - deskX	, SliderY + 10 + 2 - deskY, 100-4, 100-4, r3dColor(255,255,255),  MatTexArray[i]);

			FixedString s( MatTexArray[i]->getFileLoc().FileName );
			FixedString sName = s.GetName() + s.GetExt();
			r3dscpy( MatFName, sName.c_str() );

		}
		else
		{
			// 4 is self illum map
			if( i && i != 4 ) r3dDrawBox2D( SliderX + 10 + 2 - deskX, SliderY + 10 + 2 - deskY, 100-4, 100-4, r3dColor(0,0,0));
			else
				r3dDrawBox2D(SliderX + 10 + 2 - deskX, SliderY + 10 + 2 - deskY, 100-4, 100-4, r3dColor(255,255,255));

			*MatFName = '\0';
		}

		if (imgui_Button(SliderX+10+2+106,SliderY+10+2, 180, 20, "CLEAR", 0))
		{
			r3dRenderer->DeleteTexture( MatTexArray[i] );
			MatTexArray[i] = NULL;
		}

		if (imgui_Button(SliderX+10+2+106,SliderY+42+2, 180, 20, "EDIT", 0))
		{
			if( MatTexArray[ i ] )
			{
#ifndef FINAL_BUILD
				const char* name = MatTexArray[i]->getFileLoc().FileName ;

				char fullPath[ 4096 ] ;
				GetCurrentDirectory( sizeof fullPath - 2, fullPath ) ;
				strcat( fullPath, "\\" ) ;
				strcat( fullPath, name ) ;

				for( size_t i = 0, e = strlen( fullPath) ; i < e ; i ++ )
				{
					if( fullPath[ i ] == '/' )
						fullPath[ i ] = '\\' ;
				}

				const char* execStr = g_texture_edit_cmd->GetString() ;

				ShellExecute( NULL, "open", execStr, fullPath, NULL, SW_SHOWNORMAL ) ;
#endif
			}
		}

		SliderY += 122.f;

		if( i == 4 )
			SliderY += imgui_Value_Slider( SliderX, SliderY, "Self Illumination", &EditMaterial->SelfIllumMultiplier, 0.f, 1.f, "%.2f" ) ;

		SliderY += 11.f;

		SliderY += imgui_Static(SliderX, SliderY, "Low Lighting Quality Params", 220 );
		float lowQSelfIllum = EditMaterial->lowQSelfIllum;
		float lowQMetallness = EditMaterial->lowQMetallness;
#if 0
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Self Illumination", &lowQSelfIllum, 0, 1, "%-1.03f");		// Material type selection
#endif
		SliderY += imgui_Value_Slider(SliderX, SliderY, "Metallness", &lowQMetallness, 0, 1, "%-1.03f");		// Material type selection

		
		if (lowQSelfIllum != EditMaterial->lowQSelfIllum || lowQMetallness != EditMaterial->lowQMetallness)
		{
			if (r_overall_quality->GetInt() != 1)
			{
				MessageBox(win::hWnd, "Set overall quality to low to modify this value and see the changes", "No editing allowed", MB_ICONEXCLAMATION);
			}
			else
			{
				EditMaterial->lowQSelfIllum = lowQSelfIllum;
				EditMaterial->lowQMetallness = lowQMetallness;
			}
		}

		SliderY += 12.0f;

		SliderY = DrawMaterialTypeSelection( SliderX, SliderY, EditMaterial->TypeName );

		if( imgui_Button( SliderX, SliderY, 180.f, 22.f, "Save To Lib" ) )
		{
			if( EditMaterial )
			{
				char MatFile[ MAX_PATH ];
				void MakeMaterialFileName( char* MatFile, const char* MatName, const char* DepotName );
				MakeMaterialFileName( MatFile, EditMaterial->Name, EditMaterial->DepotName );

				if( _access_s( MatFile, 0 ) == 0 )
				{
					FILE* file = fopen( MatFile, "wt" );

					EditMaterial->SaveAscii( file );

					fclose( file );
				}
				else
				{
					MessageBox( 0, "This material has no file - can't save", "Error", MB_OK );
				}
			}
		}

		SliderY += 22.f ;

		g_pDesktopManager->End();
		
		char DirName[512];
		sprintf(DirName,  "%s/*.*", EditMaterial->OriginalDir);

		imgui_Static(5, 45, "Texture List:", 300);

		static float offset = 0.f ;

		if (imgui_FileList(5, 70, 300, 450, DirName, MatFName, &offset, false, bChangedVal ))
		{
			char SS1[256];

			sprintf(SS1, "%s/%s", EditMaterial->OriginalDir, MatFName);
			if (!MatTexArray[i] || strcmp(MatTexArray[i]->getFileLoc().FileName, SS1) != 0)
			{
				MaterialTextureChangeUndo * pUndo = static_cast<MaterialTextureChangeUndo*>(g_pUndoHistory->CreateUndoItem(UA_MATERIAL_TEXTURE_CHANGE, 0));
				if( MatTexArray[i] )
					pUndo->SetData( EditMaterial, r3dMaterial::TexType( i ), MatTexArray[i], MatTexArray[i]->getFileLoc().FileName, SS1);
				else
					pUndo->SetData( EditMaterial, r3dMaterial::TexType( i ), 0, 0, SS1);
			}

			r3dRenderer->DeleteTexture( MatTexArray[i] );
			MatTexArray[i] = r3dRenderer->LoadTexture(SS1);
		}

		EditMaterial->SetTextures( MatTexArray );
	}

	SliderY = 50;
	SliderX = r3dRenderer->ScreenW - 380-362;

	if (showExtendedMaterialsEditor)
	{
		g_pDesktopManager->Begin("shared_materials");

		r3dColor color;
		float bias = 0; 
		float mult = 0; 
		float pow_offset = 0; 
		float power = 0;
		float blendValue = 0;

		static int lastActiveMaterial = -1;
		stringlist_t materialsNames;

		for (int m = 0; m < r3dMaterialLibrary::GetExtendedMaterialDataCount(); ++m)
		{
			char butName[256];
			sprintf_s(butName, "%s (%d)", r3dMaterialLibrary::GetExtendedMaterialName(m).c_str(), m );

			materialsNames.push_back(butName);
		}

		static float matOffset = 0.f;
		static int matIndex = -1;
		if ( imgui_DrawList( SliderX, SliderY, 360, 300, materialsNames, &matOffset, &matIndex ) )
		{
			lastActiveMaterial = matIndex;
		}

		SliderY += 310;

		if (lastActiveMaterial != -1)
		{

			static char newName[ 256 ] = {};
			strcpy_s(newName, r3dMaterialLibrary::GetExtendedMaterialName(lastActiveMaterial).c_str());

			char editName[256];
			sprintf_s(editName, "Material (%d) %s", lastActiveMaterial, newName );

			imgui_Static(SliderX, SliderY, editName, 300);
			//imgui2_StartArea ( "materialArea", SliderX, SliderY, 360, 30 );
			//imgui2_StringValue ( editName, newName );
			//imgui2_EndArea ();

			//if (stricmp(newName, r3dMaterialLibrary::GetExtendedMaterialName(lastActiveMaterial).c_str()))
			//{
			//	r3dMaterialLibrary::UpdateExtendedMaterialsName(lastActiveMaterial, newName);
			//}

			SliderY += 33;


			float fres[4];
			float col[4];

			r3dMaterialLibrary::GetExtendedMaterialData(lastActiveMaterial, fres, col);

			color = r3dColor(col[0]*255.0f, col[1]*255.0f, col[2]*255.0f, 255.0f);

			bias = fres[0];
			mult = fres[1];
			pow_offset = fres[2];
			power = fres[3];
			blendValue = col[3];

			r3dColor colorSave = color;
			float biasSave = bias; 
			float multSave = mult; 
			float pow_offsetSave = pow_offset;
			float powerSave = power; 
			float blendValueSave = col[3];

			SliderY += imgui_Value_Slider(SliderX, SliderY, "Fresnel bias",	&bias, 0, 1, "%-02.2f");
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Fresnel scale", &mult, 0, 10, "%-02.2f");
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Fresnel pow offset",	&pow_offset, 0, 2, "%-02.2f");
			SliderY += imgui_Value_Slider(SliderX, SliderY, "Fresnel power",	&power, 0, 5, "%-02.2f");

			SliderY += imgui_Value_Slider(SliderX, SliderY, "Material color blend",	&blendValue, 0, 1, "%-02.2f");

			SliderY += imgui_DrawColorPicker(SliderX, SliderY, "Diffuse Color", &color, 360, 0);


			if (colorSave != color || biasSave != bias || multSave != mult || pow_offsetSave != pow_offset || powerSave != power 
				|| blendValue != blendValueSave)
			{
				float fres[4] = {bias, mult, pow_offset, power };
				float col[4] = {(float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, blendValue };

				r3dMaterialLibrary::UpdateExtendedMaterialData(lastActiveMaterial, fres, col);
				r3dMaterialLibrary::UpdateExtendedMaterialsTexture();
			}

			if( imgui_Button( SliderX, SliderY, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, "Apply changes" ) )
			{

				r3dMaterialLibrary::SaveDefaultExtendedMaterialParameters();
			}

			SliderY += 30;

		}

		g_pDesktopManager->End();

	}

}


void Editor_Level :: FlushStates()
{
	if ( !ColorCorrectionTicked && InColorCorrection )
	{
		RestoreCCLUT3DTexture ();
		InColorCorrection = false;
	}

	ColorCorrectionTicked = false;

	//------------------------------------------------------------------------

	int RoadTickedOld = g_iRoadsTicked;

	g_iRoadsTicked = r3dTL::Max ( 0, g_iRoadsTicked - 1 );

	if ( RoadTickedOld > 0 && g_iRoadsTicked == 0 )
	{
	}

	int GamePlayTickedOld = g_iGamePlayTicked;

	g_iGamePlayTicked = r3dTL::Max ( 0, g_iGamePlayTicked - 1 );

	if ( GamePlayTickedOld > 0 && g_iGamePlayTicked == 0 )
	{
		g_iDoubleClickCheckerCounter = 0;
		g_dObjects.clear ();
		g_dObjectNames.clear ();
	}

	int ProcessObjectsTickedOld = g_iProcessObjectsTicked;
	g_iProcessObjectsTicked = r3dTL::Max ( 0, g_iProcessObjectsTicked - 1 );
	
	if ( ProcessObjectsTickedOld > 0 && g_iProcessObjectsTicked == 0 )
	{
	}

	int ProcessGroupsTickedOld = g_iProcessGroupsTicked;
	g_iProcessGroupsTicked = r3dTL::Max ( 0, g_iProcessGroupsTicked - 1 );

	if ( ProcessGroupsTickedOld > 0 && g_iProcessGroupsTicked == 0 )
	{
		for ( uint32_t i = 0; i < obj_Group::m_dGroups.Count (); i++ )
		{
			obj_Group::m_dGroups[i]->SetPreviewMode(false);
			obj_Group::m_dGroups[i]->ResetJustCreatedFlag ();
		}
	}

	int ProcessWaterPlanesTickedOld = g_iProcessWaterPlanesTicked;
	g_iProcessWaterPlanesTicked = r3dTL::Max ( 0, g_iProcessWaterPlanesTicked - 1 );

	int ProcessBattleZoneTickedOld = g_iProcessBattleZoneTicked;
	g_iProcessBattleZoneTicked = r3dTL::Max ( 0, g_iProcessBattleZoneTicked - 1 );

	int PickerEnableTickedOld = g_iPickerEnableTicked;
	g_iPickerEnableTicked = r3dTL::Max ( 0, g_iPickerEnableTicked - 1 );

	if ( PickerEnableTickedOld > 0 && g_iPickerEnableTicked == 0 )
		g_Manipulator3d.Disable ();
}

//--------------------------------------------------------------------------------------------------------
const char * GetComputerIdName()
{
	static char buffer[ MAX_PATH ] = { 0 };

	if ( ! *buffer )
	{
		DWORD dwCount = MAX_PATH; 

		GetComputerName( buffer, &dwCount ); 
		strcat( buffer, "." );
		DWORD dwCount1 = MAX_PATH - dwCount - 1;
		GetUserName( buffer + dwCount + 1, &dwCount1 );
	}
	return buffer;
}

//------------------------------------------------------------------------

namespace
{
	template < bool Write >
	void SerializeXMLEditorSettings( pugi::xml_node root, Editor_Level* EL )
	{
		const bool W = Write ;

		SerializeXMLVal<W>( "start_position"			, root, &gCam							);
		SerializeXMLVal<W>( "start_orientation"			, root, &gCam.vPointTo					);


		SerializeXMLVal<W>( "main_mode"					, root, &EL->MainToolIdx				);
		SerializeXMLVal<W>( "terrain_mode"				, root, &EL->TerrainEditMode			);
		SerializeXMLVal<W>( "obj_mode"					, root, &EL->ObjectEditMode				);
		SerializeXMLVal<W>( "env_mode"					, root, &EL->EnvironmentEditMode		);
		SerializeXMLVal<W>( "paint_terrain_color"		, root, &EL->m_tPaintTerrainColor		);
		SerializeXMLVal<W>( "collections_brush_radius"	, root, &BrushRadius					);
		SerializeXMLVal<W>( "water_brush_radius"		, root, &g_fWaterPlaneBrushRadius		);
		SerializeXMLVal<W>( "water_edit_mode"			, root, &g_iWaterEditMode				);

		for(int i=0; i<TE_NUM; ++i)
		{
			char BrushName[ 256 ] ;
			sprintf( BrushName, "brush_%s", EditModeToString( EditMode_t( i ) ) );

			strlwr( BrushName ) ;

			if( pugi::xml_node bn = SerializeXMLNode<W>( root, BrushName ) )
			{
				SerializeXMLVal<W>( "radius"		, bn, &TerrainBrushRadiusVal[ i ]		);
				SerializeXMLVal<W>( "hardness"		, bn, &TerrainBrushHardnessVal[ i ]		);
				SerializeXMLVal<W>( "strength"		, bn, &TerrainBrushStrengthVal[ i ]		);
				SerializeXMLVal<W>( "height"		, bn, &TerrainBrushStrengthVal[ i ]		);
				SerializeXMLVal<W>( "height_adjust"	, bn, &TerrainBrushHeightAdjustVal[ i ]	);
				SerializeXMLVal<W>( "noise"			, bn, &TerrainBrushNoiseVal[ i ]		);
				SerializeXMLVal<W>( "noise_raise"	, bn, &TerrainBrushNoiseRaiseVal[ i ]	);
			}
		}

		for(int i=0; i<TE2_NUM; ++i)
		{
			char BrushName[ 256 ] ;
			sprintf( BrushName, "brush_%s", EditModeToString2( EditMode2_t( i ) ) );

			strlwr( BrushName ) ;

			if( pugi::xml_node bn = SerializeXMLNode<W>( root, BrushName ) )
			{
				SerializeXMLVal<W>( "radius"		, bn, &Terrain2BrushRadiusVal[ i ]			);
				SerializeXMLVal<W>( "hardness"		, bn, &Terrain2BrushHardnessVal[ i ]		);
				SerializeXMLVal<W>( "strength"		, bn, &Terrain2BrushStrengthVal[ i ]		);
				SerializeXMLVal<W>( "height"		, bn, &Terrain2BrushStrengthVal[ i ]		);
				SerializeXMLVal<W>( "height_adjust"	, bn, &Terrain2BrushHeightAdjustVal[ i ]	);
				SerializeXMLVal<W>( "noise"			, bn, &Terrain2BrushNoiseVal[ i ]			);
				SerializeXMLVal<W>( "noise_raise"	, bn, &Terrain2BrushNoiseRaiseVal[ i ]		);
			}
		}

		for(int i=0; i<TE3_NUM; ++i)
		{
			char BrushName[ 256 ] ;
			sprintf( BrushName, "brush_%s", EditModeToString3( EditMode3_t( i ) ) );

			strlwr( BrushName ) ;

			if( pugi::xml_node bn = SerializeXMLNode<W>( root, BrushName ) )
			{
				SerializeXMLVal<W>( "radius"		, bn, &Terrain3BrushRadiusVal[ i ]			);
				SerializeXMLVal<W>( "hardness"		, bn, &Terrain3BrushHardnessVal[ i ]		);
				SerializeXMLVal<W>( "strength"		, bn, &Terrain3BrushStrengthVal[ i ]		);
				SerializeXMLVal<W>( "height"		, bn, &Terrain3BrushStrengthVal[ i ]		);
				SerializeXMLVal<W>( "height_adjust"	, bn, &Terrain3BrushHeightAdjustVal[ i ]	);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------

void Editor_Level::SaveSettings( const char* targetDir )
{
	pugi::xml_document xmlFile;
	xmlFile.append_child( pugi::node_comment ).set_value("Editor Settings File") ;
	pugi::xml_node xmlRoot = xmlFile.append_child() ;
	xmlRoot.set_name( "root" ) ;

	char XMLFilePath[ MAX_PATH ] ;
	sprintf( XMLFilePath, LEVELEDITOR_SETTINGS_FILE, targetDir );

	SerializeXMLEditorSettings<true>( xmlRoot, this );

	xmlFile.save_file( XMLFilePath );
}

//------------------------------------------------------------------------

void ReadXMLEditorSettingsStartPosition( r3dPoint3D* oPos )
{
	char XMLFilePath[ MAX_PATH ] ;
	sprintf( XMLFilePath, LEVELEDITOR_SETTINGS_FILE, r3dGameLevel::GetHomeDir() );

	r3dFile* f = r3d_open( XMLFilePath, "rb" );

	if( f )
	{
		r3dTL::TArray< char > xmlFileBuffer;

		xmlFileBuffer.Resize( f->size + 1 );

		fread( &xmlFileBuffer[ 0 ], f->size, 1, f );
		xmlFileBuffer[ f->size ] = 0;

		pugi::xml_document xmlLevelFile ;
		pugi::xml_parse_result parseResult = xmlLevelFile.load_buffer_inplace( &xmlFileBuffer[0], f->size );
		fclose( f );

		if( !parseResult )
		{
			r3dError( "LoadLevel: Failed to parse %s, error: %s", XMLFilePath, parseResult.description() );
		}

		pugi::xml_node root = xmlLevelFile.root().child( "root" ) ;

		SerializeXMLVal<false>( "start_position", root, oPos );
	}
}

int Editor_Level::LoadSettingsXML()
{
	char XMLFilePath[ MAX_PATH ] ;
	sprintf( XMLFilePath, LEVELEDITOR_SETTINGS_FILE, r3dGameLevel::GetHomeDir() );

	r3dFile* f = r3d_open( XMLFilePath, "rb" );

	if( f )
	{
		r3dTL::TArray< char > xmlFileBuffer;

		xmlFileBuffer.Resize( f->size + 1 );

		fread( &xmlFileBuffer[ 0 ], f->size, 1, f );
		xmlFileBuffer[ f->size ] = 0;

		pugi::xml_document xmlLevelFile ;
		pugi::xml_parse_result parseResult = xmlLevelFile.load_buffer_inplace( &xmlFileBuffer[0], f->size );
		fclose( f );

		if( !parseResult )
		{
			r3dError( "LoadLevel: Failed to parse %s, error: %s", XMLFilePath, parseResult.description() );
			return 0;
		}

		pugi::xml_node root = xmlLevelFile.root().child( "root" ) ;

		SerializeXMLEditorSettings<false>( root, this );
		Hud->SetCameraDir(gCam.vPointTo);

		return 1 ;

	}

	return 0 ;

}

//------------------------------------------------------------------------

void Editor_Level::LoadSettings()
{
	for( int i = 0, e = TE2_NUM ; i < e ; i ++ )
	{
		Terrain2BrushRadiusVal[ i ] = 10.0f ;
		Terrain2BrushHardnessVal[ i ] = 0.5f ;
		Terrain2BrushStrengthVal[ i ] = 0.5f ;
		Terrain2BrushHeightVal[ i ] = 0.f ;
		Terrain2BrushHeightAdjustVal[ i ] = 0.5f ;
		Terrain2BrushNoiseVal[ i ] = 0.f ;
		Terrain2BrushNoiseRaiseVal[ i ] = 0.f ;
	}

	LoadSettingsXML();
}

//--------------------------------------------------------------------------------------------------------
void Editor_Level::PreInit ()
{
	r3dMesh::g_pMeshLoadCallbacker = MeshLoaded;
	r3dMesh::g_pMeshUnloadCallbacker = MeshUnloaded;
}

//--------------------------------------------------------------------------------------------------------
void Editor_Level::Init()
{
	r3dMesh::g_pMeshLoadCallbacker = MeshLoaded;
	r3dMesh::g_pMeshUnloadCallbacker = MeshUnloaded;

	g_bEditMode = true;
	LoadSettings();

	//Desctop().SetViewPosition( 0, 50 );
	Desktop().SetViewSize( r3dRenderer->ScreenW, r3dRenderer->ScreenH );

	Desktop_c * pDesc;

	pDesc = g_pDesktopManager->AddDesktop( "ed_menu_options" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 50 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 130 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_sys_settings" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 50 + 33 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 130 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_terrain" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 85 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 130 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_roads" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 410, 80 );
	pDesc->SetViewSize( 400, r3dRenderer->ScreenH - 130 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_objects" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 50 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 95 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_env" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 85 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 130 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_decals" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 180 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 215 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_postfx" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 50 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 95 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_colorcorr" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 50 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 95 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_collections" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 388-375, 50 );
	pDesc->SetViewSize( 380 + 375, r3dRenderer->ScreenH - 95 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_materials" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, 50 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 95 );

	pDesc = g_pDesktopManager->AddDesktop( "ed_dmglib" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 380, 50 );
	pDesc->SetViewSize( 365, r3dRenderer->ScreenH - 95 );

	pDesc = g_pDesktopManager->AddDesktop( "shared_materials" );
	pDesc->SetViewPosition( r3dRenderer->ScreenW - 380-362, 50 );
	pDesc->SetViewSize( 362, r3dRenderer->ScreenH - 50 );

	// we will write options file when user logins into game
	void writeGameOptionsFile();
	writeGameOptionsFile();

	// create debug object
	if( g_bEditMode )
	{
		m_DebugTexture = srv_CreateGameObject( "obj_DebugTexture", "BugMeNot", r3dPoint3D( 0, 0, 0 ) ) ;
	}

	ScopeTest_Reticle		= r3dRenderer->LoadTexture( "Data/Weapons/Scopes/Scope_ACOG.dds" ) ;
	ScopeTest_Normal		= r3dRenderer->LoadTexture( "Data/Weapons/Scopes/Frame_ACOG_N.dds" ) ;
	ScopeTest_Mask			= r3dRenderer->LoadTexture( "Data/Weapons/Scopes/Frame_ACOG.dds" ) ;
	ScopeTest_BlurMask		= r3dRenderer->LoadTexture( "Data/Weapons/Scopes/FrameMask_ACOG.dds" ) ;
	SimpleCrosshair_Reticle = r3dRenderer->LoadTexture( "Data/Weapons/Scopes/default_aim.dds" ) ;
	SimpleCrosshair_Mask	= r3dRenderer->LoadTexture( "Data/Shaders/TEXTURE/white_transparent.dds" ) ;

	if( Terrain3 )
	{
		g_pTerrain3Editor = gfx_new Terrain3Editor;
	}

	if( Terrain2 )
	{
		g_pTerrain2Editor = gfx_new Terrain2Editor;
	}
}

//--------------------------------------------------------------------------------------------------------
void Editor_Level::Release()
{
	r3dFinishBackGroundTasks();

	g_bEditMode = false;
	SaveSettings( r3dGameLevel::GetHomeDir() );

	SAFE_DELETE( m_pPreviewBuffer );

	r3dRenderer->DeleteTexture( ScopeTest_Reticle ) ;
	r3dRenderer->DeleteTexture( ScopeTest_Normal ) ;
	r3dRenderer->DeleteTexture( ScopeTest_Mask ) ;
	r3dRenderer->DeleteTexture( ScopeTest_BlurMask ) ;
	r3dRenderer->DeleteTexture( SimpleCrosshair_Reticle ) ;
	r3dRenderer->DeleteTexture( SimpleCrosshair_Mask ) ;

	FreeGrassPlaneManager();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------------------------------------------
void UndoEntityChanged::Set( EntInfo_t &state )
{
	int nOldState = __RepositionObjectsOnTerrain;
	__RepositionObjectsOnTerrain = 0; // hack:

	// find object by hash
	GameObject* obj = GameWorld().GetObjectByHash(state.objHashID);
	if(obj)
	{
		obj->SetPosition( state.vPos );
		obj->SetRotationVector( state.vRot );
		obj->SetScale( state.vScl );
	}
	__RepositionObjectsOnTerrain = nOldState;
}
//--------------------------------------------------------------------------------------------------------
void UndoEntityGroupChanged::Set( EntInfo_t* pStates )
{
	r3d_assert(pStates);
	r3d_assert(m_numItems>0);
	int nOldState = __RepositionObjectsOnTerrain;
	__RepositionObjectsOnTerrain = 0; // hack:

	for(int i=0; i<m_numItems; ++i)
	{
		// find object by hash
		GameObject* obj = GameWorld().GetObjectByHash(pStates[i].objHashID);
		if(obj)
		{
			obj->SetPosition( pStates[i].vPos );
			obj->SetRotationVector( pStates[i].vRot );
			obj->SetScale( pStates[i].vScl );
		}
	}
	__RepositionObjectsOnTerrain = nOldState;
}

//------------------------------------------------------------------------

void UndoEntityAddDel::ExpandState ( EntAddDel_t &state )
{
	if( state.pEnt )
	{
		state.EditorID = state.pEnt->EditorID;
		state.OwnerID = state.pEnt->ownerID;
	}
	else
	{
		state.EditorID = invalidGameObjectID;
		state.OwnerID = invalidGameObjectID;
	}
}

//--------------------------------------------------------------------------------------------------------
void UndoEntityAddDel::AddItem ( EntAddDel_t &state )
{
	ExpandState( state );

	m_dData.PushBack(state);
}

//--------------------------------------------------------------------------------------------------------
void UndoEntityAddDel::Create( EntAddDel_t &state )
{
	ExpandState( state );

	m_dData.PushBack(state);
	Create ();
}

//--------------------------------------------------------------------------------------------------------
void UndoEntityAddDel::Create ()
{
	r3dTL::TArray< GameObject * > dData;

	for ( uint32_t i = 0; i < m_dData.Count(); i++ )
	{
		GameObject *o = m_dData[i].pEnt;
		if (o && o->isSerializable())
		{
			pugi::xml_document xmlObj;
			SaveLevelObject( xmlObj, o );
			ModifyShadowsFlagForTransparentObject(o);
			xmlObj.save( m_dData[i].data );
		}
		if ( m_dData[i].bDelete )
		{
			o->OnDelete();
			GameWorld().DeleteObject( o );
		}
	}
}

GameObject * LoadLevelObject ( pugi::xml_node & curNode );

//--------------------------------------------------------------------------------------------------------
void UndoEntityAddDel::Undo()			
{ 
	for ( uint32_t i = m_dData.Count (); i-- > 0; )
	{
		EntAddDel_t& entry = m_dData[i];

		if ( entry.bDelete )
		{
			pugi::xml_document xmlObj;

			xmlObj.load( entry.data.GetData() );
			pugi::xml_node xmlNode = xmlObj.child("object");

			GameObject* oldPtr = entry.pEnt;

			entry.pEnt = LoadLevelObject( xmlNode );

			entry.pEnt->EditorID = entry.EditorID;
			entry.pEnt->ownerID = entry.OwnerID;
		}
		else
		{
			if( GameObject* ent = GameWorld().GetObjectByEditorID( entry.EditorID ) )
			{
				GameWorld().DeleteObject( ent );
			}
			entry.pEnt = NULL;
		}
	}

	g_Manipulator3d.PickedObjectSet( NULL );
}

//--------------------------------------------------------------------------------------------------------
void UndoEntityAddDel::Redo()			
{ 
	for ( uint32_t i = 0; i < m_dData.Count (); i++ )
	{
		EntAddDel_t& entry = m_dData[i];

		if ( entry.bDelete )
		{
			if( GameObject* ent = GameWorld().GetObjectByEditorID( entry.EditorID ) )
			{
				GameWorld().DeleteObject( ent );
			}

			entry.pEnt = NULL;
		}
		else
		{
			pugi::xml_document xmlObj;
			xmlObj.load( entry.data.GetData() );
			pugi::xml_node xmlNode = xmlObj.child("object");
			entry.pEnt = LoadLevelObject( xmlNode );

			if( entry.pEnt )
			{
				entry.pEnt->EditorID = entry.EditorID;
				entry.pEnt->ownerID = entry.OwnerID;
			}
		}
	}

	g_Manipulator3d.PickedObjectSet( NULL );
}

//--------------------------------------------------------------------------------------------------------
const FixedString& UndoEntityAddDel::GetTitle()	
{ 
	if ( m_dData.Count () > 0 )
		m_sTitle = Va( "%s", m_dData[0].bDelete ? "Del" : "Add" );
	else
		m_sTitle = "no object";
	return m_sTitle; 
}

//------------------------------------------------------------------------

void UndoReplaceObjects::UndoRedo( bool redo )
{
	r3dTL::TArray<obj_Prefab*> toRecreate;
	for( int i = 0, e = (int)m_dData.Count(); i < e; i ++ )
	{
		UndoEntry& ue = m_dData[ i ];

		r3d_assert( ue.EditorID != invalidGameObjectID );

		if( GameObject * oldObj = GameWorld().GetObjectByEditorID( ue.EditorID ) )
		{
			const r3dString& meshFileName = ( redo ? ue.TargType : ue.SrcType );

			GameObject * newObj = srv_CreateGameObject( oldObj->Class->ID, meshFileName.c_str(), oldObj->GetPosition() );

			pugi::xml_document xmlObj;

			xmlObj.load( ue.xmlBuffer.GetData() );
			pugi::xml_node xmlNode = xmlObj.child( "object" );

			newObj->ReadSerializedData( xmlNode );

			obj_Prefab * prefab = oldObj->ownerPrefab;
			if (prefab)
			{
				prefab->OpenForModification();
				prefab->RemoveObject(oldObj);
				prefab->AddNewObject(newObj);
				toRecreate.PushBack(prefab);
			}
			else
			{
				GameWorld().DeleteObject( oldObj );
			}

			newObj->EditorID = ue.EditorID;
			newObj->ownerID = ue.OwnerID;

			ue.Object = newObj;
		}
	}

	for (uint32_t i = 0; i < toRecreate.Count(); ++i)
	{
		toRecreate[i]->CloseAfterModification();
	}
}

//------------------------------------------------------------------------

void UndoReplaceObjects::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void UndoReplaceObjects::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void UndoReplaceObjects::AddItem( const UndoEntry &entry )
{
	m_dData.PushBack( entry );

	UndoEntry& last = m_dData.GetLast();

	if( last.Object )
	{
		last.EditorID = last.Object->EditorID;
		last.OwnerID = last.Object->ownerID;
	}
	else
	{
		last.EditorID = invalidGameObjectID;
		last.OwnerID = invalidGameObjectID;
	}

	GameObject *o = last.Object;
	if( o && (o->isSerializable() || o->ownerPrefab) )
	{
		pugi::xml_document xmlObj;

		pugi::xml_node node = xmlObj.append_child();

		node.set_name( "object" );

		o->WriteSerializedData( node );
		
		xmlObj.save( last.xmlBuffer );
	}
}

//------------------------------------------------------------------------

void DebugShowRipples()
{
	if( LevelEditor.m_DebugTexture )
	{
		obj_WaterPlane* found = 0 ;

		// find water plane
		for( GameObject *obj = GameWorld().GetFirstObject(); obj ; obj = GameWorld().GetNextObject( obj ) )
		{
			if( obj->Class->Name == "obj_WaterPlane" )
			{
				found = (obj_WaterPlane*) obj ;
				break ;
			}
		}

		if( found )
		{
			static_cast< obj_DebugTexture * >( LevelEditor.m_DebugTexture ) -> SetDebugTex( found->ripplesRT[0]->Tex, 0.0f, 0.0f, 0.85f * r3dRenderer->ScreenH / r3dRenderer->ScreenW, 0.85f  ) ;
		}
	}
}

void DebugTextureOff()
{
	if( LevelEditor.m_DebugTexture )
	{
		static_cast<obj_DebugTexture*>( LevelEditor.m_DebugTexture ) -> SetDebugTex( 0, 0.f, 0.f, 0.f, 0.f ) ;
	}
}

void DebugCyclePlayerAura()
{
	for(GameObject *obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if( obj->isObjType(OBJTYPE_Human) )
		{

			obj_Player::AuraType& at = static_cast< obj_Player* >( obj )->m_AuraType ;

			at = obj_Player::AuraType( at + 1 );
			if( at >= obj_Player::AT_COUNT )
			{
				at = obj_Player::AuraType( 0 );
			}
		}
	}
}

void DebugPlayerDie()
{
	if( g_bEditMode )
	{
		for(GameObject *obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->isObjType(OBJTYPE_Human) )
			{
				static_cast< obj_Player* >( obj )->DoDeath( 0, true, storecat_INVALID );
			}
		}
	}
}

void DebugPlayerAuraTrasnparency( float alpha )
{
	if( g_bEditMode )
	{
		for(GameObject *obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->isObjType( OBJTYPE_Human ) )
			{
				static_cast< obj_Player* >( obj )->m_AuraTransarency = alpha;
			}
		}
	}
}

void DebugExportPhysxScene()
{
	if (g_pPhysicsWorld)
		g_pPhysicsWorld->ExportWholeScene("game_scene.physx");
}

void DebugPlayerRagdoll()
{
	if( g_bEditMode )
	{
		for(GameObject *obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->isObjType(OBJTYPE_Human) )
			{
				static bool ragdoll = true;
				r3dPoint3D force;
				force.x = u_GetRandom(-1, 1);
				force.y = u_GetRandom(-1, 1);
				force.z = u_GetRandom(-1, 1);
				force *= 100;
				static_cast< obj_Player* >( obj )->DoRagdoll(ragdoll, 255, &force);
				ragdoll = !ragdoll;
			}
		}
	}
}

void DumpVMemStats()
{
	
	void ConPrint( const char* , ... );

	UINT vmem = r3dRenderer->pd3ddev->GetAvailableTextureMem() ;

	ConPrint( "===============================\n" ) ;
	ConPrint( "Available video memory: %.2f MB\n", vmem / 1024.f / 1024.f ) ;

	ConPrint( "Allocated by us: %.2f MB\n", r3dRenderer->Stats.GetVMemTotal() / 1024.f / 1024.f ) ;
	ConPrint( "Textures: %.2f MB\n", r3dRenderer->Stats.GetTexMem() / 1024.f / 1024.f ) ;
	ConPrint( "Thumbnails: %.2f MB\n", r3dThumbnailTexture::TotalThumbnailSizeInVMem / 1024.f / 1024.f );
	ConPrint( "UITextures: %.2f MB\n", r3dRenderer->Stats.GetUITexMem() / 1024.f / 1024.f ) ;
	ConPrint( "RTs: %.2f MB\n", r3dRenderer->Stats.GetRenderTargetMem() / 1024.f / 1024.f ) ;
	ConPrint( "Buffers: %.2f MB\n", r3dRenderer->Stats.GetBufferMem() / 1024.f / 1024.f ) ;
	ConPrint( "Player Textures: %.2f MB\n", r3dRenderer->Stats.GetPlayerTexMem() / 1024.f / 1024.f ) ;
	ConPrint( "Player Buffers: %.2f MB\n", r3dRenderer->Stats.GetPlayerBufferMem() / 1024.f / 1024.f ) ;
	ConPrint( "Player Total: %.2f MB\n", (	r3dRenderer->Stats.GetPlayerBufferMem() +  
											r3dRenderer->Stats.GetPlayerTexMem() ) / 1024.f / 1024.f ) ;
}

void DumpPlayerTexes()
{
#define R3D_PLAYER_TEXEX_DUMP "playertextures.txt"
	FILE* fout = fopen( R3D_PLAYER_TEXEX_DUMP, "wt" ) ;

	typedef r3dgameMultiMapCmp( size_t, r3dTexture*, std::greater< size_t > ) TexMap ;
	TexMap texes ;

	for( r3dTexture * tex = r3dRenderer->FirstTexture ; tex ; tex = tex->pNext )
	{
		if( tex->GetFlags() & r3dTexture::fPlayerTexture )
		{
			texes.insert( std::pair<size_t, r3dTexture*>( tex->GetTextureSizeInVideoMemory(), tex ) ) ;
		}
	}

	for( TexMap :: iterator i = texes.begin(), e = texes.end() ; i != e ; ++ i )
	{
		r3dTexture* tex = i->second ;
		fprintf( fout, "%-100s - %4dx%4d - %5d kb\n", tex->getFileLoc().FileName, tex->GetWidth(), tex->GetHeight(), tex->GetTextureSizeInVideoMemory() / 1024 ) ;
	}

	void ConPrint( const char* , ... );
	ConPrint( "Dumped to " R3D_PLAYER_TEXEX_DUMP "\n" ) ;

	fclose( fout ) ;
}

void DumpRTStats()
{
	void ConPrint( const char* , ... );
	
	r3dTL::TArray< r3dScreenBuffer* > rts ;

	r3dRenderer->EnumerateRenderTargets( &rts ) ;

	FILE* fout = fopen( "rtsdump.txt", "wt" ) ;

	char buffer[ 1024 ] ;

	typedef r3dgameMultiMapCmp( int, r3dScreenBuffer*, std::greater<int> ) RTMap ;

	RTMap rtmap ;

	for( int i = 0, e = rts.Count() ; i < e ; i ++ )
	{
		r3dScreenBuffer* sb = rts[ i ] ;

		if( sb->Tex )
		{
			rtmap.insert( RTMap::value_type( sb->UsedMemory, sb ) ) ;
		}
		else
		{
			rtmap.insert( RTMap::value_type( 0, sb ) ) ;
		}
	}

	int sum = 0 ;

	for( RTMap::iterator i = rtmap.begin(), e = rtmap.end() ; i != e ; ++ i )
	{
		r3dScreenBuffer* sb = i->second ;

		if( sb->Tex )
		{
			int bytes = sb->UsedMemory ;
			sum += bytes ;

			char* ztype = "Z_SHARED";

			if( sb->zType == r3dScreenBuffer::Z_OWN )
			{
				ztype = "Z_OWN" ;
			}

			if( sb->zType == r3dScreenBuffer::Z_SHADOW )
			{
				ztype = "Z_SHADOW" ;
			}

			const char* dbgName = sb->GetDebugName();

			sprintf( buffer, "%-32s = %-5dx%-5d - %-5dkb %s\n", dbgName ? dbgName : "(unknown)", (int)sb->Width, (int)sb->Height, bytes / 1024, ztype ) ;
		}
		else
		{
			sprintf( buffer, "%-32s = Empty\n", sb->GetDebugName() ) ;
		}

		ConPrint( "%s", buffer ) ;
		fputs( buffer, fout ) ;
	}

	sprintf( buffer, "---------------------------------\n" ) ;
	ConPrint( "%s", buffer ) ;
	fputs( buffer, fout ) ;

	int shadowZbufSize = r3dGetShadowZBufferSize() ;
	sprintf( buffer, "Shadow ZBuf: %d kb\n", shadowZbufSize / 1024 ) ;
	ConPrint( "%s", buffer ) ;
	fputs( buffer, fout ) ;
	sum += shadowZbufSize ;

	int bbZbufSize = int( ScreenBuffer->Width * ScreenBuffer->Height * 4 ) ;
	sprintf( buffer, "System ZBuf: %d kb\n", bbZbufSize / 1024 ) ;
	ConPrint( "%s", buffer ) ;
	fputs( buffer, fout ) ;
	sum += bbZbufSize ;

	sprintf( buffer, "Total: %.1f MB\n", sum / 1024.f / 1024.f ) ;
	ConPrint( "%s", buffer ) ;
	fputs( buffer, fout ) ;

	fclose( fout ) ;
}

void DisplayLumaInfo()
{
	// display exposure/ luminance tab
	float luma = 0.5f, exp=0.0f ;
	IDirect3DTexture9* tex = NULL ;

	struct CreateDelTex
	{
		CreateDelTex()
		{
			D3D_V( r3dRenderer->pd3ddev->CreateTexture( 1, 1, 0, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &tex, NULL ) ) ;
			D3D_V( tex->GetSurfaceLevel( 0, &surf ) ) ;
		}

		~CreateDelTex()
		{
			surf->Release() ;
			tex->Release() ;
		}

		IDirect3DSurface9* surf ;
		IDirect3DTexture9* tex ;

	} cdt ;

	char exp_val[ 512 ] ;

	D3DLOCKED_RECT lrect ;

	// can be device lost
	if( r3dRenderer->pd3ddev->GetRenderTargetData( SceneExposure1->GetTex2DSurface(), cdt.surf ) == D3D_OK ) 
	{
		D3D_V( cdt.tex->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) ) ;
		exp=(*(float*)lrect.pBits);		
		D3D_V( cdt.tex->UnlockRect( 0 ) ) ;
	}


	if( r3dRenderer->pd3ddev->GetRenderTargetData( AvgSceneLuminanceBuffer->GetTex2DSurface(), cdt.surf ) == D3D_OK ) 
	{
		D3D_V( cdt.tex->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) ) ;
		luma=(*(float*)lrect.pBits);		
		D3D_V( cdt.tex->UnlockRect( 0 ) ) ;
	}

	sprintf( exp_val, "Scene Exposure: %7.4f, Luma: %7.4f", exp, luma ) ;

	imgui_Static( 5, r3dRenderer->ScreenH-235, exp_val ) ;
}

void DrawSunPath()
{
	extern r3dSun* Sun;

	if( Sun )
	{
		const float DISPLAY_RAD = 5000.f;

		const float step = 0.05f;
		for( float t = 0 ; t < 1.0f ; t += step )
		{
			r3dPoint3D vec0 = Sun->GetSunVecAtNormalizedTime( t );
			r3dPoint3D vec1 = Sun->GetSunVecAtNormalizedTime( t + step );

			vec0 = gCam + vec0 * DISPLAY_RAD;
			vec1 = gCam + vec1 * DISPLAY_RAD;

			r3dDrawUniformLine3D( vec0, vec1, gCam, r3dColor::green );
		}

		r3dPoint3D curVec = Sun->GetCurrentSunVec();

		r3dPoint3D perp = curVec.Cross( r3dPoint3D(1,0,0) );

		if( perp.Length() < 0.00001f )
			perp = curVec.Cross( r3dPoint3D(0,1,0) );

		perp.Normalize();

		r3dPoint3D perp2 = perp.Cross( curVec );

		perp2.Normalize();

		r3dPoint3D centre = gCam + curVec * DISPLAY_RAD;

		perp *= DISPLAY_RAD * 0.03125f;
		perp2 *= DISPLAY_RAD * 0.03125f;

		r3dDrawUniformLine3D( centre - perp, centre + perp, gCam, r3dColor::yellow );
		r3dDrawUniformLine3D( centre - perp2, centre + perp2, gCam, r3dColor::yellow );

		r3dRenderer->Flush();
	}
}

void DrawMoonSpot()
{
	const float DISPLAY_RAD = 5000.f;

	r3dPoint3D curVec = GetMoonVec();

	r3dPoint3D perp = curVec.Cross( r3dPoint3D(1,0,0) );

	if( perp.Length() < 0.00001f )
		perp = curVec.Cross( r3dPoint3D(0,1,0) );

	perp.Normalize();

	r3dPoint3D perp2 = perp.Cross( curVec );

	perp2.Normalize();

	r3dPoint3D centre = gCam + curVec * DISPLAY_RAD;

	perp *= DISPLAY_RAD * 0.03125f;
	perp2 *= DISPLAY_RAD * 0.03125f;

	r3dDrawUniformLine3D( centre - perp, centre + perp, gCam, r3dColor::yellow );
	r3dDrawUniformLine3D( centre - perp2, centre + perp2, gCam, r3dColor::yellow );

	r3dRenderer->Flush();
}

void SpawnZombie()
{
	r3dPoint3D spawnPos = UI_TargetPos;

	obj_Zombie* z = static_cast<obj_Zombie *>( srv_CreateGameObject( "obj_Zombie", "obj_Zombie", spawnPos ) );

	z->SetZombieState( EZombieStates::ZState_Sleep, true );

	PKT_S2C_CreateZombie_s czs;

	int heroId = 20183;

	const HeroConfig* heroConfig = g_pWeaponArmory->getHeroConfig( heroId );

	czs.FromID			= 575;
	czs.spawnID			= 0;
	czs.spawnPos		= spawnPos;
	czs.spawnDir		= 0;
	czs.moveCell		= r3dPoint3D( 0, 0, 0 );	// cell position from PKT_C2C_MoveSetCell
	czs.HeroItemID		= 20183; // ItemID of base character
	czs.HeadIdx			= rand() % heroConfig->getNumHeads();
	czs.BodyIdx			= rand() % heroConfig->getNumBodys();
	czs.LegsIdx			= rand() % heroConfig->getNumLegs();
	czs.State			= rand() % 2 ? EZombieStates::ZState_Sleep : EZombieStates::ZState_Idle;
	czs.FastZombie		= 0;
	czs.WalkSpeed		= 2;
	czs.RunSpeed		= 4;

	memcpy(&z->CreateParams, &czs, sizeof(czs));

	z->OnCreate();
}

void DumpAtmoTextureStats()
{
#ifndef FINAL_BUILD
	void ConPrint( const char*, ... );

	int totalSize = 0;

	for( int i = 0, e = r3dAtmosphere::SKY_PHASE_COUNT; i < e; i ++ )
	{
		int loadOutput = 0;

		char msg[ 512 ];

		if( r3dTexture * tex = r3dGameLevel::Environment->StaticSkyTextures[ i ] )
		{
			sprintf( msg, "%s is loaded", r3dGameLevel::Environment->StaticSkyTextureNames[ i ].c_str() );
			totalSize += tex->GetTextureSizeInVideoMemory();

			loadOutput = 1;
		}

		if( r3dTexture * tex = r3dGameLevel::Environment->StaticSkyGlowTextures[ i ] )
		{
			sprintf( msg + strlen( msg ), loadOutput ? ", %s is loaded\n" : "%s is loaded\n", r3dGameLevel::Environment->StaticSkyGlowTextureNames[ i ].c_str() );
			totalSize += tex->GetTextureSizeInVideoMemory();

			loadOutput = 1;
		}

		if( loadOutput )
			ConPrint( "%s", msg );
	}

	ConPrint( "Sky texture size: %.2f MB\n", totalSize / 1024.f / 1024.f );
#endif
}

void DumpFrozenShadowStats()
{
#ifndef FINAL_BUILD
	void ConPrint( const char*, ... );

	int frozenCount = 0;
	int frozenSize = 0;

	for (uint32_t i = 0, i_end = WorldLightSystem.Lights.Count(); i != i_end; ++i)
	{
		r3dLight* l = WorldLightSystem.Lights[ i ];

		if( l->bIsShadowDepthFrozen && l->ShadowDepth )
		{
			frozenCount ++;
			frozenSize += l->ShadowDepth->GetTextureSizeInVideoMemory();
		}
	}

	ConPrint( "We have %d textures of %.2f MB total size\n", frozenCount, frozenSize / 1024.f / 1024.f );
#endif
}

void DumpShadowDipsStats()
{
#ifndef FINAL_BUILD
	void ConPrint( const char*, ... );
	ConPrint( "%d (%d double)", 
					r3dRenderer->Stats.GetNumShadowDips(),
					r3dRenderer->Stats.GetNumDoubleShadowDips() );
#endif
}

#endif // FINAL_BUILD