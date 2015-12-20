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
#ifndef MR_RIG_TO_ANIM_MAP_H
#define MR_RIG_TO_ANIM_MAP_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBitArray.h"
#include "morpheme/mrRig.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::MapPair
/// \brief Class for holding and accessing a simple integer map between rig and anim channel indexes.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------
class MapPair
{
public:
  MapPair() {}
  ~MapPair() {}

  void locate();
  void dislocate();

  void clear() { m_rigChannelIndex = m_animChannelIndex = 0; }

public:
  uint16_t m_rigChannelIndex;
  uint16_t m_animChannelIndex;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::RigToAnimEntryMap
/// \brief Class for holding a table of key, value pairs that correspond to the mapping of
/// rig bone indices to animation channel set indices.
/// \ingroup CoreModule
///
/// The set of rig bone index keys are strictly increasing while the animation channel indexes
/// are not. The rig to anim map must have a one to one correspondence, i.e. no repeated keys
/// or values are allowed.
//----------------------------------------------------------------------------------------------------------------------
class RigToAnimEntryMap
{
public:
  //---------------------------------------------------------------------
  /// \name   Memory management
  /// \brief  Functions to get the memory requirements and initialise the layout
  ///         for a rig to anim channel map
  //---------------------------------------------------------------------
  //@{
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEntries);

  static RigToAnimEntryMap* init(NMP::Memory::Resource& resource, uint32_t numEntries);

  static RigToAnimEntryMap* relocate(void*& ptr);
  
  NM_INLINE NMP::Memory::Format getInstanceMemoryRequirements() const;

  void locate();

  void dislocate();
  //@}

  //---------------------------------------------------------------------
  /// \name   Rig to anim channels
  /// \brief  Accessors for the rig to anim channel maps
  //---------------------------------------------------------------------
  //@{
  
  /// \brief Checks to see if the passed in RigAndAnimEntryMap is the same as this one.
  bool isEqual(const RigToAnimEntryMap* compare) const;
  
  NM_INLINE uint32_t getNumEntries() const;
  
  /// \brief MapPair entry index lookup.
  ///
  /// Asserts if index out of range.
  /// Instant array access, requires no lookup.
  NM_INLINE void setEntryRigChannelIndex(const uint32_t entryIndex, const uint16_t rigChannelIndex);
  NM_INLINE void setEntryAnimChannelIndex(const uint32_t entryIndex, const uint16_t animChannelIndex);
  NM_INLINE uint16_t getEntryRigChannelIndex(const uint32_t entryIndex) const;
  NM_INLINE uint16_t getEntryAnimChannelIndex(const uint32_t entryIndex) const;

  /// \brief Rig and anim channel based lookup functions.
  /// \return true if requested rig or anim channel is found.
  NM_INLINE bool setAnimIndexForRigIndex(const uint16_t rigChannelIndex, const uint16_t animChannelIndex);
  NM_INLINE bool getAnimIndexForRigIndex(const uint16_t rigChannelIndex, uint16_t& animChannelIndex) const;
  NM_INLINE bool setRigIndexForAnimIndex(const uint16_t animChannelIndex, const uint16_t rigChannelIndex);
  NM_INLINE bool getRigIndexForAnimIndex(const uint16_t animChannelIndex, uint16_t& rigChannelIndex) const;

  /// \brief Find the MapPair entry index of an anim or rig channel index.
  /// \return true if requested rig or anim channel is found.
  bool findEntryIndexForRigChannelIndex(const uint16_t rigChannelIndex, uint32_t& entryIndex) const;
  bool findEntryIndexForAnimChannelIndex(const uint16_t animChannelIndex, uint32_t& entryIndex) const;

  /// \brief Look up bone in the rig and find the index that it maps to in the animation.
  /// \return -1 if the rig bone is not found or does not have an equivalent entry in the animation.
  int16_t getAnimChannelIndexFromRigBoneName(const char* rigBoneName, const AnimRigDef* rig) const;
  
