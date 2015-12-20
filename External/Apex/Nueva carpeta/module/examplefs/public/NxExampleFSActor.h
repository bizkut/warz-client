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

#ifndef NX_EXAMPLE_FSACTOR_H
#define NX_EXAMPLE_FSACTOR_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxExampleFSAsset;

class NxExampleFSActorDesc : public NxApexDesc
{
public:
	physx::PxVec3  position;

	const char*    fieldSamplerFilterDataName;
	const char*    fieldBoundaryFilterDataName;

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE NxExampleFSActorDesc() : NxApexDesc()
	{
		init();
	}

	/**
	\brief sets members to default values.
	*/
	PX_INLINE void setToDefault()
	{
		NxApexDesc::setToDefault();
		init();
	}

	/**
	\brief checks if this is a valid descriptor.
	*/
	PX_INLINE bool isValid() const
	{
		if (!NxApexDesc::isValid())
		{
			return false;
		}

		return true;
	}

private:

	PX_INLINE void init()
	{
		position = physx::PxVec3(0.0f);

		fieldSamplerFilterDataName = 0;
		fieldBoundaryFilterDataName = 0;
	}
};

class NxExampleFSActor : public NxApexActor, public NxApexRenderable
{
protected:
	virtual ~NxExampleFSActor() {}

public:
	/**
	Returns the asset the instance has been created from.
	*/
	virtual NxExampleFSAsset* 	getExampleFSAsset() const = 0;


	/**
	Gets the global position of the actor
	*/
	virtual physx::PxVec3		getPosition() const = 0;

	/**
	Sets the global position of the actor
	*/
	virtual void				setPosition(const physx::PxVec3&) = 0;

};

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_EXAMPLE_FSACTOR_H
