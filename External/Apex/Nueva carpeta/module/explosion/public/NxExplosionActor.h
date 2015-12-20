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

#ifndef NX_EXPLOSION_ACTOR_H
#define NX_EXPLOSION_ACTOR_H

#include "NxApex.h"
#include "NxFieldBoundaryShared.h"


namespace physx
{
namespace apex
{


PX_PUSH_PACK_DEFAULT

class NxExplosionAsset;
class NxFieldBoundaryActor;
class ExplosionAssetParam;

#if NX_SDK_VERSION_MAJOR == 2
class NxExplosionActor : public NxApexActor, public NxApexActorSource
#elif NX_SDK_VERSION_MAJOR == 3
class NxExplosionActor : public NxApexActor
#endif
{
protected:
	virtual ~NxExplosionActor() {}

public:
	/**
	Return true if the explosion actor is enabled.
	*/
	virtual bool					isEnable() = 0;

	/**
	Enable/disable explosion actor. Default status is enable. Can switch it freely.
	A disabled explosion actor still exists there, but has no effect to the scene.
	*/
	virtual bool					disable() = 0;
	virtual bool					enable() = 0;

	/**
	Returns the NxActor the Explosion has been attached to.
	Returns NULL if the Explosion is static.
	*/
#if NX_SDK_VERSION_MAJOR == 2
	virtual NxActor*				getAttachedNxActor() const = 0;
//#elif NX_SDK_VERSION_MAJOR == 3
//	virtual PxActor*				getAttachedPxActor() const = 0;
#endif

	/**
	Attach Explosion actor to a NxActor
	*/
#if NX_SDK_VERSION_MAJOR == 2
	virtual void					setAttachedNxActor(NxActor* actor) = 0;
//#elif NX_SDK_VERSION_MAJOR == 3
//	virtual void					setAttachedPxActor(PxActor* actor) = 0;
#endif

	/**
	Gets location and orientation of the Explosion.
	If the attached NxActor is NULL, this is global pose (relative to the world frame),
	otherwise it is local pose (relative to the actor frame of the NxActor attached).
	*/
	virtual physx::PxMat44			getPose() const = 0;

	/**
	Sets location and orientation of the Explosion.
	If the attached NxActor is NULL, this is global pose (relative to the world frame),
	otherwise it is for local pose (relative to the actor frame of the NxActor attached).
	*/
	virtual void					setPose(const physx::PxMat44& pose) = 0;

	/**
	Gets the explosion actor's scale
	*/
	virtual physx::PxF32			getScale() const = 0;

	/**
	Sets the explosion actor's scale
	*/
	virtual void					setScale(physx::PxF32 scale) = 0;

	/**
	Retrieves the name string for the explosion actor
	*/
	virtual const char*				getName() const = 0;

	/**
	Set a name string for the explosion actor that can be retrieved with getName()
	*/
	virtual void					setName(const char* name) = 0;

	/**
	Adds a field boundary actor to the explosion actor to define its volume of activity.
	*/
#if NX_SDK_VERSION_MAJOR == 2
	virtual void					addFieldBoundary(NxFieldBoundaryActor& bound) = 0;
#endif

	/**
	Removes a field boundary actor from the explosion actor.
	*/
#if NX_SDK_VERSION_MAJOR == 2
	virtual void					removeFieldBoundary(NxFieldBoundaryActor& bound) = 0;
#endif

	/**
	Returns the asset the actor has been created from.
	*/
	virtual NxExplosionAsset* 	    getExplosionAsset() const = 0;


	/*******************************************************************************
	 The next 4 functions usually called under shockwave explosion mode, though they
	 also work for implosion & explosion cases.
	*/

	/**
	Returns the outer radius (far distance) the explosion has been confined.
	*/
#if NX_SDK_VERSION_MAJOR == 2
	virtual physx::PxF32			getOuterBoundRadius() const = 0;
#endif

	/**
	Set the outer radius (far distance) the explosion will be confined.
	*/
#if NX_SDK_VERSION_MAJOR == 2	
	virtual void					setOuterBoundRadius(physx::PxF32) = 0;
#endif

	/**
	Returns the inner radius (near distance) the explosion has been confined.
	*/
#if NX_SDK_VERSION_MAJOR == 2
	virtual physx::PxF32			getInnerBoundRadius() const = 0;
#endif

	/**
	Set the inner radius (near distance) the explosion will be confined.
	*/
#if NX_SDK_VERSION_MAJOR == 2
	virtual void					setInnerBoundRadius(physx::PxF32) = 0;
#endif
};

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_EXPLOSION_ACTOR_H
