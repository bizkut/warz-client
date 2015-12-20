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
// inline included by NMPointIK.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
// PointIK::Params construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PointIK::Params::Params(uint32_t numJointsInChain)
{
  init(numJointsInChain);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PointIK::Params* PointIK::Params::init(NMP::Memory::Resource* memDesc, uint32_t numJointsInChain)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(PointIK::Params));

  PointIK::Params* result = static_cast<PointIK::Params*>(memDesc->ptr);
  NMP::Memory::Resource* internalMemTrack = memDesc;

  // Initialise shell
  NMP::Memory::Format format(sizeof(PointIK::Params), NMP_VECTOR_ALIGNMENT);
  internalMemTrack->align(format);
  internalMemTrack->increment(format);

  // Initialise per-joint params
  format = IKJointParams::getMemoryRequirements();
  NMP::Memory::Resource res = *internalMemTrack;
  res.format = format;
  // Call init on all per-joint params in case they are not pod-types
  for (uint32_t j = 0; j < numJointsInChain; ++j)
  {
    internalMemTrack->align(format);
    res.ptr = internalMemTrack->ptr;
    IKJointParams* jointParams = IKJointParams::init(&res);
    if (j == 0)
    {
      result->perJointParams = jointParams;
    }
    internalMemTrack->increment(format);
  }

  // Double-check size
  NMP_ASSERT(internalMemTrack->format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PointIK::Params::init(uint32_t numJointsInChain)
{
  pointingVector.set(1.0f, 0, 0);
  worldUpAxis.set(0.0f, 1.0f, 0.0f);
  bias = 0.0f;
  minimiseRotation = true;
  keepUpright = false;
  for (uint32_t j = 0; j < numJointsInChain; ++j)
  {
    perJointParams[j].setDefaults();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format PointIK::Params::getMemoryRequirements(uint32_t numJointsInChain)
{
  // Shell
  NMP::Memory::Format result(sizeof(PointIK::Params), NMP_VECTOR_ALIGNMENT);

  // Per-joint params
  for (uint32_t j = 0; j < numJointsInChain; ++j)
  {
    result += IKJointParams::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PointIK::Params::setLinearBiasCurve(
  uint32_t numJointsInChain,
  float curveBias,
  bool fixedEndEffector /* = true */)
{
  float totalWeight = 0;
  uint32_t N = fixedEndEffector ? numJointsInChain - 1 : numJointsInChain;
  for (uint32_t j = 0; j < N; ++j)
  {
    perJointParams[j].weight *= NMRU::PointIK::calcBiasWeight(j, N, curveBias);
    if (perJointParams[j].enabled)
    {
      totalWeight += perJointParams[j].weight;
    }
  }
  if (fixedEndEffector)
  {
    perJointParams[N].weight = 0;
  }
  if (totalWeight == 0)
  {
    return;
  }
  // Rescale to take account of fixed joints
  for (uint32_t j = 0; j < N; ++j)
  {
    perJointParams[j].weight /= totalWeight;
  }

}

//----------------------------------------------------------------------------------------------------------------------
// PointIK::Params serialisation functions

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PointIK::Params::dislocate(Params* target, uint32_t numJoints)
{
  // Dislocate pointed-to data
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    IKJointParams::endianSwap(&target->perJointParams[j]);
  }

  // Unfix pointers
  UNFIX_PTR_RELATIVE(IKJointParams, target->perJointParams, target);

  // Dislocate data
  NMP::endianSwap(target->pointingVector);
  NMP::endianSwap(target->worldUpAxis);
  NMP::endianSwap(target->bias);
  NMP::endianSwap(target->minimiseRotation);
  NMP::endianSwap(target->keepUpright);
  NMP::endianSwap(target->perJointParams);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PointIK::Params::locate(Params* target, uint32_t numJoints)
{
  // Locate data
  NMP::endianSwap(target->pointingVector);
  NMP::endianSwap(target->worldUpAxis);
  NMP::endianSwap(target->bias);
  NMP::endianSwap(target->minimiseRotation);
  NMP::endianSwap(target->keepUpright);
  NMP::endianSwap(target->perJointParams);

  // Refix pointers
  REFIX_PTR_RELATIVE(IKJointParams, target->perJointParams, target);

  // Locate pointed-to data
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    IKJointParams::endianSwap(&target->perJointParams[j]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PointIK::Params::relocate(Params* target, void* location, uint32_t numJoints)
{
  NMP::Memory::Format format = getMemoryRequirements(numJoints);
  NMP::Memory::Resource internalMemTrack;
  internalMemTrack.format = format;
  internalMemTrack.ptr = location;

  // Skip shell
  format.set(sizeof(PointIK::Params), NMP_VECTOR_ALIGNMENT);
  internalMemTrack.align(format);
  internalMemTrack.increment(format);

  // Per-joint params
  format = IKJointParams::getMemoryRequirements();
#ifdef NM_DEBUG
  // Only need to do this in debug to check we use up all the memory - otherwise we only
  // need the alignment.
  for (uint32_t j = 1; j < numJoints; ++j)
  {
    format += IKJointParams::getMemoryRequirements();
  }
#endif
  internalMemTrack.align(format);
  target->perJointParams = (IKJointParams*)internalMemTrack.ptr;
  internalMemTrack.increment(format);

  // Check we used up all the memory we think we should have
  NMP_ASSERT(internalMemTrack.format.size == 0);
}

//----------------------------------------------------------------------------------------------------------------------
// PointIK solver functions and helpers

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float PointIK::calcBiasWeight(uint32_t jointPosition, uint32_t numJointsInChain, float bias)
{
  NMP_ASSERT(numJointsInChain >= 1);
  return ((2.0f * bias * GeomUtils::safelyDivide(float(jointPosition), float(numJointsInChain - 1), 1.0f)) + 1.0f - bias) / numJointsInChain;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool PointIK::closeEnough(
  const GeomUtils::PosQuat& joint,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const NMP::Vector3& pointingVector,
  float accuracy)
{
  // Vector from end effector through target (in moving frame)
  NMP::Vector3 directionToTarget = joint.q.inverseRotateVector(localTargetFF.t) - localEndEffector.t;
  directionToTarget.normalise();

  // Pointing vector in moving frame
  NMP::Vector3 pointer = localEndEffector.q.rotateVector(pointingVector);

  return NMP::nmfabs(1.0f - pointer.dot(directionToTarget)) <= accuracy;
}

} // end of namespace NMRU
