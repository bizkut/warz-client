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
#ifndef AP_ANALYSIS_PROPERTY_UTILS_H
#define AP_ANALYSIS_PROPERTY_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrAttribAddress.h"
#include "export/mcExport.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class DataBuffer;
}

namespace MR
{
class AnimRigDef;
class NetworkDef;
class Network;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  kAPModeTrajectory     = 0,            ///< Extract analysis properties from the trajectory motion
  kAPModeRigJoint       = 1,            ///< Extract analysis properties from a specified rig joint
  kAPModeMax,
  kAPInvalidMode        = 0xFFFFFFFF
} AnalysisPropertyMode;

//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  kAPSpaceTrajectoryWorld = 0,            ///< In the co-ordinate frame of the trajectory at the analysis start frame
  kAPSpaceTrajectoryLocal = 1,            ///< In the co-ordinate frame of the trajectory at the previous analysis frame
  kAPSpaceTrajectoryMax,
  kAPInvalidTrajectorySpace = 0xFFFFFFFF
} AnalysisPropertyTrajectorySpace;

//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  kAPSpaceCharacterWorld  = 0,            ///< In the co-ordinate frame of the character root at the analysis start frame
  kAPSpaceCharacterLocal  = 1,            ///< In the co-ordinate frame of the character root at the current analysis frame
  kAPSpaceJointLocal      = 2,            ///< In the co-ordinate frame of the parent joint at the current analysis frame
  kAPSpaceJointMax,
  kAPInvalidJointSpace    = 0xFFFFFFFF
} AnalysisPropertyJointSpace;

//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  kAPXAxis                = 0,
  kAPYAxis                = 1,
  kAPZAxis                = 2,
  kAPAxisMax,
  kAPInvalidAxis          = 0xFFFFFFFF
} AnalysisPropertyAxis;

//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  kAPSampleRangeDefault             = 0,
  kAPSampleRangeAtStart             = 1,
  kAPSampleRangeAtEnd               = 2,
  kAPSampleRangeBetweenSyncEvents   = 3,
  kAPSampleRangeMax,
  kAPInvalidSampleRange             = 0xFFFFFFFF
} AnalysisPropertySampleRange;

//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  kAPDataModelConstant  = 0,
  kAPDataModelLinear    = 1,
  kAPDataModelMax,
  kAPInvalidDataModel   = 0xFFFFFFFF
} AnalysisPropertyDataModelMode;

//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  kAPDataModelEvaluationAtStart   = 0,
  kAPDataModelEvaluationAtEnd     = 1,
  kAPDataModelEvaluationCustom    = 2,
  kAPDataModelEvaluationMax,
  kAPInvalidDataModelEvaluation   = 0xFFFFFFFF
} AnalysisPropertyDataModelEvaluation;

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisAnimSetDependencyMap
//----------------------------------------------------------------------------------------------------------------------
class AnalysisAnimSetDependencyMap
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numAnimSets);

  /// animSetDependencyIndices[i] = i   anim set i has its own data
  /// animSetDependencyIndices[i] = j   where (i != j), anim set i is dependent on anim set j
  /// animSetDependencyIndices[i] = -1  invalid anim set dependency (Analysis properties don't exist?)
  static AnalysisAnimSetDependencyMap* init(
    NMP::Memory::Resource& resource,
    uint32_t numAnimSets,
    const uint32_t* animSetDependencyIndices);

  static AnalysisAnimSetDependencyMap* createFromExport(
    const ME::DataBlockExport* analysisNodeDataBlock);

  void releaseAndDestroy();

public:
  uint32_t    m_numAnimSets;
  uint32_t    m_numUsedAnimSets;
  uint32_t*   m_animSetDependencyIndices;   ///< Array whose entries indicate which anim set it is dependent on
  uint32_t*   m_usedAnimSetIndices;         ///< Array of anim set indices that contain data
  uint32_t*   m_animSetEntryMap;            ///< Array whose entries indicate which used anim set it refers to
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertyDataModel
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertyDataModel
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AnalysisPropertyDataModel* init(NMP::Memory::Resource& resource);

  void readExport(const ME::DataBlockExport* dataBlock);

  // Constant value filtering
  static float fitDataModelConstant(
    uint32_t numSamples,
    const float* samples,
    uint32_t rangeStart,
    uint32_t rangeEnd);

  // Linear regression filtering
  static float fitDataModelLinear(
    uint32_t numSamples,
    const float* samples,
    uint32_t rangeStart,
    uint32_t rangeEnd,
    float paramU);

  // Fit the sample data using the specified mode
  float fitDataModel(
    uint32_t numSamples,
    const float* samples,
    uint32_t rangeStart,
    uint32_t rangeEnd) const;

public:
  AnalysisPropertyDataModelMode   m_dataModelMode;
  float                           m_dataModelParam;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertySubRange
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertySubRange
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numFrames);

  static AnalysisPropertySubRange* init(
    NMP::Memory::Resource& resource,
    uint32_t numFrames);

  void readExport(const ME::DataBlockExport* dataBlock);

  void instanceInit(MR::Network* network);

  void instanceUpdate(MR::Network* network);

  void findSampleRange(uint32_t& rangeStart, uint32_t& rangeEnd) const;

  void findSampleDiffRange(uint32_t& rangeStart, uint32_t& rangeEnd) const;

