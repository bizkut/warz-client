#ifndef	__R3D_MESH_H
#define	__R3D_MESH_H

#define R3D_PIVOT_CENTER	1
#define R3D_PIVOT_BOTTOM	2

#include "MeshGlobalBuffer.h"

#define R3D_ALLOW_ASYNC_MESH_LOADING 1

class r3dVertexBuffer;

#pragma pack(push)
#pragma pack(1)

// mesh, skinned
struct r3dSkinnedMeshVertex
{
	const static D3DVERTEXELEMENT9 VBDecl[];
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	short4	 	pos;
	short2 		texcoord;   // The texture coordinates
	ubyte4		normal;
	ubyte4		tangent;
	short4		weights;	// 4 blend weights
	ubyte4		indices;	// 4 blend indices

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct r3dInstancedDataVertex
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	static D3DVERTEXELEMENT9 VBDecl[];
	float world[12];

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct r3dInstancedUnionMeshVertex
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct r3dInstancedUnionMeshVertex2UV
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	static D3DVERTEXELEMENT9 VBDecl[];
	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

struct PrecalculatedMeshShaderConsts
{
	struct 
	{
		float4 worldViewProj[ 4 ] ;
		float4 scaledWorld[ 3 ] ;
		float4 normalWorld[ 3 ] ;
		float4 scaledWorldView[ 3 ] ;
		float4 normalWorldView[ 3 ] ;
		float4 texcUnpackScale ;
	} VS;

};

#pragma pack(pop)

struct MeshDeferredRenderable : Renderable
{
	static void Draw( Renderable* RThis, const r3dCamera& Cam );

	int						BatchIdx;
	DWORD					Color;
	r3dMesh*				Mesh;
};

struct MeshDeferredRenderableInstancing : Renderable
{
	static void Draw( Renderable* RThis, const r3dCamera& Cam );

	//int						BatchIdx;
	//DWORD					Color;
	r3dMesh*				Mesh;
};

struct MeshDeferredHighlightRenderable : Renderable
{
	static void DoDraw( Renderable* RThis, float distance, const r3dCamera& Cam );
	
	void Init( r3dMesh* mesh, DWORD color );

	r3dMesh*	Mesh;
	int			DoExtrude;
	DWORD		Color;
};


struct MeshShadowRenderable : Renderable
{
	static void DrawSingleBatch( Renderable* RThis, const r3dCamera& Cam );
	static void Draw( Renderable* RThis, const r3dCamera& Cam );

	void (*SubDrawFunc)( Renderable* This, const r3dCamera& Cam );

	r3dMesh*				Mesh;
	int						BatchIdx;
};

class MeshTextureDensityVisualizer;


struct InstancedData
{
	//D3DXMATRIX world;
	float prt[10];
	DWORD color;
};

struct VBData;

struct VBCacheInfo
{
	VBData*						Parent;
	InstancedData*			InstancesVBPtr;
	int								StartIndex;
	int								Count;
};

//---------------------------------------------------------

#define R3D_CHECK_MESH_MATERIAL_CONSISTENCY 0

class r3dMesh
{
private:

	VBCacheInfo*							InstancesVB;
	int												numInstancesInVB;
	volatile LONG							m_Loaded ;
	volatile LONG							m_Drawable ;

public:

#if R3D_CHECK_MESH_MATERIAL_CONSISTENCY
	static r3dTL::TArray< r3dMesh* > AllMeshes;
#endif

	enum EObjectFlags
	{
		obfStatic           = 0x0001,	// Can't be moved/rotated
		obfNoRotate         = 0x0002,	// Can't be rotated, but can be moved
		obfNotClipable      = 0x0008,	// Object is not clipable
		obfDynaLightApplied   = 0x0100,	// dynamic lights applied.
		obfSaveColors         = 0x0200,
		obfMissingSource      = 0x0400,	// no .SCO file when loading mesh ( only .SCB was there )
		obfPlayerMesh         = 0x0800, // add to player buffer when calculating statistics + draw double depth shadows differently ( gaps in body parts are intollerable )
	};

	enum EVertexFlags
	{
		vfPrecise			= 0x0001,
		vfBending			= 0x0002,
		vfUnsharedBuffer	= 0x0004,
		vfHaveSecondUV		= 0x0008
	};


	char		Name[R3D_MAX_OBJECT_NAME];
	r3dString	FileName;	

	r3dBox3D	localBBox;
	r3dPoint3D	CentralPoint;
	r3dPoint3D  vPivot;

	int			RefCount;
	int			SectorRefCount;

	int			NumVertices;
	int			NumIndices;

	r3dPoint3D	unpackScale;
	r3dPoint2D	texcUnpackScale;
	r3dPoint2D	texc2UnpackScale;

	static const int ConstNumMatChunks = 256;
	r3dTriBatch	MatChunks[256];
	char**		MatChunksNames; // used when saving to bin file format only
	int			NumMatChunks;

	int			Flags;

	int			VertexFlags ;

