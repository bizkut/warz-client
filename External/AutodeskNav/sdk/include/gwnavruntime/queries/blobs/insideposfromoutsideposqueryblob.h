/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_InsidePosFromOutsidePosQueryBlob_H
#define Navigation_InsidePosFromOutsidePosQueryBlob_H

#include "gwnavruntime/queries/utils/baseinsideposfromoutsideposquery.h"
#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/database/database.h"

namespace Kaim
{

class InsidePosFromOutsidePosQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	InsidePosFromOutsidePosQueryOutputBlob() : m_result(0) {}

	InsidePosFromOutsidePosQueryResult GetResult() const { return (InsidePosFromOutsidePosQueryResult)m_result; }
public:
	KyUInt32 m_result;
	Vec3f m_insidePos3f;
	NavTriangleBlob m_insidePosTriangle;
};
inline void SwapEndianness(Endianness::Target e, InsidePosFromOutsidePosQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_insidePos3f);
	SwapEndianness(e, self.m_insidePosTriangle);
}


class InsidePosFromOutsidePosQueryOutputBlobBuilder : public BaseBlobBuilder<InsidePosFromOutsidePosQueryOutputBlob>
{
public:
	InsidePosFromOutsidePosQueryOutputBlobBuilder(BaseInsidePosFromOutsidePosQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_SET(m_blob->m_insidePos3f, m_query->GetInsidePos());
		BLOB_BUILD(m_blob->m_insidePosTriangle, NavTriangleBlobBuilder(m_query->GetInsidePosTrianglePtr()));
	}

private:
	BaseInsidePosFromOutsidePosQuery* m_query;
};


class InsidePosFromOutsidePosQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, InsidePosFromOutsidePosQueryBlob, 0)
public:
	InsidePosFromOutsidePosQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_inputPos3f;
	KyFloat32 m_distFromObstacle;
	KyFloat32 m_horizontalTolerance;
	PositionSpatializationRange m_positionSpatializationRange;

	BlobRef<InsidePosFromOutsidePosQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, InsidePosFromOutsidePosQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId        );
	SwapEndianness(e, self.m_databaseIdx        );
	SwapEndianness(e, self.m_inputPos3f         );
	SwapEndianness(e, self.m_distFromObstacle   );
	SwapEndianness(e, self.m_horizontalTolerance);
	SwapEndianness(e, self.m_positionSpatializationRange);

	SwapEndianness(e, self.m_queryOutput);
}

class InsidePosFromOutsidePosQueryBlobBuilder : public BaseBlobBuilder<InsidePosFromOutsidePosQueryBlob>
{
public:
	InsidePosFromOutsidePosQueryBlobBuilder(BaseInsidePosFromOutsidePosQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_inputPos3f                 , m_query->GetInputPos());
			BLOB_SET(m_blob->m_distFromObstacle           , m_query->GetDistFromObstacle());
			BLOB_SET(m_blob->m_horizontalTolerance        , m_query->GetHorizontalTolerance());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			
			switch (m_query->GetResult())
			{
			case INSIDEPOSFROMOUTSIDE_NOT_INITIALIZED :
			case INSIDEPOSFROMOUTSIDE_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, InsidePosFromOutsidePosQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseInsidePosFromOutsidePosQuery* m_query;
};
}

#endif // Kaim_InsidePosFromOutsidePosQueryInputBlob_H
