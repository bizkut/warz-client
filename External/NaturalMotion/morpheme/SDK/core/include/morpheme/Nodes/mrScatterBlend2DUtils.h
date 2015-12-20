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
#ifndef MR_SCATTER_BLEND_2D_UTILS_H
#define MR_SCATTER_BLEND_2D_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatterBlend2DSubSampleManager
/// \brief A manager for the static sub-division sample data layout
//----------------------------------------------------------------------------------------------------------------------
class ScatterBlend2DSubSampleManager
{
public:
  // Structure 
  class SubDivisionDataBlock
  {
  public:
    // Information
    NM_INLINE uint16_t getNumSubSamplesPerTriangle() const;
    NM_INLINE uint16_t getNumSubSamplesPerEdgeFacet() const;
    NM_INLINE uint16_t getNumSubSimplexesPerTriangle() const;
    NM_INLINE uint16_t getNumSubSimplexesPerEdgeFacet() const;

    NM_INLINE uint16_t getNumInteriorSubSamplesPerEdge() const;
    NM_INLINE uint16_t getNumInteriorSubSamplesPerTriangle() const;

    // Barycentric sub-sample weights
    NM_INLINE const float* getSubSampleWeights(
      uint32_t triangleVertexElementIndex) const;

    NM_INLINE void getTriangleSubSampleWeights(
      uint32_t triangleSubSampleIndex,
      float& weightA,
      float& weightB,
      float& weightC) const;

    // Sub-division triangle indices
    NM_INLINE const uint16_t* getSubDivisionTriangleIndices(
      uint32_t triangleVertexElementIndex) const;

    // Sub-division edge facet indices
    NM_INLINE const uint16_t* getSubDivisionEdgeFacetIndices(
      uint32_t triangleVertexElementIndex,
      uint32_t edgeVertexElementIndex) const;

    // Triangle facet vertex sub-samples
    NM_INLINE const uint16_t* getVertexEdgeFacetIndices(
      uint32_t triangleVertexElementIndex) const;

    // Triangle facet sub-samples
    NM_INLINE const uint16_t* getSubSampleEdgeFacetIndices(
      uint32_t triangleVertexElementIndex) const;

    // Vertex sub-sample indices
    NM_INLINE const uint16_t* getVertexSubSampleIndices() const;

    NM_INLINE uint32_t getVertexSubSampleIndex(
      uint32_t triangleVertexElementIndex) const;

    // Interior edge sub-sample indices
    NM_INLINE const uint16_t* getInteriorEdgeSubSampleIndices(
      uint32_t edgeVertexElementIndex) const;

    // Interior edge sub-sample weights
    NM_INLINE const float* getInteriorEdgeSubSampleWeights(
      uint32_t edgeVertexElementIndex) const;

    NM_INLINE void getInteriorEdgeSubSampleWeight(
      uint32_t elementSubSampleIndex,
      float& weightA,
      float& weightB) const;

    // Interior edge sub-sample permutation map
    NM_INLINE const uint16_t* getInteriorEdgeSubSamplePermutationMap(
      uint32_t permutationIndex) const;

    // Interior triangle sub-sample indices
    NM_INLINE const uint16_t* getInteriorTriangleSubSampleIndices() const;
    NM_INLINE uint32_t getInteriorTriangleSubSampleIndex(
      uint32_t elementSubSampleIndex) const;

    // Triangle sub-sample permutation map
    NM_INLINE const uint16_t* getTriangleSubSamplePermutationMap(
      uint32_t permutationIndex) const;

  public:
    //------------------------
    // Information
    uint16_t          m_numSubSamplesPerTriangle;
    uint16_t          m_numSubSamplesPerEdgeFacet;
    uint16_t          m_numSubSimplexesPerTriangle;
    uint16_t          m_numSubSimplexesPerEdgeFacet;

    uint16_t          m_numInteriorSubSamplesPerEdge;
    uint16_t          m_numInteriorSubSamplesPerTriangle;

