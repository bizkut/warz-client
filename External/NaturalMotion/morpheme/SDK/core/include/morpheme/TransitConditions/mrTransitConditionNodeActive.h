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
#ifndef MR_TRANSIT_CONDITION_NODE_ACTIVE_H
#define MR_TRANSIT_CONDITION_NODE_ACTIVE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefNodeActive
/// \brief Transition condition definition for TransitConditionNodeActive.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefNodeActive : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
  static TransitCondition* instanceInit(
    TransitConditionDef*   tcDef,
    NMP::Memory::Resource& memRes,
    Network*               net,
    NodeID                 sourceNodeID);
  static bool instanceUpdate(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);

  NM_INLINE NodeID getNodeID() const { return m_nodeID; }
  NM_INLINE void setNodeID(NodeID nodeID) { m_nodeID = nodeID; }

protected:
  TransitConditionDefNodeActive() { m_type = TRANSCOND_NODE_ACTIVE_ID; };

  NodeID m_nodeID; ///< Node to check
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionNodeActive
/// \brief Condition satisfied when a specific node was active in the previous frame. Can't use the current
/// frame as there is no guarantee that has been updated yet.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionNodeActive : public TransitCondition
{
  friend class TransitConditionDefNodeActive;

protected:
  TransitConditionNodeActive();
  ~TransitConditionNodeActive();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_CONTROL_PARAM_FLOAT_GREATER_H
//----------------------------------------------------------------------------------------------------------------------
