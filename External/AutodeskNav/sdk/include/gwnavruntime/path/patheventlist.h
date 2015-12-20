/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: MUBI

#ifndef Navigation_PathEventList_H
#define Navigation_PathEventList_H

#include "gwnavruntime/path/positiononpath.h"
#include "gwnavruntime/containers/bitfield.h"
#include "gwnavruntime/database/navtagptr.h"
#include "gwnavruntime/containers/sharedpoollist.h"


namespace Kaim
{

/// Defines the different kinds of PathEvent.
enum PathEventType
{
	PathEventType_Undefined              , ///< The PathEvent is not yet defined.
	PathEventType_NavGraphVertex         , ///< The PathEvent is on a NavGraph vertex.
	PathEventType_NavTagSwitch           , ///< The PathEvent is at a transition between different NavTags.
	PathEventType_OutsideToNavMeshSwitch , ///< The PathEvent is at a transition between the NavMesh and a PathEdge going from outside to the NavMesh.
	PathEventType_NavMeshToOutsideSwitch , ///< The PathEvent is at a transition between the NavMesh and a PathEdge going from the NavMesh to outside .
	PathEventType_OutsideNavDataPathNode , ///< The PathEvent is outside the NavMesh.

	/// PathValidityInterval bound specific: bound reached start or end of the
	/// path or the max distance of validation, the bound is on the NavMesh
	PathEventType_OnNavMeshPathNode,

	/// PathValidityInterval bound specific: bound hit a border or a non-traversable
	///  navTag boundary during validation (it also include one-way navTag transition).
	PathEventType_OnNavMeshAfterCollision
};

/// Defines the PathEvent status relative to its place in the PathEventList.
enum PathEventStatusInList
{
	PathEventStatus_Undefined,          ///< PathEvent is not yet defined.
	PathEventStatus_WithinBounds,       ///< PathEvent neither lower bound nor upper bound.
	PathEventStatus_LowerBound,         ///< PathEvent is the PathValidityInterval lower bound.
	PathEventStatus_UpperBound,         ///< PathEvent is the PathValidityInterval upper bound.
	PathEventStatus_TemporaryUpperBound ///< PathEvent is the PathValidityInterval temporary upper bound.
};

/// Defines whether the PathEvent is a check point or not. A check point have to
/// be validated to allow path followers to go behind.
enum CheckPointStatus
{
	CheckPointStatus_EventIsNotACheckPoint,
	CheckPointStatus_EventIsACheckPoint
};


/// A PathEvent is a particular PositionOnPath which feature is specified by a
/// #PathEventType.
class PathEvent
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Path)
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PathEvent()
		: m_eventType(PathEventType_Undefined)
		, m_eventStatusInList(PathEventStatus_Undefined)
		, m_checkPointStatus(CheckPointStatus_EventIsNotACheckPoint)
		, m_userSpecific(0)
	{}

	void SetCheckPointStatus(CheckPointStatus checkPointStatus);
	bool IsACheckPoint()  const;
	bool IsOnUpperBound() const;
	bool IsOnLowerBound() const;

	// ---------------------------------- Public Data Members ----------------------------------

	PositionOnPath m_positionOnPath;
	PathEventType m_eventType;
	PathEventStatusInList m_eventStatusInList;
	CheckPointStatus m_checkPointStatus;

	KyUInt32 m_userSpecific;
};


/// This class represents an interval between two PathEvents within a PathEventList.
class PathEventInterval
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Path)
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PathEventInterval() {}


	// ---------------------------------- Public Data Members ----------------------------------

	PathEvent m_endingEventOnPath;
	NavTagPtr m_navTagOnInterval;
};


