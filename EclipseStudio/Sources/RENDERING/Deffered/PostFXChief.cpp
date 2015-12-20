#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PFX_Copy.h"

#include "PostFXChief.h"

extern r3dScreenBuffer* DynamicEnvMap;

extern r3dScreenBuffer* AvgSceneLuminanceBuffer;
extern r3dScreenBuffer* SceneExposure0;
extern r3dScreenBuffer* SceneExposure1;

extern r3dScreenBuffer* ScreenBuffer;
extern r3dScreenBuffer* BlurredScreenBuffer;
extern r3dScreenBuffer* TempShadowBuffer; // NOTE : Same as Scaleform_RenderToTextureRT
extern r3dScreenBuffer* DistortionBuffer;
extern r3dScreenBuffer* TempScreenBuffer;
extern r3dScreenBuffer* BlurBuffer;
extern r3dScreenBuffer* TempBuffer;
extern r3dScreenBuffer* DepthBuffer;

extern r3dScreenBuffer* One8Buffer0;
extern r3dScreenBuffer* One8Buffer1;
extern r3dScreenBuffer* One16Buffer0;
extern r3dScreenBuffer* One16Buffer1;
extern r3dScreenBuffer* One32Buffer0;
extern r3dScreenBuffer* One32Buffer1;
extern r3dScreenBuffer* One64Buffer0;
extern r3dScreenBuffer* One64Buffer1;

extern r3dScreenBuffer*	gScreenSmall;  // 1/4th of screen size
extern r3dScreenBuffer*	gScreenSmall2;  // 1/4th of screen size

extern r3dScreenBuffer*	gBuffer_Color;	// rgb - albedo, a - gloss
extern r3dScreenBuffer*	gBuffer_Normal;	// rgb - normal, a - reflectivity
extern r3dScreenBuffer*	gBuffer_Depth;  // R32 - depth
extern r3dScreenBuffer*	gBuffer_Aux;  // r - ssao channel, gb - motion blur velocities, a - self illum
extern r3dScreenBuffer* gFlashbang_Multiframe;

extern r3dScreenBuffer* PrevDepthBuffer ;
extern r3dScreenBuffer* PrevSSAOBuffer ;

extern r3dScreenBuffer* CurrentSSAOBuffer ;

extern r3dScreenBuffer* GetSSRBuffer();
extern r3dScreenBuffer* GetPrevSSRBuffer();

PostFXChief* g_pPostFXChief = 0;

static PFX_Copy gPFX_AuxiliaryCopy( R3D_BLEND_NOALPHA, PostFXChief::DEFAULT_COLOR_WRITE_MASK ) ;

//------------------------------------------------------------------------

namespace
{
	int DEFUALT_RMODE = R3D_BLEND_NOALPHA | R3D_BLEND_NZ;

	enum
	{
		DEF_STAGE_SOURCE,
		DEF_STAGE_NORMAL,
		DEF_STAGE_DEPTH,
		DEF_STAGE_AUX,
		DEF_STAGE_COUNT
	};
}

//------------------------------------------------------------------------


PostFXChief::PostFXChief()
: mZeroStageFilter( R3D_POINT )
{
	mActionFuncTable.Resize( AT_COUNT );

	mActionFuncTable[ AT_DRAW_FX		] = &PostFXChief::DrawFX;
	mActionFuncTable[ AT_DRAW_LAST_FX	] = &PostFXChief::DrawLastFX;
	mActionFuncTable[ AT_CLEAR			] = &PostFXChief::Clear;
	mActionFuncTable[ AT_CLEAR_LAST		] = &PostFXChief::ClearLast;
	mActionFuncTable[ AT_SWAPBUFFERS	] = &PostFXChief::SwapBuffers;
	mActionFuncTable[ AT_GRABSCREEN		] = &PostFXChief::GrabScreen;
}

//------------------------------------------------------------------------

PostFXChief::~PostFXChief()
{

}

//------------------------------------------------------------------------

