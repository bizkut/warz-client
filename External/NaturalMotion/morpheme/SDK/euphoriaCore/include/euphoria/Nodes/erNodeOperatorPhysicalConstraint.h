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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_OPERATOR_PHYSICALCONSTRAINT_H
#define MR_NODE_OPERATOR_PHYSICALCONSTRAINT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "euphoria/erAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace physx
{
  class PxD6Joint;
}

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorPhysicalConstraint
/// \brief Container for PhysicalConstraint operator definition.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorPhysicalConstraintDef : public MR::AttribData
{

public:
  static AttribDataOperatorPhysicalConstraintDef* create(
    NMP::MemoryAllocator* allocator,
    bool                  constrainPositionX,
    bool                  constrainPositionY,
    bool                  constrainPositionZ,
    bool                  constrainOrientationTwist,
    bool                  constrainOrientationSwing1,
    bool                  constrainOrientationSwing2,
    float                 partInertiaMultiplier,
    uint16_t              refCount);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorPhysicalConstraintDef* init(
    NMP::Memory::Resource& resource,
    bool                   constrainPositionX,
    bool                   constrainPositionY,
    bool                   constrainPositionZ,
    bool                   constrainOrientationTwist,
    bool                   constrainOrientationSwing1,
    bool                   constrainOrientationSwing2,
    float                  partInertiaMultiplier,
    uint16_t               refCount);

  NM_INLINE AttribDataOperatorPhysicalConstraintDef() { setType(ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_DEF); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_DEF; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static bool copy(MR::AttribData* source, MR::AttribData* dest);

  // Attributes
  bool m_constrainPositionX;
  bool m_constrainPositionY;
  bool m_constrainPositionZ;
  bool m_constrainOrientationTwist;
  bool m_constrainOrientationSwing1;
  bool m_constrainOrientationSwing2;
  float m_partInertiaMultiplier;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorPhysicalConstraintState
/// \brief State data for the constraint operator.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorPhysicalConstraintState : public MR::AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorPhysicalConstraintState* init(
    NMP::Memory::Resource& resource,
    uint16_t refCount = 0);

  NM_INLINE AttribDataOperatorPhysicalConstraintState() { setType(ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_STATE); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_STATE; }

  static MR::AttribDataHandle create(NMP::MemoryAllocator* allocator);

  physx::PxD6Joint* m_physxJoint;

  // Cache the previous values so we know when they've changed and we need to recreate the joint
  int m_prevPartIndex;
};


//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
MR::AttribData* nodeOperatorPhysicalConstraintOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update. We have 1:
  MR::Network* net);

void nodeOperatorPhysicalConstraintOutputDeleteInstance(
  const MR::NodeDef* nodeDef,
  MR::Network*       net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_VECTOR3_TO_FLOATS_H
//----------------------------------------------------------------------------------------------------------------------
