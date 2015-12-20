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
#ifndef MR_TRAJECTORY_PREDICTION_H
#define MR_TRAJECTORY_PREDICTION_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TrajectoryPredictor
/// \brief A class that dead reckons the position of the character root. The prediction accounts
/// for the rotational component over a long duration by decomposing the transform into a pivot
/// about a point in space.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryPredictor
{
public:
  TrajectoryPredictor() {}
  ~TrajectoryPredictor() {}

  NM_INLINE void init(
    const NMP::Vector3& upAxisV,
    float deltaTime,
    const NMP::Vector3& deltaPos,
    const NMP::Quat& deltaQuat);

  void initUpAxis(const NMP::Vector3& upAxisV);

  void setVelocities(
    float deltaTime,
    const NMP::Vector3& deltaPos,
    const NMP::Quat& deltaQuat);

  void predict(
    float deltaTime,
    NMP::Vector3& deltaPos,
    NMP::Quat& deltaQuat) const;

  const NMP::Vector3& getPivotPosition() const { return m_pivotAdjPos; }
  const NMP::Vector3& getAngularVelocity() const { return m_trajAdjAngVel; }
  const NMP::Vector3& getLinearVelocity() const { return m_trajAdjPosVel; }
  bool isRotating() const { return m_useRotation; }

protected:
  NMP::Quat         m_alignUpAxisQuat;

  NMP::Vector3      m_trajAdjPosVel;
  NMP::Vector3      m_trajAdjAngVel;
  NMP::Vector3      m_pivotAdjPos;

  bool              m_useRotation;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TrajectoryPredictor::init(
  const NMP::Vector3& upAxisV,
  float deltaTime,
  const NMP::Vector3& deltaPos,
  const NMP::Quat& deltaQuat)
{
  initUpAxis(upAxisV);
  setVelocities(deltaTime, deltaPos, deltaQuat);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRAJECTORY_PREDICTION_H
//----------------------------------------------------------------------------------------------------------------------