void
PostFXChief::Init()
{
	// VSes
	{		
		mDefaultVSID	= r3dRenderer->GetVertexShaderIdx( "VS_POSTFX_DEFAULT"		);
		mRestoreWVSID	= r3dRenderer->GetVertexShaderIdx( "VS_POSTFX_RECONST_W"	);
	}

	ResetBuffers();

	mPendingActions.Reserve( 256 );
	gPFX_AuxiliaryCopy.Init() ;
}

//------------------------------------------------------------------------

void
PostFXChief::Close()
{
	gPFX_AuxiliaryCopy.Close() ;
	mPendingActions.Clear();
}

//------------------------------------------------------------------------

void
PostFXChief::AddFX( PostFX& fx, RTType dest /*= RTT_PINGPONG_NEXT*/, RTType src /*= RTT_PINGPONG_LAST*/, RECT* targetViewport /*= NULL*/ )
{
	Action act;

	act.Type			= AT_DRAW_FX;
	act.DrawFX.FX		= &fx;
	act.DrawFX.Dest		= dest;
	act.DrawFX.Source	= src;

	if( targetViewport )
	{
		act.DrawFX.TargetViewport = * targetViewport ;
	}
	else
	{
		// 0 value viewport means default
		act.DrawFX.TargetViewport.left = 0 ;
		act.DrawFX.TargetViewport.right = 0 ;
		act.DrawFX.TargetViewport.top = 0 ;
		act.DrawFX.TargetViewport.bottom = 0 ;
	}

	fx.PushDefaultSettings();

	mPendingActions.PushBack( act );
}

//------------------------------------------------------------------------

void
PostFXChief::AddClear( DWORD color, RTType dest )
{
	Action act;

	act.Type		= AT_CLEAR;
	act.Clear.Dest	= dest;
	act.Clear.Color	= 0;

	mPendingActions.PushBack( act );
}

//------------------------------------------------------------------------

void
PostFXChief::AddSwapBuffers()
{
	Action act;
	act.Type = AT_SWAPBUFFERS;

	mPendingActions.PushBack( act );
}

//------------------------------------------------------------------------

void
PostFXChief::AddGrabScreen( r3dScreenBuffer* target, RTType source )
{
	Action act ;

	act.Type				= AT_GRABSCREEN ;
	act.GrabScren.Source	= source ;
	act.GrabScren.Target	= target ;

	mPendingActions.PushBack( act ) ;
}

//------------------------------------------------------------------------

void
PostFXChief::Execute( bool toBackBuffer, bool resetTargets )
{
	if( resetTargets )
	{
		ResetBuffers();
	}

	if( !mPendingActions.Count() && !toBackBuffer )
		return;

	D3DPERF_BeginEvent( D3DCOLOR_XRGB(255,255,255), L"PostFXChief" );

	// common vertex shader constants
	{
		D3DXMATRIX InvViewProj;

		D3DXMatrixMultiply( &InvViewProj, &r3dRenderer->InvProjMatrix, &r3dRenderer->InvViewMatrix );

		D3DXMATRIX scale;

		memset( &scale, 0, sizeof scale );

		scale._11 = r3dRenderer->NearClip;
		scale._22 = r3dRenderer->NearClip;
		scale._33 = r3dRenderer->NearClip;
		scale._44 = r3dRenderer->NearClip;

		InvViewProj = scale * InvViewProj;

		D3DXMatrixTranspose( &InvViewProj, &InvViewProj );

		// NOTE : synchronize constant start with postfx_common.h
		// float4x4 	g_mInvViewProj 		: register ( c32 );
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 32, (float*)InvViewProj, 4 );
	}

	// skip ping pong stage at s0
	r3dRenderer->SetTex( gBuffer_Normal->Tex,	DEF_STAGE_NORMAL );
	r3dRenderer->SetTex( gBuffer_Depth->Tex,	DEF_STAGE_DEPTH );
	r3dRenderer->SetTex( gBuffer_Aux->Tex,		DEF_STAGE_AUX );

	COMPILE_ASSERT( DEF_STAGE_COUNT <= FREE_TEX_STAGE_START );

	SetDefaultTexFilteringTillN< NUM_SAMPLERS - 1 >();

	for( int i = 0, e = NUM_SAMPLERS; i < e; i ++ )
	{
		SetDefaultTexAddressMode( i );
	}

	IDirect3DVertexDeclaration9* prevDecl;

	prevDecl = d3dc.pDecl;
	d3dc._SetDecl( g_ScreenSpaceQuadVertexDecl );

	 r3dDrawSetFullScreenQuadVB() ;

	r3dRenderer->SetRenderingMode( DEFUALT_RMODE );

	SetDefaultColorWriteMask();

	bool canPatch = true ;

