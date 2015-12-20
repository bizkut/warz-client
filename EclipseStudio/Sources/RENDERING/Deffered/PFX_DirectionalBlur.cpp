#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "RenderDeffered.h"

#include "PFX_DirectionalBlur.h"

//------------------------------------------------------------------------

PFX_DirectionalBlur::Settings::Settings()
: AlphaMul( 1.f )
{

}

//------------------------------------------------------------------------

PFX_DirectionalBlur::PFX_DirectionalBlur( float xDir, float yDir, int tapCount )
: Parent( this )
, mDirX( xDir )
, mDirY( yDir )
, mTapCount( tapCount )
{
	typedef unsigned __int64 uint64_t;

	const int MAX_SOURCE_TAPS = 63;

	COMPILE_ASSERT( MAX_SOURCE_TAPS & 1 );

	uint64_t		gauss0[ MAX_SOURCE_TAPS ], 
					gauss1[ MAX_SOURCE_TAPS ];

	gauss0[ 0 ] = 1;

	uint64_t *pg0 = gauss0;
	uint64_t *pg1 = gauss1;

	float results[ MAX_SOURCE_TAPS ];

	int actualTapCount = tapCount * 2 + 1;

	float pw = 1.0f;

	for( int tt = 2; pw >= 0.f ; tt ++ )
	{
		int t = tt;
		
		if( tt > MAX_SOURCE_TAPS )
		{
			t = MAX_SOURCE_TAPS;
			pw -= 0.1f;
		}
		else
		{
			t = tt;

			for( int j = 0, e = t; j < e; j ++ )
			{
				int idx0 = j - 1;
				int idx1 = j;

				uint64_t add0 = idx0 < 0 ? 0 : pg0[ idx0 ];
				uint64_t add1 = idx1 >= e - 1 ? 0 : pg0[ idx1 ];

				pg1[ j ] = add0 + add1;
			}
		}

		uint64_t sum = uint64_t( 1 ) << uint64_t( t - 1 );

		if( t & 1 )
		{
			// ignore samples below 'IGNORE_BELOW'
			// they may not influence the results much
			const float IGNORE_BELOW = 0.125f / actualTapCount;

			int validTapCount = 0;

			for( int i = 0, e = t; i < e; i ++ )
			{
				results[ i ] = powf( float( double( pg1[ i ] ) / sum ), pw );

				if( results[ i ] > IGNORE_BELOW )
					validTapCount ++;
			}

			if( validTapCount >= actualTapCount || pw <= 0.f )
			{
				// use only the ones below IGNORE_BELOW
				// or maybe not in case t == MAX_SOURCE_TAPS
				for( int	i = (t - 1) / 2 - tapCount, 
							j = 0, e = tapCount + 1; 
							j < e; 
							i ++, j ++ )
				{
					float r = results[ i ];

					// reverse
					int idx = e - j - 1;
					mConstants[ COEFS_PS_CONST_START + idx / 4 ][ idx % 4 ] = r;
				}

				// renormalize
				float sum = 0;

				for( int j = 1, e = tapCount + 1; j < e; j ++ )
				{
					float& constant = mConstants[ COEFS_PS_CONST_START + j / 4 ][ j % 4 ];
					sum += constant;
				}

				sum *= 2;
				sum += mConstants[ COEFS_PS_CONST_START ][ 0 ];

				for( int j = 0, e = tapCount + 1; j < e; j ++ )
				{
					mConstants[ COEFS_PS_CONST_START + j / 4 ][ j % 4 ] /= sum;
				}

				break;
			}
		}

		if( tt < MAX_SOURCE_TAPS )
		{
			std::swap( pg0, pg1 );
		}
	}
}

//------------------------------------------------------------------------


void
PFX_DirectionalBlur::PushSettings( const Settings& settings )
{
	mSettings.PushBack( settings );
	mSettingsPushed = 1;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DirectionalBlur::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DirectionalBlur::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src ) /*OVERRIDE*/
{
	r3d_assert(src->Width >= dest->Width && src->Height >= dest->Height);
	DoPrepare( src, 1.f );
}

//------------------------------------------------------------------------

