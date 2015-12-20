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
#include "myNetworkPackaging.h"
#include "ArmReachForBodyPartPackaging.h"
#include "ArmReachForBodyPart.h"
#include "ArmPackaging.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "euphoria/erLimbTransforms.h"
#include "euphoria/erSharedEnums.h"
#include "helpers/Reach.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// ArmReachForBodyPartUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPartUpdatePackage::update(
  float NMP_UNUSED(timeStep), 
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // The presence of a params input together with a reach-enabled flag signals reaching is on
  // if not we can just return.
  //
  if (in->getReachActionParamsImportance() != 0.0f && in->getReachEnabled())
  {
    data->reachActionParams = in->getReachActionParams();
  }
  else
  {
    data->targetImportance = 0.0f;
    return;
  }

  // Determine existence or otherwise of a reach target.
  data->targetImportance = in->getTargetImportance();

  // If we have a reach target then we want to know if it's a new one and/or whether the target limb is an arm (and if 
  // so the target arm's index).
  bool isNewReachTarget = false;
  bool isReachForArm = false;
  int32_t targetArmIndex = -1;
  if (data->targetImportance != 0.0f)
  {
    const ReachTarget& reachTarget = in->getTarget();

    isNewReachTarget = data->target.elapsedTime > reachTarget.elapsedTime;
    isReachForArm = reachTarget.type == ER::LimbTypeEnum::L_arm;
    if (isReachForArm)
    {
      targetArmIndex = reachTarget.limbRigIndex - owner->owner->data->firstArmRigIndex;
    }
    // Cache the reachtarget
    data->target = reachTarget;
  }

  // Reach for arm will be paused until the target arm is in position.
  // (i.e. when target arm end-effector is within a threshold distance from it's target)
  //
  // Threshold distance (of 150mm) in character reference units
  // (TODO: needs exposing, eg. via ReachParams)
  const float targetArmPoseDistanceThreshold = SCALE_DIST(0.15f);
  float targetArmDistanceFromPose = FLT_MAX;
  if (feedIn->getArmReachReactionDistanceFromTargetImportance() != 0.0f)
  {
    targetArmDistanceFromPose = feedIn->getArmReachReactionDistanceFromTarget();
  }

  bool delayReach = false;
  float readyImportance = 1.0f - data->armStrengthReduction;
  if (isReachForArm)
  {
    delayReach = (
      (data->state == ArmReachForBodyPartData::State_Waiting) &&
      (targetArmDistanceFromPose > targetArmPoseDistanceThreshold));
    if (delayReach)
    {
      float delayedReachImportance =
        NMP::clampValue(targetArmPoseDistanceThreshold / targetArmDistanceFromPose, 0.0f, 0.2f);
      readyImportance = NMP::minimum(delayedReachImportance, readyImportance);
    }
  }

  // If we have a target then we reach for it otherwise arm target is set to "ready" position
  //
  // "ready position" (use DefaultPose end)
  if (data->targetImportance == 0.0f || delayReach)
  {
    ER::HandFootTransform defaultPoseEndTMW =
      owner->owner->data->defaultPoseEndsRelativeToRoot[owner->data->limbNetworkIndex] *
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM;

    // Bake result into a control structure
    LimbControl& control = out->startControlModification();

    float stiffness = owner->data->normalStiffness * 1.5f;
    float dampingRatio = owner->data->normalDampingRatio * 1.5f;
    control.reinit(stiffness, dampingRatio, owner->data->normalDriveCompensation);

    // Apply speed limiting to the position
    float maxDisplacement = calcMaxDisplacement(data->reachActionParams.effectorSpeedLimit, dampingRatio, stiffness);
    NMP::Vector3 ikTgtPos(
      calcDisplacementLimitedTarget(
      defaultPoseEndTMW.translation(),
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
      maxDisplacement));

    control.setTargetPos(ikTgtPos, 1.0f);
    // Normal weighting ramps as effector gets closer to line of approach
    control.setTargetOrientation(defaultPoseEndTMW, 1.0f);
    control.setGravityCompensation(1.0f); 
    control.setControlAmount(ER::reachControl, 1.0f);
    out->stopControlModification(readyImportance);
    data->targetImportance = 0;

    MR_DEBUG_DRAW_POINT(pDebugDrawInst,
      defaultPoseEndTMW.translation(), SCALE_DIST(0.1f), NMP::Colour::RED); // desired tgt
    MR_DEBUG_DRAW_POINT(pDebugDrawInst,
      ikTgtPos, SCALE_DIST(0.1f), NMP::Colour::GREEN); // adjusted tgt
    MR_DEBUG_DRAW_POINT(pDebugDrawInst,
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
      SCALE_DIST(0.1f), NMP::Colour::WHITE); // effector
  }
  // Reach is on
  else
  {
    // If the cached target is older than the input target, input target is a new one so reset timers
    //
    if (isNewReachTarget)
    {
      // Zero the timers
      data->elapsedTime = 0.0f;
      data->elapsedTimeOnTarget = 0.0f;
      data->elapsedTimeToTarget = 0.0f;
      data->armStrengthReduction = 0.0f;
      data->state = ArmReachForBodyPartData::State_Reaching;
      // Store the position of the target when it first comes in (fixed reference value for body request calcs)
      data->referenceTargetRelSupport = data->target;
      // If in support then the target is stored in support frame coordinates
      if(in->getSupportTMImportance() > 0.0f)
      {
        in->getSupportTM().inverseTransformVector(data->target.point, data->referenceTargetRelSupport.point);
        in->getSupportTM().inverseRotateVector(data->target.normal, data->referenceTargetRelSupport.normal);
      }
    }
    // Otherwise, we have no new target but need to check if we're done with the cached one
    // and return without further ado if so
    else
    {
      if (data->state == ArmReachForBodyPartData::State_Done)
        return;
    }

    // No reach for self except if target is root part
    //
    int32_t limbRigIndex = owner->data->limbRigIndex;
    if (data->target.limbRigIndex == limbRigIndex && data->target.partIndex != 0)
    {
      data->targetImportance = 0.0f;
    }
    // Reaching is on
    else
    {
      // Register if character is currently balancing - used in feedback for full body motion
      // We need both support amount and the associated support tm, we can have smallish support amount and invalid tm
      // but not support tm importance with invalid tm (we assert on this in feedback()).
      data->supportAmount =
        (in->getSupportAmountImportance() && in->getSupportTMImportance() > 0.0f)
          ? in->getSupportAmount() : 0.0f;

      float limbLength = owner->data->length;
      float tgtAdjustment = 0.0f;

      // Target input adjusted below
      NMP::Vector3 ikTgt = data->target.point;
      // Approach vector adjusted by angle adjustment calc below
      NMP::Vector3 targetApproach(owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.getReachConeDir());
      // Angular error as computed by the angle adjustement calc (used in normal weighting calc)
      float angularError = 0.0f;
      // Distance error (calculated later, allow the reach for arm code to run before we do that).
      float effectorToTargetDistanceSquared = FLT_MAX;
      const float outOfReachDistanceSq = 
        data->reachActionParams.outOfReachDistance * data->reachActionParams.outOfReachDistance;

      // Arm reaches do not work directly with the input target/normal but the projection of the input target
      // onto a bone, the normal is also generated procedurally from the difference between effector and target.
      if (isReachForArm)
      {
        // Retrieve arm joints for target projection calc
        // This needs to be done direcly with part data (when MORPH-22200 is done)
        //
        const ArmAndLegLimbSharedState& targetLimbState = owner->owner->data->armLimbSharedStates[targetArmIndex];

        NMP::Vector3 armJointPositions[NetworkConstants::rigMaxNumPartsPerLimb];

        // Iterate over parts between root and end (not including root so first part is the clavicle)
        for (int32_t iPart = 1; iPart < targetLimbState.m_numPartsInChain; ++iPart)
        {
          armJointPositions[iPart - 1] = targetLimbState.m_partTMs[iPart].translation();
        }
        armJointPositions[targetLimbState.m_numPartsInChain - 1] = targetLimbState.m_endTM.translation();

        // Calc target projection (onto the nearest bone).
        //
        NMP::Vector3 boneDir;
        NMP::Vector3 armIKTgt;
        float diffMinSq = FLT_MAX;
        const int32_t lastJoint = targetLimbState.m_numPartsInChain - 1;
        for (int32_t j = 0; j < lastJoint; ++j)
        {
          MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
            armJointPositions[j], armJointPositions[j+1] - armJointPositions[j], NMP::Colour::LIGHT_BLUE);

          float t;
          NMP::Vector3 pointOnBone =
            NMRU::GeomUtils::getPointProjectedOntoLine(
            armJointPositions[j], armJointPositions[j+1], data->target.point, &t);

          // Clamp the point on bone target so it remains on a bone
          if (t < 0.0f)
          {
            pointOnBone = armJointPositions[j];
          }
          else if (t > 1.0f)
          {
            pointOnBone = armJointPositions[j+1];
          }

          float diffTgtMagSq = (data->target.point - pointOnBone).magnitudeSquared();
          if (diffMinSq > diffTgtMagSq)
          {
            diffMinSq = diffTgtMagSq;
            armIKTgt = pointOnBone;
            boneDir = NMP::vNormalise(armJointPositions[j] - armJointPositions[j+1]);

            // If we hit this assert then we have a zero length bone, and vNormalise will have returned a somewhat 
            // meaningless default unit direction this is not something we envisage seeing but could be dealt with if 
            // required for eg. by just ignoring such bones. For now just an assert to watch for this case.
            NMP_ASSERT((armJointPositions[j] - armJointPositions[j+1]).magnitudeSquared() > SCALE_DIST(0.001f));
          }
        }

        // Store result for further filtering.
        ikTgt = armIKTgt;
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, ikTgt, SCALE_DIST(1.2f), NMP::Colour::YELLOW);
        
        // Calculate difference between current end effector position and target.
        NMP::Vector3 effectorToTarget = ikTgt 
          - owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation();
        effectorToTargetDistanceSquared = effectorToTarget.magnitudeSquared();

        // Use the above result to generate a target normal while on the way to the target
        // and save the result in chest local coordinates
        if (effectorToTargetDistanceSquared > outOfReachDistanceSq || isNewReachTarget)
        {
          // The normal is the component of the reach error orthogonal to the bone.
          data->target.normal = -effectorToTarget.getComponentOrthogonalToDir(boneDir);
          data->target.normal.normalise();

          // Store the normal just generated in chest frame coordinates, once the hand is on target we'll stick with it.
          owner->owner->data->spineLimbSharedStates[0].m_endTM.inverseRotateVector(
            data->target.normal, data->cachedTargetNormalLocal);
          
          MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
            ikTgt, data->target.normal * SCALE_DIST(1.0f), NMP::Colour::LIGHT_TURQUOISE);
        }
        // Unpack the chest local normal for once the reach is achieved.
        else
        {
          owner->owner->data->spineLimbSharedStates[0].m_endTM.rotateVector(
            data->cachedTargetNormalLocal, data->target.normal);
          MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
            ikTgt, 2.0f * data->target.normal * SCALE_DIST(1.0f), NMP::Colour::TURQUOISE);
        }
      }

      // Arm avoidance for arm reaching case.
      // MORPH-16413: Need to verify/adjust arm avoidance to work on characters with more than two arms.
      bool twoArmedCharacter;
      twoArmedCharacter = NetworkConstants::networkMaxNumArms == 2;
      if ( twoArmedCharacter && (isReachForArm && ! (data->state == ArmReachForBodyPartData::State_Holding)))
      {
        // Define two planes: 
        // the "reach arm plane" defined by the triangle of reaching arm shoulder, end effector and target
        // the "target arm plane" defined by the triangle of target arm shoulder, elbow and wrist
        //
        // Two heuristics/tests:
        // 1 the target arm wrist must be below the reach arm plane
        // if not 1 then
        //  2 the projection of the reaching arm end effector onto the target arm plane must be on the inside of the
        // triangle edges that meet at the elbow

        // Reach arm plane (shoulder, end effector, target)
        NMP::Vector3 reachArmTri[3];
        reachArmTri[0] = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition;
        reachArmTri[1] = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation();
        reachArmTri[2] = ikTgt;

        // Target arm plane (shoulder, wrist, elbow)
        NMP::Vector3 tgtArmTri[3];
        tgtArmTri[0] = owner->owner->data->armLimbSharedStates[targetArmIndex].m_basePosition;
        tgtArmTri[1] = owner->owner->data->armLimbSharedStates[targetArmIndex].m_endTM.translation();
        tgtArmTri[2] = owner->owner->data->armLimbSharedStates[targetArmIndex].m_midLimbPartPosition;

        bool entangled = false;

        // Test 1:
        // target arm wrist is not in front of the plane swept by the reaching arm
        bool inFront;
        inFront = pointInFrontOfTriangle(tgtArmTri[1], reachArmTri[0], reachArmTri[1], reachArmTri[2]);
        // inFrontness needs to flip for a right arm reach
        ER::HandFootTransform defaultPoseEndTM =
          owner->owner->data->defaultPoseEndsRelativeToRoot[owner->data->limbNetworkIndex];
        bool isLeftArm;
        isLeftArm = defaultPoseEndTM.translation().z < 0;
        if (!isLeftArm)
        {
          inFront = !inFront;
        }
        // Test 2 applied iff not 1
        if (!inFront)
        {
          // The outcode bits say which triangle edges the wrist is outside/inside 
          uint32_t outCode = 0;
          pointInTrianglePrism(reachArmTri[1], tgtArmTri[0], tgtArmTri[1], tgtArmTri[2], outCode);

          // Hijack the ik target if required, so as to back off reaching in the direction of the shoulder.
          if (outCode & 6) // (2nd and/or 3rd bits set <==> beyond target arm elbow)
          {
            ikTgt = (reachArmTri[1] + reachArmTri[0]) * 0.5f;
            entangled = true;
          }
        }

        // Some debug output to show what arm avoidance was thinking..
        // (technicolour if no problem otherwise drab grey)
        if (!entangled)
        {
          MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, reachArmTri[0], reachArmTri[1], reachArmTri[2], NMP::Colour::RED);
          MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, tgtArmTri[0], tgtArmTri[1], tgtArmTri[2], NMP::Colour::PURPLE);
        }
        else
        {
          MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, reachArmTri[0], reachArmTri[1], reachArmTri[2], NMP::Colour::LIGHT_RED);
          MR_DEBUG_DRAW_TRIANGLE(pDebugDrawInst, tgtArmTri[0], tgtArmTri[1], tgtArmTri[2], NMP::Colour::LIGHT_GREY);
        }
      }

