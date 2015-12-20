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

#ifndef NX_MODULE_EXAMPLE_FS_H
#define NX_MODULE_EXAMPLE_FS_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxExampleFSAsset;
class NxExampleFSAssetAuthoring;

class NxModuleExampleFSDesc : public NxApexDesc
{
public:

	/**
	\brief Constructor sets to default.
	*/
	PX_INLINE NxModuleExampleFSDesc();

	/**
	\brief (re)sets the structure to the default.
	*/
	PX_INLINE void	setToDefault();

	/**
		Returns true iff an object can be created using this descriptor.
	*/
	PX_INLINE bool	isValid() const;

	/**
		Module configurable parameter.
	*/
	PxU32 moduleValue;
};

PX_INLINE NxModuleExampleFSDesc::NxModuleExampleFSDesc()
{
	setToDefault();
}

PX_INLINE void NxModuleExampleFSDesc::setToDefault()
{
	NxApexDesc::setToDefault();
	moduleValue = 0;
}

PX_INLINE bool NxModuleExampleFSDesc::isValid() const
{
	return NxApexDesc::isValid();
}

class NxModuleExampleFS : public NxModule
{
public:
	virtual void                        init(const NxModuleExampleFSDesc&) = 0;

	virtual PxU32                       getModuleValue() const = 0;

protected:
	virtual ~NxModuleExampleFS() {}
};

#if !defined(_USRDLL)
/* If this module is distributed as a static library, the user must call this
 * function before calling NxApexSDK::createModule("ExampleFS")
 */
void instantiateModuleExampleFS();
#endif

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_MODULE_EXAMPLE_FS_H
