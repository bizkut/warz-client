// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.

#include <SampleApexRenderResources.h>

#include <Renderer.h>

#include <RendererVertexBuffer.h>
#include <RendererVertexBufferDesc.h>

#include <RendererIndexBuffer.h>
#include <RendererIndexBufferDesc.h>

#include <RendererInstanceBuffer.h>
#include <RendererInstanceBufferDesc.h>

#include <RendererTexture.h>
#include <RendererTextureDesc.h>

#include <RendererMesh.h>
#include <RendererMeshDesc.h>

#include <RendererMaterial.h>
#include <RendererMaterialDesc.h>
#include <RendererMaterialInstance.h>

#if !USE_RENDERER_MATERIAL
#include <SampleMaterialAsset.h>
#endif

#include <NxApexRenderContext.h>
#include <NxUserRenderIndexBufferDesc.h>
#include <NxUserRenderInstanceBuffer.h>
#include <NxUserRenderResourceDesc.h>
#include <NxUserRenderSpriteBufferDesc.h>
#include <NxUserRenderSurfaceBufferDesc.h>
#include <NxUserRenderVertexBufferDesc.h>

#include <PxMat34Legacy.h>

static RendererVertexBuffer::Hint convertFromApexVB(physx::apex::NxRenderBufferHint::Enum apexHint)
{
	RendererVertexBuffer::Hint vbhint = RendererVertexBuffer::HINT_STATIC;
	if (apexHint == physx::apex::NxRenderBufferHint::DYNAMIC || apexHint == physx::apex::NxRenderBufferHint::STREAMING)
	{
		vbhint = RendererVertexBuffer::HINT_DYNAMIC;
	}
	return vbhint;
}

static RendererVertexBuffer::Semantic convertFromApexVB(physx::apex::NxRenderVertexSemantic::Enum apexSemantic)
{
	RendererVertexBuffer::Semantic semantic = RendererVertexBuffer::NUM_SEMANTICS;
	switch (apexSemantic)
	{
	case physx::apex::NxRenderVertexSemantic::POSITION:
		semantic = RendererVertexBuffer::SEMANTIC_POSITION;
		break;
	case physx::apex::NxRenderVertexSemantic::NORMAL:
		semantic = RendererVertexBuffer::SEMANTIC_NORMAL;
		break;
	case physx::apex::NxRenderVertexSemantic::TANGENT:
		semantic = RendererVertexBuffer::SEMANTIC_TANGENT;
		break;
	case physx::apex::NxRenderVertexSemantic::COLOR:
		semantic = RendererVertexBuffer::SEMANTIC_COLOR;
		break;
	case physx::apex::NxRenderVertexSemantic::TEXCOORD0:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD0;
		break;
	case physx::apex::NxRenderVertexSemantic::TEXCOORD1:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD1;
		break;
	case physx::apex::NxRenderVertexSemantic::TEXCOORD2:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD2;
		break;
	case physx::apex::NxRenderVertexSemantic::TEXCOORD3:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD3;
		break;
	case physx::apex::NxRenderVertexSemantic::BONE_INDEX:
		semantic = RendererVertexBuffer::SEMANTIC_BONEINDEX;
		break;
	case physx::apex::NxRenderVertexSemantic::BONE_WEIGHT:
		semantic = RendererVertexBuffer::SEMANTIC_BONEWEIGHT;
		break;
	case physx::apex::NxRenderVertexSemantic::DISPLACEMENT_TEXCOORD:
		semantic = RendererVertexBuffer::SEMANTIC_DISPLACEMENT_TEXCOORD;
		break;
	case physx::apex::NxRenderVertexSemantic::DISPLACEMENT_FLAGS:
		semantic = RendererVertexBuffer::SEMANTIC_DISPLACEMENT_FLAGS;
		break;
	default:
		//PX_ASSERT(semantic < RendererVertexBuffer::NUM_SEMANTICS);
		break;
	}
	return semantic;
}

static RendererVertexBuffer::Format convertFromApexVB(physx::apex::NxRenderDataFormat::Enum apexFormat)
{
	RendererVertexBuffer::Format format = RendererVertexBuffer::NUM_FORMATS;
	switch (apexFormat)
	{
	case physx::apex::NxRenderDataFormat::FLOAT1:
		format = RendererVertexBuffer::FORMAT_FLOAT1;
		break;
	case physx::apex::NxRenderDataFormat::FLOAT2:
		format = RendererVertexBuffer::FORMAT_FLOAT2;
		break;
	case physx::apex::NxRenderDataFormat::FLOAT3:
		format = RendererVertexBuffer::FORMAT_FLOAT3;
		break;
	case physx::apex::NxRenderDataFormat::FLOAT4:
		format = RendererVertexBuffer::FORMAT_FLOAT4;
		break;
	case physx::apex::NxRenderDataFormat::B8G8R8A8:
		format = RendererVertexBuffer::FORMAT_COLOR_BGRA;
		break;
	case physx::apex::NxRenderDataFormat::UBYTE4:
	case physx::apex::NxRenderDataFormat::R8G8B8A8:
		format = RendererVertexBuffer::FORMAT_COLOR_RGBA;
		break;
	case physx::apex::NxRenderDataFormat::USHORT1:
	case physx::apex::NxRenderDataFormat::USHORT2:
	case physx::apex::NxRenderDataFormat::USHORT3:
	case physx::apex::NxRenderDataFormat::USHORT4:
		format = RendererVertexBuffer::FORMAT_USHORT4;
		break;
	default:
		//PX_ASSERT(format < RendererVertexBuffer::NUM_FORMATS || apexFormat==NxRenderDataFormat::UNSPECIFIED);
		break;
	}
	return format;
}

static RendererVertexBuffer::Semantic convertFromApexSB(physx::apex::NxRenderSpriteSemantic::Enum apexSemantic)
{
	RendererVertexBuffer::Semantic semantic = RendererVertexBuffer::NUM_SEMANTICS;
	switch (apexSemantic)
	{
	case physx::apex::NxRenderSpriteSemantic::POSITION:
		semantic = RendererVertexBuffer::SEMANTIC_POSITION;
		break;
	case physx::apex::NxRenderSpriteSemantic::COLOR:
		semantic = RendererVertexBuffer::SEMANTIC_COLOR;
		break;
	case physx::apex::NxRenderSpriteSemantic::COLOR_FLOAT4:
		semantic = RendererVertexBuffer::SEMANTIC_COLOR;
		break;
	case physx::apex::NxRenderSpriteSemantic::VELOCITY:
		semantic = RendererVertexBuffer::SEMANTIC_NORMAL;
		break;
	case physx::apex::NxRenderSpriteSemantic::SCALE:
		semantic = RendererVertexBuffer::SEMANTIC_TANGENT;
		break;
	case physx::apex::NxRenderSpriteSemantic::LIFE_REMAIN:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD0;
		break;
	case physx::apex::NxRenderSpriteSemantic::DENSITY:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD1;
		break;
	case physx::apex::NxRenderSpriteSemantic::SUBTEXTURE:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD2;
		break;
	case physx::apex::NxRenderSpriteSemantic::ORIENTATION:
		semantic = RendererVertexBuffer::SEMANTIC_TEXCOORD3;
		break;
	default:
		//PX_ASSERT(semantic < RendererVertexBuffer::NUM_SEMANTICS);
		break;
	}
	return semantic;
}

static RendererIndexBuffer::Hint convertFromApexIB(physx::apex::NxRenderBufferHint::Enum apexHint)
{
	RendererIndexBuffer::Hint ibhint = RendererIndexBuffer::HINT_STATIC;
	if (apexHint == physx::apex::NxRenderBufferHint::DYNAMIC || apexHint == physx::apex::NxRenderBufferHint::STREAMING)
	{
		ibhint = RendererIndexBuffer::HINT_DYNAMIC;
	}
	return ibhint;
}

