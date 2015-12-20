/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_SpatializedPointsInAabbFromPosQuery_DisplayListBuilder_H
#define Navigation_SpatializedPointsInAabbFromPosQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/spatializedpointsinaabbfromposqueryblob.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputdisplaylistbuilder.h"

namespace Kaim
{

class SpatializedPointCollectorInAABBQueryDisplayListBuilder : public IDisplayListBuilder
{
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const SpatializedPointCollectorInAABBQueryBlob* queryBlob = (SpatializedPointCollectorInAABBQueryBlob*) blob;
		
		SpatializedPointCollectorInAABBQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();
			Box3f aabb(queryBlob->m_startPos3f, queryBlob->m_startPos3f);
			aabb.m_min -= queryBlob->m_extentBox.m_min;
			aabb.m_max += queryBlob->m_extentBox.m_max;

			VisualShapeColor shapeColor;
			switch((SpatializedPointCollectorInAABBQueryResult)queryOutputBlob->m_result)
			{
				case SPATIALIZEDPOINTCOLLECTOR_NOT_INITIALIZED: break;
				case SPATIALIZEDPOINTCOLLECTOR_NOT_PROCESSED:
					{
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushBox(aabb, shapeColor);
					}
					break;
				case SPATIALIZEDPOINTCOLLECTOR_DONE_START_OUTSIDE:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushBox(aabb, shapeColor);
					}
					break;
				case SPATIALIZEDPOINTCOLLECTOR_DONE_LACK_OF_WORKING_MEMORY:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushBox(aabb, shapeColor);
						
						shapeColor.m_lineColor = VisualColor::Lime;
						displayList->PushTriangle(queryOutputBlob->m_startTriangle.m_triangle, shapeColor);
						displayList->PushText(queryBlob->m_startPos3f + offsetVector, shapeColor.m_lineColor, "Lack of working memory!");
					}
					break;
				case SPATIALIZEDPOINTCOLLECTOR_DONE:
					{
						shapeColor.m_lineColor = VisualColor::LightGreen;
						displayList->PushBox(aabb, shapeColor);
						
						shapeColor.m_lineColor = VisualColor::Lime;
						displayList->PushTriangle(queryOutputBlob->m_startTriangle.m_triangle, shapeColor);
						
						shapeColor.SetOnlyTriangleColor(VisualColor::Lime);
						SpatializedPointBlob* points = queryOutputBlob->m_queryDynamicOutputBlob.m_spatializedPoints.GetValues();
						for (KyUInt32 i = 0; i < queryOutputBlob->m_queryDynamicOutputBlob.m_spatializedPoints.GetCount(); ++i)
						{
							displayList->PushPoint(points[i].m_position, shapeColor);
							displayList->PushText(points[i].m_position + offsetVector, shapeColor.m_triangleColor, SpatializedPoint::GetObjectTypeDescrition((SpatializedPointObjectType)points[i].m_type));
						}
					}
					break;
			}
		}
	}
};

}

#endif