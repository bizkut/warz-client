#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "RenderDeffered.h"

#include "PFX_ExposureBlend.h"


//------------------------------------------------------------------------
PFX_ExposureBlend::PFX_ExposureBlend( )
: Parent( this )
, mPrevTime( 0 )
{

}

//------------------------------------------------------------------------
PFX_ExposureBlend::~PFX_ExposureBlend()
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExposureBlend::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_EXPOSUREBLEND" );

	mPrevTime = r3dGetTime() ;
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_ExposureBlend::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_ExposureBlend::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )	/*OVERRIDE*/
{
	void SetupFilmToneConstants( int reg );
	SetupFilmToneConstants( 0 ) ;

	float newTime = r3dGetTime() ;

	float dt = (newTime - mPrevTime) ;

#if ENABLE_MULTI_GPU_OPTIMIZATIONS
	extern int gSLI_Crossfire_NumGPUs;
	extern int gRenderFrameCounter;

	if (gSLI_Crossfire_NumGPUs > 1)
	{
		static float saveDt = dt;
		if (gRenderFrameCounter % 2 != 0)
		{
			dt = saveDt;
		}
		else
		{
			saveDt = dt;
		}
		dt *= gSLI_Crossfire_NumGPUs * 2;
	}
#endif

	float expCtrl[ 4 ] = {	R3D_MIN( dt * r_light_adapt_speed_pos->GetFloat(), 1.0f ), 
							R3D_MIN( dt * r_light_adapt_speed_neg->GetFloat(), 1.0f ), 
							0.f, 0.f } ;

	mPrevTime = newTime ;

	// float4      vLerpCtrl           : register(c3);
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 4, expCtrl, 1 ) ) ;
	
	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START ) ;

	r3dRenderer->SetTex( g_pPostFXChief->GetBuffer( PostFXChief::RTT_AVG_SCENE_LUMA )->Tex, PostFXChief::FREE_TEX_STAGE_START ) ;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExposureBlend::FinishImpl() /*OVERRIDE*/
{
	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START > () ;
}

//------------------------------------------------------------------------
