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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef AP_ANALYSIS_CONDITION_ATFRAME_H
#define AP_ANALYSIS_CONDITION_ATFRAME_H
//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/AnalysisCondition.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionAtFrame
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionAtFrame : public AnalysisCondition
{
public:
  typedef enum
  {
    kFirstFrame   = 0,
    kLastFrame    = 1,
    kCustom       = 2,
    numOptions,
    kInvalid      = 0xFFFFFFFF
  } AtFrameMode;

public:
  AtFrameMode   m_mode;
  uint32_t      m_frameIndex;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionAtFrameBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionAtFrameBuilder : public AnalysisConditionBuilder
{
public:
  /// \brief Compute the memory requirements for this analysis condition definition
  static NMP::Memory::Format getMemoryRequirements();

  /// \brief Initialise the data for this analysis condition definition
  static AnalysisCondition* init(
    NMP::Memory::Resource&      memRes,
    AnalysisConditionAtFrame::AtFrameMode mode,
    uint32_t                    frameIndex);

  /// \brief Initialise a default AtFrame analysis condition
  static NM_INLINE AnalysisCondition* create(
    AnalysisConditionAtFrame::AtFrameMode mode,
    uint32_t                    frameIndex);

  /// \brief Compute the memory requirements for this analysis condition definition
  virtual NMP::Memory::Format getMemoryRequirements(
    AssetProcessor*             processor,
    const MR::NetworkDef*       networkDef,
    const ME::AnalysisNodeExport* analysisExport) NM_OVERRIDE;

  /// \brief Initialise the data for this analysis condition definition
  virtual AnalysisCondition* init(
    NMP::Memory::Resource&      memRes,
    AssetProcessor*             processor,
    const MR::NetworkDef*       networkDef,
    const ME::AnalysisNodeExport* analysisExport) NM_OVERRIDE;

  /// \brief Function to check if the analysis condition has triggered
  static bool instanceUpdate(
    AnalysisCondition*          condition,
    MR::Network*                network,
    AnalysisInterval*           analysisInterval,
    uint32_t                    frameIndex);
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AnalysisCondition* AnalysisConditionAtFrameBuilder::create(
  AnalysisConditionAtFrame::AtFrameMode mode,
  uint32_t                    frameIndex)
{
  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  AnalysisCondition* result = init(memRes, mode, frameIndex);
  return result;
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_CONDITION_ATFRAME_H
//----------------------------------------------------------------------------------------------------------------------