static RendererIndexBuffer::Format convertFromApexIB(physx::apex::NxRenderDataFormat::Enum apexFormat)
{
	RendererIndexBuffer::Format format = RendererIndexBuffer::NUM_FORMATS;
	switch (apexFormat)
	{
	case physx::apex::NxRenderDataFormat::UBYTE1:
		PX_ASSERT(0); /* UINT8 Indices not in HW. */
		break;
	case physx::apex::NxRenderDataFormat::USHORT1:
		format = RendererIndexBuffer::FORMAT_UINT16;
		break;
	case physx::apex::NxRenderDataFormat::UINT1:
		format = RendererIndexBuffer::FORMAT_UINT32;
		break;
	default:
		PX_ASSERT(format < RendererIndexBuffer::NUM_FORMATS);
	}
	return format;
}

static RendererTexture::Format convertFromApexSB(physx::apex::NxRenderDataFormat::Enum apexFormat)
{
	RendererTexture::Format format = RendererTexture::NUM_FORMATS;
	switch (apexFormat)
	{
	case physx::apex::NxRenderDataFormat::FLOAT1:
		format = RendererTexture::FORMAT_R32F;
		break;
	default:
		PX_ASSERT(format < RendererTexture::NUM_FORMATS);
		break;
	}
	return format;
}

static RendererMesh::Primitive convertFromApex(physx::apex::NxRenderPrimitiveType::Enum apexPrimitive)
{
	RendererMesh::Primitive primitive = RendererMesh::NUM_PRIMITIVES;
	switch (apexPrimitive)
	{
	case physx::apex::NxRenderPrimitiveType::TRIANGLES:
		primitive = RendererMesh::PRIMITIVE_TRIANGLES;
		break;
	case physx::apex::NxRenderPrimitiveType::TRIANGLE_STRIP:
		primitive = RendererMesh::PRIMITIVE_TRIANGLE_STRIP;
		break;

	case physx::apex::NxRenderPrimitiveType::LINES:
		primitive = RendererMesh::PRIMITIVE_LINES;
		break;
	case physx::apex::NxRenderPrimitiveType::LINE_STRIP:
		primitive = RendererMesh::PRIMITIVE_LINE_STRIP;
		break;

	case physx::apex::NxRenderPrimitiveType::POINTS:
		primitive = RendererMesh::PRIMITIVE_POINTS;
		break;
	case physx::apex::NxRenderPrimitiveType::POINT_SPRITES:
		primitive = RendererMesh::PRIMITIVE_POINT_SPRITES;
		break;

	case physx::apex::NxRenderPrimitiveType::UNKNOWN: // Make compiler happy
		break;
	}
	PX_ASSERT(primitive < RendererMesh::NUM_PRIMITIVES);
	return primitive;
}

/*********************************
* SampleApexRendererVertexBuffer *
*********************************/

SampleApexRendererVertexBuffer::SampleApexRendererVertexBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderVertexBufferDesc& desc) :
	m_renderer(renderer)
{
	m_vertexbuffer = 0;
	SampleRenderer::RendererVertexBufferDesc vbdesc;
	vbdesc.hint = convertFromApexVB(desc.hint);
	for (physx::PxU32 i = 0; i < physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS; i++)
	{
		physx::apex::NxRenderVertexSemantic::Enum apexSemantic = physx::apex::NxRenderVertexSemantic::Enum(i);
		physx::apex::NxRenderDataFormat::Enum apexFormat = desc.buffersRequest[i];
		if (apexSemantic == physx::apex::NxRenderVertexSemantic::TANGENT && apexFormat != physx::apex::NxRenderDataFormat::UNSPECIFIED)
		{
			PX_ASSERT(apexFormat == physx::apex::NxRenderDataFormat::FLOAT3 || apexFormat == physx::apex::NxRenderDataFormat::FLOAT4);
			// always use FLOAT4 for tangents!!!
			apexFormat = physx::apex::NxRenderDataFormat::FLOAT4;
		}
		else if (apexSemantic == physx::apex::NxRenderVertexSemantic::BONE_INDEX && apexFormat != physx::apex::NxRenderDataFormat::UNSPECIFIED)
		{
			PX_ASSERT(apexFormat == physx::apex::NxRenderDataFormat::USHORT1
				|| apexFormat == physx::apex::NxRenderDataFormat::USHORT2
				|| apexFormat == physx::apex::NxRenderDataFormat::USHORT3
				|| apexFormat == physx::apex::NxRenderDataFormat::USHORT4);

			// use either USHORT1 for destruction or USHORT4 for everything else. fill with 0 in writeBuffer
			if (apexFormat == physx::apex::NxRenderDataFormat::USHORT2 || physx::apex::NxRenderDataFormat::USHORT3)
			{
				apexFormat = physx::apex::NxRenderDataFormat::USHORT4;
			}
		}
		else if (apexSemantic == physx::apex::NxRenderVertexSemantic::BONE_WEIGHT && apexFormat != physx::apex::NxRenderDataFormat::UNSPECIFIED)
		{
			PX_ASSERT(apexFormat == physx::apex::NxRenderDataFormat::FLOAT1
				|| apexFormat == physx::apex::NxRenderDataFormat::FLOAT2
				|| apexFormat == physx::apex::NxRenderDataFormat::FLOAT3
				|| apexFormat == physx::apex::NxRenderDataFormat::FLOAT4);

			// use either FLOAT1 for destruction or FLOAT4 for everything else. fill with 0.0 in writeBuffer
			if (apexFormat == physx::apex::NxRenderDataFormat::FLOAT2 || physx::apex::NxRenderDataFormat::FLOAT3)
			{
				apexFormat = physx::apex::NxRenderDataFormat::FLOAT4;
			}
		}
		RendererVertexBuffer::Semantic semantic = convertFromApexVB(apexSemantic);
		RendererVertexBuffer::Format   format   = convertFromApexVB(apexFormat);
		if (semantic < RendererVertexBuffer::NUM_SEMANTICS && format < RendererVertexBuffer::NUM_FORMATS)
		{
			vbdesc.semanticFormats[semantic] = format;
		}
	}
#if defined(APEX_CUDA_SUPPORT)
	vbdesc.registerInCUDA = desc.registerInCUDA;
	vbdesc.interopContext = desc.interopContext;
#endif
	vbdesc.maxVertices = desc.maxVerts;
	m_vertexbuffer = m_renderer.createVertexBuffer(vbdesc);
	PX_ASSERT(m_vertexbuffer);
	m_uvOrigin = desc.uvOrigin;
}

SampleApexRendererVertexBuffer::~SampleApexRendererVertexBuffer(void)
{
	if (m_vertexbuffer)
	{
		m_vertexbuffer->release();
	}
}

bool SampleApexRendererVertexBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
#if defined(APEX_CUDA_SUPPORT)
	if (m_vertexbuffer)
	{
		return	m_vertexbuffer->getInteropResourceHandle(handle);
	}
	else
	{
		return false;
	}
#else
	CUgraphicsResource* tmp = &handle;
	PX_FORCE_PARAMETER_REFERENCE(tmp);

	return false;
#endif
}

