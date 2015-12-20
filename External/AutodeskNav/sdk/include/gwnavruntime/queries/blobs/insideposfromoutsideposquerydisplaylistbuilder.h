/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_InsidePosFromOutsidePosQuery_DisplayListBuilder_H
#define Navigation_InsidePosFromOutsidePosQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/insideposfromoutsideposqueryblob.h"
#include "gwnavruntime/queries/insideposfromoutsideposquery.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputdisplaylistbuilder.h"

namespace Kaim
{

class InsidePosFromOutsidePosQueryDisplayListBuilder : public IDisplayListBuilder
{
	void DrawSearchBox(const InsidePosFromOutsidePosQueryBlob* queryBlob, const VisualColor& color, ScopedDisplayList* displayList)
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
		const InsidePosFromOutsidePosQueryBlob* queryBlob = (InsidePosFromOutsidePosQueryBlob*) blob;
		KyUInt32 subdivisionCount = 24;
		
		InsidePosFromOutsidePosQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();
			KyFloat32 flagRadius = 0.3f;
			
			VisualShapeColor shapeColor;
			switch((InsidePosFromOutsidePosQueryResult)queryOutputBlob->m_result)
			{
				case INSIDEPOSFROMOUTSIDE_NOT_INITIALIZED: break;
				case INSIDEPOSFROMOUTSIDE_NOT_PROCESSED:
					{
						DrawSearchBox(queryBlob, VisualColor::Orange, displayList);

						shapeColor.m_lineColor = VisualColor::White;
						displayList->PushDisk(queryBlob->m_inputPos3f, queryBlob->m_distFromObstacle, subdivisionCount, shapeColor);
					}
					break;
				case INSIDEPOSFROMOUTSIDE_DONE_POS_NOT_FOUND:
					{
						DrawSearchBox(queryBlob, VisualColor::Red, displayList);
						
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushDisk(queryBlob->m_inputPos3f, queryBlob->m_distFromObstacle, subdivisionCount, shapeColor);
					}
					break;
				case INSIDEPOSFROMOUTSIDE_DONE_POS_FOUND:
					{
						DrawSearchBox(queryBlob, VisualColor::LightGreen, displayList);
						displayList->PushText(queryOutputBlob->m_insidePos3f + offsetVector, VisualColor::Lime, "Inside pos");

						shapeColor.m_lineColor = VisualColor::Lime;
						displayList->PushPoint(queryOutputBlob->m_insidePos3f, flagRadius, shapeColor);
						
						shapeColor.m_lineColor = VisualColor::LightGreen;
						displayList->PushDisk(queryBlob->m_inputPos3f, queryBlob->m_distFromObstacle, subdivisionCount, shapeColor);


						shapeColor.SetOnlyTriangleColor(VisualColor::Lime);
						displayList->PushArrow(queryBlob->m_inputPos3f, queryOutputBlob->m_insidePos3f, 0.05f, shapeColor);
						
					}
					break;
			}
		}
	}
};

}

#endif