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
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorRollDownStairs.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsRig.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erBody.h"
#include "euphoria/erLimb.h"
#include "euphoria/erEuphoriaLogger.h"
#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
// Utility Functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Calculate the contribution to the blend weight of the pose animation as a function of the reference body part's
// orientation relative to the slope normal.
static float calculateWeightFromOrientation(
  const NMP::Vector3& slopeNormal,
  const NMP::Vector3& characterForwardDirection,
  const NMP::Vector3& characterAngularVelocity,
  const AttribDataOperatorRollDownStairsDef* const stateData)
{
  float weight(0.0f);

  NMP_ASSERT(stateData);      // Null check.

  if (stateData)
  {
    // The transition region (over which the pose anim is blended in/out) is different depending on whether the
    // character is rolling from back to front (entering the pose) or rolling from front to back (exiting the pose)
    // - determine roll direction using axis of rotation and slope normal (the result's magnitude is ignored).
    const NMP::Vector3 rollDirection(NMP::vCross(characterAngularVelocity, slopeNormal));

    // Find the angular separation between the reference part's forward direction and the slope normal.
    NMP::Quat rotationFromSlopeNormalToForwardDirection;
    rotationFromSlopeNormalToForwardDirection.forRotation(slopeNormal, characterForwardDirection);
    const float angWithNormal(rotationFromSlopeNormalToForwardDirection.angle());

    // - dot the characterForwardDirection with the direction in which the character is rolling to determine which
    //   transition region we are in.
    const bool isRollingFromBackToFront = NMP::vDot(rollDirection, characterForwardDirection) > 0.0f;

    // Calculate the pose weight.
    if (isRollingFromBackToFront)
    {
      const float angle  = angWithNormal;
      const float minAng = NMP::degreesToRadians(stateData->m_minEnterAngleWithNormalToApplyRollPose);
      const float maxAng = NMP::degreesToRadians(stateData->m_maxEnterAngleWithNormalToApplyRollPose);

      const float range = NMP::maximum(maxAng - minAng, 0.0001f);
      weight = (angle - minAng) / range;
    }
    else
    {
      const float angle = (2.0f * NM_PI) - angWithNormal;
      const float minAng = NMP::degreesToRadians(stateData->m_minExitAngleWithNormalToApplyRollPose);
      const float maxAng = NMP::degreesToRadians(stateData->m_maxExitAngleWithNormalToApplyRollPose);

      const float range = NMP::maximum(maxAng - minAng, 0.0001f);
      weight = 1.0f - ((angle - minAng) / range);
    }

    weight = NMP::clampValue(weight, 0.0f, 1.0f);
  }

  return weight;
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the contribution to the pose animations blend weight from the angular velocity of the reference body part.
//
// Calculates the reference part's angular velocity as a fraction of the 'Angular Velocity To Apply Pose' range defined
// in the attributes. The result is clamped to the interval [0.0, 1.0].
static float calculateWeightFromAngularVelocity(
  const NMP::Vector3& characterAngularVelocity,
  MR::PhysicsRig* const physicsRig,
  AttribDataOperatorRollDownStairsDef* stateData)
{
  float weight(0.0f);

  NMP_ASSERT(physicsRig);     // Null check.
  NMP_ASSERT(stateData);      // Null check.

  if (physicsRig && stateData)
  {
    // Dimensionally scale angular velocity range defined in attributes.
    const ER::DimensionalScaling& dimensionalScaling = Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();

    const float minAngVel(NMP::degreesToRadians(dimensionalScaling.scaleAngVel(stateData->m_minAngularVelocityToApplyRollPose)));
    const float maxAngVel(NMP::degreesToRadians(dimensionalScaling.scaleAngVel(stateData->m_maxAngularVelocityToApplyRollPose)));

    const float angVelMag(characterAngularVelocity.magnitude());

    // Calculate the angular velocity as a fraction of the range.
    const float velocityRange = (maxAngVel - minAngVel);
    weight = (velocityRange > 0.0f)? (angVelMag - minAngVel) / velocityRange : 1.0f;
    weight = NMP::clampValue(weight, 0.0f, 1.0f);
  }

  return weight;
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the contribution to the pose animations blend weight from the tangential velocity of the reference body
// part.
//
// Calculate the component of linear velocity of the reference body part parallel to the slopeDirection as a fraction
// of the 'Tangential Velocity To Apply Pose' range defined in the attributes. The result is clamped to the
// interval [0.0, 1.0].
static float calculateWeightFromTangentialVelocity(
  const NMP::Vector3& slopeDirection,
  MR::PhysicsRig* const physicsRig,
  AttribDataOperatorRollDownStairsDef* stateData)
{
  float weight(0.0f);

  NMP_ASSERT(physicsRig); // Null check.
  NMP_ASSERT(stateData);  // Null check.

  if (physicsRig && stateData)
  {
    const ER::DimensionalScaling& dimensionalScaling = Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();
    const float minVel(dimensionalScaling.scaleVel(stateData->m_minTangentialVelocityToApplyRollPose));
    const float maxVel(dimensionalScaling.scaleVel(stateData->m_maxTangentialVelocityToApplyRollPose));

    const NMP::Vector3  COMVel(physicsRig->calculateCentreOfMassVelocity());
    const float velMag(COMVel.dot(slopeDirection));

    // Calculate the linear velocity as a fraction of the range.
    const float velocityRange = (maxVel - minVel);
    weight = (velocityRange > 0.0f)? (velMag - minVel) / velocityRange : 1.0f;
    weight = NMP::clampValue(weight, 0.0f, 1.0f);
  }

  return weight;
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the acceleration that should be applied to the reference body part
//
// A push acceleration is applied to the reference body part to ensure that the character keeps moving down the slope.
static NMP::Vector3 calculatePushAcceleration(
  const float pushAccelerationMaxMagnitude,
  const float velocityRatio,
  const NMP::Vector3& slopeDirection,
  MR::PhysicsRig* const physicsRig)
{
  // calculate cheating force.
  NMP::Vector3 acceleration(NMP::Vector3Zero());

  NMP_ASSERT(physicsRig); // null check

  if (physicsRig)
  {
    const ER::DimensionalScaling& dimensionalScaling = Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();

    const float pushMultiplier(1.0f - velocityRatio);
    if (pushMultiplier > 0.0f)
    {
      acceleration = slopeDirection * pushMultiplier * dimensionalScaling.scaleAccel(pushAccelerationMaxMagnitude);
      const NMP::Vector3 upAxis(physicsRig->getPhysicsScene()->getWorldUpDirection());
      acceleration = acceleration.getComponentOrthogonalToDir(upAxis);
    }
  }

  return acceleration;
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the acceleration that should be applied to the reference body part
//
// A push acceleration is applied to the reference body part to ensure that the character keeps moving down the slope.
static NMP::Vector3 calculateSpineAlignementAcceleration(
  const NMP::Vector3& slopeNormal,
  const NMP::Vector3& slopeDirection,
  const float sideRollAmount,
  ER::Body* const body,
  MR::PhysicsRig* const physicsRig,
  MR::InstanceDebugInterface* debugInterface)
{
  // Compute the angular accel to align the (spine up direction with the steps)
  //

  // Early out for anything but a positive non-zero sideRollAmount
  if (sideRollAmount <= 0.0f)
  {
    return NMP::Vector3::InitTypeZero;
  }

  // Calc spine up direction
  NMP::Vector3 rootPos  = body->getLimb(body->m_definition->getFirstSpineLimbIndex()).getRootTransform().translation();
  NMP::Vector3 chestPos  = body->getLimb(body->m_definition->getFirstSpineLimbIndex()).getEndTransform().translation();
  NMP::Vector3 spineUp(chestPos - rootPos);
  spineUp.fastNormalise();

  // Calc level direction (across step)
  NMP::Vector3 levelDirection(NMP::vCross(slopeNormal, slopeDirection));
  levelDirection.fastNormalise();

  // Correction axis of rotation
  NMP::Vector3 errorToLevelDirection(NMP::vCross(spineUp, levelDirection));

  // The acceleration (using the error calc'd above together with a dimensionally scaled reference gain 
  // scaled by the user specified sideRollAmount)
  const ER::DimensionalScaling& dimensionalScaling = Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();
  const float gain = dimensionalScaling.scaleStrength(60.0f) * sideRollAmount;
  const NMP::Vector3 result( gain * errorToLevelDirection);

  // Some debug output
  //
  if (debugInterface)
  {
#if defined MR_OUTPUT_DEBUGGING
    // Dimensionally scaled distance of 1 for debug draw dimensions
    const float one = dimensionalScaling.scaleDist(1.0f);

    // The slope and level directions
    MR_DEBUG_DRAW_LINE(
      debugInterface,
      rootPos,
      rootPos + slopeDirection * one,
      NMP::Colour::LIGHT_BLUE);
    MR_DEBUG_DRAW_LINE(
      debugInterface,
      rootPos,
      rootPos + levelDirection * one,
      NMP::Colour::LIGHT_RED);

    // The spine up
    MR_DEBUG_DRAW_POINT(
      debugInterface, 
      chestPos, one, NMP::Colour::LIGHT_GREEN);
    MR_DEBUG_DRAW_POINT(
      debugInterface,
      rootPos, one, NMP::Colour::LIGHT_GREEN);
    MR_DEBUG_DRAW_LINE(
      debugInterface,
      rootPos,
      rootPos + spineUp,
      NMP::Colour::LIGHT_GREEN);

    // The angular acceleration
    MR_DEBUG_DRAW_LINE(
      debugInterface,
      rootPos,
      rootPos + result,
      NMP::Colour::WHITE);
#endif
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
static void endianSwap(AttribDataOperatorRollDownStairsDef* target)
{
  NMP_ASSERT(target); // null check

  if (target)
  {
    NMP::endianSwap(target->m_minEnterAngleWithNormalToApplyRollPose);
    NMP::endianSwap(target->m_maxEnterAngleWithNormalToApplyRollPose);
    NMP::endianSwap(target->m_minExitAngleWithNormalToApplyRollPose);
    NMP::endianSwap(target->m_maxExitAngleWithNormalToApplyRollPose);
    NMP::endianSwap(target->m_minTangentialVelocityToApplyRollPose);
    NMP::endianSwap(target->m_maxTangentialVelocityToApplyRollPose);
    NMP::endianSwap(target->m_minAngularVelocityToApplyRollPose);
    NMP::endianSwap(target->m_maxAngularVelocityToApplyRollPose);
    NMP::endianSwap(target->m_minMotionRatioToBeConsideredRolling);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template< typename attributeType, typename valueType > static void writeToOutputCPPin(
  const MR::PinIndex OutCPPinID,
  const valueType& value,
  MR::NodeDef* const node,
  MR::Network* const net)
{
  NMP_ASSERT(net);  // Null check.
  NMP_ASSERT(node); // Null check.

  if (net && node)
  {
    MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
    MR::OutputCPPin* const outputCPPin(nodeBin->getOutputCPPin(OutCPPinID));

    // Create the output attributes if they don't already exist.
    if (!outputCPPin->m_attribDataHandle.m_attribData)
    {
      outputCPPin->m_attribDataHandle = attributeType::create(net->getPersistentMemoryAllocator(), value);
    }
    else
    {
      // Write value to existing output pin
      attributeType* const outCPAttrib(outputCPPin->getAttribData<attributeType>());

      if (outCPAttrib)
      {
        outCPAttrib->m_value = value;
      }
    }

    // The requested output pin will automatically be flagged as having been updated this frame,
    // but we need to make sure that all pins we have updated are correctly flagged.
    outputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorRollDownStairsDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorRollDownStairsDef::locate(MR::AttribData* target)
{
  MR::AttribData::locate(target);

  AttribDataOperatorRollDownStairsDef* const result(static_cast< AttribDataOperatorRollDownStairsDef* >(target));
  endianSwap(result);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorRollDownStairsDef::dislocate(MR::AttribData* target)
{
  MR::AttribData::dislocate(target);

  AttribDataOperatorRollDownStairsDef* const result(static_cast< AttribDataOperatorRollDownStairsDef* >(target));
  endianSwap(result);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorRollDownStairsDef* AttribDataOperatorRollDownStairsDef::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataOperatorRollDownStairsDef* result;

  NMP::Memory::Format memReqs = AttribDataOperatorRollDownStairsDef::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataOperatorRollDownStairsDef::init(resource, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorRollDownStairsDef::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataOperatorRollDownStairsDef), MR_ATTRIB_DATA_ALIGNMENT);
  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorRollDownStairsDef* AttribDataOperatorRollDownStairsDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorRollDownStairsDef* const result(static_cast< AttribDataOperatorRollDownStairsDef* >(resource.ptr));
  resource.increment(sizeof(AttribDataOperatorRollDownStairsDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_ROLLDOWNSTAIRS_DEF);
  result->setRefCount(refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* nodeOperatorRollDownStairsOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  MR::Network* net)
{
  NMP_ASSERT(node); // Null check.
  NMP_ASSERT(net);  // Null check.

  NMP_ASSERT(outputCPPinIndex < NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_COUNT); // Pin index range check.

  if (node && net)
  {
    MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

    // Default values for the output pins
    NMP::Vector3 balanceErrorOutput(NMP::Vector3Zero());  // Used to force the character to fall if it is balanced.
    float poseWeightOutput(0.0f);                         // Pose anim blend weight.
    bool isRollingOutput(true);                           // Is the character considered to be rolling.

    // Performance does nothing if not enabled.
    MR::AttribDataBool* const inputEnableAttrib(
      net->updateOptionalInputCPConnection<MR::AttribDataBool>(node->getInputCPConnection(0), animSet));
    // If not connected, assume enabled.
    bool enabled(inputEnableAttrib ? inputEnableAttrib->m_value :true);
    if (enabled)
    {
      // Update remaining input parameters.
      //

      // Get the roll down slope attribute data.
      AttribDataOperatorRollDownStairsDef* const stateData(
        node->getAttribData<AttribDataOperatorRollDownStairsDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

      MR::AttribDataVector3* const inputSlopeNormalAttrib(
      net->updateOptionalInputCPConnection<MR::AttribDataVector3>(node->getInputCPConnection(1), animSet));
      NMP::Vector3 slopeNormal(inputSlopeNormalAttrib->m_value);
      slopeNormal.fastNormalise();

      MR::AttribDataFloat* const inputBalanceAmountAttrib(
      net->updateOptionalInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(2), animSet));
      // If not connected the character is assumed to be unbalanced.
      const float balanceAmount((inputBalanceAmountAttrib) ? inputBalanceAmountAttrib->m_value : 0.0f);

      MR::AttribDataFloat* const inputPushAccelerationMaxMagnitudeAttrib(
      net->updateOptionalInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(3), animSet));
      const float pushAccelerationMaxMagnitude((inputPushAccelerationMaxMagnitudeAttrib) ?
        inputPushAccelerationMaxMagnitudeAttrib->m_value : 50.0f); // Note that the default gets dimensionally scaled

      // This behaviour applies a single frame 'Pose' animation and a 'Push' acceleration to a character.
      // - The Pose is typically a curled up / foetal position suitable for rolling.
      // - The Push is used to keep the character moving down the slope.
      //
      // The magnitude of the Push and the blend weight of the Pose are determined by the velocity of the character down
      // the slope and its orientation relative to the slope normal. In general:
      // - The magnitude of the Push increases if the character is moving slowly.
      // - The blend weight of the Pose increases if the character is moving quickly.
      //
      // The algorithm proceeds as follows:
      // If the character is unbalanced:
      //   1. Find the contribution to the Pose blend weight from the characters orientation.
      //   2. Find the contribution to the Pose blend weight from the characters linear and angular velocity.
      //   3. Determine the Pose blend weight by combining the linear and angular velocity blend weights with the
      //      orientation blend weight.
      //   4. Determine the Push from the linear and angular velocity blend weights.
      //
      // If the character is balanced:
      //   Set the balance error s.t. it will become unbalanced.

      MR::PhysicsRig* const physicsRig(getPhysicsRig(net));
      ER::Character* character = networkGetCharacter(net);
      ER::Body* body = character ? &character->getBody() : 0;

      // Note that the physics rig etc can be null

      if (physicsRig  && character && body)
      {
        const NMP::Vector3 upAxis(physicsRig->getPhysicsScene()->getWorldUpDirection());
        NMP::Vector3 slopeDirection(-NMRU::GeomUtils::calculateOrthogonalPlanarVector(slopeNormal, upAxis));
        slopeDirection.fastNormalise();

        // Character is considered to be rolling if balanceAmount < this.
        const float ROLLING_BALANCE_THRESHOLD(0.7f);
        if (balanceAmount < ROLLING_BALANCE_THRESHOLD)
        {
          // The character is unbalanced

          // The pose animation weight depends on three factors : orientation of the character, angular velocity and
          // tangential velocity.

          // Find mean of spine root and end transforms.
          const ER::Limb& spineLimb = body->getLimb(body->m_definition->getFirstSpineLimbIndex());
          NMP::Matrix34 spineTM(NMP::Matrix34::kIdentity);
          spineTM.interpolate(spineLimb.getRootTransform(), spineLimb.getEndTransform(), 0.5f);

          MR_DEBUG_DRAW_MATRIX(net->getDispatcher()->getDebugInterface(), spineTM, 1.0f)

            // 1. Find the contribution to the Pose blend weight from the characters orientation
            const float weightFromOrientation =
            calculateWeightFromOrientation(slopeNormal, spineTM.xAxis(), spineLimb.getCOMAngularVelocity(), stateData);

          // 2. Find the contribution to the Pose blend weight from the characters linear and angular velocity
          const float weightFromAngularVelocity 
            = calculateWeightFromAngularVelocity(spineLimb.getCOMAngularVelocity(), physicsRig, stateData);
          const float weightFromTangentialVelocity =
            calculateWeightFromTangentialVelocity(slopeDirection, physicsRig, stateData);

          const float weightFromVelocity(NMP::maximum(weightFromAngularVelocity, weightFromTangentialVelocity));

          // 3. Determine the Pose blend weight by combining the linear and angular velocity blend weights with the
          //    orientation blend weight.
          poseWeightOutput = weightFromOrientation * weightFromVelocity * stateData->m_poseWeightMultiplier;

          // 4. Determine the Push from the linear and angular velocity blend weights
          const NMP::Vector3 pushAcceleration(calculatePushAcceleration(pushAccelerationMaxMagnitude, weightFromVelocity, slopeDirection, physicsRig));

          MR::PhysicsRigPhysX3::PartPhysX3* const pelvisPhysicsPart =
            static_cast< MR::PhysicsRigPhysX3::PartPhysX3* >(physicsRig->getPart(spineLimb.getRootIndex()));
          NMP_ASSERT(pelvisPhysicsPart);

          MR::addAccelerationToActor(*pelvisPhysicsPart->getRigidBody(), pushAcceleration);
          MR_DEBUG_DRAW_LINE(net->getDispatcher()->getDebugInterface(), pelvisPhysicsPart->getTransform().translation(), pushAcceleration, NMP::Colour::YELLOW);

          // 5 Determine the angular acceleration correction to align spine with step-horizontal
          const NMP::Vector3 spineAlignmentAcceleration(
            calculateSpineAlignementAcceleration( 
            slopeNormal, 
            slopeDirection, 
            stateData->m_sideRollAmount, 
            body,physicsRig,
            net->getDispatcher()->getDebugInterface()));
          MR::addAngularAccelerationToActor(*pelvisPhysicsPart->getRigidBody(), spineAlignmentAcceleration);

          // determine if character is rolling
          isRollingOutput = (weightFromVelocity >= stateData->m_minMotionRatioToBeConsideredRolling);
        }
        else
        {
          // the character is balanced - set the balance error s.t. it will become unbalanced
          const NMP::Vector3 COMVel(physicsRig->calculateCentreOfMassVelocity());

          // Use balance velocity error, so even if the character is on the slope and manages to get up, will fall again.
          const NMP::Vector3 leftDirection(vCross(slopeNormal, slopeDirection));

          bool onLeft = COMVel.dot(leftDirection) > 0.0f;
          NMP::Vector3 side = (onLeft) ? leftDirection * 0.5f : leftDirection * -0.5f;
          balanceErrorOutput = slopeDirection + side;
          balanceErrorOutput.fastNormalise();

          // character is not considered to be rolling if the balance amount is above the threshold
          isRollingOutput = false;
        }
      }
    }

    // Write to output CPs
    writeToOutputCPPin< MR::AttribDataFloat >(
      NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_ROLLPOSEANIMWEIGHT, poseWeightOutput, node, net);
    writeToOutputCPPin< MR::AttribDataVector3 >(
      NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_BALANCEDESIREDVELOCITY, balanceErrorOutput, node, net);
    writeToOutputCPPin< MR::AttribDataBool >(
      NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_ISROLLING, isRollingOutput, node, net);
  }

  // Return the value of the requested output cp pin.
  MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
