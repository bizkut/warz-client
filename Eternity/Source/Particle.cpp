#include "r3dPCH.h"
#include "r3d.h"

#include "JobChief.h"

#include "Particle_Int.h"
#include "Particle.h"

#include "../GameEngine/gameobjects\obj_Mesh.h"
#include "../../EclipseStudio\Sources\GameLevel.h"

extern r3dLightSystem	WorldLightSystem;

enum { 
	MAX_PARTICLE_BUFFER_SIZE = 16300,	// max number of *quads*. since we're using 16bit buffer, this*4 must be < 65656
	PARTICLE_BUFFER_SIZE = 12190,		// 2mb total
	PARTICLE_CACHE_SIZE = 1024,
	MESH_CACHE_SIZE = 1024,
	UNIFIED_SIZE = 1024*1024*2,	//size of unified vertex buffer
};


float gFillBufferTime;
float gFillUpdateTime;

int gNumTrails = 0;

unsigned short *gPartIdxArr;	//max particles per effect
int gPartIdxArrSize ;

static r3dScreenBuffer* FullSizeDepth ;
static r3dScreenBuffer* HalfSizeDepth ;

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);
void GetParticleDepthDimensions(float &w, float &h);

R3D_FORCEINLINE int GetParticleDownScale()
{
	return r_environment_quality->GetInt() == 1 ? 2 : 1 ;
}

static void psReloadTexture( r3dTexture*& tex )
{
	if( !tex )
		return;

	tex->DestroyResources( false );
	tex->LoadResources();
}


//-------------------------------------------------
//INSTANCING
#pragma pack(push,1)
struct PARTICLE_INSTANCE
{
	float4 corner;
};

struct PARTICLE_INDEX
{
	r3dColor24 Color;					//COLOR0
	r3dPoint3D 	pos;					//UV0
	float size, uvmix, rect, blend	;	//UV1
	float4 unified1;					//UV2
	r3dColor24 unified2;				//UV3
	r3dColor24 unified3;				//UV4
};

#pragma pack(pop)

static LPDIRECT3DVERTEXDECLARATION9 g_pParticleInstanceDecl = 0;
LPDIRECT3DVERTEXDECLARATION9 getInstanceVertexDecl()
{
	if(g_pParticleInstanceDecl == 0)
	{
		D3DVERTEXELEMENT9 VBDecl[] = 
		{
			{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},	//corner
			{1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
			{1, 0+4, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},	//pos
			{1, 0+4+12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},	//size_uvmix_rect_type
			{1, 0+4+12+16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},	//unified1
			{1, 0+4+12+16+16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},	//unified2
			{1, 0+4+12+16+16+4, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},	//unified3
			D3DDECL_END()
		};

		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &g_pParticleInstanceDecl) );
	}
	return g_pParticleInstanceDecl;
}

//-------------------------------------------------





LPDIRECT3DVERTEXDECLARATION9 R3D_PARTICLE_VERTEX::pDecl = 0;
D3DVERTEXELEMENT9 R3D_PARTICLE_VERTEX::VBDecl[] = 
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0},
	{0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 R3D_PARTICLE_VERTEX::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}

LPDIRECT3DVERTEXDECLARATION9 R3D_PARTICLE_VERTEX_BUMP::pDecl = 0;
D3DVERTEXELEMENT9 R3D_PARTICLE_VERTEX_BUMP::VBDecl[] = 
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0},
	{0, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 R3D_PARTICLE_VERTEX_BUMP::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}

bool		_r3d_ParticleEditorActive    = false;
static	const int	_r3d_ParticleEditorArraySize = 4096;
float		_r3d_ParticleWarmUpDelta     = 1.0f / 20;	// at 20 fps
int		_r3d_ParticleActiveSystems   = 0;		// number of active particle systems

r3dScreenBuffer*	g_ShadowMap ;
r3dScreenBuffer*	g_DistortBuff ;
int					g_DistortEnabled ;

D3DXMATRIX g_LightMtx ;

r3dVector	g_SunDir ( 0, 1, 0 );
r3dVector	P_WindDirection;
float		P_WindPower = 0;

static	int		ParticleVShader_ID = -1;
static	int		ParticleInstVShader_ID = -1;
static	int		ParticleInstVShaderShadow_ID = -1;
static	int		ParticleInstVShaderFillSM_ID = -1;
static	int		ParticleInstVShaderNM_ID = -1;
static	int		ParticleInstVShaderNMShadow_ID = -1;

static	int		ParticleInstPShaderFillSM_ID = - 1;

ParticlePSIds gParticlePSIds ;

static  int		ParticleNormalMapVShader_ID = -1;
static	int		ParticleVShaderMesh_ID = -1;
static	int		ParticlePShaderMesh_ID[] = {-1, -1};
static	int		ParticlePShaderMesh_NODISTORT_ID[] = {-1, -1};

static	r3dParticleTrailCache*	partTrailCache = NULL;

static	int		NumMeshesInMeshFactoryCache = 0;
static	r3dMesh*	__ParticleMeshFactoryCache[MESH_CACHE_SIZE];

static	r3dParticleData* ParticleCache[PARTICLE_CACHE_SIZE];
static	int		NumParticleCache = 0;

// vectors for rotation when camera facing
static	r3dPoint3D	camSavedU, camSavedR;
static	r3dPoint3D	rotVecU[360], rotVecR[360];

#ifdef _MSC_VER

static	r3dIBuffer16*			_ParticleIB = NULL;
static	r3dD3DVertexBufferTunnel _unifiedVB ; // Buffer to hold vertices
static unsigned int _unifiedVBOffset = 0;	//offset from start of VB in bytes
static	r3dD3DIndexBufferTunnel _instanceIB ;
static	r3dD3DVertexBufferTunnel _instanceVB ; // Buffer to hold vertices

template <class T> 
r3dVBufferT <T>* GetParticleVB ();

struct ParticleBuffers: r3dIResource
{
	ParticleBuffers( const r3dIntegrityGuardian& ig = r3dIntegrityGuardian() )
	: r3dIResource( ig )
	{
		R3D_ENSURE_MAIN_THREAD();
	}

	~ParticleBuffers()
	{
		R3D_ENSURE_MAIN_THREAD();
	}

