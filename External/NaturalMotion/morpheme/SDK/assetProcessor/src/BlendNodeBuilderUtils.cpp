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
#include "assetProcessor/BlendNodeBuilderUtils.h"
#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
uint32_t determineEventMatchingTypeFromDataBlock(
  const ME::DataBlockExport* nodeDefDataBlock)
{
  bool durationEventBlendPassThrough;
  nodeDefDataBlock->readBool(durationEventBlendPassThrough, "DurationEventBlendPassThrough");
  bool durationEventBlendInSequence;
  nodeDefDataBlock->readBool(durationEventBlendInSequence, "DurationEventBlendInSequence");
  bool durationEventBlendSameUserData;
  nodeDefDataBlock->readBool(durationEventBlendSameUserData, "DurationEventBlendSameUserData");
  bool durationEventBlendOnOverlap;
  nodeDefDataBlock->readBool(durationEventBlendOnOverlap, "DurationEventBlendOnOverlap");
  bool durationEventBlendWithinRange;
  nodeDefDataBlock->readBool(durationEventBlendWithinRange, "DurationEventBlendWithinRange");

  uint32_t durationEventMatchingOp = determineEventMatchingTypeFromFlags(
                                       durationEventBlendPassThrough,
                                       durationEventBlendInSequence,
                                       durationEventBlendSameUserData,
                                       durationEventBlendOnOverlap,
                                       durationEventBlendWithinRange);

  return durationEventMatchingOp;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t determineEventMatchingTypeFromFlags(
  bool matchPassThrough,
  bool matchInSequence,
  bool matchSameUserData,
  bool matchOnOverlap,
  bool matchWithinRange)
{
  if (matchPassThrough)
    return DURATION_EVENT_MATCH_PASS_THROUGH;

  if (matchInSequence)
  {
    if (matchSameUserData)
    {
      if (matchOnOverlap)
        return DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_ON_OVERLAP;
      if (matchWithinRange)
        return DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_WITHIN_RANGE;
      return DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS;
    }
    if (matchOnOverlap)
      return DURATION_EVENT_MATCH_IN_SEQUENCE_ON_OVERLAP;
    if (matchWithinRange)
      return DURATION_EVENT_MATCH_IN_SEQUENCE_WITHIN_RANGE;
    return DURATION_EVENT_MATCH_IN_SEQUENCE;
  }

  if (matchSameUserData)
  {
    if (matchOnOverlap)
      return DURATION_EVENT_MATCH_SAME_IDS_ON_OVERLAP;
    if (matchWithinRange)
      return DURATION_EVENT_MATCH_SAME_IDS_WITHIN_RANGE;
    return DURATION_EVENT_MATCH_SAME_IDS;
  }

  if (matchOnOverlap)
    return DURATION_EVENT_MATCH_ON_OVERLAP;
  if (matchWithinRange)
    return DURATION_EVENT_MATCH_WITHIN_RANGE;

  return DURATION_EVENT_MATCH_IN_SEQUENCE;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format getBlendNodeTimeStretchModeMemoryRequirements(
  const ME::NodeExport*             nodeDefExport,
  NodeTimeStretchModes&             timeStretchMode,
  uint32_t&                         numAttribDatas)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  NMP::Memory::Format result;

  // Work out how many attrib datas we actually have.
  int32_t mode = kNodeTimeStretchInvalid;
  nodeDefDataBlock->readInt(mode, "TimeStretchMode");
  timeStretchMode = (NodeTimeStretchModes)mode;

  numAttribDatas = 0;
  switch (timeStretchMode)
  {
    case kNodeTimeStretchNone:
      break;
    case kNodeTimeStretchMatchEvents:
      {
        // Reserve space for the start sync event index.
        result += MR::AttribDataUInt::getMemoryRequirements();

        // Reserve space for the DurationEvent matching operation.
        result += MR::AttribDataUInt::getMemoryRequirements();

        // Reserve space for the loop flag.
        result += MR::AttribDataBool::getMemoryRequirements();

        numAttribDatas += 3;
      }
      break;
    case kNodeTimeStretchInvalid:
    default:
      NMP_THROW_ERROR("Invalid TimeStretchMode %d for node '%s'", timeStretchMode, nodeDefExport->getName());
      break;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void initBlendNodeTimeStretchModeAttribData(
  NMP::Memory::Resource&            memRes,         // Mem resource where we need to build this data.
  const ME::NodeExport*             nodeDefExport,  // Exported from connect.
  MR::NodeDef*                      nodeDef)        // To initialise.
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t timeStretchMode = kNodeTimeStretchInvalid;
  nodeDefDataBlock->readInt(timeStretchMode, "TimeStretchMode");
  if (timeStretchMode == kNodeTimeStretchInvalid)
  {
    NMP_THROW_ERROR("Invalid TimeStretchMode %d for node '%s'", timeStretchMode, nodeDefExport->getName());
  }

  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    //---------------------------
    // Playback start sync event index.
    int32_t startSyncEventIndexVal;
    nodeDefDataBlock->readInt(startSyncEventIndexVal, "StartEventIndex");
    MR::AttribDataUInt* startSyncEventIndex = MR::AttribDataUInt::init(memRes, startSyncEventIndexVal, MR::IS_DEF_ATTRIB_DATA);
    NodeDefBuilder::initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX,     // semantic,    
      0,                                              // animSetIndex,
      startSyncEventIndex,                            // attribData,  
      MR::AttribDataUInt::getMemoryRequirements());   // attribMemReqs

    //---------------------------
    // Duration event matching mode.
    uint32_t durationEventMatchingOp = determineEventMatchingTypeFromDataBlock(nodeDefDataBlock);
    MR::AttribDataUInt* durationEventMatchingOpAttrib = MR::AttribDataUInt::init(memRes, durationEventMatchingOp, MR::IS_DEF_ATTRIB_DATA);
    NodeDefBuilder::initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP, // semantic,    
      0,                                              // animSetIndex,
      durationEventMatchingOpAttrib,                  // attribData,  
      MR::AttribDataUInt::getMemoryRequirements());   // attribMemReqs

    //---------------------------
    // Do we allow looping on this node.
    bool loopVal = false;
    nodeDefDataBlock->readBool(loopVal, "Loop");
    MR::AttribDataBool* loopable = MR::AttribDataBool::init(memRes, loopVal, MR::IS_DEF_ATTRIB_DATA);
    NodeDefBuilder::initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_LOOP,                       // semantic,    
      0,                                              // animSetIndex,
      loopable,                                       // attribData,  
      MR::AttribDataBool::getMemoryRequirements());   // attribMemReqs
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodePassThroughModes initBlendNodePassThroughMode(
  MR::NodeDef*                      nodeDef,
  const ME::NodeExport*             nodeDefExport)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Check the PassThroughMode is valid and mark the def as a filter if it passes through anything.
  int32_t mode = kNodePassThroughInvalid;
  nodeDefDataBlock->readInt(mode, "PassThroughMode");
  NodePassThroughModes passThroughMode = (NodePassThroughModes)mode;

  switch (passThroughMode)
  {
  case kNodePassThroughSource0:
    // Flag node as a filter.
    nodeDef->setPassThroughChildIndex(0);
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);
    break;
  case kNodePassThroughSource1:
    // Flag node as a filter.
    nodeDef->setPassThroughChildIndex(1);
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);
    break;
  case kNodePassThroughNone:
    break;
  case kNodePassThroughInvalid:
  default:
    NMP_THROW_ERROR("Invalid PassThroughMode %d for node '%s'", passThroughMode, nodeDefExport->getName());
    break;
  }

  return passThroughMode;
}

