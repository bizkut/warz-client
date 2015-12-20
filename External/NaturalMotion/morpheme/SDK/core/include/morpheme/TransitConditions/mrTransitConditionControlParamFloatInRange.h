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
#ifndef MR_TRANSIT_CONDITION_CONTROL_PARAM_FLOAT_IN_RANGE_H
#define MR_TRANSIT_CONDITION_CONTROL_PARAM_FLOAT_IN_RANGE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefControlParamFloatInRange
/// \brief Transition condition definition for TransitConditionControlParamFloatInRange.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefControlParamFloatInRange : public TransitConditionDef
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
  NM_INLINE float getLowerTestValue() const { return m_lowerTestValue; }
  NM_INLINE void setLowerTestValue(float lowerTestValue) { m_lowerTestValue = lowerTestValue; }
  NM_INLINE float getUpperTestValue() const { return m_upperTestValue; }
  NM_INLINE void setUpperTestValue(float upperTestValue) { m_upperTestValue = upperTestValue; }

protected:
  TransitConditionDefControlParamFloatInRange() { m_type = TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID; };

  CPConnection  m_cpConnection;     ///< Connection to the Control Parameter attribute data to check against.
  float         m_lowerTestValue;   ///< Value to test control parameter is greater than or equal to.
  float         m_upperTestValue;   ///< Value to test control parameter is less than or equal to.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionControlParamFloatInRange
/// \brief Condition satisfied when Float Control Parameter value is in range than our test value.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionControlParamFloatInRange : public TransitCondition
{
  friend class TransitConditionDefControlParamFloatInRange;

protected:
  TransitConditionControlParamFloatInRange();
  ~TransitConditionControlParamFloatInRange();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_CONTROL_PARAM_FLOAT_IN_RANGE_H
//----------------------------------------------------------------------------------------------------------------------
