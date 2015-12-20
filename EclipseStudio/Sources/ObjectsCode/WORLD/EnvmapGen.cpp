#include "r3dPCH.h"
#include "r3d.h"

#include "ObjectsCode/world/EnvmapProbes.h"
#include "../../RENDERING/Deffered/HUDFilters.h"

#include "RENDERING/Deffered/RenderDeffered.h"

#include "EnvmapGen.h"

enum FaceEdge
{
	FACEEDGE_LEFT,
	FACEEDGE_RIGHT,
	FACEEDGE_TOP,
	FACEEDGE_BOTTOM
};

enum FaceCorner
{
	FACECORNER_LEFTTOP,
	FACECORNER_RIGHTTOP,
	FACECORNER_BOTTOMLEFT,
	FACECORNER_BOTTOMRIGHT
};

extern	r3dScreenBuffer*	ScreenBuffer;
extern	r3dCamera			gCam;
extern	float 				__r3dGlobalAspect;
extern	bool				g_bEditMode;
extern	int					g_RenderRadialBlur;

void Stitch3CubeMapCorners( r3dScreenBuffer* tempRT, int dim, D3DCUBEMAP_FACES face0, FaceCorner faceCorner0, D3DCUBEMAP_FACES face1, FaceCorner faceCorner1, D3DCUBEMAP_FACES face2, FaceCorner faceCorner2 );

void GameRender();
void UpdateZPrepassSettings();
void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

struct plain_float4
{
	float a;
	float r;
	float g;
	float b;
};

struct EnvmapConvert16U
{
	static const D3DFORMAT RENDER_FMT = D3DFMT_A8R8G8B8;
	static const D3DFORMAT TARGET_FMT = D3DFMT_R5G6B5;
	typedef unsigned char RENDER_CHANNEL_FORMAT_TYPE;
	typedef unsigned RENDER_FORMAT_TYPE;
	typedef unsigned INTENSITY_TYPE;

	static const unsigned ZERO_VAL = 0;
};

struct EnvmapConvert64F
{
	static const D3DFORMAT RENDER_FMT = D3DFMT_A32B32G32R32F;
	static const D3DFORMAT TARGET_FMT = D3DFMT_A16B16G16R16F;
	typedef float RENDER_CHANNEL_FORMAT_TYPE;
	typedef plain_float4 RENDER_FORMAT_TYPE;
	typedef float INTENSITY_TYPE;

	static const plain_float4 ZERO_VAL;
};

const plain_float4 EnvmapConvert64F::ZERO_VAL = { 0.f, 0.f, 0.f, 0.f };

