/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef GwNavRuntime_VisualDebugAttributes_H
#define GwNavRuntime_VisualDebugAttributes_H

#include "gwnavruntime/blob/blobfield32.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

class FloatStat;
class SentBlobTypeStats;
class VisualDebugServer;

enum VisualDebugStatType
{
	VisualDebugStatType_UInt  = Kaim::BlobField32::Type_KyUInt32,
	VisualDebugStatType_Int   = Kaim::BlobField32::Type_KyInt32,
	VisualDebugStatType_Float = Kaim::BlobField32::Type_KyFloat32,
	VisualDebugStatType_String
};

class VisualDebugAttributeHeader
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	VisualDebugAttributeHeader(const char* name) : m_name(name) {}

	String m_name;
	KyUInt32 m_index; // index of this header in VisualDebugAttributeGroup::m_headers
};

class VisualDebugAttributeValues
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	VisualDebugAttributeValues(KyUInt32 value32Count)
	{
		m_values32.Resize(value32Count);
		for (UPInt i = 0; i < value32Count; ++i)
			m_values32[i] = 0;
	}

	void AddInt32Value(KyInt32 val)                       { m_values32.PushBack(*(KyUInt32*)&val); }
	void AddUInt32Value(KyUInt32 val)                     { m_values32.PushBack(val);              }
	void AddValue32(KyUInt32 val)                         { m_values32.PushBack(val);              }
	void AddFloat32Value(KyFloat32 val);

	void SetStringValue(const char* val)                  { m_stringValue = val;                                                     }
	void SetInt32Value(KyInt32 val, KyUInt32 idx = 0)     { if (OutOfValuesBound(idx) == false) m_values32[idx] = *(KyUInt32*)&val;  }
	void SetUInt32Value(KyUInt32 val, KyUInt32 idx = 0)   { if (OutOfValuesBound(idx) == false) m_values32[idx] = val;               }
	void SetValue32(KyUInt32 val, KyUInt32 idx = 0)       { if (OutOfValuesBound(idx) == false) m_values32[idx] = val;               }
	void SetFloat32Value(KyFloat32 val, KyUInt32 idx = 0);

	const String& GetStringValue()     const              { return m_stringValue; }
	KyInt32   GetInt32Value(KyUInt32 idx)   const         { if (OutOfValuesBound(idx)) return 0;    else return *(KyInt32*)(&m_values32[idx]); }
	KyUInt32  GetUInt32Value(KyUInt32 idx)  const         { if (OutOfValuesBound(idx)) return 0;    else return m_values32[idx]; }
	KyFloat32 GetFloat32Value(KyUInt32 idx) const;

private:
	bool OutOfValuesBound(KyUInt32 valueIndex) const
	{
		KY_ASSERT(valueIndex < m_values32.GetSize());
		if (valueIndex < m_values32.GetSize())
			return false;
		else
			return true;
	}

public:
	String m_stringValue;
	KyArray<KyUInt32> m_values32;
};

class VisualDebugAttribute
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	VisualDebugAttribute() : m_name(""), m_valueType(VisualDebugStatType_UInt) {}

	VisualDebugAttribute(const char* name) : m_name(name) {}

	String m_name;
	VisualDebugStatType m_valueType;
	VisualDebugAttributeValues* m_values;  // pointer to the structure storing the item values
	VisualDebugAttributeHeader* m_header;  // pointer to the header (this could be shared between items)
};

/// A group of Key / valuetype / arrayof values, where the keys and types are created once, and
/// values updated whenever.
/// Usage:
/// 1. Create an enum describing all your items. It will allow you to access your stat in O(1).
/// 2. Create each item corresponding to each element of your enum, in your enum order.
/// 3. Update your item value(s) accessing the item with your enum.
class VisualDebugAttributeGroup
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	VisualDebugAttributeGroup() : m_name(""), m_descriptionSent(false) {}
	VisualDebugAttributeGroup(const char* name) : m_name(name), m_descriptionSent(false) {}

	~VisualDebugAttributeGroup() { Clear(); }

	void Clear();

	VisualDebugAttributeHeader* GetOrCreateHeader(const char* headerName);

	/// Call these to build your stat group
	VisualDebugAttribute* CreateFloatAttribute (const char* name, KyUInt32 statValueEnum, KyUInt32 valueCount = 1, VisualDebugAttributeHeader* header = KY_NULL);
	VisualDebugAttribute* CreateFloatStatAttribute (const char* name, KyUInt32 statValueEnum);
	VisualDebugAttribute* CreateSentItemsAttribute(const char* name, KyUInt32 statValueEnum);
	VisualDebugAttribute* CreateIntAttribute (const char* name, KyUInt32 statValueEnum, KyUInt32 valueCount = 1, VisualDebugAttributeHeader* header = KY_NULL);
	VisualDebugAttribute* CreateUIntAttribute(const char* name, KyUInt32 statValueEnum, KyUInt32 valueCount = 1, VisualDebugAttributeHeader* header = KY_NULL);
	VisualDebugAttribute* CreateStringAttribute(const char* name, KyUInt32 statValueEnum, VisualDebugAttributeHeader* header = KY_NULL);

	/// Call this to update your stat group
	void UpdateStringValue   (KyUInt32 valEnum, const char* val);
	void UpdateFloatValue    (KyUInt32 valEnum, KyFloat32 val  , KyUInt32 valIdx = 0);
	void UpdateUIntValue     (KyUInt32 valEnum, KyUInt32 val   , KyUInt32 valIdx = 0);
	void UpdateIntValue      (KyUInt32 valEnum, KyInt32 val    , KyUInt32 valIdx = 0);
	void UpdateFloatStatValue(KyUInt32 valEnum, const FloatStat& floatStat);
	void UpdateSentItemsValues(KyUInt32 valEnum, const SentBlobTypeStats& stats);

	//////////////////////////////////////////////////////////////////////////
	// VisualDebug
	//////////////////////////////////////////////////////////////////////////

	/// The decription must be sent only once on creation or on reconnection
	bool IsDescriptionSent()                      { return m_descriptionSent; }
	void SetDescriptionSent(bool descriptionSent) { m_descriptionSent = descriptionSent; }

	/// Send the group description (items' names, types, headers)
	void SendGroupDescription(VisualDebugServer* visualDebugServer);

	/// Send the group values (items' values only)
	void SendGroupValues(VisualDebugServer* visualDebugServer);

private:
	VisualDebugAttribute* CreateStatItem(const char* name, VisualDebugAttributeValues* statValue, VisualDebugStatType type
		, KyUInt32 statValueEnum, VisualDebugAttributeHeader* header);

public:
	String m_name;
	KyArray<VisualDebugAttribute*> m_items;
	KyArray<VisualDebugAttributeValues*> m_itemsValues;
	KyArray<VisualDebugAttributeHeader*> m_headers;
	bool m_descriptionSent;
};

}

#endif
