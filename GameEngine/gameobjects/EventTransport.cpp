#include "r3dPCH.h"
#include "r3d.h"
#include "r3dNetwork.h"

#include "EventTransport.h"

#include "GameObjects\ObjManag.h"

/*
	r3dNetwork	p2pNet;

void p2pUpdate()
{
  p2pNet.Update();
}

bool p2pIsHost()
{
  return p2pNet.IsHost();
}

static void preparePacket(const GameObject* from, DefaultPacket* packetData)
{
  r3d_assert(packetData);
  r3d_assert(packetData->EventID >= 0);
  
  if(from) {
    r3d_assert(from->NetworkID);
    //r3d_assert(from->NetworkLocal);

    packetData->FromID = toP2pNetId(from->NetworkID);
  } else {
    packetData->FromID = 0; // world event
  }
  
  return;
}
	
void p2pSendBroadcast(const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
  preparePacket(from, packetData);
  p2pNet.SendBroadcast(packetData, packetSize, guaranteedAndOrdered);
}

void p2pSendToHost(const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
  preparePacket(from, packetData);
  p2pNet.SendToHost(packetData, packetSize, guaranteedAndOrdered);
}

void p2pSendToPeer(DWORD peerId, const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
  preparePacket(from, packetData);
  p2pNet.SendToPeer(packetData, packetSize, peerId, guaranteedAndOrdered);
}

void p2pDisconnectPeer(DWORD peerId)
{
  p2pNet.DisconnectPeer(peerId);
}
*/