#if defined(MR_OUTPUT_DEBUGGING)
      NMP::Vector3 angleLimitedTgt = ikTgt; // Cache for debug viz (below)
#endif
      float distanceFromTargetSq = 
        (ikTgt - owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation()).magnitudeSquared();
      const float spineAvoidanceDistSqCutoff = SCALE_DIST( 0.15f ) * SCALE_DIST( 0.15f );
      if ( distanceFromTargetSq > spineAvoidanceDistSqCutoff )
      {
        ikTgt = owner->avoidSpineUsingAngleCoefficient(
          ikTgt, 
          owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation(),
          NM_PI / 60.0f,
          pDebugDrawInst,
          &angularError,
          &targetApproach);
      }

      // Slides the target up the approach normal
      ikTgt = reachApproachAdjustedTarget(
        ikTgt,
        data->target.normal,
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
        targetApproach,
        data->reachActionParams.penetrationAdjustment,
        owner->data->length / 2.0f,
        &tgtAdjustment);

      // Speed limiting
      // If this code is allowed to run after the above, then it is quite possible that it undoes
      // some of the good work by producing a target that e.g. lies inside the leg. However,
      // we have yet to encounter significant use cases where the issue arises.
      //

      // Speed limiting calc below needs the control values we'll be using later for the LimbControl so we do them here.
      float dampingRatio = owner->data->normalDampingRatio;
      float stiffness = owner->data->normalStiffness * (1.0f - data->armStrengthReduction) * 2.0f;
      float driveCompensation = owner->data->normalDriveCompensation * 2.0f;

      // Apply speed limiting to the displacement.
      float maxDisplacement = calcMaxDisplacement(data->reachActionParams.effectorSpeedLimit, dampingRatio, stiffness);
      ikTgt = calcDisplacementLimitedTarget(
        ikTgt,
        owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation(),
        maxDisplacement);

      // Build framework for swivel heuristic
      //

      // spineUpDir
      NMP::Vector3 chestPos = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_rootTM.translation();
      NMP::Vector3 pelvisPos = owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation();
      NMP::Vector3 spineUpVec = chestPos - pelvisPos;
      NMP::Vector3 spineUpDir(spineUpVec);
      spineUpDir.normalise();

      // shoulderToSpineLineOfClosestApproach and direction and magnitude
      NMP::Vector3 shoulderPos
        = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation();
      NMP::Vector3 shoulderToSpineLoca(chestPos - shoulderPos);
      shoulderToSpineLoca -= (spineUpDir.dot(shoulderToSpineLoca) * spineUpDir);
      float shoulderToSpineDistance = shoulderToSpineLoca.magnitude();
      NMP::Vector3 shoulderToSpineLocaDir(shoulderToSpineLoca);
      shoulderToSpineLocaDir.normalise();
      NMP::Vector3 shoulderPointOnSpine = shoulderPos + shoulderToSpineLoca;

      // Spine to target projection (target is projected onto a cylinder with axis the spine up dir
      // and radius the shoulder to spine distance)
      NMP::Vector3 spineToTargetLoca(ikTgt - chestPos);
      spineToTargetLoca -= (spineUpDir.dot(spineToTargetLoca) * spineUpDir);
      NMP::Vector3 pointOnSpine = (ikTgt - spineToTargetLoca);
      spineToTargetLoca.normalise();
      spineToTargetLoca *= shoulderToSpineDistance;
      NMP::Vector3 targetProjection = shoulderPointOnSpine + spineToTargetLoca;

      // shoulderToTarget
      NMP::Vector3 shoulderToTargetProjection(targetProjection - shoulderPos);
      float shoulderToTargetProjectionDotShoulderToSpineLocaDir =
        shoulderToTargetProjection.dot(shoulderToSpineLocaDir);

