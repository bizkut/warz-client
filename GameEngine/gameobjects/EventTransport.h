#pragma once

#include "r3dNetwork.h"
#include "GameObjects\GameObj.h"	// for DefaultPacket crap

#pragma pack(push)
#pragma pack(1)

// p2p network IDs will be 2bytes long. so, we have to take extreme care that engine network IDs will be in 0-0xffff range
typedef WORD	gp2pnetid_t;
static const gp2pnetid_t invalidP2pNetID = 0xFFFF;

__forceinline gp2pnetid_t toP2pNetId(DWORD id) {
  r3d_assert(id < 0xFFFF);
  return gp2pnetid_t(id);
}

struct DefaultPacket : r3dNetPacketHeader
{	
	gp2pnetid_t	FromID;

	DefaultPacket(int in_eventID) :
	  r3dNetPacketHeader(in_eventID),
	  FromID(invalidP2pNetID) 
	{
	}
};

template <int PKT_ID> 
struct DefaultPacketMixin : public DefaultPacket
{	
	DefaultPacketMixin() : DefaultPacket(PKT_ID) {}
};

#pragma pack(pop)


//extern	void		p2pSendBroadcast(const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered = true);
extern	void		p2pSendToHost(const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered = true);
//extern	void		p2pSendToPeer(DWORD peerId, const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered = true);
//extern	void		p2pDisconnectPeer(DWORD peerId);
//extern	bool		p2pIsHost();
//extern	void		p2pUpdate();
