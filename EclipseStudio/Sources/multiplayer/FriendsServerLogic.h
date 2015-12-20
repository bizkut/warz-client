#pragma once

#include "r3dNetwork.h"
#include "../../ServerNetPackets/NetPacketsFriendsServer.h"
using namespace NetPacketsFriendsServer;

class FriendsServerLogic : public r3dNetCallback
{
  protected:
	CRITICAL_SECTION csNetwork;
	r3dNetwork	g_net;
	bool		isConnected_;
	float		connectStartTime_;

	// r3dNetCallback virtuals
virtual	void		OnNetPeerConnected(DWORD peerId);
virtual	void		OnNetPeerDisconnected(DWORD peerId);
virtual	void		OnNetData(DWORD peerId, const r3dNetPacketHeader* packetData, int packetSize);

  public:
	volatile int	friendsServerId_;
  
	volatile bool	versionChecked_;

  public:
	FriendsServerLogic();
	virtual ~FriendsServerLogic();

	int		ConnectAsync(const char* host, int port);
	void		Disconnect();

	bool		IsConnected() { return isConnected_; }
	
	void		Tick();
};

extern	FriendsServerLogic gFriendsServerLogic;
