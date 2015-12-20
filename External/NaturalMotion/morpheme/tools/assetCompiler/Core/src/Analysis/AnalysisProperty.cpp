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
#include "Analysis/AnalysisProperty.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnalysisPropertyBuilder
//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyBuilder::defaultReleaseFn(
  AnalysisProperty* NMP_UNUSED(property),
  MR::Network* NMP_UNUSED(network))
{
  // There is no internal data to release
}

//----------------------------------------------------------------------------------------------------------------------
// Range distribution functions
bool AnalysisPropertyBuilder::processSampleRangeDistribution(
  uint32_t numSamples,
  const float* samples,
  float& minVal,
  float& maxVal,
  float& centreVal)
{
  NMP_VERIFY(numSamples > 0);
  NMP_VERIFY(samples);

  // Determine the min and max of the distribution
  float minVal_ = samples[0];
  float maxVal_ = samples[0];
  float centreSum = samples[0];
  for (uint32_t i = 1; i < numSamples; ++i)
  {
    minVal_ = NMP::minimum(minVal_, samples[i]);
    maxVal_ = NMP::maximum(maxVal_, samples[i]);
    centreSum += samples[i];
  }
  minVal = minVal_;
  maxVal = maxVal_;
  centreVal = centreSum / numSamples;

  return false; // Distribution range doesn't support wrapping
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyBuilder::processAngularRangeDistribution(
  uint32_t NMP_UNUSED(numSamples),
  const float* NMP_UNUSED(samples),
  float& minVal,
  float& maxVal,
  float& centreVal)
{
  minVal = -NM_PI;
  maxVal = NM_PI;
  centreVal = 0.0f;
  return true; // The distribution range supports wrapping
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyBuilder::processHalfAngularRangeDistribution(
  uint32_t NMP_UNUSED(numSamples),
  const float* NMP_UNUSED(samples),
  float& minVal,
  float& maxVal,
  float& centreVal)
{
  minVal = -NM_PI / 2.0f;
  maxVal = NM_PI / 2.0f;
  centreVal = 0.0f;
  return false; // No wrapping ?
}

}
