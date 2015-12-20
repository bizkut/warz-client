/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_DiskCanGoQueryBlob_H
#define Navigation_DiskCanGoQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/queries/utils/basediskcangoquery.h"

namespace Kaim
{

class DiskCanGoQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DiskCanGoQueryOutputBlob() : m_result(0) {}

	DiskCanGoQueryResult GetResult() const { return (DiskCanGoQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavTriangleBlob m_startTriangle;
	NavTriangleBlob m_destTriangle;
	BlobRef<QueryDynamicOutputBlob> m_queryDynamicOutputBlobRef;
};
inline void SwapEndianness(Endianness::Target e, DiskCanGoQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_startTriangle);
	SwapEndianness(e, self.m_destTriangle);
	SwapEndianness(e, self.m_queryDynamicOutputBlobRef);
}


class DiskCanGoQueryOutputBlobBuilder : public BaseBlobBuilder<DiskCanGoQueryOutputBlob>
{
public:
	DiskCanGoQueryOutputBlobBuilder(BaseDiskCanGoQuery* query) { m_query = query; }
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
	BaseDiskCanGoQuery* m_query;
};



class DiskCanGoQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, DiskCanGoQueryBlob, 0)
public:
	DiskCanGoQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

	DynamicOutputMode GetDynamicOutputMode() const { return (DynamicOutputMode)m_dynamicOutputMode; }

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_startPos3f;
	Vec3f m_destPos3f;
	KyFloat32 m_radius;
	PositionSpatializationRange m_positionSpatializationRange;
	KyUInt32 m_dynamicOutputMode; ///< this member is used to tell the Query what it should store into QueryDynamicOutput.

	BlobRef<DiskCanGoQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, DiskCanGoQueryBlob& self)
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

class DiskCanGoQueryBlobBuilder : public BaseBlobBuilder<DiskCanGoQueryBlob>
{
public:
	DiskCanGoQueryBlobBuilder(BaseDiskCanGoQuery* query) { m_query = query; }
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
			case DISKCANGO_NOT_INITIALIZED :
			case DISKCANGO_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, DiskCanGoQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseDiskCanGoQuery* m_query;
};
}

#endif // Kaim_DiskCanGoQueryInputBlob_H
