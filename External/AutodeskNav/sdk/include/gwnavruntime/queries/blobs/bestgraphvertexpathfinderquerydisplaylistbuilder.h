/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_BestGraphVertexPathFinderQuery_DisplayListBuilder_H
#define Navigation_BestGraphVertexPathFinderQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/bestgraphvertexpathfinderqueryblob.h"
#include "gwnavruntime/queries/blobs/astarquerydisplaylistbuilder.h"
#include "gwnavruntime/path/pathdisplay.h"
#include "gwnavruntime/path/pathblob.h"
#include "gwnavruntime/kernel/SF_Math.h"

namespace Kaim
{

class BestGraphVertexPathFinderQueryDisplayListBuilder : public IDisplayListBuilder
{
public:
	static void BuildPropagationCylinder(ScopedDisplayList* displayList, const Vec3f& center, KyFloat32 radius)
	{
		VisualShapeColor shapeColor;
		shapeColor.m_lineColor = VisualColor::Black;
		shapeColor.m_triangleColor = VisualColor::DimGray;

		const KyFloat32 heigth = 200.f;
		const Vec3f P = center - heigth * 0.5f * Vec3f::UnitZ();

		const KyUInt32 subdivisionCount = 14;
		const KyFloat32 angleStep = ((float)(KY_MATH_PI) * 2.0f) / subdivisionCount;
		const KyFloat32 cosAngleStep = cosf(angleStep);
		const KyFloat32 sinAngleStep = sinf(angleStep);
		const KyFloat32 cosAngleHalfStep = cosf(angleStep*0.5f);
		const KyFloat32 sinAngleHalfStep = sinf(angleStep*0.5f);
		const Vec3f heightVector(0.0f, 0.0f, heigth);

		Vec2f curStepDir(1.0f, 0.0f);
		Vec3f v0 = P + radius * curStepDir;
		for(KyUInt32 i = 0; i < subdivisionCount; i++)
		{

			const KyFloat32 formerDirX = curStepDir.x;

			Vec2f planeDir;
			planeDir.x = cosAngleHalfStep * formerDirX - sinAngleHalfStep * curStepDir.y;
			planeDir.y = sinAngleHalfStep * formerDirX + cosAngleHalfStep * curStepDir.y;

			curStepDir.x = cosAngleStep * formerDirX - sinAngleStep * curStepDir.y;
			curStepDir.y = sinAngleStep * formerDirX + cosAngleStep * curStepDir.y;


			const Vec3f v1 = P + radius * curStepDir;

			displayList->PushQuad(v0, v1, v1 + heightVector, v0 + heightVector, shapeColor);

			if (i%2 == 0)
			{
				AStarQueryDisplayListBuilder::BuildVerticalForbiddenWallLogo(displayList, 
					v0, v1, v1 + heightVector, v0 + heightVector, planeDir.PerpCCW());
			}

			v0 = v1;
		}
	}

	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/)
	{
		const BestGraphVertexPathFinderQueryBlob* bestGraphVertexPathFinderQueryBlob = (BestGraphVertexPathFinderQueryBlob*) blob;
		
		BestGraphVertexPathFinderQueryOutputBlob* bestGraphVertexPathFinderQueryOutputBlob = bestGraphVertexPathFinderQueryBlob->m_queryOutput.Ptr();
		if (bestGraphVertexPathFinderQueryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();
			VisualColor textColor = VisualColor::Red;
			switch((BestGraphVertexPathFinderQueryResult)bestGraphVertexPathFinderQueryOutputBlob->m_result)
			{
				case BESTGRAPHVERTEX_NOT_INITIALIZED:
					break;

				case BESTGRAPHVERTEX_NOT_PROCESSED:
				case BESTGRAPHVERTEX_PROCESSING_TRAVERSAL:
				case BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE:
				case BESTGRAPHVERTEX_PROCESSING_REFINING_INIT:
				case BESTGRAPHVERTEX_PROCESSING_REFINING:
				case BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING_INIT:
				case BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING:
				case BESTGRAPHVERTEX_PROCESSING_PATHBUILDING:
				case BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT:
				case BESTGRAPHVERTEX_PROCESSING_CHANNEL_COMPUTE:
					break;

				case BESTGRAPHVERTEX_DONE_START_OUTSIDE:
					{
						displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "Start outside!");
						break;
					}
				case BESTGRAPHVERTEX_DONE_START_NAVTAG_FORBIDDEN:
					{
						displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "Start NavTag forbidden!");
						break;
					}
				case BESTGRAPHVERTEX_DONE_PATH_NOT_FOUND:
					{
						BuildPropagationCylinder(displayList, bestGraphVertexPathFinderQueryBlob->m_startPos3f,
							bestGraphVertexPathFinderQueryBlob->m_propagationRadius);
						displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "Path not found !");
						break;
					}
				case BESTGRAPHVERTEX_DONE_NAVDATA_CHANGED:
					{
						displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "NavData changed!");
						break;
					}
				case BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY:
					{
						displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "Lack of working memory!");
						break;
					}
				case BESTGRAPHVERTEX_DONE_COMPUTATION_ERROR :
					{
						displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "Computation Error");
						break;
					}
				case BESTGRAPHVERTEX_DONE_CHANNELCOMPUTATION_ERROR :
					{
						displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "Channel Computation Error");
						break;
					}
				case BESTGRAPHVERTEX_DONE_PATH_FOUND:
					{
						PathDisplayListBuilder pathDisplayListBuilder;
						pathDisplayListBuilder.DisplayPath(displayList, &bestGraphVertexPathFinderQueryOutputBlob->m_pathBlob);
					}
					break;

				case BESTGRAPHVERTEX_DONE_COMPUTATION_CANCELED:
					displayList->PushText(bestGraphVertexPathFinderQueryBlob->m_startPos3f + offsetVector, textColor, "Query Canceled");
					break;
			}
		}
	}
};

}

#endif // Navigation_BestGraphVertexPathFinderQuery_DisplayListBuilder_H