#if defined(MR_OUTPUT_DEBUGGING)
      static bool drawOriginalOutput = false;

      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
        MR::VT_Delta, shoulderPos, targetApproach, NMP::Colour::YELLOW);

      if (drawOriginalOutput)
      {

        NMP::Vector3 testDotPoint =
          shoulderPos + shoulderToSpineLocaDir * shoulderToTargetProjectionDotShoulderToSpineLocaDir;
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, testDotPoint, SCALE_DIST(0.03f), NMP::Colour::RED);
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, shoulderPos, SCALE_DIST(0.03f), NMP::Colour::YELLOW);
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, shoulderPointOnSpine, SCALE_DIST(0.03f), NMP::Colour::YELLOW);
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, shoulderPos, testDotPoint - shoulderPos, NMP::Colour::YELLOW);

        MR_DEBUG_DRAW_POINT(pDebugDrawInst, pointOnSpine, SCALE_DIST(0.1f), NMP::Colour::WHITE);
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, targetProjection, SCALE_DIST(0.1f), NMP::Colour::WHITE);
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
          MR::VT_Delta, shoulderPointOnSpine, targetProjection - shoulderPointOnSpine, NMP::Colour::WHITE);
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
          MR::VT_Delta, shoulderPos, shoulderPointOnSpine - shoulderPos, NMP::Colour::YELLOW);

        // Draw the spine
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, pelvisPos, spineUpVec, NMP::Colour::DARK_PURPLE);

        MR_DEBUG_DRAW_POINT(pDebugDrawInst, ikTgt, SCALE_DIST(0.1f), NMP::Colour::GREEN);
      }
      else
      {
        // Draw:
        // The original target input position and normal
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, data->target.point, SCALE_DIST(0.05f), NMP::Colour::RED);
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst,
          MR::VT_Delta, data->target.point, SCALE_DIST(data->target.normal), NMP::Colour::RED);
        // the final adjusted target
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, ikTgt, SCALE_DIST(0.1f), NMP::Colour::GREEN);
        // adjusted reach approach
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, angleLimitedTgt,
          SCALE_DIST(targetApproach), NMP::Colour::GREEN);

        MR_DEBUG_DRAW_POINT(pDebugDrawInst, shoulderPos, SCALE_DIST(0.03f), NMP::Colour::YELLOW);
        MR_DEBUG_DRAW_POINT(pDebugDrawInst, chestPos, SCALE_DIST(0.03f), NMP::Colour::YELLOW);
      }