#ifndef FINAL_BUILD
	// some grab screens need last image, which may be in fact
	// operated on in final buffer due to optimizational "patching".
	// this prevents our internal buffer grabbing ( which may be
	// of larger resolution for artist screenshot work )
	for( int i = 0, e = mPendingActions.Count() ; i < e; i ++ )
	{
		if( mPendingActions[ i ].Type == PostFXChief::AT_GRABSCREEN )
		{
			canPatch = false ;
			break ;
		}
	}
#endif

	if( r_internal_width->GetInt() || r_internal_height->GetInt() )
		canPatch = false ;

	bool needCopy = false ;


	needCopy = false ;

	if( toBackBuffer )
	{
		if( canPatch )
		{
			// patch actions so that the last FXes go to final buffer
			// to avoid excessive copying.

			needCopy = PatchActions() ;
		}
		else
			needCopy = true ;
	}
		
	for( uint32_t i = 0, e = mPendingActions.Count(); i < e; i ++ )
	{
		const Action& act = mPendingActions[ i ];

		( this->*mActionFuncTable[ act.Type ] )( act );
	}

	if( needCopy )
	{
		CopyOutput();
	}

	d3dc._SetDecl( prevDecl ) ;

	mPendingActions.Clear();

	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------

int
PostFXChief::GetDefaultVSId() const
{
	return mDefaultVSID;
}

//------------------------------------------------------------------------

int
PostFXChief::GetRestoreWVSId() const
{
	return mRestoreWVSID;
}

//------------------------------------------------------------------------

void
PostFXChief::BindBufferTexture( RTType type, int stage )
{
	r3dRenderer->SetTex( mScreenBuffers[ type ]->Tex, stage );
}

//------------------------------------------------------------------------

