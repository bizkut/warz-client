// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_JOINT_CONTROL_UTILITIES_H
#define MR_JOINT_CONTROL_UTILITIES_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief A control method that computes the velocity between the previous and current
/// target joint positions, then applies velocity and acceleration clamping within the
/// specified axis.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
void jointPosVelControlInAxis(
  float               deltaTime,                  ///< IN:  The delta time update step.
  bool                jointPosVelInAxisEnable,    ///< IN:  Flag to enable clamping to a maximum velocity.
  float               jointPosVelMaxInAxis,       ///< IN:  The maximum velocity magnitude in the axis vector.
  bool                jointPosAccelInAxisEnable,  ///< IN:  Flag to enable clamping to a maximum acceleration.
  float               jointPosAccelMaxInAxis,     ///< IN:  The maximum acceleration magnitude in the axis vector.
  const NMP::Vector3& axisV,                      ///< IN:  The axis vector to perform control in.
  const NMP::Vector3& prevTargetJointPos,         ///< IN:  Previous target joint position.
  const NMP::Vector3& prevTargetJointPosVel,      ///< IN:  Previous target joint velocity.
  const NMP::Vector3& curTargetJointPos,          ///< IN:  Current target joint position.
  NMP::Vector3&       curTargetJointPosVel        ///< OUT: Current target joint velocity (Can reference same data as prevTargetJointPosVel).
);

