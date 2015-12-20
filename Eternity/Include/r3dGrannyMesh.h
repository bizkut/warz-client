#pragma once

#define R3D_PIVOT_CENTER	1
#define R3D_PIVOT_BOTTOM	2

#include "MeshGlobalBuffer.h"

#include "r3dObj.h"

class r3dGrannyMesh ;

typedef D3DXMATRIX PrecalculatedMatrices[6];

struct MeshGrannyDeferredRenderable : Renderable
{
	static void Draw( Renderable* RThis, const r3dCamera& Cam );

	int						BatchIdx;
	DWORD					Color;
	r3dGrannyMesh*			Mesh;
};

struct MeshGrannyShadowRenderable : Renderable
{
	static void DrawSingleBatch( Renderable* RThis, const r3dCamera& Cam );
	static void Draw( Renderable* RThis, const r3dCamera& Cam );

	void (*SubDrawFunc)( Renderable* This, const r3dCamera& Cam );

	r3dGrannyMesh*			Mesh;
	int						BatchIdx;
};

//---------------------------------------------------------


class r3dGrannyMesh : public r3dMeshBase
{
public:

	int			Flags;
	int			UsePreciseVertices;
	int			PositionVDataOffset;
	int			VDataStride;

	MeshGlobalBuffer::Entry buffers;

	int		HasAlphaTextures;

	struct granny_model*	pGraModel ;
	struct granny_mesh*		pGraMesh ;

	r3dGrannyFile*			pGraFile ;

public:
	r3dGrannyMesh( int UsePreciseVertices );
	~r3dGrannyMesh();

	virtual r3dPoint3D* GetVertexPositions() const OVERRIDE { return 0 ; } ;
	virtual uint32_t*	GetIndices() const OVERRIDE { return 0 ; };
	virtual const r3dPoint3D &	GetVertexPos ( int nIndex ) const OVERRIDE ;

	void		InitVertsList(int NumVerts);
	void		InitIndexList(int numIndexes);

	// flags
	BOOL		IsFlagSet(int flag)				{ return BOOL(Flags & flag);	}
	void		SetFlag(int flag, BOOL bSet)	{ bSet ? (Flags |= flag) : (Flags &= ~flag);	}
	BOOL		IsValid()						{ return Flags & obfValid;	}
	void		SetValid(BOOL valid)			{ SetFlag(obfValid, valid);	}

	void		Unload();

	// I/O functions
	bool		Load( const char* fname, int MeshIdx = 0 );
	void		FindAlphaTextures();

	void		Instantiate( struct granny_model_instance** ppInstance ) ;

	R3D_FORCEINLINE int IsSkeletal() const ;
private:

	void		ResetXForm();
public:

	void		FillBuffers();

	// fill buffers code for 2 types of vertices
	void		FillBuffersUnique( struct granny_data_type_definition* dataTypeDef, int VertexSize, int normalsTangentsOffset );

	void		LoadWeights(const char* fname);
	void		LoadWeights_BinaryV1(r3dFile *f);

	//Drawing
	void		GetWorldMatrix( const r3dVector& vPos, const r3dVector& vScl, const r3dVector& vRot, D3DXMATRIX &mRes );
	void		SetWorldMatrix(const r3dVector& pos, const r3dVector& scale, const D3DXMATRIX& rotation);
	void		SetWorldMatrix(const D3DXMATRIX& world);

	void		AppendShadowRenderables( RenderArray& oArr );
	void		AppendRenderablesDeferred( RenderArray& oArr, const r3dColor& color );
	void		AppendTransparentRenderables( RenderArray& oArr, const r3dColor& color, float dist );
	void		DrawMeshStart( const r3dColor* objectColor );
	void		DrawMeshEnd();
	void		DrawMeshDeferredBatch( int batchIdx, UINT MatFlags );
	void		DrawMeshDeferred( const r3dColor& objectColor, UINT MatFlags );		// Fill G-Buffer
	void		DrawMeshWithoutMaterials();
	void		DrawMeshSimple( int bTextured );
	void		DrawMeshShadowsBatch( int batchIdx );
	void		DrawMeshShadows();

	void		DrawMeshStartInstances ();
	void		DrawMeshEndInstances ();
	void		DrawMeshSimpleInstances ( int instanceCount );

	void		RecalcBoundBox();
	void		RecalcBasisVectors();
	void		RecalcTangentWs( r3dPoint3D* VertexBinormals );

	// collisions
	virtual	BOOL		ContainsRay(const r3dPoint3D& vStart, const r3dPoint3D& vNormalizedDirection, float RayLen, float *ClipDist, r3dMaterial **material, const r3dVector& pos, const D3DXMATRIX& rotation, int * OutMinFace = NULL );
	virtual	BOOL		ContainsQuickRay(const r3dPoint3D& vStart, const r3dPoint3D& vNormalizedDirection, float RayLen, float *ClipDist, r3dMaterial **material, const r3dVector& pos, const D3DXMATRIX& rotation);

private:
	static void MeshLoaded ( const char* sMesh );
	static void MeshUnloaded ( const char* sMesh );
	void PackVertexData(void *dstVertices);
	void PackNormalsAndTangents(void *dstVerticesOffsettedToNormals, size_t vertexSize);

public:

	typedef void ( CallbackMeshLoadUnload_t )( const char * szFileName );

	static CallbackMeshLoadUnload_t * g_pMeshLoadCallbacker;
	static CallbackMeshLoadUnload_t * g_pMeshUnloadCallbacker;

};

R3D_FORCEINLINE int r3dGrannyMesh::IsSkeletal() const
{
#if R3D_CALL_GRANNY
	return pGraModel->Skeleton && pGraModel->Skeleton->BoneCount > 1 ? 1 : 0 ;
#else
	return 0 ;
#endif
}

void r3dMeshSetWorldMatrix( const D3DXMATRIX& world, const r3dPoint3D* UnpackScale );
void r3dPrepareShaderMatrices(PrecalculatedMatrices &out, const D3DXMATRIX &world, const r3dPoint3D* ScaleBox);
/**	Set matrices into shader constants. */
void r3dApplyPrecalculatedMatrices(const PrecalculatedMatrices &m);

