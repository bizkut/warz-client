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

#ifndef NX_APEX_RENDER_VOLUME_H
#define NX_APEX_RENDER_VOLUME_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxIofxAsset;
class NxIofxActor;

/**
	An object which "owns" a volume of world space.  Any particles which enter the
	owned volume will be migrated to an IOFX actor owned by this object (if the
	particle's IOFX asset is affected by this volume).

	When volumes overlap, their relative priorities break the tie.  If multiple volumes
	have the same priority, the tie breaker goes to the volume that owns the particle.
*/
class NxApexRenderVolume : public NxApexRenderable, public NxApexInterface
{
public:
	/// Returns true if the volume affects all IOFX assets
	virtual bool	getAffectsAllIofx() const = 0;

	/// Adds IOFX asset to volume's list of affected IOFX assets, returns false on failure
	virtual bool    addIofxAsset(NxIofxAsset& iofx) = 0;

	/// Moves the render volume while maintaining its dimensions
	virtual void	setPosition(const PxVec3& pos) = 0;

	/// Directly assigns a new AABB ownership volume
	virtual void    setOwnershipBounds(const PxBounds3& b) = 0;

	/// Retrieves the configured AABB bounds of the volume.  Call getBounds() for the "live" bounds.
	virtual PxBounds3 getOwnershipBounds() const = 0;

	/** \brief Retrieve list of IOFX actors owned by this volume (one per IOFX Asset per IOS actor)
	 *
	 * Returns count of 0 if empty.  Returned pointer is undefined when count is 0.
	 *
	 * The bounds of each of these IOFX is guaranteed to be within the bounds of the volume itself.
	 * Calling the updateRenderResources and dispatchRenderResources() methods of the volume will
	 * implicitly call the same methods of each of these IOFX actors, so there is no need to iterate
	 * over them for rendering purposes, unless you require special logic per IOFX.
	 *
	 * It is not necessary to release these actors, they will be released automatically when their
	 * volume, their IOFX Asset, or their host IOS actor are released.
	 *
	 * This call is not thread-safe.  The returned buffer is only valid until the next APEX API
	 * call that steps the simulation or modifies the number of IOFX actors in a scene.
	 */
	virtual NxIofxActor* const* 	getIofxActorList(PxU32& count) const = 0;

	/** \brief Retrieve list of volume's affected IOFX assets.
	 *
	 * Returns count of 0 if empty or volume affects all IOFX assets.  Returned pointer is
	 * undefined when count is 0.
	 *
	 * The returned buffer not thread-safe, and is only valid until the next APEX API
	 * call that steps the simulation or modifies the number of IOFX assets in a scene.
	 */
	virtual NxIofxAsset* const* 	getIofxAssetList(PxU32& count) const = 0;

	virtual PxVec3					getPosition() const = 0; ///< Returns center of ownership bounds
	virtual PxU32					getPriority() const = 0; ///< Returns priority of volume

	/** \brief Returns true if this volume affects the specified IOFX asset
	 *
	 * Callers must acquire render lock of the volume before calling
	 * this function, for thread safety.
	 */
	virtual bool					affectsIofxAsset(const NxIofxAsset& iofx) const = 0;

protected:
	virtual ~NxApexRenderVolume() {}
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_APEX_RENDER_VOLUME_H
