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
#ifndef MR_MIRRORED_ANIM_MAPPING_DEF_H
#define MR_MIRRORED_ANIM_MAPPING_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataMirroredAnimMapping
/// \brief Describes hierarchy of bones etc.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataMirroredAnimMapping : public AttribData
{
public:

  /// \struct MR::AttribDataMirroredAnimMapping::SimpleMapping
  /// \brief Holds a mapping between a left and right event (or track) ID.
  struct SimpleMapping
  {
    uint32_t  leftIndex;    ///< left index
    uint32_t  rightIndex;   ///< right index
  };

  static NMP::Memory::Format getMemoryRequirements(uint32_t numValues, uint32_t numEvents, uint32_t numTracks, uint32_t numBones);
  static AttribDataMirroredAnimMapping* init(
    NMP::Memory::Resource& resource,
    uint32_t               numValues,
    uint32_t               numEvents,
    uint32_t               numTracks,
    uint32_t               numBones,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataMirroredAnimMapping() { setType(ATTRIB_TYPE_ANIM_MIRRORED_MAPPING); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_ANIM_MIRRORED_MAPPING; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  /// \brief returns the number of mappings in the map.
  uint32_t getNumMappings() const;

  /// \brief  returned the left bone for the specified mapping index pair.
  uint32_t getLeftBone(uint32_t mappingIndex) const;

  /// \brief returned the right bone for the specified mapping index pair.
  uint32_t getRightBone(uint32_t mappingIndex) const;

  /// \brief Track ID mappings.
  uint32_t getNumTrackIdsToRemap() const;
  uint32_t getLeftTrackIdMapping(const uint32_t index) const;
  uint32_t getRightTrackIdMapping(const uint32_t index) const;

  /// \brief Event user data mappings.
  uint32_t getNumEventIdsToRemap() const;
  uint32_t getLeftEventIdMapping(const uint32_t index) const;
  uint32_t getRightEventIdMapping(const uint32_t index) const;

  /// \brief returns the number of bones
  uint32_t getNumBones() { return m_numBones; }

  /// \brief
  const NMP::Quat* getOffset(uint32_t idx) const;

  /// \return Found mapped track ID or the input track ID if not found.
  uint32_t findTrackIDMapping(uint32_t trackID) const;

  /// \return Fount mapped event ID or the input event ID if not found.
  uint32_t findEventMappingID(uint32_t eventID) const;

  uint32_t       m_axis;            ///< 0=YZ plane, 1=ZX, 2=XY.
  uint32_t       m_numBoneMappings; ///< Number of bone mapping pairs.
  SimpleMapping* m_boneMappings;    ///< The bone mapping pairs array.
  uint32_t       m_numTrackIds;     ///< Number of event track ID's to be re-mapped.
  SimpleMapping* m_trackIds;        ///< The track ID's to be re-mapped.
  uint32_t       m_numEventIds;     ///< Number of event user data ID's to be re-mapped.
  SimpleMapping* m_eventIds;        ///< The event user data ID's to be re-mapped.
  uint32_t       m_numBones;        ///< The number of bones in the rig.
  NMP::Quat*     m_quatOffsets;     ///< The event user data ID's to be re-mapped.
};

//----------------------------------------------------------------------------------------------------------------------
// AttribDataMirroredAnimMapping functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Quat* AttribDataMirroredAnimMapping::getOffset(uint32_t idx) const
{
  return m_quatOffsets + idx;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getNumMappings() const
{
  return m_numBoneMappings;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getLeftBone(uint32_t mappingIndex) const
{
  NMP_ASSERT(m_boneMappings);
  NMP_ASSERT(mappingIndex < getNumMappings());
  return m_boneMappings[mappingIndex].leftIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getRightBone(uint32_t mappingIndex) const
{
  NMP_ASSERT(m_boneMappings);
  NMP_ASSERT(mappingIndex < getNumMappings());
  return m_boneMappings[mappingIndex].rightIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getNumTrackIdsToRemap() const
{
  return m_numTrackIds;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getNumEventIdsToRemap() const
{
  return m_numEventIds;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getLeftTrackIdMapping(const uint32_t index) const
{
  NMP_ASSERT(m_trackIds);
  NMP_ASSERT(index < m_numTrackIds);
  return m_trackIds[index].leftIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getRightTrackIdMapping(const uint32_t index) const
{
  NMP_ASSERT(m_trackIds);
  NMP_ASSERT(index < m_numTrackIds);
  return m_trackIds[index].rightIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getLeftEventIdMapping(const uint32_t index) const
{
  NMP_ASSERT(m_eventIds);
  NMP_ASSERT(index < m_numEventIds);
  return m_eventIds[index].leftIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataMirroredAnimMapping::getRightEventIdMapping(const uint32_t index) const
{
  NMP_ASSERT(m_eventIds);
  NMP_ASSERT(index < m_numEventIds);
  return m_eventIds[index].rightIndex;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_MIRRORED_ANIM_MAPPING_DEF_H
//----------------------------------------------------------------------------------------------------------------------
