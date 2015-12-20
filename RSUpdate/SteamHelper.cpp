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
  steamID       = 0;
  playerName[0] = 0;
}

CSteamHelper::~CSteamHelper()
{
}

void CSteamHelper::SyncDllFromResource()
{
  extern bool GetResource(const char* name, const char* type, CkByteData& out_data);
  extern bool r3dGetFileCrc32(const char* fname, DWORD* out_crc32, DWORD* out_size);

  CkByteData data;
  if(!GetResource(MAKEINTRESOURCE(IDR_DATA_STEAM_API), "DATA", data)) {
    MessageBox(NULL, "No steam_api.dll resource", "Warning", MB_OK | MB_ICONERROR);
    return;
  }
    
  const char* dllName = "steam_api.dll";
    
  // compare disk and memory version of steam dlls
  DWORD dllCrc32 = 0;
  DWORD dllSize  = 0;
  r3dGetFileCrc32(dllName, &dllCrc32, &dllSize);
  DWORD memCrc32 = r3dCRC32(data.getBytes(), data.getSize());
  if(dllCrc32 == memCrc32) 
    return;
    
  // replace disk version
  ::SetFileAttributes(dllName, FILE_ATTRIBUTE_NORMAL);
  FILE* f = fopen(dllName, "wb");
  if(f == NULL) {
    MessageBox(NULL, "Can not update steam_api.dll", "Warning", MB_OK | MB_ICONERROR);
    return;
  }

  fwrite(data.getBytes(), 1, data.getSize(), f);
  fclose(f);
  
  return;
}

void CSteamHelper::InitSteam()
{
  int ld = SteamAPI_LoadDynamic();
  if(ld == 0)
  {
    MessageBox(NULL, "Steam DLL loading error\nSteam functionality won't work", "Steam Connection", MB_OK | MB_ICONSTOP);
    return;
  }

  bool init = SteamAPI_Init();
  if(!init)
  {
    MessageBox(NULL, "Unable to init STEAM API\nSteam functionality won't work", "Steam Connection", MB_OK | MB_ICONSTOP);
    return;
  }

  bool logged = SteamUser()->BLoggedOn();
  if(!logged)
  {
    MessageBox(NULL, "You must be logged on to Steam to play/register The War Z game", "Steam Connection", MB_OK | MB_ICONSTOP);
    return;
  }
    
  r3dOutToLog("Steam is available\n"); CLOG_INDENT;
    
  // get id and player name
  steamID    = SteamUser()->GetSteamID().ConvertToUint64();
  r3dscpy(playerName, SteamFriends()->GetPersonaName());
	
  // get auth token
  char ticketData[2048];
  uint32 sizeTicket = 0;
  SteamUser()->GetAuthSessionTicket(ticketData, sizeof(ticketData), &sizeTicket);
  authToken.append2((BYTE*)ticketData, sizeTicket);

  return;
}
