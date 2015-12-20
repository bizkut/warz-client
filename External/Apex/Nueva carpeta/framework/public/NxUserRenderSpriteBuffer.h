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

#ifndef NX_USER_RENDER_SPRITE_BUFFER_H
#define NX_USER_RENDER_SPRITE_BUFFER_H

/*!
\file
\brief classes NxUserRenderSpriteBuffer and NxApexRenderSpriteBufferData
*/

#include "NxApexRenderBufferData.h"
#include "NxUserRenderSpriteBufferDesc.h"

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
\brief Class for storing sprite texture render data
*/
class NxRenderSpriteTextureData
{
public:
	/// Layout
	NxRenderSpriteTextureLayout::Enum layout;
	physx::PxU32 arrayIndex; //!< array index for array textures or cubemap face index
	physx::PxU32 mipLevel;   //!< mipmap level

public:
	PX_INLINE NxRenderSpriteTextureData(void)
	{
		layout = NxRenderSpriteTextureLayout::NONE;
		arrayIndex = 0;
		mipLevel = 0;
	}
};

/**
\brief Sprite buffer data
*/
class NxApexRenderSpriteBufferData : public NxApexRenderBufferData<NxRenderSpriteSemantic, NxRenderSpriteSemantic::Enum>, public NxApexModuleSpecificRenderBufferData
{
};

/**
\brief Used for storing per-sprite instance data for rendering.
*/
class NxUserRenderSpriteBuffer
{
public:
	virtual		~NxUserRenderSpriteBuffer() {}

	/**
	\brief Called when APEX wants to update the contents of the sprite buffer.

	The source data type is assumed to be the same as what was defined in the descriptor.
	APEX should call this function and supply data for ALL semantics that were originally
	requested during creation every time its called.

	\param [in] data				Contains the source data for the sprite buffer.
	\param [in] firstSprite			first sprite to start writing to.
	\param [in] numSprites			number of vertices to write.
	*/
	virtual void writeBuffer(const physx::NxApexRenderSpriteBufferData& data, physx::PxU32 firstSprite, physx::PxU32 numSprites) = 0;

	///Get the low-level handle of the buffer resource
	///\return true if succeeded, false otherwise
	virtual bool getInteropResourceHandle(CUgraphicsResource& handle)
#if NX_APEX_DEFAULT_NO_INTEROP_IMPLEMENTATION
	{
		PX_FORCE_PARAMETER_REFERENCE(&handle);
		return false;
	}
#else
	= 0;
#endif

	/**
	\brief Max number of interop textures
	*/
	static const physx::PxU32 MAX_INTEROP_TEXTURES = 4;


	/**
	\brief Get interop texture usage
	*/
	virtual physx::PxU32 getInteropTextureUsage(NxRenderSpriteTextureData* dataList)
	{
		PX_FORCE_PARAMETER_REFERENCE(dataList);
		return 0;
	}

	/**
	\brief Get interop texture handle list
	*/
	virtual bool getInteropTextureHandleList(CUgraphicsResource* handleList)
	{
		PX_FORCE_PARAMETER_REFERENCE(handleList);
		return false;
	}
};

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_USER_RENDER_SPRITE_BUFFER_H
