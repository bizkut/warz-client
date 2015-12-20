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
#ifndef ANIM_SOURCE_BUILDER_NSA_H
#define ANIM_SOURCE_BUILDER_NSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class UnchangingDataNSABuilder : public MR::UnchangingDataNSA
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t unchangingPosNumChannels,
    uint32_t unchangingQuatNumChannels);
  
  static MR::UnchangingDataNSA* init(
    NMP::Memory::Resource& resource,
    uint32_t unchangingPosNumChannels,
    uint32_t unchangingQuatNumChannels,
    const MR::QuantisationScaleAndOffsetVec3& unchangingPosQuantisationInfo,
    const MR::QuantisationScaleAndOffsetVec3& unchangingQuatQuantisationInfo,
    const MR::UnchangingKeyVec3* unchangingPosData,
    const MR::UnchangingKeyVec3* unchangingQuatData);
};

//----------------------------------------------------------------------------------------------------------------------
class SectionDataNSABuilder : public MR::SectionDataNSA
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numSectionAnimFrames,
    uint32_t sampledPosNumChannels,
    uint32_t sampledQuatNumChannels);

  static MR::SectionDataNSA* init(
    NMP::Memory::Resource& resource,
    uint32_t numSectionAnimFrames,
    uint32_t sampledPosNumChannels,
    uint32_t sampledQuatNumChannels,
    const MR::QuantisationMeanAndSetVec3* sampledPosQuantisationData,
    const MR::SampledPosKey* sampledPosData,
    const MR::QuantisationMeanAndSetVec3* sampledQuatQuantisationData,
    const MR::SampledQuatKeyTQA* sampledQuatData);

public:
  NM_INLINE uint32_t getNumSectionAnimFrames() const { return m_numSectionAnimFrames; }

  NM_INLINE uint32_t getSampledPosNumChannels() const { return m_sampledPosNumChannels; }
  NM_INLINE uint32_t getSampledQuatNumChannels() const { return m_sampledQuatNumChannels; }

  NM_INLINE const MR::QuantisationMeanAndSetVec3* getSampledPosQuantisationData() const { return m_sampledPosQuantisationData; }
  NM_INLINE const MR::SampledPosKey* getSampledPosData() const { return m_sampledPosData; }

  NM_INLINE const MR::QuantisationMeanAndSetVec3* getSampledQuatQuantisationData() const { return m_sampledQuatQuantisationData; }
  NM_INLINE const MR::SampledQuatKeyTQA* getSampledQuatData() const { return m_sampledQuatData; }
};

//----------------------------------------------------------------------------------------------------------------------
class AnimSourceNSABuilder : public MR::AnimSourceNSA
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numFrameSections,
    uint32_t numChannelSections,
    const MR::CompToAnimChannelMap* unchangingPosCompToAnimMap,
    const MR::CompToAnimChannelMap* unchangingQuatCompToAnimMap,
    const MR::CompToAnimChannelMap* const* sampledPosCompToAnimMaps,
    const MR::CompToAnimChannelMap* const* sampledQuatCompToAnimMaps,
    uint32_t sampledPosNumQuantisationSets,
    uint32_t sampledQuatNumQuantisationSets,
    const uint32_t* sectionStartFrames,
    const MR::TrajectorySourceNSA* trajectoryData,
    const NMP::OrderedStringTable* channelNames);

  static MR::AnimSourceNSA* init(
    NMP::Memory::Resource& resource,
    float duration,
    float sampleFrequency,
    uint32_t numChannelSets,
    uint32_t numFrameSections,
    uint32_t numChannelSections,
    const MR::CompToAnimChannelMap* unchangingPosCompToAnimMap,
    const MR::CompToAnimChannelMap* unchangingQuatCompToAnimMap,
    const MR::CompToAnimChannelMap* const* sampledPosCompToAnimMaps,
    const MR::CompToAnimChannelMap* const* sampledQuatCompToAnimMaps,
    const MR::QuantisationScaleAndOffsetVec3* posMeansQuantisationInfo,
    uint32_t sampledPosNumQuantisationSets,
    uint32_t sampledQuatNumQuantisationSets,
    const MR::QuantisationScaleAndOffsetVec3* sampledPosQuantisationInfo,
    const MR::QuantisationScaleAndOffsetVec3* sampledQuatQuantisationInfo,
    const MR::UnchangingDataNSA* unchangingData,
    const uint32_t* sectionStartFrames,
    const MR::SectionDataNSA* const* sectionDataTable,
    const MR::TrajectorySourceNSA* trajectoryData,
    const NMP::OrderedStringTable* channelNames);
};

//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceNSABuilder : private MR::TrajectorySourceNSA
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numAnimFrames,
    bool isDeltaPosChanging,
    bool isDeltaQuatChanging);

  static MR::TrajectorySourceNSA* init(
    NMP::Memory::Resource& resource,
    uint32_t numAnimFrames,
    float sampleFrequency,
    const MR::QuantisationScaleAndOffsetVec3& sampledDeltaPosKeysInfo,
    const MR::QuantisationScaleAndOffsetVec3& sampledDeltaQuatKeysInfo,
    const MR::SampledPosKey* sampledDeltaPosKeys,
    const MR::SampledQuatKeyTQA* sampledDeltaQuatKeys);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_BUILDER_NSA_H
//----------------------------------------------------------------------------------------------------------------------
