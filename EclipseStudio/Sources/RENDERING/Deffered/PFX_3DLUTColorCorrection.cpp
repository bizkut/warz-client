#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "../../EclipseStudio/Sources/ObjectsCode/Nature/obj_LocalColorCorrection.h"
#include "../../EclipseStudio/Sources/ObjectsCode/ai/AI_Player.H"

#include "PostFXChief.h"

#include "PFX_3DLUTColorCorrection.h"
#include "HUDFilters.h"


r3dTexture* PFX_3DLUTColorCorrection::s_damageHitColorCorrectionTex = NULL;
float PFX_3DLUTColorCorrection::s_damageHitColorCorrectionVal = 0.0f;
//------------------------------------------------------------------------

PFX_3DLUTColorCorrection::PFX_3DLUTColorCorrection()
: Parent( this )
{

}

//------------------------------------------------------------------------

PFX_3DLUTColorCorrection::~PFX_3DLUTColorCorrection()
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_3DLUTColorCorrection::InitImpl() /*OVERRIDE*/
{
	m3DLUTPSIDs[ 0 ]	= r3dRenderer->GetPixelShaderIdx( "PS_CC_LUT3D_1" );
	m3DLUTPSIDs[ 1 ]	= r3dRenderer->GetPixelShaderIdx( "PS_CC_LUT3D_2" );
	m3DLUTPSIDs[ 2 ]	= r3dRenderer->GetPixelShaderIdx( "PS_CC_LUT3D_3" );

	mData.PixelShaderID = m3DLUTPSIDs[ 0 ];

	if(s_damageHitColorCorrectionTex == 0)
	{
		char tempstr[512];
		GetCCLUT3DTextureFullPath(tempstr, "DamegeHit.dds");
		s_damageHitColorCorrectionTex = r3dRenderer->LoadTexture( tempstr, D3DFMT_UNKNOWN, false, 1, 0, D3DPOOL_MANAGED, PlayerTexMem );
		r3d_assert(s_damageHitColorCorrectionTex != NULL);
		s_damageHitColorCorrectionVal = 0.0f;
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_3DLUTColorCorrection::CloseImpl() /*OVERRIDE*/
{
	if(s_damageHitColorCorrectionTex)
	{
		r3dRenderer->DeleteTexture(s_damageHitColorCorrectionTex);
		s_damageHitColorCorrectionTex = NULL;
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_3DLUTColorCorrection::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	void ReloadCCLUT3DTexture( const char*, r3dAtmosphere::SkyPhase, HUDFilters );

	HUDFilterSettings &hfs = gHUDFilterSettings[r_hud_filter_mode->GetInt()];

	r3dTexture* tex0;
	r3dTexture* tex1;
	float texLerp;

	hfs.GetCurrentColorCorrection( &tex0, &tex1, &texLerp );

	if( tex0 == 0 )
	{
		r3dAtmosphere::SkyPhase phase0, phase1;
		GetAdjecantSkyPhasesAndLerpT( &phase0, &phase1, &texLerp );

		ReloadCCLUT3DTexture( "default.dds", phase0, static_cast<HUDFilters>(r_hud_filter_mode->GetInt()) );
	}

	if( tex1 == 0 )
	{
		tex1 = tex0;
	}

	r3dTexture* pLocalCCTex = NULL;
	float fFadeCoef = 0;
	// PT: we are not using local color correction right now, so, if player needs to render damage hit color correction, use it instead of local color correction
	// if later we will start using local color corrections - add new filter for player color correction
	if(s_damageHitColorCorrectionVal > 0.001f)
	{
		pLocalCCTex = s_damageHitColorCorrectionTex;
		fFadeCoef = s_damageHitColorCorrectionVal;
	}
	else
	{
		// find nearest local color correction
		obj_LocalColorCorrection* pNearestLocalCC = obj_LocalColorCorrection::FindNearestColorCorrection ();
		fFadeCoef = pNearestLocalCC ? R3D_CLAMP( ( 1.0f - ( ( gCam - pNearestLocalCC->GetPosition() ).Length () - pNearestLocalCC->RadiusMin() ) / (pNearestLocalCC->RadiusMax() - pNearestLocalCC->RadiusMin()) ), 0.0f, 1.0f ) : 0.0f;

		// reset local cc if not appropriate
		if ( fFadeCoef <= 0.0f ) 
			pNearestLocalCC = NULL;
		if ( pNearestLocalCC && !pNearestLocalCC->CCTexture () )
			pNearestLocalCC = NULL;

		if(pNearestLocalCC!=NULL)
			pLocalCCTex = pNearestLocalCC->CCTexture();
	}

	float lerps[ 4 ] = { 0 };

	int texCount = 0;

	r3dRenderer->SetTex( tex0, PostFXChief::FREE_TEX_STAGE_START + texCount ++ );

	if( tex1 != tex0 )
	{
		lerps[ texCount - 1 ] = texLerp;
		r3dRenderer->SetTex( tex1, PostFXChief::FREE_TEX_STAGE_START + texCount ++ );
	}

	if ( pLocalCCTex )
	{
		lerps[ texCount - 1 ] = fFadeCoef;
		r3dRenderer->SetTex( pLocalCCTex, PostFXChief::FREE_TEX_STAGE_START + texCount ++ );
	}

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, lerps, 1 ) );

	mData.PixelShaderID = m3DLUTPSIDs[ texCount - 1 ];
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_3DLUTColorCorrection::FinishImpl()	/*OVERRIDE*/
{

}

