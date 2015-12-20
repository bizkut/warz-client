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
#ifndef AP_ANALYSIS_CONDITION_CROSSED_DURATION_FRACTION_H
#define AP_ANALYSIS_CONDITION_CROSSED_DURATION_FRACTION_H
//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/AnalysisCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionFractionThroughSource
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionCrossedDurationFraction : public AnalysisCondition
{
public:
  // Definition data
  MR::AttribAddress m_sourceNodeFractionalPosAttribAddress;
  MR::NodeID        m_sourceNodeID; // Source node that generates the fractional position data
  float             m_triggerFraction;

  // State data
  float             m_lastUpdateTimeFraction;  ///< The time fraction value from the last frames update.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionFractionThroughSourceBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionCrossedDurationFractionBuilder : public AnalysisConditionBuilder
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

  /// \brief Function to reset the internal state of the condition
  static void instanceReset(  
    AnalysisCondition* condition);
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_CONDITION_CROSSED_DURATION_FRACTION_H
//----------------------------------------------------------------------------------------------------------------------
