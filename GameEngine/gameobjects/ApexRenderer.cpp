//=========================================================================
//	Module: ApexRenderer.cpp
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if APEX_ENABLED

#include "ApexWorld.h"
#include "ApexRenderer.h"
#include "../../src/Eternity/SF/RenderBuffer.h"

//-------------------------------------------------------------------------
//	Support classes
//-------------------------------------------------------------------------

//	Maximum bones allowed for material rendering. This is hardware skinning constraint.
#define MAX_BONES_ALLOWED 60
//	Size in Vector 4 values
#define BONE_MATRIX_SIZE 3

UserRenderer apexUserRenderer;

//////////////////////////////////////////////////////////////////////////

namespace
{
	template<typename T, int N>
	void CopyValues(const void *src, void *dst)
	{
		const T *s = reinterpret_cast<const T*>(src);
		T *d = reinterpret_cast<T*>(dst);
		for (int i = 0; i < N; ++i)
			d[i] = s[i];
	}

	typedef void (*CopyFunc)(const void*, void*);
	const CopyFunc CopyTableArray[] = 
	{
		0,

		// Bytes
		&CopyValues<byte, 1>,
		&CopyValues<byte, 2>,
		&CopyValues<byte, 3>,
		&CopyValues<byte, 4>,

		//	Ushorts
		&CopyValues<unsigned short, 1>,
		&CopyValues<unsigned short, 2>,
		&CopyValues<unsigned short, 3>,
		&CopyValues<unsigned short, 4>,

		//	shorts
		&CopyValues<short, 1>,
		&CopyValues<short, 2>,
		&CopyValues<short, 3>,
		&CopyValues<short, 4>,

		//	uint
		&CopyValues<unsigned int, 1>,
		&CopyValues<unsigned int, 2>,
		&CopyValues<unsigned int, 3>,
		&CopyValues<unsigned int, 4>,

		// R8G8B8A8
		&CopyValues<byte, 4>,
		// B8G8R8A8
		&CopyValues<byte, 4>,
		// R32G32B32A32_FLOAT
		&CopyValues<float, 4>,
		// B32G32R32A32_FLOAT
		&CopyValues<float, 4>,

		// BYTE_UNORM
		&CopyValues<byte, 1>,
		&CopyValues<byte, 2>,
		&CopyValues<byte, 3>,
		&CopyValues<byte, 4>,

		// SHORT_UNORM
		&CopyValues<short, 1>,
		&CopyValues<short, 2>,
		&CopyValues<short, 3>,
		&CopyValues<short, 4>,

		// BYTE_SNORM
		&CopyValues<byte, 1>,
		&CopyValues<byte, 2>,
		&CopyValues<byte, 3>,
		&CopyValues<byte, 4>,

		// SHORT_SNORM
		&CopyValues<short, 1>,
		&CopyValues<short, 2>,
		&CopyValues<short, 3>,
		&CopyValues<short, 4>,

		// HALF
		&CopyValues<short, 1>,
		&CopyValues<short, 2>,
		&CopyValues<short, 3>,
		&CopyValues<short, 4>,

		// FLOAT
		&CopyValues<float, 1>,
		&CopyValues<float, 2>,
		&CopyValues<float, 3>,
		&CopyValues<float, 4>,

		// FLOAT3x4
		&CopyValues<float, 12>,
		// FLOAT4x4
		&CopyValues<float, 16>,

		// FLOAT4_QUAT
		&CopyValues<float, 4>,
		// BYTE_SNORM4_QUATXYZW
		&CopyValues<byte, 4>,
		// SHORT_SNORM4_QUATXYZW
		&CopyValues<short, 4>
	};

//////////////////////////////////////////////////////////////////////////

	class UserRenderVertexBuffer: public NxUserRenderVertexBuffer
	{
		friend class UserRenderer;

		r3dVertexBuffer *vb;
		D3DVERTEXELEMENT9 vertexDesc[NxRenderVertexSemantic::NUM_SEMANTICS];
		size_t numberOfVertexElements;

