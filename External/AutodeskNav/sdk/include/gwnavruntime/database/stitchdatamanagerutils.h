/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_StitchDataManagerUtils_H
#define Navigation_StitchDataManagerUtils_H

#include "gwnavruntime/base/types.h"


namespace Kaim
{

class MemoryManagerUtils
{
public:

	
	template<class T>
	static bool IsMemoryAlignedForClass(T* memory)
	{
		return IsMemoryAlignedForClassFromClassSize<(KyUInt32)sizeof(T)>((char*)memory);
	}

	template<class T>
	static  KyUInt32 GetPaddingForMemory(char* memory)
	{
		return GetPaddingForMemoryFromClassSize<(KyUInt32)sizeof(T)>(memory);
	}

	template<class T>
	static KyUInt32 GetPaddingForBytesCount(KyUInt32 currentBytesCount)
	{
		return GetPaddingForBytesCountFromClassSize<(KyUInt32)sizeof(T)>((UPInt)currentBytesCount);
	}

	template<KyUInt32 sizeOfClass>
	static inline bool IsMemoryAlignedForClassFromClassSize(char* memory)
	{
		return GetPaddingForMemoryFromClassSize<sizeOfClass>(memory) == 0;
	}

	template<KyUInt32 sizeOfClass>
	static inline KyUInt32 GetPaddingForMemoryFromClassSize(char* memory)
	{
		return GetPaddingForBytesCountFromClassSize<sizeOfClass>((UPInt)memory);
	}

	template<KyUInt32 sizeOfClass>
	static inline KyUInt32 GetPaddingForBytesCountFromClassSize(UPInt currentBytesCount)
	{
		if ((sizeOfClass & 0x1) != 0)
			// no padding
			return 0;
		else if ((sizeOfClass & 0x2) != 0)
			// padding to be 2 bytes aligned
			// need one 1 byte more if currentBytesCount is not already even
			return (KyUInt32)(currentBytesCount & 0x1);
		else
		{
#ifndef KY_64BIT_POINTERS
			// padding to be 4 bytes aligned
			// add 
			return (KyUInt32)((4 - (currentBytesCount & 0x3)) & 0x3);
#else
			if ((sizeOfClass & 0x4) != 0)
				// padding to be 4 bytes aligned
				return (KyUInt32)((4 - (currentBytesCount & 0x3)) & 0x3);
			else
				// padding to be 8 bytes aligned
				return (KyUInt32)((8 - (currentBytesCount & 0x7)) & 0x7);
#endif
		}

	}

	
};


}

#endif //Navigation_StitchDataManagerUtils_H

