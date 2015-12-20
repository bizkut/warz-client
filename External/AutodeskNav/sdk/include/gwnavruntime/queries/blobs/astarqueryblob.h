/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_AStarQueryBlob_H
#define Navigation_AStarQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/queries/blobs/navhalfedgeblob.h"
#include "gwnavruntime/queries/blobs/timeslicedqueryinfoblob.h"
#include "gwnavruntime/queries/astarquery.h"
#include "gwnavruntime/path/pathblob.h"

namespace Kaim
{

class AStarQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	AStarQueryOutputBlob() : m_result(0) {}

	AStarQueryResult GetResult() const { return (AStarQueryResult)m_result; }
public:
	KyUInt32 m_result;
	TimeSlicedQueryInfoBlob m_timeSlicedQueryInfo;
	NavTriangleBlob m_startTriangle;
	NavTriangleBlob m_destTriangle;
	PathBlob m_pathBlob;
	PathBlob m_abstractPathBlob;
};
inline void SwapEndianness(Endianness::Target e, AStarQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_timeSlicedQueryInfo);
	SwapEndianness(e, self.m_startTriangle);
	SwapEndianness(e, self.m_destTriangle);
	SwapEndianness(e, self.m_abstractPathBlob);
}


class AStarQueryOutputBlobBuilder : public BaseBlobBuilder<AStarQueryOutputBlob>
{
public:
	AStarQueryOutputBlobBuilder(BaseAStarQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_BUILD(m_blob->m_timeSlicedQueryInfo, TimeSlicedQueryBlobBuilder(m_query));
		BLOB_BUILD(m_blob->m_startTriangle, NavTriangleBlobBuilder(m_query->GetStartTrianglePtr()));
		BLOB_BUILD(m_blob->m_destTriangle, NavTriangleBlobBuilder(m_query->GetDestTrianglePtr()));
		
		if (m_query->GetResult() == ASTAR_DONE_PATH_FOUND)
		{
			BLOB_BUILD(m_blob->m_pathBlob, PathBlobBuilder(m_query->GetPath()));
			BLOB_BUILD(m_blob->m_abstractPathBlob, PathBlobBuilder(m_query->GetAbstractPath()));
		}
	}

private:
	BaseAStarQuery* m_query;
};

class AStarQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, AStarQueryBlob, 3)
public:
	AStarQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	KyUInt32 m_tryCanGoMode;
	KyUInt32 m_channelMode;
	KyFloat32 m_fromOutsideNavMeshDistance;
	KyFloat32 m_toOutsideNavMeshDistance;
	KyFloat32 m_propagationBoxExtent;
	KyUInt32  m_abstractGraphTraversalMode;

	Vec3f m_startPos3f;
	Vec3f m_destPos3f;
	PositionSpatializationRange m_positionSpatializationRange;
	BlobRef<AStarQueryOutputBlob> m_queryOutput;

};
inline void SwapEndianness(Endianness::Target e, AStarQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_tryCanGoMode);
	SwapEndianness(e, self.m_channelMode);
	SwapEndianness(e, self.m_fromOutsideNavMeshDistance);
	SwapEndianness(e, self.m_toOutsideNavMeshDistance);
	SwapEndianness(e, self.m_propagationBoxExtent);
	SwapEndianness(e, self.m_abstractGraphTraversalMode);
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_destPos3f);
	SwapEndianness(e, self.m_positionSpatializationRange);

	SwapEndianness(e, self.m_queryOutput);
}



class AStarQueryBlobBuilder : public BaseBlobBuilder<AStarQueryBlob>
{
public:
	AStarQueryBlobBuilder(BaseAStarQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_startPos3f                 , m_query->GetStartPos());
			BLOB_SET(m_blob->m_destPos3f                  , m_query->GetDestPos());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			BLOB_SET(m_blob->m_tryCanGoMode               , (KyUInt32)m_query->GetTryCanGoMode());
			BLOB_SET(m_blob->m_channelMode                , (KyUInt32)m_query->GetComputeChannelMode());
			BLOB_SET(m_blob->m_fromOutsideNavMeshDistance , m_query->GetFromOutsideNavMeshDistance());
			BLOB_SET(m_blob->m_toOutsideNavMeshDistance   , m_query->GetToOutsideNavMeshDistance());
			BLOB_SET(m_blob->m_propagationBoxExtent       , m_query->GetPropagationBoxExtent());
			BLOB_SET(m_blob->m_abstractGraphTraversalMode , m_query->GetAbstractGraphTraversalMode());

			switch (m_query->GetResult())
			{
			case ASTAR_NOT_INITIALIZED :
			case ASTAR_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, AStarQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseAStarQuery* m_query;
};
}

#endif // Kaim_AstarQueryInputBlob_H
