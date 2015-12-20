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
#include "Analysis/AnalysisPropertySpeedForCutTurns.h"
#include "morpheme/mrNetworkDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertySpeedForCutTurnsBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertySpeedForCutTurns), NMP_VECTOR_ALIGNMENT);

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsValue;

  // Extracted features
  NMP::Memory::Format memReqsFeatures = AnalysisPropertySampleBuffer::getMemoryRequirements(
    numAnimSets,
    numFrames);
  result += memReqsFeatures;

  // Sub-Range
  NMP::Memory::Format memReqsSubRange = AnalysisPropertySubRange::getMemoryRequirements(numFrames);
  result += memReqsSubRange;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProperty* AnalysisPropertySpeedForCutTurnsBuilder::init(
  NMP::Memory::Resource&        memRes,
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* analysisExport,
  float                         sampleFrequency,
  uint32_t                      numFrames)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();
  NMP_VERIFY(analysisNodeDataBlock);
  MR::AnimSetIndex numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertySpeedForCutTurns), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertySpeedForCutTurns* result = (AnalysisPropertySpeedForCutTurns*)memRes.alignAndIncrement(memReqsHdr);

  // Set the network instance and evaluation handlers
  result->m_instanceInitFn = instanceInit;
  result->m_instanceUpdateFn = instanceUpdate;
  result->m_instanceReleaseFn = AnalysisPropertyBuilder::defaultReleaseFn;
  result->m_evaluatePropertyFn = evaluateProperty;

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_value = (float*)memRes.alignAndIncrement(memReqsValue);
  result->m_numDims = 1;
  result->m_value[0] = 0.0f;

  // Turn Axis
  uint32_t axis = (uint32_t)kAPInvalidAxis;
  analysisNodeDataBlock->readUInt(axis, "TurnAxis");
  NMP_VERIFY_MSG(
    axis < kAPAxisMax,
    "Invalid axis attribute");
  result->m_turnAxis = (AnalysisPropertyAxis)axis;

  bool flipAxisDirection = false;
  bool status = analysisNodeDataBlock->readBool(flipAxisDirection, "FlipTurnAxisDirection");
  NMP_VERIFY_MSG(
    status,
    "Invalid flipAxisDirection attribute");
  result->m_flipTurnAxisDirection = flipAxisDirection;

  // Sample buffer (This analysis property is specific to the trajectory)
  AnalysisPropertySampleBuffer* sampleBuffer = AnalysisPropertySampleBuffer::init(memRes, numAnimSets, numFrames);
  result->m_sampleBuffer = sampleBuffer;
  sampleBuffer->m_sampleFrequency = sampleFrequency;
  sampleBuffer->setTrajectoryMode(networkDef, kAPSpaceTrajectoryLocal);

  // Sub-Range
  result->m_subRange = AnalysisPropertySubRange::init(memRes, numFrames);
  result->m_subRange->readExport(analysisNodeDataBlock);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySpeedForCutTurnsBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertySpeedForCutTurns* target = (AnalysisPropertySpeedForCutTurns*)property;

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);
  sampleBuffer->instanceInit(network);

  // Sub-Range
  AnalysisPropertySubRange* subRange = target->m_subRange;
  NMP_VERIFY(subRange);
  subRange->instanceInit(network);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySpeedForCutTurnsBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertySpeedForCutTurns* target = (AnalysisPropertySpeedForCutTurns*)property;

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);
  sampleBuffer->instanceUpdate(network);

  // Sub-Range
  AnalysisPropertySubRange* subRange = target->m_subRange;
  NMP_VERIFY(subRange);
  subRange->instanceUpdate(network);
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertySpeedForCutTurnsBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertySpeedForCutTurns* target = (AnalysisPropertySpeedForCutTurns*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);
  uint32_t numSamples = sampleBuffer->m_numFrames - 1;
  NMP_VERIFY_MSG(
    numSamples > 0,
    "There are an insufficient number of samples to evaluate TurningRate");

  NMP_VERIFY(target->m_turnAxis < 3);
  NMP::Vector3 turnAxis(0.0f, 0.0f, 0.0f);
  if (target->m_flipTurnAxisDirection)
    turnAxis[target->m_turnAxis] = -1.0f;
  else
    turnAxis[target->m_turnAxis] = 1.0f;

  // Default value
  target->m_value[0] = 0.0f;

  NMP::Memory::Format memReqs(sizeof(float) * numSamples, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResTRD = NMPMemoryAllocateFromFormat(memReqs);
  float* turningRateData = (float*)memResTRD.ptr;
  NMP::Memory::Resource memResTVD = NMPMemoryAllocateFromFormat(memReqs);
  float* posVelData = (float*)memResTVD.ptr;
  NMP::Memory::Resource memResS = NMPMemoryAllocateFromFormat(memReqs);
  float* smoothedData = (float*)memResS.ptr;
  NMP::Memory::Resource memResW = NMPMemoryAllocateFromFormat(memReqs);
  float* weights = (float*)memResW.ptr;

  //---------------------------
  // Extract the per frame trajectory and turning speeds
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    // Note that the [0] entry should be zero since it was set using absolute time
    NMP::Quat deltaAtt = sampleBuffer->m_channelAtt[i + 1];
    NMP::Vector3 deltaAttVel = deltaAtt.toRotationVector() * sampleBuffer->m_sampleFrequency;
    float velInAxis = deltaAttVel.dot(turnAxis);
    turningRateData[i] = velInAxis;

    // Position channel velocity
    NMP::Vector3 trajectoryDeltaVel = sampleBuffer->m_channelPos[i + 1] * sampleBuffer->m_sampleFrequency;
    posVelData[i] = trajectoryDeltaVel.magnitude();
  }

  //---------------------------
  // Sample range buffer
  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleDiffRange(rangeStart, rangeEnd);
  uint32_t sampleCount = rangeEnd - rangeStart + 1;

  // Apply smoothing to the turning rate data
  AnalysisPropertyUtils::processSmoothedSamples(
    numSamples,
    turningRateData,
    rangeStart,
    rangeEnd,
    smoothedData);

  // Compute the turning speed weight vector
  AnalysisPropertyUtils::calculateWeightsForNearZeroValuedSamples(
    sampleCount,
    smoothedData,
    weights);

  // Recover the pos velocity data sub-section
  float* posVelSubData = &posVelData[rangeStart];
 
  // Compute the weighted trajectory speed estimate
  float paramValue = 0.0f;
  float sumWeights = 0.0f;
  for (uint32_t i = 0; i < sampleCount; ++i)
  {
    paramValue += posVelSubData[i] * weights[i];
    sumWeights += weights[i];
  }
  NMP_VERIFY(sumWeights > 0.0f);
  paramValue /= sumWeights;

  target->m_value[0] = paramValue;

  //---------------------------
  // Tidy up
  NMP::Memory::memFree(turningRateData);
  NMP::Memory::memFree(posVelData);
  NMP::Memory::memFree(smoothedData);
  NMP::Memory::memFree(weights);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertySpeedForCutTurnsBuilder::calculateRangeDistribution(
  AssetProcessor*             NMP_UNUSED(processor),
  uint32_t                    numSamples,
  const float*                samples,
  float&                      minVal,
  float&                      maxVal,
  float&                      centreVal)
{
  return processSampleRangeDistribution(
    numSamples,
    samples,
    minVal,
    maxVal,
    centreVal);
}

}
