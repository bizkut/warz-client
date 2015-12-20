
// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "NMPlatform/NMBitArray.h"
#include "NMPlatform/NMProfiler.h"
#include "NMGeomUtils/NMGeomUtils.h"

#include "euphoria/erValueValidators.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erDebugControls.h"
#include "euphoria/erEuphoriaUserData.h"
#include "euphoria/erBody.h"
#include "euphoria/erRigConstraint.h"
#include "euphoria/erLimb.h"
#include "euphoria/erLimbIK.h"
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erGravityCompensation.h"
#include "euphoria/erContactFeedback.h"

#include "physics/mrPhysicsScene.h"

#include "physics/mrPhysicsSerialisationBuffer.h"

namespace ER
{

EuphoriaRigPartUserData::RigPartToDataMap* EuphoriaRigPartUserData::s_rigPartToDataMap = 0;
NMP::HeapAllocator* EuphoriaRigPartUserData::s_rpMapAllocator = 0;

#ifdef NMP_ENABLE_ASSERTS
Body::DebugCallback* Body::m_prePhysDebugCallback = NULL;
#endif // NMP_ENABLE_ASSERTS

void Body::setUserContactHandler(UserContactHandler* handler) 
{ 
  m_userContactHandler = handler; 
  ContactFeedback::setUserContactHandler(handler);
}

//----------------------------------------------------------------------------------------------------------------------
RigConstraint* Body::createRigConstraint(
  const uint32_t partIndexA,
  const NMP::Matrix34& partJointFrameA,
  const uint32_t partIndexB,
  const NMP::Matrix34& partJointFrameB)
{
  return m_rigConstraintManager->create(partIndexA, partJointFrameA, partIndexB, partJointFrameB, 
    (MR::PhysicsRigPhysX3Articulation*)m_physicsRig);
}

//----------------------------------------------------------------------------------------------------------------------
void Body::destroyRigConstraint(const uint32_t partIndexA, const uint32_t partIndexB)
{
  m_rigConstraintManager->destroy(partIndexA, partIndexB);
}

//----------------------------------------------------------------------------------------------------------------------
LimbInterface& Body::getLimbInterface(uint32_t i)
{
  NMP_ASSERT(i < m_definition->m_numLimbs);
  return m_limbs[i];
}

//----------------------------------------------------------------------------------------------------------------------
const LimbInterface& Body::getLimbInterface(uint32_t i) const
{
  NMP_ASSERT(i < m_definition->m_numLimbs);
  return m_limbs[i];
}

//----------------------------------------------------------------------------------------------------------------------
Limb& Body::getLimb(uint32_t i)
{
  NMP_ASSERT(i < m_definition->m_numLimbs);
  return m_limbs[i];
}

//----------------------------------------------------------------------------------------------------------------------
const Limb& Body::getLimb(uint32_t i) const
{
  NMP_ASSERT(i < m_definition->m_numLimbs);
  return m_limbs[i];
}

//----------------------------------------------------------------------------------------------------------------------
Body* Body::createInstance(
  BodyDef*        bodyDef,
  MR::PhysicsRig* physicsRig,
  int             animSetIndex,
  bool            addPartsToEuphoria)
{
  Body* body = (Body*) NMPMemoryAlloc(sizeof(Body));
  NMP_ASSERT(body);
  NMP_ASSERT(physicsRig->getUserData() == 0);
  physicsRig->setUserData(body);
  body->create(bodyDef, physicsRig->getPhysicsScene());
  body->initialise(physicsRig, animSetIndex, addPartsToEuphoria);
  ContactFeedback::initialise(physicsRig->getPhysicsScene(), ((MR::PhysicsRigPhysX3*)physicsRig)->getClientID());
  return body;
}

//----------------------------------------------------------------------------------------------------------------------
void Body::destroyInstance(Body*  body)
{
  ContactFeedback::deinitialise(body->getPhysicsRig()->getPhysicsScene());
  body->destroy();
  NMP::Memory::memFree(body);
}

//----------------------------------------------------------------------------------------------------------------------
void Body::create(BodyDef* bodyDef, MR::PhysicsScene* physicsScene)
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_physicsRig = 0;
  m_userContactHandler = 0;
  m_physicsScene = physicsScene;
  m_definition = bodyDef;
  m_limbs = (Limb*) NMPMemoryAlloc(sizeof(Limb) * m_definition->m_numLimbs);
  NMP_ASSERT(m_limbs);
  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
  {
    new(&m_limbs[i]) Limb();
    m_limbs[i].create(m_definition->m_limbDefs[i], this);
  }

  m_gravComp = (GravityCompensation*) NMPMemoryAlloc(sizeof(GravityCompensation));
  NMP_ASSERT(m_gravComp);
  m_gravComp->init(m_definition->m_numLimbs, m_limbs);

  const uint32_t rigConstraintCapacity = 16;
  NMP::Memory::Resource rigConstraintMem =
    NMPMemoryAllocateFromFormat(RigConstraintManager::getMemoryRequirements(rigConstraintCapacity));
  m_rigConstraintManager = RigConstraintManager::init(rigConstraintMem, rigConstraintCapacity);

  // setup bitArrays for debug-draw and feature flags
  NMP::Memory::Resource debugFlagMem =
    NMPMemoryAllocateFromFormat(NMP::BitArray::getMemoryRequirements(kNumDebugDrawFlags));
  m_debugDrawFlags = NMP::BitArray::init(debugFlagMem, kNumDebugDrawFlags);

  NMP::Memory::Resource featureFlagMem =
    NMPMemoryAllocateFromFormat(NMP::BitArray::getMemoryRequirements(kNumFeatureFlags));
  m_featureFlags = NMP::BitArray::init(featureFlagMem, kNumFeatureFlags);
}



//----------------------------------------------------------------------------------------------------------------------
// sum mass over limbs
void Body::calculateMass()
{
  m_mass = 0.0f;
  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
    m_mass += m_limbs[i].getMass();
}

//----------------------------------------------------------------------------------------------------------------------
// scalar mass weighted avg of limb positions/orientations
void Body::updateCentreOfMass()
{
  // reset position component to zero ready for summation
  m_centreOfMass.setToZero();
  m_mass = 0.0f;

  // sum over limbs
  for (int i = 0; i < getNumLimbs(); ++i)
  {
    m_centreOfMass += m_limbs[i].getMass() * m_limbs[i].getCentreOfMass(); // mass weighted position
    m_mass += m_limbs[i].getMass();
  }
  m_centreOfMass /= m_mass;
}

