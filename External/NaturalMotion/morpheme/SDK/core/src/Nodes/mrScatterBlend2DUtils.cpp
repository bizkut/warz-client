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
#include "morpheme/Nodes/mrScatterBlend2DUtils.h"
#include "morpheme/Nodes/mrScatterBlendProjectionUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// MR::ScatterBlend2DSubSampleManager
//----------------------------------------------------------------------------------------------------------------------
// 0 Sub-divisions (1 triangle, 3 sub-samples)
static const float g_subSampleWeightsA0[3] = {1.0f, 0.0f, 0.0f};
static const float g_subSampleWeightsB0[3] = {0.0f, 1.0f, 0.0f};
static const float g_subSampleWeightsC0[3] = {0.0f, 0.0f, 1.0f};
// Sub-division triangles
static const uint16_t g_subDivisionTriangleIndicesA0[1] = {0};
static const uint16_t g_subDivisionTriangleIndicesB0[1] = {1};
static const uint16_t g_subDivisionTriangleIndicesC0[1] = {2};
// Facet A
static const uint16_t g_subDivisionEdgeFacetAIndicesA0[1] = {1};
static const uint16_t g_subDivisionEdgeFacetAIndicesB0[1] = {2};
// Facet B
static const uint16_t g_subDivisionEdgeFacetBIndicesA0[1] = {2};
static const uint16_t g_subDivisionEdgeFacetBIndicesB0[1] = {0};
// Facet C
static const uint16_t g_subDivisionEdgeFacetCIndicesA0[1] = {0};
static const uint16_t g_subDivisionEdgeFacetCIndicesB0[1] = {1};
// Edge facet vertex sub-sample indices
static const uint16_t g_vertexEdgeFacetAIndices0[2] = {1, 2};
static const uint16_t g_vertexEdgeFacetBIndices0[2] = {2, 0};
static const uint16_t g_vertexEdgeFacetCIndices0[2] = {0, 1};
// Vertex sub-sample indices
static const uint16_t g_vertexSubSampleIndices0[3] = {0, 1, 2};
// Triangle sub-sample permutation map
static const uint16_t g_triangleSubSamplePermutationMapABC0[3] = {0, 1, 2};
static const uint16_t g_triangleSubSamplePermutationMapACB0[3] = {0, 2, 1};
static const uint16_t g_triangleSubSamplePermutationMapBAC0[3] = {1, 0, 2};
static const uint16_t g_triangleSubSamplePermutationMapBCA0[3] = {1, 2, 0};
static const uint16_t g_triangleSubSamplePermutationMapCAB0[3] = {2, 0, 1};
static const uint16_t g_triangleSubSamplePermutationMapCBA0[3] = {2, 1, 0};

//----------------------------------------------------------------------------------------------------------------------
// 1 Sub-divisions (3 triangles, 6 sub-samples)
static const float g_subSampleWeightsA1[6] = {1.0f, 0.5f, 0.0f, 0.5f, 0.0f, 0.0f};
static const float g_subSampleWeightsB1[6] = {0.0f, 0.5f, 1.0f, 0.0f, 0.5f, 0.0f};
static const float g_subSampleWeightsC1[6] = {0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f};
// Sub-division triangles
static const uint16_t g_subDivisionTriangleIndicesA1[4] = {0, 2, 5, 1};
static const uint16_t g_subDivisionTriangleIndicesB1[4] = {1, 4, 3, 4};
static const uint16_t g_subDivisionTriangleIndicesC1[4] = {3, 1, 4, 3};
// Facet A
static const uint16_t g_subDivisionEdgeFacetAIndicesA1[2] = {2, 4};
static const uint16_t g_subDivisionEdgeFacetAIndicesB1[2] = {4, 5};
// Facet B
static const uint16_t g_subDivisionEdgeFacetBIndicesA1[2] = {5, 3};
static const uint16_t g_subDivisionEdgeFacetBIndicesB1[2] = {3, 0};
// Facet C
static const uint16_t g_subDivisionEdgeFacetCIndicesA1[2] = {0, 1};
static const uint16_t g_subDivisionEdgeFacetCIndicesB1[2] = {1, 2};
// Edge facet vertex sub-sample indices
static const uint16_t g_vertexEdgeFacetAIndices1[2] = {2, 5};
static const uint16_t g_vertexEdgeFacetBIndices1[2] = {5, 0};
static const uint16_t g_vertexEdgeFacetCIndices1[2] = {0, 2};
// Sub-sample edge facet indices
static const uint16_t g_subSampleEdgeFacetAIndices1[3] = {2, 4, 5};
static const uint16_t g_subSampleEdgeFacetBIndices1[3] = {5, 3, 0};
static const uint16_t g_subSampleEdgeFacetCIndices1[3] = {0, 1, 2};
// Vertex sub-sample indices
static const uint16_t g_vertexSubSampleIndices1[3] = {0, 2, 5};
// Interior edge sub-sample indices
static const uint16_t g_interiorEdgeSubSampleIndicesBC1[1] = {4};
static const uint16_t g_interiorEdgeSubSampleIndicesCA1[1] = {3};
static const uint16_t g_interiorEdgeSubSampleIndicesAB1[1] = {1};
// Interior edge sub-sample weights
static const float g_interiorEdgeSubSampleWeights1[1] = {0.5f};
// Interior edge sub-sample permutation map
static const uint16_t g_interiorEdgeSubSamplePermutationMap1[1] = {0};
// Triangle sub-sample permutation map
static const uint16_t g_triangleSubSamplePermutationMapABC1[6] = {0, 1, 2, 3, 4, 5};
static const uint16_t g_triangleSubSamplePermutationMapACB1[6] = {0, 3, 5, 1, 4, 2};
static const uint16_t g_triangleSubSamplePermutationMapBAC1[6] = {2, 1, 0, 4, 3, 5};
static const uint16_t g_triangleSubSamplePermutationMapBCA1[6] = {2, 4, 5, 1, 3, 0};
static const uint16_t g_triangleSubSamplePermutationMapCAB1[6] = {5, 3, 0, 4, 1, 2};
static const uint16_t g_triangleSubSamplePermutationMapCBA1[6] = {5, 4, 2, 3, 1, 0};

