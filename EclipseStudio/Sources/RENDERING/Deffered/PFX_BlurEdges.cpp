#include "r3dPCH.h"
#include "r3d.h"

#include "PFX_BlurEdges.h"

//------------------------------------------------------------------------

PFX_BlurEdges::Params::Params()
: threshold( 0.01f )
, maxd( 1.0f )
, amplify( 1.0f )
{

}

//------------------------------------------------------------------------

PFX_BlurEdges::PFX_BlurEdges( float xDir, float yDir, int tapCount )
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

	for( int t = 2; t <= MAX_SOURCE_TAPS; t ++ )
	{
		for( int j = 0, e = t; j < e; j ++ )
		{
			int idx0 = j - 1;
			int idx1 = j;

			uint64_t add0 = idx0 < 0 ? 0 : pg0[ idx0 ];
			uint64_t add1 = idx1 >= e - 1 ? 0 : pg0[ idx1 ];

			pg1[ j ] = add0 + add1;
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
				results[ i ] = float( pg1[ i ] ) / sum;

				if( results[ i ] > IGNORE_BELOW )
					validTapCount ++;
			}

			if( validTapCount >= actualTapCount || t == MAX_SOURCE_TAPS )
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

				// renormalize coefs
				float sum = 0;

				for( int j = 1, e = tapCount + 1; j < e; j ++ )
				{
					sum += mConstants[ COEFS_PS_CONST_START + j / 4 ][ j % 4 ];
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

		std::swap( pg0, pg1 );
	}

	SetParams( Params() );
}

//------------------------------------------------------------------------

PFX_BlurEdges::~PFX_BlurEdges()
{

}

//------------------------------------------------------------------------

void
PFX_BlurEdges::SetParams( const Params& parms )
{
	mConstants[ DEPTH_THRESHOLD_PS_CONST ][ 0 ] = parms.threshold;
	mConstants[ DEPTH_THRESHOLD_PS_CONST ][ 1 ] = parms.maxd;
	mConstants[ DEPTH_THRESHOLD_PS_CONST ][ 2 ] = parms.amplify;
	mConstants[ DEPTH_THRESHOLD_PS_CONST ][ 3 ] = 0.f;
}

//------------------------------------------------------------------------

PFX_BlurEdges::Params
PFX_BlurEdges::GetParams() const
{
	Params parms;

	parms.threshold = mConstants[ DEPTH_THRESHOLD_PS_CONST ][ 0 ];
	parms.maxd		= mConstants[ DEPTH_THRESHOLD_PS_CONST ][ 1 ];
	parms.amplify	= mConstants[ DEPTH_THRESHOLD_PS_CONST ][ 2 ];

	return parms;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BlurEdges::InitImpl()	/*OVERRIDE*/
{
	r3dTL::TArray <D3DXMACRO> defines( 1 );

	defines[ 0 ].Name		= "NUM_TAPS";

	char def[] = "0";
	r3d_assert( mTapCount < 10 );
	def[ 0 ] += mTapCount;
	defines[ 0 ].Definition = def;

	char name[] = "PS_EDGE_BLUR0";
	name[ sizeof name - 2 ] += mTapCount;

	mData.PixelShaderID	= r3dRenderer->AddPixelShaderFromFile( name, "Edge_Blur_ps.hls", 0, defines );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BlurEdges::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BlurEdges::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	for( int i = 0, e = mTapCount; i < e; i ++ )
	{
		mConstants[ i / 2 ][ i % 2 * 2 + 0 ] = mDirX * ( i + 1 ) / dest->Width;
		mConstants[ i / 2 ][ i % 2 * 2 + 1 ] = mDirY * ( i + 1 ) / dest->Height;
	}

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, mConstants[0], NUM_PS_CONSTS );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BlurEdges::FinishImpl() /*OVERRIDE*/
{

}

int PFX_BlurEdges::TapCount()
{
	return mTapCount;
}

