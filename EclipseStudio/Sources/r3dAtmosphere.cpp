#include "r3dPCH.h"
#include "r3d.h"

#include "r3dAtmosphere.h"
#include "GameLevel.h"

#include "Editors/LevelEditor.h"

#include "..\eternity/SF/script.h"
#include "TrueNature/SkyDome.h"

#include "GameObjects/ObjManag.h"
#include "ObjectsCode/effects/obj_ParticleSystem.h"
#include "ObjectsCode/Nature/wind.h"

#include "XMLHelpers.h"

#include "TrueNature/Sun.h"

void WriteTimeCurve( FILE* fout, r3dTimeGradient2& curve, const char * szName );
void ReadTimeCurveNew( Script_c &script, r3dTimeGradient2& curve );

extern r3dCamera gCam ;

extern r3dSun* Sun;

#define PT_EMPTY_STR "<empty>"

int GetAtmoDownScale()
{
	return r_environment_quality->GetInt() == 1 ? 2 : 1 ;
}

static void atmoReloadTexture( r3dTexture*& tex )
{
	if( !tex )
		return;

	char FileName[ 1024 ];
	r3dscpy( FileName, tex->getFileLoc().FileName );
	FileName[ 1023 ] = 0;

	tex->Unload();
	tex->Load( FileName, D3DFMT_FROM_FILE, GetAtmoDownScale() );
}


void r3dAtmosphere :: Reset()
{
	if( RainParticleSystem )
	{
		GameWorld().DeleteObject( RainParticleSystem ) ;
		RainParticleSystem = 0 ;
	}
	RainParticleSystemName[ 0 ] = 0;

	for( int i = 0, e = SKY_PHASE_COUNT; i < e ; i ++ )
	{
		StaticSkyTextureNames[ i ] = "";

		StaticSkyGlowTextureNames[ i ] = "";

		StaticSkyIntensities[ i ] = 1.0f;
	}

	bStaticSkyEnable = false ;
	bCustomStaticMeshEnable = false ;
	bStaticSkyPlanarMapping = false ;

	SunLightOn = 1;

	DawnStart	= 4.0f;
	DawnEnd		= 7.0f;

	DuskStart	= 20.f;
	DuskEnd		= 22.f;

    ParticleShadingCoef = 0.7f;

	__CurTime = 10;
	bVolumeFog = 0;

	BacklightIntensity = 0.1f;
	BacklightColor.Reset(255.0f);
	
	SunIntensity = 1.0f;
	SkyIntensity = 1.0f;
	MoonIntensity = 1.0f;

	SunColor.Reset(255.0f);
	SunAmbientColor.Reset(55.0f);
	LambdaCol.Reset(255.0f);
	Fog_HeightFadeStart.Reset(100);
	Fog_Density.Reset(0);
	Fog_HeightFadeRange.Reset(1.0f);
	Fog_Distance.Reset(1.0f);
	Fog_MaxHeight = 1500.0f;
	Fog2_MaxHeight = 1024.0f ;

	Aerial_Density.Reset(0.3f);
	Aerial_Distance.Reset(0.9f);

	Aerial_MipBias = 0.f ;

	SkyFog_Start = 5.0f;
	SkyFog_End = 15.0f;

	Fog_MaxHeight = 6000.0f;
	Fog_HStart.Reset(400.0f);;
	Fog_HRange.Reset(1000.0f);

	Fog_SkydomeMult1.Reset(50.0f);
	Fog_SkydomeMult2.Reset(300.0f);

	Fog_CameraDist.Reset(100.0f);
	Fog_CameraRange.Reset(100.0f);
	Fog_CameraExp.Reset(1.0f);
	Fog_MaxDensity.Reset(1.0f);


	HGg.Reset(0.9993f);
	InscatteringMultiplier.Reset(1.0f);
	BetaRayMultiplier.Reset(4.0f);
	BetaMieMultiplier.Reset(0.0006f);
	SunIntensityCoef.Reset(1.0f);
	Turbitity.Reset(1.0f);

	SunSpotAmplify.Reset( 10.22f );
	SunSpotPow.Reset( 96.0f );
	SunSpotColor.Reset( 255.0f );
	SunSpotIntensity.Reset( 3.0f );

	SkyDomeWindFactor = 1.0f;
	SkyDomeCloudsScale = 1.0f;
	SkyDomeCloudsDensity = 0.5f;
	CloudFadeOutStartDist = 4000.0f;
	CloudFadeOutEndDist = 7000.0f;

	SkySunsetBumpness = 11.47f;
	SkySunsetOffset = -0.0074f;
	SkySunsetPower = 3.17f;
	SkySunsetStartCoef = 0.3f;
	SkyCloudsFadeStart = 2000;
	SkyCloudsFadeEnd = 1000;

	SunElevationAngle = -5.8f;
	SunDirectionAngle = 0.0f;

	SunMovementAngleRange = 190.f;

	MoonLongitude = 0.f;
	MoonLatitude = 0.f;

	SSSDiffuse = 0.0f;
	SSSSpecular = 0.0f;
	SSSSpecPower = 100.0f;

	StaticTexGenScaleX = 0.00001f;
	StaticTexGenScaleY = 0.00001f;

	StaticTexGetOffsetX = 0.5f;
	StaticTexGetOffsetY = 0.5f;

	SkyDomeRotationY = 0.f;

}

