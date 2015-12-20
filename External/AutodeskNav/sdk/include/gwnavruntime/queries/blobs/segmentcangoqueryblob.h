/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_SegmentCanGoQueryBlob_H
#define Navigation_SegmentCanGoQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/queries/utils/basesegmentcangoquery.h"

namespace Kaim
{

class SegmentCanGoQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	SegmentCanGoQueryOutputBlob() : m_result(0) {}

	SegmentCanGoQueryResult GetResult() const { return (SegmentCanGoQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavTriangleBlob m_startTriangle;
	NavTriangleBlob m_destTriangle;
	BlobRef<QueryDynamicOutputBlob> m_queryDynamicOutputBlobRef;
};
inline void SwapEndianness(Endianness::Target e, SegmentCanGoQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_startTriangle);
	SwapEndianness(e, self.m_destTriangle);
	SwapEndianness(e, self.m_queryDynamicOutputBlobRef);
}


class SegmentCanGoQueryOutputBlobBuilder : public BaseBlobBuilder<SegmentCanGoQueryOutputBlob>
{
public:
	SegmentCanGoQueryOutputBlobBuilder(BaseSegmentCanGoQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_BUILD(m_blob->m_startTriangle, NavTriangleBlobBuilder(m_query->GetStartTrianglePtr()));
		BLOB_BUILD(m_blob->m_destTriangle, NavTriangleBlobBuilder(m_query->GetDestTrianglePtr()));
		QueryDynamicOutput* queryDynamicOutput = m_query->GetQueryDynamicOutput();
		if (queryDynamicOutput != KY_NULL && queryDynamicOutput->IsEmpty() == false)
		{
			BUILD_REFERENCED_BLOB(m_blob->m_queryDynamicOutputBlobRef, QueryDynamicOutputBlobBuilder(queryDynamicOutput));
		}
	}

private:
	BaseSegmentCanGoQuery* m_query;
};



class SegmentCanGoQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, SegmentCanGoQueryBlob, 0)
public:
	SegmentCanGoQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

	DynamicOutputMode GetDynamicOutputMode() const { return (DynamicOutputMode)m_dynamicOutputMode; }

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_startPos3f;
	Vec3f m_destPos3f;
	KyFloat32 m_radius;
	PositionSpatializationRange m_positionSpatializationRange;
	KyUInt32 m_dynamicOutputMode; ///< this member is used to tell the Query what it should store into QueryDynamicOutput.

	BlobRef<SegmentCanGoQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, SegmentCanGoQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_destPos3f);
	SwapEndianness(e, self.m_radius);
	SwapEndianness(e, self.m_positionSpatializationRange);
	SwapEndianness(e, self.m_dynamicOutputMode);

	SwapEndianness(e, self.m_queryOutput);
}


class SegmentCanGoQueryBlobBuilder : public BaseBlobBuilder<SegmentCanGoQueryBlob>
{
public:
	SegmentCanGoQueryBlobBuilder(BaseSegmentCanGoQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_startPos3f                 , m_query->GetStartPos());
			BLOB_SET(m_blob->m_destPos3f                  , m_query->GetDestPos());
			BLOB_SET(m_blob->m_radius                     , m_query->GetRadius());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			BLOB_SET(m_blob->m_dynamicOutputMode          , (KyUInt32)m_query->GetDynamicOutputMode());
			
			switch (m_query->GetResult())
			{
			case SEGMENTCANGO_NOT_INITIALIZED :
			case SEGMENTCANGO_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, SegmentCanGoQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseSegmentCanGoQuery* m_query;
};
}

#endif // Kaim_SegmentCanGoQueryInputBlob_H
