/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: MUBI - secondary contact: LAPA
#ifndef Navigation_IAvoidanceComputer_H
#define Navigation_IAvoidanceComputer_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class Bot;


/// This interface class defines the way avoidance is computed.
/// We provide a default AvoidanceComputer (based on Avoidances).
/// You can implement your own IAvoidanceComputer to be used either in
/// our Trajectory class or in your own ITrajectory implementation class.
class IAvoidanceComputer: public RefCountBase<IAvoidanceComputer, MemStat_PathFollowing>
{
	KY_CLASS_WITHOUT_COPY(IAvoidanceComputer)

public:
	/// Enumerates the possible results of a call to IAvoidanceComputer::Compute().
	enum AvoidanceResult
	{
		NoAvoidance = 0,  ///< The trajectory goes in target direction at the desired speed.
		SlowDown,         ///< The trajectory goes in target direction but at lower speed than the desired one.
		Accelerate,       ///< The trajectory goes in target direction but at higher speed than the desired one.
		Turn,             ///< The trajectory deviates from target direction.
		Stop,             ///< The avoidance computer decided that stopping the Bot is the best solution.
		SolutionNotFound, ///< The avoidance computer was not able to find a collision-free solution.
		NoTrajectory
	};


	// ---------------------------------- Public Member Functions ----------------------------------

	IAvoidanceComputer() {}

	virtual ~IAvoidanceComputer() {}

	// ---------------------------------- Pure Virtual Functions ----------------------------------
	virtual AvoidanceResult Compute(Bot& bot, KyFloat32 simulationTimeInSeconds, KyFloat32 desiredSpeed, const Vec2f& movingDirection, Vec3f& avoidanceVelocity) = 0;

	static const char* GetAvoidanceResultString(AvoidanceResult avoidanceResult)
	{
		switch(avoidanceResult)
		{
		case NoAvoidance:
			return "No avoidance";
		case SlowDown:
			return "Slow down";
		case Accelerate:
			return "Accelerate";
		case Turn:
			return "Turn";
		case Stop:
			return "Stop";
		case SolutionNotFound:
			return "Solution not found";
		default:
			return "";
		};
	}

	static const char* GetAvoidanceResultDescription(AvoidanceResult avoidanceResult)
	{
		switch(avoidanceResult)
		{
		case NoAvoidance:
			return "The trajectory goes in target direction at the desired speed";
		case SlowDown:
			return "The trajectory goes in target direction but at lower speed than the desired one";
		case Accelerate:
			return "The trajectory goes in target direction but at higher speed than the desired one";
		case Turn:
			return "The trajectory deviates from target direction";
		case Stop:
			return "The avoidance computer decided that stopping the Bot is the best solution";
		case SolutionNotFound:
			return "The avoidance computer was not able to find any collision-free solutions";
		default:
			return "";
		};
	}

public: // internal
	// Used specifically by the ShortcutTrajectory in order to skip, cf. World::SetNumberOfFramesForShortcutTrajectoryPeriodicUpdate()
	virtual bool IsComputationRequiredThisFrame(Bot* /*bot*/, KyFloat32 /*simulationTimeInSeconds*/) { return true; }
};

} // namespace Kaim

#endif // Navigation_IAvoidanceComputer_H
