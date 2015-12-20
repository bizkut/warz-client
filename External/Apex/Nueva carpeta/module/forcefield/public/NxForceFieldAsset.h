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

#ifndef NX_FORCE_FIELD_ASSET_H
#define NX_FORCE_FIELD_ASSET_H

#include "NxApex.h"
#include "NxForceFieldPreview.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

#define NX_FORCEFIELD_AUTHORING_TYPE_NAME "ForceFieldAsset"

class NxForceFieldActor;

//----------------
//ForceField Asset
//----------------
class NxForceFieldAsset : public NxApexAsset
{
protected:
	/**
	\brief force field asset default destructor.
	*/
	virtual   ~NxForceFieldAsset() {}

public:
	/**
	\brief returns the default scale of the asset.
	*/
	virtual physx::PxF32		getDefaultScale() const = 0;

	/**
	\brief release an actor created from this asset.
	*/
	virtual void				releaseForceFieldActor(NxForceFieldActor&) = 0;
};

//--------------------
//ForceField Authoring
//--------------------
class NxForceFieldAssetAuthoring : public NxApexAssetAuthoring
{
protected:
	/**
	\brief force field asset authoring default destructor.
	*/
	virtual ~NxForceFieldAssetAuthoring() {}

public:
};


PX_POP_PACK

} // namespace apex
} // namespace physx

#endif // NX_FORCE_FIELD_ASSET_H
