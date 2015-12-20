/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{


KY_INLINE GateArrayComputer::GateArrayComputer()
	: m_advancedVisualDebuggingEnabled(false)
{
	Clear();
}

KY_INLINE void GateArrayComputer::Clear()
{
	m_database = KY_NULL;
	m_traverseLogicUserData = KY_NULL;
	m_stringPulledBubbleList = KY_NULL;
	m_channel = KY_NULL;
	m_currentBubbleIdx = KyUInt32MAXVAL;

	m_previousGateNavTriangle.Invalidate();

	m_result = GateArrayComputerResult_NOT_INITIALIZED;

	m_computationDurationMks = 0.f;
}



KY_INLINE void GateArrayComputer::Initialize(Database* database, const StringPulledBubbleList& stringPulledBubbleList, Channel* channel)
{
	Clear();

	m_database = database;
	m_stringPulledBubbleList = &stringPulledBubbleList;
	m_channel = channel;

	UpdateInternalParametersFromConfig();
	m_channel->Initialize();

	m_result = GateArrayComputerResult_NOT_PROCESSED;
}


KY_INLINE ChannelComputerConfig& GateArrayComputer::GetComputerConfig() { return m_computerConfig; }

KY_INLINE KyFloat32 GateArrayComputer::GetComputationDurationMks() const { return m_computationDurationMks; }
KY_INLINE void*     GateArrayComputer::GetTraverseLogicUserData()  const { return m_traverseLogicUserData;  }

KY_INLINE void GateArrayComputer::SetComputerConfig(const ChannelComputerConfig& computerConfig)
{
	m_computerConfig = computerConfig;
	UpdateInternalParametersFromConfig();
}

KY_INLINE void GateArrayComputer::SetTraverseLogicUserData(void* traverseLogicUserData) { m_traverseLogicUserData = traverseLogicUserData; }


template<class TLogic>
KY_INLINE void GateArrayComputer::Advance()
{
	ScopedProfilerSumMks scopedProfilerMks(&m_computationDurationMks);
	const KyUInt32 bubbleCount = m_stringPulledBubbleList->GetBubbleCount();

	switch(m_result)
	{
	case GateArrayComputerResult_NOT_PROCESSED :
	case GateArrayComputerResult_ComputingFirstGate :
		{
			if (m_result == GateArrayComputerResult_NOT_PROCESSED)
			{
				if (bubbleCount < 2)
				{
					m_result = GateArrayComputerResult_Done_InputError;
					return;
				}

				m_prevEdgeDir2d.Set(0.f, 0.f);
				m_currentBubbleIdx = 0;
				m_result = GateArrayComputerResult_ComputingFirstGate;
			}

			m_nextEdgeDir2d.Set(0.0f, 0.0f);
			if (m_currentBubbleIdx < bubbleCount - 1)
				m_nextEdgeDir2d = m_stringPulledBubbleList->GetEdge(m_currentBubbleIdx).m_normalizedPerpCCW.PerpCW();

			m_currentNewGate = Gate();

			if (KY_FAILED(TreatNextStringPulledBubble_FirstGate<TLogic>(m_currentBubbleIdx)))
			{
				m_result = GateArrayComputerResult_Done_FirstGateError;
				return;
			}

			if (m_channel->GetGateCount() != 0)
				m_result = GateArrayComputerResult_ComputingIntermediateGates;
			else
				m_result = GateArrayComputerResult_ComputingLastGate;

			break;
		}
	case GateArrayComputerResult_ComputingIntermediateGates :
		{
			if (KY_SUCCEEDED(ComputeIntermediaryGates<TLogic>()))
				m_result = GateArrayComputerResult_ComputingLastGate;
			else
				m_result = GateArrayComputerResult_Done_IntermediateGatesError;
			m_channelSectionWidener.Clear();
			break;
		}
	case GateArrayComputerResult_ComputingLastGate:
		{
			if (KY_FAILED(TreatNextStringPulledBubble_LastGate<TLogic>()))
			{
				m_result = GateArrayComputerResult_Done_LastGateError;
				return;
			}

			m_prevEdgeDir2d = m_nextEdgeDir2d;
			++m_currentBubbleIdx;

			if (m_currentBubbleIdx >= bubbleCount)
			{
				m_result = GateArrayComputerResult_Done;
				return;
			}

			m_result = GateArrayComputerResult_ComputingFirstGate;
			break;
		}

	default :
		break;
	}
}

template<class TLogic>
inline KyResult GateArrayComputer::Compute()
{
	while (IsFinished() == false)
	{
		Advance<TLogic>();
	}

	if (m_result == GateArrayComputerResult_Done)
		return KY_SUCCESS;

	return KY_ERROR;
}

template<class TLogic>
inline KyResult GateArrayComputer::TreatNextStringPulledBubble_LastGate()
{
	m_channel->AddGate(m_currentNewGate);
	if (m_currentNewGate.m_type == LeftTurnStart)
	{
		const KyFloat32 prevAngleDeg = GetAngleDegFromAbsoluteDirection(m_prevEdgeDir2d);
		const KyFloat32 nextAngleDeg = GetAngleDegFromAbsoluteDirection(m_nextEdgeDir2d);
		const KyFloat32 totalAngleDeg = (nextAngleDeg > prevAngleDeg) ? (nextAngleDeg - prevAngleDeg) : (nextAngleDeg - prevAngleDeg + 360.0f);
		if (totalAngleDeg > m_computerConfig.m_turnSamplingAngleDeg)
		{
			const KyUInt32 intermediaryGateCount = (KyUInt32)floorf(totalAngleDeg / m_computerConfig.m_turnSamplingAngleDeg);
			const KyFloat32 interGateAngleRad = (totalAngleDeg * KY_PI_DIVIDED_BY_180) / (1.0f + intermediaryGateCount);
			const KyFloat32 cosAngleStep = cosf(interGateAngleRad);
			const KyFloat32 sinAngleStep = sinf(interGateAngleRad);
			Vec2f orthoDir = m_prevEdgeDir2d.PerpCW();
			m_currentNewGate.m_type = LeftTurnIntermediary;
			for (KyUInt32 i = 0; i < intermediaryGateCount; ++i)
			{
				Rotate(orthoDir, cosAngleStep, sinAngleStep);
				KY_FORWARD_ERROR_NO_LOG(ComputeLeftTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate,
					m_channelCornerPos, orthoDir, m_channelCornerRadius));
				m_channel->AddGate(m_currentNewGate);
			}
		}

		m_currentNewGate.m_type = LeftTurnEnd;
		KY_FORWARD_ERROR_NO_LOG(ComputeLeftTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate,
			m_channelCornerPos, m_nextEdgeDir2d.PerpCW(), m_channelCornerRadius));
		m_channel->AddGate(m_currentNewGate);
	}

	if (m_currentNewGate.m_type == RightTurnStart)
	{
		const KyFloat32 prevAngleDeg = GetAngleDegFromAbsoluteDirection(m_prevEdgeDir2d);
		const KyFloat32 nextAngleDeg = GetAngleDegFromAbsoluteDirection(m_nextEdgeDir2d);
		const KyFloat32 totalAngleDeg = (nextAngleDeg > prevAngleDeg) ? (nextAngleDeg - prevAngleDeg - 360.0f) : (nextAngleDeg - prevAngleDeg);
		if (totalAngleDeg < -m_computerConfig.m_turnSamplingAngleDeg)
		{
			const KyUInt32 intermediaryGateCount = (KyUInt32)floorf(-totalAngleDeg / m_computerConfig.m_turnSamplingAngleDeg);
			const KyFloat32 interGateAngleRad = (totalAngleDeg * KY_PI_DIVIDED_BY_180) / (1.0f + intermediaryGateCount);
			const KyFloat32 cosAngleStep = cosf(interGateAngleRad);
			const KyFloat32 sinAngleStep = sinf(interGateAngleRad);
			Vec2f orthoDir = m_prevEdgeDir2d.PerpCCW();
			m_currentNewGate.m_type = RightTurnIntermediary;
			for (KyUInt32 i = 0; i < intermediaryGateCount; ++i)
			{
				Rotate(orthoDir, cosAngleStep, sinAngleStep);
				KY_FORWARD_ERROR_NO_LOG(ComputeRightTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate,
					m_channelCornerPos, orthoDir, m_channelCornerRadius));
				m_channel->AddGate(m_currentNewGate);
			}
		}

		m_currentNewGate.m_type = RightTurnEnd;
		KY_FORWARD_ERROR_NO_LOG(ComputeRightTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate,
			m_channelCornerPos, m_nextEdgeDir2d.PerpCCW(), m_channelCornerRadius));
		m_channel->AddGate(m_currentNewGate);
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult GateArrayComputer::TreatNextStringPulledBubble_FirstGate(KyUInt32 bubbleIdx)
{
	const Bubble& bubble = m_stringPulledBubbleList->GetBubble(bubbleIdx);

	m_channelCornerPos = bubble.GetCenter();
	m_channelCornerRadius = 2.0f * bubble.GetRadius();

	// We offset the Channel corner positions slightly inside the NavMesh.
	// But this constraint is relaxed when this lead to collapse the Gates. This relaxation is done capping
	// the margin to apply to string pulled bubble half radius (we never shrink gates more than to their half).
	const KyFloat32 margin = Min(0.25f * m_channelCornerRadius, m_database->GetRayCanGoMarginFloat(ChannelMargin));
	if (margin > 0.0)
	{
		// The DiagonalStrip corners are typically exactly on NavMesh borders, but we want to
		// ensure the Channel is defined inside the NavMesh (accordingly to the ChannelMargin).
		Vec2f prevEdgeOrthoDir2d = (bubble.GetRotationDirection() == CounterClockwise) ? m_prevEdgeDir2d.PerpCW() : m_prevEdgeDir2d.PerpCCW();
		Vec2f nextEdgeOrthoDir2d = (bubble.GetRotationDirection() == CounterClockwise) ? m_nextEdgeDir2d.PerpCW() : m_nextEdgeDir2d.PerpCCW();
		prevEdgeOrthoDir2d.Normalize();
		nextEdgeOrthoDir2d.Normalize();
		Vec2f bissector = prevEdgeOrthoDir2d + nextEdgeOrthoDir2d;
		if (DotProduct(m_prevEdgeDir2d, m_nextEdgeDir2d) < 0.71f) // just a bit more than sqrt(0.5) = 0.707
		{
			// When the edges are not almost in the same direction, we have some times to adjust bissector accordingly.
			const KyFloat32 bissectorDirRelativeToPrevEdge = DotProduct(m_prevEdgeDir2d, bissector);
			if (bissectorDirRelativeToPrevEdge == 0.0f)
			{
				// This case arise when previous and next edges are collinear. They are pointing in opposite
				// direction, we offset along the previous edge direction.
				bissector = m_prevEdgeDir2d;
			}
			else if (bissectorDirRelativeToPrevEdge < 0.0f)
			{
				// This case arise when the bubble radius is large enough to make the pulled string turn more
				// than 180 degrees around it. In that case, the sum of normals are going outwards the NavMesh,
				// we reverse the bisector to offset inside the NavMesh.
				bissector = -bissector;
			}
		}
		bissector.Normalize();
		m_channelCornerPos += margin * bissector;
		m_channelCornerRadius -= 2.0f * margin;
	}

	const bool isSmallTurn = (DotProduct(m_nextEdgeDir2d, m_prevEdgeDir2d) > m_minTurnAngleCos);

	if (bubbleIdx == 0)
	{
		m_currentNewGate.m_type = StartGate;
		m_currentNewGate.m_leftPos  = m_channelCornerPos;
		m_currentNewGate.m_pathPos  = m_channelCornerPos;
		m_currentNewGate.m_rightPos = m_channelCornerPos;
	}
	else if (bubbleIdx == m_stringPulledBubbleList->GetBubbleCount() - 1)
	{
		m_currentNewGate.m_type = EndGate;
		m_currentNewGate.m_leftPos  = m_channelCornerPos;
		m_currentNewGate.m_pathPos  = m_channelCornerPos;
		m_currentNewGate.m_rightPos = m_channelCornerPos;
	}
	else
	{
		switch (bubble.GetRotationDirection())
		{
		case UninitializedRotationDirection:
		case UndefinedRotationDirection:
		case NoRotation:
			return KY_ERROR;

		case CounterClockwise:
			if (isSmallTurn)
			{
				m_currentNewGate.m_type = SmallLeftTurn;
				KY_FORWARD_ERROR_NO_LOG(ComputeLeftTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate, m_channelCornerPos,
					0.5f * (m_prevEdgeDir2d.PerpCW() + m_nextEdgeDir2d.PerpCW()), m_channelCornerRadius));
			}
			else
			{
				m_currentNewGate.m_type = LeftTurnStart;
				KY_FORWARD_ERROR_NO_LOG(ComputeLeftTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate, m_channelCornerPos,
					m_prevEdgeDir2d.PerpCW(), m_channelCornerRadius));
			}
			break;

		case Clockwise:
			if (isSmallTurn)
			{
				m_currentNewGate.m_type = SmallRightTurn;
				KY_FORWARD_ERROR_NO_LOG(ComputeRightTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate, m_channelCornerPos,
					0.5f * (m_prevEdgeDir2d.PerpCCW() + m_nextEdgeDir2d.PerpCCW()), m_channelCornerRadius));
			}
			else
			{
				m_currentNewGate.m_type = RightTurnStart;
				KY_FORWARD_ERROR_NO_LOG(ComputeRightTurnGate<TLogic>(m_channel->GetGate(m_channel->GetGateCount() - 1), m_currentNewGate, m_channelCornerPos, m_prevEdgeDir2d.PerpCCW(), m_channelCornerRadius));
			}
			break;
		}
	}

	return KY_SUCCESS;
}


