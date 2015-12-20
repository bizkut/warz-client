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
#ifndef ANIM_SOURCE_COMPRESSOR_NSA_H
#define ANIM_SOURCE_COMPRESSOR_NSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AnimSource/AnimSourceCompressor.h"
#include "assetProcessor/AnimSource/TrajectorySourceCompressorNSA.h"
#include "assetProcessor/AnimSource/SectionCompilerNSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::AnimSourceCompressorNSA
/// \brief    A compressor class to compile the uncompressed runtime animation into the NSA format.
/// \ingroup  CompressedAnimationAssetProcessorModule
///
/// The NSA compressor is divided into three components:
/// 1: The compressor - Responsible for overall management of compression options and the compilation pipeline.
///
/// 2: The section compiler - Responsible for compiling the uncompressed animation data into individual
///    binary components, ready for eventual re-assembly.
///
/// 3: The animation builder - Responsible for assembling the compiled pieces into a runtime animation.
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorNSA : public AnimSourceCompressor
{
public:
  //-----------------------
  // Constructor / Destructor
  AnimSourceCompressorNSA();
  ~AnimSourceCompressorNSA();

protected:
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
  size_t computeFinalMemoryRequirements() const NM_OVERRIDE;

  /// \brief Callback function to build the compiled animation
  void buildAnimation(NMP::Memory::Resource& buffer) const NM_OVERRIDE;

  /// \brief Callback function to build the compiled RigToAnimMap
  void buildRigToAnimMaps() NM_OVERRIDE;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \name   Compression functions
  /// \brief  Functions that transform data into the required compressed binary format
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to compile the trajectory source into the required binary format
  void compileTrajectoryData();
  //@}

protected:
  // Compiled data
  UnchangingDataCompiledNSA*            m_unchangingDataCompiled;
  SampledDataCompiledNSA*               m_sampledDataCompiled;
  
  // Trajectory source
  NMP::Memory::Format                   m_memReqsTrajectorySource;
  MR::TrajectorySourceNSA*              m_trajectorySource;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_COMPRESSOR_NSA_H
//----------------------------------------------------------------------------------------------------------------------