#endif // defined(MR_OUTPUT_DEBUGGING)

      // Default to zero..
      float swivelAmount = 0.0f;
      // ..but if reach action params has importance
      if (in->getReachActionParamsImportance() != 0.0f)
      {
        // and swivel is user specified
        if ( data->reachActionParams.swivelMode == ER::REACH_SWIVEL_SPECIFIED)
        {
          swivelAmount = data->reachActionParams.swivelAmount;
        }
        // or choose swivel amount from "body reach heuristic"
        else if ( data->reachActionParams.swivelMode == ER::REACH_SWIVEL_AUTOMATIC)
        {
          // "body reach heuristic"
          // First by height of target, then
          // if height is above shoulder or below shoulder but "above elbow"
          // (denoted by half arm length)
          //  scale swivel on the height offset
          // otherwise
          //  scale swivel on the lateral offset
          const float halfArmLength = limbLength * 0.5f;
          float tgtToShoulder = (shoulderPos - ikTgt).dot(spineUpDir);
          // above shoulder
          if (tgtToShoulder < 0.0f)
          {
            swivelAmount = -0.5f;
          }
          // Above half arm length
          else if (tgtToShoulder < halfArmLength)
          {
            swivelAmount = -0.5f * ((halfArmLength - tgtToShoulder) / halfArmLength);
          }
          // Below half arm length
          else
          {
            swivelAmount =
              (shoulderToTargetProjectionDotShoulderToSpineLocaDir - shoulderToSpineDistance) / shoulderToSpineDistance;
            if (swivelAmount > 1.0f)
            {
              swivelAmount = 1.0f;
            }
            else if (swivelAmount < -1.0f)
            {
              swivelAmount = -1.0f;
            }
          }
        }
      }

      // Bake result into a control structure
      LimbControl& control = out->startControlModification();
      control.reinit(stiffness, dampingRatio, driveCompensation);

      // Limit arm maximum extension by clamping the distance between arm base and reach target.
      data->maxArmExtension = owner->data->length * data->reachActionParams.maxLimbExtensionScale;
      // Clamp the distance between arm base and reach target.
      const NMP::Vector3 armBase = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_basePosition;
      NMP::Vector3 toEnd = ikTgt - armBase;
      toEnd.clampMagnitude(data->maxArmExtension);
      ikTgt = armBase + toEnd;

      control.setTargetPos(ikTgt, 1);
      // normal weighting ramps as effector gets closer to line of approach
      float normalWeight = 1.0f - NMP::clampValue(NMP::nmfabs(angularError / NM_PI_OVER_TWO), 0.0f, 1.0f);
      control.setTargetNormal(data->target.normal, normalWeight);
      control.setGravityCompensation(data->armStrengthReduction > 0.0f ? 0.0f : 1.0f);
      control.setControlAmount(ER::reachControl, 1.0f);
      control.setSwivelAmount(swivelAmount);
      // Disable collisions until we're within the specified target radius
      if (effectorToTargetDistanceSquared > outOfReachDistanceSq)
      {
        control.setCollisionGroupIndex((float) data->reachActionParams.handsCollisionGroupIndex);
      }

      // and write to output
      out->stopControlModification(data->targetImportance);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPartFeedbackPackage::feedback(
  float timeStep,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // No action for targets of zero importance except internal reset.
  //
  if (data->targetImportance == 0.0f)
  {
    // Elapsed time resets
    data->target.elapsedTime = FLT_MAX;
    data->elapsedTime = 0.0f;
    data->elapsedTimeOnTarget = 0.0f;
    data->elapsedTimeToTarget = 0.0f;
    
    // Behaviour state reset two cases: reach action pending or done.
    // 
    if (in->getReachActionParamsImportance() > 0.0f && in->getReachEnabled())
    {
      data->state = ArmReachForBodyPartData::State_Waiting;
      feedOut->setCurrentReachTarget(data->target); // dummy target to fool reach reaction into doing something
    }
    else
    {
      data->state = ArmReachForBodyPartData::State_Done;
    }

    // Nothing more to do except feedback state and return.
    //
    feedOut->setState(data->state);
    return;
  }

  // If we got this far and are in an active reach state then determine basic feedout values (effector-target error,
  // reach strength and ik target). If reach strength goes to zero in the next block we'll set state "done" and return.
  //
  if (data->state != ArmReachForBodyPartData::State_Done)
  {
    const NMP::Vector3& endEffectorPos(
      owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_endTM.translation());
    const NMP::Vector3& targetPos = data->target.point;
    float endEffectorToTargetDistanceSquared = (targetPos - endEffectorPos).magnitudeSquared();

    // Update the timers and the state
    //
    data->elapsedTime += timeStep;
    if ((data->state != ArmReachForBodyPartData::State_CompletedRampdown) &&
      (data->state != ArmReachForBodyPartData::State_FailedRampdown))
    {
      if (endEffectorToTargetDistanceSquared > 
          data->reachActionParams.outOfReachDistance * data->reachActionParams.outOfReachDistance)
      {
        data->elapsedTimeOnTarget = 0.0f;
        data->elapsedTimeToTarget += timeStep;
        data->state =
          (data->elapsedTimeToTarget < data->reachActionParams.outOfReachTimeout)
          ? ArmReachForBodyPartData::State_Reaching : ArmReachForBodyPartData::State_FailedRampdown;
      }
      else
      {
        data->elapsedTimeOnTarget += timeStep;
        data->state =
          (data->elapsedTimeOnTarget < data->reachActionParams.withinReachTimeout)
          ? ArmReachForBodyPartData::State_Holding : ArmReachForBodyPartData::State_CompletedRampdown;
      }
    }

    // Set rampdown weakness if required
    //
    float internalRampdownStrengthReduction = 0.0f;
    if (data->state == ArmReachForBodyPartData::State_FailedRampdown)
    {
      float rampDownTime = (data->elapsedTimeToTarget - data->reachActionParams.outOfReachTimeout);
      internalRampdownStrengthReduction =
        NMP::minimum(rampDownTime / data->reachActionParams.rampDownFailedDuration, 1.0f);
      data->elapsedTimeToTarget += timeStep;
    }
    else if (data->state == ArmReachForBodyPartData::State_CompletedRampdown)
    {
      float rampDownTime = (data->elapsedTimeOnTarget - data->reachActionParams.withinReachTimeout);
      internalRampdownStrengthReduction =
        NMP::minimum(rampDownTime / data->reachActionParams.rampDownCompletedDuration, 1.0f);
      data->elapsedTimeOnTarget += timeStep;
    }

    // Compute current reachStrength from max of internal and external strength reductions
    //
    float maxStrengthReduction =
      NMP::maximum(1.0f - data->reachActionParams.strength, internalRampdownStrengthReduction);
    data->armStrengthReduction = NMP::clampValue(maxStrengthReduction, 0.0f, 1.0f);
    const float reachStrength = 1.0f - data->armStrengthReduction;
    NMP_ASSERT(0.0f <= reachStrength && reachStrength <= 1.0f);

    // If, after the above reach-strength calcs we still have strength, then set the relevant feedouts and proceed with 
    // full-body cals, otherwise set and feedout state "done" and return.
    //
    NMP_ASSERT(0.0f <= reachStrength && reachStrength <= 1.0f);
    if (reachStrength > 0.0f)
    {
      // Status feedback
      //
      feedOut->setDistanceSquaredToTarget(endEffectorToTargetDistanceSquared, reachStrength);
      if (reachStrength < 1.0f)
      {
        feedOut->setArmStrengthReduction(data->armStrengthReduction, 1.0f);
      }
      feedOut->setCurrentReachTarget(data->target, NMP::clampValue(reachStrength, 0.0f, 1.0f));
    }
    else // reachStrength is 0 - we're done
    {
      data->state = ArmReachForBodyPartData::State_Done;
      feedOut->setState(data->state);
      return;
    }

    // Full body motion requests ///////////////////////////////////////////////////////////////////////////////////////

    // Full body reach support motion (is only required for leg and spine reaches when in support).
    //
    bool isReachForSpine = data->target.type == ER::LimbTypeEnum::L_spine;
    bool isReachForLeg = data->target.type == ER::LimbTypeEnum::L_leg;
    if ((data->supportAmount == 0.0f) || !(isReachForLeg || isReachForSpine))
    {
      feedOut->setState(data->state);
      return;
    }

    // Framework on which to hang our requests
    //
    // Obtain support tm and up and forward directions.
    //
    const ER::LimbTransform& supportTM = in->getSupportTM();
    const NMP::Vector3& up(supportTM.upDirection());
    const NMP::Vector3& fwd(supportTM.frontDirection());

    // Calc reference shoulder and target heights
    NMP::Vector3 referenceTargetPosWorld;
    supportTM.matrix().transformVector(data->referenceTargetRelSupport.point, referenceTargetPosWorld);
    const float referenceTargetHeight = referenceTargetPosWorld.dot(up);

    const float referenceShoulderHeight = supportTM.translation().dot(up)
      + (owner->owner->data->bodyLimbSharedState.m_averageOfDefaultPoseShouldersTMWorld.translation()
      - owner->owner->data->bodyLimbSharedState.m_averageOfDefaultPoseFeetTMWorld.translation()).dot(up);

    // Calc required shoulder drop to bring the reference shoulder within reach of the reference target height
    const float desiredShoulderHeight = referenceTargetHeight + data->maxArmExtension * 0.8f;
    float shoulderDropFromUpright =  referenceShoulderHeight - desiredShoulderHeight;
    
    // Calc forwards component of the chest motion request (simple function of the total drop from upright).
    float desiredChestForwardFromUpright = NMP::maximum(2.0f * shoulderDropFromUpright, 0.0f);
    float chestForward = (owner->owner->data->spineLimbSharedStates[0].m_endTM.translation() -
      supportTM.translation()).dot(fwd);
    float desiredShoulderForwardMotion(desiredChestForwardFromUpright - chestForward);

    // Calc downwards component of the chest motion request from difference between current and desired shoulder height.
    const float shoulderHeight 
      = owner->owner->data->armLimbSharedStates[owner->data->childIndex].m_reachLimit.m_transform.translation().dot(up);
    // Limit upward speed
    float verticalDisplacement = desiredShoulderHeight - shoulderHeight;
    if (verticalDisplacement > 0.0f)
    {
      // Speed limiting calc below needs the control values.
      float dampingRatio = owner->data->normalDampingRatio;
      float stiffness = owner->data->normalStiffness * (1.0f - data->armStrengthReduction) * 2.0f;

      // Apply speed limiting to any upwards displacement (should really be an accel limit).
      const float maxSpeed = SCALE_VEL(0.75f);
      float maxDisplacement = calcMaxDisplacement(maxSpeed, dampingRatio, stiffness);
      verticalDisplacement = NMP::minimum(verticalDisplacement, maxDisplacement);
    }
    const float desiredShoulderVerticalMotion =  verticalDisplacement;

    // Chest translation request
    TranslationRequest chestTranslation(
      up * desiredShoulderVerticalMotion + fwd * desiredShoulderForwardMotion,
      SCALING_SOURCE);
    chestTranslation.imminence = SCALE_FREQUENCY(2.0f);
    chestTranslation.passOnAmount = 1.0f;
#if defined(MR_OUTPUT_DEBUGGING)
    chestTranslation.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
    feedOut->setChestTranslationRequest(chestTranslation, reachStrength);

    // Set downwards component of the pelvis motion request, equal to the shoulder vertical motion.
    const float desiredPelvisVerticalMotion = desiredShoulderVerticalMotion;

    NMP::Vector3 desiredPelvisTranslation(0.0f, desiredPelvisVerticalMotion, 0.0f);
    TranslationRequest pelvisTranslation(desiredPelvisTranslation, SCALING_SOURCE);
    pelvisTranslation.imminence = SCALE_FREQUENCY(2.0f);
    pelvisTranslation.passOnAmount = 1.0f;
#if defined(MR_OUTPUT_DEBUGGING)
    pelvisTranslation.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
    feedOut->setPelvisTranslationRequest(pelvisTranslation, reachStrength);

    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta,
      owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
      chestTranslation.translation * 10.0f, NMP::Colour::LIGHT_PURPLE);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, supportTM, SCALE_DIST(1.0f));

    // Chest direction calculations:
    //
    // The request is for a twist towards the target about the current spine up direction.
    // 
    // The desired chest forward direction is computed to lie somewhere between two directions: support 
    // forward and the current direction of the reference target (relative to the current support). This offset, gives
    // the basis for a "total twist" which is then scaled to have the character contort more towards targets further
    // down the body and not at all for targets at or above the chest.
    //
    
    // Reference chest forward is just the forward direction of the support tm ("chest facing front").
    NMP::Vector3 referenceChestForward = supportTM.frontDirection();

    // Calculate target bearing in the horizontal (spine up plane).
    NMP_ASSERT(supportTM.isValidTM(0.01f));
    NMP::Vector3 referenceTargetBearing = referenceTargetPosWorld - supportTM.translation();
    referenceTargetBearing = referenceTargetBearing.getComponentOrthogonalToDir(supportTM.upDirection());
    referenceTargetBearing.normalise();

    // Determine twist scale from target projection onto spine up line.
    //
    NMP::Vector3 spineUp(owner->owner->data->spineLimbSharedStates[0].m_endTM.translation() -
      owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation());
    spineUp.normalise();
    float twistScale = 0.0f;
