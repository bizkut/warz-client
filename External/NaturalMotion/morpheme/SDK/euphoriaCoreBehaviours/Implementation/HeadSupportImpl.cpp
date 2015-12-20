/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
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
#include "HeadPackaging.h"
#include "HeadSupportPackaging.h"
#include "HeadSupport.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadSupportUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  if (owner->in->getChestControlledAmount() > 0.0f)
  {
    const float supportImportance = 1.0f;
    LimbControl& control = out->startControlModification();
    control.reinit(owner->data->normalStiffness, owner->data->normalDampingRatio, owner->data->normalDriveCompensation);
    // Use the default pose as a basic support pose, but allow other things to request other orientations
    NMP::Quat rootQuat = owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_rootTM.toQuat();
    NMP::Quat targetEndQ =
      rootQuat * owner->owner->data->defaultPoseEndsRelativeToRoot[owner->data->limbNetworkIndex].toQuat();
    float desiredImportance = in->getDesiredHeadRelChestOrientationImportance();
    if (desiredImportance > 0.0f)
    {
      NMP::Quat desiredQ = rootQuat * in->getDesiredHeadRelChestOrientation();
      targetEndQ.slerp(desiredQ, desiredImportance);
    }
    control.setTargetOrientation(targetEndQ, supportImportance);
    // We don't actually care where the head is, so long as the direction is right, but set the
    // position with a low weight so that debugging is less confusing.
    control.setTargetPos(owner->owner->data->headLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
      supportImportance * 0.001f);
    control.setGravityCompensation(1.0f);
    control.setImplicitStiffness(0.5f);
    control.setControlAmount(ER::supportControl, 1.0f);
    // Note, we don't set control.setInSupport because that makes it support the root with the end
    out->stopControlModification(supportImportance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HeadSupport::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

