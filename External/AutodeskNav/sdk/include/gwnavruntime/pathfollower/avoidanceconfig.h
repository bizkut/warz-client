/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_AvoidanceConfig_H
#define Navigation_AvoidanceConfig_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_Math.h"

namespace Kaim
{


/// Set of parameters used to configure AvoidanceComputer.
class AvoidanceConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	AvoidanceConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_enableSlowingDown  = true;
		m_enableStop         = true;

		m_enableForcePassage = true;
		m_waitPassageTimeLimit  = 1.f;
		m_forcePassageTimeLimit = 0.5f;

		m_minimalTimeToCollision = 3.0f;

		m_avoidanceAngleSpan        = 90.0f;
		m_avoidanceSampleCount      = 20;

		m_timeToCollisionInfluence  = 0.85f;
		m_desiredVelocityInfluence  = 0.10f;
		m_previousVelocityInfluence = 0.05f;

		m_safetyDistance = 0.1f;

		m_stopCollisionTime = 0.25f;
		m_stopWaitTime = 0.2f;
	}


	bool operator==(const AvoidanceConfig& other) const
	{
		return m_enableSlowingDown        == other.m_enableSlowingDown
			&& m_enableStop               == other.m_enableStop

			&& m_enableForcePassage       == other.m_enableForcePassage
			&& m_waitPassageTimeLimit     == other.m_waitPassageTimeLimit
			&& m_forcePassageTimeLimit    == other.m_forcePassageTimeLimit

			&& m_avoidanceAngleSpan        == other.m_avoidanceAngleSpan
			&& m_avoidanceSampleCount      == other.m_avoidanceSampleCount
			&& m_timeToCollisionInfluence  == other.m_timeToCollisionInfluence
			&& m_desiredVelocityInfluence  == other.m_desiredVelocityInfluence
			&& m_previousVelocityInfluence == other.m_previousVelocityInfluence
			&& m_safetyDistance            == other.m_safetyDistance

			&& m_stopCollisionTime        == other.m_stopCollisionTime
			&& m_stopWaitTime              == other.m_stopWaitTime

			&& m_minimalTimeToCollision    == other.m_minimalTimeToCollision;
	}

	bool operator!=(const AvoidanceConfig& other) const
	{
		return !operator==(other);
	}

public:
	/// If set to true, velocity candidates with reduced speed will be considered; else only velocity candidates
	/// with Bot::GetDesiredSpeed() length will be considered.
	/// \defaultvalue true
	bool m_enableSlowingDown;

	/// If set to true, null velocity candidate will be considered; else the AvoidanceSolver will
	/// never consider stopping the Bot resulting in sometimes making it force its way through colliders.
	/// \defaultvalue true
	bool m_enableStop;

	/// If set to true, solutions with collisions are allowed if no other solutions are found after a while.
	/// \defaultvalue true
	bool m_enableForcePassage;

	///<  If m_enableForcePassage is true, passage will be forced after m_waitPassageTimeLimit seconds
	/// \defaultvalue 1.f
	KyFloat32 m_waitPassageTimeLimit;

	///<  If m_enableForcePassage is true, passage is forced for at most m_forcePassageTimeLimit seconds only after m_waitPassageTimeLimit seconds delay
	/// \defaultvalue 0.5f
	KyFloat32 m_forcePassageTimeLimit;

	/// If an obstacle may collide another one in m_minimalTimeToCollision seconds, take it into account for the avoidance.
	/// \defaultvalue 3.0f
	KyFloat32 m_minimalTimeToCollision;

	/// Avoidance turning velocities angle span in degrees
	/// \defaultvalue 90.0f;
	KyFloat32 m_avoidanceAngleSpan;

	/// Avoidance turning velocities sample count
	/// \defaultvalue 20
	KyUInt32  m_avoidanceSampleCount;

	/// Influence of the time to collision in choosing the avoidance velocity [0.0f - 1.0f]
	/// \defaultvalue 0.7f
	KyFloat32 m_timeToCollisionInfluence;

	/// Influence of the desired velocity in choosing the avoidance velocity [0.0f - 1.0f]
	/// \defaultvalue 0.2f
	KyFloat32 m_desiredVelocityInfluence;

	/// Influence of the previous velocity in choosing the avoidance velocity [0.0f - 1.0f] 
	/// \defaultvalue 0.1f
	KyFloat32 m_previousVelocityInfluence;

	///  Safety distance around collider to avoid moving too close
	/// \defaultvalue 0.1f
	KyFloat32 m_safetyDistance;

	/// If a collision cannot be avoided in less that stopCollisionTime seconds, the entity will stop   
	/// \defaultvalue 0.5f
	KyFloat32 m_stopCollisionTime;

	/// If the entity stopped, wait stopWaitTime seconds before moving again
	/// \defaultvalue 0.5f
	KyFloat32 m_stopWaitTime;

};

} // namespace Kaim

#endif // Navigation_AvoidanceComputerConfig_H
