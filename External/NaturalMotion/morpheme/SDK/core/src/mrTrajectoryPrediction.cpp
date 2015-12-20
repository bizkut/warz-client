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
#include "morpheme/mrTrajectoryPrediction.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryPredictor::initUpAxis(const NMP::Vector3& upAxisV)
{
  // Compute the alignment transform H_y that rotates the canonical Y axis
  // onto the world up axis vector
  m_alignUpAxisQuat.forRotation(NMP::Vector3(0, 1, 0), upAxisV);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryPredictor::setVelocities(
  float deltaTime,
  const NMP::Vector3& deltaPos,
  const NMP::Quat& deltaQuat)
{
  NMP_ASSERT(deltaTime > 0.0f);
  float recipDeltaTime = 1.0f / deltaTime;

  // Inverse alignment transform
  NMP_ASSERT(fabs(1.0f - m_alignUpAxisQuat.magnitude()) < 1e-3f);
  NMP::Quat invAlignupAxisQuat = m_alignUpAxisQuat;
  invAlignupAxisQuat.conjugate();

  //---------------------------
  // Compute the corresponding trajectory delta in the canonical Y up coordinate frame.
  // Points in the world space are transformed: x' = H_d * x
  // (H_y * H_y^-1) * x' = H_d * (H_y * H_y^-1) * x
  // u' = (H_y^-1 * H_d * H_y) * u,  where u and u' are points in the canonical Y up frame.
  // H_d' = H_y^-1 * H_d * H_y,  where H_d' is the corresponding trajectory delta in the canonical Y up frame
  NMP::Quat deltaAdjQuat = invAlignupAxisQuat * deltaQuat * m_alignUpAxisQuat;
  NMP::Vector3 deltaAdjPos = invAlignupAxisQuat.rotateVector(deltaPos);

  // Ensure that the delta quat is in the positive hemisphere so that the rotation angle
  // is within the range [-pi:pi]. Correspondingly the axis direction may then be flipped.
  if (deltaAdjQuat.w < 0.0f)
  {
    deltaAdjQuat *= -1.0f;
  }

  //---------------------------
  // Compute the velocities in the Y up coordinate frame.
  m_trajAdjAngVel = deltaAdjQuat.fastLog() * (recipDeltaTime * 2.0f);
  m_trajAdjPosVel = deltaAdjPos * recipDeltaTime;

  //---------------------------
  // Pivot point remains stationary under the transform: R*x + t = x
  // (R - I)*x = -t
  float A[4];
  A[0] = -2.0f * deltaAdjQuat.y * deltaAdjQuat.y;
  A[1] = 2.0f * deltaAdjQuat.w * deltaAdjQuat.y;
  A[2] = -A[1];
  A[3] = A[0];
  float det = A[0] * A[0] + A[1] * A[1];
  if (det < 1e-7f)
  {
    // Translational component of motion only. No rotation, i.e. pivot point
    // is at infinity
    m_useRotation = false;
    m_pivotAdjPos.setToZero();
  }
  else
  {
    // Can use screw decomposition to represent the rotation and translation components
    // of motion as a single rotation about a fixed pivot point in space
    m_useRotation = true;
    float b[2];
    b[0] = -deltaAdjPos.x;
    b[1] = -deltaAdjPos.z;
    // Invert
    float recipDet = 1.0f / det;
    float iA[4];
    iA[0] = A[3] * recipDet;
    iA[1] = -A[1] * recipDet;
    iA[2] = -A[2] * recipDet;
    iA[3] = A[0] * recipDet;
    // Solve for the pivot position
    m_pivotAdjPos.x = iA[0] * b[0] + iA[1] * b[1];
    m_pivotAdjPos.y = 0.0f;
    m_pivotAdjPos.z = iA[2] * b[0] + iA[3] * b[1];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectoryPredictor::predict(
  float deltaTime,
  NMP::Vector3& deltaPos,
  NMP::Quat& deltaQuat) const
{
  NMP::Quat deltaAdjQuat;
  NMP::Vector3 deltaAdjPos;

  //---------------------------
  // Compute the predicted trajectory delta rotation in the canonical Y up frame
  NMP::Vector3 trajAdjAngVel(0.0f, m_trajAdjAngVel.y, 0.0f);
  deltaAdjQuat.fastExp(trajAdjAngVel * (0.5f * deltaTime));

  //---------------------------
  // Compute the predicted trajectory delta position component in the canonical Y up frame
  if (m_useRotation)
  {
    // Use screw decomposition to represent the rotation and translation components
    // of motion as a single rotation about a fixed pivot point in space.
    // H = T_p * R * T_p^-1,  where T_p is the pivot point translation and R the screw rotation
    float omc = 2.0f * deltaAdjQuat.y * deltaAdjQuat.y;
    float s = 2.0f * deltaAdjQuat.w * deltaAdjQuat.y;
    deltaAdjPos.x = omc * m_pivotAdjPos.x - s * m_pivotAdjPos.z;
    deltaAdjPos.z = s * m_pivotAdjPos.x + omc * m_pivotAdjPos.z;

    // Linear component of translation in the Y direction
    deltaAdjPos.y = m_trajAdjPosVel.y * deltaTime;
  }
  else
  {
    // Translational component of motion only. No rotation, i.e. pivot point
    // is at infinity
    deltaAdjPos = m_trajAdjPosVel * deltaTime;
  }

  // Transform the predicted trajectory delta back to the world coordinate frame.
  // H_d = H_y * H_d' * H_y^-1
  NMP::Quat invAlignupAxisQuat = m_alignUpAxisQuat;
  invAlignupAxisQuat.conjugate();
  deltaQuat = m_alignUpAxisQuat * deltaAdjQuat * invAlignupAxisQuat;
  deltaPos = m_alignUpAxisQuat.rotateVector(deltaAdjPos);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