    // Barycentric sub-sample weights: A, B, C arrays
    const float*      m_subSampleWeights[3];

    // Sub-division triangle indices: A, B, C arrays
    const uint16_t*   m_subDivisionTriangleIndices[3];

    // Sub-division edge facet indices: A, B arrays for each of the 3 edges
    // opposite the corresponding triangle vertex.
    const uint16_t*   m_subDivisionEdgeFacetIndices[3][2];

    // Vertex sub-sample indices for the edge facets: index arrays for each of the
    // 3 edge facets opposite the corresponding triangle vertex.
    const uint16_t*   m_vertexEdgeFacetIndices[3];

    // Sub-sample indices for the edge facets: index arrays for each of the
    // 3 edge facets opposite the corresponding triangle vertex.
    const uint16_t*   m_subSampleEdgeFacetIndices[3];

    // Vertex sub-sample indices: A, B, C
    const uint16_t*   m_vertexSubSampleIndices;

    // Interior edge sub-sample indices for each of the 3 edges
    // opposite the corresponding triangle vertex. BC, CA, AB
    const uint16_t*   m_interiorEdgeSubSampleIndices[3];

    // Interior edge sub-sample weights
    const float*      m_interiorEdgeSubSampleWeights[2];

    // Interior edge sub-sample permutation map
    const uint16_t*   m_interiorEdgeSubSamplePermutationMap[2];

    // Interior triangle sub-sample indices.
    const uint16_t*   m_interiorTriangleSubSampleIndices;

    // Triangle sub-sample permutation map
    const uint16_t*   m_triangleSubSamplePermutationMap[6];
  };

  NM_INLINE static const SubDivisionDataBlock& getSubDivisionDataBlock(uint32_t subDivisionIndex);

  /// \brief Compute the permutation index for the difference between the
  /// vertex ordering of stored sub-samples and the required vertex ordering.
  static uint32_t getInteriorEdgeVertexOrderPermutation(
    uint32_t srcVertexAID,
    uint32_t srcVertexBID,
    uint32_t destVertexAID,
    uint32_t destVertexBID);

