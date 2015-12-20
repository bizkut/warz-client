#include "r3dPCH.h"
#include "r3d.h"
#include "Win32Input.h"
#include "Updater.h"

void cmdLine_Init()
{
  AllocConsole();
  freopen("CONOUT$", "wb", stdout);
  freopen("CONIN$", "r", stdin);

  extern int _r3d_bLogToConsole;
  _r3d_bLogToConsole = 1;
}

void cmdLine_MainLoop(CUpdater& updater)
{
  updater.Start();
  
  r3dStartFrame();
  while(1)
  {
    r3dEndFrame();
    r3dStartFrame();
    ::Sleep(100);

    switch(updater.status_)
    {
      case CUpdater::STATUS_Checking:
	break;

      case CUpdater::STATUS_NeedLogin:
        updater.SwitchToUpdater();
        break;
    
      case CUpdater::STATUS_NeedRegister:
	break;

      case CUpdater::STATUS_Updating:
	break;
    }

    if(updater.result_ != CUpdater::RES_UPDATING)
      break;
      
    char title[512];
    sprintf(title, "%s: %.1f%%", updater.updMsg1_, updater.prgTotal_.getCoef() * 100.0f);
    SetConsoleTitle(title);
  }
  
  updater.Stop();

  switch(updater.result_)
  {
    default: r3d_assert(0); 
    case CUpdater::RES_STOPPED:
      break;

    case CUpdater::RES_PLAY:
      r3dOutToLog("Update ok\n");
      break;
      
    case CUpdater::RES_ERROR:
      r3dOutToLog("Update error\n'%s'\n\nPlease retry later", updater.updErr1_);
      break;
  }
  
  r3dOutToLog("\nPress enter to exit...");
  std::getchar();

  //TerminateProcess(GetCurrentProcess(), 0);
  
  return;
}

