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
#ifndef NX_WIND_ASSET_H
#define NX_WIND_ASSET_H

#include "NxApex.h"
#include "NxWindPreview.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

#define NX_WIND_AUTHORING_TYPE_NAME				"WindAsset"

class NxWindActor;

/**
\brief An APEX Wind Asset.  This is deserialized from and a stream and contains all of the static and
shared data needed to create instances of the NxWindPreview and the NxWindActor classes.
*/
class NxWindAsset : public NxApexAsset
{
protected:
	virtual ~NxWindAsset() {}
public:
	/**
	\brief Release an actor instance from this asset.
	*/
	virtual void			releaseWindActor(NxWindActor&) = 0;
	/**
	\brief Release a preview instance from this asset.
	*/
	virtual void			releaseWindPreview(NxWindPreview& preview) = 0;
	/**
	\brief Sets the maximum number of gusts that each wind actor can create per frame.  Default is 5 per frame.
	*/
	virtual void			setMaxGustsToCreatePerFrame(physx::PxU32 maxGustsPerFrame) = 0;
	/**
	\brief Sets the default time between each gust performing a raycast to find the ground.  Default is 1 Second.
	*/
	virtual void			setTimeBetweenRaycastsGusts(physx::PxF32 raycastTime) = 0;
	/**
	\brief Set the Level of Detail (LOD) Radius used for activating and deactivating all wind force fields in the actors created after this call.
	*/
	virtual void			setLodRadius(physx::PxF32) = 0;
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_WIND_ASSET_H