  /// \brief Compute the permutation index for the difference between the
  /// vertex ordering of stored sub-samples and the required vertex ordering.
  /// The source vertex IDs encode the sub-sample ordering of the source data arrays
  /// The dest vertex IDs encode the desired vertex re-ordering the user requires.
  /// The index returned from the function can be used to retrieve the appropriate
  /// permutation map whose entries determine which sub-sample index in the destination
  /// the source component should be mapped to. i.e. the forward mapping.
  static uint32_t getTriangleVertexOrderPermutation(
    uint32_t srcVertexAID,
    uint32_t srcVertexBID,
    uint32_t srcVertexCID,
    uint32_t destVertexAID,
    uint32_t destVertexBID,
    uint32_t destVertexCID);

private:
  // Triangle sub-division data blocks
  static const SubDivisionDataBlock sm_subDivisionDataBlocks[3];
};


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getNumSubSamplesPerTriangle() const
{
  return m_numSubSamplesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getNumSubSamplesPerEdgeFacet() const
{
  return m_numSubSamplesPerEdgeFacet;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getNumSubSimplexesPerTriangle() const
{
  return m_numSubSimplexesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getNumSubSimplexesPerEdgeFacet() const
{
  return m_numSubSimplexesPerEdgeFacet;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getNumInteriorSubSamplesPerEdge() const
{
  return m_numInteriorSubSamplesPerEdge;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getNumInteriorSubSamplesPerTriangle() const
{
  return m_numInteriorSubSamplesPerTriangle;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getSubSampleWeights(
  uint32_t triangleVertexElementIndex) const
{
  NMP_ASSERT(triangleVertexElementIndex < 3);
  return m_subSampleWeights[triangleVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getTriangleSubSampleWeights(
  uint32_t triangleSubSampleIndex,
  float& weightA,
  float& weightB,
  float& weightC) const
{
  NMP_ASSERT(triangleSubSampleIndex < m_numSubSamplesPerTriangle);
  weightA = m_subSampleWeights[0][triangleSubSampleIndex];
  weightB = m_subSampleWeights[1][triangleSubSampleIndex];
  weightC = m_subSampleWeights[2][triangleSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getSubDivisionTriangleIndices(
  uint32_t triangleVertexElementIndex) const
{
  NMP_ASSERT(triangleVertexElementIndex < 3);
  return m_subDivisionTriangleIndices[triangleVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getSubDivisionEdgeFacetIndices(
  uint32_t triangleVertexElementIndex,
  uint32_t edgeVertexElementIndex) const
{
  NMP_ASSERT(triangleVertexElementIndex < 3);
  NMP_ASSERT(edgeVertexElementIndex < 2);
  return m_subDivisionEdgeFacetIndices[triangleVertexElementIndex][edgeVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getVertexEdgeFacetIndices(
  uint32_t triangleVertexElementIndex) const
{
  NMP_ASSERT(triangleVertexElementIndex < 3);
  return m_vertexEdgeFacetIndices[triangleVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getSubSampleEdgeFacetIndices(
  uint32_t triangleVertexElementIndex) const
{
  NMP_ASSERT(triangleVertexElementIndex < 3);
  return m_subSampleEdgeFacetIndices[triangleVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getVertexSubSampleIndices() const
{
  return m_vertexSubSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getVertexSubSampleIndex(
  uint32_t triangleVertexElementIndex) const
{
  NMP_ASSERT(triangleVertexElementIndex < 3);
  return m_vertexSubSampleIndices[triangleVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getInteriorEdgeSubSampleIndices(
  uint32_t edgeElementIndex) const
{
  NMP_ASSERT(edgeElementIndex < 3);
  return m_interiorEdgeSubSampleIndices[edgeElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getInteriorEdgeSubSampleWeights(
  uint32_t edgeVertexElementIndex) const
{
  NMP_ASSERT(edgeVertexElementIndex < 2);
  return m_interiorEdgeSubSampleWeights[edgeVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getInteriorEdgeSubSampleWeight(
  uint32_t elementSubSampleIndex,
  float& weightA,
  float& weightB) const
{
  NMP_ASSERT(elementSubSampleIndex < m_numInteriorSubSamplesPerEdge);
  weightA = m_interiorEdgeSubSampleWeights[0][elementSubSampleIndex];
  weightB = m_interiorEdgeSubSampleWeights[1][elementSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getInteriorEdgeSubSamplePermutationMap(
  uint32_t permutationIndex) const
{
  NMP_ASSERT(permutationIndex < 2);
  return m_interiorEdgeSubSamplePermutationMap[permutationIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getInteriorTriangleSubSampleIndices() const
{
  return m_interiorTriangleSubSampleIndices;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getInteriorTriangleSubSampleIndex(
  uint32_t elementSubSampleIndex) const
{
  NMP_ASSERT(elementSubSampleIndex < m_numInteriorSubSamplesPerTriangle);
  return m_interiorTriangleSubSampleIndices[elementSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* ScatterBlend2DSubSampleManager::SubDivisionDataBlock::getTriangleSubSamplePermutationMap(
  uint32_t permutationIndex) const
{
  NMP_ASSERT(permutationIndex < 6);
  return m_triangleSubSamplePermutationMap[permutationIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const ScatterBlend2DSubSampleManager::SubDivisionDataBlock&
ScatterBlend2DSubSampleManager::getSubDivisionDataBlock(uint32_t subDivisionIndex)
{
  NMP_ASSERT(subDivisionIndex < 3);
  return sm_subDivisionDataBlocks[subDivisionIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void scatterBlend2DComputeChildNodeWeightsForInputControl(
  Network* net,
  const NodeDef* nodeDef,
  const AttribDataScatterBlend2DDef* attribDef,
  const float* inputMotionParameters,
  ScatterBlend2DParameteriserInfo& result);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SCATTER_BLEND_2D_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
