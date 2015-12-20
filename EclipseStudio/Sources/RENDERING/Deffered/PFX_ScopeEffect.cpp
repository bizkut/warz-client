//=========================================================================
//	Module: PFX_ScopeEffect.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"
#include "GameCommon.h"
#include "PFX_ScopeEffect.h"

#include "ObjectsCode/world/EnvmapProbes.h"

//------------------------------------------------------------------------

ScopeMaterialParams::ScopeMaterialParams()
: Metalness ( 0.22f )
, Chromeness ( 0.5f )
, SpecularPower ( 0.22f )
, Lighting( 1 )
{

}

//////////////////////////////////////////////////////////////////////////

PFX_ScopeEffect::PFX_ScopeEffect()
: Parent(this)
, m_ps_NoShadows_ID(-1)
, m_ps_NormalShadows_ID(-1)
, m_ps_HWShadows_ID(-1)
, m_ps_FilterNoLighting(-1)
, m_vsID(-1)
, m_scopeReticuleTexture(0)
, m_scopeNormalTexture(0)
, m_scopeMaskTexture(0)
, m_scopeBlurTexture(0)
, m_blurredSceneImage(0)
, m_scopeScale(1.0f)
, m_scopeOffset(0, 0)
{
	D3DXMatrixIdentity( &m_lightMtx );
}

//////////////////////////////////////////////////////////////////////////

PFX_ScopeEffect::~PFX_ScopeEffect()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_ScopeEffect::SetScopeReticle(r3dTexture *tex)
{
	m_scopeReticuleTexture = tex;
}

//------------------------------------------------------------------------

void PFX_ScopeEffect::SetScopeNormal(r3dTexture *tex)
{
	m_scopeNormalTexture = tex ;
}

//////////////////////////////////////////////////////////////////////////

void PFX_ScopeEffect::SetScopeMask(r3dTexture *tex)
{
	m_scopeMaskTexture = tex;
}

void PFX_ScopeEffect::SetScopeBlurMask(r3dTexture *tex)
{
	m_scopeBlurTexture = tex;
}
//////////////////////////////////////////////////////////////////////////

void PFX_ScopeEffect::SetBlurredSceneImage(r3dScreenBuffer *bsi)
{
	m_blurredSceneImage = bsi;
}

//------------------------------------------------------------------------

void PFX_ScopeEffect::SetLightMatrix( const D3DXMATRIX& mtx )
{
	m_lightMtx = mtx ;
}

//------------------------------------------------------------------------

void
PFX_ScopeEffect::SetScopeScale( float scale )
{
	m_scopeScale = scale ;
}

//------------------------------------------------------------------------

void
PFX_ScopeEffect::SetScopeOffset( const r3dPoint2D &uvOffset )
{
	m_scopeOffset = uvOffset;
}

//------------------------------------------------------------------------

float
PFX_ScopeEffect::GetScopeScale() const
{
	return m_scopeScale ;
}

//------------------------------------------------------------------------

void
PFX_ScopeEffect::SetMaterialParams( const ScopeMaterialParams& params )
{
	m_materialParams = params ;
}

//------------------------------------------------------------------------

ScopeMaterialParams
PFX_ScopeEffect::GetMaterialParams() const
{
	return m_materialParams ;
}

//////////////////////////////////////////////////////////////////////////

void PFX_ScopeEffect::InitImpl()
{
	m_ps_NoShadows_ID		= r3dRenderer->GetPixelShaderIdx( "PS_SCOPE_FILTER_NO_SHADOWS" ) ;
	m_ps_NormalShadows_ID	= r3dRenderer->GetPixelShaderIdx( "PS_SCOPE_FILTER_NORMAL_SHADOWS" ) ;
	m_ps_HWShadows_ID		= r3dRenderer->GetPixelShaderIdx( "PS_SCOPE_FILTER_HW_SHADOWS" ) ;
	m_ps_FilterNoLighting	= r3dRenderer->GetPixelShaderIdx( "PS_SCOPE_FILTER_NO_LIGHTING" ) ;

	m_vsID = r3dRenderer->GetVertexShaderIdx("VS_SCOPE_FILTER");
	mData.PixelShaderID = m_ps_NoShadows_ID;
	mData.VertexShaderID = m_vsID;
	mData.VSType = PostFXData::VST_CUSTOM;
}

