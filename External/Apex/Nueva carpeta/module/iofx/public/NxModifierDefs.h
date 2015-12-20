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

#ifndef NX_MODIFIER_DEFS_H
#define NX_MODIFIER_DEFS_H

#include "NxApexUsingNamespace.h"

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

/**
Roll type of a particle
*/
struct NxApexMeshParticleRollType
{
	enum Enum
	{
		SPHERICAL = 0, ///< roll as a sphere
		CUBIC,		   ///< as a cubical object
		FLAT_X,		   ///< as a flat object (shortest axis is x)
		FLAT_Y,		   ///< as a flat object (shortest axis is y)
		FLAT_Z,		   ///< as a flat object (shortest axis is z)
		LONG_X,		   ///< as a long object (longest axis is x)
		LONG_Y,		   ///< as a long object (longest axis is y)
		LONG_Z,		   ///< as a long object (longest axis is z)
		SPRITE,		   ///< as a sprite

		COUNT
	};
};

/**
Modifiers list
Note: These are serialized to disk, so if you reorder them or change existing modifier types, you
will need to version the stream and map the old values to the new values.
If new values are just appended, no other special care needs to be handled.
*/
enum ModifierTypeEnum
{
	ModifierType_Invalid						= 0,
	ModifierType_Rotation						= 1,
	ModifierType_SimpleScale					= 2,
	ModifierType_RandomScale					= 3,
	ModifierType_ColorVsLife					= 4,
	ModifierType_ColorVsDensity					= 5,
	ModifierType_SubtextureVsLife				= 6,
	ModifierType_OrientAlongVelocity			= 7,
	ModifierType_ScaleAlongVelocity				= 8,
	ModifierType_RandomSubtexture				= 9,
	ModifierType_RandomRotation					= 10,
	ModifierType_ScaleVsLife					= 11,
	ModifierType_ScaleVsDensity					= 12,
	ModifierType_ScaleVsCameraDistance			= 13,
	ModifierType_ViewDirectionSorting			= 14,
	ModifierType_RotationRate					= 15,
	ModifierType_RotationRateVsLife				= 16,
	ModifierType_OrientScaleAlongScreenVelocity	= 17,
	ModifierType_ScaleByMass					= 18,

	ModifierType_Count
};

/**
Stage at which the modifier is applied
*/
enum ModifierStage
{
	ModifierStage_Spawn = 0,		///< at the spawn
	ModifierStage_Continuous = 1,	///< on every frame

	ModifierStage_Count
};

enum ColorChannel
{
	ColorChannel_Red	= 0,
	ColorChannel_Green	= 1,
	ColorChannel_Blue	= 2,
	ColorChannel_Alpha	= 3
};

enum ScaleAxis
{
	ScaleAxis_X			= 0,
	ScaleAxis_Y			= 1,
	ScaleAxis_Z			= 2
};

enum ModifierUsage
{
	ModifierUsage_Spawn			= 0x01,
	ModifierUsage_Continuous	= 0x02,

	ModifierUsage_Sprite		= 0x04,
	ModifierUsage_Mesh			= 0x08,
};

PX_POP_PACK

}
} // namespace physx::apex

#endif // NX_MODIFIER_DEFS_H
