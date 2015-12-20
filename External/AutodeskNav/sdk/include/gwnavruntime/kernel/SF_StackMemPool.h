/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_StackMemPool.h
Content     :   Policy-based memory pool, which tries to allocate memory
                on stack first.
Created     :   January 22, 2009
Authors     :   Sergey Sikorskiy

**************************************************************************/

#ifndef INC_KY_Kernel_StackMemPool_H
#define INC_KY_Kernel_StackMemPool_H

#include "gwnavruntime/kernel/SF_Memory.h"
#include "gwnavruntime/kernel/SF_Array.h"

namespace Kaim {

////////////////////////////////////////////////////////////////////////////////
// StackMemPool Alloc/Free policies.

// Regular Alloc/Free logic.
struct MemPoolImmediateFree
{
    MemPoolImmediateFree(MemoryHeap* pheap) : pHeap(pheap) {}

    void* Alloc(UPInt nbytes, UPInt align)
    {
        return pHeap ? Memory::AllocInHeap(pHeap, nbytes, align) : Memory::Alloc(nbytes, align);
    }
    void Free(void* p)
    {
        Memory::Free(p);
    }

private:
    MemoryHeap*    pHeap;
};

// Postpone freeing of memory till the very end.
struct MemPoolPostponeFree
{
    MemPoolPostponeFree(MemoryHeap* pheap) : pHeap(pheap) {}
    ~MemPoolPostponeFree()
    {
        for (UPInt i = 0; i < AllocatedMem.GetSize(); ++i)
        {
            Memory::Free(AllocatedMem[i]);
        }
    }

    void* Alloc(UPInt nbytes, UPInt align)
    {
        void* mem = pHeap ? Memory::AllocInHeap(pHeap, nbytes, align) : Memory::Alloc(nbytes, align);

        if (mem)
        {
            AllocatedMem.PushBack(mem);
        }

        return mem;
    }
    static void Free(void* /*p*/)
    {
        ; // Do nothing.
    }

private:
    MemoryHeap*    pHeap;
    Array<void*>   AllocatedMem;
};

////////////////////////////////////////////////////////////////////////////////
// Memory pool, which tries to allocate memory on stack.
// It can be used as a more flexible version of a stack memory buffer, or as 
// a faster memory allocator for in-place constructors.
template <UPInt N = 512, UPInt A = sizeof(void*), class AFP = MemPoolImmediateFree>
class StackMemPool : AFP
{
public:
    StackMemPool(MemoryHeap* pheap = NULL)
    : AFP(pheap), BuffPtr(AlignMem(Buff)), BuffSize(buff_max_size - (BuffPtr - Buff))
    {
    }

public:
    void* Alloc(UPInt nbytes)
    {
        void* tmpBuffPtr = NULL;

        if (nbytes <= BuffSize)
        {
            // Allocate in Buffer.
            tmpBuffPtr = BuffPtr;

            BuffPtr = AlignMem(BuffPtr + nbytes);
            const UPInt curSize = BuffPtr - Buff;
            BuffSize = buff_max_size > curSize ? buff_max_size - curSize : 0;
        } else
        {
            // Allocate in heap.
            tmpBuffPtr = AFP::Alloc(nbytes, align);
        }

        return tmpBuffPtr;
    }
    void Free(void* p)
    {
        if (!(p >= Buff && p < Buff + buff_max_size))
        {
            // Pointer is not inside of the Buffer.
            AFP::Free(p);
        }
    }

private:
    // Return new aligned offset. Align must be power of two.
    static
    char* AlignMem(char* offset)
    {
        return reinterpret_cast<char*>(A + ((reinterpret_cast<UPInt>(offset) - 1) & ~(A - 1)));
    }

private:
    // align must be power of two.
    enum { buff_max_size = N, align = A };

    char  Buff[buff_max_size];
    char* BuffPtr;
    UPInt BuffSize;
};

} // Scaleform

#undef new

template <Kaim::UPInt N, Kaim::UPInt A>
inline
void* operator new (Kaim::UPInt nbytes, Kaim::StackMemPool<N, A>& pool)
{
    return pool.Alloc(nbytes);
}

template <Kaim::UPInt N, Kaim::UPInt A>
inline
void operator delete (void* p, Kaim::StackMemPool<N, A>& pool)
{
    pool.Free(p);
}

// Redefine operator 'new' if necessary.
#if defined(KY_DEFINE_NEW)
#define new KY_DEFINE_NEW
#endif

// This header has to follow "#define new".
#include "gwnavruntime/kernel/SF_HeapNew.h"

#endif // INC_GMEMSTACKPOOL_H
