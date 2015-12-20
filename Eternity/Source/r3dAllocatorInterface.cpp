//=========================================================================
//	Module: r3dAllocatorInterface.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3dAllocatorInterface.h"
#include "r3dArenaAllocator.h"

#include "r3dAssert.h"

//////////////////////////////////////////////////////////////////////////

#ifndef ENABLE_ADDITIONAL_VALIDATION
#pragma optimize("tpg", on)
#endif

#ifdef USE_R3D_MEMORY_ALLOCATOR

//////////////////////////////////////////////////////////////////////////

namespace
{
	class Singleton
	{
	public:
		static r3dArenaAllocator & Instance(const r3dAllocationTypes t)
		{
			//	Double-checked locking
			if (!allocators[t])
			{
				CRITICAL_SECTION * cs = Singleton::GetAllocatorGuard(t);
				CSHolder guard(cs);
				if (!allocators[t])
				{
					void *mem = malloc(sizeof(r3dArenaAllocator));
					allocators[t] = new (mem) r3dArenaAllocator(cs);
				}
			}
			return *allocators[t];
		}

	//////////////////////////////////////////////////////////////////////////
	
		static CRITICAL_SECTION * GetAllocatorGuard(const r3dAllocationTypes t)
		{
			if (!guards[t])
			{
				guards[t] = reinterpret_cast<CRITICAL_SECTION*>(malloc(sizeof(CRITICAL_SECTION)));
				InitializeCriticalSection(guards[t]);
			}
			return guards[t];
		}

	private:
		Singleton();
		static r3dArenaAllocator * allocators[ALLOC_TYPE_TOTAL];
		static CRITICAL_SECTION * guards[ALLOC_TYPE_TOTAL];
	};

	CRITICAL_SECTION * Singleton::guards[ALLOC_TYPE_TOTAL] = {0};
	r3dArenaAllocator * Singleton::allocators[ALLOC_TYPE_TOTAL] = {0};
}

//////////////////////////////////////////////////////////////////////////

void * r3dAllocateMemory(size_t size, unsigned int alignment, r3dAllocationTypes t)
{
	return Singleton::Instance(t).Allocate(size, alignment);
}

//////////////////////////////////////////////////////////////////////////

void r3dDeallocateMemory(void *ptr, unsigned int alignment, r3dAllocationTypes t)
{
	Singleton::Instance(t).Deallocate(ptr, alignment);
}

//////////////////////////////////////////////////////////////////////////

r3dAllocatorMemoryStats r3dGetAllocatorStats(r3dAllocationTypes t)
{
	return Singleton::Instance(t).GetMemStats();
}

//////////////////////////////////////////////////////////////////////////

//	Pugi xml
void * r3dPugiAllocateMemory(size_t size)
{
	return r3dAllocateMemory(size, 1, ALLOC_TYPE_GAME);
}

//////////////////////////////////////////////////////////////////////////

void r3dPugiDeallocateMemory(void *ptr)
{
	return r3dDeallocateMemory(ptr, 1, ALLOC_TYPE_GAME);
}

//////////////////////////////////////////////////////////////////////////

void * operator new(size_t size, r3dAllocationTypes t)
{
	return r3dAllocateMemory(size, 1, t);
}

//////////////////////////////////////////////////////////////////////////

void operator delete(void *pMemory, r3dAllocationTypes t)
{
	r3dDeallocateMemory(pMemory, 1, t);
}

//////////////////////////////////////////////////////////////////////////

void * operator new[](size_t size, r3dAllocationTypes t)
{
	return r3dAllocateMemory(size, 1, t);
}

//////////////////////////////////////////////////////////////////////////

void operator delete[](void *pMemory, r3dAllocationTypes t)
{
	r3dDeallocateMemory(pMemory, 1, t);
}

//////////////////////////////////////////////////////////////////////////

void * operator new(size_t size)
{
	return r3dAllocateMemory(size, 1, ALLOC_TYPE_GAME);
}

//////////////////////////////////////////////////////////////////////////

void * operator new[](size_t size)
{
	return r3dAllocateMemory(size, 1, ALLOC_TYPE_GAME);
}

//////////////////////////////////////////////////////////////////////////

void operator delete(void *pMemory)
{
	if (Singleton::Instance(ALLOC_TYPE_GFX).DeallocateOnlyOwnMemory(pMemory))
		return;

	Singleton::Instance(ALLOC_TYPE_GAME).Deallocate(pMemory, 1);
}

//////////////////////////////////////////////////////////////////////////

void operator delete[](void *pMemory)
{
	::operator delete(pMemory);
}

//////////////////////////////////////////////////////////////////////////

#endif 

#ifndef ENABLE_ADDITIONAL_VALIDATION
#pragma optimize("", on)
#endif
