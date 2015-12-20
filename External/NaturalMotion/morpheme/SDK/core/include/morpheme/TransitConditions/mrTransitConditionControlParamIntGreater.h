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
#ifndef MR_TRANSIT_CONDITION_CONTROL_PARAM_INT_GREATER_H
#define MR_TRANSIT_CONDITION_CONTROL_PARAM_INT_GREATER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionControlParamIntGreater
/// \brief Transition condition definition for TransitConditionControlParamIntGreater.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefControlParamIntGreater : public TransitConditionDef
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
  NM_INLINE int32_t getTestValue() const { return m_testValue; }
  NM_INLINE void setTestValue(int32_t testValue) { m_testValue = testValue; }
  NM_INLINE bool getOrEqual() const { return m_orEqual; }
  NM_INLINE void setOrEqual(bool orEqual) { m_orEqual = orEqual; }

protected:
  TransitConditionDefControlParamIntGreater() { m_type = TRANSCOND_CONTROL_PARAM_INT_GREATER_ID; };

  CPConnection  m_cpConnection; ///< Connection to the input Control Parameter attribute data to check against.
  int32_t       m_testValue;    ///< Value to test control parameter against.
  bool          m_orEqual;      ///< Condition will be satisfied if values are equal.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionControlParamIntGreater
/// \brief Condition satisfied when Int Control Parameter value is greater than our test value.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionControlParamIntGreater : public TransitCondition
{
  friend class TransitConditionDefControlParamIntGreater;

protected:
  TransitConditionControlParamIntGreater();
  ~TransitConditionControlParamIntGreater();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_CONTROL_PARAM_INT_GREATER_H
//----------------------------------------------------------------------------------------------------------------------
