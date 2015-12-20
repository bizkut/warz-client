/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_DisplayListRemovalBlob_H
#define Navigation_DisplayListRemovalBlob_H

#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{


/// Send this blob to destroy the current displayList with id "displayListId" and eventually
/// being associated with the worldElement with id "worldElementId".
/// If worldElementId = KyUInt32MAXVAL, it means that the displayList with id "displayListId" and not
/// being attached to any worldElement will be destroyed.
class DisplayListRemovalBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, DisplayListRemovalBlob, 0)

public:
	DisplayListRemovalBlob() {}

	KyUInt32 m_displayListId;
	KyUInt32 m_worldElementId;
};

inline void SwapEndianness(Kaim::Endianness::Target e, DisplayListRemovalBlob& self)
{
	SwapEndianness(e, self.m_displayListId);
	SwapEndianness(e, self.m_worldElementId);
}

class DisplayListRemovalBlobBuilder : public BaseBlobBuilder<DisplayListRemovalBlob>
{
public:
	DisplayListRemovalBlobBuilder(KyUInt32 displayListId, KyUInt32 worldElementId = KyUInt32MAXVAL)
		: m_displayListId(displayListId), m_worldElementId(worldElementId) {}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_displayListId, m_displayListId);
		BLOB_SET(m_blob->m_worldElementId, m_worldElementId);
	}
private:
	KyUInt32 m_displayListId;
	KyUInt32 m_worldElementId;
};


/// Send this blob to destroy the current displayList with name "displayListName" and eventually
/// being associated with the worldElement with id "worldElementId".
/// If worldElementId = KyUInt32MAXVAL, it means that the displayList with name "displayListName" and not
/// being attached to any worldElement will be destroyed.
class DisplayListRemovalFromNameBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, DisplayListRemovalFromNameBlob, 0)

public:
	DisplayListRemovalFromNameBlob() {}

	KyUInt32 m_worldElementId;
	BlobArray<char> m_displayListName;
	BlobArray<char> m_displayListGroupName;
};

inline void SwapEndianness(Kaim::Endianness::Target e, DisplayListRemovalFromNameBlob& self)
{
	SwapEndianness(e, self.m_worldElementId);
	SwapEndianness(e, self.m_displayListName);
	SwapEndianness(e, self.m_displayListGroupName);
}

class DisplayListRemovalFromNameBlobBuilder : public BaseBlobBuilder<DisplayListRemovalFromNameBlob>
{
public:
	DisplayListRemovalFromNameBlobBuilder(const char* displayListName, const char* displayListGroupName, KyUInt32 worldElementId = KyUInt32MAXVAL)
		: m_displayListName(displayListName), m_displayListGroupName(displayListGroupName), m_worldElementId(worldElementId) {}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_worldElementId, m_worldElementId);
		BLOB_STRING(m_blob->m_displayListName, m_displayListName.ToCStr());
		BLOB_STRING(m_blob->m_displayListGroupName, m_displayListGroupName.ToCStr());
	}
private:
	Kaim::String m_displayListName;
	Kaim::String m_displayListGroupName;
	KyUInt32 m_worldElementId;
};

}

#endif // Navigation_VisualDebugIdRemovalBlob_H
