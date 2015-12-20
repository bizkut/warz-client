#pragma once

#include "TimeGradient.h"
#include "r3dProtect.h"

class r3dAtmosphere
{
  public:
	r3dSec_type<float, 0x384778fa> __CurTime;

	enum SkyPhase
	{
		SKY_PHASE_DAWN,
		SKY_PHASE_DAY,
		SKY_PHASE_DUSK,
		SKY_PHASE_NIGHT,
		SKY_PHASE_COUNT
	};

	typedef r3dTL::TFixedArray< r3dTexture*, SKY_PHASE_COUNT > SkyPhaseTextures;
	typedef r3dTL::TFixedArray< r3dString, SKY_PHASE_COUNT > SkyPhaseTextureNames;
	typedef r3dTL::TFixedArray< float, SKY_PHASE_COUNT > SkyPhaseFloats;
	typedef r3dTL::TFixedArray< int, SKY_PHASE_COUNT > TextureLoadedFlags;

	SkyPhaseTextures		StaticSkyTextures;
	SkyPhaseTextures		StaticSkyGlowTextures;

	SkyPhaseTextureNames	StaticSkyTextureNames;
	SkyPhaseTextureNames	StaticSkyGlowTextureNames;

	SkyPhaseFloats			StaticSkyIntensities;

	r3dMesh*			StaticSkyMesh;

	r3dString			StaticSkyMeshName;

	float				ParticleShadingCoef;

	float				StaticTexGenScaleX;
	float				StaticTexGenScaleY;

	float				StaticTexGetOffsetX;
	float				StaticTexGetOffsetY;

	int					bStaticSkyEnable;
	int					bCustomStaticMeshEnable;
	int					bStaticSkyPlanarMapping;

	int					SunLightOn;

	float				DawnStart;
	float				DawnEnd;

	float				DuskStart;
	float				DuskEnd;

	r3dTimeGradient2	SunColor;
	float				SunIntensity;
	r3dColor			GetCurrentSunColor();

	r3dTimeGradient2	SunAmbientColor;
	float				SkyIntensity;

	r3dColor			GetCurrentSunAmbientColor();

	r3dTimeGradient2	LambdaCol;
	r3dColor			GetCurrentLambdaColor();

	r3dTimeGradient2	BacklightColor;
	r3dColor			GetCurrentBacklightColor();

	float				BacklightIntensity;

	int					bVolumeFog;
	int					FogVersion;

	r3dTimeGradient2 	Fog_Color;

	float				Fog_MaxHeight;
	r3dTimeGradient2	Fog_HStart;
	r3dTimeGradient2	Fog_HRange;

	r3dTimeGradient2	Fog_SkydomeMult1; //= 50.0f;
	r3dTimeGradient2	Fog_SkydomeMult2;// = 300.0f;

	r3dTimeGradient2	Fog_CameraDist;
	r3dTimeGradient2	Fog_CameraRange;
	r3dTimeGradient2	Fog_CameraExp;
	r3dTimeGradient2	Fog_MaxDensity;


	float				Aerial_MipBias;
	r3dTimeGradient2 	Aerial_Density;
	r3dTimeGradient2 	Aerial_Distance;


	r3dTimeGradient2 	Fog_Distance;
	r3dTimeGradient2 	Fog_Height;
	r3dTimeGradient2 	Fog_Range;
	r3dTimeGradient2 	Fog_HeightFadeStart;
	r3dTimeGradient2 	Fog_HeightFadeRange;
	r3dTimeGradient2 	Fog_Density;

	float				Fog2_MaxHeight;

	float				SkyFog_Start;
	float				SkyFog_End;

	r3dTimeGradient2 	Fog2_Distance;
	r3dTimeGradient2 	Fog2_HeightFadeStart;
	r3dTimeGradient2 	Fog2_HeightFadeRange;
	r3dTimeGradient2 	Fog2_Density;




	r3dTimeGradient2 	HGg;
	float				GetCurrentHGg();

	r3dTimeGradient2 	InscatteringMultiplier;
	float				GetCurrentInscatteringMultiplier();

	r3dTimeGradient2 	BetaRayMultiplier;
	float				GetCurrentBetaRayMultiplier();

	r3dTimeGradient2 	BetaMieMultiplier;
	float				GetCurrentBetaMieMultiplier();

	r3dTimeGradient2 	SunIntensityCoef;
	float				GetCurrentSunIntensityCoef();

	r3dTimeGradient2 	Turbitity;
	float				GetCurrentTurbitity();

	r3dTimeGradient2	SunSpotAmplify;
	r3dTimeGradient2	SunSpotPow;
	r3dTimeGradient2	SunSpotColor;
	r3dTimeGradient2	SunSpotIntensity;

	float				SkyDomeWindFactor;
	float				SkyDomeCloudsScale;
	float				SkyDomeCloudsDensity;

	float				SkySunsetBumpness;
	float				SkySunsetOffset;
	float				SkySunsetPower;
	float				SkySunsetStartCoef;
	float				SkyCloudsFadeStart;
	float				SkyCloudsFadeEnd;

	float				CloudFadeOutStartDist;
	float				CloudFadeOutEndDist;
	float				SunElevationAngle;
	float				SunDirectionAngle;

	float				SunMovementAngleRange;

