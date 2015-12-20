/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   HeapPT_SysAllocMalloc.h
Content     :   Malloc System Allocator
Created     :   2009
Authors     :   Maxim Shemanarev

Notes       :   System Allocator that uses regular malloc/free

**************************************************************************/

#ifndef INC_KY_Kernel_SysAllocMalloc_H
#define INC_KY_Kernel_SysAllocMalloc_H

#include "gwnavruntime/kernel/SF_SysAlloc.h"

namespace Kaim {

// ***** SysAllocPagedMalloc
//
//------------------------------------------------------------------------
class SysAllocPagedMalloc : public SysAllocBase_SingletonSupport<SysAllocPagedMalloc, SysAllocPaged>
{
public:
    enum { MinGranularity = 64*1024 };

    KY_EXPORT SysAllocPagedMalloc(UPInt granularity = MinGranularity);

    virtual void    GetInfo(Info* i) const;
    virtual void*   Alloc(UPInt size, UPInt align);
    virtual bool    Free(void* ptr, UPInt size, UPInt align);

    virtual UPInt   GetFootprint() const { return Footprint; }
    virtual UPInt   GetUsedSpace() const { return Footprint; }

    virtual UPInt   GetBase() const; // DBG

private:
    UPInt   Granularity;
    UPInt   Footprint;
    UPInt   Base; // DBG
};

} // Scaleform

#endif
