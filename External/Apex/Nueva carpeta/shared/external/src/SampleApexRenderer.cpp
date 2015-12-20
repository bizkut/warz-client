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
#include <SampleApexRenderer.h>

#include <SampleApexRenderResources.h>

#if !USE_RENDERER_MATERIAL
#include <SampleMaterialAsset.h>
#endif

#include <NxApexRenderContext.h>
#include <NxUserRenderIndexBufferDesc.h>
#include <NxUserRenderInstanceBuffer.h>
#include <NxUserRenderResourceDesc.h>
#include <NxUserRenderBoneBufferDesc.h>
#include <NxUserRenderSpriteBufferDesc.h>
#include <NxUserRenderSurfaceBufferDesc.h>
#include <NxUserRenderVertexBufferDesc.h>

/**********************************
* SampleApexRenderResourceManager *
**********************************/

SampleApexRenderResourceManager::SampleApexRenderResourceManager(SampleRenderer::Renderer& renderer) :
	m_renderer(renderer)
{
	m_numVertexBuffers   = 0;
	m_numIndexBuffers    = 0;
	m_numSurfaceBuffers  = 0;
	m_numBoneBuffers     = 0;
	m_numInstanceBuffers = 0;
	m_numResources       = 0;
}

SampleApexRenderResourceManager::~SampleApexRenderResourceManager(void)
{
	RENDERER_ASSERT(m_numVertexBuffers   == 0, "Not all Vertex Buffers were released prior to Render Resource Manager destruction!");
	RENDERER_ASSERT(m_numIndexBuffers    == 0, "Not all Index Buffers were released prior to Render Resource Manager destruction!");
	RENDERER_ASSERT(m_numSurfaceBuffers  == 0, "Not all Surface Buffers were released prior to Render Resource Manager destruction!");
	RENDERER_ASSERT(m_numBoneBuffers     == 0, "Not all Bone Buffers were released prior to Render Resource Manager destruction!");
	RENDERER_ASSERT(m_numInstanceBuffers == 0, "Not all Instance Buffers were released prior to Render Resource Manager destruction!");
	RENDERER_ASSERT(m_numResources       == 0, "Not all Resources were released prior to Render Resource Manager destruction!");
}

physx::apex::NxUserRenderVertexBuffer* SampleApexRenderResourceManager::createVertexBuffer(const physx::apex::NxUserRenderVertexBufferDesc& desc)
{
	SampleApexRendererVertexBuffer* vb = 0;

	unsigned int numSemantics = 0;
	for (unsigned int i = 0; i < physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS; i++)
	{
		numSemantics += desc.buffersRequest[i] != physx::apex::NxRenderDataFormat::UNSPECIFIED ? 1 : 0;
	}
	PX_ASSERT(desc.isValid());
	if (desc.isValid() && numSemantics > 0)
	{
		vb = new SampleApexRendererVertexBuffer(m_renderer, desc);
		m_numVertexBuffers++;
	}
	return vb;
}

void SampleApexRenderResourceManager::releaseVertexBuffer(physx::apex::NxUserRenderVertexBuffer& buffer)
{
	PX_ASSERT(m_numVertexBuffers > 0);
	m_numVertexBuffers--;
	delete &buffer;
}

physx::apex::NxUserRenderIndexBuffer* SampleApexRenderResourceManager::createIndexBuffer(const physx::apex::NxUserRenderIndexBufferDesc& desc)
{
	SampleApexRendererIndexBuffer* ib = 0;
	PX_ASSERT(desc.isValid());
	if (desc.isValid())
	{
		ib = new SampleApexRendererIndexBuffer(m_renderer, desc);
		m_numIndexBuffers++;
	}
	return ib;
}

void SampleApexRenderResourceManager::releaseIndexBuffer(physx::apex::NxUserRenderIndexBuffer& buffer)
{
	PX_ASSERT(m_numIndexBuffers > 0);
	m_numIndexBuffers--;
	delete &buffer;
}

physx::apex::NxUserRenderSurfaceBuffer* SampleApexRenderResourceManager::createSurfaceBuffer(const physx::apex::NxUserRenderSurfaceBufferDesc& desc)
{
	SampleApexRendererSurfaceBuffer* sb = 0;
	PX_ASSERT(desc.isValid());
	if (desc.isValid())
	{
		sb = new SampleApexRendererSurfaceBuffer(m_renderer, desc);
		m_numSurfaceBuffers++;
	}
	return sb;
}

void SampleApexRenderResourceManager::releaseSurfaceBuffer(physx::apex::NxUserRenderSurfaceBuffer& buffer)
{
	PX_ASSERT(m_numSurfaceBuffers > 0);
	m_numSurfaceBuffers--;
	delete &buffer;
}

