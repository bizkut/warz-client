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

#ifndef NX_MODULE_IOFX_H
#define NX_MODULE_IOFX_H

#include "NxApex.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

class NxIofxAsset;
class NxIofxAssetAuthoring;
class NxApexRenderVolume;

/**
IOFX Module. This module is used to convert physical parameters of particles into visual parameters
that can be used to render particles.  NxApexRenderVolume provide the ability to partition world space
into separate renderables.
*/
class NxModuleIofx : public NxModule
{
protected:
	virtual							~NxModuleIofx() {}

public:
	/// Disable use of OGL/D3D Interop.  Must be called before emitters are created to have any effect
	virtual void                    disableCudaInterop() = 0;

	/// Disable use of CUDA IOFX modifiers. Must be called before emitters are created to have any effect
	virtual void                    disableCudaModifiers() = 0;

	/** \brief Disables deferred allocation of IOFX actors
	 *
	 * By default, IOFX actors and their render resources will not be
	 * created until it has objects to render.
	 */
	virtual void                    disableDeferredRenderableAllocation() = 0;

	/** \brief Disables triple buffering of interop graphical resources
	 *
	 * By default, when OGL/D3D interop is in use the IOFX module will triple buffer render resources
	 * to allow the game render thread to be detached from the simulation thread.  If the game can
	 * guarantee that NxApexScene::prepareRenderResourceContexts() will be called by the render thread
	 * between NxApexScene::fetchResults() and NxApexScene::simulate() of the next simulation step, it
	 * may call this method to restrict interop to double buffering.  Must be called before any
	 * emitters are created to have any effect.
	 */
	virtual void                    onlyDoubleBufferInteropResources() = 0;

	/** \brief Create a new render volume
	 *
	 * Render volumes may be created at any time, unlike most other APEX
	 * asset and actor types.  Their insertion into the scene is
	 * deferred if the simulation is active.
	 */
	virtual NxApexRenderVolume*     createRenderVolume(const NxApexScene& apexScene, const PxBounds3& b, PxU32 priority, bool allIofx) = 0;

	/** \brief Release a render volume
	 *
	 * Render volumes may be released at any time, unlike most other APEX
	 * asset and actor types.  If the simulation is active, their
	 * resources are released at the end of the simulation step.
	 */
	virtual void                    releaseRenderVolume(NxApexRenderVolume& volume) = 0;
};

#if !defined(_USRDLL)
#define instantiateModuleIOFX	instantiateModuleIofx
void instantiateModuleIofx();
#endif

PX_POP_PACK

}
} // namespace physx::apex

#endif // #ifndef __NX_MODULE_IOFX_H__
