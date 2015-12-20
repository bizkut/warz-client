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
#ifndef AP_ANALYSIS_CONDITION_DISCRETE_EVENT_TRIGGERED_H
#define AP_ANALYSIS_CONDITION_DISCRETE_EVENT_TRIGGERED_H
//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/AnalysisCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionAtEvent
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionDiscreteEventTriggered : public AnalysisCondition
{
public:
  // Definition data
  MR::AttribAddress m_sourceNodeSampledEventsAttribAddress;
  uint32_t          m_eventUserData;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionAtEventBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionDiscreteEventTriggeredBuilder : public AnalysisConditionBuilder
{
public:
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

  /// \brief Function to initialise the network instance for the analysis condition
  static void instanceInit(
    AnalysisCondition* condition,
    MR::Network* network,
    AnalysisInterval* analysisInterval);

  /// \brief Function to check if the analysis condition has triggered
  static bool instanceUpdate(
    AnalysisCondition* condition,
    MR::Network* network,
    AnalysisInterval* analysisInterval,
    uint32_t frameIndex);
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_CONDITION_DISCRETE_EVENT_TRIGGERED_H
//----------------------------------------------------------------------------------------------------------------------
