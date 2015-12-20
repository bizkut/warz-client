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
#ifndef SAMPLE_APEX_RENDERER_H
#define SAMPLE_APEX_RENDERER_H

#include <NxUserRenderer.h>
#include <NxUserRenderResourceManager.h>


namespace SampleRenderer
{
class Renderer;
}

namespace SampleFramework
{
class SampleMaterialAsset;
}


class SampleApexRenderResourceManager : public physx::apex::NxUserRenderResourceManager
{
public:
	SampleApexRenderResourceManager(SampleRenderer::Renderer& renderer);
	virtual								~SampleApexRenderResourceManager(void);

public:
	virtual physx::apex::NxUserRenderVertexBuffer*		createVertexBuffer(const physx::apex::NxUserRenderVertexBufferDesc& desc);
	virtual void										releaseVertexBuffer(physx::apex::NxUserRenderVertexBuffer& buffer);

	virtual physx::apex::NxUserRenderIndexBuffer*		createIndexBuffer(const physx::apex::NxUserRenderIndexBufferDesc& desc);
	virtual void										releaseIndexBuffer(physx::apex::NxUserRenderIndexBuffer& buffer);

	virtual physx::apex::NxUserRenderSurfaceBuffer*		createSurfaceBuffer(const physx::apex::NxUserRenderSurfaceBufferDesc& desc);
	virtual void										releaseSurfaceBuffer(physx::apex::NxUserRenderSurfaceBuffer& buffer);

	virtual physx::apex::NxUserRenderBoneBuffer*		createBoneBuffer(const physx::apex::NxUserRenderBoneBufferDesc& desc);
	virtual void										releaseBoneBuffer(physx::apex::NxUserRenderBoneBuffer& buffer);

	virtual physx::apex::NxUserRenderInstanceBuffer*	createInstanceBuffer(const physx::apex::NxUserRenderInstanceBufferDesc& desc);
	virtual void										releaseInstanceBuffer(physx::apex::NxUserRenderInstanceBuffer& buffer);

	virtual physx::apex::NxUserRenderSpriteBuffer*		createSpriteBuffer(const physx::apex::NxUserRenderSpriteBufferDesc& desc);
	virtual void										releaseSpriteBuffer(physx::apex::NxUserRenderSpriteBuffer& buffer);

	virtual physx::apex::NxUserRenderResource*			createResource(const physx::apex::NxUserRenderResourceDesc& desc);
	virtual void										releaseResource(physx::apex::NxUserRenderResource& resource);

	virtual physx::PxU32								getMaxBonesForMaterial(void* material);

	// change the material of a render resource
	void												setMaterial(physx::apex::NxUserRenderResource& resource, void* material);
protected:
	SampleRenderer::Renderer&	m_renderer;

	physx::PxU32				m_numVertexBuffers;
	physx::PxU32				m_numIndexBuffers;
	physx::PxU32				m_numSurfaceBuffers; //?
	physx::PxU32				m_numBoneBuffers;
	physx::PxU32				m_numInstanceBuffers;
	physx::PxU32				m_numResources;
};

class SampleApexRenderer : public physx::apex::NxUserRenderer
{
public:
	SampleApexRenderer() : mForceWireframe(false), mOverrideMaterial(NULL) {}
	virtual void renderResource(const physx::apex::NxApexRenderContext& context);

	bool mForceWireframe;
	SampleFramework::SampleMaterialAsset* mOverrideMaterial;
};


#endif