void
PostFXChief::SetDefaultTexAddressMode( int stage )
{
	r3dRenderer->pd3ddev->SetSamplerState( stage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( stage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( stage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
}

//------------------------------------------------------------------------

namespace
{
	// first 4 stages get screen sized color/depth/normals/aux and hence do not have bilinear filtering
	// ( 0 stage can be overriden bilinear in case we render from smaller to larger target )
	// others are bilinear by default. In case you change it in a PFX you should restore in FinishImpl()
	template < int Stage >
	struct DefaultFiltering
	{
		static const r3dFilter Result = R3D_BILINEAR;
	};

	template <> struct DefaultFiltering< 0 > { static const r3dFilter Result = R3D_POINT; };
	template <> struct DefaultFiltering< 1 > { static const r3dFilter Result = R3D_POINT; };
	template <> struct DefaultFiltering< 2 > { static const r3dFilter Result = R3D_POINT; };
	template <> struct DefaultFiltering< 3 > { static const r3dFilter Result = R3D_POINT; };
}

template< int Stage >
void
PostFXChief::SetDefaultTexFiltering()
{
	r3dSetFiltering( DefaultFiltering< Stage > :: Result, Stage );	
}

//------------------------------------------------------------------------

template< int Stage >
void PostFXChief::SetDefaultTexFilteringTillN()
{
	SetDefaultTexFiltering< Stage >();
	SetDefaultTexFilteringTillN< Stage-1 >();
}

//------------------------------------------------------------------------

template<> void PostFXChief::SetDefaultTexFilteringTillN<0>()
{
	SetDefaultTexFiltering< 0 >();
}

//------------------------------------------------------------------------

template void PostFXChief::SetDefaultTexFilteringTillN< PostFXChief::NUM_SAMPLERS - 1 >();

//------------------------------------------------------------------------

void
PostFXChief::SetDefaultColorWriteMask()
{
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, DEFAULT_COLOR_WRITE_MASK ) );
}

//------------------------------------------------------------------------

r3dScreenBuffer*
PostFXChief::GetBuffer( RTType type ) const
{
	r3d_assert( type >= 0 && type < RTT_COUNT );
	return mScreenBuffers[ type ];
}

//------------------------------------------------------------------------

void
PostFXChief::ResetBuffers()
{
	extern r3dScreenBuffer*	Scaleform_RenderToTextureRT ;

	mScreenBuffers[ RTT_PINGPONG_LAST				]	= ScreenBuffer ;
	mScreenBuffers[ RTT_PINGPONG_NEXT				]	= TempScreenBuffer;
	mScreenBuffers[ RTT_PINGPONG_LAST_AS_TEMP		]	= ScreenBuffer ;
	mScreenBuffers[ RTT_PINGPONG_NEXT_AS_TEMP		]	= TempScreenBuffer;
	mScreenBuffers[ RTT_FULL0_64BIT					]	= ScreenBuffer ;
	mScreenBuffers[ RTT_FULL1_64BIT					]	= TempScreenBuffer;
	mScreenBuffers[ RTT_TEMP0_64BIT					]	= BlurredScreenBuffer;
	mScreenBuffers[ RTT_HALVED0_32BIT				]	= gScreenSmall;
	mScreenBuffers[ RTT_HALVED1_32BIT				]	= gScreenSmall2;
	mScreenBuffers[ RTT_ONEFOURTH0_64BIT			]	= TempBuffer;
	mScreenBuffers[ RTT_ONEFOURTH1_64BIT			]	= BlurBuffer;
	mScreenBuffers[ RTT_ONE8_0_64BIT				]	= One8Buffer0;
	mScreenBuffers[ RTT_ONE8_1_64BIT				]	= One8Buffer1;
	mScreenBuffers[ RTT_ONE16_0_64BIT				]	= One16Buffer0;
	mScreenBuffers[ RTT_ONE16_1_64BIT				]	= One16Buffer1;
	mScreenBuffers[ RTT_ONE32_0_64BIT				]	= One32Buffer0;
	mScreenBuffers[ RTT_ONE32_1_64BIT				]	= One32Buffer1;
	mScreenBuffers[ RTT_ONE64_0_64BIT				]	= One64Buffer0;
	mScreenBuffers[ RTT_ONE64_1_64BIT				]	= One64Buffer1;
	mScreenBuffers[ RTT_AUX_32BIT					]	= gBuffer_Aux;
	mScreenBuffers[ RTT_DIFFUSE_32BIT				]	= gBuffer_Color;
	mScreenBuffers[ RTT_DEPTH_32BIT					]	= gBuffer_Depth;
	mScreenBuffers[ RTT_MLAA_LINES_H				]	= gBuffer_MLAA_LinesH;
	mScreenBuffers[ RTT_MLAA_LINES_V				]	= gBuffer_MLAA_LinesV;
	mScreenBuffers[ RTT_NORMALS_32BIT				]	= gBuffer_Normal;
	mScreenBuffers[ RTT_FLASHBANG_MULTIFRAME_16BIT	]	= gFlashbang_Multiframe;
	mScreenBuffers[ RTT_DISTORTION_32BIT			]	= DistortionBuffer;
	mScreenBuffers[ RTT_UI_CHARACTER_32BIT			]	= Scaleform_RenderToTextureRT ;

	mScreenBuffers[ RTT_PREV_FRAME_DEPTH		]	= PrevDepthBuffer ;
	mScreenBuffers[ RTT_PREV_FRAME_SSAO			]	= PrevSSAOBuffer ;

	mScreenBuffers[ RTT_AVG_SCENE_LUMA			]	= AvgSceneLuminanceBuffer ;
	
	mScreenBuffers[ RTT_SCENE_EXPOSURE0			]	= SceneExposure0 ;
	mScreenBuffers[ RTT_SCENE_EXPOSURE1			]	= SceneExposure1 ;

	mScreenBuffers[RTT_SCENE_SSR						] = GetSSRBuffer();
	mScreenBuffers[RTT_SCENE_PREVSSR				] = GetPrevSSRBuffer();


}

//------------------------------------------------------------------------

static r3dTL::TArray< r3dString > g_PostFXNames;

void
PostFXChief::DEBUG_MakePostFXSnapshot()
{
	g_PostFXNames.Clear();

	for( int i = 0, e = (int)mPendingActions.Count(); i < e; i ++ )
	{
		Action& act = mPendingActions[ i ];

		if( act.Type == PostFXChief::AT_DRAW_FX )
		{
			g_PostFXNames.PushBack( act.DrawFX.FX->GetName() );
		}
	}
}

//------------------------------------------------------------------------

void
PostFXChief::DEBUG_PrintPostFXes()
{
	const float YSTART = 80.0f;

	float x = 20.0f, y = YSTART;
	for( int i = 0, e = (int)g_PostFXNames.Count(); i < e; i ++ )
	{
		int addme = 0;
		if( !memcmp( g_PostFXNames[ i ].c_str(), "class PFX_", sizeof "class PFX_" - 1 ) )
			addme = sizeof "class PFX_" - 1;

		_r3dSystemFont->PrintF( x, y, r3dColor::white, g_PostFXNames[ i ].c_str() + addme );

		if( !( ( i + 1 ) % 30 ) )
		{
			x += 280.0f;
			y = YSTART;
		}
		else
		{
			y += 11.f;
		}
	}
}

//------------------------------------------------------------------------

bool
PostFXChief::PatchActions()
{
	bool needCopy = true;
	uint32_t actionCount = mPendingActions.Count();

	bool pingPongLastSourceGoesFirst = false ;

	for( int i = actionCount - 1; i >= 0; i -- )
	{
		Action& act = mPendingActions[ i ];

		switch( act.Type )
		{
		case AT_DRAW_FX:
			if( act.DrawFX.Dest == RTT_PINGPONG_NEXT )
			{
				i = -1 ;
			}
			else
			{
				if( act.DrawFX.Source == RTT_PINGPONG_LAST )
				{
					pingPongLastSourceGoesFirst = true ;
					i = -1 ;
				}
			}
			break ;
		}
	}

	if( !pingPongLastSourceGoesFirst )
	{
		// patch last RTT_PINGPONG_NEXT output to draw to final screen

		int i;
		for( i = actionCount - 1; i >= 0; i -- )
		{
			Action& act = mPendingActions[ i ];

			switch( act.Type )
			{
			case AT_DRAW_FX:
				if( act.DrawFX.Dest == RTT_PINGPONG_NEXT )
				{
					act.Type = AT_DRAW_LAST_FX;
					needCopy = false;
				}
				break;

			case AT_CLEAR:
				if( act.Clear.Dest == RTT_PINGPONG_NEXT )
				{
					act.Type = AT_CLEAR_LAST;
					needCopy = false;
				}
				break;
			}

			if( !needCopy )
				break;
		}

		i ++;

		if( !needCopy )
		{
			// patch all succeeding RTT_PINGPONG_LAST outputs to go to final screen
			// (these must be ADD type effects)

			for( ; i < (int)actionCount; i ++ )
			{
				Action& act = mPendingActions[ i ];

				if( act.Type == AT_DRAW_FX && act.DrawFX.Dest == RTT_PINGPONG_LAST )
				{
					act.Type = AT_DRAW_LAST_FX;
				}
			}
		}
	}

	return needCopy ;
}

//------------------------------------------------------------------------

void
PostFXChief::CopyOutput()
{
	r3dScreenBuffer * src	= mScreenBuffers[ RTT_PINGPONG_LAST ];
	r3dScreenBuffer * dest	= mScreenBuffers[ RTT_PINGPONG_NEXT	];

	PrepareBackBufferRender();

	PFX_Copy::Settings sts ;

	RECT rect = { 0, 0, 0, 0 } ;

	if( r_internal_width->GetInt() || r_internal_height->GetInt() )
	{
		sts.ForceFiltering = true ;

		rect.right = (int)r3dRenderer->ScreenW ;
		rect.bottom = (int)r3dRenderer->ScreenH ;
	}

	gPFX_Copy.PushSettings( sts );

	DoDrawFX( &gPFX_Copy, src, dest, rect );
}

//------------------------------------------------------------------------

void
PostFXChief::DEBUG_SaveStates()
{
#ifdef _DEBUG
	for( int i = 0, e = NUM_SAMPLERS; i < e; i ++ )
	{
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_ADDRESSU, mDEBUG_AddressMode[i] + 0 ) );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_ADDRESSV, mDEBUG_AddressMode[i] + 1 ) );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_ADDRESSW, mDEBUG_AddressMode[i] + 2 ) );

		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_MINFILTER, mDEBUG_Filtering[i] + 0 ) );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_MAGFILTER, mDEBUG_Filtering[i] + 1 ) );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_MIPFILTER, mDEBUG_Filtering[i] + 2 ) );

		D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_COLORWRITEENABLE, &mDEBUG_ColorWriteMask ) );
	}