//----------------------------------------------------------------------------------------------------------------------
// 2 Sub-divisions (16 triangles, 15 sub-samples)
static const float g_subSampleWeightsA2[15] = {1.0f, 0.75f, 0.5f, 0.25f, 0.0f, 0.75f, 0.5f, 0.25f, 0.0f, 0.5f, 0.25f, 0.0f, 0.25f, 0.0f, 0.0f};
static const float g_subSampleWeightsB2[15] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 0.0f, 0.25f, 0.5f, 0.75f, 0.0f, 0.25f, 0.5f, 0.0f, 0.25f, 0.0f};
static const float g_subSampleWeightsC2[15] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.25f, 0.25f, 0.25f, 0.5f, 0.5f, 0.5f, 0.75f, 0.75f, 1.0f};
// Sub-division triangles
static const uint16_t g_subDivisionTriangleIndicesA2[16] = {0, 2, 9, 1, 4, 11, 2, 8, 14, 9, 11, 12, 2, 11, 9, 7};
static const uint16_t g_subDivisionTriangleIndicesB2[16] = {1, 6, 5, 6, 8, 7, 3, 7, 12, 10, 13, 10, 7, 10, 6, 10};
static const uint16_t g_subDivisionTriangleIndicesC2[16] = {5, 1, 6, 5, 3, 8, 7, 3, 13, 12, 10, 13, 6, 7, 10, 6};
// Facet A
static const uint16_t g_subDivisionEdgeFacetAIndicesA2[4] = {4, 8, 11, 13};
static const uint16_t g_subDivisionEdgeFacetAIndicesB2[4] = {8, 11, 13, 14};
// Facet B
static const uint16_t g_subDivisionEdgeFacetBIndicesA2[4] = {14, 12, 9, 5};
static const uint16_t g_subDivisionEdgeFacetBIndicesB2[4] = {12, 9, 5, 0};
// Facet C
static const uint16_t g_subDivisionEdgeFacetCIndicesA2[4] = {0, 1, 2, 3};
static const uint16_t g_subDivisionEdgeFacetCIndicesB2[4] = {1, 2, 3, 4};
// Edge facet vertex sub-sample indices
static const uint16_t g_vertexEdgeFacetAIndices2[2] = {4, 14};
static const uint16_t g_vertexEdgeFacetBIndices2[2] = {14, 0};
static const uint16_t g_vertexEdgeFacetCIndices2[2] = {0, 4};
// Sub-sample edge facet indices
static const uint16_t g_subSampleEdgeFacetAIndices2[5] = {4, 8, 11, 13, 14};
static const uint16_t g_subSampleEdgeFacetBIndices2[5] = {14, 12, 9, 5, 0};
static const uint16_t g_subSampleEdgeFacetCIndices2[5] = {0, 1, 2, 3, 4};
// Vertex sub-sample indices
static const uint16_t g_vertexSubSampleIndices2[3] = {0, 4, 14};
// Interior edge sub-sample indices
static const uint16_t g_interiorEdgeSubSampleIndicesBC2[3] = {8, 11, 13};
static const uint16_t g_interiorEdgeSubSampleIndicesCA2[3] = {12, 9, 5};
static const uint16_t g_interiorEdgeSubSampleIndicesAB2[3] = {1, 2, 3};
// Interior edge sub-sample weights
static const float g_interiorEdgeSubSampleWeightsA2[3] = {0.75f, 0.5f, 0.25f};
static const float g_interiorEdgeSubSampleWeightsB2[3] = {0.25f, 0.5f, 0.75f};
// Interior edge sub-sample permutation map
static const uint16_t g_interiorEdgeSubSamplePermutationMapAB2[3] = {0, 1, 2};
static const uint16_t g_interiorEdgeSubSamplePermutationMapBA2[3] = {2, 1, 0};
// Interior triangle sub-sample indices
static const uint16_t g_interiorTriangleSubSampleIndices2[3] = {6, 7, 10};
// Triangle sub-sample permutation map
static const uint16_t g_triangleSubSamplePermutationMapABC2[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
static const uint16_t g_triangleSubSamplePermutationMapACB2[15] = {0, 5, 9, 12, 14, 1, 6, 10, 13, 2, 7, 11, 3, 8, 4};
static const uint16_t g_triangleSubSamplePermutationMapBAC2[15] = {4, 3, 2, 1, 0, 8, 7, 6, 5, 11, 10, 9, 13, 12, 14};
static const uint16_t g_triangleSubSamplePermutationMapBCA2[15] = {4, 8, 11, 13, 14, 3, 7, 10, 12, 2, 6, 9, 1, 5, 0};
static const uint16_t g_triangleSubSamplePermutationMapCAB2[15] = {14, 12, 9, 5, 0, 13, 10, 6, 1, 11, 7, 2, 8, 3, 4};
static const uint16_t g_triangleSubSamplePermutationMapCBA2[15] = {14, 13, 11, 8, 4, 12, 10, 7, 3, 9, 6, 2, 5, 1, 0};

//----------------------------------------------------------------------------------------------------------------------
const ScatterBlend2DSubSampleManager::SubDivisionDataBlock ScatterBlend2DSubSampleManager::sm_subDivisionDataBlocks[3] =
{
  //------------------------
  // 0 Sub-divisions
  {
    3, // m_numSubSamplesPerTriangle
    2, // m_numSubSamplesPerEdgeFacet
    1, // m_numSubSimplexesPerTriangle
    1, // m_numSubSimplexesPerEdgeFacet
    0, // m_numInteriorSubSamplesPerEdge
    0, // m_numInteriorSubSamplesPerTriangle
    {g_subSampleWeightsA0, g_subSampleWeightsB0, g_subSampleWeightsC0},
    {g_subDivisionTriangleIndicesA0, g_subDivisionTriangleIndicesB0, g_subDivisionTriangleIndicesC0},
    {
      {g_subDivisionEdgeFacetAIndicesA0, g_subDivisionEdgeFacetAIndicesB0},
      {g_subDivisionEdgeFacetBIndicesA0, g_subDivisionEdgeFacetBIndicesB0},
      {g_subDivisionEdgeFacetCIndicesA0, g_subDivisionEdgeFacetCIndicesB0}
    },
    {g_vertexEdgeFacetAIndices0, g_vertexEdgeFacetBIndices0, g_vertexEdgeFacetCIndices0},
    {g_vertexEdgeFacetAIndices0, g_vertexEdgeFacetBIndices0, g_vertexEdgeFacetCIndices0},
    g_vertexSubSampleIndices0,
    {NULL, NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    NULL,
    {
      g_triangleSubSamplePermutationMapABC0,
      g_triangleSubSamplePermutationMapACB0,
      g_triangleSubSamplePermutationMapBAC0,
      g_triangleSubSamplePermutationMapBCA0,
      g_triangleSubSamplePermutationMapCAB0,
      g_triangleSubSamplePermutationMapCBA0
    }
  },

  //------------------------
  // 1 Sub-divisions
  {
    6, // m_numSubSamplesPerTriangle
    3, // m_numSubSamplesPerEdgeFacet
    4, // m_numSubSimplexesPerTriangle
    2, // m_numSubSimplexesPerEdgeFacet
    1, // m_numInteriorSubSamplesPerEdge
    0, // m_numInteriorSubSamplesPerTriangle
    {g_subSampleWeightsA1, g_subSampleWeightsB1, g_subSampleWeightsC1},
    {g_subDivisionTriangleIndicesA1, g_subDivisionTriangleIndicesB1, g_subDivisionTriangleIndicesC1},
    {
      {g_subDivisionEdgeFacetAIndicesA1, g_subDivisionEdgeFacetAIndicesB1},
      {g_subDivisionEdgeFacetBIndicesA1, g_subDivisionEdgeFacetBIndicesB1},
      {g_subDivisionEdgeFacetCIndicesA1, g_subDivisionEdgeFacetCIndicesB1}
    },
    {g_vertexEdgeFacetAIndices1, g_vertexEdgeFacetBIndices1, g_vertexEdgeFacetCIndices1},
    {g_subSampleEdgeFacetAIndices1, g_subSampleEdgeFacetBIndices1, g_subSampleEdgeFacetCIndices1},
    g_vertexSubSampleIndices1,
    {g_interiorEdgeSubSampleIndicesBC1, g_interiorEdgeSubSampleIndicesCA1, g_interiorEdgeSubSampleIndicesAB1},
    {g_interiorEdgeSubSampleWeights1, g_interiorEdgeSubSampleWeights1},
    {g_interiorEdgeSubSamplePermutationMap1, g_interiorEdgeSubSamplePermutationMap1},
    NULL,
    {
      g_triangleSubSamplePermutationMapABC1,
      g_triangleSubSamplePermutationMapACB1,
      g_triangleSubSamplePermutationMapBAC1,
      g_triangleSubSamplePermutationMapBCA1,
      g_triangleSubSamplePermutationMapCAB1,
      g_triangleSubSamplePermutationMapCBA1
    }
  },

  //------------------------
  // 2 Sub-divisions
  {
    15, // m_numSubSamplesPerTriangle
    5, // m_numSubSamplesPerEdgeFacet
    16, // m_numSubSimplexesPerTriangle
    4, // m_numSubSimplexesPerEdgeFacet
    3, // m_numInteriorSubSamplesPerEdge
    3, // m_numInteriorSubSamplesPerTriangle
    {g_subSampleWeightsA2, g_subSampleWeightsB2, g_subSampleWeightsC2},
    {g_subDivisionTriangleIndicesA2, g_subDivisionTriangleIndicesB2, g_subDivisionTriangleIndicesC2},
    {
      {g_subDivisionEdgeFacetAIndicesA2, g_subDivisionEdgeFacetAIndicesB2},
      {g_subDivisionEdgeFacetBIndicesA2, g_subDivisionEdgeFacetBIndicesB2},
      {g_subDivisionEdgeFacetCIndicesA2, g_subDivisionEdgeFacetCIndicesB2}
    },
    {g_vertexEdgeFacetAIndices2, g_vertexEdgeFacetBIndices2, g_vertexEdgeFacetCIndices2},
    {g_subSampleEdgeFacetAIndices2, g_subSampleEdgeFacetBIndices2, g_subSampleEdgeFacetCIndices2},
    g_vertexSubSampleIndices2,
    {g_interiorEdgeSubSampleIndicesBC2, g_interiorEdgeSubSampleIndicesCA2, g_interiorEdgeSubSampleIndicesAB2},
    {g_interiorEdgeSubSampleWeightsA2, g_interiorEdgeSubSampleWeightsB2},
    {g_interiorEdgeSubSamplePermutationMapAB2, g_interiorEdgeSubSamplePermutationMapBA2},
    g_interiorTriangleSubSampleIndices2,
    {
      g_triangleSubSamplePermutationMapABC2,
      g_triangleSubSamplePermutationMapACB2,
      g_triangleSubSamplePermutationMapBAC2,
      g_triangleSubSamplePermutationMapBCA2,
      g_triangleSubSamplePermutationMapCAB2,
      g_triangleSubSamplePermutationMapCBA2
    }
  }
};

//----------------------------------------------------------------------------------------------------------------------
uint32_t ScatterBlend2DSubSampleManager::getInteriorEdgeVertexOrderPermutation(
  uint32_t srcVertexAID,
  uint32_t srcVertexBID,
  uint32_t destVertexAID,
  uint32_t destVertexBID)
{
  // Permutation map
  const uint32_t vertexOrder[2][2] =
  {
    // AB
    {destVertexAID, destVertexBID},
    // BA
    {destVertexBID, destVertexAID}
  };

  // Find which permutation of the source vertices matches the destination
  uint32_t permutationIndex;
  for (permutationIndex = 0; permutationIndex < 2; ++permutationIndex)
  {
    const uint32_t* data = vertexOrder[permutationIndex];
    if (srcVertexAID == data[0] && srcVertexBID == data[1])
      break;
  }
  NMP_ASSERT(permutationIndex < 2); // There must be a match

  return permutationIndex;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t ScatterBlend2DSubSampleManager::getTriangleVertexOrderPermutation(
  uint32_t srcVertexAID,
  uint32_t srcVertexBID,
  uint32_t srcVertexCID,
  uint32_t destVertexAID,
  uint32_t destVertexBID,
  uint32_t destVertexCID)
{
  // Permutation map
  const uint32_t vertexOrder[6][3] =
  {
    // ABC
    {destVertexAID, destVertexBID, destVertexCID},
    // ACB
    {destVertexAID, destVertexCID, destVertexBID},
    // BAC
    {destVertexBID, destVertexAID, destVertexCID},
    // BCA
    {destVertexBID, destVertexCID, destVertexAID},
    // CAB
    {destVertexCID, destVertexAID, destVertexBID},
    // CBA
    {destVertexCID, destVertexBID, destVertexAID}
  };

  // Find which permutation of the source vertices matches the destination
  uint32_t permutationIndex;
  for (permutationIndex = 0; permutationIndex < 6; ++permutationIndex)
  {
    const uint32_t* data = vertexOrder[permutationIndex];
    if (srcVertexAID == data[0] && srcVertexBID == data[1] && srcVertexCID == data[2])
      break;
  }
  NMP_ASSERT(permutationIndex < 6); // There must be a match

  return permutationIndex;
}

//----------------------------------------------------------------------------------------------------------------------
// Projection functions when input control is outside the annotation
//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_PLATFORM_SIMD
// \brief Function adds entries to the match buffer with the minimum distance to each
// exterior edge facet sub-sample point.
//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionClosestApproachBuffer(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& sampleP,
  ScatterBlendProjectionClosestFacetSubSampleBuffer* matchBuffer)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Reset the match buffer
  NMP_ASSERT(matchBuffer);
  matchBuffer->reset();

  // Quantisation scale and offset
  NMP::vpu::vector4_t qScalesX = NMP::vpu::set4f(attribDef->m_qScale[0]);
  NMP::vpu::vector4_t qScalesY = NMP::vpu::set4f(attribDef->m_qScale[1]);
  NMP::vpu::vector4_t qOffsetsX = NMP::vpu::set4f(attribDef->m_qOffset[0]);
  NMP::vpu::vector4_t qOffsetsY = NMP::vpu::set4f(attribDef->m_qOffset[1]);

  // Multiplexed input sample
  NMP::vpu::vector4_t samplePX = NMP::vpu::set4f(sampleP.x);
  NMP::vpu::vector4_t samplePY = NMP::vpu::set4f(sampleP.y);

  uint32_t numSubSamplesPerEdgeFacet = subDivisionDataBlock.getNumSubSamplesPerEdgeFacet();

  //------------------------
  // Iterate over the input exterior edge facet data
  uint32_t numExteriorEdgeFacets = attribDef->m_numExteriorEdgeFacets;
  for (uint32_t extIndex = 0; extIndex < numExteriorEdgeFacets; extIndex += 4)
  {
    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices[4];
    triangleSampleIndices[0] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex]);
    triangleSampleIndices[1] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex + 1]);
    triangleSampleIndices[2] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex + 2]);
    triangleSampleIndices[3] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex + 3]);

    // Get the appropriate edge facet sub-sample table from the manager   
    const uint16_t* subSampleEdgeFacetIndices[4];
    subSampleEdgeFacetIndices[0] = subDivisionDataBlock.getSubSampleEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex]);
    subSampleEdgeFacetIndices[1] = subDivisionDataBlock.getSubSampleEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex + 1]);
    subSampleEdgeFacetIndices[2] = subDivisionDataBlock.getSubSampleEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex + 2]);
    subSampleEdgeFacetIndices[3] = subDivisionDataBlock.getSubSampleEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex + 3]);

    //------------------------
    // Compute the minimum squared distance from the input point to the edge facet sub-sample points
    NMP::vpu::vector4_t triangleSubSampleIndexMin = NMP::vpu::zero4f();
    NMP::vpu::vector4_t distSqMin = NMP::vpu::set4f(FLT_MAX);
    for (uint32_t subSampleIndex = 0; subSampleIndex < numSubSamplesPerEdgeFacet; ++subSampleIndex)
    {
      NMP::Vector3 triangleSubSampleIndicesVec;
      uint32_t* triangleSubSampleIndices = (uint32_t*)&triangleSubSampleIndicesVec;
      uint32_t sampleIndex;

      //------------------------
      // Get the sample index in the global list
      triangleSubSampleIndices[0] = subSampleEdgeFacetIndices[0][subSampleIndex];
      sampleIndex = triangleSampleIndices[0][triangleSubSampleIndices[0]];
      // Recover the quantised samples
      NMP::vpu::vector4_t qVal0 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndex],
        attribDef->m_samples[1][sampleIndex],
        0,
        0);

      //------------------------
      // Get the sample index in the global list
      triangleSubSampleIndices[1] = subSampleEdgeFacetIndices[1][subSampleIndex];
      sampleIndex = triangleSampleIndices[1][triangleSubSampleIndices[1]];
      // Recover the quantised samples
      NMP::vpu::vector4_t qVal1 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndex],
        attribDef->m_samples[1][sampleIndex],
        0,
        0);

      //------------------------
      // Get the sample index in the global list
      triangleSubSampleIndices[2] = subSampleEdgeFacetIndices[2][subSampleIndex];
      sampleIndex = triangleSampleIndices[2][triangleSubSampleIndices[2]];
      // Recover the quantised samples
      NMP::vpu::vector4_t qVal2 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndex],
        attribDef->m_samples[1][sampleIndex],
        0,
        0);

      //------------------------
      // Get the sample index in the global list
      triangleSubSampleIndices[3] = subSampleEdgeFacetIndices[3][subSampleIndex];
      sampleIndex = triangleSampleIndices[3][triangleSubSampleIndices[3]];
      // Recover the quantised samples
      NMP::vpu::vector4_t qVal3 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndex],
        attribDef->m_samples[1][sampleIndex],
        0,
        0);

      //------------------------
      // Multiplex and dequantise the sample data
      NMP::vpu::Vector3MP sampleMP;
      sampleMP.pack(qVal0, qVal1, qVal2, qVal3);
      sampleMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleMP.x), qScalesX, qOffsetsX);
      sampleMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleMP.y), qScalesY, qOffsetsY);

      // Compute the squared distance
      NMP::vpu::vector4_t VX = NMP::vpu::sub4f(samplePX, sampleMP.x);
      NMP::vpu::vector4_t VY = NMP::vpu::sub4f(samplePY, sampleMP.y);
      NMP::vpu::vector4_t distSq = NMP::vpu::add4f(NMP::vpu::mul4f(VX, VX), NMP::vpu::mul4f(VY, VY));
      NMP::vpu::vector4_t maskDistSq = NMP::vpu::mask4cmpLT(distSq, distSqMin); // distSq < distSqMin

      // Update the state
      triangleSubSampleIndexMin = NMP::vpu::sel4cmpMask(maskDistSq, NMP::vpu::load4f((float*)triangleSubSampleIndices), triangleSubSampleIndexMin);
      distSqMin = NMP::vpu::sel4cmpMask(maskDistSq, distSq, distSqMin);
    }

    //------------------------
    // Store the multiplexed data
    NMP::Vector3 triangleSubSampleIndicesVec;
    uint32_t* triangleSubSampleIndices = (uint32_t*)&triangleSubSampleIndicesVec;
    NMP::vpu::store4f((float*)triangleSubSampleIndices, triangleSubSampleIndexMin);
    NMP::Vector3 distSqVec;
    float* distVals = (float*)&distSqVec;
    NMP::vpu::store4f(distVals, distSqMin);

    //------------------------
    // Add the entries to the buffer
    uint32_t numEntriesRem = numExteriorEdgeFacets - extIndex;
    uint32_t blockSize = NMP::minimum((uint32_t)4, numEntriesRem);
    for (uint32_t i = 0; i < blockSize; ++i)
    {
      matchBuffer->addBufferEntry(
        (uint16_t)(extIndex + i),
        (uint16_t)triangleSubSampleIndices[i],
        distVals[i]);
    }
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionClosestApproachBuffer(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& sampleP,
  ScatterBlendProjectionClosestFacetSubSampleBuffer* matchBuffer)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Reset the match buffer
  NMP_ASSERT(matchBuffer);
  matchBuffer->reset();

  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);
  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  uint16_t numSubSamplesPerEdgeFacet = subDivisionDataBlock.getNumSubSamplesPerEdgeFacet();

  //------------------------
  // Compute the closest matches from the input point to the exterior edge facet sub-samples
  for (uint16_t extIndex = 0; extIndex < attribDef->m_numExteriorEdgeFacets; ++extIndex)
  {
    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex]);

    // Get the appropriate edge facet sub-sample table from the manager
    const uint16_t* subSampleEdgeFacetIndices = subDivisionDataBlock.getSubSampleEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex]);

    // Compute the minimum squared distance from the input point to the edge facet sub-sample points
    uint16_t triangleSubSampleIndexMin = 0xFFFF;
    float distSqMin = FLT_MAX;
    for (uint16_t subSampleIndex = 0; subSampleIndex < numSubSamplesPerEdgeFacet; ++subSampleIndex)
    {
      // Get the sample index in the global list
      uint16_t triangleSubSampleIndex = subSampleEdgeFacetIndices[subSampleIndex];
      uint16_t sampleIndex = triangleSampleIndices[triangleSubSampleIndex];

      // Recover the edge facet sub-sample
      NMP::Vector3 sampleX(
        (float)attribDef->m_samples[0][sampleIndex] * qScale.x + qOffset.x,
        (float)attribDef->m_samples[1][sampleIndex] * qScale.y + qOffset.y,
        0.0f);

      // Compute the squared distance
      NMP::Vector3 v = sampleP - sampleX;
      float distSq = v.magnitudeSquared();
      if (distSq < distSqMin)
      {
        triangleSubSampleIndexMin = triangleSubSampleIndex;
        distSqMin = distSq;
      }
    }
    NMP_ASSERT(triangleSubSampleIndexMin != 0xFFFF);
    matchBuffer->addBufferEntry(
      extIndex,
      triangleSubSampleIndexMin,
      distSqMin);
  }
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
// \brief Function iterates through the N sorted entries of the match buffer and projects the
// input control point onto the sub-division edge facet surface and edges. It updates the
// minimum distance, position and Barycentric weights accordingly.
//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionClosestApproachUpdate(
  const AttribDataScatterBlend2DDef* attribDef,
  const ScatterBlendProjectionClosestFacetSubSampleBuffer* matchBuffer,
  const NMP::Vector3& sampleP,
  const uint16_t numSortedEntries,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  NMP_ASSERT(matchBuffer);
  const ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry* matchEntry = matchBuffer->getSortedEntry(0);
  float distanceSqMin = matchEntry->m_distanceSq;
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[matchEntry->m_extIndex];

  // Recover the sub-sample point
  const uint16_t* triangleSampleIndices0 = attribDef->getTriangleSampleIndices(
    exteriorTriangleIndexMin);
  uint16_t sampleIndex0 = triangleSampleIndices0[matchEntry->m_subDivIndex];
  XMin.set(
    (float)attribDef->m_samples[0][sampleIndex0] * attribDef->m_qScale[0] + attribDef->m_qOffset[0],
    (float)attribDef->m_samples[1][sampleIndex0] * attribDef->m_qScale[1] + attribDef->m_qOffset[1],
    0.0f);

  // Recover the triangle Barycentric weights
  subDivisionDataBlock.getTriangleSubSampleWeights(
    matchEntry->m_subDivIndex,
    weightsMin.x,
    weightsMin.y,
    weightsMin.z);

  //------------------------
  // Quantisation scale and offset
  NMP::vpu::vector4_t qScalesX = NMP::vpu::set4f(attribDef->m_qScale[0]);
  NMP::vpu::vector4_t qScalesY = NMP::vpu::set4f(attribDef->m_qScale[1]);
  NMP::vpu::vector4_t qOffsetsX = NMP::vpu::set4f(attribDef->m_qOffset[0]);
  NMP::vpu::vector4_t qOffsetsY = NMP::vpu::set4f(attribDef->m_qOffset[1]);

  // Tolerances
  NMP::vpu::vector4_t minVals = NMP::vpu::set4f(-1e-3f);
  NMP::vpu::vector4_t maxVals = NMP::vpu::set4f(1.0f + 1e-3f);

  // Multiplexed input sample
  NMP::vpu::vector4_t samplePX = NMP::vpu::set4f(sampleP.x);
  NMP::vpu::vector4_t samplePY = NMP::vpu::set4f(sampleP.y);

  // Multiplexed result data
  NMP::vpu::vector4_t resultFlags = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultExtIndices = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultSubDivIndices = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultX = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultY = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinX = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinY = NMP::vpu::zero4f();
  NMP::vpu::vector4_t distSqMin = NMP::vpu::set4f(distanceSqMin);

  //------------------------
  // Iterate over the input exterior edge facet data
  uint32_t numSubSimplexesPerEdgeFacet = subDivisionDataBlock.m_numSubSimplexesPerEdgeFacet;
  for (uint32_t entryIndex = 0; entryIndex < numSortedEntries; entryIndex += 4)
  {
    NMP::Vector3 extIndicesVec;
    uint32_t* extIndices = (uint32_t*)&extIndicesVec;
    extIndices[0] = matchBuffer->getSortedEntry(entryIndex)->m_extIndex;
    extIndices[1] = matchBuffer->getSortedEntry(entryIndex + 1)->m_extIndex;
    extIndices[2] = matchBuffer->getSortedEntry(entryIndex + 2)->m_extIndex;
    extIndices[3] = matchBuffer->getSortedEntry(entryIndex + 3)->m_extIndex;

    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices[4];
    triangleSampleIndices[0] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndices[0]]);
    triangleSampleIndices[1] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndices[1]]);
    triangleSampleIndices[2] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndices[2]]);
    triangleSampleIndices[3] = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndices[3]]);

    // Get the appropriate sub-division edge facet table from the manager
    const uint16_t* subDivisionEdgeFacetIndicesA[4];
    subDivisionEdgeFacetIndicesA[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[0]], 0);
    subDivisionEdgeFacetIndicesA[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[1]], 0);
    subDivisionEdgeFacetIndicesA[2] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[2]], 0);
    subDivisionEdgeFacetIndicesA[3] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[3]], 0);

    const uint16_t* subDivisionEdgeFacetIndicesB[4];
    subDivisionEdgeFacetIndicesB[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[0]], 1);
    subDivisionEdgeFacetIndicesB[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[1]], 1);
    subDivisionEdgeFacetIndicesB[2] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[2]], 1);
    subDivisionEdgeFacetIndicesB[3] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndices[3]], 1);

    // Iterate over the exterior edge facet sub-divisions
    for (uint32_t subDivIndex = 0; subDivIndex < numSubSimplexesPerEdgeFacet; ++subDivIndex)
    {
      uint32_t triangleSubSampleIndicesA, triangleSubSampleIndicesB;
      uint32_t sampleIndicesA, sampleIndicesB;

      //------------------------
      // Recover the edge facet vertex indices (triangle local indices)      
      triangleSubSampleIndicesA = subDivisionEdgeFacetIndicesA[0][subDivIndex];
      triangleSubSampleIndicesB = subDivisionEdgeFacetIndicesB[0][subDivIndex];
      // Recover the corresponding sample indices within the global list
      sampleIndicesA = triangleSampleIndices[0][triangleSubSampleIndicesA];
      sampleIndicesB = triangleSampleIndices[0][triangleSubSampleIndicesB];
      // Recover the quantised samples
      NMP::vpu::vector4_t qValA0 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesA],
        attribDef->m_samples[1][sampleIndicesA],
        0,
        0);
      NMP::vpu::vector4_t qValB0 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesB],
        attribDef->m_samples[1][sampleIndicesB],
        0,
        0);

      //------------------------
      // Recover the edge facet vertex indices (triangle local indices)      
      triangleSubSampleIndicesA = subDivisionEdgeFacetIndicesA[1][subDivIndex];
      triangleSubSampleIndicesB = subDivisionEdgeFacetIndicesB[1][subDivIndex];
      // Recover the corresponding sample indices within the global list
      sampleIndicesA = triangleSampleIndices[1][triangleSubSampleIndicesA];
      sampleIndicesB = triangleSampleIndices[1][triangleSubSampleIndicesB];
      // Recover the quantised samples
      NMP::vpu::vector4_t qValA1 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesA],
        attribDef->m_samples[1][sampleIndicesA],
        0,
        0);
      NMP::vpu::vector4_t qValB1 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesB],
        attribDef->m_samples[1][sampleIndicesB],
        0,
        0);

      //------------------------
      // Recover the edge facet vertex indices (triangle local indices)      
      triangleSubSampleIndicesA = subDivisionEdgeFacetIndicesA[2][subDivIndex];
      triangleSubSampleIndicesB = subDivisionEdgeFacetIndicesB[2][subDivIndex];
      // Recover the corresponding sample indices within the global list
      sampleIndicesA = triangleSampleIndices[2][triangleSubSampleIndicesA];
      sampleIndicesB = triangleSampleIndices[2][triangleSubSampleIndicesB];
      // Recover the quantised samples
      NMP::vpu::vector4_t qValA2 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesA],
        attribDef->m_samples[1][sampleIndicesA],
        0,
        0);
      NMP::vpu::vector4_t qValB2 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesB],
        attribDef->m_samples[1][sampleIndicesB],
        0,
        0);

      //------------------------
      // Recover the edge facet vertex indices (triangle local indices)      
      triangleSubSampleIndicesA = subDivisionEdgeFacetIndicesA[3][subDivIndex];
      triangleSubSampleIndicesB = subDivisionEdgeFacetIndicesB[3][subDivIndex];
      // Recover the corresponding sample indices within the global list
      sampleIndicesA = triangleSampleIndices[3][triangleSubSampleIndicesA];
      sampleIndicesB = triangleSampleIndices[3][triangleSubSampleIndicesB];
      // Recover the quantised samples
      NMP::vpu::vector4_t qValA3 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesA],
        attribDef->m_samples[1][sampleIndicesA],
        0,
        0);
      NMP::vpu::vector4_t qValB3 = NMP::vpu::set4i(
        attribDef->m_samples[0][sampleIndicesB],
        attribDef->m_samples[1][sampleIndicesB],
        0,
        0);

      //------------------------
      // Multiplex and dequantise the sample data
      NMP::vpu::Vector3MP sampleAMP;
      sampleAMP.pack(qValA0, qValA1, qValA2, qValA3);
      sampleAMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.x), qScalesX, qOffsetsX);
      sampleAMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.y), qScalesY, qOffsetsY);

      NMP::vpu::Vector3MP sampleBMP;
      sampleBMP.pack(qValB0, qValB1, qValB2, qValB3);
      sampleBMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.x), qScalesX, qOffsetsX);
      sampleBMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.y), qScalesY, qOffsetsY);

      // Compute the facet edge vector AB
      NMP::vpu::vector4_t UX = NMP::vpu::sub4f(sampleBMP.x, sampleAMP.x);
      NMP::vpu::vector4_t UY = NMP::vpu::sub4f(sampleBMP.y, sampleAMP.y);

      // Compute the input point relative to vertex A
      NMP::vpu::vector4_t PX = NMP::vpu::sub4f(samplePX, sampleAMP.x);
      NMP::vpu::vector4_t PY = NMP::vpu::sub4f(samplePY, sampleAMP.y);

      //------------------------
      // Compute the projected point on the edge facet:
      // X = alpha * U, where  U = B - A, and
      // U^T * (P - X) = 0
      NMP::vpu::vector4_t M = NMP::vpu::add4f(NMP::vpu::mul4f(UX, UX), NMP::vpu::mul4f(UY, UY));
      // Right hand vector
      NMP::vpu::vector4_t b = NMP::vpu::add4f(NMP::vpu::mul4f(UX, PX), NMP::vpu::mul4f(UY, PY));

      //------------------------
      // Multiplexed solve. Note that zero length edges should be removed during asset compilation.
      NMP::vpu::vector4_t recipDet = NMP::vpu::rcp4f(M);
      NMP::vpu::vector4_t ry = NMP::vpu::mul4f(recipDet, b);
      // Recover the Barycentric coordinates for vertices A, B
      NMP::vpu::vector4_t rx = NMP::vpu::sub4f(NMP::vpu::one4f(), ry);

      //------------------------
      // Recover the projected point and compute the squared distance to the input point
      NMP::vpu::vector4_t QX = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.x, rx), NMP::vpu::mul4f(sampleBMP.x, ry));
      NMP::vpu::vector4_t QY = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.y, rx), NMP::vpu::mul4f(sampleBMP.y, ry));
      NMP::vpu::vector4_t VX = NMP::vpu::sub4f(samplePX, QX);
      NMP::vpu::vector4_t VY = NMP::vpu::sub4f(samplePY, QY);
      NMP::vpu::vector4_t distSq = NMP::vpu::add4f(NMP::vpu::mul4f(VX, VX), NMP::vpu::mul4f(VY, VY));

      // Determine if the projected point is closer than the current minimum
      NMP::vpu::vector4_t maskDistSq = NMP::vpu::mask4cmpLT(distSq, distSqMin); // distSq < distSqMin

      //------------------------
      // Determine if the projected point lies within the edge facet
      NMP::vpu::vector4_t maskRxA = NMP::vpu::mask4cmpLT(rx, minVals); // rx < minVal
      NMP::vpu::vector4_t maskRxB = NMP::vpu::mask4cmpLT(rx, maxVals); // rx < maxVal
      NMP::vpu::vector4_t maskRyA = NMP::vpu::mask4cmpLT(ry, minVals); // ry < minVal
      NMP::vpu::vector4_t maskRyB = NMP::vpu::mask4cmpLT(ry, maxVals); // ry < maxVal
      NMP::vpu::vector4_t maskRx = NMP::vpu::and4(NMP::vpu::not4(maskRxA), maskRxB); // rx >= minVal && rx < maxVal
      NMP::vpu::vector4_t maskRy = NMP::vpu::and4(NMP::vpu::not4(maskRyA), maskRyB); // ry >= minVal && ry < maxVal

      // Update mask
      NMP::vpu::vector4_t maskR = NMP::vpu::and4(maskDistSq, NMP::vpu::and4(maskRx, maskRy));

      //------------------------
      // Update the multiplexed result based on the condition mask
      resultFlags = NMP::vpu::sel4cmpMask(maskR, maskR, resultFlags);
      resultExtIndices = NMP::vpu::sel4cmpMask(maskR, NMP::vpu::load4f((float*)extIndices), resultExtIndices);
      resultSubDivIndices = NMP::vpu::sel4cmpMask(maskR, NMP::vpu::set4i(subDivIndex), resultSubDivIndices);
      resultX = NMP::vpu::sel4cmpMask(maskR, rx, resultX);
      resultY = NMP::vpu::sel4cmpMask(maskR, ry, resultY);
      XMinX = NMP::vpu::sel4cmpMask(maskR, QX, XMinX);
      XMinY = NMP::vpu::sel4cmpMask(maskR, QY, XMinY);
      distSqMin = NMP::vpu::sel4cmpMask(maskR, distSq, distSqMin);
    }
  }

  //------------------------
  // Store the multiplexed data
  NMP::Vector3 childNodeFlagsVec;
  uint32_t* childNodeFlags = (uint32_t*)&childNodeFlagsVec;
  NMP::vpu::store4f((float*)childNodeFlags, resultFlags);
  NMP::Vector3 distSqVec;
  float* distVals = (float*)&distSqVec;
  NMP::vpu::store4f(distVals, distSqMin);

  // Find the first valid entry in the block of four
  uint32_t index = 0;
  for (; index < 4; ++index)
  {
    if (childNodeFlags[index])
      break;
  }
  if (index == 4)
    return; // The initial sub-sample point was closest

  // Find the minimum within the block of four
  for (uint32_t i = index + 1; i < 4; ++i)
  {
    if (distVals[i] < distVals[index])
      index = i;
  }

  // Demultiplex the result data if the input control lies within a edge facet
  NMP::Vector3 extIndicesVec;
  uint32_t* extIndices = (uint32_t*)&extIndicesVec;
  NMP::vpu::store4f((float*)extIndices, resultExtIndices);
  NMP::Vector3 subDivIndicesVec;
  uint32_t* subDivIndices = (uint32_t*)&subDivIndicesVec;
  NMP::vpu::store4f((float*)subDivIndices, resultSubDivIndices);
  NMP::Vector3 R[2];
  NMP::vpu::store4f((float*)&R[0], resultX);
  NMP::vpu::store4f((float*)&R[1], resultY);

  uint32_t extIndex = extIndices[index];
  uint32_t subDivIndex = subDivIndices[index];

  // Exterior triangle index
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[extIndex];

  // Recover the projected point
  NMP::Vector3 XVec[2];
  NMP::vpu::store4f((float*)&XVec[0], XMinX);
  NMP::vpu::store4f((float*)&XVec[1], XMinY);
  XMin.set(XVec[0][index], XVec[1][index], 0.0f);

  // Interpolate the Barycentric vertex weights
  const uint16_t* subDivisionEdgeFacetIndices[2];
  uint32_t triangleSubSampleIndices[2];
  uint32_t exteriorEdgeFacetIndex;
  exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
  subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    exteriorEdgeFacetIndex, 0);
  subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    exteriorEdgeFacetIndex, 1);
  triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
  triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];

  NMP::vpu::vector4_t weightsA = NMP::vpu::set4f(
    subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[0]],
    subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[0]],
    subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[0]],
    0.0f);
  NMP::vpu::vector4_t weightsB = NMP::vpu::set4f(
    subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[1]],
    subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[1]],
    subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[1]],
    0.0f);
  NMP::vpu::vector4_t weights =
    NMP::vpu::add4f(
    NMP::vpu::mul4f(weightsA, NMP::vpu::set4f(R[0][index])),
    NMP::vpu::mul4f(weightsB, NMP::vpu::set4f(R[1][index])));
  NMP::vpu::store4f((float*)&weightsMin, weights);
}

