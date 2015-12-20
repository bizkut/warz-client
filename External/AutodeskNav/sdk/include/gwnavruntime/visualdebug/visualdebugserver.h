/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef GwNavRuntime_VisualDebugServer_H
#define GwNavRuntime_VisualDebugServer_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/kernel/SF_Threads.h"
#include "gwnavruntime/visualdebug/visualdebugclientregistry.h"
#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/visualdebug/visualdebugattributes.h"
#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"


namespace Kaim
{

class FileOpenerBase;
class File;

class ServerStatusChangedCallback;
class MessageBlob;
class MessageAggregatedBlob;
class MessageBlobHandler;
class BlobFieldsStore;

class Profiler;
class FloatStat;
class IQuery;

class Vec3f;

class VisualDebugAttributesManager;

/// Configure the VisualDebugServer.
/// Call UseNavigationLab() or UseLocalFile() to determine how to run the VisualDebugServer.
/// Note that Mode_UseLocalFile mode logs _all_ sent data in to a file, it is mainly intended for testing.
class VisualDebugServerConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
public:
	static KyUInt32 DefaultServerPort() { return 4888; }
	static KyUInt32 DefaultBroadcastPort() { return 4887; }

public:
	/// Enumerates the visual debug information destination.
	enum Mode {
		Mode_UseNavigationLab, ///< Sends visual debug information to the NavigationLab over the Network
		Mode_UseLocalFile      ///< Writes _all_ visual debug information to a file on the platform
	};

	/// Define the behavior for VisualDebugServer::Start.
	enum WaitOnStart {
		DoNotWaitOnStart, ///< Start won't wait for a connection
		DoWaitOnStart     ///< Start will block until a connection occurs
	};

	/// Define how to detect disconnections.
	enum DisconnectionMode {
		UseSocketErrors, ///< use socket errors to detect disconnections (recommended)
		UseHeartbeat     ///< use heartbeat messages to detect disconnections (disconnection can occur when breakpointing)
	};

	/// Determine how the data are sent.
	enum RunMode {
		RunSynchronously, ///< run in current thread (recommended for most platforms)
		RunAsynchronously ///< use secondary threads for sending blobs
	};

public:
	VisualDebugServerConfig()
		: m_mode(Mode_UseNavigationLab)
		, m_serverPort(0)
		, m_broadcastPort(0)
		, m_waitOnStart(DoNotWaitOnStart)
		, m_navigationLabCommandHandlers(KY_NULL)
		, m_localFileName(KY_NULL)
		, m_localFileOpener(KY_NULL)
		, m_localFileMegaBytesLimit(100)
		, m_maximumMegaBytes(1)
		, m_expectedKiloBytesPerFrame(256)
		, m_disconnectionMode(UseSocketErrors)
#if defined(KY_OS_WIIU)
		// On Wii U, by default VisualDebugServer is run in another thread with affinity set to OS_THREAD_ATTR_AFFINITY_CORE2
		// https://www.warioworld.com/wiiu/downloads/getfile.asp?file=NetworkOptimizationOnTheWiiU.zip&size=298882
		, m_runMode(RunAsynchronously)
#else
		, m_runMode(RunSynchronously)
#endif
	{}

	/// Reduce chances of selecting conflicting ports by looking at :
	///  - https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.txt
	///  - https://ps4.scedev.net/docs/ps4-en,Net-Overview-orbis,Notes/1/
	/// \param serverPort Indicates the port to use for connecting to the server
	/// \param broadcastPort Indicates the port to use for broadcasting the availability of this server
	///        Passing 0 disables broadcasting.
	///        Broadcast port the NavigationLab listen to can be changed in "Window/Preferences/Visual debug" menu.
	/// \param navigationLabCommandHandlers Allows advanced use of the server by specifying handlers. 
	///        If NULL or empty, received messages are discarded when received. 
	///        Otherwise, the user has to call HandleNextReceivedMessage() or GetNextReceivedMessage(), if not received messages won't be freed over frame.
	void UseNavigationLab(KyUInt32 serverPort = DefaultServerPort(), KyUInt32 broadcastPort = DefaultBroadcastPort(), MessageReceiverRegistry* navigationLabCommandHandlers = KY_NULL)
	{
		
		m_mode                         = Mode_UseNavigationLab;
		m_navigationLabCommandHandlers = navigationLabCommandHandlers;
		m_serverPort                   = serverPort;
		m_broadcastPort                = broadcastPort;
		m_localFileName                = KY_NULL;
		m_localFileOpener              = KY_NULL;
	}

