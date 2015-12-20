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
#include "euphoria/erGravityCompensation.h"
#include "euphoria/erDebugControls.h"
#include "euphoria/erDebugDraw.h"
#include "euphoria/erLimb.h"
#include "euphoria/erBody.h"
#include "euphoria/erEuphoriaUserData.h"

//----------------------------------------------------------------------------------------------------------------------
// test defines to test force based damping in latest drop
static bool debug_drawTorques = false;
static bool debug_drawGravity = false;
//----------------------------------------------------------------------------------------------------------------------

// Enable to print detailed GC info
#define DEBUG_GCx

namespace ER
{
// If rigs get complicated enough that feet sizes differe significantly from the humanoid proportions then
// we should improve this by pulling the size out of the reference widget for the legs, or making it a
// parameter of characteristics
static float s_footHalfWidth = 0.07f;
static float s_footHalfLength = 0.12f;

bool GravityCompensation::getDebugDrawTorquesFlag() { return debug_drawTorques; }
void GravityCompensation::setDebugDrawTorquesFlag(bool b) { debug_drawTorques = b; }
bool GravityCompensation::getDebugDrawGravityFlag() { return debug_drawGravity; }
void GravityCompensation::setDebugDrawGravityFlag(bool b) { debug_drawGravity = b; }

//----------------------------------------------------------------------------------------------------------------------
void GravityCompensation::init(int numLimbs, ER::Limb* limbRef)
{
  m_numLimbs = numLimbs;
  m_limb = (LimbData*) NMPMemoryAlloc(sizeof(LimbData) * numLimbs);
  NMP_ASSERT(m_limb);
  m_gravity.setToZero();
  for (uint32_t i = 0; i < m_numLimbs; i++)
  {
    m_limb[i].endSupported = false;
    m_limb[i].rootSupported = 0.0f;
    m_limb[i].length = 0;
    m_limb[i].gravityCompensationMultiplier = 1.0f; // on by default
    m_limb[i].supportForceMultipler = 1.0f;
    m_limb[i].isLeg = false;
  }

  for (int i = 0; i < 32; i++)
  {
    m_orderedLimbIndices[i] = i;
  }

  m_limbRef = limbRef;
  m_totalMass = 0.0f;
  m_centreOfMass.setToZero();
  m_ZMPOffset.setToZero();
  m_bridgeSupportLimb = -1;
  assertIsInitialised(this);
}

//----------------------------------------------------------------------------------------------------------------------
void GravityCompensation::setLimbLength(int limb, int length)
{
  NMP_ASSERT(m_limb[limb].length == 0);
  m_limb[limb].length = length;
  m_limb[limb].bodyIndex = (int*) NMPMemoryAlloc(sizeof(int) * length);
  NMP_ASSERT(m_limb[limb].bodyIndex);
}

//----------------------------------------------------------------------------------------------------------------------
// as count goes from 0 to length, we should get the bodyIndex from root to end effector
void GravityCompensation::setLimbBodyIndex(int limb, int count, int bodyIndex)
{
  m_limb[limb].bodyIndex[count] = bodyIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void GravityCompensation::deinit()
{
  for (uint32_t i = 0; i < m_numLimbs; i++)
  {
    if (m_limb[i].length)
    {
      NMP::Memory::memFree(m_limb[i].bodyIndex);
    }
  }
  NMP::Memory::memFree(m_limb);
}

//----------------------------------------------------------------------------------------------------------------------
void GravityCompensation::apply(MR::PhysicsRig* physicsRig, MR::InstanceDebugInterface* pDebugDrawInst)
{
  float gravityMagnitudeSqr = m_gravity.magnitudeSquared();
  if (gravityMagnitudeSqr < 0.00001f)
  {
    return;
  }

#if defined(MR_OUTPUT_DEBUGGING)
  const ER::DimensionalScaling& dimensionalScaling = 
    Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();
#endif

  // Controlled use of alloca for fast stack allocation
  int numParts = (int)physicsRig->getNumParts();
  PartData* partData = (PartData*) alloca(numParts * sizeof(PartData));

  // initialise some per-part data and calculate the total mass and centreOfMass of the character here
  m_centreOfMass.setToZero();
  m_totalMass = 0.0f;
  m_totalPartMass = 0.0f;
  for (int i = 0; i < numParts; i++)
  {
    MR::PhysicsRig::Part* part = physicsRig->getPart(i);

    partData[i].subtreeMass = part->getAugmentedMass();
    partData[i].subtreePartMass = part->getMass();
    partData[i].isSupported = false;
    partData[i].hasIterated = false;
    partData[i].subtreeCOM =  part->getAugmentedCOMPosition() * partData[i].subtreeMass;
    partData[i].partTorque.setToZero();

    m_totalMass += partData[i].subtreeMass;
    m_totalPartMass += partData[i].subtreePartMass;
    m_centreOfMass += partData[i].subtreeCOM;
  }
  m_centreOfMass /= m_totalMass;

  // calculate how many end and root supports we have
  int numEndSupports = 0;
  int numRootSupports = 0;
  for (uint32_t limb = 0; limb < m_numLimbs; ++limb)
  {
    if (m_limb[limb].endSupported)
    {
      numEndSupports++;
    }
    for (int i = 0, N = m_limb[limb].length - 1; i < N; i++)
    {
      m_limbRef[limb].setForceMultiplier(i, 1.0f);
    }
    if (m_limb[limb].rootSupported)
    {
      numRootSupports++;
    }
  }
  // if he's in mid-air, then there is no sag to compensate for
  if (numRootSupports + numEndSupports == 0)
    return;

  NMP::Vector3 totalCompensatedAcc(0, 0, 0);

  // Here is multi-foot support code, it calculates a scale value for each supporting limb which represents how much of
  //  the weight of the whole character it will take. Proportion is scale/totalScale, separate for arms and legs
  NMP::Vector3 gravity(0, 0, 0);
  NMP::Vector3 centreOfFeet(0, 0, 0);
  float scale[12];
  float totalLegScale = 0.0f, totalArmScale = 0.0f;
  if (numEndSupports)
  {
    float rcpGravMagSqr = 1.0f / (1e-10f + gravityMagnitudeSqr);

    for (uint32_t j = 0; j < m_numLimbs; j++)
    {
      if (m_limb[j].endSupported)
      {
        partData[m_limb[j].bodyIndex[0]].isSupported = true; // used by spine to know when to do bridge support
        centreOfFeet += m_limbRef[j].getEndTransform().translation();
        NMP::Vector3 centreToFoot = m_limbRef[j].getEndTransform().translation() - m_centreOfMass;
        centreToFoot -= m_gravity * vDot(centreToFoot, m_gravity) * rcpGravMagSqr;
        // scale each legs strength contribution in proportion to GC & its proximity. This is only an approximation of
        // the right weighting, not very accurate on quadrupeds
        scale[j] = m_limb[j].gravityCompensationMultiplier / (1e-10f + centreToFoot.magnitude());

        if (m_limb[j].isLeg)
        {
          totalLegScale += scale[j];
        }
        else
        {
          totalArmScale += scale[j];
        }
      }
    }

    // This code skews the apparent gravity vector to point to the zero moment point (ZMP)
    // in practice this means the vector is vertical if within the support polygon, and points to the closest point
    // on the polygon if it starts outside it.
    centreOfFeet /= (float)numEndSupports;
    NMP::Vector3 toFeet = centreOfFeet - m_centreOfMass;
    NMP::Vector3 gravToFeet = m_gravity * vDot(m_gravity, toFeet) * rcpGravMagSqr;
    gravToFeet += m_ZMPOffset;
    gravity = gravToFeet * vDot(m_gravity, gravToFeet) / vDot(gravToFeet, gravToFeet);
  }

  NMP::Vector3* grav = (NMP::Vector3*) alloca(m_numLimbs * sizeof(NMP::Vector3));

  // scale the gravity vector that each supporting limb uses, to comply with multiple supports.
  // e.g. if a quadruped then each leg would use the original gravity vector scaled by 1/4
  for (uint32_t j = 0; j < m_numLimbs; j++)
  {
    if (!m_limb[j].endSupported)
      continue;
    grav[j] = gravity;
    if (m_limb[j].isLeg)
    {
      if (totalLegScale)
        grav[j] *= scale[j] / totalLegScale;
    }
    else
    {
      if (totalArmScale)
        grav[j] *= scale[j] / totalArmScale;
    }
  }

#if defined(MR_OUTPUT_DEBUGGING)
  if (debug_drawGravity)
  {
    MR_DEBUG_CONTROL_ENTER(pDebugDrawInst, ER::kDrawGravityCompensationGravityControlID);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration, m_centreOfMass, 
      totalCompensatedAcc * dimensionalScaling.scaleDist(0.1f) / dimensionalScaling.scaleAccel(1.0f), 
      NMP::Colour::WHITE);
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration, m_centreOfMass, 
      m_gravity * dimensionalScaling.scaleDist(0.1f) / dimensionalScaling.scaleAccel(1.0f), 
      NMP::Colour::RED);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)

