/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_ResetTrajectoryStatus_H
#define Navigation_ResetTrajectoryStatus_H

namespace Kaim
{

// Values above ResetTrajectoryStatus_Success allows the Trajectory to be reset.
enum ResetTrajectoryStatus
{
	ResetTrajectoryStatus_InInvalidNavData,       ///< PositionOnPath is in Invalid NavData or no Path is associated
	ResetTrajectoryStatus_OutsideNavMesh,         ///< Bot is outside NavMesh whereas PositionOnPath is on NavMesh
	ResetTrajectoryStatus_OutsideChannel,         ///< Bot is NavMesh whereas PositionOnPath is on a portion of the Path that is associated to a channel, and channel cannot be reached
	ResetTrajectoryStatus_CannotShortcutToPath,   ///< Bot is in NavMesh but it cannot reach the Path in straight line
	ResetTrajectoryStatus_Pending,                ///< Typically, occurs when trying to ExitManualControl whereas the exit status is still pending
	ResetTrajectoryStatus_Success,                ///< PathEdgeType to move on forward from the given PositionOnPath is inside NavMesh and Bot is in side NavMesh as well
	                                              ///< if Path has a Channel, bot is also in the channel
	ResetTrajectoryStatus_Success_OutsideNavMesh, ///< PathEdgeType to move on forward from the given PositionOnPath is outside NavMesh and Bot is outside NavMesh as well
	ResetTrajectoryStatus_Success_OutsideChannel, ///< Bot in NavMesh but not in channel whereas PositionOnPath is on a portion of the Path that is associated to a channel
	                                              ///< but it is possible to join the channel in straight line (the ShortcutTrajectory will be used until the channel is reached)
};

}

#endif