template< typename Config >
void GenerateEnvmapT( r3dTexture* tex, const r3dString& texName, const r3dPoint3D& pos, int LDR )
{
#ifdef FINAL_BUILD
	return;
#else
	R3D_ENSURE_MAIN_THREAD();

	r3d_assert( tex->isCubemap() );

	IDirect3DCubeTexture9* cube;

	struct EnableDisableDistanceCull
	{
		EnableDisableDistanceCull()
		{
			oldValue = r_allow_distance_cull->GetInt();
			r_allow_distance_cull->SetInt( 0 );
		}

		~EnableDisableDistanceCull()
		{
			r_allow_distance_cull->SetInt( oldValue );
		}

		int oldValue;

	} enableDisableDistanceCull; (void)enableDisableDistanceCull;

	// tell' em all not to draw themselves
	struct ClearRestoreRenderSettings
	{
		ClearRestoreRenderSettings()
		{
			prevEditorInitted			= g_bEditMode;
			prevColorCorrection			= gHUDFilterSettings[HUDFilter_Default].enableColorCorrection;
			prevRadialBlur				= g_RenderRadialBlur;
			prevHideIcons				= r_hide_icons->GetInt();
			prevOcclusionQueryEnabled	= r_use_oq->GetInt();
			prevZPrepass				= r_z_allow_prepass->GetInt();
			prevDeviceClears			= r_device_clear->GetInt();

			savedCam					= gCam;

			g_bEditMode					= false;
			g_RenderRadialBlur			= false;
			gHUDFilterSettings[HUDFilter_Default].enableColorCorrection = 0;
			r_hide_icons->SetInt(1);
			r_use_oq->SetInt(0);
			r_z_allow_prepass->SetInt(0);
			r_device_clear->SetInt(1);

			UpdateZPrepassSettings();
			g_EnvmapProbes.SetForceGlobal( true );
		}

		~ClearRestoreRenderSettings()
		{
			g_bEditMode				= prevEditorInitted;
			g_RenderRadialBlur		= prevRadialBlur;
			gHUDFilterSettings[HUDFilter_Default].enableColorCorrection	= prevColorCorrection;
			r_hide_icons->SetInt(prevHideIcons);
			r_use_oq->SetInt(prevOcclusionQueryEnabled);
			r_z_allow_prepass->SetInt(prevZPrepass);
			r_device_clear->SetInt( prevDeviceClears );

			gCam					= savedCam;

			UpdateZPrepassSettings();
			g_EnvmapProbes.SetForceGlobal( false );
		}

		bool prevEditorInitted;
		int prevColorCorrection;
		int prevRadialBlur;
		int prevOcclusionQueryEnabled;
		int prevHideIcons;
		int prevZPrepass;
		int prevDeviceClears;

		r3dCamera savedCam;

	} clearRestoreRenderSettings; (void)clearRestoreRenderSettings;

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"CUBE MAP RENDER");

	r3d_assert( tex->GetWidth() == tex->GetHeight() );

	if( LDR )
	{
		D3D_V( r3dRenderer->pd3ddev->CreateCubeTexture(	tex->GetWidth(), 1, D3DUSAGE_RENDERTARGET, Config::RENDER_FMT, D3DPOOL_DEFAULT, &cube, NULL ) );
	}
	else
	{
		D3D_V( r3dRenderer->pd3ddev->CreateCubeTexture(	tex->GetWidth(), 1, 0, Config::RENDER_FMT, D3DPOOL_SYSTEMMEM, &cube, NULL ) );
	}

	SetD3DResourcePrivateData(tex->GetD3DTexture(), "GenerateEnvmap: Color" );

	r3dPoint3D dirs[] = {	r3dPoint3D( +1, +0, +0 ), 
							r3dPoint3D( -1, +0, +0 ),
							r3dPoint3D( +0, +1, +0 ),
							r3dPoint3D( +0, -1, +0 ),
							r3dPoint3D( +0, +0, +1 ),
							r3dPoint3D( +0, +0, -1 ) };

	r3dPoint3D ups[] = {	r3dPoint3D( +0, +1, +0 ), 
							r3dPoint3D( +0, +1, +0 ),
							r3dPoint3D( +0, +0, -1 ),
							r3dPoint3D( +0, +0, +1 ),
							r3dPoint3D( +0, +1, +0 ),
							r3dPoint3D( +0, +1, +0 ) };

	gCam			= pos;
	gCam.FOV		= 90;
	gCam.Aspect		= 1.0f;

	for( int i = 0, e = sizeof dirs / sizeof dirs[ 0 ]; i < e; i ++ )
	{
		gCam.vPointTo	= dirs[ i ];
		gCam.vUP		= ups[ i ];

		r3dRenderer->SetCamera( gCam, false );

		GameRender();

		if( LDR )
		{
			IDirect3DSurface9 *srcSurf( NULL );
			IDirect3DSurface9* cubeSizedRT( NULL );

			g_pPostFXChief->AddFX( gPFX_ConvertToLDR );

			g_pPostFXChief->Execute( false, true );

			D3D_V( g_pPostFXChief->GetBuffer( PostFXChief::RTT_PINGPONG_NEXT )->Tex->AsTex2D()->GetSurfaceLevel( 0, &srcSurf ) );

			D3D_V( cube->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &cubeSizedRT ) );

#if 0
			D3D_V( D3DXSaveTextureToFile( (r3dSTLString("cubeface") + char('0' + i ) + ".dds").c_str(), D3DXIFF_DDS, ScreenBuffer->Tex->AsTex2D(), NULL ) );
#endif

			D3D_V( r3dRenderer->pd3ddev->StretchRect( srcSurf, NULL, cubeSizedRT, NULL, D3DTEXF_LINEAR ) );

			srcSurf->Release();
			cubeSizedRT->Release();
		}
		else
		{
			IDirect3DSurface9* tempSurf( NULL );
			IDirect3DSurface9* cubeSurf( NULL );

			D3D_V( cube->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &cubeSurf ) );

			D3D_V( g_pPostFXChief->GetBuffer( PostFXChief::RTT_PINGPONG_LAST )->Tex->AsTex2D()->GetSurfaceLevel( 0, &tempSurf ) );

			ID3DXBuffer* buff( NULL );
			D3DXSaveSurfaceToFileInMemory( &buff, D3DXIFF_DDS, tempSurf, NULL, NULL );
			D3DXLoadSurfaceFromFileInMemory( cubeSurf, NULL, NULL, buff->GetBufferPointer(), buff->GetBufferSize(), NULL,  D3DX_DEFAULT, 0, NULL );

			SAFE_RELEASE( buff );
			SAFE_RELEASE( tempSurf );
			SAFE_RELEASE( cubeSurf );
		}
	}

	LPD3DXBUFFER derBuffer( NULL );

	D3D_V( D3DXSaveTextureToFileInMemory( &derBuffer, D3DXIFF_DDS, cube, NULL ) );

	cube->Release();

	IDirect3DCubeTexture9* convertedTex( NULL );

	// generate mips
	D3D_V( D3DXCreateCubeTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, 
													derBuffer->GetBufferPointer(), derBuffer->GetBufferSize(),
													tex->GetWidth(), 0, 0, Config::RENDER_FMT, D3DPOOL_SYSTEMMEM, 
													D3DX_FILTER_LINEAR, D3DX_FILTER_BOX, 0, NULL, NULL, &convertedTex ) );

	int mips = 0;

	int w = tex->GetWidth();

	for(; w; w /= 2 )
	{
		mips ++;	
	}

	for( int i = 1, e = mips; i < e; i ++ )
	{
		D3DLOCKED_RECT lrects[ 6 ];

		D3DSURFACE_DESC desc;
		convertedTex->GetLevelDesc( i, &desc );

		int dim = desc.Width;
		
		for( int f = 0; f < 6; f ++ )
		{
			D3D_V( convertedTex->LockRect( D3DCUBEMAP_FACES( f ), i, &lrects[ f ], NULL, 0 ) );
		}

		struct Avarage
		{
			R3D_FORCEINLINE Config::RENDER_FORMAT_TYPE operator() ( Config::RENDER_FORMAT_TYPE v0, Config::RENDER_FORMAT_TYPE v1 )
			{
				union RGBA
				{
					Config::RENDER_CHANNEL_FORMAT_TYPE rgba[ 4 ];
					Config::RENDER_FORMAT_TYPE val;
				} a, b;

				a.val = v0;
				b.val = v1;

				for( int i = 0; i < 4; i ++ )
				{
					a.rgba[ i ] = ( b.rgba[ i ] + a.rgba[ i ] ) / 2;
				}

				return a.val;
			}

			R3D_FORCEINLINE Config::RENDER_FORMAT_TYPE operator() ( Config::RENDER_FORMAT_TYPE v0, Config::RENDER_FORMAT_TYPE v1, Config::RENDER_FORMAT_TYPE v2 )
			{
				union RGBA
				{
					Config::RENDER_CHANNEL_FORMAT_TYPE rgba[ 4 ];
					Config::RENDER_FORMAT_TYPE val;
				} a, b, c;

				a.val = v0;
				b.val = v1;
				c.val = v2;

				for( int i = 0; i < 4; i ++ )
				{
					a.rgba[ i ] = ( a.rgba[ i ] + b.rgba[ i ] + c.rgba[ i ] ) / 3;
				}

				return a.val;
			}

		};

		struct
		{
			void operator () ( void* vp0, void* vp1, int step0, int step1, int count )
			{
				Config::RENDER_FORMAT_TYPE *p0 = (Config::RENDER_FORMAT_TYPE*)vp0;
				Config::RENDER_FORMAT_TYPE *p1 = (Config::RENDER_FORMAT_TYPE*)vp1;

				Avarage avg;
				for( int i = 0, e = count; i < e; i ++ )
				{
					Config::RENDER_FORMAT_TYPE a = avg( *p0, *p1 );
					*p0 = a;
					*p1 = a;

					p0 += step0;
					p1 += step1;
				}
			}

			void operator () ( void* vp0, void* vp1, void* vp2 )
			{
				Config::RENDER_FORMAT_TYPE *p0 = (Config::RENDER_FORMAT_TYPE*)vp0;
				Config::RENDER_FORMAT_TYPE *p1 = (Config::RENDER_FORMAT_TYPE*)vp1;
				Config::RENDER_FORMAT_TYPE *p2 = (Config::RENDER_FORMAT_TYPE*)vp2;

				Avarage avg;
		
				Config::RENDER_FORMAT_TYPE a = avg( *p0, *p1, *p2 );

				*p0 = a;
				*p1 = a;
				*p2 = a;
			}
		} fixup; fixup;

#if 0
		struct
		{
			void operator () ( void* vp0, void* vp1, int step0, int step1, int count, int col )
			{
				int *p0 = (int*)vp0;
				int *p1 = (int*)vp1;

				for( int i = 0, e = count; i < e; i ++ )
				{
					*p0 = col * i / ( count - 1 );
					*p1 = col * i / ( count - 1 );

					p0 += step0;
					p1 += step1;
				}
			}

			void operator () ( void* vp0, void* vp1, void* vp2, int col )
			{
				int *p0 = (int*)vp0; *p0 = col;
				int *p1 = (int*)vp1; *p1 = col;
				int *p2 = (int*)vp2; *p2 = col;
			}
		} show; show;
#endif
		
		if( dim > 1 )
		{
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits + 2 * dim - 1,	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits + dim, dim, dim, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits + dim,			(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits + 2 * dim - 1, dim, dim, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits + dim,			(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits + 2 * dim - 1, dim, dim, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits + 2 * dim - 1,	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits + dim, dim, dim, dim - 2 );

			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits + dim - 2,		(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits + 2 * dim - 1, -1, dim, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits + 1,				(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits + dim, 1, dim, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits + 1,				(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits + (dim - 1) * dim + 1, 1, 1, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits + dim - 2,		(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits + 1, -1, 1, dim - 2 );

			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits + dim * ( dim - 1 ) + 1,			(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits + 2 * dim - 1, 1, dim, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits + dim * ( dim - 1 ) + dim - 2,	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits + dim, -1, dim, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits + dim * ( dim - 1 ) + 1,			(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits + 1, 1, 1, dim - 2 );
			fixup( (Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits + dim * ( dim - 1 ) + dim - 2,	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits + (dim - 1) * dim + 1, -1, 1, dim - 2 );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits + dim * ( dim - 1 ), 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits + dim - 1,
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits + dim - 1 + dim * ( dim - 1 ) );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits + dim - 1 + dim * ( dim - 1 ), 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits,
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits + dim * ( dim - 1 ) );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits + dim * ( dim - 1 ), 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits + dim * ( dim - 1 ),
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits + dim - 1 + dim * ( dim - 1 ) );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits + dim - 1 + dim * ( dim - 1 ), 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Y ].pBits + dim * ( dim - 1 ) + dim - 1,				
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits + dim * ( dim - 1 ) );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits, 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits + dim * ( dim - 1 ) + dim - 1,
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits + dim - 1 );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits + dim - 1, 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits + dim * ( dim - 1 ),
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Z ].pBits );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_X ].pBits, 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits,
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits + dim - 1 );

			fixup(	(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_X ].pBits + dim - 1, 
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_POSITIVE_Y ].pBits + dim - 1,
					(Config::RENDER_FORMAT_TYPE*)lrects[ D3DCUBEMAP_FACE_NEGATIVE_Z ].pBits );
		}
		else
		{
			// Lowest mip. Have to make same color for edgeless rendering on "non seamless cubemap" cards
			// find max to make everything "skylike"

			struct
			{
				void operator() ( D3DLOCKED_RECT* lrect )
				{
					union RGBA
					{
						Config::RENDER_CHANNEL_FORMAT_TYPE rgba[ 4 ];
						Config::RENDER_FORMAT_TYPE val;
					} a;

					a.val = *(Config::RENDER_FORMAT_TYPE*)lrect->pBits;

					Config::INTENSITY_TYPE intensity = a.rgba[ 0 ] + a.rgba[ 1 ] + a.rgba[ 2 ];

					if( intensity > maxIntens )
					{
						maxIntens = intensity;
						maxVal = a.val;
					}

				}

				Config::RENDER_FORMAT_TYPE maxVal;
				Config::INTENSITY_TYPE maxIntens;

			} maxColor = { Config::ZERO_VAL, 0 };

			for( int f = 0; f < 6; f ++ )
			{
				maxColor( lrects + f );
			}

			for( int f = 0; f < 6; f ++ )
			{
				*(Config::RENDER_FORMAT_TYPE*)lrects[ f ].pBits = maxColor.maxVal;
			}
		}

		for( int f = 0; f < 6; f ++ )
		{
			D3D_V( convertedTex->UnlockRect( D3DCUBEMAP_FACES( f ), i ) );
		}
	}

	derBuffer->Release();

	D3D_V( D3DXSaveTextureToFileInMemory( &derBuffer, D3DXIFF_DDS, convertedTex, NULL ) );

	convertedTex->Release();

	D3D_V( D3DXCreateCubeTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, 
													derBuffer->GetBufferPointer(), derBuffer->GetBufferSize(),
													tex->GetWidth(), 0, 0, Config::TARGET_FMT, D3DPOOL_MANAGED, 
													D3DTEXF_LINEAR, D3DTEXF_LINEAR, 0, NULL, NULL, &convertedTex ) );	

	D3D_V( D3DXSaveTextureToFile( texName.c_str(), D3DXIFF_DDS, convertedTex, NULL ) );
	tex->SetNewD3DTexture(convertedTex);

	derBuffer->Release();

	D3DPERF_EndEvent();