	virtual	void		D3DCreateResource()  OVERRIDE
	{
		R3D_ENSURE_MAIN_THREAD();

		r3dDeviceTunnel::CreateVertexBuffer( UNIFIED_SIZE, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &_unifiedVB ) ;

		r3dDeviceTunnel::CreateIndexBuffer(6*sizeof(short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &_instanceIB ) ;

		WORD* Idx;
		_instanceIB.Lock(0, 0, (void**)&Idx, 0);
		*Idx++ = 0;
		*Idx++ = 1;
		*Idx++ = 2;
		*Idx++ = 0;
		*Idx++ = 2;
		*Idx++ = 3;
		_instanceIB.Unlock();

		r3dDeviceTunnel::CreateVertexBuffer(sizeof(PARTICLE_INSTANCE)*4, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &_instanceVB  );
		PARTICLE_INSTANCE* ptr;
		_instanceVB.Lock(0, 0, (void**)&ptr, 0) ;

		ptr[0].corner.x = 0.0f;		ptr[0].corner.y = 1.0f;
		ptr[0].corner.z = 0.0f;		ptr[0].corner.w = 0.0f;

		ptr[1].corner.x = 1.0f;		ptr[1].corner.y = 0.0f;
		ptr[1].corner.z = 1.0f;		ptr[1].corner.w = 0.0f;

		ptr[2].corner.x = 0.0f;		ptr[2].corner.y = -1.0f;
		ptr[2].corner.z = 1.0f;		ptr[2].corner.w = 1.0f;

		ptr[3].corner.x = -1.0f;	ptr[3].corner.y = 0.0f;
		ptr[3].corner.z = 0.0f;		ptr[3].corner.w = 1.0f;

		_instanceVB.Unlock() ;

		r3dDeviceTunnel::SetD3DResourcePrivateData( &_instanceIB, "Particles: InstanceIB");
		r3dDeviceTunnel::SetD3DResourcePrivateData( &_instanceVB, "Particles: InstanceVB");
		r3dDeviceTunnel::SetD3DResourcePrivateData( &_unifiedVB, "Particles: UnifiedIB");		

		r3dRenderer->Stats.AddBufferMem ( +UNIFIED_SIZE + sizeof(PARTICLE_INSTANCE)*4 + 6*sizeof(short) );
	}

	virtual	void		D3DReleaseResource() OVERRIDE
	{
		R3D_ENSURE_MAIN_THREAD();

		_instanceIB.ReleaseAndReset();
		_instanceVB.ReleaseAndReset();
		_unifiedVB.ReleaseAndReset();

		r3dRenderer->Stats.AddBufferMem( -(int)(UNIFIED_SIZE + sizeof(PARTICLE_INSTANCE)*4 + 6*sizeof(short)) );
	}
} *gParticleBuffers;


static r3dMesh* r3dParticleSystemAddMesh(const char* fname)
{
	if(NumMeshesInMeshFactoryCache >= MESH_CACHE_SIZE) {
		r3dOutToLog("!!!warninig!!! particle mesh cache overflow\n");
		return NULL;
	}

	for(int i=0;i<NumMeshesInMeshFactoryCache;i++)
		if(stricmp(__ParticleMeshFactoryCache[i]->FileName.c_str(), fname) == NULL)
			return __ParticleMeshFactoryCache[i];

	r3dMesh* msh = gfx_new r3dMesh( 0 );
	if(!msh->Load(fname, true)) {
		delete msh;
		return NULL;
	}
	msh->FillBuffers();

	__ParticleMeshFactoryCache[NumMeshesInMeshFactoryCache] = msh;
	NumMeshesInMeshFactoryCache++;

	return msh;
}

void r3dParticleSystemReloadCachedData()
{
	for(int i=0; i<NumParticleCache; i++)
	{
		ParticleCache[ i ]->Reload();
	}
}

void r3dParticleSystemReloadCachedDataTextures()
{
	for(int i=0; i<NumParticleCache; i++) {
		ParticleCache[i]->ReloadTextures() ;
	}
}


void r3dParticleSystemReleaseCachedData()
{
	for(int i=0;i<NumMeshesInMeshFactoryCache;i++) {
		SAFE_DELETE(__ParticleMeshFactoryCache[i]);
	}
	NumMeshesInMeshFactoryCache = 0;
}

static void hwPS_Init()
{
	if( ParticleVShader_ID == -1 )
	{
		ParticleVShader_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE_CPU" );

		ParticleInstVShader_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE_GPU" );
		ParticleInstVShaderShadow_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE_GPU_SHADOW" );
		ParticleInstVShaderFillSM_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE_GPU_FILLSM" ) ;
		ParticleInstVShaderNM_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE_GPU_W_BUMP" );
		ParticleInstVShaderNMShadow_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE_GPU_W_BUMP_SHADOW" );

		ParticleNormalMapVShader_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE_CPU_W_BUMP" );

		ParticleVShaderMesh_ID = r3dRenderer->GetVertexShaderIdx( "VS_PARTICLE2" );

		ParticleInstPShaderFillSM_ID = r3dRenderer->GetPixelShaderIdx( "PS_PARTICLE_FILLSM" ) ;

		ParticlePShaderMesh_ID[0] = r3dRenderer->GetPixelShaderIdx( "PS_PARTICLE2" );
		ParticlePShaderMesh_ID[1] = r3dRenderer->GetPixelShaderIdx( "PS_PARTICLE2_HR" );
		ParticlePShaderMesh_NODISTORT_ID[0] = r3dRenderer->GetPixelShaderIdx( "PS_PARTICLE2_NODISTORT" );		
		ParticlePShaderMesh_NODISTORT_ID[1] = r3dRenderer->GetPixelShaderIdx( "PS_PARTICLE2_NODISTORT_HR" );		
	}

	_ParticleIB = r3dCreateIndexBuffer16(PARTICLE_BUFFER_SIZE * 6);

	gParticleBuffers = gfx_new ParticleBuffers;
	gParticleBuffers->D3DCreateResource();

	gPartIdxArrSize = UNIFIED_SIZE / sizeof( PARTICLE_INDEX ) + 1 ;
	gPartIdxArr = gfx_new unsigned short [ gPartIdxArrSize ] ;


}

static void hwPS_Close()
{
	if(gParticleBuffers)
		gParticleBuffers->D3DReleaseResource();
	SAFE_DELETE( gParticleBuffers );
	SAFE_DELETE( _ParticleIB );

	ParticleInstVShader_ID = -1;
	ParticleVShader_ID = -1;
	ParticleNormalMapVShader_ID = -1;

	delete [] gPartIdxArr ;
}


template <class T> 
void PrepareAdditionalDataVert(T* V)
{
}

template <> 
void PrepareAdditionalDataVert(R3D_PARTICLE_VERTEX_BUMP * V)
{
	r3dVector vTang = V[0].Pos - V[1].Pos;
	vTang.Normalize();
	V[0].Tang = vTang;
	V[1].Tang = vTang;
	V[2].Tang = vTang;
	V[3].Tang = vTang;
}

#endif


static int calcBufferSizeToFitInMb(int maxSize, int* out_size)
{
	int numQuads = 1;
	for(; numQuads<=MAX_PARTICLE_BUFFER_SIZE; numQuads++) {
		int size = (numQuads * sizeof(R3D_PARTICLE_VERTEX) * 4) + (numQuads * sizeof(WORD) * 6);
		if(size >= maxSize + 1)
			break;
		*out_size = size;
	}
	return numQuads - 1;
}

static void dumpDebugInfo()
{
	//r3dOutToLog("r3dParticleSystemInit()\n");
	//r3dOutToLog(" particle: %d\n", sizeof(r3dSingleParticle));
	//r3dOutToLog(" emitter: %d\n", sizeof(r3dParticleElement));
	//r3dOutToLog(" max_size: %d\n", sizeof(r3dParticleData) + sizeof(r3dParticleElement) * r3dParticleData::MAX_EMITTER_SLOTS);

#if 0  
	for(int i=1; i<4; i++) {
		int size;
		int num = calcBufferSizeToFitInMb(i * 1024*1024, &size);
		r3dOutToLog("%dmb: %d particles (%d bytes, %x)\n", i, num, size, size);
	}
#endif

	/*
	1mb: 6096 particles (1048512 bytes, fffc0)
	2mb: 12192 particles (2097024 bytes, 1fff80)
	3mb: 16300 particles (2803600 bytes, 2ac790)
	*/
}


void r3dSetParticleVars ( r3dVector vSunDir, r3dScreenBuffer* shadowMap, r3dScreenBuffer* distortBuf, const D3DXMATRIX& lightMtx )
{
	g_SunDir = vSunDir ;
	g_SunDir.Normalize() ;

	g_ShadowMap		= shadowMap ;
	g_DistortBuff	= distortBuf ;

	g_LightMtx = lightMtx ;
}

R3D_FORCEINLINE void DoTransparentSetDistort( int bEnable )
{
	if( bEnable && !g_DistortEnabled  )
	{
		g_DistortBuff->Activate( 1 ) ;		
	}
	else
	{
		if( !bEnable && g_DistortEnabled )
		{
			g_DistortBuff->Deactivate() ;
		}
	}

	g_DistortEnabled = bEnable ;
}

void r3dTransparentSetDistort( int bEnable )
{
	if( !r_smart_transp_distort->GetInt() )
	{
		DoTransparentSetDistort( 1 ) ;
	}
	else
	{
		DoTransparentSetDistort( bEnable ) ;
	}
}

void r3dTransparentDistortFlush()
{
	DoTransparentSetDistort( 0 ) ;
}

r3dTexture* g_BlankDistortTex = NULL;

static bool gInitialized;

void r3dParticleSystemInit()
{
	dumpDebugInfo();

	if(gInitialized) {
		r3dError("r3dParticleSystemInit() called twice\n");
		return;
	}

	gInitialized = true;

	hwPS_Init();

	partTrailCache = gfx_new r3dParticleTrailCache();

	//  if(r3dParticleSystemLoad("data\\particles\\default.prt") == NULL) {
	//  r3dError("default.prt isn't found\n");
	//  return;
	// }

	g_BlankDistortTex = r3dRenderer->LoadTexture("data\\particles\\blank_distort.dds");

	return;
}

void r3dParticleSystemClose()
{
	r3d_assert( gInitialized );
	gInitialized = false;

	if(_r3d_ParticleActiveSystems) {
		r3dError("there is active particle systems\n");
	}

	SAFE_DELETE(partTrailCache);

	hwPS_Close();

	r3dParticleSystemReleaseCachedData();

	for(int i=0; i<NumParticleCache; i++) {
		SAFE_DELETE(ParticleCache[i]);
	}
	NumParticleCache = 0;


	r3dRenderer->DeleteTexture(g_BlankDistortTex);
	g_BlankDistortTex = NULL;
}


void r3dParticleSystemStartFrame()
{
}

void r3dParticleSystemReloadCacheFor ( const char* fname )
{
	for(int i=0; i<NumParticleCache; i++) 
	{
		if(stricmp(ParticleCache[i]->FileName, fname) == NULL) 
		{
			ParticleCache[i]->Reload ();
			break;
		}
	}
}

r3dParticleData* r3dCacheParticleData( const char* fname )
{
	// check cache
	for(int i=0; i<NumParticleCache; i++) 
	{
		if(stricmp(ParticleCache[i]->FileName, fname) == NULL) {
			r3dParticleData* pd = ParticleCache[i];
			if(_r3d_ParticleEditorActive) {
				// in editor, we need to reload particle data, to discard changes made
				pd->Load(pd->FileName);
				r3dOutToLog("particle: reloaded '%s'\n", fname);
			}

			return pd;
		}
	}

	return NULL;
}

r3dParticleSystem* r3dParticleSystemLoad(const char* fname)
{
	if(ParticleVShader_ID == -1) {
		r3dError("r3dParticleSystemInit() isn't called\n");
		return NULL;
	}

	if(NumParticleCache >= PARTICLE_CACHE_SIZE) {
		r3dOutToLog("!!!warning!!! particle cache overflow\n");
		return gfx_new r3dParticleSystem(ParticleCache[0]);
	}

	// check cache
	r3dParticleData* pd = r3dCacheParticleData( fname );

	if( !pd )
	{
		if(r3d_access(fname, 0) != 0) {
			r3dOutToLog("!!!warning!!! particle '%s' not found\n", fname);
			if(NumParticleCache) 
				return gfx_new r3dParticleSystem(ParticleCache[0]);
			return NULL;
		}

		// new particle data, load it and insert to cache
		pd = gfx_new r3dParticleData();
		if(!pd->Load(fname)) {
			delete pd;
			r3dOutToLog("!!!warning!!! can't load particle '%s'\n", fname);

			// at this point, we must have default particle in slot 0
			if(NumParticleCache) 
				return gfx_new r3dParticleSystem(ParticleCache[0]);
			return NULL;
		}
		ParticleCache[NumParticleCache++] = pd;
	}

	return gfx_new r3dParticleSystem(pd);
}

r3dParticleTrailCache::r3dParticleTrailCache()
{
	for(int i=0; i<MAX_TRAIL_ENTRIES; i++)
		data[i].bUsed = false;

	lastFree = 0;
}

r3dParticleTrailCache::~r3dParticleTrailCache()
{
}


r3dParticleTrailData* r3dParticleTrailCache::Acquire(int* out_idx)
{
	int idx = 0;

	if(data[lastFree].bUsed == false) 
	{
		idx = lastFree;
	} 
	else 
	{
		for(idx=0; idx<MAX_TRAIL_ENTRIES; idx++) {
			if(data[idx].bUsed == false)
				break;
		}

		if(idx == MAX_TRAIL_ENTRIES)
			idx = 0;
	}

	lastFree = idx + 1;
	if(lastFree >= MAX_TRAIL_ENTRIES) lastFree = 0;

	*out_idx = idx;

	data[idx].bUsed = true;
	return &data[idx];
}

void r3dParticleEmitter::InitDefaults()
{
	bInited	= 0;
	bActive	= 1;
	bCastsShadows = 0 ;
	bDirectionOriented = 0;
	bMeshDisableCulling = 0;
	EmitDistance = 0;
	bMeshUseDistortTexture = 0;

	vDirOrientedAdditRotation = r3dVector ( 0,0,0 );

	//BlendMode      = DEFAULT_BLEND_MODE;

	StartFrame 	= 0;
	
	SetNumFrames( 1 );

	PlayOnce = 0;

	FrameRate	= 10;
	UVSpeed	= 0;

	StartTime      = -1;
	EndTime	= -1;

	bSingleParticle  = 0;
	bRandomDirection = 0;
	RandomVector	  = r3dVector(1,1,1);

	bEmmiter	= 0;
	bEmitterDistanceSpawn = 0;
	bUsedAsEmitter = 0;
	bEmmiterTypeID = 0;

	ParticleType   = R3D_PARTICLE_CAMERAQUAD;

	RayWidth = 10;

	Name[0] = 0;

	Mesh        = NULL;

	ParticleLifeTime			= 1.0f;
	ParticleBirthRate				= 100;
	ParticleSize				= 1;
	ParticleVelocity			= 1;
	ParticleGravity				= 0;
	ParticleSpin				= 1;
	MotionRand			= r3dVector(0,0,0);
	MotionRandDelta		= 0.0f;
	MotionRandSmooth	= 0.0f;
	BornPosRand			= r3dPoint3D(0,0,0);
	ParticleLifeTimeVar			= 0;
	ParticleSizeVar	= 0;
	ParticleVelocityVar	= 0;
	ParticleGravityVar	= 0;
	ParticleSpinVar	= 0;
	DirectionRand	= r3dVector(0.0f, 0.0f, 0.0f);
	EmmiterOffset = r3dVector(0.0f, 0.0f, 0.0f);

	TrailStepDist  = 1.0f;

	AngleXOverLife.Reset(0);
	AngleYOverLife.Reset(0);
	AngleZOverLife.Reset(0);

	ColorOverLife.Reset(255);
	OpacityOverLife.Reset(255);

	BirthChartsTimeLapse = 1 ;

	BirthSizeOverLife.Reset(1);
	SizeOverLife.Reset(1);
	VelocityOverLife.Reset(1);
	GravityOverLife.Reset(1);
	SpinOverLife.Reset(1);
	BindGravityOverLife.Reset(1);

	BlendModeOverLife.Reset(DEFAULT_BLEND_MODE);
	FramesOverLife.Reset( 0, 1 );

	MeshFName = "";

	TrailSizeCoefMin = 1.f;
	TrailSizeCoefMax = 1.f;
	TrailOpacityCoefMin = 1.f;
	TrailOpacityCoefMax = 1.f;
	TrailTaleFade = 1.f;
	TrailTaleFadePow = 0.33f;
	TrailDrift = 0.f;

	return;
}

void r3dParticleEmitter::Unload()
{  
  Mesh = NULL;
  //TODO: add r3dParticleSystemRemoveMesh sometime later
}

void r3dParticleEmitter::Reset ()
{
	char sNameLast[MAX_PATH];
	r3dscpy(sNameLast,Name);
	InitDefaults ();
	r3dscpy(Name,sNameLast);
}

void r3dParticleEmitter::Copy(const r3dParticleEmitter* PE, const char* NewName)
{
	Unload();
	InitDefaults();
	bInited = 1;

	r3dscpy(Name, NewName);

	//BlendMode              = PE->BlendMode;
	StartFrame		= PE->StartFrame;
	
	iNumFrames		= PE->iNumFrames;
	fNumFrames		= PE->fNumFrames;

	FrameRate		= PE->FrameRate;
	UVSpeed		= PE->UVSpeed;

	StartTime		= PE->StartTime;
	EndTime		= PE->EndTime;

	bSingleParticle	= PE->bSingleParticle;
	if(bSingleParticle == 30) bSingleParticle = 0;
	bRandomDirection	= PE->bRandomDirection;
	bDirectionOriented = PE->bDirectionOriented;
	bMeshDisableCulling = PE->bMeshDisableCulling;
	EmitDistance = PE->EmitDistance;
	vDirOrientedAdditRotation = PE->vDirOrientedAdditRotation;
	
	if(bRandomDirection) 
		RandomVector	= PE->RandomVector;

	ParticleLifeTime			= PE->ParticleLifeTime;
	ParticleBirthRate				= PE->ParticleBirthRate;
	ParticleSize				= PE->ParticleSize;
	ParticleVelocity			= PE->ParticleVelocity;
	ParticleGravity				= PE->ParticleGravity;
	ParticleSpin				= PE->ParticleSpin;

	MotionRand			= PE->MotionRand;
	MotionRandDelta		= PE->MotionRandDelta;
	MotionRandSmooth	= PE->MotionRandSmooth;

	BornPosRand			= PE->BornPosRand;
	DirectionRand		= PE->DirectionRand;

	ParticleLifeTimeVar			= PE->ParticleLifeTimeVar;
	ParticleSizeVar				= PE->ParticleSizeVar;
	ParticleVelocityVar			= PE->ParticleVelocityVar;
	ParticleGravityVar			= PE->ParticleGravityVar;
	ParticleSpinVar				= PE->ParticleSpinVar;

	TrailStepDist		= PE->TrailStepDist;
	TrailSizeCoefMin	= PE->TrailSizeCoefMin;
	TrailSizeCoefMax	= PE->TrailSizeCoefMax;
	TrailOpacityCoefMin	= PE->TrailOpacityCoefMin;
	TrailOpacityCoefMax	= PE->TrailOpacityCoefMax;
	TrailTaleFade		= PE->TrailTaleFade;
	TrailTaleFadePow	= PE->TrailTaleFadePow;
	TrailDrift			= PE->TrailDrift;

	EmmiterOffset		= PE->EmmiterOffset;

	BirthChartsTimeLapse	= PE->BirthChartsTimeLapse;

	ColorOverLife.CopyFrom(PE->ColorOverLife);
	OpacityOverLife.CopyFrom(PE->OpacityOverLife);
	SizeOverLife.CopyFrom(PE->SizeOverLife);
	BirthSizeOverLife.CopyFrom(PE->BirthSizeOverLife);
	VelocityOverLife.CopyFrom(PE->VelocityOverLife);
	GravityOverLife.CopyFrom(PE->GravityOverLife);
	SpinOverLife.CopyFrom(PE->SpinOverLife);
	BindGravityOverLife.CopyFrom(PE->BindGravityOverLife);
	BlendModeOverLife.CopyFrom(PE->BlendModeOverLife);
	FramesOverLife.CopyFrom(PE->FramesOverLife);

	AngleXOverLife.CopyFrom(PE->AngleXOverLife);
	AngleYOverLife.CopyFrom(PE->AngleYOverLife);
	AngleZOverLife.CopyFrom(PE->AngleZOverLife);

	RayWidth		= PE->RayWidth;
	ParticleType		= PE->ParticleType;

	if(ParticleType == R3D_PARTICLE_MESH)
	{
#ifndef WO_SERVER
		Mesh = r3dGOBAddMesh( ( "Data/ObjectsDepot/" + PE->MeshFName ).c_str() );
#endif
		bMeshUseDistortTexture = PE->bMeshUseDistortTexture;
	}
}




void r3dParticleEmitter::Load(const char* FName, const char* PartName)
{
	Unload();
	InitDefaults();
	bInited = 1;

	r3dscpy(Name, PartName);

	StartFrame		= r3dReadCFG_I(FName, PartName, "StartFrame", 0);
	SetNumFrames( r3dReadCFG_I(FName, PartName, "NumFrames", 1) );

	PlayOnce = r3dReadCFG_I(FName, PartName, "PlayOnce", 0);

	FrameRate		= r3dReadCFG_F(FName, PartName, "FrameRate", 30);
	UVSpeed		= r3dReadCFG_F(FName, PartName, "UVSpeed", 0);

	//BlendMode		= r3dReadCFG_I(FName, PartName, "BlendMode", DEFAULT_BLEND_MODE);

	StartTime		= r3dReadCFG_F(FName, PartName, "StartTime", -1);
	EndTime		= r3dReadCFG_F(FName, PartName, "EndTime", -1);

	bSingleParticle	= r3dReadCFG_I(FName, PartName, "IsSingleParticle", 0);
	if(bSingleParticle == 30) bSingleParticle = 0;
	bRandomDirection	= r3dReadCFG_I(FName, PartName, "EmitAtRandomDirection", 0);
	if(bRandomDirection) 
		RandomVector	= r3dReadCFG_V(FName, PartName, "RandomDirectionVector", r3dVector(1,1,1));

	bEmmiter				= r3dReadCFG_I(FName, PartName, "bEmmiter", 0);
	bEmitterDistanceSpawn	= r3dReadCFG_I(FName, PartName, "bEmitterDistanceSpawn", 0);
	bUsedAsEmitter			= r3dReadCFG_I(FName, PartName, "bUsedAsEmitter", 0);
	bEmmiterTypeID			= r3dReadCFG_I(FName, PartName, "bEmmiterTypeID", 0);

	bDirectionOriented	= r3dReadCFG_I(FName, PartName, "DirectionOriented", 0);
	bMeshDisableCulling	= r3dReadCFG_I(FName, PartName, "MeshDisableCulling", 0);

	if( r3dCFGHas( FName, PartName, "EmitDistancePassed" ) )
	{
		EmitDistance	= r3dReadCFG_F(FName, PartName, "EmitDistancePassed", 0);
	}
	else
	{
		EmitDistance	= r3dReadCFG_F(FName, PartName, "EmitDistance", 0);
	}

	ParticleLifeTime		= r3dReadCFG_F(FName, PartName, "LifeTime", 1);
	ParticleBirthRate		= r3dReadCFG_F(FName, PartName, "NumberOfParticles", 1);
	ParticleSize			= r3dReadCFG_F(FName, PartName, "Size", 1);
	ParticleVelocity		= r3dReadCFG_F(FName, PartName, "Velocity", 1);
	ParticleGravity			= r3dReadCFG_F(FName, PartName, "Weight", 1);
	ParticleSpin			= r3dReadCFG_F(FName, PartName, "Spin", 1);

	vDirOrientedAdditRotation = r3dReadCFG_V(FName, PartName, "DirOrientedAdditRotation", r3dVector(0,0,0));

	MotionRand			= r3dReadCFG_V(FName, PartName, "MotionRandomness", r3dVector(0,0,0));
	MotionRandDelta		= r3dReadCFG_F(FName, PartName, "MotionRandDelta", 0.0f);
	MotionRandSmooth	= r3dReadCFG_F(FName, PartName, "MotionRandSmooth", 0.0f);
	BornPosRand			= r3dReadCFG_V(FName, PartName, "BornArea", r3dVector(0,0,0));
	DirectionRand		= r3dReadCFG_V(FName, PartName, "DirectionRandomness", r3dVector(0,0,0));

	ParticleLifeTimeVar			= r3dReadCFG_F(FName, PartName, "LifeVariation", 1);
	ParticleSizeVar				= r3dReadCFG_F(FName, PartName, "SizeVariation", 1);
	ParticleVelocityVar			= r3dReadCFG_F(FName, PartName, "VelocityVariation", 1);
	ParticleGravityVar			= r3dReadCFG_F(FName, PartName, "WeightVariation", 1);
	ParticleSpinVar				= r3dReadCFG_F(FName, PartName, "SpinVariation", 1);

	TrailStepDist			= r3dReadCFG_F(FName, PartName, "TrailStepDist", 1);
	TrailSizeCoefMin		= r3dReadCFG_F(FName, PartName, "TrailSizeCoefMin", 1);
	TrailSizeCoefMax		= r3dReadCFG_F(FName, PartName, "TrailSizeCoefMax", 1);
	TrailOpacityCoefMin		= r3dReadCFG_F(FName, PartName, "TrailOpacityCoefMin", 1);
	TrailOpacityCoefMax		= r3dReadCFG_F(FName, PartName, "TrailOpacityCoefMax", 1);
	TrailTaleFade			= r3dReadCFG_F(FName, PartName, "TrailTaleFade", 1);
	TrailTaleFadePow		= r3dReadCFG_F(FName, PartName, "TrailTaleFadePow", 0.33f);
	TrailDrift				= r3dReadCFG_F(FName, PartName, "TrailDrift", 0.0f);

	BirthChartsTimeLapse	= r3dReadCFG_F(FName, PartName, "BirthChartsTimeLapse", 1.0f);

	EmmiterOffset = r3dReadCFG_V(FName, PartName, "EmmiterOffset", r3dVector(0,0,0));

	ColorOverLife     .LoadGradient(FName, PartName, "ColorOverLife");
	OpacityOverLife   .LoadGradient(FName, PartName, "OpacityOverLife");
	SizeOverLife      .LoadGradient(FName, PartName, "SizeOverLife");
	BirthSizeOverLife .LoadGradient(FName, PartName, "BirthSizeOverLife");
	VelocityOverLife  .LoadGradient(FName, PartName, "VelocityOverLife");
	GravityOverLife    .LoadGradient(FName, PartName, "WeightOverLife");
	SpinOverLife      .LoadGradient(FName, PartName, "SpinOverLife");
	BindGravityOverLife.LoadGradient(FName, PartName, "BindWeightOverLife");
	AngleXOverLife    .LoadGradient(FName, PartName, "AngleXOverLife");
	AngleYOverLife    .LoadGradient(FName, PartName, "AngleYOverLife");
	AngleZOverLife    .LoadGradient(FName, PartName, "AngleZOverLife");
	BlendModeOverLife .LoadGradient(FName, PartName, "BlendModeOverLife");
	FramesOverLife    .LoadGradient(FName, PartName, "FramesOverLife");

	if( r3dCFGHas( FName, PartName, "ParticleType" ) )
	{
		ParticleType = r3dReadCFG_I(FName, PartName, "ParticleType", R3D_PARTICLE_CAMERAQUAD);
	}
	else
	{
		ParticleType = r3dReadCFG_I(FName, PartName, "QuadType", R3D_PARTICLE_CAMERAQUAD);
	}

	bCastsShadows = !!r3dReadCFG_I(FName, PartName, "CastsShadows", 0 ) ;

	RayWidth	= r3dReadCFG_F(FName, PartName, "RayWidth", 10);

	if(ParticleType == R3D_PARTICLE_MESH)
	{
		MeshFName = r3dReadCFG_S(FName, PartName, "MeshFile", "arrow01.sco");

#ifndef WO_SERVER
		Mesh = r3dGOBAddMesh( ( "Data/ObjectsDepot/" +  MeshFName ).c_str() );
#endif

		if( r3dCFGHas( FName, PartName, "UseDistortionTexture" ) )
		{
			bMeshUseDistortTexture	= r3dReadCFG_I(FName, PartName, "UseDistortionTexture", 0);
		}
		else
		{
			bMeshUseDistortTexture	= r3dReadCFG_I(FName, PartName, "MeshUseDistortTexture", 0);
		}

	}

}



void r3dParticleEmitter::Save(const char* FName)
{
	r3dWriteCFG_I(FName, Name, "StartFrame", StartFrame);
	r3dWriteCFG_I(FName, Name, "NumFrames", iNumFrames);

	r3dWriteCFG_I(FName, Name, "PlayOnce", PlayOnce);	

	r3dWriteCFG_F(FName, Name, "FrameRate", FrameRate);
	r3dWriteCFG_F(FName, Name, "UVSpeed", UVSpeed);

	//r3dWriteCFG_I(FName, Name, "BlendMode", BlendMode);

	r3dWriteCFG_F(FName, Name, "StartTime", StartTime);
	r3dWriteCFG_F(FName, Name, "EndTime", EndTime);

	r3dWriteCFG_I(FName, Name, "IsSingleParticle", bSingleParticle);
	r3dWriteCFG_I(FName, Name, "EmitAtRandomDirection", bRandomDirection);
	if(bRandomDirection) 
		r3dWriteCFG_V(FName, Name, "RandomDirectionVector", RandomVector);

	r3dWriteCFG_I(FName, Name, "bEmmiter", bEmmiter);
	r3dWriteCFG_I(FName, Name, "bEmitterDistanceSpawn", bEmitterDistanceSpawn);
	r3dWriteCFG_I(FName, Name, "bUsedAsEmitter", bUsedAsEmitter);
	r3dWriteCFG_I(FName, Name, "bEmmiterTypeID", bEmmiterTypeID);

	r3dWriteCFG_I(FName, Name, "DirectionOriented", bDirectionOriented);
	r3dWriteCFG_I(FName, Name, "MeshDisableCulling", bMeshDisableCulling);
	r3dWriteCFG_F(FName, Name, "EmitDistance", EmitDistance);

	r3dWriteCFG_F(FName, Name, "LifeTime", ParticleLifeTime);
	r3dWriteCFG_F(FName, Name, "NumberOfParticles", ParticleBirthRate);
	r3dWriteCFG_F(FName, Name, "Size", ParticleSize);
	r3dWriteCFG_F(FName, Name, "Velocity", ParticleVelocity);
	r3dWriteCFG_F(FName, Name, "Weight", ParticleGravity);
	r3dWriteCFG_F(FName, Name, "Spin", ParticleSpin);

	r3dWriteCFG_V(FName, Name, "DirOrientedAdditRotation", vDirOrientedAdditRotation);
	
	r3dWriteCFG_V(FName, Name, "MotionRandomness", MotionRand);		
	r3dWriteCFG_F(FName, Name, "MotionRandSmooth", MotionRandSmooth);
	r3dWriteCFG_F(FName, Name, "MotionRandDelta", MotionRandDelta);
	r3dWriteCFG_V(FName, Name, "BornArea", BornPosRand);
	r3dWriteCFG_V(FName, Name, "DirectionRandomness", DirectionRand);

	r3dWriteCFG_F(FName, Name, "LifeVariation", ParticleLifeTimeVar);
	r3dWriteCFG_F(FName, Name, "SizeVariation", ParticleSizeVar);
	r3dWriteCFG_F(FName, Name, "VelocityVariation", ParticleVelocityVar);
	r3dWriteCFG_F(FName, Name, "WeightVariation", ParticleGravityVar);
	r3dWriteCFG_F(FName, Name, "SpinVariation", ParticleSpinVar);

	r3dWriteCFG_F(FName, Name, "TrailStepDist", TrailStepDist);
	r3dWriteCFG_F(FName, Name, "TrailSizeCoefMin", TrailSizeCoefMin);
	r3dWriteCFG_F(FName, Name, "TrailSizeCoefMax", TrailSizeCoefMax);
	r3dWriteCFG_F(FName, Name, "TrailOpacityCoefMin", TrailOpacityCoefMin);
	r3dWriteCFG_F(FName, Name, "TrailOpacityCoefMax", TrailOpacityCoefMax);
	r3dWriteCFG_F(FName, Name, "TrailTaleFade", TrailTaleFade);
	r3dWriteCFG_F(FName, Name, "TrailTaleFadePow", TrailTaleFadePow);
	r3dWriteCFG_F(FName, Name, "TrailDrift", TrailDrift);
	r3dWriteCFG_F(FName, Name, "BirthChartsTimeLapse", BirthChartsTimeLapse);

	r3dWriteCFG_V(FName, Name, "EmmiterOffset", EmmiterOffset);

	ColorOverLife     .SaveGradient(FName, Name, "ColorOverLife");
	OpacityOverLife   .SaveGradient(FName, Name, "OpacityOverLife");
	SizeOverLife      .SaveGradient(FName, Name, "SizeOverLife");
	BirthSizeOverLife .SaveGradient(FName, Name, "BirthSizeOverLife");
	VelocityOverLife  .SaveGradient(FName, Name, "VelocityOverLife");
	GravityOverLife    .SaveGradient(FName, Name, "WeightOverLife");
	SpinOverLife      .SaveGradient(FName, Name, "SpinOverLife");
	BindGravityOverLife.SaveGradient(FName, Name, "BindWeightOverLife");
	AngleXOverLife    .SaveGradient(FName, Name, "AngleXOverLife");
	AngleYOverLife    .SaveGradient(FName, Name, "AngleYOverLife");
	AngleZOverLife    .SaveGradient(FName, Name, "AngleZOverLife");
	BlendModeOverLife .SaveGradient(FName, Name, "BlendModeOverLife");
	FramesOverLife    .SaveGradient(FName, Name, "FramesOverLife");

	r3dWriteCFG_I(FName, Name, "ParticleType", ParticleType);
	r3dWriteCFG_I(FName, Name, "CastsShadows", bCastsShadows);

	r3dWriteCFG_F(FName, Name, "RayWidth", RayWidth);

	if(ParticleType == R3D_PARTICLE_MESH)
	{
		r3dWriteCFG_S(FName, Name, "MeshFile", MeshFName.c_str());
		r3dWriteCFG_I(FName, Name, "MeshUseDistortTexture", bMeshUseDistortTexture);
	}
}




r3dParticleData::r3dParticleData()
: GlowIntensity ( 0.f )
, GlowFromDistortB_or_FromDiffuseA ( 1.f )
, TextureFileName("")
, DistortTextureFileName("")
, NormalTextureFileName("")
, bReceivesShadows( 1 )
, bAcceptsDynamicLights( 1 )
{ 
	NumInstances   = 0;
	FileName[0]    = 0;

	Texture	       = NULL;
	DistortTexture = NULL;
	NormalTexture  = NULL;

	WarmUpTime     = -1;
	ZSortEnabled   = 0;
	DepthBlendValue	= 1.0f;

	EmitterType	 = 0;		// 0 - point, 1 - line, 2 - Mesh
	EmitMesh	 = NULL;
	EmitVector	 = r3dVector(0,0,0);
	EmitRadius	 = 0;
	EmitTime	 = -1;

	HasLight = 0;
	LightCastsShadows = 0;
	LightSSShadowBlur = 0;

	LightFunctionID = 0;
	LightLifetime = 1800.0f;
	LightIntensity = 1.0f;
	LightRadius1Base = 2.0f;
	LightRadius2Base = 6.0f;

	LightColor.Reset(255);
	LightRadius1.Reset(1);
	LightRadius2.Reset(1);
	ParticleLightOffset.Assign(0,0,0);

	bDeferredMeshParticles = false;
	bDeferredMeshParticlesShadows = false;
	bDistort       = false;
	bNormalMap	   = false;
	fBumpPower		= 1.0f;
	
	OrgDirection   = r3dPoint3D(0, 1, 0);

	for(int i=0;i<MAX_EMITTER_SLOTS;i++) {
		PType[i] = NULL;
	}

	m_CastsShadows = false ;

	CullDistance = 2600.f;

	return;
}

r3dParticleData::~r3dParticleData()
{
	Unload ();
}

void r3dParticleData::ReloadTextures()
{
	psReloadTexture( Texture );
	psReloadTexture( DistortTexture );
	psReloadTexture( NormalTexture );
}

void r3dParticleData::Unload ()
{
	r3dRenderer->DeleteTexture(Texture);
	r3dRenderer->DeleteTexture(DistortTexture);
	r3dRenderer->DeleteTexture(NormalTexture);
	for ( int i = 0; i < MAX_EMITTER_SLOTS; i++ )
		SAFE_DELETE(PType[i]);
	SAFE_DELETE ( EmitMesh );
}

void r3dParticleData::Reload ()
{
	r3dCloseCFG_Cur ();
	Unload ();
	Load ( FileName );
}

struct paramsss
{
	AtlasDesc::Rect* rects ; 
	int count ;
	char* name ;
};

int r3dParticleData::Load(const char* FName)
{
	char Str1[256];
	char Str2[256];

	r3dscpy(FileName, FName);

	int numEmitters = 0;
	for(int i=0;i<MAX_EMITTER_SLOTS;i++)
	{ 
		sprintf(Str1,"ParticleType%d", i+1);
		sscanf(r3dReadCFG_S(FName, "System", Str1, "-NONE-"), "%s", Str2);

		//r3dOutToLog("PSYSTEM %s  LOAD %d  %s\n", FName, i, Str2);
		if(!strstr(Str2,"-NONE-"))
		{
			PType[i] = gfx_new r3dParticleEmitter;
			PType[i]->Load(FName, Str2);
			if(PType[i]->StartFrame < 0) 
				PType[i]->StartFrame = random(PType[i]->GetNumFramesI());

			numEmitters++;
		}
	}
	if(numEmitters == 0) 
		return 0;

	FixSubEmitters();

	HasLight = r3dReadCFG_I(FName, "System", "HasLight", 0);

	LightCastsShadows = r3dReadCFG_I(FName, "System", "LightCastsShadows", 0);
	LightSSShadowBlur = r3dReadCFG_I(FName, "System", "LightShadowsBlur", 0);

	SSSBParams.PhysRange	= r3dReadCFG_F(FName, "System", "ShadowBlur_PhysRange", SSSBParams.PhysRange );
	SSSBParams.Bias			= r3dReadCFG_F(FName, "System", "ShadowBlur_Bias", SSSBParams.Bias );
	SSSBParams.Sense		= r3dReadCFG_F(FName, "System", "ShadowBlur_Sense", SSSBParams.Sense );
	SSSBParams.Radius		= r3dReadCFG_F(FName, "System", "ShadowBlur_Radius", SSSBParams.Radius );

	LightFunctionID = r3dReadCFG_I(FName, "System", "LightFunctionID", 0);
	LightLifetime = r3dReadCFG_F(FName, "System", "LightLifetime", 0);
	LightIntensity = r3dReadCFG_F(FName, "System", "LightIntensity", 1.0f);
	LightRadius1Base = r3dReadCFG_F(FName, "System", "BaseLightRadius1", 2.0f);
	LightRadius2Base = r3dReadCFG_F(FName, "System", "BaseLightRadius2", 6.0f);
	LightRadius1.LoadGradient(FName, "System", "LightRadius1");
	LightRadius2.LoadGradient(FName, "System", "LightRadius2");
	LightColor.LoadGradient(FName, "System", "LightColor");
	ParticleLightOffset= r3dReadCFG_V(FName, "System", "LightOffset", r3dVector(0,0,0));


	if( r3dCFGHas( FName, "System", "CorrectParticleMeshes" ) )
	{
		bDeferredMeshParticles = r3dReadCFG_I(FName, "System", "CorrectParticleMeshes", 0);
	}
	else
	{
		bDeferredMeshParticles = r3dReadCFG_I(FName, "System", "DeferredMeshParticles", 0);
	}

	if ( bDeferredMeshParticles )
	{
		if( r3dCFGHas( FName, "System", "ShadowsFromCorrectParticleMeshes" ) )
		{
			bDeferredMeshParticlesShadows = r3dReadCFG_I(FName, "System", "ShadowsFromCorrectParticleMeshes", 0);
		}
		else
		{
			bDeferredMeshParticlesShadows = r3dReadCFG_I(FName, "System", "DeferredMeshParticlesShadows", 0);
		}
	}

	bAcceptsDynamicLights = r3dReadCFG_I(FName, "System", "AcceptsDynamicLights", 1);
	bReceivesShadows = r3dReadCFG_I(FName, "System", "ReceivesShadows", 1);
	bDistort = r3dReadCFG_I(FName, "System", "HaveDistortionTexture", 0);
	bNormalMap = r3dReadCFG_I(FName, "System", "HaveNormalTexture", 0);
	NormalTexture = NULL;
	DistortTexture = NULL;

	if(bDistort)
	{
		sprintf(Str1, "Data\\Particles\\%s", r3dReadCFG_S(FName, "System", "MeshDistortTexture", "Distort.dds"));

		DistortTextureFileName = r3dReadCFG_S(FName, "System", "MeshDistortTexture", "Distort.dds");

		DistortTexture = r3dRenderer->LoadTexture(Str1, D3DFMT_UNKNOWN, false, GetParticleDownScale());
	}

	TextureFileName = r3dReadCFG_S(FName, "System", "Texture", "Texture.dds");

	sprintf(Str1, "Data\\Particles\\%s", r3dReadCFG_S(FName, "System", "Texture", "Texture.dds"));

	Texture = r3dRenderer->LoadTexture(Str1, D3DFMT_UNKNOWN, false, GetParticleDownScale());

	char Str3[256];
	sprintf(Str3, "%s%s", Str1, ".Atlas");

	Atlas.load(Str3);
	if(r_instanced_particles->GetInt() && Atlas.count > (254 - 64)) // 64 is start of rectangle data in vertex shader
	{
		r3dError("Atlas %s have too many frames for GPU particles, 190 max", Str3);
	}

	if ( bNormalMap )
	{
		fBumpPower = r3dReadCFG_F(FName, "System", "BumpPower", 1);
		NormalTextureFileName = r3dReadCFG_S(FName, "System", "NormalTexture", "Texture.dds");
		sprintf(Str1, "Data\\Particles\\%s", NormalTextureFileName.c_str());

		NormalTexture = r3dRenderer->LoadTexture(Str1, D3DFMT_UNKNOWN, false, GetParticleDownScale());
	}

	GlowFromDistortB_or_FromDiffuseA = r3dReadCFG_F( FName, "System", "GlowFromDistortB_or_FromDiffuseA", 1.f ) ;
	GlowIntensity = r3dReadCFG_F( FName, "System", "GlowIntensity", 0.f ) ;

	WarmUpTime     = r3dReadCFG_F(FName, "System", "WarmUpTime", -1);
	ZSortEnabled   = r3dReadCFG_I(FName, "System", "ZSortEnabled", 0);
	DepthBlendValue	= r3dReadCFG_F(FName, "System", "DepthBlendValue", 1.0f);

	OrgGlobalScale = r3dReadCFG_F(FName, "System", "GlobalScale", 1);

	EmitRadius 	= r3dReadCFG_F(FName, "System", "EmitRadius", 0);
	EmitTime 	= r3dReadCFG_F(FName, "System", "EmitTime", 3600);

	OrgDirection 	= r3dReadCFG_V(FName, "System", "Direction", r3dVector(0,1,0));

	CullDistance 	= r3dReadCFG_F(FName, "System", "CullDistance", 2600);

	UpdateCastsShadows();

	return 1;
}

void r3dParticleData::FixSubEmitters()
{
	// fix wrong 'subemitters?'
	for(int i=0;i<MAX_EMITTER_SLOTS;i++)
	{
		if( r3dParticleEmitter* pe = PType[i] )
		{
			if( pe->bEmmiter && PType[pe->bEmmiterTypeID] && pe->bEmmiterTypeID != i )
				PType[pe->bEmmiterTypeID]->bUsedAsEmitter = true;
			else // not valid slot
				pe->bEmmiter = false;
		}
	}
}

void
r3dParticleData::UpdateCastsShadows()
{
	m_CastsShadows = 0 ;

	for( int i = 0, e = r3dParticleData::MAX_EMITTER_SLOTS ; i < e ; i ++ )
	{
		if( const r3dParticleEmitter* emm = PType[ i ] )
		{
			if( emm->bCastsShadows )
				m_CastsShadows = true ;
		}
	}
}

bool r3dParticleData::CastsShadows() const
{
	return m_CastsShadows || r_force_parts_transp_shad->GetInt() ;
}

void r3dParticleData::Save(const char* FName)
{
	unlink(FName);

	char Str1[256];

	r3dscpy(FileName, FName);

	int CID = 1;
	for(int i=0;i<MAX_EMITTER_SLOTS;i++)
	{ 
		if(PType[i] == NULL)
			continue;

		sprintf(Str1,"ParticleType%d", i+1);

		r3dWriteCFG_S(FName, "System", Str1, PType[i]->Name);
		PType[i]->Save(FName);
	}


	r3dWriteCFG_I(FName, "System", "HasLight", HasLight);

	r3dWriteCFG_I(FName, "System", "LightCastsShadows", LightCastsShadows );
	r3dWriteCFG_I(FName, "System", "LightShadowsBlur", LightSSShadowBlur);

	r3dWriteCFG_F(FName, "System", "ShadowBlur_PhysRange", SSSBParams.PhysRange );
	r3dWriteCFG_F(FName, "System", "ShadowBlur_Bias", SSSBParams.Bias );
	r3dWriteCFG_F(FName, "System", "ShadowBlur_Sense", SSSBParams.Sense );
	r3dWriteCFG_F(FName, "System", "ShadowBlur_Radius", SSSBParams.Radius );

	r3dWriteCFG_I(FName, "System", "LightFunctionID", LightFunctionID);
	r3dWriteCFG_F(FName, "System", "LightLifetime", LightLifetime);
	r3dWriteCFG_F(FName, "System", "LightIntensity", LightIntensity);
	r3dWriteCFG_F(FName, "System", "BaseLightRadius1", LightRadius1Base);
	r3dWriteCFG_F(FName, "System", "BaseLightRadius2", LightRadius2Base);
	LightRadius1.SaveGradient(FName, "System", "LightRadius1");
	LightRadius2.SaveGradient(FName, "System", "LightRadius2");
	LightColor.SaveGradient(FName, "System", "LightColor");
	r3dWriteCFG_V(FName, "System", "LightOffset", ParticleLightOffset);
	
	r3dWriteCFG_I(FName, "System", "DeferredMeshParticles", bDeferredMeshParticles);

	if ( bDeferredMeshParticles )
		r3dWriteCFG_I(FName, "System", "DeferredMeshParticlesShadows", bDeferredMeshParticlesShadows);

	r3dWriteCFG_I(FName, "System", "AcceptsDynamicLights", bAcceptsDynamicLights );
	r3dWriteCFG_I(FName, "System", "ReceivesShadows", bReceivesShadows );

	r3dWriteCFG_I(FName, "System", "HaveDistortionTexture", bDistort);
	r3dWriteCFG_I(FName, "System", "HaveNormalTexture", bNormalMap);

	if(bDistort)
		r3dWriteCFG_S(FName, "System", "MeshDistortTexture", DistortTextureFileName.c_str());

	r3dWriteCFG_S(FName, "System", "Texture", TextureFileName.c_str());

	if(bNormalMap)
	{
		r3dWriteCFG_F(FName, "System", "BumpPower", fBumpPower);
		r3dWriteCFG_S(FName, "System", "NormalTexture", NormalTextureFileName.c_str());
	}

	r3dWriteCFG_F(FName, "System", "GlowFromDistortB_or_FromDiffuseA", GlowFromDistortB_or_FromDiffuseA );
	r3dWriteCFG_F(FName, "System", "GlowIntensity", GlowIntensity );

// 	r3dWriteCFG_I(FName, "System", "XDiv", XDiv);
// 	r3dWriteCFG_I(FName, "System", "YDiv", YDiv);
	r3dWriteCFG_F(FName, "System", "WarmUpTime", WarmUpTime);
	r3dWriteCFG_I(FName, "System", "ZSortEnabled", ZSortEnabled);
	r3dWriteCFG_F(FName, "System", "DepthBlendValue", DepthBlendValue);

	r3dWriteCFG_F(FName, "System", "GlobalScale", OrgGlobalScale);

	r3dWriteCFG_F(FName, "System", "EmitRadius", EmitRadius);

	r3dWriteCFG_F(FName, "System", "EmitTime", EmitTime);
	r3dWriteCFG_V(FName, "System", "Direction", OrgDirection);

	r3dWriteCFG_F(FName, "System", "CullDistance", CullDistance);	
}


r3dParticleSystem::r3dParticleSystem(const r3dParticleData* data)
{ 
	PD = data;

	_r3d_ParticleActiveSystems++;
	const_cast<r3dParticleData*>(PD)->NumInstances++;

	bRenderUntextured  = 0;

	ArraySize          = 0;
	Array              = NULL;

	bInheretDirectionFromSystem = 0;

	StartIB = 0 ;

	IsVisible = true;
	Init(r3dGetTime());
}


r3dParticleSystem::~r3dParticleSystem() 
{
	if( ParticleLight.pLightSystem )
	{
		TurnLightOff();
	}

	r3d_assert(_r3d_ParticleActiveSystems);
	r3d_assert(PD->NumInstances);
	_r3d_ParticleActiveSystems--;
	const_cast<r3dParticleData*>(PD)->NumInstances--;

	SAFE_DELETE_ARRAY(Array);
}



void r3dParticleSystem::Init(float CurTime)
{
	r3d_assert(PD);

	for(int i=0;i<r3dParticleData::MAX_EMITTER_SLOTS;i++)
	{
		ToEmitWithDistance[i] = 0.f;
	}

	bEmit                 = 0;

	StartTime 		= CurTime;
	PrevTime		= CurTime;
	NumAliveParticles 	= 0;

	DeflectorTop		= 1000000;
	DeflectorBottom	= -1000000;

	Position              = r3dPoint3D(0, 0, 0);
	PrevPosition          = r3dPoint3D(0, 0, 0);
	Direction             = PD->OrgDirection;

	// prevent division by zero
	MaxBBoxSize			= 0.01f;

	GlobalScale		= 1.0f;
	WholeGlobalScale      = GlobalScale * PD->OrgGlobalScale;
	GlobalRenderScale     = 1.0f; 
	BornScale	        = 1.0f;

	BBox.Org.Assign(0,0,0);
	BBox.Size.Assign(0,0,0);	

	EmitersTypes = 0;

	CreateBuffers();

	IsVisible = true;

	ParticleLight.TurnOff();
	ParticleLight.SetType(R3D_OMNI_LIGHT);
	ParticleLight.SetColor(255, 201, 14);
	ParticleLight.bCastShadows = 0;
	ParticleLight.SetRadius(2.0f,6.0f);
	ParticleLight.Intensity = 1.0f;

	// VERY VERY BAD IDEA !!!!
	if (PD->HasLight) 
	{
		TurnLightOn();
	}

	Update(CurTime, 0);
	return;
}

void r3dParticleSystem::CreateBuffers()
{
	// create array
	ArraySize = 0;
	for(int i=0;i<r3dParticleData::MAX_EMITTER_SLOTS;i++) 
	{
		if(PD->PType[i] == NULL)
			continue;

		const r3dParticleEmitter& PE = *PD->PType[i];

		// see if I'm emitted by someboedy

		bool slave = false ;

		for(int j=0;j<r3dParticleData::MAX_EMITTER_SLOTS;j++)
		{
			if( i == j )
				continue ;

			if(PD->PType[j] == NULL)
				continue;

			const r3dParticleEmitter& possibleEmitter = *PD->PType[j];
			if( possibleEmitter.bEmmiter && possibleEmitter.bEmmiterTypeID == i )
			{
				slave = true ;
				break ;
			}
		}

		if( slave )
			continue ;

		int toAdd = 0 ;

		if(PE.bSingleParticle)
			toAdd = 1;
		else
			toAdd = int(PE.ParticleBirthRate * PE.ParticleLifeTime);

		const r3dParticleEmitter* slaveEmitter = PD->PType[ PE.bEmmiterTypeID ] ;

		if( PE.bEmmiter && slaveEmitter )
		{
			float dur = PE.ParticleLifeTime ;

			if( PE.EndTime > 0 )
			{
				if( PE.StartTime > 0 )
				{
					dur = PE.EndTime - PE.StartTime ;
				}
				else
				{
					dur = PE.EndTime ;
				}
			}

			dur = R3D_MIN( dur, PE.ParticleLifeTime ) ;

			ArraySize += toAdd + int( PE.ParticleBirthRate * dur ) * int( slaveEmitter->ParticleBirthRate * PE.ParticleLifeTime ) ;
		}
		else
		{
			ArraySize += toAdd ;
		}		
	}
	ArraySize = ArraySize * 11 / 10; // *1.1f - increase array a little, for some unexpected things
	if(ArraySize < 32) ArraySize = 32;
	CalculatedArraySize = ArraySize;
	if(_r3d_ParticleEditorActive && ArraySize < _r3d_ParticleEditorArraySize)
		ArraySize = _r3d_ParticleEditorArraySize;

	ArraySize = R3D_MIN( ArraySize, int( UNIFIED_SIZE / sizeof( PARTICLE_INDEX ) ) ) ;

	SAFE_DELETE_ARRAY(Array);
	Array = gfx_new r3dSingleParticle[ArraySize];

	for(int i=0;i<ArraySize;i++) {
		Array[i].Active = 0;
	}

	return;
}

void r3dParticleSystem::Restart(float CurTime)
{
	r3d_assert(ArraySize);

	// in case of editor, we can recreate buffers
	if(_r3d_ParticleEditorActive) {
		CreateBuffers();
	}

	bEmit 	    = 1; 

	StartTime 	    = CurTime;
	PrevTime	    = CurTime;
	NumAliveParticles = 0;
	NumAliveQuads     = 0;
	PrevPosition      = Position;

	if ( ParticleLight.pLightSystem )
	{
		TurnLightOff();
	}

	if( PD->HasLight )
	{
		TurnLightOn();
	}

	for(int i=0;i<r3dParticleData::MAX_EMITTER_SLOTS;i++)
	{
		if(PD->PType[i] == NULL)
			continue;

		ToEmitWithDistance[i] = 0.f;

		LastTimeCreated[i] = CurTime - PD->WarmUpTime;
		PTypeSpawned[i]    = 0;
	}

	for(int i=0;i<ArraySize;i++) 
		Array[i].Active = 0;

	if(PD->WarmUpTime > 0) {
		for(float time = CurTime - PD->WarmUpTime; time < CurTime; time += _r3d_ParticleWarmUpDelta) {
			IsVisible = true;
			Update(time);
		}
	} else {
		IsVisible = true;
		Update(CurTime, 0);
	}

	return;
}


float GetFloatVariation(float BaseVal, float Variation)
{
	if(!Variation) return BaseVal;
	return u_GetRandom(BaseVal-Variation, BaseVal+Variation);
}



r3dVector GetVectorVariation(const r3dVector& BaseVal, const r3dVector& Variation)
{
	if(!Variation.X && !Variation.Y && !Variation.Z) return BaseVal;
	r3dVector res;
	res.x = u_GetRandom(BaseVal.x - Variation.x, BaseVal.x + Variation.x);
	res.y = u_GetRandom(BaseVal.y - Variation.y, BaseVal.y + Variation.y);
	res.z = u_GetRandom(BaseVal.z - Variation.z, BaseVal.z + Variation.z);
	
	return res;
}

r3dPoint3D _GetRandomSurfacePoint(r3dMesh *Mesh)
{
	r3dPoint3D Pos1;

	float Var1 = u_GetRandom(0.0f, 0.99f);
	float Var2 = u_GetRandom(0.0f, 0.99f);
	int  FD = random(Mesh->NumIndices/3);

	r3dPoint3D P1 = Mesh->VertexPositions[Mesh->Indices[FD*3+1]];
	r3dPoint3D V1 = Mesh->VertexPositions[Mesh->Indices[FD*3+2]] - Mesh->VertexPositions[Mesh->Indices[FD*3+1]];

	P1 += V1*Var1;

	V1 = P1 - Mesh->VertexPositions[Mesh->Indices[FD*3+0]];
	P1 = Mesh->VertexPositions[Mesh->Indices[FD*3+0]];

	P1 += V1*Var2;

	//Mesh->WorldVertexList[random(Mesh->NumVertices - 1)]


	D3DXMATRIX 	mWorld, mMove, RotateMatrix;	
	//D3DXMatrixIdentity(&RotateMatrix);
	//RotateMatrix = Mesh->RotateMatrix;

	//D3DXMatrixTranslation(&mMove, Mesh->vMove.X, Mesh->vMove.Y, Mesh->vMove.Z);
	//D3DXMatrixMultiply(&mWorld, &RotateMatrix, &mMove);

#define DXV(v) D3DXVECTOR3(v.x, v.y, v.z)

	D3DXVECTOR3 vert;
	D3DXVECTOR3 v = DXV(P1);
	//D3DXVec3TransformCoord(&vert, &v, &mWorld);

	Pos1.X = vert.x;
	Pos1.Y = vert.y;
	Pos1.Z = vert.z;

	return Pos1;
}


inline float _RandomFloat()
{
	return u_GetRandom(-1.0f, 1.0f);
}


r3dVector MakeRandVector()
{
	r3dVector V;
	float x = _RandomFloat();
	float y = _RandomFloat();
	float z = _RandomFloat();

	V.Assign(x,y,z); 

	V.Normalize();
	return V;
}



r3dPoint3D r3dParticleSystem::GetEmitterPosition(const r3dPoint3D& center, const r3dParticleEmitter& PE)
{
	r3dPoint3D pos;

	switch(PD->EmitterType)
	{
	default:
		r3d_assert(0 && "invalid emitter type");
		break;

	case R3D_EMITTYPE_POINT:
		pos = r3dGetConfusionPosition(center, PD->EmitRadius) + PE.EmmiterOffset * WholeGlobalScale;
		break;

	case R3D_EMITTYPE_LINE:
		{
			float Var1 = u_GetRandom(0.0f, 0.99f);
			pos = center + PD->EmitVector * Var1 + PE.EmmiterOffset * WholeGlobalScale;

			break;
		}

	case R3D_EMITTYPE_MESH:
		{
			if(PD->EmitMesh)
				pos = _GetRandomSurfacePoint(PD->EmitMesh) + PE.EmmiterOffset * WholeGlobalScale;
			else
				pos = center + PE.EmmiterOffset * WholeGlobalScale;

			break;
		}

	case R3D_EMITTYPE_RING:
		pos = r3dGetRingConfusionPosition(center, PD->EmitRadius) + PE.EmmiterOffset * WholeGlobalScale;
		break;

	case R3D_EMITTYPE_SPHERE:
		pos = r3dGetRingConfusionPosition(center, u_GetRandom(0, PD->EmitRadius)) + PE.EmmiterOffset * WholeGlobalScale;
		break;
	}

	return pos;
}

int r3dParticleSystem::AddType(int EmitterIdx, float CurTime, const r3dPoint3D& BPosition, bool bDistanceBased, bool bUpdate, int iLod)
{
	r3d_assert(PD->PType[EmitterIdx] != NULL);
	const r3dParticleEmitter& PE = *PD->PType[EmitterIdx];

	if(!PE.bActive) 
		return 0;

	if(PE.bSingleParticle && PTypeSpawned[EmitterIdx]) 
		return 0;

	if(PE.StartTime > 0)
		if((CurTime - StartTime) < PE.StartTime) 
			return 0;

	if(PE.EndTime > 0)
		if((CurTime - StartTime) > PE.EndTime) 
			return 0;

	int PToAdd = 0 ;

	if( PE.bSingleParticle || bDistanceBased || !bUpdate )
	{
		PToAdd = 1 ;
	}
	else
	{
		PToAdd = int(float(PE.ParticleBirthRate*BornScale) * (CurTime - LastTimeCreated[EmitterIdx]));
	}
	
	if(!PToAdd) 
		return 0;

	if(bUpdate) 
		LastTimeCreated[EmitterIdx] = CurTime;

	if(PToAdd > (ArraySize-NumAliveParticles-1)) 
		PToAdd = ArraySize - 1 - NumAliveParticles;

	// r3dOutToLog("Adding %d partiocles at %d %d %d at time %f\n", PToAdd, int(BPosition.x),int(BPosition.y),int(BPosition.z), CurTime);

	for(int j=0;j<PToAdd;j++)
	{
		r3d_assert(NumAliveParticles + 1 <= ArraySize);
		CreateParticle(&Array[NumAliveParticles], PE, EmitterIdx, CurTime, BPosition);
		if(Array[NumAliveParticles].lod+1 >= iLod)	//add only particle in this lod
			NumAliveParticles++;
	}

	return PToAdd;
}

void r3dParticleSystem::CreateParticle(r3dSingleParticle* P, const r3dParticleEmitter& PE, int EmitterIdx, float CurTime, const r3dPoint3D& BPosition)
{
	P->Type 	    = EmitterIdx;
	P->Active 	    = 1;

	if( PE.bSingleParticle )
	{
		// always spawn/process
		P->lod = 3 ;
	}
	else
	{
		P->lod = rand() & 3 ;
	}

	P->BirthTime      = CurTime;
	P->LastSlaveSpawn = CurTime;
	P->LifeTime	    = GetFloatVariation(PE.ParticleLifeTime, PE.ParticleLifeTimeVar);
	if(P->LifeTime == 0.0f) // rare case when it can become 0
		P->LifeTime = PE.ParticleLifeTime;

	const r3dPoint3D emitPos = GetEmitterPosition(BPosition, PE);
	const r3dPoint3D partPos = GetVectorVariation(emitPos, PE.BornPosRand * WholeGlobalScale);
	P->Position       = partPos;
	P->PrevPosition   = partPos;
	
	if(PE.bRandomDirection)
		P->Direction    = MakeRandVector()*PE.RandomVector;
	else
		P->Direction      = GetVectorVariation(Direction, PE.DirectionRand);

	if(PE.StartFrame < 0)
		P->StaticFrame  = random(PE.GetNumFramesI());

	P->BaseVelocity		= GetFloatVariation(PE.ParticleVelocity, PE.ParticleVelocityVar) * WholeGlobalScale;
	P->BaseSize			= GetFloatVariation(PE.ParticleSize, PE.ParticleSizeVar) * 
								WholeGlobalScale * 
									PE.BirthSizeOverLife.GetFloatValue( R3D_MIN( ( CurTime - StartTime ) / PE.BirthChartsTimeLapse, 1.f ) );
	P->BaseRotation		= GetFloatVariation(PE.ParticleSpin, PE.ParticleSpinVar);
	P->BaseGravity		= GetFloatVariation(PE.ParticleGravity, PE.ParticleGravityVar) * WholeGlobalScale;

	P->MotionRandSrc	= r3dPoint3D(0,0,0) ;
	P->MotionRandTarg	= r3dPoint3D(0,0,0) ;

	P->LastRandMotionChange = CurTime - PE.MotionRandDelta - 0.01f ;

	P->EmitTravelDistance = 0.f ;

	P->TrailIdx       = 0xFFFF;

	if(PE.ParticleType != R3D_PARTICLE_MESH) {
		NumAliveQuads++;
	}

	if(PE.ParticleType == R3D_PARTICLE_TRAIL) {
		int idx;
		r3dParticleTrailData* trail = partTrailCache->Acquire(&idx);
		P->TrailIdx = idx;

		trail->AccumDist		= 0.0f;
		trail->CurPos			= 0;
		trail->MaxPos			= 0;
		trail->PrevPos[0]		= partPos;
		trail->DriftDir[0]		= r3dPoint3D( u_GetRandom( -1, +1 ), u_GetRandom( -1, +1 ), u_GetRandom( -1, +1 ) ) ;
		trail->SizeCoefs[0]		= 1.f ;
		trail->OpacityCoefs[0]	= 1.f ;
		trail->TimeBorn[0]		= CurTime ;

		NumAliveQuads += r3dParticleTrailData::MAX_TRAIL_POINTS - 1;
	}

	EmitersTypes |= 1 << PE.ParticleType;
	return;
}

void r3dParticleSystem::ProcessParticle(r3dSingleParticle* P, float CurTime)
{
	// if our emitter was destroyed - remove particle
	if(PD->PType[P->Type] == NULL) {
		P->Active = FALSE;
		return;
	}
	const r3dParticleEmitter& PE = *PD->PType[P->Type];

	const float PTime = (CurTime - P->BirthTime) / P->LifeTime;
	r3d_assert(PTime < 2.0f);

	P->Velocity	    = P->BaseVelocity * PE.VelocityOverLife.GetFloatValue(PTime);
	float PGravity	    = P->BaseGravity   * PE.GravityOverLife.GetFloatValue(PTime);
	float PBindGravity = PE.BindGravityOverLife.GetFloatValue(PTime); /* OLD_REMOVED: * Bind; */
	float PSize       = P->BaseSize     * PE.SizeOverLife.GetFloatValue(PTime); //NOTE: it is calculated here AND in GetOverTimeParams to save 4 bytes

	r3dVector RealV;
	RealV = P->Direction * P->Velocity;
	RealV.Y -= PGravity;

	r3dPoint3D prevPos = P->PrevPosition;

	P->EmitTravelDistance += ( P->Position - prevPos ).Length() ;

	P->PrevPosition = P->Position;
	P->Position 	+= RealV * TimePassed;
	
	if( CurTime - P->LastRandMotionChange > PE.MotionRandDelta )
	{
		P->MotionRandSrc	= P->MotionRandTarg ;
		P->MotionRandTarg	= GetVectorVariation( r3dPoint3D(0,0,0), PE.MotionRand ) ;

		P->LastRandMotionChange = CurTime ;
	}

	if( PE.MotionRandDelta < 0.0001f )
	{
		P->Position += P->MotionRandTarg * TimePassed ;
	}
	else
	{
		float lerpK = R3D_MIN( ( CurTime - P->LastRandMotionChange ) / PE.MotionRandDelta, 1.f ) ;

		if( lerpK > 0.5f ) 
		{
			lerpK = powf( ( lerpK - 0.5f ) * 2, PE.MotionRandSmooth ) ;
			lerpK = ( lerpK + 1.0f ) * 0.5f ;
		}
		else
		{
			lerpK = powf( ( 0.5f - lerpK ) * 2, PE.MotionRandSmooth ) ;
			lerpK = ( 1.f - lerpK ) * 0.5f ;
		}

		P->Position += ( P->MotionRandSrc * ( 1.0f - lerpK ) + P->MotionRandTarg * lerpK ) * TimePassed ;
	}

	if(P_WindPower)
	{
		r3dVector VV;
		VV = P_WindDirection *(P_WindPower *u_GetRandom(0.0f, 1.0f));
		P->Position += VV * TimePassed;
	}

	if(P->Position.Y > DeflectorTop)
	{
		P->Direction.Assign(_RandomFloat(),0,_RandomFloat());
		P->Position.Y = DeflectorTop-1;
	}

	if(P->Position.Y < DeflectorBottom)
	{
		P->Direction.Assign(_RandomFloat(),0, _RandomFloat());
		P->Position.Y = DeflectorBottom+1;
	}

	P->Position += SourceMoveDelta * PBindGravity;

	// trailidx can be invalid if particle type switched in editor
	if(PE.ParticleType == R3D_PARTICLE_TRAIL && P->TrailIdx != 0xFFFF) 
	{
		r3dParticleTrailData* trail = partTrailCache->Get(P->TrailIdx);

		// fill trail data

		if( PE.TrailDrift > 0.f )
		{
			for( int i = 0 ; i < r3dParticleTrailData::MAX_TRAIL_POINTS ; i ++ )
			{
				trail->PrevPos[ i ] += trail->DriftDir[ i ] * ( PE.TrailDrift * TimePassed );
			}
		}

		trail->AccumDist += (P->Position - prevPos).Length();
		while(trail->AccumDist > PE.TrailStepDist)
		{ 
			trail->AccumDist = 0;

			++trail->CurPos;
			trail->CurPos = trail->CurPos % r3dParticleTrailData::MAX_TRAIL_POINTS;
			if(trail->MaxPos < r3dParticleTrailData::MAX_TRAIL_POINTS) {
				++trail->MaxPos;
			}
			assert(trail->CurPos <  r3dParticleTrailData::MAX_TRAIL_POINTS);
			assert(trail->MaxPos <= r3dParticleTrailData::MAX_TRAIL_POINTS);

			//r3dOutToLog("trail[%02d] added, %f %f\n", trail->CurPos, P->Position.x, P->Position.z);

			trail->PrevPos[trail->CurPos]		= P->Position;
			trail->DriftDir[trail->CurPos]		= r3dPoint3D( u_GetRandom( -1, +1 ), u_GetRandom( -1, +1 ), u_GetRandom( -1, +1 ) ) ;
			trail->SizeCoefs[trail->CurPos]		= u_GetRandom( PE.TrailSizeCoefMin, PE.TrailSizeCoefMax );
			trail->OpacityCoefs[trail->CurPos]	= u_GetRandom( PE.TrailOpacityCoefMin, PE.TrailOpacityCoefMax );
			trail->TimeBorn[trail->CurPos]		= CurTime;
		}

		NumAliveQuads += r3dParticleTrailData::MAX_TRAIL_POINTS - 1;
	}

	if(PE.ParticleType != R3D_PARTICLE_MESH) {
		NumAliveQuads++;
	}

	if(PE.ParticleType == R3D_PARTICLE_RAY && bInheretDirectionFromSystem)
	{
		P->Direction = Direction;
	}

	if(PE.bEmmiter) 
	{
		if( !PE.bEmitterDistanceSpawn )
		{
			LastTimeCreated[ PE.bEmmiterTypeID ] = P->LastSlaveSpawn ;
			AddType(PE.bEmmiterTypeID, CurTime, P->Position, false, true, 1);
			P->LastSlaveSpawn = LastTimeCreated[ PE.bEmmiterTypeID ] ;			
		}
		else
		{
			const r3dParticleEmitter& PE2 = *PD->PType[ PE.bEmmiterTypeID ];

			float emitDelta = PE2.EmitDistance ;

			if( emitDelta > 0.f )
			{
				float initialDistance = P->EmitTravelDistance ;
				float emitTravelDistance = initialDistance ;
				float emitPos = 0.f ;

				while( emitTravelDistance > 0.f )
				{
					r3dPoint3D spawnPos = P->PrevPosition + ( P->Position - P->PrevPosition ) * emitPos / initialDistance ;

					AddType( PE.bEmmiterTypeID, CurTime, spawnPos, true, false, 1 );

					emitTravelDistance -= emitDelta ;
					emitPos += emitDelta ;
				}

				P->EmitTravelDistance = emitTravelDistance ;
			}
		}
	}

	return;
}


R3D_FORCEINLINE void r3dParticleSystem::GetOverTimeParams(const r3dSingleParticle* P, r3dSingleParticle_OverTime* pot)
{
	r3d_assert(PD->PType[P->Type] != NULL);
	const r3dParticleEmitter& PE = *PD->PType[P->Type];

	const float PTime = (PrevTime - P->BirthTime) / P->LifeTime;

	pot->Time	 = PTime;
	pot->Color	 = PE.ColorOverLife.GetColorValue(PTime);
	pot->Color.A	 = (unsigned char)PE.OpacityOverLife.GetFloatValue(PTime);
	pot->BlendMode = PE.BlendModeOverLife.GetFloatValue(PTime);
	pot->Size	 = P->BaseSize     * PE.SizeOverLife.GetFloatValue(PTime);
	pot->Rotation	 = P->BaseRotation + PE.SpinOverLife.GetFloatValue(PTime);
	pot->AngleX    = PE.AngleXOverLife.GetFloatValue(PTime);
	pot->AngleY    = PE.AngleYOverLife.GetFloatValue(PTime);
	pot->AngleZ    = PE.AngleZOverLife.GetFloatValue(PTime);
	pot->RayWidth	 = PE.RayWidth * WholeGlobalScale;

	return;
}

void r3dParticleSystem::Update(float CurTime, bool bUpdate)
{
	struct HackTimerz
	{
		HackTimerz()
		{
			startTime = r3dGetTime();
		}

		~HackTimerz()
		{
			gFillUpdateTime = r3dGetTime() - startTime ;
		}

		float startTime;
	} hackTimerz;

#if 0
	R3DPROFILE_FUNCTION("r3dParticleSystem::Update");
#endif

	r3d_assert(ArraySize);
	//r3dOutToLog("Update at %d %d %d at time %f\n", int(Position.x),int(Position.y),int(Position.z), CurTime);

	TimePassed = CurTime - PrevTime;
	PrevTime      = CurTime;
	if(PD->WarmUpTime>0.0f)
	{
		if(!IsVisible)
		{
			for(int i=0; i<NumAliveParticles; i++)	Array[i].BirthTime += TimePassed;
			for(int i=0; i<r3dParticleData::MAX_EMITTER_SLOTS;i++)	LastTimeCreated[i] += TimePassed;
			return;
		}
		IsVisible = false;

		if(TimePassed > _r3d_ParticleWarmUpDelta*10.0f)
		{
			StartTime = CurTime;
			float dt = TimePassed - _r3d_ParticleWarmUpDelta*10.0f;
			TimePassed = 0.1f;
			for(int i=0; i<NumAliveParticles; i++)	Array[i].BirthTime += dt;
			for(int i=0; i<r3dParticleData::MAX_EMITTER_SLOTS;i++)	LastTimeCreated[i] += dt;
		}
	}

	SourceMoveDelta  = Position - PrevPosition;
	WholeGlobalScale = GlobalScale * PD->OrgGlobalScale;

	if(r_instanced_particles->GetInt()==false)
	{
	if(NumAliveParticles) {
		for(int i=0; i<NumAliveParticles; i++) {
			if((CurTime - Array[i].BirthTime) > Array[i].LifeTime) {
				NumAliveParticles--;
				memmove(&Array[i], &Array[i+1], sizeof(Array[i])*(NumAliveParticles-i));
				i--;
			}
		}
	}}else
	if(NumAliveParticles)
	{
		int disp = 0;
		for(int i=0; i<NumAliveParticles; i++)
		{
			r3dSingleParticle& P = Array[i] ;

			if((CurTime - P.BirthTime) > P.LifeTime)
			{
				if( P.TrailIdx != 0xFFFF )
				{
					partTrailCache->Release( P.TrailIdx );
					P.TrailIdx = 0xFFFF ;
				}

				disp++;
			}
			else
				if(disp>0)	Array[i-disp] = P;
		}

		NumAliveParticles -= disp;
	}

	/* this version is slower, because of more copy when particles is long lived
	// delete outlived particles inside array
	if(NumAliveParticles3) {
	int trg = 0;
	for(int src=0; src<NumAliveParticles3; src++) {
	if((CurTime - Array3[src].BirthTime) > Array3[src].LifeTime) {
	continue;
	}
	if(trg != src) memmove(&Array3[trg], &Array3[src], sizeof(Array3[0]));
	trg++;
	}
	NumAliveParticles3 = trg;
	}
	*/  

	if(PD->EmitTime > 0)
		if(PD->EmitTime < (CurTime - StartTime)) 
			bEmit = 0;

	extern r3dCamera gCam;
	float curLod = (BBox.Org - gCam).Length() / MaxBBoxSize;
	//turn off particles LOD for highest quality settings
	if(r_environment_quality->GetInt() == r_environment_quality->GetMaxVal())	curLod = 1.0f;
	curLod  = curLod < 1.0f ? 1.0f : (curLod >= 3.0f ? 3.0f : curLod);
	int iLod = int(curLod);
	float fade = 1.0f - (curLod - float(iLod));

	for(int i=0; i<r3dParticleData::MAX_EMITTER_SLOTS;i++)
	{
		if(PD->PType[i] == NULL) 
			continue;
		const r3dParticleEmitter& PE = *PD->PType[i];
		if(bEmit || PE.EmitDistance>0) 
		{
			if(!PD->PType[i]->bUsedAsEmitter) 
			{
				if(PE.EmitDistance>0)
				{
					ToEmitWithDistance[i] += SourceMoveDelta.Length()/PE.EmitDistance;
					int numToEmit = int( ToEmitWithDistance[i] );
					r3dVector delta = SourceMoveDelta/(float)numToEmit;
					PTypeSpawned[i] = numToEmit;
					if(numToEmit < 10000) // safe guard
					{
						ToEmitWithDistance[ i ] -= numToEmit ;

						while(numToEmit)
						{
							AddType(i, CurTime, PrevPosition + float(numToEmit)*delta, true, numToEmit==1, iLod);
							--numToEmit;
						}
					}
					else
					{
						ToEmitWithDistance[i] = 0.f ;
					}
				}
				else
				{
					int oldCount = NumAliveParticles ;

					AddType(i, CurTime, Position, false, bUpdate, iLod);

					if( oldCount < NumAliveParticles )
					{
						PTypeSpawned[i] = 1;
					}
				}
			}
		}
	}

	// Process active vertices
	NumAliveQuads = 0;
	

	float minx = FLT_MAX, miny=FLT_MAX, minz=FLT_MAX, maxx=-FLT_MAX, maxy=-FLT_MAX, maxz=-FLT_MAX;

	for(int i=0; i<NumAliveParticles; i++)
	{
		//lod update only for "unphased" effects

		r3dSingleParticle& part = Array[ i ] ;

		if(part.lod+1 < iLod && PD->WarmUpTime>0.0f)
		{
			const r3dParticleEmitter& PE = *PD->PType[part.Type];
			const float PTime = (CurTime - part.BirthTime) / part.LifeTime;
			float PSize       = part.BaseSize*PE.SizeOverLife.GetFloatValue(PTime); //NOTE: it is calculated here AND in GetOverTimeParams to save 4 bytes
			if(PE.ParticleType == R3D_PARTICLE_BEAM) // special case
			{
				const r3dPoint3D& p0 = part.Position ;
				const r3dPoint3D& p1 = BeamTargetPosition ;

				float rayWidth = PE.RayWidth  ;

				minx = R3D_MIN(minx, p0.x - rayWidth );
				miny = R3D_MIN(miny, p0.y - rayWidth );
				minz = R3D_MIN(minz, p0.z - rayWidth );
				minx = R3D_MIN(minx, p1.x - rayWidth );
				miny = R3D_MIN(miny, p1.y - rayWidth );
				minz = R3D_MIN(minz, p1.z - rayWidth );

				maxx = R3D_MAX(maxx, p0.x + rayWidth );
				maxy = R3D_MAX(maxy, p0.y + rayWidth );
				maxz = R3D_MAX(maxz, p0.z + rayWidth );
				maxx = R3D_MAX(maxx, p1.x + rayWidth );
				maxy = R3D_MAX(maxy, p1.y + rayWidth );
				maxz = R3D_MAX(maxz, p1.z + rayWidth );
			}
			else
			{
				minx = R3D_MIN(minx, part.Position.x-PSize);
				miny = R3D_MIN(miny, part.Position.y-PSize);
				minz = R3D_MIN(minz, part.Position.z-PSize);
				maxx = R3D_MAX(maxx, part.Position.x+PSize);
				maxy = R3D_MAX(maxy, part.Position.y+PSize);
				maxz = R3D_MAX(maxz, part.Position.z+PSize);
			}
			continue;
		}
		if(part.Active && PD->PType[part.Type])
		{
			ProcessParticle(&part, CurTime);
			const r3dParticleEmitter& PE = *PD->PType[part.Type];
			const float PTime = (CurTime - part.BirthTime) / part.LifeTime;
			float PSize       = part.BaseSize*PE.SizeOverLife.GetFloatValue(PTime); //NOTE: it is calculated here AND in GetOverTimeParams to save 4 bytes

			if( PE.ParticleType == R3D_PARTICLE_TRAIL )
			{
				float hw = PE.RayWidth * 0.5f ;

				float maxSizeCoef = 0.f ;

				if( part.TrailIdx != 0xFFFF )
				{
					r3dParticleTrailData* ptd =	partTrailCache->Get( part.TrailIdx ) ;

					for( int i = 0, e = ptd->MaxPos ; i < e; i ++ )
					{
						const r3dPoint3D& pos = ptd->PrevPos[ i ] ;

						float sc = ptd->SizeCoefs[ i ] ;

						maxSizeCoef = R3D_MAX( sc, maxSizeCoef ) ;

						minx = R3D_MIN( minx, pos.x - hw * sc );
						miny = R3D_MIN( miny, pos.y - hw * sc );
						minz = R3D_MIN( minz, pos.z - hw * sc );

						maxx = R3D_MAX( maxx, pos.x + hw * sc );
						maxy = R3D_MAX( maxy, pos.y + hw * sc );
						maxz = R3D_MAX( maxz, pos.z + hw * sc );
					}
				}

				minx = R3D_MIN( minx, part.Position.x - hw * maxSizeCoef );
				miny = R3D_MIN( miny, part.Position.y - hw * maxSizeCoef );
				minz = R3D_MIN( minz, part.Position.z - hw * maxSizeCoef );

				maxx = R3D_MAX( maxx, part.Position.x + hw * maxSizeCoef );
				maxy = R3D_MAX( maxy, part.Position.y + hw * maxSizeCoef );
				maxz = R3D_MAX( maxz, part.Position.z + hw * maxSizeCoef );
			}
			else
			{
				if(PE.ParticleType == R3D_PARTICLE_BEAM) // special case
				{
					const r3dPoint3D& p0 = part.Position ;
					const r3dPoint3D& p1 = BeamTargetPosition ;

					float rayWidth = PE.RayWidth  ;

					minx = R3D_MIN(minx, p0.x - rayWidth );
					miny = R3D_MIN(miny, p0.y - rayWidth );
					minz = R3D_MIN(minz, p0.z - rayWidth );
					minx = R3D_MIN(minx, p1.x - rayWidth );
					miny = R3D_MIN(miny, p1.y - rayWidth );
					minz = R3D_MIN(minz, p1.z - rayWidth );

					maxx = R3D_MAX(maxx, p0.x + rayWidth );
					maxy = R3D_MAX(maxy, p0.y + rayWidth );
					maxz = R3D_MAX(maxz, p0.z + rayWidth );
					maxx = R3D_MAX(maxx, p1.x + rayWidth );
					maxy = R3D_MAX(maxy, p1.y + rayWidth );
					maxz = R3D_MAX(maxz, p1.z + rayWidth );
				}
				else
				{
					minx = R3D_MIN(minx, part.Position.x-PSize);
					miny = R3D_MIN(miny, part.Position.y-PSize);
					minz = R3D_MIN(minz, part.Position.z-PSize);
					maxx = R3D_MAX(maxx, part.Position.x+PSize);
					maxy = R3D_MAX(maxy, part.Position.y+PSize);
					maxz = R3D_MAX(maxz, part.Position.z+PSize);
				}
			}
		}
	}

	// not a single particle participated
	if( minx > maxx )
	{
		minx = maxx = miny = maxy = minz = maxz = 0 ;
	}

	BBox.Org.x  = minx;
	BBox.Org.y  = miny;
	BBox.Org.z  = minz;
	BBox.Size.x = R3D_ABS(maxx - minx);
	BBox.Size.y = R3D_ABS(maxy - miny);
	BBox.Size.z = R3D_ABS(maxz - minz);
	if(BBox.Size.Length() > MaxBBoxSize)	MaxBBoxSize = BBox.Size.Length()*2.0f;	//extend box size with hysteresis
	if(BBox.Size.Length()*3 < MaxBBoxSize)	MaxBBoxSize = BBox.Size.Length();	//collaps box when particle effect reduces in size up to 2 times

	PrevPosition  = Position; 
	NumTrisToDraw = 0;

#ifndef FINAL_BUILD
	// HasLight property can be switched only in editor
	{
		if( ParticleLight.pLightSystem && !PD->HasLight )
		{
			TurnLightOff();
		}

		if( !ParticleLight.pLightSystem && PD->HasLight )
		{
			TurnLightOn();
		}
	}
#endif


	float LightT = (CurTime - StartTime)/ PD->LightLifetime;
	if (LightT >1) ParticleLight.TurnOff();

	ParticleLight.bCastShadows = PD->LightCastsShadows;
	ParticleLight.bUseGlobalSSSBParams = false;
	ParticleLight.bSSShadowBlur = PD->LightSSShadowBlur;
	ParticleLight.SSSBParams = PD->SSSBParams;

	ParticleLight.SetColor(PD->LightColor.GetColorValue(LightT));
	ParticleLight.Intensity = PD->LightIntensity;
	ParticleLight.SetRadius(PD->LightRadius1Base*PD->LightRadius1.GetFloatValue(LightT),
							PD->LightRadius2Base*PD->LightRadius1.GetFloatValue(LightT));
	
	ParticleLight.SetPosition(Position+Direction*PD->ParticleLightOffset.Y);
}


//
// particles shading based on light direction
//

r3dVector	gPartShadeDir   = r3dPoint3D(0, 1, 0);
r3dColor	gPartShadeColor = r3dColor(255, 255, 255);

static r3dColor adjCol(const r3dColor& clr, const r3dColor& add, float k)
{
	float r = (float)clr.R + (float)add.R * k;
	float g = (float)clr.G + (float)add.G * k;
	float b = (float)clr.B + (float)add.B * k;

	return r3dColor(R3D_MIN(r, 255.0f), R3D_MIN(g, 255.0f), R3D_MIN(b, 255.0f), (float)clr.A);
}

static r3dColor adjCol(const r3dColor& clr, r3dVector& v, float sunK)
{
	v.Y = 0; v.Normalize();
	float k = gPartShadeDir.Dot(v);
	k = (k + 1.0f) / 2;	// bring [0 - 1]

	//k *= 2;
	//k = 0.5f + k * 1.0f;	// bring [0.5 - 1.5]

	return adjCol(clr, gPartShadeColor, k * sunK);
}


R3D_FORCEINLINE float GetSunK( const r3dPoint3D& pos, const r3dCamera& Cam )
{
	// koef between view angle to particle and sun(shade) direction
	r3dVector v1 = (pos - Cam);
	v1.y = 0;
	if (v1.LengthSq() > 0)
	{
		v1.Normalize();
	}


	float sunK = gPartShadeDir.Dot(v1);
	sunK = (sunK + 1)/2;

	return sunK;
}

template <class T>
static void doQuadShade(const r3dPoint3D* corners, r3dPoint3D& Position, T* V, const r3dColor& Col, float BlendMode, const r3dCamera& Cam )
{
	float sunK = GetSunK( Position, Cam );

	// calc 4 pseudo normals (facing 45' away from quad corners)

	r3dVector n[4];

	n[0] = ((corners[1] - corners[0]) + (corners[3] - corners[0]));
	n[1] = ((corners[2] - corners[1]) + (corners[0] - corners[1]));
	n[2] = ((corners[3] - corners[2]) + (corners[1] - corners[2]));
	n[3] = ((corners[0] - corners[3]) + (corners[2] - corners[3]));
	n[0].Normalize();
	n[1].Normalize();
	n[2].Normalize();
	n[3].Normalize();

	V[0].color = Col; //adjCol(Col, n[0], sunK * BlendMode);
	V[1].color = Col;//adjCol(Col, n[1], sunK * BlendMode);
	V[2].color = Col;//adjCol(Col, n[2], sunK * BlendMode);
	V[3].color = Col;//adjCol(Col, n[3], sunK * BlendMode);
	
	return;
}

//
// particles shading end
//

template < class T >
void r3dParticleSystem::PrepareCommon(const r3dPoint3D* corners, T *V, float BlendMode, const r3dColor24& Col, float UVScroll, float Frame, float Frame1, float Trans, const r3dCamera& Cam )
{
	// set vertices colors
	int shading = 0;
//	if(shading) {
		V[0].color = Col;
		V[1].color = Col;
		V[2].color = Col;
		V[3].color = Col;
//	} else {
//		doQuadShade(corners,Position, V, Col, BlendMode, Cam );
//	}

	AtlasDesc::Rect rect;

	rect = PD->Atlas.rect((int)Frame);
	V[0].tu = rect.minX;
	V[0].tv = rect.minY;

	V[1].tu = rect.maxX;
	V[1].tv = rect.minY;

	V[2].tu = rect.maxX;
	V[2].tv = rect.maxY;

	V[3].tu = rect.minX;
	V[3].tv = rect.maxY;

	rect = PD->Atlas.rect((int)Frame1);
    V[0].tu1 = rect.minX;
	V[0].tv1 = rect.minY;

	V[1].tu1 = rect.maxX;
	V[1].tv1 = rect.minY;

	V[2].tu1 = rect.maxX;
	V[2].tv1 = rect.maxY;

	V[3].tu1 = rect.minX;
	V[3].tv1 = rect.maxY;

	V[0].uvmix = Trans;
	V[1].uvmix = Trans;
	V[2].uvmix = Trans;
	V[3].uvmix = Trans;

	V[0].BlendMode = BlendMode;
	V[1].BlendMode = BlendMode;
	V[2].BlendMode = BlendMode;
	V[3].BlendMode = BlendMode;

	PrepareAdditionalDataVert<T>(V);
}

template <class T>
void r3dParticleSystem::PrepareVerticesCamera(r3dPoint3D* corners, T* V, const r3dCamera &Cam, const r3dPoint3D& Pos, float Size, float rot)
{
	int ang = ((int)rot-45) % 360;
	if(ang<0) ang += 360;

	V[0].Pos = corners[0] = Pos + rotVecU[ang]*Size;
	V[1].Pos = corners[1] = Pos + rotVecR[ang]*Size;
	V[2].Pos = corners[2] = Pos - rotVecU[ang]*Size;
	V[3].Pos = corners[3] = Pos - rotVecR[ang]*Size;
}

template <class T>
void r3dParticleSystem::PrepareVerticesArbitrary(r3dPoint3D* corners, T *V, const r3dPoint3D& Pos, float Size, const D3DXMATRIX* mr)
{
	r3dPoint3D vv[4];
	vv[0] = r3dPoint3D(-1,0, 1)*Size;
	vv[1] = r3dPoint3D( 1,0, 1)*Size;
	vv[2] = r3dPoint3D( 1,0,-1)*Size;
	vv[3] = r3dPoint3D(-1,0,-1)*Size;

	D3DXVec3TransformCoord(vv[0].d3dx(), vv[0].d3dx(), mr);
	D3DXVec3TransformCoord(vv[1].d3dx(), vv[1].d3dx(), mr);
	D3DXVec3TransformCoord(vv[2].d3dx(), vv[2].d3dx(), mr);
	D3DXVec3TransformCoord(vv[3].d3dx(), vv[3].d3dx(), mr);

	V[0].Pos = corners[0] = Pos + vv[0];
	V[1].Pos = corners[1] = Pos + vv[1];
	V[2].Pos = corners[2] = Pos + vv[2];
	V[3].Pos = corners[3] = Pos + vv[3];
}

template <class T>
void r3dParticleSystem::PrepareVerticesRay(r3dPoint3D* corners, T* V, const r3dCamera &Cam, const r3dPoint3D& Pos, const r3dPoint3D& prevPos, float SizeW, float SizeH)
{
	// make vector perpendicular to line and to camera vectors
	r3dVector up =(Pos - prevPos).Cross(Cam - Pos);
	up.Normalize();

	r3dVector dir = Pos - prevPos;
	dir.Normalize();

	r3dPoint3D P1 = Pos;
	r3dPoint3D P2 = Pos + dir*SizeH;

	V[0].Pos = corners[0] = (P2 - up * SizeW / 2);
	V[1].Pos = corners[1] = (P2 + up * SizeW / 2);
	V[2].Pos = corners[2] = (P1 + up * SizeW / 2);
	V[3].Pos = corners[3] = (P1 - up * SizeW / 2);
}


template <class T>
void r3dParticleSystem::PrepareVerticesBeam(r3dPoint3D* corners, T* V, const r3dCamera &Cam, const r3dPoint3D& Pos, const r3dPoint3D& nextPos, float SizeW, float SizeH)
{
	r3dVector dir = nextPos - Pos;
	dir.Normalize();
	r3dVector dirToCamera = Cam - Pos;
	dirToCamera.Normalize();
	r3dVector up =dir.Cross(dirToCamera);

	r3dPoint3D P1 = Pos;
	r3dPoint3D P2 = Pos + dir*SizeH;

	V[0].Pos = corners[0] = (P2 - up * SizeW / 2);
	V[1].Pos = corners[1] = (P2 + up * SizeW / 2);
	V[2].Pos = corners[2] = (P1 + up * SizeW / 2);
	V[3].Pos = corners[3] = (P1 - up * SizeW / 2);
}

template <class T>
void r3dParticleSystem::PrepareVerticesTrail(r3dPoint3D* corners, T* V, const r3dCamera& Cam, const r3dPoint3D& P1, const r3dPoint3D& P2, float SizeW, float SizeH)
{
	// make vector perpendicular to line and to camera vectors
	r3dVector V2 =(P2  - P1).Cross(Cam - P1);
	V2.Normalize();

	V[0].Pos = corners[0] = (P1 - V2 * SizeW / 2);
	V[1].Pos = corners[1] = (P1 + V2 * SizeW / 2);
	V[2].Pos = corners[2] = (P2 + V2 * SizeW / 2);
	V[3].Pos = corners[3] = (P2 - V2 * SizeW / 2);
}

static r3dCamera _PCam;
class ParticleZSort
{
public:
	bool operator()(const r3dSingleParticle& lhs, const r3dSingleParticle& rhs) 
	{
		r3dVector V1, V2;
		V1 = lhs.Position - _PCam;
		V2 = rhs.Position - _PCam;

		// back to front sort
		return V2.LengthSq() < V1.LengthSq();
	}
};


template<class T>
void r3dParticleSystem::FillBuffers(const r3dCamera& Cam)
{
	gNumTrails = 0;

	NumTrisToDraw = 0;
	if(!NumAliveParticles || !NumAliveQuads)	return;

	_PCam = Cam;
	if(PD->ZSortEnabled && NumAliveParticles > 2)	std::sort(&Array[0], &Array[NumAliveParticles], ParticleZSort());

	r3d_assert(NumAliveQuads);

	WORD* Idx = _ParticleIB->Lock(NumAliveQuads * 6, &StartIB);
	WORD StartVertexPos = 0;
	T*	ptr;
	int flg = D3DLOCK_NOOVERWRITE;
	if(_unifiedVBOffset + NumAliveQuads*4*sizeof(T) > UNIFIED_SIZE)
	{
		flg = D3DLOCK_DISCARD;
		_unifiedVBOffset = 0;
		r3dRenderer->Stats.AddNumLocksDiscard ( 1 ) ;
	}
	else
		r3dRenderer->Stats.AddNumLocksNooverwrite ( 1 ) ;

	_unifiedVB->Lock(_unifiedVBOffset, NumAliveQuads*4*sizeof(T), (void**)&ptr, flg);
	r3dRenderer->Stats.AddBytesLocked ( +NumAliveQuads*4*sizeof(T) );
	r3dRenderer->Stats.AddNumLocks ( 1 ) ;

	// keep track of vertex buffer size
	const int VertexBufferSize = NumAliveQuads * 4;


	// create quad vectors for all 360 angles
	{
		D3DXMATRIX mV;
		D3DXMatrixTranspose(&mV, &r3dRenderer->ViewMatrix);
		const r3dVector vr = r3dVector(mV._11, mV._12, mV._13);
		const r3dVector vu = r3dVector(mV._21, mV._22, mV._23);
		const r3dVector vf = r3dVector(mV._31, mV._32, mV._33);

		// if view vectors was changed
		if(vu != camSavedU || vr != camSavedR) {
			camSavedU = vu;
			camSavedR = vr;

			float a = 0.f;

			for(int i=0; i<360; i++, a += 1.f ) {
				float sin_a = sinf(-R3D_DEG2RAD(a));
				float cos_a = cosf(-R3D_DEG2RAD(a));
				r3dRotateVectorAboutVector(vu, rotVecU[i], vf, sin_a, cos_a);
				r3dRotateVectorAboutVector(vr, rotVecR[i], vf, sin_a, cos_a);
			}
		}
	}

	for(int i=0; i<NumAliveParticles; i++)
	{
		const r3dSingleParticle& pt = Array[i];
		if(!pt.Active || PD->PType[pt.Type]==NULL)	continue;
		const r3dParticleEmitter& PE = *PD->PType[pt.Type];
		if(PE.ParticleType == R3D_PARTICLE_MESH)	continue;
		if((NumTrisToDraw * 2) + 4 > VertexBufferSize) {	r3dOutToLog("!!!warning!!! particle vertex buffer overlow\n");	break;	}

		r3dSingleParticle_OverTime pot;
		GetOverTimeParams(&pt, &pot);

		const float PTime = (PrevTime - pt.BirthTime) / pt.LifeTime;
		int StartFrame = PE.StartFrame;
		int NumFrames  = PE.GetNumFramesI();
		float FPS	   = PE.FrameRate;
		if(StartFrame == -1) StartFrame = pt.StaticFrame; 
		int Frame = StartFrame; 
		int Frame1 = StartFrame; 
		float Trans = 0;
		if(NumFrames != 1)
		{
			float Fr1 = pot.Time * pt.LifeTime * FPS;

			if( PE.PlayOnce )
			{
				Frame = int(Fr1);

				if( Frame >= NumFrames )
					Frame = NumFrames - 1;

				Frame1 = Frame + 1;

				if( Frame1 >= NumFrames )
					Frame1 = NumFrames - 1;
			}
			else
			{
				Trans = Fr1 - floor(Fr1);

				Frame = int(Fr1);

				Frame = Frame %(NumFrames);
				Frame += StartFrame;      

				Frame1 = int(Fr1+1);

				Frame1 = Frame1 %(NumFrames);
				Frame1 += StartFrame;     
			}
		}

		const float UVScroll = PE.UVSpeed * (pot.Time * pt.LifeTime);

		r3dPoint3D corners[4];

		int vertsAdded = 0; // number of added vertices (4 means one quad)
		switch(PE.ParticleType)
		{
		case R3D_PARTICLE_CAMERAQUAD:
			{
				PrepareVerticesCamera(corners, ptr, Cam, pt.Position, pot.Size*GlobalRenderScale, pot.Rotation);
				vertsAdded += 4;
				break;
			}

		case R3D_PARTICLE_ARBITARYQUAD:
			{
				D3DXMATRIX  mr;
				D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(pot.AngleX), R3D_DEG2RAD(pot.AngleY), R3D_DEG2RAD(pot.AngleZ));

				if(PE.bDirectionOriented) {
					D3DXMATRIX  mr2;
					GetRotationMatrixFromVector(pt.Direction, &mr2); 
					//TITOV01 note, change pt.Direction to Direction, if whole particle system direction must be used
					mr = mr * mr2;
				}

				PrepareVerticesArbitrary(corners, ptr, pt.Position, pot.Size*GlobalRenderScale, &mr);
				vertsAdded += 4;
				break;
			}

		case R3D_PARTICLE_TRAIL:
			{
				// trailidx can be invalid if particle type switched in editor
				if(pt.TrailIdx == 0xFFFF)
					break;

				r3dParticleTrailData* trail = partTrailCache->Get(pt.TrailIdx);

				int start = trail->CurPos;
				int end   = r3dParticleTrailData::MAX_TRAIL_POINTS - 1;

				// if we have filled whole array, loop around beginning
				if(trail->MaxPos == r3dParticleTrailData::MAX_TRAIL_POINTS) {
					end = start;
				}

				//r3dOutToLog("drawing from %d to %d(max: %d)\n", start, end, trail->MaxPos);

				r3dPoint3D sv = pt.Position;
				int trailIdx = start;
				do 
				{
					// check if we have enough space for next trail quad
					if((NumTrisToDraw * 2) + vertsAdded + 4 > VertexBufferSize)
						break;

					gNumTrails ++;

					PrepareVerticesTrail(corners, ptr + vertsAdded, Cam, sv, trail->PrevPos[trailIdx], pot.RayWidth*GlobalRenderScale, pot.Size*GlobalRenderScale);
					vertsAdded += 4;

					sv = trail->PrevPos[trailIdx];

					if(--trailIdx < 0) {
						trailIdx = r3dParticleTrailData::MAX_TRAIL_POINTS - 1;
					}
				} while(trailIdx != end);
			}
			break;

		case R3D_PARTICLE_RAY:
			{
				r3dPoint3D PrPos = pt.PrevPosition;
				if(pt.Velocity == 0) {
					PrPos = pt.Position - pt.Direction * pot.Size;
				}

				PrepareVerticesRay(corners, ptr, Cam, pt.Position, PrPos, pot.RayWidth*GlobalRenderScale, pot.Size*GlobalRenderScale);
				vertsAdded += 4;
			}
			break;

		case R3D_PARTICLE_BEAM:
			{
				r3dVector PrPos  = Position - BeamTargetPosition;
				float     Length = PrPos.Length();

				PrepareVerticesBeam(corners, ptr, Cam, Position, BeamTargetPosition, pot.RayWidth*GlobalRenderScale, Length);
				vertsAdded += 4;
			}
			break;

		default:
			r3dOutToLog("unknown particle type: %d\n", PE.ParticleType);
			break;
		}

		// form indexes for quads
		while(vertsAdded >= 4) 
		{
			PrepareCommon(corners, ptr, pot.BlendMode, pot.Color, UVScroll, (float)Frame,(float)Frame1, Trans, Cam);

			ptr += 4;
			NumTrisToDraw += 2;

			// 2 triangles indixes
			*Idx++ = StartVertexPos;
			*Idx++ = StartVertexPos+1;
			*Idx++ = StartVertexPos+2;
			*Idx++ = StartVertexPos;
			*Idx++ = StartVertexPos+2;
			*Idx++ = StartVertexPos+3;
			StartVertexPos += 4;

			vertsAdded -= 4;
		}
		r3d_assert(vertsAdded == 0);
	} 