#if defined(MR_OUTPUT_DEBUGGING)
    NMP::Vector3 pointOnSpineUp =
#endif
    NMRU::GeomUtils::getPointProjectedOntoLine(
      owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
      owner->owner->data->spineLimbSharedStates[0].m_endTM.translation() -
      spineUp * referenceShoulderHeight, targetPos,
      &twistScale);
    twistScale = NMP::clampValue(twistScale, 0.0f, 1.0f);

    // Calculate the required rotation from reference chest forward (i.e. support forward) to target.
    //
    // 1 get the total offset between reference forward and target bearing
    NMP::Quat qChestToTargetTwist;
    qChestToTargetTwist.forRotation(referenceChestForward, referenceTargetBearing);
    // 2 convert this to a rotation vector scaling the result by the twistScale computed above
    NMP::Vector3 referenceTwist = twistScale * qChestToTargetTwist.toRotationVector();
    // 3 convert this back to a quat and apply it to the reference forward to obtain desired
    NMP::Quat qRotation;
    qRotation.fromRotationVector(referenceTwist);
    NMP::Vector3 desiredChestForward = qRotation.rotateVector(referenceChestForward);
    // 4 project result into the spineUp plane to have twist applied only about the spine
    desiredChestForward = desiredChestForward.getComponentOrthogonalToDir(spineUp);
    desiredChestForward.normalise();

