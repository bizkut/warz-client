#include "r3dPCH.h"
#include "r3d.h"

#include "obj_EnvmapProbe.h"

#include "EnvmapGen.h"

#include "GameLevel.h"

#include "EnvmapProbes.h"

static r3dTexture* EnvmapProbes_GetClosestTexture(const r3dPoint3D& pos)
{
	return g_EnvmapProbes.GetClosestTexture(pos);
}

EnvmapProbes::EnvmapProbes()
: mForceGlobal( false )
, mLastCamPos( 0.f, 0.f, 0.f )
, mEnvmapDirty( 0 )
, mEnvmapStep( 0 )
, mEnvmapSubStep( 0 )
, mEnvmapObjPos( 0 )
, mLastTimeOfDay( 0 )
, mEnvmapUpdate( false )
{
	// set callback in evenity
	extern r3dTexture* (*r3dMat_EnvmapProbes_GetClosestTexture)(const r3dPoint3D& pos);
	r3dMat_EnvmapProbes_GetClosestTexture = &EnvmapProbes_GetClosestTexture;
}

//------------------------------------------------------------------------

EnvmapProbes::~EnvmapProbes()
{

}

//------------------------------------------------------------------------

r3dTexture*
EnvmapProbes::GetClosestTexture( const r3dPoint3D& pos ) const
{
	extern r3dScreenBuffer*	DynamicEnvMap0;
	extern r3dScreenBuffer*	DynamicEnvMap1;

	if( mEnvmapUpdate )
		return NULL;

	if( r_dynamic_envmap_probes->GetInt() && DynamicEnvMap0 )
	{
		return DynamicEnvMap0->Tex;
	}
	else
	{
		float minDist	= FLT_MAX;
		int minIdx		= WRONG_IDX;

		if( !mForceGlobal )
		{
			for( uint32_t i = 0, e = mProbes.Count(); i < e; i ++ )
			{
				obj_EnvmapProbe* probe = mProbes[ i ];
				float newDist = (pos - probe->GetPosition()).LengthSq();

				if( newDist < minDist && ( probe->GetBBoxWorld().ContainsPoint( pos ) || probe->IsInfinite() ) )
				{
					minDist	= newDist;
					minIdx	= i;
				}
			}
		}

		if( minIdx == WRONG_IDX )
		{
			return mGlobalEnvmap;
		}
		else
		{
			r3dTexture* tex = mProbes[ minIdx ]->GetTexture();
			return tex ? tex : mGlobalEnvmap;
		}
	}
}

//------------------------------------------------------------------------

void
EnvmapProbes::Register( obj_EnvmapProbe* probe )
{
	r3d_assert( GetIdxOf( probe ) == WRONG_IDX );

	mProbes.PushBack( probe );
}

//------------------------------------------------------------------------

void
EnvmapProbes::Unregister( obj_EnvmapProbe* probe )
{
	int idx = GetIdxOf( probe );

	r3d_assert( idx != WRONG_IDX );

	mProbes.Erase( idx );
}

//------------------------------------------------------------------------

void
EnvmapProbes::SetEnvmapUpdateMode( bool bUpdateEnvmap )
{
	mEnvmapUpdate = bUpdateEnvmap;
}

//------------------------------------------------------------------------

int
EnvmapProbes::GetCount() const
{
	return (int)mProbes.Count();
}

//------------------------------------------------------------------------

obj_EnvmapProbe*
EnvmapProbes::GetProbe( int idx ) const
{
	return mProbes[ idx ];
}

//------------------------------------------------------------------------

void
EnvmapProbes::DeselectAll()
{
	for( uint32_t i = 0, e = mProbes.Count(); i < e; i ++ )
	{
		mProbes[ i ]->SetSelected( false );
	}
}

//------------------------------------------------------------------------

void
EnvmapProbes::GenerateAll()
{
	for( uint32_t i = 0, e = mProbes.Count(); i < e; i ++ )
	{
		mProbes[ i ]->Generate();
	}
}

