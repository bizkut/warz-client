/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

Filename    :   Amp_Socket.h
Content     :   Socket wrapper class

Created     :   July 03, 2009
Authors     :   Boris Rayskiy, Alex Mantzaris

**************************************************************************/

#ifndef INC_KY_AMP_SOCKET_H
#define INC_KY_AMP_SOCKET_H

#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/kernel/SF_Memory.h"
#include "gwnavruntime/kernel/SF_HeapNew.h"

#ifdef KY_ENABLE_SOCKETS

namespace Kaim {
class String;
class Lock;
namespace Net {
namespace AMP {

class SocketInterface
{
public:
    virtual ~SocketInterface() { }
    virtual bool CreateStream(bool listener) = 0;
    virtual bool CreateDatagram(bool broadcast) = 0;
    virtual bool Bind() = 0;
    virtual bool Listen(int) = 0;
    virtual bool Connect() = 0;
    virtual bool Accept(int timeoutMs) = 0;
    virtual int Send(const char*, UPInt) const = 0;
    virtual int Receive(char*, int) const = 0;
    virtual int SendBroadcast(const char*, UPInt) const = 0;
    virtual int ReceiveBroadcast(char*, int) const = 0;
    virtual void SetListenPort(UInt32) = 0;
    virtual void SetBroadcastPort(UInt32) = 0;
    virtual void SetAddress(UInt32, const char*) = 0;
    virtual void SetBlocking(bool) = 0;
    virtual void SetBroadcast(bool) = 0;
    virtual void GetName(UInt32*, UInt32*, char*, UInt32) = 0;
    virtual bool Shutdown(int timeoutMsForOrderlyRelease) = 0;
    virtual bool Startup() = 0;
    virtual void Cleanup() = 0;
    virtual int GetLastError() const = 0;
    virtual bool IsValid() const = 0;
    virtual bool IsListening() const = 0;
    virtual bool ShutdownListener(int timeoutMsForOrderlyRelease) = 0;
    virtual bool CheckAbort() const = 0;
};

class SocketImplFactory
{
public:
    virtual ~SocketImplFactory() { }
    virtual SocketInterface* Create() = 0;
    virtual void Destroy(SocketInterface* socketImpl) = 0;
};

template<class C>
class DefaultSocketFactory : public NewOverrideBase<Stat_Default_Mem>, public SocketImplFactory
{
public:
    virtual ~DefaultSocketFactory() { }
    virtual SocketInterface* Create()
    {
        return KY_NEW C();
    }
    virtual void Destroy(SocketInterface* socketImpl)
    {
        delete socketImpl;
    }
};

enum LogSilentMode
{
    LOG_SILENT,
    LOG_VERBOSE
};

class Socket
{
public:
    Socket(bool initLib, SocketImplFactory* socketImplFactory);
    ~Socket();

    // Create a socket, either to a specific address and port (client), or a listener (server)
    bool    CreateClient(const char* ipAddress, UInt32 port, String* errorMsg);
    bool    CreateServer(UInt32 port, String* errorMsg);
    // Shuts down and performs cleanup, timeoutMsForOrderlyRelease == 0 means no orderly release
    void    Destroy(int timeoutMsForOrderlyRelease = 0);
    // Wait until an incoming connection is accepted (only relevant for server sockets)
    bool    Accept(int timeoutMs);
    // Send a packet over the network
    int     Send(const char* dataBuffer, UPInt dataBufferSize, LogSilentMode silentMode = LOG_VERBOSE) const;
    // Receive a packet over the network (blocks)
    int     Receive(char* dataBuffer, int dataSize, LogSilentMode silentMode = LOG_VERBOSE) const;
    // Test the outgoing connection
    bool    IsConnected() const;
    // Set blocking
    void    SetBlocking(bool blocking);
    // Set a lock for multithreaded access
    void    SetLock(Lock* lock);
    // Is the socket valid?
    bool IsValid() const;
    // Check whether a reconnection should happen
    bool    CheckAbort() const;

private:
    enum  { SocketListenBacklog = 10 };  // Number of simultaneous connections (server only)

    SocketImplFactory*  SocketFactory;
    SocketInterface*    SocketImpl;
    bool                IsServer;
    bool                InitLib;

#ifdef KY_ENABLE_THREADS
    Lock*               CreateLock;
#endif

    // Private helper functions
    bool    Shutdown(int timeoutMsForOrderlyRelease);
    bool    Startup();
    void    Cleanup();
};

class BroadcastSocket
{
public:
    BroadcastSocket(bool initLib, SocketImplFactory* socketImplFactory);
    ~BroadcastSocket();
    bool    Create(UInt32 port, bool broadcast);
    void    Destroy();
    int     Broadcast(const char* dataBuffer, UPInt dataBufferSize) const;
    int     Receive(char* dataBuffer, int dataSize) const;
    void    GetName(UInt32* port, UInt32* address, char* name, UInt32 nameSize) const;
    int     GetLastError() const { return SocketImpl->GetLastError(); }

private:
    SocketImplFactory*  SocketFactory;
    SocketInterface*    SocketImpl;
    bool                InitLib;
};


#ifndef KY_AMP_CHANNEL_NAME
#define KY_AMP_CHANNEL_NAME "GwNavAmp"
#endif

} // namespace AMP
} // namespace Net
} // namespace Kaim

#endif // KY_ENABLE_SOCKETS

#endif // INC_KY_AMP_SOCKET_H