#endif
}

//------------------------------------------------------------------------

void
PostFXChief::DEBUG_CheckStates()
{
#ifdef _DEBUG
	for( int i = 0, e = NUM_SAMPLERS; i < e; i ++ )
	{
		DWORD val;
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_ADDRESSU, &val ) );
		r3d_assert( mDEBUG_AddressMode[i][0] == val );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_ADDRESSV, &val ) );
		r3d_assert( mDEBUG_AddressMode[i][1] == val );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_ADDRESSW, &val ) );
		r3d_assert( mDEBUG_AddressMode[i][2] == val );

		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_MINFILTER, &val ) );
		r3d_assert( mDEBUG_Filtering[i][0] == val );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_MAGFILTER, &val ) );
		r3d_assert( mDEBUG_Filtering[i][1] == val );
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState( i, D3DSAMP_MIPFILTER, &val ) );
		r3d_assert( mDEBUG_Filtering[i][2] == val );

		D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_COLORWRITEENABLE, &val ) );
		r3d_assert( mDEBUG_ColorWriteMask == val );

		IDirect3DBaseTexture9* normals( NULL );
		D3D_V( r3dRenderer->pd3ddev->GetTexture( DEF_STAGE_NORMAL, &normals ) );

		IDirect3DBaseTexture9* depth( NULL );
		D3D_V( r3dRenderer->pd3ddev->GetTexture( DEF_STAGE_DEPTH, &depth ) );

		IDirect3DBaseTexture9* aux( NULL );
		D3D_V( r3dRenderer->pd3ddev->GetTexture( DEF_STAGE_AUX, &aux ) );

		r3d_assert( normals == gBuffer_Normal->AsTex2D() );
		r3d_assert( depth == gBuffer_Depth->AsTex2D() );
		r3d_assert( aux == gBuffer_Aux->AsTex2D() );

		normals->Release();
		depth->Release();
		aux->Release();
	}
