#include	"r3dPCH.h"

#include	"r3d.h"

#include "r3dBackgroundTaskDispatcher.h"

#include "r3dMeshTexDensityVisualizer.h"
#include "../SF/RenderBuffer.h"
#include "r3dDeviceQueue.h"
#include "r3dVCacheOptimize.h"
#include "r3dVCacheAnalyze.h"

int r3dMeshObject_SkipFillBuffers = 0;
extern	r3dgfxMap(DWORD, r3dSTLString) _r3d_mVBufferMap;

//r3dVertexBuffer * r3dMesh::pInstancesVB = NULL;

//------------------------------------------------------------------------

struct VBData
{
	r3dVertexBuffer*	InstancesVB;
	typedef std::pair<int, int> int_pair;
	typedef r3dgfxVector(int_pair) VBRanges;
  VBRanges					ranges;
	int								LockCount;
	VBData *					Next;
	InstancedData*		BufferPointer;
};

struct InstancingMeshVBCache
{
	static const int MaxInstancesToDraw = 3200;

	VBData * Top;

	void AllocVBData()
	{
		VBData * vbData = gfx_new VBData;
		vbData->LockCount = 0;
		vbData->Next = Top;
		Top = vbData;
		vbData->InstancesVB = gfx_new r3dVertexBuffer( MaxInstancesToDraw, sizeof ( InstancedData ), 0, true );
		vbData->ranges.push_back(std::make_pair<int, int>(0, MaxInstancesToDraw));
	}

	VBCacheInfo* GetVBRange(int count)
	{
		r3d_assert(count < MaxInstancesToDraw);
		// FIXME [Alexey] VB sharing causes flickering. Need to investigate more
		count = MaxInstancesToDraw;
		VBCacheInfo* info = FindVBRange(count);
		if (!info)
		{
			// alloc
			AllocVBData();
			info = FindVBRange(count);
		}

		r3d_assert(info);
		// Lock vb
		info->Parent->LockCount++;
		if (info->Parent->LockCount == 1)
		{
			info->Parent->BufferPointer = static_cast<InstancedData*>(info->Parent->InstancesVB->Lock(0, MaxInstancesToDraw));
		}

		info->InstancesVBPtr = info->Parent->BufferPointer + info->StartIndex;
		return info;
	}

	VBCacheInfo* FindVBRange(int count)
	{
		VBData * topPtr = Top;
		while(topPtr)
		{

			VBData::VBRanges::iterator it = topPtr->ranges.begin();
			while(it != topPtr->ranges.end())
			{
				if (it->second > count )
				{
					VBCacheInfo* info = gfx_new VBCacheInfo;
					info->Count = count;
					info->Parent = topPtr;
					info->StartIndex = it->first;

					it->first += count;
					it->second -= count;

					return info;
				}
				else
					if (it->second == count )
					{
						VBCacheInfo* info = gfx_new VBCacheInfo;
						info->Count = count;
						info->Parent = topPtr;
						info->StartIndex = it->first;
						topPtr->ranges.erase(it);

						return info;
					}
					++it;
			}

			topPtr = topPtr->Next;
		}

		return 0;
	}

	struct VBRangeComp
	{
		bool operator () (const VBData::int_pair& left,  const VBData::int_pair& right)const
		{
			return left.first < right.first;
		}

		bool operator () (const int& left, const VBData::int_pair& right)const
		{
			return left < right.first;
		}

		bool operator () (const VBData::int_pair& left, const int& right)const
		{
			return left.first < right;
		}
	};

	void PutVBRange(VBCacheInfo* info)
	{
		r3d_assert(!info->Parent->LockCount);

		VBData * data = info->Parent;
		VBData::VBRanges::iterator it = std::lower_bound(data->ranges.begin(), data->ranges.end(), info->StartIndex, VBRangeComp()) ;

		data->ranges.insert(it, std::make_pair(info->StartIndex, info->Count));

		// fill the holes
		it = data->ranges.begin();
		while( it != data->ranges.end())
		{
			VBData::VBRanges::iterator itNext = it + 1;
			if (itNext != data->ranges.end())
			{
				if (itNext->first == it->first + it->second)
				{
					itNext->first = it->first;
					itNext->second += it->second;
					itNext = data->ranges.erase(it);
				}
			}
			it = itNext;
		}

		delete info;
	}

	void UnlockVBRange(VBCacheInfo* info)
	{
		r3d_assert(info->Parent->LockCount);

		VBData * data = info->Parent;
		data->LockCount--;
		if (data->LockCount == 0)
		{
			data->InstancesVB->Unlock();
		}
	}

};

static InstancingMeshVBCache gInstancingMeshVBCache;

INT64 RENDERABLE_EMITTER_USER_SORT_VALUE = 15 * RENDERABLE_USER_SORT_VALUE;


LPDIRECT3DVERTEXDECLARATION9 r3dSkinnedMeshVertex::pDecl = 0;
const D3DVERTEXELEMENT9 r3dSkinnedMeshVertex::VBDecl[] = 
{
	{ 0, 0,  D3DDECLTYPE_SHORT4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION		, 0 },
	{ 0, 8,  D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD		, 0 },
	{ 0, 12, D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL			, 0 },
	{ 0, 16, D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT			, 0 },
	{ 0, 20, D3DDECLTYPE_SHORT4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT		, 0 },
	{ 0, 28, D3DDECLTYPE_UBYTE4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES	, 0 },

	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 r3dSkinnedMeshVertex::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}

