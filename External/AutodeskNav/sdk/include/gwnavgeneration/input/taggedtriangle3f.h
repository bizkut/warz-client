/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_ClientInputTriangle_H
#define GwNavGen_ClientInputTriangle_H


#include "gwnavruntime/math/triangle3f.h"
#include "gwnavruntime/database/navtag.h"

namespace Kaim
{

class DynamicTaggedTriangle3f
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynamicTaggedTriangle3f() {}
	DynamicTaggedTriangle3f(const Triangle3f& triangle) : m_triangle3f(triangle) {}
	DynamicTaggedTriangle3f(const Triangle3f& triangle, const DynamicNavTag& navTag) : m_triangle3f(triangle), m_navTag(navTag) {}

	Triangle3f m_triangle3f;
	DynamicNavTag m_navTag;
};

class TaggedTriangle3f
{
	KY_CLASS_WITHOUT_COPY(TaggedTriangle3f)
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	TaggedTriangle3f() : m_navTagIdx(KyUInt32MAXVAL) {}
	Triangle3f m_triangle3f;
	NavTag m_navTag;
	mutable KyUInt32 m_navTagIdx;
};
inline void SwapEndianness(Endianness::Target e, TaggedTriangle3f& self)
{
	SwapEndianness(e, self.m_triangle3f);
	SwapEndianness(e, self.m_navTag);
	SwapEndianness(e, self.m_navTagIdx);
}

class TaggedTriangle3fBlobBuilder : public BaseBlobBuilder<TaggedTriangle3f>
{
public:
	TaggedTriangle3fBlobBuilder(const DynamicTaggedTriangle3f& navTag) : m_dynamicInputTriangle(&navTag) {}
private:
	void DoBuild()
	{
		BLOB_SET(m_blob->m_triangle3f, m_dynamicInputTriangle->m_triangle3f);

		NavTagBlobBuilder navTagBuilder(m_dynamicInputTriangle->m_navTag);
		BLOB_BUILD(m_blob->m_navTag, navTagBuilder);
	}

	const DynamicTaggedTriangle3f* m_dynamicInputTriangle;
};

}


#endif