	MeshGlobalBuffer::Entry buffers;
	r3dVertexBuffer*		UnsharedVertexBuffer ;
	r3dIndexBuffer*			UnsharedIndexBuffer ;

	r3dPoint3D*	VertexPositions;
	r3dVector*	VertexNormals;
	r3dPoint2D*	VertexUVs;
	r3dPoint2D*	VertexSecondUVs;
	r3dPoint3D*	VertexTangents;
	char*		VertexTangentWs;
	r3dColor*	VertexColors;

	uint32_t*	Indices;

	int			SizeInVMem ;

	int			HasAlphaTextures;
	float		ExtrudeAmmount;

	int			numInstances;
	int			instanceIdInCurrentBuffer;
public:
	r3dMesh( int UsePreciseVertices );
	virtual ~r3dMesh();

	int			GetNumVertexes	() const				{ return NumVertices; }
	int			GetNumMaterials	() const				{ return NumMatChunks; }
	int			GetNumPolygons	() const;
	int			GetMaterialsSize() const;

	const r3dPoint3D& getCentralPoint() const { return CentralPoint; }
	const r3dPoint3D& getPivot() const { return vPivot; }

	r3dMaterial*	GetFaceMaterial( int faceIdx ) const;

	virtual r3dPoint3D* GetVertexPositions() const ;
	virtual uint32_t*	GetIndices() const ;

	virtual const r3dPoint3D &	GetVertexPos ( int nIndex ) const { return VertexPositions[ nIndex ]; }

	void		InitVertsList(int NumVerts);
	void		InitIndexList(int numIndexes);

	// flags
	void		SetFlag(int flag, BOOL bSet)	{ bSet ? (Flags |= flag) : (Flags &= ~flag);	}
	R3D_FORCEINLINE BOOL IsFlagSet(int flag)		{ return BOOL(Flags & flag);	}
	R3D_FORCEINLINE BOOL IsDrawable() const			{ return m_Drawable; }
	R3D_FORCEINLINE BOOL IsDrawablePure() const		{ return m_Drawable; }
	R3D_FORCEINLINE BOOL IsLoaded() const			{ return m_Loaded; }
	void		SetDrawable();
	void		SetLoaded() ;

	void		Unload();
	void		UnloadMaterials();


	// I/O functions
	bool		Load(const char* fname, bool use_default_material = false, bool force_sync = true, bool use_thumbnails = false );
	void		LoadMaterials( bool use_thumbnails );

	void		TouchMaterials();
	void		ReleaseMaterials();

	void		FindAlphaTextures();

	static void Init() ;
	static void Close() ;
private:

	static void DoLoadMesh( struct r3dTaskParams* params );
	bool		DoLoad( bool use_default_material, bool use_thumbnails ) ;

	int 		LoadAscii(r3dFile *f, bool use_default_material );

	bool 		LoadBin(r3dFile *f, bool use_default_material, bool use_thumbnails );
	bool 		SaveBin(const char* fname);
	bool 		SaveBinPS3(const char* fname);

	void		ResetXForm();

public:
	int			HasMaterialOfType( const char* typeName );

	static bool	CanLoad( const char* fname ) ;

	void		NormalizeTexcoords() ;

	static void	FillMeshBuffers( r3dTaskParams* params ) ;
	void		FillBuffersAsync() ;
	void		FillBuffers();
	void		FillBuffers_SaveVertexData();

	// fill buffers code for 2 types of vertices
	template <typename VERTEX>
	void		FillBuffersUnique();
	template <typename VERTEX> 
	void		FillSingleVertexBase(VERTEX* pV, int index);
	void		FillSingleVertex(void *pV_, int index);

	// weight
	struct r3dWeight
	{
		BYTE	BoneID[4];
		float	Weight[4];
	};
	r3dWeight*	pWeights;
	int		vbCurSize_;

private:
	friend void DoFillBuffersMeshMainThread( void* Ptr ) ;
	void		DoFillBuffersMainThread() ;
	void		DoFillBuffers( bool deleteSystemVertexData );

	void		AllocateWeights();
	void		TryLoadWeights(const char* baseFileName);

	void		LoadWeights(const char* fname);
	void		LoadWeights_BinaryV1(r3dFile* f, bool from_scb);
	bool		SaveWeights_BinaryV1(FILE* f);
	void		DeleteWeights();
public:

	//Drawing
	void		GetWorldMatrix( const r3dVector& vPos, const r3dVector& vScl, const r3dVector& vRot, D3DXMATRIX &mRes );
	void		SetShaderConsts(const r3dVector& pos, const r3dVector& scale, const D3DXMATRIX& rotation);
	void		SetShaderConsts(const D3DXMATRIX& world);
	void		SetShaderConsts_Localized(const D3DXMATRIX& world);

