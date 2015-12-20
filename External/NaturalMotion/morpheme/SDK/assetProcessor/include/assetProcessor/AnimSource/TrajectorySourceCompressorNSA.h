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
#ifndef TRAJECTORY_SOURCE_COMPRESSOR_NSA_H
#define TRAJECTORY_SOURCE_COMPRESSOR_NSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/AnimSource/mrTrajectorySourceNSA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressor.h"
#include "assetProcessor/AnimSource/Vector3TableBuilder.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TrajectorySourceCompressorNSA
/// \brief This class is responsible for compiling a trajectory source in the NSA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceCompressorNSA : public TrajectorySourceCompressor
{
public:
  //-----------------------
  // Constructor / Destructor
  TrajectorySourceCompressorNSA();
  ~TrajectorySourceCompressorNSA();
  
  //---------------------------------------------------------------------
  /// \name   Callback functions
  /// \brief  Function handlers that are called by the compressor framework
  //---------------------------------------------------------------------
  //@{

  /// \brief Callback function to initialise the compressor
  void initCompressor() NM_OVERRIDE;

  /// \brief Callback function to perform cleanup prior to terminating the compressor
  void termCompressor() NM_OVERRIDE;

  /// \brief Callback function to compute the final memory requirements for the entire animation
  size_t computeTrajectoryRequirements() const NM_OVERRIDE;

  /// \brief Callback function to build the compiled animation
  void buildTrajectorySource(NMP::Memory::Resource& resource) const NM_OVERRIDE;
  //@}

protected:
  //-----------------------
  // Pos
  void compileSampledPosQuantisationInfo(
    Vector3Table* sampledTrajectoryDeltaPosTable,
    Vector3QuantisationTable* sampledTrajectoryDeltaPosQuantisation);
    
  void compileSampledPosData(
    Vector3Table* sampledTrajectoryDeltaPosTable,
    Vector3QuantisationTable* sampledTrajectoryDeltaPosQuantisation);
  
  void compileUnchangingPosData();

  void checkForUnchangingChannelPos();
  
  void tidyComponentsPos();
  
  //-----------------------
  // Quat  
  void compileSampledQuatQuantisationInfo(
    RotVecTable* sampledTrajectoryDeltaQuatTable,
    Vector3QuantisationTable* sampledTrajectoryDeltaQuatQuantisation);
    
  void compileSampledQuatData(
    RotVecTable* sampledTrajectoryDeltaQuatTable,
    Vector3QuantisationTable* sampledTrajectoryDeltaQuatQuantisation);

  void compileUnchangingQuatData();

  void checkForUnchangingChannelQuat();
  
  void tidyComponentsQuat();

protected:
  // Compiled components
  MR::QuantisationScaleAndOffsetVec3    m_sampledTrajectoryDeltaPosKeysInfo;
  MR::QuantisationScaleAndOffsetVec3    m_sampledTrajectoryDeltaQuatKeysInfo;
  MR::SampledPosKey*                    m_sampledTrajectoryDeltaPosKeys;
  MR::SampledQuatKeyTQA*                m_sampledTrajectoryDeltaQuatKeys;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // TRAJECTORY_SOURCE_COMPRESSOR_NSA_H
//----------------------------------------------------------------------------------------------------------------------
