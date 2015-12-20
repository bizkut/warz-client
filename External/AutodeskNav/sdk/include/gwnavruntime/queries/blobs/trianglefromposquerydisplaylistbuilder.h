/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_TriangleFromPosQuery_DisplayListBuilder_H
#define Navigation_TriangleFromPosQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/trianglefromposqueryblob.h"

namespace Kaim
{

class TriangleFromPosQueryDisplayListBuilder : public IDisplayListBuilder
{
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const TriangleFromPosQueryBlob* queryBlob = (TriangleFromPosQueryBlob*) blob;
		KyFloat32 triangleZOffset = 0.1f;
		
		TriangleFromPosQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();

			VisualShapeColor shapeColor;
			switch((TriangleFromPosQueryResult)queryOutputBlob->m_result)
			{
				case TRIANGLEFROMPOS_NOT_INITIALIZED: break;
				case TRIANGLEFROMPOS_NOT_PROCESSED:
					{
						shapeColor.m_lineColor = VisualColor::Orange;
						displayList->PushPoint(queryBlob->m_inputPos3f, shapeColor);
					}
					break;
				case TRIANGLEFROMPOS_DONE_NO_TRIANGLE_FOUND:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushPoint(queryBlob->m_inputPos3f, shapeColor);
						displayList->PushText(queryBlob->m_inputPos3f + offsetVector, shapeColor.m_lineColor, "No triangle found!");
					}
					break;
				case TRIANGLEFROMPOS_DONE_LACK_OF_WORKING_MEMORY:
					{
						shapeColor.m_lineColor = VisualColor::Red;
						displayList->PushText(queryBlob->m_inputPos3f + offsetVector, shapeColor.m_lineColor, "Lack of working memory!");
						displayList->PushPoint(queryBlob->m_inputPos3f, shapeColor);
					}
					break;
				case TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND:
					{
						shapeColor.m_lineColor = VisualColor::Lime;
						displayList->PushPoint(queryBlob->m_inputPos3f, shapeColor);
						Triangle3f triangle = queryOutputBlob->m_resultTrianglePtr.m_triangle;
						triangle.A.z += triangleZOffset;
						triangle.B.z += triangleZOffset;
						triangle.C.z += triangleZOffset;
						displayList->PushTriangle(triangle, shapeColor);
					}
					break;
			}

			shapeColor.m_lineColor = VisualColor::Cyan;
			const KyFloat32 spatializationRangeAbovePosition = queryBlob->m_positionSpatializationRange.m_rangeAbovePosition;
			const KyFloat32 spatializationRangeBelowPosition = queryBlob->m_positionSpatializationRange.m_rangeBelowPosition;
			const Vec3f rangeTop    = queryBlob->m_inputPos3f + Vec3f(0.0f, 0.0f, spatializationRangeAbovePosition);
			const Vec3f rangeBottom = queryBlob->m_inputPos3f - Vec3f(0.0f, 0.0f, spatializationRangeBelowPosition);
			displayList->PushLine(rangeBottom, rangeTop, shapeColor.m_lineColor);
		}
	}
};

}

#endif