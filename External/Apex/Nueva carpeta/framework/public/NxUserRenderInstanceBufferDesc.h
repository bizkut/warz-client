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

#ifndef NX_USER_RENDER_INSTANCE_BUFFER_DESC_H
#define NX_USER_RENDER_INSTANCE_BUFFER_DESC_H

/*!
\file
\brief class NxUserRenderInstanceBufferDesc, structs NxRenderDataFormat and NxRenderInstanceSemantic
*/

#include "NxUserRenderResourceManager.h"
#include "NxApexRenderDataFormat.h"

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

#pragma warning(push)
#pragma warning(disable:4121)

/**
\brief Enumerates the potential instance buffer semantics
*/
struct NxRenderInstanceSemantic
{
	/**
	\brief Enume of the potential instance buffer semantics
	*/
	enum Enum
	{
		POSITION = 0,	//!< position of instance
		ROTATION_SCALE,	//!< rotation matrix and scale baked together
		VELOCITY_LIFE,	//!< velocity & life remain (1.0=new .. 0.0=dead) baked together
		DENSITY,		//!< particle density at instance location
		COLOR,			//!< color of instance
		COLOR_FLOAT4,	//!< color of instance with float4 format
		UV_OFFSET,		//!< an offset to apply to all U,V coordinates
		LOCAL_OFFSET,	//!< the static initial position offset of the instance

		USER_DATA,		//!< User data - 32 bits

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
		case ROTATION_SCALE:
			return NxRenderDataFormat::FLOAT3x3;
		case VELOCITY_LIFE:
			return NxRenderDataFormat::FLOAT4;
		case DENSITY:
			return NxRenderDataFormat::FLOAT1;
		case COLOR:
			return NxRenderDataFormat::B8G8R8A8;
		case COLOR_FLOAT4:
			return NxRenderDataFormat::FLOAT4;
		case UV_OFFSET:
			return NxRenderDataFormat::FLOAT2;
		case LOCAL_OFFSET:
			return NxRenderDataFormat::FLOAT3;
		case USER_DATA:
			return NxRenderDataFormat::UINT1;
		default:
			PX_ALWAYS_ASSERT();
			return NxRenderDataFormat::NUM_FORMATS;
		}
	}
};

/**
\brief Describes the data and layout of an instance buffer
*/
class NxUserRenderInstanceBufferDesc
{
public:
	NxUserRenderInstanceBufferDesc(void)
	{
		registerInCUDA = false;
		interopContext = 0;
		maxInstances = 0;
		hint         = NxRenderBufferHint::STATIC;
		for (physx::PxU32 i = 0; i < NxRenderInstanceSemantic::NUM_SEMANTICS; i++)
		{
			semanticFormats[i] = NxRenderDataFormat::UNSPECIFIED;
			semanticOffsets[i] = physx::PxU32(-1);
		}
		stride = 0;
	}

	/**
	\brief Check if parameter's values are correct
	*/
	bool isValid(void) const
	{
		physx::PxU32 numFailed = 0;

		numFailed += registerInCUDA && !interopContext;
		numFailed += registerInCUDA && (stride == 0);
		numFailed += registerInCUDA && (semanticOffsets[NxRenderInstanceSemantic::POSITION] == physx::PxU32(-1));
		numFailed += (semanticFormats[NxRenderInstanceSemantic::POSITION] == NxRenderDataFormat::UNSPECIFIED);

		return (numFailed == 0);
	}

public:
	physx::PxU32					maxInstances; //!< The maximum amount of instances this buffer will ever hold.
	NxRenderBufferHint::Enum		hint; //!< Hint on how often this buffer is updated.

	/**
	\brief Array of the corresponding formats for each semantic.

	NxRenderInstanceSemantic::UNSPECIFIED is used for semantics that are disabled
	*/
	NxRenderDataFormat::Enum		semanticFormats[NxRenderInstanceSemantic::NUM_SEMANTICS];
	/**
	\brief Array of the corresponding offsets (in bytes) for each semantic. Required when CUDA interop is used!
	*/
	physx::PxU32					semanticOffsets[NxRenderInstanceSemantic::NUM_SEMANTICS];

	physx::PxU32					stride; //!< The stride between instances of this buffer. Required when CUDA interop is used!

	bool							registerInCUDA; //!< Declare if the resource must be registered in CUDA upon creation

	/**
	This context must be used to register and unregister the resource every time the
	device is lost and recreated.
	*/
	physx::pxtask::CudaContextManager*   interopContext;
};

#pragma warning(pop)

PX_POP_PACK

}
} // end namespace physx::apex

#endif // NX_USER_RENDER_INSTANCE_BUFFER_DESC_H