	_unifiedVB->Unlock();
	_ParticleIB->Unlock();

}


class ParticleZSortNew
{
	const r3dSingleParticle* arr;
public:
	ParticleZSortNew(const r3dSingleParticle* _arr) : arr(_arr){};
	bool operator()(unsigned short lhs, unsigned short rhs) 
	{
		r3dVector V1 = arr[lhs].Position - _PCam;
		r3dVector V2 = arr[rhs].Position - _PCam;

		// back to front sort
		return V2.LengthSq() < V1.LengthSq();
	}
};

namespace
{

	struct ParticleFillParams
	{
		PARTICLE_INDEX*			LockArea ;
		r3dSingleParticle*		Array ;
		const r3dParticleData*	PD ;
		r3dParticleSystem*		This ;
		int iLod;	float curLod, fade;

		volatile LONG		FillCount ;
	} ;

	void FillCommonParticleData( const r3dSingleParticle& pt, const r3dParticleEmitter& PE, const r3dParticleData* PD, const r3dSingleParticle_OverTime& pot, r3dParticleSystem* This, PARTICLE_INDEX *p, float clf=1.0f )
	{
		const float PTime = (This->PrevTime - pt.BirthTime) / pt.LifeTime;
		int StartFrame = PE.StartFrame;
		if(StartFrame == -1) StartFrame = pt.StaticFrame; 
		int Frame = StartFrame; 
		int Frame1 = StartFrame; 
		float Trans = 0;

		int inf = PE.GetNumFramesI() ;
		float fnf = PE.GetNumFramesF() ;

		if ( inf > 1)
		{
			float Fr1 = pot.Time * pt.LifeTime * PE.FrameRate / fnf ;

			if( PE.PlayOnce )
			{
				Fr1 = PE.FramesOverLife.GetFloatValue( Fr1 );

				Fr1 *= fnf;

				Frame = int(Fr1);

				if( Frame >= inf )
					Frame = inf - 1;

				Frame1 = Frame + 1;

				if( Frame1 >= inf )
					Frame1 = inf - 1;
			}
			else
			{
				Fr1 -= floor( Fr1 );

				Fr1 = PE.FramesOverLife.GetFloatValue( Fr1 );

				Fr1 *= fnf;

				Trans = Fr1 - floor(Fr1);

				Frame = int(Fr1);

				Frame = Frame % inf;
				Frame += StartFrame;

				Frame1 = int(Fr1+1);

				Frame1 = Frame1 % inf;
				Frame1 += StartFrame;
			}
		}
		if( PD->Atlas.count <= 0 ) Frame = Frame1 = 0;

		p->unified3.R = (BYTE)PE.ParticleType ;

		float rect;

		if( !PD->Atlas.count )
		{
			rect = 0.f ;
		}
		else
		{
			rect = float(Frame % PD->Atlas.count) + 4096.0f*float(Frame1 % PD->Atlas.count) ;
		}


		r3dColor24 c = pot.Color;
		float a = float(c.A) * clf;
		c.A = a > 255.0f ? (BYTE)255 : (BYTE)a;
		p->Color = c;
		//p->Color = pot.Color;

		p->uvmix = Trans;
		p->rect = rect;
		p->blend = pot.BlendMode;
	}
}