template < bool Write >
int r3dAtmosphere :: SerializeXML( pugi::xml_node root )
{
	const bool W = Write ;

	if( W )
	{
		FogVersion = 2 ;
	}
	else
	{
		FogVersion = 0 ;
	}

	if( pugi::xml_node atmoNode = SerializeXMLNode<W>( root, "atmosphere" ) )
	{
		float time = __CurTime;
		SerializeXMLVal<W>( "cur_time", atmoNode, &time );
		__CurTime = time;
	
		SerializeXMLVal<W>( "sun_backlightintensity"					, atmoNode, &BacklightIntensity				);
		SerializeXMLCurve3f<W>( "sun_backlightcolor_curve"				, atmoNode, &BacklightColor					);
	
		SerializeXMLCurve3f<W>( "sun_color_curve"				, atmoNode, &SunColor					);
		SerializeXMLCurve3f<W>( "moon_color_curve"				, atmoNode, &MoonLightColor				);
		SerializeXMLCurve3f<W>( "moon_ambi_curve"				, atmoNode, &MoonAmbientColor			);
		SerializeXMLCurve3f<W>( "sky_color_curve"				, atmoNode, &SunAmbientColor			);
		SerializeXMLCurve3f<W>( "lambda_col_curve"				, atmoNode, &LambdaCol					);
		SerializeXMLCurve3f<W>( "hg_g_curve"					, atmoNode, &HGg						);
		SerializeXMLCurve3f<W>( "inscattering_multiplier_curve"	, atmoNode, &InscatteringMultiplier		);
		SerializeXMLCurve3f<W>( "beta_ray_multiplier_curve"		, atmoNode, &BetaRayMultiplier			);
		SerializeXMLCurve3f<W>( "beta_mie_multiplier_curve"		, atmoNode, &BetaMieMultiplier			);
		SerializeXMLCurve3f<W>( "sun_intensity_curve"			, atmoNode, &SunIntensityCoef			);
		SerializeXMLCurve3f<W>( "turbitity_curve"				, atmoNode, &Turbitity					);

		SerializeXMLCurve3f<W>( "sun_spot_amplify"				, atmoNode, &SunSpotAmplify			);
		SerializeXMLCurve3f<W>( "sun_spot_pow"					, atmoNode, &SunSpotPow				);
		SerializeXMLCurve3f<W>( "sun_spot_color"				, atmoNode, &SunSpotColor			);
		SerializeXMLCurve3f<W>( "sun_spot_intensity"			, atmoNode, &SunSpotIntensity		);		

		SerializeXMLVal<W>( "fog_version"						, atmoNode, &FogVersion			);

		SerializeXMLCurve3f<W>( "fog3_HStart"			, atmoNode, &Fog_HStart		);
		SerializeXMLCurve3f<W>( "fog3_HRange"			, atmoNode, &Fog_HRange		);
		SerializeXMLCurve3f<W>( "fog3_HMult1"			, atmoNode, &Fog_SkydomeMult1	);
		SerializeXMLCurve3f<W>( "fog3_HMult2"			, atmoNode, &Fog_SkydomeMult2	);
	
		SerializeXMLCurve3f<W>( "fog3_CamDist"			, atmoNode, &Fog_CameraDist		);
		SerializeXMLCurve3f<W>( "fog3_CamRange"			, atmoNode, &Fog_CameraRange		);
		SerializeXMLCurve3f<W>( "fog3_CamExp"			, atmoNode, &Fog_CameraExp		);
		SerializeXMLCurve3f<W>( "fog3_MaxDensity"			, atmoNode, &Fog_MaxDensity		);

		SerializeXMLCurve3f<W>( "fog_color_curve"				, atmoNode, &Fog_Color					);
		SerializeXMLCurve3f<W>( "fog_new_range_curve"			, atmoNode, &Fog_Range					);
		SerializeXMLCurve3f<W>( "fog_distance_curve"			, atmoNode, &Fog_Distance				);
		SerializeXMLCurve3f<W>( "fog_new_density_curve"			, atmoNode, &Fog_Density				);
		SerializeXMLCurve3f<W>( "fog_height_curve"				, atmoNode, &Fog_Height					);
		SerializeXMLCurve3f<W>( "aerial_density_curve"			, atmoNode, &Aerial_Density				);
		SerializeXMLCurve3f<W>( "aerial_distance_curve"			, atmoNode, &Aerial_Distance			);

		SerializeXMLVal<W>( "skyfog_start"						, atmoNode, &SkyDomeWindFactor		);
		SerializeXMLVal<W>( "skyfog_end"						, atmoNode, &SkyDomeCloudsScale		);
		SerializeXMLVal<W>( "skydome_wind_factor"				, atmoNode, &SkyDomeWindFactor		);
		SerializeXMLVal<W>( "skydome_clouds_scale"				, atmoNode, &SkyDomeCloudsScale		);
		SerializeXMLVal<W>( "skydome_clouds_density"			, atmoNode, &SkyDomeCloudsDensity	);
		SerializeXMLVal<W>( "cloudfade_outstart_dist"			, atmoNode, &CloudFadeOutStartDist	);
		SerializeXMLVal<W>( "cloudfade_out_enddist"				, atmoNode, &CloudFadeOutEndDist	);
		SerializeXMLVal<W>( "sky_sunset_bumpness"				, atmoNode, &SkySunsetBumpness		);
		SerializeXMLVal<W>( "sky_sunset_offset"					, atmoNode, &SkySunsetOffset		);
		SerializeXMLVal<W>( "sky_sunset_power"					, atmoNode, &SkySunsetPower			);
		SerializeXMLVal<W>( "sky_sunset_startcoef"				, atmoNode, &SkySunsetStartCoef		);
		SerializeXMLVal<W>( "sky_clouds_fadestart"				, atmoNode, &SkyCloudsFadeStart		);
		SerializeXMLVal<W>( "sky_clouds_fadeend"				, atmoNode, &SkyCloudsFadeEnd		);
		SerializeXMLVal<W>( "sun_elevation_angle"				, atmoNode, &SunElevationAngle		);
		SerializeXMLVal<W>( "sun_direction_angle"				, atmoNode, &SunDirectionAngle		);
		SerializeXMLVal<W>( "moon_longitude"					, atmoNode, &MoonLongitude			);
		SerializeXMLVal<W>( "moon_latitude"						, atmoNode, &MoonLatitude			);
		SerializeXMLVal<W>( "sss_diffuse"						, atmoNode, &SSSDiffuse				);
		SerializeXMLVal<W>( "sss_specular"						, atmoNode, &SSSSpecular				);
		SerializeXMLVal<W>( "sss_specpower"						, atmoNode, &SSSSpecPower				);
		SerializeXMLVal<W>( "have_new_simple_fog"				, atmoNode, &bVolumeFog					);
		SerializeXMLVal<W>( "fog_max_height"					, atmoNode, &Fog_MaxHeight				);
		SerializeXMLVal<W>( "aerial_mip_bias"					, atmoNode, &Aerial_MipBias				);
		SerializeXMLVal<W>( "sun_intensity"						, atmoNode, &SunIntensity				);
		SerializeXMLVal<W>( "sky_intensity"						, atmoNode, &SkyIntensity				);
		SerializeXMLVal<W>( "moon_intensity"					, atmoNode, &MoonIntensity				);
		SerializeXMLVal<W>( "skydome_rotation_y"				, atmoNode, &SkyDomeRotationY			);

		if( !W )
		{
			r3dString prevVerStaticSkyTexture = "";

			SerializeXMLVal<W>( "static_sky_tex_name"			, atmoNode, &prevVerStaticSkyTexture	);

			if( prevVerStaticSkyTexture.Length() )
			{
				for( int i = 0, e = SKY_PHASE_COUNT; i < e ; i ++ )
				{
					StaticSkyTextureNames[ i ] = prevVerStaticSkyTexture;
				}
			}
		}

		for( int i = 0, e = SKY_PHASE_COUNT; i < e ; i ++ )
		{
			char name[ 64 ];

			sprintf( name, "static_sky_tex_name_phase%d", i );
			SerializeXMLVal<W>( name, atmoNode, &StaticSkyTextureNames[ i ] );

			sprintf( name, "static_sky_glow_tex_name_phase%d", i );
			SerializeXMLVal<W>( name, atmoNode, &StaticSkyGlowTextureNames[ i ] );

			sprintf( name, "static_sky_intensity_phase%d", i );
			SerializeXMLVal<W>( name, atmoNode, &StaticSkyIntensities[ i ] );
		}

		SerializeXMLVal<W>( "static_sky_mesh_name"				, atmoNode, &StaticSkyMeshName			);
		SerializeXMLVal<W>( "static_sky_enable"					, atmoNode, &bStaticSkyEnable			);
		SerializeXMLVal<W>( "static_sky_custom_mesh_enable"		, atmoNode, &bCustomStaticMeshEnable	);
		SerializeXMLVal<W>( "static_sky_planar_mapping"			, atmoNode, &bStaticSkyPlanarMapping	);

		SerializeXMLVal<W>( "static_sky_dawn_start"				, atmoNode, &DawnStart	);
		SerializeXMLVal<W>( "static_sky_dawn_end"				, atmoNode, &DawnEnd	);
		SerializeXMLVal<W>( "static_sky_dusk_start"				, atmoNode, &DuskStart	);
		SerializeXMLVal<W>( "static_sky_dusk_end"				, atmoNode, &DuskEnd	);

		SerializeXMLVal<W>( "static_texgenscale_x"				, atmoNode, &StaticTexGenScaleX			);
		SerializeXMLVal<W>( "static_texgenscale_y"				, atmoNode, &StaticTexGenScaleY			);
		SerializeXMLVal<W>( "static_texgetoffset_x"				, atmoNode, &StaticTexGetOffsetX		);
		SerializeXMLVal<W>( "static_texgetoffset_y"				, atmoNode, &StaticTexGetOffsetY		);

		SerializeXMLVal<W>( "rain_particles"					, atmoNode, RainParticleSystemName		);
		SerializeXMLVal<W>( "sunlight"							, atmoNode, &SunLightOn					);

		SerializeXMLVal<W>("particle_shading_coef"				, atmoNode, &ParticleShadingCoef		);
		
		if( g_pWind )
		{
			r3dWind::Settings windSts = g_pWind->GetSettings() ;

			SerializeXMLVal<W>( "wind_dir_x",			atmoNode, &windSts.DirX ) ;
			SerializeXMLVal<W>( "wind_dir_z",			atmoNode, &windSts.DirZ ) ;
			SerializeXMLVal<W>( "wind_pattern_scale",	atmoNode, &windSts.WindPatternScale ) ;
			SerializeXMLVal<W>( "wind_pattern_speed",	atmoNode, &windSts.WindPatternSpeed ) ;

			if( !W )
			{
				g_pWind->SetSettings( windSts ) ;
			}
		}

		return 1 ;
	}
	else
	{
		return 0 ;
	}
}

