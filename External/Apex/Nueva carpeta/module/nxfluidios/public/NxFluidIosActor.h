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

#ifndef NX_FLUID_IOS_ACTOR_H
#define NX_FLUID_IOS_ACTOR_H

#include "foundation/Px.h"
#include "NxApexDefs.h"

#if NX_SDK_VERSION_MAJOR == 2

#include "NxApexActor.h"
#include "Nxp.h"
namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxFluidIosAsset;
/**
\brief Fluid IOS Actor. PhysX-based partice system.
 * This actor class does not have any set methods because actor creation will often times
 * result in immmediate NxFluid creation, and most NxFluid parameters are not run time
 * configurable.  Thus all parameters must be specified up front in the asset file.
 */
class NxFluidIosActor : public NxApexActor
{
public:
	/* Get configuration data */
	/// Get the collision group of this fluid actor
	virtual NxCollisionGroup			getCollisionGroup() const = 0;

	/* Get run time data */
	///Get the current particle count
	virtual physx::PxU32				getParticleCount() const = 0;

protected:
	virtual ~NxFluidIosActor()	{}
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_SDK_VERSION_MAJOR

#endif // NX_FLUID_IOS_ACTOR_H
