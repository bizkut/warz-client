/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: NOBODY

#ifndef Navigation_PathValidityInterval_H
#define Navigation_PathValidityInterval_H

#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/navmesh/traverselogic.h"
#include "gwnavruntime/path/patheventlist.h"
#include "gwnavruntime/path/positiononpath.h"
#include "gwnavruntime/queries/raycastquery.h"
#include "gwnavruntime/queries/trianglefromposquery.h"
#include "gwnavruntime/queries/raycangoonsegmentquery.h"
#include "gwnavruntime/queries/raycastonsegmentquery.h"
#include "gwnavruntime/navgraph/navgraphlink.h"

namespace Kaim
{

class LivePath;

/// Enumerate the different validity status of a LivePath.
enum PathValidityStatus
{
	/// No Path has been set yet.
	PathValidityStatus_NoPath = 0,

	/// The Path has been completely checked and is valid.
	PathIsValid,

	/// NavData have changed around the Path, its validation is about to start.
	ValidityIntervalMustBeRecomputed,

	/// The Path is currently under validation. The check is not finished but no
	/// dirtiness has been found so far.
	ValidityIsBeingChecked,

	/// The validation can not be processed at all. The progress of the bot on path is in an invalid area; it is either:
	/// - outside navMesh,
	/// - on navMesh but on a non-traversable NavTag,
	/// - was on a NavGraph that has been unloaded.
	Dirty_ProgressInvalid,

	/// The Path validity around the progress of the bot on path is limited:
	/// - browsing the path backward on path failed
	/// - forward browsing has not yet reach the Path end.
	Dirty_InvalidBeforeProgress_ProcessingAfter,

	/// The Path validity around the progress of the bot on path is limited:
	/// - browsing the path backward on path failed
	/// - forward browsing completed successfully up to Path end.
	Dirty_InvalidBeforeProgress_ValidAfter,

	/// The Path validity around the progress of the bot on path is limited:
	/// - backward browsing completed successfully up to Path start
	/// - browsing the path forward failed.
	Dirty_ValidBeforeProgress_InvalidAfter,

	/// The Path validity around the progress of the bot on path is limited: both backward and
	/// forward validation browsing failed.
	Dirty_InvalidBeforeAndAfterProgress,

	/// KY_DEPRECATED, prefer using Bot::FlagPathAsPotentiallyDirty() and BotNeedFullComputeFlags::ChannelHasBeenInvalidated()
	Dirty_ChannelInvalid,
};


/// Maintains runtime validity information on a given Path.
class PathValidityInterval
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	PathValidityInterval();
	~PathValidityInterval();
	void Clear();
	void ClearBounds();
	void ClearBoundsAndEventList();

	PathValidityStatus GetPathValidityStatus() const;

	void SetPathValidityStatus(PathValidityStatus status);
	void SetPath(Path* path);

	template<class TraverseLogic>
	KyResult ReStartIntervalComputation(Path* path, void* traverseLogicUserData, const PositionOnPath& targetOnPath,
		KyFloat32 minDistanceFromTargetOnPathBackward, KyFloat32 minDistanceFromTargetOnPathForward);

	enum FirstNewIntervalExtendedStatus
	{
		FirstNewIntervalIsNotAnExtension,
		FirstNewIntervalIsAnExtension
	};

	template<class TraverseLogic>
	KyResult ValidateForward(Path* path, void* traverseLogicUserData,
		KyFloat32 minimumDistanceAdvancement, FirstNewIntervalExtendedStatus* firstIntervalStatus);


private:
	KyResult ValidateTargetOnPath(PositionOnPath& targetOnPath);

	template<class TraverseLogic>
	KyResult ValidateBackWard(Path* path, void* traverseLogicUserData, KyFloat32 minimumdrawBackDistance);

	template<class TraverseLogic>
	KyResult TestCanGoToPrevPathNode(Path* path, void* traverseLogicUserData);
	template<class TraverseLogic>
	KyResult TestCanGoToNextPathNode(Path* path, void* traverseLogicUserData, FirstNewIntervalExtendedStatus* firstNewInterval);