#else
//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionClosestApproachUpdate(
  const AttribDataScatterBlend2DDef* attribDef,
  const ScatterBlendProjectionClosestFacetSubSampleBuffer* matchBuffer,
  const NMP::Vector3& sampleP,
  const uint16_t numSortedEntries,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  NMP_ASSERT(matchBuffer);
  const ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry* matchEntry = matchBuffer->getSortedEntry(0);
  float distanceSqMin = matchEntry->m_distanceSq;
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[matchEntry->m_extIndex];

  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);
  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  // Recover the sub-sample point
  const uint16_t* triangleSampleIndices0 = attribDef->getTriangleSampleIndices(
    exteriorTriangleIndexMin);
  uint16_t sampleIndex0 = triangleSampleIndices0[matchEntry->m_subDivIndex];
  XMin.set(
    (float)attribDef->m_samples[0][sampleIndex0] * qScale.x + qOffset.x,
    (float)attribDef->m_samples[1][sampleIndex0] * qScale.y + qOffset.y,
    0.0f);

  // Recover the triangle Barycentric weights
  subDivisionDataBlock.getTriangleSubSampleWeights(
    matchEntry->m_subDivIndex,
    weightsMin.x,
    weightsMin.y,
    weightsMin.z);

  //------------------------
  NMP::Vector3 XXMin(NMP::Vector3::InitZero);
  float RMin[2] = {0.0f, 0.0f};
  uint16_t extIndexMin = 0xFFFF;
  uint16_t subDivIndexMin = 0xFFFF;

  // Iterate over the closest edge facets and project the input point onto the facet surfaces
  for (uint16_t entryIndex = 0; entryIndex < numSortedEntries; ++entryIndex)
  {
    matchEntry = matchBuffer->getSortedEntry(entryIndex);

    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[matchEntry->m_extIndex]);

    // Get the appropriate sub-division edge facet table from the manager
    const uint16_t* subDivisionEdgeFacetIndices[2];
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[matchEntry->m_extIndex], 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[matchEntry->m_extIndex], 1);

    // Iterate over the exterior edge facet sub-divisions
    for (uint16_t subDivIndex = 0; subDivIndex < subDivisionDataBlock.m_numSubSimplexesPerEdgeFacet; ++subDivIndex)
    {
      // Recover the edge facet vertex indices (triangle local indices)
      uint16_t triangleSubSampleIndices[2];
      triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
      triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];

      // Recover the corresponding sample indices within the global list
      uint16_t sampleIndices[2];
      sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
      sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

      // Dequantise the edge facet vertices
      NMP::Vector3 sampleA(
        (float)attribDef->m_samples[0][sampleIndices[0]] * qScale.x + qOffset.x,
        (float)attribDef->m_samples[1][sampleIndices[0]] * qScale.y + qOffset.y,
        0.0f);
      NMP::Vector3 sampleB(
        (float)attribDef->m_samples[0][sampleIndices[1]] * qScale.x + qOffset.x,
        (float)attribDef->m_samples[1][sampleIndices[1]] * qScale.y + qOffset.y,
        0.0f);

      // Compute the facet edge vectors AB
      NMP::Vector3 U = sampleB - sampleA;
      NMP_ASSERT(U.magnitude() > 1e-4f);

      // Compute the input point relative to vertex A
      NMP::Vector3 Pa = sampleP - sampleA;

      //------------------------
      // Compute the projected point on the edge facet edge AB: X = alpha * U
      float UtPa = U.dot(Pa);
      float UtU = U.dot(U);
      float r[2];
      r[1] = UtPa / UtU;      

      const float minVal = -1e-3f;
      const float maxVal = 1.0f + 1e-3f;
      if (r[1] >= minVal && r[1] <= maxVal)
      {
        // Recover the projected point
        r[0] = 1.0f - r[1];
        NMP::Vector3 X = sampleA * r[0] + sampleB * r[1];

        // Compute the squared distance to the input point
        NMP::Vector3 v = sampleP - X;
        float distSq = v.magnitudeSquared();
        if (distSq < distanceSqMin)
        {
          distanceSqMin = distSq;
          extIndexMin = matchEntry->m_extIndex;
          subDivIndexMin = subDivIndex;
          XXMin = X;
          RMin[0] = r[0];
          RMin[1] = r[1];
        }
      }
    }
  }

  //------------------------
  // Interpolate the Barycentric vertex weights
  if (extIndexMin != 0xFFFF)
  {
    exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[extIndexMin];
    XMin = XXMin;

    const uint16_t* subDivisionEdgeFacetIndices[2];
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 1);
    uint16_t triangleSubSampleIndices[2];
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndexMin];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndexMin];

    // Compute the triangle Barycentric weights
    NMP::Vector3 weightsA;
    subDivisionDataBlock.getTriangleSubSampleWeights(
      triangleSubSampleIndices[0],
      weightsA.x,
      weightsA.y,
      weightsA.z);
    NMP::Vector3 weightsB;
    subDivisionDataBlock.getTriangleSubSampleWeights(
      triangleSubSampleIndices[1],
      weightsB.x,
      weightsB.y,
      weightsB.z);
    weightsMin = weightsA * RMin[0] + weightsB * RMin[1];
  }
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
// \brief Function sets the parameteriser info result structure with the specified
// minimum distance, position, blend weights and source node IDs.
static void scatterBlend2DAnnotationProjectionSetResult(
  ScatterBlend2DParameteriserInfo& result,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const uint16_t exteriorTriangleIndexMin,
  const NMP::Vector3& XMin,
  const NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Set the Barycentric blend weights
  result.m_childNodeWeights[0] = weightsMin.x;
  result.m_childNodeWeights[1] = weightsMin.y;
  result.m_childNodeWeights[2] = weightsMin.z;

  // Set the output motion parameters
  result.m_achievedParameters[0] = XMin.x;
  result.m_achievedParameters[1] = XMin.y;

  // Recover the vertex sample indices for the triangle
  const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndexMin);
  uint16_t triangleVertexIndices[3];
  triangleVertexIndices[0] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[0]];
  triangleVertexIndices[1] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[1]];
  triangleVertexIndices[2] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[2]];
  NMP_ASSERT(triangleVertexIndices[0] < attribDef->m_numVertices);
  NMP_ASSERT(triangleVertexIndices[1] < attribDef->m_numVertices);
  NMP_ASSERT(triangleVertexIndices[2] < attribDef->m_numVertices);

  // Set the blending sources
  result.m_childNodeIDs[0] = nodeDef->getChildNodeID(triangleVertexIndices[0]);
  result.m_childNodeIDs[1] = nodeDef->getChildNodeID(triangleVertexIndices[1]);
  result.m_childNodeIDs[2] = nodeDef->getChildNodeID(triangleVertexIndices[2]);
}

//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionClosestApproach(
  NMP::TempMemoryAllocator* tempDataAllocator,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& sampleP,
  ScatterBlend2DParameteriserInfo& result)
{
  // Allocate the temporary memory for the closest matches
  NMP_ASSERT(tempDataAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempDataAllocator->createChildAllocator();
  NMP::Memory::Format memReqs = ScatterBlendProjectionClosestFacetSubSampleBuffer::getMemoryRequirements(attribDef->m_numExteriorEdgeFacets);
  NMP::Memory::Resource memRes = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  NMP_ASSERT(memRes.ptr);
  ScatterBlendProjectionClosestFacetSubSampleBuffer* matchBuffer = ScatterBlendProjectionClosestFacetSubSampleBuffer::init(memRes, attribDef->m_numExteriorEdgeFacets);

  // Compute the closest matches from the input point to the exterior edge facet sub-samples
  scatterBlend2DAnnotationProjectionClosestApproachBuffer(
    attribDef,
    sampleP,
    matchBuffer);

  // Perform a partial sort of the sub-sample distances. Note that we recover sorted
  // entries in blocks of 4 since the closest approach update will process such blocks
  // in parallel when using SIMD.
  const uint16_t numSortedEntries = NMP::minimum((uint16_t)4, matchBuffer->getNumEntries());
  matchBuffer->partialSort(numSortedEntries);

  // Find the closest approach sub-division edge facet point
  NMP::Vector3 XMin;
  NMP::Vector3 weightsMin;
  uint16_t exteriorTriangleIndexMin;
  scatterBlend2DAnnotationProjectionClosestApproachUpdate(
    attribDef,
    matchBuffer,
    sampleP,
    numSortedEntries,
    exteriorTriangleIndexMin,
    XMin,
    weightsMin);

  // Set the result
  scatterBlend2DAnnotationProjectionSetResult(
    result,
    nodeDef,
    attribDef,
    exteriorTriangleIndexMin,
    XMin,
    weightsMin);

  // Revert to the rollback point
  childAllocator->memFree(matchBuffer);
  tempDataAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
// This Non-SIMD version of the function is faster than the SIMD version. The SIMD version
// requires you to iterate the major loop over the sub-division edges since these are in multiples
// of 1, 2 and 4 for the sub-division levels, while the exterior triangle facets have already been
// packed to a multiple of four in the asset compiler. This also fragments the output arrays
// to be less cache friendly when performing the projection update.
static uint32_t scatterBlend2DAnnotationProjectionInDirEdgeCull(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& Up,
  const NMP::Vector3& sampleP,
  uint16_t* extIndices,
  uint16_t* subDivIndices)
{
  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);

  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  // Remove the offset from the sample point
  NMP::Vector3 samplePmO = sampleP - qOffset;

  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  uint32_t numEntries = 0;
  for (uint16_t extIndex = 0; extIndex < attribDef->m_numExteriorEdgeFacets; ++extIndex)
  {
    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex]);
    NMP_ASSERT(triangleSampleIndices);

    // Get the appropriate sub-division edge facet table from the manager
    const uint16_t* subDivisionEdgeFacetIndices[2];
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 1);

    for (uint16_t subDivIndex = 0; subDivIndex < subDivisionDataBlock.m_numSubSimplexesPerEdgeFacet; ++subDivIndex)
    {
      // Recover the edge facet vertex indices (triangle local indices)
      uint16_t triangleSubSampleIndices[2];
      triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
      triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];

      // Recover the corresponding sample indices within the global list
      uint16_t sampleIndices[2];
      sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
      sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

      // Recover the edge facet vertices. Note that there is no need
      // to apply the offset since we will be computing relative vectors.
      NMP::Vector3 sampleA(
        (float)attribDef->m_samples[0][sampleIndices[0]] * qScale.x,
        (float)attribDef->m_samples[1][sampleIndices[0]] * qScale.y,
        0.0f);

      NMP::Vector3 sampleB(
        (float)attribDef->m_samples[0][sampleIndices[1]] * qScale.x,
        (float)attribDef->m_samples[1][sampleIndices[1]] * qScale.y,
        0.0f);

      // Compute the relative vectors between the input point and edge vertices
      NMP::Vector3 Ua = sampleA - samplePmO;
      NMP::Vector3 Ub = sampleB - samplePmO;

      // Determine if the projection would intersect the edge by testing the
      // dot product signs between the relative and projection complement vectors.
      // A change in sign indicates the projection intersects the edge.
      float UaDotUp = Ua.dot(Up);
      float UbDotUp = Ub.dot(Up);
      if (UaDotUp * UbDotUp <= 0.0f)
      {
        extIndices[numEntries] = extIndex;
        subDivIndices[numEntries] = subDivIndex;
        numEntries++;
      }
    }
  }

  // Pad to a block of four
  for (uint32_t i = numEntries; i & 0x03; ++i)
  {
    extIndices[i] = extIndices[i - 1];
    subDivIndices[i] = subDivIndices[i - 1];
  }

  return numEntries;
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend2DAnnotationProjectionInDirUpdate(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& Up,
  const NMP::Vector3& sampleP,
  const uint32_t numEntries,
  const uint16_t* extIndices,
  const uint16_t* subDivIndices,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Multiplexed quantisation scale and offset
  NMP::vpu::vector4_t qScalesX = NMP::vpu::set4f(attribDef->m_qScale[0]);
  NMP::vpu::vector4_t qScalesY = NMP::vpu::set4f(attribDef->m_qScale[1]);
  NMP::vpu::vector4_t qOffsetsX = NMP::vpu::set4f(attribDef->m_qOffset[0]);
  NMP::vpu::vector4_t qOffsetsY = NMP::vpu::set4f(attribDef->m_qOffset[1]);

  // Multiplexed projection vector complement
  NMP::vpu::vector4_t UpX = NMP::vpu::set4f(Up.x);
  NMP::vpu::vector4_t UpY = NMP::vpu::set4f(Up.y);

  // Tolerances
  NMP::vpu::vector4_t detTol = NMP::vpu::set4f(1e-5f);
  NMP::vpu::vector4_t minVals = NMP::vpu::set4f(-1e-3f);
  NMP::vpu::vector4_t maxVals = NMP::vpu::set4f(1.0f + 1e-3f);

  // Multiplexed input sample
  NMP::vpu::vector4_t samplePX = NMP::vpu::set4f(sampleP.x);
  NMP::vpu::vector4_t samplePY = NMP::vpu::set4f(sampleP.y);

  // Multiplexed result data
  NMP::vpu::vector4_t resultFlags = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultEntries = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultX = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultY = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinX = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinY = NMP::vpu::zero4f();
  NMP::vpu::vector4_t distSqMin = NMP::vpu::set4f(FLT_MAX);

  //------------------------
  // Iterate over the input exterior edge facet data
  for (uint32_t entryIndex = 0; entryIndex < numEntries; entryIndex += 4)
  {
    // Entry indices to process
    NMP::Vector3 entryIndicesVec;
    uint32_t* entryIndices = (uint32_t*)&entryIndicesVec;
    entryIndices[0] = entryIndex;
    entryIndices[1] = entryIndex + 1;
    entryIndices[2] = entryIndex + 2;
    entryIndices[3] = entryIndex + 3;

    uint32_t extIndex;
    uint32_t subDivIndex;
    uint32_t triangleSubSampleIndices[2];
    uint32_t sampleIndices[2];
    uint32_t exteriorTriangleIndex;
    uint32_t exteriorEdgeFacetIndex;
    const uint16_t* triangleSampleIndices;
    const uint16_t* subDivisionEdgeFacetIndices[2];

    //------------------------
    // Get the appropriate triangle sample indices from the attrib data
    extIndex = extIndices[entryIndex];
    subDivIndex = subDivIndices[entryIndex];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Get the appropriate tetrahedron sample indices from the attrib data
    extIndex = extIndices[entryIndex + 1];
    subDivIndex = subDivIndices[entryIndex + 1];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Get the appropriate tetrahedron sample indices from the attrib data
    extIndex = extIndices[entryIndex + 2];
    subDivIndex = subDivIndices[entryIndex + 2];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Get the appropriate tetrahedron sample indices from the attrib data
    extIndex = extIndices[entryIndex + 3];
    subDivIndex = subDivIndices[entryIndex + 3];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Multiplex and dequantise the sample data
    NMP::vpu::Vector3MP sampleAMP;
    sampleAMP.pack(qValA0, qValA1, qValA2, qValA3);
    sampleAMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.x), qScalesX, qOffsetsX);
    sampleAMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.y), qScalesY, qOffsetsY);

    NMP::vpu::Vector3MP sampleBMP;
    sampleBMP.pack(qValB0, qValB1, qValB2, qValB3);
    sampleBMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.x), qScalesX, qOffsetsX);
    sampleBMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.y), qScalesY, qOffsetsY);

    // Compute the facet edge vector AB
    NMP::vpu::vector4_t UX = NMP::vpu::sub4f(sampleBMP.x, sampleAMP.x);
    NMP::vpu::vector4_t UY = NMP::vpu::sub4f(sampleBMP.y, sampleAMP.y);

    // Compute the input point relative to vertex A
    NMP::vpu::vector4_t PX = NMP::vpu::sub4f(samplePX, sampleAMP.x);
    NMP::vpu::vector4_t PY = NMP::vpu::sub4f(samplePY, sampleAMP.y);

    //------------------------
    // Compute the projected point on the edge facet:
    // X = alpha * U, where
    // Up^T * (P - X) = 0
    NMP::vpu::vector4_t M = NMP::vpu::add4f(NMP::vpu::mul4f(UpX, UX), NMP::vpu::mul4f(UpY, UY));
    // Right hand vector
    NMP::vpu::vector4_t b = NMP::vpu::add4f(NMP::vpu::mul4f(UpX, PX), NMP::vpu::mul4f(UpY, PY));

    //------------------------
    // Multiplexed solve
    NMP::vpu::vector4_t maskDet = NMP::vpu::mask4cmpLT(NMP::vpu::abs4f(M), detTol);
    NMP::vpu::vector4_t detD = NMP::vpu::sel4cmpMask(maskDet, NMP::vpu::one4f(), M);
    NMP::vpu::vector4_t recipDet = NMP::vpu::rcp4f(detD);
    NMP::vpu::vector4_t ry = NMP::vpu::mul4f(recipDet, b);
    // Recover the Barycentric coordinates for vertices A, B
    NMP::vpu::vector4_t rx = NMP::vpu::sub4f(NMP::vpu::one4f(), ry);

    //------------------------
    // Recover the projected point and compute the squared distance to the input point
    NMP::vpu::vector4_t QX = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.x, rx), NMP::vpu::mul4f(sampleBMP.x, ry));
    NMP::vpu::vector4_t QY = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.y, rx), NMP::vpu::mul4f(sampleBMP.y, ry));
    NMP::vpu::vector4_t VX = NMP::vpu::sub4f(samplePX, QX);
    NMP::vpu::vector4_t VY = NMP::vpu::sub4f(samplePY, QY);
    NMP::vpu::vector4_t distSq = NMP::vpu::add4f(NMP::vpu::mul4f(VX, VX), NMP::vpu::mul4f(VY, VY));

    // Determine if the projected point is closer than the current minimum
    NMP::vpu::vector4_t maskDistSq = NMP::vpu::mask4cmpLT(distSq, distSqMin); // distSq < distSqMin

    //------------------------
    // Determine if the projected point lies within the edge facet
    NMP::vpu::vector4_t maskRxA = NMP::vpu::mask4cmpLT(rx, minVals); // rx < minVal
    NMP::vpu::vector4_t maskRxB = NMP::vpu::mask4cmpLT(rx, maxVals); // rx < maxVal
    NMP::vpu::vector4_t maskRyA = NMP::vpu::mask4cmpLT(ry, minVals); // ry < minVal
    NMP::vpu::vector4_t maskRyB = NMP::vpu::mask4cmpLT(ry, maxVals); // ry < maxVal
    NMP::vpu::vector4_t maskRx = NMP::vpu::and4(NMP::vpu::not4(maskRxA), maskRxB); // rx >= minVal && rx < maxVal
    NMP::vpu::vector4_t maskRy = NMP::vpu::and4(NMP::vpu::not4(maskRyA), maskRyB); // ry >= minVal && ry < maxVal

    // Update mask
    NMP::vpu::vector4_t maskR =
      NMP::vpu::and4(
        NMP::vpu::and4(NMP::vpu::not4(maskDet), maskDistSq),
        NMP::vpu::and4(maskRx, maskRy));

    //------------------------
    // Update the multiplexed result based on the condition mask
    resultFlags = NMP::vpu::sel4cmpMask(maskR, maskR, resultFlags);
    resultEntries = NMP::vpu::sel4cmpMask(maskR, NMP::vpu::load4f((float*)entryIndices), resultEntries);
    resultX = NMP::vpu::sel4cmpMask(maskR, rx, resultX);
    resultY = NMP::vpu::sel4cmpMask(maskR, ry, resultY);
    XMinX = NMP::vpu::sel4cmpMask(maskR, QX, XMinX);
    XMinY = NMP::vpu::sel4cmpMask(maskR, QY, XMinY);
    distSqMin = NMP::vpu::sel4cmpMask(maskR, distSq, distSqMin);
  }

  //------------------------
  // Store the multiplexed data
  NMP::Vector3 childNodeFlagsVec;
  uint32_t* childNodeFlags = (uint32_t*)&childNodeFlagsVec;
  NMP::vpu::store4f((float*)childNodeFlags, resultFlags);
  NMP::Vector3 distSqVec;
  float* distVals = (float*)&distSqVec;
  NMP::vpu::store4f(distVals, distSqMin);

  // Find the first valid entry in the block of four
  uint32_t index = 0;
  for (; index < 4; ++index)
  {
    if (childNodeFlags[index])
      break;
  }
  if (index == 4)
    return false;

  // Find the minimum within the block of four
  for (uint32_t i = index + 1; i < 4; ++i)
  {
    // Note invalid entries will be set to FLT_MAX
    if (distVals[i] < distVals[index])
      index = i;
  }

  // Demultiplex the result data if the input control lies within a edge facet
  const uint16_t* subDivisionEdgeFacetIndices[2];
  uint32_t triangleSubSampleIndices[2];
  uint32_t exteriorEdgeFacetIndex;

  NMP::Vector3 entryIndicesVec;
  uint32_t* entryIndices = (uint32_t*)&entryIndicesVec;
  NMP::vpu::store4f((float*)entryIndices, resultEntries);
  NMP::Vector3 R[2];
  NMP::vpu::store4f((float*)&R[0], resultX);
  NMP::vpu::store4f((float*)&R[1], resultY);

  uint32_t entryIndex = entryIndices[index];
  uint32_t extIndex = extIndices[entryIndex];
  uint32_t subDivIndex = subDivIndices[entryIndex];

  // Exterior triangle index
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[extIndex];

  // Recover the projected point
  NMP::Vector3 XVec[2];
  NMP::vpu::store4f((float*)&XVec[0], XMinX);
  NMP::vpu::store4f((float*)&XVec[1], XMinY);
  XMin.set(XVec[0][index], XVec[1][index], 0.0f);

  // Interpolate the Barycentric vertex weights
  exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
  subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    exteriorEdgeFacetIndex, 0);
  subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    exteriorEdgeFacetIndex, 1);
  triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
  triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];

  NMP::vpu::vector4_t weightsA = NMP::vpu::set4f(
    subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[0]],
    subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[0]],
    subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[0]],
    0.0f);
  NMP::vpu::vector4_t weightsB = NMP::vpu::set4f(
    subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[1]],
    subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[1]],
    subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[1]],
    0.0f);
  NMP::vpu::vector4_t weights =
    NMP::vpu::add4f(
    NMP::vpu::mul4f(weightsA, NMP::vpu::set4f(R[0][index])),
    NMP::vpu::mul4f(weightsB, NMP::vpu::set4f(R[1][index])));
  NMP::vpu::store4f((float*)&weightsMin, weights);

  return true;
}