	float				MoonLongitude;
	float				MoonLatitude;
	
	r3dTimeGradient2	MoonLightColor;
	float				MoonIntensity;

	r3dColor			GetCurrentMoonLightColor();

	r3dTimeGradient2	MoonAmbientColor;
	r3dColor			GetCurrentMoonAmbientColor();

	float EnvGetDayT();
	float EnvGetNightT();

	float				SSSDiffuse;
	float				SSSSpecular;
	float				SSSSpecPower;

	float				SkyDomeRotationY;

	class GameObject*	RainParticleSystem;
	char				RainParticleSystemName[ 64 ];

  public:
	r3dAtmosphere()
	: StaticSkyMesh( 0 )
	, RainParticleSystem( 0 )
	, FogVersion( 0 )
	{
		Reset();
	}


#ifdef WO_SERVER
	void Reset() {};
#else
	void Reset();
#endif


	~r3dAtmosphere() {};

  template < bool Write >
  int	SerializeXML( pugi::xml_node root );

  int	LoadFromXML( pugi::xml_node root );
  int	SaveToXML( pugi::xml_node root );

  void  ReloadTextures();
  void	Update();

  void UpdateTextures();

  void	SetRainParticle( const char* Name );
  void	ClearRainParticle();

  void	OnParticleSystemDelete( class obj_ParticleSystem* system );

  void	EnableStaticSky();
  void	DisableStaticSky();
  void	SetStaticSkyTextures( const SkyPhaseTextureNames& textureNames, const SkyPhaseTextureNames& glowTextureNames );
  void	SetStaticSkyMesh( const r3dString& meshName );	
  
  void	ConvertFog_V0();
  void	ConvertFog_V1();

  void	LoadSkyTextures( SkyPhase phase );
  void	DeleteSkyTextures( SkyPhase phase );
};

r3dAtmosphere::SkyPhase GetNextSkyPhase( r3dAtmosphere::SkyPhase prevPhase, r3dAtmosphere::SkyPhase currPhase );
r3dAtmosphere::SkyPhase GetNextSkyPhase( r3dAtmosphere::SkyPhase currPhase );
void GetAdjecantSkyPhasesAndLerpT( r3dAtmosphere::SkyPhase *oPhase0, r3dAtmosphere::SkyPhase *oPhase1, float* oLerpT );
const char* SkyPhaseToName( r3dAtmosphere::SkyPhase phase );

r3dPoint3D GetMoonVec();

r3dPoint3D GetEnvLightDir();
float4 GetEnvLightColor();
float4 GetEnvLightAmbient();

r3dColor GetEnvBackLightColor();

float GetEnvSkyIntensity();

bool IsNight();

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::EnvGetDayT()
{
	const r3dAtmosphere& atmo = *this;

	float t = ( atmo.__CurTime - atmo.DawnStart ) / ( atmo.DuskEnd - atmo.DawnStart );

	return R3D_CLAMP( t, 0.f, 1.f );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::EnvGetNightT()
{
	const r3dAtmosphere& atmo = *this;

	float t = 0.f;

	float L = ( 24.f - atmo.DuskEnd + atmo.DawnStart);

	if( atmo.__CurTime >= atmo.DuskEnd )
	{
		t = ( atmo.__CurTime - atmo.DuskEnd ) / L;
	}
	else
	{
		if( atmo.__CurTime < atmo.DawnStart )
		{
			t = ( atmo.__CurTime + ( 24.f - atmo.DuskEnd ) ) / L;
		}
		else
		{
			if( atmo.__CurTime < ( atmo.DawnEnd + atmo.DuskStart ) * 0.5f )
				t = 1.f;
			else
				t = 0.f;
		}
	}

	return R3D_CLAMP( t, 0.f, 1.f );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dColor r3dAtmosphere::GetCurrentSunColor()
{
	return SunColor.GetColorValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dColor r3dAtmosphere::GetCurrentSunAmbientColor()
{
	return SunAmbientColor.GetColorValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dColor r3dAtmosphere::GetCurrentLambdaColor()
{
	return LambdaCol.GetColorValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dColor r3dAtmosphere::GetCurrentBacklightColor()
{
	return BacklightColor.GetColorValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::GetCurrentHGg()
{
	return HGg.GetFloatValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::GetCurrentInscatteringMultiplier()
{
	return InscatteringMultiplier.GetFloatValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::GetCurrentBetaRayMultiplier()
{
	return BetaRayMultiplier.GetFloatValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::GetCurrentBetaMieMultiplier()
{
	return BetaMieMultiplier.GetFloatValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::GetCurrentSunIntensityCoef()
{
	return SunIntensityCoef.GetFloatValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE float r3dAtmosphere::GetCurrentTurbitity()
{
	return Turbitity.GetFloatValue( EnvGetDayT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dColor r3dAtmosphere::GetCurrentMoonLightColor()
{
	return MoonLightColor.GetColorValue( EnvGetNightT() );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dColor r3dAtmosphere::GetCurrentMoonAmbientColor()
{
	return MoonAmbientColor.GetColorValue( EnvGetNightT() );
}

extern int g_OverrideAmbientAndIntensity;
extern float g_OverrideIntensity;
extern r3dColor g_OverrideAmbientColor;