//----------------------------------------------------------------------------------------------------------------------
AP::NodeBlendModes getBlendNodeBlendMode( const ME::NodeExport* nodeDefExport )
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const char* nodeName = nodeDefExport->getName();

  int32_t mode = kNodeBlendInvalid;
  nodeDefDataBlock->readInt(mode, "BlendMode");
  NodeBlendModes blendMode = (NodeBlendModes)mode;
  switch (blendMode)
  {
  case kInterpQuatInterpPos:
    break;
  case kInterpQuatAddPos:
    break;
  case kAddQuatInterpPos:
    break;
  case kAddQuatAddPos:
    break;
  case kNodeBlendInvalid:
  default:
    NMP_THROW_ERROR("Invalid BlendMode %d for Blend2 node '%s'", blendMode, nodeName);
    break;
  }

  return blendMode;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataBlendFlags* initBlendNodeBlendFlags(
  NMP::Memory::Resource&            memRes,           // Mem resource where we need to build this data.
  const ME::NodeExport*             nodeDefExport,    // Exported from connect.
  MR::NodeDef*                      nodeDef)          // To initialise.
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Add the blend flags attribute
  MR::AttribDataBlendFlags* blendFlags = MR::AttribDataBlendFlags::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  NodeDefBuilder::initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_BLEND_FLAGS,                      // semantic,    
    0,                                                    // animSetIndex,
    blendFlags,                                           // attribData,  
    MR::AttribDataBlendFlags::getMemoryRequirements());   // attribMemReqs

  //---------------------------
  // Set the data
  NMP_USED_FOR_ASSERTS( bool readBoolSuceeded = )
  nodeDefDataBlock->readBool(blendFlags->m_alwaysBlendTrajectoryAndTransforms, "AlwaysBlendTrajectoryAndTransforms");
  NMP_ASSERT( readBoolSuceeded );

  NMP_USED_FOR_ASSERTS( readBoolSuceeded = )
  nodeDefDataBlock->readBool(blendFlags->m_alwaysCombineSampledEvents, "AlwaysCombineSampledEvents");
  NMP_ASSERT( readBoolSuceeded );

  return blendFlags;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
