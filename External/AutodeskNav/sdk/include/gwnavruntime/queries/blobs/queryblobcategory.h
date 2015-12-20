/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_QueryBlobCategory_H
#define Navigation_QueryBlobCategory_H


#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/iblobcategorybuilder.h"


namespace Kaim
{


enum QueryCategory
{
	// DO NOT remove, comment or change order in this enum
	Blob_Navigation_Query_PathFinderQueryBlob                        =  0,
	Blob_Navigation_Query_AStarQueryBlob                             =  1,
	Blob_Navigation_Query_RayCanGoQueryBlob                          =  2,
	Blob_Navigation_Query_RayCastQueryBlob                           =  3,
	Blob_Navigation_Query_DiskCanGoQueryBlob                         =  4,
	Blob_Navigation_Query_DiskCastQueryBlob                          =  5,
	Blob_Navigation_Query_SegmentCanGoQueryBlob                      =  6,
	Blob_Navigation_Query_SegmentCastQueryBlob                       =  7,
	Blob_Navigation_Query_DiskCollisionQueryBlob                     =  8,
	Blob_Navigation_Query_DiskExpansionQueryBlob                     =  9,
	Blob_Navigation_Query_SpatializedPointCollectorInAABBQueryBlob   = 10,
	Blob_Navigation_Query_TriangleFromPosQueryBlob                   = 11,
	Blob_Navigation_Query_InsidePosFromOutsidePosQueryBlob           = 12,
	Blob_Navigation_Query_NearestBorderHalfEdgeFromPosQueryBlob      = 13,
	Blob_Navigation_Query_DynamicNavMeshQueryBlob                    = 14,
	Blob_Navigation_Query_BestGraphVertexPathFinderQueryBlob         = 15,
	Blob_Navigation_Query_CollisionRayCastQueryBlob                  = 16,

	Blob_Navigation_Query_Count
};


class QueryCategoryBuilder : public IBlobCategoryBuilder
{
public:
	QueryCategoryBuilder() : IBlobCategoryBuilder(Blob_Navigation, Blob_Navigation_Query, Blob_Navigation_Query_Count) {}
	virtual void Build(BlobCategory* category) const;
};


}


#endif

