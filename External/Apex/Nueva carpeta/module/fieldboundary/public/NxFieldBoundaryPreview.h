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

#ifndef NX_FIELD_BOUNDARY_PREVIEW_H
#define NX_FIELD_BOUNDARY_PREVIEW_H

#include "NxApex.h"

namespace physx
{
namespace apex
{
PX_PUSH_PACK_DEFAULT

namespace APEX_FIELD_BOUNDARY
{
/**
*/
static const physx::PxU32 FIELD_BOUNDARY_DRAW_ICON = 0x01;
/**
*/
static const physx::PxU32 FIELD_BOUNDARY_DRAW_ICON_BOLD = 0x02;
/**
*/
static const physx::PxU32 FIELD_BOUNDARY_DRAW_BOUNDARIES = 0x04;
} //namespace APEX_FIELD_BOUNDARY

class NxFieldBoundaryPreview : public NxApexAssetPreview
{
public:
	/**
	*/
	virtual void setDetailLevel(physx::PxU32) const = 0;
protected:
	NxFieldBoundaryPreview() {}
};

PX_POP_PACK
} //namespace apex
} //namespace physx

#endif //NX_FIELD_BOUNDARY_PREVIEW_H
