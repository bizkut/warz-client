/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#include "tbbtlsalloc.h"
#include "tbb/scalable_allocator.h"


namespace KyGlue
{

// ----------- TBB scalable allocator interface -----------
//void * scalable_malloc (UPInt size);
//void   scalable_free (void* ptr);
//void * scalable_realloc (void* ptr, UPInt size);
//void * scalable_calloc (UPInt nobj, UPInt size);
//int    scalable_posix_memalign (void** memptr, UPInt alignment, UPInt size);
//void * scalable_aligned_malloc (UPInt size, UPInt alignment);
//void * scalable_aligned_realloc (void* ptr, UPInt size, UPInt alignment);
//void   scalable_aligned_free (void* ptr);

void* TbbTlsAlloc::Alloc(Kaim::UPInt size)
{
	return scalable_aligned_malloc(size, sizeof(void*));
}

void* TbbTlsAlloc::Realloc(void* oldPtr, Kaim::UPInt newSize)
{
	return scalable_aligned_realloc(oldPtr, newSize, sizeof(void*));
}

void TbbTlsAlloc::Free(void* ptr)
{
	return scalable_aligned_free(ptr);
}


}



