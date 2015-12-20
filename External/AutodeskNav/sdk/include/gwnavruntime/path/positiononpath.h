/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: JUBA

#ifndef Navigation_PositionOnPath_H
#define Navigation_PositionOnPath_H

#include "gwnavruntime/path/path.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

///< Enumerates the different status an objectPositionOnPath can have on its Path.
enum PositionOnPathStatus
{
	PositionOnPathStatus_Undefined = 0, ///< The PositionOnPath is not yet defined.
	PositionOnPathStatus_OnPathNode,    ///< The PositionOnPath is on a Path node.
	PositionOnPathStatus_OnPathEdge     ///< The PositionOnPath is on a Path edge.
};


/// This class aggregates all necessary information about a position on a Path,
/// namely:
/// - a Vec3f position,
/// - a #PositionOnPathStatus, and
/// - a Path edge or node index.
class PositionOnPath
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Path)

public:
	// ---------------------------------- Main API Functions ----------------------------------

	PositionOnPath();

	bool operator==(const PositionOnPath& positionOnPath) const;
	bool operator!=(const PositionOnPath& positionOnPath) const;

	void InitOnPathNode     (Path* path, KyUInt32 pathNodeIdx);
	void InitOnPathFirstNode(Path* path);
	void InitOnPathLastNode (Path* path);
	void InitOnPathEdge     (Path* path, const Vec3f& positionOnEdge, KyUInt32 pathEdgeIdx);
	void Clear();

	bool                 IsAtFirstNodeOfPath()     const;
	bool                 IsAtLastNodeOfPath()      const;
	bool                 IsAtFirstEdgeOfPath()     const;
	bool                 IsAtLastEdgeOfPath()      const;
	Path*                GetPath()                 const;
	const Vec3f&         GetPosition()             const;
	PositionOnPathStatus GetPositionOnPathStatus() const;
	KyFloat32            GetRemainingDistanceToEndOfPath() const;
	
	/// This function has to be used carefully, cf. GetPathEdgeIndex()
	PathEdgeType         GetPathEdgeType()         const;

	/// return the type of the edge given by GetEdgeIdxToMoveOnForward()
	PathEdgeType         GetEdgeTypeToMoveOnForward()         const;

	/// return the type of the edge given by GetEdgeIdxToMoveOnBackward()
	PathEdgeType         GetEdgeTypeToMoveOnBackward()         const;

	/// Returns true when this PositionOnPath precede or is at same position
	/// that the other one.
	bool DoesPrecede(const PositionOnPath& other) const;

	/// Returns true when this PositionOnPath strictly precede the other one;
	/// returns false if it as as the same position or after.
	bool IsStrictlyBefore(const PositionOnPath& other) const;


	// ---------------------------------- Path Edge Functions ----------------------------------

	/// This function has to be used carefully:
	/// - if this instance is at the first Path node, it returns the first edge index (0),
	/// - if it is on another Path node, it returns the index of the edge ending at this Path node,
	/// - else it returns the index of the edge current instance lies on.
	/// \pre GetPositionOnPathStatus() != PositionOnPathStatus_Undefined
	KyUInt32 GetPathEdgeIndex() const;

	/// If this in on a Path node, it returns the index of the edge starting at this Path node,
	/// else it returns the index of the edge current instance lies on.
	/// \pre GetPositionOnPathStatus() != PositionOnPathStatus_Undefined
	/// \pre Current instance must not be at the last node of the Path.
	KyUInt32 GetEdgeIdxToMoveOnForward()  const;

	/// If this in on a Path node, it returns the index of the edge ending at this Path node,
	/// else it returns the index of the edge current instance lies on.
	/// \pre GetPositionOnPathStatus() != PositionOnPathStatus_Undefined
	/// \pre Current instance must not be at the first node of the Path.
	KyUInt32 GetEdgeIdxToMoveOnBackward() const;


	// ---------------------------------- Move Functions ----------------------------------

	/// Main distance-based forward move method: moves the position along distanceMove
	/// meters measured on the broken-line.
	/// \pre distanceMove >= 0.0f
	void MoveForward(KyFloat32 distanceMove);

	/// Main distance-based backward move method: moves the position along distanceMove
	/// meters measured on the broken-line.
	/// \pre distanceMove >= 0.0f
	void MoveBackward(KyFloat32 distanceMove);

	/// Atomic move method: moves forward the position on the current Path edge it lays on,
	/// up to distanceMove meters.
	/// If the next Path node is at a distance lower than distanceMove this instance position
	/// is set at this Path node, and the distance from initial position to this Path node is returned;
	/// else the position is moved for distanceMove meters on the edge it lays on, and distanceMove is returned.
	/// \pre distanceMove >= 0.0f
	KyFloat32 MoveForward_StopAtPathNode(KyFloat32 distanceMove);

	/// Atomic move method: moves backward the position on the current Path edge it lays on,
	/// up to distanceMove meters.
	/// If the previous Path node is at a distance lower than distanceMove this instance position
	/// is set at this Path node, and the distance from initial position to this Path node is returned;
	/// else the position is moved for distanceMove meters on the edge it lays on, and distanceMove is returned.
	/// \pre distanceMove >= 0.0f
	KyFloat32 MoveBackward_StopAtPathNode(KyFloat32 distanceMove);

	/// Atomic forward move method: must be used with lot of care.
	/// Consider using MoveForward method instead, unless you really need to finely control
	/// the PositionOnPath move.
	void MoveForwardToNextPathNode();

	/// Atomic backward move method: must be used with lot of care.
	/// Consider using MoveBackward method instead, unless you really need to finely control
	/// the PositionOnPath move.
	void MoveBackwardToPrevPathNode();

	/// Atomic forward move method: must be used with lot of care.
	/// Consider using MoveForward method instead, unless you really need to finely control
	/// the PositionOnPath move.
	/// \pre GetPositionOnPathStatus() != PositionOnPathStatus_Undefined
	/// \pre Current instance must not be at the last node or edge of the Path.
	void MoveForwardToNextPathEdge();

	/// Atomic forward move method: must be used with lot of care.
	/// Consider using MoveForward method instead, unless you really need to finely control
	/// the PositionOnPath move.
	/// \pre GetPositionOnPathStatus() != PositionOnPathStatus_Undefined
	/// \pre Current instance must not be at the first node or edge of the Path.
	void MoveBackwardToPrevPathEdge();


	/// Return the distance along the Path from/to the other position.
	/// KyFloat32MAXVAL if the otherPositionOnPath is on a different Path.
	KyFloat32 ComputeDistance(const PositionOnPath& otherPositionOnPath) const;


	/// Move forward to the closest position on path from originalPosition.
	/// Returns the square distance between originalPosition and the positionOnPath
	KyFloat32 MoveForwardToClosestPositionOnPath(const Vec3f& originalPosition, const PositionOnPath* upToPosOnPath = KY_NULL);

	// ---------------------------------- Unsafe Version of Functions ----------------------------------
	// These functions are unsafe in regard to the pre-conditions mentioned in the functions of similar name without the _Unsafe prefix.

	// cf. MoveForward_StopAtPathNode
	KyFloat32 MoveForward_StopAtPathNode_Unsafe(KyFloat32 distanceMove);
	// cf. MoveBackward_StopAtPathNode
	KyFloat32 MoveBackward_StopAtPathNode_Unsafe(KyFloat32 distanceMove);
	// cf. MoveForwardToNextPathNode
	void MoveForwardToNextPathNode_Unsafe();
	// cf. MoveBackwardToPrevPathNode
	void MoveBackwardToPrevPathNode_Unsafe();
	// cf. MoveForwardToNextPathEdge
	void MoveForwardToNextPathEdge_Unsafe();
	// cf. MoveBackwardToPrevPathEdge
	void MoveBackwardToPrevPathEdge_Unsafe();