int	r3dAtmosphere :: LoadFromXML( pugi::xml_node root )
{
	strcpy( RainParticleSystemName, PT_EMPTY_STR ) ;

	int res = SerializeXML<false>( root );

	if( FogVersion == 1 )
	{
		ConvertFog_V1() ;
	}

	if( bStaticSkyEnable )
	{
		EnableStaticSky();
	}
	else
	{
		DisableStaticSky();
	}

	SetRainParticle( RainParticleSystemName ) ;

	return res ;
}

int	r3dAtmosphere :: SaveToXML( pugi::xml_node root )
{
	return SerializeXML<true>( root );
}


void
r3dAtmosphere::EnableStaticSky()
{
	for( int i = 0, e = SKY_PHASE_COUNT ; i < e ; i ++ )
	{
		DeleteSkyTextures( SkyPhase( i ) );
	}

	UpdateTextures();

	SAFE_DELETE( StaticSkyMesh );

	if( bCustomStaticMeshEnable )
	{
		StaticSkyMesh = gfx_new r3dMesh( 0 );
		StaticSkyMesh->Load( StaticSkyMeshName.c_str(), true, true );
		if( !StaticSkyMesh->IsLoaded() )
		{
			SAFE_DELETE( StaticSkyMesh );
		}
		else
		{
			StaticSkyMesh->FillBuffers();
		}
	}

	UpdateTextures();

	bStaticSkyEnable = 1;
}

