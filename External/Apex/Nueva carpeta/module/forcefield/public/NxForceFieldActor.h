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

#ifndef NX_FORCE_FIELD_ACTOR_H
#define NX_FORCE_FIELD_ACTOR_H

#include "NxApex.h"


namespace physx
{
namespace apex
{


PX_PUSH_PACK_DEFAULT

class NxForceFieldAsset;
class ForceFieldAssetParams;

class NxForceFieldActor : public NxApexActor
{
protected:
	virtual ~NxForceFieldActor() {}

public:
	/**
	Return true if the force field actor is enabled.
	*/
	virtual bool					isEnable() = 0;

	/**
	Disable force field actor. Default status is enable. Can switch it freely.
	A disabled explosion actor still exists there, but has no effect to the scene.
	*/
	virtual bool					disable() = 0;

	/**
	Enable force field actor. Default status is enable. Can switch it freely.
	A disabled explosion actor still exists there, but has no effect to the scene.
	*/
	virtual bool					enable() = 0;

	/**
	Gets location and orientation of the force field.
	*/
	virtual physx::PxMat44			getPose() const = 0;

	/**
	Sets location and orientation of the force field.
	*/
	virtual void					setPose(const physx::PxMat44& pose) = 0;

	/**
	Gets the force field actor's scale.
	*/
	virtual physx::PxF32			getScale() const = 0;

	/**
	Sets the force field actor's scale. (0.0f, +inf)
	*/
	virtual void					setScale(physx::PxF32 scale) = 0;

	/**
	Retrieves the name string for the force field actor.
	*/
	virtual const char*				getName() const = 0;

	/**
	Set a name string for the force field actor that can be retrieved with getName().
	*/
	virtual void					setName(const char* name) = 0;

	/**
	Set strength for the force field actor.
	*/
	virtual void					setStrength(const physx::PxF32 strength) = 0;

	/**
	Set lifetime for the force field actor.
	*/
	virtual void					setLifetime(const physx::PxF32 lifetime) = 0;

	/**
	Set falloff type (linear, steep, scurve, custom, none) for the force field actor.
	*/
	virtual void					setFalloffType(const char* type) = 0;

	/**
	Set falloff multiplier for the force field actor.
	*/
	virtual void					setFalloffMultiplier(const physx::PxF32 multiplier) = 0;

	/**
	Returns the asset the actor has been created from.
	*/
	virtual NxForceFieldAsset* 	    getForceFieldAsset() const = 0;

#if NX_SDK_VERSION_MAJOR == 3
	/**
	Add filter data
	*/
	virtual void					addFilterData(const physx::PxFilterData& data) = 0;

	/**
	Remove filter data
	*/
	virtual void					removeFilterData(const physx::PxFilterData& data) = 0;

	/**
	Returns pointer to array of filter data
	*/
	virtual void					getFilterData(physx::PxFilterData* data, physx::PxU32& size) = 0;
#endif // NX_SDK_VERSION_MAJOR == 3

};

PX_POP_PACK

} // namespace apex
} // namespace physx

#endif // NX_FORCE_FIELD_ACTOR_H
