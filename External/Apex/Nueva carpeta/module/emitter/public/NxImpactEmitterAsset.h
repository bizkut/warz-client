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

#ifndef NX_IMPACT_EMITTER_ASSET_H
#define NX_IMPACT_EMITTER_ASSET_H

#include "NxApex.h"

namespace physx
{
namespace apex
{


PX_PUSH_PACK_DEFAULT

#define NX_IMPACT_EMITTER_AUTHORING_TYPE_NAME "ImpactEmitterAsset"

class NxImpactEmitterActor;
class NxEmitterLodParamDesc;

///Impact emitter asset class
class NxImpactEmitterAsset : public NxApexAsset
{
protected:
	PX_INLINE NxImpactEmitterAsset() {}
	virtual ~NxImpactEmitterAsset() {}

public:
	/// Get the setID that corresponds to the set name \sa NxImpactEmitterActor::registerImpact()
	virtual physx::PxU32			querySetID(const char* setName) = 0;

	/**
	\brief Get a list of the event set names.  The name index in the outSetNames list is the setID.
	\param [in,out] inOutSetNames An array of const char * with at least nameCount
	\param [in,out] nameCount The size of setNames as input and output, set to -1 if setIDs > nameCount
	*/
	virtual void					getSetNames(const char** inOutSetNames, physx::PxU32& nameCount) = 0;
};

///Impact emitter asset authoring. Used to create Impact Emitter assets
class NxImpactEmitterAssetAuthoring : public NxApexAssetAuthoring
{
protected:
	virtual ~NxImpactEmitterAssetAuthoring() {}

public:
};


PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_IMPACT_EMITTER_ASSET_H
