/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: MUBI - secondary contact: LAPA
#ifndef Navigation_AvoidanceSolver_H
#define Navigation_AvoidanceSolver_H

#include "gwnavruntime/pathfollower/iavoidancesolver.h"
#include "gwnavruntime/pathfollower/avoidanceconfig.h"
#include "gwnavruntime/pathfollower/iavoidancecomputer.h"
#include "gwnavruntime/world/bot.h"
#include "gwnavruntime/channel/raycastinchannel.h"
#include "gwnavruntime/queries/raycastquery.h"
#include "gwnavruntime/queries/diskcollisionquery.h"


namespace Kaim
{

class VelocitySample
{
public:

	VelocitySample()
		: m_normalizedDirection      (0.0f, 0.0f)
		, m_type                     (IAvoidanceComputer::NoAvoidance)
		, m_timeToImpactScore        (0.0f)
		, m_smoothedTimeToImpactScore(0.0f)
		, m_desiredVelocityScore     (0.0f)
		, m_previousVelocityScore    (0.0f)
		, m_collisionTime            (0.0f)
		, m_colliderIndex            (-1)
	{}

	Vec2f                               m_normalizedDirection;
	IAvoidanceComputer::AvoidanceResult m_type;

	KyFloat32 m_timeToImpactScore;
	KyFloat32 m_smoothedTimeToImpactScore;
	KyFloat32 m_desiredVelocityScore;
	KyFloat32 m_previousVelocityScore;
	KyFloat32 m_collisionTime;
	KyInt32   m_colliderIndex;

	KyFloat32 ComputeFinalScore(const AvoidanceConfig& config) const
	{
		return ((m_smoothedTimeToImpactScore * config.m_timeToCollisionInfluence) + 
			    (m_desiredVelocityScore      * config.m_desiredVelocityInfluence) + 
			    (m_previousVelocityScore     * config.m_previousVelocityInfluence));
	}

	void ScoreDesiredVelocity (KyFloat32 score) { m_desiredVelocityScore = score; }
	void ScorePreviousVelocity(KyFloat32 score) { m_previousVelocityScore = score; }

	typedef void(VelocitySample::*ScoreVelocityFunction)(KyFloat32);

	// Debugging functions
	KyFloat32 GetTimeToImpactScore        (const AvoidanceConfig& /*config*/) const { return m_timeToImpactScore; }
	KyFloat32 GetSmoothedTimeToImpactScore(const AvoidanceConfig& /*config*/) const { return m_smoothedTimeToImpactScore; }
	KyFloat32 GetDesiredVelocityScore     (const AvoidanceConfig& /*config*/) const { return m_desiredVelocityScore; }
	KyFloat32 GetPreviousVelocityScore    (const AvoidanceConfig& /*config*/) const { return m_previousVelocityScore; }
	KyFloat32 GetCollisionTime            (const AvoidanceConfig& /*config*/) const { return m_collisionTime; }
	typedef KyFloat32(VelocitySample::*ScoreAccessorFunction)(const AvoidanceConfig&) const;

};


typedef KyArrayPOD<VelocitySample, MemStat_PathFollowing, Kaim::ArrayConstPolicy<0, 16, true> > VelocitySampleArray;

/// AvoidanceSolver computes a velocity compatible with all the colliders in
/// AvoidanceWorkspace and the NavData accordingly to TraverseLogic.
template<class TraverseLogic>
class AvoidanceSolver : public IAvoidanceSolver
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	AvoidanceSolver(void* traverseLogicUserData = KY_NULL)
		: m_traverseLogicUserData(traverseLogicUserData)
	{}

	/// Compute the more appropriate velocity. It stops at the first compatible
	/// one in this order:
	/// -# directly check desiredVelocity
	/// -# try reducing speed in desiredVelocity direction
	/// -# search solutions in other directions.
	/// If no solution is found, outputVelocity is set to null vector.
	virtual IAvoidanceComputer::AvoidanceResult Solve( AvoidanceSolverConfig& config, const Vec3f& previousVelocity, const Vec3f& desiredVelocity, Vec3f& outputVelocity);

	
protected:

	void      ComputeMovingDirection          ( const AvoidanceSolverConfig& config, Vec2f& movingDirection );
	KyFloat32 ComputeTimeHorizon              ( const AvoidanceSolverConfig& config, KyFloat32 movingSpeed );
	void      ComputeTurningVelocitiesSamples ( const AvoidanceSolverConfig& config, const Vec2f& movingDirection, KyFloat32 timeHorizon );
	bool      ScoreForTimeToCollision         ( const AvoidanceSolverConfig& config, KyFloat32 timeHorizon, KyFloat32 movingSpeed );
	void      SmoothTimeToCollisionSamples    (); 
	void      ScoreForDistanceToDirection     ( const Vec2f& direction, VelocitySample::ScoreVelocityFunction scoreFunction ); 
	void      ScoreForPreviousDirection       ( const Vec2f& previousDirection, KyFloat32 previousSpeed ); 
	void      ComputeNoAvoidanceVelocitySample( VelocitySample& noAvoidanceSample, const Vec2f& desiredDirection ); 
	KyUInt32  FindClosestSampleToDirection    ( const Vec2f& direction ); 
	void      SelectBestVelocitySample        ( const AvoidanceSolverConfig& config, VelocitySample& favoriteSample, const Vec2f& previousDirection ); 
	bool      IsDirectionValid                ( AvoidanceSolverConfig& config, const Vec2f& direction, KyFloat32 speed, KyFloat32 time );

	void DebugDrawSample (ScopedDisplayList& displayList_Samples, const VelocitySample& velocitySample, const Vec3f& rootPosition, VelocitySample::ScoreAccessorFunction scoreAccessorFunction, const AvoidanceConfig& config, KyFloat32 movingSpeed);
	void DebugDrawSamples(const char* groupName, const char* displayListName, Kaim::World* world, KyUInt32 visualDebugId, const Vec3f& rootPosition, VelocitySample::ScoreAccessorFunction scoreAccessorFunction, const AvoidanceConfig& config, KyFloat32 movingSpeed);
	void DebugDraw       (AvoidanceSolverConfig& config, const Vec3f& previousVelocity, const Vec3f& desiredVelocity, const Vec3f& outputVelocity, const VelocitySample& noAvoidanceSample, const VelocitySample& favoriteSample, KyFloat32 movingSpeed);

public: // internal
	void* GetTraverseLogicUserData() const { return m_traverseLogicUserData; }
	void SetTraverseLogicUserData(void* traverseLogicUserData) { m_traverseLogicUserData = traverseLogicUserData; }

	void ComputeAllowedNavTags( AvoidanceSolverConfig& config );

public: // internal
	void*                           m_traverseLogicUserData;
	VelocitySampleArray             m_velocitySamples;
	LocalCostAwareTraverseLogicData m_localCostAwareTraverseLogicData;
};

} // namespace Kaim

#include "gwnavruntime/pathfollower/avoidancesolver.inl"

#endif // Navigation_AvoidanceSolver_H
