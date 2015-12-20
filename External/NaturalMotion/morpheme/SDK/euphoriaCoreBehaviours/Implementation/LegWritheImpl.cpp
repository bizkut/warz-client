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
#include "LegWrithePackaging.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"
#include "LegWrithe.h"
#include "Leg.h"
#include "MyNetwork.h"
#include "Helpers/Helpers.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// LegWritheUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void LegWritheUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (in->getWritheParamsImportance() == 0.0f)
  {
    return;
  }

  const WritheParameters& writheParams = in->getWritheParams();

  if (data->lastResetTimer > writheParams.resetTargetTP)
  {
    //Work out based TM in root's space.
    NMP::Matrix34 basedTM = writheParams.defaultPoseEndRelativeToRoot;
    if (!writheParams.basedOnDefaultPose)
    {
      NMP::Matrix34 rootTM = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM;
      NMP::Matrix34 endTM = owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_endTM;
      rootTM.invertFast();
      basedTM.multiply(endTM, rootTM);
    }

    //Work out random translation to add.
    uint32_t childIndex = owner->data->childIndex;
    float rx = data->rng.genFloat(-0.5f, 0.5f, childIndex);
    float ry = data->rng.genFloat(-0.5f, 0.5f, childIndex);
    float rz = data->rng.genFloat(-0.5f, 0.5f, childIndex);
    NMP::Vector3 writheTranslation(rx, ry, rz);
    writheTranslation.fastNormalise();
    writheTranslation = writheTranslation * owner->data->length * writheParams.amplitudeScale;
    basedTM.translation() += writheTranslation;
    data->targetTM = basedTM;
    data->lastResetTimer = 0.0f; // Reset timer.
  }

  // Send control struct.
  LimbControl& control = out->startControlModification();
  control.reinit(
    owner->data->normalStiffness * writheParams.stiffnessScale,
    owner->data->normalDampingRatio,
    owner->data->normalDriveCompensation * writheParams.driveCompensationScale);
  control.setExpectedRootForTarget(
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM,
    1.0f,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootData.velocity,
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity);
  NMP::Matrix34 currentTargetTM = data->targetTM;
  currentTargetTM.multiply(owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootTM);
  control.setTargetPos(currentTargetTM.translation(), 1.0f, 
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootData.velocity);
  control.setTargetOrientation(currentTargetTM, 1.0f, 
    owner->owner->data->legLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity);
  control.setControlAmount(ER::writheControl, 1.0f);
  control.setGravityCompensation(1.0f);
  out->stopControlModification(1.0f);

  // Store data.
  data->lastResetTimer += timeStep; // Update timer.

  // Debug
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, currentTargetTM, SCALE_DIST(1.0f));
}

//----------------------------------------------------------------------------------------------------------------------
void LegWrithe::entry()
{
  data->targetTM.identity();
  data->rng.setSeed(owner->owner->getAndBumpNetworkRandomSeed());
  data->lastResetTimer = 100000.0f;  // Set to a really long time to be sure it starts at the entry.
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
