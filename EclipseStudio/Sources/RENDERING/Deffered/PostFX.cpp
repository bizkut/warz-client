#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PostFX.h"

PostFXData::PostFXData()
: PixelShaderID( WRONG_SHADER_ID )
, VertexShaderID( WRONG_SHADER_ID )
, VSType( VST_DEFAULT )
{
	TexTransform[ 0 ] = 1.f;
	TexTransform[ 1 ] = 1.f;
	TexTransform[ 2 ] = 0.f;
	TexTransform[ 3 ] = 0.f;
}

//------------------------------------------------------------------------

PostFX::~PostFX()
{

}

//------------------------------------------------------------------------
void
PostFX::Init()
{
	InitImpl();

	r3d_assert( mData.PixelShaderID != PostFXData::WRONG_SHADER_ID );

	// you prolly forgot to set mData.VSType to PostFXData::VST_CUSTOM
	// when custom shader is used
	r3d_assert(	mData.VertexShaderID	== PostFXData::WRONG_SHADER_ID ||
				mData.VSType			== PostFXData::VST_CUSTOM );

	switch( mData.VSType )
	{
	case PostFXData::VST_DEFAULT:
		mData.VertexShaderID		= g_pPostFXChief->GetDefaultVSId();
		break;

	case PostFXData::VST_RESTORE_W:
		mData.VertexShaderID		= g_pPostFXChief->GetRestoreWVSId();
		break;

	case PostFXData::VST_CUSTOM:
		r3d_assert( mData.VertexShaderID != PostFXData::WRONG_SHADER_ID );
		break;
	}

#ifdef R3DPROFILE_ENABLED
	nameHash = r3dHash::MakeHash(mName);
#endif
}

//------------------------------------------------------------------------
void
PostFX::Close()
{
	CloseImpl();
}

//------------------------------------------------------------------------
const PostFXData&
PostFX::Prepare( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	// place a d3d mark
	{
		char Name[ 64 ];
		char wName[ 128 ];

		Name[ 0 ] = 0;

		sscanf( mName, "class %63s", Name );

		uint32_t j = 0;
		for( uint32_t i = 0, e = strlen( Name ); i < e; i ++ )
		{
			wName[ j ++ ] = Name[ i ];
			wName[ j ++ ] = 0;
		}

		wName[ j ++ ] = 0;
		wName[ j	] = 0;

		D3DPERF_BeginEvent( 0, (WCHAR*)wName );
#ifdef R3DPROFILE_ENABLED
		r3dProfiler::StartSample(mName, nameHash);
#endif
	}

	mData.TexTransform[ 0 ] = 1.f;
	mData.TexTransform[ 1 ] = 1.f;
	mData.TexTransform[ 2 ] = 0.5f / dest->GetActualMipWidth();
	mData.TexTransform[ 3 ] = 0.5f / dest->GetActualMipHeight();

	PrepareImpl( dest, src );
	return mData;
}

//------------------------------------------------------------------------
void
PostFX::Finish()
{
	FinishImpl();

	mSettingsPushed = 0;

#ifdef R3DPROFILE_ENABLED
	r3dProfiler::EndSample(mName, nameHash);
#endif
	D3DPERF_EndEvent();

}

//------------------------------------------------------------------------

void
PostFX::PushDefaultSettings()
{
	if( !mSettingsPushed )
	{
		PushDefaultSettingsImpl();
	}

	mSettingsPushed = 0;
}

//------------------------------------------------------------------------

/*virtual*/
void
PostFX::PushDefaultSettingsImpl()
{
	
}