  /// \brief Sort the entries by rig channel index
  void sortByRigChannels();
  //@}
  
protected:
  uint32_t       m_numEntries;
  MapPair*       m_entries;
};

//----------------------------------------------------------------------------------------------------------------------
class AnimToRigTableMap
{
public:
  //---------------------------------------------------------------------
  /// \name   Memory management
  /// \brief  Functions to get the memory requirements and initialise the layout
  ///         for a rig to anim channel map
  //---------------------------------------------------------------------
  //@{
  static NMP::Memory::Format getMemoryRequirements(uint32_t numAnimChannels);

  static AnimToRigTableMap* init(NMP::Memory::Resource& resource, uint32_t numAnimChannels);

  static AnimToRigTableMap* relocate(void*& ptr);

  void locate();

  void dislocate();
  //@}

  //---------------------------------------------------------------------
  /// \name   Anim to rig channels
  /// \brief  Accessors for the anim to rig mappings
  //---------------------------------------------------------------------
  //@{

  /// \brief Checks to see if the passed in AnimToRigTableMap is the same as this one.
  bool isEqual(const AnimToRigTableMap* compare) const;

  /// \brief Computes the table entry information
  void setEntryInfo();

  NM_INLINE uint16_t getNumUsedEntries() const;
  NM_INLINE uint16_t getNumAnimChannelsForLOD() const;

  NM_INLINE uint32_t getNumAnimChannels() const;
  NM_INLINE const uint16_t* getAnimToRigEntries() const;
  
  NM_INLINE void setAnimToRigMapEntry(const uint16_t animChannelIndex, const uint16_t rigChannelIndex);
  NM_INLINE const uint16_t& getAnimToRigMapEntry(const uint16_t animChannelIndex) const;
  //@}

protected:
  uint32_t      m_numAnimChannels;
  uint16_t      m_numUsedEntries;
  uint16_t      m_numAnimChannelsForLOD;
  uint16_t*     m_animToRigEntries;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::RigToAnimMap
/// \brief Class for holding a table of key, value pairs that correspond to the mapping of
/// rig bone indices to animation channel set indices.
/// \ingroup CoreModule
///
/// The set of rig bone index keys are strictly increasing while the animation channel indexes
/// are not. The rig to anim map must have a one to one correspondence, i.e. no repeated keys
/// or values are allowed.
/// The rig to anim map also stores compression channel to rig channel mapping tables for the
/// pos and quat parts.
//----------------------------------------------------------------------------------------------------------------------
class RigToAnimMap
{
public:
  // Enumeration describing the three different mapping schemes that can be used with the RigToAnimMap.
  //
  // MapPairs  - A table of (rig, anim) channel mappings.
  //
  // AnimToRig - A one way lookup table of anim to rig channels. Note that this scheme gives quick
  //             decompression performance by recovering the comp to rig channel mappings at decompression
  //             time via the comp to anim maps stored within the animation. Since the lookup table
  //             contains the mapping between anim and rig channels only, the RigToAnimMap will be
  //             agnostic of which channels are unchanging and the compression method used during compilation.
  //             Typically there should be a similar number of RigToAnimMaps as there are rigs.
  //
  // CompToRig - Precompiled compression channel to rig channel lookup tables. Note that this scheme
  //             gives the fastest decompression speed since the mappings between compression channels
  //             and rig channels is performed during asset compilation. However, these mappings are
  //             very dependent on the animation data and how each compression channel is compiled.
  //             Typically there should be as many RigToAnimMaps as there are animations.
  enum RigToAnimMapType
  {
    MapPairs,
    AnimToRig,
    CompToRig
  };

public:
  //---------------------------------------------------------------------
  /// \name   Memory management
  /// \brief  Functions to get the memory requirements and initialise the layout
  ///         for a rig to anim channel map
  //---------------------------------------------------------------------
  //@{
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numRigBones,
    const NMP::Memory::Format& memReqsRigToAnimMapData);