#endif
}

float4 EdgeToPosXForm( int targetDim, FaceEdge edge, float fixOffset )
{
#if 0
	float4 result( 1.f, 1.f, ( fixOffset ?  r_0debug_helper->GetFloat() : 0.f ) / targetDim, 0.0f / targetDim );
#else
	float4 result( 1.f, 1.f, -fixOffset / targetDim, 0.0f / targetDim );
#endif

#if 0
	const float INV_DIM = 2.0f / targetDim;

	switch( edge )
	{
	case FACEEDGE_LEFT:
		result = float4( INV_DIM, 1.0f, -1.0f, 0.f );
		break;
	case FACEEDGE_RIGHT:
		result = float4( INV_DIM, 1.0f, +1.0f, 0.f );
		break;
	case FACEEDGE_TOP:
		result = float4( 1.0f, INV_DIM, 0.f + r_0debug_helper->GetFloat(), +1.0f + r_1debug_helper->GetFloat());
		break;
	case FACEEDGE_BOTTOM:
		result = float4( 1.0f, INV_DIM, 0.f + r_0debug_helper->GetFloat(), -1.0f + r_1debug_helper->GetFloat() );
		break;
	}
#endif

	return result;
}

float4 CornerToPosXForm( int targetDim, FaceCorner corner )
{
	float4 result( 1.f, 1.f, 0.f, 0.f );

#if 0
	const float PIX_SIZE = 1.0f / targetDim;
	const float INV_DIM = 1.0f / targetDim;

	switch( corner )
	{
	case FACECORNER_LEFTTOP:
		result = float4( INV_DIM, INV_DIM, -1.0f + PIX_SIZE, +1.0f - PIX_SIZE );
		break;
	case FACECORNER_RIGHTTOP:
		result = float4( INV_DIM, INV_DIM, +1.0f - PIX_SIZE, +1.0f - PIX_SIZE );
		break;
	case FACECORNER_BOTTOMLEFT:
		result = float4( INV_DIM, INV_DIM, -1.0f + PIX_SIZE, -1.0f + PIX_SIZE );
		break;
	case FACECORNER_BOTTOMRIGHT:
		result = float4( INV_DIM, INV_DIM, +1.0f - PIX_SIZE, -1.0f + PIX_SIZE );
		break;
	}
#endif

	return result;
}

void CornerToViewportSourceRect( RECT *oRect, int targetDim, FaceCorner corner )
{
	switch( corner )
	{
	case FACECORNER_LEFTTOP:
		oRect->left = 0;
		oRect->right = 1;
		oRect->top = 0;
		oRect->bottom = 1;
		break;
	case FACECORNER_RIGHTTOP:
		oRect->left = targetDim - 1;
		oRect->right = targetDim;
		oRect->top = 0;
		oRect->bottom = 1;
		break;
	case FACECORNER_BOTTOMLEFT:
		oRect->left = 0;
		oRect->right = 1;
		oRect->top = targetDim - 1;
		oRect->bottom = targetDim;
		break;
	case FACECORNER_BOTTOMRIGHT:
		oRect->left = targetDim - 1;
		oRect->right = targetDim;
		oRect->top = targetDim - 1;
		oRect->bottom = targetDim;
		break;
	}
}

void EdgeToViewportTargetRect( RECT *oRect, int texDim, FaceEdge edge, int face )
{
	switch( edge )
	{
	case FACEEDGE_RIGHT:
		oRect->left = face * 2;
		oRect->right = face * 2 + 1;
		oRect->top = 0;
		oRect->bottom = texDim;
		break;
	case FACEEDGE_LEFT:
		oRect->left = face * 2 + 1;
		oRect->right = face * 2 + 2;
		oRect->top = 0;
		oRect->bottom = texDim;
		break;
	case FACEEDGE_TOP:
		oRect->left = 12;
		oRect->right = texDim + 12;
		oRect->top = face * 2;
		oRect->bottom = face * 2 + 1;
		break;
	case FACEEDGE_BOTTOM:
		oRect->left = 12;
		oRect->right = texDim + 12;
		oRect->top = face * 2 + 1;
		oRect->bottom = face * 2 + 2;
		break;
	}
}

