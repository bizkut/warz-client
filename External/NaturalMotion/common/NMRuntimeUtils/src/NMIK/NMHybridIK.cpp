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
#include "NMIK/NMHybridIK.h"
#include "NMIK/NMTwoBoneIK.h"
//----------------------------------------------------------------------------------------------------------------------

// Tuning and performance settings local to HybridIK
#define HYBRIDIK_QUIT_WHEN_STOPPEDx
#define HYBRIDIK_QUIT_WHEN_ACCURATEx
#ifndef NMIK_NO_TERTIARIES
  #define HYBRIDIK_TERTIARIES_ONLY_ARG(x) x
#else
  #define HYBRIDIK_TERTIARIES_ONLY_ARG(x)
#endif

//----------------------------------------------------------------------------------------------------------------------
// Debug draw macros.  These are macros simply to allow them to be called in place
// without the need to pass arguments.  Debug draw is very inefficient and should probably
// never be used except in internal builds.
#define JOINT_FRAME_SIZE 0.04f
#ifdef NMRU_HYBRIDIK_DEBUGDRAW_JOINT_FRAMES
  #define DEBUG_DRAW_JOINT_FRAME \
  debugBuffer->drawPosQuatFrame(nextT, JOINT_FRAME_SIZE);
#else
  #define DEBUG_DRAW_JOINT_FRAME {}
#endif

#ifdef NMRU_HYBRIDIK_DEBUGDRAW
  #define SET_DEBUG_TAG(j) \
  if (debugBuffer) \
    debugBuffer->tag = j;
  #define INCREMENT_DEBUG_TAG() \
  if (debugBuffer) \
    ++(debugBuffer->tag);
int32_t g_startDebugTag = 0;
  #define SET_START_DEBUG_TAG() \
  if (debugBuffer) \
    g_startDebugTag = debugBuffer->tag;
  #define SET_DEBUG_TAG_TO_START() \
  if (debugBuffer) \
    debugBuffer->tag = g_startDebugTag;
#else
  #define SET_DEBUG_TAG(j) {}
  #define INCREMENT_DEBUG_TAG() {}
  #define SET_START_DEBUG_TAG() {}
  #define SET_DEBUG_TAG_TO_START() {}
#endif

#define HINGE_AXIS_SIZE 0.1f
#ifdef NMRU_HYBRIDIK_DEBUGDRAW_HINGES
  #define DEBUG_DRAW_HINGES \
  if (params->perJointParams[jidd].isHinge) \
  { \
    NMP::Vector3 axis = prevT.q.rotateVector(params->perJointParams[jidd].hingeAxis); \
    debugBuffer->drawLine(nextT.t, nextT.t + axis*HINGE_AXIS_SIZE); \
  }
#else
  #define DEBUG_DRAW_HINGES {}
#endif

#ifdef NMRU_HYBRIDIK_DEBUGDRAW_POLEVECTORS
  #define DEBUG_DRAW_POLEVECTORS(j) \
  if (params->endEffectorPoleVectorParams.active) \
  { \
    GeomUtils::PosQuat endEffectorT = globalTransform ? *globalTransform : GeomUtils::identityPosQuat(); \
    GeomUtils::PosQuat myJointT; \
    for (uint32_t jj = 0; jj < numJoints; ++jj) \
    { \
      const uint32_t& jji = jointIndices[jj] \
      GeomUtils::premultiplyPosQuat(joints[jji], endEffectorT); \
      if ((signed)jj == (signed)j) \
        myJointT = endEffectorT; \
    } \
    NMP::Vector3 referenceVector = target.q.rotateVector(params->endEffectorPoleVectorParams.referencePoleVector); \
    NMP::Vector3 controlVector = endEffectorT.q.rotateVector(params->endEffectorPoleVectorParams.controlPoleVector); \
    debugBuffer->drawDashedLine(myJointT.t, myJointT.t + (controlVector * 0.1f), 0.04f); \
    debugBuffer->drawDashedLine(myJointT.t, myJointT.t + (referenceVector * 0.2f), 0.04f); \
  }
#else
  #define DEBUG_DRAW_POLEVECTORS(j) {}
#endif

#define LIMIT_SIZE 0.05f
#define LIMIT_DIAL_SIZE 0.06f
#define LIMIT_SEGMENTS 5
#ifdef NMRU_HYBRIDIK_DEBUGDRAW_LIMITS
  #define DEBUG_DRAW_LIMITS \
  if (params->perJointParams[jidd].isLimited) \
  { \
    JointLimits::Params& limits = params->perJointParams[jidd].limits; \
    \
    /* Twist limits */ \
    NMP::Quat jointQ; \
    JointLimits::toInternal(limits, joints[jidd].q, jointQ); \
    NMP::Quat swingQ; \
    GeomUtils::separateSwingFromTwistAndSwing(jointQ, swingQ); \
    swingQ = prevT.q * limits.frame * swingQ; \
    NMP::Vector3 startPoint = nextT.t; \
    float limitSize = LIMIT_SIZE; \
    NMP::Vector3 limitColour = GeomUtils::DebugDraw::defaultColour; \
    /* Limit goes red if violated */ \
    if (JointLimits::degreeOfViolation(limits, joints[jidd].q) > 1.0f) \
    { \
      limitColour = NMP::Vector3(1.0f, 0, 0, 1.0f); \
    } \
    NMP::Vector3 twistDial(0, limitSize, 0); \
    float twistRange = limits.upper.x - limits.lower.x; \
    float stepSize = twistRange / LIMIT_SEGMENTS; \
    for (uint32_t limitI = 0; limitI <= LIMIT_SEGMENTS; ++limitI) \
    { \
      float t = limits.lower.x + (stepSize * limitI); \
      NMP::Vector3 twist(t, 0, 0); \
      NMP::Quat twistQ; \
      GeomUtils::twistSwingToQuat(twist, twistQ); \
      twistQ = swingQ * twistQ; \
      NMP::Vector3 nextPoint = nextT.t + twistQ.rotateVector(twistDial); \
      debugBuffer->drawLine(startPoint, nextPoint, limitColour); \
      startPoint = nextPoint; \
    } \
    debugBuffer->drawLine(startPoint, nextT.t, limitColour); \
    /* Twist dial */ \
    twistDial = (joints[jidd].q * ~limits.offset).getYAxis() * LIMIT_DIAL_SIZE; \
    NMP::Vector3 nextPoint = nextT.t + prevT.q.rotateVector(twistDial); \
    debugBuffer->drawLine(nextT.t, nextPoint, limitColour); \
    \
    /* Swing limits */ \
    if (!params->perJointParams[jidd].isHinge) \
    { \
      NMP::Vector3 swingDial(LIMIT_SIZE, 0, 0); \
      NMP::Vector3 startPoint1 = nextT.t; \
      NMP::Vector3 startPoint2 = nextT.t; \
      NMP::Vector3 startPoint3 = nextT.t; \
      NMP::Vector3 startPoint4 = nextT.t; \
      stepSize = NM_PI_OVER_TWO / LIMIT_SEGMENTS; \
      for (uint32_t limitI = 0; limitI <= LIMIT_SEGMENTS; ++limitI) \
      { \
        float theta = stepSize * limitI; \
        float s1 = cosf(theta) * limits.upper.y; \
        float s2 = sinf(theta) * limits.upper.z; \
        \
        /* First quarter-segment */ \
        NMP::Vector3 swing(0, s1, s2); \
        GeomUtils::twistSwingToQuat(swing, swingQ); \
        swingQ = prevT.q * limits.frame * swingQ; \
        NMP::Vector3 nextSwingPoint = nextT.t + swingQ.rotateVector(swingDial); \
        debugBuffer->drawLine(startPoint1, nextSwingPoint, limitColour); \
        startPoint1 = nextSwingPoint; \
        /* Second quarter-segment */ \
        swing.set(0, -s1, -s2); \
        GeomUtils::twistSwingToQuat(swing, swingQ); \
        swingQ = prevT.q * limits.frame * swingQ; \
        nextSwingPoint = nextT.t + swingQ.rotateVector(swingDial); \
        debugBuffer->drawLine(startPoint2, nextSwingPoint, limitColour); \
        startPoint2 = nextSwingPoint; \
        /* Third quarter-segment */ \
        s1 = sinf(theta) * limits.upper.y; \
        s2 = cosf(theta) * limits.upper.z; \
        swing.set(0, -s1, s2); \
        GeomUtils::twistSwingToQuat(swing, swingQ); \
        swingQ = prevT.q * limits.frame * swingQ; \
        nextSwingPoint = nextT.t + swingQ.rotateVector(swingDial); \
        debugBuffer->drawLine(startPoint3, nextSwingPoint, limitColour); \
        startPoint3 = nextSwingPoint; \
        /* Fourth quarter-segment */ \
        swing.set(0, s1, -s2); \
        GeomUtils::twistSwingToQuat(swing, swingQ); \
        swingQ = prevT.q * limits.frame * swingQ; \
        nextSwingPoint = nextT.t + swingQ.rotateVector(swingDial); \
        debugBuffer->drawLine(startPoint4, nextSwingPoint, limitColour); \
        startPoint4 = nextSwingPoint; \
      } \
      /* Swing dial */ \
      swingDial = (joints[jidd].q * ~limits.offset).getXAxis() * LIMIT_DIAL_SIZE; \
      nextPoint = nextT.t + prevT.q.rotateVector(swingDial); \
      debugBuffer->drawLine(nextT.t, nextPoint, limitColour); \
    } \
  }
#else
  #define DEBUG_DRAW_LIMITS {}
#endif

#ifdef NMRU_HYBRIDIK_DEBUGDRAW
  #define DEBUG_DRAW(j) \
