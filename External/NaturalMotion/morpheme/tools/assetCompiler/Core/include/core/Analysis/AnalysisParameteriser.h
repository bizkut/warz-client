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
#ifndef AP_ANALYSIS_PARAMETERISER_H
#define AP_ANALYSIS_PARAMETERISER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVectorContainer.h"
#include "Analysis/AnalysisProperty.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class OrderedStringTable;
}

namespace MR
{
class NetworkDef;
class Network;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisParameteriser
//----------------------------------------------------------------------------------------------------------------------
class AnalysisParameteriser
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t maxNumProperties);

  static AnalysisParameteriser* init(NMP::Memory::Resource& resource, uint32_t maxNumProperties);

  static AnalysisParameteriser* create(uint32_t maxNumProperties);

  void releaseAndDestroy();

  void readAnalysisProperties(
    AP::AssetProcessor* processor,
    MR::NetworkDef* networkDef,
    float sampleFrequency,
    uint32_t numFrames,
    const ME::AnalysisTaskExport* task,
    const ME::AnalysisNodeExport* analyserExport);

  // Feature extraction from network instance
  void instanceInit(MR::Network* network);
  void instanceUpdate(MR::Network* network);
  void instanceRelease(MR::Network* network);

  // Analysis of extracted features
  void evaluateProperties();

  NM_INLINE NMP::VectorContainer<AnalysisProperty*>* getAnalysisProperties() const;

  NM_INLINE uint32_t getNumAnalysisProperties() const;

  AnalysisProperty* getAnlysisProperty(uint32_t idx) const;

  AnalysisPropertyBuilder* getAnalysisPropertyBuilder(
    AssetProcessor* processor,
    uint32_t idx,
    const ME::AnalysisTaskExport* task,
    const ME::AnalysisNodeExport* analyserExport) const;

private:
  NMP::VectorContainer<AnalysisProperty*>*    m_analysisProperties;
};


//----------------------------------------------------------------------------------------------------------------------
// AnalysisParameteriser inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::VectorContainer<AnalysisProperty*>* AnalysisParameteriser::getAnalysisProperties() const
{
  return m_analysisProperties;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnalysisParameteriser::getNumAnalysisProperties() const
{
  return m_analysisProperties->size();
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_PARAMETERISER_H
//----------------------------------------------------------------------------------------------------------------------
