/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: JUBA

#ifndef Navigation_PositionOnLivePath_H
#define Navigation_PositionOnLivePath_H

#include "gwnavruntime/path/positiononpath.h"
#include "gwnavruntime/path/patheventlist.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/queries/raycangoquery.h"


namespace Kaim
{

class Bot;

/// Enumerates the admissible values for PositionOnLivePath status accordingly to its position relatively to PathEvents.
enum OnEventListStatus
{
	OnEventListStatus_Undefined = 0, ///< The PositionOnLivePath relative position in PathEventList is not yet defined.
	OnEventListStatus_OnEvent,       ///< The PositionOnLivePath is on a PathEvent.
	OnEventListStatus_OnInterval     ///< The PositionOnLivePath is strictly on a PathEventInterval.
};


/// This class aggregates all necessary information to describe a position on a
/// LivePath, namely:
/// - a PositionOnPath and
/// - PathEventList related information.
///
/// Be careful with instances of this class: they are intended to be kept alive
/// but, up to now, only the instance in Bot is maintained accordingly to LivePath updates. 
/// As a consequence, all the other instances can not be used through several frames.
///
/// You are not intended to create or manage instances of this class. Nevertheless,
/// it is fully documented since you will probably need to read and interpret it.
class PositionOnLivePath
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Path)