/*static*/ void r3dParticleSystem::DoFillBuffers( void* Data, size_t Start, size_t Count )
{
	r3dRenderer->Stats.AddNumParticlesRendered ( Count ) ;

	ParticleFillParams* params = ( ParticleFillParams* )Data;

	r3dSingleParticle* Array	= params->Array ;
	PARTICLE_INDEX* LockArea	= params->LockArea ;
	const r3dParticleData* PD	= params->PD ;
	r3dParticleSystem* This		= params->This ;

	PARTICLE_INDEX* ptr = LockArea + Start ;
	PARTICLE_INDEX* vertStart = ptr ;

	for( uint32_t i = Start, e = Start + Count ; i < e; i ++ )
	{
		const r3dSingleParticle& pt = Array[ gPartIdxArr [ i ] ];
		const r3dParticleEmitter& PE = *PD->PType[ pt.Type ];

		r3dSingleParticle_OverTime pot;
		This->GetOverTimeParams( &pt, &pot );

		ptr->pos = pt.Position;
		ptr->size = pot.Size;

		switch(PE.ParticleType)
		{
		case R3D_PARTICLE_CAMERAQUAD:
			ptr->unified1.x = pot.Rotation;
			ptr->unified1.y = ptr->unified1.z = ptr->unified1.w = 1.0f;	//normalize() generates NaN if 0 passed into R3D_PARTICLE_RAY
			ptr->unified2 = r3dColor24(255,0,0,0);	//normalize() generates NaN if 0 passed into R3D_PARTICLE_ARBITARYQUAD
			break;

		case R3D_PARTICLE_ARBITARYQUAD:
			{
				ptr->unified1.x = pot.AngleX;	ptr->unified1.y = pot.AngleY;	ptr->unified1.z = pot.AngleZ;
				ptr->unified1.w = 1.0f;	//normalize() generates NaN if 0 passed into R3D_PARTICLE_RAY

				r3dPoint3D vF = (pt.Direction.NormalizeTo()+1.0f)*0.5f*255.0f;
				ptr->unified2.R = (unsigned char)vF.x;	ptr->unified2.G = (unsigned char)vF.y;	ptr->unified2.B = (unsigned char)vF.z;
				ptr->unified2.A = PE.bDirectionOriented ? 255 : 0;
			}
			break;

		case R3D_PARTICLE_BEAM:
			{
				r3dVector PrPos  = This->Position - This->BeamTargetPosition;
				float     Length = PrPos.Length();

				ptr->unified1.x = This->BeamTargetPosition.x;	ptr->unified1.y = This->BeamTargetPosition.y; ptr->unified1.z = This->BeamTargetPosition.z;
				ptr->unified1.w = pot.RayWidth;

				ptr->size		= Length;
				ptr->pos		= This->Position;
				ptr->unified2	= r3dColor24(255,0,0,0);	//normalize() generates NaN if 0 passed into R3D_PARTICLE_ARBITARYQUAD
															//unified2.w == 0 gives negative side of up
			}
			break;

		case R3D_PARTICLE_TRAIL:
			{
				r3d_assert( This->EmitersTypes & ( 1 << R3D_PARTICLE_TRAIL ) );

				// trailidx can be invalid if particle type switched in editor
				if(pt.TrailIdx == 0xFFFF)
					break;

				r3dParticleTrailData* trail = partTrailCache->Get(pt.TrailIdx);

				int start = trail->CurPos;
				int end   = r3dParticleTrailData::MAX_TRAIL_POINTS - 1;

				// if we have filled whole array, loop around beginning
				if(trail->MaxPos == r3dParticleTrailData::MAX_TRAIL_POINTS) 
				{
					end = start;
				}

				r3dPoint3D sv = pt.Position;
				int trailIdx = start;

				r3dSingleParticle_OverTime pot;
				r3dSingleParticle spt = pt;

				float coef  = fmodf( trail->AccumDist / PE.TrailStepDist, 1.f ) / r3dParticleTrailData::MAX_TRAIL_POINTS ;

				for(;;)
				{
					// check if we have enough space for next trail quad
					if( ptr - LockArea >= This->NumAliveQuads )
						break;

					const r3dPoint3D& p1 = trail->PrevPos[ trailIdx ] ;
					const r3dPoint3D& p2 = sv ;

					spt.BirthTime = trail->TimeBorn[ trailIdx ] ;

					coef += 1.f / r3dParticleTrailData::MAX_TRAIL_POINTS ;

					This->GetOverTimeParams( &spt, &pot );

					gNumTrails ++;

					ptr->unified1.x = p1.x ; ptr->unified1.y = p1.y ; ptr->unified1.z = p1.z ;
					ptr->unified1.w = pot.RayWidth * 2 ;
					ptr->pos		= p2 ;
					ptr->unified2	= r3dColor24(255,0,255,0) ;
					ptr->size		= pot.Size * trail->SizeCoefs[ trailIdx ] ;

					sv = trail->PrevPos[trailIdx];

					float clf = params->curLod;
					if(spt.lod < params->iLod)	clf *= params->fade;

					float extraFade = R3D_MAX( 1.f - coef * PE.TrailTaleFade, 0.f ) ;

					extraFade = powf( extraFade, PE.TrailTaleFadePow ) ;

					clf *= trail->OpacityCoefs[ trailIdx ] * extraFade ;

					FillCommonParticleData( spt, PE, PD, pot, This, ptr, clf );

					ptr ++;

					if(--trailIdx < 0) {
						trailIdx = r3dParticleTrailData::MAX_TRAIL_POINTS - 1;
					}

					if( trailIdx == end )
						break;
				}

				break;
			}

		case R3D_PARTICLE_RAY:
			{
				r3dPoint3D PrPos = pt.PrevPosition;
				if(pt.Velocity == 0)	PrPos = pt.Position - pt.Direction * pot.Size;
				ptr->unified1.x = PrPos.x;	ptr->unified1.y = PrPos.y;	ptr->unified1.z = PrPos.z;
				ptr->unified1.w = pot.RayWidth;
				ptr->unified2 = r3dColor24(255,0,0,255);	//normalize() generates NaN if 0 passed into R3D_PARTICLE_ARBITARYQUAD
															//unified2.w == 1 gives positive side of up
			}
			break;
		}

		if( PE.ParticleType != R3D_PARTICLE_TRAIL )
		{
			float clf = params->curLod;
			if(pt.lod < params->iLod)	clf *= params->fade;
			FillCommonParticleData( pt, PE, PD, pot, This, ptr, clf );
			ptr ++;
		}
	}

	LONG CountDone = ptr - vertStart ;

	InterlockedExchangeAdd( &params->FillCount, CountDone );
}

