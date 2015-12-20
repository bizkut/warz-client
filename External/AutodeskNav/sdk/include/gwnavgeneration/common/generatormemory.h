/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef GwNavGen_GeneratorMemory_H
#define GwNavGen_GeneratorMemory_H


#include "gwnavruntime/basesystem/straightmalloc.h"
#include "gwnavruntime/kernel/SF_SysAlloc.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim {


class ITlsAlloc : public RefCountBaseV<ITlsAlloc, MemStat_NavDataGen>
{
public:
	virtual ~ITlsAlloc() {}
	virtual void* Alloc(UPInt size) = 0;
	virtual void  Free(void* ptr) = 0;
	virtual void* Realloc(void* oldPtr, UPInt newSize) = 0;
};


class GeneratorMemoryHeap;

class GeneratorMemory
{
public:
	static void InitTlsAlloc(ITlsAlloc* tlsAlloc);
	static void ClearTlsAlloc();

	static MemoryHeap* GlobalHeap() { return Memory::pGlobalHeap; }
	static MemoryHeap* SysHeap()    { return s_sysHeap; }
	static MemoryHeap* TlsHeap()    { return s_tlsHeap; }
	static MemoryHeap* MagicHeap()  { return s_magicHeap; }

public:
	static MemoryHeap* s_sysHeap;
	static MemoryHeap* s_tlsHeap;
	static MemoryHeap* s_magicHeap;
};


// Optimized allocator used in generator, use an optional TLS memory heap to reduce contention in multi-threaded computations
// You should derive from GeneratorSysAlloc to hook your own Alloc/Free/Realloc
class GeneratorSysAlloc : public SysAllocBase
{
public:
	GeneratorSysAlloc() { Clear(); }
	virtual ~GeneratorSysAlloc() {}

	enum AllocType { SlowSimpleAlloc = 0, FastMixedAlloc = 1 };

	// Must be called before BaseSystem::Init()
	// The default is FastMixedAlloc, this enables to get very good multi-core performance
	// at a very reasonable memory cost (sizeof(void*) per alloc)
	// It is strongly recommended that you don't call this function
	void SetAllocType(AllocType allocType)
	{
		if (m_heapEngineInitialized == false) // does nothing if initHeapEngine() has been called already
			m_allocType = allocType;
	}

	// respect the same Alloc/Free/Realloc prototypes than all SysAllocBase derivation
	virtual void* Alloc(UPInt size, UPInt align) = 0;
	virtual void  Free(void* ptr, UPInt size, UPInt align) = 0;
	virtual void* Realloc(void* oldPtr, UPInt oldSize, UPInt newSize, UPInt align) = 0;

protected:
	// Implementation of SysAllocBase based on GeneratorMemoryHeap
	virtual bool initHeapEngine(const void* heapDesc);
	virtual void shutdownHeapEngine();

	void Clear()
	{
		m_allocType = FastMixedAlloc;
		m_heapEngineInitialized = false;
	}

protected:
	AllocType m_allocType;
	bool m_heapEngineInitialized;
};


// GeneratorSysAlloc derivation that implements Alloc/Free/Realloc with StraightMalloc
// GeneratorSysAllocMalloc is the class that should be used in Generator BaseSystem initialization:
// Kaim::BaseSystem::Config config;
// config.m_sysAlloc = Kaim::GeneratorSysAllocMalloc::InitSystemSingleton();
// Kaim::BaseSystem baseSystem(config);
class GeneratorSysAllocMalloc : public SysAllocBase_SingletonSupport<GeneratorSysAllocMalloc, GeneratorSysAlloc>
{
public:
	GeneratorSysAllocMalloc() {}
	virtual ~GeneratorSysAllocMalloc() {}

	// Implementation of SysAlloc based on malloc and free (WINDOWS an LINUX only)
	virtual void* Alloc(UPInt size, UPInt align)                                    { return StraightSysAllocMalloc::Alloc(size, align); }
	virtual void  Free(void* ptr, UPInt size, UPInt align)                          { return StraightSysAllocMalloc::Free(ptr, size, align); }
	virtual void* Realloc(void* oldPtr, UPInt oldSize, UPInt newSize, UPInt align)  { return StraightSysAllocMalloc::Realloc(oldPtr, oldSize, newSize, align); }
};



}

#endif
