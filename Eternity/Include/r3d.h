#ifndef	__R3D__H
#define	__R3D__H

// Version Variable:
// MUST be changed if some global changes in data structure happens

#define	R3D_ETERNITY_VERSION	0x00030001

class   _r3d;
class   r3dPoint2D;
class   r3dPoint3D;
typedef r3dPoint3D r3dVector;
class   r3dCamera;
class   r3dLight;
class   r3dLightSystem;
class   r3dMesh;
class   r3dGrannyMesh;
class   r3dTexture;
class   r3dThumbnailTexture;
class   r3dTexturePool;
class   r3dRenderLayer;

#include "r3dSys_WIN.h"

#include "r3dAssert.h"
#include "r3dHash.h"
#include "r3dString.h"

class r3dCSHolder
{
public:
	CRITICAL_SECTION& cs;

#if R3D_ENABLE_STALL_DETECTION
	void EnterWithStallDetection( CRITICAL_SECTION& cs );
#endif

	r3dCSHolder(CRITICAL_SECTION& in_cs) : cs(in_cs)
	{
#if R3D_ENABLE_STALL_DETECTION
		EnterWithStallDetection(cs);
#else
		EnterCriticalSection(&cs);
#endif
	}

	~r3dCSHolder()
	{
		LeaveCriticalSection(&cs);
	}
};

struct r3dCSHolderWithDeviceQueue
{
	CRITICAL_SECTION& cs;

	r3dCSHolderWithDeviceQueue(CRITICAL_SECTION& in_cs) ;
	~r3dCSHolderWithDeviceQueue() ;
};


#include "TSG_STL\TArray.h"
#include "TSG_STL\TLinkedArray.h"
#include "TSG_STL\TStaticArray.h"
#include "TSG_STL\TFixedArray.h"
#include "TSG_STL\TTabArray.h"
#include "TSG_STL\T2DArray.h"
#include "TSG_STL\TFixedAllocator.h"

#include "..\..\External\pugiXML\src\pugixml.hpp"

enum r3dBudgetCategories
{
	TexMem,
	PlayerTexMem,
	UITexMem,
	BufferMem,
	PlayerBufferMem,
	TerrainBufferMem,
	GrassBufferMem,
	OtherBufferMem,
	UIBufferMem,
	RenderTargetMem,
	PlayerTotalMem,
	VMemTotal,
	NumMaterialChanges,
	NumModesChanges,
	NumTextureChanges,
	NumTrianglesRendered,
	AverageStripLength,
	NumDraws,
	NumTerrainDraws,
	NumTerrainTris,
	NumOcclusionQueries,
	NumParticlesRendered,
	NumLocks,
	NumNooverwriteLock,
	NumDiscardLocks,
	NumBytesLocked,
	NumVisGridOcclusions,

	FrameTime,

	BUDGET_CATEGORY_COUNT
};


extern	const char*	r3dError(const char* , ...);
extern void r3dPushExtralErrorInfo( const char* info );
extern void r3dPopExtraErrorInfo();

extern bool r3dAddReport(const char* Str, int FoundPlayer);
extern bool r3dOutToLog(const char* Str, ...);
extern bool r3dArtBug(const char* Str, ...);
extern void r3dArtBugComment( const char* Str );
extern void r3dPurgeArtBugs();
extern void r3dShowArtBugs();
extern void r3dUpdateArtBugs();

#ifndef FINAL_BUILD
#define D3D_V(expr) { HRESULT d3dv_hr = expr; if((d3dv_hr) != D3D_OK ) { r3dRenderer->CheckOutOfMemory( d3dv_hr ) ; r3dError( "D3D call failed (%d) in file '%s' line '%d'. D3D Call:" #expr, d3dv_hr, __FILE__, __LINE__); } }
#else
#define D3D_V(expr) { HRESULT d3dv_hr = expr; if((d3dv_hr) != D3D_OK ) { r3dRenderer->CheckOutOfMemory( d3dv_hr ) ; r3dOutToLog( "D3D call failed (%d) in file '%s' line '%d'. D3D Call:" #expr"\n", d3dv_hr, __FILE__, __LINE__); } }
#endif

#include "r3dHash.h"
#include "r3dConst.h"
#include "r3dProfiler.h"
#include "r3dFileMan.h"
#include "r3dFileTracker.h"
#include "r3dUtils.h"
#include "r3dMath.h"
#include "r3dPoint.h"
#include "r3dPlane.h"
#include "r3dBoxEx.h"
#include "r3dPoly.h"
#include "r3dRay.h"
#include "r3dSphere.h"
#include "r3dCone.h"
#include "r3dCylinder.h"
#include "r3dColor.h"

