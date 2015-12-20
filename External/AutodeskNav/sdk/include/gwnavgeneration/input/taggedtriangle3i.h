/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_TaggedTriangle3i_H
#define GwNavGen_TaggedTriangle3i_H

#include "gwnavruntime/math/triangle3i.h"
#include "gwnavruntime/database/navtag.h"

namespace Kaim
{

class DynamicTaggedTriangle3i
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	DynamicTaggedTriangle3i() {}
	DynamicTaggedTriangle3i(const Triangle3i& triangle) : m_triangle3i(triangle) {}
	DynamicTaggedTriangle3i(const Triangle3i& triangle, const DynamicNavTag& navTag) : m_triangle3i(triangle), m_dynamicNavTag(navTag) {}

public:
	Triangle3i m_triangle3i;
	DynamicNavTag m_dynamicNavTag;
};

class TaggedTriangle3i
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	TaggedTriangle3i(): m_navTagIdx(KyUInt32MAXVAL) {}
	bool IsWalkable() const { return m_navTag.IsWalkable(); }

public:
	Triangle3i m_triangle3i;
	NavTag m_navTag;
	mutable KyUInt32 m_navTagIdx; // index in CellGenerator::m_navTagArray
	// BIG WARNING.. THIS weird mutable KyUInt32 m_navTagIdx WORKS IN MULTITHREAD GENERATION ONLY BECAUSE THE PARALLEL FOR NEVER PROCESS 2 NavCells at the same CellPos
};

inline void SwapEndianness(Endianness::Target e, TaggedTriangle3i& self)
{
	SwapEndianness(e, self.m_triangle3i);
	SwapEndianness(e, self.m_navTag);
	SwapEndianness(e, self.m_navTagIdx);
}

class TaggedTriangle3iBlobBuilder : public BaseBlobBuilder<TaggedTriangle3i>
{
public:
	TaggedTriangle3iBlobBuilder(const DynamicTaggedTriangle3i& dynamicTaggedTriangle3i) : m_dynamicTaggedTriangle3i(&dynamicTaggedTriangle3i) {}
private:
	void DoBuild()
	{
		BLOB_SET(m_blob->m_triangle3i, m_dynamicTaggedTriangle3i->m_triangle3i);
		BLOB_BUILD(m_blob->m_navTag, NavTagBlobBuilder(m_dynamicTaggedTriangle3i->m_dynamicNavTag));
	}

	const DynamicTaggedTriangle3i* m_dynamicTaggedTriangle3i;
};

class TaggedTriangle3iBlobCopier : public BaseBlobBuilder<TaggedTriangle3i>
{
public:
	TaggedTriangle3iBlobCopier(const TaggedTriangle3i& taggedTriangle3i) : m_triangle3i(&taggedTriangle3i) {}
private:
	void DoBuild()
	{
		BLOB_SET(m_blob->m_triangle3i, m_triangle3i->m_triangle3i);
		BLOB_BUILD(m_blob->m_navTag, NavTagCopier(m_triangle3i->m_navTag));
	}

	const TaggedTriangle3i* m_triangle3i;
};

}


#endif

