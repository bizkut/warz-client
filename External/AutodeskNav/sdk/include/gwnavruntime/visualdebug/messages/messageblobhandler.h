/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_MessageBlobHandler_H
#define Navigation_MessageBlobHandler_H

#include "gwnavruntime/visualdebug/visualdebugclientregistry.h"
#include "gwnavruntime/visualdebug/amp/Amp_Message.h"
#include "gwnavruntime/visualdebug/amp/Amp_Interfaces.h"

namespace Kaim
{

class MessageBlobHandler : public Net::AMP::IMessageHandler
{
public:
	MessageBlobHandler() : m_logReceivedUnknownBlobType(true) {}
	virtual ~MessageBlobHandler() {}
	virtual void Handle(Net::AMP::Message* message);
	virtual Mode HandleMode() { return Handle_OnUserDemand; }

	template<class T>
	void SetReceiverVal(IMessageReceiver* receiver) { m_messageReceiverRegistry.SetVal<T>(receiver); }

	void SetReceiverVal(KyUInt32 blobTypeId, IMessageReceiver* receiver) { m_messageReceiverRegistry.SetVal(blobTypeId, receiver); }

	template<class T>
	void SetReceiverRef(IMessageReceiver* receiver) { m_messageReceiverRegistry.SetRef<T>(receiver); }

	void SetReceiverRef(KyUInt32 blobTypeId, IMessageReceiver* receiver) { m_messageReceiverRegistry.SetRef(blobTypeId, receiver); }

	KyResult TransferContent(MessageReceiverRegistry& fromRegistry) { return m_messageReceiverRegistry.TransferContent(fromRegistry); }

	void Clear() { m_messageReceiverRegistry.Clear(); }

public: // internal
	void Receive(Net::AMP::Message* message, KyUInt32 blobTypeId, const char* msgBuffer, KyUInt32 size);

public: // internal
	MessageReceiverRegistry m_messageReceiverRegistry;
	bool m_logReceivedUnknownBlobType;
};


class MessageAggregatedBlobHandler : public Net::AMP::IMessageHandler
{
public:
	MessageAggregatedBlobHandler(Ptr<MessageBlobHandler> msgBlobHandler)
		: m_msgBlobHandler(msgBlobHandler)
	{}

	virtual ~MessageAggregatedBlobHandler() {}
	virtual void Handle(Net::AMP::Message* message);
	virtual Mode HandleMode() { return Handle_OnUserDemand; }

protected:
	Ptr<MessageBlobHandler> m_msgBlobHandler;
};


}

#endif