void r3dParticleSystem::FillBuffersNew( const r3dCamera& cam, int bOnlyShadowCasters )
{
	R3DPROFILE_FUNCTION( "FillBuffersNew" ) ;

	NumTrisToDraw = 0;
	if(!NumAliveParticles || !NumAliveQuads)	return;

	gNumTrails = 0;

	float curLod = (BBox.Org - cam).Length() / MaxBBoxSize;
	//turn off particles LOD for highest quality settings
	if(r_environment_quality->GetInt() == r_environment_quality->GetMaxVal())	curLod = 1.0f;
	if(r_environment_quality->GetInt() == r_environment_quality->GetMinVal())	curLod = R3D_MAX( 2.0f, curLod ) ;

	curLod  = curLod < 1.0f ? 1.0f : (curLod >= 4.0f ? 4.0f : curLod);
	int iLod = int(curLod);
	float fade = 1.0f - (curLod - float(iLod));

	int nPrt=0;

	r3dParticleEmitter* const * ptypes = PD->PType ;

	if( r_force_parts_transp_shad->GetInt() )
	{
		bOnlyShadowCasters = 0 ;
	}

	for(int i=0; i<NumAliveParticles; i++)
	{
		const r3dSingleParticle& pt = Array[i];
		if(pt.lod+1 < iLod)	continue;
		if(!pt.Active || PD->PType[pt.Type]==NULL) continue;

		const r3dParticleEmitter& PE = *ptypes[pt.Type];
		if(PE.ParticleType == R3D_PARTICLE_MESH) continue;

		if( bOnlyShadowCasters && !PE.bCastsShadows )
			continue ;

		gPartIdxArr[nPrt++] = i;
		if( nPrt > gPartIdxArrSize - 1 )	break;
	}

	nPrt = R3D_MIN( nPrt, gPartIdxArrSize ) ;

	_PCam = cam;
	if(PD->ZSortEnabled && NumAliveParticles > 2)	std::sort(&gPartIdxArr[0], &gPartIdxArr[nPrt], ParticleZSortNew(Array));

	bool hasTrail = EmitersTypes & ( 1 << R3D_PARTICLE_TRAIL ) ? true : false ;

	int numToLock = hasTrail ? NumAliveQuads : nPrt ;

	if(numToLock == 0)
		return;

	PARTICLE_INDEX*	ptr, *vertStart;
	int flg = D3DLOCK_NOOVERWRITE;
	if(_unifiedVBOffset + numToLock*sizeof(PARTICLE_INDEX) > UNIFIED_SIZE)
	{
		flg = D3DLOCK_DISCARD;
		_unifiedVBOffset = 0;
		r3dRenderer->Stats.AddNumLocksDiscard ( 1 ) ;
	}
	else
		r3dRenderer->Stats.AddNumLocksNooverwrite ( 1 ) ;

	_unifiedVB->Lock(_unifiedVBOffset, numToLock*sizeof(PARTICLE_INDEX), (void**)&ptr, flg);
	r3dRenderer->Stats.AddBytesLocked ( +numToLock*sizeof(PARTICLE_INDEX) );
	r3dRenderer->Stats.AddNumLocks ( 1 ) ;

	vertStart = ptr ;

	ParticleFillParams fillParams ;

	fillParams.Array		= Array ;
	fillParams.LockArea		= vertStart ;
	fillParams.PD			= PD ;
	fillParams.This			= this ;

	fillParams.FillCount	= 0;
	fillParams.curLod	= curLod;
	fillParams.iLod	= iLod;
	fillParams.fade	= fade;
	
	DoFillBuffers( &fillParams, 0, nPrt );

	NumTrisToDraw += fillParams.FillCount ;
	_unifiedVB->Unlock();
}