  // for the remaining (unsupported) limbs, we iterate up the character towards the root, blending on the full gravity
  // vector in proportion to how much that limb's root is supported. in practice this means the whole character will
  // use a leaning gravity vector if he is falling over, but if the root of the spine (the pelvis) is flagged as being
  // supported (e.g. in contact or pinned) then the spine and upper body will inherit the full vertical gravity vector instead.
  for (int _limb = m_numLimbs - 1; _limb >= 0; _limb--)
  {
    uint32_t limb = m_orderedLimbIndices[_limb];
    // if end is not in support, so we need to work out the gravity thats applied to it
    if (!m_limb[limb].endSupported)
    {
      // the line below relies on the order of limbs so head is before spine root being supported means it can use
      // full gravity vector
      gravity += (m_gravity - gravity) * m_limb[limb].rootSupported;
      grav[limb] = gravity;
    }

#if defined(MR_OUTPUT_DEBUGGING)
    if (debug_drawGravity)
    {
      MR_DEBUG_CONTROL_ENTER(pDebugDrawInst, ER::kDrawGravityCompensationGravityControlID);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration, m_limbRef[limb].getCentreOfMass(), 
        grav[limb] * dimensionalScaling.scaleDist(0.1f) / dimensionalScaling.scaleAccel(1.0f), NMP::Colour::WHITE);
    }
#endif // defined(MR_OUTPUT_DEBUGGING)
  }