void EdgeToViewportSourceRect( RECT *oRect, int texDim, FaceEdge edge )
{
	switch( edge )
	{
	case FACEEDGE_LEFT:
		oRect->left = 0;
		oRect->right = 1;
		oRect->top = 0;
		oRect->bottom = texDim;
		break;
	case FACEEDGE_RIGHT:
		oRect->left = texDim - 1;
		oRect->right = texDim;
		oRect->top = 0;
		oRect->bottom = texDim;
		break;
	case FACEEDGE_TOP:
		oRect->left = 0;
		oRect->right = texDim;
		oRect->top = 0;
		oRect->bottom = 1;
		break;
	case FACEEDGE_BOTTOM:
		oRect->left = 0;
		oRect->right = texDim;
		oRect->top = texDim - 1;
		oRect->bottom = texDim;
		break;
	}
}

void EdgeToTexcXForm( float4* oRes, int dim, int targWidth, int targHeight, int face, FaceEdge edge, int inverseCoord )
{
	float hk = float( dim ) / targWidth;
	float vk = float( dim ) / targHeight;

	switch( edge )
	{
	case FACEEDGE_RIGHT:
		*oRes = float4( 0.0f, -0.5f * vk, 
						0.5f / targWidth + face * 2.0f / targWidth, 
						+ 0.5f * vk + 0.5f / targHeight );

		if( inverseCoord )
			oRes->y = -oRes->y;
		break;
	case FACEEDGE_LEFT:
		*oRes = float4( 0.0f, -0.5f * vk, 
						1.5f / targWidth + face * 2.0f / targWidth,
						+ 0.5f * vk  + 0.5f / targHeight );

		if( inverseCoord )
			oRes->y = -oRes->y;
		break;
	case FACEEDGE_TOP:
		*oRes = float4( 0.5f * hk, 0.0f, 
						0.5f * hk + 12.5f / targWidth,
						0.5f / targHeight + face * 2.0f / targHeight );

		if( inverseCoord )
			oRes->x = -oRes->x;
		break;
	case FACEEDGE_BOTTOM:
		*oRes = float4( 0.5f * hk, 0.0f,
						0.5f * hk + 12.5f / targWidth,
						1.5f / targHeight + face * 2.0f / targHeight );
		if( inverseCoord )
			oRes->x = -oRes->x;
		break;
	}

}

void TexcXFormOff( float4* oRes )
{
	oRes[ 0 ] = float4( + 0,	+ 0,	+ 3,	+ 3 );
}

void CornerToTexcXForm( float4* oRes, int dim, int targWidth, int targHeight, int face, FaceCorner corner )
{
	float4 result( 0.f, 0.f, 0.f, 0.f );

	float hk = float( dim ) / targWidth;
	float vk = float( dim ) / targHeight;

	switch( corner )
	{
	case FACECORNER_LEFTTOP:
		*oRes = float4( 0.0f, 0.0f, 
						1.5f / targWidth + face * 2.0f / targWidth, 
						0.5f / targHeight );
		break;
	case FACECORNER_RIGHTTOP:
		*oRes = float4( 0.0f, 0.0f, 
						0.5f / targWidth + face * 2.0f / targWidth,
						1.5f / targHeight );
		break;
	case FACECORNER_BOTTOMLEFT:
		*oRes = float4( 0.0f, 0.0f, 
						12.5f / targWidth,
						1.5f / targHeight + face * 2.0f / targHeight );
		break;
	case FACECORNER_BOTTOMRIGHT:
		*oRes = float4( 0.0f, 0.0f,
						( 12.5f + dim - 1 ) / targWidth,
						1.5f / targHeight + face * 2.0f / targHeight );
		break;
	}
}

int IsTopBottom( FaceEdge edge )
{
	return edge == FACEEDGE_TOP || edge == FACEEDGE_BOTTOM;
}

int IsLeftRight( FaceEdge edge )
{
	return edge == FACEEDGE_LEFT  || edge == FACEEDGE_RIGHT;
}


void StitchTwoCubeMapEdges(	r3dScreenBuffer* tempRT,
							int mip,
							int cubeWidth,
							D3DCUBEMAP_FACES face0,
							D3DCUBEMAP_FACES face1,
							FaceEdge face0_edge,
							FaceEdge face1_edge,
							int inverseCoord,
							float swap1,
							float fixOffset )
{
	r3dRenderer->SetTex( tempRT->Tex, 0 );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP ) );

	r3dSetFiltering( R3D_POINT, 0 );

	int DIM = cubeWidth;

	RECT rect;
	EdgeToViewportSourceRect( &rect, DIM, face0_edge );

	r3dRenderer->SetViewport(	(float)rect.left, (float)rect.top, 
								(float)( rect.right - rect.left ), 
								(float)( rect.bottom - rect.top ) );

	enum 
	{
		VS_CONST_COUNT = 5
	};

	r3dTL::TFixedArray< float4, VS_CONST_COUNT > vsConst;

	// float4 PosXfm : register( c0 );
	vsConst[ 0 ] = EdgeToPosXForm( DIM, face0_edge, fixOffset );

	// float4 TexcSwapArg : register( c1 );
	vsConst[ 1 ] = float4( 0.f, swap1, 0, 0 );

	// float4 TexcXForm[ 3 ] : register( c2 );
	EdgeToTexcXForm( &vsConst[ 2 ], cubeWidth, (int)tempRT->Width, (int)tempRT->Height, face0, face0_edge, 0 );
	EdgeToTexcXForm( &vsConst[ 3 ], cubeWidth, (int)tempRT->Width, (int)tempRT->Height, face1, face1_edge, inverseCoord );
	TexcXFormOff( &vsConst[ 4 ] );

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConst[ 0 ].x, VS_CONST_COUNT ) );

	enum 
	{
		PS_CONST_COUNT = 10
	};

	r3dTL::TFixedArray< float4, PS_CONST_COUNT > psConst;

	// float4 Lod : register( c9 );
	psConst[ 9 ] = float4( (float)mip, 0.f, 0.f, 0.f );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConst[ 0 ].x, PS_CONST_COUNT ) );

#ifndef FINAL_BUILD
	int debug = 0;
	if( debug && (	face0_edge == FACEEDGE_BOTTOM && face1_edge == FACEEDGE_BOTTOM
						||
					face1_edge == FACEEDGE_BOTTOM && face0_edge == FACEEDGE_BOTTOM
						) )
	{
		static int i = 0;

		r3dColor clr;

		switch( i )
		{
		case 0:
			clr = r3dColor::red;
			break;
		case 1:
			clr = r3dColor::green;
			break;
		case 2:
			clr = r3dColor::blue;
			break;
		case 3:
			clr = r3dColor::yellow;
			break;
		}

		i ++;

		if( i >= 4 ) i = 0;

		r3dSetFwdColorPixelShader( clr );
	}
#endif

	r3dDrawFullScreenQuad( false );

#ifndef FINAL_BUILD
	if( debug )
	{
		r3dRenderer->SetPixelShader( "PS_REMOVECUBEMAPEDGE_CUBESOURCE" );
	}
#endif
}

