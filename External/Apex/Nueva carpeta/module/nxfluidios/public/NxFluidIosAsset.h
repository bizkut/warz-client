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
#ifndef NX_FLUID_IOS_ASSET_H
#define NX_FLUID_IOS_ASSET_H

#include "NxApexDefs.h"
#if NX_SDK_VERSION_MAJOR == 2

#include "NxApex.h"
#include "NxBounds3.h" // required for NxParticleData.h, included by NxFluidDesc.h
#include "fluids/NxFluidDesc.h"
#include <limits.h>

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

#define NX_FLUID_IOS_AUTHORING_TYPE_NAME "NxFluidIosAsset"

/**
 \brief APEX Particle System Asset (PhysX-based)
 */
class NxFluidIosAsset : public NxIosAsset
{
public:
	/**
	 \brief Retrieves the current actor template.

	 If the template is NULL this will return false;
	  otherwise it will fill in dest and return true.
	 */
	virtual bool						getFluidTemplate(NxFluidDescBase& dest) const = 0;
	/// Retrieves the maximum allowed particle number
	virtual physx::PxU32				getMaxParticleCount() const = 0;
	/// Retrieves the radius of a particle
	virtual physx::PxF32				getParticleRadius() const = 0;
	/// Retrieves the name of the collision group
	virtual const char* 				getCollisionGroupName() const = 0;

protected:
	virtual ~NxFluidIosAsset()	{}
};

/**
 \brief APEX Particle System Asset (PhysX-based) Authoring class
 */
class NxFluidIosAssetAuthoring : public NxApexAssetAuthoring
{
protected:
	virtual ~NxFluidIosAssetAuthoring()	{}
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_SDK_VERSION_MAJOR == 2
#endif // NX_FLUID_IOS_ASSET_H