void
PFX_DirectionalBlur::DoPrepare( r3dScreenBuffer* src, float stepAmplify )
{
	r3d_assert( mSettings.Count() );

	const Settings& sts = mSettings[ 0 ];

	mConstants[ NUM_PS_CONSTS - 1 ][ 0 ] = sts.AlphaMul;
	mConstants[ NUM_PS_CONSTS - 1 ][ 1 ] = 0;
	mConstants[ NUM_PS_CONSTS - 1 ][ 2 ] = 0;
	mConstants[ NUM_PS_CONSTS - 1 ][ 3 ] = 0;

	mSettings.Erase( 0 );

	for( int i = 0, e = mTapCount; i < e; i ++ )
	{
		float offset = i + 1.0f;

		mConstants[ i / 2 ][ i % 2 * 2 + 0 ] = mDirX * offset / src->Width * stepAmplify;
		mConstants[ i / 2 ][ i % 2 * 2 + 1 ] = mDirY * offset / src->Height * stepAmplify;
	}

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, mConstants[0], NUM_PS_CONSTS );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DirectionalBlur::FinishImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DirectionalBlur::PushDefaultSettingsImpl()	/*OVERRIDE*/
{
	mSettings.PushBack( Settings() );
}

//------------------------------------------------------------------------

int PFX_DirectionalBlur::TapCount() const
{
	return mTapCount;
}

//-------------------------------------------------------------------------
//	PFX_DirectionalBlurOptimized
//-------------------------------------------------------------------------

PFX_DirectionalBlurOptimized::PFX_DirectionalBlurOptimized(float xDir, float yDir, int tapCount)
: PFX_DirectionalBlur(xDir, yDir, tapCount)
{
	//	Adjust constants to use bilinear filtering hardware capabilities and making 1 bilinear filtered tap instead 2.
	for (int j = 0, e = tapCount / 2; j < e; ++j)
	{
		int i1 = COEFS_PS_CONST_START + (j * 2 + 1) / 4;
		int k1 = (j * 2 + 1) % 4;
		int i2 = COEFS_PS_CONST_START + (j * 2 + 2) / 4;
		int k2 = (j * 2 + 2) % 4;
		float &w1 = mConstants[i1][k1];
		float w2 = mConstants[i2][k2];
		w1 += w2;
	}
}

//-------------------------------------------------------------------------

void
PFX_DirectionalBlurOptimized::DoPrepare(r3dScreenBuffer* src, float stepAmplify)
{
	r3d_assert( mSettings.Count() );

	const Settings& sts = mSettings[ 0 ];

	// Alexey. We can force mip level now. 
	int activeMip = src->ActiveMip;
	if (activeMip == -1)
		activeMip = 0;

	mConstants[ NUM_PS_CONSTS - 1 ][ 0 ] = sts.AlphaMul;
	mConstants[ NUM_PS_CONSTS - 1 ][ 1 ] = (float)activeMip;
	mConstants[ NUM_PS_CONSTS - 1 ][ 2 ] = 0;
	mConstants[ NUM_PS_CONSTS - 1 ][ 3 ] = 0;

	mSettings.Erase( 0 );

	for( int i = 0, e = mTapCount / 2; i < e; i ++ )
	{
		int i1 = COEFS_PS_CONST_START + (i * 2 + 1) / 4;
		int k1 = (i * 2 + 1) % 4;
		int i2 = COEFS_PS_CONST_START + (i * 2 + 2) / 4;
		int k2 = (i * 2 + 2) % 4;

		float offset = i * 2 + 1.0f + mConstants[i2][k2] / mConstants[i1][k1];

		mConstants[ i / 2 ][ i % 2 * 2 + 0 ] = mDirX * offset / src->GetActualMipWidth() * stepAmplify;
		mConstants[ i / 2 ][ i % 2 * 2 + 1 ] = mDirY * offset / src->GetActualMipHeight() * stepAmplify;
	}

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, mConstants[0], NUM_PS_CONSTS );

	r3dSetFiltering(R3D_BILINEAR, 0);
}

//-------------------------------------------------------------------------

