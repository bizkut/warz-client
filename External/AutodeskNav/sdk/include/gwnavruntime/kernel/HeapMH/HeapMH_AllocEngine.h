/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

Filename    :   HeapMH_AllocEngine.h
Content     :   The main allocation engine
            :   
Created     :   2009
Authors     :   Maxim Shemanarev

**************************************************************************/

#ifndef INC_KY_Kernel_HeapMH_AllocEngine_H
#define INC_KY_Kernel_HeapMH_AllocEngine_H

#include "gwnavruntime/kernel/SF_Atomic.h"
#include "gwnavruntime/kernel/HeapMH/HeapMH_Root.h"
#include "gwnavruntime/kernel/HeapMH/HeapMH_AllocBitSet2.h"

namespace Kaim { namespace HeapMH {

//------------------------------------------------------------------------
class AllocEngineMH
{
    typedef List<PageMH> PageListType;
public:
    AllocEngineMH(SysAlloc*     sysAlloc,
                  MemoryHeapMH* heap,
                  UPInt         minAlignSize=16,
                  UPInt         limit=0);
    ~AllocEngineMH();

    bool IsValid() const { return true; }

    void FreeAll();

    UPInt SetLimit(UPInt lim)            { return Limit = lim; }
    void  SetLimitHandler(void* handler) { pLimHandler = handler; }

    void*   Alloc(UPInt size, PageInfoMH* info);
    void*   Alloc(UPInt size, UPInt alignSize, PageInfoMH* info);

    void*   ReallocInPage(PageMH* page, void* oldPtr, UPInt newSize, PageInfoMH* newInfo);
    void*   ReallocInNode(NodeMH* node, void* oldPtr, UPInt newSize, PageInfoMH* newInfo);
    void*   ReallocGeneral(PageMH* page, void* oldPtr, UPInt newSize, PageInfoMH* newInfo);
    void*   Realloc(void* oldPtr, UPInt newSize);
    void    Free(PageMH* page, void* ptr);
    void    Free(NodeMH* node, void* ptr);
    void    Free(void* ptr);

    void    GetPageInfo(PageMH* page, PageInfoMH* info) const;
    void    GetPageInfo(NodeMH* node, PageInfoMH* info) const;
    void    GetPageInfoWithSize(PageMH* page, const void* ptr, PageInfoMH* info) const;
    void    GetPageInfoWithSize(NodeMH* node, const void* ptr, PageInfoMH* info) const;

    UPInt   GetFootprint() const { return Footprint; }
    UPInt   GetUsedSpace() const { return UsedSpace; }

    UPInt   GetUsableSize(void* ptr);

private:
    PageMH* allocPage(bool* limHandlerOK);
    void*   allocDirect(UPInt size, UPInt alignSize, bool* limHandlerOK, PageInfoMH* info);
    void    freePage(PageMH* page);

    void*   allocFromPage(UPInt size, PageInfoMH* info);
    void*   allocFromPage(UPInt size, UPInt alignSize, PageInfoMH* info);

    SysAlloc*       pSysAlloc;
    MemoryHeapMH*   pHeap;
    UPInt           MinAlignSize;
    AllocBitSet2MH  Allocator;
    PageListType    Pages;
    UPInt           Footprint;
    UPInt           UsedSpace;
    UPInt           Limit;
    void*           pLimHandler;
    UPInt           UseCount;
};

}} // Kaim::HeapMH

#endif

