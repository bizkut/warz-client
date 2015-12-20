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
#ifndef AP_ANALYSIS_CONDITION_H
#define AP_ANALYSIS_CONDITION_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class NetworkDef;
class Network;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class AssetProcessor;
class AnalysisCondition;
class AnalysisInterval;

//----------------------------------------------------------------------------------------------------------------------
// Analysis condition function declaration
typedef void (*AnalysisConditionInstanceInitFn)(
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* analysisInterval);

typedef bool (*AnalysisConditionInstanceUpdateFn)(
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* analysisInterval,
  uint32_t frameIndex);

typedef void (*AnalysisConditionInstanceResetFn)(
  AnalysisCondition* condition);

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisCondition
//----------------------------------------------------------------------------------------------------------------------
class AnalysisCondition
{
public:
  AnalysisConditionInstanceInitFn     m_instanceInitFn;
  AnalysisConditionInstanceUpdateFn   m_instanceUpdateFn;
  AnalysisConditionInstanceResetFn    m_instanceResetFn;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisConditionBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnalysisConditionBuilder
{
public:
  /// \brief Compute the memory requirements for this analysis condition definition
  virtual NMP::Memory::Format getMemoryRequirements(
    AssetProcessor*             processor,
    const MR::NetworkDef*       networkDef,
    const ME::AnalysisNodeExport* analysisExport) = 0;

  /// \brief Initialise the data for this analysis condition definition
  virtual AnalysisCondition* init(
    NMP::Memory::Resource&      memRes,
    AssetProcessor*             processor,
    const MR::NetworkDef*       networkDef,
    const ME::AnalysisNodeExport* analysisExport) = 0;

  NM_INLINE AnalysisCondition* create(
    AssetProcessor*             processor,
    const MR::NetworkDef*       networkDef,
    const ME::AnalysisNodeExport* analysisExport);
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AnalysisCondition* AnalysisConditionBuilder::create(
  AssetProcessor*             processor,
  const MR::NetworkDef*       networkDef,
  const ME::AnalysisNodeExport* analysisExport)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(processor, networkDef, analysisExport);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  AnalysisCondition* result = init(memRes, processor, networkDef, analysisExport);
  return result;
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_CONDITION_H
//----------------------------------------------------------------------------------------------------------------------
