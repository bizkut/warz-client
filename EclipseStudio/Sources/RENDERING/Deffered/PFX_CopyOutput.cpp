#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "RenderDeffered.h"

#include "PFX_CopyOutput.h"


//------------------------------------------------------------------------
PFX_CopyOutput::PFX_CopyOutput( )
: Parent( this )
, mForceFiltering( false )
{

}

//------------------------------------------------------------------------
PFX_CopyOutput::~PFX_CopyOutput()
{

}

//------------------------------------------------------------------------

void
PFX_CopyOutput::SetFiltering( bool forceFiltering )
{
	mForceFiltering = forceFiltering ;
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_CopyOutput::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_COPY_OUTPUT" );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_CopyOutput::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_CopyOutput::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )	/*OVERRIDE*/
{
	if( mForceFiltering )
	{
		r3dSetFiltering( R3D_BILINEAR, 0 ) ;
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_CopyOutput::FinishImpl() /*OVERRIDE*/
{
	if( mForceFiltering )
	{
		if( g_pPostFXChief->GetZeroTexStageFilter() )
			r3dSetFiltering( R3D_BILINEAR, 0 );
		else
			r3dSetFiltering( R3D_POINT, 0 );
	}
}

//------------------------------------------------------------------------
