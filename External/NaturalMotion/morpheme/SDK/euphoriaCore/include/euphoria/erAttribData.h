// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef ER_ATTRIBDATA_H
#define ER_ATTRIBDATA_H

//----------------------------------------------------------------------------------------------------------------------
// Node types.
#include "morpheme/nodes/mrNodes.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrAttribAddress.h"
#include "euphoria/erDefines.h"
#include "euphoria/nodes/erNodeOperatorHit.h"
#include "euphoria/nodes/erNodeOperatorRollDownStairs.h"
#include "euphoria/nodes/erNodeOperatorPhysicalConstraint.h"

#include "physics/mrPhysicsRig.h"
#include "euphoria/erBodyDef.h"

#include "euphoria/erInteractionProxyDef.h"

//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

class Character;
struct BehaviourMessageParams;

//----------------------------------------------------------------------------------------------------------------------
class AttribDataBehaviourParameters : public MR::AttribData
{
public:
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_BEHAVIOUR_PARAMETERS; }

  static AttribDataBehaviourParameters* create(
    NMP::MemoryAllocator* allocator,
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numInputCPInts,
    uint32_t numInputCPFloats,
    uint32_t numInputCPUInt64s,
    uint32_t numInputCPVector3s,
    uint32_t numControlParameterInputs,
    uint32_t numControlParameterOutputs,
    uint16_t refCount = 0
  );

  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numControlParameterInputs,
    uint32_t numControlParameterOutputs
  );

  static AttribDataBehaviourParameters* init(
    NMP::Memory::Resource& resource,
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numInputCPInts,
    uint32_t numInputCPFloats,
    uint32_t numInputCPUInt64s,
    uint32_t numInputCPVector3s,
    uint32_t numControlParameterInputs,
    uint32_t numControlParameterOutputs,
    uint16_t refCount = 0);

  NM_INLINE AttribDataBehaviourParameters() { setType(ATTRIB_TYPE_BEHAVIOUR_PARAMETERS); setRefCount(0); }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  MR::AttribDataIntArray*   m_ints;
  MR::AttribDataFloatArray* m_floats;
  MR::AttribDataUInt64Array* m_uint64s;

  // Types must match up with the manifest lua file.
  struct OutputControlParam
  {
    MR::AttribDataSemanticEnum type;
    void* data;
  };

  MR::AttribDataIntArray*   m_controlParameterInputsTypes;
  MR::AttribDataIntArray*   m_controlParameterOutputsTypes;

  uint32_t m_numInputCPInts;
  uint32_t m_numInputCPFloats;
  uint32_t m_numInputCPUInt64s;
  uint32_t m_numInputCPVector3s;
};


//----------------------------------------------------------------------------------------------------------------------
class AttribDataBehaviourState : public MR::AttribData
{
public:
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_BEHAVIOUR_STATE; }

  static AttribDataBehaviourState* create(
    NMP::MemoryAllocator* allocator,
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numVector3s,
    uint16_t refCount = 0
    );

  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numVector3s
    );

  static AttribDataBehaviourState* init(
    NMP::Memory::Resource& resource,
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numVector3s,
    uint16_t refCount = 0);

  NM_INLINE AttribDataBehaviourState() { setType(ATTRIB_TYPE_BEHAVIOUR_STATE); setRefCount(0); }

  MR::AttribDataIntArray*     m_ints;
  MR::AttribDataFloatArray*   m_floats;
  MR::AttribDataUInt64Array*  m_uint64s;
  MR::AttribDataFloatArray*   m_vector3Data; // stored as 4 floats per vector3
  bool m_startedBehaviour;
};

//----------------------------------------------------------------------------------------------------------------------
struct BehaviourAnimationData
{
  int32_t m_animationID;
  bool m_isSet; // true if this comes from a real input, false if it's default
  uint32_t m_numTMs;
  NMP::Matrix34* m_TMs;
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataCharacter : public MR::AttribData
{
public:
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_CHARACTER; }

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataCharacter* create(
    NMP::MemoryAllocator*   allocator,
    Character*              character,
    uint16_t                refCount = 0
  );
  static AttribDataCharacter* init(
    NMP::Memory::Resource&  resource,
    Character*              character,
    uint16_t                refCount = 0);

  NM_INLINE AttribDataCharacter() { setType(ATTRIB_TYPE_CHARACTER); }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  Character* m_character;
};

//----------------------------------------------------------------------------------------------------------------------
// Setup data for the Behaviour node
class AttribDataBehaviourSetup : public MR::AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numChildNodeIDs);
  static AttribDataBehaviourSetup* init(
    NMP::Memory::Resource& resource,
    uint32_t               numChildNodeIDs,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataBehaviourSetup() { setType(ATTRIB_TYPE_BEHAVIOUR_SETUP); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_BEHAVIOUR_SETUP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  int32_t  m_behaviourID;
  MR::AttribDataIntArray* m_childNodeIDs;
};

//----------------------------------------------------------------------------------------------------------------------
// Describes hierarchy of limbs etc.
class AttribDataBodyDef : public MR::AttribData
{
public:
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_BODY_DEF; }

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataBodyDef* init(NMP::Memory::Resource& resource, BodyDef* bodyDef);

  NM_INLINE AttribDataBodyDef() { setType(ATTRIB_TYPE_BODY_DEF); }

  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);

  BodyDef*             m_bodyDef;     ///< Pointer to the (possibly shared) BodyDef itself.
  NMP::Memory::Format  m_rigMemReqs;  ///< Requirements of referenced bodyDef.
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataInteractionProxyDef : public MR::AttribData
{
public:
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_INTERACTION_PROXY_DEF; }

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataInteractionProxyDef* init(NMP::Memory::Resource& resource, InteractionProxyDef* interactionProxyDef);

  NM_INLINE AttribDataInteractionProxyDef() { setType(ATTRIB_TYPE_INTERACTION_PROXY_DEF); }

  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);

  InteractionProxyDef*  m_interactionProxyDef; ///< Pointer to the InteractionProxyDef itself.
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Return the ER::Character for the network if it exists
///  This function may return NULL if no ER::Character has been assigned to the MR::Network.
Character* networkGetCharacter(MR::Network* network);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Allow setting of the ER::Character for this network.
///  It is valiad to pass NULL into this function, although some behaviour nodes require an ER::Character and will
///  assert if one is not present.
void networkSetCharacter(MR::Network* network, Character* character);

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_ATTRIBDATA_H
//----------------------------------------------------------------------------------------------------------------------
