/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_VisualDebugClient_H
#define Navigation_VisualDebugClient_H

#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/kernel/SF_Threads.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/base/memory.h"

#if defined(KY_BUILD_SHIPPING) || !defined(KY_ENABLE_SOCKETS) || !defined(KY_ENABLE_THREADS)
# define KY_DISABLE_VISUALDEBUGCLIENT
#endif

#ifndef KY_DISABLE_VISUALDEBUGCLIENT


#include "gwnavruntime/visualdebug/internal/socketdispatcher.h"
#include "gwnavruntime/visualdebug/amp/Amp_ThreadMgr.h"
#include "gwnavruntime/visualdebug/visualdebugclientregistry.h"
#include "gwnavruntime/visualdebug/messages/messageblobhandler.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{


class FileOpenerBase;
class File;

class IQuery;
class MessageBlob;
class MessageAggregatedBlob;
class SocketDispatcherFactory;
class ClientStatusChangedCallback;
class AcknowledgeConnectionMessageReceiver;

class VisualDebugClientScopedFileWriter;


class MessagePortHandler;

/// The VisualDebugClient class manages the retrieval of data from a VisualDebugServer running in the game engine.
/// This class is used internally by the NavigationLab. It can be used 
/// to integrate the visualization of debug data into your level editing or debugging tools.
///  It has 4 exclusive modes:
///    - Listening for available servers (broadcast)
///    - Connected to a server
///    - Writing to file
///    - Reading from file
/// If KY_BUILD_SHIPPING is defined this class is empty.
class VisualDebugClient
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	/// Enumerated the running mode of the VisualDebugClient.
	enum RunMode
	{
		RunSynchronously = 0, ///< Operations are done in the main thread
		RunAsynchronously = 1 ///< Operations are made in separate threads
	};


	// ------------------ Main API ------------------

	VisualDebugClient();
	~VisualDebugClient();

	/// Defines a special value that can be passed to WaitWhileEstablishingConnection to block execution
	/// in the calling thread until the connection is established.
	static KyUInt32 GetInfiniteWaitDelay() { return KY_WAIT_INFINITE; }

	/// Setup the internal receiver registry
	KyResult SetupReceiverRegistry(MessageReceiverRegistry& receiverRegistry);


	// ------------------ Server Connection Mode ------------------

	/// Initiates a new connection to the VisualDebugServer listening on the specified host and port.
	/// \param ipAddress    The IP address or network name of the host running the VisualDebugServer.
	/// \param port         The network port configured for use by the VisualDebugServer.
	/// \param runMode      one theRunMode enumeration.
	/// \return A #KyResult that indicates the success or failure of the request. 
	KyResult ConnectToServer(const char* ipAddress, KyUInt32 port, RunMode runMode = RunSynchronously);

	/// Closes the current network socket connection, if any. 
	void CloseConnection();

	/// Indicates whether the VisualDebugClient is connected and SendEndOfSynchronization was called 
	bool IsConnected() const;

	/// Indicates whether ThreadMgr has established a connection. 
	bool IsConnectionEstablished() const;

	/// Check if everything was received. 
	bool IsReceptionPipelineEmpty()
	{ return (m_socketThreadMgr ? m_socketThreadMgr->IsReceptionPipelineEmpty() : true); }

	/// Receives all messages since the last call to Update(), and
	/// processes them by calling the receivers set up for the MessageReceiverRegistry.
	KyResult Update();


	template <class T>
	KyResult Send(BaseBlobBuilder<T>& blobBuilder)
	{
		if (m_socketThreadMgr)
		{
			blobBuilder.m_heap = GetHeap();
			Ptr<BaseBlobHandler> blobHandler = *KY_HEAP_NEW(blobBuilder.m_heap) BlobHandler<T>;
			blobBuilder.Build((BlobHandler<T>&)*blobHandler);
			return this->CreateMessageAndSend(blobHandler);
		}
		return KY_ERROR;
	}

	/// Must be send after having sent or received data for synchronization.
	/// Otherwise, the server will be in syncing state for ever, never being seen has fully connected
	KyResult SendEndOfSynchronization();

	KyResult Send(Kaim::BaseBlobHandler& blobHandler);

	/// Sends a flat blob (with no Array inside). The blob must be in the local platform endianness.
	/// (swap will be done internally when writing data to the socket).
	KyResult SendFlatBlob(KyUInt32 blobtypeId, KyUInt32 size, char* blob);

	// Used to send queries to be exectued remotely
	KyResult Send(IQuery* query);

	// Wait until connection is established, or until the specified time interval has passed.
	/// For internal use. 
	bool WaitWhileEstablishingConnection(KyUInt32 maxDelayMilliseconds);


	// ------------------ Broadcast Mode ------------------
	void StartListeningForAvailableServers(KyUInt32 port, RunMode runMode = RunAsynchronously); ///< RunAsynchronously is highly recommended to run this in a seperated thread because listening for Wii U servers is slow since it is not made via a real broadcasting
	void ListenForAvailableServers();
	void GetAvailableServers(KyArray<Ptr<Net::AMP::MessagePort> >& availableServers);
	void StopListeningForAvailableServers();

	// ------------------ File Modes ------------------
	KyResult ReadFromFile(const char* readFromFile, Kaim::FileOpenerBase* fileOpener, RunMode runMode = RunSynchronously);
	KyResult WriteToFile(const char* writeToFile, Kaim::FileOpenerBase* fileOpener, VisualDebugClientScopedFileWriter & writer, RunMode runMode = RunSynchronously);

