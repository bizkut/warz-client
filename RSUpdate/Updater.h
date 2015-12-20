#pragma once

#include "FileSystem/r3dFileSystem.h"
#include "r3dHttpFS.h"

#include "updProgress.h"
#include "WOLoginHelper.h"
#include "WOCreateAccHelper.h"
#include "WOCheckSerialHelper.h"

class updUpdaterData
{
  public:
	DWORD		crc32_;
	long		size_;
	std::string	url_;
	std::string	ver_;

  public:
	updUpdaterData()
	{
	  crc32_ = 0;
	}

	bool		ParseFromXML(char* xml, int xml_size);
};

class updPackageData
{
  public:
	DWORD		version_;
	std::string	base_;
	std::string	inc_;

  public:
	updPackageData()
	{
	  version_ = 0;
	}
	
	bool		ParseFromXML(char* xml, int xml_size);
};

class CUpdater
{
  public:
	std::string	updateUrl_;
	updPackageData	updateData_;
	
	updUpdaterData	newUpdater_;
	volatile bool	updaterVersionOk_;
	
	r3dFileSystem	fslocal_;	// expected copy of filesystem
	r3dFileSystem	fswork_;	// working copy of local filesystem
	r3dHttpFS	fsbase_;
	r3dHttpFS	fsinc_;
	
	enum { NUM_DOWNLOAD_THREADS = 3, };
	
	// update jobs
	struct updjob_s
	{
	  bool isInc;                // updating from incremental archive
	  const r3dFS_FileEntry* fe; // ptr
	  CkByteData data;
	};
	std::list<updjob_s*> updateJobs_;
	std::list<updjob_s*> readyJobs_;
	CRITICAL_SECTION csJobs_;
	int		numUpdatedFiles_;
	
	CRITICAL_SECTION csDataWrite_;

	enum EUpdaterStatus
	{
	  STATUS_Checking,
	  STATUS_SerialCheck,
	  STATUS_NeedRegister,
	  STATUS_NeedLogin,
	  STATUS_TimeExpired,
	  STATUS_Updating,
	};
	EUpdaterStatus	status_;
	
	// progress
	enum EResult
	{
	  RES_UPDATING = 0,
	  RES_PLAY,	// ready to play
	  RES_ERROR,
	  RES_STOPPED,
	};
	EResult		result_;
	bool		updateOk_; // flag that update was successful
	bool		FailUpdate(const char* , ...);

	char		updMsg1_[1024];
	char		updErr1_[1024];
	bool		showProgress_;
	updProgress	prgTotal_;
	updProgress	prgHttp_[NUM_DOWNLOAD_THREADS];

  private:
	friend static unsigned int __stdcall CUpdater_MainThreadEntry(LPVOID in);
	void		MainThreadEntry();
	HANDLE		mainThread_;

	friend static unsigned int __stdcall CUpdater_DownloadThreadEntry(LPVOID in);
	void		DownloadThreadEntry();
	bool		 DlThread_ProcessJob(updjob_s& job, int dlIdx);
	volatile LONG	 dlThreadIdx_;
	volatile LONG	 dlThreadError_; // TRUE if one of download thread failed

	HANDLE		evtJobsReady_;	// event "jobs is ready to download"
	HANDLE		downloadThreads_[NUM_DOWNLOAD_THREADS];

	bool		GetUpdaterData();	// check if update for updater is available
	bool		NeedUpdaterUpdate();
	bool		InstallNewUpdater();

	EUpdaterStatus	GetSteamLinkedStatus();
	
	bool		GetPackageData();	// retreive package update info

	bool		OpenLocalFS();
	bool		BuildUpdateList();
	void		 ClearUpdateFlags(r3dFS_FileList& fl);
	void		 MarkUpdatedFiles(const r3dFS_FileList& fl, bool incremental);
	
	bool		mainExeWasChanged_;
	bool		ProcessUpdates();
	void             FinalizeJob(const updjob_s& job);
	void		 ExtractJobFile(const r3dFS_FileEntry* fe, const CkByteData& cdata);
	void             CheckAllJobsFinished();
	void		 RemoveDeletedFiles();
	void		 AddFirewallException();
	void		 SetWorkArchiveVersion();
	void		 MakeSureExtractedFilesExists();
	void		 WaitReadyToPlay();
	void		 CompressArchive();
	void		 ValidateArchive();
	void		 SetRSUpdateStatus(int status, bool addLog = false);
	
  public:
	HANDLE		loginThread_;
	const char*	loginErrMsg_;

	// login helper stuff
	CLoginHelper	gUserProfile;
	friend static unsigned int __stdcall CUpdater_LoginThreadEntry(LPVOID in);
	void		LoginThreadEntry();
	void		ProcessLoginResults();

	CCreateAccHelper createAccHelper;
	friend static unsigned int __stdcall CUpdater_CreateAccThreadEntry(LPVOID in);
	void		CreateAccThreadEntry();

	CCheckSerialHelper checkSerialHelper;
	friend static unsigned int __stdcall CUpdater_CheckSerialThreadEntry(LPVOID in);
	void		CheckSerialThreadEntry();

  //
  // news section
  //
  public:	
	struct news_s {
	  std::string	name_;
	  std::string	url_;
	  std::string	date_;
	};
	std::vector<news_s> newsData_;
	
	enum { NEWS_UPDATE_PERIOD = 60 };
	CRITICAL_SECTION csNews_;
	volatile DWORD	newsStatus_;
	std::string	serverStatus_;
	std::string	surveyLinkIn_;
	std::string	surveyLinkOut_;

	bool		IsServerOnline() {
	  EnterCriticalSection(&csNews_);
	  bool online = stricmp(serverStatus_.c_str(), "ONLINE") == 0;
	  LeaveCriticalSection(&csNews_);
	  return online;
	}

  private:
	friend static unsigned int __stdcall CUpdater_NewsThreadEntry(LPVOID in);
	void		NewsThreadEntry();
	int		 GetNews();
	void		 ParseNewsNode(pugi::xml_node xmlNode, std::vector<news_s>& news);
	void		 FilterSurveyLinks();
	HANDLE		newsThread_;

  private:	
	// make copy constructor and assignment operator inaccessible
	CUpdater(const CUpdater& rhs);
	CUpdater& operator=(const CUpdater& rhs);

  public:
	CUpdater();
	~CUpdater();
	
	void		Start();
	void		Stop();
	void		 StopThread(HANDLE& h);
	void		RequestStop();

	int		CheckForNewData(); // not in thread for now

	void		DoLogin();
	void		DoCheckSerial();
	void		DoCreateAccount();
	void		DoApplyNewKey(const char* key);
	void		SwitchToUpdater() {
	  status_ = STATUS_Updating;
	}
};