void r3dParticleSystem::DrawMeshParticles()
{
	if ( PD->bDeferredMeshParticles )
		return;

	for(int i=NumAliveParticles-1;i>=0;i--)
	{
		const r3dSingleParticle& pt = Array[i];
		if(!pt.Active || PD->PType[pt.Type]==NULL) 
			continue;

		const r3dParticleEmitter& PE = *PD->PType[pt.Type];

		if(PE.ParticleType != R3D_PARTICLE_MESH)
			continue;

		{
			r3dSingleParticle_OverTime pot;
			GetOverTimeParams(&pt, &pot);
			float halfDepthPixelW = 0;
			float halfDepthPixelH = 0;
			GetParticleDepthDimensions(halfDepthPixelW, halfDepthPixelH);
			halfDepthPixelW = 0.5f / halfDepthPixelW;
			halfDepthPixelH = 0.5f / halfDepthPixelH;

			if(PE.Mesh)
			{
				D3DXMATRIX mr;
				if(PE.bDirectionOriented)
				{
					D3DXMATRIX mAdit;
					D3DXMatrixRotationYawPitchRoll (&mAdit, R3D_DEG2RAD(PE.vDirOrientedAdditRotation.x), R3D_DEG2RAD(PE.vDirOrientedAdditRotation.y - 90), R3D_DEG2RAD(PE.vDirOrientedAdditRotation.z) );

					r3dVector V = pt.PrevPosition - pt.Position;
					if((V.x*V.x + V.y*V.y + V.z*V.z) >= R3D_EPSILON) 
						GetRotationMatrixFromVector(V, &mr, 0);
					else
						D3DXMatrixIdentity(&mr);

					mr = mAdit * mr;
				}
				else
				{
					D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(pot.AngleY), R3D_DEG2RAD(pot.AngleX), R3D_DEG2RAD(pot.AngleZ));
				}

				// scale mesh
				r3dVector vMeshSize = PE.Mesh->localBBox.Size;
				float fScale = r3dTL::Max ( r3dTL::Max ( vMeshSize.x, vMeshSize.y ), vMeshSize.z );

				r3dMaterial* meshMat = PE.Mesh->MatChunks[ 0 ].Mat;

				if( meshMat )
					r3dRenderer->SetThumbTex( meshMat->Texture, 0 );
				else
					r3dRenderer->SetTex( NULL );

				if(PE.bMeshUseDistortTexture && meshMat )
					r3dRenderer->SetTex( meshMat->NormalTexture, 2 );
				else
					r3dRenderer->SetTex( g_BlankDistortTex, 2 );

				PE.Mesh->SetShaderConsts(pt.Position, (1.0f/fScale)*GlobalRenderScale*r3dVector(pot.Size,pot.Size,pot.Size), mr);

				float UVScroll = PE.UVSpeed * (pot.Time * pt.LifeTime);
				D3DXVECTOR4 vConsts[] = 
				{
					D3DXVECTOR4(float(pot.Color.R)/255.0f, float(pot.Color.G)/255.0f, float(pot.Color.B)/255.0f, float(pot.Color.A)/255.0f),
					D3DXVECTOR4(halfDepthPixelW, halfDepthPixelH, 0, 0)
				};
				D3DXVECTOR4 UVSpeed(UVScroll, pot.BlendMode, 1.f / PD->DepthBlendValue, 0);

				r3dRenderer->pd3ddev->SetVertexShaderConstantF(29, &vConsts[0].x, _countof(vConsts));
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(18,(float *)&UVSpeed, 1);

				if(PE.bMeshDisableCulling)
					r3dRenderer->SetCullMode( D3DCULL_NONE );
				PE.Mesh->DrawMeshSimple( 0 );
				if(PE.bMeshDisableCulling)
					r3dRenderer->RestoreCullMode();
			}
		}
	}

	return;
}

