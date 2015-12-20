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
#ifndef __NULL_RENDERER_H_
#define __NULL_RENDERER_H_

#include "NxApex.h"

/* This class is intended for use by command line tools that require an
 * APEX SDK.  Apps which use this renderer should _NOT_ call
 * updateRenderResources() or dispatchRenderResources().  You _WILL_
 * crash.
 */

namespace physx
{
namespace apex
{

class NullRenderResourceManager : public NxUserRenderResourceManager
{
public:
	NxUserRenderVertexBuffer*	createVertexBuffer(const NxUserRenderVertexBufferDesc&)
	{
		return NULL;
	}
	NxUserRenderIndexBuffer*	createIndexBuffer(const NxUserRenderIndexBufferDesc&)
	{
		return NULL;
	}
	NxUserRenderBoneBuffer*		createBoneBuffer(const NxUserRenderBoneBufferDesc&)
	{
		return NULL;
	}
	NxUserRenderInstanceBuffer*	createInstanceBuffer(const NxUserRenderInstanceBufferDesc&)
	{
		return NULL;
	}
	NxUserRenderSpriteBuffer*   createSpriteBuffer(const NxUserRenderSpriteBufferDesc&)
	{
		return NULL;
	}
	NxUserRenderResource*		createResource(const NxUserRenderResourceDesc&)
	{
		return NULL;
	}
	void						releaseVertexBuffer(NxUserRenderVertexBuffer&) {}
	void						releaseIndexBuffer(NxUserRenderIndexBuffer&) {}
	void						releaseBoneBuffer(NxUserRenderBoneBuffer&) {}
	void						releaseInstanceBuffer(NxUserRenderInstanceBuffer&) {}
	void						releaseSpriteBuffer(NxUserRenderSpriteBuffer&) {}
	void						releaseResource(NxUserRenderResource&) {}
	physx::PxU32						getMaxBonesForMaterial(void*)
	{
		return 0;
	}
};

}
} // end namespace physx::apex

#endif
