/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

Filename    :   Amp_ThreadMgr.h
Content     :   Manages the socket threads
Created     :   December 2009
Authors     :   Alex Mantzaris

**************************************************************************/


#ifndef INC_KY_AMP_THREAD_MGR_H
#define INC_KY_AMP_THREAD_MGR_H


#include "gwnavruntime/kernel/SF_Types.h"

#ifdef KY_ENABLE_SOCKETS

#include "gwnavruntime/visualdebug/amp/Amp_Socket.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/visualdebug/amp/Amp_Interfaces.h"
#include "gwnavruntime/kernel/SF_Threads.h"
#include "gwnavruntime/visualdebug/amp/Amp_MessageRegistry.h"
#include "gwnavruntime/kernel/SF_File.h"
#include "gwnavruntime/kernel/SF_Log.h"
#include "gwnavruntime/visualdebug/amp/Amp_Stream.h"


namespace Kaim {
    // Forward declarations
    class Lock;
    class Event;
namespace Net {
namespace AMP {


// Utility class allows to discard messages as soon as they arrive
class DiscardMessageHandler : public IMessageHandler
{
    virtual void Handle(Message*)
    {
        //Do nothing, it allows the ThreadMgr to discard messages as they arrive
    }

    virtual Mode HandleMode() { return Handle_AfterDecompression; }
};

// Manages the socket connection threads for AMP
// An instance of this class is contained both in the AMP server singleton and in the AMP client
// The caller can pass in callback objects for notification
// The SendInterface::OnSendLoop gets called from the socket thread for app-specific processing
// The ConnStatusInterface::OnStatusChanged gets called whenever the connection status is updated
// A MsgHandler can be specified for immediate message handling, bypassing the received queue (for AS debugger)
class ThreadMgr : public RefCountBase<ThreadMgr, StatAmp_Server>
{
public:
    ThreadMgr(bool initSocketLib, SendInterface* sendCallback, 
              ConnStatusInterface* connectionCallback, Kaim::Event* sendQueueWaitEvent,
              SocketImplFactory* socketFactory, MessageTypeRegistry* msgTypeRegistry = NULL,
              SInt32 processorForSocketLoop = -1, SInt32 processorForCompressLoop = -1);
    virtual ~ThreadMgr();

    // Initialize
    void                ForceConnectionBySkippingSetupMessages() { DoForceConnectionBySkippingSetupMessages = true; }
    void                InitSync(const char* ipAddress, UInt32 port, UInt32 broadcastPort);
    bool                ConnectSync(UInt32 timeoutMs);
    void                StartBroadcastRecvSync(UInt32 port);
#if defined(KY_ENABLE_THREADS)
    // Initialize AMP - the threads for sending and receiving messages
    bool                InitAsync(const char* ipAddress, UInt32 port, UInt32 broadcastPort);
    // Initialize the broadcast receive thread
    void                StartBroadcastRecv(UInt32 port);
#endif
    // Set the connected app
    void                SetBroadcastInfo(const char* connectedApp, const char* connectedFile);

    // Uninitialize AMP - Performs thread cleanup
    void                UninitAmp();

    // Heartbeat interval
    void                SetHeartbeatInterval(UInt32 heartbeatInterval);

    // Accessors
    const StringLH&                         GetAddress() const               { return IpAddress; }
    UInt32                                  GetPort() const                  { return Port; }
    UInt32                                  GetBroadcastRecvPort() const     { return BroadcastRecvPort; }
    
    ConnStatusInterface::StatusType         GetConnectionStatus() const;

    UInt32                                  GetReceivedQueueSize() const { return MsgReceivedQueue.GetSize(); }
    UInt32                                  GetUncompressedQueueSize() const { return MsgUncompressedQueue.GetSize(); }
    UInt32                                  GetSendQueueSize() const { return MsgSendQueue.GetSize(); }
    UInt32                                  GetCompressedQueueSize() const { return MsgCompressedQueue.GetSize(); }
    UInt32                                  GetSendRate() const { return SendRate; }
    UInt32                                  GetReceiveRate() const { return ReceiveRate; }

