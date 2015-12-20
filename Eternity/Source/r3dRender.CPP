#include	"r3dPCH.h"
#include	"r3d.h"

#pragma warning(disable:4996)

#include <ddraw.h>
#include <oleauto.h>
#include <initguid.h>
#include <wbemidl.h>

#ifndef WO_SERVER
#include "NVApi/nvapi.h"
#include "ATICrossfireDetect/atimgpud.h"
#define R3D_NVAPI "NVApi: "
#pragma comment(lib, "../External/NVApi/x86/nvapi.lib")
#pragma comment(lib, "../External/ATICrossfireDetect/atimgpud_s_x86.lib")
#endif

#include "../SF/Console/Config.h"

#include "r3dDeviceQueue.h"

#include "r3dBackgroundTaskDispatcher.h"

// PunkBuster SDK
#ifdef __WITH_PB__
#include "../../External/PunkBuster/pbcommon.h"
#endif

#ifndef FINAL_BUILD
#define __NVPERFHUD_2 		1
#endif

#define USE_RASTER_CLIPPING	1	// use own raster clipping

static	int 			CurrentMaterialID;
IDirect3DBaseTexture9* 	CurrentTexID[ 16 ];
static	int 			CurrentRenderingMode;
#define RENDMODE_STACK_SIZE	16
static	int		RModeStack[RENDMODE_STACK_SIZE];
static	int		RModeStackTop;
float 		__r3dGlobalAspect = 1.0f;
int gSLI_Crossfire_NumGPUs = 1;

int gResetCount = 0;

r3dTexture*	__r3dShadeTexture[16];
#ifndef FINAL_BUILD
r3dTexture*	__r3dMipLevelTextures[5];
#endif
int	__r3dDisplayMipLevels = 0;

char	__r3dBaseShaderPath[256];
char	__r3dBaseShaderCachePath[256];

static	r3dVertexArray*	vtxArray2D;
static	r3dVertexArray*	vtxArrayMinimalist2D_Lines;
static	r3dVertexArray*	vtxArray3D;
static	r3dVertexArray*	vtxArray3D_Lines;

static int gLockCounter = 0 ;
static int gIntegrityCounter = 0 ;

static int		gWatchDogEnable = 0 ;
static float	gWatchDogLastReset = 0.f ;
static float	gWatchDogTimeOut = 0.f ;
const char*		gWatchDogItemName = 0 ;

r3dgfxMap(DWORD, r3dSTLString) _r3d_mShaderMap;
r3dgfxMap(DWORD, r3dSTLString) _r3d_mVBufferMap;

CRITICAL_SECTION g_ResourceCritSection ;

int	VS_Mode = D3DCREATE_HARDWARE_VERTEXPROCESSING;

DWORD gMainThreadID = GetCurrentThreadId();

void r3dFrameQueryRelease();

void r3dGPUStats::Reset()
{
	mNumTerrainDraws			= 0;
	mNumTerrainTris				= 0;
	mNumDraws    				= 0;
	mNumMaterialChanges			= 0;
	mNumModesChanges       		= 0;
	mNumTextureChanges     		= 0;
	mNumTrianglesRendered  		= 0;
	mAverageStripLength    		= 200;
	mNumOcclusionQueries   		= 0;
	mNumParticlesRendered		= 0;
	mNumLocks					= 0;
	mNumLocksDiscard			= 0;
	mNumLocksNooverwrite		= 0;
	mBytesLocked				= 0;
	mNumVisGridOcclusions		= 0;
	mNumDoubleShadowDips		= 0;
	mNumShadowDips				= 0;
}

void r3dGPUStats::ResetVMemStats()
{
	mTexMem				= 0;
	mPlayerTexMem		= 0;
	mUITexMem			= 0;
	mBufferMem			= 0;
	mPlayerBufferMem	= 0;
	mTerrainBufferDXMem	= 0;
	mGrassBufferDXMem	= 0;
	mOtherBufferDXMem	= 0;
	mUIBufferMem		= 0;
	mRenderTargetMem	= 0;
	mVMemTotal			= 0;
}

static void ShowCeilingWarning( void* )
{
	int at = r_vmem_msg_target->GetInt() ;

	char buf[ 256 ] ;
	sprintf( buf, "Hitting memory ceiling of %d MB", at / 1024 / 1024 ) ;

	MessageBox( r3dRenderer->HLibWin, buf, "Message", MB_OK ) ;

	r_vmem_msg_shown->SetInt( 1 ) ;
}

void r3dGPUStats::AddVMem( int val )
{
	InterlockedExchangeAdd( &mVMemTotal, val ) ;

	int at = r_vmem_msg_target->GetInt() ;

	if( at && mVMemTotal >= at && !r_vmem_msg_shown->GetInt() )
	{
		ProcessCustomDeviceQueueItem( ShowCeilingWarning, NULL ) ;
	}
}

void r3dGPUStats::AddTexMem( int val )
{
	InterlockedExchangeAdd( &mTexMem, val ) ;
	AddVMem( val ) ;
}

void r3dGPUStats::AddPlayerTexMem( int val )
{
	InterlockedExchangeAdd( &mPlayerTexMem, val ) ;
	AddTexMem( val ) ;
}

void r3dGPUStats::AddUITexMem( int val )
{
	InterlockedExchangeAdd( &mUITexMem, val ) ;
	AddTexMem( val );
}

void r3dGPUStats::AddBufferMem( int val )
{
	InterlockedExchangeAdd( &mBufferMem, val ) ;
	AddVMem( val ) ;
}

void r3dGPUStats::AddUIBufferMem( int val )
{
	InterlockedExchangeAdd( &mUIBufferMem, val ) ;
	AddVMem( val );
}

void r3dGPUStats::AddPlayerBufferMem( int val )
{
	InterlockedExchangeAdd( &mPlayerBufferMem, val ) ;
}

void r3dGPUStats::AddRenderTargetMem( int val )
{
	InterlockedExchangeAdd( &mRenderTargetMem, val ) ;
	AddVMem( val ) ;
}

void r3dGPUStats::AddTerrainBufferDXMem( int val )
{
	InterlockedExchangeAdd( &mTerrainBufferDXMem, val ) ;
	AddVMem( val ) ;
}

void r3dGPUStats::AddGrassBufferDXMem( int val )
{
	InterlockedExchangeAdd( &mGrassBufferDXMem, val ) ;
	AddVMem( val ) ;
}

void r3dGPUStats::AddOtherBufferDXMem( int val )
{
	InterlockedExchangeAdd( &mOtherBufferDXMem, val ) ;
	AddVMem( val ) ;
}

R3D_FORCEINLINE void r3dRenderLayer::SetBlackPixelShader()
{
	if( ForceBlackPixelShaderId >= 0 )
	{
		PixelShaders[ ForceBlackPixelShaderId ].SetActive( 1 );
	}
}

void r3dRenderLayer::SetMipMapBias(float Bias, int Stage)
{
	if (Stage != -1)
		pd3ddev->SetSamplerState(Stage, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
	else
	{
		pd3ddev->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
		pd3ddev->SetSamplerState(1, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
		pd3ddev->SetSamplerState(2, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
		pd3ddev->SetSamplerState(3, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
		pd3ddev->SetSamplerState(4, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
		pd3ddev->SetSamplerState(5, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
		pd3ddev->SetSamplerState(6, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
		pd3ddev->SetSamplerState(7, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&Bias)));
	}
}

void r3dRenderLayer::ResetShaders(int bResetSystemShaders)
{
	for (int i=0;i<NumPixelShaders;i++) PixelShaders[i].Unload();

	for (int i=0;i<NumVertexShaders;i++) VertexShaders[i].Unload();

	NumPixelShaders = 0;
	NumVertexShaders = 0;
}

int r3dRenderLayer::AddPixelShaderFromFile(const char* ShaderName, const char* FileName, int bSystem)
{
	r3dTL::TArray <D3DXMACRO> defines;
	return AddPixelShaderFromFile(ShaderName, FileName, bSystem, defines);
}

int r3dRenderLayer::AddPixelShaderFromFile(const char* ShaderName, const char* FileName, int bSystem, const ShaderMacros & defines)
{
	if( !allowShaderLoading_ )
	{
		r3dError( "r3dRenderLayer::AddPixelShaderFromFile: shader is trying to be loaded when it's not alowed! Move shader loading to renderer initialization area (r3dInitShaders() (%s - %s)!", ShaderName, FileName );
	}

	int ShType = 0;

	int idx = GetShaderIdx(ShaderName);
	if (idx != -1 ) return idx;

	if (strstr(FileName, ".hls")) ShType = 1;

	// r3dOutToLog("ShaderType  %s  is %d\n", FileName, ShType);

	r3d_assert(NumPixelShaders < NUM_SHADERS_SIZE);

	r3dscpy(PixelShaders[NumPixelShaders].Name, ShaderName);
	PixelShaders[NumPixelShaders].bSystem = bSystem;

	if( !PixelShaders[NumPixelShaders].Load( FileName, ShType, defines ) )
	{
		r3dError( "Pixel Shader Compilation Failed!\n%s", LastCompilationError );
	}

	NumPixelShaders++;
	return NumPixelShaders-1;
}

void r3dRenderLayer::ReloadShaderByFileName(const char* FileName)
{
	char base_file[MAX_PATH];
	r3dscpy(base_file, FileName + strlen(__r3dBaseShaderPath) + 1);

	// skip cached shaders (somehow they still have .hls extension)
	if(strnicmp(base_file, "Cache", 5) == 0)
		return;

	r3dOutToLog("r3d: trying to reload shader %s\n", FileName);

	for(int i=0; i<NumPixelShaders; i++)
	{
		r3dPixelShader& sh = PixelShaders[i];
		if( r3dIsSamePath(sh.FileName, FileName)) 
		{
			r3dOutToLog("  reloading PS %s\n", sh.Name);
			ReloadPixelShader( sh.Name, base_file );
		}
	}

	for(int i=0; i<NumVertexShaders; i++)
	{
		r3dVertexShader& sh = VertexShaders[i];
		if( r3dIsSamePath(sh.FileName, FileName) )
		{
			r3dOutToLog("  reloading VS %s\n", sh.Name);
			ReloadVertexShader( sh.Name, base_file );
		}
	}
}

void
r3dRenderLayer::ReloadPixelShader( const char* ShaderName, const char* FileName )
{
	int ShType = 0;

	int idx = GetShaderIdx( ShaderName );
	if (idx != -1 )
	{
		if( strstr( FileName, ".hls" ) ) ShType = 1;

		r3dPixelShader::MacroArr preservedMacros = PixelShaders[ idx ].Macros;

		ShaderMacros macros( preservedMacros.Count() );

		for( uint32_t i = 0, e = macros.Count(); i < e; i ++ )
		{
			macros[ i ].Name		= preservedMacros[ i ].Name.c_str();
			macros[ i ].Definition	= preservedMacros[ i ].Definition.c_str();
		}

		if( !PixelShaders[ idx ].Load( FileName, ShType, macros ) )
		{
			MessageBoxA( NULL, LastCompilationError, "Pixel Shader Compilation Failed!", MB_ICONEXCLAMATION );
		}
	}
	else
	{
		AddPixelShaderFromFile( ShaderName, FileName );
	}
}

void
r3dRenderLayer::ReloadPixelShader( const char* ShaderName, const char* FileName, const ShaderMacros & macros )
{
	int ShType = 0;

	int idx = GetShaderIdx( ShaderName );
	if (idx != -1 )
	{
		if( strstr( FileName, ".hls" ) ) ShType = 1;

		if( !PixelShaders[ idx ].Load( FileName, ShType, macros ) )
		{
			MessageBoxA( NULL, LastCompilationError, "Pixel Shader Compilation Failed!", MB_ICONEXCLAMATION );
		}
	}
	else
	{
		AddPixelShaderFromFile( ShaderName, FileName );
	}
}

int r3dRenderLayer::AddVertexShaderFromFile(const char* ShaderName, const char* FileName, int bSystem)
{
	ShaderMacros defines;
	return AddVertexShaderFromFile(ShaderName, FileName, bSystem, defines);
}

int r3dRenderLayer::AddVertexShaderFromFile(const char* ShaderName, const char* FileName, int bSystem, const ShaderMacros & defines)
{
	if( !allowShaderLoading_ )
	{
		r3dError( "r3dRenderLayer::AddVertexShaderFromFile: shader is trying to be loaded when it's not alowed! Move shader loading to renderer initialization area (r3dInitShaders()) (%s - %s)!", ShaderName, FileName );
	}

	int ShType = 0;

	int idx = GetShaderIdx(ShaderName);
	if (idx != -1 ) return idx;

	if (strstr(FileName, ".hls")) ShType = 1;

	r3d_assert(NumVertexShaders < NUM_SHADERS_SIZE);

	if( !VertexShaders[NumVertexShaders].Load(FileName, ShType, defines) )
	{
		r3dError( "Vertex Shader Compilation Failed!\n%s", LastCompilationError );
	}
	r3dscpy(VertexShaders[NumVertexShaders].Name, ShaderName);
	VertexShaders[NumVertexShaders].bSystem = bSystem;

	NumVertexShaders++;
	return NumVertexShaders-1;
}

void
r3dRenderLayer::ReloadVertexShader( const char* ShaderName, const char* FileName )
{
	int ShType = 0;

	int idx = GetShaderIdx(ShaderName);
	if (idx != -1 )
	{
		if (strstr(FileName, ".hls")) ShType = 1;

		r3dVertexShader::MacroArr preservedMacros = VertexShaders[ idx ].Macros;

		ShaderMacros macros( preservedMacros.Count() );

		for( uint32_t i = 0, e = macros.Count(); i < e; i ++ )
		{
			macros[ i ].Name		= preservedMacros[ i ].Name.c_str();
			macros[ i ].Definition	= preservedMacros[ i ].Definition.c_str();
		}

		if( !VertexShaders[ idx ].Load( FileName, ShType, macros ) )
		{
			MessageBoxA( NULL, LastCompilationError, "Vertex Shader Compilation Failed!", MB_ICONEXCLAMATION );
		}
	}
	else
	{
		AddVertexShaderFromFile( ShaderName, FileName );
	}
}


int r3dRenderLayer::GetShaderIdx(const char* Name)
{ 
	for(int i=0;i<NumPixelShaders;i++)
		if (!stricmp(PixelShaders[i].Name, Name))  return i;

	for(int i=0;i<NumVertexShaders;i++)
		if (!stricmp(VertexShaders[i].Name, Name))  return i;

	return -1;
}

int
r3dRenderLayer::GetPixelShaderIdx( const char* Name )
{
	for(int i=0;i<NumPixelShaders;i++)
		if (!stricmp(PixelShaders[i].Name, Name))  return i;

	r3d_assert( false );

	return -1;
}

int
r3dRenderLayer::GetVertexShaderIdx( const char* Name )
{
	for(int i=0;i<NumVertexShaders;i++)
		if (!stricmp(VertexShaders[i].Name, Name))  return i;

	r3d_assert( false );

	return -1;
}

void r3dRenderLayer::SetPixelShader(const char* Name)
{
	if( ForceBlackPixelShader )
	{
		SetBlackPixelShader();
		return;
	}

	for(int i=0;i<NumPixelShaders;i++) {
		if(stricmp(PixelShaders[i].Name, Name) == NULL) {
			CurrentPixelShaderID = i;
			PixelShaders[i].SetActive(1);
			return;
		}
	}

	r3d_assert( false ) ;
	
	CurrentPixelShaderID = -1;
	D3D_V( d3dc._SetPixelShader(0) );
}


void r3dRenderLayer::SetPixelShader(int Id)
{
	if( ForceBlackPixelShader )
	{
		SetBlackPixelShader();
		return;
	}

	CurrentPixelShaderID = Id;
	if(Id != -1) {
		PixelShaders[Id].SetActive(1);
		return;
	}
	
	D3D_V( d3dc._SetPixelShader(0) );
}



void r3dRenderLayer::SetVertexShader(const char* Name)
{
	for(int i=0;i<NumVertexShaders;i++) {
		if(stricmp(VertexShaders[i].Name, Name) == NULL) {
			VertexShaders[i].SetActive(1);
			CurrentVertexShaderID = i;
			return;
		}
	}

	r3d_assert( false ) ;

	CurrentVertexShaderID = -1;
	D3D_V( d3dc._SetVertexShader(0) );
	d3dc._SetDecl(R3D_DEBUG_VERTEX::getDecl());
}

void r3dRenderLayer::SetVertexShader(int Id)
{
	CurrentVertexShaderID = Id;
	if(Id != -1) {
		VertexShaders[Id].SetActive(1);
		return;
	}

	//  Flush();
	D3D_V( d3dc._SetVertexShader(0) );
	d3dc._SetDecl(R3D_DEBUG_VERTEX::getDecl());
}

void r3dRenderLayer::SetValidVertexShader(int id )
{
	r3d_assert( id >= 0 && id < NUM_SHADERS_SIZE ) ;

	CurrentVertexShaderID = id;
	VertexShaders[id].SetActive(1);
}

void r3dRenderLayer::SetDefaultCullMode( D3DCULL CullMode )
{
	DefaultCullMode = CullMode ;
	SetCullMode( CullMode );
}

int r3dRenderLayer::GetCurrentPixelShaderIdx() const
{
	return CurrentPixelShaderID;
}

int r3dRenderLayer::GetCurrentVertexShaderIdx() const
{
	return CurrentVertexShaderID;
}



HANDLE		r3d_Thread1;

int r3dRenderLayer::Init(HWND hWindow, const char* DisplayName)
{
	InitializeCriticalSection( &g_ResourceCritSection ) ;

	r3dTexture::Init() ;
	r3dMesh::Init() ;

	InitDeviceQueue();

	SupportsVertexTextureFetch = 1 ;

	gMainThreadID = GetCurrentThreadId();

	void checkLicenseKey();
	checkLicenseKey();

	extern	void 		r3d_UtilityThread(DWORD in);

	// expose this process handle to other threads
	DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), 
		&r3d_CurrentProcess, PROCESS_ALL_ACCESS, TRUE, 0);
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
		&r3d_MainThread, THREAD_ALL_ACCESS, TRUE, 0);

#ifndef FINAL_BUILD
	// create utility thread
	DWORD temp;
	r3d_Thread1 = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)r3d_UtilityThread,
		(void *)NULL,		// argument
		0,			// creation flags
		&temp			// thread ID
		);
#endif

	//
	//FIXME: add support for DisplayName, second monitors and something...
	//
	Version      = R3D_ETERNITY_VERSION;
	HLibWin      = hWindow;

	//  RTBuffer = NULL;

	AmbientColor = r3dColor(0,0,0);

	// interfaces
	pdi          = NULL;
	pd3d         = NULL;

	// textures
	FirstTexture = NULL;

	// ptumik: WTF this crap is doing in render layer?!?! TODO: fix this crap!
	Keyboard = game_new r3dKeyboard;
	Mouse = game_new r3dMouse;
	Gamepad = game_new r3dGamepad;

	// create & init direct input
	if(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&pdi, NULL) == DI_OK)
	{
		Keyboard->SetCapture();
		//!r3d->Mouse.SetCapture();
	}
	r3dOutToLog ("Input Devices enabled: Mouse, Keyboard\n");

	// create & init direct draw
	pd3d = Direct3DCreate9(D3D_SDK_VERSION);

	if(pd3d == NULL)
		return FALSE;

	//------------------------------------------------------------------------

	D3DCAPS9 Caps;
	ZeroMemory( &Caps, sizeof(Caps) );
	pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);

	// can be NULL in server executable...
	if( r_max_texture_dim )
	{
		int max_tex_dim = R3D_MIN( Caps.MaxTextureWidth, Caps.MaxTextureHeight ) ;

		r_max_texture_dim->SetInt( max_tex_dim );

		// fix some settings
		r_dir_sm_size->SetInt( R3D_MIN( max_tex_dim, r_dir_sm_size->GetInt() ) );
		r_shared_sm_size->SetInt( R3D_MIN( max_tex_dim, r_shared_sm_size->GetInt() ) );
	}


	deviceLost_ = false;

	// ok, redetect everything

	NumPixelShaders = 0;
	NumVertexShaders = 0;

	unlink("Default.ini");
	unlink("DefaultShaderFile.shader");

	int LocalVideoMemoryDDraw() ;
	int LocalVideoMemoryWMI() ;

	int vmemDDraw	= LocalVideoMemoryDDraw() ;
	int vmemWMI		= LocalVideoMemoryWMI() ;

	r3dOutToLog( "VMEM As seen through DDRaw: %d\n", vmemDDraw / 1024 / 1024 ) ;
	r3dOutToLog( "VMEM As seen through WMI: %d\n", vmemWMI / 1024 / 1024 ) ;

	r_local_vmem_size->SetInt( R3D_MAX( vmemDDraw, vmemWMI ) ) ;
	r_local_vmem_ddraw->SetInt( vmemDDraw ) ;
	r_local_vmem_wmi->SetInt( vmemWMI ) ;

	return TRUE;
}


void r3dRenderLayer::FlushTextures()
{
	while(FirstTexture)
		DeleteTexture(FirstTexture, 1);
}

void
r3dRenderLayer::SetD3DDev( IDirect3DDevice9* dev )
{
#if R3D_DEBUG_MULTITHREADING
	pd3ddev.Set( dev );
#else
	pd3ddev = dev;
#endif
}

int r3dRenderLayer::Close()
{
#ifndef WO_SERVER 
	if( NVApiStereoHandle )
	{
		if( NvAPI_Stereo_DestroyHandle( NVApiStereoHandle ) != NVAPI_OK )
		{
			r3dOutToLog( R3D_NVAPI "failed to destroy stereo handle!" ) ;
		}
	}
#endif

	r3dMesh::Close() ;

	CloseDeviceQueue();

	unlink("Default.ini");
	unlink("DefaultShaderFile.shader");

	r3dMaterialLibrary::Destroy();

	delete _r3dSystemFont;

	//  TerminateThread(r3d_Thread1, 0);

	int refcnt;

	// destroy input devices
	delete Mouse;
	delete Keyboard;
	delete Gamepad;


	if(pdi) {
		refcnt = pdi->Release();
		if(refcnt) r3dOutToLog("WARNING: pdi have %d reference counts\n", refcnt);
		else       pdi = NULL;
	}
	if(pdi) {
		while( (refcnt = pdi->Release()) != 0) 
			;
		pdi = NULL;
	}

	r3dOutToLog("\n");

	// clear shaders
	DeleteTexture(__r3dShadeTexture[SHADETEXT_BLUE]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_DEFAULT_NORMAL]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_WHITE]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_BLACK]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_MISSING]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_NOISE]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_SUNFLAREMASK]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_DEFAULT_METALNESS]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_GREY]);
#ifndef FINAL_BUILD
	DeleteTexture(__r3dShadeTexture[SHADETEXT_DENSITY_CHECKER]);

	DeleteTexture(__r3dMipLevelTextures[0]);
	DeleteTexture(__r3dMipLevelTextures[1]);
	DeleteTexture(__r3dMipLevelTextures[2]);
	DeleteTexture(__r3dMipLevelTextures[3]);
	DeleteTexture(__r3dMipLevelTextures[4]);
#endif

	DeleteTexture(__r3dShadeTexture[SHADETEXT_DEFAULT_DISTORTION]);

	DeleteTexture(__r3dShadeTexture[SHADETEXT_WINTER_DIFFUSE]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_WINTER_NORMAL]);
	DeleteTexture(__r3dShadeTexture[SHADETEXT_WINTER_SPECULAR]);

	ResetShaders();

	r3dFrameQueryRelease();	

	// destroy all textures
	PurgeThumbnailTextures();
	FlushTextures();

	extern IDirect3DTexture9* _r3d_screenshot_copy;
	SAFE_RELEASE(_r3d_screenshot_copy);

	SAFE_DELETE(vtxArray2D);
	SAFE_DELETE(vtxArray3D);
	SAFE_DELETE(vtxArray3D_Lines);
	SAFE_DELETE(vtxArrayMinimalist2D_Lines)

	SAFE_RELEASE( MainRT );
	SAFE_RELEASE( MainDSS );

	SAFE_RELEASE( sysMemSurfaceForScreenShots );
	SAFE_RELEASE( sysMemTextureForFrontBufferCopy );

	for(uint32_t i=0; i<VertexDecls.size(); ++i)
		VertexDecls[i]->Release();
	VertexDecls.clear();

#ifdef _DEBUG
	{
		r3dgfxMap(DWORD, r3dSTLString)::iterator pIter;
		for(pIter = _r3d_mShaderMap.begin(); pIter != _r3d_mShaderMap.end(); ++pIter) {
			r3dOutToLog("WARNING: Shader '%s' not unloaded\n", (*pIter).second.c_str());
		}

		for(pIter = _r3d_mVBufferMap.begin(); pIter != _r3d_mVBufferMap.end(); ++pIter) {
			r3dOutToLog("WARNING: Vertex Buffer '%s' not unloaded\n", (*pIter).second.c_str());
		}
	}
#endif

	r3dTexture::Close() ;

	/*
	if (RTBuffer)
	{
	RTBuffer->Release();
	RTBuffer = NULL;
	}
	*/

	if(pd3ddev) {
		refcnt = pd3ddev->Release();
		if(!refcnt) SetD3DDev( NULL );
		if(refcnt) r3dOutToLog("WARNING: pd3ddev have %d reference counts\n", refcnt);
	}
	if(pd3ddev) {
		// force destroy: warning: may crash!
		while( (refcnt = pd3ddev->Release()) != 0) 
			;
		SetD3DDev( NULL );
	}

	if(pd3d) {
		refcnt = pd3d->Release();
		if(!refcnt) pd3d = NULL;
		if(refcnt) r3dOutToLog("WARNING: pd3d have %d reference counts\n", refcnt);
	}
	if(pd3d) {
		// force destroy: warning: may crash!
		while( (refcnt = pd3d->Release()) != 0) 
			;
		pd3d = NULL;
	}

	DeleteCriticalSection( &g_ResourceCritSection ) ;

	return TRUE;
}



struct 
{
	char		*ModeName;
	int		ModeCode;
} \
_ModeNames[] =
{
	{ "D3DFMT_R8G8B8               ", 20 },
	{ "D3DFMT_A8R8G8B8             ", 21 },
	{ "D3DFMT_X8R8G8B8             ", 22 },
	{ "D3DFMT_R5G6B5               ", 23 },
	{ "D3DFMT_X1R5G5B5             ", 24 },
	{ "D3DFMT_A1R5G5B5             ", 25 },
	{ "D3DFMT_A4R4G4B4             ", 26 },
	{ "D3DFMT_R3G3B2               ", 27 },
	{ "D3DFMT_A8                   ", 28 },
	{ "D3DFMT_A8R3G3B2             ", 29 },
	{ "D3DFMT_X4R4G4B4             ", 30 },
	{ "D3DFMT_A8P8                 ", 40 },
	{ "D3DFMT_P8                   ", 41 },
	{ "D3DFMT_L8                   ", 50 },
	{ "D3DFMT_A8L8                 ", 51 },
	{ "D3DFMT_A4L4                 ", 52 },
	{ "D3DFMT_V8U8                 ", 60 },
	{ "D3DFMT_L6V5U5               ", 61 },
	{ "D3DFMT_X8L8V8U8             ", 62 },
	{ "D3DFMT_Q8W8V8U8             ", 63 },
	{ "D3DFMT_V16U16               ", 64 },
	{ "D3DFMT_W11V11U10            ", 65 },
	{ "D3DFMT_D16_LOCKABLE         ", 70 },
	{ "D3DFMT_D32                  ", 71 },
	{ "D3DFMT_D15S1                ", 73 },
	{ "D3DFMT_D24S8                ", 75 },
	{ "D3DFMT_D16                  ", 80 },
	{ "D3DFMT_D24X8                ", 77 },
	{ "D3DFMT_D24X4S4              ", 79 },
	{ "D3DFMT_VERTEXDATA           ", 100 },
	{ "D3DFMT_INDEX16              ", 101 },
	{ "D3DFMT_INDEX32              ", 102 },
};
static	int	_Num_ModeNames = sizeof(_ModeNames) / sizeof(*_ModeNames);



const char* __GetFormatName(int ModeCode)
{
	for (int i=0;i<_Num_ModeNames;i++)
		if ( _ModeNames[i].ModeCode == ModeCode ) return _ModeNames[i].ModeName;

	return "UNDEFINED MODE"; 
}

