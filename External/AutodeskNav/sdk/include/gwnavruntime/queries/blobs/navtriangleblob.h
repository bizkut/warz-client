/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_NavTriangleBlob_H
#define Navigation_NavTriangleBlob_H

#include "gwnavruntime/math/triangle3f.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/database/navtag.h"


namespace Kaim
{

class NavTrianglePtr;

// Blob to visual debug NavTriangle information.
class NavTriangleBlob
{
public:
	enum ValidityStatus
	{
		VALID,
		INVALID
	};
	NavTriangleBlob()
		: m_floorIdx(NavFloorIdx_Invalid)
		, m_triangleIdx(CompactNavTriangleIdx_MAXVAL)
		, m_status((KyUInt16)INVALID)
	{}

	ValidityStatus GetValidityStatus() const { return (ValidityStatus)m_status; }

	Triangle3f m_triangle;
	CellPos    m_cellPos; // The position of the cell that contains this triangle within the grid of PathData cells.
	NavFloorIdx m_floorIdx;
	CompactNavTriangleIdx m_triangleIdx;
	KyUInt16 m_status;
	NavTag m_navTag;
};

inline void SwapEndianness(Kaim::Endianness::Target e, NavTriangleBlob& self)
{
	SwapEndianness(e, self.m_triangle);
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_floorIdx);
	SwapEndianness(e, self.m_triangleIdx);
	SwapEndianness(e, self.m_status);
	SwapEndianness(e, self.m_navTag);
}


class NavTriangleBlobBuilder : public BaseBlobBuilder<NavTriangleBlob>
{
public:
	NavTriangleBlobBuilder(const NavTrianglePtr& trianglePtr) : m_trianglePtr(trianglePtr) {}
private:
	void DoBuild()
	{
		if (IsWriteMode())
		{
			if (m_trianglePtr.IsValid() == false)
			{
				m_blob->m_status = (KyUInt16)NavTriangleBlob::INVALID;
				return;
			}

			m_blob->m_floorIdx = (NavFloorIdx)m_trianglePtr.GetNavFloor()->m_idxInTheNavCellBuffer;
			m_blob->m_triangleIdx = (CompactNavTriangleIdx)m_trianglePtr.GetTriangleIdx();
			m_blob->m_cellPos = m_trianglePtr.GetCellPos();
			m_blob->m_status = (KyUInt16)NavTriangleBlob::VALID;
			m_trianglePtr.GetVerticesPos3f(m_blob->m_triangle);
		}

		if (m_trianglePtr.IsValid() == true)
		{
			BLOB_BUILD(m_blob->m_navTag, NavTagCopier(m_trianglePtr.GetNavTag()));
		}
	}

	NavTrianglePtr m_trianglePtr;
};

}

#endif // Kaim_NavTriangleBlob_H