//----------------------------------------------------------------------------------------------------------------------
void Body::updateContactState()
{
  m_isInContact = false;

  for (int limbIndex(0); !m_isInContact && (limbIndex < getNumLimbs()) ; ++limbIndex)
  {
    m_isInContact |= m_limbs[limbIndex].isInContact();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Body::initialise(
  MR::PhysicsRig* physicsRig,
  int             animSetIndex,
  bool            addPartsToEuphoria)
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_physicsRig = physicsRig;

  m_dimensionalScaling.setFromPhysicsRig(*physicsRig);

  for (int i = 0; i < MAX_NUM_LINKS; i++)
  {
    m_softLimitAdjustment[i] = 0.0f;
    m_softLimitStiffnessScale[i] = 1.0f;
    m_outputEnabledOnJoints[i] = true;
  }

  for (uint32_t i = 0; i < m_physicsRig->getNumParts(); i++)
  {
    ER::EuphoriaRigPartUserData::create(m_physicsRig->getPart(i));
  }

  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
  {
    m_limbs[i].initialise();
    // set up the grav comp so it has a basic knowledge of the limbs
    m_gravComp->setLimbLength(i, m_limbs[i].getNumPartsInChain());
    for (uint32_t j = 0; j < m_limbs[i].getNumPartsInChain(); ++j)
    {
      m_gravComp->setLimbBodyIndex(i, j, m_limbs[i].getPhysicsRigPartIndex(j));
    }
  }

  NMP::Vector3 gravity = physicsRig->getPhysicsScene()->getGravity();
  m_gravComp->setGravity(gravity);

  // Operating params
  for (uint32_t iLimb = 0; iLimb < m_definition->getNumLimbs(); ++iLimb)
  {
    LimbIK::OperatingParams& OPs = m_limbs[iLimb].getIK().m_OP;
    LimbDef& limbDef = *m_limbs[iLimb].getDefinition();
    OPs.m_neutralPoseWeight = limbDef.m_neutralPoseWeight;
    OPs.m_useGuidePose = false;

    uint32_t numJoints = m_limbs[iLimb].getNumJointsInChain();
    for (uint32_t iJoint = 0 ; iJoint < numJoints ; ++iJoint)
    {
      if (limbDef.m_activeGuidePoseJoints[iJoint])
      {
        OPs.m_guidePoseWeights[iJoint] = limbDef.m_guidePoseWeight;
        if (limbDef.m_guidePoseWeight > 0.0f)
          OPs.m_useGuidePose = true;

      }
      else
      {
        OPs.m_guidePoseWeights[iJoint] = 0.0f;
      }
      OPs.m_positionWeights[iJoint] = limbDef.m_positionWeights[iJoint];
      OPs.m_orientationWeights[iJoint] = limbDef.m_orientationWeights[iJoint];
    }
  }

  m_centreOfMass.setToZero();
  calculateMass();

  m_doingPoseModification = false;

#ifdef NMP_ENABLE_ASSERTS
  for (int i = 0 ; i < m_maxNumDebugJointControls ; ++i)
  {
    m_debugJointControls[i].childPart = 0;
    m_debugJointControls[i].debugEnabled = false;
    m_debugJointControls[i].ts.setToZero();
  }
#endif

  m_extraGravity.setToZero();

  // defaults for features and debug draw flags
  m_debugDrawFlags->clearAll();
  m_featureFlags->setAll();
  // can turn hamstrings off here
  setFeatureFlag(kFeatureHamstrings, true);
  m_hamstringStiffness = 0.0f;
  m_softLimitStiffness = 0.0f;
  m_animSetIndex = animSetIndex;

  // Create the per-shape data - needed for other characters to interact with our parts.
  if (addPartsToEuphoria)
  {
    for (uint32_t iPart = 0; iPart < m_physicsRig->getNumParts(); ++iPart)
    {
      MR::PhysicsRig::Part* part = m_physicsRig->getPart(iPart);
      MR::PhysicsRigPhysX3::PartPhysX3* partPhysX3 = (MR::PhysicsRigPhysX3::PartPhysX3*) part;
      physx::PxRigidBody* rigidBody = partPhysX3->getRigidBody();
      physx::PxU32 numShapes = rigidBody->getNbShapes();
      for (physx::PxU32 iShape = 0; iShape != numShapes; ++iShape)
      {
        physx::PxShape* shape = 0;
        rigidBody->getShapes(&shape, 1, iShape);
        NMP_ASSERT(MR::PhysXPerShapeData::getFromShape(shape) == 0);
        MR::PhysXPerShapeData::create(shape);
      }
    }
  }

  assertIsInitialised(this);
}

//----------------------------------------------------------------------------------------------------------------------
int Body::getRootLimbIndex() const
{
  return m_definition->m_rootLimbIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void Body::destroy()
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_gravComp->deinit();
  NMP::Memory::memFree(m_gravComp);

  for (uint32_t i = 0; i < m_physicsRig->getNumParts(); i++)
  {
    MR::PhysicsRig::Part* part = m_physicsRig->getPart(i);
    ER::EuphoriaRigPartUserData* data = ER::EuphoriaRigPartUserData::getFromPart(part);
    ER::EuphoriaRigPartUserData::destroy(data, part);

    // Destroy the per shape data
    MR::PhysicsRigPhysX3::PartPhysX3* partPhysX3 = (MR::PhysicsRigPhysX3::PartPhysX3*) part;
    physx::PxRigidBody* rigidBody = partPhysX3->getRigidBody();
    physx::PxU32 numShapes = rigidBody->getNbShapes();
    for (physx::PxU32 iShape = 0; iShape != numShapes; ++iShape)
    {
      physx::PxShape* shape = 0;
      rigidBody->getShapes(&shape, 1, iShape);
      MR::PhysXPerShapeData* shapeData = MR::PhysXPerShapeData::getFromShape(shape);
      MR::PhysXPerShapeData::destroy(shapeData, shape);
    }
  }

  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
  {
    m_limbs[i].destroy();
    (&m_limbs[i])->~Limb();
  }
  NMP::Memory::memFree(m_limbs);

  m_rigConstraintManager->clear();
  NMP::Memory::memFree(m_rigConstraintManager);
  NMP::Memory::memFree(m_featureFlags);
  NMP::Memory::memFree(m_debugDrawFlags);
}

//----------------------------------------------------------------------------------------------------------------------
/// This class simulates spring constraints that are connected over several joints (e.g. the
/// hamstring). It does so by modifying the strength, damping and target angle of each joint, in much
/// the same way as the soft limits do other than spanning multiple joints - the main difference is
/// that the hamstrings don't weaken if the limb has full strength.

class Hamstring
{
public:

  void setLimbIndex(int index, ER::Limb* limbs)
  {
    m_limbIndex = index;
    m_excess = 0.0f;
    m_numJoints = limbs[index].getNumJointsInChain();
    NMP_ASSERT(m_numJoints <= m_maxJointsInLimb);
    for (int i = 0; i < m_numJoints; i++)
    {
      m_jointIndices[i] = limbs[index].getPhysicsRigJointIndex(i);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  void setBendScale(int index, const NMP::Vector3& scale)
  {
    // We swap swing1 and swing2 here due to us having different convention to PhysX. This chould be
    // done at the point where variables s1 and s2 become the vector in limbDef. Note that both are
    // twist-lean.
    m_bendScale[index].x = scale.x;
    m_bendScale[index].y = scale.z;
    m_bendScale[index].z = scale.y;
  }

  //--------------------------------------------------------------------------------------------------------------------
  void apply(Body::PerLinkData* linkData, float hamstringStrength)
  {
    float total = 0.0f;
    float totalStrength = 0.0f;
    for (int i = 0; i < m_numJoints; i++)
    {
      int index = m_jointIndices[i];
      total += linkData[index].currentAngle.dot(m_bendScale[i]);
      totalStrength += m_bendScale[i].magnitudeSquared();
    }
    total /= 1e-10f + totalStrength;
    m_excess = total - m_bendTotal;
    if (m_excess <= 0.0f) // inside the hamstrings, so we don't need to adjust anything
      return;

    for (int i = 0; i < m_numJoints; i++)
    {
      int index = m_jointIndices[i];
      NMP::Vector3 limited = linkData[index].currentAngle - m_bendScale[i] * m_excess;
      NMP_ASSERT(linkData[index].strength + hamstringStrength >  0.0f);
      // We adjust the targetRot twist and swing for each joint, to affect the coupled limit. This
      // calculates the corrected target that provides the acceleration from two targets (the
      // original and the limited one), assuming the total strength is the sum of the normal and
      // coupled strengths.
      linkData[index].targetRot = (limited * hamstringStrength + linkData[index].targetRot * linkData[index].strength) /
                                  (linkData[index].strength + hamstringStrength);
      // we also need to increase the strength given that we've hit a limit
      linkData[index].strength += hamstringStrength;
      linkData[index].applyThisPart = true;
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  void updateJointsToCalculate(bool* doSoftLimits)
  {
    for (int i = 0; i < m_numJoints; i++)
    {
      doSoftLimits[m_jointIndices[i]] = true;
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Basically the idea here is to draw the limb like a fishing rod, so a wire goes through spokes
  // attached to each joint the limit acts like this springy wire is constraining the limb. (Wire
  // goes red when limit breached)
  void draw(
    MR::PhysicsRig* physicsRig, 
    Body::PerLinkData* NMP_UNUSED(linkData), 
    MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
    const DimensionalScaling& MR_OUTPUT_DEBUG_ARG(dimensionalScaling)
    )
  {
    NMP::Vector3 lastOffset(0, 0, 0);
    for (int i = 0; i < m_numJoints; i++)
    {
      int index = m_jointIndices[i];
      const MR::PhysicsJointDef* jointDef = physicsRig->getPhysicsRigDef()->m_joints[index];

      // get joint frame on either side
      NMP::Matrix34 frame1ToParentPart = jointDef->m_parentPartFrame;
      NMP::Matrix34 parentPartToWorld = physicsRig->getPart(jointDef->m_parentPartIndex)->getTransform();
      NMP::Matrix34 frame1World;
      frame1World.multiply(frame1ToParentPart, parentPartToWorld);
      NMP::Matrix34 frame2ToChildPart = jointDef->m_childPartFrame;
      NMP::Matrix34 childPartToWorld = physicsRig->getPart(jointDef->m_childPartIndex)->getTransform();
      NMP::Matrix34 frame2World;
      frame2World.multiply(frame2ToChildPart, childPartToWorld);

      // Calculate an offset, this isn't guaranteed to be good visual for every rig, just ones that
      // are similar in setup to the main male character
      NMP::Vector3 offsetParent = frame1World.r[0] * m_bendScale[i].y + 
        frame1World.r[1] * fabsf(m_bendScale[i].x) + frame1World.r[2] * (m_bendScale[i].x - m_bendScale[i].z);
      NMP::Vector3 offsetChild  = frame2World.r[0] * m_bendScale[i].y + 
        frame2World.r[1] * fabsf(m_bendScale[i].x) + frame2World.r[2] * (m_bendScale[i].x - m_bendScale[i].z);

      NMP::Vector3 offsetWorld(0, 0, 0);
      if (i)
      {
        offsetWorld += offsetChild;
      }
      if (i < m_numJoints - 1)
      {
        offsetWorld += offsetParent;
      }
      // length of offset is proportional to bendScale
      offsetWorld = frame1World.r[3] + NMP::vNormalise(offsetWorld) * m_bendScale[i].magnitude();

#if defined(MR_OUTPUT_DEBUGGING)
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, frame1World, dimensionalScaling.scaleDist(0.2f));
      MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, frame2World, dimensionalScaling.scaleDist(0.4f));
      MR_DEBUG_DRAW_LINE(pDebugDrawInst, frame1World.r[3], offsetWorld, NMP::Colour::WHITE);

      if (i)
      {
        if (m_excess > 0.0f)
        {
          MR_DEBUG_DRAW_LINE(pDebugDrawInst, offsetWorld, lastOffset, NMP::Colour::RED);
        }
        else
        {
          MR_DEBUG_DRAW_LINE(pDebugDrawInst, offsetWorld, lastOffset, NMP::Colour::BLUE);
        }
      }
      lastOffset = offsetWorld;
#endif // defined(MR_OUTPUT_DEBUGGING)
    }
  }

  static const int m_maxJointsInLimb = 6;
  // bend scales are sensitivities, if you make them larger, then the limit will be reached quickly if this joint bends
  NMP::Vector3 m_bendScale[m_maxJointsInLimb];
  float m_bendTotal;
  float m_excess;
  int m_numJoints;
  int m_jointIndices[m_maxJointsInLimb];
  int m_limbIndex;
};

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 Body::getTotalGravity() const
{
  return m_physicsRig->getPhysicsScene()->getGravity() + m_extraGravity;
}

//----------------------------------------------------------------------------------------------------------------------
// Determines the limb/part index associated with the given actor or returns false if the actor is
// not part of the body. Note that indices determined in this way are unique because although root
// parts of each limb are also parts of the spine limb, these parts are always identified as parts
// of the spine
//----------------------------------------------------------------------------------------------------------------------
ER::LimbTypeEnum::Type Body::getActorLimbPartIndex(
  const physx::PxActor* actor, int& limbRigIndex, int& partLimbIndex) const
{
  limbRigIndex = partLimbIndex = -1;

  // this is a harmless assert but possibly a sign that something has gone awry since
  // asking for the limb part index of no actor makes little sense
  NMP_ASSERT(actor);

  // if actor pointer is null release code should just return without searching
  if (!actor)
  {
    return ER::LimbTypeEnum::L_unknown;
  }

  // for each limb in the body
  // i iterates over limbs
  int32_t numLimbs = m_definition->m_numLimbs;
  for (int32_t i = 0; (limbRigIndex == -1) && (i < numLimbs); ++i)
  {
    const Limb& limb = getLimb(i);
    // for each part j iterates over each part in the limb
    int32_t numParts = limb.getTotalNumParts();
    // skip root part except on the root limb (i.e. the spine, see comment above)
    int32_t jBegin = (i == getRootLimbIndex() ? 0 : 1);
    for (int32_t j = jBegin; (partLimbIndex == -1) && (j < numParts); ++j)
    {
      const MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* part = 
        (const MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation*)limb.getPart(j);

      // if match found
      if (part->getArticulationLink() == actor)
      {
        // populate results
        //
        limbRigIndex = i;
        partLimbIndex = j;
        return limb.getType();
      }
    }
  }
  return ER::LimbTypeEnum::L_unknown;
}

//----------------------------------------------------------------------------------------------------------------------
ER::LimbTypeEnum::Type Body::getKinematicActorLimbPartIndex(
  const physx::PxActor* actor, int& limbRigIndex, int& partLimbIndex) const
{
  limbRigIndex = partLimbIndex = -1;

  // for each limb in the body
  // i iterates over limbs
  int32_t numLimbs = m_definition->m_numLimbs;
  for (int32_t i = 0; (limbRigIndex == -1) && (i < numLimbs); ++i)
  {
    const Limb& limb = getLimb(i);

    // for each part j iterates over each part in the limb
    int32_t numParts = limb.getTotalNumParts();
    // skip root part except on the root limb (i.e. the spine, see comment above)
    int32_t jBegin = (i == getRootLimbIndex() ? 0 : 1);
    for (int32_t j = jBegin; (partLimbIndex == -1) && (j < numParts); ++j)
    {
      const MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* part = 
        (const MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation*)limb.getPart(j);

      // if match found
      if (part->getKinematicActor() == actor)
      {
        // populate results
        //
        limbRigIndex = i;
        partLimbIndex = j;
        return limb.getType();
      }
    }
  }
  return ER::LimbTypeEnum::L_unknown;
}

// get the actor pointer from a limb part index
//----------------------------------------------------------------------------------------------------------------------
const MR::PhysicsRig::Part* Body::getPartFromLimbPartIndex(const int limbRigIndex, const int partLimbIndex) const
{
  // for each limb in the body
  // i iterates over limbs
  int32_t numLimbs = m_definition->m_numLimbs;
  NMP_ASSERT(limbRigIndex >= 0 && limbRigIndex < numLimbs);

  if (limbRigIndex >= 0 && limbRigIndex < numLimbs)
  {
    const Limb& limb = getLimb(limbRigIndex);

    int32_t numParts = limb.getTotalNumParts();
    NMP_ASSERT(partLimbIndex >= 0 && partLimbIndex < numParts);

    if (partLimbIndex >= 0 && partLimbIndex < numParts)
    {
      return limb.getPart(partLimbIndex);
    }
  }

  return (const MR::PhysicsRig::Part*)NULL;
}

//----------------------------------------------------------------------------------------------------------------------
ER::LimbTypeEnum::Type Body::getLimbTypeFromLimbPartIndex(int32_t limb, int32_t part) const
{
  // not a body part
  if (limb == -1 || part == -1)
  {
    return ER::LimbTypeEnum::L_unknown;
  }

  if (limb >= getNumLimbs())
  {
    NMP_ASSERT_MSG(limb < getNumLimbs(), "Bad limb index");
    return ER::LimbTypeEnum::L_unknown;
  }
  else if (part >= (int32_t)getLimb(limb).getTotalNumParts())
  {
    NMP_ASSERT_MSG(part < (int32_t)getLimb(limb).getTotalNumParts(), "Bad part index");
    return ER::LimbTypeEnum::L_unknown;
  }
  return getLimb(limb).getType();
}

//----------------------------------------------------------------------------------------------------------------------
void Body::prePhysicsStep(float timeDelta, MR::InstanceDebugInterface* pDebugDrawInst)
{
  NMP_ASSERT(timeDelta >= 0.0f);
  if (timeDelta == 0.0f)
  {
    return; // Nothing needs updating.
  }

#ifdef NMP_ENABLE_ASSERTS
  const int maxNumJointsInRig = 50;
  NMP_ASSERT(maxNumJointsInRig >= (int) m_physicsRig->getNumJoints());
#endif

  //////////////////////////////////////////////////////////////////////////
  // limb update: IK etc...
  ////////////////////////////////////////////////////////////////////////
  CREATE_TIMER_AND_START;
  NM_BEGIN_PROFILING("m_limbs prephys");

  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
  {
    Limb& limb = m_limbs[i];
    // If we have no output, don't process the limb (is this correct?).
    if (limb.getIsOutputEnabledOnAnyJoint())
    {
      limb.getIK().m_OP.m_limitsEnabled = getFeatureFlag(kFeatureJointLimits);
      limb.prePhysicsStep(timeDelta, pDebugDrawInst);

#if defined(MR_OUTPUT_DEBUGGING)
      // Draw joint limits and dials (before application of soft limits, which will alter target orientations)
      for (int j = 0, N = limb.getNumJointsInChain(); j < N; ++j)
      {
        const int rigJointId = limb.getPhysicsRigJointIndex(j);
        if (getDebugDrawFlag(kDrawDials) && limb.getDebugDrawFlag(Limb::kDrawDials))
        {
          drawDials(pDebugDrawInst, rigJointId, 0.125f);
        }
        if (getDebugDrawFlag(kDrawLimits) && limb.getDebugDrawFlag(Limb::kDrawLimits))
        {
          const MR::PhysicsRigPhysX3::JointPhysX3* const joint =
            static_cast< const MR::PhysicsRigPhysX3::JointPhysX3* >(m_physicsRig->getJoint(rigJointId));

          drawLimit(
            pDebugDrawInst,
            rigJointId,
            joint->getModifiableLimits().getSwing1Limit(),
            joint->getModifiableLimits().getSwing2Limit(),
            joint->getModifiableLimits().getTwistLimitLow(),
            joint->getModifiableLimits().getTwistLimitHigh(),
            0.1f,
            true,
            NMP::Vector3Zero());
        }
      }
#endif
    }
  }

  NM_END_PROFILING();
  RESET_TIMER_AND_PRINT("erBody prePhys limbs");

  //////////////////////////////////////////////////////////////////////////
  // Gravity compensation
  ////////////////////////////////////////////////////////////////////////
  NM_BEGIN_PROFILING("gravity comp");
  if (getFeatureFlag(kFeatureGravityCompensation))
  {
    m_gravComp->apply(m_physicsRig, pDebugDrawInst);
  }
  NM_END_PROFILING();
  RESET_TIMER_AND_PRINT("erBody prePhys gravityCompensation");

#ifdef NMP_ENABLE_ASSERTS
  // Allow over-riding of joint targets for debug
  if (m_prePhysDebugCallback)
  {
    m_prePhysDebugCallback(this, timeDelta);
  }
#endif

  // Soft limits and hamstrings
  NM_BEGIN_PROFILING("soft limits and hamstrings");
  if (getFeatureFlag(kFeatureSoftLimits))
  {
    applySmartJointLimits(pDebugDrawInst);
  }
  NM_END_PROFILING(); // soft limits and hamstrings
  RESET_TIMER_AND_PRINT("erBody prePhys softAndHamstrings");

  // Collision sets
  static const int m_maxCollisionGroupIndices = 32; // 32 bits available!
  int collisionGroupIndicesToActivate[m_maxCollisionGroupIndices];
  int numCollisionGroupIndices = 0;

  for (uint32_t iLimb = 0 ; iLimb < m_definition->m_numLimbs ; ++iLimb)
  {
    int collisionGroupIndexToActivate = m_limbs[iLimb].getCollisionGroupIndexToActivate();
    if (collisionGroupIndexToActivate >= 0)
    {
      collisionGroupIndicesToActivate[numCollisionGroupIndices++] = collisionGroupIndexToActivate;
    }
  }
  ((MR::PhysicsRigPhysX3Articulation*)m_physicsRig)->setCollisionGroupsToActivate(&collisionGroupIndicesToActivate[0], numCollisionGroupIndices);
  RESET_TIMER_AND_PRINT("erBody prePhys collisionGroups");

  // Reset contact report data
  NM_BEGIN_PROFILING("Reset contact reports");
  int numParts = (int)m_physicsRig->getNumParts();
  for (int i = 0; i < numParts; i++)
  {
    ER::EuphoriaRigPartUserData::getFromPart(m_physicsRig->getPart(i))->reset();
  }
  NM_END_PROFILING();

  m_rigConstraintManager->update();

  STOP_TIMER_AND_PRINT("erBody prePhys resetContactReports");
}

//----------------------------------------------------------------------------------------------------------------------
// Loops over all joints and applies soft limits and hamstrings to rig by modifying the desired joint orientations
//----------------------------------------------------------------------------------------------------------------------
void Body::applySmartJointLimits(MR::InstanceDebugInterface* pDebugDrawInst)
{
  MR::PhysicsRigDef* physicsRigDef = m_definition->getPhysicsRigDef();
  bool hamstringEnabled[MAX_NUM_LINKS] = { false };

  uint32_t numLimbs = getNumLimbs();
  if (getFeatureFlag(kFeatureHamstrings))
  {
    for (uint32_t i = 0; i < numLimbs; ++i)
    {
      LimbDef* limbDef =  m_limbs[i].getDefinition();
      if (limbDef->m_hamstring.m_enabled)
      {
        Hamstring curLimit;
        curLimit.setLimbIndex(i, m_limbs);
        curLimit.updateJointsToCalculate(hamstringEnabled);
      }
    }
  }

  // Go through various early-out scenarios and only enable this flag if none are reached
  const int numJoints = m_physicsRig->getNumJoints();
  PerLinkData* linkData = (PerLinkData*)alloca(numJoints * sizeof(PerLinkData));
  bool doSoftLimits[MAX_NUM_LINKS] = { false };
  const float adjustmentCutoff = 0.99f;
  for (int i = 0; i < numJoints; i++)
  {
    const MR::PhysicsSixDOFJointDef* jointDef = static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[i]);
    const MR::PhysicsSixDOFJointDef::SoftLimit* softJointLimitDef = jointDef->m_softLimit;

    if (softJointLimitDef)
    {
      // If limb stiffness < network's normalStiffness and softLimit enabled
      if ((softJointLimitDef->m_swing1Motion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED ||
           softJointLimitDef->m_swing2Motion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED ||
           softJointLimitDef->m_twistMotion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED) &&
          m_softLimitAdjustment[i] < adjustmentCutoff)
      {
        linkData[i].softLimitStrength = softJointLimitDef->m_strengthScale *
          NMP::sqr(m_softLimitStiffness * m_softLimitStiffnessScale[i]);
        if (linkData[i].softLimitStrength > 0.0f)
        {
          doSoftLimits[i] = true;
        }
      }
    }
  }

  for (int i = 0; i < numJoints; i++)
  {
    const MR::PhysicsSixDOFJointDef* jointDef = static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[i]);
    const MR::PhysicsSixDOFJointDef::SoftLimit* softJointLimitDef = jointDef->m_softLimit;

    if (softJointLimitDef)
    {
      linkData[i].softLimit = NMP::Vector3(
        fabs(softJointLimitDef->m_swing1Limit),
        fabs(softJointLimitDef->m_swing2Limit),
        softJointLimitDef->m_twistLimitLow,
        softJointLimitDef->m_twistLimitHigh);
    }
    else
    {
      linkData[i].softLimit.setToZero();
    }

    if (doSoftLimits[i] || hamstringEnabled[i])
    {
      calculateSoftLimitsData(i, linkData[i]);
    }
  }

  if (getFeatureFlag(kFeatureHamstrings))
  {
    for (uint32_t i = 0; i < numLimbs; ++i)
    {
      LimbDef* limbDef =  m_limbs[i].getDefinition();
      uint32_t numJointsInHamstring = m_limbs[i].getNumJointsInChain();
      if (limbDef->m_hamstring.m_enabled)
      {
        NMP::Vector3* bendScales = limbDef->m_hamstring.m_bendScaleWeights;
        Hamstring curLimit;
        curLimit.setLimbIndex(i, m_limbs);
        for (uint32_t jointIdx = 0; jointIdx < numJointsInHamstring; ++jointIdx)
        {
          curLimit.setBendScale(jointIdx, bendScales[jointIdx]);
        }
        curLimit.m_bendTotal = limbDef->m_hamstring.m_thresholdAngle;
        curLimit.apply(linkData, NMP::sqr(limbDef->m_hamstring.m_stiffness));
#if defined(MR_OUTPUT_DEBUGGING)
        if (m_limbs[i].getDebugDrawFlag(ER::LimbInterface::kDrawHamstrings))
        {
          MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawHamstringsControlID, i);
          curLimit.draw(m_physicsRig, linkData, pDebugDrawInst, m_dimensionalScaling);
        }
#endif
      }
    }
  }

  // Finally modify the joints' target orientations
  for (int i = 0; i < numJoints; i++)
  {
    const MR::PhysicsSixDOFJointDef* joint = static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[i]);
    const MR::PhysicsSixDOFJointDef::SoftLimit* softJointLimit = joint->m_softLimit;

    if (doSoftLimits[i] || hamstringEnabled[i])
    {
      if (softJointLimit && doSoftLimits[i])
      {
        bool applyToSwing =
          softJointLimit->m_swing1Motion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED ||
          softJointLimit->m_swing2Motion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED;
        bool applyToTwist = softJointLimit->m_twistMotion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED;
        calculateSoftLimits(pDebugDrawInst, linkData[i], applyToSwing, applyToTwist, i);
      }

      // Only do conversion back when soft limits or hamstrings were actually applied
      if (linkData[i].applyThisPart)
      {
        // Apply the result
        NMP::Quat targetQuat;
        // Dividing by 4 gives a decent approximation of the limit/target, plus it matches the
        // conversion that took it from a quat
        NMRU::GeomUtils::twistSwingToQuat(linkData[i].targetRot / 4.0f, targetQuat);
        m_physicsRig->getJoint(i)->setTargetOrientation(targetQuat);
        m_physicsRig->getJoint(i)->setStrength(linkData[i].strength);
        m_physicsRig->getJoint(i)->setDamping(linkData[i].damping);
      }
    }
  }

#if defined(MR_OUTPUT_DEBUGGING)
  // Draw the results: dials and joints drawn will reflect the changed desired orientation and softLimit extents.
  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
  {
    Limb& limb = m_limbs[i];
    for (int j = 0, N = limb.getNumJointsInChain(); j < N; ++j)
    {
      const int rigJointId = limb.getPhysicsRigJointIndex(j);
      if (limb.getDebugDrawFlag(Limb::kDrawSoftLimits) && doSoftLimits[rigJointId])
      {
        MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kEnableSoftLimitsControlID, i);

        const MR::PhysicsSixDOFJointDef* jointDef = static_cast<const MR::PhysicsSixDOFJointDef*>(physicsRigDef->m_joints[i]);
        const MR::PhysicsSixDOFJointDef::SoftLimit* softJointLimitDef = jointDef->m_softLimit;

        bool applyToSwing = false;
        bool applyToTwist = false;
        if (softJointLimitDef)
        {
          applyToSwing =
            softJointLimitDef->m_swing1Motion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED ||
            softJointLimitDef->m_swing2Motion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED;
          applyToTwist = softJointLimitDef->m_twistMotion == MR::PhysicsSixDOFJointDef::MOTION_LIMITED;
        }

        drawLimit(
          pDebugDrawInst, rigJointId, 
          linkData[rigJointId].softLimit.x, 
          linkData[rigJointId].softLimit.y, 
          linkData[rigJointId].softLimit.z,
          linkData[rigJointId].softLimit.w, 0.1f, false, linkData[rigJointId].softLimitOffset,
          applyToSwing, applyToTwist);

        // draw changed dials bigger than the originals
        if (limb.getDebugDrawFlag(Limb::kDrawDials))
        {
          drawDials(pDebugDrawInst, rigJointId, 0.15f);
        }
      }
    } // all joints
  } // all limbs
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void Body::disableBehaviourEffects()
{
  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
  {
    m_limbs[i].disable();
  }

  NMP_ASSERT(m_rigConstraintManager);
  if (m_rigConstraintManager)
  {
    m_rigConstraintManager->clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Body::postPhysicsStep(float timeDelta)
{
  NMP_ASSERT(timeDelta >= 0.0f);
  if (timeDelta == 0.0f)
  {
    return; // Nothing needs updating.
  }
  EUPHORIA_LOG_ENTER_FUNC();
  CREATE_TIMER_AND_START;
  for (uint32_t i = 0; i < m_definition->m_numLimbs; ++i)
  {
    m_limbs[i].postPhysicsStep(timeDelta);
  }
  RESET_TIMER_AND_PRINT("erBody postPhys limbs");

  updateCentreOfMass();
  updateContactState();

  // Get ready for next update
  for (uint32_t iLimb = 0 ; iLimb != m_definition->m_numLimbs ; ++iLimb)
  {
    m_limbs[iLimb].setIsOutputEnabledOnAnyJoint(false);
    m_limbs[iLimb].setIsEndExternallySupported(false);
  }
  for (int i = 0; i < MAX_NUM_LINKS; i++)
  {
    m_outputEnabledOnJoints[i] = false;
  }

  // Don't need the guide  poses any more this update, so zap them so if the behaviour changes they
  // don't hang over into the next update.
  STOP_TIMER_AND_PRINT("erBody postPhys COM and GuidePose");
}

//----------------------------------------------------------------------------------------------------------------------
void Body::drawDials(
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  int MR_OUTPUT_DEBUG_ARG(index),
  float MR_OUTPUT_DEBUG_ARG(size))
{
#if defined(MR_OUTPUT_DEBUGGING)
  if (!getDebugDrawFlag(kDrawDials))
  {
    return;
  }

  const MR::PhysicsJointDef* jointDef = m_physicsRig->getPhysicsRigDef()->m_joints[index];

  // Calc the world coordinate tm's of parent and child frames (actual and desired) of the joint. We
  // use these to extract the swing and twist needle directions (see below)
  //
  // frame 1 is the "parent joint frame"
  NMP::Matrix34 frame1ToParentPart = jointDef->m_parentPartFrame;
  NMP::Matrix34 parentPartToWorld = m_physicsRig->getPart(jointDef->m_parentPartIndex)->getTransform();
  NMP::Matrix34 frame1World;
  frame1World.multiply(frame1ToParentPart, parentPartToWorld);
  //
  // mat actual is the "child joint frame"
  NMP::Matrix34 frame2ToChildPart = jointDef->m_childPartFrame;
  NMP::Matrix34 childPartToWorld = m_physicsRig->getPart(jointDef->m_childPartIndex)->getTransform();
  NMP::Matrix34 matActual;
  matActual.multiply(frame2ToChildPart, childPartToWorld);
  //
  // matTarget is the "desired child joint frame"
  NMP::Matrix34 matTarget;
  matTarget.identity();
  matTarget.fromQuat(m_physicsRig->getJoint(index)->getTargetOrientation());
  matTarget.multiply(frame1ToParentPart);
  matTarget.multiply(parentPartToWorld);

  // Calculate the needle directions
  //
  // swing directions are just the x-axes (of the child frames)
  //
  NMP::Vector3 swingActual = matActual.xAxis();
  NMP::Vector3 swingTarget = matTarget.xAxis();
  //
  // twist needles are the y-axes (of the child frames) with swing component removed
  //
  // actual
  NMP::Vector3 twistActual = matActual.yAxis();
  //
  // target
  NMP::Vector3 twistTarget = matTarget.yAxis();

  // positions
  const NMP::Vector3& posActual = matActual.translation();
  const NMP::Vector3& posTarget = matTarget.translation();

  MR_DEBUG_DRAW_LINE(pDebugDrawInst, posActual, posActual + swingActual * size, NMP::Colour::WHITE);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, posActual, posActual + twistActual * size * 0.5f, NMP::Colour::WHITE);

  // desired
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, posTarget, posTarget + swingTarget * size, NMP::Colour::ORANGE);
  MR_DEBUG_DRAW_LINE(pDebugDrawInst, posTarget, posTarget + twistTarget * size * 0.5f, NMP::Colour::ORANGE);

  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, frame1World, m_dimensionalScaling.scaleDist(0.05f));
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, matActual, m_dimensionalScaling.scaleDist(0.1f));
#endif // defined(MR_OUTPUT_DEBUGGING)
}

