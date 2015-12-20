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
#ifndef ANIM_SOURCE_MOTION_ANALYSER_H
#define ANIM_SOURCE_MOTION_ANALYSER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AnimSource/AnimSourceUncompressed.h"
#include "assetProcessor/AnimSource/Vector3TableBuilder.h"
#include "NMNumerics/NMPosSpline.h"
#include "NMNumerics/NMQuatSpline.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimSourceMotionAnalyser
/// \brief A class that analyses the motion of an uncompressed runtime ready animation source.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceMotionAnalyser
{
public:
  //-----------------------
  // Constructor / Destructor
  AnimSourceMotionAnalyser();
  ~AnimSourceMotionAnalyser();
  
  void tidy();

  /// \brief Main analysis function
  void analyse(
    const AnimSourceUncompressed* inputAnim,
    const MR::AnimRigDef*         rig,
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap,
    NMP::BasicLogger*             messageLogger,
    NMP::BasicLogger*             errorLogger);

  //-----------------------
  // Pointers to external data
  const AnimSourceUncompressed* getInputAnimation() const { return m_inputAnim; }
  const MR::AnimRigDef* getRig() const { return m_rig; }
  const MR::RigToAnimEntryMap* getRigToAnimEntryMap() const { return m_rigToAnimEntryMap; }
  NMP::BasicLogger* getMessageLogger() const { return m_messageLogger; }
  NMP::BasicLogger* getErrorLogger() const { return m_errorLogger; }

  //-----------------------
  // Accessors
  const ChannelSetTable* getChannelSetsLS() const { return m_channelSetsLS; }
  const ChannelSetTable* getChannelSetsWS() const { return m_channelSetsWS; }

  float getAverageMaxBoundsWS() const { return m_averageMaxBoundsWS; }
  float getAverageBoneLength() const { return m_averageBoneLength; }
  float getAverageInterFrameDisplacement() const { return m_averageInterFrameDisplacement; }

protected:
  void computeAverageMaxBoundsWS();
  void computeAverageBoneLength();
  void computeAverageInterFrameDisplacementWS();

protected:
  //-----------------------
  // Pointers to external data
  const AnimSourceUncompressed*       m_inputAnim;
  const MR::AnimRigDef*               m_rig;
  const MR::RigToAnimEntryMap*        m_rigToAnimEntryMap;

  NMP::BasicLogger*                   m_messageLogger;
  NMP::BasicLogger*                   m_errorLogger;

  //-----------------------
  // Local and worldspace transforms for the original sampled data in rig bone index order
  // with missing channels filled with the bind pose data
  ChannelSetTable*                    m_channelSetsLS;
  ChannelSetTable*                    m_channelSetsWS;

  //-----------------------
  // Statistics
  float                               m_averageMaxBoundsWS;
  float                               m_averageBoneLength;
  float                               m_averageInterFrameDisplacement;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_MOTION_ANALYSER_H
//----------------------------------------------------------------------------------------------------------------------
