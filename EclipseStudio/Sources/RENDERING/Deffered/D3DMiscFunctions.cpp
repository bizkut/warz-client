#include "r3dPCH.h"

#include <WindowsX.h>

#include "r3d.h"
#include "r3dLight.h"
#include "d3dfont.h"
#include "r3dConePrim.h"

#include "../GameEngine/TrueNature2/Terrain3.h"
#include "../GameEngine/TrueNature2/Terrain2.h"

#include "GameCommon.h"
#include "GameLevel.h"

#include "CommonPostFX.h"
#include "RenderDeferredPointLightsOptimized.h"

#include "rendering/Deffered/CommonPostFX.h"
#include "rendering/Deffered/PostFXChief.h"

#include "../../ObjectsCode/weapons/ExplosionVisualController.h"
#include "../../ObjectsCode/weapons/FlashbangVisualController.h"
#include "../../ObjectsCode/ai/AI_Player.H"
#include "../../ObjectsCode/Gameplay/obj_Zombie.h"

#include "../SF/Console/Config.h"

#include "D3DMiscFunctions.h"

void UpdateD3DAntiCheat_WallHack();
bool UpdateD3DAntiCheat_ScreenHelpers2( bool lastAttempt );
void InvalidateScreenHelperAnticheat();

extern r3dD3DQuery*		AntiCheatQuery;

static IDirect3DTexture9* g_TempSaveDataS;

static r3dTL::TArray< DWORD > g_AntiCheat_PrePresentPixels;
static int g_AntiCheat_PrePresentPixels_Width;
static int g_AntiCheat_PrePresentPixels_Height;
static int g_AntiCheat_ScreenHelper2Counter = 0;

static bool D3DCheaters[ ANTICHEAT_COUNT ] = { false };
//static int CheatCount = 0;
//static int NoCheatCount = 0;
//static bool AntiCheatQueryIssued=false;
static bool EnableD3DAntiCheatCodepath[ ANTICHEAT_COUNT ] = { false };

void issueD3DAntiCheatCodepath( AntiCheatType anticheatType )
{
	VMPROTECT_BeginMutation("issueD3DAntiCheatCodepath");
	if(!( anticheatType >= 0 && anticheatType < ANTICHEAT_COUNT ))
		return;

	if( D3DCheaters[ anticheatType ] )
		return;

	if( anticheatType == ANTICHEAT_WALLHACK && !r3dRenderer->SupportsOQ )
		return;

	if( anticheatType == ANTICHEAT_SCREEN_HELPERS2 )
	{
		g_AntiCheat_ScreenHelper2Counter = 0;
		g_AntiCheat_PrePresentPixels.Clear();
	}

	EnableD3DAntiCheatCodepath[ anticheatType ] = true;

	VMPROTECT_End();
}

void UpdateD3DAntiCheat()
{
	VMPROTECT_BeginMutation("UpdateD3DAntiCheat");
	if( !r3dRenderer->DeviceAvailable )
	{
		InvalidateScreenHelperAnticheat();
		return;
	}

	// do it once per request
	if( EnableD3DAntiCheatCodepath[ ANTICHEAT_WALLHACK ] )
	{
		UpdateD3DAntiCheat_WallHack();
		return;
	}
	VMPROTECT_End();
}

