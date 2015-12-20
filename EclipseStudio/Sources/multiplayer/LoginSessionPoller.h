#pragma once

// class for polling login server to keep our session alive
class CLoginSessionPoller
{
  public:
	volatile DWORD	isConnected_;
	
  private:
	int		POLL_INTERVAL;	// polling interval, in seconds

	DWORD		CustomerID_;
	DWORD		SessionID_;
	
	HANDLE		hPollThread;
	friend static unsigned int WINAPI PollingThread_Entry(void* param);
	DWORD		PollingThread();
	bool		 UpdateLoginSession();
	
	HANDLE		hWaitTick;
  
  public:
	CLoginSessionPoller();
	~CLoginSessionPoller();
	
	void		Start(DWORD CustomerID, DWORD SessionID);
	void		Stop();

	bool		IsConnected() const {
		return isConnected_ ? true : false;
	}
	
	void		ForceTick() {
		SetEvent(hWaitTick);
	}
};

// static because it should terminate in game end, if case we forgot to do it
extern	CLoginSessionPoller	gLoginSessionPoller;
