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
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "CctObstacleContext.h"
#include "CctCharacterControllerManager.h"

using namespace physx;
using namespace Cct;


ObstacleContext::ObstacleContext(const CharacterControllerManager& cctMan)
	: mCCTManager(cctMan)
{
}

ObstacleContext::~ObstacleContext()
{
}

void ObstacleContext::release()
{
	delete this;
}

ObstacleHandle ObstacleContext::addObstacle(const PxObstacle& obstacle)
{
	const PxGeometryType::Enum type = obstacle.getType();
	if(type==PxGeometryType::eBOX)
	{
		const PxU32 index = mBoxObstacles.size();
		mBoxObstacles.pushBack(static_cast<const PxBoxObstacle&>(obstacle));
		ObstacleHandle handle = encodeHandle(index, type);
		mCCTManager.onObstacleAdded(handle, this);
		return handle;
	}
	else if(type==PxGeometryType::eCAPSULE)
	{
		const PxU32 index = mCapsuleObstacles.size();
		mCapsuleObstacles.pushBack(static_cast<const PxCapsuleObstacle&>(obstacle));
		ObstacleHandle handle = encodeHandle(index, type);
		mCCTManager.onObstacleAdded(handle, this);
		return handle;
	}
	else return INVALID_OBSTACLE_HANDLE;
}

bool ObstacleContext::removeObstacle(ObstacleHandle handle)
{
	const PxGeometryType::Enum type = decodeType(handle);
	const PxU32 index = decodeIndex(handle);

	if(type==PxGeometryType::eBOX)
	{
		const PxU32 size = mBoxObstacles.size();
		PX_ASSERT(index<size);
		if(index>=size)
			return false;

		mBoxObstacles.replaceWithLast(index);
		mCCTManager.onObstacleRemoved(handle,encodeHandle(size-1, type));
		return true;
	}
	else if(type==PxGeometryType::eCAPSULE)
	{
		const PxU32 size = mCapsuleObstacles.size();
		PX_ASSERT(index<size);
		if(index>=size)
			return false;

		mCapsuleObstacles.replaceWithLast(index);
		mCCTManager.onObstacleRemoved(handle,encodeHandle(size-1, type));
		return true;
	}
	else return false;
}

bool ObstacleContext::updateObstacle(ObstacleHandle handle, const PxObstacle& obstacle)
{
	const PxGeometryType::Enum type = decodeType(handle);
	PX_ASSERT(type==obstacle.getType());
	if(type!=obstacle.getType())
		return false;

	const PxU32 index = decodeIndex(handle);

	if(type==PxGeometryType::eBOX)
	{
		const PxU32 size = mBoxObstacles.size();
		PX_ASSERT(index<size);
		if(index>=size)
			return false;

		mBoxObstacles[index] = static_cast<const PxBoxObstacle&>(obstacle);
		mCCTManager.onObstacleUpdated(handle,this);
		return true;
	}
	else if(type==PxGeometryType::eCAPSULE)
	{
		const PxU32 size = mCapsuleObstacles.size();
		PX_ASSERT(index<size);
		if(index>=size)
			return false;

		mCapsuleObstacles[index] = static_cast<const PxCapsuleObstacle&>(obstacle);
		mCCTManager.onObstacleUpdated(handle,this);
		return true;
	}
	else return false;
}

PxU32 ObstacleContext::getNbObstacles() const
{
	return mBoxObstacles.size() + mCapsuleObstacles.size();
}

const PxObstacle* ObstacleContext::getObstacle(PxU32 i) const
{
	const PxU32 nbBoxes = mBoxObstacles.size();
	if(i<nbBoxes)
		return &mBoxObstacles[i];
	i -= nbBoxes;

	const PxU32 nbCapsules = mCapsuleObstacles.size();
	if(i<nbCapsules)
		return &mCapsuleObstacles[i];

	return NULL;
}

const PxObstacle* ObstacleContext::getObstacleByHandle(ObstacleHandle handle) const
{	
	PxGeometryType::Enum type = decodeType(handle);
	PxU32 index = decodeIndex(handle);

	if(type == PxGeometryType::eBOX)
	{
		PX_ASSERT(index < mBoxObstacles.size());
		return &mBoxObstacles[index];
	}
	else
	{
		PX_ASSERT(index < mCapsuleObstacles.size());
		return &mCapsuleObstacles[index];
	}
}