	public:
		explicit UserRenderVertexBuffer(const NxUserRenderVertexBufferDesc &desc)
		: vb(0)
		, numberOfVertexElements(0)
		{
			size_t vertexSize = CreateVertexDescription(desc);
			vb = gfx_new r3dVertexBuffer(desc.maxVerts, vertexSize, 0, desc.hint == NxRenderBufferHint::DYNAMIC);
		}

		~UserRenderVertexBuffer()
		{
			delete vb;
		}

		void writeBuffer(const NxApexRenderVertexBufferData& data, physx::PxU32 firstVertex, physx::PxU32 numVertices)
		{
			if (!vb) return;
			char *vbMem = reinterpret_cast<char*>(vb->Lock(firstVertex, numVertices));
			uint32_t vertexSize = vb->GetItemSize();
			uint32_t elementIdx = 0;

			for (int i = 0; i < NxRenderVertexSemantic::NUM_SEMANTICS; ++i)
			{
				const NxApexRenderSemanticData &sd = data.getSemanticData(static_cast<NxRenderVertexSemantic::Enum>(i));
				if (!sd.data)
					continue;

				r3d_assert(static_cast<int>(sd.format) < _countof(CopyTableArray));
				if (static_cast<int>(sd.format) >= _countof(CopyTableArray))
					continue;

				const char *charData = reinterpret_cast<const char*>(sd.data);
				uint32_t elementOffset = vertexDesc[elementIdx++].Offset;

				if (numberOfVertexElements == 1 && sd.stride == vertexSize)
				{
					//	This is only vertex buffer data, so we can fast memcpy it
					memcpy_s(vbMem, numVertices * vertexSize, charData + firstVertex * sd.stride, sd.stride * numVertices);
				}
				else
				{
					for (uint32_t j = firstVertex; j < numVertices; ++j)
					{
						CopyFunc f = CopyTableArray[sd.format];
						if (f)
						{
							f(charData + j * sd.stride, vbMem + j * vertexSize + elementOffset);
						}
					}
				}
			}
			
			vb->Unlock();
		}
	
		/**	Create declaration and return vertex size. */
		UINT CreateVertexDescription(const NxUserRenderVertexBufferDesc &desc)
		{
			//	Apex types to d3d types conversion table
			static const BYTE type2type[NxRenderDataFormat::SHORT_SNORM4_QUATXYZW + 1] = 
			{
				D3DDECLTYPE_UNUSED,
				//	UBYTES
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UBYTE4,
				//	USHORTS
				D3DDECLTYPE_SHORT2, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,
				//	SHORTS
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_SHORT2, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,
				//	UINTS
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,
				//	Color
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_D3DCOLOR, D3DDECLTYPE_FLOAT4, D3DDECLTYPE_FLOAT4,
				//	UBYTES Normalized
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UBYTE4N,
				//	USHORTS Normalized
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_USHORT2N, D3DDECLTYPE_UNUSED, D3DDECLTYPE_USHORT4N,
				//	BYTES Normalized
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,
				//	SHORTS Normalized
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_SHORT2N, D3DDECLTYPE_UNUSED, D3DDECLTYPE_SHORT4N,
				//	HALF
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_FLOAT16_2, D3DDECLTYPE_UNUSED, D3DDECLTYPE_FLOAT16_4,
				//	FLOAT
				D3DDECLTYPE_FLOAT1, D3DDECLTYPE_FLOAT2, D3DDECLTYPE_FLOAT3, D3DDECLTYPE_FLOAT4,
				//	OTHER
				D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED
			};

			//	Apex usage to d3d usage conversion table
			static const BYTE usage2usage[NxRenderVertexSemantic::NUM_SEMANTICS] = 
			{
				D3DDECLUSAGE_POSITION,
				D3DDECLUSAGE_NORMAL,
				D3DDECLUSAGE_TANGENT,
				D3DDECLUSAGE_BINORMAL,
				D3DDECLUSAGE_COLOR,
				D3DDECLUSAGE_TEXCOORD,
				D3DDECLUSAGE_TEXCOORD,
				D3DDECLUSAGE_TEXCOORD,
				D3DDECLUSAGE_TEXCOORD,
				D3DDECLUSAGE_BLENDINDICES,
				D3DDECLUSAGE_BLENDWEIGHT
			};

			//	Usage index conversion table
			static const BYTE usageIndexTable[NxRenderVertexSemantic::NUM_SEMANTICS] = 
			{
				0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0
			};

			//	Vertex elements offsets
			static const BYTE vertexElementsOffsets[D3DDECLTYPE_UNUSED] = 
			{
				4, 8, 12, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8
			};

			int c = 0;
			int offset = 0;
			for (int i = 0; i < NxRenderVertexSemantic::NUM_SEMANTICS; ++i)
			{
				if (desc.buffersRequest[i] == NxRenderDataFormat::UNSPECIFIED)
					continue;

				vertexDesc[c].Stream = 0;
				vertexDesc[c].Offset = offset;
				vertexDesc[c].Type = type2type[desc.buffersRequest[i]];
				vertexDesc[c].Method = D3DDECLMETHOD_DEFAULT;
				vertexDesc[c].Usage = usage2usage[i];
				vertexDesc[c].UsageIndex = usageIndexTable[i];
				offset += vertexElementsOffsets[vertexDesc[c].Type];
				++c;
			}

			D3DVERTEXELEMENT9 declTerminator = D3DDECL_END();
			vertexDesc[c] = declTerminator;
			numberOfVertexElements = c;

			return D3DXGetDeclVertexSize(&vertexDesc[0], 0);
		}
	};

//////////////////////////////////////////////////////////////////////////

