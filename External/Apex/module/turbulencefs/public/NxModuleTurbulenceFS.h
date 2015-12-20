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

#ifndef NX_MODULE_TURBULENCE_FS_H
#define NX_MODULE_TURBULENCE_FS_H

#include "NxApex.h"
#include "NxApexShape.h"
#include "NxHeatSourceActor.h"

namespace physx
{
namespace apex
{

class NxTurbulenceFSAsset;
class NxTurbulenceFSAssetAuthoring;


/**
\brief Class for TurbulenceFS module.
*/
class NxModuleTurbulenceFS : public NxModule
{
public:

	/// Set custom timestep parameters for the specified scene (only for one simulation call)
	virtual bool                        setCustomTimestep(const NxApexScene& apexScene, float timestep, int numIterations) = 0;

	/// Enable/disable multi-solve feature for the specified scene (disable by default)
	virtual bool                        setMultiSolveEnabled(const NxApexScene& apexScene, bool enabled) = 0;

protected:
	virtual ~NxModuleTurbulenceFS() {}
};

#if !defined(_USRDLL)
/* If this module is distributed as a static library, the user must call this
 * function before calling NxApexSDK::createModule("Turbulence")
 */
void instantiateModuleTurbulenceFS();
#endif

}
} // end namespace physx::apex

#endif // NX_MODULE_TURBULENCE_FS_H
