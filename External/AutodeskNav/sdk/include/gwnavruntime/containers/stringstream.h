/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: ? - secondary contact: NOBODY
#ifndef Navigation_StringStream_H
#define Navigation_StringStream_H

#include "gwnavruntime/kernel/SF_Std.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim
{


class StringStream
{
private:
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(StringStream)

public:
	StringStream() : m_ownString(), m_string(m_ownString) {}

	explicit StringStream(String& str) : m_string(str) {}

	const String& Str() const { return m_string; }

	const char* CStr() const { return m_string.ToCStr(); }

	void Clear() { m_string.Clear(); }

	StringStream& AppendString(const char* str, KyUInt32 length);

	StringStream& AppendChar(char value) { return AppendString(&value, 1); }

	template <typename T>
	StringStream& AppendValue(T value, const char* format)
	{
		char tempBuffer[64];
		KyUInt32 length = (KyUInt32)SFsprintf(tempBuffer, 64, format, value);
		AppendString(tempBuffer, length);
		return *this;
	}

private:
	String m_ownString;
	String& m_string;
};


inline StringStream& operator<<(StringStream& ss, KyInt32 value)         { return ss.AppendValue(value, "%i"); }
inline StringStream& operator<<(StringStream& ss, KyUInt32 value)        { return ss.AppendValue(value, "%u"); }
inline StringStream& operator<<(StringStream& ss, KyInt64 value)         { return ss.AppendValue((KyInt32)value, "%i"); }
inline StringStream& operator<<(StringStream& ss, KyUInt64 value)        { return ss.AppendValue((KyUInt32)value, "%u"); }
inline StringStream& operator<<(StringStream& ss, KyFloat32 value)       { return ss.AppendValue(value, "%f"); }
inline StringStream& operator<<(StringStream& ss, KyFloat64 value)       { return ss.AppendValue(value, "%f"); }
inline StringStream& operator<<(StringStream& ss, bool value)            { return value ? ss.AppendString("true", 4) : ss.AppendString("false", 5); }
inline StringStream& operator<<(StringStream& ss, char value)            { return ss.AppendChar(value); }
inline StringStream& operator<<(StringStream& ss, const char* str)
{
	if (str == KY_NULL)
		return ss;
	return ss.AppendString(str, (KyUInt32)(SFstrlen(str)));
}



} // namespace Kaim

#endif // Navigation_StringStream_H
