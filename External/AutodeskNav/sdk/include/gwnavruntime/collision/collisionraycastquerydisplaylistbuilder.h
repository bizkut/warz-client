/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JAPA - secondary contact: NOBODY
#ifndef Navigation_CollisionRayCastQuery_DisplayListBuilder_H
#define Navigation_CollisionRayCastQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"

namespace Kaim
{

class CollisionRayCastQueryDisplayListBuilder : public IDisplayListBuilder
{
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const CollisionRayCastQueryBlob* queryBlob = (CollisionRayCastQueryBlob*)blob;
		
		CollisionRayCastQueryOutputBlob* queryOutputBlob = queryBlob->m_queryOutput.Ptr();
		if (queryOutputBlob != NULL)
		{
			float arrowHalfWidth = 0.05f;

			VisualShapeColor rayShapeColor;
			VisualShapeColor triangleShapeColor;

			switch(queryOutputBlob->GetResult())
			{
			case RayHit:
				{
					rayShapeColor.m_triangleColor = VisualColor::Red;
					displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_endPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
					displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_endPos3f, rayShapeColor.m_triangleColor);
				}
				break;
			case RayDidNotHit:
				{
					rayShapeColor.m_triangleColor = VisualColor::Lime;
					displayList->PushArrow(queryBlob->m_startPos3f, queryBlob->m_endPos3f, arrowHalfWidth, rayShapeColor, 3.0f, 0.1f);
					displayList->PushLine(queryBlob->m_startPos3f, queryBlob->m_endPos3f, rayShapeColor.m_triangleColor);
				}
				break;
			default:
				break;
			}
		}
	}
};

}

#endif // Navigation_CollisionRayCastQuery_DisplayListBuilder_H