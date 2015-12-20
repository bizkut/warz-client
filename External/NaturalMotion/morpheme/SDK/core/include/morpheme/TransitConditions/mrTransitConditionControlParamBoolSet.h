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
#ifndef MR_TRANSIT_CONDITION_CONTROL_PARAM_BOOL_SET_H
#define MR_TRANSIT_CONDITION_CONTROL_PARAM_BOOL_SET_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionControlParamBoolSet
/// \brief Transition condition definition for TransitConditionControlParamBoolSet.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefControlParamBoolSet : public TransitConditionDef
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

  NM_INLINE const CPConnection* getCPConnection() const { return &m_cpConnection; }
  NM_INLINE void setCPConnection(NodeID sourceNodeID, PinIndex sourcePinIndex) { m_cpConnection.set(sourceNodeID, sourcePinIndex);};
  NM_INLINE bool getTestValue() const { return m_value; }
  NM_INLINE void setTestValue(bool testValue) { m_value = testValue; }
  
protected:
  TransitConditionDefControlParamBoolSet() { m_type = TRANSCOND_CONTROL_PARAM_BOOL_SET_ID; };

  CPConnection  m_cpConnection; ///< Connection to the input Control Parameter attribute data to check against.
  bool          m_value;        ///< Condition will be satisfied if values are equal.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionControlParamBoolSet
/// \brief Condition satisfied when Int Control Parameter value is greater than our test value.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionControlParamBoolSet : public TransitCondition
{
  friend class TransitConditionDefControlParamBoolSet;

protected:
  TransitConditionControlParamBoolSet();
  ~TransitConditionControlParamBoolSet();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_CONTROL_PARAM_BOOL_SET_H
//----------------------------------------------------------------------------------------------------------------------