void __EvaluateDisplayModes(IDirect3D9 *d3d, ModesArray &outModes)
{
	// search if that mode available..
	D3DDISPLAYMODE mode;
	D3DFORMAT displayFormats[] = 
	{
		D3DFMT_A1R5G5B5, 
		D3DFMT_A2R10G10B10, 
		D3DFMT_A8R8G8B8, 
		D3DFMT_R5G6B5, 
		D3DFMT_X1R5G5B5, 
		D3DFMT_X8R8G8B8 

	};

	for (int k = 0; k < _countof(displayFormats); ++k)
	{
		D3DFORMAT fmt = displayFormats[k];
		int n_modes = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, fmt);

		for(int i = 0; i < n_modes; i++)
		{
			d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, fmt, i, &mode);
			outModes.PushBack(mode);
			//r3dOutToLog ("Mode %d:  \t  %dx%d  \tFormat: %s\tRefresh rate: %i\n", i, (int)mode.Width, (int)mode.Height, __GetFormatName((int)mode.Format), mode.RefreshRate);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool r3dRenderLayer::FindCompatibleNVidiaStereoDisplayMode(const char *displayName, D3DDISPLAYMODE &inOutMode, const ModesArray &allModes)
{
#ifdef WO_SERVER
	return false;
#else
	if (!NVApiActive || !NVApiStereoMode)
		return false;

	NvDisplayHandle dh;
	NvAPI_Status st = NvAPI_GetAssociatedNvidiaDisplayHandle(displayName, &dh);
	if (st != NVAPI_OK)
	{
		r3dOutToLog(R3D_NVAPI "NvAPI_GetAssociatedNvidiaDisplayHandle call error");
		return false;
	}
	NvPhysicalGpuHandle pgh[NVAPI_MAX_PHYSICAL_GPUS];
	NvU32 numGPUs = 0;

	st = NvAPI_GetPhysicalGPUsFromDisplay(dh, pgh, &numGPUs);
	if (st != NVAPI_OK)
	{
		r3dOutToLog(R3D_NVAPI "NvAPI_GetAssociatedNvidiaDisplayHandle call error");
		return false;
	}

	if (numGPUs == 0)
		return false;

	NvPhysicalGpuHandle gpuHandle = pgh[0];

	NV_GPU_DISPLAYIDS dispIDs[3];
	NvU32 dispsNum = _countof(dispIDs);
	for (NvU32 i = 0; i < dispsNum; ++i)
	{
		dispIDs[i].version = NV_GPU_DISPLAYIDS_VER;
	}
	st = NvAPI_GPU_GetConnectedDisplayIds(gpuHandle, dispIDs, &dispsNum, 0xfffffff);
	if (st != NVAPI_OK)
	{
		r3dOutToLog(R3D_NVAPI "NvAPI_GPU_GetConnectedDisplayIds call error");
		return false;
	}
	if (dispsNum == 0)
		return false;

	bool found = false;
	//	If connected through DVI and stereo is enabled, force appropriate display mode
	if (dispIDs[0].connectorType == NV_MONITOR_CONN_TYPE_DVI)
	{
		//	Only requirement is 120Hz refresh rate
		for (uint32_t i = 0; i < allModes.Count(); ++i)
		{
			const D3DDISPLAYMODE &m = allModes[i];
			if (m.RefreshRate == 120 && m.Width == inOutMode.Width && m.Height == inOutMode.Height)
			{
				inOutMode = m;
				found = true;
				break;
			}
		}
	}
	else if (dispIDs[0].connectorType == NV_MONITOR_CONN_TYPE_HDMI)
	{
		//	Force resolution and refresh rate
		for (uint32_t i = 0; i < allModes.Count(); ++i)
		{
			const D3DDISPLAYMODE &m = allModes[i];
			//	Try 1080p and 720p
			if
			(
				m.RefreshRate == 24 && m.Width == 1920 && m.Height == 1080 ||
				m.RefreshRate == 60 && m.Width == 1280 && m.Height == 720
			)
			{
				inOutMode = m;
				found = true;
				break;
			}
		}
	}

	return found;
#endif
}


//////////////////////////////////////////////////////////////////////////

bool r3dRenderLayer::SearchForModeWithOptimalRefreshRate(int w, int h, int bpp, D3DFORMAT neededFormat, D3DDISPLAYMODE &result)
{
	D3DDISPLAYMODE adapterDesktopDisplayMode;
	pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &adapterDesktopDisplayMode);

	bool modeFound = false;
	int n_modes = pd3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, neededFormat);
	int refreshRateBestRanking = 100000;

	for (int i = 0; i < n_modes; i++)
	{
		D3DDISPLAYMODE m;
		pd3d->EnumAdapterModes(D3DADAPTER_DEFAULT, neededFormat, i, &m);

		//	Skip modes with different bb formats and/or screen dimensions
		if ((int)m.Width != w || (int)m.Height != h || (int)m.Format != neededFormat)
			continue;

		int curRanking = abs(static_cast<int>(m.RefreshRate - adapterDesktopDisplayMode.RefreshRate));
		if (curRanking < refreshRateBestRanking)
		{
			refreshRateBestRanking = curRanking;
			modeFound = true;
			result = m;
		}
	}
	return modeFound;
}

//////////////////////////////////////////////////////////////////////////

bool r3dRenderLayer::AdjustWindowSize( int XRes, int YRes, int BPP, int isWindowed, D3DDISPLAYMODE& result )
{
	D3DDISPLAYMODE mode;
	if(!isWindowed)
	{
		// search if that mode available..
		D3DFORMAT NeededFormat = (BPP == 16) ? DISPLAY_FORMAT_16 : DISPLAY_FORMAT_32;
		bool ModeFound = SearchForModeWithOptimalRefreshRate(XRes, YRes, BPP, NeededFormat, mode);

		if(!ModeFound)
		{
			r3dOutToLog("ERROR: Can't find ARGB format\n");
			D3DFORMAT NeededFormat = D3DFMT_R5G6B5;
			ModeFound = SearchForModeWithOptimalRefreshRate(XRes, YRes, BPP, NeededFormat, mode);
		}

		if(!ModeFound)
		{
			r3dOutToLog( "fullscreen mode for %dx%d is not found, falling back to windowed mode\n", XRes, YRes );
			return false;
		}

		// set window params to fullscreen mode
		extern void r3dWinStyleModify(HWND hWnd, int add, DWORD style);
		r3dWinStyleModify(HLibWin, 0, WS_SYSMENU);
		r3dWinStyleModify(HLibWin, 0, WS_BORDER);
		r3dWinStyleModify(HLibWin, 0, WS_CAPTION);
		r3dWinStyleModify(HLibWin, 1, WS_DLGFRAME);
	}
	else
	{
		// get windowed display mode
		pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

		// adjust windows rect, so it will fit into screen rect
		RECT rc;
		SetRect(&rc, 0, 0, 0, 0);
		AdjustWindowRect(&rc, GetWindowLong(HLibWin, GWL_STYLE), false);

		UINT new_width  = R3D_MIN((UINT)XRes, (UINT)(mode.Width - (rc.right  - rc.left)));
		UINT new_height = R3D_MIN((UINT)YRes, (UINT)(mode.Height - (rc.bottom - rc.top)));
		if(new_width != XRes || new_height != YRes) {
			r3dOutToLog("res adjusted %dx%d->%dx%d\n", XRes, YRes, new_width, new_height);
		}

		mode.Width  = new_width;
		mode.Height = new_height;
	}
	
	r3d_assert(mode.Width && mode.Height);
	//r3dOutToLog("mode %dx%d %s\n", mode.Width, mode.Height, D3DFORMAT_String(mode.Format));

	// adjust our window size
	RECT rc;
	SetRect(&rc, 0, 0, mode.Width, mode.Height);        
	AdjustWindowRect(&rc, GetWindowLong(HLibWin, GWL_STYLE), false);
	SetWindowPos(HLibWin, HWND_TOP, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOMOVE);

	result = mode;

	bFullScreen = !isWindowed;

	return true;
}

int LocalVideoMemoryDDraw()
{
	DWORD total = 0, free;
	typedef HRESULT (WINAPI *DIRECTDRAWCREATEEX)(GUID*, void**, REFIID, IUnknown*);
	if (HINSTANCE ddrawDLL = LoadLibrary("ddraw.dll"))
	{
		if (DIRECTDRAWCREATEEX ddrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress(ddrawDLL, "DirectDrawCreateEx"))
		{
			IDirectDraw7 *dd;
			if (SUCCEEDED(ddrawCreateEx(0, (void**)&dd, IID_IDirectDraw7, 0)))
			{
				DDSCAPS2 ddsc;
				
				ZeroMemory(&ddsc, sizeof(ddsc));
				ddsc.dwCaps = DDSCAPS_LOCALVIDMEM;
				dd->GetAvailableVidMem(&ddsc, &total, &free);
				dd->Release();
			}
		}
		FreeLibrary(ddrawDLL);
	}

	return total;
}

struct DDRAW_MATCH
{
	GUID guid;
	HMONITOR hMonitor;
	CHAR strDriverName[512];
	bool bFound;
};

BOOL WINAPI DDEnumCallbackEx( GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm )
{
	UNREFERENCED_PARAMETER( lpDriverDescription );

	DDRAW_MATCH* pDDMatch = ( DDRAW_MATCH* ) lpContext;
	if( pDDMatch->hMonitor == hm )
	{
		pDDMatch->bFound = true;
		strcpy_s( pDDMatch->strDriverName, 512, lpDriverName );
		memcpy( &pDDMatch->guid, lpGUID, sizeof( GUID ) );
	}
	return TRUE;
}

HRESULT GetDeviceIDFromHMonitor( HMONITOR hm, WCHAR* strDeviceID, int cchDeviceID )
{

	HINSTANCE hInstDDraw;

	hInstDDraw = LoadLibrary( "ddraw.dll" );
	if( hInstDDraw )
	{
		DDRAW_MATCH match;
		ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
		match.hMonitor = hm;

		LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = NULL;
		pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

		if( pDirectDrawEnumerateEx )
			pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );

		if( match.bFound )
		{
			LONG iDevice = 0;
			DISPLAY_DEVICEA dispdev;

			ZeroMemory( &dispdev, sizeof( dispdev ) );
			dispdev.cb = sizeof( dispdev );

			while( EnumDisplayDevicesA( NULL, iDevice, ( DISPLAY_DEVICEA* )&dispdev, 0 ) )
			{
				// Skip devices that are monitors that echo another display
				if( dispdev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER )
				{
					iDevice++;
					continue;
				}

				// Skip devices that aren't attached since they cause problems
				if( ( dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
				{
					iDevice++;
					continue;
				}

				if( _stricmp( match.strDriverName, dispdev.DeviceName ) == 0 )
				{
					MultiByteToWideChar( CP_ACP, 0, dispdev.DeviceID, -1, strDeviceID, cchDeviceID );
					return S_OK;
				}

				iDevice++;
			}
		}

		FreeLibrary( hInstDDraw );
	}

	return E_FAIL;
}

typedef BOOL ( WINAPI* PfnCoSetProxyBlanket )( IUnknown* pProxy, DWORD dwAuthnSvc, DWORD dwAuthzSvc,
											  OLECHAR* pServerPrincName, DWORD dwAuthnLevel, DWORD dwImpLevel,
											  RPC_AUTH_IDENTITY_HANDLE pAuthInfo, DWORD dwCapabilities );

int LocalVideoMemoryWMI()
{
	return 0; // doesn't work on laptops with two video cards. throws exception that interface isn't available

	HMONITOR hMonitor = r3dRenderer->pd3d->GetAdapterMonitor( 0 );

	WCHAR strInputDeviceID[512];
	GetDeviceIDFromHMonitor( hMonitor, strInputDeviceID, 512 );

	int res = 0 ;

	HRESULT hr;
	bool bGotMemory = false;
	HRESULT hrCoInitialize = S_OK;
	IWbemLocator* pIWbemLocator = NULL;
	IWbemServices* pIWbemServices = NULL;
	BSTR pNamespace = NULL;

	hrCoInitialize = CoInitialize( 0 );

	hr = CoCreateInstance(	CLSID_WbemLocator,
							NULL,
							CLSCTX_INPROC_SERVER,
							IID_IWbemLocator,
							( LPVOID* )&pIWbemLocator );

	if( FAILED( hr ) ) r3dOutToLog( "WMI: CoCreateInstance failed: 0x%0.8x\n", hr );

	if( SUCCEEDED( hr ) && pIWbemLocator )
	{
		// Using the locator, connect to WMI in the given namespace.
		pNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );

		hr = pIWbemLocator->ConnectServer( pNamespace, NULL, NULL, 0L,
			0L, NULL, NULL, &pIWbemServices );

		if( FAILED( hr ) ) r3dOutToLog( "WMI: pIWbemLocator->ConnectServer failed: 0x%0.8x\n", hr );

		if( SUCCEEDED( hr ) && pIWbemServices != NULL )
		{
			HINSTANCE hinstOle32 = NULL;

			hinstOle32 = LoadLibraryW( L"ole32.dll" );
			if( hinstOle32 )
			{
				PfnCoSetProxyBlanket pfnCoSetProxyBlanket = NULL;

				pfnCoSetProxyBlanket = ( PfnCoSetProxyBlanket )GetProcAddress( hinstOle32, "CoSetProxyBlanket" );
				if( pfnCoSetProxyBlanket != NULL )
				{
					// Switch security level to IMPERSONATE. 
					pfnCoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
						RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );
				}

				FreeLibrary( hinstOle32 );
			}

			IEnumWbemClassObject* pEnumVideoControllers = NULL;
			BSTR pClassName = NULL;

			pClassName = SysAllocString( L"Win32_VideoController" );

			hr = pIWbemServices->CreateInstanceEnum( pClassName, 0, NULL, &pEnumVideoControllers );

			if( FAILED( hr ) ) r3dOutToLog( "WMI: pIWbemServices->CreateInstanceEnum failed: 0x%0.8x\n", hr );

			if( SUCCEEDED( hr ) && pEnumVideoControllers )
			{
				IWbemClassObject* pVideoControllers[10] = {0};
				DWORD uReturned = 0;
				BSTR pPropName = NULL;

				// Get the first one in the list
				pEnumVideoControllers->Reset();
				hr = pEnumVideoControllers->Next( 5000,             // timeout in 5 seconds
					10,                  // return the first 10
					pVideoControllers,
					&uReturned );

				if( FAILED( hr ) ) r3dOutToLog( "WMI: pEnumVideoControllers->Next failed: 0x%0.8x\n", hr );
				if( uReturned == 0 ) r3dOutToLog( "WMI: pEnumVideoControllers uReturned == 0\n" );

				VARIANT var;
				if( SUCCEEDED( hr ) )
				{
					bool bFound = false;
					for( UINT iController = 0; iController < uReturned; iController++ )
					{
						pPropName = SysAllocString( L"PNPDeviceID" );
						hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );

						if( FAILED( hr ) )
							r3dOutToLog( "WMI: pVideoControllers[iController]->Get PNPDeviceID failed: 0x%0.8x\n", hr );

						if( SUCCEEDED( hr ) )
						{
							if( wcsstr( var.bstrVal, strInputDeviceID ) != 0 )
								bFound = true;
						}
						VariantClear( &var );
						if( pPropName ) SysFreeString( pPropName );

						if( bFound )
						{
							pPropName = SysAllocString( L"AdapterRAM" );
							hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );

							if( FAILED( hr ) )
								r3dOutToLog( "WMI: pVideoControllers[iController]->Get AdapterRAM failed: 0x%0.8x\n",	hr );

							if( SUCCEEDED( hr ) )
							{
								bGotMemory = true;
								res = var.ulVal;
							}
							VariantClear( &var );
							if( pPropName ) SysFreeString( pPropName );
							break;
						}
						SAFE_RELEASE( pVideoControllers[iController] );
					}
				}
			}

			if( pClassName )
				SysFreeString( pClassName );
			SAFE_RELEASE( pEnumVideoControllers );
		}

		if( pNamespace )
			SysFreeString( pNamespace );
		SAFE_RELEASE( pIWbemServices );
	}

	SAFE_RELEASE( pIWbemLocator );

	if( SUCCEEDED( hrCoInitialize ) )
		CoUninitialize();

	return res ;
}


void PrintAdapterId(FILE* file, const D3DADAPTER_IDENTIFIER9& ai, const D3DCAPS9& caps)
{
	fprintf(file, "Driver: %s\n",		ai.Driver);
	fprintf(file, "Description: %s\n",	ai.Description);
	fprintf(file, "DeviceName: %s\n",	ai.DeviceName);

	fprintf(file, "VendorId: %d\n", 	ai.VendorId);
	fprintf(file, "DeviceId: %d\n", 	ai.DeviceId);
	fprintf(file, "SubSysId: %d\n", 	ai.SubSysId);
	fprintf(file, "Revision: %d\n", 	ai.Revision);

	int vmemDDraw	= r_local_vmem_ddraw->GetInt() / ( 1024 * 1024 ) ;
	int vmemWMI		= r_local_vmem_wmi->GetInt() / ( 1024 * 1024 ) ;

	fprintf(file, "Local Video Memory(DDRAW): %d Mb\n", 	vmemDDraw );
	fprintf(file, "Local Video Memory(WMI): %d Mb\n", 		vmemWMI );

	fprintf(file, "VertexShaderVersion: %d.%d\n", D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion));
	fprintf(file, "PixelShaderVersion: %d.%d\n", D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));

	fprintf(file, "MaxVertexShaderConst: %d\n", 			caps.MaxVertexShaderConst);
	fprintf(file, "PixelShader1xMaxValue: %f\n", 			caps.PixelShader1xMaxValue);
	fprintf(file, "MaxVShaderInstructionsExecuted: %d\n", 	caps.MaxVShaderInstructionsExecuted);
	fprintf(file, "MaxPShaderInstructionsExecuted: %d\n", 	caps.MaxPShaderInstructionsExecuted);
	fprintf(file, "MaxVertexShader30InstructionSlots: %d\n",caps.MaxVertexShader30InstructionSlots);
	fprintf(file, "MaxPixelShader30InstructionSlots: %d\n", caps.MaxPixelShader30InstructionSlots);
	fprintf(file, "MaxTextureWidth: %d\n", caps.MaxTextureWidth);
	fprintf(file, "MaxTextureHeight: %d\n", caps.MaxTextureHeight);

	

	
	fflush(file);
}

void SaveCaps(FILE* file, const D3DCAPS9& caps)
{
	int version = 1; // D3DCAPS only

	fwrite(&version, sizeof(version), 1, file);
	fwrite(&caps, sizeof(caps), 1, file);
	fflush(file);
}

void SaveCaps(const D3DADAPTER_IDENTIFIER9& adapterId, const D3DCAPS9& caps)
{
	char path[MAX_PATH * 2];
	bool res = CreateConfigPath(path);

	if(res)
	{
		char binName[MAX_PATH * 2];
		char txtName[MAX_PATH * 2];
		sprintf(binName, "%s%s", path, "GPUCaps.bin");
		sprintf(txtName, "%s%s", path, "GPU.txt");

		FILE* bin = fopen(binName, "wb");
		FILE* txt = fopen(txtName, "w");

		if(bin)	SaveCaps(bin, caps);
		if(txt)	PrintAdapterId(txt, adapterId, caps);

		fclose(txt);
		fclose(bin);
	}	
}

static int DoesGPUSupportVertexFetch( IDirect3D9* d3d )
{
	D3DADAPTER_IDENTIFIER9 ident;
	D3D_V( d3d->GetAdapterIdentifier( 0, 0, &ident ) );

	// these are known not to have it

	switch( ident.VendorId )
	{
		// ATI/AMD
	case 0x1002:
		switch( ident.DeviceId )
		{
		case 0x71c1:
		case 0x71c2:
		case 0x71c3:
		case 0x71c6:
		case 0x71c7:
		case 0x4153:
		case 0x4173:
		case 0x4152:
		case 0x4172:
		case 0x5B60:
		case 0x5B63:
		case 0x5B73:
		case 0x5B70:
		case 0x7142:
		case 0x7146:
		case 0x7183:
		case 0x7187:
		case 0x71A3:
		case 0x7166:
		case 0x71A7:
		case 0x7162:
		case 0x7143:
		case 0x7163:
		case 0x715F:
		case 0x7147:
		case 0x71CE:
		case 0x7167:
		case 0x71EE:
		case 0x71E3:
		case 0x717F:
		case 0x71C0:
		case 0x71E0:
		case 0x71E2:
		case 0x7291:
		case 0x72B1:
		case 0x71E1:
		case 0x71E7:
		case 0x71E6:
		case 0x710A:
		case 0x7109:
		case 0x7100:
		case 0x7129:
		case 0x7120:
		case 0x712A:
		case 0x724B:
		case 0x7249:
		case 0x7269:
		case 0x726B:
		case 0x7240:
		case 0x7244:
		case 0x7260:
		case 0x7264:
		case 0x7280:
		case 0x72A0:
		case 0x7188:
			return 0 ;
			break;

		default:
			//------------------------------------------------------------------------
			// try through format caps.

			HRESULT hres = d3d->CheckDeviceFormat( 0, D3DDEVTYPE_HAL, r3dRenderLayer::DISPLAY_FORMAT_32, D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8 ); 

			if( hres == S_OK )
				return 1 ;
			else
				return 0 ;

			break ;


		}
		break ;
	}

	return 1 ;

}

int r3dRenderLayer::InitStereo()
{
#ifndef WO_SERVER 
	NvAPI_Status status ;
	status = NvAPI_Initialize() ;

	if( status == NVAPI_OK )
	{
		NVApiActive = 1 ;

		NvU8 isStereoEnabled ;
		status = NvAPI_Stereo_IsEnabled( &isStereoEnabled ) ;

		r3dOutToLog( "NVApi successfuly initialized.\n" ) ;

		// Stereo status report an error
		if ( status != NVAPI_OK )
		{
			// GeForce Stereoscopic 3D driver is not installed on the system
			r3dOutToLog( R3D_NVAPI "stereo is not available. Stereo driver not installed?\n" ) ;

		}
		// Stereo is available but not enabled, let's enable it
		else
		if( NVAPI_OK == status )
		{
			if( isStereoEnabled )
			{
				// stereo is enabled. going into stereo mode.
				NVApiStereoMode = 1 ;
				r3dOutToLog( R3D_NVAPI "stereo is available and enabled\n" ) ;
			}
			else
			{
				NVApiStereoMode = 0 ;
				r3dOutToLog( R3D_NVAPI "stereo is available but not enabled\n" ) ;
			}

			// set direct mode no matter what
			status = NvAPI_Stereo_SetDriverMode( NVAPI_STEREO_DRIVER_MODE_DIRECT ) ;
			//NvAPI_Stereo_SetSurfaceCreationMode(NVApiStereoHandle, NVAPI_STEREO_SURFACECREATEMODE_FORCEMONO);

			if( status != NVAPI_OK )
			{
				r3dOutToLog( R3D_NVAPI "couldn't set direct mode. Disabling stereo whatsoever." ) ;
				NVApiStereoMode = 0 ;
			}
		}

	}
	else
	{
		NvAPI_ShortString errorMessage = { 0 };
		NvAPI_GetErrorMessage(status, errorMessage);

		r3dOutToLog( "Could not initialize NVApi.\n" ) ;

		if( errorMessage[ 0 ] )
		{
			r3dOutToLog(  "Reason: \"%s\"\n", errorMessage ) ;
		}
		else
		{
			r3dOutToLog(  "\n", errorMessage ) ;
		}

		NVApiActive = 0 ;
		NVApiStereoMode = 0 ;
	}

	return NVApiActive && NVApiStereoMode ;
#else
	return 0;
#endif
}

bool r3dRenderLayer::DetectSLIorCrossfire() const
{
#ifndef WO_SERVER
	//	SLI/Crossire enabled only in fullscreen mode
	if (!r_fullscreen->GetBool())
	{
		gSLI_Crossfire_NumGPUs = 1;
		return false;
	}

	//	Check for SLI and crossfire
	int numATIGPUs = AtiMultiGPUAdapters();
	if (numATIGPUs > 1)
	{
		r3dOutToLog("ATI Crossfire is enabled with %i GPUs\n", numATIGPUs);
	}
	int numNVGPUs = 1;
	NV_GET_CURRENT_SLI_STATE ss;
	ZeroMemory(&ss, sizeof(ss));
 	NvAPI_Status rv = NvAPI_D3D_GetCurrentSLIState(pd3ddev, &ss);
	ss.version = NV_GET_CURRENT_SLI_STATE_VER;
	if (rv == NVAPI_OK)
	{
		if (ss.maxNumAFRGroups > 1)
		{
			numNVGPUs = ss.maxNumAFRGroups;
			r3dOutToLog("Nvidia SLI is enabled with %i GPUs\n", numNVGPUs);
		}
	}

	gSLI_Crossfire_NumGPUs = std::max(1, std::max(numATIGPUs, numNVGPUs));

	r_need_reset_exposure->SetInt(gSLI_Crossfire_NumGPUs * 2);

	return gSLI_Crossfire_NumGPUs > 1;
#else
	return false;
#endif
}

void r3dRenderLayer::SetEye( StereoEyeEnum eye )
{
#ifndef WO_SERVER 
	if( NVApiStereoMode )
	{
		NvAPI_Status status ;

		NV_STEREO_ACTIVE_EYE nvEye ;

		switch( eye )
		{
		case R3D_STEREO_EYE_LEFT:
			nvEye = r_3d_stereo_swap_eyes->GetInt() ? NVAPI_STEREO_EYE_RIGHT : NVAPI_STEREO_EYE_LEFT ;
			break ;
		case R3D_STEREO_EYE_RIGHT:
			nvEye = r_3d_stereo_swap_eyes->GetInt() ? NVAPI_STEREO_EYE_LEFT : NVAPI_STEREO_EYE_RIGHT ;
			break ;
		case R3D_STEREO_EYE_MONO:
			nvEye = NVAPI_STEREO_EYE_MONO ;
			break ;
		default:
			r3dError( "r3dRenderLayer::SetEye: your eye is not known to me." ) ;
			break ;
		}

		status = NvAPI_Stereo_SetActiveEye( NVApiStereoHandle, nvEye ) ;

		if( status != NVAPI_OK )
		{
			r3dOutToLog( R3D_NVAPI "failed to set eye!\n" ) ;
		}
		else
		{
			if (eye != R3D_STEREO_EYE_MONO)
				PresentEye = R3D_STEREO_EYE_LEFT;
		}
	}
#endif

	ActiveStereoEye = eye ;
}

void BrightnessContrastCallback( int oldI, float oldF )
{
	r3dRenderer->ApplyGammaRamp() ;
}

void StereoChangeCallback(int oldI, float oldF)
{

#ifndef WO_SERVER 

	if (!r3dRenderer->NVApiActive)
		return;

	if (r_3d_vision_direct->GetInt())
	{
		NvAPI_Status st = NvAPI_Stereo_Activate(r3dRenderer->NVApiStereoHandle);
		if (st != NVAPI_OK || !r_fullscreen->GetBool())
		{
			r3dOutToLog( R3D_NVAPI "error activating stereo\n" );
			r3dRenderer->NVApiStereoMode = 0;
		}
		else
		{
			r3dOutToLog( R3D_NVAPI "stereo successfully activated\n" );
			r3dRenderer->NVApiStereoMode = 1;
		}
	}
	else
	{
		NvAPI_Status st = NvAPI_Stereo_Deactivate(r3dRenderer->NVApiStereoHandle);
		if (st != NVAPI_OK)
		{
			r3dOutToLog( R3D_NVAPI "error deactivating stereo\n" );
		}
		else
		{
			r3dOutToLog( R3D_NVAPI "stereo successfully deactivated\n" );
			r3dRenderer->NVApiStereoMode = 0;
		}
	}
#endif

}

