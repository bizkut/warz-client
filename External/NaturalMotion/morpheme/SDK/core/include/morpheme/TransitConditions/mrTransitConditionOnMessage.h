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
#ifndef MR_TRANSIT_CONDITION_ON_MESSAGE_H
#define MR_TRANSIT_CONDITION_ON_MESSAGE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefOnMessage
/// \brief Transition condition definition for TransitConditionDefOnMessage.
/// \ingroup CoreTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefOnMessage : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
  static TransitCondition* instanceInit(
    TransitConditionDef*   tcDef,
    NMP::Memory::Resource& memRes,
    Network*               net,
    NodeID                 sourceNodeID);

  static void instanceReset(TransitCondition* tc, TransitConditionDef* tcDef);

  static void instanceSetState(TransitCondition* tc, TransitConditionDef* tcDef, bool state);

  NM_INLINE MessageID getMessageID() const { return m_messageID; }
  NM_INLINE void setMessageID(MessageID messageID) { m_messageID = messageID; }

protected:
  TransitConditionDefOnMessage() { m_type = TRANSCOND_ON_MESSAGE_ID; };

  MessageID m_messageID; ///<
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionOnRequest
/// \brief Condition set or un-set directly by the user of the Network.
/// \ingroup CoreTransitConditionsModule
/// \see MR:: TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionOnRequest : public TransitCondition
{
  friend class TransitConditionDefOnMessage;

protected:
  TransitConditionOnRequest();
  ~TransitConditionOnRequest();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_ON_MESSAGE_H
//----------------------------------------------------------------------------------------------------------------------