if (debugBuffer && debugBuffer->drawNextLine()) \
{ \
  GeomUtils::PosQuat prevT = globalTransform ? *globalTransform : GeomUtils::identityPosQuat(); \
  for (uint32_t jdd = 0; jdd < numJoints; ++jdd) \
  { \
    const uint32_t& jidd = jointIndices[jdd]; \
    GeomUtils::PosQuat nextT = prevT; \
    GeomUtils::premultiplyPosQuat(joints[jidd], nextT); \
    if ((signed)jdd >= j) \
    { \
      DEBUG_DRAW_HINGES; \
      DEBUG_DRAW_LIMITS; \
    } \
    if ((signed)jdd > j) \
    { \
      debugBuffer->drawLine(prevT.t, nextT.t); \
      DEBUG_DRAW_JOINT_FRAME; \
    } \
    prevT = nextT; \
    switch (j) \
    { \
      case -1: \
        DEBUG_DRAW_POLEVECTORS(jdd); \
        break; \
      default: \
        if ((signed)jdd == j) \
          DEBUG_DRAW_POLEVECTORS(j); \
    } \
  } \
}
#else
  #define DEBUG_DRAW(j) { (void)globalTransform; }
#endif

#define TARGET_SIZE 0.1f
#if defined(NMRU_HYBRIDIK_DEBUGDRAW_TARGET) && defined(NMRU_HYBRIDIK_DEBUGDRAW)
  #define DEBUG_DRAW_TARGET \
if (debugBuffer && debugBuffer->drawNextLine()) \
{ \
  debugBuffer->drawLocator(target.t, TARGET_SIZE); \
  debugBuffer->drawPosQuatFrame(target, TARGET_SIZE / 2.0f); \
}
#else
  #define DEBUG_DRAW_TARGET {}
#endif

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
#define INWARD_LOOP true
#define OUTWARD_LOOP false