LPDIRECT3DVERTEXDECLARATION9 r3dInstancedDataVertex::pDecl = 0;
D3DVERTEXELEMENT9 r3dInstancedDataVertex::VBDecl[] = 
{
	{1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
	{1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
	{1, 28, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5},
	{1, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	//{1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 6},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 r3dInstancedDataVertex::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}

LPDIRECT3DVERTEXDECLARATION9 r3dInstancedUnionMeshVertex::pDecl = 0;
D3DVERTEXELEMENT9 r3dInstancedUnionMeshVertex::VBDecl[] = 
{

	{ 0, 0,		D3DDECLTYPE_SHORT4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
	{ 0, 8,		D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
	{ 0, 12,	D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
	{ 0, 16,	D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0 },

	{1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
	{1, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
	{1, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5},
	{1, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	//{1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 6},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 r3dInstancedUnionMeshVertex::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}

LPDIRECT3DVERTEXDECLARATION9 r3dInstancedUnionMeshVertex2UV::pDecl = 0;
D3DVERTEXELEMENT9 r3dInstancedUnionMeshVertex2UV::VBDecl[] = 
{

	{ 0, 0,		D3DDECLTYPE_SHORT4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
	{ 0, 8,		D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
	{ 0, 12,	D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	1 },
	{ 0, 16,	D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
	{ 0, 20,	D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0 },

	{1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
	{1, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
	{1, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5},
	{1, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	//{1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 6},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 r3dInstancedUnionMeshVertex2UV::getDecl()
{
	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}

struct R3D_2UV_MESH_VERTEX
{
private:
	static LPDIRECT3DVERTEXDECLARATION9	pDecl;
public:
	const static D3DVERTEXELEMENT9	VBDecl[];
	const static unsigned			VBDeclElemCount;

	short4 		pos;
	short2		texcoord;
	short2		tex2coord;

	ubyte4		normal;
	ubyte4		tangent;

	static LPDIRECT3DVERTEXDECLARATION9 getDecl();
};

LPDIRECT3DVERTEXDECLARATION9 R3D_2UV_MESH_VERTEX::pDecl = 0;
const D3DVERTEXELEMENT9 R3D_2UV_MESH_VERTEX::VBDecl[] = 
{
	{ 0, 0,		D3DDECLTYPE_SHORT4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
	{ 0, 8,		D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
	{ 0, 12,	D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	1 },
	{ 0, 16,	D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
	{ 0, 20,	D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0 },
	D3DDECL_END()
};

const /*static*/ unsigned R3D_2UV_MESH_VERTEX::VBDeclElemCount = sizeof VBDecl / sizeof VBDecl[ 0 ];

LPDIRECT3DVERTEXDECLARATION9 R3D_2UV_MESH_VERTEX::getDecl()
{
	R3D_ENSURE_MAIN_THREAD();

	if(pDecl == 0)
	{
		( r3dDeviceTunnel::CreateVertexDeclaration(VBDecl, &pDecl) );
	}
	return pDecl;
}


//------------------------------------------------------------------------

int r3dMesh::GetNumPolygons() const
{
	r3d_assert( m_Loaded ) ;

	int nNum = 0;
	for ( int i = 0; i < NumMatChunks; i++ )
	{
		const r3dTriBatch& batch = MatChunks[ i ];
		nNum += ( batch.EndIndex - batch.StartIndex ) / 3;
	}
	return nNum;
}

//------------------------------------------------------------------------

static int GetTexMemorySize( r3dTexture* pTex )
{
	if ( pTex )
		return pTex->GetTextureSizeInVideoMemory();
	return 0;
}

static int GetTexMemorySize( r3dThumbnailTexture* pTex )
{
	if( !pTex )
		return 0;

	if ( pTex->GetFullSizeTexture()->IsDrawable() )
		return pTex->GetFullSizeTexture()->GetTextureSizeInVideoMemory();
	else
		return pTex->GetThumbnailByteSize();
}


int r3dMesh::GetMaterialsSize() const
{
	r3d_assert( m_Loaded ) ;

	int nSize = 0;
	for ( int i = 0; i < NumMatChunks; i++ )
	{ 
		r3dMaterial* pMat = MatChunks[ i ].Mat;
		if ( pMat )
		{
			nSize += GetTexMemorySize( pMat->Texture );
			nSize += GetTexMemorySize( pMat->NormalTexture );
			nSize += GetTexMemorySize( pMat->GlossTexture );
			nSize += GetTexMemorySize( pMat->EnvPowerTexture );
			nSize += GetTexMemorySize( pMat->SelfIllumTexture );
			nSize += GetTexMemorySize( pMat->NormalDetailTexture );
		}	
	}
	return nSize;
}

//------------------------------------------------------------------------

r3dMaterial* r3dMesh::GetFaceMaterial( int faceIdx ) const
{
	// ptumik: if mesh is not loaded yet, return NULL instead of asserting, as there are way too many cases when we can call GetFaceMaterial without checking if mesh was already loaded
	if(!m_Loaded)
		return NULL;

	for(int m=0; m<NumMatChunks; ++m)
	{
		if( ((faceIdx*3) >= MatChunks[m].StartIndex) && ((faceIdx*3) <= MatChunks[m].EndIndex))
		{
			return MatChunks[m].Mat;
		}
	}

	return NULL;
}


//------------------------------------------------------------------------

//static 
r3dMesh::CallbackMeshLoadUnload_t * r3dMesh::g_pMeshLoadCallbacker = NULL;
r3dMesh::CallbackMeshLoadUnload_t * r3dMesh::g_pMeshUnloadCallbacker = NULL;

//MeshGlobalBuffer globalBuffer;

#if R3D_CHECK_MESH_MATERIAL_CONSISTENCY
r3dTL::TArray< r3dMesh* > r3dMesh::AllMeshes;
#endif

//-----------------------------------------------------------------------
r3dMesh::r3dMesh( int UsePreciseVertices )
//-----------------------------------------------------------------------
{
#if R3D_CHECK_MESH_MATERIAL_CONSISTENCY
	AllMeshes.PushBack( this );
#endif

	SectorRefCount = 0;

	VertexFlags = 0 ;
	SizeInVMem = 0 ;

	RefCount = 0 ;

	if( UsePreciseVertices )
	{
		VertexFlags |= vfPrecise ;
	}

	MatChunksNames		= 0;
	NumMatChunks		= 0;
	NumIndices		= 0;
	NumVertices     = 0;
	vPivot			= r3dPoint3D(0, 0, 0);
	CentralPoint	= r3dPoint3D(0, 0, 0);
	localBBox.Org	= r3dPoint3D(0, 0, 0);
	localBBox.Size	= r3dPoint3D(0, 0, 0);
	unpackScale		= r3dPoint3D( 1.f, 1.f, 1.f );
	texcUnpackScale	= r3dPoint2D( 1.f, 1.f );
	texc2UnpackScale= r3dPoint2D( 1.f, 1.f );

	m_Loaded = 0 ;
	m_Drawable = 0 ;
	numInstancesInVB = 0;
	VertexPositions = NULL;

	Indices			= NULL;
	VertexNormals	= NULL;
	VertexTangents	= NULL;
	VertexTangentWs	= NULL;
	VertexColors    = NULL;
	VertexUVs		= NULL;
	VertexSecondUVs		= NULL;
	pWeights		= NULL;
	vbCurSize_		= 0;

	HasAlphaTextures	= 0;
	ExtrudeAmmount		= 0.f;

	Flags = 0;

	UnsharedVertexBuffer = NULL ;
	UnsharedIndexBuffer = NULL ;

	numInstances = 0;
	instanceIdInCurrentBuffer = -1;

	InstancesVB = NULL;

#ifndef FINAL_BUILD
	densityVisualizer = gfx_new MeshTextureDensityVisualizer(this);
#endif
}

//-----------------------------------------------------------------------
r3dMesh::~r3dMesh()
//-----------------------------------------------------------------------
{
#ifndef FINAL_BUILD
	SAFE_DELETE(densityVisualizer);
#endif
	Unload();

#if R3D_CHECK_MESH_MATERIAL_CONSISTENCY
	for( int i = 0, e = AllMeshes.Count(); i < e; i ++ )
	{
		if( AllMeshes[ i ] == this )
		{
			AllMeshes.Erase( i );
			break;
		}
	}
#endif
}

//-----------------------------------------------------------------------
void r3dMesh::Unload()
{
	if( g_async_loading->GetInt() )
	{
		R3D_ENSURE_MAIN_THREAD() ;

		for( ; ! m_Drawable ; )
		{
			ProcessDeviceQueue( r3dGetTime(), 0.33f ) ;

			Sleep( 1 ) ;
		}
	}

	if(!IsLoaded())
		return;

	if( Flags & obfPlayerMesh )
	{
		r3dRenderer->Stats.AddPlayerBufferMem( -SizeInVMem ) ;
	}

	SizeInVMem = 0 ;

	MeshUnloaded ( FileName.c_str () );

	SAFE_DELETE_ARRAY(VertexPositions);
	SAFE_DELETE_ARRAY(Indices);

	SAFE_DELETE_ARRAY(VertexTangents);
	SAFE_DELETE_ARRAY(VertexTangentWs);
	SAFE_DELETE_ARRAY(VertexColors);
	SAFE_DELETE_ARRAY(VertexUVs);
	SAFE_DELETE_ARRAY(VertexSecondUVs);
	SAFE_DELETE_ARRAY(pWeights);

	r3d_assert( ( VertexFlags & vfUnsharedBuffer ) || ( !UnsharedIndexBuffer && !UnsharedVertexBuffer ) ) ;

	SAFE_DELETE( UnsharedVertexBuffer ) ;
	SAFE_DELETE( UnsharedIndexBuffer ) ;

	if (InstancesVB)
		gInstancingMeshVBCache.PutVBRange(InstancesVB);

	InstancesVB = 0;


	

	SAFE_DELETE_ARRAY(VertexNormals);

// 	if(g_pVB != NULL) 
// 	{
// #ifdef _DEBUG
// 		_r3d_mVBufferMap.erase((DWORD)g_pVB);
// #endif
// 
// 		g_pVB->Release();
// 		g_pVB = NULL;
// 
// 		r3dRenderer->Stats.BufferMem -= NumVertices * sizeof(R3D_MESH_VERTEX);
// 	}
// 
// 	if(g_IB != NULL) {
// 		g_IB->Release();
// 		g_IB = NULL;
// 		r3dRenderer->Stats.BufferMem -= NumIndices * sizeof(DWORD);
// 	}

	if( Flags & obfPlayerMesh )
	{
		UnloadMaterials();
	}

	NumIndices 	 = 0;
	NumVertices   = 0;
	NumMatChunks  = 0;

	Flags = 0;

	m_Drawable = 0 ;
	m_Loaded = 0 ;

	return;
}

void
r3dMesh::UnloadMaterials()
{
	for( int i = 0 ; i < NumMatChunks ; i ++ )
	{
		r3dMaterial* mat = MatChunks[ i ].Mat  ;

		if( !mat )
			continue ;

		// prevent double deletion ( what if we have same mat replicated across several chunks )
		for( int ii = i + 1 ; ii < NumMatChunks ; ii ++ )
		{
			if( MatChunks[ ii ].Mat == mat )
				MatChunks[ ii ].Mat = NULL ;
		}

		r3dMaterialLibrary::UnloadManagedMaterial( mat ) ;

		MatChunks[ i ].Mat = NULL ;
	}
}

void
r3dMesh::SetDrawable()
{
	r3d_assert( m_Loaded ) ;

	InterlockedExchange( &m_Drawable, 1 );
}

void
r3dMesh::SetLoaded()
{
	m_Loaded = 1 ;
}

void r3dMesh::RecalcBoundBox()
{
	r3d_assert( m_Loaded ) ;

	r3dPoint3D *WV = VertexPositions;
	r3dPoint3D *Sz = &VertexPositions[NumVertices];

	float minx = WV->X;
	float miny = WV->Y;
	float minz = WV->Z;
	float maxx = WV->X;
	float maxy = WV->Y;
	float maxz = WV->Z;

	while(++WV < Sz)
	{
		if(minx > WV->X) minx = WV->X;
		if(miny > WV->Y) miny = WV->Y;
		if(minz > WV->Z) minz = WV->Z;
		if(maxx < WV->X) maxx = WV->X;
		if(maxy < WV->Y) maxy = WV->Y;
		if(maxz < WV->Z) maxz = WV->Z;
	}

	localBBox.Org.X  = minx;// + vMove.x;
	localBBox.Org.Y  = miny;// + vMove.y;
	localBBox.Org.Z  = minz;// + vMove.y;
	localBBox.Size.X = R3D_ABS( maxx - minx );
	localBBox.Size.Y = R3D_ABS( maxy - miny );
	localBBox.Size.Z = R3D_ABS( maxz - minz );

	if( VertexFlags & vfPrecise )
	{
		unpackScale.x = 1.f;
		unpackScale.y = 1.f;
		unpackScale.z = 1.f;
	}
	else
	{
		unpackScale.x = R3D_MAX( fabs( localBBox.Org.x ), fabs( localBBox.Org.x + localBBox.Size.x ) ) * 1.001f;
		unpackScale.y = R3D_MAX( fabs( localBBox.Org.y ), fabs( localBBox.Org.y + localBBox.Size.y ) ) * 1.001f;
		unpackScale.z = R3D_MAX( fabs( localBBox.Org.z ), fabs( localBBox.Org.z + localBBox.Size.z ) ) * 1.001f;
	}

	return;
}

void r3dMesh::RecalcBasisVectors()
{
	r3d_assert( m_Loaded ) ;

	struct Binormals
	{
		Binormals( int Count )
		{
			binormals = gfx_new r3dPoint3D[ Count ];
		}

		~Binormals()
		{
			delete [] binormals;
		}

		r3dPoint3D* binormals;
	} localBinormals( NumVertices );

	r3dPoint3D* VertexBinormals = localBinormals.binormals;

	for( int i = 0; i < NumVertices ; i ++ )
	{
		VertexTangents[ i ].Assign( 0, 0, 0 );
		VertexTangentWs[ i ] = 0;
	} 

	for(int i = 0; i < NumIndices/3; i++)
	{
		r3dPoint3D P1,P2,P3;
		r3dVector  V1, V2, CP;
		r3dVector  U, V, UV;

		int idx0 = Indices[i*3+0];
		int idx1 = Indices[i*3+1];
		int idx2 = Indices[i*3+2];

		P1 = VertexPositions[idx0];
		P2 = VertexPositions[idx1];
		P3 = VertexPositions[idx2];

		// Calculate X
		V1.X = P2.X - P1.X;
		V1.Y = VertexUVs[idx1].x - VertexUVs[idx0].x;
		V1.Z = VertexUVs[idx1].y - VertexUVs[idx0].y;

		V2.X = P3.X - P1.X;
		V2.Y = VertexUVs[idx2].x - VertexUVs[idx0].x;
		V2.Z = VertexUVs[idx2].y - VertexUVs[idx0].y;

		CP = V1.Cross(V2);

		r3d_assert(CP.x != 0);


		U.X = -CP.Y / CP.X;
		V.X = -CP.Z / CP.X;

		//Calculate Y
		V1.X = P2.Y - P1.Y;
		V1.Y = VertexUVs[idx1].x - VertexUVs[idx0].x;
		V1.Z = VertexUVs[idx1].y - VertexUVs[idx0].y;

		V2.X = P3.Y - P1.Y;
		V2.Y = VertexUVs[idx2].x - VertexUVs[idx0].x;
		V2.Z = VertexUVs[idx2].y - VertexUVs[idx0].y;

		CP = V1.Cross(V2);

		r3d_assert(CP.x != 0);		U.Y = -CP.Y / CP.X;
		V.Y = -CP.Z / CP.X;


		//Calculate Z
		V1.X = P2.Z - P1.Z;
		V1.Y = VertexUVs[idx1].x - VertexUVs[idx0].x;
		V1.Z = VertexUVs[idx1].y - VertexUVs[idx0].y;

		V2.X = P3.Z - P1.Z;
		V2.Y = VertexUVs[idx2].x - VertexUVs[idx0].x;
		V2.Z = VertexUVs[idx2].y - VertexUVs[idx0].y;

		CP = V1.Cross(V2);

		r3d_assert(CP.x != 0);

		U.Z = -CP.Y / CP.X;
		V.Z = -CP.Z / CP.X;

		U.Normalize();
		V.Normalize();

		VertexTangents[idx0] += U; VertexTangents[idx1] += U; VertexTangents[idx2] += U;
		VertexBinormals[idx0] += V; VertexBinormals[idx1] += V; VertexBinormals[idx2] += V;
	}

	for(int i=0;i<NumVertices;i++)
	{
		VertexTangents[i].Normalize();
		VertexBinormals[i].Normalize();
	}

	RecalcTangentWs( VertexBinormals );
}

void
r3dMesh::RecalcTangentWs( r3dPoint3D* VertexBinormals )
{
	r3d_assert( m_Loaded ) ;

	for( int i = 0, e = NumVertices; i < e; i ++ )
	{
		r3dPoint3D binorm = VertexNormals[i].Cross( VertexTangents[i] );

		if( binorm.Dot( VertexBinormals[ i ] ) >= 0 )
		{
			VertexTangentWs[ i ] = char(255);
		}
		else
		{
			VertexTangentWs[ i ] = 0;
		}
	}
}

namespace
{
	r3dgfxVector(int) tempIndexBuffer0 ;
	r3dgfxVector(int) tempIndexBuffer1 ;
	r3dgfxVector(int) remapIndices ;
	void* tempGenericMem ;
	size_t tempGenericMemSize ;
}

void
r3dMesh::OptimizeVCache()
{
	int	minVerts[ R3D_ARRAYSIZE( MatChunks ) ] ;
	int maxVerts[ R3D_ARRAYSIZE( MatChunks ) ] ;

	for( int i = 0, e = this->NumMatChunks ; i < e; i ++ )
	{
		const r3dTriBatch& tbatch = this->MatChunks[ i ] ;

		int minVert = 0x7fffffff ;
		int maxVert = 0 ;

		// antiwarning
		{
			for( int i = tbatch.StartIndex, e = tbatch.EndIndex ; i < e; i ++ )
			{
				minVert = R3D_MIN( (int)Indices[ i ], minVert ) ;
				maxVert = R3D_MAX( (int)Indices[ i ], maxVert ) ;
			}
		}

		minVerts[ i ] = minVert ;
		maxVerts[ i ] = maxVert ;
	}

	for( int i = 0, e = this->NumMatChunks ; i < e; i ++ )
	{
		const r3dTriBatch& tbatch = this->MatChunks[ i ] ;

		int numBatchIndices = tbatch.EndIndex - tbatch.StartIndex ;

		tempIndexBuffer0.resize( numBatchIndices ) ;
		tempIndexBuffer1.resize( numBatchIndices ) ;

		int minVert = minVerts[ i ] ;
		int maxVert = maxVerts[ i ] ;

		int numBatchVerts = 0 ;

		if( minVert > maxVert )
		{
			minVert = maxVert ;
		}
		else
		{
			numBatchVerts = maxVert - minVert + 1 ;
		}

		// is this vertex range unintersected?
		bool unintersected = true ;

		for( int j = 0, e = this->NumMatChunks ; j < e ; j ++ )
		{
			if( i == j )
				continue ;

			if( minVert >= minVerts[ j ] && minVert <= maxVerts[ j ] 
					||
				maxVert >= minVerts[ j ] && maxVert <= maxVerts[ j ]
					)
			{
				r3dArtBug( "Material chunks of mesh %s intersect!\n", this->FileName.c_str() ) ;
				unintersected = false ;
				break ;
			}
		}

		memcpy( &tempIndexBuffer0[ 0 ], Indices + tbatch.StartIndex, numBatchIndices * sizeof( Indices[0] ) ) ;

		if( minVert )
		{
			for( int i = 0, e = (int)tempIndexBuffer0.size() ; i < e ; i ++ )
			{
				tempIndexBuffer0[ i ] -= minVert ;
			}
		}

#if 0
		VertexCacheOptimizer opti ;
		opti.Optimize( &newIndexBuffer0[0], NumIndices / 3 ) ;
#else
		optimizePostTLTipsify( (unsigned int*)&tempIndexBuffer1[0], (unsigned int*)&tempIndexBuffer0[0], numBatchIndices, numBatchVerts ) ;
		tempIndexBuffer1.swap( tempIndexBuffer0 ) ;
#endif

		if( unintersected )
		{
			BuildPreTLOptimizeMap( &remapIndices, &tempIndexBuffer1[0], &tempIndexBuffer0[0], numBatchVerts, numBatchIndices ) ;
			tempIndexBuffer1.swap( tempIndexBuffer0 ) ;
		}

		if( minVert )
		{
			for( int i = 0, e = (int)tempIndexBuffer0.size() ; i < e ; i ++ )
			{
				tempIndexBuffer0[ i ] += minVert ;
			}
		}

		memcpy( Indices + tbatch.StartIndex, &tempIndexBuffer0[ 0 ], numBatchIndices * sizeof( Indices[ 0 ] ) ) ;

		if( unintersected )
		{
			if( VertexPositions )	RemapComponent( VertexPositions + minVert,	remapIndices ) ;
			if( VertexNormals )		RemapComponent( VertexNormals + minVert,	remapIndices ) ;
			if( VertexUVs )			RemapComponent( VertexUVs + minVert,		remapIndices ) ;
			if( VertexSecondUVs )		RemapComponent( VertexSecondUVs + minVert,	remapIndices ) ;
			if( VertexTangents )	RemapComponent( VertexTangents + minVert,	remapIndices ) ;
			if( VertexTangentWs )	RemapComponent( VertexTangentWs + minVert,	remapIndices ) ;
			if( pWeights )			RemapComponent( pWeights + minVert,			remapIndices ) ;
			if( VertexColors )		RemapComponent( VertexColors + minVert,		remapIndices ) ;
		}
	}
}

//------------------------------------------------------------------------

void
r3dMesh::DisableThumbnails()
{
	for( int i = 0, e = NumMatChunks; i < e; i ++ )
	{
		r3dMaterial* mat = MatChunks[ i ].Mat;

		if( mat != r3dMaterialLibrary::GetDefaultMaterial() )
		{
			MatChunks[ i ].Mat->DisableThumbnails();
		}
	}
}

//------------------------------------------------------------------------

void
r3dMesh::EnableThumbnails()
{
	for( int i = 0, e = NumMatChunks; i < e; i ++ )
	{
		r3dMaterial* mat = MatChunks[ i ].Mat;

		if( mat != r3dMaterialLibrary::GetDefaultMaterial() )
		{
			mat->EnableThumbnails();
		}
	}
}

template <typename T>
/*static*/
void
r3dMesh::RemapComponent( T* comp, const r3dgfxVector(int)& compMap )
{
	size_t reqTempSize = sizeof( T ) * compMap.size() ;

	if( reqTempSize > tempGenericMemSize )
	{
		free( tempGenericMem ) ;
		tempGenericMem		= malloc( reqTempSize ) ;
		tempGenericMemSize	= reqTempSize ;
	}

	T* tmp = (T*) tempGenericMem ;

	memcpy( tmp, comp, reqTempSize ) ;

	for( int i = 0, e = compMap.size() ; i < e ; i ++ )
	{
		int idx = compMap[ i ] ;

		r3d_assert( idx < e ) ;

		comp[ i ] = tmp[ idx ] ;
	}
}

template void r3dMesh::RemapComponent( r3dPoint3D* comp, const r3dgfxVector(int)& compMap ) ;
template void r3dMesh::RemapComponent( r3dPoint2D* comp, const r3dgfxVector(int)& compMap ) ;
template void r3dMesh::RemapComponent( char* comp, const r3dgfxVector(int)& compMap ) ;
template void r3dMesh::RemapComponent( r3dWeight* comp, const r3dgfxVector(int)& compMap ) ;

void FillPosition( R3D_MESH_VERTEX& vertex, const r3dPoint3D& pos, const r3dPoint3D& unpackScale )
{
	vertex.pos = r3dNormShort4( pos, unpackScale );
}

void FillPosition( R3D_BENDING_MESH_VERTEX& vertex, const r3dPoint3D& pos, const r3dPoint3D& unpackScale )
{
	vertex.pos = r3dNormShort4( pos, unpackScale );
}

void FillPosition( R3D_MESH_PRECISE_VERTEX& vertex, const r3dPoint3D& pos, const r3dPoint3D& unpackScale )
{
	vertex.pos = float3( pos.x, pos.y, pos.z );
}

void FillPosition( r3dSkinnedMeshVertex& vertex, const r3dPoint3D& pos, const r3dPoint3D& unpackScale )
{
	vertex.pos = r3dNormShort4( pos, unpackScale );
}

void FillPosition( R3D_2UV_MESH_VERTEX& vertex, const r3dPoint3D& pos, const r3dPoint3D& unpackScale )
{
	vertex.pos = r3dNormShort4( pos, unpackScale );
}

r3dPoint2D UnpackTexcoord(short u, short v, const r3dPoint2D &texcUnpackScale)
{
	r3dPoint2D rv;
	rv.x = u / 32767.0f;
	rv.y = v / 32767.0f;

	rv.x *= texcUnpackScale.x;
	rv.y *= texcUnpackScale.y;
	return rv;
}

short2 PackTexcoord( const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale )
{
	float x = uv.x / texcUnpackScale.x ;
	float y = uv.y / texcUnpackScale.y ;

	r3d_assert( x >= -1.f && x <= 1.f ) ;
	r3d_assert( y >= -1.f && y <= 1.f ) ;

	short sx = short( 32767 * x ) ;
	short sy = short( 32767 * y ) ;

	return short2( sx, sy ) ;
}

void FillTexcoord( R3D_MESH_VERTEX& vertex, const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale )
{
	vertex.texcoord = PackTexcoord( uv, texcUnpackScale ) ;
}

void FillTexcoord( R3D_BENDING_MESH_VERTEX& vertex, const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale )
{
	vertex.texcoord = PackTexcoord( uv, texcUnpackScale ) ;
}

void FillTexcoord( R3D_MESH_PRECISE_VERTEX& vertex, const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale )
{
	vertex.texcoord = float2( uv.x, uv.y );
}

void FillTexcoord( r3dSkinnedMeshVertex& vertex, const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale )
{
	vertex.texcoord = PackTexcoord( uv, texcUnpackScale ) ;
}

void FillTexcoord( R3D_2UV_MESH_VERTEX& vertex, const r3dPoint2D& uv, const r3dPoint2D& texcUnpackScale )
{
	vertex.texcoord = PackTexcoord( uv, texcUnpackScale ) ;
}

template <typename VERTEX> 
R3D_FORCEINLINE void r3dMesh::FillSingleVertexBase(VERTEX* pVertices, int index)
{
	int ind = index;

	FillPosition( *pVertices, VertexPositions[ ind ], unpackScale );

	pVertices->normal	= r3dUnitNormByte4( VertexNormals[ ind ] );

	pVertices->tangent		= r3dUnitNormByte4( VertexTangents[ ind ] );
	pVertices->tangent.w	= 255 - VertexTangentWs[ ind ];

	FillTexcoord( *pVertices, VertexUVs[ ind ], texcUnpackScale );
}

template <> 
void r3dMesh::FillSingleVertexBase(R3D_WORLD_VERTEX* pVertices, int index)
{
	int ind = index;
	pVertices->Pos 		= VertexPositions[ ind ];
	pVertices->Normal	= VertexNormals[ ind ];

	pVertices->vU		= VertexTangents[ ind ];
	pVertices->vV		= VertexNormals[ ind ].Cross( VertexTangents[ ind ] ) * ( VertexTangentWs[ ind ] > 0 ? 1.f : -1.f ) ;

	pVertices->tu 		= VertexUVs[ ind ].x;
	pVertices->tv 		= VertexUVs[ ind ].y;
}


// instantiate some 
template void r3dMesh::FillSingleVertexBase(R3D_MESH_VERTEX* pVertices, int index);

void r3dMesh::FillSingleVertex(void *in_pV, int index)
{
	//r3d_assert(!pWeights == meshUsingSimpleVertex);

	if(pWeights == NULL) 
	{
		if( VertexFlags & vfPrecise )
		{
			R3D_MESH_PRECISE_VERTEX* pV = (R3D_MESH_PRECISE_VERTEX*)in_pV;
			FillSingleVertexBase<R3D_MESH_PRECISE_VERTEX>(pV, index);
		}
		else
		if( VertexColors )
		{
			R3D_BENDING_MESH_VERTEX* pV = (R3D_BENDING_MESH_VERTEX*)in_pV;
			FillSingleVertexBase<R3D_BENDING_MESH_VERTEX>(pV, index);

			pV->bendCoefs.x = VertexColors[ index ].R ;
			pV->bendCoefs.y = VertexColors[ index ].G ;
			pV->bendCoefs.z = VertexColors[ index ].B ;
			pV->bendCoefs.w = 0 ;
		}
		else
		if( VertexFlags & vfHaveSecondUV)
		{
			R3D_2UV_MESH_VERTEX* pV = (R3D_2UV_MESH_VERTEX*)in_pV;
			FillSingleVertexBase<R3D_2UV_MESH_VERTEX>(pV, index);
			pV->tex2coord = PackTexcoord( VertexSecondUVs[index], texc2UnpackScale ) ;
		}
		else
		{
			R3D_MESH_VERTEX* pV = (R3D_MESH_VERTEX*)in_pV;
			FillSingleVertexBase<R3D_MESH_VERTEX>(pV, index);
		}
		return;
	}

	r3dSkinnedMeshVertex* pV = (r3dSkinnedMeshVertex*)in_pV;
	FillSingleVertexBase<r3dSkinnedMeshVertex>(pV, index);

	int idx = index;

	pV->weights.x = r3dNormShort( pWeights[ idx ].Weight[ 0 ], 1.0f);
	pV->weights.y = r3dNormShort( pWeights[ idx ].Weight[ 1 ], 1.0f);
	pV->weights.z = r3dNormShort( pWeights[ idx ].Weight[ 2 ], 1.0f);
	pV->weights.w = r3dNormShort( pWeights[ idx ].Weight[ 3 ], 1.0f);

	pV->indices.x = pWeights[idx].BoneID[0];
	pV->indices.y = pWeights[idx].BoneID[1];
	pV->indices.z = pWeights[idx].BoneID[2];
	pV->indices.w = pWeights[idx].BoneID[3];

	/*
	r3d_assert(pV->indices[0] < iNumUsedBones);
	r3d_assert(pV->indices[1] < iNumUsedBones);
	r3d_assert(pV->indices[2] < iNumUsedBones);
	r3d_assert(pV->indices[3] < iNumUsedBones);
	*/  

	return;
}

static void r3dMesh_NormalizeTexcoords(int NumVertices, r3dPoint2D* VertexUVs, r3dPoint2D& texcUnpackScale)
{
	// calc texc unpack scale
	float	miu = +FLT_MAX, miv = +FLT_MAX, 
			mau = -FLT_MAX, mav = -FLT_MAX ;

	for( int i = 0 ; i < NumVertices ; i ++ )
	{
		const r3dPoint2D& uv = VertexUVs[ i ] ;
		miu = R3D_MIN( uv.x, miu ) ;
		mau = R3D_MAX( uv.x, mau ) ;
		miv = R3D_MIN( uv.y, miv ) ;
		mav = R3D_MAX( uv.y, mav ) ;
	}

	float avU = ( miu + mau ) * 0.5f ;
	float avV = ( miv + mav ) * 0.5f ;

	float dU = float( (int)avU ) ;
	float dV = float( (int)avV ) ;

	for( int i = 0 ; i < NumVertices ; i ++ )
	{
		r3dPoint2D& uv = VertexUVs[ i ] ;

		uv.x -= dU ;
		uv.y -= dV ;
	}

	miu -= dU ;
	mau -= dU ;
	miv -= dV ;
	mav -= dV ;

	texcUnpackScale.x = R3D_MAX( fabs( miu ), fabs( mau ) ) ;
	texcUnpackScale.y = R3D_MAX( fabs( miv ), fabs( mav ) ) ;

	// everything near zero - won't heart if we norm using 1
	if( texcUnpackScale.x <= FLT_MIN ) { texcUnpackScale.x = 1.0f ; }
	if( texcUnpackScale.y <= FLT_MIN ) { texcUnpackScale.y = 1.0f ; }
}


void r3dMesh::NormalizeTexcoords()
{
	r3dMesh_NormalizeTexcoords(NumVertices, VertexUVs, texcUnpackScale);
	if(VertexSecondUVs)
		r3dMesh_NormalizeTexcoords(NumVertices, VertexSecondUVs, texc2UnpackScale);
}

/*static*/
void
r3dMesh::FillMeshBuffers( r3dTaskParams* params )
{
	if( g_async_loading->GetInt() && ! R3D_IS_MAIN_THREAD() )
	{
		static_cast<r3dMesh*>( static_cast< r3dTaskPtrParams* >( params )->Ptr )->DoFillBuffers( true );
	}

}

void r3dMesh::FillBuffersAsync()
{
#if R3D_ALLOW_ASYNC_MESH_LOADING
	if( g_async_loading->GetInt() && R3D_IS_MAIN_THREAD() && g_pBackgroundTaskDispatcher )
	{
		r3dBackgroundTaskDispatcher::TaskDescriptor td ;

		/**	Function to execute. */
		td.Fn				= FillMeshBuffers ;

		r3dTaskPtrParams* tpp = g_pBackgroundTaskDispatcher->AllocPtrTaskParam() ;
		tpp->Ptr = this ;

		td.Params			= tpp ;
		td.CompletionFlag	= NULL ;

		td.Priority			= R3D_TASKPRIORITY_MESH;

		g_pBackgroundTaskDispatcher->AddTask( td ) ;
	}
	else
#endif
	{
		DoFillBuffers( true );
	}
}

void r3dMesh::FillBuffers()
{
	DoFillBuffers( true );
}

void r3dMesh::FillBuffers_SaveVertexData()
{
	DoFillBuffers( false );
}

static void DoFillBuffersMeshMainThread( void* Ptr )
{
	static_cast< r3dMesh* >( Ptr )->DoFillBuffersMainThread() ;
}

void r3dMesh::DoFillBuffersMainThread()
{
	// ban precise & bending at the same time
	r3d_assert( ! ( VertexFlags & vfPrecise )
					||
				! ( VertexFlags & vfBending ) ) ;

	if(pWeights)
	{
		FillBuffersUnique<r3dSkinnedMeshVertex>();
	}
	else
	if( VertexFlags & vfPrecise )
	{
		FillBuffersUnique<R3D_MESH_PRECISE_VERTEX>();
	}
	else
	if( VertexFlags & vfBending )
	{
		FillBuffersUnique<R3D_BENDING_MESH_VERTEX>();
	}
	else
	if( VertexFlags & vfHaveSecondUV )
	{
		FillBuffersUnique<R3D_2UV_MESH_VERTEX>();
	}
	else
	{
		FillBuffersUnique<R3D_MESH_VERTEX>();
	}
}

void r3dMesh::DoFillBuffers( bool deleteSystemVertexData )
{
	if(!m_Loaded)
		return;

	if(r3dMeshObject_SkipFillBuffers)
		return;

	if(!NumIndices || !NumVertices) 
		return;

	// if those asserts fire - means you are calling this function twice!! Shouldn't happen
	r3d_assert(VertexNormals);
	r3d_assert(VertexUVs);
	r3d_assert(VertexTangents);
	r3d_assert(VertexTangentWs);

	texcUnpackScale = r3dPoint2D( 1.f, 1.f ) ;

	if( !(VertexFlags & vfPrecise) )
	{
		NormalizeTexcoords();
	}

	if( g_async_loading->GetInt() && !R3D_IS_MAIN_THREAD() )
	{
		// throw it back into main thread, because we can't break locking when we're rendering from the same buffer
		// ( it is now shared )
		ProcessCustomDeviceQueueItem( DoFillBuffersMeshMainThread, this ) ;
	}
	else
	{
		DoFillBuffersMainThread() ;
	}


	if( deleteSystemVertexData )
	{
		SAFE_DELETE_ARRAY(VertexNormals);
		SAFE_DELETE_ARRAY(VertexUVs);
		SAFE_DELETE_ARRAY(VertexSecondUVs);
		SAFE_DELETE_ARRAY(VertexTangents);
		SAFE_DELETE_ARRAY(VertexTangentWs);
		SAFE_DELETE_ARRAY(VertexColors);
	}

	InterlockedExchange( &m_Drawable, 1 );
}  

template <class T>
void r3dMesh::FillBuffersUnique()
{

// 	int vb_size = NumVertices * sizeof(T);
// 	int ib_size = NumIndices * sizeof(DWORD);
// 
// 	DWORD	  vb_usage;
// 	D3DPOOL vb_pool;
// 	DWORD	  vb_lock;
// 
// 	vb_usage = D3DUSAGE_WRITEONLY;
// 	vb_pool  = D3DPOOL_MANAGED;
// 	vb_lock  = 0;

	typedef DWORD IndexType ;

	if( Flags & obfPlayerMesh )
	{
		VertexFlags |= vfUnsharedBuffer ;
	}

	if( VertexFlags & vfUnsharedBuffer )
	{
		UnsharedVertexBuffer = gfx_new r3dVertexBuffer( NumVertices, sizeof( T ) ) ;
		UnsharedIndexBuffer = gfx_new r3dIndexBuffer( NumIndices, false, sizeof( IndexType ) ) ;

		buffers.InitUnshared( UnsharedVertexBuffer, UnsharedIndexBuffer ) ;
	}
	else
	{
		buffers.Init(NumVertices, NumIndices, sizeof(T), sizeof(IndexType));
	}

	SizeInVMem = sizeof(T) * NumVertices + sizeof(IndexType) * NumIndices ;

	if( Flags & obfPlayerMesh )
	{
		r3dRenderer->Stats.AddPlayerBufferMem( SizeInVMem ) ;
	}

	void *vmem, *imem;
	buffers.Lock(vmem, imem);

	if( !r_optimize_meshes->GetInt() )
	{
		memcpy(imem, Indices, sizeof(DWORD)*NumIndices);
	}

	for (int i=0;i<NumVertices;i++)
		FillSingleVertex(&((T*)vmem)[i], i);

	if( r_optimize_meshes->GetInt() )
	{
		int	minVerts[ R3D_ARRAYSIZE( MatChunks ) ] ;
		int maxVerts[ R3D_ARRAYSIZE( MatChunks ) ] ;

		for( int i = 0, e = this->NumMatChunks ; i < e; i ++ )
		{
			const r3dTriBatch& tbatch = this->MatChunks[ i ] ;

			int minVert = 0x7fffffff ;
			int maxVert = 0 ;

			// antiwarning
			{
				for( int i = tbatch.StartIndex, e = tbatch.EndIndex ; i < e; i ++ )
				{
					minVert = R3D_MIN( (int)Indices[ i ], minVert ) ;
					maxVert = R3D_MAX( (int)Indices[ i ], maxVert ) ;
				}
			}

			minVerts[ i ] = minVert ;
			maxVerts[ i ] = maxVert ;
		}

		for( int i = 0, e = this->NumMatChunks ; i < e; i ++ )
		{
			const r3dTriBatch& tbatch = this->MatChunks[ i ] ;

			int numBatchIndices = tbatch.EndIndex - tbatch.StartIndex ;

			tempIndexBuffer0.resize( numBatchIndices ) ;
			tempIndexBuffer1.resize( numBatchIndices ) ;

			int minVert = minVerts[ i ] ;
			int maxVert = maxVerts[ i ] ;

			int numBatchVerts = 0 ;

			if( minVert > maxVert )
			{
				minVert = maxVert ;
			}
			else
			{
				numBatchVerts = maxVert - minVert + 1 ;
			}

			// is this vertex range unintersected?
			bool unintersected = true ;

			for( int j = 0, e = this->NumMatChunks ; j < e ; j ++ )
			{
				if( i == j )
					continue ;

				if( minVert >= minVerts[ j ] && minVert <= maxVerts[ j ] 
						||
					maxVert >= minVerts[ j ] && maxVert <= maxVerts[ j ]
						)
				{
					r3dArtBug( "Material chunks of mesh %s intersect!\n", this->FileName.c_str() ) ;
					unintersected = false ;
					break ;
				}
			}

			memcpy( &tempIndexBuffer0[ 0 ], Indices + tbatch.StartIndex, numBatchIndices * sizeof( Indices[0] ) ) ;

			if( minVert )
			{
				for( int i = 0, e = (int)tempIndexBuffer0.size() ; i < e ; i ++ )
				{
					tempIndexBuffer0[ i ] -= minVert ;
				}
			}

#if 0
			VertexCacheOptimizer opti ;
			opti.Optimize( &newIndexBuffer0[0], NumIndices / 3 ) ;
#else
			optimizePostTLTipsify( (unsigned int*)&tempIndexBuffer1[0], (unsigned int*)&tempIndexBuffer0[0], numBatchIndices, numBatchVerts ) ;
			tempIndexBuffer1.swap( tempIndexBuffer0 ) ;
#endif

			if( unintersected )
			{
				PreTLOptimize( (T*)vmem + minVert, &tempIndexBuffer1[0], (T*) vmem + minVert, &tempIndexBuffer0[0], numBatchVerts, numBatchIndices ) ;
				tempIndexBuffer1.swap( tempIndexBuffer0 ) ;
			}

			if( minVert )
			{
				for( int i = 0, e = (int)tempIndexBuffer0.size() ; i < e ; i ++ )
				{
					tempIndexBuffer0[ i ] += minVert ;
				}
			}

			memcpy( (int*)imem + tbatch.StartIndex, &tempIndexBuffer0[0], numBatchIndices * sizeof( Indices[ 0 ] ) ) ;
		}
	}
	else
	{
		memcpy( (int*)imem, Indices, sizeof( Indices[ 0 ] ) * NumIndices ); 
	}

	buffers.Unlock();

}

//-----------------------------------------------------------------------
// will make a pivot in 0,0,0 in local coord
void r3dMesh::ResetXForm()
{
	r3d_assert( m_Loaded ) ;

	for(int i = 0; i < NumVertices; i++)
		VertexPositions[i] -= vPivot;
	vPivot = r3dPoint3D(0,0,0);

	RecalcBoundBox();
	CentralPoint = (localBBox.Org + (localBBox.Size / 2.0f));

	//FillBuffers();
}

//------------------------------------------------------------------------

int r3dMesh::HasMaterialOfType( const char* typeName )
{
	for( int i = 0, e = NumMatChunks; i < e; i ++ )
	{
		if( r3dMaterial* mat = MatChunks[ i ].Mat )
		{
			if( !strcmp( mat->TypeName, typeName ) )
			{
				return 1;
			}
		}
	}

	return 0;
}

//-----------------------------------------------------------------------

void r3dMeshSetShaderConsts( const D3DXMATRIX& world, const r3dMesh* mesh )
{
	PrecalculatedMeshShaderConsts pm;
	if(mesh)
		r3dPrepareMeshShaderConsts(pm, world, &mesh->unpackScale, &mesh->texcUnpackScale, r3dRenderer->ViewMatrix, r3dRenderer->ViewProjMatrix, &mesh->texc2UnpackScale);
	else
		r3dPrepareMeshShaderConsts(pm, world, NULL, NULL, r3dRenderer->ViewMatrix, r3dRenderer->ViewProjMatrix, NULL);
	r3dApplyPreparedMeshVSConsts(pm);
}

//------------------------------------------------------------------------

void r3dMeshSetShaderConsts_Localized( const D3DXMATRIX& world, const r3dMesh* mesh )
{
	PrecalculatedMeshShaderConsts pm;
	if(mesh)
		r3dPrepareMeshShaderConsts(pm, world, &mesh->unpackScale, &mesh->texcUnpackScale, r3dRenderer->ViewMatrix_Localized, r3dRenderer->ViewProjMatrix_Localized, &mesh->texc2UnpackScale);
	else
		r3dPrepareMeshShaderConsts(pm, world, NULL, NULL, r3dRenderer->ViewMatrix_Localized, r3dRenderer->ViewProjMatrix_Localized, NULL);
	r3dApplyPreparedMeshVSConsts(pm);
}

//////////////////////////////////////////////////////////////////////////

void r3dPrepareMeshShaderConsts(PrecalculatedMeshShaderConsts &out, const D3DXMATRIX &world, const r3dPoint3D* ScaleBox, const r3dPoint2D* TexcUnpackScale, const D3DXMATRIX& viewMtx, const D3DXMATRIX& viewProjMtx, const r3dPoint2D* Texc2UnpackScale)
{
	D3DXMATRIX scale;
	if (ScaleBox)
	{
		D3DXMatrixScaling(&scale, ScaleBox->x, ScaleBox->y, ScaleBox->z);
	}
	else
	{
		D3DXMatrixIdentity(&scale);
	}

	D3DXMATRIX scaledWorld = scale * world;

	D3DXMATRIX ShaderMat;
	ShaderMat =  scaledWorld * viewProjMtx;
	D3DXMatrixTranspose(&ShaderMat, &ShaderMat );

	memcpy( out.VS.worldViewProj, &ShaderMat, sizeof out.VS.worldViewProj );

	ShaderMat =  scaledWorld;
	D3DXMatrixTranspose(&ShaderMat, &ShaderMat);
	memcpy( &out.VS.scaledWorld, &ShaderMat, sizeof out.VS.scaledWorld );

	D3DXMATRIX UnpackNormal(
		2.f,   0.f,  0.f, 0.f,
		0.f,   2.f,  0.f, 0.f,
		0.f,   0.f,  2.f, 0.f,
		-1.f, -1.f, -1.f, 1.f	);

	D3DXMATRIX world_noT = world;

	world_noT._41 = 0.f;
	world_noT._42 = 0.f;
	world_noT._43 = 0.f;

	// normal means no unpack scale, we do not support non uniform scaling
	// hence no inverse transpose.
	// float4x4 NormalMatrix : register (c8);
	ShaderMat =  UnpackNormal * world_noT;
	D3DXMatrixTranspose(&ShaderMat, &ShaderMat);
	memcpy( out.VS.normalWorld, &ShaderMat, sizeof out.VS.normalWorld ) ;

	ShaderMat =  scaledWorld * viewMtx;
	D3DXMatrixTranspose(&ShaderMat, &ShaderMat);
	memcpy( out.VS.scaledWorldView, &ShaderMat, sizeof out.VS.scaledWorldView ) ;

	// float4x4 NormalViewMatrix		: register (c16);

	D3DXMATRIX cam_noT = viewMtx;

	cam_noT._41 = 0.f;
	cam_noT._42 = 0.f;
	cam_noT._43 = 0.f;

	ShaderMat =  UnpackNormal * world_noT * cam_noT;
	D3DXMatrixTranspose(&ShaderMat, &ShaderMat);

	memcpy( out.VS.normalWorldView, &ShaderMat, sizeof out.VS.normalWorldView ) ;

	if( TexcUnpackScale )
	{
		out.VS.texcUnpackScale = float4( TexcUnpackScale->x, TexcUnpackScale->y, 0.f, 0.f ) ;
	}
	else
	{
		out.VS.texcUnpackScale = float4( 1.f, 1.f, 1.f, 1.f ) ;
	}
	
	if( Texc2UnpackScale )
	{
		out.VS.texcUnpackScale.z = Texc2UnpackScale->x;
		out.VS.texcUnpackScale.w = Texc2UnpackScale->y;
	}
}

//------------------------------------------------------------------------

void r3dMeshSetVSTexcUnpackScale( r3dPoint2D unpackScale )
{
	float vConst[ 4 ] = {
		unpackScale.x, unpackScale.y, 0.f, 0.f
	} ;

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 16, vConst, 1 ) );
}

//------------------------------------------------------------------------

void r3dApplyPreparedMeshVSConsts(const PrecalculatedMeshShaderConsts &vsc)
{
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF(0, (float*)&vsc.VS, sizeof vsc.VS / 16 ) );
}

void r3dApplyPreparedMeshVSConsts_DepthPrepass(const PrecalculatedMeshShaderConsts &vsc)
{
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF(0, (float*)&vsc, 4 ) );
}

//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////

static void mesh_CreateWorldMatrix(D3DXMATRIX& out, const r3dPoint3D& vPivot, const r3dVector& pos, const r3dVector& scale, const D3DXMATRIX& rotation)
{
	D3DXMATRIX  m1, m2, ms, mWorld, mMove;
	D3DXMatrixTranslation(&mMove, pos.X, pos.Y, pos.Z );
	//D3DXMatrixTranslation(&m1, vPivot.X, vPivot.Y, vPivot.Z );
	//D3DXMatrixTranslation(&m2, -vPivot.X, -vPivot.Y, -vPivot.Z );
	D3DXMatrixScaling(&ms, scale.X, scale.Y, scale.Z );

	out = ms;	// scale
	//out *= m2;	// offset so pivot will be at 0,0,0
	out *= rotation;  
	//out *= m1;	// restore pivot
	out *= mMove;
	return;
}


//-----------------------------------------------------------------------
void r3dMesh::GetWorldMatrix( const r3dVector& vPos, const r3dVector& vScl, const r3dVector& vRot, D3DXMATRIX &mRes)
{
	D3DXMATRIX mRot;
	D3DXMatrixRotationYawPitchRoll(&mRot, R3D_DEG2RAD(vRot.X), R3D_DEG2RAD(vRot.Y), R3D_DEG2RAD(vRot.Z));
	mesh_CreateWorldMatrix( mRes, vPivot, vPos, vScl, mRot );
}

//-----------------------------------------------------------------------
void r3dMesh::SetShaderConsts(const r3dVector& pos, const r3dVector& scale, const D3DXMATRIX& rotation)
{
	D3DXMATRIX mWorld;
	mesh_CreateWorldMatrix(mWorld, vPivot, pos, scale, rotation);

	r3dMeshSetShaderConsts( mWorld, this );
}


void r3dMesh::SetShaderConsts(const D3DXMATRIX& world)
{
	r3dMeshSetShaderConsts( world, this );
}

//------------------------------------------------------------------------

void r3dMesh::SetShaderConsts_Localized(const D3DXMATRIX& world)
{
	r3dMeshSetShaderConsts_Localized( world, this );
}



void SetObjLightsConstants(r3dLightSystem *WL, r3dBoundBox &BBox)
{
	r3dVector VL = r3dVector(1.0f,0.5f,1.0f);
	r3dVector LC = r3dVector(0,0,0);

	int  NumPointLights = 0;
	r3dVector PP[8];
	float    PR[8];
	r3dVector PC[8];

	for (int i=0;i<8;i++)
	{
		PP[i] = r3dVector(100,100,100);
		PR[i] = 0;
		PC[i] = r3dVector(0,0,0);
	}

	for (uint32_t i = 0, i_end = WL->Lights.Count(); i != i_end; ++i)
	{
		r3dLight *L = WL->Lights[i];
		if (!L) continue;

		if (L->Type == R3D_DIRECT_LIGHT )
		{
			VL = -L->Direction;
			LC = r3dVector(float(L->R)/255.0f,float(L->G)/255.0f, float(L->B)/255.0f );
		}

		if (L->Type == R3D_OMNI_LIGHT && NumPointLights < 8 && L->IsOn())
			if(BBox.Intersect(L->BBox))
			{
				PP[NumPointLights] = r3dVector(L->X,L->Y,L->Z);
				PC[NumPointLights] = r3dVector(float(L->R)/255.0f,float(L->G)/255.0f, float(L->B)/255.0f );
				PR[NumPointLights] = L->GetOuterRadius(); //*1.2f;
				NumPointLights ++;
			}
	}  

	/*
	float AC[4]; 
	AC[0] = float(r3dRenderer->AmbientColor.R)/255.0f;
	AC[1] = float(r3dRenderer->AmbientColor.G)/255.0f;
	AC[2] = float(r3dRenderer->AmbientColor.B)/255.0f;
	AC[3] = 1;

	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  5, (float *)&VL,  1 );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  6, (float *)&LC,  1 );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  7, AC,  1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  7, AC,  1 );
	*/

	float PPos[16][4]; 
	//  float PCol[4][4]; 


	for (int i=0;i<8;i++)
	{
		PPos[i][0] = PP[i].X;
		PPos[i][1] = PP[i].Y;
		PPos[i][2] = PP[i].Z;
		PPos[i][3] = PR[i];

		PPos[i+8][0] = PC[i].X;
		PPos[i+8][1] = PC[i].Y;
		PPos[i+8][2] = PC[i].Z;
		PPos[i+8][3] = 1;
	}

	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  30, PPos[0],  16 );
}

extern int VS_SMDEPTHPASS_ORTHO_ID;
extern int VS_SMDEPTHPASS_ORTHO_INSTANCED_ID;
extern int VS_SMDEPTHPASS_PROJ_ID;
extern int VS_SMDEPTHPASS_PROJ_PARABOLOID_ID;
extern int VS_SMDEPTHPASS_PROJ_INSTANCED_ID;
extern int VS_FILLGBUFFER_ID;
extern int VS_DEPTH_PREPASS_ID;
extern int VS_FILLGBUFFER_INSTANCED_ID;
extern int VS_SMDEPTHPATH_APEX_ORTHO_ID;
extern int VS_SMDEPTHPATH_APEX_PROJ_ID;
extern int VS_FILLGBUFFER_INSTANCED_2UV_ID;


void r3dMesh::DrawMeshInstanced2()
{

	r3d_assert(numInstancesInVB < InstancingMeshVBCache::MaxInstancesToDraw);
	static const int maxInstancedInVB = InstancingMeshVBCache::MaxInstancesToDraw;

	D3DXMATRIX mViewProjT ;
	D3DXMatrixTranspose(&mViewProjT, &r3dRenderer->ViewProjMatrix );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF ( 0, (float*)&mViewProjT, 4 );

	float texScale[4] = { texcUnpackScale.x, texcUnpackScale.y, texc2UnpackScale.x, texc2UnpackScale.y};
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 16, texScale, 1 ) );

	buffers.Set();

	r3dRenderer->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | numInstancesInVB));
	r3dRenderer->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));

	// FIXME: [Alexey] Reaaly strange behavior here. Some objects are culled but not added to render. needs to be fixed on higher level
	if (InstancesVB->Parent->LockCount > 0)
	{
		gInstancingMeshVBCache.UnlockVBRange(InstancesVB);
	}

	r3dMatVSType matVs;
	
	if (VertexFlags & vfHaveSecondUV)
	{ 
		matVs = R3D_MATVS_INSTANCING_2UV;
		d3dc._SetDecl( r3dInstancedUnionMeshVertex2UV::getDecl() );
	}
	else
	{
		matVs = R3D_MATVS_INSTANCING;
		d3dc._SetDecl( r3dInstancedUnionMeshVertex::getDecl() );
	}

	// We have selfIllum premultiplied with GameObject coor in instancing data. Here we provide DiffuseColor from r3dMaterial, no need to multiply it with per-instance data
	r3dMaterial::SetRenderedObjectColor( r3dColor(0xFFFFFFFF) ) ;

	InstancesVB->Parent->InstancesVB->Set(1, InstancesVB->StartIndex);

	if(NumMatChunks)
	{
		for (int i=0;i<NumMatChunks;i++)
		{
			const r3dTriBatch& batch = MatChunks[ i ];
			if ((batch.Mat->Flags & R3D_MAT_TRANSPARENT) == 0)
			{
				batch.Mat->Start( matVs, R3D_MATF_INSTANCING );

				r3dInstancedDraw idraw( numInstancesInVB );
				r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0,buffers.minVertexIndex,NumVertices, buffers.startIndex + MatChunks[i].StartIndex,(MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);

				batch.Mat->End();
			}
		}
	}
	else
	{
		r3d_assert(0);
		r3dInstancedDraw idraw( numInstancesInVB );
		r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0,buffers.minVertexIndex,NumVertices,buffers.startIndex,NumIndices/3);
	}

	r3dRenderer->SetStreamSourceFreq(0, 1);
	r3dRenderer->SetStreamSourceFreq(1, 1);
	d3dc._SetDecl(R3D_MESH_VERTEX::getDecl()); 

	ResetInstancingInfo();
}

//-----------------------------------------------------------------------

//void r3dMesh::DrawMeshInstanced(const D3DXMATRIX* world, int ShadowMap, bool force_render )
//{
//	//R3DPROFILE_FUNCTION("r3dMesh::DrawMeshInstanced");
//	if(!IsDrawable())
//		return;
//
//	static const int maxInstancedInVB = MaxInstancesToDraw;
//	if(!pInstancesVB)
//	{
//		r3d_assert ( sizeof(R3D_MESH_VERTEX) == D3DXGetDeclVertexSize( R3D_MESH_VERTEX::VBDecl, 0 ) );
//		r3d_assert ( sizeof(InstancedData) == D3DXGetDeclVertexSize( r3dInstancedDataVertex::VBDecl, 1 ) );
//
//		pInstancesVB = gfx_new r3dVertexBuffer( maxInstancedInVB, sizeof ( InstancedData ), 0, true );
//
//		numInstancesInVB = 0;
//	}
//	if(world)
//		//InstanceArray[numInstancesInArray++].world = *world;
//	{
//		float * mat = InstanceArray[numInstancesInArray].world;
//
//		//InstanceArray[numInstancesInArray].world = *world;
//
//		mat[0] = world->_11;
//		mat[1] = world->_21;
//		mat[2] = world->_31;
//		mat[3] = world->_41;
//		mat[4] = world->_12;
//		mat[5] = world->_22;
//		mat[6] = world->_32;
//		mat[7] = world->_42;
//		mat[8] = world->_13;
//		mat[9] = world->_23;
//		mat[10] = world->_33;
//		mat[11] = world->_43;
//
//
//		numInstancesInArray++;
//	}
//
//	if(numInstancesInArray == MaxInstancesToDraw || (force_render && numInstancesInArray>0))
//	{
//		if(numInstancesInArray == 1)
//		{
//			SetShaderConsts(InstanceArray[0].world);
//			if( ShadowMap )
//			{
//				DrawMeshShadows();
//			}
//			else
//			{
//				DrawMeshDeferred( r3dColor::white, 0 );
//			}
//		}
//		else
//		{
//
//			D3DXMATRIX mViewProjT ;
//			D3DXMatrixTranspose(&mViewProjT, &r3dRenderer->ViewProjMatrix );
//			r3dRenderer->pd3ddev->SetVertexShaderConstantF ( 0, (float*)&mViewProjT, 4 );
//
//			d3dc._SetDecl( r3dInstancedUnionMeshVertex::getDecl() );
//
//			int offset = 0;
//			{
//				InstancedData* pInstances = NULL;
//				if((numInstancesInVB + numInstancesInArray) >= maxInstancedInVB)
//				{
//					//D3D_V ( pInstancesVB->Lock(0,0,(void**)&pInstances, D3DLOCK_DISCARD ) );
//					pInstances = (InstancedData*) pInstancesVB->Lock( 0,pInstancesVB->GetItemCount() );
//					numInstancesInVB = numInstancesInArray;
//					offset = 0;
//				}
//				else
//				{
//					offset = sizeof(InstancedData)*numInstancesInVB;
//					//D3D_V ( pInstancesVB->Lock(offset,sizeof(InstancedData)*numInstancesInArray,(void**)&pInstances, D3DLOCK_NOOVERWRITE ) );
//					pInstances = (InstancedData*) pInstancesVB->Lock( numInstancesInVB, numInstancesInArray );
//
//					numInstancesInVB += numInstancesInArray;
//				}
//				r3d_assert ( pInstances );
//				memcpy(pInstances, InstanceArray, numInstancesInArray*sizeof(InstancedData));
//				//D3D_V ( pInstancesVB->Unlock() );
//				pInstancesVB->Unlock();
//			}
//			buffers.Set();
//
//			r3dRenderer->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | numInstancesInArray));
//			r3dRenderer->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
//
//			pInstancesVB->Set( 1, offset );
//
//			if(NumMatChunks)
//			{
//				for (int i=0;i<NumMatChunks;i++)
//				{
//					MatChunks[i].Mat->Start( R3D_MATVS_INSTANCING, 0 );
//
//					r3dInstancedDraw idraw( numInstancesInArray );
//					r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0,buffers.minVertexIndex,NumVertices, buffers.startIndex + MatChunks[i].StartIndex,(MatChunks[i].EndIndex-MatChunks[i].StartIndex)/3);
//
//					MatChunks[i].Mat->End();
//				}
//			}
//			else
//			{
//				r3dInstancedDraw idraw( numInstancesInArray );
//				r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0,buffers.minVertexIndex,NumVertices,buffers.startIndex,NumIndices/3);
//			}
//
//			r3dRenderer->SetStreamSourceFreq(0, 1);
//			r3dRenderer->SetStreamSourceFreq(1, 1);
//			d3dc._SetDecl(R3D_MESH_VERTEX::getDecl()); 
//
//		}
//
//		numInstancesInArray = 0;
//	}
//}

//-----------------------------------------------------------------------

/*static*/
void
MeshDeferredRenderable::Draw( Renderable* RThis, const r3dCamera& Cam )
{
	MeshDeferredRenderable * This = static_cast<MeshDeferredRenderable*>( RThis );
	r3dMesh* Mesh = This->Mesh;

	r3dColor color;

	color.SetPacked( This->Color );

	Mesh->DrawMeshStart( &color );

	Mesh->DrawMeshDeferredBatch( This->BatchIdx, 0 );

	Mesh->DrawMeshEnd();

}

//------------------------------------------------------------------------

void MeshDeferredHighlightRenderable::DoDraw( Renderable* RThis, float distance, const r3dCamera& Cam )
{
	D3DPERF_BeginEvent( 0, L"MeshDeferredHighlightRenderable::Draw" );

	MeshDeferredHighlightRenderable * This = static_cast<MeshDeferredHighlightRenderable*>( RThis );
	r3dMesh* Mesh = This->Mesh;

	r3dColor color( r3dColor::white );	

	if( This->DoExtrude )
	{
		r3dRenderer->SetVertexShader( VS_FILLGBUFFER_EXTRUDE_ID );
	}
	else
	{
		r3dRenderer->SetVertexShader( VS_FILLGBUFFER_ID );
	}

	float factor = r_highlight_thickness->GetFloat() * 64;

	r3dPoint3D normal( 0, 1, 0 );

	normal.Normalize();

	r3dColor highlightColor;
	highlightColor.SetPacked( This->Color );
#if 0
	SetFillGBufferConstantPixelShader( highlightColor, 1.0f, normal, 0.0f, 0.0f, r_highlight_glow->GetFloat(), 0.f );
#else
	r3dSetFwdColorPixelShader( highlightColor );

#endif

	if( !This->DoExtrude )
	{
		Mesh->DrawMeshSimple( 0 );
	}
	else
	{
		const float LINE_SATURATE = 2.0f;

		if( distance < LINE_SATURATE )
			factor *= distance / LINE_SATURATE;

		const float DSP = 0.0625f * 0.25f * factor;

		{
			float vsConst[ 4 ] = { DSP, 0.f, 0.f, 0 };
			// float3 vExtrudeScale            : register( c17 );
			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
			Mesh->DrawMeshSimple( 0 );
		}

		{
			float vsConst[ 4 ] = { -DSP, 0.f, 0.f, 0 };
			// float3 vExtrudeScale            : register( c17 );
			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
			Mesh->DrawMeshSimple( 0 );
		}

		{
			float vsConst[ 4 ] = { 0.0f, -DSP, 0.f, 0 };
			// float3 vExtrudeScale            : register( c17 );
			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
			Mesh->DrawMeshSimple( 0 );
		}

		{
			float vsConst[ 4 ] = { 0.0f, +DSP, 0.f, 0 };
			// float3 vExtrudeScale            : register( c17 );
			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
			Mesh->DrawMeshSimple( 0 );
		}
	}

	D3DPERF_EndEvent();
}

void MeshDeferredHighlightRenderable::Init( r3dMesh* mesh, DWORD color )
{
	DrawFunc = NULL;
	Mesh = mesh;
	Color = color;
}

//------------------------------------------------------------------------
/*static*/

void
MeshShadowRenderable::DrawSingleBatch( Renderable* RThis, const r3dCamera& Cam )
{
	MeshShadowRenderable* This = static_cast< MeshShadowRenderable* >( RThis );

	r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE] );
	//This->Mesh->DrawMeshWithoutMaterials();
	This->Mesh->DrawMeshShadows(); // call shadow, otherwise it will not update vdecl and shader properly
}

//------------------------------------------------------------------------
/*static*/

void
MeshShadowRenderable::Draw( Renderable* RThis, const r3dCamera& Cam )
{
	MeshShadowRenderable* This = static_cast< MeshShadowRenderable* >( RThis );

	This->Mesh->DrawMeshStart( 0 );

	This->Mesh->DrawMeshShadowsBatch( This->BatchIdx );

	This->Mesh->DrawMeshEnd();
}

//------------------------------------------------------------------------

void
r3dMesh::AppendShadowRenderables( RenderArray& oArr )
{
	if(!IsDrawable())
		return;

	if( HasAlphaTextures )
	{
		for (int i=0;i<NumMatChunks;i++)
		{
			MeshShadowRenderable rend;

			const r3dTriBatch& batch = MatChunks[ i ];

			rend.SubDrawFunc = MeshShadowRenderable::Draw;

			rend.BatchIdx		= i;
			rend.Mesh			= this;
			rend.SortValue		= (UINT64)batch.Mat->ID << 32 | buffers.VB.GetStamp();

			oArr.PushBack( rend );
		}
	}
	else
	{
		MeshShadowRenderable rend;

		rend.SubDrawFunc = MeshShadowRenderable::DrawSingleBatch;

		rend.Mesh		= this;
		rend.SortValue	= (UINT64) buffers.VB.GetStamp() << 32;

		oArr.PushBack( rend );
	}
}


//------------------------------------------------------------------------
bool r3dMesh::AppendRenderablesDeferredInstanced( RenderArray& oArr, const r3dColor& color, const D3DXQUATERNION& quat, const r3dPoint3D& pos, const r3dPoint3D& scale)
{
	r3d_assert(numInstances < InstancingMeshVBCache::MaxInstancesToDraw);

	bool firstInstance = instanceIdInCurrentBuffer == -1;

	if (firstInstance)
	{
		MeshDeferredRenderableInstancing rend;
		rend.Mesh			= this;
		oArr.PushBack( rend );
		instanceIdInCurrentBuffer = oArr.Count() - 1;
	}

	if(!InstancesVB)
	{
		r3d_assert ( sizeof(R3D_MESH_VERTEX) == D3DXGetDeclVertexSize( R3D_MESH_VERTEX::VBDecl, 0 ) );
		r3d_assert ( sizeof(InstancedData) == D3DXGetDeclVertexSize( r3dInstancedDataVertex::VBDecl, 1 ) );

		InstancesVB = gInstancingMeshVBCache.GetVBRange(numInstances);
	}

	InstancedData * pInstancesVBPtr = InstancesVB->InstancesVBPtr + numInstancesInVB;


	r3d_assert(pInstancesVBPtr);
	pInstancesVBPtr->prt[0] = quat.x;
	pInstancesVBPtr->prt[1] = quat.y;
	pInstancesVBPtr->prt[2] = quat.z;
	pInstancesVBPtr->prt[3] = quat.w;

	pInstancesVBPtr->prt[4] = pos.x;
	pInstancesVBPtr->prt[5] = pos.y;
	pInstancesVBPtr->prt[6] = pos.z;

	pInstancesVBPtr->prt[7] = scale.x;
	pInstancesVBPtr->prt[8] = scale.y;
	pInstancesVBPtr->prt[9] = scale.z;

	pInstancesVBPtr->color = color.GetPacked();

	numInstancesInVB++;

	if (numInstancesInVB == numInstances)
	{
		gInstancingMeshVBCache.UnlockVBRange(InstancesVB);
		pInstancesVBPtr = 0;
	}

	return firstInstance;
}

//------------------------------------------------------------------------

void r3dMesh::AppendRenderablesDeferred( RenderArray& oArr, const r3dColor& color, float selfIllumMultiplier )
{
	if(!IsDrawable())
		return;

	
	r3dColor col = color;

	float a = selfIllumMultiplier;
	a *= 255.0f;

	if (a < 0.0f)
		a = 0;
	if (a > 255.0f)
		a = 255.0f;

	col.A = BYTE(a);

	const DWORD premultColor = col.GetPacked();

	for (int i=0;i<NumMatChunks;i++)
	{
		const r3dTriBatch& batch = MatChunks[ i ];
		if ((batch.Mat->Flags & R3D_MAT_TRANSPARENT) == 0)
		{
			MeshDeferredRenderable rend;

			rend.BatchIdx		= i;
			rend.Color			= premultColor;
			rend.Mesh			= this;
			rend.SortValue		= (UINT64)batch.Mat->ID << 32 | (UINT64) buffers.VBId << 16;

			oArr.PushBack( rend );
		}

	}	
}

//------------------------------------------------------------------------

void r3dMesh::AppendTransparentRenderables( RenderArray& oArr, const r3dColor& color, float dist, int forceAll )
{
	if (!IsDrawable())
		return;

	int idist = GetRevIDist(dist);

	for (int i = 0; i < NumMatChunks; ++i)
	{
		const r3dTriBatch& batch = MatChunks[ i ];
		if ( ( batch.Mat->Flags & R3D_MAT_TRANSPARENT ) || forceAll )
		{
			MeshDeferredRenderable rend;

			rend.BatchIdx		= i;
			rend.Color			= color.GetPacked();
			rend.Mesh			= this;
			rend.SortValue		= RENDERABLE_EMITTER_USER_SORT_VALUE | idist;

			oArr.PushBack( rend );
		}

	}	
}

void r3dMesh::AppendTransparentRenderablesColors( RenderArray& oArr, float dist, int forceAll )  // FakeGlass
{
	if (!IsDrawable())
		return;

	int idist = GetRevIDist(dist);

	for (int i = 0; i < NumMatChunks; ++i)
	{
		const r3dTriBatch& batch = MatChunks[ i ];
		if ( ( batch.Mat->Flags & R3D_MAT_TRANSPARENT ) || forceAll )
		{
			MeshDeferredRenderable rend;

			rend.BatchIdx		= i;
			rend.Color			= this->MatChunks[i].Mat->FGlassColor.GetPacked();
			rend.Mesh			= this;
			rend.SortValue		= RENDERABLE_EMITTER_USER_SORT_VALUE | idist;

			oArr.PushBack( rend );
		}

	}	
}

//------------------------------------------------------------------------

void r3dMesh::DrawMeshStart( const r3dColor* objectColor )
{
	// set per object color
	if( objectColor )
	{
		r3dMaterial::SetRenderedObjectColor( *objectColor ) ;
	}

	buffers.Set();

	if(!pWeights) 
	{
		if( VertexFlags & vfPrecise )
		{
			d3dc._SetDecl( R3D_MESH_PRECISE_VERTEX::getDecl() );
		}
		else
		if( VertexFlags & vfBending )
		{
			d3dc._SetDecl( R3D_BENDING_MESH_VERTEX::getDecl() ) ;
		}
		else
		if( VertexFlags & vfHaveSecondUV )
		{
			d3dc._SetDecl( R3D_2UV_MESH_VERTEX::getDecl() );
		}
		else
		{
			d3dc._SetDecl( R3D_MESH_VERTEX::getDecl() );
		}
	} 
	else 
	{
		float vConst[ 8 ] = {
						unpackScale.x, unpackScale.y, unpackScale.z, 0.f,
						texcUnpackScale.x, texcUnpackScale.y, ExtrudeAmmount, 0.f
					};

		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 22, vConst, 2 ) );

		d3dc._SetDecl( r3dSkinnedMeshVertex::getDecl() );
	}
#ifndef FINAL_BUILD
	densityVisualizer->SetTexelDensityStreamAndVDecl();
#endif
}

//------------------------------------------------------------------------

void r3dMesh::DrawMeshEnd()
{
	d3dc._SetDecl(R3D_MESH_VERTEX::getDecl());

	numInstances = 0;
}

//------------------------------------------------------------------------

void r3dMesh::DrawMeshDeferredBatch( int batchIdx, UINT MatFlags )
{
	const r3dTriBatch& batch = MatChunks[ batchIdx ];

	if (batch.Mat->Flags & R3D_MAT_SKIP_DRAW)
		return;

	r3dMatVSType matVs = R3D_MATVS_DEFAULT;
	if(pWeights) matVs = R3D_MATVS_SKIN;
	else if(VertexFlags & vfHaveSecondUV) matVs = R3D_MATVS_DECALMASK;
	batch.Mat->Start( matVs , MatFlags );

	int numTris = ( batch.EndIndex-batch.StartIndex ) / 3 ;

#ifndef FINAL_BUILD
	if( r_cut_meshes->GetInt() )
	{
		numTris = R3D_MIN( numTris, r_cut_meshes->GetInt() ) ;
	}

	densityVisualizer->SetShaderConstants();
	densityVisualizer->SetShaders();

#endif
	r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0, buffers.minVertexIndex, NumVertices, buffers.startIndex + batch.StartIndex, numTris );

	batch.Mat->End();

}

//------------------------------------------------------------------------

void r3dMesh::DrawMeshDeferred( const r3dColor& objectColor, UINT MatFlags )
{
	if(!IsDrawable())
		return;

	DrawMeshStart( &objectColor );

	for (int i=0;i<NumMatChunks;i++)
	{
		DrawMeshDeferredBatch( i, MatFlags );
	}

	DrawMeshEnd();

}

//------------------------------------------------------------------------

void
r3dMesh::DrawMeshWithoutMaterials()
{
	if(!IsDrawable())
		return;

	DrawMeshStart( 0 );

	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, buffers.minVertexIndex, NumVertices, buffers.startIndex, NumIndices/3 );

	DrawMeshEnd();
}


//------------------------------------------------------------------------

void r3dMesh::DrawMeshShadowsBatch( int batchIdx )
{
	const r3dTriBatch& batch = MatChunks[ batchIdx ];

	r3dMaterial::SetShadowVertexShader( pWeights ? R3D_MATVS_SKIN : R3D_MATVS_DEFAULT );

	if (batch.Mat->Texture)
		r3dRenderer->SetThumbTex( batch.Mat->Texture );
	else
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE]);

	UINT startIndex = buffers.startIndex + batch.StartIndex;
	UINT primCount = (batch.EndIndex-batch.StartIndex)/3;

#if 0
	if( r3dRenderer->DoubleDepthShadowPath )
	{
		r3dRenderer->SetCullMode( D3DCULL_CW );
		r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST,0,buffers.minVertexIndex,NumVertices,startIndex,primCount );
		
		r3dRenderer->SetCullMode( D3DCULL_CCW );
		r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST,0,buffers.minVertexIndex,NumVertices,startIndex,primCount );
	}
	else
