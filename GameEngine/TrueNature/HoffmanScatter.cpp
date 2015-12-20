#include "r3dPCH.h"
#include "r3d.h"

#include "HoffmanScatter.h"
#include "GameLevel.h"


HoffmanScatter hoffman;


int BaseVC = 0;

extern	void		r3dSetVshC(HoffmanScatter::VSConst iReg, const r3dPoint3D& v, float f4 = 1.0f);
extern	void		r3dSetVshC(HoffmanScatter::VSConst iReg, const D3DXVECTOR4& v);
extern	void		r3dSetPshC(HoffmanScatter::PSConst iReg, const r3dPoint3D& v, float f4 = 1.0f);

void r3dSetVshC( HoffmanScatter::VSConst iReg, const r3dPoint3D& v, float f4)
{
  float f[4];
  f[0] = v.X;
  f[1] = v.Y;
  f[2] = v.z;
  f[3] = f4;
  r3dRenderer->pd3ddev->SetVertexShaderConstantF(iReg, (const float *)&f[0], 1);
}

void r3dSetVshC( HoffmanScatter::VSConst iReg, const D3DXVECTOR4& v)
{
  r3dRenderer->pd3ddev->SetVertexShaderConstantF(iReg, (const float *)&v, 1);
}

void r3dSetPshC( HoffmanScatter::PSConst iReg, const r3dPoint3D& v, float f4)
{
  float f[4];
  f[0] = v.X;
  f[1] = v.Y;
  f[2] = v.z;
  f[3] = f4;
  r3dRenderer->pd3ddev->SetPixelShaderConstantF(iReg, (const float *)&f[0], 1);
}

void r3dSetPshC( HoffmanScatter::PSConst iReg, const D3DXVECTOR4& v)
{
  r3dRenderer->pd3ddev->SetPixelShaderConstantF(iReg, (const float *)&v, 1);
}



static void logInfo(const char* s1, const char* s2, const char* s3)
{
  r3dOutToLog("logInfo: %s, %s, %s\n", s1, s2, s3);
}

