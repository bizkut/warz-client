/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_DiskExpansionQueryBlob_H
#define Navigation_DiskExpansionQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/queries/blobs/navhalfedgeblob.h"
#include "gwnavruntime/queries/utils/basediskexpansionquery.h"

namespace Kaim
{

class DiskExpansionQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DiskExpansionQueryOutputBlob() : m_result(0) {}

	DiskExpansionQueryResult GetResult() const { return (DiskExpansionQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavTriangleBlob m_centerTriangle;
	KyFloat32 m_resultRadius;
	Vec3f m_collisionPos3f;
	NavHalfEdgeBlob m_collisionHalfEdge;
	BlobRef<QueryDynamicOutputBlob> m_queryDynamicOutputBlobRef;
};
inline void SwapEndianness(Endianness::Target e, DiskExpansionQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result                   );
	SwapEndianness(e, self.m_centerTriangle           );
	SwapEndianness(e, self.m_resultRadius             );
	SwapEndianness(e, self.m_collisionPos3f           );
	SwapEndianness(e, self.m_collisionHalfEdge        );
	SwapEndianness(e, self.m_queryDynamicOutputBlobRef);
}


class DiskExpansionQueryOutputBlobBuilder : public BaseBlobBuilder<DiskExpansionQueryOutputBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DiskExpansionQueryOutputBlobBuilder(BaseDiskExpansionQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());

		BLOB_BUILD(m_blob->m_centerTriangle   , NavTriangleBlobBuilder(m_query->GetCenterTrianglePtr()));
		BLOB_SET(m_blob->m_resultRadius       , m_query->GetResultRadius());
		BLOB_SET(m_blob->m_collisionPos3f     , m_query->GetCollisionPos());
		BLOB_BUILD(m_blob->m_collisionHalfEdge, NavHalfEdgeBlobBuilder(m_query->GetCollisionNavHalfEdgePtr()));

		QueryDynamicOutput* queryDynamicOutput = m_query->GetQueryDynamicOutput();
		if (queryDynamicOutput != KY_NULL && queryDynamicOutput->IsEmpty() == false)
		{
			BUILD_REFERENCED_BLOB(m_blob->m_queryDynamicOutputBlobRef, QueryDynamicOutputBlobBuilder(queryDynamicOutput));
		}
	}

private:
	BaseDiskExpansionQuery* m_query;
};

class DiskExpansionQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, DiskExpansionQueryBlob, 0)
public:
	DiskExpansionQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

	DynamicOutputMode GetDynamicOutputMode() const { return (DynamicOutputMode)m_dynamicOutputMode; }

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_centerPos3f;
	KyFloat32 m_radiusMax;
	KyFloat32 m_safetyDist;
	PositionSpatializationRange m_positionSpatializationRange;
	KyUInt32 m_dynamicOutputMode; ///< this member is used to tell the Query what it should store into QueryDynamicOutput.

	BlobRef<DiskExpansionQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, DiskExpansionQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_centerPos3f);
	SwapEndianness(e, self.m_radiusMax);
	SwapEndianness(e, self.m_safetyDist);
	SwapEndianness(e, self.m_positionSpatializationRange);
	SwapEndianness(e, self.m_dynamicOutputMode);

	SwapEndianness(e, self.m_queryOutput);
}



class DiskExpansionQueryBlobBuilder : public BaseBlobBuilder<DiskExpansionQueryBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DiskExpansionQueryBlobBuilder(BaseDiskExpansionQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_centerPos3f                , m_query->GetCenterPos());
			BLOB_SET(m_blob->m_radiusMax                  , m_query->GetRadiusMax());
			BLOB_SET(m_blob->m_safetyDist                 , m_query->GetSafetyDist());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			BLOB_SET(m_blob->m_dynamicOutputMode          , (KyUInt32)m_query->GetDynamicOutputMode());

			switch (m_query->GetResult())
			{
			case DISKEXPANSION_NOT_INITIALIZED :
			case DISKEXPANSION_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, DiskExpansionQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseDiskExpansionQuery* m_query;
};
}

#endif // Kaim_DiskExpansionQueryInputBlob_H
