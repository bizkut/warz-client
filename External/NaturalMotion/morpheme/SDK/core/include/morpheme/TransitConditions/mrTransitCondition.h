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
#ifndef MR_TRANSIT_CONDITION_H
#define MR_TRANSIT_CONDITION_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class TransitCondition;
class TransitConditionDef;
class Network;
struct ConditionAttribEntry;

//----------------------------------------------------------------------------------------------------------------------
// Typedefs for register-able TransitCondition functions.

/// \brief This fn locates a transit condition def in a particular address in memory
typedef bool (*TransitCondDefLocateFn)(
  TransitConditionDef* transitCondDef);
/// \brief This fn dislocates a transit condition def from it's location in memory (also endian swapping
/// for the target platform if necessary).
typedef bool (*TransitCondDefDislocateFn)(
  TransitConditionDef* transitCondDef);
/// \brief This fn relocates a transit condition instance
typedef bool (*TransitCondInstanceRelocateFn)(
  TransitCondition* transitCondDef, void* location);
/// \brief This fn gets the memory requirements of a TransitCondition, given the TransitConditionDef.
typedef NMP::Memory::Format (*TransitCondInstanceGetMemReqsFn)(
  TransitConditionDef* transitCondDef);
/// \brief This fn creates an instance of a TransitCondition class from the TransitConditionDef, in the memory provided.
///
/// It actually has 2 modes of operation:
///   1. Creation -  creates an instance of a TransitCondition class from the TransitConditionDef, in the memory provided.
///      This is the first step of initialisation and is indicated by passing a valid memRes where we want to construct the condition.
///      The function should only ever be called in this mode once for any condition.
///   2. Extending the lifespan of dependent data - So if our condition is dependent on some source node data,
///      such as the sampled events buffer, this will extend the lifespan of this buffer so that it remains available for the condition 
///      to interrogate.
///      This is the second step of initialisation and is indicated by passing a NULL memRes.
///      This function will be called once for every source node that shares the transition that uses this condition;
///      this could be several times for ActiveState transitions.
typedef TransitCondition *(*TransitCondInstanceInitFn)(
  TransitConditionDef*   transitCondDef,
  NMP::Memory::Resource& memRes,
  Network*               net,
  NodeID                 sourceNodeID);
/// \brief This is the update function of the condition, it is called on every frame that the condition is active.
typedef bool (*TransitCondInstanceUpdateFn)(
  TransitCondition*    transitCond,
  TransitConditionDef* transitCondDef,
  Network*             net,
  NodeID               smActiveNodeID);
/// \brief
typedef void (*TransitCondInstanceQueueDepsFn)(
  TransitCondition*    transitCond,
  TransitConditionDef* transitCondDef,
  Network*             net,
  NodeID               smActiveNodeID);
/// \brief
typedef void (*TransitCondInstanceResetFn)(
  TransitCondition*    transitCond,
  TransitConditionDef* transitCondDef);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDef
