/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobFieldArray_H
#define Navigation_BlobFieldArray_H


#include "gwnavruntime/base/types.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/blob/blobfield32.h"
#include "gwnavruntime/blob/blobfieldstring.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"


namespace Kaim
{

class String;

class BlobFieldArray
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(NavData, BlobFieldArray, 0)
	KY_CLASS_WITHOUT_COPY(BlobFieldArray)
public:
	BlobFieldArray() {}
public:
	BlobArray<char> m_name;
	BlobArray<BlobField32> m_field32s;
	BlobArray<BlobFieldString> m_fieldStrings;
	BlobArray<BlobMultiField32> m_multiField32s;
};
inline void SwapEndianness(Endianness::Target e, BlobFieldArray& self)
{
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_field32s);
	SwapEndianness(e, self.m_fieldStrings);
	SwapEndianness(e, self.m_multiField32s);
}


class BlobFieldsMapping
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	void Clear()
	{
		m_field32Maps.Clear();
		m_fieldStringMaps.Clear();
		m_multiField32Maps.Clear();
	}

	void AddString(const char* name, String& value)     { m_fieldStringMaps.PushBack(BlobFieldStringMapping(name, &value)); }
	void AddGuid(const char* name, KyGuid& value)       { m_fieldStringMaps.PushBack(BlobFieldStringMapping(name, &value)); }

	void AddBool(const char* name, bool& value)         { m_field32Maps.PushBack(BlobField32Mapping(BlobField32::Type_KyUInt32 , name, &value)); }
	void AddInt32(const char* name, KyInt32& value)     { m_field32Maps.PushBack(BlobField32Mapping(BlobField32::Type_KyInt32  , name, &value)); }
	void AddFloat32(const char* name, KyFloat32& value) { m_field32Maps.PushBack(BlobField32Mapping(BlobField32::Type_KyFloat32, name, &value)); }
	void AddUInt32(const char* name, KyUInt32& value)   { m_field32Maps.PushBack(BlobField32Mapping(BlobField32::Type_KyUInt32 , name, &value)); }

	BlobMultiField32Mapping& AddMultiUInt32(const char* name, const char* category, KyUInt32 size)   { m_multiField32Maps.PushBack(BlobMultiField32Mapping(BlobField32::Type_KyUInt32, name, category, size)); return m_multiField32Maps.Back(); }
	BlobMultiField32Mapping& AddMultiInt32(const char* name, const char* category, KyUInt32 size)    { m_multiField32Maps.PushBack(BlobMultiField32Mapping(BlobField32::Type_KyInt32, name, category, size)); return m_multiField32Maps.Back(); }
	BlobMultiField32Mapping& AddMultiFloat32(const char* name, const char* category, KyUInt32 size)  { m_multiField32Maps.PushBack(BlobMultiField32Mapping(BlobField32::Type_KyFloat32, name, category, size)); return m_multiField32Maps.Back(); }

	KyResult ReadFromBlobFieldArray(const BlobFieldArray& blobFieldArray);

public:
	KyArray<BlobField32Mapping> m_field32Maps;
	KyArray<BlobFieldStringMapping> m_fieldStringMaps;
	KyArray<BlobMultiField32Mapping> m_multiField32Maps;
};

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, BlobFieldsMapping& fieldsMapping)
{
	for (KyUInt32 i = 0; i < fieldsMapping.m_fieldStringMaps.GetCount(); ++i)
		os << fieldsMapping.m_fieldStringMaps[i] << Endl;

	for (KyUInt32 i = 0; i < fieldsMapping.m_field32Maps.GetCount(); ++i)
		os << fieldsMapping.m_field32Maps[i] << Endl;

	for (KyUInt32 i = 0; i < fieldsMapping.m_multiField32Maps.GetCount(); ++i)
		os << fieldsMapping.m_multiField32Maps[i] << Endl;

	return os;
}



class BlobFieldArrayBuilder : public BaseBlobBuilder<BlobFieldArray>
{
public:
	BlobFieldArrayBuilder(const char* name, BlobFieldsMapping& mapping) : m_name(name), m_mapping(&mapping) {}

private:
	virtual void DoBuild();

public: // internal
	const char* m_name;
	BlobFieldsMapping* m_mapping;
};


}


#endif

