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
#ifndef CHANNELSET_INFO_COMPRESSOR_QSA_H
#define CHANNELSET_INFO_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "morpheme/AnimSource/mrAnimSectionQSA.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
#include "assetProcessor/AnimSource/AnimSourceUncompressed.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4324)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorQSA;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetInfoCompressorQSA
/// \brief This class is responsible for compiling the channel set information and quantising
/// the set of unchanging channels into its final binary data.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetInfoCompressorQSA
{
public:
  // Enumeration for the channel compression method
  enum chanMethodType
  {
    UnchangingDefault,
    Unchanging,
    Sampled,
    Spline,
    NumChanMethods
  };

public:
  //-----------------------
  // Constructor / Destructor
  ChannelSetInfoCompressorQSA(AnimSourceCompressorQSA* manager);
  ~ChannelSetInfoCompressorQSA();

  void setDefaults();
  void validateOptions();

  /// \brief Main function to compile the channel set information data.
  void compileChannelSetInfo();

  //---------------------------------------------------------------------
  /// \name   Channel set information functions
  /// \brief  Accessor functions that modify the channel set parameters.
  //---------------------------------------------------------------------
  //@{

  chanMethodType getCompChanMethodForPosChan(uint32_t animChanIndex) const;
  void setCompChanMethodForPosChan(uint32_t animChanIndex, chanMethodType method);
  void setDesiredCompChanMethodForPosChan(uint32_t animChanIndex, chanMethodType method);

  chanMethodType getCompChanMethodForQuatChan(uint32_t animChanIndex) const;
  void setCompChanMethodForQuatChan(uint32_t animChanIndex, chanMethodType method);
  void setDesiredCompChanMethodForQuatChan(uint32_t animChanIndex, chanMethodType method);
  uint32_t getUnchangingDefaultPosNumChans() const { return (uint32_t)m_unchangingDefaultPosCompToAnimMap.size(); }
  uint32_t getUnchangingDefaultQuatNumChans() const { return (uint32_t)m_unchangingDefaultQuatCompToAnimMap.size(); }
  uint32_t getUnchangingPosNumChans() const { return (uint32_t)m_unchangingPosCompToAnimMap.size(); }
  uint32_t getUnchangingQuatNumChans() const { return (uint32_t)m_unchangingQuatCompToAnimMap.size(); }
  uint32_t getSampledPosNumChans() const { return (uint32_t)m_sampledPosCompToAnimMap.size(); }
  uint32_t getSampledQuatNumChans() const { return (uint32_t)m_sampledQuatCompToAnimMap.size(); }
  uint32_t getSplinePosNumChans() const { return (uint32_t)m_splinePosCompToAnimMap.size(); }
  uint32_t getSplineQuatNumChans() const { return (uint32_t)m_splineQuatCompToAnimMap.size(); }

  // Comp to anim channel maps
  const std::vector<uint32_t>& getUnchangingPosCompToAnimMap() const { return m_unchangingPosCompToAnimMap; }
  const std::vector<uint32_t>& getUnchangingQuatCompToAnimMap() const { return m_unchangingQuatCompToAnimMap; }
  const std::vector<uint32_t>& getSampledPosCompToAnimMap() const { return m_sampledPosCompToAnimMap; }
  const std::vector<uint32_t>& getSampledQuatCompToAnimMap() const { return m_sampledQuatCompToAnimMap; }
  const std::vector<uint32_t>& getSplinePosCompToAnimMap() const { return m_splinePosCompToAnimMap; }
  const std::vector<uint32_t>& getSplineQuatCompToAnimMap() const { return m_splineQuatCompToAnimMap; }

  // Unchanging channels recovered from quantisation
  const NMP::Vector3* getUnchangingPosQuantised() const { return m_unchangingPosQuantised; }
  const NMP::Quat* getUnchangingQuatQuantised() const { return m_unchangingQuatQuantised; }

  // Compiled channel set information
  size_t getChannelSetInfoDataSize() const { return m_channelSetInfoDataSize; }
  size_t getUnchangingPosDataSize() const { return m_unchangingPosDataSize; }
  size_t getUnchangingQuatDataSize() const { return m_unchangingQuatDataSize; }
  MR::ChannelSetInfoQSA* getChannelSetInfoData() const { return m_channelSetInfoData; }
  //@}

  // Debugging functions
  void writeDebug(FILE* filePointer);
  void writeCompToAnimMaps(FILE* filePointer);

public:
  //---------------------------------------------------------------------
  /// \name   Callback functions
  /// \brief  Function handlers that are called by the sectioning compressor framework.
  //---------------------------------------------------------------------
  //@{

  /// \brief Callback function to initialise the compressor.
  void initCompressor();

  /// \brief Callback function to perform cleanup prior to terminating the compressor.
  void termCompressor();

  /// \brief Callback function to determine the size of the channel set information in bytes.
  size_t computeChannelSetInfoRequirements() const;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \name   Compression functions
  /// \brief  Functions that compile the channel set information into the required
  ///         compressed binary format.
  //---------------------------------------------------------------------
  //@{
  void computeCompChannelMaps();
  void computeChannelSetRootInfo();
  void unchangingPosComputeInfo();
  void unchangingQuatComputeInfo();
  void buildChannelSetInfo();

  bool checkForUnchangingPosDefault(
    uint32_t animChannelIndex,
    const AnimSourceUncompressed* animSource,
    const std::vector<const MR::AnimRigDef*>& rigs,
    const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps) const;

  bool checkForUnchangingQuatDefault(
    uint32_t animChannelIndex,
    const AnimSourceUncompressed* animSource,
    const std::vector<const MR::AnimRigDef*>& rigs,
    const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps) const;
  //@}

protected:
  AnimSourceCompressorQSA*            m_manager;

  //-----------------------
  // Channel compression methods
  uint32_t                            m_numChannelSets;
  chanMethodType*                     m_posChanCompMethods;         ///< Pos channel compression methods
  chanMethodType*                     m_quatChanCompMethods;        ///< Quat channel compression methods

  //-----------------------
  // Compression to anim channel maps
  std::vector<uint32_t>               m_unchangingDefaultPosCompToAnimMap;
  std::vector<uint32_t>               m_unchangingDefaultQuatCompToAnimMap;
  std::vector<uint32_t>               m_unchangingPosCompToAnimMap;
  std::vector<uint32_t>               m_unchangingQuatCompToAnimMap;
  std::vector<uint32_t>               m_sampledPosCompToAnimMap;
  std::vector<uint32_t>               m_sampledQuatCompToAnimMap;
  std::vector<uint32_t>               m_splinePosCompToAnimMap;
  std::vector<uint32_t>               m_splineQuatCompToAnimMap;

  //-----------------------
  // Unchanging pos channel
  Vector3Table*                       m_unchangingPosTable;         ///< Channel table for the unchanging pos channels.
                                                                    ///< Only a single frame for each channel is stored
  Vector3QuantisationTable*           m_unchangingPosQuantisation;  ///< Quantisation information for the sampled pos channel
  NMP::Vector3*                       m_unchangingPosQuantised;     ///< Unchanging pos channels recovered from quantisation

  //-----------------------
  // Unchanging quat channel
  RotVecTable*                        m_unchangingQuatTable;            ///< Channel table for the unchanging quat channels.
                                                                        ///< Only a single frame for each channel is stored and
                                                                        ///< as tan quarter angle rotation vectors
  Vector3QuantisationTable*           m_unchangingQuatQuantisation;     ///< Quantisation information for the sampled pos channel
  NMP::Quat*                          m_unchangingQuatQuantised;        ///< Unchanging quat channels recovered from quantisation

protected:
  //-----------------------
  // Intermediate compiled data
  NMP::Quat                           m_worldspaceRootQuat;             ///< Worldspace quat required to recover the topmost animation data local quat.
                                                                        ///< This is stored since animations

  uint16_t                            m_worldspaceRootID;               ///< Anim channel index corresponding to the parent of the topmost animation data channel.
                                                                        ///< Note that this may not correspond to the CharacterWorldSpaceTM rig channel (0).

  MR::QuantisationInfoQSA             m_unchangingPosQuantisationInfo;  ///< Quantisation info about ranges of all unchanging pos channels
  MR::QuantisationInfoQSA             m_unchangingQuatQuantisationInfo; ///< Quantisation info about ranges of all unchanging quat channels

  size_t                              m_unchangingPosDataSize;
  MR::UnchangingKeyQSA*               m_unchangingPosData;              ///< Table of default values for unvarying pos channels

  size_t                              m_unchangingQuatDataSize;
  MR::UnchangingKeyQSA*               m_unchangingQuatData;             ///< Table of default values for unvarying quat channels (tan quarter rot vecs)

  //-----------------------
  // Compiled channel set info
  size_t                              m_channelSetInfoDataSize;
  MR::ChannelSetInfoQSA*              m_channelSetInfoData;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // CHANNELSET_INFO_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
