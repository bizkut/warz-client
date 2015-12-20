/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_NearestObstacleHalfEdgeFromPosQueryBlob_H
#define Navigation_NearestObstacleHalfEdgeFromPosQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/navhalfedgeblob.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/queries/utils/basenearestborderhalfedgefromposquery.h"
#include "gwnavruntime/database/database.h"

namespace Kaim
{

class NearestBorderHalfEdgeFromPosQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	NearestBorderHalfEdgeFromPosQueryOutputBlob() : m_result(0) {}

	NearestBorderHalfEdgeFromPosQueryResult GetResult() const { return (NearestBorderHalfEdgeFromPosQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavHalfEdgeBlob m_nearestHalfEdgeOnBorder;
	NavHalfEdgeBlob m_nextHalfEdgeOnBorder;
	NavHalfEdgeBlob m_prevHalfEdgeOnBorder;
	Vec3f m_nearestPosOnHalfEdge;
	KyFloat32 m_squareDistFromHalfEdge;
};
inline void SwapEndianness(Endianness::Target e, NearestBorderHalfEdgeFromPosQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_nearestHalfEdgeOnBorder);
	SwapEndianness(e, self.m_nextHalfEdgeOnBorder   );
	SwapEndianness(e, self.m_prevHalfEdgeOnBorder   );
	SwapEndianness(e, self.m_nearestPosOnHalfEdge   );
	SwapEndianness(e, self.m_squareDistFromHalfEdge );
}


class NearestBorderHalfEdgeFromPosQueryOutputBlobBuilder : public BaseBlobBuilder<NearestBorderHalfEdgeFromPosQueryOutputBlob>
{
public:
	NearestBorderHalfEdgeFromPosQueryOutputBlobBuilder(BaseNearestBorderHalfEdgeFromPosQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_BUILD(m_blob->m_nearestHalfEdgeOnBorder, NavHalfEdgeBlobBuilder(m_query->GetNearestHalfEdgePtrOnBorder()));
		BLOB_BUILD(m_blob->m_nextHalfEdgeOnBorder   , NavHalfEdgeBlobBuilder(m_query->GetNextHalfEdgePtrAlongBorder()));
		BLOB_BUILD(m_blob->m_prevHalfEdgeOnBorder   , NavHalfEdgeBlobBuilder(m_query->GetPrevHalfEdgePtrAlongBorder()));
		BLOB_SET(m_blob->m_nearestPosOnHalfEdge, m_query->GetNearestPosOnHalfEdge());
		BLOB_SET(m_blob->m_squareDistFromHalfEdge, m_query->GetSquareDistFromNearestBorder());
	}

private:
	BaseNearestBorderHalfEdgeFromPosQuery* m_query;
};


class NearestBorderHalfEdgeFromPosQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, NearestBorderHalfEdgeFromPosQueryBlob, 0)
public:
	NearestBorderHalfEdgeFromPosQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_inputPos3f;
	KyFloat32 m_horizontalTolerance;
	PositionSpatializationRange m_positionSpatializationRange;

	BlobRef<NearestBorderHalfEdgeFromPosQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, NearestBorderHalfEdgeFromPosQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_inputPos3f);
	SwapEndianness(e, self.m_horizontalTolerance);
	SwapEndianness(e, self.m_positionSpatializationRange);

	SwapEndianness(e, self.m_queryOutput);
}

class NearestBorderHalfEdgeFromPosQueryBlobBuilder : public BaseBlobBuilder<NearestBorderHalfEdgeFromPosQueryBlob>
{
public:
	NearestBorderHalfEdgeFromPosQueryBlobBuilder(BaseNearestBorderHalfEdgeFromPosQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_inputPos3f                 , m_query->GetInputPos());
			BLOB_SET(m_blob->m_horizontalTolerance        , m_query->GetHorizontalTolerance());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());

			switch (m_query->GetResult())
			{
			case NEARESTHALFEDGE_NOT_INITIALIZED :
			case NEARESTHALFEDGE_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, NearestBorderHalfEdgeFromPosQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseNearestBorderHalfEdgeFromPosQuery* m_query;
};
}

#endif // Kaim_NearestObstacleHalfEdgeFromPosQueryInputBlob_H
