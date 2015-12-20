/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

Filename    :   HeapMH_DebugInfo.h
Content     :   Debug and statistics implementation.
Created     :   July 14, 2008
Authors     :   Maxim Shemanarev

**************************************************************************/

#ifndef INC_KY_Kernel_HeapMH_DebugInfo_H
#define INC_KY_Kernel_HeapMH_DebugInfo_H

#include "gwnavruntime/kernel/SF_HeapTypes.h"

#ifdef KY_MEMORY_ENABLE_DEBUG_INFO

#include "gwnavruntime/kernel/SF_List.h"
#include "gwnavruntime/kernel/SF_Atomic.h"
#include "gwnavruntime/kernel/HeapMH/HeapMH_Root.h"

namespace Kaim { namespace HeapMH {

using namespace Heap;

class AllocEngineMH;

//------------------------------------------------------------------------
template<class T>
struct DebugDataPoolMH : ListNode<DebugDataPoolMH<T> >
{
    typedef DebugDataPoolMH<T>   SelfType;

    DebugDataPoolMH() : UseCount(0) {}

    static UPInt GetSize(UPInt bytes)
    { 
        return (bytes - sizeof(SelfType)) / sizeof(T); 
    }

    T* GetElement(UPInt i) 
    {
        return (T*)((UByte*)this + sizeof(SelfType) + i*sizeof(T));
    }
    UPInt   UseCount;
};



//------------------------------------------------------------------------
struct DebugDataMH : ListNode<DebugDataMH>
{
    enum { ChainLimit = 10 };

    typedef DebugDataPoolMH<DebugDataMH> DataPool;

    void Clear()
    {
        DataCount   = 0;
        ChainCount  = 0;
        pParent     = 0;
        pNextData   = 0;
        RefCount    = 0;
        Address     = 0;
        Size        = 0;
        memset(&Info, 0, sizeof(Info));
    }

    void MessUp()
    {
        memset(&DataCount,  0xFE, sizeof(DataCount));
        memset(&ChainCount, 0xFE, sizeof(ChainCount));
        memset(&pParent,    0xFE, sizeof(pParent));
        memset(&pNextData,  0xFE, sizeof(pNextData));
        memset(&RefCount,   0xFE, sizeof(RefCount));
        memset(&Address,    0xFE, sizeof(Address));
        memset(&Size,       0xFE, sizeof(Size));
        memset(&Info,       0xFE, sizeof(Info));
    }


    UPIntHalf       DataCount;
    UPIntHalf       ChainCount;
    DataPool*       pDataPool;
    DebugDataMH*    pParent;
    DebugDataMH*    pNextData;      // Next data entry in chain
    UPInt           RefCount;
    UPInt           Address;
    UPInt           Size;
    AllocInfo       Info;
};




//------------------------------------------------------------------------
class DebugStorageMH
{
    enum 
    { 
        PoolSize = Heap_DebugAllocPoolSize,
    };

    typedef DebugDataPoolMH<DebugDataMH> DataPoolType;

public:
    struct DebugDataPtr
    {
        DebugDataPtr() : pData(0), Index(~UPInt(0)), pSelf(0), pPrev(0) {}
        DebugDataMH*  pData;
        UPInt         Index;
        DebugDataMH*  pSelf;
        DebugDataMH*  pPrev;
    };

    DebugStorageMH(SysAlloc* alloc, LockSafe* rootLocker);

    unsigned GetStatId(PageInfoMH* page, UPInt parentAddr, const AllocInfo* info);

    bool AddAlloc(UPInt addr, UPInt size, PageInfoMH* pageInfo, const AllocInfo* info);

    bool AddAlloc(PageInfoMH* parentInfo, UPInt parentAddr, UPInt thisAddr, 
                  UPInt size, PageInfoMH* pageInfo, const AllocInfo* info);

    void RemoveAlloc(UPInt addr, PageInfoMH* pageInfo);

    void RelinkAlloc(DebugDataPtr* ptr, UPInt oldAddr, 
                     UPInt newAddr, UPInt newSize, PageInfoMH* newInfo);

    void CheckDataTail(const DebugDataPtr* ptr, UPInt usable);

    void FreeAll();

    void GetDebugData(UPInt addr, PageInfoMH* pageInfo, DebugDataPtr* ptr);

    void UnlinkAlloc(UPInt addr, PageInfoMH* pageInfo, DebugDataPtr* ptr);

    void GetStats(AllocEngineMH* allocator, StatBag* bag) const;

    const DebugDataMH* GetFirstEntry() const;
    const DebugDataMH* GetNextEntry(const DebugDataMH* entry) const;

    void VisitMem(MemVisitor* visitor, unsigned flags) const;

    bool DumpMemoryLeaks(const char* heapName);

    void UltimateCheck();

    UPInt GetUsedSpace() const;

private:
    bool        allocDataPool();
    void        freeDataPool(DataPoolType* pool);

    DebugDataMH* getDebugData(PageInfoMH* page);
    void         setDebugData(PageInfoMH* page, DebugDataMH* data);
    DebugDataMH* allocDebugData();
    void        freeDebugData(DebugDataMH* data);
    void        unlinkDebugData(PageInfoMH* page, DebugDataPtr* ptr);
    void        linkDebugData(PageInfoMH* page, DebugDataMH* data);
    void        findInChainWithin(DebugDataMH* chain, UPInt addr, DebugDataPtr* ptr);
    void        findInChainExact(DebugDataMH* chain, UPInt addr, DebugDataPtr* ptr);
    void        findDebugData(PageInfoMH* page, UPInt addr, DebugDataPtr* ret);
    void        fillDataTail(DebugDataMH* data, UPInt usable);
    void        reportViolation(DebugDataMH* data, const char* msg);

    SysAlloc*           pAllocator;
    List<DataPoolType>  DataPools;
    List<DebugDataMH>   UsedDataList;
    List<DebugDataMH>   FreeDataList;
    LockSafe*           pRootLocker;
};

}} // Kaim::HeapMH

#endif // KY_MEMORY_ENABLE_DEBUG_INFO

#endif

