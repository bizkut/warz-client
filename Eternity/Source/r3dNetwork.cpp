#include "r3dPCH.h"
#include "r3d.h"
#include "r3dNetwork.h"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "RakNetStatistics.h"
#include "GetTime.h"

	int	_r3d_Network_DoLog = 0;

class r3dNetworkImpl
{
  public:
	r3dNetworkImpl();
	
	const char*	networkName;

	RakNet::RakPeerInterface* peer;
	bool            isHost;
	
	RakNet::SystemAddress* peerData;
	DWORD		maxPeers;

	RakNet::RakNetStatistics stat_;

	r3dNetCallback*	callback;
};

r3dNetworkImpl::r3dNetworkImpl()
{
  networkName  = "unknown";
  
  peer	       = NULL;
  isHost       = false;
  maxPeers     = 0;
  peerData     = NULL;

  callback     = NULL;
  
  memset(&stat_, 0, sizeof(stat_));
}

r3dNetwork::r3dNetwork()
{
  impl = NULL;
  dumpStats_ = 0;
}

r3dNetwork::~r3dNetwork()
{
  if(impl) r3dOutToLog("!!!! r3dNetwork %s is not deinitialized\n", impl->networkName);
}

int r3dNetwork::Initialize(r3dNetCallback* callback, const char* networkName)
{
  r3d_assert(impl == NULL);
  r3d_assert(callback);
  
  // make double sure that our first free packet ID is above raknet user range
  if(r3dNetwork::RAKNET_USER_PACKET < ID_USER_PACKET_ENUM)
	r3dError("bad RAKNET_USER_PACKET\n");
  
  //r3dOutToLog("r3dNetwork: Initialized\n"); CLOG_INDENT;
  
  impl = game_new r3dNetworkImpl;
  impl->networkName = networkName;
  impl->callback    = callback;
  impl->callback->net_ = this;
  
  return 1;
}

void r3dNetwork::Deinitialize()
{
  if(!impl)
    return;
    
  r3dOutToLog("r3dNetwork: Deinitialize\n");

  if(impl->peer)
  {
    impl->peer->Shutdown(1000);
    RakNet::RakPeerInterface::DestroyInstance(impl->peer);
    impl->peer = NULL;
  }

  if(dumpStats_ >= 1)
  {
    char statbuf[1024 * 8];
    RakNet::StatisticsToString(&impl->stat_, statbuf, 2);
    r3dOutToLog("r3dNetwork %s Global Stats:\n%s", impl->networkName, statbuf);
  }

  SAFE_DELETE_ARRAY(impl->peerData);
  impl->maxPeers = 0;
  
  impl->callback->net_ = NULL;
    
  SAFE_DELETE(impl);
  return;
}