  static RigToAnimMap* init(
    NMP::Memory::Resource& resource,
    RigToAnimMapType rigToAnimMapType,
    uint32_t numRigBones,
    const NMP::Memory::Format& memReqsRigToAnimMapData);
    
  NM_INLINE NMP::Memory::Format getInstanceMemoryRequirements() const;

  void locate();
  void dislocate();
  void relocate();
  //@}

  //---------------------------------------------------------------------
  /// \name   Rig to anim channels
  /// \brief  Accessors for the rig to anim channel maps
  //---------------------------------------------------------------------
  //@{

  /// \brief Checks to see if the passed in RigToAnimMap is the same as this one.
  bool isEqual(const MR::RigToAnimMap* compare) const;
  
  NM_INLINE RigToAnimMapType getRigToAnimMapType() const;

  NM_INLINE uint32_t getNumRigBones() const;
  NM_INLINE const NMP::BitArray* getUsedFlags() const;
  NM_INLINE NMP::BitArray* getUsedFlags();
  
  NM_INLINE void* getRigToAnimMapData();
  NM_INLINE const void* getRigToAnimMapData() const;
  //@}

protected:
  // Header info
  RigToAnimMapType        m_rigToAnimMapType;
  
  // Rig channel used flags
  NMP::BitArray*          m_usedFlags;
  
  // Payload data
  NMP::Memory::Format     m_memReqsRigToAnimMapData;
  void*                   m_rigToAnimMapData;
};

//----------------------------------------------------------------------------------------------------------------------
// Attribute data container for Rig To Anim Maps.
// Array of these held in the NetworkDef as a shared resource.
class AttribDataRigToAnimMap : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataRigToAnimMap* init(
    NMP::Memory::Resource& resource,
    RuntimeAssetID         rigToAnimMapAssetID,
    uint32_t               numRigJoints,
    NMP::Memory::Format&   rigToAnimMapMemoryFormat ///< Memory requirements of the referenced rig to anim map.
  );

  NM_INLINE AttribDataRigToAnimMap() { setType(ATTRIB_TYPE_RIG_TO_ANIM_MAP); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RIG_TO_ANIM_MAP; }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void prepForSPU(AttribData* target, NMP::MemoryAllocator* allocator);

  bool fixupRigToAnimMap();
  bool unfixRigToAnimMap();

  NMP::Memory::Format  m_rigToAnimMapMemReqs; ///< Requirements of referenced rig to anim map.
  RigToAnimMap*        m_rigToAnimMap;        ///< Maps from rig bone index to animation bone index.
                                              ///< Points into shared array of resources.
  RuntimeAssetID       m_rigToAnimMapAssetID; ///< The Network specific runtime ID of the RigToAnimMap asset used by
                                              ///< this binding.
  uint32_t             m_numRigJoints;
};


