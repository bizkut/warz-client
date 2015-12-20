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
#ifndef ANIM_SOURCE_COMPRESSOR_H
#define ANIM_SOURCE_COMPRESSOR_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMBasicLogger.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "assetProcessor/AnimSource/AnimSourcePreprocessor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup CompressedAnimationAssetProcessorModule AnimSource Asset Processor
/// \ingroup AssetProcessorModule
///
/// Classes and functions for compiling uncompressed runtime animations into the final runtime binary animation assets.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSourceCompressor
/// \brief    A virtual base class that provides the framework for compiling the basic runtime ready animation
///           format into its final binary format by using some user defined compression algorithm.
/// \ingroup  CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressor
{
public:
  //-----------------------
  // Rig to anim maps
  static MR::RigToAnimMap* buildRigToAnimEntryMap(
    const MR::AnimRigDef* rig,
    const MR::RigToAnimEntryMap* rigToAnimEntryMap);

  static MR::RigToAnimMap* buildAnimToRigTableMap(
    const MR::AnimRigDef* rig,
    const MR::RigToAnimEntryMap* rigToAnimEntryMap,
    const MR::RigToAnimEntryMap* conglomerateRigToAnimEntryMap);

public:
  //-----------------------
  // Constructor / Destructor
  AnimSourceCompressor();
  virtual ~AnimSourceCompressor();

  /// \brief Main compression function to convert a basic uncompressed animation into
  /// your desired format
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
    NMP::BasicLogger*                          errorLogger);

  // Input options
  const AnimSourceUncompressed* getInputAnimation() const { return m_inputAnimResampled; }
  const char* getAnimFileName() const { return m_animFileName; }
  const char* getTakeName() const { return m_takeName; }
  const char* getOptions() const { return m_options; }

  float getResampleRate() const { return m_resampleRate; }
  bool getGenerateDeltas() const { return m_generateDeltas; }

  NM_INLINE const std::vector<const MR::AnimRigDef*>& getRigs() const;
  const MR::AnimRigDef* getConglomerateRig() const { return m_conglomerateRig; }

  NM_INLINE const std::vector<const MR::RigToAnimEntryMap*>& getRigToAnimEntryMaps() const;
  const MR::RigToAnimEntryMap* getConglomerateRigToAnimEntryMap() const { return m_conglomerateRigToAnimEntryMap; }

  NM_INLINE std::vector<MR::RigToAnimMap*>& AnimSourceCompressor::getRigToAnimMaps() const;

  NMP::BasicLogger* getMessageLogger() const { return m_messageLogger; }
  NMP::BasicLogger* getErrorLogger() const { return m_errorLogger; }

  // Channel names table
  bool getAddChannelNamesTable() const { return m_addChannelNamesTable; }
  void setAddChannelNamesTable(bool enable) { m_addChannelNamesTable = enable; }
  const NMP::OrderedStringTable* getChannelNamesTable() const { return m_inputAnimResampled->getChannelNames(); }
  NMP::Memory::Format getMemReqsChannelNamesTable() const { return m_memReqsChannelNamesTable; }

  // Compression rate (bytes/bone/sec)
  float getCompressionRate() const;

protected:
  //---------------------------------------------------------------------
  /// \name   Callback functions
  /// \brief  Function handlers that are called by the compressor framework
  //---------------------------------------------------------------------
  //@{

  /// \brief Callback function to initialise the compressor
  virtual void initCompressor();

  /// \brief Callback function to perform cleanup prior to terminating the compressor
  virtual void termCompressor();

  /// \brief Callback function to compute the memory requirements for the channel names table
  virtual size_t computeChannelNamesTableRequirements() const;

  /// \brief Callback function to compute the final memory requirements for the entire animation
  virtual size_t computeFinalMemoryRequirements() const = 0;

  /// \brief Callback function to build the compiled animation
  virtual void buildAnimation(NMP::Memory::Resource& resource) const = 0;

  /// \brief Callback function to build the compiled RigToAnimMap
  virtual void buildRigToAnimMaps() = 0;
  //@}

protected:
  // Input options
  void initOptions(
    const AnimSourceUncompressed*               inputAnim,
    const char*                                 animFileName,
    const char*                                 takeName,
    const acAnimInfo*                           animInfo,
    const char*                                 options,
    const MR::AnimRigDef*                       conglomerateRig,
    const MR::RigToAnimEntryMap*                conglomerateRigToAnimEntryMap,
    const std::vector<const MR::AnimRigDef*>*   rigs,
    const std::vector<const MR::RigToAnimEntryMap*>* rigToAnimEntryMaps,
    std::vector<MR::RigToAnimMap*>*             rigToAnimMaps,
    NMP::BasicLogger*                           messageLogger,
    NMP::BasicLogger*                           errorLogger);

  void termOptions();

  bool parseOptionsForGenerateDeltas();
  float parseOptionsForResampleRate();

  void getUnchangingPosAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const;
  void getChangingPosAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const;

  void getUnchangingQuatAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const;
  void getChangingQuatAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const;

  // Debugging
  void writeChannelNamesTable(const char* filename) const;

