#pragma once

#include "CkByteData.h"

class CSteamHelper
{
  public:
	__int64		steamID;		// 0 if steam is not available
	char		playerName[256];
	CkByteData	authToken;

  public:
	CSteamHelper();
	~CSteamHelper();
	
	void		SyncDllFromResource();
	void		InitSteam();
};

extern	CSteamHelper	gSteam;