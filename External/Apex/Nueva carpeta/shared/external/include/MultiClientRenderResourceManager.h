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

#ifndef MULTI_CLIENT_RENDER_RESOURCE_MANAGER_H
#define MULTI_CLIENT_RENDER_RESOURCE_MANAGER_H


#include "NxUserRenderResourceManager.h"
#include "NxUserRenderer.h"

#include <vector>


class MultiClientRenderResourceManager : public physx::apex::NxUserRenderResourceManager
{
public:

	MultiClientRenderResourceManager();
	~MultiClientRenderResourceManager();

	void addChild(physx::apex::NxUserRenderResourceManager* rrm, bool destroyAutomatic);


	virtual physx::apex::NxUserRenderVertexBuffer*   createVertexBuffer(const physx::apex::NxUserRenderVertexBufferDesc& desc);
	virtual void                                     releaseVertexBuffer(physx::apex::NxUserRenderVertexBuffer& buffer);

	virtual physx::apex::NxUserRenderIndexBuffer*    createIndexBuffer(const physx::apex::NxUserRenderIndexBufferDesc& desc);
	virtual void                                     releaseIndexBuffer(physx::apex::NxUserRenderIndexBuffer& buffer);

	virtual physx::apex::NxUserRenderBoneBuffer*     createBoneBuffer(const physx::apex::NxUserRenderBoneBufferDesc& desc);
	virtual void                                     releaseBoneBuffer(physx::apex::NxUserRenderBoneBuffer& buffer);

	virtual physx::apex::NxUserRenderInstanceBuffer* createInstanceBuffer(const physx::apex::NxUserRenderInstanceBufferDesc& desc);
	virtual void                                     releaseInstanceBuffer(physx::apex::NxUserRenderInstanceBuffer& buffer);

	virtual physx::apex::NxUserRenderSpriteBuffer*   createSpriteBuffer(const physx::apex::NxUserRenderSpriteBufferDesc& desc);
	virtual void                                     releaseSpriteBuffer(physx::apex::NxUserRenderSpriteBuffer& buffer);

	virtual physx::apex::NxUserRenderResource*       createResource(const physx::apex::NxUserRenderResourceDesc& desc);

	virtual void                                     releaseResource(physx::apex::NxUserRenderResource& resource);

	virtual physx::PxU32                             getMaxBonesForMaterial(void* material);
protected:

	struct Child
	{
		Child(physx::apex::NxUserRenderResourceManager* _rrm, bool destroy) : rrm(_rrm), destroyRrm(destroy) {}

		physx::apex::NxUserRenderResourceManager* rrm;
		bool destroyRrm;
	};

	std::vector<Child> mChildren;
};


class MultiClientUserRenderer : public physx::apex::NxUserRenderer
{
public:
	MultiClientUserRenderer() {}
	virtual ~MultiClientUserRenderer() {}

	void addChild(physx::apex::NxUserRenderer* child);
	virtual void renderResource(const physx::apex::NxApexRenderContext& context);

protected:
	std::vector<physx::apex::NxUserRenderer*> mChildren;
};


#endif // MULTI_CLIENT_RENDER_RESOURCE_MANAGER_H
