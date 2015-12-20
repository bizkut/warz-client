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
// inline included by NMIKCommon.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
// IKJointParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE IKJointParams* IKJointParams::init(NMP::Memory::Resource* memDesc)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(IKJointParams));

  return static_cast<IKJointParams*>(memDesc->ptr);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void IKJointParams::setDefaults()
{
  isHinge = false;
  weight = 1.0f;
  redundancyControlWeight = 1.0f;
  isLimited = false;
  applyHardLimits = true;
  applySoftLimits = false;
  hardLimitRepulsionFactor = 0.0f;
  softLimitSoftness = 5.0f;
  limits = JointLimits::Params();
  enabled = true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format IKJointParams::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(IKJointParams), NMP_VECTOR_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void IKJointParams::setHingeParametersFromLimits(
  const NMP::Vector3& parentBone,
  const NMP::Vector3& childBone)
{
  // At the moment, assume we are only using NMSTANDARD limits - this tells us that the twist axis is
  // the x-axis
  NMP_ASSERT(limits.type == JointLimits::NMSTANDARD_JOINTLIMITS);

  // Hinge axis is the x-axis or negative x-axis in the limit frame
  hingeAxis.set(1.0f, 0, 0);

  // Find the orientation when the two bones are parallel in the plane perpendicular
  // to the twist axis.  Do calculations in the limit frame.
  NMP::Vector3 v1 = limits.frame.inverseRotateVector(parentBone);
  NMP::Vector3 v2 = limits.offset.inverseRotateVector(childBone);
  v1.normalise();
  v2.normalise();
  NMP::Quat straightQ = GeomUtils::forRotationAroundAxis(v2, v1, hingeAxis);

  // Find the orientation when the joint is on its minimum and maximum twist limit
  NMP::Quat lowerTwistLimitQ;
  NMP::Quat upperTwistLimitQ;
  GeomUtils::twistSwingToQuat(NMP::Vector3(limits.lower.x, 0, 0), lowerTwistLimitQ);
  GeomUtils::twistSwingToQuat(NMP::Vector3(limits.upper.x, 0, 0), upperTwistLimitQ);

  // Find whichever of the two is closest to the straight orientation.  This is then our
  // 'furthest bent backwards' limit.  If it turns out it's the lower limit, the hinge
  // axis needs to be reversed.
  float dotQLower = NMP::nmfabs(lowerTwistLimitQ.dot(straightQ));
  float dotQUpper = NMP::nmfabs(upperTwistLimitQ.dot(straightQ));
  // The cosine of an angle is larger the smaller the angle
  if (dotQLower > dotQUpper)
  {
    hingeAxis = -hingeAxis;
  }

  // Rotate hinge axis back into joint frame.
  hingeAxis = limits.frame.rotateVector(hingeAxis);

  hingeAxis.normalise();
}

//----------------------------------------------------------------------------------------------------------------------
// IKJointParams serialisation

NM_INLINE void IKJointParams::endianSwap(IKJointParams* params)
{
  NMP::endianSwap(params->hingeAxis);
  NMP::endianSwap(params->isHinge);
  NMP::endianSwap(params->weight);
  NMP::endianSwap(params->redundancyControlWeight);
  NMP::endianSwap(params->isLimited);
  NMP::endianSwap(params->applyHardLimits);
  NMP::endianSwap(params->applySoftLimits);
  NMP::endianSwap(params->hardLimitRepulsionFactor);
  NMP::endianSwap(params->softLimitSoftness);
  params->limits.endianSwap();
  NMP::endianSwap(params->enabled);
}

//----------------------------------------------------------------------------------------------------------------------
// PoleVectorControlParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PoleVectorControlParams::PoleVectorControlParams()
{
  init();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PoleVectorControlParams* PoleVectorControlParams::init(NMP::Memory::Resource* memDesc)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(PoleVectorControlParams));

  return static_cast<PoleVectorControlParams*>(memDesc->ptr);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PoleVectorControlParams::init()
{
  active = false;
  weight = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format PoleVectorControlParams::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(PoleVectorControlParams), NMP_VECTOR_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_UNIT_TESTING
  #include <cstring>
NM_INLINE bool PoleVectorControlParams::saveCode(char prefixString[], std::ostream& stream)
{
  const int DUMPSIZE = 1024;
  int n;
  char dump[DUMPSIZE];
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sactive = %s;\n", prefixString, active ? "true" : "false");
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  if (active)
  {
    n = NMP_SPRINTF(dump, DUMPSIZE, "%scontrolPoleVector.set(%ff, %ff, %ff);\n", prefixString, controlPoleVector.x, controlPoleVector.y, controlPoleVector.z);
    if (n < 0 || n >= DUMPSIZE) return false;
    stream << dump;
    n = NMP_SPRINTF(dump, DUMPSIZE, "%sreferencePoleVector.set(%ff, %ff, %ff);\n", prefixString, referencePoleVector.x, referencePoleVector.y, referencePoleVector.z);
    if (n < 0 || n >= DUMPSIZE) return false;
    stream << dump;
    n = NMP_SPRINTF(dump, DUMPSIZE, "%sweight = %ff;\n", prefixString, weight);
    if (n < 0 || n >= DUMPSIZE) return false;
    stream << dump;
  }
  return true;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PoleVectorControlParams::endianSwap(PoleVectorControlParams* params)
{
  NMP::endianSwap(params->controlPoleVector);
  NMP::endianSwap(params->referencePoleVector);
  NMP::endianSwap(params->active);
  NMP::endianSwap(params->weight);
}

//----------------------------------------------------------------------------------------------------------------------
// TertiaryConstraints construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE TertiaryConstraints* TertiaryConstraints::init(
  NMP::Memory::Resource* memDesc,
  uint32_t numPoleVectorControllers,
  uint32_t numRotations /* = 0 */)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(TertiaryConstraints));

  // Initialise shell
  TertiaryConstraints* result = static_cast<TertiaryConstraints*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(TertiaryConstraints), NMP_NATURAL_TYPE_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Initialise values
  result->maxNumPoleVectorControllers = numPoleVectorControllers;
  result->maxNumRotations = numRotations;

  // Initialise pole vector controllers
  NMP::Memory::Format poleVectorsFormat = PoleVectorControlParams::getMemoryRequirements();
  //  Need to call init on all params objects
  NMP::Memory::Resource onePoleVectorsResource;
  onePoleVectorsResource.format = poleVectorsFormat;
  for (uint32_t i = 0; i < numPoleVectorControllers; ++i)
  {
    internalMemTrack.align(poleVectorsFormat);
    if (i == 0)
    {
      result->poleVectorControllers = static_cast<PoleVectorControlParams*>(internalMemTrack.ptr);
    }
    onePoleVectorsResource.ptr = internalMemTrack.ptr;
    PoleVectorControlParams::init(&onePoleVectorsResource);
    internalMemTrack.increment(poleVectorsFormat);
  }

  // Initialise quat constraints
  NMP::Memory::Format quatConstraintsFormat(sizeof(NMP::Quat), NMP_VECTOR_ALIGNMENT);
  quatConstraintsFormat *= numRotations;
  internalMemTrack.align(quatConstraintsFormat);
  result->quatConstraints = static_cast<NMP::Quat*>(internalMemTrack.ptr);
  internalMemTrack.increment(quatConstraintsFormat);

  // Initialise pole vector joints array
  NMP::Memory::Format poleVectorJointsFormat(sizeof(uint32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  poleVectorJointsFormat *= numPoleVectorControllers;
  internalMemTrack.align(poleVectorJointsFormat);
  result->poleVectorJoints = static_cast<uint32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(poleVectorJointsFormat);

  // Initialise quat constraint joints array
  NMP::Memory::Format quatJointsFormat(sizeof(uint32_t), NMP_NATURAL_TYPE_ALIGNMENT);
  quatJointsFormat *= numRotations;
  internalMemTrack.align(quatJointsFormat);
  result->quatJoints = static_cast<uint32_t*>(internalMemTrack.ptr);
  internalMemTrack.increment(quatJointsFormat);

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TertiaryConstraints::init()
{
  numPoleVectorControllers = 0;
  numRotations = 0;
  for (uint32_t i = 0; i < maxNumPoleVectorControllers; ++i)
  {
    poleVectorControllers[i].init();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format TertiaryConstraints::getMemoryRequirements(
  uint32_t numPoleVectorControllers,
  uint32_t numRotations /* = 0 */)
{
  // Shell
  NMP::Memory::Format result(sizeof(TertiaryConstraints), NMP_NATURAL_TYPE_ALIGNMENT);

  // Pole vector controllers
  NMP::Memory::Format poleVectorsFormat = PoleVectorControlParams::getMemoryRequirements();
  // Bug work-around
  for (uint32_t i = 0; i < numPoleVectorControllers; ++i)
  {
    result += poleVectorsFormat;
  }

  // Quat constraints
  NMP::Memory::Format quatConstraintsFormat(sizeof(NMP::Quat), NMP_VECTOR_ALIGNMENT);
  quatConstraintsFormat *= numRotations;
  result += quatConstraintsFormat;

  // Joint index arrays
  result += (NMP::Memory::Format(sizeof(uint32_t), NMP_NATURAL_TYPE_ALIGNMENT) * numPoleVectorControllers);
  result += (NMP::Memory::Format(sizeof(uint32_t), NMP_NATURAL_TYPE_ALIGNMENT) * numRotations);

  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shared solver utilities

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE char isViolated(
  const IKJointParams& params,
  const NMP::Quat& q,
  float factor /* = xClampTighten */)
{
  if (!params.isLimited)
  {
    return 0;
  }
#ifdef SMOOTH_LIMITS
  else if (factor == 1.0f)
  {
    return JointLimits::isViolated(JointLimits::Params(params.limits, xClampTighten), q, xLimitSmoothness);
  }
  else
  {
    return JointLimits::isViolated(JointLimits::Params(params.limits, factor), q, xLimitSmoothness);
  }
#else
  else if (factor == 1.0f)
  {
    return JointLimits::isViolated(JointLimits::Params(params.limits, xClampTighten), q);
  }
  else
  {
    return JointLimits::isViolated(JointLimits::Params(params.limits, factor), q);
  }
#endif
}

} // end of namespace NMRU
