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
#include "Analysis/AnalysisPropertyTravelDirectionInPlane.h"
#include "morpheme/mrNetworkDef.h"
#include "NMNumerics/NMNumericUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyTravelDirectionInPlaneBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyTravelDirectionInPlane), NMP_VECTOR_ALIGNMENT);

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
AnalysisProperty* AnalysisPropertyTravelDirectionInPlaneBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyTravelDirectionInPlane), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyTravelDirectionInPlane* result = (AnalysisPropertyTravelDirectionInPlane*)memRes.alignAndIncrement(memReqsHdr);

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

  // Plane Normal
  uint32_t axis = (uint32_t)kAPInvalidAxis;
  analysisNodeDataBlock->readUInt(axis, "PlaneNormal");
  NMP_VERIFY_MSG(
    axis < kAPAxisMax,
    "Invalid PlaneNormal attribute");
  result->m_planeNormal = (AnalysisPropertyAxis)axis;

  bool flipAxisDirection = false;
  bool status = analysisNodeDataBlock->readBool(flipAxisDirection, "FlipNormalDirection");
  NMP_VERIFY_MSG(
    status,
    "Invalid FlipTurnAxisDirection attribute");
  result->m_flipPlaneNormalDirection = flipAxisDirection;

  // Facing Direction
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

  // Sample buffer
  AnalysisPropertySampleBuffer* sampleBuffer = AnalysisPropertySampleBuffer::init(memRes, numAnimSets, numFrames);
  result->m_sampleBuffer = sampleBuffer;
  sampleBuffer->m_sampleFrequency = sampleFrequency;
  sampleBuffer->readExport(networkDef, analysisExport);

  // Data Model
  result->m_dataModel = AnalysisPropertyDataModel::init(memRes);
  result->m_dataModel->readExport(analysisNodeDataBlock);

  // Sub-Range
  result->m_subRange = AnalysisPropertySubRange::init(memRes, numFrames);
  result->m_subRange->readExport(analysisNodeDataBlock);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyTravelDirectionInPlaneBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyTravelDirectionInPlane* target = (AnalysisPropertyTravelDirectionInPlane*)property;

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
void AnalysisPropertyTravelDirectionInPlaneBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyTravelDirectionInPlane* target = (AnalysisPropertyTravelDirectionInPlane*)property;
  
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
void AnalysisPropertyTravelDirectionInPlaneBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyTravelDirectionInPlane* target = (AnalysisPropertyTravelDirectionInPlane*)property;
  AnalysisPropertySampleBuffer* sampleBuffer = target->m_sampleBuffer;
  NMP_VERIFY(sampleBuffer);  
  NMP_VERIFY(sampleBuffer->m_numFrames > 0);
  uint32_t numSamples = sampleBuffer->m_numFrames - 1;
  NMP_VERIFY_MSG(
    numSamples > 0,
    "There are an insufficient number of samples to evaluate TravelDirectionInPlane");

  NMP_VERIFY(target->m_planeNormal < 3);
  NMP_VERIFY(target->m_facingDirection < 3);
  NMP_VERIFY(target->m_planeNormal != target->m_facingDirection);

  NMP::Vector3 planeNormal(0.0f, 0.0f, 0.0f);
  if (target->m_flipPlaneNormalDirection)
    planeNormal[target->m_planeNormal] = -1.0f;
  else
    planeNormal[target->m_planeNormal] = 1.0f;

  NMP::Vector3 facingDirection(0.0f, 0.0f, 0.0f);
  if (target->m_flipFacingDirection)
    facingDirection[target->m_facingDirection] = -1.0f;
  else
    facingDirection[target->m_facingDirection] = 1.0f;

  // Default value
  target->m_value[0] = 0.0f;

  NMP::Memory::Format memReqsDelta(sizeof(NMP::Vector3) * numSamples, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResDelta = NMPMemoryAllocateFromFormat(memReqsDelta);
  NMP::Vector3* deltaPosVec = (NMP::Vector3*)memResDelta.ptr;
  NMP::Memory::Format memReqs(sizeof(float) * numSamples, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResF = NMPMemoryAllocateFromFormat(memReqs);
  float* featureData = (float*)memResF.ptr;

  //---------------------------
  // Compute the delta pos channel data
  if (sampleBuffer->m_mode == kAPModeTrajectory &&
      sampleBuffer->m_trajectorySpace == kAPSpaceTrajectoryLocal)
  {
    // Note that the [0] entry should be zero since it was set using absolute time
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      deltaPosVec[i] = sampleBuffer->m_channelPos[i + 1];
    }
  }
  else
  {
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      deltaPosVec[i] = sampleBuffer->m_channelPos[i + 1] - sampleBuffer->m_channelPos[i];
    }
  }

  //---------------------------
  // Compute the angle of rotation about the positive axis at the first frame.
  NMP::Vector3 deltaPos = deltaPosVec[0];
  float vertDeltaPos = deltaPos.dot(planeNormal);
  deltaPos -= (planeNormal * vertDeltaPos);
  // Rotation that moves the facing direction onto the travel heading direction
  NMP::Quat q;
  q.forRotation(facingDirection, deltaPos);
  NMP::Vector3 alignAtt = q.toRotationVector();
  float angle = alignAtt.dot(planeNormal);
  featureData[0] = angle;

  //---------------------------
  // Deal with wrapping at the -pi and +pi boundary by tracking the quadrant
  // that the angle lies in each frame.
  for (uint32_t i = 1; i < numSamples; ++i)
  {
    deltaPos = deltaPosVec[i];
    vertDeltaPos = deltaPos.dot(planeNormal);
    deltaPos -= (planeNormal * vertDeltaPos);
    // Rotation that moves the facing direction onto the travel heading direction
    q.forRotation(facingDirection, deltaPos);
    alignAtt = q.toRotationVector();
    float curAngle = alignAtt.dot(planeNormal);

    // Condition the current rotation angle to be in the same quadrant as the
    // previous frames rotation
    NMP::closestAngle(angle, curAngle);
    featureData[i] = angle;
  }

  //---------------------------
  // Sample range buffer
  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleDiffRange(rangeStart, rangeEnd);

  // Process the sample data
  float smoothAngle = target->m_dataModel->fitDataModel(
    numSamples,
    featureData,
    rangeStart,
    rangeEnd);

  // Compute the feature value in the range [-pi:pi]
  NMP::cycleAngle(smoothAngle);
  target->m_value[0] = smoothAngle;

  // Tidy up
  NMP::Memory::memFree(deltaPosVec);
  NMP::Memory::memFree(featureData);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyTravelDirectionInPlaneBuilder::calculateRangeDistribution(
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