    bool                                  IsReceptionPipelineEmpty() const { return (OutOfQueueInReceptionMessages + GetUncompressedQueueSize() + GetReceivedQueueSize()) == 0; }
    bool                                  IsEmissionPipelineEmpty() const { return GetTotalMessagesInEmissionPipeline() == 0; }
    
    UInt32                                GetTotalMessagesInEmissionPipeline() const { return (MessageInEmissionStream + OutOfQueueInEmissionMessages + GetCompressedQueueSize() + GetSendQueueSize()); }

    // Has a socket been successfully created? Thread-safe
    bool                IsValidSocket();
    // Has a message been received from the other end recently? Thread-safe
    bool                IsValidConnection();
    // Place a message on the queue to be sent.  Thread-safe
    enum MessageValidity
    {
        MessageIsValid = 0,
        UnregisteredMessageType,
        UnversionnedMessageType
    };
    MessageValidity      SendAmpMessage(Message* msg);
    void                 UpdateSync(bool & wereActionPerformed); //< should only be called if init with InitSync
    bool                 BroadcastRecv(); //< automatically called in secondary thread in Async, or must be called regularly in Sync mode

    // User MUST ensure that messages are valid
    void                 SendMultipleAmpMessages(List<Message>& msgList, UInt32 listSize)
    {
        MsgSendQueue.PushListToBack(msgList, listSize);
        CheckHeapLimit();
    }

    // Send a log message.  Thread-safe
    void                SendLog(const String& logMessage, LogMessageId messageType);

    void                ClearAllMessagesToSend();
    // Retrieve the next received message from the queue.  Thread-safe
    Ptr<Message>        GetNextReceivedMessage();
    void                GetAllReceivedMessages(List<Message>& msgList, UInt32& listSize)
    {
        MsgUncompressedQueue.PopToList(msgList, listSize);
    }

    // Clear all messages of the specified type from the received queue
    void                ClearReceivedMessages(const Message* msg);

    const MessageTypeRegistry&	GetMsgTypeRegistry() const { return MsgTypeRegistry; } // only provided as const because changing it while running would be unsafe
    // UNSAFE: This function should only be accessed from outside when ThreadMgr is running synchronous (InitSync, ConnectSync, UpdateSync..)
    // Read a stream and creates a message of the appropriate type
    Message*                    CreateAndReadMessage(File& str);

    // UNSAFE: This function should only be accessed from outside when ThreadMgr is running synchronous (InitSync, ConnectSync, UpdateSync..)
    // Create messages expected to be sent for connection
    void CreateConnectionSetupMessages(List<Message>& msgList);

    // used internally
    enum ExitStatus
    {
        ExitStatus_NotExiting = 0, // not exiting
        ExitStatus_Immediate = 1, // exit immediately
        ExitStatus_WaitForEmissionEmptiness = 2, // exit once all queued messages were sent including the disconnection message
        ExitStatus_WaitForReceptionEmptiness = 3, // exit once all queued messages were received
    };

    ExitStatus GetExitStatus()
    {
        return Exiting;
    }

public:
  
    
    // Stats
    AtomicInt<UInt32> SendStatLoop;
    AtomicInt<UInt32> SendStatTime;
    AtomicInt<UInt32> MaxSendStatTime;
    AtomicInt<UInt32> MaxSendRate;


private:
    enum 
    {
        // maximum packet size - larger messages are broken up
        BufferSize = 8*1024,  // 8 kBytes is the default size of Wii U socket's internal buffers

        // If no message has been sent for this long, we send a heartbeat message
#ifdef KY_OS_WII
        DefaultHeartbeatIntervalMillisecs = 0, // 0 means no heartbeat
#else
        DefaultHeartbeatIntervalMillisecs = 1000,
#endif
    };