#else
	if( r3dRenderer->DoubleDepthShadowPath )
	{
		r3dRenderer->SetCullMode( D3DCULL_CW );
		r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
#endif
	{

		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST,0,buffers.minVertexIndex,NumVertices,startIndex,primCount );
	}
}

//-----------------------------------------------------------------------
void r3dMesh::DrawMeshSimple( int bTextured )
{
	if(!IsDrawable())
		return;

	DrawMeshStart( 0 );

	if ( bTextured ) 
	{
		for (int i=0;i<NumMatChunks;i++)
		{
			const r3dTriBatch& batch = MatChunks[ i ];

			if (batch.Mat->Texture)
				r3dRenderer->SetThumbTex( batch.Mat->Texture );
			else
				r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE]);

			r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0, buffers.minVertexIndex,NumVertices,buffers.startIndex + batch.StartIndex,(batch.EndIndex-batch.StartIndex)/3);
		}
	}
	else
	{
		r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0, buffers.minVertexIndex, NumVertices, buffers.startIndex, NumIndices/3);
	}

	DrawMeshEnd();
}

void r3dMesh::DrawMeshShadows()
{
	if(!IsDrawable())
		return;

	DrawMeshStart( 0 );

	if( HasAlphaTextures )
	{
		for (int i=0;i<NumMatChunks;i++)
		{
			DrawMeshShadowsBatch( i );
		}
	}
	else
	{
		r3dMaterial::SetShadowVertexShader( pWeights ? R3D_MATVS_SKIN : R3D_MATVS_DEFAULT );
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE]);

		if( r3dRenderer->DoubleDepthShadowPath )
		{
			if( Flags & obfPlayerMesh )
			{
				r3dRenderer->SetCullMode( D3DCULL_CCW );
				r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0, buffers.minVertexIndex, NumVertices, buffers.startIndex, NumIndices/3);
			}
			else
			{
#ifndef FINAL_BUILD
				r3dRenderer->Stats.AddNumDoubleShadowDips( 1 );
#endif

				r3dRenderer->SetCullMode( D3DCULL_CW );
				r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0, buffers.minVertexIndex, NumVertices, buffers.startIndex, NumIndices/3);

				r3dRenderer->SetCullMode( D3DCULL_CCW );
				r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0, buffers.minVertexIndex, NumVertices, buffers.startIndex, NumIndices/3);
			}
		}
		else
		{
			r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST,0, buffers.minVertexIndex, NumVertices, buffers.startIndex, NumIndices/3);
		}
	}

	DrawMeshEnd();
}