int r3dRenderLayer::SetMode(int XRes, int YRes, int BPP, int Flags, int RenderPath)
{
	HRESULT	res;
	int	isWindowed;
	int	DepthMode;

	RenderPath = RenderPath; //UNUSED

	// RTBuffer = NULL;

	DepthMode = D3DFMT_D24S8;

	if(Flags & R3DSetMode_Windowed)
		isWindowed = 1;
	else 
		isWindowed = 0;

	DeviceType = D3DDEVTYPE_HAL;
	CurrentBPP = BPP;

	//
	//FIXME: handle mode switching gracefully
	// and detect if we can render in window
	//

	CLOG_INDENT;

	// display adapter info
	D3DADAPTER_IDENTIFIER9 di;
	pd3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &di);
	{
		r3dOutToLog("Adapter: %s\n", di.Description); CLOG_INDENT;
		//r3dOutToLog("%s (0x%x 0x%x 0x%x 0x%x)\n", di.DeviceName, di.VendorId, di.DeviceId, di.SubSysId, di.Revision);

		DWORD dw1 = (di.DriverVersion.HighPart >> 16) & 0xFFFF;
		DWORD dw2 = (di.DriverVersion.HighPart >>  0) & 0xFFFF;
		DWORD dw3 = (di.DriverVersion.LowPart  >> 16) & 0xFFFF;
		DWORD dw4 = (di.DriverVersion.LowPart  >>  0) & 0xFFFF;
		r3dOutToLog("%s %d.%d.%d.%d\n", di.Driver, dw1, dw2, dw3, dw4);
	}


	// display monitor info
	MONITORINFOEX mi;
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	{
		HMONITOR hm = pd3d->GetAdapterMonitor(D3DADAPTER_DEFAULT);

		::GetMonitorInfo(hm, &mi);

		r3dOutToLog("Monitor: %s (%dx%d)\n", mi.szDevice, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);
	}


	ModesArray allModes;
	__EvaluateDisplayModes(pd3d, allModes);

	D3DDISPLAYMODE mode;

	if(!AdjustWindowSize( XRes, YRes, BPP, isWindowed, mode ))
	{
		r3dOutToLog("Adjust Window Size failed, xres=%d, yres=%d, bpp=%d, windowed=%d\n", XRes, YRes, BPP, isWindowed);
		return FALSE;
	}
	//	Modify mode for stereo
	FindCompatibleNVidiaStereoDisplayMode(mi.szDevice, mode, allModes);

	r3dOutToLog("r3dRenderLayer::SetMode(x=%d,y=%d,bpp=%d,windowed=%d,hz=%d)\n", mode.Width, mode.Height, BPP, Flags, mode.RefreshRate); 

	// Set up the structure used to create the D3DDevice. Since we are now
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed						= isWindowed;
	d3dpp.hDeviceWindow					= HLibWin;
	d3dpp.BackBufferWidth				= mode.Width;
	d3dpp.BackBufferHeight				= mode.Height;
	d3dpp.BackBufferFormat				= BACK_BUFFER_FORMAT;
	d3dpp.BackBufferCount				= r_vsync_enabled->GetInt()==0?1:2; // enable triple buffering if vsync is on. should improve performance slightly

	d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;

	d3dpp.Flags							= 0;
	d3dpp.FullScreen_RefreshRateInHz	= d3dpp.Windowed ? 0 : mode.RefreshRate;

	d3dpp.EnableAutoDepthStencil		= FALSE;
	d3dpp.AutoDepthStencilFormat		= (D3DFORMAT)DepthMode;
	MainDSSFormat = (D3DFORMAT)DepthMode ;

	//if(!d3dpp.Windowed)
	d3dpp.PresentationInterval = r_vsync_enabled->GetInt()==0?D3DPRESENT_INTERVAL_IMMEDIATE:D3DPRESENT_INTERVAL_ONE;

	D3DCAPS9 Caps;
	ZeroMemory( &Caps, sizeof(Caps) );
	pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, DeviceType, &Caps);

	SaveCaps(di, Caps);

	VS_Mode = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// Create the D3DDevice
	IDirect3DDevice9* dev ( NULL );

	UINT AdapterToUse = D3DADAPTER_DEFAULT;

#ifdef __NVPERFHUD_2 
	for (UINT Adapter=0; Adapter<pd3d->GetAdapterCount(); Adapter++)
	{ 
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res = pd3d->GetAdapterIdentifier(Adapter,0,&Identifier); 
		if(strstr(Identifier.Description,"PerfHUD") != 0) 
		{ 
			AdapterToUse = Adapter;
			DeviceType = D3DDEVTYPE_REF;
			break;
		} 
	}
#endif

	r3dOutToLog("Creating d3d device\n");
	res = pd3d->CreateDevice(AdapterToUse, DeviceType, HLibWin, VS_Mode, &d3dpp, &dev);
	
	if(res == D3DERR_INVALIDCALL ) r3dOutToLog ("Error: Invalid Call\n");
	if(res == D3DERR_NOTAVAILABLE ) r3dOutToLog ("Error: Not Available\n");
	if(res == D3DERR_OUTOFVIDEOMEMORY )
	{
		r3dOutToLog ("Error: Out of video memory\n");
		MessageBox(NULL, "Error: Failed to init D3D Device.\nPlease close any other applications and try again\n", "Error", MB_OK);
		exit(0);
	}

	if(res != D3D_OK)
	{
		r3dOutToLog("Failed to create d3d device (error=%d), falling back to software vertex processing\n", res);
		res = pd3d->CreateDevice(D3DADAPTER_DEFAULT, DeviceType, HLibWin, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dev);
		if(res != D3D_OK)
		{
			r3dOutToLog("Failed to init D3D Device with SW processing!\n");
			return FALSE;
		}
	}

	SetD3DDev( dev );

#ifndef WO_SERVER
	if( this->NVApiActive )
	{
		if( NvAPI_Stereo_CreateHandleFromIUnknown( dev, &NVApiStereoHandle ) != NVAPI_OK)
		{
			r3dOutToLog( R3D_NVAPI "failed to create stereo handle!\n" ) ;
			NVApiStereoMode = 0 ;
		}
		else
		{
			if (isWindowed)
			{
				NvU8 windowedStereo = 0;
				// Force mono for windowed mode
				//NvAPI_Stereo_IsWindowedModeSupported(&windowedStereo);
				if (windowedStereo == 0)
				{
					r3dOutToLog( R3D_NVAPI "windowed mode stereo is not supported\n" ) ;
					NVApiStereoMode = 0;
				}
			}
		}
		if (NVApiStereoMode)
		{
			r3dOutToLog( R3D_NVAPI "successfully created stereo handle\n" ) ;
			StereoChangeCallback(0, 0);
		}
	}
#endif

#ifdef FINAL_BUILD
	D3DPERF_SetOptions(1); // tell PIX to not profile our game
#endif

	if( dev->CreateQuery( D3DQUERYTYPE_OCCLUSION, NULL ) != D3D_OK )
	{
		r_use_oq->SetBool( false );
		SupportsOQ = 0;
	}
	else
	{
		SupportsOQ = 1;
	}
	
	if( dev->CreateQuery( D3DQUERYTYPE_EVENT, NULL ) != D3D_OK )
	{
		SupportsEventQueries = 0;
	}
	else
	{
		SupportsEventQueries = 1;
	}

	SupportsVertexTextureFetch = DoesGPUSupportVertexFetch( pd3d );

#ifndef WO_SERVER
#ifndef FINAL_BUILD
	if( r_disable_vfetch->GetInt() )
	{
		SupportsVertexTextureFetch = 0;
	}
#endif
#endif

	SupportsR32FBlending = 0 ;

	if( pd3d->CheckDeviceFormat(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
									r3dRenderLayer::DISPLAY_FORMAT_32, 
									D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
									D3DRTYPE_SURFACE, D3DFMT_R32F ) == D3D_OK )
	{
		SupportsR32FBlending = 1 ;
	}

	bool supportsTimeStamp		= dev->CreateQuery( D3DQUERYTYPE_TIMESTAMP, NULL ) == D3D_OK ;

	if( supportsTimeStamp)
	{
		r3dOutToLog( "GPU supports D3DQUERYTYPE_TIMESTAMP\n" ) ;
	}
	else
	{
		r3dOutToLog( "GPU DOES NOT support D3DQUERYTYPE_TIMESTAMP\n" ) ;
	}

	bool supportsTimeStampFreq	= dev->CreateQuery( D3DQUERYTYPE_TIMESTAMPFREQ, NULL ) == D3D_OK ;

	if( supportsTimeStampFreq )
	{
		r3dOutToLog( "GPU supports D3DQUERYTYPE_TIMESTAMPFREQ\n" ) ;
	}
	else
	{
		r3dOutToLog( "GPU DOES NOT support D3DQUERYTYPE_TIMESTAMPFREQ\n" ) ;
	}

	if(  supportsTimeStamp && 
		 supportsTimeStampFreq &&
		r_allow_gpu_timestamps->GetBool() )
	{
		SupportsStampQueries = 1 ;
	}
	else
	{
		SupportsStampQueries = 0 ;
#if !DISABLE_PROFILER
		if (gProfileD3DFromCommandLine)
		{
			r3dError("Can't profile GPU because GPU Stamp Queries are not supported on target GPU");
		}
#endif
	}

	bool supportsNullRT = pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, DeviceType, BACK_BUFFER_FORMAT, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, NULL_RT_FORMAT) == D3D_OK;
	SupportsNULLRenderTarget = supportsNullRT ? 1 : 0;

	bool supportsHWShadowMap = pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, DeviceType, BACK_BUFFER_FORMAT, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D24X8) == D3D_OK;
	SupportsHWShadowMapping = supportsHWShadowMap ? 1 : 0;

	bool supportsINTZ = pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, DeviceType, BACK_BUFFER_FORMAT, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, INTZ_FORMAT) == D3D_OK;
	SupportsINTZTextures = supportsINTZ ? 1 : 0;

	bool supportsRESZ = pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, DeviceType, BACK_BUFFER_FORMAT, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, RESZ_FORMAT) == D3D_OK;
	SupportsRESZResolve = supportsRESZ ? 1 : 0;

	r3dOutToLog( "D3D hack support = NullRT: %d, hwPCF: %d, IntZ: %d, ResZ: %d\n", supportsNullRT, supportsHWShadowMap, supportsINTZ, supportsRESZ ) ;

	//	Turn off hardware shadows for ATI hardware due to old driver errors
	D3DADAPTER_IDENTIFIER9 ai;
	pd3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &ai);
#if 0
	if (ai.VendorId == 0x1002)
	{
		SupportsHardwareShadowMapping = 0;
		SupportsINTZTextures = 0;
	}
#endif

#ifdef FINAL_BUILD
	if( r3dGetDeviceStrength() <= S_MEDIUM )
	{
		r_z_allow_prepass->SetInt( 1 ) ;
	}
	else
	{
		r_z_allow_prepass->SetInt( 0 ) ;
	}
#endif

	if( !Reset() )
		return FALSE ;
	ResetStats();


	// detect best texture format for our needs
	// FIXME: add few checks if we want to use 16bpp textures in 32bpp mode and vice versa

	int Want32bpp = (BPP == 32);

	// Texture format's listing
	/*r3dOutToLog ("Texture Formats availability\n");
	for(int i=0; i<_Num_ModeNames; i++)
	{
		if ( IsTextureFormatAvailable((D3DFORMAT)_ModeNames[i].ModeCode))
			r3dOutToLog ("Texture Format found ID %d: \t %s\n", _ModeNames[i].ModeCode,_ModeNames[i].ModeName);
	}*/

	/*r3dOutToLog ("Special Texture Formats availability\n");
	if ( IsTextureFormatAvailable(D3DFMT_UYVY))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_UYVY\n");
	if ( IsTextureFormatAvailable(D3DFMT_YUY2))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_YUY2\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT1))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT1\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT2))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT2\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT3))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT3\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT4))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT4\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT5))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT5\n");*/

	// __r3dShadeTexture[0] = LoadTexture("Data\\Shaders\\Texture\\Point1.bmp", D3DFMT_A8R8G8B8);	

	__r3dShadeTexture[SHADETEXT_BLUE] = LoadTexture("Data\\Shaders\\Texture\\blue.dds");
	__r3dShadeTexture[SHADETEXT_DEFAULT_NORMAL] = LoadTexture("Data\\Shaders\\Texture\\defaultnormal.dds");
	__r3dShadeTexture[SHADETEXT_WHITE] = LoadTexture("Data\\Shaders\\Texture\\White.dds");
	__r3dShadeTexture[SHADETEXT_BLACK] = LoadTexture("Data\\Shaders\\Texture\\Blank.dds");
	__r3dShadeTexture[SHADETEXT_MISSING] = LoadTexture("Data\\Shaders\\Texture\\Missing.dds");
	__r3dShadeTexture[SHADETEXT_NOISE] = LoadTexture("Data\\Shaders\\Texture\\Noise02.dds");
	__r3dShadeTexture[SHADETEXT_SUNFLAREMASK] = LoadTexture("Data\\Shaders\\Texture\\SunFlareMask01.dds");
	__r3dShadeTexture[SHADETEXT_DEFAULT_METALNESS] = LoadTexture("Data\\Shaders\\Texture\\DefaultMetalness.dds");
	__r3dShadeTexture[SHADETEXT_GREY] = LoadTexture("Data\\Shaders\\Texture\\grey.dds");

	__r3dShadeTexture[SHADETEXT_DEFAULT_DISTORTION] = LoadTexture("Data\\Shaders\\Texture\\DefaultDistortion.dds");

	__r3dShadeTexture[SHADETEXT_WINTER_DIFFUSE] = LoadTexture("Data\\Shaders\\Texture\\snow_diffuse.dds");
	__r3dShadeTexture[SHADETEXT_WINTER_SPECULAR] = LoadTexture("Data\\Shaders\\Texture\\snow_spec.dds");
	__r3dShadeTexture[SHADETEXT_WINTER_NORMAL] = LoadTexture("Data\\Shaders\\Texture\\snow_normal.dds");

#ifndef FINAL_BUILD
	__r3dShadeTexture[SHADETEXT_DENSITY_CHECKER] = LoadTexture("Data\\Shaders\\Texture\\Checker_big.dds");

	__r3dMipLevelTextures[0] = LoadTexture("Data\\Shaders\\Texture\\mipTexture13.dds");
	__r3dMipLevelTextures[1] = LoadTexture("Data\\Shaders\\Texture\\mipTexture12.dds");
	__r3dMipLevelTextures[2] = LoadTexture("Data\\Shaders\\Texture\\mipTexture11.dds");
	__r3dMipLevelTextures[3] = LoadTexture("Data\\Shaders\\Texture\\mipTexture10.dds");
	__r3dMipLevelTextures[4] = LoadTexture("Data\\Shaders\\Texture\\mipTexture9.dds");
#endif

	UpdateShaderProfiles();

	r3dIntegrityGuardian ig ;

	_r3dSystemFont = game_new CD3DFont(ig, "Verdana", 10, D3DFONT_BOLD | D3DFONT_FILTERED | D3DFONT_SKIPGLYPH);
	_r3dSystemFont->CreateSystemFont();

	r3dMaterialLibrary::Reset();

	//r_brightness->SetChangeCallback( BrightnessContrastCallback ) ;
	//r_contrast->SetChangeCallback( BrightnessContrastCallback ) ;
	r_3d_vision_direct->SetChangeCallback( &StereoChangeCallback );

	return TRUE;
}
/*
static WORD GetRampFor( WORD val )
{
	int ival = val * 256 ;
	ival = int( ival * r_contrast->GetFloat() * 2.0f + ( r_brightness->GetFloat() - 0.5f ) * 65536 ) ;

	return R3D_MAX( R3D_MIN( ival, 0xffff ), 0 ) ;
}*/

void r3dRenderLayer::ApplyGammaRamp()
{/*
	// we do manual brightness/ contrast
#if 0
	if( pd3ddev )
	{
		D3DGAMMARAMP ramp ;

		for( int i = 0, e = R3D_ARRAYSIZE( ramp.blue ) ; i < e ; i ++ )
		{
			ramp.green[ i ] = GetRampFor( i ) ;
			ramp.blue[ i ] = GetRampFor( i ) ;
			ramp.red[ i ] = GetRampFor( i ) ;
		}

		pd3ddev->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &ramp ) ;
	}
#endif*/
}

int r3dRenderLayer::IsTextureFormatAvailable(D3DFORMAT Fmt)
{
	HRESULT	res;

	res = pd3d->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		DeviceType,
		d3dpp.BackBufferFormat,
		0,	
		D3DRTYPE_TEXTURE,
		Fmt);

	return res == D3D_OK;
}

//////////////////////////////////////////////////////////////////////////

bool r3dRenderLayer::IsRenderTargetFormatAvailable(D3DFORMAT fmt)
{
	HRESULT	res = E_FAIL;

	res = pd3d->CheckDeviceFormat
	(
		D3DADAPTER_DEFAULT,
		DeviceType,
		d3dpp.BackBufferFormat,
		D3DUSAGE_RENDERTARGET,	
		D3DRTYPE_TEXTURE,
		fmt
	);

	return res == D3D_OK;
}

// some static variables

static	int		ZBuffer_Mode     = D3DZB_TRUE; //D3DZB_USEW; //
//FIX::RasterCaps member of this structure contains the D3DPRASTERCAPS_WBUFFER

//
// Renderer itself
//


r3dRenderLayer::r3dRenderLayer()
: ActiveStereoEye( R3D_STEREO_EYE_MONO )
, PresentEye( R3D_STEREO_EYE_MONO )
, DoubleDepthShadowPath( 0 )
{
	bInited = 0;
	pfnGetVertColor    = NULL;

	pdi = NULL; 
	pd3d= NULL; 
	SetD3DDev( NULL );

	ViewX = 0.f ;
	ViewY = 0.f ;
	ViewW = 0.f ;
	ViewH = 0.f ;

	ViewMinZ = 0.f ;
	ViewMaxZ = 1.f ;

	DeviceAvailable = 1 ;
	AllowNullViewport = 0 ;	

	Fog.Enabled        = 0;
	Fog.Start          = 0;
	Fog.End            = 10000;
	Fog.Color          = r3dColor(0, 0, 0);

	NearClip = 0.1f;
	FarClip  = 42000.0f;

	ShadowPassType	= SPT_ORTHO;
	ShadowSliceIndexBit = 0;

	CurrentPixelShaderID	= -1;
	CurrentVertexShaderID	= -1;

	SupportsOQ = 0;
	SupportsEventQueries = 0;
	SupportsStampQueries = 0;
	
	SupportsR32FBlending = 0;

	SupportsNULLRenderTarget = 0;
	SupportsHWShadowMapping = 0;
	SupportsRESZResolve = 0;
	SupportsINTZTextures = 0;

	NVApiActive = 0 ;
	NVApiStereoMode = 0 ;
	NVApiStereoHandle = 0 ;

	allowShaderLoading_ = false;
	InstanceCount = 1;
	ActiveCommandCount = 0;
	LastIssueActiveCommandsCount = 0;

	PendingQuery	= NULL ;
	DefaultCullMode	= D3DCULL_CCW;

	DistanceCullRefPos = r3dPoint3D( 0, 0, 0 );

	D3DXMatrixIdentity( &ViewMatrix );
	D3DXMatrixIdentity( &InvViewMatrix );
	D3DXMatrixIdentity( &ProjMatrix );
	D3DXMatrixIdentity( &InvProjMatrix );
	D3DXMatrixIdentity( &ViewProjMatrix );

	D3DXMatrixIdentity( &ViewMatrix_Localized );
	D3DXMatrixIdentity( &InvViewMatrix_Localized );	
	D3DXMatrixIdentity( &ViewProjMatrix_Localized );

	LocalizationPos = r3dPoint3D( 0, 0, 0 );

	SupportsVertexTextureFetch = 1;

	for(int i = 0; i<MAX_RENDER_TARGETS; ++i )
		RTs[i] = 0;

	MainDSSFormat = D3DFMT_D24S8 ;

	MainRT = 0;
	MainDSS = 0;

	DSS = 0;
	sysMemSurfaceForScreenShots = 0;
	sysMemTextureForFrontBufferCopy = 0;

	HLibWin = 0 ;

	ZRange = R3D_ZRANGE_WORLD ;

	FrustumType = R3D_FRUSTUM_NORMAL ;

	ZDir = ZDIR_NORMAL;

	ForceBlackPixelShader = 0;

	ForceBlackPixelShaderId = -1;

	LastThumbUpdated = 0;

	bFullScreen = 0;
}


r3dRenderLayer::~r3dRenderLayer()
{
}

static void d3dSetDefaultStates()
{
	IDirect3DDevice9* pd3ddev = r3dRenderer->pd3ddev;

	pd3ddev->SetRenderState(D3DRS_CLIPPING, 	TRUE);

	pd3ddev->SetRenderState(D3DRS_ZENABLE, 	ZBuffer_Mode);
	pd3ddev->SetRenderState(D3DRS_ZWRITEENABLE, 	TRUE);

	pd3ddev->SetRenderState(D3DRS_SHADEMODE,    	D3DSHADE_GOURAUD);
	pd3ddev->SetRenderState(D3DRS_SPECULARENABLE, 0);

	pd3ddev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );
	pd3ddev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	pd3ddev->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_COLOR2 );
	pd3ddev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1 );
	pd3ddev->SetRenderState(D3DRS_COLORVERTEX, TRUE);

	pd3ddev->SetRenderState( D3DRS_LIGHTING, 0);


	pd3ddev->SetRenderState(D3DRS_FILLMODE, 	D3DFILL_SOLID);

	pd3ddev->SetRenderState(D3DRS_FOGENABLE,      FALSE);
	pd3ddev->SetRenderState(D3DRS_FOGCOLOR,       0x00FFFFFF);

	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );
	
	pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	D3DBLEND_ONE);
	pd3ddev->SetRenderState(D3DRS_DESTBLEND,   	D3DBLEND_ONE);

	pd3ddev->SetRenderState(D3DRS_LIGHTING,     	FALSE);

	for (int i=0;i<8;i++)
	{
		pd3ddev->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pd3ddev->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3ddev->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		CurrentTexID[i] = (IDirect3DBaseTexture9*)-1;
	}

	// setup default material
	{
		D3DMATERIAL9 mtrl;
		ZeroMemory(&mtrl, sizeof(mtrl));
		mtrl.Diffuse.r = 1.0f;
		mtrl.Diffuse.g = 1.0f;
		mtrl.Diffuse.b = 1.0f;
		mtrl.Diffuse.a = 1.0f;
		mtrl.Ambient.r = 0.0f;
		mtrl.Ambient.g = 0.0f;
		mtrl.Ambient.b = 0.0f;
		mtrl.Ambient.a = 1.0f;
		pd3ddev->SetMaterial(&mtrl);
	}

	d3dc._SetDecl(R3D_DEBUG_VERTEX::getDecl());
	return;
}

bool r3dRenderLayer::Reset()
{
	bInited       = 1;

	Stats.ResetVMemStats() ;

	//  ZBuffer_Mode  = D3DZB_TRUE;
	vtxArray2D					= r3dCreateVertexArray(4096, sizeof(R3D_SCREEN_VERTEX), R3D_SCREEN_VERTEX::getDecl(), r3dVertexArray::TRIANGLE_LIST );
	vtxArrayMinimalist2D_Lines	= r3dCreateVertexArray(4096, sizeof(R3D_MINIMALIST_VERTEX_2D), R3D_MINIMALIST_VERTEX_2D::getDecl(), r3dVertexArray::LINE_STRIP );
	vtxArray3D			= r3dCreateVertexArray(4096, sizeof(R3D_DEBUG_VERTEX), R3D_DEBUG_VERTEX::getDecl(), r3dVertexArray::TRIANGLE_LIST );
	vtxArray3D_Lines	= r3dCreateVertexArray(4096, sizeof(R3D_DEBUG_VERTEX), R3D_DEBUG_VERTEX::getDecl(), r3dVertexArray::LINE_LIST  );

	AmbientColor = r3dColor(0,0,0);

	pd3ddev->GetDeviceCaps(&d3dCaps);

	// this is definetelly a DX9 card - disable vertex fetch
	// because we use some formats which don't vfetch well with these cards
	if( d3dCaps.MaxTextureWidth < 8192 )
	{
		SupportsVertexTextureFetch = 0;
	}

	DWORD NEEDED_PSVER = D3DPS_VERSION( 3, 0 ) ;
	DWORD NEEDED_VSVER = D3DVS_VERSION( 3, 0 ) ;

	if( d3dCaps.PixelShaderVersion < NEEDED_PSVER ||
		d3dCaps.VertexShaderVersion < NEEDED_VSVER )
	{
		MessageBox( NULL, "Our apologies, but you need a video card that supports shader model 3 or higher to run this game.", "Apologies", MB_OK );
		return false ;
	}

	if( d3dCaps.NumSimultaneousRTs < 3 )
	{
		MessageBox( NULL, "Our apologies, but you need a video card that supports at least 3 simultaneous Render Targets to run this game.", "Apologies", MB_OK );
		return false ;
	}

	{
		if( r_target_shader_model->GetInt() == 0 )
		{
			r_target_shader_model->SetInt( 3 ) ;
		}
		r_card_shader_model->SetInt( 3 ) ;
	}
	r_trees_noninst_render->SetBool( false );

	r3dPrintVMem() ;

	//
	// Init default render states
	//
	d3dSetDefaultStates();

	UpdateDimmensions(); 

	AquireMainRT() ;

	RTs[ 0 ] = MainRT ;
	DSS = MainDSS ;

	return true;
}

//--------------------------------------------------------------------------------------------------------
void r3dRenderLayer::PushCamera( const r3dCamera &cam )
{	
	m_CameraStack.PushBack( cam );
	SetCamera( cam, false );
}



//--------------------------------------------------------------------------------------------------------
void r3dRenderLayer::PopCamera()
{
	if ( m_CameraStack.Count() <= 1 )
	{
		assert( false );
		return;
	}

	m_CameraStack.PopBack();
	SetCamera( m_CameraStack.GetLast(), false );
}

//------------------------------------------------------------------------

void r3dRenderLayer::BuildViewMtx( const r3dCamera &Cam, D3DXMATRIX* oMtx, D3DXMATRIX* oLocalizedMtx )
{
	D3DXVECTOR3 VEye, VAt, VUP;

	VEye.x = Cam.X;
	VEye.y = Cam.Y;
	VEye.z = Cam.Z;
	VAt.x  = Cam.X + Cam.vPointTo.X*10;
	VAt.y  = Cam.Y + Cam.vPointTo.Y*10;
	VAt.z  = Cam.Z + Cam.vPointTo.Z*10;

	VUP.x = Cam.vUP.x;
	VUP.y = Cam.vUP.y;
	VUP.z = Cam.vUP.z;

	D3DXMatrixLookAtLH(oMtx, &VEye, &VAt, &VUP );

	if( oLocalizedMtx )
	{
		VEye.x -= LocalizationPos.x;
		VEye.y -= LocalizationPos.y;
		VEye.z -= LocalizationPos.z;
		VAt.x  -= LocalizationPos.x;
		VAt.y  -= LocalizationPos.y;
		VAt.z  -= LocalizationPos.z;

		D3DXMatrixLookAtLH( oLocalizedMtx, &VEye, &VAt, &VUP );
	}
}

//------------------------------------------------------------------------

void r3dRenderLayer::BuildPerspectiveMtx( const r3dCamera &Cam, D3DXMATRIX* oMtx )
{
	D3DVIEWPORT9 vp;
	GetViewport(&vp);

	FLOAT fAspect = (((FLOAT)vp.Width) / (FLOAT)vp.Height);
	fAspect = Cam.Aspect; //1.777f ;//1.33333f;

	float nearClip = Cam.NearClip;
	float farClip = Cam.FarClip;

	BuildMatrixPerspectiveFovLH(oMtx, (Cam.FOV * D3DX_PI) / 180.0f, fAspect, nearClip, farClip);

#if !defined(FINAL_BUILD)
	oMtx->_11 *= r_hackerzoom->GetFloat();
	oMtx->_22 *= r_hackerzoom->GetFloat();
#endif
}

