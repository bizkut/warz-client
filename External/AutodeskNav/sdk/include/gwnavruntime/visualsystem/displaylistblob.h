/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: MAMU
#ifndef Navigation_DisplayListBlob_H
#define Navigation_DisplayListBlob_H


#include "gwnavruntime/visualsystem/visualshapecolor.h"
#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/math/transform.h"
#include "gwnavruntime/math/box3f.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_Debug.h"


namespace Kaim
{

class DisplayShape
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	DisplayShape()
		: m_displayShapeType(KyUInt32MAXVAL)
	{}

	void Clear() { m_data.Clear(); }
	void Reserve(KyUInt32 numberOfFloat) { m_data.Reserve(numberOfFloat); }
	void Push(KyUInt32 value) { m_data.PushBack((KyFloat32)value); }
	void Push(KyFloat32 value) { m_data.PushBack(value); }

	void Push3(const Vec3f& pos)
	{
		m_data.PushBack(pos.x);
		m_data.PushBack(pos.y);
		m_data.PushBack(pos.z);
	}

	void Push6(const Box3f& box)
	{
		Push3(box.m_min);
		Push3(box.m_max);
	}

	void Push12(const Transform& transform)
	{
		Push3(transform.m_rotationMatrix.m_vX);
		Push3(transform.m_rotationMatrix.m_vY);
		Push3(transform.m_rotationMatrix.m_vZ);
		Push3(transform.m_translation);
	}

public: // internal
	KyUInt32 m_displayShapeType;
	KyArrayPOD<KyFloat32, MemStat_VisualSystem> m_data;
	VisualShapeColor m_color;
};

class DisplayShapeBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	KyUInt32 m_displayShapeType; //enum DisplayShapeType
	BlobArray<KyFloat32> m_data;
	VisualColor m_triangleColor;
	VisualColor m_lineColor;
};

inline void SwapEndianness(Kaim::Endianness::Target e, DisplayShapeBlob& self)
{
	SwapEndianness(e, self.m_displayShapeType);
	SwapEndianness(e, self.m_data);
	SwapEndianness(e, self.m_triangleColor);
	SwapEndianness(e, self.m_lineColor);
}


class DisplayText
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	Vec3f m_position;
	KyArrayPOD<char, MemStat_VisualSystem> m_text;
	VisualColor m_color;
};

class DisplayTextBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	Vec3f m_position;
	BlobArray<char> m_text;
	VisualColor m_color;
};

inline void SwapEndianness(Kaim::Endianness::Target e, DisplayTextBlob& self)
{
	SwapEndianness(e, self.m_position);
	SwapEndianness(e, self.m_text);
	SwapEndianness(e, self.m_color);
}

/// Indicates if the displayList will be only visible when received, or will be remanent until replaced or removed.
enum DisplayListLifeSpan
{
	DisplayListLifeSpan_SingleFrame,     /// displayList available in the NavigationLab only at the frame when it was sent
	DisplayListLifeSpan_UserControlled   /// displayList available in the NavigationLab until replaced, removed, or associated world element is destroyed
};

class DisplayListBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)
	KY_ROOT_BLOB_CLASS(VisualDebug, DisplayListBlob, 1)

public:
	DisplayListLifeSpan GetDisplayListLifeSpan() const
	{
		if (m_displayListId == KyUInt32MAXVAL ||
			m_displayListId == GetDisplayListIdForLifeSpanUserControlledWithoutDisplayListId())
		{
			return DisplayListLifeSpan_UserControlled;
		}
		else
		{
			return DisplayListLifeSpan_SingleFrame;
		}
	}

	static KyUInt32 GetDisplayListIdForLifeSpanUserControlledWithoutDisplayListId() { return KyUInt32MAXVAL - 1; }

public:
	BlobArray<char> m_listName;
	BlobArray<char> m_groupName; /// group under which the displayList will be put, if not specified m_worldElementId should be
	BlobArray<DisplayTextBlob> m_textBlobs;
	BlobArray<DisplayShapeBlob> m_shapeBlobs;
	KyUInt32 m_notSelectedState;
	KyUInt32 m_selectedState;
	KyUInt32 m_worldElementId; /// Specifies the visualDebugId of a worldElement with which it should be associated in the Lab, if not specified m_groupName should be
	KyUInt32 m_displayListId;  /// Unique identification of a displayList being visualdebugged with a life > 1 frame (not mandatory)
};

inline void SwapEndianness(Kaim::Endianness::Target e, DisplayListBlob& self)
{
	SwapEndianness(e, self.m_displayListId);
	SwapEndianness(e, self.m_worldElementId);
	SwapEndianness(e, self.m_listName);
	SwapEndianness(e, self.m_groupName);
	SwapEndianness(e, self.m_textBlobs);
	SwapEndianness(e, self.m_shapeBlobs);
	SwapEndianness(e, self.m_notSelectedState);
	SwapEndianness(e, self.m_selectedState);
}

}

#endif // Navigation_DisplayListBlob_H