#else
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend2DAnnotationProjectionInDirUpdate(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& Up,
  const NMP::Vector3& sampleP,
  const uint32_t numEntries,
  const uint16_t* extIndices,
  const uint16_t* subDivIndices,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);
  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  //------------------------
  // Project the input control onto the closest exterior edge facet sub-division simplex.
  uint16_t extIndexMin = 0xFFFF;
  uint16_t subDivIndexMin = 0xFFFF;
  float distanceSqMin = FLT_MAX;
  float RMin[2] = {0.0f, 0.0f};

  for (uint32_t entryIndex = 0; entryIndex < numEntries; ++entryIndex)
  {
    uint16_t extIndex = extIndices[entryIndex];
    uint16_t subDivIndex = subDivIndices[entryIndex];

    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex]);

    // Get the appropriate sub-division edge facet table from the manager
    const uint16_t* subDivisionEdgeFacetIndices[2];
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 1);

    // Recover the edge facet vertex indices (triangle local indices)
    uint16_t triangleSubSampleIndices[2];
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];

    // Recover the corresponding sample indices within the global list
    uint16_t sampleIndices[2];
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];
    NMP_ASSERT(sampleIndices[0] != sampleIndices[1]);

    // Dequantise the edge facet vertices
    NMP::Vector3 sampleA(
      (float)attribDef->m_samples[0][sampleIndices[0]] * qScale.x + qOffset.x,
      (float)attribDef->m_samples[1][sampleIndices[0]] * qScale.y + qOffset.y,
      0.0f);
    NMP::Vector3 sampleB(
      (float)attribDef->m_samples[0][sampleIndices[1]] * qScale.x + qOffset.x,
      (float)attribDef->m_samples[1][sampleIndices[1]] * qScale.y + qOffset.y,
      0.0f);

    // Compute the facet edge vector AB
    NMP::Vector3 U = sampleB - sampleA;
    NMP_ASSERT(U.magnitude() > 1e-4f);

    // Compute the input point relative to vertex A
    NMP::Vector3 P = sampleP - sampleA;

    //------------------------
    // Compute the projected point on the edge facet:
    // X = alpha * U, where
    // Up^T * (P - X) = 0
    float M = Up.dot(U);
    if (NMP::nmfabs(M) < 1e-5f)
      continue;
    float b = Up.dot(P);
    float r[2];
    // Recover the Barycentric coordinates for vertices A, B
    r[1] = b / M;
    r[0] = 1.0f - r[1];

    // Determine if the projected point lies within the edge facet
    const float minVal = -1e-3f;
    const float maxVal = 1.0f + 1e-3f;
    if (r[0] >= minVal && r[0] <= maxVal &&
        r[1] >= minVal && r[1] <= maxVal)
    {
      // Recover the projected point
      NMP::Vector3 X = sampleA * r[0] + sampleB * r[1];

      // Compute the squared distance to the input point
      NMP::Vector3 vv = sampleP - X;
      float distSq = vv.magnitudeSquared();
      if (distSq < distanceSqMin)
      {
        distanceSqMin = distSq;
        extIndexMin = extIndex;
        subDivIndexMin = subDivIndex;
        XMin = X;
        RMin[0] = r[0];
        RMin[1] = r[1];
      }
    }
  }

  if (extIndexMin == 0xFFFF)
    return false;

  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[extIndexMin];

  //------------------------
  // Interpolate the Barycentric vertex weights
  const uint16_t* subDivisionEdgeFacetIndices[2];
  subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 0);
  subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 1);
  uint16_t triangleSubSampleIndices[2];
  triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndexMin];
  triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndexMin];

  NMP::Vector3 weightsA;
  subDivisionDataBlock.getTriangleSubSampleWeights(
    triangleSubSampleIndices[0],
    weightsA.x,
    weightsA.y,
    weightsA.z);
  NMP::Vector3 weightsB;
  subDivisionDataBlock.getTriangleSubSampleWeights(
    triangleSubSampleIndices[1],
    weightsB.x,
    weightsB.y,
    weightsB.z);
  weightsMin = weightsA * RMin[0] + weightsB * RMin[1];

  return true;
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend2DAnnotationProjectionInDirClosestApproach(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& projectionDirection,
  const NMP::Vector3& sampleP,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Quantisation scale and offset
  NMP::vpu::vector4_t qScalesX = NMP::vpu::set4f(attribDef->m_qScale[0]);
  NMP::vpu::vector4_t qScalesY = NMP::vpu::set4f(attribDef->m_qScale[1]);
  NMP::vpu::vector4_t qOffsetsX = NMP::vpu::set4f(attribDef->m_qOffset[0]);
  NMP::vpu::vector4_t qOffsetsY = NMP::vpu::set4f(attribDef->m_qOffset[1]);

  // Multiplexed Projection vector
  NMP::vpu::vector4_t VpX = NMP::vpu::set4f(projectionDirection.x);
  NMP::vpu::vector4_t VpY = NMP::vpu::set4f(projectionDirection.y);

  // Multiplexed input sample
  NMP::vpu::vector4_t samplePX = NMP::vpu::set4f(sampleP.x);
  NMP::vpu::vector4_t samplePY = NMP::vpu::set4f(sampleP.y);

  // Multiplexed result data
  NMP::vpu::vector4_t resultFlags = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultEntries = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinX = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinY = NMP::vpu::zero4f();
  NMP::vpu::vector4_t distSqMin = NMP::vpu::set4f(FLT_MAX);

  uint32_t numSubSamplesPerEdgeFacet = subDivisionDataBlock.getNumSubSamplesPerEdgeFacet();
  uint32_t numEntries = attribDef->m_numExteriorEdgeFacets * numSubSamplesPerEdgeFacet;

  //------------------------
  // Iterate over the input exterior edge facet data
  for (uint32_t entryIndex = 0; entryIndex < numEntries; entryIndex += 4)
  {
    // Entry indices to process
    NMP::Vector3 entryIndicesVec;
    uint32_t* entryIndices = (uint32_t*)&entryIndicesVec;
    uint32_t index;
    uint32_t extIndex, subSampleIndex;
    uint32_t exteriorTriangleIndex;
    uint32_t exteriorEdgeFacetIndex;
    uint32_t triangleSubSampleIndex;
    uint32_t sampleIndex;
    const uint16_t* triangleSampleIndices;
    const uint16_t* subSampleEdgeFacetIndices;

    //------------------------
    // Get the appropriate triangle sample indices from the attrib data
    extIndex = entryIndex / numSubSamplesPerEdgeFacet;
    subSampleIndex = entryIndex % numSubSamplesPerEdgeFacet;
    entryIndices[0] = entryIndex;
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate edge facet sub-sample table from the manager
    subSampleEdgeFacetIndices = subDivisionDataBlock.getSubSampleEdgeFacetIndices(exteriorEdgeFacetIndex);
    // Get the sample index in the global list
    triangleSubSampleIndex = subSampleEdgeFacetIndices[subSampleIndex];
    sampleIndex = triangleSampleIndices[triangleSubSampleIndex];

    // Recover the quantised samples
    NMP::vpu::vector4_t qVal0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndex],
      attribDef->m_samples[1][sampleIndex],
      0,
      0);

    //------------------------
    // Get the appropriate triangle sample indices from the attrib data
    index = entryIndex + 1;
    extIndex = index / numSubSamplesPerEdgeFacet;
    subSampleIndex = index % numSubSamplesPerEdgeFacet;
    entryIndices[1] = index;
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate edge facet sub-sample table from the manager
    subSampleEdgeFacetIndices = subDivisionDataBlock.getSubSampleEdgeFacetIndices(exteriorEdgeFacetIndex);
    // Get the sample index in the global list
    triangleSubSampleIndex = subSampleEdgeFacetIndices[subSampleIndex];
    sampleIndex = triangleSampleIndices[triangleSubSampleIndex];

    // Recover the quantised samples
    NMP::vpu::vector4_t qVal1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndex],
      attribDef->m_samples[1][sampleIndex],
      0,
      0);

    //------------------------
    // Get the appropriate triangle sample indices from the attrib data
    index = entryIndex + 2;
    extIndex = index / numSubSamplesPerEdgeFacet;
    subSampleIndex = index % numSubSamplesPerEdgeFacet;
    entryIndices[2] = index;
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate edge facet sub-sample table from the manager
    subSampleEdgeFacetIndices = subDivisionDataBlock.getSubSampleEdgeFacetIndices(exteriorEdgeFacetIndex);
    // Get the sample index in the global list
    triangleSubSampleIndex = subSampleEdgeFacetIndices[subSampleIndex];
    sampleIndex = triangleSampleIndices[triangleSubSampleIndex];

    // Recover the quantised samples
    NMP::vpu::vector4_t qVal2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndex],
      attribDef->m_samples[1][sampleIndex],
      0,
      0);

    //------------------------
    // Get the appropriate triangle sample indices from the attrib data
    index = entryIndex + 3;
    extIndex = index / numSubSamplesPerEdgeFacet;
    subSampleIndex = index % numSubSamplesPerEdgeFacet;
    entryIndices[3] = index;
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate edge facet sub-sample table from the manager
    subSampleEdgeFacetIndices = subDivisionDataBlock.getSubSampleEdgeFacetIndices(exteriorEdgeFacetIndex);
    // Get the sample index in the global list
    triangleSubSampleIndex = subSampleEdgeFacetIndices[subSampleIndex];
    sampleIndex = triangleSampleIndices[triangleSubSampleIndex];

    // Recover the quantised samples
    NMP::vpu::vector4_t qVal3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndex],
      attribDef->m_samples[1][sampleIndex],
      0,
      0);

    //------------------------
    // Multiplex and dequantise the sample data
    NMP::vpu::Vector3MP sampleMP;
    sampleMP.pack(qVal0, qVal1, qVal2, qVal3);
    sampleMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleMP.x), qScalesX, qOffsetsX);
    sampleMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleMP.y), qScalesY, qOffsetsY);

    // Compute the orthogonal vector from the projected point to the sub-sample vertex
    NMP::vpu::vector4_t PX = NMP::vpu::sub4f(sampleMP.x, samplePX);
    NMP::vpu::vector4_t PY = NMP::vpu::sub4f(sampleMP.y, samplePY);
    NMP::vpu::vector4_t alpha = NMP::vpu::add4f(NMP::vpu::mul4f(VpX, PX), NMP::vpu::mul4f(VpY, PY));
    NMP::vpu::vector4_t vvX = NMP::vpu::sub4f(PX, NMP::vpu::mul4f(VpX, alpha));
    NMP::vpu::vector4_t vvY = NMP::vpu::sub4f(PY, NMP::vpu::mul4f(VpY, alpha));

    // Determine if the projected point is closer than the current minimum
    NMP::vpu::vector4_t distSq = NMP::vpu::add4f(NMP::vpu::mul4f(vvX, vvX), NMP::vpu::mul4f(vvY, vvY));
    NMP::vpu::vector4_t maskDistSq = NMP::vpu::mask4cmpLT(distSq, distSqMin); // distSq < distSqMin

    //------------------------
    // Update the multiplexed result based on the condition mask
    resultFlags = NMP::vpu::sel4cmpMask(maskDistSq, maskDistSq, resultFlags);
    resultEntries = NMP::vpu::sel4cmpMask(maskDistSq, NMP::vpu::load4f((float*)entryIndices), resultEntries);
    XMinX = NMP::vpu::sel4cmpMask(maskDistSq, sampleMP.x, XMinX);
    XMinY = NMP::vpu::sel4cmpMask(maskDistSq, sampleMP.y, XMinY);
    distSqMin = NMP::vpu::sel4cmpMask(maskDistSq, distSq, distSqMin);
  }

  //------------------------
  // Store the multiplexed data
  NMP::Vector3 childNodeFlagsVec;
  uint32_t* childNodeFlags = (uint32_t*)&childNodeFlagsVec;
  NMP::vpu::store4f((float*)childNodeFlags, resultFlags);
  NMP::Vector3 distSqVec;
  float* distVals = (float*)&distSqVec;
  NMP::vpu::store4f(distVals, distSqMin);

  // Find the first valid entry in the block of four
  uint32_t index = 0;
  for (; index < 4; ++index)
  {
    if (childNodeFlags[index])
      break;
  }
  NMP_ASSERT(index < 4);

  // Find the minimum within the block of four
  for (uint32_t i = index + 1; i < 4; ++i)
  {
    // Note invalid entries will be set to FLT_MAX
    if (distVals[i] < distVals[index])
      index = i;
  }

  // Demultiplex the result data if the input control lies within a edge facet
  NMP::Vector3 entryIndicesVec;
  uint32_t* entryIndices = (uint32_t*)&entryIndicesVec;
  NMP::vpu::store4f((float*)entryIndices, resultEntries);

  uint32_t entryIndex = entryIndices[index];
  uint32_t extIndex = entryIndex / numSubSamplesPerEdgeFacet;
  uint32_t subSampleIndex = entryIndex % numSubSamplesPerEdgeFacet;

  // Exterior triangle index
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[extIndex];
  
  // Recover the projected point
  NMP::Vector3 XVec[2];
  NMP::vpu::store4f((float*)&XVec[0], XMinX);
  NMP::vpu::store4f((float*)&XVec[1], XMinY);
  XMin.set(XVec[0][index], XVec[1][index], 0.0f);

  // Recover the triangle Barycentric weights
  uint32_t exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
  const uint16_t* subSampleEdgeFacetIndices = subDivisionDataBlock.getSubSampleEdgeFacetIndices(exteriorEdgeFacetIndex);
  uint32_t triangleSubSampleIndex = subSampleEdgeFacetIndices[subSampleIndex];
  subDivisionDataBlock.getTriangleSubSampleWeights(
    triangleSubSampleIndex,
    weightsMin.x,
    weightsMin.y,
    weightsMin.z);

  return true;
}