	void UseLocalFile(const char* localFileName, FileOpenerBase* localFileOpener = KY_NULL)
	{
		m_mode                         = Mode_UseLocalFile;
		m_navigationLabCommandHandlers = KY_NULL;
		m_serverPort                   = 0;
		m_localFileName                = localFileName;
		m_localFileOpener              = localFileOpener;
	}

	void SetLocalFileSizeLimit(KyInt32 localFileMegaBytesLimit) { m_localFileMegaBytesLimit = localFileMegaBytesLimit; }

	void SetWaitOnStart(WaitOnStart waitOnStart) { m_waitOnStart = waitOnStart; }
	
	/// Define memory limitations for the VisualDebugServer
	void SetMemoryConsumption(KyUInt32 expectedKiloBytesPerFrame, KyUInt32 maximumMegaBytesToUse)
	{
		m_expectedKiloBytesPerFrame = expectedKiloBytesPerFrame;
		m_maximumMegaBytes = maximumMegaBytesToUse;
	}

	void SetDisconnectionMode(DisconnectionMode disconnectionMode) { m_disconnectionMode = disconnectionMode; }
	
	void SetRunMode(RunMode runMode) { m_runMode = runMode; }

	void SetServerName(const char* serverName) { m_serverName = String(serverName); }

public: // internal
	/// The visual debug information destination mode.
	Mode m_mode;

	// ------------- Relevant attributes if #m_mode = Mode_UseNavigationLab. -----------

	/// The port used to listen to NavigationLab incoming connections
	KyUInt32 m_serverPort;
	
	/// The port used to broadcast the existence of this server
	KyUInt32 m_broadcastPort;

	WaitOnStart m_waitOnStart;

	/// Optional, handles the commands that come from the Gameware Navigation from the Network.
	/// This is typically set in our examples lightweight game engine, the LabGame and its LabEngine
	/// to control the examples from the NavigationLab. Works only if running in Mode_UseNavigationLab mode.
	MessageReceiverRegistry* m_navigationLabCommandHandlers;

	// ------------- Relevant attributes if #m_mode = Mode_UseLocalFile. -----------

	/// mandatory if #m_mode = Mode_UseLocalFile.
	const char* m_localFileName;

	/// if #m_localFileOpener is NULL, DefaultFileOpener will be used
	FileOpenerBase* m_localFileOpener;
	
	/// The size limit of the local file, when this limit is reached, the VisualDebugServer disconnects
	KyInt32 m_localFileMegaBytesLimit;

	// ------------- Memory allocations of messages -----------

	/// Specifies the maximum amount of memory this VisualDebugServer is allowed to allocate for creating messages.
	KyUInt32 m_maximumMegaBytes;
	/// Specifies the expected amount of memory to allocate per frame. Used to define size of a message.
	KyUInt32 m_expectedKiloBytesPerFrame;

	// ------------- Various attributes -----------

	/// Define how to detect disconnections
	DisconnectionMode m_disconnectionMode;

	/// Define how to run the server, either directly in the current thread (recommended), or using secondary threads.
	RunMode m_runMode;

	/// Associate a name to the server to ease identification of this server when connecting from the NavigationLab
	String m_serverName;
};

}

// Define KY_DISABLE_VISUALDEBUGSERVER to disable VisualDebugServer
// In that case, it will be replaced by fully inlined classe, DisabledVisualDebugServer, 
// thanks to the define at the end of this file.
// If VisualDebugServer is modified, please apply the changes into disabledvisualdebugserver.inl as well.
#if defined(KY_BUILD_SHIPPING) || !defined(KY_ENABLE_SOCKETS) || !defined(KY_ENABLE_THREADS)
# define KY_DISABLE_VISUALDEBUGSERVER
#endif

#ifndef KY_DISABLE_VISUALDEBUGSERVER

#include "gwnavruntime/visualdebug/amp/Amp_ThreadMgr.h"
#include "gwnavruntime/visualdebug/internal/socketdispatcher.h"
#include "gwnavruntime/visualdebug/internal/visualdebugstat.h"
#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavruntime/blob/blobhandler.h"


namespace Kaim
{

/// The VisualDebugServer manages the sending of data to clients.
/// Each World maintains an instance of this class, accessible through its World::GetVisualDebugServer() and created on World::StartVisualDebug() call.
/// Call NewFrame() must be called at the beginning of each game frame in your game loop in order to get frames correctly rendered in the NavigationLab.
/// IsConnected() should be called to know when VisualDebugServer is ready for calling Send* functions to be sure not to do useless computations.
/// VisualDebugServer is considered ready, i.e. IsConnected returning true, after NewFrame() was called while the connection is established and the synchronization step ended.
/// With KY_BUILD_SHIPPING defined, this class is still present but replaced by a fully inlined emptied version.
class VisualDebugServer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)

