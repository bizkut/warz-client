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

// This define will use RendererMaterial instead of SampleMaterialAsset
#ifndef USE_RENDERER_MATERIAL
#define USE_RENDERER_MATERIAL 0
#endif

#include <NxUserRenderer.h>
#include <NxUserRenderResourceManager.h>

#include <NxApexRenderContext.h>
#include <NxUserRenderBoneBuffer.h>
#include <NxUserRenderIndexBuffer.h>
#include <NxUserRenderInstanceBuffer.h>
#include <NxUserRenderResource.h>
#include <NxUserRenderSpriteBuffer.h>
#include <NxUserRenderSurfaceBuffer.h>
#include <NxUserRenderVertexBuffer.h>

#include <RendererInstanceBuffer.h>
#include <RendererMeshContext.h>

#define	USE_RENDER_SPRITE_BUFFER 1
#if USE_RENDER_SPRITE_BUFFER
#include <RendererMaterial.h>
#endif

namespace SampleRenderer
{
	class Renderer;
	class RendererVertexBuffer;
	class RendererIndexBuffer;
	class RendererMesh;
	class RendererMeshContext;
	class RendererTexture;
}

namespace SampleFramework
{
	class SampleMaterialAsset;
}

using SampleRenderer::RendererVertexBuffer;
using SampleRenderer::RendererIndexBuffer;
using SampleRenderer::RendererInstanceBuffer;
using SampleRenderer::RendererMesh;
using SampleRenderer::RendererTexture;


/*********************************
* SampleApexRendererVertexBuffer *
*********************************/

class SampleApexRendererVertexBuffer : public physx::apex::NxUserRenderVertexBuffer
{
	friend class SampleApexRendererMesh;
public:
	SampleApexRendererVertexBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderVertexBufferDesc& desc);
	virtual ~SampleApexRendererVertexBuffer(void);

	virtual bool getInteropResourceHandle(CUgraphicsResource& handle);

protected:
	void fixUVOrigin(void* uvdata, physx::PxU32 stride, physx::PxU32 num);
	void flipColors(void* uvData, physx::PxU32 stride, physx::PxU32 num);

	virtual void writeBuffer(const physx::apex::NxApexRenderVertexBufferData& data, physx::PxU32 firstVertex, physx::PxU32 numVerts);

	bool writeBufferFastPath(const physx::apex::NxApexRenderVertexBufferData& data, physx::PxU32 firstVertex, physx::PxU32 numVerts);

protected:
	SampleRenderer::Renderer&				m_renderer;
	SampleRenderer::RendererVertexBuffer*	m_vertexbuffer;
	physx::apex::NxTextureUVOrigin::Enum	m_uvOrigin;
};


/********************************
* SampleApexRendererIndexBuffer *
********************************/

class SampleApexRendererIndexBuffer : public physx::apex::NxUserRenderIndexBuffer
{
	friend class SampleApexRendererMesh;
public:
	SampleApexRendererIndexBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderIndexBufferDesc& desc);
	virtual ~SampleApexRendererIndexBuffer(void);

	virtual bool getInteropResourceHandle(CUgraphicsResource& handle);

private:
	virtual void writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 numElements);

private:
	SampleRenderer::Renderer&					m_renderer;
	SampleRenderer::RendererIndexBuffer*		m_indexbuffer;
	physx::apex::NxRenderPrimitiveType::Enum	m_primitives;
};


/********************************
* SampleApexRendererSurfaceBuffer *
********************************/

class SampleApexRendererSurfaceBuffer : public physx::apex::NxUserRenderSurfaceBuffer
{
public:
	SampleApexRendererSurfaceBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderSurfaceBufferDesc& desc);
	virtual ~SampleApexRendererSurfaceBuffer(void);

private:
	virtual void writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 width, physx::PxU32 height, physx::PxU32 depth = 1);

private:
	SampleRenderer::Renderer&					m_renderer;
	SampleRenderer::RendererTexture*			m_texture;
};


/*******************************
* SampleApexRendererBoneBuffer *
*******************************/

class SampleApexRendererBoneBuffer : public physx::apex::NxUserRenderBoneBuffer
{
	friend class SampleApexRendererMesh;
public:
	SampleApexRendererBoneBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderBoneBufferDesc& desc);
	virtual ~SampleApexRendererBoneBuffer(void);

	const physx::PxMat44* getBones() const { return m_bones; }

public:
	virtual void writeBuffer(const physx::apex::NxApexRenderBoneBufferData& data, physx::PxU32 firstBone, physx::PxU32 numBones);

private:
	SampleRenderer::Renderer&	m_renderer;
	physx::PxMat44*				m_bones;
	physx::PxU32				m_maxBones;
};


/***********************************
* SampleApexRendererInstanceBuffer *
***********************************/

class SampleApexRendererInstanceBuffer : public physx::apex::NxUserRenderInstanceBuffer
{
	friend class SampleApexRendererMesh;
public:

	SampleApexRendererInstanceBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderInstanceBufferDesc& desc);
	virtual ~SampleApexRendererInstanceBuffer(void);

	physx::PxU32 getMaxInstances(void) const
	{
		return m_maxInstances;
	}

public:
	virtual void writeBuffer(const physx::apex::NxApexRenderInstanceBufferData& data, physx::PxU32 firstInstance, physx::PxU32 numInstances);

	virtual bool getInteropResourceHandle(CUgraphicsResource& handle);

private:

	template<typename ElemType>
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

	void internalWriteBuffer(physx::apex::NxRenderInstanceSemantic::Enum semantic, const void* srcData, physx::PxU32 srcStride,
	                         physx::PxU32 firstDestElement, physx::PxU32 numElements);

