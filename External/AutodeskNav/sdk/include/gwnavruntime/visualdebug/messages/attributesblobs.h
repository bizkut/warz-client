/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_VisualDebugAttributesBlobs_H
#define Navigation_VisualDebugAttributesBlobs_H

#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim
{

class VisualDebugAttribute;
class VisualDebugAttributeGroup;
class VisualDebugAttributeHeader;
class VisualDebugAttributeValues;

class VisualDebugAttributeBlob
{
public:
	VisualDebugAttributeBlob() {}

	BlobArray<char> m_name;
	KyUInt32 m_valueType;  // cast this to VisualDebugStatType
	KyUInt32 m_valueCount;
	KyInt32 m_headerIdx;   // -1 if has no header, else index of the header in VisualDebugAttributeGroupBlob::m_headers
};

inline void SwapEndianness(Endianness::Target e, VisualDebugAttributeBlob& self)
{
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_valueType);
	SwapEndianness(e, self.m_valueCount);
	SwapEndianness(e, self.m_headerIdx);
}

class VisualDebugAttributeBlobBuilder : public BaseBlobBuilder<VisualDebugAttributeBlob>
{
public:
	VisualDebugAttributeBlobBuilder(VisualDebugAttribute* attribute) : m_attribute(attribute) {}
	virtual void DoBuild();
	VisualDebugAttribute* m_attribute;
};


class VisualDebugAttributeHeaderBlob
{
public:
	VisualDebugAttributeHeaderBlob() {}
	BlobArray<char> m_name;
	KyInt32 m_index;        // index of the header in VisualDebugAttributeGroupBlob::m_headers
};

inline void SwapEndianness(Endianness::Target e, VisualDebugAttributeHeaderBlob& self)
{
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_index);
}

class VisualDebugAttributeHeaderBlobBuilder : public BaseBlobBuilder<VisualDebugAttributeHeaderBlob>
{
public:
	VisualDebugAttributeHeaderBlobBuilder(VisualDebugAttributeHeader* header) : m_header(header) {}
	virtual void DoBuild();
	VisualDebugAttributeHeader* m_header;
};

class AttributeGroupRemovalBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, AttributeGroupRemovalBlob, 0)
	KY_CLASS_WITHOUT_COPY(AttributeGroupRemovalBlob)

public:
	AttributeGroupRemovalBlob() {}

	BlobArray<char> m_groupName;
	KyUInt32 m_worldElementId;
};

inline void SwapEndianness(Endianness::Target e, AttributeGroupRemovalBlob& self)
{
	SwapEndianness(e, self.m_groupName);
	SwapEndianness(e, self.m_worldElementId);
}

class AttributeGroupRemovalBlobBuilder : public BaseBlobBuilder<AttributeGroupRemovalBlob>
{
public:
	AttributeGroupRemovalBlobBuilder(const char* groupName, KyUInt32 worldElementId = KyUInt32MAXVAL)
		: m_groupName(groupName), m_worldElementId(worldElementId) {}

	virtual void DoBuild()
	{
		BLOB_STRING(m_blob->m_groupName, m_groupName.ToCStr());
		BLOB_SET(m_blob->m_worldElementId, m_worldElementId);
	}
private:
	String m_groupName;
	KyUInt32 m_worldElementId;
};


/// Contains attributes names, types and headers,
/// and DOES NOT contain attributes values (see VisualDebugAttributeGroupValuesBlob)
class AttributeGroupBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, AttributeGroupBlob, 0)
	KY_CLASS_WITHOUT_COPY(AttributeGroupBlob)

public:
	AttributeGroupBlob() {}

	BlobArray<char> m_name;
	BlobArray<VisualDebugAttributeBlob> m_attributes;
	BlobArray<VisualDebugAttributeHeaderBlob> m_headers;
};

inline void SwapEndianness(Endianness::Target e, AttributeGroupBlob& self)
{
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_attributes);
	SwapEndianness(e, self.m_headers);
}

class VisualDebugAttributeGroupBlobBuilder : public BaseBlobBuilder<AttributeGroupBlob>
{
public:
	VisualDebugAttributeGroupBlobBuilder(VisualDebugAttributeGroup* attributeGroup) : m_attributeGroup(attributeGroup) {}
	virtual void DoBuild();
	VisualDebugAttributeGroup* m_attributeGroup;
};


class VisualDebugAttributeValuesBlob
{
public:
	VisualDebugAttributeValuesBlob() {}

	BlobArray<char> m_string;
	BlobArray<KyUInt32> m_values32; // could store a KyUInt32, KyInt32, KyFloat32 depending on attributes type (VisualDebugAttributeBlob::m_valueType)
};

inline void SwapEndianness(Endianness::Target e, class VisualDebugAttributeValuesBlob& self)
{
	SwapEndianness(e, self.m_string);
	SwapEndianness(e, self.m_values32);
}

class VisualDebugAttributeValuesBlobBuilder : public BaseBlobBuilder<VisualDebugAttributeValuesBlob>
{
public:
	VisualDebugAttributeValuesBlobBuilder(VisualDebugAttributeValues* attributeValue) : m_attributeValue(attributeValue) {}
	virtual void DoBuild();
	VisualDebugAttributeValues* m_attributeValue;
};


/// Contains attributes values 
/// and DOES NOT contain attributes names, types and headers (see VisualDebugAttributeGroupBlob)
class AttributeGroupValuesBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, AttributeGroupValuesBlob, 0)
	KY_CLASS_WITHOUT_COPY(AttributeGroupValuesBlob)

public:
	AttributeGroupValuesBlob() {}
	BlobArray<char> m_name;
	BlobArray<VisualDebugAttributeValuesBlob> m_attributesValues;
};

inline void SwapEndianness(Endianness::Target e, AttributeGroupValuesBlob& self)
{
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_attributesValues);
}

class VisualDebugAttributeGroupValuesBlobBuilder : public BaseBlobBuilder<AttributeGroupValuesBlob>
{
public:
	VisualDebugAttributeGroupValuesBlobBuilder(VisualDebugAttributeGroup* attributeGroup) : m_attributeGroup(attributeGroup) {}
	virtual void DoBuild();
	VisualDebugAttributeGroup* m_attributeGroup;
};

}

#endif
