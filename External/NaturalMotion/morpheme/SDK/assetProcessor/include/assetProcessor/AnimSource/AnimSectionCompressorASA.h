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
#ifndef ANIM_SECTION_COMPRESSOR_ASA_H
#define ANIM_SECTION_COMPRESSOR_ASA_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AnimSource/AnimSourceCompressorASA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorASA;

class AnimSectionASABuilder : public MR::AnimSectionASA
{
public:

  /// \brief Function to build the animation section within the specified memory buffer
  static void init(
    const AnimSourceUncompressed* inputAnim,
    MR::AnimSectionASA*           section,
    const MR::ChannelSetASAInfo*  channelSetInfo,
    uint32_t                      numSubSections,
    uint32_t                      startFrame,
    uint32_t                      endFrame);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSectionCompressorASA
/// \brief    This class is responsible for compiling a section of the animation keyframes into the ASA
///           animation format.
/// \ingroup  CompressedAnimationAssetProcessorModule
///
/// \details  A section of ASA data is composed of:
///   - Header data block.
///   - Array of pointers to each subsection
///
/// The packed sample data is continuous on channels for each frame in memory to ensure that the
/// data is not fragmented which can lead to cache miss performance hits at decompression time.
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionCompressorASA
{

public:
  //-----------------------
  // Constructor / Destructor
  AnimSectionCompressorASA(AnimSourceCompressorASA* manager, uint32_t sectionID);
  ~AnimSectionCompressorASA();

  /// \brief Main function to fit splines to the frame data, transform the data
  /// into the required representation for compression then compile the resulting
  /// data into binary format by finding the optimal assignment of bits for each
  /// channel.
  bool compileSection();

  /// \brief Function to write debug information about the section to a file.
  void writeDebug(FILE* filePointer) const;

  void writeSectionData(FILE* filePointer) const;

  //---------------------------------------------------------------------
  /// \name   Accessors
  /// \brief  Functions that retrieve information for the section data.
  //---------------------------------------------------------------------
  //@{
  /// \brief Function to calculate the start and end channels of a section
  void calculateStartEndChannelIndex(
    uint32_t  sampledPosNumChans,
    uint32_t  numSubsections,
    uint32_t  subsectionIndex,
    uint32_t& channelStartIndex,
    uint32_t& channelEndIndex);

  /// \brief Function to get the start frame index of this section.
  uint32_t getSectionStartFrame() const { return m_sectionStartFrame; }

  /// \brief Function to get the end frame index of this section.
  uint32_t getSectionEndFrame() const { return m_sectionEndFrame; }

  /// \brief Function to compute the number of frames in this section.
  uint32_t getNumSectionFrames() const;

  /// \brief Function to get the section size in bytes.
  size_t getSectionSize() const { return m_sectionSize; }

  /// \brief Function to get the parent animation compressor to which this section belongs.
  AnimSourceCompressorASA* getAnimSourceCompressor() { return m_manager; }

protected:
  //---------------------------------------------------------------------
  AnimSourceCompressorASA*            m_manager;                    ///< The parent animation compressor
  uint32_t                            m_sectionID;                  ///< The section number index

  //-----------------------
  // Section Information
  uint32_t                            m_numChannelSets;             ///< The number of animation channels (unchanging + animated)
  uint32_t                            m_sectionStartFrame;          ///< The frame index of the start of this section
  uint32_t                            m_sectionEndFrame;            ///< The frame index of the end of this section
  size_t                              m_sectionSize;                ///< The size of this compiled section in bytes
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SECTION_COMPRESSOR_ASA_H
//----------------------------------------------------------------------------------------------------------------------
