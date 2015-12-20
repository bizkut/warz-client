/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_RayCastQueryBlob_H
#define Navigation_RayCastQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/queries/blobs/navhalfedgeblob.h"
#include "gwnavruntime/queries/utils/baseraycastquery.h"

namespace Kaim
{

class RayCastQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	RayCastQueryOutputBlob() : m_result(0) {}

	RayCastQueryResult GetResult() const { return (RayCastQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavTriangleBlob m_startTriangle;
	NavTriangleBlob m_arrivalTriangle;
	Vec3f m_arrivalPos3f;
	Vec3f m_collisionPos3f;
	NavHalfEdgeBlob m_collisionHalfEdge;
	BlobRef<QueryDynamicOutputBlob> m_queryDynamicOutputBlobRef;
};
inline void SwapEndianness(Endianness::Target e, RayCastQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result                   );
	SwapEndianness(e, self.m_startTriangle            );
	SwapEndianness(e, self.m_arrivalTriangle          );
	SwapEndianness(e, self.m_arrivalPos3f             );
	SwapEndianness(e, self.m_collisionPos3f           );
	SwapEndianness(e, self.m_collisionHalfEdge        );
	SwapEndianness(e, self.m_queryDynamicOutputBlobRef);
}


class RayCastQueryOutputBlobBuilder : public BaseBlobBuilder<RayCastQueryOutputBlob>
{
public:
	RayCastQueryOutputBlobBuilder(BaseRayCastQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());

		BLOB_BUILD(m_blob->m_startTriangle    , NavTriangleBlobBuilder(m_query->GetStartTrianglePtr()));
		BLOB_BUILD(m_blob->m_arrivalTriangle  , NavTriangleBlobBuilder(m_query->GetArrivalTrianglePtr()));
		BLOB_SET(m_blob->m_arrivalPos3f       , m_query->GetArrivalPos());
		BLOB_SET(m_blob->m_collisionPos3f     , m_query->GetCollisionPos());
		BLOB_BUILD(m_blob->m_collisionHalfEdge, NavHalfEdgeBlobBuilder(m_query->GetCollisionNavHalfEdgePtr()));

		QueryDynamicOutput* queryDynamicOutput = m_query->GetQueryDynamicOutput();
		if (queryDynamicOutput != KY_NULL && queryDynamicOutput->IsEmpty() == false)
		{
			BUILD_REFERENCED_BLOB(m_blob->m_queryDynamicOutputBlobRef, QueryDynamicOutputBlobBuilder(queryDynamicOutput));
		}
	}

private:
	BaseRayCastQuery* m_query;
};

class RayCastQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, RayCastQueryBlob, 0)
public:
	RayCastQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

	DynamicOutputMode GetDynamicOutputMode() const { return (DynamicOutputMode)m_dynamicOutputMode; }

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_startPos3f;
	Vec2f m_maxMove2D;
	PositionSpatializationRange m_positionSpatializationRange;
	KyUInt32 m_dynamicOutputMode; ///< this member is used to tell the Query what it should store into QueryDynamicOutput.

	BlobRef<RayCastQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, RayCastQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_maxMove2D);
	SwapEndianness(e, self.m_positionSpatializationRange);
	SwapEndianness(e, self.m_dynamicOutputMode);

	SwapEndianness(e, self.m_queryOutput);
}



class RayCastQueryBlobBuilder : public BaseBlobBuilder<RayCastQueryBlob>
{
public:
	RayCastQueryBlobBuilder(BaseRayCastQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_startPos3f                 , m_query->GetStartPos());
			BLOB_SET(m_blob->m_maxMove2D                  , m_query->GetMaxMove2D());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			BLOB_SET(m_blob->m_dynamicOutputMode          , (KyUInt32)m_query->GetDynamicOutputMode());

			switch (m_query->GetResult())
			{
			case RAYCAST_NOT_INITIALIZED :
			case RAYCAST_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, RayCastQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseRayCastQuery* m_query;
};
}

#endif // Kaim_RayCastQueryInputBlob_H
