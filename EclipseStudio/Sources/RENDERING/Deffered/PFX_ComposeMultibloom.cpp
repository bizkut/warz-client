#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "RenderDeffered.h"

#include "PFX_ComposeMultibloom.h"

//------------------------------------------------------------------------
PFX_ComposeMultibloom::Settings::Settings()
{
	for (int i = 0; i < bloomParts.COUNT; ++i)
	{
		bloomParts[i] = PostFXChief::RTT_COUNT;
	}
}

//------------------------------------------------------------------------
PFX_ComposeMultibloom::PFX_ComposeMultibloom(int numImages)
: Parent( this )
, mColorWriteMask(D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED)
, mNumImages(numImages)
{

}

//------------------------------------------------------------------------
PFX_ComposeMultibloom::~PFX_ComposeMultibloom()
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_ComposeMultibloom::InitImpl() /*OVERRIDE*/
{
	char psName[] = "PS_COMPOSE_MULTIBLOOM0";
	psName[strlen(psName) - 1] += mNumImages;
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( psName );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_ComposeMultibloom::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_ComposeMultibloom::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )	/*OVERRIDE*/
{
	const Settings& sts = mSettingsArr[ 0 ];

	r3dRenderer->SetRenderingMode( R3D_BLEND_ADD | R3D_BLEND_PUSH );

	if( mColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{		
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, mColorWriteMask ) );
	}

	r3dSetFiltering( R3D_BILINEAR, 0 );

	for (int i = 0; i < mNumImages; ++i)
	{
		int texSlot = PostFXChief::FREE_TEX_STAGE_START + i;

		r3dSetFiltering( R3D_BILINEAR, texSlot );

		r3dRenderer->SetTex(g_pPostFXChief->GetBuffer(sts.bloomParts[i])->Tex, texSlot );
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ComposeMultibloom::FinishImpl() /*OVERRIDE*/
{
	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	const Settings& sts = mSettingsArr[ 0 ];

	r3dSetFiltering( g_pPostFXChief->GetZeroTexStageFilter(), 0 );

	if( mNumImages > 0 ) g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START + 0 >();
	if( mNumImages > 1 ) g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START + 1 >();
	if( mNumImages > 2 ) g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START + 2 >();
	if( mNumImages > 3 ) g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START + 3 >();

	r3d_assert( mNumImages <= 4 );

	if( mColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{
		g_pPostFXChief->SetDefaultColorWriteMask();
	}

	mSettingsArr.Erase( 0 );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ComposeMultibloom::PushDefaultSettingsImpl() /*OVERRIDE*/
{
	mSettingsArr.PushBack( Settings() ) ;
}

//------------------------------------------------------------------------

void
PFX_ComposeMultibloom::PushSettings( const Settings& settings ) 
{
	mSettingsArr.PushBack( settings );

	mSettingsPushed = 1;
}
