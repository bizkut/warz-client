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

#ifndef USERALLOCATOR_H
#define USERALLOCATOR_H

#include "NxApexDefs.h"
#include "foundation/PxAllocatorCallback.h"
#include "PsShare.h"

#if defined(PX_CHECKED) || defined(_DEBUG)
#if defined(PX_WINDOWS)
#define USE_MEM_TRACKER
#endif
#endif

class NxUserAllocator;
class PhysX28Allocator;

namespace MEM_TRACKER
{
	class MemTracker;
};

/* User allocator for APEX and 3.0 PhysX SDK */
class UserPxAllocator : public physx::PxAllocatorCallback
{
public:
	UserPxAllocator(const char* context, const char* dllName, bool useTrackerIfSupported = true);
	virtual		   ~UserPxAllocator();

	physx::PxU32	getHandle(const char* name);


	void*			allocate(size_t size, const char* typeName, const char* filename, int line);
	void			deallocate(void* ptr);

	size_t			getAllocatedMemoryBytes()
	{
		return mMemoryAllocated;
	}

	NxUserAllocator* get28PhysXAllocator();

	static bool dumpMemoryLeaks(const char* filename);

private:
	bool				trackerEnabled() const { return mUseTracker && (NULL != mMemoryTracker); }

	const char*			mContext;
	size_t				mMemoryAllocated;
	PhysX28Allocator*	mNxAllocator;
	const bool			mUseTracker;

	static MEM_TRACKER::MemTracker	*mMemoryTracker;
	static int gMemoryTrackerClients;

	// Poor man's memory leak check
	static unsigned int mNumAllocations;
	static unsigned int mNumFrees;

	friend class PhysX28Allocator;
};

#endif