void StitchEdges( r3dScreenBuffer* cube, r3dScreenBuffer* tempRT0, int mip )
{
	IDirect3DSurface9* surf( NULL );

	IDirect3DSurface9* prevRT;
	r3dRenderer->GetRT( 0, &prevRT );

	int cubeWidth = 0;

	for( int i = 0; i < 6; i ++ )	
	{
		surf = cube->GetTexCUBESurface( i, mip );

		D3DSURFACE_DESC sdesc;

		D3D_V( surf->GetDesc( &sdesc ) );

		cubeWidth = sdesc.Width;

		RECT targRect;
		RECT srcRect;

		EdgeToViewportTargetRect( &targRect, sdesc.Width, FACEEDGE_LEFT, i );
		EdgeToViewportSourceRect( &srcRect, sdesc.Width, FACEEDGE_LEFT );
		D3D_V( r3dRenderer->pd3ddev->StretchRect( surf, &srcRect, tempRT0->GetTex2DSurface(), &targRect, D3DTEXF_NONE ) );

		EdgeToViewportTargetRect( &targRect, sdesc.Width, FACEEDGE_RIGHT, i );
		EdgeToViewportSourceRect( &srcRect, sdesc.Width, FACEEDGE_RIGHT );
		D3D_V( r3dRenderer->pd3ddev->StretchRect( surf, &srcRect, tempRT0->GetTex2DSurface(), &targRect, D3DTEXF_NONE ) );

		EdgeToViewportTargetRect( &targRect, sdesc.Width, FACEEDGE_TOP, i );
		EdgeToViewportSourceRect( &srcRect, sdesc.Width, FACEEDGE_TOP );
		D3D_V( r3dRenderer->pd3ddev->StretchRect( surf, &srcRect, tempRT0->GetTex2DSurface(), &targRect, D3DTEXF_NONE ) );

		EdgeToViewportTargetRect( &targRect, sdesc.Width, FACEEDGE_BOTTOM, i );
		EdgeToViewportSourceRect( &srcRect, sdesc.Width, FACEEDGE_BOTTOM );
		D3D_V( r3dRenderer->pd3ddev->StretchRect( surf, &srcRect, tempRT0->GetTex2DSurface(), &targRect, D3DTEXF_NONE ) );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_POSITIVE_X
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_POSITIVE_X, mip );

		r3dRenderer->SetRT( 0, surf );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_X,
														D3DCUBEMAP_FACE_POSITIVE_Z,
														FACEEDGE_LEFT,
														FACEEDGE_RIGHT, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_X,
														D3DCUBEMAP_FACE_NEGATIVE_Z,
														FACEEDGE_RIGHT,
														FACEEDGE_LEFT, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_X, 
														D3DCUBEMAP_FACE_POSITIVE_Y,
														FACEEDGE_TOP, 
														FACEEDGE_RIGHT, 0, 1.f, 1.5f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_X, 
														D3DCUBEMAP_FACE_NEGATIVE_Y,
														FACEEDGE_BOTTOM, 
														FACEEDGE_RIGHT, 1, 1.f, 0.0f );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_NEGATIVE_X
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_NEGATIVE_X, mip );

		r3dRenderer->SetRT( 0, surf );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_X, 
														D3DCUBEMAP_FACE_NEGATIVE_Z,
														FACEEDGE_LEFT, 
														FACEEDGE_RIGHT, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_X, 
														D3DCUBEMAP_FACE_POSITIVE_Z,
														FACEEDGE_RIGHT, 
														FACEEDGE_LEFT, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_X, 
														D3DCUBEMAP_FACE_POSITIVE_Y,
														FACEEDGE_TOP,
														FACEEDGE_LEFT, 1, 1.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_X, 
														D3DCUBEMAP_FACE_NEGATIVE_Y,
														FACEEDGE_BOTTOM,
														FACEEDGE_LEFT, 0, 1.f, 1.5f );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_POSITIVE_Y
	if( 1 )
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_POSITIVE_Y, mip );

		r3dRenderer->SetRT( 0, surf );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Y,
														D3DCUBEMAP_FACE_POSITIVE_X,
														FACEEDGE_RIGHT,
														FACEEDGE_TOP, 0, 1.f, 0.f );


		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Y,
														D3DCUBEMAP_FACE_NEGATIVE_X,												
														FACEEDGE_LEFT, 
														FACEEDGE_TOP, 1, 1.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Y,
														D3DCUBEMAP_FACE_POSITIVE_Z,
														FACEEDGE_BOTTOM, 
														FACEEDGE_TOP, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Y,
														D3DCUBEMAP_FACE_NEGATIVE_Z,
														FACEEDGE_TOP, 
														FACEEDGE_TOP, 1, 0.f, 0.f );

	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_NEGATIVE_Y
	if( 1 )
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_NEGATIVE_Y, mip );

		r3dRenderer->SetRT( 0, surf );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Y,
														D3DCUBEMAP_FACE_POSITIVE_X, 
														FACEEDGE_RIGHT,
														FACEEDGE_BOTTOM, 1, 1.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Y,
														D3DCUBEMAP_FACE_NEGATIVE_X, 
														FACEEDGE_LEFT,
														FACEEDGE_BOTTOM, 0, 1.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Y, 
														D3DCUBEMAP_FACE_POSITIVE_Z,
														FACEEDGE_TOP, 
														FACEEDGE_BOTTOM, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Y,
														D3DCUBEMAP_FACE_NEGATIVE_Z, 
														FACEEDGE_BOTTOM,
														FACEEDGE_BOTTOM, 1, 0.f, 0.0f );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_POSITIVE_Z
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_POSITIVE_Z, mip );

		r3dRenderer->SetRT( 0, surf );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Z,
														D3DCUBEMAP_FACE_POSITIVE_X,
														FACEEDGE_RIGHT,
														FACEEDGE_LEFT, 0, 0.f, 0.f );


		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Z,
														D3DCUBEMAP_FACE_NEGATIVE_X, 
														FACEEDGE_LEFT,
														FACEEDGE_RIGHT, 0, 0.f, 0.f );


		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Z,
														D3DCUBEMAP_FACE_POSITIVE_Y,
														FACEEDGE_TOP,
														FACEEDGE_BOTTOM, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_POSITIVE_Z,
														D3DCUBEMAP_FACE_NEGATIVE_Y,
														FACEEDGE_BOTTOM,
														FACEEDGE_TOP, 0, 0.f, 0.f );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_NEGATIVE_Z
	if( 1 )
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_NEGATIVE_Z, mip );

		r3dRenderer->SetRT( 0, surf );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Z,
														D3DCUBEMAP_FACE_NEGATIVE_X, 
														FACEEDGE_RIGHT,
														FACEEDGE_LEFT, 0, 0.f, 0.f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Z, 
														D3DCUBEMAP_FACE_NEGATIVE_Y,
														FACEEDGE_BOTTOM,
														FACEEDGE_BOTTOM, 1, 0.f, 1.5f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Z,
														D3DCUBEMAP_FACE_POSITIVE_Y,
														FACEEDGE_TOP, 
														FACEEDGE_TOP, 1, 0.f, 1.5f );

		StitchTwoCubeMapEdges( tempRT0, mip, cubeWidth,	D3DCUBEMAP_FACE_NEGATIVE_Z,
														D3DCUBEMAP_FACE_POSITIVE_X,
														FACEEDGE_LEFT,
														FACEEDGE_RIGHT, 0, 0.f, 0.f );
	}

	D3DPERF_BeginEvent( 0, L"Stitch Corners" );

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_POSITIVE_X
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_POSITIVE_X, mip );

		r3dRenderer->SetRT( 0, surf );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_LEFTTOP, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_RIGHTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_BOTTOMRIGHT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_RIGHTTOP, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_LEFTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_RIGHTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_BOTTOMRIGHT, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_BOTTOMLEFT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_BOTTOMRIGHT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_BOTTOMLEFT, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_BOTTOMRIGHT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_RIGHTTOP );
	}


	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_NEGATIVE_X
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_NEGATIVE_X, mip );

		r3dRenderer->SetRT( 0, surf );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_LEFTTOP, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_RIGHTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_LEFTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_RIGHTTOP, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_LEFTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_BOTTOMLEFT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_BOTTOMRIGHT, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_BOTTOMLEFT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_LEFTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_BOTTOMLEFT, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_BOTTOMRIGHT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_BOTTOMLEFT );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_POSITIVE_Y
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_POSITIVE_Y, mip );

		r3dRenderer->SetRT( 0, surf );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_LEFTTOP, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_RIGHTTOP,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_LEFTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_RIGHTTOP, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_LEFTTOP,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_RIGHTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_BOTTOMRIGHT, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_RIGHTTOP,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_LEFTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_BOTTOMLEFT, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_LEFTTOP,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_RIGHTTOP );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_NEGATIVE_Y
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_NEGATIVE_Y, mip );

		r3dRenderer->SetRT( 0, surf );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_LEFTTOP, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_BOTTOMLEFT,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_BOTTOMRIGHT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_RIGHTTOP, 
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_BOTTOMRIGHT,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_BOTTOMLEFT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_BOTTOMRIGHT, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_BOTTOMLEFT,
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_BOTTOMRIGHT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_BOTTOMLEFT, 
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_BOTTOMRIGHT,
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_BOTTOMLEFT );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_POSITIVE_Z
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_POSITIVE_Z, mip );

		r3dRenderer->SetRT( 0, surf );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_LEFTTOP, 
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_RIGHTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_BOTTOMLEFT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_RIGHTTOP, 
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_LEFTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_BOTTOMRIGHT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_BOTTOMRIGHT, 
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_BOTTOMLEFT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_RIGHTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_POSITIVE_Z, FACECORNER_BOTTOMLEFT, 
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_BOTTOMRIGHT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_LEFTTOP );
	}

	//------------------------------------------------------------------------
	// D3DCUBEMAP_FACE_NEGATIVE_Z
	{
		surf = cube->GetTexCUBESurface( D3DCUBEMAP_FACE_NEGATIVE_Z, mip );

		r3dRenderer->SetRT( 0, surf );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_LEFTTOP, 
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_RIGHTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_RIGHTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_RIGHTTOP, 
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_LEFTTOP,
								D3DCUBEMAP_FACE_POSITIVE_Y, FACECORNER_LEFTTOP );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_BOTTOMRIGHT, 
								D3DCUBEMAP_FACE_NEGATIVE_X, FACECORNER_BOTTOMLEFT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_BOTTOMLEFT );

		Stitch3CubeMapCorners(	tempRT0, cubeWidth,
								D3DCUBEMAP_FACE_NEGATIVE_Z, FACECORNER_BOTTOMLEFT, 
								D3DCUBEMAP_FACE_POSITIVE_X, FACECORNER_BOTTOMRIGHT,
								D3DCUBEMAP_FACE_NEGATIVE_Y, FACECORNER_BOTTOMRIGHT );
	}

	D3DPERF_EndEvent();

	prevRT->Release();
	r3dRenderer->SetRT( 0, prevRT );
}