void
r3dAtmosphere::DisableStaticSky()
{
	for( int i = 0, e = SKY_PHASE_COUNT ; i < e ; i ++ )
	{
		if( StaticSkyTextures[ i ] )
		{
			r3dRenderer->DeleteTexture( StaticSkyTextures[ i ] );
			StaticSkyTextures[ i ] = NULL;
		}

		if( StaticSkyGlowTextures[ i ] )
		{
			r3dRenderer->DeleteTexture( StaticSkyGlowTextures[ i ] );
			StaticSkyGlowTextures[ i ] = NULL;
		}
	}

	if( StaticSkyMesh )
	{
		SAFE_DELETE( StaticSkyMesh ) ;
	}

	extern r3dSkyDome *SkyDome;
	if( SkyDome )
	{
		StaticSkySettings sts ;
		SkyDome->SetStaticSkyParams( sts ) ;
	}

	bStaticSkyEnable = 0 ;
}

void
r3dAtmosphere::SetStaticSkyTextures( const SkyPhaseTextureNames& textureNames, const SkyPhaseTextureNames& glowTextureNames )
{
	StaticSkyTextureNames		= textureNames;
	StaticSkyGlowTextureNames	= glowTextureNames;

	if( bStaticSkyEnable )
	{
		// reloads stuff
		DisableStaticSky();
		EnableStaticSky();
	}
}


void
r3dAtmosphere::SetStaticSkyMesh( const r3dString& meshName )
{
	StaticSkyMeshName = meshName ;

	if( bStaticSkyEnable )
	{
		// reloads stuff
		DisableStaticSky();
		EnableStaticSky();
	}
}

