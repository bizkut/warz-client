/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   HeapPT_SysAllocMMAP.h
Platform    :   Unix, Linux, MacOS
Content     :   MMAP based System Allocator
Created     :   2009
Authors     :   Maxim Shemanarev, Boris Rayskiy

Notes       :   System Allocator that uses regular malloc/free

**************************************************************************/

#ifndef INC_KY_Kernel_HeapPT_SysAllocMMAP_H
#define INC_KY_Kernel_HeapPT_SysAllocMMAP_H

#include "gwnavruntime/kernel/SF_SysAlloc.h"

namespace Kaim {

// ***** SysAllocMMAP
//
//------------------------------------------------------------------------
class SysAllocMMAP : public SysAllocBase_SingletonSupport<SysAllocMMAP, SysAllocPaged>
{
public:
    enum { MinGranularity = 4*1024 };

    SysAllocMMAP(UPInt granularity = MinGranularity);


    virtual void    GetInfo(Info* i) const;
    virtual void*   Alloc(UPInt size, UPInt align);
    virtual bool    ReallocInPlace(void* oldPtr, UPInt oldSize,
                                   UPInt newSize, UPInt align);
    virtual bool    Free(void* ptr, UPInt size, UPInt align);


private:

    UPInt   Alignment;
    UPInt   Granularity;
    UPInt   Footprint;

};

} // Scaleform

#endif 

