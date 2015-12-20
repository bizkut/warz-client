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

#ifndef NX_USER_RENDER_SPRITE_BUFFER_DESC_H
#define NX_USER_RENDER_SPRITE_BUFFER_DESC_H

/*!
\file
\brief class NxUserRenderSpriteBufferDesc, structs NxRenderDataFormat and NxRenderSpriteSemantic
*/

#include "NxApexUsingNamespace.h"
#include "NxApexRenderDataFormat.h"
#include "NxUserRenderResourceManager.h"

namespace physx
{
namespace pxtask
{
class CudaContextManager;
}
};

namespace physx
{
namespace apex
{

PX_PUSH_PACK_DEFAULT

#if !defined(PX_PS4)
	#pragma warning(push)
	#pragma warning(disable:4121)
#endif	//!PX_PS4

/**
\brief potential semantics of a sprite buffer
*/
struct NxRenderSpriteSemantic
{
	/**
	\brief Enum of sprite buffer semantics types
	*/
	enum Enum
	{
		POSITION = 0,	//!< Position of sprite
		COLOR,			//!< Color of sprite
		COLOR_FLOAT4,	//!< Color of sprite with float4 format
		VELOCITY,		//!< Linear velocity of sprite
		SCALE,			//!< Scale of sprite
		LIFE_REMAIN,	//!< 1.0 (new) .. 0.0 (dead)
		DENSITY,		//!< Particle density at sprite location
		SUBTEXTURE,		//!< Sub-texture index of sprite
		ORIENTATION,	//!< 2D sprite orientation (angle in radians, CCW in screen plane)

		USER_DATA,		//!< User data - 32 bits (passed from Emitter)

		NUM_SEMANTICS	//!< Count of semantics, not a valid semantic.
	};

	/**
	\brief Get semantic format
	*/
	static PX_INLINE NxRenderDataFormat::Enum getSemanticFormat(Enum semantic)
	{
		switch (semantic)
		{
		case POSITION:
			return NxRenderDataFormat::FLOAT3;
		case COLOR:
			return NxRenderDataFormat::B8G8R8A8;
		case COLOR_FLOAT4:
			return NxRenderDataFormat::FLOAT4;
		case VELOCITY:
			return NxRenderDataFormat::FLOAT3;
		case SCALE:
			return NxRenderDataFormat::FLOAT3;
		case LIFE_REMAIN:
			return NxRenderDataFormat::FLOAT1;
		case DENSITY:
			return NxRenderDataFormat::FLOAT1;
		case SUBTEXTURE:
			return NxRenderDataFormat::FLOAT1;
		case ORIENTATION:
			return NxRenderDataFormat::FLOAT1;
		case USER_DATA:
			return NxRenderDataFormat::UINT1;
		default:
			PX_ALWAYS_ASSERT();
			return NxRenderDataFormat::NUM_FORMATS;
		}
	}
};

/**
\brief Struct for sprite texture layout info
*/
struct NxRenderSpriteTextureLayout
{
	/**
	\brief Enum of sprite texture layout info
	*/
	enum Enum
	{
		NONE = 0,
		POSITION_FLOAT4, //float4(POSITION.x, POSITION.y, POSITION.z, 1)
		SCALE_ORIENT_SUBTEX_FLOAT4, //float4(SCALE.x, SCALE.y, ORIENTATION, SUBTEXTURE)
		COLOR_BGRA8,
		COLOR_FLOAT4,
	};