//--------------------------------------------------------------------------------------------------------
void r3dRenderLayer::SetCamera( const r3dCamera &Cam, bool asDistanceCullReference )
{
	if( asDistanceCullReference )
	{
		DistanceCullRefPos = Cam;
		LocalizationPos = Cam;
	}

	// hardware vp
	D3DXMATRIX 	WorldMatrix;

	D3DXMatrixIdentity(&WorldMatrix);
	D3DXMatrixIdentity(&ProjMatrix);

	float nearClip = Cam.NearClip;
	float farClip = Cam.FarClip;

	switch( Cam.ProjectionType )
	{
	case r3dCamera::PROJTYPE_ORTHO:
		BuildMatrixOrthoLH(&ProjMatrix, Cam.Width, Cam.Height, nearClip, farClip );
		// for better precision
		r3dOrthoProjInverse( InvProjMatrix, ProjMatrix );
		break;
	case r3dCamera::PROJTYPE_PRESPECTIVE:
		BuildPerspectiveMtx( Cam, &ProjMatrix) ;
		// for better precision
		r3dPerspProjInverse( InvProjMatrix, ProjMatrix );
		break;
	case r3dCamera::PROJTYPE_CUSTOM:
		D3DXMatrixIdentity( &ProjMatrix );
		D3DXMatrixIdentity( &InvProjMatrix );
		break;
	}

	// view & inverse view matrices
	{
		BuildViewMtx( Cam, &ViewMatrix, &ViewMatrix_Localized );

		D3DXMatrixInverse( &InvViewMatrix, NULL, &ViewMatrix );
		D3DXMatrixInverse( &InvViewMatrix_Localized, NULL, &ViewMatrix_Localized );
	}

	ViewProjMatrix = ViewMatrix * ProjMatrix;
	ViewProjMatrix_Localized = ViewMatrix_Localized * ProjMatrix;

	NearClip	= Cam.NearClip;
	FarClip		= Cam.FarClip;

	if( DeviceAvailable )
	{
#ifndef FINAL_BUILD
		pd3ddev->SetTransform(D3DTS_WORLD,      &WorldMatrix);
		pd3ddev->SetTransform(D3DTS_PROJECTION, &ProjMatrix);
		pd3ddev->SetTransform(D3DTS_VIEW,       &ViewMatrix);
#endif
	}

	float n = 0.0f;
	float f = 1.0f;

	if( ZDir == ZDIR_INVERSED && Cam.ProjectionType == r3dCamera::PROJTYPE_PRESPECTIVE )
	{
		R3D_SWAP( n, f );
	}

	D3DXMATRIX mat;

	if( Cam.ProjectionType == r3dCamera::PROJTYPE_CUSTOM )
	{
		float R = R3D_MAX( n, f );

		FrustumCorners[0] = D3DXVECTOR3(-R, -R,  n); // xyz
		FrustumCorners[1] = D3DXVECTOR3( R, -R,  n); // Xyz
		FrustumCorners[2] = D3DXVECTOR3(-R,  R,  n); // xYz
		FrustumCorners[3] = D3DXVECTOR3( R,  R,  n); // XYz
		FrustumCorners[4] = D3DXVECTOR3(-R, -R,  f); // xyZ
		FrustumCorners[5] = D3DXVECTOR3( R, -R,  f); // XyZ
		FrustumCorners[6] = D3DXVECTOR3(-R,  R,  f); // xYZ
		FrustumCorners[7] = D3DXVECTOR3( R,  R,  f); // XYZ

		mat = InvViewMatrix;
	}
	else
	{
		FrustumCorners[0] = D3DXVECTOR3(-1.0f, -1.0f,  n); // xyz
		FrustumCorners[1] = D3DXVECTOR3( 1.0f, -1.0f,  n); // Xyz
		FrustumCorners[2] = D3DXVECTOR3(-1.0f,  1.0f,  n); // xYz
		FrustumCorners[3] = D3DXVECTOR3( 1.0f,  1.0f,  n); // XYz
		FrustumCorners[4] = D3DXVECTOR3(-1.0f, -1.0f,  f); // xyZ
		FrustumCorners[5] = D3DXVECTOR3( 1.0f, -1.0f,  f); // XyZ
		FrustumCorners[6] = D3DXVECTOR3(-1.0f,  1.0f,  f); // xYZ
		FrustumCorners[7] = D3DXVECTOR3( 1.0f,  1.0f,  f); // XYZ

		D3DXMatrixMultiply( &mat, &InvProjMatrix, &InvViewMatrix );
	}

	for( int i = 0; i < 8; i++ )
		D3DXVec3TransformCoord( &FrustumCorners[i], &FrustumCorners[i], &mat );

	D3DXPlaneFromPoints( &FrustumPlanes[0], &FrustumCorners[0], &FrustumCorners[1], &FrustumCorners[2] ); // Near
	D3DXPlaneFromPoints( &FrustumPlanes[1], &FrustumCorners[6], &FrustumCorners[7], &FrustumCorners[5] ); // Far
	D3DXPlaneFromPoints( &FrustumPlanes[2], &FrustumCorners[2], &FrustumCorners[6], &FrustumCorners[4] ); // Left
	D3DXPlaneFromPoints( &FrustumPlanes[3], &FrustumCorners[7], &FrustumCorners[3], &FrustumCorners[5] ); // Right
	D3DXPlaneFromPoints( &FrustumPlanes[4], &FrustumCorners[2], &FrustumCorners[3], &FrustumCorners[6] ); // Top
	D3DXPlaneFromPoints( &FrustumPlanes[5], &FrustumCorners[1], &FrustumCorners[0], &FrustumCorners[4] ); // Bottom

	CameraPosition = Cam;

	if ( m_CameraStack.Count() )
	{
		m_CameraStack[ m_CameraStack.Count() - 1 ] = Cam;
	}
	else
	{
		m_CameraStack.PushBack( Cam );
	}
}

void r3dRenderLayer::SetCameraEx(const D3DXMATRIX& view, const D3DXMATRIX& proj, float nearD, float farD, bool asDistanceCullReference)
{
	D3DXMATRIX 	WorldMatrix;
	D3DXMatrixIdentity(&WorldMatrix);

	ViewMatrix = view;
	ProjMatrix = proj;

	ViewProjMatrix = ViewMatrix * ProjMatrix ;

	NearClip	= nearD;
	FarClip		= farD;

#ifndef FINAL_BUILD
	pd3ddev->SetTransform(D3DTS_WORLD,      &WorldMatrix);
	pd3ddev->SetTransform(D3DTS_PROJECTION, &ProjMatrix);
	pd3ddev->SetTransform(D3DTS_VIEW,       &ViewMatrix);
#endif

	D3DXMATRIX mat;

	D3DXMatrixInverse( &InvViewMatrix, NULL, &ViewMatrix );

	if( asDistanceCullReference )
	{
		DistanceCullRefPos = r3dPoint3D(	InvViewMatrix._41, 
											InvViewMatrix._42,
											InvViewMatrix._43 );

		LocalizationPos = DistanceCullRefPos;
	}

	//------------------------------------------------------------------------

	ViewMatrix_Localized = ViewMatrix;

	r3dPoint3D camPos( InvViewMatrix._41, InvViewMatrix._42, InvViewMatrix._43 );

	D3DXVECTOR3 locaOffset( camPos.x - LocalizationPos.x, camPos.y - LocalizationPos.y, camPos.z - LocalizationPos.z );

	ViewMatrix_Localized._41 = -(	locaOffset.x * ViewMatrix_Localized._11 + 
									locaOffset.y * ViewMatrix_Localized._21 + 
									locaOffset.z * ViewMatrix_Localized._31 );
	ViewMatrix_Localized._42 = -(	locaOffset.x * ViewMatrix_Localized._12 + 
									locaOffset.y * ViewMatrix_Localized._22 + 
									locaOffset.z * ViewMatrix_Localized._32 );
	ViewMatrix_Localized._43 = -(	locaOffset.x * ViewMatrix_Localized._13 + 
									locaOffset.y * ViewMatrix_Localized._23 + 
									locaOffset.z * ViewMatrix_Localized._33 );

	D3DXMatrixInverse( &InvViewMatrix_Localized, NULL, &ViewMatrix_Localized );

	ViewProjMatrix_Localized = ViewMatrix_Localized * ProjMatrix;

	//------------------------------------------------------------------------

	D3DXMatrixInverse( &InvProjMatrix, NULL, &ProjMatrix );

	D3DXMatrixMultiply( &mat, &InvProjMatrix, &InvViewMatrix );

	float n = 0.0f;
	float f = 1.0f;

	if( ZDir == ZDIR_INVERSED )
	{
		R3D_SWAP( n, f );
	}

	FrustumCorners[0] = D3DXVECTOR3(-1.0f, -1.0f,  n); // xyz
	FrustumCorners[1] = D3DXVECTOR3( 1.0f, -1.0f,  n); // Xyz
	FrustumCorners[2] = D3DXVECTOR3(-1.0f,  1.0f,  n); // xYz
	FrustumCorners[3] = D3DXVECTOR3( 1.0f,  1.0f,  n); // XYz
	FrustumCorners[4] = D3DXVECTOR3(-1.0f, -1.0f,  f); // xyZ
	FrustumCorners[5] = D3DXVECTOR3( 1.0f, -1.0f,  f); // XyZ
	FrustumCorners[6] = D3DXVECTOR3(-1.0f,  1.0f,  f); // xYZ
	FrustumCorners[7] = D3DXVECTOR3( 1.0f,  1.0f,  f); // XYZ

	for( int i = 0; i < 8; i++ )
		D3DXVec3TransformCoord( &FrustumCorners[i], &FrustumCorners[i], &mat );

	D3DXPlaneFromPoints( &FrustumPlanes[0], &FrustumCorners[0], &FrustumCorners[1], &FrustumCorners[2] ); // Near
	D3DXPlaneFromPoints( &FrustumPlanes[1], &FrustumCorners[6], &FrustumCorners[7], &FrustumCorners[5] ); // Far
	D3DXPlaneFromPoints( &FrustumPlanes[2], &FrustumCorners[2], &FrustumCorners[6], &FrustumCorners[4] ); // Left
	D3DXPlaneFromPoints( &FrustumPlanes[3], &FrustumCorners[7], &FrustumCorners[3], &FrustumCorners[5] ); // Right
	D3DXPlaneFromPoints( &FrustumPlanes[4], &FrustumCorners[2], &FrustumCorners[3], &FrustumCorners[6] ); // Top
	D3DXPlaneFromPoints( &FrustumPlanes[5], &FrustumCorners[1], &FrustumCorners[0], &FrustumCorners[4] ); // Bottom

	CameraPosition = camPos;
}

void r3dRenderLayer::BuildMatrixPerspectiveFovLH( D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf )
{
	if( ZDir == ZDIR_INVERSED )
	{
		R3D_SWAP( zn, zf );
	}

	D3DXMatrixPerspectiveFovLH( pOut, fovy, Aspect, zn, zf );
}

void r3dRenderLayer::BuildMatrixPerspectiveOffCenterLH( D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf )
{
	if( ZDir == ZDIR_INVERSED )
	{
		R3D_SWAP( zn, zf );
	}

	D3DXMatrixPerspectiveOffCenterLH( pOut, l, r, b, t, zn, zf );
}

void r3dRenderLayer::BuildMatrixOrthoLH( D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf )
{
	if( ZDir == ZDIR_INVERSED )
	{
		R3D_SWAP( zn, zf );
	}

	D3DXMatrixOrthoLH( pOut, w, h, zn, zf );
}

void r3dRenderLayer::BuildMatrixOrthoOffCenterLH( D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf )
{
	if( ZDir == ZDIR_INVERSED )
	{
		R3D_SWAP( zn, zf );
	}

	D3DXMatrixOrthoOffCenterLH( pOut, l, r, b, t, zn, zf );
}

void r3dRenderLayer::SetZRange( r3dZRange range )
{
	ZRange = range ;

	DoSetViewport( ViewX, ViewY, ViewW, ViewH ) ;
}

void r3dRenderLayer::DoSetViewport ( float X, float Y, float W, float H )
{
	D3DVIEWPORT9 vp;
	vp.X = (DWORD)X;
	vp.Y = (DWORD)Y;
	vp.Width = (DWORD)W;
	vp.Height = (DWORD)H;

	SetViewportZRange( &vp ) ;

	SetViewportInternal(&vp);
}

void r3dRenderLayer::SetViewportInternal ( const D3DVIEWPORT9* viewport )
{
	Flush();

	ViewX = (float)viewport->X ;
	ViewY = (float)viewport->Y ;
	ViewW = (float)viewport->Width ;
	ViewH = (float)viewport->Height ;

	ViewMinZ = viewport->MinZ ;
	ViewMaxZ = viewport->MaxZ ;

#ifndef FINAL_BUILD
	if( AllowNullViewport && r_null_viewport->GetInt() )
	{
		D3DVIEWPORT9 vp = *viewport ;

		vp.X = 0 ;
		vp.Y = 0 ;
		vp.Width  = 1 ;
		vp.Height = 1 ;

#undef SetViewport
		D3D_V( pd3ddev->SetViewport( &vp ) );
		return ;
#define SetViewport DoSetViewport	

	}
	else
	{
		if( r_3d_stereo_emu->GetInt() )
		{
			IDirect3DSurface9* rt ;

			GetRT( 0, &rt ) ;

			if( rt )
			{
				rt->Release() ;
			}

			if( rt == MainRT && ActiveStereoEye != R3D_STEREO_EYE_MONO )
			{
				D3DVIEWPORT9 vp = *viewport ;

				vp.Width /= 2 ;

				vp.X /= 2 ;

				if( ( ActiveStereoEye == R3D_STEREO_EYE_RIGHT ) ^!!r_3d_stereo_swap_eyes->GetInt() )
				{
					vp.X += (int)ScreenW2 ;
				}

#undef SetViewport
				D3D_V( pd3ddev->SetViewport( &vp ) );
#define SetViewport DoSetViewport

				return ;
			}
		}
	}
#endif

#undef SetViewport
	D3D_V( pd3ddev->SetViewport( viewport ) );
#define SetViewport DoSetViewport	
}

void r3dRenderLayer::SetViewportZRange( D3DVIEWPORT9* oViewport )
{
	switch( ZRange )
	{
	case R3D_ZRANGE_FIRST_PERSON:
		if( ZDir == ZDIR_NORMAL )
		{
			oViewport->MinZ = 0.f;
			oViewport->MaxZ = r_first_person_z_end->GetFloat();
		}
		else
		{
			oViewport->MinZ = 1.0f - r_first_person_z_end->GetFloat();
			oViewport->MaxZ = 1.0f;
		}
		break ;
	case R3D_ZRANGE_WORLD:
		if( ZDir == ZDIR_NORMAL )
		{
			oViewport->MinZ = r_first_person_z_end->GetFloat();
			oViewport->MaxZ = 1.0f;
		}
		else
		{
			oViewport->MinZ = 0.f;
			oViewport->MaxZ = 1.0f - r_first_person_z_end->GetFloat();
		}
		break ;
	case R3D_ZRANGE_WHOLE:
		oViewport->MinZ = 0.f;
		oViewport->MaxZ = 1.0f;
		break ;
	case R3D_ZRANGE_ANTICHEAT:
		if( ZDir == ZDIR_NORMAL )
		{
			oViewport->MinZ = r_wall_hack_anti_cheat_z;
			oViewport->MaxZ = 1.0f;
		}
		else
		{
			oViewport->MinZ = 0.0f;
			oViewport->MaxZ = 1.0f - r_wall_hack_anti_cheat_z;
		}
		break ;
	}
}

void r3dRenderLayer::DoGetViewport( D3DVIEWPORT9* oViewport )
{
	oViewport->X = (DWORD) ViewX ;
	oViewport->Y = (DWORD) ViewY ;

	oViewport->Width = (DWORD)ViewW ;
	oViewport->Height = (DWORD)ViewH ;

	oViewport->MinZ = ViewMinZ ;
	oViewport->MaxZ = ViewMaxZ ;
}

void r3dRenderLayer::ResetViewport()
{
	Flush();

	ViewX = 0; 
	ViewY = 0; 
	ViewW = ScreenW; 
	ViewH = ScreenH;

	D3DVIEWPORT9 view;
	view.X      = (DWORD)ViewX;
	view.Y      = (DWORD)ViewY;
	view.Width  = (DWORD)ViewW;
	view.Height = (DWORD)ViewH;

	SetViewportZRange( &view ) ;

#ifndef FINAL_BUILD
	if( AllowNullViewport && r_null_viewport->GetInt() )
	{
		view.X = 0 ;
		view.Y = 0 ;
		view.Width  = 0 ;
		view.Height = 0 ;
	}
#endif

	SetViewportInternal( &view ) ;
}


static	int	_bVSActive = 0;
extern	void	fog_UpdateVS(int bVShaderActive)
{
	if(bVShaderActive != -1)
		_bVSActive = bVShaderActive;

	if(_bVSActive) {
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_FOGTABLEMODE,   D3DFOG_NONE);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR);
	} else {
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_FOGVERTEXMODE,  D3DFOG_NONE);
	}

}

void r3dRenderLayer::SetFog(int fogEnabled)
{
	Fog.Enabled = fogEnabled;

	float Start = 0;
	float End = Fog.End; // float(FarClip);

	if (Fog.Enabled)
	{
		pd3ddev->SetRenderState (D3DRS_FOGENABLE, 0);
		pd3ddev->SetRenderState (D3DRS_FOGCOLOR,  Fog.Color.GetPacked());
		pd3ddev->SetRenderState (D3DRS_FOGSTART, *(DWORD *)(&Start));
		pd3ddev->SetRenderState (D3DRS_FOGEND,   *(DWORD *)(&End));

		fog_UpdateVS(-1);
	}
	else
		pd3ddev->SetRenderState (D3DRS_FOGENABLE, 0);
}

void
r3dRenderLayer::StretchRect( r3dScreenBuffer* Source, r3dScreenBuffer* Target, int Filter /*= 0*/ )
{
	// if this fails, IMPLEMENT ME
	Source->AsTex2D() ;
	Target->AsTex2D();

	D3D_V( pd3ddev->StretchRect( Source->GetTex2DSurface(), NULL, Target->GetTex2DSurface(), NULL, Filter ? D3DTEXF_LINEAR : D3DTEXF_NONE ) );
}

void r3dRenderLayer :: ResetStats()
{
	Stats.Reset();
}



bool g_bStartFrame = false;

void r3dRenderLayer::StartFrame()
{
	if( DeviceAvailable )
	{
		d3dc.Reset();
		CurrentCullMode = D3DCULL( -1 );

		DSS = 0 ;
		SetDSS( MainDSS ) ;

		RTs[ 0 ] = 0 ;
		SetRT( 0, MainRT ) ;

		ResetTextureCache();

		pd3ddev->BeginScene();

		ZDir = ZDirEnum(-1);

		if( r_inverse_zbuffer->GetInt() )
		{
			SetZDir( ZDIR_INVERSED );
		}
		else
		{
			SetZDir( ZDIR_NORMAL );
		}

		PresentEye = R3D_STEREO_EYE_MONO;
		SetEye( R3D_STEREO_EYE_MONO ) ;

#ifndef WO_SERVER 
/*
		if( NVApiStereoHandle )
		{
			float separation, convergence ;

			NvAPI_Status status ;

			status = NvAPI_Stereo_GetSeparation( NVApiStereoHandle, &separation ) ;
			if( status != NVAPI_OK )
				r3dOutToLog( R3D_NVAPI "NvAPI_Stereo_GetSeparation failed!" ) ;

			status = NvAPI_Stereo_GetConvergence( NVApiStereoHandle, &convergence ) ;
			if( status != NVAPI_OK )
				r3dOutToLog( R3D_NVAPI "NvAPI_Stereo_GetConvergence failed!" ) ;

 			r_3d_stereo_separation->SetFloat( separation ) ;
 			r_3d_stereo_convergance->SetFloat( convergence ) ;
		}
*/
#endif
	}

	g_bStartFrame = true;
}

void r3dRenderLayer::EndFrame()
{
	if( DeviceAvailable )
	{
		IssuePendingQuery();

		int thumbCount = ThumbnailTextures.Count();
		int thumbsToUpdate = R3D_MIN( 512, thumbCount );

		for( int i = 0; i < thumbsToUpdate ; i ++, LastThumbUpdated ++ )
		{
			LastThumbUpdated %= thumbCount;
			ThumbnailTextures[ LastThumbUpdated ]->Update();
		}

		ProcessDeletedTextures();

		D3D_V( pd3ddev->EndScene() );
	}

	g_bStartFrame = false;

	if( gWatchDogEnable )
	{
		if( IsDeviceLost() )
		{
			// this can hold stuff, so reset it by ourselves
			r3dResetWatchDog();
		}
		else
		{
			float passed = r3dGetTime() - gWatchDogLastReset ;
			if( passed > gWatchDogTimeOut )
			{
				r3dError( "Watchdog timeout for %s: %.2f over %.2f!\n", gWatchDogItemName ? gWatchDogItemName : "UNKNOWN", passed, gWatchDogTimeOut ) ;
			}
		}
	}
}

void r3dRenderLayer::GetBackBufferViewport( float* X, float* Y, float* Width, float* Height )
{
	float W = (float)r3dRenderer->d3dpp.BackBufferWidth;
	float H = (float)r3dRenderer->d3dpp.BackBufferHeight;

	*X		= W * r_screen_offset_x->GetFloat();
	*Y		= H * r_screen_offset_y->GetFloat();
	*Width	= W * r_screen_scale_x->GetFloat();
	*Height = H * r_screen_scale_y->GetFloat();
}

void r3dRenderLayer::SetBackBufferViewport()
{
	float X, Y, Width, Height;

	GetBackBufferViewport( &X, &Y, &Width, &Height );

	SetViewport( X, Y, Width, Height );
}

void r3dRenderLayer::StartRender(int bClear, bool clearToWhite)
{
	static int ClearFlag = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL;
	static int _FlagSet = 0;

	if ( !_FlagSet)
	{
		if (CurrentBPP == 16 )
			ClearFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
		else
			ClearFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

		_FlagSet = 1;
	}

	if (bClear)
		D3D_V( pd3ddev->Clear(0, NULL, ClearFlag, clearToWhite?D3DCOLOR_ARGB(255,255,255,255):Fog.Color.GetPacked(), r3dRenderer->GetClearZValue(), 0 ) );


	D3D_V( d3dc._SetVertexShader(0) );
	d3dc._SetDecl(R3D_DEBUG_VERTEX::getDecl());

	ResetViewport();

	CurrentRenderingMode = R3D_BLEND_NONE;
	CurrentMaterialID    = -999;

	RModeStackTop        = 0;

	SetTex(NULL);
	SetMaterial(NULL);
	SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	//  pd3ddev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, 1);

}



void r3dRenderLayer::StartRenderSimple(int bClear)
{
	static int ClearFlag = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL;
	static int _FlagSet = 0;

	if ( !_FlagSet)
	{
		if (CurrentBPP == 16 )
			ClearFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
		else
			ClearFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

		_FlagSet = 1;
	}

	if (bClear)
		pd3ddev->Clear(0, NULL, ClearFlag, Fog.Color.GetPacked(), r3dRenderer->GetClearZValue(), 0 );


	ResetViewport();

	CurrentRenderingMode = R3D_BLEND_NONE;
	CurrentMaterialID    = -999;

	RModeStackTop        = 0;
}

int r3dRenderLayer::IsStereoActive()
{
	return NVApiStereoMode || r_3d_stereo_emu->GetInt() ;
}

//////////////////////////////////////////////////////////////////////////

void r3dRenderLayer::SetPresentEye(StereoEyeEnum eye)
{
	if (NVApiStereoMode)
		PresentEye = eye;
}

//////////////////////////////////////////////////////////////////////////

void r3dRenderLayer::SetDeviceLost()
{
	deviceLost_ = true;
}

void r3dRenderLayer::EnumerateRenderTargets( r3dTL::TArray< r3dScreenBuffer* >* oSBArray )
{
	*oSBArray = ScreenBuffers_ ;
}

void r3dRenderLayer::RegisterScreenBuffer( r3dScreenBuffer* sb )
{
	for( int i = 0, e = ScreenBuffers_.Count() ; i < e ; i ++ )
	{
		r3d_assert( ScreenBuffers_[ i ] != sb ) ;
	}

	ScreenBuffers_.PushBack( sb ) ;
}

void r3dRenderLayer::UnregisterScreenBuffer( r3dScreenBuffer* sb )
{
	for( int i = 0, e = ScreenBuffers_.Count() ; i < e ; i ++ )
	{
		if( ScreenBuffers_[ i ] == sb )
		{
			ScreenBuffers_.Erase( i, 1 ) ;
			return ;
		}
	}

	r3d_assert( false ) ;
}

void r3dRenderLayer::RegisterResource(r3dIResource* res)
{
	R3D_ENSURE_MAIN_THREAD();

	size_t firstFree = -1;
	for(size_t i = 0; i<resources_.Count(); ++i)
	{
		if(resources_[i] == res)
		{
			r3dError("r3d: resource %p already registered\n", res);
			return;
		}

		if(resources_[i] == NULL && firstFree == -1)
		{
			firstFree = i;
		}
	}

	if(firstFree == -1)
	{
		firstFree = resources_.Count();
		resources_.PushBack( res );
	}
	else
	{
		resources_[ firstFree ] = res ;
	}

	r3d_assert( resources_[ firstFree ] == res );
}

void r3dRenderLayer::DeregisterResource(r3dIResource* res)
{
	R3D_ENSURE_MAIN_THREAD();

	for( uint32_t i = 0, e = resources_.Count() ; i < e ; i ++ )
	{
		if( res == resources_[ i ] )
		{
			resources_[ i ] = NULL ;
			return ;
		}
	}

	r3dError("r3d: resource %p was not registered\n", res);
	return;
}

static int eventCounter = 0;
static IDirect3DQuery9* pQueryEvent[3] = {0};

IDirect3DQuery9*	g_pOcclusionQueries[ R3D_NUM_OCCLUSION_QUERIES ] = {0};
int					g_pOcclusionQueryUsed[ R3D_NUM_OCCLUSION_QUERIES ] = {0};

int					g_LastAllocatedQueryIdx = 0;
int					g_NumAllocatedNodes = 0;

int r3dAllocateOcclusionQuery()
{
	if( g_NumAllocatedNodes >= R3D_NUM_OCCLUSION_QUERIES )
		return -1;

	int i;

	for( i = g_LastAllocatedQueryIdx + 1 ; i != g_LastAllocatedQueryIdx ; )
	{
		i ++;

		if( i >= R3D_NUM_OCCLUSION_QUERIES )
			i = 0;

		if( !g_pOcclusionQueryUsed[ i ] )
		{
			g_pOcclusionQueryUsed[ i ] = 1;

			g_NumAllocatedNodes ++;

			g_LastAllocatedQueryIdx = i;

			return i;
		}
	}

	return -1;
}

void r3dFreeOcclusionQuery( int i )
{
	g_NumAllocatedNodes --;
	g_pOcclusionQueryUsed[ i ] = 0;
}

IDirect3DQuery9* r3dGetOcclusionQuery( int i )
{
	return g_pOcclusionQueries[ i ];
}

//------------------------------------------------------------------------

void r3dFrameQueryCreate()
{
	if ( pQueryEvent[ 0 ] == 0 && r3dRenderer->SupportsEventQueries)
	{
		eventCounter = 0;

		D3D_V( r3dRenderer->pd3ddev->CreateQuery(D3DQUERYTYPE_EVENT, &pQueryEvent[0]) );
		D3D_V( r3dRenderer->pd3ddev->CreateQuery(D3DQUERYTYPE_EVENT, &pQueryEvent[1]) );
		D3D_V( r3dRenderer->pd3ddev->CreateQuery(D3DQUERYTYPE_EVENT, &pQueryEvent[2]) );

		pQueryEvent[1]->Issue(D3DISSUE_END);
		pQueryEvent[2]->Issue(D3DISSUE_END);
	}
	if( g_pOcclusionQueries[0] == 0 && r3dRenderer->SupportsOQ )
	{
		for(int i =0; i<R3D_NUM_OCCLUSION_QUERIES; ++i)
		{
			D3D_V( r3dRenderer->pd3ddev->CreateQuery(D3DQUERYTYPE_OCCLUSION, &g_pOcclusionQueries[i] ) );
		}
	}

	for(int i =0; i<R3D_NUM_OCCLUSION_QUERIES; ++i)
	{
		g_pOcclusionQueryUsed[ i ] = 0;
	}

	g_NumAllocatedNodes = 0;
}

void r3dFrameQueryRelease()
{
	R3D_ENSURE_MAIN_THREAD();

	for ( int i = 0; i < 3; i++ )
	{
		if ( pQueryEvent[i] )
		{
			pQueryEvent[i]->Release();
			pQueryEvent[i] = 0;
		}
	}
	for(int i =0; i<R3D_NUM_OCCLUSION_QUERIES; ++i)
	{
		if(g_pOcclusionQueries[i])
		{
			g_pOcclusionQueries[i]->Release();
			g_pOcclusionQueries[i] = 0;
		}
	}
}

void ( *r3dDeviceResetCallback )() = NULL ;
void ( *r3dDevicePreResetCallback )() = NULL ;

