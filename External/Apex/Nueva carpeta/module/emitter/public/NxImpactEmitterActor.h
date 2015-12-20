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

#ifndef NX_IMPACT_EMITTER_ACTOR_H
#define NX_IMPACT_EMITTER_ACTOR_H

#include "NxApex.h"

namespace physx
{
namespace apex
{


PX_PUSH_PACK_DEFAULT

class NxImpactEmitterAsset;
class NxApexRenderVolume;

///Impact emitter actor.  Emits particles at impact places
class NxImpactEmitterActor : public NxApexActor
{
protected:
	virtual ~NxImpactEmitterActor() {}

public:
	///Gets the pointer to the underlying asset
	virtual NxImpactEmitterAsset* 	getEmitterAsset() const = 0;

	/**
	\brief Registers an impact in the queue

	\param hitPos impact position
	\param hitDir impact direction
	\param surfNorm normal of the surface that is hit by the impact
	\param setID - id for the event set which should be spawned. Specifies the behavior. \sa NxImpactEmitterAsset::querySetID

	*/
	virtual void registerImpact(const physx::PxVec3& hitPos, const physx::PxVec3& hitDir, const physx::PxVec3& surfNorm, physx::PxU32 setID) = 0;

	///Emitted particles are injected to specified render volume on initial frame.
	///Set to NULL to clear the preferred volume.
	virtual void setPreferredRenderVolume(NxApexRenderVolume* volume) = 0;
};


PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_IMPACT_EMITTER_ACTOR_H
