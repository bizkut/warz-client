/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_Memory.h
Content     :   General purpose memory allocation functions
Created     :   December 28, 1998 - 2009
Authors     :   Michael Antonov, Maxim Shemanarev

**************************************************************************/

#ifndef INC_KY_Kernel_Memory_H
#define INC_KY_Kernel_Memory_H

#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/kernel/SF_MemoryHeap.h"

// Unified SysAlloc. New malloc-friendly allocator
#include "gwnavruntime/kernel/HeapMH/HeapMH_SysAllocMalloc.h"
#include "gwnavruntime/kernel/SF_StackRegistry.h"
#define KY_SYSALLOC_DEFAULT_CLASS Kaim::SysAllocMalloc

/*
// Include the right allocation header for the platform involved.
// This is ok even if users use custom allocators, as having the headers
// included here does NOT force the code to be linked.
#if defined(KY_OS_WIN32) || defined(KY_OS_XBOX360) || defined(KY_OS_WINCE)
    #include "gwnavruntime/kernel/HeapPT/HeapPT_SysAllocWinAPI.h"
    #define  KY_SYSALLOC_DEFAULT_CLASS SysAllocWinAPI
    #include "gwnavruntime/kernel/HeapPT/HeapPT_SysAllocMalloc.h"
    //#define  KY_SYSALLOC_DEFAULT_CLASS SysAllocMalloc
#elif defined(KY_OS_PS3)
    #include "gwnavruntime/kernel/HeapPT/HeapPT_SysAllocPS3.h"
    #define  KY_SYSALLOC_DEFAULT_CLASS SysAllocPS3
#elif defined(KY_OS_WII)
    #include "gwnavruntime/kernel/HeapPT/HeapPT_SysAllocWii.h"
    #define  KY_SYSALLOC_DEFAULT_CLASS SysAllocWii
#elif defined(KY_OS_3DS)
    #include "gwnavruntime/kernel/HeapPT/HeapPT_SysAlloc3DS.h"
    #define  KY_SYSALLOC_DEFAULT_CLASS SysAlloc3DS
#else
    #include "gwnavruntime/kernel/HeapMH/HeapMH_SysAllocMalloc.h"
    #define  KY_SYSALLOC_DEFAULT_CLASS SysAllocPagedMalloc
#endif
*/


// Define this macro to enable memory tracking; since tracking is not free
// this variable probably should not be defined for a release build.
//#define   KY_MEMORY_TRACKSIZES

// This file requires operator 'new' to NOT be defined; its definition
// is restored in the bottom of the header based on KY_DEFINE_NEW.
#undef new

// ***** Memory Allocation Macros

// These macros are used for allocation to ensure that the allocation location
// filename and line numbers are recorded in debug builds. The heap-based
// operator new versions are declared in a separate header file "HeapNew.h".

#if !defined(KY_MEMORY_ENABLE_DEBUG_INFO)
    // Allocate from global heap.
    #define KY_ALLOC(s,id)                    Kaim::Memory::Alloc((s))
    #define KY_MEMALIGN(s,a,id)               Kaim::Memory::Alloc((s),(a))
    #define KY_REALLOC(p,s,id)                Kaim::Memory::Realloc((p),(s))
    #define KY_FREE(p)                        Kaim::Memory::Free((p))
    #define KY_FREE_ALIGN(s)                  Kaim::Memory::Free((s))
    // Heap versions.
    #define KY_HEAP_ALLOC(heap,s,id)          Kaim::Memory::AllocInHeap((heap),(s))
    #define KY_HEAP_MEMALIGN(heap,s,a,id)     Kaim::Memory::AllocInHeap((heap),(s),(a))
    #define KY_HEAP_REALLOC(heap,p,s,id)      Kaim::Memory::ReallocInHeap((heap),(p),(s))
    #define KY_HEAP_AUTO_ALLOC(addr,s)        Kaim::Memory::AllocAutoHeap((addr),(s))
    #define KY_HEAP_AUTO_ALLOC_ID(addr,s,id)  Kaim::Memory::AllocAutoHeap((addr),(s))
    #define KY_HEAP_FREE(heap, p)             Kaim::Memory::Free((p))