/// PathEventList aggregates all PathEvents and PathEventIntervals in a
/// PathValidityInterval.
class PathEventList
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)
	KY_CLASS_WITHOUT_COPY(PathEventList)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PathEventList();
	~PathEventList();

	void Clear();
	bool IsBuildCompleted() const;

	KyUInt32 GetPathEventCount()        const;
	KyUInt32 GetPathEventIntervalCount() const;

	PathEvent& GetPathEvent(KyUInt32 eventIdx);
	PathEvent& GetFirstPathEvent();
	PathEvent& GetLastPathEvent();

	const PathEvent& GetPathEvent(KyUInt32 eventIdx) const;
	const PathEvent& GetFirstPathEvent() const;
	const PathEvent& GetLastPathEvent() const;

	/// Return the index of interval ending at the event of index eventIdx.
	/// \pre    eventIdx is different from 0, the index of the first event.
	KyUInt32 GetIntervalIdxBeforeEvent(KyUInt32 eventIdx) const;

	/// Return the index of interval starting at the event of index eventIdx.
	/// \pre    eventIdx is different from GetPathEventIntervalCount() - 1, the index of the last event.
	KyUInt32 GetIntervalIdxAfterEvent(KyUInt32 eventIdx) const;

	/// Returns a pointer to the NavTag lying under interval ending at the event of index eventIdx.
	/// \pre    eventIdx is different from 0, the index of the first event.
	const NavTag* GetNavTagOnIntervalBeforeEvent(KyUInt32 eventIdx) const;

	/// Returns a pointer to the NavTag lying under the interval starting at the event of index eventIdx.
	/// \pre    eventIdx is different from GetPathEventIntervalCount() - 1, the index of the last event.
	const NavTag* GetNavTagOnIntervalAfterEvent(KyUInt32 eventIdx) const;

	KyUInt32 GetStartPathEventIdxOfInterval(KyUInt32 intervalIdx) const;
	KyUInt32 GetEndPathEventIdxOfInterval(KyUInt32 intervalIdx)   const;

	const PathEvent& GetStartPathEventOfInterval(KyUInt32 intervalIdx) const;
	const PathEvent& GetEndPathEventOfInterval(KyUInt32 intervalIdx)   const;

	const NavTag* GetNavTagOfEventInterval(KyUInt32 intervalIdx) const;


	// ---------------------------------- Public Data Members ----------------------------------

	// the first interval is a dummy one used to store the first event.
	// all the other PathEventInterval store the PathEvent at the end of the interval + 
	// a NavTagPtr from which the NavTag along the interval can be retrieved
	KyArray<PathEventInterval, MemStat_Bot> m_pathEventIntervalArray;
};

KY_INLINE void PathEvent::SetCheckPointStatus(CheckPointStatus checkPointStatus) { m_checkPointStatus = checkPointStatus; }
KY_INLINE bool PathEvent::IsACheckPoint() const  { return m_checkPointStatus == CheckPointStatus_EventIsACheckPoint; }
KY_INLINE bool PathEvent::IsOnUpperBound() const { return m_eventStatusInList == PathEventStatus_UpperBound || m_eventStatusInList == PathEventStatus_TemporaryUpperBound; }
KY_INLINE bool PathEvent::IsOnLowerBound() const { return m_eventStatusInList == PathEventStatus_LowerBound; }

KY_INLINE PathEventList::PathEventList() {}
KY_INLINE PathEventList::~PathEventList() { Clear(); }

KY_INLINE void PathEventList::Clear() {  m_pathEventIntervalArray.Clear(); }

KY_INLINE KyUInt32 PathEventList::GetPathEventCount() const { return m_pathEventIntervalArray.GetCount(); }

KY_INLINE KyUInt32 PathEventList::GetPathEventIntervalCount() const
{
	if (m_pathEventIntervalArray.GetCount() != 0)
		return m_pathEventIntervalArray.GetCount() - 1;

	return 0;
}

KY_INLINE KyUInt32 PathEventList::GetIntervalIdxBeforeEvent(KyUInt32 eventIdx) const
{
	KY_LOG_ERROR_IF(eventIdx >= GetPathEventCount(), ("Input index is not a valid event index"));
	KY_LOG_ERROR_IF(eventIdx == 0, ("You cannot retrieve the index of the interval before the first event. It will return unused index"));
	return eventIdx - 1;
}

