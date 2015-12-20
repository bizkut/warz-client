/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_StraightMalloc_H
#define Navigation_StraightMalloc_H


#include "gwnavruntime/kernel/SF_Types.h"


namespace Kaim {


/// multi-platform abstraction of malloc/free/realloc
/// Note that this is used only by the generation through MemoryHeap
/// and MemoryHeap::Realloc doesn't maintain alignment!
class StraightMemoryHeapMalloc
{
public:
	
	static void* Alloc(UPInt size); ///< Alloc without alignment
	static void* Realloc(void* oldPtr, UPInt newSize); ///< Realloc without alignment

	/// Alloc with alignment
	/// NOTE that Realloc should not be used with memory allocated with this function, 
	/// otherwise the reallocated memory could be not properly aligned.
	static void* Alloc(UPInt size, UPInt align);
	
	static void  Free(void* ptr);
};


/// multi-platform abstraction of malloc/free/realloc
/// Note that this is used via GeneratorSysAlloc
/// and it requires to maintain alignment
class StraightSysAllocMalloc
{
public:
	static void* Alloc(UPInt size, UPInt align);
	static void  Free(void* ptr, UPInt size, UPInt align);
	static void* Realloc(void* oldPtr, UPInt oldSize, UPInt newSize, UPInt align);
};

}

#endif