int r3dRenderLayer::TryToRestoreDevice()
{
	R3D_ENSURE_MAIN_THREAD();

	if(!deviceLost_)
		return 1;

	r3dFinishBackGroundTasks();

	int DeviceWasAvailable = DeviceAvailable ;

	DeviceAvailable = 1 ;
	HRESULT hr;
	hr = r3dRenderer->pd3ddev->TestCooperativeLevel();

	struct FinishBackGroundTasksOnExit
	{
		FinishBackGroundTasksOnExit()
		{
			doFinish = 0;
		}

		~FinishBackGroundTasksOnExit()
		{
			if( doFinish )
				r3dFinishBackGroundTasks();
		}

		int doFinish;
	} finishBackGroundTasksOnExit; (void)finishBackGroundTasksOnExit;

	if(hr == D3DERR_DEVICENOTRESET || hr == S_OK) 
	{
		// because we lock device queue here for resource integrity reasons
		// we first have to acquire resource critical section ( resources first acquire resource critical section,
		// and then device queue critical section when they load )
		// Otherwise we have a chance of dead locking, because we try to acquire resource critical section
		// when we delete texture in r3dScreenBuffer::D3DReleaseResource AFTER blocking the device queue.
		// This happens in device resetting process
		r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

		// first of all, wait for the shitty locks & 'resource integrity' to stabilize
		for( ; ; )
		{
			BlockDeviceQueue() ;

			ProcessDeviceQueue( r3dGetTime(), 0.033f ) ;

			if( !gLockCounter && !gIntegrityCounter )
				break ;

			UnblockDeviceQueue();

			r3dResetWatchDog();

			Sleep( 0 ) ;
		}

		struct LiftQueueBlockOnExit
		{
			~LiftQueueBlockOnExit()
			{
				UnblockDeviceQueue();
			}

		} liftQueueBlockOnExit ; (void)liftQueueBlockOnExit ;

		// all locked resources should now be unlocked.

		r3dOutToLog("d3d: starting to reset device\n");

		r3dFrameQueryRelease();
		
		extern IDirect3DTexture9* _r3d_screenshot_copy;
		SAFE_RELEASE(_r3d_screenshot_copy);

		// otherwise shouldn't have ruined it

		int resources_count = resources_.Count();

		if( DeviceWasAvailable )
		{
			IDirect3DSurface9* surf ;
			D3D_V( r3dRenderer->pd3ddev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &surf ) );
			SetRT( 0, surf );

			surf->Release();
		
			for(int i=1; i<4; ++i)
			{
				SetRT( i, NULL );
			}
			for(int i=0; i<16; ++i)
				SetTex(NULL, i);

			SAFE_RELEASE( MainRT ) ;
			SAFE_RELEASE( MainDSS ) ;

			if( r3dDevicePreResetCallback )
				r3dDevicePreResetCallback();

			for( size_t i = 0; i < resources_.Count() ; ++ i )
			{
				r3dIResource* res = resources_[i];
				if( res ) 
				{
					res->D3DReleaseResource();
				}
			}

			UnloadUnmanagedTextures();

			r3dOutToLog("released\n");
		}
		else
		{
			r3dOutToLog( "Device lost during Reset attempt, hence all resources are already released!\n" );
		}

		hr = r3dRenderer->pd3ddev->Reset(&d3dpp);

		if( hr == D3DERR_DEVICELOST )
		{
			// ah du Schiesse!
			DeviceAvailable = 0 ;
			return 0 ;
		}


		if(hr != D3D_OK) {
			r3dError("unable to reset d3d device, 0x%X\n", hr);
			return 0;
		}

		gResetCount ++ ;

		r3dOutToLog("reseted\n");

		d3dSetDefaultStates();

		r3dFrameQueryCreate();

		AquireMainRT() ;
		RTs[ 0 ] = MainRT ;
		DSS = MainDSS ;

		PendingQuery = 0 ;

		for( size_t i = 0; i < resources_.Count(); ++ i ) 
		{
			if(resources_[i]) 
			{
				resources_[i]->D3DCreateResource();
			}
		}
		// make sure that we are not creating new resource while restoring d3d device!!!
		r3d_assert( resources_count == resources_.Count() );		

		LoadUnmanagedTextures();

		r3dOutToLog("restored (%d). reset ok\n", gResetCount);

		deviceLost_ = false;

		DeviceAvailable = 1 ;

		if ( g_bStartFrame ) // restore frame device 
			StartFrame();

		r3dResetWatchDog();

		if( r3dDeviceResetCallback )
			r3dDeviceResetCallback();

		finishBackGroundTasksOnExit.doFinish = 1;

		return 1;
	}

	if( !DeviceWasAvailable )
	{
		DeviceAvailable = 0 ;
	}

	return 0;
}

void r3dRenderLayer::UpdateSettings()
{	
	R3D_ENSURE_MAIN_THREAD();

	D3DDISPLAYMODE mode;
	if(!AdjustWindowSize( r_width->GetInt(), r_height->GetInt(), r_bpp->GetInt(), !r_fullscreen_load->GetInt(), mode ))
	{
		// let's try again in windowed mode
		AdjustWindowSize( r_width->GetInt(), r_height->GetInt(), r_bpp->GetInt(), 1, mode );
	}

	if( d3dpp.BackBufferWidth != mode.Width ||
		d3dpp.BackBufferHeight != mode.Height )
	{
		r3dOutToLog("UpdateSettings: changing resolution to width: %d height: %d\n", r_width->GetInt(), r_height->GetInt());
		d3dpp.BackBufferWidth	= mode.Width;
		d3dpp.BackBufferHeight	= mode.Height;

		CurRenderPipeline->DestroyResolutionBoundResources();

		if( r3dDevicePreResetCallback )
			r3dDevicePreResetCallback();

		for( uint32_t i = 0, e = resources_.Count(); i < e; i ++ )
		{
			if( r3dIResource* res = resources_[ i ] )
			{
				res->D3DReleaseResource();
			}
		}

		UnloadUnmanagedTextures();

		int mainRTRefs = 0 ;
		if( MainRT )
		{
			mainRTRefs = MainRT->Release() ;
			MainRT = NULL ;
		}

		int dssRefs = 0 ;
		if( MainDSS )
		{
			dssRefs = MainDSS->Release() ;
			MainDSS = NULL ;
		}

		r3dFrameQueryRelease();

		HRESULT hr ;
		for( ; ; )
		{
			hr = pd3ddev->Reset( &d3dpp ) ;

			if( hr != D3DERR_DEVICELOST )
				break ;

			r3dOutToLog( "Failed to reset device after settings change!\n") ;
			r3dProcessWindowMessages();
			Sleep( 250 ) ;
		}

		if( hr != S_OK )
		{
			r3dError( "Unable to Reset d3d device after settings change!\n");
		}

		AquireMainRT();
		RTs[ 0 ] = MainRT ;
		DSS = MainDSS ;

		r3dFrameQueryCreate();		

		UpdateDimmensions();

		for( uint32_t i = 0, e = resources_.Count(); i < e; i ++ )
		{
			if( r3dIResource* res = resources_[ i ] )
			{
				res->D3DCreateResource();
			}
		}

		CurRenderPipeline->CreateResolutionBoundResources();

		LoadUnmanagedTextures();

		if( r3dDeviceResetCallback )
			r3dDeviceResetCallback() ;
	}
}

// allow some minor difference cause we resize in window mode so that window fits into screen
static const float ASPECT_DELTA = 0.025f ;

enum PackedD3DFormat
{
	P_D3DFMT_R8G8B8               ,
	P_D3DFMT_A8R8G8B8             ,
	P_D3DFMT_X8R8G8B8             ,
	P_D3DFMT_R5G6B5               ,
	P_D3DFMT_X1R5G5B5             ,
	P_D3DFMT_A1R5G5B5             ,
	P_D3DFMT_A4R4G4B4             ,
	P_D3DFMT_R3G3B2               ,
	P_D3DFMT_A8                   ,
	P_D3DFMT_A8R3G3B2             ,
	P_D3DFMT_X4R4G4B4             ,
	P_D3DFMT_A2B10G10R10          ,
	P_D3DFMT_A8B8G8R8             ,
	P_D3DFMT_X8B8G8R8             ,
	P_D3DFMT_G16R16               ,
	P_D3DFMT_A2R10G10B10          ,
	P_D3DFMT_A16B16G16R16         ,

	P_D3DFMT_A8P8                 ,
	P_D3DFMT_P8                   ,

	P_D3DFMT_L8                   ,
	P_D3DFMT_A8L8                 ,
	P_D3DFMT_A4L4                 ,

	P_D3DFMT_V8U8                 ,
	P_D3DFMT_L6V5U5               ,
	P_D3DFMT_X8L8V8U8             ,
	P_D3DFMT_Q8W8V8U8             ,
	P_D3DFMT_V16U16               ,
	P_D3DFMT_A2W10V10U10          ,

	P_D3DFMT_UYVY                 ,
	P_D3DFMT_R8G8_B8G8            ,
	P_D3DFMT_YUY2                 ,
	P_D3DFMT_G8R8_G8B8            ,
	P_D3DFMT_DXT1                 ,
	P_D3DFMT_DXT2                 ,
	P_D3DFMT_DXT3                 ,
	P_D3DFMT_DXT4                 ,
	P_D3DFMT_DXT5                 ,

	P_D3DFMT_D16_LOCKABLE         ,
	P_D3DFMT_D32                  ,
	P_D3DFMT_D15S1                ,
	P_D3DFMT_D24S8                ,
	P_D3DFMT_D24X8                ,
	P_D3DFMT_D24X4S4              ,
	P_D3DFMT_D16                  ,

	P_D3DFMT_D32F_LOCKABLE        ,
	P_D3DFMT_D24FS8               ,

	P_D3DFMT_L16                  ,

	P_D3DFMT_Q16W16V16U16         ,

	P_D3DFMT_MULTI2_ARGB8         ,

	P_D3DFMT_R16F                 ,
	P_D3DFMT_G16R16F              ,
	P_D3DFMT_A16B16G16R16F        ,

	P_D3DFMT_R32F                 ,
	P_D3DFMT_G32R32F              ,
	P_D3DFMT_A32B32G32R32F        ,

	P_D3DFMT_CxV8U8               ,

	P_D3DFMT_A1                   ,

	P_D3DFMT_A2B10G10R10_XR_BIAS  ,

	P_D3DFMT_COUNT
};

const char* PackedFmtToString( PackedD3DFormat fmt )
{
	switch( fmt )
	{
	case P_D3DFMT_R8G8B8               : return "D3DFMT_R8G8B8"               ;
	case P_D3DFMT_A8R8G8B8             : return "D3DFMT_A8R8G8B8"             ;
	case P_D3DFMT_X8R8G8B8             : return "D3DFMT_X8R8G8B8"             ;
	case P_D3DFMT_R5G6B5               : return "D3DFMT_R5G6B5"               ;
	case P_D3DFMT_X1R5G5B5             : return "D3DFMT_X1R5G5B5"             ;
	case P_D3DFMT_A1R5G5B5             : return "D3DFMT_A1R5G5B5"             ;
	case P_D3DFMT_A4R4G4B4             : return "D3DFMT_A4R4G4B4"             ;
	case P_D3DFMT_R3G3B2               : return "D3DFMT_R3G3B2"               ;
	case P_D3DFMT_A8                   : return "D3DFMT_A8"                   ;
	case P_D3DFMT_A8R3G3B2             : return "D3DFMT_A8R3G3B2"             ;
	case P_D3DFMT_X4R4G4B4             : return "D3DFMT_X4R4G4B4"             ;
	case P_D3DFMT_A2B10G10R10          : return "D3DFMT_A2B10G10R10"          ;
	case P_D3DFMT_A8B8G8R8             : return "D3DFMT_A8B8G8R8"             ;
	case P_D3DFMT_X8B8G8R8             : return "D3DFMT_X8B8G8R8"             ;
	case P_D3DFMT_G16R16               : return "D3DFMT_G16R16"               ;
	case P_D3DFMT_A2R10G10B10          : return "D3DFMT_A2R10G10B10"          ;
	case P_D3DFMT_A16B16G16R16         : return "D3DFMT_A16B16G16R16"         ;
	case P_D3DFMT_A8P8                 : return "D3DFMT_A8P8"                 ;
	case P_D3DFMT_P8                   : return "D3DFMT_P8"                   ;
	case P_D3DFMT_L8                   : return "D3DFMT_L8"                   ;
	case P_D3DFMT_A8L8                 : return "D3DFMT_A8L8"                 ;
	case P_D3DFMT_A4L4                 : return "D3DFMT_A4L4"                 ;
	case P_D3DFMT_V8U8                 : return "D3DFMT_V8U8"                 ;
	case P_D3DFMT_L6V5U5               : return "D3DFMT_L6V5U5"               ;
	case P_D3DFMT_X8L8V8U8             : return "D3DFMT_X8L8V8U8"             ;
	case P_D3DFMT_Q8W8V8U8             : return "D3DFMT_Q8W8V8U8"             ;
	case P_D3DFMT_V16U16               : return "D3DFMT_V16U16"               ;
	case P_D3DFMT_A2W10V10U10          : return "D3DFMT_A2W10V10U10"          ;
	case P_D3DFMT_UYVY                 : return "D3DFMT_UYVY"                 ;
	case P_D3DFMT_R8G8_B8G8            : return "D3DFMT_R8G8_B8G8"            ;
	case P_D3DFMT_YUY2                 : return "D3DFMT_YUY2"                 ;
	case P_D3DFMT_G8R8_G8B8            : return "D3DFMT_G8R8_G8B8"            ;
	case P_D3DFMT_DXT1                 : return "D3DFMT_DXT1"                 ;
	case P_D3DFMT_DXT2                 : return "D3DFMT_DXT2"                 ;
	case P_D3DFMT_DXT3                 : return "D3DFMT_DXT3"                 ;
	case P_D3DFMT_DXT4                 : return "D3DFMT_DXT4"                 ;
	case P_D3DFMT_DXT5                 : return "D3DFMT_DXT5"                 ;
	case P_D3DFMT_D16_LOCKABLE         : return "D3DFMT_D16_LOCKABLE"         ;
	case P_D3DFMT_D32                  : return "D3DFMT_D32"                  ;
	case P_D3DFMT_D15S1                : return "D3DFMT_D15S1"                ;
	case P_D3DFMT_D24S8                : return "D3DFMT_D24S8"                ;
	case P_D3DFMT_D24X8                : return "D3DFMT_D24X8"                ;
	case P_D3DFMT_D24X4S4              : return "D3DFMT_D24X4S4"              ;
	case P_D3DFMT_D16                  : return "D3DFMT_D16"                  ;
	case P_D3DFMT_D32F_LOCKABLE        : return "D3DFMT_D32F_LOCKABLE"        ;
	case P_D3DFMT_D24FS8               : return "D3DFMT_D24FS8"               ;
	case P_D3DFMT_L16                  : return "D3DFMT_L16"                  ;
	case P_D3DFMT_Q16W16V16U16         : return "D3DFMT_Q16W16V16U16"         ;
	case P_D3DFMT_MULTI2_ARGB8         : return "D3DFMT_MULTI2_ARGB8"         ;
	case P_D3DFMT_R16F                 : return "D3DFMT_R16F"                 ;
	case P_D3DFMT_G16R16F              : return "D3DFMT_G16R16F"              ;
	case P_D3DFMT_A16B16G16R16F        : return "D3DFMT_A16B16G16R16F"        ;
	case P_D3DFMT_R32F                 : return "D3DFMT_R32F"                 ;
	case P_D3DFMT_G32R32F              : return "D3DFMT_G32R32F"              ;
	case P_D3DFMT_A32B32G32R32F        : return "D3DFMT_A32B32G32R32F"        ;
	case P_D3DFMT_CxV8U8               : return "D3DFMT_CxV8U8"               ;
	case P_D3DFMT_A1                   : return "D3DFMT_A1"                   ;
	case P_D3DFMT_A2B10G10R10_XR_BIAS  : return "D3DFMT_A2B10G10R10_XR_BIAS"  ;

	default:
		r3d_assert( false ) ;
	};

	return "UNKNOWN" ;
}

PackedD3DFormat PackD3DFormat( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_R8G8B8               : return P_D3DFMT_R8G8B8               ;
	case D3DFMT_A8R8G8B8             : return P_D3DFMT_A8R8G8B8             ;
	case D3DFMT_X8R8G8B8             : return P_D3DFMT_X8R8G8B8             ;
	case D3DFMT_R5G6B5               : return P_D3DFMT_R5G6B5               ;
	case D3DFMT_X1R5G5B5             : return P_D3DFMT_X1R5G5B5             ;
	case D3DFMT_A1R5G5B5             : return P_D3DFMT_A1R5G5B5             ;
	case D3DFMT_A4R4G4B4             : return P_D3DFMT_A4R4G4B4             ;
	case D3DFMT_R3G3B2               : return P_D3DFMT_R3G3B2               ;
	case D3DFMT_A8                   : return P_D3DFMT_A8                   ;
	case D3DFMT_A8R3G3B2             : return P_D3DFMT_A8R3G3B2             ;
	case D3DFMT_X4R4G4B4             : return P_D3DFMT_X4R4G4B4             ;
	case D3DFMT_A2B10G10R10          : return P_D3DFMT_A2B10G10R10          ;
	case D3DFMT_A8B8G8R8             : return P_D3DFMT_A8B8G8R8             ;
	case D3DFMT_X8B8G8R8             : return P_D3DFMT_X8B8G8R8             ;
	case D3DFMT_G16R16               : return P_D3DFMT_G16R16               ;
	case D3DFMT_A2R10G10B10          : return P_D3DFMT_A2R10G10B10          ;
	case D3DFMT_A16B16G16R16         : return P_D3DFMT_A16B16G16R16         ;
	case D3DFMT_A8P8                 : return P_D3DFMT_A8P8                 ;
	case D3DFMT_P8                   : return P_D3DFMT_P8                   ;
	case D3DFMT_L8                   : return P_D3DFMT_L8                   ;
	case D3DFMT_A8L8                 : return P_D3DFMT_A8L8                 ;
	case D3DFMT_A4L4                 : return P_D3DFMT_A4L4                 ;
	case D3DFMT_V8U8                 : return P_D3DFMT_V8U8                 ;
	case D3DFMT_L6V5U5               : return P_D3DFMT_L6V5U5               ;
	case D3DFMT_X8L8V8U8             : return P_D3DFMT_X8L8V8U8             ;
	case D3DFMT_Q8W8V8U8             : return P_D3DFMT_Q8W8V8U8             ;
	case D3DFMT_V16U16               : return P_D3DFMT_V16U16               ;
	case D3DFMT_A2W10V10U10          : return P_D3DFMT_A2W10V10U10          ;
	case D3DFMT_UYVY                 : return P_D3DFMT_UYVY                 ;
	case D3DFMT_R8G8_B8G8            : return P_D3DFMT_R8G8_B8G8            ;
	case D3DFMT_YUY2                 : return P_D3DFMT_YUY2                 ;
	case D3DFMT_G8R8_G8B8            : return P_D3DFMT_G8R8_G8B8            ;
	case D3DFMT_DXT1                 : return P_D3DFMT_DXT1                 ;
	case D3DFMT_DXT2                 : return P_D3DFMT_DXT2                 ;
	case D3DFMT_DXT3                 : return P_D3DFMT_DXT3                 ;
	case D3DFMT_DXT4                 : return P_D3DFMT_DXT4                 ;
	case D3DFMT_DXT5                 : return P_D3DFMT_DXT5                 ;
	case D3DFMT_D16_LOCKABLE         : return P_D3DFMT_D16_LOCKABLE         ;
	case D3DFMT_D32                  : return P_D3DFMT_D32                  ;
	case D3DFMT_D15S1                : return P_D3DFMT_D15S1                ;
	case D3DFMT_D24S8                : return P_D3DFMT_D24S8                ;
	case D3DFMT_D24X8                : return P_D3DFMT_D24X8                ;
	case D3DFMT_D24X4S4              : return P_D3DFMT_D24X4S4              ;
	case D3DFMT_D16                  : return P_D3DFMT_D16                  ;
	case D3DFMT_D32F_LOCKABLE        : return P_D3DFMT_D32F_LOCKABLE        ;
	case D3DFMT_D24FS8               : return P_D3DFMT_D24FS8               ;
	case D3DFMT_L16                  : return P_D3DFMT_L16                  ;
	case D3DFMT_Q16W16V16U16         : return P_D3DFMT_Q16W16V16U16         ;
	case D3DFMT_MULTI2_ARGB8         : return P_D3DFMT_MULTI2_ARGB8         ;
	case D3DFMT_R16F                 : return P_D3DFMT_R16F                 ;
	case D3DFMT_G16R16F              : return P_D3DFMT_G16R16F              ;
	case D3DFMT_A16B16G16R16F        : return P_D3DFMT_A16B16G16R16F        ;
	case D3DFMT_R32F                 : return P_D3DFMT_R32F                 ;
	case D3DFMT_G32R32F              : return P_D3DFMT_G32R32F              ;
	case D3DFMT_A32B32G32R32F        : return P_D3DFMT_A32B32G32R32F        ;
	case D3DFMT_CxV8U8               : return P_D3DFMT_CxV8U8               ;
	case D3DFMT_A1                   : return P_D3DFMT_A1                   ;
	case D3DFMT_A2B10G10R10_XR_BIAS  : return P_D3DFMT_A2B10G10R10_XR_BIAS  ;

	default:
		r3d_assert( false ) ;
		return P_D3DFMT_R8G8B8 ;
	};
}

bool IsZBufferFormat( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_D16_LOCKABLE         :
	case D3DFMT_D32                  :
	case D3DFMT_D15S1                :
	case D3DFMT_D24S8                :
	case D3DFMT_D24X8                :
	case D3DFMT_D24X4S4              :
	case D3DFMT_D16                  :
	case D3DFMT_D32F_LOCKABLE        :
	case D3DFMT_D24FS8               :
		return true ;
	}

	return false ;
}

bool IsMultiElementFormat( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_MULTI2_ARGB8:

#if 0
	case D3DFMT_G8R8_G8B8:
	case D3DFMT_R8G8_B8G8:
	case D3DFMT_UYVY:
	case D3DFMT_YUY2:
#endif
		return true ;
	}

	return false ;
}

D3DFORMAT UnpackD3DFormat( PackedD3DFormat fmt )
{
	switch( fmt )
	{
	case P_D3DFMT_R8G8B8               : return D3DFMT_R8G8B8               ;
	case P_D3DFMT_A8R8G8B8             : return D3DFMT_A8R8G8B8             ;
	case P_D3DFMT_X8R8G8B8             : return D3DFMT_X8R8G8B8             ;
	case P_D3DFMT_R5G6B5               : return D3DFMT_R5G6B5               ;
	case P_D3DFMT_X1R5G5B5             : return D3DFMT_X1R5G5B5             ;
	case P_D3DFMT_A1R5G5B5             : return D3DFMT_A1R5G5B5             ;
	case P_D3DFMT_A4R4G4B4             : return D3DFMT_A4R4G4B4             ;
	case P_D3DFMT_R3G3B2               : return D3DFMT_R3G3B2               ;
	case P_D3DFMT_A8                   : return D3DFMT_A8                   ;
	case P_D3DFMT_A8R3G3B2             : return D3DFMT_A8R3G3B2             ;
	case P_D3DFMT_X4R4G4B4             : return D3DFMT_X4R4G4B4             ;
	case P_D3DFMT_A2B10G10R10          : return D3DFMT_A2B10G10R10          ;
	case P_D3DFMT_A8B8G8R8             : return D3DFMT_A8B8G8R8             ;
	case P_D3DFMT_X8B8G8R8             : return D3DFMT_X8B8G8R8             ;
	case P_D3DFMT_G16R16               : return D3DFMT_G16R16               ;
	case P_D3DFMT_A2R10G10B10          : return D3DFMT_A2R10G10B10          ;
	case P_D3DFMT_A16B16G16R16         : return D3DFMT_A16B16G16R16         ;
	case P_D3DFMT_A8P8                 : return D3DFMT_A8P8                 ;
	case P_D3DFMT_P8                   : return D3DFMT_P8                   ;
	case P_D3DFMT_L8                   : return D3DFMT_L8                   ;
	case P_D3DFMT_A8L8                 : return D3DFMT_A8L8                 ;
	case P_D3DFMT_A4L4                 : return D3DFMT_A4L4                 ;
	case P_D3DFMT_V8U8                 : return D3DFMT_V8U8                 ;
	case P_D3DFMT_L6V5U5               : return D3DFMT_L6V5U5               ;
	case P_D3DFMT_X8L8V8U8             : return D3DFMT_X8L8V8U8             ;
	case P_D3DFMT_Q8W8V8U8             : return D3DFMT_Q8W8V8U8             ;
	case P_D3DFMT_V16U16               : return D3DFMT_V16U16               ;
	case P_D3DFMT_A2W10V10U10          : return D3DFMT_A2W10V10U10          ;
	case P_D3DFMT_UYVY                 : return D3DFMT_UYVY                 ;
	case P_D3DFMT_R8G8_B8G8            : return D3DFMT_R8G8_B8G8            ;
	case P_D3DFMT_YUY2                 : return D3DFMT_YUY2                 ;
	case P_D3DFMT_G8R8_G8B8            : return D3DFMT_G8R8_G8B8            ;
	case P_D3DFMT_DXT1                 : return D3DFMT_DXT1                 ;
	case P_D3DFMT_DXT2                 : return D3DFMT_DXT2                 ;
	case P_D3DFMT_DXT3                 : return D3DFMT_DXT3                 ;
	case P_D3DFMT_DXT4                 : return D3DFMT_DXT4                 ;
	case P_D3DFMT_DXT5                 : return D3DFMT_DXT5                 ;
	case P_D3DFMT_D16_LOCKABLE         : return D3DFMT_D16_LOCKABLE         ;
	case P_D3DFMT_D32                  : return D3DFMT_D32                  ;
	case P_D3DFMT_D15S1                : return D3DFMT_D15S1                ;
	case P_D3DFMT_D24S8                : return D3DFMT_D24S8                ;
	case P_D3DFMT_D24X8                : return D3DFMT_D24X8                ;
	case P_D3DFMT_D24X4S4              : return D3DFMT_D24X4S4              ;
	case P_D3DFMT_D16                  : return D3DFMT_D16                  ;
	case P_D3DFMT_D32F_LOCKABLE        : return D3DFMT_D32F_LOCKABLE        ;
	case P_D3DFMT_D24FS8               : return D3DFMT_D24FS8               ;
	case P_D3DFMT_L16                  : return D3DFMT_L16                  ;
	case P_D3DFMT_Q16W16V16U16         : return D3DFMT_Q16W16V16U16         ;
	case P_D3DFMT_MULTI2_ARGB8         : return D3DFMT_MULTI2_ARGB8         ;
	case P_D3DFMT_R16F                 : return D3DFMT_R16F                 ;
	case P_D3DFMT_G16R16F              : return D3DFMT_G16R16F              ;
	case P_D3DFMT_A16B16G16R16F        : return D3DFMT_A16B16G16R16F        ;
	case P_D3DFMT_R32F                 : return D3DFMT_R32F                 ;
	case P_D3DFMT_G32R32F              : return D3DFMT_G32R32F              ;
	case P_D3DFMT_A32B32G32R32F        : return D3DFMT_A32B32G32R32F        ;
	case P_D3DFMT_CxV8U8               : return D3DFMT_CxV8U8               ;
	case P_D3DFMT_A1                   : return D3DFMT_A1                   ;
	case P_D3DFMT_A2B10G10R10_XR_BIAS  : return D3DFMT_A2B10G10R10_XR_BIAS  ;

	default:
		r3d_assert( false ) ;
	};

	return D3DFMT_UNKNOWN ;
}

template < typename T >
void FillFormatValues( T &Formats )
{
	COMPILE_ASSERT( T::COUNT >= P_D3DFMT_COUNT ) ;

	for( int i = 0, e = P_D3DFMT_COUNT ; i < e; i ++ )
	{
		Formats[ i ] = UnpackD3DFormat( PackedD3DFormat( i ) );
	}
}

r3dDeviceInfo::r3dDeviceInfo()
{
	memset( this, 0, sizeof*this );
}

