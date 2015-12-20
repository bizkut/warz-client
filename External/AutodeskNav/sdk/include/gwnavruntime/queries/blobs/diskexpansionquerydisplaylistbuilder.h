/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_DiskExpansionQuery_DisplayListBuilder_H
#define Navigation_DiskExpansionQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/diskexpansionqueryblob.h"
#include "gwnavruntime/queries/diskexpansionquery.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputdisplaylistbuilder.h"

namespace Kaim
{

class DiskExpansionQueryDisplayListBuilder : public IDisplayListBuilder
{
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const DiskExpansionQueryBlob* queryBlob = (DiskExpansionQueryBlob*) blob;
		KyUInt32 subdivisionCount = 24;
		KyFloat32 triangleZOffset = 0.1f;
		
		DiskExpansionQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();

			VisualShapeColor shapeColor;
			switch((DiskExpansionQueryResult)queryOutputBlob->m_result)
			{
				case DISKEXPANSION_NOT_INITIALIZED: break;
				case DISKEXPANSION_NOT_PROCESSED:
					{
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushDisk(queryBlob->m_centerPos3f, queryBlob->m_radiusMax, subdivisionCount, shapeColor);
					}
					break;
				case DISKEXPANSION_DONE_CENTER_OUTSIDE:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_centerPos3f + offsetVector, shapeColor.m_lineColor, "Center outside!");
						displayList->PushDisk(queryBlob->m_centerPos3f, queryBlob->m_radiusMax, subdivisionCount, shapeColor);
					}
					break;
				case DISKEXPANSION_DONE_CENTER_NAVTAG_FORBIDDEN:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						
						Triangle3f triangle = queryOutputBlob->m_centerTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						displayList->PushDisk(queryBlob->m_centerPos3f, queryBlob->m_radiusMax, subdivisionCount, shapeColor);
						displayList->PushText(queryBlob->m_centerPos3f + offsetVector, shapeColor.m_lineColor, "Center NavTag forbidden!");
					}
					break;
				case DISKEXPANSION_DONE_CANNOT_EXPAND:
					{
						shapeColor.m_lineColor = VisualColor::Lime;
						Triangle3f triangle = queryOutputBlob->m_centerTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);

						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushDisk(queryBlob->m_centerPos3f, queryBlob->m_radiusMax, subdivisionCount, shapeColor);
						displayList->PushText(queryBlob->m_centerPos3f + offsetVector, shapeColor.m_lineColor, "Cannot expand!");
						
					}
					break;
				case DISKEXPANSION_DONE_LACK_OF_WORKING_MEMORY:
					{
						shapeColor.m_lineColor = VisualColor::Lime;
						Triangle3f triangle = queryOutputBlob->m_centerTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);
						
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushDisk(queryBlob->m_centerPos3f, queryBlob->m_radiusMax, subdivisionCount, shapeColor);
						displayList->PushText(queryBlob->m_centerPos3f + offsetVector, shapeColor.m_lineColor, "Lack of working memory!");
						
					}
					break;
				case DISKEXPANSION_DONE_UNKNOWN_ERROR:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_centerPos3f + offsetVector, shapeColor.m_lineColor, "Unknown error!");
						displayList->PushDisk(queryBlob->m_centerPos3f, queryBlob->m_radiusMax, subdivisionCount, shapeColor);
					}
					break;
				case DISKEXPANSION_DONE_DISK_FIT:
					{
						shapeColor.m_lineColor = VisualColor::Lime;
						Triangle3f triangle = queryOutputBlob->m_centerTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);
						displayList->PushDisk(queryBlob->m_centerPos3f, queryOutputBlob->m_resultRadius, subdivisionCount, shapeColor);
					}
					break;
				case DISKEXPANSION_DONE_COLLISION_FOUND:
					{
						shapeColor.m_lineColor = VisualColor::Lime;
						Triangle3f triangle = queryOutputBlob->m_centerTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);
						
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushDisk(queryBlob->m_centerPos3f, queryOutputBlob->m_resultRadius, subdivisionCount, shapeColor);
						displayList->PushLine(queryOutputBlob->m_collisionPos3f, queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor);
						displayList->PushText(queryOutputBlob->m_collisionPos3f + offsetVector, shapeColor.m_lineColor, "Collision point");
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