void StitchLastMip( r3dScreenBuffer* cube, r3dScreenBuffer* tempRT, int mip, D3DCUBEMAP_FACES face0 )
{
	IDirect3DSurface9* surf( NULL );
	surf = cube->GetTexCUBESurface( face0, mip );

	D3DSURFACE_DESC sdesc;
	D3D_V( surf->GetDesc( &sdesc ) );

	RECT srcRect;
	srcRect.left = 0;
	srcRect.top = 0;
	srcRect.right = sdesc.Width;
	srcRect.bottom = sdesc.Height;

	D3D_V( r3dRenderer->pd3ddev->StretchRect( surf, &srcRect, tempRT->GetTex2DSurface(), &srcRect, D3DTEXF_NONE ) );

	//------------------------------------------------------------------------

	r3dRenderer->SetTex( cube->Tex, 0 );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP ) );

	tempRT->Activate();

	int DIM = int( cube->Width ) >> mip;

	r3dRenderer->SetViewport( 0.f, 0.f, (float)DIM, (float)DIM );

	enum 
	{
		VS_CONST_COUNT = 2
	};

	r3dTL::TFixedArray< float4, VS_CONST_COUNT > vsConst;

	// float4 PosXfm : register( c0 );
	vsConst[ 0 ] = float4( 1, 1, 0, 0 );

	// float4 TexcSwapArg : register( c1 );
	vsConst[ 1 ] = float4( 0, 0, 0, 0 );

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConst[ 0 ].x, VS_CONST_COUNT ) );

	enum 
	{
		PS_CONST_COUNT = 10
	};

	r3dTL::TFixedArray< float4, PS_CONST_COUNT > psConst;

	// float4x3 TexXfm[ 3 ] : register( c0 );
	TexcXFormOff( &psConst[ 0 ] );
	TexcXFormOff( &psConst[ 3 ] );
	TexcXFormOff( &psConst[ 6 ] );

	// float4 Lod : register( c9 );
	psConst[ 9 ] = float4( (float)mip, 0.f, 0.f, 0.f );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConst[ 0 ].x, PS_CONST_COUNT ) );

	r3dDrawFullScreenQuad( false );

	tempRT->Deactivate();

	//------------------------------------------------------------------------

	D3D_V( r3dRenderer->pd3ddev->StretchRect( tempRT->GetTex2DSurface(), &srcRect, surf, &srcRect, D3DTEXF_NONE ) );
}

void Stitch3CubeMapCorners( r3dScreenBuffer* tempRT, int dim, D3DCUBEMAP_FACES face0, FaceCorner faceCorner0, D3DCUBEMAP_FACES face1, FaceCorner faceCorner1, D3DCUBEMAP_FACES face2, FaceCorner faceCorner2 )
{
	r3dRenderer->SetTex( tempRT->Tex, 0 );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP ) );

	RECT viewportRect;
	CornerToViewportSourceRect( &viewportRect, dim, faceCorner0 );

	r3dRenderer->SetViewport(	float( viewportRect.left ), 
								float( viewportRect.top ), 
								float( viewportRect.right - viewportRect.left ), 
								float( viewportRect.bottom - viewportRect.top ) );

	enum 
	{
		VS_CONST_COUNT = 5
	};

	r3dTL::TFixedArray< float4, VS_CONST_COUNT > vsConst;

	// float4 PosXfm : register( c0 );
	vsConst[ 0 ] = CornerToPosXForm( dim, faceCorner0 );

	// float4 TexcSwapArg : register( c1 );
	vsConst[ 1 ] = float4( 0, 0, 0, 0 );

	int iw = (int)tempRT->Width;
	int ih = (int)tempRT->Height;

	// float4 TexcXForm[ 3 ] : register( c2 );
	CornerToTexcXForm( &vsConst[ 2 ], dim, iw, ih, face0, faceCorner0 );
	CornerToTexcXForm( &vsConst[ 3 ], dim, iw, ih, face1, faceCorner1 );
	CornerToTexcXForm( &vsConst[ 4 ], dim, iw, ih, face2, faceCorner2 );

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConst[ 0 ].x, VS_CONST_COUNT ) );

	enum 
	{
		PS_CONST_COUNT = 1
	};

	r3dTL::TFixedArray< float4, PS_CONST_COUNT > psConst;

	// float4 Lod : register( c0 );
	psConst[ 0 ] = float4( (float)0, 0.f, 0.f, 0.f );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConst[ 0 ].x, PS_CONST_COUNT ) );

#if 0
	int curPSId = r3dRenderer->CurrentPixelShaderID;
	if( face0 == D3DCUBEMAP_FACE_POSITIVE_X && faceCorner0 == FACECORNER_BOTTOMRIGHT )
	{
		r3dSetFwdColorPixelShader( r3dColor::red );
	}
