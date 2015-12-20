#include "r3dPCH.h"
#include "r3d.h"

#include "LoginSessionPoller.h"
#include "Backend/WOBackendAPI.h"
#include "r3dDebug.h"

#include "ClientGameLogic.h"
#include "GameCode/UserProfile.h"
#include "GameCode/UserFriends.h"
#include "GameCode/UserClans.h"

CLoginSessionPoller	gLoginSessionPoller;

CLoginSessionPoller::CLoginSessionPoller()
{
	POLL_INTERVAL	= 120; //@ check every N sec
	
	CustomerID_     = 0;
	SessionID_      = 0;
	
	hPollThread     = NULL;
	hWaitTick       = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	isConnected_	= 1; // connected by default
}

CLoginSessionPoller::~CLoginSessionPoller()
{
	CloseHandle(hWaitTick);
    if(hPollThread != NULL)
    {
        TerminateThread(hPollThread, 0);
        CloseHandle(hPollThread);
    }
}

static unsigned int WINAPI PollingThread_Entry(void* param)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	CLoginSessionPoller* This = (CLoginSessionPoller*)param;
	return This->PollingThread();
}

DWORD CLoginSessionPoller::PollingThread()
{
	try
	{
		while(true)
		{
			::WaitForSingleObject(hWaitTick, POLL_INTERVAL * 1000);

			if(!UpdateLoginSession())
				continue;

			if(!isConnected_)
				return 1;
		}
	}
	catch(const char* err)
	{
		r3dOutToLog("!!! CLoginSessionPoller crashed, %s\n", err);
		return 3;
	}
	catch(...)
	{
		r3dOutToLog("!!! CLoginSessionPoller crashed\n");
		return 3;
	}
	return 0;
}

bool CLoginSessionPoller::UpdateLoginSession()
{
	CWOBackendReq req("api_LoginSessionPoller.aspx");
	req.AddSessionInfo(CustomerID_, SessionID_);
	req.Issue();
	
	// result codes from CWOBackendReq 
	switch(req.resultCode_) 
	{
		case 0: 
			isConnected_ = true;
			break;
		case 1:
			// disconnected
			isConnected_ = false;
			r3dOutToLog("UpdateLoginSession: disconnected\n");
			return true;
		default:
			// some shit happens
			return true;
	}

/*
	__int64 gameSessionId = gClientLogic().GetGameSessionID();
	if(gameSessionId != 0)
		return true;

	// if we're not in game, we should have XML with some things
	try
	{
		// update friend data
		pugi::xml_document xmlFile;
		req.ParseXML(xmlFile);

		pugi::xml_node xmlNode = xmlFile.child("ldata");
		gUserProfile.friends->SetCurrentData(xmlNode);
		gUserProfile.clans->SetCurrentData(xmlNode);
	}
	catch(...)
	{
		// failed to parse XML or something else happens
	}
*/	
	
	return true;
}

void CLoginSessionPoller::Start(DWORD CustomerID, DWORD SessionID)
{
	r3d_assert(hPollThread == NULL);
	CustomerID_ = CustomerID;
	SessionID_  = SessionID;
	isConnected_= 1; // connected by default
	
	hPollThread = (HANDLE)_beginthreadex(NULL, 0, PollingThread_Entry, (void*)this, 0, NULL);
    if(hPollThread == NULL)
        r3dError("Failed to begin thread");
}

void CLoginSessionPoller::Stop()
{
    if(hPollThread != NULL)
    {
        TerminateThread(hPollThread, 0); // not safe!!!
        CloseHandle(hPollThread);
        hPollThread = NULL;
    }
}