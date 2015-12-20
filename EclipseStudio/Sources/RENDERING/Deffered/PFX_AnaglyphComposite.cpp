#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_AnaglyphComposite.h"

//------------------------------------------------------------------------

PFX_AnaglyphComposite::PFX_AnaglyphComposite()
: Parent( this )
, mBufferID( PostFXChief::RTT_TEMP0_64BIT )
{

}

//------------------------------------------------------------------------

PFX_AnaglyphComposite::~PFX_AnaglyphComposite()
{

}

//------------------------------------------------------------------------

void
PFX_AnaglyphComposite::SetRightBufferID( int BufferID )
{
	mBufferID = BufferID ;
}


//------------------------------------------------------------------------
/*virtual*/

void
PFX_AnaglyphComposite::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetShaderIdx( "PS_ANAGLYPHCOMPOSITE" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_AnaglyphComposite::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_AnaglyphComposite::PrepareImpl(	r3dScreenBuffer* /*dest*/,
									r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	r3dRenderer->SetTex( g_pPostFXChief->GetBuffer( (PostFXChief::RTType)mBufferID  )->Tex, PostFXChief::FREE_TEX_STAGE_START );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_AnaglyphComposite::FinishImpl() /*OVERRIDE*/
{

}