#else // KY_MEMORY_ENABLE_DEBUG_INFO
    #define KY_ALLOC(s,id)                    Kaim::Memory::Alloc((s),      Kaim::AllocInfo((id),__FILE__,__LINE__))
    #define KY_MEMALIGN(s,a,id)               Kaim::Memory::Alloc((s),(a),  Kaim::AllocInfo((id),__FILE__,__LINE__))
    #define KY_REALLOC(p,s,id)                Kaim::Memory::Realloc((p),(s),Kaim::AllocInfo((id),__FILE__,__LINE__))
    #define KY_FREE(p)                        Kaim::Memory::Free((p))
    #define KY_FREE_ALIGN(s)                  Kaim::Memory::Free((s))
    // Heap versions.
    #define KY_HEAP_ALLOC(heap,s,id)          Kaim::Memory::AllocInHeap((heap),(s),      Kaim::AllocInfo((id),__FILE__,__LINE__))
    #define KY_HEAP_MEMALIGN(heap,s,a,id)     Kaim::Memory::AllocInHeap((heap),(s),(a),  Kaim::AllocInfo((id),__FILE__,__LINE__))
    #define KY_HEAP_REALLOC(heap,p,s,id)      Kaim::Memory::ReallocInHeap((heap),(p),(s))
    #define KY_HEAP_AUTO_ALLOC(addr,s)        Kaim::Memory::AllocAutoHeap((addr),(s),    Kaim::AllocInfo(Kaim::Stat_Default_Mem,__FILE__,__LINE__))
    #define KY_HEAP_AUTO_ALLOC_ID(addr,s,id)  Kaim::Memory::AllocAutoHeap((addr),(s),    Kaim::AllocInfo((id),__FILE__,__LINE__))
    #define KY_HEAP_FREE(heap, p)             Kaim::Memory::Free((p))

#endif // !defined(KY_MEMORY_ENABLE_DEBUG_INFO)

#ifdef KY_ENABLE_STACK_REGISTRY
KY_INLINE void* RegisterStack(void* p) { Kaim::StackRegistry::MapCurrentStackToUid(Kaim::UPInt(p)); return p;}
#define KY_REGISTER_STACK(p)   RegisterStack(p);
#define KY_UNREGISTER_STACK(p) Kaim::StackRegistry::UnMapStackFromUid((Kaim::UPInt)p)
#else
#define KY_REGISTER_STACK(p) p
#define KY_UNREGISTER_STACK(p)
#endif

namespace Kaim {

// ***** Memory Class

// Maintains current heap and the global allocator, wrapping heap access functions.
// The main purpose of wrapping is to allow GAllocDebugInfo temporary to be converted
// from a constant reference to an optional pointer argument for the allocation
// functions in MemoryHeap.

class Memory
{
public:
    static MemoryHeap *pGlobalHeap;


    // May need to export this later and check allocation status.
    static void         KY_STDCALL SetGlobalHeap(MemoryHeap *heap) { pGlobalHeap = heap; }
    static MemoryHeap*  KY_STDCALL GetGlobalHeap()                 { return pGlobalHeap; }

    // *** Operations with memory arenas
    //--------------------------------------------------------------------
    static void KY_STDCALL CreateArena(UPInt arena, SysAllocPaged* sysAlloc) { pGlobalHeap->CreateArena(arena, sysAlloc); }
    static void KY_STDCALL DestroyArena(UPInt arena)                         { pGlobalHeap->DestroyArena(arena); }
    static bool KY_STDCALL ArenaIsEmpty(UPInt arena)                         { return pGlobalHeap->ArenaIsEmpty(arena); }

    // *** Memory Allocation
    // Memory::Alloc of size==0 will allocate a tiny block & return a valid pointer;
    // this makes it suitable for new operator.
    static void* KY_STDCALL Alloc(UPInt size)                                       { return KY_REGISTER_STACK(pGlobalHeap->Alloc(size)); }
    static void* KY_STDCALL Alloc(UPInt size, UPInt align)                          { return KY_REGISTER_STACK(pGlobalHeap->Alloc(size, align)); }
    static void* KY_STDCALL Alloc(UPInt size, const AllocInfo& info)                { return KY_REGISTER_STACK(pGlobalHeap->Alloc(size, &info)); }
    static void* KY_STDCALL Alloc(UPInt size, UPInt align, const AllocInfo& info)   { return KY_REGISTER_STACK(pGlobalHeap->Alloc(size, align, &info)); }