//////////////////////////////////////////////////////////////////////////


void PFX_ScopeEffect::CloseImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_ScopeEffect::PrepareImpl(r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/)
{
	// Prepare texture matrix (offset and scale) for given aspect ratio
	float sw = r3dRenderer->ScreenW;
	float sh = r3dRenderer->ScreenH;

	float aspect = sw / sh;
	float halfPixelX = 0.5f / sw;
	float halfPixelY = 0.5f / sh;

	const float SCALE = 0.05f ;

	float rSScale = 1.0f / m_scopeScale ;

	float aspRSScale = rSScale * aspect ;

	float userOffsetX = -m_scopeOffset.x * aspRSScale;
	float userOffsetY = -m_scopeOffset.y * rSScale;

	D3DXVECTOR4 vsConsts[ 6 ] =
	{
		// float4 tex2ScaleOffset: register (c0);
		D3DXVECTOR4( aspRSScale, 1.0f * rSScale, - ( aspRSScale - 1) * 0.5f + halfPixelX + userOffsetX, - ( rSScale - 1 ) * 0.5f + halfPixelY + userOffsetY ),
		// float4 vToViewSpace : register(c1) ;
		D3DXVECTOR4( SCALE, SCALE, 0.2f, 0.f ),
		// float4x4 vToShadowMtx : register (c2);
	} ;


	D3DXMATRIX shadowCompoundMtx ;

#if 1
	D3DXMatrixScaling( &shadowCompoundMtx, SCALE, SCALE, SCALE ) ;
#else
	D3DXMatrixScaling( &shadowCompoundMtx, 1.f, 1.f, 1.f ) ;
#endif

	D3DXMatrixMultiply( &shadowCompoundMtx, &shadowCompoundMtx, &r3dRenderer->InvViewMatrix ) ;
	D3DXMatrixMultiply( &shadowCompoundMtx, &shadowCompoundMtx, &m_lightMtx ) ;

	D3DXMatrixTranspose( (D3DXMATRIX*)( vsConsts + 2 ), &shadowCompoundMtx ) ;

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF(0, (float*)vsConsts, R3D_ARRAYSIZE( vsConsts ) ) );
	r3dRenderer->SetTex(m_scopeMaskTexture, PostFXChief::FREE_TEX_STAGE_START);
	r3dRenderer->SetTex(m_scopeReticuleTexture, PostFXChief::FREE_TEX_STAGE_START + 1);
	r3dRenderer->SetTex(m_scopeBlurTexture, PostFXChief::FREE_TEX_STAGE_START+2);
	if (m_blurredSceneImage)
		r3dRenderer->SetTex(m_blurredSceneImage->Tex, PostFXChief::FREE_TEX_STAGE_START + 3);

	extern r3dSun *Sun ;
	D3DXVECTOR3 sunDir = D3DXVECTOR3( Sun->SunLight.Direction.x, 
									Sun->SunLight.Direction.y,
									Sun->SunLight.Direction.z ) ;

	D3DXVECTOR3 viewSunDir ;
	D3DXVec3TransformNormal( &viewSunDir, &sunDir, &r3dRenderer->ViewMatrix ) ;
	D3DXVec3Normalize( &viewSunDir, &viewSunDir ) ;

	if( m_materialParams.Lighting )
	{
		if( !r_shadows->GetInt() )
		{
			mData.PixelShaderID = m_ps_NoShadows_ID ;
		}
		else
		{
			if( r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF )
			{
				mData.PixelShaderID = m_ps_HWShadows_ID ;
			}
			else
			{
				mData.PixelShaderID = m_ps_NormalShadows_ID ;
			}
		}		
	}
	else
	{
		mData.PixelShaderID = m_ps_FilterNoLighting ;
	}

	float chromeness = 7.0f - 7.0f * m_materialParams.Chromeness ;

	D3DXVECTOR4 vConsts[ ] =
	{
		// float4 vLight : register( c0 ) ;
		D3DXVECTOR4( viewSunDir.x, viewSunDir.y, viewSunDir.z, 0.f ),
		// float4 vAmbient : register( c1 ) ;
		D3DXVECTOR4( r3dRenderer->AmbientColor.R / 255.f, r3dRenderer->AmbientColor.G / 255.f, r3dRenderer->AmbientColor.B / 255.f, 0.f	),
		// float4 vSunColor : register( c2 ) ;
		D3DXVECTOR4( Sun->SunLight.R / 255.f, Sun->SunLight.G / 255.f, Sun->SunLight.B / 255.f, 0.f ),
		// float4 vShadowControl : register(c3) ;
		D3DXVECTOR4( 0.f, 0.f, LFUpdateShadowMap->Width, LFUpdateShadowMap->Height ),
		// float4 vShadowSteps : register(c4) ;
		D3DXVECTOR4( 1.0f / LFUpdateShadowMap->Width, 0.0f, 0.f, 1.0f / LFUpdateShadowMap->Height ),
		// float4 vMatParams : register( c5 ) ;
		D3DXVECTOR4( m_materialParams.Metalness, chromeness, m_materialParams.SpecularPower, 0.f ),
		// float4x3 vInvView : register( c6 );
		D3DXVECTOR4(),
		D3DXVECTOR4(),
		D3DXVECTOR4(),
		// 4th cause we're D3DXMatrixTransposing into it
		D3DXVECTOR4()
	} ;

	// pow 2.2 on ambient
	vConsts[ 1 ][ 0 ] = pow( vConsts[ 1 ][ 0 ], 2.2f );
	vConsts[ 1 ][ 1 ] = pow( vConsts[ 1 ][ 1 ], 2.2f );
	vConsts[ 1 ][ 2 ] = pow( vConsts[ 1 ][ 2 ], 2.2f );

	D3DXMatrixTranspose( (D3DXMATRIX*)( vConsts + R3D_ARRAYSIZE(vConsts) - 4 ), &r3dRenderer->InvViewMatrix ) ;

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &vConsts->x, R3D_ARRAYSIZE(vConsts ) ) ) ;

	if( m_scopeNormalTexture )
	{
		r3dRenderer->SetTex( m_scopeNormalTexture, PostFXChief::FREE_TEX_STAGE_START + 4 );
	}
	else
	{
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_DEFAULT_NORMAL], PostFXChief::FREE_TEX_STAGE_START + 4 );		
	}
	
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(PostFXChief::FREE_TEX_STAGE_START + 4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(PostFXChief::FREE_TEX_STAGE_START + 4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	r3dSetFiltering( R3D_BILINEAR, PostFXChief::FREE_TEX_STAGE_START + 4 ) ;

	r3dRenderer->SetTex( LFUpdateShadowMap->Tex, PostFXChief::FREE_TEX_STAGE_START + 5 );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(PostFXChief::FREE_TEX_STAGE_START + 5, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(PostFXChief::FREE_TEX_STAGE_START + 5, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	r3dFilter shadowFilter = r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF ? R3D_BILINEAR : R3D_POINT ;

	r3dSetFiltering( shadowFilter, PostFXChief::FREE_TEX_STAGE_START + 5 ) ;

	r3dRenderer->SetTex( g_EnvmapProbes.GetClosestTexture( gCam ), PostFXChief::FREE_TEX_STAGE_START + 6 );
	r3dSetFiltering( R3D_BILINEAR, PostFXChief::FREE_TEX_STAGE_START + 6 ) ;

}

//////////////////////////////////////////////////////////////////////////

void PFX_ScopeEffect::FinishImpl()
{

}


