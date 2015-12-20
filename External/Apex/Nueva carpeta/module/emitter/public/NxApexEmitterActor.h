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

#ifndef NX_APEX_EMITTER_ACTOR_H
#define NX_APEX_EMITTER_ACTOR_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxApexEmitterAsset;
class NxEmitterExplicitGeom;
class NxEmitterLodParamDesc;
class NxApexRenderVolume;

/// Apex emitter actor class. Emits particles whithin a given shape.
class NxApexEmitterActor : public NxApexActor
{
protected:
	virtual ~NxApexEmitterActor() {}

public:
	/**
	\brief Returns the asset the instance has been created from.
	*/
	virtual NxApexEmitterAsset* getEmitterAsset() const = 0;

	/**
	\brief Returns the explicit geometry for THIS ACTOR only
	*/
	virtual NxEmitterExplicitGeom* isExplicitGeom() = 0;

	/**
	\brief Gets the global pose
	*/
	virtual physx::PxMat44	     getGlobalPose() const = 0;
	/**
	\brief Sets the curent pose of the emitter
	*/
	virtual void				 setCurrentPose(const physx::PxMat44& pose) = 0;
	/**
	\brief Sets the curent position of the emitter
	*/
	virtual void				 setCurrentPosition(const physx::PxVec3& pos) = 0;

#if (NX_SDK_VERSION_MAJOR == 2) || defined(DOXYGEN)
	/**
	\brief PhysX SDK 2.8.X.  Attaches the emitter to an actor
	NxActor pointer can be NULL to detach existing actor
	*/
	virtual void				 setAttachActor(NxActor*) = 0;
#endif
#if (NX_SDK_VERSION_MAJOR == 3) || defined(DOXYGEN)
	/**
	\brief PhysX SDK 3.X.  Attaches the emitter to an actor
	PxActor pointer can be NULL to detach existing actor
	*/
	virtual void				 setAttachActor(PxActor*) = 0;
#endif
	/**
	\brief sets the relative pose of the emitter in the space of the actor to which it is attatched
	*/
	virtual void				 setAttachRelativePose(const physx::PxMat44& pose) = 0;

#if (NX_SDK_VERSION_MAJOR == 2) || defined(DOXYGEN)
	/// PhysX SDK 2.8.X.  Retirieves the actor, to which the emitter is attatched. NULL is returned for an unattatched emitter.
	virtual const NxActor* 		 getAttachActor() const = 0;
#endif
#if (NX_SDK_VERSION_MAJOR == 3) || defined(DOXYGEN)
	/// PhysX SDK 3.X. Retirieves the actor, to which the emitter is attatched. NULL is returned for an unattatched emitter.
	virtual const PxActor* 		 getAttachActor() const = 0;
#endif
	/// Retrieves the relative pose of the emitter in the space of the actor to which it is attatched
	virtual const physx::PxMat44 getAttachRelativePose() const = 0;
	///	Retrieves the particle radius
	virtual physx::PxF32         getObjectRadius() const = 0;

	/* Collision filtering settings for overlap tests */
	/// Sets collision groups used to reject particles that overlap the geometry
	virtual void				setOverlapTestCollisionGroups(physx::PxU32) = 0;
#if (NX_SDK_VERSION_MAJOR == 2) || defined(DOXYGEN)
	/// PhysX SDK 2.8.X only.  Sets collision groups mask. \sa NxGroupsMask
	virtual void				setOverlapTestCollisionGroupsMask(NxGroupsMask*) = 0;
#endif

	/// Gets collision groups used to reject particles that overlap the geometry
	virtual physx::PxU32		getOverlapTestCollisionGroups() const = 0;
#if (NX_SDK_VERSION_MAJOR == 2) || defined(DOXYGEN)
	/// PhysX SDK 2.8.X only.  Gets collision groups mask. \sa NxGroupsMask
	virtual const NxGroupsMask*	getOverlapTestCollisionGroupsMask() const = 0;
#endif

	/*
	\brief start emitting particles
	 * If persistent is true, the emitter will emit every frame until stopEmit()is
	 * called.
	 */
	virtual void                 startEmit(bool persistent = true) = 0;
	///stop emitting particles
	virtual void                 stopEmit() = 0;
	///true if the emitter is  emitting particles
	virtual bool                 isEmitting() = 0;

	///	Gets	LOD settings
	virtual const NxEmitterLodParamDesc& getLodParamDesc() const = 0;
	///	Sets	LOD settings
	virtual void				 setLodParamDesc(const NxEmitterLodParamDesc&) = 0;


	/* Override authored scalable parameters, if necessary */
	///Sets the range from which the density of particles whithin the volume is randomly chosen
	virtual void                 setDensityRange(const NxRange<physx::PxF32>&) = 0;
	///Sets the range from which the emition rate is randomly chosen
	virtual void                 setRateRange(const NxRange<physx::PxF32>&) = 0;

	///Sets the range from which the velocity of a particle is randomly chosen
	virtual void                 setVelocityRange(const NxRange<physx::PxVec3>&) = 0;
	///Sets the range from which the lifetime of a particle is randomly chosen
	virtual void                 setLifetimeRange(const NxRange<physx::PxF32>&) = 0;

	///Sets whether or not authored asset particles are emitted
	virtual void				 emitAssetParticles(bool enable) = 0;
	///Gets whether or not authored asset particles are emitted
	virtual bool				 getEmitAssetParticles() const = 0;

	///Emitted particles are injected to specified render volume on initial frame.
	///Set to NULL to clear the preferred volume.
	virtual void                 setPreferredRenderVolume(NxApexRenderVolume* volume) = 0;
};

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_APEX_EMITTER_ACTOR_H
