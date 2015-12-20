// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erLimbIK.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "euphoria/erValueValidators.h"
#include "euphoria/erDebugControls.h"
#include "euphoria/erLimbDef.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "NMIK/NMHybridIK.h"

#include "physics/mrPhysicsRigDef.h"

//----------------------------------------------------------------------------------------------------------------------
#define HINGE_SWING_LIMIT 0

// If this is defined then the IK effectiveness will be artifically reduced when the timestep is
// small so that it converges at approximately the same rate as the behaviours were originally tuned
// at.
#define SCALE_IK_EFFECTIVENESSx

#if defined(MR_OUTPUT_DEBUGGING)
// only for debug draw
static const float drawMatrixSizeNormal = 0.2f;
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// LimbIK implementation starts here
//----------------------------------------------------------------------------------------------------------------------
const LimbIK::OperatingParams s_defaultOP;

LimbIK::OperatingParams::OperatingParams()
{
  m_positionWeight = 1.0f;
  m_orientationWeight = 1.0f;
  m_normalWeight = 0.0f;

  m_neutralPoseWeight = 0.0f;
  m_useGuidePose = false;
  for (int i = 0 ; i < s_maxNumJoints ; ++i)
  {
    m_positionWeights[i] = 1.0f;
    m_orientationWeights[i] = 1.0f;
    m_guidePoseWeights[i] = 0.0f;
  }

  m_limitsEnabled = true;

#if defined(MR_OUTPUT_DEBUGGING)
  m_NMIKDebugDrawStep = 0;
  m_NMIKTestSolve = false;
  m_NMIKTestTargetTranslationX = 0;
  m_NMIKTestTargetTranslationY = 0;
  m_NMIKTestTargetTranslationZ = 0;
  m_NMIKTestTargetRotationX = 0;
  m_NMIKTestTargetRotationY = 0;
  m_NMIKTestTargetRotationZ = 0;
  m_NMIKTestSolveForceContinuous = true;
#endif
  m_maxIterations = 1;
}

