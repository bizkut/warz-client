/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef INC_KY_AMP_MESSAGEREGISTRY_H
#define INC_KY_AMP_MESSAGEREGISTRY_H

#include "gwnavruntime/visualdebug/amp/Amp_Interfaces.h"
#include "gwnavruntime/visualdebug/amp/Amp_Message.h"
#include "gwnavruntime/kernel/SF_Debug.h"
#include "gwnavruntime/kernel/SF_HeapNew.h"
#include "gwnavruntime/kernel/SF_StringHash.h"
#include "gwnavruntime/kernel/SF_Atomic.h"

namespace Kaim
{
namespace Net
{
namespace AMP
{

class ThreadMgr;

// Only a base class, it is used directly by the registry.
class BaseMessageTypeDescriptor : public NewOverrideBase<Stat_Default_Mem>
{
public:
    virtual ~BaseMessageTypeDescriptor() { }

    const String& GetMessageTypeName() const  { return MessageTypeName; }
    UInt32 GetMessageLatestVersion() const  { return MessageLatestVersion; }
    UInt32 GetMessageType() const  { return MessageType; }
    UInt32 GetPeerMessageType() const  { return PeerMessageType; }
    bool CanHandle(IMessageHandler::Mode handlerMode) const { return MessageHandler!=NULL && MessageHandler->HandleMode() == handlerMode; }
    void Handle(Message* msg) const  { MessageHandler->Handle(msg); }

    virtual BaseMessageTypeDescriptor* Clone() const = 0;
    virtual Message* CreateMessage(MemoryHeap* heap) const = 0;

protected:
    BaseMessageTypeDescriptor(const char* messageTypeName, UInt32 messageLatestVersion, UInt32 messageType, Ptr<IMessageHandler> messageHandler)
        : MessageHandler(messageHandler)
        , MessageTypeName(messageTypeName)
        , MessageLatestVersion(messageLatestVersion)
        , MessageType(messageType)
        , PeerMessageType(messageType)
    {
    }

    friend class MessageTypeRegistry;

    Ptr<IMessageHandler> MessageHandler;
    String MessageTypeName;
    UInt32 MessageLatestVersion;
    UInt32 MessageType;
    UInt32 PeerMessageType;
};

// MessageTypeDescriptor is used directly by the registry.
// TMessage is expected to inherit from Message class
template <class TMessage>
class MessageTypeDescriptor : public BaseMessageTypeDescriptor
{
private:
    friend class MessageTypeRegistry;

    MessageTypeDescriptor(const char* messageTypeName, UInt32 messageLatestVersion, UInt32 messageType, Ptr<IMessageHandler> messageHandler)
        :BaseMessageTypeDescriptor(messageTypeName, messageLatestVersion, messageType, messageHandler)
    {
    }

    virtual MessageTypeDescriptor<TMessage>* Clone() const { return KY_NEW(MessageTypeDescriptor<TMessage>)(MessageTypeName, MessageLatestVersion, MessageType, MessageHandler); }
    virtual TMessage* CreateMessage(MemoryHeap* heap) const
    {
        TMessage* msg = KY_HEAP_NEW(heap) TMessage();
        msg->SetMessageType(MessageType);
        return msg;
    }
};

// This is class is used to register different kind of messages and is in charge to assign them an id/index
// It can also be used to translate/convert message type
class MessageTypeRegistry
{
public:

    void Clear();

    MessageTypeRegistry();

    ~MessageTypeRegistry()
    {
        for (UInt32 i = 0; i < Descriptors.GetSize(); ++i)
            delete Descriptors[i];
        Descriptors.Clear();
    }