void UpdateD3DAntiCheat_WallHack()
{
	VMPROTECT_BeginMutation("UpdateD3DAntiCheat_WallHack");
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

	// issue character drawing query
	{
#ifndef FINAL_BUILD
		D3DPERF_BeginEvent( 0, L"AntiCheat" );
#endif

		GameObject* testdummy = NULL ;

		ObjectManager& gw = GameWorld() ;
		for( GameObject* obj = gw.GetFirstObject(); obj; obj = gw.GetNextObject(obj) )
		{
			if( obj->isObjType( OBJTYPE_Human ) )
			{
				obj_Player* a_pl = static_cast<obj_Player*> ( obj ) ;

				if( !a_pl->NetworkLocal )
				{
					testdummy = obj ;
					break;
				}
			}

			if( obj->isObjType( OBJTYPE_Zombie ) )
			{
				obj_Zombie* a_zmb = static_cast<obj_Zombie*> ( obj ) ;

				if( a_zmb->ZombieState == EZombieStates::ZState_Sleep ||
					a_zmb->ZombieState == EZombieStates::ZState_Dead )
					continue;

				testdummy = obj;
				break;
			}
		}

		if( !testdummy )
		{
			return ;
		}

		r3dCamera cam ;

		r3dPoint3D offset = r3dPoint3D( 0, 0.5f * testdummy->GetBBoxLocal().Size.y, 2 * testdummy->GetObjectsRadius() );

		cam.SetPosition( testdummy->GetPosition() - offset ) ;
		cam.PointTo( testdummy->GetPosition() ) ;

		cam.FOV = 45 ;

		struct SetRestoreCam
		{
			SetRestoreCam( r3dCamera& cam )
			{
				r3dRenderer->SetCamera( cam, false ) ;
			}

			~SetRestoreCam()
			{
				r3dRenderer->SetCamera( gCam, false ) ;
			}
		} setRestoreCam( cam ) ;

		RenderArray ra [ rsCount ] ;
		testdummy->AppendRenderables( ra, cam ) ;

		// if player is not visible, array can be 0
		if(ra[ rsFillGBuffer ].Count() == 0)
			return ;

		float CLEAR_VAL	= r_wall_hack_anti_cheat_z - 0.00025f ;

		if( r3dRenderer->ZDir == r3dRenderLayer::ZDIR_INVERSED )
			CLEAR_VAL = 1.0f - CLEAR_VAL;

		D3DRECT clearRect ;

		// hide it behind minimap
		float vx = 0.850f * r3dRenderer->ScreenW;
		float vy = 0.075f * r3dRenderer->ScreenH;
		float vw = 0.950f * r3dRenderer->ScreenW - vx;
		float vh = 0.200f * r3dRenderer->ScreenH - vy;

		clearRect.x1 = int(vx - 1.0f);
		clearRect.y1 = int(vy - 1.0f);
		clearRect.x2 = int(vx + vw + 1.0f);
		clearRect.y2 = int(vy + vh + 1.0f);

#if 0
		r3dDrawLine2D( (float)clearRect.x1, (float)clearRect.y1, (float)clearRect.x2, (float)clearRect.y1, 1, r3dColor::red ) ;
		r3dDrawLine2D( (float)clearRect.x2, (float)clearRect.y1, (float)clearRect.x2, (float)clearRect.y2, 1, r3dColor::red ) ;
#endif

		struct SetResetViewport
		{
			SetResetViewport( float x, float y, float w, float h )
			{
				r3dRenderer->GetViewport( &savedViewPort ) ;

				r3dRenderer->SetZRange( R3D_ZRANGE_ANTICHEAT ) ;

				r3dRenderer->SetViewport( x, y, w, h ) ;
			}

			~SetResetViewport()
			{
				r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;

				r3dRenderer->SetViewport( (float)savedViewPort.X, (float)savedViewPort.Y, (float)savedViewPort.Width, (float)savedViewPort.Height ) ;
			}

			D3DVIEWPORT9 savedViewPort ;

		} gSetResetViewport( vx, vy, vw, vh ) ;

		D3D_V( r3dRenderer->pd3ddev->Clear( 1, &clearRect, D3DCLEAR_ZBUFFER, 0, CLEAR_VAL, 0 ) ) ;

		( AntiCheatQuery->Issue( D3DISSUE_BEGIN ) );

#if 0
	//	if( r_debug_helper->GetInt() )
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ZENABLE, FALSE ) ) ;
		}