//----------------------------------------------------------------------------------------------------------------------
// RigToAnimEntryMap functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format RigToAnimEntryMap::getInstanceMemoryRequirements() const
{
  return getMemoryRequirements(m_numEntries);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t RigToAnimEntryMap::getNumEntries() const
{
  return m_numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void RigToAnimEntryMap::setEntryRigChannelIndex(const uint32_t entryIndex, const uint16_t rigChannelIndex)
{
  NMP_ASSERT(entryIndex < m_numEntries);
  m_entries[entryIndex].m_rigChannelIndex = rigChannelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void RigToAnimEntryMap::setEntryAnimChannelIndex(const uint32_t entryIndex, const uint16_t animChannelIndex)
{
  NMP_ASSERT(entryIndex < m_numEntries);
  m_entries[entryIndex].m_animChannelIndex = animChannelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t RigToAnimEntryMap::getEntryRigChannelIndex(const uint32_t entryIndex) const
{
  NMP_ASSERT(entryIndex < m_numEntries);
  return m_entries[entryIndex].m_rigChannelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t RigToAnimEntryMap::getEntryAnimChannelIndex(const uint32_t entryIndex) const
{
  NMP_ASSERT(entryIndex < m_numEntries);
  return m_entries[entryIndex].m_animChannelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool RigToAnimEntryMap::setAnimIndexForRigIndex(const uint16_t rigChannelIndex, const uint16_t animChannelIndex)
{
  uint32_t entryIndex;
  if (findEntryIndexForRigChannelIndex(rigChannelIndex, entryIndex))
  {
    m_entries[entryIndex].m_animChannelIndex = animChannelIndex;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool RigToAnimEntryMap::getAnimIndexForRigIndex(const uint16_t rigChannelIndex, uint16_t& animChannelIndex) const
{
  uint32_t entryIndex;
  if (findEntryIndexForRigChannelIndex(rigChannelIndex, entryIndex))
  {
    animChannelIndex = m_entries[entryIndex].m_animChannelIndex;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool RigToAnimEntryMap::setRigIndexForAnimIndex(const uint16_t animChannelIndex, const uint16_t rigChannelIndex)
{
  uint32_t entryIndex;
  if (findEntryIndexForAnimChannelIndex(animChannelIndex, entryIndex))
  {
    m_entries[entryIndex].m_rigChannelIndex = rigChannelIndex;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool RigToAnimEntryMap::getRigIndexForAnimIndex(const uint16_t animChannelIndex, uint16_t& rigChannelIndex) const
{
  uint32_t entryIndex;
  if (findEntryIndexForAnimChannelIndex(animChannelIndex, entryIndex))
  {
    rigChannelIndex = m_entries[entryIndex].m_rigChannelIndex;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimToRigTableMap
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t AnimToRigTableMap::getNumUsedEntries() const
{
  return m_numUsedEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t AnimToRigTableMap::getNumAnimChannelsForLOD() const
{
  return m_numAnimChannelsForLOD;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimToRigTableMap::getNumAnimChannels() const
{
  return m_numAnimChannels;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* AnimToRigTableMap::getAnimToRigEntries() const
{
  return m_animToRigEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AnimToRigTableMap::setAnimToRigMapEntry(const uint16_t animChannelIndex, const uint16_t rigChannelIndex)
{
  NMP_ASSERT(animChannelIndex < m_numAnimChannels);
  m_animToRigEntries[animChannelIndex] = rigChannelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t& AnimToRigTableMap::getAnimToRigMapEntry(const uint16_t animChannelIndex) const
{
  // Cast to signed integer to allow using -1 index.
  // Check MR::RigToAnimMap::getMemoryRequirements and MR::RigToAnimMap::init.
  NMP_ASSERT((int16_t)animChannelIndex >= -1 && (int16_t)animChannelIndex < (int16_t)m_numAnimChannels);
  return m_animToRigEntries[(int16_t)animChannelIndex];
}

//----------------------------------------------------------------------------------------------------------------------
// RigToAnimMap functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format RigToAnimMap::getInstanceMemoryRequirements() const
{
  return getMemoryRequirements(m_usedFlags->getNumBits(), m_memReqsRigToAnimMapData);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE RigToAnimMap::RigToAnimMapType RigToAnimMap::getRigToAnimMapType() const
{
  return m_rigToAnimMapType;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t RigToAnimMap::getNumRigBones() const
{
  return m_usedFlags->getNumBits();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::BitArray* RigToAnimMap::getUsedFlags() const
{
  return m_usedFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::BitArray* RigToAnimMap::getUsedFlags()
{
  return m_usedFlags;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void* RigToAnimMap::getRigToAnimMapData()
{
  return m_rigToAnimMapData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const void* RigToAnimMap::getRigToAnimMapData() const
{
  return m_rigToAnimMapData;
}

//----------------------------------------------------------------------------------------------------------------------
// Asset management functions
//----------------------------------------------------------------------------------------------------------------------
bool locateRigToAnimMap(uint32_t assetType, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_RIG_TO_ANIM_MAP_H
//----------------------------------------------------------------------------------------------------------------------
