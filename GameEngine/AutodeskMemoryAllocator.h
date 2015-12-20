//=========================================================================
//	Module: AutodeskMemoryAllocator.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once
#include "r3dArenaAllocator.h"

//////////////////////////////////////////////////////////////////////////

#ifdef USE_R3D_MEMORY_ALLOCATOR
namespace NAMESPACE_NAME
{
	class r3dAutodeskAlloc: public NAMESPACE_NAME::SysAlloc
	{
	public:
		virtual void* Alloc(size_t size, size_t align)
		{
			return r3dAllocateMemory(size, align, MEMORY_TYPE);
		}

		virtual void Free(void* ptr, size_t size, size_t align)
		{
			r3dDeallocateMemory(ptr, align, MEMORY_TYPE);
		}

		virtual void* Realloc(void* oldPtr, size_t oldSize, size_t newSize, size_t align)
		{
			void *newMem = r3dAllocateMemory(newSize, align, MEMORY_TYPE);
			memcpy_s(newMem, newSize, oldPtr, R3D_MIN(oldSize, newSize));
			r3dDeallocateMemory(oldPtr, align, MEMORY_TYPE);
			return newMem;
		}
	};
}
#endif