/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_SpatializedPointCollectorInAABBQueryBlob_H
#define Navigation_SpatializedPointCollectorInAABBQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/queries/utils/basespatializedpointcollectorinaabbquery.h"

namespace Kaim
{

class SpatializedPointCollectorInAABBQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	SpatializedPointCollectorInAABBQueryOutputBlob() : m_result(0) {}

	SpatializedPointCollectorInAABBQueryResult GetResult() const { return (SpatializedPointCollectorInAABBQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavTriangleBlob m_startTriangle;
	QueryDynamicOutputBlob m_queryDynamicOutputBlob;
};
inline void SwapEndianness(Endianness::Target e, SpatializedPointCollectorInAABBQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_startTriangle);
	SwapEndianness(e, self.m_queryDynamicOutputBlob);
}


class SpatializedPointCollectorInAABBQueryOutputBlobBuilder : public BaseBlobBuilder<SpatializedPointCollectorInAABBQueryOutputBlob>
{
public:
	SpatializedPointCollectorInAABBQueryOutputBlobBuilder(BaseSpatializedPointCollectorInAABBQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_BUILD(m_blob->m_startTriangle, NavTriangleBlobBuilder(m_query->GetStartTrianglePtr()));
		QueryDynamicOutput* queryDynamicOutput = m_query->GetQueryDynamicOutput();
		if (queryDynamicOutput != KY_NULL && queryDynamicOutput->IsEmpty() == false)
		{
			BUILD_BLOB(m_blob->m_queryDynamicOutputBlob, QueryDynamicOutputBlobBuilder(queryDynamicOutput));
		}
	}

private:
	BaseSpatializedPointCollectorInAABBQuery* m_query;
};


class SpatializedPointCollectorInAABBQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, SpatializedPointCollectorInAABBQueryBlob, 0)
public:
	SpatializedPointCollectorInAABBQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_startPos3f;
	Box3f m_extentBox;
	PositionSpatializationRange m_positionSpatializationRange;

	BlobRef<SpatializedPointCollectorInAABBQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, SpatializedPointCollectorInAABBQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_extentBox);
	SwapEndianness(e, self.m_positionSpatializationRange);

	SwapEndianness(e, self.m_queryOutput);
}

class SpatializedPointCollectorInAABBQueryBlobBuilder : public BaseBlobBuilder<SpatializedPointCollectorInAABBQueryBlob>
{
public:
	SpatializedPointCollectorInAABBQueryBlobBuilder(BaseSpatializedPointCollectorInAABBQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_startPos3f                 , m_query->GetStartPos());
			BLOB_SET(m_blob->m_extentBox                  , m_query->GetExtentBox());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());

			switch (m_query->GetResult())
			{
			case SPATIALIZEDPOINTCOLLECTOR_NOT_INITIALIZED :
			case SPATIALIZEDPOINTCOLLECTOR_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, SpatializedPointCollectorInAABBQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseSpatializedPointCollectorInAABBQuery* m_query;
};
}

#endif // Kaim_SpatializedPointCollectorInAABBQueryInputBlob_H
