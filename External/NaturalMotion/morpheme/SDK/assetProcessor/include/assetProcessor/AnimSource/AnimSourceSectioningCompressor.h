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
#ifndef ANIM_SOURCE_SECTIONING_COMPRESSOR_H
#define ANIM_SOURCE_SECTIONING_COMPRESSOR_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "assetProcessor/AnimSource/AnimSourceCompressor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSourceSectioningCompressor
/// \brief    A base class that provides the framework for a compressor that sections uncompressed runtime
///           animations into suitable data chunks. Compressed animation assets that have been sectioned in
///           such a way can then be easily DMA'd to SPU for decompression.
/// \ingroup  CompressedAnimationAssetProcessorModule
///
/// A sectioning compressor builds an animation source that has the following layout:
///
/// [AnimSourceHeader] : DMA alignment
/// [Channel Set Info] : DMA alignment
/// [Section 0] : DMA alignment
///     ...
/// [Section N] : DMA alignment
/// [Trajectory] : DMA alignment
/// [Channel Names] : DMA alignment
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceSectioningCompressor : public AnimSourceCompressor
{
public:
  //-----------------------
  // Constructor / Destructor
  AnimSourceSectioningCompressor();
  virtual ~AnimSourceSectioningCompressor();

  //-----------------------
  /// \brief Main compression function to convert a basic uncompressed animation into
  /// your desired format.
  virtual NMP::Memory::Resource compressAnimation(
    const AnimSourceUncompressed*              inputAnim,
    const char*                                animFileName,
    const char*                                takeName,
    const acAnimInfo*                          animInfo,
    const char*                                options,
    const MR::AnimRigDef*                      conglomerateRig,
    const MR::RigToAnimEntryMap*               conglomerateRigToAnimEntryMap,
    const std::vector<const MR::AnimRigDef*>*  rigs,
    const std::vector<const MR::RigToAnimEntryMap*>* rigToAnimEntryMaps,
    std::vector<MR::RigToAnimMap*>*            rigToAnimMaps,
    NMP::BasicLogger*                          messageLogger,
    NMP::BasicLogger*                          errorLogger) NM_OVERRIDE;

  //-----------------------
  // Section functions
  uint32_t getNumSections() const { return (uint32_t)m_startFrames.size(); }
  size_t getSectionsByteOffset() const { return m_sectionsByteOffset; }
  const std::vector<uint32_t>& getSectionStartFrames() const { return m_startFrames; }
  const std::vector<uint32_t>& getSectionEndFrames() const { return m_endFrames; }
  const std::vector<size_t>& getSectionSizes() const { return m_sectionSizes; }
  size_t getSectionsSize() const;

  //-----------------------
  // Memory requirements
  const NMP::Memory::Format& getMemReqsAnimSourceHdr() const { return m_memReqsAnimSourceHdr; }
  const NMP::Memory::Format& getMemReqsChannelSetInfo() const { return m_memReqsChannelSetInfo; }
  const NMP::Memory::Format& getMemReqsTrajectorySource() const { return m_memReqsTrajectorySource; }
  const NMP::Memory::Format& getMemReqsAdditionalData() const { return m_memReqsAdditionalData; }

protected:
  //---------------------------------------------------------------------
  /// \name   Callback functions
  /// \brief  Function handlers that are called by the sectioning compressor framework
  //---------------------------------------------------------------------
  //@{

  /// \brief Callback function to determine the size of the trajectory source in bytes.
  virtual size_t computeTrajectoryRequirements() const = 0;

  /// \brief Callback function to determine the size of the anim source header in bytes.
  virtual size_t computeAnimSourceHeaderRequirements() const = 0;

  /// \brief Callback function to determine the size of the channel set information in bytes.
  virtual size_t computeChannelSetInfoRequirements() const = 0;

  /// \brief Callback function to compute the memory requirements for any additional data
  virtual size_t computeAdditionalDataRequirements() const;

  /// \brief Callback function that is called prior to computing the section requirements.
  virtual void onBeginComputeSectionRequirements();

  /// \brief Callback function that is called to compute to determine the section requirements.
  virtual bool computeSectionRequirements();

  /// \brief Callback function to determine the size of a section in bytes.
  virtual bool computeSectionRequirements(
    uint32_t startFrame,
    uint32_t endFrame,
    size_t&  sectionSize) const = 0;

  /// \brief Callback function to append new section information data.
  virtual void appendSectionInformation(
    uint32_t startFrame,
    uint32_t endFrame,
    size_t   sectionSize);

  /// \brief Callback function that is called after to computing the section requirements.
  virtual void onEndComputeSectionRequirements();

  /// \brief Callback function to compute the final memory requirements for the entire animation.
  size_t computeFinalMemoryRequirements() const NM_OVERRIDE;

  /// \brief Callback function that is called prior to building the animation source.
  virtual void onBeginBuildAnimation(NMP::Memory::Resource& buffer);

  /// \brief Callback function to build the compiled animation.
  void buildAnimation(NMP::Memory::Resource& buffer) const NM_OVERRIDE;

  /// \brief Callback function that is called after building the animation source.
  virtual void onEndBuildAnimation(NMP::Memory::Resource& buffer);

  /// \brief Callback function to build the compiled animation header.
  virtual void buildAnimSourceHeader(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the header
  ) const = 0;

  /// \brief Callback function to build the channel set info.
  virtual void buildChannelSetInfo(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the channel set info
  ) const = 0;

  /// \brief Callback function to build a section.
  virtual void buildSection(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint32_t               sectionIndx,     ///< The index of the section to build
    uint8_t*               data             ///< The memory address in which to build the header
  ) const = 0;

  /// \brief Callback function to build the trajectory.
  virtual void buildTrajectorySource(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the trajectory source
  ) const = 0;

  /// \brief Callback function to build the channel names table.
  virtual void buildChannelNamesTable(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the channel names table
  ) const;

  /// \brief Callback function to build any additional data.
  virtual void buildAdditionalData(
    NMP::Memory::Resource& buffer,
    uint8_t* data
  ) const;
  //@}

  /// \brief Function to compute the number of sections required for a specified
  /// frames per section rate.
  uint32_t computeNumSections(uint32_t framesPerSection) const;

  /// \brief Function to make an even distribution of frames amongst the sections. The
  /// function returns the maximum number of frames per section assigned.
  uint32_t distributeSectionFrames(uint32_t numSections, uint32_t numFrames);

  /// \brief Function to make an even distribution of frames amongst the sections and
  /// compute the corresponding section budget sizes. The function returns the maximum
  /// of the section sizes.
  size_t distributeSectionSizes(uint32_t numSections, size_t budgetSections);

  /// \brief Function to compute the total number of section frame samples. Note that
  /// the boundary frames are repeated between sections.
  uint32_t getNumSectionFrameSamples() const;

  /// \brief Function to compute the number of section frame samples in the specified section.
  uint32_t getNumSectionFrameSamples(uint32_t sectionIndex) const;

protected:
  // AnimSource header
  NMP::Memory::Format                 m_memReqsAnimSourceHdr;

  // Channel Set Info
  NMP::Memory::Format                 m_memReqsChannelSetInfo;

  // Sections
  size_t                              m_sectionsByteOffset;           ///< Byte offset of the section data from the anim source
  std::vector<uint32_t>               m_startFrames;
  std::vector<uint32_t>               m_endFrames;
  std::vector<size_t>                 m_sectionSizes;

  // TrajectorySource
  NMP::Memory::Format                 m_memReqsTrajectorySource;

  // Additional data
  NMP::Memory::Format                 m_memReqsAdditionalData;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_SECTIONING_COMPRESSOR_H
//----------------------------------------------------------------------------------------------------------------------