KY_INLINE KyUInt32 PathEventList::GetIntervalIdxAfterEvent(KyUInt32 eventIdx) const
{
	KY_LOG_ERROR_IF(eventIdx >= GetPathEventCount(), ("Input index is not a valid event index"));
	KY_LOG_ERROR_IF(eventIdx == GetPathEventCount() - 1, ("You cannot retrieve the index of the interval after the last event. It will return unused index"));
	return eventIdx;
}

KY_INLINE const NavTag* PathEventList::GetNavTagOnIntervalBeforeEvent(KyUInt32 eventIdx) const
{
	return GetNavTagOfEventInterval(GetIntervalIdxBeforeEvent(eventIdx));
}
KY_INLINE const NavTag* PathEventList::GetNavTagOnIntervalAfterEvent(KyUInt32 eventIdx) const
{
	return GetNavTagOfEventInterval(GetIntervalIdxAfterEvent(eventIdx));
}

KY_INLINE const NavTag* PathEventList::GetNavTagOfEventInterval(KyUInt32 intervalIdx) const
{
	KY_LOG_ERROR_IF(intervalIdx >= GetPathEventIntervalCount(), ("Input index is not a valid interval index"));
	return m_pathEventIntervalArray[intervalIdx + 1].m_navTagOnInterval.GetNavTag();
}

KY_INLINE KyUInt32 PathEventList::GetStartPathEventIdxOfInterval(KyUInt32 intervalIdx) const
{
	KY_LOG_ERROR_IF(intervalIdx >= GetPathEventIntervalCount(), ("Input index is not a valid interval index"));
	return intervalIdx;
}
KY_INLINE KyUInt32 PathEventList::GetEndPathEventIdxOfInterval(KyUInt32 intervalIdx) const
{
	KY_LOG_ERROR_IF(intervalIdx >= GetPathEventIntervalCount(), ("Input index is not a valid interval index"));
	return intervalIdx + 1;
}

KY_INLINE const PathEvent& PathEventList::GetStartPathEventOfInterval(KyUInt32 intervalIdx) const
{
	KY_DEBUG_ASSERTN(intervalIdx < GetPathEventIntervalCount(), ("Input index is not a valid interval index"));
	return m_pathEventIntervalArray[intervalIdx].m_endingEventOnPath;
}
KY_INLINE const PathEvent& PathEventList::GetEndPathEventOfInterval(KyUInt32 intervalIdx) const
{
	KY_DEBUG_ASSERTN(intervalIdx < GetPathEventIntervalCount(), ("Input index is not a valid interval index"));
	return m_pathEventIntervalArray[intervalIdx + 1].m_endingEventOnPath;
}

KY_INLINE PathEvent& PathEventList::GetPathEvent(KyUInt32 eventIdx)
{
	KY_DEBUG_ASSERTN(eventIdx < GetPathEventCount(), ("Input index is not a valid event index"));
	return m_pathEventIntervalArray[eventIdx].m_endingEventOnPath;
}

KY_INLINE PathEvent& PathEventList::GetFirstPathEvent() { return GetPathEvent(0); }
KY_INLINE PathEvent& PathEventList::GetLastPathEvent() { return GetPathEvent(GetPathEventCount() - 1); }

KY_INLINE const PathEvent& PathEventList::GetPathEvent(KyUInt32 eventIdx) const
{
	KY_DEBUG_ASSERTN(eventIdx < GetPathEventCount(), ("Input index is not a valid event index"));
	return m_pathEventIntervalArray[eventIdx].m_endingEventOnPath;
}

KY_INLINE const PathEvent& PathEventList::GetFirstPathEvent() const { return GetPathEvent(0); }
KY_INLINE const PathEvent& PathEventList::GetLastPathEvent() const { return GetPathEvent(GetPathEventCount() - 1); }

KY_INLINE bool PathEventList::IsBuildCompleted() const
{
	return GetPathEventCount() != 0 && GetLastPathEvent().m_eventStatusInList == PathEventStatus_UpperBound;
}

} // namespace Kaim

#endif // Navigation_PathEventList_H