#endif

	r3dDrawFullScreenQuad( false );

#if 0
	r3dRenderer->SetPixelShader( curPSId );
#endif
}

//------------------------------------------------------------------------

void StitchEnvmapMipEdges( r3dScreenBuffer* cube, r3dScreenBuffer* tempRT0 )
{
	D3DPERF_BeginEvent( 0, L"StitchEnvmapMipEdges" );

	struct SetRestoreStates
	{
		SetRestoreStates()
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

			r3dRenderer->SetCullMode( D3DCULL_NONE );
			r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );
		}

		~SetRestoreStates()
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) );
			r3dRenderer->RestoreCullMode();
		}

	} setRestoreStates; (void)setRestoreStates;

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	int PSID = r3dRenderer->GetPixelShaderIdx( "PS_REMOVECUBEMAPEDGE_CUBESOURCE" );
	int VSID = r3dRenderer->GetVertexShaderIdx( "VS_REMOVECUBEMAPEDGE_CUBESOURCE" );

	int PSID_ONEPIXEL = r3dRenderer->GetPixelShaderIdx( "PS_REMOVECUBEMAPEDGE_CUBESOURCE_ONE_PIXEL_MIP" );

	r3dRenderer->SetPixelShader( PSID );
	r3dRenderer->SetVertexShader( VSID );

	for( int i = 0, e = cube->GetNumMipLevels() - 1; i < e; i ++ )
	{
		StitchEdges( cube, tempRT0, i );
	}

	r3dRenderer->SetPixelShader( PSID_ONEPIXEL );

	StitchLastMip( cube, tempRT0, cube->GetNumMipLevels() - 1, D3DCUBEMAP_FACE_NEGATIVE_X );
	StitchLastMip( cube, tempRT0, cube->GetNumMipLevels() - 1, D3DCUBEMAP_FACE_POSITIVE_X );

	StitchLastMip( cube, tempRT0, cube->GetNumMipLevels() - 1, D3DCUBEMAP_FACE_NEGATIVE_Y );
	StitchLastMip( cube, tempRT0, cube->GetNumMipLevels() - 1, D3DCUBEMAP_FACE_POSITIVE_Y );

	StitchLastMip( cube, tempRT0, cube->GetNumMipLevels() - 1, D3DCUBEMAP_FACE_NEGATIVE_Z );
	StitchLastMip( cube, tempRT0, cube->GetNumMipLevels() - 1, D3DCUBEMAP_FACE_POSITIVE_Z );

	D3DPERF_EndEvent();
}

void GenerateEnvmap( r3dTexture* tex, const r3dString& texName, const r3dPoint3D& pos )
{
	GenerateEnvmapT<EnvmapConvert16U>( tex, texName, pos, 1 );
}

void GenerateEnvmap64F( r3dTexture* tex, const r3dString& texName, const r3dPoint3D& pos )
{
	GenerateEnvmapT<EnvmapConvert64F>( tex, texName, pos, 0 );
}

struct GenEnvmapClearRestoreRenderSettings
{
	GenEnvmapClearRestoreRenderSettings()
	{
		prevEditorInitted			= g_bEditMode;
		prevColorCorrection			= gHUDFilterSettings[HUDFilter_Default].enableColorCorrection;
		prevRadialBlur				= g_RenderRadialBlur;
#ifndef FINAL_BUILD
		prevHideIcons				= r_hide_icons->GetInt();
#endif
		prevOcclusionQueryEnabled	= r_use_oq->GetInt();
		prevZPrepass				= r_z_allow_prepass->GetInt();
		prevDeviceClears			= r_device_clear->GetInt();

		prevDrawDistance			= r_default_draw_distance->GetFloat();

#ifndef FINAL_BUILD
		prevGrassDraw				= r_grass_draw->GetInt();
#endif
		prevDecals					= r_decals->GetInt();

		prevShadows					= r_shadows->GetInt();

		prevSSR						= r_use_ssr->GetInt();

		prevSSAO					= r_ssao->GetInt();

		prevFirstPerson				= r_first_person->GetInt();

		prevAniso					= r_anisotropy->GetInt();

		prevTranspShadows			= r_transp_shadows->GetInt();
		prevTerraShadows			= r_terra_shadows->GetInt();
		prevShadowBlur				= r_shadow_blur->GetInt();

		prevEnvQuality				= r_environment_quality->GetInt();

		prevLFSMCacheDist = r_lfsm_cache_dist->GetFloat();
		prevFXAA_Shadows = r_fxaa_shadow_blur->GetInt();

		prevInstancedRender = r_use_instancing->GetInt();

		r_fxaa_shadow_blur->SetInt( 0 );

		r_min_shadow_slice->SetInt( r_active_shadow_slices->GetInt() - 1 );

		r_transp_shadows->SetInt( 0 );
		r_terra_shadows->SetInt( 0 );
		r_shadow_blur->SetInt( 0 );

		r_environment_quality->SetInt( 1 );

		r_anisotropy->SetInt( 1 );

		r_first_person->SetInt( 0 );

#if 0
		r_shadows->SetInt( 0 );
#endif

		r_decals->SetInt( 0 );
#ifndef FINAL_BUILD
		r_grass_draw->SetInt( 0 );
#endif
		r_default_draw_distance->SetFloat( 256.f );

		g_bEditMode					= false;
		g_RenderRadialBlur			= false;
		gHUDFilterSettings[HUDFilter_Default].enableColorCorrection = 0;
#ifndef FINAL_BUILD
		r_hide_icons->SetInt(1);
#endif
		r_use_oq->SetInt(0);
		r_z_allow_prepass->SetInt(0);
		r_device_clear->SetInt(1);

		r_use_ssr->SetInt( 0 );

		r_allow_size_cull->SetInt( 1 );
		r_cull_size->SetInt( 4 );

		r_use_instancing->SetInt( 0 );

		r_enable_matrix_cache->SetInt( 0 );

		UpdateZPrepassSettings();

		g_EnvmapProbes.SetEnvmapUpdateMode( true );
	}

	~GenEnvmapClearRestoreRenderSettings()
	{
		g_bEditMode				= prevEditorInitted;
		g_RenderRadialBlur		= prevRadialBlur;
		gHUDFilterSettings[HUDFilter_Default].enableColorCorrection	= prevColorCorrection;

		r_fxaa_shadow_blur->SetInt( prevFXAA_Shadows );

		r_anisotropy->SetInt( prevAniso );

		r_min_shadow_slice->SetInt( 0 );

#ifndef FINAL_BUILD
		r_hide_icons->SetInt(prevHideIcons);
#endif
		r_use_oq->SetInt(prevOcclusionQueryEnabled);
		r_z_allow_prepass->SetInt(prevZPrepass);
		r_device_clear->SetInt( prevDeviceClears );

		r_allow_size_cull->SetInt( 0 );

#ifndef FINAL_BUILD
		r_grass_draw->SetInt( prevGrassDraw );
#endif

		r_decals->SetInt( prevDecals );

		r_shadows->SetInt( prevShadows );

		r_use_ssr->SetInt( prevSSR );

		r_ssao->SetInt( prevSSAO );

		r_first_person->SetInt( prevFirstPerson );

		r_transp_shadows->SetInt( prevTranspShadows );
		r_terra_shadows->SetInt( prevTerraShadows );
		r_shadow_blur->SetInt( prevShadowBlur );

		r_environment_quality->SetInt( prevEnvQuality );

		r_lfsm_cache_dist->SetFloat( prevLFSMCacheDist );

		UpdateZPrepassSettings();

		g_EnvmapProbes.SetEnvmapUpdateMode( false );

		r_default_draw_distance->SetFloat( prevDrawDistance );

		r_use_instancing->SetInt( prevInstancedRender );

		r_enable_matrix_cache->SetInt( 1 );
	}

