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

#ifndef NX_MODULE_FIELD_SAMPLER_H
#define NX_MODULE_FIELD_SAMPLER_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT


/**
\brief Class for storing 64-bit groups filtering parameters 
	   used for collision filtering between IOSes and Field Samplers.
	   (similar to 128-bit groups filtering system in the 284 PhysX SDK)
*/
class NxGroupsFilteringParams64
{
public:

	/**
	\brief Constructor that configures it such that each of the 64 bits in NxGroupsMasks64 
		   represents a different collision group, and that if two objects interact 
		   (collide) only if there exists at least one group of which they are both members.
		   (Initially DISABLED)
	*/
	PX_INLINE NxGroupsFilteringParams64()
	{
		const0 = const1 = NxGroupsMask64(0, 0);
		op0 = op1 = NxGroupsFilterOp::OR;
		op2 = NxGroupsFilterOp::AND;
		flag = false;
	}

	NxGroupsMask64			const0, const1;
	NxGroupsFilterOp::Enum	op0, op1, op2;
	bool					flag;
};

/**
 \brief FieldSampler module class
 */
class NxModuleFieldSampler : public NxModule
{
protected:
	virtual					~NxModuleFieldSampler() {}

public:
	/**
	\brief Set boundary groups filtering parameters for the specified scene
	*/	
	virtual bool			setFieldBoundaryGroupsFilteringParams(const NxApexScene& apexScene ,
	        const NxGroupsFilteringParams64& params) = 0;

	/**
	\brief Get boundary groups filtering parameters for the specified scene
	*/
	virtual bool			getFieldBoundaryGroupsFilteringParams(const NxApexScene& apexScene ,
	        NxGroupsFilteringParams64& params) const = 0;

	/**
	\brief Set field sampler groups filtering parameters for the specified scene
	*/
	virtual bool			setFieldSamplerGroupsFilteringParams(const NxApexScene& apexScene ,
	        const NxGroupsFilteringParams64& params) = 0;

	/**
	\brief Get field sampler groups filtering parameters for the specified scene
	*/
	virtual bool			getFieldSamplerGroupsFilteringParams(const NxApexScene& apexScene ,
	        NxGroupsFilteringParams64& params) const = 0;

#if NX_SDK_VERSION_MAJOR == 3
	/**
		Set flag to toggle PhysXMonitor for ForceFields.
	*/
	virtual void			enablePhysXMonitor(const NxApexScene& apexScene, bool enable) = 0;
	/**
		Add filter data (collision group) to PhysXMonitor. 
		Only actors with coincided filterData will be proccessed by PhysXMonitor.
	*/
	virtual void			addPhysXMonitorFilterData(const NxApexScene& apexScene, physx::PxFilterData filterData) = 0;
	/**
		Remove filter data (collision group) from PhysXMonitor. 
		Only actors with coincided filterData will be proccessed by PhysXMonitor.
	*/
	virtual void			removePhysXMonitorFilterData(const NxApexScene& apexScene, physx::PxFilterData filterData) = 0;
#endif

#ifdef APEX_TEST
	/**
		Interface for data transfer between tests and module
	*/
	virtual bool setPhysXMonitorParticlesData(const NxApexScene& apexScene, physx::PxU32 numParticles, physx::PxVec4** positions, physx::PxVec4** velocities) = 0;
	virtual void getPhysXMonitorParticlesData(const NxApexScene& apexScene, physx::PxVec4** velocities) = 0;
#endif
};

#if !defined(_USRDLL)
/* If this module is distributed as a static library, the user must call this
 * function before calling NxApexSDK::createModule("FieldSampler")
 */
void instantiateModuleFieldSampler();
#endif

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_MODULE_FIELD_SAMPLER_H
