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
#ifndef MR_NODE_OPERATOR_APPLYIMPULSE_H
#define MR_NODE_OPERATOR_APPLYIMPULSE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataOperatorApplyImpulse
/// \brief Container for ApplyImpulse operator definition.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorApplyImpulseDef : public MR::AttribData
{

public:
  static AttribDataOperatorApplyImpulseDef* create(
    NMP::MemoryAllocator* allocator,
    int                   impulseType,
    bool                  positionInWorldSpace,
    bool                  directionInWorldSpace,
    uint16_t              refCount);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorApplyImpulseDef* init(
    NMP::Memory::Resource& resource,
    int                    impulseType,
    bool                   positionInWorldSpace,
    bool                   directionInWorldSpace,
    uint16_t               refCount);

  NM_INLINE AttribDataOperatorApplyImpulseDef() { setType(ATTRIB_TYPE_OPERATOR_APPLYIMPULSE_DEF); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_APPLYIMPULSE_DEF; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static bool copy(MR::AttribData* source, MR::AttribData* dest);

  // Attributes
  int   m_impulseType; // 0 is impulse, 1 is velChange
  bool  m_multiplyByTimestep; // true if the desired push should be multiplied by dt (convert force to impulse etc)
  bool  m_positionInWorldSpace;
  bool  m_directionInWorldSpace;

};

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
MR::AttribData* nodeOperatorApplyImpulseOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update. We have 1:
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeOperatorApplyImpulseOutCPPinIDs
{
  NODE_OPERATOR_APPLYIMPULSE_OUT_CP_PINID_PERFORMANCE = 0,

  NODE_OPERATOR_APPLYIMPULSE_OUT_CP_PINID_COUNT
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_VECTOR3_TO_FLOATS_H
//----------------------------------------------------------------------------------------------------------------------
