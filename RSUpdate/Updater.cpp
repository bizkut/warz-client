#include "r3dPCH.h"
#include "r3d.h"
#include "FileSystem/r3dFSStructs.h"
#include "FileSystem/r3dFSCompress.h"
#include "shellapi.h"

#include "..\..\External\pugiXML\src\pugixml.hpp"

#include "Updater.h"
#include "UPDATER_CONFIG.h"
#include "HttpDownload.h"
#include "SelfUpdateEXE.h"
#include "FirewallUtil.h"
#include "SteamHelper.h"
#include "WOBackendAPI.h"
#include "HWInfoPost.h"

static const char* g_errCheckUpdater    = "Failed to connect to update server.\nCheck that you have a working internet connection and you are not behind a proxy or firewall.";
static const char* g_errDownloadUpdater = "There was a problem downloading new updater\nCheck that you have a working internet connection and you are not behind a proxy or firewall.";
static const char* g_errFailedDownload  = "There was a problem downloading from CDN server\nCheck that you have a working internet connection and you are not behind a proxy or firewall.";

bool updUpdaterData::ParseFromXML(char* xml_data, int xml_size)
{
  r3d_assert(xml_data);
	
  pugi::xml_document xmlDoc;
  pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(xml_data, xml_size);
  if(!parseResult) {
    r3dOutToLog("Failed to parse XML, error: %s", parseResult.description());
    return false;
  }
  pugi::xml_node root = xmlDoc.child("p");
  pugi::xml_node upd  = root.child("u");
  
  crc32_   = upd.attribute("crc").as_uint();
  size_    = upd.attribute("size").as_uint();
  url_     = upd.attribute("url").value();
  ver_     = upd.attribute("ver").value();
  
  if(crc32_ == 0) {
    r3dOutToLog("updUpdaterData: invalid version\n");
    return false;
  }

  if(url_.length() < 1) {
    r3dOutToLog("updUpdaterData: invalid base\n");
    return false;
  }
    
  return true;
}

bool updPackageData::ParseFromXML(char* xml_data, int xml_size)
{
  r3d_assert(xml_data);
	
  pugi::xml_document xmlDoc;
  pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(xml_data, xml_size);
  if(!parseResult) {
    r3dOutToLog("Failed to parse XML, error: %s", parseResult.description());
    return false;
  }
  pugi::xml_node root = xmlDoc.child("p");
  pugi::xml_node upd  = root.child("d");
  version_ = upd.attribute("ver").as_uint();
  base_    = upd.attribute("base").value();
  inc_     = upd.attribute("inc").value();
  
  if(version_ == 0) {
    r3dOutToLog("updPackageData: invalid version\n");
    return false;
  }

  if(base_.length() < 1) {
    r3dOutToLog("updPackageData: invalid base\n");
    return false;
  }
    
  return true;
}