#else
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend2DAnnotationProjectionInDirClosestApproach(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& projectionDirection,
  const NMP::Vector3& sampleP,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);
  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  uint16_t numSubSamplesPerEdgeFacet = subDivisionDataBlock.getNumSubSamplesPerEdgeFacet();
  float distanceSqMin = FLT_MAX;

  for (uint16_t extIndex = 0; extIndex < attribDef->m_numExteriorEdgeFacets; ++extIndex)
  {
    uint16_t exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];

    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex]);

    // Get the appropriate edge facet sub-sample table from the manager    
    const uint16_t* subSampleEdgeFacetIndices = subDivisionDataBlock.getSubSampleEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex]);

    for (uint16_t subSampleIndex = 0; subSampleIndex < numSubSamplesPerEdgeFacet; ++subSampleIndex)
    {
      // Get the sample index in the global list
      uint16_t triangleSubSampleIndex = subSampleEdgeFacetIndices[subSampleIndex];
      uint16_t sampleIndex = triangleSampleIndices[triangleSubSampleIndex];

      // Recover the edge facet sub-sample
      NMP::Vector3 sampleX(
        (float)attribDef->m_samples[0][sampleIndex] * qScale.x + qOffset.x,
        (float)attribDef->m_samples[1][sampleIndex] * qScale.y + qOffset.y,
        0.0f);

      // Compute the orthogonal vector from the projected point to the sub-sample vertex
      NMP::Vector3 P = sampleX - sampleP;
      float alpha = projectionDirection.dot(P);
      NMP::Vector3 vv = P - projectionDirection * alpha;

      // Compute the squared distance
      float distSq = vv.magnitudeSquared();
      if (distSq < distanceSqMin)
      {
        // Update the minimum
        distanceSqMin = distSq;
        exteriorTriangleIndexMin = exteriorTriangleIndex;
        XMin = sampleX;

        // Recover the triangle Barycentric weights
        subDivisionDataBlock.getTriangleSubSampleWeights(
          triangleSubSampleIndex,
          weightsMin.x,
          weightsMin.y,
          weightsMin.z);
      }
    }
  }

  return true;
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
// \brief Function iterates through the external edge facets and projects the
// input control point onto the sub-division edge facets.
static void scatterBlend2DAnnotationProjectionInDir(
  NMP::TempMemoryAllocator* tempDataAllocator,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& projectionDirection,
  const NMP::Vector3& sampleP,
  ScatterBlend2DParameteriserInfo& result)
{
  // Compute the orthogonal complement to the projection direction.
  NMP::Vector3 Up;
  Up.set(projectionDirection.y, -projectionDirection.x, 0.0f);

  // Compute the list of exterior edge facet (extIndex, subDivIndex) pairs to process.
  // Cull edges by performing a fast test to determine if the projection intersects an edge.
  NMP::TempMemoryAllocator* childAllocator = tempDataAllocator->createChildAllocator();
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);
  uint16_t entryBufferSize = attribDef->m_numExteriorEdgeFacets * subDivisionDataBlock.m_numSubSimplexesPerEdgeFacet;
  uint16_t entryBufferSize4 = NMP::nmAlignedValue4(entryBufferSize);
  NMP::Memory::Format memReqs(sizeof(uint16_t) * entryBufferSize4, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResT = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  NMP::Memory::Resource memResST = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  uint16_t* extIndices = (uint16_t*)memResT.ptr;
  uint16_t* subDivIndices = (uint16_t*)memResST.ptr;
  uint32_t numEntries = scatterBlend2DAnnotationProjectionInDirEdgeCull(
    attribDef,
    Up,
    sampleP,
    extIndices,
    subDivIndices);

  // Project the input control onto the annotation
  uint16_t exteriorTriangleIndexMin = 0;
  NMP::Vector3 XMin;
  NMP::Vector3 weightsMin;
  bool status = scatterBlend2DAnnotationProjectionInDirUpdate(
    attribDef,
    Up,
    sampleP,
    numEntries,
    extIndices,
    subDivIndices,
    exteriorTriangleIndexMin,
    XMin,
    weightsMin);

  if (!status)
  {
    status = scatterBlend2DAnnotationProjectionInDirClosestApproach(
      attribDef,
      projectionDirection,
      sampleP,
      exteriorTriangleIndexMin,
      XMin,
      weightsMin);
  }

  //------------------------
  // Set the result
  scatterBlend2DAnnotationProjectionSetResult(
    result,
    nodeDef,
    attribDef,
    exteriorTriangleIndexMin,
    XMin,
    weightsMin);

  // Tidy up the back-face culling buffer
  childAllocator->memFree(extIndices);
  childAllocator->memFree(subDivIndices);
  tempDataAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionInDirection(
  NMP::TempMemoryAllocator* tempDataAllocator,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& inputMotionParameters,
  ScatterBlend2DParameteriserInfo& result)
{
  NMP::Vector3 projectionDirection(
    attribDef->m_projectionVector[0],
    attribDef->m_projectionVector[1],
    0.0f);

  scatterBlend2DAnnotationProjectionInDir(
    tempDataAllocator,
    nodeDef,
    attribDef,
    projectionDirection,
    inputMotionParameters,
    result);
}

//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionTowardsTarget(
  NMP::TempMemoryAllocator* tempDataAllocator,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& inputMotionParameters,
  ScatterBlend2DParameteriserInfo& result)
{
  NMP::Vector3 projectionDirection(
    attribDef->m_projectionVector[0] - inputMotionParameters.x,
    attribDef->m_projectionVector[1] - inputMotionParameters.y,
    0.0f);
  projectionDirection.normalise();

  scatterBlend2DAnnotationProjectionInDir(
    tempDataAllocator,
    nodeDef,
    attribDef,
    projectionDirection,
    inputMotionParameters,
    result);
}

//----------------------------------------------------------------------------------------------------------------------
// This Non-SIMD version of the function is faster than the SIMD version. The SIMD version
// requires you to iterate the major loop over the sub-division edges since these are in multiples
// of 1, 2 and 4 for the sub-division levels, while the exterior triangle facets have already been
// packed to a multiple of four in the asset compiler. This also fragments the output arrays
// to be less cache friendly when performing the projection update.
static uint32_t scatterBlend2DAnnotationProjectionBackFaceCull(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& sampleP,
  uint16_t* extIndices,
  uint16_t* subDivIndices,
  uint16_t* projDirs)
{
  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);

  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  // Remove the offset from the sample point
  NMP::Vector3 samplePmO = sampleP - qOffset;

  // NOTE: we perform projection in the viewport aspect ratio adjusted space.
  samplePmO.x *= attribDef->m_displayScale[0];
  samplePmO.y *= attribDef->m_displayScale[1];

  // Apply aspect ratio scale to the quantisation scales
  qScale.x *= attribDef->m_displayScale[0];
  qScale.y *= attribDef->m_displayScale[1];

  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  uint32_t numEntries = 0;
  for (uint16_t extIndex = 0; extIndex < attribDef->m_numExteriorEdgeFacets; ++extIndex)
  {
    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex]);
    NMP_ASSERT(triangleSampleIndices);

    // Get the appropriate sub-division edge facet table from the manager
    const uint16_t* subDivisionEdgeFacetIndices[2];
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 1);

    for (uint16_t subDivIndex = 0; subDivIndex < subDivisionDataBlock.m_numSubSimplexesPerEdgeFacet; ++subDivIndex)
    {
      // Recover the edge facet vertex indices (triangle local indices)
      uint16_t triangleSubSampleIndices[2];
      triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
      triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];

      // Recover the corresponding sample indices within the global list
      uint16_t sampleIndices[2];
      sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
      sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

      // Recover the edge facet vertices. Note that there is no need
      // to apply the offset since we will be computing relative vectors.
      NMP::Vector3 sampleA(
        (float)attribDef->m_samples[0][sampleIndices[0]] * qScale.x,
        (float)attribDef->m_samples[1][sampleIndices[0]] * qScale.y,
        0.0f);

      NMP::Vector3 sampleB(
        (float)attribDef->m_samples[0][sampleIndices[1]] * qScale.x,
        (float)attribDef->m_samples[1][sampleIndices[1]] * qScale.y,
        0.0f);

      // Compute the facet edge vector AB
      NMP::Vector3 U = sampleB - sampleA;

      // Compute the normal to the edge facet
      NMP::Vector3 N;
      N.set(U.y, -U.x, 0.0f);

      // Compute the direction vector from the sample point to the edge mid-point
      NMP::Vector3 V = (sampleA + sampleB) * 0.5f - samplePmO;

      // Compare with the projection direction
      float NdotP = N.dot(V);
      if (NdotP < 1e-3f)
      {
        // Determine which axis direction to project in
        uint16_t pdIndex;
        if (fabs(N.x) >= fabs(N.y))
          pdIndex = 0; // Projection in X
        else
          pdIndex = 1; // Projection in Y

        extIndices[numEntries] = extIndex;
        subDivIndices[numEntries] = subDivIndex;
        projDirs[numEntries] = pdIndex;
        numEntries++;
      }
    }
  }

  // Pad to a block of four
  for (uint32_t i = numEntries; i & 0x03; ++i)
  {
    uint32_t ii = i - 1;
    extIndices[i] = extIndices[ii];
    subDivIndices[i] = subDivIndices[ii];
    projDirs[i] = projDirs[ii];
  }

  return numEntries;
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionInAxisForFacetUpdate(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& sampleP,
  uint32_t numEntries,
  uint16_t* extIndices,
  uint16_t* subDivIndices,
  uint16_t* pdIndices,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // NOTE: we perform projection in the viewport aspect ratio adjusted space.
  NMP::vpu::vector4_t displayScaleX = NMP::vpu::set4f(attribDef->m_displayScale[0]);
  NMP::vpu::vector4_t displayScaleY = NMP::vpu::set4f(attribDef->m_displayScale[1]);

  // Multiplexed quantisation scale and offset (and apply viewport aspect ratio scale)
  NMP::vpu::vector4_t qScalesX = NMP::vpu::mul4f(NMP::vpu::set4f(attribDef->m_qScale[0]), displayScaleX);
  NMP::vpu::vector4_t qScalesY = NMP::vpu::mul4f(NMP::vpu::set4f(attribDef->m_qScale[1]), displayScaleY);
  NMP::vpu::vector4_t qOffsetsX = NMP::vpu::mul4f(NMP::vpu::set4f(attribDef->m_qOffset[0]), displayScaleX);
  NMP::vpu::vector4_t qOffsetsY = NMP::vpu::mul4f(NMP::vpu::set4f(attribDef->m_qOffset[1]), displayScaleY);

  // Projection vector table
  float UpsX[2] = {0.0f, 1.0f};
  float UpsY[2] = {1.0f, 0.0f};

  // Tolerances
  NMP::vpu::vector4_t detTol = NMP::vpu::set4f(1e-5f);
  NMP::vpu::vector4_t minVals = NMP::vpu::set4f(-1e-3f);
  NMP::vpu::vector4_t maxVals = NMP::vpu::set4f(1.0f + 1e-3f);

  // Multiplexed input sample (and apply viewport aspect ratio scale)
  NMP::vpu::vector4_t samplePX = NMP::vpu::mul4f(NMP::vpu::set4f(sampleP.x), displayScaleX);
  NMP::vpu::vector4_t samplePY = NMP::vpu::mul4f(NMP::vpu::set4f(sampleP.y), displayScaleY);

  // Multiplexed result data
  NMP::vpu::vector4_t resultFlagsInterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultEntriesInterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultXInterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultYInterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinXInterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinYInterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t distSqMinInterior = NMP::vpu::set4f(FLT_MAX);

  NMP::vpu::vector4_t resultEntriesExterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultXExterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultYExterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinXExterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t XMinYExterior = NMP::vpu::zero4f();
  NMP::vpu::vector4_t distSqMinExterior = NMP::vpu::set4f(FLT_MAX);

  //------------------------
  // Iterate over the input exterior edge facet data
  for (uint32_t entryIndex = 0; entryIndex < numEntries; entryIndex += 4)
  {
    // Entry indices to process
    NMP::Vector3 entryIndicesVec;
    uint32_t* entryIndices = (uint32_t*)&entryIndicesVec;
    entryIndices[0] = entryIndex;
    entryIndices[1] = entryIndex + 1;
    entryIndices[2] = entryIndex + 2;
    entryIndices[3] = entryIndex + 3;

    uint32_t extIndex;
    uint32_t subDivIndex;
    uint32_t triangleSubSampleIndices[2];
    uint32_t sampleIndices[2];
    uint32_t exteriorTriangleIndex;
    uint32_t exteriorEdgeFacetIndex;
    const uint16_t* triangleSampleIndices;
    const uint16_t* subDivisionEdgeFacetIndices[2];

    //------------------------
    // Get the appropriate triangle sample indices from the attrib data
    extIndex = extIndices[entryIndex];
    subDivIndex = subDivIndices[entryIndex];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Get the appropriate tetrahedron sample indices from the attrib data
    extIndex = extIndices[entryIndex + 1];
    subDivIndex = subDivIndices[entryIndex + 1];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Get the appropriate tetrahedron sample indices from the attrib data
    extIndex = extIndices[entryIndex + 2];
    subDivIndex = subDivIndices[entryIndex + 2];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Get the appropriate tetrahedron sample indices from the attrib data
    extIndex = extIndices[entryIndex + 3];
    subDivIndex = subDivIndices[entryIndex + 3];
    exteriorTriangleIndex = attribDef->m_exteriorTriangleIndices[extIndex];
    exteriorEdgeFacetIndex = attribDef->m_exteriorEdgeFacetIndices[extIndex];
    triangleSampleIndices = attribDef->getTriangleSampleIndices(exteriorTriangleIndex);
    // Get the appropriate sub-division edge facet table from the manager
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      exteriorEdgeFacetIndex, 1);
    // Recover the edge facet vertex indices (triangle local indices)
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);

    //------------------------
    // Multiplex and dequantise the sample data
    NMP::vpu::Vector3MP sampleAMP;
    sampleAMP.pack(qValA0, qValA1, qValA2, qValA3);
    sampleAMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.x), qScalesX, qOffsetsX);
    sampleAMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.y), qScalesY, qOffsetsY);

    NMP::vpu::Vector3MP sampleBMP;
    sampleBMP.pack(qValB0, qValB1, qValB2, qValB3);
    sampleBMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.x), qScalesX, qOffsetsX);
    sampleBMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.y), qScalesY, qOffsetsY);

    //------------------------
    // Compute the facet edge vector AB
    NMP::vpu::vector4_t UX = NMP::vpu::sub4f(sampleBMP.x, sampleAMP.x);
    NMP::vpu::vector4_t UY = NMP::vpu::sub4f(sampleBMP.y, sampleAMP.y);

    // Projection vector
    NMP::vpu::vector4_t UpX = NMP::vpu::set4f(
      UpsX[pdIndices[entryIndex]],
      UpsX[pdIndices[entryIndex + 1]],
      UpsX[pdIndices[entryIndex + 2]],
      UpsX[pdIndices[entryIndex + 3]]);
    NMP::vpu::vector4_t UpY = NMP::vpu::set4f(
      UpsY[pdIndices[entryIndex]],
      UpsY[pdIndices[entryIndex + 1]],
      UpsY[pdIndices[entryIndex + 2]],
      UpsY[pdIndices[entryIndex + 3]]);

    // Compute the projected point on the edge facet:
    // X = alpha * U, where
    // Up^T * (Pa - X) = 0
    NMP::vpu::vector4_t PaX = NMP::vpu::sub4f(samplePX, sampleAMP.x);
    NMP::vpu::vector4_t PaY = NMP::vpu::sub4f(samplePY, sampleAMP.y);
    NMP::vpu::vector4_t M = NMP::vpu::add4f(NMP::vpu::mul4f(UpX, UX), NMP::vpu::mul4f(UpY, UY));
    // Right hand vector
    NMP::vpu::vector4_t b = NMP::vpu::add4f(NMP::vpu::mul4f(UpX, PaX), NMP::vpu::mul4f(UpY, PaY));

    //------------------------
    // Multiplexed solve
    NMP::vpu::vector4_t maskDet = NMP::vpu::mask4cmpLT(NMP::vpu::abs4f(M), detTol);
    NMP::vpu::vector4_t detD = NMP::vpu::sel4cmpMask(maskDet, NMP::vpu::one4f(), M);
    NMP::vpu::vector4_t recipDet = NMP::vpu::rcp4f(detD);
    NMP::vpu::vector4_t ry = NMP::vpu::mul4f(recipDet, b);
    // Recover the Barycentric coordinates for vertices A, B
    NMP::vpu::vector4_t rx = NMP::vpu::sub4f(NMP::vpu::one4f(), ry);

    //------------------------
    // Recover the projected point and compute the squared distance to the input point
    NMP::vpu::vector4_t QX = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.x, rx), NMP::vpu::mul4f(sampleBMP.x, ry));
    NMP::vpu::vector4_t QY = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.y, rx), NMP::vpu::mul4f(sampleBMP.y, ry));
    NMP::vpu::vector4_t VX = NMP::vpu::sub4f(samplePX, QX);
    NMP::vpu::vector4_t VY = NMP::vpu::sub4f(samplePY, QY);
    NMP::vpu::vector4_t distSqInterior = NMP::vpu::add4f(NMP::vpu::mul4f(VX, VX), NMP::vpu::mul4f(VY, VY));

    // Determine if the projected point is closer than the current minimum
    NMP::vpu::vector4_t maskDistSqInterior = NMP::vpu::mask4cmpLT(distSqInterior, distSqMinInterior);

    //------------------------
    // Determine if the projected point lies within the edge facet
    NMP::vpu::vector4_t maskRxA = NMP::vpu::mask4cmpLT(rx, minVals); // rx < minVal
    NMP::vpu::vector4_t maskRxB = NMP::vpu::mask4cmpLT(rx, maxVals); // rx < maxVal
    NMP::vpu::vector4_t maskRyA = NMP::vpu::mask4cmpLT(ry, minVals); // ry < minVal
    NMP::vpu::vector4_t maskRyB = NMP::vpu::mask4cmpLT(ry, maxVals); // ry < maxVal
    NMP::vpu::vector4_t maskRx = NMP::vpu::and4(NMP::vpu::not4(maskRxA), maskRxB); // rx >= minVal && rx < maxVal
    NMP::vpu::vector4_t maskRy = NMP::vpu::and4(NMP::vpu::not4(maskRyA), maskRyB); // ry >= minVal && ry < maxVal

    // Update mask
    NMP::vpu::vector4_t maskR =
      NMP::vpu::and4(
        NMP::vpu::and4(NMP::vpu::not4(maskDet), maskDistSqInterior),
        NMP::vpu::and4(maskRx, maskRy));

    //------------------------
    // Update the multiplexed result based on the condition mask
    resultFlagsInterior = NMP::vpu::sel4cmpMask(maskR, maskR, resultFlagsInterior);
    resultEntriesInterior = NMP::vpu::sel4cmpMask(maskR, NMP::vpu::load4f((float*)entryIndices), resultEntriesInterior);
    resultXInterior = NMP::vpu::sel4cmpMask(maskR, rx, resultXInterior);
    resultYInterior = NMP::vpu::sel4cmpMask(maskR, ry, resultYInterior);
    XMinXInterior = NMP::vpu::sel4cmpMask(maskR, QX, XMinXInterior);
    XMinYInterior = NMP::vpu::sel4cmpMask(maskR, QY, XMinYInterior);
    distSqMinInterior = NMP::vpu::sel4cmpMask(maskR, distSqInterior, distSqMinInterior);

    //------------------------
    // Clamp to the edge facet
    NMP::vpu::vector4_t rxClamped;
    rxClamped = NMP::vpu::sel4cmpLT(rx, NMP::vpu::zero4f(), NMP::vpu::zero4f(), rx);
    rxClamped = NMP::vpu::sel4cmpLT(rxClamped, NMP::vpu::one4f(), rxClamped, NMP::vpu::one4f());
    NMP::vpu::vector4_t ryClamped;
    ryClamped = NMP::vpu::sel4cmpLT(ry, NMP::vpu::zero4f(), NMP::vpu::zero4f(), ry);
    ryClamped = NMP::vpu::sel4cmpLT(ryClamped, NMP::vpu::one4f(), ryClamped, NMP::vpu::one4f());

    // Recover the projected point
    NMP::vpu::vector4_t QXext = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.x, rxClamped), NMP::vpu::mul4f(sampleBMP.x, ryClamped));
    NMP::vpu::vector4_t QYext = NMP::vpu::add4f(NMP::vpu::mul4f(sampleAMP.y, rxClamped), NMP::vpu::mul4f(sampleBMP.y, ryClamped));

     // Compute the residual distance in the orthogonal complement direction
    NMP::vpu::vector4_t VXext = NMP::vpu::sub4f(samplePX, QXext);
    NMP::vpu::vector4_t VYext = NMP::vpu::sub4f(samplePY, QYext);
    NMP::vpu::vector4_t dExt = NMP::vpu::add4f(NMP::vpu::mul4f(UpX, VXext), NMP::vpu::mul4f(UpY, VYext));

    // Update the minimum squared error
    NMP::vpu::vector4_t distSqExterior, maskDistSqExterior;
    distSqExterior = NMP::vpu::mul4f(dExt, dExt);
    maskDistSqExterior = NMP::vpu::mask4cmpLT(distSqExterior, distSqMinExterior);

    // Update the multiplexed result based on the condition mask
    distSqMinExterior = NMP::vpu::sel4cmpMask(maskDistSqExterior, distSqExterior, distSqMinExterior);
    resultEntriesExterior = NMP::vpu::sel4cmpMask(maskDistSqExterior, NMP::vpu::load4f((float*)entryIndices), resultEntriesExterior);
    resultXExterior = NMP::vpu::sel4cmpMask(maskDistSqExterior, rxClamped, resultXExterior);
    resultYExterior = NMP::vpu::sel4cmpMask(maskDistSqExterior, ryClamped, resultYExterior);
    XMinXExterior = NMP::vpu::sel4cmpMask(maskDistSqExterior, QXext, XMinXExterior);
    XMinYExterior = NMP::vpu::sel4cmpMask(maskDistSqExterior, QYext, XMinYExterior);
  }

  //------------------------
  // Store the multiplexed data (interior)
  NMP::Vector3 childNodeFlagsVecInterior;
  uint32_t* childNodeFlagsInterior = (uint32_t*)&childNodeFlagsVecInterior;
  NMP::vpu::store4f((float*)childNodeFlagsInterior, resultFlagsInterior);
  NMP::Vector3 distSqVecInterior;
  float* distValsInterior = (float*)&distSqVecInterior;
  NMP::vpu::store4f(distValsInterior, distSqMinInterior);
  NMP::Vector3 entryIndicesVecInterior;
  uint32_t* entryIndicesInterior = (uint32_t*)&entryIndicesVecInterior;
  NMP::vpu::store4f((float*)entryIndicesInterior, resultEntriesInterior);
  NMP::Vector3 RInterior[2];
  NMP::vpu::store4f((float*)&RInterior[0], resultXInterior);
  NMP::vpu::store4f((float*)&RInterior[1], resultYInterior);
  NMP::Vector3 XVecInterior[2];
  NMP::vpu::store4f((float*)&XVecInterior[0], XMinXInterior);
  NMP::vpu::store4f((float*)&XVecInterior[1], XMinYInterior);

  // Store the multiplexed data (exterior)
  NMP::Vector3 distSqVecExt;
  float* distValsExterior = (float*)&distSqVecExt;
  NMP::vpu::store4f(distValsExterior, distSqMinExterior);
  NMP::Vector3 entryIndicesVecExterior;
  uint32_t* entryIndicesExterior = (uint32_t*)&entryIndicesVecExterior;
  NMP::vpu::store4f((float*)entryIndicesExterior, resultEntriesExterior);
  NMP::Vector3 RExterior[2];
  NMP::vpu::store4f((float*)&RExterior[0], resultXExterior);
  NMP::vpu::store4f((float*)&RExterior[1], resultYExterior);
  NMP::Vector3 XVecExterior[2];
  NMP::vpu::store4f((float*)&XVecExterior[0], XMinXExterior);
  NMP::vpu::store4f((float*)&XVecExterior[1], XMinYExterior);

  //------------------------
  // Find the first valid entry in the block of four interior samples
  uint32_t index = 0;
  for (; index < 4; ++index)
  {
    if (childNodeFlagsInterior[index])
      break;
  }

  // Determine projection details
  uint32_t extIndexMin, subDivIndexMin;
  float RMin[2];
  if (index != 4)
  {
    // INTERIOR: Find the minimum within the block of four
    for (uint32_t i = index + 1; i < 4; ++i)
    {
      // Note invalid entries will be set to FLT_MAX
      if (distValsInterior[i] < distValsInterior[index])
        index = i;
    }

    uint32_t entryIndex = entryIndicesInterior[index];
    extIndexMin = extIndices[entryIndex];
    subDivIndexMin = subDivIndices[entryIndex];
    RMin[0] = RInterior[0][index];
    RMin[1] = RInterior[1][index];
    XMin.set(XVecInterior[0][index], XVecInterior[1][index], 0.0f);
  }
  else
  {
    // EXTERIOR: Find the minimum within the block of four
    index = 0;
    for (uint32_t i = 1; i < 4; ++i)
    {
      if (distValsExterior[i] < distValsExterior[index])
        index = i;
    }

    uint32_t entryIndex = entryIndicesExterior[index];
    extIndexMin = extIndices[entryIndex];
    subDivIndexMin = subDivIndices[entryIndex];
    RMin[0] = RExterior[0][index];
    RMin[1] = RExterior[1][index];
    XMin.set(XVecExterior[0][index], XVecExterior[1][index], 0.0f);
  }

  // Apply the inverse viewport aspect ratio scaling factor
  XMin.x /= attribDef->m_displayScale[0];
  XMin.y /= attribDef->m_displayScale[1];

  //------------------------
  // Interpolate the Barycentric vertex weights
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[extIndexMin];
  const uint16_t* subDivisionEdgeFacetIndices[2];
  subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 0);
  subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 1);
  uint16_t triangleSubSampleIndices[2];
  triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndexMin];
  triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndexMin];

  NMP::vpu::vector4_t weightsA = NMP::vpu::set4f(
    subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[0]],
    subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[0]],
    subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[0]],
    0.0f);
  NMP::vpu::vector4_t weightsB = NMP::vpu::set4f(
    subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[1]],
    subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[1]],
    subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[1]],
    0.0f);
  NMP::vpu::vector4_t weights =
    NMP::vpu::add4f(
    NMP::vpu::mul4f(weightsA, NMP::vpu::set4f(RMin[0])),
    NMP::vpu::mul4f(weightsB, NMP::vpu::set4f(RMin[1])));
  NMP::vpu::store4f((float*)&weightsMin, weights);
}

