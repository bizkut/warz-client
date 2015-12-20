/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_NavHalfEdgeBlob_H
#define Navigation_NavHalfEdgeBlob_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"
#include "gwnavruntime/blob/baseblobbuilder.h"


namespace Kaim
{

class NavHalfEdgePtr;

// Blob to visual debug NavTriangle information.
class NavHalfEdgeBlob
{
public:
	enum ValidityStatus
	{
		VALID,
		INVALID
	};

	NavHalfEdgeBlob()
		: m_halfEdgeIdx(CompactNavHalfEdgeIdx_MAXVAL)
		, m_status((KyUInt16)INVALID)
	{}

	ValidityStatus GetValidityStatus() const { return (ValidityStatus)m_status; }

	Vec3f m_startPos3f;
	Vec3f m_endPos3f;
	CellPos m_cellPos; // The position of the cell that contains this triangle within the grid of PathData cells.
	CompactNavHalfEdgeIdx m_halfEdgeIdx;
	KyUInt16 m_status;
};

inline void SwapEndianness(Kaim::Endianness::Target e, NavHalfEdgeBlob& self)
{
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_endPos3f);
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_halfEdgeIdx);
	SwapEndianness(e, self.m_status);
}


class NavHalfEdgeBlobBuilder : public BaseBlobBuilder<NavHalfEdgeBlob>
{
public:
	NavHalfEdgeBlobBuilder(const NavHalfEdgePtr& halfEdgePtr) : m_halfEdgePtr(halfEdgePtr) {}
private:
	void DoBuild()
	{
		if (IsWriteMode())
		{
			if (m_halfEdgePtr.IsValid() == false)
			{
				m_blob->m_status = (KyUInt16)NavHalfEdgeBlob::INVALID;
				return;
			}

			m_blob->m_halfEdgeIdx = (CompactNavHalfEdgeIdx)m_halfEdgePtr.GetHalfEdgeIdx();
			m_blob->m_cellPos = m_halfEdgePtr.GetCellPos();
			m_blob->m_status = (KyUInt16)NavHalfEdgeBlob::VALID;
			m_halfEdgePtr.GetVerticesPos3f(m_blob->m_startPos3f, m_blob->m_endPos3f);
		}
	}

	NavHalfEdgePtr m_halfEdgePtr;
};
}

#endif // Navigation_NavHalfEdgeBlob_H
