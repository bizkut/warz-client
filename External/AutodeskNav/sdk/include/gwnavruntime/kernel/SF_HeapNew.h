/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_HeapNew.h
Content     :   Defines heap allocation macros such as KY_HEAP_NEW, etc.
Created     :   September 9, 2008
Authors     :   Michael Antonov, Maxim Shemanarev

Notes       :   This file should be included in internal headers and
                source files but NOT the public headers because it
                #undefines operator new. It is separated from Memory
                for this reason.

**************************************************************************/

#include "gwnavruntime/kernel/SF_Memory.h"

// Operator 'new' must NOT be defined, or the macros below will not compile
// correctly (compiler would complain about the number of macro arguments).
#undef new

#ifndef INC_KY_Kernel_HeapNew_H
#define INC_KY_Kernel_HeapNew_H


// Define heap macros
#ifdef KY_MEMORY_ENABLE_DEBUG_INFO

#define KY_HEAP_NEW(heap)        new(heap,__FILE__,__LINE__)
#define KY_HEAP_NEW_ID(heap,id)  new(heap, id, __FILE__,__LINE__)
#define KY_HEAP_AUTO_NEW(addr)   new((Kaim::MemAddressPtr)((void*)addr),__FILE__,__LINE__)
// Global new
#define KY_NEW                    new(__FILE__,__LINE__)

#else

#define KY_HEAP_NEW(pheap)        new(pheap)
#define KY_HEAP_NEW_ID(pheap,id)  new(pheap,id)
#define KY_HEAP_AUTO_NEW(paddr)   new((Kaim::MemAddressPtr)((void*)paddr))
#define KY_NEW                    new

#endif 


#endif // INC_KY_HEAPNEW_H
