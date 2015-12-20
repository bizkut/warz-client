/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   AMP
Filename    :   Amp_Interfaces.h
Content     :   Interfaces for customizing the behavior of AMP
Created     :   December 2009
Authors     :   Alex Mantzaris

**************************************************************************/

#ifndef INC_KY_AMP_INTERFACES_H
#define INC_KY_AMP_INTERFACES_H

#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/visualdebug/amp/Amp_Stats.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim {
namespace Net {
namespace AMP {

class Message;

// 
// Custom AMP behavior is achieved by overriding the classes in this file
//

//Interface used to handle received messages that are dispatched by the MessageTypeRegistry
class IMessageHandler : public RefCountBase<IMessageHandler, MemStat_VisualDebugMessage>
{
public:
    virtual ~IMessageHandler() {};
    virtual void Handle(Message* message) = 0;
    
    enum Mode
    {
        Handle_OnUserDemand = 0,       // specify that this handler will be manually called by the user after calling ThreadMgr::GetNextReceivedMessage()
        Handle_AfterDecompression = 1, // specify that this handler will be automatically called by the ThreadMgr in the CompressLoop after decompressing the message
        Handle_ImmediatelyWhenRead = 2,        // not recommended, used internally for specific messages, specify that this handler will be automatically called by the ThreadMgr as soon as the message is created and read from the socket
    };

    virtual Mode HandleMode() = 0;

};


// SendInterface::OnSendLoop is called once per "frame" 
// from the ThreadManager send thread
class SendInterface
{
public:
    virtual ~SendInterface() { }
    virtual bool OnSendLoop() = 0;
};

// ConnStatusInterface::OnStatusChanged is called by ThreadManager 
// whenever a change in the connection status has been detected
class ConnStatusInterface
{
public:
    enum StatusType
    {
        CS_Idle         = 0x0,
        CS_Connecting   = 0x1,
        CS_OK           = 0x2,
        CS_Failed       = 0x3,
    };

    virtual ~ConnStatusInterface() { }
    virtual void OnStatusChanged(StatusType newStatus, StatusType oldStatus, const char* message) = 0;
    virtual void OnMsgVersionMismatch(int peerVersion, int localVersion, const char* message) = 0;
};

} // namespace AMP
} // namespace Net
} // namespace Kaim

#endif