public:

	// --------------- Server Status ----------------

	/// Indicates the beginning of a new game frame. Auto-increment the frame index
	/// _NOTE_ : NewFrame() must be called at the beginning of the Game frame so that a frame of VisualDebugServer mathces exactly a Game frame
	void NewFrame();
	void NewFrame(KyUInt32 gameFrameIndex) { NewFrame(); m_gameFrameIndex = gameFrameIndex; } ///< same as NewFrame() but force the frame index

	/// \return true between Start and Stop calls, false otherwise.
	bool IsStarted() const { return m_isStarted; }

	/// Tells if it's worth calling the Send* functions.
	bool IsConnected() { return (m_isConnected == true && IsConnectionEstablished() == true); }

	// ------------------ Send Statistics ------------------ 
	/// Send the key-value with min, max, average values that will be displayed in the NavigationLab under the given group name with other key-values.

	void SendStats(const char* statName, const KyFloat32 value, const KyFloat32 average, const KyFloat32 max, const KyFloat32 min, const char* groupName);
	void SendStats(const FloatStat& timeStat, const char* groupName);
	void SendStats(const char* statName, const FloatStat& timeStat, const char* groupName);


	// ------------------ Send Single Value ------------------ 
	/// Send a key-value pair that will be displayed in the NavigationLab under the given group name with other key-values.

	void SendUInt32(const char* statName, const KyUInt32 value, const char* groupName);
	void SendInt32(const char* statName, const KyInt32 value, const char* groupName);
	void SendFloat32(const char* statName, const KyFloat32 value, const char* groupName);
	void SendString(const char* statName, const char* value, const char* groupName);

	// --------------- Send Game-Specific info ---------------

	void SendGameCamera(const Vec3f& cameraPos, const Vec3f& cameraLookAtPos, const Vec3f& cameraUp, KyFloat32 fovInDegrees);
	void SendGameCameraAttitude(const Vec3f& cameraPos, const Vec3f& cameraRight, const Vec3f& cameraUp, const Vec3f& cameraFront, KyFloat32 fovInDegrees);

	// --------------- Advanced usage : Handling Received Blob Messages ----------------

	void PauseMessageProcessing()  { m_messageProcessingIsPaused = true;  }
	void ResumeMessageProcessing() { m_messageProcessingIsPaused = false; }

private:
	KyResult OpenNavigationLabConnection();
	KyResult OpenFileConnection();

	KyResult AggregateMessage(Ptr<BaseBlobHandler> handlerPtr);
	KyResult CreateAggregateMessage(KyUInt32 deepBlobSize);
	KyResult SendAggregateMessage();

	KyResult SetupMessageRegistry(MessageReceiverRegistry* receiverRegistry);
	KyResult OpenConnection(SocketDispatchMode mode, KyUInt32 port, KyUInt32 broadcastPort, bool initSocketConnection);

	void AddToStats(KyUInt32 blobtypeId, KyUInt32 deepBlobSize);
	void ClearStatistics();
	void SendInternalStatistics();

	void SendStatGroups();
	VisualDebugStatGroup* GetStatGroup(const char* groupName);
	VisualDebugStatGroup* GetOrCreateStatGroup(const char* groupName);

	void ClearSession(); // clear stats, message, timer, gameFrameIndex
	void OnDisconnection(); // called in VisualDebugServer's operatingThread to clear the session if OnDisconnectionUnsafeCallback() was called from the socketThread
	void OnConnection(); // set m_isConnected to true and apply what has to be done in VisualDebugServer's operatingThread on connection

	KyResult OnConnectionUnsafeCallback();    // called from the socketThread when connection is established
	KyResult OnDisconnectionUnsafeCallback(); // called from the socketThread when disconnected

	void UpdateThreadMgr(bool& out_needToWait);

	// Checks if there is enough memory before creating a new message
	void CheckMemory();

	void InitAttributesManager();
	void CreateProfilingAttributes();
	void CreateAdvancedProfilingAttributes();
	void UpdateProfilingAttributes();
	void UpdateAdvancedProfilingAttributes();

	// Indicate if the step of synchronization is disabled or not, used internally
	void DisableSynchronization(bool disabled) { m_synchronizationStepDisabled = disabled; }

	// \return true if synchronizing is required, false is sync is done or disabled
	bool Synchronize();

