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
#ifndef AC_ANIM_INFO_H
#define AC_ANIM_INFO_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include <string>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \struct AP::acAnimInfo
/// \brief Base class for an animation converter.
/// \ingroup AnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
struct acAnimInfo
{
  acAnimInfo() :
    m_StartTime(0),
    m_EndTime(0),
    m_FramesPerSecond(0),
    m_NumFrames(0),
    m_TakeName(),
    m_TakeIndex(0)
  {}
  acAnimInfo(const acAnimInfo& ai)
  {
    m_StartTime = ai.m_StartTime;
    m_EndTime = ai.m_EndTime;
    m_FramesPerSecond = ai.m_FramesPerSecond;
    m_NumFrames = ai.m_NumFrames;
    m_TakeName = ai.m_TakeName;
    m_TakeIndex = ai.m_TakeIndex;
  }

  float       m_StartTime;        ///< start time in seconds.
  float       m_EndTime;          ///< end time in seconds.
  float       m_FramesPerSecond;  ///< number of frames per second in the cycle.
  uint32_t    m_NumFrames;        ///< number of frames in the cycle.

  std::string m_TakeName;
  uint32_t    m_TakeIndex;        ///< Within the source animation file (XMD file may contain multiple takes).

  // Bind pose details
  enum {
    kBindPoseFPS = 30,              ///< The number of frames per second that we put in the animation representing the bind pose (created when there are no takes or cycles in an animation file)
    kBindPoseTakeIndex = 0xffffffff ///< The take index for m_TakeIndex in a bind pose animation (created when there are no takes or cycles in an animation file)
  };
};

typedef std::vector<acAnimInfo> acAnimInfos;

NM_INLINE acAnimInfo* findTake(const char*takeName, acAnimInfos& animInfos)
{
  for (acAnimInfos::iterator it = animInfos.begin(), end = animInfos.end(); it != end; ++it)
  {
    acAnimInfo& info = *it;
    if (strcmp(info.m_TakeName.c_str(), takeName) == 0)
    {
      return &info;
    }
  }
  return NULL;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AC_ANIM_INFO_H
//----------------------------------------------------------------------------------------------------------------------
