#include "r3dPCH.h"
#include "r3d.h"

#include "SteamHelper.h"
#include "resource.h"

// header and functions for dynamically linking steam
#include "steam_api_dyn.h"
#include "steam_api_dyn.cpp"

CSteamHelper	gSteam;

CSteamHelper::CSteamHelper()
{
  IS_ENABLED = false;
  inited_    = false;
  
  steamID    = 0;
}

CSteamHelper::~CSteamHelper()
{
}

void CSteamHelper::InitSteam()
{
  if(!IS_ENABLED)
    return;

  int ld = SteamAPI_LoadDynamic();
  if(ld == 0)
    return;

  bool init = SteamAPI_Init();
  if(!init)
    return;

  r3dOutToLog("Steam is available\n"); CLOG_INDENT;
  inited_ = true;

  RecheckUser();

  return;
}

void CSteamHelper::Shutdown()
{
  if(!inited_)
    return;

  SteamAPI_Shutdown();
  inited_ = false;
}
  
bool CSteamHelper::RecheckUser()
{
  if(!inited_)
    return false;

  bool logged = SteamUser()->BLoggedOn();
  if(!logged)
  {
    steamID = 0;
    return false;
  }
    
  // get id
  steamID    = SteamUser()->GetSteamID().ConvertToUint64();
  r3dscpy(country, SteamUtils()->GetIPCountry());
	
  // get auth token
  char ticketData[2048];
  uint32 sizeTicket = 0;
  SteamUser()->GetAuthSessionTicket(ticketData, sizeof(ticketData), &sizeTicket);
  authToken.clear();
  authToken.append2((BYTE*)ticketData, sizeTicket);

  return true;
}
