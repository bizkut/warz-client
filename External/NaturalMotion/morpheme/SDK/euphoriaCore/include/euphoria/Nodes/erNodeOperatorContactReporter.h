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
#ifndef MR_NODE_OPERATOR_CONTACTREPORTER_H
#define MR_NODE_OPERATOR_CONTACTREPORTER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "euphoria/erAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorContactReporter
/// \brief Container for ContactReporter operator definition.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorContactReporterDef : public MR::AttribData
{

public:
  static AttribDataOperatorContactReporterDef* create(
    NMP::MemoryAllocator* allocator,
    float minImpulseMagnitude,
    int32_t numCollisionGroups,
    uint16_t refCount);

  static NMP::Memory::Format getMemoryRequirements(int32_t numCollisionGroups);

  static AttribDataOperatorContactReporterDef* init(
    NMP::Memory::Resource& resource,
    float minImpulseMagnitude,
    int32_t numCollisionGroups,
    uint16_t refCount);

  NM_INLINE AttribDataOperatorContactReporterDef() { setType(ATTRIB_TYPE_OPERATOR_CONTACTREPORTER_DEF); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_CONTACTREPORTER_DEF; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static bool copy(MR::AttribData* source, MR::AttribData* dest);

  // Attributes
  float m_minImpulseMagnitude;
  int32_t m_numCollisionGroups;
  int32_t* m_collisionGroups;
};

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
MR::AttribData* nodeOperatorContactReporterOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update.
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// OutputCP and state data init.
void nodeOperatorContactReporterInitInstance(
  MR::NodeDef* node,
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeOperatorContactReporterOutCPPinIDs
{
  NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_LIMBINDEX = 0,
  NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_PARTINDEX,
  NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_RIGPARTINDEX,
  NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_POINTLOCAL,
  NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_NORMALLOCAL,
  NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_IMPULSEWORLD,

  NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_COUNT
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_CONTACTREPORTER_H
//----------------------------------------------------------------------------------------------------------------------
