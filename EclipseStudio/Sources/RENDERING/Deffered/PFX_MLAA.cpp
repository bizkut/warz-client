#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_MLAA.h"

r3dScreenBuffer* gBuffer_MLAA_LinesH;
r3dScreenBuffer* gBuffer_MLAA_LinesV;

static r3dTexture* pAreaTex;
static const int nAreaSize = 512;

float MLAA_DiscontFactor = 1.3f;

void UpdateMLAA()
{
	if( r_mlaa->GetInt() )
	{
		if( !pAreaTex )
		{
			InitMLAA() ;
		}
	}
	else
	{
		if( pAreaTex )
		{
			CloseMLAA() ;
		}
	}
}


void InitMLAA()
{
	pAreaTex = r3dRenderer->AllocateTexture();
	pAreaTex->Create( nAreaSize, nAreaSize, D3DFMT_R32F, 1 );

	float* tabAires = gfx_new float[ nAreaSize * nAreaSize ];

	int T, borne;
	float T_on_2;

	for( int i = 0; i < nAreaSize; i++ )
	{
		T = ( i + 1 );
		for( int j = 0; j < nAreaSize; j++ )
		{
			tabAires[ i * nAreaSize + j ] = 0.0f;
			
			T_on_2 = T / 2.0f;
			if ( T % 2 == 0 )
			{
				borne = ( T / 2 ) - 1;
			}
			else
			{
				borne = ( T / 2 );
			}
			if ( j < borne )
			{
				tabAires[ i * nAreaSize + j ] = ( 1.0f + ( ( T_on_2 - j - 1.0f ) / T_on_2 ) ) / 4.0f;
			}
			else if ( j == borne )
			{
				if ( T % 2 == 0 )
				{
					tabAires[ i * nAreaSize + j ] = 1.0f / ( 2.0f * T );
				}
				else
				{
					tabAires[ i * nAreaSize + j ] = 1.0f / ( 8.0f * T );
				}
			}
			else
			{
				tabAires[ i * nAreaSize + j ] = 0.0f;
			}
		}
	}

	D3DLOCKED_RECT lockRect;

	D3D_V( pAreaTex->AsTex2D()->LockRect( 0, &lockRect, NULL, 0 ) );

	for ( int i = 0; i < nAreaSize; i++ )
		memcpy( ( ( char* ) lockRect.pBits ) + i * lockRect.Pitch, tabAires + i * nAreaSize, nAreaSize * sizeof( float ) );
	delete[] tabAires;

	D3D_V( pAreaTex->AsTex2D()->UnlockRect( 0 ) );

	gBuffer_MLAA_LinesH = r3dScreenBuffer::CreateClass( "MLAA_LinesH", r3dRenderer->ScreenW, r3dRenderer->ScreenH, D3DFMT_A8R8G8B8 );
	gBuffer_MLAA_LinesV = r3dScreenBuffer::CreateClass( "MLAA_LinesV", r3dRenderer->ScreenW, r3dRenderer->ScreenH, D3DFMT_A8R8G8B8 );
}

void CloseMLAA()
{
	if( pAreaTex )
	{
		r3dRenderer->DeleteTexture( pAreaTex );

		pAreaTex = NULL ;

		SAFE_DELETE(gBuffer_MLAA_LinesH);
		SAFE_DELETE(gBuffer_MLAA_LinesV);
	}
}

static void SetAddressMode( int nStage )
{
	r3dRenderer->pd3ddev->SetSamplerState( nStage, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
	r3dRenderer->pd3ddev->SetSamplerState( nStage, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );
	r3dRenderer->pd3ddev->SetSamplerState( nStage, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER );
}

static void RestoreAddressMode( int nStage )
{
	r3dRenderer->pd3ddev->SetSamplerState( nStage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( nStage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( nStage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
}

//------------------------------------------------------------
PFX_MLAA_DiscontMap::PFX_MLAA_DiscontMap()
: Parent( this )
{

}

PFX_MLAA_DiscontMap::~PFX_MLAA_DiscontMap()
{

}

void PFX_MLAA_DiscontMap::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_MLAA_DISCONT_MAP" );
}

void PFX_MLAA_DiscontMap::CloseImpl()
{
	
}

void PFX_MLAA_DiscontMap::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, D3DXVECTOR4( 1.0f / r3dRenderer->ScreenW, 1.0f / r3dRenderer->ScreenH, MLAA_DiscontFactor, 0 ), 1 );
}

void PFX_MLAA_DiscontMap::FinishImpl()
{
	
}

//------------------------------------------------------------
PFX_MLAA_LineDetect::PFX_MLAA_LineDetect()
: Parent( this )
{
	m_fLevel = 1.0f;
}

PFX_MLAA_LineDetect::~PFX_MLAA_LineDetect()
{

}

void PFX_MLAA_LineDetect::InitImpl()
{
	
}

void PFX_MLAA_LineDetect::CloseImpl()
{

}

void PFX_MLAA_LineDetect::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	SetAddressMode( 0 );
	SetAddressMode( PostFXChief::FREE_TEX_STAGE_START );
	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START );
	g_pPostFXChief->BindBufferTexture( PostFXChief::RTT_PINGPONG_NEXT, PostFXChief::FREE_TEX_STAGE_START );
}

