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
#ifndef AP_NDMESH_ANALYSIS_DATA_H
#define AP_NDMESH_ANALYSIS_DATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
#include "Analysis/ControlParameterRegularSampleGrid.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class NDMeshAnalysisData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numControlParameters,
    uint32_t numAnalysisProperties,
    const uint32_t* sampleCounts);

  static NDMeshAnalysisData* init(
    NMP::Memory::Resource& resource,
    uint32_t numControlParameters,
    uint32_t numAnalysisProperties,
    const uint32_t* sampleCounts);

  static NDMeshAnalysisData* createForAnalysis(
    const ControlParameterRegularSampleGrid* regularSampleGrid,
    uint32_t numAnalysisProperties);

  static NDMeshAnalysisData* createForPreview(
    const ME::AnalysisNodeExport* ndMeshNodeExport,
    MR::AnimSetIndex              animSetIndex);

  void releaseAndDestroy();

  // Write the result data to the analysis task result data export
  void writeExportData(
    ME::DataBlockExport* resultData) const;

  void writePerSetExportData(
    MR::AnimSetIndex animSetIndex,
    ME::DataBlockExport* resultData) const;

  // Regular grid
  NM_INLINE uint32_t getSampleCount(uint32_t i) const;
  NM_INLINE float getStartRange(uint32_t i) const;
  NM_INLINE float getEndRange(uint32_t i) const;

  NM_INLINE const float* getStartRanges() const;
  NM_INLINE const float* getEndRanges() const;

  NM_INLINE const uint32_t* getSampleCounts() const;
  NM_INLINE uint32_t getNumAnalysedSamples() const;

  // Control parameters
  NM_INLINE uint32_t getNumControlParameters() const;

  NM_INLINE float getControlParameterSample(
    uint32_t cpIndex,
    uint32_t sampleIndex) const;

  NM_INLINE void setControlParameterSample(
    uint32_t cpIndex,
    uint32_t sampleIndex,
    float value);

  // Analysis properties
  NM_INLINE uint32_t getNumAnalysisProperties() const;

  NM_INLINE float getAnalysisPropertySample(
    uint32_t apIndex,
    uint32_t sampleIndex) const;

  NM_INLINE void setAnalysisPropertySample(
    uint32_t apIndex,
    uint32_t sampleIndex,
    float value);

  // Accessors
  NM_INLINE bool getBuildInverseMap() const;

private:
  uint32_t      m_numControlParameters;
  uint32_t      m_numAnalysisProperties;
  uint32_t      m_numAnalysedSamples; // i.e. NumSamples0 * NumSamples1 * ...
  bool          m_buildInverseMap;

  // Regular grid
  uint32_t*     m_sampleCounts;
  float*        m_startRanges;
  float*        m_endRanges;

  // Control parameters
  float**       m_controlParameterSamples;

  // Analysis properties
  float**       m_analysisPropertySamples;
};


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAnalysisData::getSampleCount(uint32_t i) const
{
  NMP_VERIFY(i < m_numControlParameters);
  return m_sampleCounts[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshAnalysisData::getStartRange(uint32_t i) const
{
  NMP_VERIFY(i < m_numControlParameters);
  return m_startRanges[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshAnalysisData::getEndRange(uint32_t i) const
{
  NMP_VERIFY(i < m_numControlParameters);
  return m_endRanges[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NDMeshAnalysisData::getStartRanges() const
{
  return m_startRanges;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* NDMeshAnalysisData::getEndRanges() const
{
  return m_endRanges;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* NDMeshAnalysisData::getSampleCounts() const
{
  return m_sampleCounts;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAnalysisData::getNumAnalysedSamples() const
{
  return m_numAnalysedSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAnalysisData::getNumControlParameters() const
{
  return m_numControlParameters;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshAnalysisData::getControlParameterSample(
  uint32_t cpIndex,
  uint32_t sampleIndex) const
{
  NMP_VERIFY(cpIndex < m_numControlParameters);
  NMP_VERIFY(sampleIndex < m_numAnalysedSamples);
  return m_controlParameterSamples[cpIndex][sampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshAnalysisData::setControlParameterSample(
  uint32_t cpIndex,
  uint32_t sampleIndex,
  float value)
{
  NMP_VERIFY(cpIndex < m_numControlParameters);
  NMP_VERIFY(sampleIndex < m_numAnalysedSamples);
  m_controlParameterSamples[cpIndex][sampleIndex] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NDMeshAnalysisData::getNumAnalysisProperties() const
{
  return m_numAnalysisProperties;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NDMeshAnalysisData::getAnalysisPropertySample(
  uint32_t apIndex,
  uint32_t sampleIndex) const
{
  NMP_VERIFY(apIndex < m_numAnalysisProperties);
  NMP_VERIFY(sampleIndex < m_numAnalysedSamples);
  return m_analysisPropertySamples[apIndex][sampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NDMeshAnalysisData::setAnalysisPropertySample(
  uint32_t apIndex,
  uint32_t sampleIndex,
  float value)
{
  NMP_VERIFY(apIndex < m_numAnalysisProperties);
  NMP_VERIFY(sampleIndex < m_numAnalysedSamples);
  m_analysisPropertySamples[apIndex][sampleIndex] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool NDMeshAnalysisData::getBuildInverseMap() const
{
  return m_buildInverseMap;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_NDMESH_ANALYSIS_DATA_H
//----------------------------------------------------------------------------------------------------------------------
