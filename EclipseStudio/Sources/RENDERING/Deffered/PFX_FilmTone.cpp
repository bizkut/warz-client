#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_FilmTone.h"

PFX_FilmTone::PFX_FilmTone()
: Parent( this )
{
}

//------------------------------------------------------------------------

PFX_FilmTone::~PFX_FilmTone()
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmTone::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "PS_DS_FILMTONE" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmTone::CloseImpl() /*OVERRIDE*/
{
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmTone::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	void SetupFilmToneConstants( int reg );
	SetupFilmToneConstants( 0 ) ;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmTone::FinishImpl() /*OVERRIDE*/
{

}

