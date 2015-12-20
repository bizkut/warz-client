#pragma once

#include "CkByteData.h"
#include "steam_api_dyn.h"

class CSteamHelper
{
  public:
	bool		IS_ENABLED;		// globally set var to enable/disable steam support
	bool		inited_;		// true if we have loaded DLL
	
	__int64		steamID;		// 0 if steam is not available
	CkByteData	authToken;
	char		country[16];

  public:
	CSteamHelper();
	~CSteamHelper();
	
	void		InitSteam();		// try to load & init steam dlls
	void		Shutdown();
	
	bool		RecheckUser();		// get currently logged steam user ID
};

extern	CSteamHelper	gSteam;