//=========================================================================
//	Module: PFX_LensDirt_ExtractLuma.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"
#include "GameCommon.h"
#include "PFX_LensDirt_ExtractLuma.h"

//------------------------------------------------------------------------

PFX_LensDirt_ExtractLuma::Settings::Settings()
: Threshold( 0.125f )
{

}

//////////////////////////////////////////////////////////////////////////

PFX_LensDirt_ExtractLuma::PFX_LensDirt_ExtractLuma()
: Parent(this)
{
	
}

//////////////////////////////////////////////////////////////////////////

PFX_LensDirt_ExtractLuma::~PFX_LensDirt_ExtractLuma()
{

}

//------------------------------------------------------------------------

void PFX_LensDirt_ExtractLuma::SetSettings( const Settings& sts )
{
	m_Settings = sts;
}

//------------------------------------------------------------------------

const PFX_LensDirt_ExtractLuma::Settings& PFX_LensDirt_ExtractLuma::GetSettings() const
{
	return m_Settings;
}

//////////////////////////////////////////////////////////////////////////

void PFX_LensDirt_ExtractLuma::InitImpl()
{	
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_LENSDIRT_EXTRACTLUMA" );
}

//////////////////////////////////////////////////////////////////////////


void PFX_LensDirt_ExtractLuma::CloseImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_LensDirt_ExtractLuma::PrepareImpl(r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/)
{
	// float4 vThreshold : register( c0 );
	float psConst[ 4 ] = { m_Settings.Threshold, m_Settings.Threshold, m_Settings.Threshold, m_Settings.Threshold };
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psConst, 1 ) );
}

//////////////////////////////////////////////////////////////////////////

void PFX_LensDirt_ExtractLuma::FinishImpl()
{

}