protected:
  const AnimSourceUncompressed*               m_inputAnimUncompressed;
  AnimSourceUncompressed*                     m_inputAnimResampled;
  const char*                                 m_animFileName;
  const char*                                 m_takeName;
  const acAnimInfo*                           m_animInfo;
  const char*                                 m_options;

  const MR::AnimRigDef*                       m_conglomerateRig;
  const MR::RigToAnimEntryMap*                m_conglomerateRigToAnimEntryMap;
  const std::vector<const MR::AnimRigDef*>*   m_rigs;
  const std::vector<const MR::RigToAnimEntryMap*>* m_rigToAnimEntryMaps;
  std::vector<MR::RigToAnimMap*>*             m_rigToAnimMaps;

  NMP::BasicLogger*                           m_messageLogger;
  NMP::BasicLogger*                           m_errorLogger;

  // Animation options
  float                                       m_resampleRate;
  bool                                        m_generateDeltas;

  // Channel names table
  bool                                        m_addChannelNamesTable;
  NMP::Memory::Format                         m_memReqsChannelNamesTable;

  // Animation source
  NMP::Memory::Format                         m_memReqsAnimSource;
};


//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::TrajectorySourceCompressor
/// \brief    A virtual base class that provides the framework for compiling the basic runtime ready 
///           trajectory source
/// \ingroup  CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceCompressor
{
public:
  //-----------------------
  // Constructor / Destructor
  TrajectorySourceCompressor();
  virtual ~TrajectorySourceCompressor();
  
  /// \brief Main compression function to convert a basic uncompressed trajectory into
  /// your desired format
  virtual NMP::Memory::Resource compressTrajectory(
    const TrajectorySourceUncompressed*        inputTrajectory,
    const char*                                options,
    NMP::BasicLogger*                          messageLogger,
    NMP::BasicLogger*                          errorLogger);

  //---------------------------------------------------------------------
  /// \name   Callback functions
  /// \brief  Function handlers that are called by the compressor framework
  //---------------------------------------------------------------------
  //@{

  /// \brief Callback function to initialise the compressor
  virtual void initCompressor();

  /// \brief Callback function to perform cleanup prior to terminating the compressor
  virtual void termCompressor();

  /// \brief Callback function to compute the final memory requirements for the entire animation
  virtual size_t computeTrajectoryRequirements() const = 0;

  /// \brief Callback function to build the compiled animation
  virtual void buildTrajectorySource(NMP::Memory::Resource& resource) const = 0;
  //@}

protected:
  void initOptions(
    const TrajectorySourceUncompressed*        inputTrajectory,
    const char*                                options,
    NMP::BasicLogger*                          messageLogger,
    NMP::BasicLogger*                          errorLogger);

  void termOptions();

protected:
  const TrajectorySourceUncompressed*         m_inputTrajUncompressed;
  const char*                                 m_options;
  
  NMP::BasicLogger*                           m_messageLogger;
  NMP::BasicLogger*                           m_errorLogger;
  
  // Trajectory source
  NMP::Memory::Format                         m_memReqsTrajSource;
};


//----------------------------------------------------------------------------------------------------------------------
// AnimSourceCompressor Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const std::vector<const MR::AnimRigDef*>& AnimSourceCompressor::getRigs() const
{
  NMP_VERIFY_MSG(m_rigs, "Missing rigs from AnimSourceCompressor");
  return *m_rigs;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const std::vector<const MR::RigToAnimEntryMap*>& AnimSourceCompressor::getRigToAnimEntryMaps() const
{
  NMP_VERIFY_MSG(m_rigToAnimEntryMaps, "Missing rig to anim entry maps from AnimSourceCompressor");
  return *m_rigToAnimEntryMaps;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE std::vector<MR::RigToAnimMap*>& AnimSourceCompressor::getRigToAnimMaps() const
{
  NMP_VERIFY_MSG(m_rigToAnimMaps, "Missing rig to anim maps from AnimSourceCompressor");
  return *m_rigToAnimMaps;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_COMPRESSOR_H
//----------------------------------------------------------------------------------------------------------------------