	bool prevEditorInitted;

	int prevColorCorrection;
	int prevRadialBlur;
	int prevOcclusionQueryEnabled;
	int prevHideIcons;
	int prevZPrepass;
	int prevDeviceClears;
	int prevGrassDraw;
	int prevDecals;
	int prevShadows;

	int prevSSAO;
	int prevSSR;

	int prevFirstPerson;

	int prevAniso;

	int prevTranspShadows;
	int prevTerraShadows;
	int prevShadowBlur;

	int prevEnvQuality;

	int prevFXAA_Shadows;

	int prevInstancedRender;

	float prevDrawDistance;
	float prevLFSMCacheDist;

};

void GenerateEnvmapDynamic_Prepare()
{
	D3DPERF_BeginEvent( 0, L"GenerateEnvmapDynamic_Prepare" );
	GenEnvmapClearRestoreRenderSettings clearRestoreRenderSettings; (void)clearRestoreRenderSettings;
	CurRenderPipeline->RenderStage_Prepare();
	D3DPERF_EndEvent();
}

void GenerateEnvmapDynamic_Terrain()
{
	D3DPERF_BeginEvent( 0, L"GenerateEnvmapDynamic_Terrain" );
	GenEnvmapClearRestoreRenderSettings clearRestoreRenderSettings; (void)clearRestoreRenderSettings;
	CurRenderPipeline->RenderStage_Terrain();
	D3DPERF_EndEvent();
}

void GenerateEnvmapDynamic_Objects( int* ioObjectPos )
{
	D3DPERF_BeginEvent( 0, L"GenerateEnvmapDynamic_Objects" );

	GenEnvmapClearRestoreRenderSettings clearRestoreRenderSettings; (void)clearRestoreRenderSettings;

	int count = CurRenderPipeline->GetStagedRenderObjectCount();
	int toDraw = R3D_MIN( count - *ioObjectPos, 256 );

	CurRenderPipeline->RenderStage_Deferred( *ioObjectPos, toDraw );

	*ioObjectPos += toDraw;

	D3DPERF_EndEvent();
}

void GenerateEnvmapDynamic_Finalize( int dim )
{
	D3DPERF_BeginEvent( 0, L"GenerateEnvmapDynamic_Finalize" );

	D3DVIEWPORT9 viewport;

	viewport.MinZ = 0.f;
	viewport.MaxZ = 1.0f;

	viewport.X		= 0;
	viewport.Y		= 0;
	viewport.Width	= dim;
	viewport.Height = dim;

	CurRenderPipeline->RenderStage_Finilize( &viewport );

	D3DPERF_EndEvent();
}

int GenerateEnvmapDynamic( r3dScreenBuffer* cube, const r3dPoint3D& pos, GenEnvmapSequence step, GenEnvmapSubSequence subStep, int* ioObjectPosition )
{
	R3D_ENSURE_MAIN_THREAD();

	r3d_assert( cube->bCubemap );

	int ready = 0;

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"DYNAMIC CUBE MAP RENDER");

	r3dPoint3D dirs[] = {	r3dPoint3D( +1, +0, +0 ), 
							r3dPoint3D( -1, +0, +0 ),
							r3dPoint3D( +0, +1, +0 ),
							r3dPoint3D( +0, -1, +0 ),
							r3dPoint3D( +0, +0, +1 ),
							r3dPoint3D( +0, +0, -1 ) };

	r3dPoint3D ups[] = {	r3dPoint3D( +0, +1, +0 ), 
							r3dPoint3D( +0, +1, +0 ),
							r3dPoint3D( +0, +0, -1 ),
							r3dPoint3D( +0, +0, +1 ),
							r3dPoint3D( +0, +1, +0 ),
							r3dPoint3D( +0, +1, +0 ) };

	struct SetRestoreCamera
	{
		SetRestoreCamera()
		{
			oldCam = gCam;
		}

		~SetRestoreCamera()
		{
			gCam = oldCam;
		}

		r3dCamera oldCam;
	} setRestoreCamera; (void)setRestoreCamera;

	gCam			= pos;
	gCam.FOV		= 90;
	gCam.Aspect		= 1.0f;

	for( int i = 0, e = sizeof dirs / sizeof dirs[ 0 ]; i < e; i ++ )
	{
		if( i != step )
			continue;

		gCam.vPointTo	= dirs[ i ];
		gCam.vUP		= ups[ i ];

		r3dRenderer->SetCamera( gCam, false );

		switch( subStep )
		{
		case GENENVMAPSUB_PREPARE:
			GenerateEnvmapDynamic_Prepare();
			break;
		case GENENVMAPSUB_TERRAIN:
			GenerateEnvmapDynamic_Terrain();
			break;
		case GENENVMAPSUB_OBJECTS:
			GenerateEnvmapDynamic_Objects( ioObjectPosition );
			break;
		case GENENVMAPSUB_FINALIZE:
			{
				D3DSURFACE_DESC sdesc;
				cube->GetTexCUBESurface( 0, 0 )->GetDesc( &sdesc );
				GenerateEnvmapDynamic_Finalize( sdesc.Width );

				IDirect3DSurface9* tempSurf( NULL );

				IDirect3DSurface9* surf = cube->GetTexCUBESurface( i, 0 );

				extern r3dScreenBuffer* gBuffer_StagedRender_Temporary;

				D3D_V( gBuffer_StagedRender_Temporary->Tex->AsTex2D()->GetSurfaceLevel( 0, &tempSurf ) );

				RECT rect;

				rect.left = 0;
				rect.top = 0;
				rect.right = sdesc.Width;
				rect.bottom = sdesc.Height;

				D3D_V( r3dRenderer->pd3ddev->StretchRect( tempSurf, NULL, surf, NULL, D3DTEXF_NONE ) );

				SAFE_RELEASE( tempSurf );

				IDirect3DCubeTexture9* convertedTex( NULL );

				int mips = 0;

				int w = int( cube->Width );

				for(; w; w /= 2 )
				{
					mips ++;	
				}

				for( int i = 1, e = mips; i < e; i ++ )
				{
					for( int f = 0; f < 6; f ++ )
					{
						if( f != step )
							continue;

						IDirect3DSurface9 *dst;
						IDirect3DSurface9 *src; 

						src = cube->GetTexCUBESurface( f, i - 1 );
						dst = cube->GetTexCUBESurface( f, i );

						D3D_V( r3dRenderer->pd3ddev->StretchRect( src , NULL, dst, NULL, D3DTEXF_LINEAR ) );
					}
				}
			}
			break;
		}
	}


#if 1
	if( step == GENENVMAP_STITCH )
	{
		StitchEnvmapMipEdges( cube, g_pPostFXChief->GetBuffer( PostFXChief::RTT_PINGPONG_LAST ) );
		ready = 1;
	}
#if 0
	else
	{
		StitchEnvmapMipEdges( cube, g_pPostFXChief->GetBuffer( PostFXChief::RTT_PINGPONG_LAST ) );
	}
#endif
#endif

#ifndef FINAL_BUILD
	if( r_1debug_helper->GetInt() && step == GENENVMAP_STITCH && 0 )
	{
		D3DXSaveTextureToFile( "testenv.dds", D3DXIFF_DDS, cube->AsTexCUBE(), NULL );

		r_1debug_helper->SetInt( 0 );
	}
#endif
	D3DPERF_EndEvent();

	return ready;
}