	void		AppendShadowRenderables( RenderArray& oArr );
	void		AppendRenderablesDeferred( RenderArray& oArr, const r3dColor& color, float selfIllumMultiplier );
	bool		AppendRenderablesDeferredInstanced( RenderArray& oArr, const r3dColor& color, const D3DXQUATERNION& quat,  const r3dPoint3D& pos, const r3dPoint3D& scale);
	void		AppendTransparentRenderables( RenderArray& oArr, const r3dColor& color, float dist, int forceAll );
	void		AppendTransparentRenderablesColors( RenderArray& oArr, float dist, int forceAll );
	void		DrawMeshStart( const r3dColor* objectColor );
	void		DrawMeshEnd();
	void		DrawMeshDeferredBatch( int batchIdx, UINT MatFlags );
	void		DrawMeshDeferred( const r3dColor& objectColor, UINT MatFlags );		// Fill G-Buffer

	void ResetInstancingInfo();

	void		DrawMeshWithoutMaterials();
	// [Alexey] Seem it deprecated
	//void		DrawMeshInstanced( const D3DXMATRIX* world, int ShadowMap, bool force_render=false );
	void		DrawMeshInstanced2();
	void		DrawMeshSimple( int bTextured );
	void		DrawMeshShadowsBatch( int batchIdx );
	void		DrawMeshShadows();

	void		DrawMeshStartInstances ();
	void		DrawMeshEndInstances ();
	void		DrawMeshSimpleInstances ( int instanceCount );

	void		SetupTexcUnpackScaleVSConst() const ;

	void		RecalcBoundBox();
	void		RecalcBasisVectors();
	void		RecalcTangentWs( r3dPoint3D* VertexBinormals );	

	void		OptimizeVCache();

	void		DisableThumbnails();
	void		EnableThumbnails();

	template <typename T>
	static void	RemapComponent( T* comp, const r3dgfxVector(int)& compMap ) ;

	// collisions
	virtual	BOOL		ContainsRay(const r3dPoint3D& vStart, const r3dPoint3D& vNormalizedDirection, float RayLen, float *ClipDist, r3dMaterial **material, const r3dVector& pos, const D3DXMATRIX& rotation, int * OutMinFace = NULL );
	virtual	BOOL		ContainsQuickRay(const r3dPoint3D& vStart, const r3dPoint3D& vNormalizedDirection, float RayLen, float *ClipDist, r3dMaterial **material, const r3dVector& pos, const D3DXMATRIX& rotation);

	int			IsSkeletal() const ;

	static void	FlushLoadingBuffers() ;

private:
	static void MeshLoaded ( const char* sMesh );
	static void MeshUnloaded ( const char* sMesh );

#ifndef FINAL_BUILD
	MeshTextureDensityVisualizer *densityVisualizer;
#endif

public:

	typedef void ( CallbackMeshLoadUnload_t )( const char * szFileName );

	static CallbackMeshLoadUnload_t * g_pMeshLoadCallbacker;
	static CallbackMeshLoadUnload_t * g_pMeshUnloadCallbacker;

};

void r3dMeshSetShaderConsts( const D3DXMATRIX& world, const r3dMesh* mesh );
void r3dMeshSetShaderConsts_Localized( const D3DXMATRIX& world, const r3dMesh* mesh );
void r3dPrepareMeshShaderConsts(PrecalculatedMeshShaderConsts &out, const D3DXMATRIX &world, const r3dPoint3D* ScaleBox, const r3dPoint2D* TexcUnpackScale, const D3DXMATRIX& viewMtx, const D3DXMATRIX& viewProjMtx, const r3dPoint2D* Texc2UnpackScale);
/**	Set matrices into shader constants. */
void r3dMeshSetVSTexcUnpackScale( const r3dPoint2D unpackScale ) ;
void r3dApplyPreparedMeshVSConsts(const PrecalculatedMeshShaderConsts &vsc);

void r3dApplyPreparedMeshVSConsts_DepthPrepass(const PrecalculatedMeshShaderConsts &vsc);

R3D_FORCEINLINE int GetRevIDist( float dist )
{
	return 0xffff - R3D_MIN( (int)dist, 0xffff );
}

R3D_FORCEINLINE int r3dMesh::IsSkeletal() const
{
	return pWeights ? 1 : 0 ;
}

R3D_FORCEINLINE void r3dMesh::SetupTexcUnpackScaleVSConst() const
{
	r3dMeshSetVSTexcUnpackScale( this->texcUnpackScale ) ;
}

void FillPosition( R3D_MESH_VERTEX& vertex, const r3dPoint3D& pos, const r3dPoint3D& unpackScale );
void FillPosition( R3D_BENDING_MESH_VERTEX& vertex, const r3dPoint3D& pos, const r3dPoint3D& unpackScale );
void FillTexcoord( R3D_MESH_VERTEX& vertex, const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale );
void FillTexcoord( R3D_BENDING_MESH_VERTEX& vertex, const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale );
r3dPoint2D UnpackTexcoord(short u, short v, const r3dPoint2D &texcUnpackScale);

void ToSkinFileName( char (&skinFile)[256], const char* baseFile );

#endif //__R3D_MESH_H
