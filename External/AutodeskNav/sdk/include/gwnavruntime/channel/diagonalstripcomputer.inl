/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{


class DiagonalStripComputerContext
{
public:
	Vec3f GetOrthoStartLeft()  { return m_edgeStart + m_orthoEdge2D * m_idealClearanceRadius; }
	Vec3f GetOrthoStartRight() { return m_edgeStart - m_orthoEdge2D * m_idealClearanceRadius; }
	Vec3f GetOrthoEndLeft()    { return   m_edgeEnd + m_orthoEdge2D * m_idealClearanceRadius; }
	Vec3f GetOrthoEndRight()   { return   m_edgeEnd - m_orthoEdge2D * m_idealClearanceRadius; }
public:
	NavTrianglePtr m_currentTriangle;
	Vec3f m_edgeStart;
	Vec3f m_edgeEnd;
	Vec3f m_orthoEdge2D;
	Vec3f m_edge;
	Vec3f m_edgeDir2D;
	Vec3f m_edgedir3D;
	Vec3f m_currentStartOnEdge;

	KyFloat32 m_edgeLength2D;
	KyFloat32 m_idealClearanceRadius;
	KyFloat32 m_dotProdForLeftStart;
	KyFloat32 m_dotProdForRightStart;
	KyFloat32 m_dotProdForLeftEnd;
	KyFloat32 m_dotProdForRightEnd;
	KyFloat32 m_crossProdForLeftStart;
	KyFloat32 m_crossProdForRightStart;
	KyFloat32 m_crossProdForLeftEnd;
	KyFloat32 m_crossProdForRightEnd;
	KyFloat32 m_edgeLength2D_Inv;
	KyFloat32 m_edgeSquareLength2d;
	Vec3f m_endPointOnLeft;
	Vec3f m_endPointOnRight;
	DiagonalStripJunctionType m_endJuntionType;
};


// Class for scoped object defined in DiagonalStripComputer::Advance
KY_EXIT_CLASS_SCOPE_BEGIN(DiagonalStripComputer, Advance)
	self->ClearDisplayList();
KY_EXIT_CLASS_SCOPE_END
inline void DiagonalStripComputer::ClearDisplayList()
{
	KY_DELETE_AND_SET_NULL(m_displayList);
}

template<class TLogic>
inline void DiagonalStripComputer::Advance(WorkingMemory* workingMemory, ChannelArrayComputerStats& channelStats)
{
	switch (m_result)
	{
	case DiagonalStripResult_NOT_PROCESSED :
	case DiagonalStripResult_BuildDiagonalForPathEdge :
		{
			// uncomment for VisualDebug
			// KY_EXIT_SCOPE_INSTANCE(DiagonalStripComputer, Advance)
			// m_displayList = KY_NEW ScopedDisplayList(m_database->GetWorld(), DisplayList_Enable);
			// m_displayList->InitSingleFrameLifespan("polygon", "ClearanceStrip");

			if (m_result == DiagonalStripResult_NOT_PROCESSED)
			{
				if (m_currentPathEdgeIdx > m_lastEdgeIdxOfSection)
				{
					m_result = DiagonalStripResult_Done_DiagonalStripComputerError;
					return;
				}

				m_result = DiagonalStripResult_BuildDiagonalForPathEdge;
			}

			ScopedProfilerSumMs queryTimer(&channelStats.m_diagonalstripComputer_ComputeDiagonals);
			if (KY_FAILED(BuildDiagonalsForPathEdge<TLogic>(workingMemory)))
			{
				m_result = DiagonalStripResult_Done_DiagonalStripComputerError;
				return;
			}

			if (m_currentPathEdgeIdx < m_lastEdgeIdxOfSection)
			{
				++m_currentPathEdgeIdx;
				break;
			}

			// add the endPoint of the Path
			{
				const Vec3f& endPoint = m_rawPath->GetPathEdgeEndPosition(m_lastEdgeIdxOfSection);
				if ((endPoint - m_lastStripCornerLeft).GetSquareLength2d() > 0.00001f)// 1 mm difference
				{
					m_diagonalStrip->PushBackCorner(endPoint, LeftSide, 0.0f);
					m_lastStripCornerLeft = endPoint;
				}

				if ((endPoint - m_lastStripCornerRight).GetSquareLength2d() > 0.00001f)// 1 mm difference
				{
					m_diagonalStrip->PushBackCorner(endPoint, RightSide, 0.0f);
					m_lastStripCornerRight = endPoint;
				}
			}

			m_result = DiagonalStripResult_ComputingMaxRadius_Init;
			break;
		}
	case DiagonalStripResult_ComputingMaxRadius_Init :
		{
			const KyInt32 cornerCount = (KyInt32)m_diagonalStrip->GetCornerCount();
			if (cornerCount < 4)
			{
				m_result = DiagonalStripResult_Done_ComputeMaxRadiusError;
				return;
			}

			m_diagonalStrip->SetCornerMaxRadius(0, 0.0f);
			m_diagonalStrip->SetCornerMaxRadius(1, 0.0f);
			m_diagonalStrip->SetCornerMaxRadius(cornerCount - 1, 0.0f);
			m_diagonalStrip->SetCornerMaxRadius(cornerCount - 2, 0.0f);

			const KyInt32 lastCornerIndexForMaxRadius = cornerCount - 2;
			m_currentCornerIdx = 2;

			if (m_currentCornerIdx < lastCornerIndexForMaxRadius)
			{
				// at least one corner radius to compute
				m_result = DiagonalStripResult_ComputingMaxRadius;
				return;
			}

			// skip the max radius computation
			m_result = DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward_Init;
			break;
		}
	case DiagonalStripResult_ComputingMaxRadius :
		{
			const KyInt32 cornerCount = (KyInt32)m_diagonalStrip->GetCornerCount();
			const KyInt32 lastCornerIndexForMaxRadius = cornerCount - 2;

			ScopedProfilerSumMs queryTimer(&channelStats.m_diagonalstripComputer_ComputeMaxRadius);
			const KyUInt32 numberOfCornerPerAdvance = 15;
			for(KyUInt32 i = 0; i < numberOfCornerPerAdvance; ++i)
			{
				KY_ASSERT(m_currentCornerIdx < lastCornerIndexForMaxRadius);

				if (KY_FAILED(ComputeCornerRadius(m_currentCornerIdx)))
				{
					m_result = DiagonalStripResult_Done_ComputeMaxRadiusError;
					return;
				}

				++m_currentCornerIdx;

				if (m_currentCornerIdx >= lastCornerIndexForMaxRadius)
					break;
			}

			if (m_currentCornerIdx < lastCornerIndexForMaxRadius)
				// Keep computing max radius
				return;

			m_result = DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward_Init;
			break;
		}
	case DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward_Init :
	case DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward :
		{
			const KyInt32 cornerCount = (KyInt32)m_diagonalStrip->GetCornerCount();
			const KyInt32 lastCornerIndexForCompatibleRadius_Backward = cornerCount - 1;

			if (m_result == DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward_Init)
			{
				m_currentCornerIdx = 1;
				if (m_currentCornerIdx >= lastCornerIndexForCompatibleRadius_Backward)
				{
					// no corner radius to compute backward
					m_result = DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward_Init;
					return;
				}

				m_result = DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward;
			}

			ScopedProfilerSumMs queryTimer(&channelStats.m_diagonalstripComputer_ComputeNeighbor);

			const KyUInt32 numberOfCornerPerAdvance = 150;
			for(KyUInt32 i = 0; i < numberOfCornerPerAdvance; ++i)
			{
				KY_ASSERT(m_currentCornerIdx < lastCornerIndexForCompatibleRadius_Backward);
				if (KY_FAILED(ComputeBackwardNeighborCompatibleCornerRadius(m_currentCornerIdx)))
				{
					m_result = DiagonalStripResult_Done_ComputeNeighborCompatibleRadiusError;
					return;
				}

				++m_currentCornerIdx;

				if (m_currentCornerIdx >= lastCornerIndexForCompatibleRadius_Backward)
					break;
			}

			if (m_currentCornerIdx < lastCornerIndexForCompatibleRadius_Backward)
				// keep computing backward
				return;

			m_result = DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward_Init;
			break;
		}
	case DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward_Init :
	case DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward :
		{
			if (m_result == DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward_Init)
			{
				m_currentCornerIdx = m_diagonalStrip->GetCornerCount() - 2;

				if (m_currentCornerIdx <= 0)
				{
					// nothing to do
					m_result = DiagonalStripResult_Done;
					return;
				}

				m_result = DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward;
			}

			ScopedProfilerSumMs queryTimer(&channelStats.m_diagonalstripComputer_ComputeNeighbor);

			const KyUInt32 numberOfCornerPerAdvance = 150;
			for(KyUInt32 i = 0; i < numberOfCornerPerAdvance; ++i)
			{
				KY_ASSERT(m_currentCornerIdx > 0);
				if (KY_FAILED(ComputeForwardNeighborCompatibleCornerRadius(m_currentCornerIdx)))
				{
					m_result = DiagonalStripResult_Done_ComputeNeighborCompatibleRadiusError;
					return;
				}

				--m_currentCornerIdx;

				if (m_currentCornerIdx <= 0)
					break;
			}

			if (m_currentCornerIdx > 0)
				// go on
				return;

			m_result = DiagonalStripResult_Done;
			break;
		}
	default:
		break;
	}
}

template<class TLogic>
inline KyResult DiagonalStripComputer::ComputeDiagonalsFromPath(WorkingMemory* workingMemory, ChannelArrayComputerStats& channelStats)
{
	while (IsFinished() == false)
	{
		Advance<TLogic>(workingMemory, channelStats);
	}

	if (m_result == DiagonalStripResult_Done)
		return KY_SUCCESS;

	return KY_ERROR;
}

template<class TLogic>
inline KyResult DiagonalStripComputer::BuildDiagonalsForPathEdge(WorkingMemory* workingMemory)
{
	DiagonalStripComputerContext context;
	InitContext(context, workingMemory);

	if (m_displayList != KY_NULL)
	{
		m_displayList->PushLine(context.m_edgeStart, context.m_edgeEnd, VisualColor::Yellow);
		char str[64];
		KyUInt32 textLength = (KyUInt32) SFsprintf(str, 64, "Edge Idx : %u", m_currentPathEdgeIdx);
		m_displayList->PushText((context.m_edgeStart + context.m_edgeEnd) * 0.5f + Vec3f(0.1f, 0.1f, 0.1f), VisualColor::Yellow, str, textLength);
	}

	KyFloat32 maxDotProdStart = Max(context.m_dotProdForRightStart, context.m_dotProdForLeftStart);
	KyFloat32 minDotProdEnd   = Min(context.m_dotProdForRightEnd, context.m_dotProdForLeftEnd);

	Vec3f bissectricIntersection;
	bool bissectriceIntersect = Intersections::SegmentVsSegment2d(m_startPointOnLeft, m_startPointOnRight, context.m_endPointOnLeft, context.m_endPointOnRight, bissectricIntersection);
	if (bissectriceIntersect || minDotProdEnd < maxDotProdStart)
	{
		KY_FORWARD_ERROR_NO_LOG(ProcessPolygonWithTriangles<TLogic>(workingMemory, context, bissectricIntersection, bissectriceIntersect, maxDotProdStart, minDotProdEnd));
	}
	else
	{
		if (m_startJunctionType == JunctionType_Slanting)
		{
			KY_FORWARD_ERROR_NO_LOG(ProcessTheTwoTriangleStartingThePolygon<TLogic>(workingMemory, context));
			maxDotProdStart = Max(context.m_dotProdForRightStart, context.m_dotProdForLeftStart);
			bissectriceIntersect = Intersections::SegmentVsSegment2d(m_startPointOnLeft, m_startPointOnRight, context.m_endPointOnLeft, context.m_endPointOnRight, bissectricIntersection);
		}

		if (context.m_endJuntionType == JunctionType_AlmostOrtho)
		{
			if (IsCCWQuadConvex(m_startPointOnRight, context.m_endPointOnRight, context.m_endPointOnLeft, m_startPointOnLeft))
			{
				// displayListForPolygonAourndPath.PushQuad(firstPointOnRight, secondPointOnRight, secondPointOnLeft, firstPointOnLeft, quadColor);
				KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTrapeze<TLogic>(workingMemory, context.m_currentStartOnEdge, context.m_edgeEnd,
					context.m_currentTriangle, m_startPointOnLeft, context.m_endPointOnLeft));
			}
			else
			{
				KY_FORWARD_ERROR_NO_LOG(ProcessPolygonWithTriangles<TLogic>(workingMemory, context, bissectricIntersection, bissectriceIntersect, maxDotProdStart, minDotProdEnd));
			}
		}
		else
		{
			KY_FORWARD_ERROR_NO_LOG(EndPolygonFromAlmostOrthoStart<TLogic>(workingMemory, context, maxDotProdStart, minDotProdEnd));
		}
	}

	m_startPointOnLeft  = context.m_endPointOnLeft;
	m_startPointOnRight = context.m_endPointOnRight;
	m_startJunctionType = context.m_endJuntionType;
	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult DiagonalStripComputer::ProcessTheTwoTriangleStartingThePolygon(WorkingMemory* workingMemory, DiagonalStripComputerContext& context)
{
	// if (dotProdForLeftStart < 0.f)                   if (dotProdForLeftStart >= 0.f)
	//        P    Q                                              R
	//         +   +-------------                                 +------------------
	//         |   |                                              | 
	//      +  |   |                                              +A     
	//      B\ |   |                                             /|     
	//        \|   |                                            / |
	//         +-----------                                    +---------
	//          \  |                                          /|  |      
	//           \ |                                         / |  |      
	//           A +                                      B +  |  |      
	//             |                                           |  |
	//             R---------------                          P +  Q-------------
	// we turn around firstPointOnRight to form           we turn around firstPointOnLeft to form  
	// the triangles BAP and PAQ                          the triangles BAP and PAQ 
	// with A = firstPointOnRight                         with A = firstPointOnLeft
	//  and B = firstPointOnLeft                           and B = firstPointOnRight
	bool turnAroundFirstPointOnRight =  context.m_dotProdForLeftStart < 0.f;

	KY_ASSERT(turnAroundFirstPointOnRight ? context.m_crossProdForRightStart <= 0 : context.m_crossProdForLeftStart >= 0);
	KY_ASSERT(turnAroundFirstPointOnRight ?   context.m_dotProdForRightStart >= 0 :   context.m_dotProdForLeftStart >= 0);

	const Vec3f& A = turnAroundFirstPointOnRight ? m_startPointOnRight :  m_startPointOnLeft;
	const Vec3f& B = turnAroundFirstPointOnRight ?  m_startPointOnLeft : m_startPointOnRight;

	const Vec3f& P = turnAroundFirstPointOnRight ?  context.GetOrthoStartLeft() : context.GetOrthoStartRight();

	const KyFloat32 distAToEdge_x_EdgeLength2D = turnAroundFirstPointOnRight ? -context.m_crossProdForRightStart : context.m_crossProdForLeftStart;
	const KyFloat32 distPQ_x_EdgeLength2D      = turnAroundFirstPointOnRight ?    context.m_dotProdForRightStart : context.m_dotProdForLeftStart;

	const KyFloat32 distAToEdge = distAToEdge_x_EdgeLength2D * context.m_edgeLength2D_Inv;
	const KyFloat32 distPQ      = distPQ_x_EdgeLength2D      * context.m_edgeLength2D_Inv;
	const KyFloat32 distQA      = context.m_idealClearanceRadius + distAToEdge;
	const KyFloat32 distAR      = context.m_idealClearanceRadius - distAToEdge;

	const Vec3f Q = turnAroundFirstPointOnRight ? A + context.m_orthoEdge2D * distQA : A - context.m_orthoEdge2D * distQA;
	const Vec3f R = turnAroundFirstPointOnRight ? A - context.m_orthoEdge2D * distAR : A + context.m_orthoEdge2D * distAR;

	const Vec3f middleRQ = (R+Q)*0.5f;
	const Vec3f edge_Vs_PA_Intersect = context.m_edgeStart + context.m_edge * (context.m_idealClearanceRadius * distPQ / distQA) * context.m_edgeLength2D_Inv;

	if (turnAroundFirstPointOnRight)
	{
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_edgeStart, edge_Vs_PA_Intersect, context.m_currentTriangle, B, A, P, A));
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, edge_Vs_PA_Intersect, middleRQ, context.m_currentTriangle, P, A, Q, A));
		m_startPointOnLeft  = Q;
		m_startPointOnRight = R;
	}
	else
	{
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_edgeStart, edge_Vs_PA_Intersect, context.m_currentTriangle, A, B, A, P));
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, edge_Vs_PA_Intersect, middleRQ, context.m_currentTriangle, A, P, A, Q));
		m_startPointOnLeft  = R;
		m_startPointOnRight = Q;
	}

	context.m_currentStartOnEdge = middleRQ;
	m_startJunctionType = JunctionType_AlmostOrtho;

	context.m_dotProdForLeftStart    =   DotProduct2d(context.m_edge,  m_startPointOnLeft - context.m_edgeStart);
	context.m_dotProdForRightStart   =   DotProduct2d(context.m_edge, m_startPointOnRight - context.m_edgeStart);
	context.m_crossProdForLeftStart  = CrossProduct_z(context.m_edge,  m_startPointOnLeft - context.m_edgeStart);
	context.m_crossProdForRightStart = CrossProduct_z(context.m_edge, m_startPointOnRight - context.m_edgeStart);

	return KY_SUCCESS;
}


