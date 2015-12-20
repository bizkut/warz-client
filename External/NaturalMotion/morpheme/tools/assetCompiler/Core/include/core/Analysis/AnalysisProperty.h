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
#ifndef AP_ANALYSIS_PROPERTY_H
#define AP_ANALYSIS_PROPERTY_H
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
class AnalysisProperty;

//----------------------------------------------------------------------------------------------------------------------
// Analysis property function declaration
typedef void (*AnalysisPropertyInstanceInitFn)(
  AnalysisProperty* property,
  MR::Network* network);

typedef void (*AnalysisPropertyInstanceUpdateFn)(
  AnalysisProperty* property,
  MR::Network* network);

typedef void (*AnalysisPropertyInstanceReleaseFn)(
  AnalysisProperty* property,
  MR::Network* network);

typedef void (*AnalysisPropertyEvaluateFn)(
  AnalysisProperty* property);

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisProperty
//----------------------------------------------------------------------------------------------------------------------
class AnalysisProperty
{
public:
  NM_INLINE void releaseAndDestroy();

  NM_INLINE void instanceInit(MR::Network* network);
  NM_INLINE void instanceUpdate(MR::Network* network);
  NM_INLINE void instanceRelease(MR::Network* network);

  NM_INLINE void evaluateProperty();

public:
  // Feature extraction from network instance
  AnalysisPropertyInstanceInitFn      m_instanceInitFn;
  AnalysisPropertyInstanceUpdateFn    m_instanceUpdateFn;
  AnalysisPropertyInstanceReleaseFn   m_instanceReleaseFn;

  // Analysis of extracted features
  AnalysisPropertyEvaluateFn          m_evaluatePropertyFn;

  // Analysis property data
  uint32_t                            m_numDims;
  float*                              m_value;
};


//----------------------------------------------------------------------------------------------------------------------
// AnalysisProperty inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisProperty::releaseAndDestroy()
{
  NMP_VERIFY(m_instanceReleaseFn);
  m_instanceReleaseFn(this, NULL);
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisProperty::instanceInit(MR::Network* network)
{
  NMP_VERIFY(m_instanceInitFn);
  m_instanceInitFn(this, network);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisProperty::instanceUpdate(MR::Network* network)
{
  NMP_VERIFY(m_instanceUpdateFn);
  m_instanceUpdateFn(this, network);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisProperty::instanceRelease(MR::Network* network)
{
  NMP_VERIFY(m_instanceReleaseFn);
  m_instanceReleaseFn(this, network);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisProperty::evaluateProperty()
{
  NMP_VERIFY(m_evaluatePropertyFn);
  m_evaluatePropertyFn(this);
}


//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertyBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertyBuilder
{
public:
  /// \brief Compute the memory requirements for this analysis property definition
  virtual NMP::Memory::Format getMemoryRequirements(
    AssetProcessor*               processor,
    MR::NetworkDef*               networkDef,
    const ME::AnalysisNodeExport* analysisExport,
    uint32_t                      numFrames) = 0;

  /// \brief Initialise the data for this analysis condition definition
  virtual AnalysisProperty* init(
    NMP::Memory::Resource&        memRes,
    AssetProcessor*               processor,
    MR::NetworkDef*               networkDef,
    const ME::AnalysisNodeExport* analysisExport,
    float                         sampleFrequency,
    uint32_t                      numFrames) = 0;

  NM_INLINE AnalysisProperty* create(
    AssetProcessor*               processor,
    MR::NetworkDef*               networkDef,
    const ME::AnalysisNodeExport* analysisExport,
    float                         sampleFrequency,
    uint32_t                      numFrames);

  /// \brief Function to compute the appropriate range distribution from the
  /// analysis property sample data.
  virtual bool calculateRangeDistribution(
    AssetProcessor*               processor,
    uint32_t                      numSamples,
    const float*                  samples,
    float&                        minVal,
    float&                        maxVal,
    float&                        centreVal) = 0;

public:
  static void defaultReleaseFn(
    AnalysisProperty* property,
    MR::Network* network);

  //---------------------------
  // Range distribution functions
  static bool processSampleRangeDistribution(
    uint32_t numSamples,
    const float* samples,
    float& minVal,
    float& maxVal,
    float& centreVal);

  static bool processAngularRangeDistribution(
    uint32_t numSamples,
    const float* samples,
    float& minVal,
    float& maxVal,
    float& centreVal);

  static bool processHalfAngularRangeDistribution(
    uint32_t numSamples,
    const float* samples,
    float& minVal,
    float& maxVal,
    float& centreVal);
};


//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertyBuilder inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AnalysisProperty* AnalysisPropertyBuilder::create(
  AssetProcessor*               processor,
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* analysisExport,
  float                         sampleFrequency,
  uint32_t                      numFrames)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(
    processor,
    networkDef,
    analysisExport,
    numFrames);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  AnalysisProperty* result = init(
    memRes,
    processor,
    networkDef,
    analysisExport,
    sampleFrequency,
    numFrames);

  return result;
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_PROPERTY_H
//----------------------------------------------------------------------------------------------------------------------
