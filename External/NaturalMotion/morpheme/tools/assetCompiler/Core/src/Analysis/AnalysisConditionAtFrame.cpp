// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/AnalysisConditionAtFrame.h"
#include "Analysis/AnalysisInterval.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisConditionAtFrameBuilder::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AnalysisConditionAtFrame), NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisCondition* AnalysisConditionAtFrameBuilder::init(
  NMP::Memory::Resource&      memRes,
  AnalysisConditionAtFrame::AtFrameMode mode,
  uint32_t                    frameIndex)
{
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisConditionAtFrame), NMP_VECTOR_ALIGNMENT);
  AnalysisConditionAtFrame* result = (AnalysisConditionAtFrame*)memRes.alignAndIncrement(memReqsHdr);
  result->m_instanceInitFn = NULL;
  result->m_instanceUpdateFn = instanceUpdate;
  result->m_instanceResetFn = NULL;

  result->m_mode = mode;
  result->m_frameIndex = frameIndex;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisConditionAtFrameBuilder::getMemoryRequirements(
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       NMP_UNUSED(networkDef),
  const ME::AnalysisNodeExport* NMP_USED_FOR_VERIFY(analysisExport))
{
  NMP_VERIFY(analysisExport);
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisCondition* AnalysisConditionAtFrameBuilder::init(
  NMP::Memory::Resource&      memRes,
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       NMP_UNUSED(networkDef),
  const ME::AnalysisNodeExport* analysisExport)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();
  bool status;

  // Mode  
  uint32_t mode = 0;
  status = analysisNodeDataBlock->readUInt(mode, "Mode");
  NMP_VERIFY_MSG(
    status,
    "Unable to find Mode attribute in the export");

  // Frame index
  uint32_t frameIndex = 0;
  status = analysisNodeDataBlock->readUInt(frameIndex, "FrameIndex");
  NMP_VERIFY_MSG(
    status,
    "Unable to find FrameIndex attribute in the export");

  AnalysisCondition* result = AnalysisConditionAtFrameBuilder::init(
    memRes,
    (AnalysisConditionAtFrame::AtFrameMode)mode,
    frameIndex);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisConditionAtFrameBuilder::instanceUpdate(  
  AnalysisCondition* condition,
  MR::Network* NMP_UNUSED(network),
  AnalysisInterval* analysisInterval,
  uint32_t frameIndex)
{
  NMP_VERIFY(condition);
  NMP_VERIFY(analysisInterval);

  uint32_t maxNumFrames = analysisInterval->getMaxNumFrames();
  NMP_VERIFY(frameIndex < maxNumFrames);

  AnalysisConditionAtFrame* target = (AnalysisConditionAtFrame*)condition;
  bool result = false;
  switch (target->m_mode)
  {
  case AnalysisConditionAtFrame::kFirstFrame:
    result = (frameIndex == 0);
    break;

  case AnalysisConditionAtFrame::kLastFrame:
    result = (frameIndex == maxNumFrames - 1);
    break;

  case AnalysisConditionAtFrame::kCustom:
    result = (frameIndex == target->m_frameIndex);
    break;

  default:
    NMP_VERIFY_FAIL("Invalid mode (%d)", target->m_mode);
  }

  return result;
}

}
