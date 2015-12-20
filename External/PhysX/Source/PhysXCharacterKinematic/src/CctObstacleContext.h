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

#ifndef CCT_OBSTACLE_CONTEXT
#define CCT_OBSTACLE_CONTEXT

/* Exclude from documentation */
/** \cond */

#include "characterkinematic/PxControllerObstacles.h"
#include "PsUserAllocated.h"
#include "PsArray.h"
#include "CmPhysXCommon.h"

namespace physx
{
	struct PxRaycastHit;

namespace Cct
{
	class CharacterControllerManager;

	class ObstacleContext : public PxObstacleContext, public Ps::UserAllocated
	{
		public:
												ObstacleContext(const CharacterControllerManager& );
		virtual									~ObstacleContext();

		// PxObstacleContext
		virtual	void							release();
		virtual	ObstacleHandle					addObstacle(const PxObstacle& obstacle);
		virtual	bool							removeObstacle(ObstacleHandle handle);
		virtual	bool							updateObstacle(ObstacleHandle handle, const PxObstacle& obstacle);
		virtual	PxU32							getNbObstacles()		const;
		virtual	const PxObstacle*				getObstacle(PxU32 i)	const;
		virtual	const PxObstacle*				getObstacleByHandle(ObstacleHandle handle)	const;
		//~PxObstacleContext

static PX_FORCE_INLINE ObstacleHandle			encodeHandle(PxU32 index, PxGeometryType::Enum type)
												{
													PX_ASSERT(index<=0xffff);
													PX_ASSERT(type<=0xffff);
													return (PxU16(index)<<16)|PxU32(type);
												}
static PX_FORCE_INLINE PxGeometryType::Enum		decodeType(ObstacleHandle handle)
												{
													return PxGeometryType::Enum(handle & 0xffff);
												}
static PX_FORCE_INLINE PxU32					decodeIndex(ObstacleHandle handle)
												{
													return handle>>16;
												}

				const PxObstacle*				raycastSingle(PxRaycastHit& hit, const PxVec3& origin, const PxVec3& unitDir, const PxReal distance, ObstacleHandle& obstacleHandle)	const;
				const PxObstacle*				raycastSingle(PxRaycastHit& hit, const ObstacleHandle& obstacleHandle, const PxVec3& origin, const PxVec3& unitDir, const PxReal distance)	const; // raycast just one obstacle handle

				Ps::Array<PxBoxObstacle>		mBoxObstacles;
				Ps::Array<PxCapsuleObstacle>	mCapsuleObstacles;

	private:
		const CharacterControllerManager&		mCCTManager;
	};


} // namespace Cct

}

/** \endcond */
#endif
