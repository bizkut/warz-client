/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

namespace Kaim
{

KY_INLINE PositionOnPath::PositionOnPath() : m_onPathStatus(PositionOnPathStatus_Undefined), m_currentIndexOnPath(KyUInt32MAXVAL), m_mustUpdateDistanceInternalState(true) {}

KY_INLINE bool PositionOnPath::IsAtFirstNodeOfPath() const { return m_onPathStatus == PositionOnPathStatus_OnPathNode && m_currentIndexOnPath == 0; }
KY_INLINE bool PositionOnPath::IsAtLastNodeOfPath()  const { return m_onPathStatus == PositionOnPathStatus_OnPathNode && m_currentIndexOnPath + 1 == m_path->GetNodeCount(); };

KY_INLINE bool PositionOnPath::IsAtFirstEdgeOfPath() const { return m_onPathStatus != PositionOnPathStatus_Undefined && m_currentIndexOnPath == 0; }
KY_INLINE bool PositionOnPath::IsAtLastEdgeOfPath()  const 
{
	return IsAtLastNodeOfPath() ||
		(m_onPathStatus == PositionOnPathStatus_OnPathEdge && m_currentIndexOnPath + 1 == m_path->GetEdgeCount());
};

KY_INLINE KyUInt32 PositionOnPath::GetPathEdgeIndex() const
{
	KY_LOG_ERROR_IF(m_onPathStatus == PositionOnPathStatus_Undefined, ("Error, The OnPathStatus is undefined"));
	if (m_onPathStatus == PositionOnPathStatus_OnPathNode)
		//return m_currentIndexOnPath == 0 ? 0 :m_currentIndexOnPath - 1;
		return Max<KyUInt32>(1, m_currentIndexOnPath) - 1;

	return m_currentIndexOnPath;
}


KY_INLINE KyUInt32 PositionOnPath::GetEdgeIdxToMoveOnForward()  const
{
	KY_LOG_ERROR_IF(m_onPathStatus == PositionOnPathStatus_Undefined, ("Error, The OnPathStatus is undefined"));
	if (m_onPathStatus == PositionOnPathStatus_OnPathNode)
		return Min<KyUInt32>(m_currentIndexOnPath, m_path->GetEdgeCount()-1);
	return m_currentIndexOnPath;
}

KY_INLINE KyUInt32 PositionOnPath::GetEdgeIdxToMoveOnBackward() const
{
	return GetPathEdgeIndex();
}

KY_INLINE Path*                PositionOnPath::GetPath()                 const { return m_path;                                      }
KY_INLINE const Vec3f&         PositionOnPath::GetPosition()             const { return m_position;                                  }
KY_INLINE PositionOnPathStatus PositionOnPath::GetPositionOnPathStatus() const { return m_onPathStatus;                              }
KY_INLINE PathEdgeType         PositionOnPath::GetPathEdgeType()         const { return m_path->GetPathEdgeType(GetPathEdgeIndex()); }

KY_INLINE PathEdgeType  PositionOnPath::GetEdgeTypeToMoveOnForward()   const { return m_path->GetPathEdgeType(GetEdgeIdxToMoveOnForward()); }
KY_INLINE PathEdgeType  PositionOnPath::GetEdgeTypeToMoveOnBackward()   const { return m_path->GetPathEdgeType(GetEdgeIdxToMoveOnBackward()); }

KY_INLINE KyFloat32     PositionOnPath::GetRemainingDistanceToEndOfPath() const { return m_distanceToEndOfPath; }

KY_INLINE bool PositionOnPath::operator!=(const PositionOnPath& other) const { return !(*this == other); }
KY_INLINE bool PositionOnPath::operator==(const PositionOnPath& other) const
{
	return
		m_path               == other.m_path               &&
		m_onPathStatus       == other.m_onPathStatus       &&
		m_currentIndexOnPath == other.m_currentIndexOnPath &&
		m_position           == other.m_position;
}


KY_INLINE void PositionOnPath::InitOnPathFirstNode(Path* path) { InitOnPathNode(path, 0); }
KY_INLINE void PositionOnPath::InitOnPathLastNode(Path* path)
{
	KY_ASSERT(path != KY_NULL);
	InitOnPathNode(path, path->GetNodeCount() - 1);
}

KY_INLINE KyFloat32 PositionOnPath::MoveForward_StopAtPathNode(KyFloat32 distanceMove)
{
	if (IsAtLastNodeOfPath() == false)
		return MoveForward_StopAtPathNode_Unsafe(distanceMove);
	return 0.f;
}

KY_INLINE KyFloat32 PositionOnPath::MoveBackward_StopAtPathNode(KyFloat32 distanceMove)
{
	if (IsAtFirstNodeOfPath() == false)
		return MoveBackward_StopAtPathNode_Unsafe(distanceMove);
	return 0.f;
}

KY_INLINE void PositionOnPath::MoveForwardToNextPathNode()
{
	if (IsAtLastNodeOfPath() == false)
		MoveForwardToNextPathNode_Unsafe();
}

KY_INLINE void PositionOnPath::MoveBackwardToPrevPathNode()
{
	if (IsAtFirstNodeOfPath() == false)
		MoveBackwardToPrevPathNode_Unsafe();
}

KY_INLINE void PositionOnPath::MoveForwardToNextPathEdge()
{
	if (IsAtLastNodeOfPath() == false)
		MoveForwardToNextPathEdge_Unsafe();
}

KY_INLINE void PositionOnPath::MoveBackwardToPrevPathEdge()
{
	if (IsAtFirstNodeOfPath() == false)
		MoveBackwardToPrevPathEdge_Unsafe();
}

}
