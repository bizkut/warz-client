#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "RenderDeffered.h"

#include "PFX_ConvertToLDR.h"


//------------------------------------------------------------------------
PFX_ConvertToLDR::PFX_ConvertToLDR( )
: Parent( this )
{

}

//------------------------------------------------------------------------
PFX_ConvertToLDR::~PFX_ConvertToLDR()
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ConvertToLDR::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_CONVERT_TO_LDR" );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_ConvertToLDR::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_ConvertToLDR::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )	/*OVERRIDE*/
{
	void SetupFilmToneConstants( int reg );
	SetupFilmToneConstants( 0 );

	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START );
	int srcIdx = r_last_exposure_source->GetInt();
	r3dRenderer->SetTex( g_pPostFXChief->GetBuffer( ( srcIdx ? PostFXChief::RTT_SCENE_EXPOSURE0 : PostFXChief::RTT_SCENE_EXPOSURE1 ) )->Tex, PostFXChief::FREE_TEX_STAGE_START );

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ConvertToLDR::FinishImpl() /*OVERRIDE*/
{
	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START > () ;
}

//------------------------------------------------------------------------
