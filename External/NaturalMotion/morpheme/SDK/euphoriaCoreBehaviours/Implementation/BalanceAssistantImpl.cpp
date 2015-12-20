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
#include "BalanceAssistant.h"
#include "BodyFrame.h"
#include "MyNetwork.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erLimb.h"
#define SCALING_SOURCE owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BalanceAssistant::update(float NMP_UNUSED(timeStep))
{
  if (
    in->getBalanceAssistanceParametersImportance() == 0.0f ||
    in->getDesiredPelvisTMImportance() == 0.0f ||
    in->getBalanceAmountImportance() == 0.0f)
  {
    return;
  }

#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
  MR::InstanceDebugInterface* pDebugDrawInst = rootModule->getDebugInterface();
  MR_DEBUG_MODULE_ENTER(pDebugDrawInst, getClassName());
#endif

  const BalanceAssistanceParameters& balanceAssistantParams = in->getBalanceAssistanceParameters();
  const float balanceAmount = in->getBalanceAmount();
  const MyNetworkData* myNetworkData = owner->owner->data;

  float stiffness = myNetworkData->normalStiffness;
  if (balanceAssistantParams.reduceAssistanceWhenFalling)
  {
    stiffness *= balanceAmount;
  }
  float criticalDamping = 2.0f * stiffness;

  float orientationStiffness = stiffness * balanceAssistantParams.orientationAssistanceAmount;
  float positionStiffness = stiffness * balanceAssistantParams.positionAssistanceAmount;
  float velocityDamping = criticalDamping * balanceAssistantParams.velocityAssistanceAmount;
  float velocityDampingFromPosition = 0.0f;

  if (orientationStiffness > 0.0f || positionStiffness > 0.0f)
  {
    MR_DEBUG_DRAW_MATRIX(rootModule->getDebugInterface(), in->getDesiredPelvisTM(), SCALE_DIST(1.0f));
  }

  // Early out if there's nothing to do
  if (orientationStiffness == 0.0f && positionStiffness == 0.0f && velocityDamping == 0.0f)
  {
    return;
  }

  ER::Character* character = owner->owner->getCharacter();
  ER::Body& body = character->getBody();
  MR::PhysicsRig* physicsRig = body.getPhysicsRig();

  int pelvisIndex = body.getLimb(myNetworkData->firstSpineRigIndex).getDefinition()->m_rootIndex;
  int chestIndex = body.getLimb(myNetworkData->firstSpineRigIndex).getDefinition()->m_endIndex;
  MR::PhysicsRig::Part* pelvis = physicsRig->getPart(pelvisIndex);
  MR::PhysicsRig::Part* chest = physicsRig->getPart(chestIndex);

  const ER::LimbTransform& pelvisTM = owner->owner->data->spineLimbSharedStates[0].m_rootTM;

  MR_DEBUG_DRAW_POINT(
    rootModule->getDebugInterface(),
    pelvisTM.translation() - NMP::Vector3(0, 1, 0),
    SCALE_DIST(1.0f) * balanceAmount,
    NMP::Colour::WHITE);

  // Calculate some generally useful things
  NMP::Vector3 averageFootPos(0, 0, 0);
  NMP::Vector3 averageStanceFootVel(0, 0, 0);
  float numStanceFeet = 1e-10f;
  float numFeet = 1e-10f;
  bool useCounterForceOnFeet[NetworkConstants::networkMaxNumLegs];
  for (uint32_t i = 0; i < myNetworkData->numLegs; i++)
  {
    averageFootPos += owner->owner->data->legLimbSharedStates[i].m_endTM.translation();
    numFeet++;

    // Stance foot vel is our measure of the ground speed for the landing swing foot
    if (!feedIn->getIsStepping(i) && owner->owner->data->legLimbSharedStates[i].m_endData.isCollidingWithWorld)
    {
      ++numStanceFeet;
      averageStanceFootVel += owner->owner->data->legLimbSharedStates[i].m_endData.velocity;
      useCounterForceOnFeet[i] = true;
    }
    else
    {
      useCounterForceOnFeet[i] = false;
    }
  }
  averageFootPos /= numFeet;
  averageStanceFootVel /= numStanceFeet;

  // COM and COMvel relative to the feet
  NMP::Vector3 CoMPosition = owner->owner->data->bodyLimbSharedState.m_dynamicState.getPosition();
  NMP::Vector3 CoMVelocity = owner->owner->data->bodyLimbSharedState.m_dynamicState.getVelocity();

  NMP::Vector3 CoMPosRelFeet = CoMPosition - averageFootPos;
  // Apply assistance using torques on the pelvis
  if (orientationStiffness > 0.0f)
  {
    // Use a critically damped spring
    static float dampingRatio = 1.0f;
    float strength = orientationStiffness * orientationStiffness;
    float damping = 2.0f * orientationStiffness * dampingRatio;

    // Evaluate the angular "errors" from our target
    const ER::LimbTransform& targetPelvisTM = in->getDesiredPelvisTM();
    NMP::Vector3 angleError = NMP::vCross(targetPelvisTM.upDirection(), pelvisTM.upDirection());
    NMP::Vector3 angVelError = owner->owner->data->spineLimbSharedStates[0].m_endData.angularVelocity;
    NMP::Vector3 angAcceleration = -strength * angleError - damping * angVelError;

    // below, scale by 2 because we're bending more than just the pelvis, its affecting
    // neighbouring bodies too
    NMP::Vector3 pelvisInertia = pelvis->getMassSpaceInertiaTensor();
    // the body is flexible so we aren't rotating the whole body here

    // Clamp the acceleration
    float angAccelerationMag = angAcceleration.magnitude();
    float angAccelerationClamp = SCALE_ANGACCEL(balanceAssistantParams.maxAngularAccelerationAssistance);
    if (angAccelerationClamp > 0.0f && angAccelerationMag > angAccelerationClamp)
    {
      angAcceleration *= angAccelerationClamp / angAccelerationMag;
    }

    // Apply the torque
    float approxPelvisInertia = 2.0f * pelvisInertia.magnitude();
    NMP::Vector3 pelvisTorque = angAcceleration * approxPelvisInertia;
    pelvis->addTorque(pelvisTorque);

    MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_Normal, pelvisTM.translation(),
      targetPelvisTM.upDirection() * SCALE_DIST(1.0f), NMP::Colour::WHITE);
    MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_AngleDelta,
      pelvisTM.translation(), angleError, NMP::Colour::YELLOW);
    MR_DEBUG_DRAW_VECTOR(rootModule->getDebugInterface(), MR::VT_AngularVelocity,
      pelvisTM.translation(), angVelError, NMP::Colour::TURQUOISE);
  }

  //
  NMP::Vector3 pelvisAcc(NMP::Vector3::InitZero);

  // Apply assistance using forces on the pelvis (primarily)
  if (positionStiffness > 0.0f)
  {
    float strength = positionStiffness * positionStiffness;
    // Use a critically damped spring so add onto the velocity damping
    velocityDampingFromPosition += 2.0f * positionStiffness;

    // flatten the difference
    CoMPosRelFeet -= myNetworkData->up * myNetworkData->up.dot(CoMPosRelFeet);
    MR_DEBUG_DRAW_VECTOR(
      rootModule->getDebugInterface(), MR::VT_Delta, averageFootPos, CoMPosRelFeet, NMP::Colour::YELLOW);
    pelvisAcc -= strength * CoMPosRelFeet;
  } // positionStiffness

  // Also apply external forces to provoke/push stagger - relative to the ground
  if (velocityDamping > 0.0f || velocityDampingFromPosition > 0.0f)
  {
    NMP::Vector3 velErrorFromPosition = CoMVelocity - averageStanceFootVel;
    NMP::Vector3 velError = CoMVelocity - balanceAssistantParams.targetVelocity;
    MR_DEBUG_DRAW_VECTOR(
      rootModule->getDebugInterface(), MR::VT_Velocity, averageFootPos, -velErrorFromPosition, NMP::Colour::TURQUOISE);
    MR_DEBUG_DRAW_VECTOR(
      rootModule->getDebugInterface(), MR::VT_Velocity, averageFootPos, -velError, NMP::Colour::BLUE);
    pelvisAcc -= velErrorFromPosition * velocityDampingFromPosition;
    pelvisAcc -= velError * velocityDamping;
  }

  pelvisAcc = pelvisAcc.getComponentOrthogonalToDir(owner->owner->data->up);

  // Clamp the acceleration
  float linearAcceleration = pelvisAcc.magnitude();
  float linearAccelerationClamp = SCALE_ACCEL(balanceAssistantParams.maxLinearAccelerationAssistance);
  if (linearAccelerationClamp > 0.0f && linearAcceleration > linearAccelerationClamp)
  {
    pelvisAcc *= linearAccelerationClamp / linearAcceleration;
  }

  // Apply the actual force
  float approxPelvisMass = pelvis->getMass(); 
  NMP::Vector3 pelvisForce = pelvisAcc * approxPelvisMass;
  MR_DEBUG_DRAW_VECTOR(
    rootModule->getDebugInterface(), MR::VT_Force, pelvis->getPosition(), pelvisForce, NMP::Colour::WHITE);
  pelvis->addForce((1.0f - balanceAssistantParams.chestToPelvisRatio) * pelvisForce);
  chest->addForce(balanceAssistantParams.chestToPelvisRatio * pelvisForce);

  // Apply the counter force on the feet if desired
  if (balanceAssistantParams.useCounterForceOnFeet && numStanceFeet > 0.5f)
  {
    float scale = 0.5f / numStanceFeet;
    for (uint32_t i = 0; i < myNetworkData->numLegs; i++)
    {
      if (useCounterForceOnFeet[i])
      {
        int footIndex = body.getLimb(
          myNetworkData->firstLegRigIndex + i).getDefinition()->m_endIndex;
        MR::PhysicsRig::Part* foot = physicsRig->getPart(footIndex);
        MR_DEBUG_DRAW_VECTOR(
          rootModule->getDebugInterface(), MR::VT_Force, foot->getPosition(), -pelvisForce, NMP::Colour::WHITE);

        foot->addForce(-pelvisForce * scale);
        if (body.getLimb(
          myNetworkData->firstLegRigIndex + i).getDefinition()->m_numPartsInChain > 0)
        {
          MR::PhysicsRig::Part* shin = physicsRig->getPart(footIndex - 1);
          shin->addForce(-pelvisForce * scale);
        }
      }
    }
  }


}

//----------------------------------------------------------------------------------------------------------------------
void BalanceAssistant::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