// some stuff there uses r3dPoint.h ...
#include "TSG_STL\BigEndian.h"

#define R3D_ZERO_VECTOR r3dVector(0, 0, 0)

// A structure for our custom vertex type
struct R3D_WORLD_VERTEX
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	static D3DVERTEXELEMENT9 VBDecl[];
	r3dPoint3D 	Pos;
    r3dPoint3D	Normal;
    float 		tu, tv;   // The texture coordinates
    r3dVector	vU;
    r3dVector	vV;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

#pragma pack(push)
#pragma pack(1)

struct R3D_MESH_VERTEX
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	const static D3DVERTEXELEMENT9	VBDecl[];
	const static unsigned			VBDeclElemCount;

	short4 		pos;
	short2		texcoord;

	ubyte4		normal;
	ubyte4		tangent;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

// the wind will bend this vertex
struct R3D_BENDING_MESH_VERTEX
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl ;

public:
	static const D3DVERTEXELEMENT9	VBDecl[] ;
	static const unsigned			VBDeclElemCount ;

	short4 		pos ;
	short2		texcoord ;

	ubyte4		normal ;
	ubyte4		tangent ;

	// x for low frequency bending, y for medium and z for high
	ubyte4		bendCoefs ;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl() ;
};

struct R3D_MESH_PRECISE_VERTEX
{
	float3 		pos;
	
	float2		texcoord;

	ubyte4		normal;
	ubyte4		tangent;

private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	static D3DVERTEXELEMENT9 VBDecl[];

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

#pragma pack(pop)

struct R3D_DEBUG_VERTEX // for rendering debug stuff on screen
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	static D3DVERTEXELEMENT9 VBDecl[];
	r3dPoint3D 	Pos;
	r3dColor24	color;
	float 		tu, tv;   // The texture coordinates
	r3dPoint3D 	Normal;