void FillDeviceInfo( r3dDeviceInfo* oInfo )
{
	r3d_assert( r3dRenderer ) ;
	r3d_assert( r3dRenderer->pd3ddev ) ;

	D3D_V( r3dRenderer->pd3d->GetAdapterIdentifier( 0, 0, &oInfo->AdapterIdentifier ) );
	D3D_V( r3dRenderer->pd3ddev->GetDeviceCaps( &oInfo->Caps ) ) ;

	r3dTL::TFixedArray< D3DFORMAT, P_D3DFMT_COUNT > Formats ;

	FillFormatValues( Formats ) ;

	for( uint32_t i = 0, e = P_D3DFMT_COUNT ; i < e; i ++ )
	{
		#define R3D_CHECK_USAGE(d3dusage,usage) \
			if( r3dRenderer->pd3d->CheckDeviceFormat( 0, D3DDEVTYPE_HAL, r3dRenderLayer::DISPLAY_FORMAT_32, d3dusage, D3DRTYPE_TEXTURE, Formats[ i ] ) == D3D_OK ) tflags |= usage ;	\
			if( r3dRenderer->pd3d->CheckDeviceFormat( 0, D3DDEVTYPE_HAL, r3dRenderLayer::DISPLAY_FORMAT_32, d3dusage, D3DRTYPE_CUBETEXTURE, Formats[ i ] ) == D3D_OK ) cflags |= usage ;
		{
			WORD& tflags = oInfo->TextureFormatCaps[ i ] ;
			WORD& cflags = oInfo->CubeTextureFormatCaps[ i ] ;

			tflags = 0 ;
			cflags = 0 ;

			R3D_CHECK_USAGE( D3DUSAGE_DYNAMIC				, r3dDeviceInfo::DYNAMIC			) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_FILTER			, r3dDeviceInfo::FILTER			) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_SRGBREAD		, r3dDeviceInfo::SRGB_READ			) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_SRGBWRITE		, r3dDeviceInfo::SRGB_WRITE		) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_VERTEXTEXTURE	, r3dDeviceInfo::VERTEX_TEXTURE	) ;

			if( IsZBufferFormat( Formats[ i ] ) )
			{
				R3D_CHECK_USAGE( D3DUSAGE_DEPTHSTENCIL			, r3dDeviceInfo::DEPTHSTENCIL		) ;
			}

			R3D_CHECK_USAGE( D3DUSAGE_RENDERTARGET			, r3dDeviceInfo::RENDERTARGET		) ;

			if( !IsMultiElementFormat( Formats[ i ] ) )
			{
				R3D_CHECK_USAGE( D3DUSAGE_AUTOGENMIPMAP			, r3dDeviceInfo::AUTOGENMIPMAP		) ;
			}
		}

		#undef R3D_CHECK_USAGE

		#define R3D_CHECK_USAGE(d3dusage,usage) \
			if( r3dRenderer->pd3d->CheckDeviceFormat( 0, D3DDEVTYPE_HAL, r3dRenderLayer::DISPLAY_FORMAT_32, d3dusage, D3DRTYPE_SURFACE, Formats[ i ] ) == D3D_OK ) sflags |= usage ;

		{
			WORD& sflags = oInfo->SurfaceFormatCaps[ i ] ;

			sflags = 0 ;

			if( IsZBufferFormat( Formats[ i ] ) )
			{
				R3D_CHECK_USAGE( D3DUSAGE_DEPTHSTENCIL			, r3dDeviceInfo::DEPTHSTENCIL		) ;
			}

			R3D_CHECK_USAGE( D3DUSAGE_RENDERTARGET			, r3dDeviceInfo::RENDERTARGET		) ;
		}

		#undef R3D_CHECK_USAGE

		#define R3D_CHECK_USAGE(d3dusage,usage) \
			if( r3dRenderer->pd3d->CheckDeviceFormat( 0, D3DDEVTYPE_HAL, r3dRenderLayer::DISPLAY_FORMAT_32, d3dusage, D3DRTYPE_VOLUMETEXTURE, Formats[ i ] ) == D3D_OK ) vflags |= usage ;
		{
			WORD& vflags = oInfo->VolTextureFormatCaps[ i ] ;

			vflags = 0 ;
			R3D_CHECK_USAGE( D3DUSAGE_DYNAMIC				, r3dDeviceInfo::DYNAMIC			) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_FILTER			, r3dDeviceInfo::FILTER			) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_SRGBREAD		, r3dDeviceInfo::SRGB_READ			) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_SRGBWRITE		, r3dDeviceInfo::SRGB_WRITE		) ;
			R3D_CHECK_USAGE( D3DUSAGE_QUERY_VERTEXTEXTURE	, r3dDeviceInfo::VERTEX_TEXTURE	) ;
		}

		#undef R3D_CHECK_USAGE
	}

	oInfo->ExtraFlags = 0 ;

	if( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_OCCLUSION, NULL ) == D3D_OK )
	{
		oInfo->ExtraFlags |= r3dDeviceInfo::OCCLUSION_QUERIES ;
	}

	if( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_TIMESTAMP, NULL ) == D3D_OK )
	{
		oInfo->ExtraFlags |= r3dDeviceInfo::STAMP_QUERIES ;
	}

	oInfo->LocalMemSize = R3D_MAX( LocalVideoMemoryDDraw(), LocalVideoMemoryWMI() );
		
}

void FPrintfDeviceInfo( const char* file, const r3dDeviceInfo& info )
{
	FILE* fo = fopen( file, "wb" ) ;

	// format caps for now..
	fprintf( fo, "%22s %-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Format", "Dynam", "Filter", "SRGBR", "SRGBW", "VerText", "DS", "RT", "AutoMip" ) ;

	for( uint32_t i = 0, e = P_D3DFMT_COUNT ; i < e; i ++ )
	{
		char Dyn, Fil, SR, SW, VT, DS, RT, AM ;

		Dyn = Fil = SR = SW = VT = DS = RT = AM = ' ' ;	

		DWORD caps = info.TextureFormatCaps[ i ] ;

		if( caps & r3dDeviceInfo::DYNAMIC )			{		Dyn = '+' ;		}
		if( caps & r3dDeviceInfo::FILTER )			{		Fil = '+' ;		}
		if( caps & r3dDeviceInfo::SRGB_READ )		{		SR = '+' ;		}
		if( caps & r3dDeviceInfo::SRGB_WRITE )		{		SW = '+' ;		}
		if( caps & r3dDeviceInfo::VERTEX_TEXTURE )	{		VT = '+' ;		}
		if( caps & r3dDeviceInfo::DEPTHSTENCIL )	{		DS = '+' ;		}
		if( caps & r3dDeviceInfo::RENDERTARGET )	{		RT = '+' ;		}
		if( caps & r3dDeviceInfo::AUTOGENMIPMAP )	{		AM = '+' ;		}

#if 0
#if 0
		if( ( caps ^ scaps ) & DeviceInfo::DYNAMIC )		{		Dyn = '!' ;		}
		if( ( caps ^ scaps ) & DeviceInfo::FILTER )			{		Fil = '!' ;		}
		if( ( caps ^ scaps ) & DeviceInfo::SRGB_READ )		{		SR = '!' ;		}
		if( ( caps ^ scaps ) & DeviceInfo::SRGB_WRITE )		{		SW = '!' ;		}
		if( ( caps ^ scaps ) & DeviceInfo::VERTEX_TEXTURE )	{		VT = '!' ;		}
		if( ( caps ^ scaps ) & DeviceInfo::DEPTHSTENCIL )	{		DS = '!' ;		}
		if( ( caps ^ scaps ) & DeviceInfo::RENDERTARGET )	{		RT = '!' ;		}
		if( ( caps ^ scaps ) & DeviceInfo::AUTOGENMIPMAP )	{		AM = '!' ;		}
#else
		if( !( caps & r3dDeviceInfo::DYNAMIC			) && scaps & r3dDeviceInfo::DYNAMIC )		{		Dyn = '!' ;		}
		if( !( caps & r3dDeviceInfo::FILTER			) && scaps & r3dDeviceInfo::FILTER )			{		Fil = '!' ;		}
		if( !( caps & r3dDeviceInfo::SRGB_READ			) && scaps & r3dDeviceInfo::SRGB_READ )		{		SR = '!' ;		}
		if( !( caps & r3dDeviceInfo::SRGB_WRITE		) && scaps & r3dDeviceInfo::SRGB_WRITE )		{		SW = '!' ;		}
		if( !( caps & r3dDeviceInfo::VERTEX_TEXTURE	) && scaps & r3dDeviceInfo::VERTEX_TEXTURE )	{		VT = '!' ;		}
		if( !( caps & r3dDeviceInfo::DEPTHSTENCIL		) && scaps & r3dDeviceInfo::DEPTHSTENCIL )	{		DS = '!' ;		}
		if( !( caps & r3dDeviceInfo::RENDERTARGET		) && scaps & r3dDeviceInfo::RENDERTARGET )	{		RT = '!' ;		}
		if( !( caps & r3dDeviceInfo::AUTOGENMIPMAP		) && scaps & r3dDeviceInfo::AUTOGENMIPMAP )	{		AM = '!' ;		}
#endif
#endif

		fprintf( fo, "%22s %-8c%-8c%-8c%-8c%-8c%-8c%-8c%-8c\n", PackedFmtToString( PackedD3DFormat( i ) ), Dyn, Fil, SR, SW, VT, DS, RT, AM ) ;
	}

	fclose( fo ) ;
}

bool IsAspectLessThanForceAspect( float aspect )
{
	return aspect < r_force_aspect->GetFloat() - ASPECT_DELTA ;
}
bool IsAspectGreaterThanForceAspect( float aspect )
{
	return aspect > r_force_aspect->GetFloat() + ASPECT_DELTA ;
}

void r3dRenderLayer::UpdateDimmensions()
{
	ScreenW       = (float)d3dpp.BackBufferWidth;
	ScreenH       = (float)d3dpp.BackBufferHeight;

	if( r_force_aspect->GetFloat() > 0.f )
	{
		float currentAspect = ScreenW / ScreenH ;

		if( IsAspectLessThanForceAspect( currentAspect ) )
		{
			ScreenH = ScreenW / r_force_aspect->GetFloat() ;
		}
		else
		if( IsAspectGreaterThanForceAspect( currentAspect ) )
		{
			ScreenW = ScreenH * r_force_aspect->GetFloat() ;
		}
	}

	ScreenW2      = ScreenW / 2;
	ScreenH2      = ScreenH / 2;

	r_screen_scale_x->SetVal( ScreenW / (float)d3dpp.BackBufferWidth );
	r_screen_scale_y->SetVal( ScreenH / (float)d3dpp.BackBufferHeight );

	r_screen_offset_x->SetVal( ( (float)d3dpp.BackBufferWidth - ScreenW )  / d3dpp.BackBufferWidth / 2.f );
	r_screen_offset_y->SetVal( ( (float)d3dpp.BackBufferHeight - ScreenH )  / d3dpp.BackBufferHeight / 2.f );

	__r3dGlobalAspect = ScreenW / ScreenH;

	ResetViewport();
}

void r3dRenderLayer::ChangeForceAspect( float val )
{
/*#ifdef FINAL_BUILD
	r_force_aspect->SetFloat( val ) ;
	UpdateDimmensions() ;
#endif*/
}

void r3dRenderLayer::SetShaderLoadingEnabled( bool enabled )
{
	r3d_assert( !!allowShaderLoading_ ^ !!enabled );

	allowShaderLoading_ = enabled;
}

void r3dRenderLayer::ResetTextureCache()
{
	for( size_t i = 0, e = R3D_ARRAYSIZE(CurrentTexID); i < e ; i ++  )
	{
		CurrentTexID[ i ] = (IDirect3DBaseTexture9*)-1;
	}
}

DisplayResolutions r3dRenderLayer::GetDisplayResolutions() const
{

	r3d_assert( pd3d );

	DisplayResolutions res ;

	D3DFORMAT DisplayFormat = r_bpp->GetInt() > 16 ? DISPLAY_FORMAT_32 : DISPLAY_FORMAT_16 ;

	for( UINT i = 0, e = pd3d->GetAdapterModeCount( D3DADAPTER_DEFAULT, DisplayFormat ); i < e; i ++ )
	{
		D3DDISPLAYMODE mode;
		HRESULT hr = pd3d->EnumAdapterModes( D3DADAPTER_DEFAULT, DisplayFormat, i, &mode );

		if( hr == D3D_OK )
		{
			bool alreadyExists = false ;

			for( uint32_t i = 0, e = res.Count(); i < e; i ++ )
			{
				const r3dDisplayResolution& r = res[ i ] ;

				if( mode.Width	== r.Width 
						&&
					mode.Height	== r.Height
						)
				{
					alreadyExists = true ;
					break ;					
				}				
			}

			if( ! alreadyExists )
			{
				r3dDisplayResolution nr;

				nr.Width	= mode.Width ;
				nr.Height	= mode.Height ;

				res.PushBack( nr );
			}			
		}
	}

	return res ;
}

void r3dRenderLayer::Flush()
{
	vtxArray2D->Flush();
	vtxArray3D->Flush();
	vtxArray3D_Lines->Flush();
}



void Show_Copyright();

int gRenderFrameCounter = 0;
void r3dRenderLayer::EndRender( bool present /*= false*/ )
{
	void checkLicenseKey();
	checkLicenseKey();

#if 0
	Show_Copyright();
#endif

	if( DeviceAvailable )
	{
		R3DPROFILE_START("r3dRenderLayer::EndRender:Flush");

		Flush();

		R3DPROFILE_END("r3dRenderLayer::EndRender:Flush");

		D3D_V( d3dc._SetVertexShader(0) );
		d3dc._SetDecl(R3D_DEBUG_VERTEX::getDecl());
	}

	// Present the backbuffer contents to the display
	if( !present )
		return;

	gRenderFrameCounter++; // end of frame

	HRESULT hr = D3DERR_DEVICELOST ;

	if( r3dRenderer->DeviceAvailable )
	{
		R3DPROFILE_START("r3dRenderLayer::EndRender:Present");
		SetEye(PresentEye);

#ifdef __WITH_PB__
#ifndef WO_SERVER
		// Capture Frame for PunkBuster
		if ( PBReceiveSS != NULL )
		{
			unsigned int x, y, width, height ;
			unsigned int centerX = 0 ;
			unsigned int centerY = 0 ;
			unsigned int centerXpct = 100 ;
			unsigned int centerYpct = 100 ;
			PB_SScenterXpct = 100 ;
			PB_SScenterYpct = 100 ;

			unsigned int screenWidth, screenHeight ;
			screenWidth = d3dpp.BackBufferWidth ;
			screenHeight = d3dpp.BackBufferHeight ;

			centerX =  screenWidth / 2 ;
			centerY =  screenHeight / 2 ;

			if ( PB_SScenterXpct > 0 && PB_SScenterYpct > 0 )
			{
				width = ((PB_SScenterXpct/100) * screenWidth );
				height = ((PB_SScenterYpct/100) * screenHeight );
				x = centerX - width/2 ;
				y = centerY - height/2;
			}
			else
			{
				x = centerX - PB_SSrectWidth/2 ;
				y = centerY - PB_SSrectHeight/2;
				width = PB_SSrectWidth;
				height = PB_SSrectHeight;
			}

			IDirect3DSurface9 *BBuf = 0;

			IDirect3DDevice9 *d = r3dRenderer->pd3ddev;

			r3dRenderer->GetRT(0, &BBuf);

			IDirect3DSurface9 *sysmemBB = r3dRenderer->GetTempSurfaceForScreenShots();

			if (SUCCEEDED(d->GetRenderTargetData(BBuf, sysmemBB)))
			{

				D3DLOCKED_RECT lockedRect ;
				sysmemBB->LockRect ( &lockedRect, NULL, D3DLOCK_READONLY ) ;

				PBReceiveSS ( (unsigned char*)lockedRect.pBits, width, height, 32 ) ;

				sysmemBB->UnlockRect();
			}

			BBuf->Release() ;

			PBReceiveSS = NULL ;

		}
#endif
#endif

		hr = pd3ddev->Present(NULL, NULL, NULL, NULL);

		R3DPROFILE_END("r3dRenderLayer::EndRender:Present");
	}

	ProcessDeviceQueue(r3dGetTime(), 0.033f);

	if(hr == D3DERR_DEVICELOST ) {

		if( !deviceLost_ )
		{
			r3dOutToLog("r3d: device is lost\n");
			deviceLost_ = true;
		}

		TryToRestoreDevice();

		// this is important - do not wait for token
		return;
	}

	r3dFrameQueryCreate();
	//FlushGPU();
}



void r3dRenderLayer::EndRenderSimple( bool present /*= false*/ )
{
	// Present the backbuffer contents to the display
	if( present )
		pd3ddev->Present( NULL, NULL, NULL, NULL );
}



void r3dRenderLayer::SetRenderingMode(int Mode)
{
	if(Mode & R3D_BLEND_POP) 
	{
		if(RModeStackTop)
			SetRenderingMode(RModeStack[--RModeStackTop]);
		return;
	}

	if(Mode & R3D_BLEND_PUSH)
		RModeStack[RModeStackTop++] = CurrentRenderingMode;

	Mode &= ~(R3D_BLEND_PUSH | R3D_BLEND_POP);
	if(CurrentRenderingMode == Mode)
		return;

	CurrentRenderingMode = Mode;
	Stats.AddNumModesChanges( 1 );

	// Z Buffer Control
	if(Mode & R3D_BLEND_ZC)
		pd3ddev->SetRenderState(D3DRS_ZENABLE,	ZBuffer_Mode);
	else
		pd3ddev->SetRenderState(D3DRS_ZENABLE,	0);

	if(Mode & R3D_BLEND_ZW)
		pd3ddev->SetRenderState(D3DRS_ZWRITEENABLE, 		TRUE);
	else
		pd3ddev->SetRenderState(D3DRS_ZWRITEENABLE, 		0);

	// Blending Mode Control
	if(Mode & R3D_BLEND_NOALPHA) 
	{
		pd3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, 	0);
	} 
	else
	{
		pd3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, 	TRUE);

		if( Mode & R3D_BLEND_MAX )
		{
			pd3ddev->SetRenderState(D3DRS_SRCBLEND,			D3DBLEND_ONE );
			pd3ddev->SetRenderState(D3DRS_DESTBLEND,		D3DBLEND_ONE );
			pd3ddev->SetRenderState(D3DRS_BLENDOP,			D3DBLENDOP_MAX );
		}
		else
		if( Mode & R3D_BLEND_MIN )
		{
			pd3ddev->SetRenderState(D3DRS_SRCBLEND,			D3DBLEND_ONE );
			pd3ddev->SetRenderState(D3DRS_DESTBLEND,		D3DBLEND_ONE );
			pd3ddev->SetRenderState(D3DRS_BLENDOP,			D3DBLENDOP_MIN );
		}
		else
		{
			pd3ddev->SetRenderState(D3DRS_BLENDOP,			D3DBLENDOP_ADD );
			if(Mode & R3D_BLEND_ALPHA) 
			{
				pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	 	D3DBLEND_SRCALPHA);
				pd3ddev->SetRenderState(D3DRS_DESTBLEND, 		D3DBLEND_INVSRCALPHA);
			}
			else if(Mode & R3D_BLEND_ADD) 
			{
				pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	 	D3DBLEND_ONE);
				pd3ddev->SetRenderState(D3DRS_DESTBLEND, 		D3DBLEND_ONE);
			} 
			else if(Mode & R3D_BLEND_SUB) 
			{
				pd3ddev->SetRenderState(D3DRS_SRCBLEND, 		D3DBLEND_ZERO);
				pd3ddev->SetRenderState(D3DRS_DESTBLEND, 		D3DBLEND_INVSRCCOLOR);
			} 
			else if(Mode & R3D_BLEND_MODULATE) 
			{
				pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	 	D3DBLEND_ZERO);
				pd3ddev->SetRenderState(D3DRS_DESTBLEND, 	   	D3DBLEND_SRCCOLOR);
			}
			else if(Mode & R3D_BLEND_ADDMODULATE) 
			{
				pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	 	D3DBLEND_DESTCOLOR);
				pd3ddev->SetRenderState(D3DRS_DESTBLEND, 	   	D3DBLEND_SRCCOLOR);
			}
			else if(Mode & R3D_BLEND_COLOR)
			{
				pd3ddev->SetRenderState(D3DRS_SRCBLEND, 	 	D3DBLEND_SRCCOLOR);
				pd3ddev->SetRenderState(D3DRS_DESTBLEND, 	   	D3DBLEND_INVSRCCOLOR);
			}
		}
	}
}

int	r3dRenderLayer::GetRenderingMode() const
{
	return CurrentRenderingMode;
}

void r3dRenderLayer::ResetMaterial()
{
	// First Reset all rendering modes for stages
	SetTex(NULL,0);
	SetTex(NULL,1);
	SetTex(NULL,2);
	SetTex(NULL,3);
	SetTex(NULL,4);
	SetTex(NULL,5);
	SetTex(NULL,6);
	SetTex(NULL,7);

	CurrentTexID[ 0 ] = (IDirect3DBaseTexture9*)-1;
	CurrentTexID[ 1 ] = (IDirect3DBaseTexture9*)-1;
	CurrentTexID[ 2 ] = (IDirect3DBaseTexture9*)-1;
	CurrentTexID[ 3 ] = (IDirect3DBaseTexture9*)-1;
	CurrentTexID[ 4 ] = (IDirect3DBaseTexture9*)-1;
	CurrentTexID[ 5 ] = (IDirect3DBaseTexture9*)-1;
	CurrentTexID[ 6 ] = (IDirect3DBaseTexture9*)-1;
	CurrentTexID[ 7 ] = (IDirect3DBaseTexture9*)-1;

	SetVertexShader(-1);
	SetPixelShader(-1);
}


void r3dRenderLayer::SetMaterial(r3dMaterial *Mat)
{
	if(Mat == NULL)
	{
		r3dMaterial::ResetMaterialFilter();
		CurrentMaterialID = -999;

		ResetMaterial();

		SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
		pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		pd3ddev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		RestoreCullMode();

		pd3ddev->SetRenderState(D3DRS_ALPHAREF, 1);
		Stats.AddNumMaterialChanges( 1 );

		return;
	}


	// if(Mat->ID == CurrentMaterialID)
	//    return;

	// Flush();

	CurrentMaterialID = Mat->ID;
	Stats.AddNumMaterialChanges( 1 );

	ResetMaterial();

	SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	SetThumbTex(Mat->Texture);

	// if (Mat->Flags & R3D_MAT_TEXTURE)
	//	if (!Mat->Texture->bHaveAlpha)
	//      SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );


	if(Mat->Flags & R3D_MAT_ADDOP)    SetRenderingMode(R3D_BLEND_ADD_ZC);
	if(Mat->Flags & R3D_MAT_SUBOP)    SetRenderingMode(R3D_BLEND_SUB_ZC);

	pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3ddev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	pd3ddev->SetRenderState(D3DRS_ALPHAREF, (unsigned long)Mat->AlphaRef);
}


#ifdef SetTexture
#undef SetTexture
#endif

R3D_FORCEINLINE void r3dRenderLayer::SetTex_( IDirect3DBaseTexture9* d3dTex, int StageID )
{
	if ( StageID >= D3DVERTEXTEXTURESAMPLER0 ) 
	{
		D3D_V( pd3ddev->SetTexture( StageID, d3dTex ) );
		return;
	}

	if( d3dTex == CurrentTexID[StageID] )
		return;

	if( CurrentTexID[StageID] != d3dTex )
	{
		Stats.AddNumTextureChanges( 1 );
		CurrentTexID[StageID] =d3dTex;
		D3D_V( pd3ddev->SetTexture(StageID, d3dTex) );
	}
}

void r3dRenderLayer::SetTex(r3dTexture *Tex, int StageID)
{
	SetTex_( Tex ? Tex->GetD3DTexture() : NULL, StageID );
}

void r3dRenderLayer::SetThumbTex(r3dThumbnailTexture* Tex, int StageID/*=0*/)
{
	SetTex_( Tex ? Tex->GetD3DTex() : NULL, StageID );
}

void r3dRenderLayer::Render2DPolygon(int NumV, R3D_SCREEN_VERTEX *V)
{
	vtxArray2D->Add(V, NumV);
}
void r3dRenderLayer::Begin2DPolygon(int NumVertsInPoly, int numPoly)
{
	vtxArray2D->StartVertices(NumVertsInPoly, numPoly);
}
void r3dRenderLayer::Fill2DPolygon(int NumV, R3D_SCREEN_VERTEX *V)
{
	vtxArray2D->SetVtx(V, NumV);
}
void r3dRenderLayer::End2DPolygon()
{
	vtxArray2D->EndVertices();
}



void r3dRenderLayer::Render3DPolygon(int NumV, R3D_DEBUG_VERTEX *V)
{
	vtxArray3D->Add(V, NumV);
}
void r3dRenderLayer::BeginFill3DPolygon(int NumV)
{
	vtxArray3D->StartListVertices(NumV);
}
void r3dRenderLayer::Fill3DPolygon(int NumV, R3D_DEBUG_VERTEX *V)
{
	vtxArray3D->SetListVtx(V, NumV);
}
void r3dRenderLayer::EndFill3DPolygon()
{
	vtxArray3D->EndListVertices();
}
int r3dRenderLayer::GetPolygon3DBufferCapacity()
{
	return vtxArray3D->GetCapacity();
}

int r3dRenderLayer::Get3DLineMaxVertexCount()
{
	return vtxArray3D_Lines->GetCapacity();
}


void r3dRenderLayer::Render3DLine(int NumV, R3D_DEBUG_VERTEX *V)
{
	vtxArray3D_Lines->AddList(V, NumV);
}
void r3dRenderLayer::BeginFill3DLine(int NumV)
{
	vtxArray3D_Lines->StartListVertices(NumV);
}
void r3dRenderLayer::Fill3DLine(int NumV, R3D_DEBUG_VERTEX *V)
{
	vtxArray3D_Lines->SetListVtx(V, NumV);
}
void r3dRenderLayer::EndFill3DLine()
{
	vtxArray3D_Lines->EndListVertices();
}




void r3dRenderLayer::Render3DTriangles(int NumV, R3D_DEBUG_VERTEX *V)
{
	vtxArray3D->AddList( V, NumV );
}

void r3dRenderLayer::SetForceBlackPixelShader( int bForceBlackShader )
{
	if( bForceBlackShader )
	{
		if( ForceBlackPixelShaderId < 0 )
		{
			ForceBlackPixelShaderId = r3dRenderer->GetPixelShaderIdx( "PS_DARKNESS" );
		}

		if( ForceBlackPixelShaderId >= 0 )
		{
			ForceBlackPixelShader = 1;
		}
	}
	else
	{
		ForceBlackPixelShader = 0;
	}
}

void r3dRenderLayer::UnloadUnmanagedTextures()
{
	for( r3dTexture* Tex = FirstTexture; Tex; Tex = Tex->pNext)
	{
		if( Tex->GetPool() == D3DPOOL_DEFAULT && !( Tex->GetFlags() & r3dTexture::fCreated ) )
		{
			r3d_assert( !Tex->PendingAsyncOps() );

			if( Tex->GetRefs() )
			{
				Tex->SetNeedReload( 1 );
				Tex->DestroyResources( false );
			}
		}
	}

	for( int i = 0, e = (int)ThumbnailTextures.Count(); i < e; i ++ )
	{
		ThumbnailTextures[ i ]->ReleasePoolDefaultThumnail();
	}
}

void r3dRenderLayer::LoadUnmanagedTextures()
{
	int prevAsyncLoading = g_async_loading->GetInt();
	g_async_loading->SetInt( 0 );

	for( r3dTexture* Tex = FirstTexture; Tex; Tex = Tex->pNext)
	{
		if( Tex->GetPool() == D3DPOOL_DEFAULT && !( Tex->GetFlags() & r3dTexture::fCreated ) && Tex->NeedsReload() )
		{
			Tex->LoadResources();
			Tex->SetNeedReload( 0 );
		}
	}

	g_async_loading->SetInt( prevAsyncLoading );

	for( int i = 0, e = (int)ThumbnailTextures.Count(); i < e; i ++ )
	{
		ThumbnailTextures[ i ]->RecreatePoolDefaultThumbnail();
	}
}

void r3dMaterial::RenderPrimitive(int Start1, int End1, int Start2, int End2)
{
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0,Start1, End1, Start2, End2 );
}


R3D_FORCEINLINE VisibilityInfoEnum r3dRenderLayer::IsSphereInsideFrustum_NORMAL(const r3dPoint3D& c, float r, const D3DXPLANE (&planes)[6])
{
	float fDistance = 0;

	VisibilityInfoEnum result = VI_INSIDE;

	for(int i = 0; i < 6; ++i) 
	{
		fDistance = planes[i].a * c.x + planes[i].b * c.y + planes[i].c * c.z + planes[i].d;
		if (fDistance < -r)
			return VI_OUTSIDE;
		if (fDistance < r)
			result = VI_INTERSECTING;
	}

	return result;
}

