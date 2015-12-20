/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY

namespace Kaim
{
KY_INLINE void PositionOnLivePath::ClearEventListLocation()
{
	m_currentOnEventListIdx = KyUInt32MAXVAL;
	m_onEventListStatus = OnEventListStatus_Undefined;
}
KY_INLINE bool                  PositionOnLivePath::IsAtFirstNodeOfPath()         const { return m_positionOnPath.IsAtFirstNodeOfPath();         }
KY_INLINE bool                  PositionOnLivePath::IsAtLastNodeOfPath()          const { return m_positionOnPath.IsAtLastNodeOfPath();          }
KY_INLINE bool                  PositionOnLivePath::IsAtFirstEdgeOfPath()         const { return m_positionOnPath.IsAtFirstEdgeOfPath();         }
KY_INLINE bool                  PositionOnLivePath::IsAtLastEdgeOfPath()          const { return m_positionOnPath.IsAtLastEdgeOfPath();          }
KY_INLINE KyUInt32              PositionOnLivePath::GetPathEdgeIndex()            const { return m_positionOnPath.GetPathEdgeIndex();            }
KY_INLINE KyUInt32              PositionOnLivePath::GetEdgeIdxToMoveOnForward()   const { return m_positionOnPath.GetEdgeIdxToMoveOnForward();   }
KY_INLINE KyUInt32              PositionOnLivePath::GetEdgeIdxToMoveOnBackward()  const { return m_positionOnPath.GetEdgeIdxToMoveOnBackward();  }
KY_INLINE PathEdgeType          PositionOnLivePath::GetEdgeTypeToMoveOnForward()  const { return m_positionOnPath.GetEdgeTypeToMoveOnForward();  }
KY_INLINE PathEdgeType          PositionOnLivePath::GetEdgeTypeToMoveOnBackward() const { return m_positionOnPath.GetEdgeTypeToMoveOnBackward(); }
KY_INLINE const Path*           PositionOnLivePath::GetPath()                     const { return m_positionOnPath.GetPath();                     }
KY_INLINE const Vec3f&          PositionOnLivePath::GetPosition()                 const { return m_positionOnPath.GetPosition();                 }
KY_INLINE PositionOnPathStatus  PositionOnLivePath::GetPositionOnPathStatus()     const { return m_positionOnPath.GetPositionOnPathStatus();     }
KY_INLINE PathEdgeType          PositionOnLivePath::GetPathEdgeType()             const { return m_positionOnPath.GetPathEdgeType();             }
KY_INLINE const PositionOnPath& PositionOnLivePath::GetPositionOnPath()           const { return m_positionOnPath;                               }
KY_INLINE KyUInt32              PositionOnLivePath::GetOnEventListIndex()         const { return m_currentOnEventListIdx;                        }
KY_INLINE OnEventListStatus     PositionOnLivePath::GetOnEventListStatus()        const { return m_onEventListStatus;                            }

KY_INLINE bool PositionOnLivePath::IsAtLowerBoundOfLivePath() const
{
	return m_onEventListStatus == OnEventListStatus_OnEvent && GetOnEventListIndex() == 0;
}
KY_INLINE bool PositionOnLivePath::IsAtUpperBoundOfLivePath() const
{
	return m_onEventListStatus == OnEventListStatus_OnEvent && (GetOnEventListIndex() + 1) == m_pathEventList->GetPathEventCount();
}

KY_INLINE bool PositionOnLivePath::IsAtACheckPoint() const
{
	return m_onEventListStatus == OnEventListStatus_OnEvent && m_pathEventList->GetPathEvent(GetOnEventListIndex()).IsACheckPoint();
}


KY_INLINE Path* PositionOnLivePath::GetPath() { return m_positionOnPath.GetPath(); }

KY_INLINE bool PositionOnLivePath::DoesPrecede(const PositionOnLivePath& other)      const { return m_positionOnPath.DoesPrecede(other.m_positionOnPath);      }
KY_INLINE bool PositionOnLivePath::IsStrictlyBefore(const PositionOnLivePath& other) const { return m_positionOnPath.IsStrictlyBefore(other.m_positionOnPath); }

KY_INLINE bool PositionOnLivePath::operator!=(const PositionOnLivePath& other) const { return !(*this == other); }
KY_INLINE bool PositionOnLivePath::operator==(const PositionOnLivePath& other) const
{
	return
		m_bot                   == other.m_bot                   &&
		m_positionOnPath        == other.m_positionOnPath        &&
		m_currentOnEventListIdx == other.m_currentOnEventListIdx &&
		m_onEventListStatus     == other.m_onEventListStatus;
}

KY_INLINE bool PositionOnLivePath::IsEventListStatusValid() const
{
	if (GetOnEventListStatus() == OnEventListStatus_Undefined)
	{
		// KY_LOG_ERROR(("You should not call this function if current PositionOnLivePath is not located on the PathEventList"));
		return false;
	}

	return true;
}


KY_INLINE void PositionOnLivePath::InitOnPathFirstNode(Path* path)
{
	ClearEventListLocation();
	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.InitOnPathFirstNode(path);
}

KY_INLINE void PositionOnLivePath::InitOnPathLastNode(Path* path)
{
	ClearEventListLocation();
	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.InitOnPathLastNode(path);
}

KY_INLINE void PositionOnLivePath::InitOnPathNode(Path* path, KyUInt32 pathNodeIdx)
{
	ClearEventListLocation();
	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.InitOnPathNode(path, pathNodeIdx);
}

KY_INLINE void PositionOnLivePath::InitOnPathEdge(Path* path, const Vec3f& posOnEdge, KyUInt32 pathEdgeIdx)
{
	ClearEventListLocation();
	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.InitOnPathEdge(path, posOnEdge, pathEdgeIdx);
}

KY_INLINE void PositionOnLivePath::InitOnPositionOnPath(const PositionOnPath& positionOnPath)
{
	ClearEventListLocation();
	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath = positionOnPath;
}

KY_INLINE void PositionOnLivePath::MoveForward_StopAtEvent(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveForward(distanceMove);
	StopAtEventAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::MoveBackward_StopAtEvent(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtLowerBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveBackward(distanceMove);
	StopAtEventAfterMovingBackward();
}

inline const NavTag* PositionOnLivePath::GetUpcomingEventNavTag() const
{
	OnEventListStatus onEventListStatus = GetOnEventListStatus();
	switch (onEventListStatus)
	{
	case OnEventListStatus_OnInterval :
		{
			KyUInt32 intervalIdx = GetOnEventListIndex();
			return m_pathEventList->GetNavTagOfEventInterval(intervalIdx);
		}
	case OnEventListStatus_OnEvent :
		{
			KyUInt32 eventIndex = GetOnEventListIndex();
			if (m_pathEventList->GetPathEvent(eventIndex).IsOnUpperBound() == false)
				return m_pathEventList->GetNavTagOnIntervalAfterEvent(eventIndex);
		}
	default :
		break;
	}

	return KY_NULL;
}

inline const NavTag* PositionOnLivePath::GetPreviousEventNavTag() const
{
	OnEventListStatus onEventListStatus = GetOnEventListStatus();
	switch (onEventListStatus)
	{
	case OnEventListStatus_OnInterval :
		{
			KyUInt32 intervalIdx = GetOnEventListIndex();
			return m_pathEventList->GetNavTagOfEventInterval(intervalIdx);
		}
	case OnEventListStatus_OnEvent :
		{
			KyUInt32 eventIndex = GetOnEventListIndex();
			if (m_pathEventList->GetPathEvent(eventIndex).IsOnLowerBound() == false)
				return m_pathEventList->GetNavTagOnIntervalBeforeEvent(eventIndex);
		}
	default :
		break;
	}

	return KY_NULL;
}


template<class TLogic>
inline void PositionOnLivePath::MoveForwardAndGetCost_StopAtEventOrAtPathNode(void* traverseLogicUserData, KyFloat32 distanceMove, RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	const Vec3f posBeforeMove = GetPosition();
	const NavTag* navTag = GetUpcomingEventNavTag();
	//KY_LOG_ERROR_IF(navTag == KY_NULL, ("This PositionOnLivePath is on the last event, which means we should have not tried to move it forward"));

	MoveForward_StopAtEventOrAtPathNode(distanceMove);
	GetCostAfterStopAtEventOrAtPathNode<TLogic>(traverseLogicUserData, posBeforeMove, navTag, rayCanGoMarginMode, cost, typename TLogic::CostMultiplierUsage());
}

template<class TLogic>
inline void PositionOnLivePath::GetCostAfterStopAtEventOrAtPathNode(void* /*traverseLogicUserData*/, const Vec3f& posBeforeMove, const NavTag* /*navTag*/,
	RayCanGoMarginMode /*rayCanGoMarginMode*/, KyFloat32& cost, const LogicWithoutCostMultipler&)
{
	cost = Distance(posBeforeMove, GetPosition());
}

template<class TLogic>
inline void PositionOnLivePath::GetCostAfterStopAtEventOrAtPathNode(void* traverseLogicUserData, const Vec3f& posBeforeMove, const NavTag* navTag,
	RayCanGoMarginMode /*rayCanGoMarginMode*/, KyFloat32& cost, const LogicWithCostMultiplerPerNavTag&)
{
	KyFloat32 costMultiplier = 1.f;
	if (navTag != KY_NULL)
	{
		bool unsudeBool = TLogic::CanTraverseNavTag(traverseLogicUserData, *navTag, &costMultiplier);
		KY_UNUSED(unsudeBool);
		KY_LOG_WARNING_IF(unsudeBool == false, ("as we are moving along the Path, The CanTraverseNavTag shoulf have succeeded"));
	}

	cost = Distance(posBeforeMove, GetPosition()) * costMultiplier;
};

template<class TLogic>
inline void PositionOnLivePath::GetCostAfterStopAtEventOrAtPathNode(void* traverseLogicUserData, const Vec3f& posBeforeMove, const NavTag* /*navTag*/,
	RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost, const LogicWithCostMultiplerPerTriangle&)
{
	PathEdgeType edgeType = m_positionOnPath.GetPathEdgeType();
	if (edgeType == PathEdgeType_OnNavMesh && m_positionOnPath.GetPath()->m_database != KY_NULL)
	{
		RayCanGoQuery<TLogic> canGoQuery;
		canGoQuery.BindToDatabase(m_positionOnPath.GetPath()->m_database);
		canGoQuery.SetTraverseLogicUserData(traverseLogicUserData);
		canGoQuery.SetMarginMode(rayCanGoMarginMode);
		canGoQuery.SetComputeCostMode(QUERY_COMPUTE_COST_ALONG_3DAXIS);

		canGoQuery.Initialize(posBeforeMove, GetPosition());
		// We do have the triangle !
		canGoQuery.SetPerformQueryStat(QueryStat_TargetOnPathShortcutForward);
		canGoQuery.PerformQuery();

		if (canGoQuery.GetResult() != Kaim::RAYCANGO_DONE_SUCCESS)
		{
			KY_LOG_WARNING(("as we are moving along the Path, The RayCanGoQuery shoulf have succeeded"));
			return;
		}

		cost = canGoQuery.GetComputedCost();
		return;
	}

	KyFloat32 costMultiplier = 1.f;

	if (edgeType == PathEdgeType_OnNavGraph)
	{
		const NavGraphEdgePtr& edgePtr = m_positionOnPath.GetPath()->GetEdgeNavGraphEdgePtr(GetPathEdgeIndex());
		if (edgePtr.IsValid())
		{
			bool unsudeBool = TLogic::CanTraverseNavGraphEdge(traverseLogicUserData, edgePtr.GetRawPtr(), &costMultiplier);
			KY_UNUSED(unsudeBool);
			KY_LOG_WARNING_IF(unsudeBool == false, ("as we are moving along the Path, The CanTraverseNavTag shoulf have succeeded"));
		}
	}

	cost = Distance(posBeforeMove, GetPosition()) * costMultiplier;
}



KY_INLINE KyUInt32 PositionOnLivePath::GetNextPathEventIdx() const
{
	if (m_currentOnEventListIdx >= m_pathEventList->GetPathEventCount())
	{
		KY_LOG_ERROR(("This PositionOnLivePath has not a correct location on the PathEventList, this index is invalid"));
		return KyUInt32MAXVAL;
	}

	return GetNextPathEventIdx_Unsafe();
}

KY_INLINE KyUInt32 PositionOnLivePath::GetPrevPathEventIdx() const
{
	if (m_currentOnEventListIdx == 0 || m_currentOnEventListIdx >= m_pathEventList->GetPathEventCount())
	{
		KY_LOG_ERROR(("This PositionOnLivePath has not a correct location on the PathEventList, this index is invalid"));
		return KyUInt32MAXVAL;
	}

	return GetPrevPathEventIdx_Unsafe();
}


KY_INLINE void PositionOnLivePath::MoveToPathEvent(KyUInt32 pathEventIdx)
{
	KY_DEBUG_ASSERTN(pathEventIdx != KyUInt32MAXVAL, ("Error, path event index is invalid"));
	KY_DEBUG_ASSERTN(pathEventIdx < m_pathEventList->GetPathEventCount(), ("Error, path event index is invalid because out of bound"));
	const PositionOnPath& eventPosOnPath = m_pathEventList->GetPathEvent(pathEventIdx).m_positionOnPath;

	m_positionOnPath = eventPosOnPath;
	m_currentOnEventListIdx = pathEventIdx;
	m_onEventListStatus = OnEventListStatus_OnEvent;
}


KY_INLINE void PositionOnLivePath::MoveForwardToNextPathEvent()
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	MoveForwardToNextPathEvent_Unsafe();
}

KY_INLINE void PositionOnLivePath::MoveBackwardToPrevPathEvent()
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtLowerBoundOfLivePath() == true)
		return;
	MoveBackwardToPrevPathEvent_Unsafe();
}

