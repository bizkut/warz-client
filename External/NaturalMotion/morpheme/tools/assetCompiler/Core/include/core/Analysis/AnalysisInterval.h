// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef AP_ANALYSIS_INTERVAL_H
#define AP_ANALYSIS_INTERVAL_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVectorContainer.h"
#include "Analysis/AnalysisCondition.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class OrderedStringTable;
}

namespace MR
{
class NetworkDef;
class Network;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisInterval
//----------------------------------------------------------------------------------------------------------------------
class AnalysisInterval
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t maxNumConditions, uint32_t maxNumFrames);

  static AnalysisInterval* init(NMP::Memory::Resource& resource, uint32_t maxNumConditions, uint32_t maxNumFrames);

  static AnalysisInterval* create(uint32_t maxNumConditions, uint32_t maxNumFrames);

  void releaseAndDestroy();

  void readAnalysisConditions(
    AP::AssetProcessor* processor,
    const MR::NetworkDef* networkDef,
    const ME::AnalysisTaskExport* task,
    const ME::AnalysisNodeExport* networkControl);

  void instanceInit(MR::Network* network);
  void instanceUpdate(MR::Network* network, uint32_t frameIndex);
  void instanceReset();

  NM_INLINE uint32_t getMaxNumFrames() const;

  NM_INLINE NMP::VectorContainer<AnalysisCondition*>* getStartConditions() const;
  NM_INLINE uint32_t getStartFrame() const;
  NM_INLINE void setStartFrame(uint32_t startFrame);
  NM_INLINE bool getStartTriggered() const;
  NM_INLINE void setStartTriggered(bool startTriggered);

  NM_INLINE NMP::VectorContainer<AnalysisCondition*>* getStopConditions() const;
  NM_INLINE uint32_t getStopFrame() const;
  NM_INLINE void setStopFrame(uint32_t stopFrame);
  NM_INLINE bool getStopTriggered() const;
  NM_INLINE void setStopTriggered(bool stopTriggered);

private:
  NMP::VectorContainer<AnalysisCondition*>*   m_startConditions;
  NMP::VectorContainer<AnalysisCondition*>*   m_stopConditions;
  uint32_t                                    m_maxNumFrames;
  uint32_t                                    m_startFrame;
  uint32_t                                    m_stopFrame;
  bool                                        m_startTriggered;
  bool                                        m_stopTriggered;
};

//----------------------------------------------------------------------------------------------------------------------
// AnalysisInterval inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnalysisInterval::getMaxNumFrames() const
{
  return m_maxNumFrames;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::VectorContainer<AnalysisCondition*>* AnalysisInterval::getStartConditions() const
{
  return m_startConditions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnalysisInterval::getStartFrame() const
{
  return m_startFrame;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisInterval::setStartFrame(uint32_t startFrame)
{
  m_startFrame = startFrame;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AnalysisInterval::getStartTriggered() const
{
  return m_startTriggered;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisInterval::setStartTriggered(bool startTriggered)
{
  m_startTriggered = startTriggered;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::VectorContainer<AnalysisCondition*>* AnalysisInterval::getStopConditions() const
{
  return m_stopConditions;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnalysisInterval::getStopFrame() const
{
  return m_stopFrame;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisInterval::setStopFrame(uint32_t stopFrame)
{
  m_stopFrame = stopFrame;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AnalysisInterval::getStopTriggered() const
{
  return m_stopTriggered;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnalysisInterval::setStopTriggered(bool stopTriggered)
{
  m_stopTriggered = stopTriggered;
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_INTERVAL_H
//----------------------------------------------------------------------------------------------------------------------
