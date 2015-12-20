#include "r3dPCH.h"
#include "r3d.h"
#include "FileSystem/r3dFSCompress.h"

#include "shellapi.h"
#pragma comment(lib, "shell32.lib")

#include "SelfUpdateEXE.h"

	bool	g_IsExeUpdated = false;

const char* getAdminShellExecuteVerb()
{
  const char* verb = "open";
  
  OSVERSIONINFO osvi = {0};
  osvi.dwOSVersionInfoSize = sizeof(osvi);
  GetVersionEx(&osvi);
  r3dOutToLog("OS Version: %d %d %s\n", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.szCSDVersion);
  if(osvi.dwMajorVersion >= 6) {
    verb = "runas";
  }
  
  return verb;
}


bool selfUpd_TestIfUpdated()
{
  char exeMain[MAX_PATH];
  char exeUpd[MAX_PATH];
  GetModuleFileName(NULL, exeMain, sizeof(exeMain));

  if(strstr(exeMain, ".new.exe") == NULL)
  {
    // try to delete new updater file (it was left from previous run)
    sprintf(exeUpd, "%s.new.exe", exeMain);
    DeleteFile(exeUpd);
    return false;
  }
    
  g_IsExeUpdated = true;
    
  // move new file back to original (current exe must have .new.exe suffix)
  r3dOutToLog(".exe updated, moving back\n");
  ::Sleep(500);

  strcpy(exeUpd, exeMain);
  r3d_assert(strlen(exeMain) > 8);
  exeMain[strlen(exeMain)-8] = 0;
  r3dOutToLog("exeMain: %s\n", exeMain);
    
  bool movesuccess = false;
  for(int i=0; i<10; i++) {
    ::DeleteFile(exeMain);
    if(::MoveFile(exeUpd, exeMain) != 0) {
      movesuccess = true;
      break;
    }
    // try to copy.
    if(::CopyFile(exeUpd, exeMain, FALSE) != 0) {
      movesuccess = true;
      break;
    }
    ::Sleep(100);
  }

  if(!movesuccess) 
  {
    char msg[1024];
    sprintf(msg, 
      "Failed to replace updater file\n\n"\
      "Please manually rename %s to %s", 
      exeUpd, exeMain);
      
    MessageBox(NULL, msg, "Updater", MB_OK | MB_ICONEXCLAMATION);
    // open current directory
    ShellExecute(NULL, "open", ".", NULL, NULL, SW_SHOW);
    TerminateProcess(GetCurrentProcess(), 0);
    return false;
  }
  
  return true;
}

bool selfUpd_StartUpdate(const BYTE* data, long size)
{
  char exeMain[MAX_PATH];
  char exeUpd[MAX_PATH];
  GetModuleFileName(NULL, exeMain, sizeof(exeMain));

  r3dOutToLog("Trying to update\n");
  sprintf(exeUpd, "%s.new.exe", exeMain);
  FILE* f = fopen(exeUpd, "wb");
  if(f == NULL) {
    r3dError("can't open %s for writing\n", exeUpd);
  }
  fwrite(data, size, 1, f);
  fclose(f);

  // close handle used to detect if updater is already running
  extern HANDLE g_updaterEvt;
  CloseHandle(g_updaterEvt);

  r3dOutToLog("executing %s\n", exeUpd);
  
  const char* verb = "open"; // no need to run elevated, we checked for write permission before
  int err = (int)::ShellExecute(NULL, verb, exeUpd, __r3dCmdLine, NULL, SW_SHOW);
  if(err < 32) {
    r3dError("failed to launch %s, %d", exeUpd, err);
  }
      
  TerminateProcess(GetCurrentProcess(), 0);
  return true;
}