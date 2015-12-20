//=========================================================================
//	Module: PFX_LensDirt.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"
#include "GameCommon.h"
#include "PFX_LensDirt.h"

//////////////////////////////////////////////////////////////////////////

static const float LIGHT_EFFECT_BORDER = 1.33f;

PFX_LensDirt::Settings::Settings()
: Area( 1.0f )
, GlowStrength( 1.0f )
, LightStrength( 1.0f )
, Pow( 1.0f )
{

}

//------------------------------------------------------------------------

PFX_LensDirt::PFX_LensDirt()
: Parent(this)
, m_lengthDirtTexture( NULL )
, m_LastTime( r3dGetTime() )
{
	
}

//////////////////////////////////////////////////////////////////////////

PFX_LensDirt::~PFX_LensDirt()
{

}

//------------------------------------------------------------------------

const PFX_LensDirt::Settings& PFX_LensDirt::GetSettings() const
{
	return m_Settings;
}

//------------------------------------------------------------------------

void PFX_LensDirt::SetSettings( const PFX_LensDirt::Settings& sts )
{
	m_Settings = sts;
}

//------------------------------------------------------------------------

void PFX_LensDirt::SetVPMatrix( const D3DXMATRIX& mtx )
{
	m_VPMtx = mtx;
}

//------------------------------------------------------------------------

void PFX_LensDirt::PurgeLights()
{
	m_Lights.Clear();
}

//------------------------------------------------------------------------

void PFX_LensDirt::AddLight( r3dPoint3D pos, r3dColor colr, float strength, float radius, int isDir )
{
	if( m_Lights.Count() >= LensDirtShaderId::MAX_LIGHTS )
		return;

	if( isDir && m_SunIntensity <= 0.f )
		return;

	Light light;

	light.Pos		= pos;
	light.IsDir		= isDir;
	light.Strength	= strength;
	light.Radius	= radius;
	light.Color		= colr;

	D3DXVECTOR4 d3dpos( pos.x, pos.y, pos.z, 1.0f );
	D3DXVec4Transform( &d3dpos, &d3dpos, &m_VPMtx );

	d3dpos.x /= d3dpos.w;
	d3dpos.y /= d3dpos.w;

	if( !isDir )
	{
		light.Radius /= d3dpos.w;
	}

	if( d3dpos.w > 0
			&& 
		d3dpos.x >= -LIGHT_EFFECT_BORDER && d3dpos.x <= LIGHT_EFFECT_BORDER
			&&
		d3dpos.y >= -LIGHT_EFFECT_BORDER && d3dpos.y <= LIGHT_EFFECT_BORDER )
	{
		light.ProjectedPos = r3dPoint3D( d3dpos.x, d3dpos.y, d3dpos.z );
		m_Lights.PushBack( light );	
	}
}

//------------------------------------------------------------------------

int PFX_LensDirt::IsActive() const
{
	return m_Lights.Count();
}

//------------------------------------------------------------------------

int PFX_LensDirt::IsFull() const
{
	return m_Lights.Count() >= LensDirtShaderId::MAX_LIGHTS;
}

//------------------------------------------------------------------------

void PFX_LensDirt::SetSunOn( int on )
{
	m_SunOn = on;
}

//////////////////////////////////////////////////////////////////////////

void PFX_LensDirt::InitImpl()
{
	LensDirtShaderId ldsid;
	ldsid.light_count = 0;
	mData.PixelShaderID = gLensDirtShaderIDs[ ldsid.Id ];

	m_lengthDirtTexture = r3dRenderer->LoadTexture( "Data\\Shaders\\Texture\\LenzDirt01_D.dds" );
}

//////////////////////////////////////////////////////////////////////////