/// \brief Base class for all transition condition definitions.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDef
{
public:
  /// \brief Check if the stored condition has been satisfied and update our condition instance appropriately.
  NM_INLINE bool condUpdate(
    TransitCondition* tc,
    Network*          net,
    NodeID            smActiveNodeID);

  /// \brief Check if the stored condition has been satisfied and update our condition instance appropriately.
  NM_INLINE void condQueueDeps(
    TransitCondition* tc,
    Network*          net,
    NodeID            smActiveNodeID);

  /// \brief Make this condition instance ready for re-use.
  NM_INLINE void condReset(TransitCondition* tc);

  /// \brief Initialise a condition instance from this def in the memory provided.
  NM_INLINE TransitCondition* condInit(
    NMP::Memory::Resource& memRes,
    Network*               net,
    NodeID                 sourceNodeID = INVALID_NODE_ID);

  /// \brief Get memory requirements of a condition instance given this def.
  NM_INLINE NMP::Memory::Format condGetMemoryRequirements();

  TransitConditType getType() const { return m_type; }
  void setType(TransitConditType type) { m_type = type; }

  bool getInvertFlag() const { return m_invert; }
  void setInvertFlag(bool invert) { m_invert = invert; }

  static bool locate(TransitConditionDef* tcDef);
  static bool dislocate(TransitConditionDef* tcDef);
  static bool relocate(TransitConditionDef* tcDef);

  bool condRelocate(TransitCondition* tc, void* location);
  bool condRelocate(TransitCondition* tc) { return condRelocate(tc, tc); }

  // Simply sets conditions m_satisfied flag to false.
  static void defaultInstanceReset(TransitCondition* tc, TransitConditionDef* tcDef);

  // Does nothing.
  static bool defaultInstanceUpdate(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);

  // Does nothing.
  static void defaultInstanceQueueDeps(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);

  // This function can only be used if an inherited instance class has no members.
  static NMP::Memory::Format defaultInstanceGetMemoryRequirements(TransitConditionDef* tcDef);

protected:
  TransitConditionDef() {};
  ~TransitConditionDef() {};

  TransitConditType               m_type;      ///< The unique type ID of this transition condition.
  bool                            m_invert;    ///< Invert the logic of this condition.

  /// Registered functions.
  TransitCondDefLocateFn          m_locateFn;
  TransitCondDefDislocateFn       m_dislocateFn;
  TransitCondInstanceRelocateFn   m_relocateFn;
  TransitCondInstanceGetMemReqsFn m_getMemReqsFn;
  TransitCondInstanceInitFn       m_initFn;
  TransitCondInstanceUpdateFn     m_updateFn;
  TransitCondInstanceQueueDepsFn  m_queueDepsFn;
  TransitCondInstanceResetFn      m_resetFn;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitCondition
/// \brief Base class for all transition conditions.
/// \ingroup TransitConditionsModule
///
/// Defines a condition that will decide whether a state change is triggered.
//----------------------------------------------------------------------------------------------------------------------
class TransitCondition
{
public:
  /// \brief Has this condition been satisfied.
  bool condIsSatisfied() const { return m_satisfied; }

  /// \brief Set the status of this condition.
  /// Overrides anything previously set by the conditions update function, but may be reset on the next update.
  void condSetState(bool state) { m_satisfied = state; }

  /// \brief What is the unique type ID of this transition condition.
  TransitConditType condGetType() const { return m_type; }

protected:
  /// We don't allow construction of TransitCondition - use init to create a new TransitCondition.
  TransitCondition();
  ~TransitCondition();

protected:
  bool               m_satisfied; ///< Has this condition been satisfied.
  TransitConditType  m_type;      ///< The unique type ID of this transition condition.
};

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDef functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool TransitConditionDef::condUpdate(
  TransitCondition* tc,
  Network*          net,
  NodeID            smActiveNodeID)
{
  NMP_ASSERT(m_updateFn);
  return m_updateFn(tc, this, net, smActiveNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransitConditionDef::condQueueDeps(
  TransitCondition* tc,
  Network*          net,
  NodeID            smActiveNodeID)
{
  NMP_ASSERT(m_queueDepsFn);
  m_queueDepsFn(tc, this, net, smActiveNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransitConditionDef::condReset(TransitCondition* tc)
{
  NMP_ASSERT(m_resetFn);
  m_resetFn(tc, this);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE TransitCondition* TransitConditionDef::condInit(
  NMP::Memory::Resource& memRes,
  Network*               net,
  NodeID                 sourceNodeID)
{
  NMP_ASSERT(m_initFn);
  return m_initFn(this, memRes, net, sourceNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format TransitConditionDef::condGetMemoryRequirements()
{
  NMP_ASSERT(m_getMemReqsFn);
  return m_getMemReqsFn(this);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool TransitConditionDef::condRelocate(TransitCondition* tc, void* location)
{
  if (m_relocateFn)
  {
    return m_relocateFn(tc, location);
  }
  else
  {
    return true;
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_H
//----------------------------------------------------------------------------------------------------------------------
