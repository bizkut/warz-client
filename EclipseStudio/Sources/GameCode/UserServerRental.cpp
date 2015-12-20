#include "r3dPCH.h"
#include "r3d.h"

#include "UserServerRental.h"
#include "backend/WOBackendAPI.h"
#include "UserProfile.h"

CUserServerRental::CUserServerRental()
{
	havePrices = false;
}

CUserServerRental::~CUserServerRental()
{
}

CUserServerRental::myServer_s* CUserServerRental::GetServerById(DWORD gameServerId)
{
	for(size_t i=0; i<serverList.size(); i++)
	{
		if(serverList[i].ginfo.gameServerId == gameServerId)
			return &serverList[i];
	}
	
	return NULL;
}

int CUserServerRental::GetSlotID(const GBGameInfo& ginfo)
{
	int slotID = -1;
	if(ginfo.IsGameworld())
	{
		switch(ginfo.maxPlayers)
		{
			case 10:  slotID = 0; break;
			case 30:  slotID = 1; break;
			case 50:  slotID = 2; break;
			case 70:  slotID = 3; break;
			case 100: slotID = 4; break;
		}
	}
	else
	{
		switch(ginfo.maxPlayers)
		{
			case 10:  slotID = 0; break;
			case 20:  slotID = 1; break;
			case 30:  slotID = 2; break;
			case 40:  slotID = 3; break;
			case 50:  slotID = 4; break;
		}
	}
	
	return slotID;
}

static void parsePriceOptions(pugi::xml_node xmlItem, CUserServerRental::priceOps_s& opt)
{
	opt.Base_US = xmlItem.attribute("Base_US").as_int();
	opt.Base_EU = xmlItem.attribute("Base_EU").as_int();
	opt.Base_RU = xmlItem.attribute("Base_RU").as_int();
	opt.Base_SA = xmlItem.attribute("Base_SA").as_int();
	opt.PVE = xmlItem.attribute("PVE").as_int();
	opt.PVP = xmlItem.attribute("PVP").as_int();
	opt.Slot1 = xmlItem.attribute("Slot1").as_int();
	opt.Slot2 = xmlItem.attribute("Slot2").as_int();
	opt.Slot3 = xmlItem.attribute("Slot3").as_int();
	opt.Slot4 = xmlItem.attribute("Slot4").as_int();
	opt.Slot5 = xmlItem.attribute("Slot5").as_int();
	opt.Passworded = xmlItem.attribute("Passworded").as_int();
	opt.MonthX2 = xmlItem.attribute("MonthX2").as_int();
	opt.MonthX3 = xmlItem.attribute("MonthX3").as_int();
	opt.MonthX6 = xmlItem.attribute("MonthX6").as_int();
	opt.WeekX1 = xmlItem.attribute("WeekX1").as_int();
	opt.OptNameplates = xmlItem.attribute("OptNameplates").as_int();
	opt.OptCrosshair = xmlItem.attribute("OptCrosshair").as_int();
	opt.OptTracers = xmlItem.attribute("OptTracers").as_int();
}

int CUserServerRental::ApiGetServerRentPrices()
{
	if(havePrices)
		return 0;

	CWOBackendReq req(&gUserProfile, "api_ServersMgr.aspx");
	req.AddParam("func", "prices");
	if(!req.Issue())
	{
		r3dOutToLog("ApiGetServerRentPrices FAILED, code: %d\n", req.resultCode_);
		return req.resultCode_;
	}
	
	pugi::xml_document xmlFile;
	req.ParseXML(xmlFile);
	
	pugi::xml_node xmlPrices = xmlFile.child("srent");
	parsePriceOptions(xmlPrices.child("g"), optGameServer);
	parsePriceOptions(xmlPrices.child("s"), optStronghold);
	
	havePrices = true;
	return 0;
}

int CUserServerRental::ApiRentServer(const rentParams_s& params, int price)
{
	CWOBackendReq req(&gUserProfile, "api_ServersMgr.aspx");
	req.AddParam("func", "rent");
	req.AddParam("isGameServer", params.isGameServer);
	req.AddParam("mapID", params.mapID);
	req.AddParam("regionID", params.regionID);
	req.AddParam("slotID", params.slotID);
	req.AddParam("rentID", params.rentID);
	req.AddParam("name", params.name);
	req.AddParam("password", params.password);
	req.AddParam("pveID", params.pveID);
	req.AddParam("nameplates", params.nameplates);
	req.AddParam("crosshair", params.crosshair);
	req.AddParam("tracers", params.tracers);
	req.AddParam("client_price", price);
	if(!req.Issue())
	{
		r3dOutToLog("ApiRentServer FAILED, code: %d\n", req.resultCode_);
		return req.resultCode_;
	}
	
	int ServerID;
	int nargs = sscanf(req.bodyStr_, "%d", &ServerID);
	r3d_assert(nargs == 1);
	
	gUserProfile.ProfileData.GamePoints -= price;
	
	return 0;
}

