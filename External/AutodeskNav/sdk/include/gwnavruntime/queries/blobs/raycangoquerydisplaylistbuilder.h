/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_RayCanGoQuery_DisplayListBuilder_H
#define Navigation_RayCanGoQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/raycangoqueryblob.h"
#include "gwnavruntime/queries/raycangoquery.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputdisplaylistbuilder.h"

namespace Kaim
{

class RayCanGoQueryDisplayListBuilder : public IDisplayListBuilder
{
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const RayCanGoQueryBlob* queryBlob = (RayCanGoQueryBlob*) blob;
		KyFloat32 triangleZOffset = 0.1f;
		
		RayCanGoQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();
			float arrowHalfWidth = (queryBlob->m_marginMode != NoMargin) ? 0.5f * queryBlob->m_margin : 0.05f;
			VisualShapeColor rayShapeColor;
			VisualShapeColor triangleShapeColor;
			switch((RayCanGoQueryResult)queryOutputBlob->m_result)
			{
				case RAYCANGO_NOT_INITIALIZED: break;
				case RAYCANGO_NOT_PROCESSED:
					{
						rayShapeColor.m_triangleColor = VisualColor::Orange;
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);
					}
					break;
				case RAYCANGO_DONE_START_OUTSIDE:
					{
						rayShapeColor.m_triangleColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_startPos3f + offsetVector, rayShapeColor.m_triangleColor, "Start outside!");
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);
					}
					break;
				case RAYCANGO_DONE_START_NAVTAG_FORBIDDEN:
					{
						rayShapeColor.m_triangleColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_startPos3f + offsetVector, rayShapeColor.m_triangleColor, "Start NavTag forbidden!");
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);
						
						triangleShapeColor.SetOnlyLineColor(rayShapeColor.m_triangleColor);
						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, triangleShapeColor);
					}
					break;
				case RAYCANGO_DONE_COLLISION_DETECTED:
					{
						rayShapeColor.m_triangleColor = VisualColor::Red;
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);

						triangleShapeColor.SetOnlyLineColor(VisualColor::Lime);
						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, triangleShapeColor);
					}
					break;
				case RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN:
					{
						rayShapeColor.m_triangleColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_destPos3f + offsetVector, rayShapeColor.m_triangleColor, "Border at less than margin!");
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);

						triangleShapeColor.SetOnlyLineColor(VisualColor::Lime);
						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, triangleShapeColor);
					}
					break;
				case RAYCANGO_DONE_ARRIVAL_IN_WRONG_FLOOR:
					{
						rayShapeColor.m_triangleColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_destPos3f + offsetVector, rayShapeColor.m_triangleColor, "Arrival in wrong floor!");
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);

						triangleShapeColor.SetOnlyLineColor(VisualColor::Lime);
						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, triangleShapeColor);
					}
					break;
				case RAYCANGO_DONE_LACK_OF_WORKING_MEMORY:
					{
						rayShapeColor.m_triangleColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_destPos3f + offsetVector, rayShapeColor.m_triangleColor, "Lack of working memory!");
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);

						triangleShapeColor.SetOnlyLineColor(VisualColor::Lime);
						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, triangleShapeColor);
					}
					break;
				case RAYCANGO_DONE_UNKNOWN_ERROR:
					{
						rayShapeColor.m_triangleColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_destPos3f + offsetVector, rayShapeColor.m_triangleColor, "Unknown error!");
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);
					}
					break;
				case RAYCANGO_DONE_SUCCESS:
					{
						rayShapeColor.m_triangleColor = VisualColor::Lime;
						displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_destPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
						displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_destPos3f, rayShapeColor.m_triangleColor);
						
						triangleShapeColor.SetOnlyLineColor(VisualColor::Lime);
						
						Triangle3f triangle = queryOutputBlob->m_startTriangle.m_triangle;
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, triangleShapeColor);
						triangle = queryOutputBlob->m_destTriangle.m_triangle;
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, triangleShapeColor);
						
					}
					break;
			}

			if (queryBlob->m_marginMode != NoMargin)
			{
				rayShapeColor.SetOnlyLineColor(rayShapeColor.m_triangleColor);
				displayList->PushStadium(queryBlob->m_startPos3f, queryBlob->m_destPos3f, queryBlob->m_margin, 24, rayShapeColor);
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