	class UserRenderIndexBuffer: public NxUserRenderIndexBuffer
	{
		friend class UserRenderer;
		r3dIndexBuffer *ib;
	public:
		explicit UserRenderIndexBuffer(const NxUserRenderIndexBufferDesc& desc)
		: ib(0)
		{
			ib = gfx_new r3dIndexBuffer(desc.maxIndices, desc.hint == NxRenderBufferHint::DYNAMIC, desc.format == NxRenderDataFormat::UINT1 ? 4 : 2);
		}

		~UserRenderIndexBuffer()
		{
			delete ib;
		}

		void writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 numElements)
		{
			if (!ib) return;
			void *data = ib->Lock(firstDestElement, numElements);
			memcpy_s(data, ib->GetDataLength(), srcData, srcStride * numElements);
			ib->Unlock();
		}

	};

//////////////////////////////////////////////////////////////////////////

	class UserRenderBoneBuffer: public NxUserRenderBoneBuffer
	{
		friend class UserRenderer;

		D3DXVECTOR4 *boneMatrices;
		size_t numberOfBones;

	public:
		explicit UserRenderBoneBuffer(const NxUserRenderBoneBufferDesc &desc)
		: boneMatrices(0)
		, numberOfBones(0)
		{
			r3d_assert(desc.maxBones <= MAX_BONES_ALLOWED);
			r3d_assert(desc.buffersRequest[0] == NxRenderDataFormat::FLOAT3x4);

			size_t bufferSize = BONE_MATRIX_SIZE * desc.maxBones;
			boneMatrices = gfx_new D3DXVECTOR4[bufferSize];
			numberOfBones = desc.maxBones;
		}

		~UserRenderBoneBuffer()
		{
			delete [] boneMatrices;
		}

		void writeBuffer(const NxApexRenderBoneBufferData& data, physx::PxU32 firstBone, physx::PxU32 numBones)
		{
			if (!boneMatrices)
				return;

			r3d_assert(firstBone < MAX_BONES_ALLOWED);
			r3d_assert(numBones + firstBone <= MAX_BONES_ALLOWED);
			if (firstBone >= MAX_BONES_ALLOWED || numBones + firstBone > MAX_BONES_ALLOWED)
				return;

			const NxApexRenderSemanticData &rsd = data.getSemanticData(NxRenderBoneSemantic::POSE);
			r3d_assert(rsd.data);
			r3d_assert(rsd.stride == sizeof(D3DXVECTOR4) * BONE_MATRIX_SIZE);
			if (rsd.stride != sizeof(D3DXVECTOR4) * BONE_MATRIX_SIZE || !rsd.data)
				return;

			const float *floatData = reinterpret_cast<const float*>(rsd.data);
			uint32_t nextBoneOffset = rsd.stride / sizeof(float);
			//	Transpose matrices here
			for (uint32_t i = 0; i < numBones; ++i, floatData += nextBoneOffset)
			{
				D3DXVECTOR4 *v = &boneMatrices[i * BONE_MATRIX_SIZE];
				v[0].x = floatData[0];
				v[1].x = floatData[3];
				v[2].x = floatData[6];

				v[0].y = floatData[1];
				v[1].y = floatData[4];
				v[2].y = floatData[7];

				v[0].z = floatData[2];
				v[1].z = floatData[5];
				v[2].z = floatData[8];

				v[0].w = floatData[9];
				v[1].w = floatData[10];
				v[2].w = floatData[11];
			}
		}
	};

