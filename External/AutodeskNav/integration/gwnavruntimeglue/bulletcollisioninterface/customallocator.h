/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef KyRuntimeGlue_CustomAllocator_H
#define KyRuntimeGlue_CustomAllocator_H

KY_INLINE void* CustomAlignedAlloc(size_t size, int alignment)
{
	if (size == 0)
		return KY_NULL;

	return KY_MEMALIGN(size, alignment, Kaim::MemStat_World_Other);
}

KY_INLINE void CustomAlignedFreeFunc(void* memblock)
{
	if (memblock)
		KY_FREE_ALIGN(memblock);
}

KY_INLINE void* CustomAllocFunc(size_t size)
{
	if (size == 0)
		return KY_NULL;

	return KY_ALLOC(size, Kaim::MemStat_World_Other);
}

KY_INLINE void CustomFreeFunc(void* memblock)
{
	if (memblock)
		KY_FREE(memblock);
}

#endif // KyRuntimeGlue_CustomAllocator_H