void SampleApexRendererVertexBuffer::fixUVOrigin(void* uvdata, physx::PxU32 stride, physx::PxU32 num)
{
#define ITERATE_UVS(_uop,_vop)								\
	for(physx::PxU32 i=0; i<num; i++)						\
	{														\
	physx::PxF32 &u = *(((physx::PxF32*)uvdata) + 0);	\
	physx::PxF32 &v = *(((physx::PxF32*)uvdata) + 1);	\
	u=_uop;												\
	v=_vop;												\
	uvdata = ((physx::PxU8*)uvdata)+stride;				\
}
	switch (m_uvOrigin)
	{
	case physx::apex::NxTextureUVOrigin::ORIGIN_BOTTOM_LEFT:
		// nothing to do...
		break;
	case physx::apex::NxTextureUVOrigin::ORIGIN_BOTTOM_RIGHT:
		ITERATE_UVS(1 - u, v);
		break;
	case physx::apex::NxTextureUVOrigin::ORIGIN_TOP_LEFT:
		ITERATE_UVS(u, 1 - v);
		break;
	case physx::apex::NxTextureUVOrigin::ORIGIN_TOP_RIGHT:
		ITERATE_UVS(1 - u, 1 - v);
		break;
	default:
		PX_ASSERT(0); // UNKNOWN ORIGIN / TODO!
	}
#undef ITERATE_UVS
}

void SampleApexRendererVertexBuffer::flipColors(void* uvData, physx::PxU32 stride, physx::PxU32 num)
{
	for (physx::PxU32 i = 0; i < num; i++)
	{
		physx::PxU8* color = ((physx::PxU8*)uvData) + (stride * i);
		std::swap(color[0], color[3]);
		std::swap(color[1], color[2]);
	}
}

// special fast path for interleaved vec3 position and normal and optional vec4 tangent (avoids painfully slow strided writes to locked vertex buffer)
bool SampleApexRendererVertexBuffer::writeBufferFastPath(const physx::apex::NxApexRenderVertexBufferData& data, physx::PxU32 firstVertex, physx::PxU32 numVerts)
{
	for (physx::PxU32 i = 0; i < physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS; i++)
	{
		physx::apex::NxRenderVertexSemantic::Enum apexSemantic = (physx::apex::NxRenderVertexSemantic::Enum)i;
		switch(apexSemantic)
		{
		case physx::apex::NxRenderVertexSemantic::POSITION:
		case physx::apex::NxRenderVertexSemantic::NORMAL:
		case physx::apex::NxRenderVertexSemantic::TANGENT:
			break;
		default:
			if(data.getSemanticData(apexSemantic).data)
				return false;
		}
	}

	const physx::apex::NxApexRenderSemanticData& positionSemantic = data.getSemanticData(physx::apex::NxRenderVertexSemantic::POSITION);
	const physx::apex::NxApexRenderSemanticData& normalSemantic = data.getSemanticData(physx::apex::NxRenderVertexSemantic::NORMAL);
	const physx::apex::NxApexRenderSemanticData& tangentSemantic = data.getSemanticData(physx::apex::NxRenderVertexSemantic::TANGENT);

	const physx::PxVec3* PX_RESTRICT positionSrc = (const physx::PxVec3*)positionSemantic.data;
	const physx::PxVec3* PX_RESTRICT normalSrc = (const physx::PxVec3*)normalSemantic.data;
	const physx::PxVec4* PX_RESTRICT tangentSrc = (const physx::PxVec4*)tangentSemantic.data;

	RendererVertexBuffer::Format positionFormat = m_vertexbuffer->getFormatForSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	RendererVertexBuffer::Format normalFormat = m_vertexbuffer->getFormatForSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	RendererVertexBuffer::Format tangentFormat = m_vertexbuffer->getFormatForSemantic(RendererVertexBuffer::SEMANTIC_TANGENT);

	if(positionSrc == 0 || positionSemantic.stride != 12 || RendererVertexBuffer::getFormatByteSize(positionFormat) != 12)
		return false;

	if(normalSrc == 0 || normalSemantic.stride != 12 || RendererVertexBuffer::getFormatByteSize(normalFormat) != 12)
		return false;

	if(tangentSrc != 0 && (tangentSemantic.stride != 16 || RendererVertexBuffer::getFormatByteSize(tangentFormat) != 16))
		return false;

	physx::PxU32 stride = 0;
	void* positionDst = m_vertexbuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);
	void* normalDst = m_vertexbuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride);
	void* tangentDst = tangentSrc ? m_vertexbuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TANGENT, stride) : 0;

	bool useFastPath = stride == (tangentSrc ? 40 : 24);
	useFastPath &= normalDst == (physx::PxU8*)positionDst + 12;
	useFastPath &= !tangentSrc || tangentDst == (physx::PxU8*)positionDst + 24;

	if(useFastPath)
	{
		physx::PxU8* dstIt = (physx::PxU8*)positionDst + stride * firstVertex;
		physx::PxU8* dstEnd = dstIt + stride * numVerts;

		for(; dstIt < dstEnd; dstIt += stride)
		{
			*(physx::PxVec3* PX_RESTRICT)(dstIt   ) = *positionSrc++;
			*(physx::PxVec3* PX_RESTRICT)(dstIt+12) = *normalSrc++;
			if(tangentSrc)
				*(physx::PxVec4* PX_RESTRICT)(dstIt+24) = -*tangentSrc++;
		}
	}

	m_vertexbuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	m_vertexbuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	if(tangentSrc)
		m_vertexbuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TANGENT);

	return useFastPath;
}