	static void ComputeNormals(R3D_DEBUG_VERTEX* v, int count);

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

// A structure for our custom vertex type
struct R3D_SIMPLEWORLD_VERTEX
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
    r3dPoint3D 	Pos;
    r3dPoint3D	Normal;
    r3dColor24 	color;
    float 	tu, tv;   // The texture coordinates
		
	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct R3D_SCREEN_VERTEX
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
    float		x, y, z, rwh;
    DWORD       color;
    DWORD       specular;
    FLOAT       tu, tv;
    FLOAT       tu2, tv2;
    FLOAT       tu3, tv3;
    FLOAT       tu4, tv4;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct R3D_MINIMALIST_VERTEX_2D
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	short2 pos ;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct R3D_POS_VERTEX
{
private:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;

public:
	r3dPoint3D pos;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

#define RENDERABLE_USER_SORT_VALUE (1ull << 56)

struct Renderable
{
	INT64	SortValue;
	void (*DrawFunc)( Renderable* This, const r3dCamera& Cam );

};

#define OBJECTMANAGER_MAXOBJECTS 16383 

enum
{
	MAX_RENDERABLE_SIZE = 64
};

enum
{
	NumShadowSlices = 3,
	NumTransparentShadowSlices = 1
};

enum eRenderStageID
{
	rsCreateSM,
	rsCreateTransparentSM = rsCreateSM + NumShadowSlices,
	rsFillGBufferFirstPerson,
	rsFillGBuffer,
	rsFillGBufferEffects,
	rsFillGBufferAfterEffects,
	rsDrawComposite1,
	rsDrawComposite2,
	rsDrawPhysicsMeshes, // in editor only
	rsDrawDebugData,
	rsDrawBloomGlow,
	rsDrawDistortion,
	rsDrawTransparents,
	rsDrawBoundBox,
	rsDrawDepthEffect,
	rsDrawHighlightPass0,
	rsDrawHighlightPass1,
	rsDrawFlashUI,
	rsDrawDepth,
	rsDepthPrepass,
	rsCount
};

typedef r3dTL::TTabArray< Renderable, MAX_RENDERABLE_SIZE > RenderArray;

#include "../SF/Console/Config.h"

struct r3dTriBatch
{
	r3dString MatName;
	class r3dMaterial*	Mat;
	int		StartIndex;
	int		EndIndex;
};

#include "pShader.h"
#include "vShader.h"
#include "r3dTex.h"
#include "r3dMat.h"
#include "r3dCam.h"
#include "r3dObj.h"
#include "r3dRender.h"
#include "r3dD3DCache.h"
#include "r3dInput.h"
#include "r3dLight.h"
#include "d3dFont.h"
#include "r3dBuffer.h"
#include "r3dCubeMap.h"
#include "Pipeline.h"

#include "r3dAnimation.h"
#include "r3dSkeleton.h"
#include "r3dSkin.h"

#include "CVar.h"

extern 	HANDLE		MainProcess;

extern 	r3dRenderLayer  *r3dRenderer;
extern	r3dMouse	*Mouse;
extern	r3dKeyboard	*Keyboard;
extern	r3dGamepad	*Gamepad;
extern r3dInputMappingMngr* InputMappingMngr;

// game options
enum GAME_OPTIONS_QUALITY
{
	GOQ_OFF = 0,
	GOQ_LOW,
	GOQ_MEDIUM,
	GOQ_HIGH,
	GOQ_ULTRA,
	GOQ_CUSTOM
};

// time manipulation
extern	float		r3dGetTime();
extern	void		r3dStartFrame();
extern	void		r3dEndFrame();
extern	void		r3dResetFrameTime();
extern	float		r3dGetFrameTime();
extern	float		r3dGetFrameTime2(); // will limit delta to 1/10 of a second
extern  float		r3dGetAveragedFrameTime(); // for animation, otherwise it will jerking

extern float		r3dGetAvgFPS();

extern void r3dPrintCallStack();

#define R3D_PROFILE_CRITICAL_SECTION 0

#if R3D_PROFILE_CRITICAL_SECTION
#pragma warning( disable: 4551 )
#define EnterCriticalSection(arg) _errno; { float time = r3dGetTime() ; EnterCriticalSection(arg) ; float span = r3dGetTime() - time ; if( span > 0.033 ) { r3dOutToLog("Critical Section too long: %.3f (%s, line %d)\n", span, __FILE__, __LINE__ ) ; r3dPrintCallStack() ; } }
#endif


extern	int		_r3d_iLogIndent;
class r3dLogIndent
{
  public:
  r3dLogIndent()  {  _r3d_iLogIndent++;                       }
  ~r3dLogIndent() {  if(_r3d_iLogIndent) _r3d_iLogIndent--;   }
};
#define CONCATENATE_DIRECT(s1, s2)	s1##s2
#define CONCATENATE(s1, s2)		CONCATENATE_DIRECT(s1, s2)
#define ANONYMOUS_VARIABLE(str)		CONCATENATE(str, __LINE__)
#define	CLOG_INDENT			r3dLogIndent ANONYMOUS_VARIABLE(unique)

extern	void 		thread_Terminate(DWORD in);
extern 	void 		r3dInitLogFile();
extern 	void 		r3dInitRepFile(int FoundPlayer);

enum r3dFilter
{
	R3D_POINT,
	R3D_BILINEAR,
	R3D_ANISOTROPIC
};

extern	void		r3dSetFiltering( r3dFilter filter, int Stage = -1 );

// this is for special cases, for usual texturing call r3dSetFiltering( R3D_ANISOTROPIC, ..
extern	void		r3dSetAnisotropy( int value, int stage ) ;
extern	void		r3dSetDefaultMaxAnisotropy();
extern	int			r3dToggleScreenShot();
extern	int			r3dUpdateScreenShot();
extern	DWORD 		r3dVectortoRGBA(r3dVector *v, FLOAT fHeight );

extern 	void		r3dCreateWorldTexGenMatrix(D3DXMATRIX *mR, float ScaleX, float ScaleZ);

extern 	void		r3dDrawNormQuad(r3dCamera &Cam, const r3dPoint3D& Pos, const r3dPoint3D& Norm, float Size, const r3dColor24& Col);

extern	void		r3dSetWireframe(int bOn);

extern	int			r3dIsDXT( D3DFORMAT fmt );

extern	int			r3dGetPitchHeight( int count, D3DFORMAT fmt );
extern	int			r3dGetPixelSize( int count, D3DFORMAT fmt );
extern	int			r3dGetTextureBlockLineSize( int width, D3DFORMAT fmt );
extern	int			r3dGetTextureBlockHeight( int height, D3DFORMAT fmt );

extern wchar_t* utf8ToWide(const char* str); // returns pointer to a temp buffer!!!!!!!!!!! do not save that pointer anywhere!
extern char* wideToUtf8(const wchar_t* str); // returns pointer to a temp buffer!!!!!!!!!!! do not save that pointer anywhere!


// note: some crap
extern	float		_srv_World_Scale;
__forceinline float	srv_GetWorldScale() {
  return _srv_World_Scale;
}
__forceinline void	srv_SetWorldScale(float scale) {
  _srv_World_Scale = scale;
}

#define SRV_WORLD_SCALE(x) ((x) * _srv_World_Scale)

	//
	// Game Main helper stuff
	//
extern_nspace(game, void	PreInit());
extern_nspace(game, void	Init());
extern_nspace(game, void	MainLoop());
extern_nspace(game, void	Shutdown());

	// global vars, before & after init
extern_nspace(win,  HINSTANCE	hInstance);
extern_nspace(win,  HWND	hWnd);
extern_nspace(win,  const char	*szWinName);
extern_nspace(win,  HICON	hWinIcon);

	// keybord input
extern_nspace(win,  int		input_Flush());
extern_nspace(win,  int		kbhit());
extern_nspace(win,  int		getch( bool bPop = true ));

	// TRUE if in suspended state, do not process game!
extern_nspace(win,  int		ProcessSuspended());
extern_nspace(win,  int		bSuspended);
extern_nspace(win,  int		bNeedRender);

extern	char		__r3dCmdLine[1024];

extern  void		r3dCloseCFG_Cur (); // for reloading
extern 	const char* 	r3dReadCFG_S(const char* FileName, const char* group, const char* name, const char* defaultvalue);
extern 	int 		r3dReadCFG_I(const char* FileName, const char* group, const char* name, int defval);
extern 	float 		r3dReadCFG_F(const char* FileName, const char* group, const char* name, float defval);
extern 	r3dVector 	r3dReadCFG_V(const char* FileName, const char* group, const char* name, const r3dPoint3D& DefVal);

extern bool			r3dCFGHas(const char* FileName, const char* group, const char* name);

extern 	void 		r3dWriteCFG_V(const char* FileName, const char* group, const char* name, const r3dPoint3D& DefVal);
extern 	void 		r3dWriteCFG_I(const char* FileName, const char* group, const char* name, int val);
extern 	void 		r3dWriteCFG_F(const char* FileName, const char* group, const char* name, float val);
extern 	void 		r3dWriteCFG_S(const char* FileName, const char* group, const char* name, const char* Str);

extern	HANDLE		r3d_CurrentProcess;
extern	HANDLE		r3d_MainThread;

extern	float		r3dRandomFloat();
extern	r3dVector	r3dMakeAOVector();
extern	r3dPoint3D	r3dGetConfusionPosition(const r3dPoint3D& P, float Confusion);
extern	r3dPoint3D	r3dGetRingConfusionPosition(const r3dPoint3D& P, float Confusion);


// Viewport / Screen conversions
	// screen coordinate to world vector from camera
extern	void		r3dScreenTo3D(float scr_x, float scr_y, r3dVector *out_v);
extern  void		r3dScreenPosTo3DPos( float scr_x, float scr_y, r3dVector *out_p );
	// world coordinates to screen one
extern	int		r3dProjectToScreen(const r3dPoint3D& pos, r3dPoint3D* scrCoord);
extern	int		r3dProjectToScreenAlways(const r3dPoint3D& pos, r3dPoint3D* scrCoord, int offsetXpixels, int offsetYpixels); // offsets from center of icon, otherwise icon will be half way outside of screen area

// 2D-3D Rendering functions
struct r3dDrawLineDesc
{
	r3dPoint3D p1, p2;
	r3dColor24 c;
	float width;
};

struct r3dDrawTriDesc
{
	r3dPoint3D p1, p2, p3;
	r3dColor24 c;
};

extern	void		r3dStartLineStrip2D( int count ) ;
extern	void		r3dLineStrip2D( int x, int y ) ;
extern	void		r3dEndLineStrip2D() ;
extern	void		r3dDrawLine2D(float x1, float y1, float x2, float y2, float w, const r3dColor& clr);
extern	void 		r3dDrawLine3D(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex=NULL);
extern  void		r3dDrawLineList3D(const r3dDrawLineDesc *lines, int numLines, const r3dCamera &Cam, r3dTexture *Tex = 0);
extern	void 		r3dDrawLine3DColors(const r3dPoint3D& P1, const r3dPoint3D& P2, r3dCamera &Cam, float Width, const r3dColor24& clr, const r3dColor24& clr2, r3dTexture *Tex=NULL);
extern	void 		r3dDrawLine3D1(const r3dPoint3D& P1, const r3dPoint3D& P2, r3dCamera &Cam, float Width1, float Width2, const r3dColor24& clr, r3dTexture *Tex=NULL);
extern	void 		r3dDrawUniformLine3D(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dCamera &Cam, const r3dColor24& clr);
extern	void 		r3dFillUniformLine3D(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dCamera &Cam, const r3dColor24& clr);
extern  void		r3dDrawConeSolid ( const r3dCone & tCone, r3dCamera &Cam, r3dColor tColor, float fLen = 1.0f, int iSamples = 12 );
extern	void		r3dDrawCone2D(const r3dPoint3D& pos, float radius, float angle, const r3dPoint3D& dir, const r3dColor& clr1, const r3dColor& clr2);
extern  void		r3dDrawCylinder( const r3dCylinder & cylinder, const r3dCamera &Cam, r3dColor tColor, DWORD dwNumSegments = 16 );
extern  void		r3dDrawCylinderSolid( const r3dCylinder & cylinder, const r3dCamera &Cam, r3dColor tColor, DWORD dwNumSegments = 16, bool bIncludeOutline = false, r3dColor24 const & clr2 = r3dColor24::white );
extern	void		r3dDrawCircle2D(const r3dPoint2D& pos, float radius, const r3dColor& clr1, const r3dColor& clr2);
extern  void		r3dDrawPlane(const r3dPoint3D& normal, const r3dPoint3D& point, const r3dColor& clr1, float sizeX = 1000.0f, float sizeY = 100.0f);
extern	void 		r3dDrawCircle3D(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex=NULL, int Segments = 16, float* Heights = NULL );
extern  void		r3dDrawUniformCircle3D(const r3dPoint3D& P1, const float Radius, r3dCamera &Cam, const r3dColor24& clr);
extern  void		r3dDrawUniformSphere(r3dVector const & vPos, float fRadius, r3dCamera &Cam, r3dColor24 const & clr, uint32_t dwNumSphereRings = 6, uint32_t dwNumSphereSegments = 12, r3dVector vScale = r3dVector(1,1,1) );

extern	void		r3dDrawUniformBoundBox( const r3dBoundBox &Box, const r3dCamera &Cam, const r3dColor24& clr );
extern  void		r3dDrawUniformBoundBox( const r3dBoundBox &Box, const D3DXMATRIX& transform, const r3dCamera &Cam, const r3dColor24& clr ); 
extern	void		r3dDrawSphereSolid(r3dVector const & vPos, float fRadius, const r3dCamera	&Cam, r3dColor24 const & clr, uint32_t dwNumSphereRings	= 8, uint32_t dwNumSphereSegments =	16, bool bIncludeOutline = false, r3dColor24 const & clr2 = r3dColor24::white );
extern	void		r3dDrawBoundBox( const r3dBoundBox	&Box, const r3dCamera &Cam, const	r3dColor24&	clr, float fLineWidth =	0.1f, bool flush = true );
extern  void		r3dDrawOrientedBoundBox( const r3dBoundBox &Box, const r3dPoint3D& rotation, const r3dCamera &Cam, const r3dColor24& clr, float fLineWidth = 0.1f ); 
extern	void		r3dDrawOrientedBoundBoxSolid( const r3dBoundBox &Box, const r3dPoint3D& rotation, const r3dCamera &Cam, const r3dColor24& clr, float fLineWidth = 0.1f, bool bIncludeOutline = false, r3dColor24 const & clr2 = r3dColor24::white, CD3DFont* font = NULL );
extern	void		r3dDrawOrientedPlaneSolid( const r3dBoundBox &Box, const r3dPoint3D& rotation, const r3dCamera &Cam, const r3dColor24& clr, float fLineWidth = 0.1f, bool bIncludeOutline = false, r3dColor24 const & clr2 = r3dColor24::white, CD3DFont* font = NULL );
extern	void		r3dDrawGridPlane( const	r3dPoint3D&	centrePos, const r3dCamera &Cam, float width, float	depth, int cellsX, int cellsZ, const r3dColor24& colr, float lineWidth );
extern	void		r3dDrawUniformGridPlane( const r3dPoint3D& centrePos, const	r3dCamera &Cam,	float width, float depth, int cellsX, int cellsZ, const	r3dColor24&	colr );
extern	void		r3dDrawTriangle3D(const	r3dPoint3D&	P1,	const r3dPoint3D& P2, const	r3dPoint3D&	P3,	const r3dCamera &Cam,	const r3dColor24& clr, r3dTexture *Tex=NULL, float *TC=NULL, bool bDontDot = false );
extern	void		r3dDrawTriangleList3D(const r3dDrawTriDesc *tris, int numTris, const r3dCamera &Cam, r3dTexture *Tex=NULL, bool bDontDot = false );

extern	void		r3dDrawGeoSpheresStart();
extern	void		r3dDrawGeoSphere(); // all transforms should go as world matrix
extern	void		r3dDrawGeoSpheresEnd();

extern	void		r3dDrawChamferBoxStart();
extern  void		r3dDrawChamferBox();
extern  void		r3dDrawChamferBoxEnd();

extern	void		r3dDrawSolidBoxStart();
extern  void		r3dDrawSolidBox();
extern  void		r3dDrawSolidBoxEnd();

extern	void		r3dDrawConesStart();
extern	void		r3dDrawCone(); // all transforms should go as world matrix
extern	void		r3dDrawConesEnd();

extern  void		r3dSetIdentityTransform( int constant );
extern  void		r3dSetFwdColorShaders( r3dColor colr );
extern  void		r3dSetFwdColorPixelShader( r3dColor colr );
extern  void		r3dSetColorAndAlphaPixelShader( r3dColor colr );
extern	void		r3dPushSetPosUtilVertexDecl();
extern	void		r3dPushSetPosNormUtilVertexDecl();
extern	void		r3dPopUtilVertexDecl();
extern	void		r3dDrawConeLines( r3dColor colr ); // doesn't require r3dDrawConesStart/End

extern	void 		r3dDrawBox3D         (float x, float y, float z, float w, float h, float d, const r3dColor24& clr );
extern	void 		r3dDrawBox2D         (float x, float y, float w, float h, const r3dColor24& clr, r3dTexture *Tex=NULL, float *TC=NULL);
extern	void 		r3dDrawBox2D         ( r3dPoint2D p0, r3dPoint2D p1, r3dPoint2D p2, r3dPoint2D p3, const r3dColor24& clr, r3dTexture *Tex=NULL, float *TC=NULL);
extern  void		r3dDrawBox2DWireframe(float x, float y, float w, float h, const r3dColor24& clr);
extern	void 		r3dDrawBoxFS         (float w, float h, const r3dColor24& clr, r3dTexture *Tex=NULL, float fOffset=0.0f);
extern	void 		r3dDrawBox2DNoTex    (float x, float y, float w, float h, const r3dColor24& clr, float *TC=NULL);
extern	void 		r3dDrawBox2DZ        (float x, float y, float w, float h, float z, const r3dColor24& clr, r3dTexture *Tex=NULL, float *TC=NULL);
extern	void 		r3dDrawBox2D1        (float x, float y, float w, float h, const r3dColor24& clr, r3dTexture *Tex=NULL, float *TC=NULL);
extern	void 		r3dDrawBox2DRotate   (float center_x, float center_y, float w, float h, float fRot, const r3dColor24& clr, r3dTexture *Tex = NULL, float *TC = NULL);
extern	void 		r3dDrawBox2DGradient (float x, float y, float w, float h, const r3dColor24& C1, const r3dColor24& C2, const r3dColor24& C3, const r3dColor24& C4, r3dTexture *Tex=NULL, float *TC=NULL);
extern	void 		r3dDrawBox2DGradientV(float x, float y, float w, float h, const r3dColor24& TopClr, const r3dColor24& BottomClr, r3dTexture *Tex=NULL, float *TC=NULL);
extern	void 		r3dDrawBox2DGradientH(float x, float y, float w, float h, const r3dColor24& TopClr, const r3dColor24& BottomClr, r3dTexture *Tex=NULL, float *TC=NULL);
extern	void 		r3dDrawBox2DShadow   (float x, float y, float w, float h, float sWidth, float bWidth, const r3dColor24& clr);

extern	void		r3dDrawSetFullScreenQuadVB();
extern	void		r3dDrawFullScreenQuad( bool halfRes );

extern void			r3dGenerateOrientedBoundBoxPoints( r3dPoint3D* points, const int pointsSize, const r3dBoundBox & Box,  const r3dPoint3D &rotation );
extern void			r3dGenerateOrientedPlanePoints( r3dPoint3D* points, const int pointsSize, const r3dBoundBox & Box,  const r3dPoint3D &rotation );

extern IDirect3DVertexDeclaration9* g_ScreenSpaceQuadVertexDecl ;

struct r3dSetRestoreFSQuadVDecl
{
	r3dSetRestoreFSQuadVDecl()
	{
		prevVDECL = d3dc.pDecl;
		d3dc._SetDecl( g_ScreenSpaceQuadVertexDecl );
	}

	~r3dSetRestoreFSQuadVDecl()
	{
		if( prevVDECL )
		{
			d3dc._SetDecl( prevVDECL );
		}
	}

	IDirect3DVertexDeclaration9* prevVDECL;
};


#define	UTIL_TEXT_ALIGN_LEFT	0
#define	UTIL_TEXT_ALIGN_CENTER	1
#define	UTIL_TEXT_ALIGN_RIGHT	2

extern	void 		r3dDrawDrawTextInBox(float x, float y, float w, float h, CD3DFont* pFont, CHAR* pText, r3dColor24 clr, int Align=UTIL_TEXT_ALIGN_CENTER, int Margin=10);

extern	void		r3dDrawTriangleList(R3D_SCREEN_VERTEX *vertices, int num);

// 2D Filters
extern	void 		r3dDrawBlurQuad	(float x, float y, float w, float h, float Tap[8], float Power);
extern	void 		r3dBlurBuffer	(r3dScreenBuffer *SourceTex, 	r3dScreenBuffer *TempTex, 	int BlurPower);
extern	void 		r3dBlurBufferG	(r3dScreenBuffer *SourceTex1, 	r3dScreenBuffer *SourceTex, 	r3dScreenBuffer *TempTex,  int BlurPower);
extern	void 		r3dBlur2Buffer	(r3dScreenBuffer *SourceTex, 	r3dScreenBuffer *TargetTex, 	r3dScreenBuffer *TempTex,  int BlurPower);
extern	void 		r3dBlurDOFBuffer(r3dScreenBuffer *DepthBuffer, 	r3dScreenBuffer *TempTex1, 	r3dScreenBuffer *TempTex2, int BlurPower);


// 3D Math functions
extern	void		GetRotationMatrixFromVectorFast(const r3dPoint3D& vOrient, D3DXMATRIX *mOutRotationMatrix);
extern	void		GetRotationMatrixFromVector(const r3dPoint3D& vOrient, D3DXMATRIX *mOutRotationMatrix, float fBanking = 0.0f);
extern	r3dPoint3D	r3dEulerFromVector(const r3dPoint3D& vVision);
	// get 2D angle between vectors
extern	float		r3dGetAngle2D(const r3dPoint3D& v1, const r3dPoint3D& v2);

void r3dUtilInit();
void r3dUtilClose();

extern	DWORD		r3dCRC32(const BYTE* data, DWORD size);

__forceinline bool r3d_float_isFinite(float f)
{
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF /*| _FPCLASS_ND | _FPCLASS_PD*/) & _fpclass(f) )); // ptumik: removed ND\PD, as those are numbers close to zero and should be fine
}

__forceinline bool r3d_vector_isFinite(r3dPoint3D v)
{
	return r3d_float_isFinite(v.x) && r3d_float_isFinite(v.y) && r3d_float_isFinite(v.z);
}

enum r3dDevStrength
{
	S_WEAK,
	S_MEDIUM,
	S_STRONG,
	S_ULTRA
};

r3dDevStrength r3dGetDeviceStrength();

#define R3D_BASE_SHADER_CACHE_PATH "Data\\Shaders\\Cache"

#define R3D_VSHADERPROFILE_DEFAULT  "vs_3_0"
#define R3D_PSHADERPROFILE_DEFAULT  "ps_3_0"

#define R3D_VSHADERPROFILE_20  "vs_2_0"
#define R3D_PSHADERPROFILE_20  "ps_2_0"

#ifndef FINAL_BUILD
#define R3D_SET_DEBUG_D3D_NAMES 1
#else
#define R3D_SET_DEBUG_D3D_NAMES 0
#endif 

#define R3D_PROFILE_LOADING 0

#endif	//__R3D__H