#else
//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionInAxisForFacetUpdate(
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& samplePIn,
  uint32_t numEntries,
  uint16_t* extIndices,
  uint16_t* subDivIndices,
  uint16_t* pdIndices,
  uint16_t& exteriorTriangleIndexMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // NOTE: we perform projection in the viewport aspect ratio adjusted space.
  NMP::Vector3 sampleP(
    samplePIn.x * attribDef->m_displayScale[0],
    samplePIn.y * attribDef->m_displayScale[1],
    1.0f);

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Quantisation scale and offset (and apply viewport aspect ratio scale)
  NMP::Vector3 qScale(
    attribDef->m_qScale[0] * attribDef->m_displayScale[0],
    attribDef->m_qScale[1] * attribDef->m_displayScale[1],
    0.0f);
  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0] * attribDef->m_displayScale[0],
    attribDef->m_qOffset[1] * attribDef->m_displayScale[1],
    0.0f);

  // Orthogonal complement vectors to the axis aligned projection directions
  NMP::Vector3 Ups[2];
  Ups[0].set(0.0f, 1.0f, 0.0f);
  Ups[1].set(1.0f, 0.0f, 0.0f);

  //------------------------
  // Project the input control onto the closest exterior edge facet sub-division simplex.
  uint16_t extIndexMinInterior = 0xFFFF;
  uint16_t subDivIndexMinInterior = 0xFFFF;
  float distanceSqMinInterior = FLT_MAX;
  float RMinInterior[2] = {0.0f, 0.0f};
  float XMinInterior[2] = {0.0f, 0.0f};

  uint16_t extIndexMinExterior = 0xFFFF;
  uint16_t subDivIndexMinExterior = 0xFFFF;
  float distanceSqMinExterior = FLT_MAX;
  float RMinExterior[2] = {0.0f, 0.0f};
  float XMinExterior[2] = {0.0f, 0.0f};

  for (uint32_t entryIndex = 0; entryIndex < numEntries; ++entryIndex)
  {
    uint16_t extIndex = extIndices[entryIndex];
    uint16_t subDivIndex = subDivIndices[entryIndex];
    uint16_t pdIndex = pdIndices[entryIndex];
    NMP_ASSERT(pdIndex < 2);

    // Get the appropriate triangle sample indices from the attrib data
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
      attribDef->m_exteriorTriangleIndices[extIndex]);

    // Get the appropriate sub-division edge facet table from the manager
    const uint16_t* subDivisionEdgeFacetIndices[2];
    subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 0);
    subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
      attribDef->m_exteriorEdgeFacetIndices[extIndex], 1);

    // Recover the edge facet vertex indices (triangle local indices)
    uint16_t triangleSubSampleIndices[2];
    triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndex];
    triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndex];

    // Recover the corresponding sample indices within the global list
    uint16_t sampleIndices[2];
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];

    // Dequantise the edge facet vertices
    NMP::Vector3 sampleA(
      (float)attribDef->m_samples[0][sampleIndices[0]] * qScale.x + qOffset.x,
      (float)attribDef->m_samples[1][sampleIndices[0]] * qScale.y + qOffset.y,
      0.0f);
    NMP::Vector3 sampleB(
      (float)attribDef->m_samples[0][sampleIndices[1]] * qScale.x + qOffset.x,
      (float)attribDef->m_samples[1][sampleIndices[1]] * qScale.y + qOffset.y,
      0.0f);

    //------------------------
    // Projection vector
    NMP::Vector3 Up = Ups[pdIndex];

    // Compute the projected point on the edge facet:
    // X = alpha * U, where
    // Up^T * (Pa - X) = 0
    NMP::Vector3 U = sampleB - sampleA;
    NMP_ASSERT(U.magnitude() > 1e-4f);
    float M = Up.dot(U);
    if (NMP::nmfabs(M) < 1e-5f)
      continue;

    NMP::Vector3 Pa = sampleP - sampleA;
    float b = Up.dot(Pa);
    float r[2];
    // Recover the Barycentric coordinates for vertices A, B
    r[1] = b / M;
    r[0] = 1.0f - r[1];

    // Determine if the projected point lies within the edge facet
    const float minVal = -1e-3f;
    const float maxVal = 1.0f + 1e-3f;
    if (r[0] >= minVal && r[0] <= maxVal &&
        r[1] >= minVal && r[1] <= maxVal)
    {
      // Recover the projected point
      NMP::Vector3 X = sampleA * r[0] + sampleB * r[1];

      // Compute the squared distance to the input point
      NMP::Vector3 V = sampleP - X;
      float distSqInterior = V.magnitudeSquared();
      if (distSqInterior < distanceSqMinInterior)
      {
        distanceSqMinInterior = distSqInterior;
        extIndexMinInterior = extIndex;
        subDivIndexMinInterior = subDivIndex;
        XMinInterior[0] = X.x;
        XMinInterior[1] = X.y;
        RMinInterior[0] = r[0];
        RMinInterior[1] = r[1];
      }
    }
    else
    {
      // Clamp to the edge facet
      r[0] = NMP::clampValue(r[0], 0.0f, 1.0f);
      r[1] = NMP::clampValue(r[1], 0.0f, 1.0f);

      // Recover the projected point
      NMP::Vector3 X = sampleA * r[0] + sampleB * r[1];

      // Compute the residual distance in the orthogonal complement direction
      NMP::Vector3 V = sampleP - X;
      float d = Up.dot(V);

      // Update the minimum squared error
      float distSqExterior = d * d;
      if (distSqExterior < distanceSqMinExterior)
      {
        distanceSqMinExterior = distSqExterior;
        extIndexMinExterior = extIndex;
        subDivIndexMinExterior = subDivIndex;
        XMinExterior[0] = X.x;
        XMinExterior[1] = X.y;
        RMinExterior[0] = r[0];
        RMinExterior[1] = r[1];
      }
    }
  }

  //------------------------
  // Determine projection details
  uint16_t extIndexMin, subDivIndexMin;
  float RMin[2];
  if (extIndexMinInterior != 0xFFFF)
  {
    extIndexMin = extIndexMinInterior;
    subDivIndexMin = subDivIndexMinInterior;
    RMin[0] = RMinInterior[0];
    RMin[1] = RMinInterior[1];
    XMin.set(XMinInterior[0], XMinInterior[1], 0.0f);
  }
  else
  {
    extIndexMin = extIndexMinExterior;
    subDivIndexMin = subDivIndexMinExterior;
    RMin[0] = RMinExterior[0];
    RMin[1] = RMinExterior[1];
    XMin.set(XMinExterior[0], XMinExterior[1], 0.0f);
  }

  // Remove the viewport display aspect ratio scale
  XMin.x /= attribDef->m_displayScale[0];
  XMin.y /= attribDef->m_displayScale[1];

  //------------------------
  // Interpolate the Barycentric vertex weights
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[extIndexMin];
  const uint16_t* subDivisionEdgeFacetIndices[2];
  subDivisionEdgeFacetIndices[0] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 0);
  subDivisionEdgeFacetIndices[1] = subDivisionDataBlock.getSubDivisionEdgeFacetIndices(
    attribDef->m_exteriorEdgeFacetIndices[extIndexMin], 1);
  uint16_t triangleSubSampleIndices[2];
  triangleSubSampleIndices[0] = subDivisionEdgeFacetIndices[0][subDivIndexMin];
  triangleSubSampleIndices[1] = subDivisionEdgeFacetIndices[1][subDivIndexMin];

  NMP::Vector3 weightsA;
  subDivisionDataBlock.getTriangleSubSampleWeights(
    triangleSubSampleIndices[0],
    weightsA.x,
    weightsA.y,
    weightsA.z);
  NMP::Vector3 weightsB;
  subDivisionDataBlock.getTriangleSubSampleWeights(
    triangleSubSampleIndices[1],
    weightsB.x,
    weightsB.y,
    weightsB.z);
  weightsMin = weightsA * RMin[0] + weightsB * RMin[1];
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
// \brief Function recovers the minimum distance, position and corresponding Barycentric weights
// for the closest entry in the match buffer.
static void scatterBlend2DAnnotationProjectionClosestEdgeFacetSubSample(
  const AttribDataScatterBlend2DDef* attribDef,
  const ScatterBlendProjectionClosestFacetSubSampleBuffer* matchBuffer,
  uint16_t& exteriorTriangleIndexMin,
  float& distanceSqMin,
  NMP::Vector3& XMin,
  NMP::Vector3& weightsMin)
{
  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  NMP_ASSERT(matchBuffer);
  const ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry* matchEntry = matchBuffer->getSortedEntry(0);
  distanceSqMin = matchEntry->m_distanceSq;
  exteriorTriangleIndexMin = attribDef->m_exteriorTriangleIndices[matchEntry->m_extIndex];

  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);
  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  // Recover the sub-sample point
  const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(
    exteriorTriangleIndexMin);
  uint16_t sampleIndex = triangleSampleIndices[matchEntry->m_subDivIndex];
  NMP::Vector3 qVal(
    (float)attribDef->m_samples[0][sampleIndex],
    (float)attribDef->m_samples[1][sampleIndex],
    0.0f);
  XMin = qVal * qScale + qOffset;

  // Recover the tetrahedron Barycentric weights
  subDivisionDataBlock.getTriangleSubSampleWeights(
    matchEntry->m_subDivIndex,
    weightsMin.x,
    weightsMin.y,
    weightsMin.z);
}