#endif

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE1, 0 ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE2, 0 ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE3, 0 ) );

		PrecalculatedMeshShaderConsts replacementVSConsts;
		PrecalculatedMeshShaderConsts savedVSConsts;

		r3dPoint3D unpackScale( 1, 1, 1 );
		r3dPoint2D texUnpackScale( 1, 1 );

		if( r3dMesh* mesh = testdummy->GetObjectMesh() )
		{
			unpackScale = mesh->unpackScale;
			texUnpackScale = texUnpackScale;
		}

		r3dPrepareMeshShaderConsts( replacementVSConsts, testdummy->GetTransformMatrix(), &unpackScale, &texUnpackScale, r3dRenderer->ViewMatrix, r3dRenderer->ViewProjMatrix, NULL);

		savedVSConsts = testdummy->preparedVSConsts;
		testdummy->preparedVSConsts = replacementVSConsts;

		for( int i = 0, e = ra[ rsFillGBuffer ].Count() ; i < e; i ++ )
		{
			ra[ rsFillGBuffer ][ i ].DrawFunc( &ra[ rsFillGBuffer ][ i ], cam );
		}

		testdummy->preparedVSConsts = savedVSConsts;

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, TRUE ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE1, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE2, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE3, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );

		D3D_V( r3dRenderer->pd3ddev->Clear( 1, &clearRect, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) ) ;

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

#if 0
	//	if( r_debug_helper->GetInt() )
		{
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ZENABLE, TRUE ) ) ;
		}
#endif

		( AntiCheatQuery->Issue( D3DISSUE_END ) );

#ifndef FINAL_BUILD
		D3DPERF_EndEvent();
#endif
	}

	bool deviceWasLost = false;

	// immediately wait for query result, so we'll create screen shot of exactly this frame
	while(true)
	{
		int result ;
		HRESULT hr = AntiCheatQuery->GetData( &result, sizeof result, D3DGETDATA_FLUSH );
		if(hr == S_FALSE)
			continue;

		if( hr == D3DERR_DEVICELOST )
		{
			r3dRenderer->SetDeviceLost();
			deviceWasLost = true;
			break;
		}

		if(hr == S_OK)
		{
			if( result && result < 10000 )
			{
				D3DCheaters[ ANTICHEAT_WALLHACK ] = true;

				// force renderer to make copy of current screenshot
				extern int _r3d_sscopy_width;
				extern int _r3d_sscopy_height;
				extern char FoundPlayerCheat[512];
				_r3d_sscopy_width  = 800;
				_r3d_sscopy_height = 600;
				strcpy(FoundPlayerCheat,"AnticheatSystemAuto");
			}
			break;
		}

		// any other code, including D3DERR_DEVICELOST
		break;
	}

	if( !deviceWasLost )
	{
		EnableD3DAntiCheatCodepath[ ANTICHEAT_WALLHACK ] = false;
	}

	VMPROTECT_End();
}

//------------------------------------------------------------------------
void UpdateD3DAntiCheatPrePresent()
{
	if( !r3dRenderer->DeviceAvailable )
	{
		InvalidateScreenHelperAnticheat();
		return;
	}

	if( EnableD3DAntiCheatCodepath[ ANTICHEAT_SCREEN_HELPERS2 ] )
	{
		if( !g_AntiCheat_PrePresentPixels.Count() )
		{
			IDirect3DDevice9 *d = r3dRenderer->pd3ddev;

			struct Locals
			{
				Locals()
				{
					BBuf = 0;
				}

				~Locals()
				{
					if( BBuf ) BBuf->Release();
				}

				IDirect3DSurface9 *BBuf;

			} locals; (void)locals;

			r3dRenderer->GetRT(0, &locals.BBuf);

			IDirect3DSurface9 *sysmemBB = r3dRenderer->GetTempSurfaceForScreenShots();

			if (!SUCCEEDED(d->GetRenderTargetData(locals.BBuf, sysmemBB)))
			{
				InvalidateScreenHelperAnticheat();
				return;
			}

#if 0
			D3DXSaveSurfaceToFile( "test0.bmp", D3DXIFF_BMP, sysmemBB, NULL, NULL );
#endif

			D3DSURFACE_DESC sdesc;
			sysmemBB->GetDesc( &sdesc );

			g_AntiCheat_PrePresentPixels_Width = sdesc.Width;
			g_AntiCheat_PrePresentPixels_Height = sdesc.Height;

			g_AntiCheat_PrePresentPixels.Resize( g_AntiCheat_PrePresentPixels_Width * g_AntiCheat_PrePresentPixels_Height );

			D3DLOCKED_RECT lrect;

			sysmemBB->LockRect( &lrect, NULL, D3DLOCK_READONLY );

			r3d_assert( lrect.Pitch = sdesc.Width * sizeof( DWORD ) );

			memcpy( &g_AntiCheat_PrePresentPixels[ 0 ], lrect.pBits, lrect.Pitch * sdesc.Height );

			sysmemBB->UnlockRect();
		}
	}
}