void
r3dAtmosphere::ConvertFog_V0()
{
	r3dTimeGradient2 heightMax = Fog_HeightFadeStart ;
	r3dTimeGradient2 thicknessOfFadeZone = Fog_HeightFadeRange ;

	Fog_HeightFadeStart.Reset( 
					heightMax.GetFloatValue( 0 ) * thicknessOfFadeZone.GetFloatValue( 0 ) * 0.01f
						,
					heightMax.GetFloatValue( 1 ) * thicknessOfFadeZone.GetFloatValue( 1 ) * 0.01f
					) ;

	for( int i = 1, e = heightMax.NumValues - 1 ; i < e ; i ++ )
	{
		float t = heightMax.Values[ i ].time ;

		Fog_HeightFadeStart.AddValue( 
				t, 
				heightMax.Values[ i ].val[ 0 ] * thicknessOfFadeZone.GetFloatValue( t ) * 0.01f ) ;
	}

	Fog_HeightFadeRange.Reset(
			heightMax.GetFloatValue( 0 ) - Fog_HeightFadeStart.GetFloatValue( 0 ),
			heightMax.GetFloatValue( 1 ) - Fog_HeightFadeStart.GetFloatValue( 1 )
		) ;

	for( int i = 1, e = Fog_HeightFadeStart.NumValues - 1 ; i < e ; i ++ )
	{
		float t = Fog_HeightFadeStart.Values[ i ].time ;

		Fog_HeightFadeRange.AddValue( 
				t,
				heightMax.Values[ i ].val[ 0 ] - Fog_HeightFadeStart.Values[ i ].val[ 0 ]			
			) ;
	}
}

void
r3dAtmosphere::ConvertFog_V1()
{
	r3dTimeGradient2 heightStart	= Fog_Height ;
	r3dTimeGradient2 fogFadeRange	= Fog_Range ;

	Fog_Height.Reset(
					heightStart.GetFloatValue( 0 ) + fogFadeRange.GetFloatValue( 0 )
						,
					heightStart.GetFloatValue( 1 ) + fogFadeRange.GetFloatValue( 1 ) 
					) ;

	for( int i = 1, e = heightStart.NumValues - 1 ; i < e ; i ++ )
	{
		float t = heightStart.Values[ i ].time ;

		Fog_Height.AddValue( 
				t, 
				heightStart.Values[ i ].val[ 0 ] + fogFadeRange.GetFloatValue( t ) ) ;
	}

	Fog_Range.Reset(
			( 1.0f - heightStart.GetFloatValue( 0 ) / Fog_Height.GetFloatValue( 0 ) ) * 100.f,
			( 1.0f - heightStart.GetFloatValue( 1 ) / Fog_Height.GetFloatValue( 1 ) ) * 100.f 
		) ;

	for( int i = 1, e = heightStart.NumValues - 1 ; i < e ; i ++ )
	{
		float t = heightStart.Values[ i ].time ;

		Fog_Range.AddValue( 
				t,
				( 1.0f - heightStart.Values[ i ].val[ 0 ] / Fog_Height.Values[ i ].val[ 0 ]	) * 100.f
			) ;
	}
}

void r3dAtmosphere::LoadSkyTextures( SkyPhase phase )
{
	if( StaticSkyTextureNames[ phase ].Length() )
	{
		if( !StaticSkyTextures[ phase ] )
		{
			r3dOutToLog( "Loading sky texture: %s\n", StaticSkyTextureNames[ phase ].c_str() );
			StaticSkyTextures[ phase ] = r3dRenderer->LoadTexture( StaticSkyTextureNames[ phase ].c_str(), D3DFMT_UNKNOWN, false, GetAtmoDownScale() );
		}
	}

	if( StaticSkyGlowTextureNames[ phase ].Length() )
	{
		if( !StaticSkyGlowTextures[ phase ] )
		{
			r3dOutToLog( "Loading sky texture: %s\n", StaticSkyGlowTextureNames[ phase ].c_str() );
			StaticSkyGlowTextures[ phase ] = r3dRenderer->LoadTexture( StaticSkyGlowTextureNames[ phase ].c_str(), D3DFMT_UNKNOWN, false, GetAtmoDownScale() );
		}
	}
}

void r3dAtmosphere::DeleteSkyTextures( SkyPhase phase )
{
	if( StaticSkyTextures[ phase ] )
	{
		r3dOutToLog( "Unloading sky texture: %s\n", StaticSkyTextureNames[ phase ].c_str() );
		r3dRenderer->DeleteTexture( StaticSkyTextures[ phase ] );

		StaticSkyTextures[ phase ] = NULL;
	}

	if( StaticSkyGlowTextures[ phase ] )
	{
		r3dOutToLog( "Unloading sky texture: %s\n", StaticSkyGlowTextureNames[ phase ].c_str() );
		r3dRenderer->DeleteTexture( StaticSkyGlowTextures[ phase ] );

		StaticSkyGlowTextures[ phase ] = NULL;
	}
}