//////////////////////////////////////////////////////////////////////////

	class UserRenderResource: public NxUserRenderResource
	{
		friend class UserRenderer;
		uint32_t firstVertex;
		uint32_t numVerts;

		UserRenderIndexBuffer * indexBuffer;
		uint32_t firstIndex;
		uint32_t numIndices;

		UserRenderBoneBuffer * boneBuffer;
		uint32_t firstBone;
		uint32_t numBones;

		uint32_t  numVertexBuffers;
		UserRenderVertexBuffer * vertexBuffers[8];

		D3DCULL cullMode;
		D3DPRIMITIVETYPE primType;

		uint32_t numPrimitives;
		IDirect3DVertexDeclaration9 *meshVD;

		ApexActorBase *owner;
		r3dMaterial *mtl;

		D3DPRIMITIVETYPE GetD3DPrimitiveType(NxRenderPrimitiveType::Enum a) const
		{
			static const D3DPRIMITIVETYPE pts[] =
			{
				D3DPT_FORCE_DWORD,
				D3DPT_TRIANGLELIST,
				D3DPT_TRIANGLESTRIP,
				D3DPT_LINELIST,
				D3DPT_LINESTRIP,
				D3DPT_POINTLIST,
				D3DPT_POINTLIST
			};
			r3d_assert(a < _countof(pts));
			if (a >= _countof(pts))
				return D3DPT_FORCE_DWORD;

			return pts[a];
		}

		void CalcNumPrimitives(uint32_t numIndices)
		{
			switch (primType)
			{
			case D3DPT_TRIANGLELIST:
				numPrimitives = numIndices / 3;
				break;
			case D3DPT_TRIANGLESTRIP:
				numPrimitives = numIndices - 2;
				break;
			default:
				r3d_assert(!"Primitive type is unsupported");
			}
		}

	public:
		explicit UserRenderResource(const NxUserRenderResourceDesc &desc)
		: firstVertex(desc.firstVertex)
		, numVerts(desc.numVerts)
		, indexBuffer(static_cast<UserRenderIndexBuffer*>(desc.indexBuffer))
		, firstIndex(desc.firstIndex)
		, numIndices(desc.numIndices)
		, boneBuffer(static_cast<UserRenderBoneBuffer*>(desc.boneBuffer))
		, firstBone(desc.firstBone)
		, numBones(desc.numBones)
		, numVertexBuffers(desc.numVertexBuffers)
		, cullMode(D3DCULL_FORCE_DWORD)
		, primType(GetD3DPrimitiveType(desc.primitives))
		, meshVD(0)
		, owner(reinterpret_cast<ApexActorBase*>(desc.userRenderData))
		, mtl(0)
		{
			CalcNumPrimitives(numIndices);

			switch (desc.cullMode)
			{
			case NxRenderCullMode::CLOCKWISE:
				cullMode = D3DCULL_CW;
				break;
			case NxRenderCullMode::COUNTER_CLOCKWISE:
				cullMode = D3DCULL_CCW;
				break;
			default:
				r3d_assert(!"Unknown cull ordering");
			}

			r3d_assert(numVertexBuffers <= _countof(vertexBuffers));
			if (numVertexBuffers <= _countof(vertexBuffers))
			{
				for (uint32_t i = 0; i < numVertexBuffers; ++i)
				{
					vertexBuffers[i] = static_cast<UserRenderVertexBuffer*>(desc.vertexBuffers[i]);
				}
			}

			setMaterial(desc.material);
		}

		~UserRenderResource()
		{
		}
		
		virtual void setVertexBufferRange(physx::PxU32 firstVertex,   physx::PxU32 numVerts)
		{
			this->firstVertex = firstVertex;
			this->numVerts = numVerts;
		}

		virtual void setIndexBufferRange(physx::PxU32 firstIndex,    physx::PxU32 numIndices)
		{
			this->firstIndex = firstIndex;
			this->numIndices = numIndices;
			CalcNumPrimitives(numIndices);
		}

		virtual void setBoneBufferRange(physx::PxU32 firstBone,     physx::PxU32 numBones)
		{
			this->firstBone = firstBone;
			this->numBones = numBones;
		}

		virtual void setInstanceBufferRange(physx::PxU32 firstInstance, physx::PxU32 numInstances)
		{
		}

		virtual void setSpriteBufferRange(physx::PxU32 firstSprite,   physx::PxU32 numSprites)
		{
		}

		virtual void setMaterial(void* material)
		{
			mtl = reinterpret_cast<r3dMaterial*>(material);
			if (owner)
				owner->AddMaterial(mtl);
		}

		virtual physx::PxU32 getNbVertexBuffers() const
		{
			return numVertexBuffers;
		}

		virtual NxUserRenderVertexBuffer*   getVertexBuffer(physx::PxU32 index) const
		{
			return vertexBuffers[index];
		}

		virtual NxUserRenderIndexBuffer*    getIndexBuffer() const
		{
			return indexBuffer;
		}

		virtual NxUserRenderBoneBuffer*     getBoneBuffer()	const
		{
			return boneBuffer;
		}

		virtual NxUserRenderInstanceBuffer* getInstanceBuffer()	const
		{
			return 0;
		}

		virtual NxUserRenderSpriteBuffer*   getSpriteBuffer() const
		{
			return 0;
		}

	};

	/**	Vertex elements description comparator. */
	bool operator == (const D3DVERTEXELEMENT9 &l, const D3DVERTEXELEMENT9 &r)
	{
		return
			l.Method == r.Method &&
			l.Offset == r.Offset &&
			l.Stream == l.Stream &&
			l.Type == r.Type &&
			l.Usage == r.Usage &&
			l.UsageIndex == r.UsageIndex;
	}
}
//////////////////////////////////////////////////////////////////////////

