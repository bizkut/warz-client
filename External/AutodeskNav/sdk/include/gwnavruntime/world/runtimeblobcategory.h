/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_RuntimeBlobRegistry_H
#define Navigation_RuntimeBlobRegistry_H


#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/iblobcategorybuilder.h"


namespace Kaim
{


enum RuntimeCategory
{
	// DO NOT remove, comment or change order in this enum
	Blob_Navigation_Runtime_BoxObstacleBlob                           =  0,
	Blob_Navigation_Runtime_BoxObstacleSpatializedCylindersBlob       =  1,
	Blob_Navigation_Runtime_BoxObstaclesCollectionBlob                =  2,
	Blob_Navigation_Runtime_CylinderObstacleBlob                      =  3,
	Blob_Navigation_Runtime_PathBlob                                  =  4,
	Blob_Navigation_Runtime_PositionOnPathBlob                        =  5,
	Blob_Navigation_Runtime_TrajectoryBlob                            =  6,
	Blob_Navigation_Runtime_BotConfigBlob                             =  7,
	Blob_Navigation_Runtime_SpatializedCylinderConfigBlob             =  8,
	Blob_Navigation_Runtime_SpatializedCylinderBlob                   =  9,
	Blob_Navigation_Runtime_PointOfInterestBlob                       = 10,
	Blob_Navigation_Runtime_DynamicFloorsAtCellPosBlob                = 11,
	Blob_Navigation_Runtime_PositionOnLivePathBlob                    = 12,
	Blob_Navigation_Runtime_LivePathBlob                              = 13,
	Blob_Navigation_Runtime_LodMaxBlob                                = 14,
	Blob_Navigation_Runtime_PathFollowerBlob                          = 15,
	Blob_Navigation_Runtime_SpatializationResultBlob                  = 16,
	Blob_Navigation_Runtime_BoxObstacleSpatializedCylinderConfigsBlob = 17,
	Blob_Navigation_Runtime_BoxObstacleSpatializationResultsBlob      = 18,
	Blob_Navigation_Runtime_SpatializedPointBlob                      = 19,
	Blob_Navigation_Runtime_DatabaseBindingBlob                       = 20,
	Blob_Navigation_Runtime_QueryQueueBlob                            = 21,
	Blob_Navigation_Runtime_AvoidanceComputerBlob                     = 22,
	Blob_Navigation_Runtime_ChannelBlob                               = 23,
	Blob_Navigation_Runtime_ChannelArrayBlob                          = 24,
	Blob_Navigation_Runtime_CircleArcSplineBlob                       = 25,
	Blob_Navigation_Runtime_ChannelTrajectoryBlob                     = 26,
	Blob_Navigation_Runtime_ColliderCollectorConfigBlob               = 27,
	Blob_Navigation_Runtime_ShortcutTrajectoryBlob                    = 28,
	Blob_Navigation_Runtime_PositionOnCircleArcSplineBlob             = 29,
	Blob_Navigation_Runtime_FrontDirectionBlob                        = 30,
	Blob_Navigation_Runtime_BubbleBlob                                = 31,
	Blob_Navigation_Runtime_BubbleFunnelExtremityConstraintBlob       = 32,
	Blob_Navigation_Runtime_CircleArcSplineComputerInputBlob          = 33,

	Blob_Navigation_Runtime_Count
};

class RuntimeCategoryBuilder : public IBlobCategoryBuilder
{
public:
	RuntimeCategoryBuilder() : IBlobCategoryBuilder(Blob_Navigation, Blob_Navigation_Runtime, Blob_Navigation_Runtime_Count) {}
	virtual void Build(BlobCategory* category) const;
};


}


#endif