private:
	void SetupMessageRegistryForServerPorts(Net::AMP::MessageTypeRegistry& customMsgTypeRegistry);
	void SetupMessageRegistryForBlobs(Net::AMP::MessageTypeRegistry& customMsgTypeRegistry);
	void CreateManager(SocketDispatchMode mode, Net::AMP::MessageTypeRegistry& customMsgTypeRegistry, bool initSocketConnection);
	void DestroyManager();

	KyResult OnConnection(); //< called from the socketThread when conneciton is established
	
	KyResult CreateMessageAndSend(Ptr<BaseBlobHandler> handlerPtr);
	KyResult CreateMessageAndSend(KyUInt32 blobtypeId, KyUInt32 shallowBlobSize, KyUInt32 deepBlobSize, char* blob);
	KyResult SendMessage(MessageBlob* msg);

	Ptr<Net::AMP::Message> CreateAndReadMessage(File& stream);

	void HandleAllReceivedMessages();
	void Handle(Ptr<Net::AMP::Message> msg);

public: //internal
	Net::AMP::ThreadMgr*       GetThreadMgr()       { return m_socketThreadMgr.GetPtr(); }
	const Net::AMP::ThreadMgr* GetThreadMgr() const { return m_socketThreadMgr.GetPtr(); }
	MemoryHeap* GetHeap() { return Memory::GetHeapByAddress(m_socketThreadMgr.GetPtr()); }
	
	void WaitForEmptiedSendQueue();

	RunMode GetRunMode() const { return m_runMode; }

	File* GetFile() { return m_file; }

private:
	static const KyUInt32 NotConnected = 0;
	Kaim::AtomicInt<KyUInt32> m_wasConnected; //used to check the connection.
	bool m_isSynchronized;

	KyArray<char> m_receiveBuffer;

	Kaim::Ptr<Net::AMP::ThreadMgr> m_socketThreadMgr;
	Kaim::Ptr<Net::AMP::DiscardMessageHandler> m_msgDiscarder;
	Kaim::Ptr<MessagePortHandler> m_receivedMsgPort;	
	Kaim::Ptr<MessageBlobHandler> m_msgBlobHandler;
	Kaim::Ptr<MessageAggregatedBlobHandler> m_msgAggregatedBlobHandler;
	Kaim::SocketDispatcherFactory* m_socketFactory;
	Kaim::Ptr<File> m_file;

	// Callback for connection status change
	friend class ClientStatusChangedCallback;
	Kaim::Ptr<ClientStatusChangedCallback> m_statusCallback;
	Kaim::Event m_connectedEvent;

	RunMode m_runMode;
};



class VisualDebugClientScopedFileWriter
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	VisualDebugClientScopedFileWriter();
	~VisualDebugClientScopedFileWriter();

	KyResult Write(char* blob, KyUInt32 blobTypeId, KyUInt32 deepBlobSize);
	KyResult WriteFrame(char* blob, KyUInt32 blobTypeId, KyUInt32 deepBlobSize); // call WriteFrame to end the frame previous Write belonged to

private:
	KyResult CreateAggregateMessage(KyUInt32 deepBlobSize);
	KyResult WriteMessageToFile(Net::AMP::Message*& message);
	KyResult WriteToFile();

public: //internal
	void Init(VisualDebugClient* visualDebugClient, VisualDebugClient::RunMode runMode);

private:
	VisualDebugClient* m_visualDebugClient;
	VisualDebugClient::RunMode m_runMode;
	MessageAggregatedBlob* m_aggregatedBlobMsg;
};



} // namespace Kaim

#else

// Only keep symbols that could be used as parameters.
// VisualDebugClient is not expected to be used in Shipping.
namespace Kaim
{

class VisualDebugClient
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	enum RunMode
	{
		RunSynchronously = 0,
		RunAsynchronously = 1
	};

	static KyUInt32 GetInfiniteWaitDelay() { return KY_WAIT_INFINITE; }
};

}

#endif

#endif // Navigation_VisualDebugClient_H