//----------------------------------------------------------------------------------------------------------------------
static void scatterBlend2DAnnotationProjectionInAxisForFacet(
  NMP::TempMemoryAllocator* tempDataAllocator,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& inputMotionParameters,
  ScatterBlend2DParameteriserInfo& result)
{
  // Compute the list of exterior edge facet (extIndex, subDivIndex) pairs to process.
  // Use back-face culling to remove the invalid edge facets.
  NMP::TempMemoryAllocator* childAllocator = tempDataAllocator->createChildAllocator();
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);
  uint16_t entryBufferSize = attribDef->m_numExteriorEdgeFacets * subDivisionDataBlock.m_numSubSimplexesPerEdgeFacet;
  uint16_t entryBufferSize4 = NMP::nmAlignedValue4(entryBufferSize);
  NMP::Memory::Format memReqs(sizeof(uint16_t) * entryBufferSize4, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResT = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  NMP::Memory::Resource memResST = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  NMP::Memory::Resource memResPD = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  uint16_t* extIndices = (uint16_t*)memResT.ptr;
  uint16_t* subDivIndices = (uint16_t*)memResST.ptr;
  uint16_t* projDirs = (uint16_t*)memResPD.ptr;
  uint32_t numEntries = scatterBlend2DAnnotationProjectionBackFaceCull(
    attribDef,
    inputMotionParameters,
    extIndices,
    subDivIndices,
    projDirs);

  uint16_t exteriorTriangleIndexMin = 0;
  NMP::Vector3 XMin;
  NMP::Vector3 weightsMin;

  if (numEntries > 0)
  {
    // Project the input control onto the annotation using per facet projection
    // axis directions.
    scatterBlend2DAnnotationProjectionInAxisForFacetUpdate(
      attribDef,
      inputMotionParameters,
      numEntries,
      extIndices,
      subDivIndices,
      projDirs,
      exteriorTriangleIndexMin,
      XMin,
      weightsMin);
  }
  else
  {
    // All exterior edge facets are back faces. This can happen if the annotation
    // contains degenerate triangles that overlap. Deal with this gracefully at runtime
    // by finding the closest edge facet sub-sample
    NMP::Memory::Format memReqsCEF = ScatterBlendProjectionClosestFacetSubSampleBuffer::getMemoryRequirements(attribDef->m_numExteriorEdgeFacets);
    NMP::Memory::Resource memResCEF = NMPAllocatorAllocateFromFormat(childAllocator, memReqsCEF);
    NMP_ASSERT(memResCEF.ptr);
    ScatterBlendProjectionClosestFacetSubSampleBuffer* matchBuffer = ScatterBlendProjectionClosestFacetSubSampleBuffer::init(memResCEF, attribDef->m_numExteriorEdgeFacets);

    // Compute the closest matches from the input point to the exterior edge facet sub-samples
    scatterBlend2DAnnotationProjectionClosestApproachBuffer(
      attribDef,
      inputMotionParameters,
      matchBuffer);

    matchBuffer->sort1();

    float distanceSqMin;
    scatterBlend2DAnnotationProjectionClosestEdgeFacetSubSample(
      attribDef,
      matchBuffer,
      exteriorTriangleIndexMin,
      distanceSqMin,
      XMin,
      weightsMin);

    // Tidy up
    childAllocator->memFree(matchBuffer);
  }

  //------------------------
  // Set the result
  scatterBlend2DAnnotationProjectionSetResult(
    result,
    nodeDef,
    attribDef,
    exteriorTriangleIndexMin,
    XMin,
    weightsMin);

  // Tidy up the back-face culling buffer
  childAllocator->memFree(extIndices);
  childAllocator->memFree(subDivIndices);
  childAllocator->memFree(projDirs);
  tempDataAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
typedef void (*ScatterBlend2DAnnotationProjectionFn)(
  NMP::TempMemoryAllocator* tempDataAllocator,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const NMP::Vector3& inputMotionParameters,
  ScatterBlend2DParameteriserInfo& result);

static ScatterBlend2DAnnotationProjectionFn s_scatterBlend2DAnnotationProjectionFns[MR::NumScatterBlend2DProjectionModes] =
{
  scatterBlend2DAnnotationProjectionClosestApproach,
  scatterBlend2DAnnotationProjectionTowardsTarget,
  scatterBlend2DAnnotationProjectionInDirection,
  scatterBlend2DAnnotationProjectionInAxisForFacet
};

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
// Core triangle search inside the annotation
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend2DComputeTriangleBlendWeightsForInputControl(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,  
  const NMP::Vector3& sampleP,
  ScatterBlend2DParameteriserInfo& result,
  uint16_t numEntries,
  const uint16_t* triangleIndices,
  const uint16_t* subTriangleIndices)
{
  NMP_ASSERT(attribDef);
  NMP_ASSERT(attribDef->m_numVertices == nodeDef->getNumChildNodes());
  NMP_ASSERT(numEntries > 0);
  NMP_ASSERT(triangleIndices);
  NMP_ASSERT(subTriangleIndices);

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  // Quantisation scale and offset
  NMP::vpu::vector4_t qScalesX = NMP::vpu::set4f(attribDef->m_qScale[0]);
  NMP::vpu::vector4_t qScalesY = NMP::vpu::set4f(attribDef->m_qScale[1]);
  NMP::vpu::vector4_t qOffsetsX = NMP::vpu::set4f(attribDef->m_qOffset[0]);
  NMP::vpu::vector4_t qOffsetsY = NMP::vpu::set4f(attribDef->m_qOffset[1]);

  // Tolerances
  NMP::vpu::vector4_t minVals = NMP::vpu::set4f(-1e-3f);
  NMP::vpu::vector4_t maxVals = NMP::vpu::set4f(1.0f + 1e-3f);

  // Multiplexed input sample
  NMP::vpu::vector4_t samplePX = NMP::vpu::set4f(sampleP.x);
  NMP::vpu::vector4_t samplePY = NMP::vpu::set4f(sampleP.y);

  // Multiplexed result data
  NMP::vpu::vector4_t resultFlags = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultEntries = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultX = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultY = NMP::vpu::zero4f();
  NMP::vpu::vector4_t resultZ = NMP::vpu::zero4f();

  // Iterate over the input triangle data
  for (uint16_t entryIndex = 0; entryIndex < numEntries; entryIndex += 4)
  {
    // Entry indices to process
    NMP::Vector3 entryIndicesVec;
    uint32_t* entryIndices = (uint32_t*)&entryIndicesVec;
    entryIndices[0] = entryIndex;
    entryIndices[1] = entryIndex + 1;
    entryIndices[2] = entryIndex + 2;
    entryIndices[3] = entryIndex + 3;

    uint32_t triangleSubSampleIndices[3];
    uint32_t sampleIndices[3];
    const uint16_t* triangleSampleIndices;
    uint16_t triangleIndex;
    uint16_t subTriangleIndex;

    //------------------------
    // Recover the vertex sample indices for the triangle
    triangleIndex = triangleIndices[entryIndices[0]];
    subTriangleIndex = subTriangleIndices[entryIndices[0]];
    NMP_ASSERT(triangleIndex < attribDef->m_numTriangles);
    NMP_ASSERT(subTriangleIndex < subDivisionDataBlock.m_numSubSimplexesPerTriangle);
    triangleSampleIndices = attribDef->getTriangleSampleIndices(triangleIndex);
    // Get the triangle local sub-sample indices for the sub-division triangle.
    triangleSubSampleIndices[0] = subDivisionDataBlock.m_subDivisionTriangleIndices[0][subTriangleIndex];
    triangleSubSampleIndices[1] = subDivisionDataBlock.m_subDivisionTriangleIndices[1][subTriangleIndex];
    triangleSubSampleIndices[2] = subDivisionDataBlock.m_subDivisionTriangleIndices[2][subTriangleIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];
    sampleIndices[2] = triangleSampleIndices[triangleSubSampleIndices[2]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);
    NMP::vpu::vector4_t qValC0 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[2]],
      attribDef->m_samples[1][sampleIndices[2]],
      0,
      0);

    //------------------------
    // Recover the vertex sample indices for the triangle
    triangleIndex = triangleIndices[entryIndices[1]];
    subTriangleIndex = subTriangleIndices[entryIndices[1]];
    NMP_ASSERT(triangleIndex < attribDef->m_numTriangles);
    NMP_ASSERT(subTriangleIndex < subDivisionDataBlock.m_numSubSimplexesPerTriangle);
    triangleSampleIndices = attribDef->getTriangleSampleIndices(triangleIndex);
    // Get the triangle local sub-sample indices for the sub-division triangle.
    triangleSubSampleIndices[0] = subDivisionDataBlock.m_subDivisionTriangleIndices[0][subTriangleIndex];
    triangleSubSampleIndices[1] = subDivisionDataBlock.m_subDivisionTriangleIndices[1][subTriangleIndex];
    triangleSubSampleIndices[2] = subDivisionDataBlock.m_subDivisionTriangleIndices[2][subTriangleIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];
    sampleIndices[2] = triangleSampleIndices[triangleSubSampleIndices[2]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);
    NMP::vpu::vector4_t qValC1 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[2]],
      attribDef->m_samples[1][sampleIndices[2]],
      0,
      0);

    //------------------------
    // Recover the vertex sample indices for the triangle
    triangleIndex = triangleIndices[entryIndices[2]];
    subTriangleIndex = subTriangleIndices[entryIndices[2]];
    NMP_ASSERT(triangleIndex < attribDef->m_numTriangles);
    NMP_ASSERT(subTriangleIndex < subDivisionDataBlock.m_numSubSimplexesPerTriangle);
    triangleSampleIndices = attribDef->getTriangleSampleIndices(triangleIndex);
    // Get the triangle local sub-sample indices for the sub-division triangle.
    triangleSubSampleIndices[0] = subDivisionDataBlock.m_subDivisionTriangleIndices[0][subTriangleIndex];
    triangleSubSampleIndices[1] = subDivisionDataBlock.m_subDivisionTriangleIndices[1][subTriangleIndex];
    triangleSubSampleIndices[2] = subDivisionDataBlock.m_subDivisionTriangleIndices[2][subTriangleIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];
    sampleIndices[2] = triangleSampleIndices[triangleSubSampleIndices[2]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);
    NMP::vpu::vector4_t qValC2 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[2]],
      attribDef->m_samples[1][sampleIndices[2]],
      0,
      0);

    //------------------------
    // Recover the vertex sample indices for the triangle
    triangleIndex = triangleIndices[entryIndices[3]];
    subTriangleIndex = subTriangleIndices[entryIndices[3]];
    NMP_ASSERT(triangleIndex < attribDef->m_numTriangles);
    NMP_ASSERT(subTriangleIndex < subDivisionDataBlock.m_numSubSimplexesPerTriangle);
    triangleSampleIndices = attribDef->getTriangleSampleIndices(triangleIndex);
    // Get the triangle local sub-sample indices for the sub-division triangle.
    triangleSubSampleIndices[0] = subDivisionDataBlock.m_subDivisionTriangleIndices[0][subTriangleIndex];
    triangleSubSampleIndices[1] = subDivisionDataBlock.m_subDivisionTriangleIndices[1][subTriangleIndex];
    triangleSubSampleIndices[2] = subDivisionDataBlock.m_subDivisionTriangleIndices[2][subTriangleIndex];
    // Recover the corresponding sample indices within the global list    
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];
    sampleIndices[2] = triangleSampleIndices[triangleSubSampleIndices[2]];

    // Recover the quantised samples
    NMP::vpu::vector4_t qValA3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[0]],
      attribDef->m_samples[1][sampleIndices[0]],
      0,
      0);
    NMP::vpu::vector4_t qValB3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[1]],
      attribDef->m_samples[1][sampleIndices[1]],
      0,
      0);
    NMP::vpu::vector4_t qValC3 = NMP::vpu::set4i(
      attribDef->m_samples[0][sampleIndices[2]],
      attribDef->m_samples[1][sampleIndices[2]],
      0,
      0);

    //------------------------
    // Multiplex and dequantise the sample data
    NMP::vpu::Vector3MP sampleAMP;
    sampleAMP.pack(qValA0, qValA1, qValA2, qValA3);
    sampleAMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.x), qScalesX, qOffsetsX);
    sampleAMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleAMP.y), qScalesY, qOffsetsY);

    NMP::vpu::Vector3MP sampleBMP;
    sampleBMP.pack(qValB0, qValB1, qValB2, qValB3);
    sampleBMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.x), qScalesX, qOffsetsX);
    sampleBMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleBMP.y), qScalesY, qOffsetsY);

    NMP::vpu::Vector3MP sampleCMP;
    sampleCMP.pack(qValC0, qValC1, qValC2, qValC3);
    sampleCMP.x = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleCMP.x), qScalesX, qOffsetsX);
    sampleCMP.y = NMP::vpu::madd4f(NMP::vpu::c2float4i(sampleCMP.y), qScalesY, qOffsetsY);

    //------------------------
    // Multiplexed 2x2 design matrix
    NMP::vpu::vector4_t D00, D01;
    NMP::vpu::vector4_t D10, D11;
    D00 = NMP::vpu::sub4f(sampleAMP.x, sampleCMP.x);
    D01 = NMP::vpu::sub4f(sampleBMP.x, sampleCMP.x);
    D10 = NMP::vpu::sub4f(sampleAMP.y, sampleCMP.y);
    D11 = NMP::vpu::sub4f(sampleBMP.y, sampleCMP.y);
    // Right hand vector
    NMP::vpu::vector4_t b0, b1;
    b0 = NMP::vpu::sub4f(samplePX, sampleCMP.x);
    b1 = NMP::vpu::sub4f(samplePY, sampleCMP.y);

    //------------------------
    // Multiplexed solve
    // Note that the design matrix is composed from sub-triangle vertex samples. We make sure that
    // each triangle is valid (not flattened into a line, or point) during asset compilation
    // so we can always assume that the determinant is non-zero.
    NMP::vpu::vector4_t det = NMP::vpu::sub4f(NMP::vpu::mul4f(D00, D11), NMP::vpu::mul4f(D10, D01));
    NMP::vpu::vector4_t recipDet = NMP::vpu::rcp4f(det);
    b0 = NMP::vpu::mul4f(b0, recipDet);
    b1 = NMP::vpu::mul4f(b1, recipDet);
    NMP::vpu::vector4_t rx, ry, rz;
    rx = NMP::vpu::sub4f(NMP::vpu::mul4f(D11, b0), NMP::vpu::mul4f(D01, b1));
    ry = NMP::vpu::sub4f(NMP::vpu::mul4f(D00, b1), NMP::vpu::mul4f(D10, b0));
    rz = NMP::vpu::sub4f(NMP::vpu::sub4f(NMP::vpu::one4f(), rx), ry);

    //------------------------
    // Check if the input control lies within the sub-division tetrahedron
    // (avoiding conditional branches)
    NMP::vpu::vector4_t maskRxA = NMP::vpu::mask4cmpLT(rx, minVals); // rx < minVal
    NMP::vpu::vector4_t maskRxB = NMP::vpu::mask4cmpLT(rx, maxVals); // rx < maxVal
    NMP::vpu::vector4_t maskRyA = NMP::vpu::mask4cmpLT(ry, minVals); // ry < minVal
    NMP::vpu::vector4_t maskRyB = NMP::vpu::mask4cmpLT(ry, maxVals); // ry < maxVal
    NMP::vpu::vector4_t maskRzA = NMP::vpu::mask4cmpLT(rz, minVals); // rz < minVal
    NMP::vpu::vector4_t maskRzB = NMP::vpu::mask4cmpLT(rz, maxVals); // rz < maxVal
    NMP::vpu::vector4_t maskRx = NMP::vpu::and4(NMP::vpu::not4(maskRxA), maskRxB); // rx >= minVal && rx < maxVal
    NMP::vpu::vector4_t maskRy = NMP::vpu::and4(NMP::vpu::not4(maskRyA), maskRyB); // ry >= minVal && ry < maxVal
    NMP::vpu::vector4_t maskRz = NMP::vpu::and4(NMP::vpu::not4(maskRzA), maskRzB); // rz >= minVal && rz < maxVal
    NMP::vpu::vector4_t maskR = NMP::vpu::and4(NMP::vpu::and4(maskRx, maskRy), maskRz);

    //------------------------
    // Update the multiplexed result based on the condition mask
    resultFlags = NMP::vpu::sel4cmpMask(maskR, maskR, resultFlags);
    resultEntries = NMP::vpu::sel4cmpMask(maskR, NMP::vpu::load4f((float*)entryIndices), resultEntries);
    resultX = NMP::vpu::sel4cmpMask(maskR, rx, resultX);
    resultY = NMP::vpu::sel4cmpMask(maskR, ry, resultY);
    resultZ = NMP::vpu::sel4cmpMask(maskR, rz, resultZ);
  }

  //------------------------
  // Store the multiplexed data
  NMP::Vector3 childNodeFlagsVec;
  uint32_t* childNodeFlags = (uint32_t*)childNodeFlagsVec.getPtr();
  NMP::vpu::store4f((float*)childNodeFlags, resultFlags);

  // Demultiplex the result data if the input control lies within a triangle
  for (uint16_t i = 0; i < 4; ++i)
  {
    if (childNodeFlags[i])
    {
      NMP::Vector3 entryIndicesVec;
      uint32_t* entryIndices = (uint32_t*)&entryIndicesVec;
      NMP::vpu::store4f((float*)entryIndices, resultEntries);
      NMP::Vector3 R[3];
      NMP::vpu::store4f((float*)&R[0], resultX);
      NMP::vpu::store4f((float*)&R[1], resultY);
      NMP::vpu::store4f((float*)&R[2], resultZ);

      uint16_t triangleIndex = triangleIndices[entryIndices[i]];
      uint16_t subTriangleIndex = subTriangleIndices[entryIndices[i]];

      // Set the motion parameters
      result.m_achievedParameters[0] = sampleP.x;
      result.m_achievedParameters[1] = sampleP.y;

      // Set the blending sources
      NMP_ASSERT(triangleIndex < attribDef->m_numTriangles);
      const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(triangleIndex);
      uint16_t triangleVertexIndices[3];
      triangleVertexIndices[0] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[0]];
      triangleVertexIndices[1] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[1]];
      triangleVertexIndices[2] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[2]];
      NMP_ASSERT(triangleVertexIndices[0] < attribDef->m_numVertices);
      NMP_ASSERT(triangleVertexIndices[1] < attribDef->m_numVertices);
      NMP_ASSERT(triangleVertexIndices[2] < attribDef->m_numVertices);
      result.m_childNodeIDs[0] = nodeDef->getChildNodeID(triangleVertexIndices[0]);
      result.m_childNodeIDs[1] = nodeDef->getChildNodeID(triangleVertexIndices[1]);
      result.m_childNodeIDs[2] = nodeDef->getChildNodeID(triangleVertexIndices[2]);

      // Interpolate the Barycentric vertex weights
      uint32_t triangleSubSampleIndices[3];
      triangleSubSampleIndices[0] = subDivisionDataBlock.m_subDivisionTriangleIndices[0][subTriangleIndex];
      triangleSubSampleIndices[1] = subDivisionDataBlock.m_subDivisionTriangleIndices[1][subTriangleIndex];
      triangleSubSampleIndices[2] = subDivisionDataBlock.m_subDivisionTriangleIndices[2][subTriangleIndex];
      NMP::vpu::vector4_t weightsA = NMP::vpu::set4f(
        subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[0]],
        subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[0]],
        subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[0]],
        0.0f);
      NMP::vpu::vector4_t weightsB = NMP::vpu::set4f(
        subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[1]],
        subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[1]],
        subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[1]],
        0.0f);
      NMP::vpu::vector4_t weightsC = NMP::vpu::set4f(
        subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[2]],
        subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[2]],
        subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[2]],
        0.0f);
      NMP::vpu::vector4_t weights =
        NMP::vpu::add4f(
        NMP::vpu::add4f(
        NMP::vpu::mul4f(weightsA, NMP::vpu::set4f(R[0][i])),
        NMP::vpu::mul4f(weightsB, NMP::vpu::set4f(R[1][i]))),
        NMP::vpu::mul4f(weightsC, NMP::vpu::set4f(R[2][i])));

      NMP::Vector3 W;
      NMP::vpu::store4f((float*)&W, weights);
      result.m_childNodeWeights[0] = W.x;
      result.m_childNodeWeights[1] = W.y;
      result.m_childNodeWeights[2] = W.z;

      return true;
    }
  }

  return false;
}

