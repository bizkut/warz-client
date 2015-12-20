/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE Path*                   LivePath::GetFollowedPath()       const { return m_followedPath;                             }
KY_INLINE PathValidityStatus      LivePath::GetPathValidityStatus() const { return m_validityInterval.GetPathValidityStatus(); }
KY_INLINE const PathEventList&    LivePath::GetPathEventList()      const { return m_validityInterval.m_pathEventList;         }
KY_INLINE PathSource              LivePath::GetFollowedPathSource() const { return m_pathType;                                 }
KY_INLINE ChannelArray*           LivePath::GetChannelArray()       const
{
	Path* path = GetFollowedPath();
	if (path == KY_NULL)
		return KY_NULL;

	return path->m_channelArray;
}

KY_INLINE PathEventList&    LivePath::GetPathEventList()   { return m_validityInterval.m_pathEventList; }

KY_INLINE UpperBoundType    LivePath::GetUpperBoundType() const
{
	KY_ASSERT(GetFollowedPath() != KY_NULL);

	const PathEvent& lastpathEvent = GetPathEventList().GetLastPathEvent();
	return
		(lastpathEvent.m_positionOnPath.IsAtLastNodeOfPath())                      ? PathLastNode :
		(lastpathEvent.m_eventStatusInList == PathEventStatus_TemporaryUpperBound) ? ValidityTemporaryUpperBound :
		/*(lastpathEvent.m_eventStatusInList == PathEventStatus_UpperBound) */       ValidityUpperBound;
}

KY_INLINE bool LivePath::NeedToReplaceCurrentPathWithNewPath() const { return m_newPathSource != PathSource_NoPath;                       }
KY_INLINE bool LivePath::IsComputingNewPath()                  const { return m_pathFinderQueryStatus == PathFinderQueryStatus_Computing; }

KY_INLINE void LivePath::InjectUserPath(Path* newPath)                      { SetNewPath(newPath, PathSource_UserInjected);           }
KY_INLINE void LivePath::InjectPathFromPathFinderQueryOnDone(Path* newPath) { SetNewPath(newPath, PathSource_PathFinderQueryResult);  }
KY_INLINE void LivePath::ForceValidityIntervalRecompute()
{
	m_validityInterval.SetPathValidityStatus(ValidityIntervalMustBeRecomputed);
	m_needToSendLivePathBlob = true;
}
KY_INLINE void LivePath::ClearFollowedPath()
{
	m_validityInterval.Clear();
	m_followedPath = KY_NULL;
	m_pathType = PathSource_NoPath;
	m_needToSendLivePathBlob = true;
}

template<class TraverseLogic>
KY_INLINE KyResult LivePath::ReStartIntervalComputation(void* traverseLogicUserData, const PositionOnPath& positionOnPath,
	KyFloat32 minDistanceFromTargetOnPathBackward, KyFloat32 minDistanceFromTargetOnPathFrowaard)
{
	m_needToSendLivePathBlob = true;
	return m_validityInterval.ReStartIntervalComputation<TraverseLogic>(m_followedPath, traverseLogicUserData, positionOnPath,
		minDistanceFromTargetOnPathBackward, minDistanceFromTargetOnPathFrowaard);
}

template<class TraverseLogic>
KY_INLINE KyResult LivePath::ValidateForward(void* traverseLogicUserData, KyFloat32 minimumDistanceAdvancement,
	PathValidityInterval::FirstNewIntervalExtendedStatus* firstIntervalStatus)
{
	m_needToSendLivePathBlob = true;
	return m_validityInterval.ValidateForward<TraverseLogic>(m_followedPath, traverseLogicUserData, minimumDistanceAdvancement, firstIntervalStatus);
}

}