//----------------------------------------------------------------------------------------------------------------------
void Body::drawLimit(
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  int index,
  float swing1Limit,
  float swing2Limit,
  float twistMin,
  float twistMax,
  float size,
  bool MR_OUTPUT_DEBUG_ARG(drawFrame), const NMP::Vector3& offset,
  bool swingEnabled,
  bool twistEnabled)
{

  const MR::PhysicsJointDef* jointDef = m_physicsRig->getPhysicsRigDef()->m_joints[index];

  // joint graphics (swing cone, twist wedge) colours are computed to show how close current angles are to
  // the given limits (passed in by client)
  // output colour is a blend of green to red (green->red : centre of limit->outside limit)
  //
  // transform given limits (for comparison with twist swing swing values)
  //
  float tanTwistMin, tanTwistMax, tanSwing1Limit, tanSwing2Limit;
  tanTwistMin = tanf(twistMin / 4.0f);
  tanTwistMax = tanf(twistMax / 4.0f);
  tanSwing1Limit = tanf(swing1Limit / 4.0f);
  tanSwing2Limit = tanf(swing2Limit / 4.0f);
  //
  // get current angles in tss
  //
  NMP::Vector3 tss;
  NMP::Quat currentQuat = m_physicsRig->getJointQuat(index);
  NMRU::GeomUtils::quatToTwistSwing(currentQuat, tss);
  //
  // swing cone colour (from distance to boundary (as in IK clamping code))
  //
  float swingDistSq = NMP::sqr(tss.y / tanSwing2Limit) + NMP::sqr(tss.z / tanSwing1Limit);
  swingDistSq = NMP::clampValue(swingDistSq, 0.0f, 1.0f);
  swingDistSq *= swingDistSq; // non-linearity to emphasize limits
  NMP::Vector3 coneCol(swingDistSq, 1 - swingDistSq, 0.0f);
  //
  // twist wedge colour (from distance to boundary)
  //
  float twistDist;
  if (tss.x <= 0)
  {
    twistDist = (tss.x - tanTwistMin) / -tanTwistMin;
  }
  else
  {
    twistDist = (tanTwistMax - tss.x) / tanTwistMax;
  }
  twistDist = 1.0f - NMP::clampValue(twistDist, 0.1f, 1.0f);
  twistDist *= twistDist;
  NMP::Vector3 wedgeCol(twistDist, 1.0f - twistDist, 0.0f);

  // Limit Cone, "squished" cone
  // draw swing cone
  //
  NMP::Vector3 limits(0.0f, swing2Limit, swing1Limit);
  int numSegments = 32;
  NMP::Vector3 lastPos;
  NMP::Vector3 centre = jointDef->m_parentPartFrame.translation();
  NMP::Matrix34 frame1ToParentPart = jointDef->m_parentPartFrame;
  NMP::Matrix34 parentPartToWorld = m_physicsRig->getPart(jointDef->m_parentPartIndex)->getTransform();
  // do we draw this?
  NMP::Matrix34 frame1World;
  frame1World.multiply(frame1ToParentPart, parentPartToWorld);

  NMP::Matrix34 frame2ToChildPart = jointDef->m_childPartFrame;
  NMP::Matrix34 childPartToWorld = m_physicsRig->getPart(jointDef->m_childPartIndex)->getTransform();
  NMP::Matrix34 frame2World;
  frame2World.multiply(frame2ToChildPart, childPartToWorld);

  centre.transform(parentPartToWorld);
  if (swingEnabled)
  {
    const float twoPi = 2.0f * NM_PI;
    for (int i = 0; i < numSegments; i++)
    {
      float angle = i * twoPi / (float)(numSegments - 1);
      NMP::Vector3 coord(0.0f, limits.y * sinf(angle) + offset.y, limits.z * cosf(angle) + offset.z);
      float amount = coord.magnitude();
      coord.normalise();
      NMP::Vector3 pos;
      pos.x = cosf(amount) * size;
      pos.y = sinf(amount) * size * coord.y;
      pos.z = sinf(amount) * size * coord.z;
      pos.transform(frame1World);

#if defined(MR_OUTPUT_DEBUGGING)
      if (i)
      {
        MR_DEBUG_DRAW_LINE(pDebugDrawInst, lastPos, pos, NMP::Colour(coneCol, 255));
      }
      if (drawFrame && (i == 0 || i == numSegments / 2 || i == numSegments / 4 || i == numSegments * 3 / 4))
      {
        MR_DEBUG_DRAW_LINE(pDebugDrawInst, centre, pos, NMP::Colour(coneCol, 255));
      }
#endif // defined(MR_OUTPUT_DEBUGGING)

      lastPos = pos;
    }
  }

  if (twistEnabled)
  {
    NMP::Vector3 tll;
    NMRU::GeomUtils::quatToTwistLean(currentQuat, tll);
    // Circle segment
    // draw twist wedge
    //
    for (int i = 0; i < numSegments / 4; i++)
    {
      float angle = -offset.x + twistMin + (twistMax - twistMin) * (float)i / ((float)(numSegments / 4) - 1.0f);
      NMP_ASSERT(twistMax >= twistMin);
      angle += tll.x; // i.e. wedge will not twist, but will rotate with frame 2.
      NMP::Vector3 pos;
      pos.x = 0.0f;
      pos.y = size * 0.5f * cosf(angle);
      pos.z = size * 0.5f * sinf(angle);
      pos.transform(frame2World);

#if defined(MR_OUTPUT_DEBUGGING)
      if (i)
      {
        MR_DEBUG_DRAW_LINE(pDebugDrawInst, lastPos, pos, NMP::Colour(wedgeCol, 255));
      }
      if (i == 0 || i == (numSegments / 4) - 1)
      {
        MR_DEBUG_DRAW_LINE(pDebugDrawInst, centre, pos, NMP::Colour(wedgeCol, 255));
      }
#endif // defined(MR_OUTPUT_DEBUGGING)

      lastPos = pos;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Body::calculateSoftLimitsData(int index, Body::PerLinkData& linkData) const
{
  // Calc adjusted soft limit, this 'adjust' is what pushes the soft limits out to the maximum
  // when using full strength. It is strength/normalStrength (clamped to 0:1)
  NMP_ASSERT(m_softLimitAdjustment[index] >= 0.0f && m_softLimitAdjustment[index] <= 1.0f);
  // Square the adjustment so that the soft limits stay effective at low & moderate strengths.
  float adjust = NMP::sqr(m_softLimitAdjustment[index]);
  linkData.softLimit.x += (NM_PI - linkData.softLimit.x) * adjust;
  linkData.softLimit.y += (NM_PI - linkData.softLimit.y) * adjust;
  linkData.softLimit.z += (-NM_PI - linkData.softLimit.z) * adjust; // low twist
  linkData.softLimit.w += (NM_PI - linkData.softLimit.w) * adjust; // high twist

  // Note that the swing values returned from PhysX are the opposite of what we call them - i.e.
  // swing1 and swing 2 are swapped. For the soft limit calculations we switch to their convention
  // in the Body::calculateSoftLimits function.

  // Get joint limit ts (from current joint orientation)
  NMP::Quat currentQuat = m_physicsRig->getJointQuat(index);
  NMRU::GeomUtils::quatToTwistSwing(currentQuat, linkData.currentAngle);
  linkData.currentAngle *= 4.0f; // this gives a decent approximation of twist lean, and is faster

  // Get the soft limit as a simple offset vector
  const MR::PhysicsJointDef* jointDef = m_physicsRig->getPhysicsRigDef()->m_joints[index];

  // We currently only support soft limits on rigs which have sixDoF joints.
  NMP_ASSERT(jointDef->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
  const MR::PhysicsSixDOFJointDef* sixDOFDef = (const MR::PhysicsSixDOFJointDef*)jointDef;
  NMP::Quat softLimitParentOrientation = sixDOFDef->getSoftLimitParentLocalQuat();

  NMP::Quat rotation = ~jointDef->m_parentFrameQuat * softLimitParentOrientation;
  NMRU::GeomUtils::quatToTwistSwing(rotation, linkData.softLimitOffset);
  linkData.softLimitOffset *= 4.0f; // this gives a decent approximation of twist lean, and is faster

  // Get joint target ts (from current target orientation)
  NMP::Quat targetQuat = m_physicsRig->getJoint(index)->getTargetOrientation();
  NMRU::GeomUtils::quatToTwistSwing(targetQuat, linkData.targetRot);
  linkData.targetRot *= 4.0f; // this gives a decent approximation of twist lean, and is faster

  // Get the current joint strength
  linkData.strength = m_physicsRig->getJoint(index)->getStrength();
  linkData.damping = m_physicsRig->getJoint(index)->getDamping();
  linkData.applyThisPart = false;
}

//----------------------------------------------------------------------------------------------------------------------
void Body::calculateSoftLimits(
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst), 
  Body::PerLinkData& linkData, 
  bool applyToSwing, 
  bool applyToTwist, 
  int index) const
{
  const float minStrength = 1e-6f;
  if (linkData.strength + linkData.softLimitStrength < minStrength)
  {
    return;
  }

  NMP::Vector3 limit = linkData.currentAngle - linkData.softLimitOffset; // in twist lean { t, l, l }!
  const float minSoftLimitAngle = NMP::degreesToRadians(1.0f);
  // Switch since we use a different swing1/2 convention to PhysX
  float softSwing1 = linkData.softLimit.y + minSoftLimitAngle;
  float softSwing2 = linkData.softLimit.x + minSoftLimitAngle;
  float currentSwing1 = limit.y;
  float currentSwing2 = limit.z;
  if (applyToSwing)
  {
    NMP::Vector3 p(currentSwing1 / softSwing1, currentSwing2 / softSwing2, 0);
    float distSqr = p.magnitudeSquared();
    if (distSqr > 1.0f) // outside the soft limits
    {
      float dist = sqrtf(distSqr);
      // Approx ellipse clamping here, we want to push in along the normal of the ellipse, *not* towards the centre
      // This is accurate enough for its task
      NMP::Vector3 dir(0, currentSwing1 / (softSwing1 * softSwing1), currentSwing2 / (softSwing2 * softSwing2));
      dir.normalise();
      limit -= dir * dir.dot(limit) * (dist - 1.0f) / dist;
    }
  }

  // limit twist
  if (applyToTwist)
  {
    limit.x = NMP::clampValue(limit.x, linkData.softLimit.z, linkData.softLimit.w);
  }
  if (limit == linkData.currentAngle) // we are inside all limits (no clamping needed), so don't need to adjust strength or target
  {
    return;            // note that, we should get acceptable behaviour even without this return.
  }
  limit += linkData.softLimitOffset;
#if defined(MR_OUTPUT_DEBUGGING)
  if (getDebugDrawFlag(kDrawSoftLimits))
  {
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kEnableSoftLimitsControlID, index);
    const MR::PhysicsJointDef* jointDef = m_physicsRig->getPhysicsRigDef()->m_joints[index];
    NMP::Quat test;
    NMRU::GeomUtils::twistleanToQuat(limit, test);
    NMP::Matrix34 matTarget(test, NMP::Vector3(0, 0, 0));
    NMP::Matrix34 frame1ToParentPart = jointDef->m_parentPartFrame;
    NMP::Matrix34 parentPartToWorld = m_physicsRig->getPart(jointDef->m_parentPartIndex)->getTransform();
    matTarget.multiply(frame1ToParentPart);
    matTarget.multiply(parentPartToWorld);
    MR_DEBUG_DRAW_LINE(
      pDebugDrawInst,
      matTarget.translation(),
      matTarget.translation() + matTarget.xAxis() * 0.2f,
      NMP::Colour::WHITE);
  }
#else
  (void) index;
#endif // defined(MR_OUTPUT_DEBUGGING)

  // This calculates the new target, given that the new strength will be the sum of the original and
  // SL strengths.
  linkData.targetRot = (limit * linkData.softLimitStrength + linkData.targetRot * linkData.strength) /
    (linkData.strength + linkData.softLimitStrength);
  linkData.strength = linkData.strength + linkData.softLimitStrength;
  linkData.damping = linkData.damping + 2.0f * NMP::fastSqrt(linkData.softLimitStrength);
  linkData.applyThisPart = true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Body::startPoseModification(MR::PhysicsSerialisationBuffer& savedState)
{
  for (uint32_t i = 0 ; i < m_physicsRig->getNumJoints() ; ++i)
  {
    MR::PhysicsRig::Joint* joint = m_physicsRig->getJoint(i);
    // drive
    savedState.addValue(joint->getStrength());
    savedState.addValue(joint->getDamping());
    joint->setDamping(0);
    joint->setStrength(0);
    savedState.addValue(joint->getDriveCompensation());
    joint->setDriveCompensation(0.0f);
  }
  for (int i = 0 ; i < getNumLimbs() ; ++i)
  {
    m_limbs[i].disable();
  }
  m_doingPoseModification = true;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Body::stopPoseModification(MR::PhysicsSerialisationBuffer& savedState)
{
  for (uint32_t i = 0 ; i < m_physicsRig->getNumJoints() ; ++i)
  {
    MR::PhysicsRig::Joint* joint = m_physicsRig->getJoint(i);
    // drive
    float strength = savedState.getValue(strength);
    float damping = savedState.getValue(damping);
    joint->setStrength(strength);
    joint->setDamping(damping);
    float driveCompensation = savedState.getValue(driveCompensation);
    joint->setDriveCompensation(driveCompensation);
  }
  m_doingPoseModification = false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Body::togglePoseFreeze()
{
  if (!m_doingPoseModification)
  {
    return;
  }

  for (uint32_t i = 0 ; i < m_physicsRig->getNumJoints() ; ++i)
  {
    MR::PhysicsRig::Joint* joint = m_physicsRig->getJoint(i);
    if (joint->getStrength() == 0.0f)
    {
      float dampingValue = 10000.0f;
      float strengthValue = 1000000.0f;
      joint->setDamping(dampingValue);
      joint->setStrength(strengthValue);
      NMP::Quat q = m_physicsRig->getJointQuat(i);
      m_physicsRig->getJoint(i)->setTargetOrientation(q);
    }
    else
    {
      joint->setDamping(0);
      joint->setStrength(0);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool Body::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  for (uint32_t i = 0 ; i < m_physicsRig->getNumParts() ; ++i)
  {
    if (!m_physicsRig->getPart(i)->storeState(savedState))
    {
      return false;
    }
  }

  for (uint32_t i = 0 ; i < m_physicsRig->getNumJoints() ; ++i)
  {
    if (!m_physicsRig->getJoint(i)->storeState(savedState))
    {
      return false;
    }
  }

  for (int i = 0 ; i < getNumLimbs() ; ++i)
  {
    if (!m_limbs[i].storeState(savedState))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Body::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  for (uint32_t i = 0 ; i < m_physicsRig->getNumParts() ; ++i)
  {
    if (!m_physicsRig->getPart(i)->restoreState(savedState))
    {
      return false;
    }
  }

  for (uint32_t i = 0 ; i < m_physicsRig->getNumJoints() ; ++i)
  {
    if (!m_physicsRig->getJoint(i)->restoreState(savedState))
    {
      return false;
    }
  }

  for (int i = 0 ; i < getNumLimbs() ; ++i)
  {
    if (!m_limbs[i].restoreState(savedState))
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Body::handlePhysicsJointMask(const MR::PhysicsRig::JointChooser& jointChooser)
{
  const int nLimbs = getNumLimbs();
  for (int iLimb = 0 ; iLimb < nLimbs ; ++iLimb)
  {
    Limb& limb = m_limbs[iLimb];

    const uint32_t nParts = limb.getTotalNumParts();
    {
      int physicsRigPartIndex = limb.getPhysicsRigPartIndex(0);
      limb.setIsRootExternallySupported(m_physicsRig->getPart(physicsRigPartIndex)->isBeingKeyframed());
    }

    // don't overwrite externally support flag as it may have been set by client code (to inform euphoria about a
    // constrained limb etc).
    if (!limb.getEndIsExternallySupported())
    {
      int physicsRigPartIndex = limb.getPhysicsRigPartIndex(nParts - 1);
      limb.setIsEndExternallySupported(m_physicsRig->getPart(physicsRigPartIndex)->isBeingKeyframed());
    }

    const uint32_t nJoints = limb.getNumJointsInChain();
    for (uint32_t iJoint = 0; iJoint < nJoints; ++iJoint)
    {
      uint32_t i = limb.getPhysicsRigJointIndex(iJoint);
      if (jointChooser.useJoint(i))
      {
        limb.setIsOutputEnabledOnAnyJoint(true);
        break;
      }
    }
  }

  for (uint32_t iJoint = 0 ; iJoint < jointChooser.m_physicsRigDef->getNumJoints() ; ++iJoint)
  {
    if (jointChooser.useJoint(iJoint))
    {
      m_outputEnabledOnJoints[iJoint] = true;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Body::setPrePhysDebugCallback(Body::DebugCallback* cb)
{
#ifdef NMP_ENABLE_ASSERTS
  m_prePhysDebugCallback = cb;
#else
  (void) cb;
#endif // NMP_ENABLE_ASSERTS
  return;
}

} // namespace
