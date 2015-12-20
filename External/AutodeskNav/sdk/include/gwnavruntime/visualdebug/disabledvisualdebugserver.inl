/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifdef KY_DISABLE_VISUALDEBUGSERVER

#include "gwnavruntime/visualdebug/amp/Amp_Message.h"

namespace Kaim
{

namespace Net
{
namespace AMP
{
class ThreadMgr;
}
}

// The VisualDebugServer manages the sending of data to clients.
// Each World maintains an instance of this class, accessible through its World::GetVisualDebugServer() and created on World::StartVisualDebug() call.
// Call NewFrame() must be called at the beginning of each game frame in your game loop in order to get frames correctly rendered in the NavigationLab.
// IsConnected() should be called to know when before VisualDebugServer is ready for calling Send* functions to be sure not to do useless computations.
// VisualDebugServer is considered ready after NewFrame() was called while the connection is established.
// With KY_BUILD_SHIPPING defined, this class is still present but replaced by a fully inlined emptied version.
class DisabledVisualDebugServer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)

public:

	// --------------- Server Status ----------------

	// Indicates the beginning of a new game frame. Auto-increment the frame index
	// _NOTE_ : NewFrame() must be called at the beginning of the Game frame so that a frame of VisualDebugServer mathces exactly a Game frame
	KY_INLINE void NewFrame() {}
	KY_INLINE void NewFrame(KyUInt32) {} //< same as NewFrame() but force the frame index

	// \return true between Start and Stop calls, false otherwise.
	KY_INLINE bool IsStarted() const { return false; }

	// Tells if it's worth calling the Send* functions.
	KY_INLINE bool IsConnected() { return false; }

	// ------------------ Send Statistics ------------------ 
	// Send the key-value with min, max, average values that will be displayed in the NavigationLab under the given group name with other key-values.

	KY_INLINE void SendStats(const char*, const KyFloat32, const KyFloat32, const KyFloat32, const KyFloat32, const char*) {}
	KY_INLINE void SendStats(const FloatStat&, const char*) {}
	KY_INLINE void SendStats(const char*, const FloatStat&, const char*) {}


	// ------------------ Send Single Value ------------------ 
	// Send a key-value pair that will be displayed in the NavigationLab under the given group name with other key-values.

	KY_INLINE void SendUInt32(const char*, const KyUInt32, const char*) {}
	KY_INLINE void SendInt32(const char*, const KyInt32, const char*) {}
	KY_INLINE void SendFloat32(const char*, const KyFloat32, const char*) {}
	KY_INLINE void SendString(const char*, const char*, const char*) {}

	// --------------- Send Game-Specific info ---------------

	KY_INLINE void SendGameCamera(const Vec3f&, const Vec3f&, const Vec3f&, KyFloat32) {}
	KY_INLINE void SendGameCameraAttitude(const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&, KyFloat32) {}

	// --------------- Advanced usage : Handling Received Blob Messages ----------------
	// Handle received messages based on the MessageReceiverRegistry parameter provided to Start()
	// HandleNextReceivedMessage() can be used alone to retrieve the next message and handle it immediately.
	// \code
	// while (server->HandleNextReceivedMessage() == KY_SUCCESS) {}
	// \endcode
	//
	// Alternatively, for more advanced use, for instance to first store all messages and then to handle them later,
	// you can use GetNextReceivedMessage() and HandleMessage().
	// \code
	// Ptr<Kaim::Net::AMP::Message> msgPtr = server->GetNextReceivedMessage();
	// while (msgPtr != KY_NULL)
	// {
	//     HandleMessage(msgPtr); 
	//     msgPtr = server->GetNextReceivedMessage();
	// }
	// \endcode

	// \return KY_SUCCESS if a message was handled, KY_ERROR if there's no message to handle.
	KY_INLINE KyResult HandleNextReceivedMessage() { return KY_ERROR; }
	// Return next received message
	KY_INLINE Ptr<Net::AMP::Message> GetNextReceivedMessage() { return KY_NULL; }
	// Handle the given message
	KY_INLINE KyResult HandleMessage(Net::AMP::Message*) { return KY_ERROR; }

	KY_INLINE void PauseMessageProcessing()  {}
	KY_INLINE void ResumeMessageProcessing() {}

private:

