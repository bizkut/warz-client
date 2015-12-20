/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_NearestBorderHalfEdgeFromPosQuery_DisplayListBuilder_H
#define Navigation_NearestBorderHalfEdgeFromPosQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/nearestborderhalfedgefromposqueryblob.h"
#include "gwnavruntime/queries/nearestborderhalfedgefromposquery.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputdisplaylistbuilder.h"

namespace Kaim
{

class NearestBorderHalfEdgeFromPosQueryDisplayListBuilder : public IDisplayListBuilder
{
	void DrawSearchBox(const NearestBorderHalfEdgeFromPosQueryBlob* queryBlob, const VisualColor& color, ScopedDisplayList* displayList)
	{
		Vec3f boxMax(queryBlob->m_inputPos3f);
		Vec3f boxMin(queryBlob->m_inputPos3f);
		boxMin.x -= queryBlob->m_horizontalTolerance;
		boxMin.y -= queryBlob->m_horizontalTolerance;
		boxMin.z -= queryBlob->m_positionSpatializationRange.m_rangeBelowPosition;

		boxMax.x += queryBlob->m_horizontalTolerance;
		boxMax.y += queryBlob->m_horizontalTolerance;
		boxMax.z += queryBlob->m_positionSpatializationRange.m_rangeAbovePosition;

		VisualShapeColor shapeColor;
		shapeColor.m_lineColor = color;
		displayList->PushBox(Box3f(boxMin, boxMax), shapeColor);
	}

	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const NearestBorderHalfEdgeFromPosQueryBlob* queryBlob = (NearestBorderHalfEdgeFromPosQueryBlob*) blob;
		
		NearestBorderHalfEdgeFromPosQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();
			KyFloat32 flagRadius = 0.3f;

			VisualShapeColor shapeColor;
			switch((NearestBorderHalfEdgeFromPosQueryResult)queryOutputBlob->m_result)
			{
				case NEARESTHALFEDGE_NOT_INITIALIZED: break;
				case NEARESTHALFEDGE_NOT_PROCESSED:
					{
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushPoint(queryBlob->m_inputPos3f, flagRadius, shapeColor);
						DrawSearchBox(queryBlob, VisualColor::Orange, displayList);
					}
					break;
				case NEARESTHALFEDGE_HALFEDGE_NOT_FOUND:
					{
						DrawSearchBox(queryBlob, VisualColor::Red, displayList);
					}
					break;
				case NEARESTHALFEDGE_DONE_LACK_OF_WORKING_MEMORY:
					{
						displayList->PushText(queryBlob->m_inputPos3f + offsetVector, VisualColor::Red, "Lack of memory");
						DrawSearchBox(queryBlob, VisualColor::Red, displayList);
					}
					break;
				case NEARESTHALFEDGE_HALFEDGE_FOUND:
					{
						DrawSearchBox(queryBlob, VisualColor::LightGreen, displayList);
						shapeColor.m_lineColor = VisualColor::Lime;
						displayList->PushPoint(queryOutputBlob->m_nearestPosOnHalfEdge, flagRadius, shapeColor);
						

						shapeColor.SetOnlyTriangleColor(VisualColor::Lime);
						displayList->PushArrow(queryBlob->m_inputPos3f, queryOutputBlob->m_nearestPosOnHalfEdge, 0.05f, shapeColor);
						displayList->PushText(queryOutputBlob->m_nearestPosOnHalfEdge + offsetVector, shapeColor.m_triangleColor, "Nearest half edge");
						displayList->PushLine(queryOutputBlob->m_nearestHalfEdgeOnBorder.m_startPos3f, queryOutputBlob->m_nearestHalfEdgeOnBorder.m_endPos3f, 
						                      shapeColor.m_triangleColor);


						shapeColor.m_triangleColor = VisualColor::Orange;
						Vec3f prevHalfEdgeMiddle = (queryOutputBlob->m_prevHalfEdgeOnBorder.m_startPos3f + queryOutputBlob->m_prevHalfEdgeOnBorder.m_endPos3f) * 0.5f;
						displayList->PushLine(queryOutputBlob->m_prevHalfEdgeOnBorder.m_startPos3f, queryOutputBlob->m_prevHalfEdgeOnBorder.m_endPos3f, 
						                      shapeColor.m_triangleColor);
						displayList->PushArrow(queryBlob->m_inputPos3f, prevHalfEdgeMiddle, 0.01f, shapeColor);
						displayList->PushText(prevHalfEdgeMiddle + offsetVector, shapeColor.m_triangleColor, "Prev");

						Vec3f nextHalfEdgeMiddle = (queryOutputBlob->m_nextHalfEdgeOnBorder.m_startPos3f + queryOutputBlob->m_nextHalfEdgeOnBorder.m_endPos3f) * 0.5f;
						displayList->PushLine(queryOutputBlob->m_nextHalfEdgeOnBorder.m_startPos3f, queryOutputBlob->m_nextHalfEdgeOnBorder.m_endPos3f,
						                      shapeColor.m_triangleColor);
						displayList->PushArrow(queryBlob->m_inputPos3f, nextHalfEdgeMiddle, 0.01f, shapeColor);
						displayList->PushText(nextHalfEdgeMiddle + offsetVector, shapeColor.m_triangleColor, "Next");
					}
					break;
			}
		}
	}
};

}

#endif