public:
	// ---------------------------------- Main API Functions ----------------------------------

	PositionOnLivePath();
	void BindToBot(Bot* bot);
	Bot* GetBot() const { return m_bot; }
	void Clear();
	void ClearEventListLocation();


	// ---------------------------------- Comparison Operators ----------------------------------

	bool operator==(const PositionOnLivePath& positionOnLivePath) const;
	bool operator!=(const PositionOnLivePath& positionOnLivePath) const;


	// ---------------------------------- Raw Path Related Functions ----------------------------------

	Path*                 GetPath();
	const Path*           GetPath()                  const;
	const Vec3f&          GetPosition()              const;
	const PositionOnPath& GetPositionOnPath()        const;
	PositionOnPathStatus  GetPositionOnPathStatus()  const;
	PathEdgeType          GetPathEdgeType()          const;

	bool IsAtFirstNodeOfPath() const;
	bool IsAtLastNodeOfPath()  const;
	
	bool IsAtFirstEdgeOfPath() const;
	bool IsAtLastEdgeOfPath()  const;

	/// If this is at a Path node, it returns the index of the edge ending at this Path node,
	/// except if it is at the first Path node, in which case it returns the first edge index (0).
	/// If this is on a Path edge it returns the index of the edge it lies on.
	KyUInt32 GetPathEdgeIndex() const;

	/// If this in on a Path node, it returns the index of the edge starting at this path node,
	/// else it returns the index of the edge it lies on.
	/// \pre Current instance must not be at the last node of the path.
	KyUInt32 GetEdgeIdxToMoveOnForward()  const;

	/// return the type of the edge given by GetEdgeIdxToMoveOnForward()
	PathEdgeType GetEdgeTypeToMoveOnForward()  const;

	/// return the type of the edge given by GetEdgeIdxToMoveOnBackward()
	PathEdgeType GetEdgeTypeToMoveOnBackward() const;

	/// If this in on a Path node, it returns the index of the edge ending at this Path node,
	/// else it returns the index of the edge it lies on.
	/// \pre Current instance must not be at the first node of the path.
	KyUInt32 GetEdgeIdxToMoveOnBackward() const;

	/// Returns true when this PositionOnLivePath precede or is at same position on the Path that the other one.
	/// It just test the PositionOnPath, it does not consider the location on the event list.
	bool DoesPrecede(const PositionOnLivePath& other) const;

	/// Returns true when this PositionOnLivePath precede than the other one; returns false if it as as the same pos.
	/// It just test the PositionOnPath, it does not consider the location on the event list.
	bool IsStrictlyBefore(const PositionOnLivePath& other) const;

	// ---------------------------------- PositionOnLivePath initialization on Path----------------------------------
	/// This 4 functions init the positionOnPath, but they also clear all the data concerning
	/// the location on the PathEventList. You can then call FindLocationOnEventListFromScratch()
	/// to have this object on a valid state.

	void InitOnPathFirstNode(Path* path);
	void InitOnPathLastNode(Path* path);
	void InitOnPathNode(Path* path, KyUInt32 pathNodeIdx);
	void InitOnPathEdge(Path* path, const Vec3f& positionOnEdge, KyUInt32 pathEdgeIdx);
	void InitOnPositionOnPath(const PositionOnPath& positionOnPath);


	// ---------------------------------- PathEventList Related Functions ----------------------------------

	OnEventListStatus GetOnEventListStatus() const;

	PathEvent GetPathEvent(KyUInt32 pathEventIdx) const;

	/// This function has to be used carefully:
	/// - if the GetOnEventListStatus() == OnEvent, it returns the index of the PathEvent in the PathEventList,
	/// - if the GetOnEventListStatus() == OnEventInterval, it returns the index of the PathEventInterval in the PathEventList.
	KyUInt32 GetOnEventListIndex() const;

	/// This function has to be used carefully:
	/// - if this in on an PathEvent different from the UpperBound, it returns the index of the next PathEvent,
	/// - if this in on an EventInterval, it returns the index of the ending PathEvent.
	/// Can return KyUInt32MAXVAL if current instance is on the Upper bound or if it is not located on the PathEventList.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	KyUInt32 GetNextPathEventIdx() const;

	/// This function has to be used carefully:
	/// - if this in on an PathEvent different from the LowerBound, it returns the index of the previous PathEvent,
	/// - if this in on an EventInterval, it returns the index of the starting PathEvent.
	/// Can return KyUInt32MAXVAL if current instance is on the Upper bound or if it is not located on the PathEventList.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	KyUInt32 GetPrevPathEventIdx() const;

	bool IsAtLowerBoundOfLivePath() const;
	bool IsAtUpperBoundOfLivePath() const;

	/// Indicates if the PositionOnLIvePath is at an event that is a check point
	bool IsAtACheckPoint() const;

	/// Returns a pointer to the navTag along the path after the position on path.
	/// If target on path is on a PathEvent, it returns the NavTag between this
	/// PathEvent and the next PathEvent. If it lies strictly within two PathEvents,
	/// it returns the NavTag along this PathEventInterval
	/// Note: if position on path is at the upper bound of the validity interval,
	/// it will return KY_NULL.
	const NavTag* GetUpcomingEventNavTag() const;
	
	/// Returns a pointer to the navTag along the path before the position on path.
	/// If position on path is on a PathEvent, it returns the NavTag between this
	/// PathEvent and the previous PathEvent. If it lies strictly within two PathEvents,
	/// it returns the NavTag along this PathEventInterval
	/// Note: if position on path is at the lower bound of the validity interval,
	/// it will return KY_NULL.
	const NavTag* GetPreviousEventNavTag() const;

	/// This function computes the location on the PathEventList from the PositionOnPath.
	void FindLocationOnEventListFromScratch();

	// ---------------------------------- Move Functions ----------------------------------

	/// This function moves forward the position along distanceMove meters measured on the broken-line.
	/// If the UpperBound of the PathValidityInterval is reached, this instance is stopped at this upper bound.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForward(KyFloat32 distanceMove);

	/// This function moves backward the position along distanceMove meters measured on the broken-line.
	/// If the LowerBound of the PathValidityInterval is reached, this instance is stopped at this lower bound.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackward(KyFloat32 distanceMove);

	/// This function moves forward the position along distanceMove meters measured on the broken-line.
	/// If a Path node is reached before covering the distanceMove, this instance is stopped at this Path node.
	/// If the UpperBound of the PathValidityInterval is reached, this instance is stopped at this upper bound.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForward_StopAtPathNode(KyFloat32 distanceMove);

	/// This function moves backward the position along distanceMove meters measured on the broken-line.
	/// If a Path node is reached before covering the distanceMove, this instance is stopped at this Path node.
	/// If the LowerBound of the PathValidityInterval is reached, this instance is stopped at this lower bound.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackward_StopAtPathNode(KyFloat32 distanceMove);

	/// This function moves forward the position along distanceMove meters measured on the broken-line.
	/// If a PathEvent is reached before covering the distanceMove, this instance is stopped at this path event.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForward_StopAtEvent(KyFloat32 distanceMove);

	/// This function moves backward the position along distanceMove meters measured on the broken-line.
	/// If a PathEvent is reached before covering the distanceMove, this instance is stopped at this path event.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackward_StopAtEvent(KyFloat32 distanceMove);

	/// This function moves forward the position along distanceMove meters measured on the broken-line.
	/// If a PathEvent or a Path node is reached before covering the distanceMove, this instance is stopped
	/// at this PathEvent or Path node.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForward_StopAtEventOrAtPathNode(KyFloat32 distanceMove);

	/// This function moves backward the position along distanceMove meters measured on the broken-line.
	/// If a PathEvent or a Path node is reached before covering the distanceMove, this instance is stopped
	/// at this PathEvent or Path node.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackward_StopAtEventOrAtPathNode(KyFloat32 distanceMove);

	/// This function moves forward the position along distanceMove meters measured on the broken-line.
	/// If a PathEvent which is a checkpoint is reached before covering the distanceMove, this instance is stopped at this path event.
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForward_StopAtCheckPoint(KyFloat32 distanceMove);

	/// This function moves forward the position up to the next Path node.
	/// \pre This current instance must not be located on the UpperBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForwardToNextPathNode();

	/// This function moves backward the position up to the previous Path node.
	/// \pre This current instance must not be located on the LowerBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackwardToPrevPathNode();

	/// This function moves the position to the specified path event.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveToPathEvent(KyUInt32 pathEventIdx);

	/// This function moves forward the position up to the next path event.
	/// \pre This current instance must not be located on the UpperBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForwardToNextPathEvent();

	/// This function moves backward the position up to the previous path event.
	/// \pre This current instance must not be located on the LowerBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackwardToPrevPathEvent();

	/// This function moves forward the position up to the start of the next path edge.
	/// \pre This current instance must not be located on the UpperBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForwardToNextPathEdge();

	/// This function moves forward the position up to the start of the next path edge.
	/// \pre This current instance must not be located on the UpperBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackwardToPrevPathEdge();

	/// This function moves forward the position up to the next path event interval.
	/// \pre This current instance must not be located on the UpperBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForwardToNextPathEventInterval();

	/// This function moves forward the position up to the next path event that is a check point.
	/// \pre This current instance must not be located on the UpperBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveForwardToNextCheckPoint();

	/// This function moves backward the position up to the previous path event that is a check point.
	/// \pre This current instance must not be located on the LowerBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	void MoveBackwardToPrevCheckPoint();

	/// Move forward to the closest position on path from originalPosition.
	/// Returns the distance between position and the positionOnPath found.
	/// \pre This current instance must not be located on the UpperBound.
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	KyFloat32 MoveForwardToClosestPositionOnPath(const Vec3f& position);

	/// Moves this PositionOnLivePath forward to the provided PositionOnPath
	/// and updates PathEventList related informations.
	/// \pre This current instance must be located before newPositionOnPathForward.
	void SetNewPositionOnPathForward(const PositionOnPath& newPositionOnPathForward);

	/// Moves this PositionOnLivePath backward to the provided PositionOnPath
	/// and updates PathEventList related informations.
	/// \pre This current instance must be located after newPositionOnPathBackward.
	void SetNewPositionOnPathBackward(const PositionOnPath& newPositionOnPathBackward);

	// ---------------------------------- Move Functions with Cost ----------------------------------

	/// This function moves forward the position along distanceMove meters measured on the broken-line.
	/// If a PathEvent or a Path node is reached before covering the distanceMove, this instance is stopped
	/// at this PathEvent or Path node. It compute the cost that corresponds to that move according to the
	/// the NavTag and the cost multipliers of the TraverseLogic
	/// \pre distanceMove >= 0.0f
	/// \pre GetOnEventListStatus() != OnEventListStatus_Undefined
	template<class TraverseLogic>
	void MoveForwardAndGetCost_StopAtEventOrAtPathNode(void* traverseLogicUserData, KyFloat32 distanceMove, RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost);

	
	// ---------------------------------- Unsafe Version of Functions ----------------------------------
	// These functions are unsafe in regard to the pre-conditions mentioned in the functions of similar name without the _Unsafe prefix.

	// cf. GetNextPathEventIdx()
	KyUInt32 GetNextPathEventIdx_Unsafe() const;
	// cf. GetPrevPathEventIdx()
	KyUInt32 GetPrevPathEventIdx_Unsafe() const;
	// cf. MoveForwardToNextPathEvent()
	void MoveForwardToNextPathEvent_Unsafe();
	// cf. MoveBackwardToPrevPathEvent()
	void MoveBackwardToPrevPathEvent_Unsafe();
	// cf. MoveForwardToNextPathEventInterval()
	void MoveForwardToNextPathEventInterval_Unsafe();

