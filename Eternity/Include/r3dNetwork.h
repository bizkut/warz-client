#ifndef	__R3D_NETWORK_H
#define	__R3D_NETWORK_H

class r3dNetworkImpl;
class r3dNetwork;
struct r3dNetPacketHeader;

enum { r3dInvalidNetPeerID = 0xFFFFFFFF };

class r3dNetCallback
{
  public:
	r3dNetwork*	net_;

  public:
	r3dNetCallback() {
	  net_ = NULL;
	}
virtual ~r3dNetCallback() {}
virtual	void		OnNetPeerConnected(DWORD peerId) = NULL;
virtual	void		OnNetPeerDisconnected(DWORD peerId) = NULL;
virtual	void		OnNetData(DWORD peerId, const r3dNetPacketHeader* packetData, int packetSize) = NULL;
};

class r3dNetwork
{
  private:
	r3dNetworkImpl*	impl;

  public:
	int		dumpStats_;
	int		lastPing_;
	DWORD		firstBindIP_;
	const static int FIRST_FREE_PACKET_ID = 0;
	const static int RAKNET_USER_PACKET   = 134; // must be more that RakNet::ID_USER_PACKET_ENUM

  public:
	r3dNetwork();
	~r3dNetwork();

	int		Initialize(r3dNetCallback* callback, const char* networkName);
	void		Deinitialize();
	void            Update();

	int		CreateHost(int port, int numPeers);
	int		CreateClient(int port);
	int		Connect(const char* hostname, int port);
	bool		IsStillConnecting(); // should be only called after Connect()

	bool		IsHost();
	bool		IsConnected();
	DWORD		GetPeerIp(DWORD peerId);
	USHORT		GetPeerPort(DWORD peerId);
	USHORT		GetHostPort() ;

	void		DisconnectPeer(DWORD peerId, bool immidiateDisconnect = false);
	void		SendBroadcast(const r3dNetPacketHeader* data, int dataSize, bool isReliable = true);
	void		SendToHost(const r3dNetPacketHeader* data, int dataSize, bool isReliable = true);
	void		SendToPeer(const r3dNetPacketHeader* data, int dataSize, DWORD peerId, bool isReliable = true);
	void		SendToAddress(char *data, int dataSize, char *address, unsigned short port);
};

#pragma pack(push)
#pragma pack(1)
struct r3dNetPacketHeader
{
	// this first BYTE will be passed directly to RakNet network library.
	const BYTE	RakNetPacketID;
	const BYTE	EventID;

	r3dNetPacketHeader(int PKT_ID) : RakNetPacketID(r3dNetwork::RAKNET_USER_PACKET), EventID((BYTE)PKT_ID) 
	{
		r3d_assert(PKT_ID >= r3dNetwork::FIRST_FREE_PACKET_ID && PKT_ID < 0xFF);
	}
};

template <int PKT_ID> 
struct r3dNetPacketMixin : public r3dNetPacketHeader
{
	r3dNetPacketMixin() : r3dNetPacketHeader(PKT_ID) {}
};
#pragma pack(pop)


#endif //__R3D_NETWORK_H