template<class TLogic>
inline KyResult GateArrayComputer::ComputeLeftTurnGate(const Gate& previousGate, Gate& gate, const Vec3f& leftCorner, const Vec2f& direction, KyFloat32 gateWidth)
{
	const Vec2f halfGate = (0.5f * gateWidth) * direction;
	const Vec3f pathPos = leftCorner + halfGate;

	RayCastQuery<TLogic> rayCast;
	rayCast.BindToDatabase(m_database);
	rayCast.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCast.Initialize(previousGate.m_pathPos, pathPos.Get2d() - previousGate.m_pathPos.Get2d());
	rayCast.SetPerformQueryStat(QueryStat_Channel);
	rayCast.PerformQuery();
	if (rayCast.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
		return KY_ERROR;

	gate.m_pathPos  = rayCast.GetArrivalPos();
	gate.m_leftPos  = gate.m_pathPos - halfGate;
	gate.m_rightPos = gate.m_pathPos + halfGate;

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult GateArrayComputer::ComputeRightTurnGate(const Gate& previousGate, Gate& gate, const Vec3f& rightCorner, const Vec2f& direction, KyFloat32 gateWidth)
{
	const Vec2f halfGate = (0.5f * gateWidth) * direction;
	const Vec3f pathPos = rightCorner + halfGate;

	RayCastQuery<TLogic> rayCast;
	rayCast.BindToDatabase(m_database);
	rayCast.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCast.Initialize(previousGate.m_pathPos, pathPos.Get2d() - previousGate.m_pathPos.Get2d());
	rayCast.SetPerformQueryStat(QueryStat_Channel);
	rayCast.PerformQuery();
	if (rayCast.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
		return KY_ERROR;

	gate.m_pathPos = rayCast.GetArrivalPos();
	gate.m_leftPos  = gate.m_pathPos + halfGate;
	gate.m_rightPos = gate.m_pathPos - halfGate;

	return KY_SUCCESS;
}

template<class TLogic>
KY_INLINE KyResult GateArrayComputer::ComputeIntermediaryGates()
{
	KY_ASSERT(m_channel->GetGateCount() != 0);
	m_prevGateIdx = m_channel->GetGateCount() - 1;
	Gate& prevGate = m_channel->GetGate(m_prevGateIdx);

	if ((prevGate.m_type == StartGate) ||
		(m_currentNewGate.m_type == EndGate) ||
		(SquareDistance2d(m_currentNewGate.m_pathPos, prevGate.m_pathPos) > m_intermediaryGateMinDistance * m_intermediaryGateMinDistance))
	{
		ClearancePolygonIntersector clearancePolygonIntersector;
		InitClearancePolygonIntersectorForCurrentSection(clearancePolygonIntersector);

		QueryUtils queryUtils(m_database, m_database->GetWorkingMemory(), GetTraverseLogicUserData());

		BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector>
			edgeIntersectionVisitor(GetTraverseLogicUserData(), clearancePolygonIntersector);

		BestFirstSearchTraversal<BestFirstSearch2dBorderCollector<TLogic, ClearancePolygonIntersector> >
			traversalForPolylineCast(queryUtils, m_database->GetActiveData()->GetCellBox(), edgeIntersectionVisitor);

		TriangleFromPosQuery triangleQuery;
		triangleQuery.BindToDatabase(m_database);
		triangleQuery.Initialize(prevGate.m_pathPos);
		triangleQuery.SetPerformQueryStat(QueryStat_Channel);
		triangleQuery.PerformQuery();
		if (triangleQuery.GetResult() != TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND)
			return KY_ERROR;

		m_previousGateNavTriangle = triangleQuery.GetResultTrianglePtr();

		if ((traversalForPolylineCast.SetStartTriangle(m_previousGateNavTriangle.GetRawPtr()) != TraversalResult_DONE) ||
			(traversalForPolylineCast.Search() != TraversalResult_DONE))
		{
			return KY_ERROR;
		}

		m_channelSectionWidener.Initialize(m_database, m_channel, &prevGate, &m_currentNewGate, m_previousGateNavTriangle);
		KY_FORWARD_ERROR_NO_LOG(m_channelSectionWidener.Compute(
			clearancePolygonIntersector.m_borderEdgesOnLeft, clearancePolygonIntersector.m_borderEdgesOnRight
			));
	}

	return KY_SUCCESS;
}


} // namespace Kaim