void SampleApexRendererVertexBuffer::writeBuffer(const physx::apex::NxApexRenderVertexBufferData& data, physx::PxU32 firstVertex, physx::PxU32 numVerts)
{
	if (!m_vertexbuffer || !numVerts)
	{
		return;
	}

	if(writeBufferFastPath(data, firstVertex, numVerts))
		return;

	for (physx::PxU32 i = 0; i < physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS; i++)
	{
		physx::apex::NxRenderVertexSemantic::Enum apexSemantic = (physx::apex::NxRenderVertexSemantic::Enum)i;
		const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(apexSemantic);
		if (semanticData.data)
		{
			const void* srcData = semanticData.data;
			const physx::PxU32 srcStride = semanticData.stride;

			RendererVertexBuffer::Semantic semantic = convertFromApexVB(apexSemantic);
			if (semantic < RendererVertexBuffer::NUM_SEMANTICS)
			{
				RendererVertexBuffer::Format format = m_vertexbuffer->getFormatForSemantic(semantic);
				physx::PxU32 semanticStride = 0;
				void* dstData = m_vertexbuffer->lockSemantic(semantic, semanticStride);
				void* dstDataCopy = dstData;
				PX_ASSERT(dstData && semanticStride);
				if (dstData && semanticStride)
				{
#if defined(RENDERER_DEBUG) && 0 // enable to confirm that destruction bone indices are within valid range.
					// verify input data...
					if (apexSemantic == NxRenderVertexSemantic::BONE_INDEX)
					{
						physx::PxU32 maxIndex = 0;
						for (physx::PxU32 i = 0; i < numVerts; i++)
						{
							physx::PxU16 index = *(physx::PxU16*)(((physx::PxU8*)srcData) + (srcStride * i));
							if (index > maxIndex)
							{
								maxIndex = index;
							}
						}
						PX_ASSERT(maxIndex < RENDERER_MAX_BONES);
					}
#endif
					dstData = ((physx::PxU8*)dstData) + firstVertex * semanticStride;
					physx::PxU32 formatSize = RendererVertexBuffer::getFormatByteSize(format);

					if (apexSemantic == physx::apex::NxRenderVertexSemantic::TANGENT && semanticData.format == physx::apex::NxRenderDataFormat::FLOAT4)
					{
						// invert entire tangent
						for (physx::PxU32 j = 0; j < numVerts; j++)
						{
							physx::PxVec4 tangent = *(physx::PxVec4*)srcData;
							*(physx::PxVec4*)dstData = -tangent;
							dstData = ((physx::PxU8*)dstData) + semanticStride;
							srcData = ((physx::PxU8*)srcData) + srcStride;
						}
					}
					else if (apexSemantic == physx::apex::NxRenderVertexSemantic::TANGENT && semanticData.format == physx::apex::NxRenderDataFormat::FLOAT3)
					{
						// we need to increase the data from float3 to float4
						const physx::apex::NxApexRenderSemanticData& bitangentData = data.getSemanticData(physx::apex::NxRenderVertexSemantic::BINORMAL);
						const physx::apex::NxApexRenderSemanticData& normalData = data.getSemanticData(physx::apex::NxRenderVertexSemantic::NORMAL);
						if (bitangentData.format != physx::apex::NxRenderDataFormat::UNSPECIFIED && normalData.format != physx::apex::NxRenderDataFormat::UNSPECIFIED)
						{
							PX_ASSERT(bitangentData.format == physx::apex::NxRenderDataFormat::FLOAT3);
							const void* srcDataBitangent = bitangentData.data;
							const physx::PxU32 srcStrideBitangent = bitangentData.stride;

							PX_ASSERT(normalData.format == physx::apex::NxRenderDataFormat::FLOAT3);
							const void* srcDataNormal = normalData.data;
							const physx::PxU32 srcStrideNormal = normalData.stride;

							for (physx::PxU32 j = 0; j < numVerts; j++)
							{
								physx::PxVec3 normal = *(physx::PxVec3*)srcDataNormal;
								physx::PxVec3 bitangent = *(physx::PxVec3*)srcDataBitangent;
								physx::PxVec3 tangent = *(physx::PxVec3*)srcData;
								float tangentw = physx::PxSign(normal.cross(tangent).dot(bitangent));
								*(physx::PxVec4*)dstData = physx::PxVec4(tangent, -tangentw);

								dstData = ((physx::PxU8*)dstData) + semanticStride;
								srcData = ((physx::PxU8*)srcData) + srcStride;
								srcDataBitangent = ((physx::PxU8*)srcDataBitangent) + srcStrideBitangent;
								srcDataNormal = ((physx::PxU8*)srcDataNormal) + srcStrideNormal;
							}
						}
						else
						{
							// just assume 1.0 as tangent.w if there is no bitangent to calculate this from
							for (physx::PxU32 j = 0; j < numVerts; j++)
							{
								physx::PxVec3 tangent = *(physx::PxVec3*)srcData;
								*(physx::PxVec4*)dstData = physx::PxVec4(tangent, 1.0f);
								dstData = ((physx::PxU8*)dstData) + semanticStride;
								srcData = ((physx::PxU8*)srcData) + srcStride;
							}
						}
					}
					else if (apexSemantic == physx::apex::NxRenderVertexSemantic::BONE_INDEX && (semanticData.format == physx::apex::NxRenderDataFormat::USHORT2 || semanticData.format == physx::apex::NxRenderDataFormat::USHORT3))
					{
						unsigned int numIndices = 0;
						switch (semanticData.format)
						{
						case physx::apex::NxRenderDataFormat::USHORT1: numIndices = 1; break;
						case physx::apex::NxRenderDataFormat::USHORT2: numIndices = 2; break;
						case physx::apex::NxRenderDataFormat::USHORT3: numIndices = 3; break;
						default:
							PX_ALWAYS_ASSERT();
							break;
						}

						for (physx::PxU32 j = 0; j < numVerts; j++)
						{
							unsigned short* boneIndices = (unsigned short*)srcData;
							unsigned short* dstBoneIndices = (unsigned short*)dstData;

							for (unsigned int i = 0; i < numIndices; i++)
							{
								dstBoneIndices[i] = boneIndices[i];
							}
							for (unsigned int i = numIndices; i < 4; i++)
							{
								dstBoneIndices[i] = 0;
							}

							dstData = ((physx::PxU8*)dstData) + semanticStride;
							srcData = ((physx::PxU8*)srcData) + srcStride;
						}
					}
					else if (apexSemantic == physx::apex::NxRenderVertexSemantic::BONE_WEIGHT && (semanticData.format == physx::apex::NxRenderDataFormat::FLOAT2 || semanticData.format == physx::apex::NxRenderDataFormat::FLOAT3))
					{
						unsigned int numWeights = 0;
						switch (semanticData.format)
						{
						case physx::apex::NxRenderDataFormat::FLOAT1: numWeights = 1; break;
						case physx::apex::NxRenderDataFormat::FLOAT2: numWeights = 2; break;
						case physx::apex::NxRenderDataFormat::FLOAT3: numWeights = 3; break;
						default:
							PX_ALWAYS_ASSERT();
							break;
						}

						for (physx::PxU32 j = 0; j < numVerts; j++)
						{
							float* boneIndices = (float*)srcData;
							float* dstBoneIndices = (float*)dstData;

							for (unsigned int i = 0; i < numWeights; i++)
							{
								dstBoneIndices[i] = boneIndices[i];
							}
							for (unsigned int i = numWeights; i < 4; i++)
							{
								dstBoneIndices[i] = 0.0f;
							}

							dstData = ((physx::PxU8*)dstData) + semanticStride;
							srcData = ((physx::PxU8*)srcData) + srcStride;
						}
					}
					else if (formatSize == 4)
					{
						for (physx::PxU32 j = 0; j < numVerts; j++)
						{
							*(physx::PxU32*)dstData = *(physx::PxU32*)srcData;
							dstData = ((physx::PxU8*)dstData) + semanticStride;
							srcData = ((physx::PxU8*)srcData) + srcStride;
						}
					}
					else if (formatSize == 12)
					{
						for (physx::PxU32 j = 0; j < numVerts; j++)
						{
							*(physx::PxVec3*)dstData = *(physx::PxVec3*)srcData;
							dstData = ((physx::PxU8*)dstData) + semanticStride;
							srcData = ((physx::PxU8*)srcData) + srcStride;
						}
					}
					else
					{
						for (physx::PxU32 j = 0; j < numVerts; j++)
						{
							memcpy(dstData, srcData, formatSize);
							dstData = ((physx::PxU8*)dstData) + semanticStride;
							srcData = ((physx::PxU8*)srcData) + srcStride;
						}
					}

					// fix-up the UVs...
					if ((semantic >= RendererVertexBuffer::SEMANTIC_TEXCOORD0  &&
						semantic <= RendererVertexBuffer::SEMANTIC_TEXCOORDMAX) ||
						semantic == RendererVertexBuffer::SEMANTIC_DISPLACEMENT_TEXCOORD)
					{
						fixUVOrigin(dstDataCopy, semanticStride, numVerts);
					}

#if defined(RENDERER_PS3)
					// we have to flip the colors
					if (semantic == RendererVertexBuffer::SEMANTIC_COLOR)
					{
						flipColors(dstDataCopy, semanticStride, numVerts);
					}
#endif

					m_vertexbuffer->unlockSemantic(semantic);
				}
			}
		}
	}
}


/********************************
* SampleApexRendererIndexBuffer *
********************************/

SampleApexRendererIndexBuffer::SampleApexRendererIndexBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderIndexBufferDesc& desc) :
	m_renderer(renderer)
{
	m_indexbuffer = 0;
	SampleRenderer::RendererIndexBufferDesc ibdesc;
	ibdesc.hint       = convertFromApexIB(desc.hint);
	ibdesc.format     = convertFromApexIB(desc.format);
	ibdesc.maxIndices = desc.maxIndices;
#if defined(APEX_CUDA_SUPPORT)
	ibdesc.registerInCUDA = desc.registerInCUDA;
	ibdesc.interopContext = desc.interopContext;
#endif
	m_indexbuffer = m_renderer.createIndexBuffer(ibdesc);
	PX_ASSERT(m_indexbuffer);

	m_primitives = desc.primitives;
}

bool SampleApexRendererIndexBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
#if defined(APEX_CUDA_SUPPORT)
	if (m_indexbuffer)
	{
		return	m_indexbuffer->getInteropResourceHandle(handle);
	}
	else
	{
		return false;
	}
#else
	CUgraphicsResource* tmp = &handle;
	PX_FORCE_PARAMETER_REFERENCE(tmp);

	return false;
#endif
}