int CUserServerRental::ApiRenewServer(DWORD gameServerId, const rentParams_s& params, int price, int* out_HoursLeft)
{
	CWOBackendReq req(&gUserProfile, "api_ServersMgr.aspx");
	req.AddParam("func", "renew");
	req.AddParam("GSID", gameServerId);
	req.AddParam("isGameServer", params.isGameServer);
	req.AddParam("mapID", params.mapID);
	req.AddParam("regionID", params.regionID);
	req.AddParam("slotID", params.slotID);
	req.AddParam("rentID", params.rentID);
	req.AddParam("name", params.name);
	req.AddParam("password", params.password);
	req.AddParam("pveID", params.pveID);
	req.AddParam("nameplates", params.nameplates);
	req.AddParam("crosshair", params.crosshair);
	req.AddParam("tracers", params.tracers);
	req.AddParam("client_price", price);
	if(!req.Issue())
	{
		if(req.resultCode_ == 4)
		{
			// special case - need to wait until you can donate
			int HoursLeft;
			int nargs = sscanf(req.bodyStr_, "%d", &HoursLeft);
			r3d_assert(nargs == 1);
			if(out_HoursLeft)
				*out_HoursLeft = HoursLeft;
			
			r3dOutToLog("ApiRenewServer: need to wait for %d hours before donation\n", req.resultCode_, HoursLeft);
			return req.resultCode_;
		}
	
		r3dOutToLog("ApiRenewServer FAILED, code: %d\n", req.resultCode_);
		return req.resultCode_;
	}
	
	int ServerID;
	int nargs = sscanf(req.bodyStr_, "%d", &ServerID);
	r3d_assert(nargs == 1);
	
	gUserProfile.ProfileData.GamePoints -= price;
	
	return 0;
}

int CUserServerRental::ApiGetServersList()
{
	CWOBackendReq req(&gUserProfile, "api_ServersMgr.aspx");
	req.AddParam("func", "list");
	if(!req.Issue())
	{
		r3dOutToLog("ApiGetServersList FAILED, code: %d\n", req.resultCode_);
		return req.resultCode_;
	}
	
	serverList.clear();
	
	pugi::xml_document xmlFile;
	req.ParseXML(xmlFile);
	
	pugi::xml_node xmlServerList = xmlFile.child("slist");
	pugi::xml_node xmlItem = xmlServerList.first_child();
	while(!xmlItem.empty())
	{
		myServer_s srv;
		srv.ginfo.gameServerId = xmlItem.attribute("ID").as_uint();
		srv.ginfo.region       = xmlItem.attribute("ServerRegion").as_uint();
		//srv.ginfo.type = xmlItem.attribute("ServerType").as_uint();
		srv.ginfo.flags        = xmlItem.attribute("ServerFlags").as_uint();
		srv.ginfo.mapId        = xmlItem.attribute("ServerMap").as_uint();
		srv.ginfo.maxPlayers   = xmlItem.attribute("ServerSlots").as_uint();
		r3dscpy(srv.ginfo.name,  xmlItem.attribute("ServerName").value());
		r3dscpy(srv.pwd,         xmlItem.attribute("ServerPwd").value());
		srv.AdminKey           = xmlItem.attribute("AdminKey").as_uint();
		srv.RentHours          = xmlItem.attribute("RentHours").as_uint();
		srv.WorkHours          = xmlItem.attribute("WorkHours").as_uint();
		//srv.ReservedSlots      = xmlItem.attribute("ReservedSlots").as_uint();
		srv.status             = 0;
		srv.curPlayers         = 0;

		serverList.push_back(srv);

		xmlItem = xmlItem.next_sibling();
	}
	
	return 0;
}

int CUserServerRental::ApiSetServerParams(DWORD gameServerId, const char* pwd, int flags, uint32_t gameTimeLimit)
{
	CWOBackendReq req(&gUserProfile, "api_ServersMgr.aspx");
	req.AddParam("func",   "setparams2");
	req.AddParam("GSID",   gameServerId);
	req.AddParam("pwd",    pwd);
	req.AddParam("flags",  flags);
	req.AddParam("timeLimit", gameTimeLimit);
	if(!req.Issue())
	{
		r3dOutToLog("ApiSetServerParams FAILED, code: %d\n", req.resultCode_);
		return req.resultCode_;
	}
	
	return 0;
}