void PFX_MLAA_LineDetect::FinishImpl()
{
	RestoreAddressMode( 0 );
	RestoreAddressMode( PostFXChief::FREE_TEX_STAGE_START );
	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START >();
	m_fLevel *= 2;
}

void PFX_MLAA_LineDetect::ResetLevel()
{
	m_fLevel = 1.0f;
}

float PFX_MLAA_LineDetect::GetLevel()
{
	return m_fLevel;
}

//------------------------------------------------------------
PFX_MLAA_LineDetectV::PFX_MLAA_LineDetectV():
PFX_MLAA_LineDetect()
{
}

PFX_MLAA_LineDetectV::~PFX_MLAA_LineDetectV()
{
}

void PFX_MLAA_LineDetectV::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_MLAA_LINE_DETECT_V" );
}

void PFX_MLAA_LineDetectV::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	PFX_MLAA_LineDetect::PrepareImpl( dest, src );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, D3DXVECTOR4( GetLevel(), 1 / r3dRenderer->ScreenH, 0, 0 ), 1 );
}

//------------------------------------------------------------
PFX_MLAA_LineDetectH::PFX_MLAA_LineDetectH():
PFX_MLAA_LineDetect()
{
}

PFX_MLAA_LineDetectH::~PFX_MLAA_LineDetectH()
{
}

void PFX_MLAA_LineDetectH::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_MLAA_LINE_DETECT_H" );
}

void PFX_MLAA_LineDetectH::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	PFX_MLAA_LineDetect::PrepareImpl( dest, src );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, D3DXVECTOR4( GetLevel(), 1 / r3dRenderer->ScreenW, 0, 0 ), 1 );
}

//------------------------------------------------------------
PFX_MLAA_AlphaCalc::PFX_MLAA_AlphaCalc()
: Parent( this )
{

}

PFX_MLAA_AlphaCalc::~PFX_MLAA_AlphaCalc()
{

}

void PFX_MLAA_AlphaCalc::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_MLAA_ALPHA_CALC" );
}

void PFX_MLAA_AlphaCalc::CloseImpl()
{

}

void PFX_MLAA_AlphaCalc::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	SetAddressMode( 0 );
	SetAddressMode( PostFXChief::FREE_TEX_STAGE_START );
	SetAddressMode( PostFXChief::FREE_TEX_STAGE_START + 1 );
	SetAddressMode( PostFXChief::FREE_TEX_STAGE_START + 2 );

	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START );
	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START + 1 );
	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START + 2 );
	g_pPostFXChief->BindBufferTexture( PostFXChief::RTT_MLAA_LINES_H, PostFXChief::FREE_TEX_STAGE_START );
	g_pPostFXChief->BindBufferTexture( PostFXChief::RTT_MLAA_LINES_V, PostFXChief::FREE_TEX_STAGE_START + 1 );
	r3dRenderer->SetTex( pAreaTex, PostFXChief::FREE_TEX_STAGE_START + 2 );

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, D3DXVECTOR4( 1 / r3dRenderer->ScreenW, 1 / r3dRenderer->ScreenH, 1.0f / nAreaSize, 1.0f / nAreaSize ), 1 );
}

void PFX_MLAA_AlphaCalc::FinishImpl()
{
	RestoreAddressMode( 0 );
	RestoreAddressMode( PostFXChief::FREE_TEX_STAGE_START );
	RestoreAddressMode( PostFXChief::FREE_TEX_STAGE_START + 1 );
	RestoreAddressMode( PostFXChief::FREE_TEX_STAGE_START + 2 );

	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START >();
	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START + 1 >();
	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START + 2 >();
}

//------------------------------------------------------------
PFX_MLAA_Blend::PFX_MLAA_Blend()
: Parent( this )
{

}

PFX_MLAA_Blend::~PFX_MLAA_Blend()
{

}

void PFX_MLAA_Blend::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_MLAA_BLEND" );
}

void PFX_MLAA_Blend::CloseImpl()
{

}

void PFX_MLAA_Blend::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	SetAddressMode( PostFXChief::FREE_TEX_STAGE_START );

	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START );
	g_pPostFXChief->BindBufferTexture( PostFXChief::RTT_TEMP0_64BIT, PostFXChief::FREE_TEX_STAGE_START );

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, D3DXVECTOR4( 1 / r3dRenderer->ScreenW, 1 / r3dRenderer->ScreenH, 0.0f, 0.0f ), 1 );
}

void PFX_MLAA_Blend::FinishImpl()
{
	RestoreAddressMode( PostFXChief::FREE_TEX_STAGE_START );

	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START >();
}