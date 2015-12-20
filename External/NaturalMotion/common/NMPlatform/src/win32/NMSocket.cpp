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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"

#include <ws2tcpip.h>
#include <stdio.h>
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4996)
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
bool NetworkInitialize()
{
  WSADATA wsaData;
  int32_t ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

  return (ret == 0);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkShutdown()
{
  WSACleanup();
}

//----------------------------------------------------------------------------------------------------------------------
int32_t NetworkSetNonBlocking(int32_t socket, bool nonBlocking)
{
  int32_t nonBlockingInt = nonBlocking ? 1 : 0;
  return ioctl(socket, FIONBIO, &nonBlockingInt);
}

//----------------------------------------------------------------------------------------------------------------------
const char* GetNetworkAddress()
{
  static char addrBuffer[17];

  SOCKET sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd == SOCKET_ERROR)
  {
    return 0;
  }

  INTERFACE_INFO InterfaceList[20];
  unsigned long nBytesReturned;
  if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
               sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
  {
    return 0;
  }

  sockaddr_in* pAddress;
  pAddress = (sockaddr_in*) & (InterfaceList[0].iiAddress);

  unsigned char* addrBytes = (unsigned char*)&pAddress->sin_addr;
  NMP_SPRINTF(addrBuffer, 17, "%d.%d.%d.%d",
              addrBytes[0], addrBytes[1], addrBytes[2], addrBytes[3]);

  return addrBuffer;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
