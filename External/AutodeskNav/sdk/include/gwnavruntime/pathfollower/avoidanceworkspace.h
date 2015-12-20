/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MUBI - secondary contact: LAPA

#ifndef Navigation_AvoidanceWorkspace_H
#define Navigation_AvoidanceWorkspace_H

#include "gwnavruntime/pathfollower/avoidancecollider.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/pathfollower/avoidanceconfig.h"

namespace Kaim
{
class Bot;

// We want theses arrays to work on contiguous memory, never to release it
typedef KyArrayPOD<AvoidanceCollider, MemStat_PathFollowing, Kaim::ArrayConstPolicy<0, 16, true> > ColliderArray;

enum AvoidanceWorkspaceAddColliderResult
{
	AvoidanceWorkspace_ColliderAdded = 0, ///<
	AvoidanceWorkspace_ColliderIgnored ///<
};


/// AvoidanceWorkspace aggregates at once:
/// - a root collider, that represent the Bot for which a velocity as to be found
/// - the Avoidance for nearby potential collider to be avoidance.
/// It is typically fed by an IAvoidanceComputer instance and then provided
/// to a IAvoidanceSolver to compute the more appropriate velocity.
class AvoidanceWorkspace
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	AvoidanceWorkspace()
		: m_doSendVisualDebugBlob(true)
		, m_visualDebugAltitude(0.f)
	{}

	/// Reset the workspace and prepare it for a given root collider.
	/// visualDebugAltitude is used only in order to VisualDebug the 2d AvoidanceWorkspace at the given altitude, usually the Bot's altitude represented by this rootColider
	void Reset(const AvoidanceCollider& rootCollider, KyFloat32 visualDebugAltitude = 0.f);

	/// Compute the time to collision and filters accordingly:
	/// - if bellow #m_minimalTimeToCollision: insert the collider in the workspace
	///   and return AvoidanceWorkspace_ColliderAdded
	/// - else: ignore the collider and return AvoidanceWorkspace_ColliderIgnored
	/// All colliders are however retained in a ColliderArray, accordingly to the
	/// filtering, an can be retrieved there afterwards.
	AvoidanceWorkspaceAddColliderResult AddCollider(const AvoidanceConfig& config, AvoidanceCollider& collider);

	/// Compute a time to impact.
	KyFloat32 ComputeTimeToCollision(Bot* bot, const Vec2f& candidateVelocity, KyFloat32 safetyDistance, KyInt32& colliderIndex) const;

	// ---------------------------------- Public Data Members ----------------------------------

	/// The AvoidanceCollider corresponding to the Bot for which the
	/// AvoidanceWorkspace is used to compute an avoidance velocity.
	AvoidanceCollider m_rootCollider;

	/// The array of Colliders which Avoidance have been kept in #m_avoidances.
	ColliderArray m_colliders;

	/// The array of discarded Colliders. They are namely used in Integrate()
	/// function to complement #m_avoidances with the ones that should
	/// enter the focus area in next frame.
	ColliderArray m_ignoredColliders;


public: //internal

	// when true and debugging a Bot in VisualDebugLOD_Maximal, activate the sending of
	// a display list representing the avoidance workspace
	bool m_doSendVisualDebugBlob;
	KyFloat32 m_visualDebugAltitude;
};

} // namespace Kaim

#endif // Navigation_AvoidanceWorkspace_H
