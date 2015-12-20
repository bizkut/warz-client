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
#ifndef NMP_SOCKET_WRAPPER_H
#define NMP_SOCKET_WRAPPER_H
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
class SocketWrapper
{
public:
  SocketWrapper();
  SocketWrapper(socket_t socket);

  /// \brief Gets the internal socket_t.
  /// \return the internal socket_t or -1 if the socket is invalid.
  socket_t getSocket();

  /// \brief Sets the internal socket_t and declares it valid.
  /// \param socket The socket_t to set.
  void setSocket(socket_t socket);

  /// \brief Declares this socket to be invalid.
  void setInvalid();

  /// \brief Queries whether this socket is valid.
  /// \return bool validity value.
  bool isValid();

protected:
  socket_t m_socket;
  bool     m_valid;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_SOCKET_WRAPPER_H
//----------------------------------------------------------------------------------------------------------------------
