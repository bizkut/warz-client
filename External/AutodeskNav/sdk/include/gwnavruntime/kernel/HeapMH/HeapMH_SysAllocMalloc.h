/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   HeapMH_SysAllocMalloc.h
Content     :   System Allocator Interface
Created     :   2009
Authors     :   Maxim Shemanarev

Notes       :   Interface to the system allocator.

**************************************************************************/

#ifndef INC_KY_Kernel_HeapMH_SysAllocMalloc_H
#define INC_KY_Kernel_HeapMH_SysAllocMalloc_H

#include "gwnavruntime/kernel/SF_SysAlloc.h"

#if defined(KY_OS_WIN32) || defined(KY_OS_WINCE) || defined(KY_OS_XBOX) || defined(KY_OS_XBOX360)
#include <malloc.h>
#elif defined(KY_OS_WII)
#include <string.h>
#else
#include <stdlib.h>
#include <string.h>
#endif

namespace Kaim {

class SysAllocMalloc : public SysAllocBase_SingletonSupport<SysAllocMalloc, SysAlloc>
{
public:
#if defined(KY_OS_WII)
    // Amount of memory to reserve in MEM2 arena; 0 is all available space
    SysAllocMalloc(UPInt size = 0)
    {
        UByte *m2arenaLo = (UByte*) OSGetMEM2ArenaLo();
        UByte *m2arenaHi = (UByte*) OSGetMEM2ArenaHi();
        if (size > 0 && (m2arenaLo + size < m2arenaHi))
            m2arenaHi = m2arenaLo + size;
        OSSetMEM2ArenaLo(m2arenaHi);
        //SF_DEBUG_WARNING(m2arenaHi-m2arenaLo < 65536, "GSysAllocMalloc: Less than 64k memory available");

        ::new(PrivateData) SysAllocStatic(m2arenaLo, (UByte*)m2arenaHi-(UByte*)m2arenaLo);
    }
#else
    SysAllocMalloc() {}
#endif
    virtual ~SysAllocMalloc() {}

#if (defined(KY_OS_WIN32) || defined(KY_OS_WINCE) || defined(KY_OS_XBOX) || defined(KY_OS_XBOX360)) && !defined(KY_CC_GNU)
    virtual void* Alloc(UPInt size, UPInt align)
    {
        return _aligned_malloc(size, align);
    }

    virtual void  Free(void* ptr, UPInt size, UPInt align)
    {
        KY_UNUSED2(size, align);
        _aligned_free(ptr);
    }

    virtual void* Realloc(void* oldPtr, UPInt oldSize, UPInt newSize, UPInt align)
    {
        KY_UNUSED(oldSize);
        return _aligned_realloc(oldPtr, newSize, align);
    }
#elif defined(KY_OS_PS3)
    virtual void* Alloc(UPInt size, UPInt align)
    {
        return memalign(align, size);
    }

    virtual void  Free(void* ptr, UPInt size, UPInt align)
    {
        KY_UNUSED2(size, align);
        free(ptr);
    }

    virtual void* Realloc(void* oldPtr, UPInt oldSize, UPInt newSize, UPInt align)
    {
        KY_UNUSED(oldSize);
        return reallocalign(oldPtr, newSize, align);
    }

#elif defined(KY_OS_WII)
    virtual void* Alloc(UPInt size, UPInt align)
    {
        return GetAllocator()->Alloc(size, align);
    }

    virtual void  Free(void* ptr, UPInt size, UPInt align)
    {
        GetAllocator()->Free(ptr, size, align);
    }

    virtual void* Realloc(void* oldPtr, UPInt oldSize, UPInt newSize, UPInt align)
    {
        void* newPtr = NULL;
        if (GetAllocator()->ReallocInPlace(oldPtr, oldSize, newSize, align))
            newPtr = oldPtr;
        else {
            newPtr = GetAllocator()->Alloc(newSize, align);
            if (newPtr)
            {
                memcpy(newPtr, oldPtr, (newSize < oldSize) ? newSize : oldSize);
                GetAllocator()->Free(oldPtr, oldSize, align);
            }
        }
        return newPtr;
    }

private:
    UPInt PrivateData[(sizeof(SysAllocStatic) + sizeof(UPInt)) / sizeof(UPInt)];
    SysAllocStatic* GetAllocator() { return (SysAllocStatic*) PrivateData; }

#else
    virtual void* Alloc(UPInt size, UPInt align)
    {
        if (align < sizeof(void*))
            align = sizeof(void*);

        UPInt ptr = (UPInt)malloc(size+(align-1)+sizeof(UPInt));
        UPInt aligned = 0;
        if (ptr)
        {
            aligned = (UPInt(ptr) + sizeof(UPInt) + (align-1)) & ~(align-1);
            if (aligned == ptr)
                aligned += align;
            *(((UPInt*)aligned)-1) = aligned-ptr;
        }
        return (void*)aligned;
    }

    virtual void  Free(void* ptr, UPInt size,  UPInt align)
    {
        if(ptr)
        {
            UPInt src = UPInt(ptr) - *(((UPInt*)ptr)-1);
            free((void*)src);
        }
    }

    virtual void* Realloc(void* oldPtr, UPInt oldSize, UPInt newSize, UPInt align)
    {
        void* newPtr = Alloc(newSize, align);
        if (newPtr)
        {
            memcpy(newPtr, oldPtr, (newSize < oldSize) ? newSize : oldSize);
            Free(oldPtr, oldSize, align);
        }
        return newPtr;
    }
#endif
};


} // Scaleform

#endif
