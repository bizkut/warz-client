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
#include "NMPlatform/NMSocketWrapper.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
SocketWrapper::SocketWrapper() :
  m_valid(false)
{
  // Nothing needed here
}

//----------------------------------------------------------------------------------------------------------------------
SocketWrapper::SocketWrapper(socket_t socket)
{
  m_socket = socket;
  m_valid = true;
}

//----------------------------------------------------------------------------------------------------------------------
socket_t SocketWrapper::getSocket()
{
  if (m_valid == false)
  {
    NMP_ASSERT_FAIL(); // attempted to get invalid socket -- check for validity first
    return (socket_t) - 1;
  }
  return m_socket;
}

//----------------------------------------------------------------------------------------------------------------------
void SocketWrapper::setSocket(
  socket_t socket)
{
  m_socket = socket;
  m_valid = true;
}

//----------------------------------------------------------------------------------------------------------------------
void SocketWrapper::setInvalid()
{
  m_valid = false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SocketWrapper::isValid()
{
  return m_valid;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
