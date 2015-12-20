/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_System.h
Content     :   General kernel initalization/cleanup, including that
                of the memory allocator.
Created     :   Ferbruary 5, 2009
Authors     :   Michael Antonov

**************************************************************************/

#ifndef INC_KY_Kernel_System_H
#define INC_KY_Kernel_System_H

#include "gwnavruntime/kernel/SF_Memory.h"

namespace Kaim {

// ***** System Core Initialization class

// System initialization must take place before any other KY_Kernel objects are used;
// this is done my calling System::Init(). Among other things, this is necessary to
// initialize the memory allocator. Similarly, System::Destroy must be
// called before program exist for proper clenup. Both of these tasks can be achieved by
// simply creating System object first, allowing its constructor/destructor do the work.

// Note that for GFx use this class super-seeded by the GFxSystem class, which
// should be used instead.

class System
{
public:

    // Two default argument constructors, to allow specifying KY_SysAllocPaged with and without
    // HeapDesc for the root heap.
    KY_EXPORT System(SysAllocBase* psysAlloc = KY_SYSALLOC_DEFAULT_CLASS::InitSystemSingleton())
    {
        Init(psysAlloc);
    }
    KY_EXPORT System(const MemoryHeap::HeapDesc& rootHeapDesc,
            SysAllocBase* psysAlloc = KY_SYSALLOC_DEFAULT_CLASS::InitSystemSingleton())
    {
        Init(rootHeapDesc, psysAlloc);
    }

    KY_EXPORT ~System()
    {
        Destroy();
    }

    // Initializes System core, setting the global heap that is needed for GFx
    // memory allocations. Users can override memory heap implementation by passing
    // a different memory heap here.   
    KY_EXPORT static void KY_CDECL Init(const MemoryHeap::HeapDesc& rootHeapDesc,
                     SysAllocBase* psysAlloc = KY_SYSALLOC_DEFAULT_CLASS::InitSystemSingleton());
    
    KY_EXPORT static void KY_CDECL Init(
                     SysAllocBase* psysAlloc = KY_SYSALLOC_DEFAULT_CLASS::InitSystemSingleton())
    {
        Init(MemoryHeap::RootHeapDesc(), psysAlloc);
    }

    // De-initializes System more, finalizing the threading system and destroying
    // the global memory allocator.
    KY_EXPORT static void KY_CDECL Destroy();
    KY_EXPORT static bool hasMemoryLeaks;
};

} // Scaleform

#endif