private:
	physx::PxU32							m_maxInstances;
	SampleRenderer::RendererInstanceBuffer*	m_instanceBuffer;
};


#if USE_RENDER_SPRITE_BUFFER

/*********************************
* SampleApexRendererSpriteBuffer *
*********************************/

/*
 *	This class is just a wrapper around the vertex buffer class because there is already
 *	a point sprite implementation using vertex buffers.  It takes the sprite buffer semantics
 *  and just converts them to vertex buffer semantics and ignores everything but position and color.
 *  Well, not really, it takes the lifetime and translates it to color.
 */
class SampleApexRendererSpriteBuffer : public physx::apex::NxUserRenderSpriteBuffer
{
public:
	SampleApexRendererSpriteBuffer(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderSpriteBufferDesc& desc);
	virtual ~SampleApexRendererSpriteBuffer(void);

	virtual bool getInteropResourceHandle(CUgraphicsResource& handle);

private:
	void flipColors(void* uvData, physx::PxU32 stride, physx::PxU32 num);

	virtual void writeBuffer(const physx::apex::NxApexRenderSpriteBufferData& data, physx::PxU32 firstSprite, physx::PxU32 numSprites);

public:
	SampleRenderer::Renderer&             m_renderer;
	SampleRenderer::RendererVertexBuffer* m_vertexbuffer;
};

#endif /* USE_RENDER_SPRITE_BUFFER */


/*************************
* SampleApexRendererMesh *
*************************/

/*
 *	There is some sprite hackery in here now.  Basically, if a sprite buffer is used
 *	we just treat is as a vertex buffer (because it really is a vertex buffer).
 */
class SampleApexRendererMesh : public physx::apex::NxUserRenderResource
{
	friend class SampleApexRenderer;
public:
	SampleApexRendererMesh(SampleRenderer::Renderer& renderer, const physx::apex::NxUserRenderResourceDesc& desc);
	virtual ~SampleApexRendererMesh();

	enum BlendType
	{
		BLENDING_ENABLED = 0,
		BLENDING_DISABLED,
		BLENDING_ANY,
		BLENDING_DEFAULT = BLENDING_ANY
	};

public:
	void setVertexBufferRange(physx::PxU32 firstVertex, physx::PxU32 numVerts);
	void setIndexBufferRange(physx::PxU32 firstIndex, physx::PxU32 numIndices);
	void setBoneBufferRange(physx::PxU32 firstBone, physx::PxU32 numBones);
	void setInstanceBufferRange(physx::PxU32 firstInstance, physx::PxU32 numInstances);

#if USE_RENDER_SPRITE_BUFFER
	void setSpriteBufferRange(physx::PxU32 firstSprite, physx::PxU32 numSprites)
	{
		setVertexBufferRange(firstSprite, numSprites);
	}
#endif

#if !USE_RENDERER_MATERIAL
	static void pickMaterial(SampleRenderer::RendererMeshContext& context, bool hasBones, SampleFramework::SampleMaterialAsset& material, BlendType hasBlending = BLENDING_DEFAULT);
#endif

	virtual void setMaterial(void* material) { setMaterial(material, BLENDING_DEFAULT); }
	void setMaterial(void* material, BlendType hasBlending);
	void setScreenSpace(bool ss);

	physx::PxU32 getNbVertexBuffers() const
	{
		return m_numVertexBuffers;
	}

	physx::apex::NxUserRenderVertexBuffer* getVertexBuffer(physx::PxU32 index) const
	{
		physx::apex::NxUserRenderVertexBuffer* buffer = 0;
		PX_ASSERT(index < m_numVertexBuffers);
		if (index < m_numVertexBuffers)
		{
			buffer = m_vertexBuffers[index];
		}
		return buffer;
	}

	physx::apex::NxUserRenderIndexBuffer* getIndexBuffer() const
	{
		return m_indexBuffer;
	}

	physx::apex::NxUserRenderBoneBuffer* getBoneBuffer()	const
	{
		return m_boneBuffer;
	}

	physx::apex::NxUserRenderInstanceBuffer* getInstanceBuffer()	const
	{
		return m_instanceBuffer;
	}

#if USE_RENDER_SPRITE_BUFFER
	physx::apex::NxUserRenderSpriteBuffer* getSpriteBuffer()	const
	{
		return m_spriteBuffer;
	}
#endif

protected:
	void render(const physx::apex::NxApexRenderContext& context, bool forceWireframe = false, SampleFramework::SampleMaterialAsset* overrideMaterial = NULL);

protected:
	SampleRenderer::Renderer&				m_renderer;

#if USE_RENDER_SPRITE_BUFFER
	SampleApexRendererSpriteBuffer*			m_spriteBuffer;

	// currently this renderer's sprite shaders take 2 variables, particleSize and windowWidth
	const SampleRenderer::RendererMaterial::Variable*  m_spriteShaderVariables[2];
#endif

	SampleApexRendererVertexBuffer**		m_vertexBuffers;
	physx::PxU32							m_numVertexBuffers;

	SampleApexRendererIndexBuffer*			m_indexBuffer;

	SampleApexRendererBoneBuffer*			m_boneBuffer;
	physx::PxU32							m_firstBone;
	physx::PxU32							m_numBones;

	SampleApexRendererInstanceBuffer*		m_instanceBuffer;

	SampleRenderer::RendererMesh*			m_mesh;
	SampleRenderer::RendererMeshContext		m_meshContext;
	physx::PxMat44							m_meshTransform;
	physx::apex::NxRenderCullMode::Enum		m_cullMode;
};
