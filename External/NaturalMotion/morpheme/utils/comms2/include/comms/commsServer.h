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
#ifndef MCOMMS_COMMSSERVER_H
#define MCOMMS_COMMSSERVER_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix34.h"
#include "comms/packet.h"
#include "comms/corePackets.h"
#include "comms/sceneObject.h"
#include "comms/connection.h"
#include "comms/connectionManager.h"
#include "comms/commsDebugClient.h"

#include "NMPlatform/NMHashMap.h"

#define LOG_BUFFER_SIZE    512

//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

struct PacketBase;
class ChunkMemory;
class CommsServerModule;
class CoreCommsServerModule;
class RuntimeTargetInterface;

//----------------------------------------------------------------------------------------------------------------------
/// Structures and types used by the string caching mechanism.
///
struct StringHashed
{
  StringToken  m_token;          // sequentially incrementing ID that uniquely identifies the string in question
  size_t       m_stringLength;   // we calculate this when we create this structure, so hash it for posterity
  const char*  m_string;         // pointer to the string buffer containing a copy of the original string
};

typedef NMP::hash_map<int, StringHashed> StringMap;

//----------------------------------------------------------------------------------------------------------------------
/// CommsServer encapsulates all the lower-level networking and general management code
/// and contains hooks that the derived class will implement to specific functionality.
class CommsServer
{
public:
  friend class ConnectionManager;

  CommsServer(RuntimeTargetInterface* target, int mainPortNumber);
  virtual ~CommsServer();

  static CommsServer* getInstance();

  /// Initialize and start the server; if initNetworking is true, the server will bring Winsock, XNet, etc online first;
  /// if the server is integrated into a game title, this is probably unnecessary, as the game will have done it already.
  bool startServer(bool initNetworking);
  void stopServer();

  // Deal with commands.
  bool update();

  inline void scheduleStep(bool s) { m_isStepScheduled = s; }
  inline bool isStepScheduled() const { return m_isStepScheduled; }
  bool isAsynchronousSteppingEnabled() const;
  bool shouldSendFrameDataToConnection(const Connection* connection) const;

  virtual void sendDefData(const GUID& networkGUID, Connection* connection);
  virtual void sendInstanceStaticData(const GUID& networkGUID, InstanceID id, Connection* connection);

  virtual void beginFrame(float deltaTime);
  virtual void endFrame(float deltaTime);
  virtual void sendFrameData(Connection* connection);

  virtual void beginUpdateFrameData(Connection* connection, float deltaTime);
  virtual void endUpdateFrameData(Connection* connection, float deltaTime);

  /// adds a packet to be sent to all connections.
  bool broadcastPacket(PacketBase& pkt);

  /// access the string cache; either return a fresh ID for the given string or
  /// return the ID assigned during a previous call. All new strings are automatically
  /// transmitted to the host; the StringToken can then be used to reference the strings
  /// in other packet structures.
  StringToken getTokenForString(const char* str);

  inline uint32_t getFrameTicker() const { return m_frameTicker; }

  inline bool isInited() const { return m_inited; }
  inline bool isServerRunning() const { return isInited(); }

  /// The server identifies itself through the connection. By default it calls the methods to identify
  /// the protocol and the runtime capabilities.
  /// This method is executed in the context of the commands thread. Use with care.
  virtual void onNewConnection(Connection* newConnection);

  /// <
  virtual void onConnectionClosed(Connection* closedConnection);

  /// <
  virtual void onStartSession(Connection* connection);

  /// <
  virtual void onStopSession(Connection* connection);

  /// <
  inline RuntimeTargetInterface* getRuntimeTargetInterface() const { return m_target; }

  // Send cached string data.
  void sendStringMap();

  virtual void identify(IdentificationReplyPacket& identityPacket) const;

  void sendTargetStatus(Connection* connection);
  void broadcastTargetStatus();

protected:
  ///
  bool handleCmdPacket(CmdPacketBase* cmdPacket, Connection* connection, bool& gotStep);

  static CommsServer*    sm_instance;

private:
  bool initConnectionManager();
  void termConnectionManager();

  bool init(bool initNetworking);
  void term();

  bool  m_inited;
  bool  m_networkInited;

  uint32_t              m_stringTokenIndex;
  StringMap*            m_stringMap;
  ChunkMemory*          m_stringCacheMMgr;

  uint32_t              m_frameTicker;

protected:
  ConnectionManager*      m_connectionManager;
  RuntimeTargetInterface* m_target;

public:
  static uint32_t getMaxNumberOfConnections() { return ConnectionManager::MAX_NUMBER_CONNECTIONS; }
  uint32_t getNumConnections() const;
  uint32_t getNumRunningConnections() const;
  Connection* getConnection(uint32_t connIndex) const;

  bool  m_isStepScheduled;

  /// < buffer used to format log messages.
  wchar_t               m_logBuffer[LOG_BUFFER_SIZE];

  StepCmdPacket m_lastMasterStepCmd;

  int m_mainPortNumber;

public:
  void registerModule(CommsServerModule* module);
  void unregisterModule(CommsServerModule* module);

  CoreCommsServerModule* getCoreModule() const { return m_coreModule; }

private:
  static const uint8_t   MAX_NUM_MODULES = 8;
  uint8_t                m_numModules;
  CommsServerModule*     m_modules[MAX_NUM_MODULES];
  CoreCommsServerModule* m_coreModule;

public:
  inline NetworkError getLastNetworkError() const { return m_lastNetworkError; }
  inline void setLastNetworkError(NetworkError networkError) { m_lastNetworkError = networkError; }

private:
  NetworkError          m_lastNetworkError;
};

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4100)
#endif

/// a memory manager for the string cache buffer.
/// it can only allocate, deallocates do nothing. once finished with, all heaps are simply discarded.
/// it creates a heap of 'heapSize' bytes and allocates directly from that
/// pool without adding headers/footers to the returned pointers - once full, it allocates
/// new pools of at least 'subHeapSize' bytes.
class ChunkMemory
{
public:
  ChunkMemory(size_t heapSize, size_t subHeapSize);
  ~ChunkMemory();

  // malloc & free substitutes
  void* alloc   (size_t size);
  void  dealloc (void* ptr);

  // reset the manager but don't discard heaps;
  // allows for easy reuse of pool memory
  void  resetForReuse();

protected:
  void logStats();

  // create a new heap and assign to m_currentHeap
  void newSubHeap(size_t sz);

  // heap structure used to store everything we need to know about
  // a single storage block
  struct Heap
  {
    unsigned char*  m_heap;
    size_t          m_heapUse;
    size_t          m_heapSize;
    unsigned int    m_pad;
  };

  Heap              m_heapList[64];     ///< list of used heaps
  unsigned int      m_heapIndex;        ///< number of used heap entries
  Heap*             m_currentHeap;      ///< heap being used presently
  unsigned int      m_heapWalkIdx;      ///<

  unsigned int      m_allocCount;       ///< number of alloc calls
  unsigned int      m_deallocCount;     ///< number of dealloc calls
  size_t            m_subHeapSize;      ///< size of a new sub-heap, set in ctor
  size_t            m_totalAllocSize;   ///< total memory usage footprint
};

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_COMMSSERVER_H
//----------------------------------------------------------------------------------------------------------------------