void r3dMesh::DrawMeshStartInstances ()
{
	if(!IsDrawable())
		return;

	r3d_assert ( pWeights == NULL );
	r3d_assert ( !(VertexFlags & vfPrecise ) );

	// Set up the geometry data stream
	buffers.Set();
}

void r3dMesh::DrawMeshEndInstances ()
{
	if(!IsDrawable())
		return;

	r3dRenderer->SetStreamSourceFreq(0,1);
	r3dRenderer->SetStreamSourceFreq(1,1);
}

void r3dMesh::DrawMeshSimpleInstances ( int instanceCount )
{
	if(!IsDrawable())
		return;

	r3dInstancedDraw idraw( instanceCount );
	r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST, 0, buffers.minVertexIndex, NumVertices, buffers.startIndex, NumIndices/3);

}

#define TRIRAYCOLL_EPSILON	0.000001

#define CROSS(dest,v1,v2) \
	dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
	dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
	dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) \
	(v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
	dest[0]=v1[0]-v2[0]; \
	dest[1]=v1[1]-v2[1]; \
	dest[2]=v1[2]-v2[2]; 

int intersect_triangle_floats_cull(float orig[3], float dir[3],
								   float vert0[3], float vert1[3], float vert2[3],
								   float *t, float *u, float *v)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det;

	/* find vectors for two edges sharing vert0 */
	SUB(edge1, vert1, vert0);
	SUB(edge2, vert2, vert0);

	/* begin calculating determinant - also used to calculate U parameter */
	CROSS(pvec, dir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = DOT(edge1, pvec);

	if(det < TRIRAYCOLL_EPSILON)
		return 0;

	/* calculate distance from vert0 to ray origin */
	SUB(tvec, orig, vert0);

	/* calculate U parameter and test bounds */
	*u = DOT(tvec, pvec);
	if(*u < 0.0f || *u > det)
		return 0;

	/* prepare to test V parameter */
	CROSS(qvec, tvec, edge1);

	/* calculate V parameter and test bounds */
	*v = DOT(dir, qvec);
	if(*v < 0.0f || *u + *v > det)
		return 0;

	/* calculate t, scale parameters, ray intersects triangle */
	*t = DOT(edge2, qvec);
	// Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
	if(*t < 0.0f) 
		return 0;

	inv_det = 1.0f / det;
	*t *= inv_det;
	*u *= inv_det;
	*v *= inv_det;
	return 1;
}

int intersect_triangle_floats_nocull(float orig[3], float dir[3],
									 float vert0[3], float vert1[3], float vert2[3],
									 float *t, float *u, float *v)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det;

	/* find vectors for two edges sharing vert0 */
	SUB(edge1, vert1, vert0);
	SUB(edge2, vert2, vert0);

	/* begin calculating determinant - also used to calculate U parameter */
	CROSS(pvec, dir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = DOT(edge1, pvec);

	if(det > -TRIRAYCOLL_EPSILON && det < TRIRAYCOLL_EPSILON)
		return 0;
	inv_det = 1.0f / det;

	/* calculate distance from vert0 to ray origin */
	SUB(tvec, orig, vert0);

	/* calculate U parameter and test bounds */
	*u = DOT(tvec, pvec) * inv_det;
	if(*u < 0.0 || *u > 1.0)
		return 0;

	/* prepare to test V parameter */
	CROSS(qvec, tvec, edge1);

	/* calculate V parameter and test bounds */
	*v = DOT(dir, qvec) * inv_det;
	if(*v < 0.0 || *u + *v > 1.0)
		return 0;

	/* calculate t, ray intersects triangle */
	*t = DOT(edge2, qvec) * inv_det;
	// Intersection point is valid if distance is positive (else it can just be a face behind the orig point)
	if(*t < 0.0f) 
		return 0;

	return 1;
}

#undef SUB
#undef DOT
#undef CROSS

BOOL r3dMesh::ContainsRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float RayLen, float *ClipDist, r3dMaterial **material, const r3dVector& pos, const D3DXMATRIX& rotation, int * OutMinFace /*= NULL*/ )
{
	if(!NumVertices) return FALSE;

	R3DPROFILE_FUNCTION( "r3dMesh::ContainsRay" ) ;

	float 	MinClipDist = 9999999;
	int		MinFace     = -1;
	*material = 0;

	//NOTE: add caching of world matrix after changing RotateMatrix

	D3DXMATRIX  mWorld;
	mesh_CreateWorldMatrix(mWorld, vPivot, pos, r3dPoint3D(1, 1, 1), rotation);

	D3DXMATRIX mwI, mrI;
	D3DXMatrixInverse(&mwI, NULL, &mWorld);
	D3DXMatrixInverse(&mrI, NULL, &rotation);

	// transform ray to object local space
	r3dPoint3D vRay1;
	r3dPoint3D vStart1;
	D3DXVec3TransformNormal(vRay1.d3dx(),  vRay.d3dx(),   &mrI);
	D3DXVec3TransformCoord(vStart1.d3dx(), vStart.d3dx(), &mwI);

	for(int i=0; i<NumIndices/3; i++) 
	{
		float t, u, v;   
		if(!intersect_triangle_floats_cull((float *)vStart1, (float *)vRay1, (float *)&VertexPositions[Indices[i*3+0]], (float *)&VertexPositions[Indices[i*3+1]], (float *)&VertexPositions[Indices[i*3+2]], &t, &u, &v)) 
			continue;

		/*
		if(t<0) {
		r3dOutToLog("r3dPoint3D vStart(%f, %f, %f)\n", vStart1.X, vStart1.Y, vStart1.Z);
		r3dOutToLog("r3dPoint3D vRay1(%f, %f, %f)\n", vRay1.X, vRay1.Y, vRay1.Z);
		r3dOutToLog("r3dPoint3D f.Vertices[0](%f, %f, %f)\n", f.Vertices[0].X, f.Vertices[0].Y, f.Vertices[0].Z);
		r3dOutToLog("r3dPoint3D f.Vertices[1](%f, %f, %f)\n", f.Vertices[1].X, f.Vertices[1].Y, f.Vertices[1].Z);
		r3dOutToLog("r3dPoint3D f.Vertices[2](%f, %f, %f)\n", f.Vertices[2].X, f.Vertices[2].Y, f.Vertices[2].Z);
		intersect_triangle_floats_cull((float *)vStart1, (float *)vRay1, (float *)*f.Vertices[0], (float *)*f.Vertices[1], (float *)*f.Vertices[2], &t, &u, &v);
		intersect_triangle_floats_nocull((float *)vStart1, (float *)vRay1, (float *)*f.Vertices[0], (float *)*f.Vertices[1], (float *)*f.Vertices[2], &t, &u, &v);
		}

		extern void U_DLog(const char* , ...);
		U_DLog("C %s %f\n", Name, t);
		*/    
		if(t < MinClipDist) 
		{
			if( r3dMaterial* mat = GetFaceMaterial( i ) )
			{
				*material = mat;
			}

			MinFace     = i;
			MinClipDist = t;
		}
	}

	if(MinFace != -1) 
	{
		*ClipDist = MinClipDist;

		if( OutMinFace )
			*OutMinFace = MinFace;

		return TRUE;
	}

	return FALSE;
}


