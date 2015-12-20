// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
# pragma once
#endif
#ifndef M_PHYSICS_DEBUG_INTERFACE_INL
#define M_PHYSICS_DEBUG_INTERFACE_INL
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mPhysicsDebugInterface.inl
/// \brief 
/// \ingroup PhysicsModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// ensure all these structs are the same layout for all compilers
#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRigDefPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRigDefPersistentData *PhysicsRigDefPersistentData::endianSwap(void *buffer)
{
  PhysicsRigDefPersistentData *data = (PhysicsRigDefPersistentData *) buffer;
  NMP::netEndianSwap(data->m_animSetIndex);
  NMP::netEndianSwap(data->m_numParts);
  NMP::netEndianSwap(data->m_numJoints);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRigDefPersistentData *PhysicsRigDefPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRigPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRigPersistentData *PhysicsRigPersistentData::endianSwap(void *buffer)
{
  PhysicsRigPersistentData *data = (PhysicsRigPersistentData *) buffer;
  NMP::netEndianSwap(data->m_animSetIndex);
  NMP::netEndianSwap(data->m_numParts);
  NMP::netEndianSwap(data->m_numJoints);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRigPersistentData *PhysicsRigPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRigFrameData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRigFrameData *PhysicsRigFrameData::endianSwap(void *buffer)
{
  PhysicsRigFrameData *data = (PhysicsRigFrameData *) buffer;
  NMP::netEndianSwap(data->m_animSetIndex);
  NMP::netEndianSwap(data->m_refCount);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRigFrameData *PhysicsRigFrameData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsShapePersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsShapePersistentData *PhysicsShapePersistentData::endianSwap(void *buffer)
{
  PhysicsShapePersistentData *data = (PhysicsShapePersistentData *)buffer;
  NMP::netEndianSwap(data->m_parentIndex);
  NMP::netEndianSwap(data->m_localPose);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsShapePersistentData *PhysicsShapePersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsCapsulePersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsCapsulePersistentData *PhysicsCapsulePersistentData::endianSwap(void *buffer)
{
  PhysicsShapePersistentData *baseData = PhysicsShapePersistentData::endianSwap(buffer);
  PhysicsCapsulePersistentData *data = (PhysicsCapsulePersistentData *)baseData;
  NMP::netEndianSwap(data->m_radius);
  NMP::netEndianSwap(data->m_height);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsCapsulePersistentData *PhysicsCapsulePersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsBoxPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsBoxPersistentData *PhysicsBoxPersistentData::endianSwap(void *buffer)
{
  PhysicsShapePersistentData *baseData = PhysicsShapePersistentData::endianSwap(buffer);
  PhysicsBoxPersistentData *data = (PhysicsBoxPersistentData *)baseData;
  NMP::netEndianSwap(data->m_width);
  NMP::netEndianSwap(data->m_height);
  NMP::netEndianSwap(data->m_depth);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsBoxPersistentData *PhysicsBoxPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsSpherePersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSpherePersistentData *PhysicsSpherePersistentData::endianSwap(void *buffer)
{
  PhysicsShapePersistentData *baseData = PhysicsShapePersistentData::endianSwap(buffer);
  PhysicsSpherePersistentData *data = (PhysicsSpherePersistentData *)baseData;
  NMP::netEndianSwap(data->m_radius);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSpherePersistentData *PhysicsSpherePersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPartStaticOutputData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsPartPersistentData *PhysicsPartPersistentData::endianSwap(void *buffer)
{
  PhysicsPartPersistentData *outputData = (PhysicsPartPersistentData *) buffer;
  NMP::netEndianSwap(outputData->m_nameToken);
  NMP::netEndianSwap(outputData->m_numBoxes);
  NMP::netEndianSwap(outputData->m_numCapsules);
  NMP::netEndianSwap(outputData->m_numSpheres);
  NMP::netEndianSwap(outputData->m_parentIndex);
  NMP::netEndianSwap(outputData->m_physicsObjectID);

  for (uint32_t i = 0; i != outputData->m_numBoxes; ++i)
  {
    PhysicsBoxPersistentData* boxOutputData = outputData->getBox(i);
    PhysicsBoxPersistentData::endianSwap(boxOutputData);
  }

  for (uint32_t i = 0; i != outputData->m_numCapsules; ++i)
  {
    PhysicsCapsulePersistentData* capsuleOutputData = outputData->getCapsule(i);
    PhysicsCapsulePersistentData::endianSwap(capsuleOutputData);
  }

  for (uint32_t i = 0; i != outputData->m_numSpheres; ++i)
  {
    PhysicsSpherePersistentData* sphereOutputData = outputData->getSphere(i);
    PhysicsSpherePersistentData::endianSwap(sphereOutputData);
  }

  return outputData;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsPartPersistentData *PhysicsPartPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsBoxPersistentData *PhysicsPartPersistentData::getBox(uint32_t index) const
{
  NMP_ASSERT(index < m_numBoxes);
  PhysicsBoxPersistentData *firstBox = (PhysicsBoxPersistentData *)(this + 1);
  PhysicsBoxPersistentData *box = firstBox + index;
  return box;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsCapsulePersistentData *PhysicsPartPersistentData::getCapsule(uint32_t index) const
{
  NMP_ASSERT(index < m_numCapsules);
  PhysicsBoxPersistentData *firstBox = (PhysicsBoxPersistentData *)(this + 1);
  PhysicsCapsulePersistentData *firstCapsule = (PhysicsCapsulePersistentData *)(firstBox + m_numBoxes);
  PhysicsCapsulePersistentData *capsule = firstCapsule + index;
  return capsule;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSpherePersistentData *PhysicsPartPersistentData::getSphere(uint32_t index) const
{
  NMP_ASSERT(index < m_numSpheres);
  PhysicsBoxPersistentData *firstBox = (PhysicsBoxPersistentData *)(this + 1);
  PhysicsCapsulePersistentData *firstCapsule = (PhysicsCapsulePersistentData *)(firstBox + m_numBoxes);
  PhysicsSpherePersistentData *firstSphere = (PhysicsSpherePersistentData *)(firstCapsule + m_numCapsules);
  PhysicsSpherePersistentData *sphere = firstSphere + index;
  return sphere;
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPartFrameData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsPartFrameData *PhysicsPartFrameData::endianSwap(void *buffer)
{
  PhysicsPartFrameData *frameData = (PhysicsPartFrameData *)buffer;
  NMP::netEndianSwap(frameData->m_globalPose);
  return frameData;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsPartFrameData *PhysicsPartFrameData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsJointPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsJointPersistentData* PhysicsJointPersistentData::endianSwap(PhysicsJointPersistentData* persistentData)
{
  NMP::netEndianSwap(persistentData->m_parentLocalFrame);
  NMP::netEndianSwap(persistentData->m_childLocalFrame);
  NMP::netEndianSwap(persistentData->m_parentPartIndex);
  NMP::netEndianSwap(persistentData->m_childPartIndex);
  NMP::netEndianSwap(persistentData->m_jointType);
  NMP::netEndianSwap(persistentData->m_nameToken);

  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsSixDOFJointPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSixDOFJointPersistentData *PhysicsSixDOFJointPersistentData::endianSwap(void *buffer)
{
  PhysicsSixDOFJointPersistentData *persistentData = (PhysicsSixDOFJointPersistentData *)buffer;

  PhysicsJointPersistentData::endianSwap(persistentData);

  NMP::netEndianSwap(persistentData->m_swing1Limit);
  NMP::netEndianSwap(persistentData->m_swing2Limit);
  NMP::netEndianSwap(persistentData->m_twistLimitLow);
  NMP::netEndianSwap(persistentData->m_twistLimitHigh);

  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSixDOFJointPersistentData *PhysicsSixDOFJointPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRagdollJointPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRagdollJointPersistentData *PhysicsRagdollJointPersistentData::endianSwap(void *buffer)
{
  PhysicsRagdollJointPersistentData *persistentData = (PhysicsRagdollJointPersistentData *)buffer;

  PhysicsJointPersistentData::endianSwap(persistentData);

  NMP::netEndianSwap(persistentData->m_coneAngle);
  NMP::netEndianSwap(persistentData->m_planeMinAngle);
  NMP::netEndianSwap(persistentData->m_planeMaxAngle);
  NMP::netEndianSwap(persistentData->m_twistMinAngle);
  NMP::netEndianSwap(persistentData->m_twistMaxAngle);

  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRagdollJointPersistentData *PhysicsRagdollJointPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsSoftLimitPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSoftLimitPersistentData *PhysicsSoftLimitPersistentData::endianSwap(void *buffer)
{
  PhysicsSoftLimitPersistentData *persistentData = (PhysicsSoftLimitPersistentData *)buffer;
  NMP::netEndianSwap(persistentData->m_softLimitFrame1Local);
  NMP::netEndianSwap(persistentData->m_softLimitFrame2Local);
  NMP::netEndianSwap(persistentData->m_parentPartIndex);
  NMP::netEndianSwap(persistentData->m_childPartIndex);
  NMP::netEndianSwap(persistentData->m_softSwing1Enabled);
  NMP::netEndianSwap(persistentData->m_softSwing2Enabled);
  NMP::netEndianSwap(persistentData->m_softTwistEnabled);
  NMP::netEndianSwap(persistentData->m_swing1SoftLimit);
  NMP::netEndianSwap(persistentData->m_swing2SoftLimit);
  NMP::netEndianSwap(persistentData->m_twistSoftLimitLow);
  NMP::netEndianSwap(persistentData->m_twistSoftLimitHigh);
  NMP::netEndianSwap(persistentData->m_softLimitStrength);
  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsJointFrameData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsJointFrameData* PhysicsJointFrameData::endianSwap(PhysicsJointFrameData* frameData)
{
  NMP::netEndianSwap(frameData->m_jointType);

  return frameData;
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsSixDOFJointFrameData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSixDOFJointFrameData* PhysicsSixDOFJointFrameData::endianSwap(void *buffer)
{
  PhysicsSixDOFJointFrameData* const frameData = reinterpret_cast<PhysicsSixDOFJointFrameData*>(buffer);

  PhysicsJointFrameData::endianSwap(frameData);

  NMP::netEndianSwap(frameData->m_swing1Limit);
  NMP::netEndianSwap(frameData->m_swing2Limit);
  NMP::netEndianSwap(frameData->m_twistLimitLow);
  NMP::netEndianSwap(frameData->m_twistLimitHigh);

  return frameData;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSixDOFJointFrameData *PhysicsSixDOFJointFrameData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRagdollJointFrameData
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRagdollJointFrameData* PhysicsRagdollJointFrameData::endianSwap(void *buffer)
{
  PhysicsRagdollJointFrameData* const frameData = reinterpret_cast<PhysicsRagdollJointFrameData*>(buffer);

  PhysicsJointFrameData::endianSwap(frameData);

  NMP::netEndianSwap(frameData->m_coneAngle);
  NMP::netEndianSwap(frameData->m_planeMinAngle);
  NMP::netEndianSwap(frameData->m_planeMaxAngle);
  NMP::netEndianSwap(frameData->m_twistMinAngle);
  NMP::netEndianSwap(frameData->m_twistMaxAngle);

  return frameData;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRagdollJointFrameData* PhysicsRagdollJointFrameData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsSoftLimitPersistentData *PhysicsSoftLimitPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPartOverrideColour
//----------------------------------------------------------------------------------------------------------------------
inline PhysicsPartOverrideColour *PhysicsPartOverrideColour::endianSwap(void *buffer)
{
  PhysicsPartOverrideColour *colourOverride = (PhysicsPartOverrideColour *)buffer;
  // don't endian swap m_overrideColour as it only contains uint8_ts.
  NMP::netEndianSwap(colourOverride->m_physicsObjectID);
  NMP::netEndianSwap(colourOverride->m_overrideTypeID);
  return colourOverride;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsPartOverrideColour *PhysicsPartOverrideColour::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

// restore the default packing alignment
#pragma pack(pop)

}

#endif