NxUserRenderVertexBuffer * UserRenderResourceManager::createVertexBuffer(const NxUserRenderVertexBufferDesc& desc)
{
	return gfx_new UserRenderVertexBuffer(desc);
}

//////////////////////////////////////////////////////////////////////////

void UserRenderResourceManager::releaseVertexBuffer(NxUserRenderVertexBuffer& buffer)
{
	delete &buffer;
}

//////////////////////////////////////////////////////////////////////////

NxUserRenderIndexBuffer * UserRenderResourceManager::createIndexBuffer(const NxUserRenderIndexBufferDesc& desc)
{
	return gfx_new UserRenderIndexBuffer(desc);
}

//////////////////////////////////////////////////////////////////////////

void UserRenderResourceManager::releaseIndexBuffer(NxUserRenderIndexBuffer& buffer)
{
	delete &buffer;
}

//////////////////////////////////////////////////////////////////////////

NxUserRenderBoneBuffer * UserRenderResourceManager::createBoneBuffer(const NxUserRenderBoneBufferDesc& desc)
{
	return gfx_new UserRenderBoneBuffer(desc);
}

//////////////////////////////////////////////////////////////////////////

void UserRenderResourceManager::releaseBoneBuffer(NxUserRenderBoneBuffer& buffer)
{
	delete &buffer;
}