public: // internal

	// --------------- For internal use only ----------------
	/// These functions are used by internal components.  They are mainly used by World and WorldElement instances.
	/// So there's no need for Navigation's users to call them directly

	VisualDebugServer();
	~VisualDebugServer();

	KyResult Start(const VisualDebugServerConfig& config);
	KyResult ReStart() { return Start(m_config); }
	void Stop();

	/// Defines a special value that can be passed to VisualDebugServer::WaitWhileEstablishingConnection()
	/// in the calling thread until the connection is established.
	static KyUInt32 GetInfiniteWaitDelay() { return KY_WAIT_INFINITE; }

	/// Wait for the connection to be established (pass GetInfiniteWaitDelay() to wait infinitely if no connection).
	bool WaitWhileEstablishingConnection(KyUInt32 maxDelayMilliseconds);

	/// Effectively, send/write all messages (exactly they are passed to the OS).
	void Flush();

	/// Usually, set by Kaim::World with the given simulation time passed to Kaim::World::Update().
	/// It can differ from the frame time computed in NewFrame(), so a VisualDebug replay can be played with the frame time or the simulation time.
	/// By default, the NavigationLab will use the simulation time if it is provided instead of the frame time,
	/// because simulation time is usually a fix step which is better to use for debugging purposes.
	void SetSimulationTime(KyFloat32 simulationTimeInSeconds) { m_simulationTimeInSeconds = simulationTimeInSeconds; }

	/// Sends a blob from a BlobBuilder, the blob will be built into the message, no copy occurs.
	/// This Send function should be favor over Send(BaseBlobHandler&) since the memory used to build the blobs is reused here
	template <class T>
	KyResult Send(BaseBlobBuilder<T>& blobBuilder)
	{
		const KyUInt32 blobType = T::GetBlobTypeId();
		Ptr<BaseBlobHandler> blobHandler = m_handlerRegistry.Get(blobType);
		if (blobHandler == KY_NULL)
		{
			blobHandler = *KY_NEW BlobHandler<T>;
			m_handlerRegistry.SetVal(blobType, blobHandler);
		}

		blobBuilder.Build((BlobHandler<T>&)*blobHandler);
		return this->AggregateMessage(blobHandler);
	}

	/// Sends a blob, the blob will be copied into a secondary buffer.
	/// Send(BaseBlobBuilder<T>&) should be favor over this Send function since the memory used to build the blobs will be reused there.
	/// Portion of code using this function when KY_BUILD_SHIPPING is defined should be guarded with #ifndef KY_BUILD_SHIPPING
	/// in order to save cpu and memory taken to build the blob in Shipping build.
	KyResult Send(BaseBlobHandler& blobHandler);

	// Send a IQuery if the inheriting query supports it
	// _NOTE_: Currently, only used in order to send back the result of a query requested by the NavigationLab.
	KyResult Send(IQuery* query);

	/// Get and handle all received messages based on the MessageReceiverRegistry parameter provided to Start()
	KyResult HandleAllReceivedMessages();
	/// \return KY_SUCCESS if a message was handled, KY_ERROR if there's no message to handle.
	KyResult HandleNextReceivedMessage();
	/// Return next received message
	Ptr<Net::AMP::Message> GetNextReceivedMessage();
	/// Handle the given message
	KyResult HandleMessage(Ptr<Net::AMP::Message> msg);

	/// Enable additional statistics to be sent. These statistics are mainly useful for internal development/debugging.
	void SetAdvancedInternalStatistics(bool enable) { m_enableAdvancedInternalStatistics = enable; }

	/// Set the ThreadId that Send and Update are supposed to be called from. By default, assigned when calling Start.
	void SetThreadId(ThreadId threadId) { m_operatingThreadId = threadId; }

	/// Get the ThreadId that Send and Update are supposed to be called from.
	ThreadId GetThreadId() const  { return m_operatingThreadId; }

	/// Check if it is safe to call Update and Send functions.
	bool CanSafelyOperate() const { return (m_operatingThreadId == GetCurrentThreadId()); }

	/// Indicates whether the ThreadManager has established a connection to a network socket or if the localFile can be written.
	/// The connection being established doesn't mean the VisualDebugServer is ready to send information, cf. IsConnected()
	bool IsConnectionEstablished() const;

	/// Tells if currently synchronizing, in this state Send should only be called to send synchronization data (typically the NavData to be used for debugging).
	bool IsSynchronizing() { return (m_isConnected == false && IsConnectionEstablished() == true); }

	// Wait until all messages are sent (or if connection is lost or ended client-side).
	void WaitForEmptiedSendQueue();

	/// Retrieves the number of the current frame. 
	KyUInt32 GetFrameIndex() const { return m_gameFrameIndex; }

	// Called when EndOfSynchronizationBlob is received, done internally by a receiver set by the VisualDebugServer.
	void OnSynchronizationDone() { m_isSynchronized = true; }

	Net::AMP::ThreadMgr* GetThreadMgr()             { return m_socketThreadMgr.GetPtr(); } ///< Accessor to the internal ThreadMgr
	const Net::AMP::ThreadMgr* GetThreadMgr() const { return m_socketThreadMgr.GetPtr(); } ///< Accessor to the internal ThreadMgr

	/// \return The MemoryHeap used for sending messages.
	MemoryHeap* GetHeap() { return Memory::GetHeapByAddress(m_socketThreadMgr.GetPtr()); }

