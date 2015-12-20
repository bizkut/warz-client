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
#ifndef ANIM_SOURCE_COMPRESSOR_ASA_H
#define ANIM_SOURCE_COMPRESSOR_ASA_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "morpheme/AnimSource/mrAnimSourceASA.h"
#include "assetProcessor/AnimSource/AnimSourceSectioningCompressor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

class AnimSectionCompressorASA;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetASAInfoBuilder
/// \brief A class to build the channel set information
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetASAInfoBuilder : public MR::ChannelSetASAInfo
{
public:

  /// \brief Initialisation function to build the channel set information for the
  /// position and orientation channels of an animation.
  static void init(
    ChannelSetASAInfo&   chanSetInfo,
    uint32_t             numRequiredQuatSamples,
    const NMP::Quat*     quatKeyframes,
    uint32_t             numRequiredPosSamples,
    const NMP::Vector3*  posKeyframes);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSourceASALayout
/// \brief    Class to hold the byte offset layout of components within the compiled binary animation asset.
/// \ingroup  CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceASALayout
{
public:
  AnimSourceASALayout() {}
  ~AnimSourceASALayout() {}

  // Byte offsets within memory buffer
  size_t m_headerOffset;
  size_t m_sectionInfoOffset;
  size_t m_sectionPtrsOffset;
  size_t m_chanSetInfoOffset;
  size_t m_sectionsOffset;
  size_t m_trajectoryOffset;
  size_t m_chanNamesOffset;

  NMP::Memory::Format m_trajectoryMemReqs;  ///< Memory requirements of trajectory channel.
  size_t              m_totalSize;          ///< Total size of the memory buffer
};

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSourceCompressorASA
/// \brief    A compressor class to compile the uncompressed runtime animation into the ASA format.
/// \ingroup  CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorASA : public AnimSourceSectioningCompressor
{
public:
  AnimSourceCompressorASA() {}
  ~AnimSourceCompressorASA() {}

  AnimSourceASALayout& getAnimSourceMemLayout() { return m_animSourceMemLayout; }
  const AnimSourceASALayout& getAnimSourceMemLayout() const { return m_animSourceMemLayout; }
  //-----------------------
  // Section functions
  uint32_t getNumSubSections() const { return m_numSubsections; }

protected:
  /// \brief Callback function to initialise the compressor.
  virtual void initCompressor();

  /// \brief Callback function to determine the size of a section in bytes.
  virtual bool computeSectionRequirements(
    uint32_t startFrame,
    uint32_t endFrame,
    size_t&  sectionSize) const;

  /// \brief Callback function to determine the size of a section in bytes.
  virtual bool computeSectionRequirements();

  /// \brief Callback function to compute the memory requirements for the entire animation.
  virtual size_t computeFinalMemoryRequirements() const;

  /// \brief Callback function to build the compiled animation.
  virtual void buildAnimation(NMP::Memory::Resource& buffer) const;

protected:
  AnimSourceASALayout m_animSourceMemLayout;

protected:
  /// \brief Callback function to determine the size of the trajectory source in bytes.
  virtual size_t computeTrajectoryRequirements() const { return 0; }

  /// \brief Callback function to determine the size of the anim source header in bytes.
  virtual size_t computeAnimSourceHeaderRequirements() const { return 0; }

  /// \brief Callback function to determine the size of the channel set information in bytes.
  virtual size_t computeChannelSetInfoRequirements() const { return 0; }

  size_t computeSectionMemoryRequirements(uint32_t numFrames, uint32_t numSubSections);

  void computeSectionBudgets();

  void setDefaults();

  void parseOptions();

  /// \brief Callback function that is called after building the animation source.
  virtual void onEndBuildAnimation(NMP::Memory::Resource& NMP_UNUSED(buffer)) { }

  /// \brief Callback function to build the compiled animation header.
  virtual void buildAnimSourceHeader(
    NMP::Memory::Resource& NMP_UNUSED(buffer), ///< The resource where to build the animation
    uint8_t* NMP_UNUSED(data)                  ///< The memory address in which to build the header
  ) const { }

  /// \brief Callback function to build the channel set info.
  virtual void buildChannelSetInfo(
    NMP::Memory::Resource& NMP_UNUSED(buffer), ///< The resource where to build the animation
    uint8_t* NMP_UNUSED(data)                  ///< The memory address in which to build the channel set info
  ) const { }

  /// \brief Callback function to build a section.
  virtual void buildSection(
    NMP::Memory::Resource& NMP_UNUSED(buffer), ///< The resource where to build the animation
    uint32_t NMP_UNUSED(sectionIndx),          ///< The index of the section to build
    uint8_t* NMP_UNUSED(data)                  ///< The memory address in which to build the header
  ) const { }

  /// \brief Callback function to build the trajectory.
  virtual void buildTrajectorySource(
    NMP::Memory::Resource& NMP_UNUSED(buffer), ///< The resource where to build the animation
    uint8_t* NMP_UNUSED(data)                  ///< The memory address in which to build the trajectory source
  ) const { }

  /// \brief Callback function to build the channel names table.
  virtual void buildChannelNamesTable(
    NMP::Memory::Resource& NMP_UNUSED(buffer), ///< The resource where to build the animation
    uint8_t* NMP_UNUSED(data)                  ///< The memory address in which to build the channel names table
  ) const { }

  /// \brief Callback function to build the compiled RigToAnimMap
  void buildRigToAnimMaps() NM_OVERRIDE;

protected:
  //-----------------------
  // Compressor sections
  std::vector<AnimSectionCompressorASA*>      m_animSectionCompressors;     ///< Compressor classes for compiling the section data

  //-----------------------
  // Compressor options
  uint32_t                                    m_maxFramesPerSection;        ///< Maximum number of frames within each section
  uint32_t                                    m_numSubsections;             ///<
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_COMPRESSOR_ASA_H
//----------------------------------------------------------------------------------------------------------------------