SampleApexRendererIndexBuffer::~SampleApexRendererIndexBuffer(void)
{
	if (m_indexbuffer)
	{
		m_indexbuffer->release();
	}
}

void SampleApexRendererIndexBuffer::writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 numElements)
{
	if (m_indexbuffer && numElements)
	{
		void* dstData = m_indexbuffer->lock();
		PX_ASSERT(dstData);
		if (dstData)
		{
			RendererIndexBuffer::Format format = m_indexbuffer->getFormat();

			if (m_primitives == physx::apex::NxRenderPrimitiveType::TRIANGLES)
			{
				physx::PxU32 numTriangles = numElements / 3;
				if (format == RendererIndexBuffer::FORMAT_UINT16)
				{
					physx::PxU16*       dst = ((physx::PxU16*)dstData) + firstDestElement;
					const physx::PxU16* src = (const physx::PxU16*)srcData;
					for (physx::PxU32 i = 0; i < numTriangles; i++)
						for (physx::PxU32 j = 0; j < 3; j++)
						{
							dst[i * 3 + j] = src[i * 3 + (2 - j)];
						}
				}
				else if (format == RendererIndexBuffer::FORMAT_UINT32)
				{
					physx::PxU32*       dst = ((physx::PxU32*)dstData) + firstDestElement;
					const physx::PxU32* src = (const physx::PxU32*)srcData;
					for (physx::PxU32 i = 0; i < numTriangles; i++)
						for (physx::PxU32 j = 0; j < 3; j++)
						{
							dst[i * 3 + j] = src[i * 3 + (2 - j)];
						}
				}
				else
				{
					PX_ASSERT(0);
				}
			}
			else
			{
				if (format == RendererIndexBuffer::FORMAT_UINT16)
				{
					physx::PxU16*		dst = ((physx::PxU16*)dstData) + firstDestElement;
					const physx::PxU8*	src = (const physx::PxU8*)srcData;
					for (physx::PxU32 i = 0; i < numElements; i++, dst++, src += srcStride)
					{
						*dst = *((physx::PxU16*)src);
					}
				}
				else if (format == RendererIndexBuffer::FORMAT_UINT32)
				{
					physx::PxU32*		dst = ((physx::PxU32*)dstData) + firstDestElement;
					const physx::PxU8*	src = (const physx::PxU8*)srcData;
					for (physx::PxU32 i = 0; i < numElements; i++, dst++, src += srcStride)
					{
						*dst = *((physx::PxU32*)src);
					}
				}
				else
				{
					PX_ASSERT(0);
				}
			}
		}
		m_indexbuffer->unlock();
	}
}


/********************************
* SampleApexRendererSurfaceBuffer *
********************************/

SampleApexRendererSurfaceBuffer::SampleApexRendererSurfaceBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderSurfaceBufferDesc& desc) :
	m_renderer(renderer)
{
	m_texture = 0;
	SampleRenderer::RendererTextureDesc tdesc;
	//tdesc.hint       = convertFromApexSB(desc.hint);
	tdesc.format		= convertFromApexSB(desc.format);
	tdesc.width			= desc.width;
	tdesc.height		= desc.height;
	tdesc.depth			= desc.depth;
	tdesc.filter		= SampleRenderer::RendererTexture::FILTER_LINEAR;
	tdesc.addressingU	= SampleRenderer::RendererTexture::ADDRESSING_CLAMP;
	tdesc.addressingV	= SampleRenderer::RendererTexture::ADDRESSING_CLAMP;
	tdesc.addressingW	= SampleRenderer::RendererTexture::ADDRESSING_CLAMP;
	tdesc.numLevels		= 1;

	/*
	#if defined(APEX_CUDA_SUPPORT)
	ibdesc.registerInCUDA = desc.registerInCUDA;
	ibdesc.interopContext = desc.interopContext;
	#endif
	*/
	m_texture = m_renderer.createTexture(tdesc);
	PX_ASSERT(m_texture);
}

/*	bool SampleApexRendererSurfaceBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
		{
#if defined(APEX_CUDA_SUPPORT)
			if (m_Surfacebuffer)
			{
				return	m_Surfacebuffer->getInteropResourceHandle(handle);
			}
			else
			{
				return false;
			}
#else
			return false;
#endif
		}
*/
SampleApexRendererSurfaceBuffer::~SampleApexRendererSurfaceBuffer(void)
{
	if (m_texture)
	{
		m_texture->release();
	}
}

void SampleApexRendererSurfaceBuffer::writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 width, physx::PxU32 height, physx::PxU32 depth)
{
	if (m_texture && width && height && depth)
	{
		RendererTexture::Format format = m_texture->getFormat();
		if (format == RendererTexture::FORMAT_R32F)
		{
			physx::PxU32 pitch = width * srcStride * sizeof(physx::PxF32); //??? correct pitch?
			void* dstData = m_texture->lockLevel(0, pitch);
			PX_ASSERT(dstData);
			if (dstData)
			{
				physx::PxF32* dst = ((physx::PxF32*)dstData) + firstDestElement;
				const physx::PxF32* src = (const physx::PxF32*)srcData;
				for(physx::PxU32 d = 0; d < depth; d++)
					for(physx::PxU32 z = 0; z < height; z++)
						for(physx::PxU32 x = 0; x < width; x++)
						{
							*dst = *((physx::PxF32*)src);
							++dst;
							src += srcStride;
						}
			}
			m_texture->unlockLevel(0);
		}
		else
		{
			PX_ASSERT(0);
		}

	}
}


/*******************************
* SampleApexRendererBoneBuffer *
*******************************/

SampleApexRendererBoneBuffer::SampleApexRendererBoneBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderBoneBufferDesc& desc) :
	m_renderer(renderer)
{
	m_maxBones = desc.maxBones;
	m_bones    = new physx::PxMat44[m_maxBones];
}

SampleApexRendererBoneBuffer::~SampleApexRendererBoneBuffer(void)
{
	if (m_bones)
	{
		delete [] m_bones;
	}
}

void SampleApexRendererBoneBuffer::writeBuffer(const physx::apex::NxApexRenderBoneBufferData& data, physx::PxU32 firstBone, physx::PxU32 numBones)
{
	const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(physx::apex::NxRenderBoneSemantic::POSE);
	if (semanticData.data && m_bones)
	{
		const void* srcData = semanticData.data;
		const physx::PxU32 srcStride = semanticData.stride;
		for (physx::PxU32 i = 0; i < numBones; i++)
		{
			// the bones are stored in PxMat34Legacy format -> PxMat44
			m_bones[firstBone + i] = physx::PxMat44(*(const physx::general_shared3::PxMat34Legacy*)srcData);
			srcData = ((physx::PxU8*)srcData) + srcStride;
		}
	}
}