static void logError(const char* s1, const char* s2, const char* s3)
{
  r3dError("logError: %s, %s, %s\n", s1, s2, s3);
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
    const float n = 1.0003f;
    const float N = 2.545e25f;
    const float pn = 0.035f;

HoffmanScatter::HoffmanScatter()
: m_HGg				( 0.9993f )
, m_inscatteringMultiplier( 1.0f )
, m_betaRayMultiplier( 4.0f )
, m_betaMieMultiplier(0.0006f )
, m_sunIntensity	( 1.0f )
, m_turbitity		( 1.0f )
, m_fCloudsScale	( 1.0f )
, m_fCloudDensity	( 0.5f )
, m_fSkySunsetBumpness ( 20.0f )
, m_fSkySunsetOffset ( 0.0f )
, m_fSkySunsetPower ( 1.5f )
, m_fSkySunsetStartCoef ( 0.3f )
, m_fSkyCloudsFadeStart ( 2000.0f )
, m_fSkyCloudsFadeEnd ( 1000.0f )
, m_cgSkyFragmentShader_Normals( -1 )
, m_cgSkyFragmentShader_NormalsLQ( -1 )
, m_cgSkyFragmentShader( -1 )
, m_cgSkyFragmentShaderLQ( -1 )

{
    float fLambda[3], fLambda2[3], fLambda4[3];

    fLambda[0]  = 1.0f / 650e-9f;   // red
    fLambda[1]  = 1.0f / 570e-9f;   // green
    fLambda[2]  = 1.0f / 475e-9f;   // blue



    for( int i = 0; i < 3; ++i )
    {
        fLambda2[i] = fLambda[i]*fLambda[i];
        fLambda4[i] = fLambda2[i]*fLambda2[i];
    }

    r3dPoint3D vLambda2( fLambda2[0], fLambda2[1], fLambda2[2] ); 
    r3dPoint3D vLambda4( fLambda4[0], fLambda4[1], fLambda4[2] ); 

    // Rayleigh scattering constants

    const float fTemp = R3D_PI * R3D_PI * (n * n - 1.0f) * (n * n - 1.0f) * (6.0f + 3.0f * pn) / (6.0f - 7.0f * pn) / N;
    const float fBeta = 8.0f * fTemp * R3D_PI / 3.0f;
 
    m_betaRay = fBeta * vLambda4;

    const float fBetaDash = fTemp / 2.0f;

    m_betaDashRay = fBetaDash * vLambda4;

    // Mie scattering constants

    const float T       = 2.0f;
    const float c       = (6.544f * T - 6.51f) * 1e-17f;
    const float fTemp2  = 0.434f * c * (2.0f * R3D_PI) * (2.0f* R3D_PI) * 0.5f;

    m_betaDashMie = fTemp2 * vLambda2;

    const float K[3]    = {0.685f, 0.679f, 0.670f};
    const float fTemp3  = 0.434f * c * R3D_PI * (2.0f * R3D_PI) * (2.0f * R3D_PI);

    r3dPoint3D vBetaMieTemp( K[0] * fLambda2[0], K[1] * fLambda2[1], K[2] * fLambda2[2] );

    m_betaMie = fTemp3 * vBetaMieTemp;

	m_fSkyIntensity = 1.0f;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
HoffmanScatter::~HoffmanScatter()
{
}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
void HoffmanScatter::create()
{
    // Create sky and terrain shader programs

    m_validSkyLight = true;
    m_validAerialPerspective = true;

    m_cgVertexShader            = r3dRenderer->GetVertexShaderIdx( "VS_SKYHOFFMAN" );

	m_cgSkyFragmentShader       = r3dRenderer->GetPixelShaderIdx( "PS_SKYHOFFMAN" );
    m_cgSkyFragmentShaderLQ		= r3dRenderer->GetPixelShaderIdx( "PS_SKYHOFFMAN_LQ" );

	m_cgSkyFragmentShader_Normals	= r3dRenderer->GetPixelShaderIdx( "PS_SKYHOFFMAN_W_NORMALS" );
	m_cgSkyFragmentShader_NormalsLQ = r3dRenderer->GetPixelShaderIdx( "PS_SKYHOFFMAN_W_NORMALS_LQ" );

    // Log success

    if( true == m_validSkyLight )
	{
		// logInfo( "HoffmanScatter", "create", "Skylight shader created successfully." );
	}
    else
        logError( "HoffmanScatter", "create", "Skylight shader creation failed." );

    if( true == m_validAerialPerspective )
	{
		// logInfo( "HoffmanScatter", "create", "Aerial perspective shader created successfully." );
	}
    else
        logError( "HoffmanScatter", "create", "Aerial perspective shader creation failed." );
}

//-----------------------------------------------------------------------------
//  setSunIntensity
//-----------------------------------------------------------------------------
void HoffmanScatter::setSunIntensity( const float a_value )
{
    m_sunIntensity = a_value;
}

//-----------------------------------------------------------------------------
//  setMie
//-----------------------------------------------------------------------------
void HoffmanScatter::setMie( const float a_value )
{
    m_betaMieMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setRayleigh
//-----------------------------------------------------------------------------
void HoffmanScatter::setRayleigh( const float a_value )
{
    m_betaRayMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setHg
//-----------------------------------------------------------------------------
void HoffmanScatter::setHg( const float a_value )
{
    m_HGg = a_value;
}

//-----------------------------------------------------------------------------
//  setInscattering
//-----------------------------------------------------------------------------
void HoffmanScatter::setInscattering( const float a_value )
{
    m_inscatteringMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setTurbidity
//-----------------------------------------------------------------------------
void HoffmanScatter::setTurbidity( const float a_value )
{
    m_turbitity = a_value;
}

//-----------------------------------------------------------------------------
//  isSkyLightValid
//-----------------------------------------------------------------------------
const bool HoffmanScatter::isSkyLightValid() const
{
    return m_validSkyLight;
}

//-----------------------------------------------------------------------------
//  isAerialPerspectiveValid
//-----------------------------------------------------------------------------
const bool HoffmanScatter::isAerialPerspectiveValid() const
{
    return m_validAerialPerspective;
}

//-----------------------------------------------------------------------------
//  renderSkyLight
//-----------------------------------------------------------------------------
extern bool g_bSkyDomeNeedFullUpdate;

void HoffmanScatter::renderSkyLight( const r3dCamera& a_camera
                                   , const r3dPoint3D& a_sunDirection
								   , const r3dPoint3D& tFog_CloudAnim_Params
                                   , r3dColor fogColor
								   , r3dPoint2D tCloudsDim
								   , bool normals
								   , float mie
								   , float angle
								   , float angle_range
								   , bool setShaders
								   )
{
    // Compute thetaS dependencies

	r3dPoint3D vecZenith( 0.0f, 1.0f, 0.0f );

	float   fThetaO = a_sunDirection.Dot(vecZenith);
			fThetaO = acosf( fThetaO );

	float   fThetaS = fabs( ( angle + ( angle_range - 180.f ) * 0.5f ) / angle_range - 0.5f ) * float( M_PI ) ;

	static float fOldTheta = -fThetaS;

	if(fOldTheta != fThetaS || g_bSkyDomeNeedFullUpdate)
	{
		computeAttenuation(fThetaS, angle_range);
		fOldTheta = fThetaS;
	}
	

	// Set shader constants

	setShaderConstants( a_camera, a_sunDirection, tFog_CloudAnim_Params, fogColor, tCloudsDim, mie, m_fSkyIntensity );

	// Enable programs

	if( setShaders )
	{
		r3dRenderer->SetVertexShader(m_cgVertexShader);

		if( normals )
		{
			if(r_environment_quality->GetInt()==1)
				r3dRenderer->SetPixelShader(m_cgSkyFragmentShader_NormalsLQ);
			else
				r3dRenderer->SetPixelShader(m_cgSkyFragmentShader_Normals);
		}
		else
		{
			if(r_environment_quality->GetInt()==1)
				r3dRenderer->SetPixelShader(m_cgSkyFragmentShaderLQ);
			else
				r3dRenderer->SetPixelShader(m_cgSkyFragmentShader);
		}
	}

	// Draw sky dome
	//!glDrawElements          ( GL_TRIANGLE_STRIP, a_indexCount, GL_UNSIGNED_INT, a_pIndices );

	// Disable programs
	//r3dRenderer->SetVertexShader();
	//r3dRenderer->SetPixelShader();
}

//-----------------------------------------------------------------------------
//  renderAerialPerspective
//-----------------------------------------------------------------------------
void HoffmanScatter::renderAerialPerspective( const r3dCamera& a_camera
                                            , const r3dPoint3D& a_sunDirection
											, const r3dPoint3D& tFog_CloudAnim_Params
                                            , r3dColor fogColor
											, r3dPoint2D tCloudsDim
											, float mie
											, float angle
											, float angle_range
											)
{
    // Compute thetaS dependencies

	float   fThetaS = fabs( ( angle + ( angle_range - 180.f ) * 0.5f ) / angle_range - 0.5f ) * float( M_PI ) ;

    computeAttenuation( fThetaS, angle_range );

    // Set shader constants
    setShaderConstants( a_camera, a_sunDirection, tFog_CloudAnim_Params, fogColor, tCloudsDim, mie, m_fSkyIntensity );

    r3dSetPshC(PSC_LightVector, a_sunDirection);
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Private methods
//*********************************************************************************************************************
//*********************************************************************************************************************

	float fLambda[3] = {1.00f, 0.8f, 1.0f }; 
	

//-----------------------------------------------------------------------------
//  computeAttenuation
//-----------------------------------------------------------------------------
void HoffmanScatter::computeAttenuation( const float a_theta, float angle_range )
{
    float fBeta = 0.04608365822050f * m_turbitity - 0.04586025928522f;
    float fTauR, fTauA, fTau[3];
    float m = 1.0f / (cosf( a_theta ) + 0.15f * powf( 93.885f - a_theta / angle_range * 180.f / R3D_PI * 180.0f, -1.253f ));  // Relative Optical Mass

	//float fLambda[3] = {0.65f, 0.57f, 0.475f }; 
	
    for( int i = 0; i < 3; ++i )
    {
        // Rayleigh Scattering
        // lambda in um.

        fTauR = expf( -m * 0.008735f * powf( fLambda[i], float(-4.08f) ) );

        // Aerosal (water + dust) attenuation
        // beta - amount of aerosols present 
        // alpha - ratio of small to large particle sizes. (0:4,usually 1.3)

        const float fAlpha = 1.3f;
		float p = powf(fLambda[i], -fAlpha);
        fTauA = expf(-m * fBeta * p);  // lambda should be in um
		//	Prevent INF * 0, which is NAN. ATI cards more strict about IEEE754 conformance, and NAN in shader produces visual artifacts
		fTauA = std::min(fTauA, std::numeric_limits<float>::max());

        fTau[i] = fTauR * fTauA; 

    }

    m_sunColorAndIntensity = D3DXVECTOR4( fTau[0], fTau[1], fTau[2], m_sunIntensity * 100.0f );
}

//-----------------------------------------------------------------------------
//  setShaderConstants
//-----------------------------------------------------------------------------

namespace
{
	float saturate( float a )
	{
		return R3D_MAX( R3D_MIN( a, 1.f), 0.f );
	}
}

void HoffmanScatter::setShaderConstants( const r3dCamera& a_camera
                                       , const r3dPoint3D& a_sunDirection
									   , const r3dPoint3D& tFog_CloudAnim_Params
									   , r3dColor fogColor
									   , r3dPoint2D tCloudsDim
									   , float mieScale
									   , float amplify
                                       )
{
    float fReflectance = 0.1f;

    r3dPoint3D vecBetaR           ( m_betaRay * m_betaRayMultiplier );
    r3dPoint3D vecBetaDashR       ( m_betaDashRay * m_betaRayMultiplier );
    r3dPoint3D vecBetaM           ( m_betaMie * m_betaMieMultiplier );
    r3dPoint3D vecBetaDashM       ( m_betaDashMie * m_betaMieMultiplier );
    r3dPoint3D vecBetaRM          ( vecBetaR + vecBetaM );
    r3dPoint3D vecOneOverBetaRM   ( 1.0f / vecBetaRM.X, 1.0f / vecBetaRM.Y, 1.0f / vecBetaRM.Z );
    r3dPoint3D vecG               ( 1.0f - m_HGg * m_HGg, 1.0f + m_HGg * m_HGg, 2.0f * m_HGg );
    D3DXVECTOR4 vecTermMulitpliers ( m_inscatteringMultiplier, 0.138f * fReflectance, 0.113f * fReflectance, 0.08f * fReflectance );
	
	r3dSetVshC( VSC_EyePos,                a_camera, 1.0f / ( tFog_CloudAnim_Params.x - tFog_CloudAnim_Params.y ) );
    r3dSetVshC( VSC_SunDir,                a_sunDirection, tFog_CloudAnim_Params.y );
    r3dSetVshC( VSC_BetaRPlusBetaM,        vecBetaRM );
    r3dSetVshC( VSC_HGg,                   vecG );
    r3dSetVshC( VSC_BetaDashR,             vecBetaDashR );
    r3dSetVshC( VSC_BetaDashM,             vecBetaDashM );
    r3dSetVshC( VSC_OneOverBetaRPlusBetaM, vecOneOverBetaRM );

    r3dSetVshC( VSC_Multipliers,           vecTermMulitpliers);
    r3dSetVshC( VSC_SunColorAndIntensity,  m_sunColorAndIntensity);

    r3dSetPshC( PSC_EyePos,                  a_camera, tFog_CloudAnim_Params.z );
    r3dSetPshC( PSC_SunDir,                  a_sunDirection, m_fCloudsScale / 10000.0f);
    r3dSetPshC( PSC_BetaRPlusBetaM,          vecBetaRM, saturate( ( m_fCloudDensity - 0.5f ) * 2 ) );
    r3dSetPshC( PSC_HGg,                     vecG, saturate( m_fCloudDensity * 2.f ) );
    r3dSetPshC( PSC_BetaDashR,               vecBetaDashR );
    r3dSetPshC( PSC_BetaDashM,               vecBetaDashM );
    r3dSetPshC( PSC_OneOverBetaRPlusBetaM,   vecOneOverBetaRM );

    r3dSetPshC( PSC_Multipliers,           vecTermMulitpliers);
    r3dSetPshC( PSC_SunColorAndIntensity,  m_sunColorAndIntensity);


    r3dSetPshC( PSC_SunsetParams,D3DXVECTOR4 (m_fSkySunsetBumpness, m_fSkySunsetOffset, m_fSkySunsetPower, m_fSkySunsetStartCoef) );
    r3dSetPshC( PSC_SunsetCloudsParams, D3DXVECTOR4 (r3dGameLevel::Environment->SkyFog_Start, r3dGameLevel::Environment->SkyFog_End, 1.0f/tCloudsDim.x, 1.0f/tCloudsDim.y) );

	float mieScaleA[4] = {mieScale, 0.0f, 0.0f, 0.0f};
	
	r3dSetPshC( PSC_MieScale, mieScaleA ) ;

	float amplifyA[4] = {amplify, 0.0f, 0.0f, 0.0f } ;

	r3dSetPshC( PSC_Amplify, amplifyA ) ;

}