template<class TLogic>
inline KyResult DiagonalStripComputer::EndPolygonFromAlmostOrthoStart(WorkingMemory* workingMemory, DiagonalStripComputerContext& context,
	KyFloat32 maxDotProdStart, KyFloat32 minDotProdEnd)
{
	
	/*
	// turnAroundsecondPointOnRight == true       turnAroundsecondPointOnRight == false
	// ------ +Q +P                                      --------+R                              
	//        |  |                                               |                               
	//        |  |  +B                                           + A                             
	//        |  | /                                             |\
	//        |  |/                                              | \
	// ----------+                                        ----------+
	//        | /                                                |  |\
	//        |/                                                 |  | \
	//        +A                                                 |  |  + B                       
	//        |                                                  |  |                            
	// -------+R                                                 +Q + P                          
	// we end the quad and we turn                      we end the quad and  we turn             
	// around secondPointOnRight to form                around secondPointOnLeft to form         
	// the triangle BAP and PAQ                         the triangle BAP and PAQ                 
	//   with A = secondPointOnRight                     with A = secondPointOnLeft              
	//   with B = secondPointOnRight                     with B = secondPointOnRight             
	*/
	bool turnAroundsecondPointOnRight = context.m_dotProdForLeftEnd > context.m_edgeSquareLength2d;

	KY_ASSERT(turnAroundsecondPointOnRight ?                  context.m_crossProdForRightEnd <= 0 :                  context.m_crossProdForLeftEnd >= 0);
	KY_ASSERT(turnAroundsecondPointOnRight ? context.m_edgeSquareLength2d-context.m_dotProdForRightEnd >= 0 : context.m_edgeSquareLength2d-context.m_dotProdForLeftEnd >= 0);

	const Vec3f& A = turnAroundsecondPointOnRight ? context.m_endPointOnRight :   context.m_endPointOnLeft;
	const Vec3f& B = turnAroundsecondPointOnRight ?  context.m_endPointOnLeft :  context.m_endPointOnRight;
	const Vec3f& P = turnAroundsecondPointOnRight ? context.GetOrthoEndLeft() : context.GetOrthoEndRight();

	const KyFloat32 distAToEdge_x_EdgeLength2D = turnAroundsecondPointOnRight ?  -context.m_crossProdForRightEnd : context.m_crossProdForLeftEnd;
	const KyFloat32 distPQ_x_EdgeLength2D = context.m_edgeSquareLength2d - (turnAroundsecondPointOnRight ? context.m_dotProdForRightEnd : context.m_dotProdForLeftEnd);

	const KyFloat32 distAToEdge = distAToEdge_x_EdgeLength2D * context.m_edgeLength2D_Inv;
	const KyFloat32 distPQ      = distPQ_x_EdgeLength2D      * context.m_edgeLength2D_Inv;
	const KyFloat32 distQA      = context.m_idealClearanceRadius + distAToEdge;
	const KyFloat32 distAR      = context.m_idealClearanceRadius - distAToEdge;

	const Vec3f R = turnAroundsecondPointOnRight ? A - context.m_orthoEdge2D * distAR : A + context.m_orthoEdge2D * distAR;
	const Vec3f Q = turnAroundsecondPointOnRight ? A + context.m_orthoEdge2D * distQA : A - context.m_orthoEdge2D * distQA;

	const Vec3f middleRQ = (R+Q)*0.5f;
	const Vec3f edgePAIntersect = context.m_edgeEnd - context.m_edge * (context.m_idealClearanceRadius * distPQ / distQA) * context.m_edgeLength2D_Inv;

	if (minDotProdEnd != maxDotProdStart)
	{
		if (turnAroundsecondPointOnRight)
		{
			if (IsCCWQuadConvex(m_startPointOnRight, R, Q, m_startPointOnLeft))
			{
				// displayListForPolygonAourndPath.PushQuad(firstPointOnRight, R, Q, firstPointOnLeft, quadColor);
				KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTrapeze<TLogic>(workingMemory, context.m_currentStartOnEdge, middleRQ, context.m_currentTriangle,
					m_startPointOnLeft, Q));
			}
			else
			{
				if (KY_FAILED(ProcessTwoOppositeTriangles<TLogic>(workingMemory, context, Q, R, middleRQ)))
					return KY_ERROR;
			}
		}
		else
		{
			if (IsCCWQuadConvex(m_startPointOnRight, Q, R, m_startPointOnLeft))
			{
				// displayListForPolygonAourndPath.PushQuad(firstPointOnRight, Q, R, firstPointOnLeft, quadColor);
				KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTrapeze<TLogic>(workingMemory, context.m_currentStartOnEdge, middleRQ, context.m_currentTriangle,
					m_startPointOnLeft, R));
			}
			else
			{
				if (KY_FAILED(ProcessTwoOppositeTriangles<TLogic>(workingMemory, context, R, Q, middleRQ)))
					return KY_ERROR;
			}
		}
	}

	if (turnAroundsecondPointOnRight)
	{
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, middleRQ, edgePAIntersect, context.m_currentTriangle, Q, A, P, A));
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, edgePAIntersect, context.m_edgeEnd, context.m_currentTriangle, P, A, B, A));
	}
	else
	{
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, middleRQ, edgePAIntersect, context.m_currentTriangle, A, Q, A, P));
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, edgePAIntersect, context.m_edgeEnd, context.m_currentTriangle, A, P, A, B));
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult DiagonalStripComputer::ProcessPolygonWithTriangles(WorkingMemory* workingMemory, DiagonalStripComputerContext& context,
	Vec3f& bissectricIntersection, bool bissectriceIntersect, KyFloat32 maxDotProdStart, KyFloat32 minDotProdEnd)
{
	if (bissectriceIntersect == false)
	{
		KyFloat32 crossProdIntersection = 0.f;
		if (minDotProdEnd >= 0.f && maxDotProdStart <= context.m_edgeSquareLength2d)
		{
			// we try to compute of the intersection only if it has a chance to happen between edgeStart and EdgeEnd
			if (Intersections::LineVsLine2d(m_startPointOnLeft, m_startPointOnRight, context.m_endPointOnLeft, context.m_endPointOnRight, bissectricIntersection))
			{
				bissectriceIntersect = true;
				crossProdIntersection = CrossProduct_z(context.m_edge, bissectricIntersection - context.m_edgeStart);
			}
		}

		if (bissectriceIntersect == false || fabs(crossProdIntersection) >= context.m_edgeLength2D*context.m_idealClearanceRadius)
		{
			return ProcessTwoOppositeTriangles<TLogic>(workingMemory, context, context.m_endPointOnLeft, context.m_endPointOnRight, context.m_edgeEnd);
		}
	}

	return ProcessTrianglesTurningAroundBissectriceIntersection<TLogic>(workingMemory, context, bissectricIntersection);
}