/***********************************
* SampleApexRendererInstanceBuffer *
***********************************/
#if 0
physx::PxF32 lifeRemain[0x10000]; //64k
#endif
SampleApexRendererInstanceBuffer::SampleApexRendererInstanceBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderInstanceBufferDesc& desc)
{
	m_maxInstances   = desc.maxInstances;
	m_instanceBuffer = 0;

	SampleRenderer::RendererInstanceBufferDesc ibdesc;

	ibdesc.hint = RendererInstanceBuffer::HINT_DYNAMIC;
	ibdesc.maxInstances = desc.maxInstances;

	for (physx::PxU32 i = 0; i < RendererInstanceBuffer::NUM_SEMANTICS; i++)
	{
		ibdesc.semanticFormats[i] = RendererInstanceBuffer::NUM_FORMATS;
	}

	for (physx::PxU32 i = 0; i < physx::apex::NxRenderInstanceSemantic::NUM_SEMANTICS; i++)
	{
		// Skip unspecified, but if it IS specified, IGNORE the specification and make your own up!! yay!
		if (desc.semanticFormats[i] == physx::apex::NxRenderDataFormat::UNSPECIFIED)
		{
			continue;
		}

		switch (i)
		{
		case physx::apex::NxRenderInstanceSemantic::POSITION:
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_POSITION] = RendererInstanceBuffer::FORMAT_FLOAT3;
			break;
		case physx::apex::NxRenderInstanceSemantic::ROTATION_SCALE:
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_NORMALX] = RendererInstanceBuffer::FORMAT_FLOAT3;
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_NORMALY] = RendererInstanceBuffer::FORMAT_FLOAT3;
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_NORMALZ] = RendererInstanceBuffer::FORMAT_FLOAT3;
			break;
		case physx::apex::NxRenderInstanceSemantic::VELOCITY_LIFE:
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_VELOCITY_LIFE] = RendererInstanceBuffer::FORMAT_FLOAT4;
			break;
		case physx::apex::NxRenderInstanceSemantic::DENSITY:
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_DENSITY] = RendererInstanceBuffer::FORMAT_FLOAT1;
			break;
		case physx::apex::NxRenderInstanceSemantic::UV_OFFSET:
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_UV_OFFSET] = RendererInstanceBuffer::FORMAT_FLOAT2;
			break;
		case physx::apex::NxRenderInstanceSemantic::LOCAL_OFFSET:
			ibdesc.semanticFormats[RendererInstanceBuffer::SEMANTIC_LOCAL_OFFSET] = RendererInstanceBuffer::FORMAT_FLOAT3;
			break;
		}
	}

#if defined(APEX_CUDA_SUPPORT)
	ibdesc.registerInCUDA = desc.registerInCUDA;
	ibdesc.interopContext = desc.interopContext;
#endif
	m_instanceBuffer = renderer.createInstanceBuffer(ibdesc);
}

SampleApexRendererInstanceBuffer::~SampleApexRendererInstanceBuffer(void)
{
	if (m_instanceBuffer)
	{
		m_instanceBuffer->release();
	}
}

void SampleApexRendererInstanceBuffer::writeBuffer(const physx::apex::NxApexRenderInstanceBufferData& data, physx::PxU32 firstInstance, physx::PxU32 numInstances)
{
	for (physx::PxU32 i = 0; i < physx::apex::NxRenderInstanceSemantic::NUM_SEMANTICS; i++)
	{
		physx::apex::NxRenderInstanceSemantic::Enum semantic = (physx::apex::NxRenderInstanceSemantic::Enum)i;
		const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(semantic);
		if (semanticData.data)
		{
			internalWriteBuffer(semantic, semanticData.data, semanticData.stride, firstInstance, numInstances);
		}
	}
}

bool SampleApexRendererInstanceBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
#if defined(APEX_CUDA_SUPPORT)
	if (m_instanceBuffer)
	{
		return	m_instanceBuffer->getInteropResourceHandle(handle);
	}
	else
	{
		return false;
	}
#else
	CUgraphicsResource* tmp = &handle;
	PX_FORCE_PARAMETER_REFERENCE(tmp);

	return false;
#endif
}

/*	template<typename ElemType>
	void internalWriteSemantic(SampleRenderer::RendererInstanceBuffer::Semantic semantic, const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 numElements)
	{
		physx::PxU32 destStride = 0;
		physx::PxU8* destData = (physx::PxU8*)m_instanceBuffer->lockSemantic(semantic, destStride);
		if (destData)
		{
			destData += firstDestElement * destStride;
			for (physx::PxU32 i = 0; i < numElements; i++)
			{
				ElemType* srcElemPtr = (ElemType*)(((physx::PxU8*)srcData) + srcStride * i);

				*((ElemType*)destData) = *srcElemPtr;

				destData += destStride;
			}
			m_instanceBuffer->unlockSemantic(semantic);
		}
	}
*/

void SampleApexRendererInstanceBuffer::internalWriteBuffer(physx::apex::NxRenderInstanceSemantic::Enum semantic, const void* srcData, physx::PxU32 srcStride,
	                                                       physx::PxU32 firstDestElement, physx::PxU32 numElements)
{
	if (semantic == physx::apex::NxRenderInstanceSemantic::POSITION)
	{
		internalWriteSemantic<physx::PxVec3>(RendererInstanceBuffer::SEMANTIC_POSITION, srcData, srcStride, firstDestElement, numElements);
	}
	else if (semantic == physx::apex::NxRenderInstanceSemantic::ROTATION_SCALE)
	{
		PX_ASSERT(m_instanceBuffer);

		physx::PxU32 xnormalsStride = 0;
		physx::PxU8* xnormals = (physx::PxU8*)m_instanceBuffer->lockSemantic(RendererInstanceBuffer::SEMANTIC_NORMALX, xnormalsStride);
		physx::PxU32 ynormalsStride = 0;
		physx::PxU8* ynormals = (physx::PxU8*)m_instanceBuffer->lockSemantic(RendererInstanceBuffer::SEMANTIC_NORMALY, ynormalsStride);
		physx::PxU32 znormalsStride = 0;
		physx::PxU8* znormals = (physx::PxU8*)m_instanceBuffer->lockSemantic(RendererInstanceBuffer::SEMANTIC_NORMALZ, znormalsStride);

		PX_ASSERT(xnormals && ynormals && znormals);

		if (xnormals && ynormals && znormals)
		{
			xnormals += firstDestElement * xnormalsStride;
			ynormals += firstDestElement * ynormalsStride;
			znormals += firstDestElement * znormalsStride;

			for (physx::PxU32 i = 0; i < numElements; i++)
			{
				physx::PxVec3* p = (physx::PxVec3*)(((physx::PxU8*)srcData) + srcStride * i);

				*((physx::PxVec3*)xnormals) = p[0];
				*((physx::PxVec3*)ynormals) = p[1];
				*((physx::PxVec3*)znormals) = p[2];

				xnormals += xnormalsStride;
				ynormals += ynormalsStride;
				znormals += znormalsStride;
			}
		}
		m_instanceBuffer->unlockSemantic(RendererInstanceBuffer::SEMANTIC_NORMALX);
		m_instanceBuffer->unlockSemantic(RendererInstanceBuffer::SEMANTIC_NORMALY);
		m_instanceBuffer->unlockSemantic(RendererInstanceBuffer::SEMANTIC_NORMALZ);
	}
	else if (semantic == physx::apex::NxRenderInstanceSemantic::VELOCITY_LIFE)
	{
		internalWriteSemantic<physx::PxVec4>(RendererInstanceBuffer::SEMANTIC_VELOCITY_LIFE, srcData, srcStride, firstDestElement, numElements);
	}
	else if (semantic == physx::apex::NxRenderInstanceSemantic::DENSITY)
	{
		internalWriteSemantic<physx::PxF32>(RendererInstanceBuffer::SEMANTIC_DENSITY, srcData, srcStride, firstDestElement, numElements);
	}
	else if (semantic == physx::apex::NxRenderInstanceSemantic::UV_OFFSET)
	{
		internalWriteSemantic<physx::PxVec2>(RendererInstanceBuffer::SEMANTIC_UV_OFFSET, srcData, srcStride, firstDestElement, numElements);
	}
	else if (semantic == physx::apex::NxRenderInstanceSemantic::LOCAL_OFFSET)
	{
		internalWriteSemantic<physx::PxVec3>(RendererInstanceBuffer::SEMANTIC_LOCAL_OFFSET, srcData, srcStride, firstDestElement, numElements);
	}
}

#if USE_RENDER_SPRITE_BUFFER

/*********************************
* SampleApexRendererSpriteBuffer *
*********************************/