#else
//----------------------------------------------------------------------------------------------------------------------
static bool scatterBlend2DComputeTriangleBlendWeightsForInputControl(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,  
  const NMP::Vector3& sampleP,
  ScatterBlend2DParameteriserInfo& result,
  uint16_t numEntries,
  const uint16_t* triangleIndices,
  const uint16_t* subTriangleIndices)
{
  NMP_ASSERT(attribDef);
  NMP_ASSERT(attribDef->m_numVertices == nodeDef->getNumChildNodes());
  NMP_ASSERT(numEntries > 0);
  NMP_ASSERT(triangleIndices);
  NMP_ASSERT(subTriangleIndices);

  // Set the motion parameters
  result.m_achievedParameters[0] = sampleP.x;
  result.m_achievedParameters[1] = sampleP.y;

  // Quantisation scale and offset
  NMP::Vector3 qScale(
    attribDef->m_qScale[0],
    attribDef->m_qScale[1],
    0.0f);
  NMP::Vector3 qOffset(
    attribDef->m_qOffset[0],
    attribDef->m_qOffset[1],
    0.0f);

  // Get the sub-division data block from the sub-sample manager
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  for (uint16_t entryIndex = 0; entryIndex < numEntries; ++entryIndex)
  {
    uint16_t triangleIndex = triangleIndices[entryIndex];
    uint16_t subTriangleIndex = subTriangleIndices[entryIndex];

    // Recover the vertex sample indices for the triangle
    NMP_ASSERT(triangleIndex < attribDef->m_numTriangles);
    const uint16_t* triangleSampleIndices = attribDef->getTriangleSampleIndices(triangleIndex);
    uint16_t triangleVertexIndices[3];
    triangleVertexIndices[0] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[0]];
    triangleVertexIndices[1] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[1]];
    triangleVertexIndices[2] = triangleSampleIndices[subDivisionDataBlock.m_vertexSubSampleIndices[2]];
    NMP_ASSERT(triangleVertexIndices[0] < attribDef->m_numVertices);
    NMP_ASSERT(triangleVertexIndices[1] < attribDef->m_numVertices);
    NMP_ASSERT(triangleVertexIndices[2] < attribDef->m_numVertices);

    // Get the triangle local sub-sample indices for the sub-division triangle.
    uint16_t triangleSubSampleIndices[3];
    triangleSubSampleIndices[0] = subDivisionDataBlock.m_subDivisionTriangleIndices[0][subTriangleIndex];
    triangleSubSampleIndices[1] = subDivisionDataBlock.m_subDivisionTriangleIndices[1][subTriangleIndex];
    triangleSubSampleIndices[2] = subDivisionDataBlock.m_subDivisionTriangleIndices[2][subTriangleIndex];

    // Recover the corresponding sample indices within the global list
    uint16_t sampleIndices[3];
    sampleIndices[0] = triangleSampleIndices[triangleSubSampleIndices[0]];
    sampleIndices[1] = triangleSampleIndices[triangleSubSampleIndices[1]];
    sampleIndices[2] = triangleSampleIndices[triangleSubSampleIndices[2]];

    // Dequantise the sample data
    NMP::Vector3 sampleA;
    sampleA.x = (float)attribDef->m_samples[0][sampleIndices[0]] * qScale.x + qOffset.x;
    sampleA.y = (float)attribDef->m_samples[1][sampleIndices[0]] * qScale.y + qOffset.y;

    NMP::Vector3 sampleB;
    sampleB.x = (float)attribDef->m_samples[0][sampleIndices[1]] * qScale.x + qOffset.x;
    sampleB.y = (float)attribDef->m_samples[1][sampleIndices[1]] * qScale.y + qOffset.y;

    NMP::Vector3 sampleC;
    sampleC.x = (float)attribDef->m_samples[0][sampleIndices[2]] * qScale.x + qOffset.x;
    sampleC.y = (float)attribDef->m_samples[1][sampleIndices[2]] * qScale.y + qOffset.y;

    //------------------------
    // Compute the Barycentric weights
    float M[4];
    M[0] = sampleA.x - sampleC.x;
    M[1] = sampleB.x - sampleC.x;
    M[2] = sampleA.y - sampleC.y;
    M[3] = sampleB.y - sampleC.y;
    float b[2];
    b[0] = sampleP.x - sampleC.x;
    b[1] = sampleP.y - sampleC.y;
    float det = M[0] * M[3] - M[2] * M[1];
    NMP_ASSERT(det != 0.0f);
    float recipDet = 1.0f / det;
    b[0] *= recipDet;
    b[1] *= recipDet;
    float r[3];
    r[0] = M[3] * b[0] - M[1] * b[1];
    r[1] = M[0] * b[1] - M[2] * b[0];
    r[2] = 1.0f - r[0] - r[1];

    //------------------------
    // Check if the input control lies within the sub-division triangle
    const float minVal = -1e-3f;
    const float maxVal = 1.0f + 1e-3f;
    if (r[0] >= minVal && r[0] <= maxVal &&
        r[1] >= minVal && r[1] <= maxVal &&
        r[2] >= minVal && r[2] <= maxVal)
    {
      // Recover the Barycentric weights assigned to the vertices of the sub-division triangle.
      // i.e. the weighting contribution of the triangle vertex sources assigned to each of
      // the sub-division triangle vertices.
      NMP::Vector3 weightsA(
        subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[0]],
        subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[0]],
        subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[0]]);

      NMP::Vector3 weightsB(
        subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[1]],
        subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[1]],
        subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[1]]);

      NMP::Vector3 weightsC(
        subDivisionDataBlock.m_subSampleWeights[0][triangleSubSampleIndices[2]],
        subDivisionDataBlock.m_subSampleWeights[1][triangleSubSampleIndices[2]],
        subDivisionDataBlock.m_subSampleWeights[2][triangleSubSampleIndices[2]]);

      // Interpolate the Barycentric vertex weights: Note vector3 multiply ignores the w components
      NMP::Vector3 weights = weightsA * r[0] + weightsB * r[1] + weightsC * r[2];

      // Set the Barycentric blend weights
      result.m_childNodeWeights[0] = weights.x;
      result.m_childNodeWeights[1] = weights.y;
      result.m_childNodeWeights[2] = weights.z;

      // Set the blending sources
      result.m_childNodeIDs[0] = nodeDef->getChildNodeID(triangleVertexIndices[0]);
      result.m_childNodeIDs[1] = nodeDef->getChildNodeID(triangleVertexIndices[1]);
      result.m_childNodeIDs[2] = nodeDef->getChildNodeID(triangleVertexIndices[2]);

      return true;
    }
  }

  return false;
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
// Triangle index unpacking functions for the core triangle search inside the annotation
//----------------------------------------------------------------------------------------------------------------------
static uint16_t scatterBlend2DUnpackTriangleIndicesU8U0(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t bspLeafNodeEntryIndex,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices)
{
  // 8-bits
  const uint8_t* leafNodeTriangleIndices = (const uint8_t*)attribDef->m_bspLeafNodeTriangleIndices;
  // 1-bit (flags are not stored)
  NMP_ASSERT(!attribDef->m_bspLeafNodeSubTriangleFlags);
  uint16_t numEntries = 0;
  uint16_t nodeEntryOffsetsBegin = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex];
  uint16_t nodeEntryOffsetsEnd = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex + 1];
  for (uint16_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
  {
    triangleIndices[numEntries] = (uint16_t)leafNodeTriangleIndices[nodeEntryOffset];
    subTriangleIndices[numEntries] = 0;
    numEntries++;
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
static uint16_t scatterBlend2DUnpackTriangleIndicesU16U0(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t bspLeafNodeEntryIndex,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices)
{
  // 16-bits
  const uint16_t* leafNodeTriangleIndices = (const uint16_t*)attribDef->m_bspLeafNodeTriangleIndices;
  // 1-bit (flags are not stored)
  NMP_ASSERT(!attribDef->m_bspLeafNodeSubTriangleFlags);
  uint16_t numEntries = 0;
  uint16_t nodeEntryOffsetsBegin = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex];
  uint16_t nodeEntryOffsetsEnd = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex + 1];
  for (uint16_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
  {
    triangleIndices[numEntries] = leafNodeTriangleIndices[nodeEntryOffset];
    subTriangleIndices[numEntries] = 0;
    numEntries++;
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
static uint16_t scatterBlend2DUnpackTriangleIndicesU8U8(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t bspLeafNodeEntryIndex,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices)
{
  // 8-bits
  const uint8_t* leafNodeTriangleIndices = (const uint8_t*)attribDef->m_bspLeafNodeTriangleIndices;
  // 8-bits
  const uint8_t* subTriangleFlags = (const uint8_t*)attribDef->m_bspLeafNodeSubTriangleFlags;
  uint16_t numEntries = 0;
  uint16_t nodeEntryOffsetsBegin = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex];
  uint16_t nodeEntryOffsetsEnd = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex + 1];
  NMP_ASSERT(nodeEntryOffsetsEnd > nodeEntryOffsetsBegin);
  for (uint16_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
  {
    uint16_t triangleIndex = (uint16_t)leafNodeTriangleIndices[nodeEntryOffset];
    uint16_t flags = (uint16_t)subTriangleFlags[nodeEntryOffset];
    for (uint16_t i = 0; i < 4; ++i)
    {
      triangleIndices[numEntries] = triangleIndex;
      subTriangleIndices[numEntries] = i;
      numEntries += ((flags >> i) & 0x01);
    }
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
static uint16_t scatterBlend2DUnpackTriangleIndicesU16U8(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t bspLeafNodeEntryIndex,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices)
{
  // 16-bits
  const uint16_t* leafNodeTriangleIndices = (const uint16_t*)attribDef->m_bspLeafNodeTriangleIndices;
  // 8-bits
  const uint8_t* subTriangleFlags = (const uint8_t*)attribDef->m_bspLeafNodeSubTriangleFlags;
  uint16_t numEntries = 0;
  uint16_t nodeEntryOffsetsBegin = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex];
  uint16_t nodeEntryOffsetsEnd = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex + 1];
  for (uint16_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
  {
    uint16_t triangleIndex = leafNodeTriangleIndices[nodeEntryOffset];
    uint16_t flags = (uint16_t)subTriangleFlags[nodeEntryOffset];
    for (uint16_t i = 0; i < 4; ++i)
    {
      triangleIndices[numEntries] = triangleIndex;
      subTriangleIndices[numEntries] = i;
      numEntries += ((flags >> i) & 0x01);
    }
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
static uint16_t scatterBlend2DUnpackTriangleIndicesU8U16(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t bspLeafNodeEntryIndex,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices)
{
  // 8-bits
  const uint8_t* leafNodeTriangleIndices = (const uint8_t*)attribDef->m_bspLeafNodeTriangleIndices;
  // 16-bits
  const uint16_t* subTriangleFlags = (const uint16_t*)attribDef->m_bspLeafNodeSubTriangleFlags;
  uint16_t numEntries = 0;
  uint16_t nodeEntryOffsetsBegin = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex];
  uint16_t nodeEntryOffsetsEnd = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex + 1];
  for (uint16_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
  {
    uint16_t triangleIndex = (uint16_t)leafNodeTriangleIndices[nodeEntryOffset];
    uint16_t flags = subTriangleFlags[nodeEntryOffset];
    for (uint16_t i = 0; i < 16; ++i)
    {
      triangleIndices[numEntries] = triangleIndex;
      subTriangleIndices[numEntries] = i;
      numEntries += ((flags >> i) & 0x01);
    }
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
static uint16_t scatterBlend2DUnpackTriangleIndicesU16U16(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t bspLeafNodeEntryIndex,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices)
{
  // 16-bits
  const uint16_t* leafNodeTriangleIndices = (const uint16_t*)attribDef->m_bspLeafNodeTriangleIndices;
  // 16-bits
  const uint16_t* subTriangleFlags = (const uint16_t*)attribDef->m_bspLeafNodeSubTriangleFlags;
  uint16_t numEntries = 0;
  uint16_t nodeEntryOffsetsBegin = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex];
  uint16_t nodeEntryOffsetsEnd = attribDef->m_bspLeafNodeEntryOffsets[bspLeafNodeEntryIndex + 1];
  for (uint16_t nodeEntryOffset = nodeEntryOffsetsBegin; nodeEntryOffset < nodeEntryOffsetsEnd; ++nodeEntryOffset)
  {
    uint16_t triangleIndex = leafNodeTriangleIndices[nodeEntryOffset];
    uint16_t flags = subTriangleFlags[nodeEntryOffset];
    for (uint16_t i = 0; i < 16; ++i)
    {
      triangleIndices[numEntries] = triangleIndex;
      subTriangleIndices[numEntries] = i;
      numEntries += ((flags >> i) & 0x01);
    }
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
static uint16_t scatterBlend2DUnpackTriangleIndicesAll(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices)
{
  const MR::ScatterBlend2DSubSampleManager::SubDivisionDataBlock& subDivisionDataBlock =
    MR::ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(attribDef->m_numTriangleSubDivisions);

  uint16_t numEntries = 0;
  for (uint16_t triangleIndex = 0; triangleIndex < attribDef->m_numTriangles; ++triangleIndex)
  {
    for (uint16_t subTriangleIndx = 0; subTriangleIndx < subDivisionDataBlock.m_numSubSimplexesPerTriangle; ++subTriangleIndx)
    {
      triangleIndices[numEntries] = triangleIndex;
      subTriangleIndices[numEntries] = subTriangleIndx;
      numEntries++;
    }
  }

  return numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
typedef uint16_t (*ScatterBlend2DUnpackTriangleIndicesFn)(
  const AttribDataScatterBlend2DDef* attribDef,
  uint16_t bspLeafNodeEntryIndex,
  uint16_t* triangleIndices,
  uint16_t* subTriangleIndices
  );

static ScatterBlend2DUnpackTriangleIndicesFn s_scatterBlend2DUnpackTriangleIndicesFns[6] =
{
  scatterBlend2DUnpackTriangleIndicesU8U0,
  scatterBlend2DUnpackTriangleIndicesU8U8,
  scatterBlend2DUnpackTriangleIndicesU8U16,
  scatterBlend2DUnpackTriangleIndicesU16U0,
  scatterBlend2DUnpackTriangleIndicesU16U8,
  scatterBlend2DUnpackTriangleIndicesU16U16
};

//----------------------------------------------------------------------------------------------------------------------
// ScatterBlend2D API
//----------------------------------------------------------------------------------------------------------------------
void scatterBlend2DComputeChildNodeWeightsForInputControl(
  Network* net,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const float* inputMotionParameters,
  ScatterBlend2DParameteriserInfo& result)
{
  NMP_ASSERT(attribDef);
  NMP_ASSERT(inputMotionParameters);
  NMP::TempMemoryAllocator* tempDataAllocator = net->getTempMemoryAllocator();

  // Input motion parameters
  NMP::Vector3 sampleP(
    inputMotionParameters[0],
    inputMotionParameters[1],
    0.0f);
  result.m_desiredParameters[0] = inputMotionParameters[0];
  result.m_desiredParameters[1] = inputMotionParameters[1];

  //------------------------
  // Search the BSP nodes for the partition containing the input control
  NMP::TempMemoryAllocator* childAllocator = tempDataAllocator->createChildAllocator();
  NMP_ASSERT(attribDef->m_bspMaxNumSubTrianglesInPartition > 0);
  uint16_t entryBufferSize = attribDef->m_bspMaxNumSubTrianglesInPartition + 1; // +1 required for unpacking fns
  uint16_t entryBufferSize4 = NMP::nmAlignedValue4(entryBufferSize); 
  NMP::Memory::Format memReqs(sizeof(uint16_t) * entryBufferSize4, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResT = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  NMP::Memory::Resource memResST = NMPAllocatorAllocateFromFormat(childAllocator, memReqs);
  uint16_t* triangleIndices = (uint16_t*)memResT.ptr;
  uint16_t* subTriangleIndices = (uint16_t*)memResST.ptr;

  uint16_t numEntries;
  if (attribDef->hasBSPTree())
  {
    // Quantisation scale and offset
    NMP::Vector3 qScale(
      attribDef->m_qScale[0],
      attribDef->m_qScale[1],
      0.0f);
    NMP::Vector3 qOffset(
      attribDef->m_qOffset[0],
      attribDef->m_qOffset[1],
      0.0f);

    // BSP partition search. Note that we assume an addressing system where the branch
    // nodes precede the leaf nodes.
    uint16_t bspNodeIndex = 0;
    while (bspNodeIndex < attribDef->m_bspNumBranchNodes)
    {
      uint16_t sampleIndices[2];
      sampleIndices[0] = attribDef->m_bspBranchNodeHyperPlaneSampleIndices[0][bspNodeIndex];
      sampleIndices[1] = attribDef->m_bspBranchNodeHyperPlaneSampleIndices[1][bspNodeIndex];
      NMP_ASSERT(sampleIndices[0] < attribDef->m_numVertices + attribDef->m_numSubSamples);
      NMP_ASSERT(sampleIndices[1] < attribDef->m_numVertices + attribDef->m_numSubSamples);

      // Dequantise the sample data
      NMP::Vector3 sampleA;
      sampleA.x = (float)attribDef->m_samples[0][sampleIndices[0]] * qScale.x + qOffset.x;
      sampleA.y = (float)attribDef->m_samples[1][sampleIndices[0]] * qScale.y + qOffset.y;

      NMP::Vector3 sampleB;
      sampleB.x = (float)attribDef->m_samples[0][sampleIndices[1]] * qScale.x + qOffset.x;
      sampleB.y = (float)attribDef->m_samples[1][sampleIndices[1]] * qScale.y + qOffset.y;

      // Recover the hyper-plane
      NMP::Vector3 N;
      N.x = sampleA.y - sampleB.y;
      N.y = sampleB.x - sampleA.x;
      N.w = sampleA.x * sampleB.y - sampleB.x * sampleA.y;

      // Calculate the partition metric and update the node index
      float d = sampleP.x * N.x + sampleP.y * N.y + N.w;
      if (d < 0)
      {
        bspNodeIndex = attribDef->m_bspBranchNodeLeft[bspNodeIndex];
      }
      else
      {
        NMP_ASSERT(bspNodeIndex < attribDef->m_bspNumBranchNodes);
        bspNodeIndex = attribDef->m_bspBranchNodeRight[bspNodeIndex];
      }
      NMP_ASSERT(bspNodeIndex < attribDef->m_bspNumBranchNodes + attribDef->m_bspNumLeafNodes);
    }

    // Compute the leaf node index
    bspNodeIndex -= attribDef->m_bspNumBranchNodes;

    // Unpack the triangle entries to search
    NMP_ASSERT(attribDef->m_bspUnpackTriangleIndicesFn < 6);
    numEntries = s_scatterBlend2DUnpackTriangleIndicesFns[attribDef->m_bspUnpackTriangleIndicesFn](
      attribDef,
      bspNodeIndex,
      triangleIndices,
      subTriangleIndices);
  }
  else
  {
    // There is no BSP tree so we must search all triangles to see if the input control lies
    // within the annotation.
    numEntries = scatterBlend2DUnpackTriangleIndicesAll(
      attribDef,
      triangleIndices,
      subTriangleIndices);
  }

  //------------------------
  // Pad the remaining block of 4 entries with the last value for SIMD
  for (uint32_t entryIndex = numEntries; entryIndex & 0x03; ++entryIndex)
  {
    triangleIndices[entryIndex] = triangleIndices[entryIndex - 1];
    subTriangleIndices[entryIndex] = subTriangleIndices[entryIndex - 1];
  }

  //------------------------
  // Determine if the input control lies within any of the triangles then
  // compute the corresponding triangle blend weights and child node IDs.
  result.m_wasProjected = !scatterBlend2DComputeTriangleBlendWeightsForInputControl(
    nodeDef,
    attribDef,
    sampleP,
    result,
    numEntries,
    triangleIndices,
    subTriangleIndices);

  // Tidy up the unpacked triangle indices
  childAllocator->memFree(triangleIndices);
  childAllocator->memFree(subTriangleIndices);
  tempDataAllocator->destroyChildAllocator(childAllocator);

  //------------------------
  // Project the input control back onto the annotation if it lies outside
  if (result.m_wasProjected)
  {
    NMP_ASSERT(attribDef->m_projectionMode < MR::NumScatterBlend2DProjectionModes);
    s_scatterBlend2DAnnotationProjectionFns[attribDef->m_projectionMode](
      tempDataAllocator,
      nodeDef,
      attribDef,
      sampleP,
      result);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
