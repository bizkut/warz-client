/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef GwNavGen_GeneratorMemoryHeap_H
#define GwNavGen_GeneratorMemoryHeap_H


#include "gwnavruntime/kernel/SF_MemoryHeap.h"
#include "gwnavgeneration/common/generatormemory.h"


namespace Kaim
{

class GeneratorSysAlloc;
class ITlsAlloc;


class GeneratorMemoryHeap : public MemoryHeap
{
public:
	enum HeapType { HeapType_System, HeapType_Tls, HeapType_Magic };
	static bool InitMemorySystem(GeneratorSysAlloc* sysAlloc, GeneratorSysAlloc::AllocType allocType); // called by GeneratorSysAlloc::initHeapEngine()
	static void CleanUpMemorySystem();                // called by GeneratorSysAlloc::shutdownHeapEngine()
	static bool InitTlsAlloc(ITlsAlloc* tlsAlloc);    // called by GeneratorMemory::InitTlsAlloc() - called by Generator::Generate()
	static void ClearTlsAlloc();

public:
	GeneratorMemoryHeap(HeapType heapType) : m_heapType(heapType) {}

	// SimpleMemoryHeap interface implemented in GeneratorMemoryHeap derivations
	virtual void* Alloc(UPInt size, const AllocInfo* info = 0);
	virtual void* Alloc(UPInt size, UPInt align, const AllocInfo* info = 0);
	virtual void* Realloc(void* oldPtr, UPInt newSize);
	virtual void* ReallocAutoHeap(void* oldPtr, UPInt newSize);
	virtual void Free(void* ptr);
	virtual void* AllocSysDirect(UPInt size);
	virtual void FreeSysDirect(void* ptr, UPInt size);
	virtual void* AllocAutoHeap(const void* thisPtr, UPInt size, UPInt align, const AllocInfo* info = 0);
	virtual void* AllocAutoHeap(const void* thisPtr, UPInt size, const AllocInfo* info = 0);
	virtual MemoryHeap* GetAllocHeap(const void* thisPtr);

	// GeneratorMemoryHeap specific interface
	virtual void* InternalAlloc(UPInt size, const AllocInfo* info) = 0;
	virtual void* InternalRealloc(void* oldPtr, UPInt newSize) = 0;
	virtual void  InternalFree(void* ptr) = 0;

public: // SimpleMemoryHeap interface with empty/trivial implementations
	virtual void    CreateArena(UPInt /*arena*/, SysAllocPaged* /*sysAlloc*/) {}
	virtual void    DestroyArena(UPInt /*arena*/) {}
	virtual bool    ArenaIsEmpty(UPInt /*arena*/) { return true; }
	virtual MemoryHeap* CreateHeap(const char* name, const HeapDesc& desc) { KY_UNUSED2(name, desc); return this; }
	virtual void    SetLimitHandler(LimitHandler* /*handler*/) {}
	virtual void    SetLimit(UPInt /*newLimit*/) {}
	virtual void    AddRef() {}
	virtual void    Release() {}
	virtual UPInt   GetUsableSize(const void* ptr) { KY_UNUSED(ptr); return 0; }
	virtual bool    GetStats(StatBag* /*bag*/) { return false; }
	virtual UPInt   GetFootprint()      const { return false; }
	virtual UPInt   GetTotalFootprint() const { return false; }
	virtual UPInt   GetUsedSpace()      const { return false; }
	virtual UPInt   GetTotalUsedSpace() const { return false; }
	virtual void    GetRootStats(RootStats* /*stats*/) {}
	virtual void    VisitMem(Heap::MemVisitor* /*visitor*/, unsigned /*flags*/) {}
	virtual void    VisitRootSegments(Heap::SegVisitor* /*visitor*/) {}
	virtual void    VisitHeapSegments(Heap::SegVisitor* /*visitor*/) const {}
	virtual void    SetTracer(HeapTracer* /*tracer*/) {}

protected: // SimpleMemoryHeap interface empty implementations
	virtual void  destroyItself() {}
	virtual void  ultimateCheck() {}
	virtual void  releaseCachedMem() {}
	virtual bool  dumpMemoryLeaks() { return false; }
	virtual void  checkIntegrity() const  {}
	virtual void  getUserDebugStats(RootStats* /*stats*/) const {}

protected:
	HeapType m_heapType;
};

}

#endif
