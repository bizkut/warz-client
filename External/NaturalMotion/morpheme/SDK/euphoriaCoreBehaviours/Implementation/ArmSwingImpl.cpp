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
#include "MyNetworkPackaging.h"
#include "ArmSwingPackaging.h"
#include "ArmSwing.h"
#include "ArmPackaging.h"
#include "MyNetworkPackaging.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// This behaviour could be improved by using the angular velocity of the whole spine (rather than just the chest)
// However, this would make the module dependent on bodyFrame which make it less portable
void ArmSwingUpdatePackage::update(
  float NMP_UNUSED(timeStep), 
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // don't try and swing if hand is constrained
  if (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_isConstrained)
  {
    return;
  }
  // don't swing arms around when you are in contact, it looks silly, but do when grabbing
  if (owner->in->getIsBodyBarFeetContacting() && (!(in->getNumConstrainedArms() > 0)))
  {
    return;
  }

  // early-out if no stiffness
  float stiffnessScale = 1.0f;
  float importanceScale = 1.0f;
  if (in->getSwingStiffnessScaleImportance())
  {
    stiffnessScale = in->getSwingStiffnessScale();
    if (stiffnessScale == 0.0f)
    {
      return;
    }
    importanceScale = NMP::clampValue(stiffnessScale, 0.0f, 1.0f);
  }

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
    owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation(),
    in->getRotationRequest().rotation * SCALE_DIST(1.0f), 
    NMP::Colour(ER::convertControlAmountsToDefaultColour(in->getRotationRequest().debugControlAmounts)));

  // The idea of this module is to control the root orientation and angular
  // velocity by waving the arms around (they simply swing in the opposite
  // direction to the required spring-damper like torque)
  if (
    in->getRotationRequestImportance() &&
    !owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_mainPartIsColliding
    && owner->data->normalStiffness > 0.001f)
  {
    // How much do we have to swing an arm in order to rotate the pelvis with normalStiffness?
    // The answer depends on how heavy the arms are compared to the body we're trying to rotate.
    //
    // It should be that it will swing more for smaller strengthReduction, since the
    // calculated urgency is imminence/(stiffness*strengthReduction).
    // This fixed mass value below still scales with the character mass, and can be controlled later by the stiffness
    // scale. So this is an acceptable value. It is less than the full upper body mass, as the
    // upper body is bendable, so a force gives a larger acceleration than if it were rigid.
    float massUpperBody = SCALE_MASS(11.0f);
    float massArm = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_dynamicState.mass;
    // large swingfrequencyScale will swing harder and more frequently
    float strengthReduction = massArm / massUpperBody;

    float imminence = in->getRotationRequest().imminence;
    NMP::Vector3 rotation = in->getRotationRequest().rotation;

    // calculate the importance and stiffness with which to swing, based on its capability
    ProcessRequest process;
    const float importance = importanceScale * process.processRequest(
      in->getRotationRequestImportance(),
      imminence,
      owner->data->normalStiffness * strengthReduction,
      1.0f,
      owner->owner->data->minStiffnessForActing * strengthReduction,
      owner->data->normalStiffness * in->getRotationRequest().stiffnessScale * strengthReduction);

    if (!importance)
    {
      return;
    }

    float stiffness = owner->data->normalStiffness * process.stiffnessScale;
    // 1. get target rotation acc / sqr(stiffness)
    // speed scale gives an extra distance to the hand targets, which works well for the sort of speeds used
    const float eps = 1e-10f;
    rotation -= owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootData.angularVelocity
      / (eps + in->getRotationRequest().imminence);

    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta,
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation(),
      rotation * SCALE_DIST(1.0f),  NMP::Colour(ER::getDefaultColourForControl(ER::swingControl)));

    // 2. shift hand laterally to current direction... this spins it around and out
    NMP::Vector3 pivot = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation();
    NMP::Vector3 pivotToHand = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation()
      - pivot;
    NMP::Vector3 handShift = vCross(pivotToHand, rotation);

    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
      handShift,  NMP::Colour(ER::getDefaultColourForControl(ER::swingControl)));

    // Construct target based on where the hand is plus handShift
    NMP::Vector3 handTarget = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation()
      + handShift; 
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, handTarget, SCALE_DIST(0.1f), NMP::Colour(ER::getDefaultColourForControl(ER::swingControl), 255));

    // 3. if arm would swing into body, then prevent it from swinging
    NMP::Vector3 sideways = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.rightDirection();
    float dot = (owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation()
      - owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation()).dot(sideways);
    if (dot < 0.0f)
    {
      sideways = -sideways;
    }
    float outwardsHandDistance 
      = (handTarget - 
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation()
        ).dot(sideways);
    bool armsOutwardsOnly = in->getArmsOutwardsOnlyImportance() ? in->getArmsOutwardsOnly() : false;
    if (armsOutwardsOnly && outwardsHandDistance < -0.0f)
    {
      handTarget -= sideways * outwardsHandDistance;
    }
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, handTarget, SCALE_DIST(0.1f), NMP::Colour::WHITE);

    LimbControl& control = out->startControlModification();
    control.reinit(stiffness * stiffnessScale, owner->data->normalDampingRatio, owner->data->normalDriveCompensation);
    control.setTargetPos(handTarget, 1.0f);
    control.setControlAmount(ER::swingControl, 1.0f);
    out->stopControlModification(importance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmSwing::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

