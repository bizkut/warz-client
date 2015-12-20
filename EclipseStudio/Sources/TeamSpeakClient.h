#pragma once

class CTeamSpeakClient
{
  public:
	unsigned __int64 scHandlerID;
	
	int		m_myClientID;
	std::string	m_identity;
	int		InitializeIdentity();
	
	std::string	m_captureMode;
	std::string	m_playbackMode;
	std::string	m_defCaptureId;
	std::string	m_defPlaybackId;
	
	struct dev_s
	{
	  std::string	name;
	  std::string	id;
	  bool		isDefault;
	};
	std::vector<dev_s> m_playbackDevices;
	std::vector<dev_s> m_captureDevices;
	
	// logical states
	bool		m_captureOpened;
	bool		m_playbackOpened;

	bool		m_usingPTT;
	bool		m_pttActive;
	
	CRITICAL_SECTION  m_csClients;
	stdext::hash_map<int, int> m_ClientToPlayerIdx;
	int		OnClientAppear(int clientID); // return playerIdx of connected player
	void		OnClientDissapear(bool bMainThread, int clientID);
	
	float		m_nextUpdate;
	
	// events
	enum EEvents
	{
	  EVT_FailedToConnect,
	  EVT_Connected,
	  EVT_ClientDisconnected,
	  EVT_StartTalking,
	  EVT_StopTalking,
	};
	struct evt_s
	{
	  int		code;
	  int		playerIdx;
	};
	std::queue<evt_s> m_events;
	CRITICAL_SECTION  m_csEvents;
	void		AddEvent(int clientID, int code);
	
  public:
	CTeamSpeakClient();
	~CTeamSpeakClient();
	
	int		Startup();
	int		 EnumeratePlaybackDevices(const char* mode);
	int		 EnumerateCaptureDevices(const char* mode);
	
	int		Shutdown();
	
	int		OpenDevices(int playbackIdx, int captureIdx);
	void		StartConnect(const char* host, int port, DWORD voicePwd, DWORD voiceId);
	void		Disconnect();
	
	void		Tick();
	void		 ProcessEvents();
	
	void		SetPushToTalk(bool enable);
	void		SetVolume(float volume);
	void		MuteClient(int clientID);
	void		UnmuteClient(int clientID);
};

extern CTeamSpeakClient	gTeamSpeakClient;
