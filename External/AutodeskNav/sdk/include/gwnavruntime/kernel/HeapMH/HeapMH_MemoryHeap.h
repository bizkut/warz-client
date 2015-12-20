/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   HeapMH_MemoryHeap.h
Content     :   Public memory heap class declaration, MagicHeader engine.
Created     :   October 1, 2008
Authors     :   Michael Antonov, Maxim Shemanarev

**************************************************************************/

#ifndef INC_KY_Kernel_HeapMH_MemoryHeap_H
#define INC_KY_Kernel_HeapMH_MemoryHeap_H

#include "gwnavruntime/kernel/SF_MemoryHeap.h"

namespace Kaim {

// *** Predeclarations
//------------------------------------------------------------------------
namespace HeapMH
{
    class  RootMH;
    class  AllocEngineMH;
    class  DebugStorageMH;
    struct PageMH;
    struct NodeMH;
    struct PageInfoMH;
}

// ***** MemoryHeap
//------------------------------------------------------------------------
class MemoryHeapMH : public MemoryHeap
{
private:
    friend class HeapMH::RootMH;

    MemoryHeapMH();  // Explicit creation and destruction is prohibited
    virtual ~MemoryHeapMH() {}

public:

    // *** Operations with memory arenas
    //
    //--------------------------------------------------------------------
    virtual void CreateArena(UPInt arena, SysAllocPaged* sysAlloc);
    virtual void DestroyArena(UPInt arena);
    virtual bool ArenaIsEmpty(UPInt arena);

    // *** Initialization
    //
    // Creates a nested child heap; The heap should be destroyed 
    // by calling release. If child heap creation failed due to 
    // out-of-memory condition, returns 0. If child heap creation 
    // is not supported a pointer to the same parent heap will be returned.
    //--------------------------------------------------------------------
    virtual MemoryHeap* CreateHeap(const char* name, 
                                   const HeapDesc& desc);

    // *** Service functions
    //--------------------------------------------------------------------

    virtual void    SetLimitHandler(LimitHandler* handler);
    virtual void    SetLimit(UPInt newLimit);
    virtual void    AddRef();
    virtual void    Release();

    // *** Allocation API
    //--------------------------------------------------------------------
    virtual void*   Alloc(UPInt size, const AllocInfo* info = 0);
    virtual void*   Alloc(UPInt size, UPInt align, const AllocInfo* info = 0);
    virtual void*   Realloc(void* oldPtr, UPInt newSize); 
    virtual void    Free(void* ptr);
    virtual void*   AllocAutoHeap(const void *thisPtr, UPInt size,
                                  const AllocInfo* info = 0);

    virtual void*   AllocAutoHeap(const void *thisPtr, UPInt size, UPInt align,
                                  const AllocInfo* info = 0);

    virtual MemoryHeap* GetAllocHeap(const void *thisPtr);

    virtual UPInt   GetUsableSize(const void* ptr);


    virtual void*   AllocSysDirect(UPInt size);
    virtual void    FreeSysDirect(void* ptr, UPInt size);

    // *** Statistics
    //--------------------------------------------------------------------
    virtual bool    GetStats(StatBag* bag);
    virtual UPInt   GetFootprint() const;
    virtual UPInt   GetTotalFootprint() const;
    virtual UPInt   GetUsedSpace() const;
    virtual UPInt   GetTotalUsedSpace() const;
    virtual void    GetRootStats(RootStats* stats);
    virtual void    VisitMem(Heap::MemVisitor* visitor, unsigned flags);

    virtual void    VisitRootSegments(Heap::SegVisitor* visitor);
    virtual void    VisitHeapSegments(Heap::SegVisitor* visitor) const;
    virtual void    SetTracer(HeapTracer* tracer);

private:
    //--------------------------------------------------------------------
    virtual void  destroyItself();
    virtual void  ultimateCheck();
    virtual void  releaseCachedMem();
    virtual bool  dumpMemoryLeaks();
    virtual void  checkIntegrity() const;
    virtual void  getUserDebugStats(RootStats* stats) const;

    void* allocMem(UPInt size, const AllocInfo* info);
    void* allocMem(UPInt size, UPInt align, const AllocInfo* info);
    void* allocMem(HeapMH::PageInfoMH* parentInfo, const void *thisPtr, UPInt size, const AllocInfo* info);
    void* allocMem(HeapMH::PageInfoMH* parentInfo, const void *thisPtr, UPInt size, UPInt align, const AllocInfo* info);
    void* reallocMem(HeapMH::PageMH* page, void* oldPtr, UPInt newSize);
    void* reallocMem(HeapMH::NodeMH* node, void* oldPtr, UPInt newSize);

    void freeLocked(void* ptr);
    void freeMem(HeapMH::PageMH* page, void* ptr);
    void freeMem(HeapMH::NodeMH* node, void* ptr);

    //--------------------------------------------------------------------
    HeapMH::AllocEngineMH*  pEngine;
    HeapMH::DebugStorageMH* pDebugStorage;
};

} // Scaleform

#endif