void
PFX_DirectionalBlurOptimized::FinishImpl()
{
	r3dSetFiltering(g_pPostFXChief->GetZeroTexStageFilter(), 0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DirectionalBlurOptimized::InitImpl()	/*OVERRIDE*/
{
	char name[] = "PS_MULTITAP_BLUR00";
	name[ sizeof name - 2 ] += mTapCount % 10;
	name[ sizeof name - 3 ] += mTapCount / 10;

	mData.PixelShaderID	= r3dRenderer->GetPixelShaderIdx(name);
}

//-------------------------------------------------------------------------
//	PFX_DirectionalDepthBlur
//-------------------------------------------------------------------------

PFX_DirectionalDepthBlur::Settings::Settings()
: Start( 100.f )
, End( 500.f )
, Reverse( false )
, UseStencil( false )
{

}

//------------------------------------------------------------------------

PFX_DirectionalDepthBlur::PFX_DirectionalDepthBlur( float xDir, float yDir, int tapCount )
: PFX_DirectionalBlur( xDir, yDir, tapCount )
, mPSForwardID( -1 )
, mPSReverseID( -1 )
{

}

//------------------------------------------------------------------------

PFX_DirectionalDepthBlur::~PFX_DirectionalDepthBlur()
{

}

//------------------------------------------------------------------------

void PFX_DirectionalDepthBlur::InitImpl()
{
	char name[] = "PS_MULTITAP_DEPTH_BLUR00_F";
	name[ sizeof name - 4 ] += TapCount() % 10;
	name[ sizeof name - 5 ] += TapCount() / 10;

	mPSForwardID = r3dRenderer->GetPixelShaderIdx( name );

	name[ sizeof name - 2 ] = 'R';
	mPSReverseID = r3dRenderer->GetPixelShaderIdx( name );

	mData.PixelShaderID	= mPSForwardID;
}

//------------------------------------------------------------------------

void PFX_DirectionalDepthBlur::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	r3d_assert( mSettings.Count() );

	const Settings& sts = mSettings[ 0 ];

	PFX_DirectionalBlur::DoPrepare( src, sts.Reverse ? 2.f : 1.f );

	float vConsts[ 4 ];

	if( sts.Reverse)
	{
		vConsts[ 0 ] = 0.f;
		vConsts[ 1 ] = 0.f;
		vConsts[ 2 ] = 0.f;

		r3dRenderer->SetTex( sts.ReverseMask->Tex, PostFXChief::FREE_TEX_STAGE_START );

		mData.PixelShaderID = mPSReverseID;
	}
	else
	{
		vConsts[ 0 ] = 1.f / ( sts.End - sts.Start );
		vConsts[ 1 ] = - sts.Start / ( sts.End - sts.Start );
		vConsts[ 2 ] = sts.Start;

		mData.PixelShaderID = mPSForwardID;
	}

	vConsts[ 3 ] = 0.f;
	
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( PFX_DirectionalBlur::NUM_PS_CONSTS, vConsts, 1 );

	if( sts.UseStencil )
	{
		SetupLightMaskStencilStates( SCM_LITAREA );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, TRUE ) ) ;
	}

}

//------------------------------------------------------------------------

/*virtual*/

void
PFX_DirectionalDepthBlur::FinishImpl() /*OVERRIDE*/
{
	PFX_DirectionalBlur::FinishImpl();

	const Settings& sts = mSettings[ 0 ];

	mSettings.Erase( 0 );

	if( sts.UseStencil )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) ) ;
	}
}

//------------------------------------------------------------------------

/*virtual*/
void
PFX_DirectionalDepthBlur::PushDefaultSettingsImpl()	/*OVERRIDE*/
{
	PFX_DirectionalBlur::PushDefaultSettingsImpl();
	mSettings.PushBack( Settings() );
}

//------------------------------------------------------------------------

void
PFX_DirectionalDepthBlur::PushSettings ( const Settings& settings )
{
	PFX_DirectionalBlur::PushSettings( PFX_DirectionalBlur::Settings() );
	mSettings.PushBack( settings );

	mSettingsPushed = 1;
}

//------------------------------------------------------------------------

PFX_DirectionalBloomWithGlowBlur::PFX_DirectionalBloomWithGlowBlur(float xDir, float yDir)
: PFX_DirectionalBlurOptimized(xDir, yDir, 16)
{
}

//------------------------------------------------------------------------

void
PFX_DirectionalBloomWithGlowBlur::InitImpl()
{
	char name[] = "PS_MULTITAP_PRESERVRE_GLOW_BLUR00";
	name[ sizeof name - 2 ] += TapCount() % 10;
	name[ sizeof name - 3 ] += TapCount() / 10;

	mData.PixelShaderID	= r3dRenderer->GetPixelShaderIdx( name );
}