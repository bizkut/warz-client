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

#ifndef NX_MODULE_TURBULENCE_FS_H
#define NX_MODULE_TURBULENCE_FS_H

#include "NxApex.h"
#include "NxApexShape.h"

namespace physx
{
namespace apex
{

class NxTurbulenceFSAsset;
class NxTurbulenceFSAssetAuthoring;

/**
\brief Base class for Turbulence Heat Source.
*/
class NxTurbulenceHeatSource
{
public:
	float mAverageTemperature;
	float mStdTemperature;

	NxTurbulenceHeatSource()
	{
		mAverageTemperature = 0.0f;
		mStdTemperature = 0.0f;
	}
	void setTemperature(float averageTemperature, float stdTemperature)
	{
		mAverageTemperature = averageTemperature;
		mStdTemperature = stdTemperature;
	};
	float getAverageTemperature()
	{
		return mAverageTemperature;
	}
	float getStdTemperature()
	{
		return mStdTemperature;
	}

	///intersect the collision shape against a given AABB
	virtual bool						intersectAgainstAABB(physx::PxBounds3) = 0;

	///If it is a box, cast to box class, return NULL otherwise
	virtual const NxApexBoxShape* 		isBoxShape() const
	{
		return NULL;
	}

	///If it is a sphere, cast to sphere class, return NULL otherwise
	virtual const NxApexSphereShape* 	isSphereShape() const
	{
		return NULL;
	}

	///If it is a box, cast to box class, return NULL otherwise
	virtual NxApexBoxShape* 			getBoxShape()
	{
		return NULL;
	}

	///If it is a sphere, cast to sphere class, return NULL otherwise
	virtual NxApexSphereShape* 			getSphereShape()
	{
		return NULL;
	}

	///If it is a half space, cast to half space class, return NULL otherwise
	//virtual const NxApexHalfSpaceShape *	isHalfSpaceGeom() const	{ return NULL; }

	virtual void release() = 0;

	//virtual void visualize(NxApexRenderDebug *renderer) const = 0;

};

/**
\brief Class for Turbulence Sphere Heat Source.
*/
class NxTurbulenceSphereHeatSource : public NxTurbulenceHeatSource
{

};

/**
\brief Class for Turbulence Box Heat Source.
*/
class NxTurbulenceBoxHeatSource : public NxTurbulenceHeatSource
{

};


/**
\brief Class for TurbulenceFS module.
*/
class NxModuleTurbulenceFS : public NxModule
{
public:
	/// Add heat source to the specified scene
	virtual bool                        addHeatSource(const NxApexScene& apexScene, NxTurbulenceHeatSource* object) = 0;

	/// Remove heat source object from the specified scene
	virtual bool                        removeHeatSource(const NxApexScene& apexScene, NxTurbulenceHeatSource* object) = 0;

	/// Create a new sphere heat source
	virtual NxTurbulenceSphereHeatSource*    createTurbulenceSphereHeatSource() = 0;

	/// Create a new box heat source
	virtual NxTurbulenceBoxHeatSource*    createTurbulenceBoxHeatSource() = 0;

	/// Set custom timestep parameters for the specified scene (only for one simulation call)
	virtual bool                        setCustomTimestep(const NxApexScene& apexScene, float timestep, int numIterations) = 0;

	/// Enable/disable multi-solve feature for the specified scene (disable by default)
	virtual bool                        setMultiSolveEnabled(const NxApexScene& apexScene, bool enabled) = 0;

protected:
	virtual ~NxModuleTurbulenceFS() {}
};

#if !defined(_USRDLL)
/* If this module is distributed as a static library, the user must call this
 * function before calling NxApexSDK::createModule("Turbulence")
 */
void instantiateModuleTurbulenceFS();
#endif

}
} // end namespace physx::apex

#endif // NX_MODULE_TURBULENCE_FS_H
