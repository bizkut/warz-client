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
#ifndef MR_MESSAGE_H
#define MR_MESSAGE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrDefines.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::Message
/// \brief Stores data to be handled by nodes or triggers transitions.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
struct Message
{
public:

  void*         m_data;     ///< The data associated with this message, valid to be null.
  uint32_t      m_dataSize; ///< The size of the attached data in byte.
  MessageID     m_id;       ///< Previously known as RequestID.
  MessageType   m_type;     ///< Allows message handlers to determine the format of the message data.
  bool          m_status;   ///< Is the message enabled if used as a transit message condition.

  NM_INLINE Message();
  NM_INLINE Message(MessageID id, MessageType type, bool status, void* data, uint32_t dataSize);

  void locate();
  void dislocate();

  NM_INLINE void set(MessageID id, MessageType type, bool status, void* data, uint32_t dataSize);
};

//----------------------------------------------------------------------------------------------------------------------
/// Message functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Message::Message()
{
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Message::Message(MessageID id, MessageType type, bool status, void* data, uint32_t dataSize)
{
  set(id, type, status, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Message::set(MessageID id, MessageType type, bool status, void* data, uint32_t dataSize)
{
  m_id = id;
  m_type = type;
  m_status = status;
  m_data = data;
  m_dataSize = dataSize;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_MESSAGE_H
//----------------------------------------------------------------------------------------------------------------------
