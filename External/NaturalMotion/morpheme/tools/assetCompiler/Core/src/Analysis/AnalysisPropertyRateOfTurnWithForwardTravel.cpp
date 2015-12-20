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
#include "Analysis/AnalysisPropertyRateOfTurnWithForwardTravel.h"
#include "morpheme/mrNetworkDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyRateOfTurnWithForwardTravelBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyRateOfTurnWithForwardTravel), NMP_VECTOR_ALIGNMENT);

  // Result value
  NMP::Memory::Format memReqsValue(sizeof(float), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsValue;

  // Extracted features
  NMP::Memory::Format memReqsFeatures = AnalysisPropertySampleBuffer::getMemoryRequirements(
    numAnimSets,
    numFrames);
  result += memReqsFeatures;

  // Data Model
  NMP::Memory::Format memReqsDataModel = AnalysisPropertyDataModel::getMemoryRequirements();
  result += memReqsDataModel;

  // Sub-Range
  NMP::Memory::Format memReqsSubRange = AnalysisPropertySubRange::getMemoryRequirements(numFrames);
  result += memReqsSubRange;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProperty* AnalysisPropertyRateOfTurnWithForwardTravelBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyRateOfTurnWithForwardTravel), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyRateOfTurnWithForwardTravel* result = (AnalysisPropertyRateOfTurnWithForwardTravel*)memRes.alignAndIncrement(memReqsHdr);

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

  uint32_t axis = (uint32_t)kAPInvalidAxis;
  analysisNodeDataBlock->readUInt(axis, "TurnAxis");
  NMP_VERIFY_MSG(
    axis < kAPAxisMax,
    "Invalid TurnAxis attribute");
  result->m_turnAxis = (AnalysisPropertyAxis)axis;

  bool flipAxisDirection = false;
  bool status = analysisNodeDataBlock->readBool(flipAxisDirection, "FlipTurnAxisDirection");
  NMP_VERIFY_MSG(
    status,
    "Invalid FlipTurnAxisDirection attribute");
  result->m_flipTurnAxisDirection = flipAxisDirection;

  axis = (uint32_t)kAPInvalidAxis;
  analysisNodeDataBlock->readUInt(axis, "FacingDirection");
  NMP_VERIFY_MSG(
    axis < kAPAxisMax,
    "Invalid FacingDirection attribute");
  result->m_facingDirection = (AnalysisPropertyAxis)axis;

  flipAxisDirection = false;
  status = analysisNodeDataBlock->readBool(flipAxisDirection, "FlipFacingDirection");
  NMP_VERIFY_MSG(
    status,
    "Invalid FlipFacingDirection attribute");
  result->m_flipFacingDirection = flipAxisDirection;

  NMP_VERIFY_MSG(
    result->m_turnAxis != result->m_facingDirection,
    "The Turn axis and FacingDirection can't be collinear");

  // Sample buffer (This analysis property is specific to the trajectory)
  AnalysisPropertySampleBuffer* sampleBuffer = AnalysisPropertySampleBuffer::init(memRes, numAnimSets, numFrames);
  result->m_sampleBuffer = sampleBuffer;
  sampleBuffer->m_sampleFrequency = sampleFrequency;
  sampleBuffer->setTrajectoryMode(networkDef, kAPSpaceTrajectoryLocal);

  // Data Model
  result->m_dataModel = AnalysisPropertyDataModel::init(memRes);
  result->m_dataModel->readExport(analysisNodeDataBlock);

  // Sub-Range
  result->m_subRange = AnalysisPropertySubRange::init(memRes, numFrames);
  result->m_subRange->readExport(analysisNodeDataBlock);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyRateOfTurnWithForwardTravelBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyRateOfTurnWithForwardTravel* target = (AnalysisPropertyRateOfTurnWithForwardTravel*)property;
  
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
void AnalysisPropertyRateOfTurnWithForwardTravelBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyRateOfTurnWithForwardTravel* target = (AnalysisPropertyRateOfTurnWithForwardTravel*)property;
  
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
void AnalysisPropertyRateOfTurnWithForwardTravelBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyRateOfTurnWithForwardTravel* target = (AnalysisPropertyRateOfTurnWithForwardTravel*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);
  uint32_t numSamples = sampleBuffer->m_numFrames - 1;
  NMP_VERIFY_MSG(
    numSamples > 0,
    "There are an insufficient number of samples to evaluate VelocityInAxis");

  NMP_VERIFY(
    sampleBuffer->m_mode == kAPModeTrajectory &&
    sampleBuffer->m_trajectorySpace == kAPSpaceTrajectoryLocal);

  NMP_VERIFY(target->m_turnAxis < 3);
  NMP_VERIFY(target->m_facingDirection < 3);
  NMP_VERIFY(target->m_turnAxis != target->m_facingDirection);

  NMP::Vector3 turnAxis(0.0f, 0.0f, 0.0f);
  if (target->m_flipTurnAxisDirection)
    turnAxis[target->m_turnAxis] = -1.0f;
  else
    turnAxis[target->m_turnAxis] = 1.0f;

  NMP::Vector3 facingDirection(0.0f, 0.0f, 0.0f);
  if (target->m_flipFacingDirection)
    facingDirection[target->m_facingDirection] = -1.0f;
  else
    facingDirection[target->m_facingDirection] = 1.0f;

  // Default value
  target->m_value[0] = 0.0f;

  NMP::Memory::Format memReqs(sizeof(float) * numSamples, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResF = NMPMemoryAllocateFromFormat(memReqs);
  float* featureData = (float*)memResF.ptr;
  NMP::Memory::Resource memResS = NMPMemoryAllocateFromFormat(memReqs);
  float* smoothedData = (float*)memResS.ptr;

  // Using per frame trajectory delta
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    // Note that the [0] entry should be zero since it was set using absolute time
    NMP::Vector3 deltaPos = sampleBuffer->m_channelPos[i + 1];
    NMP::Vector3 deltaAtt = sampleBuffer->m_channelAtt[i + 1].toRotationVector();

    // Magnitude of translation in the facing direction.
    float deltaPosInFacingDir = deltaPos.dot(facingDirection);

    // Magnitude of rotation about the vertical direction.
    float deltaAttAboutVertical = deltaAtt.dot(turnAxis);

    // Rate of turn with forwards speed.
    if (NMP::nmfabs(deltaPosInFacingDir) > 0.00001f)
      featureData[i] = deltaAttAboutVertical / deltaPosInFacingDir;
    else
      featureData[i] = 0.0f;
  }

  // Sample range buffer
  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleDiffRange(rangeStart, rangeEnd);

  // Process the sample data
  target->m_value[0] = target->m_dataModel->fitDataModel(
    numSamples,
    featureData,
    rangeStart,
    rangeEnd);

  // Tidy up
  NMP::Memory::memFree(featureData);
  NMP::Memory::memFree(smoothedData);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyRateOfTurnWithForwardTravelBuilder::calculateRangeDistribution(
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
