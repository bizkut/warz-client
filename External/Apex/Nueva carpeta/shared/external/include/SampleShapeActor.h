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
#ifndef __SAMPLE_SHAPE_ACTOR_H__
#define __SAMPLE_SHAPE_ACTOR_H__

#include "SampleActor.h"

#include "NxApexDefs.h"
#if NX_SDK_VERSION_MAJOR == 2
#include "NxActor.h"
#include "NxScene.h"
#include "NxShape.h"
#include "NxFromPx.h"
#elif NX_SDK_VERSION_MAJOR == 3
#include "PxActor.h"
#include "PxRigidDynamic.h"
#endif

#include "NxApexRenderDebug.h"

#include <Renderer.h>
#include <RendererMeshContext.h>

class SampleShapeActor : public SampleFramework::SampleActor
{
public:
	SampleShapeActor(physx::apex::NxApexRenderDebug* rdebug)
		: mBlockId(-1)
		, mApexRenderDebug(rdebug)
		, mRenderer(NULL)
		, mPhysxActor(NULL)
	{
	}

	virtual ~SampleShapeActor(void)
	{
		if (mApexRenderDebug != NULL)
		{
			mApexRenderDebug->reset(mBlockId);
		}

		if (mPhysxActor)
		{
#if NX_SDK_VERSION_MAJOR == 2
			mPhysxActor->getScene().releaseActor(*mPhysxActor);
#elif NX_SDK_VERSION_MAJOR == 3
			mPhysxActor->release();
#endif
		}
	}

	physx::PxTransform getPose() const
	{
		return physx::PxTransform(mTransform);
	}

	void setPose(const physx::PxTransform& pose)
	{
		mTransform = physx::PxMat44(pose);
		if (mPhysxActor)
		{
#if NX_SDK_VERSION_MAJOR == 2
			NxMat34 mat;
			physx::apex::NxFromPxMat44(mat, mTransform);
			mPhysxActor->setGlobalPose(mat);
#elif NX_SDK_VERSION_MAJOR == 3
			if (physx::PxRigidDynamic* rd = mPhysxActor->isRigidDynamic())
			{
				rd->setGlobalPose(this->convertToPhysicalCoordinates(mTransform));
			}
#endif
		}
		if (mApexRenderDebug != NULL)
		{
			mApexRenderDebug->setDrawGroupPose(mBlockId, mTransform);
		}
	}

#if NX_SDK_VERSION_MAJOR == 2
	void setGroupsMask(const NxGroupsMask& mask)
	{
		const physx::PxU32 numShapes = mPhysxActor->getNbShapes();
		NxShape* const* shapes = mPhysxActor->getShapes();
		for (physx::PxU32 i = 0; i < numShapes; ++i)
		{
			shapes[i]->setGroupsMask(mask);
		}
	}

	virtual void tick(float /*dtime*/, bool /*rewriteBuffers*/ = false)
	{
		if (mPhysxActor && !mPhysxActor->isSleeping())
		{
			physx::apex::PxFromNxMat34(mTransform, mPhysxActor->getGlobalPose());
			if (mApexRenderDebug != NULL)
			{
				mApexRenderDebug->setDrawGroupPose(mBlockId, mTransform);
			}
		}
	}

	NxActor* getPhysXActor()
	{
		return mPhysxActor;
	}

#elif NX_SDK_VERSION_MAJOR == 3

	virtual void tick(float dtime, bool rewriteBuffers = false)
	{
		if (mPhysxActor)
		{
			physx::PxRigidDynamic* rd = mPhysxActor->isRigidDynamic();
			if (rd && !rd->isSleeping())
			{
				mTransform = this->convertToGraphicalCoordinates(rd->getGlobalPose());
				if (mApexRenderDebug != NULL)
				{
					mApexRenderDebug->setDrawGroupPose(mBlockId, mTransform);
				}
			}
		}
	}

	physx::PxActor* getPhysXActor()
	{
		return mPhysxActor;
	}

#endif

	virtual void render(bool /*rewriteBuffers*/ = false)
	{
		if (mRenderer)
		{
			mRenderer->queueMeshForRender(mRendererMeshContext);
		}
	}

protected:
	physx::PxI32						mBlockId;
	physx::apex::NxApexRenderDebug*		mApexRenderDebug;
	SampleRenderer::Renderer*			mRenderer;
	SampleRenderer::RendererMeshContext	mRendererMeshContext;
	physx::PxMat44						mTransform; 
#if NX_SDK_VERSION_MAJOR == 2
	NxActor*							mPhysxActor;
#elif NX_SDK_VERSION_MAJOR == 3
	physx::PxActor*						mPhysxActor;
#endif

private:
	virtual physx::PxMat44		convertToGraphicalCoordinates(const physx::PxTransform & physicsPose) const
	{
		return physx::PxMat44(physicsPose);
	}

	virtual physx::PxTransform	convertToPhysicalCoordinates(const physx::PxMat44 & graphicsPose) const
	{
		return physx::PxTransform(graphicsPose);
	}
};

#endif