// globals

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::resetInternalState()
{
  const NMP::Quat zeroQuat (NMP::Quat::kIdentity);
  for (int i = 0; i < m_numJoints; ++i)
  {
    // Move orientation into the parent frame
    m_ikParams->perJointParams[i].bindPose = m_ikParams->perJointParams[i].limits.frame * zeroQuat;
  }
  for (int i = 0; i < m_numJoints + 1; ++i)
  {
    m_bodyMatrix[i].identity();
  }

  m_lastSolutionEndEffectorTM.identity();
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::init(const MR::PhysicsRigDef* physicsRigDef, const ER::LimbDef* limbDef)
{
  m_numJoints = limbDef->m_numJointsInChain;
  m_joint = (JointDefinition*) NMPMemoryAlloc(sizeof(JointDefinition) * m_numJoints);
  NMP_ASSERT(m_joint);
  m_bodyMatrix = (NMP::Matrix34*) NMPMemoryAlloc(sizeof(NMP::Matrix34) * (m_numJoints + 1));
  NMP_ASSERT(m_bodyMatrix);
  m_finalQuats = (NMP::Quat*) NMPMemoryAlloc(sizeof(NMP::Quat) * m_numJoints);
  NMP_ASSERT(m_finalQuats);

  // operating params
  m_OP = s_defaultOP;

  m_rootOffset.identity();
  m_endOffset.identity();

  // initialise internal joint representation
  for (int i = 0; i < m_numJoints; i++)
  {
    int index = limbDef->getPhysicsRigJointIndex(i);
    const MR::PhysicsSixDOFJointDef* jointDef =
      static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[index]);

    m_joint[i].parentBodyToJoint = jointDef->m_parentPartFrame;
    m_joint[i].childBodyToJointInv = jointDef->m_childPartFrame;
    m_joint[i].childBodyToJointInv.invert();

    NMP_ASSERT(m_joint[i].parentBodyToJoint.isValidTM(0.01f));
    NMP_ASSERT(m_joint[i].childBodyToJointInv.isValidTM(0.01f));
  }

  // NMIK initialisation
  // NMIK has one extra joint to account for the offset end effector
  m_numIKJoints = m_numJoints + 1;

  // Allocate and assign the joint index array
  m_ikJointIndices = (uint32_t*)NMPMemoryAllocAligned(
    sizeof(uint32_t) * m_numIKJoints, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(m_ikJointIndices);
  for (uint32_t j = 0; j < m_numIKJoints; ++j)
  {
    m_ikJointIndices[j] = j;
  }

  // Allocate NMIK geometry array
  NMP::Memory::Format ikGeomMemFormat(sizeof(NMRU::GeomUtils::PosQuat), NMP_VECTOR_ALIGNMENT);
  ikGeomMemFormat *= m_numIKJoints;
  m_ikGeometry = (NMRU::GeomUtils::PosQuat*)NMPMemoryAllocAligned(
    ikGeomMemFormat.size, NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(m_ikGeometry);

#if defined(MR_OUTPUT_DEBUGGING)
  m_debugLines = 0;

  // Allocate test solve independent geometry
  m_ikTestSolveGeometry = (NMRU::GeomUtils::PosQuat*)NMPMemoryAllocAligned(
    ikGeomMemFormat.size, NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(m_ikTestSolveGeometry);

  // Allocate "scratch space" for IK debug draw (drawing each iteration of the solve e.g.)
  void* memoryBlock = NMP::Memory::memAllocAligned(sizeof(NMRU::GeomUtils::DebugDraw), NMP_VECTOR_ALIGNMENT);
  m_debugLines = new(memoryBlock) NMRU::GeomUtils::DebugDraw();
  m_debugLines->restrictToTag = m_OP.m_NMIKDebugDrawStep; // Set the tag to restrict drawing

  m_debugDrawData.root.identity();
  m_debugDrawData.end.identity();
  m_debugDrawData.jdd = (JointDrawData*)NMP::Memory::memAllocAligned(sizeof(JointDrawData)* m_numJoints, NMP_VECTOR_ALIGNMENT);

#endif

  // Fill the NMIK geometry
  NMRU::GeomUtils::PosQuat parentJointToParentBodyT = NMRU::GeomUtils::identityPosQuat();
  for (uint32_t j = 0; j < (unsigned)m_numJoints; ++j)
  {
    int index = limbDef->getPhysicsRigJointIndex(j);
    const MR::PhysicsSixDOFJointDef* jointDef =
      static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[index]);

    NMRU::GeomUtils::fromMatrix34PosQuat(m_ikGeometry[j], jointDef->m_parentPartFrame);
    NMRU::GeomUtils::multiplyInversePosQuat(m_ikGeometry[j], parentJointToParentBodyT);
    NMRU::GeomUtils::fromMatrix34PosQuat(parentJointToParentBodyT, jointDef->m_childPartFrame);
    m_ikGeometry[j].q.normalise();
  }

  // End effector 'joint' goes from end joint to end body
  // We'll tack on the end effector offset later (we don't know it yet)
  m_ikGeometry[m_numIKJoints - 1] = parentJointToParentBodyT;
  NMRU::GeomUtils::invertPosQuat(m_ikGeometry[m_numIKJoints - 1]);
  m_ikGeometry[m_numIKJoints - 1].q.normalise();
#if defined(MR_OUTPUT_DEBUGGING)
  // Copy to test solve geometry
  memcpy(m_ikTestSolveGeometry, m_ikGeometry, ikGeomMemFormat.size);
#endif

  // Allocate HybridIK params object
  NMP::Memory::Format ikParamsMemFormat(NMRU::HybridIK::Params::getMemoryRequirements(m_numIKJoints));
  NMP::Memory::Resource ikParamsMemResource;
  ikParamsMemResource.format = ikParamsMemFormat;
  ikParamsMemResource.ptr = (NMRU::HybridIK::Params*)NMPMemoryAllocAligned(
    ikParamsMemFormat.size, NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(ikParamsMemResource.ptr);

  // Initialise the IK solver parameters object with this memory
  m_ikParams = NMRU::HybridIK::Params::init(&ikParamsMemResource, m_numIKJoints);
  if (m_ikParams)
  {
    m_ikParams->init(m_numIKJoints);
  }

  // Set the joint limit parameters, since they are done once only on initialisation.
  // Allocate storage so we can save the original limits somewhere, then we can modify them for scale
  // and other settings
  NMP::Memory::Format ikLimitsMemFormat(sizeof(NMRU::JointLimits::Params), NMP_VECTOR_ALIGNMENT);
  ikLimitsMemFormat *= m_numJoints;
  m_ikLimits = (NMRU::JointLimits::Params*)NMPMemoryAllocAligned(
    ikLimitsMemFormat.size, NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(m_ikLimits);
  for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
  {
    int index = limbDef->getPhysicsRigJointIndex(j);
    const MR::PhysicsSixDOFJointDef* jointDef =
      static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[index]);

    setSingleJointLimits(j, NMP::Vector3(
        jointDef->limits().getTwistLimitLow(),
        jointDef->limits().getSwing2Limit(),
        jointDef->limits().getSwing1Limit(),
        jointDef->limits().getTwistLimitHigh()));

    // Set limit frame to joint orientation so, in physics terms, current joint rotation is nil
    NMRU::JointLimits::Params& ikLimits = m_ikParams->perJointParams[j].limits;
    ikLimits.frame = m_ikGeometry[j].q;
    ikLimits.offset.identity();
  }

  // Initialise the finalQuats
  for (uint32_t j = 0; j < (unsigned)m_numJoints; ++j)
  {
    m_finalQuats[j] = ~m_ikParams->perJointParams[j].limits.frame * m_ikGeometry[j].q;
    m_finalQuats[j].normalise();
  }

  // Set the direction hints parameters, which are fixed so should not be being set every update,
  // and are therefore factored out of setNMIKParams
  for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
  {
    NMRU::HybridIK::PerJointParams& jointParams = m_ikParams->perJointParams[j];
    jointParams.directionHintQuat = NMRU::JointLimits::centre(jointParams.limits);
  }

  // Disable the end joint, which doesn't rotate, it's just an end effector
  m_ikParams->perJointParams[m_numIKJoints-1].enabled = false;

  // setup bitArrays for debug-draw, feature, and override flags
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Memory::Resource debugFlagMem =
    NMPMemoryAllocateFromFormat(NMP::BitArray::getMemoryRequirements(kNumDebugDrawFlags));
  m_debugDrawFlags = NMP::BitArray::init(debugFlagMem, kNumDebugDrawFlags);
  m_debugDrawFlags->clearAll();

  NMP::Memory::Resource featureFlagMem =
    NMPMemoryAllocateFromFormat(NMP::BitArray::getMemoryRequirements(kNumFeatureFlags));
  m_featureFlags = NMP::BitArray::init(featureFlagMem, kNumFeatureFlags);
  m_featureFlags->setAll();
#endif

  // this needs to be done before setNMIKParams, as the latter uses the offsets to calculate toEndGradation etc.
  setRootAndEndOffsets(limbDef->m_rootOffset, limbDef->m_endOffset);
  setNMIKParams();

#if defined(MR_OUTPUT_DEBUGGING)
  m_limbIndex = MR::INVALID_LIMB_INDEX;
#endif
  resetInternalState();

  assertIsInitialised(this);
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setRootAndEndOffsets(const NMP::Matrix34& rootOffset, const NMP::Matrix34& endOffset)
{
  m_rootOffset = rootOffset;
  m_endOffset = endOffset;

  // Modify NMIK geometry on the basis of the offsets
  NMRU::GeomUtils::PosQuat trueEndEffectorToCurrentEndEffector;
  NMRU::GeomUtils::fromMatrix34PosQuat(trueEndEffectorToCurrentEndEffector, endOffset);
  NMRU::GeomUtils::premultiplyPosQuat(trueEndEffectorToCurrentEndEffector, m_ikGeometry[m_numIKJoints - 1]);
#if defined(MR_OUTPUT_DEBUGGING)
  m_ikTestSolveGeometry[m_numIKJoints - 1] = m_ikGeometry[m_numIKJoints - 1];
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::deinit()
{
  NMP::Memory::memFree(m_joint);
  NMP::Memory::memFree(m_bodyMatrix);
  NMP::Memory::memFree(m_finalQuats);

  // NMIK
  NMP::Memory::memFree(m_ikJointIndices);
  NMP::Memory::memFree(m_ikGeometry);
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Memory::memFree(m_ikTestSolveGeometry);
  m_debugLines->~DebugDraw();
  NMP::Memory::memFree(m_debugLines);
  NMP::Memory::memFree(m_debugDrawData.jdd);
#endif

  NMP::Memory::memFree(m_ikParams);
  NMP::Memory::memFree(m_ikLimits);
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Memory::memFree(m_debugDrawFlags);
  NMP::Memory::memFree(m_featureFlags);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::getFinalQuat(int j, NMP::Quat& q) const
{
  NMP_ASSERT(0 <= j && j < getNumJointAngles());
  q = m_finalQuats[j];
  NMP_ASSERT(NMP::nmfabs(q.magnitudeSquared() - 1.0f) < 1e-3f);
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setInitialJointQuats(NMP::Quat* initialPose)
{
  for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
  {
    // Final quats are in the limit frame
    m_finalQuats[j] = initialPose[j];
    m_ikGeometry[j].q = m_ikParams->perJointParams[j].limits.frame * initialPose[j];
#if defined(MR_OUTPUT_DEBUGGING)
    if (!m_OP.m_NMIKTestSolve || !m_OP.m_NMIKTestSolveForceContinuous)
    {
      m_ikTestSolveGeometry[j].q = m_ikGeometry[j].q;
    }
#endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setGuidePose(const NMP::Quat* guidePose)
{
  for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
  {
    // The orientation is in the limit frame, so I have to move it into the parent frame
    // OPTIMISE: If the guide pose and limits.frame are unchanging then needn't recompute this each frame.
    m_ikParams->perJointParams[j].bindPose = m_ikParams->perJointParams[j].limits.frame * guidePose[j];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::getGuidePose(NMP::Quat* guidePose)
{
  for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
  {
    guidePose[j] = ~m_ikParams->perJointParams[j].limits.frame * m_ikParams->perJointParams[j].bindPose;
  }
}

#if defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
// Calculates limb joint data (root and joint transforms) for use by drawLimb()
//----------------------------------------------------------------------------------------------------------------------
void LimbIK::calcLimbDrawInfo(const NMP::Quat* quats, LimbDrawData& ldd) const
{
  // store the limb root
  NMP::Matrix34 TM;
  ldd.root = TM = m_bodyMatrix[0];
  // compute the remaining joint TMs (in world frame coordinates)
  for (int i = 0; i < m_numJoints; i++)
  {
    // joint frame tm's
    ldd.jdd[i].jTMw.multiply(m_joint[i].parentBodyToJoint, TM);
    NMP::Matrix34 j_TMj(quats[i], NMP::Vector3(0, 0, 0));
    ldd.jdd[i].j_TMw.multiply(j_TMj, ldd.jdd[i].jTMw);
    if (i == m_numJoints - 1)
    {
      NMP::Matrix34 bTM_j = m_joint[i].childBodyToJointInv;
      bTM_j.multiply(m_endOffset, bTM_j);
      TM.multiply(bTM_j, ldd.jdd[i].j_TMw);
    }
    else
    {
      const NMP::Matrix34& bTM_j = m_joint[i].childBodyToJointInv;
      TM.multiply(bTM_j, ldd.jdd[i].j_TMw);
    }
  }
  ldd.end = TM;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setLimbIndex(MR::LimbIndex limbIndex)
{
  m_limbIndex = limbIndex;
}

//----------------------------------------------------------------------------------------------------------------------
// Externally callable limb drawing API.
// Eg erLimb can call this to debug draw a limb given a set of quats
//----------------------------------------------------------------------------------------------------------------------
void LimbIK::drawLimb(
  MR::InstanceDebugInterface* pDebugDrawInst,
  const NMP::Quat* quats,
  const NMP::Matrix34& offsetForDebug,
  const NMP::Vector3& boneColour, bool drawLimits, bool drawJointFrames,
  const DimensionalScaling& dimensionalScaling) const
{
  LimbDrawData ldd;
  ldd.jdd = (JointDrawData*)alloca((m_numJoints) * sizeof(JointDrawData));
  calcLimbDrawInfo(quats, ldd);
  drawLimb(pDebugDrawInst, ldd, offsetForDebug, boneColour, drawLimits, drawJointFrames, dimensionalScaling);
}

//----------------------------------------------------------------------------------------------------------------------
// Private limb drawing API for internal use.
// Aux for the public version or something for LimbIK to call multiple times without necessarily
// having to realloc or recalc limb matrix data
//----------------------------------------------------------------------------------------------------------------------
void LimbIK::drawLimb(
  MR::InstanceDebugInterface* pDebugDrawInst,
  const LimbDrawData&         ldd,
  const NMP::Matrix34&        offsetForDebug,
  const NMP::Vector3&         boneColour,
  bool                        drawLimits,
  bool                        drawJointFrames,
  const DimensionalScaling&   dimensionalScaling) const
{
  if (pDebugDrawInst == 0)
  {
    return;
  }

  // line from root to first joint fixed frame
  MR_DEBUG_DRAW_LINE(
    pDebugDrawInst,
    offsetForDebug.getTransformedVector(ldd.root.translation()),
    offsetForDebug.getTransformedVector(ldd.jdd[0].jTMw.translation()),
    NMP::Colour(boneColour));

  for (int i = 0; i < m_numJoints; i++)
  {
    if (i < m_numJoints - 1)
    {
      // line from ith moving joint to (i+1)th fixed joint
      MR_DEBUG_DRAW_LINE(
        pDebugDrawInst,
        offsetForDebug.getTransformedVector(ldd.jdd[i].j_TMw.translation()),
        offsetForDebug.getTransformedVector(ldd.jdd[i+1].jTMw.translation()),
        NMP::Colour(boneColour));
    }
    else
    {
      // last joint to end effector
      MR_DEBUG_DRAW_LINE(
        pDebugDrawInst,
        offsetForDebug.getTransformedVector(ldd.jdd[m_numJoints - 1].j_TMw.translation()),
        offsetForDebug.getTransformedVector(ldd.end.translation()),
        NMP::Colour(boneColour));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, ldd.end * offsetForDebug, 
        SCALE_DIST(drawMatrixSizeNormal));
    }

    // if required draw the joint limit (with twist and swing indicator needles)
    if (drawLimits)
    {
      drawLimit(pDebugDrawInst, i, ldd.jdd[i], SCALE_DIST(0.1f));
    }

    // if required draw the (parent and child) joint frames
    if (drawJointFrames)
    {
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, offsetForDebug * ldd.jdd[i].jTMw, 
        SCALE_DIST(drawMatrixSizeNormal));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, offsetForDebug * ldd.jdd[i].j_TMw, 
        SCALE_DIST(drawMatrixSizeNormal));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for the limbDraw API.
// Draws limit graphics (in a colour that indicates how close current state is to the limit)
//----------------------------------------------------------------------------------------------------------------------
void LimbIK::drawLimit(MR::InstanceDebugInterface* pDebugDrawInst, int jointId, const JointDrawData& jdd, float size) const
{
  // Current state comes from orientation offset between parent and child joint frames (j and j_ say),
  // i.e. j_TMj = j_TMw . wTMj
  NMP::Matrix34 j_TMw, wTMj, j_TMj;
  j_TMw = jdd.j_TMw;
  wTMj.invertFast(jdd.jTMw);
  j_TMj.multiply(j_TMw, wTMj);

  // limits to check against
  float swingDistSq, twistDist;
  NMRU::JointLimits::Params jointLimits = m_ikLimits[jointId];
  jointLimits.frame.identity();

  // swing and twist graphic colours (derived from degree of violation from distance to limit boundary)
  NMRU::JointLimits::degreeOfViolationTwistSwingSqSimple(jointLimits, j_TMj.toQuat(), twistDist, swingDistSq);
  twistDist = NMP::sqr(NMP::clampValue(twistDist, -1.0f, 1.0f));
  twistDist *= twistDist;
  swingDistSq = NMP::sqr(NMP::clampValue(swingDistSq, 0.0f, 1.0f));
  NMP::Vector3 coneCol(swingDistSq, 1 - swingDistSq, 0.0f);
  NMP::Vector3 wedgeCol(twistDist, 1 - twistDist, 0.0f);

  // fish out the radian joint limits (note a slight swizzle is required on the twist limits for correct drawing)
  float twistMin = -jointLimits.getTwistMax();
  float twistMax = -jointLimits.getTwistMin();
  float swing2Limit = jointLimits.getSwing1Limit();
  float swing1Limit = jointLimits.getSwing2Limit();

  // some shared params
  int numSegments = 32;
  const NMP::Matrix34& frame1World = jdd.jTMw;
  NMP::Vector3 centre(frame1World.translation());
  const float twoPi = 2.0f * NM_PI;
  NMP::Vector3 lastPos;

  // Limit cone
  // draw swing cone
  for (int i = 0; i < numSegments; i++)
  {
    float angle = i * twoPi / (float)(numSegments - 1);
    NMP::Vector3 coord(0.0f, swing2Limit * sinf(angle), swing1Limit * cosf(angle));
    float amount = coord.magnitude();
    coord.normalise();
    NMP::Vector3 pos;
    pos.x = cosf(amount) * size;
    pos.y = sinf(amount) * size * coord.y;
    pos.z = sinf(amount) * size * coord.z;
    pos.transform(frame1World);

    if (i)
    {
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, lastPos, pos, NMP::Colour(coneCol, 255));
    }
    if (i == 0 || i == numSegments / 2 || i == numSegments / 4 || i == numSegments * 3 / 4)
    {
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, centre, pos, NMP::Colour(coneCol, 255));
    }
    lastPos = pos;
  }

  // Circle segment
  // draw twist wedge
  for (int i = 0; i < numSegments / 4; i++)
  {
    float angle = twistMin + (twistMax - twistMin) * (float)i / ((float)(numSegments / 4) - 1.0f);
    NMP_ASSERT(twistMax >= twistMin);
    NMP::Vector3 pos;
    pos.x = 0.0f;
    pos.y = size * 0.5f * cosf(angle);
    pos.z = size * 0.5f * sinf(angle);
    pos.transform(frame1World);

    if (i)
    {
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, lastPos, pos, NMP::Colour(wedgeCol, 255));
    }
    if (i == 0 || i == (numSegments / 4) - 1)
    {
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, centre, pos, NMP::Colour(wedgeCol, 255));
    }
    lastPos = pos;
  }

  // draw twist swing direction indicators
  drawTwistSwingPointers(pDebugDrawInst, jdd.jTMw, jdd.j_TMw , size);
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::drawTwistSwingPointers(
  MR::InstanceDebugInterface* pDebugDrawInst, const NMP::Matrix34& frame1World, const NMP::Matrix34& frame2World,
  float MR_OUTPUT_DEBUG_ARG(size)) const
{
  // grow size by 20% (assumes cones drawing at size, so making needles a little bigger for visibility)
  size *= 1.2f;

  // Calculate the needle directions.
  // Swing direction is just the x-axis (of the child frame)
  NMP::Vector3 swingFrame2 = frame2World.xAxis();
  // Twist needle is the y-axis (of the child frame) with swing component removed
  NMP::Vector3 twistFrame2;
  NMP::Quat rotation;
  rotation.forRotation(frame2World.xAxis(), frame1World.xAxis());
  rotation.rotateVector(frame2World.yAxis(), twistFrame2);

  // positions
  const NMP::Vector3& posFrame2 = frame2World.translation();
  NMP::Vector3 colour (0.9f, 0.9f, 0.9f);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, posFrame2, posFrame2 + swingFrame2 * size, NMP::Colour(colour, 255));
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, posFrame2, posFrame2 + twistFrame2 * size * 0.5f, NMP::Colour(colour, 255));
}

#endif // defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 LimbIK::calcEndEffectorTMFromRootAndJointQuats(
  const NMP::Quat& rootQuat,
  const NMP::Vector3& rootPos,
  const NMP::Quat* jointQuats) const
{
  // bodyMatrix[0] needs to be set before we can FK to end effector
  NMP::Matrix34 invRootOffset = m_rootOffset;
  invRootOffset.invertFast();
  NMP::Matrix34 rootBodyMatrix(rootQuat, rootPos);

  NMP::Matrix34 TM = invRootOffset * rootBodyMatrix;

  for (int i = 0; i < m_numJoints; i++)
  {
    // joint frame tm's
    NMP::Matrix34 j_TMj(jointQuats[i], NMP::Vector3(0, 0, 0));
    NMP::Matrix34 j_TMw = j_TMj * m_joint[i].parentBodyToJoint * TM;

    const NMP::Matrix34& bTM_j = m_joint[i].childBodyToJointInv;
    TM.multiply(bTM_j, j_TMw);
  }

  TM = m_endOffset * TM;
  return TM;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setSingleJointLimitScale(int index, float scale)
{
  NMRU::HybridIK::PerJointParams& jointParams = m_ikParams->perJointParams[index];
  NMRU::JointLimits::Params& ikLimits = jointParams.limits;

  ikLimits.lower = m_ikLimits[index].lower * scale;
  ikLimits.upper = m_ikLimits[index].upper * scale;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setSingleJointLimits(const int index, const NMP::Vector3& limits)
{
  NMP_ASSERT(index >= 0);
  NMP_ASSERT(index < m_numJoints);

  NMRU::HybridIK::PerJointParams& jointParams = m_ikParams->perJointParams[index];
  jointParams.isLimited = true;

  NMRU::JointLimits::Params& ikLimits = jointParams.limits;
  ikLimits.setTwistLeanLimits(-limits.w, -limits.x, limits.y, limits.z);

  // Find hinges and set hinge parameters
  if (ikLimits.upper.y < 1e-3f && ikLimits.upper.z < 1e-3f)
  {
    jointParams.isHinge = true;
    jointParams.setHingeParametersFromLimits(m_ikGeometry[index].t, m_ikGeometry[index+1].t);
    // Set swing limits to zero for hinges
    jointParams.limits.upper.y = HINGE_SWING_LIMIT;
    jointParams.limits.upper.z = HINGE_SWING_LIMIT;
  }

  // Copy limits back into internal structure.
  m_ikLimits[index] = jointParams.limits;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::solve(
  const NMP::Quat&             rootBodyQuat, 
  const NMP::Vector3&          rootBodyPos, 
  const NMP::Quat&             targetQuat,
  const NMP::Vector3&          targetNormal, 
  const NMP::Vector3&          localNormal,
  const NMP::Vector3&          targetPoint, 
  ER::LimbIK::SolverErrorData* solverErrorData,
  const float                  timeDelta,
  MR::InstanceDebugInterface*  MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  const NMP::Matrix34&         MR_OUTPUT_DEBUG_ARG(rootBodyMatrixForDebug),
  const DimensionalScaling&    MR_OUTPUT_DEBUG_ARG(dimensionalScaling))
{
  NMP_ASSERT(targetQuat.isNormal(EUPHORIA_QUAT_ERROR_LIMIT));
  NMP_ASSERT(targetPoint.isValid());
  NMP_ASSERT(rootBodyQuat.isNormal(EUPHORIA_QUAT_ERROR_LIMIT));

  NMP::Matrix34 target(targetQuat, targetPoint);
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Matrix34 rootBodyMatrix(rootBodyQuat, rootBodyPos);
  NMP_ASSERT(rootBodyMatrix.isValidTM(0.001f));

  // bodyMatrix[0] needs to be set before any call to
  NMP::Matrix34 invRootOffset = m_rootOffset;
  invRootOffset.invertFast();
  m_bodyMatrix[0].multiply(invRootOffset, rootBodyMatrix);

  NMP::Matrix34 offsetForDebug(NMP::Matrix34::kIdentity);
  if (getDebugDrawFlag(kDrawRelativeToRoot))
  {
    NMP::Matrix34 rootBodyMatrixInverse(rootBodyMatrix);
    rootBodyMatrixInverse.invertFast();
    offsetForDebug = rootBodyMatrixInverse * rootBodyMatrixForDebug;
  }

  if (getDebugDrawFlag(kDrawTarget))
  {
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawIKTargetsControlID, m_limbIndex);

    NMP::Matrix34 m = target * offsetForDebug;
    MR_DEBUG_DRAW_POINT(pDebugDrawInst,
      targetPoint, 
      SCALE_DIST(0.05f),
      NMP::Colour::WHITE);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, m, 
      SCALE_DIST(m_OP.m_orientationWeight * drawMatrixSizeNormal));
    MR_DEBUG_DRAW_VECTOR(
      pDebugDrawInst,
      MR::VT_Delta,
      m.translation(), 
      offsetForDebug.toQuat().rotateVector(targetNormal * SCALE_DIST(1.0f)) * m_OP.m_normalWeight,
      NMP::Colour::WHITE);
  }

  if (getDebugDrawFlag(kDrawInputPose))
  {
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawIKInputPoseControlID, m_limbIndex);

    NMP::Quat* initialJointQuats = (NMP::Quat*)alloca(m_numJoints * sizeof(NMP::Quat));
    for (int i = 0; i < m_numJoints; ++i)
    {
      initialJointQuats[i] = ~m_ikParams->perJointParams[i].limits.frame * m_ikGeometry[i].q;
    }
    NMP_ASSERT(m_debugDrawData.jdd);
    calcLimbDrawInfo(initialJointQuats, m_debugDrawData);
    drawLimb(pDebugDrawInst, m_debugDrawData, offsetForDebug, 
      NMP::Vector3(0.5f, 0.5f, 1.0f), false, false, dimensionalScaling);
  }

  if (getDebugDrawFlag(kDrawGuidePose))
  {
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawIKGuidePoseControlID, m_limbIndex);

    // grab whatever limbNMIK is currently using as the guide pose
    NMP::Quat* guidePoseQuats = (NMP::Quat*)alloca(m_numJoints * sizeof(NMP::Quat));
    for (int i = 0; i < m_numJoints; ++i)
    {
      guidePoseQuats[i] = ~m_ikParams->perJointParams[i].limits.frame * m_ikParams->perJointParams[i].bindPose;
    }
    NMP_ASSERT(m_debugDrawData.jdd);
    calcLimbDrawInfo(guidePoseQuats, m_debugDrawData);
    drawLimb(pDebugDrawInst, m_debugDrawData, offsetForDebug, 
      NMP::Vector3(0.25f, 0.25f, 1.0f), true, false, dimensionalScaling);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)

  // NMIK SOLVE
#if 1// defined(MR_OUTPUT_DEBUGGING)
  setNMIKParams();
#else
  updateIKTargetWeights();
  setPriorityOrientationConstraint();
#endif

  // Set the end effector normal geometry for NMIK.
  // targetNormal input is wrt world but internally required wrt target frame (we rely on the fact that
  // targetQuat is a valid orientation - asserted on entry above).
  m_ikParams->endEffectorPoleVectorParams.referencePoleVector = targetQuat.inverseRotateVector(targetNormal);
  m_ikParams->endEffectorPoleVectorParams.controlPoleVector = localNormal;

  // Get the transform of the parent of the root joint in the world, used to associate the
  // frame the target is expressed in with the frame of the joint chain
  NMRU::GeomUtils::PosQuat globalRootParentTransform;
  globalRootParentTransform.q = rootBodyQuat;
  globalRootParentTransform.t = rootBodyPos;
  NMRU::GeomUtils::PosQuat rootOffsetT;
  NMRU::GeomUtils::fromMatrix34PosQuat(rootOffsetT, m_rootOffset);
  NMRU::GeomUtils::premultiplyInversePosQuat(rootOffsetT, globalRootParentTransform);

  // Get the target transform in PosQuat form
  NMRU::GeomUtils::PosQuat ikTarget;
  ikTarget.q = targetQuat;
  ikTarget.t = targetPoint;

  // Call the solver
  NMRU::GeomUtils::PosQuat solutionEE;

#if defined(MR_OUTPUT_DEBUGGING)
  // Pre-call the solver for a test solve, just to generate debug output
  if (m_OP.m_NMIKTestSolve)
  {
    // Generate a test IK target
    NMRU::GeomUtils::PosQuat testTarget;
    testTarget.t.set(
      m_OP.m_NMIKTestTargetTranslationX, m_OP.m_NMIKTestTargetTranslationY, m_OP.m_NMIKTestTargetTranslationZ);
    testTarget.q.fromEulerXYZ(
      NMP::Vector3(m_OP.m_NMIKTestTargetRotationX, m_OP.m_NMIKTestTargetRotationY, m_OP.m_NMIKTestTargetRotationZ));

    NMRU::HybridIK::solve(
      m_ikParams,
      m_ikTestSolveGeometry,
      m_ikJointIndices,
      m_numIKJoints,
      testTarget,
      solutionEE,
      &globalRootParentTransform,
      m_debugLines);
  }
#endif

#ifdef SCALE_IK_EFFECTIVENESS
  // MORPH-21516 - This block of code reduces the IK effectiveness when the timestep is small to
  // prevent the improved IK convergence from generating unexpectedly strong limb movements.
  // Effectiveness is the fractional reduction in the error each solver iteration/step. If we scale
  // the IK result, then we scale the effectiveness by the same value. Multiple iterations will
  // multiply the error (1-effectiveness). We want to calculate the effectiveness scale based on the
  // current timestep t1 such that the IK effectiveness over the number of steps that's equivalent
  // to the reference timestep t0, is equal to the reference effectiveness. Of course, the reference
  // effectiveness itself is only a rough guess.
  float e0 = 0.75f; // reference effectiveness
  float t0 = 1.0f/30.0f; // reference timestep
  float t1 = timeDelta; // actual timestep
  float e1 = 1.0f - powf(1.0f-e0, t0 / t1); // expected effectiveness
  float IKWeightScale = (1.0f - powf(1.0f - e0, t1/t0)) / e0;
  IKWeightScale = NMP::clampValue(IKWeightScale, 0.0f, 1.0f);
#else
  (void) timeDelta;
#endif  
  if (!m_OP.m_useGuidePose)
  {
    NMRU::GeomUtils::DebugDraw* debugLines = 0;

#if defined(MR_OUTPUT_DEBUGGING)
    if (!m_OP.m_NMIKTestSolve)
    {
      debugLines = m_debugLines;
    }
#endif // defined(MR_OUTPUT_DEBUGGING)

#ifdef SCALE_IK_EFFECTIVENESS
    for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
    {
      m_ikParams->perJointParams[j].weight *= IKWeightScale;
    }
#endif
    NMRU::HybridIK::solve(
      m_ikParams,
      m_ikGeometry,
      m_ikJointIndices,
      m_numIKJoints,
      ikTarget,
      solutionEE,
      &globalRootParentTransform,
      debugLines);
  }
  else
  {
    // The arm ik fixup fixup consists of a two-pass solve.
    // The first solve is carried out with a weight of zero on the clavicle, the second
    // weights of zero on all but the clavicle
    // The full fixup involves (in fact allows for) non-zero pre-solve comfy weight on
    // the clavicle without throwing off the final solution whilst avoiding having the clavicles
    // raising when they don't need to (eg. when reaching for stomach).
    // The effect of applying this fix up is to get:
    // 
    // 1 clav preset from guide pose  
    // 2 ik solve from shoulder to root  
    // 3 ik solve on clavicle only / "clavicle fixup"  
    //
    // LimbIK::solve() - check that clavicle fixup code and limb ui are in place and remove temporary comments and code
    // below: MORPH-11274
    // Initial runs of this algo on the test reaches have shown promising results on problem cases,
    // a little more testing if required to prove out the approach. should we decide to run with this
    // then some tweaks will be required to optimise and some limb UI may be needed to specify when this
    // approach should be used (eg. arms only)

    // Make a copy of user-specified joint weights and zero the weight on the guide pose joints
    float pjWeights[LimbIK::s_maxNumJoints];
    for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
    {
      pjWeights[j] = m_ikParams->perJointParams[j].weight;
      m_ikParams->perJointParams[j].weight = 1.0f - m_OP.m_guidePoseWeights[j];

#ifdef SCALE_IK_EFFECTIVENESS
      m_ikParams->perJointParams[j].weight *= IKWeightScale;
#endif
    }

    NMRU::GeomUtils::DebugDraw* debugLines = 0;
#if defined(MR_OUTPUT_DEBUGGING)
    if (!m_OP.m_NMIKTestSolve)
    {
      debugLines = m_debugLines;
    }
#endif // defined(MR_OUTPUT_DEBUGGING)

    // guide pose solve
    NMRU::HybridIK::solve(
      m_ikParams,
      m_ikGeometry,
      m_ikJointIndices,
      m_numIKJoints,
      ikTarget,
      solutionEE,
      &globalRootParentTransform,
      debugLines);

    // restore the clavicle weight and zero weights on remaining joints
    for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
    {
      m_ikParams->perJointParams[j].weight = m_OP.m_guidePoseWeights[j];
#ifdef SCALE_IK_EFFECTIVENESS
      m_ikParams->perJointParams[j].weight *= IKWeightScale;
#endif
    }

    // clavicle fixup solve
    NMRU::HybridIK::solve(
      m_ikParams,
      m_ikGeometry,
      m_ikJointIndices,
      m_numIKJoints,
      ikTarget,
      solutionEE,
      &globalRootParentTransform,
      debugLines);

    // restore user-specified weights for next frame
    for (uint32_t j = 1; (signed)j < m_numJoints; ++j)
    {
      m_ikParams->perJointParams[j].weight = pjWeights[j];
    }
  }

  NMRU::GeomUtils::fromPosQuatMatrix34(m_lastSolutionEndEffectorTM, solutionEE);

  // Move the results into the limit frame for output
  for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
  {
    m_finalQuats[j] = ~m_ikParams->perJointParams[j].limits.frame * m_ikGeometry[j].q;
  }
  // END OF NMIK SOLVE

  // Revisit LimbIK debug draw particularly wrt intermediate output for viewing each iteration
  // MORPH-11263
  // see what we can do about getting intermediate results out of nmik
  // quick fix compromise: "yellow line output is NMIK's version of "draw each iteration" "
#if defined(MR_OUTPUT_DEBUGGING)
  if (getDebugDrawFlag(kDrawEachIteration))
  {
    // Draw debug lines
    uint32_t numDebugLines = m_debugLines->getNumLines();
    for (uint32_t l = 0; l < numDebugLines; ++l)
    {
      NMP::Vector3 start;
      NMP::Vector3 end;
      NMP::Vector3 colour;
      m_debugLines->getColouredDebugLine(l, start, end, colour);
      start = offsetForDebug.getTransformedVector(start);
      end = offsetForDebug.getTransformedVector(end);
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, start, end, NMP::Colour(colour, 255));
    }
    m_debugLines->clearBuffer();
  }

  if (getDebugDrawFlag(kDrawFinalResult))
  {
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawIKFinalResultsControlID, m_limbIndex);
    NMP_ASSERT(m_debugDrawData.jdd);
    calcLimbDrawInfo(m_finalQuats, m_debugDrawData);
    drawLimb(pDebugDrawInst, m_debugDrawData, offsetForDebug, 
      NMP::Vector3(1.0f, 1.0f, 1.0f), false, false, dimensionalScaling);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)

  // debug error output
  if (solverErrorData)
  {
    const NMP::Vector3& ikSolutionEndEffectorPos = m_lastSolutionEndEffectorTM.translation();
    // positional error
    NMP::Vector3 posError = ikSolutionEndEffectorPos - target.translation();
    // full ori error
    NMP::Matrix34 targInv = target;
    targInv.transpose3x3();
    NMP::Matrix34 toCurrent;
    toCurrent.multiply3x3(targInv, m_lastSolutionEndEffectorTM);
    NMP::Quat qua = toCurrent.toQuat();
    float fullOriError = qua.angle();
    qua.forRotation(
      m_lastSolutionEndEffectorTM.getRotatedVector(localNormal), target.getRotatedVector(localNormal));
    // normal only orientation error
    float normalOriError = qua.angle();

    // Store the errors
    solverErrorData->m_positionError = posError.magnitude();
    solverErrorData->m_normalError = normalOriError;
    solverErrorData->m_orientationError = fullOriError;
    solverErrorData->m_positionErrorWeight = m_OP.m_positionWeight;
    solverErrorData->m_normalErrorWeight = m_OP.m_normalWeight;
    solverErrorData->m_orientationErrorWeight = m_OP.m_orientationWeight;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool LimbIK::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*this);
  savedState.addRawData(m_bodyMatrix, (m_numJoints + 1) * sizeof(*m_bodyMatrix));
  savedState.addRawData(m_finalQuats, (m_numJoints) * sizeof(*m_finalQuats));
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LimbIK::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *this = savedState.getValue<LimbIK>();
  savedState.getRawData(m_bodyMatrix);
  savedState.getRawData(m_finalQuats);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::updateIKTargetWeights()
{
  m_ikParams->targetPositionWeight = m_OP.m_positionWeight;
  m_ikParams->targetOrientationWeight = m_OP.m_orientationWeight;
  m_ikParams->endEffectorPoleVectorParams.weight = m_OP.m_normalWeight;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setPriorityOrientationConstraint()
{
  // By default orientation is a secondary task, so even if the orientationWeight is 1 and positionWeight is 0.01
  // orientation task won't compete with the position task. Here we allow the orientation task to compete with the
  // position task if the orientationWeight is greater than than the positionWeight.

  const float orientationOrNormalWeight =
    NMP::maximum(m_ikParams->targetOrientationWeight, m_ikParams->endEffectorPoleVectorParams.weight);
    
  if (m_ikParams->targetPositionWeight < orientationOrNormalWeight)
  {
    if(m_ikParams->endEffectorPoleVectorParams.weight < m_ikParams->targetOrientationWeight)
    {
      m_ikParams->constraints.set(NMRU::HybridIK::IKCONSTRAINT_PRIMARY_ORIENTATION);
      m_ikParams->constraints.clear(NMRU::HybridIK::IKCONSTRAINT_PRIMARY_POLEVECTOR);
    }
    else
    {
      m_ikParams->constraints.set(NMRU::HybridIK::IKCONSTRAINT_PRIMARY_POLEVECTOR);
      m_ikParams->constraints.clear(NMRU::HybridIK::IKCONSTRAINT_PRIMARY_ORIENTATION);
    }
  }
  else
  {
    m_ikParams->constraints.clear(NMRU::HybridIK::IKCONSTRAINT_PRIMARY_ORIENTATION);
    m_ikParams->constraints.clear(NMRU::HybridIK::IKCONSTRAINT_PRIMARY_POLEVECTOR);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LimbIK::setNMIKParams()
{
  // those that are changing at runtime, i.e. controlled by behaviour
  updateIKTargetWeights();

  // Set the global IK parameters
  m_ikParams->maxIterations = m_OP.m_maxIterations;
  m_ikParams->traverseOneWayOnly = true;
  m_ikParams->directionHintsFactor = 0.3f;
  m_ikParams->reverseDirectionOfSolve = true;
  m_ikParams->accuracy = 0.0f;
  m_ikParams->orientationAccuracy = 0.0f;
  m_ikParams->endEffectorPoleVectorParams.active = true;
  m_ikParams->endEffectorPoleVectorParams.controlPoleVector.set(1.0f, 0, 0); // Normal is always x-axis
  m_ikParams->endEffectorPoleVectorParams.referencePoleVector.set(1.0f, 0, 0);
  m_ikParams->constraints.clearThenSet(
    NMRU::HybridIK::IKCONSTRAINT_PRIMARY_POSITION |
    NMRU::HybridIK::IKCONSTRAINT_SECONDARY_ORIENTATION |
    NMRU::HybridIK::IKCONSTRAINT_SECONDARY_POLEVECTOR |
    NMRU::HybridIK::IKCONSTRAINT_SECONDARY_BINDPOSE);
  // Bindpose weight:
  // (ie. the weight assigned to the swiveled guide pose) is taken from connect-authored Limb->IKSolver setting
  // currently appearing as "Neutral Pose Weight". This is a single scalar for the whole limb (correspondingly per joint
  // bind pose weights below are set to 1).
  m_ikParams->jointBindPoseWeight = m_OP.m_neutralPoseWeight;

  // set the orientation constraint as a primary or leave it as a secondary task depending of the IKTargetWeights.
  setPriorityOrientationConstraint();

  // Set the per-joint parameters
  for (uint32_t j = 0; (signed)j < m_numJoints; ++j)
  {
    NMRU::HybridIK::PerJointParams& jointParams = m_ikParams->perJointParams[j];

    // Weights
    jointParams.weight = 1.0f;
    jointParams.positionWeight = m_OP.m_positionWeights[j];
    jointParams.orientationWeight = m_OP.m_orientationWeights[j];
    jointParams.endEffectorPoleVectorWeight = m_OP.m_orientationWeights[j];
    jointParams.redundancyControlWeight = m_OP.m_neutralPoseWeight;
    // The per-joint bindpose weights (guide pose appropriately swiveled) are set uniformly across all joints
    jointParams.bindPoseWeight = 1;

    // Always initialise the secondaryOnly parameter as off
    jointParams.secondaryOnly = false;

    // Special for end joint - don't solve for position, just orientation, unless this is a position-only solve
    if ((signed)j == m_numJoints - 1)
    {
      bool doingPosition =
        (m_ikParams->constraints & NMRU::HybridIK::IKCONSTRAINT_POSITION) && m_ikParams->targetPositionWeight > 0;
      bool doingOrientation = (m_ikParams->constraints & NMRU::HybridIK::IKCONSTRAINT_ORIENTATION) &&
        m_ikParams->targetOrientationWeight > 0;
      bool doingPoleVector = (m_ikParams->constraints & NMRU::HybridIK::IKCONSTRAINT_POLEVECTOR) &&
        m_ikParams->endEffectorPoleVectorParams.weight > 0;
      if (doingPosition && (doingOrientation || doingPoleVector))
      {
        jointParams.secondaryOnly = true;
      }
    }

    // Joint limits switching on or off
#if defined(MR_OUTPUT_DEBUGGING)
    jointParams.applyHardLimits = getFeatureFlag(kFeatureUseJointLimits) && m_OP.m_limitsEnabled;
#else
    jointParams.applyHardLimits = m_OP.m_limitsEnabled;
#endif

  } // for numJoints
}

} // NAMESPACE ER
