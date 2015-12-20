// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMIK/NMIKCommon.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
#ifndef NMIK_NO_TERTIARIESx
void solveJointTertiaries(
  float& weightOut,
  const IKJointParams* jointParams,
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData)
{
  const uint32_t& ji = solverData.jointIndices[solverData.jointPosition];
  GeomUtils::PosQuat& joint = joints[ji];
  NMP::Quat newQ = joint.q;
  const float& weightIn = jointParams->redundancyControlWeight;
  const TertiaryConstraints* tertiaries = solverData.tertiaries;

  // Set swivelAxis - all secondary rotation is around this axis.  For normal secondaries we could in
  // theory use either the direction to the end effector of the direction to the target (or anywhere
  // in between).  We use the direction to the target because only that is compatible with PointIK.
  NMP::Vector3 swivelAxis = vNormalise(solverData.localTarget.t);

  // We can't do swivel control for hinge joints unless the hinge axis and the swivel axis happen
  // to line up (like in a roll bone).  Also the various other provisos for conditioning.
  bool canSwivel =
    tertiaries && // We can stop all these comparisons and skip everything if there are no tertiaries.
    solverData.jointPosition < solverData.numJoints - 1 &&
    solverData.localEndEffector.t.magnitudeSquared() > GeomUtils::gTolSq &&
    solverData.localTarget.t.magnitudeSquared() > GeomUtils::gTolSq &&
    (!jointParams->isHinge || swivelAxis.distanceSquaredTo(jointParams->hingeAxis) < GeomUtils::nTolSq);

  // Initialise the output weight
  weightOut = 0;

  if (canSwivel && weightIn > 0)
  {

    // Generic redundancy control
    NMP::Quat swivelQuat(0, 0, 0, 0);

    // Secondary pole vector constraints
    for (uint32_t i = 0; i < tertiaries->numPoleVectorControllers; ++i)
    {
      const PoleVectorControlParams& secondary = tertiaries->poleVectorControllers[i];
      if (tertiaries->poleVectorJoints[i] == ji && secondary.active)
      {
        NMP::Vector3 fromV = newQ.rotateVector(secondary.controlPoleVector);
        NMP::Vector3 toV = secondary.referencePoleVector;
        NMP::Quat q =
          GeomUtils::forRotationAroundAxisSoft(fromV, toV, swivelAxis, xHingeSoftness, xFlipSoftness) *
          secondary.weight * weightIn;
        if (swivelQuat.dot(q) >= 0)
        {
          swivelQuat += q;
        }
        else
        {
          swivelQuat -= q;
        }

        // Increment the output weight
        weightOut += secondary.weight;

        // Debug draw of pole vectors
  #ifdef NMRU_HYBRIDIK_CCDSOLVER_DEBUGDRAW_POLES
        if (solverData.debugBuffer)
        {
          NMP::Vector3 refV = solverData.globalJointTM.q.rotateVector(toV);
          NMP::Vector3 controlV = solverData.globalJointTM.q.rotateVector(fromV);
          solverData.debugBuffer->drawDashedLine(
            solverData.globalJointTM.t, solverData.globalJointTM.t + (controlV * 0.1f), 0.04f);
          solverData.debugBuffer->drawDashedLine(
            solverData.globalJointTM.t, solverData.globalJointTM.t + (refV * 0.2f), 0.04f);
        }
  #endif
      }
    }

    // Secondary quat constraints
    for (uint32_t i = 0; i < tertiaries->numRotations; ++i)
    {
      if (tertiaries->quatJoints[i] == ji)
      {
        NMP::Quat qConstraint = tertiaries->quatConstraints[i];
        float weight = qConstraint.magnitude();
        NMP::Quat q =
          GeomUtils::closestRotationAroundAxisSoft(qConstraint, swivelAxis, xHingeSoftness) *
          weight * weightIn;
        if (swivelQuat.dot(q) >= 0)
        {
          swivelQuat += q;
        }
        else
        {
          swivelQuat -= q;
        }

        // Increment the output weight
        weightOut += weight;
      }
    }

    // Merge and apply secondary constraints
    if ((tertiaries->numPoleVectorControllers + tertiaries->numRotations) > 0 && swivelQuat.magnitudeSquared() > 0)
    {
      swivelQuat.fastNormalise();
      if (newQ.dot(swivelQuat) < 0)
      {
        swivelQuat = -swivelQuat;
      }
      newQ = swivelQuat * newQ;
    }

  } // end of redundancy control condition

  // Copy back result
  joint.q = newQ;

} // end of solveJointTertiaries()

#else
void solveJointTertiaries(
  float& ,
  const IKJointParams* ,
  GeomUtils::PosQuat* ,
  const IKSolverData& ,
  const TertiaryConstraints*)
{
  // ...
}
#endif // #ifndef NMIK_NO_TERTIARIES

} // end of namespace NMRU