//------------------------------------------------------------------------

void UpdateD3DAntiCheatPostPresent()
{
	if( !r3dRenderer->DeviceAvailable )
	{
		InvalidateScreenHelperAnticheat();
		return;
	}

	if( EnableD3DAntiCheatCodepath[ ANTICHEAT_SCREEN_HELPERS2 ] )
	{
		bool exceededMaxFrames = ++g_AntiCheat_ScreenHelper2Counter > 10;

		if( UpdateD3DAntiCheat_ScreenHelpers2( exceededMaxFrames ) )
		{
			if( exceededMaxFrames )
			{
#ifndef FINAL_BUILD
				r3dOutToLog( "AntiCheat catching attempt exceeded maximum allowed frames!\n" );
#endif
			}

			g_AntiCheat_PrePresentPixels.Clear();
			EnableD3DAntiCheatCodepath[ ANTICHEAT_SCREEN_HELPERS2 ] = false;
		}
	}
}

//------------------------------------------------------------------------

void AnticheatOnDeviceReset()
{
	InvalidateScreenHelperAnticheat();
}

//------------------------------------------------------------------------

bool UpdateD3DAntiCheat_ScreenHelpers2( bool lastAttempt )
{
	if( !g_AntiCheat_PrePresentPixels.Count() )
		return false;

	bool result = false;

	HDC windowDC = GetWindowDC( r3dRenderer->HLibWin );

	WINDOWINFO winfo;
	memset( &winfo, 0, sizeof winfo );

	winfo.cbSize = sizeof winfo;

	GetWindowInfo( r3dRenderer->HLibWin, &winfo );

	// put window at 0 0 for convenience
	winfo.rcClient.left -= winfo.rcWindow.left;
	winfo.rcClient.right -= winfo.rcWindow.left;
	winfo.rcClient.top -= winfo.rcWindow.top;
	winfo.rcClient.bottom -= winfo.rcWindow.top;

	winfo.rcWindow.bottom -= winfo.rcWindow.top;
	winfo.rcWindow.top = 0;

	winfo.rcWindow.right -= winfo.rcWindow.left;
	winfo.rcWindow.left = 0;

	int	nTotalWid = winfo.rcWindow.right - winfo.rcWindow.left,
		nTotalHeight = winfo.rcWindow.bottom - winfo.rcWindow.top;

	int nClientWid = winfo.rcClient.right - winfo.rcClient.left,
		nClientHt = winfo.rcClient.bottom - winfo.rcClient.top;

	if( r_fullscreen->GetInt() )
	{
		nTotalWid = g_AntiCheat_PrePresentPixels_Width;
		nTotalHeight = g_AntiCheat_PrePresentPixels_Height;

		nClientWid = g_AntiCheat_PrePresentPixels_Width;
		nClientHt = g_AntiCheat_PrePresentPixels_Height;
	}

	if( g_AntiCheat_PrePresentPixels_Width != nClientWid
			||
		g_AntiCheat_PrePresentPixels_Height != nClientHt )
	{
		return false;
	}

	HDC hBitmapdc;
	HBITMAP hBitmap, hOriginal;

	if( windowDC || r_fullscreen->GetInt() )
	{
		DWORD* pixel = NULL;

		if( !r_fullscreen->GetInt() )
		{
			hBitmap = CreateCompatibleBitmap(windowDC,nTotalWid,nTotalHeight);

			hBitmapdc = CreateCompatibleDC(windowDC);

			hOriginal = (HBITMAP)SelectBitmap(hBitmapdc, hBitmap);

			BitBlt(	hBitmapdc, 0, 0, nTotalWid, nTotalHeight,
					windowDC, 0, 0, SRCCOPY );

			BITMAPINFO bmpInfo;

			memset( &bmpInfo, 0, sizeof bmpInfo );

			bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmpInfo.bmiHeader.biWidth = nTotalWid;
			bmpInfo.bmiHeader.biHeight = nTotalHeight;
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biBitCount = 32;
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biSizeImage = 0;

			GetDIBits( hBitmapdc, hBitmap, 0, nTotalHeight, NULL, &bmpInfo, DIB_RGB_COLORS );

			r3d_assert( nTotalWid * nTotalHeight * sizeof( DWORD ) <= bmpInfo.bmiHeader.biSizeImage );

			pixel = (DWORD*)malloc( bmpInfo.bmiHeader.biSizeImage );
			GetDIBits( hBitmapdc, hBitmap, 0, nTotalHeight, pixel, &bmpInfo, DIB_RGB_COLORS );

#if 0
			IDirect3DSurface9 *sysmemBB = r3dRenderer->GetTempSurfaceForScreenShots();

			D3DLOCKED_RECT lrect;
			D3D_V( sysmemBB->LockRect( &lrect, NULL, 0 ) );

			D3DSURFACE_DESC smbbDesc;
			sysmemBB->GetDesc( &smbbDesc );

			r3d_assert( lrect.Pitch * smbbDesc.Height == nClientWid * nClientHt * sizeof( DWORD ) );

			for( int i = 0, e = nClientHt; i < e ; i ++ )
			{
				memcpy( (char*)lrect.pBits + lrect.Pitch * i, pixel + ( nTotalHeight - 1 - ( i + winfo.rcClient.top ) ) * nTotalWid + winfo.rcClient.left, lrect.Pitch );
			}

			D3D_V( sysmemBB->UnlockRect() );

			D3DXSaveSurfaceToFile( "test1.bmp", D3DXIFF_BMP, sysmemBB, NULL, NULL );
#endif
			DeleteDC( hBitmapdc );
			DeleteObject( hBitmap );
		}
		else
		{
			int pixelDataSize = g_AntiCheat_PrePresentPixels_Width * g_AntiCheat_PrePresentPixels_Height * sizeof( DWORD );

			pixel = (DWORD*)malloc( pixelDataSize );

			struct Locals
			{
				Locals()
				{
					BBuf = 0;
				}

				~Locals()
				{
					if( BBuf ) BBuf->Release();
				}

				IDirect3DSurface9 *BBuf;

			} locals; (void)locals;

			r3dRenderer->GetRT(0, &locals.BBuf);

			IDirect3DDevice9 *d = r3dRenderer->pd3ddev;

			IDirect3DSurface9 *sysmemBB = r3dRenderer->GetTempSurfaceForScreenShots();

			if (!SUCCEEDED(d->GetRenderTargetData(locals.BBuf, sysmemBB)))
			{
				InvalidateScreenHelperAnticheat();
				return false;
			}

			D3DSURFACE_DESC sdesc;
			sysmemBB->GetDesc( &sdesc );

			D3DLOCKED_RECT lrect;

			sysmemBB->LockRect( &lrect, NULL, D3DLOCK_READONLY );

			r3d_assert( lrect.Pitch * sdesc.Height == pixelDataSize );

			memcpy( pixel, lrect.pBits, lrect.Pitch * sdesc.Height );

			sysmemBB->UnlockRect();

#if 0
			D3DXSaveSurfaceToFile( "test1.jpg", D3DXIFF_JPG, sysmemBB, NULL, NULL );
#endif
		}

		int subAreaHeight = int( nClientHt * 0.8f );
		int subAreaWidth = int( nClientWid * 0.8f );

		int yStart = ( nClientHt - subAreaHeight ) / 2;
		int yEnd = nClientHt - yStart;

		int xStart = ( nClientWid - subAreaWidth ) / 2;
		int xEnd = nClientWid - yStart;

		xStart += winfo.rcClient.left;
		xEnd += winfo.rcClient.left;

		yStart += nTotalHeight - winfo.rcClient.bottom;
		yEnd += nTotalHeight - winfo.rcClient.bottom;

		int totalSubArea = ( xEnd - xStart ) * ( yEnd - yStart );
		int unequalCount = 0;

		//------------------------------------------------------------------------
#if 0
		FILE * fout = fopen( "test0.raw", "wb" );

		for( int y = 0, e = nTotalHeight; y < e; y ++ )
		{
			for( int x = 0, e = nTotalWid; x < e; x ++ )
			{
				fputc( pixel[ x + y * nTotalWid ], fout );
			}
		}

		fclose( fout );

		FILE * fout1 = fopen( "test1.raw", "wb" );

		for( int y = 0, e = nClientHt; y < e; y ++ )
		{
			for( int x = 0, e = nClientWid; x < e; x ++ )
			{
				fputc( g_AntiCheat_PrePresentPixels[ x + y * nClientWid ], fout1 );
			}
		}

		fclose( fout1 );
#endif

		//------------------------------------------------------------------------

		for( int y = yStart, e = yEnd; y < e; y ++ )
		{
			for( int x = xStart, e = xEnd; x < e; x ++ )
			{
				int localX = x - winfo.rcClient.left;
				int localY =
						r_fullscreen->GetInt() ?
							y - winfo.rcClient.top
								:
							( nTotalHeight - y - 1 ) - winfo.rcClient.top;

				DWORD pixel0 = pixel[ x + y * nTotalWid ] & 0xffffff;
				DWORD pixel1 = g_AntiCheat_PrePresentPixels[ localX + localY * g_AntiCheat_PrePresentPixels_Width ] & 0xffffff;

				if( pixel0 != pixel1 )
				{
					unequalCount ++;
				}
			}
		}

#if 0
		r3dOutToLog( "%d\n", unequalCount );
#endif

		if( !unequalCount )
		{
			result = true;
		}
		else
		if( lastAttempt )
		{
			result = true;

			// caught some significant extra stuff on screen
			D3DCheaters[ ANTICHEAT_SCREEN_HELPERS2 ] = true;

			int texWidth = 386;
			int texHeight = 256;

			if( !g_TempSaveDataS )
			{
				D3D_V( r3dRenderer->pd3ddev->CreateTexture( texWidth, texHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &g_TempSaveDataS, NULL ) );
			}

			if( g_TempSaveDataS )
			{
				D3DLOCKED_RECT lrect;

				g_TempSaveDataS->LockRect( 0, &lrect, NULL, 0 );

				r3d_assert( lrect.Pitch == texWidth * sizeof( DWORD ) );

				memset( lrect.pBits, 0, lrect.Pitch * texHeight );

				for( int y=0, yy=0; y < texHeight; y++, yy = y * nClientHt / texHeight )
				{
					for( int x=0, xx=0; x < texWidth; x++, xx = x * nClientWid / texWidth )
					{
						int src_idx = ( nTotalHeight - winfo.rcClient.top - yy - 1 ) * nTotalWid + xx + winfo.rcClient.left;
						if( src_idx < nTotalWid * nTotalHeight )
						{
							if(r_fullscreen->GetInt()) // in fullscreen image in inverted, so write it upside down to correct for that
								*((DWORD*)lrect.pBits + (texHeight-y-1) * texWidth + x) = pixel[ src_idx ];
							else
								*((DWORD*)lrect.pBits + y * texWidth + x) = pixel[ src_idx ];
						}
						else
						{
#ifndef FINAL_BUILD
							r3d_assert(false);
#endif
						}
					}
				}

				g_TempSaveDataS->UnlockRect( 0 );

#if 0
				D3DXSaveTextureToFile( "test_final.jpg", D3DXIFF_JPG, g_TempSaveDataS, NULL );
#endif
			}
		}

		free( pixel );

		ReleaseDC( r3dRenderer->HLibWin, windowDC );
	}

	return result;
}

//------------------------------------------------------------------------

IDirect3DTexture9* GetCheatScreenshot()
{
	return g_TempSaveDataS;
}

void ReleaseCheatScreenshot()
{
	SAFE_RELEASE( g_TempSaveDataS );
}

//------------------------------------------------------------------------

void InvalidateScreenHelperAnticheat()
{
	g_AntiCheat_PrePresentPixels.Clear();
	g_AntiCheat_ScreenHelper2Counter = 0;
}