    // MsgQueue encapsulates a message queue
    // It is a thread-safe list that keeps track of its size
    class MsgQueue
    {
    public:
        MsgQueue();
		~MsgQueue() { Clear(); }
        void            PushBack(Message* msg);
        Message*        PopFront();
        void            Clear();
        void            ClearMsgType(const Message* msg);
        UInt32          GetSize() const;

        void            PushListToBack(List<Message>& msgList, UInt32 listSize);
        void            PopToList(List<Message>& msgList, UInt32& listSize);

    public: // internal
        bool                    IsAsync;
        Lock                    QueueLock;
        List<Message>           Queue;
        AtomicInt<UInt32>       Size;
    };


    class SendReceiveContext : public NewOverrideBase<Stat_Default_Mem>
    {
    public:
        SendReceiveContext(Ptr<AmpStream> streamSend, Ptr<AmpStream> streamReceived);

        UInt64 LastSampleTime;
        UInt32 BytesSent;
        UInt32 BytesReceived;
        UPInt StreamSendDataLeft;
        const char* SendBuffer;

        UInt32 SilentFrameCount;
        UInt32 MaxSilentFrameCount;
        LogSilentMode SilentMode;

        UInt32 SendStatLoop;
        UInt64 SendStatTicks;

        char BufferReceived[ThreadMgr::BufferSize];

        Ptr<AmpStream> StreamSend;
        Ptr<AmpStream> StreamReceived;
    };
    
    class BroadCastRcvContext : public NewOverrideBase<Stat_Default_Mem>
    {
    public:
        BroadCastRcvContext(bool initSocketLib, SocketImplFactory* socketFactory)
            : WiiuSocket(initSocketLib, socketFactory)
            , LastWiiuAttempt(0)
        {}

        ~BroadCastRcvContext()
        {
            WiiuSocket.Destroy();
        }

        Socket WiiuSocket;
        UInt64 LastWiiuAttempt;
        char WiiuBuffer[BufferSize];
        char BufferReceived[BufferSize];
    };

    bool IsAsync;
    SInt32 ProcessorForSocketLoop;
    SInt32 ProcessorForCompressLoop;
#if defined(KY_ENABLE_THREADS)
    Ptr<Thread>         SocketThread;  // Takes messages from the send queue and sends them
    Ptr<Thread>         BroadcastThread;  // Broadcasts listener socket IP address and port 
    Ptr<Thread>         BroadcastRecvThread;  // Listens for broadcast messages 
    Ptr<Thread>         CompressThread;   // Uncompresses/compresses received/sending messages
#endif

    UInt32              Port;
    Lock                BroadcastInfoLock;
    UInt32              BroadcastPort;
    StringLH            BroadcastApp;
    StringLH            BroadcastFile;
    UInt32              BroadcastRecvPort;
    StringLH            IpAddress;
    bool                Server;  // Server or Client?
    bool                InitSocketLib;
    bool                DoForceConnectionBySkippingSetupMessages; // typically used to skip RegistryMessage and SetupMessage when simulating socket from file;
    Lock                SocketLock;
    Socket              Sock;
    BroadcastSocket     BroadcastSock; // used for receive _XOR_ send
    SendReceiveContext* SendReceiveCallContext;
    BroadCastRcvContext* BroadCastRcvCallContext;

    // Status is checked from send and received threads
    mutable Lock                        InitLock;
    mutable Lock                        StatusLock;
    AtomicInt<ExitStatus>               Exiting;
    UInt64                              LastSendHeartbeat;
    UInt64                              LastRcvdHeartbeat;
    ConnStatusInterface::StatusType     ConnectionStatus;
    bool                                LastConnected;
    AtomicInt<UInt32>                   SendRate;
    AtomicInt<UInt32>                   ReceiveRate;

    // Message queues
    MsgQueue                            MsgReceivedQueue;
    MsgQueue                            MsgSendQueue;
    MsgQueue                            MsgUncompressedQueue;
    MsgQueue                            MsgCompressedQueue;
    Kaim::Event*                        SendQueueWaitEvent;
    UInt32								LimitCheckHysterisisPercent;
    void								CheckHeapLimit();
    
