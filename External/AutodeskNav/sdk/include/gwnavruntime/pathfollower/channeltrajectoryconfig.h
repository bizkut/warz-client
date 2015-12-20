/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: MUBI
#ifndef Navigation_ChannelTrajectoryConfig_H
#define Navigation_ChannelTrajectoryConfig_H

#include "gwnavruntime/pathfollower/radiusprofile.h"
#include "gwnavruntime/pathfollower/splineconfig.h"


namespace Kaim
{

class ChannelTrajectoryConfigBlob;


/// Class that configures how the trajectory is computed from the Channel.
class ChannelTrajectoryConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:

	/// Enumerates the SplineTrajectory velocity computation modes.
	enum VelocityComputationMode
	{
		/// This mode is designed to let a bot without any inertia slide as close
		/// as possible along spline.
		VelocityComputationMode_MatchSpline = 0,

		/// In this mode, we use m_targetOnSplineDistance to anticipate ahead. This
		/// mode is friendly with animation driven systems.
		VelocityComputationMode_AnimationDrivenLocomotion
	};

	ChannelTrajectoryConfig() { SetDefaults(); }

	void SetDefaults();
	void UpdateFromBlob(const ChannelTrajectoryConfigBlob& blob);
	bool operator==(const ChannelTrajectoryConfig& other) const;
	bool operator!=(const ChannelTrajectoryConfig& other) const { return !operator==(other); }

public:

	//-------------------- Spline computation parameters --------------------

	SplineConfig m_splineConfig;
	RadiusProfileArray m_radiusProfileArray;

	/// Once the bot has moved of m_recomputationDistance meters along the spline,
	/// the spline will be recomputed.
	/// \defaultvalue 1.0f
	KyFloat32 m_recomputationDistance;

	/// When updating a followed spline, we ensure the spline will not change on
	/// m_stabilityDistance meters.
	/// \defaultvalue 5.0f
	KyFloat32 m_stabilityDistance;


	//-------------------- Spline follow parameters --------------------

	/// The target is a point along the spline that is used to compute the final
	/// velocity. m_targetOnSplineDistance parameter controls the distance this
	/// target is from the bot position on spline, when in
	/// VelocityComputationMode_AnimationDrivenLocomotion mode. In
	/// VelocityComputationMode_MatchSpline mode, the target position is as close
	/// as possible of the bot and its distance is thus not controllable.
	/// \defaultvalue 0.6f
	KyFloat32 m_targetOnSplineDistance;

	/// If the bot is more than m_maxDistanceToSplinePosition meters from its
	/// position on the spline, then the spline will be recomputed.
	/// \defaultvalue 0.3f
	KyFloat32 m_maxDistanceToSplinePosition;

	/// Choose whether you want to directly read your velocity from the spline
	/// or if you are animation driven instead.
	/// \defaultvalue VelocityComputationMode_MatchSpline
	VelocityComputationMode m_velocityComputationMode;
};

} // namespace Kaim

#endif // Navigation_ChannelTrajectoryConfig_H