SampleApexRendererSpriteBuffer::SampleApexRendererSpriteBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderSpriteBufferDesc& desc) :
	m_renderer(renderer)
{
	m_vertexbuffer = 0;
	SampleRenderer::RendererVertexBufferDesc vbdesc;
	vbdesc.hint = convertFromApexVB(desc.hint);
#if defined(APEX_CUDA_SUPPORT)
	vbdesc.registerInCUDA = desc.registerInCUDA;
	vbdesc.interopContext = desc.interopContext;
#endif
	for (physx::PxU32 i = 0; i < physx::apex::NxRenderSpriteSemantic::NUM_SEMANTICS; i++)
	{
		RendererVertexBuffer::Semantic semantic = convertFromApexSB((physx::apex::NxRenderSpriteSemantic::Enum)i);
		RendererVertexBuffer::Format   format   = convertFromApexVB(desc.semanticFormats[i]);
		if (semantic < RendererVertexBuffer::NUM_SEMANTICS && format < RendererVertexBuffer::NUM_FORMATS)
		{
			vbdesc.semanticFormats[semantic] = format;
		}
	}
	vbdesc.maxVertices = desc.maxSprites;
	m_vertexbuffer = m_renderer.createVertexBuffer(vbdesc);
	PX_ASSERT(m_vertexbuffer);
}

bool SampleApexRendererSpriteBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
#if defined(APEX_CUDA_SUPPORT)
	if (m_vertexbuffer)
	{
		return	m_vertexbuffer->getInteropResourceHandle(handle);
	}
	else
	{
		return false;
	}
#else
	CUgraphicsResource* tmp = &handle;
	PX_FORCE_PARAMETER_REFERENCE(tmp);

	return false;
#endif
}

SampleApexRendererSpriteBuffer::~SampleApexRendererSpriteBuffer(void)
{
	if (m_vertexbuffer)
	{
		m_vertexbuffer->release();
	}
}

void SampleApexRendererSpriteBuffer::flipColors(void* uvData, physx::PxU32 stride, physx::PxU32 num)
{
	for (physx::PxU32 i = 0; i < num; i++)
	{
		physx::PxU8* color = ((physx::PxU8*)uvData) + (stride * i);
		std::swap(color[0], color[3]);
		std::swap(color[1], color[2]);
	}
}

void SampleApexRendererSpriteBuffer::writeBuffer(const physx::apex::NxApexRenderSpriteBufferData& data, physx::PxU32 firstSprite, physx::PxU32 numSprites)
{
	if (!m_vertexbuffer || !numSprites)
	{
		return;
	}

	for (physx::PxU32 i = 0; i < physx::apex::NxRenderSpriteSemantic::NUM_SEMANTICS; i++)
	{
		physx::apex::NxRenderSpriteSemantic::Enum semantic = (physx::apex::NxRenderSpriteSemantic::Enum)i;
		const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(semantic);
		if (semanticData.data)
		{
			const void* srcData = semanticData.data;
			const physx::PxU32 srcStride = semanticData.stride;

			physx::apex::NxRenderVertexSemantic::Enum apexSemantic = physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS;
			switch (semantic)
			{
			case physx::apex::NxRenderSpriteSemantic::COLOR:
				apexSemantic = physx::apex::NxRenderVertexSemantic::COLOR;
				break;
			case physx::apex::NxRenderSpriteSemantic::POSITION:
				apexSemantic = physx::apex::NxRenderVertexSemantic::POSITION;
				break;
			case physx::apex::NxRenderSpriteSemantic::COLOR_FLOAT4:
				apexSemantic = physx::apex::NxRenderVertexSemantic::COLOR;
				break;
			default:
				break;
			}

			if (apexSemantic == physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS)
			{
				continue;
			}

			RendererVertexBuffer::Semantic vbSemantic = convertFromApexVB(apexSemantic);
			if (vbSemantic < RendererVertexBuffer::NUM_SEMANTICS)
			{
				RendererVertexBuffer::Format format = m_vertexbuffer->getFormatForSemantic(vbSemantic);
				physx::PxU32 semanticStride = 0;
				void* dstData = m_vertexbuffer->lockSemantic(vbSemantic, semanticStride);
				void* dstDataCopy = dstData;
				PX_ASSERT(dstData && semanticStride);
				if (dstData && semanticStride)
				{
					dstData = ((physx::PxU8*)dstData) + firstSprite * semanticStride;
					physx::PxU32 formatSize = RendererVertexBuffer::getFormatByteSize(format);
					for (physx::PxU32 j = 0; j < numSprites; j++)
					{
						memcpy(dstData, srcData, formatSize);
						srcData = ((physx::PxU8*)srcData) + srcStride;
						dstData = ((physx::PxU8*)dstData) + semanticStride;
					}

#if defined(RENDERER_PS3)
					// we have to flip the colors
					if (vbSemantic == RendererVertexBuffer::SEMANTIC_COLOR)
					{
						flipColors(dstDataCopy, semanticStride, numSprites);
					}
#endif

					m_vertexbuffer->unlockSemantic(vbSemantic);
				}
			}
		}
	}
}

#endif /* USE_RENDER_SPRITE_BUFFER */


/*************************
* SampleApexRendererMesh *
*************************/
SampleApexRendererMesh::SampleApexRendererMesh(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderResourceDesc& desc) :
	m_renderer(renderer)
{
	m_vertexBuffers     = 0;
	m_numVertexBuffers  = 0;
	m_indexBuffer       = 0;

	m_boneBuffer        = 0;
	m_firstBone         = 0;
	m_numBones          = 0;

	m_instanceBuffer    = 0;

	m_mesh              = 0;

	m_meshTransform = physx::PxMat44::createIdentity();

	m_numVertexBuffers = desc.numVertexBuffers;
	if (m_numVertexBuffers > 0)
	{
		m_vertexBuffers = new SampleApexRendererVertexBuffer*[m_numVertexBuffers];
		for (physx::PxU32 i = 0; i < m_numVertexBuffers; i++)
		{
			m_vertexBuffers[i] = static_cast<SampleApexRendererVertexBuffer*>(desc.vertexBuffers[i]);
		}
	}

	physx::PxU32 numVertexBuffers = m_numVertexBuffers;
#if USE_RENDER_SPRITE_BUFFER
	m_spriteBuffer = static_cast<SampleApexRendererSpriteBuffer*>(desc.spriteBuffer);
	if (m_spriteBuffer)
	{
		numVertexBuffers = 1;
	}
#endif

	RendererVertexBuffer** internalsvbs = 0;
	if (numVertexBuffers > 0)
	{
		internalsvbs    = new RendererVertexBuffer*[numVertexBuffers];

#if USE_RENDER_SPRITE_BUFFER
		if (m_spriteBuffer)
		{
			internalsvbs[0] = m_spriteBuffer->m_vertexbuffer;
		}
		else
#endif
		{
			for (physx::PxU32 i = 0; i < m_numVertexBuffers; i++)
			{
				internalsvbs[i] = m_vertexBuffers[i]->m_vertexbuffer;
			}
		}
	}

	m_indexBuffer    = static_cast<SampleApexRendererIndexBuffer*>(desc.indexBuffer);
	m_boneBuffer     = static_cast<SampleApexRendererBoneBuffer*>(desc.boneBuffer);
	m_instanceBuffer = static_cast<SampleApexRendererInstanceBuffer*>(desc.instanceBuffer);

	m_cullMode       = desc.cullMode;

	SampleRenderer::RendererMeshDesc meshdesc;
	meshdesc.primitives       = convertFromApex(desc.primitives);

	meshdesc.vertexBuffers    = internalsvbs;
	meshdesc.numVertexBuffers = numVertexBuffers;

#if USE_RENDER_SPRITE_BUFFER
	// the sprite buffer currently uses a vb
	if (m_spriteBuffer)
	{
		meshdesc.firstVertex      = desc.firstSprite;
		meshdesc.numVertices      = desc.numSprites;
	}
	else
#endif
	{
		meshdesc.firstVertex      = desc.firstVertex;
		meshdesc.numVertices      = desc.numVerts;
	}

	{
		if (m_indexBuffer != 0)
		{
			meshdesc.indexBuffer      = m_indexBuffer->m_indexbuffer;
			meshdesc.firstIndex       = desc.firstIndex;
			meshdesc.numIndices       = desc.numIndices;
		}
	}

	meshdesc.instanceBuffer   = m_instanceBuffer ? m_instanceBuffer->m_instanceBuffer : 0;
	meshdesc.firstInstance    = desc.firstInstance;
	meshdesc.numInstances     = desc.numInstances;
	m_mesh = m_renderer.createMesh(meshdesc);
	PX_ASSERT(m_mesh);
	if (m_mesh)
	{
		m_meshContext.mesh      = m_mesh;
		m_meshContext.transform = &m_meshTransform;
	}

#if USE_RENDER_SPRITE_BUFFER
	// the sprite buffer currently uses a vb
	if (m_spriteBuffer)
	{
		setVertexBufferRange(desc.firstSprite, desc.numSprites);
	}
	else
#endif
	{
		setVertexBufferRange(desc.firstVertex, desc.numVerts);
	}
	setIndexBufferRange(desc.firstIndex, desc.numIndices);
	setBoneBufferRange(desc.firstBone, desc.numBones);
	setInstanceBufferRange(desc.firstInstance, desc.numInstances);

	setMaterial(desc.material);

	if (internalsvbs)
	{
		delete [] internalsvbs;
	}
}

