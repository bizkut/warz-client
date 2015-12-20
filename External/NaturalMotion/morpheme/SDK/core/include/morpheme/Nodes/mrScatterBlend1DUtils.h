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
#ifndef MR_SCATTER_BLEND_1D_UTILS_H
#define MR_SCATTER_BLEND_1D_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatterBlend1DSubSampleManager
/// \brief A manager for the static sub-division sample data layout
//----------------------------------------------------------------------------------------------------------------------
class ScatterBlend1DSubSampleManager
{
public:
  // Structure 
  class SubDivisionDataBlock
  {
  public:
    // Information
    NM_INLINE uint16_t getNumSubSamplesPerLine() const;

    // Barycentric sub-sample weights
    NM_INLINE const float* getSubSampleWeights(
      uint32_t lineVertexElementIndex) const;

    NM_INLINE void getLineSubSampleWeights(
      uint32_t lineSubSampleIndex,
      float& weightA,
      float& weightB) const;

  public:
    // Information
    uint16_t          m_numSubSamplesPerLine;

    // Barycentric sub-sample weights: A, B arrays
    const float*      m_subSampleWeights[2];
  };

  NM_INLINE static const SubDivisionDataBlock& getSubDivisionDataBlock(uint32_t subDivisionIndex);

private:
  // Line sub-division data blocks
  static const SubDivisionDataBlock sm_subDivisionDataBlocks[3];
};


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlend1DSubSampleManager::SubDivisionDataBlock::getNumSubSamplesPerLine() const
{
  return m_numSubSamplesPerLine;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* ScatterBlend1DSubSampleManager::SubDivisionDataBlock::getSubSampleWeights(
  uint32_t lineVertexElementIndex) const
{
  NMP_ASSERT(lineVertexElementIndex < 2);
  return m_subSampleWeights[lineVertexElementIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ScatterBlend1DSubSampleManager::SubDivisionDataBlock::getLineSubSampleWeights(
  uint32_t lineSubSampleIndex,
  float& weightA,
  float& weightB) const
{
  NMP_ASSERT(lineSubSampleIndex < m_numSubSamplesPerLine);
  weightA = m_subSampleWeights[0][lineSubSampleIndex];
  weightB = m_subSampleWeights[1][lineSubSampleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const ScatterBlend1DSubSampleManager::SubDivisionDataBlock&
ScatterBlend1DSubSampleManager::getSubDivisionDataBlock(uint32_t subDivisionIndex)
{
  NMP_ASSERT(subDivisionIndex < 3);
  return sm_subDivisionDataBlocks[subDivisionIndex];
}

//----------------------------------------------------------------------------------------------------------------------
// ScatterBlend1DParameteriserInfo
//----------------------------------------------------------------------------------------------------------------------
class ScatterBlend1DParameteriserInfo
{
public:
  NodeID    m_childNodeIDs[2];        ///< The node IDs of the active blend sources
  float     m_childNodeWeights[2];    ///< The Barycentric weights for the active sources
  float     m_motionParameter;        ///< The motion parameter required to achieve the blending (Projection onto annotation)
  bool      m_wasProjected;           ///< Flag indicating if the desired motion parameters were projected back back onto the annotation.
};

//----------------------------------------------------------------------------------------------------------------------
void scatterBlend1DComputeChildNodeWeightsForInputControl(
  const NodeDef* nodeDef,
  const AttribDataScatterBlend1DDef* attribDef,
  float inputMotionParameter,
  ScatterBlend1DParameteriserInfo& result);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SCATTER_BLEND_1D_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
