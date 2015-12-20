/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_AStarQuery_DisplayListBuilder_H
#define Navigation_AStarQuery_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/idisplaylistbuilder.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/queries/blobs/astarqueryblob.h"
#include "gwnavruntime/path/pathdisplay.h"
#include "gwnavruntime/path/pathblob.h"
#include "gwnavruntime/queries/utils/baseastarquery.h"

namespace Kaim
{

class AStarQueryDisplayListBuilder : public IDisplayListBuilder
{
public:
	static void BuildVerticalForbiddenWallLogo(ScopedDisplayList* displayList,
		const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& /*D*/, const Vec2f& AtoBDir2d)
	{
		const KyFloat32 segmentRadius = 0.1f;
		const Vec3f center = (A+C) * 0.5;
		KyFloat32 altDiff = (C.z-A.z);
		const KyFloat32 length = DotProduct(B.Get2d() - A.Get2d(), AtoBDir2d);

		KyFloat32 radius = 0.6f * Min(altDiff, length) * 0.5f ;
		const Vec3f frontPlane(AtoBDir2d.PerpCCW());

		{
			VisualShapeColor whiteshapeColor;
			whiteshapeColor.m_triangleColor = VisualColor::White;
			const Vec3f frontOffset = frontPlane *segmentRadius;
			Vec3f corner1 = center - radius*0.75f * AtoBDir2d - radius*0.25f * Vec3f::UnitZ() + frontOffset;
			Vec3f corner2 = center + radius*0.75f * AtoBDir2d - radius*0.25f * Vec3f::UnitZ() + frontOffset;
			Vec3f corner3 = center + radius*0.75f * AtoBDir2d + radius*0.25f * Vec3f::UnitZ() + frontOffset;
			Vec3f corner4 = center - radius*0.75f * AtoBDir2d + radius*0.25f * Vec3f::UnitZ() + frontOffset;
			displayList->PushQuad(corner1, corner2, corner3, corner4, whiteshapeColor);
		}
		{
			VisualShapeColor redshapeColor;
			redshapeColor.m_triangleColor = VisualColor::Red;
			const Vec3f frontOffset = frontPlane *segmentRadius * 0.5f;
			const KyFloat32 sqrt2Over2 = 0.7071067f * radius;
			Vec3f v0 = frontOffset + center +     radius * AtoBDir2d +        0.f * Vec3f::UnitZ();
			Vec3f v1 = frontOffset + center + sqrt2Over2 * AtoBDir2d + sqrt2Over2 * Vec3f::UnitZ();
			Vec3f v2 = frontOffset + center +        0.f * AtoBDir2d +     radius * Vec3f::UnitZ();
			Vec3f v3 = frontOffset + center - sqrt2Over2 * AtoBDir2d + sqrt2Over2 * Vec3f::UnitZ();
			Vec3f v4 = frontOffset + center -     radius * AtoBDir2d -        0.f * Vec3f::UnitZ();
			Vec3f v5 = frontOffset + center - sqrt2Over2 * AtoBDir2d - sqrt2Over2 * Vec3f::UnitZ();
			Vec3f v6 = frontOffset + center -        0.f * AtoBDir2d -     radius * Vec3f::UnitZ();
			Vec3f v7 = frontOffset + center + sqrt2Over2 * AtoBDir2d - sqrt2Over2 * Vec3f::UnitZ();
			displayList->PushTriangle(center + frontOffset, v0, v1, redshapeColor);
			displayList->PushTriangle(center + frontOffset, v1, v2, redshapeColor);
			displayList->PushTriangle(center + frontOffset, v2, v3, redshapeColor);
			displayList->PushTriangle(center + frontOffset, v3, v4, redshapeColor);
			displayList->PushTriangle(center + frontOffset, v4, v5, redshapeColor);
			displayList->PushTriangle(center + frontOffset, v5, v6, redshapeColor);
			displayList->PushTriangle(center + frontOffset, v6, v7, redshapeColor);
			displayList->PushTriangle(center + frontOffset, v7, v0, redshapeColor);
		}
		
	}
	static void BuildOpenBox(ScopedDisplayList* displayList, const Vec3f& start, const Vec3f& dest,
		Vec3f& A, Vec3f& B, Vec3f& C, Vec3f& D, const Vec2f& AtoBDir2d, KyFloat32 minAlt, KyFloat32 maxAlt, bool doLinkToStartAndDest)
	{
		Vec3f A2 = A; 
		Vec3f B2 = B;
		Vec3f C2 = C;
		Vec3f D2 = D;
		A.z = minAlt;
		C.z = minAlt;
		B.z = minAlt;
		D.z = minAlt;
		A2.z = maxAlt;
		C2.z = maxAlt;
		B2.z = maxAlt;
		D2.z = maxAlt;

		VisualShapeColor shapeColor;
		shapeColor.m_triangleColor = VisualColor::Black;

		if (doLinkToStartAndDest)
		{
			displayList->PushLine(A, start, shapeColor.m_triangleColor);
			displayList->PushLine(C, start, shapeColor.m_triangleColor);
			displayList->PushLine(A2, start, shapeColor.m_triangleColor);
			displayList->PushLine(C2, start, shapeColor.m_triangleColor);
			displayList->PushLine(B, dest, shapeColor.m_triangleColor);
			displayList->PushLine(D, dest, shapeColor.m_triangleColor);
			displayList->PushLine(B2, dest, shapeColor.m_triangleColor);
			displayList->PushLine(D2, dest, shapeColor.m_triangleColor);
		}
		shapeColor.m_lineColor = VisualColor::Black;
		shapeColor.m_triangleColor = VisualColor::DimGray;

		BuildVerticalForbiddenWallLogo(displayList, A, B, B2, A2,           AtoBDir2d);
		displayList->PushQuad( A, B, B2, A2, shapeColor);

		BuildVerticalForbiddenWallLogo(displayList, C, A, A2, C2,  AtoBDir2d.PerpCW());
		displayList->PushQuad(C, A, A2, C2, shapeColor);

		BuildVerticalForbiddenWallLogo(displayList, D, C, C2, D2,          -AtoBDir2d);
		displayList->PushQuad(D, C, C2, D2, shapeColor);

		BuildVerticalForbiddenWallLogo(displayList, B, D, D2, B2, AtoBDir2d.PerpCCW());
		displayList->PushQuad(B, D, D2, B2, shapeColor);

	}