	template<class TraverseLogic>
	void FindLastValidPositionForward(Path* path, void* traverseLogicUserData, FirstNewIntervalExtendedStatus* firstNewInterval);
	template<class TraverseLogic>
	void FindLastValidPositionBackward(Path* path, void* traverseLogicUserData);

	template<class TraverseLogic>
	void ValidateNavTagTransitionsBackward(void* traverseLogicUserData, const LogicDoNotUseCanEnterNavTag& navTagType);
	template<class TraverseLogic>
	void ValidateNavTagTransitionsBackward(void* traverseLogicUserData, const LogicDoUseCanEnterNavTag& navTagType);

	template<class TraverseLogic>
	KyResult ValidateNavTagTransitionForward(void* traverseLogicUserData, KyUInt32 firstNewEvent, const LogicDoNotUseCanEnterNavTag& navTagType);
	template<class TraverseLogic>
	KyResult ValidateNavTagTransitionForward(void* traverseLogicUserData, KyUInt32 firstNewEvent,
		const LogicDoUseCanEnterNavTag& navTagType);


	// No need for traverseLogic since we already check navTags going backwards from the targetOnPath
	KyResult BuildEventListFromLowerBoundToUpperBound(Path* path); 

	void CreateIntervalForNavTagSubSegment(Path* path, KyUInt32 pathEdgeIdxToMoveOn, FirstNewIntervalExtendedStatus* firstNewInterval)
	{
		CreateIntervalsForNavTagSubsegmentButLast(path, pathEdgeIdxToMoveOn, firstNewInterval);
		const PathEventType lastEventType = GetEventTypeForLastNavTagSubSegment(path, pathEdgeIdxToMoveOn);
		CreateIntervalForLastNavTagSubSegment(path, pathEdgeIdxToMoveOn, lastEventType, firstNewInterval);
		m_needToCheckLastEventForRemoval = (lastEventType == PathEventType_OnNavMeshPathNode);
	}
	void CreateIntervalsForNavTagSubsegmentButLast(Path* path, KyUInt32 pathEdgeIdxToMoveOn, FirstNewIntervalExtendedStatus* firstNewInterval);
	void CreateIntervalForLastNavTagSubSegment(Path* path, KyUInt32 pathEdgeIdxToMoveOn, PathEventType typeOfLastEvent, FirstNewIntervalExtendedStatus* firstNewInterval);
	void CheckIfLastEventShouldBeRemoved(const NavTagSubSegment& incomingNavTagSubSegment, FirstNewIntervalExtendedStatus* firstNewInterval);

	PathEventType GetEventTypeForLastNavTagSubSegment(Path* path, KyUInt32 pathEdgeIdxToMoveOn);
	void CreateIntervalForGraphEdge(Path* path, KyUInt32 pathEdgeIdxToMoveOn);
	void CreateIntervalForOutsideAlongGraphEdge(Path* path, KyUInt32 pathEdgeIdxToMoveOn);
	void CreateIntervalForFromOutsidePathEdge(Path* path, KyUInt32 pathEdgeIdxToMoveOn);
	void CreateIntervalForToOutsidePathEdge(Path* path, KyUInt32 pathEdgeIdxToMoveOn);

	PathEventType GetEventTypeFromLowerBound(Path* path, const PositionOnPath& posOnPath);
	void SetPathEventStatusForLastEvent(PathEventStatusInList status);

	void CreateNewPathEvent(Path* path, KyUInt32 pathEdgeIdxToMoveOn, PathEventType pathEventType, const NavTagPtr& navTagptr);
public: // internal
	PathEventList m_pathEventList;

	PositionOnPath m_upperBound;
	PositionOnPath m_lowerBound;

	NavTrianglePtr m_upperTrianglePtr;
	NavTrianglePtr m_lowerTrianglePtr;

	PathValidityStatus m_pathValidityStatus;

	bool m_needToCheckLastEventForRemoval;
	Ptr<QueryDynamicOutput> m_queryDynamicOutput;
};

}

#include "gwnavruntime/path/pathvalidityinterval.inl"

#endif
