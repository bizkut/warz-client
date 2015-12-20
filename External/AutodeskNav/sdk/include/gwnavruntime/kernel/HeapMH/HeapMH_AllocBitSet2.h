/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

Filename    :   HeapMH_AllocBitSet2.h
Content     :   "Magic-header based" Bit-set based allocator, 2 bits 
                per block.

Created     :   2009
Authors     :   Maxim Shemanarev

**************************************************************************/

#ifndef INC_KY_Kernel_HeapMH_AllocBitSet2_H
#define INC_KY_Kernel_HeapMH_AllocBitSet2_H

#include "gwnavruntime/kernel/SF_HeapBitSet2.h"
#include "gwnavruntime/kernel/HeapMH/HeapMH_FreeBin.h"

namespace Kaim { namespace HeapMH {

//------------------------------------------------------------------------
class AllocBitSet2MH
{
public:
    AllocBitSet2MH();

    void Reset() { Bin.Reset(); }

    void  InitPage(PageMH* page, UInt32 index);
    void  ReleasePage(UByte* start);

    void* Alloc(UPInt size, MagicHeadersInfo* headers);
    void* Alloc(UPInt size, UPInt alignSize, MagicHeadersInfo* headers);
    void* ReallocInPlace(PageMH* page, void* oldPtr, UPInt newSize, UPInt* oldSize, MagicHeadersInfo* headers);
    void  Free(PageMH* page, void* ptr, MagicHeadersInfo* headers, UPInt* oldBytes);

    UPInt GetUsableSize(const PageMH* page, const void* ptr) const;

private:
    ListBinMH Bin;
};

}} // Kaim::HeapMH


#endif