#endif
}

//------------------------------------------------------------------------

void
PostFXChief::PatchDefaultTextures( r3dScreenBuffer* dest )
{
	if( dest == gBuffer_Normal )
	{
		r3dRenderer->SetTex( NULL, DEF_STAGE_NORMAL );
	}
}

//------------------------------------------------------------------------

void
PostFXChief::DepatchDefaultTextures( r3dScreenBuffer* dest )
{
	if( dest == gBuffer_Normal )
	{
		r3dRenderer->SetTex( gBuffer_Normal->Tex, DEF_STAGE_NORMAL );
	}
}

//------------------------------------------------------------------------

void
PostFXChief::DoDrawFX( PostFX* PFX, r3dScreenBuffer* src, r3dScreenBuffer* dest, RECT viewportRect )
{
	if( viewportRect.left == viewportRect.right && viewportRect.right == 0 )
	{
		viewportRect.left = 0 ;
		viewportRect.right = (int)dest->GetActualMipWidth() ;
		viewportRect.top = 0 ;
		viewportRect.bottom = (int)dest->GetActualMipHeight() ;
	}

	int destWidth = viewportRect.right - viewportRect.left ;
	int destHeight = viewportRect.bottom - viewportRect.top ;

	if( destWidth > src->Width ||
		destHeight > src->Height )
	{
		mZeroStageFilter = R3D_BILINEAR;
	}
	else
	{
		mZeroStageFilter = R3D_POINT;
	}

	r3dSetFiltering( mZeroStageFilter, 0 );

	DEBUG_SaveStates();

	r3dRenderer->SetViewport( (float) viewportRect.left, (float) viewportRect.top, (float) destWidth, (float) destHeight ) ;

	const PostFXData& data = PFX->Prepare( dest, src );

	PatchDefaultTextures( dest );

	r3dRenderer->SetTex( src->Tex, 0 );

	{
		// NOTE : synchronize constant start with postfx_common.h
		// float4 		g_vTexcTransform	: register ( c36 );
		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, data.TexTransform, 1 ) );
	}

	r3d_assert( data.VertexShaderID != data.WRONG_SHADER_ID && 
				data.PixelShaderID	!= data.WRONG_SHADER_ID );

	r3dRenderer->SetVertexShader( data.VertexShaderID );
	r3dRenderer->SetPixelShader( data.PixelShaderID );

	r3dPushExtralErrorInfo( PFX->GetName() ) ;

	r3dRenderer->Draw( D3DPT_TRIANGLESTRIP, 0, 2 );

	r3dPopExtraErrorInfo() ;

	DepatchDefaultTextures( dest );

	PFX->Finish();

	DEBUG_CheckStates();

	// fx should always restore the mode in case it changes it 
	// ( use R3D_BLEND_PUSH/R3D_BLEND_POP )
	r3d_assert( r3dRenderer->GetRenderingMode() == DEFUALT_RMODE );
}

