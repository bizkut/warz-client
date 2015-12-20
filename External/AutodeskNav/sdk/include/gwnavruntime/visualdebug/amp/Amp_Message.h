/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   AMP
Filename    :   Amp_Message.h
Content     :   Messages sent back and forth to AMP
Created     :   December 2009
Authors     :   Alex Mantzaris

**************************************************************************/

// This file includes a number of different classes
// All of them derive from GFxMessage and represent the types of messages
// sent back and forth between AMP and the GFx application

#ifndef INC_KY_AMP_MESSAGE_H
#define INC_KY_AMP_MESSAGE_H

#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/kernel/SF_Array.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_Debug.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim {
    class File;
namespace Net {
namespace AMP {


class RawData
{
public:
    void       Read(Kaim::File& str);
    void       Write(Kaim::File& str) const;

    Kaim::ArrayLH_POD<Kaim::UByte, MemStat_VisualDebugMessage>  Data;
};

// Generic message that includes only the type
// Although not abstract, this class is intended as a base class for concrete messages
class Message : public RefCountBase<Message, MemStat_VisualDebugMessage>, public ListNode<Message>
{
public:
    static const UInt32 InvalidVersion = KY_MAX_UINT32;
    static const UInt32 InvalidMsgType = KY_MAX_UINT32;

    virtual ~Message() { }

    // Message serialization methods
    virtual void            Read(File& str);
    virtual void            Write(File& str) const;

    // Message printing for debugging
    virtual String          ToString() const
    {
        char str[32];
        Kaim::SFsprintf(str, 32, "%s-%u", GetMessageTypeName(), GetMessageType());
        return str; 
    }

    // Type comparison for queue filtering
    bool                    IsSameType(const Message& msg) const  { return MsgType == msg.MsgType; }
    UInt32                  GetMessageType() const { return MsgType; }
    void                    SetMessageType(UInt32 msgType) 
    {
        KY_DEBUG_ASSERTN(msgType!=Message::InvalidMsgType, ("setting an invalid message type, check the registry"));
        MsgType = msgType; 
    } //< should be set accordingly to a MessageTypeRegistry (done automatically by the AmpThreadMgr when sending and receiving)
    virtual const char*    GetMessageTypeName() const        { return "Message Base"; }

    // version for backwards-compatibility
    void        SetVersion(UInt32 version)                  { Version = version; } //< When sending a message, the version is already set.  This is useful when receiving it
    UInt32      GetVersion() const                          { return Version; }


    // Compression
    virtual Message*        Compress() const;
    virtual Message*        Uncompress()                    { return NULL; }

    // IP address information
    virtual void    SetPeerName(const char* name) { KY_UNUSED(name); }
    virtual void    SetPeerAddress(UInt32 address) { KY_UNUSED(address); }


    // Version for backwards compatibility
    static UInt32       GetLatestVersion()          { return Version_Latest; }

    // Helper read/write methods
    static void         ReadString(File& inFile, String* str);
    static void         WriteString(File& outFile, const String& str);

protected:

    UInt32         MsgType;  // for serialization
    UInt32         Version;

    // Protected constructor because Message is intended to be used as a base class only
    Message(): MsgType(Message::InvalidMsgType), Version(InvalidVersion) {}

    enum VersionType
    {
        Version_Latest = 0,
    };
};

// MessageSetup
// Both peers send this message on connection
class MessageSetup : public Message
{
public:
    static const char*    GetTypeName() { return "MessageSetup"; }
    static UInt32       GetLatestVersion()    { return Message::GetLatestVersion(); }

    MessageSetup();
    virtual ~MessageSetup() { }

    virtual const char*     GetMessageTypeName() const        { return MessageSetup::GetTypeName(); }
    
    // Message serialization methods
    virtual void            Read(File& str);
    virtual void            Write(File& str) const;

    // Message printing for debugging
    virtual String          ToString() const
    {
        String str = Kaim::Net::AMP::Message::ToString();
        char cstr[32];
        Kaim::SFsprintf(cstr, 32, "(heartbeatInterval:%u)", HeartbeatInterval);
        str += cstr;
        return str; 
    }

    virtual Message*        Compress() const                { return NULL; } //no need to compress such a message
    virtual Message*        Uncompress()                    { return NULL; }

    // HeartbeatInterval is compared to the local value in the ThreadMgr, and on reception the biggest value is kept, ensuring that the connection is maintained the same way on both ends.
    UInt32 HeartbeatInterval; 
};

// Heartbeat message
// Used for connection verification
// A lack of received messages for a long time period signifies lost connection
class MessageHeartbeat : public Message
{
public:
    static const char*    GetTypeName() { return "MessageHeartbeat"; }
    static UInt32         GetLatestVersion()    { return Message::GetLatestVersion(); }

