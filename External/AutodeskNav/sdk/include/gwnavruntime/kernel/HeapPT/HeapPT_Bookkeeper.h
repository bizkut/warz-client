/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

Filename    :   HeapPT_Bookkeeper.h
Content     :   Bookkeeping allocator
Created     :   2009
Authors     :   Maxim Shemanarev

Notes       :   Internal allocator used to store bookkeeping information.

**************************************************************************/

#ifndef INC_KY_Kernel_HeapPT_Bookkeeper_H
#define INC_KY_Kernel_HeapPT_Bookkeeper_H

#include "gwnavruntime/kernel/SF_List.h"
#include "gwnavruntime/kernel/SF_SysAlloc.h"
#include "gwnavruntime/kernel/HeapPT/HeapPT_PageTable.h"
#include "gwnavruntime/kernel/HeapPT/HeapPT_AllocBitSet1.h"

namespace Kaim { namespace HeapPT {

using namespace Heap;

// ***** Bookkeeper
//
//  Internal allocator used to store bookkeeping information.
//------------------------------------------------------------------------
class Bookkeeper
{
public:
    typedef List<HeapSegment> SegmentListType;

    Bookkeeper(SysAllocPaged* sysAlloc, UPInt granularity = Heap_PageSize);

    SysAllocPaged* GetSysAlloc() { return pSysAlloc; }

    void*  Alloc(UPInt size);
    void   Free(void* ptr, UPInt size);

    void   VisitMem(MemVisitor* visitor, unsigned flags) const;
    void   VisitSegments(SegVisitor* visitor) const;

    UPInt  GetFootprint() const { return Footprint; }
    UPInt  GetUsedSpace() const { return Footprint - Allocator.GetTotalFreeSpace(); }

private:
    UPInt           getHeaderSize(UPInt dataSize) const;
    HeapSegment*    allocSegment(UPInt size);
    void            freeSegment(HeapSegment* seg);

    SysAllocPaged*      pSysAlloc;
    UPInt               Granularity;
    SegmentListType     SegmentList;
    AllocBitSet1        Allocator;
    UPInt               Footprint;
};

}} //Kaim::Heap

#endif
