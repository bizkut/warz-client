#pragma once

#include "r3dNetwork.h"

#define FSNET_SERVER_PORT	34003	// port that friend server will open

namespace NetPacketsFriendsServer
{
#pragma pack(push)
#pragma pack(1)

#define FSNET_VERSION		(0x00000002)
#define FSNET_KEY1		0x13A26322
#define FSNET_KEY2		0x1024F2BA

//
// Server Browser Packet IDs
// 
enum fspktType_e
{
  FSPKT_ValidateConnectingPeer = r3dNetwork::FIRST_FREE_PACKET_ID,
  FSPKT_S2C_FriendServerInfo,
  
  // some test crap for now
  FSPKT_S2C_Ping,
  FSPKT_C2S_Pong,
  
  FSPKT_LAST_PACKET_ID
};

#if FSPKT_LAST_PACKET_ID > 255
  #error Shit happens, more that 255 packet ids
#endif

struct FSPKT_ValidateConnectingPeer_s : public r3dNetPacketMixin<FSPKT_ValidateConnectingPeer>
{
	DWORD		version;
	DWORD		key1;
	DWORD		key2;
};

struct FSPKT_S2C_FriendServerInfo_s : public r3dNetPacketMixin<FSPKT_S2C_FriendServerInfo>
{
	BYTE		serverId;
};

struct FSPKT_S2C_Ping_s : public r3dNetPacketMixin<FSPKT_S2C_Ping>
{
};

struct FSPKT_C2S_Pong_s : public r3dNetPacketMixin<FSPKT_C2S_Pong>
{
};


#pragma pack(pop)

}; // namespace NetPacketsFriendsServer
