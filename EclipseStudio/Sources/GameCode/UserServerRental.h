#pragma once

#include "../../ServerNetPackets/NetPacketsGameBrowser.h"

class CUserServerRental
{
  public:
	struct priceOps_s
	{
	  int		Base_US;
	  int		Base_EU;
	  int		Base_RU;
	  int		Base_SA; // south america

	  int		PVE;
	  int		PVP;
	  int		Slot1;
	  int		Slot2;
	  int		Slot3;
	  int		Slot4;
	  int		Slot5;
	  int		Passworded;
	  int		MonthX2;
	  int		MonthX3;
	  int		MonthX6;
	  int		WeekX1;
	  int		OptNameplates;
	  int		OptCrosshair;
	  int		OptTracers;
	  
	  priceOps_s()
	  {
		memset(this, 0, sizeof(*this));
	  }
	};
	priceOps_s	optGameServer;
	priceOps_s	optStronghold;
	bool		havePrices;
	
	enum 
	{
		HOURLY_RENTID_START = 100,
	};
	
	struct rentParams_s
	{
	  int isGameServer;
	  int mapID;
	  int regionID;
	  int slotID;
	  int rentID; // 0: 1month, 1: 2month, 2: 3month, 3: 6month, 100+: hourly rent
	  char name[64];
	  char password[64];
	  int pveID;
	  int nameplates;
	  int crosshair;
	  int tracers;
	};
	static int	GetSlotID(const GBGameInfo& ginfo);
	
	struct myServer_s
	{
	  GBGameInfo	ginfo;
	  DWORD		AdminKey;
	  char		pwd[16];
	  int		RentHours;
	  int		WorkHours;
	  
	  int		status;	// 0 - unknown, 1 - pending, 2 - offline, 3 - online
	  int		curPlayers;
	};
	std::vector<myServer_s> serverList;
	myServer_s*	GetServerById(DWORD gameServerId);
	
  public:
	CUserServerRental();
	~CUserServerRental();
	
	int		ApiGetServerRentPrices();
	int		ApiRentServer(const rentParams_s& params, int price);
	int		ApiRenewServer(DWORD gameServerId, const rentParams_s& params, int price, int* out_HoursLeft = NULL);
	int		ApiGetServersList();
	int		ApiSetServerParams(DWORD gameServerId, const char* pwd, int flags, uint32_t gameTimeLimit);
};
