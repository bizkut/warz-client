/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_ServerMemoryLimitBlob_H
#define Navigation_ServerMemoryLimitBlob_H

#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"
#include "gwnavruntime/base/endianness.h"

namespace Kaim
{


	class ServerMemoryLimitBlob
	{
		KY_ROOT_BLOB_CLASS(VisualDebugMessage, ServerMemoryLimitBlob, 0)
	public:

		ServerMemoryLimitBlob() : m_memoryLimit(0) {}

		KyUInt32 m_memoryLimit;
	};

	inline void SwapEndianness(Endianness::Target e, ServerMemoryLimitBlob& self)
	{
		SwapEndianness(e, self.m_memoryLimit);
	}

} // namespace Kaim

#endif // Navigation_ServerMemoryLimit_H