    MessageHeartbeat();
    virtual ~MessageHeartbeat() { }

    virtual const char*     GetMessageTypeName() const        { return MessageHeartbeat::GetTypeName(); }
    
    virtual Message*        Compress() const                { return NULL; } //no need to compress such a message
    virtual Message*        Uncompress()                    { return NULL; }
};

// Used to signify that the connection is explicitly ended by the remote peer
class MessageDisconnection : public Message
{
public:
    static const char*    GetTypeName() { return "MessageDisconnection"; }
    static UInt32         GetLatestVersion()    { return Message::GetLatestVersion(); }

    MessageDisconnection();
    virtual ~MessageDisconnection() { }

    virtual const char*     GetMessageTypeName() const        { return MessageDisconnection::GetTypeName(); }

    virtual Message*        Compress() const                { return NULL; } //no need to compress such a message
    virtual Message*        Uncompress()                    { return NULL; }
};


// Log message
// Sent from server to client
class MessageLog : public Message
{
public:
    static const char*       GetTypeName() { return "MessageLog"; }
    static UInt32            GetLatestVersion() { return Message::GetLatestVersion(); }

    MessageLog(const String& logText = "", UInt32 logCategory = 0, const UInt64 timeStamp = 0);
    virtual ~MessageLog() { }
    void SetLog(const String& logText = "", UInt32 logCategory = 0, const UInt64 timeStamp = 0);

    // Message overrides
    virtual void            Read(File& str);
    virtual void            Write(File& str) const;
    virtual const char*     GetMessageTypeName() const        { return MessageLog::GetTypeName(); }

    // Accessors
    UInt32                  GetLogCategory() const;
    const StringLH&         GetTimeStamp() const;
    const StringLH&         GetLogText() const { return LogText; }


private:
    void SetTimeStamp(const UInt64 timeStamp);

    StringLH                LogText;
    UInt32                  LogCategory;
    StringLH                TimeStamp;
};


// Message containing the server listening port
// Broadcast via UDP
// The server IP address is known from the origin of the UDP packet
class MessagePort : public Message
{
public:
    static const char*        GetTypeName() { return "MessagePort"; }
    static UInt32             GetLatestVersion()    { return Message::GetLatestVersion(); }

    enum PlatformType
    {
        PlatformOther = 0,

        PlatformWindows,
        PlatformMac,
        PlatformLinux,

        PlatformXbox360,
        PlatformPs3,
        PlatformWii,
        Platform3DS,

        PlatformAndroid,
        PlatformIphone,

        PlatformNgp,
        PlatformWiiU
    };

    MessagePort();
    virtual ~MessagePort() { }

    // Message overrides
    virtual void            Read(File& str);
    virtual void            Write(File& str) const;
    virtual const char*     GetMessageTypeName() const        { return MessagePort::GetTypeName(); }

    // Accessors
    UInt32              GetPort() const;
    UInt32              GetPeerAddress() const;
    const StringLH&     GetPeerName() const;
    PlatformType        GetPlatform() const;
	const char*         GetPlatformName() const;
    const StringLH&     GetAppName() const;
    const StringLH&     GetFileName() const;
    virtual void        SetPeerName(const char* name);
    virtual void        SetPeerAddress(UInt32 address);
    void                SetPlatform(PlatformType platform);
    void                SetPort(UInt32 port);
    void                SetAppName(const char*  appName);
    void                SetFileName(const char* fileName);

protected:
    UInt32              Port;
    UInt32              Address;
    StringLH            PeerName;
    PlatformType        Platform;
    StringLH            AppName;
    StringLH            FileName;
};

// Compressed Message
// Can be any other message type once uncompressed
class MessageCompressed : public Message
{
public:
    static const char*  GetTypeName()           { return "MessageCompressed"; }
    static UInt32       GetLatestVersion()      { return Message::GetLatestVersion(); }
    MessageCompressed();
    ~MessageCompressed();

    // Message overrides
    virtual const char*     GetMessageTypeName() const        { return MessageCompressed::GetTypeName(); }

    virtual void            Read(File& str);
    virtual void            Write(File& str) const;

    virtual Message*        Uncompress();

    // Accessors
    void                    AddCompressedData(UByte* data, UPInt dataSize);
    const RawData&          GetCompressedData() const;

private:
    RawData          CompressedData;
};


} // namespace AMP
} // namespace Net
} // namespace Kaim

#endif

