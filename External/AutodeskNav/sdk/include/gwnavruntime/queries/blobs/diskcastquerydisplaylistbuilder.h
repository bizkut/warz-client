/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_DiskCastQuery_DisplayListBuilder_H
#define Navigation_DiskCastQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/diskcastqueryblob.h"
#include "gwnavruntime/queries/diskcastquery.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputdisplaylistbuilder.h"

namespace Kaim
{

class DiskCastQueryDisplayListBuilder : public IDisplayListBuilder
{
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const DiskCastQueryBlob* queryBlob = (DiskCastQueryBlob*) blob;
		KyFloat32 triangleZOffset = 0.1f;
		
		DiskCastQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();
			KyUInt32 subdivisionCount = 24;

			VisualShapeColor shapeColor;
			switch((DiskCastQueryResult)queryOutputBlob->m_result)
			{
				case DISKCAST_NOT_INITIALIZED: break;
				case DISKCAST_NOT_PROCESSED:
					{
						displayList->PushText(queryBlob->m_startPos3f + offsetVector, VisualColor::Red, "Not processed...");
						
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_startPos3f + queryBlob->m_normalizedDir2d * queryBlob->m_maxDist, 
						                        queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_START_OUTSIDE:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_startPos3f + offsetVector, shapeColor.m_lineColor, "Start outside!");
						displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_startPos3f + queryBlob->m_normalizedDir2d * queryBlob->m_maxDist, 
						                        queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_START_NAVTAG_FORBIDDEN:
					{
						shapeColor.m_lineColor = VisualColor::Red;

						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						displayList->PushText(queryBlob->m_startPos3f + offsetVector, shapeColor.m_lineColor, "Start NavTag forbidden!");
						displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_startPos3f + queryBlob->m_normalizedDir2d * queryBlob->m_maxDist, 
						                        queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_CANNOT_MOVE:
					{
						shapeColor.m_lineColor = VisualColor::Lime;

						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);
						
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushLine(queryOutputBlob->m_collisionPos3f, queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor);
						displayList->PushText(queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor, "Collision point");
						displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_startPos3f + queryBlob->m_normalizedDir2d * queryBlob->m_maxDist, 
						                        queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR:
					{
						shapeColor.m_lineColor = VisualColor::Lime;

						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushLine(queryOutputBlob->m_collisionPos3f, queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor);
						displayList->PushText(queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor, "Collision point (arrival error)");
						displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_startPos3f + queryBlob->m_normalizedDir2d * queryBlob->m_maxDist, 
						                        queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_LACK_OF_WORKING_MEMORY:
					{
						shapeColor.m_lineColor = VisualColor::Lime;
						
						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_startPos3f + offsetVector, shapeColor.m_lineColor, "Lack of working memory!");
						displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_startPos3f + queryBlob->m_normalizedDir2d * queryBlob->m_maxDist, 
						                        queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_UNKNOWN_ERROR:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_startPos3f + offsetVector, shapeColor.m_lineColor, "Unknown error!");
						displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_startPos3f + queryBlob->m_normalizedDir2d * queryBlob->m_maxDist, 
						                        queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED:
					{
						shapeColor.m_lineColor = VisualColor::Lime;

						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						triangle = queryOutputBlob->m_arrivalTriangle.m_triangle;
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						displayList->PushStadium(queryBlob->m_startPos3f, queryOutputBlob->m_arrivalPos3f, queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
				case DISKCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION:
					{
						shapeColor.m_lineColor = VisualColor::Lime;

						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						triangle = queryOutputBlob->m_arrivalTriangle.m_triangle;
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);
						
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushLine(queryOutputBlob->m_collisionPos3f, queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor);
						displayList->PushText(queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor, "Collision point");
						displayList->PushStadium(queryBlob->m_startPos3f, queryOutputBlob->m_arrivalPos3f, queryBlob->m_radius, subdivisionCount, shapeColor);
					}
					break;
			}

			QueryDynamicOutputBlob* queryDynamicOutputBlob = queryOutputBlob->m_queryDynamicOutputBlobRef.Ptr();
			if (queryDynamicOutputBlob != NULL)
			{
				QueryDynamicOutputDisplayListBuilder dynamicOutputDLBuilder;
				dynamicOutputDLBuilder.DoBuild(displayList, (char*)queryDynamicOutputBlob, 0);
			}
		}
	}
};

}

#endif