//------------------------------------------------------------------------

void
PostFXChief::DoClear( const Action& act )
{
	D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, act.Clear.Color, r3dRenderer->GetClearZValue(), 0 ) );
}

//------------------------------------------------------------------------

void
PostFXChief::PrepareBackBufferRender()
{
	r3dRenderer->SetBackBufferViewport();
}

//------------------------------------------------------------------------

void
PostFXChief::DrawFX( const Action& act )
{
	PostFX* PFX = act.DrawFX.FX;

	r3dScreenBuffer * src	= mScreenBuffers[ act.DrawFX.Source ];
	r3dScreenBuffer * dest	= mScreenBuffers[ act.DrawFX.Dest ];

	dest->Activate();

	DoDrawFX( PFX, src, dest, act.DrawFX.TargetViewport );

	dest->Deactivate();
}

//------------------------------------------------------------------------

void
PostFXChief::DrawLastFX( const Action& act )
{
	PostFX* PFX = act.DrawFX.FX;

	r3dScreenBuffer * src	= mScreenBuffers[ act.DrawFX.Source ];
	r3dScreenBuffer * dest	= mScreenBuffers[ act.DrawFX.Dest ];

	PrepareBackBufferRender();

	DoDrawFX( PFX, src, dest, act.DrawFX.TargetViewport );
}

//------------------------------------------------------------------------

void
PostFXChief::Clear( const Action& act )
{
	r3dScreenBuffer * dest = mScreenBuffers[ act.Clear.Dest ];

	dest->Activate();

	DoClear( act );

	dest->Deactivate();
}

//------------------------------------------------------------------------

void
PostFXChief::ClearLast( const Action& act )
{
	DoClear( act );
}

//------------------------------------------------------------------------

void
PostFXChief::SwapBuffers( const Action& act )
{
	std::swap( mScreenBuffers[ RTT_PINGPONG_LAST ], mScreenBuffers[ RTT_PINGPONG_NEXT ] );
	std::swap( mScreenBuffers[ RTT_PINGPONG_LAST_AS_TEMP ], mScreenBuffers[ RTT_PINGPONG_NEXT_AS_TEMP ] );
}

//------------------------------------------------------------------------

void
PostFXChief::GrabScreen( const Action& act )
{
	r3dScreenBuffer* source		= mScreenBuffers[ act.GrabScren.Source ] ;

	act.GrabScren.Target->Activate() ;

	gPFX_AuxiliaryCopy.PushDefaultSettings() ;

	RECT rect = { 0, 0, 0, 0 } ;

	DoDrawFX( &gPFX_AuxiliaryCopy, source, act.GrabScren.Target, rect ) ;

	act.GrabScren.Target->Deactivate() ;
}

PostFXChief g_PostFXChief;