physx::apex::NxUserRenderBoneBuffer* SampleApexRenderResourceManager::createBoneBuffer(const physx::apex::NxUserRenderBoneBufferDesc& desc)
{
	SampleApexRendererBoneBuffer* bb = 0;
	PX_ASSERT(desc.isValid());
	if (desc.isValid())
	{
		bb = new SampleApexRendererBoneBuffer(m_renderer, desc);
		m_numBoneBuffers++;
	}
	return bb;
}

void SampleApexRenderResourceManager::releaseBoneBuffer(physx::apex::NxUserRenderBoneBuffer& buffer)
{
	PX_ASSERT(m_numBoneBuffers > 0);
	m_numBoneBuffers--;
	delete &buffer;
}

physx::apex::NxUserRenderInstanceBuffer* SampleApexRenderResourceManager::createInstanceBuffer(const physx::apex::NxUserRenderInstanceBufferDesc& desc)
{
	SampleApexRendererInstanceBuffer* ib = 0;
	PX_ASSERT(desc.isValid());
	if (desc.isValid())
	{
		ib = new SampleApexRendererInstanceBuffer(m_renderer, desc);
		m_numInstanceBuffers++;
	}
	return ib;
}

void SampleApexRenderResourceManager::releaseInstanceBuffer(physx::apex::NxUserRenderInstanceBuffer& buffer)
{
	PX_ASSERT(m_numInstanceBuffers > 0);
	m_numInstanceBuffers--;
	delete &buffer;
}

physx::apex::NxUserRenderSpriteBuffer* SampleApexRenderResourceManager::createSpriteBuffer(const physx::apex::NxUserRenderSpriteBufferDesc& desc)
{
#if USE_RENDER_SPRITE_BUFFER
	SampleApexRendererSpriteBuffer* sb = 0;
	PX_ASSERT(desc.isValid());
	if (desc.isValid())
	{
		// convert SB to VB
		sb = new SampleApexRendererSpriteBuffer(m_renderer, desc);
		m_numVertexBuffers++;
	}
	return sb;
#else
	return NULL;
#endif
}

void  SampleApexRenderResourceManager::releaseSpriteBuffer(physx::apex::NxUserRenderSpriteBuffer& buffer)
{
#if USE_RENDER_SPRITE_BUFFER
	// LRR: for now, just use a VB
	PX_ASSERT(m_numVertexBuffers > 0);
	m_numVertexBuffers--;
	delete &buffer;
#endif
}

physx::apex::NxUserRenderResource* SampleApexRenderResourceManager::createResource(const physx::apex::NxUserRenderResourceDesc& desc)
{
	SampleApexRendererMesh* mesh = 0;
	PX_ASSERT(desc.isValid());
	if (desc.isValid())
	{
		mesh = new SampleApexRendererMesh(m_renderer, desc);
		m_numResources++;
	}
	return mesh;
}

void SampleApexRenderResourceManager::releaseResource(physx::apex::NxUserRenderResource& resource)
{
	PX_ASSERT(m_numResources > 0);
	m_numResources--;
	delete &resource;
}

physx::PxU32 SampleApexRenderResourceManager::getMaxBonesForMaterial(void* material)
{
	if (material != NULL)
	{
		unsigned int maxBones = 0xffffffff;
#if USE_RENDERER_MATERIAL
		// don't yet know if this material even supports bones, but this would be the max...
		maxBones = RENDERER_MAX_BONES;
#else
		SampleFramework::SampleMaterialAsset* materialAsset = static_cast<SampleFramework::SampleMaterialAsset*>(material);
		for (size_t i = 0; i < materialAsset->getNumVertexShaders(); i++)
		{
			unsigned int maxBonesMat = materialAsset->getMaxBones(i);
			if (maxBonesMat > 0)
			{
				maxBones = std::min(maxBones, maxBonesMat);
			}
		}
#endif

		return maxBones != 0xffffffff ? maxBones : 0;
	}
	else
	{
		return 0;
	}
}



void SampleApexRenderResourceManager::setMaterial(physx::apex::NxUserRenderResource& resource, void* material)
{
	static_cast<SampleApexRendererMesh&>(resource).setMaterial(material);
}


/*********************
* SampleApexRenderer *
*********************/

void SampleApexRenderer::renderResource(const physx::apex::NxApexRenderContext& context)
{
	if (context.renderResource)
	{
		static_cast<SampleApexRendererMesh*>(context.renderResource)->render(context, mForceWireframe, mOverrideMaterial);
	}
}