    // If handler is NULL it means that the message is not treated
    // TMessage is expected to inherit from Message class
    template<class TMessage>
    void AddMessageType(Ptr<IMessageHandler> handler)
    {
        // Call GetMessageTypeDescriptor_Unique in order to prevent conflicting hashId
        BaseMessageTypeDescriptor* myDescriptor = GetMessageTypeDescriptor_Unique(TMessage::GetTypeName());
        if (myDescriptor == NULL)
        {
            UInt32 localMsgType = static_cast<Kaim::UInt32>(Descriptors.GetSize());
            MessageTypeDescriptor<TMessage>* descriptor = KY_NEW MessageTypeDescriptor<TMessage>(TMessage::GetTypeName(), TMessage::GetLatestVersion() , localMsgType, handler);
            StringToMsgType.SetCaseInsensitive(TMessage::GetTypeName(), localMsgType);
            Descriptors.PushBack(descriptor);
        }
        else
        {
            if (myDescriptor->GetMessageTypeName() != TMessage::GetTypeName())
            {
                KY_DEBUG_ERRORN(("MessageType '%s-%u' ignored, it conflicts with descriptor %u, %s-%u",
                    TMessage::GetTypeName(), TMessage::GetLatestVersion(),
                    static_cast<unsigned int>(myDescriptor->GetMessageType()), 
                    myDescriptor->GetMessageTypeName().ToCStr(), static_cast<unsigned int>(myDescriptor->GetMessageLatestVersion())));
                return;
            }

            KY_DEBUG_WARNINGN_IF(myDescriptor->MessageHandler != NULL, ("Handler of descriptor %u, %s-%u is overridden", 
				static_cast<unsigned int>(myDescriptor->GetMessageType()), 
				myDescriptor->GetMessageTypeName().ToCStr(), static_cast<unsigned int>(myDescriptor->GetMessageLatestVersion())));
            myDescriptor->MessageHandler = handler;
        }
    }

    void AddMessageTypeRegistry(const MessageTypeRegistry& other);

    BaseMessageTypeDescriptor* GetMessageTypeDescriptor(const String& messageTypeName) const
    {
        BaseMessageTypeDescriptor* descriptor = GetMessageTypeDescriptor_Unique(messageTypeName);
        if (descriptor->GetMessageTypeName() == messageTypeName)
            return descriptor;
        return NULL;
    }

    // TMessage is expected to inherit from Message class
    template<class TMessage>
    UInt32 GetMessageType() const
    {
        return GetMessageType(TMessage::GetTypeName());
    }

    UInt32 GetMessageType(const String& messageTypeName) const
    {
        const BaseMessageTypeDescriptor* descriptor = GetMessageTypeDescriptor(messageTypeName);
        return (descriptor ? descriptor->GetMessageType() : Message::InvalidMsgType);
    }

    UInt32 GetMessageTypeCount() const
    {
        return (UInt32)Descriptors.GetSize();
    }

    BaseMessageTypeDescriptor* GetMessageTypeDescriptor(UInt32 localMsgType) const
    {
        if (localMsgType < Descriptors.GetSize())
            return Descriptors[localMsgType];

        return NULL;
    }

    UInt32 GetLocalMessageType(UInt32 peerMsgType) const
    {
        if (MappingEnabled)
        {
            if (peerMsgType < MsgTypeConverter.GetSize())
            {
                return MsgTypeConverter[peerMsgType];
            }
            return Message::InvalidMsgType; //in that case there is a problem, the type are not properly mapped
        }
        
        return peerMsgType; // use the same
    }

    UInt32 GetPeerMessageType(UInt32 localMsgType) const
    {
        if (MappingEnabled)
        {
            if (localMsgType < Descriptors.GetSize())
            {
                return Descriptors[localMsgType]->GetPeerMessageType();
            }

            return Message::InvalidMsgType;
        }

        return localMsgType;
    }

    struct RegistryEntry
    {
        String MsgTypeName;
        UInt32 Version;
    };
    void MapMsgTypes(RegistryEntry* peerRegistryData, UInt32 dataSize);
    void DisableMapping() { MappingEnabled = false; }
    bool IsMappingEnabled() const { return MappingEnabled!=0; }

    // TMessage is expected to inherit from Message class
    template<class TMessage>
    TMessage* CreateMessage( void* memoryHeap, bool doMapType) const
    {
        return static_cast<TMessage*>(CreateMessage(memoryHeap, doMapType, TMessage::GetTypeName()));
    }

    Message* CreateMessage( void* memoryHeap, bool doMapType, const String& messageTypeName) const;

    Message* CreateMessageRegistry(void* memoryHeap, bool doMapType) const;

private:
    // Doesn't check the real name
    BaseMessageTypeDescriptor* GetMessageTypeDescriptor_Unique(const String& messageTypeName) const
    {
        const UInt32* pMsgType = StringToMsgType.GetCaseInsensitive(messageTypeName);
        return (pMsgType && (*pMsgType < Descriptors.GetSize()) ? Descriptors[*pMsgType] : NULL);
    }

protected:
    // entry count must be the same between Descriptors and StringToMsgType
    Array<BaseMessageTypeDescriptor*> Descriptors;
    StringHash<UInt32> StringToMsgType; //associate a MsgTypeName to an entry in Descriptors

    Array<UInt32> MsgTypeConverter; //associate peerMsgType to an entry in Descriptors (aka localMsgType)
    UInt32 MappingEnabled;
};


}
}
}
#endif

