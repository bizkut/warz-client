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

#ifndef NX_PARTICLE_IOS_ASSET_H
#define NX_PARTICLE_IOS_ASSET_H

#include "NxApex.h"
#include <limits.h>

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

#define NX_PARTICLE_IOS_AUTHORING_TYPE_NAME "ParticleIosAsset"

/**
 \brief APEX Particle System Asset
 */
class NxParticleIosAsset : public NxIosAsset
{
public:
	///Get the radius of a particle
	virtual physx::PxF32						getParticleRadius() const = 0;
	///Get the rest density of a particle
	//virtual physx::PxF32						getRestDensity() const = 0;
	///Get the maximum number of particles that are allowed to be newly created on each frame
	virtual physx::PxF32						getMaxInjectedParticleCount() const	= 0;
	///Get the maximum number of particles that this IOS can simulate
	virtual physx::PxU32						getMaxParticleCount() const = 0;
	///Get the mass of a particle
	virtual physx::PxF32						getParticleMass() const = 0;

protected:
	virtual ~NxParticleIosAsset()	{}
};

/**
 \brief APEX Particle System Asset Authoring class
 */
class NxParticleIosAssetAuthoring : public NxApexAssetAuthoring
{
public:
	///Set the radius of a particle
	virtual void setParticleRadius(physx::PxF32) = 0;
	///Set the rest density of a particle
	//virtual void setRestDensity( physx::PxF32 ) = 0;
	///Set the maximum number of particles that are allowed to be newly created on each frame
	virtual void setMaxInjectedParticleCount(physx::PxF32 count) = 0;
	///Set the maximum number of particles that this IOS can simulate
	virtual void setMaxParticleCount(physx::PxU32 count) = 0;
	///Set the mass of a particle
	virtual void setParticleMass(physx::PxF32) = 0;

	///Set the (NRP) name for the collision group.
	virtual void setCollisionGroupName(const char* collisionGroupName) = 0;
	///Set the (NRP) name for the collision group mask.
	virtual void setCollisionGroupMaskName(const char* collisionGroupMaskName) = 0;

protected:
	virtual ~NxParticleIosAssetAuthoring()	{}
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_PARTICLE_IOS_ASSET_H