private:
	bool IsEventListStatusValid() const;

	void FixLocationOnEventListAfterMovingForward();
	void FixLocationOnEventListAfterMovingBackward();
	void StopAtEventAfterMovingForward();
	void StopAtEventAfterMovingBackward();
	void StopAtCheckPointAfterMovingForward();

	template<class TraverseLogic>
	void GetCostAfterStopAtEventOrAtPathNode(void* traverseLogicUserData, const Vec3f& posBeforeMove, const NavTag* navTag,
		RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost, const LogicWithoutCostMultipler&);
	template<class TraverseLogic>
	void GetCostAfterStopAtEventOrAtPathNode(void* traverseLogicUserData, const Vec3f& posBeforeMove, const NavTag* navTag,
		RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost, const LogicWithCostMultiplerPerNavTag&);
	template<class TraverseLogic>
	void GetCostAfterStopAtEventOrAtPathNode(void* traverseLogicUserData, const Vec3f& posBeforeMove, const NavTag* navTag,
		RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost, const LogicWithCostMultiplerPerTriangle&);

public: // internal
	void SetMarginModeUsedForValidation(RayCanGoMarginMode marginModeUsedForValidation) { m_marginModeUsedForValidation = marginModeUsedForValidation; }
	RayCanGoMarginMode GetMarginModeUsedForValidation() const { return m_marginModeUsedForValidation; }
	KyFloat32 GetMarginFloatValueUsedForValidation() const;

private:
	Bot* m_bot;
	PathEventList* m_pathEventList; // We could access this through bot, but it is an optimization
	PositionOnPath m_positionOnPath;
	OnEventListStatus m_onEventListStatus;
	KyUInt32 m_currentOnEventListIdx; // index on the event or index of the interval. position is on ]EventIntervalStart, EventIntervalEnd]
	RayCanGoMarginMode m_marginModeUsedForValidation;
};

} // namespace Kaim

#include "gwnavruntime/path/positiononlivepath.inl"

#endif // Navigation_PositionOnLivePath_H
