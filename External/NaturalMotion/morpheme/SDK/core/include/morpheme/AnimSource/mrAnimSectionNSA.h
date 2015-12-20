// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_ANIM_SECTION_NSA_H
#define MR_ANIM_SECTION_NSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrTask.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::UnchangingDataNSA
/// \brief Stores channel set information that is global to all sections within the compressed animation.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class UnchangingDataNSA
{
public:  
  /// \brief Function to compute the memory requirements for the unchanging pos data
  static NM_INLINE NMP::Memory::Format getMemoryReqsUnchangingPosData(uint32_t unchangingPosNumChannels);
  
  /// \brief Function to compute the memory requirements for the unchanging quat data
  static NM_INLINE NMP::Memory::Format getMemoryReqsUnchangingQuatData(uint32_t unchangingQuatNumChannels);
  
  static UnchangingDataNSA* relocate(void*& ptr);
  
public:
  UnchangingDataNSA() {}
  ~UnchangingDataNSA() {}

  void locate();
  void dislocate();
  NM_INLINE void relocate();
  
  void unchangingPosDecompress(
    const AnimToRigTableMap*    animToRigTableMap,
    const CompToAnimChannelMap* compToAnimTableMap,
    NMP::DataBuffer*            outputBuffer) const;

  void unchangingQuatDecompress(
    const AnimToRigTableMap*    animToRigTableMap,
    const CompToAnimChannelMap* compToAnimTableMap,
    NMP::DataBuffer*            outputBuffer) const;

protected:
  uint32_t                          m_unchangingPosNumChannels;       ///< The number of pos channels with unvarying data
  uint32_t                          m_unchangingQuatNumChannels;      ///< The number of quat channels with unvarying data

  QuantisationScaleAndOffsetVec3    m_unchangingPosQuantisationInfo;  ///< Quantisation info about ranges of all unchanging pos channels
  QuantisationScaleAndOffsetVec3    m_unchangingQuatQuantisationInfo; ///< Quantisation info about ranges of all unchanging quat channels

  UnchangingKeyVec3*                m_unchangingPosData;              ///< Table of default values for unvarying pos channels quantised
                                                                      ///< into the [qMin, qMax] range of unchangingPosKeysInfo
  UnchangingKeyVec3*                m_unchangingQuatData;             ///< Table of default values for unvarying quat channels (tan quarter rot vecs)
                                                                      ///< quantised into the [qMin, qMax] range of unchangingQuatKeysInfo
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::SectionDataNSA
/// \brief A sectioned packet of data within a 2D grid that divides the sampled animation data
///        (frame and channel wise) into suitable component sizes.
/// \ingroup SourceAnimationCompressedModule
///
/// \details The frame and anim channel data is sectioned into a 2D grid arrangement during asset
/// compilation, so that each block is of suitable size for DMAing to the processor for decompression.
//----------------------------------------------------------------------------------------------------------------------
class SectionDataNSA
{
public:
  static NMP::Memory::Format getMemoryReqsMeanAndSetVec3(
    uint32_t    numChannels               ///< IN: The number of channels
    );

  /// \brief Function to compute the memory requirements for the sampled pos data
  static void getMemoryReqsSampledPosData(
    uint32_t    numSectionAnimFrames,     ///< IN: The number of keyframes in the section
    uint32_t    sampledPosNumChannels,    ///< IN: The number of channels
    size_t&     alignment,                ///< OUT: The alignment of the data block
    size_t&     keyFrameDataStride,       ///< OUT: The size of a frame of channel data
    size_t&     keyFrameDataMemReqs       ///< OUT: The total size of the sampled pos data
    );
    
  /// \brief Function to compute the memory requirements for the sampled quat data
  static void getMemoryReqsSampledQuatData(
    uint32_t    numSectionAnimFrames,     ///< IN: The number of keyframes in the section
    uint32_t    sampledQuatNumChannels,   ///< IN: The number of channels
    size_t&     alignment,                ///< OUT: The alignment of the data block
    size_t&     keyFrameDataStride,       ///< OUT: The size of a frame of channel data
    size_t&     keyFrameDataMemReqs       ///< OUT: The total size of the sampled quat data
    );
    
  SectionDataNSA* relocate(void*& ptr);

public:
  SectionDataNSA() {}
  ~SectionDataNSA() {}

  void locate();
  void dislocate();
  NM_INLINE void relocate();

  void sampledPosDecompress(
    const QuantisationScaleAndOffsetVec3&   posMeansQuantisationInfo,
    const QuantisationScaleAndOffsetVec3*   sampledPosQuantisationInfo,
    const AnimToRigTableMap*                animToRigTableMap,
    const CompToAnimChannelMap*             compToAnimTableMap,
    uint32_t                                sectionFrameIndex,
    float                                   interpolant,
    NMP::DataBuffer*                        outputBuffer) const;

  void sampledQuatDecompress(
    const QuantisationScaleAndOffsetVec3*   sampledQuatQuantisationInfo,
    const AnimToRigTableMap*                animToRigTableMap,
    const CompToAnimChannelMap*             compToAnimTableMap,
    uint32_t                                sectionFrameIndex,
    float                                   interpolant,
    NMP::DataBuffer*                        outputBuffer) const;

protected:
  uint32_t                          m_numSectionAnimFrames;         ///< The number of frames contained within this sub-section
  uint32_t                          m_sampledPosNumChannels;        ///< The number of sampled position channels in this sub-section
  uint32_t                          m_sampledQuatNumChannels;       ///< The number of sampled orientation channels in this sub-section
  
  SampledPosKey*                    m_sampledPosData;               ///< Quantised data for the sampled pos channels
  QuantisationMeanAndSetVec3*       m_sampledPosQuantisationData;   ///< Array of quantisation mean/qset data for the sampled pos channels
  
  SampledQuatKeyTQA*                m_sampledQuatData;              ///< Quantised data for the sampled quat channels
  QuantisationMeanAndSetVec3*       m_sampledQuatQuantisationData;  ///< Array of quantisation mean/qset data for the sampled quat channels
};


//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataNSA functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format UnchangingDataNSA::getMemoryReqsUnchangingPosData(uint32_t unchangingPosNumChannels)
{
  NMP::Memory::Format memReqsPos(sizeof(UnchangingKeyVec3) * unchangingPosNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  return memReqsPos;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format UnchangingDataNSA::getMemoryReqsUnchangingQuatData(uint32_t unchangingQuatNumChannels)
{
  NMP::Memory::Format memReqsQuat(sizeof(UnchangingKeyVec3) * unchangingQuatNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  return memReqsQuat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void UnchangingDataNSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_NATURAL_TYPE_ALIGNMENT),
    "Unchanging channel data must be aligned to %d bytes.",
    NMP_NATURAL_TYPE_ALIGNMENT);
    
  void* ptr = this;
  relocate(ptr);
}

//----------------------------------------------------------------------------------------------------------------------
// SectionDataNSA functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SectionDataNSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Sub-sections must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  void* ptr = this;
  relocate(ptr);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIM_SECTION_NSA_H
//----------------------------------------------------------------------------------------------------------------------