void r3dSetParticlesDepthRef( r3dScreenBuffer* FullSizeBuffer, r3dScreenBuffer* HalfSizeBuffer )
{
	FullSizeDepth = FullSizeBuffer ;
	HalfSizeDepth = HalfSizeBuffer ;
}

void ActivateParticleDepth()
{
	if( r_half_depth_particles->GetBool() )
	{
		HalfSizeDepth->Activate() ;
	}
	else
	{
		FullSizeDepth->Activate() ;
	}
}

void DeactivateParticleDepth()
{
	if( r_half_depth_particles->GetBool() )
	{
		HalfSizeDepth->Deactivate() ;
	}
	else
	{
		FullSizeDepth->Deactivate() ;
	}
}

void RemoveParticleDepth()
{
	r3dRenderer->SetTex( 0, 1 );
}

void SetParticleDepth()
{
	if( r_half_depth_particles->GetBool() )
	{
		r3dRenderer->SetTex( HalfSizeDepth->Tex, 1 );
	}
	else
	{
		r3dRenderer->SetTex( FullSizeDepth->Tex, 1 );
	}
}

void GetParticleDepthDimensions(float &w, float &h)
{
	if (r_half_depth_particles->GetBool())
	{
		w = HalfSizeDepth->Width;
		h = HalfSizeDepth->Height;
	}
	else
	{
		w = FullSizeDepth->Width;
		h = FullSizeDepth->Height;
	}
}

struct LightAndScore
{
	r3dLight* light ;
	int score ;
};

static int light_sort_func( const LightAndScore& l0, const LightAndScore& l1 )
{
	return l0.score > l1.score ;
}

static r3dTL::TArray< LightAndScore > g_LightsAndScores ;

R3D_FORCEINLINE int NeedDistForGlow( const r3dParticleData* PD )
{
	int needDistForGlow =	PD->GlowIntensity > 0.001953125f
								&&
							r_glow->GetInt()
								||
							!r_smart_transp_distort->GetInt() ;

	return needDistForGlow ;
}

R3D_FORCEINLINE int NeedDistortForTex( const r3dParticleData* PD )
{
	return PD->bDistort || !r_smart_transp_distort->GetInt() ;
}

R3D_FORCEINLINE int NeedDistort( const r3dParticleData* PD )
{
	return r_distort->GetInt() && ( NeedDistForGlow( PD ) || NeedDistortForTex( PD ) ) ;
}

R3D_FORCEINLINE void ShaderIDFillDistort( ParticlePShaderId* id, const r3dParticleData* PD )
{
	if( r_distort->GetInt() )
	{
		int forTex = NeedDistortForTex( PD ) ;

		id->distort_tex = forTex ? 1 : 0 ;
		id->distort_buf	= forTex || NeedDistForGlow( PD ) ? 1 : 0 ;
	}
	else
	{
		id->distort_tex = 0 ;
		id->distort_buf	= 0 ;
	}
}