private:
	KyFloat32 ComputeDistanceToEndOfPathFromPathEdge(KyUInt32 pathEdgeIdx, const Vec3f& positionOnEdge) const;
	KyFloat32 ComputeDistanceToEndOfPathFromPathNode(KyUInt32 pathNodeIdx) const;
	KyFloat32 ComputeDistance(KyUInt32 fromPathNodeIdx, KyUInt32 toPathNodeIdx) const;

	void ClampOnCurrentPathEdge(const Kaim::Vec3f& originalPosition);

public: // internal
	void GetRawValues(Path*& path, Vec3f& position, PositionOnPathStatus& onPathStatus, KyUInt32& currentIndexOnPath, KyFloat32& distanceToEndOfPath) const
	{
		path = m_path;
		position = m_position;
		onPathStatus = m_onPathStatus;
		currentIndexOnPath = m_currentIndexOnPath;
		distanceToEndOfPath = m_distanceToEndOfPath;
	}
	void SetRawValues(Path* path, const Vec3f& position, PositionOnPathStatus onPathStatus, KyUInt32 currentIndexOnPath, KyFloat32 distanceToEndOfPath)
	{
		m_path = path;
		m_position = position;
		m_onPathStatus = onPathStatus;
		m_currentIndexOnPath = currentIndexOnPath;
		m_distanceToEndOfPath = distanceToEndOfPath;
	}

private:
	Ptr<Path> m_path;
	Vec3f m_position;

	PositionOnPathStatus m_onPathStatus;
	KyUInt32 m_currentIndexOnPath; // index on the Path edge or index of the Path node. position is on ]EdgeStart, EdgeEnd]
	
	KyFloat32 m_distanceToEndOfPath; // Maintained informatively, ie.e it is not used to differentiate two PositionOnPath instances.
	bool m_mustUpdateDistanceInternalState; // Internal state used to prevent some useless computations of m_distanceToEndOfPath in some functions.
};

} // namespace Kaim


#include "gwnavruntime/path/positiononpath.inl"

#endif // Navigation_PositionOnPath_H