private:
	// Config
	VisualDebugServerConfig m_config; // The config passed to Start()
	KyUInt32  m_gameFrameIndex; // The current frame index
	KyFloat32 m_simulationTimeInSeconds;
	Profiler* m_gameFrameTimeProfiler;
	bool m_synchronizationStepDisabled;

	// Status
	bool m_isStarted;  // Set to true between calls to Start and Stop
	bool m_isConnectionEstablished;
	bool m_isSynchronized; // Set to true when EndOfSynchronization is received y the client, not though m_isConnected is not yet set to true, NewFrame needs to be called.
	bool m_isConnected; // set to true if m_isConnectionEstablished is true on a call to NewFrame that successfully ended the synchronization, so IsConnected can be used to ensure only full frames are sent
	bool m_wasDisconnected; // Used to cancel previous m_aggregatedBlobMsg
	ThreadId m_operatingThreadId; // ThreadId of the thread that Send functions and Update are supposed to be called from

	// Socket/Thread management
	Ptr<Net::AMP::ThreadMgr> m_socketThreadMgr;
	Ptr<Net::AMP::DiscardMessageHandler> m_msgDiscarder; // used to discard messages we are not interested in
	Ptr<MessageBlobHandler> m_msgBlobHandler;            // used to handle received MessageBlob
	SocketDispatcherFactory* m_socketFactory;            // in charge of creating the socket depending on VisualDebugServerConfig::Mode
	Ptr<File> m_file;                                    // used when mode is Mode_UseLocalFile 
	
	friend class ServerStatusChangedCallback;
	Ptr<ServerStatusChangedCallback> m_statusCallback; // Callback for connection status change
	Event m_connectedEvent;

	// Message construction
	typedef UserPtrBlobRegistry<BaseBlobHandler> BlobHandlerRegistry;
	BlobHandlerRegistry m_handlerRegistry;
	MessageAggregatedBlob* m_aggregatedBlobMsg;

	// Message reception
	List<Net::AMP::Message> m_msgList;
	Kaim::UInt32 m_msgListSize;

	// Stats
	KyUInt32 m_messagesNotSentOnWait;
	KyUInt32 m_memoryLimitReached; // Records if the memory limit was reached during this frame
	KyFloat32 m_totalFlushTimeMs; // Time spend waiting for the Send queue to be emptied (either in NewFrame or in Flush, or even in CheckMemory)

	KyUInt32 m_sentBlobCountInFrame;
	KyUInt32 m_sentBlobSizeInFrame;
	KyUInt32 m_sentMessageCountInFrame;
	KyUInt32 m_receivedMessageCountInFrame;

	UserBlobRegistry<SentBlobTypeStats> m_sentBlobTypeStatsRegistry;

	KyArray<VisualDebugStatGroup> m_statGroups;
	bool m_enableAdvancedInternalStatistics;

	VisualDebugAttributesManager* m_visualDebugAttributesManager;
	VisualDebugAttributeGroup* m_profilingVisualDebug;
	VisualDebugAttributeGroup* m_advancedProfilingVisualDebug;

	bool m_messageProcessingIsPaused;
};

} // namespace Kaim

#else
// Here we use a separated file in order to easily maintain both versions by comparing the two files (visualdebugserver.h and disabledvisualdebugserver.inl)
# include "gwnavruntime/visualdebug/disabledvisualdebugserver.inl"

// This allows to forward declare VisualDebugServer
// and to use DisabledVisualDebugServer separately in unit tests
namespace Kaim
{

class VisualDebugServer : public DisabledVisualDebugServer
{
};

}
#endif

#endif // GwNavRuntime_VisualDebugServer_H

