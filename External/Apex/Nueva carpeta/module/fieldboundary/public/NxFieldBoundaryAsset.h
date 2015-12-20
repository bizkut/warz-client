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

#ifndef NX_FIELD_BOUNDARY_ASSET_H
#define NX_FIELD_BOUNDARY_ASSET_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

#define NX_FIELD_BOUNDARY_AUTHORING_TYPE_NAME "FieldBoundaryAsset"

class NxFieldBoundaryActor;

/**
\brief An enum to define the type of the field boundary.
*/
enum NxApexFieldShapeType
{
	NX_APEX_SHAPE_SPHERE  = 0,
	NX_APEX_SHAPE_BOX     = 1,
	NX_APEX_SHAPE_CAPSULE = 2,
	NX_APEX_SHAPE_CONVEX  = 3
};

/**
\brief defines if the field boundary is an include or an exclude field boundary.
*/
enum NxApexFieldBoundaryFlag
{
	NX_APEX_FFB_INCLUDE          = 0,
	NX_APEX_FFB_EXCLUDE          = 1,
};

/**
\brief Descriptor used to create a spherical field boundary.
*/
struct NxApexSphereFieldShapeDesc
{
	PX_INLINE NxApexSphereFieldShapeDesc()
	{
		setToDefault();
	}
	PX_INLINE void setToDefault()
	{
		localPose = PxMat44::createIdentity();
		radius = 1.0f;
	}
	physx::PxMat44	localPose;
	physx::PxF32	radius;
};

/**
\brief Descriptor used to create a box field boundary.
*/
struct NxApexBoxFieldShapeDesc
{
	PX_INLINE NxApexBoxFieldShapeDesc()
	{
		setToDefault();
	}
	PX_INLINE void setToDefault()
	{
		localPose = PxMat44::createIdentity();
		dimensions = physx::PxVec3(1.0f, 1.0f, 1.0f);
	}
	physx::PxMat44	localPose;
	physx::PxVec3	dimensions;
};

/**
\brief Descriptor used to create a capsule field boundary.
*/
struct NxApexCapsuleFieldShapeDesc
{
	PX_INLINE NxApexCapsuleFieldShapeDesc()
	{
		setToDefault();
	}
	PX_INLINE void setToDefault()
	{
		localPose = PxMat44::createIdentity();
		radius = 1.0f;
		height = 1.0f;
	}
	physx::PxMat44	localPose;
	physx::PxF32	radius;
	physx::PxF32	height;
};

/**
\brief Descriptor used to create a convex field boundary.
*/
struct NxApexConvexFieldShapeDesc
{
	/**
	\brief Default constructor for a convex field shape descriptor.
	*/
	PX_INLINE NxApexConvexFieldShapeDesc()
	{
		setToDefault();
	}
	/**
	\brief function to set a convex field shape descriptor to the default values.
	*/
	PX_INLINE void setToDefault()
	{
		localPose = PxMat44::createIdentity();
		flags				= 0;
		numVertices			= 0;
		pointStrideBytes	= 0;
		points				= NULL;
	}
	/**
	\brief The relative pose for this convex.
	*/
	physx::PxMat44	localPose;
	physx::PxU32	flags;
	physx::PxU32	numVertices;
	physx::PxU32	pointStrideBytes;
	const void*		points;
};

/**
\brief The field boundary asset class.
*/
class NxFieldBoundaryAsset : public NxApexAsset
{
protected:
	/**
	\brief field boundary asset default constructor.
	*/
	virtual ~NxFieldBoundaryAsset() {}

public:
	/**
	\brief returns the default scale of the asset.
	*/
	virtual physx::PxVec3			getDefaultScale() const = 0;
	/**
	\brief returns the number of actors created from this asset
	\see NxFieldBoundaryAsset::getActor()
	*/
	virtual physx::PxU32			getActorCount() const = 0;
	/**
	\brief returns an actor created by this asset
	\see NxFieldBoundaryAsset::getActorCount()
	*/
	virtual NxFieldBoundaryActor*	getActor(physx::PxU32 index) const = 0;
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_FIELD_BOUNDARY_ASSET_H
