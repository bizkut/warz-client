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
#include "Analysis/AnalysisPropertyRotationDeltaInAxis.h"
#include "morpheme/mrNetworkDef.h"
#include "NMNumerics/NMNumericUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyRotationDeltaInAxisBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyRotationDeltaInAxis), NMP_VECTOR_ALIGNMENT);

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
AnalysisProperty* AnalysisPropertyRotationDeltaInAxisBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyRotationDeltaInAxis), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyRotationDeltaInAxis* result = (AnalysisPropertyRotationDeltaInAxis*)memRes.alignAndIncrement(memReqsHdr);

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

  // Axis
  uint32_t axis = (uint32_t)kAPInvalidAxis;
  analysisNodeDataBlock->readUInt(axis, "Axis");
  NMP_VERIFY_MSG(
    axis < kAPAxisMax,
    "Invalid axis attribute");
  result->m_axis = (AnalysisPropertyAxis)axis;

  bool flipAxisDirection = false;
  bool status = analysisNodeDataBlock->readBool(flipAxisDirection, "FlipAxisDirection");
  NMP_VERIFY_MSG(
    status,
    "Invalid flipAxisDirection attribute");
  result->m_flipAxisDirection = flipAxisDirection;

  // Extracted features
  AnalysisPropertySampleBuffer* sampleBuffer = AnalysisPropertySampleBuffer::init(memRes, numAnimSets, numFrames);
  result->m_sampleBuffer = sampleBuffer;
  sampleBuffer->m_sampleFrequency = sampleFrequency;
  sampleBuffer->readExport(networkDef, analysisExport);

  // Sub-Range
  result->m_subRange = AnalysisPropertySubRange::init(memRes, numFrames);
  result->m_subRange->readExport(analysisNodeDataBlock);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyRotationDeltaInAxisBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyRotationDeltaInAxis* target = (AnalysisPropertyRotationDeltaInAxis*)property;
  
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
void AnalysisPropertyRotationDeltaInAxisBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyRotationDeltaInAxis* target = (AnalysisPropertyRotationDeltaInAxis*)property;
  
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
void AnalysisPropertyRotationDeltaInAxisBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyRotationDeltaInAxis* target = (AnalysisPropertyRotationDeltaInAxis*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);

  // Rotation axis
  NMP_VERIFY(target->m_axis < 3);
  NMP::Vector3 axis(0.0f, 0.0f, 0.0f);
  if (target->m_flipAxisDirection)
    axis[target->m_axis] = -1.0f;
  else
    axis[target->m_axis] = 1.0f;

  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleRange(rangeStart, rangeEnd);

  //---------------------------
  // Compute the angle of rotation about the positive axis at the first frame
  NMP::Vector3 chanAtt = sampleBuffer->m_channelAtt[rangeStart].toRotationVector();
  float angle = chanAtt.dot(axis);
  float angle0 = angle;

  //---------------------------
  // Deal with wrapping and multiple revolutions by tracking the quadrant that
  // the angle lies in each frame.
  for (uint32_t i = rangeStart + 1; i <= rangeEnd; ++i)
  {
    // Compute the angle of rotation about the positive axis
    chanAtt = sampleBuffer->m_channelAtt[i].toRotationVector();
    float curAngle = chanAtt.dot(axis);

    // Condition the current rotation angle to be in the same quadrant as the
    // previous frames rotation
    NMP::closestAngle(angle, curAngle);

    // Update the rotation angle
    angle = curAngle;
  }

  // Set the result
  target->m_value[0] = angle - angle0;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyRotationDeltaInAxisBuilder::calculateRangeDistribution(
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
