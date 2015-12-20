/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
#include "Helpers/Aim.h"

//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
#error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// matchWindingDirection
// 
// Transform targetTM to ensure that any rotation from the poseTM about the supplied winding axis is in the same 
// direction as the winding axis. Compares the rotation from the pose to the current orientation to the winding axis to 
// determine whether it is already wound the wrong way. Then compares the current orientation to the target orientation 
// to determine whether the current TM is likely to continue rotating in the wrong direction. When both these cases are 
// true the target transform if rotated through pi to force the current TM to start rotating in the correct direction.
// 
// This function should be applied to spine and head limbs with the winding axis calculated from the aiming arm to keep
// limb winding consistent across the character.
// 
void matchWindingDirection(
  const NMP::Vector3& windingAxis,
  const NMP::Matrix34& currentTM,
  const NMP::Matrix34& poseTM,
  NMP::Matrix34& targetTM)
{
  static const float windingThreshold(NM_PI * 0.125f); // pi / 8

  // Find rotation from the pose to the current TM and from the current TM to the target TM.
  NMP::Matrix34 invTargetTM(targetTM);
  invTargetTM.invert();

  NMP::Matrix34 invCurrentTM(currentTM);
  invCurrentTM.invert();

  const NMP::Vector3 poseToCurrent((poseTM * invCurrentTM).toRotationVector());
  const NMP::Vector3 currentToTarget((currentTM * invTargetTM).toRotationVector());

  const float orientationAlignment = windingAxis.dot(poseToCurrent);   // Current orientation compared with winding.
  const float rotationAlignment    = windingAxis.dot(currentToTarget); // Likely movement compared with winding.

  if ((orientationAlignment < -windingThreshold) && (rotationAlignment < -windingThreshold))
  {
    // Rotate target TM by pi about the winding axis to encourage any limb using the target TM as an IK orientation
    // target to start rotating in the winding direction.
    NMP::Matrix34 targetOffset(NMP::Matrix34::kIdentity);
    targetOffset.fromRotationVector(windingAxis * NM_PI);
    targetTM = targetOffset * targetTM;
  }
}

}