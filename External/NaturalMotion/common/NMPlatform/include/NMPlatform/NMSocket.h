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
#ifndef NMP_SOCKET_H
#define NMP_SOCKET_H
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file NMSocket.h
/// \ingroup NaturalMotionPlatform
/// \brief Defines interface for management of a network communications socket.
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

/// \brief Initialize local network services.  This must be called before any sockets are created.
/// \return true if the network library is successfully initialized.
bool NetworkInitialize();

/// \brief Shutdown all network services. Ensure that all sockets are closed before calling this function.
void NetworkShutdown();

/// \brief Set socket to be blocking or non-blocking.
/// \return 0 on success.
int32_t NetworkSetNonBlocking(
  int32_t socket,     ///< Identifier of socket to set.
  bool    nonBlocking ///< If true, the socket will be switched to non-blocking mode.
);

/// \brief Returns a string describing the address of the primary adapter on the network.
/// \return A pointer to the address string, or 0 if the address could not be obtained.
const char* GetNetworkAddress();

/// \brief Returns the last error to occur from a socket call. This is not thread safe on all platforms.
/// \return error value.
int32_t GetNetworkError();

} // namespace NMP

// NOTE: If this include fails to find socket_imp.h, the platform-specific NMPlatform include is missing from your
//  include path.
#include "socket_imp.h"

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_SOCKET_H
//----------------------------------------------------------------------------------------------------------------------
