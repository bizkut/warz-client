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
#ifndef TRAJECTORY_SOURCE_COMPRESSOR_QSA_H
#define TRAJECTORY_SOURCE_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/AnimSource/mrTrajectorySourceQSA.h"
#include "assetProcessor/AnimSource/Vector3TableBuilder.h"
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
#include "assetProcessor/AnimSource/Vector3QuantisationTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorQSA;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TrajectorySourceCompressorQSA
/// \brief This class is responsible for compiling a trajectory source in the QSA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceCompressorQSA
{
public:
  //-----------------------
  // Constructor / Destructor
  TrajectorySourceCompressorQSA(AnimSourceCompressorQSA* manager);
  ~TrajectorySourceCompressorQSA();

  void initCompressor();
  void termCompressor();

  void compileTrajectorySource();

  size_t computeTrajectoryRequirements() const;

  void buildTrajectorySource(
    NMP::Memory::Resource& buffer,
    uint8_t*               data) const;

protected:
  void sampledTrajectoryDeltaPosComputeInfo();
  void sampledTrajectoryDeltaQuatComputeInfo();
  void sampledTrajectoryDeltaPosQuantise();
  void sampledTrajectoryDeltaQuatQuantise();

protected:
  AnimSourceCompressorQSA*            m_manager;

  //-----------------------
  // Sampled delta pos channel
  Vector3Table*                       m_sampledTrajectoryDeltaPosTable;
  Vector3QuantisationTable*           m_sampledTrajectoryDeltaPosQuantisation;

  //-----------------------
  // Sampled delta quat channel
  RotVecTable*                        m_sampledTrajectoryDeltaQuatTable;
  Vector3QuantisationTable*           m_sampledTrajectoryDeltaQuatQuantisation;

protected:
  //-----------------------
  // Compiled trajectory source
  MR::TrajectoryKeyInfoQSA            m_sampledTrajectoryDeltaPosKeysInfo;
  MR::TrajectoryKeyInfoQSA            m_sampledTrajectoryDeltaQuatKeysInfo;

  uint32_t                            m_sampledTrajectoryDeltaPosSize;
  uint32_t                            m_sampledTrajectoryDeltaQuatSize;
  MR::TrajectoryPosKeyQSA*            m_sampledTrajectoryDeltaPosKeys;
  MR::TrajectoryQuatKeyQSA*           m_sampledTrajectoryDeltaQuatKeys;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // TRAJECTORY_SOURCE_COMPRESSOR_QSA_H
//----------------------------------------------------------------------------------------------------------------------
