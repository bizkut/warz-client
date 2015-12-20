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
#ifndef ANIM_SOURCE_COMPRESSOR_QSA_H
#define ANIM_SOURCE_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "NMNumerics/NMSimpleKnotVector.h"
#include "NMNumerics/NMPosSpline.h"
#include "NMNumerics/NMQuatSpline.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
#include "assetProcessor/AnimSource/AnimSourceSectioningCompressor.h"
#include "assetProcessor/AnimSource/AnimSourceMotionAnalyser.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  // Alignment warning disabled to work around issues in AnimSourceCompressorQSA.
  #pragma warning(push)
  #pragma warning(disable: 4324)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class ChannelSetInfoCompressorQSA;
class AnimSectionCompressorQSA;
class TrajectorySourceCompressorQSA;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimSourceCompressorQSA
/// \brief A compressor class to compile the uncompressed runtime animation into the QSA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorQSA : public AnimSourceSectioningCompressor
{
public:
  //-----------------------
  // Constructor / Destructor
  AnimSourceCompressorQSA();
  ~AnimSourceCompressorQSA();

  //-----------------------
  // Animation info
  uint32_t getMaxNumKeyframes() const;
  uint32_t getNumAnimChannelSets() const;
  uint32_t getNumRigChannelSets() const; // Conglomerate rig

  uint32_t getFramesPerKey() const { return m_framesPerKey; }

  float getQuantisationSetQuality() const { return m_quantisationSetQuality; }

  bool getUseDefaultPose() const { return m_useDefaultPose; }
  bool getUseWorldSpace() const { return m_useWorldSpace; }

  //-----------------------
  // Motion analyser
  const AnimSourceMotionAnalyser* getMotionAnalyser() const { return m_motionAnalyser; }
  AnimSourceMotionAnalyser* getMotionAnalyser() { return m_motionAnalyser; }

  bool getGenerateDeltas() const; // Input animation has been converted to deltas for additive animation
  const ChannelSetTable* getAnimChannelSets() const; // Local space channel sets in anim channel order
  const ChannelSetTable* getChannelSetsLS() const; // Local space channel sets in rig channel order
  const ChannelSetTable* getChannelSetsWS() const; // World space channel sets in rig channel order

  //-----------------------
  // Channel set information
  ChannelSetInfoCompressorQSA* getChannelSetInfoCompressor() const { return m_channelSetInfoCompressor; }

  //-----------------------
  // Animation section data
  uint32_t getNumAnimSectionCompressors() const { return (uint32_t)m_animSectionCompressors.size(); }
  AnimSectionCompressorQSA* getAnimSectionCompressor(uint32_t i) const;

  //-----------------------
  // Trajectory source
  TrajectorySourceCompressorQSA* getTrajectorySourceCompressor() const { return m_trajectorySourceCompressor; }

protected:
  //---------------------------------------------------------------------
  /// \name   Callback functions
  /// \brief  Function handlers that are called by the sectioning compressor framework
  //---------------------------------------------------------------------
  //@{

  /// \brief Callback function to initialise the compressor. This is the main function
  /// that computes the section requirements, transforms the sample data into the required
  /// compressed format and finds the best allocation of bits for each channel. It
  /// independently compiles the data into binary blocks for later reassembly into a
  /// single contiguous memory block of the final compressed animation.
  void initCompressor() NM_OVERRIDE;

  /// \brief Callback function to perform cleanup prior to terminating the compressor.
  void termCompressor() NM_OVERRIDE;

  /// \brief Callback function to determine the size of the trajectory source in bytes.
  size_t computeTrajectoryRequirements() const NM_OVERRIDE;

  /// \brief Callback function to compute the memory requirements for any additional data
  size_t computeAdditionalDataRequirements() const NM_OVERRIDE;

  /// \brief Callback function to determine the size of the anim source header in bytes.
  size_t computeAnimSourceHeaderRequirements() const NM_OVERRIDE;

  /// \brief Callback function to determine the size of the channel set information in bytes.
  size_t computeChannelSetInfoRequirements() const NM_OVERRIDE;

  /// \brief Callback function that is called to determine the section requirements.
  // This function does nothing since the section byte budgets and frame offsets
  /// should have already been computed when the compressor was initialised.
  bool computeSectionRequirements() NM_OVERRIDE;

  /// \brief Callback function to determine the size of a section in bytes. This function
  // is not used and should never be called for this compressor.
  bool computeSectionRequirements(
    uint32_t startFrame,
    uint32_t endFrame,
    size_t&  sectionSize) const NM_OVERRIDE;

  /// \brief Callback function that is called after building the animation source. This allows
  // us to spit out a load of debugging information for the compressed animation.
  void onEndBuildAnimation(NMP::Memory::Resource& buffer) NM_OVERRIDE;

  /// \brief Callback function to build the compiled animation header.
  void buildAnimSourceHeader(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the header
  ) const NM_OVERRIDE;

  /// \brief Callback function to build the channel set info.
  void buildChannelSetInfo(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the channel set info
  ) const NM_OVERRIDE;

  /// \brief Callback function to build a section.
  void buildSection(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint32_t               sectionIndx,     ///< The index of the section to build
    uint8_t*               data             ///< The memory address in which to build the header
  ) const NM_OVERRIDE;

  /// \brief Callback function to build the trajectory.
  void buildTrajectorySource(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the trajectory source
  ) const NM_OVERRIDE;

  /// \brief Callback function to build the channel names table.
  void buildChannelNamesTable(
    NMP::Memory::Resource& buffer,          ///< The resource where to build the animation
    uint8_t* data                           ///< The memory address in which to build the channel names table
  ) const NM_OVERRIDE;

  /// \brief Callback function to build any additional data.
  void buildAdditionalData(
    NMP::Memory::Resource& buffer,
    uint8_t* data
    ) const NM_OVERRIDE;

  /// \brief Callback function to build the compiled RigToAnimMap
  void buildRigToAnimMaps() NM_OVERRIDE;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \name   Compression functions
  /// \brief  Functions that transform data into the required compressed binary format
  //---------------------------------------------------------------------
  //@{

  /// \brief Function for setting the initial default processing options.
  void setDefaults();

  /// \brief Function for parsing the input options string.
  void parseOptions();

  /// \brief Function to validate the compressor options.
  void validateOptions();

  bool getOptionsSampled() const;
  void setOptionsSampled();

  /// \brief Function to compute the number of sections and the byte budget assigned to each section.
  void computeSectionBudgets();

  /// \brief Function to compute the memory requirements for the animation source header.
  size_t computeAnimSourceHeaderRequirements(uint32_t numSections) const;

  void compileAnimSourceSampledLS();
  void compileAnimSourceSampledWS();
  void compileAnimSourceWithOptions();
  void compareAndRestoreCompressorState();

  /// \brief Function to compile the anim info and section data using the current options.
  void compileSections();

  void computeError();

  bool getAnimSourceHasFullPrec() const;
  void saveCompressorState();
  void restoreCompressorState();

  void clearTrajectory();
  void clearCompressorState();
  void clearCompressorStateSaved();
  //@}

  //---------------------------------------------------------------------
  /// \name   Testing functions
  /// \brief  Functions that output debugging information to a file
  //---------------------------------------------------------------------
  //@{
  void testLocation(NMP::Memory::Resource& buffer) const;

  void writeDebugQSAInfo(
    FILE* filePointer,
    NMP::Memory::Resource& buffer) const;
  //@}

protected:
  //-----------------------
  // Compressor options
  uint32_t                            m_maxFramesPerSection;              ///< Maximum number of frames within each section
  float                               m_desiredCompressionRate;           ///< Desired rate in (bytes/bone/sec)
  uint32_t                            m_framesPerKey;
  float                               m_quantisationSetQuality;
  bool                                m_useDefaultPose;
  bool                                m_useWorldSpace;

  //-----------------------
  // Motion analyser
  AnimSourceMotionAnalyser*           m_motionAnalyser;                   ///< Analyses the motion for discontinuities and potential jitter

  //-----------------------
  // Compressor channel set information
  ChannelSetInfoCompressorQSA*        m_channelSetInfoCompressor;         ///< Compressor class for compiling the channel set information
  ChannelSetInfoCompressorQSA*        m_channelSetInfoCompressorSaved;

  //-----------------------
  // Compressor sections
  std::vector<AnimSectionCompressorQSA*>  m_animSectionCompressors;       ///< Compressor classes for compiling the section data
  std::vector<AnimSectionCompressorQSA*>  m_animSectionCompressorsSaved;  ///< Compressor classes for compiling the section data

  //-----------------------
  // Compressor trajectory source
  TrajectorySourceCompressorQSA*      m_trajectorySourceCompressor;       ///< Compressor class for compiling the trajectory source

  //-----------------------
  // Information
  float                               m_error;
  size_t                              m_budgetExtra;                      ///< Extra byte budget required to compile valid sections

  NMP::Memory::Format                 m_memReqsChannelNamesTableSaved;
  NMP::Memory::Format                 m_memReqsTrajectorySourceSaved;
  NMP::Memory::Format                 m_memReqsChannelSetInfoSaved;
  NMP::Memory::Format                 m_memReqsAnimSourceHdrSaved;
  std::vector<uint32_t>               m_startFramesSaved;
  std::vector<uint32_t>               m_endFramesSaved;
  std::vector<size_t>                 m_sectionSizesSaved;
  float                               m_errorSaved;
  size_t                              m_budgetExtraSaved;                 ///< Extra byte budget required to compile valid sections

  //-----------------------
  // Status
  bool                                m_status;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