void r3dAtmosphere::ReloadTextures()
{
	for( int i = 0, e = SKY_PHASE_COUNT; i < e; i ++ )
	{
		atmoReloadTexture( StaticSkyTextures[ i ] );
		atmoReloadTexture( StaticSkyGlowTextures[ i ] );
	}
}

void r3dAtmosphere::Update()
{
	if( RainParticleSystem )
	{
		RainParticleSystem->SetPosition( gCam ) ;
	}

	UpdateTextures();
}

void r3dAtmosphere::UpdateTextures()
{
	SkyPhase phase0, phase1;
	float lerpT;

	GetAdjecantSkyPhasesAndLerpT( &phase0, &phase1, &lerpT );

	SkyPhase phase2 = GetNextSkyPhase( phase0, phase1 );

	LoadSkyTextures( phase0 );
	LoadSkyTextures( phase1 );

	int deleteStart = -1;

	if( lerpT > 0.75f )	
	{
		LoadSkyTextures( phase2 );
		deleteStart = GetNextSkyPhase( phase2 );
	}
	else
	{
		deleteStart = GetNextSkyPhase( phase1 );
	}

	for( int i = deleteStart; i != phase0; i = GetNextSkyPhase( SkyPhase( i ) ) )
	{
		DeleteSkyTextures( SkyPhase( i ) );
	}
}

void r3dAtmosphere::SetRainParticle( const char* Name ) 
{
	if( RainParticleSystem )
	{
		GameObject* deleteMe = RainParticleSystem;

		// avoid OnParticleSystemDelete firing
		RainParticleSystem = 0;

		GameWorld().DeleteObject( deleteMe );
	}

	if( RainParticleSystemName != Name )
	{
		strcpy( RainParticleSystemName, Name ) ;
	}

	if( !stricmp( RainParticleSystemName, PT_EMPTY_STR ) || !strlen( RainParticleSystemName ) )
	{
		return ;
	}

	RainParticleSystem = srv_CreateGameObject( "obj_ParticleSystem", Name, gCam ) ;

	// have to load/save separately
	RainParticleSystem->bPersistent = false ;
	RainParticleSystem->SetObjFlags(OBJFLAG_SkipCastRay);

	obj_ParticleSystem* gem = static_cast<obj_ParticleSystem*>( RainParticleSystem ) ;
	gem->bKeepAlive		= true ;
	gem->bKillDelayed	= false ;
	gem->bKill			= false ;
}

void r3dAtmosphere::ClearRainParticle()
{
	SetRainParticle( PT_EMPTY_STR ) ;
}

void r3dAtmosphere::OnParticleSystemDelete( obj_ParticleSystem* system )
{
	if( system == RainParticleSystem )
	{
		strcpy( RainParticleSystemName, PT_EMPTY_STR );
		RainParticleSystem = NULL;
	}
}

r3dAtmosphere::SkyPhase GetNextSkyPhase( r3dAtmosphere::SkyPhase prevPhase, r3dAtmosphere::SkyPhase currPhase )
{
	switch( currPhase )
	{
	case r3dAtmosphere::SKY_PHASE_DAWN:
		return r3dAtmosphere::SKY_PHASE_DAY;
	case r3dAtmosphere::SKY_PHASE_DAY:
		if( prevPhase == r3dAtmosphere::SKY_PHASE_DAWN )
			return r3dAtmosphere::SKY_PHASE_DAY;
		else
			return r3dAtmosphere::SKY_PHASE_DUSK;
	case r3dAtmosphere::SKY_PHASE_DUSK:
		return r3dAtmosphere::SKY_PHASE_NIGHT;
	case r3dAtmosphere::SKY_PHASE_NIGHT:
		if( prevPhase == r3dAtmosphere::SKY_PHASE_DUSK )
			return r3dAtmosphere::SKY_PHASE_NIGHT;
		else
			return r3dAtmosphere::SKY_PHASE_DAWN;
	}

	r3d_assert( false && "GetNextSkyPhase: unknown sky phase!" );

	return r3dAtmosphere::SkyPhase( 0 );
}

r3dAtmosphere::SkyPhase GetNextSkyPhase( r3dAtmosphere::SkyPhase currPhase )
{
	switch( currPhase )
	{
	case r3dAtmosphere::SKY_PHASE_DAWN:
		return r3dAtmosphere::SKY_PHASE_DAY;
	case r3dAtmosphere::SKY_PHASE_DAY:
		return r3dAtmosphere::SKY_PHASE_DUSK;
	case r3dAtmosphere::SKY_PHASE_DUSK:
		return r3dAtmosphere::SKY_PHASE_NIGHT;
	case r3dAtmosphere::SKY_PHASE_NIGHT:
		return r3dAtmosphere::SKY_PHASE_DAWN;
	}

	r3d_assert( false && "GetNextSkyPhase: unknown sky phase!" );

	return r3dAtmosphere::SkyPhase( 0 );
}

