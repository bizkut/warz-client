/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_TriangleFromPosQueryBlob_H
#define Navigation_TriangleFromPosQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/queries/trianglefromposquery.h"
#include "gwnavruntime/database/database.h"

namespace Kaim
{

class TriangleFromPosQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	TriangleFromPosQueryOutputBlob() : m_result(0) {}

	TriangleFromPosQueryResult GetResult() const { return (TriangleFromPosQueryResult)m_result; }
public:
	KyUInt32 m_result;
	NavTriangleBlob m_resultTrianglePtr;
	KyFloat32 m_altitudeOfProjection;
};
inline void SwapEndianness(Endianness::Target e, TriangleFromPosQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_resultTrianglePtr);
	SwapEndianness(e, self.m_altitudeOfProjection );
}


class TriangleFromPosQueryOutputBlobBuilder : public BaseBlobBuilder<TriangleFromPosQueryOutputBlob>
{
public:
	TriangleFromPosQueryOutputBlobBuilder(TriangleFromPosQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_SET(m_blob->m_altitudeOfProjection, m_query->GetAltitudeOfProjectionInTriangle());
		BLOB_BUILD(m_blob->m_resultTrianglePtr, NavTriangleBlobBuilder(m_query->GetResultTrianglePtr()));
	}

private:
	TriangleFromPosQuery* m_query;
};


class TriangleFromPosQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, TriangleFromPosQueryBlob, 0)
public:
	TriangleFromPosQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

	TriangleFromPosQueryType GetQueryType() const { return (TriangleFromPosQueryType)m_queryType; }

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_inputPos3f;
	KyUInt32 m_queryType;
	PositionSpatializationRange m_positionSpatializationRange;

	BlobRef<TriangleFromPosQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, TriangleFromPosQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_inputPos3f);
	SwapEndianness(e, self.m_queryType);
	SwapEndianness(e, self.m_positionSpatializationRange);

	SwapEndianness(e, self.m_queryOutput);
}

class TriangleFromPosQueryBlobBuilder : public BaseBlobBuilder<TriangleFromPosQueryBlob>
{
public:
	TriangleFromPosQueryBlobBuilder(TriangleFromPosQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_inputPos3f                 , m_query->GetInputPos());
			BLOB_SET(m_blob->m_queryType                  , (KyUInt32)m_query->GetQueryType());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());

			switch (m_query->GetResult())
			{
			case TRIANGLEFROMPOS_NOT_INITIALIZED :
			case TRIANGLEFROMPOS_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, TriangleFromPosQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	TriangleFromPosQuery* m_query;
};
}

#endif // Kaim_TriangleFromPosQueryInputBlob_H