void HybridIK::solve(
  const HybridIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t numJoints,
  const GeomUtils::PosQuat& target,
  const GeomUtils::PosQuat* globalTransform /* = 0 */,
  GeomUtils::DebugDraw* debugBuffer /* = 0 */)
{
  GeomUtils::PosQuat unused;
  solve(params, joints, jointIndices, numJoints, target, unused, globalTransform, debugBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void HybridIK::solve(
  const HybridIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t numJoints,
  const GeomUtils::PosQuat& target,
  GeomUtils::PosQuat& solutionEE,
  const GeomUtils::PosQuat* globalTransform /* = 0 */,
  GeomUtils::DebugDraw* debugBuffer /* = 0 */)
{
  NMP_ASSERT(params);
  NMP_ASSERT(joints);
  NMP_ASSERT(jointIndices);

  // Sanity check for optimisations
#ifdef NMIK_NO_TERTIARIES
  NMP_ASSERT(!params->constraints.areSet(
    IKCONSTRAINT_TERTIARY_ORIENTATION | IKCONSTRAINT_TERTIARY_POLEVECTOR | IKCONSTRAINT_TERTIARY_BINDPOSE));
#endif

  //----------------------------------------------------------------------------------------------------------------------
  // PRE-CLAMP
  // Joints that are violating hard limits from the outset can cause problems for IK, so they need to be clamped onto
  // the limit boundary
  preClampJoints(params->perJointParams, joints, numJoints);

  // We need to keep track of the end effector in the current joint frame at each stage.  We need to
  // FK out to the end effector to find it.
  GeomUtils::PosQuat localEndEffector;
  GeomUtils::identityPosQuat(localEndEffector);
  // Start at 1 because the current joint is the root, so we don't include its transform
  for (uint32_t j = 1; j < numJoints; ++j)
  {
    const uint32_t& ji = jointIndices[j];

    GeomUtils::premultiplyPosQuat(joints[ji], localEndEffector);
  }

  SET_DEBUG_TAG(-1);
  DEBUG_DRAW_TARGET;
  INCREMENT_DEBUG_TAG()
  SET_START_DEBUG_TAG();
#ifdef NMRU_HYBRIDIK_DEBUGDRAW_INITIAL_POSE
  DEBUG_DRAW(-1);
#endif

  //----------------------------------------------------------------------------------------------------------------------
  // Iteration loop.  Inside this loop we go from root to end effector, and back down once.
  //   Recalculate the maximum number of iterations to adjust for non-reversed solve direction
  //   'Reversed' is the direction from root to end effector first, because in the standard
  //   CCD algorithm it is more normal to start at the end effector to give the end joints more bias.
  uint32_t maxIters = params->maxIterations + (!params->reverseDirectionOfSolve && !params->traverseOneWayOnly ? 1 : 0);
  for (uint32_t i = 0; i < maxIters; ++i)
  {
    bool solveInwardLoop = !((params->reverseDirectionOfSolve && params->traverseOneWayOnly) ||
      (!params->reverseDirectionOfSolve && !params->traverseOneWayOnly && i == maxIters-1));
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    bool solveOutwardLoop = !(!params->reverseDirectionOfSolve && (i == 0 || params->traverseOneWayOnly));
#endif

    // Record the global end effector position, for reasons of detecting when motion has stopped
    // perhaps due to all joints reaching limit boundaries, or full reach.
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    GeomUtils::PosQuat startingEndEffectorTM = localEndEffector;
    GeomUtils::multiplyPosQuat(startingEndEffectorTM, joints[jointIndices[0]]);
    if (globalTransform)
    {
      GeomUtils::multiplyPosQuat(startingEndEffectorTM, *globalTransform);
    }
#endif

    // We need to keep track of the 'globalJointTM', the fixed frame at each joint relative to the root.
    // OPTIMISE  If we are doing more than one iteration there is no need to recalculate this.
    GeomUtils::PosQuat globalJointTM;
    if (globalTransform)
    {
      globalJointTM = *globalTransform;
      globalJointTM.t += globalJointTM.q.rotateVector(joints[jointIndices[0]].t);
    }
    else
    {
      globalJointTM.q.identity();
      globalJointTM.t = joints[jointIndices[0]].t;
    }

    // Except on the first iteration, or if we're not quitting when the chain stops moving, the end effector
    // transform is relative to the parent of the root, so it needs to be shifted back to the moving frame
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    if (i != 0)
    {
      GeomUtils::multiplyInversePosQuat(localEndEffector, joints[jointIndices[0]]);
    }
#endif

    //----------------------------------------------------------------------------------------------------------------------
    // Outward loop.  Traverse from root to end effector
    for (uint32_t j = 0; j < numJoints; ++j)
    {
#ifdef HYBRIDIK_QUIT_WHEN_ACCURATE
      bool solved =
#endif
        solveJoint(
          params,
          joints,
          jointIndices,
          j,
          i,
          OUTWARD_LOOP,
          numJoints,
          localEndEffector,
          target,
          globalJointTM,
          globalTransform,
          debugBuffer);

      // Quit early
#ifdef HYBRIDIK_QUIT_WHEN_ACCURATE
      if (solved)
      {
        // Recover the output IK transform by combining globalJointTM and localEndEffectorTM
        GeomUtils::multiplyPosQuat(solutionEE, localEndEffector, joints[jointIndices[j]]);
        GeomUtils::multiplyPosQuat(solutionEE, globalJointTM);

        return;
      }
#endif

    } // end of outward loop

    // Detect stoppage - if end effector has not moved, limb may be at full reach or on limit
    // boundary, so no need to solve further.
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    GeomUtils::PosQuat globalEETMNow = globalJointTM; // Set comparison point
    if (solveOutwardLoop &&
        globalEETMNow.t.distanceSquaredTo(startingEndEffectorTM.t) < GeomUtils::nTolSq &&
        (globalEETMNow.q - startingEndEffectorTM.q).magnitudeSquared() < GeomUtils::nTolSq)
    {
      // Return solution end effector TM
      solutionEE = globalEETMNow;
      return;
    }
    // Update point of comparison
    startingEndEffectorTM = globalEETMNow;
#endif

    // If this is the last iteration and we're only doing the outward loop, skip the inward loop
    // OPTIMISE The only purpose of doing the inward loop when we are only solving on the outward
    // loop (params->reverseDirectionOfSolve && params->traverseOneWayOnly) is to re-accumulate
    // transforms for the new end effector transform, for the next iteration.  However, by calling
    // solveJoint we unnecessarily recalculate globalJointTM as well, which we can more easily
    // calculate in one go (above) without needing to traverse the chain.  However this isn't
    // urgent because the optimised version of IK does only one iteration anyway.
    if (i == maxIters - 1 && !solveInwardLoop)
    {
      // Return solution end effector TM
      solutionEE = globalJointTM;
      // This shift happens in solveJoint() if this switch is defined otherwise we need to do it here
#ifndef HYBRIDIK_QUIT_WHEN_STOPPED
      solutionEE.q *= joints[numJoints - 1].q;
#endif

      SET_DEBUG_TAG_TO_START();
      DEBUG_DRAW(-1);

      return;
    }

    // The local end effector TM should now be the identity, so make sure of this
    GeomUtils::identityPosQuat(localEndEffector);

    // If we're testing for stoppage then the globalJointTM has been moved to the end of the
    // chain, so move it back to the end joint's fixed frame
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    globalJointTM.q *= ~joints[numJoints-1].q;
#endif

    //----------------------------------------------------------------------------------------------------------------------
    // Inward loop.  Traverse from end effector to root
    for (int32_t j = numJoints - 1; j >= 0; --j)
    {
#ifdef HYBRIDIK_QUIT_WHEN_ACCURATE
      bool solved =
#endif
        solveJoint(
          params,
          joints,
          jointIndices,
          (unsigned)j,
          i,
          INWARD_LOOP,
          numJoints,
          localEndEffector,
          target,
          globalJointTM,
          globalTransform,
          debugBuffer);

      // Quit early
#ifdef HYBRIDIK_QUIT_WHEN_ACCURATE
      if (solved)
      {
        // Recover the output IK transform by combining globalJointTM and localEndEffector
        GeomUtils::multiplyPosQuat(solutionEE, localEndEffector, joints[jointIndices[j]]);
        GeomUtils::multiplyPosQuat(solutionEE, globalJointTM);

        return;
      }
#endif

    } // end of inward loop

    // Detect stoppage - if end effector has not moved, limb may be at full reach or on limit
    // boundary, so no need to solve further.
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    globalEETMNow = localEndEffector;
    if (globalTransform)
    {
      GeomUtils::multiplyPosQuat(globalEETMNow, *globalTransform);
    }
    if (solveInwardLoop &&
        globalEETMNow.t.distanceSquaredTo(startingEndEffectorTM.t) < GeomUtils::nTolSq &&
        (globalEETMNow.q - startingEndEffectorTM.q).magnitudeSquared() < GeomUtils::nTolSq)
    {
      // Return solution end effector TM
      solutionEE = globalEETMNow;
      return;
    }
#endif

    // Calculate and write end effector TM output on last iteration
    if (i == maxIters - 1)
    {
      solutionEE = localEndEffector;
      // This shift happens in solveJoint() if this switch is defined otherwise we need to do it here
#ifndef HYBRIDIK_QUIT_WHEN_STOPPED
      GeomUtils::multiplyPosQuat(solutionEE, joints[0]);
#endif
      if (globalTransform)
      {
        GeomUtils::multiplyPosQuat(solutionEE, *globalTransform);
      }
    }

  } // end of iteration loop

  SET_DEBUG_TAG_TO_START();
  DEBUG_DRAW(-1);

} // end of HybridIK::solve()

//----------------------------------------------------------------------------------------------------------------------
bool HybridIK::solveJoint(
  const HybridIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t jointPosition,
  uint32_t iteration,
  bool isInwardLoop,
  uint32_t numJoints,
  GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& target,
  GeomUtils::PosQuat& globalJointTM,
  const GeomUtils::PosQuat*
#ifdef NMRU_HYBRIDIK_DEBUGDRAW_EVERY_JOINT
  globalTransform /* = 0 */
#endif
  ,
  GeomUtils::DebugDraw* debugBuffer /* = 0 */)
{
  NMP_ASSERT(jointIndices);

  uint32_t& j = jointPosition;
  uint32_t& i = iteration;
  const uint32_t& ji = jointIndices[j];
  GeomUtils::PosQuat& joint = joints[ji];
  const HybridIK::PerJointParams& jointParams = params->perJointParams[ji];

  // Simplify whether or not we solve here.  We skip inward loops or outward loops under certain
  // circumstances.
  bool doSolve =
    (isInwardLoop && !(params->reverseDirectionOfSolve && params->traverseOneWayOnly)) ||
    (!isInwardLoop && !(!params->reverseDirectionOfSolve && (i == 0 || params->traverseOneWayOnly)));

  // Other checks for doing a solve on this joint
  bool includeThisJoint =
    // It can be useful to comment out the line below for debugging sometimes to solve even when result will be discarded
    jointParams.weight != 0 &&
    jointParams.enabled;            // Joint is fixed

  //----------------------------------------------------------------------------------------------------------------------
  // MAIN SOLVE CLAUSE
  // Skip under certain conditions
  if (doSolve && includeThisJoint)
  {

    //----------------------------------------------------------------------------------------------------------------------
    // SOLVE

    // Obtain the target in the fixed frame
    GeomUtils::PosQuat targetFF = target;
    GeomUtils::multiplyInversePosQuat(targetFF, globalJointTM);

    //----------------------------------------------------------------------------------------------------------------------
    // Position/Orientation IK via Hybrid IK system

    // Package arguments for processing
    const IKSolverData solverData = {
      localEndEffector,
      targetFF,
      globalJointTM,
      joint.q, // solveStartQuat
      0, // No redundancy control
      j,
      jointIndices,
      numJoints,
      debugBuffer
    };

    HybridIK::solveHybridJoint(joints, localEndEffector, solverData, params);

    //----------------------------------------------------------------------------------------------------------------------
#ifdef NMRU_HYBRIDIK_DEBUGDRAW_EVERY_JOINT
    INCREMENT_DEBUG_TAG()
    DEBUG_DRAW((signed)j)
#endif

    // Quit early
#ifdef HYBRIDIK_QUIT_WHEN_ACCURATE
    if (closeEnough(joint, localEndEffector, targetFF, params))
    {
      return true;
    }
#endif

  } // end of solve clause

  //----------------------------------------------------------------------------------------------------------------------
  // SHIFT

  // Shift frame up to parent joint or down to child joint, depending whether this is inward
  // or outward loop.
  // Note that when HYBRIDIK_QUIT_WHEN_STOPPED is enabled, we want the transforms to move
  // right to the ends of the chain so they can be used to test for when no joints move.
  if (isInwardLoop)
  {
    // Shift frame up to parent joint
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    GeomUtils::multiplyPosQuat(localEndEffector, joint);
#endif
    if (j > 0)
    {
      const uint32_t& jiNext = jointIndices[j-1];
#ifndef HYBRIDIK_QUIT_WHEN_STOPPED
      GeomUtils::multiplyPosQuat(localEndEffector, joint);
#endif
      globalJointTM.t -= globalJointTM.q.rotateVector(joint.t);
      globalJointTM.q *= ~joints[jiNext].q;
    }
  }
  else
  {
    // Shift frame down to child joint
#ifdef HYBRIDIK_QUIT_WHEN_STOPPED
    globalJointTM.q *= joint.q;
#endif
    if (j < numJoints - 1)
    {
      const uint32_t& jiNext = jointIndices[j+1];
      GeomUtils::multiplyInversePosQuat(localEndEffector, joints[jiNext]);
#ifndef HYBRIDIK_QUIT_WHEN_STOPPED
      globalJointTM.q *= joint.q;
#endif
      globalJointTM.t += globalJointTM.q.rotateVector(joints[jiNext].t);
    }

    // Sanity check
    NMP_ASSERT(localEndEffector.t.magnitudeSquared() * 0 == 0);
    NMP_ASSERT(localEndEffector.q.magnitudeSquared() * 0 == 0);
    NMP_ASSERT(NMP::nmfabs(localEndEffector.q.magnitudeSquared() - 1.0f) < 0.1f);
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
#define PRIMARY_SOLVE true
#define SECONDARY_SOLVE false
#define REDUNDANCY_ONLY true
#define FULL_SOLVE false
void HybridIK::solveHybridJoint(
  GeomUtils::PosQuat* joints,
  GeomUtils::PosQuat& localEndEffector,
  const IKSolverData& solverData,
  const Params* params)
{
  const uint32_t& jointPosition = solverData.jointPosition;
  const uint32_t& ji = solverData.jointIndices[jointPosition];
  const HybridIK::PerJointParams& jointParams = params->perJointParams[ji];
  GeomUtils::PosQuat& joint = joints[ji];
  NMP::Quat oldQ = joint.q;

  //----------------------------------------------------------------------------------------------------------------------
  // SECONDARY CONSTRAINTS
  solveHybridJointPositionOrientation(SECONDARY_SOLVE, joints, localEndEffector, solverData, params);

  //----------------------------------------------------------------------------------------------------------------------
  // PRIMARY CONSTRAINTS
  if (!jointParams.secondaryOnly)
  {
    solveHybridJointPositionOrientation(PRIMARY_SOLVE, joints, localEndEffector, solverData, params);
  }

  //----------------------------------------------------------------------------------------------------------------------
  // WEIGHT and CLAMP
  // I believe it is more accurate to clamp before the swivel component, otherwise swivel can prevent
  // the approach to the target.
  // If we were doing two-bone solves, we were weighting and clamping anyway.
  bool doTwoBoneSolve = jointParams.doTwoBoneSolve && (signed)jointPosition < ((signed)solverData.numJoints - 2);
  if (!doTwoBoneSolve)
  {
#ifdef DIRECT_CLAMP
    weightAndClamp(jointParams, oldQ, joint.q, true);
#else
    weightAndClamp(jointParams, oldQ, joint.q, false);
#endif
  }

  //----------------------------------------------------------------------------------------------------------------------
  // TERTIARY REDUNDANCY CONTROL

#ifndef NMIK_NO_TERTIARIES
  oldQ = joint.q;

  // First, apply bind pose again as a partial rotation about the swivel axis
  if (params->constraints.isSet(IKCONSTRAINT_TERTIARY_BINDPOSE))
  {
    jointApplyBindPose(
      SECONDARY_SOLVE, REDUNDANCY_ONLY,
      joints, solverData, params);
  }

  // To blend the tertiaries correctly we need to do them independently
  NMP::Quat startQ = joint.q;
  NMP::Quat blendedQ(0, 0, 0, 0);
  float orientationsWeight = 0;
  float tertiariesWeight = 0;

  solveJointOrientations(
    SECONDARY_SOLVE, REDUNDANCY_ONLY, orientationsWeight,
    joints, solverData, params);
  if (blendedQ.dot(joint.q) >= 0)
  {
    blendedQ += joint.q * orientationsWeight;
  }
  else
  {
    blendedQ -= joint.q * orientationsWeight;
  }

  joint.q = startQ;
  solveJointTertiaries(
    tertiariesWeight, &jointParams, joints, solverData);
  if (blendedQ.dot(joint.q) >= 0)
  {
    blendedQ += joint.q * tertiariesWeight;
  }
  else
  {
    blendedQ -= joint.q * tertiariesWeight;
  }

  // Blend orientation and tertiaries results, and weight
  if (orientationsWeight != 0 || tertiariesWeight != 0)
  {
    blendedQ.fastNormalise();
    joint.q = startQ;
    GeomUtils::shortestArcFastSlerp(joint.q, blendedQ, jointParams.redundancyControlWeight);
  }
  else
  {
    joint.q = startQ;
  }

  //----------------------------------------------------------------------------------------------------------------------
  // WEIGHT and CLAMP
  // Avoid another clamp if we haven't done any tertiary control
  if (orientationsWeight != 0 || tertiariesWeight != 0 ||
      params->constraints.isSet(IKCONSTRAINT_TERTIARY_BINDPOSE))
  {
  #if defined(DIRECT_CLAMP) && defined(DIRECT_CLAMP_EVEN_FOR_SWIVEL)
    weightAndClamp(jointParams, oldQ, joint.q, true);
  #else
    bool doNormalClamp = (jointPosition == numJoints - 1) ||
                         localEndEffector.t.magnitudeSquared() < GeomUtils::nTolSq ||
                         solverData.localTarget.t.magnitudeSquared() < GeomUtils::nTolSq;
    weightAndClamp(jointParams, oldQ, joint.q, doNormalClamp);
  #endif
  }

#endif // #ifndef NMIK_NO_TERTIARIES

} // end of HybridIK::solveHybridJoint()

//----------------------------------------------------------------------------------------------------------------------
void HybridIK::solveHybridJointPositionOrientation(
  bool isPrimary,
  GeomUtils::PosQuat* joints,
  GeomUtils::PosQuat& localEndEffector,
  const IKSolverData& solverData,
  const Params* params)
{
  const uint32_t& ji = solverData.jointIndices[solverData.jointPosition];
  const HybridIK::PerJointParams& jointParams = params->perJointParams[ji];
  GeomUtils::PosQuat& joint = joints[ji];

  bool applyBindPose = (isPrimary && (params->constraints.isSet(IKCONSTRAINT_PRIMARY_BINDPOSE))) ||
    (!isPrimary && (params->constraints.isSet(IKCONSTRAINT_SECONDARY_BINDPOSE)));
  bool solveForOrientation = (isPrimary && (params->constraints & IKCONSTRAINT_PRIMARY_GEN_ORIENT)) ||
    (!isPrimary && (params->constraints & IKCONSTRAINT_SECONDARY_GEN_ORIENT));
  bool solveForPosition = (isPrimary && params->constraints.isSet(IKCONSTRAINT_PRIMARY_POSITION)) ||
    (!isPrimary && params->constraints.isSet(IKCONSTRAINT_SECONDARY_POSITION));

  ////////////////
  // BIND POSE
  if (applyBindPose)
  {
    // NOTE If the bind pose violates the joint limit, then this can potentially cause
    // problems for a two-bone position solve, since that requires the joint to be
    // within the limits to start with.  For performance reasons, it is up to the user to
    // control bind pose validity.
    jointApplyBindPose(
      isPrimary, FULL_SOLVE,
      joints, solverData, params);
  }

  // Quit early
  if (!solveForOrientation && !solveForPosition)
  {
    return;
  }

  NMP::Quat oldQ = joint.q;
  NMP::Quat resultQ(0, 0, 0, 0);

  bool doTwoBoneSolve =
    jointParams.doTwoBoneSolve && (signed)solverData.jointPosition < (signed)solverData.numJoints - 2;
  float orientationsWeight = 0;

  ///////////////////////////////
  // ORIENTATION AND POLE VECTOR
  if (solveForOrientation)
  {
    solveJointOrientations(
      isPrimary, FULL_SOLVE, orientationsWeight,
      joints, solverData, params);

    // Any motion should be weighted and clamped if we are doing a two-bone solve, because
    // that method requires weighting and clamping internally to the position solve.
    if (doTwoBoneSolve)
    {
#ifdef DIRECT_CLAMP
      weightAndClamp(jointParams, oldQ, joint.q, true);
#else
      weightAndClamp(jointParams, oldQ, joint.q, false);
#endif
    }

    // Need to use the correct weights
    resultQ += joint.q * orientationsWeight;
  }

  // Reset start orientation
  joint.q = oldQ;

  /////////////
  // POSITION
  float positionWeight = params->targetPositionWeight * jointParams.positionWeight;
  if (solveForPosition)
  {
    if (doTwoBoneSolve)
    {
      solveTwoBoneJoint(joints, localEndEffector, solverData, params);
    }
    else
    {
      solveCCDJoint(joints, solverData, params);
    }
  }
  else
  {
    positionWeight = NMP::maximum(0.0f, 1.0f - orientationsWeight);
  }

  //////////
  // BLEND

  // Even if I'm not solving for position, I have to balance the orientation with 'resistance'
  // to moving at all - this gives the expected behaviour, i.e. that as you reduce the orientation
  // weights the IK chain will 'try less hard' to achieve the orientation goals.
  resultQ += joint.q * NMP::floatSelect(resultQ.dot(joint.q), positionWeight, -positionWeight);

  // Reset start orientation
  joint.q = oldQ;

  // Blend position and orientation results
  if (resultQ.magnitudeSquared() > 0)
  {
    resultQ.fastNormalise();
    joint.q = resultQ;
  }

} // end of HybridIK::solveHybridJointPositionOrientation()

//----------------------------------------------------------------------------------------------------------------------
void HybridIK::solveCCDJoint(
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params)
{
  const uint32_t& ji = solverData.jointIndices[solverData.jointPosition];
  const HybridIK::PerJointParams& jointParams = params->perJointParams[ji];
  GeomUtils::PosQuat& joint = joints[ji];

  // Exit if this joint has zero weight
  if (jointParams.weight == 0 || !jointParams.enabled)
  {
    return;
  }

  //----------------------------------------------------------------------------------------------------------------------
  // POSITION IK
  bool doPositionIK =
    solverData.jointPosition < solverData.numJoints - 1 &&
    solverData.localEndEffector.t.magnitudeSquared() > GeomUtils::gTolSq &&
    solverData.localTarget.t.magnitudeSquared() > GeomUtils::gTolSq;
  if (doPositionIK)
  {

#ifndef TWOBONE_LIMIT_DIVERGENCE_TRADEOFF
    // If a two-bone root joint is set, do a two-bone solve instead
    if (jointParams.twoBoneRootJointIndex >= 0)
    {
      HybridIK::solveTwoBoneHinge(joints, solverData);
    }

    // Non two-bone joints
    else
#endif
    {

      NMP::Quat newQ = joint.q;

#ifndef NMRU_HYBRIDIK_DEBUGDRAW_DIRECTION_HINTS
      solverData.debugBuffer = 0;
#endif

      // INVERSE KINEMATICS ALGORITHM
      HybridIK::forRotationToTarget(
        jointParams, joint.q, newQ, params->directionHintsFactor, solverData);

      // Copy back result
      joint.q = newQ;

#ifdef TWOBONE_LIMIT_DIVERGENCE_TRADEOFF
      // If we are balancing two-bone with CCD, then doing a two-bone solve must come after,
      // and in addition to the CCD solve
      if (jointParams.twoBoneRootJointIndex >= 0)
      {
        HybridIK::solveTwoBoneHinge(joints, solverData, params);
      }
#endif

    } // Clause encompassing all non-two-bone-hinge-joints, i.e. most joints

  } // if (doPositionIK)

} // end of HybridIK::solveCCDJoint()

//----------------------------------------------------------------------------------------------------------------------
void HybridIK::solveTwoBoneJoint(
  GeomUtils::PosQuat* joints,
  GeomUtils::PosQuat& localEndEffector,
  const IKSolverData& solverData,
  const Params* params)
{
  const uint32_t& jointIndex = solverData.jointIndices[solverData.jointPosition];
  const HybridIK::PerJointParams& jointParams = params->perJointParams[jointIndex];
  GeomUtils::PosQuat& joint = joints[jointIndex];

  //----------------------------------------------------------------------------------------------------------------------
  // Set up 2-bone IK chain
  // Start with both the end effector (the second 'joint' in the 2-bone chain) and the hinge
  // joint (the middle joint) at the end effector, and then start each loop by moving the
  // hinge joint up to its parent.
  static GeomUtils::PosQuat twoBoneJoints[3]; // Static to avoid continual construction
  twoBoneJoints[0].q = joint.q;
  twoBoneJoints[0].t.setToZero();
  twoBoneJoints[1].q = solverData.localEndEffector.q;
  twoBoneJoints[1].t = solverData.localEndEffector.t;
  twoBoneJoints[2].q.identity();
  twoBoneJoints[2].t.setToZero();
  TwoBoneIK::Params twoBoneSolverParams;

  //----------------------------------------------------------------------------------------------------------------------
  // Treating the current joint as the root in each case, loop through from the end-effector-but-one
  // up towards this joint treating each joint in turn as the hinge in a two-bone system
  for (int32_t hingePosition = (signed)solverData.numJoints - 2; hingePosition > (signed)solverData.jointPosition;
       --hingePosition)
  {
    const uint32_t& hingeJoint = solverData.jointIndices[hingePosition];
    const HybridIK::PerJointParams& hingeParams = params->perJointParams[hingeJoint];

    //----------------------------------------------------------------------------------------------------------------------
    // SHIFT
    // Shift to next joint up
    const uint32_t& hingeChild = solverData.jointIndices[hingePosition+1];
    GeomUtils::premultiplyInversePosQuat(joints[hingeChild], twoBoneJoints[1]);
    GeomUtils::multiplyPosQuat(twoBoneJoints[2], joints[hingeChild]);

    NMP::Quat realHingeFixedFrameToVirtualHingeFixedFrame = twoBoneJoints[1].q * ~joints[hingeJoint].q;

    //----------------------------------------------------------------------------------------------------------------------
    // Conditions for doing a two-bone solve on this pair of joints
    // In the current version, we only pair up roots and genuine hinge joints.
    if (hingeParams.isHinge)
    {

      //----------------------------------------------------------------------------------------------------------------------
      // Choose a mid-joint hinge axis
      twoBoneSolverParams.preventBendingBackwards = false;
      twoBoneSolverParams.zeroHingeTanHalfAngle = 0;
      twoBoneSolverParams.correctHingeDrift = false;
      if (hingeParams.isHinge)
      {
        // Get the hinge axis in the correct space for our virtual two-bone system
        twoBoneSolverParams.hingeAxis =
          realHingeFixedFrameToVirtualHingeFixedFrame.rotateVector(hingeParams.hingeAxis);

        // Calculate backwards-bending params (the two-bone solver's hinge joint 'limits') from
        // the joint limits.
        //   Extra check for if any real or virtual bones are zero length, when this kind of
        //   limit is not meaningful
        if (hingeParams.isLimited && hingeParams.applyHardLimits &&
            twoBoneJoints[1].t.magnitudeSquared() > GeomUtils::gTolSq &&
            twoBoneJoints[2].t.magnitudeSquared() > GeomUtils::gTolSq &&
            joints[hingeJoint].t.magnitudeSquared() > GeomUtils::gTolSq &&
            joints[hingeChild].t.magnitudeSquared() > GeomUtils::gTolSq)
        {
          twoBoneSolverParams.preventBendingBackwards = true;

          ////////////////////////////////////////////////////////////////////////////////////////////////////
          // What this is doing is calculating the rotation from the current orientation to where the
          // bones of the virtual limb are straight, called qs.  Then we calculate the rotation from
          // current to where the hinge joint is rotated at its furthest backwards limit, which is
          // calculated by making use of knowledge of when the real bones either side of that joint
          // are parallel (that joint is 'straight').  The angle between these two gives us the 'zero
          // hinge angle', an offset from 'straight' for the two-bone IK to work out which direction
          // of hinge rotation to favour in its solution.
          ////////////////////////////////////////////////////////////////////////////////////////////////////

          const NMP::Vector3& axis = twoBoneSolverParams.hingeAxis;
          //   When the bones of this virtual two-bone system are 'straight', what is the hinge rotation?
          NMP::Vector3 v1 = vNormalise(twoBoneJoints[1].t);
          NMP::Vector3 v2 = vNormalise(twoBoneJoints[1].q.rotateVector(twoBoneJoints[2].t));
          NMP::Quat qs = GeomUtils::forRotationAroundAxis(v2, v1, axis);

          // If the hinge axis and the joint limits are compatible, the hinge axis should be parallel to
          // the x-axis in the limit frame.  If it is, calculate the zero hinge angle from the straight-limb
          // twist limit combined with the 'straightAngle' rotation calculated above.
          // If it isn't, the zero angle is taken to be when the two real bones are parallel.  In other words,
          // the zero hinge angle will just be zero, but transferred onto the virtual limb.  We have to
          // do this because if the hinge axis isn't lined up with the joint limits then it's hard to
          // use the joint limits to determine the allowed extent of joint rotation.
          NMP::Vector3 hingeAxisInLimitFrame = hingeParams.limits.frame.inverseRotateVector(hingeParams.hingeAxis);
          if (NMP::nmfabs(NMP::nmfabs(hingeAxisInLimitFrame.x) - 1.0f) < GeomUtils::gTol)
          {
            // This is calculating the relevant quaternion representing the orientation of this
            // joint when it is on its furthest backwards-bent limit.  As long as I've calculated
            // my hinge axis correctly, then this is defined by whichever limit is closest to a
            // straight limb.
            NMP::Quat lowerTwistLimitQ;
            NMP::Quat upperTwistLimitQ;
            // It is probably okay to use twistSwingToQuat(), it is pretty fast (no trig or sqrts)
            GeomUtils::twistSwingToQuat(NMP::Vector3(hingeParams.limits.lower.x, 0, 0), lowerTwistLimitQ);
            GeomUtils::twistSwingToQuat(NMP::Vector3(hingeParams.limits.upper.x, 0, 0), upperTwistLimitQ);
            NMP::Quat preMultiply = realHingeFixedFrameToVirtualHingeFixedFrame * hingeParams.limits.frame;
            NMP::Quat postMultiply = ~hingeParams.limits.offset * ~twoBoneJoints[1].q;
            NMP::Quat qhLower = preMultiply * lowerTwistLimitQ * postMultiply;
            NMP::Quat qhUpper = preMultiply * upperTwistLimitQ * postMultiply;
            float dotQLower = NMP::nmfabs(qhLower.dot(qs));
            float dotQUpper = NMP::nmfabs(qhUpper.dot(qs));
            // Clamping to -0.9999f avoids a divide-by-zero, and only prevents silly values of zero hinge angle.
            float cosHalfAngle = NMP::clampValue(NMP::maximum(dotQLower, dotQUpper), -0.9999f, 1.0f);
            twoBoneSolverParams.zeroHingeTanHalfAngle = NMP::fastSqrt((1.0f - cosHalfAngle) / (1.0f + cosHalfAngle));
          }
          else
          {
            //  What is the hinge rotation when the neighbouring /true/ bones are straight?
            v1 = vNormalise(twoBoneJoints[1].q.rotateVector(joints[hingeJoint].q.inverseRotateVector(joints[hingeJoint].t)));
            v2 = vNormalise(twoBoneJoints[1].q.rotateVector(joints[hingeChild].t));
            NMP::Quat qh = GeomUtils::forRotationAroundAxis(v2, v1, axis);

            // Rotation between the orientation where the virtual bones are parallel, and the reference orientation
            // as calculated above.  Calculate this via the dot-product.
            // OPTIMISE We can eliminate the arccos when we convert the two-bone solver to taking tan-quarter-angle
            // pseudo-limits instead of limits in radians.
            float dotQ = NMP::nmfabs(qh.dot(qs));
            // Clamping to -0.9999f avoids a divide-by-zero, and only prevents silly values of zero hinge angle.
            float cosHalfAngle = NMP::clampValue(dotQ, -0.9999f, 1.0f);
            twoBoneSolverParams.zeroHingeTanHalfAngle = NMP::fastSqrt((1.0f - cosHalfAngle) / (1.0f + cosHalfAngle));
          }
        }

      } // end of if (hingeParams.isHinge)

      // If the hinge axis is not provided, use one perpendicular to the bones of our virtual system.
      // We do this because the primary purpose of the hinge action is to set the distance between
      // root and end effector, and this choice of hinge axis does this most effectively
      // OPTIMISE: choosing a 'good' hinge axis is not crucial - we could eliminate conditionals by
      // forcing a simple calculation such as makeOrthogonal(twoBoneJoints[1].t).  This has been shown to
      // produce valid, and in particular smooth results, but does give some unusual twists in the
      // rotations, more often requiring clamping.
      else
      {
        static const float xUnstableHingeAxisTolerance = 1e-4f;
        twoBoneSolverParams.hingeAxis.cross(twoBoneJoints[1].q.rotateVector(twoBoneJoints[2].t), twoBoneJoints[1].t);
        // Test for unstable hinge axis and correct
        if (twoBoneSolverParams.hingeAxis.magnitudeSquared() <= xUnstableHingeAxisTolerance)
        {
          // Try getting the axis defined by the bones when they are in their zero position
          twoBoneSolverParams.hingeAxis.cross(twoBoneJoints[2].t, twoBoneJoints[1].t);
          if (twoBoneSolverParams.hingeAxis.magnitudeSquared() <= xUnstableHingeAxisTolerance)
          {
            // Give up and get any axis - we'll end up here if the end effector is coincident
            // with the hinge joint, or with the root joint (say the IK chain is curled up)
            // - that's okay, the 2-bone solver should handle it adequately
            twoBoneSolverParams.hingeAxis.makeOrthogonal(twoBoneJoints[1].t);
          }
        }
        twoBoneSolverParams.hingeAxis.normalise();
      } // end of setting of hinge axis

      //----------------------------------------------------------------------------------------------------------------------
      // Set other parameters
      // Turn off orientation, pole vectors and swivel since secondary constraints are done externally
      twoBoneSolverParams.endJointOrientationWeight = 0;
      twoBoneSolverParams.midJointRedundancyParams.active = false;
      twoBoneSolverParams.endEffectorPoleVectorParams.active = false;
      twoBoneSolverParams.swivelOrientationWeight = 0;
      twoBoneSolverParams.hingeConditionLowerBound = 0.0f; // Never scale down hinge rotation

      //----------------------------------------------------------------------------------------------------------------------
      // SOLVE
      NMP::Quat oldVirtualRootQuat = twoBoneJoints[0].q;
      NMP::Quat oldVirtualHingeQuat = twoBoneJoints[1].q;
      TwoBoneIK::solve(&twoBoneSolverParams, twoBoneJoints, solverData.localTarget);

  #ifdef NMRU_HYBRIDIK_TWOBONESOLVE_DEBUGDRAW
      if (solverData.debugBuffer)
      {
        GeomUtils::PosQuat prevT = GeomUtils::identityPosQuat();
        for (uint32_t jdd = 0; jdd < solverData.jointPosition; ++jdd)
        {
          const uint32_t& jidd = solverData.jointIndices[jdd];
          GeomUtils::premultiplyPosQuat(joints[jidd], prevT);
        }
        GeomUtils::PosQuat T = twoBoneJoints[0];
        T.t = joints[jointIndex].t;
        GeomUtils::premultiplyPosQuat(T, prevT);
        GeomUtils::PosQuat nextT = prevT;
        GeomUtils::premultiplyPosQuat(twoBoneJoints[1], nextT);
        solverData.debugBuffer->drawDashedLine(prevT.t, nextT.t, 0.02f);
        NMP::Vector3 axis = prevT.q.rotateVector(twoBoneSolverParams.hingeAxis);
        solverData.debugBuffer->drawDashedLine(nextT.t, nextT.t + axis * HINGE_AXIS_SIZE, 0.02f);
        prevT = nextT;
        GeomUtils::premultiplyPosQuat(twoBoneJoints[2], nextT);
        solverData.debugBuffer->drawDashedLine(prevT.t, nextT.t, 0.02f);
      }
  #endif

      // The two-bone system assumes that the root joint can move freely.  However, if we have specified this
      // joint as a hinge, this is not true; so even without joint limits we must restrict the root motion
      // to the closest position to the calculated motion that is possible around the correct axis
      // OPTIMISE You could remove the need for this by not allowing hinge joints to act as roots of a
      // two-bone solve.  Or always have joint limits.
      if (jointParams.isHinge)
      {
        NMP::Quat rootRotation = twoBoneJoints[0].q * ~oldVirtualRootQuat;
        float condition;
        twoBoneJoints[0].q =
          GeomUtils::closestRotationAroundAxis(rootRotation, jointParams.hingeAxis, &condition) * oldVirtualRootQuat;
        // Weight result by conditioning, to avoid instabilities
        GeomUtils::shortestArcFastSlerp(twoBoneJoints[0].q, oldVirtualRootQuat, 1.0f - condition);
      }

      //----------------------------------------------------------------------------------------------------------------------
      // WEIGHT
      // Weight is combined from the two joints involved, plus the position weight
      float weight = jointParams.weight * hingeParams.weight * params->targetPositionWeight;
      float invWeight = 1.0f - weight;
      GeomUtils::shortestArcFastSlerp(twoBoneJoints[0].q, oldVirtualRootQuat, invWeight);
      GeomUtils::shortestArcFastSlerp(twoBoneJoints[1].q, oldVirtualHingeQuat, invWeight);

      //----------------------------------------------------------------------------------------------------------------------
      // Get result
      NMP::Quat oldRootQuat = joint.q;
      NMP::Quat oldHingeQuat = joints[hingeJoint].q;
      joint.q = twoBoneJoints[0].q;
      joints[hingeJoint].q = oldHingeQuat * ~oldVirtualHingeQuat * twoBoneJoints[1].q;
      joint.q.fastNormalise();
      joints[hingeJoint].q.fastNormalise();

      //----------------------------------------------------------------------------------------------------------------------
      // CLAMP
      bool jointIsLimited = jointParams.isLimited && jointParams.applyHardLimits;
      bool hingeIsLimited = hingeParams.isLimited && hingeParams.applyHardLimits;
      if (jointIsLimited || hingeIsLimited)
      {
        char clampValRoot = jointParams.applyHardLimits ? isViolated(jointParams, joint.q) : 0;
        char clampValHinge = hingeParams.applyHardLimits ? isViolated(hingeParams, joints[hingeJoint].q) : 0;
        if (clampValRoot || clampValHinge)
        {
          NMP::Quat unclampedHingeQuat = joints[hingeJoint].q;

          // Bisection search for intermediate pose on limit boundary, employing smooth motion of both joints
          // towards their desired orientation: if hinge joint hits a limit, root joint is also prevented
          // from moving further, and vice versa.  This is important to preserve convergence guarantee.
          float a = 0;
          float b = 1.0f;
          float fromDotToRoot = oldRootQuat.dot(joint.q);
          float fromDotToHinge = oldHingeQuat.dot(joints[hingeJoint].q);
          // fastSlerp can't handle reversed quats so we have to check for this explicitly
          if (fromDotToRoot < 0)
          {
            fromDotToRoot = -fromDotToRoot;
            joint.q = -joint.q;
          }
          if (fromDotToHinge < 0)
          {
            fromDotToHinge = -fromDotToHinge;
            joints[hingeJoint].q = -joints[hingeJoint].q;
          }

          // Destination quats
          NMP::Quat targetRootQ = joint.q;
          NMP::Quat targetHingeQ = joints[hingeJoint].q;

          // Bisection search loop
          static const uint32_t xMaxTwoBoneBinarySearchIters = 10;
          bool currentlyViolatingLimit = true;
          for (uint32_t iter = 0; iter < xMaxTwoBoneBinarySearchIters; ++iter)
          {
            float t = 0.5f * (a + b);
            joint.q.fastSlerp(oldRootQuat, targetRootQ, t, fromDotToRoot);
            joints[hingeJoint].q.fastSlerp(oldHingeQuat, targetHingeQ, t, fromDotToHinge);
            currentlyViolatingLimit =
              (jointIsLimited && isViolated(jointParams, joint.q)) ||
              (hingeIsLimited && isViolated(hingeParams, joints[hingeJoint].q));
            if (currentlyViolatingLimit)
            {
              b = t;
            }
            else
            {
              a = t;
            }
          }
          // If we end over the limit, make sure our output is inside the limit
          if (currentlyViolatingLimit)
          {
            joint.q.fastSlerp(oldRootQuat, targetRootQ, a, fromDotToRoot);
            joints[hingeJoint].q.fastSlerp(oldHingeQuat, targetHingeQ, a, fromDotToHinge);
          }

          // Copy back clamped result
          twoBoneJoints[0].q = joint.q;
          twoBoneJoints[1].q = twoBoneJoints[1].q * ~unclampedHingeQuat * joints[hingeJoint].q;

          // If we have clamped, we need to make a second motion of the root from here towards the target
          GeomUtils::PosQuat movingEE;
          GeomUtils::multiplyPosQuat(movingEE, twoBoneJoints[2], twoBoneJoints[1]);
          NMP::Quat newQ;
          HybridIK::forRotationToTarget(jointParams, joint.q, newQ, solverData);
          // Apply weight
          GeomUtils::shortestArcFastSlerp(newQ, joint.q, 1.0f - jointParams.weight);
          // And clamp again
          if (jointIsLimited)
          {
            JointLimits::Params tightenedLimits(jointParams.limits, xClampTighten);
#ifdef SMOOTH_LIMITS
  #ifdef DIRECT_CLAMP
            JointLimits::clamp(tightenedLimits, newQ, xLimitSmoothness);
  #else
            JointLimits::clampBetween(tightenedLimits, joint.q, newQ, xLimitSmoothness);
  #endif
#else
  #ifdef DIRECT_CLAMP
            JointLimits::clamp(tightenedLimits, newQ);
  #else
            JointLimits::clampBetween(tightenedLimits, joint.q, newQ);
  #endif
#endif
          }
          // Copy back result to our virtual and real joint chains
          twoBoneJoints[0].q = newQ;
          joint.q = newQ;
        }
      } // end of clamping clause

    } // end of conditions for doing a two-bone solve on this pair of joints

    //----------------------------------------------------------------------------------------------------------------------
    // Adjust local end effector (moving frame)
    GeomUtils::multiplyPosQuat(localEndEffector, twoBoneJoints[2], twoBoneJoints[1]);

    // Sanity check
    NMP_ASSERT(localEndEffector.t.magnitudeSquared() * 0 == 0);
    NMP_ASSERT(localEndEffector.q.magnitudeSquared() * 0 == 0);
    NMP_ASSERT(NMP::nmfabs(localEndEffector.q.magnitudeSquared() - 1.0f) < 0.1f);

  } // end of loop through different hinge joints

} // end of function HybridIK::solveTwoBoneJoint()

//----------------------------------------------------------------------------------------------------------------------
void HybridIK::solveTwoBoneHinge(
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params)
{
  const uint32_t& jointIndex = solverData.jointIndices[solverData.jointPosition];
  const HybridIK::PerJointParams& jointParams = params->perJointParams[jointIndex];
  GeomUtils::PosQuat& joint = joints[jointIndex];
  NMP::Quat newQ = joint.q;

  // Generate a virtual two-bone system
  GeomUtils::PosQuat twoBoneJoints[3];
  // FK to the root
  GeomUtils::identityPosQuat(twoBoneJoints[1]);
  int32_t jr = solverData.jointPosition;
  uint32_t jir = jointIndex;
  do
  {
    GeomUtils::multiplyPosQuat(twoBoneJoints[1], joints[jir]);
    // Go to next joint up
    --jr;
    jir = solverData.jointIndices[jr];
  }
  while (jr >= 0 && (signed)jir != jointParams.twoBoneRootJointIndex);
  // The root joint's transform just takes us back into the mid joint frame,
  // so that our target is in the right space.
  GeomUtils::invertPosQuat(twoBoneJoints[0], twoBoneJoints[1]);
  // And the end joint is just the end effector transform
  twoBoneJoints[2] = solverData.localEndEffector;
  // Generate a target in the moving frame
  GeomUtils::PosQuat targetMovingFrame;
  targetMovingFrame.q = ~joint.q * solverData.localTarget.q;
  targetMovingFrame.t = joint.q.inverseRotateVector(solverData.localTarget.t);

  // Get the hinge axis in the joint moving frame
  NMP::Vector3 hingeAxis = joint.q.inverseRotateVector(jointParams.hingeAxis);
  if (!jointParams.isHinge)
  {
    // Find a good rotation axis
    static const float xUnstableHingeAxisTolerance = 1e-4f;
    float quality = 0;

    // Get the axis that folds the limb and choose a direction based on
    // the direction hint orientation
    if (quality < xUnstableHingeAxisTolerance)
    {
      hingeAxis.cross(solverData.localEndEffector.t, twoBoneJoints[0].t);
      quality = hingeAxis.magnitudeSquared();
      NMP::Vector3 desiredDirection = jointParams.directionHintQuat.rotateVector(solverData.localEndEffector.t);
      NMP::Vector3 hingeAxisFromDesiredDirection;
      hingeAxisFromDesiredDirection.cross(joint.q.inverseRotateVector(desiredDirection), twoBoneJoints[0].t);
      if (hingeAxisFromDesiredDirection.dot(hingeAxis) < 0)
      {
        hingeAxis = -hingeAxis;
      }
    }

    // Get the axis that rotates us to our direction hint orientation
    if (quality < xUnstableHingeAxisTolerance && jointParams.directionHintQuat.w <= 1.0f)
    {
      // Get the direction of the end effector when the joint is in the direction hint
      // orientation
      NMP::Vector3 desiredDirection = jointParams.directionHintQuat.rotateVector(solverData.localEndEffector.t);
      hingeAxis.cross(solverData.localEndEffector.t, joint.q.inverseRotateVector(desiredDirection));
      quality = hingeAxis.magnitudeSquared();
    }

    // Get the axis that most effectively varies the distance to the root
    if (quality < xUnstableHingeAxisTolerance)
    {
      hingeAxis.cross(solverData.localEndEffector.t, twoBoneJoints[0].t);
      quality = hingeAxis.magnitudeSquared();
    }

    // Get the axis that rotates the end effector towards the target
    if (quality < xUnstableHingeAxisTolerance)
    {
      hingeAxis.cross(solverData.localEndEffector.t, targetMovingFrame.t);
      quality = hingeAxis.magnitudeSquared();
    }

    // Get any perpendicular hinge axis
    if (quality < xUnstableHingeAxisTolerance)
    {
      hingeAxis.makeOrthogonal(solverData.localEndEffector.t);
    }

    hingeAxis.normalise();
  }

  // Solve for the possible angles of rotation
  float angle1, angle2;
#ifdef NMRU_HYBRIDIK_TWOBONESOLVE_DEBUGDRAW
  if (solverData.debugBuffer)
  {
    solverData.debugBuffer->worldFrameOffset.q = solverData.globalJointTM.q * joint.q;
    solverData.debugBuffer->worldFrameOffset.t = solverData.globalJointTM.t;
  }
  // OPTIMISE The two-bone solve should return tan-half-angle to avoid trig
  TwoBoneIK::solveHinge(
    hingeAxis, twoBoneJoints, targetMovingFrame, angle1, angle2,
    TwoBoneIK::xTwoBoneIKReachStabilityFactor, solverData.debugBuffer);
  if (solverData.debugBuffer)
  {
    GeomUtils::identityPosQuat(solverData.debugBuffer->worldFrameOffset);
  }
#else
  TwoBoneIK::solveHinge(hingeAxis, twoBoneJoints, targetMovingFrame, angle1, angle2);
#endif

#ifdef TWOBONE_LIMIT_DIVERGENCE_TRADEOFF
  // Reduce the amount of rotation depending on how close the root joint is to its limits
  float twoBoneWeight = 1.0f;
  if (params->perJointParams[jir].isLimited)
  {
    twoBoneWeight =
      JointLimits::degreeOfViolation(params->perJointParams[jir].limits, joints[jir].q, 1.0f);
    // Take the weight to a high power, which will keep it close to 1 except as we approach the limit.
    twoBoneWeight *= twoBoneWeight;
    twoBoneWeight *= twoBoneWeight;
    twoBoneWeight *= twoBoneWeight;
    twoBoneWeight = NMP::clampValue((1.0f - twoBoneWeight), 0.0f, 1.0f);
  }
#else
  static const float twoBoneWeight = 1.0f;
#endif

  // Find the quaternion that represents this rotation
  NMP::Quat jointRotation(hingeAxis, angle1 * twoBoneWeight);
  // Update joint orientation
  newQ *= jointRotation;

  // We need to choose between the two solutions, by finding the semicircle in the plane perpendicular
  // to the hinge axis we want this joint to operate in.  That semicircle is defined as whichever it
  // would be in were the joint at its 'direction hint' orientation - so this can still cause flipping
  // if the direction hint direction is close to parallel to the direction to the root joint.  In other
  // words, you should select direction hint orientations which tend to put the 2-bone system at
  // right-angles.  Otherwise you're not really saying much about which way you want the joint to
  // bend, are you?
  if (jointParams.directionHintQuat.w <= 1.0f)
  {
    NMP::Vector3 newEndEffectorVector = jointRotation.rotateVector(twoBoneJoints[2].t);
    NMP::Vector3 testDirection;
    testDirection.cross(hingeAxis, twoBoneJoints[0].t);
    NMP::Vector3 desiredDirection = (~joint.q * jointParams.directionHintQuat).rotateVector(twoBoneJoints[2].t);
    // Comparing dot products tells us whether to switch to the other solution
    if (testDirection.dot(desiredDirection) * testDirection.dot(newEndEffectorVector) < 0)
    {
      jointRotation.fromAxisAngle(hingeAxis, angle2 * twoBoneWeight);
      newQ = joint.q * jointRotation;
    }

#define DEBUG_DRAW_TWOBONE_SOLUTION_CHOICE_VECTORS
#if defined(NMRU_HYBRIDIK_TWOBONESOLVE_DEBUGDRAW) && defined(DEBUG_DRAW_TWOBONE_SOLUTION_CHOICE_VECTORS)
    if (solverData.debugBuffer)
    {
      GeomUtils::PosQuat jointMovingFrameWorld = solverData.globalJointTM;
      jointMovingFrameWorld.q.multiply(joint.q);
      NMP::Vector3 testDirectionWorld = jointMovingFrameWorld.q.rotateVector(testDirection) + jointMovingFrameWorld.t;
      solverData.debugBuffer->drawDashedLine(jointMovingFrameWorld.t, testDirectionWorld, 0.02f, NMP::Vector3(0.0f, 1.0f, 0.0f));
      NMP::Vector3 desiredDirectionWorld = jointMovingFrameWorld.q.rotateVector(desiredDirection) + jointMovingFrameWorld.t;
      solverData.debugBuffer->drawDashedLine(jointMovingFrameWorld.t, desiredDirectionWorld, 0.04f, NMP::Vector3(1.0f, 0.0f, 0.0f));
      NMP::Vector3 newEEWorld = jointMovingFrameWorld.q.rotateVector(newEndEffectorVector) + jointMovingFrameWorld.t;
      solverData.debugBuffer->drawDashedLine(jointMovingFrameWorld.t, newEEWorld, 0.1f, NMP::Vector3(1.0f, 0.0f, 0.0f));
    }
#endif
  }

  joint.q = newQ;
  joint.q.fastNormalise();

} // end of function HybridIK::solveTwoBoneHinge()

//----------------------------------------------------------------------------------------------------------------------
void HybridIK::jointApplyBindPose(
  bool isPrimary,
  bool HYBRIDIK_TERTIARIES_ONLY_ARG(redundancyOnly),
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params)
{
  // Help for the compiler optimising away the tertiaries code
#ifdef NMIK_NO_TERTIARIES
  static const bool redundancyOnly = false;
#endif

  const uint32_t& ji = solverData.jointIndices[solverData.jointPosition];

  const HybridIK::PerJointParams& jointParams = params->perJointParams[ji];
  GeomUtils::PosQuat& joint = joints[ji];

  // BIND POSE CONSTRAINT
  // Applied as a partial rotation
  float bindPoseWeight = jointParams.bindPoseWeight * params->jointBindPoseWeight;
  bool applyBindPose =
    ((isPrimary && !redundancyOnly && params->constraints.isSet(IKCONSTRAINT_PRIMARY_BINDPOSE)) ||
     (!isPrimary && !redundancyOnly && params->constraints.isSet(IKCONSTRAINT_SECONDARY_BINDPOSE)) ||
     ((redundancyOnly && params->constraints.isSet(IKCONSTRAINT_TERTIARY_BINDPOSE)) &&
     bindPoseWeight > 0 && jointParams.bindPose.magnitudeSquared() > 0));
  if (applyBindPose)
  {
    // We can't do swivel control for hinge joints unless the hinge axis and the swivel axis happen
    // to line up (like in a roll bone).  Also the various other provisos for conditioning.
#ifndef NMIK_NO_TERTIARIES
    bool coincident = 
      solverData.jointPosition == (solverData.numJoints - 1) ||
      solverData.localEndEffector.t.magnitudeSquared() < GeomUtils::gTolSq ||
      solverData.localTarget.t.magnitudeSquared() < GeomUtils::gTolSq;
    bool canSwivel =
      !coincident &&
      (!jointParams.isHinge || swivelAxis.distanceSquaredTo(jointParams.hingeAxis) < GeomUtils::nTolSq);

    NMP::Quat q;
    if (redundancyOnly && canSwivel)
    {
      q = GeomUtils::closestRotationAroundAxis(jointParams.bindPose * ~joint.q, swivelAxis) * joint.q;
    }
    else
#else
    NMP::Quat q;
#endif
    {
      if (jointParams.isHinge)
      {
        q = GeomUtils::closestRotationAroundAxis(jointParams.bindPose * ~joint.q, jointParams.hingeAxis) * joint.q;
      }
      else
      {
        q = jointParams.bindPose;
      }
    }

    // Do the partial rotation via slerp
    GeomUtils::shortestArcFastSlerp(joint.q, q, NMP::clampValue(bindPoseWeight, 0.0f, 1.0f));
  }

} // end of HybridIK::jointApplyBindPose()

//----------------------------------------------------------------------------------------------------------------------
void HybridIK::solveJointOrientations(
  bool isPrimary,
  bool HYBRIDIK_TERTIARIES_ONLY_ARG(redundancyOnly),
  float& weightOut,
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params)
{
  // Help for the compiler optimising away the tertiaries code
#ifdef NMIK_NO_TERTIARIES
  static const bool redundancyOnly = false;
#endif

  const uint32_t& ji = solverData.jointIndices[solverData.jointPosition];

  const HybridIK::PerJointParams& jointParams = params->perJointParams[ji];
  GeomUtils::PosQuat& joint = joints[ji];
  NMP::Quat newQ = joint.q;
  NMP::Quat startQ = joint.q;
  NMP::Quat resultQ(0, 0, 0, 0);

  // We can't do swivel control for hinge joints unless the hinge axis and the swivel axis happen
  // to line up (like in a roll bone).  Also the various other provisos for conditioning.
#ifndef NMIK_NO_TERTIARIES
  bool coincident = 
    solverData.jointPosition == solverData.numJoints-1 ||
    solverData.localEndEffector.t.magnitudeSquared() < GeomUtils::gTolSq ||
    solverData.localTarget.t.magnitudeSquared() < GeomUtils::gTolSq;
  bool canSwivel =
    !coincident &&
    (!jointParams.isHinge || swivelAxis.distanceSquaredTo(jointParams.hingeAxis) < GeomUtils::nTolSq);
#endif

  // Initialise output weight
  weightOut = 0;

  // POLE VECTOR CONTROL
  bool solvePoleVectors =
    params->endEffectorPoleVectorParams.active &&
    ((isPrimary && !redundancyOnly && params->constraints.isSet(IKCONSTRAINT_PRIMARY_POLEVECTOR)) ||
     (!isPrimary && !redundancyOnly && params->constraints.isSet(IKCONSTRAINT_SECONDARY_POLEVECTOR)) ||
     (redundancyOnly && params->constraints.isSet(IKCONSTRAINT_TERTIARY_POLEVECTOR)));
  if (solvePoleVectors)
  {
    NMP::Quat eeFFQ = newQ * solverData.localEndEffector.q;
    NMP::Vector3 controlVector = eeFFQ.rotateVector(params->endEffectorPoleVectorParams.controlPoleVector);
    NMP::Vector3 referenceVector =
      solverData.localTarget.q.rotateVector(params->endEffectorPoleVectorParams.referencePoleVector);
    NMP::Quat endOrientate(NMP::Quat::kIdentity);
#ifndef NMIK_NO_TERTIARIES
    if (redundancyOnly)
    {
      if (canSwivel)
      {
        endOrientate = GeomUtils::forRotationAroundAxisSoft(
          controlVector, referenceVector, swivelAxis, xHingeSoftness, xFlipSoftness);
      }
      // Special case for end joint where we can do a general rotation because we're not restricted by
      // a swivel axis
      else if (coincident && !jointParams.isHinge)
      {
        endOrientate = GeomUtils::forRotationSoft(controlVector, referenceVector, xFlipSoftness);
      }
    }
    else
#endif
    if (jointParams.isHinge)
    {
      endOrientate = GeomUtils::forRotationAroundAxisSoft(
        controlVector, referenceVector, jointParams.hingeAxis, xHingeSoftness, xFlipSoftness);
    }
    else
    {
      endOrientate = GeomUtils::forRotationSoft(controlVector, referenceVector, xFlipSoftness);
    }

    endOrientate.multiply(newQ);

    // Apply weight
    endOrientate *= params->endEffectorPoleVectorParams.weight * jointParams.endEffectorPoleVectorWeight;
    GeomUtils::quatSelect(endOrientate, resultQ.dot(endOrientate));
    resultQ += endOrientate;

    // Set weight result
    weightOut += params->endEffectorPoleVectorParams.weight * jointParams.endEffectorPoleVectorWeight;

    // Debug draw of pole vectors
#ifdef NMRU_HYBRIDIK_CCDSOLVER_DEBUGDRAW_POLES
    if (solverData.debugBuffer)
    {
      NMP::Vector3 refV = solverData.globalJointTM.q.rotateVector(referenceVector);
      NMP::Vector3 controlV = solverData.globalJointTM.q.rotateVector(controlVector);
      solverData.debugBuffer->drawDashedLine(
        solverData.globalJointTM.t, solverData.globalJointTM.t + (controlV * 0.1f), 0.04f);
      solverData.debugBuffer->drawDashedLine(
        solverData.globalJointTM.t, solverData.globalJointTM.t + (refV * 0.2f), 0.04f);
    }
#endif
  }

  // ORIENTATION
  newQ = startQ;
  bool solveOrientation =
    (isPrimary && !redundancyOnly && params->constraints.isSet(IKCONSTRAINT_PRIMARY_ORIENTATION)) ||
    (!isPrimary && !redundancyOnly && params->constraints.isSet(IKCONSTRAINT_SECONDARY_ORIENTATION)) ||
    (redundancyOnly && params->constraints.isSet(IKCONSTRAINT_TERTIARY_ORIENTATION));
  if (solveOrientation)
  {
    NMP::Quat eeFFQ = newQ * solverData.localEndEffector.q;
    NMP::Quat endOrientate = newQ;
#ifndef NMIK_NO_TERTIARIES
    if (redundancyOnly)
    {
      if (canSwivel)
      {
        endOrientate = GeomUtils::closestRotationAroundAxis(solverData.localTarget.q * ~eeFFQ, swivelAxis) * newQ;
      }
      // Special case for end joint where we can do a general rotation because we're not restricted by
      // a swivel axis
      else if (coincident && !jointParams.isHinge)
      {
        endOrientate = solverData.localTarget.q * ~solverData.localEndEffector.q;
      }
    }
    else
#endif
    if (jointParams.isHinge)
    {
      endOrientate =
        GeomUtils::closestRotationAroundAxis(solverData.localTarget.q * ~eeFFQ, jointParams.hingeAxis) * newQ;
    }
    else
    {
      endOrientate = solverData.localTarget.q * ~solverData.localEndEffector.q;
    }

    // Apply weight and add to this joint's rotation
    endOrientate *= params->targetOrientationWeight * jointParams.orientationWeight;
    GeomUtils::quatSelect(endOrientate, resultQ.dot(endOrientate));
    resultQ += endOrientate;

    // Set weight result
    weightOut += params->targetOrientationWeight * jointParams.orientationWeight;
  }

  // Blend pole vector and orientation results
  newQ = startQ;
  if (resultQ.magnitudeSquared() > 0)
  {
    resultQ.fastNormalise();
    newQ = resultQ;
  }

  // Copy back result
  joint.q = newQ;

} // end of HybridIK::solveJointOrientations()

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_UNIT_TESTING
  #include <cstring>
bool HybridIK::PerJointParams::saveCode(char prefixString[], std::ostream& stream)
{
  const int DUMPSIZE = 1024;
  const int PREFIXSIZE = 100;
  int n;
  char dump[DUMPSIZE];
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sbindPose.setXYZW(%ff, %ff, %ff, %ff);\n", prefixString, bindPose.x, bindPose.y, bindPose.z, bindPose.w);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sdirectionHintQuat.setXYZW(%ff, %ff, %ff, %ff);\n", prefixString, directionHintQuat.x, directionHintQuat.y, directionHintQuat.z, directionHintQuat.w);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sisHinge = %s;\n", prefixString, isHinge ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  if (isHinge)
  {
    n = NMP_SPRINTF(dump, DUMPSIZE, "%shingeAxis.set(%ff, %ff, %ff);\n", prefixString, hingeAxis.x, hingeAxis.y, hingeAxis.z);
    if (n < 0 || n >= DUMPSIZE) return false;
    stream << dump;
  }
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sweight = %ff;\n", prefixString, weight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%spositionWeight = %ff;\n", prefixString, positionWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sendEffectorPoleVectorWeight = %ff;\n", prefixString, endEffectorPoleVectorWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sorientationWeight = %ff;\n", prefixString, orientationWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sredundancyControlWeight = %ff;\n", prefixString, redundancyControlWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sbindPoseWeight = %ff;\n", prefixString, bindPoseWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sdoTwoBoneSolve = %s;\n", prefixString, doTwoBoneSolve ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sisLimited = %s;\n", prefixString, isLimited ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sapplyHardLimits = %s;\n", prefixString, applyHardLimits ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sapplySoftLimits = %s;\n", prefixString, applySoftLimits ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%shardLimitRepulsionFactor = %ff;\n", prefixString, hardLimitRepulsionFactor);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%ssoftLimitSoftness = %ff;\n", prefixString, softLimitSoftness);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  char prefix[PREFIXSIZE];
  if (isLimited)
  {
    if (NMP_STRNCPY_S(prefix, PREFIXSIZE, prefixString) != 0) return false;
    if (NMP_STRNCAT_S(prefix, PREFIXSIZE, "limits.") != 0) return false;
    if (!limits.saveCode(prefix, stream)) return false;
  }
  n = NMP_SPRINTF(dump, DUMPSIZE, "%senabled = %s;\n", prefixString, enabled ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HybridIK::Params::saveCode(char prefixString[], std::ostream& stream, uint32_t numJoints)
{
  const int DUMPSIZE = 1024;
  const int PREFIXSIZE = 100;
  int n;
  char dump[DUMPSIZE];
  n = NMP_SPRINTF(dump, DUMPSIZE, "%smaxIterations = %d;\n", prefixString, maxIterations);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  char prefix[PREFIXSIZE];
  if (NMP_STRNCPY_S(prefix, PREFIXSIZE, prefixString) != 0) return false;
  if (NMP_STRNCAT_S(prefix, PREFIXSIZE, "endEffectorPoleVectorParams.") != 0) return false;
  if (!endEffectorPoleVectorParams.saveCode(prefix, stream)) return false;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sreverseDirectionOfSolve = %s;\n", prefixString, reverseDirectionOfSolve ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sconstraints = %ul;\n", prefixString, constraints);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sreverseDirectionOfSolve = %s;\n", prefixString, reverseDirectionOfSolve ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%straverseOneWayOnly = %s;\n", prefixString, traverseOneWayOnly ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sdirectionHintsFactor = %ff;\n", prefixString, directionHintsFactor);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%stargetPositionWeight = %ff;\n", prefixString, targetPositionWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%stargetOrientationWeight = %ff;\n", prefixString, targetOrientationWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sjointBindPoseWeight = %ff;\n", prefixString, jointBindPoseWeight);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%saccuracy = %ff;\n", prefixString, accuracy);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sorientationAccuracy = %ff;\n", prefixString, orientationAccuracy);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    n = NMP_SPRINTF(prefix, PREFIXSIZE, "%sperJointParams[%d].", prefixString, j);
    if (n < 0 || n >= PREFIXSIZE) return false;
    if (!perJointParams[j].saveCode(prefix, stream)) return false;
  }
  return true;
}
#endif

} // end of namespace NMRU

//----------------------------------------------------------------------------------------------------------------------