void GetAdjecantSkyPhasesAndLerpT(	r3dAtmosphere::SkyPhase *oPhase0, 
									r3dAtmosphere::SkyPhase *oPhase1,
									float* oLerpT )
{
	const r3dAtmosphere* atmo = r3dGameLevel::Environment;

	float t = atmo->__CurTime;

	float midDawn = ( atmo->DawnStart + atmo->DawnEnd ) * 0.5f;
	float midDusk = ( atmo->DuskStart + atmo->DuskEnd ) * 0.5f;

	if( t >= atmo->DuskEnd || t < atmo->DawnStart )
	{
		*oPhase0		= r3dAtmosphere::SKY_PHASE_NIGHT;
		*oPhase1		= r3dAtmosphere::SKY_PHASE_NIGHT;

		float nightLen = 24.f - atmo->DuskEnd + atmo->DawnStart;

		*oLerpT			= t  >= atmo->DuskEnd ? 
							( t - atmo->DuskEnd ) / nightLen
								:
							( t + 24.0f - atmo->DuskEnd ) / nightLen;
	}
	else
	if( t >= atmo->DawnStart && t < midDawn )
	{
		*oPhase0		= r3dAtmosphere::SKY_PHASE_NIGHT;
		*oPhase1		= r3dAtmosphere::SKY_PHASE_DAWN;
		*oLerpT			= ( t - atmo->DawnStart ) / ( midDawn - atmo->DawnStart );
	}
	else
	if( t >= midDawn && t < atmo->DawnEnd )
	{
		*oPhase0		= r3dAtmosphere::SKY_PHASE_DAWN;
		*oPhase1		= r3dAtmosphere::SKY_PHASE_DAY;
		*oLerpT			= ( t - midDawn ) / ( atmo->DawnEnd - midDawn );
	}
	else
	if( t >= atmo->DawnEnd && t < atmo->DuskStart )
	{
		*oPhase0		= r3dAtmosphere::SKY_PHASE_DAY;
		*oPhase1		= r3dAtmosphere::SKY_PHASE_DAY;
		*oLerpT			= ( t - atmo->DawnEnd ) / ( atmo->DuskStart - atmo->DawnEnd );
	}
	else
	if( t >= atmo->DuskStart && t < midDusk )
	{
		*oPhase0		= r3dAtmosphere::SKY_PHASE_DAY;
		*oPhase1		= r3dAtmosphere::SKY_PHASE_DUSK;
		*oLerpT			= ( t - atmo->DuskStart ) / ( midDusk - atmo->DuskStart );
	}
	else
	if( t >= midDusk && t < atmo->DuskEnd )
	{
		*oPhase0		= r3dAtmosphere::SKY_PHASE_DUSK;
		*oPhase1		= r3dAtmosphere::SKY_PHASE_NIGHT;
		*oLerpT			= ( t - midDusk ) / ( atmo->DuskEnd - midDusk );
	}
}

const char* SkyPhaseToName( r3dAtmosphere::SkyPhase phase )
{
	switch( phase )
	{
	case r3dAtmosphere::SKY_PHASE_DAWN:
		return "Dawn";
		break;
	case r3dAtmosphere::SKY_PHASE_DAY:
		return "Day";
		break;
	case r3dAtmosphere::SKY_PHASE_DUSK:
		return "Dusk";
		break;
	case r3dAtmosphere::SKY_PHASE_NIGHT:
		return "Night";
		break;
	}

	return "Darkness";
}


r3dPoint3D GetMoonVec()
{
	float radLatitude = R3D_DEG2RAD( r3dGameLevel::Environment->MoonLatitude );
	float redLongitude = R3D_DEG2RAD( r3dGameLevel::Environment->MoonLongitude );

	r3dPoint3D dir;

	dir.x = cosf( radLatitude ) * sinf( redLongitude );
	dir.y = sinf( radLatitude );
	dir.z = cosf( radLatitude ) * cosf( redLongitude );

	return dir;
}

R3D_FORCEINLINE float GetDayNightLerpT()
{
	float t = r3dGameLevel::Environment->__CurTime;

	const r3dAtmosphere* atmo = r3dGameLevel::Environment;

	float lerpT = 1.f;

	if( t < atmo->DuskStart && t >= atmo->DawnEnd )
	{
		lerpT = 0.f;
	}
	else
	if( t >= atmo->DawnStart && t < atmo->DawnEnd )
	{
		lerpT	= 1.0f - ( t - atmo->DawnStart ) / ( atmo->DawnEnd - atmo->DawnStart );
	}
	else
	if( t >= atmo->DuskStart && t < atmo->DuskEnd )
	{
		lerpT	= ( t - atmo->DuskStart ) / ( atmo->DuskEnd - atmo->DuskStart );
	}

	return lerpT;
}

