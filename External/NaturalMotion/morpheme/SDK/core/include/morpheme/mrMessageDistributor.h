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
#ifndef MR_MESSAGE_DISTRIBUTOR_H
#define MR_MESSAGE_DISTRIBUTOR_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMStringTable.h"

#include "morpheme/mrDefines.h"
#include "morpheme/mrMessage.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::MessageDistributor
/// \brief A message distributor stores all of the nodeIDs that registered an interest in a message.  When a message is
/// broad casted to the network then all nodes in the distributor with that messageID will receive the message.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
class MessageDistributor
{
public:
  MessageID   m_messageID;
  MessageType m_messageType;
  uint32_t    m_numNodeIDs;
  NodeID*     m_nodeIDs;

  uint32_t          m_numMessagePresets;
  Message**         m_messagePresets;
  NMP::OrderedStringTable* m_messagePresetIndexNamesTable;

  uint32_t getNumMessagePresets() const;
  const Message* getMessagePreset(uint32_t index) const;

  void locate();
  void dislocate();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_MESSAGE_H
//----------------------------------------------------------------------------------------------------------------------