SampleApexRendererMesh::~SampleApexRendererMesh(void)
{
	if (m_mesh)
	{
		m_mesh->release();
	}
	if (m_vertexBuffers)
	{
		delete [] m_vertexBuffers;
	}
}

void SampleApexRendererMesh::setVertexBufferRange(physx::PxU32 firstVertex, physx::PxU32 numVerts)
{
	if (m_mesh)
	{
		m_mesh->setVertexBufferRange(firstVertex, numVerts);
	}
}

void SampleApexRendererMesh::setIndexBufferRange(physx::PxU32 firstIndex, physx::PxU32 numIndices)
{
	if (m_mesh)
	{
		m_mesh->setIndexBufferRange(firstIndex, numIndices);
	}
}

void SampleApexRendererMesh::setBoneBufferRange(physx::PxU32 firstBone, physx::PxU32 numBones)
{
	m_firstBone = firstBone;
	m_numBones  = numBones;
}

void SampleApexRendererMesh::setInstanceBufferRange(physx::PxU32 firstInstance, physx::PxU32 numInstances)
{
	if (m_mesh)
	{
		m_mesh->setInstanceBufferRange(firstInstance, numInstances);
	}
}

#if !USE_RENDERER_MATERIAL
void SampleApexRendererMesh::pickMaterial(SampleRenderer::RendererMeshContext& context, bool hasBones, SampleFramework::SampleMaterialAsset& material, BlendType hasBlending)
{
	// use this if it can't find another
	context.material         = material.getMaterial(0);
	context.materialInstance = material.getMaterialInstance(0);

	// try to find a better one
	for (size_t i = 0; i < material.getNumVertexShaders(); i++)
	{
		if ((material.getMaxBones(i) > 0) == hasBones &&
			(BLENDING_ANY == hasBlending || material.getMaterial(i)->getBlending() == (BLENDING_ENABLED == hasBlending)))
		{
			context.material         = material.getMaterial(i);
			context.materialInstance = material.getMaterialInstance(i);
			break;
		}
	}
	RENDERER_ASSERT(context.material, "Material has wrong vertex buffers!")
}
#endif

void SampleApexRendererMesh::setMaterial(void* material, BlendType hasBlending)
{
	if (material)
	{
#if USE_RENDERER_MATERIAL
		m_meshContext.materialInstance = static_cast<SampleRenderer::RendererMaterialInstance*>(material);
		m_meshContext.material = &m_meshContext.materialInstance->getMaterial();
#else
		SampleFramework::SampleMaterialAsset& materialAsset = *static_cast<SampleFramework::SampleMaterialAsset*>(material);

		pickMaterial(m_meshContext, m_boneBuffer != NULL, materialAsset, hasBlending);
#endif

#if USE_RENDER_SPRITE_BUFFER
		// get sprite shader variables
		if (m_spriteBuffer)
		{
			m_spriteShaderVariables[0] = m_meshContext.materialInstance->findVariable("windowWidth", SampleRenderer::RendererMaterial::VARIABLE_FLOAT);
			m_spriteShaderVariables[1] = m_meshContext.materialInstance->findVariable("particleSize", SampleRenderer::RendererMaterial::VARIABLE_FLOAT);
		}
#endif
	}
	else
	{
		m_meshContext.material         = 0;
		m_meshContext.materialInstance = 0;
	}
}

void SampleApexRendererMesh::setScreenSpace(bool ss)
{
	m_meshContext.screenSpace = ss;
}

void SampleApexRendererMesh::render(const physx::apex::NxApexRenderContext& context, bool forceWireframe, SampleFramework::SampleMaterialAsset* overrideMaterial)
{
	if (m_mesh && m_meshContext.mesh && m_mesh->getNumVertices() > 0)
	{
#if USE_RENDER_SPRITE_BUFFER
		// set default sprite shader variables
		if (m_spriteBuffer)
		{
			// windowWidth
			if (m_spriteShaderVariables[0] && m_meshContext.materialInstance)
			{
				physx::PxU32 width, height;
				m_renderer.getWindowSize(width, height);
				physx::PxF32 fwidth = (physx::PxF32)width;
				m_meshContext.materialInstance->writeData(*m_spriteShaderVariables[0], &fwidth);
			}

			// particleSize --- we're not going to do this one for now, it's stored in the material xml file
#if 0
			if (m_spriteShaderVariables[1] && m_meshContext.materialInstance)
			{
				physx::PxF32 fsize = 0.10f;
				m_meshContext.materialInstance->writeData(*m_spriteShaderVariables[1], &fsize);
			}
#endif
		}
#endif /* #if USE_RENDER_SPRITE_BUFFER */

		m_meshTransform = context.local2world;
		if (m_boneBuffer)
		{
			if (m_numBones == 1 && 0)
			{
				// APEX really should do this for us... but it doesn't yet...
				m_meshContext.boneMatrices = 0;
				m_meshContext.numBones     = 0;
				m_meshTransform = m_boneBuffer->m_bones[0];
			}
			else
			{
				m_meshContext.boneMatrices = m_boneBuffer->m_bones + m_firstBone;
				m_meshContext.numBones     = m_numBones;
			}
		}
		switch (m_cullMode)
		{
		case physx::apex::NxRenderCullMode::CLOCKWISE:
			m_meshContext.cullMode = SampleRenderer::RendererMeshContext::CLOCKWISE;
			break;
		case physx::apex::NxRenderCullMode::COUNTER_CLOCKWISE:
			m_meshContext.cullMode = SampleRenderer::RendererMeshContext::COUNTER_CLOCKWISE;
			break;
		case physx::apex::NxRenderCullMode::NONE:
			m_meshContext.cullMode = SampleRenderer::RendererMeshContext::NONE;
			break;
		default:
			PX_ASSERT(0 && "Invalid Cull Mode");
		}
		m_meshContext.screenSpace = context.isScreenSpace;

		SampleRenderer::RendererMeshContext tmpContext = m_meshContext;
		if (forceWireframe)
		{
			tmpContext.fillMode = SampleRenderer::RendererMeshContext::LINE;
		}
#if !USE_RENDERER_MATERIAL
		if (overrideMaterial != NULL)
		{
			pickMaterial(tmpContext, m_boneBuffer != NULL, *overrideMaterial);
		}
#endif
		m_renderer.queueMeshForRender(tmpContext);
	}
}
