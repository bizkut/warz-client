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

#ifndef NX_USER_RENDER_SURFACE_BUFFER_H
#define NX_USER_RENDER_SURFACE_BUFFER_H

/*!
\file
\brief classes NxUserRenderSurfaceBuffer and NxApexRenderSurfaceBufferData
*/

#include "NxUserRenderSurfaceBufferDesc.h"

/**
\brief Cuda graphics resource
*/
typedef struct CUgraphicsResource_st* CUgraphicsResource;

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

/**
\brief surface buffer data
*/
class NxApexRenderSurfaceBufferData {};

/**
\brief Used for storing per-vertex data for rendering.
*/
class NxUserRenderSurfaceBuffer
{
public:
	virtual		~NxUserRenderSurfaceBuffer() {}

	/**
	\brief Called when APEX wants to update the contents of the surface buffer.

	The source data type is assumed to be the same as what was defined in the descriptor.

	\param [in] srcData				Contains the source data for the surface buffer.
	\param [in] srcStride			Contains the source data for the surface buffer.
	\param [in] firstDestElement	first element to start writing to.
	\param [in] width				number of points in U-dimension.
	\param [in] height				number of points in V-dimension.
	\param [in] depth				number of points in W-dimension.
	*/
	virtual void writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 width, physx::PxU32 height, physx::PxU32 depth) = 0;
};

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_USER_RENDER_SURFACE_BUFFER_H