public:
  // Options
  AnalysisPropertySampleRange m_sampleRangeMode;

  // Sample Sub-Range
  MR::AttribAddress           m_sourceNodeSyncEventPlaybackPosAddress;  ///< Address of the sync event playback pos attrib of the source node.
  MR::AttribAddress           m_sourceNodeSyncEventTrackAddress;        ///< Address of the sync event track attrib of the source node.
  float                       m_syncEventStart;
  float                       m_syncEventEnd;

  // Feature data
  uint32_t                    m_maxNumFrames;
  uint32_t                    m_numFrames;
  bool*                       m_channelFlags;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertySyncEventWatchBuffer
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertySyncEventWatchBuffer
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numFrames,
    uint32_t numSyncEventsToWatch);

  static AnalysisPropertySyncEventWatchBuffer* init(
    NMP::Memory::Resource& resource,
    uint32_t numFrames,
    uint32_t numSyncEventsToWatch);

  NM_INLINE uint32_t getNumSyncEventsToWatch() const { return m_numSyncEventsToWatch; }
  float getSyncEventToWatch(uint32_t watchIndex) const;
  void setSyncEventToWatch(uint32_t watchIndex, float syncEventPos);

  void instanceInit(MR::Network* network);

  void instanceUpdate(MR::Network* network);

  /// \brief remove consecutive triggered events of the same watch type
  void conditionBuffer();

  uint32_t getNumTriggeredEvents() const;
  uint32_t getNumTriggeredEventsOfType(uint32_t watchIndex) const;

  uint32_t findNextTriggeredEvent(uint32_t frameIndex) const;
  uint32_t findNextTriggeredEventOfType(uint32_t frameIndex, uint32_t watchIndex) const;

public:
  MR::AttribAddress           m_sourceNodeSyncEventPlaybackPosAddress;  ///< Address of the sync event playback pos attrib of the source node.
  MR::AttribAddress           m_sourceNodeSyncEventTrackAddress;        ///< Address of the sync event track attrib of the source node.

  // Sync events to watch
  uint32_t                    m_numSyncEventsToWatch;
  float*                      m_syncEventsToWatch;

  // Feature data
  uint32_t                    m_maxNumFrames;
  uint32_t                    m_numFrames;
  uint32_t*                   m_syncEventWatchBuffer;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertySampleBuffer
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertySampleBuffer
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numAnimSets,
    uint32_t maxNumFrames);

  static AnalysisPropertySampleBuffer* init(
    NMP::Memory::Resource& resource,
    uint32_t numAnimSets,
    uint32_t maxNumFrames);

  static AnalysisPropertySampleBuffer* create(
    uint32_t numAnimSets,
    uint32_t maxNumFrames);

  void readExport(
    MR::NetworkDef* networkDef,
    const ME::AnalysisNodeExport* analysisExport);

  void writeFile(const char* filename) const;

  void setTrajectoryMode(
    MR::NetworkDef* networkDef,
    AnalysisPropertyTrajectorySpace space);

  void instanceInit(MR::Network* network);

  void instanceUpdate(MR::Network* network);

public:
  // Character trajectory position
  NMP::Vector3                      m_characterPosition;
  NMP::Quat                         m_characterOrientation;

  // Definition data
  AnalysisPropertyMode              m_mode;
  AnalysisPropertyTrajectorySpace   m_trajectorySpace;

  // Rig joint properties (per anim set)
  AnalysisPropertyJointSpace        m_jointSpace;
  uint32_t                          m_numAnimSets;
  uint32_t*                         m_rigJointIndices;

  // Extracted features
  MR::AnimSetIndex                  m_animSetIndex;
  float                             m_sampleFrequency;
  uint32_t                          m_maxNumFrames;
  uint32_t                          m_numFrames;
  NMP::Vector3*                     m_channelPos;
  NMP::Quat*                        m_channelAtt;
};


//----------------------------------------------------------------------------------------------------------------------
/// \class AnalysisPropertyUtils
//----------------------------------------------------------------------------------------------------------------------
class AnalysisPropertyUtils
{
public:
  //---------------------------
  // Filtering functions
  static float averageOfSamples(
    uint32_t numSamples,
    const float* samples);

  static float linearRegressionSample(
    uint32_t numSamples,
    const float* samples,
    float fFrame);

  static float applySmoothingFilterAtFrame(
    uint32_t wndHalfSize,
    const float* kernel,
    uint32_t iFrame,
    uint32_t numFrames,
    const float* dataIn);

  static float applyLinearFilterAtFrame(
    uint32_t wndHalfSize,
    uint32_t iFrame,
    uint32_t numFrames,
    const float* dataIn);

  static float applyMedianFilterAtFrame(
    uint32_t wndHalfSize,
    uint32_t iFrame,
    uint32_t numFrames,
    const float* dataIn,
    float* medianBuffer);

  static void processSmoothedSamples(
    uint32_t numSamples,
    const float* featureData,
    float* smoothedData);

  static void processSmoothedSamples(
    uint32_t numSamples,
    const float* featureData,
    uint32_t rangeStart,
    uint32_t rangeEnd,
    float* smoothedData);

  static float processSmoothedSampleAtFrame(
    uint32_t numSamples,
    const float* featureData,
    uint32_t iFrame);

  // Assumes that most samples are near zero and uses a heuristic to compute the corresponding
  // probability weights for each sample based on the inlier normal distribution.
  static void calculateWeightsForNearZeroValuedSamples(
    uint32_t numSamples,
    const float* samples,
    float* weights);
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_ANALYSIS_PROPERTY_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
