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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMvpu.h"
#include "morpheme/Nodes/mrScatterBlend1DUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// MR::ScatterBlend1DSubSampleManager
//----------------------------------------------------------------------------------------------------------------------
// 0 Sub-divisions (1 line, 2 sub-samples)
static const float g_subSampleWeightsA0[2] = {1.0f, 0.0f};
static const float g_subSampleWeightsB0[2] = {0.0f, 1.0f};

//----------------------------------------------------------------------------------------------------------------------
// 1 Sub-divisions (2 lines, 3 sub-samples)
static const float g_subSampleWeightsA1[3] = {1.0f, 0.5f, 0.0f};
static const float g_subSampleWeightsB1[3] = {0.0f, 0.5f, 1.0f};

//----------------------------------------------------------------------------------------------------------------------
// 2 Sub-divisions (4 lines, 5 sub-samples)
static const float g_subSampleWeightsA2[5] = {1.0f, 0.75f, 0.5f, 0.25f, 0.0f};
static const float g_subSampleWeightsB2[5] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};

//----------------------------------------------------------------------------------------------------------------------
const ScatterBlend1DSubSampleManager::SubDivisionDataBlock ScatterBlend1DSubSampleManager::sm_subDivisionDataBlocks[3] =
{
  //------------------------
  // 0 Sub-divisions
  {
    2, // m_numSubSamplesPerLine
    {g_subSampleWeightsA0, g_subSampleWeightsB0}
  },

  //------------------------
  // 1 Sub-division
  {
    3, // m_numSubSamplesPerLine
    {g_subSampleWeightsA1, g_subSampleWeightsB1}
  },

  //------------------------
  // 2 Sub-divisions
  {
    5, // m_numSubSamplesPerLine
    {g_subSampleWeightsA2, g_subSampleWeightsB2}
  }
};

