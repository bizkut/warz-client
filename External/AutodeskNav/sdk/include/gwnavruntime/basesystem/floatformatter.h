/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_FloatFormatter_H
#define Navigation_FloatFormatter_H

#include "gwnavruntime/kernel/SF_Debug.h"
#include "gwnavruntime/kernel/SF_Log.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim
{

// usage
// FloatFormatter fmt4f("%.4f");
// stream << fmt4f(x) << fmt4f(y) << fmt4f(z);
class FloatFormatter
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Kaim::Stat_Default_Mem)
public:
	FloatFormatter(const char* format = "%.2f") : m_format(format) { KY_UNUSED(m_buffer); KY_UNUSED(m_format); }

	char* operator()(KyFloat32 value)
	{
	#ifndef KY_NO_LOG_OUTPUT
		SFsprintf(m_buffer, ArraySize(m_buffer), m_format, value);
		return m_buffer;
	#else
		KY_UNUSED(value);
		return KY_NULL;
	#endif
	}

private:
	const char* m_format;
	char m_buffer[64];
};

}

#endif //Navigation_FloatFormatter_H