#ifdef DEBUG_GC
  printf("GC processing\n");
#endif

  // Process the limbs in essentially the order we were originally told about (heads -> arms ->
  // spines -> legs in MyNetwork::initLimbIndices). However, we modify this order so that we process
  // unsupported limbs first. If the character is supporting itself on arms and legs (e.g. a horse)
  // then that will result in processing the bridging (spine) limb first.
  for (uint32_t _limb = 0; _limb < m_numLimbs; _limb++)
  {
    int limb = m_orderedLimbIndices[_limb];
    if (limb != m_bridgeSupportLimb && !m_limb[limb].endSupported)
    {
      applyGravityCompensation(pDebugDrawInst, limb, physicsRig, partData, grav);
    }
  }
  if (m_bridgeSupportLimb != -1)
  {
    applyGravityCompensation(
      pDebugDrawInst, m_bridgeSupportLimb, physicsRig, partData, grav);
  }
  // For the supporting limbs (legs and arms) process the unsupported limbs first, but otherwise
  // preserve the order.
  for (uint32_t _limb = 0; _limb < m_numLimbs; _limb++)
  {
    int limb = m_orderedLimbIndices[_limb];
    if (limb != m_bridgeSupportLimb && m_limb[limb].endSupported)
    {
      applyGravityCompensation(pDebugDrawInst, limb, physicsRig, partData, grav);
    }
  }
  // do all the physics API in one go
  for (int i = 0; i < numParts; i++)
  {
    if (partData[i].partTorque.x || partData[i].partTorque.y || partData[i].partTorque.z)
    {
      physicsRig->getPart(i)->addTorque(partData[i].partTorque);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GravityCompensation::applyGravityCompensation(
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst),
  int limbIndex,
  MR::PhysicsRig* physicsRig,
  PartData* partData,
  const NMP::Vector3* grav)
{
#ifdef DEBUG_GC
  printf("GC limb %d\n", limbIndex);
#endif

  float gravityMagnitudeSqr = grav[limbIndex].magnitudeSquared();
  if (gravityMagnitudeSqr < 0.000001f)
    return;

  const ER::DimensionalScaling& dimensionalScaling = 
    Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();

  LimbData& limb = m_limb[limbIndex];
  // this is used on the supporting limbs to store the mass between joint and contact
  float supportingMass = m_limbRef[limbIndex].getMass();
  int order[20];
  bool backwards[20]; // is it iterating backwards along the chain?
  int numOrderedParts = 0;
  // this is the massively complicated code for dealing fairly generically with spines
  if (limbIndex == m_bridgeSupportLimb)
  {
#ifdef DEBUG_GC
    printf("bridge limb\n");
#endif
    int head = 0;
    bool anySupported = false;
    for (int i = limb.length - 1; i > 0; i--) // start by going tip to root, in case neck is part of spine
    {
      head = i;
      if (partData[limb.bodyIndex[i]].isSupported)
      {
        anySupported = true;
        break;
      }
      backwards[numOrderedParts] = true;
      order[numOrderedParts++] = i;
    }
    int tail = 0;
    if (anySupported)
    {
      for (int i = 0; i < head; i++) // then go root to tip in case spine has a tail part
      {
        if (partData[limb.bodyIndex[i]].isSupported)
          break;
        backwards[numOrderedParts] = false;
        order[numOrderedParts++] = i;
        tail = i + 1;
      }
      int mid = (tail + head) / 2;
      for (int i = mid; i < head; i++)
      {
        backwards[numOrderedParts] = false;
        order[numOrderedParts++] = i; // then go mid to head to bridge gap
      }
      for (int i = mid; i > tail; i--)
      {
        backwards[numOrderedParts] = true;
        order[numOrderedParts++] = i; // then go mid to tail to bridge gap
      }
    }
  }
  else if (limb.endSupported) // root to tip
  {
#ifdef DEBUG_GC
    printf("End supported - root to tip\n");
#endif
    for (int i = 0; i < limb.length - 1; i++)
    {
      backwards[numOrderedParts] = false;
      order[numOrderedParts++] = i;
    }
  }
  else // tip to root
  {
#ifdef DEBUG_GC
    printf("tip to root\n");
#endif
    for (int i = limb.length - 1; i > 0; i--)
    {
      backwards[numOrderedParts] = true;
      order[numOrderedParts++] = i;
    }
  }

  int firstPartIndex = limb.bodyIndex[order[0]];
  NMP::Vector3 firstPartCOM = partData[firstPartIndex].subtreeCOM / partData[firstPartIndex].subtreeMass;
  // we have to clamp the actual subtree mass for this limb to being above the foot/hand support.
  if (limb.endSupported)
  {
    float rcpGravMagSqr = 1.0f / gravityMagnitudeSqr;

    const NMP::Matrix34& plantedTM = m_limbRef[limbIndex].getEndTransform();
    NMP::Vector3 footToCOM = firstPartCOM - plantedTM.translation();
    NMP::Vector3 gravIntercept = -m_ZMPOffset;

    float footHalfWidth = dimensionalScaling.scaleDist(s_footHalfWidth);
    float footHalfLength = dimensionalScaling.scaleDist(s_footHalfLength);

#if defined(MR_OUTPUT_DEBUGGING)
    if (debug_drawGravity)
    {
      NMP::Vector3 leftOffset = plantedTM.zAxis() * footHalfWidth;
      NMP::Vector3 frontOffset = plantedTM.yAxis() * footHalfLength;

      MR_DEBUG_CONTROL_ENTER(pDebugDrawInst, ER::kDrawGravityCompensationGravityControlID);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, plantedTM.translation(), leftOffset, NMP::Colour::RED);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, plantedTM.translation(), frontOffset, NMP::Colour::GREEN);
    }
#endif // defined(MR_OUTPUT_DEBUGGING)

    float left = gravIntercept.dot(plantedTM.zAxis());
    float front = gravIntercept.dot(plantedTM.yAxis());
    gravIntercept = plantedTM.zAxis() * NMP::clampValue(left, -footHalfWidth, footHalfWidth) +
                    plantedTM.yAxis() * NMP::clampValue(front, -footHalfLength, footHalfLength);
    firstPartCOM = plantedTM.translation() + gravIntercept + grav[limbIndex] *
      vDot(grav[limbIndex], footToCOM) * rcpGravMagSqr;

#if defined(MR_OUTPUT_DEBUGGING)
    if (debug_drawGravity) // shift in centre of mass
    {
      MR_DEBUG_CONTROL_ENTER(pDebugDrawInst, ER::kDrawGravityCompensationGravityControlID);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, firstPartCOM, -gravIntercept, NMP::Colour::TURQUOISE);
    }
