#pragma once

#define S_API extern "C"

S_API void (*SteamAPI_Shutdown)() = NULL;
S_API bool (*SteamAPI_IsSteamRunning)() = NULL;
S_API bool (*SteamAPI_Init)() = NULL;

S_API void (*SteamAPI_RunCallbacks)() = NULL;

S_API void (*SteamAPI_RegisterCallback)( class CCallbackBase *pCallback, int iCallback ) = NULL;
S_API void (*SteamAPI_UnregisterCallback)( class CCallbackBase *pCallback ) = NULL;
S_API void (*SteamAPI_RegisterCallResult)( class CCallbackBase *pCallback, SteamAPICall_t hAPICall ) = NULL;
S_API void (*SteamAPI_UnregisterCallResult)( class CCallbackBase *pCallback, SteamAPICall_t hAPICall ) = NULL;

S_API ISteamUser * (*SteamUser)() = NULL;
S_API ISteamFriends * (*SteamFriends)() = NULL;
S_API ISteamUtils * (*SteamUtils)() = NULL;
S_API ISteamMatchmaking * (*SteamMatchmaking)() = NULL;
S_API ISteamUserStats * (*SteamUserStats)() = NULL;
S_API ISteamApps * (*SteamApps)() = NULL;
S_API ISteamNetworking * (*SteamNetworking)() = NULL;
S_API ISteamMatchmakingServers * (*SteamMatchmakingServers)() = NULL;
S_API ISteamRemoteStorage * (*SteamRemoteStorage)() = NULL;
S_API ISteamHTTP * (*SteamHTTP)() = NULL;

// replace void* pointer with function pointer
static void BindProc(HMODULE hDll, void** pFunc, const char* funcName)
{
	void* ptr = ::GetProcAddress(hDll, funcName);
	if(ptr == NULL)
		throw funcName;

	*pFunc = ptr;
}

int SteamAPI_LoadDynamic()
{
	HMODULE hDll = ::LoadLibrary("steam_api.dll");
	if(hDll == NULL)
		return 0;
	
	#define BIND(PROC) BindProc(hDll, (void**)&PROC, #PROC);
	BIND(SteamAPI_Shutdown);
	BIND(SteamAPI_IsSteamRunning);
	BIND(SteamAPI_Init);

	BIND(SteamAPI_RunCallbacks);
	BIND(SteamAPI_RegisterCallback);
	BIND(SteamAPI_UnregisterCallback);
	BIND(SteamAPI_RegisterCallResult);
	BIND(SteamAPI_UnregisterCallResult);

	BIND(SteamUser);
	BIND(SteamFriends);
	BIND(SteamUtils);
	BIND(SteamMatchmaking);
	BIND(SteamUserStats);
	BIND(SteamApps);
	BIND(SteamNetworking);
	BIND(SteamMatchmakingServers);
	BIND(SteamRemoteStorage);
	BIND(SteamHTTP);
	#undef BIND
	
	return 1;
}
