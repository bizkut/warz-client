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
#ifndef MR_PHYSICS_ATTRIB_DATA_H
#define MR_PHYSICS_ATTRIB_DATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class PhysicsRig;
class PhysicsRigDef;
class PresetGroupTagsDef;

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup PhysicsAttribData  Physics-specific Attribute Data.
/// \ingroup NetworkModule
///
/// Physics attribute data is only used by the morpheme physics plugin - it is not used by the animation core.  This
/// attribute data is accessed using helper functions, rather than MR::Network API, so that the animation core does
/// not need to depend on any part of the physics plugin.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsRig
/// \brief The AttribData class that stores the network's physics rig.  Note that the contained pointer may be NULL if
///  no physics rig is currently being used.
/// \ingroup PhysicsAttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsRig : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    PhysicsRig*           physicsRig,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataPhysicsRig() { setType(ATTRIB_TYPE_PHYSICS_RIG); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_RIG; }

  MR::PhysicsRig* m_physicsRig;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsRigDef
/// \brief The AttribData class that stores the network's physics rig definition.  There is a AttribDataPhysicsRigDef
///  for each animation set that contains a PhysicsRigDef.  A network may contain a mixture of animation sets that
///  may or may not have physics rig definitions bound to them
/// \ingroup PhysicsAttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsRigDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataPhysicsRigDef* init(
    NMP::Memory::Resource& resource,
    PhysicsRigDef*         physicsRig);

  NM_INLINE AttribDataPhysicsRigDef() { setType(ATTRIB_TYPE_PHYSICS_RIG_DEF); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_RIG_DEF; }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  MR::PhysicsRigDef* m_physicsRigDef;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimToPhysicsMap
/// \brief Provides a mapping between animation rig indices and physics rig indices.  This class allows pose data to be
///  copied from animation to physics data and vice versa.  The AnimToPhyiscsMap is specific to an animation set.
//----------------------------------------------------------------------------------------------------------------------
class AnimToPhysicsMap
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numChannels, uint32_t numPhysicsChannels);
  static AnimToPhysicsMap* init(NMP::Memory::Resource& resource, uint32_t numChannels, uint32_t numPhysicsChannels);

  bool locate();
  bool dislocate();

  /// Get the part index into the animation rig from the physical rig. There should always be an animation part
  /// so long as the physical part index is valid.
  int32_t getAnimIndexFromPhysicsIndex(int32_t physicsIndex) const;

  /// Get the part index into the physical rig from the animation rig. Returns -1 if there is no physical part, even
  /// if the animation part index is valid.
  int32_t getPhysicsIndexFromAnimIndex(int32_t animIndex) const;

  void makeMappingBetweenPhysicsAndAnim(int32_t physics, int32_t anim);
  void makeMappingBetweenPhysicsAndAnim(const char* physicsPartName, const char* animPartName);
  void initializeBonesFilter();
  bool isInvalidChar(char c);

  /// Mapping from physics rig indices to animation rig indices.
  int32_t* m_mapPhysicsToAnim;
  int32_t  m_numPhysicsChannels;

  /// Mapping from animation rig indices to physics rig indices.
  int32_t* m_mapAnimToPhysics;
  int32_t  m_numAnimChannels;

  AttribDataFloatArray* m_bonesFilter; ///< Indicates which bones are physical (0) or non-physical (1)
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataAnimToPhysicsMap
/// \brief The AttribData class that contains each animation set's AnimToPhysicsMap.
/// \ingroup PhysicsAttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataAnimToPhysicsMap : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numChannels, uint32_t numPhysicsChannels);
  static AttribDataAnimToPhysicsMap* init(
    NMP::Memory::Resource& resource,
    uint32_t               numChannels,
    uint32_t               numPhysicsChannels,
    uint16_t               refCount);

  NM_INLINE AttribDataAnimToPhysicsMap() { setType(ATTRIB_TYPE_ANIM_TO_PHYSICS_MAP); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_ANIM_TO_PHYSICS_MAP; }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  AnimToPhysicsMap* m_animToPhysicsMap;
};


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTrajectoryOverrideDefAnimSet
/// \brief Specifies how the trajectory override node should determine the TM applied to the trajectory. 
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTrajectoryOverrideDefAnimSet : public AttribData
{
public:

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  static AttribDataTrajectoryOverrideDefAnimSet* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataTrajectoryOverrideDefAnimSet() { setType(ATTRIB_TYPE_TRAJECTORY_OVERRIDE_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TRAJECTORY_OVERRIDE_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  void endianSwap();

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are
  // aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t     m_physicsPartIndex;            // Index of part in physics rig.
  float        m_projectOntoGroundDistance;   // Max distance to move trajectory to ensure contact with ground.
#endif
  NMP::Vector3 m_directionInCharacterSpace;  // Direction (in character space) that should align with input direction.
#ifdef NM_HOST_64_BIT
  uint32_t     m_physicsPartIndex;            // Index of part in physics rig.
  float        m_projectOntoGroundDistance;   // Max distance to move trajectory to ensure contact with ground.
#endif
  bool         m_projectOntoGround;           // True if trajectory should be moved down onto a supporting surface.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CollisionFilterMask
/// \brief Morpheme wrapper for physics engine specific collision filter info
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class CollisionFilterMask
{
public:
  CollisionFilterMask()
    : m_word0(0), m_word1(0), m_word2(0), m_word3(0) {}
  CollisionFilterMask(const CollisionFilterMask& cfm)
    : m_word0(cfm.m_word0), m_word1(cfm.m_word1), m_word2(cfm.m_word2), m_word3(cfm.m_word3) {}

  uint32_t m_word0;
  uint32_t m_word1;
  uint32_t m_word2;
  uint32_t m_word3;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataCollisionFilterMask
/// \brief Morpheme wrapper for physics engine specific collision filter info
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataCollisionFilterMask : public AttribData
{
public:

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  static AttribDataCollisionFilterMask* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataCollisionFilterMask() { setType(ATTRIB_TYPE_COLLISION_FILTER_MASK); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_COLLISION_FILTER_MASK; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  /// Payload data
  CollisionFilterMask m_collisionFilterMask;

  void endianSwap();
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_ATTRIB_DATA_H
//----------------------------------------------------------------------------------------------------------------------
