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
#include "Analysis/AnalysisPropertyTakeOffDirectionInPlane.h"
#include "morpheme/mrNetworkDef.h"
#include "NMNumerics/NMNumericUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisPropertyTakeOffDirectionInPlaneBuilder::getMemoryRequirements(
  AssetProcessor*               NMP_UNUSED(processor),
  MR::NetworkDef*               networkDef,
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport),
  uint32_t                      numFrames)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();

  // Header
  NMP::Memory::Format result(sizeof(AnalysisPropertyTakeOffDirectionInPlane), NMP_VECTOR_ALIGNMENT);

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
AnalysisProperty* AnalysisPropertyTakeOffDirectionInPlaneBuilder::init(
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
  NMP::Memory::Format memReqsHdr(sizeof(AnalysisPropertyTakeOffDirectionInPlane), NMP_VECTOR_ALIGNMENT);
  AnalysisPropertyTakeOffDirectionInPlane* result = (AnalysisPropertyTakeOffDirectionInPlane*)memRes.alignAndIncrement(memReqsHdr);

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

  // Sub-Range
  result->m_subRange = AnalysisPropertySubRange::init(memRes, numFrames);
  result->m_subRange->readExport(analysisNodeDataBlock);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisPropertyTakeOffDirectionInPlaneBuilder::instanceInit(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyTakeOffDirectionInPlane* target = (AnalysisPropertyTakeOffDirectionInPlane*)property;
  
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
void AnalysisPropertyTakeOffDirectionInPlaneBuilder::instanceUpdate(  
  AnalysisProperty* property,
  MR::Network* network)
{
  NMP_VERIFY(property);
  AnalysisPropertyTakeOffDirectionInPlane* target = (AnalysisPropertyTakeOffDirectionInPlane*)property;
  
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
void AnalysisPropertyTakeOffDirectionInPlaneBuilder::evaluateProperty(AnalysisProperty* property)
{
  NMP_VERIFY(property);
  AnalysisPropertyTakeOffDirectionInPlane* target = (AnalysisPropertyTakeOffDirectionInPlane*)property;
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

  // Up axis
  NMP::Vector3 planeNormal(0.0f, 0.0f, 0.0f);
  if (target->m_flipPlaneNormalDirection)
    planeNormal[target->m_planeNormal] = -1.0f;
  else
    planeNormal[target->m_planeNormal] = 1.0f;

  // Facing axis
  NMP::Vector3 facingDirection(0.0f, 0.0f, 0.0f);
  if (target->m_flipFacingDirection)
    facingDirection[target->m_facingDirection] = -1.0f;
  else
    facingDirection[target->m_facingDirection] = 1.0f;

  // Sideways axis
  NMP::Vector3 sidewaysDirection;
  sidewaysDirection.cross(planeNormal, facingDirection);

  // Sample range buffer
  uint32_t rangeStart, rangeEnd;
  target->m_subRange->findSampleRange(rangeStart, rangeEnd);

  //---------------------------
  // Accumulate the trajectory position
  float M[3] = {0.0f, 0.0f, 0.0f}; // Moment matrix of trajectory component positions
  float R[2] = {0.0f, 0.0f}; // Sum of trajectory component positions
  for (uint32_t i = rangeStart; i <= rangeEnd; ++i)
  {
    // Get the component of the accumulated trajectory movement in the character's start frame facing direction
    float deltaPosAccU = facingDirection.dot(sampleBuffer->m_channelPos[i]);

    // Get the component of the accumulated trajectory movement in the character's sideways direction
    float deltaPosAccV = sidewaysDirection.dot(sampleBuffer->m_channelPos[i]);

    // Update the moment matrix
    M[0] += deltaPosAccU * deltaPosAccU;
    M[1] += deltaPosAccU * deltaPosAccV;
    M[2] += deltaPosAccV * deltaPosAccV;
    R[0] += deltaPosAccU;
    R[1] += deltaPosAccV;
  }

  //---------------------------
  // Compute Eigen-decomposition of the moment matrix to find the directions of variance.
  // D = V^T * M * V, where V is the 2x2 orthogonal basis that diagonalises M into matrix D.
  float c, s;
  NMP::jacobi(M[0], M[1], M[2], c, s);

  // Diagonalise. Note M is symmetric, positive-definite so the Eigen-values are real and positive
  float D[2];
  float c2 = c * c;
  float s2 = s * s;
  float csm1 = 2 * c * s * M[1];
  D[0] = c2 * M[0] - csm1 + s2 * M[2];
  D[1] = s2 * M[0] + csm1 + c2 * M[2];

  // Recover the most variant Eigen-vector
  float V[2];
  if (D[0] >= D[1])
  {
    V[0] = c;
    V[1] = -s;
  }
  else
  {
    V[0] = s;
    V[1] = c;
  }

  // The most variant Eigen-vector can be either aligned toward or away from
  // the principal movement direction.
  if (V[0] * R[0] + V[1] * R[1] < 0.0f)
  {
    V[0] = -V[0];
    V[1] = -V[1];
  }

  // Convert the principal movement direction to an angle in the range [-pi : pi]
  target->m_value[0] = atan2(V[1], V[0]);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisPropertyTakeOffDirectionInPlaneBuilder::calculateRangeDistribution(
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
