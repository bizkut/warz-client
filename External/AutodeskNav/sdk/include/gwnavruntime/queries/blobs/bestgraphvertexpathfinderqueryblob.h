/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_BestGraphVertexPathFinderQueryBlob_H
#define Navigation_BestGraphVertexPathFinderQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/queries/blobs/navhalfedgeblob.h"
#include "gwnavruntime/queries/blobs/timeslicedqueryinfoblob.h"
#include "gwnavruntime/queries/utils/basebestgraphvertexpathfinderquery.h"
#include "gwnavruntime/path/pathblob.h"


namespace Kaim
{

class BestGraphVertexPathFinderQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	BestGraphVertexPathFinderQueryOutputBlob() : m_result(0) {}

	BestGraphVertexPathFinderQueryResult GetResult() const { return (BestGraphVertexPathFinderQueryResult)m_result; }

public:
	KyUInt32 m_result;
	TimeSlicedQueryInfoBlob m_timeSlicedQueryInfo;
	NavTriangleBlob m_startTriangle;
	PathBlob m_pathBlob;
};
inline void SwapEndianness(Endianness::Target e, BestGraphVertexPathFinderQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result             );
	SwapEndianness(e, self.m_timeSlicedQueryInfo);
	SwapEndianness(e, self.m_startTriangle      );
	SwapEndianness(e, self.m_pathBlob           );
}


class BestGraphVertexPathFinderQueryOutputBlobBuilder : public BaseBlobBuilder<BestGraphVertexPathFinderQueryOutputBlob>
{
public:
	BestGraphVertexPathFinderQueryOutputBlobBuilder(BaseBestGraphVertexPathFinderQuery* query)
		: m_query(query)
	{}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
		BLOB_BUILD(m_blob->m_timeSlicedQueryInfo, TimeSlicedQueryBlobBuilder(m_query));
		BLOB_BUILD(m_blob->m_startTriangle, NavTriangleBlobBuilder(m_query->GetStartTrianglePtr()));

		if (m_query->GetResult() == BESTGRAPHVERTEX_DONE_PATH_FOUND)
			BLOB_BUILD(m_blob->m_pathBlob, PathBlobBuilder(m_query->GetPath()));
	}

private:
	BaseBestGraphVertexPathFinderQuery* m_query;
};

class BestGraphVertexPathFinderQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, BestGraphVertexPathFinderQueryBlob, 0)

public:
	BestGraphVertexPathFinderQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	KyFloat32 m_fromOutsideNavMeshDistance;
	KyFloat32 m_propagationRadius;

	Vec3f m_startPos3f;
	PositionSpatializationRange m_positionSpatializationRange;
	BlobRef<BestGraphVertexPathFinderQueryOutputBlob> m_queryOutput;
};

inline void SwapEndianness(Endianness::Target e, BestGraphVertexPathFinderQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId                );
	SwapEndianness(e, self.m_databaseIdx                );
	SwapEndianness(e, self.m_fromOutsideNavMeshDistance );
	SwapEndianness(e, self.m_propagationRadius          );
	SwapEndianness(e, self.m_startPos3f                 );
	SwapEndianness(e, self.m_positionSpatializationRange);
	SwapEndianness(e, self.m_queryOutput                );
}



class BestGraphVertexPathFinderQueryBlobBuilder: public BaseBlobBuilder<BestGraphVertexPathFinderQueryBlob>
{
public:
	BestGraphVertexPathFinderQueryBlobBuilder(BaseBestGraphVertexPathFinderQuery* query)
		: m_query(query)
	{}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_startPos3f                 , m_query->GetStartPos());
			BLOB_SET(m_blob->m_positionSpatializationRange, m_query->GetPositionSpatializationRange());
			BLOB_SET(m_blob->m_fromOutsideNavMeshDistance , m_query->GetFromOutsideNavMeshDistance());
			BLOB_SET(m_blob->m_propagationRadius          , m_query->GetPropagationRadius());

			switch (m_query->GetResult())
			{
			case BESTGRAPHVERTEX_NOT_INITIALIZED :
			case BESTGRAPHVERTEX_NOT_PROCESSED :
				break;
			default:
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, BestGraphVertexPathFinderQueryOutputBlobBuilder(m_query));
				break;
			}
		}
	}

private:
	BaseBestGraphVertexPathFinderQuery* m_query;
};
}

#endif // Kaim_AstarQueryInputBlob_H
