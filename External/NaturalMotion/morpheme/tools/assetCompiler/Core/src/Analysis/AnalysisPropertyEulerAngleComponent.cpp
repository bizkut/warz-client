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
#include "Analysis/AnalysisPropertyEulerAngleComponent.h"
#include "morpheme/mrNetworkDef.h"
#include "NMNumerics/NMNumericUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyEulerAngleComponentBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyEulerAngleComponent), NMP_VECTOR_ALIGNMENT);

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
AnalysisProperty* AnalysisPropertyEulerAngleComponentBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyEulerAngleComponent), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyEulerAngleComponent* result = (AnalysisPropertyEulerAngleComponent*)memRes.alignAndIncrement(memReqsHdr);

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

  // Component
  uint32_t component = (uint32_t)kAPInvalidAxis;
  analysisNodeDataBlock->readUInt(component, "Component");
  NMP_VERIFY_MSG(
    component < kAPAxisMax,
    "Invalid Component attribute");
  result->m_component = (AnalysisPropertyAxis)component;

  // Angle offset
  result->m_eulerOffsetX = 0.0f;
  result->m_eulerOffsetY = 0.0f;
  result->m_eulerOffsetZ = 0.0f;
  analysisNodeDataBlock->readFloat(result->m_eulerOffsetX, "RotationOffsetX");
  analysisNodeDataBlock->readFloat(result->m_eulerOffsetY, "RotationOffsetY");
  analysisNodeDataBlock->readFloat(result->m_eulerOffsetZ, "RotationOffsetZ");

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
void AnalysisPropertyEulerAngleComponentBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyEulerAngleComponent* target = (AnalysisPropertyEulerAngleComponent*)property;
  
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
void AnalysisPropertyEulerAngleComponentBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyEulerAngleComponent* target = (AnalysisPropertyEulerAngleComponent*)property;
  
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
void AnalysisPropertyEulerAngleComponentBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyEulerAngleComponent* target = (AnalysisPropertyEulerAngleComponent*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);
  NMP_VERIFY(target->m_component < 3);

  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleRange(rangeStart, rangeEnd);

  // Compute the offset rotation
  NMP::Vector3 eulerAngleOffset(target->m_eulerOffsetX, target->m_eulerOffsetY, target->m_eulerOffsetZ);
  NMP::Quat quatOffset;
  quatOffset.fromEulerXYZ(eulerAngleOffset);

  //---------------------------
  // Compute the Euler angle component at the first frame
  NMP::Quat chanAtt = sampleBuffer->m_channelAtt[rangeStart] * quatOffset;
  NMP::Vector3 eulerAngle = chanAtt.toEulerXYZ();
  float angle0 = eulerAngle.f[target->m_component];

  //---------------------------
  // Compute the average rotation angle
  float sumAngle = angle0;
  for (uint32_t i = rangeStart + 1; i <= rangeEnd; ++i)
  {
    // Recover the Euler angle component
    chanAtt = sampleBuffer->m_channelAtt[i] * quatOffset;
    eulerAngle = chanAtt.toEulerXYZ();
    float curAngle = eulerAngle.f[target->m_component];

    // Condition the current rotation angle to be in the same quadrant as the start angle
    NMP::closestAngle(angle0, curAngle);

    // Update the average angle
    sumAngle += curAngle;
  }

  // Set the result
  uint32_t sampleCount = rangeEnd - rangeStart + 1;
  target->m_value[0] = sumAngle / (float)sampleCount;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyEulerAngleComponentBuilder::calculateRangeDistribution(
  AssetProcessor*             NMP_UNUSED(processor),
  uint32_t                    numSamples,
  const float*                samples,
  float&                      minVal,
  float&                      maxVal,
  float&                      centreVal)
{
  return processAngularRangeDistribution(
    numSamples,
    samples,
    minVal,
    maxVal,
    centreVal);
}

}
