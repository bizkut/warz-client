/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

namespace Kaim
{

//#define DBG_SendChannelArrayComputerVisualDebug


KY_INLINE ChannelArrayComputer::ChannelArrayComputer() : m_advancedVisualDebuggingEnabled(false)
{
#ifdef DBG_SendChannelArrayComputerVisualDebug
	m_advancedVisualDebuggingEnabled = true;
#endif
	Clear();
}

KY_INLINE ChannelArrayComputer::~ChannelArrayComputer() { Clear(); }

KY_INLINE void ChannelArrayComputer::Clear()
{
	m_stats = ChannelArrayComputerStats();

	m_traverseLogicUserData = KY_NULL;
	m_rawPath = KY_NULL;
	m_channelArray = KY_NULL;
	m_pathWithChannels = KY_NULL;
	m_channelUnderConstruction = KY_NULL;

	m_result = ChannelArrayResult_NOT_INITIALIZED;

	m_diagonalStripComputer.Clear();
	m_gateArrayComputer.Clear();
	m_diagonalStrip.Clear();
	m_bubbleArray.Clear();
	m_stringPulledBubbleList.Clear();
}

KY_INLINE void ChannelArrayComputer::Init(Path* rawPath)
{
	Clear();
	m_rawPath = rawPath;
	m_result = ChannelArrayResult_NOT_PROCESSED;
}

template<class TLogic>
inline KyResult ChannelArrayComputer::FullCompute(Path* rawPath, Ptr<Path>& pathWithChannels, WorkingMemory* workingMemory)
{
	Init(rawPath);

	while (IsFinished() == false)
	{
		Advance<TLogic>(workingMemory);
	}

	if (m_result != ChannelArrayResult_Done)
		return KY_ERROR;

	pathWithChannels = m_pathWithChannels;

	m_channelArray = KY_NULL;
	m_pathWithChannels = KY_NULL;
	return KY_SUCCESS;
}

template<class TLogic>
inline void ChannelArrayComputer::Advance(WorkingMemory* workingMemory)
{
	ScopedProfilerSumMs queryTimer(&m_stats.m_totalTime);

	switch (m_result)
	{
	case ChannelArrayResult_NOT_PROCESSED :
		{
			if (m_rawPath == KY_NULL || m_rawPath->GetNodeCount() < 2)
			{
				m_result = ChannelArrayResult_Error_InvalidRawPath;
				return;
			}

			//channelArray->m_channelPersistentDisplayListIds.Initialize(m_rawPath->m_database->GetWorld());
			m_navGraphSectionBoundaryIndices.Clear();
			m_currentSectionStartNodeIdx = 0;
			m_currentSectionEndNodeIdx = 0;
			m_channelArray = *KY_NEW ChannelArray;
			m_result = ChannelArrayResult_StartNewPathSection;

			if (m_advancedVisualDebuggingEnabled)
			{
				// Render intermediary result
				ScopedProfilerSumMs queryTimer(&m_stats.m_visualDebug);

				const VisualColor edgeColor = VisualColor::Lime;
				const VisualShapeColor nodeColor(VisualShapeColor::Inactive(), VisualColor::Cyan);

				ScopedDisplayList displayList(m_rawPath->m_database->GetWorld());
				displayList.InitSingleFrameLifespan("RawPath", "ChannelComputer");

				const KyUInt32 nodeCount = m_rawPath->GetNodeCount();
				Vec3f p0 = m_rawPath->GetNodePosition(0);
				for (KyUInt32 i = 1; i < nodeCount; ++i)
				{
					const Vec3f& p1 = m_rawPath->GetNodePosition(i);
					displayList.PushLine(p0, p1, edgeColor);
					displayList.PushPoint_Tetrahedron(p1, 0.1f, nodeColor);
					p0 = p1;
				}
			}

			break;
		}

	case ChannelArrayResult_StartNewPathSection :
		{
			KY_ASSERT(m_currentSectionStartNodeIdx < m_rawPath->GetNodeCount() - 1);

			if (m_rawPath->GetPathEdgeType(m_currentSectionStartNodeIdx) == PathEdgeType_OnNavMesh)
			{
				FindEndNodeIdxOfNavMeshSection();
				KY_ASSERT(m_currentSectionEndNodeIdx <= m_rawPath->GetNodeCount() - 1);
				m_result = ChannelArrayResult_ComputingChannel_DiagonalStrip_Init;
				return;
			}

			FindEndNodeIdxOfNonNavMeshSection();
			KY_ASSERT(m_currentSectionEndNodeIdx <= m_rawPath->GetNodeCount() - 1);

			m_navGraphSectionBoundaryIndices.PushBack(m_currentSectionStartNodeIdx);
			m_navGraphSectionBoundaryIndices.PushBack(m_currentSectionEndNodeIdx);

			if (m_currentSectionEndNodeIdx == m_rawPath->GetNodeCount() - 1)
			{
				m_result = ChannelArrayResult_ComputingAggregatedPath;
			}
			else
			{
				m_currentSectionStartNodeIdx = m_currentSectionEndNodeIdx;
			}

			break;
		}
	case ChannelArrayResult_ComputingChannel_DiagonalStrip_Init :
		{
			DiagonalStripComputerConfig diagonalStripComputerConfig;
			diagonalStripComputerConfig.m_idealCornerRadius = m_channelComputerConfig.m_idealClearanceRadius;
			m_diagonalStripComputer.SetTraverseLogicUserData(GetTraverseLogicUserData());
			m_diagonalStripComputer.Init(m_rawPath->m_database, m_rawPath, m_currentSectionStartNodeIdx, m_currentSectionEndNodeIdx - 1, &m_diagonalStrip);
			m_diagonalStripComputer.SetComputerConfig(diagonalStripComputerConfig);
			m_diagonalStrip.Clear();
			m_bubbleArray.Clear();
			m_result = ChannelArrayResult_ComputingChannel_DiagonalStrip;
			break;
		}
	case ChannelArrayResult_ComputingChannel_DiagonalStrip :
		{
			{
				ScopedProfilerSumMs queryTimer(&m_stats.m_diagonalstripComputer);

				m_diagonalStripComputer.Advance<TLogic>(workingMemory, m_stats);

				if (m_diagonalStripComputer.IsFinished() == false)
					break;
			}

			if (m_advancedVisualDebuggingEnabled)
			{
				// Render intermediary result
				ScopedProfilerSumMs queryTimer(&m_stats.m_visualDebug);
				DiagonalStripDisplayConfig displayConfig;
				m_diagonalStrip.SendVisualDebug(m_rawPath->m_database->GetWorld(), displayConfig);
			}

			if (m_diagonalStripComputer.GetResult() != DiagonalStripResult_Done)
				m_result = ChannelArrayResult_Error_ChannelComputation_DiagonalStrip;
			else
			{
				m_result = ChannelArrayResult_ComputingChannel_StringPull;
				BubbleArrayBuilder::ConvertDiagonalStripToBubbleArray(m_diagonalStrip, m_bubbleArray);

				if (m_advancedVisualDebuggingEnabled)
				{
					// Render intermediary result
					ScopedProfilerSumMs queryTimer(&m_stats.m_visualDebug);
					BubbleArrayDisplayConfig displayConfig;
					m_bubbleArray.SendVisualDebug(m_rawPath->m_database->GetWorld(), displayConfig, "BubbleArray", "ChannelComputer");
				}
			}

			break;
		}
	case ChannelArrayResult_ComputingChannel_StringPull :
		{
			StringPullerComputerConfig stringPullerComputerConfig;
			stringPullerComputerConfig.m_idealClearanceRadius = m_channelComputerConfig.m_idealClearanceRadius;
			stringPullerComputerConfig.m_minBiTangentLength = 0.01f;
			KyResult stringPullerResult;
			{
				ScopedProfilerSumMs queryTimer(&m_stats.m_stringPuller);
				StringPuller stringPuller;
				stringPuller.Initialize();
				stringPuller.SetComputerConfig(stringPullerComputerConfig);
				stringPullerResult = stringPuller.Compute(m_bubbleArray, m_stringPulledBubbleList);
			}

			if (m_advancedVisualDebuggingEnabled)
			{
				// Render intermediary result
				ScopedProfilerSumMs queryTimer(&m_stats.m_visualDebug);
				StringPulledBubbleListDisplayConfig displayConfig;
				m_stringPulledBubbleList.SendVisualDebug(m_rawPath->m_database->GetWorld(), displayConfig, "StringPuller", "ChannelComputer");
			}

			if (KY_FAILED(stringPullerResult))
				m_result = ChannelArrayResult_Error_ChannelComputation_StringPull;
			else
				m_result = ChannelArrayResult_ComputingChannel_GateArray_Init;

			break;
		}
	case ChannelArrayResult_ComputingChannel_GateArray_Init :
	case ChannelArrayResult_ComputingChannel_GateArray :
		{
			ScopedProfilerSumMs queryTimer(&m_stats.m_gateArrayComputer);

			if (m_result == ChannelArrayResult_ComputingChannel_GateArray_Init)
			{
				KY_ASSERT(m_channelUnderConstruction == KY_NULL);
				m_channelUnderConstruction = *KY_NEW Channel;
				m_gateArrayComputer.Initialize(m_rawPath->m_database, m_stringPulledBubbleList, m_channelUnderConstruction);
				m_gateArrayComputer.SetTraverseLogicUserData(GetTraverseLogicUserData());
				m_gateArrayComputer.m_advancedVisualDebuggingEnabled = m_advancedVisualDebuggingEnabled;
				m_gateArrayComputer.SetComputerConfig(m_channelComputerConfig);
				m_result = ChannelArrayResult_ComputingChannel_GateArray;
			}

			m_gateArrayComputer.Advance<TLogic>();

			if (m_gateArrayComputer.IsFinished() == false)
				break;

			if (m_advancedVisualDebuggingEnabled)
			{
				// Render intermediary result
				ScopedProfilerSumMs queryTimer(&m_stats.m_visualDebug);
				ChannelDisplayConfig displayConfig;
				m_channelUnderConstruction->SendVisualDebug(m_rawPath->m_database->GetWorld(), displayConfig);
			}

			if (m_gateArrayComputer.GetResult() != GateArrayComputerResult_Done)
			{
				m_result = ChannelArrayResult_Error_ChannelComputation_GateArray;
				return;
			}

			m_result = ChannelArrayResult_ComputingChannel_PreChannelPolyline;
			break;
		}

	case ChannelArrayResult_ComputingChannel_PreChannelPolyline:
		{
			if (KY_FAILED(ComputePreChannelPolyline<TLogic>(m_channelUnderConstruction)))
			{
				m_result = ChannelArrayResult_Error_ChannelComputation_PreChannelPolyline;
				return;
			}

			m_result = ChannelArrayResult_ComputingChannel_PostChannelPolyline;
			break;
		}

	case ChannelArrayResult_ComputingChannel_PostChannelPolyline:
		{
			if (KY_FAILED(ComputePostChannelPolyline<TLogic>(m_channelUnderConstruction)))
			{
				m_result = ChannelArrayResult_Error_ChannelComputation_PostChannelPolyline;
				return;
			}

			m_channelArray->m_channelArray.PushBack(m_channelUnderConstruction);
			m_channelUnderConstruction = KY_NULL;

			if (m_currentSectionEndNodeIdx == m_rawPath->GetNodeCount() - 1)
			{
				m_result = ChannelArrayResult_ComputingAggregatedPath;
			}
			else
			{
				m_currentSectionStartNodeIdx = m_currentSectionEndNodeIdx;
				m_result = ChannelArrayResult_StartNewPathSection;
			}

			break;
		}

	case ChannelArrayResult_ComputingAggregatedPath :
		{
			if (KY_FAILED(CreateAggregatedPath<TLogic>()))
			{
				m_result  = ChannelArrayResult_Error_PathAggregation;
				return;
			}

			m_result = ChannelArrayResult_Done;
		}
	default:
		break;
	}
}


KY_INLINE void ChannelArrayComputer::FindEndNodeIdxOfNavMeshSection()
{
	const KyUInt32 pathEdgeCount = m_rawPath->GetEdgeCount();
	for (m_currentSectionEndNodeIdx = m_currentSectionStartNodeIdx + 1; m_currentSectionEndNodeIdx < pathEdgeCount; ++m_currentSectionEndNodeIdx)
	{
		if (m_rawPath->GetPathEdgeType(m_currentSectionEndNodeIdx) != PathEdgeType_OnNavMesh)
			return;
	}
}

KY_INLINE void ChannelArrayComputer::FindEndNodeIdxOfNonNavMeshSection()
{
	const KyUInt32 pathEdgeCount = m_rawPath->GetEdgeCount();
	for (m_currentSectionEndNodeIdx = m_currentSectionStartNodeIdx + 1; m_currentSectionEndNodeIdx < pathEdgeCount; ++m_currentSectionEndNodeIdx)
	{
		if (m_rawPath->GetPathEdgeType(m_currentSectionEndNodeIdx) == PathEdgeType_OnNavMesh)
			return;
	}
}


namespace
{
	template <class TLogic>
	class ExtendedPredicate : public TLogic
	{
	public:
		static bool CanTraverseGraphEdgeAndGetCost(void* /*traverseLogicUserData*/, const Vec3f& start, const Vec3f& end, const NavTag& /*navTag*/, KyFloat32& cost)
		{
			cost = Distance(start, end);
			return true;
		}
	};

}

template<class TLogic>
inline KyResult ChannelArrayComputer::ComputePreChannelPolyline(Channel* channel)
{
	/************************************************************************/
	/*                                      L0                              */
	/*      +-----------+-------------------+ - - _ _                       */
	/*      |          /                    |         - - _ _   L1          */
	/*      |         V                     |                 - +           */
	/*      |   +<---+                      | G                 .           */
	/*      |    \                          | A                 .           */
	/*      |     \                         | T                 . G         */
	/*      |      \                        | E                 . A         */
	/*      |       V                       |                   . T         */
	/*      |        +                      | 0                 . E         */
	/*      |       /                       |                   .           */
	/*      |      V                        |                   . 1         */
	/*      +-----+-------------------------+ - - - _ _ _       .           */
	/*                                     R0             - - - +           */
	/*                                                         R1           */
	/*                                                                      */
	/************************************************************************/

	// We need the polyline going from the left to the right. We thus initialize
	// the ClearancePolygon intersector as a rectangle centered on gate #0, from
	// the left to the right. After projection, we will use the left border list.

	if ((channel == KY_NULL) || (channel->GetGateCount() == 0))
		return KY_ERROR;

	Database* database = m_rawPath->m_database;
	const Gate& firstGate = channel->GetGate(0);

	ClearancePolygonIntersector clearancePolygonIntersector;
	const Vec3f& gateLeftPos  = firstGate.m_leftPos;
	const Vec3f& gateRightPos = firstGate.m_rightPos;
	const Vec2f rectangleCentralEdge = gateRightPos.Get2d() - gateLeftPos.Get2d();
	const Vec2f rectangleLeftHalfWidthVec = rectangleCentralEdge.PerpCCW() * (m_channelComputerConfig.m_idealClearanceRadius / rectangleCentralEdge.GetLength());
	clearancePolygonIntersector.InitTrapezoid(gateLeftPos, gateRightPos, gateLeftPos + rectangleLeftHalfWidthVec, gateRightPos + rectangleLeftHalfWidthVec, database->GetDatabaseGenMetrics());

	QueryUtils queryUtils(database, database->GetWorkingMemory(), GetTraverseLogicUserData());

	BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector>
		edgeIntersectionVisitor(GetTraverseLogicUserData(), clearancePolygonIntersector);

	BestFirstSearchTraversal<BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector> >
		traversalForPolylineCast(queryUtils, database->GetActiveData()->GetCellBox(), edgeIntersectionVisitor);

	TriangleFromPosQuery triangleQuery;
	triangleQuery.BindToDatabase(database);
	triangleQuery.Initialize(firstGate.m_pathPos);
	triangleQuery.SetPerformQueryStat(QueryStat_Channel);
	triangleQuery.PerformQuery();
	if (triangleQuery.GetResult() != TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND)
		return KY_ERROR;

	NavTrianglePtr navTriangle = triangleQuery.GetResultTrianglePtr();
	if ((traversalForPolylineCast.SetStartTriangle(navTriangle.GetRawPtr()) != TraversalResult_DONE) ||
		(traversalForPolylineCast.Search() != TraversalResult_DONE))
	{
		return KY_ERROR;
	}

	const KyFloat32 sqMinDistance = m_channelComputerConfig.m_channelIntermediaryGateCollapseDistance * m_channelComputerConfig.m_channelIntermediaryGateCollapseDistance;
	KyArray<Vec3f> polyline;
	polyline.PushBack(gateLeftPos);
	Vec3f lastInserted = gateLeftPos;
	for (SharedPoolList<ClearancePolygonIntersector::BorderEdge>::ConstIterator borderIt = clearancePolygonIntersector.m_borderEdgesOnRight.Begin(); borderIt != clearancePolygonIntersector.m_borderEdgesOnRight.End(); ++borderIt)
	{
		if (SquareDistance2d(lastInserted, borderIt->m_startPos) > sqMinDistance)
			polyline.PushBack(borderIt->m_startPos);
		polyline.PushBack(borderIt->m_endPos);
		lastInserted = borderIt->m_endPos;
	}
	if (SquareDistance2d(lastInserted, gateRightPos) > sqMinDistance)
		polyline.PushBack(gateRightPos);

	channel->SetPreChannelPolyline(polyline);

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult ChannelArrayComputer::ComputePostChannelPolyline(Channel* channel)
{
	/************************************************************************/
	/*      Ln-2                                                            */
	/*      + - - _ _                                                       */
	/*      .         - - _ _   Ln-1                                        */
	/*      .                 - +------------------------+----->+           */
	/*      . G                 ^                       ^       ^           */
	/*      . A                 |                      /        |           */
	/*      . T                 | G              +--->+         |           */
	/*      . E                 | A               ^             |           */
	/*      .                   | T                \            |           */
	/*      . n-2               | E                 \           |           */
	/*      .                   |                    +          |           */
	/*      .                   | n-1               ^           |           */
	/*      + - - - _ _ _       |                  /            |           */
	/*      Rn-2          - - - +-----------------+------------>+           */
	/*                         Rn-1                                         */
	/*                                                                      */
	/************************************************************************/

	// We need the polyline going from the right to the left. We thus initialize
	// the ClearancePolygon intersector as a rectangle centered on gate #0, from
	// the right to the left. After projection, we will use the right border list.

	if ((channel == KY_NULL) || (channel->GetGateCount() == 0))
		return KY_ERROR;

	Database* database = m_rawPath->m_database;
	const Gate& lastGate = channel->GetGate(channel->GetGateCount() - 1);

	ClearancePolygonIntersector clearancePolygonIntersector;
	const Vec3f& gateLeftPos  = lastGate.m_leftPos;
	const Vec3f& gateRightPos = lastGate.m_rightPos;
	const Vec2f rectangleCentralEdge = gateLeftPos.Get2d() - gateRightPos.Get2d();
	const Vec2f rectangleLeftHalfWidthVec = rectangleCentralEdge.PerpCCW() * (m_channelComputerConfig.m_idealClearanceRadius / rectangleCentralEdge.GetLength());
	clearancePolygonIntersector.InitTrapezoid(gateRightPos, gateLeftPos, gateRightPos + rectangleLeftHalfWidthVec, gateLeftPos + rectangleLeftHalfWidthVec, database->GetDatabaseGenMetrics());

	QueryUtils queryUtils(database, database->GetWorkingMemory(), GetTraverseLogicUserData());

	BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector>
		edgeIntersectionVisitor(GetTraverseLogicUserData(), clearancePolygonIntersector);

	BestFirstSearchTraversal<BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector> >
		traversalForPolylineCast(queryUtils, database->GetActiveData()->GetCellBox(), edgeIntersectionVisitor);

	TriangleFromPosQuery triangleQuery;
	triangleQuery.BindToDatabase(database);
	triangleQuery.Initialize(lastGate.m_pathPos);
	triangleQuery.SetPerformQueryStat(QueryStat_Channel);
	triangleQuery.PerformQuery();
	if (triangleQuery.GetResult() != TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND)
		return KY_ERROR;

	NavTrianglePtr navTriangle = triangleQuery.GetResultTrianglePtr();
	if ((traversalForPolylineCast.SetStartTriangle(navTriangle.GetRawPtr()) != TraversalResult_DONE) ||
		(traversalForPolylineCast.Search() != TraversalResult_DONE))
	{
		return KY_ERROR;
	}

	const KyFloat32 sqMinDistance = m_channelComputerConfig.m_channelIntermediaryGateCollapseDistance * m_channelComputerConfig.m_channelIntermediaryGateCollapseDistance;
	KyArray<Vec3f> polyline;
	polyline.PushBack(gateRightPos);
	Vec3f lastInserted = gateRightPos;
	for (SharedPoolList<ClearancePolygonIntersector::BorderEdge>::ConstIterator borderIt = clearancePolygonIntersector.m_borderEdgesOnRight.Begin(); borderIt != clearancePolygonIntersector.m_borderEdgesOnRight.End(); ++borderIt)
	{
		if (SquareDistance2d(lastInserted, borderIt->m_startPos) > sqMinDistance)
			polyline.PushBack(borderIt->m_startPos);
		polyline.PushBack(borderIt->m_endPos);
		lastInserted = borderIt->m_endPos;
	}
	if (SquareDistance2d(lastInserted, gateLeftPos) > sqMinDistance)
		polyline.PushBack(gateLeftPos);

	channel->SetPostChannelPolyline(polyline);

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult ChannelArrayComputer::CreateAggregatedPath()
{
	ScopedProfilerSumMs queryTimer(&m_stats.m_createAggregatePath);

	const KyUInt32 channelCount = m_channelArray->GetChannelCount();
	const KyUInt32 navGraphSectionCount = m_navGraphSectionBoundaryIndices.GetCount() / 2;

	KY_ASSERT(m_navGraphSectionBoundaryIndices.GetCount() % 2 == 0);
	KY_ASSERT(channelCount <= navGraphSectionCount + 1);
	KY_ASSERT(navGraphSectionCount <= channelCount + 1);

	// Compute the m_rawPath for each Channel
	KyArray<Ptr<Path> > channelPathArray(channelCount);
	for (KyUInt32 channelIdx = 0; channelIdx < channelCount; ++channelIdx)
	{
		Ptr<Path> channelPath;
		Ptr<Channel>& channel = m_channelArray->m_channelArray[channelIdx];
		KY_FORWARD_ERROR_NO_LOG(CreateChannelPath<TLogic>(m_rawPath->m_database, *channel, channelPath));
		UpdateChannelSectionFromPath(channelPath, channelIdx, channel);
		channelPathArray[channelIdx] = channelPath;
	}

	//////////////////////////////////////////////////////////////////////////
	// Compute the aggregated Path

	// - compute aggregated Path edge count
	KyUInt32 channelArrayPathEdgeCount = 0;
	for (KyUInt32 i = 0; i < channelCount; ++i)
		channelArrayPathEdgeCount += channelPathArray[i]->GetEdgeCount();

	for (KyUInt32 i = 0; i < navGraphSectionCount; ++i)
		channelArrayPathEdgeCount += m_navGraphSectionBoundaryIndices[2 * i + 1] - m_navGraphSectionBoundaryIndices[2 * i];

	// - Create the aggregated Path
	Path::CreateConfig createConfig(channelArrayPathEdgeCount + 1);
	m_pathWithChannels = Path::CreatePath(createConfig);
	m_pathWithChannels->m_channelArray = m_channelArray;
	m_pathWithChannels->m_database = m_rawPath->m_database;
	m_pathWithChannels->SetPathCostAndDistance(m_rawPath->GetPathCost(), m_rawPath->GetPathDistance());
	
	// Init the aggregate path with the full raw path cellBox
	// it will be enlarge if needed when building from channel section
	m_pathWithChannels->SetPathCellBox(m_rawPath->GetPathCellBox());

	KyInt32 clearanceDistanceInt = 0;
	KyInt32 cellSizeInCoord = KyInt32MAXVAL;
	if (m_rawPath->m_database != KY_NULL)
	{
		clearanceDistanceInt = (KyInt32)m_rawPath->m_database->GetDatabaseGenMetrics().GetNearestInteger64FromFloatValue(m_channelComputerConfig.m_idealClearanceRadius);
		cellSizeInCoord      = (KyInt32)m_rawPath->m_database->GetDatabaseGenMetrics().m_cellSizeInCoord;
	}

	// - fill the aggregated Path
	KyUInt32 m_pathWithChannelsEdgeIndex = 0;
	KyUInt32 navGraphSectionIndexOffset = 0;
	if ((navGraphSectionCount > 0) && (m_navGraphSectionBoundaryIndices[0] == 0))
	{
		// The aggregated Path starts in a NavGraph
		KY_FORWARD_ERROR_NO_LOG(FillAggregatedPathWithRawPathSection(m_navGraphSectionBoundaryIndices[0], m_navGraphSectionBoundaryIndices[1] - 1, m_pathWithChannelsEdgeIndex));
		++navGraphSectionIndexOffset;
	}
	
	for (KyUInt32 i = 0; i < channelCount; ++i)
	{
		Channel* channel = m_channelArray->GetChannel(i);
		channel->SetFirstPathNodeIdx(m_pathWithChannelsEdgeIndex);

		KY_FORWARD_ERROR_NO_LOG(FillAggregatedPathWithChannelPath(channelPathArray[i], m_pathWithChannelsEdgeIndex, clearanceDistanceInt, cellSizeInCoord));

		const KyUInt32 nextNavGraphSectionIdx = i + navGraphSectionIndexOffset;
		if (nextNavGraphSectionIdx < navGraphSectionCount)
		{
			KyUInt32& nextNavGraphSectionStartIdx = m_navGraphSectionBoundaryIndices[2 * nextNavGraphSectionIdx];
			KyUInt32& nextNavGraphSectionEndIdx = m_navGraphSectionBoundaryIndices[2 * nextNavGraphSectionIdx + 1];
			KY_FORWARD_ERROR_NO_LOG(FillAggregatedPathWithRawPathSection(nextNavGraphSectionStartIdx, nextNavGraphSectionEndIdx - 1, m_pathWithChannelsEdgeIndex));
		}
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult ChannelArrayComputer::CreateChannelPath(Database* database, const Channel& channel, Ptr<Path>& channelPath)
{
	const KyUInt32 gateCount = channel.GetGateCount();
	if (gateCount == 0)
	{
		KY_LOG_WARNING(("There is no gate fir this channel, no channel Path can be created. Channel Computation is aborted"));
		return KY_ERROR;
	}

	WorkingMemory* workingMemory = database->GetWorkingMemory();
	PathFinderQueryUtils queryUtils(database, workingMemory, GetTraverseLogicUserData());
	queryUtils.SetPerformQueryStat(QueryStat_Channel);

	PathClamperContext* pathClamperContext = workingMemory->GetOrCreatePathClamperContext();
	if (KY_FAILED(pathClamperContext->InitFromChannel(database, workingMemory, channel)))
	{
		pathClamperContext->ReleaseWorkingMemory();
		return KY_ERROR;
	}

	KyUInt32 unsusedBecauseNotTimeSliced = 0;
	while (pathClamperContext->IsClampingDone() == false)
	{
		ClampResult clampResult = (ClampResult)queryUtils.ClampOneEdge(unsusedBecauseNotTimeSliced);
		if (clampResult != ClampResult_SUCCESS)
		{
			KY_LOG_WARNING(("We failed to clamp an edge of channelPath on the NavMesh"));
			pathClamperContext->ReleaseWorkingMemory();
			return KY_ERROR;
		}
	}

	channelPath = queryUtils.ComputePathFromPathClamperContext<ExtendedPredicate<TLogic> >(
		channel.GetGate(0).m_pathPos, channel.GetGate(gateCount - 1).m_pathPos, PositionSpatializationRange(100.f, 100.f));

	pathClamperContext->ReleaseWorkingMemory();
	return KY_SUCCESS;
}


inline void ChannelArrayComputer::UpdateChannelSectionFromPath(const Path* channelPath, KyUInt32 channelIdx, Ptr<Channel>& channel)
{
	if (channel->GetGateCount() == channelPath->GetNodeCount())
		return;

	Ptr<Channel> newChannel = *KY_NEW Channel;
	newChannel->Initialize();
	newChannel->SetPreChannelPolyline(channel->GetPreChannelPolyline());
	newChannel->SetPostChannelPolyline(channel->GetPostChannelPolyline());
	newChannel->AddGate(channel->GetGate(0)); // add first gate
	KyUInt32 sectionIdx = 1; // Ignore the first section, that is before the first gate.
	ChannelSectionPtr sectionPtr;
	for (KyUInt32 pathNodeIdx = 1; pathNodeIdx < channelPath->GetNodeCount(); ++pathNodeIdx)
	{
		const Vec3f& nodePos = channelPath->GetNodePosition(pathNodeIdx);
		
		// by construction node created from the channel are exactly the same!
		sectionPtr.Set(m_channelArray, channelIdx, sectionIdx);
		if (sectionPtr.GetNextGate().m_pathPos == nodePos)
		{
			newChannel->AddGate(sectionPtr.GetNextGate());
			++sectionIdx;
			continue;
		}

		// if path node, doesn't come from a gate, create one
		const Gate& previousGate = sectionPtr.GetPreviousGate();
		const Gate& nextGate = sectionPtr.GetNextGate();
		Gate newGate;
		newGate.m_type = ClampingAdjustment;
		newGate.m_pathPos = nodePos;
		Vec2f prevToNew = nodePos.Get2d() - previousGate.m_pathPos.Get2d();
		Vec2f prevToNext =  nextGate.m_pathPos.Get2d() - previousGate.m_pathPos.Get2d();
		KyFloat32 ratio = DotProduct(prevToNew, prevToNext) / DotProduct(prevToNext, prevToNext);
		newGate.m_leftPos = previousGate.m_leftPos;
		newGate.m_rightPos = previousGate.m_rightPos;
		if (previousGate.m_leftPos != nextGate.m_leftPos)
		{
			newGate.m_leftPos += (nextGate.m_leftPos - previousGate.m_leftPos) * ratio;
			newGate.m_leftPos.z = nodePos.z;
		}
		if (previousGate.m_rightPos != nextGate.m_rightPos)
		{
			newGate.m_rightPos += (nextGate.m_rightPos - previousGate.m_rightPos) * ratio;
			newGate.m_rightPos.z = nodePos.z;
		}
		newChannel->AddGate(newGate);
	}
	
	channel = newChannel;
}

inline KyResult ChannelArrayComputer::FillAggregatedPathWithChannelPath(const Path* channelPath, KyUInt32& currentPathWithChannelsEdgeIndex,
	KyInt32 clearanceDistanceInt, KyInt32 cellSizeInCoord)
{
	// Fill the nodes
	const KyUInt32 channelPathNodeCount = channelPath->GetNodeCount();

	CellBox& cellBox = m_pathWithChannels->GetPathCellBox();
	for (KyUInt32 i = 0; i < channelPathNodeCount; ++i)
	{
		// Set first node only if this is the first node of the full path. If it is not, the corresponding
		// node should have been set previously when feeding from raw path.
		if (i != 0 || currentPathWithChannelsEdgeIndex == 0)
			SetPathNodeToPathWithChannels(channelPath, i, currentPathWithChannelsEdgeIndex + i);
		const WorldIntegerPos& nodeIntegerPos = channelPath->GetNodeIntegerPosition(i);

		if (clearanceDistanceInt != 0)
		{
			if (nodeIntegerPos.m_cellPos.x == cellBox.Min().x &&
				(nodeIntegerPos.m_coordPosInCell.x - clearanceDistanceInt) < 0)
				cellBox.ExpandByVec2(nodeIntegerPos.m_cellPos.NeighborWest());

			if(nodeIntegerPos.m_cellPos.x == cellBox.Max().x &&
				(nodeIntegerPos.m_coordPosInCell.x + clearanceDistanceInt) > cellSizeInCoord)
				cellBox.ExpandByVec2(nodeIntegerPos.m_cellPos.NeighborEast());

			if(nodeIntegerPos.m_cellPos.y == cellBox.Min().y &&
				(nodeIntegerPos.m_coordPosInCell.y - clearanceDistanceInt) < 0)
				cellBox.ExpandByVec2(nodeIntegerPos.m_cellPos.NeighborSouth());

			if(nodeIntegerPos.m_cellPos.y == cellBox.Max().y &&
				(nodeIntegerPos.m_coordPosInCell.y + clearanceDistanceInt) > cellSizeInCoord)
				cellBox.ExpandByVec2(nodeIntegerPos.m_cellPos.NeighborNorth());
		}
	}

	// Fill the edges
	const KyUInt32 channelPathEdgeCount = channelPath->GetEdgeCount();
	for (KyUInt32 i = 0; i < channelPathEdgeCount; ++i)
		SetPathEdgeToPathWithChannels(channelPath, i, currentPathWithChannelsEdgeIndex + i);

	// Update current m_pathWithChannels edge index
	currentPathWithChannelsEdgeIndex += channelPathEdgeCount;

	return KY_SUCCESS;
}

inline KyResult ChannelArrayComputer::FillAggregatedPathWithRawPathSection(KyUInt32 firstEdgeIndex, KyUInt32 lastEdgeIndex,
	KyUInt32& currentPathWithChannelsEdgeIndex)
{
	// Need to be done even though this node was already set by the ChannelPath 
	// in order to set all information to the node, such as NavGraphVertexPtr
	SetPathNodeToPathWithChannels(m_rawPath, firstEdgeIndex, currentPathWithChannelsEdgeIndex);

	// Set the edges and internal Nodes.
	for (KyUInt32 i = firstEdgeIndex; i < lastEdgeIndex; ++i)
	{
		// - the current edge
		SetPathEdgeToPathWithChannels(m_rawPath, i, currentPathWithChannelsEdgeIndex);
		++currentPathWithChannelsEdgeIndex;

		// - the next node
		SetPathNodeToPathWithChannels(m_rawPath, i + 1, currentPathWithChannelsEdgeIndex);
	}
	// - the last edge
	SetPathEdgeToPathWithChannels(m_rawPath, lastEdgeIndex, currentPathWithChannelsEdgeIndex);
	++currentPathWithChannelsEdgeIndex;

	// Need to be done even though even though there is a channel right after
	// in order to set all information to the node, such as NavGraphVertexPtr
	SetPathNodeToPathWithChannels(m_rawPath, lastEdgeIndex + 1, currentPathWithChannelsEdgeIndex);

	return KY_SUCCESS;
}


inline void ChannelArrayComputer::SetPathNodeToPathWithChannels(const Path* path, KyUInt32 pathNodeIndex, KyUInt32 pathWithChannelsNodeIndex)
{
	m_pathWithChannels->SetNodePosition         (pathWithChannelsNodeIndex, path->GetNodePosition         (pathNodeIndex));
	m_pathWithChannels->SetNodeIntegerPosition  (pathWithChannelsNodeIndex, path->GetNodeIntegerPosition  (pathNodeIndex));
	m_pathWithChannels->SetNodeNavTrianglePtr   (pathWithChannelsNodeIndex, path->GetNodeNavTrianglePtr   (pathNodeIndex));
	m_pathWithChannels->SetNodeNavGraphVertexPtr(pathWithChannelsNodeIndex, path->GetNodeNavGraphVertexPtr(pathNodeIndex));
}

inline void ChannelArrayComputer::SetPathEdgeToPathWithChannels(const Path* path, KyUInt32 pathEdgeIndex, KyUInt32 pathWithChannelsEdgeIndex)
{
	m_pathWithChannels->SetEdgeNavGraphEdgePtr(pathWithChannelsEdgeIndex, path->GetEdgeNavGraphEdgePtr(pathEdgeIndex));
	m_pathWithChannels->SetPathEdgeType       (pathWithChannelsEdgeIndex, path->GetPathEdgeType       (pathEdgeIndex));
}

} // namespace Kaim