//----------------------------------------------------------------------------------------------------------------------
// ScatterBlend1D API
//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_PLATFORM_SIMD
static bool scatterBlend1DComputeLineBlendWeightsForInputControl0(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend1DDef* attribDef,
  float inputMotionParameter,
  ScatterBlend1DParameteriserInfo& result)
{
  NMP_ASSERT(attribDef);

  // Note that the number of vertices can be greater than the number of child sources
  // if wrapping samples have been added to the sample data
  uint16_t numLines4 = NMP::nmAlignedValue4(attribDef->m_numLines);
  uint16_t lastSubSample = attribDef->m_numSubSamplesPerLine - 1;

  // Input motion parameter
  NMP::vpu::vector4_t sampleP = NMP::vpu::set4f(inputMotionParameter);

  // Result state
  NMP::vpu::vector4_t resultFlags = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultLineIndices = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultWeights = NMP::vpu::zero4f();

  uint16_t lineIndex = 0;
  for (; lineIndex < numLines4; lineIndex += 4)
  {
    uint32_t lineVertexIndicesA[4];
    uint32_t lineVertexIndicesB[4];
    NMP::Vector3 lineIndicesVec;
    uint32_t* lineIndices = (uint32_t*)&lineIndicesVec;
    const uint16_t* lineSampleIndices;

    // Line indices to process
    lineIndices[0] = lineIndex;
    lineIndices[1] = lineIndex + 1;
    lineIndices[2] = lineIndex + 2;
    lineIndices[3] = lineIndex + 3;

    // Recover the vertex indices
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[0] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[0] = lineSampleIndices[0];
    lineVertexIndicesB[0] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[0] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[0] < attribDef->m_numVertices);
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[1] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[1] = lineSampleIndices[0];
    lineVertexIndicesB[1] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[1] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[1] < attribDef->m_numVertices);
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[2] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[2] = lineSampleIndices[0];
    lineVertexIndicesB[2] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[2] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[2] < attribDef->m_numVertices);
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[3] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[3] = lineSampleIndices[0];
    lineVertexIndicesB[3] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[3] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[3] < attribDef->m_numVertices);

    // Recover the sample data
    NMP::vpu::vector4_t sampleA = NMP::vpu::set4f(
      attribDef->m_samples[lineVertexIndicesA[0]],
      attribDef->m_samples[lineVertexIndicesA[1]],
      attribDef->m_samples[lineVertexIndicesA[2]],
      attribDef->m_samples[lineVertexIndicesA[3]]);

    NMP::vpu::vector4_t sampleB = NMP::vpu::set4f(
      attribDef->m_samples[lineVertexIndicesB[0]],
      attribDef->m_samples[lineVertexIndicesB[1]],
      attribDef->m_samples[lineVertexIndicesB[2]],
      attribDef->m_samples[lineVertexIndicesB[3]]);

    // Compute the weight
    NMP::vpu::vector4_t AP = NMP::vpu::sub4f(sampleP, sampleA);
    NMP::vpu::vector4_t AB = NMP::vpu::sub4f(sampleB, sampleA);
    NMP::vpu::vector4_t recipAB = NMP::vpu::rcp4f(AB); // We make sure this is not singular in the asset compiler
    NMP::vpu::vector4_t weights = NMP::vpu::mul4f(AP, recipAB);

    // Check if the input control lies within the line (avoiding conditional branches)
    NMP::vpu::vector4_t maskA = NMP::vpu::mask4cmpLT(weights, NMP::vpu::zero4f()); // weight < 0
    NMP::vpu::vector4_t maskB = NMP::vpu::mask4cmpLT(NMP::vpu::neg4f(weights), NMP::vpu::negOne4f()); // -weight < -1
    NMP::vpu::vector4_t mask = NMP::vpu::not4(NMP::vpu::or4(maskA, maskB)); // weight >= 0 && weight <= 1

    // Update the result based on the condition mask
    resultFlags = NMP::vpu::sel4cmpMask(mask, mask, resultFlags);
    resultLineIndices = NMP::vpu::sel4cmpMask(mask, NMP::vpu::load4f((float*)lineIndices), resultLineIndices);
    resultWeights = NMP::vpu::sel4cmpMask(mask, weights, resultWeights);
  }

  //------------------------
  // Recover the result
  NMP::Vector3 childNodeFlagsVec;
  uint32_t* childNodeFlags = (uint32_t*)&childNodeFlagsVec;
  NMP::vpu::store4f((float*)childNodeFlags, resultFlags);

  for (uint16_t i = 0; i < 4; ++i)
  {
    if (childNodeFlags[i])
    {
      NMP::Vector3 childNodeWeights;
      NMP::vpu::store4f((float*)&childNodeWeights, resultWeights);
      NMP::Vector3 lineIndicesVec;
      uint32_t* lineIndices = (uint32_t*)&lineIndicesVec;
      NMP::vpu::store4f((float*)lineIndices, resultLineIndices);
      const uint16_t* lineSampleIndices = attribDef->getLineSampleIndices(lineIndices[i]);

      uint32_t vertexSourceIndexA = attribDef->m_vertexSourceIndices[lineSampleIndices[0]];
      uint32_t vertexSourceIndexB = attribDef->m_vertexSourceIndices[lineSampleIndices[lastSubSample]];
      result.m_childNodeIDs[0] = nodeDef->getChildNodeID(vertexSourceIndexA);
      result.m_childNodeIDs[1] = nodeDef->getChildNodeID(vertexSourceIndexB);

      result.m_childNodeWeights[0] = 1.0f - childNodeWeights[i];
      result.m_childNodeWeights[1] = childNodeWeights[i];
      result.m_motionParameter = inputMotionParameter;
      return true;
    }
  }

  return false;
}

