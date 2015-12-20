/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobFieldString_H
#define Navigation_BlobFieldString_H


#include "gwnavruntime/base/types.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/base/kyguid.h"
#include "gwnavruntime/basesystem/logstream.h"


namespace Kaim
{


class BlobFieldString
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BlobArray<char> m_name;
	BlobArray<char> m_value;
};

inline void SwapEndianness(Endianness::Target e, BlobFieldString& self)
{
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_value);
}


class BlobFieldStringMapping
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BlobFieldStringMapping() : m_name(KY_NULL), m_string(KY_NULL), m_guid(KY_NULL) {}

	BlobFieldStringMapping(const char* name, String* string_) :
		m_name(name), m_string(string_), m_guid(KY_NULL)
	{}

	BlobFieldStringMapping(const char* name, KyGuid* guid) :
		m_name(name), m_string(KY_NULL), m_guid(guid)
	{}

public:
	const char* m_name;
	String* m_string;
	KyGuid* m_guid;
};

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, BlobFieldStringMapping& mapping)
{
	os << mapping.m_name << " : " << mapping.m_string->ToCStr() << Endl;
	return os;
}


class BlobFieldStringBuilder : public BaseBlobBuilder<BlobFieldString>
{
public:
	BlobFieldStringBuilder(BlobFieldStringMapping& mapping) : m_mapping(&mapping) {}

private:
	virtual void DoBuild()
	{
		String str;
		if (m_mapping->m_string != KY_NULL)
		{
			str = *m_mapping->m_string;
		}
		else
		{
			char guidChars[37];
			m_mapping->m_guid->ToString(guidChars);
			str = guidChars;
		}

		BLOB_STRING(m_blob->m_name, m_mapping->m_name);
		BLOB_STRING(m_blob->m_value, str.ToCStr());
	}

	BlobFieldStringMapping* m_mapping;
};


}


#endif