void PFX_LensDirt::CloseImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_LensDirt::PrepareImpl(r3dScreenBuffer* /*dest*/, r3dScreenBuffer* src )
{
	extern r3dCamera gCam;
	extern r3dSun* Sun;

	float newTime = r3dGetTime();
	float dt = newTime - m_LastTime;

	if( m_SunOn )
		m_SunIntensity += dt * 6;
	else
		m_SunIntensity -= dt * 8;

	m_LastTime = newTime;

	m_SunIntensity = R3D_MIN( R3D_MAX( m_SunIntensity, 0.f ), 1.f );

	LensDirtShaderId ldsid;

	int numUsedLights = R3D_MIN( (int)m_Lights.Count(), (int)LensDirtShaderId::MAX_LIGHTS );

	ldsid.light_count = numUsedLights;

	mData.PixelShaderID = gLensDirtShaderIDs[ ldsid.Id ];

	float psConst[ 1 + LensDirtShaderId::MAX_LIGHTS * 2 ][ 4 ] = { 0 };
	
	// float3 vStrengthGlow_StrengthLight_Aspect_Opacity : register( c0 );
	psConst[ 0 ][ 0 ] = m_Settings.GlowStrength;
	psConst[ 0 ][ 1 ] = m_Settings.LightStrength;
	psConst[ 0 ][ 2 ] = src->Width / src->Height;
	psConst[ 0 ][ 3 ] = R3D_MIN( R3D_MAX( m_Settings.Opacity, 0.f ), 1.f );

	for( int i = 0; i < numUsedLights; i ++ )
	{
		const Light& l = m_Lights[ i ];

		float blendIn0 = 1.0f - ( l.ProjectedPos.x - 1.0f ) / ( LIGHT_EFFECT_BORDER - 1.0f );
		float blendIn1 = 1.0f + ( l.ProjectedPos.x + 1.0f ) / ( LIGHT_EFFECT_BORDER - 1.0f );
		float blendIn2 = 1.0f - ( l.ProjectedPos.y - 1.0f ) / ( LIGHT_EFFECT_BORDER - 1.0f );
		float blendIn3 = 1.0f + ( l.ProjectedPos.y + 1.0f ) / ( LIGHT_EFFECT_BORDER - 1.0f );

		blendIn0 = R3D_MAX( R3D_MIN( blendIn0, 1.0f ), 0.f );
		blendIn1 = R3D_MAX( R3D_MIN( blendIn1, 1.0f ), 0.f );
		blendIn2 = R3D_MAX( R3D_MIN( blendIn2, 1.0f ), 0.f );
		blendIn3 = R3D_MAX( R3D_MIN( blendIn3, 1.0f ), 0.f );

		// float4 vLightData[ NUM_LIGHTS ] : register( c1 );
		psConst[ i + 1 ][ 0 ] = l.ProjectedPos.x * 0.5f + 0.5f;
		psConst[ i + 1 ][ 1 ] = -l.ProjectedPos.y * 0.5f + 0.5f;
		psConst[ i + 1 ][ 2 ] = l.Radius * m_Settings.Area;
		psConst[ i + 1 ][ 3 ] = l.Strength * blendIn0 * blendIn1 * blendIn2 * blendIn3;

		if( l.IsDir )
			psConst[ i + 1 ][ 3 ] *= m_SunIntensity;

		// float3 vLightColors[ NUM_LIGHTS ] : register( c5 );
		psConst[ i + 1 + LensDirtShaderId::MAX_LIGHTS ][ 0 ] = l.Color.R / 255.0f;
		psConst[ i + 1 + LensDirtShaderId::MAX_LIGHTS ][ 1 ] = l.Color.G / 255.0f;
		psConst[ i + 1 + LensDirtShaderId::MAX_LIGHTS ][ 2 ] = l.Color.B / 255.0f;
		psConst[ i + 1 + LensDirtShaderId::MAX_LIGHTS ][ 3 ] = m_Settings.Pow;
	}

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psConst[ 0 ], sizeof psConst / sizeof psConst[ 0 ] ) );

	r3dRenderer->SetTex( m_lengthDirtTexture, PostFXChief::FREE_TEX_STAGE_START );
	r3dRenderer->SetTex( g_pPostFXChief->GetBuffer( PostFXChief::RTT_ONE16_0_64BIT )->Tex, PostFXChief::FREE_TEX_STAGE_START + 1 );

}

//////////////////////////////////////////////////////////////////////////

void PFX_LensDirt::FinishImpl()
{

}


