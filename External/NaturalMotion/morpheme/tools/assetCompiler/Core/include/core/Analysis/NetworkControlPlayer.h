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
#ifndef AP_NETWORK_CONTROL_PLAYER_H
#define AP_NETWORK_CONTROL_PLAYER_H
//----------------------------------------------------------------------------------------------------------------------
#include "export/mcExport.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AnalysisProcessor.h"
#include "Analysis/AnalysisInterval.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class NetworkDef;
class Network;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class AssetProcessor;
class ControlTake;

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NetworkControlPlayer
/// \brief Stores network control state from control parameter and requests over a range of frames.
class NetworkControlPlayer
{
public:
  NetworkControlPlayer();
  ~NetworkControlPlayer();

  bool init(AP::AssetProcessor* processor, const ME::AnalysisTaskExport* task);
  void term();

  NM_INLINE uint32_t getNumTakes() const;
  NM_INLINE const ControlTake* getTake(uint32_t takeIndex) const;

  NM_INLINE MR::NetworkDef* getNetworkDef() const;

  NM_INLINE MR::NodeID getStartStateNodeID() const;

  NM_INLINE uint32_t getFrameCount() const;

  NM_INLINE float getDeltaTime() const;
  NM_INLINE float getSampleFrequency() const;

  NM_INLINE uint32_t getNumDimensions() const;

  /// \brief Get the number of takes that are taken in a specified dimension.
  NM_INLINE uint32_t getNumTakesForDimension(uint32_t dimension) const;

  /// \brief Get the take name from the sample take address. i.e. convert
  /// the take address {1, 2, 3} into the string "Take_1_2_3"
  void getTakeAddressName(char* takeName, uint32_t* takeAddress) const;

  /// \brief Get the sample take address from the take name. i.e. convert
  /// "Take_1_2_3" into the take address components: {1, 2, 3}
  void getTakeAddress(const char* takeName, uint32_t* takeAddress) const;

  /// \brief Clear the sample take address. sampleTakeAddress is an array of length
  /// dimensionCount whose components address the dimensional samples.
  void initTakeAddress(uint32_t* takeAddress) const;

  /// \brief Increment the sample take address. Returns false if the increment
  /// caused the sample take address to overflow to zero.
  bool incrementTakeAddress(uint32_t* takeAddress) const;

  /// \brief Recover the flattened take index from the take address
  uint32_t getTakeIndex(const uint32_t* takeAddress) const;

  NM_INLINE AnalysisInterval* getAnalysisInterval();

  NM_INLINE AssetProcessor* getAssetProcessor() const;

  NM_INLINE const ME::AnalysisNodeExport* getNetworkControlAnalysisNode() const;

private:
  AssetProcessor*               m_processor;
  const ME::AnalysisNodeExport* m_networkControlAnalysisNode;

  uint32_t                    m_frameCount;
  float                       m_deltaTime;

  uint32_t                    m_dimensionCount;
  uint32_t*                   m_dimensionSamplesCount;

  AnalysisInterval*           m_analysisInterval;

  std::vector<ControlTake*>   m_controlTakes;
  MR::NodeID                  m_startStateNodeID;

  MR::NetworkDef*             m_processedNetDef;
};


//----------------------------------------------------------------------------------------------------------------------
// NetworkControlPlayer inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkControlPlayer::getNumTakes() const
{
  return (uint32_t) m_controlTakes.size();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const ControlTake* NetworkControlPlayer::getTake(uint32_t takeIndex) const
{
  if (takeIndex < (uint32_t) m_controlTakes.size())
  {
    return m_controlTakes[takeIndex];
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NetworkDef* NetworkControlPlayer::getNetworkDef() const
{
  return m_processedNetDef;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NodeID NetworkControlPlayer::getStartStateNodeID() const
{
  return m_startStateNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkControlPlayer::getFrameCount() const
{
  return m_frameCount;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NetworkControlPlayer::getDeltaTime() const
{
  return m_deltaTime;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float NetworkControlPlayer::getSampleFrequency() const
{
  if (m_deltaTime > 0.0f)
    return 1.0f / m_deltaTime;
  return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkControlPlayer::getNumDimensions() const
{
  return m_dimensionCount;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t NetworkControlPlayer::getNumTakesForDimension(uint32_t dimension) const
{
  NMP_ASSERT(dimension < m_dimensionCount);
  return m_dimensionSamplesCount[dimension];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AnalysisInterval* NetworkControlPlayer::getAnalysisInterval()
{
  return m_analysisInterval;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AssetProcessor* NetworkControlPlayer::getAssetProcessor() const
{
  return m_processor;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const ME::AnalysisNodeExport* NetworkControlPlayer::getNetworkControlAnalysisNode() const
{
  return m_networkControlAnalysisNode;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_NETWORK_CONTROL_PLAYER_H
//----------------------------------------------------------------------------------------------------------------------