template<class TLogic>
inline KyResult DiagonalStripComputer::ProcessTwoOppositeTriangles(WorkingMemory* workingMemory, DiagonalStripComputerContext& context,
	const Vec3f& endPointLeft, const Vec3f& endPointRight, const Vec3f& endPointOnPath)
{
	// check if the diagonal going from m_startPointOnRight to endPointLeft is the good one
	KyFloat32 crossProd1 = CrossProduct_z(endPointRight - m_startPointOnRight, endPointLeft - m_startPointOnRight);
	KyFloat32 crossProd2 = CrossProduct_z(endPointLeft - m_startPointOnRight, m_startPointOnLeft - m_startPointOnRight);
	if (crossProd1 > 0.f && crossProd2 > 0.f)
	{
		// turn around m_startPointOnRight and then around endPointLeft
		// compute the intersection of endPointLeft && m_startPointOnRight
		Vec3f intersect;
		if (Intersections::SegmentVsSegment2d(context.m_edgeStart, context.m_edgeEnd, endPointLeft, m_startPointOnRight, intersect) == false)
			return KY_ERROR;

		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, intersect, context.m_currentTriangle,
			m_startPointOnLeft, m_startPointOnRight, endPointLeft, m_startPointOnRight));
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, intersect, endPointOnPath, context.m_currentTriangle, endPointLeft,
			m_startPointOnRight, endPointLeft, endPointRight));
	}
	else
	{
		// turn around m_startPointOnLeft and then around endPointRight
		// compute the intersection of endPointRight && m_startPointOnLeft
		Vec3f intersect;
		if (Intersections::SegmentVsSegment2d(context.m_edgeStart, context.m_edgeEnd, endPointRight,  m_startPointOnLeft, intersect) == false)
			return KY_ERROR;

		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, intersect, context.m_currentTriangle,
			m_startPointOnLeft, m_startPointOnRight, m_startPointOnLeft, endPointRight));
		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, intersect, endPointOnPath, context.m_currentTriangle,
			m_startPointOnLeft, endPointRight, endPointLeft, endPointRight));
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult DiagonalStripComputer::ProcessTrianglesTurningAroundBissectriceIntersection(WorkingMemory* workingMemory, DiagonalStripComputerContext& context, const Vec3f& bissectricIntersection)
{
	KyFloat32 crossProdWithIntersect = CrossProduct_z(context.m_edge, bissectricIntersection - context.m_edgeStart);
	bool turnAroundLeft = crossProdWithIntersect > 0;
	if (turnAroundLeft)
	{
		m_startPointOnLeft = bissectricIntersection;
		if (context.m_edgeLength2D > context.m_idealClearanceRadius)
		{
			if (m_startJunctionType != JunctionType_AlmostOrtho)
			{
				const Vec3f orthoStartRight = context.GetOrthoStartRight();
				KyFloat32 firstTriangle_crossProd1 = CrossProduct_z(m_startPointOnRight - m_startPointOnLeft,           orthoStartRight - m_startPointOnLeft);
				KyFloat32 firstTriangle_crossProd2 = CrossProduct_z(orthoStartRight     - m_startPointOnLeft, context.m_endPointOnRight - m_startPointOnLeft);
				if (firstTriangle_crossProd1 > 0 && firstTriangle_crossProd2 > 0)
				{
					Vec3f intersect;
					if (Intersections::SegmentVsSegment2d(context.m_edgeStart, context.m_edgeEnd, m_startPointOnLeft, orthoStartRight, intersect) == false)
						return KY_ERROR;

					KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, intersect, context.m_currentTriangle,
						m_startPointOnLeft, m_startPointOnRight, m_startPointOnLeft, orthoStartRight));
					m_startPointOnRight = orthoStartRight;
					context.m_currentStartOnEdge = intersect;
				}
			}

			if (context.m_endJuntionType != JunctionType_AlmostOrtho)
			{
				const Vec3f orthoEndRight = context.GetOrthoEndRight();
				KyFloat32 secondtTriangle_crossProd1 = CrossProduct_z(m_startPointOnRight - m_startPointOnLeft,             orthoEndRight - m_startPointOnLeft);
				KyFloat32 secondTriangle_crossProd2 = CrossProduct_z(     orthoEndRight   - m_startPointOnLeft, context.m_endPointOnRight - m_startPointOnLeft);
				if (secondtTriangle_crossProd1 > 0 && secondTriangle_crossProd2 > 0)
				{
					Vec3f intersect;
					if (Intersections::SegmentVsSegment2d(context.m_edgeStart, context.m_edgeEnd, m_startPointOnLeft, orthoEndRight, intersect) == false)
						return KY_ERROR;

					KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, intersect, context.m_currentTriangle,
						m_startPointOnLeft, m_startPointOnRight, m_startPointOnLeft, orthoEndRight));
					m_startPointOnRight = orthoEndRight;
					context.m_currentStartOnEdge = intersect;
				}
			}
		}

		BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, context.m_edgeEnd, context.m_currentTriangle, m_startPointOnLeft,
			m_startPointOnRight, m_startPointOnLeft, context.m_endPointOnRight);
	}
	else
	{
		m_startPointOnRight = bissectricIntersection;

		if (context.m_edgeLength2D > context.m_idealClearanceRadius)
		{
			if (m_startJunctionType != JunctionType_AlmostOrtho)
			{
				const Vec3f orthoStartLeft = context.GetOrthoStartLeft();
				KyFloat32 firstTriangle_crossProd1 = CrossProduct_z(    orthoStartLeft - m_startPointOnRight,       m_startPointOnLeft - m_startPointOnRight);
				KyFloat32 firstTriangle_crossProd2 = CrossProduct_z(m_startPointOnLeft - m_startPointOnRight, context.m_endPointOnLeft - m_startPointOnRight);
				if (firstTriangle_crossProd1 > 0 && firstTriangle_crossProd2 > 0)
				{
					Vec3f intersect;
					if (Intersections::SegmentVsSegment2d(context.m_edgeStart, context.m_edgeEnd, m_startPointOnRight, orthoStartLeft, intersect)== false)
						return KY_ERROR;

					KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, intersect, context.m_currentTriangle,
						m_startPointOnLeft, m_startPointOnRight, orthoStartLeft, m_startPointOnRight));
					m_startPointOnLeft = orthoStartLeft;
					context.m_currentStartOnEdge = intersect;
				}
			}

			if (context.m_endJuntionType != JunctionType_AlmostOrtho)
			{
				const Vec3f orthoEndLeft = context.GetOrthoEndLeft();
				KyFloat32 secondtTriangle_crossProd1 = CrossProduct_z(orthoEndLeft - m_startPointOnRight, m_startPointOnLeft - m_startPointOnRight);
				KyFloat32 secondTriangle_crossProd2  = CrossProduct_z(m_startPointOnLeft - m_startPointOnRight, context.m_endPointOnLeft - m_startPointOnRight);
				if (secondtTriangle_crossProd1 > 0 && secondTriangle_crossProd2 > 0)
				{
					Vec3f intersect;
					if (Intersections::SegmentVsSegment2d(context.m_edgeStart, context.m_edgeEnd, m_startPointOnRight, orthoEndLeft, intersect) == false)
						return KY_ERROR;

					KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, intersect, context.m_currentTriangle,
						m_startPointOnLeft, m_startPointOnRight, orthoEndLeft, m_startPointOnRight));
					m_startPointOnLeft = orthoEndLeft;
					context.m_currentStartOnEdge = intersect;
				}
			}
		}

		KY_FORWARD_ERROR_NO_LOG(BuildDiagonalsInTriangle<TLogic>(workingMemory, context.m_currentStartOnEdge, context.m_edgeEnd, context.m_currentTriangle,
			m_startPointOnLeft, m_startPointOnRight, context.m_endPointOnLeft, m_startPointOnRight));
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult DiagonalStripComputer::BuildDiagonalsInTriangle(WorkingMemory* workingMemory,
	const Vec3f& pathEdgeStart, const Vec3f& pathEdgeEnd, NavTrianglePtr& inAndOutTrianglePtr,
	const Vec3f& pointOnStartSideLeft, const Vec3f& pointOnStartSideRight,
	const Vec3f& pointOnEndSideLeft, const Vec3f& pointOnEndSideRight)
{
	VisualShapeColor triangleColor;
	triangleColor.m_triangleColor = VisualColor::Orange;
	triangleColor.m_triangleColor.m_a = 30;
	triangleColor.m_lineColor = VisualColor::Black;

	ClearancePolygonIntersector clearancePolygonIntersector;
	KY_ASSERT(pointOnStartSideLeft == pointOnEndSideLeft || pointOnStartSideRight == pointOnEndSideRight);
	if (pointOnStartSideLeft == pointOnEndSideLeft)
	{
		clearancePolygonIntersector.InitTriangleAroundLeft(pathEdgeStart, pathEdgeEnd, pointOnStartSideLeft, pointOnStartSideRight, pointOnEndSideRight,m_database->GetDatabaseGenMetrics());
		if (m_displayList != KY_NULL)
			m_displayList->PushTriangle(pointOnStartSideLeft, pointOnStartSideRight, pointOnEndSideRight, triangleColor);
	}
	else
	{
		clearancePolygonIntersector.InitTriangleAroundRight(pathEdgeStart, pathEdgeEnd, pointOnStartSideLeft, pointOnEndSideLeft, pointOnStartSideRight, m_database->GetDatabaseGenMetrics());
		if (m_displayList != KY_NULL)
			m_displayList->PushTriangle(pointOnStartSideLeft, pointOnStartSideRight, pointOnEndSideLeft, triangleColor);
	}

	KY_FORWARD_ERROR_NO_LOG(ComputeTriangleFromStartToEnd(workingMemory, pathEdgeStart, pathEdgeEnd, inAndOutTrianglePtr));

	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), clearancePolygonIntersector);
	BestFirstSearchTraversal<BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector> > traversalForSegmentCast(queryUtils, m_database->GetActiveData()->GetCellBox(),
	edgeIntersectionVisitor);


	if (m_queryDynamicOutput != KY_NULL)
	{
		if (m_displayList != KY_NULL)
		{
			VisualShapeColor crossedTriangleColor;
			crossedTriangleColor.m_triangleColor = VisualColor::Yellow;
			crossedTriangleColor.m_triangleColor.m_a = 60;
			crossedTriangleColor.m_lineColor = VisualColor::Black;
			Vec3f v0, v1, v2;
			for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetNavTrianglePtrCount(); ++i)
			{
				const NavTrianglePtr& triangle = m_queryDynamicOutput->GetNavTrianglePtr(i);
				triangle.GetVerticesPos3f(v0, v1, v2);
				m_displayList->PushTriangle(v0, v1, v2, crossedTriangleColor);
			}
		}

		for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetNavTrianglePtrCount(); ++i)
		{
			const NavTrianglePtr& triangle = m_queryDynamicOutput->GetNavTrianglePtr(i);
			if (traversalForSegmentCast.SetStartTriangle(triangle.GetRawPtr()) != TraversalResult_DONE)
				return KY_ERROR;
		}
	}

	if (traversalForSegmentCast.Search() != TraversalResult_DONE)
		return KY_ERROR;

	FillDiagonalStrip(clearancePolygonIntersector);
	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult DiagonalStripComputer::BuildDiagonalsInTrapeze(WorkingMemory* workingMemory, const Vec3f& pathEdgeStart, const Vec3f& pathEdgeEnd,
	NavTrianglePtr& inAndOutTrianglePtr, const Vec3f& pointOnStartSideLeft, const Vec3f& pointOnEndSideLeft)
{
	KY_FORWARD_ERROR_NO_LOG(ComputeTriangleFromStartToEnd(workingMemory, pathEdgeStart, pathEdgeEnd, inAndOutTrianglePtr));

	ClearancePolygonIntersector clearancePolygonIntersector;
	clearancePolygonIntersector.InitTrapezoid(pathEdgeStart, pathEdgeEnd, pointOnStartSideLeft, pointOnEndSideLeft, m_database->GetDatabaseGenMetrics());
	BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), clearancePolygonIntersector);
	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	BestFirstSearchTraversal<BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector> > traversalForSegmentCast(queryUtils, m_database->GetActiveData()->GetCellBox(),
		edgeIntersectionVisitor);

	if (m_displayList != KY_NULL)
	{
		VisualShapeColor quadColor;
		quadColor.m_triangleColor = VisualColor::Green;
		quadColor.m_triangleColor.m_a = 30;
		quadColor.m_lineColor = VisualColor::Black;

		m_displayList->PushQuad(clearancePolygonIntersector.m_onStartLeftPos3f, clearancePolygonIntersector.m_onStartRightPos3f,
		clearancePolygonIntersector.m_onEndRightPos3f, clearancePolygonIntersector.m_onEndLeftPos3f, quadColor);
	}


	if (m_queryDynamicOutput != KY_NULL)
	{
		if (m_displayList != KY_NULL)
		{
			VisualShapeColor crossedTriangleColor;
			crossedTriangleColor.m_triangleColor = VisualColor::Yellow;
			crossedTriangleColor.m_triangleColor.m_a = 60;
			crossedTriangleColor.m_lineColor = VisualColor::Black;
			Vec3f v0, v1, v2;
			for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetNavTrianglePtrCount(); ++i)
			{
				const NavTrianglePtr& triangle = m_queryDynamicOutput->GetNavTrianglePtr(i);
				triangle.GetVerticesPos3f(v0, v1, v2);
				m_displayList->PushTriangle(v0, v1, v2, crossedTriangleColor);
			}
		}

		for(KyUInt32 i = 0; i < m_queryDynamicOutput->GetNavTrianglePtrCount(); ++i)
		{
			const NavTrianglePtr& triangle = m_queryDynamicOutput->GetNavTrianglePtr(i);
			if (traversalForSegmentCast.SetStartTriangle(triangle.GetRawPtr()) != TraversalResult_DONE)
				return KY_ERROR;
		}
	}

	if (traversalForSegmentCast.Search() != TraversalResult_DONE)
		return KY_ERROR;

	FillDiagonalStrip(clearancePolygonIntersector);

	return KY_SUCCESS;
}


} // namespace Kaim