#include "GuRaycastTests.h"
#include "PxBoxGeometry.h"
#include "PxCapsuleGeometry.h"
#include "PsMathUtils.h"
using namespace Gu;
const PxObstacle* ObstacleContext::raycastSingle(PxRaycastHit& hit, const PxVec3& origin, const PxVec3& unitDir, const PxReal distance, ObstacleHandle& obstacleHandle) const
{
	PxRaycastHit localHit;
	PxF32 t = FLT_MAX;
	const PxObstacle* touchedObstacle = NULL;

	const PxSceneQueryFlags hintFlags = PxSceneQueryFlag::eDISTANCE;

	const PxU32 nbExtraBoxes = mBoxObstacles.size();
	for(PxU32 i=0;i<nbExtraBoxes;i++)
	{
		const PxBoxObstacle& userBoxObstacle = mBoxObstacles[i];

		PxU32 status = raycast_box(	PxBoxGeometry(userBoxObstacle.mHalfExtents),
									PxTransform(toVec3(userBoxObstacle.mPos), userBoxObstacle.mRot),
									origin, unitDir, distance,
									hintFlags,
									1, &localHit, false, NULL, NULL);
		if(status && localHit.distance<t)
		{
			t = localHit.distance;
			hit = localHit;
			obstacleHandle = encodeHandle(i, PxGeometryType::eBOX);
			touchedObstacle = &userBoxObstacle;
		}
	}

	const PxU32 nbExtraCapsules = mCapsuleObstacles.size();
	for(PxU32 i=0;i<nbExtraCapsules;i++)
	{
		const PxCapsuleObstacle& userCapsuleObstacle = mCapsuleObstacles[i];

		PxU32 status = raycast_capsule(	PxCapsuleGeometry(userCapsuleObstacle.mRadius, userCapsuleObstacle.mHalfHeight),
										PxTransform(toVec3(userCapsuleObstacle.mPos), userCapsuleObstacle.mRot),
										origin, unitDir, distance,
										hintFlags,
										1, &localHit, false, NULL, NULL);
		if(status && localHit.distance<t)
		{
			t = localHit.distance;
			hit = localHit;
			obstacleHandle = encodeHandle(i, PxGeometryType::eCAPSULE);
			touchedObstacle = &userCapsuleObstacle;
		}
	}
	return touchedObstacle;
}


const PxObstacle* ObstacleContext::raycastSingle(PxRaycastHit& hit, const ObstacleHandle& obstacleHandle, const PxVec3& origin, const PxVec3& unitDir, const PxReal distance) const
{	
	const PxSceneQueryFlags hintFlags = PxSceneQueryFlag::eDISTANCE;

	PxGeometryType::Enum geomType = decodeType(obstacleHandle);
	if(geomType == PxGeometryType::eBOX)
	{
		const PxBoxObstacle& userBoxObstacle = mBoxObstacles[decodeIndex(obstacleHandle)];

		PxU32 status = raycast_box(	PxBoxGeometry(userBoxObstacle.mHalfExtents),
			PxTransform(toVec3(userBoxObstacle.mPos), userBoxObstacle.mRot),
			origin, unitDir, distance,
			hintFlags,
			1, &hit, false, NULL, NULL);

		if(status)
		{								
			return &userBoxObstacle;
		}
	}
	else
	{
		const PxCapsuleObstacle& userCapsuleObstacle = mCapsuleObstacles[decodeIndex(obstacleHandle)];

		PxU32 status = raycast_capsule(	PxCapsuleGeometry(userCapsuleObstacle.mRadius, userCapsuleObstacle.mHalfHeight),
			PxTransform(toVec3(userCapsuleObstacle.mPos), userCapsuleObstacle.mRot),
			origin, unitDir, distance,
			hintFlags,
			1, &hit, false, NULL, NULL);
		if(status)
		{
			return &userCapsuleObstacle;
		}
	}

	return NULL;
}