//------------------------------------------------------------------------

void
EnvmapProbes::Generate( int id )
{
	for( uint32_t i = 0, e = mProbes.Count(); i < e; i ++ )
	{
		if( id - 1 == mProbes[ i ]->GetID() )
		{
			mProbes[ i ]->Generate();
		}
	}
}

//------------------------------------------------------------------------

void
EnvmapProbes::Init()
{
	mGlobalEnvmap = r3dRenderer->LoadTexture( "Data\\Shaders\\Texture\\Cubemap.dds" );

	mLastCamPos = r3dPoint3D( FLT_MAX, FLT_MAX, FLT_MAX );

	mEnvmapDirty = 0;

	ResetGenStep();
}

//------------------------------------------------------------------------

void
EnvmapProbes::Close()
{
	r3dRenderer->DeleteTexture( mGlobalEnvmap );
}

//------------------------------------------------------------------------

void
EnvmapProbes::MarkDirty()
{
	ResetGenStep();
	mEnvmapDirty = 1;
}

//------------------------------------------------------------------------

void
EnvmapProbes::Update( const r3dPoint3D& camPos )
{
	if( r_dynamic_envmap_probes->GetInt() )
	{
		if( ( mLastCamPos - camPos ).Length() > r_dynamic_envmap_update_range->GetFloat() )
		{
			MarkDirty();
			mLastCamPos = camPos;
		}

		if( fabs( r3dGameLevel::Environment->__CurTime - mLastTimeOfDay ) > 0.5f )
		{
			MarkDirty();
			mLastTimeOfDay = r3dGameLevel::Environment->__CurTime;
		}

		if( mEnvmapDirty )
		{
			extern r3dScreenBuffer* DynamicEnvMap0;
			extern r3dScreenBuffer* DynamicEnvMap1;

			if( !DynamicEnvMap0 )
			{
				void CreateDynamicEnvMapResources( int );

				float width, height;
				GetDesiredRTDimmensions( &width, &height );

				CreateDynamicEnvMapResources( (int)R3D_MIN( height, width ) );
			}

			mEnvmapUpdate = 1;

			int res = GenerateEnvmapDynamic( DynamicEnvMap1, gCam, (GenEnvmapSequence)mEnvmapStep, (GenEnvmapSubSequence)mEnvmapSubStep, &mEnvmapObjPos );

			mEnvmapUpdate = 0;

			if( mEnvmapStep != GENENVMAP_STITCH )
			{
				if( mEnvmapSubStep == GENENVMAPSUB_OBJECTS )
				{
					if( mEnvmapObjPos >= CurRenderPipeline->GetStagedRenderObjectCount() )
					{
						mEnvmapSubStep ++;
						mEnvmapObjPos = 0;
					}
				}
				else
					mEnvmapSubStep ++;

				if( mEnvmapSubStep == GENENVMAPSUB_COUNT )
				{
					mEnvmapSubStep = 0;
					mEnvmapStep ++;
				}
			}

			if( res )
			{
				R3D_SWAP( DynamicEnvMap0, DynamicEnvMap1 );

				mEnvmapDirty = 0;
				ResetGenStep();
			}
		}
	}
}

//------------------------------------------------------------------------

void
EnvmapProbes::SetForceGlobal( bool forceGlobal )
{
	mForceGlobal = forceGlobal;
}

//------------------------------------------------------------------------

int
EnvmapProbes::GetIdxOf( obj_EnvmapProbe* probe )
{
	for( uint32_t i = 0, e = mProbes.Count(); i < e; i ++ )
	{
		if( mProbes[ i ] == probe )
		{
			return (int)i;
		}
	}

	return -1;
}

//------------------------------------------------------------------------

void EnvmapProbes::ResetGenStep()
{
	mEnvmapStep		= 0;
	mEnvmapSubStep	= 0;
	mEnvmapObjPos	= 0;
}

//------------------------------------------------------------------------

EnvmapProbes g_EnvmapProbes;