public: // internal

	// --------------- For internal use only ----------------
	// These functions are used by internal components.  They are mainly used by World and WorldElement instances.
	// So there's no need for Navigation's users to call them directly

	DisabledVisualDebugServer() {}
	~DisabledVisualDebugServer() {}

	KY_INLINE KyResult Start(const VisualDebugServerConfig&) { return KY_ERROR; }
	KY_INLINE KyResult ReStart() { return KY_ERROR; }
	KY_INLINE void Stop() {}

	// Defines a special value that can be passed to VisualDebugServer::WaitWhileEstablishingConnection()
	// in the calling thread until the connection is established.
	static KyUInt32 GetInfiniteWaitDelay() { return KY_WAIT_INFINITE; }

	// Wait for the connection to be established (pass GetInfiniteWaitDelay() to wait infinitely if no connection).
	KY_INLINE bool WaitWhileEstablishingConnection(KyUInt32) { return false; }

	// Effectively, send/write all messages (exactly they are passed to the OS).
	KY_INLINE void Flush() {}

	// Usually, set by Kaim::World with the given simulation time passed to Kaim::World::Update().
	// It can differ from the frame time computed in NewFrame(), so a VisualDebug replay can be played with the frame time or the simulation time.
	// By default, the NavigationLab will use the simulation time if it is provided instead of the frame time,
	// because simulation time is usually a fix step which is better to use for debugging purposes.
	KY_INLINE void SetSimulationTime(KyFloat32) {}

	// Sends a blob from a BlobBuilder, the blob will be built into the message, no copy occurs.
	// This Send function should be favor over Send(BaseBlobHandler&) since the memory used to build the blobs is reused here
	template <class T>
	KY_INLINE KyResult Send(BaseBlobBuilder<T>&) { return KY_ERROR; }

	// Sends a blob, the blob will be copied into a secondary buffer.
	// Send(BaseBlobBuilder<T>&) should be favor over this Send function since the memory used to build the blobs will be reused there.
	// Portion of code using this function when KY_BUILD_SHIPPING is defined should be guarded with #ifndef KY_BUILD_SHIPPING
	// in order to save cpu and memory taken to build the blob in Shipping build.
	KY_INLINE KyResult Send(BaseBlobHandler&) { return KY_ERROR; }

	// Send a IQuery if the inheriting query supports it
	// _NOTE_: Currently, only used in order to send back the result of a query requested by the NavigationLab.
	KY_INLINE KyResult Send(IQuery*) { return KY_ERROR; }

	// Enable additional statistics to be sent. These statistics are mainly useful for internal development/debugging.
	KY_INLINE void SetAdvancedInternalStatistics(bool) {}

	// Set the ThreadId that Send and Update are supposed to be called from. By default, assigned when calling Start.
	KY_INLINE void SetThreadId(ThreadId) {}

	// Get the ThreadId that Send and Update are supposed to be called from.
	KY_INLINE ThreadId GetThreadId() const  { return 0; }

	// Check if it is safe to call Update and Send functions.
	KY_INLINE bool CanSafelyOperate() const { return false; }

	// Indicates whether the ThreadManager has established a connection to a network socket or if the localFile can be written.
	// The connection being established doesn't mean the VisualDebugServer is ready to send information, cf. IsConnected()
	KY_INLINE bool IsConnectionEstablished() const { return false; }

	// Tells if currently synchronizing, in this state Send should only be called to send synchronization data (typically the NavData to be used for debugging).
	KY_INLINE bool IsSynchronizing() { return false; }

	// Wait until all messages are sent (or if connection is lost or ended client-side).
	KY_INLINE void WaitForEmptiedSendQueue() {}

	// Retrieves the number of the current frame. 
	KY_INLINE KyUInt32 GetFrameIndex() const { return KyUInt32MAXVAL; }

	// Called when EndOfSynchronizationBlob is received, done internally by a receiver set by the VisualDebugServer.
	KY_INLINE void OnSynchronizationDone() {}

	KY_INLINE Net::AMP::ThreadMgr* GetThreadMgr()             { return KY_NULL; } //< Accessor to the internal ThreadMgr
	KY_INLINE const Net::AMP::ThreadMgr* GetThreadMgr() const { return KY_NULL; } //< Accessor to the internal ThreadMgr

	// \return The MemoryHeap used for sending messages.
	KY_INLINE MemoryHeap* GetHeap() { return KY_NULL; }


private:
};

} // namespace Kaim

#endif