KY_INLINE void PositionOnLivePath::MoveForwardToNextPathEventInterval()
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;
	MoveForwardToNextPathEventInterval_Unsafe();
}


KY_INLINE void PositionOnLivePath::MoveForward_StopAtEventOrAtPathNode(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveForward_StopAtPathNode_Unsafe(distanceMove);
	StopAtEventAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::MoveBackward_StopAtEventOrAtPathNode(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtLowerBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveBackward_StopAtPathNode_Unsafe(distanceMove);
	StopAtEventAfterMovingBackward();
}

KY_INLINE void PositionOnLivePath::MoveForward_StopAtCheckPoint(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;
	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveForward(distanceMove);
	StopAtCheckPointAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::MoveForward(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveForward(distanceMove);
	FixLocationOnEventListAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::MoveBackward(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtLowerBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveBackward(distanceMove);
	FixLocationOnEventListAfterMovingBackward();
}

KY_INLINE void PositionOnLivePath::MoveForwardToNextPathNode()
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveForwardToNextPathNode_Unsafe();
	FixLocationOnEventListAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::MoveBackwardToPrevPathNode()
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtLowerBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveBackwardToPrevPathNode_Unsafe();
	FixLocationOnEventListAfterMovingBackward();
}

KY_INLINE void PositionOnLivePath::MoveForwardToNextPathEdge()
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveForwardToNextPathEdge_Unsafe();
	FixLocationOnEventListAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::MoveBackwardToPrevPathEdge()
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtLowerBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveBackwardToPrevPathEdge_Unsafe();
	FixLocationOnEventListAfterMovingBackward();
}

KY_INLINE void PositionOnLivePath::MoveForward_StopAtPathNode(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtUpperBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveForward_StopAtPathNode_Unsafe(distanceMove);
	FixLocationOnEventListAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::MoveBackward_StopAtPathNode(KyFloat32 distanceMove)
{
	if (IsEventListStatusValid() == false)
		return;
	if (IsAtLowerBoundOfLivePath() == true)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath.MoveBackward_StopAtPathNode_Unsafe(distanceMove);
	FixLocationOnEventListAfterMovingBackward();
}

KY_INLINE KyFloat32 PositionOnLivePath::MoveForwardToClosestPositionOnPath(const Vec3f& position)
{
	if (IsEventListStatusValid() == false)
		return KyFloat32MAXVAL;
	if (IsAtUpperBoundOfLivePath() == true)
		return SquareDistance(m_positionOnPath.GetPosition(), position);

	m_marginModeUsedForValidation = NoMargin;
	const PositionOnPath& upperBound = m_pathEventList->GetLastPathEvent().m_positionOnPath;
	KyFloat32 distanceToPos = m_positionOnPath.MoveForwardToClosestPositionOnPath(position, &upperBound);
	FixLocationOnEventListAfterMovingForward();	
	return distanceToPos;
}

KY_INLINE void PositionOnLivePath::SetNewPositionOnPathForward(const PositionOnPath& newPositionOnPathForward)
{
	if (IsEventListStatusValid() == false)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath = newPositionOnPathForward;
	FixLocationOnEventListAfterMovingForward();
}

KY_INLINE void PositionOnLivePath::SetNewPositionOnPathBackward(const PositionOnPath& newPositionOnPathBackward)
{
	if (IsEventListStatusValid() == false)
		return;

	m_marginModeUsedForValidation = NoMargin;
	m_positionOnPath = newPositionOnPathBackward;
	FixLocationOnEventListAfterMovingBackward();
}

} // namespace Kaim