    // Allocate while automatically identifying heap and allocation id based on the specified address.
    static void* KY_STDCALL AllocAutoHeap(const void *p, UPInt size)                                     { return KY_REGISTER_STACK(pGlobalHeap->AllocAutoHeap(p, size)); }
    static void* KY_STDCALL AllocAutoHeap(const void *p, UPInt size, UPInt align)                        { return KY_REGISTER_STACK(pGlobalHeap->AllocAutoHeap(p, size, align)); }
    static void* KY_STDCALL AllocAutoHeap(const void *p, UPInt size, const AllocInfo& info)              { return KY_REGISTER_STACK(pGlobalHeap->AllocAutoHeap(p, size, &info)); }
    static void* KY_STDCALL AllocAutoHeap(const void *p, UPInt size, UPInt align, const AllocInfo& info) { return KY_REGISTER_STACK(pGlobalHeap->AllocAutoHeap(p, size, align, &info)); }

    // Allocate in a specified heap. The later functions are provided to ensure that 
    // AllocInfo can be passed by reference with extended lifetime.
    static void* KY_STDCALL AllocInHeap(MemoryHeap* heap, UPInt size)                                       { return KY_REGISTER_STACK(heap->Alloc(size)); }
    static void* KY_STDCALL AllocInHeap(MemoryHeap* heap, UPInt size, UPInt align)                          { return KY_REGISTER_STACK(heap->Alloc(size, align)); }
    static void* KY_STDCALL AllocInHeap(MemoryHeap* heap, UPInt size, const AllocInfo& info)                { return KY_REGISTER_STACK(heap->Alloc(size, &info)); }
    static void* KY_STDCALL AllocInHeap(MemoryHeap* heap, UPInt size, UPInt align, const AllocInfo& info)   { return KY_REGISTER_STACK(heap->Alloc(size, align, &info)); }

    // Reallocate to a new size, 0 return == can't reallocate, previous memory is still valid
    // 
    // Realloc to decrease size will never fail
    // Realloc of pointer == 0 is equivalent to Memory::Alloc
    // Realloc of size == 0, shrinks to the minimal size, pointer remains valid and requires Free().
    // NOTE: Realloc guarantees the alignment specified in Alloc() 
    static void* KY_STDCALL Realloc(void *p, UPInt newSize)                         { KY_UNREGISTER_STACK(p); return KY_REGISTER_STACK(pGlobalHeap->Realloc(p, newSize)); }
    static void* KY_STDCALL ReallocAutoHeap(void *p, UPInt newSize)                 { KY_UNREGISTER_STACK(p); return KY_REGISTER_STACK(pGlobalHeap->ReallocAutoHeap(p, newSize)); }
    static void* KY_STDCALL ReallocInHeap(MemoryHeap* heap, void *p, UPInt newSize) { KY_UNREGISTER_STACK(p); return KY_REGISTER_STACK(heap->Realloc(p, newSize)); }

    // Free allocated/reallocated block
    static void         KY_STDCALL Free(void *p)                    { KY_UNREGISTER_STACK(p); pGlobalHeap->Free(p); }

