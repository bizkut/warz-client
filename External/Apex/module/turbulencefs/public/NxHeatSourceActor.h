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
#ifndef NX_HEAT_SOURCE_ACTOR_H
#define NX_HEAT_SOURCE_ACTOR_H

#include "NxApex.h"
#include "NxApexShape.h"

namespace physx
{
namespace apex
{

struct NxTurbulenceParameters;
class NxHeatSourceAsset;


/**
 \brief Turbulence HeatSource Actor class
 */
class NxHeatSourceActor : public NxApexActor//, public NxApexRenderable
{
protected:
	virtual ~NxHeatSourceActor() {}

public:
		/**
	 \brief Returns the asset the instance has been created from.
	 */
	virtual NxHeatSourceAsset*			getHeatSourceAsset() const = 0;

	//enable/disable the heating
	virtual void setEnabled(bool enable) = 0;

	virtual bool isEnabled() const = 0;

	///intersect the collision shape against a given AABB
	virtual bool						intersectAgainstAABB(physx::PxBounds3) = 0;

	virtual  NxApexShape* 				getShape() const = 0;

	///If it is a box, cast to box class, return NULL otherwise
	virtual NxApexBoxShape* 			getBoxShape() = 0;

	///If it is a sphere, cast to sphere class, return NULL otherwise
	virtual NxApexSphereShape* 			getSphereShape() = 0;

	virtual  float						getAverageTemperature() const = 0;

	virtual  float						getStdTemperature() const = 0;

	//get the pose of a heat source shape
	virtual physx::PxMat44				getPose() const = 0;

	virtual  void						setTemperature(float averageTemperature, float stdTemperature) = 0;

	//set the pose of a heat source shape
	virtual void						setPose(physx::PxMat44 pose) = 0;

	virtual void						release() = 0;
};


}
} // end namespace physx::apex

#endif 
