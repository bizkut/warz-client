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
#ifndef AP_CONTROL_PARAMETER_REGULAR_SAMPLE_GRID_H
#define AP_CONTROL_PARAMETER_REGULAR_SAMPLE_GRID_H
//----------------------------------------------------------------------------------------------------------------------
#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class NetworkDef;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class ControlParameterRegularSampleGrid
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numControlParameters);

  static ControlParameterRegularSampleGrid* init(
    NMP::Memory::Resource& resource,
    uint32_t numControlParameters);

  static ControlParameterRegularSampleGrid* createFromExport(
    const ME::AnalysisNodeExport* analysisNodeExport,
    const MR::NetworkDef* networkDef);

  void releaseAndDestroy();

  NM_INLINE uint32_t getNumControlParameters() const;
  NM_INLINE MR::NodeID getControlParamNodeID(uint32_t i) const;
  NM_INLINE uint32_t getSampleCount(uint32_t i) const;
  NM_INLINE float getStartRange(uint32_t i) const;
  NM_INLINE float getEndRange(uint32_t i) const;

  NM_INLINE const uint32_t* getSampleCounts() const;
  NM_INLINE uint32_t getNumSamples() const;

private:
  uint32_t      m_numControlParameters;
  MR::NodeID*   m_controlParamNodeIDs;
  uint32_t*     m_sampleCounts;
  float*        m_startRanges;
  float*        m_endRanges;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ControlParameterRegularSampleGrid::getNumControlParameters() const
{
  return m_numControlParameters;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NodeID ControlParameterRegularSampleGrid::getControlParamNodeID(uint32_t i) const
{
  NMP_VERIFY(i < m_numControlParameters);
  return m_controlParamNodeIDs[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ControlParameterRegularSampleGrid::getSampleCount(uint32_t i) const
{
  NMP_VERIFY(i < m_numControlParameters);
  return m_sampleCounts[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float ControlParameterRegularSampleGrid::getStartRange(uint32_t i) const
{
  NMP_VERIFY(i < m_numControlParameters);
  return m_startRanges[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float ControlParameterRegularSampleGrid::getEndRange(uint32_t i) const
{
  NMP_VERIFY(i < m_numControlParameters);
  return m_endRanges[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint32_t* ControlParameterRegularSampleGrid::getSampleCounts() const
{
  return m_sampleCounts;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ControlParameterRegularSampleGrid::getNumSamples() const
{
  if (m_numControlParameters > 0)
  {
    uint32_t numSamples = 1;
    for (uint32_t i = 0; i < m_numControlParameters; ++i)
    {
      numSamples *= m_sampleCounts[i];
    }
    return numSamples;
  }
  return 0;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_CONTROL_PARAMETER_REGULAR_SAMPLE_GRID_H
//----------------------------------------------------------------------------------------------------------------------