#else
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend1DComputeLineBlendWeightsForInputControl0(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend1DDef* attribDef,
  float inputMotionParameter,
  ScatterBlend1DParameteriserInfo& result)
{
  NMP_ASSERT(attribDef);

  // Note that the number of vertices can be greater than the number of child sources
  // if wrapping samples have been added to the sample data
  uint16_t lastSubSample = attribDef->m_numSubSamplesPerLine - 1;

  for (uint16_t lineIndex = 0; lineIndex < attribDef->m_numLines; ++lineIndex)
  {
    // Recover the vertex sample indices for the line
    const uint16_t* lineSampleIndices = attribDef->getLineSampleIndices(lineIndex);
    NMP_ASSERT(lineSampleIndices);
    uint16_t lineVertexIndices[2];
    lineVertexIndices[0] = lineSampleIndices[0];
    lineVertexIndices[1] = lineSampleIndices[lastSubSample];

    // Recover the sample data
    NMP_ASSERT(lineVertexIndices[0] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndices[1] < attribDef->m_numVertices);
    float sampleA = attribDef->m_samples[lineVertexIndices[0]];
    float sampleB = attribDef->m_samples[lineVertexIndices[1]];

    // Check if the input control lies within the line
    float weight = (inputMotionParameter - sampleA) / (sampleB - sampleA);
    if (weight >= 0.0f && weight <= 1.0f)
    {
      uint32_t vertexSourceIndexA = attribDef->m_vertexSourceIndices[lineVertexIndices[0]];
      uint32_t vertexSourceIndexB = attribDef->m_vertexSourceIndices[lineVertexIndices[1]];
      result.m_childNodeIDs[0] = nodeDef->getChildNodeID(vertexSourceIndexA);
      result.m_childNodeIDs[1] = nodeDef->getChildNodeID(vertexSourceIndexB);
      result.m_childNodeWeights[0] = 1.0f - weight;
      result.m_childNodeWeights[1] = weight;
      result.m_motionParameter = inputMotionParameter;
      return true;
    }
  }

  return false;
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_PLATFORM_SIMD
static bool scatterBlend1DComputeLineBlendWeightsForInputControlN(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend1DDef* attribDef,
  float inputMotionParameter,
  ScatterBlend1DParameteriserInfo& result)
{
  NMP_ASSERT(attribDef);

  // Note that the number of vertices can be greater than the number of child sources
  // if wrapping samples have been added to the sample data
  uint16_t numLines4 = NMP::nmAlignedValue4(attribDef->m_numLines);
  uint16_t lastSubSample = attribDef->m_numSubSamplesPerLine - 1;

  // Input motion parameter
  NMP::vpu::vector4_t sampleP = NMP::vpu::set4f(inputMotionParameter);

  // Result state
  NMP::vpu::vector4_t resultFlags = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultLineIndices = NMP::vpu::zero4f();

  uint16_t lineIndex = 0;
  for (; lineIndex < numLines4; lineIndex += 4)
  {
    uint32_t lineVertexIndicesA[4];
    uint32_t lineVertexIndicesB[4];
    NMP::Vector3 lineIndicesVec;
    uint32_t* lineIndices = (uint32_t*)&lineIndicesVec;
    const uint16_t* lineSampleIndices;

    // Line indices to process
    lineIndices[0] = lineIndex;
    lineIndices[1] = lineIndex + 1;
    lineIndices[2] = lineIndex + 2;
    lineIndices[3] = lineIndex + 3;

    // Recover the vertex indices
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[0] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[0] = lineSampleIndices[0];
    lineVertexIndicesB[0] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[0] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[0] < attribDef->m_numVertices);
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[1] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[1] = lineSampleIndices[0];
    lineVertexIndicesB[1] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[1] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[1] < attribDef->m_numVertices);
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[2] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[2] = lineSampleIndices[0];
    lineVertexIndicesB[2] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[2] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[2] < attribDef->m_numVertices);
    lineSampleIndices = &attribDef->m_lineSampleIndices[lineIndices[3] * attribDef->m_numSubSamplesPerLine];
    lineVertexIndicesA[3] = lineSampleIndices[0];
    lineVertexIndicesB[3] = lineSampleIndices[lastSubSample];
    NMP_ASSERT(lineVertexIndicesA[3] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndicesB[3] < attribDef->m_numVertices);

    // Recover the sample data
    NMP::vpu::vector4_t sampleA = NMP::vpu::set4f(
      attribDef->m_samples[lineVertexIndicesA[0]],
      attribDef->m_samples[lineVertexIndicesA[1]],
      attribDef->m_samples[lineVertexIndicesA[2]],
      attribDef->m_samples[lineVertexIndicesA[3]]);

    NMP::vpu::vector4_t sampleB = NMP::vpu::set4f(
      attribDef->m_samples[lineVertexIndicesB[0]],
      attribDef->m_samples[lineVertexIndicesB[1]],
      attribDef->m_samples[lineVertexIndicesB[2]],
      attribDef->m_samples[lineVertexIndicesB[3]]);

    // Compute the weight
    NMP::vpu::vector4_t AP = NMP::vpu::sub4f(sampleP, sampleA);
    NMP::vpu::vector4_t AB = NMP::vpu::sub4f(sampleB, sampleA);
    NMP::vpu::vector4_t recipAB = NMP::vpu::rcp4f(AB); // We make sure this is not singular in the asset compiler
    NMP::vpu::vector4_t weights = NMP::vpu::mul4f(AP, recipAB);

    // Check if the input control lies within the line (avoiding conditional branches)
    NMP::vpu::vector4_t maskA = NMP::vpu::mask4cmpLT(weights, NMP::vpu::zero4f()); // weight < 0
    NMP::vpu::vector4_t maskB = NMP::vpu::mask4cmpLT(NMP::vpu::neg4f(weights), NMP::vpu::negOne4f()); // -weight < -1
    NMP::vpu::vector4_t mask = NMP::vpu::not4(NMP::vpu::or4(maskA, maskB)); // weight >= 0 && weight <= 1

    // Update the result based on the condition mask
    resultFlags = NMP::vpu::sel4cmpMask(mask, mask, resultFlags);
    resultLineIndices = NMP::vpu::sel4cmpMask(mask, NMP::vpu::load4f((float*)lineIndices), resultLineIndices);
  }

  //------------------------
  // Recover the result
  NMP::Vector3 childNodeFlagsVec;
  uint32_t* childNodeFlags = (uint32_t*)&childNodeFlagsVec;
  NMP::vpu::store4f((float*)childNodeFlags, resultFlags);

  for (uint16_t i = 0; i < 4; ++i)
  {
    if (childNodeFlags[i])
    {
      const MR::ScatterBlend1DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
        MR::ScatterBlend1DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numSubDivisions);

      NMP::Vector3 lineIndicesVec;
      uint32_t* lineIndices = (uint32_t*)&lineIndicesVec;
      NMP::vpu::store4f((float*)lineIndices, resultLineIndices);
      const uint16_t* lineSampleIndices = attribDef->getLineSampleIndices(lineIndices[i]);

      uint32_t vertexSourceIndexA = attribDef->m_vertexSourceIndices[lineSampleIndices[0]];
      uint32_t vertexSourceIndexB = attribDef->m_vertexSourceIndices[lineSampleIndices[lastSubSample]];
      result.m_childNodeIDs[0] = nodeDef->getChildNodeID(vertexSourceIndexA);
      result.m_childNodeIDs[1] = nodeDef->getChildNodeID(vertexSourceIndexB);

      //------------------------
      // Check if the input control lies within the sub-division lines. Note since
      // there are very few sub-division lines then it is best to use scalar code for
      // this search.
      float weight = 0.0f;
      for (uint16_t k = 0; k < lastSubSample; ++k)
      {
        float sampleA = attribDef->m_samples[lineSampleIndices[k]];
        float sampleB = attribDef->m_samples[lineSampleIndices[k + 1]];
        float subWeight = (inputMotionParameter - sampleA) / (sampleB - sampleA);
        if (subWeight >= 0.0f && subWeight <= 1.0f)
        {
          // Recover the Barycentric weights assigned to the vertices of the sub-division lines.
          float weightA = subDivisionDataBlock.m_subSampleWeights[1][k];
          float weightB = subDivisionDataBlock.m_subSampleWeights[1][k + 1];

          // Interpolate the Barycentric vertex weights (Second components)
          weight = (1.0f - subWeight) * weightA + subWeight * weightB;
          break;
        }
      }

      // Set the weights
      result.m_childNodeWeights[0] = 1.0f - weight;
      result.m_childNodeWeights[1] = weight;
      result.m_motionParameter = inputMotionParameter;
      return true;
    }
  }

  return false;
}

