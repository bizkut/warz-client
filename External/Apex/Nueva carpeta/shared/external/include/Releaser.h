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

#ifndef RELEASER_H
#define RELEASER_H

#include <stdio.h>

#include "NxApexInterface.h"
#include "NxParameterized.h"
#include "NxParameterizedTraits.h"

#define RELEASE_AT_EXIT(obj, typeName) Releaser<typeName> obj ## Releaser(obj);

template<typename T>
class Releaser
{
	Releaser(Releaser<T>& rhs);
	Releaser<T>& operator =(Releaser<T>& rhs);

public:
	Releaser(T* obj, void* memory = NULL) : mObj(obj), mMemory(memory) 
	{
	}

	~Releaser() 
	{
		doRelease();
	}

	void doRelease();

	void reset(T* newObj = NULL, void* newMemory = NULL)
	{
		if (newObj != mObj)
		{
			doRelease(mObj);
		}
		mObj = newObj;
		mMemory = newMemory;
	}

private:
	T* mObj;

	void* mMemory;
};

template<> PX_INLINE void Releaser<NxParameterized::Interface>::doRelease()
{
	if (mObj != NULL)
	{
		mObj->destroy();
	}
}

template<> PX_INLINE void Releaser<NxParameterized::Traits>::doRelease()
{
	if (mMemory != NULL)
	{
		mObj->free(mMemory);
	}
}

template<> PX_INLINE void Releaser<physx::apex::NxApexInterface>::doRelease()
{
	if (mObj != NULL)
	{
		mObj->release();
	}
}

template<> PX_INLINE void Releaser<physx::PxFileBuf>::doRelease()
{
	if (mObj != NULL)
	{
		mObj->release();
	}
}

template<> PX_INLINE void Releaser<FILE>::doRelease()
{
	if (mObj != NULL)
	{
		fclose(mObj);
	}
}

#endif	// RESOURCE_MANAGER_H
