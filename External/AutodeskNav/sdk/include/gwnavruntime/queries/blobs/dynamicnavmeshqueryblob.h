/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_DynamicNavMeshQueryBlob_H
#define Navigation_DynamicNavMeshQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/dynamicnavmeshquery.h"
#include "gwnavruntime/navmesh/navfloor.h"
#include "gwnavruntime/navmesh/blobs/navfloorblob.h"
#include "gwnavruntime/blob/blobref.h"
#include "gwnavruntime/database/database.h"

namespace Kaim
{

class DynamicNavMeshQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, DynamicNavMeshQueryBlob, 0)
public:
	DynamicNavMeshQueryBlob() {}

public:
	KyUInt32 m_queryInfoId;
	KyUInt32 m_databaseIdx;

	// BlobRef<NavFloorBlob> m_inputNavfloor;

	// SECONDARY INPUT: extracted in ExtractInputDataFromNavFloor()
	PixelPos m_cellOriginPixel; ///< The pixel position of the lower left corner of the navcell that contain the processed navfloor. 
	KyFloat32 m_integerPrecision;
	// RESULT
	KyUInt32 m_result; ///< Updated during processing to indicate the result of the query. 
	BlobArray<BlobRef<NavFloorBlob> > m_resultNavFloors; ///< Handler to hold the final blob after computation
};

inline void SwapEndianness(Endianness::Target e, DynamicNavMeshQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	//SwapEndianness(e, self.m_inputNavfloor);
	SwapEndianness(e, self.m_cellOriginPixel);
	SwapEndianness(e, self.m_integerPrecision);
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_resultNavFloors);
}

class DynamicNavMeshQueryBlobBuilder : public BaseBlobBuilder<DynamicNavMeshQueryBlob>
{
public:
	DynamicNavMeshQueryBlobBuilder(DynamicNavMeshQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);
		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_integerPrecision , m_query->m_database->GetDatabaseGenMetrics().m_integerPrecision);
		}
		BLOB_SET(m_blob->m_cellOriginPixel, m_query->GetCellOriginPixel());
		BLOB_SET(m_blob->m_result, m_query->GetResult());
		//BLOB_SET(m_blob->m_inputNavfloor, m_query->m_navfloorPtr.GetNavFloorBlob());


		BlobRef<NavFloorBlob>* floors = BLOB_ARRAY(m_blob->m_resultNavFloors, m_query->m_resultNavfloorHandlers.GetCount());
		for(KyUInt32 i = 0; i < m_query->m_resultNavfloorHandlers.GetCount(); ++i)
		{
			if (m_query->m_resultNavfloorHandlers[i] != KY_NULL)
				COPY_REFERENCED_BLOB_FROM_HANDLER(floors[i], *m_query->m_resultNavfloorHandlers[i]);
		}
	}

private:
	DynamicNavMeshQuery* m_query;
};
}

#endif // Navigation_DynamicNavMeshQueryBlob_H
