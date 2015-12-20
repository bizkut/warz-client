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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_BLEND_NODE_DEF_BUILDER_UTILS_H
#define MR_BLEND_NODE_DEF_BUILDER_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "morpheme/mrNetworkDef.h"

#include "export/mcExport.h"
#include "export/mcExportXml.h"

#include "AssetProcessor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Utility for determining what the appropriate event matching type is (DURATION_EVENT_MATCH_...), from
/// the data block of a ME::NodeExport.
//----------------------------------------------------------------------------------------------------------------------
uint32_t determineEventMatchingTypeFromDataBlock(const ME::DataBlockExport* nodeDefDataBlock);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Utility for determining what the appropriate event matching type is (DURATION_EVENT_MATCH_...), from
/// a set of flags.
//----------------------------------------------------------------------------------------------------------------------
uint32_t determineEventMatchingTypeFromFlags(
  bool matchPassThrough,
  bool matchInSequence,
  bool matchSameUserData,
  bool matchOnOverlap,
  bool matchWithinRange);

//----------------------------------------------------------------------------------------------------------------------
enum NodeTimeStretchModes
{
  kNodeTimeStretchNone = 0,
  kNodeTimeStretchMatchEvents,
  kNodeTimeStretchInvalid
};

enum NodeSampledEventBlendModes
{
  kMergeSampledEvents = 0,
  kAddSampledEvents,
  kSampledEventBlendModeInvalid
};

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format getBlendNodeTimeStretchModeMemoryRequirements(
  const ME::NodeExport*             nodeDefExport,
  NodeTimeStretchModes&             timeStretchMode,
  uint32_t&                         numAttribDatas);

//----------------------------------------------------------------------------------------------------------------------
void initBlendNodeTimeStretchModeAttribData(
  NMP::Memory::Resource&            memRes,         ///< Mem resource where we need to build this data.
  const ME::NodeExport*             nodeDefExport,  ///< Exported from connect.
  MR::NodeDef*                      nodeDef);       ///< To initialise.

//----------------------------------------------------------------------------------------------------------------------
enum NodePassThroughModes
{
  kNodePassThroughSource0 = 0,
  kNodePassThroughSource1,
  kNodePassThroughNone,
  kNodePassThroughInvalid
};

//----------------------------------------------------------------------------------------------------------------------
NodePassThroughModes initBlendNodePassThroughMode(
  MR::NodeDef*                      nodeDef,
  const ME::NodeExport*             nodeDefExport);


//----------------------------------------------------------------------------------------------------------------------
enum NodeBlendModes
{
  kInterpQuatInterpPos = 0,
  kInterpQuatAddPos,
  kAddQuatInterpPos,
  kAddQuatAddPos,
  kNodeBlendInvalid
};

//----------------------------------------------------------------------------------------------------------------------
NodeBlendModes getBlendNodeBlendMode(
  const ME::NodeExport*             nodeDefExport   ///< Exported from connect.
);

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataBlendFlags* initBlendNodeBlendFlags(
  NMP::Memory::Resource&            memRes,           ///< Mem resource where we need to build this data.
  const ME::NodeExport*             nodeDefExport,    ///< Exported from connect.
  MR::NodeDef*                      nodeDef           ///< To initialise.
);

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_BLEND_NODE_DEF_BUILDER_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
