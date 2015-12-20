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
// Included via NMRuntimeFramework/Socket.h.  Not to be included directly.

// System includes
#include <winsock2.h>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
//----------------------------------------------------------------------------------------------------------------------

typedef int           socklen_t;
typedef SOCKET        socket_t;
typedef ::fd_set      fd_set;
typedef ::sockaddr    sockaddr;
typedef ::sockaddr_in sockaddr_in;

static const socket_t SOCKET_ERROR_INVALID = INVALID_SOCKET;
static const int SOCKET_ERROR_GENERAL = SOCKET_ERROR;

//----------------------------------------------------------------------------------------------------------------------
inline socket_t socket(int domain, int type, int protocol)
{
  return ::socket(domain, type, protocol);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t send(socket_t s, const char *msg, uint32_t len, int flags)
{
  return ::send(s, msg, len, flags);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t recv(socket_t s, char *buf, uint32_t len, int flags)
{
  return ::recv(s, buf, len, flags);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t shutdown(socket_t s, int how)
{
  return ::shutdown(s, how);
}

//----------------------------------------------------------------------------------------------------------------------
inline socket_t accept(socket_t s, sockaddr *addr, socklen_t *addrlen)
{
  return ::accept(s, addr, addrlen);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t bind(socket_t s, sockaddr *addr, socklen_t addrlen)
{
  return ::bind(s, addr, addrlen);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t listen(socket_t s, int32_t backlog)
{
  return ::listen(s, backlog);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t setsockopt(
  socket_t    s,
  int32_t     level,
  int32_t     optname,
  const void* optval,
  socklen_t   optlen)
{
  return ::setsockopt((SOCKET)s, level, optname, (const char FAR*)optval, optlen);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t ioctl(
  socket_t s,
  int32_t  request,
  void*    argp)
{
  return ioctlsocket((SOCKET)s, (long)request, (u_long*)argp);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t getSocketsWithMessages(int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, uint32_t timeoutInUsec)
{
  timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = timeoutInUsec;

  return select(nfds, readfds, writefds, exceptfds, &timeout);
}

//----------------------------------------------------------------------------------------------------------------------
inline int closesocket(SOCKET s)
{
  return ::closesocket(s);
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t GetNetworkError()
{
  return WSAGetLastError();
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
