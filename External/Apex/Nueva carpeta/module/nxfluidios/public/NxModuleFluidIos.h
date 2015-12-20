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
#ifndef NX_MODULE_FLUID_IOS_H
#define NX_MODULE_FLUID_IOS_H

#include "NxApex.h"
#include <limits.h>

class NxCompartment;

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxFluidIosAsset;
class NxFluidIosAssetAuthoring;

/**
\brief APEX Particles module descriptor. Used for initializing the module.
*/
class NxModuleFluidIosDesc : public NxApexDesc
{
public:

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE NxModuleFluidIosDesc() : NxApexDesc()
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
		bool retVal = NxApexDesc::isValid();
		return retVal;
	}

private:

	PX_INLINE void init()
	{
	}
};

/**
\brief APEX FluidIOS module. A particle system based upon PhysX SDK 2.8 particles
*/
class NxModuleFluidIos : public NxModule
{
protected:
	virtual									~NxModuleFluidIos() {}

public:
	/// Initializer. \sa NxModuleFluidIosDesc
	//virtual void							init( const NxModuleFluidIosDesc & moduleFluidIosDesc ) = 0;
	//virtual void init( ::NxParameterized::Interface &desc ) = 0;
	/**
	\brief Sets the compartment that will be used for non-SPH calculations in the given ApexScene.

	\sa NxCompartment
	*/
	virtual void                            setCompartment(const NxApexScene&, NxCompartment& comp) = 0;
	/**
	\brief Gets the compartment that is used for non-SPH calculations in the given ApexScene.

	\sa NxCompartment
	*/
	virtual const NxCompartment*            getCompartment(const NxApexScene&) const = 0;

	/**
	\brief Sets the compartment that will be used for SPH calculations in the given ApexScene.

	If none provided, the compartment for non-SPH calculations will br used
	\sa NxCompartment
	*/
	virtual void                            setSPHCompartment(const NxApexScene&, NxCompartment& comp) = 0;
	/**
	\brief Gets the compartment that is used for SPH calculations in the given ApexScene.

	\sa NxCompartment
	*/
	virtual const NxCompartment*            getSPHCompartment(const NxApexScene&) const = 0;
};

#if !defined(_USRDLL)
#define instantiateModuleFluidIOS	instantiateModuleFluidIos
#define instantiateModuleNxFluidIOS	instantiateModuleFluidIos
#define instantiateModuleNxFluidIos	instantiateModuleFluidIos
void instantiateModuleFluidIos();
#endif

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_MODULE_FLUID_IOS_H
