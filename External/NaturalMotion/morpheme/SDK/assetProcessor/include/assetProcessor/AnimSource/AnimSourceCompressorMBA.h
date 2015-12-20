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
#ifndef ANIM_SOURCE_COMPRESSOR_MBA_H
#define ANIM_SOURCE_COMPRESSOR_MBA_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AnimSource/AnimSourceSectioningCompressor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSourceMBALayout
/// \brief    Class to hold the byte offset layout of components within the compiled binary animation asset.
/// \ingroup  CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceMBALayout
{
public:
  AnimSourceMBALayout() {}
  ~AnimSourceMBALayout() {}

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
/// \class    AP::AnimSourceCompressorMBA
/// \brief    A compressor class to compile the uncompressed runtime animation into the MBA format.
/// \ingroup  CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorMBA : public AnimSourceSectioningCompressor
{
public:
  AnimSourceCompressorMBA();
  ~AnimSourceCompressorMBA();

  AnimSourceMBALayout& getAnimSourceMemLayout() { return m_animSourceMemLayout; }
  const AnimSourceMBALayout& getAnimSourceMemLayout() const { return m_animSourceMemLayout; }

protected:
  /// \brief Callback function to determine the size of a section in bytes.
  virtual bool computeSectionRequirements(
    uint32_t startFrame,
    uint32_t endFrame,
    size_t&  sectionSize) const;

  /// \brief Callback function to compute the memory requirements for the entire animation.
  virtual size_t computeFinalMemoryRequirements() const;

  /// \brief Callback function to build the compiled animation.
  virtual void buildAnimation(NMP::Memory::Resource& buffer) const;

protected:
  AnimSourceMBALayout m_animSourceMemLayout;

protected:
  //---------------------------------------------------------------------
  /// \name   Callback functions
  /// \brief  Function handlers that are called by the sectioning compressor framework
  //---------------------------------------------------------------------
  //@{

  /// \brief Callback function to determine the size of the trajectory source in bytes.
  virtual size_t computeTrajectoryRequirements() const { return 0; }

  /// \brief Callback function to determine the size of the anim source header in bytes.
  virtual size_t computeAnimSourceHeaderRequirements() const { return 0; }

  /// \brief Callback function to determine the size of the channel set information in bytes.
  virtual size_t computeChannelSetInfoRequirements() const { return 0; }

  /// \brief Callback function that is called after building the animation source. This allows
  /// us to spit out a load of debugging information for the compressed animation.
  virtual void onEndBuildAnimation(NMP::Memory::Resource& buffer);

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
  //@}

protected:
  //---------------------------------------------------------------------
  /// \name   Testing functions
  /// \brief  Functions that output debugging information to a file
  //---------------------------------------------------------------------
  //@{
  void testLocation(NMP::Memory::Resource& buffer) const;

  void writeDebugMBAInfo(
    FILE* filePointer,
    NMP::Memory::Resource& buffer) const;
  //@}
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_COMPRESSOR_MBA_H
//----------------------------------------------------------------------------------------------------------------------