BOOL r3dMesh::ContainsQuickRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float RayLen, float *ClipDist, r3dMaterial **material, const r3dVector& pos, const D3DXMATRIX& rotation)
{
	if(!NumVertices) return FALSE;

	float 	MinClipDist = 9999999;
	int		MinFace     = -1;
	*material = 0;

	//NOTE: add caching of world matrix after changing RotateMatrix

	D3DXMATRIX  mWorld;
	mesh_CreateWorldMatrix(mWorld, vPivot, pos, r3dPoint3D(1, 1, 1), rotation);

	D3DXMATRIX mwI, mrI;
	D3DXMatrixInverse(&mwI, NULL, &mWorld);
	D3DXMatrixInverse(&mrI, NULL, &rotation);

	// transform ray to object local space
	r3dPoint3D vRay1;
	r3dPoint3D vStart1;
	D3DXVec3TransformNormal(vRay1.d3dx(),  vRay.d3dx(),   &mrI);
	D3DXVec3TransformCoord(vStart1.d3dx(), vStart.d3dx(), &mwI);

	for(int i=0; i<NumIndices/3; i++) 
	{
		float t, u, v;   
		if(intersect_triangle_floats_cull((float *)vStart1, (float *)vRay1, (float *)&VertexPositions[Indices[i*3+0]], (float *)&VertexPositions[Indices[i*3+1]], (float *)&VertexPositions[Indices[i*3+2]], &t, &u, &v)) 
		{
			*ClipDist = t;
			// look for material
			for(int m=0; m<NumMatChunks; ++m)
			{
				if( ((i*3) >= MatChunks[m].StartIndex) && ((i*3) <= MatChunks[m].EndIndex))
				{
					*material = MatChunks[m].Mat;
					break;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

/*static*/
void
r3dMesh::FlushLoadingBuffers()
{
	PreTLOptimize<R3D_MESH_PRECISE_VERTEX>( 0, 0, 0, 0, 0, 0 ) ;
	PreTLOptimize<R3D_BENDING_MESH_VERTEX>( 0, 0, 0, 0, 0, 0 ) ;
	PreTLOptimize<R3D_MESH_VERTEX>( 0, 0, 0, 0, 0, 0 ) ;
	PreTLOptimize<r3dSkinnedMeshVertex>( 0, 0, 0, 0, 0, 0 ) ;

	r3dgfxVector(int)().swap( tempIndexBuffer0 ) ;
	r3dgfxVector(int)().swap( tempIndexBuffer1 ) ;
	r3dgfxVector(int)().swap( remapIndices ) ;

	free( tempGenericMem ) ;

	tempGenericMem = 0 ;
	tempGenericMemSize = 0 ;
}


//static 
void r3dMesh::MeshLoaded ( const char* sMesh )
{
	if ( g_pMeshLoadCallbacker )
		g_pMeshLoadCallbacker (sMesh);	
}

//static 
void r3dMesh::MeshUnloaded ( const char* sMesh )
{
	if ( g_pMeshUnloadCallbacker )
		g_pMeshUnloadCallbacker (sMesh);	
}

static int IsMatrixTheSame(D3DXMATRIX &m1, D3DXMATRIX &m2)
{
	for(int i=0; i<16; i++) {
		if(fabs(m1[i] - m2[i]) > 0.0001f)
			return 0;
	}

	return 1;
}

void r3dMesh::AllocateWeights()
{
	r3d_assert( !(VertexFlags & vfPrecise) );
	r3d_assert(pWeights == NULL);
	pWeights     = gfx_new r3dWeight[NumVertices];
}


void r3dMesh::TryLoadWeights( const char* baseFileName )
{
	// first, try .wgt
	char weightName[256];

	ToSkinFileName( weightName, baseFileName ) ;

	if(r3d_access(weightName, 0) == 0) 
		LoadWeights( weightName );
}

void r3dMesh::LoadWeights(const char* fname)
{
	AllocateWeights();

	for(int i=0;i<NumVertices;i++) {
		pWeights[i].BoneID[0] = 0;
		pWeights[i].Weight[0] = 0;
		pWeights[i].BoneID[1] = 0;
		pWeights[i].Weight[1] = 0;
		pWeights[i].BoneID[2] = 0;
		pWeights[i].Weight[2] = 0;
		pWeights[i].BoneID[3] = 0;
		pWeights[i].Weight[3] = 0;
	}

	// binary format for version 0001
#define R3D_WEIGHTS_BINARY_ID	'thgw'
#define R3D_WEIGHTS_BINARY_VER	0x00000001
	struct binhdr_s
	{
		R3D_DEFAULT_BINARY_HDR;
	};

	r3dFile *f = r3d_open(fname, "rb");
	if(!f) r3dError("can't open %s\n", fname);

	binhdr_s hdr;
	if(fread(&hdr, sizeof(hdr), 1, f) != 1) r3dError("%s - can't read header from", fname);
	if(hdr.r3dID != R3D_BINARY_FILE_ID)     r3dError("%s - invalid header", fname);
	if(hdr.ID != R3D_WEIGHTS_BINARY_ID)	  r3dError("%s - not an animation file", fname);

	switch(hdr.Version) {
		default:
			r3dError("%s - unsupported version", fname);
			break;

		case 0x00000001:
			LoadWeights_BinaryV1(f, false);
			break;
	}

	fclose(f);
	return;
}

void r3dMesh::LoadWeights_BinaryV1(r3dFile* f, bool from_scb)
{
	DWORD dw1, dw2;
	fread(&dw1, sizeof(DWORD), 1, f);
	fread(&dw2, sizeof(DWORD), 1, f);

	//DWORD dwSkeletonId = dw1;
	//if(pBindSkeleton->dwSkeletonId != dwSkeletonId) 
	//  r3dError("r3dSkinMesh::LoadWeights() - skeleton ID's is different\n");

	if(NumVertices != (int)dw2)
		r3dError("r3dMesh::LoadWeights() - sco/wgt files mismatch for %s\n", f->GetFileName());

	for(int i=0; i<NumVertices; i++) 
	{
		byte  iBoneId[4] = {0, 0, 0, 0};
		float fWeight[4] = {0, 0, 0, 0};

		fread(&iBoneId[0], sizeof(byte),  4, f);
		fread(&fWeight[0], sizeof(float), 4, f);
		
		r3dWeight& w = pWeights[i];

		float ws = 0.0f;
		for(int k=0; k<4; k++) 
		{
			r3d_assert(fWeight[k] >= 0.f && fWeight[k] <= 1.f);
			w.Weight[k] = fWeight[k];
			w.BoneID[k] = iBoneId[k];

			ws += w.Weight[k];
		}

		if(ws > 0.0f) 
		{
			// renormalize weights, because some Maya exporter dude screwed it up.
			w.Weight[0] /= ws; 
			w.Weight[1] /= ws; 
			w.Weight[2] /= ws; 
			w.Weight[3] /= ws; 
		}  
			
	}

	return;
}

bool r3dMesh::SaveWeights_BinaryV1( FILE* f )
{
	if( !pWeights )
	{
		r3dOutToLog("r3dMesh::SaveWeightsBinary: requested to save mesh %s, but it has no weights!\n", this->FileName.c_str() ) ;
		return false ;
	}

	DWORD	dw1 = 0,				// supposed to be skeleton id but now is prolly obsolete
			dw2 = NumVertices ;

	fwrite( &dw1, sizeof(DWORD), 1, f );
	fwrite( &dw2, sizeof(DWORD), 1, f );


	for(int i=0; i<NumVertices; i++) 
	{
		float ws = 0.0f;

		byte  iBoneId[4] ;
		float fWeight[4] ;

		for( int k=0; k<4; k++ )
		{
			fWeight[k] = pWeights[i].Weight[k] ;
			iBoneId[k] = pWeights[i].BoneID[k] ;
		}

		fwrite(&iBoneId[0], sizeof(byte),  4, f);
		fwrite(&fWeight[0], sizeof(float), 4, f);
	}

	return true ;
}

//------------------------------------------------------------------------

void r3dMesh::DeleteWeights()
{
	delete [] pWeights ;
	pWeights = 0 ;
}

//------------------------------------------------------------------------

void r3dMesh::ResetInstancingInfo()
{
	numInstancesInVB = 0;
	if (InstancesVB)
	{
		gInstancingMeshVBCache.PutVBRange(InstancesVB);
		InstancesVB = 0;
	}

	instanceIdInCurrentBuffer = -1;
	numInstances = 0;
}

