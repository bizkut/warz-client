/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_DiskCollisionQueryBlob_H
#define Navigation_DiskCollisionQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/queries/utils/basediskcollisionquery.h"

namespace Kaim
{

class DiskCollisionQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DiskCollisionQueryOutputBlob() : m_result(0) {}

	DiskCollisionQueryResult GetResult() const { return (DiskCollisionQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavTriangleBlob m_centerTriangle;
	BlobRef<QueryDynamicOutputBlob> m_queryDynamicOutputBlobRef;
};
inline void SwapEndianness(Endianness::Target e, DiskCollisionQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result                   );
	SwapEndianness(e, self.m_centerTriangle           );
	SwapEndianness(e, self.m_queryDynamicOutputBlobRef);
}


class DiskCollisionQueryOutputBlobBuilder : public BaseBlobBuilder<DiskCollisionQueryOutputBlob>
{
public:
	DiskCollisionQueryOutputBlobBuilder(BaseDiskCollisionQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());

		BLOB_BUILD(m_blob->m_centerTriangle, NavTriangleBlobBuilder(m_query->GetCenterTrianglePtr()));

		QueryDynamicOutput* queryDynamicOutput = m_query->GetQueryDynamicOutput();
		if (queryDynamicOutput != KY_NULL && queryDynamicOutput->IsEmpty() == false)
		{
			BUILD_REFERENCED_BLOB(m_blob->m_queryDynamicOutputBlobRef, QueryDynamicOutputBlobBuilder(queryDynamicOutput));
		}
	}

private:
	BaseDiskCollisionQuery* m_query;
};

class DiskCollisionQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, DiskCollisionQueryBlob, 0)
public:
	DiskCollisionQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

	DynamicOutputMode GetDynamicOutputMode() const { return (DynamicOutputMode)m_dynamicOutputMode; }

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_centerPos3f;
	KyFloat32 m_radius;
	PositionSpatializationRange m_positionSpatializationRange;
	KyUInt32 m_dynamicOutputMode; ///< this member is used to tell the Query what it should store into QueryDynamicOutput.

	BlobRef<DiskCollisionQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, DiskCollisionQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_centerPos3f);
	SwapEndianness(e, self.m_radius);
	SwapEndianness(e, self.m_positionSpatializationRange);
	SwapEndianness(e, self.m_dynamicOutputMode);

	SwapEndianness(e, self.m_queryOutput);
}



class DiskCollisionQueryBlobBuilder : public BaseBlobBuilder<DiskCollisionQueryBlob>
{
public:
	DiskCollisionQueryBlobBuilder(BaseDiskCollisionQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_centerPos3f                , m_query->GetCenterPos());
			BLOB_SET(m_blob->m_radius                     , m_query->GetRadius());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			BLOB_SET(m_blob->m_dynamicOutputMode          , (KyUInt32)m_query->GetDynamicOutputMode());

			switch (m_query->GetResult())
			{
			case DISKCOLLISION_NOT_INITIALIZED :
			case DISKCOLLISION_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, DiskCollisionQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseDiskCollisionQuery* m_query;
};
}

#endif // Kaim_DiskCollisionQueryInputBlob_H
