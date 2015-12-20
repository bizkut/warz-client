/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_LogStream_H
#define Navigation_LogStream_H

#include "gwnavruntime/basesystem/baselog.h"

namespace Kaim
{

static const char Endl = '\n';

// LogStream useful to print to log with << sematics, usage:
// LogStream stream;
// stream << "x=" << x << "\n";
class LogStream
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Kaim::Stat_Default_Mem)
public:
#ifndef KY_NO_LOG_OUTPUT
	template <typename T>
	LogStream& Log(const char* format, const T& value)
	{
		if (Kaim::BaseLog::GetGlobalBaseLog())
			Kaim::BaseLog::GetGlobalBaseLog()->LogMessage(format, value);
		return *this;
	}
#endif
};


#ifndef KY_NO_LOG_OUTPUT
	inline LogStream& operator<<(LogStream& s, KyInt32 value)     { return s.Log("%i", value); }
	inline LogStream& operator<<(LogStream& s, KyUInt32 value)    { return s.Log("%u", value); }
	inline LogStream& operator<<(LogStream& s, KyInt64 value)     { return s.Log("%i", (KyInt32)value); }
	inline LogStream& operator<<(LogStream& s, KyUInt64 value)    { return s.Log("%u", (KyUInt32)value); }
	inline LogStream& operator<<(LogStream& s, float value)       { return s.Log("%f", value); }
	inline LogStream& operator<<(LogStream& s, double value)      { return s.Log("%f", value); }
	inline LogStream& operator<<(LogStream& s, bool value)        { return value ? s.Log("%s", "true") : s.Log("%s", "false"); }
	inline LogStream& operator<<(LogStream& s, char value)        { return s.Log("%c", value); }
	inline LogStream& operator<<(LogStream& s, const char* str)   { return s.Log("%s", str); }
#else
	inline LogStream& operator<<(LogStream& s, KyInt32 value)     { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, KyUInt32 value)    { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, KyInt64 value)     { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, KyUInt64 value)    { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, float value)       { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, double value)      { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, bool value)        { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, char value)        { KY_UNUSED(value); return s; }
	inline LogStream& operator<<(LogStream& s, const char* str)   { KY_UNUSED(str);   return s; }
#endif

}

#endif // Navigation_LogStream_H