//////////////////////////////////////////////////////////////////////////

NxUserRenderInstanceBuffer * UserRenderResourceManager::createInstanceBuffer(const NxUserRenderInstanceBufferDesc& desc)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void UserRenderResourceManager::releaseInstanceBuffer(NxUserRenderInstanceBuffer& buffer)
{
	delete &buffer;
}

//////////////////////////////////////////////////////////////////////////

NxUserRenderSpriteBuffer * UserRenderResourceManager::createSpriteBuffer(const NxUserRenderSpriteBufferDesc& desc)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void UserRenderResourceManager::releaseSpriteBuffer(NxUserRenderSpriteBuffer& buffer)
{
	delete &buffer;
}

//////////////////////////////////////////////////////////////////////////

NxUserRenderResource * UserRenderResourceManager::createResource(const NxUserRenderResourceDesc& desc)
{
	return gfx_new UserRenderResource(desc);
}

//////////////////////////////////////////////////////////////////////////

void UserRenderResourceManager::releaseResource(NxUserRenderResource& resource)
{
	delete &resource;
}

//////////////////////////////////////////////////////////////////////////

physx::PxU32 UserRenderResourceManager::getMaxBonesForMaterial(void* material)
{
	return MAX_BONES_ALLOWED;
}


//-------------------------------------------------------------------------
//	UserRenderer impl
//-------------------------------------------------------------------------

UserRenderer::UserRenderer()
: shadowRenderMode(false)
{

}

//////////////////////////////////////////////////////////////////////////

UserRenderer::~UserRenderer()
{
}

//////////////////////////////////////////////////////////////////////////

void UserRenderer::Destroy()
{
	for (DeclsMap::iterator it = declsMap.begin(), it_end = declsMap.end(); it != it_end; ++it)
	{
		SAFE_RELEASE(it->second);
	}
	declsMap.clear();
}

//////////////////////////////////////////////////////////////////////////

void UserRenderer::renderResource(const NxApexRenderContext& context)
{
	D3DPERF_BeginEvent(0, L"ApexRenderer::renderResource");
	UserRenderResource *urr = static_cast<UserRenderResource*>(context.renderResource);
	if (!urr || !urr->mtl)
		return;

	//	First prepare vertex declaration for all input vertex buffers
	D3DVERTEXELEMENT9 fullVD[NxRenderVertexSemantic::NUM_SEMANTICS] = {0};

	bool createVertexDescResult = CreateFullVertexDescription(context, &fullVD[0], _countof(fullVD));
	if (!createVertexDescResult)
		return;

	//	Hash vertex description
	uint32_t vdHash = HashVertexDescription(&fullVD[0], _countof(fullVD));

	if (!urr->meshVD)
	{
		DeclsMap::iterator it = declsMap.find(vdHash);
		if (it == declsMap.end())
		{
			//	Create and add new vertex declaration
			urr->meshVD = CreateVertexDeclaration(context, &fullVD[0]);
			if (urr->meshVD)
			{
				declsMap[vdHash] = urr->meshVD;
			}
		}
		else
		{
			urr->meshVD = it->second;
		}
	}

	//	Now setup buffers and render this resource
	d3dc._SetDecl(urr->meshVD);

	for (uint32_t i = 0; i < urr->getNbVertexBuffers(); ++i)
	{
		UserRenderVertexBuffer *vb = static_cast<UserRenderVertexBuffer*>(urr->getVertexBuffer(i));
		if (vb && vb->vb)
			vb->vb->Set(i);
	}

	UserRenderIndexBuffer *ib = static_cast<UserRenderIndexBuffer*>(urr->getIndexBuffer());
	if (ib && ib->ib)
		ib->ib->Set();

	D3DXMATRIXA16 world(&context.local2world.column0.x);
	D3DXMATRIXA16 mts[2];
	mts[0] = r3dRenderer->ViewProjMatrix;
	D3DXMatrixTranspose(&mts[0], &mts[0]);
	mts[1] = world;

	IDirect3DDevice9 *d = r3dRenderer->pd3ddev;
	d->SetVertexShaderConstantF(0, &mts[0]._11, 8);

	UserRenderBoneBuffer *ubb = static_cast<UserRenderBoneBuffer*>(urr->getBoneBuffer());
	if (ubb)
		d->SetVertexShaderConstantF(12, &ubb->boneMatrices[urr->firstBone].x, urr->numBones * BONE_MATRIX_SIZE);

	r3dMaterial * m = urr->mtl;
	shadowRenderMode
		? m->StartShadows(R3D_MATVS_APEX_DESTRUCTIBLE)
		: m->Start(R3D_MATVS_APEX_DESTRUCTIBLE, 0);
	r3dRenderer->DrawIndexed(urr->primType, urr->firstVertex, 0, urr->numVerts, urr->firstIndex, urr->numPrimitives);
	shadowRenderMode
		? m->EndShadows()
		: m->End();
	D3DPERF_EndEvent();
}

