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
#ifndef NX_WIND_ACTOR_H
#define NX_WIND_ACTOR_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxWindAsset;

/**
\brief The APEX Wind Actor class.  This class creates and manages the force fields to simulate the wind
as specified by the NxWindAsset it was derived from.
*/
class NxWindActor : public NxApexActor, public NxApexActorSource
{
protected:
	virtual ~NxWindActor() {}

public:
	/**
	\brief Returns the asset the instance has been created from.
	*/
	virtual NxWindAsset* getWindAsset() const = 0;

	/**
	\brief Gets the global pose used when the actor is added to the scene, in the NxWindActorDesc
	*/
	virtual physx::PxMat44			getGlobalPose() const = 0;
	/**
	\brief Gets the Wind actor's half length dimensions.
	*/
	virtual physx::PxVec3			getHalfLengthDimensions() const = 0;

	/**
	\brief Gets the groups bit field, used for gust raycasts
	*/
	virtual physx::PxU32			getGustRaycastGroups() const = 0;

	/**
	\brief Set the position of the wind.
	*/
	virtual void			setWindPosition(physx::PxVec3 WindPosition) = 0;

	/**
	\brief Get the position of the wind.
	*/
	virtual physx::PxVec3			getWindPosition(void) = 0;

	/**
	\brief Set the Wind Heading in radians
	*/
	virtual void			setWindHeading(physx::PxF32 Heading) = 0;

	/**
	\brief Get the Wind Heading in radians
	*/
	virtual physx::PxF32			getWindHeading(void) = 0;

	/**
	\brief Set the maximum number of gusts to create per frame.  (default is 5 gusts for each actor per frame)
	*/
	virtual void			setMaxGustsToCreatePerFrame(physx::PxU32 maxGustsPerFrame) = 0;

	/**
	\brief Set the time between gust raycasts in Seconds.
	*/
	virtual void			setTimeBetweenRaycastsGusts(physx::PxF32 raycastTime) = 0;

	/**
	\brief Set the LOD Radius used for activating and deactivating all wind force fields in this actor.
	*/
	virtual void			setLodRadius(physx::PxF32) = 0;
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_WIND_ACTOR_H
