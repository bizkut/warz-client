/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_NavTagSubSegmentBlob_H
#define Navigation_NavTagSubSegmentBlob_H

#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class NavTrianglePtr;

// Blob to visual debug NavTagSubSegment information.
class NavTagSubSegmentBlob
{
public:
	NavTagSubSegmentBlob() {}

	Vec3f m_entrancePos3f;
	Vec3f m_exitPos3f;
	NavTag m_navTag;
};

inline void SwapEndianness(Kaim::Endianness::Target e, NavTagSubSegmentBlob& self)
{
	SwapEndianness(e, self.m_entrancePos3f);
	SwapEndianness(e, self.m_exitPos3f);
	SwapEndianness(e, self.m_navTag);
}


class NavTagSubSegmentBlobBuilder : public BaseBlobBuilder<NavTagSubSegmentBlob>
{
public:
	NavTagSubSegmentBlobBuilder(const NavTagSubSegment& navTagSubSegment) : m_navTagSubSegment(navTagSubSegment) {}
private:
	void DoBuild()
	{
		BLOB_SET(m_blob->m_entrancePos3f,m_navTagSubSegment.m_entrancePos3f);
		BLOB_SET(m_blob->m_exitPos3f,m_navTagSubSegment.m_exitPos3f);
		NavTagCopier builder(m_navTagSubSegment.m_navTrianglePtr.GetNavTag());
		BUILD_BLOB(m_blob->m_navTag, builder);
	}

	NavTagSubSegment m_navTagSubSegment;
};

}

#endif // Navigation_NavTagSubSegmentBlob_H