    static MemoryHeap*  KY_STDCALL GetHeapByAddress(const void* p)  { return pGlobalHeap->GetAllocHeap(p); }
    static bool         KY_STDCALL DetectMemoryLeaks()              { return pGlobalHeap->DumpMemoryLeaks(); }
};

// ***** Macros to redefine class new/delete operators

// Types specifically declared to allow disambiguation of address in
// class member operator new. Used in HeapNew.h.

struct MemAddressStub { };
typedef MemAddressStub* MemAddressPtr;

#define KY_MEMORY_REDEFINE_NEW_IMPL(class_name, check_delete, StatType)                                              \
    void*   operator new(Kaim::UPInt sz)                                                                             \
    { void *p = KY_ALLOC(sz, StatType); return p; }                                                                  \
    void*   operator new(Kaim::UPInt sz, Kaim::MemoryHeap* heap)                                                     \
    { void *p = KY_HEAP_ALLOC(heap, sz, StatType); return p; }                                                       \
    void*   operator new(Kaim::UPInt sz, Kaim::MemoryHeap* heap, int blocktype)                                      \
    { KY_UNUSED(blocktype); void *p = KY_HEAP_ALLOC(heap, sz, blocktype); return p; }                                \
    void*   operator new(Kaim::UPInt sz, Kaim::MemAddressPtr adr)                                                    \
    { void *p = Kaim::Memory::AllocAutoHeap(adr, sz, Kaim::AllocInfo(StatType,__FILE__,__LINE__)); return p; }       \
    void*   operator new(Kaim::UPInt sz, const char* pfilename, int line)                                            \
    { void* p = Kaim::Memory::Alloc(sz, Kaim::AllocInfo(StatType, pfilename, line)); return p; }                     \
    void*   operator new(Kaim::UPInt sz, Kaim::MemoryHeap* heap, const char* pfilename, int line)                    \
    { void* p = Kaim::Memory::AllocInHeap(heap, sz, Kaim::AllocInfo(StatType, pfilename, line)); return p; }         \
    void*   operator new(Kaim::UPInt sz, Kaim::MemAddressPtr adr, const char* pfilename, int line)                   \
    { void* p = Kaim::Memory::AllocAutoHeap(adr, sz, Kaim::AllocInfo(StatType, pfilename, line)); return p; }        \
    void*   operator new(Kaim::UPInt sz, int blocktype, const char* pfilename, int line)                             \
    { void* p = Kaim::Memory::Alloc(sz, Kaim::AllocInfo(blocktype, pfilename, line)); return p; }                    \
    void*   operator new(Kaim::UPInt sz, Kaim::MemoryHeap* heap, int blocktype, const char* pfilename, int line)     \
    { void* p = Kaim::Memory::AllocInHeap(heap, sz, Kaim::AllocInfo(blocktype, pfilename, line)); return p; }        \
    void*   operator new(Kaim::UPInt sz, Kaim::MemAddressPtr adr, int blocktype, const char* pfilename, int line)    \
    { void* p = Kaim::Memory::AllocAutoHeap(adr, sz, Kaim::AllocInfo(blocktype, pfilename, line)); return p; }       \
    void    operator delete(void *p)                                                                                 \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, const char*, int)                                                               \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, int, const char*, int)                                                          \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemoryHeap*)                                                              \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemoryHeap*, int)                                                         \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemoryHeap*, const char*, int)                                            \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemoryHeap*, int,const char*,int)                                         \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemAddressPtr)                                                            \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemAddressPtr, int)                                                       \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemAddressPtr, const char*, int)                                          \
    { check_delete(class_name, p); KY_FREE(p); }                                                                     \
    void    operator delete(void *p, Kaim::MemAddressPtr,int,const char*,int)                                        \
    { check_delete(class_name, p); KY_FREE(p); }

#define KY_MEMORY_DEFINE_PLACEMENT_NEW                                                        \
    void*   operator new        (Kaim::UPInt n, void *ptr)    { KY_UNUSED(n); return ptr; }   \
    void    operator delete     (void *ptr, void *ptr2) { KY_UNUSED2(ptr,ptr2); }


#define KY_MEMORY_CHECK_DELETE_NONE(class_name, p)

// Redefined all delete/new operators in a class without custom memory initialization
#define KY_MEMORY_REDEFINE_NEW(class_name, StatType) KY_MEMORY_REDEFINE_NEW_IMPL(class_name, KY_MEMORY_CHECK_DELETE_NONE, StatType)

// Base class that overrides the new and delete operators.
// Deriving from this class, even as a multiple base, incurs no space overhead.
template<int Stat>
class NewOverrideBase
{
public:
    enum { StatType = Stat };

    // Redefine all new & delete operators.
    KY_MEMORY_REDEFINE_NEW(NewOverrideBase, Stat)
};


} // Scaleform



// *** Restore operator 'new' Definition

// If users specified KY__BUILD_DEFINE_NEW in preprocessor settings, use that definition.
#if defined(KY_BUILD_DEFINE_NEW) && !defined(KY_DEFINE_NEW)
#define KY_DEFINE_NEW new(__FILE__,__LINE__)
#endif
// Redefine operator 'new' if necessary.
#if defined(KY_DEFINE_NEW)
#define new KY_DEFINE_NEW
#endif


#endif // INC_GMEMORY_H