CUpdater::CUpdater()
{
  result_     = RES_UPDATING;
  status_     = STATUS_Checking;
  updateOk_   = false;
  
  updMsg1_[0] = 0;
  updErr1_[0] = 0;
  showProgress_ = true;
  mainExeWasChanged_ = false;
  
  updaterVersionOk_ = false;
  
  numUpdatedFiles_ = 0;

  mainThread_   = NULL;
  newsThread_   = NULL;
  newsStatus_   = 0;
  serverStatus_ = "";
  surveyLinkIn_ = "";
  surveyLinkOut_= "";

  loginThread_    = NULL;
  loginErrMsg_    = "";
  
  InitializeCriticalSection(&csDataWrite_);
  InitializeCriticalSection(&csJobs_);
  InitializeCriticalSection(&csNews_);
  
  for(int i=0; i<NUM_DOWNLOAD_THREADS; i++) {
    downloadThreads_[i] = NULL;
  }
  
  dlThreadIdx_   = 0;
  dlThreadError_ = 0;
  evtJobsReady_  = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CUpdater::~CUpdater()
{
  r3d_assert(mainThread_ == NULL);
  r3d_assert(newsThread_ == NULL);
}

bool CUpdater::FailUpdate(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(updErr1_, sizeof(updErr1_), fmt, ap);
  va_end(ap);
  
  r3dOutToLog("FailUpdate: %s\n", updErr1_);
  
  throw RES_ERROR;
  return false;
}



bool CUpdater::GetUpdaterData()
{
  r3dOutToLog("Checking for new updater\n");
  
  CkByteData data;
  bool downloadOk = false;
  for(int retry = 0; retry < 10; retry++)
  {
    if(retry == 0) {
      sprintf(updMsg1_, "Checking for new updater");
    } else {
      sprintf(updMsg1_, "Checking for new updater (try %d)", retry + 1);
    }

    data.clear();
    HttpDownload http;
    if(http.Get(UPDATE_UPDATER_URL, data)) {
      downloadOk = true;
      break;
    }
    
    ::Sleep(3000);
  }
  
  if(!downloadOk) {
    return FailUpdate(g_errCheckUpdater);
  }
  
  if(!newUpdater_.ParseFromXML((char*)data.getBytes(), data.getSize())) {
    return FailUpdate("Failed to parse new updater information");
  }
  
  sprintf(updMsg1_, "");
  r3dOutToLog("Checking for new updater - finished\n");
  return true;
}

bool CUpdater::NeedUpdaterUpdate()
{
  r3d_assert(newUpdater_.url_.length() > 0);
  if(strcmp(UPDATER_VERSION, newUpdater_.ver_.c_str()) == 0)
    return false;

  return true;    
}

bool CUpdater::InstallNewUpdater()
{
  r3dOutToLog("Downloading updater\n");

  r3d_assert(newUpdater_.url_.length() > 0);

  CkByteData data;
  bool downloadOk = false;
  for(int retry = 0; retry < 10; retry++)
  {
    if(retry == 0) {
      sprintf(updMsg1_, "Downloading updater %s", newUpdater_.ver_.c_str());
    } else {
      sprintf(updMsg1_, "Downloading updater %s (try %d)", newUpdater_.ver_.c_str(), retry + 1);
    }

    prgTotal_.set(newUpdater_.size_);

    data.clear();
    HttpDownload http(&prgTotal_);
    if(http.Get(newUpdater_.url_.c_str(), data)) {
      downloadOk = true;
      break;
    }

    ::Sleep(3000);
  }

  if(!downloadOk) {
    return FailUpdate(g_errDownloadUpdater);
  }
  
  DWORD crc32 = r3dCRC32(data.getBytes(), data.getSize());
  if(crc32 != newUpdater_.crc32_) {
    return FailUpdate("Failed to download new updater - CRC mismatch");
  }
  
  sprintf(updMsg1_, "Restarting updater");
  ::Sleep(1000);
  
  selfUpd_StartUpdate(data.getBytes(), data.getSize());

  // at this point program should restart itself with new .exe
  r3dError("SHOULD NOT GET HERE");
  return true;
}

CUpdater::EUpdaterStatus CUpdater::GetSteamLinkedStatus()
{
  // if steam is disabled in updater, skip it
  if(!UPDATER_STEAM_ENABLED)
    return STATUS_NeedLogin;

  if(gSteam.steamID == 0) 
    return STATUS_NeedLogin;
    
  sprintf(updMsg1_, "Checking for linked steam account");
  if(gUserProfile.CheckSteamLogin() == false) {
    // failed to check steam acc
    return STATUS_NeedLogin;
  }
  
  ProcessLoginResults();

  // if login was successful
  if(status_ == STATUS_Updating)
    return status_;
    
  return STATUS_NeedLogin;
}

bool CUpdater::GetPackageData()
{
  r3dOutToLog("Checking for game update\n");
  sprintf(updMsg1_, "Checking for game update");

  CkByteData   data;
  HttpDownload http(&prgTotal_);
  if(!http.Get(UPDATE_DATA_URL, data)) {
    return FailUpdate("Failed to get current game build");
  }
  
  if(!updateData_.ParseFromXML((char*)data.getBytes(), data.getSize())) {
    return FailUpdate("Failed to parse current game build");
  }
  
  sprintf(updMsg1_, "");
  r3dOutToLog("Checking for game update - finished\n");
 return true;
}

bool CUpdater::OpenLocalFS()
{
	r3dOutToLog("Opening game data\n");
  sprintf(updMsg1_, "Opening game data");

  r3d_assert(fslocal_.GetNumFiles() == 0);
  r3d_assert(fswork_.GetNumFiles() == 0);

  fslocal_.OpenArchive(BASE_RESOURSE_NAME);
  fswork_.OpenArchive(BASE_RESOURSE_NAME);
  
  r3dOutToLog("Opening game data - finished\n");
  return true;
}

void CUpdater::MarkUpdatedFiles(const r3dFS_FileList& fl, bool incremental)
{
  if(fl.files_.size() == 0)
    return;

  for(size_t i = 0; i < fl.files_.size(); i++)
  {
    const r3dFS_FileEntry& fe = *fl.files_[i];

    // mark if we have this file in work set (for future deleting unused files)
    r3dFS_FileEntry* fe_work  = const_cast<r3dFS_FileEntry*>(fswork_.GetFileEntry(fe.name));
    if(fe_work)
      fe_work->flags |= r3dFS_FileEntry::FLAG_UPDATE_EXIST;
    
    // flag FLAG_UPDATE_EXIST used in local to indicate that we already updated this file
    r3dFS_FileEntry* fe_local = const_cast<r3dFS_FileEntry*>(fslocal_.GetFileEntry(fe.name));
    if(fe_local && (fe_local->flags & r3dFS_FileEntry::FLAG_UPDATE_EXIST) != 0) {
      // this file already scheduled for update
      continue;
    }
    
    bool isNew = false;
    if(fe_local == NULL) 
    {
      fe_local = fslocal_.fl_.AddNew(fe.name);
      isNew    = true;
    }

    // mark that we have this file    
    fe_local->flags |= r3dFS_FileEntry::FLAG_UPDATE_EXIST;

    if(isNew || fe_local->crc32 != fe.crc32) 
    {
      updjob_s* job = new updjob_s();
      job->isInc = incremental;
      job->fe    = &fe;
      updateJobs_.push_back(job);
      
      prgTotal_.total += fe.csize;
    }
  }
  
  return;
}

void CUpdater::ClearUpdateFlags(r3dFS_FileList& fl)
{
  for(size_t i = 0; i < fl.files_.size(); i++) 
  {
    r3dFS_FileEntry& fe = *fl.files_[i];
    fe.flags &= ~r3dFS_FileEntry::FLAG_UPDATE_EXIST;
  }
}

bool CUpdater::BuildUpdateList()
{
  r3dOutToLog("Building update list\n");
  char bldDate[64];
  sprintf(updMsg1_, "Loading update %s", r3dFS_FileList::GetBuildDate(updateData_.version_, bldDate));

  // load remote fss
  const char* baseUrl = updateData_.base_.c_str();
  r3d_assert(*baseUrl);
  fsbase_.OpenFromWeb(baseUrl, &prgHttp_[0]);
  if(!fsbase_.IsValid()) {
    return FailUpdate(g_errFailedDownload);
  }

  const char* incrementalUrl = updateData_.inc_.c_str();
  if(*incrementalUrl) {
    fsinc_.OpenFromWeb(incrementalUrl, &prgHttp_[0]);
    if(!fsinc_.IsValid()) {
      return FailUpdate(g_errFailedDownload);
    }
  }

  sprintf(updMsg1_, "Building update list");

  // clear update flags in local sets
  ClearUpdateFlags(fslocal_.fl_);
  ClearUpdateFlags(fswork_.fl_);

  prgTotal_.set(0);
  r3d_assert(updateJobs_.size() == 0);
  MarkUpdatedFiles(fsinc_, true);
  MarkUpdatedFiles(fsbase_, false);
  
  r3dOutToLog("need to update %d files\n", updateJobs_.size());
  r3dOutToLog("Building update list - finished\n");
  return true;
}

void CUpdater::ExtractJobFile(const r3dFS_FileEntry* fe, const CkByteData& cdata)
{
  //sprintf(updMsg1_, "Extracting %s\n", fe->name);

  // check if file exist and have same crc
  DWORD size;
  DWORD crc32;
  if(r3dGetFileCrc32(fe->name, &crc32, &size)) {
    if(crc32 == fe->crc32) {
      return;
    }
  }
  
  ::DeleteFile(fe->name);
  ::MakeSureDirectoryPathExists(fe->name);
  FILE* f = fopen(fe->name, "wb");
  if(f == NULL) {
    char msg[1024];
    sprintf(msg, "Can't replace file %s\nPlease delete this file manually", fe->name);
    FailUpdate(msg);
    r3dError(msg);
  }

  BYTE* out_data;
  DWORD out_size;
  fswork_.UncompressFileData(fe, cdata.getBytes(), cdata.getSize(), &out_data, &out_size);
  r3d_assert(fe->size == out_size);

  ::EnterCriticalSection(&csDataWrite_);
  fwrite(out_data, out_size, 1, f);
  fclose(f);
  ::LeaveCriticalSection(&csDataWrite_);
  
  if(stricmp(fe->name, GAME_EXE_NAME) == 0) {
    mainExeWasChanged_ = true;
    AddFirewallException();
  }
  
  delete[] out_data;
  return;
}

void CUpdater::FinalizeJob(const updjob_s& job)
{
  // get pointer to working
  r3dFS_FileEntry* fe = const_cast<r3dFS_FileEntry*>(fswork_.fl_.Find(job.fe->name));
  if(fe == NULL) {
    fe = fswork_.fl_.AddNew(job.fe->name);
  }
  *fe = *job.fe;
  fe->flags |= r3dFS_FileEntry::FLAG_UPDATE_EXIST;
  
  // place it to archive
  int   volume;
  DWORD offset;
  fswork_.AllocSpaceForFile(fe->csize, &volume, &offset);
  fe->volume = (BYTE)volume;
  fe->offset = offset;

  fswork_.WriteFileData(*fe, job.data.getBytes(), job.data.getSize());

  prgTotal_.cur += fe->csize;

  if(fe->flags & r3dFS_FileEntry::FLAG_EXTRACT) {
    ExtractJobFile(fe, job.data);
  }

  return;
}

void CUpdater::RemoveDeletedFiles()
{
  int removed = 0;
  
  // remove files without updated flag. meaning that this file was deleted
  std::vector<r3dFS_FileEntry> todelete; // not pointers - need to have copy of file entry
  todelete.reserve(1024);
  
  for(size_t i=0; i<fswork_.fl_.files_.size(); i++) {
    const r3dFS_FileEntry* fe = fswork_.fl_.files_[i];
    if((fe->flags & r3dFS_FileEntry::FLAG_UPDATE_EXIST) == 0) {
      todelete.push_back(*fe);
      //r3dOutToLog("%s is no longer used\n", fe->name);
      removed++;
    }
  }
  
  for(size_t i=0; i<todelete.size(); i++) {
    const r3dFS_FileEntry* fe = &todelete[i];
    fswork_.fl_.Remove(fe->name);
    if(fe->flags & r3dFS_FileEntry::FLAG_EXTRACT) {
      ::DeleteFile(fe->name);
    }
  }
  
  r3dOutToLog("removed %d files\n", removed);
  return;
}

void CUpdater::AddFirewallException()
{
  // make a full qualified exe name
  char curDir[MAX_PATH];
  ::GetCurrentDirectory(sizeof(curDir), curDir);
  char fullExe[MAX_PATH];
  sprintf(fullExe, "%s\\%s", curDir, GAME_EXE_NAME);

  char desc[1024];
  sprintf(desc, "Allow network traffic for %s", GAME_TITLE);
  
  CFirewallUtil::AddApp(fullExe, GAME_TITLE, desc);
}

void CUpdater::CheckAllJobsFinished()
{
  ::Sleep(100);
  r3d_assert(updateJobs_.size() == 0);
  r3d_assert(readyJobs_.size() == 0);
  
  for(int i=0; i<NUM_DOWNLOAD_THREADS; i++) {
    DWORD rr = ::WaitForSingleObject(downloadThreads_[i], 100);
    r3d_assert(rr == WAIT_OBJECT_0);
  }
  
  return;
}

int CUpdater::CheckForNewData()
{
  try
  {
    updErr1_[0] = 0;
  
    // get current update data from server
    GetPackageData();
    
    // check if we need to update
    if(fswork_.GetBuildVersion() != updateData_.version_)
      return true;

    return false;  
  }
  catch(...)
  {
    MessageBox(NULL, updErr1_, "Error", MB_OK | MB_ICONERROR);
    return true;
  }
}

bool CUpdater::ProcessUpdates()
{
	r3dOutToLog("Processing updates\n");
  r3d_assert(readyJobs_.size() == 0);
  
  if(updateJobs_.size() == 0) {
	  r3dOutToLog("Update not needed\n");
    sprintf(updMsg1_, "Update not needed");
    SetWorkArchiveVersion();
    return true;
  }

  const float listWriteDelay = 10.0f; // write every 10 sec
  float nextListWrite = r3dGetTime();

  char bldDate[128];
  sprintf(updMsg1_, "Updating to build '%s'", r3dFS_FileList::GetBuildDate(updateData_.version_, bldDate));  
  r3dOutToLog("%s\n", updMsg1_);
  
  const int neededFiles = (int)updateJobs_.size();
  ::SetEvent(evtJobsReady_);
  r3dOutToLog("neededFiles=%d\n", neededFiles);

  while(true)
  {
    ::Sleep(1);
    
    // see if update failed
    if(dlThreadError_) {
      return FailUpdate("Failed to download update files, please try again later");
    }
    
    // see if we have finished jobs
    if(readyJobs_.size() == 0)
      continue;

    // write job to file  
    {
      ::EnterCriticalSection(&csJobs_);
      const updjob_s* job = readyJobs_.front();
      readyJobs_.pop_front();
      ::LeaveCriticalSection(&csJobs_);
      
      //r3dOutToLog("finalizing: %s (%d of %d)\n", job->fe->name, numUpdatedFiles_, neededFiles);
      FinalizeJob(*job);
      
      delete job;
      numUpdatedFiles_++;
      
      if(neededFiles == numUpdatedFiles_)
      {
        CheckAllJobsFinished();
        break;
      }
    }

    // update message
    float mbLeft = ((float)prgTotal_.total - prgTotal_.cur) / 1024.0f / 1024.0f;
    sprintf(updMsg1_, "Updating to build '%s', %.0f mb left", r3dFS_FileList::GetBuildDate(updateData_.version_, bldDate), mbLeft);  
    
    // update file list every few sec
    if(r3dGetTime() > nextListWrite) 
    {
      nextListWrite = r3dGetTime() + listWriteDelay;

      ::EnterCriticalSection(&csDataWrite_);
      fswork_.CloseVolumes();
      fswork_.WriteFileList();
      ::LeaveCriticalSection(&csDataWrite_);
    }
  }

  RemoveDeletedFiles();
  
  // finally set our build version to indicate that we're done.
  SetWorkArchiveVersion();

  sprintf(updMsg1_, "Update finished");
  r3dOutToLog("Processing updates - finished\n");
  return true;
}

void CUpdater::SetWorkArchiveVersion()
{
  ::EnterCriticalSection(&csDataWrite_);
  fswork_.fl_.buildVersion_ = updateData_.version_;
  fswork_.CloseVolumes();
  fswork_.WriteFileList();
  ::LeaveCriticalSection(&csDataWrite_);
}

void CUpdater::MakeSureExtractedFilesExists()
{
	r3dOutToLog("Making sure extracted files exists\n");
  fswork_.OpenVolumesForRead();

  for(size_t i=0; i<fswork_.fl_.files_.size(); i++) 
  {
    const r3dFS_FileEntry* fe = fswork_.fl_.files_[i];
    if((fe->flags & r3dFS_FileEntry::FLAG_EXTRACT) == 0) 
      continue;
  
    // check if file exist and have same crc
    DWORD size;
    DWORD crc32;
    if(r3dGetFileCrc32(fe->name, &crc32, &size)) {
      if(crc32 == fe->crc32 && size == fe->size)
        continue;
    }
    
    r3dOutToLog("restoring %s\n", fe->name);
  
    ::DeleteFile(fe->name);
    ::MakeSureDirectoryPathExists(fe->name);

    if(!fswork_.ExtractFile(fe, ".\\")) {
      char msg[1024];
      sprintf(msg, "Can't extract file %s\nPlease delete this file manually", fe->name);
      FailUpdate(msg);
      r3dError(msg);
    }
  }
  
  fswork_.CloseVolumes();
  
  r3dOutToLog("Making sure extracted files exists - finished\n");
  return;
}

bool CUpdater::DlThread_ProcessJob(updjob_s& job, int dlIdx)
{
  char dbgmsg[256];

  const int MAX_RETRY = 30;
  int curRetry = 0;
  
  // download updated file, loop for N retries
  do 
  {
    sprintf(dbgmsg, "downloading %s, %d bytes\n", job.fe->name, job.fe->csize);
    //OutputDebugStringA(dbgmsg);

    job.data.clear();
    HttpDownload http(&prgHttp_[dlIdx]);
  
    bool       res;
    if(job.isInc)
      res = fsinc_.GetFile(http, *job.fe, job.data);
    else
      res = fsbase_.GetFile(http, *job.fe, job.data);
    
    if(res) 
      break;
    
    ::Sleep(200);
    if(++curRetry > MAX_RETRY) {
      return false;
    } 
  } while(true);
  
  return true;
}

void CUpdater::DownloadThreadEntry()
{
  // acquire this thread download index
  int dlIdx = ::InterlockedIncrement(&dlThreadIdx_) - 1;
  r3d_assert(dlIdx >= 0 && dlIdx < NUM_DOWNLOAD_THREADS);

  try
  {
    if(::WaitForSingleObject(evtJobsReady_, INFINITE) != WAIT_OBJECT_0) {
      r3dOutToLog("Download Thread %d terminated\n", dlIdx);
      return;
    }
    r3dOutToLog("Download Thread %d Started\n", dlIdx);
    
    while(true)
    {
      ::Sleep(1);
      
      // get new job
      ::EnterCriticalSection(&csJobs_);
      if(updateJobs_.size() == 0) {
        r3dOutToLog("Download Thread %d finished\n", dlIdx);
        ::LeaveCriticalSection(&csJobs_);
        return;
      }

      updjob_s* job = updateJobs_.front();
      updateJobs_.pop_front();
      //r3dOutToLog("Download Thread %d got %s\n", dlIdx, job->fe->name);
      ::LeaveCriticalSection(&csJobs_);
    
      if(!DlThread_ProcessJob(*job, dlIdx))
      {
        if(!dlThreadError_) {
          //TODO: make a full error explanation message
          dlThreadError_++;
        }
        r3dOutToLog("Download Thread %d stopped with error\n", dlIdx);
        return;
      }
        
      // move job to finished
      ::EnterCriticalSection(&csJobs_);
      readyJobs_.push_back(job);
      ::LeaveCriticalSection(&csJobs_);
    }
  }
  catch(...)
  {
    MessageBox(NULL, "something wrong happened in download thread", "", MB_OK);
  }
}

void CUpdater::ValidateArchive()
{
	r3dOutToLog("Validating game data\n");
  sprintf(updMsg1_, "Validating game data");

  if(!fswork_.ValidateArchive(false, prgTotal_.total, prgTotal_.cur))
  {
    // remove archive files and fail update..
    FailUpdate("Game data is damaged, please restart launcher to repair damaged files");
    r3d_assert(0);
  }
  r3dOutToLog("Validating game data - finished\n");
}

void CUpdater::SetRSUpdateStatus(int status, bool addLog /* = false */)
{
/* //@DISABLED, no support on WarZ backend yet
	// must be logged in
	if(gUserProfile.CustomerID == 0)
		return;
	
	CkString r3dLog;
	if(addLog) {
		r3dLog.loadFile("r3dLog.txt", "ansi");
	}

	CWOBackendReq req("api_SetRSUpdateStatus.aspx");
	req.AddSessionInfo(gUserProfile.CustomerID, 0);
	req.SetTimeouts(10, 5);
	req.AddParam("Status", status);
	req.AddParam("Perc", int(prgTotal_.getCoef() * 100));
	req.AddParam("r3dLog", r3dLog);
	
	req.Issue();
*/	
}

void CUpdater::CompressArchive()
{
  r3dOutToLog("Optimizing game data\n");

  sprintf(updMsg1_, "Checking game data");
  float wasted = fswork_.GetArchiveWastedPerc();
  sprintf(updMsg1_, "");
  
  // optimize data if more that 30% is wasted, keep Steam in mind to avoid build redownloading
  if(wasted < 0.3f)
    return;

  sprintf(updMsg1_, "Optimizing game data");
  if(!fswork_.RebuildArchive(prgTotal_.total, prgTotal_.cur))
  {
    // remove archive files and fail update..
    FailUpdate("Game data is damaged, please restart launcher to repair damaged files");
    r3d_assert(0);
  }

  r3dOutToLog("Optimizing game data - finished\n");
}

void CUpdater::WaitReadyToPlay()
{
  r3dOutToLog("WaitReadyToPlay!\n");
  MakeSureExtractedFilesExists();

  // show progress bar at 100%
  showProgress_ = true;
  prgTotal_.cur   = 100;
  prgTotal_.total = 100;

  // set we're done.
  result_   = RES_PLAY;
  updateOk_ = true;
  
  // infinite loop, updating server status from news thread
  while(true)
  {
    ::Sleep(1 * 1000);
    if(IsServerOnline()) {
      sprintf(updMsg1_, "Press Play to launch game");
    } else {
      sprintf(updMsg1_, "Update success, waiting for servers");
    }
  }
  
  return;
}

void CUpdater::MainThreadEntry()
{
  try
  {
    // 1st step - check for updater update
    r3d_assert(status_ == STATUS_Checking);
    if(UPDATER_UPDATER_ENABLED)
    {
      // check for new version of updater
      GetUpdaterData();
      if(NeedUpdaterUpdate()) 
      {
        InstallNewUpdater();
        // at this point program should be already terminated
        return;
      }
    }
    updaterVersionOk_ = true;

    // 2nd step - check if we have steam account.
    status_ = GetSteamLinkedStatus();
    sprintf(updMsg1_, "");
    
    // wait until login is finished
    while(true) {
      if(status_ == STATUS_Updating)
        break;
      
      Sleep(50);
    }
    
    // pass login data to hardware info poster and start it
    gHwInfoPoster.hw.CustomerID = gUserProfile.CustomerID;
    gHwInfoPoster.hw.SessionID  = gUserProfile.SessionID;
    gHwInfoPoster.Start();
  
    // get current update data from server
    GetPackageData();
    
    // check if we need to update
    OpenLocalFS();
    r3dOutToLog("current build version=%d, updater data version=%d\n", fswork_.GetBuildVersion(), updateData_.version_);
    if(fswork_.GetBuildVersion() == updateData_.version_)
    {
      // ready to play
      WaitReadyToPlay();
      return;
    }
    
    // go for it
    SetRSUpdateStatus(1); // started update
    BuildUpdateList();
    SetRSUpdateStatus(2); // downloaded update data
    ProcessUpdates();
    SetRSUpdateStatus(3); // update complete
    CompressArchive();
    ValidateArchive();

    WaitReadyToPlay();
    return;
  }
  catch(EResult res)
  {
    r3dOutToLog("Exception! EResult=%d\n", res);
    r3d_assert(updErr1_[0]);
    result_ = res;
    return;
  }
  catch(char* err)
  {
    r3dOutToLog("Exception! err=%s\n", err);
    sprintf(updErr1_, "%s", err);
    result_ = RES_ERROR;
    return;
  }
  catch(...)
  {
    MessageBox(NULL, "something wrong happened", "", MB_OK);
  }

  return;
}

static unsigned int __stdcall CUpdater_MainThreadEntry(LPVOID in)
{
  CUpdater* impl = (CUpdater*)in;
  impl->MainThreadEntry();
  return 0;
}

void CUpdater::ParseNewsNode(pugi::xml_node xmlNode, std::vector<news_s>& news)
{
  news.clear();

  xmlNode = xmlNode.first_child();
  while(!xmlNode.empty())
  {
    news_s n;
    n.name_ = xmlNode.attribute("name").value();
    n.date_ = xmlNode.attribute("date").value();
    n.url_  = xmlNode.attribute("url").value();
    news.push_back(n);

    xmlNode = xmlNode.next_sibling();
  }
  
  return;
}

int CUpdater::GetNews()
{
  CkByteData xmlData;
  HttpDownload http;
  if(!http.Get(GETSERVERINFO_URL, xmlData)) {
    return 0;
  }
    
  pugi::xml_document xmlFile;
  pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace((void*)xmlData.getBytes(), xmlData.getSize());
  if(!parseResult) {
    return 0;
  }
    
  pugi::xml_node xmlInfo = xmlFile.child("info");
  if(xmlInfo.empty()) {
    return 0;
  }

  ::EnterCriticalSection(&csNews_);
    
  ParseNewsNode(xmlInfo.child("news"), newsData_);

  pugi::xml_node xmlNode;
  xmlNode = xmlInfo.child("ServerInfo");
  if(!xmlNode.empty()) {
    const char* status = xmlNode.attribute("status").value();
    serverStatus_ = status;
  }

  static bool gotSurveyLinks = false;
  if(!gotSurveyLinks) 
  {
    xmlNode = xmlInfo.child("SurveyIn");
    if(!xmlNode.empty()) {
      const char* link = xmlNode.attribute("link").value();
      surveyLinkIn_ = link;
    }

    xmlNode = xmlInfo.child("SurveyOut");
    if(!xmlNode.empty()) {
      const char* link = xmlNode.attribute("link").value();
      surveyLinkOut_ = link;
    }

    FilterSurveyLinks();
    gotSurveyLinks = true;
  }

  ::LeaveCriticalSection(&csNews_);
  
  return 1;
}

void CUpdater::NewsThreadEntry()
{
  try
  {
    // infinite loop, updating servers info
    while(true)
    {
      // do not update news while updater ir updating
      if(!updaterVersionOk_) {
        ::Sleep(500);
        continue;
      }
    
      if(GetNews()) {
        newsStatus_ = 2;
      } else {
        newsStatus_ = 1;
      }
    
      ::Sleep(NEWS_UPDATE_PERIOD * 1000);
    }

    return;
  }
  catch(char* err)
  {
    sprintf(updErr1_, "%s", err);
    result_ = RES_ERROR;
    return;
  }
  catch(...)
  {
    MessageBox(NULL, "something wrong happened", "", MB_OK);
  }

  return;
}

void CUpdater::FilterSurveyLinks()
{
  static const char* surveyFile = "./survey.info";
  char link1[512] = "";
  char link2[512] = "";

  FILE* surveyF = fopen(surveyFile, "rt");
  if(surveyF) {
    fgets(link1, 511, surveyF);
    fgets(link2, 511, surveyF);
    if(strlen(link1) > 1) link1[strlen(link1)-1] = 0;
    if(strlen(link2) > 1) link2[strlen(link2)-1] = 0;
    fclose(surveyF);
  }

  surveyF = fopen(surveyFile, "wt");
  if(!surveyF) {
    // prevent survey spamming if file write failed
    surveyLinkIn_  = "";
    surveyLinkOut_ = "";
    return;
  }
  fprintf(surveyF, "%s\n", surveyLinkIn_.c_str());
  fprintf(surveyF, "%s\n", surveyLinkOut_.c_str());
  fclose(surveyF);
    
  // filter out links for current run
  if(strcmp(surveyLinkIn_.c_str(), link1) == 0)
    surveyLinkIn_ = "";
  if(strcmp(surveyLinkOut_.c_str(), link2) == 0)
    surveyLinkOut_ = "";
}

static unsigned int __stdcall CUpdater_NewsThreadEntry(LPVOID in)
{
  CUpdater* impl = (CUpdater*)in;
  impl->NewsThreadEntry();
  return 0;
}

static unsigned int __stdcall CUpdater_DownloadThreadEntry(LPVOID in)
{
  CUpdater* impl = (CUpdater*) in;
  impl->DownloadThreadEntry();
  return 0;
}

static unsigned int __stdcall CUpdater_LoginThreadEntry(LPVOID in)
{
  CUpdater* impl = (CUpdater*)in;
  impl->LoginThreadEntry();
  return 0;
}

void CUpdater::LoginThreadEntry()
{
  try
  {
    sprintf(updMsg1_, "Logging in");
    gUserProfile.DoLogin();
    sprintf(updMsg1_, "");
    
    ProcessLoginResults();
  }
  catch(char* err)
  {
    sprintf(updErr1_, "%s", err);
    result_ = RES_ERROR;
  }
  catch(...)
  {
    MessageBox(NULL, "something wrong happened", "", MB_OK);
  }
  
  return;
}

void CUpdater::ProcessLoginResults()
{
    switch(gUserProfile.loginAnswerCode) 
    {
      default:
      case CLoginHelper::ANS_Timeout:
      case CLoginHelper::ANS_Error:
        loginErrMsg_ = "Login failed, please try again later";
        break;

      case CLoginHelper::ANS_BadPassword:
        loginErrMsg_ = "Invalid username or password";
        break;
        
      case CLoginHelper::ANS_GameActive:
        loginErrMsg_ = "Game session active, try later";
        break;

      case CLoginHelper::ANS_Frozen:
      {
        loginErrMsg_ = "Your account has been temporarily frozen";
        
        // SessionID holds ban time
        int hours = (gUserProfile.SessionID / 60) + 1;
        char buf[512];
        sprintf(buf, "Your account has been temporarily frozen because of violation of the Terms of Service ( Paragraph 2 )\n\nYou will be able to continue to use the service in %d hours", hours);
        MessageBox(NULL, buf, "Login", MB_OK | MB_ICONEXCLAMATION);
        break;
      }

      case CLoginHelper::ANS_TooManyAttemps:
      {
        loginErrMsg_ = "Too many failed attempts";
        MessageBox(NULL, "Too many failed login attempts, your IP has been blocked for one hour", "Login", MB_OK | MB_ICONEXCLAMATION);
        break;
      }

      case CLoginHelper::ANS_SecurityLock:
      {
        loginErrMsg_ = "Your account is locked";
        MessageBox(NULL, "Account was locked due to suspicious activity.\n\nPlease check your email for instructions on how to unlock it or you can reset password, that will also unlock your account.", "Login", MB_OK | MB_ICONEXCLAMATION);
        break;
      }

      case CLoginHelper::ANS_Banned:
      {
        loginErrMsg_ = "Your account has been permanently banned";
	int res = MessageBox(NULL, "Banned: This account has been investigated and permanently closed for violations of the Terms of Use", "Login", MB_OK | MB_ICONSTOP);
        if(res == IDOK)
	{
	  // THAI ShellExecute(NULL, "open", "http://www.infestationmmo.com/tos.html", "", NULL, SW_SHOW);
	  // THAI ShellExecute(NULL, "open", "http://forums.infestationmmo.com/index.php?/topic/133636-regarding-bans-for-cheating/", "", NULL, SW_SHOW);
	}
        break;
      }
        
      case CLoginHelper::ANS_TimeExpired:
        loginErrMsg_ = "Your guest game play time is expired";
        status_ = STATUS_TimeExpired;
        break;

      case CLoginHelper::ANS_Deleted:
        loginErrMsg_ = "ACCOUNT DELETED";
        MessageBox(NULL, "Your account was deleted because your payment was refunded or cancelled\n\nPlease contact your payment provider", "Account Deleted", MB_OK | MB_ICONEXCLAMATION);
        break;
        
      case CLoginHelper::ANS_SteamDoNotOwnGame:
        loginErrMsg_ = "";
        MessageBox(NULL, "You've started game under a steam account, but that account does not have The War Z game ownership", "No game ownership on Steam", MB_OK | MB_ICONEXCLAMATION);
        break;
        
      case CLoginHelper::ANS_Unknown:
        loginErrMsg_ = "Unknown status";
        MessageBox(NULL, "Unknown account status, please contact support@extreme.co.th", "Login", MB_OK | MB_ICONEXCLAMATION);
        break;

      case CLoginHelper::ANS_Logged:
        loginErrMsg_ = "";
        // login ok, continue to updating
        status_ = STATUS_Updating;
        break;
    }
}

void CUpdater::DoLogin()
{
  gUserProfile.StartLogin();  

  // create login thread
  loginThread_ = (HANDLE)_beginthreadex(
    NULL,
    0,
    &CUpdater_LoginThreadEntry,
    (void *)this,		// argument
    0,				// creation flags
    0			// thread ID
  );
}

static unsigned int __stdcall CUpdater_CheckSerialThreadEntry(LPVOID in)
{
  CUpdater* impl = (CUpdater*)in;
  impl->CheckSerialThreadEntry();
  return 0;
}

void CUpdater::CheckSerialThreadEntry()
{
  try
  {
    sprintf(updMsg1_, "Please wait");
    
    if(!checkSerialHelper.DoCheckSerial())
    {
      sprintf(updMsg1_, "");
      MessageBox(NULL, "Serial Key check failed, please try again later", "Serial Key Check", MB_OK);
      return;
    }

    sprintf(updMsg1_, "");
    switch(checkSerialHelper.CheckCode)
    {
      case 0:
        // check of,
        status_ = STATUS_NeedRegister;
        break;
        
      default:
        MessageBox(NULL, checkSerialHelper.CheckMsg, "Serial Key Check", MB_OK);
        break;
    }
  }
  catch(char* err)
  {
    sprintf(updErr1_, "%s", err);
    result_ = RES_ERROR;
  }
  catch(...)
  {
    MessageBox(NULL, "something wrong happened", "", MB_OK);
  }
  
  return;
}

void CUpdater::DoCheckSerial()
{
  if(strlen(checkSerialHelper.serial) != 19 && strlen(checkSerialHelper.serial) != 29) {
    MessageBox(NULL, "Please enter Serial Key in following format:\nXXXX-XXXX-XXXX-XXXX or XXXXX-XXXXX-XXXXX-XXXXX-XXXXX", "Create Account", MB_OK);
    return;
  }

  // create login thread
  loginThread_ = (HANDLE)_beginthreadex(
    NULL,
    0,
    &CUpdater_CheckSerialThreadEntry,
    (void *)this,		// argument
    0,				// creation flags
    NULL			// thread ID
  );
}

static unsigned int __stdcall CUpdater_CreateAccThreadEntry(LPVOID in)
{
  CUpdater* impl = (CUpdater*)in;
  impl->CreateAccThreadEntry();
  return 0;
}

// from Secure Programming Cookbook for C and C++ , Recipe 3.9: Validating Email Addresses
int spc_email_isvalid(const char *address) 
{
  int        count = 0;
  const char *c, *domain;
  static char *rfc822_specials = "()<>@,;:\\\"[]";

  /* first we validate the name portion (name@domain) */
  for (c = address;  *c;  c++) {
    if (*c == '\"' && (c == address || *(c - 1) == '.' || *(c - 1) == 
        '\"')) {
      while (*++c) {
        if (*c == '\"') break;
        if (*c == '\\' && (*++c == ' ')) continue;
        if (*c <= ' ' || *c >= 127) return 0;
      }
      if (!*c++) return 0;
      if (*c == '@') break;
      if (*c != '.') return 0;
      continue;
    }
    if (*c == '@') break;
    if (*c <= ' ' || *c >= 127) return 0;
    if (strchr(rfc822_specials, *c)) return 0;
  }
  if (c == address || *(c - 1) == '.') return 0;

  /* next we validate the domain portion (name@domain) */
  if (!*(domain = ++c)) return 0;
  do {
    if (*c == '.') {
      if (c == domain || *(c - 1) == '.') return 0;
      count++;
    }
    if (*c <= ' ' || *c >= 127) return 0;
    if (strchr(rfc822_specials, *c)) return 0;
  } while (*++c);

  return (count >= 1);
}

void CUpdater::CreateAccThreadEntry()
{
  try
  {
    sprintf(updMsg1_, "Creating Account, please wait");
    int code = createAccHelper.DoCreateAcc();
    sprintf(updMsg1_, "");

    switch(code)
    {
      default:
        MessageBox(NULL, "Account creation failed, please try again later", "Create Account", MB_OK);
        break;
        
      case 0:
        // if we created steam account, login with it
        if(gSteam.steamID > 0)
        {
          status_ = STATUS_Checking; // switch to first screen for steam login process
          status_ = GetSteamLinkedStatus();
          sprintf(updMsg1_, "");
          break;
        }
        
        // account creating success, perform usual login with username/pwd
        sprintf(updMsg1_, "Logging in");
        r3dscpy(gUserProfile.username, createAccHelper.username);
        r3dscpy(gUserProfile.passwd, createAccHelper.passwd1);
        gUserProfile.DoLogin();
        sprintf(updMsg1_, "");
        if(gUserProfile.loginAnswerCode == CLoginHelper::ANS_Logged)
        {
          // login ok, continue to updating
          status_ = STATUS_Updating;
        }
        else
        {
          // login failed somehow
          status_ = STATUS_NeedLogin;
        }
        break;
        
      // codes from WZ_ACCOUNT_CREATE
      case 2:
        MessageBox(NULL, "There is already registered account with that email!\nPlease note that you must use unique email per The War Z account", "Create Account", MB_OK);
        break;

      case 3:
        MessageBox(NULL, "Serial Key is not valid after Serial Key Check\ncontact support@extreme.co.th", "Create Account", MB_OK);
        break;
        
      case 4:
        MessageBox(NULL, "Guest Serial Keys is not active yet", "Create Account", MB_OK);
        break;
        
      // from api_AccRegister.aspx
      case 7:
        MessageBox(NULL, "You already have created The War Z account linked with this Steam account", "Create Account", MB_OK | MB_ICONSTOP);
        break;

      case 0xB:
        MessageBox(NULL, "Please use your existing The War Z account to play game under Steam", "Converted Account", MB_OK | MB_ICONSTOP);
        break;
    }
  }
  catch(char* err)
  {
    sprintf(updErr1_, "%s", err);
    result_ = RES_ERROR;
  }
  catch(...)
  {
    MessageBox(NULL, "something wrong happened", "", MB_OK);
  }
  
  return;
}

void CUpdater::DoCreateAccount()
{
  if(strlen(createAccHelper.username) < 4) {
    createAccHelper.createAccCode = CCreateAccHelper::CA_Unactive;
    MessageBox(NULL, "email address is too short", "Create Account", MB_OK);
    return;
  }
  if(spc_email_isvalid(createAccHelper.username) == 0) {
    createAccHelper.createAccCode = CCreateAccHelper::CA_Unactive;
    MessageBox(NULL, "email address is not valid", "Create Account", MB_OK);
    return;
  }
  if(strcmp(createAccHelper.passwd1, createAccHelper.passwd2) != 0) {
    createAccHelper.createAccCode = CCreateAccHelper::CA_Unactive;
    MessageBox(NULL, "Passwords do not match", "Create Account", MB_OK);
    return;
  }
      
  // create login thread
  loginThread_ = (HANDLE)_beginthreadex(
    NULL,
    0,
    &CUpdater_CreateAccThreadEntry,
    (void *)this,		// argument
    0,				// creation flags
    NULL			// thread ID
  );
}

void CUpdater::DoApplyNewKey(const char* key)
{
  if(strlen(key) != 19 && strlen(key) != 29) {
    MessageBox(NULL, "Please enter Serial Key in following format:\nXXXX-XXXX-XXXX-XXXX or XXXXX-XXXXX-XXXXX-XXXXX-XXXXX", "Create Account", MB_OK);
    return;
  }

  // not threaded right now.
  CWOBackendReq req("api_AccApplyKey.aspx");
  req.AddSessionInfo(gUserProfile.CustomerID, 0);
  req.AddParam("serial", key);
  req.Issue();
	
  switch(req.resultCode_)
  {
    case 0:
      // back to login
      status_      = STATUS_NeedLogin;
      loginErrMsg_ = "";
      updMsg1_[0]  = 0;
      break;

    // return codes from WZ_ACCOUNT_APPLYKEY
    case 2:
      MessageBox(NULL, "Your account type can not be updated, please contact support@extreme.co.th", "Update Account", MB_OK);
      break;
			
    case 3:
      MessageBox(NULL, "Serial Key is not valid", "Update Account", MB_OK);
      break;

    case 4:
      MessageBox(NULL, "Serial Key is already used", "Update Account", MB_OK);
      break;
		
    default:
      MessageBox(NULL, "Check failed, please try again later", "Update Account", MB_OK | MB_ICONERROR);
      break;
  }
}

void CUpdater::Start()
{
  r3d_assert(mainThread_ == NULL);
  r3d_assert(loginThread_ == NULL);

  // create main thread
  mainThread_ = (HANDLE)_beginthreadex(
    NULL,
    0,
    &CUpdater_MainThreadEntry,
    (void *)this,		// argument
    0,				// creation flags
    NULL			// thread ID
  );
  
  // news thread
  newsThread_ = (HANDLE)_beginthreadex(
    NULL,
    0,
    &CUpdater_NewsThreadEntry,
    (void *)this,		// argument
    0,				// creation flags
    NULL			// thread ID
  );
  
  // download threads
  for(int i=0; i<NUM_DOWNLOAD_THREADS; i++)
  {
    downloadThreads_[i] = (HANDLE)_beginthreadex(
      NULL,
      0,
      &CUpdater_DownloadThreadEntry,
      (void *)this,		// argument
      0,			// creation flags
      NULL			// thread ID
    );
  }

  return;
}

void CUpdater::StopThread(HANDLE& h)
{
  if(h != NULL)
  {
    ::TerminateThread(h, 0);
    ::CloseHandle(h);
    h = NULL;
  }
}

void CUpdater::RequestStop()
{
  r3dCSHolder csHolder1(csJobs_);
  r3dOutToLog("Requested to stop, %d files in queue\n", updateJobs_.size());
  result_ = RES_STOPPED;
}

void CUpdater::Stop()
{
  r3dOutToLog("CUpdater::Stop()\n");

  if(!updateOk_)
  {
    if(dlThreadError_)
      SetRSUpdateStatus(20, true); // error happened
    else if(numUpdatedFiles_ == 0)
      SetRSUpdateStatus(11, true); // didn't updated any file
    else
      SetRSUpdateStatus(10); // just aborted in middle
  }

  // wait for any archive operations
  extern CRITICAL_SECTION g_FileSysCritSection;
  r3dCSHolder csHolder1(g_FileSysCritSection);
  
  // terminate thread only when header write operation not in progress
  r3dCSHolder csHolder2(csDataWrite_);

  StopThread(mainThread_);
  StopThread(newsThread_);
  StopThread(loginThread_);
  
  for(int i=0; i<NUM_DOWNLOAD_THREADS; i++) 
  {
    StopThread(downloadThreads_[i]);
  }

  return;
}
