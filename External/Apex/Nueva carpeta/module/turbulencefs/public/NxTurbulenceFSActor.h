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
#ifndef NX_TURBULENCE_FSACTOR_H
#define NX_TURBULENCE_FSACTOR_H

#include "NxApex.h"
#include "NxApexShape.h"

namespace physx
{
namespace apex
{

struct NxTurbulenceParameters;
class NxTurbulenceFSAsset;


/**
 \brief Turbulence FieldSampler Actor class
 */
class NxTurbulenceFSActor : public NxApexActor, public NxApexRenderable
{
protected:
	virtual ~NxTurbulenceFSActor() {}

public:

	/**
	 \brief Returns the asset the instance has been created from.
	 */
	virtual NxTurbulenceFSAsset* getTurbulenceFSAsset() const = 0;

	///enable/disable the fluid simulation
	virtual void setEnabled(bool enable) = 0;

	///set the velocity of the grid
	virtual void setVelocity(physx::PxVec3 velocity) = 0;

	///multiply the velocity (set by setVelocity) by a multiplier and then clamp it
	virtual void setVelocityScaleAndClamp(PxReal fluidVelocityMultiplier, PxReal fluidVelocityClamp) = 0;

	/**
	get the pose of the grid - this includes only the position and rotation.<BR>
	the position is that of the center of the grid, as is determined as (pose.column3.x, pose.column3.y, pose.column3.z)<BR>
	the rotation is the rotation of the object that the grid is centered on <BR>
	(the grid does not rotate, but we use this pose for rotating the collision obstacle, the jets and imparting angular momentum)
	 */
	virtual physx::PxMat44 getPose() = 0;

	/**
	set the pose of the grid - this includes only the position and rotation.<BR>
	the position is that of the center of the grid, as is determined as (pose.column3.x, pose.column3.y, pose.column3.z)<BR>
	the rotation is the rotation of the object that the grid is centered on <BR>
	(the grid does not rotate, but we use this pose for rotating the collision obstacle, the jets and imparting angular momentum)
	 */
	virtual void setPose(physx::PxMat44 pose) = 0;

	/**
	returns the current pose of the grid.
	*/
	virtual const physx::PxMat44& getPose(void) const = 0;

	///get the grid bounding box min point
	virtual PxVec3 getGridBoundingBoxMin() = 0;
	///get the grid bounding box max point
	virtual PxVec3 getGridBoundingBoxMax() = 0;

	///get the grid size vector
	virtual PxVec3 getGridSize() = 0;

	///set the LOD weights to affect the internal calculation of LOD for the grid
	virtual void setLODWeights(PxReal maxDistance, PxReal distanceWeight, PxReal bias, PxReal benefitBias, PxReal benefitWeight) = 0;

	/**
	enable for more aggressive LOD<BR>
	if set to true then the actor can choose to use less than the available resources if it computes its benefit is not high enough to  need all available resources
	 */
	virtual void setOptimizedLODEnabled(bool enable) = 0;

	/**
	force the current LOD to a particular value, range is 0-1:<BR>
	1.0f is maximum simulation quality<BR>
	0.0f is minimum simulation quality
	 */
	virtual void setCustomLOD(PxReal LOD) = 0;
	///get the current value of the LOD, set either by the APEX LOD system, or by the user using setCustomLOD (if customLOD was enabled by using enableCustomLOD)
	virtual PxReal getCurrentLOD() = 0;
	///use this method to switch between using custom LOD and APEX calculated LOD
	virtual void enableCustomLOD(bool enable) = 0;

	/**
	methods to get the velocity field sampled at grid centers.<BR>
	call setSampleVelocityFieldEnabled(true) to enable the sampling and call getVelocityField to get back the sampled results
	 */
	virtual void getVelocityField(void** x, void** y, void** z, PxU32& sizeX, PxU32& sizeY, PxU32& sizeZ) = 0;
	virtual void setSampleVelocityFieldEnabled(bool enabled) = 0;

	///set a multiplier and a clamp on the total angular velocity induced in the system by the internal collision obstacle or by external collision objects
	virtual void setAngularVelocityMultiplierAndClamp(PxReal angularVelocityMultiplier, PxReal angularVelocityClamp) = 0;

	///set a multiplier and a clamp on the total linear velocity induced in the system by a collision obstacle
	virtual void setLinearVelocityMultiplierAndClamp(PxReal linearVelocityMultiplier, PxReal linearVelocityClamp) = 0;

	/**
	enable whether or not to use heat in the simulation (enabling heat reduces performance).<BR>
	If you are enabling heat then you also need to add temperature sources (without temperature sources you will see no effect of heat on the simulation, except a drop in performance)
	 */
	virtual	void setUseHeat(bool enable) = 0;

	///set heat specific parameters for the simulation
	virtual void setHeatBasedParameters(PxReal forceMultiplier, PxReal ambientTemperature, physx::PxVec3 heatForceDirection) = 0;

	/**
	allows external actors like wind or explosion to add a single directional velocity to the grid.<BR>
	Note that if multiple calls to this function are made only the last call is honored (i.e. the velocities are not accumulated)
	 */
	virtual	void setExternalVelocity(physx::PxVec3 vel) = 0;

	///set a multiplier for the field velocity
	virtual void setFieldVelocityMultiplier(PxReal value) = 0;

	///set a weight for the field velocity
	virtual void setFieldVelocityWeight(PxReal value) = 0;

	///set noise parameters
	virtual void setNoiseParameters(PxF32 noiseStrength, physx::PxVec3 noiseSpacePeriod, PxF32 noiseTimePeriod, PxU32 noiseOctaves) = 0;

	/**
	\brief Set grid auto velocity parameters
	\param [in]		enabled					When enabled, the Grid velocity is automatically calculated from a change of the Grid position in time
	\param [in]		teleportValue			If calculated Grid velocity is equal or greater than this value, then the Grid is assumed to be teleported, and the Grid velocity is kept from the last frame
	\param [in]		impulseClamp			This value defines the maximum magnitude for a change of the Grid velocity, which is helpfull to avoid jerks in the fluid behaviour
	\param [in]		smoothingTimeConstant	This value controls the smoothing of the Grid velocity, it is proportional to a delay in time of the Grid velocity change, set to 0 to disable the smoothing (produce no delay)
	*/
	virtual void setAutoVelocityParameters(bool enabled, PxReal teleportValue, PxReal impulseClamp, PxReal smoothingTimeConstant) = 0;
};

}
} // end namespace physx::apex

#endif // NX_TURBULENCE_FSACTOR_H
