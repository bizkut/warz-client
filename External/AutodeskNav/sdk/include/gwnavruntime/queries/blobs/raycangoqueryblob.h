/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_RayCanGoQueryBlob_H
#define Navigation_RayCanGoQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/queries/utils/baseraycangoquery.h"

namespace Kaim
{

class RayCanGoQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	RayCanGoQueryOutputBlob() : m_result(0), m_cost(KyFloat32MAXVAL) {}

	RayCanGoQueryResult GetResult() const { return (RayCanGoQueryResult)m_result; }
public:
	KyUInt32 m_result;
	KyFloat32 m_cost;
	NavTriangleBlob m_startTriangle;
	NavTriangleBlob m_destTriangle;
	BlobRef<QueryDynamicOutputBlob> m_queryDynamicOutputBlobRef;
};
inline void SwapEndianness(Endianness::Target e, RayCanGoQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_cost);
	SwapEndianness(e, self.m_startTriangle);
	SwapEndianness(e, self.m_destTriangle);
	SwapEndianness(e, self.m_queryDynamicOutputBlobRef);
}


class RayCanGoQueryOutputBlobBuilder : public BaseBlobBuilder<RayCanGoQueryOutputBlob>
{
public:
	RayCanGoQueryOutputBlobBuilder(BaseRayCanGoQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_SET(m_blob->m_cost, m_query->GetComputedCost());
		BLOB_BUILD(m_blob->m_startTriangle, NavTriangleBlobBuilder(m_query->GetStartTrianglePtr()));
		BLOB_BUILD(m_blob->m_destTriangle, NavTriangleBlobBuilder(m_query->GetDestTrianglePtr()));
		QueryDynamicOutput* queryDynamicOutput = m_query->GetQueryDynamicOutput();
		if (queryDynamicOutput != KY_NULL && queryDynamicOutput->IsEmpty() == false)
		{
			BUILD_REFERENCED_BLOB(m_blob->m_queryDynamicOutputBlobRef, QueryDynamicOutputBlobBuilder(queryDynamicOutput));
		}
	}

private:
	BaseRayCanGoQuery* m_query;
};


class RayCanGoQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, RayCanGoQueryBlob, 1)
public:
	RayCanGoQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

	DynamicOutputMode GetDynamicOutputMode() const { return (DynamicOutputMode)m_dynamicOutputMode; }
	ComputeCostMode GetComputeCostMode() const { return (ComputeCostMode)m_computeCostMode; }

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_startPos3f;
	Vec3f m_destPos3f;
	PositionSpatializationRange m_positionSpatializationRange;
	KyUInt32 m_marginMode;
	KyUInt32 m_dynamicOutputMode; ///< this member is used to tell the Query what it should store into QueryDynamicOutput.
	KyUInt32 m_computeCostMode; ///< this member is used to tell the Query what it should store into QueryDynamicOutput.

	KyFloat32 m_margin; // The float value corresponding to m_marginMode.
	BlobRef<RayCanGoQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, RayCanGoQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_destPos3f);
	SwapEndianness(e, self.m_positionSpatializationRange);
	SwapEndianness(e, self.m_marginMode);
	SwapEndianness(e, self.m_dynamicOutputMode);
	SwapEndianness(e, self.m_computeCostMode);

	SwapEndianness(e, self.m_margin);
	SwapEndianness(e, self.m_queryOutput);
}

class RayCanGoQueryBlobBuilder : public BaseBlobBuilder<RayCanGoQueryBlob>
{
public:
	RayCanGoQueryBlobBuilder(BaseRayCanGoQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_startPos3f                 , m_query->GetStartPos());
			BLOB_SET(m_blob->m_destPos3f                  , m_query->GetDestPos());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			BLOB_SET(m_blob->m_marginMode                 , (KyUInt32)m_query->GetMarginMode());
			BLOB_SET(m_blob->m_dynamicOutputMode          , (KyUInt32)m_query->GetDynamicOutputMode());
			BLOB_SET(m_blob->m_computeCostMode            , (KyUInt32)m_query->GetComputeCostMode());

			BLOB_SET(m_blob->m_margin, m_query->m_database->GetRayCanGoMarginFloat(m_query->GetMarginMode()));

			switch (m_query->GetResult())
			{
			case RAYCANGO_NOT_INITIALIZED :
			case RAYCANGO_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, RayCanGoQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseRayCanGoQuery* m_query;
};
}

#endif // Kaim_RayCanGoQueryInputBlob_H