R3D_FORCEINLINE VisibilityInfoEnum r3dRenderLayer::IsSphereInsideFrustum_PARABOLOID(const r3dPoint3D& c, float r, const D3DXPLANE (&planes)[6])
{
	float fDistance;

	fDistance = planes[0].a * c.x + planes[0].b * c.y + planes[0].c * c.z + planes[0].d;
	if(fDistance < -r)
		return VI_OUTSIDE;

	float centreDistance = ( CameraPosition - c ).Length() ;

	if( centreDistance > this->FarClip + r )
		return VI_OUTSIDE ;

	return VI_INSIDE ;
}

//////////////////////////////////////////////////////////////////////////

VisibilityInfoEnum r3dRenderLayer::IsSphereInsideCustomFrustum(const r3dPoint3D& c, float r, const D3DXPLANE (&planes)[6])
{
	return IsSphereInsideFrustum_NORMAL( c, r, planes ) ;
}

//////////////////////////////////////////////////////////////////////////

VisibilityInfoEnum r3dRenderLayer::IsSphereInsideFrustum(const r3dPoint3D& c, float r)
{
	switch( FrustumType )
	{
	case R3D_FRUSTUM_NORMAL:
		return IsSphereInsideFrustum_NORMAL( c, r, FrustumPlanes ) ;
	case R3D_FRUSTUM_PARABOLOID:
		return IsSphereInsideFrustum_PARABOLOID( c, r, FrustumPlanes ) ;
	}

	r3d_assert( false ) ;
	return VI_INSIDE ;
}

R3D_FORCEINLINE VisibilityInfoEnum r3dRenderLayer::IsBoxInsideFrustum_NORMAL(const r3dBoundBox& BBox, const D3DXPLANE (&planes)[6])
{
	VisibilityInfoEnum res = IsSphereInsideCustomFrustum( BBox.Org+BBox.Size*0.5f, 0.5f * sqrtf( BBox.Size.x*BBox.Size.x + BBox.Size.y*BBox.Size.y + BBox.Size.z*BBox.Size.z ), planes );
	if (res == VI_INTERSECTING)
	{
		bool intersect = false;
		for (uint32_t i = 0; i < _countof(planes); ++i)
		{
			res = IsBoxOnPositiveHalfSpace(planes[i], BBox);
			//	Frustum planes point outwards, so there is flip of results here
			if (res == VI_INSIDE)
				return VI_OUTSIDE;
			else if(res == VI_INTERSECTING)
				intersect = true;
		}
		if (intersect)
			res = VI_INTERSECTING;
		else
			res = VI_INSIDE;
	}
	return res;
}

R3D_FORCEINLINE VisibilityInfoEnum r3dRenderLayer::IsBoxInsideFrustum_PARABOLOID( const r3dBoundBox& BBox )
{
	r3dPoint3D pVecBounds[8];
	BYTE bOutside[8];

	ZeroMemory( &bOutside, sizeof(bOutside) );

#define DVX(v) r3dPoint3D(v.x, v.y, v.z)

	pVecBounds[0] = DVX(BBox.Org);

	pVecBounds[1] = DVX(BBox.Org);
	pVecBounds[1].x += BBox.Size.X;

	pVecBounds[2] = DVX(BBox.Org);
	pVecBounds[2].z += BBox.Size.Z;

	pVecBounds[3] = DVX(BBox.Org);
	pVecBounds[3].x += BBox.Size.X;
	pVecBounds[3].z += BBox.Size.Z;

	pVecBounds[4] = DVX(BBox.Org);
	pVecBounds[4].y += BBox.Size.Y;

	pVecBounds[5] = DVX(BBox.Org);
	pVecBounds[5].y += BBox.Size.Y;
	pVecBounds[5].x += BBox.Size.X;

	pVecBounds[6] = DVX(BBox.Org);
	pVecBounds[6].y += BBox.Size.Y;
	pVecBounds[6].z += BBox.Size.Z;

	pVecBounds[7] = DVX(BBox.Org);
	pVecBounds[7].y += BBox.Size.Y;
	pVecBounds[7].x += BBox.Size.X;
	pVecBounds[7].z += BBox.Size.Z;

#undef DVX

	// Check boundary vertices against NEAR plane 
	for( int iPoint = 0; iPoint < 8; iPoint++ )
	{
		float centreDist = ( pVecBounds[ iPoint ] - CameraPosition ).Length() ;

		if( FrustumPlanes[ 0 ].a * pVecBounds[iPoint].x +
			FrustumPlanes[ 0 ].b * pVecBounds[iPoint].y +
			FrustumPlanes[ 0 ].c * pVecBounds[iPoint].z +
			FrustumPlanes[ 0 ].d >= 0 
			 )
		{
			bOutside[ iPoint ] = 0;
		}
		else
		{
			bOutside[ iPoint ] = 1;
		}
	}

	int allOutside = 1;

	for( int i = 0; i < 8; i ++ )
	{
		if( !bOutside[ i ] )
		{
			allOutside = 0;
			break;
		}
	}

	if( allOutside )
		return VI_OUTSIDE;
	else
		return VI_INTERSECTING;
}

//////////////////////////////////////////////////////////////////////////

VisibilityInfoEnum r3dRenderLayer::IsBoxInsideCustomFrustum(const r3dBoundBox& BBox, const D3DXPLANE (&planes)[6])
{
	return IsBoxInsideFrustum_NORMAL(BBox, planes);
}

//////////////////////////////////////////////////////////////////////////

VisibilityInfoEnum r3dRenderLayer::IsBoxInsideFrustum(const r3dBoundBox& BBox)
{
#if 0
	switch( FrustumType )
	{
	case R3D_FRUSTUM_NORMAL:
		return IsBoxInsideFrustum_NORMAL2( BBox ) ;
		break ;

	case R3D_FRUSTUM_PARABOLOID:
		return IsBoxInsideFrustum_PARABOLOID( BBox ) ;
		break ;
	}

	r3d_assert( false ) ;
	return VI_INSIDE ;
#else
	return IsBoxInsideFrustum_NORMAL(BBox, FrustumPlanes);
#endif
}

bool r3dRenderLayer::DoesBoxIntersectNearPlane(const r3dBoundBox& BBox)
{
	D3DXVECTOR3 pVecBounds[8];

#define DVX(v) D3DXVECTOR3(v.x, v.y, v.z)

	pVecBounds[0] = DVX(BBox.Org);

	pVecBounds[1] = DVX(BBox.Org);
	pVecBounds[1].x += BBox.Size.X;

	pVecBounds[2] = DVX(BBox.Org);
	pVecBounds[2].z += BBox.Size.Z;

	pVecBounds[3] = DVX(BBox.Org);
	pVecBounds[3].x += BBox.Size.X;
	pVecBounds[3].z += BBox.Size.Z;

	pVecBounds[4] = DVX(BBox.Org);
	pVecBounds[4].y += BBox.Size.Y;

	pVecBounds[5] = DVX(BBox.Org);
	pVecBounds[5].y += BBox.Size.Y;
	pVecBounds[5].x += BBox.Size.X;

	pVecBounds[6] = DVX(BBox.Org);
	pVecBounds[6].y += BBox.Size.Y;
	pVecBounds[6].z += BBox.Size.Z;

	pVecBounds[7] = DVX(BBox.Org);
	pVecBounds[7].y += BBox.Size.Y;
	pVecBounds[7].x += BBox.Size.X;
	pVecBounds[7].z += BBox.Size.Z;

	for( int iPoint = 0; iPoint < 8; iPoint++ )
	{
		if( FrustumPlanes[0].a * pVecBounds[iPoint].x +
			FrustumPlanes[0].b * pVecBounds[iPoint].y +
			FrustumPlanes[0].c * pVecBounds[iPoint].z +
			FrustumPlanes[0].d < 0)
		{
			return true; // point outside of near plane
		}
	}
	return false;
}

void r3dRenderLayer::MarkEssentialD3DCommand()
{
	IssuePendingQuery() ;
	ActiveCommandCount ++ ;
}

void r3dRenderLayer::IssuePendingQuery()
{
	if( PendingQuery )
	{
		D3D_V( PendingQuery->Issue( D3DISSUE_END ) );

		PendingQuery = NULL ;
	}
}

void
r3dRenderLayer::SetPendingQuery( IDirect3DQuery9* query )
{
	IssuePendingQuery();

	LastIssueActiveCommandsCount	= ActiveCommandCount ;
	PendingQuery					= query ;
}

//------------------------------------------------------------------------

void
r3dRenderLayer::ResetQueryCounters()
{
	ActiveCommandCount				= 0;
	LastIssueActiveCommandsCount	= 0xffffffff;
}

//------------------------------------------------------------------------

void
r3dRenderLayer::UpdateShaderProfiles()
{
	sprintf( VertexShaderProfileName, R3D_VSHADERPROFILE_DEFAULT);
	sprintf( PixelShaderProfileName, R3D_PSHADERPROFILE_DEFAULT);

	r_instanced_particles->SetInt( 1 ) ;
}

//------------------------------------------------------------------------

void r3dRenderLayer::CheckOutOfMemory( HRESULT hr )
{
	if( hr == D3DERR_OUTOFVIDEOMEMORY )
	{
		r_out_of_vmem_encountered->SetInt( 1 );
		HWND targ = HLibWin;
		if( r_fullscreen->GetInt() )
		{
			targ = NULL;
			ShowWindow(HLibWin, SW_MINIMIZE);
			r3dProcessWindowMessages();
		}
		MessageBoxA( targ, "Out of video memory!", "Error", MB_ICONERROR | MB_SYSTEMMODAL );
		TerminateProcess(r3d_CurrentProcess, 0);
	}

	if( hr == E_OUTOFMEMORY )
	{
		// free some memory - maybe it will help the message box!
		r3dTL::TArray<r3dIResource*>().Swap( resources_ );
		ResetShaders();
		FlushTextures();

		HWND targ = HLibWin;
		if( r_fullscreen->GetInt() )
		{
			targ = NULL;
			ShowWindow(HLibWin, SW_MINIMIZE);
			r3dProcessWindowMessages();
		}
		MessageBoxA( targ, "Out of system memory!", "Error", MB_ICONERROR | MB_SYSTEMMODAL );
		TerminateProcess(r3d_CurrentProcess, 0);
	}
}

//------------------------------------------------------------------------

IDirect3DSurface9* r3dRenderLayer::GetTempSurfaceForScreenShots()
{
	if( !sysMemSurfaceForScreenShots )
	{
		pd3ddev->CreateOffscreenPlainSurface(d3dpp.BackBufferWidth, d3dpp.BackBufferHeight, d3dpp.BackBufferFormat, D3DPOOL_SYSTEMMEM, &sysMemSurfaceForScreenShots, 0);
	}
	return sysMemSurfaceForScreenShots;
}

//------------------------------------------------------------------------

IDirect3DTexture9* r3dRenderLayer::GetTempTextureForFrontBufferCopy()
{
	if( !sysMemTextureForFrontBufferCopy )
	{		
		RECT rc;
		GetWindowRect( GetDesktopWindow(), &rc );

		D3D_V( r3dRenderer->pd3ddev->CreateTexture( rc.right - rc.left, rc.bottom - rc.top, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &sysMemTextureForFrontBufferCopy, NULL ) );
	}

	return sysMemTextureForFrontBufferCopy;
}

//------------------------------------------------------------------------

r3dFrustumType
r3dRenderLayer::GetFrustumType() const
{
	return FrustumType ;
}

//------------------------------------------------------------------------

void
r3dRenderLayer::SetFrustumType( r3dFrustumType type )
{
	FrustumType = type ;
}

//------------------------------------------------------------------------

void r3dRenderLayer::AquireMainRT()
{
	SAFE_RELEASE( MainRT );
	D3D_V( pd3ddev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &MainRT ) );

	D3DSURFACE_DESC sdesc ;
	D3D_V( MainRT->GetDesc( &sdesc) );

	SAFE_RELEASE( MainDSS );

	int dsWidth = sdesc.Width ;
	int dsHeight = sdesc.Height ;

	if( r_internal_width->GetInt() )
	{
		dsWidth = r_internal_width->GetInt() ;
	}

	if( r_internal_height->GetInt() )
	{
		dsHeight = r_internal_height->GetInt() ;
	}

	dsWidth = R3D_MAX( dsWidth, (int)sdesc.Width ) ;
	dsHeight = R3D_MAX( dsHeight, (int)sdesc.Height ) ;

	D3D_V( pd3ddev->CreateDepthStencilSurface( dsWidth, dsHeight, MainDSSFormat, D3DMULTISAMPLE_NONE, 1, FALSE, &MainDSS, NULL ) );
	void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);
	SetD3DResourcePrivateData(MainDSS, "Main DSS");

	DSS = NULL ;
	SetDSS( MainDSS );

}

//------------------------------------------------------------------------


void r3dRenderLayer::SetVertexDecl( IDirect3DVertexDeclaration9* decl ) const
{
	d3dc._SetDecl( decl ) ;
}

void r3dRenderLayer::FlushGPU()
{
	if(SupportsEventQueries && pQueryEvent[eventCounter])
	{
		R3DPROFILE_START("SyncGPUtoCPU(3frames)")
			pQueryEvent[eventCounter]->Issue(D3DISSUE_END);
		eventCounter = (eventCounter+1)%3;
		while(S_FALSE == pQueryEvent[eventCounter]->GetData(0, 0, D3DGETDATA_FLUSH)) {};
		R3DPROFILE_END("SyncGPUtoCPU(3frames)")
	}
}

//------------------------------------------------------------------------


#if R3D_DEBUG_MULTITHREADING

//------------------------------------------------------------------------

r3dRenderLayer::DeviceWrapper::DeviceWrapper()
: mDev( NULL )
{

}

//------------------------------------------------------------------------

IDirect3DDevice9*
r3dRenderLayer::DeviceWrapper::operator->() const
{
	R3D_ENSURE_MAIN_THREAD();
	r3d_assert( r3dRenderer->DeviceAvailable );
	return mDev;
}

//------------------------------------------------------------------------

r3dRenderLayer::DeviceWrapper::operator IDirect3DDevice9*() const
{
	R3D_ENSURE_MAIN_THREAD();
	r3d_assert( r3dRenderer->DeviceAvailable );
	return mDev;
}

void
r3dRenderLayer::DeviceWrapper::Set( IDirect3DDevice9* dev)
{
	mDev = dev;
}

#endif
//------------------------------------------------------------------------

void ClearBackBufferFringes()
{
	float X, Y, W, H ;

	r3dRenderer->GetBackBufferViewport( &X, &Y, &W, &H );

	LONG	lx	= (LONG) X,
		ly	= (LONG) Y,
		lw	= (LONG) W,
		lh	= (LONG) H ;

	D3DRECT rects[ 2 ];

	bool needClear = false ;

	float actualAspect = (float)r3dRenderer->d3dpp.BackBufferWidth / r3dRenderer->d3dpp.BackBufferHeight ;

	if( r_force_aspect->GetFloat() > 0.0f )
	{
		const int PARANOID_DELTA = 4;

		if( IsAspectLessThanForceAspect( actualAspect ) )
		{
			rects[ 0 ].x1 = 0 ;
			rects[ 0 ].y1 = 0 ;

			rects[ 0 ].x2 = r3dRenderer->d3dpp.BackBufferWidth ;
			rects[ 0 ].y2 = ly + PARANOID_DELTA;

			rects[ 1 ].x1 = 0 ;
			rects[ 1 ].y1 = r3dRenderer->d3dpp.BackBufferHeight - ly - PARANOID_DELTA ;

			rects[ 1 ].x2 = r3dRenderer->d3dpp.BackBufferWidth ;
			rects[ 1 ].y2 = r3dRenderer->d3dpp.BackBufferHeight ;

			needClear = true ;
		}
		else
			if( IsAspectGreaterThanForceAspect( actualAspect ) )
			{
				rects[ 0 ].x1 = 0 ;
				rects[ 0 ].y1 = 0 ;

				rects[ 0 ].x2 = lx + PARANOID_DELTA ;
				rects[ 0 ].y2 = r3dRenderer->d3dpp.BackBufferHeight ;

				rects[ 1 ].x1 = r3dRenderer->d3dpp.BackBufferWidth - lx - PARANOID_DELTA ;
				rects[ 1 ].y1 = 0 ;

				rects[ 1 ].x2 = r3dRenderer->d3dpp.BackBufferWidth ;
				rects[ 1 ].y2 = r3dRenderer->d3dpp.BackBufferHeight ;

				needClear = true ;
			}
	}

	if( needClear )
	{
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE );
		r3dRenderer->SetViewport( 0.f, 0.f, (float)r3dRenderer->d3dpp.BackBufferWidth, (float)r3dRenderer->d3dpp.BackBufferHeight );
		D3D_V( r3dRenderer->pd3ddev->Clear( R3D_ARRAYSIZE(rects), rects, D3DCLEAR_TARGET, 0, r3dRenderer->GetClearZValue(), 0 ) );
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE );	
	}
}

void r3dGetDesktopDimmensions( int * oWidth, int* oHeight )
{
	RECT rect;
	GetWindowRect( GetDesktopWindow(), &rect );

	*oWidth = rect.right - rect.left ;
	*oHeight = rect.bottom - rect.top ;
}