#endif // defined(MR_OUTPUT_DEBUGGING)
  }

  for (int j = 0; j < numOrderedParts; j++)
  {
    int index = order[j];
    int partIndex = limb.bodyIndex[index];
    int jointIndex = !backwards[j] ? limb.bodyIndex[index+1] - 1 : limb.bodyIndex[index] - 1;
    int32_t nextIndex = limb.bodyIndex[backwards[j] ? index - 1 : index + 1];
    MR::PhysicsRig::Part* part = physicsRig->getPart(partIndex);
    MR::PhysicsRig::Joint* joint = physicsRig->getJoint(jointIndex);
    const MR::PhysicsJointDef* jointDef = physicsRig->getPhysicsRigDef()->m_joints[jointIndex];
    NMP_ASSERT((!backwards[j] && jointDef->m_childPartIndex == nextIndex) || (backwards[j] && jointDef->m_parentPartIndex == nextIndex));

#ifdef DEBUG_GC
    printf("partIndex = %d - %s, nextIndex = %d\n", 
      partIndex, physicsRig->getPhysicsRigDef()->getPartName(partIndex), nextIndex);
#endif

    int limbJointIndex = index;
    if (backwards[j])
    {
      supportingMass = m_totalMass - partData[partIndex].subtreeMass;
      limbJointIndex--;
    }
    float outerMass = m_totalMass - supportingMass;

    NMP::Vector3 jointPos;
    // TODO we need a function here to get the position of a joint... does it exist already??
    if (!backwards[j])
      part->getTransform().transformVector(jointDef->m_parentPartFrame.translation(), jointPos);
    else
      part->getTransform().transformVector(jointDef->m_childPartFrame.translation(), jointPos);

    // this means the strength multiplier is unbounded and so the multiplier value
    // represents a fraction of that required to give expected strength at root
    m_limbRef[limbIndex].setForceMultiplier(limbJointIndex,
      1.0f + limb.supportForceMultipler * outerMass / supportingMass);
    NMP_ASSERT(partData[partIndex].subtreeMass < m_totalMass);
    supportingMass -= physicsRig->getPart(nextIndex)->getAugmentedMass();
    NMP_ASSERT(backwards[j] || supportingMass > dimensionalScaling.scaleMass(-0.01f));

    NMP::Vector3 toCOM = (j == 0 ? firstPartCOM : partData[partIndex].subtreeCOM / partData[partIndex].subtreeMass);
    toCOM -= jointPos;

#if defined(MR_OUTPUT_DEBUGGING)
    if (debug_drawTorques)
    {
      MR_DEBUG_CONTROL_ENTER(pDebugDrawInst, ER::kDrawGravityCompensationTorquesControlID);
      MR_DEBUG_DRAW_POINT(
        pDebugDrawInst,
        partData[partIndex].subtreeCOM / partData[partIndex].subtreeMass,
        partData[partIndex].subtreeMass * dimensionalScaling.scaleDist(0.001f) / dimensionalScaling.scaleMass(1.0f),
        NMP::Colour::YELLOW);
      MR_DEBUG_DRAW_LINE(
        pDebugDrawInst,
        jointPos,
        partData[partIndex].subtreeCOM / partData[partIndex].subtreeMass,
        NMP::Colour::YELLOW);
    }
#endif // defined(MR_OUTPUT_DEBUGGING)

    // this line is the main player in this module
    NMP::Vector3 torque = vCross(toCOM, grav[limbIndex] * partData[partIndex].subtreeMass);

    partData[partIndex].hasIterated = true;
    // The logic that calls this function should have made sure that we have iterated over the limbs
    // (and through each limb) in the right order so that we go from unsupported parts towards the
    // supporting parts. 
    NMP_ASSERT(!partData[nextIndex].hasIterated); // the limb order is bad, parent part has already iterated!

    // Here we assume part index is same as that of parent joint
    partData[nextIndex].subtreeMass += partData[partIndex].subtreeMass;
    partData[nextIndex].subtreePartMass += partData[partIndex].subtreePartMass;
    partData[nextIndex].subtreeCOM += 
      j == 0 
      ? firstPartCOM * partData[partIndex].subtreeMass 
      : partData[partIndex].subtreeCOM;

    // Clamp the maximum GC. The value chosen here is rather arbitrary, but large enough to not kick
    // in when the character is doing "normal" things.
    static const float maxTorqueMult = 1.0f;
    float maxAngAcceleration = joint->getStrength();
    float combinedMass = partData[partIndex].subtreePartMass * (m_totalPartMass - partData[partIndex].subtreePartMass) / m_totalPartMass;
    // Pretend each joint couples the parts a constant distance apart
    float maxTorque = combinedMass * maxAngAcceleration * 
      NMP::sqr(dimensionalScaling.scaleDist(0.2f)) * maxTorqueMult;
#if defined(MR_OUTPUT_DEBUGGING)
    bool clamped = 
#endif
      torque.clampMagnitude(maxTorque);

    // Disable GC on this joint (part pair) if Euphoria isn't supposed to output anything
    if (
      m_limbRef[limbIndex].getBody()->getOutputEnabledOnJoint(jointIndex) && 
      limb.gravityCompensationMultiplier > 0.0f
      )
    {
#if defined(MR_OUTPUT_DEBUGGING)
      if (debug_drawTorques)
      {
        MR_DEBUG_CONTROL_ENTER(pDebugDrawInst, ER::kDrawGravityCompensationTorquesControlID);

        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Torque, jointPos, 
          -torque * dimensionalScaling.scaleDist(0.1f) / dimensionalScaling.scaleTorque(1.0f), 
          clamped ? NMP::Colour::RED : NMP::Colour::PURPLE);
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Torque, jointPos, 
          -torque.getNormalised() * maxTorque * 
          dimensionalScaling.scaleDist(0.1f) / dimensionalScaling.scaleTorque(1.0f), 
          NMP::Colour::ORANGE);
      }
#endif // defined(MR_OUTPUT_DEBUGGING)

      partData[partIndex].partTorque -= torque * limb.gravityCompensationMultiplier;
      partData[nextIndex].partTorque += torque * limb.gravityCompensationMultiplier;
    }
  }
}

}
