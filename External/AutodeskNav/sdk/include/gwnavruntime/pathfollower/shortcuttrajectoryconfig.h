/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TargetOnPathConfig_H
#define Navigation_TargetOnPathConfig_H

#include "gwnavruntime/base/memory.h"

namespace Kaim
{

/// Set of parameters used by the ShortcutTrajectory and TargetOnPathComputer classes to update Bot target on path position.
class ShortcutTrajectoryConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	ShortcutTrajectoryConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_snapOnPathDistance = 0.f; // disabled by default

		m_maxDistanceFromBot = 10.0f;
		m_samplingDistance   =  1.0f;

		m_shortcutAttemptDistanceRatio = 0.7f;
		m_shortcutForceAttemptDistance = 0.5f;

		m_backwardSamplingDistance =  1.0f;
		m_backwardMaxDistance      = 10.0f;
	}

	bool operator==(const ShortcutTrajectoryConfig& other) const
	{
		return m_snapOnPathDistance            == other.m_snapOnPathDistance
			&& m_maxDistanceFromBot            == other.m_maxDistanceFromBot
			&& m_samplingDistance              == other.m_samplingDistance
			&& m_shortcutAttemptDistanceRatio  == other.m_shortcutAttemptDistanceRatio
			&& m_shortcutForceAttemptDistance  == other.m_shortcutForceAttemptDistance
			&& m_backwardSamplingDistance      == other.m_backwardSamplingDistance
			&& m_backwardMaxDistance           == other.m_backwardMaxDistance;
	}

	bool operator!=(const ShortcutTrajectoryConfig& other) const
	{
		return !operator==(other);
	}

	// ---------------------------------- Forward Shortcut Parameters ----------------------------------
	/// These parameters control the way shortcut candidate position are computed.

	/// Setting a value strictly greater than 0.f forces the trajectory not to shortcut when the bot is at a distance 
	//  which is strictly inferior to m_snapOnPathDistance from the path.
	/// In this case, it computes a new position along the Path with no use of any RayCanGo.
	/// However if the Bot cannot be considered on the Path (e.g. due to Avoidance or external game events), normal shortcut applies.
	//  Note that m_snapOnPathDistance must stay a very small value in order to avoid the snapping to be remarkable, for instance 0.05f.
	/// \defaultvalue 0.f
	KyFloat32 m_snapOnPathDistance;

	/// The shortcut candidates are generated up to reach this distance.
	/// If the Bot has moved backward, it may happen the target on path stays farther from the Bot than this distance.
	/// \defaultvalue 10.0f
	KyFloat32 m_maxDistanceFromBot;

	/// The maximal distance between two consecutive target candidate position (Path nodes are always checked).
	/// \defaultvalue 1.0f
	KyFloat32 m_samplingDistance;


	// ---------------------------------- Forward Shortcut Control Parameters ----------------------------------
	/// These parameters control the shortcut attempt adaptive frequency.
	/// When the bot is far from its current target on path, it will try to shortcut
	/// only when it moved enough since last attempt. When close to its target on path
	/// it will attempt every frame.

	/// Triggers new attempt when the bot get closer to TOP than #m_shortcutAttemptDistanceRatio * {distance to TOP at last attempt}.
	/// \defaultvalue 0.7f
	KyFloat32 m_shortcutAttemptDistanceRatio;

	/// Always try to shortcut when the bot is closer to TOP than #m_shortcutForceAttemptDistance.
	/// \defaultvalue 0.5f
	KyFloat32 m_shortcutForceAttemptDistance;


	// ---------------------------------- Backward Sampling Parameters  ----------------------------------
	/// These parameters control the backward search when current target on path is no more
	/// visible. In than case, we sample PositionOnPaths backward from current target on path
	/// until we either find a visible position or reach the previous PathEvent.

	/// The backward sampling precision.
	/// \defaultvalue 1.0f
	KyFloat32 m_backwardSamplingDistance;

	/// The maximal distance along Path on which backward sampling is done.
	/// \defaultvalue 10.0f
	KyFloat32 m_backwardMaxDistance;
};

inline void SwapEndianness(Endianness::Target e, ShortcutTrajectoryConfig& self)
{
	SwapEndianness(e, self.m_maxDistanceFromBot);
	SwapEndianness(e, self.m_samplingDistance);
	SwapEndianness(e, self.m_shortcutAttemptDistanceRatio);
	SwapEndianness(e, self.m_shortcutForceAttemptDistance);
	SwapEndianness(e, self.m_backwardSamplingDistance);
	SwapEndianness(e, self.m_backwardMaxDistance);
}

} // namespace Kaim

#endif