#else
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend1DComputeLineBlendWeightsForInputControlN(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend1DDef* attribDef,
  float inputMotionParameter,
  ScatterBlend1DParameteriserInfo& result)
{
  NMP_ASSERT(attribDef);

  // Note that the number of vertices can be greater than the number of child sources
  // if wrapping samples have been added to the sample data
  const MR::ScatterBlend1DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend1DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numSubDivisions);

  for (uint16_t lineIndex = 0; lineIndex < attribDef->m_numLines; ++lineIndex)
  {
    // Recover the vertex sample indices for the line
    const uint16_t* lineSampleIndices = attribDef->getLineSampleIndices(lineIndex);
    NMP_ASSERT(lineSampleIndices);
    uint16_t lineVertexIndices[2];
    lineVertexIndices[0] = lineSampleIndices[0];
    lineVertexIndices[1] = lineSampleIndices[attribDef->m_numSubSamplesPerLine - 1];

    // Recover the sample data
    NMP_ASSERT(lineVertexIndices[0] < attribDef->m_numVertices);
    NMP_ASSERT(lineVertexIndices[1] < attribDef->m_numVertices);
    float sampleA = attribDef->m_samples[lineVertexIndices[0]];
    float sampleB = attribDef->m_samples[lineVertexIndices[1]];

    // Check if the input control lies within the line
    float weight = (inputMotionParameter - sampleA) / (sampleB - sampleA);
    if (weight >= 0.0f && weight <= 1.0f)
    {
      // Check if the input control lies within the sub-division lines
      for (uint16_t i = 0; i < attribDef->m_numSubSamplesPerLine - 1; ++i)
      {
        sampleA = attribDef->m_samples[lineSampleIndices[i]];
        sampleB = attribDef->m_samples[lineSampleIndices[i + 1]];
        float subWeight = (inputMotionParameter - sampleA) / (sampleB - sampleA);
        if (subWeight >= 0.0f && subWeight <= 1.0f)
        {
          // Recover the Barycentric weights assigned to the vertices of the sub-division lines.
          float weightA = subDivisionDataBlock.m_subSampleWeights[1][i];
          float weightB = subDivisionDataBlock.m_subSampleWeights[1][i + 1];

          // Interpolate the Barycentric vertex weights (Second components)
          weight = (1.0f - subWeight) * weightA + subWeight * weightB;
          break;
        }
      }

      uint32_t vertexSourceIndexA = attribDef->m_vertexSourceIndices[lineVertexIndices[0]];
      uint32_t vertexSourceIndexB = attribDef->m_vertexSourceIndices[lineVertexIndices[1]];
      result.m_childNodeIDs[0] = nodeDef->getChildNodeID(vertexSourceIndexA);
      result.m_childNodeIDs[1] = nodeDef->getChildNodeID(vertexSourceIndexB);
      result.m_childNodeWeights[0] = 1.0f - weight; // Recover first component
      result.m_childNodeWeights[1] = weight;
      result.m_motionParameter = inputMotionParameter;
      return true;
    }
  }

  return false;
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend1DAnnotationProjection(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend1DDef* attribDef,
  float inputMotionParameter,
  ScatterBlend1DParameteriserInfo& result)
{
  const uint16_t* lineSampleIndicesA = attribDef->getLineSampleIndices(0);
  NMP_ASSERT(lineSampleIndicesA);
  uint16_t lineVertexIndicesA[2];
  lineVertexIndicesA[0] = lineSampleIndicesA[0];
  lineVertexIndicesA[1] = lineSampleIndicesA[attribDef->m_numSubSamplesPerLine - 1];
  NMP_ASSERT(lineVertexIndicesA[0] < attribDef->m_numVertices);
  NMP_ASSERT(lineVertexIndicesA[1] < attribDef->m_numVertices);
  float sampleA = attribDef->m_samples[lineVertexIndicesA[0]];
  float dA = inputMotionParameter - sampleA;

  const uint16_t* lineSampleIndicesB = attribDef->getLineSampleIndices(attribDef->m_numLines - 1);
  NMP_ASSERT(lineSampleIndicesB);
  uint16_t lineVertexIndicesB[2];
  lineVertexIndicesB[0] = lineSampleIndicesB[0];
  lineVertexIndicesB[1] = lineSampleIndicesB[attribDef->m_numSubSamplesPerLine - 1];
  NMP_ASSERT(lineVertexIndicesB[0] < attribDef->m_numVertices);
  NMP_ASSERT(lineVertexIndicesB[1] < attribDef->m_numVertices);
  float sampleB = attribDef->m_samples[lineVertexIndicesB[1]];
  float dB = inputMotionParameter - sampleB;

  if (NMP::nmfabs(dA) < NMP::nmfabs(dB))
  {
    uint32_t vertexSourceIndexA = attribDef->m_vertexSourceIndices[lineVertexIndicesA[0]];
    uint32_t vertexSourceIndexB = attribDef->m_vertexSourceIndices[lineVertexIndicesA[1]];
    result.m_childNodeIDs[0] = nodeDef->getChildNodeID(vertexSourceIndexA);
    result.m_childNodeIDs[1] = nodeDef->getChildNodeID(vertexSourceIndexB);
    result.m_childNodeWeights[0] = 1.0f;
    result.m_childNodeWeights[1] = 0.0f;
    result.m_motionParameter = sampleA;
  }
  else
  {
    uint32_t vertexSourceIndexA = attribDef->m_vertexSourceIndices[lineVertexIndicesB[0]];
    uint32_t vertexSourceIndexB = attribDef->m_vertexSourceIndices[lineVertexIndicesB[1]];
    result.m_childNodeIDs[0] = nodeDef->getChildNodeID(vertexSourceIndexA);
    result.m_childNodeIDs[1] = nodeDef->getChildNodeID(vertexSourceIndexB);
    result.m_childNodeWeights[0] = 0.0f;
    result.m_childNodeWeights[1] = 1.0f;
    result.m_motionParameter = sampleB;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void scatterBlend1DComputeChildNodeWeightsForInputControl(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend1DDef* attribDef,
  float inputMotionParameter,
  ScatterBlend1DParameteriserInfo& result)
{
  //------------------------
  // Determine if the input control lies within any of the lines then
  // compute the corresponding edge blend weights and child node IDs.
  if (attribDef->m_numSubDivisions > 0)
  {
    // Lines have sub-samples
    result.m_wasProjected = !scatterBlend1DComputeLineBlendWeightsForInputControlN(
      nodeDef,
      attribDef,
      inputMotionParameter,
      result);
  }
  else
  {
    // Lines do not have sub-samples
    result.m_wasProjected = !scatterBlend1DComputeLineBlendWeightsForInputControl0(
      nodeDef,
      attribDef,
      inputMotionParameter,
      result);
  }

  //------------------------
  // Projection
  if (result.m_wasProjected)
  {
    scatterBlend1DAnnotationProjection(
      nodeDef,
      attribDef,
      inputMotionParameter,
      result);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
