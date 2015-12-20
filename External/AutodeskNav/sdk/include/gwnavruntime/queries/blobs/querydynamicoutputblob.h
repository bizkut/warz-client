/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


/*
* Copyright 2011 Autodesk, Inc.  All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement provided at the time of installation or download, 
* or which otherwise accompanies this software in either electronic or hard copy form.
*/


#ifndef Navigation_QueryDynamicOutputBlob_H
#define Navigation_QueryDynamicOutputBlob_H

#include "gwnavruntime/queries/querydynamicoutput.h"
#include "gwnavruntime/queries/blobs/navtriangleblob.h"
#include "gwnavruntime/queries/blobs/navtagsubsegmentblob.h"
#include "gwnavruntime/queries/blobs/navfloorboxblob.h"
#include "gwnavruntime/spatialization/spatializedcylinderblob.h"

namespace Kaim
{

class QueryDynamicOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	QueryDynamicOutputBlob() {}

public:
	BlobArray<NavFloorBoxBlob> m_navFloorBoxes;
	BlobArray<NavTriangleBlob> m_navTriangles;
	BlobArray<NavTagSubSegmentBlob> m_navTagSubSegments;
	BlobArray<SpatializedPointBlob> m_spatializedPoints;
};

inline void SwapEndianness(Endianness::Target e, QueryDynamicOutputBlob& self)
{
	SwapEndianness(e, self.m_navFloorBoxes);
	SwapEndianness(e, self.m_navTriangles);
	SwapEndianness(e, self.m_navTagSubSegments);
	SwapEndianness(e, self.m_spatializedPoints);
}


class QueryDynamicOutputBlobBuilder : public BaseBlobBuilder<QueryDynamicOutputBlob>
{
public:
	QueryDynamicOutputBlobBuilder(const QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
	virtual void DoBuild()
	{
		NavFloorBoxBlob* navFloorsBoxes       = BLOB_ARRAY(m_blob->m_navFloorBoxes , m_queryDynamicOutput->GetNavFloorPtrCount());
		NavTriangleBlob* navTrianglesBlobs    = BLOB_ARRAY(m_blob->m_navTriangles  , m_queryDynamicOutput->GetNavTrianglePtrCount());
		NavTagSubSegmentBlob* navTagSubSegmentBlobs = BLOB_ARRAY(m_blob->m_navTagSubSegments, m_queryDynamicOutput->GetNavTagSubSegmentCount());
		SpatializedPointBlob* spatializedPointBlobs = BLOB_ARRAY(m_blob->m_spatializedPoints , m_queryDynamicOutput->GetSpatializedPointCount());

		for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetNavFloorPtrCount(); ++i)
		{
			BLOB_BUILD(navFloorsBoxes[i], NavFloorBoxBlobBuilder(m_queryDynamicOutput->GetNavFloorPtr(i)));
		}
		for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetNavTrianglePtrCount(); ++i)
		{
			BLOB_BUILD(navTrianglesBlobs[i], NavTriangleBlobBuilder(m_queryDynamicOutput->GetNavTrianglePtr(i)));
		}
		for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetNavTagSubSegmentCount(); ++i)
		{
			BLOB_BUILD(navTagSubSegmentBlobs[i], NavTagSubSegmentBlobBuilder(m_queryDynamicOutput->GetNavTagSubSegment(i)));
		}
		for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetSpatializedPointCount(); ++i)
		{
			BLOB_BUILD(spatializedPointBlobs[i], SpatializedPointBlobBuilder(m_queryDynamicOutput->GetSpatializedPoint(i), VisualDebugLOD_Default));
		}
	}

private:
	const QueryDynamicOutput* m_queryDynamicOutput;
};

}

#endif // Navigation_QueryDynamicOutputBlob_H
