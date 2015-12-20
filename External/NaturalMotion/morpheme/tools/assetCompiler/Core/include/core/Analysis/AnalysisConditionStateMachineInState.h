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
#ifndef AP_ANALYSIS_CONDITION_STATE_MACHINE_IN_STATE_H
#define AP_ANALYSIS_CONDITION_STATE_MACHINE_IN_STATE_H
//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/AnalysisCondition.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionStateMachineInState
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionStateMachineInState : public AnalysisCondition
{
public:
  // Definition data.
  MR::NodeID m_stateMachineNodeID; // State machine that we are monitoring.
  uint32_t   m_stateID;            // State that we are watching to become the root of the state machine.
  bool       m_notInState;         // Becomes true when not in the indicated state.
  
  // State data.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionStateMachineInStateBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionStateMachineInStateBuilder : public AnalysisConditionBuilder
{
public:
  /// \brief Compute the memory requirements for this analysis condition definition.
  virtual NMP::Memory::Format getMemoryRequirements(
    AssetProcessor*             processor,
    const MR::NetworkDef*       netDef,
    const ME::AnalysisNodeExport* analysisExport) NM_OVERRIDE;

  /// \brief Initialise the data for this analysis condition definition.
  virtual AnalysisCondition* init(
    NMP::Memory::Resource&      memRes,
    AssetProcessor*             processor,
    const MR::NetworkDef*       netDef,
    const ME::AnalysisNodeExport* analysisExport) NM_OVERRIDE;

  /// \brief Function to initialise the network instance for the analysis condition.
  static void instanceInit(
    AnalysisCondition* condition,
    MR::Network*       net,
    AnalysisInterval*  analysisInterval);

  /// \brief Function to check if the analysis condition has triggered.
  static bool instanceUpdate(
    AnalysisCondition* condition,
    MR::Network* net,
    AnalysisInterval* analysisInterval,
    uint32_t     frameIndex);

  /// \brief Function to reset the internal state of the condition.
  static void instanceReset(  
    AnalysisCondition* condition);
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_CONDITION_STATE_MACHINE_IN_STATE_H
//----------------------------------------------------------------------------------------------------------------------