//////////////////////////////////////////////////////////////////////////

uint32_t UserRenderer::HashVertexDescription(D3DVERTEXELEMENT9 *ve, size_t count) const
{
	D3DVERTEXELEMENT9 end = D3DDECL_END();
	uint32_t hashBuf[NxRenderVertexSemantic::NUM_SEMANTICS] = {0};

	for (size_t i = 0; i < count; ++i)
	{
		if (ve[i] == end)
			break;
		hashBuf[i] = r3dHash::MakeHash(reinterpret_cast<const char*>(&ve[i]), sizeof(D3DVERTEXELEMENT9));
	}

	//	Combine hashes
	uint32_t finalHash = r3dHash::MakeHash(reinterpret_cast<const char*>(&hashBuf[0]), count * sizeof(uint32_t));
	return finalHash;
}

//////////////////////////////////////////////////////////////////////////

IDirect3DVertexDeclaration9 * UserRenderer::CreateVertexDeclaration(const NxApexRenderContext& context, D3DVERTEXELEMENT9 *ve) const
{
	if (!ve) return 0;

	//	Now create vertex declaration
	IDirect3DVertexDeclaration9 *rv = 0;
	D3D_V( r3dRenderer->pd3ddev->CreateVertexDeclaration(ve, &rv) );

	return rv;
}

//////////////////////////////////////////////////////////////////////////

bool UserRenderer::CreateFullVertexDescription(const NxApexRenderContext& context, D3DVERTEXELEMENT9 *ve, size_t count) const
{
	NxUserRenderResource *rr = context.renderResource;
	if (!rr)
		return 0;

	D3DVERTEXELEMENT9 end = D3DDECL_END();

	uint32_t entriesCount = 0;

	//	Iterate over all vertex buffers
	for (uint32_t i = 0; i < rr->getNbVertexBuffers(); ++i)
	{
		//	Fill vertex description array with descriptions in buffer
		UserRenderVertexBuffer *urvb = static_cast<UserRenderVertexBuffer*>(rr->getVertexBuffer(i));
		if (!urvb) continue;

		for (uint32_t k = 0; k < urvb->numberOfVertexElements; ++k)
		{
			r3d_assert(entriesCount < count);
			if (entriesCount >= count)
				return 0;

			ve[entriesCount] = urvb->vertexDesc[k];
			ve[entriesCount++].Stream = static_cast<WORD>(i);
		}
	}
	//	Append end terminator
	r3d_assert(entriesCount < count);
	if (entriesCount >= count)
		return 0;

	ve[entriesCount] = end;
	return true;
}

//-------------------------------------------------------------------------
//	Standalone routines
//-------------------------------------------------------------------------

void DestroyApexUserRenderer()
{
	apexUserRenderer.Destroy();
}

#endif