#if defined(MR_OUTPUT_DEBUGGING)
    
    float lengthScale = SCALE_DIST(1.0f);

    MR_DEBUG_DRAW_VECTOR( pDebugDrawInst,
      MR::VT_Normal, owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
      desiredChestForward, NMP::Colour::RED);
    MR_DEBUG_DRAW_POINT( pDebugDrawInst, pointOnSpineUp, lengthScale, NMP::Colour::GREEN);
    MR_DEBUG_DRAW_POINT( pDebugDrawInst, referenceTargetPosWorld, lengthScale, NMP::Colour::WHITE);
    MR_DEBUG_DRAW_VECTOR( pDebugDrawInst,
      MR::VT_Normal, owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
      lengthScale * referenceTargetBearing, NMP::Colour::RED);

#endif

    // Set the request.
    //
    DirectionRequest chestDirRequest(NMP::Vector3::InitOneX, desiredChestForward, SCALING_SOURCE);
    chestDirRequest.imminence = SCALE_FREQUENCY(2.0f);
#if defined(MR_OUTPUT_DEBUGGING)
    chestDirRequest.debugControlAmounts.setControlAmount(ER::reachControl, 1.0f);
#endif
    {
      feedOut->setChestDirectionRequest(chestDirRequest, reachStrength);
    }
  }
  feedOut->setState(data->state);
}

//----------------------------------------------------------------------------------------------------------------------
void ArmReachForBodyPart::entry()
{
  data->elapsedTime = 0.0f;
  data->armStrengthReduction = 0.0f;
  data->maxArmExtension = 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