void r3dProcessWindowMessages()
{
	MSG msg;
	while(PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(GetMessageW(&msg, NULL, 0, 0)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void DoIncreaseIntegerityCounter( void* )
{
	r3d_assert( gIntegrityCounter >= 0 );

	gIntegrityCounter ++ ;
}

r3dIntegrityGuardian::r3dIntegrityGuardian()
{
	ProcessCustomDeviceQueueItem( DoIncreaseIntegerityCounter, 0 );
}

void DoDecreaseIntegerityCounter( void* )
{
	r3d_assert( gIntegrityCounter > 0 );

	gIntegrityCounter -- ;
}

r3dIntegrityGuardian::~r3dIntegrityGuardian()
{
	ProcessCustomDeviceQueueItem( DoDecreaseIntegerityCounter, 0 );
}


r3dIResource::r3dIResource( const r3dIntegrityGuardian& ig )
{ 
	r3d_assert(r3dRenderer);
	if( R3D_IS_MAIN_THREAD() )
	{
		r3dRenderer->RegisterResource(this);
	}
	else
	{
		DeviceQueueItem dqi ;

		dqi.Type = DeviceQueueItem::DEFAULT_POOL_RESOURCE_REGISTER ;
		dqi.DefPoolRes.Resource = this ;

		AddDeviceQueueItem( dqi ) ;
	}
}

/*virtual*/
r3dIResource::~r3dIResource() 
{
	R3D_ENSURE_MAIN_THREAD() ;
	r3dRenderer->DeregisterResource(this); 
}

//------------------------------------------------------------------------

static void DoReleaseAndReset( void* param )
{
	r3dD3DResourceTunnel* tunnel = (r3dD3DResourceTunnel*) param ;

	tunnel->ReleaseAndReset();
}

void r3dD3DResourceTunnel::ReleaseAndReset()
{
	if( R3D_IS_MAIN_THREAD() )
	{
		if( mRes )
			mRes->Release() ;

		mRes = 0 ;
	}
	else
	{
		ProcessCustomDeviceQueueItem( DoReleaseAndReset, this );
	}
}

//------------------------------------------------------------------------

struct VLockParams
{
	UINT OffsetToLock ;
	UINT SizeToLock ;
	VOID **ppData ;
	DWORD Flags ;

	r3dD3DVertexBufferTunnel Tunnel ;
};

static void DoVLock( void* params )
{
	VLockParams* pms = ( VLockParams* ) params ;

	D3D_V( pms->Tunnel->Lock( pms->OffsetToLock, pms->SizeToLock, pms->ppData, pms->Flags ) );

	r3d_assert( gLockCounter >= 0 );

	gLockCounter ++ ;
}

void r3dD3DVertexBufferTunnel::Lock( UINT OffsetToLock, UINT SizeToLock, VOID **ppData, DWORD Flags )
{
	VLockParams params ;

	params.OffsetToLock = OffsetToLock ;
	params.SizeToLock = SizeToLock ;
	params.ppData = ppData ;
	params.Flags = Flags ;
	params.Tunnel = *this ;

	ProcessCustomDeviceQueueItem( DoVLock, &params ) ;
}

//------------------------------------------------------------------------

static void DoVUnlock( void* params )
{
	r3dD3DVertexBufferTunnel* tunnel = (r3dD3DVertexBufferTunnel*) params ;
	(*tunnel)->Unlock();

	r3d_assert( gLockCounter > 0 );

	gLockCounter -- ;
}

void r3dD3DVertexBufferTunnel::Unlock()
{
	ProcessCustomDeviceQueueItem( DoVUnlock, this ) ;
}

//------------------------------------------------------------------------

struct ILockParams
{
	UINT OffsetToLock ;
	UINT SizeToLock ;
	VOID **ppData ;
	DWORD Flags ;

	r3dD3DIndexBufferTunnel Tunnel ;
};

static void DoILock( void* params )
{
	ILockParams* pms = ( ILockParams* ) params ;

	D3D_V( pms->Tunnel->Lock( pms->OffsetToLock, pms->SizeToLock, pms->ppData, pms->Flags ) );

	r3d_assert( gLockCounter >= 0 );

	gLockCounter ++ ;
}

void r3dD3DIndexBufferTunnel::Lock( UINT OffsetToLock, UINT SizeToLock, VOID **ppData, DWORD Flags )
{
	ILockParams params ;

	params.OffsetToLock = OffsetToLock ;
	params.SizeToLock = SizeToLock ;
	params.ppData = ppData ;
	params.Flags = Flags ;
	params.Tunnel = *this ;

	ProcessCustomDeviceQueueItem( DoILock, &params ) ;
}

//------------------------------------------------------------------------

static void DoIUnlock( void* params )
{
	r3dD3DIndexBufferTunnel* tunnel = (r3dD3DIndexBufferTunnel*) params ;
	(*tunnel)->Unlock();

	r3d_assert( gLockCounter > 0 );

	gLockCounter -- ;
}

void r3dD3DIndexBufferTunnel::Unlock()
{
	ProcessCustomDeviceQueueItem( DoIUnlock, this ) ;
}

//------------------------------------------------------------------------

struct CreateTextureParamsAutoDownScale
{
	UINT *Width ;
	UINT *Height ;
	UINT *Levels ;
	DWORD Usage ;
	D3DFORMAT Format ;
	D3DPOOL Pool ;
	r3dD3DTextureTunnel* TextureTunnel ;
};

static void DoCreateTextureADS( void* params )
{
	CreateTextureParamsAutoDownScale* pms = (CreateTextureParamsAutoDownScale*) params ;

	IDirect3DTexture9* tex ;

	int W = *pms->Width ;
	int H = *pms->Height ;
	int L = *pms->Levels ;

	for( ; ; )
	{
		HRESULT hres = r3dRenderer->pd3ddev->CreateTexture ( W, H, L, pms->Usage, pms->Format, pms->Pool, &tex, 0 ) ;

		r3dRenderer->CheckOutOfMemory( hres ) ;

		if( hres == S_OK )
			break ;

		if( hres == D3DERR_OUTOFVIDEOMEMORY || 
			hres == E_OUTOFMEMORY )
		{
			W /= 2 ;
			H /= 2 ;

			if( L > 1 )
				L -- ;

			if( !W || !H )
			{
				r3dError( "DoCreateTextureADS: reached zero texture dimmensions while downscaling! Original size was: %dx%d", *pms->Width, *pms->Height ) ;
			}
		}		
	}

	*pms->Width		= W ;
	*pms->Height	= H ;
	*pms->Levels	= L ;
	
	pms->TextureTunnel->Set( tex ) ;
}


/*static*/
void
r3dDeviceTunnel::CreateTextureAutoDownScale( UINT* Width, UINT* Height, UINT* Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel  )
{
	CreateTextureParamsAutoDownScale parms ;

	parms.Width			= Width ;
	parms.Height		= Height ;
	parms.Levels		= Levels ;
	parms.Usage			= Usage ;
	parms.Format		= Format ;
	parms.Pool			= Pool ;
	parms.TextureTunnel  = TextureTunnel ;	

	ProcessCustomDeviceQueueItem( DoCreateTextureADS, &parms ) ;
	
}

//------------------------------------------------------------------------

struct CreateTextureParams
{
	UINT Width ;
	UINT Height ;
	UINT Levels ;
	DWORD Usage ;
	D3DFORMAT Format ;
	D3DPOOL Pool ;
	r3dD3DTextureTunnel* TextureTunnel ;
};

const char* PoolToString( D3DPOOL pool )
{
	switch( pool )
	{
	case D3DPOOL_MANAGED:
		return "D3DPOOL_MANAGED" ;
	case D3DPOOL_SYSTEMMEM:
		return "D3DPOOL_SYSTEMMEM" ;
	case D3DPOOL_SCRATCH:
		return "SCRATCH";
	case D3DPOOL_DEFAULT:
		return "D3DPOOL_DEFAULT" ;
	default:
		return "UNKOWN" ;
	}

}

static void DoCreateTexture( void* params )
{
	CreateTextureParams* pms = (CreateTextureParams*) params ;

	IDirect3DTexture9* tex ;

	HRESULT hr = r3dRenderer->pd3ddev->CreateTexture ( pms->Width, pms->Height, pms->Levels, pms->Usage, pms->Format, pms->Pool, &tex, 0 ) ;

	r3dRenderer->CheckOutOfMemory( hr ) ;

	if( hr != S_OK )
	{
		const char* Reason = "GENERIC" ;

		if( hr == D3DERR_OUTOFVIDEOMEMORY )
		{
			Reason = "OUT OF VIDEO MEMORY" ;
		}

		if( hr == E_OUTOFMEMORY )
		{
			Reason = "OUT OF SYSTEM MEMORY" ;
		}

		r3dError( "Failed to create texture of format %s, %dx%d in pool %s because of %s error (%d)", 
						PackedFmtToString( PackD3DFormat( pms->Format ) ),
						pms->Width, pms->Height,
						PoolToString( pms->Pool ),
						Reason,
						hr
						);
	}

	pms->TextureTunnel->Set( tex ) ;
}

void
r3dDeviceTunnel::CreateTexture( UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel )
{
	CreateTextureParams parms ;

	parms.Width			= Width ;
	parms.Height		= Height ;
	parms.Levels		= Levels ;
	parms.Usage			= Usage ;
	parms.Format		= Format ;
	parms.Pool			= Pool ;
	parms.TextureTunnel  = TextureTunnel ;	

	ProcessCustomDeviceQueueItem( DoCreateTexture, &parms ) ;
}

//------------------------------------------------------------------------

struct CreateVolumeTextureParams
{
	UINT Width ;
	UINT Height ;
	UINT Depth ;
	UINT Levels ;
	DWORD Usage ;
	D3DFORMAT Format ;
	D3DPOOL Pool ;
	r3dD3DTextureTunnel* TextureTunnel ;
};

static void DoCreateVolumeTexture( void* params )
{
	CreateVolumeTextureParams* pms = (CreateVolumeTextureParams*) params ;

	IDirect3DVolumeTexture9* tex ;

	D3D_V( r3dRenderer->pd3ddev->CreateVolumeTexture ( pms->Width, pms->Height, pms->Depth, pms->Levels, pms->Usage, pms->Format, pms->Pool, &tex, 0 ) );

	pms->TextureTunnel->Set( tex ) ;
}

void r3dDeviceTunnel::CreateVolumeTexture( UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel )
{
	CreateVolumeTextureParams parms ;

	parms.Width			= Width ;
	parms.Height		= Height ;
	parms.Depth			= Depth ;
	parms.Levels		= Levels ;
	parms.Usage			= Usage ;
	parms.Format		= Format ;
	parms.Pool			= Pool ;
	parms.TextureTunnel  = TextureTunnel ;	

	ProcessCustomDeviceQueueItem( DoCreateVolumeTexture, &parms ) ;
}

//------------------------------------------------------------------------

struct CreateCubeTextureParams
{
	UINT EdgeLength ;
	UINT Levels ;
	DWORD Usage ;
	D3DFORMAT Format ;
	D3DPOOL Pool ;
	r3dD3DTextureTunnel* TextureTunnel ;
};

static void DoCreateCubeTexture( void* params )
{
	CreateCubeTextureParams* pms = (CreateCubeTextureParams*) params ;

	IDirect3DCubeTexture9* tex ;

	D3D_V( r3dRenderer->pd3ddev->CreateCubeTexture ( pms->EdgeLength, pms->Levels, pms->Usage, pms->Format, pms->Pool, &tex, 0 ) );

	pms->TextureTunnel->Set( tex ) ;
}

void r3dDeviceTunnel::CreateCubeTexture( UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel )
{
	CreateCubeTextureParams parms ;

	parms.EdgeLength	= EdgeLength ;
	parms.Levels		= Levels ;
	parms.Usage			= Usage ;
	parms.Format		= Format ;
	parms.Pool			= Pool ;
	parms.TextureTunnel  = TextureTunnel ;	

	ProcessCustomDeviceQueueItem( DoCreateCubeTexture, &parms )  ;
}


//------------------------------------------------------------------------

struct CreateVertexBufferParams
{
	UINT Length ;
	DWORD Usage ;
	DWORD FVF ;
	D3DPOOL Pool ;
	r3dD3DVertexBufferTunnel* VertexBufferTunnel ;
};

static void DoCreateVertexBuffer( void* params )
{
	CreateVertexBufferParams* pms = ( CreateVertexBufferParams* ) params ;

	IDirect3DVertexBuffer9* buf ;
	D3D_V( r3dRenderer->pd3ddev->CreateVertexBuffer( pms->Length, pms->Usage, pms->FVF, pms->Pool, &buf, 0 ) );
	pms->VertexBufferTunnel->Set( buf ) ;
}

void
r3dDeviceTunnel::CreateVertexBuffer( UINT Length, DWORD Usage,DWORD FVF,D3DPOOL Pool, r3dD3DVertexBufferTunnel* VertexBufferTunnel )
{
	CreateVertexBufferParams parms ;

	parms.Length	= Length ;
	parms.Usage		= Usage ;
	parms.FVF		= FVF ;
	parms.Pool		= Pool ;
	parms.VertexBufferTunnel = VertexBufferTunnel ;

	ProcessCustomDeviceQueueItem( DoCreateVertexBuffer, &parms ) ;
}

//------------------------------------------------------------------------

struct CreateIndexBufferParams
{
	UINT Length ;
	DWORD Usage ;
	D3DFORMAT Format ;
	D3DPOOL Pool ;
	r3dD3DIndexBufferTunnel* IndexBufferTunnel ;
};

const char* D3DErrorToString( HRESULT hr )
{
	switch( hr )
	{
	case D3DERR_INVALIDCALL:
		return "D3DERR_INVALIDCALL" ;
	case D3DERR_OUTOFVIDEOMEMORY:
		return "D3DERR_OUTOFVIDEOMEMORY" ;
	case D3DXERR_INVALIDDATA:
		return "D3DXERR_INVALIDDATA" ;
	case E_OUTOFMEMORY:
		return "E_OUTOFMEMORY" ;
	}

	return "Unknown error" ;
}

static void DoCreateIndexBuffer( void* params )
{
	CreateIndexBufferParams* pms = ( CreateIndexBufferParams* ) params ;

	IDirect3DIndexBuffer9* buf ;

	HRESULT hr = r3dRenderer->pd3ddev->CreateIndexBuffer( pms->Length, pms->Usage, pms->Format, pms->Pool, &buf, 0 ) ;
	if( hr == D3DERR_OUTOFVIDEOMEMORY )
	{
		r3dOutToLog( "CreateIndexBuffer returned D3DERR_OUTOFVIDEOMEMORY, trying to evict resources and recreate." ) ;
		D3D_V( r3dRenderer->pd3ddev->EvictManagedResources() ) ;
		D3D_V( r3dRenderer->pd3ddev->CreateIndexBuffer( pms->Length, pms->Usage, pms->Format, pms->Pool, &buf, 0 ) ) ;
	}

	if( hr != D3D_OK )
	{
		r3dError( "CreateIndexBuffer failed with error %s", D3DErrorToString( hr ) ) ;
	}

	pms->IndexBufferTunnel->Set( buf ) ;
}

void r3dDeviceTunnel::CreateIndexBuffer( UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DIndexBufferTunnel* IndexBufferTunnel )
{
	CreateIndexBufferParams parms ;

	parms.Length	= Length ;
	parms.Usage		= Usage ;
	parms.Format	= Format ;
	parms.Pool		= Pool ;
	parms.IndexBufferTunnel  = IndexBufferTunnel ;

	ProcessCustomDeviceQueueItem( DoCreateIndexBuffer, &parms ) ;
}

//------------------------------------------------------------------------

struct CreateRenderTargetParams
{
	UINT					Width ;
	UINT					Height ;
	D3DFORMAT				Format ;
	D3DMULTISAMPLE_TYPE		MultiSample ;
	DWORD					MultisampleQuality ;
	BOOL					Lockable ;
	r3dD3DSurfaceTunnel*	SurfaceTunnel ;
};

static void DoCreateRenderTarget( void* params )
{
	CreateRenderTargetParams* pms = (CreateRenderTargetParams*) params ;

	IDirect3DSurface9* surf ;

	D3D_V( r3dRenderer->pd3ddev->CreateRenderTarget( pms->Width, pms->Height, pms->Format, pms->MultiSample, pms->MultisampleQuality, pms->Lockable, &surf, 0 ) );

	pms->SurfaceTunnel->Set( surf ) ;
};

void r3dDeviceTunnel::CreateRenderTarget( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,r3dD3DSurfaceTunnel* SurfaceTunnel )
{
	CreateRenderTargetParams params ;

	params.Width				= Width ;
	params.Height				= Height ;
	params.Format				= Format ;
	params.MultiSample			= MultiSample ;
	params.MultisampleQuality	= MultisampleQuality ;
	params.Lockable				= Lockable ;
	params.SurfaceTunnel		= SurfaceTunnel ;

	ProcessCustomDeviceQueueItem( DoCreateRenderTarget, &params ) ;
}

//------------------------------------------------------------------------

struct CreateDepthStencilParams
{
	UINT					Width ;
	UINT					Height ;
	D3DFORMAT				Format ;
	D3DMULTISAMPLE_TYPE		MultiSample ;
	DWORD					MultisampleQuality ;
	BOOL					Discard ; 
	r3dD3DSurfaceTunnel*	SurfaceTunnel ;
};

static void DoCreateDepthStencil( void * params )
{
	CreateDepthStencilParams* pms = ( CreateDepthStencilParams* ) params ;

	IDirect3DSurface9* surf ;

	D3D_V( r3dRenderer->pd3ddev->CreateDepthStencilSurface( pms->Width, pms->Height, pms->Format, pms->MultiSample, pms->MultisampleQuality, pms->Discard, &surf, 0 ) ) ;

	pms->SurfaceTunnel->Set( surf );
}

void
r3dDeviceTunnel::CreateDepthStencilSurface( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard, r3dD3DSurfaceTunnel* SurfaceTunnel )
{
	CreateDepthStencilParams params ;

	params.Width				= Width ;
	params.Height				= Height ;
	params.Format				= Format ;
	params.MultiSample			= MultiSample ;
	params.MultisampleQuality	= MultisampleQuality ;
	params.Discard				= Discard ;
	params.SurfaceTunnel		= SurfaceTunnel ;

	ProcessCustomDeviceQueueItem( DoCreateDepthStencil, &params ) ;
}

//------------------------------------------------------------------------

struct CreateVDeclParams
{
	const D3DVERTEXELEMENT9*		pVertexElements ;
	IDirect3DVertexDeclaration9**	ppDecl ;
};

static void CreateVDecl( void* params )
{
	CreateVDeclParams* pms = (CreateVDeclParams*) params ;

	HRESULT hr;
	D3D_V(hr = ( r3dRenderer->pd3ddev->CreateVertexDeclaration( pms->pVertexElements, pms->ppDecl ) ));
	if(hr == S_OK)
		r3dRenderer->VertexDecls.push_back(*pms->ppDecl); // register for deletion when device will be closed
}

void r3dDeviceTunnel::CreateVertexDeclaration( const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl )
{
	CreateVDeclParams params ;

	params.pVertexElements = pVertexElements ;
	params.ppDecl = ppDecl ;

	ProcessCustomDeviceQueueItem( CreateVDecl, &params ) ;
}

//------------------------------------------------------------------------

struct D3DXCreateTextureFFIMEXParams
{
	LPCVOID				pSrcData;
	UINT				SrcDataSize;
	UINT				Width;
	UINT				Height;
	UINT				MipLevels;
	DWORD				Usage;
	D3DFORMAT			Format;
	D3DPOOL				Pool;
	DWORD				Filter;
	DWORD				MipFilter;
	D3DCOLOR			ColorKey;
	D3DXIMAGE_INFO*		pSrcInfo;
	PALETTEENTRY*		pPalette;
	r3dD3DTextureTunnel* TextureTunnel;

	const char*			pDebugName ;

	D3DXCreateTextureFFIMEXParams()
	: pDebugName( 0 )
	{

	}
};

static void DoCreateTextureFFIMEX( void* params )
{
	D3DXCreateTextureFFIMEXParams * pms = (D3DXCreateTextureFFIMEXParams*) params ;

	IDirect3DTexture9* tex ;
	
	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, pms->pSrcData, pms->SrcDataSize, pms->Width, pms->Height, pms->MipLevels, pms->Usage, pms->Format, pms->Pool, pms->Filter, pms->MipFilter, pms->ColorKey, pms->pSrcInfo, pms->pPalette, &tex ) ;

	r3dRenderer->CheckOutOfMemory( hr );

	if( hr != S_OK )
	{
		r3dError( "D3DXCreateTextureFromFileInMemoryEx: error (%d) on %s\n", hr, pms->pDebugName ) ;
	}

	pms->TextureTunnel->Set( tex ) ;
}

void r3dDeviceTunnel::D3DXCreateTextureFromFileInMemoryEx( LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, r3dD3DTextureTunnel* TextureTunnel, const char* DEBUG_NAME, bool async /*= false*/ )
{
	R3D_DEIVCE_QUEUE_OBJ( D3DXCreateTextureFFIMEXParams, parms ) ;

	parms.pSrcData		= pSrcData ;
	parms.SrcDataSize	= SrcDataSize ;
	parms.Width			= Width;
	parms.Height		= Height ;
	parms.MipLevels		= MipLevels ;
	parms.Usage			= Usage ;
	parms.Format		= Format ;
	parms.Pool			= Pool ;
	parms.Filter		= Filter ;
	parms.MipFilter		= MipFilter ;
	parms.ColorKey		= ColorKey ;
	parms.pSrcInfo		= pSrcInfo ;
	parms.pPalette		= pPalette ;
	parms.TextureTunnel = TextureTunnel ;
	parms.pDebugName	= DEBUG_NAME ;


	if( async )
	{
		r3d_assert( g_async_d3dqueue->GetInt() ) ;
		AddCustomDeviceQueueItem( DoCreateTextureFFIMEX, &parms ) ;
	}
	else
	{
		ProcessCustomDeviceQueueItem( DoCreateTextureFFIMEX, &parms ) ;	
	}
}

//------------------------------------------------------------------------

struct D3DXCreateVolumeTextureFFIMEXParams
{
	LPCVOID			pSrcData ;
	UINT			SrcDataSize ;
	UINT			Width ;
	UINT			Height ;
	UINT			Depth ;
	UINT			MipLevels ;
	DWORD			Usage ;
	D3DFORMAT		Format ;
	D3DPOOL			Pool ;
	DWORD			Filter ;
	DWORD			MipFilter ;
	D3DCOLOR		ColorKey ;
	D3DXIMAGE_INFO*	pSrcInfo ;
	PALETTEENTRY*	pPalette ;
	r3dD3DTextureTunnel* TextureTunnel ;
};

static void DoCreateVolumeTextureFFIMEX( void* params )
{
	D3DXCreateVolumeTextureFFIMEXParams * pms = (D3DXCreateVolumeTextureFFIMEXParams*) params ;

	IDirect3DVolumeTexture9* tex ;

	D3D_V( D3DXCreateVolumeTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, pms->pSrcData, pms->SrcDataSize, pms->Width, pms->Height, pms->Depth, pms->MipLevels, pms->Usage, pms->Format, pms->Pool, pms->Filter, pms->MipFilter, pms->ColorKey, pms->pSrcInfo, pms->pPalette, &tex ) );

	pms->TextureTunnel->Set( tex ) ;
}

void r3dDeviceTunnel::D3DXCreateVolumeTextureFromFileInMemoryEx( LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT Depth, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, r3dD3DTextureTunnel* TextureTunnel, bool async /*= false*/ )
{
	R3D_DEIVCE_QUEUE_OBJ( D3DXCreateVolumeTextureFFIMEXParams, params ) ;

	params.pSrcData			= pSrcData ;
	params.SrcDataSize		= SrcDataSize ;
	params.Width			= Width ;
	params.Height			= Height ;
	params.Depth			= Depth ;
	params.MipLevels		= MipLevels ;
	params.Usage			= Usage ;
	params.Format			= Format ;
	params.Pool				= Pool ;
	params.Filter			= Filter ;
	params.MipFilter		= MipFilter ;
	params.ColorKey			= ColorKey ;
	params.pSrcInfo			= pSrcInfo ;
	params.pPalette			= pPalette ;
	params.TextureTunnel	= TextureTunnel ;

	if( async )
	{
		r3d_assert( g_async_d3dqueue->GetInt() ) ;
		AddCustomDeviceQueueItem( DoCreateVolumeTextureFFIMEX, &params ) ;	
	}
	else
	{
		ProcessCustomDeviceQueueItem( DoCreateVolumeTextureFFIMEX, &params ) ;	
	}

}

//------------------------------------------------------------------------

struct D3DXCreateCubeTextureFFIMEXParams
{
	LPCVOID					pSrcData ;
	UINT					SrcDataSize ;
	UINT					Size ;
	UINT					MipLevels ;
	DWORD					Usage ;
	D3DFORMAT				Format ;
	D3DPOOL					Pool ;
	DWORD					Filter ;
	DWORD					MipFilter ;
	D3DCOLOR				ColorKey ;
	D3DXIMAGE_INFO*			pSrcInfo ;
	PALETTEENTRY*			pPalette ;
	r3dD3DTextureTunnel*	TextureTunnel ;
};

static void DoCreateCubeTextureFFIMEX( void* params )
{
	D3DXCreateCubeTextureFFIMEXParams * pms = (D3DXCreateCubeTextureFFIMEXParams*) params ;

	IDirect3DCubeTexture9* tex ;

	D3D_V( D3DXCreateCubeTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, pms->pSrcData, pms->SrcDataSize, pms->Size, pms->MipLevels, pms->Usage,
													pms->Format, pms->Pool, pms->Filter, pms->MipFilter, pms->ColorKey, pms->pSrcInfo, pms->pPalette, &tex ) );

	pms->TextureTunnel->Set( tex ) ;
}

void r3dDeviceTunnel::D3DXCreateCubeTextureFromFileInMemoryEx( LPCVOID pSrcData, UINT SrcDataSize, UINT Size, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, r3dD3DTextureTunnel* TextureTunnel, bool async /*= false*/ )
{
	R3D_DEIVCE_QUEUE_OBJ( D3DXCreateCubeTextureFFIMEXParams, parms ) ;

	parms.pSrcData			= pSrcData ;
	parms.SrcDataSize		= SrcDataSize ;
	parms.Size				= Size ;
	parms.MipLevels			= MipLevels ;
	parms.Usage				= Usage ;
	parms.Format			= Format ;
	parms.Pool				= Pool ;
	parms.Filter			= Filter ;
	parms.MipFilter			= MipFilter ;
	parms.ColorKey			= ColorKey ;
	parms.pSrcInfo			= pSrcInfo ;
	parms.pPalette			= pPalette ;
	parms.TextureTunnel		= TextureTunnel;

	if( async )
	{
		r3d_assert( g_async_d3dqueue->GetInt() ) ;
		AddCustomDeviceQueueItem( DoCreateCubeTextureFFIMEX, &parms );
	}
	else
	{
		ProcessCustomDeviceQueueItem( DoCreateCubeTextureFFIMEX, &parms );
	}
}

//------------------------------------------------------------------------

struct SetPDataParams
{
	r3dD3DResourceTunnel* tunnel ;
	const char* data ;
};

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

void DoSetD3DResourcePrivateData( void* params )
{
	SetPDataParams* pms = (SetPDataParams*) params ;

	SetD3DResourcePrivateData( pms->tunnel->Get(), pms->data ) ;
}

void r3dDeviceTunnel::SetD3DResourcePrivateData( r3dD3DResourceTunnel* tunnel, const char* data )
{
	SetPDataParams params ;

	params.tunnel	= tunnel ;
	params.data		= data ;

	ProcessCustomDeviceQueueItem( DoSetD3DResourcePrivateData, &params ) ;
}

//------------------------------------------------------------------------

void r3dDeviceTunnel::SetD3DResourcePrivateData( r3dD3DSurfaceTunnel* tunnel, const char* data )
{
	SetD3DResourcePrivateData( static_cast<r3dD3DResourceTunnel*>(tunnel), data ) ;
}

//------------------------------------------------------------------------

IDirect3DBaseTexture9*
r3dD3DTextureTunnel::AsBaseTex() const
{
	R3D_ENSURE_MAIN_THREAD();

	return Get() ;
}

//------------------------------------------------------------------------

IDirect3DTexture9*
r3dD3DTextureTunnel::AsTex2D() const
{
	R3D_ENSURE_MAIN_THREAD();

	IDirect3DBaseTexture9* base = Get() ;
	if (!base)
		return 0;

	r3d_assert( base->GetType() == D3DRTYPE_TEXTURE );

	return (IDirect3DTexture9*) base ;
}

//------------------------------------------------------------------------

IDirect3DCubeTexture9*
r3dD3DTextureTunnel::AsTexCube() const
{
	R3D_ENSURE_MAIN_THREAD();

	IDirect3DBaseTexture9* base = Get() ;
	if (!base)
		return 0;

	r3d_assert( base->GetType() == D3DRTYPE_CUBETEXTURE );

	return (IDirect3DCubeTexture9*) base ;
}

//------------------------------------------------------------------------

IDirect3DVolumeTexture9*
r3dD3DTextureTunnel::AsTexVolume() const 
{
	R3D_ENSURE_MAIN_THREAD();

	IDirect3DBaseTexture9* base = Get() ;
	if (!base)
		return 0;

	r3d_assert( base->GetType() == D3DRTYPE_VOLUMETEXTURE );

	return (IDirect3DVolumeTexture9*) base ;
}

//------------------------------------------------------------------------

struct GetLevelCountParams
{
	int Result ;
	const r3dD3DTextureTunnel* Caller ;
};

static void DoGetLevelCount( void* ptr )
{
	GetLevelCountParams* params = static_cast< GetLevelCountParams* >( ptr ) ;
	
	params->Result = params->Caller->AsBaseTex()->GetLevelCount() ;
}

int
r3dD3DTextureTunnel::GetLevelCount() const
{
	GetLevelCountParams params ;

	params.Caller = this ;

	ProcessCustomDeviceQueueItem( DoGetLevelCount, &params ) ;

	return params.Result ;
}

//------------------------------------------------------------------------

struct GetLevelDescParams
{
	D3DSURFACE_DESC* oDesc ;
	int Level ;
	const r3dD3DTextureTunnel* Caller ;
};

static void DoGetLevelDesc2D( void* ptr )
{
	GetLevelDescParams* params = static_cast< GetLevelDescParams* >( ptr ) ;
	IDirect3DBaseTexture9* baseTex = params->Caller->AsBaseTex() ;

	switch( baseTex->GetType() )
	{
	case D3DRTYPE_TEXTURE:
		D3D_V( static_cast< IDirect3DTexture9* >( baseTex )->GetLevelDesc( params->Level, params->oDesc ) ) ;
		break ;
	case D3DRTYPE_CUBETEXTURE:
		D3D_V( static_cast< IDirect3DCubeTexture9* >( baseTex )->GetLevelDesc( params->Level, params->oDesc ) ) ;
		break ;
	default:
		r3dError( "DoGetLevelDesc2D: unsupported resource type!" ) ;
	}
}

void
r3dD3DTextureTunnel::GetLevelDesc2D( int level, D3DSURFACE_DESC* oDesc ) const
{
	GetLevelDescParams params ;

	params.Caller = this ;
	params.oDesc = oDesc ;
	params.Level = level ;

	ProcessCustomDeviceQueueItem( DoGetLevelDesc2D, &params ) ;
}

//------------------------------------------------------------------------

struct GetTexFmtParams
{
	D3DFORMAT* oFMT;
	const r3dD3DTextureTunnel* tex;
};


static void DoGetTexFmt( void* ptr )
{
	GetTexFmtParams* params = static_cast< GetTexFmtParams* >( ptr ) ;
	IDirect3DBaseTexture9* baseTex = params->tex->AsBaseTex();

	switch( baseTex->GetType() )
	{
	case D3DRTYPE_TEXTURE:
		{
			D3DSURFACE_DESC desc;
			D3D_V( static_cast< IDirect3DTexture9* >( baseTex )->GetLevelDesc( 0, &desc ) );
			*params->oFMT = desc.Format;
		}
		break;
	case D3DRTYPE_CUBETEXTURE:
		{
			D3DSURFACE_DESC desc;
			D3D_V( static_cast< IDirect3DCubeTexture9* >( baseTex )->GetLevelDesc( 0, &desc ) );
			*params->oFMT = desc.Format;
		}
		break;

	case D3DRTYPE_VOLUMETEXTURE:
		{
			D3DVOLUME_DESC desc;
			D3D_V( static_cast< IDirect3DVolumeTexture9* >( baseTex )->GetLevelDesc( 0, &desc ) );
			*params->oFMT = desc.Format;
		}
		break;
	default:
		r3dError( "DoGetLevelDesc2D: unsupported resource type!" ) ;
	}
}

//------------------------------------------------------------------------

D3DFORMAT r3dD3DTextureTunnel::GetFormat() const
{
	GetTexFmtParams params ;
	D3DFORMAT res;

	params.oFMT = &res;
	params.tex = this;

	ProcessCustomDeviceQueueItem( DoGetTexFmt, &params );

	return res;
}

//------------------------------------------------------------------------

struct LockRectParams
{
	UINT			Level ;
	D3DLOCKED_RECT*	pLockedRect ;
	const RECT*		pRect ;
	DWORD			Flags ;

	r3dD3DTextureTunnel Tunnel ;
};

static void DoLockRect( void* params )
{
	LockRectParams* pms = (LockRectParams*) params ;

	D3D_V( pms->Tunnel.AsTex2D()->LockRect( pms->Level, pms->pLockedRect, pms->pRect, pms->Flags) ) ;

	r3d_assert( gLockCounter >= 0 );

	gLockCounter ++ ;
}

void
r3dD3DTextureTunnel::LockRect( UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags )
{
	LockRectParams params ;

	params.Level		= Level ;
	params.pLockedRect	= pLockedRect ;
	params.pRect		= pRect ;
	params.Flags		= Flags ;

	params.Tunnel		= *this ;

	ProcessCustomDeviceQueueItem( DoLockRect, &params );
}

//------------------------------------------------------------------------

struct UnlockRectParams
{
	UINT				Level ;
	r3dD3DTextureTunnel Tunnel ;
};

static void DoUnlockRect( void* params )
{
	UnlockRectParams* pms = (UnlockRectParams*) params ;

	D3D_V( pms->Tunnel.AsTex2D()->UnlockRect( pms->Level ) ) ;

	r3d_assert( gLockCounter > 0 );

	gLockCounter -- ;
}


void
r3dD3DTextureTunnel::UnlockRect( UINT Level )
{
	UnlockRectParams params ;

	params.Level		= Level ;
	params.Tunnel		= *this ;

	ProcessCustomDeviceQueueItem( DoUnlockRect, &params ) ;
}


void
r3dD3DSurfaceTunnel::Set( IDirect3DSurface9* resource )
{
	R3D_ENSURE_MAIN_THREAD();

	if( resource )
	{
		D3DSURFACE_DESC desc ;
		D3D_V( resource->GetDesc( &desc ) ) ;

		mFormat = desc.Format ;
	}
	else
	{
		mFormat = D3DFMT_UNKNOWN ;
	}

	Parent::Set( resource ) ;
}

int
r3dD3DSurfaceTunnel::ReleaseAndReset()
{
	R3D_ENSURE_MAIN_THREAD();

	int refCount = 0 ;

	if( mRes )
	{
		refCount = mRes->Release() ;
		Set( 0 ) ;
	}

	return refCount ;
}

void r3dEnableWatchDog( float TimeOut )
{
	gWatchDogEnable = 1 ;	
	gWatchDogTimeOut = TimeOut ;

	r3dResetWatchDog() ;
}

void r3dSetWatchdogItemName( const char* ItemName )
{
	gWatchDogItemName = ItemName ;
}

void r3dResetWatchDog()
{
	gWatchDogLastReset = r3dGetTime() ;
}

void r3dDisableWatchDog()
{
	gWatchDogEnable = 0 ;
	gWatchDogItemName = 0 ;
}

void r3dEnsureDeviceAvailable()
{
	for( ; !r3dRenderer->DeviceAvailable ; )
	{
		r3dRenderer->TryToRestoreDevice();
		r3dProcessWindowMessages();
		Sleep( 250 ) ;
	}
}

int r3dGetTextureSizeInVideoMemory(int w, int h, int d, int mips, D3DFORMAT fmt)
{
	int texSize = 0;

	while( mips-- )
	{
		texSize += int(float(h * w * d) * GetD3DTexFormatSize(fmt));

		w /= 2; w = R3D_MAX( w, 1 );
		h /= 2; h = R3D_MAX( h, 1 );
		d /= 2; d = R3D_MAX( d, 1 );
	}
	return texSize;
}

void ZeroZeroStreamCache()
{
	d3dc._ZeroZeroStreamCache();
}

void ZeroIndexCache()
{
	d3dc._ZeroIndicesCahce();
}

void r3dStartLineStrip2D( int count )
{
	vtxArrayMinimalist2D_Lines->StartVertices( 2, count ) ;
}

void r3dLineStrip2D( int x0, int y0 )
{
	R3D_MINIMALIST_VERTEX_2D vtx ;

	vtx.pos.x = x0 ;
	vtx.pos.y = y0 ;

	vtxArrayMinimalist2D_Lines->AddVtxT( vtx ) ;

}

void r3dEndLineStrip2D()
{
	vtxArrayMinimalist2D_Lines->EndListVertices();
	vtxArrayMinimalist2D_Lines->Flush();
}

void r3dPrintVMem()
{
	UINT texMem = r3dRenderer->pd3ddev->GetAvailableTextureMem() ;

	r3dOutToLog( "We have %d MB texture memory\n", texMem / 1024 / 1024 );
}

//////////////////////////////////////////////////////////////////////////

VisibilityInfoEnum IsBoxOnPositiveHalfSpace(const D3DXPLANE &p, const r3dBoundBox &bb)
{
	VisibilityInfoEnum rv = VI_INTERSECTING;
	r3dPoint3D c = bb.Center();
	r3dPoint3D h = bb.Size * 0.5f;
	r3dPoint3D maxNorm(abs(p.a), abs(p.b), abs(p.c));
	float e = maxNorm.Dot(h);
	float s = c.Dot(r3dPoint3D(p.a, p.b, p.c)) + p.d;

	if (s - e > 0)
		rv = VI_OUTSIDE;

	if (s + e < 0)
		rv = VI_INSIDE;

	return rv;
}

//------------------------------------------------------------------------

#define R3D_STALL_TOLERANCE 0.075f

#if R3D_ENABLE_STALL_DETECTION
void r3dCSHolder::EnterWithStallDetection( CRITICAL_SECTION& cs )
{
	float start = r3dGetTime();

	for( ; !TryEnterCriticalSection( &cs ) ; )
	{
		Sleep( 1 );

		if( r3dGetTime() - start > R3D_STALL_TOLERANCE )
		{
			__asm int 3;
		}
	}
}
#endif

//------------------------------------------------------------------------

r3dCSHolderWithDeviceQueue::r3dCSHolderWithDeviceQueue(CRITICAL_SECTION& in_cs)
: cs( in_cs )
{
#ifndef WO_SERVER
	if( R3D_IS_MAIN_THREAD() )
	{
#if R3D_ENABLE_STALL_DETECTION
		float start = r3dGetTime();
#endif
		for( ; !TryEnterCriticalSection( &cs ) ; )
		{
			ProcessDeviceQueue( r3dGetTime(), 0.033f ) ;

#if R3D_ENABLE_STALL_DETECTION
			if( r3dGetTime() - start > R3D_STALL_TOLERANCE )
			{
				__asm int 3;
			}
#endif
		}
	}
	else
	{
		EnterCriticalSection( &cs ) ;
	}
#endif
}

//------------------------------------------------------------------------

r3dCSHolderWithDeviceQueue::~r3dCSHolderWithDeviceQueue()
{
#ifndef WO_SERVER
	LeaveCriticalSection( &cs ) ;
#endif
}


int gDisableDynamicObjectShadows = 0 ;