bool IsNight()
{
	float lerpT = GetDayNightLerpT();

	return lerpT > 0.999999f ;
}

r3dPoint3D GetEnvLightDir()
{
	r3dPoint3D moonVec = -GetMoonVec();

	r3dPoint3D sunVec( 0, -1, 0 );

	if( Sun )
	{
		sunVec = -Sun->GetCurrentSunVec();
	}

	float lerpT = GetDayNightLerpT();

	if( lerpT < 0.999999f )
		lerpT = 0.f;

	r3dPoint3D dir = R3D_LERP( sunVec, moonVec, lerpT );

	dir.Normalize();

	return dir;
}

//------------------------------------------------------------------------

int g_OverrideAmbientAndIntensity;
float g_OverrideIntensity;
r3dColor g_OverrideAmbientColor;

//------------------------------------------------------------------------

float4 GetEnvLightColor()
{
	float moonT = EnvGetNightT();

	r3dAtmosphere* atmo = r3dGameLevel::Environment;

	r3dColor moonColr = atmo->GetCurrentMoonLightColor();
	r3dColor sunColor = r3dColor::white;

	if( Sun )
	{
		sunColor = atmo->GetCurrentSunColor();
	}

	float lerpT = GetDayNightLerpT();

	r3dPoint3D sunColorV( sunColor.R / 255.f, sunColor.G / 255.f, sunColor.B / 255.f );

	sunColorV *= g_OverrideAmbientAndIntensity ? g_OverrideIntensity : atmo->SunIntensity;

	r3dPoint3D moonColrV( moonColr.R / 255.f, moonColr.G / 255.f, moonColr.B / 255.f );

	moonColrV *= g_OverrideAmbientAndIntensity ? g_OverrideIntensity : atmo->MoonIntensity;

	r3dPoint3D colorMix	= R3D_LERP( sunColorV, moonColrV, lerpT );

	float4 res;
	res.x = colorMix.x;
	res.y = colorMix.y;
	res.z = colorMix.z;
	res.w = 1;

	return res;
}

float4 GetEnvLightAmbient()
{
	if( g_OverrideAmbientAndIntensity )
		return float4( g_OverrideAmbientColor.R / 255.0f, g_OverrideAmbientColor.G / 255.0f, g_OverrideAmbientColor.B / 255.0f, 1.0f );

	r3dAtmosphere* atmo = r3dGameLevel::Environment;

	r3dColor moonAmbi = atmo->GetCurrentMoonAmbientColor();
	r3dColor sunAmbi = r3dColor::black;

	if( Sun )
	{
		sunAmbi = atmo->GetCurrentSunAmbientColor();
	}

	float lerpT = GetDayNightLerpT();

	r3dPoint3D sunAmbiV( sunAmbi.R / 255.f, sunAmbi.G / 255.f, sunAmbi.B / 255.f );
	//[ST] - not sure this is right spot. 
	// Vlad can you check this out - because it's nto working right - color get washed out to white, change satyurationh, etc
	//sunAmbiV *=powf(atmo->SkyIntensity, 1/2.2);

	r3dPoint3D moonAmbiV( moonAmbi.R / 255.f, moonAmbi.G / 255.f, moonAmbi.B / 255.f );

	r3dPoint3D ambiMix = R3D_LERP( sunAmbiV, moonAmbiV, lerpT );

	float4 res;

	res.x = ambiMix.x;
	res.y = ambiMix.y;
	res.z = ambiMix.z;
	res.w = R3D_LERP(atmo->SkyIntensity,1.0f,lerpT);

	return res;
}

//------------------------------------------------------------------------

r3dColor GetEnvBackLightColor()
{
	r3dColor backLightColor = r3dGameLevel::Environment->GetCurrentBacklightColor();

	r3dPoint3D backLightColorV ( backLightColor.R, backLightColor.G, backLightColor.B );
	r3dPoint3D blackColorV ( 0, 0, 0 );

	r3dPoint3D lerpedColorV = R3D_LERP( backLightColorV, blackColorV, GetDayNightLerpT() );

	return r3dColor(	R3D_MAX( R3D_MIN( (int)lerpedColorV.x, 255 ), 0 ),
						R3D_MAX( R3D_MIN( (int)lerpedColorV.y, 255 ), 0 ),
						R3D_MAX( R3D_MIN( (int)lerpedColorV.z, 255 ), 0 ) );
						
}

//------------------------------------------------------------------------

float GetEnvSkyIntensity()
{
	r3dAtmosphere::SkyPhase phase0, phase1;
	float lerpT;

	GetAdjecantSkyPhasesAndLerpT( &phase0, &phase1, &lerpT );

	return R3D_LERP(	r3dGameLevel::Environment->StaticSkyIntensities[ phase0 ], 
						r3dGameLevel::Environment->StaticSkyIntensities[ phase1 ],
						lerpT ) ;
}

//------------------------------------------------------------------------