	static void BuildPropagationBox(ScopedDisplayList* displayList, const Vec3f& start, const Vec3f& dest, KyFloat32 propagationRadius)
	{
		OrientedBox2d box2d;
		box2d.InitAs2dInflatedSegment(start, dest, propagationRadius);
		Vec3f A = box2d.m_a;
		Vec3f B = box2d.m_a + box2d.m_normalizedOrientation*box2d.m_length;
		Vec3f C = box2d.m_a + box2d.m_normalizedOrientation.PerpCCW()*box2d.m_width;
		Vec3f D = B + (C - A);
		KyFloat32 minAlt = Min(start.z, dest.z) - 100.f;
		KyFloat32 maxAlt = Max(start.z, dest.z) + 100.f;

		BuildOpenBox(displayList, start, dest, A, B, C, D, box2d.m_normalizedOrientation, minAlt, maxAlt, true);
	}

	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0)
	{
		const AStarQueryBlob* aStarQueryBlob = (AStarQueryBlob*) blob;
		
		AStarQueryOutputBlob* aStarQueryOutputBlob = aStarQueryBlob->m_queryOutput.Ptr();
		if (aStarQueryOutputBlob != NULL)
		{
			Vec3f offsetVector = Vec3f::UnitZ();
			VisualShapeColor shapeColor;
			shapeColor.m_triangleColor = VisualColor::Red;
			switch((AStarQueryResult)aStarQueryOutputBlob->m_result)
			{
				case ASTAR_NOT_INITIALIZED : break;
				case ASTAR_NOT_PROCESSED :
				case ASTAR_PROCESSING_TRAVERSAL :
				case ASTAR_PROCESSING_TRAVERSAL_DONE :
				case ASTAR_PROCESSING_ABSTRACT_PATH :
				case ASTAR_PROCESSING_REFINING_INIT :
				case ASTAR_PROCESSING_REFINING :
				case ASTAR_PROCESSING_PATHCLAMPING_INIT :
				case ASTAR_PROCESSING_PATHCLAMPING :
				case ASTAR_PROCESSING_PATHBUILDING :
				case ASTAR_PROCESSING_CHANNEL_INIT :
				case ASTAR_PROCESSING_CHANNEL_COMPUTE :
					{
						shapeColor.m_triangleColor = VisualColor::Orange;
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case ASTAR_DONE_START_OUTSIDE:
					{
						displayList->PushText(aStarQueryBlob->m_startPos3f + offsetVector, shapeColor.m_triangleColor, "Start outside!");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case ASTAR_DONE_START_NAVTAG_FORBIDDEN:
					{
						displayList->PushText(aStarQueryBlob->m_startPos3f + offsetVector, shapeColor.m_triangleColor, "Start NavTag forbidden!");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case ASTAR_DONE_END_OUTSIDE:
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "Destination outside!");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case ASTAR_DONE_END_NAVTAG_FORBIDDEN:
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "Destination NavTag forbidden!");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case ASTAR_DONE_PATH_NOT_FOUND:
					{
						BuildPropagationBox(displayList, aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, aStarQueryBlob->m_propagationBoxExtent);
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "Path not found !");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case ASTAR_DONE_NAVDATA_CHANGED:
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "NavData changed!");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY:
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "Lack of working memory!");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case Kaim::ASTAR_DONE_COMPUTATION_ERROR :
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "Computation Error");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case Kaim::ASTAR_DONE_CHANNELCOMPUTATION_ERROR :
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "ChannelComputation Error");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
				case Kaim::ASTAR_DONE_DEST_IS_START_NO_PATH :
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "startPos=destPos, no Path computed");
					}
					break;
				case ASTAR_DONE_PATH_FOUND:
					{
						shapeColor.m_triangleColor = VisualColor::Lime;
						PathDisplayListBuilder pathDisplay;
						pathDisplay.DisplayPath(displayList, &aStarQueryOutputBlob->m_pathBlob);
						if (aStarQueryOutputBlob->m_abstractPathBlob.m_nodePositions.GetCount() != 0)
						{
							pathDisplay.m_edgesColor = Kaim::VisualColor::Lime;
							pathDisplay.DisplayPath(displayList, &aStarQueryOutputBlob->m_abstractPathBlob);
						}
					}
					break;
				case ASTAR_DONE_COMPUTATION_CANCELED:
					{
						displayList->PushText(aStarQueryBlob->m_destPos3f + offsetVector, shapeColor.m_triangleColor, "Query Canceled");
						displayList->PushLine(aStarQueryBlob->m_startPos3f, aStarQueryBlob->m_destPos3f, shapeColor.m_triangleColor);
					}
					break;
			}
		}
	}
};

}

#endif