	/**
	\brief Get layout format
	*/
	static PX_INLINE NxRenderDataFormat::Enum getLayoutFormat(Enum layout)
	{
		switch (layout)
		{
		case NONE:
			return NxRenderDataFormat::UNSPECIFIED;
		case POSITION_FLOAT4:
			return NxRenderDataFormat::FLOAT4;
		case SCALE_ORIENT_SUBTEX_FLOAT4:
			return NxRenderDataFormat::FLOAT4;
		case COLOR_BGRA8:
			return NxRenderDataFormat::B8G8R8A8;
		case COLOR_FLOAT4:
			return NxRenderDataFormat::R32G32B32A32_FLOAT;
		default:
			PX_ALWAYS_ASSERT();
			return NxRenderDataFormat::NUM_FORMATS;
		}
	}

};

/**
\brief Class for storing sprite texture render data
*/
class NxUserRenderSpriteTextureDesc
{
public:
	NxRenderSpriteTextureLayout::Enum layout;	//!< texture layout
	physx::PxU32 width;							//!< texture width
	physx::PxU32 height;						//!< texture height
	physx::PxU32 pitchBytes;					//!< texture pitch bytes
	physx::PxU32 arrayIndex;					//!< array index for array textures or cubemap face index
	physx::PxU32 mipLevel;						//!< mipmap level

public:
	PX_INLINE NxUserRenderSpriteTextureDesc(void)
	{
		layout = NxRenderSpriteTextureLayout::NONE;
		width = 0;
		height = 0;
		pitchBytes = 0;

		arrayIndex = 0;
		mipLevel = 0;
	}
};

/**
\brief describes the semantics and layout of a sprite buffer
*/
class NxUserRenderSpriteBufferDesc
{
public:
	/**
	\brief Max number of sprite textures
	*/
	static const physx::PxU32 MAX_SPRITE_TEXTURES = 4;

	NxUserRenderSpriteBufferDesc(void)
	{
		maxSprites = 0;
		hint         = NxRenderBufferHint::STATIC;
		registerInCUDA = false;
		interopContext = 0;

		for (physx::PxU32 i = 0; i < NxRenderSpriteSemantic::NUM_SEMANTICS; i++)
		{
			semanticFormats[i] = NxRenderDataFormat::UNSPECIFIED;
			semanticOffsets[i] = physx::PxU32(-1);
		}
		stride = 0;

		textureCount = 0;
	}

	/**
	\brief Checks if dsta is correct
	*/
	bool isValid(void) const
	{
		physx::PxU32 numFailed = 0;

		numFailed += (semanticFormats[NxRenderSpriteSemantic::POSITION] == NxRenderDataFormat::UNSPECIFIED);
		numFailed += registerInCUDA && (interopContext == 0);
		numFailed += registerInCUDA && (stride == 0);
		numFailed += registerInCUDA && (semanticOffsets[NxRenderSpriteSemantic::POSITION] == physx::PxU32(-1));

		return (numFailed == 0);
	}

public:
	physx::PxU32					maxSprites;		//!< The maximum number of sprites that APEX will store in this buffer
	NxRenderBufferHint::Enum		hint;			//!< A hint about the update frequency of this buffer


	/**
	\brief Array of the corresponding formats for each semantic

	NxRenderDataFormat::UNSPECIFIED is used for semantics that are disabled
	*/
	NxRenderDataFormat::Enum		semanticFormats[NxRenderSpriteSemantic::NUM_SEMANTICS];

	/**
	\brief Array of the corresponding offsets (in bytes) for each semantic. Required when CUDA interop is used!
	*/
	physx::PxU32					semanticOffsets[NxRenderSpriteSemantic::NUM_SEMANTICS];

	physx::PxU32					stride; //!< The stride between sprites of this buffer. Required when CUDA interop is used!

	bool							registerInCUDA;  //!< Declare if the resource must be registered in CUDA upon creation

	/**
	This context must be used to register and unregister the resource every time the
	device is lost and recreated.
	*/
	physx::pxtask::CudaContextManager* interopContext;

	physx::PxU32					textureCount;						//!< the number of textures
	NxUserRenderSpriteTextureDesc	textureDescs[MAX_SPRITE_TEXTURES];	//!< an array of texture descriptors
};

#if !defined(PX_PS4)
	#pragma warning(pop)
#endif	//!PX_PS4

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_USER_RENDER_SPRITE_BUFFER_DESC_H