void r3dNetwork::Update()
{
  if(!impl) 
    return;
    
  RakNet::Packet* p = NULL;
  while((p = impl->peer->Receive()) != NULL)
  {
    BYTE* data = p->data;
    int   len  = p->length;

	// Handle PunkBuster Packets
#ifdef __WITH_PB__
	if ( len > 7 && memcmp ( data, "\xff\xff\xff\xffPB_", 7) == 0 )
	{
		int clientIndex = -1 ;
		clientIndex = impl->peer->GetIndexFromSystemAddress(p->systemAddress) ;

		impl->callback->OnNetData(clientIndex, (const r3dNetPacketHeader*)data, len);
	    impl->peer->DeallocatePacket(p);
		continue ;
	}
#endif
    
    // skip timestamping
    if((unsigned char)p->data[0] == ID_TIMESTAMP) {
      data += sizeof(RakNet::MessageID) + sizeof(RakNet::Time);
      len  -= sizeof(RakNet::MessageID) + sizeof(RakNet::Time);
    }
    
    if(_r3d_Network_DoLog && data[0] < ID_USER_PACKET_ENUM) {
      r3dOutToLog("r3dNetwork: %s id:%d sa:%d/%d, guid:%I64x/%d, %s\n", 
      impl->networkName, 
      data[0], 
      p->systemAddress.systemIndex, 
      impl->peer->GetIndexFromSystemAddress(p->systemAddress),
      p->guid.g,
      p->guid.systemIndex,
      p->systemAddress.ToString(false, ':'));
    }

    switch(data[0])
    {
      case ID_CONNECTION_ATTEMPT_FAILED:
        break;
    
      case ID_NO_FREE_INCOMING_CONNECTIONS:
	//r3dOutToLog("ID_NO_FREE_INCOMING_CONNECTIONS\n");
	break;
    
      case ID_ALREADY_CONNECTED:
        r3dOutToLog("!!!! ALREADY CONNECTED\n");
      case ID_CONNECTION_REQUEST_ACCEPTED:
      {
        //r3dOutToLog("ID_CONNECTION_REQUEST_ACCEPTED: %d\n", p->systemAddress.systemIndex);
        r3d_assert(!impl->isHost);
        
        RakNet::SystemAddress extIp = impl->peer->GetExternalID(p->systemAddress);
        r3dOutToLog("r3dNetwork: external ip: %s\n", extIp.ToString(true, ':'));

        DWORD peerId = 0;
        r3d_assert(impl->peerData[peerId] == RakNet::UNASSIGNED_SYSTEM_ADDRESS);
        impl->peerData[peerId] = p->systemAddress;

	impl->callback->OnNetPeerConnected(peerId);
        break;
      }

      case ID_NEW_INCOMING_CONNECTION:
      {
        //Somebody connected.  We have their IP now
	//r3dOutToLog("ID_NEW_INCOMING_CONNECTION from %s.\n", p->systemAddress.ToString(true, ':'));
	r3d_assert(impl->isHost);

	DWORD peerId = p->systemAddress.systemIndex;
	r3d_assert(peerId < impl->maxPeers);
	r3d_assert(impl->peerData[peerId] == RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	impl->peerData[peerId] = p->systemAddress;

	impl->callback->OnNetPeerConnected(peerId);
        break;
      }

      case ID_CONNECTION_LOST:
        r3dOutToLog("r3dNetwork_Peer_Timeout: %s\n", p->systemAddress.ToString(false, ':'));
        // fallthru

      case ID_DISCONNECTION_NOTIFICATION:
      {
        // Couldn't deliver a reliable packet - i.e. the other system was abnormally terminated
	//r3dOutToLog("disconnected from %d.\n", p->systemAddress.systemIndex);

	DWORD peerId = p->systemAddress.systemIndex;
	r3d_assert(peerId < impl->maxPeers);
	impl->peerData[peerId] = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	
	impl->callback->OnNetPeerDisconnected(peerId);
	
	RakNet::RakNetStatistics* rns = impl->peer->GetStatistics(p->systemAddress);
	if(rns == NULL)
	  break;
	  
	impl->stat_ += *rns;
	impl->stat_.packetlossTotal = (impl->stat_.packetlossTotal + rns->packetlossTotal) / 2.0f;

        if(dumpStats_ >= 2) {
	  char statbuf[1024 * 8];
	  RakNet::StatisticsToString(rns, statbuf, 0);
	  r3dOutToLog("r3dNetwork Stat: %s\n%s", p->systemAddress.ToString(true, ':'), statbuf);
	}
        break;
      }
      
      default:
      {
        if(data[0] < RAKNET_USER_PACKET)
        {
          // ignore other raknet packets
          break;
        }

	// packet above RAKNET_USER_PACKET is our data packets
	DWORD peerId = p->systemAddress.systemIndex;
	r3d_assert(peerId < impl->maxPeers);

        impl->callback->OnNetData(peerId, (const r3dNetPacketHeader*)data, len);
        break;
      }
    }
    
    impl->peer->DeallocatePacket(p);
  }

  // update last ping time on client
  if(impl && !impl->isHost && IsConnected()) 
  {
    int ping = impl->peer->GetLastPing(impl->peerData[0]);
    if(ping != -1)
      lastPing_ = ping;
  }

  return;
}

static int RakNet_FillBindAddresses(RakNet::SocketDescriptor* binds, int port, DWORD* firstIP)
{
  // get all computer available ip addresses
  char ac[256];
  if(gethostname(ac, sizeof(ac)) == -1)
    r3dError("gethostname failed\n");
  struct hostent* phe = gethostbyname(ac);
  if(phe == NULL) 
    r3dError("gethostbyname %s failed\n", ac);

  int numBinds = 0;
  for(numBinds = 0; numBinds < 128; ++numBinds) {
    if(phe->h_addr_list[numBinds] == NULL)
      break;
      
    in_addr ipaddr;
    memcpy(&ipaddr, phe->h_addr_list[numBinds], sizeof(in_addr));

    if(numBinds == 0)
      *firstIP = ipaddr.s_addr;
    
    char ip[128];
    r3dscpy(ip, inet_ntoa(ipaddr));
    r3dOutToLog("IP: %s\n", ip);

    // add raknet bind
    binds[numBinds] = RakNet::SocketDescriptor(port, ip);
  }

  // add localhost binding as well
  binds[numBinds++] = RakNet::SocketDescriptor(port, "127.0.0.1");
  
  return numBinds;
}


int r3dNetwork::CreateHost(int port, int numPeers)
{
  r3d_assert(impl);
  r3d_assert(impl->peer == NULL);
  r3d_assert(impl->maxPeers == 0);
  r3d_assert(numPeers > 2);

  r3dOutToLog("r3dNetwork: Creating host at port %d\n", port); CLOG_INDENT;

  impl->maxPeers = numPeers;
  impl->peerData = game_new RakNet::SystemAddress[numPeers];
  impl->peer     = RakNet::RakPeerInterface::GetInstance();
  
  // we have to bind to every available address
  // otherwise on machines with multiple IPs default gateway can be different from external IP
  // and send functions will fail.
  RakNet::SocketDescriptor sdescs[128 + 1];
  int numBinds = RakNet_FillBindAddresses(&sdescs[0], port, &firstBindIP_);
  RakNet::StartupResult sr = impl->peer->Startup(numPeers, &sdescs[0], numBinds);
  if(sr != RakNet::RAKNET_STARTED) {
    r3dError("r3dNetwork::CreateHost failed %d\n", sr);
  }

  impl->peer->SetMaximumIncomingConnections(numPeers);
  //impl->peer->SetTimeoutTime(45000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);

  impl->isHost = true;
  impl->stat_.connectionStartTime = RakNet::GetTimeUS();
  
  return 1;
}

int r3dNetwork::CreateClient(int port)
{
  r3d_assert(impl);
  r3d_assert(impl->peer == NULL);

  r3dOutToLog("r3dNetwork: Creating client\n"); CLOG_INDENT;

  impl->maxPeers = 1;
  impl->peerData = game_new RakNet::SystemAddress[1];

  RakNet::SocketDescriptor socketDescriptor;
  socketDescriptor.port = port;
  impl->peer = RakNet::RakPeerInterface::GetInstance();
  RakNet::StartupResult sr = impl->peer->Startup(1, &socketDescriptor, 1);
  if(sr != RakNet::RAKNET_STARTED) {
    r3dError("r3dNetwork::CreateClient failed %d\n", sr);
  }
  
  for(unsigned i=0; i<impl->peer->GetNumberOfAddresses(); i++) {
    r3dOutToLog("IP: %s\n", impl->peer->GetLocalIP(i));
  }

  //impl->peer->SetTimeoutTime(45000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
  
  // enable pings
  impl->peer->SetOccasionalPing(true);
  lastPing_ = -1;

  impl->isHost = false;

  return 1;
}

int r3dNetwork::Connect(const char* hostname, int port)
{
  r3d_assert(impl);
  r3d_assert(impl->peer);
  
#ifndef FINAL_BUILD
  r3dOutToLog("r3dNetwork: connecting to %s:%d\n", hostname, port); CLOG_INDENT;
#else
  r3dOutToLog("r3dNetwork: connecting\n"); CLOG_INDENT;
#endif  

  RakNet::ConnectionAttemptResult car = impl->peer->Connect(
	hostname, 
	port, 
	"", 
	0);
  if(car != RakNet::CONNECTION_ATTEMPT_STARTED)
    r3dError("r3dNetwork::Connect failed %d\n", car);
    
  impl->stat_.connectionStartTime = RakNet::GetTimeUS();

  return 1;
}

bool r3dNetwork::IsHost()
{
  r3d_assert(impl);
  return impl->isHost;
}

bool r3dNetwork::IsConnected()
{
  if(!impl)
    return false;
    
  // host is already connected
  if(impl->isHost) {
    return true;
  }
  
  // if we have host peer - we're connected
  if(impl->peerData[0] != RakNet::UNASSIGNED_SYSTEM_ADDRESS) {
    return true;
  }

  return false;
}

bool r3dNetwork::IsStillConnecting()
{
  return true;
}

DWORD r3dNetwork::GetPeerIp(DWORD peerId)
{
  r3d_assert(impl && impl->peer);
  r3d_assert(peerId < impl->maxPeers);
  return impl->peerData[peerId].address.addr4.sin_addr.s_addr;
}

USHORT r3dNetwork::GetPeerPort(DWORD peerId)
{
  r3d_assert(impl && impl->peer);
  r3d_assert(peerId < impl->maxPeers);
  return impl->peerData[peerId].address.addr4.sin_port ;
}

USHORT r3dNetwork::GetHostPort()
{
  r3d_assert(impl && impl->peer);
  r3d_assert(impl->isHost);
  RakNet::SystemAddress hostAddress;
  hostAddress = impl->peer->GetMyBoundAddress(0);
  return hostAddress.GetPort();
}

void r3dNetwork::DisconnectPeer(DWORD peerId, bool immidiateDisconnect)
{
  r3d_assert(impl && impl->peer);
  r3d_assert(peerId < impl->maxPeers);
  
  //r3dOutToLog("r3dNetwork: disconnecting peer%d\n", peerId);
  impl->peer->CloseConnection(impl->peerData[peerId], !immidiateDisconnect);
}

void r3dNetwork::SendToHost(const r3dNetPacketHeader* data, int dataSize, bool isReliable)
{
  r3d_assert(impl && impl->peer);
  r3d_assert(!impl->isHost);
  r3d_assert(IsConnected());
  
  // make sure our logical packet id is valid
  r3d_assert(data->EventID >= r3dNetwork::FIRST_FREE_PACKET_ID);
  
  /*NOTE from RakNet.
    The sequenced messages will be in order, and the ordered messages will be in order. 
    However, that doesn't mean that all messages will be in order because they are independent sets.
  */  

  //PacketPriority priority = isReliable ? HIGH_PRIORITY : MEDIUM_PRIORITY;
  //PacketReliability reliability = isReliable ? RELIABLE_ORDERED : UNRELIABLE_SEQUENCED;
  impl->peer->Send((const char*)data, dataSize, HIGH_PRIORITY, RELIABLE_ORDERED, 0, impl->peerData[0], false);
   
  //r3dOutToLog("r3dNetwork: send to host, len:%d, %08x, %08x\n", dataSize, ((DWORD*)data)[0], ((DWORD*)data)[1]);

  return;  
}

void r3dNetwork::SendToPeer(const r3dNetPacketHeader* data, int dataSize, DWORD peerId, bool isReliable)
{
  r3d_assert(impl && impl->peer);
  r3d_assert(impl->isHost);
  r3d_assert(peerId < impl->maxPeers);

  // make sure our logical packet id is valid
  r3d_assert(data->EventID >= r3dNetwork::FIRST_FREE_PACKET_ID);

  //PacketPriority priority = isReliable ? HIGH_PRIORITY : MEDIUM_PRIORITY;
  //PacketReliability reliability = isReliable ? RELIABLE_ORDERED : UNRELIABLE_SEQUENCED;
  impl->peer->Send((const char*)data, dataSize, HIGH_PRIORITY, RELIABLE_ORDERED, 0, impl->peerData[peerId], false);
  
  return;
}

void r3dNetwork::SendToAddress(char *data, int dataSize, char *address, unsigned short port)
{
	r3d_assert(impl && impl->peer);

	if ( !IsConnected() )
		return ;

	RakNet::SystemAddress ipAddress ;
	ipAddress.FromStringExplicitPort ( address, port ) ;

	RakNet::RNS2_SendParameters sendParams ;
	sendParams.data = data ;
	sendParams.length = dataSize ;
	sendParams.systemAddress = ipAddress ;
	sendParams.ttl = 0 ;

	DataStructures::List<RakNet::RakNetSocket2* > sockets;
	impl->peer->GetSockets( sockets );
	if (sockets.Size()>0) {
		sockets[0]->Send ( &sendParams, NULL, 0  ) ;
	}


	//RakNet::RakNetSocket2 *socket ;
	//socket = impl->peer->GetSocket(ipAddress) ;
	//if ( socket )
		//socket->Send ( &sendParams, NULL, 0  ) ;

	return;
}
