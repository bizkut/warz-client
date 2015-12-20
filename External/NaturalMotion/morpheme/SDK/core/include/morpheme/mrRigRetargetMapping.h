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
#ifndef  MR_RIG_RETARGET_MAPPING_DEF_H
#define  MR_RIG_RETARGET_MAPPING_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "NMRetarget/NMFKRetarget.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataRigRetargetMapping
/// \brief Defines how a rig will map to other rigs, both topologically and geometrically.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataRigRetargetMapping : public AttribData
{
public:

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Construction and Initialisation
  /// @{

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJoints);

  /// \brief Formats a given memory resource to contain one of these objects.
  static AttribDataRigRetargetMapping* init(
    NMP::Memory::Resource &resource,
    uint32_t numJoints,
    uint16_t refCount = 0);

  NM_INLINE AttribDataRigRetargetMapping() { setType(ATTRIB_TYPE_RIG_RETARGET_MAPPING); setRefCount(0); }

  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RIG_RETARGET_MAPPING; }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation and registration
  /// @{

  /// \brief Reformat this object for this platform.
  static void locate(AttribData* target);

  /// \brief Reformat this object for another platform.
  static void dislocate(AttribData* target);

  /// \brief Recalculate locations of trailing memory for this platform.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Accessors and modifiers
  /// @{

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Data
  /// @{

  /// \brief Local copy of the size of this rig.
  uint32_t m_numJoints;

  /// \brief The low level retarget params object, which contains most of the mapping data.
  NMRU::FKRetarget::Params* m_retargetParams;

  /// @}

}; // End of class MR::AttribDataRigRetargetMapping

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataRetargetStorageStats
/// \brief Stores the information needed to correctly allocate memory for the retarget solver for a network.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataRetargetStorageStats : public AttribData
{
public:
  //----------------------------------------------------------------------------------------------------------------------
  /// \name Construction and Initialisation
  /// @{

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements();

  /// \brief Formats a given memory resource to contain one of these objects.
  static AttribDataRetargetStorageStats* init(NMP::Memory::Resource &resource, uint16_t refCount = 0);

  NM_INLINE AttribDataRetargetStorageStats() { setType(ATTRIB_TYPE_RETARGET_STORAGE_STATS); setRefCount(0); }

  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RETARGET_STORAGE_STATS; }
  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation and registration
  /// @{

  /// \brief Reformat this object for this platform.
  static void locate(AttribData* target);

  /// \brief Reformat this object for another platform.
  static void dislocate(AttribData* target);

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Data
  /// @{

  /// \brief Info about the rigs to be retargeted needed to determine the memory requirements
  /// for the retarget solver.
  NMRU::FKRetarget::Solver::StorageStats m_solverStorageStats;

  /// @}

}; // End of class MR::AttribDataRetargetStorageStats

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataRetargetState
/// \brief State data for a Retarget node, in particular, its Solver object for actually doing the retargeting.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataRetargetState : public AttribData
{
public:
  //----------------------------------------------------------------------------------------------------------------------
  /// \name Construction and Initialisation
  /// @{

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements(
    const NMRU::FKRetarget::Solver::StorageStats solverStorageStats);

  /// \brief Formats a given memory resource to contain one of these objects.
  static AttribDataRetargetState* init(
    NMP::Memory::Resource &resource,
    const NMRU::FKRetarget::Solver::StorageStats solverStorageStats,
    uint16_t refCount = 0);

  /// \brief Allocate and initialise using the provided allocator.
  static AttribDataHandle create(
    NMP::MemoryAllocator *allocator,
    const NMRU::FKRetarget::Solver::StorageStats solverStorageStats,
    uint16_t refCount = 0);

  NM_INLINE AttribDataRetargetState() { setType(ATTRIB_TYPE_RETARGET_STATE); setRefCount(0); }

  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RETARGET_STATE; }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation and registration
  /// @{

  /// \brief Reformat this object for this platform.
  static void locate(AttribData* target);

  /// \brief Reformat this object for another platform.
  static void dislocate(AttribData* target);

  /// \brief Relocate internal pointers after memory move.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Data
  /// @{

  /// \brief Identifies the source rig we are retargeting from.
  uint32_t m_sourceAnimSetIndex;

  /// \brief Identifies the target rig we are retargeting to.
  uint32_t m_targetAnimSetIndex;

  /// \brief The retarget solver object, does the retargeting for a Retarget node.
  NMRU::FKRetarget::Solver* m_retargetSolver;

  /// @}

}; // End of class MR::AttribDataRetargetState

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataScaleCharacterState
/// \brief State data for a ScaleCharacter node, in particular, the rig scale and scales per joint.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataScaleCharacterState : public AttribData
{
public:
  //----------------------------------------------------------------------------------------------------------------------
  /// \name Construction and Initialisation
  /// @{

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJoints);

  /// \brief Formats a given memory resource to contain one of these objects.
  static AttribDataScaleCharacterState* init(
    NMP::Memory::Resource &resource,
    uint32_t numJoints,
    uint16_t refCount = 0);

  /// \brief Allocate and initialise using the provided allocator.
  static AttribDataHandle create(
    NMP::MemoryAllocator *allocator,
    AttribDataCreateDesc* desc,
    uint16_t refCount = 0);

  NM_INLINE AttribDataScaleCharacterState() { setType(ATTRIB_TYPE_SCALECHARACTER_STATE); setRefCount(0); }

  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SCALECHARACTER_STATE; }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation and registration
  /// @{

  /// \brief Reformat this object for this platform.
  static void locate(AttribData* target);

  /// \brief Reformat this object for another platform.
  static void dislocate(AttribData* target);

  /// \brief Relocate internal pointers after memory move.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Data
  /// @{

  /// \brief Number of joints in this rig.
  uint32_t m_numJoints;

  /// \brief The scales for each joint.
  float* m_jointScale;

  /// @}

}; // End of class MR::AttribDataScaleCharacterState


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_RIG_RETARGET_MAPPING_DEF_H
//----------------------------------------------------------------------------------------------------------------------