//----------------------------------------------------------------------------------------------------------------------
/// \brief A control method to move a driven joint position towards the desired target position. The control
/// method clamps the velocity and acceleration of the driven joint position to the specified limits. This
/// control is applied only to the component of motion in the specified axis vector. Components of motion not
/// in this axis direction are unchanged by the controller. In addition to clamping to the limits, the control
/// method also detects when overshoot of the target is likely to occur and accordingly decelerates to try and
/// prevent overshoot and possible simple harmonic motion. The motion update is modeled by using a quadratic
/// interpolation method, such that the responses between using two different deltaTime update rates appear
/// almost identical.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
void jointPosControlInAxis(
  float               deltaTime,                  ///< IN:  The delta time update step.
  bool                jointPosVelInAxisEnable,    ///< IN:  Flag to enable clamping to a maximum velocity.
  float               jointPosVelMaxInAxis,       ///< IN:  The maximum velocity magnitude in the axis vector.
  bool                jointPosAccelInAxisEnable,  ///< IN:  Flag to enable clamping to a maximum acceleration.
  float               jointPosAccelMaxInAxis,     ///< IN:  The maximum acceleration magnitude in the axis vector.
  float               weightBetweenPosAndVel,     ///< IN:  Weight factor to influence achieving the target position or velocity. 0 - position, 1 - velocity.
  const NMP::Vector3& axisV,                      ///< IN:  The axis vector to perform control in.
  const NMP::Vector3& prevTargetJointPos,         ///< IN:  Previous target joint position.
  const NMP::Vector3& prevDrivenJointPos,         ///< IN:  Previous driven joint position.
  const NMP::Vector3& prevDrivenJointPosVel,      ///< IN:  Previous driven joint velocity.
  const NMP::Vector3& curTargetJointPos,          ///< IN:  Current target joint position.
  const NMP::Vector3& curTargetJointPosVel,       ///< IN:  Current target joint velocity.
  NMP::Vector3&       curDrivenJointPos,          ///< OUT: Driven joint position (Can reference same data as prevDrivenJointPos).
  NMP::Vector3&       curDrivenJointPosVel        ///< OUT: Driven joint velocity (Can reference same data as prevDrivenJointPosVel).
);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Computes the joint angular velocity between the previous and current orientations.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 jointAngVel(
  float               deltaTime,              ///< IN:  The delta time update step.
  const NMP::Quat&    prevTargetJointAng,     ///< IN:  Previous target joint orientation.
  const NMP::Quat&    curTargetJointAng       ///< IN:  Current target joint orientation.
)
{
  NMP_ASSERT(NMP::nmfabs(1.0f - prevTargetJointAng.magnitude()) < 1.5e-3f);
  NMP_ASSERT(NMP::nmfabs(1.0f - curTargetJointAng.magnitude()) < 1.5e-3f);

  // Compute the shortest path delta quat between the previous and current joint orientations
  NMP::Quat prevAng = prevTargetJointAng;
  float signVal = NMP::floatSelect(prevAng.dot(curTargetJointAng), 1.0f, -1.0f);
  prevAng *= signVal;
  NMP::Quat targetDelta = ~prevAng * curTargetJointAng;

  // Compute the angular velocity
  NMP::Vector3 curTargetJointAngVel = targetDelta.fastLog() * (2.0f / deltaTime);
  return curTargetJointAngVel;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief A control method that computes the angular velocity between the previous and current
/// target joint orientations, then applies velocity and acceleration clamping.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
void jointAngVelControl(
  float               deltaTime,              ///< IN:  The delta time update step.
  bool                jointAngVelEnable,      ///< IN:  Flag to enable clamping to a maximum angular velocity.
  float               jointAngVelMax,         ///< IN:  The maximum angular velocity magnitude.
  bool                jointAngAccelEnable,    ///< IN:  Flag to enable clamping to a maximum angular acceleration.
  float               jointAngAccelMax,       ///< IN:  The maximum angular acceleration magnitude.
  const NMP::Quat&    prevTargetJointAng,     ///< IN:  Previous target joint orientation.
  const NMP::Vector3& prevTargetJointAngVel,  ///< IN:  Previous target joint angular velocity.
  const NMP::Quat&    curTargetJointAng,      ///< IN:  Current target joint orientation.
  NMP::Vector3&       curTargetJointAngVel    ///< OUT: Current target joint angular velocity (Can reference same data as prevTargetJointAngVel).
);

//----------------------------------------------------------------------------------------------------------------------
/// \brief A control method to move a driven joint orientation towards the desired target orientation. The
/// control method clamps the angular velocity and acceleration of the driven joint orientation to the specified
/// limits. In addition to clamping to the limits, the control method also detects when overshoot of the target
/// orientation is likely to occur and accordingly decelerates to try and prevent overshoot and possible simple
/// harmonic motion. The motion update is modeled by using a quadratic interpolation method, such that the
/// responses between using two different deltaTime update rates appear almost identical.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
void jointAngControl(
  float               deltaTime,              ///< IN:  The delta time update step.
  bool                jointAngVelEnable,      ///< IN:  Flag to enable clamping to a maximum angular velocity.
  float               jointAngVelMax,         ///< IN:  The maximum angular velocity magnitude.
  bool                jointAngAccelEnable,    ///< IN:  Flag to enable clamping to a maximum angular acceleration.
  float               jointAngAccelMax,       ///< IN:  The maximum angular acceleration magnitude.
  float               weightBetweenAngAndVel, ///< IN:  Weight factor to influence achieving the target orientation or velocity. 0 - orientation, 1 - velocity.
  const NMP::Quat&    prevTargetJointAng,     ///< IN:  Previous target joint orientation.
  const NMP::Quat&    prevDrivenJointAng,     ///< IN:  Previous driven joint orientation.
  const NMP::Vector3& prevDrivenJointAngVel,  ///< IN:  Previous driven joint angular velocity.
  const NMP::Quat&    curTargetJointAng,      ///< IN:  Current target joint orientation.
  const NMP::Vector3& curTargetJointAngVel,   ///< IN:  Current target joint angular velocity.
  NMP::Quat&          curDrivenJointAng,      ///< OUT: Driven joint orientation (Can reference same data as prevDrivenJointAng).
  NMP::Vector3&       curDrivenJointAngVel    ///< OUT: Driven joint angular velocity (Can reference same data as prevDrivenJointAngVel).
);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_JOINT_CONTROL_UTILITIES_H
//----------------------------------------------------------------------------------------------------------------------