void r3dParticleSystem::Draw( const r3dCamera &cam, bool bShadowMap )
{
	struct HackTimerz
	{
		HackTimerz()
		{
			startTime = r3dGetTime();
		}

		~HackTimerz()
		{
			gFillBufferTime = r3dGetTime() - startTime ;
		}

		float startTime;
	} hackTimerz;

	if( !PD->Texture->IsDrawable() || 
		( PD->DistortTexture && !PD->DistortTexture->IsDrawable() ) ||
		( PD->NormalTexture && !PD->NormalTexture->IsDrawable() )
		)
	{
		return ;
	}

	// sync with ParticleNew_vs.hls
	const int MAX_PARTICLE_LIGHTS = 4 ;
	const int CONSTS_PER_LIGHT = 3 ;

	D3DXVECTOR4 lightParams[ MAX_PARTICLE_LIGHTS * CONSTS_PER_LIGHT ];
	memset( lightParams, 0, sizeof lightParams ) ;

	int needLights = PD->bAcceptsDynamicLights && !bShadowMap ;

	if( needLights )
	{
		WorldLightSystem.FillVisibleArrayWithLightsInsideBox( BBox ) ;		

		g_LightsAndScores.Resize( WorldLightSystem.nVisibleLights ) ;

		r3dPoint3D centre = BBox.Center() ;

		int usedLightCount = 0 ;

		for (int i = 0 ; i < WorldLightSystem.nVisibleLights; ++i )
		{
			LightAndScore& lsc = g_LightsAndScores[ usedLightCount ] ;

			r3dLight* l = WorldLightSystem.VisibleLights[ i ] ;

			if( !l || !l->IsOn() )
				continue ;

			float delta = ( *l - centre ).Length() ;

			lsc.light = l ;
			lsc.score = int( 16384.0f * l->Radius2 * l->Intensity * ( l->R + l->G + l->B ) / delta  ) ;

			usedLightCount ++ ;
		}

		if( usedLightCount )
		{
			g_LightsAndScores.Resize( usedLightCount ) ;

			std::sort( &g_LightsAndScores[ 0 ], &g_LightsAndScores[ 0 ] + usedLightCount, light_sort_func ) ;

			int numLightsSet = std::min( MAX_PARTICLE_LIGHTS, (int)g_LightsAndScores.Count() );

			for (int i = 0 ; i < numLightsSet; ++i )
			{
				r3dLight *l = g_LightsAndScores[ i ].light ;
				float intens = l->Intensity ;

				//	Pos
				lightParams[ i * CONSTS_PER_LIGHT + 0 ] = D3DXVECTOR4(l->x, l->y, l->z, 0);
				//	Attenuation params x = 1 / (endFade - startFade); y = startFade / (endFade - startFade); zw = unused
				float invFadeDistance = 1.0f / (l->GetOuterRadius() - l->GetInnerRadius());
				lightParams[ i * CONSTS_PER_LIGHT + 1 ] = D3DXVECTOR4(invFadeDistance, l->GetInnerRadius() * invFadeDistance, 0, 0);
				//	Color
				lightParams[ i * CONSTS_PER_LIGHT + 2 ] = D3DXVECTOR4( intens * l->R / 255, intens * l->G / 255, intens * l->B / 255, 1);
			}
		}
	}

	// ParticleLight lights[ MAX_LIGHTS ] : register( c40 ) ;
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 40, &lightParams->x, R3D_ARRAYSIZE( lightParams ) ) ) ;


	IsVisible = true;
	int halfRes = r_half_res_particles->GetInt() || r_half_depth_particles->GetBool();

	R3DPROFILE_FUNCTION("r3dParticleSystem::Draw");
	if(!NumAliveParticles)
		return;

	r3dRenderer->Flush();
	r3dRenderer->SetMaterial(NULL);

	D3DPERF_BeginEvent( 0x0, L"r3dParticleSystem::Draw" );

	//just cameraquad without bump is supported by instance rendering

	if( r_instanced_particles->GetInt() )
	{
		FillBuffersNew( cam, bShadowMap );
	}
	else
	{
		if ( PD->bNormalMap )
			FillBuffers<R3D_PARTICLE_VERTEX_BUMP>(cam);
		else
			FillBuffers<R3D_PARTICLE_VERTEX>(cam);
	}

	D3DXMATRIX 	mWorld;
	D3DXMATRIX	mWVP;
	D3DXVECTOR4	FogCamVec(cam.X,cam.Y,cam.Z,1.0f/(r3dRenderer->Fog.End-r3dRenderer->Fog.Start));

	D3DXVECTOR4 vLight (0,0,0,0);
	D3DXVECTOR4 vNormal_BumpPower (0,0,0,0);
	
	{
		r3dVector vNrm = -cam.vPointTo;
		vNrm.Normalize();
		memcpy ( &vNormal_BumpPower, vNrm.d3dx (), sizeof ( r3dVector ) ); //-V512

		vNormal_BumpPower.w = 1.0f - PD->fBumpPower;


		r3dVector sunDir = g_SunDir;
		sunDir.Normalize();
		memcpy ( &vLight, sunDir.d3dx (), sizeof ( r3dVector ) ); //-V512
	}
	
	D3DXMatrixIdentity(&mWorld);
	mWVP = mWorld * r3dRenderer->ViewProjMatrix ;
	D3DXMatrixTranspose(&mWVP, &mWVP);

	float invDepthW = 0;
	float invDepthH = 0;
	GetParticleDepthDimensions(invDepthW, invDepthH);
	invDepthW = 1.0f / invDepthW;
	invDepthH = 1.0f / invDepthH;

	r3dColor cAmbient = r3dRenderer->AmbientColor;
	r3dColor cSun = r3dGameLevel::Environment->GetCurrentSunColor();

	D3DXVECTOR4 vAmbient( float(cAmbient.R)/255.f, float(cAmbient.G)/255.f,float(cAmbient.B)/255.f, 0.f ) ;

	vAmbient.x = powf( vAmbient.x, 2.2f ) ;
	vAmbient.y = powf( vAmbient.y, 2.2f ) ;
	vAmbient.z = powf( vAmbient.z, 2.2f ) ;

	D3DXVECTOR4 vSun( float(cSun.R)/255.f,float(cSun.G)/255.f,float(cSun.B)/255.f,0 ) ;

	vSun.x = powf( vSun.x, 2.2f ) ;
	vSun.y = powf( vSun.y, 2.2f ) ;
	vSun.z = powf( vSun.z, 2.2f ) ;

	float smWidth = g_ShadowMap ? g_ShadowMap->Width : 0;
	float smHeight = g_ShadowMap ? g_ShadowMap->Height : 0;
	float invSmWidth = g_ShadowMap ? 1 / g_ShadowMap->Width : 0;
	float invSmHeight = g_ShadowMap ? 1 / g_ShadowMap->Height : 0;

	D3DXVECTOR4 vConsts[] = 
	{
		D3DXVECTOR4( invDepthW, invDepthH, 0.f, 0.f ),
		// float4 vNormal_BumpPower : register(c1); 
		vNormal_BumpPower,
		// float3 vLight : register(c2); // light vector
		vLight,
		// float4 DepthFade	: register(c3);
		D3DXVECTOR4( 1.f / PD->DepthBlendValue, 0.f, 0.f, 0.f ),
		// float4 GlowControl	: register(c4);
		D3DXVECTOR4( PD->GlowFromDistortB_or_FromDiffuseA, PD->GlowIntensity, R3D_MIN( PD->GlowIntensity * 16.f, 1.f ), 0.f ),
		// float4 ShadowControl : register(c5);
		D3DXVECTOR4( r3dGameLevel::Environment->ParticleShadingCoef, 0.f, smWidth, smHeight ),

		// float4 SunColor : register(c6);
		D3DXVECTOR4( vSun  ),
		// float4 AmbientColor : register(c7);
		D3DXVECTOR4( vAmbient ),
		// float4 ShadowSteps : register(c8);
		D3DXVECTOR4( invSmWidth, 0.0f, 0.f, invSmHeight)
	};

	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&mWVP, 4);
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 15,(float *)&FogCamVec, 1);
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 27,(float *)&vNormal_BumpPower, 1);
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 28,(float *)&vLight, 1);	
	D3DXVECTOR4 halfDepthUVOffset(vConsts[0] * 0.5f);
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 29,(float *)&halfDepthUVOffset, 1);	

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0,(float *)vConsts, R3D_ARRAYSIZE(vConsts) );

	// First draw mesh particles
	// shadow map is filled in other call for mesh particles.

	if( !bShadowMap )
	{
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);

		r3dRenderer->pd3ddev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		r3dRenderer->SetVertexShader(ParticleVShaderMesh_ID);

		if( r_distort->GetInt() )
			r3dRenderer->SetPixelShader(ParticlePShaderMesh_ID[halfRes]);
		else
			r3dRenderer->SetPixelShader(ParticlePShaderMesh_NODISTORT_ID[halfRes]);

		r3dRenderer->SetTex(FullSizeDepth->Tex, 1);	

		r3dSetFiltering( R3D_POINT, 1 );
		r3dSetFiltering( R3D_BILINEAR, 2 );

		D3DPERF_BeginEvent( 0x0, L"r3dParticleSystem::DrawMeshParticles" );
		DrawMeshParticles();
		D3DPERF_EndEvent ();
	}

	if(NumTrisToDraw==0)	return;

	// someone is overwriting this constant after rendering meshes
 	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 15,(float *)&FogCamVec, 1);

	D3DXMATRIX matTexScale1;
	D3DXMATRIX VP = r3dRenderer->ViewProjMatrix ;

	// Create texture scaling matrix:
	// | 0.5      0        0        0 |
	// | 0        -0.5     0        0 |
	// | 0        0        0        0 |
	// | 0.5      0.5      1        1 |

	// This will scale and offset -1 to 1 range of x, y
	// coords output by projection matrix to 0-1 texture
	// coord range.
	D3DXMatrixIdentity(&matTexScale1);

	matTexScale1._11 = 0.5f;
	matTexScale1._22 = -0.5f; 
	matTexScale1._33 = 0.0f; 
	matTexScale1._41 = 0.5f; 
	matTexScale1._42 = 0.5f;
	matTexScale1._43 = 1.0f; 
	matTexScale1._44 = 1.0f;

	D3DXMATRIX mat ;
	D3DXMatrixMultiply(&mat, &VP, &matTexScale1);
	D3DXMatrixTranspose(&mat, &mat);

	r3dRenderer->pd3ddev->SetVertexShaderConstantF(20 ,(float *)&mat,  4);


	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&mWVP, 4);
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 12,(float *)&FogCamVec, 1);

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_SRCBLEND, 		D3DBLEND_ONE);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA);

	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

	if(!bRenderUntextured)
		r3dRenderer->SetTex(PD->Texture);
	else
		r3dRenderer->SetTex(__r3dShadeTexture[2]);

	if( !bShadowMap )
	{
		if(PD->bDistort )
			r3dRenderer->SetTex(PD->DistortTexture, 2);
		else
			r3dRenderer->SetTex(g_BlankDistortTex, 2);

		if(PD->bNormalMap)
			r3dRenderer->SetTex(PD->NormalTexture, 3);
		else
			r3dRenderer->SetTex(NULL, 3);

		SetParticleDepth();

		if( PD->bReceivesShadows && r_shadows->GetInt() && g_ShadowMap )
		{
			r3dRenderer->SetTex( g_ShadowMap->Tex, 4 ) ;

			if( r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF )
			{
				r3dSetFiltering( R3D_BILINEAR, 4 );
			}
			else
			{
				r3dSetFiltering( R3D_POINT, 4 );
			}
		}

		r3dSetFiltering( R3D_POINT, 1 );
		r3dSetFiltering( R3D_BILINEAR, 2 );

		// TODO : eliminate.
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, 	FALSE);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAREF,        	5);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAFUNC, 			D3DCMP_GREATEREQUAL);

		extern void SetVolumeFogParams();
		SetVolumeFogParams();

		r3dTransparentSetDistort( NeedDistort( PD ) ) ;
	}

	r3dRenderer->SetCullMode( D3DCULL_NONE );

	if( r_instanced_particles->GetInt() )
	{
		int shadowType = 0 ;

		if( PD->bReceivesShadows && r_shadows->GetInt() && r_particle_shadows->GetInt() && !bShadowMap )
		{
			shadowType = r_hardware_shadow_method->GetInt() == HW_SHADOW_METHOD_HW_PCF ? 2 : 1 ;
		}

		{
			const D3DXMATRIX& mV = r3dRenderer->ViewMatrix;

			float vConsts[ 10 ][ 4 ] = {
				mV._11, mV._12, mV._13, 0.0f,
				mV._21, mV._22, mV._23, 0.0f,
				mV._31, mV._32, mV._33, 0.0f,

				GetSunK( Position, cam ), GlobalRenderScale, 0.f, 0.f,

				gPartShadeDir.x, gPartShadeDir.y, gPartShadeDir.z, 1.0f,

				float(gPartShadeColor.R)/255.0f, float(gPartShadeColor.G)/255.0f, float(gPartShadeColor.B)/255.0f, 1.0f,
			};

			int VSConstCount = 6 ;

			if( shadowType )	
			{
				D3DXMatrixTranspose( (D3DXMATRIX*)( vConsts + VSConstCount ), &g_LightMtx ) ;
				VSConstCount += 4 ;
			}

			r3d_assert( VSConstCount <= R3D_ARRAYSIZE( vConsts ) ) ;

			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 30, (float*)vConsts, VSConstCount ) );

		}

		if(PD->Atlas.count>0)
		{
			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF(64, &PD->Atlas.rects[0].minX, PD->Atlas.count) );
		}
		else
		{
			D3DXVECTOR4 temp = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
			r3dRenderer->pd3ddev->SetVertexShaderConstantF(41, &temp.x, 1);
		}

		if( bShadowMap )
		{
			// float vWriteShadowParams : register( c52 ) ;
			D3DXVECTOR4 vsConst ( r_transp_shadow_coef->GetFloat(), 0.f, 0.f, 0.f ) ;
			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 52, (float*)vsConst, 1 ) ) ;

			r3dRenderer->SetRenderingMode( R3D_BLEND_MIN ) ;

			r3dRenderer->SetVertexShader( ParticleInstVShaderFillSM_ID );
			r3dRenderer->SetPixelShader( ParticleInstPShaderFillSM_ID );			
		}
		else
		{
			if( PD->bNormalMap )
			{
				if( shadowType )
				{
					r3dRenderer->SetVertexShader( ParticleInstVShaderNMShadow_ID );
				}
				else
				{
					r3dRenderer->SetVertexShader( ParticleInstVShaderNM_ID );
				}
			}
			else
			{
				if( shadowType )
				{
					r3dRenderer->SetVertexShader( ParticleInstVShaderShadow_ID );
				}
				else
				{
					r3dRenderer->SetVertexShader( ParticleInstVShader_ID );
				}
			}

			ParticlePShaderId id ;

			ShaderIDFillDistort( &id, PD ) ;

			id.normal		= PD->bNormalMap ;
			id.half_res		= halfRes ;
			id.shadow_type	= shadowType ;

			r3dRenderer->SetPixelShader( gParticlePSIds[ id.Id ] ) ;
		}

		d3dc._SetDecl(getInstanceVertexDecl());

		d3dc._SetIndices(_instanceIB.Get()) ;

		r3dRenderer->SetStreamSourceFreq( 0, ( D3DSTREAMSOURCE_INDEXEDDATA | ( UINT )NumTrisToDraw )) ;

		d3dc._SetStreamSource( 0, _instanceVB.Get(), 0, sizeof(PARTICLE_INSTANCE) );
		r3dRenderer->SetStreamSourceFreq( 1, ( D3DSTREAMSOURCE_INSTANCEDATA | 1 )) ;
		d3dc._SetStreamSource(1, _unifiedVB.Get(), _unifiedVBOffset, sizeof(PARTICLE_INDEX));

		_unifiedVBOffset += NumTrisToDraw*sizeof(PARTICLE_INDEX);

		r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST, 0,	0, 4, 0, 2);

		r3dRenderer->SetStreamSourceFreq( 0, 1 ) ;
		r3dRenderer->SetStreamSourceFreq( 1, 1 ) ;
		d3dc._SetStreamSource(1, 0, 0, 0);
	}
	else
	{
		_ParticleIB->Activate();
		// no distort means weak card so no normals too ( normal map shader doesn't support no distort! )

		ParticlePShaderId id ;

		ShaderIDFillDistort( &id, PD ) ;

		id.normal		= PD->bNormalMap ;		
		id.half_res		= halfRes ;
		id.shadow_type	= 0 ;

		r3dRenderer->SetPixelShader( gParticlePSIds[ id.Id ] ) ;

		if ( PD->bNormalMap && r_distort->GetInt() )
		{
			r3dRenderer->SetVertexShader(ParticleNormalMapVShader_ID);

			d3dc._SetDecl(R3D_PARTICLE_VERTEX_BUMP::getDecl());
			d3dc._SetStreamSource(0, _unifiedVB.Get(), _unifiedVBOffset, sizeof(R3D_PARTICLE_VERTEX_BUMP));
			_unifiedVBOffset += NumTrisToDraw*2*sizeof(R3D_PARTICLE_VERTEX_BUMP);
		}
		else
		{
			r3dRenderer->SetVertexShader(ParticleVShader_ID);

			d3dc._SetDecl(R3D_PARTICLE_VERTEX::getDecl());
			d3dc._SetStreamSource(0, _unifiedVB.Get(), _unifiedVBOffset, sizeof(R3D_PARTICLE_VERTEX));
			_unifiedVBOffset += NumTrisToDraw*2*sizeof(R3D_PARTICLE_VERTEX);
		}
		r3dRenderer->DrawIndexed(	D3DPT_TRIANGLELIST, 
									0,		// base VB index
									0,		// min index
									NumTrisToDraw*2,	// num verts to draw
									StartIB,		// start index
									NumTrisToDraw);	// numtrtis

	}

	r3dRenderer->Stats.AddNumDraws( 1 );
	r3dRenderer->RestoreCullMode();

	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

	r3dRenderer->SetTex(0, 7); // remove skydome cubemap texture
	r3dRenderer->SetVertexShader(-1);
	r3dRenderer->SetPixelShader(-1);
	d3dc._SetDecl(R3D_MESH_VERTEX::getDecl());
	r3dSetFiltering( R3D_BILINEAR, 1 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_COPY_ZCW);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);

	{ // reset world transform
		D3DXMATRIX  mWorld;
		D3DXMatrixIdentity(&mWorld);
		r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld);
	}

	D3DPERF_EndEvent();

	// r3dRenderer->SetMaterial(NULL);
	return;
}

void r3dParticleSystem::DrawDefferedMeshes(const r3dCamera &Cam, bool bShadowMap)
{
	if ( !PD->bDeferredMeshParticles )
		return;

	if ( !PD->bDeferredMeshParticlesShadows && bShadowMap )
		return;

	D3DXVECTOR4 B;
	B = D3DXVECTOR4(0.0f,0.0f,0.5f,1.0f);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  MC_MATERIAL_PARAMS, (float *)&B,  1 );

	B = D3DXVECTOR4( -0.5f, 0.5f, 1.0f, 1.0f );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_MAT_GLOW, (float*)&B, 1 );

	B = D3DXVECTOR4(Cam.x,Cam.y,Cam.z,1.0f);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  MC_CAMVEC, (float *)&B,  1 );

	B =  D3DXVECTOR4 ( 0.0f, r3dRenderer->ProjMatrix._43, r3dRenderer->ProjMatrix._33, 0.0f );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_DDEPTH, (float *)&B, 1 );


	for(int i=NumAliveParticles-1;i>=0;i--)
	{
		const r3dSingleParticle& pt = Array[i];
		if(!pt.Active) 
			continue;

		r3d_assert(PD->PType[pt.Type] != NULL);
		const r3dParticleEmitter& PE = *PD->PType[pt.Type];

		if(PE.ParticleType != R3D_PARTICLE_MESH)
			continue;

		{
			r3dSingleParticle_OverTime pot;
			GetOverTimeParams(&pt, &pot);

			if(PE.Mesh)
			{
				B = D3DXVECTOR4 (float(pot.Color.R)/255.0f, float(pot.Color.G)/255.0f, float(pot.Color.B)/255.0f, float(pot.Color.A)/255.0f);
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(  MC_MAT_DIFFUSE, (float *)&B,  1 );
				B = D3DXVECTOR4 (0.0f, 0.0f, 0.0f, 0.0f);
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(  MC_MAT_SPECULAR, (float *)&B,  1 );

				D3DXMATRIX mr;
				if(PE.bDirectionOriented)
				{
					D3DXMATRIX mAdit;
					D3DXMatrixRotationYawPitchRoll (&mAdit, R3D_DEG2RAD(PE.vDirOrientedAdditRotation.x), R3D_DEG2RAD(PE.vDirOrientedAdditRotation.y - 90), R3D_DEG2RAD(PE.vDirOrientedAdditRotation.z) );

					r3dVector V = pt.PrevPosition - pt.Position;
					if((V.x*V.x + V.y*V.y + V.z*V.z) >= R3D_EPSILON) 
						GetRotationMatrixFromVector(V, &mr, 0);
					else
						D3DXMatrixIdentity(&mr);

					mr = mAdit * mr;
				}
				else
				{
					D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(pot.AngleY), R3D_DEG2RAD(pot.AngleX), R3D_DEG2RAD(pot.AngleZ));
				}

				// scale mesh
				r3dVector vMeshSize = PE.Mesh->localBBox.Size;
				float fScale = r3dTL::Max ( r3dTL::Max ( vMeshSize.x, vMeshSize.y ), vMeshSize.z );
				
				PE.Mesh->SetShaderConsts(pt.Position, (1.0f/fScale)*GlobalRenderScale*r3dVector(pot.Size,pot.Size,pot.Size), mr);
				
				if(PE.bMeshDisableCulling)
					r3dRenderer->SetCullMode( D3DCULL_NONE );
				if( bShadowMap )
					PE.Mesh->DrawMeshShadows();
				else
				{
					r3dMaterial::SetVertexShader( R3D_MATVS_DEFAULT, 0  );

					FillbufferShaderKey key;

					SetLQ( &key ) ;

					key.flags.alpha_test = 1;

					SetFillGBufferPixelShader( key );

					PE.Mesh->DrawMeshStart( &pot.Color );

					for( int i = 0, e = PE.Mesh->GetNumMaterials(); i < e; i ++ )
					{
						PE.Mesh->DrawMeshDeferredBatch( i, 0 );
					}

					PE.Mesh->DrawMeshEnd();
				}
				if(PE.bMeshDisableCulling)
					r3dRenderer->RestoreCullMode();
			}
		}
	}

	return;
}

//------------------------------------------------------------------------

void
r3dParticleSystem::ClearParticlesOfType( BYTE type )
{
	for( int i = 0; i < NumAliveParticles; )
	{
		r3dSingleParticle& p = Array[ i ];

		if( p.Type == type )
		{
			NumAliveParticles --;
			memmove( &Array[i], &Array[i+1], sizeof(Array[i])*(NumAliveParticles-i) );
		}
		else
		{
			i ++;
		}
	}
}

//------------------------------------------------------------------------

void r3dParticleSystem::TurnLightOff()
{
	r3d_assert( ParticleLight.pLightSystem );

	ParticleLight.TurnOff();
	WorldLightSystem.Remove( &ParticleLight );
}

//------------------------------------------------------------------------

void r3dParticleSystem::TurnLightOn()
{
	r3d_assert( !ParticleLight.pLightSystem );

	ParticleLight.TurnOn();
	WorldLightSystem.Add(&ParticleLight);
}

//------------------------------------------------------------------------

r3dParticleTrailData::r3dParticleTrailData()
{
	COMPILE_ASSERT( r3dTL::IsPOD<r3dParticleTrailData>::Value );
	memset( this, 0, sizeof(*this) );
}

//------------------------------------------------------------------------

void ParticlePShaderId::ToString( char* str )
{
	strcpy( str, "PS_PARTICLE" ) ;
	if( normal ) strcat( str, "_NORMAL" ) ;
	if( distort_buf ) strcat( str, "_DISTBUF" ) ;
	if( distort_tex ) strcat( str, "_DISTTEX" ) ;
	if( half_res ) strcat( str, "_HALFRES" ) ;
	if( shadow_type == 1 ) strcat( str, "_SHADOW" ) ;
	if( shadow_type == 2 ) strcat( str, "_SHADOW_HW" ) ;
}

void ParticlePShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 5 ) ;

	defines[ 0 ].Name = "USE_NORMALMAP" ;
	defines[ 0 ].Definition = normal ? "1" : "0" ;

	defines[ 1 ].Name = "USE_DISTORT" ;
	defines[ 1 ].Definition = distort_buf ? "1" : "0" ;

	defines[ 2 ].Name = "USE_DISTORT_TEXTURE" ;
	defines[ 2 ].Definition = distort_tex ? "1" : "0" ;

	defines[ 3 ].Name = "HALF_RES_PARTICLES" ;
	defines[ 3 ].Definition = half_res ? "1" : "0" ;

	defines[ 4 ].Name = "SHADOW_TYPE" ;

	if( shadow_type == 0 ) defines[ 4 ].Definition = "0" ;
	if( shadow_type == 1 ) defines[ 4 ].Definition = "1" ;
	if( shadow_type == 2 ) defines[ 4 ].Definition = "2" ;
}