    AtomicInt<UInt32>                   OutOfQueueInEmissionMessages; // used to maintained a count of messages that are treated but out of MsgCompressedQueue and MsgSendQueue
    AtomicInt<UInt32>                   MessageInEmissionStream; // Indicates if there's a message copied in AmpStream
    AtomicInt<UInt32>                   OutOfQueueInReceptionMessages; // used to maintained a count of messages that are treated but out of MsgUncompressedQueue and MsgReceivedQueue

    // Callback interfaces
    SendInterface*                      SendCallback;         // send thread callback
    ConnStatusInterface*                ConnectionChangedCallback; // connection changed callback
    SocketImplFactory*                  SocketFactory;

    MessageTypeRegistry                 MsgTypeRegistry;
    friend class DisconnectionMessageHandler;

#if defined(KY_ENABLE_THREADS)
    // Thread functions
    static int          SocketThreadLoop(Thread* sendThread, void* param);
    static int          BroadcastThreadLoop(Thread* broadcastThread, void* param);
    static int          BroadcastRecvThreadLoop(Thread* broadcastThread, void* param);
    static int          CompressThreadLoop(Thread* uncompressThread, void* param);
#endif

    // Private helper functions
    bool                UnCompress();
    bool                Compress();
    bool                SendReceiveCall(bool & wereActionPerformed);
    bool                Send(SendReceiveContext& context, bool& couldNotSend);
    bool                Receive(SendReceiveContext& context, bool& couldNotReceive);
    int                 Broadcast();

    bool                IsServer() const;
    bool                IsRunning() const;
    bool                IsExiting() const;
    void                ResetLastReceivedTime();
    bool                SocketConnect();
    Message*            RetrieveMessageForSending();
    bool                SendReceiveLoop();
    bool                BroadcastLoop();
    bool                BroadcastRecvLoop();
    bool                CompressLoop();
    void                UpdateStatus(ConnStatusInterface::StatusType eStatus, const char* pcMessage);

public: //internal
    void                UpdateLastReceivedTime(bool updateStatus);

public: //internal
    AtomicInt<UInt32>   HeartbeatIntervalMillisecs;
};

// helper class to enable or disable lock based on a boolean variable
class Locker
{
public:
    Locker(bool activated, Lock*lock)
        : m_locker(activated ? new Lock::Locker(lock) : NULL)
    {}

    ~Locker()
    {
        if (m_locker)
            delete m_locker;
    }

    Lock::Locker* m_locker;
};

// helper functions, their implementation only exists in debug
enum DebugMsgLogEnum
{
    // Following values activate messages for specific Threads they must be completed by other values:
    DbgML_SocketThread = 1,
    DbgML_CompressThread = 1<<1,
    DbgML_BroadcastThread = 1<<2,

    // Following values should be completed with DbgML_*Thread masks :
    DbgML_Emission = 1<<3, // activate log for messages in the Emission pipeline (Send, Compress ...) 
    DbgML_Reception = 1<<4, // activate log for messages in the reception pipeline (Receive, Uncompress ...)
    DbgML_SleepTime = 1<<5, // activate log in order to know when a thread is set to sleep

    // Independent values
    DbgML_Rate = 1<<6, // activate log for displaying reception and emission rate
    DbgML_HeartbeatTolerance = 1<<7, // if set the Heartbeat Interval is multiplied by 100
};



} // namespace AMP
} // namespace Net
} // namespace Kaim

#endif


namespace Kaim {
namespace Net {
namespace AMP {
void SetDebugMsgLog(UInt32 DebugMsgLogEnum_Mask); // used to toggle extreme verbosity of the ThreadMgr in debug only, do nothing in other modes.
UInt32 GetDebugMsgLog(); //< return a mask whose values are defined in DebugMsgLogEnum
} // namespace AMP
} // namespace Net
} // namespace Kaim


#endif // INC_KY_AMP_THREAD_MGR_H
